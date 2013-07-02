/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VRingBuffer.hpp

#ifndef VRINGBUFFER_HPP
#define VRINGBUFFER_HPP

  /// \class  VRingBuffer
  ///
  /// \brief  A templated generic ring buffer.
  ///
  template<typename T, unsigned int Size> class VRingBuffer
  {
  public:

    /// \brief  
    ///   Default constructor. Initializes the variables to default values
    VRingBuffer()
      : m_uiReadIndex(0), m_uiWriteIndex(0), m_uiReadCount(0), m_uiWriteCount(0)
    {
      memset(m_Buffer, 0, sizeof(T) * Size);
    }

    /// \brief  Reads an entry from the buffer. 
    ///
    /// \param [out] pTarget The target of the read operation - the value read from the buffer will be stored there.
    ///
    /// \return true if an element could be read, false otherwise (e.g. buffer is empty).
    ///
    bool Read(T* pTarget)
    {
      // if no elements are actually in the buffer return false
      if(GetCount() == 0 || !pTarget)
        return false;

      // read the element
      memcpy(pTarget, GetPointerFromIndex(m_uiReadIndex), sizeof(T));
      m_uiReadIndex = GetIncWrappedIndex(m_uiReadIndex);

      m_uiReadCount++;

      return true;
    }

    /// \brief  Writes an element to the buffer. 
    ///
    /// \param [in] pSource The source value which will be written into the buffer.
    ///
    /// \return true if it the value can be written, false if the buffer is full.
    ///
    bool Write(T* pSource)
    {
      // If the buffer is full return false
      if(GetCount() == Size || !pSource)
        return false;

      // write the element
      memcpy(GetPointerFromIndex(m_uiWriteIndex), pSource, sizeof(T));
      m_uiWriteIndex = GetIncWrappedIndex(m_uiWriteIndex);

      m_uiWriteCount++;

      return true;
    }

    /// \brief  Returns the number of unread elements currently in the buffer. 
    ///
    /// \return The count of unread elements currently in the buffer. 
    ///
    unsigned int GetCount()
    {
      return m_uiWriteCount - m_uiReadCount;
    }

    /// \brief Returns the size of the buffer
    ///
    /// \return The size of the buffer
    ///
    unsigned int GetSize()
    {
      return Size;
    }

  protected:

    /// \brief  Helper function returning an incremented wrapped index corresponding to the size of the buffer.
    ///
    /// \param  uiIndex The index to work on.
    ///
    /// \return The incremented and possibly wrapped index. 
    ///
    inline unsigned int GetIncWrappedIndex(unsigned int uiIndex)
    {
      uiIndex++;
      return uiIndex % Size;
    }

    /// \fn inline void* GetPointerFromIndex(unsigned int uiIndex)
    ///
    /// \brief  Gets a pointer from an index to read from / write to. (Helper function)
    ///
    /// \param  uiIndex Zero-based index of the element of the buffer. 
    ///
    /// \return null The pointer to read from / write to.
    ///
    inline void* GetPointerFromIndex(unsigned int uiIndex)
    {
      return static_cast<void*>(&m_Buffer[uiIndex]);
    }

    //! The data buffer. 
    T m_Buffer[Size];

    //! The current read index. 
    unsigned int m_uiReadIndex;

    //! The current write index. 
    unsigned int m_uiWriteIndex;

    //! The count of elements read from the buffer. 
    unsigned int m_uiReadCount;

    //! The count of elements written to the buffer. 
    unsigned int m_uiWriteCount;

  };

  /// \class  VThreadSafeRingBuffer
  ///
  /// \brief  Implementation of a thread safe ring buffer which uses locks to protect the buffer accesses.
  ///
  template<typename T, unsigned int Size> class VThreadSafeRingBuffer : public VRingBuffer<T, Size>
  {
  public:

    /// \brief  Default constructor. 
    VThreadSafeRingBuffer()
      : VRingBuffer<T, Size>(), m_LockMutex()
    {
    }

    /// \brief See: VRingBuffer::Read
    bool Read(T* pTarget)
    {
      m_LockMutex.Lock();
      bool bRet = VRingBuffer<T, Size>::Read(pTarget);
      m_LockMutex.Unlock();

      return bRet;
    }

    /// \brief See: VRingBuffer::Write
    bool Write(T* pSource)
    {
      m_LockMutex.Lock();
      bool bRet = VRingBuffer<T, Size>::Write(pSource);
      m_LockMutex.Unlock();

      return bRet;      
    }

  private:

    VMutex m_LockMutex;
  };


  /// \brief
  ///   A simple ring buffer template for accumulation of values.  
  ///
  /// This buffer uses no separate read pointer; Elements can be accessed by index via the array index operator, where the most recently written element is to be found
  /// at the highest index (\c Size - 1).
  ///
  /// \param T
  ///   the type of the elements stored in the buffer
  /// 
  /// \param Size
  ///   the size of the buffer
  template<typename T>
  class VSimpleRingBuffer
  {
  public:
    /// The element type
    typedef T ElementType;

  private:
    /// The elements managed by this buffer
    T *m_pElements;
    /// The size of the buffer
    unsigned int m_iSize;
    /// The pointer denoting the next element to be overwritten
    unsigned int m_iWritePointer;

  public:

    /// \brief
    ///   Performs a zero-initialization of all elements in the buffer.
    VSimpleRingBuffer(unsigned int iSize)
    {
      m_iSize = iSize;
      m_pElements = new T[iSize];
      memset(m_pElements, 0, sizeof(T) * iSize);
      m_iWritePointer = 0;
    }

    /// \brief
    ///   Destroys the buffer.
    ~VSimpleRingBuffer()
    {
      delete[] m_pElements;
    }

    /// \brief
    ///   Adds an element to the buffer.
    void Add(T element)
    {
      m_pElements[m_iWritePointer] = element;
      m_iWritePointer = (++m_iWritePointer % m_iSize);
    }

    /// \brief
    ///   Returns the last element of the buffer.
    ///
    /// \return
    ///   the last element of the buffer. If no elements have been added so far,
    ///   this returns 0.
    T GetLast() const
    {
      return m_pElements[m_iWritePointer ? m_iWritePointer - 1 : m_iSize - 1];
    }

    /// \brief
    ///   Returns the size of this buffer.
    ///
    /// \return
    ///   the size of this buffer
    unsigned int GetSize() const
    {
      return m_iSize;
    }

    /// \brief
    ///   Returns an element from the buffer.
    ///
    /// \param iIndex
    ///   index of the element to get. \c 0 is the oldest element in the
    ///   buffer, Size - 1 is the youngest.
    ///
    /// \return
    ///   The requested element
    T operator[](unsigned int iIndex) const
    {
      iIndex = (m_iWritePointer + iIndex) % m_iSize;
      return m_pElements[iIndex];
    }
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
