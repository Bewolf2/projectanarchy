/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvBase.hpp

#ifndef HKVBASE_HPP_INCLUDED
#define HKVBASE_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/AssetFramework.hpp>

#include <Common/Base/Types/hkBaseTypes.h>
#include <Common/Base/Container/String/hkStringBuf.h>
#include <Common/Base/System/Io/FileSystem/hkFileSystem.h>


template<typename T>
struct hkvStringSafeLessI
{
  bool operator()(const T& a, const T& b) const
  {
    return (hkvStringHelper::safeCompare(a, b, true) < 0);
  }
};


class hkvStringHelper
{
public:
  /// \brief Encodes a 64-bit number in 13 characters with modified base32 encoding
  ASSETFRAMEWORK_IMPEXP static void encodeNumberBase32Mod(hkUint64 number, hkStringBuf& out_put);

  /// \brief Converts sBuffer to lower case UTF8 CORRECT!
  ASSETFRAMEWORK_IMPEXP static void toLowerCase(hkStringBuf& sBuffer);

  /// \brief
  ///   Checks, whether a string starts with a specific string (case insensitive)
  ASSETFRAMEWORK_IMPEXP static hkBool startsWithI(const char* string, const char* start);

  /// \brief
  ///   This string comparison method gracefully handles all combinations of passing NULL. A NULL string equals an empty string. Also works UTF8 correct in lower case comparisons (although that is very inefficient).
  ASSETFRAMEWORK_IMPEXP static hkInt32 safeCompare(const char* a, const char* b, bool ignoreCase=true);

  ASSETFRAMEWORK_IMPEXP static void trimString(hkStringBuf& out_string);

  ASSETFRAMEWORK_IMPEXP static void setExtension(hkStringBuf& out_fileName, const char* extension);

  /// \brief
  ///   Returns whether the string is empty, i.e. it is NULL or its first character is zero.
  ///   
  /// \param szString
  ///   String to be checked.
  /// 
  /// \return
  ///    True if the string is empty, false otherwise.
  inline static hkBool isEmpty(const char* string)
  {
    return string == NULL ? true : string[0] == '\0';
  }

  /// \brief Returns the index of the last '.' at the end of a path (after all / or \). Returns -1 if there is no such position.
  ASSETFRAMEWORK_IMPEXP static hkInt32 getExtensionPos(const char* filename);

  /// \brief Returns a pointer to the extension (including the '.') in filename. "" if there is no extension.
  ASSETFRAMEWORK_IMPEXP static const char* getExtension(const char* filename);

  /// \brief Checks whether the given file has the desired extension. szExtension must start with a '.'
  ASSETFRAMEWORK_IMPEXP static bool hasExtension(const char* szFile, const char* szExtension);

  /// \brief Copies the filename without the file extension to 'res'.
  ASSETFRAMEWORK_IMPEXP static hkBool getFilenameNoExt(char* res, const char* filename);

  /// \brief Returns a pointer to the substring that represents the file name including its extension.
  ASSETFRAMEWORK_IMPEXP static const char* getFilename(const char* pathname);

  /// \brief Canonicalizes the specified path by adjusting all backslashes to forward slashes, resolving
  ///   parent and current references (.. and .), and by converting the path to lower case on platforms
  ///   that are case-insensitive.
  ASSETFRAMEWORK_IMPEXP static const char* makeCanonicalPath(const char* path, hkStringBuf& out_result);

  /// \brief Replaces all \ by / and removes all '..' by modifying the path accordingly.
  ASSETFRAMEWORK_IMPEXP static hkStringBuf makeCleanPath(const char* szPath);

  /// \brief Modifies the given string to remove the file extension (if there is any). Thus returns the entire file path and file name, just without the file extension.
  ASSETFRAMEWORK_IMPEXP static void pathNoExtension(hkStringBuf& inout_buf);
};


class hkvTimeHelper
{
public:
  ASSETFRAMEWORK_IMPEXP static hkFileSystem::TimeStamp getCurrentTime();
  ASSETFRAMEWORK_IMPEXP static float getSecondsSince(const hkFileSystem::TimeStamp& base);
};


class hkvFileHelper
{
public:
  ASSETFRAMEWORK_IMPEXP static hkBool fileExists(const char* filePath, hkFileSystem* fs = NULL);
  ASSETFRAMEWORK_IMPEXP static hkBool dirExists(const char* filePath, hkFileSystem* fs = NULL);

  ASSETFRAMEWORK_IMPEXP static hkResult createDirectoryRecursive(const char* directory);
  ASSETFRAMEWORK_IMPEXP static hkResult getMainModuleBasePath(hkStringBuf& out_path);
  ASSETFRAMEWORK_IMPEXP static hkResult getSystemTempPath(hkStringBuf& out_path);
};

namespace hkvSystemHelper
{
  ASSETFRAMEWORK_IMPEXP const char* formatErrorMessage(hkUint32 errorCode, hkStringBuf& out_buffer);
  ASSETFRAMEWORK_IMPEXP hkResult getEnvironmentVariable(const char* name, hkStringBuf& out_value);
  ASSETFRAMEWORK_IMPEXP const char* makeUserIdString(hkStringBuf& out_buffer);
}


namespace hkvMbcs
{
  /// Convert from wide to MBCS string according to the current code page. src may be null terminated 
  /// and/or a limit on the number of source characters may be supplied.
  /// At most dstSize bytes are written into dst and if dstSize is nonzero, dst is always null terminated.
  /// The buffer size required to hold the null terminated MBCS string is returned in any case.
  ASSETFRAMEWORK_IMPEXP int mbcsFromWide(char* dst, int dstSize, const wchar_t* src, int srcCount=-1);

  /// Convert from MBCS to wide string according to the current code page. src may be null terminated 
  /// and/or a limit on the number of source characters may be supplied.
  /// At most dstSize characters are written into dst and if dstSize is nonzero, dst is always null terminated.
  /// The buffer size (in wide chars) required to hold the null terminated wide string is returned in any case.
  ASSETFRAMEWORK_IMPEXP int wideFromMbcs(wchar_t* dst, int dstSize, const char* src, int srcCount=-1);

  /// Helper to hold a temporary "wide" encoded version of a utf8 string.
  class MbcsFromWide
  {
  public:
    /// Create a converter.
    ASSETFRAMEWORK_IMPEXP MbcsFromWide(const wchar_t* wide);

    /// Access the null or null terminated wide string.
    const char* cString() { return m_mbcs.begin(); }

    /// Internal access. The array size is the number of non-null characters. It is the users responsibility
    /// to ensure that the element past the end is null. e.g. by calling pushBack(0) then popBack();
    hkArray<char>::Temp& getArray() { return m_mbcs; }
  private:
    // Invariant: m_wide.getSize() is the number of codepoints
    // If not null, there is a null "past-the-end".
    hkArray<char>::Temp m_mbcs;
  };

  /// Helper to hold a temporary "wide" encoded version of an MBCS string.
  class WideFromMbcs
  {
  public:
    /// Create a converter.
    ASSETFRAMEWORK_IMPEXP WideFromMbcs(const char* mbcs);

    /// Access the null or null terminated wide string.
    const wchar_t* cString() { return m_wide.begin(); }

    /// Internal access. The array size is the number of non-null characters. It is the users responsibility
    /// to ensure that the element past the end is null. e.g. by calling pushBack(0) then popBack();
    hkArray<wchar_t>::Temp& getArray() { return m_wide; }
  private:
    // Invariant: m_wide.getSize() is the number of codepoints
    // If not null, there is a null "past-the-end".
    hkArray<wchar_t>::Temp m_wide;
  };
}

class hkvEnumInstance;

class hkvEnumDefinition
{
public:
  ASSETFRAMEWORK_IMPEXP hkvEnumDefinition();
  ASSETFRAMEWORK_IMPEXP hkvEnumDefinition(hkUint32 numElements, const char** elementStrings);
  ASSETFRAMEWORK_IMPEXP hkvEnumDefinition(const hkvEnumDefinition& rhs);
  ASSETFRAMEWORK_IMPEXP hkvEnumDefinition& operator=(const hkvEnumDefinition& rhs);

  ASSETFRAMEWORK_IMPEXP ~hkvEnumDefinition();

  

public:
  ASSETFRAMEWORK_IMPEXP hkUint32 getNumElements() const;
  ASSETFRAMEWORK_IMPEXP hkResult stringToId(const char* name, hkUint32& out_id) const;
  ASSETFRAMEWORK_IMPEXP hkResult idToString(hkUint32 id, const char*& out_name) const;


private:
  // needed so that hkvEnumInstance can use the EmptyDefinition instance
  friend class hkvEnumInstance;
  static hkvEnumDefinition s_emptyDefinition;

  hkUint32 m_nameCount;
  const char** m_strings;
};

class hkvEnumInstance
{
public:
  ASSETFRAMEWORK_IMPEXP hkvEnumInstance();
  ASSETFRAMEWORK_IMPEXP hkvEnumInstance(const hkvEnumDefinition& definition);
  ASSETFRAMEWORK_IMPEXP hkvEnumInstance(const hkvEnumDefinition& definition, 
    hkUint32 numAllowedElements, const hkUint32* allowedElements);
  ASSETFRAMEWORK_IMPEXP hkvEnumInstance(const hkvEnumInstance& rhs);
  ASSETFRAMEWORK_IMPEXP hkvEnumInstance& operator=(const hkvEnumInstance& rhs);

public:
  ASSETFRAMEWORK_IMPEXP const hkvEnumDefinition& getDefinition() const;
  ASSETFRAMEWORK_IMPEXP const char* getAvailableElementStrings() const;

  ASSETFRAMEWORK_IMPEXP hkUint32 getDefinitionId() const;
  ASSETFRAMEWORK_IMPEXP hkUint32 getAvailableElementsId() const;
  ASSETFRAMEWORK_IMPEXP const char* getString() const;
  ASSETFRAMEWORK_IMPEXP const char* getStringForAvailableElementsId(hkUint32 id) const;

  ASSETFRAMEWORK_IMPEXP hkResult setByDefinitionId(hkUint32 id);
  ASSETFRAMEWORK_IMPEXP hkResult setByAvailableElementsId(hkUint32 id);
  ASSETFRAMEWORK_IMPEXP hkResult setByString(const char* name);

private:
  void makeAvailableElementsString();

  const hkvEnumDefinition* m_definition;
  hkArray<hkUint32> m_availableElements;
  hkStringPtr m_availableElementsString;
  hkUint32 m_value;
};


/// \class hkvEnumMapping
/// \brief Implements a mapping (cartesian product) between the elements of two
///        enums. For each possible mapping, the boolean state can be set and queried.
template<typename EnumA, hkUlong NumA, typename EnumB, hkUlong NumB>
class hkvEnumMapping
{
protected:
  hkvEnumMapping()
  {
    for (hkUlong i = 0; i < NumA * NumB; ++i)
    {
      m_mappings[i] = false;
    }
  }

private:
  hkvEnumMapping(const hkvEnumMapping&);
  hkvEnumMapping& operator=(const hkvEnumMapping&);

protected:
  void addMapping(EnumA a, EnumB b)
  {
    if (a >= NumA || b > NumB)
    {
      return;
    }
    m_mappings[(a * NumB) + b] = true;
  }

  template<hkUlong size>
  void addMappings(EnumA a, EnumB(&b)[size])
  {
    if (a >= NumA)
    {
      return;
    }
    for (hkUlong bIdx = 0; bIdx < size; ++bIdx)
    {
      if (b[bIdx] < NumB)
      {
        m_mappings[(a * NumB) + b[bIdx]] = true;
      }
    }
  }

public:
  bool isMapped(EnumA a, EnumB b) const
  {
    if (a >= NumA || b > NumB)
    {
      return false;
    }
    return m_mappings[(a * NumB) + b];
  }

private:
  bool m_mappings[NumA * NumB];
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
