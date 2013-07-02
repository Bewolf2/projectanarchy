/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VCollisionMesh.hpp

#ifndef VBASE_VCOLLISIONMESH_HPP
#define VBASE_VCOLLISIONMESH_HPP

#include <Vision/Runtime/Base/Physics/VCollisionMeshBase.hpp>

/// \brief
///   A simple collision mesh class with 16-bit indices. 
///
/// This class is used by the Vision Engine for the built-in collision toolkit
/// (VisCollisionToolkit_cl). Working with this clas is typically only required when implementing
/// custom geometry types.
class VSimpleCollisionMesh : public VSimpleCollisionMeshBase
{
public:

  /// \brief
  ///   Collision Mesh Constructor
  VBASE_IMPEXP VSimpleCollisionMesh(int iVertCount=0, int iIndexCount=0);

  /// \brief
  ///   Collision Mesh Destructor
  VBASE_IMPEXP ~VSimpleCollisionMesh();

  /// \brief
  ///   Allocates memory for iCount triangle vertex indices
  /// 
  /// \param iCount
  ///   The number of indices to allocate (must be a multiple of 3).
  /// 
  /// \note
  ///   iCount must be a multiple of 3.
  VBASE_IMPEXP virtual void AllocateIndices(int iCount);
  
  /// \brief
  ///   Frees memory allocated for indices. This will invalidate all index data in the collision mesh. 
  VBASE_IMPEXP virtual void FreeIndices(void);

  /// \brief
  ///   Resizes the collision mesh.
  /// 
  /// The collision mesh will be resized to the new amount of vertices and triangle vertex indices.
  /// Any vertex and triangle data that was already in the mesh will be copied to
  /// the newly allocated arrays. If the mesh is downsized the data is truncated to the new size.
  /// 
  /// \param iNewVertexCount
  ///   the number of vertices to allocate.
  /// 
  /// \param iNewIndexCount
  ///   the number of triangle vertex indices to allocate.
  VBASE_IMPEXP virtual void Resize(int iNewVertexCount, int iNewIndexCount);

  /// \brief
  ///   Returns the triangle vertex index at the given position.
  inline unsigned short * VISION_FASTCALL GetIndex(int iIndex) const
  {
    VASSERT(iIndex>=0 && iIndex<m_iAllocatedIndices);
    return &m_pIndex16[iIndex];
  }

  /// \brief
  ///   Returns the pointer to the whole index array.
  inline unsigned short* GetIndexPtr() const {return m_pIndex16;}

  /// \brief
  ///   Returns the stride of the index data (will always be 2 for this class)
  inline int GetIndexStride() const {return sizeof(unsigned short);}

  /// \brief
  ///   Returns the amount of memory that this mesh consumes.
  inline virtual int GetMemSize() const
  {
    return  m_iAllocatedVertices*sizeof(hkvVec3)
          + m_iAllocatedIndices*sizeof(unsigned short);    
  }

  /// \brief
  ///   Returns a pointer to the planes of the triangles
  VBASE_IMPEXP hkvPlane* GetTrianglePlanes();
  
  /// \brief
  ///   Forces the planes to be recomputed if the mesh is modified. Allocates and computes the planes if they are not cached or if they are out of date.
  VBASE_IMPEXP void ComputeTrianglePlanes();

  /// \brief
  ///   Tests whether a sphere intersects the collision mesh.
  ///
  /// If pStoreInfo is provided, the closest intersection point will be returned, otherwise 
  /// the function will return once the first intersection point is found.
  ///
  /// \param pos
  ///   Position of the sphere center.
  /// 
  /// \param fRadius
  ///   Radius of the sphere.
  /// 
  /// \param pStoreInfo
  ///   If not NULL, the closest intersection point will be stored in this structure. Note that this requires finding
  ///   all intersection points, which can require a significant amount of additional computation time.
  /// 
  /// \return
  ///   bool: True if the collision mesh penetrates the sphere.
  VBASE_IMPEXP bool CollidesWithSphere(const hkvVec3& pos, float fRadius, VTraceHitInfo* pStoreInfo=NULL);

protected:
  
  hkvPlane* m_pPlane;       ///<Cached array of triangle planes
};


#endif

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
