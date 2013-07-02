/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VScriptResource.hpp

#ifndef VSCRIPTRESOURCE_HPP_INCLUDED
#define VSCRIPTRESOURCE_HPP_INCLUDED

/// \brief
///   Vision resource class that represents the source code side of a script file. Script instances
///   share the same state via the resource
class VScriptResource : public VManagedResource
{
public:

  /// \fn SCRIPT_IMPEXP VScriptResource(VScriptResourceManager *pManager)
  ///
  /// \brief  Script Resource constructor. 
  ///
  /// \param [in] pManager  The script manager responsible for this resource. 
  ///
  SCRIPT_IMPEXP VScriptResource(VScriptResourceManager *pManager);

  /// \fn SCRIPT_IMPEXP virtual ~VScriptResource()
  ///
  /// \brief  Destructor. 
  ///
  SCRIPT_IMPEXP virtual ~VScriptResource();

  /// \fn VScriptResourceManager* GetScriptManager() const
  ///
  /// \brief  Gets the script manager responsible for this resource. 
  ///
  /// \return The script manager responsible for this resource. 
  ///
  VScriptResourceManager* GetScriptManager() const {return (VScriptResourceManager *)GetParentManager();}

  /// \fn SCRIPT_IMPEXP VScriptInstance* CreateScriptInstance()
  ///
  /// \brief  Creates a script instance from this resource. 
  ///
  /// \return A new script instance. 
  ///
  SCRIPT_IMPEXP VScriptInstance* CreateScriptInstance();

  /// \fn SCRIPT_IMPEXP virtual BOOL Reload()
  ///
  /// \brief  Reloads the resource. 
  ///
  /// \return true if it succeeds, false if it fails. 
  ///
  SCRIPT_IMPEXP virtual BOOL Reload() HKV_OVERRIDE;

  /// \fn SCRIPT_IMPEXP virtual BOOL Unload()
  ///
  /// \brief  Unloads the resource. 
  ///
  /// \return true if it succeeds, false if it fails. 
  ///
  SCRIPT_IMPEXP virtual BOOL Unload() HKV_OVERRIDE;

  /// \fn SCRIPT_IMPEXP virtual IVSerializationProxy *CreateProxy()
  ///
  /// \brief  Creates the serialization proxy. 
  ///
  /// \return The serialization proxy. 
  ///
  SCRIPT_IMPEXP virtual IVSerializationProxy *CreateProxy();

  /// \fn SCRIPT_IMPEXP void ReloadAndReplace(char* pNewContent = NULL)
  ///
  /// \brief  Reloads the script (script functions) but doesn't replace the table for this resource. 
  ///
  /// \param [in] pNewContent If non-null, the new script content. 
  ///
  SCRIPT_IMPEXP void ReloadAndReplace(char* pNewContent = NULL);

  SCRIPT_IMPEXP virtual void UnloadAndReload(BOOL bUnload, BOOL bReload) HKV_OVERRIDE;

 // VString m_sScriptText; // we don't need the script text outside Reload()
 // int m_iScriptLen;

  lua_State* m_pResourceState;
  int m_iKey;

private:
  static char *StripUTF8BOM(char* szScriptIn, int& iScriptLenInOut);
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
