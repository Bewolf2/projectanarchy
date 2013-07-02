/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/VisionManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/LightmapTool/LightmapManager.hpp>
#include <Vision/Runtime/Engine/System/Vision.hpp>
#using <mscorlib.dll>
using namespace System;
using namespace CSharpFramework;
using namespace ManagedFramework;

LightmapSceneListener g_SceneListener;


void LightmapSceneListener::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender==&Vision::Callbacks.OnWorldInit)
  {
  }

  if (pData->m_pSender==&Vision::Callbacks.OnWorldDeInit)
  {
    LightmapManager::ReleaseLightmapInfo();
  }
}

void LightmapManager::OneTimeInit()
{
  Vision::Callbacks.OnWorldInit += g_SceneListener;
  Vision::Callbacks.OnWorldDeInit += g_SceneListener;
}

void LightmapManager::OneTimeDeInit()
{
  Vision::Callbacks.OnWorldInit -= g_SceneListener;
  Vision::Callbacks.OnWorldDeInit -= g_SceneListener;
}


#define CHECK_VALID(cond,errorstr) {if (!(cond)) {SetError(errorstr);return FALSE;}}

BOOL LightmapInfoLoader::OnStartChunk(CHUNKIDTYPE chunkID, int iChunkLen)
{
  if (chunkID=='HEAD')
  {
    int iVersion, iChecksum, iEquation, iPrims, iLights, iPages;
    ReadInt(iVersion);
    ReadInt(iChecksum);
    ReadInt(iEquation);
    ReadInt(iPrims);
    ReadInt(iLights);
    ReadInt(iPages);
    CHECK_VALID(iPages==LightmapManager::iPageCount,"Mismatching number of light pages");
    LightmapManager::eLightEquation = (VIS_CFG_LightingMode)(iEquation&7);
    switch (LightmapManager::eLightEquation)
    {
      case VIS_LIGHTING_MODULATE2X:
        LightmapManager::fColorMulFactor = 2.f;
        break;
      case VIS_LIGHTING_MODULATE4X:
        LightmapManager::fColorMulFactor = 4.f;
        break;
      default:
        LightmapManager::fColorMulFactor = 1.f;
        break;

    }
    return TRUE;
  }

  if (chunkID=='INFB' || chunkID=='INFR')
  {
    bool bRadiosity = chunkID=='INFR';
    if (bRadiosity)
      LightmapManager::bLightmapRadiosity = true;
    else
      LightmapManager::bLightmapBase = true;

    int iVersion, iPage;
    char iChannels;
    ReadInt(iVersion);
    CHECK_VALID(iVersion==0,"Unsupported version number");
    ReadInt(iPage);
    CHECK_VALID(iPage>=0 && iPage<LightmapManager::iPageCount,"Invalid page index");
    LightmapPageTweakInfo *pInfo = &LightmapManager::pPageInfo[iPage];
    short iSizeX, iSizeY;
    ReadShort(iSizeX);
    ReadShort(iSizeY);
    CHECK_VALID(iSizeX==pInfo->m_iMaxSizeX,"Invalid page size");
    CHECK_VALID(iSizeY==pInfo->m_iMaxSizeY,"Invalid page size");

    bool bReadTags;
    Readbool(bReadTags);
    if (bReadTags)
      Read(pInfo->m_pRefTag,iSizeX*iSizeY);

    // read all channels
    ReadChar(iChannels);
    CHECK_VALID(iChannels==1 || iChannels==4,"Invalid number of texture channels");
    for (int i=0;i<iChannels;i++)
    {
      ReadShort(iSizeX);
      ReadShort(iSizeY);
      int iTexels = (int)iSizeX * (int)iSizeY;
      CHECK_VALID(pInfo->m_pBaseColor[i]!=nullptr, "channel not referenced");
      CHECK_VALID(iSizeX==pInfo->GetPageSizeX(i),"Invalid channel page size");
      CHECK_VALID(iSizeY==pInfo->GetPageSizeY(i),"Invalid channel page size");
      Read(bRadiosity ? pInfo->m_pRadiosityColor[i] : pInfo->m_pBaseColor[i],iTexels*sizeof(VColorExpRef));
    }
    return TRUE;
  }
  return TRUE;
}


bool LightmapManager::MapLoaded()
{
  return true;
  /*
  const char *szMapName = Vision::World.GetCurrentWorldName();
  return (szMapName!=nullptr) && (szMapName[0]!=0);
  */
}

bool LightmapManager::LoadLightmapInfo(const char *szV3DName)
{
/* OUTDATED 6.X. code
  char szFilename[_MAX_PATH];
  // open tweak light info
  VFileHelper::AddExtension(szFilename,szV3DName,"lit.lightinfo");
  LightmapInfoLoader loader;
  if (!loader.Open(szFilename, Vision::File.GetManager()))
    return false;

  iPageCount = Vision::World.GetLightmapCount();
  if (!iPageCount)
    return false;
  pPageInfo = new LightmapPageTweakInfo[iPageCount];
  int iMaxTexels = 0;
  for (int i=0;i<iPageCount;i++)
  {
    for (int j=0;j<4;j++)
      pPageInfo[i].m_spLightmapChannel[j] = Vision::World.GetLightmapTexture(i,j);
    pPageInfo[i].AllocateData();
    int iTexels = pPageInfo[i].m_iMaxSizeX*pPageInfo[i].m_iMaxSizeY;
    if (iTexels>iMaxTexels) iMaxTexels = iTexels;
  }

  // parse the tweak info file
  loader.ParseFile();
  loader.Close();
  if (loader.IsInErrorState())
    return false;

  for (int i=0;i<iPageCount;i++)
    pPageInfo[i].FinishPage();

  // create buffer to temporarely mix pages
  pUploadBuffer = new UBYTE[iMaxTexels*3];
  memset(pUploadBuffer,0,iMaxTexels*3);
*/
  return true;
}

bool LightmapManager::LoadLightGridInfo(const char *szFilename)
{
  VLightGrid_cl *pGrid = Vision::RenderLoopHelper.GetLightGrid();
  if (!pLightGridInfo)
    pLightGridInfo = new LightGridTweakInfo();


  pLightGridInfo->ReleaseColors();

  VChunkFile loader;
  if (loader.Open(szFilename, Vision::File.GetManager()))
  {
    CHUNKIDTYPE iChunkID;
    while (loader.OpenChunk(&iChunkID))
    {
      if (iChunkID=='LGIB')
      {
        loader.ReadInt(pLightGridInfo->m_iFlatColorCount);
        pLightGridInfo->m_pBaseColors = new hkvVec3[pLightGridInfo->m_iFlatColorCount];
        loader.Read(pLightGridInfo->m_pBaseColors, pLightGridInfo->m_iFlatColorCount*sizeof(hkvVec3));
        bLightGridBase = pLightGridInfo->m_bHasBase=true;
      }
      if (iChunkID=='LGIR')
      {
        loader.ReadInt(pLightGridInfo->m_iFlatColorCount);
        pLightGridInfo->m_pRadiosityColors = new hkvVec3[pLightGridInfo->m_iFlatColorCount];
        loader.Read(pLightGridInfo->m_pRadiosityColors, pLightGridInfo->m_iFlatColorCount*sizeof(hkvVec3));
        bLightGridRadiosity = pLightGridInfo->m_bHasRadiosity=true;
      }
      loader.EndChunk();
    }

    loader.Close();
    if (pLightGridInfo->m_iFlatColorCount>0)
      pLightGridInfo->m_pMixResult = new hkvVec3[pLightGridInfo->m_iFlatColorCount];
  }
  else
  {
    // create a clone of the current light grid
    pLightGridInfo->FromLightGrid(pGrid);
  }

  return true;
}


bool LightmapManager::GetSceneLightGridInfo(bool bForceReload)
{
  if (bLightgridInfoValid && !bForceReload)
    return true;

  // reset everything
  bLightgridInfoValid = false;
  bLightGridBase      = false;
  bLightGridRadiosity = false;


  // load the lightgrid info
  VLightGrid_cl *pGrid = Vision::RenderLoopHelper.GetLightGrid();
  if (pGrid && EditorManager::Scene!=nullptr)
  {
    String ^tweakFile = System::IO::Path::Combine(EditorManager::Scene->LayerDirectoryName,"LightGridTweakData.dat");
    VString sTweakFile;
    ConversionUtils::StringToVString(tweakFile,sTweakFile);
    bLightgridInfoValid = LoadLightGridInfo(sTweakFile);
  }
  else
  {
    V_SAFE_DELETE(pLightGridInfo);
  }

  return bLightgridInfoValid;
}



bool LightmapManager::GetSceneLightmapInfo(bool bForceReload)
{
  return false;
  /*
  if (bLightmapInfoValid && !bForceReload)
    return true;

  // reset everything
  bLightmapInfoValid  = false;
  bLightmapBase       = false;
  bLightmapRadiosity  = false;

  const char *szV3DName = Vision::World.GetCurrentWorldName();
  if (!szV3DName || !szV3DName[0])
    return false;

  bLightmapInfoValid = LoadLightmapInfo(szV3DName);

  return bLightmapInfoValid;
  */
}



void LightmapManager::ReleaseLightmapInfo()
{
  V_SAFE_DELETE_ARRAY(pPageInfo);
  V_SAFE_DELETE_ARRAY(pUploadBuffer);
  iPageCount = 0;
  bLightgridInfoValid = false;
  bLightmapInfoValid  = false;
  bLightmapBase       = false;
  bLightmapRadiosity  = false;
  bLightGridBase      = false;
  bLightGridRadiosity = false;

  V_SAFE_DELETE(pLightGridInfo);
}


void LightmapManager::UpdateLightmaps(bool bImmediateViewUpdate)
{
  if (!bLightmapInfoValid)
    return;
  for (int i=0;i<iPageCount;i++)
    pPageInfo[i].MixAndUploadLightmaps(pUploadBuffer);
 
  EditorManager::ActiveView->UpdateView(bImmediateViewUpdate);
}

void LightmapManager::UpdateLightGrid(bool bImmediateViewUpdate)
{
  VLightGrid_cl *pGrid = Vision::RenderLoopHelper.GetLightGrid();
  if (!pLightGridInfo || !pGrid || !bLightgridInfoValid)
    return;

  pLightGridInfo->MixAndUpdate(pGrid);

  EditorManager::ActiveView->UpdateView(bImmediateViewUpdate);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

LightmapPageTweakInfo::LightmapPageTweakInfo()
{
  m_iMaxSizeX = m_iMaxSizeY = 0;
  for (int i=0;i<4;i++)
  {
    m_pBaseColor[i] = nullptr;
    m_pRadiosityColor[i] = nullptr;
  }
  m_pRefTag = nullptr;
}

LightmapPageTweakInfo::~LightmapPageTweakInfo()
{
  for (int i=0;i<4;i++)
  {
    V_SAFE_DELETE_ARRAY(m_pBaseColor[i]);
    V_SAFE_DELETE_ARRAY(m_pRadiosityColor[i]);
  }
  V_SAFE_DELETE_ARRAY(m_pRefTag);
}



void LightmapPageTweakInfo::FinishPage()
{
  for (int i=0;i<4;i++)
  {
    if (!m_spLightmapChannel[i])
      continue;
    VColorExpRef *pBase = m_pBaseColor[i];
    VColorExpRef *pRad = m_pRadiosityColor[i];
    int iSizeX = m_spLightmapChannel[i]->GetTextureWidth();
    int iSizeY = m_spLightmapChannel[i]->GetTextureHeight();
    int iTexel = iSizeX*iSizeY;
    hkvVec3 vBase,vRad;
    for (int j=0;j<iTexel;j++,pBase++,pRad++)
    {
      // create the diff of radiosity result and base color. This is the radiosity contribution
      pBase->ToFloat(vBase);
      pRad->ToFloat(vRad);
      vRad-=vBase;
      if (vRad.x<0.f) vRad.x=0.f;
      if (vRad.y<0.f) vRad.y=0.f;
      if (vRad.z<0.f) vRad.z=0.f;
      pRad->FromFloat(vRad);
    }
  }
}


void LightmapPageTweakInfo::AllocateData()
{
  for (int i=0;i<4;i++)
  {
    VASSERT(m_pBaseColor[i]==nullptr);
    VASSERT(m_pRadiosityColor[i]==nullptr);
    if (!m_spLightmapChannel[i])
      continue;
    int iSizeX = m_spLightmapChannel[i]->GetTextureWidth();
    int iSizeY = m_spLightmapChannel[i]->GetTextureHeight();
    if (iSizeX>m_iMaxSizeX) m_iMaxSizeX=iSizeX;
    if (iSizeY>m_iMaxSizeY) m_iMaxSizeY=iSizeY;
    int iTexel = iSizeX*iSizeY;
    
    m_pBaseColor[i] = new VColorExpRef[iTexel];
    m_pRadiosityColor[i] = new VColorExpRef[iTexel];
  }
  m_pRefTag = new char[m_iMaxSizeX*m_iMaxSizeY];
}


  
bool LightmapPageTweakInfo::MixSingleChannel(int iChannel, UBYTE *pDestBuffer, bool bSwapRB)
{

  if (!m_spLightmapChannel[iChannel])
    return false;

  float fMul = 1.f/LightmapManager::fColorMulFactor;
  int iSizeX = m_spLightmapChannel[iChannel]->GetTextureWidth();
  int iSizeY = m_spLightmapChannel[iChannel]->GetTextureHeight();

  // do the mixing
  UBYTE *pDst = pDestBuffer;
  VColorExpRef *pSrcBase = m_pBaseColor[iChannel];
  VColorExpRef *pSrcRad = m_pRadiosityColor[iChannel];
  hkvVec3 vColBase,vColRadiosity,vColResult;

  hkvMat4 colorMatBase (hkvNoInitialization);
  hkvMat4 colorMatRadiosity (hkvNoInitialization);

  // convert the matrices to native classes
  ConversionUtils::Matrix4FToVisMatrix4x4 (colorMatBase, LightmapManager::baseColorMat);
  ConversionUtils::Matrix4FToVisMatrix4x4 (colorMatRadiosity, LightmapManager::radiosityColorMat);

  if (LightmapManager::bNoBaseLight) 
    colorMatBase.setZero();

  if (LightmapManager::bNoRadiosity) 
    colorMatRadiosity.setZero ();

  const hkvVec3 vBlack(0.f,0.f,0.f);
  int R = 0;
  int G = 1;
  int B = 2;
  if (bSwapRB)
  {
    R = 2;
    G = 1;
    B = 0;
  }

  char *pTag = m_pRefTag;
  for (int y=0;y<iSizeY;y++)
    for (int x=0;x<iSizeX;x++,pDst+=3,pTag++,pSrcBase++,pSrcRad++) if (pTag[0])
    {
      pSrcBase->ToFloat(vColBase);
      pSrcRad->ToFloat(vColRadiosity);

      // mix the two components
      hkvVec3 resBase = colorMatBase * vColBase;
      hkvVec3 resRad = colorMatRadiosity * vColRadiosity;
      resBase.setMax(vBlack); // clamp against black
      resRad.setMax(vBlack); // clamp against black
      vColResult = resBase+resRad;
      vColResult *= fMul; // scale according to equation (does not yet use the smooth function here)
      
      // convert to 24bit RGB
      VColorExpRef pColorConv;
      VColorRef iDstCol = pColorConv.Float_To_RGB(vColResult);
      pDst[R] = iDstCol.r;
      pDst[G] = iDstCol.g;
      pDst[B] = iDstCol.b;

    }

  return true;
}


void LightmapPageTweakInfo::MixAndUploadLightmaps(UBYTE *pDestBuffer)
{
  for (int i=0;i<4;i++)
  {
    if (!MixSingleChannel(i,pDestBuffer,true))
      continue;

    // upload to graphics card
    m_spLightmapChannel[i]->UpdateRect(0, 0, 0, 0, 0, -1, pDestBuffer, V_TEXTURE_LOCKFLAG_DISCARDABLE);
  }
}


void LightmapPageTweakInfo::Save(UBYTE *pDestBuffer)
{
  for (int i=0;i<4;i++)
  {
    if (!MixSingleChannel(i,pDestBuffer,true))
      continue;

    int iSizeX = m_spLightmapChannel[i]->GetTextureWidth();
    int iSizeY = m_spLightmapChannel[i]->GetTextureHeight();

    // use vtex to save dds texture
    Image_cl image;
    VString sAbsFilename;
    const char *szFilename = m_spLightmapChannel[i]->GetFilename();
    String ^absPath = EditorManager::EngineManager->File_MakeAbsolute(ConversionUtils::VStringToString(szFilename));
    ConversionUtils::StringToVString( absPath, sAbsFilename );
    if (sAbsFilename.IsEmpty())
      continue;
    image.AddColorMap( iSizeX, iSizeY, COLORDEPTH_24BPP, (UBYTE *) pDestBuffer );
    image.Save(sAbsFilename);
  }
}



void LightmapManager::ApplyColorMatrices(Matrix4F baseMat, Matrix4F radioMat)
{
  baseColorMat = baseMat;
  radiosityColorMat = radioMat;
}


void LightmapManager::SaveAllLightmaps()
{
  for (int i=0;i<iPageCount;i++)
    pPageInfo[i].Save(pUploadBuffer);
}


void LightmapManager::SaveLightGrid()
{
  VLightGrid_cl *pGrid = Vision::RenderLoopHelper.GetLightGrid();
  if (!pLightGridInfo || !pGrid)
    return;
  pLightGridInfo->MixAndUpdate(pGrid);

  VString sAbsFilename;
  const char *szFilename = pGrid->GetFilename();
  String ^absPath = EditorManager::EngineManager->File_MakeAbsolute(ConversionUtils::VStringToString(szFilename));
  ConversionUtils::StringToVString( absPath, sAbsFilename );
  if (sAbsFilename.IsEmpty())
    return;

  pGrid->SaveToFile(sAbsFilename, szFilename);
}




void LightGridTweakInfo::FromLightGrid(VLightGrid_cl *pGrid)
{
  ReleaseColors();
  m_iFlatColorCount = pGrid->GetOverallNodeCount()*6;
  m_pBaseColors = new hkvVec3[m_iFlatColorCount];
  m_pRadiosityColors = new hkvVec3[m_iFlatColorCount];
  m_pMixResult = new hkvVec3[m_iFlatColorCount];
  m_bHasBase=true;
  m_bHasRadiosity=false;
  pGrid->CopyColorsToBuffer(m_pBaseColors);
}

void LightGridTweakInfo::MixAndUpdate(VLightGrid_cl *pGrid)
{
  if (m_iFlatColorCount!=pGrid->GetOverallNodeCount()*6)
    return;

  hkvVec3* pSrcBase = m_pBaseColors;
  hkvVec3* pSrcRad = m_pRadiosityColors;
  hkvVec3* pDest = m_pMixResult;
  hkvVec3 vColBase,vColRadiosity,vColResult;

  hkvMat4 colorMatBase (hkvNoInitialization);
  hkvMat4 colorMatRadiosity (hkvNoInitialization);
  const hkvVec3 vBlack(0.f,0.f,0.f);

  // convert the matrices to native classes
  ConversionUtils::Matrix4FToVisMatrix4x4(colorMatBase, LightmapManager::baseColorMat);
  ConversionUtils::Matrix4FToVisMatrix4x4(colorMatRadiosity, LightmapManager::radiosityColorMat);

  if (LightmapManager::bNoBaseLight || !m_bHasBase) 
    colorMatBase.setZero();

  if (LightmapManager::bNoRadiosity || !m_bHasRadiosity) 
    colorMatRadiosity.setZero();

  // linearly loop through flat light grid list
  for (int i=0;i<m_iFlatColorCount;i++,pSrcBase++,pSrcRad++,pDest++)
  {
    if (m_pBaseColors)
      vColBase = *pSrcBase;
    if (m_pRadiosityColors)
      vColRadiosity = *pSrcRad;

    // mix the two components
    hkvVec3 resBase = colorMatBase * vColBase;
    hkvVec3 resRad  = colorMatRadiosity * vColRadiosity;
    resBase.setMax(vBlack); // clamp against black
    resRad.setMax(vBlack); // clamp against black
    vColResult = resBase+resRad;
    *pDest = vColResult;
  }

  pGrid->CopyColorsFromBuffer(m_pMixResult);
  Vision::RenderLoopHelper.InvalidateLightgrid();
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
