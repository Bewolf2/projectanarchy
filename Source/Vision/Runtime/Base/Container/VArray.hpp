/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VArray.hpp

#ifndef DC_VISION_VARRAY_HPP
#define DC_VISION_VARRAY_HPP

#include <Vision/Runtime/Base/System/Memory/VBaseMem.hpp>

/// \brief
///   Template class for array implementations
/// 
template<class TYPE, class ARG_TYPE = const TYPE&>
class VArray
{
public:

  ///
  /// @name Init/DeInit
  /// @{
  ///

  /// \brief
  ///   Array constructor
  VArray();

  /// \brief
  ///   Array constructor
  ///
  /// \param iReserveElements
  ///   An initial array will be allocated to reserve space for this many elements.
  ///
  /// \param iInitialSize
  ///   Sets how many elements should be constructed initially in the array.
  ///
  /// \param iGrowByNumElements
  ///   At each reallocation the array will grow by iGrowByNumElements.
  ///   A value of 0 will result in the array growing by a factor of 1.5.
  VArray(unsigned int iReserveElements, unsigned int iInitialSize = 0, unsigned int iGrowByNumElements = 0);

  /// \brief
  ///   Copy constructor
  VArray(const VArray& other);

  /// \brief
  ///   Destructor 
  ~VArray();

  ///
  /// @}
  ///

  ///
  /// @name Attributes
  /// @{
  ///

  /// \brief
  ///   Returns the current size of the array
  int GetSize() const;

  /// \brief
  ///   Same as GetSize()-1
  int GetUpperBound() const;

  /// \brief
  ///   Sets number of elements the array should grow by on the next resize
  void SetGrowBy(int iGrowByElements);

  /// \brief
  ///   Reserves array of size
  void Reserve(int iNewSize);

  /// \brief
  ///   Sets a new array size
  void SetSize(int nNewSize, int nGrowBy = -1, bool bDeallocateIfEmpty = true);

  ///
  /// @}
  ///

  ///
  /// @name Operations
  /// @{
  ///
  void FreeExtra();
  void RemoveAll();

  ///
  /// @}
  ///

  ///
  /// @name Compatibility with VPList
  /// @{
  ///

  inline int GetLength() const { return GetSize(); }
  inline void Reset() { RemoveAll(); }


  ///
  /// @}
  ///

  ///
  /// @name Accessing elements
  /// @{
  ///

  /// \brief Returns the element at the given index. Identical to VArray::Get.
  const TYPE& GetAt(int nIndex) const;

  /// \brief Returns the element at the given index. Identical to VArray::GetAt.
  inline const TYPE& Get(int nIndex) const { return GetAt(nIndex); }
  
  void SetAt(int nIndex, ARG_TYPE newElement);
  TYPE& ElementAt(int nIndex);
  int Find(ARG_TYPE element, int startIndex=0) const;

  const TYPE* GetData() const;
  TYPE* GetData();

  // overloaded operator helpers
  const TYPE& operator[](int nIndex) const;
  TYPE& operator[](int nIndex);

  ///
  /// @}
  ///

  ///
  /// @name Growing the array
  /// @{
  ///
  
  void SetAtGrow(int nIndex, ARG_TYPE newElement);
  int Add(ARG_TYPE newElement);
  inline int Append(ARG_TYPE newElement) { return Add(newElement); }
  int Append(const VArray& src);
  void Copy(const VArray& src);

  // Assignment operator.
  inline VArray& operator=(const VArray& rhs)   { Copy(rhs); return *this; }

  // Comparison operators.
  inline bool operator==(const VArray& rhs)   { int n=GetLength(); if(n!=rhs.GetLength()) return false; for(int i=0; i<n; ++i) { if((*this)[i] != rhs[i]) return false; } return true; }
  inline bool operator!=(const VArray& rhs)   { return !(*this == rhs); }

  // Operations that move elements around
  void InsertAt(int nIndex, ARG_TYPE newElement, int nCount = 1);
  void RemoveAt(int nIndex, int nCount = 1);
  void InsertAt(int nStartIndex, VArray* pNewArray);

  
  // Swap contents with another array.
  void Swap(VArray& other);


  ///
  /// @}
  ///

protected:
  TYPE* m_pData;   ///< the actual array of data
  int m_nSize;     ///< # of elements (upperBound - 1)
  int m_nMaxSize;  ///< max allocated
  int m_nGrowBy;   ///< grow amount
};


/////////////////////////////////////////////////////////////////////////////
// VArray<TYPE, ARG_TYPE> inline functions

template<class TYPE, class ARG_TYPE>
inline int VArray<TYPE, ARG_TYPE>::GetSize() const
  { return m_nSize; }
template<class TYPE, class ARG_TYPE>
inline int VArray<TYPE, ARG_TYPE>::GetUpperBound() const
  { return m_nSize-1; }
template<class TYPE, class ARG_TYPE>
inline void VArray<TYPE, ARG_TYPE>::RemoveAll()
  { SetSize(0, -1); }
template<class TYPE, class ARG_TYPE>
inline const TYPE& VArray<TYPE, ARG_TYPE>::GetAt(int nIndex) const
  { VASSERT(nIndex >= 0 && nIndex < m_nSize);
    return m_pData[nIndex]; }
template<class TYPE, class ARG_TYPE>
inline void VArray<TYPE, ARG_TYPE>::SetAt(int nIndex, ARG_TYPE newElement)
  { VASSERT(nIndex >= 0 && nIndex < m_nSize);
    m_pData[nIndex] = newElement; }
template<class TYPE, class ARG_TYPE>
inline TYPE& VArray<TYPE, ARG_TYPE>::ElementAt(int nIndex)
  { VASSERT(nIndex >= 0 && nIndex < m_nSize);
    return m_pData[nIndex]; }
template<class TYPE, class ARG_TYPE>
inline const TYPE* VArray<TYPE, ARG_TYPE>::GetData() const
  { return (const TYPE*)m_pData; }
template<class TYPE, class ARG_TYPE>
inline TYPE* VArray<TYPE, ARG_TYPE>::GetData()
  { return (TYPE*)m_pData; }
template<class TYPE, class ARG_TYPE>
inline int VArray<TYPE, ARG_TYPE>::Add(ARG_TYPE newElement)
  { int nIndex = m_nSize;
    SetAtGrow(nIndex, newElement);
    return nIndex; }
template<class TYPE, class ARG_TYPE>
inline const TYPE& VArray<TYPE, ARG_TYPE>::operator[](int nIndex) const
  { return GetAt(nIndex); }
template<class TYPE, class ARG_TYPE>
inline TYPE& VArray<TYPE, ARG_TYPE>::operator[](int nIndex)
  { return ElementAt(nIndex); }

/////////////////////////////////////////////////////////////////////////////
// VArray<TYPE, ARG_TYPE> out-of-line functions

template<class TYPE, class ARG_TYPE>
VArray<TYPE, ARG_TYPE>::VArray()
{
  m_pData = NULL;
  m_nSize = m_nMaxSize = m_nGrowBy = 0;
}

template<class TYPE, class ARG_TYPE>
VArray<TYPE, ARG_TYPE>::VArray(unsigned int iReserveElements, unsigned int iInitialElements, unsigned int iGrowByNumElements)
{
  m_pData = NULL;
  m_nSize = iInitialElements;
  m_nMaxSize = 0;
  m_nGrowBy = (int)iGrowByNumElements;
  Reserve( hkvMath::Max((int)iReserveElements, (int)iInitialElements) );
  if (iInitialElements > 0)
  {
    VConstructElementsZeroInit<TYPE>(m_pData, iInitialElements);
  }
}

template<class TYPE, class ARG_TYPE>
VArray<TYPE, ARG_TYPE>::VArray(const VArray& other)
{
  m_pData = NULL;
  m_nSize = m_nMaxSize = m_nGrowBy = 0;
  Copy(other);
}

template<class TYPE, class ARG_TYPE>
VArray<TYPE, ARG_TYPE>::~VArray()
{
  VASSERT(V_IS_VALID_PTR(this));

  if (m_pData != NULL)
  {
    VDestructElements<TYPE>(m_pData, m_nSize);
    delete[] (BYTE*)m_pData;
  }
}

template<class TYPE, class ARG_TYPE>
void VArray<TYPE, ARG_TYPE>::SetGrowBy(int iGrowByElements)
{
  m_nGrowBy = iGrowByElements;
}

template<class TYPE, class ARG_TYPE>
void VArray<TYPE, ARG_TYPE>::Reserve(int iNewSize)
{
  if (iNewSize > m_nMaxSize)
  {
    TYPE* pNewData = (TYPE*) new BYTE[iNewSize * sizeof(TYPE)];
    
    if (m_pData != NULL)
    {
      // copy new data from old
      memcpy(pNewData, m_pData, m_nSize * sizeof(TYPE));

      // get rid of old stuff (note: no destructors called)
      delete[] (BYTE*)m_pData;
    }
   
    m_pData = pNewData;
    m_nMaxSize = iNewSize;
  }
}

template<class TYPE, class ARG_TYPE>
void VArray<TYPE, ARG_TYPE>::SetSize(int nNewSize, int nGrowBy, bool bDeallocateIfEmpty)
{
  VASSERT(V_IS_VALID_PTR(this));
  VASSERT(nNewSize >= 0);

  if (nGrowBy != -1)
    m_nGrowBy = nGrowBy;  ///< set new size

  if (nNewSize == 0 && bDeallocateIfEmpty)
  {
    // shrink to nothing
    if (m_pData != NULL)
    {
      VDestructElements<TYPE>(m_pData, m_nSize);
      delete[] (BYTE*)m_pData;
      m_pData = NULL;
    }
    m_nSize = m_nMaxSize = 0;
  }
  else if (m_pData == NULL && nNewSize > 0)
  {
    // Construct first array. 
    m_nMaxSize = hkvMath::Max(nGrowBy, nNewSize);
    m_pData = (TYPE*) new BYTE[m_nMaxSize * sizeof(TYPE)];
    VConstructElementsZeroInit<TYPE>(m_pData, nNewSize);
    m_nSize = nNewSize;
  }
  else if (nNewSize <= m_nMaxSize)
  {
    // it fits
    if (nNewSize > m_nSize)
    {
      // initialize the new elements
      VConstructElementsZeroInit<TYPE>(&m_pData[m_nSize], nNewSize-m_nSize);
    }
    else if (m_nSize > nNewSize)
    {
      // destroy the old elements
      VDestructElements<TYPE>(&m_pData[nNewSize], m_nSize-nNewSize);
    }
    m_nSize = nNewSize;
  }
  else
  {
    nGrowBy = m_nGrowBy;
    if (nGrowBy == 0)
    {
      // otherwise, grow array by 1.5 of previous size (minimal growth = 4)
      nGrowBy = m_nMaxSize / 2 < 4 ? 4 : m_nMaxSize / 2;
    }
    int nNewMax = m_nMaxSize + nGrowBy;
    if (nNewSize > nNewMax)
    {
      nNewMax = nNewSize;
    }

    VASSERT(nNewMax >= m_nMaxSize);  ///< no wrap around
    TYPE* pNewData = (TYPE*) new BYTE[nNewMax * sizeof(TYPE)];

    // copy new data from old
    memcpy(pNewData, m_pData, m_nSize * sizeof(TYPE));

    // construct remaining elements
    VASSERT(nNewSize > m_nSize);
    VConstructElementsZeroInit<TYPE>(&pNewData[m_nSize], nNewSize-m_nSize);

    // get rid of old stuff (note: no destructors called)
    delete[] (BYTE*)m_pData;
    m_pData = pNewData;
    m_nSize = nNewSize;
    m_nMaxSize = nNewMax;
  }
}

template<class TYPE, class ARG_TYPE>
int VArray<TYPE, ARG_TYPE>::Append(const VArray& src)
{
  VASSERT(V_IS_VALID_PTR(this));
  VASSERT(this != &src);   ///< cannot append to itself

  int nOldSize = m_nSize;
  SetSize(m_nSize + src.m_nSize);
  VCopyElements<TYPE>(m_pData + nOldSize, src.m_pData, src.m_nSize);
  return nOldSize;
}

template<class TYPE, class ARG_TYPE>
void VArray<TYPE, ARG_TYPE>::Copy(const VArray& src)
{
  VASSERT(V_IS_VALID_PTR(this));
  VASSERT(this != &src);   ///< cannot append to itself

  SetSize(src.m_nSize);
  VCopyElements<TYPE>(m_pData, src.m_pData, src.m_nSize);
}

template<class TYPE, class ARG_TYPE>
void VArray<TYPE, ARG_TYPE>::FreeExtra()
{
  VASSERT(V_IS_VALID_PTR(this));

  if (m_nSize != m_nMaxSize)
  {
    // shrink to desired size
    TYPE* pNewData = NULL;
    if (m_nSize != 0)
    {
      pNewData = (TYPE*) new BYTE[m_nSize * sizeof(TYPE)];
      // copy new data from old
      memcpy(pNewData, m_pData, m_nSize * sizeof(TYPE));
    }

    // get rid of old stuff (note: no destructors called)
    delete[] (BYTE*)m_pData;
    m_pData = pNewData;
    m_nMaxSize = m_nSize;
  }
}

template<class TYPE, class ARG_TYPE>
void VArray<TYPE, ARG_TYPE>::SetAtGrow(int nIndex, ARG_TYPE newElement)
{
  VASSERT(V_IS_VALID_PTR(this));
  VASSERT(nIndex >= 0);

  if (nIndex >= m_nSize)
    SetSize(nIndex+1, -1);
  m_pData[nIndex] = newElement;
}

template<class TYPE, class ARG_TYPE>
void VArray<TYPE, ARG_TYPE>::InsertAt(int nIndex, ARG_TYPE newElement, int nCount /*=1*/)
{
  VASSERT(V_IS_VALID_PTR(this));
  VASSERT(nIndex >= 0);    ///< will expand to meet need
  VASSERT(nCount > 0);     ///< zero or negative size not allowed

  if (nIndex >= m_nSize)
  {
    // adding after the end of the array
    SetSize(nIndex + nCount, -1);   ///< grow so nIndex is valid
  }
  else
  {
    // inserting in the middle of the array
    int nOldSize = m_nSize;
    SetSize(m_nSize + nCount, -1);  ///< grow it to new size
    // destroy intial data before copying over it
    VDestructElements<TYPE>(&m_pData[nOldSize], nCount);
    // shift old data up to fill gap
    memmove(&m_pData[nIndex+nCount], &m_pData[nIndex],
      (nOldSize-nIndex) * sizeof(TYPE));

    // re-init slots we copied from
    VConstructElementsZeroInit<TYPE>(&m_pData[nIndex], nCount);
  }

  // insert new value in the gap
  VASSERT(nIndex + nCount <= m_nSize);
  while (nCount--)
    m_pData[nIndex++] = newElement;
}

template<class TYPE, class ARG_TYPE>
void VArray<TYPE, ARG_TYPE>::RemoveAt(int nIndex, int nCount)
{
  VASSERT(V_IS_VALID_PTR(this));
  VASSERT(nIndex >= 0);
  VASSERT(nCount >= 0);
  VASSERT(nIndex + nCount <= m_nSize);

  // just remove a range
  int nMoveCount = m_nSize - (nIndex + nCount);
  VDestructElements<TYPE>(&m_pData[nIndex], nCount);
  if (nMoveCount)
    memmove(&m_pData[nIndex], &m_pData[nIndex + nCount],
      nMoveCount * sizeof(TYPE));
  m_nSize -= nCount;
}

template<class TYPE, class ARG_TYPE>
void VArray<TYPE, ARG_TYPE>::InsertAt(int nStartIndex, VArray* pNewArray)
{
  VASSERT(V_IS_VALID_PTR(this));
  VASSERT(pNewArray != NULL);
  VASSERT(V_IS_VALID_PTR(this));
  VASSERT(nStartIndex >= 0);

  if (pNewArray->GetSize() > 0)
  {
    InsertAt(nStartIndex, pNewArray->GetAt(0), pNewArray->GetSize());
    for (int i = 0; i < pNewArray->GetSize(); i++)
      SetAt(nStartIndex + i, pNewArray->GetAt(i));
  }
}

template<class TYPE, class ARG_TYPE>
int VArray<TYPE, ARG_TYPE>::Find(ARG_TYPE element, int startIndex) const
{
  for ( int i = startIndex; i < m_nSize; ++i )
  {
    if ( m_pData[i] == element )
      return i;
  }

  return -1;
}

template<class TYPE, class ARG_TYPE>
void VArray<TYPE, ARG_TYPE>::Swap(VArray& other)
{
  VASSERT(V_IS_VALID_PTR(this));

  TYPE* pData    = m_pData;
  int   nSize    = m_nSize;
  int   nMaxSize = m_nMaxSize;
  int   nGrowBy  = m_nGrowBy;

  m_pData    = other.m_pData;
  m_nSize    = other.m_nSize;
  m_nMaxSize = other.m_nMaxSize;
  m_nGrowBy  = other.m_nGrowBy;

  other.m_pData    = pData;
  other.m_nSize    = nSize;
  other.m_nMaxSize = nMaxSize;
  other.m_nGrowBy  = nGrowBy;
}

#endif  //DC_VISION_VARRAY_HPP

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
