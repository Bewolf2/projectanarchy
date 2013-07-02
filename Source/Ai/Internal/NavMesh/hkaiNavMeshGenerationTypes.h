/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_NAVIGATION_MESH_GENERATION_TYPES_H
#define HKAI_NAVIGATION_MESH_GENERATION_TYPES_H

class hkBitField;
struct hkGeometry;
class hkaiNavMesh;

	/// Possible results when connecting edges.
struct hkaiNavMeshGenerationConnectivityResult
{
	enum ConnectivityResultEnum
	{
		EDGES_UNALIGNED_ANGLE_PLANAR,
		EDGES_UNALIGNED_ANGLE_VERTICALLY,
		EDGES_EXCEED_MAX_SEPARATION,
		EDGES_NO_OVERLAP,
		EDGES_OVERLAP_LESS_THAN_MIN_OVERLAP,
		EDGES_INTERVALS_DISJOINT,
		EDGES_EXCEED_MAX_STEP_HEIGHT,
		EDGES_OVERHANG_EXCEEDED,
		FACES_OVERHANG_COMPLETELY, 
		FACES_THIN_OR_CONCAVE, 
		EDGES_EXCEED_RAYCAST_MAX_STEP_HEIGHT,
		EDGES_RAYCAST_OBSTRUCTION,
		EDGES_CLIMBING_UNALIGNED_FACE_NORMAL_ANGLE,
		EDGES_CLIMBING_UNALIGNED_EDGE_ANGLE,

		CONNECTIVITY_MAX,

		CONNECT_SUCCESS = -1
	};
};

	/// Structure to specify output artifacts from nav mesh generation. In addition to the standard nav mesh
	/// outut, the intermediate artifacts may be useful to save for some kinds of post-processing operations
	/// (ex. automatic cover detection).
	///
	/// For each of the optional outputs, if the address supplied is non-NULL, then the object at that
	/// location will be used to store the output object requested.
	///
	/// \note None of these output pointers may alias each other
	///
struct hkaiNavMeshGenerationOutputs
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH, hkaiNavMeshGenerationOutputs);

	hkaiNavMeshGenerationOutputs();

		/// Geometry used for generation, formed by welding nearby vertices and removing duplicate triangles.
	hkGeometry*						m_weldedGeometry;

		/// Bitfield that indicates which of the m_weldedGeometry triangles are walkable
	hkBitField*						m_walkableTriangles;

		/// Bitfield that indicates which of the m_weldedGeometry triangles are cutting
	hkBitField*						m_cuttingTriangles;

		/// The generated nav mesh, before simplification is applied. This output may only be requested if
		/// simplification is enabled in the nav mesh generation settings.
	hkaiNavMesh* m_unsimplifiedNavMesh;

		/// The final nav mesh.
	hkaiNavMesh* m_navMesh;
};

struct hkaiNavMeshGenerationResult
{
	enum GenerationResultEnum
	{
		/// Generation was stopped by the user.
		GENERATION_HALTED,

		/// Generated nav mesh was empty at some point in the process.
		GENERATION_EMPTY,

		/// Generation ran out of memory.
		GENERATION_OUT_OF_MEMORY,

		/// Generation could not be performed due to an invalid or expired keycode.
		GENERATION_KEYCODE_ERROR,

		/// Generation created an intermediate nav mesh with more than HKAI_MAX_NUM_FACES faces or 
		/// HKAI_MAX_NUM_EDGES edges. If this happens, try hkaiSplitGenerationUtils to break the input geometry
		/// into smaller pieces and rejoin them later.
		GENERATION_TOO_BIG,

		/// Generation succeeded.
		GENERATION_SUCCESS,
	};
};

#endif // HKAI_NAVIGATION_MESH_GENERATION_TYPES_H

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
