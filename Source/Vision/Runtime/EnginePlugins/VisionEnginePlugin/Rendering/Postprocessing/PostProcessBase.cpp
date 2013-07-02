/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/PostProcessBase.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RendererNode/VRendererNodeCommon.hpp>

V_IMPLEMENT_DYNAMIC(VPostProcessingBaseComponent, IVObjectComponent, &g_VisionEngineModule);

VPostProcessingBaseComponent::VPostProcessingBaseComponent()
{
  m_bValid = true;
  m_bActive = true;
  m_iRequiredBufferFlags = 0;
  m_iNumSourceTextures = 0;
  m_bIsInitialized = false;
  m_fPriority = VIS_RENDERCONTEXTPRIORITY_POSTPROCESSOR;

  m_iPropertyUpdateCounter = 0;
  m_bReinitializationRequired = false;

  m_bufferUsageFlags = SAMPLES_ACCUMULATION_BUFFER;

  m_iTargetIndex = -1;

  Vision::Callbacks.OnReassignShaders += this;
}

VPostProcessingBaseComponent::~VPostProcessingBaseComponent()
{
  VASSERT_MSG(!IsInitialized(), "Post processors is being destroyed, but wasn't deinitialized - this should never happen!");
  VASSERT_MSG(m_iPropertyUpdateCounter == 0, "Post processor is being destroyed inside a property update block!");

  Vision::Callbacks.OnReassignShaders -= this;
}

VRendererNodeCommon* VPostProcessingBaseComponent::GetOwner() const
{
  return vstatic_cast<VRendererNodeCommon*>(m_pOwner);
}

void VPostProcessingBaseComponent::SetActive(bool bStatus)
{
  if(m_bActive == bStatus)
  {
    return;
  }

  m_bActive = bStatus;

  if(VRendererNodeCommon* pNode = vstatic_cast<VRendererNodeCommon*>(m_pOwner))
  {
    pNode->OnPostProcessorChanged();
  }
}

void VPostProcessingBaseComponent::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender == &Vision::Callbacks.OnReassignShaders)
  {
    if (!m_bIsInitialized)
      return;

    DeInitializePostProcessor();
    InitializePostProcessor();
  }
}

void VPostProcessingBaseComponent::SetOwner(VisTypedEngineObject_cl *pOwner)
{
  if (pOwner == m_pOwner)
    return;

  if (m_bIsInitialized)
    DeInitializePostProcessor();

  IVObjectComponent::SetOwner(pOwner);
}

BOOL VPostProcessingBaseComponent::CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
  if (!IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut))
  {
    return FALSE;
  }

  VRendererNodeCommon *pNode = vdynamic_cast<VRendererNodeCommon*>(pObject);

  if (pNode == NULL)
  {
    sErrorMsgOut = "Component can only be added to instances of VRendererNodeCommon or derived classes.";
    return FALSE;
  }

  if ( (pNode->GetSupportedBufferFlags() & m_iRequiredBufferFlags) != m_iRequiredBufferFlags )
  {
    sErrorMsgOut = "Active renderer does not provide all buffers required by the post processor.";
    return FALSE;
  }

  return TRUE;
}

VisRenderContext_cl *VPostProcessingBaseComponent::GetTargetContext()
{
  if(m_iTargetIndex == -1)
  {
    return NULL;
  }

  return GetOwner()->m_assignedContexts.GetAt(m_iTargetIndex);
}


V_IMPLEMENT_DYNAMIC(PostProcessRenderLoop_cl, IVObjectComponent, &g_VisionEngineModule);

PostProcessRenderLoop_cl::PostProcessRenderLoop_cl(VPostProcessingBaseComponent *pPostProcessor)
{
  VASSERT(pPostProcessor!=NULL);
  m_spPostProcessor = pPostProcessor;
}

PostProcessRenderLoop_cl::~PostProcessRenderLoop_cl()
{
}

void PostProcessRenderLoop_cl::OnDoRenderLoop(void *pUserData)
{
  if (!m_spPostProcessor->IsActive())
    return;

#if defined(_VISION_XENON)

  VisRenderContext_cl* pCurrentContext = VisRenderContext_cl::GetCurrentContext();
  VRendererNodeCommon* pRendererNode = vstatic_cast<VRendererNodeCommon*>(Vision::Renderer.GetCurrentRendererNode());

  // On Xbox360 resolved MSAA/tiled render buffer has to be rendered back to the 
  // post processor render target in order to be able to execute post processors that use blending
  // right after the initial pass.
  // This only needs to be done for the first post processor.
  const VisRenderableTextureConfig_t* pConfig = pCurrentContext->GetTargetConfig();

  if (pConfig != NULL && pConfig->m_bUsesPredicatedTiling)
  {
    INSERT_PERF_MARKER_SCOPE("Auto-restoring render target content for blending");
    VPostProcessingBaseComponent* pFirstPostProcessor = GetNextPostProcessor(pRendererNode, -FLT_MAX);

    // Check if the current render context is the first post processor and uses alpha blending.
    if (pFirstPostProcessor == m_spPostProcessor && ((pFirstPostProcessor->GetBufferUsageFlags() & VPostProcessingBaseComponent::USES_BLENDING) != 0))
    {
      // Upload the resolved texture to the post processing render target in the EDRAM.
      IVRender2DInterface* pRenderer2D = Vision::RenderLoopHelper.BeginOverlayRendering();
      const VSimpleRenderState_t iRenderState(VIS_TRANSP_NONE, RENDERSTATEFLAG_FRONTFACE|RENDERSTATEFLAG_ALWAYSVISIBLE);

      pRenderer2D->Draw2DBuffer(6, pRendererNode->GetRendererNodeHelper()->GetOverlayVertices(), 
        pRendererNode->GetGBuffer(IVRendererNode::VGBT_Accumulation), iRenderState);

      Vision::RenderLoopHelper.EndOverlayRendering();
    }
  }

#endif

  // Execute post processor
  m_spPostProcessor->Execute();

#if defined(_VISION_XENON)
  VPostProcessingBaseComponent* pNextPostProcessor = GetNextPostProcessor(pRendererNode, m_spPostProcessor->GetPriority());

  // Manually resolve the accumulation buffer, but only if the next post-processor actually needs it. Needs to be done here
  // since resolving must be done while the current context's render target is still active.
  if(pNextPostProcessor && (pNextPostProcessor->GetBufferUsageFlags() & VPostProcessingBaseComponent::SAMPLES_ACCUMULATION_BUFFER))
  {
    // Not needed if the render target resolves automatically
    if(!static_cast<VisRenderableTexture_cl*>(pCurrentContext->GetRenderTarget(0))->GetConfig()->m_bResolve)
    {
      INSERT_PERF_MARKER_SCOPE("Auto-resolving accumulation buffer for next PP");
      pCurrentContext->ResolveToTexture(pRendererNode->GetGBuffer(IVRendererNode::VGBT_Accumulation));
    }
  }
#endif
}


VPostProcessingBaseComponent *PostProcessRenderLoop_cl::GetNextPostProcessor(IVRendererNode* pRenderer, float fMinPriority)
{
  VPostProcessingBaseComponent* pFirstPostProcessor = NULL;
  float fBestPriority = FLT_MAX;

  // Find first post processor.
  for (int i = 0; i < pRenderer->Components().Count(); i++)
  {
    VPostProcessingBaseComponent* pPostProcessor = vdynamic_cast<VPostProcessingBaseComponent*>(pRenderer->Components().GetAt(i));

    if(pPostProcessor == NULL)
    {
      continue;
    }

    if (pPostProcessor->IsActive() &&
      pPostProcessor->GetPriority() < fBestPriority &&
      pPostProcessor->GetPriority() > fMinPriority)
    {
      pFirstPostProcessor = pPostProcessor;
      fBestPriority = pPostProcessor->GetPriority();
    }
  }

  return pFirstPostProcessor;
}

START_VAR_TABLE(VPostProcessingBaseComponent, IVObjectComponent, "VPostProcessingBaseComponent", 0, "")
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
