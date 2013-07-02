/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiTextureAnim.hpp

#ifndef DEFINE_VISAPITEXTUREANIM
#define DEFINE_VISAPITEXTUREANIM

class VTextureObject;
typedef VSmartPtr<VTextureObject>  VTextureObjectPtr;


/// \brief
///   Flags used for creating an animated texture via
///   VisTextureAnimInstance_cl::CreateNewTextureAnim
#define VIS_TEXTUREANIMFLAG_NONE            0x00000000
#define VIS_TEXTUREANIMFLAG_HALT            0x00000002

class VisTextureAnimInstance_cl;
typedef VSmartPtr<VisTextureAnimInstance_cl> VisTextureAnimInstancePtr;


/// \brief
///   Class that represents a single texture animation instance.
/// 
/// A texture animation instance defines the current animation state (e.g. phase number) of an
/// animated texture.
/// 
/// Different parts of geometry can share the same animated texture and trigger it independently by
/// using distinct animation instances.
class VisTextureAnimInstance_cl : public VRefCounter , public VisElementManager_cl<class VisTextureAnimInstance_cl *>
{
public:






  ///
  /// @name Get Animation Type
  /// @{
  ///


  /// \brief
  ///   Gets the animation type of this animation instance
  /// 
  /// \return
  ///   AnimType_e eAnimType : The animation type, e.g. ANIMTYPE_PERM_PINGPONG
  /// 
  /// \sa AnimType_e
  /// \sa VisTextureAnimInstance_cl::GetAnimationTiming
  inline AnimType_e GetAnimationType() const
  {
    VASSERT(m_pFirstFrame && m_pFirstFrame->m_pAnimInfo);
    return (AnimType_e)m_pFirstFrame->m_pAnimInfo->animtype;
  }

  /// \brief
  ///   Sets the animation type of this animation instance
  inline void SetAnimationType(AnimType_e eNewType)
  {
    VASSERT(m_pFirstFrame && m_pFirstFrame->m_pAnimInfo);
    m_pFirstFrame->m_pAnimInfo->animtype = eNewType;
  }

  /// \brief
  ///   Returns the texture that this animation is based on. The file extension of this texture is typically .texanim
  inline VTextureObject* GetSourceTexture() const
  {
    return m_pFirstFrame;
  }

  /// \brief
  ///   Gets the time in seconds between two frames
  /// 
  /// \return
  ///   float fTime : The time difference between two frames
  /// 
  /// \sa VisTextureAnimInstance_cl::GetAnimationType
  inline float GetAnimationTiming() const
  {
    VASSERT(m_pFirstFrame && m_pFirstFrame->m_pAnimInfo);
    return 1.f/m_pFirstFrame->m_pAnimInfo->speed;
  }



  ///
  /// @}
  ///

  ///
  /// @name Get Texture Frames / Phases
  /// @{
  ///


  /// \brief
  ///   Returns the number of animation frames in this animation
  /// 
  /// \return
  ///   int iCount : The number of animation frames in this animation
  /// 
  /// \sa VisTextureAnimInstance_cl::GetFrameByIndex
  /// \sa VisTextureAnimInstance_cl::SetAnimPhase
  /// \sa VisTextureAnimInstance_cl::IncAnimPhase
  /// \sa VisTextureAnimInstance_cl::GetAnimationTiming
  /// \sa VisTextureAnimInstance_cl::GetAnimationType
  inline int GetNumberOfFrames() const
  {
    VASSERT(m_pFirstFrame && m_pFirstFrame->m_pAnimInfo);
    return m_pFirstFrame->m_pAnimInfo->numFrames;
  }

  /// \brief
  ///   Gets the n-th animation frame texture object in this animation
  /// 
  /// \param iIndex
  ///   The 0-based index. The index must be in valid range [0..GetNumberOfFrames()-1]
  /// 
  /// \return
  ///   VTextureObject *pTexture : Texture object of the n-th texture frame
  /// 
  /// \sa VisTextureAnimInstance_cl::GetNumberOfFrames
  /// \sa VisTextureAnimInstance_cl::SetAnimPhase
  /// \sa VisTextureAnimInstance_cl::IncAnimPhase
  /// \sa VisTextureAnimInstance_cl::GetAnimationTiming
  /// \sa VisTextureAnimInstance_cl::GetAnimationType
  inline VTextureObject *GetFrameByIndex(int iIndex) const
  {
    VASSERT(m_pFirstFrame && m_pFirstFrame->m_pAnimInfo);
    return m_pFirstFrame->GetTextureAnimFrame(iIndex);
  }


  /// \brief
  ///   Gets the current animation frame texture of this animation instance
  /// 
  /// \return
  ///   VTextureObject *pTexture : Texture object of the current texture frame
  /// 
  /// \sa VisTextureAnimInstance_cl::GetNextFrame
  /// \sa VisTextureAnimInstance_cl::GetCurrentPhase
  /// \sa VisTextureAnimInstance_cl::SetAnimPhase
  /// \sa VisTextureAnimInstance_cl::IncAnimPhase
  /// \sa VisTextureAnimInstance_cl::GetNumberOfFrames
  /// \sa VisTextureAnimInstance_cl::GetFrameByIndex
  /// \sa VisTextureAnimInstance_cl::GetAnimationTiming
  /// \sa VisTextureAnimInstance_cl::GetAnimationType
  inline VTextureObject *GetCurrentFrame() const
  {
    VASSERT(m_pFirstFrame && m_pFirstFrame->m_pAnimInfo);
    return m_pFirstFrame->GetTextureAnimFrame(m_iCurrentFrame);
  }

  /// \brief
  ///   Gets the frame texture of the next animation frame
  /// 
  /// The next frame can be used to smoothly morph between two textures in a shader.
  /// 
  /// The weighting between the current and the next frame can be retrieved with the
  /// GetFrameWeighting function.
  /// 
  /// \return
  ///   VTextureObject *pTexture : Texture object of the next texture frame
  /// 
  /// \sa VisTextureAnimInstance_cl::GetCurrentFrame
  /// \sa VisTextureAnimInstance_cl::GetCurrentPhase
  /// \sa VisTextureAnimInstance_cl::SetAnimPhase
  /// \sa VisTextureAnimInstance_cl::IncAnimPhase
  /// \sa VisTextureAnimInstance_cl::GetNumberOfFrames
  /// \sa VisTextureAnimInstance_cl::GetFrameByIndex
  /// \sa VisTextureAnimInstance_cl::GetAnimationTiming
  /// \sa VisTextureAnimInstance_cl::GetAnimationType
  inline VTextureObject *GetNextFrame() const
  {
    VASSERT(m_pFirstFrame && m_pFirstFrame->m_pAnimInfo);
    return m_pFirstFrame->GetTextureAnimFrame(m_iNextFrame);
  }


  /// \brief
  ///   Gets the current float based animation phase
  /// 
  /// The current frame number is defined by the integer part of this value.
  /// 
  /// The phase weighting is defined by the fractional part of this value.
  /// 
  /// \return
  ///   float fPhase : The current animation phase
  /// 
  /// \sa VisTextureAnimInstance_cl::SetAnimPhase
  /// \sa VisTextureAnimInstance_cl::IncAnimPhase
  /// \sa VisTextureAnimInstance_cl::GetCurrentFrame
  /// \sa VisTextureAnimInstance_cl::GetNextFrame
  /// \sa VisTextureAnimInstance_cl::GetNumberOfFrames
  /// \sa VisTextureAnimInstance_cl::GetFrameByIndex
  /// \sa VisTextureAnimInstance_cl::GetAnimationTiming
  /// \sa VisTextureAnimInstance_cl::GetAnimationType
  VISION_APIFUNC inline float GetCurrentPhase() const {return m_fCurrentTime;}

  /// \brief
  ///   Returns the weighting factor for blending between the current frame and the next frame
  /// 
  /// For smoothly morphing the animation, the formula
  /// currentTexture*(1-weighting)+nextTexture*weighting can be used.
  /// 
  /// \return
  ///   float fWeighting : The weighting value [0..1] between two textures
  /// 
  /// \sa VisTextureAnimInstance_cl::GetFrameWeighting255
  /// \sa VisTextureAnimInstance_cl::GetCurrentPhase
  /// \sa VisTextureAnimInstance_cl::GetCurrentFrame
  /// \sa VisTextureAnimInstance_cl::SetAnimPhase
  /// \sa VisTextureAnimInstance_cl::IncAnimPhase
  /// \sa VisTextureAnimInstance_cl::GetNextFrame
  /// \sa VisTextureAnimInstance_cl::GetNumberOfFrames
  /// \sa VisTextureAnimInstance_cl::GetFrameByIndex
  /// \sa VisTextureAnimInstance_cl::GetAnimationTiming
  /// \sa VisTextureAnimInstance_cl::GetAnimationType
  VISION_APIFUNC inline float GetFrameWeighting() const {return (float)m_iWeighting*(1.f/255.f);}

  /// \brief
  ///   Returns the weighting factor for blending between the current frame and the next frame
  /// 
  /// Same as GetFrameWeighting but returns an integer based weighting [0..255].
  /// 
  /// \return
  ///   float iWeighting : The weighting value [0..255] between two textures
  /// 
  /// \sa VisTextureAnimInstance_cl::GetFrameWeighting
  /// \sa VisTextureAnimInstance_cl::GetCurrentPhase
  /// \sa VisTextureAnimInstance_cl::GetCurrentFrame
  /// \sa VisTextureAnimInstance_cl::GetNextFrame
  /// \sa VisTextureAnimInstance_cl::SetAnimPhase
  /// \sa VisTextureAnimInstance_cl::IncAnimPhase
  /// \sa VisTextureAnimInstance_cl::GetNumberOfFrames
  /// \sa VisTextureAnimInstance_cl::GetFrameByIndex
  /// \sa VisTextureAnimInstance_cl::GetAnimationTiming
  /// \sa VisTextureAnimInstance_cl::GetAnimationType
  VISION_APIFUNC inline UBYTE GetFrameWeighting255() const {return m_iWeighting;}

  ///
  /// @}
  ///

  ///
  /// @name Trigger /set Animation Phase
  /// @{
  ///

  /// \brief
  ///   Gets the initial trigger status of this animation instance.
  /// 
  /// \return
  ///   BOOL bTriggerStatus : TRUE, if the texture animation instance is triggered at startup
  inline BOOL GetStartTriggerStatus() const
  {
    VASSERT(m_pFirstFrame && m_pFirstFrame->m_pAnimInfo);
    return m_pFirstFrame->m_pAnimInfo->start_trigger;
  }


  /// \brief
  ///   Resets the animation, i.e. resets to the first animation frame, set the initial trigger
  ///   status
  /// 
  /// \sa VisTextureAnimInstance_cl::SetTriggerStatus
  /// \sa VisTextureAnimInstance_cl::GetCurrentPhase
  /// \sa VisTextureAnimInstance_cl::GetCurrentFrame
  /// \sa VisTextureAnimInstance_cl::GetFrameWeighting
  /// \sa VisTextureAnimInstance_cl::GetFrameWeighting255
  /// \sa VisTextureAnimInstance_cl::GetNextFrame
  /// \sa VisTextureAnimInstance_cl::GetNumberOfFrames
  /// \sa VisTextureAnimInstance_cl::GetFrameByIndex
  /// \sa VisTextureAnimInstance_cl::GetAnimationTiming
  /// \sa VisTextureAnimInstance_cl::GetAnimationType
  VISION_APIFUNC void Reset();

  /// \brief
  ///   Triggers this animation instance.
  /// 
  /// If the new status is set to FALSE the animation is paused, otherwise it will animate with
  /// defined speed and pattern.
  /// 
  /// By default a texture animation is triggered. If the animation pattern is "single rotation"
  /// for instance, it will be halted after the rotation cycle has finished.
  /// 
  /// \param bStatus
  ///   the trigger status. FALSE for pausing the animation, TRUE for continue playing.
  /// 
  /// \sa VisTextureAnimInstance_cl::Reset
  /// \sa VisTextureAnimInstance_cl::GetCurrentPhase
  /// \sa VisTextureAnimInstance_cl::GetCurrentFrame
  /// \sa VisTextureAnimInstance_cl::SetAnimPhase
  /// \sa VisTextureAnimInstance_cl::IncAnimPhase
  /// \sa VisTextureAnimInstance_cl::GetFrameWeighting
  /// \sa VisTextureAnimInstance_cl::GetFrameWeighting255
  /// \sa VisTextureAnimInstance_cl::GetNextFrame
  /// \sa VisTextureAnimInstance_cl::GetNumberOfFrames
  /// \sa VisTextureAnimInstance_cl::GetFrameByIndex
  /// \sa VisTextureAnimInstance_cl::GetAnimationTiming
  /// \sa VisTextureAnimInstance_cl::GetAnimationType
  inline void SetTriggerStatus(BOOL bStatus)
  {
    m_iTrigger = bStatus;
  }

  /// \brief
  ///   Returns the current trigger status of this animation instance.
  /// 
  /// The status is either FALSE (=halted) or TRUE (=active)
  /// 
  /// \return
  ///   BOOL bStatus : The current trigger status
  inline BOOL GetTriggerStatus() const
  {
    return m_iTrigger>0;
  }

  /// \brief
  ///   Sets a new animation phase.
  /// 
  /// The fPhase value specifies the new animation phase number and must be in a valid range
  /// [0..numberOfFrames]
  /// 
  /// The fractional part defines the weighting factor between two frames.
  /// 
  /// \param fPhase
  ///   The new animation phase
  /// 
  /// \sa VisTextureAnimInstance_cl::GetCurrentPhase
  /// \sa VisTextureAnimInstance_cl::GetCurrentFrame
  /// \sa VisTextureAnimInstance_cl::GetFrameWeighting
  /// \sa VisTextureAnimInstance_cl::GetFrameWeighting255
  /// \sa VisTextureAnimInstance_cl::GetNextFrame
  /// \sa VisTextureAnimInstance_cl::GetNumberOfFrames
  /// \sa VisTextureAnimInstance_cl::GetFrameByIndex
  /// \sa VisTextureAnimInstance_cl::GetAnimationTiming
  /// \sa VisTextureAnimInstance_cl::GetAnimationType
  VISION_APIFUNC void SetAnimPhase(float fPhase);

  /// \brief
  ///   Increases the animation phase by the specified phase delta
  /// 
  /// This is done automatically by the engine once per tick.
  /// 
  /// \param fPhaseDelta
  ///   The delta value to increase the animation phase. A value of 1.f increases by one frame
  /// 
  /// \sa VisTextureAnimInstance_cl::SetAnimPhase
  /// \sa VisTextureAnimInstance_cl::GetCurrentPhase
  /// \sa VisTextureAnimInstance_cl::GetCurrentFrame
  /// \sa VisTextureAnimInstance_cl::GetFrameWeighting
  /// \sa VisTextureAnimInstance_cl::GetFrameWeighting255
  /// \sa VisTextureAnimInstance_cl::GetNextFrame
  /// \sa VisTextureAnimInstance_cl::GetNumberOfFrames
  /// \sa VisTextureAnimInstance_cl::GetFrameByIndex
  /// \sa VisTextureAnimInstance_cl::GetAnimationTiming
  /// \sa VisTextureAnimInstance_cl::GetAnimationType
  VISION_APIFUNC void IncAnimPhase(float fPhaseDelta);


  /// \brief
  ///   Internal update function
  VISION_APIFUNC void HandleAnim(float dtime, bool bForceUpdate=false);

  ///
  /// @}
  ///

  ///
  /// @name Create Animated Textures / Instances
  /// @{
  ///

  /// \brief
  ///   Static helper function to build a new texture animation from a list of textures and to
  ///   create an instance for this animation.
  /// 
  /// The first texture in the list will be used as a reference texture, which means that a texture
  /// animation instance will be created for it if the reference texture is assigned to anything
  /// that supports animated textures (screenmasks, particle systems, etc.).
  /// 
  /// Note that a single texture object can only be a reference frame for not more than one texture
  /// animation.
  /// 
  /// \param pFrameList
  ///   A list of texture objects to be used as animation frames. The first list element specifies
  ///   the reference frame texture. Must be list of at least iNumFrames valid pointers
  /// 
  /// \param iNumFrames
  ///   Number of frames used for this animation
  /// 
  /// \param eAnimType
  ///   animation pattern to use, see AnimType_e enum definition for valid types, such as ping-pong
  ///   etc.
  /// 
  /// \param fPhaseTiming
  ///   The time in seconds between each frame
  /// 
  /// \param iFlags
  ///   flags for the animation. Currently the following bit flag constants are provided:
  ///   \li VIS_TEXTUREANIMFLAG_HALT : If specified, the default trigger status is OFF for all
  ///     instances of the animation
  /// 
  /// \return
  ///   VisTextureAnimInstance_cl *pAnim : Pointer of a new animation instance.
  /// 
  /// \sa VisTextureAnimInstance_cl::CreateTextureAnimInstance
  VISION_APIFUNC static VisTextureAnimInstance_cl* CreateNewTextureAnim(VTextureObject **pFrameList, int iNumFrames, AnimType_e eAnimType, float fPhaseTiming, int iFlags=VIS_TEXTUREANIMFLAG_NONE);


  /// \brief
  ///   Static helper function to create a new animation instance from an existing animated
  ///   texture.
  /// 
  /// The texture passed to this function must specify the first frame of an animated texture.
  /// 
  /// The new animation instance can be triggered independently.
  /// 
  /// \param pAnimTexObj
  ///   A texture object that is an animated texture
  /// 
  /// \return
  ///   VisTextureAnimInstance_cl *pAnim : Pointer of a new animation instance.
  /// 
  /// \sa VisTextureAnimInstance_cl::CreateNewTextureAnim
  VISION_APIFUNC static VisTextureAnimInstance_cl* CreateTextureAnimInstance(VTextureObject *pAnimTexObj);

  /// \brief
  ///   Dispose the object and remove it from scene
  VISION_APIFUNC virtual void DisposeObject();

  /// \brief
  ///   Non-virtual override of Release function
  inline void Release()
  {
    // NB: VisTextureAnimInstance_cl does not inherit from VisTypedEngineObject_cl and thus does not know the VObjectFlag_AutoDispose flag
    //     But since it is added to an element manager, we have to take care that a refcount of 1 deletes the object.
    //     In fact this is the same code as in VisObject3D_cl::Release(), except the VObjectFlag_AutoDispose flag.
    int iOldRef = m_iRefCount;
    VRefCounter::Release();
    if (iOldRef==2/* && IsObjectFlagSet(VObjectFlag_AutoDispose)*/)
      DisposeObject();
  }

  // [INTERNAL]
  inline int GetCurrentFrameIndex() const {return m_iCurrentFrame;}
  inline int GetNextFrameIndex() const {return m_iNextFrame;}
  inline void IncAnimTrigger() {m_iTrigger++;}
  ///
  /// @}
  ///

  /// \brief
  ///   Static function to handle all texture animations by the passed time delta. Called from inside the VisionApp_cl class
  VISION_APIFUNC static void HandleAllAnims(float dtime);

private:
  friend class VTextureManager;
  friend class VisionTextureManager;
  friend class VisGame_cl;
  friend class VisElementManager_cl<class VisTextureAnimInstance_cl *>;
  VISION_APIFUNC VisTextureAnimInstance_cl();
  VISION_APIFUNC virtual ~VisTextureAnimInstance_cl();
private:
  VTextureObjectPtr m_pFirstFrame;
  float m_fCurrentTime;
  short m_iCurrentFrame;
  short m_iNextFrame;
  char m_iTrigger;
  bool m_bChanged;
  UBYTE m_iWeighting;
};

VISION_ELEMENTMANAGER_TEMPLATE_DECL(VisTextureAnimInstance_cl)

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
