/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VCommandManager.hpp

#ifndef VISION_VCOMMANDMANAGER_HPP
#define VISION_VCOMMANDMANAGER_HPP

#ifndef _VISION_DOC

#include <Vision/Runtime/Base/Container/VMapStrToPtr.hpp>
#include <Vision/Runtime/Base/Types/VTypeManager.hpp>

struct VCommandType;

/// \brief
///   Class that manages the command type information of the complete application.
/// 
/// Because we now have command type information per plugin and applications,  consisting of many
/// plugins, we have a class that manages the commands type  information of the complete
/// application. An object of the VCommandManager  is not filled automatically with the
/// information. The application can/has to add each plugin with function calls in the code.
/// 
/// The manager has a own list type information (references) which is the sum of all plugins added
/// to that manager instance. There are functions to add  and remove plugins and to get access to
/// the command type information in  multiple ways.
class VCommandManager
{
public:
  
  // constructor
  VBASE_IMPEXP VCommandManager();
  
  /// \brief
  ///   Get the VCommandType static object pointer for the command info structure specified by
  ///   command name.
  /// 
  /// Every command has an static object of type VCommandType holding important run time
  /// information about the command. You can retrieve this object by using this function, from the
  /// global application command type list.
  /// 
  /// \param szCommandName
  ///   command name
  /// 
  /// \return
  ///   VCommandType *: NULL if the command wasn't registered.
  VBASE_IMPEXP VCommandType *FromName(const char *szCommandName);
  
  /// \brief
  ///   Return the registered command count in the global command type list.
  /// 
  /// \return
  ///   int: command type count.
  VBASE_IMPEXP int GetCommandTypeCount();

  /// \brief
  ///   Return a VCommandType object pointer by index.
  /// 
  /// \param iIndex
  ///   index in the applications action-type list.
  /// 
  /// \return
  ///   VCommandType *: NULL if the index was out of bounds.
  /// 
  /// \example
  ///   \code
  ///   for ( int i = 0; i < VCommandManager::GetCommandTypeCount(); i++ )
  ///   {
  ///     VCommandType* pType = g_pApp->m_CommandManager.GetCommandType(i);
  ///     ... do something with pType ...
  ///   }
  ///   \endcode
  VBASE_IMPEXP VCommandType *GetCommandType(UINT iIndex);
 
  /// \brief
  ///   Append all pointers to VCommandType static objects that represent command relevant for the
  ///   active editor state.
  /// 
  /// \param commandTypeList
  ///   list to hold the relevant command types
  VBASE_IMPEXP void GetRelevantCommandList(VPList &commandTypeList);

  /// \brief
  ///   Register all command type information from this plugin;
  /// 
  /// \param pModule
  ///   plugin to register.
  /// 
  /// \return
  ///   VBool: TRUE if succesful.
  VBASE_IMPEXP VBool RegisterModule(VModule *pModule);
  
  /// \brief
  ///   Unregister all command type information registered before with the specified plugin.
  /// 
  /// \param pModule
  ///   plugin to unregister.
  /// 
  /// \return
  ///   VBool: TRUE if succesful.
  VBASE_IMPEXP VBool UnregisterModule(VModule *pModule);


  /// \brief
  ///   returns a pointer to the list of registered modules
  inline VModuleList* GetModuleList()  { return &m_pluginList; }
  
  
protected:
  
  VMapStrToPtr m_commandNameMap; ///< applications global command type list
  VPList m_commandTypeList;
  
  VModuleList m_pluginList; ///< list of registered plugins

  // compare function for sorting the commands
  VBASE_IMPEXP static int VISION_CDECL CommandTypeCompare( const void *arg1, const void *arg2 );
};

#endif // _VISION_DOC

#endif // VISION_VCOMMANDMANAGER_HPP

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
