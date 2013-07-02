/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/VPostProcessScreenMasks.hpp>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/RenderingOptimizationHelpers.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RendererNode/VRendererNodeCommon.hpp>


VPostProcessScreenMasks::VPostProcessScreenMasks() 
{
  m_iRequiredBufferFlags = VBUFFERFLAG_FINALCOLOR;
  m_fPriority = VPOSTPROCESSOR_PRIORITY_SCREENMASKS;
  m_bufferUsageFlags = USES_BLENDING;
  m_bRender2D = true;
  m_bRender3D = true;
}

VPostProcessScreenMasks::~VPostProcessScreenMasks()
{
}

void VPostProcessScreenMasks::RemoveContext()
{
  GetOwner()->RemoveContext(GetTargetContext());
}

void VPostProcessScreenMasks::SetupContext()
{
  VASSERT_MSG(vdynamic_cast<VRendererNodeCommon*>(m_pOwner) != NULL, "Postprocessing effects require a valid renderer node!");
  VRendererNodeCommon* pRenderNode = GetOwner();
  
  VisRenderContext_cl* pTargetContext = GetTargetContext();
  
  pRenderNode->AddContext(pTargetContext);
}


void VPostProcessScreenMasks::InitializePostProcessor()
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
void VPostProcessScreenMasks::DeInitializePostProcessor()
{
  if (!m_bIsInitialized)
    return;

  m_bIsInitialized = false;

  RemoveContext();
}

void VPostProcessScreenMasks::Execute()
{
  if (!IsActive() || !m_bIsInitialized)
    return;

  INSERT_PERF_MARKER_SCOPE("VPostProcessScreenMasks");

  RenderingOptimizationHelpers_cl::SetShaderPreference(112);
  
  VRendererNodeCommon::RenderOverlays(m_bRender2D, m_bRender3D);
}

V_IMPLEMENT_SERIAL( VPostProcessScreenMasks, VPostProcessingBaseComponent, 0, &g_VisionEngineModule );

void VPostProcessScreenMasks::Serialize( VArchive &ar )
{
  char iLocalVersion = 0;
  if (ar.IsLoading())
  {
    ar >> iLocalVersion;
    ar >> m_bActive;
  }
  else
  {
    ar << iLocalVersion;
    ar << m_bActive;
  }
}

START_VAR_TABLE(VPostProcessScreenMasks, VPostProcessingBaseComponent, "VPostProcessScreenMasks", VFORGE_HIDECLASS, "")  
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
