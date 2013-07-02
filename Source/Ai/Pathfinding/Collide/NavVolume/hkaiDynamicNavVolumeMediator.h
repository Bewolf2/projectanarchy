/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HK_DYNAMIC_NAV_VOLUME_MEDIATOR_H
#define HK_DYNAMIC_NAV_VOLUME_MEDIATOR_H

#include <Ai/Pathfinding/Collide/NavVolume/hkaiNavVolumeMediator.h>

class hkaiStreamingCollection;
class hkcdDynamicAabbTree;

/// An implementation of hkaiNavVolumeMediator for streaming nav volumes.
class hkaiDynamicNavVolumeMediator : public hkaiNavVolumeMediator
{
	//+version(0)
	public:
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);

		hkaiDynamicNavVolumeMediator( const hkaiStreamingCollection* collection, const hkcdDynamicAabbTree* tree );
		hkaiDynamicNavVolumeMediator( hkFinishLoadedObjectFlag f );
		~hkaiDynamicNavVolumeMediator();

		virtual hkaiPackedKey getClosestPoint(const GetClosestPointInput& gcpInput, hkVector4& closestPointOut ) const;

		/// Pointer to the hkaiStreamingCollection
		hkRefPtr<const class hkaiStreamingCollection> m_collection; 

		hkRefPtr<const hkcdDynamicAabbTree> m_aabbTree;
};

#endif // HK_DYNAMIC_NAV_VOLUME_MEDIATOR_H

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
