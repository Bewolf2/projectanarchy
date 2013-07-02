/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiVertexAnimSequence.hpp

#ifndef VIS_VERTEX_ANIM_SEQUENCE_HPP_INCLUDED
#define VIS_VERTEX_ANIM_SEQUENCE_HPP_INCLUDED


class VDynamicMesh;

// define smartpointer
class VisVertexAnimSequence_cl;
typedef VSmartPtr<VisVertexAnimSequence_cl> VisVertexAnimSequencePtr;

/// \brief
///   Vertex animation sequence class derived from base animation sequence (class
///   VisAnimSequence_cl)
/// 
/// Adds additional tracks to parent class: VertexDeltaTrack
/// 
/// \sa VisAnimSequence_cl
class VisVertexAnimSequence_cl : public VisAnimSequence_cl
{
public:
  
  /// \brief
  ///   Constructor
  VISION_APIFUNC VisVertexAnimSequence_cl();

    
  /// \brief
  ///   Destructor
  VISION_APIFUNC ~VisVertexAnimSequence_cl();


  /// \brief
  ///   Returns the vertex delta track instance.
  /// 
  /// \return
  ///   VisVertexDeltaKeyFrameTrack_cl* pTrack: pointer to vertex delta track
  inline VisVertexDeltaKeyFrameTrack_cl* GetVertexDeltaTrack() const
  {
    return m_pVertexDeltaTrack;
  }

  
  /// \brief
  ///   Sets a new vertex delta track.
  /// 
  /// The old track will be deleted.
  /// 
  /// \param pVertexDeltaTrack
  ///   pointer to new vertex delta track.
  /// 
  /// \return
  ///   Nothing.
  inline void SetVertexDeltaTrack(VisVertexDeltaKeyFrameTrack_cl* pVertexDeltaTrack)
  {  
    VASSERT(pVertexDeltaTrack);
    V_SAFE_DELETE(m_pVertexDeltaTrack);
    m_pVertexDeltaTrack = pVertexDeltaTrack;
  }
  
  /// \brief
  ///   Static function to load a vertex animation sequence from a file chunk.
  /// 
  /// \param AnimFile
  ///   chunk the sequence is read from
  /// 
  /// \param pSequenceSet
  ///   animation sequence set the animation is added to
  /// 
  /// \return
  ///   VisSkeletalAnimSequence_cl*: loaded animation sequence
  static VisVertexAnimSequence_cl* ReadVertexAnimChunk(VChunkFile &AnimFile, VisAnimSequenceSet_cl* pSequenceSet);

  
  /// \brief
  ///   Returns the largest vertex delta index that has been loaded.
  /// 
  /// \return
  ///   int iIndex: largest index
  inline int GetMaxVertexDeltaIndex() const
  {
    return m_AnimatedBitField.GetBitCount();
  }


  /// \brief
  ///   Returns a pointer to the bit field that defines which vertices are animated.
  /// 
  /// \return
  ///   VBitfield *pBitField: pointer to bitfield instance
  inline const VBitfield* GetAnimBitField(void) const
  {
    return &m_AnimatedBitField;
  }

  /// \brief
  ///   overridden function
  VISION_APIFUNC virtual int CalcMemSize(void) const;

private:

  friend class VisAnimSequenceSet_cl;
  VisVertexDeltaKeyFrameTrack_cl* m_pVertexDeltaTrack;
  int m_iOriginalModelVertexCount;
  VBitfield m_AnimatedBitField;
};


#endif //VIS_VERTEX_ANIM_SEQUENCE_HPP_INCLUDED

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
