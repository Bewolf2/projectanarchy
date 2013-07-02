/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// file VStack.hpp

#ifndef _VSTACK_HPP_
#define _VSTACK_HPP_

/// \brief
///   Base class for a fixed-size stack. Please use VFixedSizeStack instead.
template<typename T>
class VFixedSizeStackBase
{
protected:
  T* m_pData;
  size_t m_uiDataSize;
  size_t m_uiSize;

  VFixedSizeStackBase(T* pData, size_t size) : 
  m_pData(pData),
  m_uiDataSize(size),
  m_uiSize(0) 
  { }

  ///copying not implemented yet
  VFixedSizeStackBase(const VFixedSizeStackBase& other);
  void operator=(const VFixedSizeStackBase& other);

public:

  /// \brief
  ///   Pushes a new object onto the stack
  ///
  /// \param value
  ///   The object to push
  void Push(const T& value)
  {
    VASSERT_MSG(m_uiSize < m_uiDataSize,"stack is out of bounds");
    m_pData[m_uiSize++] = value;
  }

  /// \brief
  ///   Pops an object from the stack
  ///
  /// \return
  ///   The object on the top of the stack
  T Pop() 
  {
    VASSERT_MSG(m_uiSize > 0,"stack out of bounds");
    return m_pData[--m_uiSize];
  }

  /// \brief
  ///   Returns the top object from the stack (without popping it)
  ///
  /// \return
  ///   The object on the top of the stack
  T& Top()
  {
    VASSERT_MSG(m_uiSize > 0,"nothing on the stack");
    return m_pData[m_uiSize-1];
  }

  /// \brief
  ///   Returns the topmost object from the stack (without popping it)
  ///
  /// \return
  ///   The object on the top of the stack 
  const T& Top() const
  {
    VASSERT_MSG(m_uiSize > 0,"nothing on the stack");
    return m_pData[m_uiSize-1];
  }

  /// \brief
  ///   Returns the number of elements currently on the stack
  size_t Size() const
  {
    return m_uiSize;
  }

  /// \brief
  ///   Index operator for randomly accessing elements on the stack
  /// 
  /// \param index
  ///   The index of the element to return
  ///
  /// \return
  ///   The value of the element at index
  T& operator [](size_t index)
  {
    VASSERT_MSG(index < m_uiSize, "Out of bounds");
    return m_pData[index];
  }

  /// \brief
  ///   Index operator for randomly accessing elements on the stack
  /// 
  /// \param index
  ///   The index of the element to get
  ///
  /// \return
  ///   The value of the element at index
  const T& operator [](size_t index) const
  {
    VASSERT_MSG(index < m_uiSize, "Out of bounds");
    return m_pData[index];
  }
};

/// \brief
///   A fixed-size stack that does not allocate memory but uses static memory instead
template<typename T,size_t size>
class VFixedSizeStack : public VFixedSizeStackBase<T>
{
private:
  T m_Data[size];

public:
  /// \brief
  ///   Constructor
  VFixedSizeStack() : VFixedSizeStackBase<T>(m_Data,size)
  {
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
