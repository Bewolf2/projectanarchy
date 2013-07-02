/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vstreamiterator_threadedtask.hpp

#ifndef DEFINE_VISAPISTREAMITERATOR_THREADEDTASK
#define DEFINE_VISAPISTREAMITERATOR_THREADEDTASK

#ifndef _VISION_DOC

#include <Vision/Runtime/Base/System/Threading/StreamProcessor/VDataStream.hpp>

template <class TYPE> class VDataInStreamIterator
{
public:
  VDataInStreamIterator(VDataStream *pStream, int iBufferSize, int iOffset = 0, int iStride = -1)    ///< Stride -1 = sizeof(TYPE)
  {
    m_pDataStream = pStream;
    m_iOffset = iOffset;
    if (iStride==-1)
      iStride = sizeof(TYPE);
    m_iStride = iStride;
    m_pBuffer = NULL;

    m_iCurrentPos = (char*)m_pDataStream->GetDataPtr() + iOffset;
    m_iEndOfStream = (char*)m_pDataStream->GetDataPtr() + m_pDataStream->GetSize();
  }

  TYPE *Next()
  {
    m_iCurrentPos = (char*)m_iCurrentPos + m_iStride;
    if (m_iCurrentPos >= m_iEndOfStream)
      return NULL;
    return (TYPE*)m_iCurrentPos;
  }

  TYPE *GetCurrent()
  {
    return (TYPE*)m_iCurrentPos;
  }

  void Reset()
  {
    m_iCurrentPos = (char*)m_pDataStream->GetDataPtr() + m_iOffset;
  }

  void Flush() {}
  void Finish() {}

private:
  VDataStream *m_pDataStream;
  int m_iOffset;
  int m_iStride;
  void *m_pBuffer;            ///<always NULL for ThreadManager
  void *m_iEndOfStream;

  void *m_iCurrentPos;
};

#define VDataOutStreamIterator VDataInStreamIterator


template <class TYPE> class VDataInObject
{
public:
  VDataInObject(VDataStream *pStream, int iObjectSize=0)
  {
    m_pDataPtr = (TYPE *)pStream->GetDataPtr();
  }

  TYPE *Get()
  {
    return m_pDataPtr;
  }

  void Flush() {}
  void Finish() {}

protected:
  TYPE *m_pDataPtr;
};

#define VDataOutObject VDataInObject


template <class TYPE> class VPointerInStreamIterator
{
public:
  VPointerInStreamIterator(VDataStream *pStream, int iPointerBufferSize, int iObjectBufferSize, int iObjectOffset=0, int iObjectSize=0) : m_PointerInStream(pStream, iPointerBufferSize)
  {
    m_iObjectOffset = iObjectOffset;
  }

  TYPE *Next()
  {
    TYPE **pNextPtr = m_PointerInStream.Next();
    if (pNextPtr==NULL || *pNextPtr==NULL)
      return NULL;
    UINT_PTR iPtr = m_iObjectOffset + (UINT_PTR)(*pNextPtr);
    return (TYPE *)iPtr;
  }

  TYPE *GetCurrent()
  {
    TYPE **pCurrentPtr = m_PointerInStream.GetCurrent();
    if (pCurrentPtr==NULL || *pCurrentPtr==NULL)
      return NULL;
    UINT_PTR iPtr = m_iObjectOffset + (UINT_PTR)(*pCurrentPtr);
    return (TYPE *)iPtr;
  }

  void Reset()
  {
    m_PointerInStream.Reset();
  }

  void Flush() 
  {
    m_PointerInStream.Flush();
  }

  void Finish() 
  {
    m_PointerInStream.Finish();
  }


protected:
  VDataInStreamIterator<TYPE *> m_PointerInStream;
  unsigned int m_iObjectOffset;
};

#endif // _VISION_DOC

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
