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
#include <Vision/Samples/Engine/ModelMerging/ModelMergingPCH.h>
#include <Vision/Samples/Engine/ModelMerging/TransitionBarbarian.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiAnimNormalizeMixerNode.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Animation/Transition/VTransitionManager.hpp> 
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCharacterController.hpp>

#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

// **************************************************
//            CONSTRUCTOR / DESTRUCTOR
// **************************************************
TransitionBarbarian_cl::TransitionBarbarian_cl()
{
  // This prevents a crash in e.g. vForge if the model and animations are missing
  m_bModelValid = false;  

  m_pPhys = NULL;

  for(int i = 0; i < MAX_ANIM_ID; i++) m_pSkeletalSequenceList[i] = NULL;
	for(int i = 0; i < MAX_ANIM_CONTROL; i++) m_pSkeletalAnimControl[i] = NULL;
}

TransitionBarbarian_cl::~TransitionBarbarian_cl()
{
}

// **************************************************
//            OVERRIDDEN ENTITY FUNCTIONS
// **************************************************
void TransitionBarbarian_cl::InitFunction()
{
  if (!HasMesh())
    return;

  SetCastShadows(TRUE);
  
  // Setup all animation sequences
	SetupAnimations();
  if (!m_bModelValid)
    return;

  if( !m_pPhys)
  {
    m_pPhys = new vHavokCharacterController();
    m_pPhys->Initialize();

    hkvAlignedBBox bbox;
    VDynamicMesh *pMesh = GetMesh();
    pMesh->GetCollisionBoundingBox(bbox);

    float r = bbox.getSizeX() * 0.5f;
    m_pPhys->Capsule_Radius = r;
    m_pPhys->Character_Top.set(0,0,bbox.m_vMax.z - r);
    m_pPhys->Character_Bottom.set(0,0,bbox.m_vMin.z + r);

    m_pPhys->Max_Slope = 75.0f;
    AddComponent(m_pPhys);
    //	pPhys->SetDebugRendering(TRUE);
  }
 
  // Get Model
  VDynamicMesh* pModel = GetMesh();
	VASSERT(pModel);

  // Transition table to use
  VTransitionTable *pTable = VTransitionManager::GlobalManager().LoadTransitionTable(pModel,"Barbarian.vTransition");
  VASSERT(pTable && pTable->IsLoaded());

	// Setup the state machine component and pass the filename of the transition file
  // in which the transitions between the various animation states are defined.
	m_pStateMachine = new VTransitionStateMachine();
  m_pStateMachine->Init(this, pTable);
	AddComponent(m_pStateMachine);

	// Set initial state
	m_pStateMachine->SetState(m_pSkeletalSequenceList[ANIMID_IDLE]);
}

void TransitionBarbarian_cl::DeInitFunction()
{
  V_SAFE_DISPOSEOBJECT(m_pStateMachine);
}

void TransitionBarbarian_cl::ThinkFunction()
{
  // nothing to do here, since the transition component gets handled from outside
}

void TransitionBarbarian_cl::MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB)
{
}

// **************************************************
//            SETUP ANIMATIONS
// **************************************************
void TransitionBarbarian_cl::SetupAnimations()
{
  m_bModelValid = false;
  VDynamicMesh *pModel = GetMesh();
  VVERIFY_OR_RET(pModel);

  m_pSkeletalSequenceList[ANIMID_IDLE] = (VisSkeletalAnimSequence_cl*)pModel->GetSequence("Idle", VIS_MODELANIM_SKELETAL);
  VVERIFY_OR_RET(m_pSkeletalSequenceList[ANIMID_IDLE]);

  m_pSkeletalSequenceList[ANIMID_WALK] = (VisSkeletalAnimSequence_cl*)pModel->GetSequence("Walk", VIS_MODELANIM_SKELETAL);
  VVERIFY_OR_RET(m_pSkeletalSequenceList[ANIMID_WALK]);

  m_pSkeletalSequenceList[ANIMID_RUN] = (VisSkeletalAnimSequence_cl*)pModel->GetSequence("Run", VIS_MODELANIM_SKELETAL);
  VVERIFY_OR_RET(m_pSkeletalSequenceList[ANIMID_RUN]);

  m_bModelValid = true;
}

void TransitionBarbarian_cl::Stand()
{
	if (m_bModelValid)
    m_pStateMachine->SetState(m_pSkeletalSequenceList[ANIMID_IDLE]);
}

void TransitionBarbarian_cl::Walk()
{
	if (m_bModelValid)
    m_pStateMachine->SetState(m_pSkeletalSequenceList[ANIMID_WALK]);
}

void TransitionBarbarian_cl::Run()
{
  if (m_bModelValid)
    m_pStateMachine->SetState(m_pSkeletalSequenceList[ANIMID_RUN]);
}

void TransitionBarbarian_cl::RotateLeft()
{
	DoRotateLeft();
}

void TransitionBarbarian_cl::RotateRight()
{
	DoRotateRight();
}

void TransitionBarbarian_cl::DoRotateLeft()
{
  // rotate the entity and turn the head
  float fTimeDiff = Vision::GetTimer()->GetTimeDifference();
  float fRot = fTimeDiff * 180.f;
  IncOrientation( fRot, 0, 0 );
}

void TransitionBarbarian_cl::DoRotateRight()
{
  // rotate the entity and turn the head
  float fTimeDiff = Vision::GetTimer()->GetTimeDifference();
  float fRot = fTimeDiff * 180.f;
  IncOrientation( -fRot, 0, 0 );
}

// **************************************************
//            STATE MACHINE
// **************************************************
VTransitionStateMachine* TransitionBarbarian_cl::GetStateMachine()
{
	return m_pStateMachine;
}

V_IMPLEMENT_SERIAL( TransitionBarbarian_cl, VisBaseEntity_cl, 0, Vision::GetEngineModule() );
  START_VAR_TABLE(TransitionBarbarian_cl, VisBaseEntity_cl, "Animated transition character for the samples", VFORGE_HIDECLASS, "")
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
