/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Samples/Engine/RenderToTexture/RenderToTexturePCH.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class ThermalImageRenderLoop_cl
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// this class performs a modified renderloop that renders objects with a different set of shaders
class ThermalImageRenderLoop_cl : public VisionRenderLoop_cl
{
public:
  // Constructor / Destructor
  ThermalImageRenderLoop_cl()
  {
    LoadShaders();
    m_spFont = Vision::Fonts.LoadFont("Fonts\\Arial_22.fnt");

    Vision::Callbacks.OnReassignShaders += this;
  }
  virtual ~ThermalImageRenderLoop_cl()
  {
    Vision::Callbacks.OnReassignShaders -= this;
  }

  // Render Loop implementation
  virtual void OnDoRenderLoop(void *pUserData) HKV_OVERRIDE
  {
    IVisVisibilityCollector_cl *pCollector = Vision::Contexts.GetCurrentContext()->GetVisibilityCollector();
    // fall back to original renderloop
    if (pCollector==NULL || (m_spThermalImageLightmap==NULL && m_spThermalImageLightGrid==NULL))
    {
      VisionRenderLoop_cl::OnDoRenderLoop(pUserData);
      return;
    }

    Vision::RenderLoopHelper.ClearScreen();

    // render all visible entities
    const VisEntityCollection_cl *pEntities = pCollector->GetVisibleEntities();
    if (pEntities!=NULL && m_spThermalImageLightGrid!=NULL)
    {
      for (int i=0;i<(int)pEntities->GetNumEntries();i++)
        Vision::RenderLoopHelper.RenderEntityWithShaders(pEntities->GetEntry(i),
          m_spThermalImageLightGrid->GetShaderCount(), m_spThermalImageLightGrid->GetShaderList());
    }

    // render all visible static geometry (does not consider terrain!)
    const VisStaticGeometryInstanceCollection_cl *pStaticGeom = pCollector->GetVisibleStaticGeometryInstances();
    if (pStaticGeom!=NULL && m_spThermalImageLightmap!=NULL)
    {
      for (int i=0;i<m_spThermalImageLightmap->GetShaderCount();i++)
        Vision::RenderLoopHelper.RenderStaticGeometryWithShader(*pStaticGeom, *m_spThermalImageLightmap->GetShader(i));
    }

    // render some text overlay:
    hkvVec2 vPos(1.f,1.f);
    IVRender2DInterface *pRI = Vision::RenderLoopHelper.BeginOverlayRendering();
      m_spFont->PrintText(pRI, vPos, "Thermal Image", V_RGBA_WHITE);
    Vision::RenderLoopHelper.EndOverlayRendering();
  }

  // Callback Handler implementation
  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE
  {
    if (pData->m_pSender == &Vision::Callbacks.OnReassignShaders)
    {
      // Re-load shaders, application lifetime management does not handle this automatically.
      LoadShaders();
    }
    
    // Call VisionRenderLoop_cl callback handler
    VisionRenderLoop_cl::OnHandleCallback(pData);
  }

private:
  void LoadShaders()
  {
    // load shaders that should be used in this renderloop
    BOOL bRes = Vision::Shaders.LoadShaderLibrary("Shaders\\ThermalImage.ShaderLib") != NULL;
    VASSERT_MSG(bRes, "failed to load ThermalImage.ShaderLib");

    m_spThermalImageLightmap = Vision::Shaders.CreateTechnique("ThermalImageLightmap", NULL);
    m_spThermalImageLightGrid = Vision::Shaders.CreateTechnique("ThermalImageLightGrid", NULL);
  }

  VCompiledTechniquePtr m_spThermalImageLightmap;
  VCompiledTechniquePtr m_spThermalImageLightGrid;
  VisFontPtr m_spFont;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// class SecurityCamEntity_cl
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// pointer to the picked camera; to avoid picking multiple cameras
SecurityCamEntity_cl *SecurityCamEntity_cl::g_pCurrentlyPickedCam = NULL;

SecurityCamEntity_cl::SecurityCamEntity_cl()
{
  m_fFOV[0] = 90.f;
  m_fFOV[1] =  0.f; //vertical fov adapts to the rendertarget aspect ratio
  Vision::Callbacks.OnEnterForeground += this;
}

SecurityCamEntity_cl::~SecurityCamEntity_cl()
{
  Vision::Callbacks.OnEnterForeground -= this;
}

// ---------------------------------------------------------------------------------
// Method: GetCurrentAngles
// Notes: converts rotation matrix to euler angles
// ---------------------------------------------------------------------------------
void SecurityCamEntity_cl::GetCurrentAngles()
{
  GetRotationMatrix().getAsEulerAngles (m_fAngles[2], m_fAngles[1], m_fAngles[0]);
}

// ---------------------------------------------------------------------------------
// Method: SecurityCamEntity_cl
// Notes: Do the Entity initialisation
// ---------------------------------------------------------------------------------
void SecurityCamEntity_cl::InitFunction()
{
  m_fUpdatePos  = 0.f;
  Activate(m_bIsActive);
  GetCurrentAngles();
  m_fTurnPos = 0.f;

  // for picking:
  m_bIsPicked = false;

  int usesKeyboard = 0;
  #if defined(SUPPORTS_KEYBOARD)
  VisSampleApp::GetInputMap()->MapTrigger(RTT_PICK_CAMERA, V_KEYBOARD, CT_KB_ENTER, VInputOptions::OncePerFrame( ONCE_ON_PRESS, usesKeyboard));
    usesKeyboard = 1;
  #endif

  #if defined(_VISION_XENON) || (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))
    VisSampleApp::GetInputMap()->MapTrigger(RTT_PICK_CAMERA, V_XENON_PAD(0), CT_PAD_A, VInputOptions::OncePerFrame( ONCE_ON_PRESS, usesKeyboard));
  
  #elif defined(_VISION_PS3)
    VisSampleApp::GetInputMap()->MapTrigger(RTT_PICK_CAMERA, V_PS3_PAD(0), CT_PAD_CROSS, VInputOptions::OncePerFrame( ONCE_ON_PRESS, usesKeyboard));
  
  #elif defined(_VISION_PSP2)
    VisSampleApp::GetInputMap()->MapTrigger(RTT_PICK_CAMERA, V_PSP2_PAD(0), CT_PAD_CROSS, VInputOptions::OncePerFrame( ONCE_ON_PRESS, usesKeyboard));
  
  #elif defined(_VISION_MOBILE)
    int width = Vision::Video.GetXRes();
    
    VTouchArea* pickArea = new VTouchArea(VInputManager::GetTouchScreen(), VRectanglef(width - 100, 0, width, 100), 0.0f);
    VisSampleApp::GetInputMap()->MapTrigger(RTT_PICK_CAMERA, pickArea, CT_TOUCH_ANY, VInputOptions::Once());

  #elif defined(_VISION_WIIU)
    VisSampleApp::GetInputMap()->MapTrigger(RTT_PICK_CAMERA, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_B, VInputOptions::OncePerFrame( ONCE_ON_PRESS, usesKeyboard));

  #endif

// create the render target:
  VisContextCamera_cl *pCamera = new VisContextCamera_cl();
  pCamera->SetPosition(GetPosition());
  pCamera->SetOrientation(GetOrientation());
  pCamera->AttachToEntity(this, hkvVec3(0,0,0));
  int iResolution = 512;
#if defined(_VISION_PSP2) || defined(_VISION_MOBILE)
  iResolution = 256;
#endif  

  VisRenderableTextureConfig_t config;
  config.m_iHeight = iResolution;
  config.m_iWidth  = iResolution;
  config.m_iMultiSampling = 0; // optionally enable MS here (e.g. set to 2, 4,...)
  config.m_bResolve = (config.m_iMultiSampling>0); // let the engine resolve it automatically
  
  #ifdef _VISION_XENON
    config.m_bResolve = true;
  #endif
  
  config.m_eFormat = VTextureLoader::DEFAULT_RENDERTARGET_FORMAT_32BPP;
  m_spRenderTarget =  Vision::TextureManager.CreateRenderableTexture("<Camera>",config);
  m_spRenderTarget->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);

  config.m_bResolve = false;
  config.m_eFormat = VVideo::GetSupportedDepthStencilFormat(VTextureLoader::D24S8, *Vision::Video.GetCurrentConfig());
  config.m_bRenderTargetOnly = true;
  config.m_bIsDepthStencilTarget = true;
  m_spDepthStencilTarget =  Vision::TextureManager.CreateRenderableTexture("<Camera_DS>",config);
  m_spDepthStencilTarget->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);

  hkvMathHelpers::adjustFovsForAspectRatio(m_fFOV[0], m_fFOV[1], float(config.m_iWidth) / float(config.m_iHeight));
  m_spRenderContext = new VisRenderContext_cl(pCamera, m_fFOV[0], m_fFOV[1], config.m_iWidth, config.m_iHeight, 5.f, 30000.f, VIS_RENDERCONTEXT_FLAGS_SECONDARYCONTEXT);
  m_spRenderContext->SetName("SecurityCamera");
  m_spRenderContext->SetRenderTarget(0, m_spRenderTarget); // main view, with clear screen
  m_spRenderContext->SetDepthStencilTarget(m_spDepthStencilTarget); // main view, with clear screen
  m_spRenderContext->SetPriority(VIS_RENDERCONTEXTPRIORITY_MIRROR);
  m_spRenderContext->SetRenderingEnabled(false);
  IVisVisibilityCollector_cl *pVisColl = new VisionVisibilityCollector_cl();
  m_spRenderContext->SetVisibilityCollector(pVisColl);
  m_spRenderContext->SetRenderLoop(new VisionRenderLoop_cl());
  
  // Prevents the debug text rendered by the message system from being rendered in this context
  m_spRenderContext->SetRenderFilterMask(~Vision::Message.GetVisibleBitmask());

  pVisColl->SetOcclusionQueryRenderContext(m_spRenderContext);
  Vision::Contexts.AddContext(m_spRenderContext);

  if (m_bUseThermalImage)
  {
    m_spRenderContext->SetRenderLoop(new ThermalImageRenderLoop_cl());
  }

  SetMonitorSurfaceName(m_szMonitorSrfName);
}

// ---------------------------------------------------------------------------------
// Method: DeInitFunction
// ---------------------------------------------------------------------------------
void SecurityCamEntity_cl::DeInitFunction()
{
  Vision::Contexts.RemoveContext(m_spRenderContext);
  m_spRenderContext = NULL;
  m_spRenderTarget = NULL;
  m_spDepthStencilTarget = NULL;
}


void SecurityCamEntity_cl::OnHandleCallback( IVisCallbackDataObject_cl* pData )
{
  if(pData->m_pSender == &Vision::Callbacks.OnEnterForeground)
  {
    // Re-apply security camera texture to monitor surface.
    SetMonitorSurfaceName(m_szMonitorSrfName);
  }
}


// ---------------------------------------------------------------------------------
// Method: ThinkFunction
// Notes: Do the periodic RenderToTexture here, also turn camera
// ---------------------------------------------------------------------------------
void SecurityCamEntity_cl::ThinkFunction()
{
  float dtime = Vision::GetTimer()->GetTimeDifference();

  ////////////////////////////////////////////////////
  // The following part is just for handling the picking
  ////////////////////////////////////////////////////
  if (m_bIsPicked)
  {
    if (VisSampleApp::GetInputMap()->GetTrigger(RTT_PICK_CAMERA))
      PickCamera(false);
  } else
  {
      // turn camera (idle)
    m_fTurnPos = hkvMath::mod (m_fTurnPos+dtime*m_fYawSpeed, hkvMath::pi () * 2.0f);
    SetOrientation(m_fAngles[0]+m_fYawAmplitude*hkvMath::sinRad (m_fTurnPos), m_fAngles[1], m_fAngles[2]);

    // check if it can be picked:
    hkvVec3 diff;
    Vision::Camera.GetCurrentCameraPosition(diff);
    diff -= GetPosition();
    if (!g_pCurrentlyPickedCam && diff.getLength()<80.f)
    {
      Vision::Message.Print(1,1,1,"Pick camera");
      if (VisSampleApp::GetInputMap()->GetTrigger(RTT_PICK_CAMERA))
        PickCamera();
    }
  }

  ////////////////////////////////////////////////////
  // Render each time interval
  ////////////////////////////////////////////////////
  if (m_fUpdateTime>0.f)
  {
    m_fUpdatePos += dtime;
    if (m_fUpdatePos>=m_fUpdateTime)
    {
      m_fUpdatePos = hkvMath::mod (m_fUpdatePos, m_fUpdateTime);
      SetRenderNextFrame(true);
    } else
      SetRenderNextFrame(false);
  } 
  else
    SetRenderNextFrame(true); // each frame
}



// ---------------------------------------------------------------------------------
// Method: Render
// Notes: Do the rendering
// ---------------------------------------------------------------------------------
void SecurityCamEntity_cl::SetRenderNextFrame(bool bStatus)
{
  m_spRenderContext->SetRenderingEnabled(bStatus);
}



// ---------------------------------------------------------------------------------
// Method: SetMonitorTextureName
// Notes: Get the texture information from texture filename
// ---------------------------------------------------------------------------------
void SecurityCamEntity_cl::SetMonitorSurfaceName(const char *szSurface)
{
  VisSurface_cl *pSrf = NULL;
  
  const char* szMeshName = "Crossing.vmesh";
  strcpy(m_szMonitorSrfName, szSurface);

  VMeshManager* MeshManager = VMeshManager::GetMeshManager();
  VisStaticMesh_cl* mesh = (VisStaticMesh_cl*)MeshManager->GetResourceByName(szMeshName);//@@@ hardcoded mesh name
  if(mesh)
    pSrf = mesh->GetSurfaceByName(m_szMonitorSrfName);

  VASSERT(pSrf);
  pSrf->m_spDiffuseTexture = m_spRenderTarget;
}


// ---------------------------------------------------------------------------------
// Method: Activate
// Notes: Activate or deactivate camera
// ---------------------------------------------------------------------------------
void SecurityCamEntity_cl::Activate(BOOL status)
{
  m_bIsActive = status;
  SetThinkFunctionStatus(m_bIsActive);
  m_fUpdatePos = 0.f;
  if (!status)
    SetRenderNextFrame(false);
}

// ---------------------------------------------------------------------------------
// Method: PickCamera
// Notes: pick or drop camera
// ---------------------------------------------------------------------------------
void SecurityCamEntity_cl::PickCamera(bool status)
{
  if (status==IsPicked()) return;
  m_bIsPicked = status;
  VisMouseCamera_cl *pCam = (VisMouseCamera_cl *)VisRenderContext_cl::GetMainRenderContext()->GetCamera()->GetParent();
  VASSERT(pCam);

  if (m_bIsPicked)
  {
    g_pCurrentlyPickedCam = this;
    AttachToParent( pCam );

    // setup local space transformation of security cam
    hkvVec3 localOri( 0, 0, 0 );
    SetLocalOrientation( localOri );
    SetLocalPosition( 35, 0, -25 );
  } else
  {
    DetachFromParent();
    if (g_pCurrentlyPickedCam==this) 
      g_pCurrentlyPickedCam = NULL;
    else 
      g_pCurrentlyPickedCam->PickCamera(false); // drop the old one
    GetCurrentAngles();
  }
}



V_IMPLEMENT_SERIAL( SecurityCamEntity_cl, VisBaseEntity_cl, 0, Vision::GetEngineModule() );
START_VAR_TABLE(SecurityCamEntity_cl, VisBaseEntity_cl, "SecurityCamEntity_cl", 0, "")  
  DEFINE_VAR_FLOAT(SecurityCamEntity_cl,  m_fUpdateTime,      "Update interval for rendering (0=each frame)", "0.05", 0,0);
  DEFINE_VAR_STRING(SecurityCamEntity_cl, m_szMonitorSrfName, "Texture filename of the monitor texture","MonitorScreen01_Mat", FS_MAX_PATH-1, 0,0);
  DEFINE_VAR_BOOL(SecurityCamEntity_cl,   m_bIsActive,        "camera active at startup","TRUE", 0,0);
  DEFINE_VAR_FLOAT(SecurityCamEntity_cl,  m_fFOV,             "field of view for rendering","90", 0,0); //only first float (horiz fov) is editable in editor
  DEFINE_VAR_FLOAT(SecurityCamEntity_cl,  m_fYawSpeed,        "Yaw speed for turning camera", "0.62832f", 0,0);
  DEFINE_VAR_FLOAT(SecurityCamEntity_cl,  m_fYawAmplitude,    "Yaw amplitude (degree)", "45", 0,0);
  DEFINE_VAR_BOOL(SecurityCamEntity_cl,   m_bUseThermalImage, "camera active at startup","FALSE", 0,0);
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
