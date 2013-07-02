/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/CubeMapHandle.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RendererNode/VRendererNodeCommon.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/ScratchTexturePool.hpp>

#define CUBEMAP_SERIAL_VERSION1         1
#define CUBEMAP_SERIAL_VERSION2         2
#define CUBEMAP_SERIAL_VERSION3         3
#define CUBEMAP_SERIAL_VERSION4         4
#define CUBEMAP_SERIAL_VERSION5         5
#define CUBEMAP_SERIAL_CURRENTVERSION   CUBEMAP_SERIAL_VERSION5

class FlipAndBlurRenderLoop_cl : public IVisRenderLoop_cl
{
public:
  FlipAndBlurRenderLoop_cl(CubeMapHandle_cl* pCubeMapHandle) :
      m_pCubeMapHandle(pCubeMapHandle)
      {
      }

  EFFECTS_IMPEXP void OnDoRenderLoop(void *pUserData) HKV_OVERRIDE
  {
    m_pCubeMapHandle->FlipAndBlur(m_iFace);
  }

  CubeMapHandle_cl* m_pCubeMapHandle;
  int m_iFace;
};

IVisRenderLoopPtr CubeMapHandle_cl::s_spDefaultRenderLoop;

// Workflow of cubemap rendering
//
// m_bActive = false: No dynamic cubemap rendering at all, contexts, techniques, callbacks and render targets aren't created.
// m_bActive = true: Contexts, techniques and render targets are created and registered.
//    m_bRenderingEnabled is true if and only if m_bContinuousUpdate || m_bUpdateForced. Callbacks are only registered if rendering is
//    enabled.
//    Every frame:
//      OnUpdateSceneFinished:
//        Compute face mask to be rendered.
//      BeginRendering:
//        If m_spRendererNode:
//          Perform renderer node for each active face, flip and blur
//        Else:
//          Enable context rendering
//      RenderHook (not RendererNode):
//        Flip and blur face
//      EndRendering:
//        Disable rendering if only a single update was forced.

CubeMapHandle_cl::CubeMapHandle_cl()
{
  m_bUpdateForced = false;     
  m_bRenderingEnabled = false; 
  m_iCurrentFace = -1;       
  m_iCurrentFaceMask = 0xff;   

  m_bActive = false;
  m_bGenMipMaps = FALSE;       
  m_iBlurPasses = 0;        
  m_iSize = 128;              
  m_bContinuousUpdate = FALSE; 
  m_fUpdateInterval = 1.0f;
  m_fElapsedTime = m_fUpdateInterval;
  m_bAlternateUpdate = false;  

  m_fNearClipPlane = 5.0f;
  m_fFarClipPlane = 32000.0f;
  m_iRenderFilterMask = VIS_ENTITY_VISIBLE_IN_CUBEMAP;

  m_bUpdateForced = false;
  m_bRenderingEnabled = false;

  m_iVisibleMask = ~VIS_ENTITY_VISIBLE_IN_CUBEMAP;
}

CubeMapHandle_cl::~CubeMapHandle_cl()
{
  SetActivate(false);
}

void CubeMapHandle_cl::InitContexts()
{
#ifdef SUPPORTS_RENDERABLE_CUBEMAPS

  float fMainNear, fMainFar;
  Vision::Contexts.GetMainRenderContext()->GetClipPlanes(fMainNear,fMainFar);

  float fNear = m_fNearClipPlane>0.f ? m_fNearClipPlane : fMainNear; 
  float fFar = m_fFarClipPlane>0.f ? m_fFarClipPlane : fMainFar; 

  IVisRenderLoop_cl *pRenderLoop = GetRenderLoop();
  VASSERT(pRenderLoop);

  for (int i=0; i<6; i++)
  {
    if (m_spRenderContext[i]!=NULL)
      continue;
    m_spRenderContext[i] = new VisRenderContext_cl();
    VisContextCamera_cl *pCam = new VisContextCamera_cl();
    m_spRenderContext[i]->SetCamera(pCam);
    m_spRenderContext[i]->SetClipPlanes(fNear, fFar);
    m_spRenderContext[i]->SetShaderFlags(VIS_RENDERSHADER_ALL_TEXTURE);
    m_spRenderContext[i]->SetRenderFilterMask(m_iRenderFilterMask);
    m_spRenderContext[i]->SetRenderLoop(pRenderLoop);
    m_spRenderContext[i]->SetPriority(VIS_RENDERCONTEXTPRIORITY_MIRROR);
    m_spRenderContext[i]->SetRenderFlags(0);

    m_spRenderContext[i]->SetFOV(90.0f, 90.0f);

    m_spRenderContext[i]->SetName("CubeMapHandler");

    if (m_spCubemap != NULL && m_spDepthTarget != NULL)
    {
      m_spRenderContext[i]->SetRenderTarget(0, m_spBlurTarget[0]);
      m_spRenderContext[i]->SetDepthStencilTarget(m_spDepthTarget);
      m_spRenderContext[i]->SetViewport(0, 0, m_iSize, m_iSize);
    }

    m_spRenderContext[i]->SetRenderingEnabled(false);

    VisionVisibilityCollector_cl *pVisColl = new VisionVisibilityCollector_cl();
    m_spRenderContext[i]->SetVisibilityCollector(pVisColl);
    pVisColl->SetOcclusionQueryRenderContext(m_spRenderContext[i]);

    Vision::Contexts.AddContext(m_spRenderContext[i]);
  }

  SetContextOrientation();

#endif // SUPPORTS_RENDERABLE_CUBEMAPS

}

void CubeMapHandle_cl::DeInitContexts()
{
#ifdef SUPPORTS_RENDERABLE_CUBEMAPS

  for (int i=0; i<6; i++)
  {
    Vision::Contexts.RemoveContext(m_spRenderContext[i]);
    m_spRenderContext[i] = NULL;
  }

#endif // SUPPORTS_RENDERABLE_CUBEMAPS
}


void CubeMapHandle_cl::EnableRendering()
{
#ifdef SUPPORTS_RENDERABLE_CUBEMAPS

  VASSERT(m_bActive && (m_bUpdateForced || m_bContinuousUpdate));

  if(m_bRenderingEnabled)
  {
    return;
  }

  Vision::Callbacks.OnUpdateSceneFinished += this;
  Vision::Callbacks.BeginRendering += this;
  Vision::Callbacks.EndRendering += this;
  Vision::Callbacks.OnRenderHook += this;

  m_bRenderingEnabled = true;

#endif // SUPPORTS_RENDERABLE_CUBEMAPS
}

void CubeMapHandle_cl::DisableRendering()
{
#ifdef SUPPORTS_RENDERABLE_CUBEMAPS

  VASSERT(!m_bActive || (!m_bUpdateForced && !m_bContinuousUpdate));

  if(!m_bRenderingEnabled)
  {
    return;
  }

  for (int i=0; i<6; i++)
  {
    m_spRenderContext[i]->SetRenderingEnabled(false);
  }

  Vision::Callbacks.OnRenderHook -= this;
  Vision::Callbacks.EndRendering -= this;
  Vision::Callbacks.BeginRendering -= this;
  Vision::Callbacks.OnUpdateSceneFinished -= this;

  m_bRenderingEnabled = false;

#endif // SUPPORTS_RENDERABLE_CUBEMAPS
}

bool CubeMapHandle_cl::WasRecentlyRendered() const
{
  for (int i=0; i<6; i++)
  {
    if (m_spRenderContext[i]!=NULL)
    {
      if (m_spRenderContext[i]->WasRecentlyRendered())
      {
        return true;
      }
    }
  }
  return false;
}

void CubeMapHandle_cl::SetRenderFilterMask(int iMask)
{
  m_iRenderFilterMask = iMask;
  for (int i=0; i<6; i++)
  {
    if (m_spRenderContext[i]!=NULL)
    {
      m_spRenderContext[i]->SetRenderFilterMask(m_iRenderFilterMask);
}
  }
}

void CubeMapHandle_cl::SetRenderLoop(IVisRenderLoop_cl *pLoop)
{
  m_spRenderLoop = pLoop;

  if(pLoop == NULL)
  {
    return;
  }

  // update on contexts as well
  for (int i=0; i<6; i++)
  {
    if (m_spRenderContext[i])
    {
      m_spRenderContext[i]->SetRenderLoop(pLoop);
    }
  }
}

IVisRenderLoop_cl* CubeMapHandle_cl::GetRenderLoop()
{
  if (!m_spRenderLoop)
  {
    if (s_spDefaultRenderLoop != NULL)
    {
      m_spRenderLoop = s_spDefaultRenderLoop;
    }
    else
    {
      m_spRenderLoop = new VisionRenderLoop_cl;
  }
  }
  return m_spRenderLoop;
}

void CubeMapHandle_cl::SetRendererNode(IVRendererNode* pRendererNode)
{
  m_spRendererNode = pRendererNode;
  SetContextOrientation();
}

IVRendererNode* CubeMapHandle_cl::GetRendererNode()
{
  return m_spRendererNode;
}

void CubeMapHandle_cl::SetClipPlanes(float fNear, float fFar) 
{ 
  m_fNearClipPlane = fNear; 
  m_fFarClipPlane = fFar;

  float fMainNear, fMainFar;
  Vision::Contexts.GetMainRenderContext()->GetClipPlanes(fMainNear, fMainFar);

  if (fNear <= 0.0f)
    fNear = fMainNear;

  if (fFar <= 0.0f)
    fFar = fMainFar;

  for (int i = 0; i < 6; i++)
  {
    if (m_spRenderContext[i] != NULL)
    {
      m_spRenderContext[i]->SetClipPlanes(fNear, fFar);
    }
  }
}

void CubeMapHandle_cl::SetContinuousUpdate(bool bStatus)
{
  m_bContinuousUpdate = bStatus; 

  if(bStatus)
  {
    EnableRendering();
    }
  else if(!m_bUpdateForced)
  {
    DisableRendering();
  }
}


void CubeMapHandle_cl::InitFunction()
{
  m_fElapsedTime = m_fUpdateInterval;
  SetActivate(true);
}



void CubeMapHandle_cl::DeInitFunction()
{
#ifdef SUPPORTS_RENDERABLE_CUBEMAPS

  SetActivate(false);
  if (m_spCubemap)
  {
    m_spCubemap->EnableUnloading();
    m_spCubemap->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
    m_spCubemap = NULL;
  }
  if (m_spDepthTarget)
  {
    m_spDepthTarget->EnableUnloading();
    m_spDepthTarget->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
    m_spDepthTarget = NULL;
  }
  SetBlurPasses(0);

#endif // SUPPORTS_RENDERABLE_CUBEMAPS
}


void CubeMapHandle_cl::SetCubemapKey(const char *szKey, int iEdgeSize)
{
  if (!szKey || !szKey[0])
    szKey = "<Cubemap>";

  SetEntityKey(szKey);

#ifdef SUPPORTS_RENDERABLE_CUBEMAPS

  for (int i=0; i<6; i++)
  {
    if (m_spRenderContext[i]!=NULL)
    {
      m_spRenderContext[i]->SetRenderTarget(0, NULL);
      m_spRenderContext[i]->SetDepthStencilTarget(NULL);
      m_spRenderContext[i]->SetViewport(0, 0, m_iSize, m_iSize);
    }
  }

  int iOldSize = m_iSize;
  VisRenderableCubeMap_cl *pOldCubemap = m_spCubemap;
  VisRenderableTexture_cl *pOldDepthTarget = m_spDepthTarget;

  if (m_spCubemap)
  {
    m_spCubemap->EnableUnloading();
    m_spCubemap->EnsureUnloaded();
    m_spCubemap = NULL;
  }
  if (m_spDepthTarget)
  {
    m_spDepthTarget->EnableUnloading();
    m_spDepthTarget->EnsureUnloaded();
    m_spDepthTarget = NULL;
  }
  m_iSize = iEdgeSize;

  VisRenderableTextureConfig_t config;
  config.m_iHeight = config.m_iWidth = m_iSize;
#if defined(_VR_DX11)
  config.m_eFormat = VTextureLoader::B8G8R8A8;
#else
  config.m_eFormat = VTextureLoader::DEFAULT_RENDERTARGET_FORMAT_32BPP;
#endif
  if (m_bGenMipMaps)
  {
    config.m_bCreateMipmaps = true;
  }


#ifdef _VISION_PS3
  config.m_bUseTiledMemory = false;
  config.m_bUseCompression = false;
#endif


  m_spCubemap = Vision::TextureManager.CreateRenderableCubeMap(szKey, config);

  config.m_eFormat = VVideo::GetSupportedDepthStencilFormat(VTextureLoader::D24S8, *Vision::Video.GetCurrentConfig());
  config.m_bRenderTargetOnly = true;
  config.m_bIsDepthStencilTarget = true;
  config.m_bCreateMipmaps = false;    // Never generate mipmaps for depth-stencil targets!

  m_spDepthTarget = ScratchTexturePool_cl::GlobalManager().GetScratchTexture(config);

  m_spCubemap->EnsureLoaded();
  m_spDepthTarget->EnsureLoaded();

  for (int i=0; i<6; i++)
  {
    if (m_spRenderContext[i]!=NULL)
    {
      m_spRenderContext[i]->SetRenderTarget(0, m_spBlurTarget[0]);
      m_spRenderContext[i]->SetDepthStencilTarget(m_spDepthTarget);
      m_spRenderContext[i]->SetViewport(0, 0, m_iSize, m_iSize);
    }
  }

  if (pOldCubemap!=m_spCubemap || iOldSize!=m_iSize)
  {
    if(m_bActive)
    {
    CreateBlurTarget();
    Invalidate();
  }
  }
#endif
}




void CubeMapHandle_cl::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
#ifdef SUPPORTS_RENDERABLE_CUBEMAPS

  if (pData->m_pSender==&Vision::Callbacks.OnUpdateSceneFinished)
  {
    UpdateFaceStatus();
    return;
  }
  else if(pData->m_pSender == &Vision::Callbacks.BeginRendering)
  {
    for (int i=0; i<6; i++)
    {
      if((m_iCurrentFaceMask & (1 << i)) == 0)
      {
        m_spRenderContext[i]->SetRenderingEnabled(false);
        continue;
      }

      m_spRenderContext[i]->GetCamera()->SetPosition(GetPosition());

      // If we have a renderer node, set it up for the context, render, and blur

      // If we have a single render loop, register the context to let it be rendered later,
      // and blur in the render hook

      if(m_spRendererNode)
      {
        char perfMarker[32];
        sprintf(perfMarker, "CubeMapHandle Face %d", i);

        INSERT_PERF_MARKER_SCOPE(perfMarker);

        VRendererNodeCommon* pNodeBase = vdynamic_cast<VRendererNodeCommon*>(m_spRendererNode);

        m_spRendererNode->DeInitializeRenderer();
        m_spRendererNode->SetFinalTargetContext(m_spRenderContext[i]);
        m_spRendererNode->InitializeRenderer();

        int iNodeIdx;
        for(iNodeIdx = 0; iNodeIdx < V_MAX_RENDERER_NODES; iNodeIdx++)
        {
          if(Vision::Renderer.GetRendererNode(iNodeIdx) == NULL)
          {
            Vision::Renderer.SetRendererNode(iNodeIdx, m_spRendererNode);
            break;
          }
        }

        VASSERT_MSG(iNodeIdx != V_MAX_RENDERER_NODES, "No renderer node index available for cubemap rendering!");

        m_spRendererNode->Execute();

        Vision::Renderer.SetRendererNode(iNodeIdx, NULL);

        if(m_iBlurPasses == 0)
        {
          m_spBlurContext[0]->SetRenderTargetCubeMap(0, m_spCubemap, i);
        }

        m_spBlurContext[0]->Activate();
        VisRenderContext_cl::PerformPendingContextSwitch();

        vstatic_cast<FlipAndBlurRenderLoop_cl*>(m_spBlurContext[0]->GetRenderLoop())->m_iFace = i;
        m_spBlurContext[0]->Execute();

        m_spBlurContext[0]->SetRenderTarget(0, m_spBlurTarget[0]);
      }
      else
      {
        m_spRenderContext[i]->SetRenderingEnabled(true);
      }
      }

#if defined(_VR_DX11)
    if(m_spRendererNode)
    {
      if ((m_iCurrentFaceMask & (1 << 6)) != 0 && m_bGenMipMaps)
      {
        m_spCubemap->CreateMipmaps();
    }
  }
#endif

  }
  else if (pData->m_pSender == &Vision::Callbacks.OnRenderHook && m_spRendererNode == NULL)
  {
    VASSERT(m_bRenderingEnabled);
    if(static_cast<VisRenderHookDataObject_cl *>(pData)->m_iEntryConst == VRH_PRE_SCREENMASKS)
    {
      for (int i=0; i<6; i++)
      {
        if (m_spRenderContext[i] == NULL || Vision::Contexts.GetCurrentContext() != m_spRenderContext[i])
          continue;

        FlipAndBlur(i);
      }

#if defined(_VR_DX11)
      if ((m_iCurrentFaceMask & (1 << 6)) != 0 && m_bGenMipMaps)
        {
        m_spCubemap->CreateMipmaps();
        }
#endif
      }
    }
  else if(pData->m_pSender == &Vision::Callbacks.EndRendering)
  {
    if(m_bUpdateForced)
    {
      m_bUpdateForced = false;
      if(!m_bContinuousUpdate)
      {
        DisableRendering();
      }
    }
  }
  else if (pData->m_pSender==&Vision::Callbacks.OnLeaveBackground)
  {
    Invalidate();
  }
  else if (pData->m_pSender==&Vision::Callbacks.OnReassignShaders)
  {
    if (m_iBlurPasses!=0)
    {
      CreateBlurTechnique();
    }
  }

#endif // SUPPORTS_RENDERABLE_CUBEMAPS

}

void CubeMapHandle_cl::SetAutoGenMipMaps(bool bStatus)
{
  if ((BOOL)bStatus != m_bGenMipMaps)
  {
    m_bGenMipMaps = (BOOL)bStatus;
    SetCubemapKey(GetEntityKey(), m_iSize);
  }
}

bool CubeMapHandle_cl::GetAutoGenMipMaps() const
{
  return m_bGenMipMaps == TRUE;
}


void CubeMapHandle_cl::SetBlurPasses(int iPasses)
{
#ifdef SUPPORTS_RENDERABLE_CUBEMAPS

  VASSERT(iPasses >= 0);

  if (m_iBlurPasses == 0 && iPasses != 0)
  {
    CreateBlurTechnique();
  }

#endif // SUPPORTS_RENDERABLE_CUBEMAPS

  m_iBlurPasses = iPasses;
}

int CubeMapHandle_cl::GetBlurPasses() const
{
  return m_iBlurPasses;
}

void CubeMapHandle_cl::CreateBlurTarget()
{
  VASSERT(m_bActive);
#ifdef SUPPORTS_RENDERABLE_CUBEMAPS
  VisRenderableTextureConfig_t config;
  config.m_iWidth = config.m_iHeight = m_iSize;
#if defined(_VR_DX11)
  config.m_eFormat = VTextureLoader::B8G8R8A8;
#else
  config.m_eFormat = VTextureLoader::DEFAULT_RENDERTARGET_FORMAT_32BPP;
#endif

  m_spBlurTarget[0] = ScratchTexturePool_cl::GlobalManager().GetScratchTexture(config, 0);
  m_spBlurTarget[1] = ScratchTexturePool_cl::GlobalManager().GetScratchTexture(config, 1);

  for (int i=0; i<6; i++)
  {
    m_spRenderContext[i]->SetRenderTarget(0, m_spBlurTarget[0]);
    m_spRenderContext[i]->SetDepthStencilTarget(m_spDepthTarget);
    m_spRenderContext[i]->SetViewport(0, 0, m_iSize, m_iSize);
  }

  m_spBlurContext[0] = new VisRenderContext_cl();
  m_spBlurContext[0]->SetCamera(m_spRenderContext[0]->GetCamera());
  m_spBlurContext[0]->SetRenderTarget(0, m_spBlurTarget[0]);
  m_spBlurContext[0]->SetRenderFlags(0);
  m_spBlurContext[0]->SetRenderFilterMask(0);
  m_spBlurContext[0]->SetVisibilityCollector(NULL);
  m_spBlurContext[0]->SetRenderLoop(new FlipAndBlurRenderLoop_cl(this));

  m_spBlurContext[1] = new VisRenderContext_cl();
  m_spBlurContext[1]->SetCamera(m_spBlurContext[0]->GetCamera());
  m_spBlurContext[1]->SetRenderTarget(0, m_spBlurTarget[1]);
  m_spBlurContext[1]->SetRenderFlags(0);
  m_spBlurContext[1]->SetRenderFilterMask(0);
  m_spBlurContext[1]->SetVisibilityCollector(NULL);
#endif // SUPPORTS_RENDERABLE_CUBEMAPS
}


void CubeMapHandle_cl::CreateBlurTechnique()
{
  if (!Vision::Shaders.LoadShaderLibrary("\\Shaders\\PostProcess.ShaderLib", SHADERLIBFLAG_HIDDEN))
    Vision::Error.FatalError("Shader lib file required for cube map blurring (PostProcess.ShaderLib) could not be loaded!");

  m_spBlurTechnique = Vision::Shaders.CreateTechnique("Blur", "");
  m_blurStepRegister.Init(m_spBlurTechnique->GetShader(0), "StepSize");
}


void CubeMapHandle_cl::DestroyBlurTarget()
{
  m_spBlurTarget[0] = m_spBlurTarget[1] = NULL;
  m_spBlurContext[0] = m_spBlurContext[1] = NULL;
}

void CubeMapHandle_cl::FlipAndBlur(int iFace)
{
#ifdef SUPPORTS_RENDERABLE_CUBEMAPS

  INSERT_PERF_MARKER_SCOPE("CubeMapHandle_cl::FlipAndBlur");

  hkvVec2 vTopLeft(0, 0);
  hkvVec2 vBottomRight((float)m_iSize, (float)m_iSize);

  hkvVec2 vTopLeftTex;
  hkvVec2 vBottomRightTex;

  if(m_spRendererNode)
  {
    vTopLeftTex = hkvVec2(0, 1);
    vBottomRightTex = hkvVec2(1, 0);
  }
  else
  {
    vTopLeftTex = hkvVec2(0, 0);
    vBottomRightTex = hkvVec2(1, 1);
  }

#ifdef _VR_DX9
  const float fHalfTexelOffset = 0.5f / m_iSize;

  vTopLeftTex += fHalfTexelOffset;
  vBottomRightTex += fHalfTexelOffset;
#endif

  VisRenderContext_cl *pContext = VisRenderContext_cl::GetCurrentContext();

  // No blur passes => only flip cubemap face (if needed), then return
  if(m_iBlurPasses == 0 && m_spRendererNode)
  {
    VSimpleRenderState_t iState(VIS_TRANSP_NONE, RENDERSTATEFLAG_ALWAYSVISIBLE | RENDERSTATEFLAG_DOUBLESIDED);

    IVRender2DInterface *pRI = Vision::RenderLoopHelper.BeginOverlayRendering();
    pRI->DrawTexturedQuad(vTopLeft, vBottomRight, m_spBlurTarget[0], vTopLeftTex, vBottomRightTex, V_RGBA_WHITE, iState);
    Vision::RenderLoopHelper.EndOverlayRendering();
    return;
  }

  const float fBlurStepHorizontal[] =
  {
    1.0f / float(m_iSize), 0, 0, 0
  };

  const float fBlurStepVertical[] =
  {
    0, 1.0f / float(m_iSize), 0, 0
  };

  // Blur => Flip in the first blur pass (if needed), then revert texture coords to non-flipped
  for (int i = 0; i < m_iBlurPasses; i++)
  {

    m_spBlurContext[1]->Activate();
    VisRenderContext_cl::PerformPendingContextSwitch();

    m_blurStepRegister.SetRegisterValueF(m_spBlurTechnique->GetShader(0), fBlurStepHorizontal);

    IVRender2DInterface *pRI = Vision::RenderLoopHelper.BeginOverlayRendering();
    pRI->DrawTexturedQuadWithShader(vTopLeft, vBottomRight, m_spBlurTarget[0], vTopLeftTex, vBottomRightTex, V_RGBA_WHITE, *(m_spBlurTechnique->m_Shaders.GetAt(0)));
    Vision::RenderLoopHelper.EndOverlayRendering();

    vTopLeftTex = hkvVec2(0, 0);
    vBottomRightTex = hkvVec2(1, 1);

#ifdef _VR_DX9
    vTopLeftTex += fHalfTexelOffset;
    vBottomRightTex += fHalfTexelOffset;
#endif

    m_spBlurContext[0]->Activate();
    VisRenderContext_cl::PerformPendingContextSwitch();

    m_blurStepRegister.SetRegisterValueF(m_spBlurTechnique->GetShader(0), fBlurStepVertical);

    pRI = Vision::RenderLoopHelper.BeginOverlayRendering();
    pRI->DrawTexturedQuadWithShader(vTopLeft, vBottomRight, m_spBlurTarget[1], vTopLeftTex, vBottomRightTex, V_RGBA_WHITE, *(m_spBlurTechnique->m_Shaders.GetAt(0)));
    Vision::RenderLoopHelper.EndOverlayRendering();
  }

  pContext->ResolveToTexture(m_spCubemap, false, 0, 0, 0, 0, 0, 0, iFace);
#endif // SUPPORTS_RENDERABLE_CUBEMAPS
}

void CubeMapHandle_cl::Invalidate()
{
#ifdef SUPPORTS_RENDERABLE_CUBEMAPS

  m_bUpdateForced = true;

  EnableRendering();

#endif // SUPPORTS_RENDERABLE_CUBEMAPS
}


void CubeMapHandle_cl::UpdateFaceStatus()
{
#ifdef SUPPORTS_RENDERABLE_CUBEMAPS

  VASSERT(m_bActive && m_bRenderingEnabled);
  VASSERT(m_bUpdateForced || m_bContinuousUpdate);

  if (!m_spCubemap)
    return;

  if(m_bUpdateForced)
  {
    m_iCurrentFaceMask = 0xff;
    return;
  }

  m_fElapsedTime += Vision::GetTimer()->GetTimeDifference();
  if(m_fElapsedTime < m_fUpdateInterval)
  {
    m_iCurrentFaceMask = 0;
    return;
  }
  else
    {
    m_fElapsedTime = m_fUpdateInterval ? hkvMath::mod(m_fElapsedTime, m_fUpdateInterval) : 0.0f;
  }

  if (m_bAlternateUpdate)
  {
    m_iCurrentFace = (m_iCurrentFace+1)%6;
    m_iCurrentFaceMask = (1<<m_iCurrentFace);
  }
    else
    {
    m_iCurrentFaceMask = 0xff;
  }

#endif // SUPPORTS_RENDERABLE_CUBEMAPS
}

void CubeMapHandle_cl::SetActivate(bool bStatus)
{
  if (m_bActive==bStatus)
  {
    return;
  }

  m_bActive = bStatus;

#ifdef SUPPORTS_RENDERABLE_CUBEMAPS

  if (bStatus)
  {
    InitContexts();
    CreateBlurTarget();
    if (m_iBlurPasses>0)
    {
      CreateBlurTechnique();
    }

    if(m_bContinuousUpdate || m_bUpdateForced)
    {
      EnableRendering();
    }

    Vision::Callbacks.OnLeaveBackground += this;
    Vision::Callbacks.OnReassignShaders += this;
  }
  else
  {
    DisableRendering();

    m_spBlurTechnique = NULL;
    DestroyBlurTarget();
    DeInitContexts();

    Vision::Callbacks.OnLeaveBackground -= this;
    Vision::Callbacks.OnReassignShaders -= this;
  }

#endif // SUPPORTS_RENDERABLE_CUBEMAPS
}


void CubeMapHandle_cl::SetDefaultRenderLoop(IVisRenderLoop_cl *pRenderLoop) 
{ 
  s_spDefaultRenderLoop = pRenderLoop; 
}

IVisRenderLoop_cl *CubeMapHandle_cl::GetDefaultRenderLoop() 
{ 
  return s_spDefaultRenderLoop; 
}

void CubeMapHandle_cl::SetContextOrientation()
{
  for(int i = 0; i < 6; i++)
  {
    if(m_spRenderContext[i] == NULL)
      continue;

    hkvVec3 vLookDir,vLookRight,vLookUp;
    switch (i)
    {
      // directly taken from the DX9 documentation diagram (see "Cubic Environment Mapping")
    case 0:vLookDir.x = ( 1.f);vLookRight.z = (-1.f);vLookUp.y = ( 1.f);break; // + X
    case 1:vLookDir.x = (-1.f);vLookRight.z = ( 1.f);vLookUp.y = ( 1.f);break; // - X 
    case 2:vLookDir.y = ( 1.f);vLookRight.x = ( 1.f);vLookUp.z = (-1.f);break; // + Y
    case 3:vLookDir.y = (-1.f);vLookRight.x = ( 1.f);vLookUp.z = ( 1.f);break; // - Y 
    case 4:vLookDir.z = ( 1.f);vLookRight.x = ( 1.f);vLookUp.y = ( 1.f);break; // + Z
    case 5:vLookDir.z = (-1.f);vLookRight.x = (-1.f);vLookUp.y = ( 1.f);break; // - Z 
    default:
      return;
    }

    hkvMat3 viewMat;

    // When using a renderer node, don't use an inverted orientation, but flip the target afterwards instead
    // as it's simpler than applying reverse cull mode to all relevant contexts of the renderer node
    if(m_spRendererNode)
    {
      viewMat.setAxisXYZ (vLookDir,-vLookRight,-vLookUp);
      m_spRenderContext[i]->SetRenderFlags(m_spRenderContext[i]->GetRenderFlags() & ~VIS_RENDERCONTEXT_FLAG_REVERSE_CULLMODE);
    }
    // When using a single render loop, use an inverted camera orientation with a flipped cull mode
    else
    {
      viewMat.setAxisXYZ (vLookDir,-vLookRight,vLookUp);
      m_spRenderContext[i]->SetRenderFlags(m_spRenderContext[i]->GetRenderFlags() | VIS_RENDERCONTEXT_FLAG_REVERSE_CULLMODE);
    }
    m_spRenderContext[i]->GetCamera()->Set(viewMat, GetPosition());
    m_spRenderContext[i]->GetCamera()->ReComputeVisibility();
  }
}


#ifdef SUPPORTS_SNAPSHOT_CREATION

// leave this function empty since we do not need/want its entity dependencies (e.g. model in vForge). The underlying texture is
// referenced by the shaders that use the cubemap
void CubeMapHandle_cl::GetDependencies(VResourceSnapshot &snapshot)
{
}

#endif



V_IMPLEMENT_SERIAL( CubeMapHandle_cl, VisBaseEntity_cl, 0, &g_VisionEngineModule );
void CubeMapHandle_cl::Serialize( VArchive &ar )
{
  VisBaseEntity_cl::Serialize(ar);
  if (ar.IsLoading())
  {
    int iVersion;
    int iBlurPasses = 0;
    m_bGenMipMaps = false;
    ar >> iVersion;
    VASSERT(iVersion>=0 && iVersion<=CUBEMAP_SERIAL_CURRENTVERSION);

    if (iVersion >= CUBEMAP_SERIAL_VERSION3)
    {
      ar >> m_bGenMipMaps;
    }

    if (iVersion >= CUBEMAP_SERIAL_VERSION2)
    {
      ar >> iBlurPasses;
    }

    if (iVersion >= CUBEMAP_SERIAL_VERSION1)
    { 
      ar >> m_fNearClipPlane;
      ar >> m_fFarClipPlane;
      ar >> m_iRenderFilterMask;
    }
    else
    {
      m_fNearClipPlane = m_fFarClipPlane = -1.f; // from main context
      m_iRenderFilterMask = VIS_ENTITY_VISIBLE_IN_CUBEMAP;
    }

    ar >> m_iSize;
    if (iVersion >= CUBEMAP_SERIAL_VERSION4)
    {
      ar >> m_bContinuousUpdate;
    }
    ar >> m_fUpdateInterval;
    int dummy;
    ar >> dummy;
    if (iVersion >= CUBEMAP_SERIAL_VERSION5)
    {
      ar >> m_bAlternateUpdate;
    }

    VMemoryTempBuffer<1024> Key;
    Key.ReadStringBinary(ar);
#if defined(SUPPORTS_RENDERABLE_CUBEMAPS) // Cube maps supported?
    SetCubemapKey((const char *)Key.GetBuffer(),m_iSize);
    SetActivate(true);
    SetBlurPasses(iBlurPasses);
#endif 
    m_fElapsedTime = m_fUpdateInterval; // render ASAP
  }
  else
  {
    ar << (int)CUBEMAP_SERIAL_CURRENTVERSION; // version
    ar << m_bGenMipMaps;
    ar << m_iBlurPasses;
    ar << m_fNearClipPlane;
    ar << m_fFarClipPlane;
    ar << m_iRenderFilterMask;
    ar << m_iSize;
    ar << m_bContinuousUpdate; // version 4
    ar << m_fUpdateInterval;
    ar << 0;
    ar << m_bAlternateUpdate; // version 5
    const char *szKey = NULL;
    if (m_spCubemap)
      szKey = m_spCubemap->GetFilename();
    ar << szKey;
  }
}

START_VAR_TABLE(CubeMapHandle_cl, VisBaseEntity_cl, "use this entity to provide cube maps for the shader system", VFORGE_HIDECLASS, "")
  DEFINE_VAR_INT(CubeMapHandle_cl, m_iSize,"one dimensional size of the texture; has to be a power of 2", "128", 0,0);
DEFINE_VAR_BOOL(CubeMapHandle_cl, m_bContinuousUpdate, "indicates whether cube map shall be updated or just be computed one single time", "0", 0, 0);
DEFINE_VAR_FLOAT(CubeMapHandle_cl, m_fUpdateInterval, "update interval of the cube map in seconds, 0.0f updates the cube map each frame", "1.0f", 0, 0);
DEFINE_VAR_FLOAT(CubeMapHandle_cl, m_fNearClipPlane, "near clip plane for rendering the cube map", "5.0f", 0, 0);
DEFINE_VAR_FLOAT(CubeMapHandle_cl, m_fFarClipPlane, "far clip plane for rendering the cube map", "32000.0f", 0, 0);
DEFINE_VAR_INT(CubeMapHandle_cl, m_iBlurPasses, "number of blur passes (0 for best performance)", "0", 0, 0);
DEFINE_VAR_BOOL(CubeMapHandle_cl, m_bGenMipMaps, "generate mip maps after rendering", "0", 0, 0);
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
