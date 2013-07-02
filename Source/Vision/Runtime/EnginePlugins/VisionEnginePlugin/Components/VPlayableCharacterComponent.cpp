/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Components/VPlayableCharacterComponent.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Animation/Transition/VTransitionStateMachine.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Input/VVirtualThumbStick.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

///////////////////////////////////////////////////////////////////////////////
// Input Map
///////////////////////////////////////////////////////////////////////////////

enum INPUTMAP_CHARACTER {
  INPUTMAP_CHARACTER_WALK_FORWARD = 0,
  INPUTMAP_CHARACTER_WALK_BACKWARD,
  INPUTMAP_CHARACTER_WALK_RUN,
  INPUTMAP_CHARACTER_ROTATE_LEFT,
  INPUTMAP_CHARACTER_ROTATE_RIGHT,

  INPUTMAP_CHARACTER_LAST_ELEMENT = INPUTMAP_CHARACTER_ROTATE_RIGHT
};

#define INPUTMAP_CHARACTER_ALTERNATIVES   4
#ifndef _VISION_PSP2
#define ROTATION_DEGREES_PER_SECOND       180
#else
#define ROTATION_DEGREES_PER_SECOND       90
#endif



///////////////////////////////////////////////////////////////////////////////
// Constructor & Destructor
///////////////////////////////////////////////////////////////////////////////

// register the class in the engine module so it is available for RTTI
V_IMPLEMENT_SERIAL(VPlayableCharacterComponent,IVObjectComponent,0,&g_VisionEngineModule);

// one global instance of our manager
VPlayableCharacterComponentManager VPlayableCharacterComponentManager::g_GlobalManager;

VPlayableCharacterComponent::VPlayableCharacterComponent()
  : m_bNoPhysicsWarningShown(false)
  , m_bIsPaused(false)
#if defined(SUPPORTS_MULTITOUCH)
  , m_pVirtualThumbStick(NULL)
#endif
{
  m_pInputMap = new VInputMap(INPUTMAP_CHARACTER_LAST_ELEMENT + 1, INPUTMAP_CHARACTER_ALTERNATIVES);

#if defined (SUPPORTS_KEYBOARD)
  // Setup controls for playable character
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_WALK_FORWARD,  V_KEYBOARD, CT_KB_W);
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_WALK_BACKWARD, V_KEYBOARD, CT_KB_S);
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_WALK_RUN,      V_KEYBOARD, CT_KB_LSHIFT);
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_ROTATE_LEFT,   V_KEYBOARD, CT_KB_A);
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_ROTATE_RIGHT,  V_KEYBOARD, CT_KB_D);
#endif

#if defined (WIN32)

#elif defined (_VISION_XENON)
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_WALK_FORWARD,  V_XENON_PAD(0), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_WALK_BACKWARD, V_XENON_PAD(0), CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_ROTATE_LEFT,   V_XENON_PAD(0), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_ROTATE_RIGHT,  V_XENON_PAD(0), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_WALK_RUN,      V_XENON_PAD(0), CT_PAD_LEFT_THUMB);

#elif defined (_VISION_PS3)
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_WALK_FORWARD,  V_PS3_PAD(0), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_WALK_BACKWARD, V_PS3_PAD(0), CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_ROTATE_LEFT,   V_PS3_PAD(0), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_ROTATE_RIGHT,  V_PS3_PAD(0), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_WALK_RUN,      V_PS3_PAD(0), CT_PAD_LEFT_THUMB);

#elif defined(_VISION_PSP2)
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_WALK_FORWARD,  V_PSP2_PAD(0), CT_PAD_LEFT_THUMB_STICK_UP,   VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_WALK_BACKWARD, V_PSP2_PAD(0), CT_PAD_LEFT_THUMB_STICK_DOWN, VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_WALK_RUN,      V_PSP2_PAD(0), CT_PAD_LEFT_TRIGGER);
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_ROTATE_LEFT,   V_PSP2_PAD(0), CT_PAD_LEFT_THUMB_STICK_LEFT, VInputOptions::DeadZone(0.6f));
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_ROTATE_RIGHT,  V_PSP2_PAD(0), CT_PAD_LEFT_THUMB_STICK_RIGHT,VInputOptions::DeadZone(0.6f));

#elif defined(_VISION_WIIU)
  // DRC mapping
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_WALK_FORWARD,  VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_WALK_BACKWARD, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_ROTATE_LEFT,   VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_ROTATE_RIGHT,  VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_WALK_RUN,      VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB);

  // Pro controller mapping
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_WALK_FORWARD,  VInputManagerWiiU::GetRemote(0), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.1f));
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_WALK_BACKWARD, VInputManagerWiiU::GetRemote(0), CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.1f));
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_ROTATE_LEFT,   VInputManagerWiiU::GetRemote(0), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.1f));
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_ROTATE_RIGHT,  VInputManagerWiiU::GetRemote(0), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.1f));
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_WALK_RUN,      VInputManagerWiiU::GetRemote(0), CT_PAD_LEFT_THUMB);

#elif defined(SUPPORTS_MULTITOUCH)

  // Virtual thumb stick
  m_pVirtualThumbStick = new VVirtualThumbStick();

  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_WALK_FORWARD,  *m_pVirtualThumbStick, CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.2f));
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_WALK_BACKWARD, *m_pVirtualThumbStick, CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.2f));
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_ROTATE_LEFT,   *m_pVirtualThumbStick, CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.2f));
  m_pInputMap->MapTrigger(INPUTMAP_CHARACTER_ROTATE_RIGHT,  *m_pVirtualThumbStick, CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.2f));

#endif

  Vision::Callbacks.OnVideoChanged += this;
}

VPlayableCharacterComponent::~VPlayableCharacterComponent()
{
  V_SAFE_DELETE(m_pInputMap);

#if defined(SUPPORTS_MULTITOUCH)
  V_SAFE_DELETE(m_pVirtualThumbStick);
#endif

  Vision::Callbacks.OnVideoChanged -= this;
}

///////////////////////////////////////////////////////////////////////////////
// Component System Overrides
///////////////////////////////////////////////////////////////////////////////

void VPlayableCharacterComponent::SetOwner(VisTypedEngineObject_cl *pOwner)
{
  IVObjectComponent::SetOwner(pOwner);

  // Insert code here to respond to attaching this component to an object
  // This function is called with pOwner==NULL when de-attaching.
  if (pOwner!=NULL)
  {
    VPlayableCharacterComponentManager::GlobalManager().Instances().AddUnique(this);

    // Set idle state
    SetState(IdleAnimation);
  }
  else
    VPlayableCharacterComponentManager::GlobalManager().Instances().SafeRemove(this);
}

BOOL VPlayableCharacterComponent::CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
  if (!IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut))
    return FALSE;

  // We need transition state machine component to be functionable
  BOOL bHasStateMachine = (pObject->Components().GetComponentOfType<VTransitionStateMachine>() != NULL);
  if (!bHasStateMachine)
    sErrorMsgOut += "Parent entity needs to have a VTransitionStateMachine component.\r\n";

  // Define criteria here that allows the editor to attach this component to the passed object.
  // In our example, the object should be derived from VisObject3D_cl to be positionable.
  BOOL bIsValidClass = pObject->IsOfType(V_RUNTIME_CLASS(VisBaseEntity_cl));
  if (!bIsValidClass)
    sErrorMsgOut += "Component can only be added to instances of VisBaseEntity_cl or derived classes.";

  if (!bHasStateMachine || !bIsValidClass)
    return FALSE;
  
  return TRUE;
}


void VPlayableCharacterComponent::Serialize(VArchive &ar)
{
  char iLocalVersion = 1;

  IVObjectComponent::Serialize(ar);

  if (ar.IsLoading())
  {
    ar >> iLocalVersion;
    VASSERT_MSG(iLocalVersion==1, "Invalid local version. Please re-export");

    // Load the animation names
    ar >> IdleAnimation; 
    ar >> WalkForwardAnim; 
    ar >> WalkBackwardAnim; 
    ar >> RunAnimation; 
  }
  else
  {
    ar << iLocalVersion;

    // Save the animation names
    ar << IdleAnimation; 
    ar << WalkForwardAnim; 
    ar << WalkBackwardAnim; 
    ar << RunAnimation; 
  }
}




///////////////////////////////////////////////////////////////////////////////
// Update Method
///////////////////////////////////////////////////////////////////////////////

void VPlayableCharacterComponent::PerFrameUpdate()
{
  if (!GetOwner())
    return;

  VisBaseEntity_cl *pOwnerEntity = (VisBaseEntity_cl *) GetOwner();

  // Ensure that we have a state machine
  VTransitionStateMachine *pStateMachine = GetStateMachine();
  if (!pStateMachine)
    return;

  // Warn the user if he 
  if (!pOwnerEntity->GetPhysicsObject() && !m_bNoPhysicsWarningShown)
  {
  }

  // Set character to idle and return if pause is enabled
  if (m_bIsPaused)
  {
    SetState(IdleAnimation);
    return;
  }

  // Gather user input
  bool bWalkForward = m_pInputMap->GetTrigger(INPUTMAP_CHARACTER_WALK_FORWARD) != 0;
  bool bWalkBackward = m_pInputMap->GetTrigger(INPUTMAP_CHARACTER_WALK_BACKWARD) != 0;
  bool bRunModifier = m_pInputMap->GetTrigger(INPUTMAP_CHARACTER_WALK_RUN) != 0;
  float fRotateLeft = m_pInputMap->GetTrigger(INPUTMAP_CHARACTER_ROTATE_LEFT);
  float fRotateRight = m_pInputMap->GetTrigger(INPUTMAP_CHARACTER_ROTATE_RIGHT);

  // Process Walk/Idle commands
  if (bWalkForward)
  {
    if (bRunModifier)
      SetState(RunAnimation);
    else
      SetState(WalkForwardAnim);
  }
  else if (bWalkBackward)
    SetState(WalkBackwardAnim);
  else
    SetState(IdleAnimation);

  // Process Rotation commands
  if (fRotateLeft > 0.0f)
  {
    float fRotValue = fRotateLeft * ROTATION_DEGREES_PER_SECOND * Vision::GetTimer()->GetTimeDifference();
    hkvVec3 vRotValue(fRotValue, 0, 0);
    pOwnerEntity->IncRotationDelta(vRotValue);
  }
  else if (fRotateRight > 0.0f)
  {
    float fRotValue = fRotateRight * ROTATION_DEGREES_PER_SECOND * Vision::GetTimer()->GetTimeDifference();
    hkvVec3 vRotValue(-fRotValue, 0, 0);
    pOwnerEntity->IncRotationDelta(vRotValue);
  }
}

void VPlayableCharacterComponent::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender == &Vision::Callbacks.OnVideoChanged)
  {
#if defined(SUPPORTS_MULTITOUCH)
    // Reposition thumb stick (use invalid region in order to get the default region)
    if (m_pVirtualThumbStick != NULL)
      m_pVirtualThumbStick->SetValidArea(VRectanglef());
#endif
  }
}

///////////////////////////////////////////////////////////////////////////////
// Protected Methods
///////////////////////////////////////////////////////////////////////////////

VTransitionStateMachine *VPlayableCharacterComponent::GetStateMachine()
{
  if (!GetOwner())
    return NULL;

  // Search for transition state machine component in owner
  VTransitionStateMachine *pStateMachine = GetOwner()->Components().GetComponentOfType<VTransitionStateMachine>();
  if (!pStateMachine)
    return NULL;

  return pStateMachine;
}

void VPlayableCharacterComponent::SetState(VString &sNewState)
{
  if (sNewState.IsEmpty())
    return;

  VTransitionStateMachine *pStateMachine = GetStateMachine();
  if (!pStateMachine)
    return;

  pStateMachine->SetState(sNewState.AsChar());
}
  



START_VAR_TABLE(VPlayableCharacterComponent,IVObjectComponent,"Playable character component. Can be attached to entities to enable simple keyboard based character control for rapid prototyping. Requires the entity to have a transition state machine component! (Control keys are W,A,S,D)",VVARIABLELIST_FLAGS_NONE, "Playable Character" )
  DEFINE_VAR_VSTRING(VPlayableCharacterComponent, IdleAnimation, "Name of the idle animation", "", 0, 0, "dropdownlist(Animation)");
  DEFINE_VAR_VSTRING(VPlayableCharacterComponent, WalkForwardAnim, "Name of the walk forward animation", "", 0, 0, "dropdownlist(Animation)");
  DEFINE_VAR_VSTRING(VPlayableCharacterComponent, WalkBackwardAnim, "Name of the walk backward animation", "", 0, 0, "dropdownlist(Animation)");
  DEFINE_VAR_VSTRING(VPlayableCharacterComponent, RunAnimation, "Name of the run animation", "", 0, 0, "dropdownlist(Animation)");
  // more variables go here...
END_VAR_TABLE

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
