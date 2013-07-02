/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/MobileForwardRenderer/VFakeSpecularGenerator.hpp>

#include <Vision/Runtime/Engine/Renderer/VisApiFrustumMeshHelper.hpp>

VFakeSpecularGenerator::VFakeSpecularGenerator()
{
  m_spMeshBuffer = VFrustumMeshHelper::CreateMeshBuffer();
  m_fSpecularPower = 1;

  Vision::Shaders.LoadShaderLibrary("Shaders\\FakeSpecularGenerator.ShaderLib", SHADERLIBFLAG_HIDDEN);

  VTechniqueConfig tc;
  VCompiledTechniquePtr spTechnique = Vision::Shaders.CreateTechnique("DrawLight", NULL, &tc);
  m_spShader = spTechnique->GetShader(0);
}

VFakeSpecularGenerator::~VFakeSpecularGenerator()
{
}

void VFakeSpecularGenerator::OnDoRenderLoop(void *pUserData)
{
  VFrustumMeshHelper::UpdateMeshBuffer(m_spMeshBuffer, Vision::Contexts.GetCurrentContext(), VFrustumMeshHelper::IN_WORLD_SPACE);

  Vision::RenderLoopHelper.ClearScreen(VisRenderLoopHelper_cl::VCTF_All, VColorRef(0, 0, 0, 0));

  VisRenderContext_cl* pContext = Vision::Contexts.GetCurrentContext();

  const VisLightSrcCollection_cl* pLights = pContext->GetVisibilityCollector()->GetVisibleLights();

  Vision::RenderLoopHelper.BeginMeshRendering();
  Vision::RenderLoopHelper.ResetMeshStreams();
  Vision::RenderLoopHelper.AddMeshStreams(m_spMeshBuffer, m_spShader->GetStreamMask () | VERTEX_STREAM_INDEXBUFFER);
  for(unsigned int iLightIndex = 0; iLightIndex < pLights->GetNumEntries(); iLightIndex++)
  {
    VisLightSource_cl* pLight = pLights->GetEntry(iLightIndex);

    if((pLight->GetVisibleBitmask() & pContext->GetRenderFilterMask()) == 0)
      continue;

    if(!pLight->GetUseSpecular())
      continue;

    hkvVec4 vDirection;

    hkvVec3 vLightPositionRel = pLight->GetPosition() - pContext->GetCamera()->GetPosition();
    hkvVec3 vLightDirection = pLight->GetDirection();

    float fAttenuation = 1;
    switch(pLight->GetType())
    {
    case VIS_LIGHT_DIRECTED:
      vDirection = vLightDirection.getAsVec4(1.0f);
      break;

    case VIS_LIGHT_SPOTLIGHT:
      {
        vDirection = vLightDirection.getAsVec4(1.0f);

        float fAngle = vLightPositionRel.getAngleBetween(-vLightDirection);
        float fConeAngle = pLight->GetProjectionAngle();
        fAttenuation = hkvMath::clamp((fConeAngle - fAngle) / fConeAngle, 0.0f, 1.0f);

        float fDistance = vLightPositionRel.getLength();
        float fRadius = pLight->GetRadius();
        fAttenuation *= hkvMath::clamp((fRadius - fDistance) / fRadius, 0.0f, 1.0f);
      }
      break;

    case VIS_LIGHT_POINT:
      vDirection = (-vLightPositionRel).getNormalized().getAsVec4(1.0f);
      float fDistance = vLightPositionRel.getLength();
      float fRadius = pLight->GetRadius();
      fAttenuation = hkvMath::clamp((fRadius - fDistance) / fRadius, 0.0f, 1.0f);
      break;
    }

    hkvVec4 vColor = pLight->GetColor().getAsVec4() * pLight->GetMultiplier() * fAttenuation;

    hkvVec4 vParams(m_fSpecularPower, 0, 0, 0);

    m_spShader->GetConstantBuffer(VSS_PixelShader)->SetSingleParameterF("fLightDirection", vDirection.data);
    m_spShader->GetConstantBuffer(VSS_PixelShader)->SetSingleParameterF("fLightColor", vColor.data);
    m_spShader->GetConstantBuffer(VSS_PixelShader)->SetSingleParameterF("fParams", vParams.data);

    Vision::RenderLoopHelper.RenderMeshes(m_spShader, VisMeshBuffer_cl::MB_PRIMTYPE_INDEXED_TRILIST, 0, 2, 6);
  }
  Vision::RenderLoopHelper.EndMeshRendering();

  // Trigger pre-screenmask render hook to make the attached cubemap handle flip and blur the cubemap target
  VisRenderHookDataObject_cl data(&Vision::Callbacks.OnRenderHook, VRH_PRE_SCREENMASKS);
  Vision::Callbacks.OnRenderHook.TriggerCallbacks(&data);
}

V_IMPLEMENT_DYNAMIC(VFakeSpecularGenerator, VisTypedEngineObject_cl, &g_VisionEngineModule);

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
