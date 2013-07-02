/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_BASE_TYPES
#define HKAI_BASE_TYPES

typedef hkUint32 hkaiPackedKey;
typedef hkUint32 hkaiSectionUid;
typedef int hkaiRuntimeIndex;
typedef hkUint32 hkaiTreeHandle;

enum hkaiIndices
{ 
	HKAI_NUM_BITS_FOR_SECTION = 10,
	HKAI_NUM_BITS_FOR_INDEX = 22,

	HKAI_MAX_NUM_SECTIONS = 1<<HKAI_NUM_BITS_FOR_SECTION,
	HKAI_MAX_NUM_EDGES = 1<<HKAI_NUM_BITS_FOR_INDEX,
	HKAI_MAX_NUM_FACES = 1<<HKAI_NUM_BITS_FOR_INDEX,

	HKAI_INVALID_RUNTIME_INDEX = -1,

	// Special section ID to mark starting pseudo-nodes in the search graphs.
	HKAI_START_NODE_SECTION_ID = ((1<<HKAI_NUM_BITS_FOR_SECTION) - 2),
	// Special section ID to mark goal pseudo-nodes in the search graphs.
	HKAI_GOAL_NODE_SECTION_ID = ((1<<HKAI_NUM_BITS_FOR_SECTION) - 3),

	// Same as hkaiNavMesh::EDGE_EXTERNAL_OPPOSITE
	HKAI_EDGE_EXTERNAL_OPPOSITE_FLAG = 64,
};

#define HKAI_INVALID_SECTION_UID (0xFFFFFFFFu)
#define HKAI_INVALID_PACKED_KEY (0xFFFFFFFFu)	


HK_FORCE_INLINE hkaiRuntimeIndex HK_CALL hkaiGetRuntimeIdFromPacked(hkaiPackedKey key);
HK_FORCE_INLINE int HK_CALL hkaiGetIndexFromPacked(hkaiPackedKey key);
HK_FORCE_INLINE hkaiPackedKey HK_CALL hkaiGetPackedKey(hkaiRuntimeIndex sectionId, int index);

HK_FORCE_INLINE hkaiPackedKey HK_CALL hkaiGetOppositePackedKey( hkUint8 flags, hkaiRuntimeIndex thisRuntimeId, hkaiPackedKey storedKey);
HK_FORCE_INLINE hkaiRuntimeIndex HK_CALL hkaiGetOppositeRuntimeIndex( hkUint8 flags, hkaiRuntimeIndex thisRuntimeId, hkaiPackedKey storedKey);

#include <Ai/Pathfinding/hkaiBaseTypes.inl>

#endif

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
