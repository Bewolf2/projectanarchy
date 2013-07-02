/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_STREAMING_CONNECTION_SET_H
#define HKAI_STREAMING_CONNECTION_SET_H

#include <Ai/Pathfinding/hkaiBaseTypes.h>

	/// A list of connections between two structures (e.g. hkaiNavMesh or hkaiDirectedGraphExplicitCost).
	/// These connections are used by the streaming system to add edge to the structures' instanced
	/// counterparts (e.g. hkaiNavMeshInstance or hkaiDirectedGraphInstance) at runtime.
struct hkaiStreamingSet
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH, hkaiStreamingSet);
	HK_DECLARE_REFLECTION();

	hkaiStreamingSet()
	:	m_thisUid(0),
		m_oppositeUid(0)
	{

	}

	hkaiStreamingSet( hkFinishLoadedObjectFlag f)
	:	m_meshConnections(f),
		m_graphConnections(f),
		m_volumeConnections(f)
	{
	}

	typedef int NodeIndex;
	typedef int CellIndex;
	typedef int FaceIndex;
	typedef int EdgeIndex;


	struct NavMeshConnection
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH, hkaiStreamingSet::NavMeshConnection);
		HK_DECLARE_REFLECTION();
		HK_DECLARE_POD_TYPE();

		HK_FORCE_INLINE FaceIndex getFaceIndex() const { return m_faceIndex; }
		HK_FORCE_INLINE EdgeIndex getEdgeIndex() const { return m_edgeIndex; }
		HK_FORCE_INLINE FaceIndex getOppositeFaceIndex() const { return m_oppositeFaceIndex; }
		HK_FORCE_INLINE EdgeIndex getOppositeEdgeIndex() const { return m_oppositeEdgeIndex; }
		
		FaceIndex m_faceIndex;			//+default(-1);
		EdgeIndex m_edgeIndex;			//+default(-1);
		FaceIndex m_oppositeFaceIndex;	//+default(-1);
		EdgeIndex m_oppositeEdgeIndex;	//+default(-1);
	};

	struct GraphConnection
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH, hkaiStreamingSet::GraphConnection);
		HK_DECLARE_REFLECTION();
		HK_DECLARE_POD_TYPE();

		HK_FORCE_INLINE NodeIndex getNodeIndex() const { return m_nodeIndex; }
		HK_FORCE_INLINE NodeIndex getOppositeNodeIndex() const { return m_oppositeNodeIndex; }

		NodeIndex m_nodeIndex;			//+default(-1);
		NodeIndex m_oppositeNodeIndex;	//+default(-1);
	};

	struct VolumeConnection
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH, hkaiStreamingSet::VolumeConnection);
		HK_DECLARE_REFLECTION();
		HK_DECLARE_POD_TYPE();

		HK_FORCE_INLINE CellIndex getCellIndex() const { return m_cellIndex; }
		HK_FORCE_INLINE CellIndex getOppositeCellIndex() const { return m_oppositeCellIndex; }

		CellIndex m_cellIndex;			//+default(-1);
		CellIndex m_oppositeCellIndex;	//+default(-1);
	};

	static void HK_CALL _findOrExpandSetsForUids( hkArray<hkaiStreamingSet>& streamingSetsA, hkArray<hkaiStreamingSet>& streamingSetsB, hkaiSectionUid uidA, hkaiSectionUid uidB, hkaiStreamingSet*& setAOut, hkaiStreamingSet*& setBOut );

	static hkResult HK_CALL _copy( hkArray<hkaiStreamingSet>& dst, const hkArrayBase<hkaiStreamingSet>& src );

	hkaiSectionUid m_thisUid;		//+default( HKAI_INVALID_SECTION_UID );
	hkaiSectionUid m_oppositeUid;	//+default( HKAI_INVALID_SECTION_UID );

	hkArray<NavMeshConnection> m_meshConnections;
	hkArray<GraphConnection> m_graphConnections;
	hkArray<VolumeConnection> m_volumeConnections;
};


#endif // HKAI_STREAMING_CONNECTION_SET_H

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
