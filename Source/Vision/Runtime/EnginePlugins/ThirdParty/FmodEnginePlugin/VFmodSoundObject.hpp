/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VFmodSoundObject.hpp

#ifndef VFMODSOUNDOBJECT_HPP_INCLUDED
#define VFMODSOUNDOBJECT_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodIncludes.hpp>

/// Serialization versions
#define VFMOD_SOUNDOBJECT_VERSION_0         0x00000000                  // Initial version
#define VFMOD_SOUNDOBJECT_VERSION_1         0x00000001
#define VFMOD_SOUNDOBJECT_VERSION_CURRENT   VFMOD_SOUNDOBJECT_VERSION_1

///\brief
///  This class represents a single positionable sound instance in the world.
///
/// This is the native counterpart of the vForge sound shape.
/// Multiple sound objects can share the same sound resource (wave file).
///
/// To position and orient the sound object in the scene, use the SetPosition/SetOrientation
/// member functions which are inherited from the VisObject3D_cl class.
///
///\see
///  VFmodSoundResource
class VFmodSoundObject : public VisObject3D_cl
{
public:

  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  FMOD_IMPEXP VFmodSoundObject();

  FMOD_IMPEXP VFmodSoundObject(VFmodSoundObjectCollection *pOwner, VFmodSoundResource *pResource, const hkvVec3 &vPos, int iFlags=VFMOD_FLAG_DEFAULT, int iPriority=128);

  FMOD_IMPEXP virtual ~VFmodSoundObject();

  /// \brief
  ///   Stops the sound and removes it from the collection. This function should be called to destroy a sound.
  FMOD_IMPEXP VOVERRIDE void DisposeObject();

  ///
  /// @}
  ///


  ///
  /// @name Sound property functions
  /// @{
  ///

  /// \brief
  ///   Gets the sound resource used by this instance
  inline VFmodSoundResource* GetResource() const { return m_spResource; }

  /// \brief
  ///   (Re-)plays the sound starting at fStartTime seconds
  ///
  /// \param fStartTime
  ///   Time offset in seconds to start playback
  ///
  /// \param bAlsoInEditor
  ///   If true, then playback will also be started inside vForge
  ///
  /// \remarks
  ///   A sound object by default starts playing after creation unless 
  ///   it has been created with the VFMOD_FLAG_PAUSED flag.
  FMOD_IMPEXP void Play(float fStartTime = 0.0f, bool bAlsoInEditor=false);  
  
  /// \brief
  ///   Stops the sound
  FMOD_IMPEXP void Stop();

  /// \brief
  ///   Indicates whether the sound is currently playing
  FMOD_IMPEXP bool IsPlaying() const;

  /// \brief
  ///   Indicates whether the sound has finished loading
  FMOD_IMPEXP bool IsReady() const;

  ///\brief
  /// Returns the current play position inside the sound (in seconds)
  ///
  /// \param  bAbsolute  
  ///   Returned seconds to be absolute or relative to start time.
  FMOD_IMPEXP float GetCurrentPosition(bool bAbsolute=false) const;

  /// \brief
  ///   Returns the pause status of this sound instance
  inline bool IsPaused() const {return (m_iFlags&VFMOD_FLAG_PAUSED)>0;}

  /// \brief
  ///   Pauses/ continues the sound
  FMOD_IMPEXP void SetPaused(bool bStatus);

  /// \brief
  ///   Returns the mute status of this sound instance
  inline bool IsMuted() const {return (m_iFlags&VFMOD_FLAG_MUTE)>0;}

  /// \brief
  ///   Mutes/ un-mutes the sound
  FMOD_IMPEXP void SetMuted(bool bStatus);

  /// \brief
  ///   Returns the volume of this sound instance
  inline float GetVolume() const {return m_fVolume;}

  /// \brief
  ///   Sets the sound's volume (range [0..1])
  FMOD_IMPEXP void SetVolume(float fVol);

  /// \brief
  ///   Returns the stereo panning for 2D sounds 
  inline float GetPan() const { return m_fPan; }

  /// \brief
  ///   Sets the stereo panning for 2D sounds
  FMOD_IMPEXP void SetPan(float fPan);

  /// \brief
  ///   Returns the fade distance at which the 3D sound has full volume
  inline float Get3DFadeMinDistance() const {return m_fFadeMin;}

  /// \brief
  ///   Returns the fade distance at which the 3D sound has faded out
  inline float Get3DFadeMaxDistance() const {return m_fFadeMax;}

  /// \brief
  ///   Sets distance attenuation parameters for 3D sounds
  FMOD_IMPEXP void Set3DFadeDistance(float fMin, float fMax);

  /// \brief
  ///   Returns true if sound object is currently in fading
  inline bool IsFading() const { return m_bFading; }

  /// \brief
  ///   Returns true if sound object is currently fading in
  inline bool IsFadingIn() const { return m_bFading && m_fVolumeFadeTarget > 0.0f; }

  /// \brief
  ///   Returns true if sound object is currently fading out
  inline bool IsFadingOut() const { return m_bFading && m_fVolumeFadeStart > 0.0f; }

  /// \brief  
  ///   Changes the volume over time to the new given value
  ///
  /// \param fNewVolume 
  ///    The new volume
  ///
  /// \param fDuration 
  ///   The duration of the fade process
  FMOD_IMPEXP void FadeTo(float fNewVolume, float fDuration);

  /// \brief  
  ///   Fades the sound objects volume out over the given duration 
  ///
  /// \param fDuration 
  ///   The duration of the fading process
  FMOD_IMPEXP void FadeOut(float fDuration);

  /// \brief  
  ///   Fades the sound objects volume in over the given duration
  ///
  /// \param fDuration 
  ///   The duration of the fading process
  FMOD_IMPEXP void FadeIn(float fDuration);

  /// \brief  
  ///   Fades the sound objects volume from a given value to a new value 
  ///
  /// \param  
  ///   fStartVolume  The start volume. 
  /// \param  
  ///   fTargetVolume Target volume. 
  /// \param  
  ///   fDuration The duration of the fading process. 
  FMOD_IMPEXP void FadeFromTo(float fStartVolume, float fTargetVolume, float fDuration);

  /// \brief
  ///   Gets the pitch
  inline float GetPitch() const  { return m_fPitch; }

  /// \brief  
  ///   Sets the pitch
  ///
  /// \param fPitch 
  ///   The pitch
  /// 
  /// 1.0 = standard pitch, the sound plays with default speed. 0.5 = half speed. 2.0 = double the normal speed. 
  /// (Note that on the PS3 the maximum is 2.0 and all values above will be clamped!)
  /// This can be used to scale sounds with respect to slow motion etc.
  ///
  FMOD_IMPEXP void SetPitch(float fPitch);

  /// \brief
  ///   Returns the inside cone angle for directional sounds
  inline float GetConeAngleInside() const {return m_fConeInside; }

  /// \brief
  ///   Returns the outside cone angle for directional sounds
  inline float GetConeAngleOutside() const {return m_fConeOutside; }
  
  /// \brief
  ///   Sets cone angles for directional sounds
  ///
  /// \param fInside
  ///   The inside angle in degree
  ///
  /// \param fOutside
  ///   The outside angle in degree
  ///
  /// The volume outside fOutside degrees is 0% and 100% inside
  /// the fInside angle. Set (-1,-1) for omni-directional sound (default)
  FMOD_IMPEXP void SetConeAngles(float fInside, float fOutside);

  /// \brief
  ///   Returns the looped status of this sound instance
  inline bool IsLooped() const {return (m_iFlags&VFMOD_FLAG_LOOPED)>0;}

  /// \brief
  ///   Returns if this sound object is used as music playback (this has consequences on the XBox 360 for example)
  inline bool IsMusic() const {return (m_iFlags&VFMOD_FLAG_MUSIC)>0;}

  /// \brief
  ///   Returns true if this sound object is disposed automatically by Fmod manager when stopped
  ///   or finished playing. Returns false if sound object needs to be disposed via DisposeObject().
  inline bool IsAutoDisposed() const {return (m_iFlags&VFMOD_FLAG_NODISPOSE)==0;}

  /// \brief
  ///   Returns if this sound object has a specific flag set or not
  inline bool HasFlag(int iFlag) const {return (m_iFlags&iFlag)>0;}

  /// \brief
  ///   Return flags
  inline int GetFlags() const { return m_iFlags; }

  /// \brief
  ///   Get start time the sound was started playing at
  inline float GetStartTime() const  { return m_fStartTime; }

  /// \brief
  ///   Returns the channel priority
  inline int GetPriority() const { return m_iPriority; }

  /// \brief
  ///   Calculates volume attenuation based on current sound position, listener and min/max fading
  FMOD_IMPEXP float CalculateAttenuatedVolume(const hkvVec3& vListenerPos) const;

  /// \brief
  ///   Returns the internal channel index in the sound engine
  FMOD_IMPEXP int GetChannelIndex() const;

  /// \brief
  ///   Getter function for direct channel access to be able to apply effects directly
  inline FMOD::Channel* GetFmodChannel() const { return m_pChannel; }

  /// \brief
  ///   Returns whether the sound object has a valid Fmod channel for playback
  inline bool IsValid() const { return m_pChannel!=NULL; }

  ///
  /// @}
  ///


  ///
  /// @name Overridden functions
  /// @{
  ///

  // overridden VisObject3D_cl notification. Used to update sound's position/orientation
  FMOD_IMPEXP VOVERRIDE void OnObject3DChanged(int iO3DFlags);

  // triggered when sound object is destroyed
  FMOD_IMPEXP VOVERRIDE void OnDisposeObject();

  // overridden VisObject3D_cl notification. Handle incoming messages, e.g. triggers
  FMOD_IMPEXP VOVERRIDE void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB);

#ifdef SUPPORTS_SNAPSHOT_CREATION
  FMOD_IMPEXP VOVERRIDE void GetDependencies(VResourceSnapshot &snapshot);
#endif

  ///
  /// @}
  ///


  ///
  /// @name Serialization
  /// @{
  ///
 
  V_DECLARE_SERIAL_DLLEXP( VFmodSoundObject, FMOD_IMPEXP );

  FMOD_IMPEXP VOVERRIDE void Serialize( VArchive &ar );

  /// \brief
  ///   Overridden function to finalize sound object once the deserialization is finished
  FMOD_IMPEXP VOVERRIDE void OnDeserializationCallback(const VSerializationContext &context);

  /// \brief
  ///   Overridden function to indicate that we need a deserialization callback
  FMOD_IMPEXP VOVERRIDE VBool WantsDeserializationCallback(const VSerializationContext &context) {return context.m_eType!=VSerializationContext::VSERIALIZATION_EDITOR;}

  ///
  /// @}
  ///


  ///
  /// @name Misc
  /// @{
  ///

  /// \brief  
  ///   Helper function to update the fading process - this is used internally by the Fmod plugin.
  ///
  /// \param bApplyVol
  ///   Set to true if calculated volume should be applied otherwise it is only returned by function.
  ///
  /// \return
  ///   The new volume
  FMOD_IMPEXP float UpdateFading(bool bApplyVol=true);

  ///\brief
  /// Helper function to render the object in the scene. Used by vForge
  FMOD_IMPEXP void DebugRender(IVRenderInterface* pRI);

  /// \brief  
  ///   Helper function to set/remove flags
  ///
  /// \param iMask
  ///   Bit mask
  ///
  /// \param bStatus
  ///   Set or remove flag
  void inline Helper_SetFlag(int iMask, bool bStatus) {if (bStatus) m_iFlags|=iMask; else m_iFlags &= (~iMask);}

  ///
  /// @}
  ///


  ///
  /// @name Internal functions
  /// @{
  ///

  // Update the sound object
  void Update();

  ///
  /// @}
  ///

private:  
  friend class VFmodManager;
  friend class VFmodSoundObjectCollection;

  VFmodSoundObjectCollection *m_pOwner;
  VFmodSoundResourcePtr m_spResource;

  int m_iFlags;
  int m_iPriority;
  float m_fVolume;
  float m_fPan;
  float m_fConeOutside,m_fConeInside;
  float m_fFadeMin,m_fFadeMax;
  float m_fStartTime;
  float m_fVolumeFadeTime,m_fVolumeFadeDuration;
  float m_fVolumeFadeTarget,m_fVolumeFadeStart;
  bool m_bFading;
  float m_fPitch;

  FMOD::Channel *m_pChannel;
  float m_fInitialFreq;
  bool m_bIsPlaying;
  bool m_bPlayedOnce;
  bool m_bUnpause;
};


/// \brief
///   A collection of sound instances where each one is ref-counted. Also has some specific sound functions such as StopAll().
class VFmodSoundObjectCollection : public VRefCountedCollection<VFmodSoundObject>
{
public:
  /// \brief
  ///   Internal function for debug output on screen
  FMOD_IMPEXP void DebugOutput();

  /// \brief
  ///   Internal function for updating all sound objects in this collection
  FMOD_IMPEXP void Update();

  /// \brief
  ///   Removes all sound objects from this collection that can be disposed (i.e. not playing anymore and VFMOD_FLAG_NODISPOSE flag not set)
  FMOD_IMPEXP void PurgeNotPlaying();

  /// \brief
  ///   Calls the Stop function on all instances and purges all sound objects from this collection that can be disposed (i.e. not playing anymore and VFMOD_FLAG_NODISPOSE flag not set)
  FMOD_IMPEXP void StopAll(bool bReasonIsUnloadWorld = false);

  /// \brief
  ///   Returns the first occurrence (or NULL) of a sound object in this collection with specified object key. The object key can be set in vForge.
  FMOD_IMPEXP VFmodSoundObject* SearchObject(const char* szName) const;
};


/// \brief
///   Sound callback data object
class VFmodSoundDataObject : public IVisCallbackDataObject_cl
{
public:
  /// \brief
  ///   Constructor
  VFmodSoundDataObject(VisCallback_cl *pSender, VFmodSoundObject* pObj, FMOD_CHANNEL_CALLBACKTYPE eType) 
    : IVisCallbackDataObject_cl(pSender), m_pSoundObject(pObj), m_eType(eType)
  {
  }

  // Sound object
  VFmodSoundObject* m_pSoundObject;

  // Fmod channel callback type
  FMOD_CHANNEL_CALLBACKTYPE m_eType;

  // String param, when it's used. Empty otherwise. 
  // This param is used for FMOD_CHANNEL_CALLBACKTYPE_SYNCPOINT events, where it represents the name of the sync point.
  VStaticString<256> m_sParam;
};

#endif // VFMODSOUNDOBJECT_HPP_INCLUDED

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
