/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_BEHAVIOR_H
#define HK_AI_BEHAVIOR_H

#include <Ai/Pathfinding/World/hkaiPathRequestInfoOwner.h>
#include <Ai/Pathfinding/World/hkaiWorld.h>

struct hkaiLocalSteeringInput;
class hkaiCharacter;

	/// The hkaiBehavior class is the base class for all behaviors.
	/// A behavior is responsible for computing the desired velocity of an
	/// hkaiCharacter at each frame, as well as other state data required 
	/// for local steering.
class hkaiBehavior : public hkReferencedObject, public hkaiPathRequestInfoOwner
{
public:

	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_STEERING);

	hkaiBehavior(hkaiWorld* world) : m_world(world) { }

	virtual ~hkaiBehavior() { }

		/// Compute the desired velocities of one or more managed characters.
		/// If hkaiWorld::stepMultithreaded is used, this function will be invoked in a multithreaded context.
		/// \param outputs Output array of local steering inputs
		/// \param outputsSize Size of the output buffer array. The caller should provide at least one space per managed character.
		/// \return Number of characters' output data that was written
	virtual int calcVelocities( hkReal timestep, hkaiLocalSteeringInput* outputs, int outputsSize ) = 0;

		/// Update character state and notify listeners. This is called by hkaiWorld, and should not be called
		/// by user code.
		///
		/// This function is always called from the thread that invoked hkaiWorld::step.
		///
		/// If you inherit from one of the standard behaviors and override this function, you should call the base 
		/// class update() at the top of your implementation.
	virtual void update( hkReal timestep ) = 0;

		/// Estimate a polyline path which the behavior will cause one of its character(s) to 
		/// follow. Used for debug drawing.
	virtual void getApproximateFuturePositions(int characterIndex, hkArray<hkVector4>::Temp& positions) const = 0;

		/// Get conservative set of bounding boxes through which the behavior may cause its characters to go.
	virtual void getFutureBoundingBoxes( hkArray<hkAabb>::Temp& regions) const = 0;

		/// Returns whether the behavior will cause any of its characters to go through any of the 
		/// sections in the provided set.
	virtual bool pathsThroughAnySection(hkSet<hkaiRuntimeIndex> const& sections) const = 0;

		/// Request a new path to the given goal. The default implementation simply
		/// calls requestPathWithMultipleGoals with a single goal.
	virtual void requestPath(hkVector4Parameter goal, int priority = 0);

		/// Request a new path to multiple potential goals. 
		/// The path to the closest goal (in terms of total distance) will be computed.
		/// Note that this is NOT a list of waypoints for the character to follow.
	virtual void requestPathWithMultipleGoals(const hkVector4* goals, int numGoals, int priority = 0) = 0;

		/// Request a new path to the given goal. The default implementation simply
		/// calls requestPathWithMultipleGoals with a single goal.
	virtual void requestVolumePath(hkVector4Parameter goal, int priority = 0);

		/// Request a new path to multiple goals.
	virtual void requestVolumePathWithMultipleGoals(const hkVector4* goals, int numGoals, int priority = 0) = 0;

		/// Number of managed characters.
	virtual int getNumCharacters() const = 0;

		/// Return a managed character.
	virtual hkaiCharacter* getCharacter(int index) = 0;

		/// Add managed characters to the given array. The default implementation uses
		/// getNumCharacters and getCharacter.
	virtual void getCharacters(hkArray<hkaiCharacter*>::Temp& characters);

		/// Return whether the character is close enough to the goal that repathing
		/// at this point would not be necessary.
	virtual bool isNearGoal() const = 0;

		/// Request a new path. This should be equivalent to calling
		/// requestPathWithMultipleGoals with the previous goals.
	virtual void repath(int priority = 0) = 0;

		/// Cancel current requested path.
	virtual void cancelRequestedPath() = 0;

		/// Inform the behavior that it should temporarily stop controlling its character(s).
		/// For single character behaviors, it is an error to call this on a character which is
		/// already being manually controlled.
		///
		/// \param characterIdx The index of the character to stop controlling; by default,
		///        stops controlling all characters managed by this behavior.
	virtual void takeManualControl(int characterIdx = -1) = 0;

		/// Inform the behavior that it should resume controlling its character(s). For single
		/// character behaviors, it is an error to call this on a character which is not being
		/// manually controlled.
		///
		/// \param characterIdx The index of the character to resume controlling; by default,
		///        resumes controlling all characters managed by this behavior which are under
		///        manual control.
	virtual void releaseManualControl(int characterIdx = -1) = 0;

	HK_FORCE_INLINE hkaiWorld* getWorld() { return m_world; }

protected:

	hkRefPtr<hkaiWorld> m_world;
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
