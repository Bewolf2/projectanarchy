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
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPrismaticConstraint.hpp>

#include <Physics/Constraint/Data/Prismatic/hkpPrismaticConstraintData.h>

// ----------------------------------------------------------------------------
// vHavokPrismaticConstraintDesc
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL( vHavokPrismaticConstraintDesc, vHavokConstraintDesc, 0, &g_vHavokModule);

// ----------------------------------------------------------------------------
const unsigned int vHavokPrismaticConstraintDesc::s_iSerialVersion = 3;

// ----------------------------------------------------------------------------
vHavokPrismaticConstraintDesc::vHavokPrismaticConstraintDesc()
:vHavokConstraintDesc(false)
{
  Reset();
}

// ----------------------------------------------------------------------------
void vHavokPrismaticConstraintDesc::Reset()
{
  vHavokConstraintDesc::Reset();

  m_vPrismaticPivot = hkvVec3::ZeroVector ();
  m_vPrismaticAxis = hkvVec3(0.f, 0.f, 1.f);
  m_fLinearMin = -FLT_MAX;
  m_fLinearMax = FLT_MAX;
  m_bAllowRotationAroundAxis = false;
}

// ----------------------------------------------------------------------------
void vHavokPrismaticConstraintDesc::Serialize( VArchive &ar )
{
  vHavokConstraintDesc::Serialize(ar);
  if (ar.IsLoading())
  {
    unsigned int iVersion = 0;
    ar >> iVersion;
    VASSERT((iVersion > 0) && (iVersion <= s_iSerialVersion));

    m_vPrismaticPivot.SerializeAsVisVector (ar);
    m_vPrismaticAxis.SerializeAsVisVector (ar);

		if(iVersion < 3 )
		{
			float constraint;
			ar >> constraint; m_fLinearMin = static_cast< hkReal >( constraint );
			ar >> constraint; m_fLinearMax = static_cast< hkReal >( constraint );
		}
		else
		{
			ar >> m_fLinearMin;
			ar >> m_fLinearMax;
		}

    if (iVersion > 1)
      ar >> m_bAllowRotationAroundAxis;
  }
  else
  {
    ar << s_iSerialVersion;
    m_vPrismaticPivot.SerializeAsVisVector (ar);
    m_vPrismaticAxis.SerializeAsVisVector (ar);
    ar << m_fLinearMin;
    ar << m_fLinearMax;
    ar << m_bAllowRotationAroundAxis;
  }
}


// ----------------------------------------------------------------------------
// vHavokPrismaticConstraint
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL( vHavokPrismaticConstraint, vHavokConstraint, 0, &g_vHavokModule);

// ----------------------------------------------------------------------------
const unsigned int vHavokPrismaticConstraint::s_iSerialVersion = 1;

// ----------------------------------------------------------------------------
hkpConstraintData* vHavokPrismaticConstraint::CreateConstraintData()
{
  return new hkpPrismaticConstraintData();
}

// ----------------------------------------------------------------------------
vHavokConstraintDesc *vHavokPrismaticConstraint::CreateConstraintDesc()
{
  return new vHavokPrismaticConstraintDesc();
}

// ----------------------------------------------------------------------------
void vHavokPrismaticConstraint::InitConstraintDataFromDesc(hkpConstraintData& data, vHavokConstraintDesc const& desc)
{
  vHavokConstraint::InitConstraintDataFromDesc(data, desc);
  hkpPrismaticConstraintData& lhData =
    static_cast<hkpPrismaticConstraintData&>(data);
  const vHavokPrismaticConstraintDesc* pPrismaticDesc = static_cast<const vHavokPrismaticConstraintDesc*>(&desc);

  hkvMat3 mAxis (hkvNoInitialization);
  mAxis.setLookInDirectionMatrix (pPrismaticDesc->m_vPrismaticAxis);

  hkRotation hRotation; vHavokConversionUtils::VisMatrixToHkRotation( mAxis, hRotation );
  hkVector4 perpAxis; perpAxis.setRotatedDir( hRotation, hkVector4::getConstant<HK_QUADREAL_0100>() ); 
  hkVector4 prismaticAxis; vHavokConversionUtils::VisVecToPhysVec_noscale( pPrismaticDesc->m_vPrismaticAxis, prismaticAxis );

  hkVector4 pivot;
  hkvVec3d worldPivot; 

  if(!GetZonedPivot( pPrismaticDesc->m_vPrismaticPivot, worldPivot ))
  {  
	  vHavokConversionUtils::VisVecToPhysVecLocal(pPrismaticDesc->m_vPrismaticPivot, pivot);
  }
  else
  {
	  vHavokConversionUtils::VisVecToPhysVecLocal(worldPivot, pivot);
  }

  hkVector4 vLocalPivots[2];
  hkVector4 vLocalAxis[2];
  hkVector4 vLocalPerpAxis[2];
  hkTransform transform;

  for (int i = 0; i < 2; ++i)
  {
    VisBaseEntity_cl *pOwnerEntity = (pPrismaticDesc->m_spBodies[i])
      ? pPrismaticDesc->m_spBodies[i]->GetOwnerEntity()
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
    vLocalAxis[i].setRotatedDir( transform.getRotation(), prismaticAxis );
    vLocalPerpAxis[i].setRotatedDir( transform.getRotation(), perpAxis );
  }

  {
	  lhData.setInBodySpace(vLocalPivots[0],vLocalPivots[1],vLocalAxis[0],vLocalAxis[1],vLocalPerpAxis[0],vLocalPerpAxis[1]);
  }

  lhData.setMaxLinearLimit(pPrismaticDesc->m_fLinearMax);
  lhData.setMinLinearLimit(pPrismaticDesc->m_fLinearMin);

  lhData.allowRotationAroundAxis(pPrismaticDesc->m_bAllowRotationAroundAxis);
  
  // Save in local space of first body. We can't save the world space
  // data from the description since the object might have moved since its creation
  vHavokConversionUtils::PhysVecToVisVecLocal(vLocalPivots[0], m_vSavedPivot); 
  vHavokConversionUtils::PhysVecToVisVec_noscale(vLocalAxis[0], m_vSavedAxis);

  m_bAllowRotationAroundAxis = pPrismaticDesc->m_bAllowRotationAroundAxis;
}

// ----------------------------------------------------------------------------
void vHavokPrismaticConstraint::SaveToDesc(vHavokConstraintDesc& desc)
{
  vHavokConstraint::SaveToDesc(desc);

  vHavokPrismaticConstraintDesc &prismaticDesc = static_cast<vHavokPrismaticConstraintDesc &>(desc);
  hkpPrismaticConstraintData *pPrismaticData = 
    static_cast<hkpPrismaticConstraintData*>(GetHkConstraintData());

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
  
  GetLocalPivot( savedPivot, prismaticDesc.m_vPrismaticPivot ); 
  vHavokConversionUtils::PhysVecToVisVec_noscale( savedAxis, prismaticDesc.m_vPrismaticAxis );

  if (pPrismaticData)
  {
    prismaticDesc.m_fLinearMin = pPrismaticData->getMinLinearLimit();
    prismaticDesc.m_fLinearMax = pPrismaticData->getMaxLinearLimit();
  }
  else
  {
    prismaticDesc.m_fLinearMin = -HK_REAL_MAX;
    prismaticDesc.m_fLinearMax = HK_REAL_MAX;
  }

  prismaticDesc.m_bAllowRotationAroundAxis = m_bAllowRotationAroundAxis;
}

// ----------------------------------------------------------------------------
void vHavokPrismaticConstraint::Serialize( VArchive &ar )
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
