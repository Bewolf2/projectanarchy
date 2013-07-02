/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VPList.hpp

#ifndef DC_VISION_VPLIST_HPP
#define DC_VISION_VPLIST_HPP

#include <Vision/Runtime/Base/Types/VType.hpp>

/// \brief
///   A List (dynamic array) of void pointers
/// 
/// This class additionally supports RTTI information since it inherits from VTypedObject.
/// When the list is destroyed the array data is freed, but the user is responsible for freeing the objects.
/// When the array is dynamically resized, it doubles the amount of available pointers every time to avoid frequent re-allocation.
/// This array class holds two counter variables: First, the number of allocated pointers and second, the number of used pointers.
/// This class represents the base class for the VRefCountedCollection template class
class VPList : public VTypedObject
{
  V_DECLARE_DYNAMIC_DLLEXP(VPList,VBASE_IMPEXP)
public:
  typedef int (VISION_CDECL *CompareFunc)(const void* elem1, const void* elem2);

  ///
  /// @name Init/DeInit
  /// @{
  ///

  /// \brief
  ///   Constructor
  VBASE_IMPEXP VPList();

  /// \brief
  ///   Copy constructor
  VBASE_IMPEXP VPList( const VPList &pl );

  /// \brief
  ///   Constructor that pre-allocates a number of pointers
  VBASE_IMPEXP VPList( int initSize);

  /// \brief
  ///   Destructor. Releases the pointer list but does not free the array elements
  VBASE_IMPEXP virtual ~VPList();

  ///
  /// @}
  ///


  ///
  /// @name Methods
  /// @{
  ///

  /// \brief
  ///   Same as Append
  VBASE_IMPEXP void Push(void *ptr){  Append(ptr);}

  /// \brief
  ///   Removes the last object and returns it
  VBASE_IMPEXP void *Pop(){ if (nPtrs<=0) return NULL; nPtrs--; return ptrs[ nPtrs ]; }

  /// \brief
  ///   Appends a pointer. Increases the array if neccessary
  VBASE_IMPEXP void Append( void *ptr );

  /// \brief
  ///   Appends all elements from the passed list
  VBASE_IMPEXP void Append( VPList &other );

  /// \brief
  ///   Returns the index of the first occurrence of the passed pointer (or -1 if not found)
  VBASE_IMPEXP int Find( const void *ptr ) const;

  /// \brief
  ///   Inserts the passed element into the specified position
  VBASE_IMPEXP void Insert( void *ptr, int addBefore );

  /// \brief
  ///   Merges two lists: Attaches elements from the other collection and clears that one
  VBASE_IMPEXP void Attach(VPList &other);
    
  /// \brief
  ///   Appends the passed pointer if it is not already in the list. Returns true if it has been added
  inline bool AddUnique( void* ptr )
  {
    if( Find( ptr ) >= 0 ) return false;
    Append( ptr );
    return true;
  }


  /// \brief
  ///   Replaces pointer at specified index. Index must be in valid range
  inline void SetAt( void *ptr, int which)
  {
    VASSERT(which < nPtrs && which >= 0);
    ptrs[which] = ptr;
  }

  /// \brief
  ///   Sets pointer at free position (i.e., at the first occurance of a NULL pointer or at the end of the array). Returns index (ALWAYS successful); resizes the array if necessary.
  VBASE_IMPEXP int     SetAtFreePos(void *ptr);

  /// \brief
  ///   Remove pointer at index. The elements after that will be moved and the array size is decreased by 1
  VBASE_IMPEXP void    RemoveAt( int which );

  /// \brief
  ///   Same as RemoveAt(Find(ptr)); thus it asserts when the pointer is not in the list
  inline void Remove( const void *ptr )
  {
    int id = Find(ptr);
    VASSERT(id>=0);
    RemoveAt(id);
  }


  /// \brief
  ///   Set the passed index to NULL. Index must be in valid range
  inline void FlagForRemovalAt(int which)
  {
    VASSERT(which >= 0 && which < nPtrs);
    ptrs[which] = NULL;
  }

  /// \brief
  ///   Same as FlagForRemovalAt(Find(ptr)); thus it asserts when the pointer is not in the list
  inline void FlagForRemoval(const void *ptr)
  {
    FlagForRemovalAt(Find(ptr));
  }

  /// \brief
  ///   Removes all occurrences of NULL from the list. Can be used for batch removal (see FlagForRemovalAt)
  VBASE_IMPEXP void    RemoveFlagged(void);

  /// \brief
  ///   Discards all NULL pointers at the end. GetLength will return correct length again. Will resort list or reallocate
  VBASE_IMPEXP void    AdjustUpperLimit();

  /// \brief
  ///   Ensures the collection has capacity for iCount elements so elements can be added with no allocation. This function does not modify the Length().
  VBASE_IMPEXP void    EnsureCapacity(int iCount);

  /// \brief
  ///   Sets the count that is returned by GetLength and also ensures enough capacity
  VBASE_IMPEXP void    SetLength(int iCount);

  /// \brief
  ///   Returns raw pointer array
  inline void  **GetPtrs() const { return ptrs; }

  /// \brief
  ///   Returns the number of used pointers
  inline int GetLength() const { return nPtrs; }

  /// \brief
  ///   Sets a new number of pointers. Truncating to 0 deletes the array. The new number must not be larger than the allocated number.
  VBASE_IMPEXP void Truncate( int start );

  /// \brief
  ///   Removes the last element from the list. No re-allocation takes place.
  VBASE_IMPEXP void Trim();

  /// \brief
  ///   Releases the array and sets the count to 0
  inline void Reset()
  {
    Truncate(0);
  }

  /// \brief
  ///   Sets the size of the list to 0 elements. Does not deallocate the array.
  inline void Clear()
  {
    nPtrs = 0;
  }

  /// \brief
  ///   Copies the elements from the passed collection. Elements that are already in this list are removed
  VBASE_IMPEXP void Copy( const VPList &pl );

  /// \brief
  ///   Accesses the element at specified index. Index must be in valid range.
  inline void *& Get(int i) const
  {
    VASSERT(i < nPtrs && i >= 0);
    return ptrs[ i ];
  }

  /// \brief
  ///   Determines whether the number of used pointers is 0 or not
  inline bool IsEmpty() {return (nPtrs==0);}

  /// \brief
  ///   Assignment operator: Copies the collection
  VBASE_IMPEXP VPList& operator =( const VPList &pl )
  {
    Copy( pl );
    return *this;
  }

  /// \brief
  ///   Array access operator. Index must be in valid range
  inline void *& operator []( int i ) const
  {
    VASSERT(i < nPtrs && i >= 0);
    return ptrs[ i ];
  }

  /// \brief
  ///   Comparison operator. Performs element-wise comparison
  VBASE_IMPEXP bool operator ==( const VPList &pl ) const;

  /// \brief
  ///   Comparison operator. Performs element-wise comparison
  inline bool operator !=( const VPList &pl ) const
  {
    return !(*this==pl);
  }

  /// \brief
  ///   Flips the order of the elements in the collection.
  VBASE_IMPEXP void Reverse();

  /// \brief
  ///   This function asserts as serialization cannot be performed on unknown object types. Derived classes may choose to implement this function properly.
  VBASE_IMPEXP virtual void Serialize( VArchive& ar ); 
  
  /// \brief
  ///   Provides >> serialization operator for derived classes.
  friend VBASE_IMPEXP VArchive& operator>>( VArchive& ar, VPList* &pObj );

  /// \brief
  ///   Returns whether the list is sorted (in ascending order according to pointer address).
  VBASE_IMPEXP bool IsSorted();

  /// \brief
  ///   Searches for an item in a sorted list and returns the index. Returns -1 if the item wasn't found. Assumes that the list is sorted.
  VBASE_IMPEXP int FindSorted(void *pPtr);

  /// \brief
  ///   Inserts an item into a sorted list. Assumes the list is sorted. If the pointer is in the list already, that index is returned.
  VBASE_IMPEXP int AddUniqueSorted(void *pPtr);
  
  /// \brief
  ///   Sort elements of the list
  /// 
  /// This method sorts list elements according to the specified comparison function
  /// 
  /// \param compare
  ///   Pointer to the comparison function. This function takes pointers to the two list
  ///   elements and compares their values. It returns:
  ///   \li 1 if the first element is greather than second one
  ///   \li 0 if the elements are equal
  ///   \li -1 if the first element is less than second one
  /// 
  /// \example
  ///   \code
  ///   int SortPrims( const void* a, const void* b )
  ///   {
  ///     VTranspPrim* pA = *(VTranspPrim**)a;
  ///     VTranspPrim* pB = *(VTranspPrim**)b;
  ///     if( pA->m_fDistance > pB->m_fDistance ) return 1;
  ///     if( pA->m_fDistance < pB->m_fDistance ) return -1;
  ///     return 0;
  ///   }
  ///   somefunc()
  ///   {
  ///     VPlist primList;
  ///     primList.Append( new VTransPrim(20) );
  ///     primList.Append( new VTransPrim(10) );
  ///     primList.Append( new VTransPrim(30) );
  ///     primList.Sort(SortPrims);
  ///   }
  ///   \endcode
  VBASE_IMPEXP void Sort(CompareFunc compare);


  ///
  /// @}
  ///


protected:
  void ** ptrs; ///< Array of pointers
  int   nPtrs;  ///< Used number of pointers
  int   ptrsSize; ///< Allocated number of pointers
  
  /// \brief
  ///   Internal function
  VBASE_IMPEXP void  Expand( int size );
};


// loading operator for a VPList
VBASE_IMPEXP VArchive& operator>>( VArchive& ar, VPList* &pObj );


#ifndef _VISION_DOC

/// \brief
///  This class wraps around VPList and provides a templated list
///
/// The wrapper functions simply perform casts and are inlined, so they disapear in release builds.
/// When the list is destroyed the array data is freed, but the user is responsible for freeing the objects. If desired, DeleteAll can be called to do this.
template<class C> class VPListT : private VPList
{
  //  V_DECLARE_DYNAMIC(VPListT);
public:
  /// \brief
  /// The comparator function template for the Sort() method.
  typedef int (VISION_CDECL *CompareFunc)(const C** elem1, const C** elem2);

  VPListT() : VPList() {}
  VPListT( const VPListT<C> &pl ) : VPList(pl) {}
  VPListT( int initSize) : VPList(initSize) {}

// Methods
  void Push(C *ptr) {VPList::Append(ptr);}
  C    *Pop(){ if (nPtrs<=0) return NULL; nPtrs--; return (C*)ptrs[ nPtrs ]; }

  void Append( C *ptr )             { VPList::Append(ptr);  }
  void Append( VPList &other )      { VPList::Append(other);}
  void Append( VPListT<C> &other )  { VPList::Append(other);}

  int Find( const C *ptr ) const { return VPList::Find(ptr);}
  void Insert( const C *ptr, int addBefore ) { return VPList::Insert( (void*) ptr,addBefore);}

//Attach a list to the end of this list. NB. Second list is empty after the attach
  void Attach(VPListT<C> &other) { VPList::Attach(other); }
  
  // -------------------------------------------------------------------------------
  // Method:  AddUnique
  // Purpose: If the input element doesn't exist in the list then add it to the end
  //          of the list. Otherwise do nothing.
  // Return:  TRUE if the input element has been added to the list by this method,
  //          i.e. no such element found in this list before adding this element,
  //          FALSE if the element has been already added to the list before
  // -------------------------------------------------------------------------------
  
  bool    AddUnique( C* ptr ) { return VPList::AddUnique( ptr ); }

  // sets pointer at index. Index must be in valid range
  void    SetAt( C *ptr, int which) { VPList::SetAt( ptr, which);  }
  // sets pointer at free position. First occurance of NULL pointer or appends item. Returns index (ALWAYS successful)
  int     SetAtFreePos(C *ptr)       { return VPList::SetAtFreePos(ptr);  }

  void    RemoveAt( int which )       { VPList::RemoveAt(which);  }
  void    Remove( const void *ptr )  { VPList::Remove(ptr);      }

  // Just flag for removal at (which)
  void    FlagForRemovalAt(int which)        { VPList::FlagForRemovalAt(which);}
  // Just flag the item (ptr) for removal from list
  void    FlagForRemoval(const C *ptr)        { VPList::FlagForRemoval(ptr);}
  // Purge list of all removed elements
  void    RemoveFlagged(void)         { VPList::RemoveFlagged();}
  // Discards all NULL pointers at the end. GetLength will return correct lenght again. Will *not* resort list or reallocate
  void    AdjustUpperLimit()          { VPList::AdjustUpperLimit();}
  // Ensures the collection has capacity for iCount elements so elements can be added with no allocation
  void    EnsureCapacity(int iCount)  {  VPList::EnsureCapacity(iCount); }

  C  **GetPtrs() const                { return (C**) VPList::GetPtrs(); }

  int     GetLength() const           { return VPList::GetLength(); }
  void    Truncate( int start )       { VPList::Truncate(start);}

  //Remove the last element from the list
  void    Trim()                      { VPList::Trim();}

  void Reset()                     { VPList::Reset();}
  void Copy( const VPListT<C> &pl ) { VPList::Copy( pl );}
  C *&    Get(int i) const            { return (C *&)VPList::Get(i);}
  bool IsEmpty()                   { return VPList::IsEmpty();}

  VPListT<C>& operator =( const VPListT<C> &pl ) { Copy( pl ); return *this; }
  C *&   operator []( int i ) const { return (C*&) VPList::operator [](i); }

  bool    operator ==( const VPListT<C> &pl ) const  { return VPList::operator ==(pl); }
  bool    operator !=( const VPListT<C> &pl ) const  { return VPList::operator !=(pl); }

  void    Reverse()                   { VPList::Reverse();}
  
  // It is _not_ possible to save pointers, but templated classes might be serializable
  // VPList::Serialize() will assert to prevent saving
  //virtual void Serialize( VArchive& ar ) { VPList::Serialize(ar); }
  
  //Provide >> operator for derived classes
  //friend VBASE_IMPEXP VArchive& operator>>( VArchive& ar, VPListT<C>* &pObj );

  //Returns whether the list is sorted (in ascending order)
  bool IsSorted()                     { return VPList::IsSorted(); }

  //Searches for an item in a sorted list and returns the index
  // Returns -1 if the item wasn't found
  //Precondition:  The list must be sorted 
  int FindSorted(C *pPtr)             { return VPList::FindSorted(pPtr); }

  //Insert an item into a sorted list
  //Precondition:  The list must be sorted 
  //Postcondition: The item has been inserted if it wasn't in the list.  The new list is also still sorted
  //returns the index of the element where it has been inserted OR been found
  int AddUniqueSorted(C *pPtr) { return VPList::AddUniqueSorted(pPtr); }
  
  /// \brief
  ///   sort elements of the list
  /// 
  /// this method sorts list elements according to the specified comparison function
  /// 
  /// \param compare
  ///   pointer to the comparison function. This function takes pointers to the two list
  ///   elements and compares their values. It returns:
  ///   \li 1 if the first element is greather than second one
  /// 
  ///   \li 0 if the elements are equal
  /// 
  ///   \li -1 if the first element is less than second one
  /// 
  /// \example
  ///   \code
  ///   int SortPrims( const VTranspPrim** a, const VTranspPrim** b )
  ///   {
  ///     VTranspPrim* pA = *a;
  ///     VTranspPrim* pB = *b;
  ///     if( pA->m_fDistance > pB->m_fDistance ) return 1;
  ///     if( pA->m_fDistance < pB->m_fDistance ) return -1;
  ///     return 0;
  ///   }
  ///   somefunc()
  ///   {
  ///     VPlist primList;
  ///     primList.Append( new VTransPrim(20) );
  ///     primList.Append( new VTransPrim(10) );
  ///     primList.Append( new VTransPrim(30) );
  ///     primList.Sort(SortPrims);
  ///   }
  ///   \endcode
  void Sort(CompareFunc compare)
  { VPList::Sort((VPList::CompareFunc)compare); }


  /// \brief
  ///   Delete all the objects in the list and reset the list
  void DeleteAll()
  {
    const int iCount = GetLength();
    for (int i=0; i<iCount; i++)
    {
      C* pObj = Get(i);
      if (pObj)
        delete pObj;
    }
    Reset();
  }
  
protected:
};

#endif


// We now have some nice macros for creating lists. 
// Visual C can't handle templates+dll exporting without completely
// messing up our precompiled headers!!

#define DECLARE_LIST(CLASS_NAME,PTR_NAME,IMPEXP)\
class CLASS_NAME : public VPList\
{\
public:\
  inline CLASS_NAME()                  {}\
  inline CLASS_NAME(int size) : VPList(size)     {}\
  inline CLASS_NAME(const CLASS_NAME &l) : VPList(l) {}\
  inline ~CLASS_NAME()               {}\
public:\
  void  Push(PTR_NAME *ptr){  VPList::Push(ptr);}\
  PTR_NAME* Pop(){  return (PTR_NAME*) VPList::Pop();}\
  IMPEXP void  Append(PTR_NAME *ptr);\
  IMPEXP void  Insert(PTR_NAME *ptr, int addBefore);\
  IMPEXP void  Remove(PTR_NAME *ptr);\
  IMPEXP void  Copy(const CLASS_NAME &l);\
  IMPEXP PTR_NAME*&Get(int i) const;\
  IMPEXP CLASS_NAME& operator =(const CLASS_NAME &l);\
  IMPEXP PTR_NAME* operator [](int i) const;\
  IMPEXP void  Set(int i, PTR_NAME *ptr);\
};


//Same as above but includes V_DECLARE_SERIAL and Serialize()
// NB!! Make sure to use the correct macros for Serialize and SerializeX!

#define DECLARE_LIST_SERIAL(CLASS_NAME,PTR_NAME,IMPEXP)\
class PTR_NAME;\
class CLASS_NAME : public VPList\
{\
  V_DECLARE_SERIAL_DLLEXP( CLASS_NAME, IMPEXP ) \
public:\
  inline CLASS_NAME()                  {}\
  inline CLASS_NAME(int size) : VPList(size)     {}\
  inline CLASS_NAME(const CLASS_NAME &l) : VPList(l) {}\
  inline ~CLASS_NAME()               {}\
public:\
  inline void  Push(PTR_NAME *ptr){  VPList::Push(ptr);}\
  inline PTR_NAME* Pop(){  return (PTR_NAME*) VPList::Pop();}\
  IMPEXP void  Append(PTR_NAME *ptr);\
  IMPEXP void  Insert(PTR_NAME *ptr, int addBefore);\
  IMPEXP void  Remove(PTR_NAME *ptr);\
  IMPEXP void  Copy(const CLASS_NAME &l);\
  IMPEXP PTR_NAME*&Get(int i) const;\
  IMPEXP CLASS_NAME& operator =(const CLASS_NAME &l);\
  IMPEXP PTR_NAME* operator [](int i) const;\
  IMPEXP void  Set(int i, PTR_NAME *ptr);\
  IMPEXP virtual void Serialize( VArchive& ar ); \
};

//There is no difference between DECLARE_LIST_SERIALX and DECLARE_LIST_SERIAL
//We still want the list itself to be Serial and not SerialX because we want 
//type info on the lists for serialization.
// We only declare both for completeness because IMPLEMENT_LIST_SERIALX is 
// different from DECLARE_LIST_SERIAL
#define DECLARE_LIST_SERIALX(CLASS_NAME,PTR_NAME,IMPEXP)\
        DECLARE_LIST_SERIAL(CLASS_NAME,PTR_NAME,IMPEXP)

// ------ begin of IMPLEMENT_LIST macro ------

#define IMPLEMENT_LIST(CLASS_NAME,PTR_NAME, PLUGIN_SRC)\
void CLASS_NAME::Append(PTR_NAME *ptr) {  VASSERT(ptr == NULL || V_IS_VALID_PTR(ptr));  VPList::Append(ptr);}\
void  CLASS_NAME::Insert(PTR_NAME *ptr, int addBefore){ VPList::Insert(ptr, addBefore);}\
void  CLASS_NAME::Remove(PTR_NAME *obj){  VPList::Remove( (const void* )obj);}\
void  CLASS_NAME::Copy(const CLASS_NAME &l){  VPList::Copy(l);}\
PTR_NAME* &CLASS_NAME::Get(int i) const{  return (PTR_NAME* &)VPList::Get(i);}\
CLASS_NAME&   CLASS_NAME::operator =(const CLASS_NAME &l){  return (CLASS_NAME&)VPList::operator =(l);}\
PTR_NAME*   CLASS_NAME::operator [](int i) const{ return (PTR_NAME*)VPList::operator [](i);}\
void  CLASS_NAME::Set(int i, PTR_NAME *ptr){  VPList::operator[](i) = ptr;}\

#define IMPLEMENT_LIST_SERIAL(CLASS_NAME,PTR_NAME, PLUGIN_SRC)\
IMPLEMENT_LIST(CLASS_NAME,PTR_NAME, PLUGIN_SRC);      \
V_IMPLEMENT_SERIAL(CLASS_NAME,VPList,1,PLUGIN_SRC);   \
                                                      \
void CLASS_NAME::Serialize( VArchive& ar )            \
{                                                     \
  VTypedObject::Serialize(ar);                        \
  if( ar.IsSaving() )                                 \
  {                                                   \
    int size = GetLength();                           \
    ar << size;                                       \
                                                      \
    for( int i = 0; i < size; i++ )                   \
      ar << (PTR_NAME*)Get( i );                      \
  }                                                   \
  else                                                \
  {                                                   \
    int size = 0;                                     \
    ar >> size;                                       \
                                                      \
    if( size > 0 )                                    \
    {                                                 \
      while( size-- )                                 \
      {                                               \
        PTR_NAME* ptr = (PTR_NAME*)0xdddddddd;        \
        ar >> ptr;                                    \
        Append( ptr );                                \
/*        VLog::Print("Added %x:  %x ("#PTR_NAME")\n",this,ptr); / *debug*/     \
      }                                               \
    }                                                 \
  }                                                   \
}                                             

// The main difference to IMPLEMENT_LIST_SERIAL is that 
// we have to create the instance we want to load
#define IMPLEMENT_LIST_SERIALX(CLASS_NAME,PTR_NAME, PLUGIN_SRC)\
IMPLEMENT_LIST(CLASS_NAME,PTR_NAME, PLUGIN_SRC)       \
V_IMPLEMENT_SERIAL(CLASS_NAME,VPList,1,PLUGIN_SRC)    \
                                                      \
void CLASS_NAME::Serialize( VArchive& ar )            \
{                                                     \
  VTypedObject::Serialize(ar);                        \
  if( ar.IsSaving() )                                 \
  {                                                   \
    int size = GetLength();                           \
    ar << size;                                       \
                                                      \
    for( int i = 0; i < size; i++ )                   \
      ar << (PTR_NAME*)Get( i );                      \
  }                                                   \
  else                                                \
  {                                                   \
    int size = 0;                                     \
    ar >> size;                                       \
                                                      \
    if( size > 0 )                                    \
    {                                                 \
      while( size-- )                                 \
      {                                               \
        PTR_NAME* ptr = new PTR_NAME;                 \
        ar >> ptr;                                    \
        Append( ptr );                                \
/*        VLog::Print("Added %x:  %x ("#PTR_NAME")\n",this,ptr);/ *debug*/     \
      }                                               \
    }                                                 \
  }                                                   \
}                                             


// ------ end of IMPLEMENT_LIST macro ------



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
