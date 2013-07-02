/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Components/VEntityLODComponent.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Animation/Transition/VTransitionStateMachine.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiAnimNormalizeMixerNode.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

/// =========================================================================== ///
/// Entity LOD Level Info Methods                                               ///
/// =========================================================================== ///
unsigned int VEntityLODLevelInfo::s_iTagCtr = 1;
VisCallback_cl VEntityLODLevelInfo::OnEntityLODDestroyed;


VEntityLODLevelInfo::~VEntityLODLevelInfo()
{
  VEntityLODDataObject data(&VEntityLODLevelInfo::OnEntityLODDestroyed, this);
  VEntityLODLevelInfo::OnEntityLODDestroyed.TriggerCallbacks(&data);
}

bool VEntityLODLevelInfo::SetModelFile(const char *szFilename, bool bAppyMotionDelta)
{
  // Load model
  m_spMesh = Vision::Game.LoadDynamicMesh(szFilename, true);
  m_spAnimConfig = NULL;
  if (m_spMesh == NULL)
    return false;

  // Create anim config only for models with skeleton
  if (m_spMesh->GetSkeleton() == NULL)
    return true;

  // Create a new final result for this config
  VisAnimFinalSkeletalResult_cl* pFinalSkeletalResult;
  m_spAnimConfig = VisAnimConfig_cl::CreateSkeletalConfig(m_spMesh, &pFinalSkeletalResult);
  if (!bAppyMotionDelta)
    m_spAnimConfig->SetFlags(m_spAnimConfig->GetFlags() | APPLY_MOTION_DELTA);
  m_spFinalSkeletalResult = pFinalSkeletalResult;

  return true;
}

/// =========================================================================== ///
/// Entity LOD Component Methods                                                ///
/// =========================================================================== ///

// Register the class in the engine module so it is available for RTTI
#ifdef SUPPORTS_LOD_HYSTERESIS_THRESHOLDING
  V_IMPLEMENT_SERIAL(VEntityLODComponent, IVLODHysteresisComponent, 0, &g_VisionEngineModule);
#else
  V_IMPLEMENT_SERIAL(VEntityLODComponent, IVObjectComponent, 0, &g_VisionEngineModule);
#endif //SUPPORTS_LOD_HYSTERESIS_THRESHOLDING


// One global instance of our manager
VEntityLODComponentManager VEntityLODComponentManager::g_GlobalManager;

VEntityLODComponent::VEntityLODComponent(int iComponentFlags) :
#ifdef SUPPORTS_LOD_HYSTERESIS_THRESHOLDING
  IVLODHysteresisComponent(iComponentFlags)
#else
  IVObjectComponent(0, iComponentFlags)
#endif //SUPPORTS_LOD_HYSTERESIS_THRESHOLDING
{
  m_pLevels = NULL;
  Level_High_Distance = 0.0f;

  LOD_LevelMode = VLOD_AUTO;
  LOD_LevelCount = 1;
  Level_Medium_Distance = 500.f * Vision::World.GetGlobalUnitScaling();
  Level_Low_Distance = 1000.f * Vision::World.GetGlobalUnitScaling();
  Level_UltraLow_Distance = 1500.f * Vision::World.GetGlobalUnitScaling();
}

VEntityLODComponent::~VEntityLODComponent()
{
  V_SAFE_DELETE_ARRAY(m_pLevels);
}

void VEntityLODComponent::CommonInit(bool bFirstInit)
{
  // Get owner entity
  VisBaseEntity_cl *pEntity = (VisBaseEntity_cl *)m_pOwner;
  if (pEntity == NULL)
    return;

  // Get number of LOD levels
  V_SAFE_DELETE_ARRAY(m_pLevels);
  m_pLevels = new VEntityLODLevelInfo[LOD_LevelCount + 2];

  const float fFarClipDistance = pEntity->GetFarClipDistance();

  // Owner entity mesh
  VDynamicMesh *pMesh = pEntity->GetMesh();
  if (pMesh != NULL)
  {
    if (bFirstInit)
      Level_High_Mesh = pMesh->GetFilename();
    if (Level_High_Mesh.IsEmpty())
      Vision::Error.Warning("Model not defined for LOD Level [%i]", VLOD_HIGH);
    else if (!m_pLevels[VLOD_HIGH].SetModelFile(Level_High_Mesh))
      Vision::Error.Warning("Model '%s' could not be loaded for LOD Level [%i]", Level_High_Mesh.AsChar(), VLOD_HIGH);
    m_pLevels[VLOD_HIGH].SetSwitchDistance(Level_High_Distance, (LOD_LevelCount >= VLOD_HIGH)? Level_Medium_Distance : fFarClipDistance);
    m_pLevels[VLOD_HIGH].m_pEntity = pEntity;
  }

  // LOD Level Medium
  if (LOD_LevelCount >= VLOD_HIGH)
  {
    if (Level_Medium_Mesh.IsEmpty())
      Vision::Error.Warning("Model not defined for LOD Level [%i]", VLOD_MEDIUM);
    else if (!m_pLevels[VLOD_MEDIUM].SetModelFile(Level_Medium_Mesh))
      Vision::Error.Warning("Model '%s' could not be loaded for LOD Level [%i]", Level_Medium_Mesh.AsChar(), VLOD_MEDIUM);
    m_pLevels[VLOD_MEDIUM].SetSwitchDistance(Level_Medium_Distance, (LOD_LevelCount >= VLOD_MEDIUM)? Level_Low_Distance : fFarClipDistance);
    m_pLevels[VLOD_MEDIUM].m_pEntity = pEntity;
  }

  // LOD Level Low
  if (LOD_LevelCount >= VLOD_MEDIUM)
  {
    if (Level_Low_Mesh.IsEmpty())
      Vision::Error.Warning("Model not defined for LOD Level [%i]", VLOD_LOW);
    else if (!m_pLevels[VLOD_LOW].SetModelFile(Level_Low_Mesh))
      Vision::Error.Warning("Model '%s' could not be loaded for LOD Level [%i]", Level_Low_Mesh.AsChar(), VLOD_LOW);
    m_pLevels[VLOD_LOW].SetSwitchDistance(Level_Low_Distance, (LOD_LevelCount >= VLOD_LOW)? Level_UltraLow_Distance : fFarClipDistance);
    m_pLevels[VLOD_LOW].m_pEntity = pEntity;
  }

  // LOD Level UltraLow
  if (LOD_LevelCount >= VLOD_LOW)
  {
    if (Level_UltraLow_Mesh.IsEmpty())
      Vision::Error.Warning("Model not defined for LOD Level [%i]", VLOD_ULTRALOW);
    else if (!m_pLevels[VLOD_ULTRALOW].SetModelFile(Level_UltraLow_Mesh))
      Vision::Error.Warning("Model '%s' could not be loaded for LOD Level [%i]", Level_UltraLow_Mesh.AsChar(), VLOD_ULTRALOW);
    m_pLevels[VLOD_ULTRALOW].SetSwitchDistance(Level_UltraLow_Distance, fFarClipDistance);
    m_pLevels[VLOD_ULTRALOW].m_pEntity = pEntity;
  }

  // If an anim config is already set on the entity set it as the anim root
  if (pEntity->GetAnimConfig() != NULL && pEntity->GetAnimConfig()->GetFinalResult()->GetSkeletalAnimInput() != NULL)
    SetSkeletalAnimRootNode(pEntity->GetAnimConfig()->GetFinalResult()->GetSkeletalAnimInput());
  
  // Set LOD level
  m_iCurrentLevel = -1;
  if (LOD_LevelMode == VLOD_NONE)
    SetLODLevel(VLOD_HIGH);
  else if (LOD_LevelMode == VLOD_AUTO)
    UpdateLOD(true);
  else if (LOD_LevelMode < LOD_LevelCount + 2)
    SetLODLevel((VEntityLODLevel_e)LOD_LevelMode);
}

void VEntityLODComponent::PerFrameUpdate()
{
  if (LOD_LevelMode == VLOD_AUTO && ((VisBaseEntity_cl *)m_pOwner)->WasVisibleInAnyLastFrame())
    UpdateLOD(false);
}

void VEntityLODComponent::SetLODLevel(VEntityLODLevel_e newLevel)
{
  LOD_LevelMode = newLevel;
  if(LOD_LevelMode != VLOD_AUTO)
    UpdateLODLevel(newLevel);
}

void VEntityLODComponent::UpdateLOD(bool bFirstTime)
{
  float fDistance = GetDistanceToCamera();

  // compute LOD level
  int iLevel = 0;
  #ifdef SUPPORTS_LOD_HYSTERESIS_THRESHOLDING
    float fThreshold = VLODHysteresisManager::GetThreshold(VLHT_ENTITIES);

    if (fThreshold > 0.0f)
    {
      if (bFirstTime)
      {
        for (int i = 0; i < LOD_LevelCount + 2; i++)
        {
          if (m_pLevels[i].m_fMinSwitchDistance < fDistance)
            iLevel = i;
        }
      }
      else
      {
        iLevel = m_iCurrentLevel;
        for (;;)
        {
          const float fNearSwitchDistance = m_pLevels[iLevel].m_fMinSwitchDistance;
          const float fFarSwitchDistance = m_pLevels[iLevel].m_fMaxSwitchDistance + fThreshold;
          bool bNear = fDistance < fNearSwitchDistance;
          bool bFar = fDistance >= fFarSwitchDistance;

          if (!bNear && !bFar)
          {
            break;
          }
          else if (bNear)
          {
            if (iLevel > 0)
              iLevel--;
            else
              break;
          }
          else if (bFar)
          {
            if (iLevel <= LOD_LevelCount)
              iLevel++;
            else
              break;
          }
        }

        VisBaseEntity_cl *pEntity = (VisBaseEntity_cl *)m_pOwner;
        if (pEntity != NULL)
          pEntity->SetClipSettings(pEntity->GetNearClipDistance(), pEntity->GetFarClipDistance(), &pEntity->GetPosition());
      }
    }
    else
  #endif //SUPPORTS_LOD_HYSTERESIS_THRESHOLDING
  {
    for (int i = 0; i < LOD_LevelCount + 2; i++)
      if (m_pLevels[i].m_fMinSwitchDistance < fDistance)
        iLevel = i;
  }

  UpdateLODLevel(iLevel);
}

void VEntityLODComponent::UpdateLODLevel(int newLevel)
{
  if (m_iCurrentLevel == newLevel)
    return;

  // Get owner entity
  VisBaseEntity_cl *pEntity = (VisBaseEntity_cl *)m_pOwner;

  m_iCurrentLevel = newLevel;
  VEntityLODLevelInfo &info = GetLevelInfo(newLevel);
  if (info.m_spMesh != NULL)
  {
    pEntity->SetMesh(info.m_spMesh);
    pEntity->SetAnimConfig(info.m_spAnimConfig);
  }
}

bool VEntityLODComponent::ConnectStateMachine()
{
  // Check for a TSM component  
  VTransitionStateMachine *pTSM = m_pOwner->Components().GetComponentOfType<VTransitionStateMachine>();
  if (pTSM == NULL)
    return false;

  SetSkeletalAnimRootNode(pTSM->GetTransitionMixer());
  return true;
}

bool VEntityLODComponent::FindAnimConfig()
{
  // First check if there is a transition state machine
  if (ConnectStateMachine())
    return true;

  // Otherwise get owner entity anim config
  VisBaseEntity_cl *pEntity = (VisBaseEntity_cl *)m_pOwner;
  if (pEntity->GetAnimConfig() == NULL)  
    return false;
  
  SetSkeletalAnimRootNode(pEntity->GetAnimConfig()->GetFinalResult()->GetSkeletalAnimInput());
  return true;
}

#ifdef SUPPORTS_LOD_HYSTERESIS_THRESHOLDING
int VEntityLODComponent::GetLODLevel() const
{
  return (int)GetCurrentLODLevel();
}
#endif //SUPPORTS_LOD_HYSTERESIS_THRESHOLDING

#if defined(WIN32) || defined(_VISION_DOC)

void VEntityLODComponent::GetVariableAttributes(VisVariable_cl *pVariable, VVariableAttributeInfo &destInfo)
{
  // Check whether we need to display the LOD low model property
  if (!strcmp(pVariable->GetName(),"Level_Low_Mesh"))
  {
    char pStr[128];
    GetVariable("LOD_LevelCount")->GetValue(this, pStr, true);

    if (!strcmp(pStr, "1"))
      destInfo.m_bHidden = true;
  }
  // Check whether we need to display the LOD low distance property
  else if (!strcmp(pVariable->GetName(),"Level_Low_Distance"))
  {
    char pStr[128];
    GetVariable("LOD_LevelCount")->GetValue(this, pStr, true);

    if (!strcmp(pStr, "1"))
      destInfo.m_bHidden = true;
  }
  // Check whether we need to display the LOD ultralow model property
  else if (!strcmp(pVariable->GetName(),"Level_UltraLow_Mesh"))
  {
    char pStr[128];
    GetVariable("LOD_LevelCount")->GetValue(this, pStr, true);

    if (!strcmp(pStr, "1") || !strcmp(pStr, "2"))
      destInfo.m_bHidden = true;
  }
  // Check whether we need to display the LOD ultralow distance property
  else if (!strcmp(pVariable->GetName(),"Level_UltraLow_Distance"))
  {
    char pStr[128];
    GetVariable("LOD_LevelCount")->GetValue(this, pStr, true);

    if (!strcmp(pStr, "1") || !strcmp(pStr, "2"))
      destInfo.m_bHidden = true;
  }
}

#endif

void VEntityLODComponent::MessageFunction( int iID, INT_PTR iParamA, INT_PTR iParamB )
{
#ifdef WIN32
  if ( iID == VIS_MSG_EDITOR_PROPERTYCHANGED )
    CommonInit(true);
#endif

  IVObjectComponent::MessageFunction(iID, iParamA, iParamB);
}


/// =========================================================================== ///
/// IVObjectComponent Overrides                                                 ///
/// =========================================================================== ///

BOOL VEntityLODComponent::CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
  if (!IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut))
    return false;

  if (!pObject->IsOfType(V_RUNTIME_CLASS(VisBaseEntity_cl)))
  {
    sErrorMsgOut = "Component can only be added to instances of VisBaseEntity_cl or derived classes.";
    return FALSE;
  }

  return TRUE;
}

void VEntityLODComponent::SetOwner(VisTypedEngineObject_cl *pOwner)
{
  IVObjectComponent::SetOwner(pOwner);

  // Insert code here to respond to attaching this component to an object
  // This function is called with pOwner==NULL when de-attaching.
  if (pOwner != NULL)
  {
    VEntityLODComponentManager::GlobalManager().Instances().AddUnique(this);
    CommonInit(true);
  }
  else
  {
    VEntityLODComponentManager::GlobalManager().Instances().SafeRemove(this);
  }
}

void VEntityLODComponent::OnVariableValueChanged(VisVariable_cl *pVar, const char * value)
{
  // Don't perform any update/invalidation code in case we don't have an owner yet.
  // SetOwner will perform an initialization once it gets called.
  if (GetOwner() == NULL)
    return;

  CommonInit(false);
}

void VEntityLODComponent::Serialize( VArchive &ar )
{
  char iLocalVersion = ENTITYLODCOMPONENT_VERSION_CURRENT;
  IVObjectComponent::Serialize(ar);
  if (ar.IsLoading())
  {
    ar >> iLocalVersion;
    VASSERT_MSG(iLocalVersion == ENTITYLODCOMPONENT_VERSION_CURRENT, "Invalid local version. Please re-export");

    ar >> LOD_LevelMode;
    ar >> LOD_LevelCount;
    ar >> Level_Medium_Mesh;
    ar >> Level_Low_Mesh;
    ar >> Level_UltraLow_Mesh;
    ar >> Level_Medium_Distance;
    ar >> Level_Low_Distance;
    ar >> Level_UltraLow_Distance;

    CommonInit(true);
  } 
  else
  {
    ar << iLocalVersion;

    ar << LOD_LevelMode;
    ar << LOD_LevelCount;
    ar << Level_Medium_Mesh;
    ar << Level_Low_Mesh;
    ar << Level_UltraLow_Mesh;
    ar << Level_Medium_Distance;
    ar << Level_Low_Distance;
    ar << Level_UltraLow_Distance;
  }
}


/// =========================================================================== ///
/// VEntityLODComponentManager IVisCallbackHandler_cl Overrides                 ///
/// =========================================================================== ///

void VEntityLODComponentManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender == &Vision::Callbacks.OnUpdateSceneFinished)
  {
    // call update function on every component
    const int iCount = m_Components.Count();
    for (int i=0;i<iCount;i++)
      m_Components.GetAt(i)->PerFrameUpdate();
  }
  else if (pData->m_pSender == &Vision::Callbacks.OnAfterSceneLoaded)
  {
    // call update function on every component
    const int iCount = m_Components.Count();
    for (int i=0;i<iCount;i++)
      m_Components.GetAt(i)->FindAnimConfig();
  }
}


/// =========================================================================== ///
/// VEntityLODComponent Variable Table                                          ///
/// =========================================================================== ///

START_VAR_TABLE(VEntityLODComponent,IVObjectComponent, "Entity LOD Component. Can be attached to any entity so it can have multiple LOD levels each represented by a different model. The highest level is defined by the model of the owner entity.", VVARIABLELIST_FLAGS_NONE, "Entity LOD" )
  DEFINE_VAR_ENUM     (VEntityLODComponent, LOD_LevelMode,            "LOD Mode - Activate a specific LOD Level or let the component set the LOD dynamically depending on the distance to the camera.", "LOD_AUTO", "LOD_HIGH,LOD_MEDIUM,LOD_LOW,LOD_ULTRALOW,LOD_DISABLED,LOD_AUTO", 0, 0);
  DEFINE_VAR_ENUM     (VEntityLODComponent, LOD_LevelCount,           "LOD Count - Specify the number of LOD Levels.", "1","1,2,3", 0, 0);
  DEFINE_VAR_VSTRING  (VEntityLODComponent, Level_Medium_Mesh,        "Modelfile for the Medium Level.", "", 0, 0, "assetpicker(Model)");
  DEFINE_VAR_VSTRING  (VEntityLODComponent, Level_Low_Mesh,           "Modelfile for the Low Level.", "", 0, 0, "assetpicker(Model)");
  DEFINE_VAR_VSTRING  (VEntityLODComponent, Level_UltraLow_Mesh,      "Modelfile for the Ultralow Level.", "", 0, 0, "assetpicker(Model)");
  DEFINE_VAR_FLOAT    (VEntityLODComponent, Level_Medium_Distance,    "Distance for the Medium Level.", "500.0", DISPLAY_HINT_GLOBALUNITSCALED, 0);
  DEFINE_VAR_FLOAT    (VEntityLODComponent, Level_Low_Distance,       "Distance for the Low Level.", "1000.0", DISPLAY_HINT_GLOBALUNITSCALED, 0);  
  DEFINE_VAR_FLOAT    (VEntityLODComponent, Level_UltraLow_Distance,  "Distance for the Ultralow Level.", "1500.0", DISPLAY_HINT_GLOBALUNITSCALED, 0);
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
