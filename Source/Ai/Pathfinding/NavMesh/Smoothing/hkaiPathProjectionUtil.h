/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_PATH_PROJECTION_UTIL_H
#define HK_AI_PATH_PROJECTION_UTIL_H

#include <Ai/Pathfinding/NavMesh/hkaiNavMeshInstance.h>
#include <Ai/Pathfinding/Utilities/hkaiPathfindingUtil.h>

class hkaiPath;
class hkaiGeneralAccessor;

	/// Utility for projection a smoothed path onto the nav mesh.
class hkaiPathProjectionUtil
{
	//+hk.MemoryTracker(ignore=True)
public:

	/// Project the pathfinding results onto the nav mesh. This is useful for displaying the path or as input to a steering system.
	static void HK_CALL getSimplifiedPathPoints(		const hkArray<hkaiPath::PathPoint>& pathIn, 
														class hkaiPath* pathOut,
														hkReal lengthTolerance = .1f,
														hkReal cosAngleTol = .998f,
														hkReal cosNormalAngleTol = .998f
													);

	static bool HK_CALL calcBarycentricCoordinates(hkVector4Parameter pos, hkVector4Parameter t0, 
		hkVector4Parameter t1, hkVector4Parameter t2, hkVector4& result);

	static bool HK_CALL calcBarycentricHeightInterpolation(const hkaiGeneralAccessor& accessor, 
		hkVector4Parameter up, hkaiPackedKey faceKey, hkVector4& pointInOut);
	
	static bool HK_CALL calcBarycentricHeightInterpolation(const hkArrayBase<hkVector4>& facePoints,	hkVector4Parameter up, hkVector4& pointInOut);


#ifndef HK_PLATFORM_SPU
	static bool HK_CALL projectSegmentOntoFaceAndClipEdgesWorldUp(const hkArrayBase<hkVector4>& faceVerts, hkaiNavMesh::FaceIndex faceIndex, hkVector4Parameter A, hkVector4Parameter B, hkVector4Parameter up, hkVector4& Aout, hkVector4& Bout);
#endif
};

#endif // HK_AI_PATH_PROJECTION_UTIL_H

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
