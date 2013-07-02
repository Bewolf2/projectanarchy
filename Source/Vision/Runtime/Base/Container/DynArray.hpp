/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file dynarray.hpp

//*****************************************************************************************************
//DYNARRAY.HPP
//
//CONTAINS: DynArray_cl, DynObjArray_cl
//
//*****************************************************************************************************


#ifndef VISION_DYNARRAY_CL
#define VISION_DYNARRAY_CL

#include <Vision/Runtime/Base/System/Memory/VBaseMem.hpp>
#include <Vision/Runtime/Base/System/IO/Serialization/VArchive.hpp>


/// \brief
///   This class represents a dynamically-sized array for POD types. 
/// 
/// \note
///   The template parameter must at least implement a copy constructor, assignment operator and comparison operator.
/// 
/// \note
///   Use this template only with POD types, as constructors/destructors won't be called. For non-POD types, use DynObjArray_cl instead.
template <class TA> class DynArray_cl
{
  public:
  /// \brief
  ///   Constructor of the dynamic array class. The size for dynamic arrays is 0.
  DynArray_cl();

  DynArray_cl(unsigned int initSize, const TA &defaultElementValue);


  /// \brief
  ///   Constructor of the dynamic array class. The size of the array is given as an parameter
  /// 
  /// \param initSize
  ///   size of the dynamic array at initialization time.
  DynArray_cl(unsigned int initSize);


  /// \brief
  ///   Copy constructor
  /// 
  /// \param copyArray
  ///   The array to copy from
  DynArray_cl(const DynArray_cl<TA> &copyArray);


  /// \brief
  ///   Destructor of the dynamic array; releases the allocated memory
  virtual ~DynArray_cl();


  /// \brief
  ///   Sets the default value which will be used for empty elements and initializes the array
  /// 
  /// \param defaultElementValue
  ///   default value for array elements.
  inline void Init(const TA &defaultElementValue);


  /// \brief
  ///   Sets the default value for empty elements
  /// 
  /// \param defaultElementValue
  ///   default value for array elements. 
  inline void SetDefaultValue(const TA &defaultElementValue);


  /// \brief
  ///   Returns the default value for empty elements
  /// 
  /// \return
  ///   TA&: default value for array elements.
  inline const TA& GetDefaultValue() const;


  /// \brief
  ///   The assignment operator for the dynamic array class. Performs a deep copy.
  /// 
  /// \param copyArray
  ///   array where to copy the data from
  inline void operator=(const DynArray_cl &copyArray);


  /// \brief
  ///   Returns the element value with the given index
  /// 
  /// The array is resized automatically if necessary.
  /// 
  /// \param index
  ///   index of the array element (0 to size-1)
  /// 
  /// \return
  ///   TA &: element value
  inline TA &operator[](unsigned int index);
  
  /// \brief
  ///   Returns the element value with the given index as an l-value
  /// 
  /// \param index
  ///   index of the array element (0 to size-1)
  /// 
  /// \return
  ///   const TA &: element value
  inline const TA &operator[](unsigned int index) const;

  /// \brief
  ///   Resize the dynamic array to the new size
  /// 
  /// Data will be kept (i.e., copied over to the resized array) up to a maximum of newSize entries. New elements will be initialized with the default value.
  /// 
  /// \param newSize
  ///   new size of array
  /// 
  /// \note
  ///   If you resize the array to a smaller size than the current size, you will lose data at the end of the array.
  inline void Resize(unsigned int newSize);

  /// \brief
  ///   Ensures that the array has at least minSize elements. Resizes the array if necessary.
  inline void EnsureSize(unsigned int minSize);

  /// \brief
  ///   Frees all allocated data and resizes the array to 0 elements.
  inline void Reset();

  /// \brief
  ///   Adjust the size of the dynamic array to the currently used size. The currently used size is determined using GetValidSize().
  inline void AdjustSize();

  /// \brief
  ///   Returns the amount of allocated memory in bytes, i.e. array size * sizeof(TA)
  inline unsigned int GetMemSize() const;

  /// \brief
  ///   Copies a given number of elements within the array
  /// 
  /// \param destElement
  ///   index of the element the data will be copied to
  /// 
  /// \param sourceElement
  ///   index of the first element which will be copied
  /// 
  /// \param number
  ///   number of elements to copy
  inline void CopyElements(unsigned int destElement, unsigned int sourceElement, unsigned int number);


  /// \brief
  ///   Get the first available element in the array which is unused or currently empty.
  /// 
  /// If no empty element is found, the array will be resized to fit an additional element, and the index of this new element will be returned.
  /// An element will be considered empty if its value is different from the default value.
  ///
  /// \return
  ///   unsigned int: index of free element 
  inline unsigned int GetFreePos() const;


  /// \brief
  ///   Search for an element in the array
  /// 
  /// \param searchElement
  ///   Element value to search for
  /// 
  /// \return
  ///   int index: index of the element if it has been found; will return -1 if the element has not been found
  inline int GetElementPos(const TA &searchElement) const ;

  /// \brief
  ///   Search for an element in the array in the range from 0 to iNumEntries-1.
  ///
  /// \param searchElement
  ///   Element value to search for.
  ///
  /// \param iNumEntries
  ///   Number of elements in the array to search.
  /// 
  /// \return
  ///   int index: index of the element if it has been found; will return -1 if the element has not been found
  inline int GetElementPos(const TA &searchElement, int iNumEntries) const ;

  /// \brief
  ///   Returns the current size of the array
  /// 
  /// \return
  ///   int size: current physical size of the array
  /// 
  /// \note
  ///   This function returns the physical size of the array. The physical size is in most cases larger than the
  ///   currently used size, since the Resize function allocates an additional buffer (in order
  ///   to avoid resizes whenever one single element will be added). To get the size of the used range,
  ///   you can use the DynArray_cl::GetValidSize function.
  inline unsigned int GetSize() const;


  /// \brief
  ///   Returns the currently used size of the array
  /// 
  ///  This function returns the logical size of the array, which corresponds to the highest index at which the
  ///  value differs from the default value plus 1. 
  ///  The physical size may be larger than the logical size, as empty elements might exist at the end of the array.
  /// 
  /// \return
  ///   int size: currently used size of the array
  inline unsigned int GetValidSize() const;

  /// \brief
  ///   Returns a pointer to the data array
  /// 
  /// This function is useful if you want to step through the array without the [] operator which
  /// performs  a range check (which is not necessary if you only access the range returned by GetSize()
  /// 
  /// \return
  ///   TA*: A pointer to the array of elements. Can be NULL if there are no elements 
  /// 
  /// \example
  ///   \code
  ///   int count = nameList.GetValidSize();
  ///   char **pNames = nameList.GetDataPtr();
  ///   for (int i=0;i<count;i++) 
  ///     if (pNames[i]) 
  ///     {
  ///       free(pNames[i]); 
  ///       pNames[i]=NULL;
  ///     }
  ///   \endcode
  inline TA* GetDataPtr() const;

  /// \brief
  ///   Returns the n-th element. Does not perform range checks.
  inline TA& Get(int i) const;

  /// \brief
  ///   Sets the element with the given index to the default value.
  /// 
  /// This element is then free again and can be returned by the function DynArray_cl::GetFreePos. Does not rearrange the array in any way.
  /// 
  /// \param index
  ///   index of the element to be removed
  inline void Remove(unsigned int index);

  /// \brief
  ///   Return whether an element is valid or not
  /// 
  /// An element is valid as long as its value is different from the default value.
  ///
  /// \param index
  ///   Index of the element to be checked
  /// 
  /// \return
  ///   BOOL valid: if TRUE then the element is valid, otherwise the element is empty.
  inline BOOL IsValid(unsigned int index) const;


  /// \brief
  ///   Returns whether the dynamic array is currently initialized (i.e., its internal data has been allocated) or not
  /// 
  /// \return
  ///   BOOL initialized: if TRUE the array is initialized
  inline BOOL IsInitialized() const;

  /// \brief
  ///   Rearranges the elements of the array in random order
  /// 
  /// \param startPos
  ///   Entry position to start from
  /// 
  /// \param numEntries
  ///   Number of entries to randomize, -1 means all remaining from startPos on
  void Randomize(unsigned int startPos=0, unsigned int numEntries=(unsigned int)-1);

  /// \brief
  ///   Packs this array, so that all valid elements form a contiguous block
  ///   at the start of the array.
  ///
  /// The order of the elements is preserved, but their indices may change
  /// if they are moved in order to fill gaps. All elements after the last
  /// valid element are set to the default value.
  ///
  /// \return
  ///   the number of valid elements in the array
  unsigned int Pack();

protected:
  static unsigned int GetNewAllocSize(unsigned int iCurrentSize, unsigned int iRequiredSize);

  unsigned int size;   ///< physical size of the array
  TA *data;   ///< pointer to the array data
  TA defaultValue;    ///< default value of the array elements
};

//*****************************************************************************************************
//NEW CLASS:                   TEMPLATE DYNAMIC ARRAY CLASS 
//*****************************************************************************************************


template<class TA> DynArray_cl<TA>::DynArray_cl(unsigned int initSize)
{
  size = initSize;
  if (size > 0)
    data = (TA*)VBaseAlloc(sizeof(TA)*size);
  else
    data = NULL;
}

template<class TA> DynArray_cl<TA>::DynArray_cl(const DynArray_cl<TA> &copyArray)
{
  size = copyArray.size;
  if (size > 0)
  {
    data = (TA*)VBaseAlloc(sizeof(TA)*size);
    for (unsigned int i = 0; i < copyArray.size; i++)
      data[i] = copyArray.data[i];
  }
  else
  {
    data = NULL;
  }
}

template<class TA> DynArray_cl<TA>::DynArray_cl(unsigned int initSize, const TA &defaultElementValue)
{
  size = initSize;
// cant be less then zero  if (size < 0) size = 0;
  if (size > 0)
    data = (TA*)VBaseAlloc(sizeof(TA)*size);
  else
    data = NULL;
  Init(defaultElementValue);
}

template<class TA> DynArray_cl<TA>::DynArray_cl()
{
  size = 0;
  data = NULL;
}

template<class TA> DynArray_cl<TA>::~DynArray_cl()
{
  if (data)
    VBaseDealloc((void*) data);

  data = NULL;
}

template<class TA> void DynArray_cl<TA>::operator=(const DynArray_cl<TA> &copyArray)
{
  Resize(copyArray.size);
  for (unsigned int i = 0; i < copyArray.size; i++)
    data[i] = copyArray.data[i];
}


template<class TA> void DynArray_cl<TA>::Init(const TA &defaultElementValue)
{
  for (unsigned int i = 0; i < size; i++)
    data[i] = defaultElementValue;
  defaultValue = defaultElementValue;
}

template<class TA> void DynArray_cl<TA>::SetDefaultValue(const TA &defaultElementValue)
{
  defaultValue = defaultElementValue;
}

template<class TA> const TA& DynArray_cl<TA>::GetDefaultValue() const
{
  return defaultValue;
}

template<class TA> void DynArray_cl<TA>::Resize(unsigned int newSize)
{
  if (newSize==size) return;

  TA *tempData;
  int copySize;

  if (newSize > size)
    copySize = size;
  else
    copySize = newSize;
  tempData = data;
  if (newSize > 0)
    data = (TA*)VBaseAlloc(sizeof(TA)*newSize);
  else
    data = NULL;
  size = newSize;
  Init(defaultValue);
  if ( (size > 0) && (tempData != NULL) )
  {
    for (int i = 0; i < copySize; i++)
      data[i] = tempData[i];
  }
  if (tempData)
    VBaseDealloc((void*)tempData);
}


template<class TA> void DynArray_cl<TA>::EnsureSize(unsigned int minSize)
{
  if (minSize>size) 
    Resize(minSize);
}


template<class TA> void DynArray_cl<TA>::Reset()
{
  if (data) VBaseDealloc(data);
  data = NULL;
  size = 0;
}


template<class TA> void DynArray_cl<TA>::AdjustSize()
{
  Resize(GetValidSize());
}

template<class TA> unsigned int DynArray_cl<TA>::GetMemSize() const
{
  return size * sizeof(TA);
}


template<class TA> void DynArray_cl<TA>::CopyElements(unsigned int destElement, unsigned int sourceElement, unsigned int number)
{
  unsigned int copynum = number;
  if (sourceElement+number > size)
    copynum = size-sourceElement;
  if (destElement+number > size)
    Resize(GetNewAllocSize(size, destElement+number));

  if (copynum>0)
    memmove(&data[destElement], &data[sourceElement], sizeof(TA)*copynum);

  // fill up the rest with default-value
  while (copynum<number)
    data[copynum++] = defaultValue;
}

template<class TA> unsigned int DynArray_cl<TA>::GetFreePos() const
{
  for (unsigned int i = 0; i < size; i++)
  {
    if ( defaultValue == data[i])
      return i;
  }
  return size;
}

template<class TA> int DynArray_cl<TA>::GetElementPos(const TA &searchElement) const
{
  for (unsigned int i = 0; i < size; i++)
  {
    if (searchElement == data[i])
      return i;
  }
  return -1;
}

template<class TA> int DynArray_cl<TA>::GetElementPos(const TA &searchElement, int iNumEntries) const
{
  unsigned int iSize = iNumEntries;
  if (size < iSize)
    iSize = size;
  for (unsigned int i = 0; i < iSize; i++)
  {
    if (searchElement == data[i])
      return i;
  }
  return -1;
}

template<class TA> unsigned int DynArray_cl<TA>::GetSize() const
{
  return(size);
}


template<class TA> unsigned int DynArray_cl<TA>::GetValidSize() const
{
  unsigned int validSize = 0;
  for (unsigned int i = 0; i < size; i++)
  {
    if (data[i] != defaultValue)
      validSize = i + 1;
  }
  return(validSize);
}

template<class TA> TA* DynArray_cl<TA>::GetDataPtr() const
{
  return data;
}

template<class TA> TA& DynArray_cl<TA>::Get(int i) const
{
  return data[i];
}


// increase array size by doubling; start with at least 16 bytes
template<class TA> unsigned int DynArray_cl<TA>::GetNewAllocSize(unsigned int iCurrentSize, unsigned int iRequiredSize)
{
  return VPointerArrayHelpers::GetAlignedElementCount((int)iCurrentSize, (int)iRequiredSize);
}

template<class TA> TA& DynArray_cl<TA>::operator[] (unsigned int index)
{
  if (index >= size) {
    Resize(GetNewAllocSize(size,index+1));
  }    
  return data[index];
}

template<class TA> const TA& DynArray_cl<TA>::operator[] (unsigned int index) const
{
  VASSERT(index < size);
  return data[index];
}

template<class TA> void DynArray_cl<TA>::Remove(unsigned int index)
{
  data[index] = defaultValue;
}

template<class TA> BOOL DynArray_cl<TA>::IsValid(unsigned int index) const
{
  return (data[index] != defaultValue);
}

template<class TA> BOOL DynArray_cl<TA>::IsInitialized() const
{
  return (data != NULL);
}

template<class TA> void DynArray_cl<TA>::Randomize(unsigned int startPos, unsigned int numEntries)
{
  if (numEntries==(unsigned int)-1)
    numEntries = size;

  if (startPos+numEntries>size)
    numEntries = size - startPos;

  if (startPos >= size)
    return;

  if (numEntries < 2)
    return;

  // for position N in the array, pick a random one from 0 to N
  
  for (unsigned int n = numEntries; n>0; n--)
  {
    // pick one item from the remaining ordered ones:
    int newpos = rand() % n;
      
    // swap the two items:
    TA temp = data[startPos + n-1];
    data[startPos + n-1] = data[startPos + newpos];
    data[startPos + newpos] = temp;    
  }
}

template<class TA> unsigned int DynArray_cl<TA>::Pack()
{
  // Find the first hole in the list. If the list is completely full of valid
  // entries, return immediately.
  unsigned int iFirstFree = GetFreePos();
  if (iFirstFree == size)
    return size;

  // Iterate over all elements following the first free element, and move any
  // valid elements found to the front.
  for (unsigned int iTest = iFirstFree + 1; iTest < size; ++iTest)
  {
    if (defaultValue != data[iTest])
    {
      data[iFirstFree] = data[iTest];
      data[iTest] = defaultValue;
      iFirstFree++;
    }
  }

  // The first free index is equal to the number of valid entries; return it.
  return iFirstFree;
}



/// \brief
///   Special version of DynArray_cl with support for serialization
/// 
/// Use this class only with pointers (and primitive data types), because it doesn't call the
/// object constructors/destructors for speed reasons!
template <class TA> class SerialDynArray_cl : public DynArray_cl<TA>
{
public:

  /// \brief
  ///   Default constructor. Used for deserialization.
  SerialDynArray_cl() : DynArray_cl<TA>() {}

  /// \brief
  ///   Constructor which takes the initial size and default value
  SerialDynArray_cl(int initSize, const TA defaultElementValue) : DynArray_cl<TA>(initSize, defaultElementValue) {}

  /// \brief
  ///   Constructor which takes the initial size
  SerialDynArray_cl(int initSize) : DynArray_cl<TA>(initSize) {}

  /// \brief
  ///   Destructor
  virtual ~SerialDynArray_cl() {}

  void SerializeX( VArchive &ar );
  
  /// \brief
  ///   Serialization operator
  template<class C> friend VArchive& operator <<( VArchive &ar, SerialDynArray_cl<C>& v );

  /// \brief
  ///   Serialization operator
  template<class C> friend VArchive& operator <<( VArchive &ar, SerialDynArray_cl<C>* v );

  /// \brief
  ///   Serialization operator
  template<class C> friend VArchive& operator >>( VArchive &ar, SerialDynArray_cl<C>& v );

  /// \brief
  ///   Serialization operator
  template<class C> friend VArchive& operator >>( VArchive &ar, SerialDynArray_cl<C>* v );

};


// Notes: Only objects that are valid get serialized. This can cause certain items'
//        indices to change after loading. Please take special care not to rely on
//        and serialize fixed indices into a SerialDynArray_cl.
template<class TA> void SerialDynArray_cl<TA>::SerializeX( VArchive &ar )
{
  if ( ar.IsLoading() )
  {
    int newSize;
    ar >> newSize;

    this->Resize(newSize);

    for (int i = 0; i < newSize; i++)
      ar >> this->data[i];
  }
  else
  {
    ar << this->GetValidSize();
    
    for (unsigned int i = 0; i < this->size; i++)
    {
      if (this->data[i] != this->defaultValue)
        ar << this->data[i];
    }
  }
}

template<class TA> VArchive& operator<<( VArchive &ar, SerialDynArray_cl<TA>& obj)
{
  obj.SerializeX( ar );
  return ar;
}

template<class TA> VArchive& operator>>( VArchive &ar, SerialDynArray_cl<TA>& obj)
{
  obj.SerializeX( ar );
  return ar;
}

template<class TA> VArchive& operator<<( VArchive &ar, SerialDynArray_cl<TA>* obj)
{
  obj->SerializeX( ar );
  return ar;
}

template<class TA> VArchive& operator>>( VArchive &ar, SerialDynArray_cl<TA>* obj)
{
  obj->SerializeX( ar );
  return ar;
}


/// \brief
///   This class represents a dynamically-sized array for non-POD types. 
/// 
/// \note
///   The template parameter must at least implement a copy constructor, assignment operator and comparison operator.
/// 
/// \note
///   Use this template for non-POD types. For POD types, use DynArray_cl instead.
template <class TA> class DynObjArray_cl
{
  public:

  /// \brief
  ///   Constructor of the dynamic array class. The default size for dynamic arrays is 0.
  DynObjArray_cl();

  /// \brief
  ///   Constructor of the dynamic array class that allows specifying initialization size and default value.
  DynObjArray_cl(unsigned int initSize, const TA &defaultElementValue);

  /// \brief
  ///   Constructor of the dynamic array class. The size of the array is given as a parameter
  DynObjArray_cl(unsigned int initSize);


  /// \brief
  ///   Destructor of the dynamic array; releases the allocated memory
  virtual ~DynObjArray_cl();


  /// \brief
  ///   Sets the default value which will be used for empty elements and initializes the array
  /// 
  /// \param defaultElementValue
  ///   default value for array elements.
  inline void Init(const TA &defaultElementValue);


  /// \brief
  ///   Set the default value for empty elements
  inline void SetDefaultValue(const TA &defaultElementValue);


  /// \brief
  ///   Returns the default value for empty elements
  inline TA& GetDefaultValue();

  /// \brief
  ///   The assignment operator for the dynamic array class. Performs a deep copy of the array.
  inline void operator=(const DynObjArray_cl &copyArray);

  /// \brief
  ///   Returns the element value with the given index. The array is resized automatically if necessary.
  inline TA &operator[](unsigned int index);
  
  /// \brief
  ///   Resize the dynamic array to the new size
  /// 
  /// Data will be kept (i.e., copied over to the resized array) up to a maximum of newSize entries. New elements will be initialized with the default value.
  /// If you resize the array to a smaller size than the current size, you will lose data at the end of the array.
  ///
  /// \param newSize
  ///   new size of array
  inline void Resize(unsigned int newSize);

  /// \brief
  ///   Frees all allocated data and resizes the array to 0 elements.
  inline void Reset();

  /// \brief
  ///   Adjust the size of the dynamic array to the currently used size. The currently used size is determined using GetValidSize().
  inline void AdjustSize();

  /// \brief
  ///   Ensures that the array has at least minSize elements. Resizes the array if necessary.
  inline void EnsureSize(unsigned int minSize) {if (minSize>size) Resize(minSize);}

  /// \brief
  ///   Returns the amount of allocated memory in bytes, i.e. array size * sizeof(element type)
  /// 
  /// \return
  ///   Number of bytes allocated
  inline unsigned int GetMemSize() const;

  /// \brief
  ///   Copies a given number of elements within the array
  /// 
  /// \param destElement
  ///   Index of the element the data will be copied to
  /// 
  /// \param sourceElement
  ///   Index of the first element which will be copied
  /// 
  /// \param number
  ///   Number of elements to copy
  inline void CopyElements(unsigned int destElement, unsigned int sourceElement, unsigned int number);


  /// \brief
  ///   Returns the first available element in the array which is unused or currently empty.
  /// 
  /// If no empty element is found, the array will be resized to fit an additional element, and the index of this new element will be returned.
  /// An element will be considered empty if its value is different from the default value.
  ///
  /// \return
  ///   unsigned int: index of free element 
  inline unsigned int GetFreePos() const;


  /// \brief
  ///   Searches for an element in the array
  /// 
  /// \param searchElement
  ///   Element value to search for
  /// 
  /// \return
  ///   int index: index of the element if it has been found; will return -1 if the element has not
  ///   been found
  inline int GetElementPos(const TA &searchElement) const ;


  /// \brief
  ///   Returns the current size of the array
  /// 
  /// \return
  ///   int size: current physical size of the array
  /// 
  /// \note
  ///   This function returns the physical size of the array. The physical size is in most cases larger than the
  ///   currently used size, since the Resize function allocates an additional buffer (in order
  ///   to avoid resizes whenever one single element will be added). To get the size of the used range,
  ///   you can use the DynArray_cl::GetValidSize function.
  inline unsigned int GetSize() const;


  /// \brief
  ///   Returns the currently used size of the array
  /// 
  ///  This function returns the logical size of the array, which corresponds to the highest index at which the
  ///  value differs from the default value plus 1. 
  ///  The physical size may be larger than the logical size, as empty elements might exist at the end of the array.
  /// 
  /// \return
  ///   int size: currently used size of the array
  inline unsigned int GetValidSize() const;

  /// \brief
  ///   Returns a pointer to the array data
  /// 
  /// This function is useful if you want to step through the array without the [] operator which
  /// performs  a range check (which is not necessary if you only access the range you got with
  /// GetSize()
  /// 
  /// \return
  ///   TA*: A pointer to the array of elements. Can be NULL if there are no elements 
  /// 
  /// \example
  ///   \code
  ///   int count = nameList.GetValidSize();
  ///   char **pNames = nameList.GetDataPtr();
  ///   for (int i=0;i<count;i++) 
  ///     if (pNames[i]) 
  ///     {
  ///       free(pNames[i]); 
  ///       pNames[i]=NULL;
  ///     }
  ///   \endcode
  inline TA* GetDataPtr() const;

  /// \brief
  ///   Returns the n-th element. Does not perform a range check.
  /// 
  /// \param i
  ///   Index of the element to be retrieved
  /// 
  /// \return
  ///   The n-th element
  inline TA& Get(int i) const;

  /// \brief
  ///   Sets the element with the given index to the default value.
  /// 
  /// This element is then free again and can be returned by the function DynArray_cl::GetFreePos. Does not rearrange the array in any way.
  /// 
  /// \param index
  ///   index of the element to be removed
  inline void Remove(unsigned int index);

  /// \brief
  ///   Return whether an element is valid or not
  /// 
  /// An element is valid as long as its value is different from the default value.
  ///
  /// \param index
  ///   Index of the element to be checked
  /// 
  /// \return
  ///   BOOL valid: if TRUE then the element is valid, otherwise the element is empty.
  inline BOOL IsValid(unsigned int index) const;

  /// \brief
  ///   Returns whether the dynamic array is currently initialized (i.e., its internal data has been allocated) or not
  /// 
  /// \return
  ///   BOOL initialized: if TRUE the array is initialized
  inline BOOL IsInitialized() const;

  /// \brief
  ///   Rearranges the elements of the array in random order
  /// 
  /// \param startPos
  ///   Entry position to start from
  /// 
  /// \param numEntries
  ///   Number of entries to randomize, -1 means all remaining from startPos on
  void Randomize(unsigned int startPos=0, unsigned int numEntries=(unsigned int)-1);


protected:
  static unsigned int GetNewAllocSize(unsigned int iCurrentSize, unsigned int iRequiredSize);

  unsigned int size;   ///< physical size of the array
  TA *data;   ///< pointer to the array data
  TA defaultValue;    ///< default value of the array elements
};

//*****************************************************************************************************
//NEW CLASS:                   TEMPLATE DYNAMIC ARRAY CLASS 
//*****************************************************************************************************


template<class TA> DynObjArray_cl<TA>::DynObjArray_cl(unsigned int initSize)
{
  size = initSize;
  if (size > 0)
  {
    data = (TA*)VBaseAlloc(sizeof(TA)*size);
    VConstructElements<TA>(data, initSize);
  }
  else
    data = NULL;
}


template<class TA> DynObjArray_cl<TA>::DynObjArray_cl(unsigned int initSize, const TA &defaultElementValue)
{
  size = initSize;
  if (size > 0)
  {
    data = (TA*)VBaseAlloc(sizeof(TA)*size);
    VConstructElements<TA>(data, initSize);
  }
  else
    data = NULL;
  Init(defaultElementValue);
}

template<class TA> DynObjArray_cl<TA>::DynObjArray_cl()
{
  size = 0;
  data = NULL;
}

template<class TA> DynObjArray_cl<TA>::~DynObjArray_cl()
{
  if (data)
  {
    VDestructElements<TA>(data, size);
    VBaseDealloc(data);
  }
  data = NULL;
}

template<class TA> void DynObjArray_cl<TA>::operator=(const DynObjArray_cl<TA> &copyArray)
{
  Resize(copyArray.size);
  for (unsigned int i = 0; i < copyArray.size; i++)
    data[i] = copyArray.data[i];
}


template<class TA> void DynObjArray_cl<TA>::Init(const TA &defaultElementValue)
{
  for (unsigned int i = 0; i < size; i++)
    data[i] = defaultElementValue;
  defaultValue = defaultElementValue;
}

template<class TA> void DynObjArray_cl<TA>::SetDefaultValue(const TA &defaultElementValue)
{
  defaultValue = defaultElementValue;
}

template<class TA> TA& DynObjArray_cl<TA>::GetDefaultValue()
{
  return defaultValue;
}

template<class TA> void DynObjArray_cl<TA>::Resize(unsigned int newSize)
{
  if (newSize==size) return;
//  if (newSize<0) newSize=0;

  TA *tempData;
  int copySize;

  if (newSize > size)
    copySize = size;
  else
    copySize = newSize;
  tempData = data;
  if (newSize > 0)
  {
    data = (TA*)VBaseAlloc(sizeof(TA)*newSize);
    VConstructElements<TA>(data, newSize);
  }    
  else
    data = NULL;
    
  int oldSize = size;
  size = newSize;
  Init(defaultValue);
  if ( (size > 0) && (tempData != NULL) )
  {
    for (int i = 0; i < copySize; i++)
      data[i] = tempData[i];
  }
  
  if (tempData)
  {
    VDestructElements<TA>(tempData, oldSize);
    VBaseDealloc(tempData);
  }    
}


template<class TA> void DynObjArray_cl<TA>::Reset()
{
  if (data)
  {
    VDestructElements<TA>(data, size);
    VBaseDealloc(data);
  }
  data = NULL;
  size = 0;
}


template<class TA> void DynObjArray_cl<TA>::AdjustSize()
{
  Resize(GetValidSize());
}

template<class TA> unsigned int DynObjArray_cl<TA>::GetMemSize() const
{
  return size * sizeof(TA);
}


template<class TA> void DynObjArray_cl<TA>::CopyElements(unsigned int destElement, unsigned int sourceElement, unsigned int number)
{
  unsigned int copynum = number;
  if (sourceElement+number > size)
    copynum = size-sourceElement;
  if (destElement+number > size)
    Resize(GetNewAllocSize(size, destElement+number));

  if (copynum>0)
    memmove(&data[destElement], &data[sourceElement], sizeof(TA)*copynum);

  // fill up the rest with default-value
  while (copynum<number)
    data[copynum++] = defaultValue;
}

template<class TA> unsigned int DynObjArray_cl<TA>::GetFreePos() const
{
  for (unsigned int i = 0; i < size; i++)
  {
    if ( defaultValue == data[i])
      return i;
  }
  return size;
}

template<class TA> int DynObjArray_cl<TA>::GetElementPos(const TA &searchElement) const
{
  for (unsigned int i = 0; i < size; i++)
  {
    if (searchElement == data[i])
      return i;
  }
  return -1;
}

template<class TA> unsigned int DynObjArray_cl<TA>::GetSize() const
{
  return(size);
}


template<class TA> unsigned int DynObjArray_cl<TA>::GetValidSize() const
{
  unsigned int validSize = 0;
  for (unsigned int i = 0; i < size; i++)
  {
    if (data[i] != defaultValue)
      validSize = i + 1;
  }
  return(validSize);
}

template<class TA> TA* DynObjArray_cl<TA>::GetDataPtr() const
{
  return data;
}

template<class TA> TA& DynObjArray_cl<TA>::Get(int i) const
{
  return data[i];
}

// increase array size by doubling; start with at least 16 bytes
template<class TA> unsigned int DynObjArray_cl<TA>::GetNewAllocSize(unsigned int iCurrentSize, unsigned int iRequiredSize)
{
  if (iCurrentSize<16)
    iCurrentSize = 16;

  while (iCurrentSize<iRequiredSize)
    iCurrentSize*=2;

  return iCurrentSize;
}

template<class TA> TA& DynObjArray_cl<TA>::operator[] (unsigned int index)
{
  if (index >= size) {
    Resize(GetNewAllocSize(size,index+1));
  }    
  return data[index];
}

template<class TA> void DynObjArray_cl<TA>::Remove(unsigned int index)
{
  data[index] = defaultValue;
}

template<class TA> BOOL DynObjArray_cl<TA>::IsValid(unsigned int index) const
{
  return (data[index] != defaultValue);
}

template<class TA> BOOL DynObjArray_cl<TA>::IsInitialized() const
{
  return (data != NULL);
}

template<class TA> void DynObjArray_cl<TA>::Randomize(unsigned int startPos, unsigned int numEntries)
{
  if (numEntries==(unsigned int)-1)
    numEntries = size;

  if (startPos+numEntries>size)
    numEntries = size - startPos;

  if (startPos >= size)
    return;

  if (numEntries < 2)
    return;

  // for position N in the array, pick a random one from 0 to N
  
  for (unsigned int n = numEntries; n>0; n--)
  {
    // pick one item from the remaining ordered ones:
    int newpos = rand() % n;
      
    // swap the two items:
    TA temp = data[startPos + n-1];
    data[startPos + n-1] = data[startPos + newpos];
    data[startPos + newpos] = temp;    
  }
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
