/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_GEOMETRY_SEGMENT_CASTER_H
#define HKAI_GEOMETRY_SEGMENT_CASTER_H

#include <Ai/Internal/SegmentCasting/hkaiIntervalPartition.h>

class hkaiEdgeGeometry;
class hkcdDynamicAabbTree;
class hkaiFaceEdges;
struct hkGeometry;
class hkBitField;

class hkaiGeometrySegmentCaster : public hkReferencedObject
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);
	
	struct AccelerationData;

	static AccelerationData* HK_CALL buildAccelerationData(
		hkGeometry const& geometry,
		hkBitField const* mask,
		hkBool supportExtrusion);

	hkaiGeometrySegmentCaster();
	~hkaiGeometrySegmentCaster();

	void addGeometry(hkGeometry const& geometry, AccelerationData const* accelerationData);

	void castSegment(
		hkVector4Parameter leftStart, 
		hkVector4Parameter rightStart,
		hkVector4Parameter displacement,
		hkaiIntervalPartition & partition) const;

	void castSegmentExtruded(
		hkVector4Parameter leftStart, 
		hkVector4Parameter rightStart,
		hkVector4Parameter displacement,
		hkVector4Parameter extrusion,
		hkaiIntervalPartition & partition) const;

	void castSegmentExtrudedEdges(
		hkVector4Parameter leftStart, 
		hkVector4Parameter rightStart,
		hkVector4Parameter displacement,
		hkVector4Parameter extrusion,
		hkaiIntervalPartition & partition) const;

	void turnSegment(
		hkVector4Parameter leftStart, 
		hkVector4Parameter rightStart,
		hkVector4Parameter leftEnd, 
		hkVector4Parameter rightEnd,
		hkaiIntervalPartition & partition) const;

	void turnSegmentExtruded(
		hkVector4Parameter leftStart, 
		hkVector4Parameter rightStart,
		hkVector4Parameter leftEnd, 
		hkVector4Parameter rightEnd,
		hkVector4Parameter extrusion,
		hkaiIntervalPartition & partition) const;

	void findExtremalProfile(
		hkArrayBase<hkVector4> const& clippingPlanes, 
		hkVector4Parameter left, hkVector4Parameter right, 
		hkVector4Parameter extremalVector, 
		hkVector4Parameter uVector, 
		hkVector4Parameter vVector, 
		hkaiIntervalPartition & uPartition, 
		hkaiIntervalPartition & vPartition);


	/// Unique canonical representation of an edge in the geometry; m_a is 
	/// constrained to be less than m_b.
	struct ExtrudedEdge
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI, ExtrudedEdge);

		ExtrudedEdge() { }
		ExtrudedEdge(int a, int b) : m_a(hkMath::min2(a,b)), m_b(hkMath::max2(a,b)) { }
		int m_a, m_b;
	};

	struct AccelerationData : public hkReferencedObject
	{
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);

		/// BVT for all faces in the geometry. Data is the triangle index.
		hkRefPtr<hkcdDynamicAabbTree> m_faceTree; 

		/// Array of unique (under vertex flipping) edges in the geometry.
		hkArray<ExtrudedEdge> m_edges;

		/// BVT for all edges in the geometry. Data is index into m_edges.
		hkRefPtr<hkcdDynamicAabbTree> m_edgeTree;

		hkUint32 getSize() const;
	};

	enum HitType
	{
		HITTYPE_FACE = 0,
		HITTYPE_TRANSLATED_FACE = 1,
		HITTYPE_EXTRUDED_EDGE = 2
	};

	static hkUint32 HK_CALL packData(HitType hitType, hkUint32 geometryIndex, hkUint32 nodeIndex);
	static void HK_CALL unpackData(hkUint32 data, HitType & hitType, hkUint32 & geometryIndex, hkUint32 & nodeIndex);

private:
	struct Geometry
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI, Geometry);

		hkGeometry const* m_geometry;
		hkRefPtr<const AccelerationData> m_accelerationData;
	};

	hkArray<Geometry> m_geometries;
};

template<>
struct hkMapOperations<hkaiGeometrySegmentCaster::ExtrudedEdge>;

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
