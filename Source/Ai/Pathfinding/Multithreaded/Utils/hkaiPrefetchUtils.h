/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_PREFETCH_UTILS_H
#define HKAI_PREFETCH_UTILS_H

/// Simple utility for prefetching (on SPU) one object at a time.
/// Class/struct T must have a default ::init function; this is used when trying to prefetch a NULL pointer
template <typename T, const int dmaGroup>
class hkSimplePrefetcher
{
	public:

		hkSimplePrefetcher();

			/// Fetches the object at addr.
		void prefetch(const T* addr);

			/// Waits for the data to be read. This should be called in between calls to prefetch and getData.
		void waitForCompletion();

			/// Returns a reference to the fetched object. Once the object has been used, prefetch can be called again to get the next object.
		const T& getData();

	protected:

		HK_ALIGN16( T m_prefetchedData );
		HK_ON_DEBUG( hkBool m_waitDone );
};

/// Simple utility for prefetching (on SPU) an array of objects at a time
template <typename T, int maxNumElems, const int dmaGroup>
class hkArrayPrefetcher
{
	public:

		hkArrayPrefetcher();

			/// Fetches the array at addr.
		void prefetch(const T* addr, int numElems);

			/// Waits for the data to be read. This should be called in between calls to prefetch and getData.
		void waitForCompletion();

			/// Returns the fetched array. Once the array has been used, prefetch can be called again to get the next array.
		const T* getData();

	protected:

		HK_ALIGN16( T m_prefetchedData[maxNumElems] );
		HK_ON_DEBUG( hkBool m_waitDone );
};

#include <Ai/Pathfinding/Multithreaded/Utils/hkaiPrefetchUtils.inl>

#endif // HKAI_PREFETCH_UTILS_H

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
