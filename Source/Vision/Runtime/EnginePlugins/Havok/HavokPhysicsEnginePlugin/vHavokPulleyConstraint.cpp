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
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPulleyConstraint.hpp>

#include <Physics/Constraint/Data/Pulley/hkpPulleyConstraintData.h>

// ----------------------------------------------------------------------------
// vHavokPrismaticConstraintDesc
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL( vHavokPulleyConstraintDesc, vHavokConstraintDesc, 0, &g_vHavokModule);

// ----------------------------------------------------------------------------
const unsigned int vHavokPulleyConstraintDesc::s_iSerialVersion = 1;

// ----------------------------------------------------------------------------
vHavokPulleyConstraintDesc::vHavokPulleyConstraintDesc()
:vHavokConstraintDesc(false)
{
  Reset();
}

// ----------------------------------------------------------------------------
void vHavokPulleyConstraintDesc::Reset()
{
  vHavokConstraintDesc::Reset();

  m_vPulleyPivots[0] = m_vPulleyPivots[1] = hkvVec3(0.0f, 0.0f, 0.0f);
  m_fLeverage = 0.0f;
  m_fRopeLength = 0.0f;
}

// ----------------------------------------------------------------------------
void vHavokPulleyConstraintDesc::Serialize( VArchive &ar )
{
  vHavokConstraintDesc::Serialize(ar);
  if (ar.IsLoading())
  {
    unsigned int iVersion = 0;
    ar >> iVersion;
    VASSERT((iVersion > 0) && (iVersion <= s_iSerialVersion));

    m_vPulleyPivots[0].SerializeAsVisVector (ar);
    m_vPulleyPivots[1].SerializeAsVisVector (ar);
    ar >> m_fLeverage;
    ar >> m_fRopeLength;
  }
  else
  {
    ar << s_iSerialVersion;
    m_vPulleyPivots[0].SerializeAsVisVector (ar);
    m_vPulleyPivots[1].SerializeAsVisVector (ar);
    ar << m_fLeverage;
    ar << m_fRopeLength;
  }
}


// ----------------------------------------------------------------------------
// vHavokPulleyConstraint
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL( vHavokPulleyConstraint, vHavokConstraint, 0, &g_vHavokModule);

// ----------------------------------------------------------------------------
const unsigned int vHavokPulleyConstraint::s_iSerialVersion = 1;

// ----------------------------------------------------------------------------
hkpConstraintData* vHavokPulleyConstraint::CreateConstraintData()
{
  return new hkpPulleyConstraintData();
}

// ----------------------------------------------------------------------------
vHavokConstraintDesc *vHavokPulleyConstraint::CreateConstraintDesc()
{
  return new vHavokPulleyConstraintDesc();
}

// ----------------------------------------------------------------------------
void vHavokPulleyConstraint::InitConstraintDataFromDesc(hkpConstraintData& data, vHavokConstraintDesc const& desc)
{
  vHavokConstraint::InitConstraintDataFromDesc(data, desc);
  hkpPulleyConstraintData& lhData =
    static_cast<hkpPulleyConstraintData&>(data);
  const vHavokPulleyConstraintDesc* pPulleyDesc = static_cast<const vHavokPulleyConstraintDesc*>(&desc);

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

  hkVector4 p0; vHavokConversionUtils::VisVecToPhysVecWorld(desc.m_vPivots[0], p0);
  hkVector4 p1; vHavokConversionUtils::VisVecToPhysVecWorld(desc.m_vPivots[1], p1);
  hkVector4 pp0; vHavokConversionUtils::VisVecToPhysVecWorld(pPulleyDesc->m_vPulleyPivots[0], pp0);
  hkVector4 pp1; vHavokConversionUtils::VisVecToPhysVecWorld(pPulleyDesc->m_vPulleyPivots[1], pp1);
  lhData.setInWorldSpace(tA, tB, p0, p1, pp0, pp1, pPulleyDesc->m_fLeverage);

  if (pPulleyDesc->m_fRopeLength > 0.0f)
    lhData.setRopeLength(pPulleyDesc->m_fRopeLength);
  
  m_savedPulleyConstraintDesc = *pPulleyDesc;
}

// ----------------------------------------------------------------------------
void vHavokPulleyConstraint::SaveToDesc(vHavokConstraintDesc& desc)
{
  vHavokConstraint::SaveToDesc(desc);

  vHavokPulleyConstraintDesc &pulleyDesc = static_cast<vHavokPulleyConstraintDesc &>(desc);

  pulleyDesc.m_vPulleyPivots[0] = m_savedPulleyConstraintDesc.m_vPulleyPivots[0];
  pulleyDesc.m_vPulleyPivots[1] = m_savedPulleyConstraintDesc.m_vPulleyPivots[1];
  pulleyDesc.m_fLeverage = m_savedPulleyConstraintDesc.m_fLeverage;
  pulleyDesc.m_fRopeLength = m_savedPulleyConstraintDesc.m_fRopeLength;
}

// ----------------------------------------------------------------------------
void vHavokPulleyConstraint::Serialize( VArchive &ar )
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
