/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_DIRECTED_GRAPH_INSTANCE_H
#define HKAI_DIRECTED_GRAPH_INSTANCE_H

#include <Ai/Pathfinding/Graph/hkaiDirectedGraphExplicitCost.h>
extern const hkClass hkaiDirectedGraphInstanceClass;

/// A modifiable instance of an hkaiDirectedGraphExplicitCost.
/// This only supports adding additional edges to a node.
class hkaiDirectedGraphInstance : public hkReferencedObject
{
public:
	//+vtable(true)
	//+version(0)
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_ASTAR);
	HK_DECLARE_REFLECTION();

	// hkaiAstar interface
	typedef hkaiDirectedGraphExplicitCost::SearchIndex SearchIndex;
	typedef hkaiDirectedGraphExplicitCost::EdgeIndex EdgeIndex;
	typedef hkaiDirectedGraphExplicitCost::NodeIndex NodeIndex;
	typedef hkaiDirectedGraphExplicitCost::EdgeCost EdgeCost;
	typedef hkaiDirectedGraphExplicitCost::Edge Edge;

	hkaiDirectedGraphInstance();
	hkaiDirectedGraphInstance( hkFinishLoadedObjectFlag f );

	void init(const hkaiDirectedGraphExplicitCost* graph, hkaiSectionUid uid);
	void tempInit( const hkaiDirectedGraphExplicitCost* graph);
	HK_FORCE_INLINE const hkaiDirectedGraphExplicitCost* getOriginalGraph() const;

	HK_FORCE_INLINE int getNumNodes() const;
#ifndef HK_PLATFORM_SPU
	HK_FORCE_INLINE int getNumOriginalEdges() const;
	HK_FORCE_INLINE int getNumOwnedEdges() const;
	HK_FORCE_INLINE int getNumEdges() const { return getNumOriginalEdges() + getNumOwnedEdges(); }
#endif
	HK_FORCE_INLINE const Edge* getEdgeBegin() const;
	HK_FORCE_INLINE const Edge* getOwnedEdgeBegin() const;
	
	HK_FORCE_INLINE const hkaiDirectedGraphExplicitCost::Node& getNode( NodeIndex n ) const;
	HK_FORCE_INLINE const hkaiDirectedGraphExplicitCost::Edge& getEdge( EdgeIndex e ) const;
	HK_FORCE_INLINE void getPosition(NodeIndex pid, hkVector4& posOut) const;
	HK_FORCE_INLINE void getLocalPosition(NodeIndex pid, hkVector4& posOut) const;

	HK_FORCE_INLINE const hkTransform& getTransform() const;
	HK_FORCE_INLINE void setTransform( const hkTransform& t);

	HK_FORCE_INLINE hkaiPackedKey getOppositeNodeKeyForEdge( const hkaiDirectedGraphExplicitCost::Edge& edge ) const;

	HK_FORCE_INLINE void getInstancedNode( NodeIndex n, hkaiDirectedGraphExplicitCost::Node& node ) const;
	HK_FORCE_INLINE hkaiDirectedGraphExplicitCost::Node& getWritableInstancedNode( NodeIndex n );
	HK_FORCE_INLINE hkaiDirectedGraphExplicitCost::Edge& getWritableEdge( EdgeIndex e );

		/// Returns a pointer to the node, or HK_NULL if it was not previously instanced
	HK_FORCE_INLINE hkaiDirectedGraphExplicitCost::Node* getInstancedNodePtr( NodeIndex n );

		/// Get the unique section ID for this graph instance.
	HK_FORCE_INLINE hkaiSectionUid getSectionUid() const;

		/// Get the runtime ID for this graph instance.
	HK_FORCE_INLINE hkaiRuntimeIndex getRuntimeId() const;

	/// Output the closest node to the given position in world space.
	hkaiPackedKey getClosestNode( hkVector4Parameter position ) const;

	void validate() const;


#ifndef HK_PLATFORM_SPU

		/// Assign the unique section ID for this graph. Note that this should be the same as 
		/// the corresponding nav mesh instance's UID.
	inline void setSectionUid( hkaiSectionUid uid );

private:

		/// Assign the runtime ID for this graph, when the graph is loaded.
	inline void setRuntimeId( hkaiRuntimeIndex id );

	inline void setOriginalPointers( const hkaiDirectedGraphExplicitCost* graph);

	hkaiDirectedGraphExplicitCost::Edge* addEdgeForNode( NodeIndex n );
	void removeOwnedEdgeForNode( NodeIndex n, EdgeIndex e );
		/// Returns the start index of the new block.
	hkaiDirectedGraphExplicitCost::EdgeIndex expandEdgesBy(int n );
	void clearOwnedEdges();

	void addFreeBlock( EdgeIndex blockStart, int blockSize );
	EdgeIndex findFreeBlock( int blockSize );

	friend class hkaiHierarchyUtils;
	friend class hkaiStreamingManager;
	friend class hkaiStreamingCollection;
#endif

protected:

	//
	// Pointers to the original graph data, and their respective sizes.
	//

	const hkaiDirectedGraphExplicitCost::Node*	m_originalNodes;		//+nosave
	int											m_numOriginalNodes;		//+nosave

	const hkaiDirectedGraphExplicitCost::Edge*	m_originalEdges;		//+nosave
	int											m_numOriginalEdges;		//+nosave

	const hkVector4*							m_originalPositions;	//+nosave
	// Note that numOriginalPositions is the same m_numOriginalNodes 

		/// Unique section ID of this graph.
	hkaiSectionUid m_sectionUid; //+default( HKAI_INVALID_SECTION_UID );

		/// Runtime ID of this graph.
		/// Only assigned when the graph is loaded.
	hkaiRuntimeIndex m_runtimeId; //+default( HKAI_INVALID_RUNTIME_INDEX )

	hkRefPtr<const hkaiDirectedGraphExplicitCost>	m_originalGraph;
	hkArray<int>									m_nodeMap;
	hkArray<hkaiDirectedGraphExplicitCost::Node>	m_instancedNodes;
	hkArray<hkaiDirectedGraphExplicitCost::Edge>	m_ownedEdges;

		/// The "reference count" for graph edges which correspond to user edges in the nav mesh.
		/// Same size as m_ownedEdges.
public:
	hkArray<int>									m_userEdgeCount;

		/// A list of "holes" in the m_ownedEdges array that are created when adding new edges.
		/// Serialization doesn't support hkArrays of hkArrays, so we have to wrap the array in a struct
	struct FreeBlockList
	{
		HK_DECLARE_REFLECTION();
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI, FreeBlockList);
		FreeBlockList() {}
		FreeBlockList(hkFinishLoadedObjectFlag f) : m_blocks(f) {}

		int getSize() const { return m_blocks.getSize(); }
		int back() const { return m_blocks.back(); }
		void pushBack(int i) { m_blocks.pushBack(i); } 
		void popBack() { m_blocks.popBack(); }
		hkArray<int> m_blocks;
	};

protected:
		/// A list of "holes" in the m_ownedEdges array that are created when adding new edges.
		/// m_freeEdgeBlocks[i] is the list of holes of size i+1.
	hkArray< FreeBlockList >							m_freeEdgeBlocks;

	hkTransform m_transform;

	HK_ON_SPU(friend struct hkaiSpuDirectedGraphAccessor);
};


#ifdef HK_PLATFORM_SPU
#	include <Ai/Pathfinding/Multithreaded/Spu/hkaiSpuPathfindingConfig.h>
#	include <Common/Base/Memory/PlatformUtils/Spu/SpuDmaCache/hkSpu4WayCache.h>
#endif

#ifdef HK_PLATFORM_SPU
/// Utility wrapper for getting graph information on SPU.
struct hkaiSpuDirectedGraphAccessor
{
	typedef hkaiDirectedGraphExplicitCost::Node Node;
	typedef hkaiDirectedGraphExplicitCost::Edge Edge;
	typedef hkaiDirectedGraphExplicitCost::EdgeIndex EdgeIndex;
	typedef hkaiDirectedGraphExplicitCost::NodeIndex NodeIndex;
	typedef hkVector4 Position;

	HK_FORCE_INLINE hkaiSpuDirectedGraphAccessor();

	/// same as hkaiDirectedGraphExplicitCost::Node, but with padded members. Only used on SPU.
	struct PaddedNode
	{
		hkPadSpu<int> m_startEdgeIndex;
		hkPadSpu<int> m_numEdges;

		PaddedNode() {}

		PaddedNode(const hkaiDirectedGraphExplicitCost::Node& unpadded)
		{
			m_startEdgeIndex = unpadded.m_startEdgeIndex;
			m_numEdges       = unpadded.m_numEdges;
		}
	};

	void init(const hkaiDirectedGraphInstance* mainMemGraph);
	void setSection( /*const hkaiStreamingCollection::InstanceInfo* */ const void* infoMainMem, hkaiRuntimeIndex sectionId );

	HK_FORCE_INLINE int getNumOriginalEdges() const;
	HK_FORCE_INLINE int getNumOwnedEdges() const { return m_numOwnedEdges; }
	//HK_FORCE_INLINE int getNumOwnedEdges() const;
	HK_FORCE_INLINE const Edge* getEdgeBegin() const;
	HK_FORCE_INLINE const Edge* getOwnedEdgeBegin() const;

	HK_FORCE_INLINE const hkaiDirectedGraphExplicitCost::Node& getNode( NodeIndex n ) const;
	HK_FORCE_INLINE void getPosition(NodeIndex pid, hkVector4& posOut) const;
	HK_FORCE_INLINE void getLocalPosition(NodeIndex pid, hkVector4& posOut) const;
	HK_FORCE_INLINE void getInstancedNode( NodeIndex n, PaddedNode& node ) const;
	HK_FORCE_INLINE hkaiPackedKey getOppositeNodeKeyForEdge( const hkaiDirectedGraphExplicitCost::Edge& edge ) const;

	HK_FORCE_INLINE const hkTransform& getTransform() const;
	HK_FORCE_INLINE hkaiRuntimeIndex getRuntimeId() const;

	hkPadSpu<const Node*>		m_originalNodes;		
	hkPadSpu<int>				m_numOriginalNodes;		

	hkPadSpu<const Edge*>		m_originalEdges;		
	hkPadSpu<int>				m_numOriginalEdges;		

	hkPadSpu<const hkVector4*>	m_originalPositions;	

	hkPadSpu<hkaiRuntimeIndex>	m_runtimeId;
	hkPadSpu<const int*>		m_nodeMap;
	
	hkPadSpu<const Node*>		m_instancedNodes;
	hkPadSpu<int>				m_numInstancedNodes;
	
	hkPadSpu<const Edge*>		m_ownedEdges;
	hkPadSpu<int>				m_numOwnedEdges;

	hkPadSpu<const hkaiDirectedGraphInstance*> m_graphMainMem;

	hkTransform m_transform;
};

typedef hkaiSpuDirectedGraphAccessor hkaiDirectedGraphAccessor;
#else
typedef hkaiDirectedGraphInstance hkaiDirectedGraphAccessor;
#endif


#include <Ai/Pathfinding/Graph/hkaiDirectedGraphInstance.inl>

#endif // HKAI_DIRECTED_GRAPH_INSTANCE_H

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
