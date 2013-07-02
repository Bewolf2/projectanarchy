/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   Geometry info class
///
/// Describes and sets up additional geometry properties that can be referenced on a per triangle level.
class VGEOM2_IMPEXP_CLASS VGGeometryInfo : public VGNameable
{
public:

  /// \brief
  ///   Geometry flags
  ///
  /// Stores geometry specific flags.
  enum GeometryFlags
  {
    VGGF_NOTHING              = 0,
    VGGF_CAST_DYNAMIC_SHADOWS = V_BIT(0),
    VGGF_CAST_STATIC_SHADOWS  = V_BIT(1)
  };


public:
  VGGeometryInfo();
  virtual ~VGGeometryInfo();



  /// \brief
  ///   Gets this geometry info's flags
  ///
  /// \return
  ///   Geometry info's flags
  inline GeometryFlags    GetGeometryFlags() const throw()                    { return m_flags; }

  /// \brief
  ///   Sets this geometry info's flags
  ///
  /// \param f
  ///   Geometry info flags to set
  inline void             SetGeometryFlags(GeometryFlags f) throw()           { m_flags = f; }



  /// \brief
  ///   Gets user defined tag
  ///
  /// \return
  ///   Tag value
  inline unsigned int     GetTag() const throw()                              { return m_tag; }

  /// \brief
  ///   Sets user defined tag
  ///
  /// \param t
  ///   Tag value to set
  inline void             SetTag(unsigned int t) throw()                      { m_tag = t; }

  /// \brief
  ///   Gets user flags
  ///
  /// \return
  ///   User flags
  inline unsigned short   GetUserFlags() const throw()                        { return m_userFlags; }

  /// \brief
  ///   Sets user flags
  ///
  /// \param f
  ///   User flags to set
  inline void             SetUserFlags(unsigned short f) throw()              { m_userFlags = f; }

  /// \brief
  ///   Gets this geometry info's custom visibility mask
  ///
  /// \return
  ///   Geometry info's custom visibility mask
  inline unsigned int     GetVisibilityMask() const throw()                   { return m_visibleMask; }

  /// \brief
  ///   Sets this geometry info's custom visibility mask
  ///
  /// \param m
  ///   Geometry info's custom visibility mask
  inline void             SetVisibilityMask(unsigned int m) throw()           { m_visibleMask = m; }

  /// \brief
  ///   Gets this geometry info's custom light mask
  ///
  /// \return
  ///   Geometry info's custom light mask
  inline unsigned short   GetLightMask() const throw()                        { return m_lightMask; }

  /// \brief
  ///   Sets this geometry info's custom light mask
  ///
  /// \param m
  ///   Geometry info's custom light mask
  inline void             SetLightMask(unsigned short m) throw()              { m_lightMask = m; }

  /// \brief
  ///   Gets this geometry info's custom trace mask
  ///
  /// \return
  ///   Geometry info's custom trace mask
  inline unsigned short   GetTraceMask() const throw()                        { return m_traceMask; }

  /// \brief
  ///   Sets this geometry info's custom trace mask
  ///
  /// \param m
  ///   Geometry info's custom trace mask
  inline void             SetTraceMask(unsigned short m) throw()              { m_traceMask = m; }



  /// \brief
  ///   Gets near clip distance
  ///
  /// \return
  ///   Near clip distance value
  inline float            GetNearClipDistance() const throw()                 { return m_nearClipDistance; }

  /// \brief
  ///   Gets far clip distance
  ///
  /// \return
  ///   Far clip distance value
  inline float            GetFarClipDistance() const throw()                  { return m_farClipDistance; }

  /// \brief
  ///   Sets near and far clip distance values
  ///
  /// \param n
  ///   Value to set as near clip distance
  ///
  /// \param f
  ///   Value to set as far clip distance
  inline void             SetClipDistances(float n, float f) throw()          { m_nearClipDistance = n; m_farClipDistance = f; }

  /// \brief
  ///   Sets the LOD index
  inline void             SetLODIndex(short iLOD)                             {m_LODIndex = iLOD;}

  /// \brief
  ///   Gets the LOD index
  inline short            GetLODIndex() const                                 {return m_LODIndex;}

  /// \brief
  ///   Gets the position the near and far clip distances are relative to (space unspecified, backends use scene/global space in this case)
  ///
  /// \return
  ///   Reference position clipping planes are relative to
  inline const hkvVec3& GetClipReference() const                              { return m_clipReference; }

  /// \brief
  ///   Sets the position the near and far clip distances are relative to (space unspecified, backends use scene/global space in this case)
  ///
  /// \param v
  ///   Reference position clipping planes are relative to
  inline void             SetClipReference(const hkvVec3& v)                  { m_clipReference = v; }



  /// \brief
  ///   Comparison operator
  ///
  /// \param rhs
  ///   Object for comparison
  ///
  /// \return
  ///   Comparison result
  inline bool operator==(const VGGeometryInfo& rhs) const  { return (m_name == rhs.m_name) && (m_flags==rhs.m_flags) && (m_userFlags==rhs.m_userFlags) && (m_LODIndex==rhs.m_LODIndex) && (m_tag==rhs.m_tag) && (m_visibleMask==rhs.m_visibleMask) && (m_lightMask==rhs.m_lightMask) && (m_traceMask==rhs.m_traceMask) && (m_nearClipDistance==rhs.m_nearClipDistance) && (m_farClipDistance==rhs.m_farClipDistance) && (m_clipReference==rhs.m_clipReference); }
  inline bool operator!=(const VGGeometryInfo& rhs) const  { return !(*this == rhs); }




private:
  GeometryFlags   m_flags;
  short           m_LODIndex;
  unsigned short  m_userFlags;
  unsigned int    m_tag;

  unsigned int    m_visibleMask;
  unsigned short  m_lightMask;
  unsigned short  m_traceMask;

  float           m_nearClipDistance;
  float           m_farClipDistance;
  hkvVec3         m_clipReference;
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
