/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/HavokPhysicsManagedPCH.h>

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/HingeConstraintState.hpp>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/ConstraintVisualizer.hpp>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokHingeConstraint.hpp>

#using <mscorlib.dll>
using namespace ManagedFramework;
using namespace CSharpFramework::Serialization;

namespace HavokManaged
{
  HingeConstraintState::HingeConstraintState()
  {
    m_pLimitsDesc = gcnew ConstraintLimitsDesc();
  }

  void HingeConstraintState::Visualize (IVRenderInterface* pRenderer, const hkvVec3& startPos, const hkvVec3& endPos, const hkvVec3& constraintPos, const hkvVec3& constraintDir, const hkvVec3& constraintOri)
  {
    const float fLineLength = 30.f;
    hkvVec3 constraintDirLine(constraintDir);
    constraintDirLine.normalizeIfNotZero();
    constraintDirLine *= fLineLength;

    ConstraintVisualizer::RenderSimpleLine(pRenderer, constraintPos, constraintPos + constraintDirLine);
    ConstraintVisualizer::RenderSimpleLine(pRenderer, constraintPos, constraintPos - constraintDirLine);
  }

  vHavokConstraintDesc *HingeConstraintState::CreateNativeDesc()
  {
    return new vHavokHingeConstraintDesc();
  }

  /// Copies the properties from the passed native constraint description into this
  /// managed description.
  void HingeConstraintState::NativeToManaged(const vHavokConstraintDesc &nativeDesc)
  {
    vHavokHingeConstraintDesc const& bsDesc = (vHavokHingeConstraintDesc const&)nativeDesc;

    VVERIFY_OR_RET(m_pLimitsDesc);
    m_pLimitsDesc->UseLimits = bsDesc.m_bUseLimits;
    m_pLimitsDesc->LimitMax = static_cast<float>(bsDesc.m_fAngleMax);
    m_pLimitsDesc->LimitMin = static_cast<float>(bsDesc.m_fAngleMin);

    ConstraintStateBase::NativeToManaged(nativeDesc);
  }

  /// Copies the properties from this managed constraint description to the passed native 
  /// description.
  void HingeConstraintState::ManagedToNative(vHavokConstraintDesc &nativeDesc)
  {
    vHavokHingeConstraintDesc &bsDesc = (vHavokHingeConstraintDesc &) nativeDesc;

    VVERIFY_OR_RET(m_pLimitsDesc);
    bsDesc.m_bUseLimits = m_pLimitsDesc->UseLimits;
    bsDesc.m_fAngleMax = m_pLimitsDesc->LimitMax;
    bsDesc.m_fAngleMin = m_pLimitsDesc->LimitMin;

    // Let the base class do its work
    ConstraintStateBase::ManagedToNative(nativeDesc);
  }

  void HingeConstraintState::GetObjectData(SerializationInfo ^si, StreamingContext sc)
  {
    ConstraintStateBase::GetObjectData(si, sc);

    si->AddValue("m_pLimitsDesc", this->m_pLimitsDesc);
  }

  HingeConstraintState::HingeConstraintState(SerializationInfo ^si, StreamingContext sc) 
    : ConstraintStateBase(si, sc)
  {
    if (SerializationHelper::HasElement(si, "m_pLimitsDesc"))
      m_pLimitsDesc = dynamic_cast<ConstraintLimitsDesc^> (si->GetValue("m_pLimitsDesc", ConstraintLimitsDesc::typeid));
    else
      m_pLimitsDesc = gcnew ConstraintLimitsDesc;
  }

  Object^ HingeConstraintState::Clone()
  {    
    HingeConstraintState ^pClonedShape = dynamic_cast<HingeConstraintState^>(ConstraintStateBase::Clone());

    pClonedShape->m_pLimitsDesc = dynamic_cast<ConstraintLimitsDesc^>(m_pLimitsDesc->Clone());

    return pClonedShape;
  }

  void HingeConstraintState::SetAnchorDataOnNative(vHavokConstraintDesc &nativeDesc, 
    AnchorDescription ^pFirstAnchor, AnchorDescription ^pSecondAnchor, 
    Vector3F &position, Vector3F &direction, Vector3F &orientation)
  {
    VVERIFY_OR_RET(pFirstAnchor);
    VVERIFY_OR_RET(pSecondAnchor);

    ConstraintStateBase::SetAnchorDataOnNative(
      nativeDesc, pFirstAnchor, pSecondAnchor, position, direction, orientation);

    VVERIFY_OR_RET(nativeDesc.m_spBodies[0] || nativeDesc.m_spBodies[1]);
    VVERIFY_OR_RET(nativeDesc.IsOfType(V_RUNTIME_CLASS(vHavokHingeConstraintDesc)));

    vHavokHingeConstraintDesc &lhDesc = static_cast<vHavokHingeConstraintDesc&>(nativeDesc);
    lhDesc.m_vHingePivot = hkvVec3 (position.X, position.Y, position.Z);
    lhDesc.m_vRotAxis = hkvVec3 (direction.X, direction.Y, direction.Z);
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
