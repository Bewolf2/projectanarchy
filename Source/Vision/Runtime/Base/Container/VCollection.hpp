/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VCollection.hpp

#ifndef VREFCOUNTEDCOLLECTION_HPP_INCLUDED
#define VREFCOUNTEDCOLLECTION_HPP_INCLUDED



/// \brief
///   Global helper functions for pointer array implementations
class VPointerArrayHelpers
{
public:

  /// \brief
  ///   Returns a new pointer list that holds at least iNewCount members. iCapacity is the current capacity that receives the new count. Only allocates if iNewCount>iCapacity
  VBASE_IMPEXP static void **ReAllocate(void** pOldPointers, int &iCapacity, int iNewCount);

  /// \brief
  ///   Returns the index of pElement in pArray
  VBASE_IMPEXP static int FindPointer(void** pArray, int iCount, const void *pElement);

  /// \brief
  ///   inserts pElement into pArray at specified position. Assumes pArray can take another element. iCount is increased by 1
  VBASE_IMPEXP static void InsertPointer(void** pArray, int &iCount, void *pElement, int iInsertBefore);

  /// \brief
  ///   Removes element at iPos and moves subsequent elements. iCount is decreased by 1
  VBASE_IMPEXP static void RemovePointerAt(void** pArray, int &iCount, int iPos);

  /// \brief
  ///   Returns an apropriate new size that is >=iNeededCount. Adding an element may call ReAllocate(GetAlignedElementCount(m_iCount,m_iCount+1));
  VBASE_IMPEXP static int GetAlignedElementCount(int iCurrentCount, int iNeededCount);
};



/// \brief
///   Template class for a collection that contains reference counted elements.
template <class RCCLASS> class VRefCountedCollection
{
public:

  /// \brief
  ///   Constructor. Takes the initial capacity as a parameter
  inline VRefCountedCollection(int iInitCapacity=0)
  {
    m_iCount = m_iCapacity = 0;
    m_ppElements = NULL;
    EnsureCapacity(iInitCapacity);
  }

  /// \brief
  ///   Destructor. Releases all elements
  inline ~VRefCountedCollection() 
  {
    Clear();
    V_SAFE_DELETE_ARRAY(m_ppElements);
  }

  /// \brief
  ///   Returns the exact number of elements in this collection
  inline int Count() const 
  {
    return m_iCount;
  }

  /// \brief
  ///   Returns the number of allocated entries in the list
  inline int GetCapacity() const 
  {
    return m_iCapacity;
  }

  /// \brief
  ///   Returns the element with index iIndex. No range check is performed.
  inline RCCLASS* GetAt(int iIndex) const 
  {
    VASSERT(iIndex>=0 && iIndex<Count()); 
    return m_ppElements[iIndex];
  }

  /// \brief
  ///   Sets the element at index iIndex. No range check is performed.
  inline void SetAt(int iIndex, RCCLASS* pElement) const 
  {
    VASSERT(iIndex>=0 && iIndex<Count());
    if (m_ppElements[iIndex] != NULL)
      m_ppElements[iIndex]->Release();
    
    m_ppElements[iIndex] = pElement;
    if (pElement != NULL)
      pElement->AddRef();
  }

  /// \brief
  ///   Adds an element to this collection, increases its refcounter and returns its index in the collection.
  inline int Add(RCCLASS *pElement) 
  {
    VASSERT(pElement);
    pElement->AddRef();
    EnsureCapacity(VPointerArrayHelpers::GetAlignedElementCount(m_iCapacity,m_iCount+1));
    m_ppElements[m_iCount++] = pElement;
    return m_iCount-1;
  }

  /// \brief
  ///   Inserts an element into this collection and increases its refcounter
  inline void Insert(RCCLASS *pElement, int iInsertBefore) 
  {
    VASSERT(pElement);
    pElement->AddRef();
    EnsureCapacity(VPointerArrayHelpers::GetAlignedElementCount(m_iCapacity,m_iCount+1));
    VPointerArrayHelpers::InsertPointer((void **)m_ppElements,m_iCount,pElement,iInsertBefore);
  }

  /// \brief
  ///   Adds an element to the collection and returns its index. If it is already present, its index is returned and not added again. If it is not present
  ///   yet, the element will be added and its refcount will be incremented.
  inline int AddUnique(RCCLASS *pElement) 
  {
    VASSERT(pElement);
    int iIndex = Find(pElement);
    if (iIndex < 0)
    {
      return Add(pElement);
    }
    else
    {
      return iIndex;
    }
  }

  /// \brief
  ///   Removes element at specified index and releases the reference
  inline void RemoveAt(int iIndex) 
  {
    GetAt(iIndex)->Release();
    VPointerArrayHelpers::RemovePointerAt((void **)m_ppElements,m_iCount,iIndex);
  }

  /// \brief
  ///   Removes pElement from this collection and releases the reference. The function asserts if the
  ///   element does not exist in collection
  inline void Remove(RCCLASS *pElement) 
  {
    RemoveAt(Find(pElement));
  }

  /// \brief
  ///   Removes pElement from this collection if it exists in the collection
  inline bool SafeRemove(RCCLASS *pElement) 
  {
    int iPos = Find(pElement); 
    if (iPos<0) 
      return false; 
    RemoveAt(iPos);
    return true;
  }

  /// \brief
  ///   Returns the index of the element in the collection (or -1 if it is not found)
  inline int Find(const RCCLASS *pElement) const 
  {
    return VPointerArrayHelpers::FindPointer((void **)m_ppElements,m_iCount,pElement);
  }

  /// \brief
  ///   Indicates whether element exists in the collection
  inline bool Contains(const RCCLASS *pElement) const 
  {
    return Find(pElement)>=0;
  }

  /// \brief
  ///   Clears the collection, and releases the references for all elements
  inline void Clear() 
  {
    const int iCount = m_iCount;
    m_iCount = 0; // reset beforehand in case d'tor looks it up in this list again
    for (int i=0;i<iCount;i++)
    {
      if (m_ppElements[i])
        m_ppElements[i]->Release();
    }
  }

  /// \brief
  ///   Same as Clear
  inline void Reset() 
  {
    Clear();
  }

  /// \brief
  ///   Ensures that the collection has at least the passed capacity (useful if user knows in advance how many elements are needed)
  inline void EnsureCapacity(int iCount) 
  {
    m_ppElements = (RCCLASS**)VPointerArrayHelpers::ReAllocate((void **)m_ppElements,m_iCapacity,iCount);
  }

  /// \brief
  ///   Returns the address of the pointer list for faster iteration
  inline RCCLASS** GetPtrs() 
  {
    return m_ppElements;
  }

  /// \brief
  ///   Serializes the content of this collection. Assumes that the underlying class supports VArchive serialization. Find Implementation in Varchive.hpp
  void SerializeX(class VArchive &ar);

  /// \brief
  ///   Assignment operator: Copies the collection and handles refcounting gracefully
  inline VRefCountedCollection<RCCLASS>& operator = ( const VRefCountedCollection<RCCLASS>& other )
  {
    Clear();
    EnsureCapacity(other.Count());
    for (int i=0;i<other.Count();i++)
      Add(other.GetAt(i));
    return *this;
  }

  /// \brief
  ///   Uniquely adds the passed elements to this collection
  inline VRefCountedCollection<RCCLASS>& operator += ( const VRefCountedCollection<RCCLASS>& other )
  {
    EnsureCapacity(Count() + other.Count()); // might overestimate though
    for (int i=0;i<other.Count();i++)
      AddUnique(other.GetAt(i));
    return  *this;
  }

  /// \brief
  ///   Checks whether both collections contain the same elements (in the same order)
  inline bool operator == ( const VRefCountedCollection<RCCLASS>& other )
  {
    return Equals(other,false);
  }

  /// \brief
  ///   Checks whether both collections contain the same elements (in the same order)
  inline bool operator != ( const VRefCountedCollection<RCCLASS>& other )
  {
    return !Equals(other,false);
  }

  /// \brief
  ///   Checks whether both collections contain the same elements (optionally with order check)
  inline bool Equals(const VRefCountedCollection<RCCLASS>& other, bool bIgnoreOrder)
  {
    if (Count() != other.Count())
      return false;

    if (bIgnoreOrder)
    {
      for (int i=0;i<Count();i++)
        if (other.Find(GetAt(i))<0 || Find(other.GetAt(i))<0)
          return false;
    } else
    {
      for (int i=0;i<Count();i++)
        if (GetAt(i)!=other.GetAt(i))
          return false;
    }
    return true;
  }

private:
  /// \brief
  ///   Prevent using the standard copy constructor
  ///
  /// Copying the collection is quite inefficient. In most cases one can simply get a reference
  /// to the other collection instead of copying it. If a copy is required, then please explicitly
  /// make one by assigning the other collection to your new one via the assignment operator.
  ///
  /// \param VRefCountedCollection<RCCLASS>&
  ///   Source collection to copy from
  VRefCountedCollection(const VRefCountedCollection<RCCLASS>& other)
  {
  }

  int m_iCount;
  int m_iCapacity;
  RCCLASS **m_ppElements;
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
