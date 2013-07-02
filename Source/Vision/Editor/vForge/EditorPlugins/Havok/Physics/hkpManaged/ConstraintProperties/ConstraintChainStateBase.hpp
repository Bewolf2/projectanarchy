/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/ConstraintStateBase.hpp>

using namespace System;
using namespace System::Runtime::Serialization;
using namespace System::ComponentModel;

using namespace CSharpFramework::UndoRedo;
using namespace CSharpFramework;
using namespace CSharpFramework::Math;
using namespace CSharpFramework::Shapes;


class vHavokConstraintChainDesc;


namespace HavokManaged
{
  /// <summary>
  /// Enumeration of supported Havok constraint chain types
  /// </summary>
  public enum struct HavokConstraintChainType_e
  {
    BallAndSocket,
    StiffSpring,
  };

  /// \brief Base class for constraint chain state objects.
  ///
  /// The constraint chain state object exposes all properties of the constraint chain 
  /// and provides functions for setting and retrieving the properties from the native 
  /// description.
  ///
  /// The factory method is also part of this base class.
  ///
  /// The ConstraintChainStateBase class is abstract. Each constraint chain type needs to 
  /// implement a state class based on this class.
  [Serializable]
  public ref class ConstraintChainStateBase abstract : public ISerializable, public ICloneable
  {
  public:
    ConstraintChainStateBase();

    // Common Properties
    [PropertyOrder(1)]
    [Description("Key of the path to align this constraint chain to (optional)")]
    property String^ PathKey
    {
      String^ get() { return m_pPathKey; }
      void set(String ^pPathKey) { m_pPathKey = pPathKey; }
    }

    [PropertyOrder(2)]
    [Description("Length of a single chain link")]
    property float LinkLength
    {
      float get() { return m_fLinkLength; }
      void set(float fLength) { m_fLinkLength = (fLength >= 0.001f) ? fLength : 0.001f; }
    }

    [PropertyOrder(3)]
    [Description("Gap between the links of the chain")]
    property float LinkGap
    {
      float get() { return m_fLinkGap; }
      void set(float fGap) { m_fLinkGap = fGap; }
    }

    [PropertyOrder(4)]
    [Description("Diameter of the chain")]
    property float Diameter
    {
      float get() { return m_fDiameter; }
      void set(float fDiameter) { m_fDiameter = (fDiameter > 0.001f) ? fDiameter : 0.001f; }
    }
    
    [PropertyOrder(5)]
    [Description("Density of the chain link material (in kg per m^3)")]
    property float MaterialDensity
    {
      float get() { return m_fMaterialDensity; }
      void set(float fMaterialDensity) { m_fMaterialDensity = (fMaterialDensity > 0.001f) ? fMaterialDensity : 0.001f; }
    }

    [PropertyOrder(6)]
    [Description("Stretching constant of the link material")]
    property float MaterialStretchingConstant
    {
      float get() { return m_fMaterialStretchingConstant; }
      void set(float fMaterialStretchingConstant) { m_fMaterialStretchingConstant = (fMaterialStretchingConstant > 0.001f) ? fMaterialStretchingConstant : 0.001f; }
    }

    [PropertyOrder(7)]
    [Description("Damping used by the constraint chain solver. Valid values range from 0.0 (no damping) to 1.0 (full damping)")]
    property float Damping
    {
      float get() { return m_fDamping; }
      void set(float fDamping) { m_fDamping = min(1.f, max(0.f, fDamping)); }
    }

    [PropertyOrder(8)]
    [Description("Linear damping to be set for each link of the constraint chain")]
    property float LinkLinearDamping
    {
      float get() { return m_fLinkLinearDamping; }
      void set(float fDamping) { m_fLinkLinearDamping = fDamping; }
    }

    [PropertyOrder(9)]
    [Description("Angular damping to be set for each link of the constraint chain")]
    property float LinkAngularDamping
    {
      float get() { return m_fLinkAngularDamping; }
      void set(float fDamping) { m_fLinkAngularDamping = fDamping; }
    }

    [PropertyOrder(10)]
    [Description("Whether to use a Havok-calculated inertia tensor for chain links")]
    property bool CalculateLinkInertia
    {
      bool get() { return m_bCalculateLinkInertia; }
      void set(bool bCalc) { m_bCalculateLinkInertia = bCalc; }
    }

    [PropertyOrder(11)]
    [Description("The scaling of the chain link inertia tensors along their x/y/z axes")]
    property Vector3F InertiaTensorAxisScaling
    {
      Vector3F get() { return m_vInertiaTensorAxisScaling; }
      void set(Vector3F vScaling) { m_vInertiaTensorAxisScaling = vScaling; }
    }

    [PropertyOrder(12)]
    [Description("The scaling all elements of the chain link inertia tensors")]
    property float InertiaTensorUniformScaling
    {
      float get() { return m_fInertiaTensorUniformScaling; }
      void set(float fScaling) { m_fInertiaTensorUniformScaling = fScaling; }
    }

    [PropertyOrder(13)]
    [Description("Whether to compute the CFM (constraint force mixing) or use a fixed value")]
    property bool ComputeCFM
    {
      bool get() { return m_bComputeCFM; }
      void set(bool bCompute) { m_bComputeCFM = bCompute; }
    }

    [PropertyOrder(14)]
    [Description("Scaling factor for computed CFM")]
    property float CFMFactor
    {
      float get() { return m_fCFMFactor; }
      void set(float fCFMFactor) { m_fCFMFactor = (fCFMFactor >= 0.f) ? fCFMFactor : 0.f; }
    }

    [PropertyOrder(15)]
    [Description("Fixed value for CFM")]
    property float FixedCFM
    {
      float get() { return m_fFixedCFM; }
      void set(float fCFM) { m_fFixedCFM = (fCFM >= 0.f) ? fCFM : 0.f; }
    }


    [PropertyOrder(20)]
    [Description("Collision layer for the chain links")]
    property int LinkCollisionLayer
    {
      int get() { return m_iLinkCollisionLayer; }
      void set(int iLayer) { m_iLinkCollisionLayer = iLayer; }
    }

    [PropertyOrder(21)]
    [Description("Collision system group for the chain links")]
    property int LinkCollisionGroup
    {
      int get() { return m_iLinkCollisionGroup; }
      void set(int iGroup) { m_iLinkCollisionGroup = iGroup; }
    }

    [PropertyOrder(22)]
    [Description("Collision subsystem ID for the chain links")]
    property int LinkSubSystemId
    {
      int get() { return m_iLinkSubSystemId; }
      void set(int iId) { m_iLinkSubSystemId = iId; }
    }

    [PropertyOrder(23)]
    [Description("No-collision subsystem ID for the chain links")]
    property int LinkSubSystemDontCollideWith
    {
      int get() { return m_iLinkSubSystemDontCollideWith; }
      void set(int iId) { m_iLinkSubSystemDontCollideWith = iId; }
    }


    [PropertyOrder(30)]
    [Description("Whether to use angular friction to further stabilize the constraint chain")]
    property bool UseAngularFriction
    {
      bool get() { return m_bUseAngularFriction; }
      void set(bool bUse) { m_bUseAngularFriction = bUse; }
    }

    [PropertyOrder(31)]
    [Description("Minimum internal friction factor (in the middle of the chain)")]
    property float FrictionFactorMin
    {
      float get() { return m_fFrictionFactorMin; }
      void set(float fFactor) { m_fFrictionFactorMin = fFactor; }
    }

    [PropertyOrder(32)]
    [Description("Maximum internal friction factor (at the ends of the chain)")]
    property float FrictionFactorMax
    {
      float get() { return m_fFrictionFactorMax; }
      void set(float fFactor) { m_fFrictionFactorMax = fFactor; }
    }

    [PropertyOrder(33)]
    [Description("Minimum tension the chain is assumed to always have (used for angular friction calculations)")]
    property float BaseTension
    {
      float get() { return m_fBaseTension; }
      void set(float fBaseTension) { m_fBaseTension = fBaseTension; }
    }


    /// \brief
    ///   Gets the native vHavok description for the constraint
    /// \param firstAnchor
    ///   Description of the first anchor connected to this joint
    /// \param secondAnchor
    ///   Description of the second anchor connected to this joint
    virtual vHavokConstraintChainDesc *GetNativeDescription(
      AnchorDescription ^pFirstAnchor, AnchorDescription ^pSecondAnchor, 
      Vector3F &position, Vector3F &direction, Vector3F &orientation);

    /// Indicates whether the user will be allowed to rotate the constraint
    virtual bool SupportsRotation() { return false; }

    // Deserialisation method
    virtual void GetObjectData(SerializationInfo ^si, StreamingContext sc);

    /// ICloneable implementation
    virtual Object^ Clone();

  protected:
    // Serialisation constructor
    ConstraintChainStateBase(SerializationInfo ^si, StreamingContext sc);

    /// Sets all properties to their default value
    void SetToDefault();

    /// Creates a new native description instance
    virtual vHavokConstraintChainDesc *CreateNativeDesc() = 0;

    /// Fills this managed description with the content of the passed native description
    virtual void NativeToManaged(const vHavokConstraintChainDesc &nativeDesc);

    /// Fills the passed native description with the content of this managed description.
    virtual void ManagedToNative(vHavokConstraintChainDesc &nativeDesc);

    /// Sets anchor specific information and position-related information in the passed 
    /// native description.
    /// This function is called once the native description has been created and the managed
    /// configuration has been set on the native one. It can be used to perform final 
    /// initialisations on the native descriptions.
    virtual void SetAnchorDataOnNative(vHavokConstraintChainDesc &nativeDesc, 
      AnchorDescription ^pFirstAnchor, AnchorDescription ^pSecondAnchor, 
      Vector3F &position, Vector3F &direction, Vector3F &orientation);

    // Variables
    String ^m_pPathKey;
    float m_fLinkLength;
    float m_fLinkGap;
    float m_fDiameter;

    float m_fMaterialDensity;
    float m_fMaterialStretchingConstant;

    float m_fDamping;
    float m_fLinkLinearDamping;
    float m_fLinkAngularDamping;

    bool m_bCalculateLinkInertia;
    Vector3F m_vInertiaTensorAxisScaling;
    float m_fInertiaTensorUniformScaling;

    bool m_bComputeCFM;
    float m_fCFMFactor;
    float m_fFixedCFM;

    int m_iLinkCollisionLayer;
    int m_iLinkCollisionGroup;
    int m_iLinkSubSystemId;
    int m_iLinkSubSystemDontCollideWith;

    bool m_bUseAngularFriction;
    float m_fFrictionFactorMin;
    float m_fFrictionFactorMax;
    float m_fBaseTension;

    static const float s_fMaterialDensityDefault = 1.0f * 1e3f;
    static const float s_fMaterialStretchingConstantDefault = 1.0f * 1e9f;
    static const float s_fFixedCFMDefault = 0.0001f;
  };
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
