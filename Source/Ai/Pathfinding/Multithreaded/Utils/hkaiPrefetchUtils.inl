/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

template <typename T, const int dmaGroup>
hkSimplePrefetcher<T, dmaGroup>::hkSimplePrefetcher()
{
#ifdef HK_PLATFORM_SPU
	HK_ASSERT2(0x5dd801b8, (sizeof(T) & 0xf) == 0, "Size must be a multiple of 16.");
#endif

	HK_ON_DEBUG(m_waitDone = true);
}

template <typename T, const int dmaGroup>
void hkSimplePrefetcher<T, dmaGroup>::prefetch(const T* addr)
{
	HK_ASSERT2(0x238341f1, m_waitDone, "Can't start prefetch without waiting for previous prefetch.");
	HK_ON_DEBUG(m_waitDone = false);

	if (addr)
	{
#ifdef HK_PLATFORM_SPU
		hkSpuDmaManager::getFromMainMemory(&m_prefetchedData, addr, sizeof(T), hkSpuDmaManager::READ_COPY, dmaGroup);
		HK_SPU_DMA_DEFER_FINAL_CHECKS_UNTIL_WAIT(addr, &m_prefetchedData, sizeof(T));
#else
		m_prefetchedData = *addr;
#endif
	}
	else
	{
		m_prefetchedData.init();
	}
}

template <typename T, const int dmaGroup>
void hkSimplePrefetcher<T, dmaGroup>::waitForCompletion()
{
	HK_ON_SPU( hkSpuDmaManager::waitForDmaCompletion(dmaGroup) );
	HK_ON_DEBUG(m_waitDone = true);
}

template <typename T, const int dmaGroup>
HK_FORCE_INLINE const T& hkSimplePrefetcher<T, dmaGroup>::getData()
{
	HK_ASSERT2(0x238341f2, m_waitDone, "Trying to get data before calling waitForCompletion()");
	return m_prefetchedData;
}

template <typename T, int maxNumElems, const int dmaGroup>
hkArrayPrefetcher<T, maxNumElems, dmaGroup>::hkArrayPrefetcher()
{
	HK_ON_DEBUG(m_waitDone = true);
}

template <typename T, int maxNumElems, const int dmaGroup>
void hkArrayPrefetcher<T, maxNumElems, dmaGroup>::prefetch(const T* addr, int numElems)
{
	HK_ASSERT2(0x238341f1, m_waitDone, "Can't start prefetch without waiting for previous prefetch.");
	HK_ON_DEBUG(m_waitDone = false);

	if (addr)
	{
#ifdef HK_PLATFORM_SPU
		HK_CHECK_ALIGN16(addr);
		int transferSize = HK_NEXT_MULTIPLE_OF(16, sizeof(T) * numElems);
		HK_ASSERT(0x69c8d2c2, transferSize <= maxNumElems * sizeof(T) );
		hkSpuDmaManager::getFromMainMemory(m_prefetchedData, addr, transferSize, hkSpuDmaManager::READ_COPY, dmaGroup);
		HK_SPU_DMA_DEFER_FINAL_CHECKS_UNTIL_WAIT(addr, m_prefetchedData, transferSize);
#else
		hkString::memCpy(m_prefetchedData, addr, sizeof(T) * numElems);
#endif
	}
	else
	{
	}
}

template <typename T, int maxNumElems, const int dmaGroup>
void hkArrayPrefetcher<T, maxNumElems, dmaGroup>::waitForCompletion()
{
	HK_ON_SPU( hkSpuDmaManager::waitForDmaCompletion(dmaGroup) );
	HK_ON_DEBUG(m_waitDone = true);
}

template <typename T, int maxNumElems, const int dmaGroup>
HK_FORCE_INLINE const T* hkArrayPrefetcher<T, maxNumElems, dmaGroup>::getData()
{
	HK_ASSERT2(0x238341f2, m_waitDone, "Trying to get data before calling waitForCompletion()");
	return m_prefetchedData;
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
