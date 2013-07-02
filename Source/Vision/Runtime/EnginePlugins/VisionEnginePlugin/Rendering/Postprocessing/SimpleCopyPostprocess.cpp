/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/SimpleCopyPostprocess.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/RenderingOptimizationHelpers.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RendererNode/VRendererNodeCommon.hpp>


VSimpleCopyPostprocess::VSimpleCopyPostprocess()
{
  m_iRequiredBufferFlags = VBUFFERFLAG_FINALCOLOR;
  m_fPriority = VPOSTPROCESSOR_PRIORITY_SIMPLE_COPY;
  m_bufferUsageFlags = SAMPLES_ACCUMULATION_BUFFER | DRAWS_FULLSCREEN_QUAD;
}

VSimpleCopyPostprocess::~VSimpleCopyPostprocess()
{
}

void VSimpleCopyPostprocess::RemoveContext()
{
  GetOwner()->RemoveContext(GetTargetContext());
}

void VSimpleCopyPostprocess::SetupContext()
{
  VASSERT_MSG(GetOwner() != NULL, "Postprocessing effects require a valid renderer node!");

  GetOwner()->AddContext(GetTargetContext());

  m_spSourceTextures[0] = GetOwner()->GetGBuffer(IVRendererNode::VGBT_Accumulation);
  m_iNumSourceTextures = 1;
  VASSERT(m_spSourceTextures[0] != NULL);
}


void VSimpleCopyPostprocess::InitializePostProcessor()
{
  if (m_bIsInitialized || !m_bActive)
    return;

  VASSERT_MSG(GetOwner()->IsInitialized(), "The renderer node that owns this post processor must be initialized before initializing the post processor.");

  SetupContext();

  m_bIsInitialized = true;
}

// ----------------------------------------------------------------------------------------
// Free all resources again
// ----------------------------------------------------------------------------------------
void VSimpleCopyPostprocess::DeInitializePostProcessor()
{
  if (!m_bIsInitialized)
    return;

  RemoveContext();

  m_bIsInitialized = false;
}

void VSimpleCopyPostprocess::Execute()
{
  if (!IsActive() || !m_bIsInitialized)
    return;

  INSERT_PERF_MARKER_SCOPE("VSimpleCopyPostprocess");

  RenderingOptimizationHelpers_cl::SetShaderPreference(112);

  int iWidth, iHeight;
  VisRenderContext_cl *pContext = VisRenderContext_cl::GetCurrentContext();
  pContext->GetSize(iWidth, iHeight);

  Vision::RenderLoopHelper.SetScissorRect(NULL);
  Vision::RenderLoopHelper.ClearScreen();

  // On DX9 a half pixel shift is required for the copy full screen pass.
#if defined(_VR_DX9)
  const hkvVec2 texelShift(1.0f / (float)(iWidth*2), 1.0f / (float)(iHeight*2));
#else
  const hkvVec2 texelShift(0.0f, 0.0f);
#endif

  VSimpleRenderState_t iState(VIS_TRANSP_NONE,RENDERSTATEFLAG_FRONTFACE|RENDERSTATEFLAG_ALWAYSVISIBLE|RENDERSTATEFLAG_NOWIREFRAME|RENDERSTATEFLAG_NOMULTISAMPLING);
  IVRender2DInterface *pRI = Vision::RenderLoopHelper.BeginOverlayRendering();
  pRI->DrawTexturedQuad(hkvVec2(0.f,0.f), hkvVec2((float)iWidth, (float)iHeight), m_spSourceTextures[0], hkvVec2(0.0f) + texelShift, hkvVec2(1.0f) + texelShift, V_RGBA_WHITE, iState);
  Vision::RenderLoopHelper.EndOverlayRendering();
}

V_IMPLEMENT_SERIAL( VSimpleCopyPostprocess, VPostProcessingBaseComponent, 0, &g_VisionEngineModule );

void VSimpleCopyPostprocess::Serialize( VArchive &ar )
{
  if (ar.IsLoading())
  {
    ar >> m_bActive;

    VRendererNodeCommon* pDummy;
    ar >> pDummy;
  }
  else
  {
    ar << m_bActive;
    ar << GetOwner();
  }
}

START_VAR_TABLE(VSimpleCopyPostprocess, VPostProcessingBaseComponent, "VSimpleCopyPostprocess", VFORGE_HIDECLASS, "")  
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
