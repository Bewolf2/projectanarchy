/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_VOLUME_PATH_SMOOTING_H
#define HKAI_VOLUME_PATH_SMOOTING_H

#include <Ai/Pathfinding/NavMesh/Streaming/hkaiStreamingCollection.h>
#include <Ai/Pathfinding/Character/hkaiPath.h>

	/// Smooths the A* path through an hkaiNavVolume.
class hkaiVolumePathSmoothingUtil
{
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_ASTAR, hkaiVolumePathSmoothingUtil );

	static hkBool32 HK_CALL findSmoothedPath(	const hkaiStreamingCollection::InstanceInfo* sectionInfo, 
		const hkArray<hkaiPackedKey>& visitedCells, 
		hkaiPackedKey startCellKey, 
		hkVector4Parameter startPoint, 
		hkVector4Parameter endPoint, 
		hkArray<hkaiPath::PathPoint>* pathArrayOut, 
		hkaiPath::PathPoint* pathPtrOut, 
		int maxPathLength
		);
};


#endif // HKAI_VOLUME_PATH_SMOOTING_H

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
