/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VDataStream.hpp

#ifndef DEFINE_VISAPIDATASTREAM
#define DEFINE_VISAPIDATASTREAM

#define VSTREAMFLAG_WRITE           1
#define VSTREAMFLAG_READ            2
#define VSTREAMFLAG_FETCH_AT_LOAD   4
#define VSTREAMFLAG_VIDEORAM        8

/// \brief
///   Data stream definition for stream processing tasks.
/// 
/// A VDataStream can be both an input and an output stream. It is fully type-agnostic, so it can
/// be used to represent arbitrary data types.
class VDataStream
{
public:

  /// \brief
  ///   Default constructor of the VDataStream object.
  VDataStream() { m_pData=0; m_iStreamSize=0; m_iStreamFlags=VSTREAMFLAG_WRITE|VSTREAMFLAG_READ; }

  /// \brief
  ///   Constructor of the VDataStream object.
  VDataStream(void *pData, int iStreamSize, int iFlags = VSTREAMFLAG_WRITE|VSTREAMFLAG_READ) { m_pData = pData; m_iStreamSize = iStreamSize; m_iStreamFlags = iFlags; }

  /// \brief
  ///   Sets the pointer to the data of this stream.
  inline void SetDataPtr(void *pData) { m_pData = pData; }

  /// \brief
  ///   Returns the pointer to the data of this stream.
  inline void *GetDataPtr() const { return m_pData; }

  /// \brief
  ///   Sets the size (in bytes) of this data stream.
  inline void SetSize(int iSize) { m_iStreamSize = iSize; }

  /// \brief
  ///   Returns the size (in bytes) of this data stream.
  inline int GetSize() const { return m_iStreamSize; }

  /// \brief
  ///   Currently unsupported.
  inline void SetFlags(int iFlags) { m_iStreamFlags = iFlags; }

  /// \brief
  ///   Currently unsupported.
  inline int GetFlags() const { return m_iStreamFlags; }

private:

  void *m_pData;
  int  m_iStreamSize;
  int  m_iStreamFlags;
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
