/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_TRAVERSAL_ANALYSIS_UTILS_H
#define HKAI_TRAVERSAL_ANALYSIS_UTILS_H


#include <Common/Base/Types/Geometry/Aabb/hkAabb.h>
#include <Ai/Pathfinding/NavMesh/hkaiNavMesh.h>
#include <Ai/Internal/SegmentCasting/hkaiNavMeshInstanceSegmentCaster.h>

struct hkaiTraversalAnalysisContext;
class hkaiIntervalPartition;

namespace hkaiTraversalAnalysisUtils
{
	/// Find the amount of vertical clearance over the plane connecting the
	/// start and end edges.
	/// 
	/// The start and end edges must have been generated via edge-matched
	/// pairing and must have been made coplanar (see raiseToPlane()).
	/// 
	/// The clearance profile returned is relative to the leftMid-rightMid
	/// segment.
	void findClearanceProfile(
		hkVector4Parameter leftStart, hkVector4Parameter rightStart,
		hkVector4Parameter leftEnd, hkVector4Parameter rightEnd,
		hkVector4Parameter up,
		hkSimdRealParameter maxClearance,
		hkaiIntervalPartition & clearanceOut,
		hkaiTraversalAnalysisContext const& context);

	void filterUnwalkableLandingZones(
		hkVector4Parameter leftStart, hkVector4Parameter rightStart,
		hkVector4Parameter leftEnd, hkVector4Parameter rightEnd,
		hkaiNavMesh::FaceIndex startFace,
		hkaiNavMesh::FaceIndex endFace,
		hkaiIntervalPartition & clearZonesOut,
		hkaiTraversalAnalysisContext const& context);

	void filterUnwalkableLandingZones_aux(
		hkVector4Parameter left, hkVector4Parameter right,
		hkBool32 castForward,
		hkaiNavMesh::FaceIndex faceIndex,
		hkaiIntervalPartition & clearZonesOut,
		hkaiNavMeshInstanceSegmentCaster const& caster,
		hkaiTraversalAnalysisContext const& context);

	hkBool32 findOffsetDist(
		hkVector4Parameter left, hkVector4Parameter right, 
		hkVector4Parameter offsetDir, 
		hkSimdRealParameter maxOffset,
		hkSimdReal & distOut,
		hkaiTraversalAnalysisContext const& context);

	hkBool32 findOffsetDistExtruded(
		hkVector4Parameter left, hkVector4Parameter right, 
		hkVector4Parameter extrusion,
		hkVector4Parameter offsetDir, 
		hkSimdRealParameter maxOffset,
		hkSimdReal & distOut,
		hkaiTraversalAnalysisContext const& context);

	hkBool32 findRaiseHeight(
		hkVector4Parameter left, hkVector4Parameter right, 
		hkSimdReal & raiseHeightOut,
		hkaiTraversalAnalysisContext const& context);

	hkBool32 findExtrudedRaiseHeight(
		hkVector4Parameter left, hkVector4Parameter right, 
		hkVector4Parameter extrusion,
		hkSimdReal & raiseHeightOut,
		hkaiTraversalAnalysisContext const& context);

	/// Translate a line segment upwards until no point of it intersects
	/// walkable geometry, up to a distance given by 
	/// context.settings.m_raiseEdgeHeightLimit. Returns whether this was
	/// successful. If false, intersections existed even at the height limit.
	hkBool32 raiseEdgeAboveGeometry(
		hkVector4 & leftInOut, hkVector4 & rightInOut, 
		hkaiTraversalAnalysisContext const& context);

	/// Like raiseEdgeAboveGeometry(), but with an extrusion applied to the 
	/// cast.
	hkBool32 raiseExtrudedEdgeAboveGeometry(
		hkVector4 & leftInOut, hkVector4 & rightInOut, 
		hkVector4Parameter extrusion,
		hkaiTraversalAnalysisContext const& context);

	void setBackEdge(
		hkVector4 & leftInOut, hkVector4 & rightInOut, 
		hkVector4Parameter facePlane, 
		hkaiTraversalAnalysisContext const& context);

	/// Move edges away from a cliff in such a way that they lie in the same
	/// up-parallel plane, each is in the same up-perpendicular plane as the
	/// corresponding original, and all points are no closer to the cliff
	/// (horizontally) than the originals.
	/// @note "left" and "right" are to be interpreted relative to a viewer
	/// looking at the cliff face while standing at the bottom of the cliff.
	void resolveHangs(
		hkVector4& bottomLeftInOut, hkVector4& bottomRightInOut,
		hkVector4& topLeftInOut, hkVector4& topRightInOut,
		hkSimdRealParameter extraDistance,
		hkVector4Parameter up);

	/// Find intervals which are unoccluded for a climb-up or climb-down
	/// motion.
	void findOverLedgeReachability(
		hkVector4Parameter bottomLeft, hkVector4Parameter bottomRight,
		hkVector4Parameter topLeft, hkVector4Parameter topRight,
		hkaiTraversalAnalysisContext const& context,
		hkaiIntervalPartition & reachableAreasOut);

	/// Given four points, oriented counterclockwise around the up-vector, 
	/// moves zero or two of the points along the up-axis, such that the
	/// resulting four points are coplanar.
	void raiseToPlane(
		hkVector4 & p0, hkVector4 & p1, hkVector4 & p2, hkVector4 & p3,
		hkVector4Parameter up);

	void makeTEquation(hkVector4Parameter left, hkVector4Parameter right, hkVector4 & tEqnOut);
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
