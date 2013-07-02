/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiAnimSequence.hpp

#ifndef VIS_ANIM_SEQUENCE_HPP_INCLUDED
#define VIS_ANIM_SEQUENCE_HPP_INCLUDED

#include <Vision/Runtime/Engine/Mesh/VisApiModel.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiKeyFrameTrack.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiAnimEventList.hpp>

// define smartpointer
class VisAnimSequence_cl;
typedef VSmartPtr<VisAnimSequence_cl> VisAnimSequencePtr;

class VisAnimEventList_cl;

#define MAX_MODEL_FILE_VERSION                0
#define MAX_ANIMATION_FILE_VERSION            0
#define MAX_SKELETAL_CHUNK_VERSION            0
#define MAX_GEOMETRY_CHUNK_VERSION            0
#define MAX_MATERIAL_CHUNK_VERSION            2
#define MAX_SKELETAL_ANIMATION_CHUNK_VERSION  0
#define MAX_EVENT_CHUNK_VERSION               0
#define MAX_VERTEX_ANIMATION_CHUNK_VERSION    1
#define MAX_OFFSET_DELTA_CHUNK_VERSION        1
#define MAX_ROTATION_DELTA_CHUNK_VERSION      1


/// \brief
///   Derived class for loading a VisAnimSequence_cl resource 
class VSequenceSerializationProxy : public IVSerializationProxy
{
public:
  inline VSequenceSerializationProxy(VisAnimSequence_cl *pSeq) {VASSERT(pSeq);m_pSeq=pSeq;}
  virtual IVSerializationProxyCreator *GetInstance();

protected:
  // serialization
  inline VSequenceSerializationProxy() {m_pSeq=NULL;}
  V_DECLARE_SERIAL_DLLEXP( VSequenceSerializationProxy, VISION_APIDATA );
  virtual void Serialize( VArchive &ar );
private:
  VisAnimSequence_cl *m_pSeq;
};


/// \brief
///   Base class with common attributes and methods for vertex and skeletal animation sequences
/// 
/// The VisAnimSequence_cl is virtual but includes all functionality that a standard sequence
/// needs.
/// 
/// An animation sequence contains the data of one animation as several animation tracks.
/// 
/// Multiple controls can use the same animation data and play it at a different time, speed and
/// direction.
/// 
/// All animation types have event and motion delta data, every track is optional and does not have
/// to be included.
/// 
/// \sa VisSkeletalAnimSequence_cl
/// \sa VisVertexAnimSequence_cl
class VisAnimSequence_cl : public VRefCounter, public IVSerializationProxyCreator
{
public:

  /// \brief
  ///   Constructor of the anim sequence class
  VISION_APIFUNC VisAnimSequence_cl();


  /// \brief
  ///   Destructor
  VISION_APIFUNC ~VisAnimSequence_cl();


  /// \brief
  ///   Returns the type of the animation sequence (VIS_MODELANIM_VERTEX, VIS_MODELANIM_SKELETAL or
  ///   VIS_MODELANIM_UNKNOWN)
  /// 
  /// \return
  ///   VisModelAnimType_e eType: type of animation resource
  inline VisModelAnimType_e GetType() const 
  {
    return m_eType;
  }


  /// \brief
  ///   Returns the length of the animation sequence in seconds.
  /// 
  /// \return
  ///   float fTime: animation length in seconds
  inline float GetLength() const 
  {
    return m_fDuration; 
  }


  /// \brief
  ///   Returns the name of the animation sequence.
  /// 
  /// This matches the name the artists used in the modeling software.
  /// 
  /// \return
  ///   const char* szName: sequence name
  inline const char* GetName() const
  {
    return m_Name.AsChar();
  }


  /// \brief
  ///   Returns the local event list of this sequence.
  /// 
  /// You can use it to add your own events to the sequence.
  /// 
  /// \return
  ///   VisAnimEventList_cl* m_pEventList: event list of the sequence.
  /// 
  /// \note
  ///   The control checks two event lists for events: Its own event list and the event list of the
  ///   animation sequence it is playing.
  /// 
  /// \note
  ///   The difference is that the sequence event list is shared by all controls that playback that
  ///   sequence.
  /// 
  /// \sa VisAnimEventList_cl;
  inline VisAnimEventList_cl* GetEventList() 
  {
    return &m_EventList;
  }


  /// \brief
  ///   Indicates whether there is a VisibilityBoundingBox track.
  /// 
  /// \return
  ///   BOOL result: if TRUE the sequence has a VisibilityBoundingBox track
  inline bool HasVisibilityBoundingBoxes(void) const 
  {
    return (m_pVisibilityBoundingBoxTrack!=NULL);
  }


  /// \brief
  ///   Indicates whether there is a offset delta track.
  /// 
  /// \return
  ///   BOOL result: if TRUE the sequence has a offset delta track
  inline bool HasOffsetDelta(void) const 
  {
    return (m_pOffsetDeltaTrack!=NULL);
  }


  /// \brief
  ///   Indicates whether there is a rotation delta track.
  /// 
  /// \return
  ///   BOOL result: if TRUE the sequence has a rotation delta track
  inline bool HasRotationDelta(void) const
  {
    return (m_pRotationDeltaTrack!=NULL);
  }


  /// \brief
  ///   Returns the VisibilityBoundingBox track.
  /// 
  /// \return
  ///   VisVisibilityBoundingBoxKeyFrameTrack_cl*: pointer to VisibilityBoundingBox track
  inline VisVisibilityBoundingBoxKeyFrameTrack_cl* GetVisibilityBoundingBoxTrack(void) const
  {
    return m_pVisibilityBoundingBoxTrack;
  }


  /// \brief
  ///   Returns the offset delta track.
  /// 
  /// \return
  ///   VisOffsetDeltaKeyFrameTrack_cl*: pointer to offset delta track
  inline VisOffsetDeltaKeyFrameTrack_cl* GetOffsetDeltaTrack(void) const
  {
    return m_pOffsetDeltaTrack;
  }


  /// \brief
  ///   Returns the rotation delta track.
  /// 
  /// \return
  ///   VisRotationDeltaKeyFrameTrack_cl*: pointer to rotation delta track
  inline VisRotationDeltaKeyFrameTrack_cl* GetRotationDeltaTrack(void) const 
  { 
    return m_pRotationDeltaTrack;
  }


  /// \brief
  ///   Releases the old track and sets the new one.
  /// 
  /// \param pVisibilityBoundingBoxTrack
  ///   pointer to new visibility bounding box track.
  /// 
  /// \return
  ///   nothing.
  inline void SetVisibilityBoundingBoxTrack(VisVisibilityBoundingBoxKeyFrameTrack_cl* pVisibilityBoundingBoxTrack)
  { 
    VASSERT(pVisibilityBoundingBoxTrack);
    V_SAFE_DELETE(m_pVisibilityBoundingBoxTrack);
    m_pVisibilityBoundingBoxTrack = pVisibilityBoundingBoxTrack;
  }
  

  /// \brief
  ///   Releases the old track and sets the new one.
  /// 
  /// \param pOffsetDeltaTrack
  ///   pointer to new offset delta track.
  /// 
  /// \return
  ///   nothing.
  inline void SetOffsetDeltaTrack(VisOffsetDeltaKeyFrameTrack_cl* pOffsetDeltaTrack)
  { 
    VASSERT(pOffsetDeltaTrack);
    V_SAFE_DELETE(m_pOffsetDeltaTrack);
    m_pOffsetDeltaTrack = pOffsetDeltaTrack;
  }

  /// \brief
  ///   Releases the old track and sets the new one to NULL.
  /// 
  /// \return
  ///   nothing.
  inline void RemoveOffsetDeltaTrack()
  { 
    V_SAFE_DELETE(m_pOffsetDeltaTrack);
    m_pOffsetDeltaTrack = NULL;
  }

  /// \brief
  ///   Releases the old track and sets the new one.
  /// 
  /// \param pRotationDeltaTrack
  ///   pointer to new rotation delta track.
  /// 
  /// \return
  ///   nothing.
  inline void SetRotationDeltaTrack(VisRotationDeltaKeyFrameTrack_cl* pRotationDeltaTrack)
  { 
    VASSERT(pRotationDeltaTrack);
    V_SAFE_DELETE(m_pRotationDeltaTrack);
    m_pRotationDeltaTrack = pRotationDeltaTrack;
  }


  /// \brief
  ///   Estimates the memory consumption of all tracks (used for the resource).
  /// 
  /// \return
  ///   int iBytes: memory consumption (in bytes) of this sequence.
  VISION_APIFUNC virtual int CalcMemSize(void) const;

  /// \brief
  ///   Internal function to set the owner sequence set after serialisation
  inline void SetOwnerSequenceSet(VisAnimSequenceSet_cl *pOwner)
  {
    m_pOwner = pOwner;
  }

  /// \brief
  ///   Internal function to get the sequence set that owns this single sequence
  inline VisAnimSequenceSet_cl* GetSequenceSet() const
  {
    return m_pOwner;
  }

protected:

  // overridden IVSerializationProxyCreator function
  VISION_APIFUNC virtual IVSerializationProxy *CreateProxy() {return new VSequenceSerializationProxy(this);}

  VisAnimSequenceSet_cl *m_pOwner; ///< the owner sequence set
  VisModelAnimType_e m_eType;   ///< type of sequence: VIS_MODELANIM_VERTEX, VIS_MODELANIM_SKELETAL
  float m_fDuration;            ///< length of the sequence in seconds
  VString m_Name;               ///< name of animation sequence e.g. run, walk backwards,...
  
  // event handling
  VisAnimEventList_cl m_EventList;

  // motion delta and animated visibility bounding boxes can exists for every kind of animation
  VisVisibilityBoundingBoxKeyFrameTrack_cl* m_pVisibilityBoundingBoxTrack;
  VisOffsetDeltaKeyFrameTrack_cl* m_pOffsetDeltaTrack;
  VisRotationDeltaKeyFrameTrack_cl* m_pRotationDeltaTrack;


  /// \brief
  ///   Used by the engine to load the visibility bounding box track from the chunkfile.
  /// 
  /// \param AnimFile
  ///   specifies the chunkfile to load the chunk from
  /// 
  /// \param pAnimSequence
  ///   animation sequence the loaded track gets added to
  /// 
  /// \return
  ///   bool bResult: True for successful loading.
  static bool ReadVisibilityBoundingBoxChunk(VChunkFile &AnimFile, VisAnimSequence_cl* pAnimSequence);


  /// \brief
  ///   Used by the engine to load the offset delta track from the chunkfile.
  /// 
  /// \param AnimFile
  ///   specifies the chunkfile to load the chunk from
  /// 
  /// \param pAnimSequence
  ///   animation sequence the loaded track gets added to
  /// 
  /// \return
  ///   bool bResult: True for successful loading.
  static bool ReadOffsetDeltaChunk(VChunkFile &AnimFile, VisAnimSequence_cl* pAnimSequence);


  /// \brief
  ///   Used by the engine to load the rotation delta track from the chunkfile.
  /// 
  /// \param AnimFile
  ///   specifies the chunkfile to load the chunk from
  /// 
  /// \param pAnimSequence
  ///   animation sequence the loaded track gets added to
  /// 
  /// \return
  ///   bool bResult: True for successful loading.
  static bool ReadRotationDeltaChunk(VChunkFile &AnimFile, VisAnimSequence_cl* pAnimSequence);

  // fixme: remove this method as soon as all models support the two new tracks
  static bool ReadMotionDeltaChunk(VChunkFile &AnimFile, VisAnimSequence_cl* pAnimSequence);


  /// \brief
  ///   Used by the engine to load the event track from the chunkfile.
  /// 
  /// \param AnimFile
  ///   specifies the chunkfile to load the chunk from
  /// 
  /// \param pAnimSequence
  ///   animation sequence the loaded track gets added to
  /// 
  /// \return
  ///   bool bResult: True for successful loading.
  static bool ReadEventChunk(VChunkFile &AnimFile, VisAnimSequence_cl* pAnimSequence);

};

#endif //VIS_ANIM_SEQUENCE_HPP_INCLUDED

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
