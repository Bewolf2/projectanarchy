/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/HavokPhysicsManagedPCH.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraint.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRigidBody.hpp>

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/ConstraintStateBase.hpp>

#using <mscorlib.dll>
using namespace ManagedFramework;
using namespace CSharpFramework::Serialization;

namespace HavokManaged
{
  ConstraintStateBase::ConstraintStateBase()
  {
    m_pBreakableDesc = gcnew ConstraintBreakableDesc();
    m_pMalleableDesc = gcnew ConstraintMalleableDesc();
  }

  vHavokConstraintDesc *ConstraintStateBase::GetNativeDescription(
    AnchorDescription ^pFirstAnchor, AnchorDescription ^pSecondAnchor, 
    Vector3F &position, Vector3F &direction, Vector3F &orientation)
  {
    // Create a new Havok joint description instance
    vHavokConstraintDesc *pDesc = CreateNativeDesc();
    VASSERT(pDesc != NULL);
    if (pDesc == NULL)
      return NULL;

    // Fill the instance with the current managed configuration
    ManagedToNative(*pDesc);

    // Set any anchor specific information in the native description
    SetAnchorDataOnNative(*pDesc, pFirstAnchor, pSecondAnchor, position, direction, orientation);

    return pDesc;
  }

  void ConstraintStateBase::ManagedToNative(vHavokConstraintDesc &nativeDesc)
  {
    VVERIFY_OR_RET(m_pBreakableDesc);
    nativeDesc.m_bBreakable = m_pBreakableDesc->IsBreakable;
    nativeDesc.m_fBreakThreshold = m_pBreakableDesc->Threshold;
    nativeDesc.m_bRevertVelocityOnBreak = m_pBreakableDesc->RevertVelocityOnBreak;

    VVERIFY_OR_RET(m_pMalleableDesc);
    nativeDesc.m_bMalleable = m_pMalleableDesc->IsMalleable;
    nativeDesc.m_fConstraintStrength = m_pMalleableDesc->Strength;
  }

  void ConstraintStateBase::NativeToManaged(const vHavokConstraintDesc &nativeDesc)
  {
    VVERIFY_OR_RET(m_pBreakableDesc);
    m_pBreakableDesc->IsBreakable = nativeDesc.m_bBreakable;
    m_pBreakableDesc->Threshold = static_cast<float>(nativeDesc.m_fBreakThreshold);
    m_pBreakableDesc->RevertVelocityOnBreak = nativeDesc.m_bRevertVelocityOnBreak;

    VVERIFY_OR_RET(m_pMalleableDesc);
    m_pMalleableDesc->IsMalleable = nativeDesc.m_bMalleable;
    m_pMalleableDesc->Strength = static_cast<float>(nativeDesc.m_fConstraintStrength);
  }

  void ConstraintStateBase::SetToDefault()
  {
    // Create a native description with default settings
    vHavokConstraintDesc *pDesc = CreateNativeDesc();
    VASSERT(pDesc != NULL);      
    if (pDesc == NULL)
      return;

    // Copy the native settings to the managed description
    NativeToManaged(*pDesc);

    V_SAFE_DELETE(pDesc);
  }

  void ConstraintStateBase::SetAnchorDataOnNative(vHavokConstraintDesc &nativeDesc, AnchorDescription ^pFirstAnchor, AnchorDescription ^pSecondAnchor, Vector3F &position, Vector3F &direction, Vector3F &orientation)
  {
    // Set the involved rigid bodies and their pivots
    if (pFirstAnchor)
    {
      nativeDesc.m_spBodies[0] = pFirstAnchor->GetRigidBody();
      nativeDesc.m_vPivots[0] = pFirstAnchor->GetAnchorPosition();
    }
    if (pSecondAnchor)
    {
      nativeDesc.m_spBodies[1] = pSecondAnchor->GetRigidBody();
      nativeDesc.m_vPivots[1] = pSecondAnchor->GetAnchorPosition();
    }
  }

  void ConstraintStateBase::GetObjectData(SerializationInfo ^si, StreamingContext sc)
  {
    si->AddValue("m_pBreakableDesc", this->m_pBreakableDesc);
    si->AddValue("m_pMalleableDesc", this->m_pMalleableDesc);
  }

  ConstraintStateBase::ConstraintStateBase(SerializationInfo ^si, StreamingContext sc)
  {
    m_pBreakableDesc = dynamic_cast<ConstraintBreakableDesc^> (si->GetValue("m_pBreakableDesc", ConstraintBreakableDesc::typeid));
    if (SerializationHelper::HasElement(si, "m_pMalleableDesc"))
      m_pMalleableDesc = dynamic_cast<ConstraintMalleableDesc^> (si->GetValue("m_pMalleableDesc", ConstraintMalleableDesc::typeid));
    else
      m_pMalleableDesc = gcnew ConstraintMalleableDesc;
  }

  Object^ ConstraintStateBase::Clone()
  {    
    ConstraintStateBase ^pClonedShape = dynamic_cast<ConstraintStateBase^>(MemberwiseClone());

    // Create a deep copy the breakable and malleable configuration
    pClonedShape->m_pBreakableDesc = dynamic_cast<ConstraintBreakableDesc^>(m_pBreakableDesc->Clone());
    pClonedShape->m_pMalleableDesc = dynamic_cast<ConstraintMalleableDesc^>(m_pMalleableDesc->Clone());

    return pClonedShape;
  }

  void ConstraintStateBase::CreateHotspots(Shape3D ^pShape)
  {
  }

  void ConstraintStateBase::DestroyHotspots(Shape3D ^pShape)
  {
  }

  void ConstraintStateBase::OnHotSpotDragBegin(HotSpotBase ^pHotSpot, VisionViewBase ^pView)
  {
  }

  void ConstraintStateBase::OnHotSpotDrag(
    HotSpotBase ^pHotSpot, VisionViewBase ^pView, float fDeltaX, float fDeltaY)
  {
  }

  void ConstraintStateBase::OnHotSpotDragEnd(HotSpotBase ^pHotSpot, VisionViewBase ^pView)
  {
  }

  void ConstraintStateBase::OnHotSpotEvaluatePosition(HotSpotBase ^pHotSpot)
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
