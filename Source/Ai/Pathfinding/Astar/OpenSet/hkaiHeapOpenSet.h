/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_ASTAR_HEAP_OPENSET_H
#define HKAI_ASTAR_HEAP_OPENSET_H

#include <Common/Base/Algorithm/Sort/hkSort.h>

/// A* open set that stores potential nodes in a heap.
struct hkaiHeapOpenSet
{
	typedef int SearchIndex;
	typedef hkReal PathCost;
#if defined(HK_REAL_IS_DOUBLE)
	typedef hkInt64 IntCost;
#else
	typedef hkInt32 IntCost;
#endif

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, hkaiHeapOpenSet );

		/// Structure that stores the index of a node in the search state, and its associated cost.
	struct IndexCostPair
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, IndexCostPair );
		/// Index of the corresponding node.
		HK_ALIGN(SearchIndex m_idx, 8);
		HK_DECLARE_POD_TYPE();

		/// Estimated node cost.
		/// Because the costs are always positive, we can use integer comparisons instead of floating comparisons
		union CostUnion
		{
			PathCost m_floatCost;
			IntCost  m_intCost;
		};

		CostUnion m_cost;

		HK_FORCE_INLINE void init(SearchIndex i, PathCost c)
		{
			m_idx = i;
			m_cost.m_floatCost = c;
		}
	};


	inline hkaiHeapOpenSet(char* storage, int storageSize);
	inline ~hkaiHeapOpenSet();

		/// Returns the number of bytes required to store the specified number of nodes
	HK_FORCE_INLINE static int getMemoryRequirement( int numNodes );
	
		/// Returns the number of nodes that can be stored in the specified number of bytes.
	HK_FORCE_INLINE static int getAllowedNumNodes( int numBytes );

	template< typename CostEstimator >
	inline void insertSearchIndex(const CostEstimator& estimator, SearchIndex nid);

	inline void insertSearchIndex(SearchIndex nid, PathCost newCost);
	inline SearchIndex popNextSearchIndex();
	void adjustValue( SearchIndex nid, PathCost updatedCost );

	HK_FORCE_INLINE int getSize() const;
	HK_FORCE_INLINE hkBool32 isEmpty() const;
	HK_FORCE_INLINE hkBool32 isFull() const;


protected:
	inline hkBool32 isOk();

	HK_FORCE_INLINE void heapSwapEntries(int i, int j);
	HK_FORCE_INLINE int heapGetParent(int i) const {return (i-1) / 2;}
	HK_FORCE_INLINE int heapGetLeftChild(int i) const {return (2*i + 1);}
	HK_FORCE_INLINE int heapGetRightChild(int i) const {return (2*i + 2);}
	HK_FORCE_INLINE int heapGetSmallerChild(int i) const;

	HK_FORCE_INLINE void upHeap(int i);
	HK_FORCE_INLINE void downHeap(int i);

	inline void verifyHeap();


public:

	HK_PAD_ON_SPU(IndexCostPair*) m_heap;
	HK_PAD_ON_SPU(int) m_heapSize;
	HK_PAD_ON_SPU(int) m_maxSize;

};

#include <Ai/Pathfinding/Astar/OpenSet/hkaiHeapOpenSet.inl>
#endif // HKAI_ASTAR_HEAP_OPENSET_H

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
