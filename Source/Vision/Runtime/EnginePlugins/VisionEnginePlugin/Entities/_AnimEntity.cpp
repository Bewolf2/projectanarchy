/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


// Animated Entity Class: Generic entity class with support for playing
// a looped animation. The animation name is passed as a string.


#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/_AnimEntity.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiVertexAnimDeformer.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiMorphingDeformer.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiPath.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

// AnimEntity_cl is deprecated, but we don't need deprecation warnings for the method implementations.
#pragma  warning(disable: 4996)

#define ANIMENTITY_VERSION_2        2     // Follow Path
#define ANIMENTITY_VERSION_3        3     // Skinning Mode option.
#define ANIMENTITY_VERSION_CURRENT  3     // Current version

extern VModule g_VisionEngineModule;

V_IMPLEMENT_SERIAL( AnimEntity_cl, VisBaseEntity_cl, 0, &g_VisionEngineModule );


AnimEntity_cl::AnimEntity_cl()
{
  Vision::Callbacks.OnAfterSceneLoaded += this;

  PathTime = 0.f;
  m_pFollowPath = NULL;
  m_fPathPos = 0.f;
  SkinningMode = SKINNINGMODE_DEFAULT;
}

AnimEntity_cl::~AnimEntity_cl()
{
  Vision::Callbacks.OnAfterSceneLoaded -= this;
}


bool AnimEntity_cl::StartAnimation(const char *szAnimName)
{
  bool result = VSimpleAnimationComponent::StartAnimation(this,szAnimName);
  if (result)
  {
    VisAnimConfig_cl* pConfig = GetAnimConfig();
    if(pConfig)
    {
      switch(SkinningMode)
      {
      case SKINNINGMODE_SOFTWARE:
        pConfig->SetSkinningMode(VIS_SKINNINGMODE_SOFTWARE);
        break;
      case SKINNINGMODE_HARDWARE:
        pConfig->SetSkinningMode(VIS_SKINNINGMODE_HARDWARE);
        break;
      }
    }
  }

  return result;
}

// *** The InitFunction will be called by the engine after the entity has been created
void AnimEntity_cl::InitFunction()
{
  StartAnimation(AnimationName);
}


void AnimEntity_cl::MessageFunction( int iID, INT_PTR iParamA, INT_PTR iParamB )
{
#ifdef WIN32
  if ( iID == VIS_MSG_EDITOR_PROPERTYCHANGED )
  {
    const char *szPropertyName = (const char *) iParamA;
    if ( _stricmp(szPropertyName, "ModelFile") == 0 )
      SkinningMode = SKINNINGMODE_DEFAULT;

    InitFunction(); // the editor changed parameter
    return;
  }
  else if (iID == VIS_MSG_EDITOR_GETLINKS)
  {
    // fill out the links that we want to provide
    VShapeLinkConfig *pConfig = (VShapeLinkConfig *)iParamA;
    pConfig->AllocateLinks(1);
    VShapeLinkInfo &info(pConfig->GetLinkInfo(0));
    info.m_eType = VShapeLinkInfo::LINK_SOURCE;
    info.m_sDisplayName = "attach to path";
    info.m_sUniqueStringID = "attach_to_path";
  }
  else if (iID == VIS_MSG_EDITOR_CANLINK)
  {
    // we want to link with paths (only if we are not yet connected)
    VShapeLink *pLink = (VShapeLink *)iParamA;
    pLink->m_bResult = 
         (m_pFollowPath==NULL || m_pFollowPath==pLink->m_pOtherObject) 
      && (pLink->m_pOtherObject!=NULL) 
      && (pLink->m_pOtherObject->IsOfType(V_RUNTIME_CLASS(VisPath_cl)));
  }
  else if (iID == VIS_MSG_EDITOR_ONLINK )
  {
    VShapeLink *pLink = (VShapeLink *)iParamA;
    VASSERT(pLink->m_pOtherObject && pLink->m_pOtherObject->IsOfType(V_RUNTIME_CLASS(VisPath_cl)));
    VisPath_cl *pPath = (VisPath_cl *)pLink->m_pOtherObject;
    m_pFollowPath = pPath; // set our path
  }
  else if (iID == VIS_MSG_EDITOR_ONUNLINK)
  {
    VShapeLink *pLink = (VShapeLink *)iParamA;
    VASSERT(pLink->m_pOtherObject && pLink->m_pOtherObject->IsOfType(V_RUNTIME_CLASS(VisPath_cl)));
    VisPath_cl *pPath = (VisPath_cl *)pLink->m_pOtherObject;
    if (m_pFollowPath==pPath)
      m_pFollowPath = NULL;
  }
#endif

  VisBaseEntity_cl::MessageFunction( iID, iParamA, iParamB );
}

void AnimEntity_cl::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  VASSERT(pData->m_pSender == &Vision::Callbacks.OnAfterSceneLoaded);

  // find the path to follow
  if (PathKey[0])
    m_pFollowPath = Vision::Game.SearchPath(PathKey);
}


void AnimEntity_cl::ThinkFunction()
{
  if (m_pFollowPath && !IsObjectFlagSet(VObjectFlag_IsNetworkReplica))
  {
    m_fPathPos += Vision::GetTimer()->GetTimeDifference()/PathTime;
    if (m_fPathPos>1.f)
    {
      if (m_pFollowPath->IsClosed())
        m_fPathPos = hkvMath::mod (m_fPathPos,1.f);
      else
        m_fPathPos = 1.f;
    }
  
    hkvVec3 vPos;
    hkvVec3 vDir;
    m_pFollowPath->EvalPointSmooth(m_fPathPos, vPos, &vDir);
    hkvMat3 mRot(hkvNoInitialization);
    mRot.setLookInDirectionMatrix (vDir);
    SetPosition(vPos);
    SetRotationMatrix(mRot);
  }
}

void AnimEntity_cl::Serialize( VArchive &ar )
{
  VisBaseEntity_cl::Serialize(ar);
  if (ar.IsLoading())
  {
    char iVersion;
    ar >> iVersion; VVERIFY (iVersion<=ANIMENTITY_VERSION_CURRENT && "Invalid entity version number. Please re-export scene");
    ar.ReadStringBinary(AnimationName, sizeof(AnimationName));
    ar.ReadStringBinary(PathKey, sizeof(PathKey));
    ar >> PathTime;
    if (iVersion>=ANIMENTITY_VERSION_2)
      ar >> m_pFollowPath;
    if (iVersion>=ANIMENTITY_VERSION_3)
      ar >> SkinningMode;

    InitFunction();
  } 
  else
  {
    ar << (char )ANIMENTITY_VERSION_CURRENT; // version
    ar.WriteStringBinary(AnimationName);
    ar.WriteStringBinary(PathKey);
    ar << PathTime;
    ar << m_pFollowPath; // ANIMENTITY_VERSION_2
    ar << SkinningMode;  // ANIMENTITY_VERSION_3

  }
}


// -------------------------------------------------------------------------- //
// Variable Attributes                                                        //
// -------------------------------------------------------------------------- //

#ifdef WIN32

void AnimEntity_cl::GetVariableAttributes(VisVariable_cl *pVariable, VVariableAttributeInfo &destInfo)
{
  // Check whether we need to display the radius property
  if (!strcmp(pVariable->GetName(),"SkinningMode"))
  {
#ifdef _VR_DX9
    if(GetMesh() && GetMesh()->GetSkeleton() && GetMesh()->GetSkeleton()->GetBoneCount() <= HARDWARESKINNING_MAX_BONECOUNT_PC_DX9)
      destInfo.m_bHidden = false;
    else
      destInfo.m_bHidden = true;
#else
    destInfo.m_bHidden = true;
#endif
  }
}

#endif

// *** The variable table is used by the plugin system of the engine and editor. In that case we do not have
// *** variables which have to be initialised or which have to be available in the editor.
START_VAR_TABLE(AnimEntity_cl,VisBaseEntity_cl,"animated entity", VFORGE_HIDECLASS, NULL )
  DEFINE_VAR_STRING(AnimEntity_cl, AnimationName, "Animation name to start", "", 128, 0, "dropdownlist(Animation)");
  DEFINE_VAR_STRING(AnimEntity_cl, PathKey, "Path key to follow", "", 128, 0, 0);
  DEFINE_VAR_FLOAT(AnimEntity_cl, PathTime, "Time for one path cycle", "10.0", 0, 0);
  DEFINE_VAR_ENUM(AnimEntity_cl, SkinningMode, "Skinning Mode(test purpose for DX9 Hardware skinning)", "SKINNINGMODE_DEFAULT","SKINNINGMODE_DEFAULT,SKINNINGMODE_SOFTWARE,SKINNINGMODE_HARDWARE", 0, 0);
END_VAR_TABLE


//////////////////////////////////////////////////////////////////////////////////////////////////////
// class StaticCollisionEntity_cl
//////////////////////////////////////////////////////////////////////////////////////////////////////

V_IMPLEMENT_SERIAL( StaticCollisionEntity_cl, VisBaseEntity_cl, 0, &g_VisionEngineModule );


void StaticCollisionEntity_cl::CommonInit()
{
  IVisPhysicsModule_cl *pPhysMod = Vision::GetApplication()->GetPhysicsModule();
  if (!pPhysMod)
  {
    Vision::Error.Warning("No physics module set.");
    return;
  }
  if (pPhysMod->GetType()!=IVisPhysicsModule_cl::VISION)
  {
    Vision::Error.Warning("Physics module must be of type Vision");
    return;
  }

  SetTraceAccuracy(VIS_TRACEACC_POLYGONS);
}


void StaticCollisionEntity_cl::Serialize( VArchive &ar )
{
  VisBaseEntity_cl::Serialize(ar);
  char iVersion, iCurrentVersion = 0;
  if (ar.IsLoading())
  {
    CommonInit();
    ar >> iVersion; VASSERT(iVersion<=iCurrentVersion);
  } 
  else
  {
    ar << iCurrentVersion;
  }
}



START_VAR_TABLE(StaticCollisionEntity_cl,VisBaseEntity_cl,"static entity that uses collision",0, NULL )
END_VAR_TABLE




//////////////////////////////////////////////////////////////////////////////////////////////////
// class VSimpleAnimationComponent
//////////////////////////////////////////////////////////////////////////////////////////////////


VSimpleAnimationComponent::VSimpleAnimationComponent(int iComponentFlags) : IVObjectComponent(0, iComponentFlags),
UseMotionDelta(FALSE)
{
}

VSimpleAnimationComponent::~VSimpleAnimationComponent()
{
}


  // IVObjectComponent override
void VSimpleAnimationComponent::SetOwner(VisTypedEngineObject_cl *pOwner)
{
  IVObjectComponent::SetOwner(pOwner);

  // Insert code here to respond to attaching this component to an object
  // This function is called with pOwner==NULL when de-attaching.
  if (pOwner != NULL)
    VSimpleAnimationComponentManager::GlobalManager().Instances().AddUnique(this);
  else
    VSimpleAnimationComponentManager::GlobalManager().Instances().SafeRemove(this);

  if (pOwner!=NULL && !AnimationName.IsEmpty())
  {
    VisBaseEntity_cl *pEntity = (VisBaseEntity_cl *)pOwner;
    StartAnimation(pEntity, AnimationName);
    
    if (pEntity->GetAnimConfig() != NULL)
    {
      pEntity->GetAnimConfig()->SetFlags(pEntity->GetAnimConfig()->GetFlags() | MULTITHREADED_ANIMATION);
    }
  }
}

BOOL VSimpleAnimationComponent::CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
  if (!IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut))
    return FALSE;

  if (!pObject->IsOfType(V_RUNTIME_CLASS(VisBaseEntity_cl)))
  {
    sErrorMsgOut = "Component can only be added to instances of VisBaseEntity_cl or derived classes.";
    return FALSE;
  }

  return TRUE;
}

void VSimpleAnimationComponent::OnVariableValueChanged(VisVariable_cl *pVar, const char * value)
{
  VisBaseEntity_cl *pEntity = (VisBaseEntity_cl *)GetOwner();
  if (pEntity != NULL && !AnimationName.IsEmpty())
  {
    StartAnimation(pEntity, AnimationName);

    if (pEntity->GetAnimConfig() != NULL)
    {
      pEntity->GetAnimConfig()->SetFlags(pEntity->GetAnimConfig()->GetFlags() | MULTITHREADED_ANIMATION);
    }
  }
}


bool VSimpleAnimationComponent::IsRelevantForSerialization() const
{
  return IVObjectComponent::IsRelevantForSerialization() && !AnimationName.IsEmpty();
}

void VSimpleAnimationComponent::PerFrameUpdate()
{
  if(GetOwner() == NULL || !UseMotionDelta)
    return;

  VisBaseEntity_cl *pOwnerEntity = (VisBaseEntity_cl *)GetOwner();

  //Physics component will take care of motion detla.
  if(pOwnerEntity->GetPhysicsObject() != NULL)
    return;

  // Rotation is not relevant for capsule so rotate just the entity
  if (pOwnerEntity->HasRotationDelta())
  {
    hkvVec3 vOwnerRot = pOwnerEntity->GetRotationDelta();  
    pOwnerEntity->IncOrientation(vOwnerRot);
    pOwnerEntity->ResetRotationDelta();
  }

  hkvVec3 vVel = hkvVec3::ZeroVector ();

  // Process delta
  hkvVec3 vMotionDelta = pOwnerEntity->GetMotionDeltaWorldSpace() + pOwnerEntity->GetRotationMatrix() * pOwnerEntity->GetMotionDeltaLocalSpace();
  
  pOwnerEntity->IncPosition(vMotionDelta);

  pOwnerEntity->ResetMotionDelta();
}


V_IMPLEMENT_SERIAL(VSimpleAnimationComponent,IVObjectComponent,0,&g_VisionEngineModule);
void VSimpleAnimationComponent::Serialize( VArchive &ar )
{
  IVObjectComponent::Serialize(ar);
  if (ar.IsLoading())
  {
    char iLocalVersion = 127; //invalid value
    ar >> iLocalVersion;
    VASSERT_MSG(iLocalVersion <= VERSION_CURRENT, "Invalid local version. Please re-export");
    ar >> AnimationName;
    if( iLocalVersion >= VERSION1)
      ar >> UseMotionDelta;
  } else
  {
    ar << static_cast<char>(VERSION_CURRENT);
    ar << AnimationName;
    ar << UseMotionDelta;
  }
}


bool VSimpleAnimationComponent::StartAnimation(VisBaseEntity_cl *pEntity, const char *szAnimName)
{
  if (pEntity==NULL)
    return false;
  VDynamicMesh *pMesh = pEntity->GetMesh();
  if (!pMesh || !pMesh->GetSequenceSetCollection() || !pMesh->GetSequenceSetCollection()->GetSequenceSetCount())
    return false;

  VisAnimFinalSkeletalResult_cl* pFinalSkeletalResult = NULL;
  VisVertexAnimDeformer_cl* pVertexAnimDeformer = NULL;
  VisMorphingDeformer_cl* pMorphingDeformer = NULL;

  // get the sequence(s) for vertex and skeletal animation
  VisSkeletalAnimSequence_cl* pAnimSequenceSkeletal = (VisSkeletalAnimSequence_cl*)pMesh->GetSequence(szAnimName, VIS_MODELANIM_SKELETAL);
  VisVertexAnimSequence_cl* pAnimSequenceVertex = (VisVertexAnimSequence_cl*)pMesh->GetSequence(szAnimName, VIS_MODELANIM_VERTEX);

  // if no sequence with the given name is present
  if ((!pAnimSequenceSkeletal) && (!pAnimSequenceVertex))
  {
    // find the first skeletal or vertex animation and use it
    for (int i=0; i<pMesh->GetSequenceSetCollection()->GetSequenceSet(0)->GetSequenceCount(); ++i)
    {
      VisAnimSequence_cl* pTempAnimSequence = pMesh->GetSequenceSetCollection()->GetSequenceSet(0)->GetSequence(i);
      if (pTempAnimSequence->GetType() == VIS_MODELANIM_SKELETAL)
      {
        // if it is a skeletal animation, create a config for it
        VisAnimConfig_cl* pConfig = VisAnimConfig_cl::CreateSkeletalConfig(pMesh, &pFinalSkeletalResult);
        if (pConfig == NULL)
        {
          continue;
        }
        // create its control
        VisSkeletalAnimControl_cl* pSkeletalAnimControl = VisSkeletalAnimControl_cl::Create(pMesh->GetSkeleton(), (VisSkeletalAnimSequence_cl*)pTempAnimSequence, VANIMCTRL_LOOP|VSKELANIMCTRL_DEFAULTS, 1.0f, true);
        // and set the control as the input for the final skeletal result
        pFinalSkeletalResult->SetSkeletalAnimInput(pSkeletalAnimControl);
        // add the entity as an events listener
        pSkeletalAnimControl->AddEventListener(pEntity);
        // set the config 
        pEntity->SetAnimConfig(pConfig);
        // report success
        return true;
      }
      else if (pTempAnimSequence->GetType() == VIS_MODELANIM_VERTEX)
      {
        if (pTempAnimSequence->GetLength() > 0.f)
        {
          // if it is a vertex animation, create a config for it
          VisAnimConfig_cl* pConfig = VisAnimConfig_cl::CreateVertexConfig(pMesh, &pVertexAnimDeformer);
          // create its control
          VisVertexAnimControl_cl* pVertexAnimControl = VisVertexAnimControl_cl::Create((VisVertexAnimSequence_cl*)pTempAnimSequence, VANIMCTRL_LOOP|VSKELANIMCTRL_DEFAULTS, 1.0f, true);
          // and add the control to the vertex anim deformer
          pVertexAnimDeformer->AddVertexAnimControl(pVertexAnimControl, 1.0f);
          // add the entity as an events listener
          pVertexAnimControl->AddEventListener(pEntity);
          // set the config 
          pEntity->SetAnimConfig(pConfig);
          // report success
          return true;
        }
        else
        {
          // if it is a morph target, create a config for it
          VisAnimConfig_cl* pConfig = VisAnimConfig_cl::CreateMorphingConfig(pMesh, &pMorphingDeformer);
          // add morph target
          pMorphingDeformer->AddMorphTarget((VisVertexAnimSequence_cl*)pTempAnimSequence, 1.0f);
          // set the config 
          pEntity->SetAnimConfig(pConfig);
          // report success
          return true;
        }
      }
    }
    // if neither a skeletal nor a vertex animation has been found, report failure
    return false;
  }

  VisAnimConfig_cl* pConfig = NULL;
  if ((pAnimSequenceSkeletal) && (pAnimSequenceVertex))
  {
    // if both a vertex and a skeletal animation with the given name have been found,
    // create a combined config for skeletal and vertex animation
    pConfig = VisAnimConfig_cl::CreateSkeletalVertexConfig(pMesh, &pFinalSkeletalResult, &pVertexAnimDeformer);
  }

  if (pAnimSequenceSkeletal)
  {
    if (!pConfig)
    {
        // if it is just a skeletal animation, create a config for it
        pConfig = VisAnimConfig_cl::CreateSkeletalConfig(pMesh, &pFinalSkeletalResult);
    }

    // if a skeletal animation has been found create a control for it
    if (pConfig)
    {
      VisSkeletalAnimControl_cl* pSkeletalAnimControl = VisSkeletalAnimControl_cl::Create(pMesh->GetSkeleton(), pAnimSequenceSkeletal, VANIMCTRL_LOOP|VSKELANIMCTRL_DEFAULTS, 1.0f, true);
      // set it as the input for the final skeletal result
      pFinalSkeletalResult->SetSkeletalAnimInput(pSkeletalAnimControl);
      // and add the entity as an events listener
      pSkeletalAnimControl->AddEventListener(pEntity);
    }
  }

  if (pAnimSequenceVertex)
  {
    if (pAnimSequenceVertex->GetLength() > 0.f)
    {
      if (!pConfig)
      {
          // if it is just a vertex animation, create a config for it
          pConfig = VisAnimConfig_cl::CreateVertexConfig(pMesh, &pVertexAnimDeformer);
      }

      // if a vertex animation has been found create a control for it
      VisVertexAnimControl_cl* pVertexAnimControl = VisVertexAnimControl_cl::Create(pAnimSequenceVertex, VANIMCTRL_LOOP|VSKELANIMCTRL_DEFAULTS, 1.0f, true);
      // add it to the vertex anim deformer
      pVertexAnimDeformer->AddVertexAnimControl(pVertexAnimControl, 1.0f);
      // and add the entity as an events listener
      pVertexAnimControl->AddEventListener(pEntity);
    }
    else
    {
      if (!pConfig)
      {
        // if it is just a morph target, create a config for it
        pConfig = VisAnimConfig_cl::CreateMorphingConfig(pMesh, &pMorphingDeformer);
      }

      // add morph target
      pMorphingDeformer->AddMorphTarget(pAnimSequenceVertex, 1.0f);
    }
  }

  // set the current config (which may be NULL)
  pEntity->SetAnimConfig(pConfig);

  // and report success
  return true;
}

/// =========================================================================== ///
/// VSimpleAnimationComponentManager IVisCallbackHandler_cl Overrides                 ///
/// =========================================================================== ///
// One global instance of our manager
VSimpleAnimationComponentManager VSimpleAnimationComponentManager::g_GlobalManager;


void VSimpleAnimationComponentManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender == &Vision::Callbacks.OnUpdateSceneBegin)
  {
    // call update function on every component
    const int iCount = m_Components.Count();
    for (int i=0;i<iCount;i++)
      m_Components.GetAt(i)->PerFrameUpdate();
  }
}


START_VAR_TABLE(VSimpleAnimationComponent,IVObjectComponent,"Component for simple entity model animation playback",VVARIABLELIST_FLAGS_NONE, "Simple Animation" )
  DEFINE_VAR_VSTRING(VSimpleAnimationComponent, AnimationName, "Name of the animation to playback", "", 0, 0, "dropdownlist(Animation)");
  DEFINE_VAR_BOOL(VSimpleAnimationComponent, UseMotionDelta, "If the component should be using the motion delta of the animation or not", FALSE, 0, 0);
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
