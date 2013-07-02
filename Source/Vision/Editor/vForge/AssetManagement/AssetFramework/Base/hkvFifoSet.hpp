/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKV_FIFOSET_HPP_INCLUDED
#define HKV_FIFOSET_HPP_INCLUDED

#include "../AssetFramework.hpp"
#include <set>

template<class TYPE>
class hkvFifoSet
{
public:
  hkvFifoSet()
  {
    m_First = m_AllEntries.end();
    m_Last = m_AllEntries.end();
  }

  bool Insert(const TYPE& data)
  {
    Entry e(data);

    // if the set already contained such an element, do nothing
    std::pair<std::set<Entry>::iterator, bool> it = m_AllEntries.insert(e);

    if (!it.second)
      return false;

    // the new last entry always points to the end entry
    it.first->m_NextEntry = m_AllEntries.end();

    if (m_First == m_AllEntries.end()) // queue is empty
    {
      // set it as the only entry
      m_First = it.first;
      m_Last = it.first;
    }
    else
    {
      // let the last entry point to this, and make this the new last element
      m_Last->m_NextEntry = it.first;
      m_Last = it.first;
    }

    return true;
  }

  bool IsEmpty() const
  {
    return m_AllEntries.empty();
  }

  unsigned int GetElementCount() const
  {
    return (unsigned int) m_AllEntries.size();
  }

  const TYPE& GetFrontElement() const
  {
    VASSERT(!IsEmpty());

    return m_First->m_Data;
  }

  void RemoveFrontElement()
  {
    VASSERT(!IsEmpty());

    typename std::set<Entry>::iterator itNext = m_First->m_NextEntry;
    m_AllEntries.erase(m_First);
    m_First = itNext;
  }

  void Clear()
  {
    m_AllEntries.clear();
    m_First = m_AllEntries.end();
    m_Last = m_AllEntries.end();
  }

private:
  struct Entry
  {
    Entry() { }
    Entry(const TYPE& data) : m_Data(data) { }

    mutable typename std::set<Entry>::iterator m_NextEntry;
    TYPE m_Data;

    bool operator< (const Entry& rhs) const
    {
      return m_Data < rhs.m_Data;
    }
  };

  typename std::set<Entry>::iterator m_First;
  typename std::set<Entry>::iterator m_Last;

  typename std::set<Entry> m_AllEntries;
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
