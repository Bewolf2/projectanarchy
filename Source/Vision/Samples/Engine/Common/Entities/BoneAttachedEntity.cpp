/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Samples/Engine/Common/Entities/BoneAttachedEntity.hpp>

// ***********************************************************************************************
// BoneAttachedEntity_cl
// ***********************************************************************************************
BoneAttachedEntity_cl::BoneAttachedEntity_cl(void)
{
  m_iAnchorBoneIndex = -1;
  m_bIsAttached = false;
  m_pAnchorEntity = NULL;
}

BoneAttachedEntity_cl::~BoneAttachedEntity_cl(void)
{
  Detach();
}

bool BoneAttachedEntity_cl::Attach(VisBaseEntity_cl *pAnchorEntity, const char *szAnchorBoneName, const hkvQuat &localRotation, const hkvVec3 &localTranslation)
{
  if (m_bIsAttached)
    Detach();

  if (!pAnchorEntity || !szAnchorBoneName)
    return false;

  // find index of bone the weapon is attached to
  VisSkeleton_cl *pSkeleton = pAnchorEntity->GetMesh()->GetSkeleton();
  m_iAnchorBoneIndex = pSkeleton->GetBoneIndexByName(szAnchorBoneName);

  // store attachment settings
  m_LocalRotation = localRotation;
  m_LocalTranslation = localTranslation;
  m_pAnchorEntity = pAnchorEntity;
  m_bIsAttached = true;

  // initial update
  Update();

  return true;
}

bool BoneAttachedEntity_cl::Detach()
{
  if(!m_bIsAttached)
    return false;
  
  m_bIsAttached = NULL;
  m_pAnchorEntity = NULL;

  return true;
}

void BoneAttachedEntity_cl::ThinkFunction()
{
  Update();
}

void BoneAttachedEntity_cl::Update()
{
  if (!m_bIsAttached)
    return;

  hkvQuat newRotation;
  hkvVec3 newTranslation, tempTranslation;

  //// calculate new transformation
  hkvVec3 boneWorldSpaceTranslation;
  hkvQuat boneWorldSpaceRotation;
  m_pAnchorEntity->GetBoneCurrentWorldSpaceTransformation(m_iAnchorBoneIndex, boneWorldSpaceTranslation, boneWorldSpaceRotation);

  //don't update if nothing changed
  //if(boneWorldSpaceTranslation == m_vBoneWorldSpaceTranslation && boneWorldSpaceRotation == m_qBoneWorldSpaceRotation)
  //{
  //  return;
  //} else
  //{
  //  m_vBoneWorldSpaceTranslation = boneWorldSpaceTranslation;
  //  m_qBoneWorldSpaceRotation = boneWorldSpaceRotation;
  //}

  //calculate new position and orientation
  newRotation = m_LocalRotation.multiplyReverse (boneWorldSpaceRotation);
  newTranslation = boneWorldSpaceRotation.transform(m_LocalTranslation);
  newTranslation += boneWorldSpaceTranslation;

  // set new transformation
  SetPosition(newTranslation);
  hkvMat3 m = newRotation.getAsMat3 ();
  SetRotationMatrix(m);
}

V_IMPLEMENT_SERIAL( BoneAttachedEntity_cl, VisBaseEntity_cl, 0, Vision::GetEngineModule() );
START_VAR_TABLE(BoneAttachedEntity_cl, VisBaseEntity_cl, "An entity that can be attached to another entity via a bone (e.g. weapon to character)", VFORGE_HIDECLASS, "Models\\Items\\Sword.model")  
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
