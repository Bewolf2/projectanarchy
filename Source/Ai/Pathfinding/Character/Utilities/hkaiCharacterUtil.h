/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_CHARACTER_UTIL_H
#define HK_AI_CHARACTER_UTIL_H

#include <Ai/Pathfinding/Astar/hkaiAstarParameters.h>

#include <Ai/Pathfinding/Utilities/hkaiPathfindingUtil.h>

#include <Ai/Pathfinding/World/hkaiWorld.h>
#include <Ai/Pathfinding/Collide/NavMesh/hkaiNavMeshQueryMediator.h>
#include <Ai/Pathfinding/Collide/NavVolume/hkaiNavVolumeMediator.h>

class hkaiCharacter;
class hkaiPath;

	/// Utility for managing path requests and results for characters.
class hkaiCharacterUtil
{
public:
HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiCharacterUtil);
	
		/// These correspond to each of the character callbacks that can be fired.
	enum CallbackType
	{
		CALLBACK_PATH_SUCCEEDED,
		CALLBACK_PATH_FAILED_INVALID_START,
		CALLBACK_PATH_FAILED_INVALID_GOAL,
		CALLBACK_PATH_FAILED_INVALID_UNREACHABLE,
		CALLBACK_PATH_NOT_READY,
	};

		/// hkaiPath results and callback type.
	struct ProcessedPath
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiCharacterUtil::ProcessedPath);

			/// Resulting path
		hkaiPath* m_path;
			/// Path request result code
		CallbackType m_result;

		hkBool32 m_lastPointIsGoal;

		ProcessedPath()
			: m_path(HK_NULL)
			, m_result(CALLBACK_PATH_NOT_READY)
			, m_lastPointIsGoal(false) { }

		ProcessedPath( CallbackType result )
			: m_path(HK_NULL)
			, m_result(result)
			, m_lastPointIsGoal(false) { }

		ProcessedPath( hkaiPath* path, CallbackType result, bool lastPointIsGoal )
			: m_path(path)
			, m_result(result)
			, m_lastPointIsGoal(lastPointIsGoal) { }
	};
	
		/// Assigns most of the input fields from the character.
	static void HK_CALL setupQueryInput( const hkaiCharacter* character, hkaiNavMeshQueryMediator::QueryInputBase& input );

		/// Assigns most of the input fields from the character. Note that query radius is not changed.
	static void HK_CALL setupGetClosestPointInput( const hkaiCharacter* character, hkaiNavMeshQueryMediator::GetClosestPointInput& gcpInput );

		/// Assigns most of the input fields from the character. Note that query radius is not changed.
	static void HK_CALL setupGetClosestPointInput( const hkaiCharacter* character, hkaiNavVolumeMediator::GetClosestPointInput& gcpInput );

		/// Create an path request for a character.
		/// The search parameters will be copied from hkaiWorld::getDefaultPathfindingInput()
	static hkaiPathfindingUtil::FindPathInput* HK_CALL generatePathRequest(const hkaiWorld* world, hkaiCharacter* character, hkVector4Parameter goal);

		/// Create a path request for a character.
		/// The search parameters will be copied from hkaiWorld::getDefaultPathfindingInput()
	static hkaiPathfindingUtil::FindPathInput* HK_CALL generatePathRequest(const hkaiWorld* world, hkaiCharacter* character, const hkVector4* goals, int numGoals);

		/// Create a path request for a flying character.
		/// The search parameters will be copied from hkaiWorld::getDefaultVolumePathfindingInput()
	static hkaiVolumePathfindingUtil::FindPathInput* HK_CALL generateVolumePathRequest(const hkaiWorld* world, hkaiCharacter* character, hkVector4Parameter goal);

		/// Create a path request for a flying character.
		/// The search parameters will be copied from hkaiWorld::getDefaultVolumePathfindingInput()
	static hkaiVolumePathfindingUtil::FindPathInput* HK_CALL generateVolumePathRequest(const hkaiWorld* world, hkaiCharacter* character, const hkVector4* goals, int numGoals );

		/// Fill in the face keys right before the path is computed.
		/// \param effectiveRadiusMultiplier The character's radius is multiplied by this value for A* queries and detecting edge penetrations. This should be greater than 1.0f (preferably around 1.5f).
	static void HK_CALL updatePathRequest(const hkaiWorld* world, const hkaiCharacter* character, hkaiPathfindingUtil::FindPathInput* input, hkReal querySize, hkReal effectiveRadiusMultiplier = 1.5f);
	
		/// Fill in the cell keys right before the path is computed.
	static void HK_CALL updatePathRequest(const hkaiWorld* world, const hkaiCharacter* character, hkaiVolumePathfindingUtil::FindPathInput* input, hkReal querySize );

		/// Input structure for determing the start and goal faces for nav mesh A*.
	struct GetFaceForPathfindingSettings
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, hkaiCharacterUtil::GetFaceForPathfindingSettings );
		GetFaceForPathfindingSettings();

			/// Length of raycasts performed in the up and down directions.
		hkReal m_raycastLength; //+default(2.0f);

			/// Query size used for getClosestPoints (if the raycasts miss).
		hkReal m_aabbQuerySize; //+default(2.0f);
			
			/// Whether or not to try to move the character away from the edges of the nav mesh.
		hkBool m_resolveEdgePenetrations; //+default(true);

			/// Character radius (only used if m_resolveEdgePenetrations is true).
		hkReal m_characterRadius; //+default(.5f);

			/// Whether or not to use hkaiNavMeshQueryMediator::coherentGetClosestPoint and coherentCastRay.
		hkBool m_useCoherentQueries; //+default(true)

			/// Tolerance used for both hkaiNavMeshQueryMediator::CoherentInput::m_isOnFaceTolerance and m_coherencyTolerance.
		hkReal m_coherencyTolerance; //+default(1e-3f);

			/// Previous face that the character was on, used for hkaiNavMeshQueryMediator::coherent*.
		hkaiPackedKey m_previousFaceKey; //+default(HKAI_INVALID_PACKED_KEY)
	};

		/// Extract the reference frame of the mesh section that this character is in (according to getCurrentNavMeshFace).
		/// Returns NULL if the character is not attached to a mesh face, or if the specified mesh is not loaded.
	static const hkaiReferenceFrame* HK_CALL getCharacterReferenceFrame( const hkaiCharacter* character, const hkaiWorld* world );

		/// Returns a face close to the requested point. The face is determined by raycasts up and down, or the closest face if the raycasts fail.
		/// If specified, will also call resolveEdgePenetrations on the closestPoint.
		/// This can be useful for pathfinding in dynamic environments - the start/end points of the path are saved,
		/// and later the actual start/end faces are computed.
	static hkaiPackedKey HK_CALL getFaceForPathfinding( const hkaiStreamingCollection* collection, hkVector4Parameter point, hkVector4Parameter up,
		const hkaiNavMeshQueryMediator* mediator, const hkaiNavMeshQueryMediator::QueryInputBase& inputBase, const GetFaceForPathfindingSettings& settings, hkVector4& pointOut );


		/// Creates an ProcessedPath frm the request's output for the chracter.
		/// An extra segment is added between the character's position and the start of the path
		/// if the projected distance between them is greater than characterToPathStartThreshold.
	static ProcessedPath HK_CALL processPathOutput(const hkaiNavMeshPathRequestInfo& requestInfo, const hkaiWorld* world, hkaiCharacter* character, hkSimdRealParameter characterToPathStartThreshold);
	
	static ProcessedPath HK_CALL processPathOutput(const hkaiNavVolumePathRequestInfo& requestInfo, const hkaiWorld* world, hkaiCharacter* character);

	/// Integrate the position/orientation of N characters forward in time.
	///
	/// \deprecated This function should not be called directly; it is used
	/// internally by integrateMotionAndProjectToMesh(). See the documentation
	/// of that function for more details.
	/// 
	/// \param timestep Integration timestep
	/// \param world AI world for all the characters
	/// \param characters Array of pointers to hkaiCharacter objects to update
	/// \param numCharacters Number of characters in the characters array
	static void HK_CALL integrateMotion( hkReal timestep, hkaiCharacter* const* characters, int numCharacters );

		/// Input parameters for hkaiCharacterUtil::integrateMotionAndProjectToMesh
	struct ProjectToMeshSettings 
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, hkaiCharacterUtil::ProjectToMeshSettings );
		ProjectToMeshSettings();

			/// GetClosestPoint query radius for putting characters back onto the nav mesh
		hkReal m_meshQueryRadius; //+default(1.0f)

			/// Raycast length for putting characters back onto the nav mesh
		hkReal m_meshRaycastLength; //+default(.25f);

			/// Integration damping factor between integrated position (at 0.0) and the projected mesh
			/// position (at 1.0). If this factor is set to 1.0, the characters will snap instantly to the mesh.
		hkReal m_meshProjectDamping; //+default(.75f)

			/// Whether or not to use hkaiNavMeshQueryMediator::coherentGetClosestPoint and coherentCastRay.
		hkBool m_useCoherentQueries; //+default(true)

			/// Tolerance used for both hkaiNavMeshQueryMediator::CoherentInput::m_isOnFaceTolerance and m_coherencyTolerance
		hkReal m_coherencyTolerance; //+default(1e-3f);
	};

	/// A simple utility for updating the position of AI characters which are 
	/// not driven by an animation system.
	/// 
	/// Each character in the array provided has their position incremented by
	/// their velocity vector multiplied by the timestep, and the resultant
	/// position is projected onto the navmesh.
	/// 
	/// This function should only be used if Havok AI has full control over the
	/// exact positioning and motion of characters. It should not be used if 
	/// other code is updating character positions. For instance, if you 
	/// transfer AI-requested velocity to the physics system, allow the physics
	/// world to step, and then transfer the character's current position and 
	/// velocity to AI with setPosition() and setVelocity(), you should not use
	/// this function. As a simple heuristic, if you are calling 
	/// hkaiCharacter::setPosition() each frame, you should not use this 
	/// function.
	/// 
	/// \param timestep Integration timestep
	/// \param world AI world for all the characters
	/// \param characters Array of pointers to hkaiCharacter objects to update
	/// \param numCharacters Number of characters in the characters array
	/// \param settings Input settings
	static void HK_CALL integrateMotionAndProjectToMesh( hkReal timestep, const hkaiWorld* world, hkaiCharacter* const* characters, int numCharacters,
		const ProjectToMeshSettings& settings );
};

#endif // HK_AI_CHARACTER_UTIL_H

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
