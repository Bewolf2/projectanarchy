/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file IVMenuSystemResource.hpp

#ifndef IVMENUSYSTEMRESOURCE_HPP_INCLUDED
#define IVMENUSYSTEMRESOURCE_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VGUIManager.hpp>

/// \brief
///   Resource base class for any GUI related resource
class IVMenuSystemResource : public VManagedResource, public VUserDataObj
{
public:

  /// \brief
  ///   Constructor for loading from file (filename can be NULL)
  inline IVMenuSystemResource(VGUIManager *pManager, const char *szFilename, VGUIManager::ResourceType type) 
    : VManagedResource(pManager) 
  {
    m_Type=type;
    SetFilename(szFilename);
  }

  /// \brief
  ///   Overridable to get relevant info from an XML node
  GUI_IMPEXP virtual bool Parse(TiXmlElement *pNode, const char *szPath);

  /// \brief
  ///   Returns the type of resource (cursor, dialog,...)
  inline VGUIManager::ResourceType GetResourceType() const {return m_Type;}

  /// \brief
  ///   Returns the GUI manager that manages this resource
  inline VGUIManager* GetMenuManager() const {return (VGUIManager *)GetParentManager();}

  /// \brief
  ///   Helper function to extract the file path
  inline void GetFilePath(char *szRes) {szRes[0]=0;VFileHelper::GetFileDir(GetFilename(),szRes);}

  VString m_sResourceName;
protected:

  // overridden resource function
  GUI_IMPEXP virtual int GetAdditionalOutputString(char *szDestBuffer, int iMaxChars) 
  {
  #pragma warning(push)
  #pragma warning(disable:4996)
    return sprintf(szDestBuffer,"%s:%s", VGUIManager::GetResourceTypeStr(m_Type),m_sResourceName.AsChar());
  #pragma warning(pop)
  }


  VGUIManager::ResourceType m_Type;
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
