/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/TerrainSectorRenderer.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


VTerrainSectorRenderLoop::VTerrainSectorRenderLoop(int iResX, int iResY)
{
  m_pSector = NULL;
  m_iResX = iResX;
  m_iResY = iResY;
  m_pTargetData = NULL;
#if ( defined(_VR_DX9) && defined(WIN32) )
  pRenderTarget = NULL;
  pTexture = NULL;
  pSurface = NULL;
  pLockableTexture = NULL;
  pLockableSurface = NULL;
#endif
}

VTerrainSectorRenderLoop::~VTerrainSectorRenderLoop()
{
#if ( defined(_VR_DX9) && defined(WIN32) )
  V_SAFE_RELEASE(pLockableSurface);
  V_SAFE_RELEASE(pLockableTexture);
  V_SAFE_RELEASE(pRenderTarget);
  V_SAFE_RELEASE(pSurface);
  V_SAFE_RELEASE(pTexture);
#endif
}

void VTerrainSectorRenderLoop::OnDoRenderLoop(void *pUserData)
{
  Vision::RenderLoopHelper.ClearScreen(VisRenderLoopHelper_cl::VCTF_All, V_RGBA_BLACK);

  const VTerrainConfig &config(m_pSector->m_Config);
  VTerrainSectorManager* pSectorManager = m_pSector->GetSectorManager();

  const int iCountX = (config.m_iSectorCount[0] == 1) ? 1 : (((m_pSector->m_iIndexX > 0) && (m_pSector->m_iIndexX < config.m_iSectorCount[0] - 1)) ? 3 : 2);
  const int iCountY = (config.m_iSectorCount[1] == 1) ? 1 : (((m_pSector->m_iIndexY > 0) && (m_pSector->m_iIndexY < config.m_iSectorCount[1] - 1)) ? 3 : 2);

  const int iBaseX = hkvMath::Max(0, m_pSector->m_iIndexX - 1);
  const int iBaseY = hkvMath::Max(0, m_pSector->m_iIndexY - 1);

  for (int iSx = 0; iSx < iCountX; ++iSx)
  {
    for (int iSy = 0; iSy < iCountY; ++iSy)
    {
      pSectorManager->GetSector(iBaseX + iSx, iBaseY + iSy)->Render(NULL, m_AllInst, 0, VPT_PrimaryOpaquePass);
    }
  }
}

VTerrainSectorRenderer::VTerrainSectorRenderer(VTerrain *pOwner, int iRes, int iBorderWidth)
{
  m_spOwner = pOwner;
  m_iBorderWidth = iBorderWidth;
  m_pSector = NULL;
  m_spContext = new VisRenderContext_cl();
  m_spContext->SetName("TerrainSectorRenderer");

  m_pRenderLoop = new VTerrainSectorRenderLoop(iRes, iRes);
  m_spContext->SetRenderLoop(m_pRenderLoop);
  m_pCamera = new VisContextCamera_cl();
  m_spContext->SetCamera(m_pCamera);
  m_spContext->SetProjectionType(VIS_PROJECTIONTYPE_ORTHOGRAPHIC);
  m_spContext->SetVisibilityCollector(new VisionVisibilityCollector_cl());
  
  VisRenderableTextureConfig_t config;
  config.m_iHeight = iRes;
  config.m_iWidth  = iRes;
  config.m_eFormat = VTextureLoader::DEFAULT_RENDERTARGET_FORMAT_32BPP;
  m_spTargetTex = Vision::TextureManager.CreateRenderableTexture("<sector>", config);
  m_spTargetTex->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
  m_spContext->SetRenderTarget(0,m_spTargetTex);

  config.m_eFormat = VVideo::GetSupportedDepthStencilFormat(VTextureLoader::D24S8, *Vision::Video.GetCurrentConfig());
  config.m_bRenderTargetOnly = true;
  config.m_bIsDepthStencilTarget = true;
  m_spDepthStencilTex = Vision::TextureManager.CreateRenderableTexture("<sector_ds>", config);
  m_spDepthStencilTex->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
  m_spContext->SetDepthStencilTarget(m_spDepthStencilTex);

  m_spTargetData = new VisBitmap_cl(NULL, iRes, iRes);
}

VTerrainSectorRenderer::~VTerrainSectorRenderer()
{

}

void VTerrainSectorRenderer::SetSector(VTerrainSector *pSector)
{
  const VTerrainConfig &config(pSector->m_Config);
  m_pSector = pSector;
  
  const int iSx = m_pSector->m_iIndexX;
  const int iSy = m_pSector->m_iIndexY;

  const int iCountX = (config.m_iSectorCount[0] == 1) ? 1 : (((iSx > 0) && (iSx < config.m_iSectorCount[0] - 1)) ? 3 : 2);
  const int iCountY = (config.m_iSectorCount[1] == 1) ? 1 : (((iSy > 0) && (iSy < config.m_iSectorCount[1] - 1)) ? 3 : 2);

  m_spOwner->EnsureSectorRangeLoaded(hkvMath::Max(0, iSx-1), hkvMath::Max(0, iSy-1), iCountX, iCountY);
  
  m_pRenderLoop->SetSector(m_pSector);
  m_pRenderLoop->m_pTargetData = m_spTargetData; // save next time...

  float fDistance = 0.0f;

  const int iBaseX = hkvMath::Max(0, m_pSector->m_iIndexX - 1);
  const int iBaseY = hkvMath::Max(0, m_pSector->m_iIndexY - 1);

  for (int x = 0; x < iCountX; ++x)
  {
    for (int y = 0; y < iCountY; ++y)
    {
      VTerrainSector* pCurrentSector = m_pSector->GetSectorManager()->GetSector(iBaseX + x, iBaseY + y);
      fDistance = hkvMath::Max(fDistance, hkvMath::Max(config.m_vTerrainPos.z, pCurrentSector->m_fMaxHeightValue) -
        hkvMath::Min(config.m_vTerrainPos.z, pCurrentSector->m_fMinHeightValue));
    }
  }

  // In case the terrain's sectors all have min/max height values of zero, a default distance of 100.0f is assumed.
  if (fDistance <= 0.0f)
    fDistance = 100.0f;

  hkvVec3 vCenter = m_pSector->GetSectorOrigin() + config.m_vSectorSize.getAsVec3(0.0f)*0.5f;
  vCenter.z = config.m_vTerrainPos.z + fDistance;

  hkvMat3 cameraMatrix (hkvNoInitialization);
  cameraMatrix.setLookInDirectionMatrix (hkvVec3(0, 0, -1), hkvVec3(0, 1, 0));

  m_pCamera->Set(cameraMatrix,vCenter);

  const float fWidth = (float)(m_spTargetTex->GetTextureWidth());
  const float fBorderWidth = ((config.m_vSectorSize.x * fWidth) / (fWidth - m_iBorderWidth*2));

  const float fHeigth = (float)(m_spTargetTex->GetTextureHeight());
  const float fBorderHeigth = ((config.m_vSectorSize.y * fHeigth) / (fHeigth - m_iBorderWidth*2));

  m_spContext->GetViewProperties()->setOrthographicSize(fBorderWidth, fBorderHeigth);

  

  const float fNear = 0.0f;
  const float fFar  = config.m_vTerrainPos.z + fDistance*2.0f;
  m_spContext->GetViewProperties()->setClipPlanes(fNear, fFar);

  m_pCamera->ReComputeVisibility();
}

float LinearToSrgb(float val)
{
  float ret;
  if (val <= 0.0)
    ret = 0.0f;
  else if (val <= 0.0031308f)
    ret = 12.92f*val;
  else if (val <= 1.0f)
    ret = (hkvMath::pow(val, 0.41666) * 1.055f) - 0.055f;
  else
    ret = 1.0f;
  return ret;
}

static void FillBorders(VTerrainSector* pSector, int sx, int sy, int iBorderWidth, UBYTE* pDest)
{
  const VTerrainConfig &config(pSector->m_Config);
  const int iIndexX = pSector->m_iIndexX;
  const int iIndexY = pSector->m_iIndexY;
  int iStride = sx*3;

  UBYTE color[3];
  size_t size = 3*sizeof(UBYTE);

  // Left border
  if (iIndexX == 0)
  {
    for (int i=iBorderWidth; i<(sy-iBorderWidth); ++i)
    {
      memcpy(color, &pDest[iStride*i + iBorderWidth*3], size);
      for (int j=0; j<iBorderWidth; ++j)
        memcpy(&pDest[iStride*i + j*3], color, size);
    }
  }

  // Right border
  if (iIndexX == (config.m_iSectorCount[0] - 1))
  {
    for (int i=iBorderWidth; i < (sy-iBorderWidth); ++i)
    {
      memcpy(color, &pDest[iStride*(i + 1) - 3 - iBorderWidth*3], size);
      for (int j=0; j<iBorderWidth; ++j)
        memcpy(&pDest[iStride*(i + 1) - 3 - j*3], color, size);
    }
  }

  // Bottom border
  if (iIndexY == 0)
  {
    for (int i=iBorderWidth; i<(sx-iBorderWidth); ++i)
    {
      int iBase = (sy - iBorderWidth - 1)*iStride;
      memcpy(color, &pDest[iBase + i*3 + 0], size);
      for (int j=0; j<iBorderWidth; ++j)
        memcpy(&pDest[iBase + i*3 + iStride*(j+1)], color, size);
    }
  }

  // Top border
  if (iIndexY == (config.m_iSectorCount[1] - 1))
  {
    for (int i=iBorderWidth; i<(sx-iBorderWidth); ++i)
    {
      memcpy(color, &pDest[i*3 + iStride*iBorderWidth], size);
      for (int j=0; j<iBorderWidth; ++j)
        memcpy(&pDest[i*3 + iStride*j], color, size);
    }
  }

  // Lower left corner
  if ((iIndexX == 0) || (iIndexY == 0))
  {
    memcpy(color, &pDest[(sy - iBorderWidth - 1)*iStride + iBorderWidth*3], size);
    for (int i=0; i<iBorderWidth; ++i)
      for (int j=0; j<iBorderWidth; ++j)
        memcpy(&pDest[(sy - iBorderWidth)*iStride + i*iStride + j*3], color, size);
  }

  // Upper left corner
  if ((iIndexX == 0) || (iIndexY == (config.m_iSectorCount[1] - 1)))
  {
    memcpy(color, &pDest[iBorderWidth*iStride + iBorderWidth*3], size);
    for (int i=0; i<iBorderWidth; ++i)
      for (int j=0; j<iBorderWidth; ++j)
        memcpy(&pDest[i*iStride + j*3], color, size);
  }

  // Lower right corner
  if ((iIndexX == (config.m_iSectorCount[0] - 1)) || (iIndexY == 0))
  {
    memcpy(color, &pDest[(sy - iBorderWidth - 1)*iStride + (sx - iBorderWidth - 1)*3], size);

    int iBase = (sy - iBorderWidth)*iStride + (sx - iBorderWidth)*3;
    for (int i=0; i<iBorderWidth; ++i)
      for (int j=0; j<iBorderWidth; ++j)
        memcpy(&pDest[iBase + i*iStride + j*3], color, size);
  }

  // Upper right corner
  if ((iIndexX == (config.m_iSectorCount[0] - 1)) || (iIndexY == (config.m_iSectorCount[1] - 1)))
  {
    memcpy(color, &pDest[(iBorderWidth + 1)*iStride - (iBorderWidth + 1)*3], size);

    int iBase = (sx - iBorderWidth)*3;
    for (int i=0; i<iBorderWidth; ++i)
      for (int j=0; j<iBorderWidth; ++j)
        memcpy(&pDest[iBase + i*iStride + j*3], color, size);
  }
}

bool VTerrainSectorRenderer::SaveRenderTarget(VTerrainSector* pSector, VisRenderableTexture_cl *pTex, const char *szFilePath)
{
  VASSERT(pSector);

  // since the functions below require an absolute path, we somehow need to get from a relative path to an absolute path
  // opening a file and getting its then final path is the only way, that I found, to accomplish this
  char szAbsFilename[FS_MAX_PATH];

  // create the file and convert the relative path into an absolute path
  IVFileOutStream* pStream = Vision::File.Create (szFilePath);
  if (!pStream)
    return false;

  strcpy (szAbsFilename, pStream->GetFileName());
  pStream->Close();

  VFileAccessManager::RCSEditFile(szAbsFilename);

  if (pTex==NULL)
    return false;

  int sx = pTex->GetTextureWidth();
  int sy = pTex->GetTextureHeight();

  Image_cl image;
  VMemoryTempBuffer<512*512*3> buffer(sx*sy*3);
  UBYTE* pDest = (UBYTE*)buffer.GetBuffer();
  Vision::Game.WriteScreenToBuffer(0, 0, sx, sy, pDest, pTex);
  UBYTE szLine[4];
  // flip vertically:
  int iStride = sx*3;
  for (int y=0; y<sy/2; y++)
  {
    UBYTE *l1 = &pDest[y*iStride];
    UBYTE *l2 = &pDest[(sy-1-y)*iStride];
    for (int x=0; x<sx; x++, l1+=3, l2+=3)
    {
      szLine[0] = l1[0]; 
      szLine[1] = l1[1]; 
      szLine[2] = l1[2]; 
      
      l1[0] = l2[2]; 
      l1[1] = l2[1]; 
      l1[2] = l2[0]; // flip components

      l2[0] = szLine[2]; 
      l2[1] = szLine[1]; 
      l2[2] = szLine[0]; // flip components        
    }
  }

  if(Vision::Renderer.GetSRGBMode() == V_SRGB_ASSUME_FOR_DIFFUSE)
  {
    for (int i = 0; i < sx*sy*3; ++i)
      pDest[i] = (UBYTE) (LinearToSrgb (((float) pDest[i]) / 255.0f) * 255.0f);
  }

  // The terrain sector renderer leaves a border area around the sector being rendered,
  // this is required to ensure proper texture filtering on sector edges. Into this border area
  // the neighbor sectors are rendered to, but not each sector has a neighbor on each side, thus the next
  // step fills all the pixels where nothing has been rendered to.
  FillBorders(pSector, sx, sy, m_iBorderWidth, pDest);

  image.AddColorMap(sx,sy,COLORDEPTH_24BPP,pDest);

  int iSavingResult = -1;

  if (VFileHelper::HasExtension(szAbsFilename,"BMP"))
    iSavingResult = image.SaveBMP(szAbsFilename);
  else if (VFileHelper::HasExtension(szAbsFilename,"JPG"))
    iSavingResult = image.SaveJPEG(szAbsFilename);
  else if (VFileHelper::HasExtension(szAbsFilename,"JPEG"))
    iSavingResult = image.SaveJPEG(szAbsFilename);
  else if (VFileHelper::HasExtension(szAbsFilename,"TGA"))
    iSavingResult = image.SaveTGA(szAbsFilename);
  else if (VFileHelper::HasExtension(szAbsFilename,"DDS"))
    iSavingResult = image.SaveUncompressedDDS(szAbsFilename);

  VFileAccessManager::RCSAddFile(szAbsFilename, true /* Binary file */);

  return iSavingResult == 0;
}


bool VTerrainSectorRenderer::SaveToFile(VTerrainSector *pSector)
{
  if (pSector)
    SetSector(pSector);
  if (!m_pSector)
    return false;

  // render into the context, readback results and save to file
  Vision::Game.SetUpdateSceneCount(Vision::Game.GetUpdateSceneCount() + 1);
  m_spContext->GetCamera()->Update();
  IVisVisibilityCollector_cl *pVisColl = m_spContext->GetVisibilityCollector();
  pVisColl->SetPropertiesFromRenderContext(m_spContext);
  pVisColl->OnDoVisibilityDetermination(m_spContext->GetRenderFilterMask());

  m_spContext->SetRecentlyRendered(true);
  m_spContext->Activate();
  VisRenderContext_cl::GlobalTick();
    
  Vision::Renderer.BeginRendering();
  Vision::RenderSceneHelper();

  // get the snapshot from the render target and write it to disk
  char szRelativePathname[FS_MAX_PATH];
  if (pSector->m_Config.GetSectorDiffuseReplacementFilename (szRelativePathname, pSector->m_iIndexX, pSector->m_iIndexY))
  {
    VASSERT_MSG (m_spContext->GetRenderTarget(0)->IsRenderable(), "Render-Target must be a renderable texture object.");
    SaveRenderTarget(m_pSector, (VisRenderableTexture_cl*) m_spContext->GetRenderTarget(0), szRelativePathname);
  }

  Vision::Renderer.EndRendering();

  VisRenderContext_cl::GetMainRenderContext()->Activate();

  return true;
}

/*
 * Havok SDK - Base file, BUILD(#20130624)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available from salesteam@havok.com.
 * 
 */
