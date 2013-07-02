/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file MergedModelFactory.hpp

#ifndef MERGEDMODELFACTORY_HPP_INCLUDED
#define MERGEDMODELFACTORY_HPP_INCLUDED

#include <Vision/Samples/Engine/ModelMerging/KeyControlledTransitionBarbarian.hpp>
#include <Vision/Samples/Engine/ModelMerging/TransitionBarbarian.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Components/VPlayerCamera.hpp>

enum BODY_PART
{
  BARBARIAN_BODY = 0,
  BARBARIAN_ARM,
  BARBARIAN_SHOULDER,
  BARBARIAN_LEGS,
  BARBARIAN_KNEE,
  BARBARIAN_ACCESSOIRE,
  BARBARIAN_BELT,
  BARBARIAN_CLOTH,
  BARBARIAN_BEARD,
  BARBARIAN_AXE,
  BARBARIAN_SWORD,
  BARBARIAN_MAX = BARBARIAN_SWORD + 1,
};


// Merged Model Entity
class MergedModelFactory_cl
{
public:
  MergedModelFactory_cl();
  ~MergedModelFactory_cl();

  void ToggleBodyPart(BODY_PART part);
  void MergeModel();
  void PreviewModel();
  void UpdatePreview();
  void DeleteModels();

private:

  KeyControlledTransitionBarbarian_cl *m_pMergedModelEntity;
  VisBaseEntity_cl **m_pPreviewModelEntities;
  VisBaseEntity_cl *m_pCameraEntity;
  VPlayerCamera *m_pPlayerCamera;
  int m_iModelsToMerge;

  // Position and Orientation
  hkvVec3 m_vPos;
  hkvVec3 m_vOri;

  // Body states
  bool m_bArmArmor;
  bool m_bShoulderArmor;  
  bool m_bLegsArmor;
  bool m_bKneeArmor;
  bool m_bAccessoire;
  bool m_bBelt;
  bool m_bCloth;
  bool m_bBeard;
  bool m_bAxe;

};

#endif  // MERGEDMODELFACTORY_HPP_INCLUDED

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
