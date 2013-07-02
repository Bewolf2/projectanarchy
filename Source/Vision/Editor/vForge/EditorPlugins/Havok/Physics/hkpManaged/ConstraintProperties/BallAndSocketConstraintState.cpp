/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/HavokPhysicsManagedPCH.h>

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/BallAndSocketConstraintState.hpp>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/ConstraintVisualizer.hpp>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokBallAndSocketConstraint.hpp>

#using <mscorlib.dll>

using namespace ManagedFramework;


namespace HavokManaged
{
  BallAndSocketConstraintState::BallAndSocketConstraintState()
  {
  }

  void BallAndSocketConstraintState::Visualize (IVRenderInterface* pRenderer, const hkvVec3& startPos, const hkvVec3& endPos, const hkvVec3& constraintPos, const hkvVec3& constraintDir, const hkvVec3& constraintOri)
  {
    ConstraintVisualizer::RenderSimpleLine(pRenderer, startPos, endPos);
  }

  vHavokConstraintDesc *BallAndSocketConstraintState::CreateNativeDesc()
  {
    return new vHavokBallAndSocketConstraintDesc();
  }

  /// Copies the properties from the passed native constraint description into this
  /// managed description.
  void BallAndSocketConstraintState::NativeToManaged(const vHavokConstraintDesc &nativeDesc)
  {
    vHavokBallAndSocketConstraintDesc const& bsDesc = (vHavokBallAndSocketConstraintDesc const&)nativeDesc;

    ConstraintStateBase::NativeToManaged(nativeDesc);
  }

  /// Copies the properties from this managed constraint description to the passed native 
  /// description.
  void BallAndSocketConstraintState::ManagedToNative(vHavokConstraintDesc &nativeDesc)
  {
    vHavokBallAndSocketConstraintDesc &bsDesc = (vHavokBallAndSocketConstraintDesc &) nativeDesc;

    // Let the base class do its work
    ConstraintStateBase::ManagedToNative(nativeDesc);
  }

  void BallAndSocketConstraintState::GetObjectData(SerializationInfo ^si, StreamingContext sc)
  {
    ConstraintStateBase::GetObjectData(si, sc);
  }

  BallAndSocketConstraintState::BallAndSocketConstraintState(SerializationInfo ^si, StreamingContext sc) : ConstraintStateBase(si, sc)
  {
  }

  Object^ BallAndSocketConstraintState::Clone()
  {    
    BallAndSocketConstraintState ^pClonedShape = dynamic_cast<BallAndSocketConstraintState^>(ConstraintStateBase::Clone());
    return pClonedShape;
  }

  void BallAndSocketConstraintState::SetAnchorDataOnNative(vHavokConstraintDesc &nativeDesc, 
      AnchorDescription ^pFirstAnchor, AnchorDescription ^pSecondAnchor,
      Vector3F &position, Vector3F &direction, Vector3F &orientation)
  {
    ConstraintStateBase::SetAnchorDataOnNative(
      nativeDesc, pFirstAnchor, pSecondAnchor, position, direction, orientation);

    if (pFirstAnchor)
    {
      VisBaseEntity_cl *pEntity = pFirstAnchor->GetEntity();
      if (pEntity)
      {
        hkvMat4 mTransform;
        pEntity->GetWorldMatrix(mTransform);
        mTransform.setScalingFactors(hkvVec3 (1));   
        mTransform.invert();
        mTransform.setRow (3, hkvVec4 (0, 0, 0, 1));
        nativeDesc.m_vPivots[0] = mTransform * hkvVec3(position.X, position.Y, position.Z);
      }
      else
      {
        nativeDesc.m_vPivots[0] = hkvVec3(position.X, position.Y, position.Z);
      }
    }

    if (pSecondAnchor)
    {
      VisBaseEntity_cl *pEntity = pSecondAnchor->GetEntity();
      if (pEntity)
      {
        hkvMat4 mTransform;
        pEntity->GetWorldMatrix(mTransform);
        mTransform.setScalingFactors(hkvVec3 (1));
        mTransform.invert();
        mTransform.setRow (3, hkvVec4 (0, 0, 0, 1));
        nativeDesc.m_vPivots[1] = mTransform * hkvVec3(position.X, position.Y, position.Z);
      }
      else
      {
        nativeDesc.m_vPivots[1] = hkvVec3(position.X, position.Y, position.Z);
      }
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
