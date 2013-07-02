/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Runtime/Engine/Renderer/RenderLoop/VisionRenderLoop.hpp>
#include <Vision/Runtime/Engine/Renderer/VisApiSimpleRendererNode.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vShaderConstantHelper.hpp>


// register the classes
V_IMPLEMENT_DYNAMIC(IVisRenderLoop_cl, VisTypedEngineObject_cl, Vision::GetEngineModule());
V_IMPLEMENT_DYNAMIC(VisionRenderLoop_cl, IVisRenderLoop_cl, Vision::GetEngineModule());

VisStaticGeometryInstanceCollection_cl VisionRenderLoop_cl::s_ShadowsGeoInstanceCollection(4096, 2048);
VisEntityCollection_cl VisionRenderLoop_cl::s_ShadowsEntityCollection(2048, 1024);

VisStaticGeometryInstanceCollection_cl VisionRenderLoop_cl::s_ShadowsRelevantGeoInstanceCollection(4096, 2048);
VisEntityCollection_cl VisionRenderLoop_cl::s_ShadowsRelevantEntityCollection(2048, 1024);

VisStaticGeometryInstanceCollection_cl VisionRenderLoop_cl::s_LitGeoInstanceCollection(4096, 2048);
VisEntityCollection_cl VisionRenderLoop_cl::s_LitEntityCollection(2048, 1024);

VisStaticGeometryInstanceCollection_cl VisionRenderLoop_cl::s_RenderGeoInstanceCollection(4096, 2048);

// Should InitRenderLoop() and DeInitRenderLoop() be made part of the interface so
// we can call it as soon as the renderloop is installed/deinstalled (like the
// physics module)?

V_IMPLEMENT_DYNCREATE(VisionShaderProvider_cl, IVisShaderProvider_cl, Vision::GetEngineModule());

VisionShaderProvider_cl::VisionShaderProvider_cl() : m_DefaultLightingColorEffects()
{
  m_bInitialized = false;

  Vision::Callbacks.OnWorldInit += this;
  Vision::Callbacks.OnEngineDeInitializing += this;
  Vision::Callbacks.OnWorldDeInit += this;
}

VisionShaderProvider_cl::~VisionShaderProvider_cl()
{
  Vision::Callbacks.OnWorldInit -= this;
  Vision::Callbacks.OnEngineDeInitializing -= this;
  Vision::Callbacks.OnWorldDeInit -= this;
  m_spSpotlightTexture = NULL;
}


void VisionShaderProvider_cl::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender==&Vision::Callbacks.OnWorldInit)
  { 
    //setup shaders the first time
    if (!m_bInitialized)
    {
      LoadDefaultTextures();
      m_bInitialized = true;
    }
    return;    
  }
  if (pData->m_pSender==&Vision::Callbacks.OnWorldDeInit)
  {
    m_spBaseShaderLib = NULL;
    m_DefaultLightingColorEffects.Clear();
    return;
  }  
  if (pData->m_pSender==&Vision::Callbacks.OnEngineDeInitializing)
  {
    m_spBaseShaderLib = NULL;
    m_DefaultLightingColorEffects.Clear();
    ReleaseDefaultTextures();
    m_bInitialized = false;
    return;
  }
}

void VisionShaderProvider_cl::SetDefaultLightingColor(VColorRef iColor)
{
  VCompiledEffect **pEffects = m_DefaultLightingColorEffects.GetPtrs();
  int iNumEffects = m_DefaultLightingColorEffects.Count();
  for (int i=0; i<iNumEffects; i++, pEffects++)
  {
    VCompiledShaderPass *pShader = (*pEffects)->GetDefaultTechnique()->GetShader(0);
    VColorRef iCol = Vision::Renderer.GetDefaultLightingColor();
    hkvVec4 vColor = iCol.ToFloat().getAsVec4 (1.0f);

    pShader->GetConstantBuffer (VSS_PixelShader)->SetSingleParameterF( "Color", vColor.data);
  }
}

VCompiledEffect* VisionShaderProvider_cl::GetDefaultLightingColorEffect(bool bUseAlphaTest, float fAlphaThreshold, bool bIsDoubleSided, bool bUsesDepthWrite)
{
  if (m_spBaseShaderLib == NULL)
    m_spBaseShaderLib = Vision::GetApplication()->GetShaderProvider()->LoadBaseShaderLib();
  if (m_spBaseShaderLib == NULL)
    return NULL;

  char szParam[FS_MAX_PATH];
  VColorRef iCol = Vision::Renderer.GetDefaultLightingColor();
  hkvVec4 vColor = iCol.ToFloat().getAsVec4 (1.0f);

  sprintf(szParam,"Color=%.4f,%.4f,%.4f,%.4f;AlphaThreshold=%.4f;CullMode=%s;DepthWrite=%s;PassType=%s;",vColor.x,vColor.y,vColor.z,vColor.w,fAlphaThreshold,
    bIsDoubleSided ? "none" : "back", bUsesDepthWrite ? "true" : "false", VPassTypeToString(VPT_AUTO)   
  );
  VCompiledEffect *pEffect;
  if (bUseAlphaTest)
    pEffect = Vision::Shaders.CreateEffect("BS_TintColorAlphaTest",szParam);
  else
    pEffect = Vision::Shaders.CreateEffect("BS_TintColor",szParam);
  if (pEffect)
    m_DefaultLightingColorEffects.AddUnique(pEffect);
  return pEffect;
}


////////////////////////////////////////////////////////////////////////////////////////
// Constructor / Destructor / Init + Deinit
////////////////////////////////////////////////////////////////////////////////////////

// Constructor
VISION_APIFUNC VisionRenderLoop_cl::VisionRenderLoop_cl()
{
  m_pCameraFrustum = NULL;
  m_bInitialized = false;
  m_bTriggerCallbacks = m_bHasRenderHookCallbacks = true;
  m_bRenderTransparentPass = true;
  InitRenderLoop();

  Vision::ResourceSystem.SetReportSurvivingResources(FALSE);
}

// Destructor
VISION_APIFUNC VisionRenderLoop_cl::~VisionRenderLoop_cl()
{
  DeInitRenderLoop();
}


// Initialization method, called from the constructor
VISION_APIFUNC void VisionRenderLoop_cl::InitRenderLoop()
{
  if (m_bInitialized)
    DeInitRenderLoop();

  /*
  // The following collections are required pretty much every frame. To avoid allocations,
  // we create them here, and only resize them when necessary.
  // TODO: DO WE REALLY NEED ALL OF THEM AT THE SAME TIME???
  // TODO: CAN WE MAKE THEM STATIC, SINCE THEY ARE TEMPORARY ANYWAY?
  m_ShadowsGeoInstanceCollection = new VisStaticGeometryInstanceCollection_cl(4096, 2048);
  m_ShadowsRelevantGeoInstanceCollection = new VisStaticGeometryInstanceCollection_cl(4096, 2048);
  m_OccluderGeoInstanceCollection = new VisStaticGeometryInstanceCollection_cl(4096, 2048);
  m_LitGeoInstanceCollection = new VisStaticGeometryInstanceCollection_cl(4096, 2048);
  m_RenderGeoInstanceCollection = new VisStaticGeometryInstanceCollection_cl(4096, 2048);

  m_ShadowsEntityCollection = new VisEntityCollection_cl(2048, 1024);
  m_ShadowsRelevantEntityCollection = new VisEntityCollection_cl(2048, 1024);
  m_OccluderEntityCollection = new VisEntityCollection_cl(2048, 1024);
  m_LitEntityCollection = new VisEntityCollection_cl(2048, 1024);

  m_ParticleGroupCollection = new VisParticleGroupCollection_cl(512, 512);
  m_MeshBufferObjectCollection = new VisMeshBufferObjectCollection_cl(512, 512);
  */

  // register callbacks
  Vision::Callbacks.OnWorldInit += this;
  Vision::Callbacks.OnWorldDeInit += this;
  Vision::Callbacks.OnNewModelLoaded += this;
  Vision::Callbacks.OnReassignShaders += this;
  //Vision::Callbacks.OnNewMeshFileLoaded += this;
  m_bInitialized = true;
  m_iFrameCounter = 0;

  m_spAttenSmoothTex = NULL;
  m_spSpotlightTexture = NULL;

#if defined(__SNC__)
#pragma diag_push
#pragma diag_suppress=1229
#endif

  // if the renderloop is created while the world exists, we have to call this manually
  if (Vision::World.IsWorldInitialized())
    OnWorldInit();

#if defined(__SNC__)
#pragma diag_pop
#endif

  m_dynLightDefaultState = *VisRenderStates_cl::GetDepthStencilDefaultState();
  m_dynLightDefaultState.m_cDepthComparisonFunc = COMPARISON_EQUAL;
  m_dynLightDefaultState.m_iStencilReadMask = 0;
  m_dynLightDefaultState.m_bDepthWriteEnabled = false;
  m_dynLightDefaultState.ComputeHash();
}

// Deinitialization method
VISION_APIFUNC void VisionRenderLoop_cl::DeInitRenderLoop()
{
  if (!m_bInitialized)
    return;

  Vision::Callbacks.OnWorldInit -= this;
  Vision::Callbacks.OnWorldDeInit -= this;
  Vision::Callbacks.OnNewModelLoaded -= this;
  Vision::Callbacks.OnReassignShaders -= this;
  //Vision::Callbacks.OnNewMeshFileLoaded -= this;

  m_spAttenSmoothTex = NULL;
  m_spSpotlightTexture = NULL;

  m_bInitialized = false;
}

// Create commonly used shaders: Wallmark, Static Light Shadows
void VisionRenderLoop_cl::CreateBaseShaders()
{
  if (Vision::GetApplication()->GetShaderProvider()->LoadBaseShaderLib()==NULL)
    return;

  m_spForegroundFillPassTechnique = Vision::Shaders.CreateTechnique("BS_ForeGroundFill",NULL);
  VASSERT(m_spForegroundFillPassTechnique!=NULL);

  m_spForegroundMaskTechnique = Vision::Shaders.CreateTechnique("BS_ForeGroundMask", NULL);
  VASSERT(m_spForegroundMaskTechnique!=NULL);
}

////////////////////////////////////////////////////////////////////////////////////////
// Dynamic light shader handling
////////////////////////////////////////////////////////////////////////////////////////

void VisionShaderProvider_cl::ReleaseDefaultTextures()
{
  m_spSpotlightTexture = NULL;
  m_spAttenSmoothTex = NULL;
}


void VisionShaderProvider_cl::LoadDefaultTextures()
{
}


VTextureObject* VisionShaderProvider_cl::GetDefaultAttenuationTexture()
{
  if (!m_spAttenSmoothTex)
    m_spAttenSmoothTex = Vision::TextureManager.Load2DTexture("Atten\\smooth.dds", VTM_FLAG_DEFAULT_NON_MIPMAPPED);
  return m_spAttenSmoothTex;
}


VTextureObject* VisionShaderProvider_cl::GetDefaultSpotlightTexture()
{
  if (!m_spSpotlightTexture)
    m_spSpotlightTexture = Vision::TextureManager.Load2DTexture("defaultSpotlight.dds", VTM_FLAG_DEFAULT_MIPMAPPED);
  return m_spSpotlightTexture;
}


// Returns a dynamic light shader, depending on the passed parameters, and modifies the shader if necessary.
VCompiledTechnique *VisionShaderProvider_cl::GetDynamicLightShader(const VisLightSource_cl *pLight, 
  const VisSurface_cl *pSurface, bool bForEntity)
{
  VASSERT(pLight != NULL && pSurface != NULL);
  
  // light faded out?
  const hkvVec3 vCamPos = VisRenderContext_cl::GetCurrentContext()->GetCamera()->GetPosition();
  float fFade = pLight->GetFadeWeight(vCamPos);
  if (fFade<=HKVMATH_LARGE_EPSILON)
    return NULL;

  VCompiledTechnique *pTech;
  VisLightSourceType_e lightType = pLight->GetType();
  hkvPlane plane_x(hkvNoInitialization);
  hkvPlane plane_y(hkvNoInitialization);
  hkvPlane plane_z(hkvNoInitialization);

  // 1: pick light shader from material
  VTextureObject *pProjTexture = NULL;
  switch (lightType)
  {
    case VIS_LIGHT_POINT: // also supports point projection
      if (pSurface->m_spDynProjPointLight)
        pProjTexture = pLight->GetProjectionTexture();
      if (pProjTexture)
      {
        pTech = pSurface->m_spDynProjPointLight;
        pLight->GetProjectionPlanes(plane_x, plane_y, plane_z);
      } 
      else
        pTech = pSurface->m_spDynPointLight;

      break;
    case VIS_LIGHT_SPOTLIGHT: 
      pTech = pSurface->m_spDynSpotLight;
      pProjTexture = pLight->GetProjectionTexture();
      if (!pProjTexture)
        pProjTexture = GetDefaultSpotlightTexture();
      pLight->GetProjectionPlanes(plane_x, plane_y, plane_z); // this has the cone angle encoded already
      break;
    case VIS_LIGHT_DIRECTED: 
      pTech = pSurface->m_spDynDirectionalLight;
      break;
    default:
      VASSERT(!"Unsupported light type");
      return NULL;
  }
  if (!pTech)
    return NULL;

  // 2: modify light shader
  const hkvVec3 vColor = pLight->GetColor().ToFloat();
  hkvVec3 lightDir;
  if (lightType == VIS_LIGHT_DIRECTED || lightType == VIS_LIGHT_SPOTLIGHT)
  {
    lightDir = pLight->GetDirection();
    lightDir.normalize();
  }
  VTextureObject *pAttTexture = pLight->GetAttenuationTexture();
  if (!pAttTexture)
    pAttTexture = GetDefaultAttenuationTexture();

  const int iPassCount = pTech->GetShaderCount();
  for (int i=0;i<iPassCount;i++)
  {
    VDynamicLightShaderBase *pPass = vdynamic_cast<VDynamicLightShaderBase*>(pTech->GetShader(i));
    VASSERT_MSG(pPass != NULL, "Dynamic light shaders must be of class VDynamicLightShaderBase");   
    if (!pPass)
    {
      Vision::Error.Warning("Dynamic light shader is not of class VDynamicLightShaderBase; dynamic light will have no effect.");
      continue;
    }
    // set light source properties
    pPass->SetPosition(pLight->GetPosition());
    pPass->SetRadius(pLight->GetRadius());
    pPass->SetColor(vColor, pLight->GetMultiplier()*fFade);
    pPass->SetAttenuationTexture(pAttTexture);

    // set light direction for directed lights
    if (lightType == VIS_LIGHT_DIRECTED)
    {
      pPass->SetDirection(lightDir);
    }
    // set light direction and cone angle for spot lights
    else if (lightType == VIS_LIGHT_SPOTLIGHT)
    {
      pPass->SetDirection(lightDir);
      pPass->SetProjectionAngle(pLight->GetProjectionAngle());
    }

    if (pProjTexture)
    {
      VDynamicLightShader *pDynamicLightShader = vdynamic_cast<VDynamicLightShader*>(pPass);
      if (pDynamicLightShader)
      {
        pDynamicLightShader->SetProjectedTexture(pProjTexture);
        pDynamicLightShader->SetProjectionPlanes(plane_x,plane_y,plane_z);
      }
    }

    pPass->m_bModified = true;

    //we don't have a shadowmapcomponent here, if there is one this function is not used
    pPass->OnBeforeLightRendered(pLight);
  }

  return pTech;
}

////////////////////////////////////////////////////////////////////////////////////////
// Callback handling
////////////////////////////////////////////////////////////////////////////////////////

void VisionRenderLoop_cl::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  static bool bLevelLoading = true;

  // setup the lightgrid shader for any new model that is loaded
  if (pData->m_pSender==&Vision::Callbacks.OnNewModelLoaded)
  {
    if (!bLevelLoading)
    {
      VDynamicMeshLoadedDataObject* pModelInfo = (VDynamicMeshLoadedDataObject *)pData;
      if (pModelInfo->m_pNewMesh)
      {
        IVisShaderProvider_cl *pShaderProvider = Vision::GetApplication()->GetShaderProvider();
        if (pShaderProvider)
          pShaderProvider->CreateLightingShaderForModel(pModelInfo->m_pNewMesh, true);
      }
    }
    return;
  }

  if (pData->m_pSender==&Vision::Callbacks.OnWorldInit)
  {
    bLevelLoading = false;
    OnWorldInit();
    return;
  }

  if (pData->m_pSender==&Vision::Callbacks.OnWorldDeInit)
  {
    #ifdef SUPPORTS_MEMEXPORT_SKINNING
      ReleaseMemexportSkinningShaders();
    #endif

    IVisShaderProvider_cl *pShaderProvider = Vision::GetApplication()->GetShaderProvider();

    m_spStaticLightShadowTechnique = NULL;
    m_spForegroundFillPassTechnique = NULL;
    m_spForegroundMaskTechnique = NULL;
    bLevelLoading = true;
    return;
  }

  if (pData->m_pSender==&Vision::Callbacks.OnReassignShaders)
  {
    CreateBaseShaders();
    return;
  }
}

VTextureObject* VisionRenderLoop_cl::GetDefaultAttenuationTexture()
{
  if (!m_spAttenSmoothTex)
    m_spAttenSmoothTex = Vision::TextureManager.Load2DTexture("Atten\\smooth.dds", VTM_FLAG_DEFAULT_NON_MIPMAPPED);
  return m_spAttenSmoothTex;
}


VTextureObject* VisionRenderLoop_cl::GetDefaultSpotlightTexture()
{
  if (!m_spSpotlightTexture)
    m_spSpotlightTexture = Vision::TextureManager.Load2DTexture("defaultSpotlight.dds", VTM_FLAG_DEFAULT_MIPMAPPED);
  return m_spSpotlightTexture;
}


#ifdef SUPPORTS_MEMEXPORT_SKINNING

#if defined(_VISION_XENON)

void VisionRenderLoop_cl::CreateMemexportSkinningShaders()
{
  if (Vision::Shaders.LoadShaderLibrary("\\Shaders\\DefaultSkinningShader.ShaderLib", SHADERLIBFLAG_HIDDEN) == NULL)
  {
    VASSERT(!"failed to load 'Shaders\\DefaultSkinningShader.ShaderLib'");
  }

  VCompiledTechnique *pTech = Vision::Shaders.CreateTechnique("SkinningMexpQuatXbox360", NULL);
  VASSERT(pTech!=NULL);
  VCompiledShaderPass *pPass = pTech->m_Shaders.GetAt(0);
  VASSERT(pPass);
  
  Vision::RenderLoopHelper.SetMemexportQuaternionSkinningShader(pPass);

  pTech = Vision::Shaders.CreateTechnique("SkinningMexpMatrixXbox360", NULL);
  VASSERT(pTech!=NULL);
  pPass = pTech->m_Shaders.GetAt(0);
  VASSERT(pPass);
  
  Vision::RenderLoopHelper.SetMemexportMatrixSkinningShader(pPass);
}

void VisionRenderLoop_cl::ReleaseMemexportSkinningShaders()
{
  Vision::RenderLoopHelper.SetMemexportQuaternionSkinningShader(NULL);
  Vision::RenderLoopHelper.SetMemexportMatrixSkinningShader(NULL);
}

#endif // _VISION_XENON


#if defined(_VISION_WIIU)

void VisionRenderLoop_cl::CreateMemexportSkinningShaders()
{
  if (Vision::Shaders.LoadShaderLibrary("\\Shaders\\DefaultSkinningShader.ShaderLib", SHADERLIBFLAG_HIDDEN) == NULL)
  {
    VASSERT(!"failed to load 'Shaders\\DefaultSkinningShader.ShaderLib'");
  }

  VCompiledTechnique *pTech = Vision::Shaders.CreateTechnique("SkinningMexpMatrixWiiU", NULL);
  VASSERT(pTech!=NULL);
  VCompiledShaderPass *pPass = pTech->m_Shaders.GetAt(0);
  VASSERT(pPass);

  Vision::RenderLoopHelper.SetMemexportMatrixSkinningShader(pPass);

  // No quaternion-based skinning on WiiU.
  Vision::RenderLoopHelper.SetMemexportQuaternionSkinningShader(NULL);
}

void VisionRenderLoop_cl::ReleaseMemexportSkinningShaders()
{
  Vision::RenderLoopHelper.SetMemexportQuaternionSkinningShader(NULL);
  Vision::RenderLoopHelper.SetMemexportMatrixSkinningShader(NULL);
}

#endif // _VISION_WIIU


#if defined(_VR_DX11)

void VisionRenderLoop_cl::CreateMemexportSkinningShaders()
{
  // No StreamOut skinning on DX9 Devices
  if (Vision::Video.GetDXFeatureLevel() < D3D_FEATURE_LEVEL_10_0)
    return;

  // Load skinning lib
  if (Vision::Shaders.LoadShaderLibrary("\\Shaders\\DefaultSkinningShader.ShaderLib", SHADERLIBFLAG_HIDDEN) == NULL)
  {
    VASSERT(!"failed to load 'Shaders\\DefaultSkinningShader.ShaderLib'");
  }

  VCompiledTechnique *pTech = NULL;

  // On DX11 with a DX11 graphics card, use compute shader skinning.
    if (Vision::Video.GetDXFeatureLevel() >= D3D_FEATURE_LEVEL_11_0)
    {
      pTech = Vision::Shaders.CreateTechnique("SkinningMatrixCompute_Unified", NULL);
      if (pTech) // should always be !=NULL but it fails when Pix is enabled
      {
        VCompiledShaderPass *pPass = pTech->m_Shaders.GetAt(0);
        Vision::RenderLoopHelper.SetComputeMatrixSkinningShader(pPass);
      }
    }
  
  // If compute shader skinning is not used, use Stream-Out skinning
  if (pTech == NULL)
  {
    pTech = Vision::Shaders.CreateTechnique("SkinningMatrixStreamOut_Unified", NULL);
    VASSERT(pTech!=NULL);
    VCompiledShaderPass *pPass = pTech->m_Shaders.GetAt(0);
    VASSERT(pPass);
    Vision::RenderLoopHelper.SetMemexportMatrixSkinningShader(pPass);
  }

  // No quaternion-based skinning on PC.
  Vision::RenderLoopHelper.SetMemexportQuaternionSkinningShader(NULL);
}

void VisionRenderLoop_cl::ReleaseMemexportSkinningShaders()
{
  Vision::RenderLoopHelper.SetMemexportQuaternionSkinningShader(NULL);
  Vision::RenderLoopHelper.SetMemexportMatrixSkinningShader(NULL);
    Vision::RenderLoopHelper.SetComputeMatrixSkinningShader(NULL);
}

#endif  // defined(_VR_DX11)

#endif  // SUPPORTS_MEMEXPORT_SKINNING


void VisionRenderLoop_cl::OnWorldInit()
{
  CreateBaseShaders();

  IVisShaderProvider_cl *pShaderProvider = Vision::GetApplication()->GetShaderProvider();
  VASSERT(pShaderProvider && "application does not have a shader provider");

  #ifdef SUPPORTS_MEMEXPORT_SKINNING
    CreateMemexportSkinningShaders();
  #endif
}


void VisionRenderLoop_cl::RenderHook(const VisMeshBufferObjectCollection_cl &visibleMeshBuffer, const VisParticleGroupCollection_cl *visibleParticleGroups, VRenderHook_e eRenderHook, bool bTriggerCallbacks)
{
#ifdef HK_DEBUG
  char szMarkerString[1024];
  szMarkerString[0] = 0;
  sprintf(szMarkerString, "VisionRenderLoop_cl::RenderHook [%x]", (unsigned int)eRenderHook);
  INSERT_PERF_MARKER_SCOPE(szMarkerString);
#endif

  int iFilterMask = VisRenderContext_cl::GetCurrentContext()->GetRenderFilterMask();

  Vision::RenderLoopHelper.RenderMeshBufferObjects(visibleMeshBuffer, eRenderHook);
  if (visibleParticleGroups != NULL)
  {
    Vision::RenderLoopHelper.RenderParticleSystems(visibleParticleGroups, eRenderHook, iFilterMask);
  }

  if (bTriggerCallbacks)
  {
    VisRenderHookDataObject_cl data(&Vision::Callbacks.OnRenderHook, eRenderHook);
    Vision::Callbacks.OnRenderHook.TriggerCallbacks(&data);
  }
}




////////////////////////////////////////////////////////////////////////////////////////
// The Render Loop Main Function
////////////////////////////////////////////////////////////////////////////////////////

void VisionShaderProvider_cl::ResetCache()
{
  m_pLastLightTechnique = NULL;
  m_pLastDynamicLight = NULL;
  m_bLastDynamicLightForEntity = false;
}

void VisionRenderLoop_cl::HandleVisibleVisibilityObjects()
{
  m_VisibilityObjectCollector.HandleVisibleVisibilityObjects();
  }

VISION_APIFUNC void VisionRenderLoop_cl::OnDoRenderLoop(void *pUserData)
{
  INSERT_PERF_MARKER_SCOPE("VisionRenderLoop_cl::OnDoRenderLoop");

  m_iFrameCounter++; // just for arbitrary custom purposes

  #ifdef _VISION_XENON
    Vision::Video.GetD3DDevice()->SetShaderGPRAllocation(0, 0, 0);
  #endif 

  #if defined (WIN32) || defined (_VISION_XENON) || defined (_VISION_PS3) || defined(_VISION_PSP2) || defined(_VISION_WIIU)
    if (Vision::RenderLoopHelper.GetReplacementRenderLoop())
    {
      // render with this renderloop instead
      Vision::RenderLoopHelper.GetReplacementRenderLoop()->OnDoRenderLoop(pUserData);
      return;
    }
  #endif

  m_pShaderProvider = Vision::GetApplication()->GetShaderProvider();
  VASSERT(m_pShaderProvider);
  m_pShaderProvider->ResetCache();

  VisRenderContext_cl *pContext = VisRenderContext_cl::GetCurrentContext();
  IVisVisibilityCollector_cl *pVisCollector = pContext->GetVisibilityCollector();
  if (pVisCollector==NULL)
    return;

  const int iRenderFlags = pContext->GetRenderFlags();
  const int iRenderFilterMask = pContext->GetRenderFilterMask();
  m_pCameraFrustum = pVisCollector->GetBaseFrustum(); // TODO:use this instead of GetViewFrustum

  const VisStaticGeometryInstanceCollection_cl *pVisibleGeoInstancesPrimaryOpaquePass = pVisCollector->GetVisibleStaticGeometryInstancesForPass(VPT_PrimaryOpaquePass);
  const VisStaticGeometryInstanceCollection_cl *pVisibleGeoInstancesSecondaryOpaquePass = pVisCollector->GetVisibleStaticGeometryInstancesForPass(VPT_SecondaryOpaquePass);
  const VisStaticGeometryInstanceCollection_cl *pVisibleGeoInstancesTransparentPass = pVisCollector->GetVisibleStaticGeometryInstancesForPass(VPT_TransparentPass);
  const VisEntityCollection_cl *pVisibleEntitiesPrimaryOpaquePass = pVisCollector->GetVisibleEntitiesForPass(VPT_PrimaryOpaquePass);
  const VisEntityCollection_cl *pVisibleEntitiesSecondaryOpaquePass = pVisCollector->GetVisibleEntitiesForPass(VPT_SecondaryOpaquePass);
  const VisEntityCollection_cl *pVisibleEntitiesTransparentPass = pVisCollector->GetVisibleEntitiesForPass(VPT_TransparentPass);
  const VisEntityCollection_cl *pVisibleForeGroundEntities = pVisCollector->GetVisibleForeGroundEntities();
  HandleVisibleVisibilityObjects();

  // Clear the screen
  if ((iRenderFlags&VIS_RENDERCONTEXT_FLAG_NO_CLEARSCREEN)==0)
  {
    const VFogParameters &fog = Vision::World.GetFogParameters();
    VColorRef clearColor = fog.depthMode != VFogParameters::Off ? fog.iDepthColor : Vision::Renderer.GetDefaultClearColor();
    Vision::RenderLoopHelper.ClearScreen(VisRenderLoopHelper_cl::VCTF_All, clearColor);
  }

  m_bHasRenderHookCallbacks = m_bTriggerCallbacks && Vision::Callbacks.OnRenderHook.HasCallbacks();

  // Get a pointer to the collection of visible mesh buffer objects
  const VisMeshBufferObjectCollection_cl *pVisibleMeshBuffer = &m_VisibilityObjectCollector.GetMeshBufferObjectCollection();

  // Get a pointer to the collection of visible particle groups
  const VisParticleGroupCollection_cl *pVisibleParticleGroups = &m_VisibilityObjectCollector.GetParticleGroupCollection();



  /*****************************************************************************************************/
  /**  First render the primary opaque geometry pass which will only contain truly opaque geometry    **/
  /*****************************************************************************************************/

  {
    INSERT_PERF_MARKER_SCOPE("PrimaryOpaquePass");
    // Render all mesh buffer objects with the render order flag "VRH_PRE_RENDERING".
    RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_PRE_RENDERING, m_bTriggerCallbacks);

    // Render all mesh buffer objects with the render order flag "VRH_PRE_PRIMARY_OPAQUE_PASS_GEOMETRY".
    RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_PRE_PRIMARY_OPAQUE_PASS_GEOMETRY, m_bTriggerCallbacks);

    // Render all primary opaque pass surface shaders on opaque world geometry
    Vision::RenderLoopHelper.RenderStaticGeometrySurfaceShaders(*pVisibleGeoInstancesPrimaryOpaquePass, VPT_PrimaryOpaquePass);

    // Render all mesh buffer objects with the render order flag "VRH_PRE_PRIMARY_OPAQUE_PASS_ENTITIES".
    RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_PRE_PRIMARY_OPAQUE_PASS_ENTITIES, m_bTriggerCallbacks);

    // Render all primary opaque pass shaders on entities (see "DrawEntitiesShaders")
    DrawEntitiesShaders(*pVisibleEntitiesPrimaryOpaquePass, VPT_PrimaryOpaquePass);
  }



  /*****************************************************************************************************/
  /**  Now render the secondary opaque geometry pass which will contain alpha tested geometry,        **/
  /**  decals and masking operations (eg. stencil buffer)                                             **/
  /*****************************************************************************************************/

  {
    INSERT_PERF_MARKER_SCOPE("SecondaryOpaquePass");

    RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_PRE_SECONDARY_OPAQUE_PASS_GEOMETRY, m_bTriggerCallbacks);

    //Render static geometry instances for secondary opaque pass
    Vision::RenderLoopHelper.RenderStaticGeometrySurfaceShaders(*pVisibleGeoInstancesSecondaryOpaquePass, VPT_SecondaryOpaquePass);

    RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_PRE_SECONDARY_OPAQUE_PASS_ENTITIES, m_bTriggerCallbacks);

    //Render entities for secondary opaque pass
    DrawEntitiesShaders(*pVisibleEntitiesSecondaryOpaquePass, VPT_SecondaryOpaquePass);
  }

  // Start the hardware occlusion query. Note that this function always has to be called in render loops.
  // Also, the position of this call in the OnDoRenderLoop is important: The zBuffer contents at this stage of rendering will
  // act as occluders in the hardware occlusion queries.
  Vision::RenderLoopHelper.PerformHardwareOcclusionQuery();

  // Render sky
  Vision::RenderLoopHelper.RenderSky();

  // Render all mesh buffer objects with the render order flag "VRH_PRE_OCCLUSION_TESTS".
  RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_PRE_OCCLUSION_TESTS, m_bTriggerCallbacks);

  // Render visible foreground entities (see DrawForegroundEntities)
  DrawForegroundEntities(*pVisibleForeGroundEntities);
  
  /*****************************************************************************************************/
  /**  Next hardware occlussion queries are performed and the dynamic lighting is rendered            **/
  /*****************************************************************************************************/

  // If we don't render the transparent pass in this render loop (which means we render it in the VPostProcessTranslucencies postprocessor),
  // we can execute the pixel counter queries here. However, 
  Vision::RenderLoopHelper.PerformHardwarePixelCounterQuery();

  // Render all mesh buffer objects with the render order flag "VRH_POST_OCCLUSION_TESTS".
  RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_POST_OCCLUSION_TESTS, m_bTriggerCallbacks);

  // Draw dynamic light 
  DrawDynamicLight();

  /*****************************************************************************************************/
  /**  Last the transparent geometry pass is rendered which will contain transparent geometry         **/
  /*****************************************************************************************************/

  if(m_bRenderTransparentPass)
  {
    INSERT_PERF_MARKER_SCOPE("TransparentPass");
    bool bDrawMasksAndDebugGeometry = (Vision::Renderer.GetCurrentRendererNode() == NULL || Vision::Renderer.GetCurrentRendererNode()->IsOfType(VSimpleRendererNode::GetClassTypeId()));

    // Mask out entities which are "always in foreground"
    MaskOutForegroundEntities(*pVisibleForeGroundEntities);

    // Render all mesh buffer objects with the render order flag "VRH_PRE_TRANSPARENT_PASS_GEOMETRY".
    RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_PRE_TRANSPARENT_PASS_GEOMETRY, m_bTriggerCallbacks);

    // Render transparent pass surface shaders on opaque world primitives 
    Vision::RenderLoopHelper.RenderStaticGeometrySurfaceShaders(*pVisibleGeoInstancesTransparentPass, VPT_TransparentPass);

    RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_PRE_TRANSPARENT_PASS_ENTITIES, m_bTriggerCallbacks);

    // Render transparent pass shaders on entities
    DrawEntitiesShaders(*pVisibleEntitiesTransparentPass, VPT_TransparentPass);

    // Render all mesh buffer objects with the render order flag "VRH_POST_TRANSPARENT_PASS_GEOMETRY"
    RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_POST_TRANSPARENT_PASS_GEOMETRY, m_bTriggerCallbacks);

    // Render all mesh buffer objects and particle systems with the render order flag "VRH_DECALS".
    RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_DECALS, m_bTriggerCallbacks);

    // Render all mesh buffer objects and particle systems with the render order flag "VRH_PARTICLES".
    RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_PARTICLES, m_bTriggerCallbacks);

    // Render all mesh buffer objects with the render order flag "VRH_ADDITIVE_PARTICLES"
    RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_ADDITIVE_PARTICLES, m_bTriggerCallbacks);

    RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_TRANSLUCENT_VOLUMES, m_bTriggerCallbacks);

    // Render all mesh buffer objects with the render order flag "VRH_CORONAS_AND_FLARES"
    RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_CORONAS_AND_FLARES, m_bTriggerCallbacks);

    // Render profiling information (charts, text, diagrams)
    if (iRenderFilterMask & Vision::Profiling.GetVisibleBitmask())
    {
      // If we have a renderer node (and are therefore rendering to an offscreen context), we only
      // render the geometry-based profiling/debug information here. In this case, the textual information
      // will be displayed in the final-stage postprocessor.
      if (bDrawMasksAndDebugGeometry)
      {
        Vision::RenderLoopHelper.RenderProfilingInfo(VIS_PROFILINGINFO_TEXT);
      }
    }

    Vision::RenderLoopHelper.RenderProfilingInfo(VIS_PROFILINGINFO_GEOMETRY);

    // Render all mesh buffer objects with the render order flag "VRH_PRE_SCREENMASKS", but only
    // if we are NOT in a renderer node (in this case, the callback will be handled by the final-stage postprocessor).
    // If we are in a renderer node, we only render the particle groups.
    if (bDrawMasksAndDebugGeometry)
      RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_PRE_SCREENMASKS, m_bTriggerCallbacks); 
    else
      Vision::RenderLoopHelper.RenderParticleSystems(pVisibleParticleGroups, VRH_PRE_SCREENMASKS, pContext->GetRenderFilterMask());

    // Render all screen masks (if we're rendering to a texture, only render them if rendering masks to a texture is enabled)
    Vision::RenderLoopHelper.RenderScreenMasks();

    // Trigger "VRH_GUI" render hook callback
    if (bDrawMasksAndDebugGeometry)
    {
      // Trigger "VRH_GUI" render hook callback
      VisRenderHookDataObject_cl data(&Vision::Callbacks.OnRenderHook, VRH_GUI);
      Vision::Callbacks.OnRenderHook.TriggerCallbacks(&data);

      // Render buffered debug lines (but do not reset buffer). If we have a renderer node, this has to be done in the final-stage post processor instead.
      Vision::Game.RenderDebugGeometry(VDGRM_ALL);

      // Render all mesh buffer objects with the render order flag "VRH_AFTER_RENDERING"
      RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_AFTER_RENDERING, m_bTriggerCallbacks);


      Vision::Message.HandleMessages();

      if (Vision::GetConsoleManager()->IsVisible())
        Vision::GetConsoleManager()->Render();
    }
  }

  m_pShaderProvider = NULL;
}




////////////////////////////////////////////////////////////////////////////////////////
// Additional Render Loop methods which are called from VisionRenderLoop_cl::OnDoRenderLoop.
////////////////////////////////////////////////////////////////////////////////////////


// Render shaders on entities (either pre-basebass or post-basepass)
void VisionRenderLoop_cl::DrawEntitiesShaders(const VisEntityCollection_cl &EntityCollection, VPassType_e ePassType, VTagFilter_e iTagFilter)
{
  INSERT_PERF_MARKER_SCOPE("VisionRenderLoop_cl::DrawEntitiesShaders");
  VisDrawCallInfo_t SurfaceShaderList[RLP_MAX_ENTITY_SURFACESHADERS];
  unsigned int iNumEntities = EntityCollection.GetNumEntries();
  unsigned int i;

  Vision::RenderLoopHelper.BeginEntityRendering();
  for (i=0; i<iNumEntities; i++)
  {
    VisBaseEntity_cl *pEntity = EntityCollection.GetEntry(i);
    if (iTagFilter==VTF_IGNORE_TAGGED_ENTRIES && pEntity->IsTagged())
      continue;
 
    if ( pEntity->HasShadersForPass(ePassType) )
    {
      // Get a list of the corresponding pre or post basepass surface shaders
      VisShaderSet_cl *pShaderSet = pEntity->GetActiveShaderSet();
      if (pShaderSet == NULL) continue;
      int iNumSurfaceShaders = pShaderSet->GetShaderAssignmentList(SurfaceShaderList, ePassType, RLP_MAX_ENTITY_SURFACESHADERS);
      VASSERT(iNumSurfaceShaders < RLP_MAX_ENTITY_SURFACESHADERS);
      //unsigned int iNumSurfaceShaders = pEntity->GetSurfaceShaderList(SurfaceShaderList, RLP_MAX_ENTITY_SURFACESHADERS, (BOOL)bPreBasepass);
      if (iNumSurfaceShaders == 0) continue;
      
      VDynamicMesh *pMesh = pEntity->GetMesh();

      // If the model has lit surfaces, and if the shaders makes use of the lighting information, we need to set up
      // the global lights.
      if (pMesh != NULL && pMesh->HasLitSurfaces() && 
        (pShaderSet->GetCombinedTrackingMask() & (VSHADER_TRACKING_LIGHTGRID_PS|VSHADER_TRACKING_LIGHTGRID_GS|VSHADER_TRACKING_LIGHTGRID_VS)) )
      {
         Vision::RenderLoopHelper.TrackLightGridInfo(pEntity);
      }
      // Render the entity with the surface shader list
      Vision::RenderLoopHelper.RenderEntityWithSurfaceShaderList(pEntity, iNumSurfaceShaders, SurfaceShaderList);
    }
  }
  Vision::RenderLoopHelper.EndEntityRendering();
}

// Masks out foreground entities (i.e. entities which have been flagged as "always in foreground")
void VisionRenderLoop_cl::MaskOutForegroundEntities(const VisEntityCollection_cl &EntityCollection)
{
  unsigned int iNumEntities = EntityCollection.GetNumEntries(); // this collection only contains foreground objects
  if (m_spForegroundMaskTechnique==NULL || iNumEntities==0)
    return;

  unsigned int i;
  const hkvMat4* pLastProj = NULL;
  INSERT_PERF_MARKER_SCOPE("VisionRenderLoop_cl::MaskOutForegroundEntities");

  Vision::RenderLoopHelper.BeginEntityRendering();

  for (i=0; i<iNumEntities; i++)
  {
    VisBaseEntity_cl *pEntity = EntityCollection.GetEntry(i);
    VASSERT_MSG(pEntity->IsObjectAlwaysInForegroundEnabled(), "Only entities with this flag should be passed to this function");
    if (!pEntity->HasShadersForPass(VPT_PrimaryOpaquePass))
      continue;

    const hkvMat4* pThisProj = pEntity->GetCustomProjectionMatrixForForegroundObject();
    if (pThisProj!=pLastProj)
    {
      VisRenderStates_cl::SetCurrentProjectionMatrix(pThisProj);
      pLastProj = pThisProj;
    }

    // depth fill pass
    Vision::RenderLoopHelper.RenderEntityWithShaders(pEntity, m_spForegroundMaskTechnique->GetShaderCount(), m_spForegroundMaskTechnique->GetShaderList());
  }

  Vision::RenderLoopHelper.EndEntityRendering();

  // reset to context projection matrix
  if (pLastProj)
  {
    VisRenderStates_cl::SetCurrentProjectionMatrix(NULL);
  }
}


// Renders foreground entities (i.e. entities which have been flagged as "always in foreground")
void VisionRenderLoop_cl::DrawForegroundEntities(const VisEntityCollection_cl &EntityCollection)
{
  unsigned int iNumEntities = EntityCollection.GetNumEntries(); // this collection only contains foreground objects
  if (iNumEntities==0 || m_spForegroundFillPassTechnique==NULL)
    return;

  INSERT_PERF_MARKER_SCOPE("VisionRenderLoop_cl::DrawForegroundEntities");

  unsigned int i;
  const hkvMat4* pLastProj = NULL;

  Vision::RenderLoopHelper.BeginEntityRendering();
  const int iPassCount = m_spForegroundFillPassTechnique->GetShaderCount();
  for (int iPass=0;iPass<=iPassCount;iPass++) // +1 passes, where the last one is the actual material pass
  {
    for (i=0; i<iNumEntities; i++)
    {
      VisBaseEntity_cl *pEntity = EntityCollection.GetEntry(i);

      // Render only Entities that are flagged as "always in foreground"
      VASSERT_MSG(pEntity->IsObjectAlwaysInForegroundEnabled(), "Only entities with this flag should be passed to this function");

      VDynamicMesh *pMesh = pEntity->GetMesh();
      VisShaderSet_cl *pShaderSet = pEntity->GetActiveShaderSet();

      VASSERT(pMesh && pShaderSet);

      const hkvMat4* pThisProj = pEntity->GetCustomProjectionMatrixForForegroundObject();

      if (pThisProj != pLastProj)
      {
        VisRenderStates_cl::SetCurrentProjectionMatrix(pThisProj);
        pLastProj = pThisProj;
      }

      if (iPass<iPassCount) // depth fill pass
      {
        VCompiledShaderPass *pPass = m_spForegroundFillPassTechnique->GetShader(iPass);
        Vision::RenderLoopHelper.RenderEntityWithShaders(pEntity, 1, &pPass);
      }
      else // material pass
      {
        const VisDrawCallInfo_t *pAssignment;

        int iNumSurfaceShaders = pShaderSet->GetShaderAssignmentList(&pAssignment);

        // If the shaders make use of the lighting information, we need to track the light grid
        if (pMesh != NULL && pMesh->HasLitSurfaces() && 
          (pShaderSet->GetCombinedTrackingMask() & (VSHADER_TRACKING_LIGHTGRID_PS|VSHADER_TRACKING_LIGHTGRID_GS|VSHADER_TRACKING_LIGHTGRID_VS)) )
        {
          Vision::RenderLoopHelper.TrackLightGridInfo(pEntity);
        }

        // Render the entity with the surface shader list
        Vision::RenderLoopHelper.RenderEntityWithSurfaceShaderList(pEntity, iNumSurfaceShaders, pAssignment);
      }
    }
  }

  Vision::RenderLoopHelper.EndEntityRendering();

  // reset to context projection matrix
  if (pLastProj)
  {
    VisRenderStates_cl::SetCurrentProjectionMatrix(NULL);
  }
}

// Returns an int value containing the flags indicating which geometry types receive light
int VisionRenderLoop_cl::GetLightReceiverFlags(const VisLightSource_cl *pLight)
{
  int iReceiverFlags = 0;
  iReceiverFlags |= VIS_LIGHTSRCVIS_MODELS;
  iReceiverFlags |= VIS_LIGHTSRCVIS_PRIMITIVES;
  return iReceiverFlags;
}


void VisionRenderLoop_cl::DrawDynamicLight()
{
  INSERT_PERF_MARKER_SCOPE("VisionRenderLoop_cl::DrawDynamicLight");

  // Some local variables for storing surfaces, shaders, surface shaders, and the like.
  VisDrawCallInfo_t SurfaceShaderList[RLP_MAX_ENTITY_SURFACESHADERS];
  VCompiledTechnique *pTechnique = NULL;
  VisRenderContext_cl *pContext = VisRenderContext_cl::GetCurrentContext();

  // Get all visible light sources
  IVisVisibilityCollector_cl *pVisColl = pContext->GetVisibilityCollector();
  if (pVisColl == NULL)
    return;
  const VisLightSrcCollection_cl *pLightSourceCollection = pVisColl->GetVisibleLights();
  if (pLightSourceCollection == NULL)
    return;

  unsigned int iNumLights = pLightSourceCollection->GetNumEntries();
  if (iNumLights == 0)
    return;

  const hkvVec3 vCamPos = pContext->GetCamera()->GetPosition();
  // For all visible lights...
  unsigned int i;
  for (i=0; i<iNumLights; i++)
  {
    VisLightSource_cl *pLight = pLightSourceCollection->GetEntry(i);
    // We're only interested in dynamic lights. Lights might not be far clipped by visibility collector to show the corona, so clip here
    if (!pLight->IsDynamic() || pLight->GetRadius()<=HKVMATH_LARGE_EPSILON || pLight->IsFarClipped(vCamPos)) 
      continue;

    // If the light intensity is not valid (intensity or its multiplier), skip it.
    if ( hkvMath::isZero(pLight->GetMultiplier()) || pLight->GetIntensity() <= HKVMATH_LARGE_EPSILON)
      continue;

    // Clear the collections of primitives and entities, since we want to build them from scratch for each light
    s_LitGeoInstanceCollection.Clear();
    s_LitEntityCollection.Clear();

    // See which geometry types have to cast shadows
    int iReceiverFlags = GetLightReceiverFlags(pLight);

    // If nothing receives light from this light source, we can proceed to the next light.
    if (!iReceiverFlags)
      continue;

    Vision::RenderLoopHelper.GetVisibleGeometryInLightsourceRange(&s_LitGeoInstanceCollection, &s_LitEntityCollection, NULL, *pLight);

    // For all illuminated entities: Render a dynamic lighting pass now.
    if (pLight->GetLightInfluenceBitMaskEntity())
    {
      VisRenderStates_cl::SetDepthStencilState(m_dynLightDefaultState);

      int j;

      int iNumLitEntities = s_LitEntityCollection.GetNumEntries();
      //      float fLastSpecular = -1.0f;

      Vision::RenderLoopHelper.BeginEntityRendering();

      for (j=0; j<iNumLitEntities; j++)
      {
        VisBaseEntity_cl *pEntity = s_LitEntityCollection.GetEntry(j);
        // Ignore foreground entities (they don't trivially support additive lighting)
        if (pEntity->IsObjectAlwaysInForegroundEnabled())
          continue;
        if (!(pEntity->GetLightInfluenceBitMask() & pLight->GetLightInfluenceBitMaskEntity()))
          continue;

        VDynamicMesh *pMesh = pEntity->GetMesh();

        int k;
        // Get list of all the surfaces in the model
        //int iNumSurfaces = pMesh->GetSurfacePtrList(pSurfaceList, RLP_MAX_ENTITY_SURFACES);
        int iNumSubmeshes = pMesh->GetSubmeshCount();
        int iNumSurfaceShaders = 0;
        VisSurface_cl **ppSurfaceArray = pEntity->GetSurfaceArray();

        // For all the surfaces...
        for (k=0; k<iNumSubmeshes; k++)
        {
          VDynamicSubmesh *pSubmesh = pMesh->GetSubmesh(k);
          VisSurface_cl *pSurface = ppSurfaceArray[pSubmesh->m_iMaterialIndex];
          pTechnique = m_pShaderProvider->GetDynamicLightShader(pLight, pSurface, true);
          if (!pTechnique || !pTechnique->GetShaderCount() || pSurface->IsFullbright()) // do not light this surface
            continue;



          // Generate a list of surface shader from the combined surface/shader information
          VisDrawCallInfo_t &drawcall(SurfaceShaderList[iNumSurfaceShaders++]);
          drawcall.Set(pSubmesh, pSurface, pTechnique->m_Shaders.GetAt(0));
        }
        // Finally, render the entity with a surface shader list.
        if (iNumSurfaceShaders>0)
          Vision::RenderLoopHelper.RenderEntityWithSurfaceShaderList(pEntity, iNumSurfaceShaders, SurfaceShaderList);
      }

      Vision::RenderLoopHelper.EndEntityRendering();

    }

    // For all occluding scene elements, we render the shadow volumes
    //bool bUseShadows = false;

    // For all illuminated world primitives: Render a dynamic lighting pass now.
    if (pLight->GetLightInfluenceBitMaskWorld()>0)
    {
      VisRenderStates_cl::SetDepthStencilState(m_dynLightDefaultState);

      int j;
      VisSurface_cl *pSurface;
      VisSurface_cl *pLastSurface = NULL;
      VCompiledTechnique *pLastTechnique = NULL;

      // We start collecting illuminated mesh instances. Whenever a relevant property changes, we set the
      // shader information, render all collected world instances, and start collecting from scratch.
      int iNumLitGeoInstances = s_LitGeoInstanceCollection.GetNumEntries();
      pLastSurface = NULL;
      s_RenderGeoInstanceCollection.Clear();
      pLastTechnique = NULL;

      // We start collecting illuminated world primitives. Whenever a relevant property changes, we set the
      // shader information, render all collected world primitives, and start collecting from scratch.
      for (j=0; j<iNumLitGeoInstances; j++)
      {
        VisStaticGeometryInstance_cl *pGI = s_LitGeoInstanceCollection.GetEntry(j);
        // we're not interested in full-bright primitives
        pSurface = pGI->GetSurface();
       // FIXME: HS#7461. Sometimes VEditableTerrainSectors return NULL here.
       // Disabled assert for now to fix the vForgeTests.
       // VASSERT(pSurface);
        
        if (pLastSurface!=pSurface)
        {
          pTechnique = m_pShaderProvider->GetDynamicLightShader(pLight, pSurface, false);
          pLastSurface = pSurface;
        }
        if (!pTechnique || !pTechnique->GetShaderCount() || pSurface->IsFullbright())
          continue;

        // If the state information is different from the previous one, we have to render the world primitives we
        // have collected so far
        if (pLastTechnique!=pTechnique)
        {
          if (s_RenderGeoInstanceCollection.GetNumEntries()!=0)
          {
            VASSERT(pLastTechnique != NULL);
            Vision::RenderLoopHelper.RenderStaticGeometryWithShader(s_RenderGeoInstanceCollection, *pLastTechnique->m_Shaders.GetAt(0));
            s_RenderGeoInstanceCollection.Clear();
          }
          // Update the stored state information
          pLastTechnique = pTechnique;
        }
        // Naturally, we have to append the primitive to our collection (otherwise it won't be collected =) ).
        s_RenderGeoInstanceCollection.AppendEntry(pGI);
      }
      // If there's still something left in the collection, render it as well.
      if (s_RenderGeoInstanceCollection.GetNumEntries()!=0)
      {
        if (pLastTechnique && pLastTechnique->GetShaderCount()>0)
          Vision::RenderLoopHelper.RenderStaticGeometryWithShader(s_RenderGeoInstanceCollection, *pLastTechnique->m_Shaders.GetAt(0));
        s_RenderGeoInstanceCollection.Clear();
      }
    }
    // Disable stenciled rendering
    VisRenderStates_cl::SetDepthStencilState(*VisRenderStates_cl::GetDepthStencilDefaultState());
  }
}


///////////////////////////////////////////////////////////////////////////////////////////
// lightgrid and dot3 lightmapping related functions
///////////////////////////////////////////////////////////////////////////////////////////


VCompiledEffect* VisionShaderProvider_cl::CreateMaterialEffect(VisSurface_cl *pSurface, int iFlags)
{
  const char* szEffectName = "vMaterial";
  char szParamStr[1024];
  szParamStr[0] = '\0';

  int iCreateFlags = EFFECTCREATEFLAG_NONE;

  // default colors for model resp. world
  if (!pSurface->IsFullbright() && !(iFlags & VisSurface_cl::VSF_NoDefaultShaders))
  {
    if ( ( pSurface->IsLightMapped() && !Vision::RenderLoopHelper.HasLightmaps() )
      || ( pSurface->GetLightingMode() == VIS_LIGHTING_LIGHTGRID && !Vision::RenderLoopHelper.HasLightGrid() ) )
    {
      bool bUseAlphaTest = pSurface->GetTransparencyType()==VIS_TRANSP_COLORKEY || pSurface->GetTransparencyType()==VIS_TRANSP_ALPHA;
      return GetDefaultLightingColorEffect(bUseAlphaTest, pSurface->GetAlphaTestThreshold(), pSurface->IsDoubleSided(), pSurface->IsDepthWriteEnabled());
    }
  }

  // valid for all types
  GetDefaultParamStringForSurface(pSurface, szParamStr);

  char szLibName[FS_MAX_PATH];
  sprintf (szLibName, "\\Shaders\\%s.ShaderLib", szEffectName);

  VShaderEffectLib* pFXLib = Vision::Shaders.LoadShaderLibrary (szLibName, SHADERLIBFLAG_HIDDEN);

  if (!pFXLib)
    Vision::Error.Warning ("Failed to load shader library '%s'",szLibName);


  VCompiledEffect *pFX = Vision::Shaders.CreateEffect (szEffectName, szParamStr, iCreateFlags, pFXLib);

  if (!pFX)
    Vision::Error.Warning ("Failed to create effect '%s'",szEffectName);

  return pFX;
}


void VisionShaderProvider_cl::CreateLightingShaderForModel(VDynamicMesh *pMesh, bool bIncludeManualAssignments)
{
  VASSERT(pMesh!=NULL);

  if (!pMesh->GetAllowShaderAssignment())
    return;

  VisShaderSet_cl *pSet = pMesh->GetShaderSet();
  if (!pSet)
  {
    pSet = new VisShaderSet_cl();
    pMesh->SetShaderSet(pSet);
  }

  const int iSurfCount = pMesh->GetSurfaceCount();
  for (int i=0;i<iSurfCount;i++)
  {
    VisSurface_cl *pSrf = pMesh->GetSurface(i);
    VASSERT(pSrf);
    pSrf->ReapplyShader(bIncludeManualAssignments);
  }

}


void VisionShaderProvider_cl::CreateLightingShaderForAllModels(bool bIncludeManualAssignments)
{
  VResourceManager *pMeshManager = VMeshManager::GetMeshManager();
  VASSERT(pMeshManager);
  int iModelCount = pMeshManager->GetResourceCount();
  for (int i=0;i<iModelCount;i++)
  {
    VBaseMesh *pMesh = (VBaseMesh *)pMeshManager->GetResourceByIndex(i);
    if (pMesh != NULL && pMesh->GetMeshType() == VMESH_DYNAMICMESH)
    {
      CreateLightingShaderForModel((VDynamicMesh*)pMesh, bIncludeManualAssignments);
    }
  }
}



void VisionShaderProvider_cl::CreateLightingShaderForWorld(bool bIncludeManualAssignments)
{
  VisResourceManager_cl &pRM = VisStaticMesh_cl::GetResourceManager();
  int iNumMeshes = pRM.GetResourceCount();
  int i,j;
  for (i=0; i<iNumMeshes; i++)
  {
    VBaseMesh *pBaseMesh = (VBaseMesh *)pRM.GetResourceByIndex(i);
    if (pBaseMesh == NULL)
      continue;
    if (pBaseMesh->GetMeshType() == VMESH_DYNAMICMESH)
      continue;
    VisStaticMesh_cl *pMG = (VisStaticMesh_cl *)pBaseMesh;
    if (!pMG)
      continue;
    int iNumSubmeshes = pMG->GetSubmeshCount();
    for (j=0; j<iNumSubmeshes; j++)
    {
      VisStaticSubmesh_cl *pSubmesh = pMG->GetSubmesh(j);
      VisSurface_cl *pSurface = pSubmesh->GetSurface();

      pSurface->ReapplyShader(bIncludeManualAssignments);
    }
  }
}

//////////////////////////////////////////////
//// VVisibilityObjectCollector             //
//////////////////////////////////////////////

VVisibilityObjectCollector::VVisibilityObjectCollector() :
m_ParticleGroupCollection(512,512),
m_MeshBufferObjectCollection(512,512)
{

}

VVisibilityObjectCollector::~VVisibilityObjectCollector()
{

}

void VVisibilityObjectCollector::HandleVisibleVisibilityObjects()
{
  m_MeshBufferObjectCollection.Clear();
  m_ParticleGroupCollection.Clear();

  VisRenderContext_cl *pContext = VisRenderContext_cl::GetCurrentContext();
  int iRenderFlags = pContext->GetRenderFilterMask();
  IVisVisibilityCollector_cl *pVisCollector = pContext->GetVisibilityCollector();
  if (pVisCollector==NULL)
    return;
  const VisVisibilityObjectCollection_cl *pVisObjectCollection = pVisCollector->GetVisibleVisObjects();

  int i;
  if (pVisObjectCollection != NULL && pVisObjectCollection->GetNumEntries()>0)
  {
    // Go through all visible visibility objects and add their particle systems/mesh buffer objects to the respective collections
    int iNumEntries = pVisObjectCollection->GetNumEntries();
    VisVisibilityObject_cl **pVisObjects = pVisObjectCollection->GetDataPtr();
    for (i=0; i<iNumEntries; i++, pVisObjects++)
    {
      int iNumObjects = (*pVisObjects)->GetObjectCount();
      VisObject3D_cl **pObjects = (*pVisObjects)->GetObjectList();
      for (int j=0; j<iNumObjects; j++, pObjects++)
      {
        if ((*pObjects)->GetSceneElementType() == VIS_SCENEELEMENTTYPE_PARTICLESYSTEM)
        {
          VisParticleGroup_cl *pGroup = vstatic_cast<VisParticleGroup_cl *>(*pObjects);
          if (!(pGroup->GetVisibleBitmask()&iRenderFlags)) continue;
          m_ParticleGroupCollection.AppendEntry(vstatic_cast<VisParticleGroup_cl *>(*pObjects));
        }
        else if ((*pObjects)->GetSceneElementType() == VIS_SCENEELEMENTTYPE_MESHBUFFEROBJECT)
        {
          VisMeshBufferObject_cl *pMBO = vstatic_cast<VisMeshBufferObject_cl *>(*pObjects);
          if (!(pMBO->GetVisibleBitmask()&iRenderFlags)) continue;
          m_MeshBufferObjectCollection.AppendEntry(vstatic_cast<VisMeshBufferObject_cl *>(*pObjects));
        }
        else
        {
          VASSERT(FALSE);
        }
      }
    }
  }

  // In order to maintain backwards compatibility, we also add all particle groups / mesh buffer objects WITHOUT visibility
  // objects. Ideally, you should have all particle groups/mesh buffer objects tied to visibility objects. In such cases, you
  // can simply skip the following step and save a bit of time that way.
  int iGroupCount = VisParticleGroup_cl::ElementManagerGetSize();
  for (i=0; i<iGroupCount; i++)
  {
    VisParticleGroup_cl *pGroup = VisParticleGroup_cl::ElementManagerGet(i);
    if (!pGroup)
      continue;
    if (pGroup->GetVisibilityObject()!=NULL)
      continue;
    if (!(pGroup->GetVisibleBitmask()&iRenderFlags))
      continue;
    m_ParticleGroupCollection.AppendEntry(pGroup);
  }

  int iMBOCount = VisMeshBufferObject_cl::ElementManagerGetSize();
  for (i=0; i<iMBOCount; i++)
  {
    VisMeshBufferObject_cl *pMBO = VisMeshBufferObject_cl::ElementManagerGet(i);
    if (!pMBO)
      continue;
    if (pMBO->GetVisibilityObject()!=NULL)
      continue;
    if (!(pMBO->GetVisibleBitmask()&iRenderFlags))
      continue;
    m_MeshBufferObjectCollection.AppendEntry(pMBO);
  }

  VisParticleGroup_cl::GetParticleGroupSorter().SortGroups(m_ParticleGroupCollection);
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
