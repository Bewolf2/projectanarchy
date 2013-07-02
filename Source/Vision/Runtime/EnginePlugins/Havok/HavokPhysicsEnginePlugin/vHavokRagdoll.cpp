/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRagdoll.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokFileStreamAccess.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCharacterController.hpp>


#include <Common/Base/System/Io/Reader/Buffered/hkBufferedStreamReader.h>
#include <Common/Base/System/Io/IStream/hkIStream.h>
#include <Common/Serialize/Util/hkSerializeUtil.h>
#include <Common/Serialize/Util/hkRootLevelContainer.h>
#include <Physics2012/Utilities/Serialize/hkpPhysicsData.h>
#include <Physics2012/Utilities/Dynamics/ScaleSystem/hkpSystemScalingUtility.h>
#include <Physics2012/Utilities/Collide/Filter/GroupFilter/hkpGroupFilterUtil.h>

//-----------------------------------------------------------------------------------

vHavokRagdoll::vHavokRagdoll()
  : m_pPhysicsWorld(NULL)
  , m_physicsSystems()
  , m_rigidBodies()
  , m_unmappedBones()
  , m_iMappedRootBoneIdx(0)
  , m_fScaling(1.f)
  , m_bInitialized(false)
  , m_bAddedToPhysicsWorld(false)
  , m_spFinalSkeletalResultRagdoll(NULL)
  , m_spAnimConfig(NULL)
  , m_bEnabled(TRUE)
  , m_sFileResourceName()
  , m_bDebugRenderingEnabled(FALSE)
  , m_debugColor(0, 0, 0, 0)
{
}

vHavokRagdoll::~vHavokRagdoll()
{
  DeleteRagdoll();
}

//-----------------------------------------------------------------------------------

void vHavokRagdoll::SetOwner(VisTypedEngineObject_cl *pOwner)
{
  // Do not initialize the component in case our module is not active
  if (vHavokPhysicsModule::GetInstance() == NULL)
  {
    IVObjectComponent::SetOwner(pOwner);
    Vision::Error.Warning(
      "Failed to initialize vHavokRagdoll since no Havok Physics module is active");
    return;
  }

  // Create / Remove rag doll structure
  if (pOwner != NULL)
  {
    IVObjectComponent::SetOwner(pOwner);
    RecreateRagdoll();
  }
  else
  {
    DeleteRagdoll();
    IVObjectComponent::SetOwner(pOwner);
  }
}

void vHavokRagdoll::SetEnabled(BOOL bStatus)
{
  m_bEnabled = bStatus;

  if (bStatus)
  {
    if (IsInitialized())
      AddToPhysicsWorld();
    else
      CreateRagdoll(); // also adds the rag doll to the world 
  }
  else
  {
    RemoveFromPhysicsWorld();
  }
}

//-----------------------------------------------------------------------------------

void vHavokRagdoll::UpdateOwner()
{
  VASSERT(GetOwner() != NULL && IsInitialized() && 
    m_bAddedToPhysicsWorld && m_bEnabled);

  CopyRigidBodyTransformationToBones();
}

//-----------------------------------------------------------------------------------

BOOL vHavokRagdoll::CanAttachToObject(VisTypedEngineObject_cl *pObject, 
  VString &sErrorMsgOut)
{
  if (!IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut))
    return FALSE;

  // Define criteria here that allows the editor to attach this component to the 
  // passed object. In our example, the object should be derived from VisObject3D_cl 
  // to be positionable.
  BOOL bIsValidClass = pObject->IsOfType(V_RUNTIME_CLASS(VisBaseEntity_cl));
  if (!bIsValidClass)
  {
    sErrorMsgOut += 
      "Component can only be added to instances of VisBaseEntity_cl or derived classes.";
  }

  if ( !bIsValidClass)
    return FALSE;

  return TRUE;
}

void vHavokRagdoll::OnVariableValueChanged(VisVariable_cl *pVar, const char * value)
{
  // Enable / Disable
  if (strcmp(pVar->GetName(), "m_bEnabled") == 0)
  {
    SetEnabled(m_bEnabled);
  }
  else if (strcmp(pVar->GetName(), "m_sFileResourceName") == 0)
  {
    // Collision file has changed -> Recreate rag doll
    RecreateRagdoll();
  }
  else if (strcmp(pVar->GetName(), "m_bDebugRenderingEnabled") == 0)
  {
    SetDebugRendering(m_bDebugRenderingEnabled);
  }
  else if (strcmp(pVar->GetName(), "m_debugColor") == 0)
  {
    SetDebugColor(m_debugColor);
  }
}

void vHavokRagdoll::MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB)
{
  IVObjectComponent::MessageFunction(iID,iParamA,iParamB);

  if (iID == VIS_MSG_EDITOR_PROPERTYCHANGED)
  {
    const char *szPropertyName = reinterpret_cast<const char*>(iParamA);
    VisBaseEntity_cl* pOwnerEntity = static_cast<VisBaseEntity_cl*>(GetOwner());

    
    // Recreate the rag doll in case the scaling gets changed within vForge
    if (strcmp(szPropertyName, "Scaling") == 0 || strcmp(szPropertyName, "ModelFile") == 0)
      RecreateRagdoll();

    // Only check for these properties if the rigid bodies are added to the world
    if (!m_bAddedToPhysicsWorld)
      return;
    VASSERT(IsInitialized());

    // Reposition the rigid bodies if position / orientation changes
    if (strcmp(szPropertyName, "Position") == 0 || strcmp(szPropertyName, "Orientation") == 0)
      CopyBoneTransformationToRigidBodies(pOwnerEntity->GetAnimConfig());

    return;
  }
}

//-----------------------------------------------------------------------------------
// Serialization

// Register the class in the engine module so it is available for RTTI
V_IMPLEMENT_SERIAL(vHavokRagdoll, IVObjectComponent, 0, &g_vHavokModule);

void vHavokRagdoll::Serialize(VArchive &ar)
{
  char iLocalVersion = VHAVOKRAGDOLL_VERSION_CURRENT;
  IVObjectComponent::Serialize(ar);

  // Load
  if (ar.IsLoading())
  {
    char iFileVersion = 0;
    ar >> iFileVersion;
    VASSERT_MSG(iFileVersion <= iLocalVersion, "Invalid file version. Please re-export");

    // VHAVOKRAGDOLL_VERSION_0
    ar >> m_bEnabled;
    ar >> m_sFileResourceName;
    ar >> m_debugColor;
  } 
  // Save
  else
  {
    ar << iLocalVersion;

    // VHAVOKRIGIDBODY_VERSION_0
    ar << m_bEnabled;
    ar << m_sFileResourceName;
    ar << m_debugColor;
  }
}

void vHavokRagdoll::OnDeserializationCallback(const VSerializationContext &context)
{
  // We can finally create the rag doll now, since the owner entity is guaranteed
  // to be de-serialized completely.
  if (IsInitialized() && m_bEnabled)
  {
    AddToPhysicsWorld();
  }
}

//-----------------------------------------------------------------------------------

void vHavokRagdoll::CreateRagdoll()
{
  // Rag doll can only be created if an owner is present
  if (IsInitialized() || GetOwner() == NULL)
    return;

  VisBaseEntity_cl* pOwnerEntity = GetOwnerEntity();
  VDynamicMesh* pDynamicMesh = pOwnerEntity->GetMesh();
  if (!pDynamicMesh)
    return;

  if (pDynamicMesh->GetSkeleton() == NULL)
  {
    if (!VStringUtil::IsEmpty(pDynamicMesh->GetFilename()))
    {
      Vision::Error.Warning("Havok Ragdoll Component: The model '%s' has no skeleton.",
        pDynamicMesh->GetFilename());
    }
    else
    {
      Vision::Error.Warning("Havok Ragdoll Component: The model has no skeleton.");
    }
    return;
  }

  // Get the World to add the rigid bodies to.
  vHavokPhysicsModule* pPhysicsModule = static_cast<vHavokPhysicsModule*>(
    Vision::GetApplication()->GetPhysicsModule());
  VASSERT(pPhysicsModule);
  m_pPhysicsWorld = pPhysicsModule->GetPhysicsWorld();
  VASSERT(m_pPhysicsWorld != NULL);

  // Load the ragdoll hkt file
  if (!LoadHktFile())
  {
    DeleteRagdoll();
    return;
  }

  // Bake the scale before computing the transformation mapping
  if (!BakeScalingIntoRigidBodies())
  {
    DeleteRagdoll();
    return;
  }

  // Find corresponding bones for the rigid bodies
  if (!MapBonesToRigidBodies())
  {
    DeleteRagdoll();
    return;
  }

  // Compute transformation mapping between rigid bodies and bones
  if (!ComputeTransformMapping())
  {
    DeleteRagdoll();
    return;
  }

  m_bInitialized = true;

  if (m_bEnabled)
    AddToPhysicsWorld();
}

void vHavokRagdoll::DeleteRagdoll()
{
  // Do not check if ragdoll is initialized since
  // this function is also called if initialization fails.

  // Remove from physics module
  RemoveFromPhysicsWorld();
  m_pPhysicsWorld = NULL;

  // Release physics systems
  for (int iSystemIdx = 0; iSystemIdx < m_physicsSystems.getSize(); iSystemIdx++)
  {
    const hkpPhysicsSystem* pSystem = m_physicsSystems[iSystemIdx];
    pSystem->removeReference();
  }

  // clear rigid body list (referenced by the physics systems)
  m_rigidBodies.clear();
  m_physicsSystems.clear();

  m_iMappedRootBoneIdx = 0;
  m_fScaling = 1.f;
  m_bInitialized = false;
}

//-----------------------------------------------------------------------------------

void vHavokRagdoll::AddToPhysicsWorld()
{
  if (m_bAddedToPhysicsWorld)
    return;

  VASSERT(IsInitialized());
  VisBaseEntity_cl* pOwnerEntity = GetOwnerEntity();
  VDynamicMesh* pDynamicMesh = pOwnerEntity->GetMesh();
  VASSERT(pDynamicMesh != NULL);

  // Make sure there is an animation config
  if (pOwnerEntity->GetAnimConfig() == NULL)
  {
    VisAnimFinalSkeletalResult_cl* pRagdollAnimRes = NULL;
    m_spAnimConfig = VisAnimConfig_cl::CreateSkeletalConfig(pDynamicMesh, &pRagdollAnimRes);
    m_spFinalSkeletalResultRagdoll = pRagdollAnimRes;

    // Set the new rag doll animation config
    pOwnerEntity->SetAnimConfig(m_spAnimConfig);
  }
  else
  {
    // Re-use existing animation config and increase reference count.
    m_spAnimConfig = pOwnerEntity->GetAnimConfig();
    m_spFinalSkeletalResultRagdoll = m_spAnimConfig->GetFinalResult();
    if (m_spFinalSkeletalResultRagdoll == NULL)
    {
      Vision::Error.Warning("Havok Ragdoll Component: Animation config is incompatible.");
      return;
    }
  }

  // Set rigid body configuration
  CopyBoneTransformationToRigidBodies(m_spAnimConfig);

  // re-apply to bones (just for double-checking, remove for final version)
  //CopyRigidBodyTransformationToBones();

  // See if we have a character controller in order to apply velocity.
  vHavokCharacterController* pCharacterController = pOwnerEntity->Components().GetComponentOfType<vHavokCharacterController>();
  if (pCharacterController != NULL && pCharacterController->GetCharacterProxy() != NULL)
  {
    const hkVector4 vVelocityHk = pCharacterController->GetCharacterProxy()->getLinearVelocity();

    for (int iRigidBodyIdx = 0; iRigidBodyIdx < m_rigidBodies.getSize(); iRigidBodyIdx++)
    {
      m_rigidBodies[iRigidBodyIdx].pRigidBody->setLinearVelocity(vVelocityHk);
    }
  }

  // Add physics systems (i.e. rigid bodies) to world

  m_pPhysicsWorld->markForWrite();
  for (int iSystemIdx = 0; iSystemIdx < m_physicsSystems.getSize(); iSystemIdx++)
  {
	  for (int rb=0; rb < m_physicsSystems[iSystemIdx]->getRigidBodies().getSize(); rb++)
	  {
		  hkpRigidBody* body =  m_physicsSystems[iSystemIdx]->getRigidBodies()[rb];
		  // Compute filter info
		  body->setCollisionFilterInfo( hkpGroupFilter::setLayer( body->getCollisionFilterInfo(), vHavokPhysicsModule::HK_LAYER_COLLIDABLE_RAGDOLL ) );

		  // Set user data so we receive info about collisions
		  body->setUserData((hkUlong)vHavokUserDataPointerPair_t::CombineTypeAndPointer(this, V_USERDATA_OBJECT));
	  }

    m_pPhysicsWorld->addPhysicsSystem(m_physicsSystems[iSystemIdx]);
  }
  m_pPhysicsWorld->unmarkForWrite();
  m_bAddedToPhysicsWorld = true;

  vHavokPhysicsModule::GetInstance()->AddRagdoll(this);
}

void vHavokRagdoll::RemoveFromPhysicsWorld()
{
  if (!m_bAddedToPhysicsWorld)
    return;

  VisBaseEntity_cl* pOwnerEntity = GetOwnerEntity();

  vHavokPhysicsModule::GetInstance()->RemoveRagdoll(this);

  m_pPhysicsWorld->markForWrite();
  for (int iSystemIdx = 0; iSystemIdx < m_physicsSystems.getSize(); iSystemIdx++)
  {
    const hkpPhysicsSystem* pSystem = m_physicsSystems[iSystemIdx];
    m_pPhysicsWorld->removePhysicsSystem(pSystem);
  }
  m_pPhysicsWorld->unmarkForWrite();
  m_bAddedToPhysicsWorld = false;

  // Release animation config
  if (m_spAnimConfig != NULL)
  {
    // If owner is NULL, it must have been destroyed
    // If reference count is 2. Only the component and the entity are referencing it.
    if (pOwnerEntity != NULL && m_spAnimConfig.GetPtr()->GetRefCount() <= 2)
    {
      VASSERT(m_spAnimConfig.GetPtr()->GetRefCount() == 2);
      pOwnerEntity->SetAnimConfig(NULL);
    }
    m_spAnimConfig = NULL;
  }

  // Reset custom bone transformations
  m_spFinalSkeletalResultRagdoll->ResetCustomBones();
  m_spFinalSkeletalResultRagdoll = NULL;
}

//-----------------------------------------------------------------------------------

bool vHavokRagdoll::LoadHktFile()
{
  if (m_sFileResourceName.IsEmpty())
    return false; // not (yet) set
  
  // Load to heap so that each object can just lives with respect to its reference 
  // rather than the loaded stream.
  vHavokStreamReader *pStreamReader = new vHavokStreamReader(m_sFileResourceName.AsChar());
  if (!pStreamReader->isOk())
  {
    Vision::Error.Warning("Havok Physics Ragdoll Component: Could not open file: \"%s\"", 
      m_sFileResourceName.AsChar());
    pStreamReader->removeReference();
    return false;
  }

  hkBufferedStreamReader *pBufStreamReader = new hkBufferedStreamReader(pStreamReader);
  hkSerializeUtil::ErrorDetails errRes;
  hkObjectResource* pResource = hkSerializeUtil::loadOnHeap(
    pBufStreamReader, &errRes);
  pStreamReader->removeReference();
  pBufStreamReader->removeReference();

  if (pResource == NULL)
  {
    if (errRes.id != hkSerializeUtil::ErrorDetails::ERRORID_NONE)
    {
      Vision::Error.Warning("Havok Physics Ragdoll Component: Could not load \"%s\": %s",
        m_sFileResourceName.AsChar(), errRes.defaultMessage.cString());
    }
    else
    {
      Vision::Error.Warning("Havok Physics Ragdoll Component: Could not load \"%s\".",
        m_sFileResourceName.AsChar());
    }
    pResource->removeReference();
    return false;
  }

  // Store physics systems
  hkRootLevelContainer* pRootLevel = NULL;
  hkpPhysicsData* pPhysicsData = NULL;

  pRootLevel = static_cast<hkRootLevelContainer*>(
    pResource->getContentsPointer(hkRootLevelContainerClass.getName(), 
      hkBuiltinTypeRegistry::getInstance().getLoadedObjectRegistry()));
  if (pRootLevel != NULL)
  {
    pPhysicsData = static_cast<hkpPhysicsData*>(
      pRootLevel->findObjectByType(hkpPhysicsDataClass.getName()));
  }

  if (pRootLevel == NULL || pPhysicsData == NULL)
  {
    Vision::Error.Warning("Havok Physics Ragdoll Component: No physics data available in \"%s\".",
      m_sFileResourceName.AsChar());
    return false;
  }

  const hkArray<hkpPhysicsSystem*>& systems = pPhysicsData->getPhysicsSystems();
  VASSERT(m_physicsSystems.getSize() == 0);
  m_physicsSystems.setSize(systems.getSize());
  
  for (int iSystemIdx = 0; iSystemIdx < systems.getSize(); iSystemIdx++)
  {
    hkpPhysicsSystem* pSystem = systems[iSystemIdx];
    pSystem->addReference();
    m_physicsSystems[iSystemIdx] = pSystem;
  }

  // Anything not used in file will be freed up here, and all the rigid bodies etc. 
  // should have been separate allocations and are now referenced by this component.
  pResource->removeReference();

  return true;
}

//-----------------------------------------------------------------------------------

bool vHavokRagdoll::MapBonesToRigidBodies()
{
  {
    // Create string map: rigid bone name -> corresponding rigid body object
    hkStringMap<hkpRigidBody*> nameToBody;
    for (int iSystemIdx = 0; iSystemIdx < m_physicsSystems.getSize(); iSystemIdx++)
    {
      // Cache name mapping
      const hkArray< hkpRigidBody*>& rigidBodies = 
        m_physicsSystems[iSystemIdx]->getRigidBodies();
      for (int iRigidBodyIdx = 0; iRigidBodyIdx < rigidBodies.getSize(); iRigidBodyIdx++)
      {
        nameToBody.insert(rigidBodies[iRigidBodyIdx]->getName(), 
          rigidBodies[iRigidBodyIdx]);
      }
    }

    // For each bone look for a similar-named rigid body. 
    // In this simple case the rigid body name is the same as bone name, 
    // just without the part before type ':', but can be whatever you like.
    const VDynamicMesh* pDynamicMesh = static_cast<VisBaseEntity_cl*>(GetOwner())->GetMesh();
    VASSERT(pDynamicMesh);
    const VisSkeleton_cl* pSkeleton = pDynamicMesh->GetSkeleton();
    VASSERT(pSkeleton);

    int iNumBones = pSkeleton->GetBoneCount();
    for (int iBoneIdx = 0; iBoneIdx < iNumBones; iBoneIdx++)
    {
      VisSkeletalBone_cl* pBone = pSkeleton->GetBone(iBoneIdx);

      // find similar rigid body name
      const char* szBoneName = pBone->m_sBoneName.AsChar();
      const char* szRigidBodyName = NULL;

      // The Vision naming scheme
      int iNameIdx = hkString::indexOf(szBoneName, ':');
      if (iNameIdx < 0) // try the Ragdoll_ style too 
      {
        if ( hkString::strNcmp(szBoneName, "Ragdoll_", 8) == 0 )
          iNameIdx = 0;
      }
      if (iNameIdx < 0) // try the name 'as is' aswell
      {
        iNameIdx = 0;
      }
      
      if (iNameIdx >= 0)
      {
        if (iNameIdx > 0)
          szRigidBodyName = szBoneName + iNameIdx + 1;
        else 
          szRigidBodyName = szBoneName;

        if (!VStringUtil::IsEmpty(szRigidBodyName))
        {
          // Find rigid body with the same name
          hkpRigidBody* pRigidBody = nameToBody.getWithDefault(szRigidBodyName, HK_NULL);

          if (pRigidBody != HK_NULL)
          {
            // Create Info object
            RigidBodyInfo info;
            info.pRigidBody = pRigidBody;
            info.iBoneIdx = iBoneIdx;

            m_rigidBodies.pushBack(info);

            // No need to reference the rigid body here. 
            // This is managed by the physics system containing the rigid body.
          }
        }
      }
    }
  }

  // Try the HCT naming scheme when creating rigid bodies for rag dolls
  if (m_rigidBodies.isEmpty())
  {
    const VDynamicMesh* pDynamicMesh = static_cast<VisBaseEntity_cl*>(GetOwner())->GetMesh();
    VASSERT(pDynamicMesh);
    const VisSkeleton_cl* pSkeleton = pDynamicMesh->GetSkeleton();
    VASSERT(pSkeleton);

    // Create string map: bone name -> corresponding bone object
    hkStringMap<int> nameToBoneIdx;

    for (int iBoneIdx = 0; iBoneIdx < pSkeleton->GetBoneCount(); iBoneIdx++)
    {
      VisSkeletalBone_cl* pBone = pSkeleton->GetBone(iBoneIdx);
      nameToBoneIdx.insert(pBone->m_sBoneName, iBoneIdx);
    }

    // For each rigid body look for a similar-named bone. 
    VMemoryTempBuffer<64> tmpBuffer;
    for (int iSystemIdx = 0; iSystemIdx < m_physicsSystems.getSize(); iSystemIdx++)
    {
      // Cache name mapping
      const hkArray<hkpRigidBody*>& rigidBodies = 
        m_physicsSystems[iSystemIdx]->getRigidBodies();

      for (int iRigidBodyIdx = 0; iRigidBodyIdx < rigidBodies.getSize(); iRigidBodyIdx++)
      {
        hkpRigidBody* pRigidBody = rigidBodies[iRigidBodyIdx];

        // Parse bone name
        const char szHCTPrefix[] = "Ragdoll_";
        const size_t iHCTPrefixLen = sizeof(szHCTPrefix) - 1;
        const char* szRigidBodyName = pRigidBody->getName();

        if (strncmp(szRigidBodyName, szHCTPrefix, iHCTPrefixLen) != 0)
          continue;

        const int iMaxLen = static_cast<int>(strlen(szRigidBodyName + iHCTPrefixLen));
        tmpBuffer.EnsureCapacity(iMaxLen + 1);
        char* szBoneName = static_cast<char*>(tmpBuffer.GetBuffer());
        strcpy_s(szBoneName, tmpBuffer.GetMemSize(), szRigidBodyName + iHCTPrefixLen);

        int iEndIdx = iMaxLen - 1;
        while (iEndIdx >= 0)
        {
          if (szBoneName[iEndIdx] < '0' || szBoneName[iEndIdx] > '9')
            break;
          iEndIdx--;
        }
        szBoneName[iEndIdx+1] = '\0';

        int iBoneIdx = nameToBoneIdx.getWithDefault(szBoneName, -1);
        if (iBoneIdx >= 0)
        {
          // Create Info object
          struct RigidBodyInfo info;
          info.pRigidBody = pRigidBody;
          info.iBoneIdx = iBoneIdx;

          m_rigidBodies.pushBack(info);

          // No need to reference the rigid body here. 
          // This is managed by the physics system containing the rigid body.
        }
      }
    }
  }

  // Check if assignment was successful.
  if (m_rigidBodies.isEmpty())
  {
    Vision::Error.Warning("Havok Ragdoll Component: No bones matching the ragdoll description found.");
    return false;
  }

  // Override mapping for root rigid body: This should always map to the root bone
  // of the skeleton, otherwise not all bones will get transformed.
  // Thus we store the actual mapped root bone in m_iMappedRootBoneIdx.
  m_iMappedRootBoneIdx = m_rigidBodies[0].iBoneIdx;
  m_rigidBodies[0].iBoneIdx = 0;

  return true;
}

//-----------------------------------------------------------------------------------

bool vHavokRagdoll::ComputeTransformMapping()
{
  VASSERT(GetOwner() != NULL);
  VisBaseEntity_cl* pOwnerEntity = static_cast<VisBaseEntity_cl*>(GetOwner());
  VDynamicMesh* pDynamicMesh = pOwnerEntity->GetMesh();
  VASSERT(pDynamicMesh != NULL);
  const VisSkeleton_cl* pSkeleton = pDynamicMesh->GetSkeleton();
  VASSERT(pSkeleton != NULL);

  // Use original pose for bone transformations
  VisSkeletalAnimResult_cl tpose(pSkeleton);
  VisAnimFinalSkeletalResult_cl::OverwriteResultWithInitialPose(&tpose);

  // Get root bone transform
  hkvMat4 rootBoneObjectTransform; 
  GetObjectSpaceBone(m_iMappedRootBoneIdx, rootBoneObjectTransform, pSkeleton, &tpose);

  // Convert to Havok transform.
  hkTransform hkRootBoneObjectTransform;
  vHavokConversionUtils::VisMatVecToPhysTransform( rootBoneObjectTransform.getRotationalPart(), rootBoneObjectTransform.getTranslation(), hkRootBoneObjectTransform);

  // Compute inverse
  hkTransform hkRootBoneObjectTransformInv;
  hkRootBoneObjectTransformInv.setInverse(hkRootBoneObjectTransform);

  // We need a reference we know matches.
  // Assume first rigid body corresponds to the root bone, so any difference there 
  // is accounted for (Y -> Z for instance on Vision Maya export..).
  // rigid body transform
  const hkTransform& rootRbTransform = m_rigidBodies[0].pRigidBody->getTransform(); 
  // Compute inverse
  hkTransform rootRbTransformInv;
  rootRbTransformInv.setInverse(rootRbTransform);

  // Compute transform for bones or add to unmapped list
  const int iNumBones = pSkeleton->GetBoneCount();
  for (int iBoneIdx = 0; iBoneIdx < iNumBones; iBoneIdx++)
  {
    VASSERT_MSG(pSkeleton->GetBone(iBoneIdx)->m_iParentIndex < iBoneIdx,
      "Parent bones must always have a smaller bone index than their children.");

    int iRbIndex = GetRigidBodyIndexForBone(iBoneIdx);
    if (iRbIndex < 0)
    {
      // unmapped bones must always have a parent index
      if (pSkeleton->GetBone(iBoneIdx)->m_iParentIndex < 0)
      {
        Vision::Error.Warning("Havok Ragdoll Component: Unmapped bones which don't have a parent bone are not allowed.");
        return false;
      }

      // Add unmapped bone entry
      struct UnmappedBoneInfo info;
      info.iBoneIdx = iBoneIdx;
      info.localTranslation = tpose.GetBoneTranslation(iBoneIdx);
      info.localRotation = tpose.GetBoneRotation(iBoneIdx);

      m_unmappedBones.pushBack(info);
    }
    else
    {
      // Mapped bone found -> compute relative transformation
      hkpRigidBody* pRigidBody = m_rigidBodies[iRbIndex].pRigidBody;

      hkTransform rbTposInRootRbSpace; 
      rbTposInRootRbSpace.setMul(rootRbTransformInv, pRigidBody->getTransform());

      // Get bone transform in object space
      hkvMat4 boneMatObject; 
      GetObjectSpaceBone(iBoneIdx, boneMatObject, pSkeleton, &tpose);

      // Convert to Havok transform
      hkTransform hkTposeObject;
      vHavokConversionUtils::VisMatVecToPhysTransform(boneMatObject.getRotationalPart(), boneMatObject.getTranslation(), 
         hkTposeObject);

      // Make relative to t-pose
      hkTransform boneTposInRootBoneSpace; 
      boneTposInRootBoneSpace.setMul(hkRootBoneObjectTransformInv, hkTposeObject);

      // Make relative to rigid body in root space
      m_rigidBodies[iRbIndex].relTransform.setMulInverseMul(
        rbTposInRootRbSpace, boneTposInRootBoneSpace);
    }
  }

  return true;
}

//-----------------------------------------------------------------------------------

bool vHavokRagdoll::BakeScalingIntoRigidBodies()
{
  VASSERT(GetOwner() != NULL);
  VisBaseEntity_cl* pOwnerEntity = static_cast<VisBaseEntity_cl*>(GetOwner());
  VDynamicMesh* pDynamicMesh = pOwnerEntity->GetMesh();
  VASSERT(pDynamicMesh != NULL);
  const VisSkeleton_cl* pSkeleton = pDynamicMesh->GetSkeleton();
  VASSERT(pSkeleton != NULL);

  // Determine if we have to bake the scale 
  const float fScaleEpsilon = 0.0001f;
  const hkvVec3& vScaling = pOwnerEntity->GetScaling();
  const bool bHasScaling = !hkvMath::isFloatEqual(1.0f, vScaling[0], fScaleEpsilon);
  const float fScaling = vScaling[0];

  // Check scaling is uniform
  if (!hkvMath::isFloatEqual(fScaling, vScaling[1], fScaleEpsilon) ||
    !hkvMath::isFloatEqual(fScaling, vScaling[2], fScaleEpsilon))
  {
    Vision::Error.Warning("Havok Ragdoll Component: Only uniform entity scaling allowed.");
    return false;
  }
  else if (!bHasScaling)
  {
    return true;
  }

  // Set scaling now
  m_fScaling = fScaling;

  // Scale all physics systems. There is no need to scale with respect to the entity's origin
  // since the transform mapping will account for this.
  for (int iSystemIdx = 0; iSystemIdx < m_physicsSystems.getSize(); iSystemIdx++)
  {
    hkpPhysicsSystem* pSystem = m_physicsSystems[iSystemIdx];
    hkpSystemScalingUtility::scaleSystem(pSystem, fScaling);
  }

  return true;
}

//-----------------------------------------------------------------------------------

void vHavokRagdoll::CopyBoneTransformationToRigidBodies(VisAnimConfig_cl* pAnimConfig)
{
  VisBaseEntity_cl* pOwnerEntity = GetOwnerEntity();

  // Get current animation result
  VASSERT_MSG(pAnimConfig, "No animation config set");
  const VisSkeletalAnimResult_cl* pObjectSpacePose = pAnimConfig->GetCurrentObjectSpaceResult();

  const hkvMat4 entityTransform(pOwnerEntity->GetRotationMatrix(), pOwnerEntity->GetPosition());

  if (m_bAddedToPhysicsWorld)
    m_pPhysicsWorld->markForWrite();

  // Position the rigid bodies
  for (int iRigidBodyIdx = 0; iRigidBodyIdx < m_rigidBodies.getSize(); iRigidBodyIdx++)
  {
    const RigidBodyInfo& rbInfo = m_rigidBodies[iRigidBodyIdx];
    hkpRigidBody* pRigidBody = rbInfo.pRigidBody;
    int iBoneIdx = rbInfo.iBoneIdx;

    hkvMat4 boneObjTransform; 
    pObjectSpacePose->GetBoneTransformationMatrix(iBoneIdx, boneObjTransform);
    boneObjTransform.setTranslation(boneObjTransform.getTranslation() * m_fScaling);
    hkvMat4 boneWorldTransform = entityTransform.multiply(boneObjTransform);

    hkTransform boneHkWorldTransform;
    vHavokConversionUtils::VisMatVecToPhysTransform(boneWorldTransform.getRotationalPart(), boneWorldTransform.getTranslation(), 
       boneHkWorldTransform);
 
    // The inverse of rbInfo.relTransform only has to be computed when initializing 
    // the rigid body placement here.
    hkTransform rigidBodyHkWorldTransform; 
    rigidBodyHkWorldTransform.setMulMulInverse(boneHkWorldTransform, rbInfo.relTransform);
    pRigidBody->setTransform(rigidBodyHkWorldTransform);
  }

  if (m_bAddedToPhysicsWorld)
    m_pPhysicsWorld->unmarkForWrite();

  // Explicitly update debug rendering (if enabled).
  // This is needed if engine view of vForge is not animated.
  vHavokPhysicsModule* pPhysicsModule = static_cast<vHavokPhysicsModule*>(
    Vision::GetApplication()->GetPhysicsModule());
  if (m_bDebugRenderingEnabled || pPhysicsModule->m_bDebugRenderRagdolls)
    UpdateDebugGeometry();
}

//-----------------------------------------------------------------------------------

void vHavokRagdoll::CopyRigidBodyTransformationToBones()
{
  if (m_bAddedToPhysicsWorld)
    m_pPhysicsWorld->markForRead();

  // Root (just generally in same space as rag doll)
  const hkTransform& rootHkWorldTransform = m_rigidBodies[0].pRigidBody->getTransform();
  hkTransform rootHkWorldTransformInv; 
  rootHkWorldTransformInv.setInverse(rootHkWorldTransform);

  VisBaseEntity_cl* pOwnerEntity = static_cast<VisBaseEntity_cl*>(GetOwner());
  VDynamicMesh* pDynamicMesh = pOwnerEntity->GetMesh();
  VASSERT(pDynamicMesh);
  const VisSkeleton_cl* pSkeleton = pDynamicMesh->GetSkeleton();
  VASSERT(pSkeleton);

  // Assume the skeleton stored in a way that always references back along array.
  const float fInvScaling = 1.f / m_fScaling;
  for (int iRigidBodyIdx=0; iRigidBodyIdx < m_rigidBodies.getSize(); iRigidBodyIdx++)
  {
    const struct RigidBodyInfo& rbInfo = m_rigidBodies[iRigidBodyIdx];
    hkpRigidBody* pRigidBody = rbInfo.pRigidBody;
    int iBoneIdx = rbInfo.iBoneIdx;

    const hkTransform& rigidBodyHkWorldTransform = pRigidBody->getTransform();
    hkTransform boneHkWorldTransform; 
    boneHkWorldTransform.setMul(rigidBodyHkWorldTransform, rbInfo.relTransform); 

    hkTransform boneHkObjTransform; 
    boneHkObjTransform.setMul(rootHkWorldTransformInv, boneHkWorldTransform);
    hkQuaternion boneHkRot; 
    boneHkRot.set(boneHkObjTransform.getRotation());

    hkvQuat boneRot;
    vHavokConversionUtils::HkQuatToVisQuat(boneHkRot, boneRot);
	hkvVec3 boneTranslation;
	vHavokConversionUtils::PhysVecToVisVecWorld(boneHkObjTransform.getTranslation(), boneTranslation );
    boneTranslation *= fInvScaling;

    m_spFinalSkeletalResultRagdoll->SetCustomBoneRotation(
      iBoneIdx, boneRot, VIS_REPLACE_BONE | VIS_OBJECT_SPACE);
    m_spFinalSkeletalResultRagdoll->SetCustomBoneTranslation(
      iBoneIdx, boneTranslation, VIS_REPLACE_BONE | VIS_OBJECT_SPACE);
  }

  for (int iUnmappedIdx = 0; iUnmappedIdx < m_unmappedBones.getSize(); iUnmappedIdx++)
  {
    const struct UnmappedBoneInfo& boneInfo = m_unmappedBones[iUnmappedIdx];
    const int iBoneIdx = boneInfo.iBoneIdx;

    // VIS_REPLACE_BONE | VIS_LOCAL_SPACE doesn't seem to recompute invobj for some reason.
    // so we compute it ourselves here
    int parentSkelBoneIdx = pSkeleton->GetBone(iBoneIdx)->m_iParentIndex;
    VASSERT(parentSkelBoneIdx >= 0);

    // As the parent index is always less than the current index,
    // we have updated the parent already.
    hkvQuat parentRot;
    hkvVec3 parentTrans;
    m_spFinalSkeletalResultRagdoll->GetCustomBoneRotation(parentSkelBoneIdx, parentRot);
    m_spFinalSkeletalResultRagdoll->GetCustomBoneTranslation(parentSkelBoneIdx, parentTrans);  

    // create matrices for the parent and the relative bone
    hkvMat4 parentRotMat = parentRot.getAsMat4();
    parentRotMat.setTranslation(parentTrans);
    hkvMat4 boneRotMat = boneInfo.localRotation.getAsMat4();
    boneRotMat.setTranslation(boneInfo.localTranslation);

    // transform from parent to current bone
    parentRotMat = parentRotMat.multiply(boneRotMat);

    // set rotational and translational part on the bone
    parentRot.setFromMat3(parentRotMat.getRotationalPart()); 
    m_spFinalSkeletalResultRagdoll->SetCustomBoneRotation( 
      iBoneIdx, parentRot, VIS_REPLACE_BONE | VIS_OBJECT_SPACE);
    m_spFinalSkeletalResultRagdoll->SetCustomBoneTranslation(
      iBoneIdx, parentRotMat.getTranslation(), VIS_REPLACE_BONE | VIS_OBJECT_SPACE);  
  }

  if (m_bAddedToPhysicsWorld)
    m_pPhysicsWorld->unmarkForRead();

  // update the root transformation
  hkvVec3 rootPos;
  hkvMat3 rootRot;
  vHavokConversionUtils::PhysTransformToVisMatVecWorld(rootHkWorldTransform, rootRot, rootPos);

  pOwnerEntity->SetPosition(rootPos);
  pOwnerEntity->SetRotationMatrix(rootRot);
}

//-----------------------------------------------------------------------------------

int vHavokRagdoll::GetRigidBodyIndexForBone(int iBoneIdx) const
{
  for (int iRigidBodyIdx = 0; iRigidBodyIdx < m_rigidBodies.getSize(); iRigidBodyIdx++)
  {
    if (m_rigidBodies[iRigidBodyIdx].iBoneIdx == iBoneIdx)
      return iRigidBodyIdx;
  }
  return -1;
}

//-----------------------------------------------------------------------------------

void vHavokRagdoll::GetObjectSpaceBone(int iBoneIdx, 
  hkvMat4& boneObjectMatrix, const VisSkeleton_cl* pSkeleton, 
  VisSkeletalAnimResult_cl* pPose)
{
  pPose->GetBoneTransformationMatrix(iBoneIdx, boneObjectMatrix);
  int parentBoneIdx = pSkeleton->GetBone(iBoneIdx)->m_iParentIndex;

  while (parentBoneIdx >= 0)
  {
    hkvMat4 boneRotMatLocal;
    pPose->GetBoneTransformationMatrix(parentBoneIdx, boneRotMatLocal);
    boneObjectMatrix = boneRotMatLocal.multiply(boneObjectMatrix);

    parentBoneIdx = pSkeleton->GetBone(parentBoneIdx)->m_iParentIndex;
  }

  // apply scaling to the translation
  boneObjectMatrix.setTranslation(boneObjectMatrix.getTranslation() * m_fScaling);
}

//-----------------------------------------------------------------------------------

void vHavokRagdoll::SetDebugRendering(BOOL bEnable)
{
  m_bDebugRenderingEnabled = bEnable;

  if (!m_bAddedToPhysicsWorld)
    return;

  vHavokPhysicsModule* pPhysicsModule = static_cast<vHavokPhysicsModule*>(
    Vision::GetApplication()->GetPhysicsModule());
  if (pPhysicsModule == NULL)
    return;

  const bool bEnableDebug = 
    m_bDebugRenderingEnabled || pPhysicsModule->m_bDebugRenderRagdolls;

  // Ensure debug display handler is created
  if (bEnableDebug)
    pPhysicsModule->SetEnabledDebug(true);

  // Get display handler
  vHavokDisplayHandler* pDisplay = pPhysicsModule->GetHavokDisplayHandler();
  if (pDisplay == NULL)
    return;

  m_pPhysicsWorld->markForRead();
  for (int iRigidBodyIdx = 0; iRigidBodyIdx < m_rigidBodies.getSize(); iRigidBodyIdx++)
  {
    // Get ID (cast from collidable pointer as its is used for display geometry ID)
    hkUlong id = (hkUlong)m_rigidBodies[iRigidBodyIdx].pRigidBody->getCollidable();
    
    // Set display properties
    // Set debug color and visibility state
    pDisplay->SetVisible(id, bEnableDebug);
    pDisplay->SetColor(id, m_debugColor);

    const struct RigidBodyInfo rbInfo = m_rigidBodies[iRigidBodyIdx];

    // update rigid body debug draw transform 
    pDisplay->updateGeometry(rbInfo.pRigidBody->getTransform(), 
      (hkUlong)rbInfo.pRigidBody->getCollidable(), 0);

    // Do not attach to owner in order to let the display handler update 
    // the transformation every frame.
  }
  m_pPhysicsWorld->unmarkForRead();
}

void vHavokRagdoll::SetDebugColor(VColorRef color)
{
  m_debugColor = color;

  if (!m_bAddedToPhysicsWorld)
    return;

  vHavokPhysicsModule* pPhysicsModule = static_cast<vHavokPhysicsModule*>(
    Vision::GetApplication()->GetPhysicsModule());
  if (pPhysicsModule == NULL)
    return;

  // Get display handler
  vHavokDisplayHandler* pDisplay = pPhysicsModule->GetHavokDisplayHandler();
  if (pDisplay == NULL)
    return;

  m_pPhysicsWorld->markForRead();
  for (int iRigidBodyIdx = 0; iRigidBodyIdx < m_rigidBodies.getSize(); iRigidBodyIdx++)
  {
    // Get ID (cast from collidable pointer as its is used for display geometry ID)
    hkUlong id = (hkUlong)m_rigidBodies[iRigidBodyIdx].pRigidBody->getCollidable();

    // Set debug color
    pDisplay->SetColor(id, m_debugColor);
  }
  m_pPhysicsWorld->unmarkForRead();
}

void vHavokRagdoll::UpdateDebugGeometry() const
{
  if (!m_bAddedToPhysicsWorld)
    return;

  vHavokPhysicsModule* pPhysicsModule = static_cast<vHavokPhysicsModule*>(
    Vision::GetApplication()->GetPhysicsModule());
  if (pPhysicsModule == NULL)
    return;

  const bool bEnableDebug = 
    m_bDebugRenderingEnabled || pPhysicsModule->m_bDebugRenderRagdolls;
  if (!bEnableDebug || m_pPhysicsWorld == NULL)
    return;

  // Get display handler
  vHavokDisplayHandler* pDisplay = pPhysicsModule->GetHavokDisplayHandler();
  if (pDisplay == NULL)
    return;

  m_pPhysicsWorld->markForRead();
  for (int iRigidBodyIdx = 0; iRigidBodyIdx < m_rigidBodies.getSize(); iRigidBodyIdx++)
  {
    const struct RigidBodyInfo rbInfo = m_rigidBodies[iRigidBodyIdx];

    // update rigid body debug draw transform 
    pDisplay->updateGeometry(rbInfo.pRigidBody->getTransform(), 
      (hkUlong)rbInfo.pRigidBody->getCollidable(), 0);
  }
  m_pPhysicsWorld->unmarkForRead();
}

//-----------------------------------------------------------------------------------

int vHavokRagdoll::GetRigidBodyIndex(const char* szBodyName) const 
{
  int index = -1;
  m_pPhysicsWorld->markForRead();
  for (int iRigidBodyIdx = 0; iRigidBodyIdx < m_rigidBodies.getSize(); iRigidBodyIdx++)
  {
    const struct RigidBodyInfo rbInfo = m_rigidBodies[iRigidBodyIdx];
	if (strcmp( rbInfo.pRigidBody->getName(), szBodyName) == 0)
	{
		index =  iRigidBodyIdx;
		break;
    }
  }
  m_pPhysicsWorld->unmarkForRead();
  return index;
}

void vHavokRagdoll::ApplyForceToRigidBody(int iBoneIndex, const hkvVec3& value, float deltaT)
{
  vHavokPhysicsModule* pPhysicsModule = static_cast<vHavokPhysicsModule*>( Vision::GetApplication()->GetPhysicsModule());
	if (pPhysicsModule == NULL)
		return;

	if ((iBoneIndex >=0) && (iBoneIndex < m_rigidBodies.getSize()))
	{
	  hkVector4 force; vHavokConversionUtils::VisVecToPhysVecLocal(value,force);
	  pPhysicsModule->MarkForWrite();
	  m_rigidBodies[iBoneIndex].pRigidBody->applyForce(deltaT, force);
	  pPhysicsModule->UnmarkForWrite();
	}
}

void vHavokRagdoll::ApplyLinearImpulseToRigidBody(int iBoneIndex, const hkvVec3& value)
{
  vHavokPhysicsModule* pPhysicsModule = static_cast<vHavokPhysicsModule*>( Vision::GetApplication()->GetPhysicsModule());
  if (pPhysicsModule == NULL)
    return;

  if ((iBoneIndex >=0) && (iBoneIndex < m_rigidBodies.getSize()))
  {
    hkVector4 impulse; vHavokConversionUtils::VisVecToPhysVecLocal(value,impulse);
    pPhysicsModule->MarkForWrite();
    m_rigidBodies[iBoneIndex].pRigidBody->applyLinearImpulse(impulse);
    pPhysicsModule->UnmarkForWrite();
  }
}

hkvVec3 vHavokRagdoll::GetPositionOfRigidBody(int iBoneIndex) const
{	
	if ( (iBoneIndex >=0) && (iBoneIndex < m_rigidBodies.getSize()) )
	{
	  hkvVec3 p;
	  m_pPhysicsWorld->markForRead();
	  const hkVector4& rbPos= m_rigidBodies[iBoneIndex].pRigidBody->getPosition();
	  m_pPhysicsWorld->unmarkForRead();
	  vHavokConversionUtils::PhysVecToVisVecLocal(rbPos, p);
	  return p;
	}
	
	return hkvVec3(hkvNoInitialization);
}

//-----------------------------------------------------------------------------------

START_VAR_TABLE(vHavokRagdoll, IVObjectComponent, "Havok Physics Ragdoll component. Can be attached to entities to .", VVARIABLELIST_FLAGS_NONE, "Havok Ragdoll" )
  DEFINE_VAR_BOOL_AND_NAME(vHavokRagdoll, m_bEnabled, "Enabled", "Enable or disable component", "True", 0, 0);
  DEFINE_VAR_VSTRING_AND_NAME(vHavokRagdoll, m_sFileResourceName,	"Ragdoll Collision File", "Defines what Havok Physics collision file to use", "", 1024, 0, "filepicker(.hkt)");
  DEFINE_VAR_BOOL_AND_NAME(vHavokRagdoll, m_bDebugRenderingEnabled, "Debug", "Enables/Disables Physics Debug Rendering.", "False", 0, 0);
  DEFINE_VAR_COLORREF_AND_NAME(vHavokRagdoll, m_debugColor, "Debug Color", "Color of this Ragdoll when Debug Rendering is active.", "50,255,50,255", 0, 0);
END_VAR_TABLE

//-----------------------------------------------------------------------------------

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
