/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

using namespace System;
using namespace System::Runtime::Serialization;
using namespace System::ComponentModel;

using namespace CSharpFramework::UndoRedo;
using namespace CSharpFramework;
using namespace CSharpFramework::Math;
using namespace CSharpFramework::Shapes;
using namespace CSharpFramework::View;

class vHavokConstraintDesc;
class vHavokRigidBody;

namespace HavokManaged
{
  ref struct AnchorDescription;

  /// <summary>
  /// Enumeration of supported Havok constraint types
  /// </summary>
  public enum struct HavokConstraintType_e
  {
    BallAndSocket,
    Hinge,
    Ragdoll,
    StiffSpring,
    Fixed,
    Prismatic,
    Cylindrical,
    PointToPlane,
    PointToPath,
    Pulley
  };

  /// \brief Managed configuration structure for the "breakable" settings of a constraint
  [Serializable]
  public ref class ConstraintBreakableDesc : public ISerializable, public ICloneable
  {
  public:
    ConstraintBreakableDesc()
    {
      m_bIsBreakable = false;
      m_fThreshold = FLT_MAX;
      m_bRevertVelocityOnBreak = false;
    }

    // Public Properties
    [PropertyOrder(1)]
    [Description("Set this value to TRUE if the joint is breakable.")]
    property bool IsBreakable
    {
      bool get() { return m_bIsBreakable; }
      void set(bool bIsBreakable) { m_bIsBreakable = bIsBreakable; }
    }

    // Public Properties
    [PropertyOrder(2)]
    [Description("Maximum force the joint can withstand without breaking. Range: (0,inf]. Only relevant if IsBreakable is TRUE.")]
    property float Threshold
    {
      float get() { return m_fThreshold; }
      void set(float fThreshold) { m_fThreshold = fThreshold; }
    }

    [PropertyOrder(3)]
    [Description("Whether to revert the velocity of the affected rigid bodies when the constraint breaks. Only relevant if IsBreakable is TRUE.")]
    property bool RevertVelocityOnBreak
    {
      bool get() { return m_bRevertVelocityOnBreak; }
      void set(bool bRevert) { m_bRevertVelocityOnBreak = bRevert; }
    }

    // Deserialization method
    virtual void GetObjectData(SerializationInfo ^si, StreamingContext sc)
    {
      si->AddValue("m_bIsBreakable", this->m_bIsBreakable);
      si->AddValue("m_fThreshold", this->m_fThreshold);
      si->AddValue("m_bRevertVelocityOnBreak", this->m_bRevertVelocityOnBreak);
    }

    /// ICloneable implementation
    virtual Object^ Clone()
    {
      return MemberwiseClone();
    }

  protected:
    // Serialization constructor
    ConstraintBreakableDesc(SerializationInfo ^si, StreamingContext sc)
    {
      m_bIsBreakable = si->GetBoolean("m_bIsBreakable");
      m_fThreshold = si->GetSingle("m_fThreshold");
      m_bRevertVelocityOnBreak  = si->GetBoolean("m_bRevertVelocityOnBreak");
    }

    // Variables
    bool m_bIsBreakable;           ///< Indicates whether the joint is breakable
    float m_fThreshold;            ///< Maximum force the joint can withstand without breaking. Range: (0,inf] 
    bool m_bRevertVelocityOnBreak; ///< Whether to revert the velocity of the affected rigid bodies when the constraint breaks
  };

  /// \brief Managed configuration structure for the "malleable" settings of a constraint
  [Serializable]
  public ref class ConstraintMalleableDesc : public ISerializable, public ICloneable
  {
  public:
    ConstraintMalleableDesc()
    {
      m_bIsMalleable = false;
      m_fStrength = 1.f;
    }

    // Public Properties
    [PropertyOrder(1)]
    [Description("Set this value to TRUE if the joint is malleable.")]
    property bool IsMalleable
    {
      bool get() { return m_bIsMalleable; }
      void set(bool bIsMalleable) { m_bIsMalleable = bIsMalleable; }
    }

    // Public Properties
    [PropertyOrder(2)]
    [Description("Strength of the joint. 0.0 is freely malleable, 1.0 is not malleable.")]
    property float Strength
    {
      float get() { return m_fStrength; }
      void set(float fStrength) { m_fStrength = min(1.f, max(0.f, fStrength)); }
    }

    // Deserialization method
    virtual void GetObjectData(SerializationInfo ^si, StreamingContext sc)
    {
      si->AddValue("m_bIsMalleable", this->m_bIsMalleable);
      si->AddValue("m_fStrength", this->m_fStrength);
    }

    /// ICloneable implementation
    virtual Object^ Clone()
    {
      return MemberwiseClone();
    }

  protected:
    // Serialization constructor
    ConstraintMalleableDesc(SerializationInfo ^si, StreamingContext sc)
    {
      m_bIsMalleable = si->GetBoolean("m_bIsMalleable");
      m_fStrength = si->GetSingle("m_fStrength");
    }

    // Variables
    bool m_bIsMalleable;           ///< Indicates whether the joint is malleable
    float m_fStrength;             ///< Strength of the joint
  };

  /// \brief Managed configuration structure for the angular limits settings of a constraint
  [Serializable]
  public ref class ConstraintLimitsDesc : public ISerializable, public ICloneable
  {
  public:
    ConstraintLimitsDesc()
    {
      m_bUseLimits = false;
      m_fLimitMax = 180.f;
      m_fLimitMin = -180.f;
    }

    // Public Properties
    [PropertyOrder(1)]
    [Description("Set this value to TRUE to activate the limits in the constraint.")]
    property bool UseLimits
    {
      bool get() { return m_bUseLimits; }
      void set(bool bUseLimits) { m_bUseLimits = bUseLimits; }
    }

    // Public Properties
    [PropertyOrder(2)]
    [Description("Max. angular limit of the constraint in degrees.\n\t0 <= Max. Limit <= (360 + Min. Limit)")]
    property float LimitMax
    {
      float get() { return m_fLimitMax; }
      void set(float fLimitMax) { m_fLimitMax = ValidateMaxLimit(fLimitMax); }
    }

    // Public Properties
    [PropertyOrder(3)]
    [Description("Min. angular limit of the constraint in degrees.\n\t(-360 + Max. Limit) <= Min. Limit <= 0")]
    property float LimitMin
    {
      float get() { return m_fLimitMin; }
      void set(float fLimitMin) { m_fLimitMin = ValidateMinLimit(fLimitMin); }
    }

    // Deserialization method
    virtual void GetObjectData(SerializationInfo ^si, StreamingContext sc)
    {
      si->AddValue("m_bUseLimits", this->m_bUseLimits);
      si->AddValue("m_fLimitMax", this->m_fLimitMax);
      si->AddValue("m_fLimitMin", this->m_fLimitMin);
    }

    /// ICloneable implementation
    virtual Object^ Clone()
    {
      return MemberwiseClone();
    }

  protected:

    /// \brief Validates the maximum angular limit.
    /// \return The passed value to be validated if succeeded or the stated one if it exceeded the permitted values.
    float ValidateMaxLimit(float fValue)
    {
      if (fValue < 0.0f) return 0.0f;
      if (fValue >= (360.0f + m_fLimitMin)) return (360.0f + m_fLimitMin);
      return fValue;
    }

    /// \brief Validates the maximum angular limit.
    /// \return The passed value to be validated if succeeded or the stated one if it exceeded the permitted values.
    float ValidateMinLimit(float fValue)
    {
      if (fValue > 0.0f) return 0.0f;
      if (fValue < (-360.0f + m_fLimitMax)) return (-360.0f + m_fLimitMax);
      return fValue;
    }

    // Serialization constructor
    ConstraintLimitsDesc(SerializationInfo ^si, StreamingContext sc)
    {
      m_bUseLimits = si->GetBoolean("m_bUseLimits");
      m_fLimitMax = si->GetSingle("m_fLimitMax");
      m_fLimitMin = si->GetSingle("m_fLimitMin");
    }

    // Variables
    bool m_bUseLimits;  ///< Indicates whether to active the angular limits or not.
    float m_fLimitMax;  ///< Maximum angular limit in degrees.
    float m_fLimitMin;  ///< Minimum angular limit in degrees.
  };

  /// \brief Base class for constraint state objects.
  ///
  /// The constraint state object exposes all properties of the constraint, provides functions for setting
  /// and retrieving the properties from the native description and additionally visualizes the constraint.
  /// The factory method is also part of this base class.
  ///
  /// The ConstraintStateBase class is abstract. Each constraint type needs to implement a state class
  /// based on this class.
  [Serializable]
  public ref class ConstraintStateBase abstract : public ISerializable, public ICloneable
  {
  public:
    ConstraintStateBase();

    // Common Properties
    [PropertyOrder(100)]
    [Description("Properties for the breakable settings.")]
    [TypeConverter(UndoableObjectConverter::typeid)]
    property ConstraintBreakableDesc^ BreakableSettings
    {
      ConstraintBreakableDesc^ get() { return m_pBreakableDesc; }
      void set(ConstraintBreakableDesc ^pBreakableDesc) { m_pBreakableDesc = pBreakableDesc; }
    }

    [PropertyOrder(101)]
    [Description("Properties for the malleable settings.")]
    [TypeConverter(UndoableObjectConverter::typeid)]
    property ConstraintMalleableDesc^ MalleableSettings
    {
      ConstraintMalleableDesc^ get() { return m_pMalleableDesc; }
      void set(ConstraintMalleableDesc ^pMalleableDesc) { m_pMalleableDesc = pMalleableDesc; }
    }

    /// Gets the native vHavok description for the constraint
    /// \param firstAnchor
    ///   Description of the first anchor connected to this joint
    /// \param secondAnchor
    ///   Description of the second anchor connected to this joint
    vHavokConstraintDesc *GetNativeDescription(AnchorDescription ^pFirstAnchor,
      AnchorDescription ^pSecondAnchor, Vector3F &position, Vector3F &direction, Vector3F &orientation);

    /// Visualizes the constraint properties in the engine
    virtual void Visualize (IVRenderInterface *pRenderer, const hkvVec3& startPos, const hkvVec3& endPos, const hkvVec3& constraintPos, const hkvVec3& constraintDir, const hkvVec3& constraintOri) = 0;

    /// Indicates whether the user will be allowed to rotate the constraint
    virtual bool SupportsRotation() { return false; }

    // Deserialization method
    virtual void GetObjectData(SerializationInfo ^si, StreamingContext sc);

    /// ICloneable implementation
    virtual Object^ Clone();

    /// Called by the parent shape to create any hotspots this constraint 
    /// type may need.
    /// \param pShape
    ///   the shape that request the hotspots to be created
    virtual void CreateHotspots(Shape3D ^pShape);

    /// Called by the parent shape to destroy any hotspots this constraint 
    /// type may have created earlier.
    /// \param pShape
    ///   the shape that request the hotspots to be destroyed
    virtual void DestroyHotspots(Shape3D ^pShape);

    /// Called by the parent shape when a hotspot is started to be dragged
    /// \param pHotSpot
    ///   the hotspot being dragged
    /// \param pView
    ///   the view in which the hotspot is being dragged
    virtual void OnHotSpotDragBegin(HotSpotBase ^pHotSpot, VisionViewBase ^pView);

    /// Called by the parent shape when a hotspot is being dragged
    /// \param pHotSpot
    ///   the hotspot being dragged
    /// \param pView
    ///   the view in which the hotspot is being dragged
    /// \param fDeltaX
    ///   the distance which the hotspot has been dragged in X direction
    /// \param fDeltaY
    ///   the distance which the hotspot has been dragged in Y direction
    virtual void OnHotSpotDrag(
      HotSpotBase ^pHotSpot, VisionViewBase ^pView, float fDeltaX, float fDeltaY);

    /// Called by the parent shape when a hotspot is not being dragged anymore
    /// \param pHotSpot
    ///   the hotspot that has been dragged
    /// \param pView
    ///   the view in which the hotspot has been dragged
    virtual void OnHotSpotDragEnd(HotSpotBase ^pHotSpot, VisionViewBase ^pView);

    /// Called by the parent shape each frame for every hotspot to do the
    /// necessary visualization.
    /// \param pHotSpot
    ///   the hotspot to evaluate
    virtual void OnHotSpotEvaluatePosition(HotSpotBase ^pHotSpot);

  protected:
    // Serialization constructor
    ConstraintStateBase(SerializationInfo ^si, StreamingContext sc);
    
    /// Sets all properties to their default value
    void SetToDefault();

    /// Creates a new native description instance
    virtual vHavokConstraintDesc *CreateNativeDesc() = 0;

    /// Fills this managed description with the content of the passed native description
    virtual void NativeToManaged(const vHavokConstraintDesc &nativeDesc);

    /// Fills the passed native description with the content of this managed description.
    virtual void ManagedToNative(vHavokConstraintDesc &nativeDesc);

    /// Sets anchor specific information and position-related information in the passed 
    /// native description.
    /// This function is called once the native description has been created and the managed
    /// configuration has been set on the native one. It can be used to perform final 
    /// initializations on the native descriptions.
    virtual void SetAnchorDataOnNative(vHavokConstraintDesc &nativeDesc, AnchorDescription ^pFirstAnchor, AnchorDescription ^pSecondAnchor, Vector3F &position, Vector3F &direction, Vector3F &orientation);

    // Variables
    ConstraintBreakableDesc ^m_pBreakableDesc;
    ConstraintMalleableDesc ^m_pMalleableDesc;
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
