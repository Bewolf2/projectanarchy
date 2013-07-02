/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   Scene baker
///
/// Generates per triangle group indices depending on a few user settings
class VGEOM2_IMPEXP_CLASS VGProcessor_GroupIndexGenerator : public VGDynamicProcessor<VGProcessor_GroupIndexGenerator>
{
public:
  VGProcessor_GroupIndexGenerator();
  virtual ~VGProcessor_GroupIndexGenerator();


  enum GroupMode
  {
    VGGM_SPATIALLY_PER_MATERIAL // Groups spatially on a per-material basis
    //@@@ extend e.g. SPATIALLY_PER_MATERIAL_AND_VISIBILITY_ZONE
  };


  /// \brief
  ///   Processes a scene - generates per triangle group indices depending on mode and some flags
  ///
  /// \param scene
  ///   Scene containing triangle data to generate group indices for
  ///
  /// \return
  ///   False if generation failed
  virtual bool      Process(VGScene& scene) const;



  /// \brief
  ///   Sets the desired generation mode
  ///
  /// \param m
  ///   Generation mode
  inline void       SetMode(GroupMode m) throw()          { m_mode = m; }

  /// \brief
  ///   Gets the currently set generation mode
  ///
  /// \return
  ///   Currently set generation mode
  inline GroupMode  GetMode() const throw()               { return m_mode; }



  /// \brief
  ///   Sets the maximum bounding dimensions (along x, y and z) for spatial grouping
  ///
  /// \param s
  ///   Size
  inline void       SetMaxBoxSize(float s) throw()        { m_maxBoundSize = s; }

  /// \brief
  ///   Gets the maximum bounding dimensions (along x, y and z) for spatial grouping
  ///
  /// \return
  ///   Size
  inline float      GetMaxBoxSize() const throw()         { return m_maxBoundSize; }



private:
  GroupMode m_mode;
  float m_maxBoundSize;
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
