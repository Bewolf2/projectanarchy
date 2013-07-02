/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VMaps.hpp

#ifndef DC_VISION_VMAPS_HPP
#define DC_VISION_VMAPS_HPP

#include <Vision/Runtime/Base/Container/VLink.hpp>

/// \brief
///   Templated map class
template <typename KEY_TYPE, typename VALUE_TYPE>
class VMap
{
protected:
  /// \brief
  ///   KeyValuePair helper struct
  struct VPair
  {
    VPair* pNext;

    KEY_TYPE key;
    VALUE_TYPE value;
  };

public:

  // Construction
  VMap(int nBlockSize = 10);
  VMap(const VMap& other);

  // Attributes
  // number of elements
  int GetCount() const;
  VBool IsEmpty() const;

  // Lookup
  VBool Lookup(KEY_TYPE key, VALUE_TYPE& rValue) const;
  VBool KeyExists(KEY_TYPE key) const;

  // Operations
  // Lookup and add if not there
  VALUE_TYPE& operator[](KEY_TYPE key);

  // add a new (key, value) pair
  // Purpose: this method replaces the value corresponding to the key by the value
  //          newValue. If the key is not included in the map then new key is created
  //          and its value is set to newValue.
  inline void SetAt(KEY_TYPE key, VALUE_TYPE newValue) { (*this)[key] = newValue; }

  // removing existing (key, ?) pair
  VBool RemoveKey(KEY_TYPE key);
  int RemoveValue(VALUE_TYPE value);
  void RemoveAll();
  void Reset() {RemoveAll();}
  void Clear(); // does not de-allocate

  // iterating all (key, value) pairs
  inline VPOSITION GetStartPosition() const { return (m_nCount == 0) ? NULL : VBEFORE_START_POSITION; }  
  void GetNextPair(VPOSITION& rNextPosition, KEY_TYPE& rKey, VALUE_TYPE& rValue) const;

  // advanced features for derived classes
  UINT GetHashTableSize() const;
  void InitHashTable(UINT hashSize, VBool bAllocNow = TRUE);

  // Overridables: special non-virtual (see map implementation for details)
  // Routine used to user-provided hash keys
  UINT HashKey(KEY_TYPE key) const;

  // Deep copy
  VMap& operator=(const VMap& other);


  // Implementation
protected:
  VPair** m_pHashTable;
  UINT m_nHashTableSize;
  int m_nCount;
  VPair* m_pFreeList;
  struct VLink* m_pBlocks;
  int m_nBlockSize;

  VPair* NewPair();
  void FreePair(VPair* , bool bDeallocOnEmpty=true);
  VPair* GetPairAt(KEY_TYPE, UINT&) const;

public:
  ~VMap();
#ifdef HK_DEBUG
  //  void Dump(CDumpContext&) const;
  void AssertValid() const;
#endif
};

// common map types
typedef VMap<void*, void*> VMapPtrToPtr;
typedef VMap<const void*, void*> VMapCPtrToPtr;
typedef VMap<const void*, const void*> VMapCPtrToCPtr;
typedef VMap<void*, int> VMapPtrToInt;
typedef VMap<const void*, int> VMapCPtrToInt;
typedef VMap<int, void*> VMapIntToPtr;
typedef VMap<int, int> VMapIntToInt;
typedef VMap<void*, unsigned int> VMapPtrToUInt;
typedef VMap<__int64, void*> VMapUniqueIDToPtr;

/////////////////////////////////////////////////////////////////////////////

template <typename KEY_TYPE, typename VALUE_TYPE>
inline VMap<KEY_TYPE, VALUE_TYPE>::VMap(int nBlockSize)
	: m_pHashTable(NULL)
	, m_nHashTableSize(17)  // default size
	, m_nCount(0)
	, m_pFreeList(NULL)
	, m_pBlocks(NULL)
{
	ASSERT(nBlockSize > 0);
	m_nBlockSize = nBlockSize;
}

template <typename KEY_TYPE, typename VALUE_TYPE>
inline VMap<KEY_TYPE, VALUE_TYPE>::VMap(const VMap& other)
	: m_pHashTable(NULL)
	, m_nHashTableSize(17)  // default size
	, m_nCount(0)
	, m_pFreeList(NULL)
	, m_pBlocks(NULL)
	, m_nBlockSize(other.m_nBlockSize)
{
	operator=(other);
}

template <typename KEY_TYPE, typename VALUE_TYPE>
inline int VMap<KEY_TYPE, VALUE_TYPE>::GetCount() const
{
	return m_nCount;
}

template <typename KEY_TYPE, typename VALUE_TYPE>
inline VBool VMap<KEY_TYPE, VALUE_TYPE>::IsEmpty() const
{
	return m_nCount == 0;
}

template <typename KEY_TYPE, typename VALUE_TYPE>
inline UINT VMap<KEY_TYPE, VALUE_TYPE>::HashKey(KEY_TYPE key) const
{
	// compute hash (key must have multiple of 4 size)
	V_COMPILE_ASSERT(sizeof(key)>=sizeof(UINT) && (sizeof(key)%sizeof(UINT)==0));
	const UINT *p = (const UINT *)&key;
	UINT res = 0;
	#pragma warning(suppress:6384)
	const int iCount = sizeof(key)/sizeof(UINT);
	for (int i=0;i<iCount;i++)
	  res ^= p[i];
	return res;
}

template <typename KEY_TYPE, typename VALUE_TYPE>
inline void VMap<KEY_TYPE, VALUE_TYPE>::InitHashTable(UINT nHashSize, VBool bAllocNow)
//
// Used to force allocation of a hash table or to override the default
//   hash table size of (which is fairly small)
{
	VASSERT(V_IS_VALID_PTR(this));
	VASSERT(m_nCount == 0);
	VASSERT(nHashSize > 0);

	if (m_pHashTable != NULL)
	{
		// free hash table
		delete[] m_pHashTable;
		m_pHashTable = NULL;
	}

	if (bAllocNow)
	{
		m_pHashTable = new VPair* [nHashSize];
		memset(m_pHashTable, 0, sizeof(VPair*) * nHashSize);
	}
	m_nHashTableSize = nHashSize;
}

template <typename KEY_TYPE, typename VALUE_TYPE>
inline void VMap<KEY_TYPE, VALUE_TYPE>::RemoveAll()
{
	VASSERT(V_IS_VALID_PTR(this));

	if (m_pHashTable != NULL)
	{
		// free hash table
		delete[] m_pHashTable;
		m_pHashTable = NULL;
	}

	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks->FreeChain();
	m_pBlocks = NULL;
}

template <typename KEY_TYPE, typename VALUE_TYPE>
inline void VMap<KEY_TYPE, VALUE_TYPE>::Clear() // does not de-allocate
{
	VASSERT(V_IS_VALID_PTR(this));
	for (int iBucket=0;iBucket<(int)m_nHashTableSize && m_pHashTable!=NULL;iBucket++)
	{
		if (m_pHashTable[iBucket]==NULL)
			continue;
		if (m_pFreeList==NULL)
    {
			m_pFreeList = m_pHashTable[iBucket];
      m_pHashTable[iBucket] = NULL;
    }
		else
		{
		  // append the whole bucket list to the free list
		  VPair *pLast = m_pFreeList;
      while (pLast->pNext!=NULL)
			  pLast = pLast->pNext;
		  pLast->pNext = m_pHashTable[iBucket];
		  m_pHashTable[iBucket] = NULL;
		}
	}
	m_nCount = 0;
}

template <typename KEY_TYPE, typename VALUE_TYPE>
inline VMap<KEY_TYPE, VALUE_TYPE>::~VMap()
{
	RemoveAll();
	ASSERT(m_nCount == 0);
}

/////////////////////////////////////////////////////////////////////////////
// Pair helpers
// same as CList implementation except we store VPair's not CNode's
//    and VPair's are singly linked all the time

template <typename KEY_TYPE, typename VALUE_TYPE>
inline typename VMap<KEY_TYPE, VALUE_TYPE>::VPair* VMap<KEY_TYPE, VALUE_TYPE>::NewPair()
{
	if (m_pFreeList == NULL)
	{
		// add another block
		VLink* newBlock = VLink::Create(m_pBlocks, m_nBlockSize, sizeof(VPair));
		// chain them into free list
		VPair* pPair = (VPair*) newBlock->data();
		// free in reverse order to make it easier to debug
		pPair += m_nBlockSize - 1;
		for (int i = m_nBlockSize-1; i >= 0; i--, pPair--)
		{
			pPair->pNext = m_pFreeList;
			m_pFreeList = pPair;
		}
	}
	ASSERT(m_pFreeList != NULL);  // we must have something

	VPair* pPair = m_pFreeList;
	m_pFreeList = m_pFreeList->pNext;
	m_nCount++;
	ASSERT(m_nCount > 0);  // make sure we don't overflow

  pPair->key = KEY_TYPE();
  pPair->value = VALUE_TYPE();

	return pPair;
}

template <typename KEY_TYPE, typename VALUE_TYPE>
inline void VMap<KEY_TYPE, VALUE_TYPE>::FreePair(VPair* pPair, bool bDeallocOnEmpty)
{

	pPair->pNext = m_pFreeList;
	m_pFreeList = pPair;
	m_nCount--;
	VASSERT(m_nCount >= 0);  // make sure we don't underflow

	// if no more elements, cleanup completely
	if (m_nCount == 0 && bDeallocOnEmpty)
		RemoveAll();
}

template <typename KEY_TYPE, typename VALUE_TYPE>
inline typename VMap<KEY_TYPE, VALUE_TYPE>::VPair* VMap<KEY_TYPE, VALUE_TYPE>::GetPairAt(KEY_TYPE key, UINT& nHash) const
// find Pair (or return NULL)
{
	nHash = HashKey(key) % m_nHashTableSize;

	if (m_pHashTable == NULL)
		return NULL;

	// see if it exists
	VPair* pPair;
	for (pPair = m_pHashTable[nHash]; pPair != NULL; pPair = pPair->pNext)
	{
		if (pPair->key == key)
			return pPair;
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////

template <typename KEY_TYPE, typename VALUE_TYPE>
inline VBool VMap<KEY_TYPE, VALUE_TYPE>::Lookup(KEY_TYPE key, VALUE_TYPE& rValue) const
{
	VASSERT(V_IS_VALID_PTR(this));

	UINT nHash;
	VPair* pPair = GetPairAt(key, nHash);
	if (pPair == NULL)
		return FALSE;  // not in map

	rValue = pPair->value;
	return TRUE;
}

template <typename KEY_TYPE, typename VALUE_TYPE>
inline VBool VMap<KEY_TYPE, VALUE_TYPE>::KeyExists(KEY_TYPE key) const
{
  VASSERT(V_IS_VALID_PTR(this));

  UINT nHash;
  VPair* pPair = GetPairAt(key, nHash);
  if (pPair == NULL)
    return FALSE;  // not in map

  return TRUE;
}

template <typename KEY_TYPE, typename VALUE_TYPE>
inline VALUE_TYPE& VMap<KEY_TYPE, VALUE_TYPE>::operator[](KEY_TYPE key)
{
	VASSERT(V_IS_VALID_PTR(this));

	UINT nHash;
	VPair* pPair;
	if ((pPair = GetPairAt(key, nHash)) == NULL)
	{
		if (m_pHashTable == NULL)
			InitHashTable(m_nHashTableSize);

		// it doesn't exist, add a new Pair
		pPair = NewPair();

		pPair->key = key;
		// 'pPair->value' is a constructed object, nothing more

		// put into hash table
		pPair->pNext = m_pHashTable[nHash];
		m_pHashTable[nHash] = pPair;
	}
	return pPair->value;  // return new reference
}

template <typename KEY_TYPE, typename VALUE_TYPE>
inline VBool VMap<KEY_TYPE, VALUE_TYPE>::RemoveKey(KEY_TYPE key)
// remove key - return TRUE if removed
{
	VASSERT(V_IS_VALID_PTR(this));

	if (m_pHashTable == NULL)
		return FALSE;  // nothing in the table

	VPair** ppPairPrev;
	ppPairPrev = &m_pHashTable[HashKey(key) % m_nHashTableSize];

	VPair* pPair;
	for (pPair = *ppPairPrev; pPair != NULL; pPair = pPair->pNext)
	{
		if (pPair->key == key)
		{
			// remove it
			*ppPairPrev = pPair->pNext;  // remove from list
			FreePair(pPair);
			return TRUE;
		}
		ppPairPrev = &pPair->pNext;
	}
	return FALSE;  // not found
}

template <typename KEY_TYPE, typename VALUE_TYPE>
inline int VMap<KEY_TYPE, VALUE_TYPE>::RemoveValue(VALUE_TYPE value)
{
	VASSERT(V_IS_VALID_PTR(this));

	int iFound = 0;
	for (int iBucket=0;iBucket<(int)m_nHashTableSize && m_pHashTable!=NULL;iBucket++)
	{
		VPair** ppPairPrev;
		ppPairPrev = &m_pHashTable[iBucket];

		VPair* pPair;
		for (pPair = *ppPairPrev; pPair != NULL; pPair = pPair->pNext)
		{
			if (pPair->value == value)
			{
				// remove it
				*ppPairPrev = pPair->pNext;  // remove from list
				FreePair(pPair);
				iFound++;
				iBucket--;break; // restart this bucket
			} 
      else
      {
				ppPairPrev = &pPair->pNext;
      }
		}
	}
	
	return iFound;
}


/////////////////////////////////////////////////////////////////////////////
// Iterating

template <typename KEY_TYPE, typename VALUE_TYPE>
inline void VMap<KEY_TYPE, VALUE_TYPE>::GetNextPair(VPOSITION& rNextPosition, KEY_TYPE& rKey, VALUE_TYPE& rValue) const
{
	VASSERT(V_IS_VALID_PTR(this));
	VASSERT(m_pHashTable != NULL);  // never call on empty map

	VPair* pPairRet = (VPair*)rNextPosition;
	VASSERT(pPairRet != NULL);

	if (pPairRet == (VPair*) VBEFORE_START_POSITION)
	{
		// find the first Pair
		for (UINT nBucket = 0; nBucket < m_nHashTableSize; nBucket++)
    {
			if ((pPairRet = m_pHashTable[nBucket]) != NULL)
				break;
    }
		ASSERT(pPairRet != NULL);  // must find something
	}

	// find next Pair
	VASSERT(V_IS_VALID_READ_RANGE(pPairRet, sizeof(VPair))); // [gerd]
	VPair* pPairNext;
	if ((pPairNext = pPairRet->pNext) == NULL)
	{
		// go to next bucket

		for (UINT nBucket = (HashKey(pPairRet->key) % m_nHashTableSize) + 1; nBucket < m_nHashTableSize; nBucket++)
    {
			if ((pPairNext = m_pHashTable[nBucket]) != NULL)
				break;
    }
	}

	rNextPosition = (VPOSITION) pPairNext;

	// fill in return data
	rKey = pPairRet->key;
	rValue = pPairRet->value;
}


#ifdef _VISION_PS3
#pragma diag_push
#pragma diag_suppress=1669
#endif

template <typename KEY_TYPE, typename VALUE_TYPE>
inline VMap<KEY_TYPE, VALUE_TYPE>& VMap<KEY_TYPE, VALUE_TYPE>::operator=(const VMap& other)
{
	// Could be optimized and copied directly
	RemoveAll();
	VPOSITION p = other.GetStartPosition();
	KEY_TYPE key;
	VALUE_TYPE val;
	while(p)
	{
		other.GetNextPair(p, key, val);
		operator[](key) = val;
	}
	
	return *this;
}  

#ifdef _VISION_PS3
#pragma diag_pop
#endif


/////////////////////////////////////////////////////////////////////////////
// Diagnostics

#ifdef HK_DEBUG
/*
template <typename KEY_TYPE, typename VALUE_TYPE>
void VMap<KEY_TYPE, VALUE_TYPE>::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);

	dc << "with " << m_nCount << " elements";
	if (dc.GetDepth() > 0)
	{
		// Dump in format "[key] -> value"
		KEY_TYPE key;
		VALUE_TYPE val;

		VPOSITION pos = GetStartPosition();
		while (pos != NULL)
		{
			GetNextPair(pos, key, val);
			dc << "\n\t[" << key << "] = " << val;
		}
	}

	dc << "\n";
}
*/
template <typename KEY_TYPE, typename VALUE_TYPE>
inline void VMap<KEY_TYPE, VALUE_TYPE>::AssertValid() const
{
//	CObject::AssertValid();

	VASSERT(m_nHashTableSize > 0);
	VASSERT(m_nCount == 0 || m_pHashTable != NULL);
    // non-empty map should have hash table
}
#endif //HK_DEBUG

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
