/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_AI_CONTROL_DRIVER_H
#define HKB_AI_CONTROL_DRIVER_H

#include <Ai/Pathfinding/Character/hkaiCharacter.h>

class hkaiCharacter;
class hkaiSingleCharacterBehavior;
class hkbContext;
class hkbCharacter;
class hkbWorld;
class hkaiWorld;
class hkbHandle;
class hkbAiControlDriverInfo;
class hkbAiSteeringModifier;
class hkbAiObjectFactory;
class hkbAiBridge;

/// A driver used to manage an HKAI representation of a hkbCharacter, and to 
/// transfer state between the two.
/// 
/// The hkbAiControlDriver for a hkbCharacter is created by the hkbAiBridge in
/// response to the character being activated in the hkbWorld. It is only
/// created if the character has a hkbAiControlDriverInfo set in its 
/// hkbCharacterData, and that data is used to control the driver.
class hkbAiControlDriver : public hkReferencedObject, private hkaiCharacter::BehaviorListener
{
public:
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );

	/// Constructor. Does not create the HKAI-side representation; this is done
	/// later when addToWorld is called. No other methods should be called
	/// until the driver is set on the character via 
	/// hkbCharacter::setAiControlDriver().
	hkbAiControlDriver(hkbAiBridge* bridge, hkbCharacter* bCharacter);

	~hkbAiControlDriver();

	/// @name Main interface
	/// @{

	/// Returns whether an HKAI-side representation for the character has been 
	/// created.
	bool isInWorld() const;

	/// Returns whether there is an active AI steering modifier on the 
	/// character, meaning the character should be placed in the world if it
	/// is not already.
	bool shouldBeInWorld() const;

	
	

	/// Requests the hkaiWorld to generate a path to the goal for the 
	/// character. This is intended to be called from user code.
	virtual void pathTo(hkVector4Parameter goal); 

	/// Requests the hkaiWorld to cancel the character's current path. This is
	/// intended to be called from user code.
	virtual void cancelPath(); 

	/// @}
	

	/// @name Interface used by control quality testing; should not be used
	/// in production code
	/// @{

	/// Disallow AI from controlling the character, even when AI control is
	/// enabled. This should ONLY be used for character testing.
	void setForceManualControl(bool forceManualControl);

	/// Update the manually requested velocity, which is used if 
	/// forceManualControl is enabled. This should ONLY be used for character 
	/// testing.
	void setManuallyRequestedVelocity(hkVector4Parameter manualRequestedVelocity);

	/// Get the character position as of the last call to transferStateToAi().
	hkVector4 const& getPrevPos() const { return m_prevPos; }

	/// Get the character velocity as of the last call to transferStateToAi().
	hkVector4 const& getPrevVel() const { return m_prevVel; }

	/// Cause a pathSucceeded event to be propagated to Behavior.
	void injectPathSucceeded();

	/// Cause a goalReached event to be propagated to Behavior.
	void injectGoalReached();

	/// @}


	/// @name Interface used by hkbAiBridge internals
	/// @{
	
	/// Instructs the driver to scan the active nodes of the behavior graph for
	/// a hkbAiSteeringModifier, and add it as its active steering modifier.
	/// This is useful for finding steering modifiers which were activated
	/// before the driver was created and assigned to the hkbCharacter.
	void handleInitiallyActiveSteeringModifier();
	
	/// Called by newly activated hkbAiSteeringModifiers.
	void reportSteeringModifierActivated(hkbAiSteeringModifier* steeringModifier);

	/// Called by deactivated hkbAiSteeringModifers.
	void reportSteeringModifierDeactivated(hkbAiSteeringModifier* steeringModifier);

	/// Return the velocity requested by AI for the character to achieve, as of
	/// the last call to transferStateFromAi().
	hkVector4 const& getRequestedVelocity() const { return m_requestedVelocity; }

	/// Return the hkbAiBridge which created and manages this driver.
	hkbAiBridge* getBridge() const { return m_bridge; }

	/// Return the currently active steering modifier, if any.
	hkbAiSteeringModifier* getSteeringModifier() const { return m_steeringModifier; }

	/// Return the HKAI-side character.
	hkaiCharacter* getAiCharacter() const { return m_aiCharacter; }

	/// Return the HKAI-side behavior.
	hkaiSingleCharacterBehavior* getAiBehavior() const { return m_aiBehavior; }

	/// Return the hkaiWorld held by the hkbAiBridge.
	hkaiWorld* getAiWorld() const;

	/// Create the HKAI-side representation for the character, and the state
	/// handle (if required). This must not be called more than once without 
	/// an intervening call to reset(), and must be called before 
	/// transferStateToAi() or transferStateFromAi() is called.
	void addToWorld(hkbWorld* bWorld, hkaiWorld* aiWorld, hkbAiObjectFactory* objectFactory);

	/// Update state and control data for the HKAI-side representation, based
	/// on the character transform and on track data set by AI modifiers.
	void transferStateToAi(const hkbContext& context, hkReal timestep);

	/// Update the requested velocity from the HKAI-side representation.
	void transferStateFromAi();

	/// Destroy the HKAI-side representation and the state handle.
	void reset();

	/// @}


private:
	/// @name hkaiCharacter::BehaviorListener callbacks
	/// @{

	virtual void goalReached(hkaiBehavior* behavior, hkaiCharacter* character) HK_OVERRIDE;
	virtual void pathSucceeded(hkaiBehavior* behavior, hkaiCharacter* character) HK_OVERRIDE;
	virtual void userEdgeEntered(hkaiBehavior* behavior, hkaiCharacter* character, UserEdgeEntry const& userEdgeEntry) HK_OVERRIDE;

	/// @}

private:
	/// Return the driver info held in the character's setup data.
	hkbAiControlDriverInfo const* getInfo() const;

	/// Find an AI steering modifier which became active before the driver was created
	hkbAiSteeringModifier* findActiveSteeringModifier() const;

	/// The bridge which created and manages this driver. This is a bare 
	/// pointer to avoid cyclic refs.
	hkbAiBridge* const m_bridge;

	/// The HKB-side character whose driver this is. That character should have
	/// this driver in its m_aiControlDriver.
	hkbCharacter* m_bCharacter;

	hkRefPtr<hkbAiSteeringModifier> m_steeringModifier;

	/// The HKAI-side character. Created by addToWorld(). 
	hkRefPtr<hkaiCharacter> m_aiCharacter;

	/// The HKAI-side behavior. Created by addToWorld().
	hkRefPtr<hkaiSingleCharacterBehavior> m_aiBehavior;

	/// The handle which is reported as the character's position for AI 
	/// purposes. HK_NULL indicates the character controller's position should
	/// be used.
	hkRefPtr<hkbHandle> m_stateHandle;

	/// Whether m_prevPos has been initialized by calling transferStateToAi().
	hkBool m_prevPosValid;
	
	/// The character position as of the last call to transferStateToAi().
	hkVector4 m_prevPos;

	/// The character velocity as of the last call to transferStateToAi().
	hkVector4 m_prevVel;

	/// The velocity requested by AI for the character to achieve, as of the
	/// last call to transferStateFromAi().
	hkVector4 m_requestedVelocity;

	/// Ignore AI-requested velocity and never overwrite m_requestedVelocity;
	/// this is used only by the control quality tests.
	hkBool m_forceManualControl;


	hkbAiControlDriver(hkbAiControlDriver const&); // noncopyable
	hkbAiControlDriver& operator=(hkbAiControlDriver const&); // nonassignable
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
