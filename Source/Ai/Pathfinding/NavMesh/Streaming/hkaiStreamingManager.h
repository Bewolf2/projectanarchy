/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_NAVMESH_STREAMING_MANAGER_H
#define HKAI_NAVMESH_STREAMING_MANAGER_H

class hkaiNavVolumeInstance;
class hkaiDirectedGraphInstance;
class hkaiStreamingCollection;

/// Utilities for attaching/detaching edges at load/unload time.
class hkaiStreamingManager : public hkReferencedObject
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_NAVMESH );

		static void HK_CALL   loadGraphInstance( class hkaiDirectedGraphInstance* graphInstance, class hkaiStreamingCollection* collection );
		static void HK_CALL unloadGraphInstance( class hkaiDirectedGraphInstance* graphInstance, class hkaiStreamingCollection* collection );

		static void HK_CALL   loadVolumeInstance( hkaiNavVolumeInstance* volume, class hkaiStreamingCollection* collection );
		static void HK_CALL unloadVolumeInstance( hkaiNavVolumeInstance* volume, class hkaiStreamingCollection* collection );	
};

#include <Ai/Pathfinding/NavMesh/Streaming/hkaiStreamingManager.inl>

#endif // HKAI_NAVMESH_STREAMING_MANAGER_H

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
