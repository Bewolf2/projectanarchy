/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_EDGE_PATH_H
#define HKAI_EDGE_PATH_H

#include <Ai/Pathfinding/Utilities/hkaiPathfindingUtil.h>
#include <Ai/Pathfinding/Dynamic/NavMeshCutter/hkaiNavMeshCutter.h>

class hkaiGeneralAccessor;
class hkaiNavMeshCutter;
class hkaiWorld;

class hkaiEdgePath : public hkReferencedObject
{
public:
	//+version(1)
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_STEERING);
	HK_DECLARE_REFLECTION();

	hkaiEdgePath();
	hkaiEdgePath(hkFinishLoadedObjectFlag f);

	/// Fill with the edges traversed by a shortest path on a navmesh.
	void fillFromFoundPath(
		hkaiPathfindingUtil::FindPathInput const& findPathInput, 
		hkaiPathfindingUtil::FindPathOutput const& findPathOutput, 
		hkReal characterRadius,
		hkReal leftTurnRadius,
		hkReal rightTurnRadius,
		hkVector4Parameter worldUp,
		hkaiGeneralAccessor const &accessor, 
		hkaiNavMeshCutter const* cutter);

	struct FollowingCornerInfo
	{
		//+version(1)

		HK_DECLARE_POD_TYPE();
		HK_DECLARE_REFLECTION();

		void setUncalculated()
		{
			m_data = 0;
		}

		hkBool32 isCalculated() const
		{
			return m_data & 0x8000u;
		}

		hkBool32 isValid() const
		{
			HK_ASSERT(0x27d13815, isCalculated());
			return m_data & 0x4000;
		}

		hkBool32 isLeft() const
		{
			HK_ASSERT(0x27d13816, isCalculated());
			return m_data & 0x2000;
		}

		hkUint16 getOffset() const
		{
			HK_ASSERT(0x27d13817, isCalculated());
			return m_data & 0x1FFF;
		}

		void setCalculatedInvalid()
		{
			m_data = 0x8000u;
		}

		void setCalculatedValid(bool isleft, int offset)
		{
			HK_ASSERT(0x27d13818, offset > 0 && offset < 2<<13);
			m_data = 0xC000u | (isleft << 13) | (hkUint16)offset;
		}

		hkUint16 m_data;
		/* The effective layout
		unsigned m_calculated : 1;
		unsigned m_valid : 1;
		unsigned m_isLeft : 1;
		unsigned m_offset : 13;
		*/
	};

	struct Edge
	{
		//+version(2)

		HK_DECLARE_POD_TYPE();
		HK_DECLARE_REFLECTION();

		/// Endpoints of the edge. The effective radius of the endpoints is stored in the W coordinate;
		/// m_left has a negative W. These are expressed in the local coordinate space of the section
		/// packed in m_face.
		hkVector4 m_left, m_right; 

		/// Local-space up-vector for the face this edge leads out of.
		hkVector4 m_up;

		/// A transformation from the space of the section which this edge leads to, to the space of the
		/// section which this edge leads out from. Multiplying a point or vector expressed in the space 
		/// of the next section will produce the equivalent point or vector, expressed in the space of 
		/// the current section.
		hkMatrix4 m_followingTransform;

		/// A persistent key to the edge itself, including the face it is attached to.
		hkaiPersistentEdgeKey m_edge;

		/// Information cached by hkaiEdgePathSteeringUtil::getFollowingCorner()
		/// on the next constraining corner after the left corner of this edge.
		FollowingCornerInfo m_leftFollowingCorner;

		/// Information cached by hkaiEdgePathSteeringUtil::getFollowingCorner()
		/// on the next constraining corner after the right corner of this edge.
		FollowingCornerInfo m_rightFollowingCorner;

		/// Edge flags copied from the edge at the time the path was found.
		hkaiNavMesh::EdgeFlags m_flags;

		hkBool32 isGoal() const { return m_left.allExactlyEqual<4>(m_right); }
		hkBool32 isUserEdge() const { return m_flags.anyIsSet(hkaiNavMesh::EDGE_USER); }
	};

	/// Allocate space at the end of the path to store new edges, and return a pointer to the newly
	/// allocated space.
	Edge* expandEdges(int numNewEdges);
	/// Allocate space at the end of the path to store new edges, and return a pointer to the newly
	/// allocated space; also output a pointer to the corresponding edge data space
	Edge* expandEdges(int numNewEdges, hkaiNavMesh::EdgeData* & edgeDataPtrOut);

	hkInt32 getNumEdges() const;
	Edge const& getEdge(int edgeIdx) const;
	hkaiNavMesh::EdgeData const* getEdgeDataPtr(int edgeIdx) const;
	hkaiNavMesh::EdgeData* getEdgeDataPtr(int edgeIdx);
	int getEdgeDataStriding() const { return m_edgeDataStriding; }

	static void HK_CALL getEdgePointsInGlobalSpace(
		Edge const& edge, 
		hkaiStreamingCollection const& streamingCollection,
		hkVector4 & leftGlobal, hkVector4 & rightGlobal);

	void getEdgePointsInGlobalSpace(
		int edgeIdx, 
		hkaiStreamingCollection const& streamingCollection,
		hkVector4 & leftGlobal, hkVector4 & rightGlobal) const;

	struct TraversalState
	{
		/// The index of the edge which exits the face the agent is currently in,
		/// or -1 to indicate that the user is not in a face which is part of the
		/// path
		hkInt32 m_faceEdge;

		/// The lowest edge index which the agent may currently be in the capsule of.
		hkInt32 m_trailingEdge;

		/// The furthest user edge along the path which the agent has crossed. The
		/// traversal will never "rewind" through this edge.
		hkInt32 m_highestUserEdgeCrossed;
	};

	/// Initialize the traversal state for a new path traversal. The result is not
	/// useful until the first update has been performed.
	void initTraversalState(TraversalState & traversalState) const;

	/// Update the traversal state in response to an agent's motion.
	///
	/// @param newFaceKey the packed key of the face the character is on. If this is
	/// HKAI_INVALID_PACKED_KEY, the function will call updateTraversalStateNaive instead.
	/// @return true if the state update succeeded; false if the character is on an unknown
	/// face and needs a repath.
	hkBool32 updateTraversalState(
		TraversalState & traversalState, 
		hkVector4 const& newPositionLocal, 
		hkaiPackedKey newFaceKey, 
		bool repairPath, 
		hkReal userEdgeTolerance, 
		hkaiStreamingCollection const& streamingCollection,
		hkaiNavMeshCutter const* cutter);

	/// Update the traversal state in response to an agent's motion.
	///
	/// This function does not use the character's current nav mesh face information
	/// to track its progress, and so is appropriate for transitory situations
	/// when the nav mesh face cannot be determined, or when an edge path is manually
	/// created without face information. This function will not update the
	/// m_faceEdge or m_highestUserEdgeCrossed fields of the TraversalState.
	void updateTraversalStateNaive(TraversalState & traversalState, hkVector4 const& newPositionGlobal) const;

	/// Determine the range of edges that should be considered when determining the next
	/// corner to path towards.
	///
	/// The range of edges may include a prefix of edges which the user has already passed,
	/// and may include a suffix of edges which are out of view.
	void getFutureEdgeRange(TraversalState const& traversalState, Edge const* & begin, Edge const* & end) const;

	void getFutureEdgeRange(int offset, Edge const* & begin, Edge const* & end) const;

	/// Reports the goal point of the path, if there is one.
	///
	/// @param goalSectionOut If the path has a goal, filled with the runtime index of the
	/// goal's section.
	/// @param goalOut If the path has a goal, filled with the local-space location of that
	/// goal.
	/// @return whether the path has a goal or not.
	hkBool32 getGoal(hkaiRuntimeIndex & goalSectionOut, hkVector4 & goalOut) const;

	hkBool32 hasGoal() const;

	void setFollowingCornerInfoInvalid(
		int startEdge,
		bool startIsLeft);

	void setFollowingCornerInfoValid(
		int startEdge,
		bool startIsLeft,
		int followingCornerEdge,
		bool followingCornerIsLeft);

	hkBool32 hasFollowingCornerInfo(
		int startEdge,
		bool startIsLeft) const;

	/// Returns whether the following corner info for the specified corner
	/// (which must have previously been set) is valid; if it is, fills info
	/// about the following corner.
	hkBool32 getFollowingCornerInfo(
		int startEdge,
		bool startIsLeft,
		int & followingCornerEdgeOut,
		bool & followingCornerIsLeftOut) const;

	static void calcWallClimbingTransform(
		hkVector4Parameter leftPoint, 
		hkVector4Parameter curUp, 
		hkVector4Parameter newUp, 
		hkMatrix4 & transformOut);

	static void calcFollowingTransform(
		hkVector4Parameter curLeftPoint, 
		hkVector4Parameter curRightPoint, 
		hkVector4Parameter curUp, 
		hkVector4Parameter newLeftPoint, 
		hkVector4Parameter newRightPoint, 
		hkVector4Parameter newUp, 
		hkMatrix4 &followingTransform);

	void buildEdge(
		hkaiGeneralAccessor const &accessor, 
		hkaiNavMeshCutter const* cutter,
		hkaiPackedKey edgeKey, 
		hkVector4 & curUp, 
		hkaiPackedKey & curFaceKey, 
		hkaiRuntimeIndex & curSection, 
		bool & curSectionIsWC, 
		hkUint32 edgeIdx);

	void replacePrefixWithEdge(
		int numToRemove, 
		hkaiPackedKey edgeFaceKey, 
		hkaiPackedKey edgeKey, 
		hkaiStreamingCollection const& streamingCollection,
		hkaiNavMeshCutter const* cutter);

	enum PpivResult
	{
		PPIV_RESULT_SUCCESS,
		PPIV_RESULT_HIT_PATH_END,
		PPIV_RESULT_INVALID_PATH,
	};

	/// Modify the edge path such that it goes the other way around an internal
	/// vertex. 
	/// 
	/// @param edgeIndex the index of the edge containing the internal vertex.
	PpivResult pullPastInternalVertex(
		int edgeIndex,
		bool isLeft,
		TraversalState const& curTraversalState,
		hkaiStreamingCollection const& streamingCollection,
		hkaiNavMeshCutter const* cutter);

	hkBool32 consecutiveEdgesShareVertex(
		int firstEdgeIndex, 
		bool checkLeft, 
		hkaiStreamingCollection const& streamingCollection, 
		hkaiNavMeshCutter const* cutter) const;

private:
	FollowingCornerInfo const& getFollowingCornerInfoPacked(
		int startEdge,
		bool startIsLeft) const;

	FollowingCornerInfo & getFollowingCornerInfoPacked(
		int startEdge,
		bool startIsLeft);
	hkArray<Edge> m_edges;
	hkArray<hkaiNavMesh::EdgeData> m_edgeData;
	int m_edgeDataStriding; //+default(0)

	hkReal m_leftTurnRadius;
	hkReal m_rightTurnRadius;
	hkReal m_characterRadius;
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
