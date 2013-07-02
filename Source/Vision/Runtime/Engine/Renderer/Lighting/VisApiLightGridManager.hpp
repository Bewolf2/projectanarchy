/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file

#ifndef VISAPILIGHTGRIDMANAGER_HPP_INCLUDED
#define VISAPILIGHTGRIDMANAGER_HPP_INCLUDED

/// \brief
///   Resource manager class that keeps track of all light grids loaded in a scene 
/// 
/// One global instance of this class can be accessed via VisLightGridManager_cl::GlobalManager().
class VisLightGridManager_cl : public VisResourceManager_cl
{
public:
  /// \brief
  ///   Constructor 
  VISION_APIFUNC VisLightGridManager_cl(const char *szManagerName="LightGrids", int iFlags=VRESOURCEMANAGERFLAG_NONE, int iMemLimit=0);

  /// \brief
  ///   Overridden CreateResource function that loads a light grid with specified filename 
  /// 
  /// This function always returns a valid lightgrid. If the file cannot be opened, then this
  /// function creates a plain color lightgrid (using Vision::Renderer.GetDefaultLightingColor())
  /// and adds a warning entry to the report dialog.
  ///
  /// \param szFilename
  ///   Lightgrid to load.
  ///
  /// \param pExtraInfo
  ///   Not used.
  VISION_APIFUNC VOVERRIDE VManagedResource *CreateResource(const char *szFilename, VResourceSnapshotEntry *pExtraInfo);

  /// \brief
  ///   Wraps around LoadResource/CreateResource
  inline VLightGrid_cl *LoadLightGrid(const char *szFilename)
  {
    return (VLightGrid_cl *)LoadResource(szFilename);
  }

  /// \brief
  ///   Static function to access the engine's singleton 
  VISION_APIFUNC static VisLightGridManager_cl& GlobalManager();

  /// \brief
  ///   There are two types of light grids. The 'simple' one is usually used on mobile devices. It stores less data and is easier to implement in a shader than the 'Default' one.
  enum LightGridMode
  {
    Simple,   ///< The simple light grid only stores one color and the main light direction.
    Default   ///< The default light grid stores 6 colors for each direction.
  };

  /// \brief
  ///   Sets the global mode for all light grids.
  ///
  /// When this function is called, all loaded light grids are replaced by their proper variant. I.e., if a 'default' 
  /// light grid is loaded, but the mode is changed to 'simple', all light grids are reloaded with their 'simple' version.
  ///
  /// All light grids that are loaded after this function was called will use the proper mode.
  ///
  /// \param Mode
  ///   the light grid mode to set
  ///
  /// \note
  ///   If the light grid mode is set to 'simple' or 'default', but no "*.vslg" or "*.vlg" file exists, 
  ///   a default light grid is created.
  VISION_APIFUNC void SetLightGridMode(VisLightGridManager_cl::LightGridMode Mode);

  /// \brief
  ///   Returns the currently set light grid mode.
  LightGridMode GetLightGridMode() const { return m_LightGridMode; }

  /// \brief
  ///   Returns the light grid filename for a given base file (e.g. scene file). Takes light grid mode into account.
  VISION_APIFUNC VString GetLightGridFilename(const char *szBaseFilename);
  
private:
  static VisLightGridManager_cl g_GlobalManager;

  LightGridMode m_LightGridMode;
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
