/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   Skeleton class
///
/// Stores root bones and bind pose information for a skeleton, etc.
class VGEOM2_IMPEXP_CLASS VGSkeleton
{
public:
  VGSkeleton();
  VGSkeleton(const VGSkeleton& other);
  virtual ~VGSkeleton();



  /// \brief
  ///   Disable sorting of the bones in a skeleton. Use with caution as will break backcompat.
  ///
  inline void             DisableBoneSorting()            { m_sortedBoneList = false; }

  /// \brief
  ///   Add a new root bone (or hierarchy)
  ///
  /// \param b
  ///   Root bone to add (might contain child bones itself) - not that it will be copied
  inline void             AddRootBone(const VGBone& b)            { m_rootBones->Add(b); }

  /// \brief
  ///   Creates a new root bone and returns a reference to it (same as implicitly adding a new bone)
  ///
  /// \return
  ///   Reference to newly added root bone
  inline VGBone&          CreateRootBone()                        { AddRootBone(VGBone()); return GetRootBone(GetNumRootBones()-1); }

  /// \brief
  ///   Return number of root bones
  ///
  /// \return
  ///   Number of root bones
  inline int              GetNumRootBones() const                 { return m_rootBones->GetSize(); }

  /// \brief
  ///   Return a reference to a root bone
  ///
  /// \param i
  ///    The index of the root bone
  ///
  /// \return
  ///   Root bone
  inline       VGBone&    GetRootBone(int i)                      { return (*m_rootBones)[i]; }
  inline const VGBone&    GetRootBone(int i) const                { return (*m_rootBones)[i]; }

  /// \brief
  ///   Removes a root bone (and the root's child hierarchy)
  ///
  /// \param i
  ///   Root bone index
  inline void             RemoveRootBone(int i)                   { m_rootBones->RemoveAt(i); }



  /// \brief
  ///   Sets index of root bone that should be used as trajectory bone (default is -1, meaning "not explicitly set / no motion delta")
  ///
  /// \param i
  ///   Index of bone used as trajectory bone
  void                    SetTrajectoryBoneIndex(int i)           { m_trajectoryBoneIndex = i; }

  /// \brief
  ///   Gets index of root bone that should be used as trajectory bone (-1 means "not explicitly set")
  ///
  /// \return
  ///   Index of bone used as trajectory bone, or -1 if not explicitly set
  int                     GetTrajectoryBoneIndex() const          { return m_trajectoryBoneIndex; }



  /// \brief
  ///   Generates a flat bone list out of the skeleton's bone hierarchy
  ///
  /// Generates a flat bone list out of the skeleton's bone hierarchy, using some pseudo-depth-first
  /// traversal, meaning that the resulting list will contain bones in an order such that child bones
  /// get stored with an index > parent's index, without exception. To precise the order - it is like
  /// depth-first, however all children get added to the flat list *before* recursion. This is due to
  /// historical reasons, and merely a Vision convention for model/anim file export - which makes use
  /// of this function. We might refer to it as "siblings-first", from here on.
  /// Furthermore, bones having the same depth will be sorted alphabetically.
  ///
  /// \param boneList
  ///   Bone list to be filled with pointers into the bone hierarchy
  ///
  /// \param boneToIndexMap
  ///   Optional map to be filled with VGBone pointers mapping to indices of the flat
  ////  bone list (will be cleared by this function)
  ///
  /// \param parentIndices
  ///   Optional array to be filled with indices (into the flat list) to a bone's parent
  ///   bone (will be cleared by this function)
  void                    GetFlatBoneList(VArray<const VGBone*, const VGBone*>& boneList, VMapCPtrToInt* boneToIndexMap = NULL, VArray<int, int>* parentIndices = NULL) const;
  void                    GetFlatBoneList(VArray<      VGBone*,       VGBone*>& boneList, VMapCPtrToInt* boneToIndexMap = NULL, VArray<int, int>* parentIndices = NULL);



  /// \brief
  ///   Generates a set of all animation names used to animate the skeleton
  ///
  /// \param names
  ///   Name set to be filled with names of the animations applied to the skeleton's bones
  void                    GetAnimationNames(VArray<VString, const VString&>& names) const;


  /// \brief
  ///   Returns whether the skeleton has animations
  bool                    HasAnimations() const;
  
  /// \brief
  ///   Returns a list of animated bones (sharing an animation name)
  ///
  /// \param name
  ///   Name to use for query
  ///
  /// \param bones
  ///   Animated bones with index into their animation arrays
  void                    GetAnimatedBones(const VString& name, VMapCPtrToInt& bones) const;//@@@ test
  void                    GetAnimatedBones(const VString& name, VMapPtrToInt & bones);      //@@@ test



  /// \brief
  ///   Assignment operator
  ///
  /// \param rhs
  ///   Skeleton to assign
  ///
  /// \return
  ///   Reference to this skeleton
  VGSkeleton& operator=(const VGSkeleton& rhs);

  
private:
  LinkedList_cl<VGBone>* m_rootBones;       // List of root bones.
  int m_trajectoryBoneIndex;
  bool m_sortedBoneList;
};

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
