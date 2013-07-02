/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_ARRAY_H
#define HKAI_ARRAY_H

#include <Common/Base/Container/Array/hkArray.h>

	/// Helper utilities manipulating arrays and handling out-of-memory errors.
class hkaiArrayUtil 
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI, hkaiArrayUtil);

		template< typename T>
		static hkBool32 isOk( const hkArrayBase<T>& array ){ return !(array.getCapacity() > 0 && array.begin() == HK_NULL); }
	

		// Note that this may be unsafe for non-POD types, e.g. if the array being copied contains a hkArray
		// member, the allocations that hkArray does in order to copy itself will not be checked for failure
		template< typename T, typename Allocator >
		static HK_FORCE_INLINE hkResult tryCopy( hkArray<T, Allocator>& dstArray, const hkArrayBase<T>& srcArray );


		template< typename T, typename Allocator >
		static hkResult reserveForNonPodType( hkArray<T, Allocator>& array, int size );

		// Expands the capacity by the capacityIncrement
		// Grows by a smaller factor than hkArray::reserve(), i.e.
		// capacity = factor * capacity.
		// If the current capacity is less than softMaxCapacity, then
		// capacity = min(softMaxCapacity, factor*capacity) 
		// will be used instead. This allows for worst-case estimates to provide better bounds than exponential growth.
		template< typename T, typename Allocator >
		static hkResult reserveSmall( hkArray<T, Allocator>& array, int capacity, int softMaxCapacity = -1 );
};

#include <Ai/Internal/Common/hkaiArrayUtil.inl>

#endif // HKAI_ARRAY_H

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
