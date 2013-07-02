/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VMapStrToPtr.hpp

#ifndef MU_VISION_VMAPSTRTOPTR_HPP
#define MU_VISION_VMAPSTRTOPTR_HPP

#include <Vision/Runtime/Base/Container/VLink.hpp>

/// \brief
///   Base class for a string-based map
class VBASE_IMPEXP_CLASS VStrMapBase
{
protected:

  /// \brief
  ///  pair type for internal storage
  template<typename T>
  struct VPair
  {
    VPair<T>* pNext;
    VString key;
    T* value;
    int hashValue;
  };

  /// \brief
  /// Constructor.
  VStrMapBase(int nBlockSize);
  VStrMapBase(const VStrMapBase& rh);
  VStrMapBase& operator= (const VStrMapBase& rh);

public:

  /// \brief
  /// Returns the number of elements.
  int GetCount() const { return m_nCount; }

  /// \brief
  /// Returns whether the map is empty.
  VBool IsEmpty() const { return m_nCount == 0; }

  /// \brief
  /// Removes existing (key, ?) pair
  VBool RemoveKey(const char *key);

  /// \brief
  /// Removes all (key, value) pairs
  void RemoveAll();

  /// \brief
  /// Removes all existing pairs with the given value; returns the number of removed pairs.
  int RemoveValue(void *pValue);

  /// \brief
  /// Returns a VPOSITION iterator pointing at the start position of the map.
  VPOSITION GetStartPosition() const;

  /// \brief
  /// Returns the hash table size.
  UINT GetHashTableSize() const { return m_nHashTableSize; }

  /// \brief
  /// Initializes the hash table.
  void InitHashTable(UINT hashSize, VBool bAllocNow = TRUE);

  /// \brief
  // Overridables: special non-virtual (see map implementation for details)
  // method used for user-provided hash keys.
  UINT HashKey(const char *key) const;

  // Implementation
protected:

  void DeepCopy(const VStrMapBase& rh);

  VBool Lookup(const char *key, void*& rValue) const;
  void*& operator[](const char *key);
  inline void SetAt(const char *key, void* newValue) { (*this)[key] = newValue; }
  void GetNextPair(VPOSITION& rNextPosition, VString& rKey, void*& rValue) const;

  VPair<void>** m_pHashTable;
  UINT m_nHashTableSize;
  int m_nCount;
  VPair<void>* m_pFreeList;
  struct VLink* m_pBlocks;
  int m_nBlockSize;

  VPair<void>* NewPair();
  void FreePair(VPair<void>*);
  VPair<void>* GetPairAt(const char*, UINT&) const;

  ~VStrMapBase(); //should never be deleted directly
public:
#ifdef HK_DEBUG
  void AssertValid() const;
#endif
};

/// \brief
///   Implementation of a hash table mapping from VString to T*
template<typename T>
class VStrMap : public VStrMapBase
{
private:
  #ifdef HK_DEBUG
  VPair<T>**& m_pTypedHashTable;
  #endif

public:

  /// \brief
  ///   Constructor
  ///
  /// \param nBlockSize
  ///   Initial size of the internal lookup table
  VStrMap(int nBlockSize = 10) : VStrMapBase(nBlockSize)
#ifdef HK_DEBUG
   ,m_pTypedHashTable( *((VPair<T>***)&m_pHashTable) )
#endif
  {
  }

  /// \brief
  ///   Copy constructor
  ///
  /// \param rh
  ///   Instance to copy from
  VStrMap(const VStrMap& rh) : VStrMapBase(rh) 
#ifdef HK_DEBUG
   ,m_pTypedHashTable( *((VPair<T>***)&m_pHashTable) )
#endif
  {}

  /// \brief
  ///   Assignment operator
  ///
  /// \param rh
  ///   Instance to copy from
  VStrMap& operator=(const VStrMap& rh)
  {
    VStrMapBase::operator=(rh);
    return *this;
  }

  /// \brief
  ///   Destructor
  ~VStrMap(){}

  /// \brief
  ///   Looks up a specific value, given the key string.
  VBool Lookup(const char *key, T*& rValue) const
  {
    return VStrMapBase::Lookup(key,(void*&)rValue);
  }

  /// \brief
  ///   Looks up a value belonging to the passed key string.
  T*& operator[](const char *key)
  {
    return (T*&)VStrMapBase::operator [](key);
  }

  /// \brief
  ///   Adds a new (key, value) pair. Replaces the value corresponding to the key by the value newValue. 
  ///   If the key is not included in the map then new key is created and its value is set to newValue.
  inline void SetAt(const char *key, T* newValue) { (*this)[key] = newValue; }

  /// \brief
  ///   Returns the next pair, given a VPOSITION iterator.
  void GetNextPair(VPOSITION& rNextPosition, VString& rKey, T*& rValue) const 
  {
    VStrMapBase::GetNextPair(rNextPosition,rKey,(void*&)rValue);
  }
};

// for backwards compatibility
typedef VStrMap<void> VMapStrToPtr;

#endif // MU_VISION_VMAPSTRTOPTR_HPP

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
