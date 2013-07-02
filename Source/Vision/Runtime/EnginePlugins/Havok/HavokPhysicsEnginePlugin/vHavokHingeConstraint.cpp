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
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokHingeConstraint.hpp>

#include <Physics/Constraint/Data/LimitedHinge/hkpLimitedHingeConstraintData.h>

// ----------------------------------------------------------------------------
// vHavokHingeConstraintDesc
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL( vHavokHingeConstraintDesc, vHavokConstraintDesc, 0, &g_vHavokModule);

// ----------------------------------------------------------------------------
const unsigned int vHavokHingeConstraintDesc::s_iSerialVersion = 3;

// ----------------------------------------------------------------------------
vHavokHingeConstraintDesc::vHavokHingeConstraintDesc()
:vHavokConstraintDesc(false)
{
  Reset();
}

// ----------------------------------------------------------------------------
void vHavokHingeConstraintDesc::Reset()
{
  vHavokConstraintDesc::Reset();
  
  m_vHingePivot = hkvVec3::ZeroVector ();
  m_vRotAxis = hkvVec3(1.f, 0.f, 0.f);
  m_bUseLimits = false;
  m_fAngleMin = -180.f;
  m_fAngleMax = 180.f;
}

// ----------------------------------------------------------------------------
void vHavokHingeConstraintDesc::Serialize( VArchive &ar )
{
  vHavokConstraintDesc::Serialize(ar);
  if (ar.IsLoading())
  {
    unsigned int iVersion = 0;
    ar >> iVersion;
    VASSERT((iVersion > 0) && (iVersion <= s_iSerialVersion));

    // Version 2: uses explicit world-space pivot
    if (iVersion >= 2)
      m_vHingePivot.SerializeAsVisVector (ar);
    else
      m_vHingePivot = m_vPivots[0];

    m_vRotAxis.SerializeAsVisVector (ar);
    
    // Version 2: Rigid body transforms retrieved from rigid bodies when needed
    if (iVersion < 2)
    {
      hkvMat4 mDummy;
      ar >> mDummy;
      ar >> mDummy;
    }

    ar >> m_bUseLimits;

		if( iVersion < 3 )
		{
			float constraint;
			ar >> constraint; m_fAngleMin = constraint;
			ar >> constraint; m_fAngleMax = constraint;
		}
		else
		{
			ar >> m_fAngleMin;
			ar >> m_fAngleMax;
		}

    // Version 2: Angles are specified in degrees
    if (iVersion < 2)
    {
      m_fAngleMin = (m_fAngleMin * HK_REAL_RAD_TO_DEG);
      m_fAngleMax = (m_fAngleMax * HK_REAL_RAD_TO_DEG);
    }
  }
  else
  {
    ar << s_iSerialVersion;
    m_vHingePivot.SerializeAsVisVector (ar);
    m_vRotAxis.SerializeAsVisVector (ar);
    ar << m_bUseLimits;
    ar << m_fAngleMin;
    ar << m_fAngleMax;
  }
}


// ----------------------------------------------------------------------------
// vHavokHingeConstraint
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL( vHavokHingeConstraint, vHavokConstraint, 0, &g_vHavokModule);

// ----------------------------------------------------------------------------
const unsigned int vHavokHingeConstraint::s_iSerialVersion = 1;

// ----------------------------------------------------------------------------
hkpConstraintData* vHavokHingeConstraint::CreateConstraintData()
{
  return new hkpLimitedHingeConstraintData();
}

// ----------------------------------------------------------------------------
vHavokConstraintDesc *vHavokHingeConstraint::CreateConstraintDesc()
{
  return new vHavokHingeConstraintDesc();
}

// ----------------------------------------------------------------------------
void vHavokHingeConstraint::ApplyCustomArchiveTransformationToJointDesc(vHavokConstraintDesc *pDesc, hkvVec3& vCustomArchivePositionOfs, hkvMat3 &vCustomArchiveRotationOfs)
{
  vHavokConstraint::ApplyCustomArchiveTransformationToJointDesc(pDesc, vCustomArchivePositionOfs, vCustomArchiveRotationOfs);

  // Apply the custom transformation to the hinge pivot and the rotation axis. These two vectors
  // are defined in world space (since they are valid for both attached objects, independent of the anchor)
  vHavokHingeConstraintDesc* pHingeDesc = static_cast<vHavokHingeConstraintDesc*>(pDesc);
  {
    hkvVec3 vTempHingePivot = pHingeDesc->m_vHingePivot; 
    vTempHingePivot = vCustomArchiveRotationOfs.transformDirection (vTempHingePivot);
    vTempHingePivot += vCustomArchivePositionOfs; 
    pHingeDesc->m_vHingePivot = vTempHingePivot;  

    hkvVec3 vTempHingeRotAxis = pHingeDesc->m_vRotAxis; 
    vTempHingeRotAxis = vCustomArchiveRotationOfs.transformDirection (vTempHingeRotAxis);
    pHingeDesc->m_vRotAxis = vTempHingeRotAxis;  
  }
}

// ----------------------------------------------------------------------------
void vHavokHingeConstraint::InitConstraintDataFromDesc(hkpConstraintData& data, vHavokConstraintDesc const& desc)
{
  vHavokConstraint::InitConstraintDataFromDesc(data, desc);
  hkpLimitedHingeConstraintData& lhData =
    static_cast<hkpLimitedHingeConstraintData&>(data);
  const vHavokHingeConstraintDesc* pHingeDesc = static_cast<const vHavokHingeConstraintDesc*>(&desc);

  hkvMat3 mAxisRotation;
  mAxisRotation.setLookInDirectionMatrix (pHingeDesc->m_vRotAxis);

	hkRotation hRotation; vHavokConversionUtils::VisMatrixToHkRotation( mAxisRotation, hRotation );
	hkVector4 rotAxis; vHavokConversionUtils::VisVecToPhysVec_noscale( pHingeDesc->m_vRotAxis, rotAxis );
	hkVector4 perpAxis; perpAxis.setRotatedDir( hRotation, hkVector4::getConstant<HK_QUADREAL_0100>() ); 

	hkVector4 pivot;
	hkvVec3d worldPivot; 

	if(!GetZonedPivot( pHingeDesc->m_vHingePivot, worldPivot ))
	{  
		vHavokConversionUtils::VisVecToPhysVecLocal(pHingeDesc->m_vHingePivot, pivot);
	}
	else
	{
		vHavokConversionUtils::VisVecToPhysVecLocal(worldPivot, pivot);
	}

	hkTransform transform;
	hkVector4 vLocalPivots[2];
  hkVector4 vLocalAxis[2];
  hkVector4 vLocalPerpAxis[2];

  for (int i = 0; i < 2; ++i)
  {
    VisBaseEntity_cl *pOwnerEntity = (pHingeDesc->m_spBodies[i])
      ? pHingeDesc->m_spBodies[i]->GetOwnerEntity()
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
    
		vLocalPivots[i].setTransformedPos( transform, pivot );
		vLocalAxis[i].setRotatedDir( transform.getRotation(), rotAxis );
		vLocalPerpAxis[i].setRotatedDir( transform.getRotation(), perpAxis );
  }

  {
	  lhData.setInBodySpace(vLocalPivots[0],vLocalPivots[1],vLocalAxis[0],vLocalAxis[1],vLocalPerpAxis[0],vLocalPerpAxis[1]);
  }

  lhData.setMaxAngularLimit (pHingeDesc->m_fAngleMax * HK_REAL_DEG_TO_RAD);
  lhData.setMinAngularLimit (pHingeDesc->m_fAngleMin * HK_REAL_DEG_TO_RAD);
  if(!pHingeDesc->m_bUseLimits)
    lhData.disableLimits();
  
  // Save in local space of first body. We can't save the world space
  // data from the description since the object might have moved since its creation
	vHavokConversionUtils::PhysVecToVisVecLocal( vLocalPivots[0], m_vSavedPivot );
  vHavokConversionUtils::PhysVecToVisVec_noscale( vLocalAxis[0], m_vSavedAxis );

  m_bUseLimits = pHingeDesc->m_bUseLimits;
}

// ----------------------------------------------------------------------------
void vHavokHingeConstraint::SaveToDesc(vHavokConstraintDesc& desc)
{
  vHavokConstraint::SaveToDesc(desc);
  
  vHavokHingeConstraintDesc &hingeDesc = static_cast<vHavokHingeConstraintDesc &>(desc);
  hkpLimitedHingeConstraintData *pHingeData = 
    static_cast<hkpLimitedHingeConstraintData*>(GetHkConstraintData());

  // Transform saved local body[0] pivot back into world space
  VisBaseEntity_cl *pOwnerEntity = (desc.m_spBodies[0])
  ? desc.m_spBodies[0]->GetOwnerEntity()
  : NULL;

	hkTransform transform;
	hkVector4 savedPivot; vHavokConversionUtils::VisVecToPhysVecLocal( m_vSavedPivot, savedPivot );
	hkVector4 savedAxis; vHavokConversionUtils::VisVecToPhysVec_noscale( m_vSavedAxis, savedAxis );

	/*! Get world matrix isn't necessarily in world space for pivoting objects.  Using the havok routines will just take care of it */
	if (pOwnerEntity)
  {
		vHavokConversionUtils::VisMatVecToPhysTransformWorld( pOwnerEntity->GetRotationMatrix(), pOwnerEntity->GetPosition(), transform );
  }
	else
	{
		transform.setIdentity();
	}

	savedPivot.setTransformedPos( transform, savedPivot );
	savedAxis.setRotatedDir( transform.getRotation(), savedAxis );

	GetLocalPivot( savedPivot, hingeDesc.m_vHingePivot ); 
	vHavokConversionUtils::PhysVecToVisVec_noscale( savedAxis, hingeDesc.m_vRotAxis );

  hingeDesc.m_bUseLimits = m_bUseLimits;
  if (pHingeData)
  {
    hingeDesc.m_fAngleMin = pHingeData->getMinAngularLimit() * HK_REAL_RAD_TO_DEG;
    hingeDesc.m_fAngleMax = pHingeData->getMaxAngularLimit() * HK_REAL_RAD_TO_DEG;
  }
  else
  {
    hingeDesc.m_fAngleMin = hkReal(-180);
    hingeDesc.m_fAngleMax = hkReal(180);
  }
}

// ----------------------------------------------------------------------------
void vHavokHingeConstraint::Serialize( VArchive &ar )
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
