/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   Mesh merger
///
/// Merges all the meshes in a scene depending on some user settable parameters
class VGEOM2_IMPEXP_CLASS VGProcessor_Merger : public VGDynamicProcessor<VGProcessor_Merger>
{
public:
  enum MergeFlags
  {
    VGMF_NOTHING        = 0,
    VGMF_MERGE          = V_BIT(1),  // Merge - if not set, meshes will be appended, only.
    VGMF_ONLYCOMPATIBLE = V_BIT(2)   // Merge compatible meshes only - if not set, meshes will be made compatible.
  };


public:
  VGProcessor_Merger();
  virtual ~VGProcessor_Merger();



  /// \brief
  ///   Processes a scene - merges meshes in scene
  ///
  /// \param scene
  ///   Scene containing meshes to merge
  ///
  /// \return
  ///   False if generation failed
  virtual bool      Process(VGScene& scene) const;



  /// \brief
  ///   Sets the desired merge flags
  ///
  /// \param e
  ///   Merge flags
  inline void       SetFlags(MergeFlags e) throw()      { m_flags = e; }

  /// \brief
  ///   Gets the currently set merge flags
  ///
  /// \return
  ///   Currently set merge flags
  inline MergeFlags GetFlags() const throw()            { return m_flags; }



private:
  MergeFlags m_flags;
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
