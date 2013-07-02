/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file AttachedDagger.hpp

#ifndef ATTACHED_WEAPON_HPP_INCLUDED
#define ATTACHED_WEAPON_HPP_INCLUDED


#ifndef _VISION_DOC

class AttachedDagger_cl : public VisBaseEntity_cl
{
public:
  AttachedDagger_cl(void);
  ~AttachedDagger_cl(void);

  // attachment setup
  bool Attach(VisBaseEntity_cl *pAnchorEntity, const char *szAnchorBoneName, const hkvQuat &LocalRotation, const hkvVec3 &LocalTranslation);
  void Detach();

  // updates the current weapon position
  void Update();

protected:
  bool m_bIsAttached;

  // attachment settings
  int m_iAnchorBoneIndex;
  hkvQuat m_LocalRotation;
  hkvVec3 m_LocalTranslation;
  VisBaseEntity_cl *m_pAnchorEntity;

public:
  V_DECLARE_SERIAL( AttachedDagger_cl, )
  IMPLEMENT_OBJ_CLASS(AttachedDagger_cl);
};

#endif // _VISION_DOC

#endif  // ATTACHED_WEAPON_HPP_INCLUDED

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
