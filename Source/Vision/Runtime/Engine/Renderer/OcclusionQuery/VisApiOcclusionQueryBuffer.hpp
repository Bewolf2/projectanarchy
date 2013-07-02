/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiOcclusionQueryBuffer.hpp

#ifndef DEFINE_VISAPIOCCLUSIONQUERYBUFFER
#define DEFINE_VISAPIOCCLUSIONQUERYBUFFER

#include <Vision/Runtime/Base/System/Memory/VBaseMem.hpp>

// *** Will be replaced once new containers are ready. ***

/// \brief
///   Simple struct used to match queries to objects in the element managers.
struct VOccData
{
  VOcclusionQuery m_Query;
  int m_iElementManagerIndex;
};

/// \brief
///   This class handles scheduled occlusion tests.
template<unsigned int CHUNK_SIZE = 128, unsigned int RESERVE = 4 * CHUNK_SIZE>
class VOcclusionQueryBuffer
{
private:
  /// \brief
  ///   Dequeue chunk.
  struct Chunk
  {
    VOccData m_Data[CHUNK_SIZE];
  };

public:
  /// \brief
  ///   Constructor.
  VOcclusionQueryBuffer() : m_pChunks(NULL), m_iChunks(0), m_iFirstElement(0), m_iSize(0)
  {
  }

  /// \brief
  ///   Destructor.
  ~VOcclusionQueryBuffer()
  {
    Clear();
  }

  /// \brief
  ///   Adds an index to the queue, returns the corresponding query object.
  VOcclusionQuery& PushBackQuery (int iElementManagerIndex)
  {
    const unsigned int uiIndex = m_iFirstElement + m_iSize;
    const unsigned int uiMaxIndex = CHUNK_SIZE * m_iChunks;

    if (uiIndex >= uiMaxIndex)
    {
      Reserve (uiIndex + 1);
    }

    unsigned int uiChunk  = uiIndex / CHUNK_SIZE;
    unsigned int uiOffset = uiIndex % CHUNK_SIZE;

    m_iSize++;

    m_pChunks[uiChunk]->m_Data[uiOffset].m_iElementManagerIndex = iElementManagerIndex;

    VOcclusionQuery& query = m_pChunks[uiChunk]->m_Data[uiOffset].m_Query;
    VConstructElements<VOcclusionQuery>(&query, 1);
    return query;
  }

  /// \brief
  ///   Removes last element.
  void PopBack ()
  {
    if (m_iSize > 0)
    {
      const unsigned int uiIndex = m_iFirstElement + m_iSize - 1;
      unsigned int iChunk  = uiIndex / CHUNK_SIZE;
      unsigned int iOffset = uiIndex % CHUNK_SIZE;
      VOccData& Data = m_pChunks[iChunk]->m_Data[iOffset];
      VDestructElements<VOcclusionQuery> (&Data.m_Query, 1);
      m_iSize--;
    }
  }

  /// \brief
  ///   Removes first element.
  void PopFront ()
  {
    if (m_iSize > 0)
    {
      unsigned int iChunk  = m_iFirstElement / CHUNK_SIZE;
      unsigned int iOffset = m_iFirstElement % CHUNK_SIZE;
      VOccData& Data = m_pChunks[iChunk]->m_Data[iOffset];
      VDestructElements<VOcclusionQuery> (&Data.m_Query, 1);

      m_iFirstElement++;
      m_iSize--;

      // First Chuck is empty, move it to the end of the chuck table
      if (m_iFirstElement == CHUNK_SIZE)
      {
        Chunk* pChunk = m_pChunks[0];
        for (int i=0; i < (m_iChunks - 1); ++i)
        {
          m_pChunks[i] = m_pChunks[i+1];
        }
        m_pChunks[m_iChunks-1] = pChunk;
        m_iFirstElement = 0;
      }
    }
  }

  /// \brief
  ///   Returns first element in the queue.
  VOccData& Front ()
  {
    VASSERT_MSG (m_iSize > 0, "Access out of bounds!");
    unsigned int iChunk  = m_iFirstElement / CHUNK_SIZE;
    unsigned int iOffset = m_iFirstElement % CHUNK_SIZE;
    return m_pChunks[iChunk]->m_Data[iOffset];
  }

  /// \brief
  ///   Access operator.
  VOccData& operator[] (const unsigned int iIndex)
  {
    VASSERT_MSG (m_iSize > iIndex, "Access out of bounds!");
    unsigned int iChunk  = (m_iFirstElement + iIndex) / CHUNK_SIZE;
    unsigned int iOffset = (m_iFirstElement + iIndex) % CHUNK_SIZE;
    return m_pChunks[iChunk]->m_Data[iOffset];
  }

  /// \brief
  ///   Clears all queued queries.
  void Reset (void)
  {
    for (unsigned int i = 0; i < m_iSize; ++i)
    {
      unsigned int iChunk  = (i + m_iFirstElement) / CHUNK_SIZE;
      unsigned int iOffset = (i + m_iFirstElement) % CHUNK_SIZE;
      VOcclusionQuery* pQuery = &(m_pChunks[iChunk]->m_Data[iOffset].m_Query);
      VDestructElements<VOcclusionQuery> (pQuery, 1);
    }

    m_iFirstElement = 0;
    m_iSize = 0;
  }

  /// \brief
  ///   Clears all queued queries. And deletes buffers.
  void Clear (void)
  {
    Reset();

    for (unsigned int i=0; i < m_iChunks; ++i)
    {
      delete[] (char*)m_pChunks[i];
    }
    delete m_pChunks;

    m_pChunks = NULL;
    m_iChunks = 0;
    m_iFirstElement = 0;
    m_iSize = 0;
  }

  /// \brief
  ///   Reserves enough chunks for iSize elements.
  void Reserve (unsigned int iSize)
  {
    if (iSize == 0)
      return;

    //Should be allocate for the first time, we'll use the template's default size.
    if (RESERVE > iSize)
      iSize = RESERVE;

    const unsigned int uiLastIndex  = m_iFirstElement + iSize - 1;
    const unsigned int iNewChunks  = (uiLastIndex / CHUNK_SIZE) + 1;

    int iAllocChucks = iNewChunks - m_iChunks;
    // Realloc chunk table and add chunks.
    if (iAllocChucks > 0)
    {
      ANALYSIS_IGNORE_WARNING_ONCE(6211)
      Chunk** pNewChunks = new Chunk*[iNewChunks];
      // Copy old chunks to new table.
      for (int i=0; i < m_iChunks; ++i)
      {
        pNewChunks[i] = m_pChunks[i];
      }

      // Add new chunks
      for (int i=0; i < iAllocChucks; ++i)
      {
        Chunk* pChunk = (Chunk*) new char[sizeof(Chunk)];
        pNewChunks[m_iChunks + i] = pChunk;
      }

      delete m_pChunks;
      m_pChunks = pNewChunks;
      m_iChunks = iNewChunks;
    }
  }

  /// \brief
  ///   Returns the current number of elements.
  unsigned int GetSize (void) const
  {
    return m_iSize;
  }

  /// \brief
  ///   Reserves whether the queue is empty.
  bool IsEmpty (void) const
  {
    return (m_iSize == 0);
  }


private:
  Chunk** m_pChunks;
  unsigned int m_iChunks;
  unsigned int m_iFirstElement;
  unsigned int m_iSize;
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
