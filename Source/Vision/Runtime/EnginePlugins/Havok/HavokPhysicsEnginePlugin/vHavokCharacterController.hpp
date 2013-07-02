/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vHavokCharacterController.hpp

#ifndef vHavokCharacterController_HPP_INCLUDED
#define vHavokCharacterController_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsIncludes.hpp>

#include <Physics2012/Dynamics/Phantom/hkpSimpleShapePhantom.h>
#include <Physics2012/Utilities/CharacterControl/CharacterProxy/hkpCharacterProxy.h>
#include <Physics2012/Utilities/CharacterControl/StateMachine/hkpDefaultCharacterStates.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/CharacterControlStates/vCharacterInput.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/CharacterControlStates/vCharacterStateClimbing.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/CharacterControlStates/vCharacterStateFlying.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/CharacterControlStates/vCharacterStateInAir.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/CharacterControlStates/vCharacterStateJumping.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/CharacterControlStates/vCharacterStateOnGround.h>

class VTransitionStateMachine;
class vHavokCharacterController;

/// Serialization versions
#define VHAVOKCHARACTERCONTROLLER_VERSION_0           0     // Initial version
#define VHAVOKCHARACTERCONTROLLER_VERSION_1           1     // More exposed properties
#define VHAVOKCHARACTERCONTROLLER_VERSION_2           2     // Switchable motion delta Z behavior
#define VHAVOKCHARACTERCONTROLLER_VERSION_3           3     // Gravity_Scale
#define VHAVOKCHARACTERCONTROLLER_VERSION_4           4     // Jump_Height
#define VHAVOKCHARACTERCONTROLLER_VERSION_5           5     // Step_Height, Step_Threshold
#define VHAVOKCHARACTERCONTROLLER_VERSION_6           6     // Enabled state
#define VHAVOKCHARACTERCONTROLLER_VERSION_CURRENT     6     // Current version

#if defined(HAVOK_SDK_VERSION_MAJOR) && (HAVOK_SDK_VERSION_MAJOR >= 2012)
typedef hkSimplexSolverInput hkpSimplexSolverInput;
#endif

#ifndef _VISION_DOC
#if defined (__SNC__)
#pragma diag_push
#pragma diag_suppress=1011
#endif

class vHavokCharacterPushableProxy : public hkpCharacterProxy, public hkpCharacterProxyListener
{
public:

	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CHARACTER);
	
	vHavokCharacterPushableProxy( const hkpCharacterProxyCinfo& info, const vHavokCharacterController *pOwner, hkReal strength = 1.0f );

	virtual ~vHavokCharacterPushableProxy();

	virtual void processConstraintsCallback( const hkpCharacterProxy* proxy, const hkArray<hkpRootCdPoint>& manifold, hkpSimplexSolverInput& input );

	bool handleSteps( const vHavokCharacterController* character, const hkpCharacterInput& input, hkpCharacterOutput& output );
	bool isOnStep() const { return m_onStepCnt > 0; }

	void drawDebug( const vHavokCharacterController* character ) const;

	hkReal m_strength;	// Relative push strength
	int m_onStepCnt;	// indicates if handleStep() projected the controller onto a step
	class DebugHavokCharacterController* m_visDebug;
};

#if defined (__SNC__)
#pragma diag_pop
#endif // snc
#endif // docs


///
/// \brief
///   Havok Character Controller
///
///
///
///
///
class vHavokCharacterController : public IVObjectComponent
{
public:

  ///
  /// @name Constructor / Destructor
  /// @{
  ///


  ///
  /// \brief
  ///   Default constructor. Sets up the input map.
  ///
  /// The actual initialisation of the component happens in the ::SetOwner function
  ///
  VHAVOK_IMPEXP vHavokCharacterController();


  ///
  /// \brief
  ///   Destructor
  ///
  VHAVOK_IMPEXP ~vHavokCharacterController();


  ///
  /// @}
  ///


  ///
  /// @name IVObjectComponent Virtual Overrides
  /// @{
  ///


  ///
  /// \brief
  ///   Overridden function to respond to owner changes.
  ///
  /// By setting the owner of this component the HavokRigidBody object will be 
  /// added to the Havok World and gets registered in the Havok module. 
  ///
  /// \param pOwner
  ///   The owner of this component.
  /// 
  /// \remarks
  ///   SetOwner(NULL) removes the HavokRigidBody object from the Havok World and
  ///   gets unregistered in the Havok module.
  /// 
  VHAVOK_IMPEXP VOVERRIDE void SetOwner(VisTypedEngineObject_cl *pOwner);


  ///
  /// \brief
  ///   Overridden function to determine if this component can be attached to a given object.
  ///
  /// The vHavokRigidBody component can be attached to VisObject3D_cl instances.
  /// 
  /// \param pObject
  ///   Possible owner candidate.
  /// 
  /// \param sErrorMsgOut
  ///   Reference to error message string.
  /// 
  /// \returns
  ///   TRUE if this component can be attached to the given object, FALSE otherwise.
  /// 
  VHAVOK_IMPEXP VOVERRIDE BOOL CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut);


  ///
  /// \brief
  ///   Overridden function to respond to variable changes.
  /// 
  /// \param pVar
  ///   Pointer to the variable object to identify the variable.
  /// 
  /// \param value
  ///   New value of the variable
  /// 
  VHAVOK_IMPEXP VOVERRIDE void OnVariableValueChanged(VisVariable_cl *pVar, const char * value);


  ///
  /// @}
  ///


  ///
  /// @name VTypedObject Virtual Overrides
  /// @{
  ///

  #ifdef WIN32

  ///
  /// \brief
  ///   Overridable that is called by the editor to retrieve per-instance variable display hints. 
  ///
  /// \param pVariable
  ///   Variable to retrieve dynamic display hints for.
  ///
  /// \param destInfo
  ///   Structure that can be modified to affect the editor's displaying 
  ///   of the respective variable (read-only, hidden).
  ///
  VHAVOK_IMPEXP virtual void GetVariableAttributes(VisVariable_cl *pVariable, VVariableAttributeInfo &destInfo) HKV_OVERRIDE;
  #endif

  ///
  /// \brief
  ///   The message function has been overridden to forward collision events to a script 
  VHAVOK_IMPEXP VOVERRIDE void MessageFunction( int iID, INT_PTR iParamA, INT_PTR iParamB );


  ///
  /// @}
  ///


  ///
  /// @name Serialization
  /// @{
  ///

  /// \brief
  ///   RTTI macro
  V_DECLARE_SERIAL_DLLEXP(vHavokCharacterController, VHAVOK_IMPEXP)

  /// \brief
  ///   RTTI macro to add a variable table
  V_DECLARE_VARTABLE(vHavokCharacterController, VHAVOK_IMPEXP)

  /// \brief
  ///   Serialization function
  ///
  /// \param ar
  ///   Binary archive
  ///
  VHAVOK_IMPEXP VOVERRIDE void Serialize( VArchive &ar );

  /// \brief
  ///   Virtual overridable that gets called when a loading archive closes
  /// 
  VHAVOK_IMPEXP VOVERRIDE void OnDeserializationCallback(const VSerializationContext &context);

  /// \brief
  ///   Virtual overridable that gets called to determine if the object needs the OnDeserializationCallback
  /// 
  VHAVOK_IMPEXP VOVERRIDE VBool WantsDeserializationCallback(const VSerializationContext &context)
  {
    return TRUE;
  }

  ///
  /// @}
  ///


  ///
  /// @name Debug Rendering
  /// @{
  ///


  ///
  /// \brief
  ///   Enable / Disable debug rendering for this rigid body's shape.
  ///
  /// \param bEnable
  ///   If TRUE, the shape of this rigid body will be displayed in Vision.
  ///
  VHAVOK_IMPEXP void SetDebugRendering(BOOL bEnable);


  ///
  /// \brief
  ///   Set the color of the debug rendering representation of this rigid body's shape.
  ///
  /// \param color
  ///   New Color of the debug rendering representation.
  ///
  VHAVOK_IMPEXP void SetDebugColor(VColorRef color);


  ///
  /// @}
  ///


  ///
  /// @name Simulation
  /// @{
  ///

  ///
  /// \brief
  ///   Enables / Disables simulation.
  ///
  /// \param bEnabled
  ///   Enabled state to set.
  ///
  VHAVOK_IMPEXP void SetEnabled(BOOL bEnabled);

  ///
  /// \brief
  ///   Returns whether simulation is enabled;
  inline bool IsEnabled() const
  {
    return (m_bEnabled == TRUE);
  }

  
  ///
  /// \brief
  ///   After step, get the actual linear velocity achieved.
  ///
  /// This includes effect of gravity if unsupported.
  ///
  /// \param currentVelocity
  ///   Reference to velocity vector.
  ///
  VHAVOK_IMPEXP void GetCurrentLinearVelocity(hkvVec3& currentVelocity) const;


  ///
  /// \brief
  ///   Directly sets a position
  ///
  /// \param x
  ///   New position
  ///
  VHAVOK_IMPEXP void SetPosition(const hkvVec3& x);


  ///
  /// \brief
  ///   Returns the controller position
  ///
  VHAVOK_IMPEXP hkvVec3 GetPosition() const;

  ///
  /// \brief
  ///   Checks if the character is standing on the ground
  ///
  VHAVOK_IMPEXP bool IsStanding() const;

  ///
  /// \brief
  ///   Checks if the character is supported by world geometry in a given direction
  ///
  VHAVOK_IMPEXP bool CheckSupport(const hkvVec3& v) const;

  ///
  /// \brief
  ///   Set if the character should jump or not
  ///
  VHAVOK_IMPEXP void SetWantJump(bool wantJump);

  ///
  /// \brief
  ///   Set to true if the character should start flying. Set to false if the character should stop flying
  ///
  VHAVOK_IMPEXP void SetFlyState(bool bIsFlying);

  ///
  /// \brief
  ///   Step the character (note this locks the world, so should only do this outside of physics step).
  ///
  /// Will not update the owner, as otherwise this could not be executed in the background.
  /// UpdateOwner will be called by the Physics Module in the main thread of physics handling.
  ///
  /// \param fTimeStep
  ///   Fixed physics time step.
  ///
  /// \param iNumSteps
  ///   Number of steps to integrate.
  ///
  /// \param fDuration
  ///   Frame-interval of application.
  ///
  VHAVOK_IMPEXP VOVERRIDE void Step(float fTimeStep, int iNumSteps, float fDuration);

  /// \brief
  ///   Updates the owner of this component with the position and rotation calculated
  ///   by the animation and physics systems.
  VHAVOK_IMPEXP void UpdateOwner();

  /// \brief
  ///   Returns a pointer to the \c hkpCharacterProxy object used internally.
  /// \return
  ///   pointer to a \c hkpCharacterProxy object
  inline const vHavokCharacterPushableProxy* GetHavokController() const { return m_pCharacterProxy; }

  /// \brief
  ///   Returns a const pointer to the \c hkpCharacterContext object used internally.
  /// \return
  ///   const pointer to a \c hkpCharacterContext object
  inline const hkpCharacterContext* GetCharacterContext() const { return m_characterContext; }

  /// \brief
  ///   Updates the collision capsule based on the Character_Top, Character_Bottom and Capsule_Radius 
  /// members.
  VHAVOK_IMPEXP void UpdateBoundingVolume();

  /// \brief
  ///   Tries to set the collision capsule to the given endpoints.  The function succeeds whenever the
  /// resulting capsule doesn't intersect the world.
  ///
  /// \param capTop
  ///   First capsule axis endpoint
  ///
  /// \param capBottom
  ///   Second capsule axis endpoint
  VHAVOK_IMPEXP bool TryUpdateBoundingVolume(const hkvVec3& capTop, const hkvVec3& capBottom);

  /// \brief
  ///   Returns the current gravity scaling factor. The default value is 1.0f (normal gravity of the world)
  inline float GetGravityScaling() const {return Gravity_Scale;}

  /// \brief
  ///   Sets the scaling of the gravity relatively to the world's default gravity. The default value is 1.0f (normal gravity of the world). Passing 0.0f disables the gravity.
  inline void SetGravityScaling(float fValue)
  {
    Gravity_Scale=fValue;
  }

  /// \brief
  ///   Sets the collision parameters of this character controller, as per the hkpGroupFilter.
  /// \param iLayer
  ///   the collision layer
  /// \param iGroup
  ///   the collision group
  /// \param iSubsystem
  ///   the collision subsystem
  /// \param iSubsystemDontCollideWith
  ///   the collision subsystem this body shouldn't collide with
  /// \note
  ///   See the Havok documentation on rigid body collisions for more 
  ///   information about what values to specify for these parameters.
  VHAVOK_IMPEXP void SetCollisionInfo(int iLayer, int iGroup, int iSubsystem, int iSubsystemDontCollideWith);

  /// \brief
  ///   Sets the collision filter of this character controller.
  /// \param iCollisionFilter
  ///   the collision filter as computed by hkpGroupFilter::calcFilterInfo
  /// \note
  ///   See the Havok documentation on rigid body collisions for more 
  ///   information about what values to specify for these parameters.
  VHAVOK_IMPEXP void SetCollisionInfo(int iCollisionFilter);

  ///
  /// @}
  ///


  //
  // Members exposed to vForge:
  //
  float Capsule_Radius;                  ///< Radius of the character controller capsule.
  hkvVec3 Character_Top;                 ///< Top of the character controller capsule.
  hkvVec3 Character_Bottom;              ///< Bottom of the character controller capsule.
  float Static_Friction;                 ///< Default static friction for surfaces hit.
  float Dynamic_Friction;                ///< Default dynamic friction for surfaces hit.
  float Max_Slope;                       ///< Maximum slope that the character can walk up (in degrees, max 90 deg).
  float Character_Mass;                  ///< Mass of the character, standing on things.
  float Character_Strength;              ///< Maximum const force that the character controller can impart onto moving objects.
  float Gravity_Scale;                   ///< Scalar factor to scale the global gravity strength. Default is 1.0
  float Jump_Height;                     ///< Jump Height of the character controller. Default is 1.5
  float Step_Height;                     ///< Maximum height of step that can be climbed.
  float Step_Threshold;                  ///< Step identity threshold.

  /// \brief Returns whether debug rendering is enabled.
  inline bool GetDebugRenderEnabled () const { return Debug != FALSE; }

public:
  ///
  /// \brief
  ///   Creates the actual Havok character controller and adds it to the simulation.
  ///
  VHAVOK_IMPEXP void CreateHavokController();

  ///
  /// \brief
  ///   Removes object from the simulation and releases the reference.
  ///
  VHAVOK_IMPEXP void DeleteHavokController();

  VHAVOK_IMPEXP vHavokCharacterPushableProxy* GetCharacterProxy(); 

  ///
  /// \brief
  ///   Creates state machine for character controller, You can create new state machine by overriding the method.
  ///
  VHAVOK_IMPEXP VOVERRIDE void CreateStateMachineAndContext();

  //
  // Members not exposed to vForge:
  //
protected:
  vHavokCharacterPushableProxy *m_pCharacterProxy; ///< Proxy of the Havok character controller
  hkpCharacterContext* m_characterContext; 
  hkvVec3 m_currentVelocity;          ///< Current velocity of the controller (in Vision units)
  bool m_wantJump;
  vCharacterInput m_characterInput;
  int m_iCollisionFilter;
  
  //
  // Members exposed to vForge:
  //
  BOOL m_bEnabled;                      ///< Determines if character controller is simulated.
  BOOL Debug;                           ///< Whether debug rendering is active or not
  VColorRef DebugColor;                 ///< Debug color of the physical RB representation
  BOOL Fly_State;                       ///< Whether character should fly or not

private:

  // Applies entity scale to specified values, validates those values, returns the
  // worldTransform matrix of the entity 
  hkvMat4 ApplyEntityScale(hkVector4 &vTop, hkVector4 &vBottom, float &fRadius);
};





#endif // vHavokCharacterController_HPP_INCLUDED

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
