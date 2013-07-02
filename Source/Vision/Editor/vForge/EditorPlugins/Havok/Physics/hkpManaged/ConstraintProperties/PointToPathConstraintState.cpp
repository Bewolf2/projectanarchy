/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/HavokPhysicsManagedPCH.h>

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/PointToPathConstraintState.hpp>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/ConstraintVisualizer.hpp>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPointToPathConstraint.hpp>

#using <mscorlib.dll>
using namespace ManagedFramework;
using namespace CSharpFramework::Serialization;

namespace HavokManaged
{
  PointToPathConstraintState::PointToPathConstraintState()
  {
    m_fPathLength = 200.0f*EditorManager::Settings->GlobalUnitScaling;
    m_fFriction = 0.0f;
  }

  void PointToPathConstraintState::Visualize (IVRenderInterface* pRenderer, const hkvVec3& startPos, const hkvVec3& endPos, const hkvVec3& constraintPos, const hkvVec3& constraintDir, const hkvVec3& constraintOri)
  {
    ConstraintVisualizer::RenderSimpleLine(pRenderer, constraintPos, constraintPos + constraintDir*m_fPathLength);
  }

  vHavokConstraintDesc *PointToPathConstraintState::CreateNativeDesc()
  {
    return new vHavokPointToPathConstraintDesc();
  }

  /// Copies the properties from the passed native constraint description into this
  /// managed description.
  void PointToPathConstraintState::NativeToManaged(const vHavokConstraintDesc &nativeDesc)
  {
    vHavokPointToPathConstraintDesc const& pointToPathDesc = (vHavokPointToPathConstraintDesc const&)nativeDesc;

    m_fPathLength = pointToPathDesc.m_fPathLength;
    m_fFriction = pointToPathDesc.m_fFriction;

    ConstraintStateBase::NativeToManaged(nativeDesc);
  }

  /// Copies the properties from this managed constraint description to the passed native 
  /// description.
  void PointToPathConstraintState::ManagedToNative(vHavokConstraintDesc &nativeDesc)
  {
    vHavokPointToPathConstraintDesc &pointToPathDesc = (vHavokPointToPathConstraintDesc &) nativeDesc;

    pointToPathDesc.m_fPathLength = m_fPathLength;
    pointToPathDesc.m_fFriction = m_fFriction;

    // Let the base class do its work
    ConstraintStateBase::ManagedToNative(nativeDesc);
  }

  void PointToPathConstraintState::GetObjectData(SerializationInfo ^si, StreamingContext sc)
  {
    ConstraintStateBase::GetObjectData(si, sc);

    si->AddValue(("m_fPathLength"), m_fPathLength);
    si->AddValue(("m_fFriction"), m_fFriction);
  }

  PointToPathConstraintState::PointToPathConstraintState(SerializationInfo ^si, StreamingContext sc) 
    : ConstraintStateBase(si, sc)
  {
    m_fPathLength = si->GetSingle("m_fPathLength");
    m_fFriction = si->GetSingle("m_fFriction");
  }

  Object^ PointToPathConstraintState::Clone()
  {    
    PointToPathConstraintState ^pClonedShape = dynamic_cast<PointToPathConstraintState^>(ConstraintStateBase::Clone());

    return pClonedShape;
  }

  void PointToPathConstraintState::SetAnchorDataOnNative(vHavokConstraintDesc &nativeDesc, 
    AnchorDescription ^pFirstAnchor, AnchorDescription ^pSecondAnchor, 
    Vector3F &position, Vector3F &direction, Vector3F &orientation)
  {
    VVERIFY_OR_RET(pFirstAnchor);
    VVERIFY_OR_RET(pSecondAnchor);

    ConstraintStateBase::SetAnchorDataOnNative(
      nativeDesc, pFirstAnchor, pSecondAnchor, position, direction, orientation);

    VVERIFY_OR_RET(nativeDesc.m_spBodies[0] || nativeDesc.m_spBodies[1]);
    VVERIFY_OR_RET(nativeDesc.IsOfType(V_RUNTIME_CLASS(vHavokPointToPathConstraintDesc)));

    vHavokPointToPathConstraintDesc &pointToPathDesc = static_cast<vHavokPointToPathConstraintDesc&>(nativeDesc);
    pointToPathDesc.m_vPathPivot = hkvVec3 (position.X, position.Y, position.Z);
    pointToPathDesc.m_vPathDirection = hkvVec3 (direction.X, direction.Y, direction.Z);
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
