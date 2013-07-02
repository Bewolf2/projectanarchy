/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   Triangle list class
///
/// Stores triangle specific data, e.g. indices, material reference, etc.
class VGEOM2_IMPEXP_CLASS VGTriangleList
{
public:
  /// \brief
  ///   Triangle flags
  ///
  /// Stores triangle specific flags, e.g. if the triangle casts a shadow, etc.
  enum TriangleFlags
  {
    VGTF_NOTHING      = 0,
    VGTF_SHADOWCASTER = V_BIT(0), // @@@ do away with this flag???
    VGTF_COLLIDER     = V_BIT(1), // @@@ write testcode that checks whether these triangles get exported to .vcolmesh
    VGTF_VISIBLE      = V_BIT(2), // @@@ write testcode that checks whether these triangles get exported to .model and .vmesh
    VGTF_DEFAULT      = VGTF_COLLIDER|VGTF_VISIBLE
  };

  /// \brief
  ///   Triangle properties
  struct TriangleProperties
  {
    int           materialIndex;      // Index into scene's materials (-1 means no material).
    int           geomInfoIndex;      // Index into scene's geometry meta information (-1 means "use defaults").
    int           groupIndex;         // Index that can be used for custom grouping (user defined).
    int           visibilityID;       // Visibility classification ID (can be set manually or generated with VGProcessor_VisibilityInfoGenerator).
    TriangleFlags triangleFlags;      // Combination of VGTriangleList::TriangleFlags.
    int           physicsInfoIndex;   // Index into scene's geometry physics properties (such as friction, etc.; -1 means "use defaults").

    ///// \brief
    /////   Constructor setting default values
    //TriangleProperties() throw() : materialIndex(-1), geomInfoIndex(-1), groupIndex(0), visibilityID(0), triangleFlags(VGTF_DEFAULT), physicsInfoIndex(-1) { }//@@@ add as init-function
  };

  /// \brief
  ///   Triangle
  struct Triangle
  {
    int                ti[3]; // Triangle indices into a vertex list.
    TriangleProperties tp;
  };


public:
    VGTriangleList();
    virtual ~VGTriangleList();



  /// \brief
  ///   Adds a triangle to the triangle list
  ///
  /// \param t
  ///   Triangle to add to the list
  inline void            AddTriangle(const Triangle& t)    { m_triangles.Add(t); }

  /// \brief
  ///   Returns the number of triangles in the triangle list
  ///
  /// \return
  ///   Number of triangles
  inline int             GetNumTriangles() const           { return m_triangles.GetSize(); }

  /// \brief
  ///   Returns the specified triangle of the triangle list
  ///
  /// \param index
  ///   Index of triangle to return (must be: 0 <= index < number of triangles)
  ///
  /// \return
  ///   Requested triangle
  inline       Triangle& GetTriangle(int index)            { VASSERT(index>=0 && index<GetNumTriangles()); return m_triangles[index]; }
  inline const Triangle& GetTriangle(int index) const      { VASSERT(index>=0 && index<GetNumTriangles()); return m_triangles[index]; } // @@@ add operator[] as equivalent



  /// \brief
  ///   Appends triangles from another triangle list
  ///
  /// \param other
  ///   Triangle list to append to this one
  ///
  /// \param indexOffset
  ///   Vertex index offset to automatically add to appended triangles
  void                   Append(const VGTriangleList& other, int indexOffset = 0);



  /// \brief
  ///   Gets a sublist containing all triangles referencing a given material (stable - order of triangles in sublist is preserved)
  ///
  /// \param matIndex
  ///   Material to extract
  ///
  /// \param out
  ///   Triangle list to be filled with triangles referencing the passed in material index - old data will be cleared
  void                   GetSublist(int matIndex, VGTriangleList& out) const;

  /// \brief
  ///   Gets a sublist containing all triangles referencing a given geometry info (stable - order of triangles in sublist is preserved)
  ///
  /// \param geomInfoIndex
  ///   Geometry info to extract
  ///
  /// \param out
  ///   Triangle list to be filled with triangles referencing the passed in geometry info index - old data will be cleared
  void                   GetSublist_GeomInfo(int geomInfoIndex, VGTriangleList& out) const; // @@@ RENAME?

  /// \brief
  ///   Gets a sublist containing all triangles referencing a given group (stable - order of triangles in sublist is preserved)
  ///
  /// \param groupIndex
  ///   Group to extract
  ///
  /// \param out
  ///   Triangle list to be filled with triangles referencing the passed in group index - old data will be cleared
  void                   GetSublist_Group(int groupIndex, VGTriangleList& out) const; // @@@ RENAME?

  /// \brief
  ///   Gets a sublist containing all triangles referencing a given visibility zone (stable - order of triangles in sublist is preserved)
  ///
  /// \param visibilityID
  ///   Visibility zone ID to extract
  ///
  /// \param out
  ///   Triangle list to be filled with triangles referencing the passed in visibility zone - old data will be cleared
  void                   GetSublist_Visibility(int visibilityID, VGTriangleList& out) const; // @@@ RENAME?

  /// \brief
  ///   Gets a sublist containing all triangles referencing a given physics info (stable - order of triangles in sublist is preserved)
  ///
  /// \param physicsInfoIndex
  ///   Physics info to extract
  ///
  /// \param out
  ///   Triangle list to be filled with triangles referencing the passed in physics info index - old data will be cleared
  void                   GetSublist_PhysicsInfo(int physicsInfoIndex, VGTriangleList& out) const; // @@@ RENAME?

  /// \brief
  ///   Gets a sublist containing all triangles with flags masked by a given flag mask (order of triangles in sublist is preserved)
  ///
  /// \param mask
  ///   Triangle flags to extract
  ///
  /// \param out
  ///   Triangle list to be filled with triangles having at least the flags set that are in the passed-in mask - old data will be cleared
  void                   GetSublist(TriangleFlags mask, VGTriangleList& out) const;



  /// \brief
  ///   Returns an array of material indices used in this triangle set (function may be slow in case of many referenced materials - O(n log n))
  ///
  /// \param out
  ///   List of material indices - old data will be cleared
  void                   GetUsedMaterials(VArray<int, int>& out) const;

  /// \brief
  ///   Returns an array of geometry info indices used in this triangle set (function may be slow in case of many referenced geometry infos - O(n log n))
  ///
  /// \param out
  ///   List of geometry info indices - old data will be cleared
  void                   GetUsedGeometryInfos(VArray<int, int>& out) const;

  /// \brief
  ///   Returns an array of group indices used in this triangle set (function may be slow in case of many referenced groups - O(n log n))
  ///
  /// \param out
  ///   List of group indices - old data will be cleared
  void                   GetUsedGroups(VArray<int, int>& out) const;

  /// \brief
  ///   Returns an array of visibility zone IDs used in this triangle set (function may be slow in case of many referenced geometry infos - O(n log n))
  ///
  /// \param out
  ///   List of visibility zone IDs - old data will be cleared
  void                   GetUsedVisibilityIDs(VArray<int, int>& out) const;

  /// \brief
  ///   Returns an array of physics info indices used in this triangle set (function may be slow in case of many referenced physics infos - O(n log n))
  ///
  /// \param out
  ///   List of physics info indices - old data will be cleared
  void                   GetUsedPhysicsInfos(VArray<int, int>& out) const;



  /// \brief
  ///   Groups triangles by materials (stable - order of triangles in groups is preserved)
  ///
  /// Convenience function sorting the triangles by material (note that it only groups and doesn't sort)
  void                   GroupByMaterial();

  /// \brief
  ///   Groups triangles by geometry information (stable - order of triangles in groups is preserved)
  ///
  /// Convenience function sorting the triangles by geometry infos they are referencing (note that it only groups and doesn't sort)
  void                   GroupByGeometryInfo();

  /// \brief
  ///   Groups triangles by group index (stable - order of triangles in groups is preserved)
  ///
  /// Convenience function sorting the triangles by group index (note that it only groups and doesn't sort)
  void                   GroupByGroupIndex();

  /// \brief
  ///   Groups triangles by visibility zone ID (stable - order of triangles in groups is preserved)
  ///
  /// Convenience function sorting the triangles by visibility zone ID (note that it only groups and doesn't sort)
  void                   GroupByVisibilityZoneID();

  /// \brief
  ///   Groups triangles by physics information (stable - order of triangles in groups is preserved)
  ///
  /// Convenience function sorting the triangles by physics infos they are referencing (note that it only groups and doesn't sort)
  void                   GroupByPhysicsInfo();



  /// \brief
  ///   Swaps contents of this list with another list (fast)
  ///
  /// \param other
  ///   Triangle list to swap contents with
  void                   Swap(VGTriangleList& other);

  /// \brief
  ///   Removes all the triangles from the triangle list
  inline void            Clear()                           { m_triangles.RemoveAll(); }



  /// \brief
  ///   Set size of triangle list (allocate a given number of triangles - renders old data in triangle list invalid)
  ///
  /// \param n
  ///   Number of triangles to create
  inline void            SetSize(int n)                    { m_triangles.SetSize(n); }



  /// \brief
  ///   Creates triangle adjacency information (note that this compares triangle indices, only, not real
  ///   vertices, as a triangle list knows nothing about the vertices, of course)
  ///
  /// \param out
  ///   Array to be filled with integers indexing adjacent triangles (for every edge, meaning 3 indices per triangle)
  void                   ComputeAdjacencyInformation(VArray<int, int>& out) const;



private:
  void FindAdjacentTriangles(VArray<int, int>& out, bool ignoreMats = false) const;
  int FindTriangleForEdge(int e0, int e1, int mat, int start, int& otherEdge, bool ignoreMats = false) const;


private:
  VArray<Triangle, const Triangle&> m_triangles;
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
