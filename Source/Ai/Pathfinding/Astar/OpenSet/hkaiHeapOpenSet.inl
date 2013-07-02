/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


hkaiHeapOpenSet::hkaiHeapOpenSet(char* storage, int storageSize)
:	m_heap( reinterpret_cast<IndexCostPair*>(storage) ),
	m_heapSize(0),
	m_maxSize( storageSize / sizeof(IndexCostPair) )
{
	HK_ASSERT(0x2fe6e26d, m_maxSize == getAllowedNumNodes(storageSize) );
}


inline int hkaiHeapOpenSet::getMemoryRequirement( int numNodes )
{
	return numNodes * sizeof(IndexCostPair);
}


inline int hkaiHeapOpenSet::getAllowedNumNodes( int numBytes )
{
	return numBytes / sizeof(IndexCostPair);
}


inline hkaiHeapOpenSet::~hkaiHeapOpenSet()
{
}


void hkaiHeapOpenSet::heapSwapEntries(int i, int j)
{
	HK_ASSERT(0x14c5b95a, i != j);
	hkAlgorithm::swap( m_heap[i], m_heap[j] );
}

// Moves the i'th heap entry up the heap (if needed) by swapping with it's parent, until heap property is satisfied

inline void hkaiHeapOpenSet::upHeap(int i)
{
	int currentIndex = i;
	int parentIndex = heapGetParent(currentIndex);

	// Copy the pair that we're moving
	SearchIndex currentPairIndex = m_heap[i].m_idx;
	IntCost currentPairCost = m_heap[i].m_cost.m_intCost;

	while (currentIndex > 0 && m_heap[parentIndex].m_cost.m_intCost > currentPairCost)
	{
		// Copy the parent down, but don't swap them (to avoid LHS penalties)
		m_heap[currentIndex] = m_heap[parentIndex];
		currentIndex = parentIndex;
		parentIndex = heapGetParent(currentIndex);
	}

	m_heap[currentIndex].m_idx = currentPairIndex;
	m_heap[currentIndex].m_cost.m_intCost = currentPairCost;
}


inline int hkaiHeapOpenSet::heapGetSmallerChild(int i) const
{
	const int leftIndex = heapGetLeftChild(i);
	const int rightIndex = heapGetRightChild(i);

	if (rightIndex >= m_heapSize)
		return leftIndex;
	else
	{
		int leftOrRight = (m_heap[leftIndex].m_cost.m_intCost > m_heap[rightIndex].m_cost.m_intCost);
		return leftIndex + leftOrRight;
	}

}

// Moves the i'th heap entry down the heap (if needed) by swapping with it's smaller child, until heap property is satisfied

inline void hkaiHeapOpenSet::downHeap(int i)
{
	int currentIndex = i;
	const int heapSize = m_heapSize;

	// If rightIndex is bigger than heapSize, take the leftIndex. Otherwise, take the index with the smaller corresponding cost
	int swapIndex = heapGetSmallerChild(currentIndex);

	while (swapIndex < heapSize && m_heap[swapIndex].m_cost.m_intCost < m_heap[currentIndex].m_cost.m_intCost)
	{
		heapSwapEntries(currentIndex, swapIndex);
		currentIndex = swapIndex;
		swapIndex = heapGetSmallerChild(currentIndex);
	}
}


template <typename CostEstimator>
inline void hkaiHeapOpenSet::insertSearchIndex( const CostEstimator& estimator, SearchIndex nid )
{
	HK_ASSERT(0x3670441e, isOk());

	const PathCost newCost = estimator.estimatedCost(nid);
	insertSearchIndex(nid, newCost);
}


inline void hkaiHeapOpenSet::insertSearchIndex( SearchIndex nid, PathCost newCost )
{
	HK_ASSERT(0x63834378, isOk());
	HK_ASSERT(0x157de74a, newCost >= 0.0f );

	const int heapSize = m_heapSize;
	IndexCostPair& newPair = m_heap[m_heapSize];
	m_heapSize = m_heapSize + 1;
	newPair.init(nid, newCost);

	upHeap(heapSize);

	HK_ASSERT(0x41c23daa, isOk());
}


inline hkaiHeapOpenSet::SearchIndex hkaiHeapOpenSet::popNextSearchIndex()
{
	HK_ASSERT(0x4abe9c4a, isOk());

	SearchIndex ret = m_heap[0].m_idx;

	// equivalent to m_heap.removeAt(0);
	m_heapSize = m_heapSize-1;
	m_heap[0] = m_heap[m_heapSize];
	downHeap(0);

	HK_ASSERT(0x5fe20827, isOk());
	return ret;
}

inline int hkaiHeapOpenSet::getSize() const
{
	return m_heapSize;
}


inline hkBool32 hkaiHeapOpenSet::isEmpty() const
{
	return m_heapSize == 0;
}


inline hkBool32 hkaiHeapOpenSet::isFull() const
{
	return m_heapSize >= m_maxSize;
}


inline hkBool32 hkaiHeapOpenSet::isOk()
{
#ifdef HK_DEBUG
	for (int h=0; h<m_heapSize; h++)
	{
		IndexCostPair& pair = m_heap[h];

		HK_ASSERT(0x14272e2e, pair.m_cost.m_floatCost >= 0.0f );

		// Verify the heap property
		int leftIndex = heapGetLeftChild(h);
		int rightIndex = heapGetRightChild(h);

		if (leftIndex < m_heapSize)
		{
			HK_ASSERT(0x41f9fded, pair.m_cost.m_floatCost <= m_heap[leftIndex].m_cost.m_floatCost);
		}
		if (rightIndex < m_heapSize)
		{
			HK_ASSERT(0x41f9fded, pair.m_cost.m_floatCost <= m_heap[rightIndex].m_cost.m_floatCost);
		}
	}
#endif // HK_DEBUG
	return true;
}

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
