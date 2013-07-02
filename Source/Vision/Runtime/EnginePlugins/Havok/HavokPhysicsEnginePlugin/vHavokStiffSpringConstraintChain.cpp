/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokStiffSpringConstraintChain.hpp>

#include <Common/Base/Types/hkBaseTypes.h>
#include <Physics2012/Dynamics/Constraint/Chain/hkpConstraintChainInstance.h>
#include <Physics2012/Dynamics/Constraint/Chain/StiffSpring/hkpStiffSpringChainData.h>

// ----------------------------------------------------------------------------
// vHavokStiffSpringConstraintChainDesc
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL( vHavokStiffSpringConstraintChainDesc, vHavokConstraintChainDesc, 0, &g_vHavokModule);

// ----------------------------------------------------------------------------
const unsigned int vHavokStiffSpringConstraintChainDesc::s_iSerialVersion = 2u;

// ----------------------------------------------------------------------------
vHavokStiffSpringConstraintChainDesc::vHavokStiffSpringConstraintChainDesc()
: vHavokConstraintChainDesc(false)
{
  Reset();
}

// ----------------------------------------------------------------------------
void vHavokStiffSpringConstraintChainDesc::Reset()
{
  vHavokConstraintChainDesc::Reset();
  m_fLinkPivotOffset = 0.25f;
}

// ----------------------------------------------------------------------------
void vHavokStiffSpringConstraintChainDesc::Serialize( VArchive &ar )
{
  vHavokConstraintChainDesc::Serialize(ar);
  if (ar.IsLoading())
  {
    unsigned int iVersion = 0;
    ar >> iVersion;
    VASSERT((iVersion > 0) && (iVersion <= s_iSerialVersion));

    if (iVersion > 1)
    {
      ar >> m_fLinkPivotOffset;
    }
  }
  else
  {
    ar << s_iSerialVersion;
    
    //Version 2
    ar << m_fLinkPivotOffset;
  }
}



// ----------------------------------------------------------------------------
// vHavokStiffSpringConstraintChain
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL( vHavokStiffSpringConstraintChain, vHavokConstraintChain, 0, &g_vHavokModule);

// ----------------------------------------------------------------------------
const unsigned int vHavokStiffSpringConstraintChain::s_iSerialVersion = 1u;
#pragma optimize("", off)
// ----------------------------------------------------------------------------
void vHavokStiffSpringConstraintChain::AppendConstraintInfo(const hkvVec3& vPivotA, 
  const hkvVec3& vPivotB, bool bIsAnchor)
{
  hkpStiffSpringChainData& ssData = 
    static_cast<hkpStiffSpringChainData&>(*m_pConstraintChainData);

  hkReal fSpringLength = bIsAnchor
    ? hkReal(0)
    : hkReal(VIS2HK_FLOAT_SCALED(m_fLinkLength) * m_fLinkPivotOffset * 2.f);


  hkVector4 p0; 
  vHavokConversionUtils::VisVecToPhysVecLocal(vPivotA,p0);
  hkVector4 p1; 
  vHavokConversionUtils::VisVecToPhysVecLocal(vPivotB,p1);
  ssData.addConstraintInfoInBodySpace(p0,p1,fSpringLength);


}
#pragma optimize("", on)
// ----------------------------------------------------------------------------
hkvVec3 vHavokStiffSpringConstraintChain::CalcLinkPivot(unsigned int iIndex)
{
  VVERIFY_OR_RET_VAL(iIndex < 2, hkvVec3::ZeroVector ());
  if (iIndex == 0)
    return hkvVec3((m_fLinkLength / 2.f) - (m_fLinkLength * m_fLinkPivotOffset), 0, 0);
  else
    return hkvVec3(-(m_fLinkLength / 2.f) + (m_fLinkLength * m_fLinkPivotOffset), 0, 0);
}

// ----------------------------------------------------------------------------
hkpConstraintChainData *vHavokStiffSpringConstraintChain::CreateConstraintChainData()
{
  return new hkpStiffSpringChainData;
}

// ----------------------------------------------------------------------------
vHavokConstraintChainDesc *vHavokStiffSpringConstraintChain::CreateConstraintChainDesc()
{
  return new vHavokStiffSpringConstraintChainDesc;
}

// ----------------------------------------------------------------------------
unsigned int vHavokStiffSpringConstraintChain::GetConstraintInfoCount()
{
  hkpStiffSpringChainData& ssData = 
    static_cast<hkpStiffSpringChainData&>(*m_pConstraintChainData);
  return ssData.m_infos.getSize();
}

// ----------------------------------------------------------------------------
hkVector4 vHavokStiffSpringConstraintChain::GetImpulseApplied() const
{
  hkpStiffSpringChainData *pChainData = 
    static_cast<hkpStiffSpringChainData*>(m_pConstraintChainData);
  const hkpStiffSpringChainData::Runtime *pRuntime = 
    pChainData->getRuntime(GetHkConstraintChainInstance()->getRuntime());
  hkpStiffSpringChainData::Runtime *pRuntimeNonConst = 
    const_cast<hkpStiffSpringChainData::Runtime*>(pRuntime);

  hkSimdReal i0; i0.load<1,HK_IO_NATIVE_ALIGNED>(&pRuntimeNonConst->getSolverResults()[0].m_impulseApplied);
  hkSimdReal i1; i1.load<1,HK_IO_NATIVE_ALIGNED>(&pRuntimeNonConst->getSolverResults()[1].m_impulseApplied);
  hkSimdReal i2; i2.load<1,HK_IO_NATIVE_ALIGNED>(&pRuntimeNonConst->getSolverResults()[2].m_impulseApplied);

  hkVector4 impulse;
  impulse.set(i0,i1,i2,hkSimdReal_0);

  return impulse;
}

// ----------------------------------------------------------------------------
unsigned int vHavokStiffSpringConstraintChain::GetMaxNumLinks() const
{
#ifdef HK_REAL_IS_DOUBLE
  return 408;
#else
  return 817;
#endif
}


// ----------------------------------------------------------------------------
float vHavokStiffSpringConstraintChain::GetLastLinkAllowedGap() const
{
  return 0.5f - m_fLinkPivotOffset;
}

// ----------------------------------------------------------------------------
bool vHavokStiffSpringConstraintChain::GetParametersFromDesc(
  vHavokConstraintChainDesc const& desc)
{
  if (!vHavokConstraintChain::GetParametersFromDesc(desc))
    return false;

  vHavokStiffSpringConstraintChainDesc const& ssDesc =
    static_cast<vHavokStiffSpringConstraintChainDesc const&>(desc);

  m_fLinkPivotOffset = hkMath::clamp(ssDesc.m_fLinkPivotOffset, 0.0f, 0.5f);

  return true;
}

// ----------------------------------------------------------------------------
void vHavokStiffSpringConstraintChain::InitConstraintChainDataFromDesc(
  hkpConstraintChainData& data, vHavokConstraintChainDesc const& desc)
{
  vHavokConstraintChain::InitConstraintChainDataFromDesc(data, desc);

  hkpStiffSpringChainData &ssData = static_cast<hkpStiffSpringChainData&>(data);
  ssData.m_damping = desc.m_fChainDamping;
}

// ----------------------------------------------------------------------------
void vHavokStiffSpringConstraintChain::RemoveConstraintInfo(unsigned int iIndex)
{
  hkpStiffSpringChainData& ssData = 
    static_cast<hkpStiffSpringChainData&>(*m_pConstraintChainData);
  ssData.m_infos.removeAtAndCopy(iIndex);
}

// ----------------------------------------------------------------------------
void vHavokStiffSpringConstraintChain::SaveToDesc(
  vHavokConstraintChainDesc& desc)
{
  vHavokConstraintChain::SaveToDesc(desc);

  const hkpStiffSpringChainData *pSsData = GetHkConstraintChainInstance()
    ? static_cast<const hkpStiffSpringChainData*>(GetHkConstraintChainInstance()->getData())
    : NULL;

  vHavokStiffSpringConstraintChainDesc& ssDesc =
    static_cast<vHavokStiffSpringConstraintChainDesc&>(desc);

  if (pSsData)
    desc.m_fChainDamping = float(pSsData->m_damping);

  ssDesc.m_fLinkPivotOffset = m_fLinkPivotOffset;
}

// ----------------------------------------------------------------------------
void vHavokStiffSpringConstraintChain::Serialize( VArchive &ar )
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
void vHavokStiffSpringConstraintChain::UpdateChainPhysicsProperties()
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

  hkpStiffSpringChainData& ssData = 
    static_cast<hkpStiffSpringChainData&>(*m_pConstraintChainData);

  ssData.m_tau = fSolverTau;
  ssData.m_cfm = fCFM;
}

// ----------------------------------------------------------------------------
void vHavokStiffSpringConstraintChain::UpdateConstraintInfo(
  unsigned int iIndex, const hkvVec3& vPivotA, const hkvVec3& vPivotB)
{
  hkpStiffSpringChainData& ssData = 
    static_cast<hkpStiffSpringChainData&>(*m_pConstraintChainData);

  hkpStiffSpringChainData::ConstraintInfo info;
  vHavokConversionUtils::VisVecToPhysVecLocal(vPivotA,info.m_pivotInA);
  vHavokConversionUtils::VisVecToPhysVecLocal(vPivotB,info.m_pivotInB);
  if ((iIndex == 0) || (iIndex == (ssData.m_infos.getSize() - 1)))
    info.m_springLength = hkReal(0);
  else
    info.m_springLength = hkReal(VIS2HK_FLOAT_SCALED(m_fLinkLength) * m_fLinkPivotOffset * 2.f);

  ssData.m_infos[iIndex] = info;
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
