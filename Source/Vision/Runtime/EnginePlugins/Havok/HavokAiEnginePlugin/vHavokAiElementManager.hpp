/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vHavokAiElementManager.hpp

#ifndef V_HAVOKAI_ELEMENT_MANAGER_HPP_INCLUDED
#define V_HAVOKAI_ELEMENT_MANAGER_HPP_INCLUDED


#define V_HAVOKAI_ELEMENT_MANAGER_FREE_INDEX_CACHE_SIZE  128

#define ADDTOELEMENTMANAGER_ADDREF() \
  {\
    AddToElementManager();\
    this->AddRef();\
  }

#define REMOVEFROMELEMENTMANAGER_RELEASE() \
  {\
    RemoveFromElementManager();\
    this->Release();\
  }


/// \brief
///   Template class used for managing all instances of the same class.
/// 
/// This class offers a couple of functions to add and remove instances (=elements) from the
/// element manager. The elements are stored in a static array.
/// 
/// To use the element manager class you just derive the class you want to manage from the element
/// manager class and set the ELEMTYPE to the pointer of the class instance Additionally you have
/// to call the AddToElementManager function in the constructor to add the element to the static
/// element table, and the RemoveFromElementManager function in the destructor to remove it again.
/// The static members of this template class have to be defined in the cpp file of your class.
template<class ELEMTYPE>
class vHavokAiElementManager
{
public: 
  /// \brief
  ///   Constructor of the element manager
  /// 
  /// class, initializes the element table
  vHavokAiElementManager();

  inline virtual ~vHavokAiElementManager()
  {
    VASSERT_MSG(m_iListIndex==VIS_INVALID, "Object may not be deleted. Use DisposeObject instead");
  }

  /// \brief
  ///   Adds an element to the static element table and returns the index of the new entry.
  /// 
  /// This index is later necessary to remove the element from the table.
  /// 
  /// \param element
  ///   this element will be stored in the element table
  /// 
  /// \return
  ///   int index: index of the element in the table
  /// 
  /// \note
  ///   In most cases this function will be called in the constructor of the element in order to
  ///   register it in the static element table. This function does not physically add anything, it
  ///   only adds a reference (pointer) to the element table.
  /// 
  /// \note
  ///   This class has a non-static m_iListIndex member (list index for a single instance). This
  ///   index is set when calling the non-static AddToElementManager function
  static int ElementManagerAdd(ELEMTYPE element);


  /// \brief
  ///   Returns a certain element in the element table by index.
  /// 
  /// The index MUST be in the valid range of [0..ElementManagerGetSize()-1]
  /// 
  /// \param index
  ///   valid index of the element table entry to return
  /// 
  /// \return
  ///   ELEMTYPE retElement : The element at index. Can be a NULL pointer
  static ELEMTYPE ElementManagerGet(unsigned int index);


  /// \brief
  ///   Returns a certain element in the element table by index.
  /// 
  /// In this version the index can be larger than the maximum valid index given by ElementManagerGetSize()
  /// 
  /// \param index
  ///   index of the element table entry to return
  /// 
  /// \return
  ///   ELEMTYPE retElement : The element at the index. Can be a NULL pointer
  static ELEMTYPE ElementManagerGetAt(unsigned int index);


  /// \brief
  ///   Returns the overall size of the static element table.
  /// 
  /// A size of 8 means that the highest entry in the element table is 7, but it doesn't mean that all of the
  /// entries from 0 to 7 are actually defined (there might be entries which are undefined and therefore
  /// set to NULL).
  /// 
  /// \return
  ///   int size : Number of entries in the table
  /// 
  /// \sa vHavokAiElementManager::ElementManagerGetUsedCount
  static unsigned int ElementManagerGetSize();


  /// \brief
  ///   Returns the number of used elements in the element table.
  /// 
  /// The function iterates through all elements and counts the elements which are != NULL. 
  /// 
  /// Thus, the count returned by this method may be smaller than the value returned by
  /// ElementManagerGetSize.
  /// 
  /// \return
  ///   int count : number of valid entries, i.e. non-NULL entries
  /// 
  /// \sa vHavokAiElementManager::ElementManagerGetSize
  static unsigned int ElementManagerGetUsedCount();


  /// \brief
  ///   Removes one element from the element table.
  /// 
  /// ElementManagerRemove removes the element with the given index from the static element table,
  /// which means that it sets this entry to NULL.
  /// 
  /// \param index
  ///   index of the element which shall be removed. You get this index from the Add/Set functions
  /// 
  /// \note
  ///   This function does not delete the object. If you, for instance, store a sound sample in the
  ///   element table and remove it in the end with this function, the sound sample will be not be
  ///   destructed or freed, it will just not be referenced anymore in the static element table. In
  ///   most cases this function is called in the destructor of the element.
  static void ElementManagerRemove(unsigned int index);


  /// \brief
  ///   Deletes all the elements in the element table
  /// 
  /// ElementManagerDeleteAll takes all the currently registered elements in the element table and
  /// deletes them (actually calling the destructor). Use this function only if the elements in the
  /// tables are pointers to deletable objects.
  /// 
  /// Since the element manager is a template class and the code for deleting is inlined, it is
  /// guaranteed that the correct delete operator is used (in case it is overloaded)
  /// 
  /// \return
  ///   int iRemovedCount : number of deleted elements
  static unsigned int ElementManagerDeleteAll();

  /// \brief
  ///   Deletes all unreferenced resources in the list
  /// 
  /// , i.e. all resources that have a reference count of 0.
  /// 
  /// Assumes the elements are of type VRefCounter or derived from it. 
  /// 
  /// \param bResize
  ///   If TRUE, the element table will be resized to the number of remaining elements.
  /// 
  /// \return
  ///   int iRemovedCount : number of removed elements
  static unsigned int ElementManagerDeleteAllUnRef(BOOL bResize=TRUE);


  /// \brief
  ///   Adjusts the number of elements (returned by ElementManagerGetSize) by eliminating NULL
  ///   entries from the end of the table.
  /// 
  /// It will be adjusted so that there are no NULL elements at the end.
  /// 
  /// if the bResize flag is set, the array will additionally be resized to the new upper limit.
  /// 
  /// \param bResize
  ///   If TRUE, the element table will be set to new size
  static void AdjustElementCounter(BOOL bResize=FALSE);

  /// \brief
  ///   Adds this element instance to the element manager
  /// 
  /// sets the m_iListIndex member accordingly.
  inline void AddToElementManager();

  /// \brief
  ///   Removes this element instance from the element manager
  inline void RemoveFromElementManager();

  /// \brief
  ///   Returns the table index of this element in the element manager
  /// 
  /// \return
  ///   int iListIndex : The index in the element manager of this element. The index can be used as
  ///   input to the ElementManagerGet() method.
  inline int GetNumber() const            {return m_iListIndex;}

  /// \brief
  ///   Finds instance(s) with the specified key. Classes must implement the HasObjectKey function
  ///   (e.g. VisObject3D_cl)
  /// 
  /// This function is used by Vision::Game.SearchEntity, SearchPath, SearchLightSource for
  /// instance
  /// 
  /// \param szObjectKey
  ///   The key to search for
  /// 
  /// \param storageArray
  ///   Optional array to store results if there are more than one matches. Can be NULL
  /// 
  /// \param bIgnoreCase
  ///   Indicates whether case sensitive string comparison should be used.
  /// 
  /// \return
  ///   ELEMTYPE : The first hit, or NULL in case there is no object in the list with specified key
  static inline ELEMTYPE FindByKey(const char *szObjectKey, DynArray_cl<ELEMTYPE> *storageArray = NULL, BOOL bIgnoreCase=TRUE);

  /// \brief
  ///   Finds the instance with the specified unique ID. The template class must implement the GetUniqueID function
  static inline ELEMTYPE FindByUniqueID(__int64 iUniqueID);

  /// \brief
  ///   Internal function, wraps around elementTable.GetFreePos()
  static inline int ElementManagerGetFreePos();

  /// \brief
  ///   Returns the array index of the passed element (or -1 if not in list)
  static inline int ElementManagerIndexOf(ELEMTYPE elem)
  {
    for (unsigned int i=0;i<g_iElementCount;i++)
      if (elementTable.GetDataPtr()[i]==elem)
        return i;
    return -1;
  }

  /// \brief
  ///   Internal function, populates the free element cache.
  static inline void ElementManagerPopulateFreeElementCache();

  /// \brief
  ///   Internal function, ensures that the element table has at least the passed size.
  static inline void ElementManagerEnsureSize(int iSize) { if ((int)elementTable.GetSize() < iSize) { elementTable.Resize(iSize); } }

  /// \brief
  ///   Internal function, invalidates the free element cache.
  static inline void ElementManagerInvalidateCache() { g_iCurrentElementCacheIndex = 0; g_iNumElementsInCache = 0; }

private:
  VHAVOKAI_IMPEXP static DynArray_cl<ELEMTYPE> elementTable;           ///< static element table, used to store the elements
  VHAVOKAI_IMPEXP static unsigned int g_iElementCount;                 ///< current number of elements (returned by GetSize)
  VHAVOKAI_IMPEXP static unsigned int g_iFreeElementCache[];
  VHAVOKAI_IMPEXP static unsigned int g_iNumElementsInCache;
  VHAVOKAI_IMPEXP static unsigned int g_iCurrentElementCacheIndex;
  VHAVOKAI_IMPEXP static unsigned int g_iResizeGranularity;

public:
  unsigned int m_iListIndex;
};

#ifdef _VISION_WIIU
  #define V_HAVOKAI_ELEMENT_MANAGER_TEMPLATE_DECL(TYPE) \
  template<> static DynArray_cl<TYPE *> vHavokAiElementManager<TYPE *>::elementTable; \
  template<> static unsigned int vHavokAiElementManager<TYPE *>::g_iElementCount; \
  template<> static unsigned int vHavokAiElementManager<TYPE *>::g_iFreeElementCache[VIS_ELEMENTMANAGER_FREE_INDEX_CACHE_SIZE]; \
  template<> static unsigned int vHavokAiElementManager<TYPE *>::g_iNumElementsInCache; \
  template<> static unsigned int vHavokAiElementManager<TYPE *>::g_iCurrentElementCacheIndex; \
  template<> static unsigned int vHavokAiElementManager<TYPE *>::g_iResizeGranularity; \

#else
  #define V_HAVOKAI_ELEMENT_MANAGER_TEMPLATE_DECL(TYPE)
#endif

#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiElementManager.inl>

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
