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
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokStiffSpringConstraint.hpp>

#include <Physics/Constraint/Data/StiffSpring/hkpStiffSpringConstraintData.h>

// ----------------------------------------------------------------------------
// vHavokBallAndSocketConstraintDesc
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL( vHavokStiffSpringConstraintDesc, vHavokConstraintDesc, 0, &g_vHavokModule);

// ----------------------------------------------------------------------------
const unsigned int vHavokStiffSpringConstraintDesc::s_iSerialVersion = 1;

// ----------------------------------------------------------------------------
vHavokStiffSpringConstraintDesc::vHavokStiffSpringConstraintDesc()
:vHavokConstraintDesc(false)
{
  Reset();
}

// ----------------------------------------------------------------------------
void vHavokStiffSpringConstraintDesc::Reset()
{
  vHavokConstraintDesc::Reset();
}

// ----------------------------------------------------------------------------
void vHavokStiffSpringConstraintDesc::Serialize( VArchive &ar )
{
  vHavokConstraintDesc::Serialize(ar);
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
// vHavokBallAndSocketConstraint
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL( vHavokStiffSpringConstraint, vHavokConstraint, 0, &g_vHavokModule);

// ----------------------------------------------------------------------------
const unsigned int vHavokStiffSpringConstraint::s_iSerialVersion = 1;

// ----------------------------------------------------------------------------
hkpConstraintData* vHavokStiffSpringConstraint::CreateConstraintData()
{
  return new hkpStiffSpringConstraintData();
}

// ----------------------------------------------------------------------------
vHavokConstraintDesc *vHavokStiffSpringConstraint::CreateConstraintDesc()
{
  return new vHavokStiffSpringConstraintDesc();
}
#pragma optimize("", off)
// ----------------------------------------------------------------------------
void vHavokStiffSpringConstraint::InitConstraintDataFromDesc(hkpConstraintData& data, vHavokConstraintDesc const& desc)
{
  vHavokConstraint::InitConstraintDataFromDesc(data, desc);
  hkpStiffSpringConstraintData& bsData =
    static_cast<hkpStiffSpringConstraintData&>(data);

  hkTransform tA;
  if (desc.m_spBodies[0] != NULL && desc.m_spBodies[0]->GetHkRigidBody() != NULL)
  {
    tA = desc.m_spBodies[0]->GetHkRigidBody()->getTransform();
  }
  else
  {
    tA.setIdentity();
  }

  hkTransform tB;
  if (desc.m_spBodies[1] != NULL && desc.m_spBodies[1]->GetHkRigidBody() != NULL)
  {
    tB = desc.m_spBodies[1]->GetHkRigidBody()->getTransform();
  }
  else
  {
    tB.setIdentity();
  }

  hkVector4 p0; vHavokConversionUtils::VisVecToPhysVecWorld(desc.m_vPivots[0],p0);
  hkVector4 p1; vHavokConversionUtils::VisVecToPhysVecWorld(desc.m_vPivots[1],p1);
  bsData.setInWorldSpace(tA, tB, p0, p1 );
}
#pragma optimize("", on)
// ----------------------------------------------------------------------------
void vHavokStiffSpringConstraint::SaveToDesc(vHavokConstraintDesc& desc)
{
  vHavokConstraint::SaveToDesc(desc);
  for (unsigned int i = 0; i < 2; ++i)
    desc.m_vPivots[i] = m_vSavedPivots[i];
}

// ----------------------------------------------------------------------------
void vHavokStiffSpringConstraint::Serialize( VArchive &ar )
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
