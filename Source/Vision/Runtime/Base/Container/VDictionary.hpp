/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VDictionary.hpp

#ifndef VDICTIONARY_HPP
#define VDICTIONARY_HPP


/// \brief
///   Key/value pair template class for the VDictionary.
template<class T> class VDictEntry : public VBaseObject
{
public:

  /// \brief
  ///   Constructor used by VDictionary.
  VDictEntry(const char* pszKey, const T &value)
  {
    m_sKey = pszKey;
    m_value = value;
  }
  
  /// \brief
  ///   Constructor used by optimized VDictionary implementations.
  VDictEntry(const char* pszKey)
  {
    m_sKey = pszKey;
  }
  
  /// \brief
  ///   Copy constructor.
  VDictEntry(const VDictEntry& other)
  {
    m_sKey = other.m_sKey;
    m_value = other.m_value;
  }
  
  VString m_sKey;
  T       m_value;
};

/// \brief
///   Dictionary class to map a string onto any other type of object
/// 
/// The key values are case-insensitive
template<class T> class VDictionary
{ 
public:
  VDictionary()
  {
  }
  
  VDictionary(const VDictionary& other)
  {
    Copy(other);
  }
  
  ~VDictionary()
  {
    Reset();
  }

  /// \brief
  ///   Returns the number of entries in the dictionary
  int GetLength() const
  {
    return m_list.GetLength();
  }

  /// \brief
  ///   Returns the key of entry i
  ///
  /// \param i
  ///   The entry number
  ///
  /// \return
  ///   The key of entry i
  const char* GetKey(int i) const
  {
    return m_list.Get(i)->m_sKey.AsChar();
  }
  
  /// \brief
  ///   Returns the value of entry i
  ///
  /// \param i
  ///   The entry number
  ///
  /// \return
  ///   The value of entry i
  T &GetValue(int i) const
  {
    return m_list.Get(i)->m_value;
  }

  /// \brief
  ///   Returns the index of the key or -1 if not found
  int Find(const char *pszKey) const
  {
    const int iCount = GetLength();
    for (int i=0; i<iCount; i++)
    {
      VDictEntry<T> *pEntry = m_list[i];
      if (_stricmp(pEntry->m_sKey, pszKey)==0)
        return i;
    }
    return -1;
  }

  /// \brief
  ///   Updates or add a dictionary pair
  int Set(const char *pszKey, const T &value)
  {
    int idx = Find(pszKey);
    if (idx == -1)
    { ///<add it
      idx = m_list.GetLength(); ///<last entry
      m_list.Append ( new VDictEntry<T>(pszKey, value) );
      return idx;
    } ///< update it
    GetValue(idx) = value;
    return idx;
  }
  
  /// \brief
  ///   Returns the value for a specific key
  bool Get(const char* pszKey, T &val) const
  {
    int idx = Find(pszKey);
    if (idx == -1)
      return false;
    val  = GetValue(idx);
    return true;
  }
  
  /// \brief
  ///   Remove an item
  bool Remove(const char *pszKey)
  {
    int idx = Find(pszKey);
    if (idx == -1)
      return FALSE;
    delete m_list.Get(idx);
    m_list.RemoveAt(idx);
    return TRUE;
  }

  /// \brief
  ///   Replace key and value at the index of a certain key
  void Replace(const char *pszOldKey, const char *pszNewKey, const T &value)
  {
    int idx = Find(pszOldKey);
    VASSERT(idx >= 0);
    VDictEntry<T>* entry = m_list.Get(idx);
    entry->m_sKey = pszNewKey;
    entry->m_value = value;
  }


  /// \brief
  ///   Returns the value for a specific key
  T &operator[](const char* pszKey) const
  {

#if defined(__SNC__)
#pragma diag_push
#pragma diag_suppress=285
#endif

    int idx = Find(pszKey);
    if (idx == -1)
      return *((T*)NULL);
    return GetValue(idx);

#if defined(__SNC__)
#pragma diag_pop
#endif

  }

  VDictionary &operator=(const VDictionary& other)
  {
    if (this != &other) {
      Copy(other);
    }
    return *this;
  }

  inline bool operator==(const VDictionary& rhs) const
  {
    int n = GetLength();
    if(n != rhs.GetLength())
      return false;

    for(int i=0; i<n; ++i)
    {
      T t;
      if(!rhs.Get(GetKey(i), t) || GetValue(i) != t)
        return false;
    }

    return true;
  }

  inline bool operator!=(const VDictionary& rhs) const
  {
    return m_list != rhs.m_list;
  }

  void Reset()
  {
    m_list.DeleteAll();
  }
  
private:
  
  void Copy(const VDictionary& other)
  {
    Reset();
    for (int i = 0; i < other.m_list.GetLength(); i++) {
      m_list.Append(new VDictEntry<T>(*((VDictEntry<T> *)(other.m_list.Get(i)))));
    }
  }
  
protected:

  //list of dictionary pairs
  VPListT< VDictEntry<T> > m_list;

  //Not needed yet:
  //Returns the dictionary pair #i
  //VDictPair<T> &GetEntry();
};


/// \brief
///   String to String dictionary class
class VStrDictionary : public VDictionary<VString>
{
public:
  V_DECLARE_SERIALX(VStrDictionary, VBASE_IMPEXP)

  /// \brief
  ///   Update or add a dictionary pair. Avoids the need to pass a VString instance to Set
  VBASE_IMPEXP int Set(const char *pszKey, const char* pszValue);
  
protected:
  void SerializeX(VArchive& ar);
};



/// \brief
///   Dictionary implementation for int,int pairs
template<int iMaxSize> class VStaticIntDictionary
{
public:
  VStaticIntDictionary()
  {
    m_iCount = 0;
  }

  inline void Set(int iKey, int iValue) 
  {
    int iIndex = GetIndex(iKey);
    if (iIndex<0)
    {
      if (m_iCount>=iMaxSize)
      {
        VASSERT_MSG(false, "Maximum size exceeded");
        return;
      }
      iIndex = m_iCount; ///< append
      m_iCount++;
    }
    m_iKey[iIndex] = iKey;
    m_iValue[iIndex] = iValue;
  }

  inline int Get(int iKey, int iDefault) const
  {
    for (int i=0;i<m_iCount;i++)
      if (m_iKey[i]==iKey)
        return m_iValue[i];
    return iDefault;
  }

  inline int GetIndex(int iKey)  const
  {
    for (int i=0;i<m_iCount;i++)
      if (m_iKey[i]==iKey)
        return i;
    return -1;
  }

  inline void SerializeX(VArchive& ar)
  {
    if (ar.IsLoading())
    {
      int iDummy;
      ar >> m_iCount;
      VASSERT_MSG(m_iCount<=iMaxSize,"Maximum size exceeded");
      for (int i=0;i<m_iCount;i++) if (i<iMaxSize)
      {
        ar >> m_iKey[i] >> m_iValue[i];
      } else
      {
        ar >>iDummy >> iDummy;
      }
    } else
    {
      ar << m_iCount;
      for (int i=0;i<m_iCount;i++)
        ar << m_iKey[i] << m_iValue[i];
    }
  }


  int m_iKey[iMaxSize];
  int m_iValue[iMaxSize];
  int m_iCount;
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
