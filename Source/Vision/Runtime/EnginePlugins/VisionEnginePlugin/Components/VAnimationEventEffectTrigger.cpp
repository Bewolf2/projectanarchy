/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Components/VAnimationEventEffectTrigger.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

/// =========================================================================== ///
/// IVAnimationEventTrigger Component Methods                                   ///
/// =========================================================================== ///

// Register the class in the engine module so it is available for RTTI
V_IMPLEMENT_SERIAL(VAnimationEventEffectTrigger, IVAnimationEventTrigger, 0, &g_VisionEngineModule);

VAnimationEventEffectTrigger::VAnimationEventEffectTrigger(int iComponentFlags) : IVTransitionEventTrigger(iComponentFlags)
{
  PositionOffset.setZero();
  OrientationOffset.setZero();
}

/// =========================================================================== ///
/// IVAnimationEventTrigger Overridable                                         ///
/// =========================================================================== ///

bool VAnimationEventEffectTrigger::CommonInit()
{
  // Initialize base component
  if (!IVTransitionEventTrigger::CommonInit())
    return false;
  
  // Get owner entity
  VisBaseEntity_cl *pEntity = (VisBaseEntity_cl *)m_pOwner;
  if (pEntity == NULL)
    return false;

  // Fill the event trigger info
  if (m_iEventTriggerInfoCount <= 0)
  {
    VEventEffectTriggerInfo_t* info = NULL;
    if(m_pActiveTriggerInfo == NULL) //make sure it does not get created more than once
    {
      // Create new list with only one entry and set properties
      info = new VEventEffectTriggerInfo_t();
    }
    else
    {
      info = (VEventEffectTriggerInfo_t*)m_pActiveTriggerInfo;
    }

    info->m_vPositionOffset = PositionOffset;
    info->m_vOrientationOffset= OrientationOffset;


    // Get effect file
    info->m_spEffectFile = VisParticleGroupManager_cl::GlobalManager().LoadFromFile(EffectFilename);
    if (info->m_spEffectFile == NULL || !GetEventTriggerInfoBaseData(info))
    {
      V_SAFE_DELETE(info);
      m_pActiveTriggerInfo = NULL;
      return false;
    }

    // Get Bone Index if specified
    if (!AttachToBone.IsEmpty())
    {
      VDynamicMesh* pMesh = pEntity->GetMesh();
      if (pMesh == NULL)
        return false;
      VisSkeleton_cl *pSkeleton = pMesh->GetSkeleton();
      if (pSkeleton == NULL)
        return false;

      info->m_iAttachToBone = pSkeleton->GetBoneIndexByName(AttachToBone);
    }

    // Set it as the active event trigger info
    m_pActiveTriggerInfo = info;
  }

  return true;
}

void VAnimationEventEffectTrigger::OnAnimationEvent() 
{
  // Get the active event trigger info
  VEventEffectTriggerInfo_t* info = (VEventEffectTriggerInfo_t*) m_pActiveTriggerInfo;

  // Check if effect file is specified
  if (info == NULL || info->m_spEffectFile == NULL)
    return;
  
  // Get bone translation and orientation (if bone ID is 0 no bone has been specified)
  VisBaseEntity_cl* pEntity = (VisBaseEntity_cl *)m_pOwner;
  hkvVec3 vPos = pEntity->GetPosition() + info->m_vPositionOffset;
  hkvVec3 vOri = pEntity->GetOrientation() + info->m_vOrientationOffset;
  if (info->m_iAttachToBone != -1)
  {
    hkvQuat vRot;
    pEntity->GetBoneCurrentWorldSpaceTransformation(info->m_iAttachToBone, vPos, vRot);

    // Add position and orientation offset
    vPos = PositionOffset + vPos;
    hkvQuat vOffsetRot; vOffsetRot.setFromEulerAngles (vOri.data[2], vOri.data[1], vOri.data[0]); 
    vRot = vRot.multiplyReverse (vOffsetRot);

    vRot.getAsEulerAngles (vOri.z, vOri.y, vOri.x);
  }

  // Trigger effect
  VisParticleEffect_cl* pEffectInstance = info->m_spEffectFile->CreateParticleEffectInstance(vPos, vOri);
  pEffectInstance->SetRemoveWhenFinished(true);
}

/// =========================================================================== ///
/// VTypedObject Overrides                                                      ///
/// =========================================================================== ///

void VAnimationEventEffectTrigger::MessageFunction( int iID, INT_PTR iParamA, INT_PTR iParamB )
{  
  IVTransitionEventTrigger::MessageFunction(iID, iParamA, iParamB);

#ifdef WIN32
  if (iID == VIS_MSG_EDITOR_GETSTANDARDVALUES)
  {
    // Get bone names
    const char *szKey = (const char *)iParamA;
    if (!strcmp(szKey,"Bone"))
    {
      // Check for model and skeleton
      VisBaseEntity_cl* pEntity = (VisBaseEntity_cl *)m_pOwner;
      if (pEntity == NULL)
        return;
      VDynamicMesh* pMesh = pEntity->GetMesh();
      if (pMesh == NULL)
        return;
      VisSkeleton_cl *pSkeleton = pMesh->GetSkeleton();
      if (pSkeleton == NULL)
        return;

      // Fill list with bone names (first entry is empty)
      VStrList *pDestList = (VStrList*) iParamB;
      pDestList->AddString(" ");
      for (int i = 0; i < pSkeleton->GetBoneCount(); i++)
        pDestList->AddString(pSkeleton->GetBone(i)->m_sBoneName.AsChar());
    }
  }
#endif
}

/// =========================================================================== ///
/// Serialization                                                               ///
/// =========================================================================== ///

void VAnimationEventEffectTrigger::Serialize( VArchive &ar )
{
  char iLocalVersion = ANIMATIONEVENTEFFECTTRIGGER_VERSION_CURRENT;
  IVTransitionEventTrigger::Serialize(ar);

  if (ar.IsLoading())
  {
    ar >> iLocalVersion;
    VASSERT_MSG(iLocalVersion == ANIMATIONEVENTEFFECTTRIGGER_VERSION_CURRENT, "Invalid local version. Please re-export");

    ar >> EffectFilename;  
    ar >> AttachToBone;    
    PositionOffset.SerializeAsVisVector(ar);  
    OrientationOffset.SerializeAsVisVector(ar);

    CommonInit();
  } 
  else
  {
    ar << iLocalVersion;

    ar << EffectFilename;  
    ar << AttachToBone;    
    PositionOffset.SerializeAsVisVector(ar);  
    OrientationOffset.SerializeAsVisVector(ar);
  }
}

/// =========================================================================== ///
/// VAnimationEventEffectTrigger Variable Table                                 ///
/// =========================================================================== ///

START_VAR_TABLE(VAnimationEventEffectTrigger, IVTransitionEventTrigger, "Animation Event Effect Trigger Component.", VCOMPONENT_ALLOW_MULTIPLE, "Event Effect Trigger")
  DEFINE_VAR_VSTRING      (VAnimationEventEffectTrigger, EffectFilename,     "Filename of the effect",                                                    "", 0, 0, "filepicker(.XML)");
  DEFINE_VAR_VSTRING      (VAnimationEventEffectTrigger, AttachToBone,       "Name of the bone the effect should be attached too (optional)",             "", 0, 0, "dropdownlist(Bone)");
  DEFINE_VAR_VECTOR_FLOAT (VAnimationEventEffectTrigger, PositionOffset,     "Relative position either to the owner object root or the attached bone",    "0,0,0", 0, 0);
  DEFINE_VAR_VECTOR_FLOAT (VAnimationEventEffectTrigger, OrientationOffset,  "Relative orientation either to the owner object root or the attached bone", "0,0,0", 0, 0);
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
