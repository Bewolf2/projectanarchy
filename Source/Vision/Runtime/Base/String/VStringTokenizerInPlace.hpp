/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VStringTokenizerInPlace.hpp

// VStringTokenizerInPlace.hpp: interface for the VStringTokenizer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VSTRINGTOKENIZERINPLACE_INCLUDED__
#define __VSTRINGTOKENIZERINPLACE_INCLUDED__

/// \brief
///   Splits a string into tokens delimited by a separator by replacing the separators with 0-Bytes. Operates in place, so the input string will be modified.
class VStringTokenizerInPlace
{
public:
  /// \brief
  ///   Constructor that takes the source string and the delimiter char as input. The constructor calls the Parse function.
  ///
  /// This function operates in place. If a modification of the original string is not allowed use a copy of the string (e.g. via VMemoryTempBuffer)
  /// If only the token count of a string should be evaluated, use the static function CountTokens() below.
  ///
  /// \param pString
  ///   Source string to tokenize
  ///
  /// \param Delimiter
  ///   The delimiter which will be used to tokenize the string
  ///
  /// \param bIgnoreInsideQuotes
  ///   Parse flag. If specified, delimiters inside quotes are ignored.
  VBASE_IMPEXP VStringTokenizerInPlace(char* pString, char Delimiter, bool bIgnoreInsideQuotes=false);

  /// \brief
  ///   Destructor
  VBASE_IMPEXP ~VStringTokenizerInPlace();

  /// \brief
  ///   Resets the read pointer of the tokenizer so the first token will be returned again on a call to Next()
  VBASE_IMPEXP void Reset();

  /// \brief
  ///   Parsing function which takes the string to tokenize as input. This function is called by the constructor, but it can be called later with a different string
  ///
  /// \param pString
  ///   Source string to tokenize
  ///
  /// \param Delimiter
  ///   The delimiter which will be used to tokenize the string
  ///
  /// \param bIgnoreInsideQuotes
  ///   Parse flag. If specified, delimiters inside quotes are ignored.
  VBASE_IMPEXP void Parse(char* pString, char Delimiter, bool bIgnoreInsideQuotes=false);

  /// \brief
  ///   Static helper functions that counts the number of tokens of a const string; useful to retrieve the token count without creating an instance of this class.
  ///
  /// \param pString
  ///   Source string to tokenize
  ///
  /// \param Delimiter
  ///   The delimiter which will be used to tokenize the string
  ///
  /// \param bIgnoreInsideQuotes
  ///   Parse flag. If specified then delimiters inside quotes are ignored.
  VBASE_IMPEXP static unsigned int CountTokens(const char* pString, char Delimiter, bool bIgnoreInsideQuotes=false);

  /// \brief
  ///   Returns the next token if available, otherwise NULL (end of string). Tokens may be empty strings, but are always valid when not NULL
  VBASE_IMPEXP char* Next();

  /// \brief
  ///   Returns the number of tokens in the string
  VBASE_IMPEXP unsigned int GetTokenCount();

protected:
  /// \brief
  ///   Internal parse function that takes into account whether the given string may be modified (none-const version) or not (const version)
  ///
  /// \param pString
  ///   Source string to tokenize
  ///
  /// \param Delimiter
  ///   The delimiter which will be used to tokenize the string
  ///
  /// \param bIgnoreInsideQuotes
  ///   Parse flag. If specified then delimiters inside quotes are ignored.
  ///
  /// \param bCountOnly
  ///   If true, it does not modify the input string and only counts the delimiters. So calling Next() does not make sense then.
  ///
  /// \param uiStringLength
  ///   The length of pString.
  ///
  /// \returns
  ///   The number of tokens that have been parsed.
  VBASE_IMPEXP static unsigned int DoParse(char* pString, char Delimiter, bool bIgnoreInsideQuotes, bool bCountOnly, unsigned int uiStringLength);

private:

  unsigned int m_uiReadPosition; ///< Internal state
  unsigned int m_uiStringLength; ///< Internal state
  char* m_pString;               ///< Internal state
  unsigned int m_uiNumTokens;    ///< Internal memory
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
