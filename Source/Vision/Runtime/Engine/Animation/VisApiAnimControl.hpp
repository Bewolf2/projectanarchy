/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiAnimControl.hpp

#ifndef VIS_ANIM_CONTROL_HPP_INCLUDED
#define VIS_ANIM_CONTROL_HPP_INCLUDED

#include <Vision/Runtime/Engine/Animation/VisApiAnimResultGenerator.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiAnimSequence.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiAnimEventList.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiAnimEventListener.hpp>

// Flag definition for anim control
#define VANIMCTRL_LOOP                  0x00000001
#define VANIMCTRL_NO_SEQUENCE_EVENTS    0x00000002
#define VANIMCTRL_NO_INTERPOLATION      0x00000004


/// \brief
///   Base animation control class with common control attributes and methods
/// 
/// An animation control represents an instance of an animation being played. Several controls can
/// use the same animation data (sequence) and play it at a different time, speed and direction.
/// 
/// This parent class performs the time calculation, event handling and motion delta for all
/// animation types (subclasses).
/// 
/// The VisAnimControl_cl is virtual but includes all functionality that a standard control needs.
/// So you can derive classes from it to play every kind of animation you want. The base class will
/// compute the current sequence time, update the motion delta, send occurring messages to its
/// listeners, and call the Sample function that you have to overwrite in the subclass to get the
/// actual result.
/// 
/// However, you can also use the two predefined classes VisVertexAnimControl and
/// VisSkeletalAnimControl.
/// 
/// VisAnimControl_cl derives from VisAnimEventListener_cl. Use the event handler functionality 
/// to register your own classes as listeners for events that occur in the animation sequences event
/// list or that you have added to the controllers event list by hand.
class VisAnimControl_cl : public VisAnimEventListener_cl
{
public:

  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  /// \brief
  ///   Constructor of the anim control class
  /// 
  /// \param iFlags
  ///   Flags of the controller. The following flags are supported:
  ///   \li VANIMCTRL_LOOP : the animation is looped,
  /// 
  ///   \li VANIMCTRL_NO_SEQUENCE_EVENTS : no sequence events are triggered,
  /// 
  ///   \li VANIMCTRL_NO_INTERPOLATION : turns off frame interpolation
  VISION_APIFUNC VisAnimControl_cl(unsigned int iFlags);


  /// \brief
  ///   Destructor
  VISION_APIFUNC virtual ~VisAnimControl_cl();

  ///
  /// @}
  ///

  ///
  /// @name Playback
  /// @{
  ///

  /// \brief
  ///   Starts or restarts the playback of a paused animation
  /// 
  ///  Paused animations continue playing at the current position if the bReset flag is false.
  /// 
  /// Newly created controls start as paused animations.
  /// 
  /// \param bReset
  ///   true resets the current position to the start (resp. to the end when playing backwards)
  VISION_APIFUNC void Play(bool bReset = true);


  /// \brief
  ///   Continues playing a paused animation. Same as Play(false);
  /// 
  /// Newly created controls start as paused animations.
  inline void Resume() 
  {
    Play(false);
  }


  /// \brief
  ///   IsPlaying returns the current playback status of the animation.
  /// 
  /// \return
  ///   bool result: true if the control is playing the animation
  inline bool IsPlaying() const 
  {
    return !m_bIsPaused;
  }

  
  /// \brief
  ///   Pauses this animation
  /// 
  ///  On updates it will always return the last result that has been calculated before it was
  /// stopped.
  /// 
  /// Use Resume to continue playing.
  inline void Pause() 
  {
    m_bIsPaused = true;
  }

  ///
  /// @}
  ///

  ///
  /// @name Sequence Timing
  /// @{
  ///

  /// \brief
  ///   Sets the current time (in seconds) of the control within the animation sequence.
  /// 
  /// \param fCurrentSequenceTime
  ///   the new position in seconds. Must be in the interval [0..sequence length].
  VISION_APIFUNC void SetCurrentSequenceTime(float fCurrentSequenceTime);


  /// \brief
  ///   Similar to SetCurrentSequenceTime but uses the interval [0..1] rather than time value
  /// 
  /// Sets the current position between start(0.f) and end(1.f).
  /// 
  /// The value has to be in the interval [0..1] regardless of how long the real animation is.
  /// 
  /// This helps to position two sequences on the same relative position e.g. 0.5 => half of
  /// sequence.
  /// 
  /// \param fCurrentSequencePosition
  ///   the new sequence position between 0 and 1.
  VISION_APIFUNC void SetCurrentSequencePosition(float fCurrentSequencePosition);


  /// \brief
  ///   Returns the current time in seconds of this control within the animation sequence.
  /// 
  /// \return
  ///   float fCurrentSequenceTime: the current sequencetime in seconds
  inline float GetCurrentSequenceTime() const
  {
    return m_fCurrentSequenceTime;
  }


  /// \brief
  ///   Returns the current relative position [0..1] of this control within the animation sequence.
  /// 
  /// \return
  ///   float fCurrentSequenceTime: the current sequence position (always in interval [0..1])
  inline float GetCurrentSequencePosition() const
  { 
    VASSERT(m_spAnimSequence);
    float fLength=m_spAnimSequence->GetLength(); 
    if (fLength<=0)
        return 0.f; 
      else 
        return (m_fCurrentSequenceTime/fLength);
  }


  /// \brief
  ///   Sets the current speed factor for playing back the animation sequence.
  /// 
  /// You can speed up, slow down or even reverse the playback.
  /// 
  /// A factor of 1.0f plays the animation at normal speed, a factor of -2.f plays the animation
  /// backwards at twice the normal speed.
  /// 
  /// The speed can be changed at any time while playing the animation.
  /// 
  /// \param fSpeedFactor
  ///   the playback speed factor
  inline void SetSpeed(float fSpeedFactor)
  {
    m_fSpeedFactor = fSpeedFactor;
  }
  

  /// \brief
  ///   Returns the current speed factor.
  /// 
  /// The default speed is 1.0f and can be changed at any time via SetSpeed.
  /// 
  /// \return
  ///   float fSpeedFactor: the playback speed
  /// 
  /// \sa SetSpeed
  inline float GetSpeed(void) const 
  {
    return m_fSpeedFactor;
  }

  ///
  /// @}
  ///

  ///
  /// @name Sequence
  /// @{
  ///

  /// \brief
  ///   Sets a new animation resource on this control.
  /// 
  /// The current sequence time is clamped to the new limits.
  /// 
  /// Furthermore it calls the virtual OnSetAnimSequence function.
  /// 
  /// \param pAnimSequence
  ///   the animation sequence to playback by the control
  VISION_APIFUNC virtual void SetAnimSequence(const VisAnimSequence_cl* pAnimSequence);


  /// \brief
  ///   virtual overridable that is called when a new sequence is set on this control.
  /// 
  /// Implement this function to perform some extra initialization when the sequence changes.
  /// 
  /// The default implementation does not do anything, but to avoid future compatibility problems,
  /// also call the base function.
  /// 
  /// \param pAnimSequence
  ///   the animation sequence that has been passed to SetAnimSequence
  VISION_APIFUNC virtual void OnSetAnimSequence(const VisAnimSequence_cl* pAnimSequence) const 
  {
  }


  /// \brief
  ///   GetAnimSequence returns the attached animation sequence.
  /// 
  /// \return
  ///   VisAnimSequence_cl* pAnimSequence: sequence played by the control
  inline const VisAnimSequence_cl* GetAnimSequence() const
  {
    return m_spAnimSequence;
  }


  /// \brief
  ///   Sets new control flags
  /// 
  /// \param iFlags
  ///   control flags. Can be a combination of the following constants
  ///   \li VANIMCTRL_LOOP : the animation is looped,
  /// 
  ///   \li VANIMCTRL_NO_SEQUENCE_EVENTS : no sequence events are triggered
  inline void SetFlags(unsigned int iFlags)
  {
    m_iFlags = iFlags;
  }


  /// \brief
  ///   Returns the currently set control flags.
  /// 
  /// The flags can be changed via SetFlags.
  /// 
  /// \return
  ///   unsigned int: control flags. Can be a combination of the following constants:
  ///   \li VANIMCTRL_LOOP : the animation is looped,
  /// 
  ///   \li VANIMCTRL_NO_SEQUENCE_EVENTS : no sequence events are triggered
  inline unsigned int GetFlags() const
  {
    return m_iFlags;
  }
  
  ///
  /// @}
  ///

  ///
  /// @name Events
  /// @{
  ///

  /// \brief
  ///   Returns the local event list of the control.
  /// 
  /// The returned instance can be used to add custom events to the control.
  /// 
  /// \return
  ///   VisAnimEventList_cl* m_pEventList: event list of the control.
  /// 
  /// \note
  ///   The control checks two event lists for events. Its own event list and the event list of the
  ///   animation sequence it's playing.
  /// 
  /// \note
  ///   The difference is that the sequence event list is shared by all controls that playback that
  ///   sequence.
  /// 
  /// \sa VisAnimEventList_cl;
  inline VisAnimEventList_cl* GetEventList(void)
  {
    return &m_EventList;
  }

  ///
  /// @}
  ///

  ///
  /// @name Update
  /// @{
  ///

  /// \brief
  ///   Update the current control state.
  /// 
  /// The current sequence time and motion deltas are calculated and events are sent.
  /// 
  /// \param fTimeDelta
  ///   time past since last state update.
  /// 
  /// \return
  ///   VisAnimState_cl* pState: pointer to updated state
  VISION_APIFUNC VisAnimState_cl* UpdateControlState(float fTimeDelta);


  /// \brief
  ///   Pure virtual function that has to be overwritten to implement the actual update
  ///   functionality.
  /// 
  /// This function gets called inside UpdateControlState.
  /// 
  /// \param fTimeDelta
  ///   time past since last state update. Same as in UpdateControlState
  /// 
  /// \return
  ///   bool bResult: return true if update was successful
  VISION_APIFUNC virtual bool OnUpdateControlState(float fTimeDelta) = 0;


protected:

  /// \brief
  ///   Calculates the current sequence time internally.
  /// 
  /// Used by UpdateAnimState and GetLocalSpaceResult
  /// 
  /// \param fTimeDelta
  ///   time past since last state update.
  /// 
  /// \return
  ///   float fRelTime: new relative time
  VISION_APIFUNC float CalculateCurrentSequenceTime(float fTimeDelta);
      

  /// \brief
  ///   Pure virtual function that is used to calculate the current frame data.
  /// 
  /// Since every control type processes different kinds of data the method  has to be overwritten
  /// to process the data(e.g. Bones, vertices) correctly.
  /// 
  /// \param pDestResult
  ///   gets filled with new result
  /// 
  /// \param fSequenceTime
  ///   current time
  VISION_APIFUNC virtual void SampleAnimation(VisAnimResult_cl* pDestResult, float fSequenceTime) = 0;


  /// \brief
  ///   UpdateEvents triggers the events inside a time interval.
  /// 
  /// No events are triggered in pause mode or if the VANIMCTRL_NO_SEQUENCE_EVENTS flag is
  /// specified for this control.
  /// 
  /// This function handles sequence length wrap-arounds correctly
  /// 
  /// \param fLastSequenceTime
  ///   time up to the events have been processed last frame
  /// 
  /// \param fNewSequenceTime
  ///   time up to the events have to be processed now
  VISION_APIFUNC void UpdateEvents(float fLastSequenceTime, float fNewSequenceTime);


  /// \brief
  ///   UpdateMotionDeltas performs the internal motion delta calculation.
  /// 
  /// It Calculates the current motion deltas that have to be applied to the character since the
  /// last call. Also takes into account if the sequence has been updated more than just once since
  /// last frame.
  /// 
  /// \param fLastSequenceTime
  ///   time up to the motion delta has been processed last frame
  /// 
  /// \param fNewSequenceTime
  ///   time up to the motion delta has to be processed now
  VISION_APIFUNC void UpdateMotionDeltas(float fLastSequenceTime, float fNewSequenceTime);

  
  /// \brief
  ///   UpdateVisibilityBoundingBox generates the visibility bounding box from the sequence at
  ///   specified time position.
  /// 
  /// The visibility bounding box always contains the animated character from the past/current/next
  /// key frame completely.
  /// 
  /// \param fCurrentSequenceTime
  ///   current sequence time
  VISION_APIFUNC void UpdateVisibilityBoundingBox(float fCurrentSequenceTime);

  
  VisAnimSequencePtr m_spAnimSequence;            ///< reference counted pointer to animation data
  float m_fCurrentSequenceTime; 
  bool m_bIsPaused;                               ///< stops animation on current time
  bool m_bIsDirty;                                ///< forces animation to resample
  unsigned int  m_iFlags;                         ///< current flags set for the control

  // time values
  float m_fSpeedFactor;
  float m_fStartAnimTime;

  // event handler stuff
  VisAnimEventList_cl m_EventList;
  
  int m_iSynchronizedControlsCount;
  DynArray_cl<VisAnimControl_cl*>m_pSynchronizedControls;  ///< list of controls that should work synchronized

  VisAnimState_cl* m_pAnimState;                ///< the state is the same for all kinds of controls
  
  // Must be serializeX here since classes derived from IVisAnimResultGenerator_cl use Serialize
  V_DECLARE_SERIALX( VisAnimControl_cl, VISION_APIFUNC );
  VISION_APIFUNC void SerializeX( VArchive &ar );
  VISION_APIFUNC VisAnimControl_cl();
  void CommonInit();
  ///
  /// @}
  ///

};


#endif //VIS_ANIM_CONTROL_HPP_INCLUDED

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
