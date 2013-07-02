/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VScriptRenderer_wrapper.hpp

#ifndef __VBITMASK_HPP
#define __VBITMASK_HPP

/// 
/// \brief
///   Small bit mask class for the scripting binding;
/// 
class VBitmask
{
protected:
  unsigned int m_iBitMask;
public:

  VBitmask() : m_iBitMask(0) {}
  VBitmask(int iMask) : m_iBitMask(iMask) {}
  VBitmask(unsigned int iMask) : m_iBitMask(iMask) {}
  VBitmask(unsigned short iMask) : m_iBitMask(iMask) {}
  VBitmask(const char * szHexValue) 
  {
    Set(szHexValue);
  }
  VBitmask(bool bAllBitsSet)
  {
    if(bAllBitsSet) Set();
    else Clear();
  }
  VBitmask(const VBitmask & bitmask) : m_iBitMask(bitmask.m_iBitMask) {}
  ~VBitmask() {}

  inline void Set()
  {
    m_iBitMask = (unsigned int) 0xffffffff;
  }

  inline void Set(int iBitNum)
  {
    VASSERT(iBitNum<32);
    m_iBitMask |= 1<<iBitNum;
  }

  void __setitem__(int iBitNum, bool bValue)
  {
    if(bValue) Set(iBitNum);
    else Clear(iBitNum);
  }

  inline void Set(const VBitmask * otherMask)
  {
    if(otherMask)
      m_iBitMask |= otherMask->m_iBitMask;
  }

  inline void Set(const char * szHexValue)
  {
    if(szHexValue)
    {
      int iValue = 0;
      sscanf( szHexValue, "%x", &iValue);

      m_iBitMask |= iValue;
    }
  }

  inline void RawSet(unsigned int uiBitmask)
  {
    m_iBitMask = uiBitmask;
  }

  inline void Clear()
  {
    m_iBitMask = 0;
  }

  inline void Clear(int iBitNum)
  {
    VASSERT(iBitNum<32);
    m_iBitMask &= ~((unsigned int)1<<iBitNum);
  }

  inline void Clear(const VBitmask * otherMask)
  {
    if(otherMask)
      m_iBitMask &= otherMask->m_iBitMask;
  }

  inline void Clear(const char * szHexValue)
  {
    if(szHexValue)
    {
      int iValue = 0;
      sscanf( szHexValue, "%x", &iValue);

      m_iBitMask &= iValue;
    }
  }

  inline bool IsSet(int iBitNum) const
  {
    VASSERT(iBitNum<32);
    return (m_iBitMask & (1<<iBitNum))!=0;
  }

  inline bool IsSet() const
  {
    return m_iBitMask!=0;
  }


  inline bool operator ==(const VBitmask &otherMask) const
  {
    return m_iBitMask == otherMask.m_iBitMask;
  }

  inline unsigned int Get() const
  {
    return m_iBitMask;
  }

  bool __getitem__(int iBitNum) const
  {
    return IsSet(iBitNum);
  }

};

#endif // __VBITMASK_HPP

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
