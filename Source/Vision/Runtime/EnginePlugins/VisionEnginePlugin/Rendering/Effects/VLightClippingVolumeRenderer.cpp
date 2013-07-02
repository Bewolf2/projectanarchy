/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/VLightClippingVolumeRenderer.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Components/VLightClippingVolumeComponent.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/VCustomVolumeObject.hpp>

bool VLightClippingVolumeRenderer::RenderLightClippingVolumeHelper(VisLightSource_cl *pLight,VStateGroupDepthStencil& sg)
{
  VLightClippingVolumeComponent* pComponent = pLight->Components().GetComponentOfBaseType<VLightClippingVolumeComponent>();
  if(pComponent != NULL && pComponent->GetVolume() != NULL && m_spLightClippingVolumeStencilFill)
  {
    INSERT_PERF_MARKER_SCOPE("VLightClippingVolumeRenderer::RenderLightClippingVolume");

    VisRenderStates_cl::SetDepthStencilState(sg);

    VCustomVolumeObject* pVolume = pComponent->GetVolume();

    VisStaticMesh_cl* pStaticMesh = pVolume->GetStaticMesh();
    if(!pStaticMesh)
      return false;

    VisMeshBuffer_cl* pMeshBuffer = pStaticMesh->GetMeshBuffer();
    if(!pMeshBuffer || pMeshBuffer->GetIndexCount() <= 0)
      return false;

    Vision::RenderLoopHelper.BeginMeshRendering();
    Vision::RenderLoopHelper.ResetMeshStreams();
    Vision::RenderLoopHelper.AddMeshStreams(pMeshBuffer, m_spLightClippingVolumeStencilFill->GetShader(0)->GetStreamMask() | VERTEX_STREAM_INDEXBUFFER);

    hkvMat4 transform;
    transform.setIdentity ();
    transform.setRotationalPart(pVolume->GetRotationMatrix());
    transform.setTranslation(pVolume->GetPosition());
    transform.setScalingFactors(pVolume->GetScale());

    Vision::RenderLoopHelper.SetMeshTransformationMatrix(transform);

    Vision::RenderLoopHelper.RenderMeshes(m_spLightClippingVolumeStencilFill->GetShader(0), pMeshBuffer->GetPrimitiveType(), 0, pMeshBuffer->GetIndexCount() / 3, pMeshBuffer->GetVertexCount());
    Vision::RenderLoopHelper.EndMeshRendering();

    return true;
  }
  return false;
}

bool VLightClippingVolumeRenderer::RenderLightClippingVolume(VisLightSource_cl *pLight)
{
  return RenderLightClippingVolumeHelper(pLight, m_LightClippingStencilStateWrite);
}

void VLightClippingVolumeRenderer::ClearLightClippingVolume(VisLightSource_cl *pLight)
{
  RenderLightClippingVolumeHelper(pLight, m_LightClippingStencilStateClear);
}

void VLightClippingVolumeRenderer::Init()
{
  m_LightClippingStencilStateWrite = *VisRenderStates_cl::GetDepthStencilDefaultState();
  m_LightClippingStencilStateWrite.m_bDepthTestEnabled = true;
  m_LightClippingStencilStateWrite.m_bDepthWriteEnabled = false;
  m_LightClippingStencilStateWrite.m_bStencilTestEnabled = true;
  m_LightClippingStencilStateWrite.m_bTwoSidedStencil = false; 
  m_LightClippingStencilStateWrite.m_cStencilComparisonFunc[0] =
  m_LightClippingStencilStateWrite.m_cStencilComparisonFunc[1] = COMPARISON_ALWAYS;
  m_LightClippingStencilStateWrite.m_cDepthComparisonFunc = COMPARISON_GREATER;
  //Front face depth pass (because of inverse test function)
  m_LightClippingStencilStateWrite.m_cStencilPassOp[1] =
  m_LightClippingStencilStateWrite.m_cStencilPassOp[0] = STENCIL_OP_INVERT;
  m_LightClippingStencilStateWrite.m_iStencilReadMask = 
  m_LightClippingStencilStateWrite.m_iStencilWriteMask = V_BIT(V_LIGHT_CLIPPING_STENCIL_BIT);
  m_LightClippingStencilStateWrite.ComputeHash();

  m_LightClippingStencilStateClear = *VisRenderStates_cl::GetDepthStencilDefaultState();
  m_LightClippingStencilStateClear.m_bDepthTestEnabled = false;
  m_LightClippingStencilStateClear.m_bDepthWriteEnabled = false;
  m_LightClippingStencilStateClear.m_bStencilTestEnabled = true;
  m_LightClippingStencilStateClear.m_bTwoSidedStencil = false;
  m_LightClippingStencilStateClear.m_cStencilComparisonFunc[0] =
  m_LightClippingStencilStateClear.m_cStencilComparisonFunc[1] = COMPARISON_ALWAYS;
  m_LightClippingStencilStateClear.m_cDepthComparisonFunc = COMPARISON_GREATER;
  m_LightClippingStencilStateClear.m_cStencilPassOp[1] =
  m_LightClippingStencilStateClear.m_cStencilPassOp[0] = STENCIL_OP_REPLACE;
  m_LightClippingStencilStateClear.m_iStencilWriteMask = V_BIT(V_LIGHT_CLIPPING_STENCIL_BIT);
  m_LightClippingStencilStateClear.m_iStencilRef = 0;
  m_LightClippingStencilStateClear.ComputeHash();

  if (!Vision::Shaders.LoadShaderLibrary("\\Shaders/LightClippingVolumes.ShaderLib", SHADERLIBFLAG_HIDDEN))
  {
    Vision::Error.Warning("Shader lib file for light clipping volumes could not be loaded!");
    return;
  }

  VCompiledEffectPtr effect = Vision::Shaders.CreateEffect("LightClippingVolumesStencilFill", NULL);
  if(effect)
  {
    m_spLightClippingVolumeStencilFill = effect->GetDefaultTechnique();
    VASSERT_MSG( m_spLightClippingVolumeStencilFill.GetPtr() != NULL, "Unable to retrieve default technique for LightClippingVolumesStencilFill shader!" );
    m_spLightClippingVolumeStencilFill->GetShader(0)->DisablePixelShader();

    int iStateGroupMask = STATEGROUP_ALL & ~STATEGROUP_DEPTHSTENCIL;
    m_spLightClippingVolumeStencilFill->GetShader(0)->m_cStateGroupMask = iStateGroupMask;
  }
}

void VLightClippingVolumeRenderer::DeInit()
{
  m_spLightClippingVolumeStencilFill = NULL;
}

void VLightClippingVolumeRenderer::PatchReadDepthStencilState(const VStateGroupDepthStencil& inputState, VStateGroupDepthStencil* pOutputStates, bool bEnableStencil)
{
  VASSERT(pOutputStates != NULL);
  for (unsigned int i=0;i<2;i++)
  {
    pOutputStates[i] = inputState;
    if (bEnableStencil)
      pOutputStates[i].m_bStencilTestEnabled = true;
    pOutputStates[i].m_iStencilRef = V_BIT(V_LIGHT_CLIPPING_STENCIL_BIT);
    pOutputStates[i].m_iStencilReadMask |= V_BIT(V_LIGHT_CLIPPING_STENCIL_BIT);
    pOutputStates[i].m_cStencilComparisonFunc[0] = (i==0) ? COMPARISON_EQUAL : COMPARISON_NOT_EQUAL;
  }
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
