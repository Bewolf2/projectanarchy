/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   Physics info class
///
/// Describes and sets up additional physics properties to be referenced on a per triangle level.
class VGEOM2_IMPEXP_CLASS VGPhysicsInfo : public VGNameable
{
public:

  /// \brief
  ///   Physics flags
  ///
  /// Stores physics specific flags.
  enum PhysicsFlags
  {
    VGPF_NOTHING                  = 0,
    VGPF_USE_ANISOTROPIC_FRICTION = V_BIT(0)
  };


public:
  VGPhysicsInfo();
  virtual ~VGPhysicsInfo();



  /// \brief
  ///   Gets flags
  ///
  /// \return
  ///   Flags
  inline PhysicsFlags     GetFlags() const throw()                            { return m_flags; }

  /// \brief
  ///   Sets flags
  ///
  /// \param f
  ///   Flags to set
  inline void             SetFlags(PhysicsFlags f) throw()                    { m_flags = f; }

  /// \brief
  ///   Gets static friction
  ///
  /// \return
  ///   Static friction
  inline float            GetStaticFriction() const throw()                   { return m_staticFriction; }

  /// \brief
  ///   Sets static friction
  ///
  /// \param f
  ///   Static friction
  inline void             SetStaticFriction(float f) throw()                  { m_staticFriction = f; }

  /// \brief
  ///   Gets dynamic friction
  ///
  /// \return
  ///   Dynamic friction
  inline float            GetDynamicFriction() const throw()                  { return m_dynamicFriction; }

  /// \brief
  ///   Sets dynamic friction
  ///
  /// \param f
  ///   Dynamic friction
  inline void             SetDynamicFriction(float f) throw()                 { m_dynamicFriction = f; }

  /// \brief
  ///   Gets restitution
  ///
  /// \return
  ///   Restitution
  inline float            GetRestitution() const throw()                      { return m_restitution; }

  /// \brief
  ///   Sets restitution
  ///
  /// \param r
  ///   Restitution
  inline void             SetRestitution(float r) throw()                     { m_restitution = r; }

  /// \brief
  ///   Gets anisotropic friction direction
  ///
  /// \return
  ///   Anisotropic friction direction
  inline const hkvVec3&   GetAnisotropicFrictionDirection() const throw()     { return m_anisoFrictionDir; }

  /// \brief
  ///   Sets anisotropic friction direction
  ///
  /// \param d
  ///   Anisotropic friction direction
  inline void             SetAnisotropicFrictionDirection(const hkvVec3& d)   { m_anisoFrictionDir = d; }

  /// \brief
  ///   Gets static anisotropic friction
  ///
  /// \return
  ///   Static anisotropic friction
  inline float            GetStaticAnisotropicFriction() const throw()        { return m_staticAnisoFriction; }

  /// \brief
  ///   Sets static anisotropic friction
  ///
  /// \param f
  ///   Static anisotropic friction
  inline void             SetStaticAnisotropicFriction(float f) throw()       { m_staticAnisoFriction = f; }

  /// \brief
  ///   Gets dynamic anisotropic friction
  ///
  /// \return
  ///   Dynamic anisotropic friction
  inline float            GetDynamicAnisotropicFriction() const throw()       { return m_dynamicAnisoFriction; }

  /// \brief
  ///   Sets dynamic anisotropic friction
  ///
  /// \param f
  ///   Dynamic anisotropic friction
  inline void             SetDynamicAnisotropicFriction(float f) throw()      { m_dynamicAnisoFriction = f; }

  /// \brief
  ///   Gets collision bitmask
  ///
  /// \return
  ///   Collision bitmask (usage unspecified)
  inline unsigned int     GetCollisionBitmask() const throw()                 { return m_bitmask; }

  /// \brief
  ///   Sets collision bitmask
  ///
  /// \param b
  ///   Collision bitmask to set (usage unspecified)
  inline void             SetCollisionBitmask(unsigned int b) throw()         { m_bitmask = b; }

  /// \brief
  ///   Gets group filter
  ///
  /// \return
  ///   Collision group filter (usage unspecified, Vision backends use it for Havok collision group filter)
  inline unsigned int     GetGroupFilter() const throw()                      { return m_groupFilter; }

  /// \brief
  ///   Sets group filter
  ///
  /// \param f
  ///   Group filter to set (usage unspecified, Vision backends use it for Havok collision group filter)
  inline void             SetGroupFilter(unsigned int f) throw()              { m_groupFilter = f; }

  /// \brief
  ///   Gets this physics info's user data string
  ///
  /// \return
  ///   Physics info's user data string
  inline const VString&   GetUserDataString() const throw()                   { return m_userDataString; }

  /// \brief
  ///   Sets this physics info's user data string
  ///
  /// \param userData
  ///   User data string to assign
  inline void             SetUserDataString(const VString& userData)          { m_userDataString = userData; }



  /// \brief
  ///   Comparison operator
  ///
  /// \param rhs
  ///   Object for comparison
  ///
  /// \return
  ///   Comparison result
  bool operator==(const VGPhysicsInfo& rhs) const;
  inline bool operator!=(const VGPhysicsInfo& rhs) const  { return !(*this == rhs); }



private:
  PhysicsFlags  m_flags;

  float         m_staticFriction, m_dynamicFriction;
  float         m_restitution;

  hkvVec3       m_anisoFrictionDir;
  float         m_staticAnisoFriction, m_dynamicAnisoFriction;

  unsigned int  m_bitmask;
  unsigned int  m_groupFilter;

  VString       m_userDataString;
};

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
