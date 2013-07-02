/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VCollisionMesh32.hpp

#ifndef VBASE_VCOLLISIONMESH32_HPP
#define VBASE_VCOLLISIONMESH32_HPP

#include <Vision/Runtime/Base/Physics/VCollisionMeshBase.hpp>
#include <Vision/Runtime/Base/Physics/VCollisionSurface.hpp>

class VCollisionData;

// version number for the collision mesh chunk in the collision data file
#define VCOLLISIONMESH32_CHUNK_VERSION 1

/// \brief
///   A simple collision mesh class with 32-bit indices
/// 
/// \note
///   The destructor does NOT delete the VCollisionSurface objects that were set in this mesh.
class VSimpleCollisionMesh32 : public VSimpleCollisionMeshBase
{
public:

  /// \brief
  ///   Collision Mesh Constructor
  VBASE_IMPEXP VSimpleCollisionMesh32(int iVertCount=0, int iIndexCount=0);

  /// \brief
  ///   Collision Mesh Destructor
  VBASE_IMPEXP ~VSimpleCollisionMesh32();

  /// \brief
  ///   Allocates memory for iCount triangle vertex indices and iCount/3 surfaces.
  /// 
  /// \param iCount
  ///   the number of indices to allocate (must be a multiple of 3).
  VBASE_IMPEXP virtual void AllocateIndices(int iCount);
  
  /// \brief
  ///   Frees memory allocated for indices. This will invalidate all index data in the collision mesh. 
  VBASE_IMPEXP virtual void FreeIndices(void);

  /// \brief
  ///   Removes unused vertices
  VBASE_IMPEXP void OptimizeVertices();

  /// \brief
  ///   Resizes the collision mesh.
  /// 
  /// The collision mesh will be resized to the new amount of vertices and triangle vertex indices.
  /// Any vertex and triangle data that was already in the mesh will be copied to
  /// the newly allocated arrays. If the mesh is downsized the data is truncated to the new size.
  /// 
  /// \param iNewVertexCount
  ///   The number of vertices to allocate.
  /// 
  /// \param iNewIndexCount
  ///   The number of triangle vertex indices to allocate.
  VBASE_IMPEXP virtual void Resize(int iNewVertexCount, int iNewIndexCount);

  /// \brief
  ///   Returns the triangle vertex index at the given position.
  inline unsigned int * VISION_FASTCALL GetIndex(int iIndex) const
  {
    VASSERT(iIndex>=0 && iIndex<m_iAllocatedIndices);
    return &m_pIndex32[iIndex];
  }

  /// \brief
  ///   Sets the triangle vertex index at the given position.
  inline void VISION_FASTCALL SetIndex(int iIndex, int idx)
  {
    VASSERT(iIndex>=0 && iIndex<m_iAllocatedIndices);
    m_pIndex32[iIndex] = idx;
  }

  /// \brief
  ///   Returns the pointer to the whole index array.
  inline unsigned int* GetIndexPtr() const {return m_pIndex32;}

  /// \brief
  ///   Returns the stride of the index data (will always be 4 for this class)
  inline int GetIndexStride() const {return sizeof(unsigned int);}

  /// \brief
  ///   Returns the number of surfaces in this mesh.
  inline int GetCollisionSurfaceCount() const
  {
    return GetIndexCount() / 3;
  }

  /// \brief
  ///   Returns a pointer to the collision surface for a given triangle
  inline VCollisionSurface* GetCollisionSurface(int iIndex) const
  {
    return m_ppSurfaces[iIndex];
  }

  /// \brief
  ///   Sets a reference to a collision surface for a specific triangle.
  /// 
  /// \param iIndex
  ///   The index of the triangle to set the collision surface for.
  /// 
  /// \param pCollSurf
  ///   The pointer to the collision surface to set for the specified triangle.
  inline void SetCollisionSurface(int iIndex, VCollisionSurface* pCollSurf)
  {
    m_ppSurfaces[iIndex] = pCollSurf;
  }

  /// \brief
  ///   Returns a pointer to the collision flags for a given triangle.
  inline unsigned short* GetCollisionFlags(int iIndex) const
  {
    return &m_puCollisionFlags[iIndex];
  }

  /// \brief
  ///   Sets the collision flags for a specific triangle.
  inline void SetCollisionFlags(int iIndex, unsigned short uCollFlags)
  {
    m_puCollisionFlags[iIndex] = uCollFlags;
  }

  /// \brief
  ///   returns the amount of memory that this collision mesh consumes.
  inline virtual int GetMemSize() const
  {
    return  m_iAllocatedVertices*sizeof(hkvVec3)
          + m_iAllocatedIndices*sizeof(unsigned int)
          + m_iAllocatedIndices/3*sizeof(VCollisionSurface*)
          + m_iAllocatedIndices/3*sizeof(unsigned short);
  }
  

  // loading/saving callbacks ... these should not be called directly
  void OnStartSaving(VCollisionData* pCollData);
  void OnLoading(VCollisionData* pCollData);

protected:
    
  // surface info for each triangle
  VCollisionSurface** m_ppSurfaces;
  
  // collision flags for each triangle
  unsigned short* m_puCollisionFlags;
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
