/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef DEFINE_VISAPIMANIFEST
#define DEFINE_VISAPIMANIFEST

#include <Vision/Runtime/Engine/System/VisApiDefs.hpp>

/// \brief
///   This class is used to load and create the vForgeManifest.txt files.
///
/// These files are stored in the project's root or in a scene's data folder and
/// store the engine and vForge plugins that are loaded in vForge as well as
/// the data directories that need to be mapped.
/// This information is also read by the vSceneViewer in order to preview scenes
/// that use this feature.
///
/// The currently supported commands that are stored in the manifest file are (in the form of \<COMMAND\>:\<VALUE\>):
/// \li LoadEnginePlugin:\<pluginname\>
/// \li LoadEditorPlugin:\<editorpluginname\>
/// \li AddDataDir:\<directory\>
class VManifest
{
public:
  ///
  /// @name VManifestEntry Struct
  /// @{
  ///

  /// \brief
  ///   Struct that holds the two parameters of a manifest entry.
  struct VManifestEntry
  {
    VString m_command;
    VString m_value;
  };

  ///
  /// @}
  ///


  ///
  /// @name Constructor
  /// @{
  ///

  /// \brief
  ///   Constructor.

  VISION_APIFUNC VManifest();

  /// \brief
  ///   Destructor.
  VISION_APIFUNC ~VManifest();

  ///
  /// @}
  ///

  ///
  /// @name Save and Load
  /// @{
  ///

  /// \brief
  ///   Tries to load the given manifest file.
  ///
  /// \param szVForgeManifestPath
  ///   Relative or absolute path to the vForgeManifest.txt that should be loaded.
  ///
  /// \return
  ///   \c true if the file could be loaded.
  VISION_APIFUNC bool LoadManifest(const char* szVForgeManifestPath);

  /// \brief
  ///   Tries to save the manifest file.
  ///
  /// \param szVForgeManifestPath
  ///   Path to the vForgeManifest.txt that should be saved.
  ///
  /// \return
  ///   \c true if the file could be written.
  VISION_APIFUNC bool SaveManifest(const char* szVForgeManifestPath);

  ///
  /// @}
  ///


  ///
  /// @name Manifest Editing
  /// @{
  ///

  /// \brief
  ///   Removes all entries from the manifest.
  VISION_APIFUNC void ClearEntries();

  /// \brief
  ///   Appends an entry to the manifest
  VISION_APIFUNC void AppendEntry(const char* szCommand, const char* szValue);

  /// \brief
  ///   Writes the number of entries and the start of the entry data block to the given parameters.
  VISION_APIFUNC void GetEntries(int& out_iNumEntries, VManifestEntry*& out_pEntries);

  ///
  /// @}
  ///

private: // functions


private: //data
  VArray<VManifestEntry> m_entries;
};

#pragma once
#endif

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
