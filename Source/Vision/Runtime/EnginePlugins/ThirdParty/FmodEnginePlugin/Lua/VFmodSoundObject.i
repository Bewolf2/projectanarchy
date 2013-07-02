#ifndef VLUA_APIDOC

%nodefaultctor VFmodSoundObject;
%nodefaultdtor VFmodSoundObject;

class VFmodSoundObject : public VisObject3D_cl
{
public:

  %rename(Remove) DisposeObject();
  virtual void DisposeObject();
  
  void Play(float fStartTime=0.0f, bool bAlsoInEditor=true); 
  void Stop();

  bool IsPlaying() const;
  bool IsReady() const;
  float GetCurrentPosition(bool bAbsolute = false) const;

  inline bool IsPaused() const;
  void SetPaused(bool bStatus);

  inline bool IsMuted() const;
  void SetMuted(bool bStatus);
  
  inline float GetVolume() const;
  void SetVolume(float fVol);

  inline float GetPan() const;
  void SetPan(float fPan);

  inline float Get3DFadeMinDistance() const;
  inline float Get3DFadeMaxDistance() const;
  void Set3DFadeDistance(float fMin, float fMax);

  inline bool IsFading() const;
  inline bool IsFadingIn() const;
  inline bool IsFadingOut() const;
  void FadeTo(float fNewVolume, float fDuration);
  void FadeOut(float fDuration);
  void FadeIn(float fDuration);
  void FadeFromTo(float fStartVolume, float fTargetVolume, float fDuration);

  inline float GetPitch() const;
  void SetPitch(float Pitch); 

  inline float GetConeAngleInside() const;
  inline float GetConeAngleOutside() const;
  void SetConeAngles(float fInside, float fOutside);

  inline bool IsLooped() const;
  inline bool IsMusic() const;
  inline bool IsAutoDisposed() const;

  inline bool IsValid() const;

  inline float GetStartTime() const;

  %extend{
    const char *GetName()
    {
      if (self->GetResource())
        return self->GetResource()->GetFilename();
        
      return NULL;
    }
  
    VSWIG_CREATE_CAST(VFmodSoundObject)
  }
};

#else

/// \brief 
///   Fmod module class: Lua wrapper class for VFmodSoundObject.
class VFmodSoundObject : public VisObject3D_cl 
{
public:

  ///
  /// @name Sound Object Functions
  /// @{
  ///
  
  /// \brief 
  ///   Stops the sound and removes it from the collection.
  /// This function should be called to destroy a sound.
  void Remove();
  
  /// \brief 
  ///   Gest the name of the sound (file name).
  ///
  /// \return 
  ///   The file name of the sound.
  string GetName();

  /// \brief 
  ///  (Re-)plays the sound starting at 'startTime' seconds.
  ///
  /// \param startTime 
  ///   Time offset in seconds to start playback.
  ///
  /// \param alsoInEditor 
  ///   If true, then playback will also be started inside vForge.
  ///
  /// \par Example
  ///   \code
  ///     -- access the Fmod manager
  ///     local sound = Fmod.GetManager():GetSound("Explosion")
  ///     sound:Play(3.5, false)
  ///   \endcode
  void Play(number startTime=0, boolean alsoInEditor=true);

  /// \brief 
  ///   Stops the sound.
  ///
  /// \par Example
  ///   \code
  ///     -- Lua script attached to a VFmodSoundObject
  ///     self:Stop()
  ///   \endcode
  void Stop();

  /// \brief 
  ///   Indicates whether the sound is currently playing.
  ///
  /// \return 
  ///   True if the sound is currently playing, otherwise false.
  boolean IsPlaying();

  /// \brief 
  ///   Indicates whether the sound has finished loading.
  ///
  /// \return 
  ///   True if the sound is fully loaded, otherwise false.
  boolean IsReady();

  /// \brief 
  ///   Returns the current play position inside the sound.
  ///
  /// \param absolute 
  ///   Returned seconds to be absolute or relative to start time.
  ///
  /// \return 
  ///   The current position of playback.
  number GetCurrentPosition(boolean absolute = false);

  /// \brief 
  ///   Check whether this sound instance is paused or not. 
  ///
  /// \return 
  ///   True if paused, otherwise false.
  boolean IsPaused();
  
  /// \brief 
  ///   Pause or continue this sound.
  ///
  /// \param pause 
  ///   Set to true to pause or false to continue from paused state.
  void SetPaused(boolean pause);
  
  /// \brief 
  ///   Check whether this sound instance is muted or not. 
  ///
  /// \return 
  ///   True if muted, otherwise false.
  boolean IsMuted();
    
  /// \brief 
  ///   Mute or unmute this sound.
  ///
  /// \param mute 
  ///   Set to true in order to mute this sound.
  ///
  /// \par Example
  ///   \code
  ///     -- Lua script attached to a VFmodSoundObject
  ///     self:SetMuted(true)
  ///   \endcode
  void SetMuted(boolean mute);

  /// \brief 
  ///   Gets the current volume.
  ///
  /// \return 
  ///   The current volume from 0 to 1.
  number GetVolume();
    
  /// \brief 
  ///   Set the volume of this sound.
  ///
  /// \param volume 
  ///   The volume in a range from 0 to 1.
  void SetVolume(number volume);

  /// \brief 
  ///   Gets the stereo panning value for 2D sounds(spread of a sound signal between left and right).
  ///
  /// \return 
  ///   The current stereo panning value.
  number GetPan();
    
  /// \brief 
  ///   Sets the stereo panning for 2D sounds (spread of a sound signal between left and right). 
  ///
  /// \param pan
  ///   The new panning value.
  void SetPan(number pan);

  /// \brief
  ///   Returns the fade distance at which the 3D sound has full volume.
  ///
  /// \return
  ///   Start distance for attenuation.
  number Get3DFadeMinDistance();

  /// \brief
  ///   Returns the fade distance at which the 3D sound has faded out.
  ///
  /// \return
  ///   Stop distance for attenuation.
  number Get3DFadeMaxDistance();

  /// \brief 
  ///   Sets distance attenuation parameters for 3D sounds. 
  ///
  /// \param min 
  ///   Start distance for attenuation.
  ///
  /// \param max 
  ///   Stop distance for attenuation.
  void Set3DFadeDistance(number min, number max);

  /// \brief 
  ///   Check if sound object is currently fading (from a volume value to another). 
  ///
  /// \return 
  ///   True if this sound is fading, otherwise false.
  boolean IsFading(); 

  /// \brief 
  ///   Check if sound object is currently fading in. 
  ///
  /// \return 
  ///   True if this sound is fading in, otherwise false.
  boolean IsFadingIn();

  /// \brief 
  ///   Check if sound object is currently fading out. 
  ///
  /// \return 
  ///   True if this sound is fading out, otherwise false.
  boolean IsFadingOut();

  /// \brief 
  ///   Fades the sound objects volume to a specific value over the given duration.
  ///
  /// \param newVolume 
  ///   Fade to this new volume value (0 to 1).
  ///
  /// \param duration 
  ///   The duration in seconds.
  void FadeTo(number newVolume, number duration);
 
  /// \brief
  ///   Fades the sound objects volume out over the given duration. 
  ///
  /// \param duration 
  ///   The duration in seconds.
  void FadeOut(number duration);

  /// \brief 
  ///   Fades the sound objects volume in over the given duration.
  ///
  /// \param duration 
  ///   The duration in seconds.
  void FadeIn(number duration);

  /// \brief 
  ///   Fades the sound objects volume from a specific volume to another over the given duration.
  ///
  /// \param startVolume 
  ///   Start fading from this volume value (0 to 1).
  ///
  /// \param targetVolume 
  ///   Fade to this volume value (0 to 1).
  ///
  /// \param duration 
  ///   The duration in seconds.
  void FadeFromTo(number startVolume, number targetVolume, number duration);

  /// \brief 
  ///   Gets the current pitch value.
  ///
  /// A pitch of 1.0 means playback at default speed. A pitch of 0.5 means half speed playback.
  /// A pitch of 2.0 means double speed playback. 
  ///
  /// \return 
  ///   The pitch (frequencies in cycles per second) value.
  number GetPitch();
  
  /// \brief 
  ///   Set the current pitch value.
  ///
  /// A pitch of 1.0 means playback at default speed. A pitch of 0.5 means half speed playback.
  /// A pitch of 2.0 means double speed playback. 
  ///
  /// \param pitch 
  ///   The new pitch (frequencies in cycles per second) value.
  void SetPitch(number pitch);

  /// \brief
  ///   Returns the inside cone angle for directional sounds.
  ///
  /// \return
  ///   The inside angle in degree.
  number GetConeAngleInside();

  /// \brief
  ///   Returns the outside cone angle for directional sounds.
  ///
  /// \return
  ///   The outside angle in degree.
  number GetConeAngleOutside();

  /// \brief 
  ///   Sets cone angles for directional sounds.
  ///
  /// \note
  ///   The 'inside' angle specifies the cone where the volume is present to 100% (volume value 1.0)
  ///   The 'outside' angle specifies the cone where the volume is present to 0% (volume value 0.0)
  ///   Set (-1,-1) for omni-directional sound (default).
  ///
  /// \param inside 
  ///   The inside angle in degree.
  ///
  /// \param outside 
  ///   The outside angle in degree (needs to be larger than inside).
  ///
  /// \par Example
  ///   \code
  ///     function OnCreate(self) -- Lua script attached to a directional VFmodSoundObject
  ///       self:SetConeAngles(30,50)
  ///     end
  ///   \endcode
  void SetConeAngles(number inside, number outside);

  /// \brief 
  ///   Returns the looped status of this sound instance. 
  ///
  /// \return 
  ///   True if the sound is looping, otherwise false.
  boolean IsLooped();
  
  /// \brief 
  ///   Returns if this sound object is used as music playback. 
  ///  (This has consequences on the XBox 360 for example). 
  ///
  /// \return 
  ///   True if this sound is used as music, otherwise false.
  boolean IsMusic();
  
  /// \brief 
  ///   Returns true if this sound object is disposed automatically
  ///   by Fmod manager when stopped or finished playing.
  ///
  /// \note
  ///   VFmodSoundObjects which have been created via the Lua interface of VFmodManager
  ///   are created without the auto dispose flag.
  ///
  /// \return 
  ///   Returns false if sound object needs to be disposed via Remove(), true if it will be removed automatically. 
  boolean IsAutoDisposed();

  /// \brief
  ///   Returns whether the sound object has a valid Fmod channel for playback
  boolean IsValid();
  
  /// \brief 
  ///   Gets the start time the sound was started playing at. 
  ///
  /// \return
  ///   The start time in sec.
  number GetStartTime();

  ///
  /// @}
  ///

};

#endif