/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/HavokPhysicsManagedPCH.h>

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/PrismaticConstraintState.hpp>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/ConstraintVisualizer.hpp>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPrismaticConstraint.hpp>

#using <mscorlib.dll>
using namespace ManagedFramework;
using namespace CSharpFramework::Serialization;

namespace HavokManaged
{
  PrismaticConstraintState::PrismaticConstraintState(bool bAllowRotationAroundAxis)
  {
    m_fLinearMin = -FLT_MAX;
    m_fLinearMax = FLT_MAX;
    m_bAllowRotationAroundAxis = bAllowRotationAroundAxis;
  }

  void PrismaticConstraintState::Visualize (IVRenderInterface* pRenderer, const hkvVec3& startPos, const hkvVec3& endPos, const hkvVec3& constraintPos, const hkvVec3& constraintDir, const hkvVec3& constraintOri)
  {
    const float fLength = 50.f*EditorManager::Settings->GlobalUnitScaling;
    hkvVec3 constraintDirLine(constraintDir);
    constraintDirLine.normalizeIfNotZero();
    constraintDirLine *= fLength;

    if (m_bAllowRotationAroundAxis)
      ConstraintVisualizer::RenderCylinder(pRenderer, constraintPos, constraintDir, fLength*2.0f);
    else
      ConstraintVisualizer::RenderPrismaticBoxes(pRenderer,constraintPos-constraintDirLine, constraintPos+constraintDirLine);
  }

  vHavokConstraintDesc *PrismaticConstraintState::CreateNativeDesc()
  {
    return new vHavokPrismaticConstraintDesc();
  }

  /// Copies the properties from the passed native constraint description into this
  /// managed description.
  void PrismaticConstraintState::NativeToManaged(const vHavokConstraintDesc &nativeDesc)
  {
    vHavokPrismaticConstraintDesc const& prismaticDesc = (vHavokPrismaticConstraintDesc const&)nativeDesc;

    m_fLinearMin = static_cast<float>(prismaticDesc.m_fLinearMin);
    m_fLinearMax = static_cast<float>(prismaticDesc.m_fLinearMax);
    m_bAllowRotationAroundAxis = prismaticDesc.m_bAllowRotationAroundAxis;

    ConstraintStateBase::NativeToManaged(nativeDesc);
  }

  /// Copies the properties from this managed constraint description to the passed native 
  /// description.
  void PrismaticConstraintState::ManagedToNative(vHavokConstraintDesc &nativeDesc)
  {
    vHavokPrismaticConstraintDesc &prismaticDesc = (vHavokPrismaticConstraintDesc &) nativeDesc;

    prismaticDesc.m_fLinearMin = m_fLinearMin;
    prismaticDesc.m_fLinearMax = m_fLinearMax;
    prismaticDesc.m_bAllowRotationAroundAxis = m_bAllowRotationAroundAxis;

    // Let the base class do its work
    ConstraintStateBase::ManagedToNative(nativeDesc);
  }

  void PrismaticConstraintState::GetObjectData(SerializationInfo ^si, StreamingContext sc)
  {
    ConstraintStateBase::GetObjectData(si, sc);

    si->AddValue(("m_fLinearMin"), m_fLinearMin);
    si->AddValue(("m_fLinearMax"), m_fLinearMax);
    si->AddValue(("m_bAllowRotationAroundAxis"), m_bAllowRotationAroundAxis);
  }

  PrismaticConstraintState::PrismaticConstraintState(SerializationInfo ^si, StreamingContext sc) 
    : ConstraintStateBase(si, sc)
  {
    m_fLinearMin = si->GetSingle("m_fLinearMin");
    m_fLinearMax = si->GetSingle("m_fLinearMax");
    m_bAllowRotationAroundAxis= si->GetSingle("m_bAllowRotationAroundAxis");
  }

  Object^ PrismaticConstraintState::Clone()
  {    
    PrismaticConstraintState ^pClonedShape = dynamic_cast<PrismaticConstraintState^>(ConstraintStateBase::Clone());

    return pClonedShape;
  }

  void PrismaticConstraintState::SetAnchorDataOnNative(vHavokConstraintDesc &nativeDesc, 
    AnchorDescription ^pFirstAnchor, AnchorDescription ^pSecondAnchor, 
    Vector3F &position, Vector3F &direction, Vector3F &orientation)
  {
    VVERIFY_OR_RET(pFirstAnchor);
    VVERIFY_OR_RET(pSecondAnchor);

    ConstraintStateBase::SetAnchorDataOnNative(
      nativeDesc, pFirstAnchor, pSecondAnchor, position, direction, orientation);

    VVERIFY_OR_RET(nativeDesc.m_spBodies[0] || nativeDesc.m_spBodies[1]);
    VVERIFY_OR_RET(nativeDesc.IsOfType(V_RUNTIME_CLASS(vHavokPrismaticConstraintDesc)));

    vHavokPrismaticConstraintDesc &prismaticDesc = static_cast<vHavokPrismaticConstraintDesc&>(nativeDesc);
    prismaticDesc.m_vPrismaticPivot = hkvVec3 (position.X, position.Y, position.Z);
    prismaticDesc.m_vPrismaticAxis = hkvVec3 (direction.X, direction.Y, direction.Z);
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
