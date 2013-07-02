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
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokGenericConstraint.hpp>

#include <Physics/Constraint/Motor/Position/hkpPositionConstraintMotor.h>
#include <Physics/Constraint/Motor/SpringDamper/hkpSpringDamperConstraintMotor.h>

#include <Physics/Constraint/Motor/Velocity/hkpVelocityConstraintMotor.h>

// ----------------------------------------------------------------------------
// vHavokGenericConstraintDesc
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL( vHavokGenericConstraintDesc, vHavokConstraintDesc, 0, &g_vHavokModule);

// ----------------------------------------------------------------------------
const unsigned int vHavokGenericConstraintDesc::s_iSerialVersion = 1;

// ----------------------------------------------------------------------------
vHavokGenericConstraintDesc::vHavokGenericConstraintDesc()
:vHavokConstraintDesc(false)
{
  Reset();
}

// ----------------------------------------------------------------------------
void vHavokGenericConstraintDesc::Reset()
{
  vHavokConstraintDesc::Reset();
  m_vGenericPivot = hkvVec3::ZeroVector ();
  m_vAngularAxis = hkvVec3(1.f, 0.f, 0.f);
  m_eXMotion = VGENERIC_MOTION_LOCKED;
  m_eYMotion = VGENERIC_MOTION_LOCKED;
  m_eZMotion = VGENERIC_MOTION_LOCKED;
  m_eAngularMotion = VGENERIC_MOTION_LOCKED;
  m_fLinearMin = -FLT_MAX;
  m_fLinearMax = FLT_MAX; 
  m_fAngularMin = -180.0f;      
  m_fAngularMax = 180.0f; 
  m_fVelocityTarget = 0.0f;
  m_fTau = 0.0f;
  m_fMinForce = 0.0f;
  m_fMaxForce = 0.0f;
  m_fLinearFriction = 0.0f;
  m_fAngularFriction = 0.0f;
}

// ----------------------------------------------------------------------------
void vHavokGenericConstraintDesc::Serialize( VArchive &ar )
{
  vHavokConstraintDesc::Serialize(ar);
  if (ar.IsLoading())
  {
    unsigned int iVersion = 0;
    ar >> iVersion;
    VASSERT((iVersion > 0) && (iVersion <= s_iSerialVersion));

    m_vGenericPivot.SerializeAsVisVector (ar);
    m_vAngularAxis.SerializeAsVisVector (ar);

    int iTmp;
    ar >> iTmp; m_eXMotion = (VGenericMotion_e)iTmp;
    ar >> iTmp; m_eYMotion = (VGenericMotion_e)iTmp;
    ar >> iTmp; m_eZMotion = (VGenericMotion_e)iTmp;
    ar >> iTmp; m_eAngularMotion = (VGenericMotion_e)iTmp;

    ar >> m_fLinearMin;
    ar >> m_fLinearMax;
    ar >> m_fAngularMin;
    ar >> m_fAngularMax;
    ar >> m_fVelocityTarget;
    ar >> m_fTau;
    ar >> m_fMinForce;
    ar >> m_fMaxForce;
    ar >> m_fLinearFriction;
    ar >> m_fAngularFriction;
  }
  else
  {
    ar << s_iSerialVersion;
    m_vGenericPivot.SerializeAsVisVector (ar);
    m_vAngularAxis.SerializeAsVisVector (ar);

    int iTmp;
    iTmp = m_eXMotion; ar << iTmp;
    iTmp = m_eYMotion; ar << iTmp;
    iTmp = m_eZMotion; ar << iTmp;
    iTmp = m_eAngularMotion; ar << iTmp;
    
    ar << m_fLinearMin;
    ar << m_fLinearMax;  
    ar << m_fAngularMin;
    ar << m_fAngularMax;
    ar << m_fVelocityTarget;
    ar << m_fTau;
    ar << m_fMinForce;
    ar << m_fMaxForce;
    ar << m_fLinearFriction;
    ar << m_fAngularFriction;
  }
}


// ----------------------------------------------------------------------------
// vHavokGenericConstraint
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL( vHavokGenericConstraint, vHavokConstraint, 0, &g_vHavokModule);

// ----------------------------------------------------------------------------
const unsigned int vHavokGenericConstraint::s_iSerialVersion = 1;

// ----------------------------------------------------------------------------
hkpConstraintData* vHavokGenericConstraint::CreateConstraintData()
{
  return new hkpGenericConstraintData();
}

// ----------------------------------------------------------------------------
vHavokConstraintDesc *vHavokGenericConstraint::CreateConstraintDesc()
{
  return new vHavokGenericConstraintDesc();
}

// ----------------------------------------------------------------------------
void vHavokGenericConstraint::SetLinearLimits(hkpConstraintConstructionKit &kit, const vHavokGenericConstraintDesc* pGenericDesc, VGenericAxis_e eAxis,
                                              vHavokGenericConstraintDesc::VGenericMotion_e eMotion)
{
  switch(eMotion)
  {
  case vHavokGenericConstraintDesc::VGENERIC_MOTION_LOCKED:
    kit.constrainLinearDof(eAxis);
    break;
  case vHavokGenericConstraintDesc::VGENERIC_MOTION_LIMITED:
    kit.setLinearLimit(eAxis, pGenericDesc->m_fLinearMin, pGenericDesc->m_fLinearMax);
    break;
  }
}

void vHavokGenericConstraint::SetAngularLimits(hkpConstraintConstructionKit &kit, const vHavokGenericConstraintDesc* pGenericDesc,
                                               vHavokGenericConstraintDesc::VGenericMotion_e eMotion)
{
  switch(eMotion)
  {
  case vHavokGenericConstraintDesc::VGENERIC_MOTION_LOCKED:
    kit.setAngularLimit(VGENERIC_AXIS_ANGULAR, 0.0f, 0.0f);
    break;
  case vHavokGenericConstraintDesc::VGENERIC_MOTION_LIMITED:
    kit.setAngularLimit(VGENERIC_AXIS_ANGULAR, hkvMath::Deg2Rad (pGenericDesc->m_fAngularMin), hkvMath::Deg2Rad (pGenericDesc->m_fAngularMax));
    break;
  }
}

void vHavokGenericConstraint::InitConstraintDataFromDesc(hkpConstraintData& data, vHavokConstraintDesc const& desc)
{
  vHavokConstraint::InitConstraintDataFromDesc(data, desc);

  hkpGenericConstraintData& genericData = static_cast<hkpGenericConstraintData&>(data);
  const vHavokGenericConstraintDesc* pGenericDesc = static_cast<const vHavokGenericConstraintDesc*>(&desc);

  // Please note: 
  // It is possible to specify 3 different angular axis, each forming a column in the angular basis matrices.
  // The angular axis indices, which have to be specified for instance for the "setAngularLimit" method, refer
  // to these columns.
  // Unfortunately, unlike the linear DOFs, there is no native possibility to constrain the DOFs away for each
  // angular axis. Furthermore trying to manually constrain the angular DOF by setting the angular limits for 
  // the corresponding axis to a very small value/ zero does not result in the expected behavior.
  // Therefore currently only one angular DOF is exposed to the user. 


  hkvMat3 mAxisRotation;
  mAxisRotation.setLookInDirectionMatrix(pGenericDesc->m_vAngularAxis);

  hkRotation hRotation; vHavokConversionUtils::VisMatrixToHkRotation( mAxisRotation, hRotation );
  hkVector4 swing1Axis; swing1Axis.setRotatedDir( hRotation, hkVector4::getConstant<HK_QUADREAL_0100>() ); 
  hkVector4 swing2Axis; swing2Axis.setRotatedDir( hRotation, hkVector4::getConstant<HK_QUADREAL_0010>() ); 
  hkVector4 angularAxis; vHavokConversionUtils::VisVecToPhysVec_noscale( pGenericDesc->m_vAngularAxis, angularAxis );
  hkVector4 pivot;
  hkvVec3d worldPivot; 

  if(!GetZonedPivot( pGenericDesc->m_vGenericPivot, worldPivot ))
  {  
	 vHavokConversionUtils::VisVecToPhysVecLocal(pGenericDesc->m_vGenericPivot, pivot);
  }
  else
  {
	 vHavokConversionUtils::VisVecToPhysVecLocal(worldPivot, pivot);
  }

  // calculate local pivot/ axis for bodies
  hkVector4 vLocalPivots[2];
  hkMatrix3 angularBasis[2];
	hkTransform transform;

	for (int i=0;i<2;i++)
  {
    VisBaseEntity_cl *pOwnerEntity = (pGenericDesc->m_spBodies[i]) ? pGenericDesc->m_spBodies[i]->GetOwnerEntity() : NULL;

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
		angularBasis[i].getColumn<0>().setRotatedDir(transform.getRotation(), angularAxis);
		angularBasis[i].getColumn<1>().setRotatedDir(transform.getRotation(), swing1Axis);
		angularBasis[i].getColumn<2>().setRotatedDir(transform.getRotation(), swing2Axis);
  }

  //  setup generic data
  hkpConstraintConstructionKit kit;
  kit.begin(&genericData);
  {
    // body A
		{
			kit.setPivotA(vLocalPivots[0]);
			kit.setAngularBasisA(angularBasis[0]);
		}

		{
			kit.setPivotB(vLocalPivots[1]);
			kit.setAngularBasisB(angularBasis[1]);
		}

    // set linear degree of freedom axis
    if (pGenericDesc->m_eXMotion != vHavokGenericConstraintDesc::VGENERIC_MOTION_UNCONSTRAINED)
	    kit.setLinearDofWorld(hkVector4::getConstant<HK_QUADREAL_1000>(), VGENERIC_AXIS_X); // X-axis
    if (pGenericDesc->m_eYMotion != vHavokGenericConstraintDesc::VGENERIC_MOTION_UNCONSTRAINED)
      kit.setLinearDofWorld(hkVector4::getConstant<HK_QUADREAL_0100>(), VGENERIC_AXIS_Y); // Y-axis
    if (pGenericDesc->m_eZMotion != vHavokGenericConstraintDesc::VGENERIC_MOTION_UNCONSTRAINED)
      kit.setLinearDofWorld(hkVector4::getConstant<HK_QUADREAL_0010>(), VGENERIC_AXIS_Z); // Z-axis

    // set angular degree of freedom axis
    if (pGenericDesc->m_eAngularMotion != vHavokGenericConstraintDesc::VGENERIC_MOTION_UNCONSTRAINED)
      kit.constrainToAngularDof(VGENERIC_AXIS_ANGULAR);

    // set limits 
    SetLinearLimits(kit, pGenericDesc, VGENERIC_AXIS_X, pGenericDesc->m_eXMotion); // X-axis
    SetLinearLimits(kit, pGenericDesc, VGENERIC_AXIS_Y, pGenericDesc->m_eYMotion); // Y-axis
    SetLinearLimits(kit, pGenericDesc, VGENERIC_AXIS_Z, pGenericDesc->m_eZMotion); // Z-axis
    SetAngularLimits(kit, pGenericDesc, pGenericDesc->m_eAngularMotion); // angular axis

    // set motors (all degrees of freedom use the same motor)
    // only create/ set motor, when target velocity of the motor is greater than zero
    if (pGenericDesc->m_fVelocityTarget > 0.0f)
    {
      hkpVelocityConstraintMotor *pMotor = new hkpVelocityConstraintMotor();
      pMotor->m_velocityTarget = pGenericDesc->m_fVelocityTarget;
      pMotor->m_tau = pGenericDesc->m_fTau;
      pMotor->setMinMaxForces(pGenericDesc->m_fMinForce, pGenericDesc->m_fMaxForce);  
      if (pGenericDesc->m_eXMotion != vHavokGenericConstraintDesc::VGENERIC_MOTION_UNCONSTRAINED)
        kit.setLinearMotor(VGENERIC_AXIS_X, pMotor); // X-axis
      if (pGenericDesc->m_eYMotion != vHavokGenericConstraintDesc::VGENERIC_MOTION_UNCONSTRAINED)
        kit.setLinearMotor(VGENERIC_AXIS_Y, pMotor); // Y-axis
      if (pGenericDesc->m_eZMotion != vHavokGenericConstraintDesc::VGENERIC_MOTION_UNCONSTRAINED)
        kit.setLinearMotor(VGENERIC_AXIS_Z, pMotor); // Z-axis
      if (pGenericDesc->m_eAngularMotion != vHavokGenericConstraintDesc::VGENERIC_MOTION_UNCONSTRAINED)
        kit.setAngularMotor(VGENERIC_AXIS_ANGULAR, pMotor); // angular axis
      pMotor->removeReference();
    }

    // set frictions
    // only set frictions, when greater than zero, in order to prevent solver issues
    if (pGenericDesc->m_fLinearFriction > 0.0f)
    {
      if (pGenericDesc->m_eXMotion != vHavokGenericConstraintDesc::VGENERIC_MOTION_UNCONSTRAINED)
        kit.setLinearFriction(VGENERIC_AXIS_X, pGenericDesc->m_fLinearFriction); // X-axis
      if (pGenericDesc->m_eYMotion != vHavokGenericConstraintDesc::VGENERIC_MOTION_UNCONSTRAINED)
        kit.setLinearFriction(VGENERIC_AXIS_Y, pGenericDesc->m_fLinearFriction); // Y-axis
      if (pGenericDesc->m_eZMotion != vHavokGenericConstraintDesc::VGENERIC_MOTION_UNCONSTRAINED)
        kit.setLinearFriction(VGENERIC_AXIS_Z, pGenericDesc->m_fLinearFriction); // Z-axis
    }
    if (pGenericDesc->m_fAngularFriction > 0.0f)
    {
      if (pGenericDesc->m_eAngularMotion != vHavokGenericConstraintDesc::VGENERIC_MOTION_UNCONSTRAINED)
        kit.setAngularFriction(VGENERIC_AXIS_ANGULAR, pGenericDesc->m_fAngularFriction); // angular axis
    }
  }
  kit.end();

  m_savedDesc = *pGenericDesc;
}

// ----------------------------------------------------------------------------
void vHavokGenericConstraint::SaveToDesc(vHavokConstraintDesc& desc)
{
  vHavokConstraint::SaveToDesc(desc);

  VVERIFY_OR_RET(desc.IsOfType(V_RUNTIME_CLASS(vHavokGenericConstraintDesc)));
  vHavokGenericConstraintDesc &genericDesc = static_cast<vHavokGenericConstraintDesc &>(desc);

  genericDesc = m_savedDesc;
}

// ----------------------------------------------------------------------------
void vHavokGenericConstraint::Serialize( VArchive &ar )
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
