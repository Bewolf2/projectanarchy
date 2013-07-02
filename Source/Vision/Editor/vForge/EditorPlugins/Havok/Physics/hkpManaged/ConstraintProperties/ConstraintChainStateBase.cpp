/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/HavokPhysicsManagedPCH.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraintChain.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRigidBody.hpp>

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/ConstraintChainStateBase.hpp>

#include <vcclr.h>

#using <mscorlib.dll>
using namespace System::Runtime::InteropServices;

using namespace ManagedFramework;
using namespace CSharpFramework::Serialization;


namespace HavokManaged
{
  // --------------------------------------------------------------------------
  ConstraintChainStateBase::ConstraintChainStateBase()
  {
  }

  // --------------------------------------------------------------------------
  ConstraintChainStateBase::ConstraintChainStateBase(SerializationInfo ^si, StreamingContext sc)
  {
    m_fDamping = si->GetSingle("m_fDamping");
    
    if (SerializationHelper::HasElement(si, "m_fLinkLinearDamping"))
      m_fLinkLinearDamping = si->GetSingle("m_fLinkLinearDamping");
    if (SerializationHelper::HasElement(si, "m_fLinkAngularDamping"))
      m_fLinkAngularDamping = si->GetSingle("m_fLinkAngularDamping");

    if (SerializationHelper::HasElement(si, "m_bCalculateLinkInertia"))
      m_bCalculateLinkInertia = si->GetBoolean("m_bCalculateLinkInertia");
    else
      m_bCalculateLinkInertia = false;
    
    if (SerializationHelper::HasElement(si, "m_vInertiaTensorAxisScaling"))
    {
      Vector3F^ v = dynamic_cast<Vector3F^>(si->GetValue("m_vInertiaTensorAxisScaling", Vector3F::typeid));
      m_vInertiaTensorAxisScaling = *v;
    }
    else
    {
      m_vInertiaTensorAxisScaling = Vector3F(1.f, 1.f, 1.f);
    }
    
    if (SerializationHelper::HasElement(si, "m_fInertiaTensorUniformScaling"))
      m_fInertiaTensorUniformScaling = si->GetSingle("m_fInertiaTensorUniformScaling");
    else
      m_fInertiaTensorUniformScaling = 1.f;

    m_fDiameter = si->GetSingle("m_fDiameter");
    m_fLinkGap = si->GetSingle("m_fLinkGap");
    m_fLinkLength = si->GetSingle("m_fLinkLength");
    m_pPathKey = si->GetString("m_sPathKey");

    if (SerializationHelper::HasElement(si, "m_fMaterialDensity"))
      m_fMaterialDensity = si->GetSingle("m_fMaterialDensity");
    else
      m_fMaterialDensity = s_fMaterialDensityDefault;

    if (SerializationHelper::HasElement(si, "m_fMaterialStretchingConstant"))
      m_fMaterialStretchingConstant = si->GetSingle("m_fMaterialStretchingConstant");
    else
      m_fMaterialStretchingConstant = s_fMaterialStretchingConstantDefault;

    if (SerializationHelper::HasElement(si, "m_bComputeCFM"))
      m_bComputeCFM = si->GetBoolean("m_bComputeCFM");
    else
      m_bComputeCFM = false;

    if (SerializationHelper::HasElement(si, "m_fCFMFactor"))
      m_fCFMFactor = si->GetSingle("m_fCFMFactor");
    else
      m_fCFMFactor = 0.f;

    if (SerializationHelper::HasElement(si, "m_fFixedCFM"))
     m_fFixedCFM = si->GetSingle("m_fFixedCFM");
    else
     m_fFixedCFM = s_fFixedCFMDefault;

    if (SerializationHelper::HasElement(si, "m_iLinkCollisionGroup"))
      m_iLinkCollisionGroup = si->GetInt32("m_iLinkCollisionGroup");
    if (SerializationHelper::HasElement(si, "m_iLinkCollisionLayer"))
      m_iLinkCollisionLayer = si->GetInt32("m_iLinkCollisionLayer");
    if (SerializationHelper::HasElement(si, "m_iLinkSubSystemId"))
      m_iLinkSubSystemId = si->GetInt32("m_iLinkSubSystemId");
    if (SerializationHelper::HasElement(si, "m_iLinkSubSystemDontCollideWith"))
      m_iLinkSubSystemDontCollideWith = si->GetInt32("m_iLinkSubSystemDontCollideWith");

    if (SerializationHelper::HasElement(si, "m_bUseAngularFriction"))
      m_bUseAngularFriction = si->GetBoolean("m_bUseAngularFriction");
    else
      m_bUseAngularFriction = false;

    if (SerializationHelper::HasElement(si, "m_fFrictionFactorMin"))
      m_fFrictionFactorMin = si->GetSingle("m_fFrictionFactorMin");
    else
      m_fFrictionFactorMin = 0.3f;

    if (SerializationHelper::HasElement(si, "m_fFrictionFactorMax"))
      m_fFrictionFactorMax = si->GetSingle("m_fFrictionFactorMax");
    else
      m_fFrictionFactorMax = 30.f;

    if (SerializationHelper::HasElement(si, "m_fBaseTension"))
      m_fBaseTension = si->GetSingle("m_fBaseTension");
    else
      m_fBaseTension = 0.f;
  }

  // --------------------------------------------------------------------------
  Object^ ConstraintChainStateBase::Clone()
  {    
    ConstraintChainStateBase ^pClonedShape = 
      dynamic_cast<ConstraintChainStateBase^>(MemberwiseClone());

    return pClonedShape;
  }

  // --------------------------------------------------------------------------
  vHavokConstraintChainDesc *ConstraintChainStateBase::GetNativeDescription(
    AnchorDescription  ^pFirstAnchor, AnchorDescription ^pSecondAnchor, 
    Vector3F &position, Vector3F &direction, Vector3F &orientation)
  {
    // Create a new Havok joint description instance
    vHavokConstraintChainDesc *pDesc = CreateNativeDesc();
    VASSERT(pDesc != NULL);
    if (pDesc == NULL)
      return NULL;

    // Fill the instance with the current managed configuration
    ManagedToNative(*pDesc);

    // Set any anchor specific information in the native description
    SetAnchorDataOnNative(*pDesc, pFirstAnchor, pSecondAnchor, position, direction, orientation);

    return pDesc;
  }

  // --------------------------------------------------------------------------
  void ConstraintChainStateBase::GetObjectData(SerializationInfo ^si, StreamingContext sc)
  {
    si->AddValue("m_fDamping", m_fDamping);
    si->AddValue("m_fLinkLinearDamping", m_fLinkLinearDamping);
    si->AddValue("m_fLinkAngularDamping", m_fLinkAngularDamping);

    si->AddValue("m_bCalculateLinkInertia", m_bCalculateLinkInertia);
    si->AddValue("m_vInertiaTensorAxisScaling", m_vInertiaTensorAxisScaling);
    si->AddValue("m_fInertiaTensorUniformScaling", m_fInertiaTensorUniformScaling);

    si->AddValue("m_fDiameter", m_fDiameter);
    si->AddValue("m_fLinkGap", m_fLinkGap);
    si->AddValue("m_fLinkLength", m_fLinkLength);
    
    si->AddValue("m_fMaterialDensity", m_fMaterialDensity);
    si->AddValue("m_fMaterialStretchingConstant", m_fMaterialStretchingConstant);

    si->AddValue("m_bComputeCFM", m_bComputeCFM);
    si->AddValue("m_fCFMFactor", m_fCFMFactor);
    si->AddValue("m_fFixedCFM", m_fFixedCFM);

    si->AddValue("m_sPathKey", this->m_pPathKey);

    si->AddValue("m_iLinkCollisionGroup", m_iLinkCollisionGroup);
    si->AddValue("m_iLinkCollisionLayer", m_iLinkCollisionLayer);
    si->AddValue("m_iLinkSubSystemId", m_iLinkSubSystemId);
    si->AddValue("m_iLinkSubSystemDontCollideWith", m_iLinkSubSystemDontCollideWith);

    si->AddValue("m_bUseAngularFriction", m_bUseAngularFriction);
    si->AddValue("m_fFrictionFactorMin", m_fFrictionFactorMin);
    si->AddValue("m_fFrictionFactorMax", m_fFrictionFactorMax);
    si->AddValue("m_fBaseTension", m_fBaseTension);
  }

  // --------------------------------------------------------------------------
  void ConstraintChainStateBase::ManagedToNative(vHavokConstraintChainDesc &nativeDesc)
  {
    nativeDesc.m_fChainDamping = m_fDamping;
    nativeDesc.m_fLinkLinearDamping = m_fLinkLinearDamping;
    nativeDesc.m_fLinkAngularDamping = m_fLinkAngularDamping;

    nativeDesc.m_bCalculateLinkInertia = m_bCalculateLinkInertia;
    nativeDesc.m_vInertiaTensorAxisScaling = hkvVec3(
      m_vInertiaTensorAxisScaling.X, m_vInertiaTensorAxisScaling.Y, m_vInertiaTensorAxisScaling.Z);
    nativeDesc.m_fInertiaTensorUniformScaling = m_fInertiaTensorUniformScaling;

    nativeDesc.m_fDiameter = m_fDiameter;
    nativeDesc.m_fLinkGap = m_fLinkGap;
    nativeDesc.m_fLinkLength = m_fLinkLength;

    nativeDesc.m_fMaterialDensity = m_fMaterialDensity;
    nativeDesc.m_fMaterialStretchingConstant = m_fMaterialStretchingConstant;

    nativeDesc.m_bComputeCfm = m_bComputeCFM;
    nativeDesc.m_fCfmFactor = m_fCFMFactor;
    nativeDesc.m_fFixedCfm = m_fFixedCFM;

    nativeDesc.m_iLinkCollisionGroup = m_iLinkCollisionGroup;
    nativeDesc.m_iLinkCollisionLayer = m_iLinkCollisionLayer;
    nativeDesc.m_iLinkSubSystemId = m_iLinkSubSystemId;
    nativeDesc.m_iLinkSubSystemDontCollideWith = m_iLinkSubSystemDontCollideWith;

    nativeDesc.m_bUseAngularFriction = m_bUseAngularFriction;
    nativeDesc.m_fFrictionFactorMin = m_fFrictionFactorMin;
    nativeDesc.m_fFrictionFactorMax = m_fFrictionFactorMax;
    nativeDesc.m_fBaseTension = m_fBaseTension;

    if (m_pPathKey)
    {
      IntPtr pPathKey = Marshal::StringToHGlobalAnsi(m_pPathKey);
      nativeDesc.SetPathKey((const char*)pPathKey.ToPointer());
      Marshal::FreeHGlobal(pPathKey);
    }
    else
    {
      nativeDesc.SetPathKey(NULL);
    }
  }

  // --------------------------------------------------------------------------
  void ConstraintChainStateBase::NativeToManaged(const vHavokConstraintChainDesc &nativeDesc)
  {
    m_fDamping = nativeDesc.m_fChainDamping;
    m_fLinkLinearDamping = nativeDesc.m_fLinkLinearDamping;
    m_fLinkAngularDamping = nativeDesc.m_fLinkAngularDamping;

    m_bCalculateLinkInertia = nativeDesc.m_bCalculateLinkInertia;
    m_vInertiaTensorAxisScaling = Vector3F(nativeDesc.m_vInertiaTensorAxisScaling.x,
      nativeDesc.m_vInertiaTensorAxisScaling.y, nativeDesc.m_vInertiaTensorAxisScaling.z);
    m_fInertiaTensorUniformScaling = nativeDesc.m_fInertiaTensorUniformScaling;

    m_fDiameter = nativeDesc.m_fDiameter;
    m_fLinkGap = nativeDesc.m_fLinkGap;
    m_fLinkLength = nativeDesc.m_fLinkLength;

    m_fMaterialDensity = nativeDesc.m_fMaterialDensity;
    m_fMaterialStretchingConstant = nativeDesc.m_fMaterialStretchingConstant;

    m_bComputeCFM = nativeDesc.m_bComputeCfm;
    m_fCFMFactor = nativeDesc.m_fCfmFactor;
    m_fFixedCFM = nativeDesc.m_fFixedCfm;

    m_iLinkCollisionGroup = nativeDesc.m_iLinkCollisionGroup;
    m_iLinkCollisionLayer = nativeDesc.m_iLinkCollisionLayer;
    m_iLinkSubSystemId = nativeDesc.m_iLinkSubSystemId;
    m_iLinkSubSystemDontCollideWith = nativeDesc.m_iLinkSubSystemDontCollideWith;

    m_bUseAngularFriction = nativeDesc.m_bUseAngularFriction;
    m_fFrictionFactorMin = nativeDesc.m_fFrictionFactorMin;
    m_fFrictionFactorMax = nativeDesc.m_fFrictionFactorMax;
    m_fBaseTension = nativeDesc.m_fBaseTension;

    m_pPathKey = ConversionUtils::VStringToString(nativeDesc.GetPathKey());
  }

  // --------------------------------------------------------------------------
  void ConstraintChainStateBase::SetAnchorDataOnNative(
    vHavokConstraintChainDesc &nativeDesc, AnchorDescription ^pFirstAnchor, 
    AnchorDescription ^pSecondAnchor, Vector3F &position, Vector3F &direction, 
    Vector3F &orientation)
  {
    // Set the involved rigid bodies and their pivots
    if (pFirstAnchor)
    {
      nativeDesc.m_spAnchorBodies[0] = pFirstAnchor->GetRigidBody();
      nativeDesc.m_vAnchorPivots[0] = pFirstAnchor->GetAnchorPosition();
    }
    if (pSecondAnchor)
    {
      nativeDesc.m_spAnchorBodies[1] = pSecondAnchor->GetRigidBody();
      nativeDesc.m_vAnchorPivots[1] = pSecondAnchor->GetAnchorPosition();
    }
  }

  // --------------------------------------------------------------------------
  void ConstraintChainStateBase::SetToDefault()
  {
    // Create a native description with default settings
    vHavokConstraintChainDesc *pDesc = CreateNativeDesc();
    VASSERT(pDesc != NULL);      
    if (pDesc == NULL)
      return;

    // Copy the native settings to the managed description
    NativeToManaged(*pDesc);

    V_SAFE_DELETE(pDesc);
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
