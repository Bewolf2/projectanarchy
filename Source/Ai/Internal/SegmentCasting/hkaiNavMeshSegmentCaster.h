/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_NAV_MESH_SEGMENT_CASTER_H
#define HKAI_NAV_MESH_SEGMENT_CASTER_H

#include <Ai/Pathfinding/NavMesh/hkaiNavMesh.h>
#include <Ai/Internal/SegmentCasting/hkaiIntervalPartition.h>

class hkcdDynamicAabbTree;

class hkaiNavMeshSegmentCaster : public hkReferencedObject
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH);

	struct AccelerationData : public hkReferencedObject
	{
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH);
		hkRefPtr<hkcdDynamicAabbTree> m_tree;
		hkUint32 getSize() const;
	};

	static AccelerationData* HK_CALL buildAccelerationData(
		hkaiNavMesh const& navMesh);

	hkaiNavMeshSegmentCaster(hkaiNavMesh const& navMesh, AccelerationData const* accelerationData);
	~hkaiNavMeshSegmentCaster();

	void castSegment(
		hkVector4Parameter leftStart, 
		hkVector4Parameter rightStart,
		hkVector4Parameter displacement,
		hkaiIntervalPartition & partition);

private:
	hkaiNavMesh const& m_navMesh;

	hkRefPtr<const AccelerationData> m_accelerationData;
};


#endif

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
