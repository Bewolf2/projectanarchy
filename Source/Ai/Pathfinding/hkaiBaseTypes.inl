/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


//
// General Access
//
inline hkaiRuntimeIndex HK_CALL hkaiGetRuntimeIdFromPacked( hkaiPackedKey key )
{
	HK_ASSERT(0x583f16bc, key != HKAI_INVALID_PACKED_KEY );
	int sectionId = (int) (key >> HKAI_NUM_BITS_FOR_INDEX);
	return sectionId;
}

inline int HK_CALL hkaiGetIndexFromPacked( hkaiPackedKey key )
{
	HK_ASSERT(0x583f16bc, key != HKAI_INVALID_PACKED_KEY );

	const hkUint32 indexMask = (1<<HKAI_NUM_BITS_FOR_INDEX) - 1;
	int idx = (int) (key & indexMask);
	return idx;
}

inline hkaiPackedKey HK_CALL hkaiGetPackedKey( hkaiRuntimeIndex sectionId, int idx )
{
	HK_ASSERT(0x59640c0c, sectionId >= 0 && sectionId < (int) HKAI_MAX_NUM_SECTIONS);
	HK_ASSERT(0x59640c0c, idx >= 0 && idx < (int) HKAI_MAX_NUM_EDGES);
	hkaiPackedKey packed = ( (hkUint32) sectionId) << HKAI_NUM_BITS_FOR_INDEX | ((hkUint32) idx);
	return packed;
}

inline hkaiPackedKey HK_CALL hkaiGetOppositePackedKey( hkUint8 flags, hkaiRuntimeIndex thisRuntimeId, hkaiPackedKey storedKey)
{
	HK_ASSERT(0x59640c0c, thisRuntimeId >= 0 && thisRuntimeId < (int) HKAI_MAX_NUM_SECTIONS);
	const hkUint32 indexMask = (1<<HKAI_NUM_BITS_FOR_INDEX) - 1;
	const hkUint32 sectionBits = ( flags & HKAI_EDGE_EXTERNAL_OPPOSITE_FLAG ) ? (storedKey & ~indexMask) : (thisRuntimeId << HKAI_NUM_BITS_FOR_INDEX);
	hkaiPackedKey packed = sectionBits | (storedKey & indexMask);
	return (storedKey == HKAI_INVALID_PACKED_KEY) ? HKAI_INVALID_PACKED_KEY : packed;
}

inline hkaiRuntimeIndex HK_CALL hkaiGetOppositeRuntimeIndex( hkUint8 flags, hkaiRuntimeIndex thisRuntimeId, hkaiPackedKey storedKey)
{
	HK_ASSERT(0x59640c0c, thisRuntimeId >= 0 && thisRuntimeId < (int) HKAI_MAX_NUM_SECTIONS);
	const hkaiRuntimeIndex idx = ( flags & HKAI_EDGE_EXTERNAL_OPPOSITE_FLAG ) ? ((int) (storedKey >> HKAI_NUM_BITS_FOR_INDEX)) : thisRuntimeId ;
	return (storedKey == HKAI_INVALID_PACKED_KEY) ? HKAI_INVALID_RUNTIME_INDEX : idx;
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
