/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiPluginManager.hpp

#ifndef VIS_API_PLUGIN_MANAGER_HPP
#define VIS_API_PLUGIN_MANAGER_HPP

////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper macros
////////////////////////////////////////////////////////////////////////////////////////////////////

//Dynamically loads and initializes the plugin from a DLL
// e.g. VISION_PLUGIN_LOAD_DYNAMIC(VisionEnginePlugin);
#define VISION_PLUGIN_LOAD_DYNAMIC(_plugin)                             \
    {if (!Vision::Plugins.LoadEnginePlugin(#_plugin))                    \
      Vision::Error.FatalError("Could not load "#_plugin".vPlugin(D)");}

//Initializes the plugin directly if it is statically linked
//#include <EnginePlugins/EnginePluginsImport.hpp> for plugins
//e.g. VISION_PLUGIN_LOAD_STATIC(VisionEnginePlugin);
#define VISION_PLUGIN_LOAD_STATIC(_plugin) {GetEnginePlugin_##_plugin ()->InitEnginePlugin();}


#if defined(_DLL) && !defined(VISIONDLL_LIB) && !defined(VBASE_LIB)  ///<We're on a platform that supports DLLs (and we want to use them)

  //Loads & inits the plugin dynamically or statically depending on the target platform
  //#include <EnginePlugins/EnginePluginsImport.hpp> for plugins
  // e.g. VISION_PLUGIN_ENSURE_LOADED(VisionEnginePlugin);
  #define VISION_PLUGIN_ENSURE_LOADED(_plugin)  VISION_PLUGIN_LOAD_DYNAMIC(_plugin)

#else   ///<We're on a platform that uses static libraries

  //Loads & inits the plugin dynamically or statically depending on the target platform
  //#include <EnginePlugins/EnginePluginsImport.hpp> for plugins
  // e.g. VISION_PLUGIN_ENSURE_LOADED(VisionEnginePlugin);
  #define VISION_PLUGIN_ENSURE_LOADED(_plugin) VISION_PLUGIN_LOAD_STATIC(_plugin)

#endif


//////////////////////////////////////////////////////////////////////////////////////////////////
// Macros for importing and exporting functions from a DLL
//////////////////////////////////////////////////////////////////////////////////////////////////

#if (defined WIN32) 
	#if !defined(VISIONDLL_LIB)
		#define VEXPORT extern "C" __declspec(dllexport)
		#define VIMPORT extern "C" __declspec(dllimport)
	#else
		#define VEXPORT extern "C" 
		#define VIMPORT extern "C" 
	#endif

#elif defined(_VISION_XENON)
#define VEXPORT extern "C" __declspec()
#define VIMPORT extern "C" __declspec()



#elif defined(_VISION_PS3)
#define VEXPORT extern "C" 
#define VIMPORT extern "C" 



#elif defined(_VISION_IOS) || defined(_VISION_ANDROID) || defined(_VISION_PSP2) || defined(_VISION_WIIU)
#define VEXPORT extern "C" 
#define VIMPORT extern "C" 

#else
  #error Undefined platform!
#endif // _DLL or _WINDLL


////////////////////////////////////////////////////////////////////////////////////////////////////
// Classes
////////////////////////////////////////////////////////////////////////////////////////////////////


class VisPluginDLL_cl;

/// \brief
///   Helper class to take care of plugin initialization and deinitialization.
/// 
/// This class can be used for statically and dynamically linked plugins, and makes sure a plugin
/// is only initialized or deinitialized once.
/// 
/// Please refer to the VisionEnginePlugin project for an example of how this class is used.
class IVisPlugin_cl
{
public:
  /// \brief
  ///   Enum that determines how the plugin reference should be embedded into a .vscene file export. 
  ///   See function IVisPlugin_cl::GetExportBehavior().
  enum VExportReferenceBehavior
  {
    VEXPORTREFERENCE_NEVER = 0,     ///< This plugin reference is never embedded into the vscene
    VEXPORTREFERENCE_WHENUSED = 1,  ///< This plugin reference is embedded if any shape or component is serialized from this plugin
    VEXPORTREFERENCE_ALWAYS = 2,    ///< This plugin reference is embedded even if no shape uses it. This is useful if the plugin provides application infrastructure, but not shapes
    VEXPORTREFERENCE_USER_SELECT = 3  ///< The user decides per vForge UI whether the plugin should be embedded or not (Export dialog)
  };

  IVisPlugin_cl()
  {
    m_iInitCount = 0;
    m_bInitialized = false;
    m_bUserSelected = true;
    m_pOwnerDLL = NULL;
  }
  
  virtual ~IVisPlugin_cl()
  {
    VASSERT(m_iInitCount==0); //Make sure you call  Vision::Shutdown(); at the end of your application
  }

  ///
  /// @name Overridables
  /// @{
  ///

  /// \brief
  ///   This overridable gets called by the engine to initialize the plugin.
  /// 
  /// This function is called exactly once since multiple init calls are transparently handled by
  /// the engine using the m_iInitCount member. It is called both for statically linked and
  /// dynamically loaded plugins.
  /// 
  /// Inside a custom plugin, this function is a good place to globally initialize custom plugin
  /// data, register modules and resource managers, etc.
  virtual void OnInitEnginePlugin() = 0;

  /// \brief
  ///   This overridable is the de-init counterpart for OnInitEnginePlugin. It gets called exactly
  ///   once before the plugin is unloaded.
  virtual void OnDeInitEnginePlugin() = 0;

  /// \brief
  ///   This function must return the actual target binary name without file extension. For
  ///   instance "VisionEnginePlugin".
  virtual const char *GetPluginName() = 0;

  /// \brief
  ///   Determines whether the plugin reference should be embedded into a .vscene file export.
  ///
  /// This function is called for every registered engine plugin at scene export time. The return value of 
  /// this function determines whether the plugin name should be put into the plugin data chunk of the 
  /// .vscene file.
  ///
  /// If the plugin is referenced there, the VSceneLoader class tries to load the plugin
  /// before loading the scene. The default return value of this function is VEXPORTREFERENCE_WHENUSED,
  /// which means the plugin will be referenced if used by any shape or component in the .vscene.
  ///
  /// \return
  ///   the export behavior for this plugin; see VExportReferenceBehavior for possible values.
  VISION_APIFUNC virtual VExportReferenceBehavior GetExportBehavior()
  {
    return VEXPORTREFERENCE_WHENUSED;
  }

  ///
  /// @}
  ///

  ///
  /// @name Internal Functions
  /// @{
  ///


  /// \brief
  ///   Called internally to initialize the plugin. Handles multiple initialization 
  ///   attempts gracefully.
  VISION_APIFUNC void InitEnginePlugin();
  
  /// \brief
  ///   Called internally to de-initialize the plugin. Handles multiple deinitialization
  ///   attempts gracefully.
  VISION_APIFUNC void DeInitEnginePlugin();

  /// \brief
  ///   Forces the plugin to deinitialize; ignores the initialization counter.
  VISION_APIFUNC void ForceDeinit();
    
  int m_iInitCount;
  bool m_bInitialized;
  bool m_bUserSelected;
  VisPluginDLL_cl* m_pOwnerDLL; ///< The DLL this plugin came from. \c NULL in case of statically linked plugin libraries.

  ///
  /// @}
  ///

};



/// \brief
///   Class that keeps information about a dynamically loaded plugin DLL
class VisPluginDLL_cl
{
public:

  VisPluginDLL_cl(const char* pszFileName, HMODULE hMod)
  {
    m_sFileName = pszFileName;
    m_pPlugin = NULL;
    
#ifdef WIN32  
    m_hMod = hMod;
#endif    
  }

  VStaticString<FS_MAX_PATH> m_sFileName; ///< the filename of the plugin
  IVisPlugin_cl * m_pPlugin;  ///< plugin init/deinit functions
  
#ifdef WIN32  
  HMODULE m_hMod;
#endif  

};


/// \brief
///   Class for handling engine and entity plugins. A global instance of this class can be
///   accessed via Vision::Plugins
/// 
/// The engine can load .vPlugin (release) and .vPluginD (debug) DLL files.
/// 
/// These files typically contain entity classes and can optionally export InitEnginePlugin and
/// DeinitEnginePlugin functions.
/// 
/// These functions are called by the engine to allow plugins to do things like register a  new
/// application, renderloop, shader callbacks, etc.
/// 
/// vForge loads the plugins (.vplugin) in the project directory and initializes each one via
/// InitEnginePlugin/DeinitEnginePlugin.
class VisPluginManager_cl
{
public:

  VisPluginManager_cl();
  
  ~VisPluginManager_cl();

  ///
  /// @name Plugin Directories
  /// @{
  ///

  /// \brief
  ///   Adds a directory to the list of engine plugin directories.
  /// 
  /// This function adds a directory to the list of directories that will be searched by
  /// LoadEnginePlugin.
  /// 
  /// The plugin loading operations will bypass the Vision file manager, since the OS functions for
  /// loading DLLs are used. This also means that plugins cannot be loaded from archive or package 
  /// files.
  /// 
  /// \param pszPluginDir
  ///   plugin directory path. It can be relative to the current directory (e.g. "plugins") or
  ///   absolute (e.g. "C:\\my game\\"); If it is \c NULL, the current directory will be added.
  /// 
  /// \return
  ///   the index of the added directory in the list of plugin directories.
  VISION_APIFUNC int AddPluginDirectory(const char *pszPluginDir = NULL);


  /// \brief
  ///   Gets the engine plugin directory with the given index.
  /// 
  /// This function returns the engine plugin directory with the given index.
  /// 
  /// The index is returned by the AddPluginDirectory function.
  /// 
  /// \param index
  ///   index of the plugin directory entry to return
  /// 
  /// \return
  ///   the plugin directory path, or \c NULL if the path is not set or \c index
  ///   is out of range
  VISION_APIFUNC const char *GetPluginDirectory(int index);
  
  ///
  /// @}
  ///

  ///
  /// @name Engine Plugins
  /// @{
  ///
  
  /// \brief
  ///   Loads an engine plugin file.
  /// 
  /// \param pszPlugin
  ///   Absolute path to the plugin file OR Short name of the plugin  (e.g. "MyPlugin" if there is
  ///   a file MyPlugin.vplugin/vpluginD in the current directory or one of  the plugin directories). 
  ///   The debug file extension will be appended in the Debug build of the engine.
  /// 
  /// \return
  ///   \c true if the plugin could be loaded, \c false otherwise.
  /// 
  /// \note
  ///   Should be called before initializing the engine, so the loaded plugin can listen to the engine
  ///   initialization event and perform its own initialization tasks.
  VISION_APIFUNC bool LoadEnginePlugin(const char *pszPlugin);
  
  /// \brief
  ///   Loads all engine plugin files in a directory.
  /// 
  /// \param pszDirAbsPath
  ///   Absolute path to plugin directory. \c NULL means the current working directory will be used.
  /// 
  /// \return
  ///   \c true if operation was successful (even if no plugin has been loaded); \c false if an error
  ///   occurred.
  /// 
  /// \note
  ///   Should be called before initializing the engine, so the loaded plugins can listen to the engine
  ///   initialization event and perform their own initialization tasks.
  VISION_APIFUNC bool LoadAllEnginePlugins(const char *pszDirAbsPath = NULL);
  
  /// \brief
  ///   Unloads a single engine plugin.
  /// 
  /// \param pszFileAbsPath
  ///   Absolute path name of the plugin.
  /// 
  /// \return
  ///   \c true if the plugin could be unloaded, \c false if not.
  VISION_APIFUNC bool UnloadEnginePlugin(const char *pszFileAbsPath);
  
  /// \brief
  ///   Unloads all engine plugins.
  /// 
  ///  Should be called after de-initializing the engine
  VISION_APIFUNC void UnloadAllEnginePlugins();
  

  /// \brief
  ///   Returns the number of loaded engine plugins
  /// 
  /// \sa VisPluginManager_cl::GetEnginePluginInfo
  VISION_APIFUNC int GetEnginePluginCount() const;

  /// \brief
  ///   Returns the plugin information structure of a loaded plugin.
  ///
  /// \param iIndex
  ///   Index of the plugin whose information to return.
  /// 
  /// \return
  ///   pointer to the information of the requested plugin
  VISION_APIFUNC const VisPluginDLL_cl* GetEnginePluginInfo(int iIndex) const;

  /// \brief
  ///   Tests whether a plugin with specified filename is currently loaded
  /// 
  /// \param szFilename
  ///   filename of the plugin to test
  /// 
  /// \return
  ///   \c true if the plugin is loaded, \c false if not
  VISION_APIFUNC bool IsEnginePluginLoaded(const char *szFilename) const;

  ///
  /// @}
  ///

  ///
  /// @name Entity Classes
  /// @{
  ///

  /// \brief
  ///   Deinitializes all dynamically loaded and statically linked plugins.
  /// 
  /// This function ignores the initialization count and forces each plugin to deinitialize.
  /// 
  /// Should only be used after the engine has been deinitialized.
  /// 
  /// Call this after UnloadAllEnginePlugins to deinitialize all the remaining  static plugins so you
  /// don't have to manually deinitialize each one.
  VISION_APIFUNC void DeInitRegisteredPlugins();

  ///
  /// @}
  ///

  ///
  /// @name For Internal Use
  /// @{
  ///

  //Registers a plugin after it has been initialized
  VISION_APIFUNC void RegisterPlugin(IVisPlugin_cl* pPlugin);

  //Deregisters a plugin after it has been deinitialized
  VISION_APIFUNC void DeregisterPlugin(IVisPlugin_cl* pPlugin);
  
  //Searches for a plugin by name (e.g. "VisionEnginePlugin")
  VISION_APIFUNC IVisPlugin_cl* GetRegisteredPlugin(const char *pszName);

  //Returns the number of registered plugins (static+dynamic)
  VISION_APIFUNC int GetRegisteredPluginCount();
  
  //Returns a specific registered plugin
  VISION_APIFUNC IVisPlugin_cl* GetRegisteredPlugin(int iIndex);
  
protected:  
  DynArray_cl<char *>   m_pluginDirs;
  VPListT<IVisPlugin_cl> m_registeredPlugins;
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
