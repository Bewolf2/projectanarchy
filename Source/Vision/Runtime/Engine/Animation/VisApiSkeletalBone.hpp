/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiSkeletalBone.hpp

#ifndef VIS_BONE_HPP_INCLUDED
#define VIS_BONE_HPP_INCLUDED

#include <Vision/Runtime/Base/Math/Quaternion/hkvQuat.h>

/// \brief
///   Bone class to hold bone specific data.
/// 
/// All data is public for direct access.
class VisSkeletalBone_cl
{

public:

  /// \brief
  ///   Constructor
  VISION_APIFUNC VisSkeletalBone_cl();


  /// \brief
  ///   Constructor of bone class which takes some data for initialization
  /// 
  /// \param szBoneName
  ///   name of the bone
  /// 
  /// \param iParentIndex
  ///   parent bone index
  VISION_APIFUNC VisSkeletalBone_cl(const char *szBoneName,int iParentIndex);


  /// \brief
  ///   Destructor
  VISION_APIFUNC ~VisSkeletalBone_cl();


  /// \brief
  ///   Compares all properties of two bones.
  /// 
  /// \param pOtherSkeletalBone
  ///   specifies the bone to compare this with
  /// 
  /// \return
  ///   bool bResult: true if all bone properties are identical
  VISION_APIFUNC bool Equals(const VisSkeletalBone_cl *pOtherSkeletalBone) const;

  /// \brief
  ///   Assignment operator that copies all relevant information
  inline VisSkeletalBone_cl& operator = (const VisSkeletalBone_cl &other)
  {
    m_sBoneName = other.m_sBoneName;
    m_sUserData = other.m_sUserData;
    m_iParentIndex = other.m_iParentIndex;
    m_iInternalIndex = other.m_iInternalIndex;
    m_BoundingBox = other.m_BoundingBox;
    m_LocalSpacePosition = other.m_LocalSpacePosition;
    m_InverseObjectSpacePosition = other.m_InverseObjectSpacePosition;
    m_LocalSpaceOrientation = other.m_LocalSpaceOrientation;
    m_InverseObjectSpaceOrientation = other.m_InverseObjectSpaceOrientation;
    return *this;
  }

public:
  friend class VisSkeleton_cl;

  VHashString   m_sBoneName;     ///< used for bone matching
  VString   m_sUserData;     ///< for custom usage
  int       m_iParentIndex;   ///< bone array index to parent

  // stick to alignment in the array - only important for using in managed code, the engine does it anyway
  int m_iInternalIndex; // for arbitrary temporary usage

  hkvAlignedBBox m_BoundingBox;  ///< only exists for the models skeleton not for the one from the animation sequence!!

  hkvVec4 m_LocalSpacePosition;
  hkvVec4 m_InverseObjectSpacePosition;

  hkvQuat m_LocalSpaceOrientation;
  hkvQuat m_InverseObjectSpaceOrientation;
};

#endif //VIS_BONE_HPP_INCLUDED

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
