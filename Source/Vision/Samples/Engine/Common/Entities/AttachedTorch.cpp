/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Samples/Engine/Common/Entities/AttachedTorch.hpp>

#if defined(SUPPORTS_SHADOW_MAPS) && !defined(_VISION_MOBILE) && !defined( HK_ANARCHY )
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VShadowMapComponentPoint.hpp>
#endif

// ***********************************************************************************************
// AttachedTorch_cl
// ***********************************************************************************************
AttachedTorch_cl::AttachedTorch_cl(void)
{
  m_pLight = NULL;
  m_spTorchParticles = NULL;
  m_pTorchFX = NULL;
  m_bVisible = false;
  m_pShadowCaster = NULL;
}

AttachedTorch_cl::~AttachedTorch_cl(void)
{
  Hide();
  Detach();
  V_SAFE_DISPOSEOBJECT(m_pLight);
}

void AttachedTorch_cl::ThinkFunction()
{
  //BoneAttachedEntity_cl::ThinkFunction();
  Update();

  //offset of the light
  hkvVec3 pos = hkvVec3(0.f, 0.f, 40.f);
  pos = GetRotationMatrix() * pos;
  pos += GetPosition();

  if(m_pLight)
  {
    m_pLight->SetPosition(pos);
    float t = Vision::GetTimer()->GetTime()*4.f;
    float fNoise = hkvMath::sinRad (t*3.f)+0.5f*hkvMath::sinRad (t*7.5f)+0.3f*hkvMath::sinRad (t*11.7f);
    m_pLight->SetMultiplier(1.5f+fNoise*0.12f);
  }

  hkvVec3 vParticlePos = pos;
  vParticlePos.z -= 8.f;
  vParticlePos.x += 5.f;
  if(m_spTorchParticles)
    m_spTorchParticles->SetPosition(pos);
}

void AttachedTorch_cl::InitFunction()
{
  SetVisibleBitmask( 0 );
  SetCastShadows(FALSE);
  m_pShadowCaster = Vision::Game.CreateEntity("VisBaseEntity_cl", GetPosition(), "Models\\Warrior\\TorchBottom.MODEL");
  m_pShadowCaster->AttachToParent(this);
  m_pShadowCaster->SetVisibleBitmask( 0 );
  m_pShadowCaster->SetCastShadows( FALSE );
  hkvVec3 vScale(2.5f, 2.5f, 1.0f);
  m_pShadowCaster->SetScaling(vScale);
}

void AttachedTorch_cl::DeInitFunction()
{
  Hide();

  m_pShadowCaster->DetachFromParent();
  Vision::Game.RemoveEntity(m_pShadowCaster);
  m_pShadowCaster = NULL;
}

void AttachedTorch_cl::Hide()
{
  if (!m_bVisible) return;

  m_bVisible = false;

  SetVisibleBitmask( 0 );
  SetCastShadows(FALSE);

  m_pShadowCaster->SetVisibleBitmask( 0 );
  m_pShadowCaster->SetCastShadows(FALSE);

  V_SAFE_DISPOSEOBJECT(m_pLight);
  V_SAFE_DISPOSEOBJECT(m_spTorchParticles);
}

void AttachedTorch_cl::Show()
{
  if (m_bVisible) return;

  m_bVisible = true;

  SetVisibleBitmask( 1 );
  SetCastShadows(FALSE);

  m_pShadowCaster->SetVisibleBitmask( ENTITY_CAST_SHADOW );
  m_pShadowCaster->SetCastShadows(TRUE);

  V_SAFE_DISPOSEOBJECT(m_pLight);

  if (m_spTorchParticles)
  {
    m_spTorchParticles->Dispose(); 
    m_spTorchParticles = NULL;
  }

  m_pTorchFX = VisParticleGroupManager_cl::GlobalManager().LoadFromFile("Models\\Warrior\\TorchFire.xml");
  VASSERT(m_pTorchFX);
  m_pTorchFX->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);

  m_pLight = new VisLightSource_cl(VIS_LIGHT_POINT, 800);
  VASSERT(m_pLight);

  m_pLight->SetMultiplier(1.5f);
  m_pLight->SetLightInfluenceBitMasks(0xFFFFFFFF, 0xFFFFFFFF);
  m_pLight->SetColor(VColorRef(255,180,105));
  m_pLight->SetAttenuation(VIS_LIGHT_ATTEN_CUSTOM);

#if defined(SUPPORTS_SHADOW_MAPS) && !defined(_VISION_MOBILE) && !defined( HK_ANARCHY )
  VShadowMapComponentPoint* pShadowMapComponent = new VShadowMapComponentPoint();
  pShadowMapComponent->SetShadowMapSize(256);
  pShadowMapComponent->SetShadowMappingMode(SHADOW_MAPPING_MODE_PCF8);
  m_pLight->AddComponent(pShadowMapComponent);
#endif

  hkvVec3 vPdir(0.0f, -90.0f, 0.0f);
  m_spTorchParticles = m_pTorchFX->CreateParticleEffectInstance(GetPosition(), vPdir);
}


bool AttachedTorch_cl::Attach(VisBaseEntity_cl *pAnchorEntity, const char *szAnchorBoneName, const hkvQuat &localRotation, const hkvVec3 &localTranslation)
{
  if(BoneAttachedEntity_cl::Attach(pAnchorEntity, szAnchorBoneName, localRotation, localTranslation))
    return true;

  return false;
}

bool AttachedTorch_cl::Detach()
{
  if(BoneAttachedEntity_cl::Detach())
    return true;
  
  return false;
}


V_IMPLEMENT_SERIAL( AttachedTorch_cl, BoneAttachedEntity_cl, 0, Vision::GetEngineModule() );
START_VAR_TABLE(AttachedTorch_cl, BoneAttachedEntity_cl, "An attachable torch with particle effects", VFORGE_HIDECLASS, "Models\\Warrior\\Torch.MODEL")
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
