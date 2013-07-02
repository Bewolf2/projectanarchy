/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// Please refer to the documentation of the transition demo for more information about this class.
// ***********************************************************************************************
#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Samples/Engine/Common/Entities/TransitionCharacter.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiAnimNormalizeMixerNode.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Animation/Transition/VTransitionManager.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCharacterController.hpp> 

#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

// **************************************************
//            CONSTRUCTOR / DESTRUCTOR
// **************************************************
TransitionCharacter_cl::TransitionCharacter_cl()
{
  // This prevents a crash in e.g. vForge if the model and animations are missing
  m_bModelValid = false;  

  for(int i = 0; i < MAX_ANIM_ID; i++) m_spSkeletalSequenceList[i] = NULL;
  m_pStateMachine = NULL;
  m_bIsMoving = false;
}

TransitionCharacter_cl::~TransitionCharacter_cl()
{
}

// **************************************************
//            OVERRIDDEN ENTITY FUNCTIONS
// **************************************************
void TransitionCharacter_cl::InitFunction()
{
  SetCastShadows(TRUE);
  
  // Setup all animation sequences
	SetupAnimations();
  if (!m_bModelValid)
    return;
	
	// Setup Physics object for offset delta
#ifdef USE_HAVOK

	vHavokCharacterController *pPhys = new vHavokCharacterController();
	pPhys->Initialize();

	hkvAlignedBBox bbox;
	GetMesh()->GetCollisionBoundingBox(bbox);

  float r = bbox.getSizeX() * 0.5f;
	pPhys->Capsule_Radius = r;
	pPhys->Character_Top.set(0,0,bbox.m_vMax.z - r);
  pPhys->Character_Bottom.set(0,0,bbox.m_vMin.z + r);

 	pPhys->Max_Slope = 75.0f;
	AddComponent(pPhys);
	//pPhys->SetDebugRendering(TRUE);

#endif //USE_HAVOK
	
  // Get Model
  VDynamicMesh* pMesh = GetMesh();
	VASSERT(pMesh);

  // Transition table to use
  VTransitionTable *pTable = VTransitionManager::GlobalManager().LoadTransitionTable(pMesh,"Models\\Warrior\\WarriorEvents.vTransition");
  VASSERT(pTable && pTable->IsLoaded());

	// Setup the state machine component and pass the filename of the transition file
  // in which the transitions between the various animation states are defined.
	m_pStateMachine = new VTransitionStateMachine();
  m_pStateMachine->Init(this, pTable);
	AddComponent(m_pStateMachine);

	// Set initial state
	m_pStateMachine->SetState(m_spSkeletalSequenceList[ANIMID_IDLE]);
}

void TransitionCharacter_cl::DeInitFunction()
{
  V_SAFE_DISPOSEOBJECT(m_pStateMachine);
}

void TransitionCharacter_cl::ThinkFunction()
{
  // nothing to do here, since the transition component gets handled from outside
}

void TransitionCharacter_cl::MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB)
{
}

// **************************************************
//            SETUP ANIMATIONS
// **************************************************
void TransitionCharacter_cl::SetupAnimations()
{
  m_bModelValid = false;
  VDynamicMesh *pMesh = GetMesh();
  VVERIFY_OR_RET(pMesh);

  // idle animations
  m_spSkeletalSequenceList[ANIMID_IDLE] = (VisSkeletalAnimSequence_cl*)pMesh->GetSequence("Idle", VIS_MODELANIM_SKELETAL);
  VVERIFY_OR_RET(m_spSkeletalSequenceList[ANIMID_IDLE]);

  // run animations
  m_spSkeletalSequenceList[ANIMID_RUN] = (VisSkeletalAnimSequence_cl*)pMesh->GetSequence("Run", VIS_MODELANIM_SKELETAL);
  VVERIFY_OR_RET(m_spSkeletalSequenceList[ANIMID_RUN]);

  // walk animations
  m_spSkeletalSequenceList[ANIMID_WALK] = (VisSkeletalAnimSequence_cl*)pMesh->GetSequence("Walk", VIS_MODELANIM_SKELETAL);
  VVERIFY_OR_RET(m_spSkeletalSequenceList[ANIMID_WALK]);

  // turn animations
  m_spSkeletalSequenceList[ANIMID_TURN] = (VisSkeletalAnimSequence_cl*)pMesh->GetSequence("Turn", VIS_MODELANIM_SKELETAL);
  VVERIFY_OR_RET(m_spSkeletalSequenceList[ANIMID_TURN]);

  // walk backwards animation
  m_spSkeletalSequenceList[ANIMID_WALKBACKWARDS] = (VisSkeletalAnimSequence_cl*)pMesh->GetSequence("Run_BWD", VIS_MODELANIM_SKELETAL);
  VVERIFY_OR_RET(m_spSkeletalSequenceList[ANIMID_WALKBACKWARDS]);

  m_bModelValid = true;
}

void TransitionCharacter_cl::Stand()
{
	if (m_bModelValid)
    m_pStateMachine->SetState(m_spSkeletalSequenceList[ANIMID_IDLE]);
  m_bIsMoving = false;
}

void TransitionCharacter_cl::Walk()
{
	if (m_bModelValid)
    m_pStateMachine->SetState(m_spSkeletalSequenceList[ANIMID_WALK]);
  m_bIsMoving = true;
}

void TransitionCharacter_cl::WalkBackwards()
{
	if (m_bModelValid)
    m_pStateMachine->SetState(m_spSkeletalSequenceList[ANIMID_WALKBACKWARDS]);
  m_bIsMoving = true;
}

void TransitionCharacter_cl::Run()
{
	if (m_bModelValid)
    m_pStateMachine->SetState(m_spSkeletalSequenceList[ANIMID_RUN]);
  m_bIsMoving = true;
}

void TransitionCharacter_cl::RotateLeft()
{
	// Start turning sequence only when model is in idle state
	if (m_bModelValid && !m_bIsMoving)
		m_pStateMachine->SetState("Turn");
	DoRotateLeft();
  m_bIsMoving = false;
}

void TransitionCharacter_cl::RotateRight()
{
	// Start turning sequence only when model is in idle state
	if (m_bModelValid && !m_bIsMoving)
		m_pStateMachine->SetState("Turn");
	DoRotateRight();
  m_bIsMoving = false;
}

void TransitionCharacter_cl::DoRotateLeft()
{
  // rotate the entity and turn the head
  float fTimeDiff = Vision::GetTimer()->GetTimeDifference();
  float fRot = fTimeDiff * 180.f;
  IncOrientation( fRot, 0, 0 );
}

void TransitionCharacter_cl::DoRotateRight()
{
  // rotate the entity and turn the head
  float fTimeDiff = Vision::GetTimer()->GetTimeDifference();
  float fRot = fTimeDiff * 180.f;
  IncOrientation( -fRot, 0, 0 );
}

// **************************************************
//            STATE MACHINE
// **************************************************
VTransitionStateMachine* TransitionCharacter_cl::GetStateMachine()
{
	return m_pStateMachine;
}

V_IMPLEMENT_SERIAL( TransitionCharacter_cl, VisBaseEntity_cl, 0, Vision::GetEngineModule() );
START_VAR_TABLE(TransitionCharacter_cl, VisBaseEntity_cl, "Animated warrior character for the samples", VFORGE_HIDECLASS, "Models\\Warrior\\WarriorEvents.model")
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
