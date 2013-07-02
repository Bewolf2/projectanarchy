/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRagdollConstraint.hpp>

#include <Physics/Constraint/Data/Ragdoll/hkpRagdollConstraintData.h>
#include <Physics/Constraint/Motor/Position/hkpPositionConstraintMotor.h>
#include <Physics/Constraint/Motor/SpringDamper/hkpSpringDamperConstraintMotor.h>

// ----------------------------------------------------------------------------
// vHavokRagdollConstraintDesc
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL( vHavokRagdollConstraintDesc, vHavokConstraintDesc, 0, &g_vHavokModule);

// ----------------------------------------------------------------------------
const unsigned int vHavokRagdollConstraintDesc::s_iSerialVersion = 1;

// ----------------------------------------------------------------------------
vHavokRagdollConstraintDesc::vHavokRagdollConstraintDesc()
:vHavokConstraintDesc(false)
{
  Reset();
}

// ----------------------------------------------------------------------------
void vHavokRagdollConstraintDesc::Reset()
{
  vHavokConstraintDesc::Reset();

  m_vPivot = hkvVec3::ZeroVector ();
  m_vTwistAxis = hkvVec3(1.f, 0.f, 0.f);
  m_vPlaneAxis = hkvVec3(0.f, 1.f, 0.f);
  m_fConeAngle = hkvMath::pi () / 4.f;
  m_fPlaneConeMinAngle = -hkvMath::pi () / 4.f;
  m_fPlaneConeMaxAngle = hkvMath::pi () / 4.f;
  m_fTwistMinAngle = -hkvMath::pi ();
  m_fTwistMaxAngle = hkvMath::pi ();
  m_bConeLimitStabilization = false;
  m_fMaxFrictionTorque = 0.f;
  m_fAngularLimitsTauFactor = 0.8f;
  m_fConeAngle = hkvMath::pi ()*0.5f;
}

// ----------------------------------------------------------------------------
void vHavokRagdollConstraintDesc::Serialize( VArchive &ar )
{
  vHavokConstraintDesc::Serialize(ar);
  if (ar.IsLoading())
  {
    unsigned int iVersion = 0;
    ar >> iVersion;
    VASSERT((iVersion > 0) && (iVersion <= s_iSerialVersion));

    m_vPivot.SerializeAsVisVector (ar);
    m_vTwistAxis.SerializeAsVisVector (ar);
    m_vPlaneAxis.SerializeAsVisVector (ar);
    ar >> m_fConeAngle;
    ar >> m_fPlaneConeMinAngle;
    ar >> m_fPlaneConeMaxAngle;
    ar >> m_fTwistMinAngle;
    ar >> m_fTwistMaxAngle;
    ar >> m_bConeLimitStabilization;
    ar >> m_fMaxFrictionTorque;
    ar >> m_fAngularLimitsTauFactor;
  }
  else
  {
    ar << s_iSerialVersion;
    m_vPivot.SerializeAsVisVector (ar);
    m_vTwistAxis.SerializeAsVisVector (ar);
    m_vPlaneAxis.SerializeAsVisVector (ar);
    ar << m_fConeAngle;
    ar << m_fPlaneConeMinAngle;
    ar << m_fPlaneConeMaxAngle;
    ar << m_fTwistMinAngle;
    ar << m_fTwistMaxAngle;
    ar << m_bConeLimitStabilization;
    ar << m_fMaxFrictionTorque;
    ar << m_fAngularLimitsTauFactor;
  }
}


// ----------------------------------------------------------------------------
// vHavokRagdollConstraint
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL( vHavokRagdollConstraint, vHavokConstraint, 0, &g_vHavokModule);

// ----------------------------------------------------------------------------
const unsigned int vHavokRagdollConstraint::s_iSerialVersion = 1;

// ----------------------------------------------------------------------------
hkpConstraintData* vHavokRagdollConstraint::CreateConstraintData()
{
  return new hkpRagdollConstraintData();
}

// ----------------------------------------------------------------------------
vHavokConstraintDesc *vHavokRagdollConstraint::CreateConstraintDesc()
{
  return new vHavokRagdollConstraintDesc();
}

// ----------------------------------------------------------------------------
void vHavokRagdollConstraint::DisableMotor()
{
  VVERIFY_OR_RET(IsValid());

  hkpRagdollConstraintData *pRagdollData = 
    static_cast<hkpRagdollConstraintData*>(GetHkConstraintData());
  pRagdollData->setMotorsEnabled(GetHkConstraintInstance()->getRuntime(), false);
  pRagdollData->setConeMotor(HK_NULL);
  pRagdollData->setPlaneMotor(HK_NULL);
  pRagdollData->setTwistMotor(HK_NULL);
}

// ----------------------------------------------------------------------------
void vHavokRagdollConstraint::EnablePositionMotor(float fTau, float fDamping, 
  float fProportionalVelocity, float fConstantVelocity)
{
  VVERIFY_OR_RET(IsValid());

  hkpRagdollConstraintData *pRagdollData = 
    static_cast<hkpRagdollConstraintData*>(GetHkConstraintData());

  hkpPositionConstraintMotor *pMotor = new hkpPositionConstraintMotor;
  pMotor->m_tau = fTau;
  pMotor->m_damping = fDamping;
  pMotor->m_proportionalRecoveryVelocity = fProportionalVelocity;
  pMotor->m_constantRecoveryVelocity = fConstantVelocity;

  pRagdollData->setConeMotor(pMotor);
  pRagdollData->setPlaneMotor(pMotor);
  pRagdollData->setTwistMotor(pMotor);
  pMotor->removeReference();

  pRagdollData->setMotorsEnabled(GetHkConstraintInstance()->getRuntime(), true);
}

// ----------------------------------------------------------------------------
void vHavokRagdollConstraint::EnableSpringDamperMotor(
  float fSpringConstant, float fSpringDamping)
{
  VVERIFY_OR_RET(IsValid());

  hkpRagdollConstraintData *pRagdollData = 
    static_cast<hkpRagdollConstraintData*>(GetHkConstraintData());

  hkpSpringDamperConstraintMotor *pMotor = new hkpSpringDamperConstraintMotor;
  pMotor->m_springConstant = fSpringConstant;
  pMotor->m_springDamping = fSpringDamping;

  pRagdollData->setConeMotor(pMotor);
  pRagdollData->setPlaneMotor(pMotor);
  pRagdollData->setTwistMotor(pMotor);
  pMotor->removeReference();

  pRagdollData->setMotorsEnabled(GetHkConstraintInstance()->getRuntime(), true);
}

// ----------------------------------------------------------------------------
void vHavokRagdollConstraint::InitConstraintDataFromDesc(hkpConstraintData& data, vHavokConstraintDesc const& desc)
{
  vHavokConstraint::InitConstraintDataFromDesc(data, desc);

  hkpRagdollConstraintData& ragdollData =
    static_cast<hkpRagdollConstraintData&>(data);
  const vHavokRagdollConstraintDesc* pRagdollDesc = 
    static_cast<const vHavokRagdollConstraintDesc*>(&desc);
	
	hkVector4 pivot;
	hkvVec3d worldPivot; 

	if(!GetZonedPivot( pRagdollDesc->m_vPivot, worldPivot ))
	{  
		vHavokConversionUtils::VisVecToPhysVecLocal(pRagdollDesc->m_vPivot, pivot);
	}
	else
	{
		vHavokConversionUtils::VisVecToPhysVecLocal(worldPivot, pivot);
	}

	//Pivot is already in world space
	hkVector4 twistAxis; vHavokConversionUtils::VisVecToPhysVec_noscale(pRagdollDesc->m_vTwistAxis, twistAxis);
	hkVector4 planeAxis; vHavokConversionUtils::VisVecToPhysVec_noscale(pRagdollDesc->m_vPlaneAxis, planeAxis);
	hkTransform transform;

  hkVector4 vLocalPivots[2];
  hkVector4 vLocalTwistAxis[2];
  hkVector4 vLocalPlaneAxis[2];

  for (int i = 0; i < 2; ++i)
  {
    VisBaseEntity_cl *pOwnerEntity = (pRagdollDesc->m_spBodies[i])
      ? pRagdollDesc->m_spBodies[i]->GetOwnerEntity()
      : NULL;

    if (pOwnerEntity)
    {
			vHavokConversionUtils::VisMatVecToPhysTransformWorld( pOwnerEntity->GetRotationMatrix(), pOwnerEntity->GetPosition(), transform );
			transform.setInverse(transform);
		}
		else
		{
			transform.setIdentity();
		}

		vLocalPivots[i].setTransformedPos(transform, pivot);
    vLocalTwistAxis[i].setRotatedDir(transform.getRotation(), twistAxis );
    vLocalPlaneAxis[i].setRotatedDir(transform.getRotation(), planeAxis );
  }

  ragdollData.setInBodySpace(vLocalPivots[0],vLocalPivots[1],vLocalPlaneAxis[0],vLocalPlaneAxis[1],vLocalTwistAxis[0],vLocalTwistAxis[1]);
  ragdollData.setConeAngularLimit(pRagdollDesc->m_fConeAngle);
  ragdollData.setPlaneMinAngularLimit(pRagdollDesc->m_fPlaneConeMinAngle);
  ragdollData.setPlaneMaxAngularLimit(pRagdollDesc->m_fPlaneConeMaxAngle);
  ragdollData.setTwistMinAngularLimit(pRagdollDesc->m_fTwistMinAngle);
  ragdollData.setTwistMaxAngularLimit(pRagdollDesc->m_fTwistMaxAngle);
  ragdollData.setConeLimitStabilization(pRagdollDesc->m_bConeLimitStabilization);
  ragdollData.setMaxFrictionTorque(pRagdollDesc->m_fMaxFrictionTorque);
  ragdollData.setAngularLimitsTauFactor(pRagdollDesc->m_fAngularLimitsTauFactor);

  m_vSavedPivot = pRagdollDesc->m_vPivot;
  m_vSavedTwistAxis = pRagdollDesc->m_vTwistAxis;
  m_vSavedPlaneAxis = pRagdollDesc->m_vPlaneAxis;
}

// ----------------------------------------------------------------------------
void vHavokRagdollConstraint::SaveToDesc(vHavokConstraintDesc& desc)
{
  vHavokConstraint::SaveToDesc(desc);

  VVERIFY_OR_RET(desc.IsOfType(V_RUNTIME_CLASS(vHavokRagdollConstraintDesc)));
  vHavokRagdollConstraintDesc &ragdollDesc = static_cast<vHavokRagdollConstraintDesc &>(desc);
  hkpRagdollConstraintData *pRagdollData = 
    static_cast<hkpRagdollConstraintData*>(GetHkConstraintData());

  ragdollDesc.m_vPivot = m_vSavedPivot;
  ragdollDesc.m_vTwistAxis = m_vSavedTwistAxis;
  ragdollDesc.m_vPlaneAxis = m_vSavedPlaneAxis;

  if (pRagdollData)
  {
    ragdollDesc.m_fConeAngle = (float)pRagdollData->getConeAngularLimit();
    ragdollDesc.m_fPlaneConeMinAngle = (float)pRagdollData->getPlaneMinAngularLimit();
    ragdollDesc.m_fPlaneConeMaxAngle = (float)pRagdollData->getPlaneMaxAngularLimit();
    ragdollDesc.m_fTwistMinAngle = (float)pRagdollData->getTwistMinAngularLimit();
    ragdollDesc.m_fTwistMaxAngle = (float)pRagdollData->getTwistMaxAngularLimit();
    ragdollDesc.m_bConeLimitStabilization = pRagdollData->getConeLimitStabilization();
    ragdollDesc.m_fMaxFrictionTorque = (float)pRagdollData->getMaxFrictionTorque();
    ragdollDesc.m_fAngularLimitsTauFactor = (float)pRagdollData->getAngularLimitsTauFactor();
  }
}

// ----------------------------------------------------------------------------
void vHavokRagdollConstraint::Serialize( VArchive &ar )
{
  vHavokConstraint::Serialize(ar);
  if (ar.IsLoading())
  {
    unsigned int iVersion = 0;
    ar >> iVersion;
    VASSERT((iVersion > 0) && (iVersion <= s_iSerialVersion));
  }
  else
  {
    ar << s_iSerialVersion;
  }
}

// ----------------------------------------------------------------------------
void vHavokRagdollConstraint::SetMotorForces(float fMin, float fMax)
{
  VVERIFY_OR_RET(IsValid());

  hkpRagdollConstraintData *pRagdollData = 
    static_cast<hkpRagdollConstraintData*>(GetHkConstraintData());

  // The following assumes that all three degrees of freedom are driven by
  // the same motor instance.
  hkpLimitedForceConstraintMotor *pMotor = 
    static_cast<hkpLimitedForceConstraintMotor*>(pRagdollData->getConeMotor());
  if (!pMotor)
    return;

  pMotor->setMinMaxForces(fMin, fMax);
}

// ----------------------------------------------------------------------------
void vHavokRagdollConstraint::SetTargetOrientation(float fYaw, float fPitch, float fRoll)
{
  VVERIFY_OR_RET(IsValid());
  hkpRagdollConstraintData *pRagdollData = 
    static_cast<hkpRagdollConstraintData*>(GetHkConstraintData());

  // Check if any motor is set
  if (!pRagdollData->getConeMotor())
    return;

  hkvMat3 mRot (hkvNoInitialization);
  mRot.setFromEulerAngles (hkvMath::Rad2Deg (fRoll), hkvMath::Rad2Deg (fPitch), hkvMath::Rad2Deg (fYaw));

  hkRotation hkRot;
  vHavokConversionUtils::VisMatrixToHkRotation(mRot, hkRot);
  pRagdollData->setTarget(hkRot);

  // Wake up the simulation
  hkpRigidBody *pBody = GetHkConstraintInstance()->getRigidBodyA();
  hkpWorld *pWorld = vHavokPhysicsModule::GetInstance()->GetPhysicsWorld();
  if (pBody && pWorld)
  {
    pWorld->markForWrite();
    pBody->activate();
    pWorld->unmarkForWrite();
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
