/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_AABB_TREE_NAV_VOLUME_MEDIATOR_H
#define HKAI_AABB_TREE_NAV_VOLUME_MEDIATOR_H

#include <Ai/Pathfinding/Collide/NavVolume/hkaiNavVolumeMediator.h>

class hkcdStaticAabbTree;

	/// hkaiNavVolumeMediator based on an hkcdStaticAabbTree.
class hkaiAabbTreeNavVolumeMediator : public hkaiNavVolumeMediator
{
public:
	HK_DECLARE_REFLECTION();
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI );

	hkaiAabbTreeNavVolumeMediator();
	hkaiAabbTreeNavVolumeMediator( hkFinishLoadedObjectFlag f );
	virtual ~hkaiAabbTreeNavVolumeMediator();

	void build( const hkaiNavVolume* navVolume );

		/// Initializes the nav volume and tree to the provided pointers.
		/// The tree must have been build for the specific nav volume, otherwise queries will fail
	void setNavVolumeAndTree( const hkaiNavVolume* navVolume, hkcdStaticAabbTree* staticTree );

	virtual hkaiPackedKey getClosestPoint(const GetClosestPointInput& gcpInput, hkVector4& closestPointOut ) const HK_OVERRIDE;

	const hkcdStaticAabbTree* getStaticTree() const { return m_tree; }

private:

	hkRefPtr<const hkaiNavVolume> m_navVolume;
	hkRefPtr<hkcdStaticAabbTree> m_tree;

};

#endif // HKAI_AABB_TREE_NAV_VOLUME_MEDIATOR_H

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
