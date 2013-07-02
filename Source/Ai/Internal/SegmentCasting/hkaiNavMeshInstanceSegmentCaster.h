/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_NAV_MESH_INSTANCE_SEGMENT_CASTER_H
#define HKAI_NAV_MESH_INSTANCE_SEGMENT_CASTER_H

#include <Ai/Pathfinding/NavMesh/hkaiNavMeshInstance.h>
#include <Ai/Internal/SegmentCasting/hkaiIntervalPartition.h>

class hkcdDynamicAabbTree;

class hkaiNavMeshInstanceSegmentCaster : public hkReferencedObject
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH);

	struct AccelerationData : public hkReferencedObject
	{
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH);

		hkRefPtr<hkcdDynamicAabbTree> m_tree; // key is face index
		hkUint32 getSize() const;
	};

	static AccelerationData* HK_CALL buildAccelerationData(
		hkaiNavMeshInstance const& navMeshInstance);

	hkaiNavMeshInstanceSegmentCaster(hkaiNavMeshInstance const& navMeshInstance, AccelerationData const* accelerationData);
	~hkaiNavMeshInstanceSegmentCaster();

	void castSegment(
		hkVector4Parameter leftStart, 
		hkVector4Parameter rightStart,
		hkVector4Parameter displacement,
		hkaiIntervalPartition & partition);


	void castSegmentAlongSurface(
		hkVector4Parameter leftStart,
		hkVector4Parameter rightStart,
		hkVector4Parameter displacement,
		hkVector4Parameter up,
		hkaiNavMesh::FaceIndex startingFace,
		hkaiIntervalPartition & partition) const;

private:
	template<bool onStartEdge>
	void castSegmentAlongSurface_aux(
		hkVector4Parameter uEquation, 
		hkVector4Parameter vEquation, 
		hkaiNavMesh::FaceIndex faceIndex, 
		hkReal minURange, hkReal maxURange, 
		hkaiIntervalPartition & partition) const;

	hkaiNavMeshInstance const& m_navMeshInstance;

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
