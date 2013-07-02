/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VCollisionMeshBase.hpp

#ifndef VBASE_VCOLLISIONMESHBASE_HPP
#define VBASE_VCOLLISIONMESHBASE_HPP

#include <Vision/Runtime/Base/Math/Primitive/VTriangle.hpp>

class VSimpleCollisionMeshBase;

// traceline flags
#define TRACEFLAG_NONE              0x00000000
#define TRACEFLAG_DOUBLESIDED       0x00000001

// internal values
#define TRACEFLAG_NOBOUNDINGBOX     0x00010000

/// \brief
///   Structure that stores the traceline hit info
struct VTraceHitInfo
{
  VTraceHitInfo() { memset(this, 0, sizeof(VTraceHitInfo)); }
  
  VSimpleCollisionMeshBase *m_pCollMesh;
  int m_iMeshPrimitiveID;
  VTriangle m_MeshTriangle;

  // touchpoint information
  hkvVec3 m_vTouchPoint;
  float m_fDistance;
  
  bool m_bDetected;
};



/// \brief
///   Abstract base class for Vision collision meshes.
///
/// This class and its derived classes are used by the Vision Engine for the built-in collision toolkit
/// (VisCollisionToolkit_cl). Working with these classes is typically only required when implementing
/// custom geometry types.
class VSimpleCollisionMeshBase : public VBaseObject
{
public:

  /// \brief
  ///   Constructor. If iVertCount and iIndexCount are given, will allocate this number of vertices and indices for the collision mesh.
  VBASE_IMPEXP VSimpleCollisionMeshBase(int iVertCount=0, int iIndexCount=0);

  /// \brief
  ///   Destructor.
  VBASE_IMPEXP virtual ~VSimpleCollisionMeshBase();

  /// \brief
  ///   Allocates memory for iCount vertices.
  /// 
  /// \param iCount
  ///   The number of vertices to allocate.
  VBASE_IMPEXP void AllocateVertices(int iCount);
  
  /// \brief
  ///   Frees memory allocated for vertices. This will invalidate all vertex data in the collision mesh.
  VBASE_IMPEXP void FreeVertices();
  
  /// \brief
  ///   Allocates memory for iCount vertex indices. This function is abstract and must be
  ///   implemented in subclasses.
  /// 
  /// \param iCount
  ///   The number of indices to allocate (must be a multiple of 3).
  VBASE_IMPEXP virtual void AllocateIndices(int iCount) = 0;
  
  /// \brief
  ///   Frees memory allocated for indices. This will invalidate all index data in the collision mesh. 
  ///   Must be implemented in subclasses.
  VBASE_IMPEXP virtual void FreeIndices(void) = 0;

  /// \brief
  ///   Resizes the collision mesh.
  /// 
  /// The collision mesh will be resized to the new amount of vertices and triangle vertex indices.
  /// Any vertex and triangle data that was already in the mesh will be copied to
  /// the newly allocated arrays. If the mesh is downsized the data is truncated to the new size.
  /// This function is abstract and must be implemented in subclasses.
  /// 
  /// \param iNewVertexCount
  ///   the number of vertices to allocate.
  /// 
  /// \param iNewIndexCount
  ///   the number of triangle vertex indices to allocate.
  VBASE_IMPEXP virtual void Resize(int iNewVertexCount, int iNewIndexCount) = 0;

  /// \brief
  ///   Returns the number of vertices allocated in this mesh.
  inline int GetVertexCount() const {return m_iAllocatedVertices;}

  /// \brief
  ///   Returns the number of triangle vertex indices allocated in this mesh.
  inline int GetIndexCount() const {return m_iAllocatedIndices;}

  /// \brief
  ///   Returns the vertex at the given index
  inline hkvVec3* VISION_FASTCALL GetVertex(int iIndex) const
  {
    VASSERT(iIndex>=0 && iIndex<m_iAllocatedVertices);
    return &m_pVertex[iIndex];
  }

  /// \brief
  ///   Returns the pointer to the vertex array.
  inline hkvVec3* GetVertexPtr() const {return m_pVertex;}

  /// \brief
  ///   Gets the stride of a vertex data type.
  inline int GetVertexStride() const {return sizeof(hkvVec3);}

  /// \brief
  ///   Set a vertex at a specific index and invalidate the bounding box.
  inline void VISION_FASTCALL SetVertex(int iIndex, const hkvVec3& pos)
  {
    VASSERT(iIndex>=0 && iIndex<m_iAllocatedVertices);
    m_pVertex[iIndex] = pos;
    m_bBoxValid = false;
  }

  /// \brief
  ///   Overrides the number of triangles to use in this mesh.
  /// 
  /// \param iCount
  ///   The number of triangles for this mesh (-1 = maximum). When -1 is used, the number of triangles will be determined based on the current number of indices
  ///   (i.e. GetIndexCount() / 3).
  void SetPrimitiveCount(int iCount) {m_iPrimitiveCount=iCount;}

  /// \brief
  ///   Returns the number of triangles in this mesh.
  /// 
  /// \return
  ///   int: the number of primitives in this mesh. If an explicit primitive count was set using SetPrimitiveCount, that value
  ///   is used, otherwise the returned value is calculated from the number of vertex indices.
  inline int GetPrimitiveCount() const 
  {
    if (m_iPrimitiveCount>=0) return m_iPrimitiveCount;
    if (m_iAllocatedIndices>0) return m_iAllocatedIndices/3;
    return m_iAllocatedVertices/3;
  }

  /// \brief
  ///   Fills a triangle structure with the triangle data at a specified index.
  inline void VISION_FASTCALL GetTriangle(VTriangle &tri, int iIndex) const
  {
    iIndex *= 3;
    if (m_pIndex16)
    {
      VASSERT(m_pIndex16[iIndex+0]<(unsigned short)m_iAllocatedVertices);
      VASSERT(m_pIndex16[iIndex+1]<(unsigned short)m_iAllocatedVertices);
      VASSERT(m_pIndex16[iIndex+2]<(unsigned short)m_iAllocatedVertices);
      tri.SetPoints(&m_pVertex[m_pIndex16[iIndex]],&m_pVertex[m_pIndex16[iIndex+1]],&m_pVertex[m_pIndex16[iIndex+2]]); 
    }
    else if (m_pIndex32)
    {
      VASSERT(m_pIndex32[iIndex+0]<(unsigned int)m_iAllocatedVertices);
      VASSERT(m_pIndex32[iIndex+1]<(unsigned int)m_iAllocatedVertices);
      VASSERT(m_pIndex32[iIndex+2]<(unsigned int)m_iAllocatedVertices);
      tri.SetPoints(&m_pVertex[m_pIndex32[iIndex]],&m_pVertex[m_pIndex32[iIndex+1]],&m_pVertex[m_pIndex32[iIndex+2]]); 
    }
    else
      tri.SetPoints(&m_pVertex[iIndex],&m_pVertex[iIndex+1],&m_pVertex[iIndex+2]);
  }

  /// \brief
  ///   Returns the vertex index of specified triangle corner. The return value is always 32 bit regardless of the internal format
  inline unsigned int GetVertexIndex(int iTriIndex, int iCorner)
  {
    VASSERT(iCorner>=0 && iCorner<3);
    int iIndex = iTriIndex*3+iCorner;
    VASSERT(iIndex>=0 && iIndex<m_iAllocatedIndices);

    return m_pIndex16!=NULL ? (unsigned int)m_pIndex16[iIndex] : (unsigned int)m_pIndex32[iIndex];
  }

  /// \brief
  ///   Performs a traceline test on this collision mesh
  /// 
  /// \param traceStart
  ///   Trace start position 
  /// 
  /// \param traceEnd
  ///   Trace end position 
  /// 
  /// \param iTraceFlags
  ///   bitmask for tracing. Supported constants are:
  ///   \li TRACEFLAG_DOUBLESIDED : Tests all triangles in both directions
  /// 
  ///   \li TRACEFLAG_NOBOUNDINGBOX : If specified, no bounding box early out is performed. Use this if
  ///     the bounding box of the mesh has already been tested with a positive result.
  /// 
  /// \param pStore
  ///   Optional result structure that receives information about first trace hit. Note that
  ///   tracing involves more operations and is thus more expensive if this parameter is specified
  /// 
  /// \return
  ///   bool bHit : true if there was an intersection. More infos about the trace result can be
  ///   obtained from the optional VTraceHitInfo parameter
  VBASE_IMPEXP bool GetTraceIntersection(const hkvVec3& traceStart, const hkvVec3& traceEnd, int iTraceFlags=TRACEFLAG_NONE, VTraceHitInfo *pStore=NULL) const;

  /// \brief
  ///   Performs a traceline test on this collision mesh and returns multiple hits
  /// 
  /// \param traceStart
  ///   Trace start position 
  /// 
  /// \param traceEnd
  ///   Trace end position 
  /// 
  /// \param iTraceFlags
  ///   bitmask for tracing. Supported constants are:
  ///   \li TRACEFLAG_DOUBLESIDED : Tests all triangles in both directions
  /// 
  ///   \li TRACEFLAG_NOBOUNDINGBOX : If specified, no bounding box early out is performed. Use this if
  ///     the bounding box of the mesh has already been tested with a positive result.
  /// 
  /// \param pStoreArray
  ///   Array to at least iArraySize entries of VTraceHitInfo elements. This array gets filled with
  ///   the trace results of the intersection test.
  /// 
  /// \param iArraySize
  ///   The maximum number of supported trace hits (thus pStoreArray must be of sufficient size).
  ///   Tracing stops if the limit is reached.
  /// 
  /// \return
  ///   int iHitCount : number of actual trace hits (i.e. how many pStoreArray elements have been
  ///   filled out). At most iArraySize.
  /// 
  /// \note
  ///   The results in the array are not sorted by distance
  VBASE_IMPEXP int GetTraceIntersection(const hkvVec3& traceStart, const hkvVec3& traceEnd, int iTraceFlags, VTraceHitInfo *pStoreArray, int iArraySize) const;

  /// \brief
  ///   Invalidate the bounding box so that it will be recomputed in the next call to
  ///   GetBoundingBox().
  inline void InvalidateBoundingBox() {m_bBoxValid=false;}

  /// \brief
  ///   Ensure that the bounding box is computed and returns it.
  /// 
  /// \return
  ///   hkvAlignedBBox*: Pointer to the bounding box for this collision mesh.
  inline const hkvAlignedBBox* GetBoundingBox() const {if (!m_bBoxValid) ((VSimpleCollisionMeshBase*)this)->ComputeBoundingBox(); return &m_BoundingBox;}

  /// \brief
  ///   Recomputes the bounding box from the vertices.
  VBASE_IMPEXP void ComputeBoundingBox();

  /// \brief
  ///   Get the amount of memory that this mesh takes. This function is abstract and must be
  ///   implemented in the subclasses.
  /// 
  /// \return
  ///   int: The amount of memory in bytes that this mesh takes.
  virtual int GetMemSize() const = 0;

  /// \brief
  ///   Renders the collision mesh (for debugging purposes)
  /// 
  /// \param pRenderer
  ///   The render interface to render the lines/triangles with
  /// 
  /// \param iColor
  ///   The color used for rendering the lines or faces
  /// 
  /// \param state
  ///   The render state (transparency type,...) for rendering the faces
  /// 
  /// \param iFlags
  ///   Additional render flags. Can be a combination of RENDERSHAPEFLAGS_LINES (render mesh as
  ///   lines) and RENDERSHAPEFLAGS_SOLID (render mesh as solid triangles)
  /// 
  /// \param pCustomTransform
  ///   Optional - used to pass a custom transformation matrix as the world-space transformation the mesh should be rendered with
  VBASE_IMPEXP void Render(IVRenderInterface *pRenderer, VColorRef iColor, const VSimpleRenderState_t &state, int iFlags=RENDERSHAPEFLAGS_LINES, const hkvMat4 *pCustomTransform=NULL) const;

  /// \brief
  ///   Renders the collision mesh in wireframe mode (for debugging purposes) 
  /// 
  /// \param pRenderer
  ///   The render interface to render the wireframe
  /// 
  /// \param iColor
  ///   The color used for rendering the wireframe
  VBASE_IMPEXP void Render(IVRenderInterface *pRenderer, VColorRef iColor) const;


  // Internal use

  // index lists / either the one or the other is used
  unsigned short *m_pIndex16;
  unsigned int *m_pIndex32;

protected:
  // vertices
  int m_iAllocatedVertices;
  hkvVec3* m_pVertex;

  // index count
  // the data type for the indices must be defined in the derived classes
  int m_iAllocatedIndices;

  // used triangles
  int m_iPrimitiveCount;

  // bounding box
  hkvAlignedBBox m_BoundingBox;
  bool m_bBoxValid;

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
