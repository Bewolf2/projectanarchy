/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   Bone class
///
/// Describes and handles bone data.
class VGEOM2_IMPEXP_CLASS VGBone : public VGAnimatable<AnimKeyPolicy_Xform>, public VGNode
{
public:
  /// \brief
  ///   Bone mask
  ///
  /// Bits to be set on bones, some processors can use them to work on a bone subset
  enum BoneMask//@@@ move to VGNode as a nodemask?
  {
    VGBM_NOTHING = 0,
    VGBM_DEFAULT = V_BIT(0),
    VGBM_SKIP    = V_BIT(1),  // @@@ more generic name?
    VGBM_RAW     = V_BIT(2)   // @@@ to be extended
  };

  static const BoneMask FULL_BONE_MASK = BoneMask(VGBM_DEFAULT|VGBM_SKIP|VGBM_RAW);



public:
  VGBone();
  VGBone(const VGBone& other);
  virtual ~VGBone();


  /// \brief
  ///   Gets the currently set bone mask
  ///
  /// \return
  ///   Currently set bone mask
  inline BoneMask         GetBoneMask() const throw()                             { return m_bm; }

  /// \brief
  ///   Sets the bone mask to be used with this bone
  ///
  /// \param bm
  ///   Bone mask to set
  inline void             SetBoneMask(BoneMask bm) throw()                        { m_bm = BoneMask(bm&VGBone::FULL_BONE_MASK); }



  /// \brief
  ///   Computes the bone's skeleton space transformation matrix by concatenating up the bone hierarchy
  ///
  /// \param m
  ///   Reference to matrix that will hold the computed skeleton space transformation matrix (if model's bind pose is identity,
  ///   this is equal to the global space bone transformation)
  inline void             ComputeSkeletonSpaceTransformation(hkvMat4& m) const    { m=VGTransformable::GetTransformation(); const VGBone* p=this; while(p=p->GetParent()) { m = p->VGTransformable::GetTransformation().multiply(m); }; }



  /// \brief
  ///   Adds a new child bone (or hierarchy)
  ///
  /// \param b
  ///   Child bone to add (might contain child bones itself) - note that it will be copied
  inline void             AddChildBone(const VGBone& b)                           { m_childBones->Add(b); (*m_childBones)[m_childBones->GetSize()-1].m_parent = this; }

  /// \brief
  ///   Creates a new child bone and returns a reference to it (same as implicitly adding a new bone)
  ///
  /// \return
  ///   Reference to newly created child bone
  inline VGBone&          CreateChildBone()                                       { AddChildBone(VGBone()); return GetChildBone(GetNumChildBones()-1); }

  /// \brief
  ///   Return number of child bones
  ///
  /// \return
  ///   Number of child bones
  inline int              GetNumChildBones() const                                { return m_childBones->GetSize(); }

  /// \brief
  ///   Return a reference to a child bone
  ///
  /// \param i
  ///   Child bone index
  ///
  /// \return
  ///   Child bone
  inline       VGBone&    GetChildBone(int i)                                     { return (*m_childBones)[i]; }
  inline const VGBone&    GetChildBone(int i) const                               { return (*m_childBones)[i]; }

  /// \brief
  ///   Gets the bone's parent bone
  ///
  /// \return
  ///   Pointer to bone's parent, or NULL
  inline VGBone*          GetParent() const throw()                               { return m_parent; }

  /// \brief
  ///   Removes a child bone (and the child's child hierarchy)
  ///
  /// \param i
  ///   Child bone index
  inline void             RemoveChildBone(int i)                                  { m_childBones->RemoveAt(i); }

  /// \brief
  ///   Sorts the childbone list alphabetically
  void                    SortChildBones();



  /// \brief
  ///   Generates a flat bone list consisting of this bone and its child bone hierarchy - for
  ///   details about the layout of the flat list, refer to the VGSkeleton::GetFlatBoneList
  ///   documentation
  ///
  /// \param boneList
  ///   Bone list to be filled with pointers into the bone hierarchy (will be cleared by
  ///   this function)
  ///
  /// \param parentIndices
  ///   Optional array to be filled with indices (into the flat list) to a bone's parent
  ///   bone (will be cleared by this function)
  void                    GetFlatBoneList(VArray<const VGBone*, const VGBone*>& boneList, VArray<int, int>* parentIndices) const;
  void                    GetFlatBoneList(VArray<      VGBone*,       VGBone*>& boneList, VArray<int, int>* parentIndices);



  /// \brief
  ///   Assignment operator
  ///
  /// This is mainly used in order to update the parent bone pointer during a copy.
  /// When copying a bone, the entire hierarchy of child bones gets copied, too, meaning
  /// that the latter have to point to the new parent bone copies instead of pointing to
  /// the copy's source bones.
  ///
  /// \param rhs
  ///   Bone to assign
  ///
  /// \return
  ///   Reference to this bone
  VGBone& operator=(const VGBone& rhs);



private:
  BoneMask                m_bm;
  LinkedList_cl<VGBone>*  m_childBones;       // List of child bones.
  VGBone*                 m_parent;           // Parent bone.
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
