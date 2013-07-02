/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_NAVMESH_EDGE_PAIR_INFO_H
#define HK_AI_NAVMESH_EDGE_PAIR_INFO_H

#include <Ai/Pathfinding/NavMesh/hkaiNavMeshInstance.h>

#ifdef HK_PLATFORM_SPU
#	include <Ai/Pathfinding/NavMesh/Spu/hkaiSpuNavMeshAccessor.h>
#endif

/// A structure contained edge and face information that is passed to hkaiAstarCostModifier
/// and hkaiAstarEdgeFilter callbacks
struct hkaiNavMeshEdgePairInfo
{
	public:
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI, hkaiNavMeshEdgePairInfo);

		/// Flags to indicate from where the cost modifier or edge filter is being called
		enum QueryType
		{
			/// Called from A* queries, e.g., hkaiPathfindingUtil::findPath
			QUERY_TYPE_ASTAR			= 1<<0,

			/// Called during line of sight queries, e.g., hkaiLineOfSightUtil::checkLineOfSight
			QUERY_TYPE_LINE_OF_SIGHT	= 1<<1,

			/// Called from character steering code, e.g., hkaiNavMeshUtils::getNearbyBoundaries
			QUERY_TYPE_LOCAL_STEERING	= 1<<2,

			/// Called by the user, i.e., non-Havok code
			QUERY_TYPE_USER				= 1<<3,

			/// Called to examine the start point of a path
			QUERY_START_POINT			= 1<<4,

			/// Called to examine an intermediate edge in the middle of a path
			QUERY_INTERMEDIATE_POINT	= 1<<5,

			/// Called to examine the goal of a path
			QUERY_END_POINT				= 1<<6
		};

			/// Information regarding an edge being examined.
		struct EdgeInfo
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI, hkaiNavMeshEdgePairInfo::EdgeInfo);

			HK_FORCE_INLINE EdgeInfo();

				/// Validation checks.
			inline void validate( const hkaiNavMeshAccessor* mesh ) const;

				/// Initialize this with the specified edge information.
			HK_FORCE_INLINE void setEdge(hkaiPackedKey edgeKey, const hkaiNavMesh::Edge* edge, const hkaiNavMesh::EdgeData* edgeData, hkVector4Parameter pos);

			//
			// Accessors
			//
			HK_FORCE_INLINE hkaiPackedKey getEdgeKey() const { return m_edgeKey; }
			HK_FORCE_INLINE const hkaiNavMesh::Edge* getEdge() const { return m_edge; }
			HK_FORCE_INLINE const hkaiNavMesh::EdgeData* getEdgeData() const { return m_edgeData; }
			HK_FORCE_INLINE const hkVector4& getPosition() const { return m_position; }

			HK_FORCE_INLINE void setPosition( hkVector4Parameter pos ) { m_position = pos; }
		
		protected:

				/// The key of the edge.
			HK_PAD_ON_SPU( hkaiPackedKey ) m_edgeKey;

				/// A pointer to the edge. This may by NULL when considering a start or end point.
			HK_PAD_ON_SPU( const hkaiNavMesh::Edge* ) m_edge;

				/// A pointer to the edge data, or NULL if hkaiNavMesh::m_edgeDataStriding is zero.
				/// On SPU, this will be the address of the data in main memory.
			HK_PAD_ON_SPU( const hkaiNavMesh::EdgeData* ) m_edgeData;

				/// The position on the edge (or the start/end position of the search)
			hkVector4 m_position;

		#if defined(HK_DEBUG)
			static HK_FORCE_INLINE bool edgeInfoEqual(const hkaiNavMesh::Edge& e0, const hkaiNavMesh::Edge& e1);
		#endif
		};

		HK_FORCE_INLINE hkaiNavMeshEdgePairInfo();

			/// Initialize this with the specified face information.
		HK_FORCE_INLINE void setFace( hkaiPackedKey faceKey, const hkaiNavMesh::Face* face, const hkaiNavMesh::FaceData* faceData );
	
			/// Set the incoming data to be a start point.
		HK_FORCE_INLINE void setStartPoint(hkVector4Parameter pos);
		
			/// Set the outgoing data to be a goal point.
		HK_FORCE_INLINE void setEndPoint(hkVector4Parameter pos);

			/// Validation checks.
		inline void validate( const hkaiNavMeshAccessor* mesh ) const;
		
		//
		// Accessors
		//
		HK_FORCE_INLINE hkaiPackedKey getFaceKey() const { return m_faceKey; }
		HK_FORCE_INLINE const hkaiNavMesh::Face* getFace() const { return m_face; }
		HK_FORCE_INLINE const hkaiNavMesh::FaceData* getFaceData() const { return m_faceData; }

	public:
			/// Edge information for the incoming edge.
		EdgeInfo m_incomingEdgeInfo;

			/// Edge information for the outgoing edge.
		EdgeInfo m_outgoingEdgeInfo;

	private:
			/// Key of the face
			/// The face will be the face for the outgoing edge, and the opposite face for the incoming edge.
		HK_PAD_ON_SPU( hkaiPackedKey ) m_faceKey;

			/// Pointer to the face.
		HK_PAD_ON_SPU( const hkaiNavMesh::Face*	) m_face;

			/// The face data for the face, or NULL if hkaiNavMesh::m_faceDataStriding is zero.
			/// On SPU, this will be the address of the data in main memory.
		HK_PAD_ON_SPU( const hkaiNavMesh::FaceData* ) m_faceData;
};

#include <Ai/Pathfinding/NavMesh/hkaiNavMeshEdgePairInfo.inl>

#endif // HK_AI_NAVMESH_EDGE_PAIR_INFO_H

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
