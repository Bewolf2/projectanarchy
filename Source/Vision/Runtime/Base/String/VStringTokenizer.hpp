/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VStringTokenizer.hpp

// VStringTokenizer.hpp: interface for the VStringTokenizer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VSTRINGTOKENIZER_H__1DE3C89D_6824_44F4_8B8F_7EF0CA23F119__INCLUDED_)
#define AFX_VSTRINGTOKENIZER_H__1DE3C89D_6824_44F4_8B8F_7EF0CA23F119__INCLUDED_

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/// \brief
///   Splits a string into tokens defined by one or more separators and builds a list of char arrays from them
class VStringTokenizer : public VPList
{
public:
  ///\brief
  /// Constructor that takes the source string and the list of separator chars as input
  ///
  ///This function splits the source string into n substrings. Every character in szSeparators is used as a separator. The sub strings
  ///are put into this VPList (VStringTokenizer inherits from it) and can thus be accessed via the [] operator.
  ///The source string remains unmodified.
  ///
  ///\param szString
  /// Source string to split
  ///
  ///\param szSeparators
  /// List of separators
  VBASE_IMPEXP VStringTokenizer(const char* szString, const char* szSeparators);

  ///\brief
  /// Destructor; releases the memory used for storing the sub strings
  VBASE_IMPEXP virtual ~VStringTokenizer();

private:
  char* m_Tokens; ///< Internal memory
};

#endif // !defined(AFX_VSTRINGTOKENIZER_H__1DE3C89D_6824_44F4_8B8F_7EF0CA23F119__INCLUDED_)

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
