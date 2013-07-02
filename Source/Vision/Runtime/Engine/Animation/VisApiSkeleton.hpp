/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiSkeleton.hpp

#ifndef VIS_SKELETON_HPP_INCLUDED
#define VIS_SKELETON_HPP_INCLUDED

#include <Vision/Runtime/Engine/Animation/VisApiSkeletalBone.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiSkeletonRemapping.hpp>

class VisSkeletalAnimResult_cl; 
class hkvQuat; 
class VChunkFile;
class VisAnimSequenceSet_cl;
class VDynamicMesh;

// define smartpointer
class VisSkeleton_cl;
typedef VSmartPtr<VisSkeleton_cl> VisSkeletonPtr;


/// \brief
///   Derived class for loading a VisSkeleton_cl resource 
class VSkeletonSerializationProxy : public IVSerializationProxy
{
public:
  inline VSkeletonSerializationProxy(VisSkeleton_cl *pSkel) {VASSERT(pSkel);m_pSkel=pSkel;}
  virtual IVSerializationProxyCreator *GetInstance();

protected:
  // serialization
  inline VSkeletonSerializationProxy() {m_pSkel=NULL;}
  V_DECLARE_SERIAL_DLLEXP( VSkeletonSerializationProxy, VISION_APIDATA );
  virtual void Serialize( VArchive &ar );
private:
  VisSkeleton_cl *m_pSkel;
};


/// \brief
///   Skeleton class that provides data structures for a bones hierarchy
class VisSkeleton_cl : public VRefCounter, public IVSerializationProxyCreator
{

public:

  enum SkeletonSourceType_e
  {
    SKELETON_CUSTOM = 0,
    SKELETON_FROM_MODEL = 1,
    SKELETON_FROM_SEQUENCESET = 2
  };

  /// \brief
  ///   Constructor
  /// 
  /// \param iBoneCount
  ///   number of bones used for allocations.
  VISION_APIFUNC VisSkeleton_cl(int iBoneCount);


  /// \brief
  ///   Destructor
  VISION_APIFUNC ~VisSkeleton_cl();



  /// \brief
  ///   Returns the number of bones in this skeleton.
  /// 
  /// \return
  ///   int iCount: bone count
  inline int GetBoneCount() const
  {
    return m_iBoneCount;
  }

    
  /// \brief
  ///   Returns the bone of specified index.
  /// 
  /// \param iBoneIndex
  ///   index of bone. Must be in valid range [0..GetBoneCount()-1]
  /// 
  /// \return
  ///   VisSkeletalBone_cl* pBone: bone pointer
  inline VisSkeletalBone_cl* GetBone(int iBoneIndex) const
  {
    VASSERT(iBoneIndex>=0 && iBoneIndex<m_iBoneCount);
    return &m_pBoneList[iBoneIndex];
  }

  /// \brief
  ///   Finds a bone by name and returns its index (or -1)
  /// 
  /// \param szBoneName
  ///   bone name (case insensitive)
  /// 
  /// \return
  ///   int: bone index
  VISION_APIFUNC int GetBoneIndexByName(const char* szBoneName) const;

  /// \brief
  ///   Finds a bone by name and returns its index (or -1)
  /// 
  /// \param BoneName
  ///   bone name (case insensitive)
  /// 
  /// \return
  ///   int: bone index
  VISION_APIFUNC int GetBoneIndexByName(const VHashString* BoneName) const;


  /// \brief
  ///   Creates a matching table for to different skeletons.
  /// 
  /// It tries to find for every bone in this skeleton a corresponding bone in the other skeleton
  /// by bone name string comparison. If there is a match the index is saved in the bone matching
  /// table.
  /// 
  /// If there was no matching bone found the index is set to -1.
  /// 
  /// \param pSourceSkeleton
  ///   skeleton to match
  /// 
  /// \param pBoneMatchingList
  ///   list to fill with matching indices. Needs to provide at least GetBoneCount() entries
  /// 
  /// \param iIndexCount
  ///   Out: Number of matching bones.
  ///
  /// \return
  ///   Nothing.
  VISION_APIFUNC void CreateBoneIndexList(const VisSkeleton_cl* pSourceSkeleton, int* pBoneMatchingList, int &iIndexCount) const;


  /// \brief
  ///   Creates a scaling mapping table for two different skeletons.
  /// 
  /// CreateBoneLengthScalingList compares the lengths of bones that exist in both skeletons. If
  /// the lengths are not identical the scaling factor is saved in the scaling list.
  /// 
  /// \param pSourceSkeleton
  ///   Other skeleton to match.
  /// 
  /// \param pBoneLengthScalingList
  ///   List to fill with bone length scalings. Needs to provide at least GetBoneCount() entries.
  /// 
  /// \param pBoneMatchingList
  ///   Used for mapping bone indices.
  /// 
  /// \param iLengthCount
  ///   Out: Length of the list
  ///
  /// \return
  ///   bool bResult: True if successful.
  bool CreateBoneLengthScalingList(const VisSkeleton_cl* pSourceSkeleton, float* pBoneLengthScalingList, const int* pBoneMatchingList, int &iLengthCount) const;


  /// \brief
  ///   Creates a rotation list for to different skeleton.
  /// 
  /// It compares the rotation of bones that exist in both skeletons. If they are not identical the
  /// difference is saved in the rotation list.
  /// 
  /// \param pSourceSkeleton
  ///   other skeleton to match
  /// 
  /// \param pBoneRotationList
  ///   list to fill with rotation differences
  /// 
  /// \param pBoneMatchingList
  ///   used for mapping bone indices
  /// 
  /// \param iRotationCount
  ///   Out: Rotation count.
  ///
  /// \return
  ///   bool bResult: True if successful.
  VISION_APIFUNC bool CreateBoneRotationList(const VisSkeleton_cl* pSourceSkeleton, hkvQuat* pBoneRotationList, const int* pBoneMatchingList, int &iRotationCount) const;


  /// \brief
  ///   Checks whether two skeletons are identical.
  /// 
  /// Bone Names, positions, orientations and parents have to be the same.
  /// 
  /// \param pSourceSkeleton
  ///   other skeleton to test
  /// 
  /// \return
  ///   bool bResult: true if skeletons are identical
  VISION_APIFUNC bool AreSkeletonsIdentical(const VisSkeleton_cl* pSourceSkeleton) const;


  /// \brief
  ///   Checks whether two skeletons need remapping.
  /// 
  /// Checks the needs for index, translation and rotation remapping.
  /// 
  /// \param pSourceSkeleton
  ///   other skeleton to test
  /// 
  /// \return
  ///   bool bResult: true if any of the channels need remapping.
  VISION_APIFUNC bool NeedsRemapping(const VisSkeleton_cl* pSourceSkeleton) const;

  /// \brief
  ///   Specifies that translation and rotation shall be ignored for remapping requirement detection. Use with caution!
  ///
  /// This method instructs the animation system to ignore any translation and rotation difference
  /// in the skeleton when it comes to the remapping requirement. Technically this means that even
  /// skeletons which differ in bone transformations do not need remapping.
  ///
  /// Use this method if your skeletons are expected to be the same, but slightly differ in their
  /// transformations due to small inaccuracies in 3DS Max or Maya - and if you want to use them
  /// for animation sharing with identical skeletons being required (e.g. sharing the final skeleton
  /// result).
  ///
  /// Attention: This function must be used with caution. It can lead to unexpected results when the
  /// skeletons differ too much.
  /// 
  /// \param bStatus
  ///   TRUE for ignoring the transformation, FALSE for taking it into account (default)
  inline void SetIgnoreTransformForRemapping(bool bStatus)
  {
    m_bIgnoreTransformForRemapping = bStatus;
  }

  /// \brief
  ///   Checks whether two skeletons need index remapping.
  /// 
  /// Remapping is not required if all bone indices/names are identical.
  /// 
  /// \param pSourceSkeleton
  ///   other skeleton to test
  /// 
  /// \return
  ///   bool bResult: true if remapping is required
  VISION_APIFUNC bool NeedsBoneIndexList(const VisSkeleton_cl* pSourceSkeleton) const;


  /// \brief
  ///   Checks whether two skeletons need translation remapping.
  /// 
  /// Remapping is not required when all bones with the same index have the same translation.
  /// 
  /// \param pSourceSkeleton
  ///   other skeleton to test
  /// 
  /// \param pBoneList
  ///   used for mapping bone indices
  /// 
  /// \return
  ///   bool bResult: true if remapping is required
  VISION_APIFUNC bool NeedsBoneTranslationList(const VisSkeleton_cl* pSourceSkeleton, const int* pBoneList) const;


  /// \brief
  ///   Checks whether two skeletons need rotation remapping.
  /// 
  /// Remapping is not required when all bones with the same index have the same rotation.
  /// 
  /// \param pSourceSkeleton
  ///   skeleton to test
  /// 
  /// \param pBoneList
  ///   used for mapping bone indices
  /// 
  /// \return
  ///   bool bResult: true if remapping is required
  VISION_APIFUNC bool NeedsBoneRotationList(const VisSkeleton_cl* pSourceSkeleton, const int* pBoneList) const;


  /// \brief
  ///   Do not use. (internal test function)
  /// 
  /// CreateSkeletonFrom is a helperfunction that can be used to fill a skeleton with testdata.
  /// 
  /// It takes list with bone names, positions and orientations to fill the skeleton with.
  /// 
  /// \param iStartIndex
  ///   bone index to start with
  /// 
  /// \param iBoneCount
  ///   number of bone to fill
  /// 
  /// \param Names
  ///   bone names
  /// 
  /// \param Positions
  ///   bone positions
  /// 
  /// \param Orientations
  ///   bone orientations
  /// 
  /// \return
  ///   bool bMappable: True if skeletons are mappable
  VISION_APIFUNC bool CreateSkeletonFrom(int iStartIndex, int iBoneCount, const VString* Names, const hkvVec3* Positions, const hkvQuat* Orientations);


  /// \brief
  ///   Static function that loads a skeleton from specified skeleton file chunk and returns a new
  ///   skeleton instance.
  /// 
  /// \param AnimFile
  ///   chunk to load skeleton from
  /// 
  /// \return
  ///   VisSkeleton_cl* pSourceSkeleton: loaded skeleton
  VISION_APIFUNC static VisSkeleton_cl* CreateSkeletonFromChunk(VChunkFile &AnimFile);


  /// \brief
  ///   Writes this skeleton to skeleton file chunk.
  /// 
  /// \param AnimFile
  ///   chunk to write skeleton to
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC void WriteSkeletonChunk(VChunkFile &AnimFile);


  /// \brief
  ///   Creates a mapping for another skeleton.
  /// 
  /// The mapping is cached, but it is only available for one direction.
  /// 
  /// \param pSourceSkeleton
  ///   other skeleton to test.
  /// 
  /// \return
  ///   VisSkeletonRemapping_cl *pMapping: Instance that represents the mapping between the two
  ///   skeletons
  VISION_APIFUNC VisSkeletonRemapping_cl* GetSkeletonRemapping(const VisSkeleton_cl* pSourceSkeleton) const;


  /// \brief
  ///   Deletes all cached mappings created for this skeleton.
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC bool RemoveAllMappings(void);


  /// \brief
  ///   Removes the mapping between this and the specified skeleton.
  /// 
  /// \param pSkeleton
  ///   other skeleton.
  /// 
  /// \return
  ///   bool bResult: true if removing mapping was successful.
  VISION_APIFUNC bool RemoveMapping(VisSkeleton_cl* pSkeleton);


  /// \brief
  ///   Sets the specified weight for a bone and all its children recursively.
  /// 
  /// \param fWeight
  ///   weight to set
  /// 
  /// \param iBoneIndex
  ///   index of the parent bone to start with.
  /// 
  /// \param fPerBoneWeightingList
  ///   pointer to per bone weighting list that gets filled by this function
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC void SetBoneWeightRecursive(float fWeight, int iBoneIndex, float* fPerBoneWeightingList);


  /// \brief
  ///   Tests whether one of the ancestors has the specified index.
  /// 
  /// \param iBoneIndex
  ///   index of start bone.
  /// 
  /// \param iParentIndex
  ///   parent index to search for.
  /// 
  /// \return
  ///   bool bIsParent: true if the bone at iParentIndex is a parent of the bone at iBoneIndex
  VISION_APIFUNC bool HasParentRecursive(int iBoneIndex, int iParentIndex) const;

public:
  // Ownership: can either be from animation OR from model OR a custom skeleton (then both pointers are zero)
  // This information is important for serialization
  VisAnimSequenceSet_cl *m_pOwnerAnimation; 
  VDynamicMesh *m_pOwnerMesh;
  int m_iOwnerIndex; ///< index inside the owner list (e.g. sequence set has a list of skeletons)
  inline SkeletonSourceType_e GetSourceType() const
  {
    if (m_pOwnerMesh) return SKELETON_FROM_MODEL;
    if (m_pOwnerAnimation) return SKELETON_FROM_SEQUENCESET;
    return SKELETON_CUSTOM;
  }

protected:
  // overridden IVSerializationProxyCreator function
  VISION_APIFUNC virtual IVSerializationProxy *CreateProxy() {return new VSkeletonSerializationProxy(this);}

private:

  short m_iBoneMaskCount;
  int m_iBoneCount;
  VisSkeletalBone_cl* m_pBoneList;
  bool m_bIgnoreTransformForRemapping;   ///< if specified, translation and rotation are not used as criteria for remapping. USE WITH CAUTION!

  // keeps a list for converting animation data from other skeletons to this skeleton
  mutable VMapPtrToPtr  m_RemappingMap;
  mutable int m_iMappingCount;
};


#endif //VIS_SKELETON_HPP_INCLUDED

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
