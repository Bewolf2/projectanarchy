/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RendererNode/VRendererNodeCommon.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/RenderingOptimizationHelpers.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/PostProcessBase.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/VPostProcessScreenMasks.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/BufferResolver.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/VPostProcessTranslucencies.hpp>

V_IMPLEMENT_SERIAL_ABSTRACT(VRendererNodeCommon, IVRendererNode, 0, &g_VisionEngineModule);

#define RENDERERNODEBASE_VERSION_1            1 // Initial version, corresponds to forward version 6 and deferred version 12
#define RENDERERNODEBASE_VERSION_CURRENT      1


VisMeshBufferObjectCollection_cl VRendererNodeCommon::s_MeshBufferObjectCollection(32, 32);


VRendererNodeCommon::VRendererNodeCommon() {
  m_iAutomaticResolveCounter = 0;
  m_pColorBufferResolver = NULL;
  m_uiResolveColorBufferRenderHook = VRH_PRE_TRANSPARENT_PASS_GEOMETRY;
  m_iPostProcessorUpdateCounter = 0;
  m_bPostProcessorAssignmentDirty = false;
  m_pDeinitDuringVideoResize = NULL;
  m_bUsesDirectRenderToFinalTargetContext = false;

  Vision::Callbacks.OnBeforeVideoChanged += this;
  Vision::Callbacks.OnVideoChanged += this;
  Vision::Callbacks.OnEnterForeground += this;
}

VRendererNodeCommon::VRendererNodeCommon(VisRenderContext_cl* pTargetContext) : IVRendererNode(pTargetContext) {
  m_iAutomaticResolveCounter = 0;
  m_pColorBufferResolver = NULL;
  m_uiResolveColorBufferRenderHook = VRH_PRE_TRANSPARENT_PASS_GEOMETRY;
  m_iPostProcessorUpdateCounter = 0;
  m_bPostProcessorAssignmentDirty = false;
  m_pDeinitDuringVideoResize = NULL;
  m_bUsesDirectRenderToFinalTargetContext = false;

  Vision::Callbacks.OnBeforeVideoChanged += this;
  Vision::Callbacks.OnVideoChanged += this;
  Vision::Callbacks.OnEnterForeground += this;
}

VRendererNodeCommon::~VRendererNodeCommon()
{
  Vision::Callbacks.OnEnterForeground -= this;
  Vision::Callbacks.OnVideoChanged -= this;
  Vision::Callbacks.OnBeforeVideoChanged -= this;
}

bool VRendererNodeCommon::RendersIntoBackBuffer()
{
  int iNumContexts = GetContextCount();
  for (int i=0; i<iNumContexts; i++)
  {
    VisRenderContext_cl *pContext = GetContext(i);
    if (pContext != NULL)
      if (pContext->RendersIntoBackBuffer())
        return true;
  }

  return false;
}

void VRendererNodeCommon::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender == &Vision::Callbacks.OnBeforeVideoChanged)
  {
    if (GetFinalTargetContext() != NULL && RendersIntoBackBuffer() && IsInitialized())
    {
      m_pDeinitDuringVideoResize = new VScopedRendererNodeDeinit(this);
    }
  }
  else if (pData->m_pSender == &Vision::Callbacks.OnVideoChanged)
  {
    V_SAFE_DELETE(m_pDeinitDuringVideoResize);
  }
#if defined(WIN32) && defined(_VR_DX9)
  else if(pData->m_pSender == &Vision::Callbacks.OnEnterForeground)
  {
    if(IsInitialized())
    {
      InitializePostProcessors();
    }
  }
#endif
}

void VRendererNodeCommon::RemovePostprocessors()
{
  VMemoryTempBuffer<256> tempMem(Components().Count() * sizeof(VPostProcessingBaseComponent*));

  VPostProcessingBaseComponent** pAllPostProcessors = reinterpret_cast<VPostProcessingBaseComponent**>(tempMem.GetBuffer());

  int iPostProcessorIndex = 0;

  for (int i=0; i<Components().Count(); i++)
  {
    if(VPostProcessingBaseComponent *pPostProcessor = vdynamic_cast<VPostProcessingBaseComponent*>(Components().GetAt(i)))
    {
      pAllPostProcessors[iPostProcessorIndex] = pPostProcessor;
      iPostProcessorIndex++;
    }
  }

  BeginPostProcessorUpdate();
  while(iPostProcessorIndex > 0)
  {
    RemoveComponent(pAllPostProcessors[iPostProcessorIndex-1]);
    iPostProcessorIndex--;
  }
  EndPostProcessorUpdate();
}

void VRendererNodeCommon::Execute()
{
  for(int i = 0; i < Components().Count(); i++)
  {
    VPostProcessingBaseComponent* pPostProcessor = vdynamic_cast<VPostProcessingBaseComponent*>(Components().GetAt(i));
    if(pPostProcessor != NULL && pPostProcessor->IsActive())
    {
      VASSERT_MSG(pPostProcessor->IsValid(), "Post processor is active but invalid");
      VASSERT_MSG(pPostProcessor->IsInitialized(), "Post processor is active but not initialized");
    }
  }

  IVRendererNode::Execute();
}

namespace
{
  int ComparePostProcessorsByPriority(const void* pPointerA, const void* pPointerB)
  {
    const VPostProcessingBaseComponent* pComponentA = *reinterpret_cast<VPostProcessingBaseComponent const* const*>(pPointerA);
    const VPostProcessingBaseComponent* pComponentB = *reinterpret_cast<VPostProcessingBaseComponent const* const*>(pPointerB);

    return (pComponentA->GetPriority() > pComponentB->GetPriority()) - (pComponentA->GetPriority() < pComponentB->GetPriority());
  }

  VisRenderContext_cl* CloneContext(const VisRenderContext_cl* pSrc)
  {
    int iPosX, iPosY, iWidth, iHeight;
    float zMin, zMax;
    pSrc->GetViewport(iPosX, iPosY, iWidth, iHeight, zMin, zMax);

    VisRenderContext_cl* pDest = new VisRenderContext_cl(pSrc->GetCamera(), 90.0f, 90.0f, iWidth, iHeight, 0.0f, 0.0f, pSrc->GetRenderFlags());
    pDest->SetRenderFilterMask(pSrc->GetRenderFilterMask());
    pDest->SetViewport(iPosX, iPosY, iWidth, iHeight, zMin, zMax);
    pDest->SetViewProperties(pSrc->GetViewProperties());
    return pDest;
  }
}


void VRendererNodeCommon::DeInitializePostProcessors()
{
  for (int i=0; i<Components().Count(); i++)
  {
    if(VPostProcessingBaseComponent *pPostProcessor = vdynamic_cast<VPostProcessingBaseComponent*>(Components().GetAt(i)))
    {
      pPostProcessor->DeInitializePostProcessor();
      VASSERT_MSG(!IsContextRegistered(pPostProcessor->GetTargetContext()), "Post processor didn't unregister its context upon deinitialization!");
      pPostProcessor->m_iTargetIndex = -1;
    }
  }

  m_assignedContexts.Clear();
}

void VRendererNodeCommon::InitializePostProcessors()
{
  VASSERT_MSG(IsInitialized(), "The renderer node must be initialized before initializing the post processors.");

  ANALYSIS_IGNORE_WARNING_BLOCK_START(6385);
  ANALYSIS_IGNORE_WARNING_BLOCK_START(6211);

  // Increment the update counter to enable modifying the post processors without recursing
  m_iPostProcessorUpdateCounter++;

  bool bInvalidPostProcessorActive = false;
  do
  {
    bInvalidPostProcessorActive = false;
    DeInitializePostProcessors();

    VSimpleCopyPostprocess* pSimpleCopy = NULL;

    // Collect post processor components
    VMemoryTempBuffer<256> tempBuffer((Components().Count() + 1) * sizeof(VPostProcessingBaseComponent*));
    VPostProcessingBaseComponent** postProcessors = reinterpret_cast<VPostProcessingBaseComponent**>(tempBuffer.GetBuffer());
    int iPostProcessorIndex = 0;
    for(int iComponentIndex = 0; iComponentIndex < Components().Count(); iComponentIndex++)
    {
      if(VPostProcessingBaseComponent* pPostProcessor = vdynamic_cast<VPostProcessingBaseComponent*>(Components().GetAt(iComponentIndex)))
      {
        // Don't take the auto added copy PP into consideration, we'll handle that separately
        if(VSimpleCopyPostprocess* pCopy = vdynamic_cast<VSimpleCopyPostprocess*>(pPostProcessor))
        {
          pSimpleCopy = pCopy;
          continue;
        }

        // HS#10443: Skip post-processors which do nothing - needs testing whether this works cleanly when the identity state changes
        if(!pPostProcessor->IsActive() /*!pPostProcessor->IsIdentity()*/)
        {
          continue;
        }

        postProcessors[iPostProcessorIndex] = pPostProcessor;
        iPostProcessorIndex++;
      }
    }

    int iNumPostProcessors = iPostProcessorIndex;

    qsort(postProcessors, iNumPostProcessors, sizeof(VPostProcessingBaseComponent*), ComparePostProcessorsByPriority);

    int iCopyPPIndex = iNumPostProcessors;

    // Scan backwards through post processors to find one which can take over the responsibility
    // of copying the scene to the final target context
    //
    //  This post processor must:
    //    - come after the MSAA resolve step
    //    - render an opaque full screen quad
    //    - not have any postprocessor afterwards that reads the accumulation buffer
    bool bUsesOffscreenRenderTarget = !m_bUsesDirectRenderToFinalTargetContext;
    for(int i = iNumPostProcessors - 1; i >= 0; i--)
    {
      if(postProcessors[i]->GetPriority() < VIS_RENDERCONTEXTPRIORITY_POSTPROCESSOR_RESOLVED)
      {
        bUsesOffscreenRenderTarget = true;
        break;
      }

      const unsigned int flags = postProcessors[i]->GetBufferUsageFlags();

      // Post processors that use their own render target can't be used for copying to the back buffer
      if((flags & VPostProcessingBaseComponent::USES_CUSTOM_RENDERTARGET) != 0)
      {
        bUsesOffscreenRenderTarget = true;
        break;
      }

      // Check first if the post processors draws an opaque full screen quad, because
      // a PP that draws a full screen quad AND samples the accumulation buffer
      // is still suitable for copying the accumulation buffer into the final target context (such as tonemapping).
      if((flags & VPostProcessingBaseComponent::DRAWS_FULLSCREEN_QUAD) != 0 && (flags & VPostProcessingBaseComponent::USES_BLENDING) == 0)
      {
        iCopyPPIndex = i;
        break;
      }

      if(flags & VPostProcessingBaseComponent::SAMPLES_ACCUMULATION_BUFFER)
      {
        bUsesOffscreenRenderTarget = true;
        break;
      }
    }

    VASSERT_MSG(bUsesOffscreenRenderTarget != m_bUsesDirectRenderToFinalTargetContext, "Renderer node indicated that it renders directly to the renderer node's final target context, but post-processors require an offscreen render target!");

    // If no suitable post processor was found, we need to make sure the scene is copied
    bool bNeedsManualCopyToTarget = (iCopyPPIndex == iNumPostProcessors) && bUsesOffscreenRenderTarget;

    // If we don't use an offscreen RT, we don't have a copy PP
    if (!bUsesOffscreenRenderTarget)
      iCopyPPIndex = -1;

    if(bNeedsManualCopyToTarget)
    {
      if(pSimpleCopy == NULL)
      {
        pSimpleCopy = new VSimpleCopyPostprocess;
        AddComponent(pSimpleCopy);
      }

      postProcessors[iNumPostProcessors] = pSimpleCopy;
      iNumPostProcessors++;
    }
    else if(pSimpleCopy != NULL)
    {
      // Remove existing copy PP if not needed
      RemoveComponent(pSimpleCopy);
    }

    m_assignedContexts.EnsureCapacity(iNumPostProcessors);

    // Create a target context for each post processor
    for(iPostProcessorIndex = 0; iPostProcessorIndex < iNumPostProcessors; iPostProcessorIndex++)
    {
      VPostProcessingBaseComponent* pPostProcessor = postProcessors[iPostProcessorIndex];

      pPostProcessor->m_iTargetIndex = iPostProcessorIndex;

      const VisRenderContext_cl* pFinalTargetContext = GetFinalTargetContext();
      VisRenderContext_cl* pContext = CloneContext(pFinalTargetContext);

      pContext->SetName(pPostProcessor->GetTypeId()->m_lpszClassName);

      pContext->SetVisibilityCollector(pFinalTargetContext->GetVisibilityCollector(), false);
      pContext->SetPriority(pPostProcessor->GetPriority());
      pContext->SetUserData(pPostProcessor);
      pContext->SetRenderLoop(new PostProcessRenderLoop_cl(pPostProcessor));

      bool bRenderIntoFinalTargetContext = (iPostProcessorIndex >= iCopyPPIndex);
      if(bRenderIntoFinalTargetContext)
      {
        pContext->SetRenderAndDepthStencilTargets(pFinalTargetContext);

        if (bUsesOffscreenRenderTarget)
        {
          // If possible, try to give the post processors that render directly into the final target context a useful depth-stencil target.
          // This is only possible if the final target context has MSAA disabled.
          bool bCanReplaceDST = false;

          if(pFinalTargetContext->RendersIntoBackBuffer())
          {
            #if !defined(_VISION_ANDROID)
              // On Android, the back buffer context uses a fixed FBO, so we can't replace the DST.
              bCanReplaceDST = Vision::Video.GetCurrentConfig()->m_eMultiSample == VVIDEO_MULTISAMPLE_OFF;
            #endif
          }
          else if(pFinalTargetContext->GetRenderTarget(0) != NULL)
          {
            bCanReplaceDST = static_cast<VisRenderableTexture_cl*>(pFinalTargetContext->GetRenderTarget(0))->GetConfig()->m_iMultiSampling <= 1;
          }

          if(bCanReplaceDST)
          {
            pContext->SetDepthStencilTarget(static_cast<VisRenderableTexture_cl*>(GetPostProcessDepthStencilTarget(VRTV_RESOLVED)));
          }
          else
          {
            Vision::Error.Warning("Could not attach a depth-stencil target to the context of the \"%s\" post processor - depth testing will not work correctly.", pPostProcessor->GetTypeId()->m_lpszClassName);
          }
        }
      }
      else
      {
        VRenderTargetVersion_e targetVersion = (pPostProcessor->GetPriority() <= VIS_RENDERCONTEXTPRIORITY_POSTPROCESSOR_RESOLVED) ? VRTV_MSAA : VRTV_RESOLVED;

        if((pPostProcessor->GetBufferUsageFlags() & VPostProcessingBaseComponent::USES_CUSTOM_RENDERTARGET) == 0)
        {
          pContext->SetRenderTarget(0, static_cast<VisRenderableTexture_cl*>(GetPostProcessColorTarget(targetVersion)));
          pContext->SetDepthStencilTarget(static_cast<VisRenderableTexture_cl*>(GetPostProcessDepthStencilTarget(targetVersion)));
        }
      }

      m_assignedContexts.Add(pContext);

      pPostProcessor->InitializePostProcessor();

      // Validity can only be determined after initialization, so deactivate the invalid postprocessor and retry the entire context setup
      if(!pPostProcessor->IsValid())
      {
        // the post-processor will have deactivated itself by now
        pPostProcessor->SetActive(false);
        bInvalidPostProcessorActive = true;
      }
    }
  }
  while ( bInvalidPostProcessorActive );

  m_bPostProcessorAssignmentDirty = false;
  m_iPostProcessorUpdateCounter--;

  VisRenderContext_cl::ElementManagerDeleteAllUnRef();

  ANALYSIS_IGNORE_WARNING_BLOCK_END;
  ANALYSIS_IGNORE_WARNING_BLOCK_END;
}

BOOL VRendererNodeCommon::AddComponent(IVObjectComponent *pComponent)
{
  BOOL bResult = IVRendererNode::AddComponent(pComponent);

  if(bResult == TRUE && pComponent->IsOfType(VPostProcessingBaseComponent::GetClassTypeId()))
  {
    OnPostProcessorChanged();
  }

  return bResult;
}

BOOL VRendererNodeCommon::RemoveComponent(IVObjectComponent *pComponent) 
{
  BOOL bIsPostProcessor = pComponent->IsOfType(VPostProcessingBaseComponent::GetClassTypeId());
  BOOL bResult = IVRendererNode::RemoveComponent(pComponent);

  if(bResult == TRUE && bIsPostProcessor)
  {
    OnPostProcessorChanged();
  }

  return bResult;
}

void VRendererNodeCommon::BeginPostProcessorUpdate()
{
  m_iPostProcessorUpdateCounter++;
}

void VRendererNodeCommon::OnPostProcessorChanged()
{
  if(m_iPostProcessorUpdateCounter == 0 && IsInitialized())
  {
    InitializePostProcessors();
  }
  else
  {
    m_bPostProcessorAssignmentDirty = true;
  }
}

void VRendererNodeCommon::EndPostProcessorUpdate()
{
  m_iPostProcessorUpdateCounter--;
  if(m_iPostProcessorUpdateCounter == 0 && m_bPostProcessorAssignmentDirty && IsInitialized())
  {
    InitializePostProcessors();
  }
}


void VRendererNodeCommon::InitializeSharedFeatures(VisRenderableTexture_cl* pDepthReadTarget, VisRenderableTexture_cl* pColorReadTarget)
{
  m_spDepthReadTarget = pDepthReadTarget;
  m_spColorReadTarget = pColorReadTarget;
#ifdef _VISION_XENON
  m_bDepthRestoreInitialized = true;
  VisRenderableTexture_cl* pDepthTarget = static_cast<VisRenderableTexture_cl*>(GetPostProcessDepthStencilTarget(VRTV_RESOLVED));
  m_fastRestoreData = VisHiZHelper_cl::CreateFastDepthRestore(pDepthTarget, pDepthReadTarget);
#endif

  V_SAFE_DELETE(m_pColorBufferResolver);
  m_ObjectsUsingResolvedColorBuffer.Clear();
  m_iAutomaticResolveCounter = 0;
}

void VRendererNodeCommon::DeInitializeSharedFeatures()
{
  FreeCustomTextureRefs(m_spSceneDepthTechnique);
  FreeCustomTextureRefs(m_spDepthOnlyTechnique);

  m_spColorReadTarget = NULL;
  m_spDepthReadTarget = NULL;
  m_spSceneDepthTechnique = NULL;
  m_spDepthOnlyTechnique = NULL;

  V_SAFE_DELETE(m_pColorBufferResolver);

#ifdef _VISION_XENON
  FreeCustomTextureRefs(m_spDepthOnlyTechnique360);
  if(m_bDepthRestoreInitialized)
  {
    VisHiZHelper_cl::DestroyFastDepthRestore(m_fastRestoreData);
    m_bDepthRestoreInitialized = false;
  }
  m_spDepthOnlyTechnique360 = NULL;
#endif

#ifdef _VISION_PS3
  m_spDummyDepthTexture = NULL;
#endif
}

void VRendererNodeCommon::PushAndDisableGlobalWireframeState()
{
  m_bSavedGlobalWireframeState = Vision::Renderer.GetWireframeMode();
  Vision::Renderer.SetWireframeMode(false);
}

void VRendererNodeCommon::PopGlobalWireframeState()
{
  Vision::Renderer.SetWireframeMode( m_bSavedGlobalWireframeState );
}

void VRendererNodeCommon::RenderSceneTexture(bool bHalfSize) 
{
  PushAndDisableGlobalWireframeState();
  IVRender2DInterface *pRenderer = Vision::RenderLoopHelper.BeginOverlayRendering();
  VSimpleRenderState_t iRenderState(VIS_TRANSP_NONE, RENDERSTATEFLAG_ALWAYSVISIBLE|RENDERSTATEFLAG_DOUBLESIDED);
  Overlay2DVertex_t *pVertices;
  if (bHalfSize)
    pVertices = GetRendererNodeHelper()->GetOverlayVerticesHalfSize();
  else
    pVertices = GetRendererNodeHelper()->GetOverlayVertices();
  pRenderer->Draw2DBuffer(6, pVertices, m_spColorReadTarget, iRenderState);
  Vision::RenderLoopHelper.EndOverlayRendering();
  PopGlobalWireframeState();
}

#ifdef _VISION_XENON
void VRendererNodeCommon::RenderSceneDepthXbox360(VisHiZHelper_cl::VRestoreHiZ_e eRestoreHiZ, float fMinY, float fMaxY)
{
  PushAndDisableGlobalWireframeState();
  if (m_spDepthOnlyTechnique == NULL)
  {
    Vision::Shaders.LoadShaderLibrary("\\Shaders\\BaseShaders.ShaderLib", SHADERLIBFLAG_HIDDEN);
    m_spDepthOnlyTechnique360 = Vision::Shaders.CreateTechnique("CopyDepthOnly_360", NULL);
    VASSERT(m_spDepthOnlyTechnique360 != NULL);
  }
  VASSERT_MSG(m_bDepthRestoreInitialized, "Fast depth restore has not been initialized");
  VisHiZHelper_cl::FastDepthRestore(m_fastRestoreData, m_spDepthOnlyTechnique360->GetShader(0), eRestoreHiZ, fMinY, fMaxY);

  PopGlobalWireframeState();
}
#endif

void VRendererNodeCommon::RenderSceneDepth(bool bHalfSize)
{
  PushAndDisableGlobalWireframeState();
  if (m_spDepthOnlyTechnique == NULL)
  {
    Vision::Shaders.LoadShaderLibrary("\\Shaders\\BaseShaders.ShaderLib", SHADERLIBFLAG_HIDDEN);
#ifdef _VISION_XENON
    m_spDepthOnlyTechnique360 = Vision::Shaders.CreateTechnique("CopyDepthOnly_360", NULL);
    VASSERT(m_spDepthOnlyTechnique360 != NULL);
#endif
    m_spDepthOnlyTechnique = Vision::Shaders.CreateTechnique("CopyDepthOnly", NULL);
    VASSERT(m_spDepthOnlyTechnique != NULL);
  }
#ifdef _VISION_XENON
  if(bHalfSize == false)
  {
    VASSERT_MSG(m_bDepthRestoreInitialized, "Fast depth restore has not been initialized");
    VisHiZHelper_cl::FastDepthRestore(m_fastRestoreData, m_spDepthOnlyTechnique360->GetShader(0), VisHiZHelper_cl::VR_HIZ_RESTORE);
  }
  else
  {
#endif
    IVRender2DInterface *pRenderer = Vision::RenderLoopHelper.BeginOverlayRendering();
    VCompiledShaderPass *pPass = m_spDepthOnlyTechnique->GetShader(0);
    VStateGroupTexture *pStateGroupTexture = pPass->GetStateGroupTexture(VSS_PixelShader, 0);

#ifdef _VISION_PS3
    if (m_spDummyDepthTexture == NULL)
      m_spDummyDepthTexture = RenderingOptimizationHelpers_cl::CreateAndPatchDummyTexture(m_spDepthReadTarget);
    if (pStateGroupTexture != NULL)
    {
      pStateGroupTexture->m_spCustomTex = m_spDummyDepthTexture;
      pPass->m_bModified = true;
    }
#else
    if (pStateGroupTexture != NULL)
    {
      pStateGroupTexture->m_spCustomTex = m_spDepthReadTarget;
      pPass->m_bModified = true;
    }
#endif

    Overlay2DVertex_t *pVertices;
    if (bHalfSize)
      pVertices = GetRendererNodeHelper()->GetOverlayVerticesHalfSize();
    else
      pVertices = GetRendererNodeHelper()->GetOverlayVertices();

    pRenderer->Draw2DBufferWithShader(6, pVertices, NULL, *pPass);

    Vision::RenderLoopHelper.EndOverlayRendering();
#ifdef _VISION_XENON
  }
#endif
  PopGlobalWireframeState();
}

void VRendererNodeCommon::RenderSceneTextureWithDepth(bool bHalfSize)
{
  PushAndDisableGlobalWireframeState();

  if (m_spSceneDepthTechnique == NULL)
  {
    Vision::Shaders.LoadShaderLibrary("\\Shaders\\BaseShaders.ShaderLib", SHADERLIBFLAG_HIDDEN);
    m_spSceneDepthTechnique = Vision::Shaders.CreateTechnique("CopyWithDepthOutput", NULL);
    VASSERT(m_spSceneDepthTechnique != NULL);
  }

#ifdef _VISION_XENON
  if(bHalfSize)
  {
    // first make sure the stencil buffer is cleared.
    // we clear it to all 1's, and then reset all geometry pixels to 0
    // For this the shader simply checks a pixels depth and does a discard on sky pixels (depth == 1.0).
    // On all pixels that pass the test, the stencil value is set to zero.
    Vision::RenderLoopHelper.ClearScreen(VisRenderLoopHelper_cl::VCTF_DepthStencil, VColorRef (0, 0, 0, 0), 1.0f, 255);
#endif

    IVRender2DInterface *pRenderer = Vision::RenderLoopHelper.BeginOverlayRendering();
    VCompiledShaderPass *pPass = m_spSceneDepthTechnique->GetShader(0);
    VStateGroupTexture *pStateGroupTexture = pPass->GetStateGroupTexture(VSS_PixelShader, 0);
    if (pStateGroupTexture != NULL)
      pStateGroupTexture->m_spCustomTex = m_spColorReadTarget;

    pStateGroupTexture = pPass->GetStateGroupTexture(VSS_PixelShader, 1);
#ifdef _VISION_PS3
    if (m_spDummyDepthTexture == NULL)
      m_spDummyDepthTexture = RenderingOptimizationHelpers_cl::CreateAndPatchDummyTexture(m_spDepthReadTarget);
    if (pStateGroupTexture != NULL)
    {
      pStateGroupTexture->m_spCustomTex = m_spDummyDepthTexture;
      pPass->m_bModified = true;
    }
#else
    if (pStateGroupTexture != NULL)
    {
      pStateGroupTexture->m_spCustomTex = m_spDepthReadTarget;
      pPass->m_bModified = true;
    }
#endif

    Overlay2DVertex_t *pVertices;
    if (bHalfSize)
      pVertices = GetRendererNodeHelper()->GetOverlayVerticesHalfSize();
    else
      pVertices = GetRendererNodeHelper()->GetOverlayVertices();
    pRenderer->Draw2DBufferWithShader(6, pVertices, NULL, *pPass); 

    Vision::RenderLoopHelper.EndOverlayRendering();

#ifdef _VISION_XENON
  }
  else
  {
    if(m_spDepthOnlyTechnique360 == NULL)
    {
      m_spDepthOnlyTechnique360 = Vision::Shaders.CreateTechnique("CopyDepthOnly_360", NULL);
      VASSERT(m_spDepthOnlyTechnique360 != NULL);
    }
    VASSERT_MSG(m_bDepthRestoreInitialized, "Fast depth restore has not been initialized");
    //Restore color first (can't do this in one step because of a certification issue with Depth only rendering right after MRT rendering)
    RenderSceneTexture(bHalfSize);
    //Restore depth and stencil with xbox 360 specific implementation
    VisHiZHelper_cl::FastDepthRestore(m_fastRestoreData, m_spDepthOnlyTechnique360->GetShader(0));
  }
#endif

  PopGlobalWireframeState();
}

void VRendererNodeCommon::FreeCustomTextureRefs(VCompiledTechniquePtr &spTech)
{
  if (spTech==NULL)
    return;
  for (int i=0;i<spTech->GetShaderCount();i++)
  {
    VCompiledShaderPass *pPass = spTech->GetShader(0);
    const unsigned int iActiveSamplerCount = pPass->GetActiveSamplerCount(VSS_PixelShader);
    for (unsigned int j=0;j<iActiveSamplerCount;j++)
    {
      VStateGroupTexture *pStateGroupTexture = pPass->GetStateGroupTexture(VSS_PixelShader, j);
      VASSERT(pStateGroupTexture != NULL);
      pStateGroupTexture->m_spCustomTex = NULL;
      pPass->m_bModified = true;
    }
  }
  spTech = NULL; // smart pointer reference
}

void VRendererNodeCommon::RenderOverlays(bool b2dObjects, bool b3dObjects)
{
  VASSERT(Vision::Renderer.IsInRenderLoop());

  const int iRenderFilterMask = Vision::Contexts.GetCurrentContext()->GetRenderFilterMask();

  // Render profiling information (charts, text, diagrams)
  if ((iRenderFilterMask & Vision::Profiling.GetVisibleBitmask()))
  {
    if (b2dObjects)
      Vision::RenderLoopHelper.RenderProfilingInfo(VIS_PROFILINGINFO_TEXT);

    if (b3dObjects)
      Vision::RenderLoopHelper.RenderProfilingInfo(VIS_PROFILINGINFO_GEOMETRY);
  }

  // render 3d lines
  if (b3dObjects)
    Vision::Game.RenderDebugGeometry(VDGRM_3D);

  if (b2dObjects)
  {
    // Trigger "VRH_PRE_SCREENMASKS" render hook callback
    {
      VisRenderHookDataObject_cl data(&Vision::Callbacks.OnRenderHook, VRH_PRE_SCREENMASKS);
      Vision::Callbacks.OnRenderHook.TriggerCallbacks(&data);
    }

    // render all other screen masks on top
    Vision::RenderLoopHelper.RenderScreenMasks();

    // Trigger "VRH_GUI" render hook callback
    {
      VisRenderHookDataObject_cl data(&Vision::Callbacks.OnRenderHook, VRH_GUI);
      Vision::Callbacks.OnRenderHook.TriggerCallbacks(&data);
    }

    // render 2d lines over the screen masks
    Vision::Game.RenderDebugGeometry(VDGRM_2D);

    Vision::Message.HandleMessages();

    if(Vision::GetConsoleManager()->IsVisible())
      Vision::GetConsoleManager()->Render();
  }

  // render visible mesh buffer objects with the render order flag "VRH_AFTER_RENDERING"
  if (b3dObjects)
  {
    DrawMeshBufferObjects(VRH_AFTER_RENDERING);
    //No particle rendering here, because you shouldn't be rendering particles in VRH_AFTER_RENDERING
    VisRenderHookDataObject_cl data(&Vision::Callbacks.OnRenderHook, VRH_AFTER_RENDERING);
    Vision::Callbacks.OnRenderHook.TriggerCallbacks(&data);
  }
}


void VRendererNodeCommon::DrawMeshBufferObjects(unsigned int iRenderOrder)
{
  s_MeshBufferObjectCollection.Clear();

  VisRenderContext_cl *pContext = VisRenderContext_cl::GetCurrentContext();
  int iRenderFlags = pContext->GetRenderFilterMask();
  int iNumMeshBufferObjects = VisMeshBufferObject_cl::ElementManagerGetSize();
  for (int i=0;i<iNumMeshBufferObjects;i++)
  {
    VisMeshBufferObject_cl *pMeshBufferObject = VisMeshBufferObject_cl::ElementManagerGet(i);
    if (!pMeshBufferObject)
      continue;  
    if (!(pMeshBufferObject->GetVisibleBitmask() & iRenderFlags))
      continue;
    if (pMeshBufferObject->GetOrder() != iRenderOrder)
      continue;
    s_MeshBufferObjectCollection.AppendEntry(pMeshBufferObject);
  }

  Vision::RenderLoopHelper.RenderMeshBufferObjects(s_MeshBufferObjectCollection, iRenderOrder);
}





bool VRendererNodeCommon::SetResolveColorBufferFlag(void* pObject, bool bStatus, VResolvedBufferUsage_e flag)
{  
  VASSERT(pObject != NULL);

  if(m_ObjectsUsingResolvedColorBuffer.KeyExists(pObject) == FALSE)
  {
    if(bStatus)
    {
      m_ObjectsUsingResolvedColorBuffer.SetAt(pObject, flag);
      if(flag == VRBU_REQUIRES_RESOLVED)
        m_iAutomaticResolveCounter++;
    }
    UpdateColorBufferResolver();
    return true;
  }

  int iCurrentValue = m_ObjectsUsingResolvedColorBuffer[pObject];
  int iFlag = bStatus ? flag : 0;

  //value is already correct
  if ( (iCurrentValue & flag) == iFlag)
  {
    return true;
  }

  if (bStatus)
  {
    m_ObjectsUsingResolvedColorBuffer.SetAt(pObject, iCurrentValue | flag);
    if(flag == VRBU_REQUIRES_RESOLVED)
      m_iAutomaticResolveCounter++;
  }
  else
  {
    int iNewVal = iCurrentValue & (~flag);
    if(flag == VRBU_REQUIRES_RESOLVED)
      m_iAutomaticResolveCounter--;
    if(iNewVal == 0)
      m_ObjectsUsingResolvedColorBuffer.RemoveKey(pObject);
    else
      m_ObjectsUsingResolvedColorBuffer.SetAt(pObject, iNewVal);
  }

  UpdateColorBufferResolver();
  return true;
}

bool VRendererNodeCommon::ResolveColorBufferManually()
{
  if (m_pColorBufferResolver == NULL)
  {
    Vision::Error.Warning("RendererNode: Trying to resolve color buffer without calling SetResolvesColorBufferManually first!");
    return false;
  }

  m_pColorBufferResolver->ResolveBuffer();
  return true;
}

void VRendererNodeCommon::UpdateColorBufferResolver()
{
  const bool bNeedsColorResolver = m_ObjectsUsingResolvedColorBuffer.GetCount() > 0;
  if (bNeedsColorResolver)
  {
    if (m_pColorBufferResolver == NULL)
    {
      m_pColorBufferResolver = new VBufferResolver(this, GetReferenceContext(), m_uiResolveColorBufferRenderHook);
      if (!m_pColorBufferResolver->Initialize(VBufferResolver::VIM_CreateNewResolveBuffer))
      {
        V_SAFE_DELETE(m_pColorBufferResolver);
        return;
      }
    }

    if(m_iAutomaticResolveCounter > 0)
    {
      m_pColorBufferResolver->SetRenderHook(m_uiResolveColorBufferRenderHook);
    }
    else
    {
      m_pColorBufferResolver->SetRenderHook(VRH_INVALID);
    }
  }
  else
  {
    // If we have an existing color buffer resolver, just disable it - it's very likely we will need it again later.
    if(m_pColorBufferResolver)
    {
      m_pColorBufferResolver->SetRenderHook(VRH_INVALID);
    }
  }
}

bool VRendererNodeCommon::GetRequiresResolvedColorBuffer(void* pObject)
{
  VASSERT(pObject != NULL);
  if(m_ObjectsUsingResolvedColorBuffer.KeyExists(pObject) == FALSE)
    return false;
  return m_ObjectsUsingResolvedColorBuffer[pObject] & VRBU_REQUIRES_RESOLVED;
}

VTextureObject* VRendererNodeCommon::GetResolvedColorBuffer()
{
  if (m_pColorBufferResolver == NULL)
  {
    Vision::Error.Warning("RendererNode: Trying to request color buffer without calling SetRequiresResolvedColorBuffer first!");
    return NULL;
  }
  return m_pColorBufferResolver->GetResolvedBuffer();
}

void VRendererNodeCommon::SetResolveColorBufferRenderHook(unsigned int uiRenderHook)
{
  m_uiResolveColorBufferRenderHook = uiRenderHook;
  if (m_pColorBufferResolver != NULL)
  {
    m_pColorBufferResolver->SetRenderHook(uiRenderHook);
  }
}

void VRendererNodeCommon::Serialize( VArchive &ar )
{
  char iLocalVersion = RENDERERNODEBASE_VERSION_CURRENT;
  if (ar.IsLoading())
  {
    ar >> iLocalVersion;
    VASSERT_MSG(iLocalVersion <= RENDERERNODEBASE_VERSION_CURRENT, "VRendererNodeCommon: Invalid version number");
  }
  else
  {
    ar << iLocalVersion;
  }

  IVRendererNode::Serialize(ar);
}

void VRendererNodeCommon::OnViewPropertiesChanged()
{
  IVRendererNode::OnViewPropertiesChanged();

  for (int i=0; i<Components().Count(); i++)
  {
    if (VPostProcessingBaseComponent *pPostProcessor = vdynamic_cast<VPostProcessingBaseComponent *>(Components().GetAt(i)))
    {
      pPostProcessor->OnViewPropertiesChanged();
    }
  }
}

START_VAR_TABLE(VRendererNodeCommon, IVRendererNode, "VRendererNodeCommon", 0, "")  
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
