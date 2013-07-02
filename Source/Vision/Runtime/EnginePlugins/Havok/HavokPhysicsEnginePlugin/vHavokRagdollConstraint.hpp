/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef V_HAVOK_RAGDOLL_CONSTRAINT_HPP_INCLUDED
#define V_HAVOK_RAGDOLL_CONSTRAINT_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraint.hpp>

/// \brief
///   Descriptor for vHavok Ragdoll constraint.
class vHavokRagdollConstraintDesc : public vHavokConstraintDesc
{
public:
  VHAVOK_IMPEXP vHavokRagdollConstraintDesc();

  VHAVOK_IMPEXP virtual void Reset();

  V_DECLARE_SERIAL_DLLEXP( vHavokRagdollConstraintDesc, VHAVOK_IMPEXP )
    VHAVOK_IMPEXP VOVERRIDE void Serialize( VArchive &ar );

public:
  hkvVec3 m_vPivot; ///< The constraint's pivot point in world space
  hkvVec3 m_vTwistAxis; ///< The constraint's twist axis (see Havok documentation)
  hkvVec3 m_vPlaneAxis; ///< The constraint's plane axis (see Havok documentation)

  float m_fConeAngle; ///< The opening half-angle (in radians) of the cone around the twist axis
  float m_fPlaneConeMinAngle; ///< The opening half-angle (in radians) of the plane min cone (negative plane axis)
  float m_fPlaneConeMaxAngle; ///< The opening half-angle (in radians) of the plane max cone (positive plane axis)
  float m_fTwistMinAngle; ///< The minimum twist angle (in radians)
  float m_fTwistMaxAngle; ///< The maximum twist angle (in radians)

  bool m_bConeLimitStabilization; ///< Whether cone limit stabilization is enabled
  float m_fMaxFrictionTorque; ///< The maximum friction value for the constraint
  float m_fAngularLimitsTauFactor; ///< The tau factor for the angular limits [0..1]

private:
  static const unsigned int s_iSerialVersion;
};

// ----------------------------------------------------------------------------
// vHavokRagdollConstraint
// ----------------------------------------------------------------------------

/// \brief
///   Implementation of the vHavok Ragdoll constraint.
class vHavokRagdollConstraint : public vHavokConstraint
{
public:
  // serialization and type management
  V_DECLARE_SERIAL_DLLEXP( vHavokRagdollConstraint, VHAVOK_IMPEXP )
    VHAVOK_IMPEXP VOVERRIDE void Serialize( VArchive &ar );

  ///@name Base Class Overrides
  //@{
protected:
  VHAVOK_IMPEXP virtual hkpConstraintData* CreateConstraintData();
  VHAVOK_IMPEXP virtual vHavokConstraintDesc *CreateConstraintDesc();
  VHAVOK_IMPEXP virtual void InitConstraintDataFromDesc(hkpConstraintData& data, vHavokConstraintDesc const& desc);
public:
  VHAVOK_IMPEXP virtual void SaveToDesc(vHavokConstraintDesc& desc);
  //@}

  ///@name Motors
  //@{
public:
  /// \brief
  ///   Enables a position-based motor for the three degrees of freedom of this constraint.
  /// \param fTau
  ///   the tau value to use in solving
  /// \param fDamping
  ///   the damping to use in solving
  /// \param fProportionalVelocity
  ///   the proportional component of the velocity with which position errors are corrected
  /// \param fConstantVelocity
  ///   the constant component of the velocity with which position errors are corrected
  VHAVOK_IMPEXP void EnablePositionMotor(float fTau, float fDamping, float fProportionalVelocity, float fConstantVelocity);
  
  /// \brief
  ///   Enables a spring-damper-based motor for the three degrees of freedom of this constraint.
  /// \param fSpringConstant
  ///   the spring constant of the spring to be used
  /// \param fSpringDamping
  ///   the damping of the spring to be used
  VHAVOK_IMPEXP void EnableSpringDamperMotor(float fSpringConstant, float fSpringDamping);

  /// \brief
  ///   Disables any motor that has been set before. Has no effect if the constraint
  ///   currently has no motor.
  VHAVOK_IMPEXP void DisableMotor();

  /// \brief
  ///   Sets the min and max limits of the force the constraint motor may use.
  /// \param fMin
  ///   the lower limit of the force
  /// \param fMax
  ///   the upper limit of the force
  VHAVOK_IMPEXP void SetMotorForces(float fMin, float fMax);

  /// \brief
  ///   Sets the target orientation the constraint motors should try to achieve.
  ///
  /// This orientation (given as Euler angles in radians) is relative to the 
  /// twist axis direction of the constraint. That is, the orientation (0, 0, 0)
  /// points straight along the twist axis.
  ///
  /// \param fYaw
  ///   the yaw component of the target orientation
  /// \param fPitch
  ///   the pitch component of the target orientation
  /// \param fRoll
  ///   the roll component of the target orientation
  VHAVOK_IMPEXP void SetTargetOrientation(float fYaw, float fPitch, float fRoll);
  //@}

private:
  static const unsigned int s_iSerialVersion;

  hkvVec3 m_vSavedPivot; ///< Saved constraint pivot point (world space)
  hkvVec3 m_vSavedTwistAxis; ///< Saved constraint twist axis (world space)
  hkvVec3 m_vSavedPlaneAxis; ///< Saved constraint plane axis (world space)
};

#endif //V_HAVOK_RAGDOLL_CONSTRAINT_HPP_INCLUDED

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
