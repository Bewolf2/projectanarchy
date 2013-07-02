/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   Mesh class
///
/// Describes and setups a mesh for usage and processing inside vGeom2. Note that this class derives from
/// VGAnimatable, storing animations relative to the mesh' transformation. E.g. this can/might be used by
/// backends for motion delta extraction.
class VGEOM2_IMPEXP_CLASS VGMesh : public VGAnimatable<AnimKeyPolicy_Xform>, public VGNode, public VGInstantiable
{
public:
  /// \brief
  ///   Type of collision mesh
  ///
  /// Used to specify collision mesh types
  enum CollisionMeshType
  {
    VGCM_TRIANGLEMESH = 0,
    VGCM_CONVEXHULL   = 1,
    VGCM_DEFAULT      = VGCM_TRIANGLEMESH
  };


public:
  VGMesh(VGVertex::VertexMask vm);
  VGMesh(const VGMesh& mesh);
  virtual ~VGMesh();



  /// \brief
  ///   Gets mesh vertex list
  ///
  /// \return
  ///   Reference to vertex list
  inline       VGVertexList&   GetVertexList()       throw()                      { return m_vertexList; }
  inline const VGVertexList&   GetVertexList() const throw()                      { return m_vertexList; }



  /// \brief
  ///   Gets mesh triangle list
  ///
  /// \return
  ///   Reference to triangle list
  inline       VGTriangleList& GetTriangleList()       throw()                    { return m_triangleList; }
  inline const VGTriangleList& GetTriangleList() const throw()                    { return m_triangleList; }



  /// \brief
  ///   Gets collision mesh vertex list
  ///
  /// \return
  ///   Reference to vertex list of collision mesh
  inline       VGVertexList&   GetCollisionMeshVertexList()       throw()         { return m_colVertexList; }
  inline const VGVertexList&   GetCollisionMeshVertexList() const throw()         { return m_colVertexList; }



  /// \brief
  ///   Gets collision mesh triangle list
  ///
  /// \return
  ///   Reference to triangle list of collision mesh
  inline       VGTriangleList& GetCollisionMeshTriangleList()       throw()       { return m_colTriangleList; }
  inline const VGTriangleList& GetCollisionMeshTriangleList() const throw()       { return m_colTriangleList; }



  /// \brief
  ///   Sets collision mesh type
  ///
  /// \param t
  ///   Type of collision mesh used by this mesh
  inline void                  SetCollisionMeshType(CollisionMeshType t) throw()  { m_colMeshType = t; }//@@@ test

  /// \brief
  ///   Gets collision mesh type
  ///
  /// \return
  ///   Type of collision mesh used by this mesh
  inline CollisionMeshType     GetCollisionMeshType() const throw()               { return m_colMeshType; }//@@@ test



  // @@@ disabled for now...
  ///// \brief
  /////   Gets the mesh pivot.
  /////
  ///// \return
  /////   Mesh pivot
  //inline const hkvMat4&      GetPivot() const throw()                           { return m_pivot; }

  ///// \brief
  /////   Sets the mesh pivot.
  /////
  ///// \param pivot
  /////   Mesh pivot
  //inline void                SetPivot(const hkvMat4& pivot)                     { m_pivot = pivot; }


  /// \brief
  ///   Sets whether vertices/ indices should be single or double buffered.
  /// 
  /// \param bVerticesDoubleBuffered
  ///   Storage mode for vertices.
  /// 
  /// \param bIndicesDoubleBuffered
  ///   Storage mode for indices.
  /// 
  /// \param bDoubleBufferingFromFile 
  ///   If value is set to true storage modes will be always determined from corresponding mesh file, global settings will be ignored.
  ///   Otherwise storage modes will be overwritten by global settings.
  inline void                  SetDoubleBuffering(bool bVerticesDoubleBuffered, bool bIndicesDoubleBuffered, bool bDoubleBufferingFromFile)
  {
    m_bVerticesDoubleBuffered = bVerticesDoubleBuffered;
    m_bIndicesDoubleBuffered = bIndicesDoubleBuffered;
    m_bDoubleBufferingFromFile = bDoubleBufferingFromFile;
  }

  /// \brief
  ///   Returns whether vertices/ indices are single or double buffered.
  /// 
  /// \param bVerticesDoubleBuffered
  ///   Storage mode for vertices.
  /// 
  /// \param bIndicesDoubleBuffered
  ///   Storage mode for indices.
  /// 
  /// \param bDoubleBufferingFromFile 
  ///   If value is true storage modes will be always determined from corresponding mesh file, global settings will be ignored.
  inline void                  GetDoubleBuffering(bool &bVerticesDoubleBuffered, bool &bIndicesDoubleBuffered, bool &bDoubleBufferingFromFile) const throw()
  {
    bVerticesDoubleBuffered = m_bVerticesDoubleBuffered;
    bIndicesDoubleBuffered = m_bIndicesDoubleBuffered;
    bDoubleBufferingFromFile = m_bDoubleBufferingFromFile;
  }
                            

  /// \brief
  ///   Extracts a mesh with only the specified material (minimal vertex count); ignores collision mesh
  ///   data, custom bounding boxes, etc. and focuses only on a minimal but valid mesh representation
  ///
  /// \param out
  ///   Mesh to be filled
  ///
  /// \param matIndex
  ///   Index of material to extract data for
  ///
  /// \return
  ///   Returns true if successful
  bool                         Extract(VGMesh& out, int matIndex) const;

  /// \brief
  ///   Extracts a mesh with only the specified triangle range (minimal vertex count); ignores collision mesh
  ///   data, custom bounding boxes, etc. and focuses only on a minimal but valid mesh representation
  ///
  /// \param out
  ///   Mesh to be filled
  ///
  /// \param start
  ///   Index of first triangle
  ///
  /// \param end
  ///   Index of triangle after last one to be grabbed
  ///
  /// \return
  ///   Returns true if successful
  bool                         Extract(VGMesh& out, int start, int end) const;//@@@ test



  /// \brief
  ///   Merges two meshes
  ///
  /// If 'appendOnly' is true, 'other' will be appended to this mesh. Otherwise, function merges
  /// two meshes such that vertices of 'other' only get inserted if not found in current mesh.
  /// Although duplicated vertices are filtered out if 'appendOnly' is set to false, note
  /// that duplicated triangles are not.
  /// Note that if mesh transformations differ, the other mesh will be transformed into 'this'
  /// mesh' space, first.
  /// Note that weight->bone pointers might have to be adapted, manually, if the mesh to merge
  /// doesn't have weights referring to the skeleton in the scene.
  ///
  /// \param other
  ///   Mesh to merge with this one
  ///
  /// \param appendOnly
  ///   Flag specifying if other mesh should be appended or merged
  ///
  /// \return
  ///   Returns false if vertex mask is incompatible.
  bool                         Merge(const VGMesh& other, bool appendOnly = false);

  /// \brief
  ///   Cuts the mesh with a plane
  ///
  /// Triangles will be cut, if necessary - everything will stay in this mesh, vertex
  /// count will probably increase.
  ///
  /// \param plane
  ///   Plane to use for the cut
  ///
  /// \param frontTris
  ///   Optional array to hold indices if split triangles that are in front of the plane
  ///
  /// \param backTris
  ///   Optional array to hold indices if split triangles that are in back of the plane
  ///
  /// \param collectTouching
  ///   Flag to specify whether non-cut, but triangles touching the plane, should be included in frontTris and/or backTris
  ///
  /// \return
  ///   Return if mesh got cut, false if mesh didn't intersect with the plane
  bool                         Cut(const hkvPlane& plane, VArray<int, int>* frontTris = NULL, VArray<int, int>* backTris = NULL, bool collectTouching = false);

  /// \brief
  ///   Cuts the mesh with a plane
  ///
  /// Same as above, but allows you to specify a list of triangle indices to work on.
  ///
  /// \param plane
  ///   Plane to use for the cut
  ///
  /// \param triIndices
  ///   Array of triangle indices to split (others will be skipped)
  ///
  /// \param frontTris
  ///   Optional array to hold indices if split triangles that are in front of the plane
  ///
  /// \param backTris
  ///   Optional array to hold indices if split triangles that are in back of the plane
  ///
  /// \param collectTouching
  ///   Flag to specify whether non-cut, but triangles touching the plane, should be included in frontTris and/or backTris
  ///
  /// \return
  ///   Return if mesh got cut, false if mesh didn't intersect with the plane
  bool                         Cut(const hkvPlane& plane, const VArray<int, int>& triIndices, VArray<int, int>* frontTris = NULL, VArray<int, int>* backTris = NULL, bool collectTouching = false);

  /// \brief
  ///   Cuts the mesh with a (convex) polygon
  ///
  /// Same as above, but only triangles intersecting with the polygon will be cut (meaning triangles intersecting with
  /// the edge of the polygon will be cut as if they would be cut by the plane the polygon is into) - this algorithm
  /// is a bit over-eager, cutting triangles that are very close to the polygon, too.
  ///
  /// \param polygon
  ///   Convex polygon to use for the cut (all vertices must be on a plane)
  ///
  /// \param frontTris
  ///   Optional array to hold indices if split triangles that are in front of the polygon's plane
  ///
  /// \param backTris
  ///   Optional array to hold indices if split triangles that are in back of the polygon's plane
  ///
  /// \param collectTouching
  ///   Flag to specify whether non-cut, but triangles touching the polygon, should be included in frontTris and/or backTris
  ///
  /// \return
  ///   Return if mesh got cut, false if mesh didn't intersect with the polygon
  bool                         Cut(const VArray<hkvVec3, const hkvVec3&>& polygon, VArray<int, int>* frontTris = NULL, VArray<int, int>* backTris = NULL, bool collectTouching = false);//@@@ test explicitly



  /// \brief
  ///   Bakes transformation
  ///
  /// This function transforms the mesh' vertices with the mesh transformation and
  /// sets the latter to identity afterwards.
  void                         BakeTransformation();



  /// \brief
  ///   Copies essential data to passed-in instance, e.g. initial name and transformation, etc..
  ///
  /// \param inst
  virtual void                 CopyEssentialsToInstance(VGInstance& inst) const       { inst.SetName(GetName()); inst.SetTransformation(VGTransformable::GetTransformation()); }

  /// \brief
  ///   Copies essential data from passed-in instance, e.g. initial name and transformation, etc.
  ///
  /// \param inst
  ///   Instance to copy data from
  virtual void                 CopyEssentialsFromInstance(const VGInstance& inst)     { SetName(inst.GetName()); SetTransformation(inst.GetTransformation()); }



  /// \brief
  ///   Gets some brief information about the type of this instantiable
  ///
  /// \return
  ///   String with name of instantiable type
  virtual const char*          GetTypeString() const throw()                          { return "Mesh"; }



  /// \brief
  ///   Assignment operator
  ///
  /// This operator performs a deep copy, updating pointers (e.g. vertex weight pointers),
  /// such that they point to the right bones after the copy, etc..
  ///
  /// \param rhs
  ///   Mesh to assign
  ///
  /// \return
  ///   Reference to this mesh
  VGMesh& operator=(const VGMesh& rhs);

  ///// \brief
  /////   Swap
  /////
  ///// \param other
  /////   Mesh to swap with
  //void Swap(VGMesh& other);


private:
  // Helper.
  bool Extract(VGTriangleList& tl_inout, VGVertexList& vl_out) const;

private:
  // Mesh data.
  VGVertexList      m_vertexList;
  VGTriangleList    m_triangleList;

  // Collision mesh data and type (in case of convex hulls, the triangle list will be empty).
  VGVertexList      m_colVertexList;
  VGTriangleList    m_colTriangleList;
  CollisionMeshType m_colMeshType;

  // Pivot (defaults to identity).
  // @@@ disabled for now...hkvMat4      m_pivot;

  bool              m_bVerticesDoubleBuffered;
  bool              m_bIndicesDoubleBuffered;
  bool              m_bDoubleBufferingFromFile; // force double buffering storage mode from file
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
