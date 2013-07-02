/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HK_AI_LOCAL_STEERING_INPUT_H
#define HK_AI_LOCAL_STEERING_INPUT_H

#include <Ai/Pathfinding/Common/hkaiReferenceFrame.h>
#include <Common/Base/DebugUtil/DeterminismUtil/hkCheckDeterminismUtil.h>

class hkaiCharacter;
struct hkaiAvoidanceProperties;
struct hkaiPathFollowingCharacterState;

extern const hkClass hkaiLocalSteeringInputClass;

	/// Input values for the local steering calculation
struct hkaiLocalSteeringInput
{
	//+version(6)
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_STEERING,hkaiLocalSteeringInput);
	HK_DECLARE_REFLECTION();
	HK_DECLARE_POD_TYPE();

		/// Constructor
		/// \note This constructor does not initialize any fields
	inline hkaiLocalSteeringInput();
	inline hkaiLocalSteeringInput(hkFinishLoadedObjectFlag f);

		/// Initialize the data to default values
	void init(hkaiCharacter* character);

		/// Initialize the data to default values, including reference frame and character state (pos/vel/fwd/up).
	void init(
		hkaiCharacter* character, 
		hkaiReferenceFrame const* referenceFrame, 
		hkaiPathFollowingCharacterState const& characterState);

		/// Validate this input data
	inline void validate( bool callCheckDeterminism = true ) const;
	
		/// Validate determinism of data. Does nothing unless determinism checks enabled.
	inline void checkDeterminism() const;


		/// Current position of the character (in the frame of reference of the character).
	hkVector4 m_currentPosition;

		/// Current forward direction of the character (in the frame of reference of the character).
		/// This is used to constrain angular velocity, and (in the case of the penalty forces-based
		/// solver) to determine which neighbors are visible to the character. This should be
		/// perpendicular to m_currentUp, even if the agent is on a slope.
	hkVector4 m_currentForward;

		/// Current up direction of the character (in the frame of reference of the character). 
	hkVector4 m_currentUp;

		/// Current velocity of the character (in the frame of reference of the character).
		/// The XYZ contains the linear velocity, the W component contains the angular velocity.
	hkVector4 m_currentVelocity;

		/// Desired linear velocity for the character (in the frame of reference of the character).
		/// Local steering will try to achieve this velocity while avoiding obstacles.
	hkVector4 m_desiredVelocity;

		/// The direction of the character's desired velocity after it's next path segment
		/// (in the frame of reference of the character). When using the penalty forces-based solver,
		/// this influences the direction that a character will turn if it needs to avoid an obstacle.
	hkVector4 m_desiredFutureDir;

		/// An optional plane equation, expressed relative to the character's current position.
		/// If nonzero, the character's position should be on the positive half-plane (that is, W should
		/// be nonnegative). Characters will ignore any collisions which would take place after they crossed
		/// to the negative half-plane.
	hkVector4 m_localGoalPlane;

		/// The distance to the next steering target, e.g. the next point on the path. This is used to keep
		/// characters from avoiding obstacles which would take place after their next turn.
	hkReal m_distToLocalGoal;

		/// The expected time to reach the next steering target. This is used to keep
		/// characters from avoiding obstacles which would take place after their next turn.
	hkReal m_timeToLocalGoal;

		/// The character for which this local steering input has been generated.
		/// This pointer provides access to some of the fixed character properties, like movement properties.
	hkaiCharacter* m_character; //+nosave

		/// Reference frame in which the steering inputs are expressed.
		/// For standard path-following behaviors, this will be the frame of reference of the character.
	const hkaiReferenceFrame* m_referenceFrame;	//+nosave

	hkaiAvoidanceProperties const* m_avoidanceProperties; //+nosave

		/// Whether or not the next steering target is the final one, e.g. the end of the path. The penalty
		/// forces-based avoidance solver uses this to disregard collisions "behind" the goal.
	hkBool m_isLocalGoalLast;

		/// When set, kinematic constraints (min/max velocity etc.) will be applied to generate this 
		/// character's output velocity
	hkBool m_applyKinematicConstraints;

		/// When set, avoidance steering will be applied to generate this character's output velocity.
		/// Otherwise, only kinematic constraints (if enabled) will be applied, and otherwise the
		/// output velocity will be the desired velocity.
	hkBool m_applyAvoidanceSteering;

		/// If set to false, no other fields will be read except m_character, no local steering will be 
		/// performed, and no results will be written back to the hkaiCharacter.
	hkBool m_enableLocalSteering;
};

#include <Ai/Pathfinding/Character/LocalSteering/hkaiLocalSteeringInput.inl>

#endif	//HK_AI_LOCAL_STEERING_INPUT_H

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
