/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Samples/Engine/BasicAnimation/BasicAnimationPCH.h>
#include <Vision/Samples/Engine/BasicAnimation/SimpleSkeletalAnimatedObject.h>


// animation event IDs
const float BONE_HIGHLIGHT_FADEOUTTIME = 0.5f;

SimpleSkeletalAnimatedObject_cl::SimpleSkeletalAnimatedObject_cl(void)
{
  pLookAtTarget = NULL;
  fLookAtRotationPhase = NULL;
}

void SimpleSkeletalAnimatedObject_cl::InitFunction()
{
  ClearData();

  SetCastShadows(TRUE);

  ANIMEVENT_FOOTSTEP_LEFT   = Vision::Animations.RegisterEvent("footstep_left");
  ANIMEVENT_FOOTSTEP_RIGHT  = Vision::Animations.RegisterEvent("footstep_right");

  IncOrientation(90.f,0.f,0.f);

}

void SimpleSkeletalAnimatedObject_cl::SetActivate(bool bStatus) 
{
  SetVisibleBitmask(bStatus ? 0xffffffff:0);
  SetCastShadows(bStatus);
  if (pLookAtTarget) pLookAtTarget->SetVisibleBitmask(bStatus ? 0xffffffff:0);

  if(bStatus == true)
    SetMode(sampleMode);
  else
    SetAnimConfig(NULL);
}

void SimpleSkeletalAnimatedObject_cl::SetMode(SampleMode_e newMode)
{
  ClearData();
  sampleMode = newMode;

  switch (newMode)
  {
  case MODE_SINGLEANIM:
    StartSingleAnimation(false); break;
  case MODE_BLENDTWOANIMS:
    BlendTwoAnimations(); break;
  case MODE_LISTENTOEVENTS:
    ListenToEvents(); break;
  case MODE_SETANIMATIONTIME:
    SetAnimationTime(); break;
  case MODE_LAYERTWOANIMATIONS:
    LayerTwoAnimations(); break;
  case MODE_FORWARDKINEMATICS:
    ForwardKinematics(); break;
  case MODE_LOOKAT:
    LookAt(); break;
  }

  // disable motion delta for this sample
  if( GetAnimConfig() )
    GetAnimConfig()->SetFlags(GetAnimConfig()->GetFlags() & (~APPLY_MOTION_DELTA));
}

void SimpleSkeletalAnimatedObject_cl::ClearData()
{
  SetAnimConfig(NULL);
  m_spNormalizeMixerNode = NULL;
  m_spLayerMixerNode = NULL;
  m_spSingleAnimControl = NULL;
  m_spBoneModifierNode = NULL;
  fBoneHighlightDuration = 0.f;
  V_SAFE_REMOVE(pLookAtTarget);
  fLookAtRotationPhase = 0.f;
}



bool SimpleSkeletalAnimatedObject_cl::GetHeadRotation(const hkvVec3 &vTargetPos, hkvQuat &targetRot, float fMaxViewAngle)
{
  hkvMat4 objectMatrix;
  hkvMat4 parentObjectMatrix;

  // get the index of the parent bone
  int parentBoneIndex = GetMesh()->GetSkeleton()->GetBone(m_iHeadBoneIndex)->m_iParentIndex;

  // get the 4x4 matrix of the head bone and its parent in object space
  const VisSkeletalAnimResult_cl* pObjectSpaceResult = GetAnimConfig()->GetFinalResult()->GetCurrentObjectSpaceResult();
  pObjectSpaceResult->GetBoneTransformationMatrix(m_iHeadBoneIndex, objectMatrix);
  pObjectSpaceResult->GetBoneTransformationMatrix(parentBoneIndex, parentObjectMatrix);

  // get the current position of the target in object space
  hkvVec3 targetObjectspace = TransformToObjectSpace(vTargetPos);

  // get the current position of the bone in object space
  hkvVec3 boneObjectspace = objectMatrix.getTranslation();

  // calculate the look at direction in object space (just for the angle constraints)
  hkvVec3 vLookAtDir = targetObjectspace - boneObjectspace;

  // check if we are within the angle constraints
  vLookAtDir.normalizeIfNotZero();
  float fCosMax = hkvMath::cosDeg (180.f - fMaxViewAngle);
  bool bInsideFocusArea = vLookAtDir.y < fCosMax;
  if (!bInsideFocusArea)
  {
    // out of focus, so just look straight ahead
    targetRot.setIdentity();
    return false;
  }

  // create a lookat-3x3 matrix from the bone position and the target position (both are in object space)
  // This orientation is later used to replace(!) the head bone in local space (VIS_REPLACE_BONE|VIS_LOCAL_SPACE flags)
  hkvMat3 lookatMatrix(hkvNoInitialization);
  lookatMatrix.setLookInDirectionMatrix (targetObjectspace - boneObjectspace);

  //  Apply our custom rotation matrix that tells the bone to focus with the eyes and not with the back of the head :-)
  lookatMatrix = lookatMatrix.multiply (m_RelativeHeadOrientation);

  // Invert the parents object matrix rotation to cancel out all parent bone orientations
  hkvMat3 parentObject = parentObjectMatrix.getRotationalPart();
  parentObject.invert();

  // put everything together
  hkvMat3 result = parentObject;
  result = result.multiply (lookatMatrix);

  // ...and make a quaternion from it
  targetRot.setFromMat3 (result);

  return true;
}

  
void SimpleSkeletalAnimatedObject_cl::UpdateLookatHeadRotation(float fTimeDelta)
{
  hkvQuat targetRotation;

  // evaluate the target rotation
  GetHeadRotation(pLookAtTarget->GetPosition(), targetRotation, 75.f);

  hkvVec3 vForwards(0.0f, -1.0f, 0.0f);
  hkvVec3 vFrom = m_CurrentNeckRotation.transform(vForwards);
  hkvVec3 vTo = targetRotation.transform(vForwards);
  float fAngle = vFrom.getAngleBetween (vTo);

  float fAnglePerSecond = 20.0f * fTimeDelta;
  float fFactor = hkvMath::Min(fAnglePerSecond / (fAngle + 0.000001f), 1.0f);
  m_CurrentNeckRotation.setSlerp(m_CurrentNeckRotation, targetRotation, fFactor); 

  // Our new bone orientation defines the rotation absolute.
  // Thus our modify mode must be VIS_REPLACE_BONE and VIS_LOCAL_SPACE
  GetAnimConfig()->GetFinalResult()->SetCustomBoneRotation(m_iHeadBoneIndex, m_CurrentNeckRotation, VIS_REPLACE_BONE|VIS_LOCAL_SPACE);

}



void SimpleSkeletalAnimatedObject_cl::ThinkFunction()
{
  // First check whether the object is actually active
  if (GetAnimConfig() == NULL)
    return;

  float dtime = Vision::GetTimer()->GetTimeDifference();

  // highlight the bone after animation event
  if (fBoneHighlightDuration > 0.0f && !sHighlightedBoneName.IsEmpty())
  {
    int iColorVal = (int) (fBoneHighlightDuration / BONE_HIGHLIGHT_FADEOUTTIME * 255.0f);
    DrawBoneBoundingBox(sHighlightedBoneName, VColorRef(iColorVal,iColorVal, 0), 2.f);
    fBoneHighlightDuration -= Vision::GetTimer()->GetTimeDifference();
  }

  // update the lookat
  if (sampleMode==MODE_LOOKAT)
  {
    VASSERT(pLookAtTarget);

    // rotate the object around the character
    fLookAtRotationPhase = hkvMath::mod (fLookAtRotationPhase+dtime*0.7f, hkvMath::pi () * 2.0f);
    hkvVec3 vNewPos(100.f*hkvMath::sinRad (fLookAtRotationPhase),100.f*hkvMath::cosRad (fLookAtRotationPhase),160.f);
    vNewPos += GetPosition();
    pLookAtTarget->SetPosition(vNewPos);

    // update the lookat orientation
    UpdateLookatHeadRotation(dtime);
  }
}

void SimpleSkeletalAnimatedObject_cl::MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB)
{
  // forward events to state objects
  if (iID == VIS_MSG_EVENT)
  {
    INT_PTR iEventID = iParamA;

    if (iEventID==ANIMEVENT_FOOTSTEP_LEFT)
    {
      sHighlightedBoneName = "skeleton1:LeftFoot";
      fBoneHighlightDuration = BONE_HIGHLIGHT_FADEOUTTIME;
    }
    else if (iEventID==ANIMEVENT_FOOTSTEP_RIGHT)
    {
      sHighlightedBoneName = "skeleton1:RightFoot";
      fBoneHighlightDuration = BONE_HIGHLIGHT_FADEOUTTIME;
    }
  }
}



//
//                              *** MODE: START SINGLE ANIMATION ***
//

void SimpleSkeletalAnimatedObject_cl::StartSingleAnimation(bool bLooped)
{
  // start a simple skeletal animation
  int iFlags = VSKELANIMCTRL_DEFAULTS;
  if (bLooped)
    iFlags |= VANIMCTRL_LOOP;
  m_spSingleAnimControl = VisAnimConfig_cl::StartSkeletalAnimation(this, "Walk", iFlags);
}



//
//                              *** MODE: BLEND TWO ANIMATIONS ***
//


void SimpleSkeletalAnimatedObject_cl::SetBlendWalkToRun(bool bDirection)
{
  const float fBlendTime = 0.4f;
  VASSERT(GetMode()==MODE_BLENDTWOANIMS);
  if (bDirection) // walk to run
  {
    m_spNormalizeMixerNode->EaseIn(m_iMixerInputRun, fBlendTime, true);
    m_spNormalizeMixerNode->EaseOut(m_iMixerInputWalk, fBlendTime, true);
  }
  else // run to walk
  {
    m_spNormalizeMixerNode->EaseOut(m_iMixerInputRun, fBlendTime, true);
    m_spNormalizeMixerNode->EaseIn(m_iMixerInputWalk, fBlendTime, true);
  }
}


void SimpleSkeletalAnimatedObject_cl::BlendTwoAnimations()
{
  // Blend two animations using a NormalizeMixer node. The mixer gets the two animations as input
  // and generates the blended result. The animation tree looks as follows:
  //
  // - FinalSkeletalResult
  //    - NormalizeMixerNode
  //       - SkeletalAnimControl (Walk Animation)
  //       - SkeletalAnimControl (Run Animation)
  //
  // The weights of the two animations are set on the mixer instance.
  //

  // create a new AnimConfig instance
  VDynamicMesh *pMesh = GetMesh();
  VisAnimFinalSkeletalResult_cl* pFinalSkeletalResult;
  VisAnimConfig_cl* pConfig = VisAnimConfig_cl::CreateSkeletalConfig(pMesh, &pFinalSkeletalResult);

  // get skeletal animation sequence
  VisSkeletalAnimSequence_cl* pAnimSequenceWalk = (VisSkeletalAnimSequence_cl*)pMesh->GetSequence("Walk", VIS_MODELANIM_SKELETAL);
  VisSkeletalAnimSequence_cl* pAnimSequenceRun = (VisSkeletalAnimSequence_cl*)pMesh->GetSequence("Run", VIS_MODELANIM_SKELETAL);
  if(!pAnimSequenceWalk || !pAnimSequenceRun)
    return;

  // create two animation controls: walk and run (use a helper function for creating them)
  VSmartPtr<VisSkeletalAnimControl_cl> spWalkAnimControl = VisSkeletalAnimControl_cl::Create(pMesh->GetSkeleton(), pAnimSequenceWalk, VANIMCTRL_LOOP|VSKELANIMCTRL_DEFAULTS, 1.0f, true);
  VSmartPtr<VisSkeletalAnimControl_cl> spRunAnimControl = VisSkeletalAnimControl_cl::Create(pMesh->GetSkeleton(), pAnimSequenceRun, VANIMCTRL_LOOP|VSKELANIMCTRL_DEFAULTS, 1.0f, true);

  // create the mixer node that blends the two animations
  // (set initial weight to show walk animation only)
  m_spNormalizeMixerNode = new VisAnimNormalizeMixerNode_cl(pMesh->GetSkeleton());
  m_iMixerInputWalk = m_spNormalizeMixerNode->AddMixerInput(spWalkAnimControl, 1.0f);
  m_iMixerInputRun = m_spNormalizeMixerNode->AddMixerInput(spRunAnimControl, 0.0f);

  // finally set the mixer as the root animation node
  pFinalSkeletalResult->SetSkeletalAnimInput(m_spNormalizeMixerNode);
  SetAnimConfig(pConfig);

  // blend from walk to run
  SetBlendWalkToRun(true);
}


//
//                              *** MODE: LISTEN TO EVENTS ***
//

void SimpleSkeletalAnimatedObject_cl::ListenToEvents()
{
  // start a simple skeletalanimation
  VSmartPtr<VisSkeletalAnimControl_cl> spAnimControl = VisAnimConfig_cl::StartSkeletalAnimation(this, "Walk", VANIMCTRL_LOOP|VSKELANIMCTRL_DEFAULTS);
  spAnimControl->SetSpeed(0.5f);

  // register two animation events (time: when feet touch floor)
  VisAnimEventList_cl *pEventList = spAnimControl->GetEventList();
  pEventList->AddEvent(0.26f, ANIMEVENT_FOOTSTEP_RIGHT);
  pEventList->AddEvent(0.76f, ANIMEVENT_FOOTSTEP_LEFT);
  spAnimControl->AddEventListener(this);
}


//
//                              *** MODE: SET ANIMATION TIME ***
//

void SimpleSkeletalAnimatedObject_cl::SetAnimationTime()
{
  // start a simple skeletal animation
  m_spSingleAnimControl = VisAnimConfig_cl::StartSkeletalAnimation(this, "Walk", VANIMCTRL_LOOP|VSKELANIMCTRL_DEFAULTS);
  m_spSingleAnimControl->Pause();

  // set the animation time manually
  m_spSingleAnimControl->SetCurrentSequenceTime(0.79f);
}


//
//                              *** MODE: LAYER TWO ANIMATIONS ***
//

void SimpleSkeletalAnimatedObject_cl::LayerTwoAnimations()
{
  // Layer two animations (using a LayerMixer node). We use this to fade-in an upper body animation 
  // on top of a full body animation. The mixer gets the two animation controls as input and generates
  // the layered result. The animation tree looks as follows:
  //
  // - FinalSkeletalResult
  //    - LayerMixerNode
  //       - SkeletalAnimControl (WalkDagger Animation; influences full body)
  //       - SkeletalAnimControl (RunDagger Animation: influences upper body only)
  //
  // The weight of the layered upper body animation is set on the mixer instance.
  //

  // Create a new AnimConfig instance
  VDynamicMesh *pMesh = GetMesh();
  VisSkeleton_cl *pSkeleton = pMesh->GetSkeleton();
  VisAnimFinalSkeletalResult_cl* pFinalSkeletalResult;
  VisAnimConfig_cl* pConfig = VisAnimConfig_cl::CreateSkeletalConfig(pMesh, &pFinalSkeletalResult);

  // get skeletal animation sequence
  VisSkeletalAnimSequence_cl* pAnimSequenceWalkDagger = (VisSkeletalAnimSequence_cl*)pMesh->GetSequence("Walk_Dagger", VIS_MODELANIM_SKELETAL);
  VisSkeletalAnimSequence_cl* pAnimSequenceDrawDagger = (VisSkeletalAnimSequence_cl*)pMesh->GetSequence("Draw_Dagger", VIS_MODELANIM_SKELETAL);
  if(!pAnimSequenceWalkDagger || !pAnimSequenceDrawDagger)
    return;

  // Create the two animation controls: WalkDagger: full body animation; DrawDagger: upper body animation.
  // Use a helper function to create the animation controls.
  VSmartPtr<VisSkeletalAnimControl_cl> spWalkDaggerAnimControl = VisSkeletalAnimControl_cl::Create(pMesh->GetSkeleton(), pAnimSequenceWalkDagger, VANIMCTRL_LOOP|VSKELANIMCTRL_DEFAULTS, 1.0f, true);
  VSmartPtr<VisSkeletalAnimControl_cl> spDrawDaggerAnimControl = VisSkeletalAnimControl_cl::Create(pMesh->GetSkeleton(), pAnimSequenceDrawDagger, VSKELANIMCTRL_DEFAULTS, 1.0f, true);

  // create the layer node which layers the two animations
  m_spLayerMixerNode = new VisAnimLayerMixerNode_cl(pMesh->GetSkeleton());
  m_spLayerMixerNode->AddMixerInput(spWalkDaggerAnimControl, 1.0f);
  int iMixerInputDrawDagger = m_spLayerMixerNode->AddMixerInput(spDrawDaggerAnimControl, 0.0f);
  
  // set a per bone weighting list for the DrawDagger (upper body) slot in the mixer. It shall overlay the
  // upper body of the character and thus only influence the upper body bones.
  int iBoneCount = pSkeleton->GetBoneCount();
  VASSERT(iBoneCount<256);
  float fPerBoneWeightingList[256];
  memset(fPerBoneWeightingList, 0, sizeof(float)*iBoneCount);
  pSkeleton->SetBoneWeightRecursive(1.f, pSkeleton->GetBoneIndexByName("skeleton1:Spine"), fPerBoneWeightingList);
  m_spLayerMixerNode->ApplyPerBoneWeightingMask(iMixerInputDrawDagger, iBoneCount, fPerBoneWeightingList);

  // finally set the mixer as the root animation node
  pFinalSkeletalResult->SetSkeletalAnimInput(m_spLayerMixerNode);
  SetAnimConfig(pConfig);

  // fade-in the upper body animation
  //m_spLayerMixerNode->EaseIn(iMixerInputDrawDagger, 0.4f, true);
}


//
//                              *** MODE: FORWARD KINEMATICS ***
//

void SimpleSkeletalAnimatedObject_cl::ForwardKinematics()
{
  // Apply forward kinematics to the head "Neck" bone, using a bone modifier node. The bone modifier
  // gets a skeletal animation as input, modifies the translation of the neck bone and generates the
  // combined result. The animation tree looks as follows:
  //
  // - FinalSkeletalResult
  //    - BoneModifierNode (modifies the neck bone)
  //      - SkeletalAnimControl (Walk Animation)
  //
  // The translation of the neck bone is set on the bone modifier node.
  //

  // create a new AnimConfig instance
  VDynamicMesh *pMesh = GetMesh();
  VisAnimFinalSkeletalResult_cl* pFinalSkeletalResult;
  VisAnimConfig_cl* pConfig = VisAnimConfig_cl::CreateSkeletalConfig(pMesh, &pFinalSkeletalResult);

  // get skeletal animation sequence
  VisSkeletalAnimSequence_cl* pAnimSequenceWalk = (VisSkeletalAnimSequence_cl*)pMesh->GetSequence("Walk", VIS_MODELANIM_SKELETAL);
  if(!pAnimSequenceWalk)
    return;

  // create the animation control to play the walk animation (via a helper function)
  VSmartPtr<VisSkeletalAnimControl_cl> spWalkAnimControl = VisSkeletalAnimControl_cl::Create(pMesh->GetSkeleton(), pAnimSequenceWalk, VANIMCTRL_LOOP|VSKELANIMCTRL_DEFAULTS, 1.0f, true);

  // create the bone modifier node that translates the head bone. Set the animation control instance
  // as the input for this node.
  m_spBoneModifierNode = new VisAnimBoneModifierNode_cl(pMesh->GetSkeleton());
  m_spBoneModifierNode->SetModifierInput(spWalkAnimControl);
  hkvQuat customBoneRotation;

  // set the neck bone translation on the bone modifier node
  m_iNeckBoneIndex = pMesh->GetSkeleton()->GetBoneIndexByName("skeleton1:Neck");
  customBoneRotation.setFromEulerAngles (0, -45, 0);
  m_spBoneModifierNode->SetCustomBoneRotation(m_iNeckBoneIndex, customBoneRotation, VIS_MODIFY_BONE);

  // finally set the bone modifier as the root animation node
  pFinalSkeletalResult->SetSkeletalAnimInput(m_spBoneModifierNode);
  SetAnimConfig(pConfig);

  // The bone modifier node is now part of the animation tree. You can at any time update the translation
  // on the bone modifier. The animation system will take care of generating the proper final result.
}



void SimpleSkeletalAnimatedObject_cl::LookAt()
{
  // start a single animation
  StartSingleAnimation(true);

  // start a simple skeletal animation
  bool bLooped = true;
  int iFlags = VSKELANIMCTRL_DEFAULTS;
  if (bLooped)
    iFlags |= VANIMCTRL_LOOP;
  m_spSingleAnimControl = VisAnimConfig_cl::StartSkeletalAnimation(this, "Run", iFlags);


  // find the head bone index
  VDynamicMesh *pMesh = GetMesh();
  m_iHeadBoneIndex = pMesh->GetSkeleton()->GetBoneIndexByName("skeleton1:Head");

  // create a rotation matrix that defines the actual orientation of the head. This is model specific and can
  // only be found out by trial and error (typically 0, 90 or 180 deg). In our case the head is rotated by 180 degrees; Without this additional
  // orientation that model would follow the target with the backside of the head.
  m_RelativeHeadOrientation.setFromEulerAngles (90, 0, 270);

  // create an entity that rotates around the model
  pLookAtTarget = Vision::Game.CreateEntity("VisBaseEntity_cl",hkvVec3(0,0,0),"Models\\MagicBall.model");
  fLookAtRotationPhase = 0.f;
}



V_IMPLEMENT_SERIAL( SimpleSkeletalAnimatedObject_cl, VisBaseEntity_cl, 0, Vision::GetEngineModule() );
START_VAR_TABLE(SimpleSkeletalAnimatedObject_cl, VisBaseEntity_cl, "SimpleSkeletalAnimatedObject_cl", 0, "Models\\Warrior\\Warrior.model")
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
