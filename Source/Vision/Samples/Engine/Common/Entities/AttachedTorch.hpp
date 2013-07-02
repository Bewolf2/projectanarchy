/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file AttachedTorch.hpp

#ifndef __ATTACHED_TORCH_HPP
#define __ATTACHED_TORCH_HPP

#include <Vision/Runtime/Engine/SceneElements/VisApiParticleGroup.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupManager.hpp>
#include <Vision/Samples/Engine/Common/Entities/BoneAttachedEntity.hpp>

#ifndef _VISION_DOC

class AttachedTorch_cl : public BoneAttachedEntity_cl
{
public:
  AttachedTorch_cl(void);
  virtual ~AttachedTorch_cl(void);

  void Show();
  void Hide();

  VOVERRIDE void ThinkFunction();
  VOVERRIDE void InitFunction();
  VOVERRIDE void DeInitFunction();

  // attachment setup
  VOVERRIDE bool Attach(VisBaseEntity_cl *pAnchorEntity, const char *szAnchorBoneName, const hkvQuat &LocalRotation, const hkvVec3 &LocalTranslation);
  VOVERRIDE bool Detach();


protected:
  VisBaseEntity_cl *m_pShadowCaster;
  bool m_bVisible;
  VisLightSource_cl *m_pLight;

  VisParticleEffectFile_cl* m_pTorchFX;
  VisParticleEffectPtr m_spTorchParticles;

public:
  V_DECLARE_SERIAL( AttachedTorch_cl, )
  IMPLEMENT_OBJ_CLASS(AttachedTorch_cl);
};

#define ENTITY_CAST_SHADOW  0x00000010

#endif // _VISION_DOC

#endif  // __ATTACHED_TORCH_HPP

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
