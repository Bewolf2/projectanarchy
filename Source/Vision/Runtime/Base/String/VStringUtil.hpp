/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VStringUtil.hpp

#ifndef _VISION_VSTRINGUTIL_HPP
#define _VISION_VSTRINGUTIL_HPP
#include <stdio.h>

// Safe sprintf version (deals with buffer overflow if log message exceeds buffer size)
#if defined WIN32 || defined WIN64 || defined _VISION_XENON 
#define vis_snprintf(__str,__cnt,__fmt,...) { _snprintf(__str,__cnt,__fmt,__VA_ARGS__); if(__cnt > 0) __str[__cnt-1] = 0; }
#else
#define vis_snprintf(__str,__cnt,__fmt,...) {  snprintf(__str,__cnt,__fmt,__VA_ARGS__); if(__cnt > 0) __str[__cnt-1] = 0; }
#endif

namespace VStringUtil
{
  /// \brief Computes a murmur2 hash of the given value
  VBASE_IMPEXP unsigned int MurmurHash2 ( const void * key, int len, unsigned int seed );

  /// \brief
  ///   Computes the hash value for a given string.
  ///
  /// This hash should not be persisted and only be used locally inside a running application. Do not expect the resulting value to
  /// be compatible between subsequent engine versions of different platforms.
  /// At this time, MurmurHash2 is used as hash algorithm. When using bCaseSensitive all characters are converted to upper case before hashing.
  ///   
  /// \param pszString
  ///   [in] string
  /// 
  /// \return
  ///    Hash value
  VBASE_IMPEXP int Hash(const char *pszString, bool bCaseSensitive = false);

  /// \brief
  ///   Returns whether the string is empty, i.e. it is NULL or its first character is zero.
  ///   
  /// \param szString
  ///   String to be checked.
  /// 
  /// \return
  ///    True if the string is empty, false otherwise.
  inline bool IsEmpty(const char* szString)
  {
    return szString == NULL || szString[0] == '\0';
  }

  /// \brief
  ///   Returns whether the string is empty, i.e. it is NULL or its first character is zero.
  ///   
  /// \param szString
  ///   String to be checked.
  /// 
  /// \return
  ///    True if the string is empty, false otherwise.
  inline bool IsEmpty(const wchar_t* szString)
  {
    return szString == NULL || *szString == L'\0';
  }

  /// \brief
  ///   Returns the index of the first occurrence of 'ch' in szString.
  ///
  /// \note
  ///   This method does not support UTF-8 strings.
  ///   
  /// \param szString
  ///   String to be checked.
  //
  /// \param c
  ///   Char that is searched for.
  ///
  /// \param iStartIndex
  ///   Search begins at this index of szString, default is 0.
  ///
  /// \param iEndIndex
  ///   Search ends at this index of szString, default is INT_MAX.
  ///
  /// \return
  ///    Returns the index of the first character matching 'c' or -1 if none is found.
  VBASE_IMPEXP int IndexOf(const char *szString, char c, int iStartIndex=0, int iEndIndex=INT_MAX);
};

#define PARAM_STRING_SEP '@'


/// \brief
///   Appends the relative path string to the base path
/// 
///  If necessary, a backslash is inserted between the two strings.
/// 
/// \param vsBasePath
///   [in/out] left part of the path.  The string contains the the result 
/// 
/// \param szRelativePath
///   [in] reltive path (right part)
/// 
/// \example
///   \code
///   VString path("c:\\Export");
///   AppendRelativePath(path, "mapDir");
///   VASSERT(strcmp("c:\\Export\\mapDir", path1.AsChar())==0);
///   VString path2("c:/Export");
///   AppendRelativePath(path2, "\\mapDir");
///   VASSERT(strcmp("c:/Export\\mapDir", path2.AsChar())==0);
///   VString path3("c:/Export/");
///   AppendRelativePath(path3, "\\mapDir");
///   VASSERT(strcmp("c:/Export/mapDir", path3.AsChar())==0);
///   VString path4("c:\\Export\\");
///   AppendRelativePath(path4, "/mapDir");
///   VASSERT(strcmp("c:\\Export\\mapDir", path4.AsChar())==0);
///   \endcode
VBASE_IMPEXP void AppendRelativePath(VString &vsBasePath, const char* szRelativePath);



// -------------------------------------------------------------------------------
// Method:  GetBracketString
//
// Returns a new string of the part between the specified outer brackets,
// You can use [] {} and () for brackets. (left = '{' right ='}')
// Returns NULL if there is no substring.

// eg.   GetBracketString("blabbla[this part]",'[',']') -> "this part"
//       GetBracketString("blabbla[this part[0]]",'[',']') -> "this part[0]"
// -------------------------------------------------------------------------------

VBASE_IMPEXP char *GetBracketString(const char *pString,char left,char right);

// -------------------------------------------------------------------------------
// Method:  GetStringBefore
//
// Returns a copy of the string part before the specified character
// eg.  GetStringBefore("blablabla[this part]",'[') -> "blablabla"
// -------------------------------------------------------------------------------

VBASE_IMPEXP char *GetStringBefore(const char *pString,char ch);
VBASE_IMPEXP const char *GetStringAfter(const char *pString,char ch);
VBASE_IMPEXP const char *IgnoreLeadingChar(const char *pString, char ch);


// Increments number that is at the end of the string
// 
// This function increments a number that has been appended to the string. If there is no such
// number yet, it is appended to the original string.
// 
// \param vsString
//   the string that contains the number to be incremented
// 
// \param iPadding
//   the number of digits that the number should contain
// 
// \example
//   \code
//   "map" -> "map_001" -> "map_002" -> "map_003" -> ... (iPadding = 3)
//   \endcode
VBASE_IMPEXP void IncrementNumbering(VString& vsString, int iPadding = 3);


//Helpers

// Wrapper around GetBracketString that fills in a VString for us
inline VBool GetBracketString(const char *pString,char left,char right, VString &res)
{
  char *pNew = GetBracketString(pString,left,right);
  if (pNew)
  {
    res = pNew;
    vStrFree(pNew);
    return TRUE;
  } else
    res.Reset();
  return FALSE;
}

// Wrapper around GetStringBefore that fills in a VString for us
inline VBool GetStringBefore(const char *pString,char ch, VString &res)
{
  char *pNew = GetStringBefore(pString,ch);
  if (pNew)
  {
    res = pNew;
    vStrFree(pNew);
    return TRUE;
  } else
    res.Reset();
  return FALSE;
}


// parameter string utils

#define IS_LETER(c) ( (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') )
#define IS_DIGIT(c) ( c >= '0' && c <= '9' )
#define IS_UNDERSCORE(c) ( c == '_' ) 

inline bool IsWhiteSpace(char c) {
  if(c == ' '     // Space
    || c == '\t'  // Tab
    || c == '\n'  // Newline
    || c == '\v'  // Vertical Tab
    || c == '\f'  // Line Feed
    || c == '\r') // Carriage Return
    return true;
  else
    return false;
}


// Parse the parameter string from the new or the old format and transform it in an "internal"
// format.
// 
// \param szStr
//   input parameter string. 
// 
// \param res
//   result striung in "internal" format.
// 
// \note
//   The imput string can be a combination of the new and old parameter string format.
// 
// \example
//   \code
//   shape.Primitives[1].MappingPlane.anchor.y -> shape@Primitives@1@MappingPlane@anchor@y
//   shape[Primitives[1,MappingPlane[anchor[y]]]] -> shape@Primitives@1@MappingPlane@anchor@y
//   \endcode
VBASE_IMPEXP VBool ParseParameterString(const char *szStr, VString &res);

// separate the first parameter from the string
VBASE_IMPEXP void SplitParameterString(const char *szStr, VString &head, VString &tail);

// Only allow what a normal programming language would allow for variable
// names. This is digits, letters and underscore. Also the name can't start 
// with a digit!
VBASE_IMPEXP VBool IsValidVariableName(const char *szName);

// Returns valid version of the passed variable name
VBASE_IMPEXP VString MakeValidVariableName(const char *szName);

// get the array index (for array parameters) from the already parsed string
VBASE_IMPEXP int SplitArrayString(const char *szStr, VString &tail);

/// \brief
/// Helper class to set the C locale when performing string operations.
///
/// This class basically wraps around the setlocale() function in C and always works on the entire locale (LC_ALL).
/// The intention is that you can temporarily change the locale inside scopes by locally instantiating this class.
///
/// Note that platforms without locale support will simply ignore the locale information set with this class.
class VLocaleFixer
{
public:

  #ifndef _VISION_PSP2

    /// \brief
    ///   Constructor of the locale specifier that allows specifying a custom locale. 
    /// \param szLocale Use NULL to initialize "English" locale or ".ACP" for the system's ASCI code page
    VLocaleFixer(const char * szLocale)
    {
      m_sLocale = setlocale (LC_ALL, NULL); ///< Get locale for reset
      setlocale (LC_ALL, szLocale == NULL ? "English" : szLocale);  ///< Make sure we write fractions with . and not ,
    }

    /// \brief
    /// Locale specifier constructor. Sets the locale to "English"
    VLocaleFixer()
    {
      m_sLocale = setlocale (LC_ALL, NULL); ///< Get locale for reset
      setlocale (LC_ALL, "English");  ///< Make sure we write fractions with . and not ,
    }
    
    /// \brief
    /// Locale fixer destructor. Sets the locale back to the value it had when the VLocaleFixer constructor was called.
    ~VLocaleFixer()
    {
      setlocale(LC_ALL, m_sLocale);
    }
    
    VString m_sLocale;

  #endif
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
