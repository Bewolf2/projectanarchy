/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef V_HAVOK_PRISMATIC_CONSTRAINT_HPP_INCLUDED
#define V_HAVOK_PRISMATIC_CONSTRAINT_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraint.hpp>

/// \brief
///   Descriptor for vHavok Prismatic constraint.
class vHavokPrismaticConstraintDesc : public vHavokConstraintDesc
{
public:
  VHAVOK_IMPEXP vHavokPrismaticConstraintDesc();

  VHAVOK_IMPEXP virtual void Reset();

  V_DECLARE_SERIAL_DLLEXP( vHavokPrismaticConstraintDesc, VHAVOK_IMPEXP )
  VHAVOK_IMPEXP VOVERRIDE void Serialize( VArchive &ar );

public:
  hkvVec3 m_vPrismaticPivot;   ///< The prismatic constraint's pivot point in world space
  hkvVec3 m_vPrismaticAxis;    ///< The prismatic constraint's axis
  hkReal m_fLinearMin;               ///< The minimum limit for the attached body's movement along the axis
  hkReal m_fLinearMax;               ///< The maximum limit for the attached body's movement along the axis
  bool m_bAllowRotationAroundAxis;  ///< This allows to change the prismatic constraint into a cylindrical constraint, where rotation around the shift axis is allowed

private:
  static const unsigned int s_iSerialVersion;
};

// ----------------------------------------------------------------------------
// vHavokPrismaticConstraint
// ----------------------------------------------------------------------------

/// \brief
///   Implementation of the vHavok Prismatic constraint.
class vHavokPrismaticConstraint : public vHavokConstraint
{
public:
  // serialization and type management
  V_DECLARE_SERIAL_DLLEXP( vHavokPrismaticConstraint, VHAVOK_IMPEXP )
  VHAVOK_IMPEXP VOVERRIDE void Serialize( VArchive &ar );

protected:
  VHAVOK_IMPEXP virtual hkpConstraintData* CreateConstraintData();
  VHAVOK_IMPEXP virtual vHavokConstraintDesc *CreateConstraintDesc();
  VHAVOK_IMPEXP virtual void InitConstraintDataFromDesc(hkpConstraintData& data, vHavokConstraintDesc const& desc);
public:
  VHAVOK_IMPEXP virtual void SaveToDesc(vHavokConstraintDesc& desc);

private:
  static const unsigned int s_iSerialVersion;

  hkvVec3 m_vSavedPivot;       ///< Saved prismatic pivot point (local space of first body)
  hkvVec3 m_vSavedAxis;        ///< Saved prismatic pivot axis (local space of first body)
  bool m_bAllowRotationAroundAxis;  ///< Whether rotation around the shift axis is allowed

};

#endif //V_HAVOK_PRISMATIC_CONSTRAINT_HPP_INCLUDED

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
