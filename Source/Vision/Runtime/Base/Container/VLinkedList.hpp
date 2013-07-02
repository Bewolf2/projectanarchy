/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VLinkedList.hpp

#ifndef VLINKEDLIST_HPP
#define VLINKEDLIST_HPP

#include <Vision/Runtime/Base/System/Memory/VBaseMem.hpp>


/// \brief
///   Class for one element of the doubly-linked list (LinkedList_cl)
template <class TL> struct LinkedList_Element_cl
{
public:
  typedef int (VISION_CDECL* CompareFunc)(const LinkedList_Element_cl& lhs, const LinkedList_Element_cl& rhs);


  void* operator new(size_t iSize)
  {
    return VBaseAlloc(iSize);
  }

  void operator delete(void *ptr)
  {
    VBaseDealloc(ptr);
  }

  LinkedList_Element_cl *next;        ///< pointer to the next element in the linked list
  LinkedList_Element_cl *prev;        ///< pointer to the previous element in the linked list
  TL value;                           ///< value of this element


private:
  // Implementation details.
#ifndef _VISION_DOC
  template<class> friend class LinkedList_cl;
  LinkedList_Element_cl<TL>* Sort(CompareFunc compare);
  LinkedList_Element_cl<TL>* Merge(LinkedList_Element_cl* other, CompareFunc compare);
#endif
};


/// \brief
///   Doubly-Linked list class
template <class TL> class LinkedList_cl
{
public:

  /// \brief
  ///   Constructor of the linked list class; initializes an empty linked list
  LinkedList_cl();
  LinkedList_cl(const LinkedList_cl &other);
  ~LinkedList_cl();
  
  
  /// \brief
  ///   Adds a new element to the end of the linked list
  void Add(const TL& newvalue);

  /// \brief
  ///   Inserts a new element at the n-th location of the linked list
  void Insert(int index, const TL& newvalue);

  /// \brief
  ///   Removes the element at the n-th location of the linked list
  void RemoveAt(int index);

  /// \brief
  ///   Removes the first element with the passed value and returns TRUE if an element has been removed.
  BOOL Remove(const TL& killvalue);

  /// \brief
  ///   Removes the given element from the linked list
  ///
  /// \param remove
  ///   The element that should be removed. It must not be NULL and it has to be part of the list. No validation is performed.
  void Remove(LinkedList_Element_cl<TL> *remove);
  
  /// \brief
  ///   Removes all the elements from the linked list and reinitializes the linked list
  void Clear();
  
  /// \brief
  ///   Checks whether the linked list is empty
  /// 
  /// \return
  ///   BOOL isEmpty: if TRUE then the linked list is completely empty, otherwise it contains at least one element.
  BOOL IsEmpty() const;

  
  /// \brief
  ///   Searches for an element in the linked list
  /// 
  /// \param newvalue
  ///   Element to search for
  /// 
  /// \return
  ///   BOOL isEntry: if TRUE then the element is in the linked list, otherwise it could not be found
  BOOL IsEntry(const TL& newvalue) const;

  
  /// \brief
  ///   Returns the current size of the linked list (number of elements)
  /// 
  /// \return
  ///   int size: number of elements the linked list currently contains
  int  GetSize() const;


  /// \brief
  ///   Sorts elements using merge sort (stable, inplace)
  ///   
  /// \param compare
  ///   Comparison function to use to classify elements to sort
  void Sort(typename LinkedList_Element_cl<TL>::CompareFunc compare);

  /// \brief  Intersect with the specified Liked list. All elements not found in the other linked list will be removed.
  ///
  /// \param  other The linked list used to perform the intersection.
  ///
  /// \return The number of removed elements.
  int IntersectWith(const LinkedList_cl<TL> &other);

  /// \brief
  ///   Returns the index-th element in the linked list
  /// 
  /// \param index
  ///   index of the element to return (has to be between 0 and (size - 1))
  /// 
  /// \return
  ///   TL& element
  /// 
  /// \note
  ///   This function doesn't check whether the index is valid. 
  TL& operator[] (int index);
  
  
  /// \brief
  ///   Returns the index-th element in the linked list
  /// 
  /// \param index
  ///   index of the element which will be returned (has to be between 0 and (size - 1))
  /// 
  /// \return
  ///   const TL& element
  /// 
  /// \note
  ///   This function doesn't check whether the index is valid. 
  const TL& operator[] (int index) const;

  LinkedList_cl &operator=(const LinkedList_cl &rhs);

  LinkedList_Element_cl<TL> *first;           ///< pointer to the first element
  LinkedList_Element_cl<TL> *last;             ///< pointer to the last element

private:
  mutable LinkedList_Element_cl<TL> *found;    ///< temporary pointer to an element
};


template<class TL> LinkedList_cl<TL>::LinkedList_cl(const LinkedList_cl<TL> &other)
{
  first=NULL;
  last=NULL;
  *this = other;
}


template<class TL> LinkedList_cl<TL>::LinkedList_cl()
{
  first=NULL;
  last=NULL;
}


template<class TL> void LinkedList_cl<TL>::Add(const TL& newvalue)
{
  LinkedList_Element_cl<TL> *newp=NULL;
  //newp=(LinkedList_Element_cl *) _nmalloc(sizeof(LinkedList_Element_cl));
  newp=new LinkedList_Element_cl<TL>;
  newp->value=newvalue;
  newp->prev=last;
  newp->next=NULL;
  if (first==NULL) {
    first=newp;
    last=newp;
  } else {
    last->next=newp;
    last=newp;
  }
}


template<class TL> void LinkedList_cl<TL>::Insert(int index, const TL& newvalue)
{
  LinkedList_Element_cl<TL> *iter=first;
  LinkedList_Element_cl<TL> *newp=NULL;
  int i;
  
  newp=new LinkedList_Element_cl<TL>;
  newp->value=newvalue;
  if (first==NULL) {
    first=newp;
    last=newp;
    newp->prev = NULL;
    newp->next = NULL;
  }
  else if (index == 0)
  {
    newp->prev = NULL;
    newp->next = first;
    first = newp;
  }
  else
  {
    for (i = 0; i < (index - 1); i++)
      iter = iter->next;
    newp->prev = iter;
    newp->next = iter->next;
    if (iter == last)
      last = newp;
    else
      iter->next->prev = newp;
    iter->next = newp;
  }
}


template<class TL> void LinkedList_cl<TL>::RemoveAt(int index)
{
  LinkedList_Element_cl<TL> *iter = first;
  LinkedList_Element_cl<TL> *delp;
  
  if (index == 0)
  {
    if (last == first)
      last = NULL;
    delp = first;
    first = first->next;
    delete delp;
  }
  else
  {
    for (int i = 0; i < (index - 1); i++)
      iter = iter->next;
    delp = iter->next;
    if (delp == last)
      last = iter;
    else
      delp->next->prev = iter;
    iter->next = delp->next;
    delete delp;
  }
}

template<class TL> BOOL LinkedList_cl<TL>::Remove(const TL& killvalue)
{
  if (IsEntry(killvalue)) 
  {
    Remove(found);
    found = NULL;
  } 

  // fail if not found
  return(FALSE);
}


template<class TL> void LinkedList_cl<TL>::Remove(LinkedList_Element_cl<TL> *remove)
{
#if defined HK_DEBUG_SLOW
  // check if the element to remove is part of this list
  LinkedList_Element_cl<TL> *check=first;
  while (check!=NULL) 
  {
    if (check==remove) 
    {
      // the element was found
      break;
    }
    check=check->next;
  }

  // if the element was found and remove was not NULL this should not be NULL
  VASSERT_MSG(check,"The element to be removed is either NULL or not part of the list!")
#endif

  if (remove->prev!=NULL) 
  {
    if (remove->next!=NULL)
    {
      remove->prev->next=remove->next;
    }
    else
    {
      remove->prev->next=NULL;
    }
  }

  if (remove->next!=NULL) 
  {
    if (remove->prev!=NULL)
    {
      remove->next->prev=remove->prev;
    }
    else
    {
      remove->next->prev=NULL;
    }
  }

  if (first==remove) first=remove->next;
  if (last==remove) last=remove->prev;
  delete remove;
  remove=NULL;
}


template<class TL> void LinkedList_cl<TL>::Clear()
{
  if (first==NULL)
    return;

  LinkedList_Element_cl<TL> *a = first,*n = first->next;
  while(a!=NULL) {
    //_nfree(a);
    delete a;
    a=NULL;
    if (n!=NULL) {
      a=n;
      if (a!=NULL) {
        if (a->next!=NULL) n=a->next; else n=NULL;
      }
    }
  }
  first=NULL;
  last=NULL;
}


template<class TL> BOOL LinkedList_cl<TL>::IsEmpty() const
{
  if (first==NULL) return(TRUE); else return(FALSE);
}


template<class TL> BOOL LinkedList_cl<TL>::IsEntry(const TL& searchvalue) const

{
  LinkedList_Element_cl<TL> *a;
  a=first;
  while (a!=NULL) {
    if (a->value==searchvalue) {
      found=a;
      return(TRUE);
    }
    a=a->next;
  }
  return(FALSE);
}

template<class TL> int LinkedList_cl<TL>::IntersectWith(const LinkedList_cl<TL> &other)
{
  //check if the current list has ant elements
  if(first==NULL) return 0;

  int iCount = 0;

  //intersection with an empty set results into an empty set
  if(other.IsEmpty())
  {
    iCount = GetSize();
    Clear();
  }

  //perform intersection
  LinkedList_Element_cl<TL> *pCurrent = first;

  while(pCurrent!=NULL)
  {
    if(!other.IsEntry(pCurrent->value))
    {
      LinkedList_Element_cl<TL> *pNext = pCurrent->next;
      Remove(pCurrent);
      pCurrent = pNext;

      iCount++;
    }
    else
    {
      pCurrent = pCurrent->next;
    }
  }
  
  return iCount;
}


#ifndef _VISION_DOC
template<class TL> LinkedList_Element_cl<TL>* LinkedList_Element_cl<TL>::Merge(LinkedList_Element_cl<TL>* other, typename LinkedList_Element_cl<TL>::CompareFunc compare)
{
  LinkedList_Element_cl<TL>* l = this;
  LinkedList_Element_cl<TL>* r = other;
  LinkedList_Element_cl<TL>* m = NULL;

  while(l || r)
  {
    LinkedList_Element_cl<TL>*& n = !r ? l : (!l ? r : ((*compare)(*l, *r) <= 0) ? l : r);
    n->prev = m;
    if(m)
      m->next = n;
    m = n;
    n = n->next;  // Move to next element to compare.
  }

  return ((*compare)(*this, *other) <= 0) ? this : other;
}


template<class TL> LinkedList_Element_cl<TL>* LinkedList_Element_cl<TL>::Sort(typename LinkedList_Element_cl<TL>::CompareFunc compare)
{
  LinkedList_Element_cl<TL> *p = this, *m = this;  // m - will point to middle of (sub)list
  while(p && p->next)
  {
    p = p->next->next;
    m = m->next;
  }

  if(m != this)
  {
    // Cut the chain.
    m->prev->next = NULL;
    m->prev = NULL;

    // Recurse left and right list and merge.
    return Sort(compare)->Merge(m->Sort(compare), compare);
  }

  return this;
}
#endif // _VISION_DOC


template<class TL> void LinkedList_cl<TL>::Sort(typename LinkedList_Element_cl<TL>::CompareFunc compare)
{
  if(first)
  {
    first = first->Sort(compare);

    // Update pointer to last element.
    last = first;
    while(last->next) last = last->next;
  }
}


template<class TL> int LinkedList_cl<TL>::GetSize() const
{
  LinkedList_Element_cl<TL> *iterator;
  int nr = 0;
  
  iterator = first;
  while (iterator != NULL)
  {
    iterator = iterator->next;
    nr++;
  }
  return nr;
}


template<class TL> TL& LinkedList_cl<TL>::operator[] (int index)
{
  LinkedList_Element_cl<TL> *iterator;
  
  iterator = first;
  for (int i = 0; i < index; i++)
  {
    iterator = iterator->next;
  }
  return iterator->value;
}


template<class TL> const TL& LinkedList_cl<TL>::operator[] (int index) const
{
  LinkedList_Element_cl<TL> *iterator;
  
  iterator = first;
  for (int i = 0; i < index; i++)
  {
    iterator = iterator->next;
  }
  return iterator->value;
}


template<class TL> LinkedList_cl<TL> &LinkedList_cl<TL>::operator=(const LinkedList_cl<TL> &rhs)
{
  Clear();
  for(LinkedList_Element_cl<TL>* n=rhs.first; n; n=n->next)
    Add(n->value);

  return *this;
}


template<class TL> LinkedList_cl<TL>::~LinkedList_cl()
{
  Clear();
}


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
