
#ifndef VLUA_APIDOC

%nodefaultctor IVTimer;
%nodefaultdtor IVTimer;

class IVTimer
{
public:
 
  float GetTime() const;

  %rename(SetForcedFrameRate) ForceFrameRate(int framesPerSecond=0);
  void  ForceFrameRate (int framesPerSecond=0);

  int  GetForcedFrameRate () const;

  void  SetMaxTimeDifference (float fMaxDifference);
 
  float  GetMaxTimeDifference () const;


  %extend{
  
    float GetTimeDiff()
    {     
      VScriptResourceManager* pMan = (VScriptResourceManager*) Vision::GetScriptManager();
      VASSERT(pMan);

      // script thinking delta or simulation tick delta
      float fTimeDiff = pMan->GetThinkInterval();
      
      if(fTimeDiff<=0) return Vision::GetTimer()->GetTimeDifference();
      return fTimeDiff;
    }
    
    void SetScriptThinkInterval(float fTimeInSec = 0.1f)
    {
      VScriptResourceManager* pMan = (VScriptResourceManager*) Vision::GetScriptManager();
      VASSERT(pMan);
      
      pMan->SetThinkInterval(fTimeInSec);
    }
    
    float GetScriptThinkInterval()
    {
      VScriptResourceManager* pMan = (VScriptResourceManager*) Vision::GetScriptManager();
      VASSERT(pMan);
      
      return pMan->GetThinkInterval();
    }
    
    float GetThinkInterval()
    {     
      return Vision::GetTimer()->GetTimeDifference();
    }
    
    void EnableSlowMotion(bool bEnable)
    {
      self->SetSlowMotion(bEnable ? VIS_ENABLED : VIS_DISABLED);
    }
    
    bool IsSlowMotionEnabled()
    {
      return self->GetSlowMotion() == VIS_ENABLED;
    }
    
    void SetFrozen(bool bEnable)
    {
      self->SetFrozen(bEnable ? VIS_ENABLED : VIS_DISABLED);
    }
    
    bool IsFrozen()
    {
      return self->GetFrozen() == VIS_ENABLED;
    }
  }
      
  void SetSlowMotionTimeScale(float fTimeScale);
  float GetSlowMotionTimeScale() const;
};


#else

/// \brief The Game Timer, the main instance in Lua is reachable via the global \b Timer instance.
///   \code
///   function OnThink(self)
///     local diff = Timer:GetTimeDiff()
///     self:IncPosition(50*diff,0,0)
///   end
///   \endcode
class IVTimer {
public:

  /// @name Constructors
  /// @{
  
  /// \brief Get the elapsed time since the scene was loaded.
  /// \return The elapsed time in sec.
  number GetTime();
  
  /// \brief Returns the time delta since the last frame.
  /// \note This function takes case about the possible different time intervals of the native and the scripting system.
  /// \return The time difference in sec.
  /// \par Example
  ///   \code
  ///     local diff = Timer:GetTimeDiff()
  ///   \endcode
  number GetTimeDiff();
  
  /// \brief Setup a diffrent think interval for script entities (default interval = 0).
  /// \param timeInSec The desired interval between two think calls (Set to 0 to use the same cycle as the native system).
  void SetScriptThinkInterval(number timeInSec = 0.1);
  
  /// \brief Returns the current think interval of script entities.
  /// \note Is zero if the script entites use the same intervall as the native system.
  /// \returns The think interval (time difference) of the scripting entities.
  /// \see SetScriptThinkInterval
  /// \see GetThinkInterval
  /// \see GetTimeDiff
  number GetScriptThinkInterval();
  
  /// \brief Returns the current think interval.
  /// \returns The think interval (time difference) of the native system.
  /// \see SetScriptThinkInterval
  /// \see GetScriptThinkInterval
  /// \see GetTimeDiff
  number GetThinkInterval();

  /// \brief Enable slow motion mode.
  /// \param enable Set to treu for slow motion
  /// \see SetSlowMotionTimeScale
  /// \see IsSlowMotionEnabled
  /// \see SetSlowMotionTimeScale
  /// \see GetSlowMotionTimeScale
  /// \par Example
  ///   \code
  ///     Timer:EnableSlowMotion(true)
  ///   \endcode
  void EnableSlowMotion(boolean enable);
  
  /// \brief Check if the slow motion mode is enabled.
  /// \return true = slow motion enables; false = slow motion disabled.
  /// \see SetSlowMotionTimeScale
  /// \see GetSlowMotionTimeScale
  boolean IsSlowMotionEnabled();
      
  /// \brief Setup the time scale of the slow motion mode.
  /// \param timeScale The scaling factor for the tim in slow motion mode.
  /// \see EnableSlowMotion
  /// \see IsSlowMotionEnabled
  /// \par Example
  ///   \code
  ///     Timer:SetSlowMotionTimeScale(0.5)
  ///   \endcode
  void SetSlowMotionTimeScale(number timeScale);
  
  /// \brief Get the current time scale of the slow motion mode (is only applied in the slow motion mode)
  /// \return The current time scale.
  /// \see EnableSlowMotion
  /// \see IsSlowMotionEnabled
  number GetSlowMotionTimeScale();
  
  /// \brief Forces the timer class to simulate a certain frame rate.
  /// \param framesPerSecond The forced amount of frames per second.
  /// \see GetForcedFrameRate
  void SetForcedFrameRate(number framesPerSecond);

  /// \brief Returns the value of the forced frame rate set via ForceFrameRate.
  /// \return The forced frame rate.
  /// \see ForceFrameRate
  number GetForcedFrameRate();

  /// \brief Sets the clamping value for the maximum time difference.
  /// \param maxDifference The max amount of frames per second.
  /// \see GetMaxTimeDifference
  void SetMaxTimeDifference(number maxDifference);
 
  /// \brief Gets the current value of the maximum time difference.
  /// \return The max amount of frames per second.
  /// \see SetMaxTimeDifference
  number GetMaxTimeDifference();

  /// \brief Freeze the timer.
  /// \param enable Set to true in order to freeze the timer.
  void SetFrozen(boolean enable);
    
  /// \brief Check if the timer is frozen.
  /// \return true = timer is frozen; false = timer is not frozen.
  boolean IsFrozen();

  /// @}
};

#endif
