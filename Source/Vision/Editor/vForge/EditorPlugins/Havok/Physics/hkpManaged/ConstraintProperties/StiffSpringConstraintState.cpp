/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/HavokPhysicsManagedPCH.h>

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/StiffSpringConstraintState.hpp>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/ConstraintVisualizer.hpp>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokStiffSpringConstraint.hpp>

#using <mscorlib.dll>

using namespace ManagedFramework;


namespace HavokManaged
{
  StiffSpringConstraintState::StiffSpringConstraintState()
  {
  }

  void StiffSpringConstraintState::Visualize (IVRenderInterface* pRenderer, const hkvVec3& startPos, const hkvVec3& endPos, const hkvVec3& constraintPos, const hkvVec3& constraintDir, const hkvVec3& constraintOri)
  {
    ConstraintVisualizer::RenderSimpleLine(pRenderer, startPos, endPos);
  }

  vHavokConstraintDesc *StiffSpringConstraintState::CreateNativeDesc()
  {
    return new vHavokStiffSpringConstraintDesc();
  }

  /// Copies the properties from the passed native constraint description into this
  /// managed description.
  void StiffSpringConstraintState::NativeToManaged(const vHavokConstraintDesc &nativeDesc)
  {
    vHavokStiffSpringConstraintDesc const& bsDesc = (vHavokStiffSpringConstraintDesc const&)nativeDesc;

    ConstraintStateBase::NativeToManaged(nativeDesc);
  }

  /// Copies the properties from this managed constraint description to the passed native 
  /// description.
  void StiffSpringConstraintState::ManagedToNative(vHavokConstraintDesc &nativeDesc)
  {
    vHavokStiffSpringConstraintDesc &bsDesc = (vHavokStiffSpringConstraintDesc &) nativeDesc;

    // Let the base class do its work
    ConstraintStateBase::ManagedToNative(nativeDesc);
  }

  void StiffSpringConstraintState::GetObjectData(SerializationInfo ^si, StreamingContext sc)
  {
    ConstraintStateBase::GetObjectData(si, sc);
  }

  StiffSpringConstraintState::StiffSpringConstraintState(SerializationInfo ^si, StreamingContext sc) : ConstraintStateBase(si, sc)
  {
  }

  Object^ StiffSpringConstraintState::Clone()
  {    
    StiffSpringConstraintState ^pClonedShape = dynamic_cast<StiffSpringConstraintState^>(ConstraintStateBase::Clone());
    return pClonedShape;
  }

  void StiffSpringConstraintState::SetAnchorDataOnNative(vHavokConstraintDesc &nativeDesc, 
      AnchorDescription ^pFirstAnchor, AnchorDescription ^pSecondAnchor,
      Vector3F &position, Vector3F &direction, Vector3F &orientation)
  {
    ConstraintStateBase::SetAnchorDataOnNative(
      nativeDesc, pFirstAnchor, pSecondAnchor, position, direction, orientation);

    if (pFirstAnchor)
    {
      nativeDesc.m_vPivots[0] = pFirstAnchor->GetWorldSpacePosition();
    }

    if (pSecondAnchor)
    {
      nativeDesc.m_vPivots[1] = pSecondAnchor->GetWorldSpacePosition();
    }
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
