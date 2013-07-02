/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   Vertex list class
///
/// Stores vertices in optimized lists for processing and usage in vGeom2; inherits
/// vertex animation features
class VGEOM2_IMPEXP_CLASS VGVertexList : public VGAnimatable<AnimKeyPolicy_Vertex>
{
public:
  typedef VGVertex::Weights                     VGVWeights;
  typedef VArray<hkvVec2,    const hkvVec2&>    Vertex2Array;
  typedef VArray<hkvVec3,    const hkvVec3&>    Vertex3Array;
  typedef VArray<hkvVec4,    const hkvVec4&>    Vertex4Array;
  typedef VArray<hkvVec4,    const hkvVec4&>   ColorArray;
  typedef VArray<VGVWeights, const VGVWeights&> WeightsArray;


public:
  VGVertexList(VGVertex::VertexMask vm);
  VGVertexList(const VGVertexList& other);
  virtual ~VGVertexList();



  /// \brief
  ///   Returns the vertex mask assigned to this list
  ///
  /// \return
  ///   Vertex mask assigned to this list
  inline VGVertex::VertexMask  GetVertexMask() const                             { return m_vm; }



  /// \brief
  ///   Adds a vertex to this list
  ///
  /// \param v
  ///   Vertex to add to this list
  ///
  /// \return
  ///   Returns false if list uses a different vertex mask (vertex gets logically adapted and added, though)
  bool                         AddVertex(const VGVertex& v);

  /// \brief
  ///   Adds a vertex to this list uniquely
  ///
  /// Adds a vertex to this list uniquely, meaning that it doesn't get appended if there is already an exact
  /// copy of it in the list - if the vertex' vertexmask is different, the vertex gets adapted logically
  ///
  /// \param v
  ///   Vertex to add to this list
  ///
  /// \return
  ///   Returns index of added vertex
  int                          AddVertex_Unique(const VGVertex& v);

  /// \brief
  ///   Returns index of a vertex
  ///
  /// Searches for a vertex (which must have the same vertex mask as the list) and returns its index
  ///
  /// \param v
  ///   Compatible vertex to search for in the list
  ///
  /// \param start
  ///   Index of vertex into list to start search with
  ///
  /// \return
  ///   Returns index of found vertex or -1 if not found (or if passed in vertex is incompatible)
  int                         FindFirstVertex(const VGVertex& v, int start=0) const;

  /// \brief
  ///   Returns number of vertices in vertex list
  ///
  /// \return
  ///   Number of vertices in vertex list
  inline int                   GetNumVertices() const                            { return m_hashes.GetSize(); }



  /// \brief
  ///   Returns list of vertex positions
  ///
  /// \return
  ///   Reference to vertex position array (check vertexmask if it contains data)
  inline const Vertex3Array&   GetPositions() const throw()                      { return m_positions; }

  /// \brief
  ///   Returns list of vertex normals
  ///
  /// \return
  ///   Reference to vertex normal array (check vertexmask if it contains data)
  inline const Vertex3Array&   GetNormals() const throw()                        { return m_normals; }

  /// \brief
  ///   Returns list of vertex tangents
  ///
  /// \return
  ///   Reference to vertex tangent array (check vertexmask if it contains data)
  inline const Vertex4Array&   GetTangents() const throw()                       { return m_tangents; }

  /// \brief
  ///   Returns list of vertex texture coordinates
  ///
  /// \param i
  ///   Texture coordinate set index (0-7)
  ///
  /// \return
  ///   Reference to vertex texture coordinate array (check vertexmask if it contains data)
  inline const Vertex2Array&   GetTexCoords(int i) const                         { VASSERT(i>=0 && i<VGVertex::MAX_NUM_TEXCOORDS); return m_texCoords[i]; }

  /// \brief
  ///   Returns list of vertex colors
  ///
  /// \param i
  ///   Color set index (0-1)
  ///
  /// \return
  ///   Reference to vertex color array (check vertexmask if it contains data)
  inline const ColorArray&     GetColors(int i) const                            { VASSERT(i>=0 && i<VGVertex::MAX_NUM_COLORS);    return m_colors   [i]; }

  /// \brief
  ///   Returns list of vertex weights
  ///
  /// \return
  ///   Reference to vertex weight array (check vertexmask if it contains data)
  inline const WeightsArray&   GetWeights() const throw()                        { return m_weights; }



  /// \brief
  ///   Returns a given vertex position
  ///
  /// \param i
  ///   Index of position to retrieve
  ///
  /// \return
  ///   Const reference to requested vertex position (throws if not available)
  inline const hkvVec3&        GetPosition(int i) const                          { return m_positions[i]; }

  /// \brief
  ///   Returns a given vertex normal
  ///
  /// \param i
  ///   Index of normal to retrieve
  ///
  /// \return
  ///   Const reference to requested vertex normal (throws if not available)
  inline const hkvVec3&        GetNormal(int i) const                            { return m_normals  [i]; }

  /// \brief
  ///   Returns a given vertex tangent
  ///
  /// \param i
  ///   Index of tangent to retrieve
  ///
  /// \return
  ///   Const reference to requested vertex tangent - note that if tangent's w < 0, tangent space is considered to be right-handed (throws if not available)
  inline const hkvVec4&        GetTangent(int i) const                           { return m_tangents [i]; }

  /// \brief
  ///   Returns a given vertex texture coordinate
  ///
  /// \param set
  ///   Texture coordinate set index (0-7)
  ///
  /// \param i
  ///   Index of texture coordinate to retrieve
  ///
  /// \return
  ///   Const reference to requested vertex texture coordinate (throws if not available)
  inline const hkvVec2&        GetTexCoord(int set, int i) const                 { VASSERT(set>=0 && set<VGVertex::MAX_NUM_TEXCOORDS); return m_texCoords[set][i]; }

  /// \brief
  ///   Returns a given vertex color
  ///
  /// \param set
  ///   Color set index (0-1)
  ///
  /// \param i
  ///   Index of color to retrieve
  ///
  /// \return
  ///   Const reference to requested vertex color (throws if not available)
  inline const hkvVec4&       GetColor(int set, int i) const                    { VASSERT(set>=0 && set<VGVertex::MAX_NUM_COLORS);    return m_colors   [set][i]; }

  /// \brief
  ///   Returns weighting data for a given vertex
  ///
  /// \param i
  ///   Index of weighting data to retrieve
  ///
  /// \return
  ///   Const reference to requested vertex weighting data (throws if not available)
  inline const VGVWeights&     GetVertexWeights(int i) const                     { return m_weights[i]; }



  /// \brief
  ///   Sets a vertex position
  ///
  /// \param i
  ///   Index of position to set
  ///
  /// \param v
  ///   Position vector to set
  void                         SetPosition(int i, const hkvVec3& v);

  /// \brief
  ///   Sets a vertex normal
  ///
  /// \param i
  ///   Index of normal to set
  ///
  /// \param v
  ///   Normal vector to set
  void                         SetNormal(int i, const hkvVec3& v);

  /// \brief
  ///   Sets a vertex tangent
  ///
  /// \param i
  ///   Index of tangent to set
  ///
  /// \param v
  ///   Tangent vector to set note that if tangent's w < 0, tangent space is considered to be right-handed
  void                         SetTangent(int i, const hkvVec4& v);

  /// \brief
  ///   Sets a vertex texture coordinate
  ///
  /// \param set
  ///   Texture coordinate set index (0-7)
  ///
  /// \param i
  ///   Index of texture coordinate to set
  ///
  /// \param v
  ///   Texture coordinate vector to set
  void                         SetTexCoord(int set, int i, const hkvVec2& v);

  /// \brief
  ///   Sets a vertex color
  ///
  /// \param set
  ///   Color set index (0-1)
  ///
  /// \param i
  ///   Index of color to set
  ///
  /// \param c
  ///   Color to set
  void                         SetColor(int set, int i, const hkvVec4& c);

  /// \brief
  ///   Sets vertex weights
  ///
  /// \param i
  ///   Index of vertex weights to set
  ///
  /// \param w
  ///   Weights to set
  void                         SetVertexWeights(int i, const VGVWeights& w);



  /// \brief
  ///   Merges two vertex lists (respects custom bounding boxes and vertex animation data)
  ///
  /// \param other
  ///   Vertex list to be merged with this one
  ///
  /// \param remappingList
  ///   List to be filled with remapping indices of merged vertices. If NULL, other list will be appended,
  ///   without trying to add the vertices uniquely
  ///
  /// \return
  ///   False if vertex mask is incompatible
  bool                         Merge(const VGVertexList& other, VArray<int, int>* remappingList = NULL);

  ///// \brief
  /////   Remove all duplicated vertices (respects vertex animation data)
  /////
  ///// \param remappingList
  /////   List to be filled with remapping indices of all vertices
  //void                       RemoveDuplicates(VArray<int, int>& remappingList);//@@@ merge change, test anim data

  /// \brief
  ///   Enhances the vertex list with new components - after calling this function, the vertex mask of this
  ///   vertex list will equal "current mask OR passed in mask", with the previously missing components added
  ///   and set to default values (respects/keeps vertex animation data)
  ///
  /// \param vm
  ///   Vertex mask specifying vertex components to add
  void                         Enhance(VGVertex::VertexMask vm);



  /// \brief
  ///   Swaps contents of this list with another list (fast)
  ///
  /// \param other
  ///   Triangle list to swap contents with
  void                         Swap(VGVertexList& other);

  /// \brief
  ///   Removes all the triangles from the triangle list
  inline void                  Clear()                                           { VGVertexList v(GetVertexMask()); v.Swap(*this); }



  /// \brief
  ///   Generates a bounding box encompassing all position vertices in this list - note that this
  ///   will compute the unanimated bounding box, vertex animations won't influence the result
  ///
  /// \param bbox
  ///   Bounding box to be generated/filled in
  ///
  /// \return
  ///   Returns false if there are no position vertices
  inline bool                  ComputeBBox(hkvAlignedBBox& bbox) const                   { return ComputeBBox(bbox, 0, GetNumVertices()); }//@@@ write version that will compute bbox respecting vertex anims?

  /// \brief
  ///   Generates a bounding box encompassing a range of position vertices in this list - note that this
  ///   will compute the unanimated bounding box, vertex animations won't influence the result
  ///
  /// \param bbox
  ///   Bounding box to be generated/filled in
  ///
  /// \param startVert
  ///   Start index of vertex range to compute bounding box for
  ///
  /// \param endVert
  ///   End index of vertex range to compute bounding box for (indexed vertex is excluded)
  ///
  /// \return
  ///   Returns false if there are no position vertices
  bool                         ComputeBBox(hkvAlignedBBox& bbox, int startVert, int endVert) const;//@@@ write version that will compute bbox respecting vertex anims?

  /// \brief
  ///   Generates a sphere encompassing all position vertices in this list - note that this
  ///   will compute the unanimated bounding sphere, vertex animations won't influence the result
  ///
  /// \param o
  ///   Desired bounding sphere's origin
  ///
  /// \return
  ///   Returns bounding sphere's radius (or -1.f if there are no position vertices in this list)
  float                        ComputeBoundingSphere(hkvVec3& o) const;//@@@ write version that will compute bbox respecting vertex anims?



  /// \brief
  ///   Creates a sublist with a given vertex mask
  ///
  /// \param inOut
  ///   Vertex list to be filled in with sublist data - vertex mask of the passed-in list will be used
  ///   as filter for sublist (selectively grabs vertex animation data, as well)
  ///
  /// \return
  ///   Return false if vertex mask requests data that isn't available
  bool                         GetSublist(VGVertexList& inOut) const;

  /// \brief
  ///   Generates a remapping list, mapping vertices (of another list) to first occurances of vertices (of this list) with identical
  ///   vertex components as specified by a passed-in vertex mask (respecting vertex animations)
  ///
  /// \param vl
  ///   Vertex list to get remapping for
  ///
  /// \param remappingList
  ///   Remapping list to be filled in with indices to the first occurrence of a vertex in this list (adds -1 if not found)
  ///
  /// \param vm
  ///   Mask specifying crucial vertex components for comparison
  ///
  /// \return
  ///   Aborts and returns false if vertex masks of both lists don't match
  bool                         ComputeUniqueVertexRemappingList(const VGVertexList& vl, VArray<int, int>& remappingList, VGVertex::VertexMask vm) const;

  /// \brief
  ///   Generates a remapping list, mapping vertices to first occurances of vertices with identical vertex components as
  ///   specified by a passed-in vertex mask (respecting vertex animations)
  ///
  /// \param remappingList
  ///   Remapping list to be filled in with indices to the first occurrence of a vertex
  ///
  /// \param vm
  ///   Mask specifying crucial vertex components for comparison
  void                         ComputeUniqueVertexRemappingList(VArray<int, int>& remappingList, VGVertex::VertexMask vm) const;

  /// \brief
  ///   Generates a remapping list, indexing 'id' vertices
  ///
  /// Generates a remapping list, indexing 'id' vertices. The latter are defined to be a unique subset of vertices
  /// based on positions and weights (respecting vertex animations)
  ///
  /// \param remappingList
  ///   Remapping list to be filled in with indices to the first occurrence of a so called 'id' vertex
  ///
  /// \return
  ///   Returns false if there are no position vertices
  bool                         ComputeIdVertexRemappingList(VArray<int, int>& remappingList) const;



  /// \brief
  ///   Transforms vertex positions, normals and tangents (note that this will transform custom bounding boxes
  ///   as well by transforming them and recreating it with the transformed vertices - this will most likely grow
  ///   those boxes)
  ///
  /// \param mat
  ///   Transformation matrix used for transformation
  void                         Transform(const hkvMat4& mat);//@@@ merge change, test anim data



  /// \brief
  ///   Extracts a VGVertex out of the data arrays (slow - don't use extensively)
  ///
  /// \param out
  ///   Vertex data to be filled in
  ///
  /// \param i
  ///   Vertex index to extract
  inline void                  ExtractVertex(VGVertex& out, int i) const         { ExtractVertex(out, i, GetVertexMask()); }

  /// \brief
  ///   Extracts a VGVertex out of the data arrays with specifying desired vertexmask (slow - don't use extensively)
  ///
  /// \param out
  ///   Vertex data to be filled in
  ///
  /// \param i
  ///   Vertex index to extract
  ///
  /// \param vm
  ///   Vertex mask specifying vertex data to extract - must be a subset of this list's mask
  ///
  /// \return
  ///   False if vertex mask is not a subset of this list's mask
  bool                         ExtractVertex(VGVertex& out, int i, VGVertex::VertexMask vm) const;



  /// \brief
  ///   Set a VGVertex
  ///
  /// \param i
  ///   Vertex index in list to be filled with new vertex data
  ///
  /// \param v
  ///   Vertex to set
  ///
  /// \return
  ///   False if vertex mask of vertex doesn't match the list's mask
  bool                         SetVertex(int i, const VGVertex& v);



  /// \brief
  ///   Set size of vertex list (allocate a given number of vertices - renders old data in
  ///   vertex list invalid and removes animation data)
  ///
  /// \param n
  ///   Number of vertices to create
  inline void                  SetSize(int n)                                    { SetSize(n, false); }



  /// \brief
  ///   Set custom bounding box or NULL if no custom bounding box should be used (free to be used in any way, Vision file format specific
  ///   backends use it as collision bounding box by convention)
  ///
  /// \param b
  ///   Pointer to custom bounding box or NULL (box will be deep-copied)
  inline void                  SetCustomBoundingBox(const hkvAlignedBBox* b)             { delete m_customBBox; m_customBBox = NULL; if(b) m_customBBox = new hkvAlignedBBox(*b); }

  /// \brief
  ///   Get custom bounding box or NULL if none is set (free to be used in any way, backends use it as collision bounding box by convention)
  ///
  /// \return
  ///   Pointer to custom bounding box or NULL
  inline const hkvAlignedBBox* GetCustomBoundingBox() const throw()              { return m_customBBox; }



  /// \brief
  ///   Assignment operator
  ///
  /// \param rhs
  ///   Right hand side
  ///
  /// \return
  ///   Reference to this vertex list
  VGVertexList&               operator=(const VGVertexList& rhs);



private:
  void AddVertex_Raw(const VGVertex& v);
  bool CompareVertex(const VGVertex& v, int i) const;
  void SetSize(int n, bool forceKeepAnimations);



private:
  // Vertex mask used by this list.
  VGVertex::VertexMask m_vm;

  // Vertex data blocks.
  Vertex3Array m_positions;
  Vertex3Array m_normals;
  Vertex4Array m_tangents;
  Vertex2Array m_texCoords[VGVertex::MAX_NUM_TEXCOORDS];
  ColorArray   m_colors   [VGVertex::MAX_NUM_COLORS];
  WeightsArray m_weights;

  VArray<int, int> m_hashes;

  // Custom bounding box.
  const hkvAlignedBBox* m_customBBox;
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
