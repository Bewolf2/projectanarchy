/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_STREAMING_UTILS_H
#define HKAI_STREAMING_UTILS_H

#include <Ai/Pathfinding/NavMesh/hkaiNavMeshUtils.h>
#include <Common/Base/Algorithm/Collide/1AxisSweep/hk1AxisSweep.h>
#include <Ai/Internal/NavMesh/hkaiNavMeshGenerationSettings.h>

class hkAabb;
class hkaiNavMeshQueryMediator;
class hkaiDirectedGraphExplicitCost;

	/// Miscellaneous utility functions for setting up streaming nav mesh sections
class hkaiStreamingUtils
{
	public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiStreamingUtils);
		
			/// Input structure for hkaiStreamingUtils::findEdgeOverlaps
		struct FindEdgeOverlapInput
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH, FindEdgeOverlapInput);
			FindEdgeOverlapInput();
			
				/// The first mesh being connected. The face, edge, and vertex data of the mesh will not be modified, but its hkaiStreamingSets will be.
			hkaiNavMesh* m_meshA;

				/// The second mesh being connected. The face, edge, and vertex data of the mesh will not be modified, but its hkaiStreamingSets will be.
			hkaiNavMesh* m_meshB;

				/// The UID of the first mesh instance.
			hkaiSectionUid m_uidA; 

				/// The UID of the second mesh instance.
			hkaiSectionUid m_uidB;

				/// The mediator for the first mesh being connected.
			const hkaiNavMeshQueryMediator* m_mediatorA;

				/// The mediator for the second mesh being connected.
			const hkaiNavMeshQueryMediator* m_mediatorB;

				/// Optional coarse graph for the first mesh. If m_graphA is HK_NULL, m_graphB must be also.
				/// The node, edge, and position data of the graph will not be modified, but its hkaiStreamingSets will be.
			hkaiDirectedGraphExplicitCost* m_graphA;

				/// Optional coarse graph for the second mesh. If m_graphA is HK_NULL, m_graphB must be also.
				/// The node, edge, and position data of the graph will not be modified, but its hkaiStreamingSets will be.
			hkaiDirectedGraphExplicitCost* m_graphB;

				/// AABB expansion tolerance used when detecting nearby edges.
			hkReal m_edgeMatchTolerance;

				/// World up vector.
			hkVector4 m_up;

				/// Edge matching parameters used to determine whether edges can be connected.
			hkaiNavMeshGenerationSettings::EdgeMatchingParameters m_edgeMatchingParams;

				/// Mesh-to-world transform for the first mesh.
			hkTransform	m_transformA;

				/// Mesh-to-world transform for the second mesh.
			hkTransform	m_transformB;
		};

			/// Finds overlaps between an array of AABBs. This should be used to test for potential dependencies between nav mesh sections.
			/// Once the potential dependencies are known, the individual edges can be collided against each other
		static void HK_CALL findPotentialDependencies( const hkArrayBase<hkAabb>& aabbs, hkArray<struct hkKeyPair>::Temp& pairs, hkReal tolerance = .05f );

			/// Finds all boundary edges in the mesh that overlap with the specified AABB, and appends their integer AABB to the array.
			/// The AabbInt's keys are indices into the FaceEdgeKeyPair array
		static void HK_CALL collectBoundaryEdges( const hkaiNavMeshInstance& instance, const hkaiNavMeshQueryMediator* mediator, const hkAabb& otherAabb, hkArray<hk1AxisSweep::AabbInt>::Temp& aabbsInt, hkArray<hkaiNavMeshUtils::FaceEdgeKeyPair>::Temp& facesEdges, hkSimdRealParameter tolerance);

			/// Generate static 1-to-1 streaming information for a pair of hkaiNavMesh's
		static void HK_CALL generateStreamingInfo( const FindEdgeOverlapInput& input );

		static void HK_CALL findEdgeOverlaps( const FindEdgeOverlapInput& input, hkaiStreamingSet* meshSetA, hkaiStreamingSet* meshSetB, hkaiStreamingSet* graphSetA, hkaiStreamingSet* graphSetB );

		static void HK_CALL connectEdges( const hkaiNavMesh& sectionA, int faceIdxA, int edgeIdxA, const hkaiDirectedGraphExplicitCost* graphA, hkaiStreamingSet* meshSetA, hkaiStreamingSet* graphSetA, 
										  const hkaiNavMesh& sectionB, int faceIdxB, int edgeIdxB, const hkaiDirectedGraphExplicitCost* graphB, hkaiStreamingSet* meshSetB, hkaiStreamingSet* graphSetB );
};


#endif // HKAI_STREAMING_UTILS_H

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
