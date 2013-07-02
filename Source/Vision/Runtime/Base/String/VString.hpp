/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VString.hpp

#ifndef VISION_VSTRING_HPP
#define VISION_VSTRING_HPP


/////////////////////////////////////////////////////////////////////////////////////
// String helper functions
/////////////////////////////////////////////////////////////////////////////////////

//See if a string ends with a specific character
VBASE_IMPEXP bool EndsWith(const char *szString, char ch);

//See if a string ends with a specific string (case sensitive)
VBASE_IMPEXP bool EndsWith(const char *szString, const char *szEnd);

//See if a string ends with a specific string (case insensitive)
VBASE_IMPEXP bool EndsWithI(const char *szString, const char *szEnd);

// See if a string contains a specific sub-string (case sensitive)
// 
// If specified sub-string is contained, pointer to the first occurrence of pszSubstr in pszString is returned,
// otherwise the return-value is NULL
//
// \param szString
//   String to search in
// \param szSubstr
//   String to search for within pszString
// \return
//   Pointer to first occurrence of pszSubstr in pszString, or NULL of pszSubstr was not found
VBASE_IMPEXP const char* Contains(const char *szString, const char *szSubstr);

// See if a string contains a specific sub-string (case insensitive)
// 
// If specified sub-string is contained, pointer to the first occurrence of pszSubstr in pszString is returned,
// otherwise the return-value is NULL
//
// \param szString
//   String to search in
// \param szSubstr
//   String to search for within pszString
// \return
//   Pointer to first occurrence of pszSubstr in pszString, or NULL of pszSubstr was not found
VBASE_IMPEXP const char* ContainsI(const char *szString, const char *szSubstr);

/// \brief
///   Class providing common helper functionality for string operations
class VStringHelper
{
public:

  /// \brief
  ///   Checks whether a string starts with a specific substring (case sensitive)
  VBASE_IMPEXP static bool StartsWith(const char *szString, const char *szStart);

  /// \brief
  ///   Checks whether a string starts with a specific substring (case insensitive)
  VBASE_IMPEXP static bool StartsWithI(const char *szString, const char *szStart);

  /// \brief
  ///   This string comparison method gracefully handles all combinations of passing NULL. A NULL string equals an empty string.
  VBASE_IMPEXP static int SafeCompare(const char *szA, const char *szB, bool bIgnoreCase=true);

  /// \brief
  ///   Converts a UCS-2 unicode character to lower case.
  VBASE_IMPEXP static wchar_t ToLowerCharUnicode(wchar_t wc);

  /// \brief
  ///   Converts a UCS-2 unicode character to upper case.
  VBASE_IMPEXP static wchar_t ToUpperCharUnicode(wchar_t wc);

  /// \brief 
  ///   Converts a UTF-8 string to lower case.
  ///
  /// \param szString
  ///   The string to convert.
  /// \param szOut
  ///   The output buffer (must be big enough), can be set to NULL in order to only compute
  ///   the required buffer size.
  /// \param iMaxSize
  ///   The size of the output buffer. If the string is longer than iMaxSize, it will be truncated accordingly.
  ///   If set to 0 this will be ignored.
  /// \param iStartCharacterIndex 
  ///   (Optional) Specify the start character (glyph) index for the toLower conversion. May be different
  ///   from the byte index for UTF8 strings.
  ///   Use GetUTF8CharacterIndex to convert a byte index to character index.
  /// \return
  ///   Returns the required/used output buffer size (including the trailing zero character).
  VBASE_IMPEXP static int ToLower(const char* szString, char* szOut, int iMaxSize=0, int iStartCharacterIndex=0);

  /// \brief 
  ///   Converts a UTF-8 string to upper case.
  ///
  /// \param szString
  ///   The string to convert.
  /// \param szOut
  ///   The output buffer (must be big enough), can be set to NULL in order to only compute
  ///   the required buffer size.
  /// \param iMaxSize
  ///   The size of the output buffer. If the string is longer than iMaxSize, it will be truncated accordingly.
  ///   If set to 0 this will be ignored.
  /// \param iStartCharacterIndex 
  ///   (Optional) Specify the start character (glyph) index for the toLower conversion. May be different
  ///   from the byte index for UTF8 strings.
  ///   Use GetUTF8CharacterIndex to convert a byte index to character index.
  /// \return
  ///   Returns the required/used output buffer size (including the trailing zero character).
  VBASE_IMPEXP static int ToUpper(const char* szString, char* szOut, int iMaxSize=0, int iStartCharacterIndex=0);

  /// \brief
  ///   Searches a string for a substring, with wildcard/joker support
  ///
  /// Both wildcards (*) and jokers (?) are supported.
  /// 
  /// \param szWildcards
  ///   Substring to search for.
  /// 
  /// \param szStr
  ///   The string in which to search.
  /// 
  /// \return
  ///   true (substring found in string), or false (no match)
  VBASE_IMPEXP static bool Match(const char* szWildcards, const char* szStr);
};

//Similar to strncpy
//The difference is that if a string of maximum length is copied
//only n-1 bytes will be copied and the string will be null terminated
//Also handles a NULL input
VBASE_IMPEXP char* vstrncpy(char *pDest, const char *pSrc, size_t iSize);

//Appends a string
VBASE_IMPEXP char* vstrncat(char *pDest, const char *pSrc, size_t iSize);

/// \brief
///   String class for ANSI and UTF8 strings.
class VString
{
  V_DECLARE_SERIALX( VString, VBASE_IMPEXP )
  friend class VStringHelper;

private:

  // Helper struct to store character type information in the first byte
  // before the actual string.
  // There are three states: 
  //  - not initialized (both set to false) -> content unknown
  //  - bOnlyANSI = true: string is known to contain only ANSI characters
  //  - bHasMbUTF8 = true: string might contain multi-bye UTF-8 characters
  struct VCharTypeFlags
  {
    bool bOnlyANSI : 1; // string only contains ANSI characters
    bool bHasMbUTF8 : 1; // string contains some UTF-8 characters
  };

  /// \brief
  ///   IndexProxy is used to control write access to a specific character position in the string.
  ///
  /// This is necessary to handle different string types (ANSI vs. UTF-8) through the same API. 
  /// Only API functions giving write access to string date use this proxy object; it is therefore only used if the string is modifiable (non-const).
  class VStringCharacterIndexProxy
  {
  public:
    VStringCharacterIndexProxy(const int iPos, VString& string) 
      : m_iPos(iPos), m_str(string) 
    {
      VASSERT(iPos >= 0 && m_iPos <= m_str.GetLen());
    }

    /// \brief
    ///   Assigns a new char value to this character.
    VBASE_IMPEXP char operator =(char c);

    /// \brief
    ///   Assigns a new wchar_t value to this character.
    VBASE_IMPEXP wchar_t operator =(wchar_t wc);

    /// \brief
    ///   Reads a character from the string
    inline operator char() const
    {
      int iOffset = GetUTF8CharacterOffset(m_str.m_pString, m_iPos, &m_str);
      VASSERT(iOffset >= 0 && iOffset <= m_str.GetSize());
      return m_str.m_pString[iOffset];
    }

    /// \brief
    ///   Reads a character from the string
    inline char* operator&()
    {
      int iOffset = GetUTF8CharacterOffset(m_str.m_pString, m_iPos, &m_str);
      VASSERT(iOffset >= 0 && iOffset <= m_str.GetSize());

      return &m_str.m_pString[iOffset];
    }

  private:
    // Members
    const int m_iPos;
    VString& m_str;
  };

  // cast to bool not supported
  operator bool() const;

public:
  ///
  /// @}
  ///

  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  /// \brief
  ///   Copy constructor.
  VBASE_IMPEXP VString(const VString& str);

  /// \brief
  ///   Constructor that copies the string from a char pointer.
  VBASE_IMPEXP VString(const char* str);

  /// \brief
  ///   Constructor that copies the string from a char pointer, using the specified string size in bytes.
  VBASE_IMPEXP VString(const char* str, int iSize);

  /// \brief
  ///   Constructor that copies the string from a single char.
  VBASE_IMPEXP VString(const char c);

  /// \brief
  ///   Constructor that copies the string from a wide char pointer.
  VBASE_IMPEXP VString(const wchar_t *str);

  /// \brief
  ///   Constructor that creates an empty string.
  inline VString() 
    : m_pString(NULL) 
  {}

  /// \brief
  ///   Destructor; releases memory allocated by the VString object.
  VBASE_IMPEXP ~VString();               

  ///
  /// @}
  ///

  ///
  /// @name Assignment Operators
  /// @{
  ///

  /// \brief
  ///   Copies the string from a char pointer. The pointer can be NULL.
  VBASE_IMPEXP VString& operator =(const char* str);

  /// \brief
  ///   Copies the string from another string object.
  VBASE_IMPEXP VString& operator =(const VString& str);

  template <class T>
  inline const VString operator +(T var) const           
  { 
    VString tstr = *this; 
    return tstr += var; 
  }

  /// \brief
  ///   Appends a string to this string
  inline VString& operator +=(const char* str)  
  { 
    return *this += (VString)str; 
  }

  /// \brief
  ///   Appends a string to this string
  VBASE_IMPEXP VString& operator +=(const VString& str);

  ///
  /// @}
  ///

  ///
  /// @name Logic Operators
  /// @{
  ///

  /// \brief
  ///   Compares two strings (case sensitive).
  inline bool operator !=(const char* str) const
  { 
    if(str && m_pString) 
      return strcmp(str, m_pString) != 0;

    return (str != m_pString); ///< this covers the case, both are NULL pointers
  }

  /// \brief
  ///   Compares two strings (case sensitive).
  inline bool operator !=(char* str) const
  { 
    if(str && m_pString)
      return strcmp(str, m_pString) != 0;

    return (str!=m_pString); ///< this covers the case, both are NULL pointers
  }

  /// \brief
  ///   Compares two strings (case sensitive).
  inline bool operator !=(const VString &other) const 
  { 
    return !(*this == other); 
  }

  /// \brief
  ///   Compares two strings (case sensitive).
  VBASE_IMPEXP bool operator ==(const VString& string) const;

  /// \brief
  ///   Compares two strings (case sensitive).
  VBASE_IMPEXP bool operator ==(const char *szString) const;

  /// \brief
  ///   Compares two strings (case sensitive).
  VBASE_IMPEXP bool operator ==(char *szString) const;
  
  /// \brief
  ///   Compares two strings (case sensitive). 
  VBASE_IMPEXP bool operator <(const VString& string) const;

  ///
  /// @}
  ///

  ///
  /// @name Modification Functions
  /// @{
  ///

  /// \brief
  ///   Converts the string to upper case characters.
  ///
  /// Causes the string data to be re-allocated if the string is a UTF-8 string.
  /// 
  /// \param iStartCharacterIndex 
  ///   (Optional) Specify the start character (glyph) index for the toLower conversion 
  ///   (may be different from the byte index).
  ///   Use GetUTF8CharacterIndex to convert a byte index to a character index.
  VBASE_IMPEXP void ToUpper(int iStartCharacterIndex = 0);

  /// \brief
  ///   Converts the string to lower case characters.
  ///
  /// Causes the string data to be re-allocated if the string is a UTF-8 string.
  /// 
  /// \param iStartCharacterIndex 
  ///   (Optional) Specify the start character (glyph) index for the toLower conversion 
  ///   (may be different from the byte index).
  ///   Use GetUTF8CharacterIndex to convert a byte index to character index.
  VBASE_IMPEXP void ToLower(int iStartCharacterIndex = 0);
  
  // Obsolete function.
  inline void SetSafeStr(const char *pString) 
  { 
    *this = pString; // Calls the assignment operator. 
  }

  /// \brief
  ///   Always returns a valid pointer (non-NULL). Instead of NULL, this will return an empty string.
  inline const char* GetSafeStr() const   
  { 
    return (m_pString == NULL) ? GetEmptyString() : m_pString;
  }

  ///
  /// @}
  ///

  ///
  /// @name Conversion Functions
  /// @{
  ///

  /// \brief
  ///   Returns a pointer to the string data. This operator will never return a NULL pointer; if the internal string data isn't allocated yet, this function returns an empty string.
  inline operator const char* () const   
  { 
    return GetSafeStr(); 
  }

  /// \brief
  ///   Returns a pointer to the string data.
  inline char* GetChar()
  { 
    // Clear character type bits.
    if (m_pString != NULL)
      _ClearCharTypeFlags(); 
    return m_pString; 
  }

  /// \brief
  ///   Returns a const pointer to the string data.
  inline const char* AsChar() const      
  {
    return GetSafeStr();
  }
  
  /// \brief
  ///   Template function to parse the string as a numerical value
  ///
  /// \param var
  ///   The variable in which to store the numerical value.
  ///
  /// Supported template types are:
  /// \li <tt>bool</tt>
  /// \li <tt>char</tt>
  /// \li <tt>short</tt>
  /// \li <tt>int</tt>
  /// \li <tt>long</tt>
  /// \li <tt>float</tt>
  /// \li <tt>double</tt>
  /// \li <tt>unsigned char</tt>
  /// \li <tt>unsigned short</tt>
  /// \li <tt>unsigned int</tt>
  /// \li <tt>unsigned long</tt>
  ///
  /// \return 
  ///   true if the value could be extracted.
  template <class T> bool GetValue(T& var) 
  { return GetVar(var); }

  ///
  /// @}
  ///

  ///
  /// @name Search
  /// @{
  ///

  /// \brief
  ///   Searches a string for a substring, with wildcard/joker support
  ///
  /// Both wildcards (*) and jokers (?) are supported.
  /// 
  /// \param szWildcards
  ///   Substring to search for.
  /// 
  /// \return
  ///   bool bResult : true (substring found in string), or false (no match)
  inline bool Search(const char* szWildcards) const
  { 
    return VStringHelper::Match(szWildcards, m_pString);
  }

  /// \brief
  ///   Finds the first occurrence of a character in the string and returns its position (or -1 if the
  ///   character is not contained in the string). 
  ///
  /// The returned position is the character index, not the byte offset (which isn't necessarily the same for UTF8 strings).
  ///
  /// \param c
  ///   The character to search for.
  inline int FindChar(char c) const
  {
    const char *p = strchr(GetSafeStr(), static_cast<int>(c));
    if (!p) 
      return -1;
    return GetUTF8CharacterIndex(m_pString, static_cast<int>(p - m_pString), 
      const_cast<VString*>(this));
  }

  /// \brief
  ///   Finds the first occurrence of a character in the string and return the position (or -1 if the
  ///   character is not contained in the string). 
  ///
  /// The returned position is the character index, not the byte offset (which isn't necessarily the same for UTF8 strings).
  ///
  /// \param c
  ///   The character to search for.
  inline int FindChar(wchar_t c) const
  {
    char utf8[3];
    const int iUTF8Size = ConvertWCharToUTF8(c, utf8);
    VASSERT(iUTF8Size >= 1);

    if (iUTF8Size == 1)
      return FindChar(utf8[0]);
    else
      return Find(utf8, false, 0);
  }

  ///
  /// @}
  ///

  ///
  /// @name String Properties
  /// @{
  ///

  /// \brief
  ///   Returns the length (number of characters) of the string.
  ///
  /// \note
  ///   If the string contains UTF8 characters, the byte size of the string may not be the same as the number of characters.
  inline int GetLen() const
  {
    if (m_pString == NULL) 
      return 0;

    if (_GetCharTypeFlags().bOnlyANSI)
      return static_cast<int>(strlen(m_pString));

    return GetUTF8CharacterIndex(m_pString, -1, const_cast<VString*>(this));
  }

  /// \brief
  ///   Returns the size of the string in bytes (excluding the
  ///   trailing zero character).
  inline int GetSize() const
  { 
    return (m_pString != NULL) ? static_cast<int>(strlen(m_pString)) : 0; 
  }

  /// \brief
  ///   Returns true if the internal string data is NULL or the string length is zero.
  inline bool IsEmpty() const 
  { 
    return (m_pString == NULL || m_pString[0] == 0); 
  }

  /// \brief
  ///   Returns true if the string contains a UTF8 character that is >= 2 bytes long.
  inline bool ContainsMultiByteCharacters() const
  {
    if (_GetCharTypeFlags().bHasMbUTF8) return true;
    if (_GetCharTypeFlags().bOnlyANSI) return false;
    
    // update character type flags by calling GetLen()
    return (GetLen() < GetSize());
  }

  /// \brief
  ///   Returns true if the string contains a UTF8 character that is >= 2 bytes long.
  ///
  /// \param szStr
  ///   The input string
  static inline bool ContainsMultiByteCharacters(const char *szStr)
  {
    if ( szStr != NULL )
    {
      for ( ; *szStr != '\0'; ++szStr )
      {
        if ( ( *szStr & 0x80 ) != 0 )   // If the MSB is set,...
        {
          return true;  //  ...we are dealing with a multi-byte character
        }
      }
    }
    return false;
  }

  /// \brief
  ///   Returns the size of the character at position iPos. May be greater than 1
  ///   if UTF8 characters are used.
  inline int GetCharacterSize(int iCharPos) const
  { 
    return GetUTF8CharacterSize(&( operator[](iCharPos) )); 
  }

  /// \brief
  ///   Index operator
  inline const char &operator [](int iPos) const
  {
    const int iOffset = GetUTF8CharacterOffset(m_pString, iPos, const_cast<VString*>(this));
    VASSERT(iOffset >= 0 && iOffset <= GetSize());
    return m_pString[iOffset];
  }

  /// \brief
  ///   Index operator
  inline VStringCharacterIndexProxy operator [](int iPos)
  {
    // Use proxy object to detect writes and to be able to modify
    // the character type flags accordingly.
    return VStringCharacterIndexProxy(iPos, *this);
  }

  ///
  /// @}
  ///

  ///
  /// @name String Formatting
  /// @{
  ///

  /// \brief
  ///   Formats the string according to the standard printf rules.
  /// 
  /// \param format
  ///   The string format.
  /// 
  /// \return
  ///   The size of the output string.
  VBASE_IMPEXP int Format(const char* format, ...);

  /// \brief
  ///   Formats the string according to the standard printf rules.
  /// 
  /// Use it just as you would use the printf function. This version
  /// handles Latin-1 strings and converts them to UTF-8 before formatting.
  /// 
  /// \param format
  ///   The string format.
  /// 
  /// \return
  ///   The size of the output string.
  VBASE_IMPEXP int FormatLatin1(const char* format, ...);
  
  /// \brief
  ///   Same as Format(), but takes a va_list argument.
  inline int FormatArgList( const char* format, va_list arglist) 
  { 
    return _Format(format, arglist); 
  }

  /// \brief
  ///   Formats the string.
  /// 
  /// Similar to the Format() method. In addition, this method supports the %h formatting flag. This
  /// flag works the same way as the %f flag, but it cuts off trailing zeros after the decimal point.
  /// 
  /// \return
  ///   Size of the output string
  /// 
  /// \param format
  ///   The string format.
  /// 
  /// \example
  ///   \code
  ///   VString str;
  ///   str.Format( "%h", 3.14 ); // output string is "3.14"
  ///   str.Format( "%f", 3.14 ); // output string is "3.140000"
  ///   str.Format( "%.3h", 2.10003 ); // output string is "2.1"
  ///   str.Format( "%d + %f = %h", 1, 2.f, 3.f ); // output string is "1 + 2.000000 = 3"
  ///   \endcode
  VBASE_IMPEXP int FormatEx( const char* format, ... );

  /// \brief
  ///   Resets the string. Afterwards, IsEmpty() will return true.
  VBASE_IMPEXP void Reset();
  
  /// \brief
  ///   Copies a substring of another string into this string object.
  /// 
  /// Takes the left (count) characters of the passed string (str) and copies these characters into
  /// this VString object.
  /// 
  /// \param szStr
  ///   The pointer to the string from which the characters are copied.
  /// 
  /// \param iCount
  ///   The number of character to copy into this VString object.
  /// 
  /// \example
  ///   \code
  ///   VString a("Hello World");
  ///   VString b;
  ///   b.Left(a, 5);
  ///   VASSERT(!strcmp(b, "Hello");
  ///   \endcode
  VBASE_IMPEXP void Left(const char* szStr, int iCount);

  /// \brief
  ///   Truncates the string to a specific byte size.
  ///
  /// This is particularly useful when handling UTF8 strings (byte size != character length).
  /// Truncation always works on full characters.
  ///
  /// \param szStr
  ///   The pointer to the string from which the characters are copied.
  ///
  /// \param iByteSize
  ///   The maximum byte size of the string afterwards.
  VBASE_IMPEXP void TruncateToSize(const char* szStr, int iByteSize);

  /// \brief
  ///   Checks whether the string ends with a specific character.
  inline bool EndsWith(char ch) const
  {
    return ::EndsWith(AsChar(), ch);
  }

  /// \brief Checks whether the string ends with a specific substring.
  /// \param szEnd The substring against which to compare the string ending.
  /// \param bCaseSensitive (\b optional) true to compare case sensitive, otherwise false (default).
  inline bool EndsWith(const char * szEnd, bool bCaseSensitive=false) const
  {
    if(bCaseSensitive)
      return ::EndsWith(AsChar(), szEnd);
    else
      return ::EndsWithI(AsChar(), szEnd);
  }

  /// \brief
  ///   Inserts character c before position iPos. if iPos==length, it appends the character.
  VBASE_IMPEXP void InsertAt(int iPos, char c);

  /// \brief
  ///   Insert str before position iPos. if iPos==length, the string is appended.
  VBASE_IMPEXP void InsertAt(int iPos, const VString &str);

  /// \brief
  ///   Removes the character at position iPos. iPos is zero-based and must be in valid range
  ///   [0..length-1].
  VBASE_IMPEXP void RemoveAt(int iPos);

  /// \brief 
  /// Searches for szPattern and returns its position. Returns -1 if nothing is found.
  inline int Find(const char *szPattern, bool bCaseSensitive = false, int iStartSearchPos = 0) const
  {
    int iOffset = _Find(szPattern, bCaseSensitive, iStartSearchPos);
    if (iOffset == -1) return -1;
    return GetUTF8CharacterIndex(m_pString, iOffset, const_cast<VString*>(this));
  }

  /// \brief
  ///   Replaces a substring with another one. Returns true if pszPattern was found and replaced, false if nothing was done.
  VBASE_IMPEXP bool Replace(const char *szPattern, const char *szReplace, bool bCaseSensitive = false, int iStartSearchPos = 0);

  /// \brief
  ///   Replaces all occurrences of a substring with another one. Returns the number of replacements that were done.
  VBASE_IMPEXP int ReplaceAll(const char *szPattern, const char *szReplace, bool bCaseSensitive = false);

  /// \brief
  /// Replaces the part between [iStartPos; iStartPos+iWordLength-1] with szReplacement.
  inline void ReplaceAt(int iStartPos, int iWordLength, const char *szReplacement)
  {
    const int iStartOffset = GetUTF8CharacterOffset(m_pString, iStartPos, this);
    const int iEndOffset = GetUTF8CharacterOffset(m_pString, iStartPos+iWordLength, this);
    _ReplaceAt(iStartOffset, iEndOffset - iStartOffset, szReplacement);
  }

  /// \brief Callback that returns whether c is a delimiter for words.
  /// \param c
  ///   A character before or after a word.
  /// \param bFront
  ///   True if the character c is before the word, false if it is after the word.
  /// \return
  ///   Should return true to indicate that c is a delimiter, false if it isn't.
  typedef bool(*IS_DELIMITER)(char c, bool bFront);

  /// \brief 
  ///   A default-delimiter function to find whole words in typical C code.
  ///   The characters [a-z;A-Z;_;0-9] are considered NOT to be delimiters, everything else is.
  VBASE_IMPEXP static bool Delimiter_CodeIdentifier(char c, bool bFront);
  
  /// \brief 
  ///   A default-delimiter function to find whole words in English language text.
  ///   The characters [a-z;A-Z;_;0-9] and the minus-sign (-) are considered NOT to be delimiters, everything else is.
  VBASE_IMPEXP static bool Delimiter_EnglishText(char c, bool bFront);

  /// \brief 
  ///   Searches for a given word, checks that it is a whole word by calling the two callbacks. Returns the start position of the word, or -1 if nothing is found.
  inline int FindWholeWord(const char* szSearchForWord, IS_DELIMITER WordDelimiter, 
    bool bCaseSensitive = false, int iStartSearchPos = 0) const
  {
    const int iStartSearchOffset = GetUTF8CharacterOffset(m_pString, iStartSearchPos, const_cast<VString*>(this));
    const int iWordStartOffset = _FindWholeWord(szSearchForWord, WordDelimiter, bCaseSensitive, iStartSearchOffset);
    if(iWordStartOffset == -1) return -1;
    return GetUTF8CharacterIndex(m_pString, iWordStartOffset, const_cast<VString*>(this));
  }

  /// \brief 
  ///   Searches for a given word, checks that it is a whole word by calling the two callbacks and replaces it if both return true.
  VBASE_IMPEXP bool ReplaceWholeWord(const char* szSearchForWord, const char *szReplacement, 
    IS_DELIMITER WordDelimiter, bool bCaseSensitive = false, int iStartSearchPos = 0);

  /// \brief 
  ///   Calls ReplaceWholeWord until nothing is replaced anymore. Returns how many words were replaced.
  VBASE_IMPEXP int ReplaceWholeWordAll(const char* szSearchForWord, const char *szReplacement, 
    IS_DELIMITER WordDelimiter, bool bCaseSensitive = false, int iStartSearchPos = 0);

  ///
  /// @}
  ///

  ///
  /// @name Misc
  /// @{
  ///

  /// \brief
  ///   Converts locale specific characters
  /// 
  /// This function converts locale-specific characters (right now just decimal point and thousand
  /// separator) to the internal format (decimal to '.', thousand separator to ',').
  VBASE_IMPEXP void ConvertToLocale();

  /// \brief
  ///   This function does the opposite of VString::ConvertToLocale(). See that function.
  VBASE_IMPEXP void ConvertFromLocale();

  /// \brief
  ///   Checks if the internal string state is valid. Useful for debugging.
  VBASE_IMPEXP void AssertValid() const;

  /// \brief
  ///   Returns true if the given string is valid UTF8.
  VBASE_IMPEXP static bool IsValidUTF8String(const char *szStr);

  /// \brief
  ///   DEPRECATED: Use 'CompareI' instead and check for a return value of zero.
  HKV_DEPRECATED_2013_1 VBASE_IMPEXP bool CompareNoCase(const char* szOtherString) const;

  /// \brief
  ///   Compares two strings (case-insensitive)
  VBASE_IMPEXP int CompareI(const char* szOtherString) const;

  /// \brief
  ///   Returns an empty string ("")
  inline static const char *GetEmptyString() { return ""; }

  // numeric conversion helpers
  VBASE_IMPEXP bool NumericParse(void* pvar, char flag) const;
  inline bool GetVar(bool& var) const           { return NumericParse((void*)&var, 'b'); }
  inline bool GetVar(char& var) const           { return NumericParse((void*)&var, 'c'); }
  inline bool GetVar(short& var) const          { return NumericParse((void*)&var, 's'); }
  inline bool GetVar(int& var) const            { return NumericParse((void*)&var, 'i'); }
  inline bool GetVar(long& var) const           { return NumericParse((void*)&var, 'l'); }
  inline bool GetVar(float& var) const          { return NumericParse((void*)&var, 'f'); }
  inline bool GetVar(double& var) const         { return NumericParse((void*)&var, 'd'); }
  inline bool GetVar(unsigned char& var) const  { return NumericParse((void*)&var, 'C'); }
  inline bool GetVar(unsigned short& var) const { return NumericParse((void*)&var, 'S'); }
  inline bool GetVar(unsigned int& var) const   { return NumericParse((void*)&var, 'I'); }
  inline bool GetVar(unsigned long& var) const  { return NumericParse((void*)&var, 'L'); }

  /// \brief
  ///   Resets the string's type flags (UTF-8 vs. ANSI)
  ///
  /// This function needs to be called after the character buffer has been directly 
  /// modified from the outside (i.e. by calling GetChar()), as the string type (containing UTF8 characters or not)
  /// may have changed.
  ///
  /// The flags are used to optimize string operations for ANSI-only strings.
  inline void InvalidateUTF8State()
  { 
    if (m_pString) 
      _ClearCharTypeFlags(); 
  }

  ///
  /// @}
  ///

  ///
  /// @name UTF-8 Conversion
  /// @{
  ///


  /// \brief
  ///   Converts a wide char string to a UTF8 string.
  ///
  /// The trailing zero is not appended to the converted string.
  ///
  /// \param pIn
  ///   Pointer to the wide char input string (2 bytes per character)
  /// \param iLenIn
  ///   Length of the input strings in wide characters. (without trailing zero)
  /// \param szOut
  ///   Pointer to the UTF8 output string. May be NULL if only the
  ///   byte length of the resulting UTF8 string needs to be calculated.
  /// \param iOutSize
  ///   Size of the output buffer in bytes. As a wide character has a size of 2 bytes
  ///   a UTF8 character consists of at most 3 bytes. (without trailing zero)
  ///
  /// \return
  ///   Returns the actual number of bytes used in the output buffer. If the input 
  ///   string is invalid the return value is 0.
  ///
  /// \note
  ///   The trailing zero is not appended to the converted string. You will have to do this yourself.
  VBASE_IMPEXP static int ConvertWCharToUTF8String(const wchar_t *pIn, int iLenIn, 
    char *szOut, int iOutSize);

  /// \brief
  ///   Converts a wide character to a UTF8 character.
  ///
  /// \param in
  ///   The wide character to be converted. 
  /// \param szOut
  ///   Pointer to the UTF8 character. While a wide character has a size of 2 bytes, a UTF8 character consists of at most 3 bytes. May be NULL if only the
  ///   byte length of the resulting UTF8 character needs to be calculated.
  ///
  /// \return
  ///   Returns the lnegth of the UTF-8 character representation. If the input 
  ///   character is invalid, the return value is 0.
  VBASE_IMPEXP static int ConvertWCharToUTF8(const wchar_t in, char *szOut);

  /// \brief
  ///   Converts a UTF8 string to a wide char string. 
  ///
  /// The trailing zero is not appended to the converted string.
  ///
  /// \param szIn
  ///   Pointer to the UTF8 input string (at most 3 bytes per UTF8 character)
  /// \param iSizeIn
  ///   Size of the input buffer in bytes. (without trailing zero)
  /// \param pOut
  ///   Pointer to the wide char output string. May be NULL if only the length of
  ///   the resulting wide char string needs to be calculated.
  /// \param iLenOut
  ///   Size of the output buffer in wide characters. May be 0 if only the
  ///   UTF8 byte length needs to be calculated. (without trailing zero)
  ///
  /// \return
  ///   Returns the actual number of wide chars used in the output buffer. If the input 
  ///   string is invalid the return value is 0.
  ///
  /// \note
  ///   The trailing zero is not appended to the converted string. You will have to do this yourself.
  VBASE_IMPEXP static int ConvertUTF8ToWCharString(const char *szIn, int iSizeIn,
    wchar_t *pOut, int iLenOut);

  /// \brief
  ///   Converts a UTF8 character to a wide character.
  ///
  /// \param szIn
  ///   Pointer to the UTF8 character (at most 3 bytes per UTF8 character)
  /// \param pOut
  ///   Pointer to the output wide char output character. May be 0 if only 
  ///   the wide char and the resulting UTF8 character needs to be validated.
  ///
  /// \return
  ///   If the input character is invalid the return value is 0, otherwise it is 1.
  VBASE_IMPEXP static int ConvertUTF8ToWChar(const char *szIn, wchar_t *pOut);

  /// \brief
  ///   Returns the UTF-8 multi-byte character size.
  ///
  /// This method also detects invalid UTF-8 symbols and interprets them as
  /// extended ASCII (i.e. ANSI).
  ///
  /// \param pChar
  ///   Pointer to the first byte of the UTF8 character.
  ///
  /// \param pEnd
  ///   Pointer to the first byte that doesn't belong to the given string.
  ///   (optional if string is null-terminated)
  VBASE_IMPEXP static int GetUTF8CharacterSize(const char *pChar, const char *pEnd=NULL);

  /// \brief
  ///   Returns the UTF8 multi-byte character size.
  ///
  /// \param wc
  ///   Unicode wide character subject to UTF8 conversion.
  static inline int GetUTF8CharacterSize(const wchar_t wc)
  {
    // Only check for 3 byte utf8 characters because wide characters are 2 bytes long.
    if (wc <= 0x007f)      return 1;
    else if (wc <= 0x07ff) return 2;
    else                   return 3; // if (in <= 0x0000ffff)
  }
  
  /// \brief
  ///   Computes the byte offset of the UTF8 character at the specified index
  ///
  /// \param szStr
  ///   Pointer to a UTF8 string.
  /// \param iPos
  ///   The index of the character to get the offset from. Specify as -1 in order to
  ///   traverse the whole string and get the string's byte size.
  ///
  /// \return
  ///   Returns the byte offset of the character.
  inline static int GetUTF8CharacterOffset(const char *szStr, int iPos)
  { 
    if (szStr == NULL)
      return 0;

    return GetUTF8CharacterOffset(szStr, iPos, NULL); 
  }

  /// \brief
  ///   Computes the character index of the UTF8 character at the specified memory offset.
  ///
  /// \param szStr
  ///   Pointer to a UTF8 string.
  /// \param iOffset
  ///   The memory offset of the character to get the index from. Specify as -1 in order to
  ///   traverse the whole string and get the character length.
  ///
  /// \return
  ///   Returns the index of the character.
  inline static int GetUTF8CharacterIndex(const char* szStr, int iOffset)
  { 
    return GetUTF8CharacterIndex(szStr, iOffset, NULL); 
  }

  /// \brief
  ///   Computes the start pointer of a UTF-8 character.
  ///
  /// This function can be used to traverse a UTF-8 string backwards.
  ///
  /// \param szUTF8Char
  ///   A pointer to a UTF-8 character. May point to any byte of the character.
  ///
  /// \param pStart
  ///   Pointer to the first byte of the string that contains the character.
  ///
  /// \param pEnd
  ///   Pointer to the first byte that doesn't belong to the given string.
  ///   (optional if string is null-terminated)
  VBASE_IMPEXP static const char* GetUTF8CharacterStartPointer(const char* szUTF8Char, 
    const char* pStart, const char* pEnd=NULL);

  /// \brief
  ///   Converts a Latin-1 ANSI string (which uses 8 bits instead of 7 bits)
  ///   to a valid UTF8 string. 
  ///
  /// An invalid ASCII string is specified if language-specific characters such as umlauts are used as normal
  /// ASCII characters (8 bit). This does not represent a valid UTF-8 string. 
  /// The conversion is done by assuming that characters that have a value > 127 are the lower
  /// byte of a unicode wide character. This corresponds to the 8 bit ISO Latin-1 encoding.
  /// The trailing zero is not appended to the converted string.
  ///
  /// \note
  ///   The trailing zero is not appended to the converted string. You will have to do this yourself.
  ///
  /// \param szIn
  ///   Pointer to a Latin-1 ANSI string. (characters may use 8 bits)
  /// \param iSizeIn
  ///   Size of the input buffer in bytes. (without trailing zero)
  /// \param szOut
  ///   Pointer to the UTF-8 output string. May be NULL if only the size of
  ///   the resulting UTF-8 string needs to be calculated.
  /// \param iSizeOut
  ///   Size of the output buffer. May be 0 if the string length 
  ///   needs to be calculated only.
  ///
  /// \return
  ///   Returns the actual size used in the output buffer.
  VBASE_IMPEXP static int ConvertLatin1ToUTF8(const char *szIn, int iSizeIn, 
    char *szOut, int iSizeOut);

  /// \brief
  ///   Converts a UTF8 string to a Latin-1 ANSI string (which uses 8 bits instead of 7 bits). 
  ///
  /// The resulting encoding corresponds to the 8 bit ISO Latin-1 encoding.
  /// The trailing zero is not appended to the converted string.
  ///
  /// \note
  ///   The trailing zero is not appended to the converted string. You will have to do this yourself.
  ///
  /// \note
  ///   Only use this helper function if absolutely necessary.
  ///
  /// \param szIn
  ///   Pointer to an UTF-8 string.
  /// \param iSizeIn
  ///   Size of the input buffer in bytes. (without trailing zero)
  /// \param szOut
  ///   Pointer to the ANSI output string. May be NULL if only the size of
  ///   the resulting string needs to be calculated.
  /// \param iSizeOut
  ///   Size of the output buffer. May be 0 if the string length 
  ///   needs to be calculated only. (without trailing zero)
  ///
  /// \return
  ///   Returns the actual size used in the output buffer.
  VBASE_IMPEXP static int ConvertUTF8ToLatin1(const char *szIn, int iSizeIn, 
    char *szOut, int iSizeOut);

  ///
  /// @}
  ///


protected:
  VBASE_IMPEXP void SerializeX(VArchive& ar);

  // data block
  /*  The byte before the pointer stores flags to determine if a string
      only contains ANSI characters or UTF-8 characters too. These flags
      will be set when the string has been traversed.
      Pure ANSI strings allow a faster random access to the characters. */
  char* m_pString;

#define VSTRINGLOCALINFOSIZE 4
  static char m_szDecimal[VSTRINGLOCALINFOSIZE+1];
  static char m_szThousand[VSTRINGLOCALINFOSIZE+1];

  ///
  /// @}
  ///

private:
  VBASE_IMPEXP int _Format(const char* format, va_list arglist);
  // Internal version of Find (Returns byte offset instead of character position -> UTF8)
  VBASE_IMPEXP int _Find(const char *szPattern, bool bCaseSensitive, int iStartSearchPos) const;
  // Internal version of ReplaceAt (only handles byte offsets instead of character positions)
  VBASE_IMPEXP void _ReplaceAt(int iStartOffset, int iWordSize, const char *szReplacement);
  // Internal version of FindWholeWord (only handles byte offsets instead of character positions)
  VBASE_IMPEXP int _FindWholeWord(const char* szSearchForWord, IS_DELIMITER WordDelimiter, 
    bool bCaseSensitive, int iStartSearchOffset) const;

  // UTF8 version for converting the characters of a string (eg. ToLower/ToUpper). 
  // Make sure to handle memory deallocation for szOut. 
  // Returns true if the new string contains characters >= 2 bytes.
  static bool _ConvertUTF8Characters(const char *szIn, char *&szOut, wchar_t (*wcharConversionFunc)(wchar_t), int iStartIndex=0); 

  /// UTF8 conversion helpers
  static bool _IsWCharForbidden(wchar_t wc);
  static bool _IsUTF8CharForbidden(unsigned char uc);
 
  // Specify pStr (not NULL) to use and update character type flags.
  // szStr and strFlagPtr (without the char type flags) should be part of the same string object then.
  inline static int GetUTF8CharacterOffset(const char *szStr, const int iPos, VString *pStr)
  {
    if (szStr == NULL)
      return 0;

    // offset equals index for ANSI strings (early out)
    if (iPos != -1 && pStr && pStr->_GetCharTypeFlags().bOnlyANSI) 
      return iPos;

    return _GetUTF8CharacterOffset(szStr, iPos, pStr);
  }

  // _GetUTF8CharacterOffset without ANSI early out
  VBASE_IMPEXP static int _GetUTF8CharacterOffset(const char *szStr, int iPos, VString *pStr);

  // Specify pStr (not NULL) to use and update character type flags.
  // szStr and strFlagPtr (without the char type flags) should be part of the same string object then.
  inline static int GetUTF8CharacterIndex(const char *szStr, const int iOffset, VString *pStr)
  {
    // index equals offset for ANSI strings (early out)
    if (iOffset != -1 && pStr && pStr->_GetCharTypeFlags().bOnlyANSI)
      return iOffset;

    return _GetUTF8CharacterIndex(szStr, iOffset, pStr);
  }

  // _GetUTF8CharacterIndex without ANSI early out
  VBASE_IMPEXP static int _GetUTF8CharacterIndex(const char *szStr, const int iOffset, VString *pStr);

  // Other UTF-8 helpers
  inline VCharTypeFlags& _GetCharTypeFlags()
  {
    VASSERT(m_pString != NULL);
    return *reinterpret_cast<VCharTypeFlags*>(m_pString - 1);
  }
  inline const VCharTypeFlags _GetCharTypeFlags() const
  {
    VASSERT(m_pString != NULL);
    return *reinterpret_cast<VCharTypeFlags*>(m_pString - 1);
  }

  inline void _ClearCharTypeFlags()
  {
    reinterpret_cast<unsigned char&>(_GetCharTypeFlags()) = 0;
  }

  inline bool _AreCharTypeFlagsInitialized() const
  { 
    const VCharTypeFlags ctf = _GetCharTypeFlags();
    return (ctf.bHasMbUTF8 || ctf.bOnlyANSI); 
  }

  // Helper function for updating the character type flags after another
  // string was inserted into this string object.
  inline void UpdateCharTypeFlagsAfterInsertionOf(const VString &sStr)
  {
    VCharTypeFlags& ctfThis = _GetCharTypeFlags();
    const VCharTypeFlags ctfOther = sStr._GetCharTypeFlags();

    if(ctfThis.bOnlyANSI)
    {
      if(ctfOther.bHasMbUTF8)
      {
        // string now contains UTF8 characters
        ctfThis = ctfOther;
      }
      else if(!ctfOther.bOnlyANSI)
      {
        // string may contain UTF8 characters now -> re-initialize
        _ClearCharTypeFlags();
      }
    }
  }

  friend class VHashString;
  friend class VStringCharacterIndexProxy;
};

/// \brief
///   Macro to convert a wide char string literal to a UTF8 VString.
///
/// Conversion is performed at runtime. Allocates memory for the converted string.
#define V_UTF8(szStr) (VString(L##szStr))

/// \brief
///   Macro to convert a wide character string to UTF8 using an output buffer.
///
/// Also sets the trailing zero character, so wcharBufferLen has to have enough space for it.
#define V_WCHAR_TO_UTF8(pWCharStr, szUTF8Buffer, iUTF8BufferLen) \
  { \
    const int _iUTF8Size_ = VString::ConvertWCharToUTF8String( \
      pWCharStr, static_cast<int>(wcslen(pWCharStr)), szUTF8Buffer, iUTF8BufferLen); \
    VASSERT_MSG(_iUTF8Size_ < static_cast<int>(iUTF8BufferLen), "UTF8 character buffer too small"); \
    (szUTF8Buffer)[_iUTF8Size_] = '\0'; \
  }

/// \brief
///   Macro to convert a UTF8 string to wide char using an output buffer.
///
/// Also sets the trailing zero character, so wcharBufferLen has to have enough space for it.
#define V_UTF8_TO_WCHAR(szUTF8, wcharBuffer, iWCharBufferLen) \
  { \
    const int _iWCharLen_ = VString::ConvertUTF8ToWCharString( \
      szUTF8, static_cast<int>(strlen(szUTF8)), wcharBuffer, iWCharBufferLen); \
    VASSERT_MSG(_iWCharLen_ < static_cast<int>(iWCharBufferLen), "Wide character buffer too small"); \
    (wcharBuffer)[_iWCharLen_] = '\0'; \
  }

/// \brief
///   Macro to convert a UTF8 string to wide char using a temporary memory buffer minimizing
///   allocations on the heap.
///
/// Also sets the trailing zero character.
#define V_WCHAR_TO_UTF8_TEMPBUFFER(pWCharStr, szUTF8PointerOut, tempBuffer) \
  { \
  const int __iUTF8Len_ = VString::ConvertWCharToUTF8String(pWCharStr, static_cast<int>(wcslen(pWCharStr)), \
    NULL, 0) + 1; \
  tempBuffer.EnsureCapacity(__iUTF8Len_); \
  szUTF8PointerOut = static_cast<char*>(tempBuffer.GetBuffer()); \
  V_WCHAR_TO_UTF8(pWCharStr, szUTF8PointerOut, __iUTF8Len_); \
  }

/// \brief
///   Macro to convert a UTF8 string to wide char using a temporary memory buffer minimizing
///   allocations on the heap.
///
/// Also sets the trailing zero character.
#define V_UTF8_TO_WCHAR_TEMPBUFFER(szUTF8, wcharPointerOut, tempBuffer) \
  { \
  const int __iWCharLen_ = VString::ConvertUTF8ToWCharString(szUTF8, static_cast<int>(strlen(szUTF8)), \
    NULL, 0) + 1; \
  tempBuffer.EnsureCapacity(__iWCharLen_*sizeof(wchar_t)); \
  wcharPointerOut = static_cast<wchar_t*>(tempBuffer.GetBuffer()); \
  V_UTF8_TO_WCHAR(szUTF8, wcharPointerOut, __iWCharLen_); \
  }

/// \brief
///   Class for fixed size strings
/// 
/// This is a template class for strings of a fixed size specified by iSize.
/// 
/// This instance will not perform any memory allocation and should therefore be used in all cases where
/// performance is critical and the maximum size of a string is known in advance.
template<int iSize> class VStaticString
{
public:

  ///
  /// @name Constructor
  /// @{
  ///

  /// \brief
  ///   Constructor that resets the string
  inline VStaticString()
  {
    Reset();
  }

  /// \brief
  ///   Constructor that initializes a string with the given character array
  inline VStaticString(const char* pszString)
  {
    Reset();
    *this = pszString;
  }

  ///
  /// @}
  ///

  ///
  /// @name String Functions
  /// @{
  ///

  /// \brief
  ///   Resets the string to "".
  inline void Reset()
  {
    m_str[0] = 0;
  }

  /// \brief
  ///   Checks if the string is empty.
  inline bool IsEmpty() const
  {
    return m_str[0] == 0;
  }

  /// \brief
  ///   Returns the length of the string.
  inline int GetLength() const
  {
    return (int)strlen(m_str);
  }

  ///
  /// @}
  ///

  ///
  /// @name String Operators
  /// @{
  ///

  /// \brief
  ///   Assigns a string
  inline VStaticString<iSize>& operator = (const char *pStr)
  {
    if (pStr)
      vstrncpy(m_str,pStr,iSize+1);
    else
      m_str[0] = 0;
    return *this;
  }

  /// \brief
  ///   Appends a string.
  inline VStaticString<iSize>& operator += (const char *pStr)
  {
    if (pStr)
      vstrncat(m_str,pStr,iSize+1);
    return *this;
  }
  
  /// \brief
  ///   Returns a modifiable char* string buffer
  inline operator char*()
  {
    return m_str;
  }

  /// \brief
  ///   Returns a modifiable char* string buffer
  inline char* AsChar()
  {
    return m_str;
  }
  
  /// \brief
  ///   Returns a const char* string pointer
  inline const char* AsChar() const
  {
    return m_str;
  }

  /// \brief
  ///   Returns a const char* string pointer
  inline operator const char*() const
  {
    return m_str;
  }
  
  /// \brief
  ///   Accesses the string as an array of characters
  inline char &operator[] (int idx)
  {
    return m_str[idx];
  }

  /// \brief
  ///   Accesses the string as an array of characters
  inline char &operator[] (size_t idx)
  {
    return m_str[idx];
  }

  /// \brief
  ///   Compares two strings (case sensitive)
  inline bool operator ==(const char* str) const
  {
    if (!str || !str[0]) 
      return IsEmpty();
    return strcmp(m_str,str)==0;
  }

  /// \brief
  ///   Compares two strings (case sensitive)
  inline bool operator !=(const char* str) const
  {
    return !((*this) == str);
  }

  /// \brief
  ///   Creates a formatted string. Works like sprintf.
  inline int Format (const char* format, ...)
  {
    va_list arglist;
    va_start (arglist, format);

    int ret = vsnprintf(m_str, iSize, format, arglist);

    va_end (arglist);
    return ret;
  }

  char m_str[iSize+1];
  ///
  /// @}
  ///

};


/// \brief
///   Simple string class with optimized comparison operations using hashes.
///
/// This simple string class can be used in all cases where an efficient string comparison mechanism is
/// required.
class VHashString
{
public:
  V_DECLARE_SERIALX( VHashString, VBASE_IMPEXP )

  /// \brief
  ///   Computes and returns the hash value calculated from the string. 
  VBASE_IMPEXP static int GetHash(const char *szString);

  /// \brief
  ///   Constructor that copies the string from another VString object.
  VBASE_IMPEXP VHashString(const VString& str);

  /// \brief
  ///   Constructor that copies the string from a char pointer.
  VBASE_IMPEXP VHashString(const char* str);

  /// \brief
  ///   Constructor that creates an empty string.
  VBASE_IMPEXP VHashString();

  /// \brief
  ///   Returns the stored hash result.
  VBASE_IMPEXP int GetStoredHash();

  /// \brief
  ///   Returns a modifiable char* string buffer
  inline operator VString&()
  {
    return m_vString;
  }

  /// \brief
  ///   Returns a modifiable char* string buffer
  inline operator char*()
  {
    return m_vString.GetChar();
  }

  /// \brief
  ///   Returns a const char* string pointer.
  inline operator const char* () const   
  { 
    return (const char*)m_vString;
  }

  /// \brief
  ///   Copies the string from a char pointer. The pointer can be NULL.
  VBASE_IMPEXP VHashString& operator  =(const char* str);

  /// \brief
  ///   Copies the string from another string object.
  VBASE_IMPEXP VHashString& operator  =(const VString& str);

  /// \brief
  ///   Compares two strings for equality (case-sensitive). 
  VBASE_IMPEXP bool CompareNoCase(const char* szOtherString) const;

  /// \brief
  ///   Compares two strings for equality (case-insensitive, using an explicit hash value). 
  VBASE_IMPEXP bool CompareNoCase(const char* szOtherString, int hash) const;

  /// \brief
  ///   Compares two strings for equality (case-insensitive, using the hash values contained in the string objects).
  VBASE_IMPEXP bool CompareNoCase(const VHashString& other) const;

  /// \brief
  ///   Compares two strings (case sensitive, using the hash values contained in the string objects).
  VBASE_IMPEXP bool operator == (const VHashString& other) const;

  /// \brief
  ///   Compares two strings (case sensitive).
  VBASE_IMPEXP bool operator == (const VString &string) const;

  /// \brief
  ///   Compares two strings (case sensitive).
  VBASE_IMPEXP bool operator == (const char *szString) const;

  /// \brief
  ///   Compares two strings (case sensitive, using the hash values contained in the string objects).
  inline bool operator != (const VHashString &other) const { return !(*this == other); }

  /// \brief
  ///   Compares two strings (case sensitive).
  inline bool operator !=(const char* other) const { return !(*this == other); }

  /// \brief
  ///   Compares two strings (case sensitive).
  inline bool operator !=(const VString &other) const { return !(*this == other); }

  /// \brief
  ///   Returns a const char* string pointer.
  inline const char* AsChar() const      
  { 
    return m_vString.AsChar(); 
  }

  /// \brief
  ///   Returns true if the string is NULL or the string length is zero.
  inline bool IsEmpty() const {return m_vString.IsEmpty();}

  /// \brief
  /// Recalculates the hash value
  VBASE_IMPEXP void Refresh();

  /// \brief
  /// Serialization function
  VBASE_IMPEXP void SerializeX( VArchive& ar );

protected:

  VString m_vString;
  int m_hash;
};

/// \brief serialization

#include <Vision/Runtime/Base/System/IO/Serialization/VArchive.hpp> //need to include this here because of circular dependency

template <int iSize>
VArchive& operator >> (VArchive& ar, VStaticString<iSize>& obj )
{
  ar.ReadStringBinary(obj.AsChar(), iSize+1);
  return ar;
}

/// \brief serialization
template <int iSize>
VArchive& operator << (VArchive& ar, VStaticString<iSize>& obj)
{
  ar.WriteStringBinary(obj.AsChar());
  return ar;
}


/////////////////////////////////////////////////////////////////////////////////////
// This macros is used to create a VString variable with the formatted text
// see VLog::Print for an usage example
/////////////////////////////////////////////////////////////////////////////////////

#define GENERATE_ARGUMENTSTRING(vstringvariablename, constchartext) \
  VString vstringvariablename; \
  va_list argptr; \
  va_start (argptr, constchartext); \
  vstringvariablename.FormatArgList(constchartext, argptr); \
  va_end (argptr);

/////////////////////////////////////////////////////////////////////////////////////
// ** trace **
/////////////////////////////////////////////////////////////////////////////////////

#if defined WIN32
  #if defined(HK_DEBUG)
    void VBASE_IMPEXP VTrace(const char *text, ...);
    #define VTRACE    VTrace
  #endif
#endif

#if !defined VTRACE
  // define a dummy function and a macro which takes the arguments behind VTRACE without compilerer errors
  // and hope the compiler optimizes this strange line (source ==> AFX.H ;))
  inline void VTrace(const char *text, ...) { }
  #define VTRACE   1 ? (void)0 : ::VTrace
#endif


#endif  //VISION_VSTRING_HPP

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
