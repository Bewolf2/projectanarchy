/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokTriggerVolume.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokShapeFactory.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCachedShape.hpp>

#include <Physics2012/Utilities/CharacterControl/CharacterProxy/hkpCharacterProxy.h>
#include <Physics2012/Utilities/Collide/TriggerVolume/hkpTriggerVolume.h>
#include <Physics2012/Dynamics/Phantom/hkpShapePhantom.h>

// -------------------------------------------------------------------------- //
// vHavokTriggerVolumeInternal                                                //
// -------------------------------------------------------------------------- //

class vHavokTriggerVolumeInternal: public hkpTriggerVolume
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);

  vHavokTriggerVolumeInternal(hkpRigidBody *pTriggerBody, vHavokTriggerVolume *pOwner) : hkpTriggerVolume(pTriggerBody), m_pOwner(pOwner) {}

  // Bring in the other callback we are not overriding
  using hkpTriggerVolume::triggerEventCallback;

  virtual void triggerEventCallback(hkpRigidBody *pBody, EventType type)
  {
    if (type & ENTERED_EVENT)
    {
      vHavokTriggerInfo triggerInfo;
      triggerInfo.m_pTriggerVolume = m_pOwner;
      triggerInfo.m_pTriggerSourceComponent = m_pOwner->m_spOnObjectEnter;
      triggerInfo.m_pObject = pBody;
      vHavokPhysicsModule::GetInstance()->OnVolumeTrigger(triggerInfo);
    }

    if (type & LEFT_EVENT)
    {
      vHavokTriggerInfo triggerInfo;
      triggerInfo.m_pTriggerVolume = m_pOwner;
      triggerInfo.m_pTriggerSourceComponent = m_pOwner->m_spOnObjectLeave;
      triggerInfo.m_pObject = pBody;
      vHavokPhysicsModule::GetInstance()->OnVolumeTrigger(triggerInfo);
    }
  }

  virtual void triggerEventCallback(hkpCharacterProxy *pProxy, EventType type)
  {
    if (type & ENTERED_EVENT)
    {
      vHavokTriggerInfo triggerInfo;
      triggerInfo.m_pTriggerVolume = m_pOwner;
      triggerInfo.m_pTriggerSourceComponent = m_pOwner->m_spOnCharacterEnter;
      triggerInfo.m_pObject = pProxy->getShapePhantom();
      vHavokPhysicsModule::GetInstance()->OnVolumeTrigger(triggerInfo);
    }

    if (type & LEFT_EVENT)
    {
      vHavokTriggerInfo triggerInfo;
      triggerInfo.m_pTriggerVolume = m_pOwner;
      triggerInfo.m_pTriggerSourceComponent = m_pOwner->m_spOnCharacterLeave;
      triggerInfo.m_pObject = pProxy->getShapePhantom();
      vHavokPhysicsModule::GetInstance()->OnVolumeTrigger(triggerInfo);
    }
  }

private:
  vHavokTriggerVolume *m_pOwner;
};


// -------------------------------------------------------------------------- //
// Constructor / Destructor                                                   //
// -------------------------------------------------------------------------- //

V_IMPLEMENT_SERIAL(vHavokTriggerVolume, IVObjectComponent, 0, &g_vHavokModule);

vHavokTriggerVolume::vHavokTriggerVolume(VHavokTriggerVolumeShapeType_e eShapeType)
{
  m_pTriggerVolume = NULL;
  m_pModule = vHavokPhysicsModule::GetInstance();
  m_szShapeCacheId = NULL;

  m_spOnObjectEnter = NULL;
  m_spOnObjectLeave = NULL;
  m_spOnCharacterEnter = NULL;
  m_spOnCharacterLeave = NULL;

  Havok_ShapeType = eShapeType;
  Havok_MotionType = VHavokTriggerVolumeMotionType_FIXED; 
  Havok_QualityType = VHavokTriggerVolumeQualityType_FIXED;
  Havok_CollisionLayer = 1;
  Havok_CollisionGroup = 0;
  Havok_SubSystemId = 0;
  Havok_SubSystemDontCollideWith = 0;

  Debug_Render = FALSE;
  Debug_Color = VColorRef(0, 0, 255, 255);
}

vHavokTriggerVolume::~vHavokTriggerVolume()
{
  CommonDeinit();
}

void vHavokTriggerVolume::DisposeObject()
{
  CommonDeinit();
  IVObjectComponent::DisposeObject();
}


// -------------------------------------------------------------------------- //
// Common Init/ Deint                                                         //
// -------------------------------------------------------------------------- //

void vHavokTriggerVolume::CommonInit()
{
  // Do not initialize the component in case our module is not active
  if (!m_pModule)
  {
    Vision::Error.Warning("Failed to initialize vHavokTriggerVolume since a non Havok physics module is currently active");
    return;
  }

  // Get owner custom volume object
  VCustomVolumeObject *pCustomVolume = GetOwnerCustomVolume();
  VVERIFY_OR_RET(pCustomVolume != NULL); 

  // Get the static mesh from the custom volume object
  VisStaticMesh_cl *pMesh = pCustomVolume->GetStaticMesh();
  if (pMesh)
  {
    // Check whether the static mesh is really there
    pMesh->EnsureLoaded();
    if (!pMesh->IsLoaded())
      pMesh = NULL;
  }

  // Get scaling vector from custom volume object
  hkvVec3 vScale = pCustomVolume->GetScale();

  if (pMesh)
  {  
    // Check whether the static mesh has enough vertices
    if (pMesh->GetNumOfVertices()<3)
      return;

    // Check whether the static mesh has valid size
	const hkvAlignedBBox& bbox = pMesh->GetBoundingBox();
	hkVector4 bbox_min; vHavokConversionUtils::VisVecToPhysVec_noscale(bbox.m_vMin, bbox_min);
	hkVector4 bbox_max; vHavokConversionUtils::VisVecToPhysVec_noscale(bbox.m_vMax, bbox_max);
	hkVector4 bbox_extent; bbox_extent.setSub(bbox_max,bbox_min); bbox_extent.mul(vHavokConversionUtils::GetVision2HavokScaleSIMD());

	hkVector4 meshTol; meshTol.setAll(hkReal((Havok_ShapeType==VHavokTriggerVolumeShapeType_CONVEX) ? HKVIS_CONVEX_SHAPE_TOLERANCE : HKVIS_MESH_SHAPE_TOLERANCE));
	hkVector4Comparison notLargeEnough = bbox_extent.less(meshTol);
	if (notLargeEnough.anyIsSet<hkVector4ComparisonMask::MASK_XYZ>())
    {
      Vision::Error.Warning("Initializing vHavokTriggerVolume with a static mesh with undersized extents (%.4f, %4f, %.4f)", 
                            bbox_extent(0), bbox_extent(1), bbox_extent(2));
      return;
    }
  }
  else 
  {
    Vision::Error.Warning("Initializing vHavokTriggerVolume with a NULL static mesh");
    return;
  }

  // Create shape, thereby the shape is not cached to file. It would not be a good idea, to cache the shape every time, when the user has modified 
  // the custom volume or has switched the custom static mesh. Instead the final shape will be cached at serialization. Only try using a collision 
  // mesh for creation if custom static mesh is used, since otherwise the static mesh is not physically present on disk.
  int iCreationFlags = (!Vision::Editor.IsInEditor()) ? vHavokShapeFactory::VShapeCreationFlags_CACHE_SHAPE : 0;
  iCreationFlags |= pCustomVolume->GetCustomStaticMesh() ? vHavokShapeFactory::VShapeCreationFlags_USE_VCOLMESH : 0;
  
  // Create Havok trigger volume 
  bool bResult = CreateHkTriggerVolume(pMesh, vScale, iCreationFlags);
  VASSERT_MSG(bResult, "Failed to create Havok Trigger Volume");

  // Set debug visibility 
  SetDebugRendering(Debug_Render);

  // Note, that the trigger components are only created here inside vForge. Serialization reads these components from the archive.
  if (!m_spOnObjectEnter)
  {
    m_spOnObjectEnter = new VisTriggerSourceComponent_cl(VHAVOKTRIGGERVOLUME_ONOBJECTENTER, VIS_OBJECTCOMPONENTFLAG_NONE);
    AddComponent(m_spOnObjectEnter);
  }
  if (!m_spOnObjectLeave)
  {
    m_spOnObjectLeave = new VisTriggerSourceComponent_cl(VHAVOKTRIGGERVOLUME_ONOBJECTLEAVE, VIS_OBJECTCOMPONENTFLAG_NONE);
    AddComponent(m_spOnObjectLeave);
  }
  if (!m_spOnCharacterEnter)
  {
    m_spOnCharacterEnter = new VisTriggerSourceComponent_cl(VHAVOKTRIGGERVOLUME_ONCHARACTERENTER, VIS_OBJECTCOMPONENTFLAG_NONE);
    AddComponent(m_spOnCharacterEnter);
  }
  if (!m_spOnCharacterLeave)
  {
    m_spOnCharacterLeave = new VisTriggerSourceComponent_cl(VHAVOKTRIGGERVOLUME_ONCHARACTERLEAVE, VIS_OBJECTCOMPONENTFLAG_NONE);
    AddComponent(m_spOnCharacterLeave);
  }
}

void vHavokTriggerVolume::CommonDeinit()
{
  RemoveHkTriggerVolume();
  m_spOnObjectEnter = NULL;
  m_spOnObjectLeave = NULL;
  m_spOnCharacterEnter = NULL;
  m_spOnCharacterLeave = NULL;
}

bool vHavokTriggerVolume::CreateHkTriggerVolume(VisStaticMesh_cl* pMesh, const hkvVec3& vScale, int iCreationFlags)
{
  VVERIFY_OR_RET_VAL(m_pModule != NULL  && pMesh != NULL, false);

  // Remove the old instance, if available
  if (m_pTriggerVolume)
  {
    ASSERT(m_pOwner); /// If there's no owner then the refcount will get 0 => crash
    RemoveHkTriggerVolume();
  }

  hkpRigidBodyCinfo cInfo;

  // Set motion type
  switch (Havok_MotionType)
  {
  case VHavokTriggerVolumeMotionType_FIXED:
    cInfo.m_motionType = hkpMotion::MOTION_FIXED;
    cInfo.m_qualityType = HK_COLLIDABLE_QUALITY_FIXED;
    break;

  case VHavokTriggerVolumeMotionType_KEYFRAMED: 
    cInfo.m_motionType = hkpMotion::MOTION_KEYFRAMED;
    cInfo.m_qualityType = HK_COLLIDABLE_QUALITY_KEYFRAMED;
    break;

  default: 
    {
      VASSERT_MSG(false, "Unsupported motion type!");
      return false;
    }
  }

  // OVERWRITES qualtiy type (See quality type settings done in the paragraph before this line)
  // Only do that for non motion fixed trigger volumes
  if (Havok_MotionType != VHavokTriggerVolumeMotionType_FIXED)
  {
    switch (Havok_QualityType)
    {
    case VHavokTriggerVolumeQualityType_AUTO:
      {
        // Nothing, as this property only overwrites automatic done settings one paragraph earlier
      }
      break;

    case VHavokTriggerVolumeQualityType_FIXED:
      cInfo.m_qualityType = HK_COLLIDABLE_QUALITY_FIXED;
      break;

    case VHavokTriggerVolumeQualityType_KEYFRAMED:
      cInfo.m_qualityType = HK_COLLIDABLE_QUALITY_KEYFRAMED;
      break;

    case VHavokTriggerVolumeQualityType_KEYFRAMED_REPORTING:
      cInfo.m_qualityType = HK_COLLIDABLE_QUALITY_KEYFRAMED_REPORTING;
      break;

    default: 
      {
        VASSERT_MSG(false, "Unsupported quality type!");
        return false;
      }
    }
  }

  hkRefPtr<hkpShape> spShape; 
  switch (Havok_ShapeType)
  {
  case VHavokTriggerVolumeShapeType_CONVEX:
    {
      spShape = vHavokShapeFactory::CreateConvexHullShapeFromMesh(pMesh, vScale, &m_szShapeCacheId, iCreationFlags);
    }
    break;

  case VHavokTriggerVolumeShapeType_MESH:
    {
      spShape = vHavokShapeFactory::CreateShapeFromMesh(pMesh, vScale, &m_szShapeCacheId, iCreationFlags);
    }
    break;

  default: 
    {
      VASSERT_MSG(false, "Unsupported shape type!");
      return false;
    }
  }

  cInfo.m_shape = spShape;
  VASSERT_MSG(cInfo.m_shape != NULL, "Failed to create shape for Havok Trigger Volume");

  // no offset applied
  cInfo.m_position.setZero();

  // calculate collision filter
  cInfo.m_collisionFilterInfo = hkpGroupFilter::calcFilterInfo(Havok_CollisionLayer, Havok_CollisionGroup, 
    Havok_SubSystemId, Havok_SubSystemDontCollideWith);

  // Create rigid body 
  hkpRigidBody *pRigidBody = new hkpRigidBody(cInfo);

  // Convert rigid body into trigger volume  
  m_pTriggerVolume = new vHavokTriggerVolumeInternal(pRigidBody, this);

  // If we already have an owner, then add the trigger volume to the Havok world
  if (GetOwner() != NULL)
  {
    UpdateVision2Havok();
    m_pModule->AddTriggerVolume(this);
  }

  return true;
}

void vHavokTriggerVolume::RemoveHkTriggerVolume()
{
  if (!m_pTriggerVolume)
    return;

  // If we still have an owner, then remove the trigger volume from the Havok world
  if (GetOwner() != NULL)
    m_pModule->RemoveTriggerVolume(this);

  m_pTriggerVolume->removeReference();
  m_pTriggerVolume->getTriggerBody()->removeReference();
  m_pTriggerVolume = NULL;

  // Remove shape from cache
  vHavokShapeFactory::RemoveShape(m_szShapeCacheId);
  m_szShapeCacheId = NULL;
}

void vHavokTriggerVolume::UpdateVision2Havok()
{
  VisObject3D_cl *pOwner3d = GetOwner3D();
  VVERIFY_OR_RET(m_pTriggerVolume && pOwner3d);

  // Get the transformation from Vision
  const hkvVec3& vPos = pOwner3d->GetPosition();
  const hkvMat3& vRot = pOwner3d->GetRotationMatrix();

  // Set the transformation in Havok
  hkTransform hkTfOut;
  vHavokConversionUtils::VisMatVecToPhysTransformWorld(vRot, vPos, hkTfOut);

  m_pModule->MarkForWrite();
  m_pTriggerVolume->getTriggerBody()->setTransform(hkTfOut); 
  m_pModule->UnmarkForWrite();
}


// -------------------------------------------------------------------------- //
// Property Functions                                                         //
// -------------------------------------------------------------------------- //

VisObject3D_cl *vHavokTriggerVolume::GetOwner3D()
{
  VisTypedEngineObject_cl *pOwner = GetOwner();
  if (pOwner == NULL)
    return NULL;

  VASSERT(pOwner->IsOfType(V_RUNTIME_CLASS(VisObject3D_cl)));
  return (VisObject3D_cl*)pOwner; 
}

VCustomVolumeObject *vHavokTriggerVolume::GetOwnerCustomVolume()
{
  VisTypedEngineObject_cl *pOwner = GetOwner();
  if (!pOwner || !pOwner->IsOfType(V_RUNTIME_CLASS(VCustomVolumeObject)))
    return NULL;

  return static_cast<VCustomVolumeObject*>(pOwner); 
}

void vHavokTriggerVolume::SetPosition(const hkvVec3& value)
{
  VVERIFY_OR_RET(m_pTriggerVolume);

  hkVector4 pos; vHavokConversionUtils::VisVecToPhysVecWorld(value,pos);
  m_pModule->MarkForWrite();
  m_pTriggerVolume->getTriggerBody()->setPosition(pos);
  m_pModule->UnmarkForWrite();
}

hkvVec3 vHavokTriggerVolume::GetPosition() const
{
  VVERIFY_OR_RET_VAL(m_pTriggerVolume, hkvVec3(hkvNoInitialization));

  hkvVec3 v; 
  m_pModule->MarkForRead();
  const hkVector4& pos = m_pTriggerVolume->getTriggerBody()->getPosition();   
  vHavokConversionUtils::PhysVecToVisVecWorld(pos,v);
  m_pModule->UnmarkForRead();
  return v;
}

void vHavokTriggerVolume::SetRotation(const hkvMat3& value)
{
  VVERIFY_OR_RET(m_pTriggerVolume);

  hkQuaternion hkQuatOut;
  vHavokConversionUtils::VisMatrixToHkQuat(value, hkQuatOut);

  m_pModule->MarkForWrite();
  m_pTriggerVolume->getTriggerBody()->setRotation(hkQuatOut);
  m_pModule->UnmarkForWrite();
}

hkvMat3 vHavokTriggerVolume::GetRotation() const 
{
  VVERIFY_OR_RET_VAL(m_pTriggerVolume, hkvMat3 ());

  hkvMat3 visMatrixOut;
  m_pModule->MarkForRead();
  const hkQuaternion& rot = m_pTriggerVolume->getTriggerBody()->getRotation();
  vHavokConversionUtils::HkQuatToVisMatrix(rot, visMatrixOut);
  m_pModule->UnmarkForRead();

  return visMatrixOut;
}

void vHavokTriggerVolume::SetMotionType(VHavokTriggerVolumeMotionType_e eMotionType)
{
  VVERIFY_OR_RET(m_pTriggerVolume);

  //initialize with default value
  hkpMotion::MotionType eHKMotionType = hkpMotion::MOTION_FIXED; 
 
  switch (eMotionType)
  {
  case VHavokTriggerVolumeMotionType_FIXED: 
    eHKMotionType = hkpMotion::MOTION_FIXED;
    break;

  case VHavokTriggerVolumeMotionType_KEYFRAMED: 
    eHKMotionType = hkpMotion::MOTION_KEYFRAMED;
    break;

  default:
    VASSERT_MSG(false, "Unsupported motion type!");
    return; 
  }

  m_pModule->MarkForWrite();   
  m_pTriggerVolume->getTriggerBody()->setMotionType(eHKMotionType);
  m_pModule->UnmarkForWrite();
  
  Havok_MotionType = eMotionType;
}

void vHavokTriggerVolume::SetQualityType(VHavokTriggerVolumeQualityType_e eQualityType)
{
  VVERIFY_OR_RET(m_pTriggerVolume);

  //initialize with default value
  hkpCollidableQualityType eHKQualityType = HK_COLLIDABLE_QUALITY_FIXED;

  switch (eQualityType)
  {
  case VHavokTriggerVolumeQualityType_AUTO:
    if (Havok_MotionType == VHavokTriggerVolumeQualityType_FIXED)
      eHKQualityType= HK_COLLIDABLE_QUALITY_FIXED;
    else
      eHKQualityType = HK_COLLIDABLE_QUALITY_KEYFRAMED;
    break;

  case VHavokTriggerVolumeQualityType_FIXED: 
    eHKQualityType = HK_COLLIDABLE_QUALITY_FIXED;
    break;

  case VHavokTriggerVolumeQualityType_KEYFRAMED: 
    eHKQualityType = HK_COLLIDABLE_QUALITY_KEYFRAMED;
    break;

  case VHavokTriggerVolumeQualityType_KEYFRAMED_REPORTING:
    eHKQualityType = HK_COLLIDABLE_QUALITY_KEYFRAMED_REPORTING;
    break;

  default:
    VASSERT_MSG(false, "Unsupported quality type!");
    return; 
  }

  m_pModule->MarkForWrite();   
  m_pTriggerVolume->getTriggerBody()->setQualityType(eHKQualityType);
  m_pModule->UnmarkForWrite();

  Havok_QualityType = eQualityType;
}

void vHavokTriggerVolume::SetCollisionInfo(int iLayer, int iGroup, int iSubsystem, int iSubsystemDontCollideWith)
{
  VVERIFY_OR_RET(m_pTriggerVolume);

  int iFilterInfo = hkpGroupFilter::calcFilterInfo(iLayer, iGroup, iSubsystem, iSubsystemDontCollideWith);

  m_pModule->MarkForWrite();
  hkpRigidBody *pRigidBody = m_pTriggerVolume->getTriggerBody();
  pRigidBody->setCollisionFilterInfo(iFilterInfo);
  hkpWorld* world = pRigidBody->getWorld();
  if (world)
    world->updateCollisionFilterOnEntity(pRigidBody, HK_UPDATE_FILTER_ON_ENTITY_FULL_CHECK, HK_UPDATE_COLLECTION_FILTER_IGNORE_SHAPE_COLLECTIONS);
  m_pModule->UnmarkForWrite();

  Havok_CollisionLayer = iLayer;
  Havok_CollisionGroup = iGroup;
  Havok_SubSystemId = iSubsystem;
  Havok_SubSystemDontCollideWith = iSubsystemDontCollideWith;
}


// -------------------------------------------------------------------------- //
// Access to Havok Internals                                                  //
// -------------------------------------------------------------------------- //

hkpRigidBody* vHavokTriggerVolume::GetHkTriggerBody()const
{ 
  VASSERT(m_pTriggerVolume != NULL);
  return m_pTriggerVolume->getTriggerBody(); 
} 


// -------------------------------------------------------------------------- //
// Debug Rendering                                                            //
// -------------------------------------------------------------------------- //

void vHavokTriggerVolume::SetDebugRendering(BOOL bEnable)
{
  VVERIFY_OR_RET(m_pTriggerVolume);

  Debug_Render = bEnable;

  vHavokPhysicsModule* pInstance = vHavokPhysicsModule::GetInstance();

  const bool bEnableDebug = Debug_Render || (pInstance ? pInstance->m_bDebugRenderTriggerVolumes : false);

  // Get ID (cast from collidable pointer as its is used for display geometry ID)
  hkpWorld* world = pInstance ? pInstance->GetPhysicsWorld() : HK_NULL;
  if (world) 
    world->markForRead();
  hkUlong id = (hkUlong)m_pTriggerVolume->getTriggerBody()->getCollidable();
  if (world) 
    world->unmarkForRead();

  // Ensure debug display handler is created
  if (bEnableDebug)
    ((vHavokPhysicsModule*) Vision::GetApplication()->GetPhysicsModule())->SetEnabledDebug(true);

  // Set display properties
  vHavokDisplayHandler* pDisplay = ((vHavokPhysicsModule*) Vision::GetApplication()->GetPhysicsModule())->GetHavokDisplayHandler();
  if (!pDisplay)
    return;

  // Set debug color and visibility state
  pDisplay->SetVisible(id, bEnableDebug);
  pDisplay->SetColor(id, Debug_Color);

  // In Editor attach the Havok Debug Shape to the owner object
  if (bEnableDebug)
    pDisplay->SetOwner(id, GetOwner3D());
}

void vHavokTriggerVolume::SetDebugColor(VColorRef color)
{
  VVERIFY_OR_RET(m_pTriggerVolume);

  Debug_Color = color;

  // Get ID (cast from collidable pointer as its is used for display geometry ID)
  hkpWorld* world = vHavokPhysicsModule::GetInstance()? vHavokPhysicsModule::GetInstance()->GetPhysicsWorld() : HK_NULL;
  if (world) 
    world->markForRead();
  hkUlong id = (hkUlong)m_pTriggerVolume->getTriggerBody()->getCollidable();
  if (world)
    world->unmarkForRead();

  vHavokDisplayHandler* pDisplay = ((vHavokPhysicsModule*) Vision::GetApplication()->GetPhysicsModule())->GetHavokDisplayHandler();
  if (!pDisplay)
    return;

  // Set debug color
  pDisplay->SetColor(id, Debug_Color);
}


// -------------------------------------------------------------------------- //
// IVObjectComponent Virtual Overrides                                        //
// -------------------------------------------------------------------------- //

BOOL vHavokTriggerVolume::CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
  if (!IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut))
    return FALSE;

  // Object has to be derived from VCustomVolumeObject 
  if ( pObject->IsOfType(V_RUNTIME_CLASS(VCustomVolumeObject)) != TRUE)
  {
    sErrorMsgOut = "Component can only be added to instances of VCustomVolumeObject or derived classes.";
    return FALSE;
  }

  return TRUE;
}

void vHavokTriggerVolume::SetOwner(VisTypedEngineObject_cl *pOwner)
{
  IVObjectComponent::SetOwner(pOwner);

  // Do not initialize the component in case our module is not active
  if (!m_pModule)
  {
    Vision::Error.Warning("Failed to initialize vHavokTriggerVolume since a non Havok physics module is currently active");
    return;
  }

  // Add or remove from manager according to whether we have an owner or not
  if (pOwner)
  {
    // Use matrix directly instead of euler angles when doing real physics, as Euler
    // angles are slower and less accurate. Before, however, ensure that the object's
    // rotation matrix is current.
    GetOwner3D()->EnsureCachedRotationMatrixValid();
    GetOwner3D()->SetUseEulerAngles(false); 

    // Add the trigger volume to the Havok physics world
    if (m_pTriggerVolume)
    {
      // Set initial object position in Havok
      UpdateVision2Havok();

      m_pModule->AddTriggerVolume(this);
    }
    else
    {
      CommonInit();
    }
  }
  else
  {
    // Remove the trigger volume from the Havok physics world
    if (m_pTriggerVolume)
      m_pModule->RemoveTriggerVolume(this);

    // Remove shape from cache
    vHavokShapeFactory::RemoveShape(m_szShapeCacheId);
    m_szShapeCacheId = NULL;
  }
}

void vHavokTriggerVolume::OnVariableValueChanged(VisVariable_cl *pVar, const char * value)
{
  // Do not touch the trigger volume in case our module is not active
  if (!m_pModule)
    return;

  if (!m_pTriggerVolume)
    return;

  CommonInit();
}

void vHavokTriggerVolume::MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB)
{
  IVObjectComponent::MessageFunction(iID,iParamA,iParamB);

  // Do not touch the trigger volume in case our module is not active
  if (!m_pModule)
    return;

  if (iID == VIS_MSG_EDITOR_PROPERTYCHANGED)
  {
    const char *szPropertyName = (const char *) iParamA;

    // Recreate the trigger volume in case the mesh gets changed within vForge
    if (_stricmp(szPropertyName, "Scaling")==0 ||  _stricmp(szPropertyName, "VolumeGeometry")==0 ||
        _stricmp(szPropertyName, "CustomStaticMesh")==0 ||  _stricmp(szPropertyName, "StaticMeshPath")==0)
      CommonInit();

    // Reposition the trigger volume in case the mesh gets positioned/ orientated within vForge
    if (m_pTriggerVolume)
    {
      if (_stricmp(szPropertyName, "Position")==0 || _stricmp(szPropertyName, "Orientation")==0)
        UpdateVision2Havok();
    }
  
    return;
  }
}

// -------------------------------------------------------------------------- //
// Serialization                                                              //
// -------------------------------------------------------------------------- //

void vHavokTriggerVolume::Serialize( VArchive &ar )
{
  char iLocalVersion = VHAVOKTRIGGERVOLUME_VERSION_CURRENT;
  IVObjectComponent::Serialize(ar);
  if (ar.IsLoading())
  {
    char iFileVersion = 0;
    ar >> iFileVersion;
    VASSERT_MSG(iFileVersion <= iLocalVersion, "Invalid file version. Please re-export");
  
    ar >> Havok_ShapeType;
    ar >> Havok_MotionType;
    ar >> Havok_QualityType;
    ar >> Havok_CollisionLayer;
    ar >> Havok_CollisionGroup;
    ar >> Havok_SubSystemId;
    ar >> Havok_SubSystemDontCollideWith;
    ar >> Debug_Color;

    VisTriggerSourceComponent_cl *pComp = NULL;
    ar >> pComp;m_spOnObjectEnter = pComp;
    ar >> pComp;m_spOnObjectLeave = pComp;
    ar >> pComp;m_spOnCharacterEnter = pComp;
    ar >> pComp;m_spOnCharacterLeave = pComp;
  } 
  else
  {
    ar << iLocalVersion;

    ar << Havok_ShapeType;
    ar << Havok_MotionType; 
    ar << Havok_QualityType;
    ar << Havok_CollisionLayer;
    ar << Havok_CollisionGroup;
    ar << Havok_SubSystemId;
    ar << Havok_SubSystemDontCollideWith;
    ar << Debug_Color;

    ar << m_spOnObjectEnter;
    ar << m_spOnObjectLeave;
    ar << m_spOnCharacterEnter;
    ar << m_spOnCharacterLeave;

    // Only cache shape to HKT file when inside vForge.
    if (Vision::Editor.IsInEditor() && vHavokPhysicsModule_GetDefaultWorldRuntimeSettings().m_bEnableShapeCaching==TRUE)
    {
      // Get owner custom volume object 
      VCustomVolumeObject *pCustomVolume = GetOwnerCustomVolume();
      VASSERT(pCustomVolume != NULL);

      // Get the static mesh from the custom volume object
      VisStaticMesh_cl *pMesh = pCustomVolume->GetStaticMesh();
      VASSERT((pMesh != NULL)&&(pMesh->IsLoaded())) 

      // Get shape
      VASSERT(m_pTriggerVolume != NULL); 
      m_pModule->MarkForRead();  
      const hkpShape *pShape = m_pTriggerVolume->getTriggerBody()->getCollidable()->getShape();
      m_pModule->UnmarkForRead();

      // Cache shape to HKT file
      if (pShape->getClassType() == &hkvConvexVerticesShapeClass)
      {
        bool shrinkToFit = false; // would be better if true, but for back compat leave false
        vHavokCachedShape::SaveConvexShape(pMesh, pCustomVolume->GetScale(), shrinkToFit, (hkvConvexVerticesShape*)pShape);
      }
      else
      {
        vHavokCachedShape::SaveMeshShape(pMesh, pCustomVolume->GetScale(), 
          VisStaticMeshInstance_cl::VIS_COLLISION_BEHAVIOR_CUSTOM, VIS_WELDING_TYPE_NONE, (hkvBvCompressedMeshShape*)pShape);
      }
    }
  }
}

void vHavokTriggerVolume::OnDeserializationCallback(const VSerializationContext &context)
{
  // We can finally create the trigger volume now, since the owner entity is guaranteed to be deserialized completely.
  if (!m_pTriggerVolume)
    CommonInit();
}


// -------------------------------------------------------------------------- //
// Variable Attributes                                                        //
// -------------------------------------------------------------------------- //

START_VAR_TABLE(vHavokTriggerVolume, IVObjectComponent, "Havok Trigger Volume Component", VVARIABLELIST_FLAGS_NONE, "Havok Trigger Volume" )
  DEFINE_VAR_ENUM     (vHavokTriggerVolume, Havok_ShapeType, "Geometry used for Physics", "Convex Hull","Convex Hull/Mesh", 0, 0);
  DEFINE_VAR_ENUM     (vHavokTriggerVolume, Havok_MotionType, "Type of Physics:\nFixed - Collides but doesn't move\nKeyframed - Can be moved", "Fixed", "Fixed/Keyframed", 0, 0);
  DEFINE_VAR_ENUM     (vHavokTriggerVolume, Havok_QualityType, "Quality type of collidable. Does not affect fixed trigger volumes! Default: Automatic assignment by motion type", "Auto", "Auto/Fixed/Keyframed/Keyframed_Reporting", 0, 0);
  DEFINE_VAR_INT      (vHavokTriggerVolume, Havok_CollisionLayer, "Defines the collision layer this Trigger Volume is assigned to.", "1", 0,0);
  DEFINE_VAR_INT      (vHavokTriggerVolume, Havok_CollisionGroup, "Defines the collision group this Trigger Volume is assigned to.", "0", 0,0);
  DEFINE_VAR_INT      (vHavokTriggerVolume, Havok_SubSystemId, "Defines the sub system ID of this Trigger Volume.", "0", 0,0);
  DEFINE_VAR_INT      (vHavokTriggerVolume, Havok_SubSystemDontCollideWith, "Defines the sub system ID this Trigger Volume should not collide with.", "0", 0,0);
  DEFINE_VAR_BOOL     (vHavokTriggerVolume, Debug_Render, "Enables/Disables Physics Debug Rendering.", "FALSE", 0, 0);
  DEFINE_VAR_COLORREF (vHavokTriggerVolume, Debug_Color, "Color of this Trigger Volume when Debug Rendering is active.", "0,0,255,255", 0, NULL);
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
