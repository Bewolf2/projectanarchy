/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_NAVIGATION_MESH_GENERATION_CALLBACK_H
#define HKAI_NAVIGATION_MESH_GENERATION_CALLBACK_H

#include <Ai/Internal/NavMesh/hkaiNavMeshGenerationSettings.h>
#include <Ai/Internal/NavMesh/hkaiNavMeshGenerationTypes.h>

class hkBitField;
struct hkGeometry;

struct hkaiNavMeshGenerationProgressCallbackContext
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiNavMeshGenerationProgressCallbackContext);

	hkaiNavMeshGenerationProgressCallbackContext( const hkaiNavMeshGenerationSettings& settings );

	const hkaiNavMeshGenerationSettings&	m_settings;
	const hkGeometry*						m_weldedGeometry;
	const hkBitField*						m_walkableTriangles;
	const hkBitField*						m_cuttingTriangles;
	const hkaiNavMesh*						m_unsimplifiedNavMesh;
	const hkaiNavMesh*						m_simplifiedNavMesh;

	//
	// Simplification info
	//
	const hkaiNavMesh* m_originalMesh;
	const hkaiNavMesh* m_simplifiedMesh; // After vertexMerging
	const hkArrayBase<int>* m_boundaryFilterNumEdgesInGroup;
	const hkArrayBase<hkaiNavMesh::EdgeIndex>* m_boundaryFilterEdgesToRemove;
	const hkArrayBase<int>* m_numFacesInPartition;
	const hkArrayBase<hkaiNavMesh::FaceIndex>* m_partitions;
	const hkArrayBase<hkaiNavMeshSimplificationUtils::Segment>* m_segments;
	const hkArrayBase<hkVector4>* m_segmentVertices;
};


	/// Callbacks that are triggered after each stage of nav mesh generation
	/// If the callback returns false, generation is stopped.
class hkaiNavMeshGenerationProgressCallback
{
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiNavMeshGenerationProgressCallback);

	enum NavMeshGenerationStep
	{
		START = 0,
		CONVERT_AND_MERGE,
		BUILD_AABB_TREE, 
		CSG_AND_TRIANGULATION,
		WELD_DUPLICATE_VERTICES,
		REMOVE_UNREACHABLE_TRIANGLES,
		CREATE_MESH,
		CONNECT_EDGES,
		CLEANUP1,
		EROSION,
		CLEANUP2,
		SIMPLIFY_START,
		MERGE_SHARED_VERTICES,
		EDGE_FILTER_SETUP,
		PARTITION,
		TRACE_BOUNDARIES,
		SEGMENT_SIMPLIFY,
		REBUILD_MESH,
		STITCH_EDGES,
		HERTEL_MEHLHORN,
		SIMPLIFY_END,
		END,
		STEP_MAX
	};

		/// Information about the generation progress.
	typedef hkaiNavMeshGenerationProgressCallbackContext CallbackContext;

		/// Callback fired at the start of a step.
	virtual bool stepStartedCallback( NavMeshGenerationStep stepType, const CallbackContext& ctx ) { return true; } 

		/// Callback fired in the middle of some steps. This can be fired a lot (potentially once for each triangle in the mesh).
		/// So for slow actions like updating a progress baf, it is recommended to only process a fraction
		//  of the callbacks, e.g. one out of a thousand.
	virtual bool stepMiddleCallback(  NavMeshGenerationStep stepType, int subStep, int numSubSteps, const CallbackContext& ctx ) { return true; } 

		/// Callback fired at the end of a step.
	virtual bool stepEndedCallback(  NavMeshGenerationStep stepType, const CallbackContext& ctx) { return true; } 

	virtual void edgeRejectedCallback( const hkaiNavMesh& mesh, hkaiNavMesh::FaceIndex faceA, hkaiNavMesh::EdgeIndex edgeA,
		hkaiNavMesh::FaceIndex faceB, hkaiNavMesh::EdgeIndex edgeB, hkaiNavMeshGenerationConnectivityResult::ConnectivityResultEnum	failureReason ) {}

	virtual ~hkaiNavMeshGenerationProgressCallback() {}
};


#endif // HKAI_NAVIGATION_MESH_GENERATION_CALLBACK_H

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
