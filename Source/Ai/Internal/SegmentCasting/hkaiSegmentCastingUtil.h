/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_JUMP_DETECTION_UTIL_H
#define HKAI_JUMP_DETECTION_UTIL_H

#include <Ai/Internal/SegmentCasting/hkaiIntervalPartition.h>
#include <Ai/Internal/Boolean/hkaiEdgeGeometry.h>
#include <Ai/Pathfinding/NavMesh/hkaiNavMesh.h>

class hkaiNavMeshInstance;

namespace hkaiSegmentCastingUtil
{
	void HK_CALL makeEdgeToWorldMatrix(
		hkVector4Parameter edgeLeft,
		hkVector4Parameter edgeRight,
		hkVector4Parameter up,
		
		hkMatrix4 & result,
		hkSimdReal & edgeLength);


	hkBool32 HK_CALL intersectSweptSegmentFace(
		hkVector4Parameter leftStartIn, 
		hkVector4Parameter rightStartIn, 
		hkVector4Parameter displacement, 

		hkaiEdgeGeometry const& edgeGeometry,
		hkaiFaceEdges const& faceEdges,
		int faceIdx,

		hkUint32 data,

		hkaiIntervalPartition & partition);

	hkBool32 HK_CALL intersectSweptSegmentExtrudedEdge(
		hkVector4Parameter leftStartIn, 
		hkVector4Parameter rightStartIn, 
		hkVector4Parameter displacement, 

		hkVector4Parameter vertexA,
		hkVector4Parameter vertexB,
		hkVector4Parameter extrusion,

		hkaiIntervalPartition & partition);

	hkBool32 HK_CALL intersectSweptSegmentFace(
		hkVector4Parameter leftStartIn, 
		hkVector4Parameter rightStartIn, 
		hkVector4Parameter displacement, 

		hkaiNavMesh const& navMesh,
		hkaiNavMesh::FaceIndex faceIndex,

		hkUint32 data,

		hkaiIntervalPartition & partition);

	hkBool32 HK_CALL intersectSweptSegmentFace(
		hkVector4Parameter leftStartIn, 
		hkVector4Parameter rightStartIn, 
		hkVector4Parameter displacement, 

		hkaiNavMeshInstance const& navMesh,
		hkaiNavMesh::FaceIndex faceIndex,

		hkUint32 data,

		hkaiIntervalPartition & partition);

	hkBool32 HK_CALL intersectSweptSegmentFace(
		hkVector4Parameter leftStartIn, 
		hkVector4Parameter rightStartIn, 
		hkVector4Parameter displacement, 

		hkGeometry const& geometry,
		int triangleIndex,

		hkUint32 data,

		hkaiIntervalPartition & partition);

	hkBool32 HK_CALL intersectSweptSegmentExtrudedEdge(
		hkVector4Parameter leftStartIn, 
		hkVector4Parameter rightStartIn, 
		hkVector4Parameter displacement, 

		hkVector4Parameter vertexA,
		hkVector4Parameter vertexB,
		hkVector4Parameter extrusion,

		hkUint32 data,

		hkaiIntervalPartition & partition);

	hkBool32 HK_CALL intersectSweptSegmentTriangle(
		hkVector4Parameter leftStart,
		hkVector4Parameter rightStart,
		hkVector4Parameter displacement,

		hkVector4Parameter vA,
		hkVector4Parameter vB,
		hkVector4Parameter vC,

		hkUint32 data,

		hkaiIntervalPartition::Interval & result);

	hkBool32 HK_CALL intersectTurnedSegmentFace(
		hkVector4Parameter leftStartIn, 
		hkVector4Parameter rightStartIn, 
		hkVector4Parameter leftEndIn,
		hkVector4Parameter rightEndIn,

		hkaiEdgeGeometry const& edgeGeometry,
		hkaiFaceEdges const& faceEdges,
		int faceIdx,

		hkUint32 data,

		hkaiIntervalPartition & partition);

	hkBool32 HK_CALL intersectTurnedSegmentFace(
		hkVector4Parameter leftStartIn, 
		hkVector4Parameter rightStartIn, 
		hkVector4Parameter leftEndIn,
		hkVector4Parameter rightEndIn,

		hkaiNavMesh const& navMesh,
		hkaiNavMesh::FaceIndex faceIndex,

		hkUint32 data,

		hkaiIntervalPartition & partition);

	hkBool32 HK_CALL intersectTurnedSegmentFace(
		hkVector4Parameter leftStartIn, 
		hkVector4Parameter rightStartIn, 
		hkVector4Parameter leftEndIn,
		hkVector4Parameter rightEndIn,

		hkGeometry const& geometry,
		int triangleIndex,

		hkUint32 data,

		hkaiIntervalPartition & partition);

	hkBool32 HK_CALL intersectTurnedSegmentExtrudedEdge(
		hkVector4Parameter leftStartIn, 
		hkVector4Parameter rightStartIn, 
		hkVector4Parameter leftEndIn,
		hkVector4Parameter rightEndIn,

		hkVector4Parameter vertexA,
		hkVector4Parameter vertexB,
		hkVector4Parameter extrusion,

		hkUint32 data,

		hkaiIntervalPartition & partition);


	hkBool32 HK_CALL intersectTurnedSegmentTriangle(
		hkVector4Parameter leftStartIn, 
		hkVector4Parameter rightStartIn, 
		hkVector4Parameter leftEndIn, 
		hkVector4Parameter rightEndIn,

		hkVector4Parameter vA,
		hkVector4Parameter vB,
		hkVector4Parameter vC,

		hkUint32 data,

		hkaiIntervalPartition & partition);
}

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
