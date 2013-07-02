/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VModuleInfoList.hpp


#ifndef _VISION_VMODULEINFOLIST_HPP
#define _VISION_VMODULEINFOLIST_HPP

#ifdef WIN32


// forward declarations
class VModuleInfo;
class ModuleList;
class ProcessIdToNameMap;
class ModuleInstance;
typedef ModuleInstance * PModuleInstance;

template<class TYPE, class ARG_TYPE> class VArray;


/// \brief
///   Manages the list of modules that are used by the current process.
/// 
/// Before using any of the other functions in this class, first call Fill() once to gather
/// the information about the loaded modules.
class VBASE_IMPEXP_CLASS VModuleInfoList
{
public:

  VModuleInfoList();
  ~VModuleInfoList();

  /// \brief
  ///   Clears the list
  void Reset();
  
  /// \brief
  ///   Fills the list with information about the currently loaded modules.
  ///
  /// Call this function before any other, so the module list is populated with current information.
  /// This function can be called again at a later time to update (i.e., clear and re-fill)
  /// the module list.
  void Fill();

  /// \brief
  ///   Returns the module list as a string.
  /// 
  /// The string contains information about each module on a separate line (terminated by \c \\n),
  /// and each line contains the module name, the module version, and the modules full path,
  /// separated by commas.
  /// 
  /// \return
  ///   the module information string. Please note that this string is only valid until either
  ///   this VModuleList instance is destroyed, or AsString() is called again.
  const char* AsString() const;

  /// \brief
  ///   Returns the length of the list
  /// \return
  ///   the number of modules in this list.
  int GetLength() const;

  /// \brief
  ///   Returns information about a module.
  /// \param index
  ///   index of the module
  /// \return
  ///   the requested module information, or \c NULL, if \c index is not a valid
  ///   list index.
  const VModuleInfo* GetModule(int index) const;


private:

  ModuleList          *m_pModuleList;           ///< List of all loaded modules
  ProcessIdToNameMap  *m_pProcessIdToNameMap;   ///< List of all process names & IDs
  
  VArray<VModuleInfo*, VModuleInfo*> *m_pModuls; ///< list of modules

  mutable char* m_szModuleListAsChar; ///< static char array to the string returned by AsString

  void AddModule(PModuleInstance pModInst); ///< helper function add one module
  void GetVersion(PModuleInstance pModInst, char* szVersion); ///< helper to get the version from the module
  
};


#endif // WIN32

#endif // _VISION_VMODULELINFOIST_HPP

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
