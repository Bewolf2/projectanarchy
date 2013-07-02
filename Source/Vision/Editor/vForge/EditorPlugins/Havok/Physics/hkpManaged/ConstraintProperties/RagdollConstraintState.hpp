/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/ConstraintStateBase.hpp>

using namespace System;
using namespace System::Runtime::Serialization;
using namespace System::ComponentModel;

using namespace ManagedFramework;

using namespace CSharpFramework::UndoRedo;
using namespace CSharpFramework;

namespace HavokManaged
{
  /// \brief Managed property class for Ragdoll constraints.
  /// 
  /// The RagdollConstraintState class exposes the properties for ragdoll
  /// constraints and provides functions for setting/retrieving them on the 
  /// native description.
  [Serializable]
  public ref class RagdollConstraintState : public ConstraintStateBase
  {
  public:
    /// Constructor. Sets the default values.
    RagdollConstraintState();

    [PropertyOrder(102)]
    [Description("The opening angle (in degrees) of the movement cone around the twist axis")]
    property float ConeAngle
    {
      float get() { return m_fConeAngle; }
      void set(float fAngle) { m_fConeAngle = hkvMath::Min(180.f, hkvMath::Max(0.f, fAngle)); }
    }

    [PropertyOrder(103)]
    [Description("The opening angle (in degrees) of the limiting cone around the negative plane axis")]
    property float PlaneConeMinAngle
    {
      float get() { return m_fPlaneConeMinAngle; }
      void set(float fAngle) { m_fPlaneConeMinAngle = hkvMath::Min(180.f, hkvMath::Max(0.f, fAngle)); }
    }

    [PropertyOrder(104)]
    [Description("The opening angle (in degrees) of the limiting cone around the positive plane axis")]
    property float PlaneConeMaxAngle
    {
      float get() { return m_fPlaneConeMaxAngle; }
      void set(float fAngle) { m_fPlaneConeMaxAngle = hkvMath::Min(180.f, hkvMath::Max(0.f, fAngle)); }
    }

    [PropertyOrder(105)]
    [Description("The minimum angle (in degrees) of rotation around the twist axis")]
    property float TwistMinAngle
    {
      float get() { return m_fTwistMinAngle; }
      void set(float fAngle) { m_fTwistMinAngle = hkvMath::Min(fAngle, m_fTwistMaxAngle); }
    }

    [PropertyOrder(106)]
    [Description("The maximum angle (in degrees) of rotation around the twist axis")]
    property float TwistMaxAngle
    {
      float get() { return m_fTwistMaxAngle; }
      void set(float fAngle) { m_fTwistMaxAngle = hkvMath::Max(fAngle, m_fTwistMinAngle); }
    }

    [PropertyOrder(107)]
    [Description("Whether to enable cone limit stabilization")]
    property bool ConeLimitStabilization
    {
      bool get() { return m_bConeLimitStabilization; }
      void set(bool bEnable) { m_bConeLimitStabilization = bEnable; }
    }

    [PropertyOrder(108)]
    [Description("The friction value")]
    property float MaxFrictionTorque
    {
      float get() { return m_fMaxFrictionTorque; }
      void set(float fFriction) { m_fMaxFrictionTorque = hkvMath::Max(0.f, fFriction); }
    }

    [PropertyOrder(109)]
    [Description("The tau factor for angular limits")]
    property float AngularLimitsTauFactor
    {
      float get() { return m_fAngularLimitsTauFactor; }
      void set(float fTau) { m_fAngularLimitsTauFactor = hkvMath::Max(0.f, hkvMath::Min(1.f, fTau)); }
    }

    /// Indicates whether the user will be allowed to rotate the constraint
    virtual bool SupportsRotation() override { return true; }

    /// Visualizes the joint in the engine view
    virtual void Visualize (IVRenderInterface *pRenderer, const hkvVec3& startPos, const hkvVec3& endPos, const hkvVec3& constraintPos, const hkvVec3& constraintDir, const hkvVec3& constraintOri) override;

    // Deserialization method
    virtual void GetObjectData(SerializationInfo ^si, StreamingContext sc) override;

    /// ICloneable implementation
    virtual Object^ Clone() override;

    virtual void CreateHotspots(Shape3D ^pShape) override;

    virtual void DestroyHotspots(Shape3D ^pShape) override;

    virtual void OnHotSpotDragBegin(HotSpotBase ^pHotSpot, VisionViewBase ^pView) override;

    virtual void OnHotSpotDrag(
      HotSpotBase ^pHotSpot, VisionViewBase ^pView, float fDeltaX, float fDeltaY) override;

    virtual void OnHotSpotDragEnd(HotSpotBase ^pHotSpot, VisionViewBase ^pView) override;

    virtual void OnHotSpotEvaluatePosition(HotSpotBase ^pHotSpot) override;

  protected:
    // Serialization constructor
    RagdollConstraintState(SerializationInfo ^si, StreamingContext sc);

    /// Creates a new native description instance (initialized to defaults)
    virtual vHavokConstraintDesc *CreateNativeDesc() override;

    /// Copies the properties from the passed native constraint description into this
    /// managed description.
    virtual void NativeToManaged(const vHavokConstraintDesc &nativeDesc) override;

    /// Copies the properties from this managed joint description to the passed native 
    /// description.
    virtual void ManagedToNative(vHavokConstraintDesc &nativeDesc) override;

    /// Sets anchor specific information and position-related information in the passed 
    /// native description.
    /// This function is called once the native description has been created and the managed
    /// configuration has been set on the native one. It can be used to perform final 
    /// initializations on the native descriptions.
    virtual void SetAnchorDataOnNative(vHavokConstraintDesc &nativeDesc, 
      AnchorDescription ^pFirstAnchor, AnchorDescription ^pSecondAnchor, 
      Vector3F &position, Vector3F &direction, Vector3F &orientation) override;

    // Variables
    float m_fConeAngle;
    float m_fPlaneConeMinAngle;
    float m_fPlaneConeMaxAngle;
    float m_fTwistMinAngle;
    float m_fTwistMaxAngle;
    bool m_bConeLimitStabilization;
    float m_fMaxFrictionTorque;
    float m_fAngularLimitsTauFactor;

  private:
    void CalcRagdollAxes(const hkvVec3& vOrientation, hkvVec3& vTwistAxis, hkvVec3& vPlaneAxis);

    HotSpotConeAngle ^m_pConeAngleHotSpot; ///< a hotspot for the twist axis cone opening angle
    HotSpotConeAngle ^m_pPlaneAngleMinHotSpot; ///< a hotspot for the minimum plane cone opening angle
    HotSpotConeAngle ^m_pPlaneAngleMaxHotSpot; ///< a hotspot for the maximum plane cone opening angle
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
