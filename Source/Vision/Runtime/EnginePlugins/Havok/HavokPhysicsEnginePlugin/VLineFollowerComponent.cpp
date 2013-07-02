/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/VLineFollowerComponent.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiVertexAnimDeformer.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiPath.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/PathCameraEntity.hpp>
#include <Vision/Runtime/Base/ThirdParty/tinyXML/TinyXMLHelper.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


///-------------------------------------------------------------------------///
/// Constructor & Destructor                                                ///
///-------------------------------------------------------------------------///

VLineFollowerComponent::VLineFollowerComponent()
{
  m_bPlayingAnim = false;
  m_pFollowPath = NULL;
  m_pPhys = NULL;

  // Register callbacks
  Vision::Callbacks.OnAfterSceneLoaded += this;
  Vision::Callbacks.OnUpdateSceneFinished += this;

  PathCameraAction::OnTriggerEvent += this;

  Debug_RenderMesh = false;
}

VLineFollowerComponent::~VLineFollowerComponent()
{
  // Deregister callbacks
  Vision::Callbacks.OnAfterSceneLoaded -= this;
  Vision::Callbacks.OnUpdateSceneFinished -= this;

  PathCameraAction::OnTriggerEvent -= this;
}

void VLineFollowerComponent::CommonInit()
{
  StartAnimation(Model_AnimationName);

  if (m_pPhys)
    m_pPhys->SetDebugRendering(Debug_RenderMesh);

  m_mDeltaRotation.setRotationMatrix (hkvVec3(0,0,1), Model_DeltaRotation);
}

void VLineFollowerComponent::CommonDeInit()
{
  m_bPlayingAnim = false;
  m_pFollowPath = NULL;
  
  if (GetOwner() != NULL && m_pPhys != NULL)
    GetOwner()->RemoveComponent(m_pPhys);
  m_pPhys = NULL;
}

///-------------------------------------------------------------------------///
/// IVObjectComponent Overrides                                             ///
///-------------------------------------------------------------------------///

void VLineFollowerComponent::SetOwner(VisTypedEngineObject_cl *pOwner)
{
  IVObjectComponent::SetOwner(pOwner);

  // Insert code here to respond to attaching this component to an object
  // This function is called with pOwner==NULL when de-attaching.
  if (pOwner!=NULL)
    CommonInit();
  else
    CommonDeInit();
}

BOOL VLineFollowerComponent::CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
  if (!IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut))
    return FALSE;

  // Define criteria here that allows the editor to attach this component to the passed object.
  // In our example, the object should be derived from VisObject3D_cl to be positionable.
  BOOL bIsValidClass = pObject->IsOfType(V_RUNTIME_CLASS(VisBaseEntity_cl));
  if (!bIsValidClass)
    sErrorMsgOut += "Component can only be added to instances of VisBaseEntity_cl or derived classes.";

  if (!bIsValidClass)
    return FALSE;

  return TRUE;
}

void VLineFollowerComponent::OnVariableValueChanged(VisVariable_cl *pVar, const char * value)
{
  
}

void VLineFollowerComponent::MessageFunction( int iID, INT_PTR iParamA, INT_PTR iParamB )
{
  if ( iID == VIS_MSG_EDITOR_PROPERTYCHANGED )
  {
    CommonInit(); 
    return;
  }
}

///-------------------------------------------------------------------------///
/// IVisCallbackHandler_cl Overrides                                        ///
///-------------------------------------------------------------------------///

void VLineFollowerComponent::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  VisBaseEntity_cl* pOwner = (VisBaseEntity_cl *)GetOwner();
  if (!pOwner)
    return;

  // Update callback
  if (pData->m_pSender == &Vision::Callbacks.OnUpdateSceneFinished) 
  {
    // Update
    PerFrameUpdate();

    // Debug Render
    if (!Vision::Editor.IsAnimatingOrPlaying())
    {
      if (Debug_RenderMesh)
      {
        hkvVec3 pos = pOwner->GetPosition();
        hkvMat3 rotMat;

        // Setup all coordinates of a single spline
        hkvVec3 p1(0,0,-Model_CapsuleHeight*0.5f-Model_CapsuleRadius);
        hkvVec3 p2(Model_CapsuleRadius*0.7f,0,-Model_CapsuleHeight*0.5f-Model_CapsuleRadius*0.7f);
        hkvVec3 p3(Model_CapsuleRadius,0,-Model_CapsuleHeight*0.5f);
        hkvVec3 p4(Model_CapsuleRadius,0,Model_CapsuleHeight*0.5f);
        hkvVec3 p5(Model_CapsuleRadius*0.7f,0,Model_CapsuleHeight*0.5f+Model_CapsuleRadius*0.7f);
        hkvVec3 p6(0,0,Model_CapsuleHeight*0.5f+Model_CapsuleRadius);

        rotMat.setRotationMatrix (hkvVec3(0,0,1), 60.f);
        pos.z -= Model_GroundOffset;

        // Draw and rotate the spline (6 times)
        int i;
        for(i=0;i<6;i++)
        {
          Vision::Game.DrawSingleLine(pos-p1, pos-p2,V_RGBA_RED);
          Vision::Game.DrawSingleLine(pos-p2, pos-p3,V_RGBA_RED);
          Vision::Game.DrawSingleLine(pos-p3, pos-p4,V_RGBA_RED);
          Vision::Game.DrawSingleLine(pos-p4, pos-p5,V_RGBA_RED);
          Vision::Game.DrawSingleLine(pos-p5, pos-p6,V_RGBA_RED);

          p1 = rotMat.transformDirection(p1);
          p2 = rotMat.transformDirection(p2);
          p3 = rotMat.transformDirection(p3);
          p4 = rotMat.transformDirection(p4);
          p5 = rotMat.transformDirection(p5);
          p6 = rotMat.transformDirection(p6);
        }  
      }
    }
  }

  // Initialization
  if (pData->m_pSender == &Vision::Callbacks.OnAfterSceneLoaded)
  {
    // Find the path to follow
    if (m_pFollowPath==NULL && Path_Key[0])
    {
      m_pFollowPath = Vision::Game.SearchPath(Path_Key);
      InitPhysics(hkvMath::mod (Path_InitialOffset, 1.f));
    }
    return;
  }

  // Event handling
  if (pData->m_pSender==&PathCameraAction::OnTriggerEvent)
  {
    TiXmlElement *pEventNode = ((VPathEventCallbackDataObject *)pData)->m_pEventNode;
    const char *szEntityKey = pEventNode->Attribute(ENTITY_PROPERTY);
    if (szEntityKey && pOwner->GetEntityKey() && !_stricmp(szEntityKey, pOwner->GetEntityKey()))
    {
      const char *szAction = pEventNode->Attribute(ACTION_PROPERTY);

      if(szAction && !_stricmp(szAction,RESET_ACTION))
      {
        InitPhysics(hkvMath::mod (Path_InitialOffset, 1.f));
      } 
      else if(szAction && !_stricmp(szAction,SET_ACTION))
      {
          const char *szPos = pEventNode->Attribute(TO_PROPERTY);
          float fPos = Path_InitialOffset;
          if(szPos) fPos = hkvMath::mod ((float)atof(szPos),1.f);
          InitPhysics(fPos);
      }      
    }
    return;
  }
}

///-------------------------------------------------------------------------///
/// VTypedObject Override                                                   ///
///-------------------------------------------------------------------------///

void VLineFollowerComponent::OnDeserializationCallback(const VSerializationContext &context)
{
  if (m_pFollowPath==NULL && Path_Key[0])
  {
    m_pFollowPath = Vision::Game.SearchPath(Path_Key);
    InitPhysics(hkvMath::mod (Path_InitialOffset, 1.f));
  }
  CommonInit();
}

void VLineFollowerComponent::PerFrameUpdate()
{
  VisBaseEntity_cl* pOwner = (VisBaseEntity_cl *)GetOwner();
  if (!pOwner || !m_pFollowPath)
    return;
    
  hkvVec3 vPos;
  hkvVec3 vDir;
  m_pFollowPath->EvalPoint(m_fCurrentPathPos, vPos, &vDir);
  vPos.z = pOwner->GetPosition().z;
  float fDiff  = Vision::GetTimer()->GetTimeDifference();

  if((vPos - pOwner->GetPosition()).getLength() < Path_TriggerDistance)
  {
    if(fDiff>0.1) 
      m_fCurrentPathPos += (3/Path_NumberSteps);
    else
      m_fCurrentPathPos += (1/Path_NumberSteps);

    if (m_fCurrentPathPos>1.f)
    {
      if (m_pFollowPath->IsClosed())
        m_fCurrentPathPos = hkvMath::mod (m_fCurrentPathPos,1.f);
      else
        m_fCurrentPathPos = 1.f;
    }

    m_pFollowPath->EvalPoint(m_fCurrentPathPos, vPos, &vDir);
    vPos.z = pOwner->GetPosition().z;
    if(Vision::Editor.IsAnimatingOrPlaying() && Debug_DisplayBoxes) 
      Vision::Game.DrawCube(vPos,80.f);
  } 
  else
  {
    if(Vision::Editor.IsAnimatingOrPlaying() && Debug_DisplayBoxes) 
      Vision::Game.DrawCube(vPos,80.f,V_RGBA_YELLOW);
  }

  // Orient the entity in the right direction
  hkvMat3 mRot(hkvNoInitialization);
  mRot.setLookInDirectionMatrix (vPos - pOwner->GetPosition());

  mRot = mRot.multiply (m_mDeltaRotation);
  pOwner->SetRotationMatrix(mRot);
  
  // The animation we are playing should have motion delta which will 
  // move towards the target position. No animation, well lets add at
  // least some motion delta
  if (!m_bPlayingAnim) 
  {
    pOwner->SetMotionDeltaLocalSpace( hkvVec3(1.f,0.f,0.f)*(Model_CapsuleRadius*fDiff) );
  }
}


///-------------------------------------------------------------------------///
/// Helper                                                                  ///
///-------------------------------------------------------------------------///

void VLineFollowerComponent::InitPhysics(float fPathPos)
{
  VisBaseEntity_cl* pOwner = (VisBaseEntity_cl *)GetOwner();
  if (!pOwner)
    return;

  hkvVec3 vPos;
  hkvVec3 vDir;
  hkvAlignedBBox bbox;

  m_fCurrentPathPos = fPathPos;
  if(m_pFollowPath)
    m_pFollowPath->EvalPoint(fPathPos, vPos, &vDir);
  else
    vPos = pOwner->GetPosition();

  // to determine correct height on the ground, perform a ray-cast:
  if (Vision::GetApplication()->GetPhysicsModule()!=NULL && Model_CapsuleHeight>0)
  {
    hkvVec3 vRayStart(vPos.x,vPos.y,vPos.z+Model_CapsuleHeight);
    hkvVec3 vRayEnd(vPos.x,vPos.y,vPos.z-Model_CapsuleHeight);
    VisPhysicsHit_t hitPoint;
    if (Vision::GetApplication()->GetPhysicsModule()->Raycast(vRayStart, vRayEnd, hitPoint)) // hit?
    {
      vPos.z = hitPoint.vImpactPoint.z + 5.f*Vision::World.GetGlobalUnitScaling(); // add some margin
    }
  }

  VDynamicMesh *pMesh = pOwner->GetMesh();
  if (pMesh)
  {
    pMesh->GetCollisionBoundingBox(bbox);
    vPos.z -= bbox.m_vMin.z;

    // Use model size if not set
    if (Model_CapsuleRadius<=0.f) Model_CapsuleRadius = hkvMath::Min( bbox.getSizeX(), bbox.getSizeY() )/2.f;
    if (Model_CapsuleHeight<=0.f) Model_CapsuleHeight = bbox.getSizeZ();
  } else
  { 
    // No model - set some sane values if not set
    if (Model_CapsuleRadius<=0.f) Model_CapsuleRadius = 40.f;
    if (Model_CapsuleHeight<=0.f) Model_CapsuleHeight = 90.f;
  }

  // Create the physics object 
  pOwner->SetPosition(vPos);
  if (!m_pPhys)
  {
    m_pPhys = new vHavokCharacterController();
    m_pPhys->Capsule_Radius = Model_CapsuleRadius;
    float h = Model_CapsuleHeight * 0.5f;
    float fPivot = Model_GroundOffset;
    m_pPhys->Character_Top.set(0, 0, h - fPivot);
    m_pPhys->Character_Bottom.set(0, 0, -h - fPivot);
    pOwner->AddComponent(m_pPhys);
  }

  // Update position
  m_pPhys->SetPosition(vPos);

  // Enable debug rendering
  m_pPhys->SetDebugRendering(Debug_RenderMesh);
  m_pPhys->SetDebugColor(V_RGBA_RED);
}


BOOL VLineFollowerComponent::StartAnimation(const char *szAnimName)
{
  VisBaseEntity_cl* pOwner = (VisBaseEntity_cl *)GetOwner();
  if (!pOwner)
    return false;

  m_bPlayingAnim = false;

  // Check for animation sequences
  VDynamicMesh *pMesh = pOwner->GetMesh();
  if (!pMesh || !pMesh->GetSequenceSetCollection() || !pMesh->GetSequenceSetCollection()->GetSequenceSetCount())
    return false;

  VisAnimFinalSkeletalResult_cl* pFinalSkeletalResult = NULL;
  VisVertexAnimDeformer_cl* pVertexAnimDeformer = NULL;

  // Get the sequence(s) for vertex and skeletal animation
  VisSkeletalAnimSequence_cl* pAnimSequenceSkeletal = (VisSkeletalAnimSequence_cl*)pMesh->GetSequence(szAnimName, VIS_MODELANIM_SKELETAL);
  VisVertexAnimSequence_cl* pAnimSequenceVertex = (VisVertexAnimSequence_cl*)pMesh->GetSequence(szAnimName, VIS_MODELANIM_VERTEX);

  // If no sequence with the given name is present
  if ((!pAnimSequenceSkeletal) && (!pAnimSequenceVertex))
  {
    VisAnimSequenceSet_cl * pSequenceSet =  pMesh->GetSequenceSetCollection()->GetSequenceSet(0);
    // Find the first skeletal or vertex animation and use it
    for (int i=0; i< pSequenceSet->GetSequenceCount(); ++i)
    {
      VisAnimSequence_cl* pTempAnimSequence = pSequenceSet->GetSequence(i);
      if (pTempAnimSequence->GetType() == VIS_MODELANIM_SKELETAL)
      {
        // If it is a skeletal animation, create a config for it
        VisAnimConfig_cl* pConfig = VisAnimConfig_cl::CreateSkeletalConfig(pMesh, &pFinalSkeletalResult);
        VisSkeletalAnimControl_cl* pSkeletalAnimControl = VisSkeletalAnimControl_cl::Create(pMesh->GetSkeleton(), (VisSkeletalAnimSequence_cl*)pTempAnimSequence, VANIMCTRL_LOOP|VSKELANIMCTRL_DEFAULTS, 1.0f, true);
        pFinalSkeletalResult->SetSkeletalAnimInput(pSkeletalAnimControl);
        pOwner->SetAnimConfig(pConfig);
        m_bPlayingAnim = true;
        return true;
      }
      else if (pTempAnimSequence->GetType() == VIS_MODELANIM_VERTEX)
      {
        // If it is a vertex animation, create a config for it
        VisAnimConfig_cl* pConfig = VisAnimConfig_cl::CreateVertexConfig(pMesh, &pVertexAnimDeformer);
        VisVertexAnimControl_cl* pVertexAnimControl = VisVertexAnimControl_cl::Create((VisVertexAnimSequence_cl*)pTempAnimSequence, VANIMCTRL_LOOP|VSKELANIMCTRL_DEFAULTS, 1.0f, true);
        pVertexAnimDeformer->AddVertexAnimControl(pVertexAnimControl, 1.0f);
        pOwner->SetAnimConfig(pConfig);
        m_bPlayingAnim = true;
        return true;
      }
    }
    // If neither a skeletal nor a vertex animation has been found, report failure
    return false;
  }

  // If both a vertex and a skeletal animation with the given name has been found
  // create a combined config for skeletal and vertex animation.
  VisAnimConfig_cl* pConfig = NULL;
  if ((pAnimSequenceSkeletal) && (pAnimSequenceVertex))
    pConfig = VisAnimConfig_cl::CreateSkeletalVertexConfig(pMesh, &pFinalSkeletalResult, &pVertexAnimDeformer);

  // If it is just a skeletal animation, create a config for it
  if (pAnimSequenceSkeletal)
  {    
    if (!pConfig)
      pConfig = VisAnimConfig_cl::CreateSkeletalConfig(pMesh, &pFinalSkeletalResult);

    // If a skeletal animation has been found create a control for it
    VisSkeletalAnimControl_cl* pSkeletalAnimControl = VisSkeletalAnimControl_cl::Create(pMesh->GetSkeleton(), pAnimSequenceSkeletal, VANIMCTRL_LOOP|VSKELANIMCTRL_DEFAULTS, 1.0f, true);
    // And set it as the input for the final skeletal result
    pFinalSkeletalResult->SetSkeletalAnimInput(pSkeletalAnimControl);
  }

  // If it is just a vertex animation, create a config for it
  if (pAnimSequenceVertex)
  {
    if (!pConfig)
      pConfig = VisAnimConfig_cl::CreateVertexConfig(pMesh, &pVertexAnimDeformer);

    // If a vertex animation has been found create a control for it
    VisVertexAnimControl_cl* pVertexAnimControl = VisVertexAnimControl_cl::Create(pAnimSequenceVertex, VANIMCTRL_LOOP|VSKELANIMCTRL_DEFAULTS, 1.0f, true);
    // And set add it to the vertex anim deformer
    pVertexAnimDeformer->AddVertexAnimControl(pVertexAnimControl, 1.0f);
  }

  // Set the current config
  pOwner->SetAnimConfig(pConfig);

  // Make sure we get the motion delta from the animation
  pOwner->GetAnimConfig()->SetFlags(pOwner->GetAnimConfig()->GetFlags() | APPLY_MOTION_DELTA | MULTITHREADED_ANIMATION);

  // And report success
  m_bPlayingAnim = true;
  return true;
}



///-------------------------------------------------------------------------///
/// Serialization                                                           ///
///-------------------------------------------------------------------------///

extern VModule g_vHavokModule;
V_IMPLEMENT_SERIAL( VLineFollowerComponent, IVObjectComponent, 0, &g_vHavokModule );

void VLineFollowerComponent::Serialize(VArchive &ar)
{
  char iLocalVersion = 1;

  IVObjectComponent::Serialize(ar);

  if (ar.IsLoading())
  {
    ar >> iLocalVersion;
    VASSERT_MSG(iLocalVersion==1, "Invalid local version. Please re-export");

    ar.ReadStringBinary(Model_AnimationName, sizeof(Model_AnimationName));
    ar.ReadStringBinary(Path_Key, sizeof(Path_Key));
    ar >> Path_NumberSteps;
    ar >> Path_TriggerDistance;
    ar >> Path_InitialOffset;
    ar >> Model_DeltaRotation;
    ar >> Model_GroundOffset;
    ar >> Model_CapsuleHeight;
    ar >> Model_CapsuleRadius;
    ar >> Debug_DisplayBoxes;
    
    Debug_RenderMesh = false;
    CommonInit();
  }
  else
  {
    ar << iLocalVersion;
    ar.WriteStringBinary(Model_AnimationName);
    ar.WriteStringBinary(Path_Key);
    ar << Path_NumberSteps;
    ar << Path_TriggerDistance;
    ar << Path_InitialOffset;
    ar << Model_DeltaRotation;
    ar << Model_GroundOffset;
    ar << Model_CapsuleHeight;
    ar << Model_CapsuleRadius;
    ar << Debug_DisplayBoxes;
  }
}

START_VAR_TABLE(VLineFollowerComponent, IVObjectComponent,"Line Follower Component", VVARIABLELIST_FLAGS_NONE, "Line Follower" )
  DEFINE_VAR_STRING (VLineFollowerComponent, Path_Key, "Key of the path to follow", "", 128, 0, NULL);
  DEFINE_VAR_FLOAT  (VLineFollowerComponent, Path_NumberSteps, "Approximate Path with a given number of steps", "50.0", 0, NULL);
  DEFINE_VAR_FLOAT  (VLineFollowerComponent, Path_TriggerDistance, "Distance to trigger new destination calculation", "40.0", 0, NULL);
  DEFINE_VAR_FLOAT  (VLineFollowerComponent, Path_InitialOffset, "Initial offset position inside the path (0: start; 1: end;)", "0.0", 0, "Slider(0,1)");
  DEFINE_VAR_STRING (VLineFollowerComponent, Model_AnimationName, "Name of the animation sequence to play", "", 128, 0, "dropdownlist(Animation)");
  DEFINE_VAR_FLOAT  (VLineFollowerComponent, Model_DeltaRotation, "Rotation of the offset delta around the Z axis", "0.0", 0, NULL);
  DEFINE_VAR_FLOAT  (VLineFollowerComponent, Model_GroundOffset, "Physics Alignment Offset of the Model on the Z axis", "5", 0, NULL);
  DEFINE_VAR_FLOAT  (VLineFollowerComponent, Model_CapsuleHeight, "Height of capsule controller shape", "90", 0, NULL);
  DEFINE_VAR_FLOAT  (VLineFollowerComponent, Model_CapsuleRadius, "Radius of capsule controller shape", "40", 0, NULL);
  DEFINE_VAR_BOOL   (VLineFollowerComponent, Debug_DisplayBoxes, "Display target boxes during animation", "FALSE", 0, NULL);
  DEFINE_VAR_BOOL   (VLineFollowerComponent, Debug_RenderMesh, "Display visible capsule mesh", "FALSE", 0, NULL);
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
