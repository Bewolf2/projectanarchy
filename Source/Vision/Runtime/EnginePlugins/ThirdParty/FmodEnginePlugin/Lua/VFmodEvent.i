#ifndef VLUA_APIDOC

%nodefaultctor VFmodEvent;
%nodefaultdtor VFmodEvent;

class VFmodEvent : public VisObject3D_cl
{
public:

  %rename(Remove) DisposeObject();
  virtual void DisposeObject();
  
  void Start(bool bAlsoInEditor=true); 
  void Stop();

  bool IsPlaying() const;
  bool IsReady() const;

  inline bool IsPaused() const;
  void SetPaused(bool bStatus);

  inline bool IsMuted() const;
  void SetMuted(bool bStatus);

  inline bool IsAutoDisposed() const;

  inline bool IsValid() const;
 
  %extend{
    const char *GetName()
    {
      char *szEventName = NULL;
      self->GetInfo(NULL,&szEventName,NULL);
      return szEventName;
    }
  
    VSWIG_CREATE_CAST(VFmodEvent)
  }
};

#else

/// \brief 
///   Fmod module class: Lua wrapper class for VFmodEvent.
class VFmodEvent : public VisObject3D_cl 
{
public:

  /// 
  /// @name Event Functions
  /// @{
  ///
  
  /// \brief 
  ///   Stops the event and removes it from the collection.
  /// This function should be called to destroy an event.
  void Remove();
  
  /// \brief 
  ///   Gest the name of the event.
  ///
  /// \return 
  ///   The name of the event.
  string GetName();

  /// \brief
  ///   Starts playing this event. 
  /// 
  /// \param alsoInEditor
  ///   If true, then playback will also be started inside vForge.
  ///
  /// \par Example
  ///   \code
  ///     -- access the Fmod manager
  ///     local event = Fmod.GetManager():GetEvent("Explosion")
  ///     event:Start(false)
  ///   \endcode
  void Start(bool alsoInEditor=true); 

  /// \brief 
  ///   Stops the event.
  ///
  /// \par Example
  ///   \code
  ///     -- Lua script attached to a VFmodEvent
  ///     self:Stop()
  ///   \endcode
  void Stop();

  /// \brief 
  ///   Indicates whether the event is currently playing.
  ///
  /// \return 
  ///   True if the event is currently playing, otherwise false.
  boolean IsPlaying();

  /// \brief 
  ///   Indicates whether the event has finished loading.
  ///
  /// \return 
  ///   True if the event is fully loaded, otherwise false.
  boolean IsReady();

  /// \brief 
  ///   Check whether this event is paused or not. 
  ///
  /// \return 
  ///   True if paused, otherwise false.
  boolean IsPaused();
  
  /// \brief 
  ///   Pause or continue this event.    
  ///
  /// \param pause 
  ///   Set to true to pause or false to continue from paused state.
  void SetPaused(boolean pause);
  
  /// \brief 
  ///   Check whether this event is muted or not. 
  ///
  /// \return 
  ///   True if muted, otherwise false.
  boolean IsMuted();
    
  /// \brief 
  ///   Mute or unmute this event.
  ///
  /// \param mute 
  ///   Set to true in order to mute this event.
  ///
  /// \par Example
  ///   \code
  ///     -- Lua script attached to a VFmodEvent
  ///     self:SetMuted(true)
  ///   \endcode
  void SetMuted(boolean mute);

  /// \brief 
  ///   Returns true if this event is disposed automatically
  ///   by Fmod manager when stopped or finished playing.
  ///
  /// \note
  ///   VFmodEvents which have been created via the Lua interface of VFmodManager
  ///   are created without the auto dispose flag.
  ///
  /// \return 
  ///   Returns false if event needs to be disposed via Remove(), true if it will be removed automatically. 
  boolean IsAutoDisposed();

  /// \brief
  ///   Returns whether the event has a valid Fmod event
  boolean IsValid();
  
  ///
  /// @}
  ///

};

#endif