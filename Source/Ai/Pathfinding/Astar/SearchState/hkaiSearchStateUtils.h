/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_ASTAR_SEARCHSTATE_UTILS_H
#define HKAI_ASTAR_SEARCHSTATE_UTILS_H

#include <Ai/Pathfinding/hkaiBaseTypes.h>

/// Utility methods for reversing the parent ordering in a search state, and gathering the list of visited states
class hkaiSearchStateUtils
{
public:

		/// Options for extracting path from the search state.
	enum GetPathOptions
	{
		GET_PATH_PRESERVE_ALL			=0,		// All state nodes correspond to path elements
		GET_PATH_IGNORE_START_AND_END	=1,		// Start and End state nodes are trimmed from path
	};

		/// Computes the path backwards from endKeyIn. Returns whether the array was big enough for all the keys.
	static hkBool32 HK_CALL getPath(
		const struct hkaiHashSearchStateBase& state, 
		hkaiPackedKey endKeyIn, 
		GetPathOptions options,
		hkArray<hkaiPackedKey>& statesOut,
		int maxCapacity);
};


#endif // HKAI_ASTAR_SEARCHSTATE_UTILS_H

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
