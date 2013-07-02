/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VLocale.hpp

#ifndef VISION_VLOCALE_HPP
#define VISION_VLOCALE_HPP

#ifdef WIN32

/// \brief
///   Provide various locale-related conversion methods.
class VLocale
{
public:
  /// \brief
  ///   Converts a string from from Unicode (UTF-16 wide characters) to ANSI/MBCS (8-bit characters), using the
  ///   current code page for encoding the target characters.
  /// \param lpWideCharStr
  ///   the Unicode source string
  /// \param cchWideChar
  ///   the length of the Unicode source string, in wide characters. Passing \c -1 processes the source 
  ///   string up to the first \c \\0 character, and also terminates the target string with a \c \\0 
  ///   character.
  /// \param lpMultiByteStr
  ///   the ANSI/MBCS target buffer
  /// \param cbMultiByte
  ///   the length of the ANSI/MBCS target buffer, in bytes
  /// \return
  ///   the number of bytes written to the target buffer, or \c 0 in case of an error.
  /// \sa GetCurrentCodePage()
  /// \sa SetCurrentCodePage()
  static VBASE_IMPEXP int VWideCharToMultiByte(LPCWSTR lpWideCharStr,int cchWideChar,LPSTR lpMultiByteStr,int cbMultiByte);

  /// \brief
  ///   Converts a string from from ANSI/MBCS (8-bit characters) to Unicode (UTF-16 wide characters), using the
  ///   current code page for interpreting the source characters.
  /// \param lpMultiByteStr
  ///   the ANSI/MBCS source string
  /// \param cbMultiByte
  ///   the length of the ANSI/MBCS source string, in bytes. Passing \c -1 processes the source 
  ///   string up to the first \c \\0 character, and also terminates the target string with a \c \\0 
  ///   character.
  /// \param lpWideCharStr
  ///   the Unicode target buffer
  /// \param cchWideChar
  ///   the length of the unicode target buffer, in wide characters
  /// \return
  ///   the number of wide characters written to the target buffer, or \c 0 in case of an error.
  /// \sa GetCurrentCodePage()
  /// \sa SetCurrentCodePage()
  static VBASE_IMPEXP int VMultiByteToWideChar(LPCSTR lpMultiByteStr,int cbMultiByte,LPWSTR lpWideCharStr,int cchWideChar);

  /// \brief
  ///   Converts a string from from UTF-16 (wide characters) to UTF-8 (8-bit characters).
  /// \param lpWideCharStr
  ///   the UTF-16 source string
  /// \param cchWideChar
  ///   the length of the UTF-16 source string, in wide characters. Passing \c -1 processes the source 
  ///   string up to the first \c \\0 character, and also terminates the target string with a \c \\0 
  ///   character.
  /// \param lpMultiByteStr
  ///   the UTF-8 target buffer
  /// \param cbMultiByte
  ///   the length of the UTF-8 target buffer, in bytes
  /// \return
  ///   the number of bytes written to the target buffer, or \c 0 in case of an error.
  static VBASE_IMPEXP int WideCharToUTF8(LPCWSTR lpWideCharStr,int cchWideChar,LPSTR lpMultiByteStr,int cbMultiByte);

  /// \brief
  ///   Converts a string from from UTF-8 (8-bit characters) to UTF-16 (wide characters).
  /// \param lpMultiByteStr
  ///   the UTF-8 source string
  /// \param cbMultiByte
  ///   the length of the UTF-8 source string, in bytes. Passing \c -1 processes the source 
  ///   string up to the first \c \\0 character, and also terminates the target string with a \c \\0 
  ///   character.
  /// \param lpWideCharStr
  ///   the UTF-16 target buffer
  /// \param cchWideChar
  ///   the length of the UTF-16 target buffer, in wide characters
  /// \return
  ///   the number of wide characters written to the target buffer, or \c 0 in case of an error.
  static VBASE_IMPEXP int UTF8ToWideChar(LPCSTR lpMultiByteStr,int cbMultiByte,LPWSTR lpWideCharStr,int cchWideChar);

  /// \brief
  ///   Converts a string from from UTF-8 to ANSI/MBCS, using the current code page for encoding the 
  ///   target characters.
  /// \param lpUTF8CharStr
  ///   the UTF-8 source string
  /// \param cchUTF8Char
  ///   the length of the UTF-8 source string, in bytes. Passing \c -1 processes the source 
  ///   string up to the first \c \\0 character, and also terminates the target string with a \c \\0 
  ///   character.
  /// \param lpMultiByteStr
  ///   the ANSI/MBCS target buffer
  /// \param cbMultiByte
  ///   the length of the ANSI/MBCS target buffer, in bytes
  /// \return
  ///   the number of bytes written to the target buffer, or \c 0 in case of an error.
  /// \sa GetCurrentCodePage()
  /// \sa SetCurrentCodePage()
  static VBASE_IMPEXP int UTF8ToMultiByte(LPCSTR lpUTF8CharStr,int cchUTF8Char,LPSTR lpMultiByteStr,int cbMultiByte);

  /// \brief
  ///   Converts a string from from ANSI/MBCS to UTF-8, using the current code page for 
  ///   interpreting the source characters.
  /// \param lpMultiByteStr
  ///   the ANSI/MBCS source string
  /// \param cbMultiByte
  ///   the length of the ANSI/MBCS source string, in bytes. Passing \c -1 processes the source 
  ///   string up to the first \c \\0 character, and also terminates the target string with a \c \\0 
  ///   character.
  /// \param lpUTF8CharStr
  ///   the UTF-8 target buffer
  /// \param cchUTF8Char
  ///   the length of the UTF-8 target buffer, in bytes
  /// \return
  ///   the number of bytes written to the target buffer, or \c 0 in case of an error.
  /// \sa GetCurrentCodePage()
  /// \sa SetCurrentCodePage()
  static VBASE_IMPEXP int MultiByteToUTF8(LPCSTR lpMultiByteStr,int cbMultiByte,LPSTR lpUTF8CharStr,int cchUTF8Char);

  /// \brief
  ///   Sets the current code page.
  ///
  /// By default, the code page is set to the system's default code page, which in turn usually is
  /// the code page suitable for the active character set.
  ///
  /// \param codepage
  ///   the code page to set.
  static VBASE_IMPEXP void SetCurrentCodepage(int codepage);

  /// \brief
  ///   Returns the code page suitable for the specified language.
  /// \param langid
  ///   the language to find the code page for
  /// \return
  ///   the code page suitable for the specified language
  static VBASE_IMPEXP int GetCodePageFromLang( LANGID langid );

  /// \brief
  ///   Returns the code page used to display the specified character set
  /// \param charset
  ///   the character set to find the code page for
  /// \return 
  ///   the code page suitable for displaying the specified character set
  static VBASE_IMPEXP int GetCodePageFromCharset( int charset );

  /// \brief
  ///   Returns the character set suitable for displaying the specified language
  /// \param langid
  ///   the language to find the character set for
  /// \return
  ///   the character set suitable for displaying the specified language
  static VBASE_IMPEXP int GetCharsetFromLang( LANGID langid );
};

#endif

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
