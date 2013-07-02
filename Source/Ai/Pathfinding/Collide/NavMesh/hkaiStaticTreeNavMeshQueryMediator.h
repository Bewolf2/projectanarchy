/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_STATIC_TREE_NAV_MESH_MEDIATOR_H
#define HK_AI_STATIC_TREE_NAV_MESH_MEDIATOR_H

#include <Common/Base/Types/Geometry/Aabb/hkAabb.h>
#include <Ai/Pathfinding/Collide/NavMesh/hkaiNavMeshQueryMediator.h>
#include <Geometry/Internal/DataStructures/StaticTree/hkcdStaticAabbTree.h>

class hkaiNavMesh;

/// An implementation of hkaiNavMeshQueryMediator that bounds the nav mesh with an AABB tree.
class hkaiStaticTreeNavMeshQueryMediator : public hkaiNavMeshQueryMediator
{
	public:
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);
		hkaiStaticTreeNavMeshQueryMediator( );

		hkaiStaticTreeNavMeshQueryMediator( hkFinishLoadedObjectFlag f);

		~hkaiStaticTreeNavMeshQueryMediator();

			/// Creates the AABB tree from the nav mesh. This must be called before any query functions.
		void				setNavMesh( const hkaiNavMesh* navMesh );

			/// Initializes the mesh and tree to the provided pointers.
			/// The tree must have been build for the specific mesh, otherwise queries will fail
		void				setNavMeshAndTree( const hkaiNavMesh* navMesh, hkcdStaticAabbTree* staticTree );

		virtual hkaiPackedKey	getClosestPoint( const GetClosestPointInput& input, hkVector4& closestPointOut ) const HK_OVERRIDE;

		virtual bool		castRay( const RaycastInput& input, HitDetails& hitOut ) const HK_OVERRIDE;

		virtual void		queryAabb( const AabbQueryInput& input, hkArray<hkaiPackedKey>::Temp& hits ) const HK_OVERRIDE;

		int					getMemoryFootPrint() const;

		const hkcdStaticAabbTree* getStaticTree() const { return m_tree; }
		
	protected:

		hkRefPtr<hkcdStaticAabbTree> m_tree;
		hkRefPtr<const hkaiNavMesh>	m_navMesh;
};

#endif // HK_AI_STATIC_TREE_NAV_MESH_MEDIATOR_H

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
