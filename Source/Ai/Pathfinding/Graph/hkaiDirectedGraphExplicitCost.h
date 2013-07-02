/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_DIRECTEDGRAPHEXPLICITCOST_H
#define HKAI_DIRECTEDGRAPHEXPLICITCOST_H

#include <Ai/Pathfinding/hkaiBaseTypes.h>
#include <Ai/Pathfinding/NavMesh/Streaming/hkaiStreamingSet.h>

extern const class hkClass hkaiDirectedGraphExplicitCostExternalEdgeInfoClass;
extern const class hkClass hkaiDirectedGraphExplicitCostEdgeClass;
extern const class hkClass hkaiDirectedGraphExplicitCostNodeClass;
extern const hkClass hkaiDirectedGraphExplicitCostClass;

/// A simple directed graph.
/// Each node has multiple edges starting from it, and going to another node (specified by the edge's target).
/// Each edge has an explicitly defined cost with it; this is typically the Euclidean distance between nodes,
/// but isn't required to be.
class hkaiDirectedGraphExplicitCost : public hkReferencedObject
{
public:
	//+vtable(true)
	//+version(6)
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_ASTAR);
	HK_DECLARE_REFLECTION();

	hkaiDirectedGraphExplicitCost();

	hkaiDirectedGraphExplicitCost( hkFinishLoadedObjectFlag f );

	// hkaiAstar interface
	typedef hkUint32 SearchIndex;
	typedef int EdgeIndex;
	typedef int NodeIndex;

		/// The cost of traveling from one Node to another
	typedef hkReal EdgeCost;

		/// An hkVector4 representing a position in the graph.
	typedef hkVector4 Position;

	enum Constants
	{
		INVALID_NODE_INDEX	 = -1,
		INVALID_EDGE_INDEX   = -1,
		INVALID_VERTEX_INDEX = -1,
	};

		/// A node in the graph. Edges for each node are stored contiguously in m_edges
	struct Node
	{
		// +version(1)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, Node );
		HK_DECLARE_POD_TYPE();
			/// The index of the node's first edge (in the m_edges array)
		int m_startEdgeIndex;

			/// The number of edges for this node
		int m_numEdges;
	};

	enum EdgeBits
	{
			/// Indicates the edge is related to an hkaiNavMesh::UserEdge.
		EDGE_IS_USER = 2,

			/// Indicates the edge connects to a separate graph.
			/// This has the same value as hkaiNavMesh::EDGE_EXTERNAL_OPPOSITE.
		EDGE_EXTERNAL_OPPOSITE = 64,
	};


		/// An edge in the graph. Each edge points to a node, and has a cost associated with travelling to the target node.
	struct Edge
	{
		//+version(1)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, Edge );
		HK_DECLARE_POD_TYPE();

		HK_FORCE_INLINE void init( SearchIndex target, hkReal cost);

		HK_FORCE_INLINE hkBool32 isExternal() const;
		HK_FORCE_INLINE hkBool32 isUserEdge() const;
		HK_FORCE_INLINE void setTargetKey( hkaiPackedKey k);

		HK_FORCE_INLINE hkaiPackedKey getOppositeNodeKeyUnchecked() const;
		HK_FORCE_INLINE NodeIndex getOppositeNodeIndex() const;

			/// Cost for traversing the edge
		hkHalf m_cost;

			/// Flags for this edge
		hkFlags<EdgeBits, hkUint16> m_flags;

	protected:
			/// The node that the edge points to
		SearchIndex m_target;
	};


	HK_FORCE_INLINE void init(const hkaiDirectedGraphExplicitCost* gConst);
	HK_FORCE_INLINE void init(Position* pos, Node* nodes, Edge* edges, int numNodes, int numEdges);
	HK_FORCE_INLINE void clearAndDeallocate();

		/// Copys the nodes, edges, and vertices of the other graph. Returns HK_SUCCESS if all memory operations succeeded.
	hkResult copy( const hkaiDirectedGraphExplicitCost& other );
	void swap( hkaiDirectedGraphExplicitCost& other );

	void validate() const;
	
	//
	// Accessor interface
	//	
	HK_FORCE_INLINE void getPosition( SearchIndex a, hkVector4& v ) const;
	HK_FORCE_INLINE const Node& getNode( SearchIndex a) const;
	HK_FORCE_INLINE const Edge& getEdge( EdgeIndex a) const;

	HK_FORCE_INLINE int getNumNodes() const;
	HK_FORCE_INLINE int getNumEdges() const;
	HK_FORCE_INLINE const Edge* getEdgeBegin() const;
	

public:
		/// Node positions. m_nodes[i] is located at m_positions[i]
	hkArray<hkVector4> m_positions;
	
		/// Array of nodes
	hkArray<struct Node> m_nodes;

		/// Array of edges
	hkArray<struct Edge> m_edges;

	hkArray<hkaiStreamingSet>	m_streamingSets;

protected:
	hkaiDirectedGraphExplicitCost( const hkaiDirectedGraphExplicitCost& other );
	void operator=(const hkaiDirectedGraphExplicitCost& other);

};


#include <Ai/Pathfinding/Graph/hkaiDirectedGraphExplicitCost.inl>

#endif // HKAI_DIRECTEDGRAPHEXPLICITCOST_H

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
