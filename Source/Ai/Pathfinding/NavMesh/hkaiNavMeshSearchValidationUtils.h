/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_NAVMESH_SEARCH_VALIDATION_UTILS_H
#define HKAI_NAVMESH_SEARCH_VALIDATION_UTILS_H

#include <Ai/Pathfinding/NavMesh/Streaming/hkaiStreamingCollection.h>
#include <Ai/Pathfinding/Utilities/hkaiPathfindingUtil.h>

#if !defined (HK_PLATFORM_SPU) 

	/// Utilities to validate inputs of various nav mesh search queries
class hkaiNavMeshSearchValidationUtils
{
public:

		/// Validate the inputs to hkaiPathfindingUtil::findPath request
		/// \param[out] validGoalMask Bit mask that determines which of the N goals are valid
		/// \return False if the query cannot be processed due to invalid inputs
	static bool HK_CALL validateFindPathInput( const hkaiStreamingCollection& collection,
		const hkaiPathfindingUtil::FindPathInput& input, hkUint32& validGoalsMask );

		/// Validate the inputs to A* pathfinding command
		/// \param[out] validGoalMask Bit mask that determines which of the N goals are valid
		/// \return False if the query cannot be processed due to invalid inputs
	static bool HK_CALL validateNavMeshAStarData( const hkaiStreamingCollection& collection,
		hkVector4Parameter startPoint,
		hkaiPackedKey startFaceKey,
		int numGoalPoints,
		const hkVector4* goalPoints,
		int numGoalFaceKeys,
		const hkaiPackedKey* goalFaceKeys,
		const hkaiNavMeshPathSearchParameters& searchParameters,
		hkUint32& validGoalsMask );

};

#endif //!defined (HK_PLATFORM_SPU) 

#endif	// HKAI_NAVMESH_SEARCH_VALIDATION_UTILS_H

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
