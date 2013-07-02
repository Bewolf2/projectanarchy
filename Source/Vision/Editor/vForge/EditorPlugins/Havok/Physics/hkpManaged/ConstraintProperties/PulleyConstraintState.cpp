/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/HavokPhysicsManagedPCH.h>

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/PulleyConstraintState.hpp>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/ConstraintVisualizer.hpp>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPulleyConstraint.hpp>

#using <mscorlib.dll>
using namespace ManagedFramework;
using namespace CSharpFramework::Actions;
using namespace CSharpFramework::Serialization;

namespace HavokManaged
{
  PulleyConstraintState::PulleyConstraintState()
  {
    float fDistance = 50.0f*EditorManager::Settings->GlobalUnitScaling;
    m_vPulleyPivotA = Vector3F(-fDistance, 0.0f, fDistance);
    m_vPulleyPivotB = Vector3F(fDistance, 0.0f, fDistance);
    m_fLeverage = 1.0f;
    m_fRopeLength = 0.0f;
  }

  void PulleyConstraintState::Visualize (IVRenderInterface* pRenderer, const hkvVec3& startPos, const hkvVec3& endPos, const hkvVec3& constraintPos, const hkvVec3& constraintDir, const hkvVec3& constraintOri)
  {
    hkvVec3 vPivotA = constraintPos + hkvVec3(m_vPulleyPivotA.X, m_vPulleyPivotA.Y, m_vPulleyPivotA.Z);
    hkvVec3 vPivotB = constraintPos + hkvVec3(m_vPulleyPivotB.X, m_vPulleyPivotB.Y, m_vPulleyPivotB.Z);

    ConstraintVisualizer::RenderSimpleLine(pRenderer, startPos, vPivotA);
    ConstraintVisualizer::RenderSimpleLine(pRenderer, vPivotA, vPivotB);
    ConstraintVisualizer::RenderSimpleLine(pRenderer, vPivotB, endPos);

    hkvVec3 vDir = vPivotB-vPivotA;
    vDir.normalizeIfNotZero();
    hkvMat3 mAxisRotation;
    mAxisRotation.setLookInDirectionMatrix (vDir);
    hkvVec3 vRight = mAxisRotation.getAxis(1);
    float fHeight = 30.0f*EditorManager::Settings->GlobalUnitScaling;
    ConstraintVisualizer::RenderCylinder(pRenderer, vPivotA, hkvVec3(vRight.x, vRight.y, vRight.z), fHeight);
    ConstraintVisualizer::RenderCylinder(pRenderer, vPivotB, hkvVec3(vRight.x, vRight.y, vRight.z), fHeight);
  }

  vHavokConstraintDesc *PulleyConstraintState::CreateNativeDesc()
  {
    return new vHavokPulleyConstraintDesc();
  }

  /// Copies the properties from the passed native constraint description into this
  /// managed description.
  void PulleyConstraintState::NativeToManaged(const vHavokConstraintDesc &nativeDesc)
  {
    vHavokPulleyConstraintDesc const& pulleyDesc = 
      (vHavokPulleyConstraintDesc const&)nativeDesc;

    m_vPulleyPivotA = Vector3F(pulleyDesc.m_vPulleyPivots[0].x, pulleyDesc.m_vPulleyPivots[0].y, pulleyDesc.m_vPulleyPivots[0].z);
    m_vPulleyPivotB = Vector3F(pulleyDesc.m_vPulleyPivots[1].x, pulleyDesc.m_vPulleyPivots[1].y, pulleyDesc.m_vPulleyPivots[1].z);

    m_fLeverage = pulleyDesc.m_fLeverage;
    m_fRopeLength = pulleyDesc.m_fRopeLength;

    ConstraintStateBase::NativeToManaged(nativeDesc);
  }

  /// Copies the properties from this managed constraint description to the passed native 
  /// description.
  void PulleyConstraintState::ManagedToNative(vHavokConstraintDesc &nativeDesc)
  {
    vHavokPulleyConstraintDesc &pulleyDesc = (vHavokPulleyConstraintDesc &) nativeDesc;

    pulleyDesc.m_vPulleyPivots[0] = hkvVec3(m_vPulleyPivotA.X, m_vPulleyPivotA.Y, m_vPulleyPivotA.Z);
    pulleyDesc.m_vPulleyPivots[1] = hkvVec3(m_vPulleyPivotB.X, m_vPulleyPivotB.Y, m_vPulleyPivotB.Z);

    pulleyDesc.m_fLeverage = m_fLeverage;
    pulleyDesc.m_fRopeLength = m_fRopeLength;

    // Let the base class do its work
    ConstraintStateBase::ManagedToNative(nativeDesc);
  }

  void PulleyConstraintState::GetObjectData(SerializationInfo ^si, StreamingContext sc)
  {
    ConstraintStateBase::GetObjectData(si, sc);

    si->AddValue("m_vPulleyPivotA", m_vPulleyPivotA);
    si->AddValue("m_vPulleyPivotB", m_vPulleyPivotB);
    si->AddValue("m_fLeverage", m_fLeverage);
    si->AddValue("m_fRopeLength", m_fRopeLength);
  }

  PulleyConstraintState::PulleyConstraintState(SerializationInfo ^si, StreamingContext sc) 
    : ConstraintStateBase(si, sc)
  {
    m_vPulleyPivotA = (Vector3F)si->GetValue("m_vPulleyPivotA", Vector3F::typeid);
    m_vPulleyPivotB = (Vector3F)si->GetValue("m_vPulleyPivotB", Vector3F::typeid);
    m_fLeverage = si->GetSingle("m_fLeverage");
    m_fRopeLength = si->GetSingle("m_fRopeLength");
  }

  Object^ PulleyConstraintState::Clone()
  {
    PulleyConstraintState ^pClonedShape = dynamic_cast<PulleyConstraintState^>(ConstraintStateBase::Clone());

    return pClonedShape;
  }

  void PulleyConstraintState::SetAnchorDataOnNative(vHavokConstraintDesc &nativeDesc, 
    AnchorDescription ^pFirstAnchor, AnchorDescription ^pSecondAnchor, 
    Vector3F &position, Vector3F &direction, Vector3F &orientation)
  {
    VVERIFY_OR_RET(pFirstAnchor);
    VVERIFY_OR_RET(pSecondAnchor);

    ConstraintStateBase::SetAnchorDataOnNative(
      nativeDesc, pFirstAnchor, pSecondAnchor, position, direction, orientation);

    VVERIFY_OR_RET(nativeDesc.m_spBodies[0] || nativeDesc.m_spBodies[1]);
    VVERIFY_OR_RET(nativeDesc.IsOfType(V_RUNTIME_CLASS(vHavokPulleyConstraintDesc)));

    if (pFirstAnchor)
    {
      nativeDesc.m_vPivots[0] = pFirstAnchor->GetWorldSpacePosition();
    }

    if (pSecondAnchor)
    {
      nativeDesc.m_vPivots[1] = pSecondAnchor->GetWorldSpacePosition();
    }

    vHavokPulleyConstraintDesc &pulleyDesc = static_cast<vHavokPulleyConstraintDesc&>(nativeDesc);
    pulleyDesc.m_vPulleyPivots[0] = hkvVec3(position.X, position.Y, position.Z) + 
                                    hkvVec3(m_vPulleyPivotA.X, m_vPulleyPivotA.Y, m_vPulleyPivotA.Z);

    pulleyDesc.m_vPulleyPivots[1] = hkvVec3(position.X, position.Y, position.Z) + 
      hkvVec3(m_vPulleyPivotB.X, m_vPulleyPivotB.Y, m_vPulleyPivotB.Z);
  }

  void PulleyConstraintState::CreateHotspots(Shape3D ^pShape)
  {
    float fSize = 30.0f*EditorManager::Settings->GlobalUnitScaling;

    {
      m_pPulleyPivotAHotSpot = gcnew HotSpot3D(pShape, fSize);
      m_pPulleyPivotAHotSpot->StartPosition = m_vPulleyPivotA;
      m_pPulleyPivotAHotSpot->ToolTipText = "Pulley Pivot A";
      EditorManager::ActiveView->HotSpots->Add(m_pPulleyPivotAHotSpot);
    }

    {
      m_pPulleyPivotBHotSpot = gcnew HotSpot3D(pShape, fSize);
      m_pPulleyPivotBHotSpot->StartPosition = m_vPulleyPivotB;
      m_pPulleyPivotBHotSpot->ToolTipText = "Pulley Pivot B";
      EditorManager::ActiveView->HotSpots->Add(m_pPulleyPivotBHotSpot);
    }
  }

  void PulleyConstraintState::DestroyHotspots(Shape3D ^pShape)
  {
    EditorManager::ActiveView->HotSpots->Remove(m_pPulleyPivotAHotSpot);
    m_pPulleyPivotAHotSpot->Remove();
    m_pPulleyPivotAHotSpot = nullptr;

    EditorManager::ActiveView->HotSpots->Remove(m_pPulleyPivotBHotSpot);
    m_pPulleyPivotBHotSpot->Remove();
    m_pPulleyPivotBHotSpot = nullptr;
  }

  void PulleyConstraintState::OnHotSpotDragBegin(HotSpotBase ^pHotSpot, VisionViewBase ^pView)
  {
    if (pHotSpot == m_pPulleyPivotAHotSpot)
      m_pPulleyPivotAHotSpot->StartPosition = PulleyPivotA;
    else if (pHotSpot == m_pPulleyPivotBHotSpot)
      m_pPulleyPivotBHotSpot->StartPosition = PulleyPivotB;
  }

  void PulleyConstraintState::OnHotSpotDrag(
    HotSpotBase ^pHotSpot, VisionViewBase ^pView, float fDeltaX, float fDeltaY)
  {
    if (pHotSpot == m_pPulleyPivotAHotSpot)
      PulleyPivotA = m_pPulleyPivotAHotSpot->CurrentPosition;
    else if (pHotSpot == m_pPulleyPivotBHotSpot)
      PulleyPivotB = m_pPulleyPivotBHotSpot->CurrentPosition;
  }

  void PulleyConstraintState::OnHotSpotDragEnd(HotSpotBase ^pHotSpot, VisionViewBase ^pView)
  {
    if (pHotSpot == m_pPulleyPivotAHotSpot) 
    {
      Vector3F vNewPosition = m_pPulleyPivotAHotSpot->CurrentPosition;
      PulleyPivotA = m_pPulleyPivotAHotSpot->StartPosition; // set old position for the action
      EditorManager::Actions->Add(SetPropertyAction::CreateSetPropertyAction(this, "PulleyPivotA", vNewPosition)); // send an action which sets the property from old value to new one
    }
    if (pHotSpot == m_pPulleyPivotBHotSpot) 
    {
      Vector3F vNewPosition = m_pPulleyPivotBHotSpot->CurrentPosition;
      PulleyPivotB = m_pPulleyPivotBHotSpot->StartPosition; // set old position for the action
      EditorManager::Actions->Add(SetPropertyAction::CreateSetPropertyAction(this, "PulleyPivotB", vNewPosition)); // send an action which sets the property from old value to new one
    }
  }

  void PulleyConstraintState::OnHotSpotEvaluatePosition(HotSpotBase ^pHotSpot)
  { 
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
