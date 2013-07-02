/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiSkeletalAnimSequence.hpp

#ifndef VIS_SKELETAL_ANIM_SEQUENCE_HPP_INCLUDED
#define VIS_SKELETAL_ANIM_SEQUENCE_HPP_INCLUDED

#include <Vision/Runtime/Engine/Animation/VisApiKeyFrameTrack.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiAnimSequence.hpp>

// define smartpointer
class VisSkeletalAnimSequence_cl;
typedef VSmartPtr<VisSkeletalAnimSequence_cl> VisSkeletalAnimSequencePtr;

/// \brief
///   Skeletal animation sequence class derived from base animation sequence
/// 
/// Adds additional tracks to parent class: TranslationTrack, RotationTrack
/// 
/// Every animation sequence represents the data from one animation with several animation tracks.
/// Several controls can use the same animation data and play it at a different time, speed and
/// direction.
/// 
/// \sa VisAnimSequence_cl
class VisSkeletalAnimSequence_cl:public VisAnimSequence_cl
{
public:

  /// \brief
  ///   Constructor of the skeletal anim sequence class.
  VISION_APIFUNC VisSkeletalAnimSequence_cl();

    
  /// \brief
  ///   Destructor
  VISION_APIFUNC ~VisSkeletalAnimSequence_cl();


  /// \brief
  ///   Returns the skeleton the animation data has been built on
  /// 
  /// \return
  ///   VisSkeleton_cl* pSkeleton: pointer to skeleton
  inline const VisSkeleton_cl* GetSkeleton(void) const
  {
    return m_spSkeleton;
  }


  /// \brief
  ///   Returns the translation track instance.
  /// 
  /// \return
  ///   VisTranslationKeyFrameTrack_cl* pTrack: pointer to translation track
  inline VisTranslationKeyFrameTrack_cl* GetTranslationTrack(void) const 
  {
    return m_pTranslationTrack;
  }


  /// \brief
  ///   Returns the rotation track instance.
  /// 
  /// \return
  ///   VisRotationKeyFrameTrack_cl* pTrack: pointer to rotation track
  inline VisRotationKeyFrameTrack_cl* GetRotationTrack(void) const
  {
    return m_pRotationTrack;
  }


  /// \brief
  ///   Returns the scaling track instance.
  /// 
  /// \return
  ///   VisScalingKeyFrameTrack_cl* pTrack: pointer to rotation track
  inline VisScalingKeyFrameTrack_cl* GetScalingTrack() const
  {
    return m_pScalingTrack;
  }
  

  /// \brief
  ///   Sets a new translation track.
  /// 
  /// \param pTranslationTrack
  ///   pointer to new translation track.
  /// 
  /// \return
  ///   Nothing.
  inline void SetTranslationTrack(VisTranslationKeyFrameTrack_cl* pTranslationTrack)
  {  
    VASSERT(pTranslationTrack);
    V_SAFE_DELETE(m_pTranslationTrack);
    m_pTranslationTrack = pTranslationTrack;
  }

  /// \brief
  ///   Sets a new rotation track.
  /// 
  /// \param pRotationTrack
  ///   pointer to new rotation track.
  /// 
  /// \return
  ///   Nothing.
  inline void SetRotationTrack(VisRotationKeyFrameTrack_cl* pRotationTrack)
  {
    VASSERT(pRotationTrack);
    V_SAFE_DELETE(m_pRotationTrack);
    m_pRotationTrack = pRotationTrack;
  }

  /// \brief
  ///   Sets a new scaling track.
  /// 
  /// \param pScalingTrack
  ///   pointer to new scaling track.
  /// 
  /// \return
  ///   Nothing.
  inline void SetScalingTrack(VisScalingKeyFrameTrack_cl* pScalingTrack)
  {
    V_SAFE_DELETE(m_pScalingTrack);
    m_pScalingTrack = pScalingTrack;
  }


  /// \brief
  ///   Helper function to ensure that all loaded quaternions are neighboorhooded to avoid playback
  ///   atrifacts.
  /// 
  /// \param bLooped
  ///   TRUE for looped animations
  /// 
  /// \return
  ///   bool bResult : true if animation data seems OK
  VISION_APIFUNC bool EnsureRotationContinuity(bool bLooped);


  /// \brief
  ///   Helper function to test whethert all loaded quaternions are neighboorhooded.
  /// 
  /// \param bLooped
  ///   TRUE for looped animations
  /// 
  /// \return
  ///   bool bResult : true if animation data seems OK
  VISION_APIFUNC bool TestRotationContinuity(bool bLooped);


  /// \brief
  ///   Load a skeletal animation sequence from a file chunk.
  /// 
  /// \param AnimFile
  ///   chunk file the sequence is read from
  /// 
  /// \param pSequenceSet
  ///   animation sequence set the animation is added to
  /// 
  /// \return
  ///   VisSkeletalAnimSequence_cl*: loaded animation sequence
  static VisSkeletalAnimSequence_cl* ReadSkeletalAnimChunk(VChunkFile &AnimFile, VisAnimSequenceSet_cl* pSequenceSet);

  /// \brief
  ///   overridden function
  VISION_APIFUNC virtual int CalcMemSize(void) const;

protected:

  friend class VisAnimSequenceSet_cl;
  VisSkeletonPtr m_spSkeleton;        ///< skeleton animation was created for

  // animation tracks holding bone animation data, in local space absolute values (so NOT relative to the initial pose)
  VisTranslationKeyFrameTrack_cl* m_pTranslationTrack;
  VisRotationKeyFrameTrack_cl* m_pRotationTrack;
  VisScalingKeyFrameTrack_cl* m_pScalingTrack;
};

#endif //VIS_SKELETAL_ANIM_SEQUENCE_HPP_INCLUDED

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
