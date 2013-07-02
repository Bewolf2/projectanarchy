/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisionAppHelpers.hpp

#ifndef VISION_APP_HELPERS_HPP
#define VISION_APP_HELPERS_HPP

#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Runtime/Engine/System/VisApiMain.hpp>
#include <Vision/Runtime/Engine/System/PluginSystem/VisApiPluginManager.hpp>
#include <Vision/Runtime/Engine/System/Error/VisApiError.hpp>

/// \brief
///   This class provides static helper functions for Vision Applications
class VisionAppHelpers
{
public:

  /// \brief
  ///   This static helper function tests whether the system supports the passed pixel- and vertex
  ///   shader version (or higher) and returns the result
  static VISION_APIFUNC bool TestShaderCompatibility(VShaderModel_e ePSVersion = VSM_SM11, VShaderModel_e eVSVersion = VSM_SM11);


  /// \brief
  ///   This static function wraps around TestShaderCompatibility and triggers a fatal error
  ///   message if the return value was false.
  static VISION_APIFUNC void CheckShaderCompatibility(VShaderModel_e ePSVersion = VSM_SM11, VShaderModel_e eVSVersion = VSM_SM11);


  /// \brief
  ///   This static helper function tests whether blending for floating point render targets is
  ///   supported on this system. Naturally this feature is a requirement for full HDR bloom
  ///   effects.
  static VISION_APIFUNC bool IsFloatingPointBlendingSupported();


  /// \brief
  ///   Retrieves the directory of the currently running application
  ///
  /// \param pszAppDir
  ///   Application directory char pointer to set.
  ///
  /// \return
  ///   Whether the function succeeded.
  static inline bool GetApplicationDir(char *pszAppDir)
  {
    return VBaseAppHelpers::GetApplicationDir(pszAppDir);
  }
  

  /// \brief
  ///   Static helper function to set the EXE directory as the current directory to find files like
  ///   plugins.
  /// 
  /// Set the current directory to the EXE dir so the sample can find its files. (We need to do
  /// this since VC2003 sets the project file dir as the default dir and not the EXE. We can change
  /// the startup directory in the settings, but it is in a user specific (SUO) file and not in the
  /// SLN/VCPROJ files we ship).
  static VISION_APIFUNC void MakeEXEDirCurrent();


  ///
  /// @name Visibility Debug Functions
  /// @{
  ///


  /// \brief
  ///   Saves the visibility zone/potal setup to an XML file that can be loaded into the scene
  ///   manager via LoadVisibilitySetup for debug purposes.
  static VISION_APIFUNC bool SaveVisibilitySetup(const char *szFilename, IVFileStreamManager *pFileManager=NULL);

  /// \brief
  ///   Loads a visibility setup from XML and adds it to the scene manager. Useful for debug
  ///   purposes.
  static VISION_APIFUNC bool LoadVisibilitySetup(const char *szFilename, IVFileStreamManager *pFileManager=NULL);

  /// \brief
  ///   Displays visibility info, i.e. zone boxes and portals. When used, this function must be
  ///   called every frame.
  static VISION_APIFUNC void DisplayVisibilityInfo();


  ///
  /// @}
  ///


  ///
  /// @name Geometry Debugging Helpers
  /// @{
  ///


  /// \brief
  ///   This function creates mesh buffer objects from the lightmap mesh information passed to this
  ///   function. vLux uses it for radiosity.
  static VISION_APIFUNC void CreateLightmapMeshes(VLightmapSceneInfo &meshInfo, VLightmapSceneInfo *pLightmapInfo=NULL, IVLightGridTracer_cl *pLightTracer=NULL);
 
  
  ///
  /// @}
  ///

};

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
