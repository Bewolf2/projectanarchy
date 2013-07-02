/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VModule.hpp
/// \brief
///   This file declares the VModule struct and related support constructs.
/// 
/// Each module in the context of Vision must define one instance of VModule, such
/// as in the following example:
/// 
/// \code
/// DECLARE_THIS_MODULE(g_thisModule, 0x100, "TestPlugin", "Your Company", "Module description", NULL)
/// \endcode
/// 
/// If your module defines a Vision Engine plugin, you should also have a class that derives
/// from IVisPlugin_cl, and a global instance thereof. In this case, pass a pointer to the 
/// global plugin instance instead of \c NULL as the last macro parameter.
/// 
/// The first parameter of the DECLARE_THIS_MODULE macro is the variable name by which you can access
/// the instance of VModule that describes your module - in the above case, it's \c g_thisModule.
/// 
/// For all classes using Vision's RTTI system, the module is a required parameter
/// when defining the implementation of a dynamic class:
/// 
/// \code
/// V_DECLARE_DYNCREATE(MyClass);
/// V_IMPLEMENT_DYNAMIC(MyClass, MyParentClass, g_thisModule);
/// \endcode
/// 
/// This allows Vision's RTTI type registry to determine which classes
/// belong to which module.

#ifndef _VISION_VMODULE_HPP
#define _VISION_VMODULE_HPP


struct VType;
struct VActionType;
struct VCommandType;
class VBroadcaster;
class IVisPlugin_cl;


#define VPLUG_EXPORT extern "C" __declspec(dllexport)


#define V_SDK_VERSION_MAJOR 0x01
#define V_SDK_VERSION_MINOR 0x00
#define V_SDK_VERSION  MAKE_VERSION(V_SDK_VERSION_MAJOR, V_SDK_VERSION_MINOR)

class VModuleCleanup;

/// \brief
///   this macro creates a custom VModule instance.
/// \details
/// The macro parameters have the following meaning:
/// - \b variable: the variable name by which the VModule instance for this module 
///   will be accessible.
/// - \b moduleVersion: the version of the module. A 16 bit value with the major 
///   version in the upper 8 bits and the minor version in the lower 8 bits.
/// - \b moduleName: the name of the module. Ideally, this should be the same as the
///   base name of the compiled static or dynamic library.
/// - \b moduleVendor: the name of the company or other entity that created the
///   module.
/// - \b description: a short description what this module contains or provides.
/// - \b sourceplugin: if available, a pointer to the IVisPlugin_cl instance of the
///   engine plugin defined by this module. May be \c NULL.
#define DECLARE_THIS_MODULE(variable, moduleVersion, moduleName, moduleVendor, description, sourceplugin) \
VModule variable = {V_SDK_VERSION, moduleVersion, moduleName, moduleVendor, description,\
                    NULL/*modulename*/, 0/*module handle*/, \
                    NULL/*TypeLinkedList*/, NULL/*ActionLinkedList*/, NULL/*CommandLinkedList*/, \
                    NULL/*LogBroadcaster*/, NULL/*ParamBroadcaster*/, sourceplugin };\
VModuleCleanup variable##cleanup(&variable);

#ifdef WIN32
  #define MAX_VMODULE_PLUGINNAME_CHARS  63
#else
  // all other platforms do not support this
  #define MAX_VMODULE_PLUGINNAME_CHARS  0
#endif

/// \brief
///   Information structure about a module
/// 
/// A plugin must have a static global instance of a VModule structure. The constructor of this
/// class is used to setup basic data  like the version and the vendor of the plugin. This
/// structure  contains a SDK version number which is used for backward compatibility  in further
/// versions.
struct VBASE_IMPEXP_CLASS VModule
{ 
public:
  // Can't add constructor or destructor !!. This internally turns it into a class
  // for the MSVC compiler and the initialization isn't done at compile time anymore!
  // Thats why we have the VModuleCleanup class.
  // ~VModule();
 
  // data about each plugin (initialized with the macro DECLARE_THIS_PLUGIN)
  unsigned short m_uiSdkVersion;     ///< version of sdk
  unsigned short m_uiModuleVersion;  ///< version of this module
  const char *m_szModuleName;        ///< name of the module
  const char *m_szModuleVendor;      ///< vendor of the module
  const char *m_szModuleDescription; ///< description of the module


  // filled by module loader
  const char *m_szOSModuleName;      ///< Win32: name of the windows dll
  unsigned int m_hOSModuleHandle;    ///< Win32: handle of the windows dll

  // VModule data
  VType *m_pTypeLinkedList;               ///< RTTI linked list for every module
  VActionType *m_pActionTypeLinkedList;   ///< action type list for every module
  VCommandType *m_pCommandTypeLinkedList; ///< command type list for every module

  VBroadcaster *m_pLogBroadcaster;       ///< broadcaster for log messages
  VBroadcaster *m_pParameterBroadcaster; ///< broadcaster for parameter messages
  
  IVisPlugin_cl *m_pSourcePlugin; ///< the engine plugin, if any, that is defined in this module
public:

  const char* GetName()
  {
    return m_szModuleName;
  }

  /// \brief
  ///   Dumps out some information about the plugin with VTRACE. This function works only in Debug
  ///   configuration!
  void DumpPlugin() const;
  
  /// \brief
  ///   Register function for a new class type.
  void RegisterKlass(VType *pType);
  
  /// \brief
  ///   Un-Register function for when a class type is removed from a module (e.g. to be moved to another plugin module).
  void UnRegisterKlass(VType *pType);
  
  /// \brief
  ///   Register function for a new action type.
  void RegisterScriptAction(VActionType *pActionType);
  
  /// \brief
  ///   Register function for a new command type.
  void RegisterCommandType(VCommandType *pCommandType);

  /// \brief
  ///   Returns the RTTI type linked list.
  VType *GetTypeList() const;
  
  
  /// \brief
  ///   Returns the action type linked list.
  VActionType *GetActionTypeList() const;
  

  /// \brief
  ///   Returns the command type linked list.
  VCommandType *GetCommandTypeList() const;


  /// \brief
  ///   Possible values for the VSenderRecevier::OnReceive iMsg variable value
  enum eLOGBROADCAST_IDS
  {
    LOG_MSG_TEXT,       ///< message is a text
    LOG_MSG_WARNING     ///< message is a warning
  };

  /// \brief
  ///   Returns the log broadcaster.
  VBroadcaster *GetLogBroadcaster();

  /// \brief
  ///   Returns the param broadcaster.
  VBroadcaster *GetParamBroadcaster();

  /// \brief
  ///   printf-like log functions which broadcasts a message.
  void Print(const char *text, ...);

  /// \brief
  ///   printf-like log functions which broadcasts a warning.
  void PrintWarning(const char *text, ...);

  /// \brief
  ///   Return TRUE if there are no instances of any of the modules referenced vtypes.
  bool CanBeRemoved();
};


#ifndef _VISION_DOC
class VBASE_IMPEXP_CLASS VModuleCleanup
{ 
  VModuleCleanup(){}

public:
  VModuleCleanup(VModule *pModule) {m_pModule = pModule;}
  ~VModuleCleanup();

  VModule *m_pModule;
};
#endif // _VISION_DOC


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
