/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_CORNER_PREDICTOR_H
#define HKAI_CORNER_PREDICTOR_H

#include <Ai/Pathfinding/Character/LocalSteering/EdgePath/hkaiEdgePath.h>

class hkaiStreamingCollection;

/// A utility for predicting the future trajectory of edge-steered characters.
///
/// Predictions are produced incrementally, one corner at a time. Each time
/// stepForward() is called, the next prediction is returned. Arbitrarily many
/// corners may be looked-ahead in this way.
/// 
/// The predictions are all made in the character's local coordinate space, as
/// specified by the positionSectionIndex argument to the constructor. If
/// future corner traversals take place in other nav mesh sections with 
/// different transforms, they will be transformed into the character's local
/// coordinate space. If the path passes through a user edge, the predictions 
/// for following corners will be "flattened" by transforming that part of the
/// geometry such that the two sides of the user edge match up. Likewise, for
/// wall-climbing meshes, changes in the up-vector will be flattened such that
/// the effective up vector for predicted corners is the character's local
/// up-vector.
/// 
/// Predictions are projected to an up-vector-normal plane. For instance, if
/// the character's local up-vector is +Y, the Y-coordinates of the hkVector4 
/// members of hkaiCornerPredictor::Prediction will be 0.
/// 
/// A hkaiCornerPredictor should not be kept between frames: changes to the
/// edge path (including those that hkaiEdgePath::updateTraversalState() may
/// cause) can invalidate the internal state of the predictor. Computation 
/// results are cached in the hkaiEdgePath, so constructing and using a new 
/// hkaiCornerPredictor on subsequent frames will not incur a performance 
/// penalty.
class hkaiCornerPredictor
{
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_STEERING, hkaiCornerPredictor);

	hkaiCornerPredictor();

	/// Initialize the predictor with results from calcForwardVector(). 
	/// 
	/// This method should be called by hkaiEdgeFollowingBehavior; it is not
	/// intended for use by user code.
	/// 
	/// The edge path must not be modified between the call to 
	/// calcForwardVector() and the call to this function, including by calling 
	/// hkaiEdgePath::updateTraversalState().
	/// 
	/// @param edgePath The hkaiEdgePath which was given to 
	/// calcForwardVector(). This must be non-const, because it may compute and
	/// store following corner information in the path. No other data in the
	/// path will be changed.
	/// @param positionLocal The local-space position passed to 
	/// calcForwardVector().
	/// @param positionSectionIndex The runtime index of the nav mesh section
	/// defining positionLocal's coordinate space, or 
	/// HKAI_INVALID_RUNTIME_INDEX to specify that positionLocal is in global 
	/// space. Predictions will be expressed in this coordinate space.
	/// @param streamingCollection The streaming collection of nav mesh 
	/// instances.
	/// @param forwardVectorLocal The forward vector returned by 
	/// calcForwardVector().
	/// @param nextEdgeIndex The next edge index returned by 
	/// calcForwardVector().
	/// @param nextIsLeft The is-left flag returned by calcForwardVector().
	/// @param upLocal The up-vector, in the same space as positionLocal.
	void init(
		hkaiEdgePath* edgePath, 
		hkVector4Parameter positionLocal, 
		hkaiRuntimeIndex positionSectionIndex,
		hkaiStreamingCollection const* streamingCollection,
		hkVector4Parameter forwardVectorLocal,
		int nextEdgeIndex,
		bool nextIsLeft,
		hkVector4Parameter upLocal);

	/// Initialize the predictor to return FULL_PREDICTION_ERROR on the first
	/// stepForward() call.
	/// 
	/// This method should be called by hkaiEdgeFollowingBehavior; it is not
	/// intended for use by user code.
	void initAsFullError();

	~hkaiCornerPredictor();

	/// Information about a user edge which will be traversed while traveling 
	/// between corners.
	struct UserEdgeTraversal
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_STEERING, UserEdgeTraversal);
		/// The point, transformed to the character's local space, where the
		/// user edge will be entered.
		hkVector4 m_entrancePointLocal;

		/// A pointer to the edge data associated with the user edge entrance.
		/// This pointer will remain valid until the hkaiEdgePath is modified
		/// (other than by the predictor itself).
		hkaiNavMesh::EdgeData const* m_edgeDataPtr;
	};

	/// Information about the character's predicted trajectory towards and then
	/// around an upcoming corner, returned by stepForward().
	/// 
	/// All points and vectors are "flattened" into the originally provided 
	/// character-local space; see the class documentation for details.
	struct Prediction
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_STEERING, Prediction);
		/// The vector from the previous leaveTurnPoint (or, initially, the 
		/// character's current position) to the enterTurnPoint.
		hkVector4 m_travelVector;

		/// The point at which the character will stop moving straight and
		/// start turning around the corner.
		hkVector4 m_enterTurnPoint;

		/// The center of the corner which the character will turn around;
		/// the magnitude of the W-coordinate is the radius of the turn
		/// (disregard its sign).
		hkVector4 m_cornerCenter;

		/// The point at which the character will stop turning around the
		/// upcoming corner and start moving toward a future corner.
		hkVector4 m_leaveTurnPoint;

		/// The signed angle, in radians, of this turn; left turns are 
		/// positive.
		hkReal m_signedAngle;

		/// Information about any user edges crossed during this trajectory,
		/// starting with and including the previous prediction's 
		/// m_leaveTurnPoint.
		hkArray<UserEdgeTraversal> m_userEdgeTraversals;
	};

	/// Possible results from calling stepForward.
	enum StepForwardResult
	{
		/// Indicates that corner prediction has succeeded, and all members of
		/// the Prediction have been filled.
		NORMAL,

		/// Indicates that the corner being processed is a goal, and only 
		/// m_travelVector, m_enterTurnPoint, and m_cornerCenter have been
		/// filled. 
		AT_GOAL,

		/// Indicates that corner prediction has partially failed, and only 
		/// m_travelVector, m_enterTurnPoint, and m_cornerCenter have been
		/// filled.
		PARTIAL_PREDICTION_ERROR,

		/// Indicates that corner prediction has fully failed, and no members
		/// have been filled.
		FULL_PREDICTION_ERROR,

		/// An internal value, not returned by stepForward().
		INIT_PARTIAL_ERROR,

		/// An internal value, not returned by stepForward().
		INIT_FULL_ERROR,

		/// An internal value, not returned by stepForward().
		NOT_INITED,
	};

	/// Step the prediction state forward one corner.
	///
	/// The first call to stepForward() will return information about the 
	/// trajectory towards and then around the first upcoming corner, the one
	/// reported by calcForwardVector().
	/// 
	/// Subsequent calls will return information about following corners which
	/// the character will turn around. The trajectory of each prediction after
	/// the first begins when the character stops turning around a corner and
	/// begins moving in a straight line towards the next corner.
	/// 
	/// The return value indicates which members of predictionOut have been
	/// filled, and which are undefined. 
	/// 
	/// It is an error to call this function if the previous stepForward call
	/// (if any) did not return NORMAL.
	StepForwardResult stepForward(Prediction & predictionOut);


private:
	// In order to present a simple and directly useful interface to the user,
	// certain predictions are made one step in advance. For instance, the
	// Prediction contains m_leaveTurnPoint. The process of finding that point,
	// of course, entails finding the following travel vector and turn entry
	// point; otherwise there would be no way of knowing when the character should
	// stop turning. Likewise, some data from the prediction is saved and used
	// during the following prediction.
	// 
	// In this class' private variables, the prefix "next" indicates data which
	// was computed during the previous stepForward() but was not output as 
	// part of the prediction because it related to the corner following the 
	// "current corner" in that output, and which will instead form part of the
	// following prediction. (Before the first call to stepForward(), it is 
	// data which has been initialized by the constructor such that it can be
	// treated as the general case by stepForward().)


	/// The character's up-vector, in the section-local space.
	hkVector4 m_up;

	/// The vector which will be output as m_travelVector by the next call to
	/// stepForward().
	hkVector4 m_nextTravelVector;

	/// The vector which will be output as m_enterTurnPoint by the next call to
	/// stepForward().
	hkVector4 m_nextEnterTurnPoint;

	/// A concatenated transform which would transform the endpoints of the 
	/// edge referred to by m_nextEdgeIndex into the character's local section.
	/// It has not been concatenated with the m_followingTransform of that 
	/// edge.
	hkMatrix4 m_nextTransform;

	/// The edge index of the corner which will be output as m_cornerCenter by
	/// the next call to stepForward().
	int m_nextEdgeIndex;

	/// Whether the corner which will be output as m_cornerCenter by the next
	/// call to stepForward() was a left corner.
	hkBool32 m_nextIsLeft;

	/// The value returned by the previous call to stepForward().
	StepForwardResult m_prevResult;

	/// Must be non-const because it can cache corner info.
	hkRefPtr<hkaiEdgePath> m_edgePath;

	hkRefPtr<const hkaiStreamingCollection> m_streamingCollection;
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
