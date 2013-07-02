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
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPointToPlaneConstraint.hpp>

#include <Physics/Constraint/Data/PointToPlane/hkpPointToPlaneConstraintData.h>

// ----------------------------------------------------------------------------
// vHavokPointToPlaneConstraintDesc
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL( vHavokPointToPlaneConstraintDesc, vHavokConstraintDesc, 0, &g_vHavokModule);

// ----------------------------------------------------------------------------
const unsigned int vHavokPointToPlaneConstraintDesc::s_iSerialVersion = 1;

// ----------------------------------------------------------------------------
vHavokPointToPlaneConstraintDesc::vHavokPointToPlaneConstraintDesc()
:vHavokConstraintDesc(false)
{
  Reset();
}

// ----------------------------------------------------------------------------
void vHavokPointToPlaneConstraintDesc::Reset()
{
  vHavokConstraintDesc::Reset();

  m_vPointToPlanePivot = hkvVec3(0.0f, 0.0f, 0.0f);
  m_vPlaneNormal = hkvVec3(0.0f, 0.0f, 1.0f);
}

// ----------------------------------------------------------------------------
void vHavokPointToPlaneConstraintDesc::Serialize( VArchive &ar )
{
  vHavokConstraintDesc::Serialize(ar);
  if (ar.IsLoading())
  {
    unsigned int iVersion = 0;
    ar >> iVersion;
    VASSERT((iVersion > 0) && (iVersion <= s_iSerialVersion));

    m_vPointToPlanePivot.SerializeAsVisVector (ar);
    m_vPlaneNormal.SerializeAsVisVector (ar);
  }
  else
  {
    ar << s_iSerialVersion;
    m_vPointToPlanePivot.SerializeAsVisVector (ar);
    m_vPlaneNormal.SerializeAsVisVector (ar);
  }
}


// ----------------------------------------------------------------------------
// vHavokPointToPlaneConstraint
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL( vHavokPointToPlaneConstraint, vHavokConstraint, 0, &g_vHavokModule);

// ----------------------------------------------------------------------------
const unsigned int vHavokPointToPlaneConstraint::s_iSerialVersion = 1;

// ----------------------------------------------------------------------------
hkpConstraintData* vHavokPointToPlaneConstraint::CreateConstraintData()
{
  return new hkpPointToPlaneConstraintData();
}

// ----------------------------------------------------------------------------
vHavokConstraintDesc *vHavokPointToPlaneConstraint::CreateConstraintDesc()
{
  return new vHavokPointToPlaneConstraintDesc();
}
#pragma optimize("", off)
// ----------------------------------------------------------------------------
void vHavokPointToPlaneConstraint::InitConstraintDataFromDesc(hkpConstraintData& data, vHavokConstraintDesc const& desc)
{
  vHavokConstraint::InitConstraintDataFromDesc(data, desc);
  hkpPointToPlaneConstraintData& lhData =
    static_cast<hkpPointToPlaneConstraintData&>(data);
  const vHavokPointToPlaneConstraintDesc* pPointToPlaneDesc = static_cast<const vHavokPointToPlaneConstraintDesc*>(&desc);

  hkTransform tA;
  if (desc.m_spBodies[0] != NULL && desc.m_spBodies[0]->GetHkRigidBody() != NULL)
    tA = desc.m_spBodies[0]->GetHkRigidBody()->getTransform();
  else
    tA.setIdentity();

  hkTransform tB;
  if (desc.m_spBodies[1] != NULL && desc.m_spBodies[1]->GetHkRigidBody() != NULL)
    tB = desc.m_spBodies[1]->GetHkRigidBody()->getTransform();
  else
    tB.setIdentity();

  hkVector4 pn; vHavokConversionUtils::VisVecToPhysVec_noscale(pPointToPlaneDesc->m_vPlaneNormal, pn);
  hkVector4 pi; vHavokConversionUtils::VisVecToPhysVecWorld(pPointToPlaneDesc->m_vPointToPlanePivot, pi);

  lhData.setInWorldSpace(tA, tB, pi, pn);

  m_savedPointToPlaneDesc = *pPointToPlaneDesc;
}
#pragma optimize("", on)
// ----------------------------------------------------------------------------
void vHavokPointToPlaneConstraint::SaveToDesc(vHavokConstraintDesc& desc)
{
  vHavokConstraint::SaveToDesc(desc);

  vHavokPointToPlaneConstraintDesc &pointToPlaneDesc = static_cast<vHavokPointToPlaneConstraintDesc &>(desc);

  pointToPlaneDesc.m_vPointToPlanePivot = m_savedPointToPlaneDesc.m_vPointToPlanePivot;
  pointToPlaneDesc.m_vPlaneNormal = m_savedPointToPlaneDesc.m_vPlaneNormal;
}

// ----------------------------------------------------------------------------
void vHavokPointToPlaneConstraint::Serialize( VArchive &ar )
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
