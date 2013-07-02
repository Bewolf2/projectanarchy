/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   Material Merger
///
/// Merges multiple materials based on their *name*, following
/// to a user-settable merge-scheme
class VGEOM2_IMPEXP_CLASS VGProcessor_MaterialMerger : public VGDynamicProcessor<VGProcessor_MaterialMerger>
{
public:
  // Defines how the merge should be applied.
  enum MergeScheme
  {
    VGMS_NOTHING,
    VGMS_FORWARD,  // Loops over materials from front to back, first occurance of a material name has precedence.
    VGMS_BACKWARD  // Same as VMMS_FORWARD, but inversed.
  };


public:
  VGProcessor_MaterialMerger() : m_mergeScheme(VGMS_NOTHING), m_ignoreCase(false)   { }
  virtual ~VGProcessor_MaterialMerger()                                             { }



  /// \brief
  ///   Processes a scene - merges materials based on their *name*
  ///
  /// \param scene
  ///   Scene containing materials to merge
  ///
  /// \return
  ///   False if merge failed
  virtual bool       Process(VGScene& scene) const;



  /// \brief
  ///   Sets the material merge-scheme to be applied
  ///
  /// \param s
  ///   Merge-scheme
  inline void        SetMergeScheme(MergeScheme s) throw()   { m_mergeScheme = s; }

  /// \brief
  ///   Gets the material merge-scheme
  ///
  /// \return
  ///   Material merge scheme in use
  inline MergeScheme GetMergeScheme() const throw()          { return m_mergeScheme; }



  /// \brief
  ///   Sets whether to compare material names case insensitively, or not
  ///
  /// \param b
  ///   Case insensitivity
  inline void        SetCaseInsensitivity(bool b) throw()    { m_ignoreCase = b; }

  /// \brief
  ///   Gets whether material names are compared case insensitively, or not
  ///
  /// \return
  ///   Case insensitivity
  inline bool        GetCaseInsensitivity() const throw()    { return m_ignoreCase; }



private:
  MergeScheme m_mergeScheme;
  bool        m_ignoreCase;
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
