#ifndef VLUA_APIDOC

%nodefaultctor VFmodManager;
%nodefaultdtor VFmodManager;

class VFmodManager
{
public:

  %extend{
    VFmodSoundObject* CreateSound(const hkvVec3 *pPos, const char *szFilename, bool bLooped=false, const char *szKey=NULL)
    {
      int iFlags = VFMOD_FLAG_PAUSED|VFMOD_FLAG_NODISPOSE;
      if (bLooped) 
        iFlags |= VFMOD_FLAG_LOOPED;
      
      VFmodSoundObject* pObject = self->CreateSoundInstance(szFilename, VFMOD_RESOURCEFLAG_DEFAULT, iFlags);
      
      if(pObject == NULL)
        return NULL;

      if (pPos)   
        pObject->SetPosition(*pPos);
      if (szKey)
        pObject->SetObjectKey(szKey);
      
      return pObject;
    }

    VFmodEvent* CreateEvent(const hkvVec3 *pPos, const char *szEventProjectPath, const char *szEventName, const char *szKey=NULL)
    {
      int iFlags = VFMOD_FLAG_PAUSED|VFMOD_FLAG_NODISPOSE;
      
      VFmodEvent* pEvent = self->CreateEvent(szEventProjectPath, szEventName, iFlags);
      
      if(pEvent == NULL)
        return NULL;

      if (pPos)   
        pEvent->SetPosition(*pPos);
      if (szKey)
        pEvent->SetObjectKey(szKey);
      
      return pEvent;
    }

    VFmodSoundObject* GetSound(const char *szKey)
    {
      if (szKey)
        return VFmodManager::GlobalManager().SoundInstances().SearchObject(szKey);
      return NULL;
    }

    VFmodEvent* GetEvent(const char *szKey)
    {
      if (szKey)
        return VFmodManager::GlobalManager().Events().SearchEvent(szKey);
      return NULL;
    }
    
    void ResetAll()
    {
      VFmodManager::GlobalManager().SoundInstances().StopAll(true);
      VFmodManager::GlobalManager().SoundInstances().Clear();
      VFmodManager::GlobalManager().Events().StopAll(true);
      VFmodManager::GlobalManager().Events().Clear(); 
    }
    
    VSWIG_CREATE_CAST_UNSAFE(VFmodManager)
  }

  void SetVolumeAll(float fVol);
  void SetMuteAll(bool bStatus);
  void SetPauseAll(bool bStatus);

};

#else

/// \brief 
///   The Lua \b Fmod global is a wrapped instance of the global VFmodManager.
/// \note
///   The Lua module containing all wrappes is called FireLight, however there is no need to access the module directly.
/// \par Example
///   \code
///     -- grab a sound object via the object key and play it
///     local sound = Fmod:GetSound("MySound")
///     sound:Play()
///   \endcode
class VFmodManager 
{
public:
  
  ///
  /// @name Instance Handling
  /// @{
  ///
  
  /// \brief 
  ///   Create a new sound object (in paused state).
  ///
  /// \param pos 
  ///   The position, where to create the sound object.
  ///
  /// \param filename 
  ///   The sound file to load.
  ///
  /// \param looped (\b optional) 
  ///   Set to true in order to loop the sound.
  ///
  /// \param key (\b optional)
  ///   The object key for the sound object.
  ///
  /// \return 
  ///   A new sound object or nil on errors (e.g. the file you specified is not present).
  ///
  /// \par Example
  ///   \code
  ///     -- access the Fmod manager
  ///     local sound = Fmod:CreateSound(Vision.hkvVec3(), "detonation.wav")
  ///     Vision.Assert(sound~=nil, "Sound is not present")
  ///     sound:Play()
  ///   \endcode
  VFmodSoundObject CreateSound(hkvVec3 pos, string filename, boolean looped=false, string key=nil);

  /// \brief 
  ///   Create a new event (in paused state).
  ///
  /// \param pos 
  ///   The position, where to create the event.
  ///
  /// \param szEventProjectPath 
  ///   Path of a working event project (.fdp), which had been created in the Fmod Designer tool.
  ///
  /// \param szEventName 
  ///   Name of the event, relative to the event project.
  ///
  /// \param looped (\b optional) 
  ///   Set to true in order to loop the event.
  ///
  /// \param key (\b optional)
  ///   The object key for the event.
  ///
  /// \return 
  ///   A new event or nil on errors (e.g. the event project you specified is not present).
  ///
  /// \par Example
  ///   \code
  ///     -- access the Fmod manager
  ///     local event = Fmod:CreateEvent(Vision.hkvVec3(), "Sound/examples","effects/detonation")
  ///     Vision.Assert(event~=nil, "Event is not present")
  ///     event:Start() 
  ///   \endcode
  VFmodEvent CreateEvent(hkvVec3 pos, string szEventProjectPath, string szEventName, boolean looped=false, string key=nil);
 
  /// \brief 
  ///   Search for a sound object via the object key.
  /// \param key 
  ///   The object key as search criterion.
  /// \return 
  ///   The sound object or nil if there is no sound object with that key.
  /// \par Example
  ///   \code
  ///     -- access the Fmod manager
  ///     local sound = Fmod:GetSound("MySound")
  ///     sound:Play()
  ///   \endcode
  VFmodSoundObject Get(string key);

  /// \brief 
  ///   Search for an event via the object key.
  ///
  /// \param key 
  ///   The object key as search criterion.
  ///
  /// \return 
  ///   The event or nil if there is no event with that key.   
  ///   
  /// \par Example
  ///   \code
  ///     -- access the Fmod manager
  ///     local sound = Fmod:GetSound("MySound")
  ///     sound:Play()
  ///   \endcode
  VFmodEvent Get(string key);

  ///
  /// @}
  ///


  /// @}
  /// @name Common Functions
  /// @{
  
  /// \brief 
  ///   Reset all sounds/ events (useful to stop looping sounds/ events, especially in the editor).
  ///
  /// \par Example
  ///   \code
  ///     function OnDestroy(self)
  ///       Fmod:ResetAll() -- avoid endlees looping sounds/ events 
  ///     end
  ///   \endcode
  void ResetAll();

  /// \brief 
  ///   Sets the volume for all sound instances (not events!)
  ///
  /// \param volume 
  ///   The volume in a range from 0 to 1.
  void SetVolumeAll(number volume);
  
  /// \brief 
  ///   Mutes/ unmutes all sound instances/ events.
  ///
  /// \param mute 
  ///   Set to true in order to mute all sound instances/ events.
  void SetMuteAll(boolean mute);
  
  /// \brief 
  ///   Pauses/ unpauses all sound instances/ events .
  ///
  /// \param pause 
  ///   Set to true to pause or false to continue from paused state.
  void SetPauseAll(boolean pause);

  ///
  /// @}
  ///

};

#endif