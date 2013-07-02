/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_AI_STEERING_MODIFIER_H
#define HKB_AI_STEERING_MODIFIER_H

#include <Behavior/Behavior/hkbBehavior.h>
#include <Behavior/Behavior/Modifier/hkbModifier.h>
#include <Behavior/Behavior/Modifier/AiControl/hkbAiControlControlData.h>
#include <Ai/Pathfinding/Character/hkaiCharacter.h>

extern const class hkClass hkbAiSteeringModifierClass;

/// The output interface by which Havok AI controls variables and events in
/// Havok Behavior.
/// 
/// This modifier should be placed near the root of the behavior graph, and
/// ordinarily be active during the character's entire lifetime if the
/// character is AI-controlled. The character will only have a representation
/// in the AI world while this modifier is active. There should be at most one
/// AI steering modifier active at a time.
class hkbAiSteeringModifier : public hkbModifier
{
public:
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
	HK_DECLARE_REFLECTION();

	hkbAiSteeringModifier();
	~hkbAiSteeringModifier();
	hkbAiSteeringModifier(hkFinishLoadedObjectFlag flag);

protected:
	hkbAiSteeringModifier(hkbAiSteeringModifier const& other);

public:

	//////////////////////
	// hkbNode interface 
	//////////////////////

	// hkbNode interface implementation.
	virtual void activate( const hkbContext& context ) HK_OVERRIDE;

	// hkbNode interface implementation.
	virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

	// hkbNode interface implementation.
	virtual void deactivate( const hkbContext& context ) HK_OVERRIDE;

	// hkbNode interface implementation.
	virtual hkbAiSteeringModifier* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

	virtual bool isValid(const hkbCharacter* character, hkStringPtr& errorString) const HK_OVERRIDE;


	//////////////////////////
	// hkbModifier interface
	//////////////////////////

	// hkbModifier interface implementation
	virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

	/// @name Event notification interface used by hkbAiControlDriver.
	/// Because the lifetime of a character's presence in the AI world may
	/// extend over the lifetimes of multiple hkbAiSteeringModifiers (if, for
	/// example, one is deactivated and another is activated on the same frame)
	/// the hkbAiSteeringModifier itself cannot be the BehaviorListener for the
	/// character's hkaiCharacter. Instead, the hkbAiControlDriver serves as
	/// sthe BehaviorListener, and forwards events to the currently active
	/// hkbAiSteeringModifier. When it does, it also passes in the hkbCharacter
	/// which owns this modifier.
	/// @{

	/// Notification of BehaviorListener::goalReached
	void notifyGoalReached(hkbCharacter * character);

	/// Notification of BehaviorListener::pathSucceeded
	void notifyPathSucceeded(hkbCharacter * character);

	/// Notification of hkbAiControlDriver::cancelPath (which has no associated
	/// BehaviorListener event)
	void notifyPathCancelled(hkbCharacter * character);

	/// Notification of BehaviorListener::userEdgeEntered
	void notifyUserEdgeEntered(hkbCharacter * character, hkaiCharacter::BehaviorListener::UserEdgeEntry const& userEdgeEntry);

	/// @}

public:

	struct SteeringInfo
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbAiSteeringModifier::SteeringInfo );
		HK_DECLARE_REFLECTION();

		SteeringInfo() { reset(); }
		SteeringInfo(hkFinishLoadedObjectFlag flag) { }

		void reset();

		hkVector4 m_requestedVelocity;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
		//+hk.Description("The current world-space velocity the character should try to achieve.")

		hkReal m_forwardSpeed;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
		//+hk.Description("The speed relative to the character's forward vector it should try to achieve.")

		hkReal m_rightSpeed;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
		//+hk.Description("The speed relative to the character's right vector it should try to achieve.")

		hkReal m_totalSpeed;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
		//+hk.Description("The current velocity magnitude the character should try to achieve.")

		hkReal m_localAngle;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
		//+hk.Description("The counterclockwise angle between the character's forward vector and its requested velocity.")

	};

	SteeringInfo m_steeringInfo; //+hk.Description("Information about the AI system's current requested velocity for the character")

	struct UserEdgeInfo
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbAiSteeringModifier::UserEdgeInfo );
		HK_DECLARE_REFLECTION();

		UserEdgeInfo() { reset(); }
		UserEdgeInfo(hkFinishLoadedObjectFlag flag) { }

		void reset();

		hkBool m_hasEntryInfoOut; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT") //+hk.Description("Whether a user edge entry has occurred, and valid data is available.")
		hkInt32 m_entryUserDataOut; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT") //+hk.Description("The first word of userdata attached to the entry edge.")
		hkVector4 m_entryPointOut; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT") //+hk.Description("The world-space position of the closest point on the entry edge.")
		hkVector4 m_exitPointOut; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT") //+hk.Description("The world-space position of the point on the exit edge corresponding to the entry point.")
		hkVector4 m_entryEdgeDirection; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT") //+hk.Description("The normalized direction pointing along the entry edge.")
		hkVector4 m_exitEdgeDirection; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT") //+hk.Description("The normalized direction pointing along the exit edge.")
		hkReal m_localAngleToExitPointOut; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT") //+hk.Description("The angle between the character's forward vector and the vector to the exit point.")
		hkReal m_localAngleToTraversalDirectionOut; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT") //+hk.Description("The angle between the character's forward vector and the vector to the exit point.")
	};

	UserEdgeInfo m_userEdgeInfo; //+hk.Description("Output information about the most recently crossed user edge.")


	/// A rule for invoking an event when a character reaches a user edge which
	/// is part of their current path. If the bitwise AND of the user edge's
	/// first data element and m_userdataMask matches m_userdataId, the event
	/// specified by m_event will be raised. Only the first matching rule will
	/// be used.
	struct UserEdgeEvent
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbAiSteeringModifier::UserEdgeEvent );
		HK_DECLARE_REFLECTION();

		UserEdgeEvent()
			: m_userdataId(0)
			, m_userdataMask(0xFFFFFFFF)
		{ }

		UserEdgeEvent(hkFinishLoadedObjectFlag flag) 
			: m_event(flag)
		{ }

		hkUint32 m_userdataId;		//+default(0)
		//hk.Description("The ID to check the user edge's data against.")
		hkUint32 m_userdataMask;	//+default(0xFFFFFFFF)
		//hk.Description("A bitmask to filter the userdataId check.")
		hkbEventProperty m_event;	//hk.desription("The event to send when the user edge is entered.")
	};

	struct EventMapping
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbAiSteeringModifier::EventMapping );
		HK_DECLARE_REFLECTION();

		EventMapping() { }
		EventMapping(hkFinishLoadedObjectFlag flag) 
			: m_startPath(flag)
			, m_goalReached(flag)
			, m_pathCancelled(flag)
			, m_userEdgeEvents(flag) { }
		EventMapping(EventMapping const& other);

		hkbEventProperty m_startPath;	//hk.description("The event to send when a character is given a new path.")

		hkbEventProperty m_goalReached;	//hk.description("The event to send when a character reaches its goal.")

		hkbEventProperty m_pathCancelled;	//hk.description("The event to send when a character's current path is cancelled.")

		hkArray<UserEdgeEvent> m_userEdgeEvents;	//hk.Description("User edge types to use to emit events.")
	};

	EventMapping m_eventMapping; //hk.description("Mappings from AI-generated events to Behavior events")


	/// Saveable version of hkaiCharacter::BehaviorListener::UserEdgeEntry for use in internal state
	struct InternalUserEdgeInfo
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbAiSteeringModifier::InternalUserEdgeInfo );
		HK_DECLARE_REFLECTION();

		InternalUserEdgeInfo() { }
		InternalUserEdgeInfo(hkFinishLoadedObjectFlag const& flag) { }

		void init(hkaiCharacter::BehaviorListener::UserEdgeEntry const& entry);
		void fillTempEntry(hkaiCharacter::BehaviorListener::UserEdgeEntry & entry, hkaiWorld* aiWorld) const;
		/// The runtime ID of the nav mesh instance the entry edge is on
		hkaiRuntimeIndex m_entrySection;

		/// The point on the user edge being entered, in instance-local coordinates
		hkVector4 m_entryPoint;

		/// The first word of userdata attached to the user edge being entered.
		hkaiNavMesh::EdgeData m_entryData;

		/// The runtime ID of the nav mesh instance the exit edge is on
		hkaiRuntimeIndex m_oppositeSection;

		/// The tangent direction of the entry edge, pointing from point A to point B, in the 
		/// space of the entry edge section. This may not be normalized.
		///
		/// Only edge-based steering correctly fills this field. If path-based steering is 
		/// used, this will be set to zero.
		hkVector4 m_entryEdgeDirection;

		/// If edge-based steering is used, a transformation which maps points 
		/// on the exit edge (in the space of the exit edge section) to points on the entry edge
		/// (in the space of the entry edge section). If path-based steering is used, the
		/// transform will map the exit point to the entry point, but is not otherwise valid.
		hkMatrix4 m_edgeTransform;
	};

	hkbAiControlControlData m_defaultControlData; //hk.description("AI control data used when no AI Control modifier is active")

protected:
	/// Fill m_steeringInfo from AI output
	void updateSteeringInfo(const hkbContext &context);

	/// Fill m_userEdgeInfo from AI output
	void updateUserEdgeInfo(const hkbContext &context);
	void updateUserEdgeInfo(hkbCharacter* character);

private:


	HKB_BEGIN_INTERNAL_STATE(0);

	hkVector4 m_requestedVelocityInternal; //+nosave

	/// Whether m_lastUserEdgeEntry has been filled.
	hkBool m_hasLastUserEdgeEntry; //+nosave

	hkbAiSteeringModifier::InternalUserEdgeInfo m_lastUserEdgeEntry; //+nosave

	HKB_END_INTERNAL_STATE();
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
