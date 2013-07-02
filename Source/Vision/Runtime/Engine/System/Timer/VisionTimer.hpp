/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisionTimer.hpp

#ifndef DEFINE_VISIONTIMER
#define DEFINE_VISIONTIMER


/// \brief
///   Interface class that handles timer functionality.
/// 
/// This is an interface class for a frame timer. Implementations of this interface
/// can be used as the primary timer for timer-dependent engine functions in Vision 
/// using the Vision::SetTimer() and Vision::GetTimer() methods. 
/// 
/// Any modifications you do to the global timer (e.g. slow motion, setting max. 
/// time difference etc.) will affect the behavior of the engine. 
/// 
/// \note 
///   All timing values are measured in seconds (floating point), thus a 
///   time difference of 0.02 matches an actual elapsed time of 20 milliseconds.
///     
///  \sa Vision::GetTimer
///  \sa Vision::SetTimer
class IVTimer : public VRefCounter
{
public:
 
  /// \brief
  /// Default constructor
  VISION_APIFUNC IVTimer() {}

  /// \brief
  /// Virtual destructor
  VISION_APIFUNC VOVERRIDE ~IVTimer() {}

  ///
  /// @name Customizable Timer Functionality
  /// @{
  ///

  /// \brief
  ///   Initializes this timer and sets the time to 0.
  VISION_APIFUNC virtual void Init();


  /// \brief
  ///   Updates this timer.
  /// 
  /// This function has to be called each simulation tick to update the time values for GetTime and
  /// GetTimeDifference. Custom implementations of this interface must implement this and fill relevant data
  /// such as the time difference.
  VISION_APIFUNC virtual void Update() = 0;

  /// \brief
  ///   Returns the time delta since the last frame.
  /// 
  /// In case the timer implementation performs a filtering/smoothing of time steps, this method should be overwritten
  /// in order to return the raw/unfiltered time-step in seconds.
  /// 
  /// \return
  ///   the raw time delta since the last frame.
  VISION_APIFUNC virtual float GetUnfilteredTimeDifference() const;
  

  /// \brief
  ///   Serializes the current state of timer.
  /// 
  /// This function serializes/deserializes the current state (including the actual time values) of this timer 
  /// instance. Upon deserialization, the timer continues in the state and with the time of the serialized timer. 
  /// 
  /// \param ar
  ///   The archive to read from or write to
  VISION_APIFUNC virtual void SerializeX( VArchive &ar );

  ///
  /// @}
  ///


  ///
  /// @name Enable / Disable Functions
  /// @{
  ///

  /// \brief
  ///   Disables the timer
  /// 
  /// Disables the timer and sets the standard duration between two frames to 0.03 seconds (simulates about 33 fps per
  /// seconds). 
  /// 
  /// \note
  ///   This function is useful for debug sessions, since the timer always returns a constant time
  ///   difference regardless of the time spend in breakpoints etc.
  inline void Disable();


  /// \brief
  ///   Enables the timer
  /// 
  /// Enables the timer if the timer has been disabled before.
  inline void Enable();
  

  ///
  /// @}
  ///

  ///
  /// @name Information Functions
  /// @{
  ///

  /// \brief
  ///   Returns the current time in the current frame relative to the last scene loaded.
  /// 
  /// This time gets updated per tick, which means that you will always get the same time value
  /// during one frame/tick by using this function. Whenever you need accurate timing you should
  /// use the IVTimer::GetCurrentTime function instead.
  /// 
  /// \return
  ///   elapsed time in seconds since the first frame
  /// 
  /// \sa IVTimer::GetCurrentTime
  VISION_APIFUNC float GetTime() const;


  
  /// \brief
  ///   Returns the accurate current time relative to the initialization of the timer.
  /// 
  /// GetCurrentTime returns the time which has elapsed since the initialization of the timer.
  /// This time is not only updated per tick/frame, instead this function computes the current time
  /// whenever it gets called.
  /// 
  /// You should use this function whenever you need to keep track of timings within a frame, since
  /// the IVTimer::GetTime function doesn't update it's time within one single frame.
  /// 
  /// GetCurrentTime always returns the real time and doesn't consider any forced framerate or slow
  /// motion mode. In case you use forced framefrates or slow motion, use the IVTimer::GetTime
  /// function instead.
  /// 
  /// Using GetCurrentTime in slow motion mode or with a forced frame rate will not return the
  /// correct results.
  /// 
  /// \return
  ///   elapsed time in seconds since the initialization of the timer
  /// 
  /// \sa IVTimer::GetTime
  VISION_APIFUNC float GetCurrentTime() const;


  /// \brief
  ///   Returns the time delta since the last frame.
  /// 
  /// GetTimeDifference returns the time delta in seconds that has been elapsed since the timer
  /// update in the last frame.
  /// 
  /// \return
  ///   the time delta, in seconds, since the last frame
  inline float GetTimeDifference() const;

  /// \brief
  ///   Sets the time difference for this frame, that going to be returned by GetTimeDifference
  /// 
  /// This function is useful during playback of recorded data.
  /// 
  /// It should be called after Vision::GetTimer()->Update since the update function evaluates the actual
  /// delta
  /// 
  /// \param fDiff
  ///   The time difference in seconds used for this frame until next call of Vision::GetTimer()->Update()
  inline void SetTimeDifference(float fDiff) {m_fTimeDifference=fDiff;}

  /// \brief
  ///   Returns the current status of the timer (see Enable/Disable function)
  /// 
  /// \return
  ///   \c true if the timer is currently disabled
  VISION_APIFUNC bool IsDisabled() const;  


  ///
  /// @}
  ///

  ///
  /// @name Other Functions
  /// @{
  ///


  /// \brief
  ///   Set the timer to a specific absolute time
  /// 
  /// \param fTime
  ///   new time in seconds
  /// 
  /// \note
  ///   This function can be used to restore the timer after loading a saved game
  VISION_APIFUNC void SetTime( float fTime );

  /// \brief
  ///   Forces the timer class to simulate a certain frame rate
  /// 
  /// After calling, the timer class returns a static time difference, which means that
  /// the timer simulates a user definable frame rate.
  /// 
  /// If activated, GetTime and GetTimeDifference do not return the real time and time difference
  /// anymore. Instead, GetTimeTifference() always returns 1.f/framesPerSecond.
  /// 
  /// This function is useful when recording a video of the application.
  /// 
  /// Note that the result will not look correct. For instance, if you render 1000 frames along a
  /// pre-defined camera path this might take 10 seconds on a fast system and 30 seconds on a
  /// low-end system.
  /// 
  /// To disable this feature, pass 0 to this function.
  /// 
  /// \param framesPerSecond
  ///   frames per seconds the timer shall simulate (0 disabled the forced framerate)
  VISION_APIFUNC void ForceFrameRate(int framesPerSecond = 0);

  /// \brief
  ///   Returns the value of the forced frame rate set via ForceFrameRate
  /// 
  /// \return
  ///   the forced frame rate (or 0 if disabled)
  /// 
  /// \sa ForceFrameRate
  inline int GetForcedFrameRate() const;

  /// \brief
  ///   Skips the next frame without updating the timer.
  /// 
  /// SkipOneFrame causes the timer class to ignore the next timer update. You can use this
  /// function to pause the game for a while without updating the in-game time. If you call this
  /// function the next call of the Update-function will not cause any updates.
  VISION_APIFUNC void SkipOneFrame();
  
  /// \brief
  ///   Sets the clamping value for the maximum time difference.
  /// 
  /// The return value of a IVTimer::GetTimeDifference() call will subsequently be clamped
  /// to the value specified by fMaxDifference.
  /// 
  /// As a consequence, the game appears slower when the real time difference exceeds the maximum
  /// and the difference is clamped.
  /// 
  /// It can be useful to specify a clamping value since some simulation processes (such as
  /// physics) might become unstable for too large time deltas.
  /// 
  /// The default value is 1 second.
  /// 
  /// \param fMaxDifference
  ///   The new maximum time difference (in seconds) returned by GetTimeDifference
  /// 
  /// \sa IVTimer::GetTimeDifference
  /// \sa IVTimer::GetMaxTimeDifference
  inline void SetMaxTimeDifference(float fMaxDifference);

  /// \brief
  ///   Gets the current value of the maximum time difference.
  /// 
  /// \return
  ///   the current maximum time difference (in seconds)
  /// 
  /// \sa IVTimer::GetTimeDifference
  /// \sa IVTimer::GetMaxTimeDifference
  VISION_APIFUNC float GetMaxTimeDifference() const;

  /// \brief
  ///   Enables or disables the slow motion mode of the timer.
  /// 
  /// If enabled, all timing operations are scaled by a scaling factor set via
  /// SetSlowMotionTimeScale.
  /// 
  /// For instance, if you have a time scale of 0.5, GetTimeDifference() will return  0.01 when
  /// actually a time of 0.02 seconds elapsed between two frames.
  /// 
  /// \param bEnable
  ///   new status for slow motion. If true, the timer is switched to slow motion mode, if
  ///   false, the timer is switched back to normal mode
  /// 
  /// \note
  ///   You can call this function at any time after the engine has been initialized.
  /// 
  /// \note
  ///   The internal timer accuracy of 1000 ticks/s might lead to some inaccuracies for low slow
  ///   motion time scales at high framerates, i.e. GetTimeDifference() might return 0.0 for some
  ///   frames.
  /// 
  /// \sa IVTimer::GetSlowMotion
  /// \sa IVTimer::SetSlowMotionTimeScale
  inline void SetSlowMotion(bool bEnable);

  /// \brief
  ///   Gets the current slow motion status of the timer.
  /// 
  /// \return
  ///   the current slow motion status of the timer
  /// 
  /// \sa IVTimer::SetSlowMotion
  /// \sa IVTimer::SetSlowMotionTimeScale
  inline bool GetSlowMotion() const;

  /// \brief
  ///   Sets the time scale for slow motion.
  /// 
  /// This only affects the timer if it is in slow motion mode (SetSlowMotion). The time scale is
  /// a float value that specifies how much time operations  are scaled down. For instance, a scale
  /// of 0.5 will run all simulations at half speed.
  /// 
  /// \param fTimeScale
  ///   The new time scale
  /// 
  /// \sa IVTimer::SetSlowMotion
  /// \sa IVTimer::GetSlowMotionTimeScale
  inline void SetSlowMotionTimeScale(float fTimeScale);

  /// \brief
  ///   Gets the current time scale for slow motion set via SetSlowMotionTimeScale
  /// 
  /// \return
  ///   float fTimeScale: The current time scale
  /// 
  /// \note
  ///   you can call this function at any time after the engine has been initialized
  /// 
  /// \note
  ///   the default scale is 0.5
  /// 
  /// \sa IVTimer::SetSlowMotion
  /// \sa IVTimer::SetSlowMotionTimeScale
  inline float GetSlowMotionTimeScale() const;

  /// \brief
  ///   Enables or disables freezing of the timer
  /// 
  /// If enabled, all timing operations are stopped until disabled, when the time
  /// value and count continue where they were stopped. This feature internally sets the slow motion time scale to 0.0f.
  /// 
  /// \param bFrozen
  ///   new status for frozen timer. If true, the timer is paused, if
  ///   false, the timer is switched back to normal mode.
  /// 
  /// \note
  ///   You can call this function at any time after the engine has been initialized.
  /// 
  /// \sa IVTimer::GetFrozen
  VISION_APIFUNC void SetFrozen(bool bFrozen);

  /// \brief
  ///   Gets the current freezing status of the timer
  /// 
  /// \return
  ///   bool enable: the current freezing status of the timer
  /// 
  /// \sa IVTimer::SetFrozen
  bool GetFrozen() const;

  ///
  /// @}
  ///

protected:
  /// \brief
  ///   Returns the clock frequency of the timer in counts per second
  uint64 GetClockFreq() const;

  bool m_bFirstStart;                    ///< used for skipping the next frame
  bool m_bDisabled;                      ///< if set to TRUE if timer is currently disabled
  bool m_bFrozen;                        ///< is set to TRUE if timer is currently frozen
  bool  m_bSlowMotionEnabled;
  float m_fDivCountsPerSecond;           ///< normally 1 / 1000
  uint64 m_iOldCount;                     ///< old timer value in counts per second
  uint64 m_iStartTimerValue;              ///< startup value of the timer at initialisation
  uint64 m_iForcedCountNumber;            ///< contains the ms to simulate if timer is disabled or ForceFrameRate has been called
  float m_fTime;                         ///< current time in seconds
  float m_fTimeDifference;               ///< time difference between last frame and current frame in seconds

  float m_fMaxTimeDifference;
  float m_fSlowMotionScale;
  uint64 m_iCurrentTimerTickPos;
};

#include <Vision/Runtime/Engine/System/Timer/VisionTimer.inl>

typedef VSmartPtr<IVTimer> IVTimerPtr;


/// Serialization versions
#define VDEFAULTTIMER_VERSION_1         1                        // Initial version
#define VDEFAULTTIMER_VERSION_CURRENT   VDEFAULTTIMER_VERSION_1

#define VDEFAULTTIMER_FILTER_FRAMECOUNT 11                       // Number of frames for filtering time steps (must be greater than 4)

/// \brief
///   Default engine implementation of the IVTimer interface
/// 
/// This is the default implementation of the IVTimer interface. Vision uses this 
/// timer implementation by default when Vision::GetTimer() is called, and a custom
/// timer is set by the application.
/// 
/// Any modifications you do to the global timer (e.g. slow motion, setting max. 
/// time difference etc.) will affect the behavior of the engine. 
/// 
/// \note 
///   All timing values are measured in seconds (floating point), thus a 
///   time difference of 0.02 matches an actual elapsed time of 20 milliseconds.
/// 
/// \note
///   The Timer class provides float accuracy (e.g. return value of GetTimeDifference), but it
///   internally has an accuracy of 1000 ticks per second. Especially when you use slow motion,
///   there might be some accuracy problems, e.g. the time difference returns 0.0 for every second
///   frame. This is not an issue as long as your application and custom physics system can handle
///   time differences of 0.0 properly.
/// 
/// \note
///   This implementation of the IVTimer interface performs per default a filtering/ smoothing of the 
///   time steps. This can significantly improve jerky motions inside your application.
///   
///  \sa IVTimer
///  \sa Vision::SetTimer
///  \sa Vision::GetTimer
class VDefaultTimer : public IVTimer
{
public:
  /// \brief
  /// Default constructor
  VISION_APIFUNC VDefaultTimer(bool bDeleteObjectUponUnref=true) {m_bDeleteObject=bDeleteObjectUponUnref;}

  /// \brief
  /// Virtual destructor
  VISION_APIFUNC virtual ~VDefaultTimer() {}

  /// \brief
  /// Overridden function called upon deletion time
  VISION_APIFUNC virtual void DeleteThis() HKV_OVERRIDE
  {
    if (m_bDeleteObject)
      IVTimer::DeleteThis();
  }

  VISION_APIFUNC virtual void Init() HKV_OVERRIDE;
  VISION_APIFUNC virtual void Update() HKV_OVERRIDE;
  VISION_APIFUNC virtual float GetUnfilteredTimeDifference() const HKV_OVERRIDE;
  VISION_APIFUNC virtual void SerializeX( VArchive &ar ) HKV_OVERRIDE;

  /// \brief
  ///   Returns whether time steps are currently filtered/ smoothed.
  inline bool IsTimeStepFilteringEnabled() const { return m_bFilterTimeStep; }

  /// \brief
  ///   En-/ Disables filtering/ smoothing of time steps.
  inline void EnableTimeStepFiltering(bool bEnable) { m_bFilterTimeStep = bEnable; }

  /// \brief
  ///   Gets the factor [0..1] for filtering/ smoothing the time steps. 
  inline float GetSmoothFactor() const { return m_fSmoothFactor; }

  /// \brief
  ///   Sets the factor [0..1] for filtering/ smoothing the time steps. 
  inline void SetSmoothFactor(float fSmoothFactor) { m_fSmoothFactor = fSmoothFactor; }

  /// \brief
  ///   Gets the factor [0..1] for compensating for the time-drift between the real world
  ///   clock and the filtered game clock, that can occur during the time step filtering. 
  inline float GetTimeDebtFactor() const { return m_fTimeDebtFactor; }

  /// \brief
  ///   Sets the factor [0..1] for compensating for the time-drift between the real world
  ///   clock and the filtered game clock, that can occur during the time step filtering. 
  inline void SetTimeDebtFactor(float fTimeDebtFactor) { m_fTimeDebtFactor = fTimeDebtFactor; }

  bool m_bDeleteObject;

protected:

  void FilterTimeStep();

  bool m_bFilterTimeStep;
  float m_fSmoothFactor,m_fTimeDebtFactor;
  float m_fLastTimeSteps[VDEFAULTTIMER_FILTER_FRAMECOUNT]; // time-steps of last VDEFAULTTIMER_FILTER_FRAMECOUNT frames
  float m_fTimeDebt;
  float m_fLastTimeDifference;
  float m_fUnfilteredTimeStep;
};



/// \brief
///   Implements a special purpose timer that returns a fixed time delta per update (e.g. 1/60 s) regardless of the actual time passed.
///
/// Typical use cases are applications that require deterministic simulation through a fixed simulation tick rate.
/// It makes sense to use this timer class together with class VFixStepSceneUpdateController on the application side.
class VFixStepTimer : public VDefaultTimer
{
public:
  /// \brief
  ///   Constructor. Sets the number of ticks per second. The number of ticks can be changed through SetSteps
  VISION_APIFUNC VFixStepTimer(int iTicksPerSecond=30, bool bDeleteObjectUponUnref=true);

  /// \brief
  ///   Destructor
  VISION_APIFUNC virtual ~VFixStepTimer() {}

  /// \brief
  ///   Set the number of simulation steps per second. The value returned by GetTimeDifference is thus always 1.0f/iTicksPerSecond
  VISION_APIFUNC void SetSteps(int iTicksPerSecond);

  VISION_APIFUNC virtual void Update() HKV_OVERRIDE;
  VISION_APIFUNC virtual void SerializeX( VArchive &ar ) HKV_OVERRIDE;

protected:
  double m_dTimeStep;
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
