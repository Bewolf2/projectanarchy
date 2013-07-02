/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VCommand.hpp

#ifndef VISION_VCOMMAND_HPP
#define VISION_VCOMMAND_HPP

#ifndef _VISION_DOC

#define V_REGISTER_COMMAND(command, do_ptr, isRel_ptr, group_name, command_name, description, bitmap_name, plugin_source) \
  VCommandType command_##command(do_ptr, isRel_ptr, group_name, command_name, description, bitmap_name, plugin_source)


typedef VBool(*V_COMMAND_FUNCPTR)();


/// \brief
///   Structure holding information about a command.
struct VBASE_IMPEXP_CLASS VCommandType
{
  // constructor to initializate all members
  VCommandType(V_COMMAND_FUNCPTR do_ptr, V_COMMAND_FUNCPTR isRel_ptr, const char* szGroup, const char* szCommand,
                const char *szDescription, const char *szBitmap, VModule *pPlugin);

  V_COMMAND_FUNCPTR DoFunction;      ///< this function will get called when the command is executed
  V_COMMAND_FUNCPTR IsRelevant;     ///< this function has to return whether the command is currently available
  
  const char *m_szGroup;          ///< group of the command (e.g. "Editing"). Currently not used, only for information purposes.
  const char *m_szCommand;        ///< name of the command (e.g. "Mirror horizontally")
  const char *m_szDescription;    ///< description of the command (e.g. "Mirror all currently selected brushes horizontally").
  const char *m_szBitmap;         ///< bitmap of the command (currently not used)

  VCommandType *m_pNextCommandType ;
  VModule *m_pPlugin;
};

#endif //_VISION_DOC

#endif // VISION_VCOMMAND_HPP

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
