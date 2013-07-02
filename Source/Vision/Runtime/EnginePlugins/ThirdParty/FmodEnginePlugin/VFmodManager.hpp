/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VFmodManager.hpp

#ifndef VFMODMANAGER_HPP_INCLUDED
#define VFMODMANAGER_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodSoundResource.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodSoundObject.hpp>  
#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodEventGroup.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodEvent.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodCollisionMeshInstance.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodReverb.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodAnimationEventSoundTrigger.hpp>


// Fmod error handling

void FMOD_ErrorCheck(FMOD_RESULT result, bool bFatal, int line, const char *szFile);

#define FMOD_ERRORCHECK(result) FMOD_ErrorCheck(result,true,__LINE__,__FILE__)
#define FMOD_WARNINGCHECK(result) FMOD_ErrorCheck(result,false,__LINE__,__FILE__)


///\brief
///  Structure to configure Fmod plugin   
///    
struct VFmodConfig 
{
  inline VFmodConfig()
  {
#ifdef VFMOD_SUPPORTS_NETWORK
    bUseNetworkSystem = true;
#endif
    fTimeStep = 1.0f/120.0f; 
#ifdef _VISION_PS3
    iMaxChannels = 32;
#else
    iMaxChannels = 128;
#endif
    iSampleRate = 48000;
    iFormat = FMOD_SOUND_FORMAT_PCM16;
    iNumOutputChannels = 2;
    iMaxInputChannels = 6;
    iResampleMethod = FMOD_DSP_RESAMPLER_LINEAR;
    bLoadEventDataAsyncronous = false;
    bUseMemoryPool = false;
    iMemoryPoolSize = 0;

    // the default value in Fmod is FMOD_DEBUG_LEVEL_LOG | FMOD_DEBUG_LEVEL_ERROR | FMOD_DEBUG_LEVEL_WARNING | FMOD_DEBUG_TYPE_EVENT
    iDebugLevel = FMOD_DEBUG_LEVEL_ERROR | FMOD_DEBUG_LEVEL_WARNING | FMOD_DEBUG_TYPE_EVENT;

    fFmodToVisionScale = 100.0f;
  }

#ifdef VFMOD_SUPPORTS_NETWORK
  bool bUseNetworkSystem;               ///< use Fmod NetEventSystem, in order to tweak event properties in your application while they are playing 
#endif
  float fTimeStep;                      ///< Fmod event system update-frequency (e.g. 1/120 second). Please note: significantly higher frequencies 
                                        ///< will lead to synchronization problems when using "Programmers"/ "ProgrammerSelected" events.
  int iMaxChannels;                     ///< maximum number of channels playing at the same time (including virtual channels)
  int iSampleRate;                      ///< sound-card's output rate
  FMOD_SOUND_FORMAT iFormat;            ///< sound-card's output format
  int iNumOutputChannels;               ///< number of output channels/ speakers to initialize the sound-card to
  int iMaxInputChannels;                ///< maximum channel count in loaded/created sounds to be supported
  FMOD_DSP_RESAMPLER iResampleMethod;   ///< Software engine resampling method

  bool bLoadEventDataAsyncronous;       ///< When set to true, this will instruct Fmod to perform all event-related loading in a separate thread, 
                                        ///< so it won't block your main thread when it needs to perform lengthy file operations

  bool bUseMemoryPool;                  ///< when no memory pool is used (default), Fmod uses Vision Memory De-/ Allocators
                                        ///< if a memory pool is provided, Fmod accesses that pool using its own memory management scheme, no extra De-/ Allocations will be done

  int iMemoryPoolSize;                  ///< fixed size of memory pool for Fmod usage, which will be used, when bUseMemoryPool is set true,
                                        ///< the memory pool size (in bytes) must be a multiple of 512

  FMOD_DEBUGLEVEL iDebugLevel;          ///< Bitmask to control debug output level. Only effects debug builds (using logging libs of Fmod).

  float fFmodToVisionScale;             ///< Sets how many engine units corresponds to 1 meter. Only affects doppler shift (see FMOD documentation).
};


/// \brief
///   Callback data object for callback triggered by Fmod plugin before the Fmod system is initialized.
///   Within this callback the Fmod plugin can be configured via VFmodConfig.
///
///\see
///  VFmodConfig
class VFmodConfigCallbackData : public IVisCallbackDataObject_cl
{
public:
  FMOD_IMPEXP VFmodConfigCallbackData(VisCallback_cl *pSender, VFmodConfig &config): m_config(config)
  {
  }

  VFmodConfig &m_config;
};


/// \brief
///   Fmod sound manager class
///
/// This manager manages directly sound-object and event instances (i.e. purge out the instances that have 
/// stopped playing) and sound collision mesh instances. Furthermore it has dedicated resource-managers
/// for sound-resources (VFmodSoundResourceManager) and event-groups (VFmodEventGroupManager).
///
/// The plugin uses one global instance of this Fmod sound manager accessible through the static
/// VFmodManager::GlobalManager() function.
class VFmodManager : public IVisCallbackHandler_cl
{
public:

  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  ///\brief
  ///Constructor
  FMOD_IMPEXP VFmodManager();

  ///\brief
  ///Destructor
  FMOD_IMPEXP virtual ~VFmodManager();

  ///
  /// @}
  ///


  ///
  /// @name Init and Deinit
  /// @{
  ///

  /// \brief
  ///   Accesses the global instance of the Fmod manager
  FMOD_IMPEXP static VFmodManager& GlobalManager();

  /// Global init function, called by plugin
  FMOD_IMPEXP void OneTimeInit();

  /// \brief
  /// Global deinit function, called by plugin
  FMOD_IMPEXP void OneTimeDeInit();

  /// \brief
  ///   Indicates, whether manager is initialized properly
  FMOD_IMPEXP bool IsInitialized() const;

  ///
  /// @}
  ///


  ///
  /// @name Access to Fmod internals
  /// @{
  ///

  /// \brief
  ///   Returns the Fmod event sound system object
  inline FMOD::EventSystem* GetFmodEventSystem() const  { return m_pEventSystem; }

  /// \brief
  ///   Returns the low-level Fmod sound system object
  inline FMOD::System* GetFmodSystem() const  { return m_pSystem; }

  ///
  /// @}
  ///


  ///
  /// @name Listener
  /// @{
  ///

  /// \brief
  ///   Optionally: Attaches a listener (If NULL, the main camera is used)
  inline void SetListenerObject(VisObject3D_cl *pListener) { m_pListenerObject = pListener; }

  /// \brief
  ///   Returns the current listener (If NULL, the main camera is used)
  inline VisObject3D_cl* GetListenerObject() const { return m_pListenerObject; }

  ///
  /// @}
  ///


  ///
  /// @name Resources
  /// @{
  ///

  /// \brief
  ///   Loads (or returns existing) sound resource (wave file). Different iUsageFlags make the resource unique.
  FMOD_IMPEXP VFmodSoundResource* LoadSoundResource(const char *szFilename, int iUsageFlags=VFMOD_RESOURCEFLAG_DEFAULT);


  /// \brief
  ///   Loads (or returns existing) event project. 
  /// 
  /// \param szEventProjectPath
  ///   Path of a working event project (.fdp), which had been created in the Fmod Designer tool  
  ///
  FMOD_IMPEXP FMOD::EventProject* LoadEventProject(const char *szEventProjectPath);

  /// \brief
  ///   Loads (or returns existing) event group 
  /// 
  /// Please note: for all events within the event group wave data will be loaded and event instances will be allocated.
  /// 
  /// \param szEventProjectPath
  ///   Path of a working event project (.fdp), which had been created in the Fmod Designer tool  
  ///
  /// \param szEventGroupName
  ///   Name of the event group, relative to the event project
  FMOD_IMPEXP VFmodEventGroup* LoadEventGroup(const char *szEventProjectPath, const char *szEventGroupName);
    
  ///
  /// @}
  ///


  ///
  /// @name Instances
  /// @{
  ///

  /// \brief
  ///   Accesses the collection of sound instances
  FMOD_IMPEXP VFmodSoundObjectCollection& SoundInstances() {return m_soundInstances;}

  /// \brief
  ///   Accesses the collection of events
  FMOD_IMPEXP VFmodEventCollection& Events() {return m_events;}

  /// \brief
  ///   Accesses the collection of collision meshes
  FMOD_IMPEXP VFmodCollisionMeshInstanceCollection& CollisionMeshes() { return m_collisionMeshes; }

  /// \brief
  ///   Accesses the collection of reverbs
  FMOD_IMPEXP VFmodReverbCollection& Reverbs() { return m_reverbs; }

  /// \brief
  ///   Creates a sound instance using the passed properties.
  ///   Sound objects are automatically disposed if stopped. To keep sound objects alive,
  ///   the VFMOD_FLAG_NODISPOSE should be provided. A sound object never gets disposed 
  ///   while playing (f.e. infinitely looped sound).
  FMOD_IMPEXP VFmodSoundObject* CreateSoundInstance(const char *szFilename, int iResourceFlags=VFMOD_RESOURCEFLAG_DEFAULT, int iInstanceFlags=VFMOD_FLAG_LOOPED, int iPriority=128);

  /// \brief
  ///   Creates an event using the passed properties.
  ///   Events are automatically disposed if stopped. To keep events alive,
  ///   the VFMOD_FLAG_NODISPOSE should be provided. An event never gets disposed 
  ///   while playing (f.e. infinitely looped event).
  /// 
  /// \param szEventProjectPath
  ///   Path of a working event project (.fdp), which had been created in the Fmod Designer tool 
  ///
  /// \param szEventName
  ///   Name of the event, relative to the event project  
  ///
  /// \param iFlags
  ///   Event instance flags
  FMOD_IMPEXP VFmodEvent* CreateEvent(const char *szEventProjectPath, const char *szEventName, int iFlags=VFMOD_FLAG_NONE);

  ///
  /// @}
  ///


  ///
  /// @name Channel groups 
  /// @{
  ///

  /// \brief
  ///   Sets the volume for all sound instances (not events!)
  FMOD_IMPEXP void SetVolumeAll(float fVol);

  /// \brief
  ///   Mutes/ unmutes all sound instances/ events
  FMOD_IMPEXP void SetMuteAll(bool bStatus);

  /// \brief
  ///   Pauses/ unpauses all sound instances/ events 
  FMOD_IMPEXP void SetPauseAll(bool bStatus);

  /// \brief
  ///   Returns the master channel group, which contains all sound instances except the background music instances
  inline FMOD::ChannelGroup* GetMasterChannelGroup() const { return m_pMasterGroup; }

  /// \brief
  ///   Returns the music channel group, which contains all background music instances
  inline FMOD::ChannelGroup* GetMusicChannelGroup() const { return m_pMusicGroup; }

  ///
  /// @}
  ///


  ///
  /// @name Misc
  /// @{
  ///

  /// \brief
  ///   Returns information on the memory usage of Fmod. This is useful for determining the required memory size, 
  ///   when using a fixed memory pool.
  ///
  /// \param pCurrentMemSize
  ///   Currently allocated memory at time of call. Optional. Specify NULL to ignore.
  ///
  /// \param pMaxMemSize
  ///   Maximum allocated memory since initialization of Fmod event system. Optional. Specify NULL to ignore. 
  ///
  /// \param bBlocking
  ///   Boolean indicating whether to favor speed or accuracy. Specifying true for this parameter will flush the DSP network 
  ///   to make sure all queued allocations happen immediately, which can be costly. 
  /// 
  /// \return
  ///   TRUE on success, otherwise FALSE
  FMOD_IMPEXP bool GetMemoryStats(int *pCurrentMemSize, int *pMaxMemSize, bool bBlocking=false) const;

  /// \brief
  /// 
  ///   Sets a background default reverb environment for the virtual reverb system. This is a reverb, that will be morphed to, 
  ///   if the listener is not within any virtual reverb zones. By default it is set to "off". 
  ///
  /// This method can be called in the OnAfterInitializeFmod callback, in order to set ambient reverb properties directly after
  /// Fmod has been initialized.
  /// 
  /// \param properties
  ///   Reverb properties
  ///
  /// \return
  ///   TRUE on success, otherwise FALSE
  FMOD_IMPEXP bool SetAmbientReverbProperties(VFmodReverbProps &properties);

  // Debug purpose only. Gets count of source voices currently playing.
  FMOD_IMPEXP int GetActiveSourceVoiceCount() const;

  // Debug purpose only. Gets count of source voices currently existing.
  FMOD_IMPEXP int GetExistingSourceVoiceCount() const;

  // Internally used function that is used to notify the manager about any channel or sound playback that is stopped.
  inline void SetAnyStopped(bool bAnyStopped=true)  { m_bAnyStopped = bAnyStopped; }

#if defined (WIN32)

  // Get path to Fmod Designer tool
  FMOD_IMPEXP bool GetDesignerPath(VString &sDesignerPath) const;

#endif

  ///
  /// @}
  ///


  ///
  /// @name Global Callbacks
  /// @{
  ///

  /// \brief
  ///   Triggered before the Fmod system is initialized. Within this callback the Fmod plugin can be configured via VFmodConfig.
  ///
  ///\see
  ///  VFmodConfig
  VisCallback_cl OnBeforeInitializeFmod;

  /// \brief
  ///   Triggered after the Fmod system is initialized. Within this callback for example the ambient reverb properties can be set.
  VisCallback_cl OnAfterInitializeFmod;

  /// \brief
  ///   Triggered before the Fmod system is deinitialized. This is the last moment that Fmod resources can be released.
  VisCallback_cl OnBeforeDeinitializeFmod;

  /// \brief
  ///   Triggered after the Fmod system is deinitialized. After this all Fmod handles are invalid.
  VisCallback_cl OnAfterDeinitializeFmod;

  /// \brief
  ///   Triggered for every VFmodSoundObject instance that fires a callback. The callback data object can be casted into VFmodSoundDataObject,
  ///   which holds information about the sound object, the event type and additional event-type dependent data.
  VisCallback_cl OnSoundCallback;

  /// \brief
  ///   Triggered for every VFmodEvent instance that fires a callback. The callback data object can be casted into VFmodEventDataObject,
  ///   which holds information about the event instance, the event type and additional event-type dependent data.
  VisCallback_cl OnEventCallback;

  ///
  /// @}
  ///


  static int PROFILING_FMOD_OVERALL;
  static int PROFILING_FMOD_PUREUPDATE;
  static int PROFILING_FMOD_PURGE;

protected:
  // overridden IVisCallbackHandler_cl function to get notified about scene changes
  FMOD_IMPEXP VOVERRIDE void OnHandleCallback(IVisCallbackDataObject_cl *pData);

  // called once per frame to update the system
  FMOD_IMPEXP void RunTick(float fTimeDelta);

  FMOD_IMPEXP void InitFmodSystem();

  FMOD_IMPEXP void DeInitFmodSystem();

private:
  friend class VFmodSoundResource;
  friend class VFmodSoundObject;
  friend class VFmodEventGroup;
  friend class VFmodEvent;
  friend class VFmodCollisionMeshInstance;

  VFmodSoundResourceManager *m_pSoundResourceManager;
  VFmodEventGroupManager *m_pEventGroupManager;

  VFmodConfig m_config;
  VFmodSoundObjectCollection m_soundInstances;
  VFmodEventCollection m_events;
  VFmodCollisionMeshInstanceCollection m_collisionMeshes;
  VFmodReverbCollection m_reverbs;
  VisObject3D_cl *m_pListenerObject;
  bool m_bAnyStopped;

  static VFmodManager g_GlobalManager;

  FMOD::EventSystem *m_pEventSystem;
  FMOD::System *m_pSystem;
  void *pMemoryPool;     ///< fixed size of memory pool for Fmod usage, which will be used, when bUseMemoryPool (in VFmodConfig) is set to true
  FMOD::ChannelGroup *m_pMasterGroup;
  FMOD::ChannelGroup *m_pMusicGroup;

#if defined(_VISION_MOBILE)
  bool m_bMasterGroupPausedInForeground;
  bool m_bMusicGroupPausedInForeground;
  bool m_bMasterEventCategoryPausedInForeground;
#endif

  float m_fTimeLeftOver; ///< simulation time left over from last simulation frame
};

#endif // VFMODMANAGER_HPP_INCLUDED

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
