/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HK_CUT_NAV_MESH_MEDIATOR_H
#define HK_CUT_NAV_MESH_MEDIATOR_H

#include <Ai/Pathfinding/Collide/NavMesh/hkaiNavMeshQueryMediator.h>

extern const class hkClass hkaiDynamicNavMeshQueryMediatorClass;

class hkaiNavMeshCutter;
class hkaiStreamingCollection;
class hkcdDynamicAabbTree;

/// An implementation of hkaiNavMeshQueryMediator for dynamically modified nav meshes.
/// This mediator works efficiently by reusing the mediator form the original uncut nav mesh and the mapping information
/// maintained in the nav mesh cutter.
/// If there are multiple navmeshes loaded, it also uses the AABB tree maintained by the hkaiStreamingCollection to reduce the
/// number of meshes queried.
class hkaiDynamicNavMeshQueryMediator : public hkaiNavMeshQueryMediator
{
	//+version(4)
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);
		HK_DECLARE_REFLECTION();

		hkaiDynamicNavMeshQueryMediator( hkFinishLoadedObjectFlag f );
		hkaiDynamicNavMeshQueryMediator( const hkaiStreamingCollection* collection, const hkcdDynamicAabbTree* tree, const hkaiNavMeshCutter* cutter );
		~hkaiDynamicNavMeshQueryMediator();

		virtual hkaiPackedKey getClosestPoint( const GetClosestPointInput& input, hkVector4& closestPointOut ) const HK_OVERRIDE;
		virtual bool castRay( const RaycastInput& input, HitDetails& hitOut ) const HK_OVERRIDE;
		virtual void queryAabb( const AabbQueryInput& input, hkArray<hkaiPackedKey>::Temp& hits ) const HK_OVERRIDE;

		/// Pointer to the hkaiStreamingCollection
		hkRefPtr<const class hkaiStreamingCollection> m_collection; 

		hkRefPtr<const hkcdDynamicAabbTree> m_aabbTree; 

		/// Optional pointer to an hkaiNavMeshCutter. If specified, the cutter's collection must be the same as m_collection here.
		hkRefPtr<const hkaiNavMeshCutter> m_cutter;

			/// The AABBs of the cut faces are expanded by this radius when checking against the query AABB.
		hkReal m_cutAabbTolerance; //+default(.05f)
};

#endif // HK_CUT_NAV_MESH_MEDIATOR_H

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
