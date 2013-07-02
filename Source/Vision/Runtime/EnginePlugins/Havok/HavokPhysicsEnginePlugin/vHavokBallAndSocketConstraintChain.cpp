/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokBallAndSocketConstraintChain.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>

#include <Physics/Constraint/Data/AngularFriction/hkpAngularFrictionConstraintData.h>

#include <Common/Base/Math/Vector/hkVector4Util.h>
#include <Common/Base/Types/hkBaseTypes.h>
#include <Physics2012/Dynamics/Constraint/Chain/hkpConstraintChainInstance.h>
#include <Physics2012/Dynamics/Constraint/Chain/BallSocket/hkpBallSocketChainData.h>

// ----------------------------------------------------------------------------
// vHavokBallAndSocketConstraintChainDesc
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL( vHavokBallAndSocketConstraintChainDesc, vHavokConstraintChainDesc, 0, &g_vHavokModule);

// ----------------------------------------------------------------------------
const unsigned int vHavokBallAndSocketConstraintChainDesc::s_iSerialVersion = 2u;

// ----------------------------------------------------------------------------
vHavokBallAndSocketConstraintChainDesc::vHavokBallAndSocketConstraintChainDesc()
: vHavokConstraintChainDesc(false)
{
  Reset();
}

// ----------------------------------------------------------------------------
void vHavokBallAndSocketConstraintChainDesc::Reset()
{
  vHavokConstraintChainDesc::Reset();
  m_fMaximumLastLinkGap = 0.05f;
}

// ----------------------------------------------------------------------------
void vHavokBallAndSocketConstraintChainDesc::Serialize( VArchive &ar )
{
  vHavokConstraintChainDesc::Serialize(ar);
  if (ar.IsLoading())
  {
    unsigned int iVersion = 0;
    ar >> iVersion;
    VASSERT((iVersion > 0) && (iVersion <= s_iSerialVersion));

    if (iVersion >= 2)
    {
      ar >> m_fMaximumLastLinkGap;
    }
  }
  else
  {
    ar << s_iSerialVersion;
    
    // Version 2
    ar << m_fMaximumLastLinkGap;
  }
}



// ----------------------------------------------------------------------------
// vHavokBallAndSocketConstraintChain
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL( vHavokBallAndSocketConstraintChain, vHavokConstraintChain, 0, &g_vHavokModule);

// ----------------------------------------------------------------------------
const unsigned int vHavokBallAndSocketConstraintChain::s_iSerialVersion = 1u;

// ----------------------------------------------------------------------------
void vHavokBallAndSocketConstraintChain::AppendConstraintInfo(const hkvVec3& vPivotA, 
  const hkvVec3& vPivotB, bool bIsAnchor)
{
  hkpBallSocketChainData& basData = 
    static_cast<hkpBallSocketChainData&>(*m_pConstraintChainData);

  hkVector4 p0; vHavokConversionUtils::VisVecToPhysVecLocal(vPivotA, p0);
  hkVector4 p1; vHavokConversionUtils::VisVecToPhysVecLocal(vPivotB, p1);
  basData.addConstraintInfoInBodySpace(p0,p1);
}

// ----------------------------------------------------------------------------
hkvVec3 vHavokBallAndSocketConstraintChain::CalcLinkPivot(unsigned int iIndex)
{
  VVERIFY_OR_RET_VAL(iIndex < 2, hkvVec3::ZeroVector ());
  if (iIndex == 0)
    return hkvVec3(m_fLinkLength / 2.f, 0, 0);
  else
    return hkvVec3(-m_fLinkLength / 2.f, 0, 0);
}

// ----------------------------------------------------------------------------
hkpConstraintChainData *vHavokBallAndSocketConstraintChain::CreateConstraintChainData()
{
  return new hkpBallSocketChainData;
}

// ----------------------------------------------------------------------------
vHavokConstraintChainDesc *vHavokBallAndSocketConstraintChain::CreateConstraintChainDesc()
{
  return new vHavokBallAndSocketConstraintChainDesc;
}

// ----------------------------------------------------------------------------
void vHavokBallAndSocketConstraintChain::FillRagdollConstraintInfo(
  vHavokRagdollConstraintInfo &ragdollInfo, unsigned int iConstraintIndex)
{
  // Derived from source code provided by Havok
  hkpBallSocketChainData& basData = 
    static_cast<hkpBallSocketChainData&>(*m_pConstraintChainData);
  hkpConstraintChainInstance *pChain = GetHkConstraintChainInstance();

  hkpRigidBody* b0 = (hkpRigidBody*)pChain->m_chainedEntities[iConstraintIndex];
  hkpRigidBody* b1 = (hkpRigidBody*)pChain->m_chainedEntities[iConstraintIndex+1];
  
  hkVector4 pivots[2] = { 
    basData.m_infos[iConstraintIndex].m_pivotInA, 
    basData.m_infos[iConstraintIndex].m_pivotInB 
  };
  
  // Create ragdoll constraint data
  hkpAngularFrictionConstraintData* data = new hkpAngularFrictionConstraintData(); // RagdollLimits do not have friction
  {
    hkVector4 twistAxis; 
	const hkQuaternion& q = (iConstraintIndex > 0) ? b0->getRotation() : b1->getRotation();
    twistAxis._setRotatedDir(q, hkVector4::getConstant<HK_QUADREAL_0010>());
    
    hkVector4 planeAxis; 
    hkVector4Util::calculatePerpendicularVector(twistAxis, planeAxis);
    planeAxis.normalize<3>(); // do we need to normalize ?

    hkVector4 twistAxisA; 
    twistAxisA._setRotatedDir(b0->getRotation(), twistAxis);
    hkVector4 planeAxisA; 
    planeAxisA._setRotatedDir(b0->getRotation(), planeAxis); 
    
    hkVector4 twistAxisB; 
    twistAxisB._setRotatedDir(b1->getRotation(), twistAxis);
    hkVector4 planeAxisB; 
    planeAxisB._setRotatedDir(b1->getRotation(), planeAxis); 

    data->setInBodySpace(pivots[0], pivots[1], planeAxisA, planeAxisB, twistAxisA, twistAxisB);

    data->m_atoms.m_angFriction.m_isEnabled = true;
    data->m_atoms.m_angFriction.m_maxFrictionTorque = hkReal(0);
  }

  hkpConstraintInstance* instance = new hkpConstraintInstance(b0, b1, data);
  data->removeReference();

  ragdollInfo.m_instance = instance;
}

// ----------------------------------------------------------------------------
unsigned int vHavokBallAndSocketConstraintChain::GetConstraintInfoCount()
{
  hkpBallSocketChainData& basData = 
    static_cast<hkpBallSocketChainData&>(*m_pConstraintChainData);
  return basData.m_infos.getSize();  
}

// ----------------------------------------------------------------------------
hkVector4 vHavokBallAndSocketConstraintChain::GetImpulseApplied() const
{
  hkpBallSocketChainData *pChainData = static_cast<hkpBallSocketChainData*>(m_pConstraintChainData);
  const hkpBallSocketChainData::Runtime *pRuntime = pChainData->getRuntime(GetHkConstraintChainInstance()->getRuntime());
  hkpBallSocketChainData::Runtime *pRuntimeNonConst = const_cast<hkpBallSocketChainData::Runtime*>(pRuntime);

  hkSimdReal i0; i0.load<1>(&pRuntimeNonConst->getSolverResults()[0].m_impulseApplied);
  hkSimdReal i1; i1.load<1>(&pRuntimeNonConst->getSolverResults()[1].m_impulseApplied);
  hkSimdReal i2; i2.load<1>(&pRuntimeNonConst->getSolverResults()[2].m_impulseApplied);

  hkVector4 impulse; impulse.set(i0,i1,i2,hkSimdReal_0);

  return impulse;
}

// ----------------------------------------------------------------------------
float vHavokBallAndSocketConstraintChain::GetLastLinkAllowedGap() const
{
  return m_fMaximumLastLinkGap;
}

// ----------------------------------------------------------------------------
unsigned int vHavokBallAndSocketConstraintChain::GetMaxNumLinks() const
{
#ifdef HK_REAL_IS_DOUBLE
  return 105;
#else
  return 211;
#endif
}

// ----------------------------------------------------------------------------
bool vHavokBallAndSocketConstraintChain::GetParametersFromDesc(
  vHavokConstraintChainDesc const& desc)
{
  if (!vHavokConstraintChain::GetParametersFromDesc(desc))
    return false;

  vHavokBallAndSocketConstraintChainDesc const& basDesc =
    static_cast<vHavokBallAndSocketConstraintChainDesc const&>(desc);
  
  m_fMaximumLastLinkGap = hkMath::clamp(basDesc.m_fMaximumLastLinkGap, 0.0f, 1.0f);

  return true;
}

// ----------------------------------------------------------------------------
void vHavokBallAndSocketConstraintChain::InitConstraintChainDataFromDesc(
  hkpConstraintChainData& data, vHavokConstraintChainDesc const& desc)
{
  vHavokConstraintChain::InitConstraintChainDataFromDesc(data, desc);

  hkpBallSocketChainData &basData = static_cast<hkpBallSocketChainData&>(data);
  basData.m_damping = desc.m_fChainDamping;
}

// ----------------------------------------------------------------------------
void vHavokBallAndSocketConstraintChain::RemoveConstraintInfo(unsigned int iIndex)
{
  hkpBallSocketChainData& basData = 
    static_cast<hkpBallSocketChainData&>(*m_pConstraintChainData);
  basData.m_infos.removeAtAndCopy(iIndex);
}

// ----------------------------------------------------------------------------
void vHavokBallAndSocketConstraintChain::SaveToDesc(
  vHavokConstraintChainDesc& desc)
{
  vHavokConstraintChain::SaveToDesc(desc);
  
  const hkpBallSocketChainData *pBasData = GetHkConstraintChainInstance()
    ? static_cast<const hkpBallSocketChainData*>(GetHkConstraintChainInstance()->getData())
    : NULL;
  vHavokBallAndSocketConstraintChainDesc &basDesc =
    static_cast<vHavokBallAndSocketConstraintChainDesc&>(desc);

  if (pBasData)
    desc.m_fChainDamping = float(pBasData->m_damping);

  basDesc.m_fMaximumLastLinkGap = m_fMaximumLastLinkGap;
}

// ----------------------------------------------------------------------------
void vHavokBallAndSocketConstraintChain::Serialize( VArchive &ar )
{
  vHavokConstraintChain::Serialize(ar);
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
void vHavokBallAndSocketConstraintChain::UpdateChainPhysicsProperties()
{
  //
  // Based on constraint chain modification sample implementation from Havok
  //
  vHavokConstraintChain::UpdateChainPhysicsProperties();

  hkReal fLength = GetChainLength();
  if (fLength <= hkReal(0))
    return;

  hkReal fSolverTau = m_pModule->GetSolverTau();

  hkReal fCFM = m_fFixedCfm;
  if (m_bComputeCfm)
  {
    int iMicroSteps = hkMath::max2(1, m_pModule->GetNumSolverMicrosteps());
    hkReal fFrameTime = hkReal(1) / hkReal(m_pModule->GetPhysicsTickCount() * iMicroSteps);
    hkReal fSpringConstant = m_fMaterialStretchingConstant / fLength;
    fCFM = hkReal(25) * (hkReal(iMicroSteps) * fSolverTau) / (fSpringConstant * fFrameTime * hkReal(m_iNumLinks + 1));
    fCFM *= m_fCfmFactor;
  }

  hkpBallSocketChainData& basData = 
    static_cast<hkpBallSocketChainData&>(*m_pConstraintChainData);

  basData.m_tau = fSolverTau;
  basData.m_cfm = fCFM;
  basData.m_maxErrorDistance = hkReal(10000);
}

// ----------------------------------------------------------------------------
void vHavokBallAndSocketConstraintChain::UpdateConstraintInfo(
  unsigned int iIndex, const hkvVec3& vPivotA, const hkvVec3& vPivotB)
{
  hkpBallSocketChainData& basData = 
    static_cast<hkpBallSocketChainData&>(*m_pConstraintChainData);

  hkpBallSocketChainData::ConstraintInfo info;
  vHavokConversionUtils::VisVecToPhysVecLocal(vPivotA, info.m_pivotInA);
  vHavokConversionUtils::VisVecToPhysVecLocal(vPivotB, info.m_pivotInB);

  basData.m_infos[iIndex] = info;
}

// ----------------------------------------------------------------------------
void vHavokBallAndSocketConstraintChain::UpdateRagdollConstraintState()
{
  // Derived from source code provided by Havok
  hkpBallSocketChainData *pChainData = 
    static_cast<hkpBallSocketChainData*>(m_pConstraintChainData);
  const hkpBallSocketChainData::Runtime *pRuntime = 
    pChainData->getRuntime(GetHkConstraintChainInstance()->getRuntime());
  hkpBallSocketChainData::Runtime *pRuntimeNonConst = 
    const_cast<hkpBallSocketChainData::Runtime*>(pRuntime);

  for (int ri = 0; ri < m_ragdollConstraintInfos.getSize(); ri++)
  {
    vHavokRagdollConstraintInfo& info = m_ragdollConstraintInfos[ri];

    // Get tension o chain
    hkSimdReal i0; i0.load<1>(&pRuntimeNonConst->getSolverResults()[info.m_sourceInfo*3+0].m_impulseApplied);
	hkSimdReal i1; i1.load<1>(&pRuntimeNonConst->getSolverResults()[info.m_sourceInfo*3+1].m_impulseApplied);
	hkSimdReal i2; i2.load<1>(&pRuntimeNonConst->getSolverResults()[info.m_sourceInfo*3+2].m_impulseApplied);
	hkVector4 impulse; impulse.set(i0,i1,i2,i2);
	const hkSimdReal impulseValue = impulse.length<3>() + hkSimdReal::fromFloat(m_fBaseTension);

    float fri = float ( ri ) / float(m_ragdollConstraintInfos.getSize()-1);
    float minDivMax = m_fFrictionFactorMin / m_fFrictionFactorMax;
    float f = 1.0f - 4.0f * (1.0f - minDivMax) * fri * (1.0f - fri);
	const hkSimdReal angFriction = impulseValue * hkSimdReal::fromFloat(f) * hkSimdReal::fromFloat(m_fFrictionFactorMax);

    hkpAngularFrictionConstraintData* data = const_cast<hkpAngularFrictionConstraintData*>(static_cast<const hkpAngularFrictionConstraintData*>(info.m_instance->getData()));
    angFriction.store<1>(&data->m_atoms.m_angFriction.m_maxFrictionTorque);
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
