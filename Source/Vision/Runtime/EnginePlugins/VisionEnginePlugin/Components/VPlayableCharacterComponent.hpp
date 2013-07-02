/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VPlayableCharacterComponent.hpp

#ifndef VPLAYABLECHARACTERCOMPONENT_HPP_INCLUDED
#define VPLAYABLECHARACTERCOMPONENT_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Animation/Transition/AnimationModule.hpp>

class VTransitionStateMachine;
class VVirtualThumbStick;

/// \brief
///   Object component class that can be used to get a playable character (controlled via keyboard)
///   up and running very quickly, e.g. for rapid prototyping.
///
/// The playable character component can be used to quickly get a playable character up and running
/// in your game, mostly for prototyping purposes. It is not meant to be used for a final game.
///
/// Requirements
///  - The owner entity needs to have a physics object to process motion/rotation delta. This can e.g.
///    be done by adding a vHavokRigidBody component to the owner entity.
///  - The owner entity needs a VTransitionStateMachine component, since the state machine is used for
///    animation playback.
///
/// The component provides parameters for the various animations (walk, run, idle). The animation playback
/// is performed with a transition state machine (VTransitionStateMachine). The WASD and SHIFT keys are
/// automatically mapped to move the character in the scene. 
///
class VPlayableCharacterComponent : public IVObjectComponent, public IVisCallbackHandler_cl
{
public:
  V_DECLARE_SERIAL_DLLEXP( VPlayableCharacterComponent , ANIMATION_IMPEXP ); // for RTTI
  V_DECLARE_VARTABLE(VPlayableCharacterComponent, ANIMATION_IMPEXP ); // This class should have a variable list

  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  /// \brief
  ///   Default constructor. Sets up the input map.
  ///
  /// The actual initialisation of the component happens in the ::SetOwner function
  ///
  VPlayableCharacterComponent();

  /// \brief
  ///   Destructor
  ~VPlayableCharacterComponent();

  ///
  /// @}
  ///

  ///
  /// @name Component System Overrides
  /// @{
  ///

  ///
  /// \brief
  ///   Overridden function. Sets the new owner of the components.
  ///
  virtual void SetOwner(VisTypedEngineObject_cl *pOwner);

  /// \brief
  ///   Overridden function. Ensures that this component can only be attached to entities
  ///   with a transition state machine component attached.
  ///
  virtual BOOL CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut);

  ///
  /// \brief
  ///   Serializes the instance to the passed archive
  ///
  virtual void Serialize(VArchive &ar);

  ///
  /// @}
  ///


  ///
  /// @name Update Methode
  /// @{
  ///

  ///
  /// \brief
  ///   Updates the the state machine for the current frame.
  /// 
  /// The PerFrameUpdate() function takes care of processing the input map (keyboard events)
  /// and updating the transition state machine and object transformation accordingly.
  ///
  /// \note
  ///   You do not have to call this function manually, since the VPlayableCharacterComponentManager
  ///   class will take care of this.
  ///
  void PerFrameUpdate();

  ///
  /// @}
  ///

  ///
  /// @name Playable Character Properties
  /// @{
  ///

  ///
  /// \brief
  ///   Pause character.
  ///
  void SetIsPaused(bool bPaused){ m_bIsPaused = bPaused; }

  ///
  /// @}
  ///

  /// \brief
  ///   Callback handler implementation.
  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

protected:
  /// \brief
  ///   Gets the transition state machine component of the owner entity
  ///
  VTransitionStateMachine *GetStateMachine();
  
  /// \brief
  ///   Sets the passed state in the transition state machine of the owner
  ///
  void SetState(VString &sNewState);

  VString IdleAnimation;      ///< Name of the idle animation; exposed to vForge
  VString WalkForwardAnim;    ///< Name of the walk forward animation; exposed to vForge
  VString WalkBackwardAnim;   ///< Name of the walk backward animation; exposed to vForge
  VString RunAnimation;       ///< Name of the run backward animation; exposed to vForge

  VInputMap *m_pInputMap;     ///< Input map for the character's keyboard/gamepad controls

  bool m_bNoPhysicsWarningShown;   ///< Used to remember whether a warning about missing physics object has already been added to the log

  bool m_bIsPaused;           ///< Pause character

private:
#if defined(SUPPORTS_MULTITOUCH)
  VVirtualThumbStick* m_pVirtualThumbStick;
#endif
};


/// \brief
///   Collection for handling playable character component
class VPlayableCharacterComponentCollection : public VRefCountedCollection<VPlayableCharacterComponent>
{
};


/// \brief
///   Manager for all VPlayableCharacterComponent instance
///
/// This manager class has a list of all available VPlayableCharacterComponent instances
/// and takes care of calling their VPlayableCharacterComponent::PerFrameUpdate function
/// on each frame.
class VPlayableCharacterComponentManager : public IVisCallbackHandler_cl
{
public:
  /// \brief
  ///   Gets the singleton of the manager
  static VPlayableCharacterComponentManager &GlobalManager()
  {
    return g_GlobalManager;
  }

  /// \brief
  ///   Should be called at plugin initialization time
  void OneTimeInit()
  {
    Vision::Callbacks.OnUpdateSceneFinished += this; // listen to this callback
  }

  /// \brief
  ///   Should be called at plugin de-initialization time
  void OneTimeDeInit()
  {
    Vision::Callbacks.OnUpdateSceneFinished -= this; // de-register
  }

  /// \brief
  ///   Callback method that takes care of updating the managed stances each frame
  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData)
  {
    VASSERT(pData->m_pSender==&Vision::Callbacks.OnUpdateSceneFinished);

    // call update function on every component
    const int iCount = m_Components.Count();
    for (int i=0;i<iCount;i++)
      m_Components.GetAt(i)->PerFrameUpdate();
  }

  /// \brief
  ///   Gets all VPlayableCharacterComponent instances this manager holds
  inline VPlayableCharacterComponentCollection &Instances() { return m_Components; }

protected:
  /// Holds the collection of all instances of MyComponent
  VPlayableCharacterComponentCollection m_Components;

  /// One global instance of our manager
  static VPlayableCharacterComponentManager g_GlobalManager;
};




#endif // VPLAYABLECHARACTERCOMPONENT_HPP_INCLUDED

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
