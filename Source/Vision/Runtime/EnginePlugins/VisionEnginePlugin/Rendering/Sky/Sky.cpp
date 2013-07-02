/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Sky/Sky.hpp>

#if !defined(_VISION_MOBILE) && !defined(HK_ANARCHY)
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/VGlobalFogPostprocess.hpp>
#endif

#if !defined( _VISION_MOBILE )
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/TimeOfDay.hpp>
#endif

extern VProgressStatus g_DiscardProgress;
#define LOADINGPROGRESS   Vision::GetApplication()->GetLoadingProgress()

VShaderConstantBuffer VSky::g_GlobalConstantBufferSky;

VSkyManager VSkyManager::s_manager;

VSkyManager &VSkyManager::GlobalManager()
{
  return s_manager;
}

void VSkyManager::OneTimeInit()
{
  Vision::Callbacks.OnEngineDeInitializing += this;
}

void VSkyManager::OneTimeDeinit()
{
  Vision::Callbacks.OnEngineDeInitializing -= this;
}

void VSkyManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender==&Vision::Callbacks.OnEngineDeInitializing)
  { 
    VSky::g_GlobalConstantBufferSky.FreeBuffer();
  }
}


float VSkyBase::CalculateFogDepth() const
{
  //only apply fog to the skyshader if the fog post processor is deactivated
  //if the post processor is activated it will apply the fog to sky automatically
  IVRendererNode* pRenderNode = Vision::Renderer.GetRendererNode(0);
  if (pRenderNode != NULL)
  {
#if !defined(_VISION_MOBILE) && !defined( HK_ANARCHY )
    const VPostProcessingBaseComponent* pFogPostProcess = static_cast<VPostProcessingBaseComponent*>(
      pRenderNode->Components().GetComponentOfType(VGlobalFogPostprocess::GetClassTypeId()));
    if (pFogPostProcess == NULL || !pFogPostProcess->IsActive())
#endif
    {
      VFogParameters fog = Vision::World.GetFogParameters();
      if (!fog.bMaskSky && (fog.depthMode != VFogParameters::Off) && (fog.fDepthEnd > fog.fDepthStart))
      {
        float camFar;
        float camNear;
        VisRenderContext_cl::GetMainRenderContext()->GetClipPlanes(camNear, camFar);
        return (camFar - fog.fDepthStart) / (fog.fDepthEnd - fog.fDepthStart);
      }
    }
  }
  return 0.f;
}


VSky::~VSky()
{
  V_SAFE_DELETE_ARRAY(m_pLayers);
  Vision::Callbacks.OnReassignShaders -= this;
}


VSky::VSky(int boxSubDivs)
  : VSkyBase()
{
  ADDTOELEMENTMANAGER_ADDREF();

  VASSERT(boxSubDivs>0);
  BoxSubDivisions = boxSubDivs;

  LayerCount = 0;
  m_pLayers = NULL; 
  m_bDefaultEffect = true;
  m_iVisibleFaces = CUBEMAPFACE_BIT_ALL;
  m_iLayerTechniqueCachedMask = 0;
  m_iTrackPSBufferMask = 0;
  m_iRenderLayerCount = 0;

  Vision::Callbacks.OnReassignShaders += this;
}

VSky::VSky(const char *szPrefixNoon, const char *szPrefixDawn, const char *szPrefixDusk, 
           const char *szPrefixNight, const char *szExtension, bool bUseBottom, VCompiledEffect *pFX)
  : VSkyBase()
{
  ADDTOELEMENTMANAGER_ADDREF();
  BoxSubDivisions = 1;

  LayerCount = 0;
  m_pLayers = NULL; 
  m_bDefaultEffect = true;
  m_iVisibleFaces = CUBEMAPFACE_BIT_ALL;
  m_iLayerTechniqueCachedMask = 0;
  m_iTrackPSBufferMask = 0;
  m_iRenderLayerCount = 0;

  char szFileName[6][FS_MAX_PATH];
  CreateLayers(4);

  sprintf(szFileName[0], "%s_right.%s", szPrefixNoon, szExtension); 
  sprintf(szFileName[1], "%s_left.%s", szPrefixNoon, szExtension);
  sprintf(szFileName[2], "%s_front.%s", szPrefixNoon, szExtension);
  sprintf(szFileName[3], "%s_back.%s", szPrefixNoon, szExtension);
  sprintf(szFileName[4], "%s_top.%s", szPrefixNoon, szExtension);
  sprintf(szFileName[5], "%s_bottom.%s", szPrefixNoon, szExtension);
  
  GetLayer(0).SetMapping(VIS_SKYMAPPING_CUBEMAP);
  if (bUseBottom) 
    GetLayer(0).SetCubemapTextureFiles(szFileName[0], szFileName[1], szFileName[2], szFileName[3], szFileName[4], szFileName[5]);
  else
    GetLayer(0).SetCubemapTextureFiles(szFileName[0], szFileName[1], szFileName[2], szFileName[3], szFileName[4], NULL);
  
  sprintf(szFileName[0], "%s_right.%s", szPrefixDawn, szExtension);
  sprintf(szFileName[1], "%s_left.%s", szPrefixDawn, szExtension);
  sprintf(szFileName[2], "%s_front.%s", szPrefixDawn, szExtension);
  sprintf(szFileName[3], "%s_back.%s", szPrefixDawn, szExtension);
  sprintf(szFileName[4], "%s_top.%s", szPrefixDawn, szExtension);
  sprintf(szFileName[5], "%s_bottom.%s", szPrefixDawn, szExtension);
  GetLayer(1).SetMapping(VIS_SKYMAPPING_CUBEMAP);
  if (bUseBottom)
    GetLayer(1).SetCubemapTextureFiles(szFileName[0], szFileName[1], szFileName[2], szFileName[3], szFileName[4], szFileName[5]);
  else 
    GetLayer(1).SetCubemapTextureFiles(szFileName[0], szFileName[1], szFileName[2], szFileName[3], szFileName[4], NULL);

  sprintf(szFileName[0], "%s_right.%s", szPrefixNight, szExtension);
  sprintf(szFileName[1], "%s_left.%s", szPrefixNight, szExtension);
  sprintf(szFileName[2], "%s_front.%s", szPrefixNight, szExtension);
  sprintf(szFileName[3], "%s_back.%s", szPrefixNight, szExtension);
  sprintf(szFileName[4], "%s_top.%s", szPrefixNight, szExtension);
  sprintf(szFileName[5], "%s_bottom.%s", szPrefixNight, szExtension);
  GetLayer(2).SetMapping(VIS_SKYMAPPING_CUBEMAP);
  if (bUseBottom)
    GetLayer(2).SetCubemapTextureFiles(szFileName[0], szFileName[1], szFileName[2], szFileName[3], szFileName[4], szFileName[5]);
  else
    GetLayer(2).SetCubemapTextureFiles(szFileName[0], szFileName[1], szFileName[2], szFileName[3], szFileName[4], NULL);
  
  sprintf(szFileName[0], "%s_right.%s", szPrefixDusk, szExtension);
  sprintf(szFileName[1], "%s_left.%s", szPrefixDusk, szExtension);
  sprintf(szFileName[2], "%s_front.%s", szPrefixDusk, szExtension);
  sprintf(szFileName[3], "%s_back.%s", szPrefixDusk, szExtension);
  sprintf(szFileName[4], "%s_top.%s", szPrefixDusk, szExtension);
  sprintf(szFileName[5], "%s_bottom.%s", szPrefixDusk, szExtension);
  GetLayer(3).SetMapping(VIS_SKYMAPPING_CUBEMAP);
  if (bUseBottom)
    GetLayer(3).SetCubemapTextureFiles(szFileName[0], szFileName[1], szFileName[2], szFileName[3], szFileName[4], szFileName[5]);
  else 
    GetLayer(3).SetCubemapTextureFiles(szFileName[0], szFileName[1], szFileName[2], szFileName[3], szFileName[4], NULL);

  Activate();
  SetEffect(pFX);

  Vision::Callbacks.OnReassignShaders += this;
}


VSky::VSky(const char* pszTextures, int iNumSides)
  : VSkyBase()
{
  VASSERT(iNumSides == 5 || iNumSides == 6);

  ADDTOELEMENTMANAGER_ADDREF();
  BoxSubDivisions = 1;

  LayerCount = 0;
  m_pLayers = NULL; 
  m_bDefaultEffect = true;
  m_iVisibleFaces = CUBEMAPFACE_BIT_ALL;
  m_iLayerTechniqueCachedMask = 0;
  m_iTrackPSBufferMask = 0;
  m_iRenderLayerCount = 0;

  CreateLayers(1);
  VSkyLayer &layer = GetLayer(0);

  const char* pszSide[6] =
  {
    "right", "left",
    "front",  "back",
    "top",    "bottom",
  };

  //Build the names of the cube sides
  char pszFile[6][FS_MAX_PATH];
  for (int i=0; i<iNumSides; i++)
    sprintf(pszFile[i], pszTextures, pszSide[i]);

  layer.SetCubemapTextureFiles( pszFile[0], pszFile[1],
                                    pszFile[2], pszFile[3],
                                    pszFile[4], (iNumSides==6) ? pszFile[5] : NULL );

  layer.SetMapping(VIS_SKYMAPPING_CUBEMAP);
  Activate();
  
  Vision::Callbacks.OnReassignShaders += this;
}

void VSky::DisposeObject()
{
  VisTypedEngineObject_cl::DisposeObject();
  if (IsActive())
    Vision::World.SetActiveSky(NULL);
  else
  {
    REMOVEFROMELEMENTMANAGER_RELEASE();
  }
}


void VSky::CreateLayers(int iCount)
{
  VASSERT(iCount>=0 && iCount<=MAX_SKY_LAYERS);
  if (iCount==LayerCount)
    return;

  V_SAFE_DELETE_ARRAY(m_pLayers);
  LayerCount = iCount;
  if (LayerCount>0)
    m_pLayers = new VSkyLayer[LayerCount];
  for (int i=0;i<LayerCount;i++)
    m_pLayers[i].SetParentSky(this);
}


void VSky::Activate()
{
  Vision::World.SetActiveSky(this);
}


void VSky::Deactivate()
{
  if (IsActive())
    Vision::World.SetActiveSky(NULL);
}



BOOL VSky::IsActive() const
{
  return Vision::World.GetActiveSky()==this;
}

BOOL VSky::IsVisible()
{
  IVSky *pSky = Vision::World.GetActiveSky();
  return (pSky!=NULL) && ((pSky->GetVisibleBitmask()&Vision::Contexts.GetCurrentContext()->GetRenderFilterMask())!=0);
}


void VSky::Tick(float dtime)
{
  for (int i=0; i<LayerCount; i++) 
    m_pLayers[i].Tick(dtime);

#if !defined(_VISION_MOBILE)
  IVTimeOfDay *pTimeOfDayInterface = Vision::Renderer.GetTimeOfDayHandler();
  if (pTimeOfDayInterface == NULL)
    return;

  VASSERT(pTimeOfDayInterface->IsOfType(V_RUNTIME_CLASS(VTimeOfDay)) && "Incompatible time of day handler installed - has to be VTimeOfDay or a subclass of it!");
  VTimeOfDay *pTimeOfDay = (VTimeOfDay *)pTimeOfDayInterface;

  float fDawnWeight, fDuskWeight, fNightWeight;
  VColorRef vSunColor(false);
  pTimeOfDay->EvaluateColorValue(1.0f, vSunColor, fDawnWeight, fDuskWeight, fNightWeight);

  if (pTimeOfDay->GetControlSky())
  {
    if (GetLayerCount()>1)
      GetLayer(1).SetIntensity(fDawnWeight);

    if (GetLayerCount()>2)
      GetLayer(2).SetIntensity(fNightWeight);

    if (GetLayerCount()>3)
      GetLayer(3).SetIntensity(fDuskWeight);
  }

  VColorRef vAmbientColor(false);
  pTimeOfDay->EvaluateColorValue(0.1f, vAmbientColor, fDawnWeight, fDuskWeight, fNightWeight);
  Vision::Renderer.SetGlobalAmbientColor(vAmbientColor.ToFloat().getAsVec4 (1.0f));
#endif
}


void VSky::Serialize( VArchive &ar )
{
  int iVersion = 0;
  VisEffectConfig_cl fxConfig;
  if (ar.IsLoading())
  {
    LOADINGPROGRESS.PushRange(0.f,100.f);
    ar >> iVersion; VVERIFY(iVersion==0 && "Invalid sky serialization version");
    ar >> BoxSubDivisions;
    ar >> m_iVisibleBitmask;
    int iLayerCount;
    ar >> iLayerCount;
    CreateLayers(iLayerCount);
    float fStep = 100.f / ((float)iLayerCount +0.1f);
    for (int i=0;i<iLayerCount;i++)
    {
      LOADINGPROGRESS.PushRange(fStep);
      ar >> m_pLayers[i];
      LOADINGPROGRESS.PopRange();
    }

    // shader
    ar >> fxConfig;
    SetEffect(fxConfig.GetEffect());
    LOADINGPROGRESS.PopRange();

  } else
  {
    ar << iVersion;
    ar << BoxSubDivisions;
    ar << m_iVisibleBitmask;
    ar << LayerCount;
    for (int i=0;i<LayerCount;i++)
      ar << m_pLayers[i];

    // shaders (save NULL effect if left to default):
    fxConfig.SetEffect(m_bDefaultEffect ? NULL : GetEffect());
    ar << fxConfig;
  }
}


BOOL VSky::SerializeSkyConfig(VChunkFile &ar)
{
  const int iSkyMagic = 0x324ab642;

  if (ar.IsLoading())
  {
    int iTemp;
    ar.ReadInt(iTemp); 
    if (iTemp!=iSkyMagic)   {ar.SetError("This is not a valid skyconfig stream position");return FALSE;}

    IVFileInStream *pIn = ar.GetChunkInStream(); // get embedded chunk stream
    VASSERT(pIn);
    int iVers;
    VArchive archive(NULL,pIn, Vision::GetTypeManager());
    archive >> iVers;
    archive.SetLoadingVersion(iVers);

    // simply deserialize sky object
    VSky *pSky;
    archive >> pSky;

    archive.Close();
    pIn->Close();
       
    Vision::World.SetActiveSky(pSky);
    return !ar.IsInErrorState();
  }
  else
  {
    ar.WriteInt(iSkyMagic);

    IVFileOutStream *pOut = ar.GetChunkOutStream(); // get embedded chunk stream
    VASSERT(pOut);
    VArchive archive(NULL,pOut, Vision::GetTypeManager());
    archive << Vision::GetArchiveVersion();
    archive << Vision::World.GetActiveSky();
    archive.Close();
    pOut->Close();

    return !ar.IsInErrorState();
  }
}


VSky* VSky::ReadFromStream(IVFileInStream *pIn)
{
  VASSERT(pIn!=NULL);
  VArchive archive(NULL,pIn, Vision::GetTypeManager());
  int iVers;
  archive >> iVers;
  archive.SetLoadingVersion(iVers);

  // simply deserialize sky object
  VSky *pSky = (VSky *)archive.ReadObject(NULL);
  archive.Close();

  return pSky;
}

void VSky::WriteToStream(IVFileOutStream *pOut)
{
  VASSERT(pOut!=NULL);
  VArchive archive(NULL,pOut, Vision::GetTypeManager());
  archive << Vision::GetArchiveVersion();
  archive << this;
  archive.Close();
}


void VSky::SetEffect(VCompiledEffect *pFX)
{
  m_spEffect = pFX;
  m_iLayerTechniqueCachedMask = 0;
  m_iTrackPSBufferMask = 0;

  for (int i=0;i<MAX_SKY_LAYERS;i++)
  {
    m_cachedTechniqueInfo[i].spCachedTechnique = NULL;
  }

  if (!m_spEffect)
  {
    m_spShaderLib = NULL;
    return;
  }

  // store smart ptr to lib
  m_spShaderLib = pFX->m_pSourceEffect ? pFX->m_pSourceEffect->GetOwnerEffectLib() : NULL;
  m_bDefaultEffect = false;
}


VISION_APIFUNC VCompiledTechnique* VSky::GetTechnique(int iLayerCount)
{
  VASSERT(iLayerCount>=0 && iLayerCount<=MAX_SKY_LAYERS);
  const int iMask = 1<<iLayerCount;
  if (m_iLayerTechniqueCachedMask & iMask)
    return m_cachedTechniqueInfo[iLayerCount].spCachedTechnique; // might be NULL

  // load default shader lib if necessary
  if (!m_spEffect)
  {
    VASSERT(m_iLayerTechniqueCachedMask==0);
    Vision::Shaders.LoadShaderLibrary("\\Shaders\\DefaultSky.ShaderLib", SHADERLIBFLAG_HIDDEN);
    VCompiledEffect *pFX = Vision::Shaders.CreateEffect("DefaultSky", NULL, 0);
    if (!pFX)
    {
      Vision::Error.Warning("Failed to create default sky effect. Sky deactivated.");
      SetVisibleBitmask(0);
      m_iLayerTechniqueCachedMask = 0xffffffff;
      return NULL;
    }
    SetEffect(pFX);
    m_bDefaultEffect = true;
  }
  m_iLayerTechniqueCachedMask |= iMask;

  // get relevant technique (inclusion tags define the layer count)
  char szInclusion[64];
  sprintf(szInclusion,"SKY_LAYER_COUNT=%i",iLayerCount);
  //sprintf(szInclusion,"%iLayer",iLayerCount);
  VTechniqueConfig *pGlobalConfig = Vision::Shaders.GetGlobalTechniqueConfig();
  VTechniqueConfig usageConfig(szInclusion,NULL);
  VCompiledTechnique *pTech = m_spEffect->FindCompatibleTechnique(&usageConfig, pGlobalConfig);
  if (!pTech)
    pTech = m_spEffect->FindCompatibleTechnique(pGlobalConfig);
  m_cachedTechniqueInfo[iLayerCount].spCachedTechnique = pTech;
  // also cache the fogcoord register
  m_cachedTechniqueInfo[iLayerCount].fogCoordConstantRegister.Init(pTech->GetShader(0),"FogCoord");

  // see whether we need to track directly into sky registers (not DX11)
#if defined (_VR_DX11)

#else
  if (pTech!=NULL && pTech->GetShaderCount()==1)
  {
    IVConstantBuffer *pPS = pTech->GetShader(0)->GetConstantBuffer(VSS_PixelShader);
    if (pPS->m_iFirstRegister<=2 && pPS->m_iAllocatedEntries >= VCONSTANTBUFFER_REGISTER_GRANULARITY*iLayerCount)
      m_iTrackPSBufferMask |= iMask;

  }
#endif

  return pTech;
}


struct SkyVertex_t
{
  hkvVec3 pos;
  hkvVec3 cubemapUVandAngle;
};



void VSky::PrepareForRendering()
{
  m_iVisibleFaces = CUBEMAPFACE_BIT_ALL; // VSky::DetermineVisibleFaces() gives a better estimation...

  // evaluate which layers are visible
  VSkyLayer *pLayer = m_pLayers;
  m_iRenderLayerCount = 0;
  const int iMaxVisibleLayers = 6; // this is currently a limitation due to vertex shader constant collision [#1703]
  for (int i=0;(i<LayerCount && m_iRenderLayerCount<iMaxVisibleLayers);i++,pLayer++)
  {
    if (pLayer->GetIntensity()>0.f)
      m_iRenderLayerIndex[m_iRenderLayerCount++] = i;
  }
  // pre-cache the technique
  if (m_iRenderLayerCount>0)
    GetTechnique(m_iRenderLayerCount);

  // create the sky mesh buffer
  if (!m_spMesh)
  {
    const int iVerticesPerFace = (BoxSubDivisions+1)*(BoxSubDivisions+1);
    const int iVertexCount = iVerticesPerFace*6;
    const int iPrimPerFace = BoxSubDivisions*BoxSubDivisions*2;

    // allocate vertices
    VisMBVertexDescriptor_t vertexDesc;
    vertexDesc.m_iPosOfs = offsetof(SkyVertex_t,pos);
    vertexDesc.m_iTexCoordOfs[0] = offsetof(SkyVertex_t,cubemapUVandAngle) | VERTEXDESC_FORMAT_FLOAT3;

    vertexDesc.m_iStride = sizeof(SkyVertex_t);

    m_spMesh = new VisMeshBuffer_cl(vertexDesc, iVertexCount,VisMeshBuffer_cl::MB_PRIMTYPE_INDEXED_TRILIST, iPrimPerFace*6*3, iPrimPerFace*6, VIS_MEMUSAGE_STATIC, true);
    m_spMesh->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
#ifdef HK_DEBUG
    m_spMesh->SetFilename("<SkyVertices>");
#endif

    SkyVertex_t *pVertices = (SkyVertex_t *)m_spMesh->LockVertices(VIS_LOCKFLAG_DISCARDABLE);
    unsigned short *pIndices = (unsigned short *)m_spMesh->LockIndices(VIS_LOCKFLAG_DISCARDABLE);

    for (int iFace=0;iFace<6;iFace++)
      CreateSkyFaceVertices(iFace, &pVertices[iFace*iVerticesPerFace], &pIndices[iFace*iPrimPerFace*3]);

    m_spMesh->UnLockVertices();
    m_spMesh->UnLockIndices();
  }
}
  

void VSky::DetermineVisibleFaces()
{
  VISION_PROFILE_FUNCTION(VIS_PROFILE_VIS_SKYVIS);

  // since the faces are rendered using single draw calls (cubemap textures) it pays off to determine the visible faces
  // accurately:
  m_iVisibleFaces = 0; // none of the faces visible by default

  VisRenderContext_cl *pContext = VisRenderContext_cl::GetCurrentContext();
  IVisVisibilityCollector_cl *pVisColl = pContext->GetVisibilityCollector();
  if (pVisColl == NULL || pVisColl->GetBaseFrustum() == NULL)
  {
    m_iVisibleFaces = 0xFF;
    return;
  }

  const hkvVec3& camPos = pContext->GetCamera()->GetPosition();
  VisFrustum_cl frustum;
  frustum.CopyFrom(*pVisColl->GetBaseFrustum());

  if (frustum.HasNearCapPlane())
  {
    // Set near plane to camera origin
    // (clipping planes for water may move them far away from the origin)
    hkvPlane* pNearPlane = frustum.GetNearPlane();
    pNearPlane->setFromPointAndNormal(camPos, pNearPlane->m_vNormal);
  } 

  // build a simple box around the sky face and test its visibility
  const float r = 100.f;
  const float d = 10.f;

  hkvAlignedBBox bbox(hkvNoInitialization);
  for (int iFace = 0; iFace < 6; iFace++)
  {
    switch (iFace)
    {
      case VTM_CUBEMAPFACE_POS_X: 
        bbox.set(hkvVec3(-r-d, -r, -r), hkvVec3(-r, r, r)); break;
      case VTM_CUBEMAPFACE_NEG_X: 
        bbox.set(hkvVec3(r, -r, -r),    hkvVec3(r+d, r, r)); break;
      case VTM_CUBEMAPFACE_POS_Y: 
        bbox.set(hkvVec3(-r, -r-d, -r), hkvVec3(r, -r, r)); break;
      case VTM_CUBEMAPFACE_NEG_Y: 
        bbox.set(hkvVec3(-r, r, -r),    hkvVec3(r, r+d, r)); break;
      case VTM_CUBEMAPFACE_POS_Z: 
        bbox.set(hkvVec3(-r, -r, r),    hkvVec3(r, r, r+d)); break;
      case VTM_CUBEMAPFACE_NEG_Z: 
        bbox.set(hkvVec3(-r, -r, -r-d), hkvVec3(r, r, -r)); break;
      default:
        continue;
    }
    bbox.translate(camPos);
    //Vision::Game.DrawBoundingBox(bbox);

    if (frustum.Overlaps(bbox))
      m_iVisibleFaces |= (1 << iFace);
  }
}


void VSky::CreateSkyFaceVertices(int iFace, SkyVertex_t *pVertices, unsigned short *pIndices)
{
  float fInvCount = 1.f/(float)BoxSubDivisions;
  float fSize = 1.f;
  int x, y;
  float d = 0.5f*fSize;

  // vertices
  for (y=0;y<=BoxSubDivisions;y++)
    for (x=0;x<=BoxSubDivisions;x++,pVertices++)
    {
      float xx = ((float)x*fInvCount-0.5f)*fSize;
      float yy = ((float)y*fInvCount-0.5f)*fSize;
      float *pPos = &pVertices->pos.x;
      switch (iFace)
      {
      case VTM_CUBEMAPFACE_NEG_X: 
         pPos[0] = d; pPos[1] = -xx; pPos[2] = -yy;
         break;
      case VTM_CUBEMAPFACE_POS_X: 
         pPos[0] = -d; pPos[1] = xx; pPos[2] = -yy;
         break;
      case VTM_CUBEMAPFACE_NEG_Y: 
         pPos[0] = xx; pPos[1] = d; pPos[2] = -yy;
         break;
      case VTM_CUBEMAPFACE_POS_Y: 
         pPos[0] = -xx; pPos[1] = -d; pPos[2] = -yy;
         break;
      case VTM_CUBEMAPFACE_POS_Z: 
         pPos[0] = xx; pPos[1] = yy; pPos[2] = d;
         break;
      case VTM_CUBEMAPFACE_NEG_Z: 
         pPos[0] = xx; pPos[1] = -yy; pPos[2] = -d;
         break;
      default:
         VASSERT(!"Invalid face specified");
         break;
      }
      hkvVec2 vPos2D(pPos[0],pPos[1]);
      vPos2D.normalizeIfNotZero();

      // create UV
      float fAngle = hkvMath::acosRad (vPos2D.x*0.9999f) / (2.0f * hkvMath::pi ()); // map to 0..0.5 (ORLY ?)
      VASSERT(fAngle>=0.f && fAngle<=0.5f);
      if (vPos2D.y<0.f)
        fAngle = 1.f - fAngle;
      fAngle = hkvMath::mod (fAngle+0.125f,1.f); // shift by 45deg so the wrapping falls into a corner where we have vertex duplicates
      if (iFace==VTM_CUBEMAPFACE_NEG_X && x==BoxSubDivisions)
        fAngle = 0.f; // is close to 1.0 otherwise which causes the wrap-around

      pVertices->cubemapUVandAngle.x = (float)x*fInvCount;
      pVertices->cubemapUVandAngle.y = (float)y*fInvCount;
      pVertices->cubemapUVandAngle.z = 1.f - fAngle; // compensate for mirroring

  
    }

  // index list
  int iRow = BoxSubDivisions+1;
  int iFirstIndex = iFace*iRow*iRow;
  for (y=0;y<BoxSubDivisions;y++,iFirstIndex++)
    for (x=0;x<BoxSubDivisions;x++,pIndices+=6,iFirstIndex++)
    {
      pIndices[0] = iFirstIndex;
      pIndices[1] = iFirstIndex+1+iRow;
      pIndices[2] = iFirstIndex+1;
      pIndices[3] = iFirstIndex;
      pIndices[4] = iFirstIndex+iRow;
      pIndices[5] = iFirstIndex+1+iRow;
    }
}

VShaderConstantBuffer &VSky::GetSkyConstantBuffer()
{
  if (g_GlobalConstantBufferSky.m_pBuffer == NULL)
    g_GlobalConstantBufferSky.AllocateBuffer(0, 24 * VCONSTANTBUFFER_REGISTER_GRANULARITY);

  return g_GlobalConstantBufferSky;
}

void VSky::SetShaderConstants(VCompiledTechnique *pTechnique)
{
  float SkyColorConst[4*MAX_SKY_LAYERS];
  float SkyTransformConst[8*MAX_SKY_LAYERS];
  hkvVec4* pSkyColor = (hkvVec4*) SkyColorConst;
  hkvVec4* pSkyTransform = (hkvVec4*) SkyTransformConst;
  const bool bTrackIntoConstBuffer = (m_iTrackPSBufferMask&(1<<m_iRenderLayerCount))!=0;

  if (bTrackIntoConstBuffer)
    pSkyColor = (hkvVec4*) pTechnique->GetShader(0)->GetConstantBuffer (VSS_PixelShader)->Lock (2, VCONSTANTBUFFER_REGISTER_GRANULARITY*m_iRenderLayerCount, VIS_LOCKFLAG_DISCARDABLE);

  // shader constant setup
  for (int i=0;i<m_iRenderLayerCount;i++,pSkyColor++,pSkyTransform+=2)
  {
    int iLayer = m_iRenderLayerIndex[i];
    VASSERT(iLayer<LayerCount);
    VSkyLayer *pLayer = &m_pLayers[iLayer];

    pSkyColor[0] = pLayer->GetColor();
    pSkyTransform[0] = hkvVec4 ((float)pLayer->GetMappingType(), pLayer->GetHeightScale(), pLayer->GetHeightPos(), 1);
    if (pLayer->GetMappingType()==VIS_SKYMAPPING_CUBEMAP)
    {
      pSkyTransform[1].set(1,1,0,0);
    }
    else
    {
      hkvVec4& dest = pSkyTransform[1];

      pLayer->GetTextureScaling(dest.x, dest.y);
      pLayer->GetScrollPosition(dest.z, dest.w);

      // apply some additional transform for backwards compatibility
      dest.x *= 0.5f; dest.y *= 0.5f;
      dest.z += 0.5f; dest.w += 0.5f;
    }
  }

  VCompiledShaderPass *pPass = pTechnique->GetShader(0);
  VShaderConstantBuffer *pPS = pPass->GetConstantBuffer(VSS_PixelShader);

  // upload shader constants (pixel shader constants might go into shader's constant buffer)
  if (bTrackIntoConstBuffer)
  {
    pPS->Unlock();
    pPass->m_bModified = true;
  }
  else
  {
#ifdef _VR_DX11
    GetSkyConstantBuffer().SetRegisterRangeF(0 * VCONSTANTBUFFER_REGISTER_GRANULARITY, SkyColorConst, m_iRenderLayerCount * VCONSTANTBUFFER_REGISTER_GRANULARITY);
#else
    VisRenderStates_cl::SetPixelShaderConstant(2, SkyColorConst, m_iRenderLayerCount);
#endif
  }
#ifdef _VR_DX11
  GetSkyConstantBuffer().SetRegisterRangeF(8 * VCONSTANTBUFFER_REGISTER_GRANULARITY, SkyTransformConst, m_iRenderLayerCount * 2 * VCONSTANTBUFFER_REGISTER_GRANULARITY);
#else
  VisRenderStates_cl::SetVertexShaderConstant(44, SkyTransformConst, m_iRenderLayerCount*2);
#endif

}

void VSky::Render()
{
  if (!m_spMesh || !m_iVisibleFaces || !m_iRenderLayerCount)
    return;

  VCompiledTechnique *pTechnique = GetTechnique(m_iRenderLayerCount);
  if (pTechnique==NULL)
    return;

  VASSERT(pTechnique->GetShaderCount()>=1);

  INSERT_PERF_MARKER_SCOPE("VSky::Render");

  //calculate fogdepth and apply to shader
  float fogCoord = CalculateFogDepth(); 
  VCompiledShaderPass* pPass = pTechnique->GetShader(0);
  m_cachedTechniqueInfo[m_iRenderLayerCount].fogCoordConstantRegister.SetRegisterValueSafeF(pPass, &fogCoord);


#if defined(_VR_DX11)
  VisRenderStates_cl::SetVSConstantBuffer(VCONSTANTBUFFERINDEX_SKY, &GetSkyConstantBuffer());
  VisRenderStates_cl::SetPSConstantBuffer(VCONSTANTBUFFERINDEX_SKY, &GetSkyConstantBuffer());
#endif

  // setup matrix
  VisRenderContext_cl *pContext = VisRenderContext_cl::GetCurrentContext();
  hkvMat4 skyMatrix; // identity
  hkvVec3 EyePos(hkvNoInitialization);
  float fNear, fFar;
  pContext->GetClipPlanes(fNear, fFar);
  float fClipCenter = (fFar + fNear) * 0.5f;
  pContext->GetCamera()->GetPosition(EyePos);
  skyMatrix.setDiagonal (hkvVec4 (fClipCenter,fClipCenter,fClipCenter,1.f));
  skyMatrix.setTranslation(EyePos);

  const int iPrimPerFace = BoxSubDivisions*BoxSubDivisions*2;

  int iViewX, iViewY, iViewWidth, iViewHeight;
  float zNear, zFar;
  pContext->GetViewport(iViewX, iViewY, iViewWidth, iViewHeight, zNear, zFar);

  pContext->SetViewport(iViewX, iViewY, iViewWidth, iViewHeight, 0.9999999f, 1.0f); // this way we do not have to bother about actual z values (problem in deferred mode)
  
  Vision::RenderLoopHelper.BeginMeshRendering();

  Vision::RenderLoopHelper.SetMeshTransformationMatrix(skyMatrix);
  Vision::RenderLoopHelper.AddMeshStreams(m_spMesh);

  SetShaderConstants(pTechnique);

  VStateGroupSampler *pClampSampler = (VStateGroupSampler *)VisRenderStates_cl::GetSamplerStateGroup(VIS_SAMPLER_CLAMP);
  const int iPasses = pTechnique->GetShaderCount();
  for (int iPass=0;iPass<iPasses;iPass++) // for each shader...
  {
    VCompiledShaderPass *pShader = pTechnique->GetShader(iPass);
    int iPrimStart=0;
    for (int iFace=0;iFace<6;iFace++,iPrimStart+=iPrimPerFace) // for each cubemap face
    {
      if ((m_iVisibleFaces&(1<<iFace))==0) // determined by visibility
        continue;
      bool bSkipFace = false;

      // bind specific textures
      for (int iStage=0;iStage<m_iRenderLayerCount;iStage++)
      {
        int iLayer = m_iRenderLayerIndex[iStage];
        VASSERT(iLayer<LayerCount);
        VSkyLayer *pLayer = &m_pLayers[iLayer];
        switch (pLayer->GetMappingType())
        {
          case VIS_SKYMAPPING_CUBEMAP:
          {
            VTextureObject *pTex = pLayer->GetCubemapFace(iFace);
            if (pTex)
              Vision::RenderLoopHelper.BindMeshTexture(pTex, iStage, pClampSampler);
            else
              bSkipFace = true; // clip any cubemap part that has no texture assigned (e.g. bottom)
            break;
          }
          case VIS_SKYMAPPING_SPHERICAL:
            Vision::RenderLoopHelper.BindMeshTexture(pLayer->GetTexture(), iStage, NULL);
            break;
          case VIS_SKYMAPPING_CYLINDRICAL:
            //if (iFace==VTM_CUBEMAPFACE_POS_Z || iFace==VTM_CUBEMAPFACE_NEG_Z)
           //   Vision::RenderLoopHelper.BindMeshTexture(texmanager.GetPlainBlackTexture(), iStage, NULL);
          //  else
            // plain black does not help here because we need alpha=0. So we clamp original texture here
              Vision::RenderLoopHelper.BindMeshTexture(pLayer->GetTexture(), iStage, NULL);
            break;
        }
      }

      if (bSkipFace) // skip this face?
        continue;

      // render call:
      Vision::RenderLoopHelper.RenderMeshes( pShader, m_spMesh->GetPrimitiveType(), iPrimStart, iPrimPerFace, m_spMesh->GetVertexCount());
    }
  }
  Vision::RenderLoopHelper.EndMeshRendering();

  pContext->SetViewport(iViewX, iViewY, iViewWidth, iViewHeight, zNear, zFar); // reset
  
#if defined(_VR_DX11)
  VisRenderStates_cl::SetVSConstantBuffer(VCONSTANTBUFFERINDEX_SKY, 0);
  VisRenderStates_cl::SetPSConstantBuffer(VCONSTANTBUFFERINDEX_SKY, 0);
#endif


}

void VSky::Initialize()
{
  int iLayers = LayerCount;
  LayerCount = -1;
  CreateLayers(iLayers);
}

void VSky::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender == &Vision::Callbacks.OnReassignShaders)
  {
    // effect will be re-created the next time PrepareRendering() is called
    SetEffect(NULL);
  }
}

#ifdef SUPPORTS_SNAPSHOT_CREATION

#define SAFE_GET_DEPEND(res) {if (res) res->GetDependencies(snapshot);}

void VSky::GetDependencies(VResourceSnapshot &snapshot)
{
  VisTypedEngineObject_cl::GetDependencies(snapshot);
  if (m_spEffect)
    m_spEffect->GetDependencies(snapshot, m_spShaderLib);

  VSkyLayer *pLayer = m_pLayers;
  for (int i=0;i<LayerCount;i++,pLayer++)
  {
    SAFE_GET_DEPEND(pLayer->GetTexture());
    for (int j=0;j<6;j++)
      SAFE_GET_DEPEND(pLayer->GetCubemapFace(j));
  }

}

#endif

V_IMPLEMENT_DYNCREATE( VSkyBase, VisTypedEngineObject_cl, &g_VisionEngineModule );

START_VAR_TABLE( VSkyBase, VisTypedEngineObject_cl, "Sky box", VVARIABLELIST_FLAGS_NONE, "Sky box" )
END_VAR_TABLE


V_IMPLEMENT_SERIAL( VSky, VSkyBase, 0, &g_VisionEngineModule );

START_VAR_TABLE(VSky,VSkyBase, "Sky box", VVARIABLELIST_FLAGS_NONE, "Sky box" )
  DEFINE_VAR_INT(VSky, BoxSubDivisions, "Number of box subdivisions", "12", 0, 0);
  DEFINE_VAR_INT(VSky, LayerCount, "Number of layers", "1", 0, 0);
END_VAR_TABLE

V_IMPLEMENT_SERIAL( VisSky_cl, VSky, 0, &g_VisionEngineModule );

START_VAR_TABLE(VisSky_cl,VSky, "Sky box", VVARIABLELIST_FLAGS_NONE, "Sky box" )
END_VAR_TABLE

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
