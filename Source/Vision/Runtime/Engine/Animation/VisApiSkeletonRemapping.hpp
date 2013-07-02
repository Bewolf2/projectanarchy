/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiSkeletonRemapping.hpp

#ifndef VIS_BONE_REMAPPING_HPP_INCLUDED
#define VIS_BONE_REMAPPING_HPP_INCLUDED

// predefines
class VisSkeleton_cl;
class VisSkeletonRemapping_cl; 
class hkvQuat; 
class VisSkeletalAnimResult_cl; 
class VisAnimState_cl;
typedef VSmartPtr<VisSkeletonRemapping_cl>  VisSkeletonRemappingPtr;

/// \brief
///   An instance of VisSkeletonRemapping_cl keeps the data to remap from one skeleton to another
/// 
/// \note
///   Remapping only considers the direction from A -> B, not the other way around.
/// 
/// \note
///   For the inverse, another remapping instance is needed.
class VisSkeletonRemapping_cl:public VRefCounter
{
public:

  /// \brief
  ///   Constructor of the remapping.
  /// 
  /// \param pDestSkeleton
  ///   skeleton that is mapped to.
  /// 
  /// \param pSourceSkeleton
  ///   skeleton that is mapped from.
  VISION_APIFUNC VisSkeletonRemapping_cl(const VisSkeleton_cl* pDestSkeleton, const VisSkeleton_cl* pSourceSkeleton);


  /// \brief
  ///   Destructor
  VISION_APIFUNC ~VisSkeletonRemapping_cl();


  /// \brief
  ///   Indicates whether the indices differ between the mapped skeletons.
  /// 
  /// \return
  ///   bool bDiffer: true if indices differ.
  inline bool IndicesDiffer()
  {
    return m_bIndicesDiffer;
  }

    
  /// \brief
  ///   Returns the list with indices that define the lookup between the skeletons.
  /// 
  /// \return
  ///   const int*: pointer to indices list.
  inline const int* GetIndicesList() const
  {
    return m_pIndexList;
  }

  /// \brief
  ///   Indicates whether the bone lengths differ between the mapped skeletons.
  /// 
  /// \return
  ///   bool bResult: true if lengths differ.
  inline bool BoneLengthsDiffer() const
  {
    return m_bBoneLengthsDiffer;
  }

   
  /// \brief
  ///   Returns the list with per-bone length scaling.
  /// 
  /// It defines the scaling factor from one bones length to the other.
  /// 
  /// \return
  ///   float* pList: pointer to length scaling list.
  inline const float* GetBoneLengthScalingList() const
  {
    return m_pBoneLengthScalingList;
  }


  /// \brief
  ///   Indicates whether the rotations differ between the mapped skeletons.
  /// 
  /// \return
  ///   bool bResult: true if rotations differ.
  inline bool RotationsDiffer() const
  {
    return m_bRotationsDiffer;
  }

  
  /// \brief
  ///   Returns the list with relative rotations.
  /// 
  /// \return
  ///   hkvQuat*: pointer to rotations list.
  inline const hkvQuat* GetRotationsList() const
  {
    return m_pRotationList;
  }


  /// \brief
  ///   Indicates whether any scaling has to be applied between the mappings.
  /// 
  /// \return
  ///   bool bResult: true if it has general scale.
  inline bool HasGeneralResize() const
  {
    return m_bGeneralResize;
  }

  
  /// \brief
  ///   Returns the general scaling factor or x,y and z direction.
  /// 
  /// \return
  ///   hkvVec3& vScale: scaling factor
  inline const hkvVec3& GetGeneralResize() const
  {
    return m_vGeneralResize;
  }


  /// \brief
  ///   Applies the remapping to convert a result.
  /// 
  /// \param pDestResult
  ///   destination result. Gets filled with converted result
  /// 
  /// \param pSourceResult
  ///   source result
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC void DoResultRemapping(VisSkeletalAnimResult_cl* pDestResult, const VisSkeletalAnimResult_cl* pSourceResult);


  /// \brief
  ///   Applies the remapping to convert a state.
  /// 
  /// \param pDestState
  ///   destination state. Gets filled with converted state.
  /// 
  /// \param pSourceState
  ///   source state
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC void DoStateRemapping(VisAnimState_cl* pDestState, const VisAnimState_cl* pSourceState);

  
  /// \brief
  ///   Returns the number of indices used for remapping.
  /// 
  /// \return
  ///   int: count
  inline int GetIndexCount() const
  {
    return m_iIndexCount;
  }

  
  /// \brief
  ///   Returns the number of lengths used for remapping.
  /// 
  /// \return
  ///   int: count
  inline int GetLengthCount() const
  {
    return m_iIndexCount;
  }


  /// \brief
  ///   Returns the number of rotations used for remapping.
  /// 
  /// \return
  ///   int: count
  inline int GetRotationCount() const
  {
    return m_iIndexCount;
  }

protected:
  bool m_bIndicesDiffer;                      ///< all types of mapping are optional!
  bool m_bBoneLengthsDiffer;
  bool m_bRotationsDiffer;
  bool m_bGeneralResize;

  int* m_pIndexList;                          ///< matching table from this skeleton to input skeleton
  float* m_pBoneLengthScalingList;            ///< one scaling factor per bone
  hkvQuat* m_pRotationList;          ///< one rotation per bone
  const VisSkeleton_cl* m_pSourceSkeleton;    ///< source skeleton

  int m_iIndexCount;
  int m_iLengthCount;
  int m_iRotationCount;

  hkvVec3 m_vGeneralResize;    ///< used for resizing stuff like bounding boxes and motion delta
};

#endif //VIS_BONE_REMAPING_HPP_INCLUDED

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
