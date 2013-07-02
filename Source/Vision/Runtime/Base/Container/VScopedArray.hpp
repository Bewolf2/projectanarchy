/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef V_SCOPED_ARRAY_HPP
#define V_SCOPED_ARRAY_HPP


/// \brief
///  Simple scoped array class that will delete the data assigned to it upon destruction.
template <typename T>
class VScopedArray 
{
private:
  T* m_pData;

  VScopedArray(VScopedArray const& rh); //disable copy constructor
  void operator=(VScopedArray const& rh); //disable asignment operator
public:

  /// \brief
  ///  constructor
  ///
  /// \param pData
  ///   pointer to the array data allocated on the heap
  explicit VScopedArray(T* pData)
  {
    m_pData = pData;
  }

  /// \brief
  ///  destructor will call delete[] on the internal pointer
  ~VScopedArray()
  {
    if(m_pData != NULL)
      delete[] m_pData;
  }

  /// \brief
  ///  releases the internal reference to the array and returns it
  T* Release()
  {
    T* pTemp = m_pData;
    m_pData = NULL;
    return pTemp;
  }

  /// \brief
  ///  Returns the internal reference to the array
  inline T* Get() 
  {
    return m_pData;
  }

  /// \brief 
  ///  Returns the internal reference to the array
  inline T const * Get() const 
  {
    return m_pData;
  }

  /// \brief
  ///  Indexing operator. Does not perform any range checking.
  inline T& operator[](size_t uiIndex)
  {
    return m_pData[uiIndex];
  }

  /// \brief
  ///  indexing operator. Does not perform any range checking.
  inline T const& operator[](size_t uiIndex) const
  {
    return m_pData[uiIndex];
  }

  /// \brief
  ///  cast to bool for null pointer check
  operator bool() const
  {
    return (m_pData != NULL);
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
