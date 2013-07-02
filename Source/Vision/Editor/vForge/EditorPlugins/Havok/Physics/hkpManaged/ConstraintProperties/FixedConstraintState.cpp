/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/HavokPhysicsManagedPCH.h>

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/FixedConstraintState.hpp>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/ConstraintVisualizer.hpp>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokGenericConstraint.hpp>

#using <mscorlib.dll>
using namespace ManagedFramework;
using namespace CSharpFramework::Serialization;

namespace HavokManaged
{
  FixedConstraintState::FixedConstraintState()
  {
  }

  void FixedConstraintState::Visualize (IVRenderInterface* pRenderer, const hkvVec3& startPos, const hkvVec3& endPos, const hkvVec3& constraintPos, const hkvVec3& constraintDir, const hkvVec3& constraintOri)
  {
    ConstraintVisualizer::RenderSimpleLine(pRenderer, startPos, endPos);
  }

  vHavokConstraintDesc *FixedConstraintState::CreateNativeDesc()
  {
    return new vHavokGenericConstraintDesc();
  }

  /// Copies the properties from the passed native constraint description into this
  /// managed description.
  void FixedConstraintState::NativeToManaged(const vHavokConstraintDesc &nativeDesc)
  {
    ConstraintStateBase::NativeToManaged(nativeDesc);
  }

  /// Copies the properties from this managed constraint description to the passed native 
  /// description.
  void FixedConstraintState::ManagedToNative(vHavokConstraintDesc &nativeDesc)
  {
    // Let the base class do its work
    ConstraintStateBase::ManagedToNative(nativeDesc);
  }

  void FixedConstraintState::GetObjectData(SerializationInfo ^si, StreamingContext sc)
  {
    ConstraintStateBase::GetObjectData(si, sc);
  }

  FixedConstraintState::FixedConstraintState(SerializationInfo ^si, StreamingContext sc) 
    : ConstraintStateBase(si, sc)
  {
  }

  Object^ FixedConstraintState::Clone()
  {    
    FixedConstraintState ^pClonedShape = dynamic_cast<FixedConstraintState^>(ConstraintStateBase::Clone());

    return pClonedShape;
  }

  void FixedConstraintState::SetAnchorDataOnNative(vHavokConstraintDesc &nativeDesc, 
    AnchorDescription ^pFirstAnchor, AnchorDescription ^pSecondAnchor, 
    Vector3F &position, Vector3F &direction, Vector3F &orientation)
  {
    VVERIFY_OR_RET(pFirstAnchor);
    VVERIFY_OR_RET(pSecondAnchor);

    ConstraintStateBase::SetAnchorDataOnNative(
      nativeDesc, pFirstAnchor, pSecondAnchor, position, direction, orientation);

    VVERIFY_OR_RET(nativeDesc.m_spBodies[0] || nativeDesc.m_spBodies[1]);
    VVERIFY_OR_RET(nativeDesc.IsOfType(V_RUNTIME_CLASS(vHavokGenericConstraintDesc)));

    vHavokGenericConstraintDesc &genericDesc = static_cast<vHavokGenericConstraintDesc&>(nativeDesc);
    genericDesc.m_vGenericPivot = hkvVec3 (position.X, position.Y, position.Z);
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
