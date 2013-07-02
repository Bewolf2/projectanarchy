/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// KeyControlledTransitionCharacter_cl
// ***********************************************************************************************
// Extends the transition state machine based animated character class with code 
// for converting the keyboard input to character movements.
// ***********************************************************************************************

#include <Vision/Samples/Engine/ModelMerging/ModelMergingPCH.h>
#include <Vision/Samples/Engine/ModelMerging/MergedModelFactory.hpp>

MergedModelFactory_cl::MergedModelFactory_cl()
{
  m_pMergedModelEntity = NULL;
  m_pPreviewModelEntities = NULL;

  m_iModelsToMerge = 2;
  m_bArmArmor = false;
  m_bShoulderArmor = false;
  m_bLegsArmor = false;
  m_bKneeArmor = false;
  m_bAccessoire = false;
  m_bBelt = false;
  m_bCloth = false;
  m_bBeard = false;
  m_bAxe = true;

  m_vPos = hkvVec3(0.f, 0.f, 0.f);
  m_vOri = hkvVec3(0.f, 0.f, 0.f);

  m_pPlayerCamera = new VPlayerCamera();
  m_pPlayerCamera->InitialYaw = 90.f;
  m_pPlayerCamera->RelativeLookAtHeight = 1.0f;
  m_pPlayerCamera->Follow = false;
  m_pPlayerCamera->Collides = false;
  m_pPlayerCamera->DepthOfField = true;
#if defined( _VISION_MOBILE )
  float fContentScaleFactor = Vision::Video.GetVideoConfig()->DisplayDensity;
  m_pPlayerCamera->CameraSensitivity /= fContentScaleFactor * 1.5f;
#endif
  m_pCameraEntity = Vision::Game.CreateEntity( "VisBaseEntity_cl", hkvVec3(0.f, 0.f, 100.f), "");
  m_pCameraEntity->AddComponent(m_pPlayerCamera);
}

MergedModelFactory_cl::~MergedModelFactory_cl()
{
  // Delete merged model
  if (m_pMergedModelEntity)
     V_SAFE_DISPOSEOBJECT(m_pMergedModelEntity);
  
  // Delete model preview
  if (m_pPreviewModelEntities != NULL)
    for (int i = 0; i < BARBARIAN_MAX; i++)
      V_SAFE_DISPOSEOBJECT(m_pPreviewModelEntities[i])

  V_SAFE_DELETE_ARRAY(m_pPreviewModelEntities);
}

void MergedModelFactory_cl::ToggleBodyPart(BODY_PART part)
{
  switch (part)
  {
  case BARBARIAN_ARM: m_bArmArmor = !m_bArmArmor; 
    m_iModelsToMerge = m_bArmArmor ? m_iModelsToMerge + 1 : m_iModelsToMerge - 1;
    break;                   
  case BARBARIAN_SHOULDER: m_bShoulderArmor = !m_bShoulderArmor;
    m_iModelsToMerge = m_bShoulderArmor ? m_iModelsToMerge + 1 : m_iModelsToMerge - 1;
    break;                  
  case BARBARIAN_LEGS: m_bLegsArmor = !m_bLegsArmor;
    m_iModelsToMerge = m_bLegsArmor ? m_iModelsToMerge + 1 : m_iModelsToMerge - 1;
    break;                  
  case BARBARIAN_KNEE: m_bKneeArmor = !m_bKneeArmor;
    m_iModelsToMerge = m_bKneeArmor ? m_iModelsToMerge + 1 : m_iModelsToMerge - 1;
    break;                  
  case BARBARIAN_ACCESSOIRE: m_bAccessoire = !m_bAccessoire;
    m_iModelsToMerge = m_bAccessoire ? m_iModelsToMerge + 1 : m_iModelsToMerge - 1;
    break;                  
  case BARBARIAN_BELT: m_bBelt = !m_bBelt;
    m_iModelsToMerge = m_bBelt ? m_iModelsToMerge + 1 : m_iModelsToMerge - 1;
    break;                  
  case BARBARIAN_CLOTH: m_bCloth = !m_bCloth;
    m_iModelsToMerge = m_bCloth ? m_iModelsToMerge + 1 : m_iModelsToMerge - 1;
    break;                  
  case BARBARIAN_BEARD: m_bBeard = !m_bBeard;
    m_iModelsToMerge = m_bBeard ? m_iModelsToMerge + 1 : m_iModelsToMerge - 1;
    break;                  
  case BARBARIAN_AXE: m_bAxe = !m_bAxe;
    break;
  default: 
    break;
  }

  // Update preview
  UpdatePreview();
}

void MergedModelFactory_cl::MergeModel()
{
  VASSERT(m_iModelsToMerge > 1 && m_iModelsToMerge < 11);
  DeleteModels();

  // Always use the body
  MeshMergeInfo_t info[32]; // maximum count
  info[0].m_pMesh = Vision::Game.LoadDynamicMesh("Barbarian_Body.model", true);

  int i = 1;

  // Armor
  if (m_bArmArmor)
    info[i++].m_pMesh = Vision::Game.LoadDynamicMesh("Barbarian_Arm.model", true);
  if (m_bShoulderArmor)
    info[i++].m_pMesh = Vision::Game.LoadDynamicMesh("Barbarian_Shoulder.model", true);
  if (m_bLegsArmor)
    info[i++].m_pMesh = Vision::Game.LoadDynamicMesh("Barbarian_Legs.model", true);
  if (m_bKneeArmor)
    info[i++].m_pMesh = Vision::Game.LoadDynamicMesh("Barbarian_Knee.model", true);
  if (m_bAccessoire)
    info[i++].m_pMesh = Vision::Game.LoadDynamicMesh("Barbarian_Accessoire.model", true);
  if (m_bBelt)
    info[i++].m_pMesh = Vision::Game.LoadDynamicMesh("Barbarian_Belt.model", true);
  if (m_bCloth)
    info[i++].m_pMesh = Vision::Game.LoadDynamicMesh("Barbarian_Cloth.model", true);
  if (m_bBeard)
    info[i++].m_pMesh = Vision::Game.LoadDynamicMesh("Barbarian_Beard.model", true);

  // Weapon
  if (m_bAxe)
    info[i++].m_pMesh = Vision::Game.LoadDynamicMesh("Barbarian_Axe.model", true);
  else
    info[i++].m_pMesh = Vision::Game.LoadDynamicMesh("Barbarian_Sword.model", true);

  // Setup model
  m_pMergedModelEntity = (KeyControlledTransitionBarbarian_cl *) Vision::Game.CreateEntity("KeyControlledTransitionBarbarian_cl", m_vPos, "Barbarian_Body.model");

  if (m_iModelsToMerge > 1)
  {
    // Merge model
    VMeshManager &modelManager(VDynamicMesh::GetResourceManager());
    VDynamicMesh *pMergedModel = modelManager.MergeDynamicMeshes("MergedModel", info, m_iModelsToMerge);
    VTransitionStateMachine* pTransitionStateMachine = m_pMergedModelEntity->Components().GetComponentOfType<VTransitionStateMachine>();
    if(pTransitionStateMachine != NULL)
      m_pMergedModelEntity->RemoveComponent(pTransitionStateMachine);

    m_pMergedModelEntity->SetMesh(pMergedModel);

    // Load sequence set
    VisAnimSequenceSet_cl *pSet = Vision::Animations.GetSequenceSetManager()->LoadAnimSequenceSet("Barbarian_Body.anim");
    m_pMergedModelEntity->GetMesh()->GetSequenceSetCollection()->Add(pSet);

    if(pTransitionStateMachine != NULL)
      m_pMergedModelEntity->AddComponent(pTransitionStateMachine);
  }

  // Init entity
  m_pMergedModelEntity->InitFunction();
  m_pMergedModelEntity->SetPosition(m_vPos);
  m_pMergedModelEntity->SetOrientation(m_vOri);
  m_pCameraEntity->AttachToParent(m_pMergedModelEntity);
  m_pPlayerCamera->ResetOldPosition();
  m_pPlayerCamera->Follow = false;
  m_pPlayerCamera->Zoom = true;
}

void MergedModelFactory_cl::PreviewModel()
{
  DeleteModels();

  m_pPreviewModelEntities = new VisBaseEntity_cl*[BARBARIAN_MAX];
  m_pPreviewModelEntities[BARBARIAN_BODY] = Vision::Game.CreateEntity("VisBaseEntity_cl", hkvVec3(0.f, 0.f, 0.f), "Barbarian_Body.model");
  m_pPreviewModelEntities[BARBARIAN_ARM] = Vision::Game.CreateEntity("VisBaseEntity_cl", hkvVec3(0.f, 0.f, 0.f), "Barbarian_Arm.model");
  m_pPreviewModelEntities[BARBARIAN_SHOULDER] = Vision::Game.CreateEntity("VisBaseEntity_cl", hkvVec3(0.f, 0.f, 0.f), "Barbarian_Shoulder.model");
  m_pPreviewModelEntities[BARBARIAN_LEGS] = Vision::Game.CreateEntity("VisBaseEntity_cl", hkvVec3(0.f, 0.f, 0.f), "Barbarian_Legs.model");
  m_pPreviewModelEntities[BARBARIAN_KNEE] = Vision::Game.CreateEntity("VisBaseEntity_cl", hkvVec3(0.f, 0.f, 0.f), "Barbarian_Knee.model");
  m_pPreviewModelEntities[BARBARIAN_ACCESSOIRE] = Vision::Game.CreateEntity("VisBaseEntity_cl", hkvVec3(0.f, 0.f, 0.f), "Barbarian_Accessoire.model");
  m_pPreviewModelEntities[BARBARIAN_BELT] = Vision::Game.CreateEntity("VisBaseEntity_cl", hkvVec3(0.f, 0.f, 0.f), "Barbarian_Belt.model");
  m_pPreviewModelEntities[BARBARIAN_CLOTH] = Vision::Game.CreateEntity("VisBaseEntity_cl", hkvVec3(0.f, 0.f, 0.f), "Barbarian_Cloth.model");
  m_pPreviewModelEntities[BARBARIAN_BEARD] = Vision::Game.CreateEntity("VisBaseEntity_cl", hkvVec3(0.f, 0.f, 0.f), "Barbarian_Beard.model");
  m_pPreviewModelEntities[BARBARIAN_AXE] = Vision::Game.CreateEntity("VisBaseEntity_cl", hkvVec3(0.f, 0.f, 0.f), "Barbarian_Axe.model");
  m_pPreviewModelEntities[BARBARIAN_SWORD] = Vision::Game.CreateEntity("VisBaseEntity_cl", hkvVec3(0.f, 0.f, 0.f), "Barbarian_Sword.model");

  // Setup animation system
  VisAnimFinalSkeletalResult_cl* pFinalSkeletalResult;
  VisAnimConfig_cl* pConfig = VisAnimConfig_cl::CreateSkeletalConfig(m_pPreviewModelEntities[BARBARIAN_BODY]->GetMesh(), &pFinalSkeletalResult);
  
  // Set animation config
  for (int i = 0; i < BARBARIAN_MAX; i++)
  {
    m_pPreviewModelEntities[i]->SetAnimConfig(pConfig);
    m_pPreviewModelEntities[i]->GetAnimConfig()->StartSkeletalAnimation(m_pPreviewModelEntities[i], "Idle", VSKELANIMCTRL_DEFAULTS | VANIMCTRL_LOOP);
    m_pPreviewModelEntities[i]->GetAnimConfig()->SetFlags(m_pPreviewModelEntities[i]->GetAnimConfig()->GetFlags() | APPLY_MOTION_DELTA);
    m_pPreviewModelEntities[i]->SetPosition(m_vPos);
    m_pPreviewModelEntities[i]->SetOrientation(m_vOri);
#ifndef _VISION_PSP2
    m_pPreviewModelEntities[i]->SetCastShadows(TRUE);
#endif
  }
  
  m_pCameraEntity->AttachToParent(m_pPreviewModelEntities[BARBARIAN_BODY]);
  m_pPlayerCamera->ResetOldPosition();
  m_pPlayerCamera->Follow = true;
  m_pPlayerCamera->Zoom = false;
  m_pPlayerCamera->InitialYaw = -90.f;

  
  UpdatePreview();
}

void MergedModelFactory_cl::UpdatePreview()
{
  VASSERT(m_pPreviewModelEntities);

  m_pPreviewModelEntities[BARBARIAN_ARM]->SetVisibleBitmask(m_bArmArmor ? 0xffffffff : 0);            
  m_pPreviewModelEntities[BARBARIAN_SHOULDER]->SetVisibleBitmask(m_bShoulderArmor ? 0xffffffff : 0);    
  m_pPreviewModelEntities[BARBARIAN_LEGS]->SetVisibleBitmask(m_bLegsArmor ? 0xffffffff : 0);          
  m_pPreviewModelEntities[BARBARIAN_KNEE]->SetVisibleBitmask(m_bKneeArmor ? 0xffffffff : 0);          
  m_pPreviewModelEntities[BARBARIAN_ACCESSOIRE]->SetVisibleBitmask(m_bAccessoire ? 0xffffffff : 0);   
  m_pPreviewModelEntities[BARBARIAN_BELT]->SetVisibleBitmask(m_bBelt ? 0xffffffff : 0);               
  m_pPreviewModelEntities[BARBARIAN_CLOTH]->SetVisibleBitmask(m_bCloth ? 0xffffffff : 0);             
  m_pPreviewModelEntities[BARBARIAN_BEARD]->SetVisibleBitmask(m_bBeard ? 0xffffffff : 0);             
  m_pPreviewModelEntities[BARBARIAN_AXE]->SetVisibleBitmask(m_bAxe ? 0xffffffff : 0);  
  m_pPreviewModelEntities[BARBARIAN_SWORD]->SetVisibleBitmask(!m_bAxe ? 0xffffffff : 0);  
}

void MergedModelFactory_cl::DeleteModels()
{
  // Detach camera 
  m_pCameraEntity->DetachFromParent();

  // Delete model preview
  if (m_pPreviewModelEntities != NULL)
  {
    m_vPos = m_pPreviewModelEntities[0]->GetPosition();
    m_vOri = m_pPreviewModelEntities[0]->GetOrientation();

    for (int i = 0; i < BARBARIAN_MAX; i++)
      V_SAFE_DISPOSEOBJECT(m_pPreviewModelEntities[i]);
  }

  V_SAFE_DELETE_ARRAY(m_pPreviewModelEntities);

  // Delete merged model
  if (m_pMergedModelEntity != NULL)
  {
    m_vPos = m_pMergedModelEntity->GetPosition();
    m_vOri = m_pMergedModelEntity->GetOrientation();

    VDynamicMesh::GetResourceManager().RemoveResource(m_pMergedModelEntity->GetMesh());

    V_SAFE_DISPOSEOBJECT(m_pMergedModelEntity);
  }
}

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
