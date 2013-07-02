/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraintChainRenderingData.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraintChain.hpp>

/////////////////////////////////////////////////////////////////////
// vHavokConstraintChainRenderingData
/////////////////////////////////////////////////////////////////////
V_IMPLEMENT_SERIAL(vHavokConstraintChainRenderingData, IVPathRenderingData, 0, &g_vHavokModule);

vHavokConstraintChainRenderingData::vHavokConstraintChainRenderingData()
{
}

vHavokConstraintChainRenderingData::~vHavokConstraintChainRenderingData()
{
}

bool vHavokConstraintChainRenderingData::SetPathObject(VTypedObject* pObject)
{
  if (!pObject->IsOfType(V_RUNTIME_CLASS(vHavokConstraintChain)))
    return false;

  m_spPathObject = (vHavokConstraintChain*)pObject;
  return true;
}

bool vHavokConstraintChainRenderingData::IsValid() const
{
  return m_spPathObject->IsValid();
}

float vHavokConstraintChainRenderingData::GetDiameter() const
{
  return m_spPathObject->GetDiameter();
}

float vHavokConstraintChainRenderingData::GetLinkLength() const
{
  return m_spPathObject->GetLinkLength();
}

float vHavokConstraintChainRenderingData::GetLinkGap() const
{
  return m_spPathObject->GetLinkGap();
}

unsigned int vHavokConstraintChainRenderingData::GetNumLinks() const
{
  return m_spPathObject->GetNumLinks();
}

float vHavokConstraintChainRenderingData::GetLastLinkExtra() const
{
  return m_spPathObject->GetLastLinkExtra();
}

bool vHavokConstraintChainRenderingData::GetLinkTransform(unsigned int iIndex, hkvMat3& mRotation, hkvVec3& vTranslation) const
{
  return m_spPathObject->GetLinkTransform(iIndex, mRotation, vTranslation);
}

void vHavokConstraintChainRenderingData::GetLinkTransforms(hkvMat3* pRotations, hkvVec3* pTranslations, bool bRemoveRoll, bool bReverseOrder) const
{
  return m_spPathObject->GetLinkTransforms(pRotations, pTranslations, bRemoveRoll, bReverseOrder);
}

void vHavokConstraintChainRenderingData::Init(VisTypedEngineObject_cl* pOwner)
{
  VASSERT(m_spPathObject != NULL);

  // nothing to do here
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
