/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_PAIRED_EDGE_FINDER_H
#define HKAI_PAIRED_EDGE_FINDER_H

#include <Ai/Pathfinding/NavMesh/hkaiNavMesh.h>
#include <Ai/Internal/SegmentCasting/hkaiNavMeshInstanceSegmentCaster.h>
#include <Common/Base/Container/Set/hkSet.h>

class hkcdDynamicAabbTree;
class hkAabb;
class hkaiNavMeshInstance;

class hkaiPairedEdgeFinder : public hkReferencedObject
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL);

	struct AccelerationData : public hkReferencedObject
	{
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL);

		struct BoundaryEdge
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL, BoundaryEdge);

			hkaiNavMesh::FaceIndex m_faceIndex;
			hkaiNavMesh::EdgeIndex m_edgeIndex;
		};
		hkArray<BoundaryEdge> m_boundaryEdges;

		hkRefPtr<hkcdDynamicAabbTree> m_boundaryEdgeTree; // key is index into m_boundaryEdges

		hkRefPtr<const hkaiNavMeshInstanceSegmentCaster::AccelerationData> m_segmentCasterAccelerationData;

		hkUint32 getSize() const;
	};

	static AccelerationData* buildAccelerationData(
		hkaiNavMeshInstance const* navMeshInstance,
		hkaiNavMeshInstanceSegmentCaster::AccelerationData const* segmentCasterAccelerationData);

	hkaiPairedEdgeFinder(
		hkaiNavMeshInstance const* fromNavMeshInstance, 
		AccelerationData const* fromAccelerationData,
		hkaiNavMeshInstance const* toNavMeshInstance, 
		AccelerationData const* toAccelerationData);

	~hkaiPairedEdgeFinder();

	struct PairedEdgeSettings
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL, PairedEdgeSettings);

		PairedEdgeSettings();

		hkVector4 m_up;
		hkReal m_minLength;
		hkReal m_minHeightDelta;
		hkReal m_maxHeightDelta;
		hkReal m_minHorizontalDelta;
		hkReal m_maxHorizontalDelta;
		hkReal m_cosMaxPlanarAngle;
		hkReal m_sinMaxPlanarAngle;
		hkReal m_cosMaxDeltaSlopeAngle;
		hkBool m_matchAngles;
		hkBool m_findInternalStarts;
		hkBool m_findInternalEnds;
	};

	struct EdgePair
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL, EdgePair);

		hkVector4 m_startRight;
		hkVector4 m_startLeft;
		hkVector4 m_endRight;
		hkVector4 m_endLeft;
		hkaiNavMesh::FaceIndex m_startFace; // in the fromNavMeshInstance
		hkaiNavMesh::EdgeIndex m_startEdge; // in the fromNavMeshInstance
		hkaiNavMesh::EdgeIndex m_endFace; // in the toNavMeshInstance
		hkaiNavMesh::EdgeIndex m_endEdge; // in the toNavMeshInstance
	};

	void findEdgePairs(
		PairedEdgeSettings const& settings, 
		hkArray<EdgePair>::Temp & pairsOut) const;


private:
	hkRefPtr<const hkaiNavMeshInstance> m_fromNavMeshInstance;
	hkRefPtr<const AccelerationData> m_fromAccelerationData;
	hkRefPtr<const hkaiNavMeshInstance> m_toNavMeshInstance;
	hkRefPtr<const AccelerationData> m_toAccelerationData;

	void findExternalPairs(
		PairedEdgeSettings const& settings, 
		hkArray<EdgePair>::Temp & pairsOut) const;

	void findInternalStarts(
		PairedEdgeSettings const& settings, 
		hkArray<EdgePair>::Temp &pairsOut ) const;

	void findInternalEnds(
		PairedEdgeSettings const& settings, 
		hkArray<EdgePair>::Temp &pairsOut ) const;

	static void HK_CALL buildPairedEdgeAabb(
		hkVector4Parameter left, 
		hkVector4Parameter right, 
		PairedEdgeSettings const& settings, 
		hkAabb & aabbOut);

	static hkBool32 HK_CALL filterPairedEdge(
		hkVector4Parameter origStartLeft,
		hkVector4Parameter origStartRight,
		hkVector4Parameter origEndLeft,
		hkVector4Parameter origEndRight,
		PairedEdgeSettings const& settings, 
		hkVector4 & filteredStartLeft,
		hkVector4 & filteredStartRight,
		hkVector4 & filteredEndLeft,
		hkVector4 & filteredEndRight);

	static hkBool32 HK_CALL edgeAnglesMatch(
		hkVector4Parameter startLeft, 
		hkVector4Parameter startRight, 
		hkVector4Parameter endLeft, 
		hkVector4Parameter endRight, 
		hkReal cosMaxHorizontalAngle);

	static hkBool32 HK_CALL matchEdgeEnds(
		hkVector4 & startLeftInOut, 
		hkVector4 & startRightInOut, 
		hkVector4 &endLeftInOut, 
		hkVector4 &endRightInOut, 
		hkVector4Parameter up);

	static hkBool32 HK_CALL slopeAnglesMatch(
		hkVector4Parameter startLeft, 
		hkVector4Parameter startRight, 
		hkVector4Parameter endLeft, 
		hkVector4Parameter endRight, 
		hkVector4Parameter up, hkReal 
		cosMaxDeltaAngle);

	static hkBool32 HK_CALL filterEdgeDistances(
		hkVector4 & startLeftInOut, 
		hkVector4 & startRightInOut, 
		hkVector4 & endLeftInOut, 
		hkVector4 & endRightInOut, 
		PairedEdgeSettings const &settings);

	/// Calculate the parameters for a nav mesh segment cast which can be used
	/// to find potential internal start sites corresponding to an end edge.
	static void HK_CALL calcInternalStartFaceCast(
		PairedEdgeSettings const& settings, 
		hkVector4Parameter toLeft, hkVector4Parameter toRight, hkVector4Parameter fwdDir, 
		hkVector4 & startLeftOut, hkVector4 & startRightOut, hkVector4 & displacementOut);

	/// Calculate the parameters for a nav mesh segment cast which can be used
	/// to find potential internal end sites corresponding to a start edge.
	static void HK_CALL calcInternalEndFaceCast(
		PairedEdgeSettings const& settings, 
		hkVector4Parameter fromLeft, hkVector4Parameter fromRight, hkVector4Parameter fwdDir, 
		hkVector4 & startLeftOut, hkVector4 & startRightOut, hkVector4 & displacementOut);

	class PairedEdgeQuery;
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
