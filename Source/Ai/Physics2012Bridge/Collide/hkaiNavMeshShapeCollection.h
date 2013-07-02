/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_NAV_MESH_SHAPE_COLLECTION_H
#define HK_AI_NAV_MESH_SHAPE_COLLECTION_H

#include <Physics2012/Collide/Shape/Compound/Collection/hkpShapeCollection.h>
class hkaiNavMesh;

	/// An implementation of hkpShapeCollection that uses an hkaiNavMesh to determin the triangles.
	/// This is NOT intended to be an efficient implementation. It is primarily used by hkaiMoppNavMeshQueryMediator
	/// during construction.
class hkaiNavMeshShapeCollection : public hkpShapeCollection
{
public:

	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_NAVMESH );
	hkaiNavMeshShapeCollection( const hkaiNavMesh* navMesh );
	~hkaiNavMeshShapeCollection();

	//
	// hkpShapeCollection interface
	//
	virtual hkpShapeKey getFirstKey() const HK_OVERRIDE;
	virtual hkpShapeKey getNextKey( hkpShapeKey oldKey ) const HK_OVERRIDE;
	virtual const hkpShape* getChildShape( hkpShapeKey key, hkpShapeBuffer& buffer ) const HK_OVERRIDE;
	virtual int getNumChildShapes( ) const HK_OVERRIDE;
	virtual void getAabb( const hkTransform& localToWorld, hkReal tolerance, hkAabb& out ) const HK_OVERRIDE;

public:

	hkRefPtr<const hkaiNavMesh> m_navMesh;

};

#endif // HK_AI_NAV_MESH_SHAPE_COLLECTION_H

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
