/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_EDGE_PATH_STEERING_UTIL_H
#define HKAI_EDGE_PATH_STEERING_UTIL_H

#include <Ai/Pathfinding/Character/LocalSteering/EdgePath/hkaiEdgePath.h>

class hkaiEdgePathSteeringUtil
{
public:
	enum CalcForwardVectorResult
	{
		RESULT_SUCCESS, ///< The function completed normally.
		RESULT_HIT_END, ///< Hit the last edge of an incomplete path; FV is an estimate only
		RESULT_POSITION_SECTION_UNLOADED, ///< The character's current section has been unloaded.
		RESULT_EDGE_SECTION_UNLOADED, ///< The section containing the first upcoming edge has been unloaded.
		RESULT_NO_EDGES ///< The edge path did not have any future edges to scan.
	};

	/// Determine the direction a character should move in to maximize their progress along
	/// an edge path.
	///
	/// @param edgePath the edge path the character is traversing.
	/// @param traversalState the character's current traversal state, which should be updated
	/// before calling this function.
	/// @param positionLocal the character's current position, expressed in the local space of
	/// the section they are in, or in global space if they are not in a section.
	/// @param positionSectionIndex the runtime index of the navmesh section the character is
	/// in (and in whose local space positionLocal is given), or HKAI_INVALID_RUNTIME_INDEX
	/// to indicate that the character is not in a navmesh section.
	/// @param streamingCollection a streaming collection for the navmesh.
	/// @param forwardVectorLocal filled with the optimal movement direction, expressed in the
	/// space of the navmesh section given by positionLocal.
	/// @param futureVectorLocal filled with an approximation of the character's optimal
	/// progress direction after rounding the upcoming corner.
	/// @param nextCornerLocal filled with the rounded corner which currently actively constrains 
	/// the character's direction of motion. The W coordinate is filled with the corner's effective
	/// radius, and is negative for right-hand turns.
	/// @param nextUserEdgePlane if at least one user edge is in view, a plane equation containing
	/// that user edge and the up-vector, and pointing towards the character. Otherwise, zero.
	/// @return an enumerant of CalcForwardVectorResult indicating the result of the operation.
	static CalcForwardVectorResult HK_CALL calcForwardVector(
		hkaiEdgePath const& edgePath, 
		hkaiEdgePath::TraversalState const& traversalState, 
		hkVector4 const& positionLocal, 
		hkaiRuntimeIndex positionSectionIndex,
		hkaiStreamingCollection const* streamingCollection,
		hkVector4 & forwardVectorLocal,
		hkVector4 & futureVectorLocal,
		hkVector4 & nextCornerLocal,
		hkVector4 & nextUserEdgePlane,
		int & nextEdgeIndex,
		bool & nextIsLeft);

	/// A variant of the above which does not return nextEdgeIndex or 
	/// nextIsLeft. 
	static CalcForwardVectorResult HK_CALL calcForwardVector(
		hkaiEdgePath const& edgePath, 
		hkaiEdgePath::TraversalState const& traversalState, 
		hkVector4 const& positionLocal, 
		hkaiRuntimeIndex positionSectionIndex,
		hkaiStreamingCollection const* streamingCollection,
		hkVector4 & forwardVectorLocal,
		hkVector4 & futureVectorLocal,
		hkVector4 & nextCornerLocal,
		hkVector4 & nextUserEdgePlane);

	/// Calculate the matrix which will transform the points of the targetEdge
	/// into the local space of the nav mesh instance at  positionSectionIndex,
	/// or into global space if positionSectionIndex is 
	/// HKAI_INVALID_RUNTIME_INDEX. This does not perform flattening.
	static CalcForwardVectorResult HK_CALL getRelativeTransform(
		hkaiRuntimeIndex positionSectionIndex, 
		hkaiEdgePath::Edge const& targetEdge, 
		hkaiStreamingCollection const* streamingCollection, 
		hkMatrix4 &relativeTransformOut);

	/// Lazily compute, store, and return following corner information for a 
	/// corner.
	static hkBool32 HK_CALL getFollowingCorner(
		hkaiEdgePath & edgePath,
		int startEdge,
		bool startIsLeft,
		hkaiStreamingCollection const* streamingCollection,
		int & followingEdge,
		bool & followingIsLeft);

	/// Calculate two point-circle tangents.
	/// 
	///
	/// @param p1 The point for the first tangent calculation.
	/// @param p2 The point for the second tangent calculation.
	/// @param c1 The circle for the first tangent calculation, with the 
	/// signed radius in W.
	/// @param c2 The circle for the second tangent calculation, with the
	/// signed radius in W.
	/// @param up The up-vector.
	/// @param[out] tangentDir1 The vector from p1 to the first tangent point,
	/// projected into the up-perpendicular plane.
	/// @param[out] tangentDir2 The vector from p1 to the second tangent point,
	/// projected into the up-perpendicular plane.
	/// @param[out] perpDir1 The left perpendicular of tangentDir1.
	/// @param[out] perpDir2 The left perpendicular of tangentDir2.
	/// @note If a circle radius is negative, the ray from p1 along the 
	/// tangent will wind CCW around the circle; if it is positive, the ray
	/// will wind CW around the circle.
	inline static void calcTangentDirs(
		hkVector4 const& p1, 
		hkVector4 const& p2, 
		hkVector4 const& c1, 
		hkVector4 const& c2, 
		hkVector4 const& up, 
		hkVector4 & tangentDir1, 
		hkVector4 & tangentDir2,
		hkVector4 & perpDir1,
		hkVector4 & perpDir2);

	inline static void calcMutualTangentPoints(
		hkVector4 const& a, 
		hkVector4 const& b, 
		hkVector4 const& up, 
		hkVector4 & aTangentPoint,
		hkVector4 & bTangentPoint);

	inline static hkBool32 inCapsule(
		hkVector4 const& p,
		hkVector4 const& c1,
		hkVector4 const& c2,
		hkVector4 const& up);

	static hkReal HK_CALL calcMultilevelDistance( 
		hkVector4Parameter a, 
		hkVector4Parameter b, 
		hkVector4Parameter up, 
		hkSimdRealParameter maxIgnoredHeight );

	static hkReal HK_CALL approximateDistanceToGoal(
		hkVector4Parameter position,
		hkVector4Parameter corner,
		hkVector4Parameter goal,
		hkVector4Parameter up,
		hkSimdRealParameter maxIgnoredHeight);

	static hkReal HK_CALL calcOptimalSpeed(
		hkReal distToGoal,
		hkReal currentSpeed,
		hkReal desiredMaxSpeed,
		hkReal desiredGoalSpeed,
		hkReal desiredMaxAcceleration,
		hkReal desiredMaxDeceleration,
		hkReal absoluteMaxAcceleration,
		hkReal absoluteMaxDeceleration,
		hkReal timestep,
		bool & approachingGoal);

};

#include <Ai/Pathfinding/Character/LocalSteering/EdgePath/hkaiEdgePathSteeringUtil.inl>

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
