/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiSkeletalAnimControl.hpp

#ifndef VIS_SKELETAL_ANIM_CONTROL_HPP_INCLUDED
#define VIS_SKELETAL_ANIM_CONTROL_HPP_INCLUDED

#include <Vision/Runtime/Engine/Animation/VisApiAnimControl.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiAnimSequence.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiSkeletalAnimSequence.hpp>

// define smartpointer
class VisSkeletalAnimControl_cl;
typedef VSmartPtr<VisSkeletalAnimControl_cl> VisSkeletalAnimControlPtr;

#define VSKELANIMCTRL_DEFAULTS   0

/// \brief
///   Skeletal animation control class derived from base animation control
/// 
/// Implements OnGetCurrentLocalSpaceResult, OnGetLocalSpaceResult, OnUpdateState,
/// OnUpdateControlStateGetAnimResult for skeletal state and result updates.
/// 
/// Overwrites SampleAnimation to perform special bones interpolation.
class VisSkeletalAnimControl_cl:public VisAnimControl_cl, public IVisAnimResultGenerator_cl
{
public:

  /// \brief
  ///   Constructor of the skeletal anim control class
  /// 
  /// \param iFlags
  ///   Flags of the controller. The following flags are supported:
  ///   \li VANIMCTRL_LOOP : the animation is looped
  /// 
  ///   \li VANIMCTRL_NO_SEQUENCE_EVENTS : no sequence events are triggered
  /// 
  ///   \li VANIMCTRL_NO_INTERPOLATION : turns off frame interpolation
  /// 
  /// \param pSkeleton
  ///   pointer to the skeleton animation is played on
  VISION_APIFUNC VisSkeletalAnimControl_cl(const VisSkeleton_cl* pSkeleton, unsigned int iFlags = VSKELANIMCTRL_DEFAULTS);


  /// \brief
  ///   Destructor
  VISION_APIFUNC ~VisSkeletalAnimControl_cl();


  /// \brief
  ///   Overridden OnGetCurrentLocalSpaceResult function to update the current skeletal result.
  /// 
  /// \return
  ///   bool bResult: true if update was successful
  VISION_APIFUNC virtual bool OnGetCurrentLocalSpaceResult();

  /// \brief
  ///   Returns the first frame of the animation
  /// 
  /// \return
  ///   The (cached if available) SkeletalAnimResult for the first frame of the animation.
  VISION_APIFUNC VisSkeletalAnimResult_cl* GetFirstFrameLocalSpaceResult();

  /// \brief
  ///   Sample animation at a given point in time (used by GetFirstFrameLocalSpaceResult and OnGetLocalSpaceResult)
  /// 
  /// \param pResult
  ///   AnimResult to write to
  /// 
  /// \param time
  ///   Time to sample at
  VISION_APIFUNC void GetResultForTime(VisSkeletalAnimResult_cl* pResult, float time);

  /// \brief
  ///   Overridden OnGetLocalSpaceResult function to evaluate the past/future state.
  /// 
  /// \param pResult
  ///   Result that gets filled with the result.
  /// 
  /// \param fTimeDelta
  ///   Time since last state update.
  /// 
  /// \return
  ///   bool bResult: true if update successful
  VISION_APIFUNC virtual bool OnGetLocalSpaceResult(VisSkeletalAnimResult_cl* pResult, float fTimeDelta);


  /// \brief
  ///   Overridden OnUpdateState function to update the skeletal state.
  /// 
  /// \param fTimeDelta
  ///   time since last state update
  /// 
  /// \return
  ///   bool bResult: true if update was successful
  VISION_APIFUNC virtual bool OnUpdateState(float fTimeDelta);

    
  /// \brief
  ///   Overridden OnUpdateControlState function to update the skeletal state.
  /// 
  /// \param fTimeDelta
  ///   time since last state update
  /// 
  /// \return
  ///   bool bResult: true if update was successful
  VISION_APIFUNC virtual bool OnUpdateControlState(float fTimeDelta);


  /// \brief
  ///   Overridden RemoveAllInputs function to remove all input components.
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC virtual void RemoveAllInputs(void);


  /// \brief
  ///   Overridden SetAnimSequence function that refreshes the bone remapping if required.
  /// 
  /// Calls SetAnimSequence of parent class.
  /// 
  /// \param pAnimSequence
  ///   the animation sequence to playback by the control
  VISION_APIFUNC virtual void SetAnimSequence(const VisAnimSequence_cl* pAnimSequence);


  ///
  /// @name Static Helper Functions
  /// @{
  ///

  /// \brief
  ///   Static helper function to create and setup a new skeletal animation control instance.
  /// 
  /// \param pSkeleton
  ///   pointer to the skeleton animation is played on
  /// 
  /// \param pSkeletalAnimSequence
  ///   the animation sequence to playback by the control
  /// 
  /// \param iControlFlags
  ///   animation flags that define the behaviour when reaching start or end of animation sequence:
  ///   \li VANIMCTRL_LOOP is used to play animations like walking in a loop
  /// 
  ///   \li VANIMCTRL_NO_SEQUENCE_EVENTS supresses events on the sequence
  /// 
  ///   \li VANIMCTRL_NO_INTERPOLATION turns off animation interpolation
  /// 
  /// \param fControlSpeed
  ///   the playback speed
  /// 
  /// \param bPlay
  ///   specifies whether the animation should immediately be started
  /// 
  /// \return
  ///   VisSkeletalAnimControl_cl *pAnimControl: new animation control instance
  VISION_APIFUNC static VisSkeletalAnimControl_cl *Create(const VisSkeleton_cl* pSkeleton, VisSkeletalAnimSequence_cl* pSkeletalAnimSequence, int iControlFlags = VSKELANIMCTRL_DEFAULTS, float fControlSpeed = 1.f, bool bPlay = true);

  ///
  /// @}
  ///

private:
  
  /// \brief
  ///   Performs current frame interpolation.
  VISION_APIFUNC void SampleAnimation(VisAnimResult_cl* pDestResult, float fSequenceTime);
  VisSkeletalAnimResult_cl* m_pTempSkeletalResult;    ///< Local anim result used when sample animations that need remapping. Stores the intermediate non-remapped result.
  VisSkeletalAnimResult_cl* m_pCachedFirstFrameResult; ///< Cached first frame of the animation for cheap query, e.g. used to calculate additive offsets
  
protected:

  // serialization
  V_DECLARE_SERIAL_DLLEXP( VisSkeletalAnimControl_cl, VISION_APIDATA )
  VISION_APIFUNC virtual void Serialize( VArchive &ar );
  VISION_APIFUNC VisSkeletalAnimControl_cl();
  void CommonInit();

  VisSkeletonRemappingPtr m_spSkeletonRemapping; ///< optional remapping: needed when control- and sequence-skeletons differ

};

#endif //VIS_SKELETAL_ANIM_CONTROL_HPP_INCLUDED

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
