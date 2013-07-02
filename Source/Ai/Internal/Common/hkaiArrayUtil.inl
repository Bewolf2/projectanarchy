/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

template< typename T, typename Allocator >
HK_FORCE_INLINE hkResult hkaiArrayUtil::tryCopy( hkArray<T, Allocator>& array, const hkArrayBase<T>& other )
{
	const hkResult res = array.reserveExactly( other.getSize() );
	if (res == HK_SUCCESS)
	{
		array = other;
	}
	return res;
}

// This is similar to hkArray::reserve() except that, if the memory allocation for
// the new array fails, the elements of the old array will be destructed correctly
template< typename T, typename Allocator >
hkResult hkaiArrayUtil::reserveForNonPodType( hkArray<T, Allocator>& array, int size )
{
	const int capacity = array.getCapacity();
	if (capacity < size)
	{
		int cap2 = 2 * capacity;
		int newSize = (size < cap2) ? cap2 : size;

		
		{
			hkMemoryAllocator& mem = Allocator().get(&array);
			const int numElem = newSize;
			const int sizeElem = sizeof(T);

			HK_ASSERT2(0x3b67c014, numElem >= 0, "Number of elements must not be negative");
			HK_ASSERT2(0x243bf8d1, sizeElem >= 0, "The size of an element must not be negative");

			typedef hkArray<char> hkAnyArray;
			hkAnyArray* self = reinterpret_cast< hkAnyArray* >(&array);
			HK_ASSERT(0x673429dd, numElem >= self->getCapacity() );

			int reqNumBytes = numElem * sizeElem;
			if( (self->m_capacityAndFlags & hkAnyArray::DONT_DEALLOCATE_FLAG) == 0)
			{
				
				{
					const int oldNumBytes = self->getCapacity()*sizeElem;

					void* pnew = mem.bufAlloc(reqNumBytes);
					if ( pnew )
					{
						hkMemUtil::memCpy(pnew, self->m_data, hkMath::min2(oldNumBytes,reqNumBytes));
					}
					else
					{
						array.clearAndDeallocate();
					}
					mem.bufFree(self->m_data, oldNumBytes);
					self->m_data = static_cast<char*>( pnew );
				}
			}
			else
			{
				char* p = static_cast<char*>(mem.bufAlloc(reqNumBytes));
				if(p)
				{
					hkMemUtil::memCpy(p, self->m_data, self->m_size*sizeElem);
				}
				self->m_data = p;
			}

			self->m_capacityAndFlags = reqNumBytes / sizeElem;

			return (self->m_data) ? HK_SUCCESS : HK_FAILURE;
		}
	}

	return HK_SUCCESS;
}


template< typename T, typename Allocator >
hkResult hkaiArrayUtil::reserveSmall( hkArray<T, Allocator>& array, int n, int softMaxCapacity )
{
	const int capacity = array.getCapacity();
	if( capacity < n)
	{
		
		int cap32 = (capacity < 1024) ? (2*capacity) : HK_NEXT_MULTIPLE_OF(16, 3*(capacity/2) );
		// if we're still under the soft max capacity, use that instead of multiplier
		if (capacity < softMaxCapacity)
		{
			cap32 = hkMath::min2( cap32, softMaxCapacity );
		}

		int newCap = (n < cap32) ? cap32 : n;
		return array.reserveExactly( newCap );
	}

	return HK_SUCCESS;
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
