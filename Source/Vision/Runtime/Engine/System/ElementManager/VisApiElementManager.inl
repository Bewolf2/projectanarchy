/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiElementManager.inl



template<class ELEMTYPE>
VisElementManager_cl<ELEMTYPE>::VisElementManager_cl()
{
  m_iListIndex = VIS_INVALID;
  elementTable.SetDefaultValue(NULL);
}


template<class ELEMTYPE>
ELEMTYPE VisElementManager_cl<ELEMTYPE>::ElementManagerGet(unsigned int index)
{
  VASSERT(/*index>=0 &&*/ index<elementTable.GetSize());
  return elementTable.GetDataPtr()[index];
}


template<class ELEMTYPE>
ELEMTYPE VisElementManager_cl<ELEMTYPE>::ElementManagerGetAt(unsigned int index)
{
  return elementTable[index];
}


template<class ELEMTYPE>
int VisElementManager_cl<ELEMTYPE>::ElementManagerAdd(ELEMTYPE element)
{
  unsigned int newIndex = ElementManagerGetFreePos();
  elementTable[newIndex] = element;
  return newIndex;
}


template<class ELEMTYPE>
void VisElementManager_cl<ELEMTYPE>::ElementManagerRemove(unsigned int index)
{
  elementTable[index] = NULL;
  AdjustElementCounter();
}


template<class ELEMTYPE>
inline void VisElementManager_cl<ELEMTYPE>::AddToElementManager()       
{
  VASSERT(m_iListIndex==VIS_INVALID);
  m_iListIndex = ElementManagerAdd((ELEMTYPE)this);
}

template<class ELEMTYPE>
inline void VisElementManager_cl<ELEMTYPE>::RemoveFromElementManager()  
{
  VASSERT(m_iListIndex!=VIS_INVALID && ElementManagerGet(m_iListIndex)==((ELEMTYPE)this));
  ElementManagerRemove(m_iListIndex);
  m_iListIndex = VIS_INVALID;
}


template<class ELEMTYPE>
unsigned int VisElementManager_cl<ELEMTYPE>::ElementManagerGetSize()
{
//  return elementTable.GetSize();
  return g_iElementCount;
}


template<class ELEMTYPE>
unsigned int VisElementManager_cl<ELEMTYPE>::ElementManagerGetUsedCount()
{
  unsigned int iCount = 0;
  const unsigned int iMaxCount = elementTable.GetSize();

  for (unsigned int i = 0; i < iMaxCount; i++)
    if (elementTable.GetDataPtr()[i])
      iCount++;

  return iCount;
}




template<class ELEMTYPE>
unsigned int VisElementManager_cl<ELEMTYPE>::ElementManagerDeleteAll()
{
  unsigned int iRemoved = 0;
  const unsigned int iMaxCount = elementTable.GetSize();
  for (unsigned int i = 0; i <iMaxCount; i++)
    if (elementTable.GetDataPtr()[i])
    {
      iRemoved++;
      delete elementTable.GetDataPtr()[i];
      VASSERT(elementTable.GetDataPtr()[i] == NULL); ///< destructor should do that!
    }

  g_iElementCount = 0;
  ElementManagerInvalidateCache();
  return iRemoved;
}

template<class ELEMTYPE>
unsigned int VisElementManager_cl<ELEMTYPE>::ElementManagerDeleteAllUnRef(BOOL bResize)
{
  unsigned int iRemovedCount = 0;
  const unsigned int iMaxCount = elementTable.GetSize();
  for (unsigned int i = 0; i < iMaxCount; i++) 
  {
    ELEMTYPE pElem = elementTable.GetDataPtr()[i];
    if (!pElem || pElem->GetRefCount()>1) continue;
    pElem->DisposeObject();
    iRemovedCount++;
    VASSERT(elementTable.GetDataPtr()[i] == NULL); // destructor should have done that!
  }
  AdjustElementCounter(bResize);
  return iRemovedCount;
}

template<class ELEMTYPE>
ELEMTYPE VisElementManager_cl<ELEMTYPE>::FindByKey(const char *szObjectKey, DynArray_cl<ELEMTYPE> *storageArray, BOOL bIgnoreCase)
{
  const unsigned int iCount = elementTable.GetSize();
  ELEMTYPE pFirstHit = NULL;
  int iHitCount = 0;
  for (unsigned int i = 0; i < iCount; i++) 
  {
    ELEMTYPE pElem = elementTable.GetDataPtr()[i];
    if (!pElem || !pElem->HasObjectKey(szObjectKey,bIgnoreCase))
      continue;
    if (!storageArray)
      return pElem;
    if (!pFirstHit)
      pFirstHit = pElem;
    (*storageArray)[iHitCount++] = pElem;
  }
  if (storageArray)
    storageArray->Resize(iHitCount);
  return pFirstHit;
}


template<class ELEMTYPE>
ELEMTYPE VisElementManager_cl<ELEMTYPE>::FindByUniqueID(__int64 iUniqueID)
{
  const unsigned int iCount = elementTable.GetSize();
  for (unsigned int i=0; i<iCount; i++)
  {
    ELEMTYPE pElem = elementTable.GetDataPtr()[i];
    if (pElem && pElem->GetUniqueID()==iUniqueID)
      return pElem;
  }
  return NULL;
}


template<class ELEMTYPE>
void VisElementManager_cl<ELEMTYPE>::AdjustElementCounter(BOOL bResize)
{
  bool bNeedsInvalidation = false;
  while (g_iElementCount>0 && elementTable[g_iElementCount-1]==NULL)
  {
    g_iElementCount--;
    bNeedsInvalidation = true;
  }
  if (bResize)
  {
    elementTable.Resize(g_iElementCount);
    bNeedsInvalidation = true;
  }
  if (bNeedsInvalidation)
    ElementManagerInvalidateCache();
}

template<class ELEMTYPE>
void VisElementManager_cl<ELEMTYPE>::ElementManagerPopulateFreeElementCache()
{
  g_iNumElementsInCache = 0;
  g_iCurrentElementCacheIndex = 0;
  const unsigned int iCount = elementTable.GetSize();
  for (unsigned int i=0; i<iCount; i++)
  {
    if (elementTable.GetDataPtr()[i]==NULL)
    {
      g_iFreeElementCache[g_iNumElementsInCache++] = i;
      if (g_iNumElementsInCache == VIS_ELEMENTMANAGER_FREE_INDEX_CACHE_SIZE)
        break;
    }
  }
}

template<class ELEMTYPE>
int VisElementManager_cl<ELEMTYPE>::ElementManagerGetFreePos()
{
  if (g_iCurrentElementCacheIndex==g_iNumElementsInCache)
  {
    ElementManagerPopulateFreeElementCache();
    VASSERT_MSG (g_iResizeGranularity <= VIS_ELEMENTMANAGER_FREE_INDEX_CACHE_SIZE,
      "g_iResizeGranularity must NEVER be set greater than VIS_ELEMENTMANAGER_FREE_INDEX_CACHE_SIZE!");

    if (g_iNumElementsInCache < g_iResizeGranularity)
    {
      unsigned int iSizeAdd = elementTable.GetSize()/4;
      if (iSizeAdd < g_iResizeGranularity)
        iSizeAdd = g_iResizeGranularity;
      ElementManagerEnsureSize(elementTable.GetSize() + iSizeAdd);
      if (g_iCurrentElementCacheIndex==g_iNumElementsInCache)
        ElementManagerPopulateFreeElementCache();
      VASSERT(g_iNumElementsInCache>0);
    }
  }

  VASSERT(g_iCurrentElementCacheIndex < VIS_ELEMENTMANAGER_FREE_INDEX_CACHE_SIZE);
  unsigned int iFreeEntry = g_iFreeElementCache[g_iCurrentElementCacheIndex++];
  if (iFreeEntry >= g_iElementCount)
    g_iElementCount = iFreeEntry+1;

  return iFreeEntry;
}

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
