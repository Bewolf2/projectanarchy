/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// Please refer to the documentation of the character animation demo for more information
// about this class (see chapter "Tutorials for Programmers: Character Animation Demo")
// ***********************************************************************************************

#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Samples/Engine/Common/Entities/AttachedTorch.hpp>
#include <Vision/Samples/Engine/Common/Entities/AnimatedWarrior.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiAnimLayerMixerNode.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiAnimNormalizeMixerNode.hpp>
 
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCharacterController.hpp> 

#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

#define ANIMCOMP_FULLBODY  0
#define ANIMCOMP_UPPERBODY 1

#define HEAD_MAX_ANGLE       30
#define HEAD_ROTATION_SPEED  70

// animation events
#define EVENT_UPPERBODYFADEIN_FINISHED            1
#define EVENT_UPPERBODYFADEOUT_FINISHED           2
#define EVENT_ANIM_LEGSPARALLEL_FIRSTTIME         3
#define EVENT_ANIM_LEGSPARALLEL_SECONDTIME        4
#define EVENT_ANIM_UPPERBODY_ARMING_FINISHED      5
#define EVENT_ANIM_UPPERBODY_DISARMING_FINISHED   6
#define EVENT_ANIM_UPPERBODY_ATTACK_FINISHED      7
#define EVENT_ANIM_UPPERBODY_GETTORCH_FINISHED    8
#define EVENT_ANIM_UPPERBODY_PUTTORCH_FINISHED    9


// **************************************************
//            CONSTRUCTOR / DESTRUCTOR
// **************************************************
AnimatedWarrior_cl::AnimatedWarrior_cl()
{
  m_bEnabled = true;
  m_bModelValid = false;  // This prevents a crash in e.g. vForge if the model and animations are missing

  m_iPrimaryUpperBodyControlIndex = -1;
  m_spPrimaryFullBodyControl = NULL;
  m_pTorch = NULL;
  m_pLayerMixer = NULL;
  m_pNormalizeMixer = NULL;
  m_spAnimConfig = NULL;

  // animation ID lookup
  m_pFullBodyAnimIDLookup[FBSTATETYPE_NORMAL] = CreateAnimIDLookupForStateType( FBSTATETYPE_NORMAL );
  m_pFullBodyAnimIDLookup[FBSTATETYPE_ARMED] = CreateAnimIDLookupForStateType( FBSTATETYPE_ARMED );

  // create full body states
  m_pCurrentFullBodyState = NULL;
  m_pFullBodyIdleState[FBSTATETYPE_NORMAL] = new FullBodyIdleState( this, "Idle", FBSTATETYPE_NORMAL );
  m_pFullBodyIdleState[FBSTATETYPE_ARMED] = new FullBodyIdleState( this, "Idle Armed", FBSTATETYPE_ARMED );
  m_pFullBodyWalkState[FBSTATETYPE_NORMAL] = new FullBodyWalkState( this, "Walk", FBSTATETYPE_NORMAL );
  m_pFullBodyWalkState[FBSTATETYPE_ARMED] = new FullBodyWalkState( this, "Walk Armed", FBSTATETYPE_ARMED );
  m_pFullBodyRunState[FBSTATETYPE_NORMAL] = new FullBodyRunState( this, "Run", FBSTATETYPE_NORMAL );
  m_pFullBodyRunState[FBSTATETYPE_ARMED] = new FullBodyRunState( this, "Run Armed", FBSTATETYPE_ARMED );
  m_pFullBodyRotateState[FBSTATETYPE_NORMAL] = new FullBodyRotateState( this, "Rotate", FBSTATETYPE_NORMAL );
  m_pFullBodyRotateState[FBSTATETYPE_ARMED] = new FullBodyRotateState( this, "Rotate Armed", FBSTATETYPE_ARMED );
  m_pFullBodyWalkBackwardsState[FBSTATETYPE_NORMAL] = new FullBodyWalkBackwardsState( this, "WalkBack", FBSTATETYPE_NORMAL ); 
  m_pFullBodyWalkBackwardsState[FBSTATETYPE_ARMED] = new FullBodyWalkBackwardsState( this, "WalkBack Armed", FBSTATETYPE_ARMED ); 

  // create upper body states
  m_pCurrentUpperBodyState = NULL;
  m_pUpperBodyIdleState = new UpperBodyIdleState( this );
  m_pUpperBodyIdleArmedState = new UpperBodyIdleArmedState( this );
  m_pUpperBodyDrawTorchState = new UpperBodyDrawTorchState( this );
  m_pUpperBodyHolsterTorchState = new UpperBodyHolsterTorchState( this );
  m_pUpperBodyAttackingState = new UpperBodyAttackingState( this );
}

AnimatedWarrior_cl::~AnimatedWarrior_cl()
{
  // cleanup
  V_SAFE_DELETE( m_pUpperBodyIdleState );
  V_SAFE_DELETE( m_pUpperBodyIdleArmedState );
  V_SAFE_DELETE( m_pUpperBodyDrawTorchState );
  V_SAFE_DELETE( m_pUpperBodyHolsterTorchState );
  V_SAFE_DELETE( m_pUpperBodyAttackingState );
  V_SAFE_DELETE( m_pFullBodyIdleState[FBSTATETYPE_NORMAL] );
  V_SAFE_DELETE( m_pFullBodyIdleState[FBSTATETYPE_ARMED] );
  V_SAFE_DELETE( m_pFullBodyWalkState[FBSTATETYPE_NORMAL] );
  V_SAFE_DELETE( m_pFullBodyWalkState[FBSTATETYPE_ARMED] );
  V_SAFE_DELETE( m_pFullBodyRunState[FBSTATETYPE_NORMAL] );
  V_SAFE_DELETE( m_pFullBodyRunState[FBSTATETYPE_ARMED] );
  V_SAFE_DELETE( m_pFullBodyRotateState[FBSTATETYPE_NORMAL] );
  V_SAFE_DELETE( m_pFullBodyRotateState[FBSTATETYPE_ARMED] );
  V_SAFE_DELETE( m_pFullBodyWalkBackwardsState[FBSTATETYPE_NORMAL] );
  V_SAFE_DELETE( m_pFullBodyWalkBackwardsState[FBSTATETYPE_ARMED] );
  V_SAFE_DELETE( m_pFullBodyAnimIDLookup[FBSTATETYPE_NORMAL] );
  V_SAFE_DELETE( m_pFullBodyAnimIDLookup[FBSTATETYPE_ARMED] );
}

// **************************************************
//            OVERRIDDEN ENTITY FUNCTIONS
// **************************************************

void AnimatedWarrior_cl::InitFunction()
{
  SetCastShadows(TRUE);
  
  SetupAnimations();
  if (!m_bModelValid)
    return;

	m_pCharacterController = new vHavokCharacterController();
	m_pCharacterController->Initialize();

	hkvAlignedBBox bbox;
	GetMesh()->GetCollisionBoundingBox(bbox);

  float r = bbox.getSizeX() * 0.5f;
	m_pCharacterController->Capsule_Radius = r;
	m_pCharacterController->Character_Top.set(0,0,bbox.m_vMax.z - r);
  m_pCharacterController->Character_Bottom.set(0,0,bbox.m_vMin.z + r);

 	m_pCharacterController->Max_Slope = 75.0f;

  if (!m_bEnabled)
    m_pCharacterController->SetEnabled(FALSE);

	AddComponent(m_pCharacterController);

  //// setup animation system
  // create config
  VDynamicMesh* pMesh = GetMesh();
  VASSERT(pMesh);
  VisSkeleton_cl* pSkeleton = pMesh->GetSkeleton();
  VASSERT(pSkeleton);

  VisAnimFinalSkeletalResult_cl* pFinalSkeletalResult;
  m_spAnimConfig = VisAnimConfig_cl::CreateSkeletalConfig(pMesh, &pFinalSkeletalResult);
  SetAnimConfig(m_spAnimConfig);

  // create mixer structure, we keep pointers on the mixers to add and remove inputs
  m_pLayerMixer = new VisAnimLayerMixerNode_cl(pSkeleton);
  pFinalSkeletalResult->SetSkeletalAnimInput(m_pLayerMixer);
  m_pNormalizeMixer = new VisAnimNormalizeMixerNode_cl(pSkeleton);
  m_pLayerMixer->AddMixerInput(m_pNormalizeMixer, 1.f);

  //// create per bone weighting list for upper body
  int iBoneCount = pSkeleton->GetBoneCount();
  float* fPerBoneWeightingList = new float[iBoneCount];
  memset(fPerBoneWeightingList, 0, sizeof(float)*iBoneCount);

  // set all bone weights above the spine to 1
  pSkeleton->SetBoneWeightRecursive(1.f, pSkeleton->GetBoneIndexByName("skeleton1:Spine"), fPerBoneWeightingList);

  int iMixerInputIndex = -1;

  for(int i=0; i<UPPERBODY_CONTROLCOUNT; i++)
  {
    m_spUpperBodyControls[i] = new VisSkeletalAnimControl_cl(pSkeleton, VSKELANIMCTRL_DEFAULTS);
    m_spUpperBodyControls[i]->AddEventListener(this); // we want to receive all events from sequence and control
    iMixerInputIndex = m_pLayerMixer->AddMixerInput(m_spUpperBodyControls[i], 0.f);
    m_pLayerMixer->ApplyPerBoneWeightingMask(iMixerInputIndex, iBoneCount, fPerBoneWeightingList);
  }
  
  V_SAFE_DELETE_ARRAY(fPerBoneWeightingList);

  // set the start animation
  BlendOverFullBodyAnimation(0.f, ANIMID_IDLE, 1.f, 0.f);
 
  SetFullBodyState(m_pFullBodyIdleState[FBSTATETYPE_NORMAL]);
  SetUpperBodyState(m_pUpperBodyIdleState);

  // initialise variables for upperbody fadein/fadeout
  m_eUpperBodyFadeState = FADESTATE_NONE;

  // setup neck bone for head rotation
  m_iHeadBoneIndex = pSkeleton->GetBoneIndexByName("skeleton1:Neck");
  m_fHeadRotationAngles[0] = 0.f; m_fHeadRotationAngles[1] = 0.f; m_fHeadRotationAngles[2] = 0.f;
  m_bHeadInMovement = false;

  // setup the torch
  hkvVec3 vDummyOrigin;
  m_pTorch = (AttachedTorch_cl *) Vision::Game.CreateEntity( "AttachedTorch_cl", vDummyOrigin );
  
  HideTorch();

  // attach the torch to the bone
  //hkvVec3 localTranslation(-11.f, -4.5f, 25.f);
  hkvVec3 localTranslation(-11.f, 5.5f, 0.f);
  float euler[3] = {90, 0, 185};
  hkvQuat localRotation;
  localRotation.setFromEulerAngles (euler[2], euler[1], euler[0]); // pass as roll, pitch, yaw
  m_pTorch->Attach(this, "skeleton1:RightHand", localRotation, localTranslation);
}

void AnimatedWarrior_cl::DeInitFunction()
{
  V_SAFE_DISPOSEOBJECT( m_pTorch );

  m_spAnimConfig = NULL;
}

//-----------------------------------------------------------------------------------

void AnimatedWarrior_cl::SetEnabled(bool bEnabled)
{
  if (m_bEnabled == bEnabled)
    return;
  m_bEnabled = bEnabled;

  if (!m_bModelValid)
    return;

#if defined(USE_HAVOK)
  // Turn on/off character controller
  if (m_pCharacterController != NULL)
    m_pCharacterController->SetEnabled(bEnabled ? TRUE : FALSE);
#endif

  if (bEnabled)
  {
    // Resume animation controls
    for (int i = 0; i < UPPERBODY_CONTROLCOUNT; i++)
      m_spUpperBodyControls[i]->Resume();
    for (int i = 0; i < m_FullBodyControlList.Count(); i++)
      m_FullBodyControlList.GetAt(i)->Resume();
  }
  else
  {
    // Pause animation controls
    for (int i = 0; i < UPPERBODY_CONTROLCOUNT; i++)
      m_spUpperBodyControls[i]->Pause();
    for (int i = 0; i < m_FullBodyControlList.Count(); i++)
      m_FullBodyControlList.GetAt(i)->Pause();
  }
}

//-----------------------------------------------------------------------------------

void AnimatedWarrior_cl::SetupAnimations()
{
  m_bModelValid = false;
  VDynamicMesh *pMesh = GetMesh();
  VVERIFY_OR_RET(pMesh);
  VisAnimEventList_cl *pEventList = NULL;

  // idle animations
  m_spSkeletalSequenceList[ANIMID_IDLE] = (VisSkeletalAnimSequence_cl*)pMesh->GetSequence("Idle", VIS_MODELANIM_SKELETAL);
  VVERIFY_OR_RET(m_spSkeletalSequenceList[ANIMID_IDLE]);
  m_spSkeletalSequenceList[ANIMID_IDLE_TORCH] = (VisSkeletalAnimSequence_cl*)pMesh->GetSequence("Idle_Look_Torch", VIS_MODELANIM_SKELETAL);
  VVERIFY_OR_RET(m_spSkeletalSequenceList[ANIMID_IDLE_TORCH]);

  // run animation
  m_spSkeletalSequenceList[ANIMID_RUN] = (VisSkeletalAnimSequence_cl*)pMesh->GetSequence("Run", VIS_MODELANIM_SKELETAL);
  VVERIFY_OR_RET(m_spSkeletalSequenceList[ANIMID_RUN]);
  pEventList = m_spSkeletalSequenceList[ANIMID_RUN]->GetEventList();
  pEventList->AddEvent(0.0f, EVENT_ANIM_LEGSPARALLEL_FIRSTTIME);
  pEventList->AddEvent(0.396f, EVENT_ANIM_LEGSPARALLEL_SECONDTIME);

  // run with torch animation
  m_spSkeletalSequenceList[ANIMID_RUN_TORCH] = (VisSkeletalAnimSequence_cl*)pMesh->GetSequence("Run_Torch", VIS_MODELANIM_SKELETAL);
  VVERIFY_OR_RET(m_spSkeletalSequenceList[ANIMID_RUN_TORCH]);
  pEventList = m_spSkeletalSequenceList[ANIMID_RUN_TORCH]->GetEventList();
  pEventList->AddEvent(0.0f, EVENT_ANIM_LEGSPARALLEL_FIRSTTIME);
  pEventList->AddEvent(0.396f, EVENT_ANIM_LEGSPARALLEL_SECONDTIME);

  // walk animation
  m_spSkeletalSequenceList[ANIMID_WALK] = (VisSkeletalAnimSequence_cl*)pMesh->GetSequence("Walk", VIS_MODELANIM_SKELETAL);
  VVERIFY_OR_RET(m_spSkeletalSequenceList[ANIMID_WALK]);
  pEventList = m_spSkeletalSequenceList[ANIMID_WALK]->GetEventList();
  pEventList->AddEvent(0.0f, EVENT_ANIM_LEGSPARALLEL_FIRSTTIME);
  pEventList->AddEvent(0.495f, EVENT_ANIM_LEGSPARALLEL_SECONDTIME);

  // walk with torch animation
  m_spSkeletalSequenceList[ANIMID_WALK_TORCH] = (VisSkeletalAnimSequence_cl*)pMesh->GetSequence("Walk_Torch", VIS_MODELANIM_SKELETAL);
  VVERIFY_OR_RET(m_spSkeletalSequenceList[ANIMID_WALK_TORCH]);
  pEventList = m_spSkeletalSequenceList[ANIMID_WALK_TORCH]->GetEventList();
  pEventList->AddEvent(0.0f, EVENT_ANIM_LEGSPARALLEL_FIRSTTIME);
  pEventList->AddEvent(0.495f, EVENT_ANIM_LEGSPARALLEL_SECONDTIME);
 
  // turn animation
  m_spSkeletalSequenceList[ANIMID_TURN] = (VisSkeletalAnimSequence_cl*)pMesh->GetSequence("Turn", VIS_MODELANIM_SKELETAL);
  VVERIFY_OR_RET(m_spSkeletalSequenceList[ANIMID_TURN]);

  // turn with torch animation
  m_spSkeletalSequenceList[ANIMID_TURN_TORCH] = (VisSkeletalAnimSequence_cl*)pMesh->GetSequence("Turn_Torch", VIS_MODELANIM_SKELETAL);
  VVERIFY_OR_RET(m_spSkeletalSequenceList[ANIMID_TURN_TORCH]);

  // walk backwards animation
  m_spSkeletalSequenceList[ANIMID_WALKBACKWARDS] = (VisSkeletalAnimSequence_cl*)pMesh->GetSequence("Run_BWD_V2", VIS_MODELANIM_SKELETAL);
  VVERIFY_OR_RET(m_spSkeletalSequenceList[ANIMID_WALKBACKWARDS]);

  // walk backwards with torch animation
  m_spSkeletalSequenceList[ANIMID_WALKBACKWARDS_TORCH] = (VisSkeletalAnimSequence_cl*)pMesh->GetSequence("Run_BWD_Torch_V2", VIS_MODELANIM_SKELETAL);
  VVERIFY_OR_RET(m_spSkeletalSequenceList[ANIMID_WALKBACKWARDS_TORCH]);

  // upper body animations (torch)
  m_spSkeletalSequenceList[ANIMID_UPPERBODY_ARM] = (VisSkeletalAnimSequence_cl*)pMesh->GetSequence("Draw_Dagger", VIS_MODELANIM_SKELETAL);
  VVERIFY_OR_RET(m_spSkeletalSequenceList[ANIMID_UPPERBODY_ARM]);
  m_spSkeletalSequenceList[ANIMID_UPPERBODY_ARM]->RemoveOffsetDeltaTrack();
  pEventList = m_spSkeletalSequenceList[ANIMID_UPPERBODY_ARM]->GetEventList();
  pEventList->AddEvent(m_spSkeletalSequenceList[ANIMID_UPPERBODY_ARM]->GetLength(), EVENT_ANIM_UPPERBODY_ARMING_FINISHED);

  m_spSkeletalSequenceList[ANIMID_UPPERBODY_DISARM] = (VisSkeletalAnimSequence_cl*)pMesh->GetSequence("Holster_Dagger", VIS_MODELANIM_SKELETAL);
  VVERIFY_OR_RET(m_spSkeletalSequenceList[ANIMID_UPPERBODY_DISARM]);
  m_spSkeletalSequenceList[ANIMID_UPPERBODY_DISARM]->RemoveOffsetDeltaTrack();
  pEventList = m_spSkeletalSequenceList[ANIMID_UPPERBODY_DISARM]->GetEventList();
  pEventList->AddEvent(m_spSkeletalSequenceList[ANIMID_UPPERBODY_DISARM]->GetLength(), EVENT_ANIM_UPPERBODY_DISARMING_FINISHED);
   
  m_spSkeletalSequenceList[ANIMID_UPPERBODY_ATTACK] = (VisSkeletalAnimSequence_cl*)pMesh->GetSequence("Strike_Dagger", VIS_MODELANIM_SKELETAL);
  VVERIFY_OR_RET(m_spSkeletalSequenceList[ANIMID_UPPERBODY_ATTACK]);
  m_spSkeletalSequenceList[ANIMID_UPPERBODY_ATTACK]->RemoveOffsetDeltaTrack();
  pEventList = m_spSkeletalSequenceList[ANIMID_UPPERBODY_ATTACK]->GetEventList();
  pEventList->AddEvent(m_spSkeletalSequenceList[ANIMID_UPPERBODY_ATTACK]->GetLength(), EVENT_ANIM_UPPERBODY_ATTACK_FINISHED);

  m_bModelValid = true;
}


void AnimatedWarrior_cl::BlendOverFullBodyAnimation(float fBlendTime, int iNewSeqId, float fNewSpeed, float fNewPosition)
{
  VASSERT((fBlendTime>=0.f)&&(iNewSeqId>=0));

  FullBodyControl_cl* pOldFullBodyControl = m_spPrimaryFullBodyControl;

  // get control and mixer input index for new animation
  FullBodyControl_cl* pNewFullBodyControl = GetFreeFullBodyControl();

  pNewFullBodyControl->SetAnimSequence(m_spSkeletalSequenceList[iNewSeqId]);
  float fCurrentPosition = fNewPosition;
  if(fNewPosition<0.f)
    fCurrentPosition = pOldFullBodyControl->GetCurrentSequencePosition();

  pNewFullBodyControl->Resume();
  pNewFullBodyControl->SetSpeed(fNewSpeed);
  pNewFullBodyControl->SetCurrentSequencePosition(fCurrentPosition);

  if(pOldFullBodyControl)
  {
    VASSERT(pOldFullBodyControl->m_iMixerInputIndex!=-1);
    m_pNormalizeMixer->EaseOut(pOldFullBodyControl->m_iMixerInputIndex, fBlendTime, true);
  }
  m_pNormalizeMixer->EaseIn(pNewFullBodyControl->m_iMixerInputIndex, fBlendTime, true);

  m_spPrimaryFullBodyControl = pNewFullBodyControl;
  VASSERT(m_spPrimaryFullBodyControl->m_iMixerInputIndex!=-1);
}


void AnimatedWarrior_cl::BlendOverUpperBodyAnimation(float fBlendTime, int iNewSeqId, float fNewSpeed, float fNewPosition)
{
  VASSERT((fBlendTime>=0.f)&&(iNewSeqId>=0)&&((m_iPrimaryUpperBodyControlIndex==-1)||(m_iPrimaryUpperBodyControlIndex==0)||(m_iPrimaryUpperBodyControlIndex==1)));

  int iNewIndex = (m_iPrimaryUpperBodyControlIndex+1)%2; // we always toggle between input 0 and 1 of the normalize mixer
  int iOldIndex = m_iPrimaryUpperBodyControlIndex;

  if(iNewIndex>=0)
  {
    m_spUpperBodyControls[iNewIndex]->SetAnimSequence(m_spSkeletalSequenceList[iNewSeqId]);
    float fCurrentPosition = fNewPosition;
    if(fNewPosition<0.f)
      if(iOldIndex>=0)
        fCurrentPosition = m_spUpperBodyControls[iOldIndex]->GetCurrentSequencePosition();
      else
        fCurrentPosition = 0;
    m_spUpperBodyControls[iNewIndex]->Pause();
    m_spUpperBodyControls[iNewIndex]->SetSpeed(fNewSpeed);
    m_spUpperBodyControls[iNewIndex]->SetCurrentSequencePosition(fCurrentPosition);

    m_pLayerMixer->EaseIn(iNewIndex+1, fBlendTime, true);
    m_iPrimaryUpperBodyControlIndex = iNewIndex;
  }

  if(iOldIndex>=0)
    m_pLayerMixer->EaseOut(iOldIndex+1, fBlendTime, true);
}


void AnimatedWarrior_cl::BlendOutUpperBodyAnimation(float fBlendTime)
{
  VASSERT((fBlendTime>=0.f)&&((m_iPrimaryUpperBodyControlIndex==0)||(m_iPrimaryUpperBodyControlIndex==1)));

  m_pLayerMixer->EaseOut(m_iPrimaryUpperBodyControlIndex+1, fBlendTime, true);
}


void AnimatedWarrior_cl::ThinkFunction()
{
  if (!m_bModelValid || !m_bEnabled)
    return;

  UpdateUpperBodyFade();

  // forward think function call to state objects
  m_pCurrentFullBodyState->OnThink();
  m_pCurrentUpperBodyState->OnThink();

  // head rotation
  if(!m_bHeadInMovement)
    RelaxHead(Vision::GetTimer()->GetTimeDifference()); 
  else
    m_bHeadInMovement = false;

  // weapon handling
  m_pTorch->Update();

  RemoveInactiveFullBodyControls(); // free all fullbody controls that are already faded out
}

void AnimatedWarrior_cl::MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB)
{
  // forward events to state objects
  if (iID == VIS_MSG_EVENT)
    TriggerEvent((int)iParamA); // iParamA is never used as a poitner here
}


// **************************************************
//              NECK BONE HANDLING
// **************************************************
void AnimatedWarrior_cl::TurnHead(float fDegreesPerSecond, float fTimeDiff, float fMinAngle, float fMaxAngle)
{
  float fAmount = fDegreesPerSecond * fTimeDiff;

  // update joint rotation (with clamp)
  m_fHeadRotationAngles[1] += fAmount;
  if (m_fHeadRotationAngles[1] > fMaxAngle)
    m_fHeadRotationAngles[1] = fMaxAngle;
  else if (m_fHeadRotationAngles[1] < fMinAngle)
    m_fHeadRotationAngles[1] = fMinAngle;

  //// set new rotation
  VASSERT(GetAnimConfig() == m_spAnimConfig);
  VisAnimFinalSkeletalResult_cl *pFinalResult = m_spAnimConfig->GetFinalResult();
  VASSERT(pFinalResult != NULL);

  // update the current localspace result and add custom rotation
  pFinalResult->GetCurrentLocalSpaceResult();
  hkvQuat tempRotation(hkvNoInitialization);
  tempRotation.setFromEulerAngles (m_fHeadRotationAngles[2], m_fHeadRotationAngles[1], m_fHeadRotationAngles[0]);
  pFinalResult->SetCustomBoneRotation(m_iHeadBoneIndex, tempRotation, VIS_MODIFY_BONE|VIS_LOCAL_SPACE);

  m_bHeadInMovement = true;
}

void AnimatedWarrior_cl::RelaxHead(float fTimeDiff)
{
  // only relax the head if it was not in movement in the last frame
  if(m_bHeadInMovement)
    return;

  // relax the head again
  if(m_fHeadRotationAngles[1] > 0.f)
    TurnHead(-HEAD_ROTATION_SPEED, fTimeDiff, 0, HEAD_MAX_ANGLE);
  else if (m_fHeadRotationAngles[1] < 0.f)
    TurnHead(HEAD_ROTATION_SPEED, fTimeDiff, -HEAD_MAX_ANGLE, 0);

  m_bHeadInMovement = false;
}


// **************************************************
//            HELPER FUNCTIONS
// **************************************************
void AnimatedWarrior_cl::TriggerEvent(int iEvent)
{
  // forward event to state objects
  m_pCurrentFullBodyState->OnEvent(iEvent);
  m_pCurrentUpperBodyState->OnEvent(iEvent);
}


AnimatedWarrior_cl::FullBodyAnimIDLookup *AnimatedWarrior_cl::CreateAnimIDLookupForStateType(FBSTATETYPE eStateType)
{
  // setup lookup tables with animation IDs for the different state types (normal, armed)
  FullBodyAnimIDLookup *pNewLookup = new FullBodyAnimIDLookup;
  
  if ( eStateType == FBSTATETYPE_NORMAL )
  {
    pNewLookup->iIdleID = ANIMID_IDLE;
    pNewLookup->iWalkID = ANIMID_WALK;
    pNewLookup->iRunID = ANIMID_RUN;
    pNewLookup->iTurnID = ANIMID_TURN;
    pNewLookup->iWalkBackwardsID = ANIMID_WALKBACKWARDS;
  }
  else
  {
    pNewLookup->iIdleID = ANIMID_IDLE_TORCH;
    pNewLookup->iWalkID = ANIMID_WALK_TORCH;
    pNewLookup->iRunID = ANIMID_RUN_TORCH;
    pNewLookup->iTurnID = ANIMID_TURN_TORCH;
    pNewLookup->iWalkBackwardsID = ANIMID_WALKBACKWARDS_TORCH;
  }

  return pNewLookup;
}


FullBodyControl_cl* AnimatedWarrior_cl::GetFreeFullBodyControl(void)
{
  FullBodyControl_cl* pFullBodyControl = NULL;

  for(int i=0; i<m_FullBodyControlList.Count(); i++)
  {
    FullBodyControl_cl* pTempFullBodyControl = (FullBodyControl_cl*)m_FullBodyControlList.GetAt(i);
    if(pTempFullBodyControl && (pTempFullBodyControl->m_iMixerInputIndex == -1))
    {
      // save pointer on control and add it to the normalize mixer
      pFullBodyControl = pTempFullBodyControl;
      break; 
    }
  }

  // if there is no free control left add a new one
  if(pFullBodyControl==NULL)
  {
    pFullBodyControl = new FullBodyControl_cl(GetMesh()->GetSkeleton());
    pFullBodyControl->AddEventListener(this);  // we want to receive all events from sequence and control
    m_FullBodyControlList.Add(pFullBodyControl);
  }

  //// add control to mixer
  // find empty mixer input
  int iMixerInputIndex = -1;
  for(int i=0; i<m_pNormalizeMixer->GetInputCount(); i++)
  {
    IVisAnimResultGenerator_cl* pResultGenerator = m_pNormalizeMixer->GetMixerInput(i);
    if(!pResultGenerator)
    {
      // found an empty input
      iMixerInputIndex = i;
      break; 
    }
  }
  if(iMixerInputIndex>=0)
  {
    pFullBodyControl->m_iMixerInputIndex = iMixerInputIndex;
    m_pNormalizeMixer->SetMixerInput(iMixerInputIndex, pFullBodyControl, 0.f);
  }
  else
    pFullBodyControl->m_iMixerInputIndex = m_pNormalizeMixer->AddMixerInput(pFullBodyControl, 0.f);
  
  VASSERT(pFullBodyControl->m_iMixerInputIndex!=-1);

  return pFullBodyControl;
}


void AnimatedWarrior_cl::RemoveInactiveFullBodyControls(void)
{
  FullBodyControl_cl* pFullBodyControl = NULL;

  for(int i=0; i<m_FullBodyControlList.Count(); i++)
  {
    pFullBodyControl = (FullBodyControl_cl*)m_FullBodyControlList.GetAt(i);
    if(pFullBodyControl && (pFullBodyControl->m_iMixerInputIndex != -1))
    {
      // check if control has already been faded out and remove it from mixer
      float fCurrentWeight = m_pNormalizeMixer->GetCurrentEaseValue(pFullBodyControl->m_iMixerInputIndex);
      if((fCurrentWeight==0)&&(pFullBodyControl!=m_spPrimaryFullBodyControl)/*&&(!m_pNormalizeMixer->IsEasingOut(pFullBodyControl->m_iMixerInputIndex))&&(!m_pNormalizeMixer->IsEasingOut(pFullBodyControl->m_iMixerInputIndex))*/)
      {
        VASSERT(pFullBodyControl!=m_spPrimaryFullBodyControl);
        bool bSuccessful = m_pNormalizeMixer->SetMixerInput(pFullBodyControl->m_iMixerInputIndex, NULL, 0.f);
        VASSERT(bSuccessful);
        pFullBodyControl->m_iMixerInputIndex = -1;
      } 
    }
  }
}


// **************************************************
//            TORCH RELATED FUNCTIONS
// **************************************************
void AnimatedWarrior_cl::ShowTorch()
{
  m_pTorch->Show();
}

void AnimatedWarrior_cl::HideTorch()
{
  m_pTorch->Hide();
}


// **************************************************
//            UPPER BODY FADE-IN HANDLING
// **************************************************
void AnimatedWarrior_cl::StartUpperBodyFadeIn()
{
  m_eUpperBodyFadeState = FADESTATE_FADEIN;
}

void AnimatedWarrior_cl::StartUpperBodyFadeOut()
{
  m_eUpperBodyFadeState = FADESTATE_FADEOUT;
}

void AnimatedWarrior_cl::UpdateUpperBodyFade()
{
  if (m_eUpperBodyFadeState == FADESTATE_NONE)
    return;

  VASSERT(m_iPrimaryUpperBodyControlIndex>=0);

  if (m_eUpperBodyFadeState == FADESTATE_FADEIN)
  {
    // test for finished easing in
    if (!m_pLayerMixer->IsEasingIn(m_iPrimaryUpperBodyControlIndex+1))
    {
      VASSERT(m_pLayerMixer->GetCurrentEaseValue(m_iPrimaryUpperBodyControlIndex+1) >= 1.f);
      m_eUpperBodyFadeState = FADESTATE_NONE;
      TriggerEvent(EVENT_UPPERBODYFADEIN_FINISHED);
    }
  }
  else if(m_eUpperBodyFadeState == FADESTATE_FADEOUT)
  {
    // test for finished easing out
    if (!m_pLayerMixer->IsEasingOut(m_iPrimaryUpperBodyControlIndex+1))
    {
      VASSERT(m_pLayerMixer->GetCurrentEaseValue(m_iPrimaryUpperBodyControlIndex+1) <= 0.f);
      m_eUpperBodyFadeState = FADESTATE_NONE;
      m_iPrimaryUpperBodyControlIndex = -1;
      TriggerEvent(EVENT_UPPERBODYFADEOUT_FINISHED);
    }
  }
  else VASSERT(false);
}


// **************************************************
//            FULL BODY STATE BASE CLASS
// **************************************************
void AnimatedWarrior_cl::FullBodyState::DoRotateLeft(float fRotationSpeed)
{
  // rotate the entity and turn the head
  float fTimeDiff = Vision::GetTimer()->GetTimeDifference();
  float fRot = fTimeDiff * fRotationSpeed;
  m_pEnt->IncOrientation( fRot, 0, 0 );
  m_pEnt->TurnHead(HEAD_ROTATION_SPEED, fTimeDiff, -HEAD_MAX_ANGLE, HEAD_MAX_ANGLE);
}

void AnimatedWarrior_cl::FullBodyState::DoRotateRight(float fRotationSpeed)
{
  // rotate the entity and turn the head
  float fTimeDiff = Vision::GetTimer()->GetTimeDifference();
  float fRot = fTimeDiff * fRotationSpeed;
  m_pEnt->IncOrientation( -fRot, 0, 0 );
  m_pEnt->TurnHead(-HEAD_ROTATION_SPEED, fTimeDiff, -HEAD_MAX_ANGLE, HEAD_MAX_ANGLE);
}


bool AnimatedWarrior_cl::FullBodyState::IsBlending()
{
  return m_pEnt->m_pNormalizeMixer->IsEasingIn(m_pEnt->m_spPrimaryFullBodyControl->m_iMixerInputIndex);
}


// **************************************************
//            FULL BODY STATE: IDLE
// **************************************************
AnimatedWarrior_cl::FullBodyIdleState::FullBodyIdleState( AnimatedWarrior_cl *pEntity, const char *szName, FBSTATETYPE eStateType ) : FullBodyState(szName, pEntity)
{
  m_eStateType = eStateType;
  m_pAnimSet = m_pEnt->m_pFullBodyAnimIDLookup[eStateType];
}

void AnimatedWarrior_cl::FullBodyIdleState::OnEnter()
{
  m_eTriggeredAction = ACTION_NONE;
}

bool AnimatedWarrior_cl::FullBodyIdleState::Stop()
{
  m_eTriggeredAction = ACTION_NONE;
  return true;
}

bool AnimatedWarrior_cl::FullBodyIdleState::Walk()
{
  m_eTriggeredAction = ACTION_WALK;
  return true;
}

bool AnimatedWarrior_cl::FullBodyIdleState::Run()
{
  m_eTriggeredAction = ACTION_RUN;
  return true;
}

bool AnimatedWarrior_cl::FullBodyIdleState::WalkBackwards()
{
  m_eTriggeredAction = ACTION_WALKBACKWARDS;
  return true;
}

bool AnimatedWarrior_cl::FullBodyIdleState::RotateLeft(float fRotationSpeed)
{ 
  // switch to rotate state immediately
  if(m_pEnt->GetFullBodyState()!=m_pEnt->m_pFullBodyRotateState[m_eStateType])
    m_pEnt->SetFullBodyState(m_pEnt->m_pFullBodyRotateState[m_eStateType]);
//  else
//    int iTest = 0;
  return m_pEnt->GetFullBodyState()->RotateLeft(fRotationSpeed);
}

bool AnimatedWarrior_cl::FullBodyIdleState::RotateRight(float fRotationSpeed)
{
  // switch to rotate state immediately
  if(m_pEnt->GetFullBodyState()!=m_pEnt->m_pFullBodyRotateState[m_eStateType])
    m_pEnt->SetFullBodyState(m_pEnt->m_pFullBodyRotateState[m_eStateType]);
  return m_pEnt->GetFullBodyState()->RotateRight(fRotationSpeed);
}

void AnimatedWarrior_cl::FullBodyIdleState::OnThink()
{
  // perform pending actions which depend on the animation component having finished blending
  if ( m_eTriggeredAction != ACTION_NONE && !IsBlending() )
  {
    switch(m_eTriggeredAction)
    {
      case ACTION_WALK:
        m_pEnt->BlendOverFullBodyAnimation(0.3f, m_pAnimSet->iWalkID, 1.f, 0.f);
        m_pEnt->SetFullBodyState(m_pEnt->m_pFullBodyWalkState[m_eStateType]);
        break;
      case ACTION_RUN:
        m_pEnt->BlendOverFullBodyAnimation(0.3f, m_pAnimSet->iRunID, 1.f, 0.f);
        m_pEnt->SetFullBodyState(m_pEnt->m_pFullBodyRunState[m_eStateType]);
        break;
      case ACTION_WALKBACKWARDS:
        m_pEnt->BlendOverFullBodyAnimation(0.3f, m_pAnimSet->iWalkBackwardsID, 1.f, 0.f);
        m_pEnt->SetFullBodyState(m_pEnt->m_pFullBodyWalkBackwardsState[m_eStateType]);
        break;
    }
    m_eTriggeredAction = ACTION_NONE;
  }
}

bool AnimatedWarrior_cl::FullBodyIdleState::Arm()
{
  if(m_eStateType != FBSTATETYPE_NORMAL)
    return false;

  // continue the new "armed" animation at the current position
  m_pEnt->BlendOverFullBodyAnimation(0.3f, ANIMID_IDLE_TORCH, 1.f, -1.f);
  m_pEnt->SetFullBodyState(m_pEnt->m_pFullBodyIdleState[FBSTATETYPE_ARMED]);

  return true;
}

bool AnimatedWarrior_cl::FullBodyIdleState::DisArm()
{
  if ( m_eStateType != FBSTATETYPE_ARMED )
    return false;

  // continue the new "disarmed" animation at the current position
  m_pEnt->BlendOverFullBodyAnimation(0.3f, ANIMID_IDLE, 1.f, -1.f);
  m_pEnt->SetFullBodyState(m_pEnt->m_pFullBodyIdleState[FBSTATETYPE_NORMAL]);

  return true;
}


// **************************************************
//            FULL BODY STATE: ROTATE
// **************************************************
AnimatedWarrior_cl::FullBodyRotateState::FullBodyRotateState( AnimatedWarrior_cl *pEntity, const char *szName, FBSTATETYPE eStateType ) : FullBodyState(szName, pEntity)
{
  m_eStateType = eStateType;
  m_pAnimSet = m_pEnt->m_pFullBodyAnimIDLookup[eStateType];
}

void AnimatedWarrior_cl::FullBodyRotateState::OnEnter()
{
  m_rotateDir = ROTATE_NONE;
  m_bAnimStartPending = false;
  m_eTriggeredAction = ACTION_NONE;
}

bool AnimatedWarrior_cl::FullBodyRotateState::RotateLeft(float fRotationSpeed)
{ 
  if(m_rotateDir != ROTATE_LEFT)
  {
    m_bAnimStartPending = false;

    // delay animation start if component is still blending
    if(IsBlending())
      m_bAnimStartPending = true;
    else
    {
      // if the charakter is already in rotation state we change the playback direction
      if(m_rotateDir == ROTATE_RIGHT)
      {
        VisAnimControl_cl* pControl = m_pEnt->m_spPrimaryFullBodyControl;
        float fCurrentSpeed = pControl->GetSpeed();
        pControl->SetSpeed(-fCurrentSpeed);
      }
      else
        m_pEnt->BlendOverFullBodyAnimation(0.2f, m_pAnimSet->iTurnID, -1.f, 0.f);
    }
    m_rotateDir = ROTATE_LEFT;
  }
  return true;
}

bool AnimatedWarrior_cl::FullBodyRotateState::RotateRight(float fRotationSpeed)
{
  if (m_rotateDir != ROTATE_RIGHT)
  {
    m_bAnimStartPending = false;

    // delay animation start if component is still blending
    if (IsBlending())
      m_bAnimStartPending = true;
    else
    {
      // if the charakter is already in rotation state we change the playback direction
      if(m_rotateDir == ROTATE_LEFT)
      {
        VisAnimControl_cl* pControl = m_pEnt->m_spPrimaryFullBodyControl;
        float fCurrentSpeed = pControl->GetSpeed();
        pControl->SetSpeed(fCurrentSpeed);
      }
      else
        m_pEnt->BlendOverFullBodyAnimation(0.2f, m_pAnimSet->iTurnID, 1.f, 0.f);
    }
    m_rotateDir = ROTATE_RIGHT;
  }

  return true;
}

bool AnimatedWarrior_cl::FullBodyRotateState::Stop()
{
  m_eTriggeredAction = ACTION_STOP;
  return true;
}

bool AnimatedWarrior_cl::FullBodyRotateState::Walk()
{
  m_eTriggeredAction = ACTION_WALK;
  return true;
}

bool AnimatedWarrior_cl::FullBodyRotateState::Run()
{
  m_eTriggeredAction = ACTION_RUN;
  return true;
}

bool AnimatedWarrior_cl::FullBodyRotateState::WalkBackwards()
{
  m_eTriggeredAction = ACTION_WALKBACKWARDS;
  return true;
}

void AnimatedWarrior_cl::FullBodyRotateState::OnThink()
{
  float fRotationSpeed = ANIMATEDCHARACTER_ROTATIONSPEED / 2.0f;

  // apply entity rotation
  if ( m_rotateDir == ROTATE_LEFT )
    DoRotateLeft(fRotationSpeed);
  else
    DoRotateRight(fRotationSpeed);

  // start any delayed rotate animation as soon as the component finished blending
  if (m_bAnimStartPending && !IsBlending())
  {
//    int iMixerInputIndex = m_eStateType;

    if (m_rotateDir == ROTATE_LEFT)
    {
      // change playback direction
      m_pEnt->BlendOverFullBodyAnimation(0.2f, m_pAnimSet->iTurnID, -1.f, 0.f);
    }
    else
    {
      // change playback direction
      m_pEnt->BlendOverFullBodyAnimation(0.2f, m_pAnimSet->iTurnID, 1.f, 0.f);
    }
    m_bAnimStartPending = false;
  }

  // perform pending actions which depend on the animation component having finished blending
  if ( m_eTriggeredAction != ACTION_NONE && !IsBlending() )
  {
    switch( m_eTriggeredAction )
    {
      case ACTION_STOP:
        m_pEnt->BlendOverFullBodyAnimation(0.5f, m_pAnimSet->iIdleID, 1.f, 0.f);
        m_pEnt->SetFullBodyState(m_pEnt->m_pFullBodyIdleState[m_eStateType]);
        break;
      case ACTION_WALK:
        m_pEnt->BlendOverFullBodyAnimation(0.3f, m_pAnimSet->iWalkID, 1.f, 0.f);
        m_pEnt->SetFullBodyState(m_pEnt->m_pFullBodyWalkState[m_eStateType]);
        break;
      case ACTION_RUN:
        m_pEnt->BlendOverFullBodyAnimation(0.3f, m_pAnimSet->iRunID, 1.f, 0.f);
        m_pEnt->SetFullBodyState( m_pEnt->m_pFullBodyRunState[m_eStateType] );
        break;
      case ACTION_WALKBACKWARDS:
        m_pEnt->BlendOverFullBodyAnimation(0.3f, m_pAnimSet->iWalkBackwardsID, 1.f, 0.f);
        m_pEnt->SetFullBodyState(m_pEnt->m_pFullBodyWalkBackwardsState[m_eStateType]);
        break;
    }
    m_eTriggeredAction = ACTION_NONE;
  }
}

bool AnimatedWarrior_cl::FullBodyRotateState::Arm()
{
  if ( m_eStateType != FBSTATETYPE_NORMAL )
    return false;

  // continue the new "armed" animation at the current position
  m_pEnt->BlendOverFullBodyAnimation(0.3f, ANIMID_TURN_TORCH, 1.f, -1.f);
  m_pEnt->SetFullBodyState( m_pEnt->m_pFullBodyRotateState[FBSTATETYPE_ARMED] );

  return true;
}

bool AnimatedWarrior_cl::FullBodyRotateState::DisArm()
{
  if ( m_eStateType != FBSTATETYPE_ARMED )
    return false;

  // continue the new "disarmed" animation at the current position
  m_pEnt->BlendOverFullBodyAnimation(0.3f, ANIMID_TURN, 1.f, -1.f);
  m_pEnt->SetFullBodyState( m_pEnt->m_pFullBodyRotateState[FBSTATETYPE_NORMAL] );

  return true;
}


// **************************************************
//            FULL BODY STATE: WALK
// **************************************************
AnimatedWarrior_cl::FullBodyWalkState::FullBodyWalkState( AnimatedWarrior_cl *pEntity, const char *szName, FBSTATETYPE eStateType ) : FullBodyState(szName, pEntity)
{
  m_eStateType = eStateType;
  m_pAnimSet = m_pEnt->m_pFullBodyAnimIDLookup[eStateType];
}

void AnimatedWarrior_cl::FullBodyWalkState::OnEnter()
{
  m_eTriggeredAction = ACTION_NONE;
}

bool AnimatedWarrior_cl::FullBodyWalkState::Stop()
{
  m_eTriggeredAction = ACTION_STOP;
  return true;
}

bool AnimatedWarrior_cl::FullBodyWalkState::Run()
{
  m_eTriggeredAction = ACTION_RUN;
  return true;
}

bool AnimatedWarrior_cl::FullBodyWalkState::WalkBackwards()
{
  m_eTriggeredAction = ACTION_WALKBACKWARDS;
  return true;
}


void AnimatedWarrior_cl::FullBodyWalkState::OnEvent(int iEvent)
{
  /*
  // todo: check if we can do that without the events directly!!
  // perform pending "run" action as soon as a LEGSPARALLEL event gets triggered
  if ((iEvent == EVENT_ANIM_LEGSPARALLEL_FIRSTTIME || iEvent == EVENT_ANIM_LEGSPARALLEL_SECONDTIME) && 
       (m_eTriggeredAction == ACTION_RUN))
  {
    // todo: check if we really need to set it to LEGSPARALLEL or not?
    m_pEnt->BlendOverFullBodyAnimation(0.5f, m_pAnimSet->iRunID, 1.f, -1.f);
    m_pEnt->SetFullBodyState(m_pEnt->m_pFullBodyRunState[m_eStateType]);
    m_eTriggeredAction = ACTION_NONE;
  }
  */
}


void AnimatedWarrior_cl::FullBodyWalkState::OnThink()
{
  // perform pending actions which depend on the animation component having finished blending
  if(m_eTriggeredAction != ACTION_NONE && !IsBlending())
  {
    switch( m_eTriggeredAction )
    {
      case ACTION_STOP:
        m_pEnt->BlendOverFullBodyAnimation(0.5f, m_pAnimSet->iIdleID, 1.f, 0.f);
        m_pEnt->SetFullBodyState( m_pEnt->m_pFullBodyIdleState[m_eStateType]);
        m_eTriggeredAction = ACTION_NONE;
        break;
      case ACTION_WALKBACKWARDS:
        m_pEnt->BlendOverFullBodyAnimation(0.5f, m_pAnimSet->iWalkBackwardsID, 1.f, 0.f);
        m_pEnt->SetFullBodyState( m_pEnt->m_pFullBodyWalkBackwardsState[m_eStateType]);
        m_eTriggeredAction = ACTION_NONE;
        break;
      case ACTION_RUN:
        m_pEnt->BlendOverFullBodyAnimation(0.3f, m_pAnimSet->iRunID, 1.f, -1.f);
        m_pEnt->SetFullBodyState( m_pEnt->m_pFullBodyRunState[m_eStateType]);
        m_eTriggeredAction = ACTION_NONE;
    }
  }
}

bool AnimatedWarrior_cl::FullBodyWalkState::Arm()
{
  if ( m_eStateType != FBSTATETYPE_NORMAL )
    return false;

  // continue the new "armed" animation at the current position
  m_pEnt->BlendOverFullBodyAnimation(0.3f, ANIMID_WALK_TORCH, 1.f, -1.f);
  m_pEnt->SetFullBodyState( m_pEnt->m_pFullBodyWalkState[FBSTATETYPE_ARMED] );
  return true;
}

bool AnimatedWarrior_cl::FullBodyWalkState::DisArm()
{
  if ( m_eStateType != FBSTATETYPE_ARMED )
    return false;

  // continue the new "disarmed" animation at the current position
  m_pEnt->BlendOverFullBodyAnimation(0.3f, ANIMID_WALK, 1.f, -1.f);
  m_pEnt->SetFullBodyState( m_pEnt->m_pFullBodyWalkState[FBSTATETYPE_NORMAL] );
  return true;
}


// **************************************************
//            FULL BODY STATE: RUN
// **************************************************
AnimatedWarrior_cl::FullBodyRunState::FullBodyRunState( AnimatedWarrior_cl *pEntity, const char *szName, FBSTATETYPE eStateType ) : FullBodyState(szName, pEntity)
{
  m_eStateType = eStateType;
  m_pAnimSet = m_pEnt->m_pFullBodyAnimIDLookup[eStateType];
}

void AnimatedWarrior_cl::FullBodyRunState::OnEnter()
{
  m_eTriggeredAction = ACTION_NONE;
}

bool AnimatedWarrior_cl::FullBodyRunState::Stop()
{
  m_eTriggeredAction = ACTION_STOP;
  return true;
}

bool AnimatedWarrior_cl::FullBodyRunState::Walk()
{
  m_eTriggeredAction = ACTION_WALK;
  return true;
}

bool AnimatedWarrior_cl::FullBodyRunState::WalkBackwards()
{
  m_eTriggeredAction = ACTION_WALKBACKWARDS;
  return true;
}


void AnimatedWarrior_cl::FullBodyRunState::OnEvent(int iEvent)
{
/*
  // perform pending "walk" action as soon as a LEGSPARALLEL event gets triggered
  if ((iEvent == EVENT_ANIM_LEGSPARALLEL_FIRSTTIME || iEvent == EVENT_ANIM_LEGSPARALLEL_SECONDTIME) && 
       (m_eTriggeredAction == ACTION_WALK))
  {
    int iMixerInputIndex = m_eStateType;
    m_pEnt->BlendOverFullBodyAnimation(0.5f, m_pAnimSet->iWalkID, 1.f, -1.f);
    m_pEnt->SetFullBodyState( m_pEnt->m_pFullBodyWalkState[m_eStateType]);
    m_eTriggeredAction = ACTION_NONE;
  }
  */
}


void AnimatedWarrior_cl::FullBodyRunState::OnThink()
{
  VASSERT(m_pEnt->m_spPrimaryFullBodyControl->m_iMixerInputIndex!=-1);

  // perform pending actions which depend on the animation component having finished blending
  if (m_eTriggeredAction != ACTION_NONE && !IsBlending())
  {
    switch(m_eTriggeredAction)
    {
      case ACTION_STOP:
        m_pEnt->BlendOverFullBodyAnimation(0.5f, m_pAnimSet->iIdleID, 1.f, 0.f);
        m_pEnt->SetFullBodyState( m_pEnt->m_pFullBodyIdleState[m_eStateType] );
        m_eTriggeredAction = ACTION_NONE;
        break;
      case ACTION_WALKBACKWARDS:
        m_pEnt->BlendOverFullBodyAnimation(0.5f, m_pAnimSet->iWalkBackwardsID, 1.f, 0.f);
        m_pEnt->SetFullBodyState(m_pEnt->m_pFullBodyWalkBackwardsState[m_eStateType]);
        m_eTriggeredAction = ACTION_NONE;
        break;
      case ACTION_WALK:
        m_pEnt->BlendOverFullBodyAnimation(0.3f, m_pAnimSet->iWalkID, 1.f, -1.f);
        m_pEnt->SetFullBodyState(m_pEnt->m_pFullBodyWalkState[m_eStateType]);
        m_eTriggeredAction = ACTION_NONE;
        break;
    }
  }
} 

bool AnimatedWarrior_cl::FullBodyRunState::DisArm()
{
  if ( m_eStateType != FBSTATETYPE_ARMED )
    return false;

  // continue the new "armed" animation at the current position
  m_pEnt->BlendOverFullBodyAnimation(0.3f, ANIMID_RUN, 1.f, -1.f);
  m_pEnt->SetFullBodyState( m_pEnt->m_pFullBodyRunState[FBSTATETYPE_NORMAL] );
  return true;
}

bool AnimatedWarrior_cl::FullBodyRunState::Arm()
{
  if ( m_eStateType != FBSTATETYPE_NORMAL )
    return false;

  // continue the new "disarmed" animation at the current position
  m_pEnt->BlendOverFullBodyAnimation(0.3f, ANIMID_RUN_TORCH, 1.f, -1.f);
  m_pEnt->SetFullBodyState( m_pEnt->m_pFullBodyRunState[FBSTATETYPE_ARMED] );
  return true;
}


// **************************************************
//            FULL BODY STATE: WALK BACKWARDS
// **************************************************
AnimatedWarrior_cl::FullBodyWalkBackwardsState::FullBodyWalkBackwardsState( AnimatedWarrior_cl *pEntity, const char *szName, FBSTATETYPE eStateType ) : FullBodyState(szName, pEntity)
{
  m_eStateType = eStateType;
  m_pAnimSet = m_pEnt->m_pFullBodyAnimIDLookup[eStateType];
}

void AnimatedWarrior_cl::FullBodyWalkBackwardsState::OnEnter()
{
  m_eTriggeredAction = ACTION_NONE;
}

bool AnimatedWarrior_cl::FullBodyWalkBackwardsState::Stop()
{
  m_eTriggeredAction = ACTION_STOP;
  return true;
}

bool AnimatedWarrior_cl::FullBodyWalkBackwardsState::Walk()
{
  m_eTriggeredAction = ACTION_WALK;
  return false;
}

bool AnimatedWarrior_cl::FullBodyWalkBackwardsState::Run()
{
  m_eTriggeredAction = ACTION_RUN;
  return false;
}

void AnimatedWarrior_cl::FullBodyWalkBackwardsState::OnThink()
{
  // perform pending actions which depend on the animation component having finished blending
  if ( m_eTriggeredAction != ACTION_NONE && !IsBlending() )
  {
    switch(m_eTriggeredAction)
    {
      case ACTION_STOP:
        m_pEnt->BlendOverFullBodyAnimation(0.5f, m_pAnimSet->iIdleID, 1.f, 0.f);
        m_pEnt->SetFullBodyState(m_pEnt->m_pFullBodyIdleState[m_eStateType]);
        break;
      case ACTION_WALK:
        m_pEnt->BlendOverFullBodyAnimation(0.5f, m_pAnimSet->iWalkID, 1.f, 0.f);
        m_pEnt->SetFullBodyState(m_pEnt->m_pFullBodyWalkState[m_eStateType]);
        break;
      case ACTION_RUN:
        m_pEnt->BlendOverFullBodyAnimation(0.5f, m_pAnimSet->iRunID, 1.f, 0.f);
        m_pEnt->SetFullBodyState(m_pEnt->m_pFullBodyRunState[m_eStateType]);
        break;
    }
    m_eTriggeredAction = ACTION_NONE;
  }
}

bool AnimatedWarrior_cl::FullBodyWalkBackwardsState::DisArm()
{
  if ( m_eStateType != FBSTATETYPE_ARMED )
    return false;

  // continue the new "armed" animation at the current position
  m_pEnt->BlendOverFullBodyAnimation(0.3f, ANIMID_WALKBACKWARDS, 1.f, -1.f);
  m_pEnt->SetFullBodyState(m_pEnt->m_pFullBodyWalkBackwardsState[FBSTATETYPE_NORMAL]);

  return true;
}

bool AnimatedWarrior_cl::FullBodyWalkBackwardsState::Arm()
{
  if ( m_eStateType != FBSTATETYPE_NORMAL )
    return false;

  // continue the new "disarmed" animation at the current position
  m_pEnt->BlendOverFullBodyAnimation(0.3f, ANIMID_WALKBACKWARDS_TORCH, 1.f, -1.f);
  m_pEnt->SetFullBodyState( m_pEnt->m_pFullBodyWalkBackwardsState[FBSTATETYPE_ARMED] );

  return true;
}


bool AnimatedWarrior_cl::UpperBodyState::IsBlending()
{
  return m_pEnt->m_pLayerMixer->IsEasingIn(m_pEnt->m_iPrimaryUpperBodyControlIndex);
}


// **************************************************
//            UPPER BODY STATE: IDLE
// **************************************************
bool AnimatedWarrior_cl::UpperBodyIdleState::Arm()
{
  m_eTriggeredAction = ACTION_DRAW_TORCH;

  return true;
}

void AnimatedWarrior_cl::UpperBodyIdleState::OnThink()
{
  // perform pending actions which depend on the animation component having finished blending
  if ( m_eTriggeredAction != ACTION_NONE && (m_pEnt->m_iPrimaryUpperBodyControlIndex==-1||!IsBlending()) )
  {
    switch(m_eTriggeredAction)
    {
      case ACTION_DRAW_TORCH:
        m_pEnt->BlendOverUpperBodyAnimation(0.2f, ANIMID_UPPERBODY_ARM, 1.f, 0.f);
        m_pEnt->SetUpperBodyState(m_pEnt->m_pUpperBodyDrawTorchState);
        m_eTriggeredAction = ACTION_NONE;
        break;
    }
  }
}


// **************************************************
//            UPPER BODY STATE: IDLE ARMED
// **************************************************
bool AnimatedWarrior_cl::UpperBodyIdleArmedState::DisArm()
{
  m_eTriggeredAction = ACTION_HOLSTER_TORCH;

  return true;
}

bool AnimatedWarrior_cl::UpperBodyIdleArmedState::Attack()
{
  m_eTriggeredAction = ACTION_ATTACK;

  return true;
}

void AnimatedWarrior_cl::UpperBodyIdleArmedState::OnThink()
{
  // perform pending actions which depend on the animation component having finished blending
  if ( m_eTriggeredAction != ACTION_NONE && (m_pEnt->m_iPrimaryUpperBodyControlIndex==-1||!IsBlending()) )
  {
    switch(m_eTriggeredAction)
    {
      case ACTION_HOLSTER_TORCH:
        m_pEnt->BlendOverUpperBodyAnimation(0.2f, ANIMID_UPPERBODY_DISARM, 1.f, 0.f);
        m_pEnt->SetUpperBodyState(m_pEnt->m_pUpperBodyHolsterTorchState);
        break;
      case ACTION_ATTACK:
        m_pEnt->BlendOverUpperBodyAnimation(0.2f, ANIMID_UPPERBODY_ATTACK, 1.f, 0.f);
        m_pEnt->SetUpperBodyState(m_pEnt->m_pUpperBodyAttackingState);
        m_pEnt->m_spUpperBodyControls[m_pEnt->m_iPrimaryUpperBodyControlIndex]->Play();
        break;
    }
    m_eTriggeredAction = ACTION_NONE;
  }
}


// **************************************************
//            UPPER BODY STATE: Draw Torch
// **************************************************
void AnimatedWarrior_cl::UpperBodyDrawTorchState::OnEnter()
{
  m_eTriggeredAction = ACTION_NONE;
  m_pEnt->StartUpperBodyFadeIn();
}

bool AnimatedWarrior_cl::UpperBodyDrawTorchState::IdleArmed()
{
  m_eTriggeredAction = ACTION_IDLE_ARMED;

  return true;
}

bool AnimatedWarrior_cl::UpperBodyDrawTorchState::DisArm()
{
  m_eTriggeredAction = ACTION_HOLSTER_TORCH;

  return true;
}


void AnimatedWarrior_cl::UpperBodyDrawTorchState::OnEvent(int iEvent)
{
  if (iEvent == EVENT_ANIM_UPPERBODY_ARMING_FINISHED)
  {
    // arming is finished, thus fade back to full body again
    m_pEnt->BlendOutUpperBodyAnimation(0.2f);
    m_pEnt->SetUpperBodyState(m_pEnt->m_pUpperBodyIdleArmedState);
  }
  else if (iEvent == EVENT_UPPERBODYFADEIN_FINISHED)
  {
    // continue arm animation
    m_pEnt->m_spUpperBodyControls[m_pEnt->m_iPrimaryUpperBodyControlIndex]->Resume();
    m_pEnt->GetFullBodyState()->Arm();
    m_pEnt->ShowTorch();
  }
}


void AnimatedWarrior_cl::UpperBodyDrawTorchState::OnThink()
{
  // perform pending actions which depend on the animation component having finished blending
  if ( m_eTriggeredAction != ACTION_NONE && (m_pEnt->m_iPrimaryUpperBodyControlIndex==-1||!IsBlending()) )
  {
    switch(m_eTriggeredAction)
    {
      case ACTION_IDLE_ARMED:
        m_pEnt->BlendOverUpperBodyAnimation(0.2f, -1, 1.f, 0.f);
        m_pEnt->SetUpperBodyState(m_pEnt->m_pUpperBodyIdleState);
        break;
      case ACTION_HOLSTER_TORCH:
        m_pEnt->BlendOverUpperBodyAnimation(0.2f, ANIMID_UPPERBODY_DISARM, 1.f, -1.f);
        m_pEnt->SetUpperBodyState(m_pEnt->m_pUpperBodyHolsterTorchState);
        break;
    }
    m_eTriggeredAction = ACTION_NONE;
  }
}


// **************************************************
//            UPPER BODY STATE: Holster Torch
// **************************************************
void AnimatedWarrior_cl::UpperBodyHolsterTorchState::OnEnter()
{
  m_eTriggeredAction = ACTION_NONE;
  m_pEnt->StartUpperBodyFadeIn();
}

bool AnimatedWarrior_cl::UpperBodyHolsterTorchState::Idle()
{
  m_eTriggeredAction = ACTION_IDLE;

  return true;
}

bool AnimatedWarrior_cl::UpperBodyHolsterTorchState::Arm()
{
  m_eTriggeredAction = ACTION_DRAW_TORCH;

  return true;
}


void AnimatedWarrior_cl::UpperBodyHolsterTorchState::OnEvent(int iEvent)
{
  if(iEvent == EVENT_ANIM_UPPERBODY_DISARMING_FINISHED)
  {
    // arming is finished, thus fade back to full body again
    m_pEnt->HideTorch();
    m_pEnt->BlendOutUpperBodyAnimation(0.2f);
    m_pEnt->SetUpperBodyState(m_pEnt->m_pUpperBodyIdleState);
  }
  else if(iEvent == EVENT_UPPERBODYFADEIN_FINISHED)
  {
    // continue arm animation
    m_pEnt->m_spUpperBodyControls[m_pEnt->m_iPrimaryUpperBodyControlIndex]->Resume();
    m_pEnt->GetFullBodyState()->DisArm();
  }
}

void AnimatedWarrior_cl::UpperBodyHolsterTorchState::OnThink()
{
  // perform pending actions which depend on the animation component having finished blending
  if ( m_eTriggeredAction != ACTION_NONE && (m_pEnt->m_iPrimaryUpperBodyControlIndex==-1||!IsBlending()) )
  {
    switch(m_eTriggeredAction)
    {
      case ACTION_IDLE:
        m_pEnt->BlendOverUpperBodyAnimation(0.2f, -1, 1.f, 0.f);
        m_pEnt->SetUpperBodyState(m_pEnt->m_pUpperBodyIdleState);
        break;
      case ACTION_DRAW_TORCH:
        m_pEnt->BlendOverUpperBodyAnimation(0.2f, ANIMID_UPPERBODY_ARM, 1.f, -1.f);
        m_pEnt->SetUpperBodyState(m_pEnt->m_pUpperBodyDrawTorchState);
        break;
    }

    m_eTriggeredAction = ACTION_NONE;
  }
}


// **************************************************
//            UPPER BODY STATE: ATTACKING
// **************************************************
void AnimatedWarrior_cl::UpperBodyAttackingState::OnEnter()
{
  m_eTriggeredAction = ACTION_NONE;
}

void AnimatedWarrior_cl::UpperBodyAttackingState::OnEvent(int iEvent)
{
  if(iEvent == EVENT_ANIM_UPPERBODY_ATTACK_FINISHED)
  {
    // attacking is finished, thus fade back to full body again
    m_pEnt->BlendOutUpperBodyAnimation(0.2f);
    m_pEnt->SetUpperBodyState(m_pEnt->m_pUpperBodyIdleArmedState);
  }
}


V_IMPLEMENT_SERIAL( AnimatedWarrior_cl, VisBaseEntity_cl, 0, Vision::GetEngineModule() );
START_VAR_TABLE(AnimatedWarrior_cl, VisBaseEntity_cl, "Animated warrior character for the samples", VFORGE_HIDECLASS, "Models\\ElfWarrior.model")
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
