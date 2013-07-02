/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiModel.hpp

#ifndef VIS_MODEL_HPP_INCLUDED
#define VIS_MODEL_HPP_INCLUDED

#include <Vision/Runtime/Base/Math/BoundingVolume/hkvAlignedBBox.h>


/// \brief
///   Enum that defines which criteria are used to merge two instances
enum VisMergeMode_e
{
  Merge_AllUnique,      ///< All instances become unique, no merging takes place
  Merge_SameReference,  ///< Only instances of the exact same reference are merged (i.e. same material pointer)
  Merge_SameName        ///< instances of the same reference or same name are merged (i.e. material name)
};


/// \brief
///   Structure that holds information for single model to merge into a new one (see VMeshManager::MergeDynamicMeshes)
struct MeshMergeInfo_t
{
  inline MeshMergeInfo_t()
  {
    m_pMesh = NULL;
    m_eMaterialMergeMode = Merge_SameReference;
    m_eBoneMergeMode = Merge_SameName;
    iFinalBoneIndex = -1;
  }

  VDynamicMesh *m_pMesh;                ///< pointer to the dynamic mesh to merge. Must be !=NULL
  hkvMat3 m_RelativeRotation;           ///< relative rotation for the baking of m_pModel compared to the new pivot
  hkvVec3 m_vRelativeOffset;            ///< relative offset for the baking of m_pModel compared to the new pivot
  VisMergeMode_e m_eMaterialMergeMode;  ///< Defines how materials are merged (e.g. all unique, or by name)
  VisMergeMode_e m_eBoneMergeMode;      ///< Defines how skeletons are merged (e.g. all unique, or by name)
  VString m_sAttachToBone;              ///< Name of the bone in the final skeleton that this (static) model should be attached to

private:
  // internal data:
  friend class VMeshManager;
  int iVertexOfs;
  int iTriOfs;
  int iSurfaceOfs;
  int iFinalBoneIndex;
  int iReserved[4];
};

#endif // VIS_MODEL_HPP_INCLUDED

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
