/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraint.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>

#include <Physics/Constraint/Data/hkpConstraintData.h>
#include <Physics2012/Dynamics/Constraint/Breakable/hkpBreakableConstraintData.h>
#include <Physics2012/Dynamics/Constraint/Malleable/hkpMalleableConstraintData.h>

#include <Vision/Runtime/Engine/System/VisApiSerialization.hpp>

// ----------------------------------------------------------------------------
// vHavokConstraintDesc
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL( vHavokConstraintDesc, VTypedObject, 0, &g_vHavokModule);

// ----------------------------------------------------------------------------
const unsigned int vHavokConstraintDesc::s_iSerialVersion = 3;

// ----------------------------------------------------------------------------
vHavokConstraintDesc::vHavokConstraintDesc()
{
  Reset();
}

// ----------------------------------------------------------------------------
vHavokConstraintDesc::vHavokConstraintDesc(bool bDummyNoInit)
{
}

// ----------------------------------------------------------------------------
void vHavokConstraintDesc::Reset()
{
  m_spBodies[0] = NULL;
  m_spBodies[1] = NULL;
  m_vPivots[0] = hkvVec3(0, 0, 0);
  m_vPivots[1] = hkvVec3(0, 0, 0);

  m_bBreakable = false;
  m_fBreakThreshold = 1000.f;
  m_bRevertVelocityOnBreak = false;

  m_bMalleable = false;
  m_fConstraintStrength = 1.f;
}

// ----------------------------------------------------------------------------
void vHavokConstraintDesc::Serialize( VArchive &ar )
{
  if (ar.IsLoading())
  {
    Reset();

    unsigned int iVersion = 0;
    ar >> iVersion;
    VASSERT((iVersion > 0) && (iVersion <= s_iSerialVersion));

    vHavokRigidBody *pBody;
    if (iVersion < 2)
    {
      for (unsigned int i = 0; i < 2; ++i)
      {
        pBody = NULL;
        ar >> pBody;
        m_spBodies[i] = pBody;
      }
    }
    else
    {
      for (unsigned int i = 0; i < 2; ++i)
      {
        m_spBodies[i] = NULL;
        VisTypedEngineObject_cl *pParentObj = VisTypedEngineObject_cl::DeSerializeObjectReference(ar);

        if (VisBaseEntity_cl *pParentEntity = vdynamic_cast<VisBaseEntity_cl*>(pParentObj))
        {
          m_spBodies[i] = pParentEntity->Components().GetComponentOfBaseType<vHavokRigidBody>();
        }
      }
    }

    m_vPivots[0].SerializeAsVisVector (ar);
    m_vPivots[1].SerializeAsVisVector (ar);

		if( iVersion < 3 )
		{
			ar >> m_bBreakable;
			float constraint;
			ar >> constraint; m_fBreakThreshold = static_cast< hkReal >( constraint );
			ar >> m_bRevertVelocityOnBreak;

			ar >> m_bMalleable;
			ar >> constraint; m_fConstraintStrength = static_cast< hkReal >( constraint );
		}
		else
		{
			ar >> m_bBreakable;
			ar >> m_fBreakThreshold;
			ar >> m_bRevertVelocityOnBreak;

			ar >> m_bMalleable;
			ar >> m_fConstraintStrength;
		}
  }
  else
  {
    ar << s_iSerialVersion;

    for (unsigned int i = 0; i < 2; ++i)
    {
      if (m_spBodies[i])
        VisTypedEngineObject_cl::SerializeObjectReference(ar, m_spBodies[i]->GetOwner());
      else
        VisTypedEngineObject_cl::SerializeObjectReference(ar, NULL);
    }

    m_vPivots[0].SerializeAsVisVector (ar);
    m_vPivots[1].SerializeAsVisVector (ar);

    ar << m_bBreakable;
    ar << m_fBreakThreshold;
    ar << m_bRevertVelocityOnBreak;

    ar << m_bMalleable;
    ar << m_fConstraintStrength;
  }
}


// ----------------------------------------------------------------------------
// vHavokConstraint
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL( vHavokConstraint, VisTypedEngineObject_cl, 0, &g_vHavokModule);

template<> DynArray_cl<vHavokConstraint *> vHavokElementManager<vHavokConstraint *>::elementTable(0, NULL);
template<> unsigned int vHavokElementManager<vHavokConstraint *>::g_iElementCount = 0;
template<> unsigned int vHavokElementManager<vHavokConstraint *>::g_iFreeElementCache[VHAVOK_ELEMENTMANAGER_FREE_INDEX_CACHE_SIZE] = { 0 };
template<> unsigned int vHavokElementManager<vHavokConstraint *>::g_iNumElementsInCache = 0;
template<> unsigned int vHavokElementManager<vHavokConstraint *>::g_iCurrentElementCacheIndex = 0;
template<> unsigned int vHavokElementManager<vHavokConstraint *>::g_iResizeGranularity = 64;

// ----------------------------------------------------------------------------
const unsigned int vHavokConstraint::s_iSerialVersion = 2;

void vHavokConstraintListenerProxy::constraintDeletedCallback(hkpConstraintInstance* constraint)
{
	m_owner->constraintDeletedCallback( constraint ); 
}

// ----------------------------------------------------------------------------
vHavokConstraint::vHavokConstraint()
{	
  m_listener.setOwner(this);
  m_pBaseConstraintData = NULL;
  m_pBreakableData = NULL;
  m_pMalleableData = NULL;
  m_pFinalConstraintData = NULL;
  m_pConstraint = NULL;
  m_bDisposed = false;   
  m_bApplyCustomArchiveTransform = false;
  m_pSerializationDesc = NULL;
  AddToElementManager();

  m_pModule = vHavokPhysicsModule::GetInstance();
  VASSERT(m_pModule);
}

// ----------------------------------------------------------------------------
vHavokConstraint::~vHavokConstraint()
{
  DisposeObject();
  V_SAFE_DELETE(m_pSerializationDesc);
}

// ----------------------------------------------------------------------------
void vHavokConstraint::constraintDeletedCallback(hkpConstraintInstance* constraint)
{
  VVERIFY_OR_RET(m_pConstraint);
  
  m_pConstraint->removeConstraintListener(&m_listener);
  m_pConstraint->removeReference();
  m_pConstraint = NULL;
}

// ----------------------------------------------------------------------------
hkpConstraintData *vHavokConstraint::CreateConstraintData()
{
  VASSERT_MSG(FALSE, "This should never be called - override this method in subclasses!");
  return NULL;
}

// ----------------------------------------------------------------------------
vHavokConstraintDesc *vHavokConstraint::CreateConstraintDesc()
{
  VASSERT_MSG(FALSE, "This should never be called - override this method in subclasses!");
  return NULL;
}

// ----------------------------------------------------------------------------
void vHavokConstraint::DisposeObject()
{
  if (m_bDisposed)
    return;

  Reset();
  RemoveFromElementManager();
  m_bDisposed = true;
}

// ----------------------------------------------------------------------------
bool vHavokConstraint::Init(vHavokConstraintDesc &desc)
{
  VASSERT_MSG(!m_bDisposed, "Cannot initialize an already disposed constraint!");
  if (m_bDisposed)
    return false;

  Reset();

  // Create the constraint data for the requested constraint type, and let
  // the subclass fill in all data required for the specific constraint type.
  m_pBaseConstraintData = CreateConstraintData();
  VASSERT(m_pBaseConstraintData);
  if (!m_pBaseConstraintData)
    return false;
  InitConstraintDataFromDesc(*m_pBaseConstraintData, desc);
  m_pFinalConstraintData = m_pBaseConstraintData;

  // If the constraint is breakable, wrap the constraint data in the
  // corresponding descriptor.
  if (desc.m_bBreakable)
  {
    m_pBreakableData = new hkpBreakableConstraintData(m_pFinalConstraintData);
    m_pBreakableData->setThreshold(desc.m_fBreakThreshold);
    m_pBreakableData->setRevertBackVelocityOnBreak(desc.m_bRevertVelocityOnBreak);
    m_pFinalConstraintData = m_pBreakableData;
  }

  // If the constraint is malleable, wrap the constraint data in the
  // corresponding descriptor
  if (desc.m_bMalleable)
  {
    m_pMalleableData = new hkpMalleableConstraintData(m_pFinalConstraintData);
    m_pMalleableData->setStrength(desc.m_fConstraintStrength);
    m_pFinalConstraintData = m_pMalleableData;
  }

  // Make sure that at least one rigid body is given. If two are given, the constraint
  // is between two entities; if only one is given, the constraint is between one
  // entity and a fixed anchor. The managed part of the framework makes sure that in
  // this case the entity-based anchor comes first.
  hkpRigidBody *pHavokBody1 = desc.m_spBodies[0] ? desc.m_spBodies[0]->GetHkRigidBody() : NULL;
  hkpRigidBody *pHavokBody2 = desc.m_spBodies[1] ? desc.m_spBodies[1]->GetHkRigidBody() : NULL;
  VASSERT(pHavokBody1);
  if(pHavokBody1 == pHavokBody2) pHavokBody2 = NULL;	// If the user attached both anchors to the same entity just ignore the second one. In this case is a Entity-World constraint.
  if (!pHavokBody1)
  {
    Vision::Error.Warning("First entity has no Havok Rigid Body Component attached!");
    return false;
  }

  // Finally, create the constraint. All type distinction is made in the 
  // constraint data, thus all constraints instance have the same class
  // in Havok.
  m_pConstraint = new hkpConstraintInstance(pHavokBody1, pHavokBody2, m_pFinalConstraintData);
  m_pConstraint->setUserData((hkUlong)this);
  m_pConstraint->addConstraintListener(&m_listener);
  m_pModule->AddConstraint(this);
  
  OnEstablish();

  return true;
}

// ----------------------------------------------------------------------------
void vHavokConstraint::InitConstraintDataFromDesc(hkpConstraintData& data, vHavokConstraintDesc const& desc)
{
  for (unsigned int i = 0; i < 2; ++i)
    m_vSavedPivots[i] = desc.m_vPivots[i];
}

// ----------------------------------------------------------------------------
bool vHavokConstraint::IsBreakable() const
{
  return ((m_pConstraint != NULL) && (m_pBreakableData != NULL));
}

// ----------------------------------------------------------------------------
bool vHavokConstraint::IsBroken() const
{
  return ((m_pConstraint != NULL) 
    && (m_pBreakableData != NULL) 
    && m_pBreakableData->getIsBroken(m_pConstraint));
}

// ----------------------------------------------------------------------------
void vHavokConstraint::OnBreak(vHavokConstraintBreakInfo_t *pBreakInfo)
{
  VVERIFY_OR_RET(m_pConstraint);
  VASSERT(IsBreakable());

  vHavokRigidBody *pBodies[2] =
  {
    vHavokRigidBody::FromHkRigidBody(m_pConstraint->getRigidBodyA()),
    vHavokRigidBody::FromHkRigidBody(m_pConstraint->getRigidBodyB())
  };

  for (unsigned int i=0; i<2; i++)
  {
    if (pBodies[i])
    {
      VisTypedEngineObject_cl *pOwnerThis = (pBodies[i] != NULL) ? pBodies[i]->GetOwner() : NULL;
      VisTypedEngineObject_cl *pOwnerOther = (pBodies[1-i] != NULL) ? pBodies[1-i]->GetOwner() : NULL;

      if(pOwnerThis)
        Vision::Game.SendMsg(pOwnerThis, VIS_MSG_HAVOK_ONJOINTBREAK, (INT_PTR) pBreakInfo, (INT_PTR) pOwnerOther);
    }
  }
}

// ----------------------------------------------------------------------------
void vHavokConstraint::OnDeserializationCallback(const VSerializationContext &context)
{
  // The joint description must have been set by the specialized joint's deserialization method
  // If this assertion is triggered then the deserialization in the derived joint class
  // it not implemented correctly.
  VVERIFY_OR_RET(m_pSerializationDesc != NULL);
  
  // At least the first rigid body must be there - if it's missing, this constraint 
  // can't be created.
  VVERIFY_OR_RET(m_pSerializationDesc->m_spBodies[0] != NULL);

  // Apply custom transformation from archive (e.g. due to prefab instantiation)
  if (m_bApplyCustomArchiveTransform)
    ApplyCustomArchiveTransformationToJointDesc(m_pSerializationDesc, m_vCustomArchivePositionOfs, m_vCustomArchiveRotationOfs);

  // Initialize with the deserialized data - this creates the Havok constraint.
  Init(*m_pSerializationDesc);

  // Finally delete the joint description that was created during deserialization,
  // and release the rigid bodies we used for creating the constraint.
  V_SAFE_DELETE(m_pSerializationDesc);
}

// ----------------------------------------------------------------------------
void vHavokConstraint::OnEstablish()
{
  VVERIFY_OR_RET(m_pConstraint);

  vHavokRigidBody *pBodies[2] =
  {
    vHavokRigidBody::FromHkRigidBody(m_pConstraint->getRigidBodyA()),
    vHavokRigidBody::FromHkRigidBody(m_pConstraint->getRigidBodyB())
  };

  for (unsigned int i=0; i<2; i++)
  {
    if (pBodies[i])
    {
      VisTypedEngineObject_cl *pOwnerThis = (pBodies[i] != NULL) ? pBodies[i]->GetOwner() : NULL;
      VisTypedEngineObject_cl *pOwnerOther = (pBodies[1-i] != NULL) ? pBodies[1-i]->GetOwner() : NULL;

      if(pOwnerThis)
        Vision::Game.SendMsg(pOwnerThis, VIS_MSG_HAVOK_ONJOINTCREATE, (INT_PTR) pOwnerThis, (INT_PTR) pOwnerOther);
    }
  }
}

// ----------------------------------------------------------------------------
void vHavokConstraint::RemoveBrokenConstraints()
{
  ///\todo Decide whether invalid constraints (i.e., broken ones, ones with
  ///      no m_pConstraint, and ones with no m_pConstraint->getOwner()) 
  ///      shall be removed. If yes, implement this here.
}

// ----------------------------------------------------------------------------
void vHavokConstraint::Reset()
{
  if (m_pConstraint)
  {
    m_pConstraint->removeConstraintListener(&m_listener);
    m_pModule->RemoveConstraint(this);
    m_pConstraint->removeReference();
    m_pConstraint = NULL;
  }
  m_pFinalConstraintData = NULL;
  if (m_pMalleableData)
  {
    m_pMalleableData->removeReference();
    m_pMalleableData = NULL;
  }
  if (m_pBreakableData)
  {
    m_pBreakableData->removeReference();
    m_pBreakableData = NULL;
  }
  if (m_pBaseConstraintData)
  {
    m_pBaseConstraintData->removeReference();
    m_pBaseConstraintData = NULL;
  }
}

// ----------------------------------------------------------------------------
void vHavokConstraint::SaveToDesc(vHavokConstraintDesc& desc)
{
  desc.Reset();
  
  desc.m_spBodies[0] = vHavokRigidBody::FromHkRigidBody(m_pConstraint->getRigidBodyA());
  desc.m_spBodies[1] = vHavokRigidBody::FromHkRigidBody(m_pConstraint->getRigidBodyB());

  desc.m_vPivots[0] = m_vSavedPivots[0];
  desc.m_vPivots[1] = m_vSavedPivots[1];

  desc.m_bBreakable = (m_pBreakableData != NULL);
  if (desc.m_bBreakable)
  {
    desc.m_fBreakThreshold = m_pBreakableData->getThreshold();
    desc.m_bRevertVelocityOnBreak = m_pBreakableData->getRevertBackVelocityOnBreak();
  }

  desc.m_bMalleable = (m_pMalleableData != NULL);
  if (desc.m_bMalleable)
  {
    desc.m_fConstraintStrength = m_pMalleableData->getStrength();
  }
}

// ----------------------------------------------------------------------------
void vHavokConstraint::Serialize( VArchive &ar )
{
  if (ar.IsLoading())
  {
    unsigned int iVersion = 0;
    ar >> iVersion;
    VASSERT((iVersion > 0) && (iVersion <= s_iSerialVersion));

    V_SAFE_DELETE(m_pSerializationDesc);
    ar >> m_pSerializationDesc;
    VASSERT(m_pSerializationDesc);

    // Remember custom offset defined in the archive
    RememberCustomArchiveTransformation(ar);

    if (iVersion >= 2)
      SerializeKey(ar);
  }
  else
  {
    ar << s_iSerialVersion;

    // Save constraint data to the description, then serialize that.
    V_SAFE_DELETE(m_pSerializationDesc);
    m_pSerializationDesc = CreateConstraintDesc();
    VASSERT(m_pSerializationDesc);
    SaveToDesc(*m_pSerializationDesc);
    ar << m_pSerializationDesc;

    SerializeKey(ar);
  }
}

void vHavokConstraint::RememberCustomArchiveTransformation(VArchive &ar)
{
  hkvVec3 vOffset(hkvNoInitialization),vEulerOfs(hkvNoInitialization);
  hkvMat3 mRotationOfs(hkvNoInitialization);
  if (ar.GetCustomShapeTransformation(vOffset,mRotationOfs,vEulerOfs))
  {
    m_vCustomArchivePositionOfs = vOffset;
    m_vCustomArchiveRotationOfs = mRotationOfs;
    m_bApplyCustomArchiveTransform = true;
  }
}

bool vHavokConstraint::GetZonedPivot(hkvVec3 const& localPivot, hkvVec3d& worldPivot )
{
	if( GetParentZone() )
	{
		worldPivot = localPivot + GetParentZone()->m_vZonePivot;
		return true;
	}
	else
		worldPivot = localPivot;

	return false;
}

void vHavokConstraint::GetLocalPivot(hkvVec3d const& zonedPivot, hkvVec3& localPivot )
{
	if( GetParentZone() )
		localPivot = hkvVec3(zonedPivot - GetParentZone()->m_vZonePivot);
	else
		localPivot = hkvVec3(zonedPivot);
}

void vHavokConstraint::GetLocalPivot(hkVector4 const& zonedPivot, hkvVec3& localPivot )
{
	if( GetParentZone() )
	{
		hkvVec3d hkvZonedPivot; vHavokConversionUtils::PhysVecToVisVecLocal( zonedPivot, hkvZonedPivot );

		localPivot = hkvVec3(hkvZonedPivot - GetParentZone()->m_vZonePivot);
	}
	else
	{
		vHavokConversionUtils::PhysVecToVisVecLocal( zonedPivot, localPivot );
	}
}

void vHavokConstraint::ApplyCustomArchiveTransformationToJointDesc(vHavokConstraintDesc *pDesc, hkvVec3& vCustomArchivePositionOfs, hkvMat3 &vCustomArchiveRotationOfs)
{
  VVERIFY_OR_RET(m_bApplyCustomArchiveTransform);

  for (int i = 0; i < 2; i++)
  {
    // Apply the custom transformation to all static anchors not attached to an actor.
    // Static anchors have their position/rotations in world space
    if (pDesc->m_spBodies[i]==NULL)
    {
      hkvVec3 vTempLocalAnchor = pDesc->m_vPivots[i]; 
      vTempLocalAnchor = vCustomArchiveRotationOfs.transformDirection (vTempLocalAnchor);
      vTempLocalAnchor += vCustomArchivePositionOfs; 
      pDesc->m_vPivots[i] = vTempLocalAnchor;  
    }
  }
}

// ----------------------------------------------------------------------------
void vHavokConstraint::SetBreakThreshold(float fThreshold)
{
  if (!m_pBreakableData)
    return;
  m_pBreakableData->setThreshold(fThreshold);
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
