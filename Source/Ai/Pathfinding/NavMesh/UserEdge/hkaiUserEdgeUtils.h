/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_USER_EDGE_UTILS_H
#define HKAI_USER_EDGE_UTILS_H

#include <Ai/Pathfinding/hkaiBaseTypes.h>
#include <Ai/Pathfinding/NavMesh/hkaiNavMesh.h>
class hkaiNavMesh;
class hkaiNavMeshInstance;
class hkaiStreamingCollection;

extern const hkClass hkaiUserEdgeUtilsClass;
extern const hkClass hkaiUserEdgeUtilsObbClass;
extern const hkClass hkaiUserEdgeUtilsUserEdgeSetupClass;
extern const hkClass hkaiUserEdgeUtilsUserEdgePairClass;
extern const hkClass hkaiUserEdgeSetupArrayClass;
extern const hkClass hkaiUserEdgePairArrayClass;

/// Miscellaneous utility functions for manipulating user edges in nav meshes
class hkaiUserEdgeUtils
{
	HK_DECLARE_REFLECTION();
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, hkaiUserEdgeUtils);
	public:

		/// A simple OBB representation used as input for offline user edge setup.
		/// The OBB will be treated as a 2D quad and intersected with the navmesh; the shortest dimension is ignored.
		/// Note that the format here is the same as in hkGeometryUtility::calcObb.
	struct Obb
	{
	public:
		HK_DECLARE_REFLECTION();
		Obb()
		{ 
			m_transform.setIdentity(); 
			m_halfExtents.setZero();
		}
			
			/// Set the OBB from a line segment and the vertical half extents (e.g. the amount the OBB goes up and down
			/// from the line segment). These define two directions and half extents for the OBB; the third direction
			/// is taken from the cross product of the line segment direction and the vertical direction, and the third 
			/// and the third half extent is HK_REAL_EPSILON.
		void setFromLineSegment( hkVector4Parameter p0, hkVector4Parameter p1, hkVector4Parameter verticalHalfExtent);

			/// Given the world up vector, gets the segment that's most in the up direction, and the biggest horizontal direction
		void getUpAndHorizontalSegments( hkVector4Parameter up, hkVector4& upOutStart, hkVector4& upOutEnd, hkVector4& horizOutStart, hkVector4& horizOutEnd ) const;

			/// Translation and rotation of the OBB
		hkTransform m_transform;
			
			/// Size in each direction
		hkVector4 m_halfExtents;

	protected:
		void _getSegmentForAxis( int i, hkVector4& segOutA, hkVector4& segOutB ) const;
	};
	
	enum UserEdgeDirection
	{
			/// User edge is not traversable.
		USER_EDGE_BLOCKED = 0,

			/// User edge is traversable from A to B.
		USER_EDGE_A_TO_B,

			/// User edge is traversable from B to A.
		USER_EDGE_B_TO_A,

			/// User edge is traversable in both directions.
		USER_EDGE_BIDIRECTIONAL
	};

	enum UserEdgeSetupSpace
	{
			/// OBBs are specified in world space
		USER_EDGE_SPACE_WORLD,

			/// OBBs are specified in instance-local space
		USER_EDGE_SPACE_LOCAL,
	};

		/// Input to hkaiUserEdgeUtils::createUserEdgePairs() representing a desired pair of user edges.
		/// This can be set up e.g., in a level editor then used after nav mesh generation.
		/// The OBBs are specified in world space by default; this can be changed with m_space.
	struct UserEdgeSetup
	{
		// +version(2)
		HK_DECLARE_REFLECTION();
		UserEdgeSetup();

			/// Set both worldUp vectors.
		inline void setUp( hkVector4Parameter up){ m_worldUpA = up; m_worldUpB = up; }

			/// Calls Obb::setFromLineSegment() on both OBBs, and sets the up vectors.
			/// See Obb::setFromLineSegment() for more information.
		void setFromLineSegments(	hkVector4Parameter A1, hkVector4Parameter A2, hkVector4Parameter B1, 
									hkVector4Parameter B2, hkVector4Parameter verticalHalfExtent );

			/// Pair of OBBs to be intersected
		Obb		m_obbA;
		Obb		m_obbB;

			/// User data to be set on the resulting edges
		hkUint32 m_userDataA;
		hkUint32 m_userDataB;
		
			/// Costs of traversing the user edges. If negative, the cost will be the Euclidean distance between the segment midpoints.
		hkReal m_costAtoB;
		hkReal m_costBtoA;

			/// World up directions. These can be different, e.g. when using wall climbing.
		hkVector4 m_worldUpA; //+default(0,0,0,0)
		hkVector4 m_worldUpB; //+default(0,0,0,0)

			/// Whether the edge goes from goes from A to B, B to A, or is bidirectional.
		hkEnum<UserEdgeDirection, hkUint8> m_direction;

			/// Whether the OBBs are specified in world or local space.
		hkEnum<UserEdgeSetupSpace, hkUint8> m_space; //+default(hkaiUserEdgeUtils::USER_EDGE_SPACE_WORLD)

			/// Whether to swap the computed direction of the edges so that they are approximately parallel and wound oppositely - This may be
			/// useful if the OBBs have been oriented visually and their front/back sides are not necessarily correctly chosen.
		hkBool m_forceAlign;	//+default(true)

	};

		/// Information for a pair of user edges. Can be added to the nav mesh, and updated after nav mesh cutting.
	struct UserEdgePair
	{
		//+version(1)
		HK_DECLARE_REFLECTION();
		UserEdgePair();
		inline void setPoints( hkVector4Parameter A1, hkVector4Parameter A2, hkVector4Parameter B1, hkVector4Parameter B2)
		{
			m_x = A1;
			m_y = A2;
			m_z = B1;
			hkVector4 w = B2;
			HK_TRANSPOSE4(m_x, m_y, m_z, w);
		}

		inline void getPoints( hkVector4& A1, hkVector4& A2, hkVector4& B1, hkVector4& B2) const
		{
			// Transpose the stored vectors. We need to specify something for the w components, might as well be 0.
			A1 = m_x;
			A2 = m_y;
			B1 = m_z;
			B2.setZero();
			HK_TRANSPOSE4(A1, A2, B1, B2);
		}

		//
		// Mesh-local space points of the edges. These are stored in transposed format to save space.
		//
		
			/// x coordinates of the points
		hkVector4 m_x;

			/// y coordinates of the points
		hkVector4 m_y;
			
			/// z coordinates of the points
		hkVector4 m_z;
		
			/// UID of mesh instance A
		hkaiSectionUid m_instanceUidA;

			/// UID of mesh instance B
		hkaiSectionUid m_instanceUidB;

			/// Face index for edge A
		hkaiNavMesh::FaceIndex m_faceA;

			/// Face index for edge B
		hkaiNavMesh::FaceIndex m_faceB;
		
			/// User data that is set for edge A
		hkaiNavMesh::EdgeData m_userDataA;

			/// User data that is set for edge B
		hkaiNavMesh::EdgeData m_userDataB;
		
			/// Traversal cost of going from edge A to edge B. If negative, the euclidean distance between the midpoints will be used.
		hkHalf m_costAtoB;

			/// Traversal cost of going from edge B to edge A. If negative, the euclidean distance between the midpoints will be used.
		hkHalf m_costBtoA;

			/// Traversability of the edge.
		hkEnum<UserEdgeDirection, hkUint8> m_direction;

	};

	//
	// Setup methods. These are intended to be called off-line, then the UserEdgePairs can be saved and loaded at runtime.
	//
		/// Intersects the OBBs in the UserEdgeSetup with the provided mesh, and generates a list of UserEdgePairs.
		/// These pairs can then be added directly to the nav mesh with addUserEdgePair(), or to a cut mesh with addUserEdgePairsToCutMesh()
	static void HK_CALL createUserEdgePairs( const UserEdgeSetup& input, const hkaiNavMeshInstance& meshA, const class hkaiNavMeshQueryMediator* mediator, hkArray<UserEdgePair>& userEdgePairsOut, hkReal minEdgeLength = .05f);

		/// Intersects the OBBs in the UserEdgeSetup with the provided meshes, and generates a list of UserEdgePairs.
		/// These pairs can then be added directly to the nav mesh with addUserEdgePair(), or to a cut mesh with addUserEdgePairsToCutMesh()
	static void HK_CALL createUserEdgePairs( const UserEdgeSetup& input, const hkaiNavMeshInstance& meshA, const hkaiNavMeshInstance& meshB, const class hkaiNavMeshQueryMediator* mediatorA, const class hkaiNavMeshQueryMediator* mediatorB, hkArray<UserEdgePair>& userEdgePairsOut, hkReal minEdgeLength = .05f);


	//
	// Methods to add user edges to nav mesh instances. These assume that there is no cutting in the hkaiNavMeshInstance (or hkaiStreamingCollection) yet, and make no
	// attempt to handle faces which have been cut.
	//

	enum ClearanceResetMode
	{
		RESET_CLEARANCE_CACHE,
		DONT_RESET_CLEARANCE_CACHE
	};

	struct UpdateParameters
	{
		HK_FORCE_INLINE UpdateParameters()
		:	m_clearanceResetMode(RESET_CLEARANCE_CACHE),
			m_updateHierarchyGraphs(true),
			m_compactHierarchyGraphs(false)
		{
		}

		hkEnum<ClearanceResetMode, hkUint8> m_clearanceResetMode;
		hkBool m_updateHierarchyGraphs;
		hkBool m_compactHierarchyGraphs;
	};

		/// Adds a user edge as specified by the UserEdgePair
	static void HK_CALL addUserEdgePair( hkaiStreamingCollection* collection, const UserEdgePair& pair, const UpdateParameters& params = UpdateParameters() );

		/// Adds a user edge as specified by the UserEdgePair
	static void HK_CALL addUserEdgePair( hkaiNavMeshInstance& instance, const UserEdgePair& pair, const UpdateParameters& params = UpdateParameters() );

		/// Adds a user edge from face A to the face B.
	static void HK_CALL addUserEdgePair(hkaiNavMeshInstance& instance, hkaiNavMesh::FaceIndex faceIndexA, hkaiNavMesh::FaceIndex faceIndexB, hkVector4Parameter edgeAStart, hkVector4Parameter edgeAEnd, hkVector4Parameter edgeBStart, hkVector4Parameter edgeBEnd, bool allowAtoB = true, bool allowBtoA = false, hkReal costAtoB = -1.0f, hkReal costBtoA = -1.0f);

		/// Adds a user edge from face A to the face B.
	static void HK_CALL addUserEdgePair( hkaiStreamingCollection* collection, hkaiPackedKey faceKeyA, hkaiPackedKey faceKeyB, hkVector4Parameter edgeAStart, hkVector4Parameter edgeAEnd, hkVector4Parameter edgeBStart, hkVector4Parameter edgeBEnd, bool allowAtoB = true, bool allowBtoA = false, hkReal costAtoB = -1.0f, hkReal costBtoA = -1.0f);

		/// Add multiple user edge pairs at one, avoiding internal fragmentation of the edge arrays
	static void HK_CALL addUserEdgePairsBatch( hkaiStreamingCollection* collection, const hkArrayBase<UserEdgePair>& pairs, const UpdateParameters& params = UpdateParameters() );


	//
	// Methods for adding user edges after cutting has occurred.
	//
	
	struct CutMeshUpdateParameters : public UpdateParameters
	{
		HK_FORCE_INLINE CutMeshUpdateParameters()
		: UpdateParameters(),
			m_minEdgeLength(.05f)
		{
			m_compactHierarchyGraphs = true;
		}
		
		hkReal m_minEdgeLength;		
	};

		/// Adds the edge pairs to the cut nav mesh. This automatically handles faces being cut, including gaps introduced from silhouettes.
		/// Note: This method should NOT be called every frame with the same input pairs, as it will create many nav mesh edges every time and eventually run out of memory.
		/// To handle edges that were removed during cutting, use hkaiUserEdgeUtils::selectiveAddUserEdgePairsToCutMesh() instead.
	static void HK_CALL addUserEdgePairsToCutMesh( hkaiStreamingCollection* collection, const hkArrayBase<UserEdgePair>& pairs, const class hkaiNavMeshCutter* cutter, const CutMeshUpdateParameters& params = CutMeshUpdateParameters() );

		/// Adds the edge pairs that are on modified faces to the cut nav mesh. This automatically handles faces being cut, including gaps introduced from silhouettes.
		/// This should be done from within an hkaiWorld::Listener::dynamicNavMeshModifiedCallback().
	static void HK_CALL selectiveAddUserEdgePairsToCutMesh( hkaiStreamingCollection* collection, const hkArrayBase<UserEdgePair>& pairs, const class hkaiNavMeshCutter* cutter, const hkArrayBase<hkaiPackedKey>& cutFaceKeys, const hkArrayBase<hkaiPackedKey>& uncutFaceKeys, const CutMeshUpdateParameters& params = CutMeshUpdateParameters() );


	//
	// Methods to remove user edges from hkaiNavMeshInstances and hkaiStreamingCollections
	//

	struct RemoveParameters
	{
		HK_FORCE_INLINE RemoveParameters()
			:	m_updateHierarchyGraphs(true)
		{
		}

		hkBool m_updateHierarchyGraphs;
	};

		/// Removes all user edges in a hkaiNavMeshInstance
	static void HK_CALL removeUserEdges( hkaiNavMeshInstance& meshInstance, hkaiStreamingCollection* collection );

		/// Removes user edges from specific faces in a collection
	static int HK_CALL removeUserEdgesFromFaces( hkaiStreamingCollection* collection, const hkArrayBase<hkaiPackedKey>& faceKeys, const RemoveParameters& params = RemoveParameters() );
	
		/// Removes user edges from a specific face in a collection. Returns the number of user edges removed.
	static int HK_CALL removeUserEdgesFromFace( hkaiStreamingCollection* collection,  hkaiPackedKey faceKey, const RemoveParameters& params = RemoveParameters() );

};

	/// Utility class for serialization of a hkaiUserEdgeUtils::UserEdgeSetup array
class hkaiUserEdgeSetupArray : public hkReferencedObject
{
public:
	HK_DECLARE_REFLECTION();
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_NAVMESH );

	hkaiUserEdgeSetupArray() {}

	hkaiUserEdgeSetupArray( hkFinishLoadedObjectFlag f)
	:	hkReferencedObject(f),
		m_edgeSetups(f)
	{

	}

	hkArray<hkaiUserEdgeUtils::UserEdgeSetup> m_edgeSetups;

};

	/// Utility class for serialization of a hkaiUserEdgeUtils::UserEdgePair array
class hkaiUserEdgePairArray : public hkReferencedObject
{
public:
	HK_DECLARE_REFLECTION();
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_NAVMESH );

	hkaiUserEdgePairArray() {}

	hkaiUserEdgePairArray( hkFinishLoadedObjectFlag f)
	:	hkReferencedObject(f),
		m_edgePairs(f)
	{

	}

	hkArray<hkaiUserEdgeUtils::UserEdgePair> m_edgePairs;

};


#endif // HKAI_USER_EDGE_UTILS_H

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
