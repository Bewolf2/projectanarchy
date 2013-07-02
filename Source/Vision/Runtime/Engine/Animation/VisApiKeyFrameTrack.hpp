/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiKeyFrameTrack.hpp

#ifndef VIS_KEYFRAME_TRACK_HPP_INCLUDED
#define VIS_KEYFRAME_TRACK_HPP_INCLUDED

#include <Vision/Runtime/Engine/Animation/VisApiKeyFrame.hpp>

class VisAnimSequence_cl;

/// \brief
///   Base keyframe track class used to store an array of keyframes of specific type.
class VisKeyFrameTrack_cl : public VBaseObject
{
public:


  /// \brief
  ///   Constructor of the keyframe track class
  /// 
  /// The constructor sets keyframe count and stores the animation sequence it belongs to.
  /// 
  /// It does not allocate keyframes of specific type.
  /// 
  /// \param pTempAnimSequence
  ///   Animation sequence the track belongs to.
  /// 
  /// \param iKeyFrameCount
  ///   Keyframe count to allocate later.
  VISION_APIFUNC VisKeyFrameTrack_cl(VisAnimSequence_cl* pTempAnimSequence, int iKeyFrameCount);


  /// \brief
  ///   Destructor
  VISION_APIFUNC virtual ~VisKeyFrameTrack_cl();


  /// \brief
  ///   Has to be called after loading all frames
  VISION_APIFUNC void Finish();


  /// \brief
  ///   GetKeyFrameCount returns number of keyframes in this track.
  /// 
  /// \return
  ///   int fKeyFrameCount: Number of keyframes.
  inline int GetKeyFrameCount() const 

  {
    return m_iKeyFrameCount;
  }


  /// \brief
  ///   GetKeyFrame returns a pointer to a keyframe of specified index
  /// 
  /// Since every keyframe type from derived classes can have different size, this function uses
  /// the m_iKeyframeStride member.
  /// 
  /// \param iFrameIndex
  ///   Keyframe index.
  /// 
  /// \return
  ///   VisKeyFrame_cl* pFrame: Pointer to keyframe.
  inline VisKeyFrame_cl* GetKeyFrame(int iFrameIndex) const 
  {
    VisKeyFrame_cl *pFrame = (VisKeyFrame_cl *)(m_pFrameBasePtr + iFrameIndex*m_iKeyframeStride);
#ifdef HK_DEBUG_SLOW
    VASSERT(pFrame->iMagicValue==0x1234abcd);
#endif
    return pFrame;
  }


  /// \brief
  ///   GetKeyFrameTime returns the time of specified keyframe.
  /// 
  /// \param iFrameIndex
  ///   keyframe index
  /// 
  /// \return
  ///   float fTime: time value of frame
  inline float GetKeyFrameTime(int iFrameIndex)
  {
    return m_fCachedTimes[iFrameIndex];
  }


  /// \brief
  ///   Returns pointers to floor and ceiling keyframe for specified time.
  /// 
  /// This function handles wrapping properly.
  /// 
  /// \param fTime
  ///   Current time.
  /// 
  /// \param pFloorFrame
  ///   Pointer reference that receives floor frame.
  /// 
  /// \param pCeilingFrame
  ///   Pointer reference that receives ceiling frame.
  /// 
  /// \param bLooped
  ///   True if animation should be looped.
  /// 
  /// \return
  ///   float fWeight: Weight factor [0..1] between frames.
  VISION_APIFUNC float GetFloorAndCeilingKeyFrame(float fTime, VisKeyFrame_cl** pFloorFrame, VisKeyFrame_cl** pCeilingFrame, bool bLooped);


  /// \brief
  ///   Returns pointers to keyframes needed for cubic interpolation.
  /// 
  /// Please note that the animation system of the engine does not support cubic interpolation, and that it is also not 
  /// planned to support this feature. This method is thus not used by the engine itself.
  ///
  /// You can use this method in your own application code if you would like to get access to the four animation
  /// frames required for cubic interpolation, e.g. in case you want to implement non-linear interpolation schemes.
  /// 
  /// \param fTime
  ///   current time.
  /// 
  /// \param pBelowFloorFrame
  ///   Pointer to BelowFloorframe.
  /// 
  /// \param pFloorFrame
  ///   Pointer to floor frame.
  /// 
  /// \param pCeilingFrame
  ///   Pointer to ceiling frame.
  /// 
  /// \param pAboveCeilingFrame
  ///   Pointer to AboveCeilingframe.
  /// 
  /// \param bLooped
  ///   True if animation should be looped.
  /// 
  /// \return
  ///   float fWeight: Weight factor [0..1] between frames.
  VISION_APIFUNC float GetCubicKeyFrames(float fTime, VisKeyFrame_cl** pBelowFloorFrame, VisKeyFrame_cl** pFloorFrame, VisKeyFrame_cl** pCeilingFrame, VisKeyFrame_cl** pAboveCeilingFrame, bool bLooped);


  /// \brief
  ///   Returns memory consumption of this keyframe track.
  /// 
  /// \return
  ///   int iBytes: Keyframe memory consumption.
  VISION_APIFUNC int CalcMemSize() const;


  /// \brief
  ///   Searches for the correct ceiling keyframe index for the time value.
  ///
  /// \param fTime
  ///   Time value for which to look up the ceiling keyframe index.
  /// 
  /// \return
  ///   int: Keyframe index.
  VISION_APIFUNC int FindCeilingKeyframeIndex(float fTime) const;

  /// \brief
  ///   Searches for the correct floor keyframe index for the time value.
  VISION_APIFUNC int FindFloorKeyframeIndex(float fTime) const;


  /// \brief
  ///   Evaluates valid floor and ceiling indices for a frame index.
  /// 
  /// \param iCurrentCeilingIndex
  ///   current ceiling index.
  /// 
  /// \param iValidFloorIndex
  ///   Result floor index.
  /// 
  /// \param iValidCeilingIndex
  ///   Result ceiling index.
  /// 
  /// \param bLooped
  ///   Wrap around for cyclic animations.
  VISION_APIFUNC void GetValidFloorAndCeilingFrames(int iCurrentCeilingIndex, int &iValidFloorIndex, int &iValidCeilingIndex, bool bLooped);

protected:
  int m_iKeyFrameCount;                 ///< Number of keyframes
  VisAnimSequence_cl* m_pAnimSequence;  ///< Animation sequence this track belongs to
  float *m_fCachedTimes;
  int m_iKeyframeStride;
  char *m_pFrameBasePtr;
};


/// \brief
///   Keyframe track class implementation used to save translation keyframes.
class VisTranslationKeyFrameTrack_cl: public VisKeyFrameTrack_cl
{
public:

  /// \brief
  ///   Constructor of the translation keyframe track class
  /// 
  /// \param pTempAnimSequence
  ///   Animation sequence the track belongs to.
  /// 
  /// \param iKeyFrameCount
  ///   Number of keyframes to allocate.
  ///
  /// \param iBoneCount
  ///   Number of bones in the skeleton
  VISION_APIFUNC VisTranslationKeyFrameTrack_cl(VisAnimSequence_cl* pTempAnimSequence, int iKeyFrameCount, int iBoneCount);


  /// \brief
  ///   Destructor
  VISION_APIFUNC ~VisTranslationKeyFrameTrack_cl();


  /// \brief
  ///   Non virtual function to access a keyframe of specified index.
  /// 
  /// \param iFrameNumber
  ///   Keyframe index.
  /// 
  /// \return
  ///   VisTranslationKeyFrame_cl* pFrame: Pointer to keyframe.
  inline VisTranslationKeyFrame_cl *GetTranslationKeyFrame(int iFrameNumber) const
  {
    return &m_pKeyFrameList[iFrameNumber];
  }

private:
friend class VisSkeletalAnimSequence_cl;
  VisTranslationKeyFrame_cl* m_pKeyFrameList;    ///< List of keyframes sorted by time
  hkvVec4* m_pTranslations;
};


/// \brief
///   Keyframe track class implementation used to save rotation keyframes.
class VisRotationKeyFrameTrack_cl: public VisKeyFrameTrack_cl
{
public:

  /// \brief
  ///   Constructor of the rotation keyframe track class.
  /// 
  /// The constructor sets keyframe count and stores the animation sequence it belongs to.
  /// 
  /// \param pTempAnimSequence
  ///   Animation sequence the track belongs to.
  /// 
  /// \param iKeyFrameCount
  ///   Keyframe count.
  ///
  /// \param iBoneCount
  ///   Number of bones in the skeleton
  VISION_APIFUNC VisRotationKeyFrameTrack_cl(VisAnimSequence_cl* pTempAnimSequence, int iKeyFrameCount, int iBoneCount);


  /// \brief
  ///   Destructor
  VISION_APIFUNC ~VisRotationKeyFrameTrack_cl();  
  

  /// \brief
  ///   Non virtual function to access a keyframe of specified index.
  /// 
  /// \param iFrameNumber
  ///   Keyframe index.
  /// 
  /// \return
  ///   VisRotationKeyFrame_cl* pFrame: Pointer to keyframe.
  inline VisRotationKeyFrame_cl *GetRotationKeyFrame(int iFrameNumber) const
  {
    return &m_pKeyFrameList[iFrameNumber];
  }

private:
friend class VisSkeletalAnimSequence_cl;
  VisRotationKeyFrame_cl* m_pKeyFrameList;    ///< List of keyframes sorted by time
  hkvQuat* m_pRotations;
};


/// \brief
///   Keyframe track class implementation used to save scaling keyframes.
class VisScalingKeyFrameTrack_cl: public VisKeyFrameTrack_cl
{
public:

  /// \brief
  ///   Constructor of the scaling keyframe track class.
  /// 
  /// The constructor sets keyframe count and saves the animation sequence it belongs to.
  /// 
  /// \param pTempAnimSequence
  ///   Animation sequence the track belongs to.
  /// 
  /// \param iKeyFrameCount
  ///   Keyframe count.
  ///
  /// \param iBoneCount
  ///   Number of bones in the skeleton
  VISION_APIFUNC VisScalingKeyFrameTrack_cl(VisAnimSequence_cl* pTempAnimSequence, int iKeyFrameCount, int iBoneCount);


  /// \brief
  ///   Destructor
  VISION_APIFUNC ~VisScalingKeyFrameTrack_cl();


  /// \brief
  ///   Non virtual function to access a keyframe of specified index.
  /// 
  /// \param iFrameNumber
  ///   Keyframe index.
  /// 
  /// \return
  ///   VisScalingKeyFrame_cl* pFrame: Pointer to keyframe.
  inline VisScalingKeyFrame_cl *GetScalingKeyFrame(int iFrameNumber) const
  {
    return &m_pKeyFrameList[iFrameNumber];
  }

private:
friend class VisSkeletalAnimSequence_cl;
  hkvVec4* m_pScalings;
  VisScalingKeyFrame_cl* m_pKeyFrameList;    ///< List of keyframes sorted by time
};


/// \brief
///   Keyframe track class implementation used to save scaling keyframes.
class VisVertexDeltaKeyFrameTrack_cl: public VisKeyFrameTrack_cl
{
public:

  /// \brief
  ///   Constructor of the vertex delta keyframe track class.
  /// 
  /// The constructor sets keyframe count and saves the animation sequence it belongs to.
  /// 
  /// \param pTempAnimSequence
  ///   Animation sequence the track belongs to.
  /// 
  /// \param iKeyFrameCount
  ///   Keyframe count.
  ///
  VISION_APIFUNC VisVertexDeltaKeyFrameTrack_cl(VisAnimSequence_cl* pTempAnimSequence, int iKeyFrameCount);


  /// \brief
  ///   Destructor
  VISION_APIFUNC ~VisVertexDeltaKeyFrameTrack_cl();
  

  /// \brief
  ///   Non virtual function to access a keyframe of specified index.
  /// 
  /// \param iFrameNumber
  ///   Keyframe index.
  /// 
  /// \return
  ///   VisVertexDeltaKeyFrame_cl* pFrame: Pointer to keyframe.
  inline VisVertexDeltaKeyFrame_cl *GetVertexDeltaKeyFrame(int iFrameNumber) const
  {
    return &m_pKeyFrameList[iFrameNumber];
  }

private:
  VisVertexDeltaKeyFrame_cl* m_pKeyFrameList;    ///< List of keyframes sorted by time
};


/// \brief
///   Keyframe track class implementation used to save offset delta keyframes.
class VisOffsetDeltaKeyFrameTrack_cl: public VisKeyFrameTrack_cl
{
public:

  /// \brief
  ///   Constructor of the offset delta keyframe track class.
  /// 
  /// The constructor sets keyframe count and saves the animation sequence it belongs to.
  /// 
  /// \param pTempAnimSequence
  ///   Animation sequence the track belongs to.
  /// 
  /// \param iKeyFrameCount
  ///   Keyframe count.
  VISION_APIFUNC VisOffsetDeltaKeyFrameTrack_cl(VisAnimSequence_cl* pTempAnimSequence, int iKeyFrameCount);


  /// \brief
  ///   Destructor
  VISION_APIFUNC ~VisOffsetDeltaKeyFrameTrack_cl(); 
  

  /// \brief
  ///   Non virtual function to access a keyframe of specified index.
  /// 
  /// \param iFrameNumber
  ///   Keyframe index.
  /// 
  /// \return
  ///   VisOffsetDeltaKeyFrame_cl* pFrame: Pointer to keyframe.
  inline VisOffsetDeltaKeyFrame_cl *GetOffsetDeltaKeyFrame(int iFrameNumber) const
  {
    return &m_pKeyFrameList[iFrameNumber];
  }

private:
  VisOffsetDeltaKeyFrame_cl* m_pKeyFrameList;    ///< List of keyframes sorted by time
};


/// \brief
///   Keyframe track class implementation used to save rotation delta keyframes.
class VisRotationDeltaKeyFrameTrack_cl: public VisKeyFrameTrack_cl
{
public:

  /// \brief
  ///   Constructor of the rotation delta keyframe track class.
  /// 
  /// The constructor sets keyframe counts and saves the animation sequence it belongs to.
  /// 
  /// \param pTempAnimSequence
  ///   Animation sequence the track belongs to.
  /// 
  /// \param iKeyFrameCount
  ///   Keyframe count.
  VISION_APIFUNC VisRotationDeltaKeyFrameTrack_cl(VisAnimSequence_cl* pTempAnimSequence, int iKeyFrameCount);


  /// \brief
  ///   Destructor
  VISION_APIFUNC ~VisRotationDeltaKeyFrameTrack_cl();


  /// \brief
  ///   Non virtual function to access a keyframe of specified index.
  /// 
  /// \param iFrameNumber
  ///   Keyframe index.
  /// 
  /// \return
  ///   VisRotationDeltaKeyFrame_cl* pFrame: Pointer to keyframe.
  inline VisRotationDeltaKeyFrame_cl *GetRotationDeltaKeyFrame(int iFrameNumber) const
  {
    return &m_pKeyFrameList[iFrameNumber];
  }
  

  /// \brief
  ///   Returns the index of the rotation axis (pitch/roll/yaw) that is stored in this track.
  /// 
  /// The index can also be -1 for no rotation.
  /// 
  /// All keyframes in this track use the same rotation axis.
  /// 
  /// \return
  ///   signed char: Axis index used for rotation.
  inline signed char GetRotationDeltaAxis(void) {return m_scAxis;}


  /// \brief
  ///   Sets the rotation axis index (pitch/roll/yaw) that is used for all keyframes in this track.
  /// 
  /// \param scAxis
  ///   Axis index used for rotation.
  inline void SetRotationDeltaAxis(signed char scAxis) {m_scAxis = scAxis;}

private:
  VisRotationDeltaKeyFrame_cl* m_pKeyFrameList;   ///< List of keyframes sorted by time
  // the rotation delta axis is the same for all keyframes of one track
  signed char m_scAxis;                           ///< -1 or 0..2 for pitch, roll and yaw (x,y,z axis)
};


/// \brief
///   Keyframe track class implementation used to save visibility bounding box keyframes.
class VisVisibilityBoundingBoxKeyFrameTrack_cl: public VisKeyFrameTrack_cl
{
public:

  /// \brief
  ///   Constructor of the visibility bounding box keyframe track class.
  /// 
  /// The constructor sets keyframe count and saves the animation sequence it belongs to.
  /// 
  /// \param pTempAnimSequence
  ///   Animation sequence the track belongs to.
  /// 
  /// \param iKeyFrameCount
  ///   Keyframe count.
  VISION_APIFUNC VisVisibilityBoundingBoxKeyFrameTrack_cl(VisAnimSequence_cl* pTempAnimSequence, int iKeyFrameCount);


  /// \brief
  ///   Destructor
  VISION_APIFUNC ~VisVisibilityBoundingBoxKeyFrameTrack_cl();  
  

  /// \brief
  ///   Non virtual function to access a keyframe of specified index.
  /// 
  /// \param iFrameNumber
  ///   Keyframe index.
  /// 
  /// \return
  ///   VisVisibilityBoundingBoxKeyFrame_cl* pFrame: Pointer to keyframe.
  inline VisVisibilityBoundingBoxKeyFrame_cl *GetVisibilityBoundingBoxKeyFrame(int iFrameNumber) const
  {
    return &m_pKeyFrameList[iFrameNumber];
  }

private:
  VisVisibilityBoundingBoxKeyFrame_cl* m_pKeyFrameList;    ///< List of keyframes sorted by time
};

#endif //VIS_KEYFRAME_TRACK_HPP_INCLUDED

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
