/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiMaterialTemplateManager.hpp

#ifndef VISAPIMATERIALTEMPLATEMANAGER_HPP_INCLUDED
#define VISAPIMATERIALTEMPLATEMANAGER_HPP_INCLUDED

#include <Vision/Runtime/Engine/Material/VisApiMaterialTemplateResource.hpp>

/// \brief
///   Resource manager class that keeps track of all material templates that are references by VisSurface_cl.
/// 
/// One global instance of this class can be accessed via VMaterialTemplateManager::GlobalManager().
class VMaterialTemplateManager : public VisResourceManager_cl
{
public:
  /// \brief
  ///   Constructor 
  VISION_APIFUNC VMaterialTemplateManager(const char* szManagerName, int iFlags = VRESOURCEMANAGERFLAG_NONE, int iMemLimit = 0);

  /// \brief
  ///   Overridden CreateResource function that loads a material template with the specified filename.
  /// 
  /// In case the given file cannot be loaded, a warning entry to the report dialog and any
  /// VisSurface_cl that fails to load its VMaterialTemplate will revert to auto shader assignment.
  ///
  /// \param szFilename
  ///   Material template to load.
  ///
  /// \param pExtraInfo
  ///   Not used.
  ///
  /// \returns
  ///   VManagedResource* pResource: The loaded material template.
  VISION_APIFUNC virtual VManagedResource* CreateResource(const char* szFilename, VResourceSnapshotEntry* pExtraInfo) HKV_OVERRIDE;

  /// \brief
  ///   Static function to access the engine's singleton.
  VISION_APIFUNC static VMaterialTemplateManager &GlobalManager();

private:
  static VMaterialTemplateManager g_GlobalManager;
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
