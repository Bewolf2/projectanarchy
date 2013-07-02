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
/// Runs over a scene and makes splits where portals are placed, applies visibility IDs to portals, then generates
/// matching per-triangle visibility IDs
class VGEOM2_IMPEXP_CLASS VGProcessor_VisibilityInfoGenerator : public VGDynamicProcessor<VGProcessor_VisibilityInfoGenerator>
{
public:
  VGProcessor_VisibilityInfoGenerator();
  virtual ~VGProcessor_VisibilityInfoGenerator();



  /// \brief
  ///   Processes a scene - generates visibility informations for a scene
  ///
  /// \param scene
  ///   Scene containing triangle data to generate visibility info for
  ///
  /// \return
  ///   False if generation failed
  virtual bool            Process(VGScene& scene) const;



  /// \brief
  ///   Sets an optional output stream for debug visualization in form of the vVisViewer file format
  ///
  /// \param os
  ///   Output stream for debug info file
  inline void             SetDebugInfoOutStream(IVFileOutStream* os) throw()                { m_debugInfoOutStream = os; }  // @@@ test

  /// \brief
  ///   Gets the currently set debug info output stream (see setter for details)
  ///
  /// \return
  ///   Output stream for debug info file in use
  inline IVFileOutStream* GetDebugInfoOutStream() const throw()                             { return m_debugInfoOutStream; }  // @@@ test



private:
  size_t DumpStateToDebugInfoStream(const VString& stepName, const VGScene& scene, const VMapIntToInt& triDebugTimestamps, int meshIndexForTimeStamps) const;



private:
  IVFileOutStream* m_debugInfoOutStream;
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
