/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokStaticMesh.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokUserData.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokShapeFactory.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>

#include <Common/Base/Reflection/Registry/hkVtableClassRegistry.h>


// --------------------------------------------------------------------------
// Constructor/Destructor
// --------------------------------------------------------------------------

void vHavokStaticMesh::SetDebugRendering (bool bEnable)
{
  vHavokPhysicsModule* pInstance = vHavokPhysicsModule::GetInstance();

  // Get ID (cast from collidable pointer as its is used for display geometry ID)
  hkpWorld* world = pInstance ? pInstance->GetPhysicsWorld() : HK_NULL;
  if (world) world->markForRead();

  const bool bEnabledDebug = bEnable || (pInstance ? pInstance->m_bDebugRenderStaticMeshes : false);

  hkUlong id = (hkUlong)m_pRigidBody->getCollidable();

  if (world) world->unmarkForRead();

  // Ensure debug display handler is created
  if (bEnabledDebug)
    ((vHavokPhysicsModule*) Vision::GetApplication()->GetPhysicsModule())->SetEnabledDebug(true);

  // Set display properties
  vHavokDisplayHandler* pDisplay = ((vHavokPhysicsModule*) Vision::GetApplication()->GetPhysicsModule())->GetHavokDisplayHandler();
  if (!pDisplay)
    return;

  // Set debug color and visibility state
  pDisplay->SetVisible(id, bEnabledDebug);
  pDisplay->SetColor(id, V_RGBA_PURPLE);
}


vHavokStaticMesh::vHavokStaticMesh(vHavokPhysicsModule &module)
  : m_pRigidBody(NULL), m_bInitialized(FALSE), m_module(module), m_iNumValidStaticMeshes(0), m_szShapeCacheId(NULL)
{
}

vHavokStaticMesh::~vHavokStaticMesh()
{
  CommonDeinit();
}

void vHavokStaticMesh::DisposeObject()
{
  CommonDeinit();
}

void vHavokStaticMesh::CommonDeinit()
{
  RemoveHkRigidBody();

  // Remove the link to this rigid body from the static meshes
  int iSMCount = m_staticMeshes.GetLength();
  for (int i = 0 ; i < iSMCount; i++)
  {
    if (m_staticMeshes[i])
	    m_staticMeshes[i]->SetPhysicsObject(NULL);
  }

  m_iNumValidStaticMeshes = 0;
}





// --------------------------------------------------------------------------
// Initialisation
// --------------------------------------------------------------------------

void vHavokStaticMesh::Init(VisStaticMeshInstance_cl &meshInstance)
{
  // Init may only be called once
  VVERIFY_OR_RET(m_bInitialized == false);

  // Add the static mesh instance
  m_staticMeshes.Append(&meshInstance);
  meshInstance.SetPhysicsObject(this);
  m_iNumValidStaticMeshes = 1;
  m_bInitialized = true;

  // Build the Havok shape
  CreateHkRigidBody();
}

void vHavokStaticMesh::Init(VisStaticMeshInstCollection &meshInstances)
{
  // Init may only be called once
  VVERIFY_OR_RET(m_bInitialized == false);

  // Add the static mesh instances
  int iCount = meshInstances.GetLength();
  for (int i = 0; i < iCount; i++)
  {
    m_staticMeshes.Append(meshInstances[i]);
    meshInstances[i]->SetPhysicsObject(this);
    m_iNumValidStaticMeshes++;
  }

  m_bInitialized = true;

  CreateHkRigidBody();
}

int vHavokStaticMesh::GetNumValidStaticMeshes() const
{
  return m_iNumValidStaticMeshes;
}

int vHavokStaticMesh::GetNumWrappedStaticMeshes()
{
  return m_staticMeshes.GetLength();
}

VisStaticMeshInstance_cl* vHavokStaticMesh::GetWrappedStaticMesh(int index)
{
  return m_staticMeshes[index];
}

void vHavokStaticMesh::RemoveStaticMesh(VisStaticMeshInstance_cl *pMesh)
{
  VVERIFY_OR_RET(pMesh != NULL);

  for (int i = 0; i < m_staticMeshes.GetLength(); ++i)
  {
    if (m_staticMeshes[i] != pMesh)
      continue;

    m_staticMeshes[i]->SetPhysicsObject(NULL);
    m_staticMeshes[i] = NULL;

    m_iNumValidStaticMeshes--;
    VASSERT(m_iNumValidStaticMeshes >= 0);

    break;
  }

  // Get rid of the Havok rigid body if there are no more meshes left.
  if (m_iNumValidStaticMeshes == 0) 
    RemoveHkRigidBody();
}

// --------------------------------------------------------------------------
// Havok Rigid Body Creation
// --------------------------------------------------------------------------

void vHavokStaticMesh::CreateHkRigidBody()
{
  // Create the Havok shape
  hkpRigidBodyCinfo cInfo;
  cInfo.m_motionType = hkpMotion::MOTION_FIXED;
  
  // We use the first static mesh instance as origin reference
  VisStaticMeshInstance_cl *pMeshInstance = m_staticMeshes[0];
  
  cInfo.m_collisionFilterInfo = pMeshInstance->GetCollisionBitmask();
  if (cInfo.m_collisionFilterInfo & (1<<15))
  {
    cInfo.m_collisionFilterInfo &= ~(1<<15);
    Vision::Error.Warning("vHavok: Static mesh [%s] has outdated collision information. Please reexport scene.", pMeshInstance->GetMesh()->GetFilename());
  }

  // Get the scaling of the first static mesh which is used as a reference
  {
	  hkMatrix4 mTransform;
	  vHavokConversionUtils::VisMatrixToHkMatrix(m_staticMeshes[0]->GetTransform(), mTransform, false, false, true);
	  hkVector4 vScale;
	  vScale.set(	mTransform.getColumn<0>().lengthSquared<3>(),
					mTransform.getColumn<1>().lengthSquared<3>(),
					mTransform.getColumn<2>().lengthSquared<3>(),
					hkSimdReal_1);
	  vScale.setSqrt(vScale);
	  vHavokConversionUtils::PhysVecToVisVec_noscale(vScale,m_vScale);
  }

  // Create the shape. 
  // We can either create the shape from mem, or serialize it in (if cached).
  // Do not set vHavokShapeFactory::VShapeCreationFlags_SHRINK, so back compat. Better to have as an option.
  const int iCreationFlags = (pMeshInstance->GetCollisionBehavior()==VisStaticMeshInstance_cl::VIS_COLLISION_BEHAVIOR_FROMFILE) ? 
                             vHavokShapeFactory::VShapeCreationFlags_ALLOW_PERTRICOLINFO : 0;
  hkRefPtr<hkpShape> spShape = vHavokShapeFactory::CreateShapeFromStaticMeshInstances(m_staticMeshes, iCreationFlags, &m_szShapeCacheId);
  cInfo.m_shape = spShape;

  // When CollisionBehavior_e::FromFile was selected and there is no collisionFilterInfo available from file (due to old vcolmesh format, convex shape),
  // a default collisionFilterInfo will be used.
  if (cInfo.m_collisionFilterInfo==0)
  {
    bool bHasMaterialCacheData = false;
    const hkpShape *pShape = (hkpShape*)spShape.val();
    const hkClass *pClass = pShape->getClassType();
    if (pClass == &hkvBvCompressedMeshShapeClass)
    {
      const hkvBvCompressedMeshShape *pMeshShape = (hkvBvCompressedMeshShape*)(pShape);
      bHasMaterialCacheData = pMeshShape->m_userData != HK_NULL;
    }
    if (!bHasMaterialCacheData)
      cInfo.m_collisionFilterInfo = vHavokPhysicsModule::HK_LAYER_COLLIDABLE_STATIC;
  }
  cInfo.m_numShapeKeysInContactPointProperties = -1; 	// Ensure shape keys are stored.
  m_pRigidBody = new hkpRigidBody(cInfo);

  // Set user data to identify this static mesh during collision detection
  m_pRigidBody->setUserData((hkUlong)vHavokUserDataPointerPair_t::CombineTypeAndPointer(this, V_USERDATA_STATIC));

  // Add our instance to the module
  UpdateVision2Havok();
  m_module.AddStaticMesh(this);
}

void vHavokStaticMesh::RemoveHkRigidBody()
{
  if (!m_pRigidBody)
    return;

  // Keep our object alive
  VSmartPtr<vHavokStaticMesh> keepAlive = this;

  // If we still have an owner, then remove the rigid body from the Havok world
  m_module.RemoveStaticMesh(this); 
  
  // Free the rigid body 
  m_pRigidBody->removeReference();
  m_pRigidBody = NULL;

  // Remove shape from cache 
  vHavokShapeFactory::RemoveShape(m_szShapeCacheId);
  m_szShapeCacheId = NULL;
}



// --------------------------------------------------------------------------
// Synchronization
// --------------------------------------------------------------------------

void vHavokStaticMesh::UpdateHavok2Vision()
{
  /// \todo: implement
  /*
  VisObject3D_cl *pOwner3d = GetOwner3d();
  VVERIFY_OR_RET(m_pRigidBody && GetOwner());

  // Update position
  hkvVec3 newPos = HK2VIS(m_pRigidBody->getPosition());
  pOwner3d->SetPosition(newPos);

  // Update rotation
  const hkQuaternion &hkRot = m_pRigidBody->getRotation();	
  hkvMat3	rotMatrix;
  vHavokConversionUtils::HkQuatToVisMatrix(hkRot, rotMatrix);  
  pOwner3d->SetRotationMatrix(rotMatrix);
  */
}

void vHavokStaticMesh::UpdateVision2Havok()
{
  VVERIFY_OR_RET(m_staticMeshes.GetLength() >= 1 && m_pRigidBody);

  int iCount = m_staticMeshes.GetLength();
  for (int i=0;i<iCount;i++)
  {
    // since collision mesh does not provide a bounding box, use bounding box of render mesh as approximation
    const hkvAlignedBBox& bbox = m_staticMeshes[i]->GetBoundingBox();
	hkVector4 bbox_min; vHavokConversionUtils::VisVecToPhysVec_noscale(bbox.m_vMin, bbox_min);
	hkVector4 bbox_max; vHavokConversionUtils::VisVecToPhysVec_noscale(bbox.m_vMax, bbox_max);
	hkVector4 bbox_extent; bbox_extent.setSub(bbox_max,bbox_min); bbox_extent.mul(vHavokConversionUtils::GetVision2HavokScaleSIMD());

	hkVector4 meshTol; meshTol.setAll(hkReal(HKVIS_MESH_SHAPE_TOLERANCE));
	hkVector4Comparison::Mask largeEnough = bbox_extent.greaterEqual(meshTol).getMask<hkVector4ComparisonMask::MASK_XYZ>();
	if (hkMath::countBitsSet(largeEnough) < 2)
    {
      Vision::Error.Warning("Attempted to create a vHavokStaticMesh with undersized extents (%.4f, %4f, %.4f)", 
							bbox_extent(0), bbox_extent(1), bbox_extent(2));
      return;
    }
  }

  // We use the first static mesh instance as origin reference
  VisStaticMeshInstance_cl *pMeshInstance = m_staticMeshes[0];

  // Get the static mesh instance position and rotation
  hkMatrix4 mTransform;
  vHavokConversionUtils::VisMatrixToHkMatrix(pMeshInstance->GetTransform(), mTransform, false, false, true);

  // Split the rotation into scale and normal matrix
  hkRotation mRotation; 
  mRotation.setCols(mTransform.getColumn<0>(),mTransform.getColumn<1>(),mTransform.getColumn<2>());
  hkVector4 vScale;
  vScale.set(mRotation.getColumn<0>().normalizeWithLength<3>(),
			 mRotation.getColumn<1>().normalizeWithLength<3>(),
			 mRotation.getColumn<2>().normalizeWithLength<3>(),
			 hkSimdReal_1);
  
  bool bUpdateDebugRendering = false;

  //Check here if we need to recalculate the precomputed collision mesh
  //should only happen inside the editor
  //if((vScale.x != m_vScale.x || vScale.y != m_vScale.y || vScale.z != m_vScale.z) && m_pRigidBody != NULL)
  hkVector4 mvScale; vHavokConversionUtils::VisVecToPhysVec_noscale(m_vScale,mvScale);
  if (!vScale.allEqual<3>(mvScale, hkSimdReal::fromFloat(HKVMATH_LARGE_EPSILON)) && (m_pRigidBody != NULL))
  {
    // Keep our object alive
    VSmartPtr<vHavokStaticMesh> keepAlive = this;

    RemoveHkRigidBody(); //Remove the old collision object
    CreateHkRigidBody(); //Create a new one (because the scale changed)
    VASSERT_MSG(m_pRigidBody != NULL, "Creating new rigid body failed");
	vHavokConversionUtils::PhysVecToVisVec_noscale(vScale, m_vScale);

    bUpdateDebugRendering = true;
  }

  // Set the transformation in Havok
  hkTransform hkTfOut;
  hkTfOut.setRotation(mRotation);
  hkTfOut.getTranslation().setMul(mTransform.getColumn<3>(),vHavokConversionUtils::GetVision2HavokScaleSIMD());
  m_pRigidBody->setTransform(hkTfOut);

  if (bUpdateDebugRendering)
  {
    SetDebugRendering (false);
  }
}




// --------------------------------------------------------------------------
// Accessing Havok Instances
// --------------------------------------------------------------------------

const hkpShape *vHavokStaticMesh::GetHkShape() const
{
  if (m_pRigidBody == NULL) 
    return NULL;

  return  m_pRigidBody->getCollidable()->getShape();
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
