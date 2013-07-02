/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VSceneLoader.hpp

#ifndef VSCENELOADER_HPP_INCLUDED
#define VSCENELOADER_HPP_INCLUDED

#if defined (WIN32) || defined(_VISION_XENON)

  #ifdef SCENEMODULE_EXPORTS
    #define SCENE_IMPEXP __declspec(dllexport)
  #elif defined SCENEMODULE_IMPORTS
    #define SCENE_IMPEXP __declspec(dllimport)
  #else
    #define SCENE_IMPEXP
  #endif


#elif defined (_VISION_PS3)
  #define SCENE_IMPEXP 



#elif defined(_VISION_PSP2)
  #define SCENE_IMPEXP 
#else
  #define SCENE_IMPEXP

#endif


#include <Vision/Runtime/Base/System/IO/Serialization/VChunkFile.hpp>


#define SCENE_MAGIG_NUMBER            220878

// SDK version 5.0
#define SCENE_FILE_VERSION1           1

// SDK version 5.5
#define SCENE_FILE_VERSION2           2

// SDK version 6.0: Previous versions are not supported
#define SCENE_FILE_VERSION3           3

// SDK version 6.2: Default lighting color
#define SCENE_FILE_VERSION4           4

// SDK version 6.4: Scene script object
#define SCENE_FILE_VERSION5           5

// SDK version 7.0: Visibility integration
#define SCENE_FILE_VERSION6           6
#define SCENE_FILE_VERSION7           7

// SDK version 7.5: Better plugin handling, percentage progress
#define SCENE_FILE_VERSION8           8
#define SCENE_FILE_VERSION9           9

// SDK version 7.5: Better plugin handling, percentage progress
#define SCENE_FILE_VERSION10          10

// SDK version 8.1.4 : Export flags
#define SCENE_FILE_VERSION11          11

// SDK version 8.2: Platform profiles
#define SCENE_FILE_VERSION12          12

// SDK version 8.3: Object sizes & bounds checks
#define SCENE_FILE_VERSION13          13

// SDK version 2012.2.1: Script Interval
#define SCENE_FILE_VERSION14          14

// SDK version 2012.2.2: Global LOD Hysteresis Thresholding
#define SCENE_FILE_VERSION15          15

// SDK version 2013.1: Moved global unit scaling from VIEW to _V3D chunk
#define SCENE_FILE_VERSION16          16

// IMPORTANT! Do not forget to enhance the SceneVersion test in the 
// engine tests if you increase the scene version number
#define SCENE_FILE_CURRENT_VERSION    SCENE_FILE_VERSION16


// local fog- chunk versioning
#define VSCENE_FOG_VERSION_0          0
#define VSCENE_FOG_VERSION_1          1
#define VSCENE_FOG_VERSION_2          2 // added possibility to toggle sky-masking for fog rendering
#define VSCENE_FOG_CURRENT_VERSION    VSCENE_FOG_VERSION_2


/// \brief
///   Class that implements basic vscene loading
///
/// Exported vscenes can be loaded through this class. The usage is straight forward and shown in the engine samples.
class VSceneLoader : public VChunkFile_cl
{
public:
  SCENE_IMPEXP VSceneLoader();
  SCENE_IMPEXP virtual ~VSceneLoader();
  
  /// \brief
  ///   Flags for different loading behavior
  enum LoadingFlags
  {
    LF_UseStreamingIfExists   = V_BIT(0), ///< If there is a corresponding .vres file to the scene, the scene loading is done via streaming. You have to call Tick periodically to advance the streaming.
    LF_ForceMobileMode        = V_BIT(1), ///< Forces the loader to use a VisionMobileShaderProvider and a simple Lightgrid, regardless of what is set in the vscene file.
    LF_UsePrewarming          = V_BIT(2), ///< Prewarms all resources by rendering every mesh once with the assigned shader which which forces the graphics driver to create its internal objects. This prevents stuttering after scene loading.
    LF_UseInterleavedLoading  = V_BIT(3) | LF_UseStreamingIfExists, ///< Does not load the whole scene at once but only a certain amount of chunks per frame. You have to call IsFinished periodically to advance the scene file loading. Also enables LF_UseStreamingIfExists.
    
    LF_PlatformDefault = 
#ifdef NEEDS_SCENE_STREAMING
    LF_UseStreamingIfExists |
#endif
#ifdef NEEDS_SCENE_PREWARMING
    LF_UsePrewarming |
#endif
    0
  };

  ///
  /// @name Load / unload a vscene
  /// @{
  ///

  /// \brief
  ///   Loads an exported vscene file and creates all shapes in it.
  ///
  /// \param szFilename
  ///   Filename of the exported vscene file.
  ///
  /// \param iLoadingFlags
  ///   A combination of LoadingFlags.
  ///
  /// \returns
  ///   true if loading was successful. If not, the error message can be retrieved via GetLastError() which is inherited from the VChunkFile_cl class.
  ///
  /// This is the key function of this class. It parses the vscene file and creates content including shape instance (entities, static meshes), sky, 
  /// scene scripts etc. It also creates the empty zone instances if streaming is used.
  /// 
  /// If you want to perform the scene loading as one part of a more complex loading operation then you can use PushRange
  /// on the ProgressStatus instance to specifiy a subrange percentage for the scene loading. In that case the LoadScene method will not
  /// call OnStart and OnFinish on the ProgressStatus instance, and you have to call these methods in your code.
	///
	/// \see
	///   UnloadScene
  ///   IsFinished
	SCENE_IMPEXP bool LoadScene(const char *szFilename, int iLoadingFlags = LF_PlatformDefault);

  /// \brief
	///   Static helper function to unload the scene content. Call this only when you
  ///   are finished with any rendering (e.g. at application shutdown)
	///
	static inline void  UnloadScene()   
  { 

    { // Trigger callback OnBeforeSceneUnloaded
      IVisCallbackDataObject_cl data(&Vision::Callbacks.OnBeforeSceneUnloaded);
      Vision::Callbacks.OnBeforeSceneUnloaded.TriggerCallbacks(&data);
    }

    Vision::DeInitWorld();

    { // Trigger callback OnAfterSceneUnloaded
      IVisCallbackDataObject_cl data(&Vision::Callbacks.OnAfterSceneUnloaded);
      Vision::Callbacks.OnAfterSceneUnloaded.TriggerCallbacks(&data);
    }
  }

  /// \brief
  ///   Static helper function to clear the scene content, and prepare empty scene
  ///   for use.	
  static inline void  ClearScene()   
  { 
    UnloadScene();

    Vision::InitWorld();
  }

  /// \brief
  ///   Advances the loading process.
  ///
  /// If streaming is used this method advances the streaming and returns true if the scene loading is completed. 
  /// If streaming is not used this method always returns true.
  SCENE_IMPEXP bool Tick();

  /// \brief
  ///   returns true if the last scene loading operation has finished.
  inline bool IsFinished() const
  { 
    return m_bIsFinished; 
  }
  
  /// \brief
  ///   prewarms all resources. 
  ///
  /// Prewarms all resources. by rendering every mesh once with the assigned shader which which forces the graphics driver to create its internal 
  /// objects. This prevents stuttering after scene loading. If streaming is used this method will only render a few objects at a time and return
  /// true if prewarming is done. If streaming is not used this method will always prewarm all resources and return true.
  SCENE_IMPEXP bool PrewarmResources();

#if defined(_DLL) && !defined(VISIONDLL_LIB)

  /// \brief
	///   Helper function that parses only the plugins chunk from a scene file and loads the required plugins
	///
	/// \param szFilename
	///   Filename of the exported vscene file
	///
	/// \param pManager
	///   Optional file manager
	///
	/// \returns
	///   true if all plugins could be loaded successfully
	///
  /// Helper function that parses only the plugins chunk from a scene file and loads the required plugins.
  /// This allows for loading the plugins before the engine initializes and the real scene loading starts.
  /// Used by the vSceneViewer when loading a scene.
  /// For the final application you should explicitly load all the plugins you need before you initialize your application
  /// (e.g. Vision::Plugins.LoadEnginePlugin).
	///
	/// \see
	///   LoadScene
	SCENE_IMPEXP bool                LoadEnginePlugins(const char *szFilename, IVFileStreamManager *pManager=NULL);
#endif

  SCENE_IMPEXP bool                ReadFogChunk();                                 ///< Loads a chunk that contains depth fog information.
  SCENE_IMPEXP bool                ReadViewChunk();                                ///< Loads a chunk that contains some view information, e.g. the clip planes.
  SCENE_IMPEXP bool                ReadSceneChunk();                               ///< Loads scene related data. Always the first chunk.
  SCENE_IMPEXP bool                ReadV3DChunk();                                 ///< Loads a chunk that contains the V3D related data.
  SCENE_IMPEXP bool                ReadShapeChunk();                               ///< Loads a full chunk that contains a serialisation archive.
  SCENE_IMPEXP bool                ReadReferencedPluginsChunk(bool bLoadPlugins);  ///< Loads the list of (probably) required engine plugins.
  SCENE_IMPEXP bool                ReadEmbeddedFileChunk();                        ///< Loads a chunk with custom embedded files.
  SCENE_IMPEXP bool                ReadZoneChunk();                                ///< Loads a chunk file with zone references.

  /// \brief
	///   Static helper function that returns the filename of the last loaded vscene
  static inline const char *GetLastLoadedSceneFileName() { return s_sLastLoadedScene; }

#ifdef WIN32
  // saving:
  SCENE_IMPEXP bool EmbedFile(const char *szFilename, IVFileStreamManager* pManager, CHUNKIDTYPE iID);
#endif

  ///
  /// @}
  ///

  ///
  /// @name Overridables
  /// @{
  ///

  /// \brief
	///   This overridable is called when the actual objects in the vscene are about to be de-serialized.
	///
	/// \param ar
	///   Reference to the archive that contains the shapes.
  ///   This instance holds useful information such as number of objects in this archive (VShapesArchive::m_iObjectCount member).
  ///   This info is available when this function is called
	///
	SCENE_IMPEXP virtual void OnStartShapeDeSerialization(VShapesArchive &ar)
  {
  }

  /// \brief
	///   This overridable is called for every object that is de-serialized.
	///
	/// \param ar
	///   The binary archive.
	///
	/// \param pObject
	///   Pointer to the object that has been read. This can be NULL.
	///
	/// \remarks
	///   This function is called exactly VShapesArchive::m_iObjectCount times. The passed pObject object is non-NULL exactly
  ///   VShapesArchive::m_iNonNullCount times. Root objects (i.e. objects that are de-serialized in the main loop) can be filtered
  ///   with VShapesArchive::m_iRecursionDepth==0.
	///
	SCENE_IMPEXP virtual void OnDeSerializeObject(VShapesArchive &ar, VTypedObject *pObject)
  {
  }

  /// \brief
	///   This overridable is called when de-serialization of objects has been completed.
	///
	/// \param ar
	///   Reference to the archive.
  SCENE_IMPEXP virtual void OnFinishShapeDeSerialization(VShapesArchive &ar)
  {
  }

  /// \brief
	///   This overridable is called when loading is aborted. The default implementation calls UnloadScene and sets the m_bAborted flag
  SCENE_IMPEXP virtual void OnAbort()
  {
    m_bAborted = true;
    UnloadScene();
  }

  /// \brief
	///   Returns whether scene laoding has been aborted by the user while loading.
  inline bool IsAborted() const
  {
    return m_bAborted;
  }

  ///
  /// @}
  ///


public:
  SCENE_IMPEXP virtual BOOL OnStartChunk(CHUNKIDTYPE chunkID, int iChunkLen);
  SCENE_IMPEXP virtual void OnError(const char *szError, CHUNKIDTYPE chunkID, int iChunkOfs);
  SCENE_IMPEXP virtual void OnWarning(const char *szWarning, CHUNKIDTYPE chunkID, int iChunkOfs);

  hkvVec3 m_vDefaultCamPos;  ///< This member can be used to retrieve the camera position that was set in vForge at export time
  hkvMat3 m_DefaultCamRot; ///< This member can be used to retrieve the camera rotation that was set in vForge at export time
  float m_fUnitScaling; ///< global unit scaling of the vForge instance that exported this scene (this is not a scene property)

  /// \brief
  ///   Static callback that is triggered when a custom (unknown) data chunk is parsed while vscene loading
	///
	/// Applications can register to this callback to hook into vscene loading. The data object can be casted to 
  /// class VCustomSceneChunkDataObject which provides the chunk file and the unknown chunk ID
	SCENE_IMPEXP static VisCallback_cl OnCustomChunkSerialization;

  /// \brief
  ///   Static callback, that is triggered while vscene loading directly before the 'SHPS' chunk. Thus custom data
  ///   chunks can be parsed before the shape data chunk gets parsed.  
  ///
  /// Applications can register to this callback to hook into vscene loading. The data object can be casted to 
  /// class VCustomSceneChunkDataObject which provides the chunk file and the unknown chunk ID
  SCENE_IMPEXP static VisCallback_cl OnCustomChunkBeforeShapesSerialization;

private:
  void FinalizeSceneLoading();
  inline bool PrewarmingStarted() { return m_iNextPrewarmIndexStaticGeometry > 0 || m_iNextPrewarmIndexEntities > 0; }

#ifdef _VR_GLES2
  // extended prewarming to minimize framerate hitches when using lazy shader compilation on OpenGL ES 2.0
  void GeneratePrewarmLights();
  void DestroyPrewarmLights();
  void GenerateEntitiesFromMeshes(VisEntityCollection_cl &entities, int iStartIndex, int iEndIndex);
  void DestroyEntities(VisEntityCollection_cl &entities);
#endif

  void PrewarmStaticInstances(VisStaticGeometryInstanceCollection_cl& staticInstances);
  void PrewarmEntity(VisBaseEntity_cl *pEntity);

private:
  // Members
  bool m_bLoadPlugins, m_bAborted, m_bIsFinished;
  int m_iSceneVersion;
  int m_eExportFlags; // VSceneExportFlags_e
  VStaticString<FS_MAX_PATH> m_sSceneFilename;
  SCENE_IMPEXP static VString s_sLastLoadedScene;
  bool m_bStreaming;
  bool m_bExternalProgress;
  VResourceSnapshot m_vsceneResources;
  VisionResourceCreator_cl m_resourceCreator;
  CHUNKIDTYPE m_currentChunkID;

  float m_fNearClipPlane;
  float m_fFarClipPlane;
  float m_fFovX;

  bool m_bForceMobileMode;
  bool m_bUsePrewarming;
  bool m_bInterleavedLoading;
  int m_iNextPrewarmIndexStaticGeometry;
  int m_iNextPrewarmIndexEntities;

#ifdef _VR_GLES2
  static const int PREWARM_LIGHT_SOURCE_COUNT = 4;

  int m_iNextPrewarmIndexMeshes;
  int m_iPrewarmLightSourceCount;
  VisLightSource_cl *m_aLightSources[ PREWARM_LIGHT_SOURCE_COUNT ];
#endif
};


/// \brief
///   Callback data object class that is triggered by the static VSceneLoader::OnCustomChunkSerialization 
///   and VSceneLoader::OnCustomChunkBeforeShapesSerialization callback.
class VCustomSceneChunkDataObject : public IVisCallbackDataObject_cl
{
public:
  inline VCustomSceneChunkDataObject(VSceneLoader &loader, CHUNKIDTYPE chunkID, VisCallback_cl *pCustomChunkCallback)
    : IVisCallbackDataObject_cl(pCustomChunkCallback),
      m_Loader(loader)
  {
    m_iChunkID = chunkID;
    m_bSuccess = true;
    m_bProcessed = false;
  }

  VSceneLoader &m_Loader; ///< The scene loader that triggered the callback. This is the chunk file with current chunk open
  CHUNKIDTYPE m_iChunkID; ///< The custom chunk ID that is not a default vscene chunk
  bool m_bSuccess;    ///< Set this value to false to indicate loading failed
  bool m_bProcessed;  ///< Only process the chunk if this member is false, otherwise it might have been processed by another listener
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
