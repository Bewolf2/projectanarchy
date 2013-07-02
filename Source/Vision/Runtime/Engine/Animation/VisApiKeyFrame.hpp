/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiKeyFrame.hpp

#ifndef VIS_KEYFRAME_HPP_INCLUDED
#define VIS_KEYFRAME_HPP_INCLUDED

#include <Vision/Runtime/Base/Math/Quaternion/hkvQuat.h>

/// \brief
///   Base keyframe class including data structures that are shared across all type of keyframes
/// 
/// This includes time value and virtual operators like = and == that have to be overwritten in
/// subclasses.
class VisKeyFrame_cl
{
public:

  /// \brief
  ///   Constructor of the base keyframe class.
  VISION_APIFUNC VisKeyFrame_cl(void);


  /// \brief
  ///   Destructor
  VISION_APIFUNC virtual ~VisKeyFrame_cl();


  /// \brief
  ///   Returns additional per-frame memory consumption of this keyframe.
  /// 
  /// Derived classes should return the amount of additional data allocated since the memory size
  /// of the frame structure itself is already measured using the track's m_iKeyframeStride member.
  /// 
  /// The value is used for the resource manager. Any derived class should implement this properly.
  /// 
  /// \return
  ///   int iBytes: Keyframe memory consumption in bytes.
  VISION_APIFUNC virtual int CalcMemSize() const
  {
    return 0;
  }


  /// \brief
  ///   Assignment operator.
  /// 
  /// Assignment operator, copies all data from source keyframe.
  /// 
  /// \param OtherKeyFrame
  ///   Source keyframe.
  VISION_APIFUNC virtual void operator=(const VisKeyFrame_cl* OtherKeyFrame) = 0;


  /// \brief
  ///   Equals operator.
  /// 
  /// Equals operator, compares all data from source keyframe with this keyframe.
  /// 
  /// \param OtherKeyFrame
  ///   Source keyframe.
  /// 
  /// \return
  ///   bool bEquals: True for equal keyframes.
  VISION_APIFUNC virtual bool operator==(const VisKeyFrame_cl* OtherKeyFrame) const = 0;

  float m_fTime;      ///< keyframes position on time
  int m_iEntryCount;  ///< for example the number of bones or vertes deltas in this keyframe

#ifdef HK_DEBUG_SLOW
  int iMagicValue;
#endif

protected:


  /// \brief
  ///   Fill keyframe with time and number of entries.
  /// 
  /// A keyframe can have for example data for all bones in the skeleton, so the entrycount value
  /// is used to specify how many of them are saved in one frame.
  /// 
  /// \param fKeyTime
  ///   Time the keyframe is placed.
  /// 
  /// \param iEntryCount
  ///   Number of entries, e.g. bones.
  VISION_APIFUNC void Set(float fKeyTime, int iEntryCount);

};


/// \brief
///   Derived class that implements a translation keyframe
/// 
/// It adds an array of translation vectors (one per bone).
class VisTranslationKeyFrame_cl : public VisKeyFrame_cl
{
public:

  /// \brief
  ///   Constructor of the translation keyframe.
  VISION_APIFUNC VisTranslationKeyFrame_cl();


  /// \brief
  ///   Destructor.
  VISION_APIFUNC ~VisTranslationKeyFrame_cl();


  /// \brief
  ///   Initializes this keyframe
  /// 
  /// \param fKeyTime
  ///   Time the keyframe is placed.
  /// 
  /// \param iBoneCount
  ///   Number of bones.
  ///
  /// \param pBoneArray
  ///   Array that holds iBoneCount entries for this frame
  VISION_APIFUNC void Set(float fKeyTime, int iBoneCount, hkvVec4* pBoneArray);


  /// \brief
  ///   Returns the size of this keyframe type.
  /// 
  /// \return
  ///   int iSize: Keyframe size in bytes.
  VISION_APIFUNC int CalcMemSize() const;


  /// \brief
  ///   Overridden assignment operator.
  /// 
  /// \param OtherKeyFrame
  ///   Source Keyframe
  VISION_APIFUNC virtual void operator=(const VisKeyFrame_cl* OtherKeyFrame);


  /// \brief
  ///   Overridden equals operator.
  /// 
  /// \param OtherKeyFrame
  ///   Source Keyframe
  /// 
  /// \return
  ///   bool bEquals: True for equal keyframes.
  VISION_APIFUNC virtual bool operator==(const VisKeyFrame_cl* OtherKeyFrame) const;

  hkvVec4* m_pTranslation;
};


/// \brief
///   Derived class that implements a rotation keyframe
/// 
/// Adds a list of rotation quaternions(one per bone) to keyframe type.
class VisRotationKeyFrame_cl : public VisKeyFrame_cl
{
public:

  /// \brief
  ///   Constructor of the rotation keyframe.
  VISION_APIFUNC VisRotationKeyFrame_cl();


  /// \brief
  ///   Destructor.
  VISION_APIFUNC ~VisRotationKeyFrame_cl();


  /// \brief
  ///   Initializes the keyframe and store reference to iBoneCount quaternions.
  /// 
  /// \param fKeyTime
  ///   Time the keyframe is placed.
  /// 
  /// \param iBoneCount
  ///   Number of bones.
  ///
  /// \param pBoneArray
  ///   Array that holds iBoneCount entries for this frame
  VISION_APIFUNC void Set(float fKeyTime, int iBoneCount, hkvQuat* pBoneArray);


  /// \brief
  ///   Returns the size of this keyframe type.
  /// 
  /// \return
  ///   int iSize: Keyframe size in bytes.
  VISION_APIFUNC int CalcMemSize() const;


  /// \brief
  ///   Overridden assignment operator.
  /// 
  /// \param OtherKeyFrame
  ///   Source keyframe.
  VISION_APIFUNC virtual void operator=(const VisKeyFrame_cl* OtherKeyFrame);


  /// \brief
  ///   Overridden equals operator.
  /// 
  /// \param OtherKeyFrame
  ///   Source keyframe.
  /// 
  /// \return
  ///   bool bEquals: True for equal keyframes.
  VISION_APIFUNC virtual bool operator==(const VisKeyFrame_cl* OtherKeyFrame) const;

  hkvQuat* m_pRotation;
};


/// \brief
///   Derived class that implements a scaling keyframe
/// 
/// Adds a list of scaling matrices (one per bone) to the keyframe.
class VisScalingKeyFrame_cl : public VisKeyFrame_cl
{
public:

  /// \brief
  ///   Constructor of the scaling keyframe.
  VISION_APIFUNC VisScalingKeyFrame_cl();


  /// \brief
  ///   Destructor.
  VISION_APIFUNC ~VisScalingKeyFrame_cl();


  /// \brief
  ///   Initializes this scaling keyframe.
  /// 
  /// \param fKeyTime
  ///   Time the keyframe is placed.
  /// 
  /// \param iBoneCount
  ///   Number of bones.
  ///
  /// \param pBoneArray
  ///   Array that holds iBoneCount entries for this frame
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC void Set(float fKeyTime, int iBoneCount, hkvVec4* pBoneArray);


  /// \brief
  ///   Returns the size of this keyframe type.
  /// 
  /// \return
  ///   int iSize: Keyframe size in bytes.
  VISION_APIFUNC int CalcMemSize() const;


  /// \brief
  ///   Overridden assignment operator.
  /// 
  /// \param OtherKeyFrame
  ///   Source keyframe.
  VISION_APIFUNC virtual void operator=(const VisKeyFrame_cl* OtherKeyFrame);


  /// \brief
  ///   Overridden equals operator.
  /// 
  /// \param OtherKeyFrame
  ///   Source keyframe.
  /// 
  /// \return
  ///   bool bEquals: True for equal keyframes.
  VISION_APIFUNC virtual bool operator==(const VisKeyFrame_cl* OtherKeyFrame) const;

  hkvVec4* m_pScaling;
};


/// \brief
///   Vertex delta keyframe derived from parent keyframe class.
/// 
/// Adds an array of position delta vectors and an array of normal delta vectors (one per vertex)
/// to general keyframe type.
class VisVertexDeltaKeyFrame_cl : public VisKeyFrame_cl
{
public:

  /// \brief
  ///   Constructor of the vertex delta keyframe.
  VISION_APIFUNC VisVertexDeltaKeyFrame_cl();


  /// \brief
  ///   Destructor.
  VISION_APIFUNC ~VisVertexDeltaKeyFrame_cl();


  /// \brief
  ///   Initializes this keyframe. It allocates iDeltaCount vectors.
  /// 
  /// \param fKeyTime
  ///   time the keyframe is placed.
  /// 
  /// \param iDeltaCount
  ///   Number of vertexdeltas.
  VISION_APIFUNC void Set(float fKeyTime, int iDeltaCount);


  /// \brief
  ///   Returns the size of this keyframe type.
  /// 
  /// \return
  ///   int iSize: Keyframe size in bytes.
  VISION_APIFUNC int CalcMemSize() const;


  /// \brief
  ///   Overridden assignment operator.
  /// 
  /// \param OtherKeyFrame
  ///   Source keyframe.
  VISION_APIFUNC virtual void operator=(const VisKeyFrame_cl* OtherKeyFrame);


  /// \brief
  ///   Overridden equals operator.
  /// 
  /// \param OtherKeyFrame
  ///   Source keyframe.
  /// 
  /// \return
  ///   bool bEquals: True for equal keyframes.
  VISION_APIFUNC virtual bool operator==(const VisKeyFrame_cl* OtherKeyFrame) const;

  int* m_pVertexIndex;
  hkvVec3* m_pPositionDelta;
  hkvVec3* m_pNormalDelta;
};


/// \brief
///   Offset Delta keyframe derived from parent keyframe class.
/// 
/// Adds a single OffsetDelta vector to general keyframe type.
class VisOffsetDeltaKeyFrame_cl : public VisKeyFrame_cl
{
public:

  /// \brief
  ///   Constructor of the offset delta keyframe.
  VISION_APIFUNC VisOffsetDeltaKeyFrame_cl();


  /// \brief
  ///   Destructor.
  VISION_APIFUNC ~VisOffsetDeltaKeyFrame_cl();


  /// \brief
  ///   Initializes this keyframe.
  /// 
  /// \param fKeyTime
  ///   Time the keyframe is placed.
  VISION_APIFUNC void Set(float fKeyTime);


  /// \brief
  ///   Returns the size of this keyframe type.
  /// 
  /// \return
  ///   int iSize: Keyframe size in bytes.
  VISION_APIFUNC int CalcMemSize() const;


  /// \brief
  ///   Overridden assignment operator.
  /// 
  /// \param OtherKeyFrame
  ///   Source keyframe.
  VISION_APIFUNC virtual void operator=(const VisKeyFrame_cl* OtherKeyFrame);


  /// \brief
  ///   Overridden equals operator.
  /// 
  /// \param OtherKeyFrame
  ///   Source keyframe.
  /// 
  /// \return
  ///   bool bEquals: True for equal keyframes.
  VISION_APIFUNC virtual bool operator==(const VisKeyFrame_cl* OtherKeyFrame) const;

  hkvVec3 m_OffsetDelta;
};


/// \brief
///   Rotation delta keyframe derived from parent keyframe class.
/// 
/// Adds a single RotationDelta vector to general keyframe type.
class VisRotationDeltaKeyFrame_cl : public VisKeyFrame_cl
{
public:

  /// \brief
  ///   Constructor of the rotation delta keyframe.
  VISION_APIFUNC VisRotationDeltaKeyFrame_cl();


  /// \brief
  ///   Destructor.
  VISION_APIFUNC ~VisRotationDeltaKeyFrame_cl();


  /// \brief
  ///   Initializes this keyframe.
  /// 
  /// \param fKeyTime
  ///   Time the keyframe is placed.
  VISION_APIFUNC void Set(float fKeyTime);

  /// \brief
  ///   Returns the size of this keyframe type.
  /// 
  /// \return
  ///   int iSize: Keyframe size in bytes.
  VISION_APIFUNC int CalcMemSize() const;


  /// \brief
  ///   Overridden assignment operator.
  /// 
  /// \param OtherKeyFrame
  ///   Source keyframe.
  VISION_APIFUNC virtual void operator=(const VisKeyFrame_cl* OtherKeyFrame);


  /// \brief
  ///   Overridden equals operator.
  /// 
  /// \param OtherKeyFrame
  ///   Source keyframe.
  /// 
  /// \return
  ///   bool bEquals: True for equal keyframes.
  VISION_APIFUNC virtual bool operator==(const VisKeyFrame_cl* OtherKeyFrame) const;

  float m_fAngle;        ///< angle in degrees 
};


/// \brief
///   Visibility bounding box keyframe derived from parent keyframe class.
/// 
/// Adds a visibility bounding box to general keyframe type.
class VisVisibilityBoundingBoxKeyFrame_cl : public VisKeyFrame_cl
{
public:

  /// \brief
  ///   Constructor of the visibility bounding box keyframe.
  VISION_APIFUNC VisVisibilityBoundingBoxKeyFrame_cl();


  /// \brief
  ///   Destructor.
  VISION_APIFUNC ~VisVisibilityBoundingBoxKeyFrame_cl();


  /// \brief
  ///   Initializes this keyframe.
  /// 
  /// \param fKeyTime
  ///   Time the keyframe is placed.
  VISION_APIFUNC void Set(float fKeyTime);


  /// \brief
  ///   Returns the size of this keyframe type.
  /// 
  /// \return
  ///   int iSize: Keyframe size in bytes.
  VISION_APIFUNC int CalcMemSize() const;


  /// \brief
  ///   Overridden assignment operator.
  /// 
  /// \param OtherKeyFrame
  ///   Source keyframe.
  VISION_APIFUNC virtual void operator=(const VisKeyFrame_cl* OtherKeyFrame);


  /// \brief
  ///   Overridden equals operator.
  /// 
  /// \param OtherKeyFrame
  ///   Source keyframe.
  /// 
  /// \return
  ///   bool bEquals: True for equal keyframes.
  VISION_APIFUNC virtual bool operator==(const VisKeyFrame_cl* OtherKeyFrame) const;

  hkvAlignedBBox m_VisibilityBoundingBox;
};


#endif //VIS_KEYFRAME_HPP_INCLUDED

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
