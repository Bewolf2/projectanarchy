/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/HavokPhysicsManagedPCH.h>

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/RagdollConstraintState.hpp>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/ConstraintVisualizer.hpp>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRagdollConstraint.hpp>

#using <mscorlib.dll>
using namespace ManagedFramework;
using namespace CSharpFramework::Actions;
using namespace CSharpFramework::Serialization;

namespace HavokManaged
{
  RagdollConstraintState::RagdollConstraintState()
  {
    m_fConeAngle = 60.f;
    m_fPlaneConeMinAngle = 60.f;
    m_fPlaneConeMaxAngle = 60.f;
    m_fTwistMinAngle = -180.f;
    m_fTwistMaxAngle = 180.f;
    m_bConeLimitStabilization = false;
    m_fMaxFrictionTorque = 0.f;
    m_fAngularLimitsTauFactor = 0.8f;
  }

  bool LineConeIntersection(IVRenderInterface *pRenderer, const hkvVec3 &vConePos, const hkvVec3 &vConeAxis, float fConeAngle, 
                            const hkvVec3 &vLineStart, const hkvVec3 &vLineDir, hkvVec3 &vIntersection)
  {  
    // cull back cone
    float fConeLineAngle = vConeAxis.dot(vLineDir);
    if (fConeLineAngle < 0)
      return false;

    float fCosAngle = hkvMath::cosDeg(fConeAngle);
    float fCosSquared = fCosAngle*fCosAngle;
    hkvVec3 vLineToCone = vLineStart - vConePos;
    float fDotA = vConeAxis.dot(vLineToCone);
    float fDotB = vLineDir.dot(vLineToCone);
    float fDotC = vLineToCone.dot(vLineToCone);

    float a = fConeLineAngle*fConeLineAngle - fCosSquared;
    float b = fConeLineAngle*fDotA - fCosSquared*fDotB;
    float c = fDotA*fDotA - fCosSquared*fDotC;

    float fInnerValue = b*b-4*a*c;
    float fDenominator = 2*a;
    if(fInnerValue < 0.0f || fDenominator == 0.0f)
      return false;

    float fSqrtInnerValue = sqrt(fInnerValue);
    float t1 = (-b + fSqrtInnerValue)/fDenominator;
    float t2 = (-b - fSqrtInnerValue)/fDenominator;

    // choose front cone 
    float t = (t1 > t2) ? t1 : t2;

    vIntersection = vLineStart+vLineDir*t;

    return true;
  }

  void RenderHyperbolicCone(IVRenderInterface* pRenderer, const hkvVec3 &vConePos, const hkvVec3 &vConeAxis, float fConeAngle, float fConeLength,
                            hkvVec3 &vPlainAxis, float fPlainMinAngle, float fPlainMaxAngle)
  {
    hkvVec3 vRight,vUp;
    if (hkvMath::Abs(vConeAxis.z) > 0.5f)
      vUp.set(1.0f, 0.0f, 0.0f); 
    else 
      vUp.set(0.0f, 0.0f, 1.0f);
    vRight = vUp;
    vRight = vRight.cross(vConeAxis);
    vUp = vConeAxis;
    vUp = vUp.cross(vRight);
    vRight.normalizeIfNotZero();
    vUp.normalizeIfNotZero();  
    
    const int iSubDiv = 64;
    hkvVec3 vVertices[iSubDiv];

    hkvVec3 vEnd(hkvNoInitialization);
    float w = 0.0f;
    float dw = hkvMath::pi()*2.0f/(float)iSubDiv;
    float fRadius;
    if (hkvMath::isFloatEqual(fConeAngle, 180.0f))
    {
      vEnd = vConePos;
      fRadius = fConeLength;
    } 
    else
    {
      fRadius = hkvMath::tanDeg(fConeAngle*0.5f) * fConeLength;
      if (fRadius<0.0f) // angles > 180 deg
      {
        fRadius = -fRadius;
        fConeLength = -fConeLength;
      }
      vEnd = vConePos + fConeLength*vConeAxis;
    }

    for (int x=0;x<iSubDiv;x++,w+=dw)
    {
      hkvVec3 vLineDir = fRadius*(vRight*hkvMath::sinRad(w)+vUp*hkvMath::cosRad(w)); 
      vVertices[x] = vEnd + vLineDir; 
      vLineDir.normalize();

      hkvVec3 vIntersection;
      if (LineConeIntersection(pRenderer, vConePos, vPlainAxis, fPlainMaxAngle*0.5f, vEnd, vLineDir, vIntersection))
      {
        if((vIntersection-vEnd).getLength()<fRadius)
          vVertices[x] = vIntersection;
      }
      else if(LineConeIntersection(pRenderer, vConePos, -vPlainAxis, fPlainMinAngle*0.5f, vEnd, vLineDir, vIntersection))
      {
        if((vIntersection-vEnd).getLength()<fRadius)
          vVertices[x] = vIntersection;
      }
    }

    VColorRef iColor(0, 0, 255, 60);
    for (int i=0;i<iSubDiv;i++)
    {
      VSimpleRenderState_t state(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_DOUBLESIDED);
      pRenderer->DrawLine(vConePos, vVertices[i], iColor, 1.0f, state);
      int iNext= (i+1) % iSubDiv;
      pRenderer->DrawLine(vVertices[i], vVertices[iNext], iColor, 1.0f, state);
      pRenderer->DrawTriangle(vConePos, vVertices[i], vVertices[iNext], iColor, state);
    }
  }

  void RagdollConstraintState::Visualize (IVRenderInterface* pRenderer, const hkvVec3& startPos, const hkvVec3& endPos, const hkvVec3& constraintPos, const hkvVec3& constraintDir, const hkvVec3& constraintOri)
  {
    VSimpleRenderState_t state(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_DOUBLESIDED);
    int iFlags = RENDERSHAPEFLAGS_LINES|RENDERSHAPEFLAGS_SOLID;

    float fConeLength = 50.f * EditorManager::Settings->GlobalUnitScaling;

    hkvVec3 vPlaneAxisVec;
    hkvVec3 vTwistAxisVec;
    CalcRagdollAxes(constraintOri, vTwistAxisVec, vPlaneAxisVec);
    hkvVec3 vTwistAxis(vTwistAxisVec.x, vTwistAxisVec.y, vTwistAxisVec.z);
    hkvVec3 vPlaneAxis(vPlaneAxisVec.x, vPlaneAxisVec.y, vPlaneAxisVec.z);
    
    RenderHyperbolicCone(pRenderer, constraintPos, vTwistAxis, ConeAngle, fConeLength, vPlaneAxisVec, PlaneConeMinAngle, PlaneConeMaxAngle);
    pRenderer->DrawLine(constraintPos, constraintPos+(vTwistAxis*fConeLength*1.5f), V_RGBA_RED, 1.0f, state);

    VColorRef iColor(127, 127, 127, 60);
    if (m_pPlaneAngleMinHotSpot && m_pPlaneAngleMinHotSpot->IsDragging)
      pRenderer->RenderCone(constraintPos, -vPlaneAxis, PlaneConeMinAngle, fConeLength, iColor, state, iFlags);

    if (m_pPlaneAngleMaxHotSpot && m_pPlaneAngleMaxHotSpot->IsDragging)
      pRenderer->RenderCone(constraintPos, vPlaneAxis, PlaneConeMaxAngle, fConeLength, iColor, state, iFlags);
  }

  vHavokConstraintDesc *RagdollConstraintState::CreateNativeDesc()
  {
    return new vHavokRagdollConstraintDesc();
  }

  /// Copies the properties from the passed native constraint description into this
  /// managed description.
  void RagdollConstraintState::NativeToManaged(const vHavokConstraintDesc &nativeDesc)
  {
    vHavokRagdollConstraintDesc const& ragdollDesc = 
      (vHavokRagdollConstraintDesc const&)nativeDesc;

    m_fConeAngle = hkvMath::Rad2Deg(ragdollDesc.m_fConeAngle * 2.f);

    float fAngle = -((90.f - ragdollDesc.m_fPlaneConeMinAngle) * 2.f);
    m_fPlaneConeMinAngle = hkvMath::Rad2Deg(fAngle);
    fAngle = ((90.f - ragdollDesc.m_fPlaneConeMaxAngle) * 2.f);
    m_fPlaneConeMaxAngle = hkvMath::Rad2Deg(fAngle);

    m_fTwistMinAngle = hkvMath::Rad2Deg(ragdollDesc.m_fTwistMinAngle);
    m_fTwistMaxAngle = hkvMath::Rad2Deg(ragdollDesc.m_fTwistMaxAngle);

    m_bConeLimitStabilization = ragdollDesc.m_bConeLimitStabilization;
    m_fMaxFrictionTorque = ragdollDesc.m_fMaxFrictionTorque;
    m_fAngularLimitsTauFactor = ragdollDesc.m_fAngularLimitsTauFactor;

    ConstraintStateBase::NativeToManaged(nativeDesc);
  }

  /// Copies the properties from this managed constraint description to the passed native 
  /// description.
  void RagdollConstraintState::ManagedToNative(vHavokConstraintDesc &nativeDesc)
  {
    vHavokRagdollConstraintDesc &ragdollDesc = (vHavokRagdollConstraintDesc &) nativeDesc;

    ragdollDesc.m_fConeAngle = hkvMath::Deg2Rad(m_fConeAngle / 2.f);

    float fAngle = -(90.f - (m_fPlaneConeMinAngle / 2.f));
    ragdollDesc.m_fPlaneConeMinAngle = hkvMath::Deg2Rad(fAngle);
    fAngle = 90.f - (m_fPlaneConeMaxAngle / 2.f);
    ragdollDesc.m_fPlaneConeMaxAngle = hkvMath::Deg2Rad(fAngle);

    ragdollDesc.m_fTwistMinAngle = hkvMath::Deg2Rad(m_fTwistMinAngle);
    ragdollDesc.m_fTwistMaxAngle = hkvMath::Deg2Rad(m_fTwistMaxAngle);

    ragdollDesc.m_bConeLimitStabilization = m_bConeLimitStabilization;
    ragdollDesc.m_fMaxFrictionTorque = m_fMaxFrictionTorque;
    ragdollDesc.m_fAngularLimitsTauFactor = m_fAngularLimitsTauFactor;

    // Let the base class do its work
    ConstraintStateBase::ManagedToNative(nativeDesc);
  }

  void RagdollConstraintState::GetObjectData(SerializationInfo ^si, StreamingContext sc)
  {
    ConstraintStateBase::GetObjectData(si, sc);

    si->AddValue("m_fConeAngle", m_fConeAngle);
    si->AddValue("m_fPlaneConeMinAngle", m_fPlaneConeMinAngle);
    si->AddValue("m_fPlaneConeMaxAngle", m_fPlaneConeMaxAngle);
    si->AddValue("m_fTwistMinAngle", m_fTwistMinAngle);
    si->AddValue("m_fTwistMaxAngle", m_fTwistMaxAngle);
    si->AddValue("m_bConeLimitStabilization", m_bConeLimitStabilization);
    si->AddValue("m_fMaxFrictionTorque", m_fMaxFrictionTorque);
    si->AddValue("m_fAngularLimitsTauFactor", m_fAngularLimitsTauFactor);
  }

  RagdollConstraintState::RagdollConstraintState(SerializationInfo ^si, StreamingContext sc) 
    : ConstraintStateBase(si, sc)
  {
    m_fConeAngle = si->GetSingle("m_fConeAngle");
    m_fPlaneConeMinAngle = si->GetSingle("m_fPlaneConeMinAngle");
    m_fPlaneConeMaxAngle = si->GetSingle("m_fPlaneConeMaxAngle");
    m_fTwistMinAngle = si->GetSingle("m_fTwistMinAngle");
    m_fTwistMaxAngle = si->GetSingle("m_fTwistMaxAngle");
    m_bConeLimitStabilization = si->GetBoolean("m_bConeLimitStabilization");
    m_fMaxFrictionTorque = si->GetSingle("m_fMaxFrictionTorque");
    m_fAngularLimitsTauFactor = si->GetSingle("m_fAngularLimitsTauFactor");
  }

  Object^ RagdollConstraintState::Clone()
  {
    RagdollConstraintState ^pClonedShape = dynamic_cast<RagdollConstraintState^>(ConstraintStateBase::Clone());

    return pClonedShape;
  }

  void RagdollConstraintState::SetAnchorDataOnNative(vHavokConstraintDesc &nativeDesc, 
    AnchorDescription ^pFirstAnchor, AnchorDescription ^pSecondAnchor, 
    Vector3F &position, Vector3F &direction, Vector3F &orientation)
  {
    VVERIFY_OR_RET(pFirstAnchor);
    VVERIFY_OR_RET(pSecondAnchor);

    ConstraintStateBase::SetAnchorDataOnNative(
      nativeDesc, pFirstAnchor, pSecondAnchor, position, direction, orientation);

    VVERIFY_OR_RET(nativeDesc.m_spBodies[0] || nativeDesc.m_spBodies[1]);
    VVERIFY_OR_RET(nativeDesc.IsOfType(V_RUNTIME_CLASS(vHavokRagdollConstraintDesc)));

    vHavokRagdollConstraintDesc &ragdollDesc = static_cast<vHavokRagdollConstraintDesc&>(nativeDesc);
    ragdollDesc.m_vPivot = hkvVec3(position.X, position.Y, position.Z);

    hkvVec3 vOrientation(orientation.X, orientation.Y, orientation.Z);
    CalcRagdollAxes(vOrientation, ragdollDesc.m_vTwistAxis, ragdollDesc.m_vPlaneAxis);
  }

  void RagdollConstraintState::CalcRagdollAxes(const hkvVec3& vOrientation, hkvVec3& vTwistAxis, hkvVec3& vPlaneAxis)
  {
    hkvMat3 mRot (hkvNoInitialization);
    mRot.setFromEulerAngles (vOrientation.z, vOrientation.y, vOrientation.x);

    vTwistAxis = mRot * hkvVec3(1.f, 0.f, 0.f);
    vPlaneAxis = mRot * hkvVec3(0.f, 1.f, 0.f);
  }


  void RagdollConstraintState::CreateHotspots(Shape3D ^pShape)
  {
    float fHotSpotdistance = 100.f * EditorManager::Settings->GlobalUnitScaling;

    {
      m_pConeAngleHotSpot = gcnew HotSpotConeAngle(
        pShape, "textures\\Hotspot_A.tga", VisionColors::Red, 
        HotSpotBase::PickType::Square, 4.0f);
      m_pConeAngleHotSpot->Distance = fHotSpotdistance;
      m_pConeAngleHotSpot->StartAngle = m_fConeAngle;
      m_pConeAngleHotSpot->ToolTipText = "Twist axis cone angle";
      EditorManager::ActiveView->HotSpots->Add(m_pConeAngleHotSpot);
    }

    {
      m_pPlaneAngleMaxHotSpot = gcnew HotSpotConeAngle(
        pShape, "textures\\Hotspot_A.tga", VisionColors::Red, 
        HotSpotBase::PickType::Square, 4.0f);
      m_pPlaneAngleMaxHotSpot->Distance = fHotSpotdistance;
      m_pPlaneAngleMaxHotSpot->StartAngle = m_fPlaneConeMaxAngle;
      m_pPlaneAngleMaxHotSpot->ToolTipText = "Plane angle max";
      Matrix3F mRot;
      mRot.FromEuler(90, 0, 180);
      m_pPlaneAngleMaxHotSpot->Rotation = mRot;
      EditorManager::ActiveView->HotSpots->Add(m_pPlaneAngleMaxHotSpot);
    }

    {
      m_pPlaneAngleMinHotSpot = gcnew HotSpotConeAngle(
        pShape, "textures\\Hotspot_A.tga", VisionColors::Red, 
        HotSpotBase::PickType::Square, 4.0f);
      m_pPlaneAngleMinHotSpot->Distance = fHotSpotdistance;
      m_pPlaneAngleMinHotSpot->StartAngle = m_fPlaneConeMinAngle;
      m_pPlaneAngleMinHotSpot->ToolTipText = "Plane angle min";
      Matrix3F mRot;
      mRot.FromEuler(-90, 0, 0);
      m_pPlaneAngleMinHotSpot->Rotation = mRot;
      EditorManager::ActiveView->HotSpots->Add(m_pPlaneAngleMinHotSpot);
    }
  }

  void RagdollConstraintState::DestroyHotspots(Shape3D ^pShape)
  {
    EditorManager::ActiveView->HotSpots->Remove(m_pConeAngleHotSpot);
    m_pConeAngleHotSpot->Remove();
    m_pConeAngleHotSpot = nullptr;

    EditorManager::ActiveView->HotSpots->Remove(m_pPlaneAngleMaxHotSpot);
    m_pPlaneAngleMaxHotSpot->Remove();
    m_pPlaneAngleMaxHotSpot = nullptr;

    EditorManager::ActiveView->HotSpots->Remove(m_pPlaneAngleMinHotSpot);
    m_pPlaneAngleMinHotSpot->Remove();
    m_pPlaneAngleMinHotSpot = nullptr;
  }

  void RagdollConstraintState::OnHotSpotDragBegin(HotSpotBase ^pHotSpot, VisionViewBase ^pView)
  {
    if (pHotSpot == m_pConeAngleHotSpot)
      m_pConeAngleHotSpot->StartAngle = ConeAngle;
    else if (pHotSpot == m_pPlaneAngleMinHotSpot)
      m_pPlaneAngleMinHotSpot->StartAngle = PlaneConeMinAngle;
    else if (pHotSpot == m_pPlaneAngleMaxHotSpot)
      m_pPlaneAngleMaxHotSpot->StartAngle = PlaneConeMaxAngle;
  }

  void RagdollConstraintState::OnHotSpotDrag(
    HotSpotBase ^pHotSpot, VisionViewBase ^pView, float fDeltaX, float fDeltaY)
  {
    if (pHotSpot == m_pConeAngleHotSpot)
      ConeAngle = m_pConeAngleHotSpot->CurrentAngle;
    else if (pHotSpot == m_pPlaneAngleMinHotSpot)
      PlaneConeMinAngle = m_pPlaneAngleMinHotSpot->CurrentAngle;
    else if (pHotSpot == m_pPlaneAngleMaxHotSpot)
      PlaneConeMaxAngle = m_pPlaneAngleMaxHotSpot->CurrentAngle;
  }

  void RagdollConstraintState::OnHotSpotDragEnd(HotSpotBase ^pHotSpot, VisionViewBase ^pView)
  {
    if ((pHotSpot == m_pConeAngleHotSpot) && (m_pConeAngleHotSpot->HasChanged))
    {
      float fNewAngle = m_pConeAngleHotSpot->CurrentAngle;
      ConeAngle = m_pConeAngleHotSpot->StartAngle; // set old angle for the action
      EditorManager::Actions->Add(SetPropertyAction::CreateSetPropertyAction(this, "ConeAngle", fNewAngle)); // send an action which sets the property from old value to new one
    }
    if ((pHotSpot == m_pPlaneAngleMinHotSpot) && (m_pPlaneAngleMinHotSpot->HasChanged))
    {
      float fNewAngle = m_pPlaneAngleMinHotSpot->CurrentAngle;
      PlaneConeMinAngle = m_pPlaneAngleMinHotSpot->StartAngle; // set old angle for the action
      EditorManager::Actions->Add(SetPropertyAction::CreateSetPropertyAction(this, "PlaneConeMinAngle", fNewAngle)); // send an action which sets the property from old value to new one
    }
    if ((pHotSpot == m_pPlaneAngleMaxHotSpot) && (m_pPlaneAngleMaxHotSpot->HasChanged))
    {
      float fNewAngle = m_pPlaneAngleMaxHotSpot->CurrentAngle;
      PlaneConeMaxAngle = m_pPlaneAngleMaxHotSpot->StartAngle; // set old angle for the action
      EditorManager::Actions->Add(SetPropertyAction::CreateSetPropertyAction(this, "PlaneConeMaxAngle", fNewAngle)); // send an action which sets the property from old value to new one
    }
  }

  void RagdollConstraintState::OnHotSpotEvaluatePosition(HotSpotBase ^pHotSpot)
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
