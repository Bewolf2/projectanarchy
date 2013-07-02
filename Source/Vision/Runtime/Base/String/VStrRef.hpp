/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VStrRef.hpp


#ifndef _VISION_VSTRREF_HPP_
#define _VISION_VSTRREF_HPP_

// Should not show up in documentation!
#ifndef _VISION_DOC

// This class encapsulates the string reference - either it keeps just the reference
// to a string or it keeps a copy of the string - whatever is required. The difference is that
// when the instance of this class is destroyed then it frees the string it refers to if it owns
// it (has a copy of it).

class VBASE_IMPEXP_CLASS VStrRef
{
public:
  // standard constructor
  VStrRef() { m_pStr = 0; m_bOwnString = false; }

  // constructor - see SetString method for description
  VStrRef( const char* pStr, bool bOwn )
  { 
    m_pStr = 0; 
    m_bOwnString = false;
    
    SetString( pStr, bOwn );
  }

  // copy constructor
  VStrRef( const VStrRef& src )
  {
    m_bOwnString = src.m_bOwnString;

    if( m_bOwnString )
      m_pStr = vStrDup( src.m_pStr );
    else
      m_pStr = src.m_pStr;
  }

  // destructor
  ~VStrRef() { FreeString(); }

  //   Sets the string for this reference object.
  // 
  // This method sets the string to be referenced by this reference object.
  // 
  // \param pStr
  //   string to reference
  // 
  // \param bOwn
  //   if true then this object will create and keep copy of the input string, if false then
  //   just reference (pointer) to the input string will be kept.
  void SetString( const char* pStr, bool bOwn = true )
  {
    FreeString();

    m_bOwnString = bOwn;

    if( m_bOwnString )
      m_pStr = vStrDup( pStr );
    else
      m_pStr = pStr;
  }

  /// \brief
  ///   Returns the string referenced by this object.
  /// 
  /// \return
  ///   string referenced by this object 
  
  const char* GetString() { return m_pStr; }

  /// \brief
  ///   Queries, whether this object owns the string it references.
  /// 
  /// \return
  ///   true if the string is owned by this object, false otherwise
  
  bool IsStringOwner() { return m_bOwnString; }

  // casting operator to const char*
  operator const char* () { return m_pStr; }

  // assignment operator
  VStrRef& operator = ( const VStrRef& src )
  {
    FreeString();

    m_bOwnString = src.m_bOwnString;
    
    if( m_bOwnString )
      m_pStr = vStrDup( src.m_pStr );
    else
      m_pStr = src.m_pStr;

    return *this;
  }

private:
  void FreeString()
  {
    if( m_bOwnString )
      vMemFree(const_cast<char*>(m_pStr));
    
    m_pStr = 0;
  }

  const char* m_pStr;   ///< string referenced or held by this object
  bool m_bOwnString;    ///< flag specifying if the string is owned by this object
};

#endif // _VISION_DOC

#endif // _VISION_VSTRREF_HPP_

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
