/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VNameValueListParser.hpp
#ifndef V_NAME_VALUE_LIST_PARSER_HPP
#define V_NAME_VALUE_LIST_PARSER_HPP


/// \brief
///   Parser for lists of name/value pairs. The characters separating names from values and
///   name-value pairs from each other can be specified as template arguments. 
///
/// The parser can operate completely in-place, returning references to data within the 
/// original string, but modifying the original string during parsing. Since the returned
/// names and values are references into the original string, they remain valid after 
/// advancing the parser to the next item.
/// This mode will also be called "destructive mode".
/// 
/// Alternatively, it can operate semi-in-place, making a temporary copy of the current 
/// name/value pair into a buffer before modifying it. The buffer size can be specified as
/// a template parameter, and storage is allocated as a member of the object - i.e., no
/// separate allocation is made for the pair buffer. In this mode, the input string is never
/// modified, and the parser can be reset in order to re-start parsing from the beginning.
/// This mode will also be called "non-destructive mode".
///
/// Additionally, the parser can operate in strict and lenient mode. Choose the strict mode
/// when the emphasis should be on correct values being returned; choose the lenient mode
/// if the emphasis should be on parsing as much of the input string as possible.
///
/// \tparam ItemSeparator
///   the character that separates the name-value pairs from each other
/// \tparam NameValueSeparator
///   the character that separates names from values in a pair
/// \tparam PairBufferSize
///   size of the buffer for name-value pairs in non-destructive mode
template<char ItemSeparator, char NameValueSeparator, unsigned int PairBufferSize = 1024>
class VNameValueListParser
{
public:
  /// \brief
  ///   The different states this parser can assume
  enum Status
  {
    STATUS_READY, ///< Ready for parsing; call next() to parse the first item
    STATUS_ITEM_PARSED, ///< An item has been successfully parsed; name() and value() return valid data
    STATUS_END_OF_INPUT, ///< End of the input string has been reached
    STATUS_ERROR_QUOTES_IN_NAME,  ///< (strict mode only): quote character found in name part of a pair
    STATUS_ERROR_UNBALANCED_QUOTES, ///< (strict mode only): quotes were opened but never closed
    STATUS_ERROR_ITEM_TOO_LONG, ///< (only when both in strict and non-destructive mode): current item did not fit into the pair buffer
    STATUS_ERROR_EMPTY_KEY ///< (strict mode only): item contained only a value; the key was empty
  };

public:
  /// \brief
  ///   Initializes a new name-value list parser. This constructor always initializes the
  ///   parser in non-destructive mode, since the passed list is const.
  /// \param list
  ///   the list to parse.
  /// \param beStrict
  ///   whether to operate in strict mode (see the Status enumeration for errors that
  ///   are detected in strict mode only)
  VNameValueListParser(const char* list, bool beStrict);
  /// \brief
  ///   Initializes a new name-value list parser. This constructor allows to specify
  ///   whether the parser should be destructive or not.
  /// \param list
  ///   the list to parse.
  /// \param beStrict
  ///   whether to operate in strict mode (see the Status enumeration for errors that
  ///   are detected in strict mode only)
  /// \param beDestructive
  ///   whether to operate completely in-place (and thereby modify the input string) or
  ///   whether to be non-destructive and leave the input string unmodified
  VNameValueListParser(char* list, bool beStrict, bool beDestructive);
private:
  VNameValueListParser(const VNameValueListParser&);
  VNameValueListParser& operator=(const VNameValueListParser&);

public:
  /// \brief
  ///   Advances the parser to the next item. After construction, the parser is assumed
  ///   to be positioned before the first item, so next() must be called once to begin
  ///   parsing.
  /// \return
  ///   \c true if a valid item has been parsed, \c false otherwise. If true is returned, name()
  ///   and value() are guaranteed to return valid, non-null strings.
  bool next();

  /// \brief
  ///   Resets the parser. In non-destructive mode, the parser assumes the initial state it
  ///   had right after construction. In destructive mode, the parser is set to the
  ///   STATUS_END_OF_INPUT status.
  void reset();

  /// \brief
  ///   Returns the name of the currently parsed pair. 
  /// \return
  ///   the name of the currently parsed pair. If the parser's state is 
  ///   STATUS_ITEM_PARSED, this is guaranteed to be a valid string; in any other
  ///   status, the return value is undefined.
  const char* name() const { return m_currentName; }

  /// \brief
  ///   Returns the value of the currently parsed pair. 
  /// \return
  ///   the value of the currently parsed pair. If the parser's state is 
  ///   STATUS_ITEM_PARSED, this is guaranteed to be a valid string; in any other
  ///   status, the return value is undefined.
  const char* value() const { return m_currentValue; }

  /// \brief
  ///   Returns the current status of the parser.
  /// \return
  ///   the current status of the parser.
  Status status() const { return m_status; }

  /// \brief
  ///   Tries to interpret the current value as a bool.
  /// \param out_value
  ///   output parameter to hold the interpreted value
  /// \return
  ///   true if the parser has a valid current value, and if this value could
  ///   successfully interpreted as a bool; false otherwise.
  bool tryValueAsBool(bool& out_value) const;
  
  /// \brief
  ///   Interprets the current value as a bool and returns it. If interpreting
  ///   the current value fails (see tryValueAsBool()), the specified default
  ///   value is returned.
  /// \param defValue
  ///   the default value to return in case of failure
  /// \return
  ///   the current value interpreted as a bool, or the default value if that
  ///   failed
  bool valueAsBoolDef(bool defValue) const;

private:
  void error(Status status);
  void init();
  bool item(const char* startPos, const char* nvSepPos);
  char* trim(char* begin, char* end);

private:
  const char* m_listString;
  const bool m_beStrict;
  const bool m_beDestructive;

  char m_currentPair[PairBufferSize];
  const char* m_currentPos;
  char* m_currentName;
  char* m_currentValue;

  Status m_status;
};


template<char ItemSeparator, char NameValueSeparator, unsigned int PairBufferSize>
VNameValueListParser<ItemSeparator, NameValueSeparator, PairBufferSize>::VNameValueListParser(
  const char* list, bool beStrict)
: m_listString(list), m_beStrict(beStrict), m_beDestructive(false)
{
  {V_COMPILE_ASSERT(ItemSeparator != '\0');}
  {V_COMPILE_ASSERT(NameValueSeparator != '\0');}
  {V_COMPILE_ASSERT(ItemSeparator != NameValueSeparator);}
  {V_COMPILE_ASSERT(PairBufferSize >= 4);}

  init();
}


template<char ItemSeparator, char NameValueSeparator, unsigned int PairBufferSize>
VNameValueListParser<ItemSeparator, NameValueSeparator, PairBufferSize>::VNameValueListParser(
  char* list, bool beStrict, bool beDestructive)
: m_listString(list), m_beStrict(beStrict), m_beDestructive(beDestructive)
{
  init();
}


template<char ItemSeparator, char NameValueSeparator, unsigned int PairBufferSize>
bool VNameValueListParser<ItemSeparator, NameValueSeparator, PairBufferSize>::next()
{
  // Already ran through the list? -> return immediately.
  if (m_currentPos == NULL)
  {
    error(STATUS_END_OF_INPUT); // not an actual error, but handled the same way
    return false;
  }

  const char* startPos = m_currentPos;
  const char* nvSepPos = NULL;
  bool inQuotes = false;
  while (true)
  {
    switch (*m_currentPos)
    {
    case '\"':
      {
        if (m_beStrict && (nvSepPos == NULL))
        {
          // Quotes in the name part
          error(STATUS_ERROR_QUOTES_IN_NAME);
          return false;
        }
        inQuotes = !inQuotes;
        break;
      }
    case ItemSeparator:
      {
        if (!inQuotes)
        {
          if (item(startPos, nvSepPos))
          {
            ++m_currentPos;
            return true; // item() produced a name/value pair
          }
          else if (m_currentPos == NULL)
          {
            return false; // error in item()
          }
 
          startPos = m_currentPos + 1; // No error, but consecutive item separators; skip over
        }
        break;
      }
    case NameValueSeparator:
      {
        if (!inQuotes && (nvSepPos == NULL))
        {
          nvSepPos = m_currentPos;
        }
        break;
      }
    case '\0':
      {
        if (m_currentPos == startPos)
        {
          error(STATUS_END_OF_INPUT); // not an actual error, but handled the same way
          return false;
        }
        if (m_beStrict && inQuotes)
        {
          error(STATUS_ERROR_UNBALANCED_QUOTES);
          return false;
        }
        
        bool itemRes = item(startPos, nvSepPos);
        if (m_currentPos != NULL)
        {
          // If we get here, we have no error, but we may have an empty item.
          m_status = itemRes ? STATUS_ITEM_PARSED : STATUS_END_OF_INPUT;
          m_currentPos = NULL;
        }
        return itemRes;
      }
    }

    ++m_currentPos;
  }
}


template<char ItemSeparator, char NameValueSeparator, unsigned int PairBufferSize>
void VNameValueListParser<ItemSeparator, NameValueSeparator, PairBufferSize>::reset()
{
  if (m_beDestructive)
  {
    m_currentName = NULL;
    m_currentValue = NULL;
    m_currentPos = NULL;
    m_status = STATUS_END_OF_INPUT;
  }
  else
  {
    init();
  }
}


template<char ItemSeparator, char NameValueSeparator, unsigned int PairBufferSize>
bool VNameValueListParser<ItemSeparator, NameValueSeparator, PairBufferSize>::tryValueAsBool(bool& out_value) const
{
  static const char* boolTrueStrings[] =  {"true",  "1", "yes", "on"};
  static const char* boolFalseStrings[] = {"false", "0", "no",  "off"};
  static const int numStrings = 4;

  if (m_status != STATUS_ITEM_PARSED)
  {
    return false;
  }

  for (int i = 0; i < numStrings; ++i)
  {
    if (_stricmp(m_currentValue, boolTrueStrings[i]) == 0)
    {
      out_value = true;
      return true;
    }
    if (_stricmp(m_currentValue, boolFalseStrings[i]) == 0)
    {
      out_value = false;
      return true;
    }
  }

  return false;
}


template<char ItemSeparator, char NameValueSeparator, unsigned int PairBufferSize>
bool VNameValueListParser<ItemSeparator, NameValueSeparator, PairBufferSize>::valueAsBoolDef(bool defValue) const
{
  bool result = defValue;
  tryValueAsBool(result);
  return result;
}


template<char ItemSeparator, char NameValueSeparator, unsigned int PairBufferSize>
void VNameValueListParser<ItemSeparator, NameValueSeparator, PairBufferSize>::error(Status status)
{
  m_currentName = NULL;
  m_currentValue = NULL;
  m_currentPos = NULL;
  m_status = status;
}


template<char ItemSeparator, char NameValueSeparator, unsigned int PairBufferSize>
void VNameValueListParser<ItemSeparator, NameValueSeparator, PairBufferSize>::init()
{
  m_currentName = NULL;
  m_currentValue = NULL;
  m_currentPos = m_listString;
  m_status = (m_currentPos != NULL) ? STATUS_READY : STATUS_END_OF_INPUT;
}


template<char ItemSeparator, char NameValueSeparator, unsigned int PairBufferSize>
bool VNameValueListParser<ItemSeparator, NameValueSeparator, PairBufferSize>::item(const char* startPos, const char* nvSepPos)
{
  // If we can already see that an item is empty, skip it right away.
  if (startPos >= m_currentPos)
  {
    return false;
  }

  // In non-destructive mode, we need to copy the current pair. Check if we exceed the maximum 
  // item length. In strict mode, this is an error; otherwise, we try to handle it gracefully.
  size_t length = m_currentPos - startPos;
  if (!m_beDestructive && (length >= PairBufferSize))
  {
    if (m_beStrict)
    {
      error(STATUS_ERROR_ITEM_TOO_LONG);
      return false;
    }
    else
    {
      length = PairBufferSize - 1;
    }
  }

  // Copy (or just assign) the part of the list string that corresponds to our current item
  char* currentPair;
  if (m_beDestructive)
  {
    currentPair = const_cast<char*>(startPos);
  }
  else
  {
    memcpy(m_currentPair, startPos, length);
    currentPair = m_currentPair;
  }
  currentPair[length] = '\0';

  // Find out where (if at all) to split between name and value. If there is no value,
  // the separator is assumed to be at the position of the item string terminator.
  size_t nvSepOffset = (nvSepPos != NULL) ? (nvSepPos - startPos) : length;
  if (nvSepOffset > length)
  {
    nvSepOffset = length;
  }

  // Assign the name/value pointers
  m_currentName = currentPair;
  m_currentValue = m_currentName + nvSepOffset;
  if (*m_currentValue != '\0')
  {
    *m_currentValue = '\0';
    ++m_currentValue;
  }

  m_currentName = trim(m_currentName, m_currentName + (nvSepOffset - 1));
  m_currentValue = trim(m_currentValue, m_currentName + (length - 1));

  // In strict mode, keyless values are not allowed
  if (m_beStrict && (*m_currentName == '\0') && (*m_currentValue != '\0'))
  {
    error(STATUS_ERROR_EMPTY_KEY);
    return false;
  }

  if ((*m_currentName == '\0') && (*m_currentValue == '\0'))
  {
    return false;
  }
  else
  {
    m_status = STATUS_ITEM_PARSED;
    return true;
  }
}


template<char ItemSeparator, char NameValueSeparator, unsigned int PairBufferSize>
char* VNameValueListParser<ItemSeparator, NameValueSeparator, PairBufferSize>::trim(char* begin, char* end)
{
  static const char* trimChars = " \t\r\n";
  
  // Trim whitespace on both sides
  while ((begin <= end) && (strchr(trimChars, *end) != NULL))
  {
    *end = '\0';
    --end;
  }
  while ((begin <= end) && (strchr(trimChars, *begin) != NULL))
  {
    ++begin;
  }

  // Remove leading and trailing quotes, if both are present and distinct
  if ((begin < end) && (*begin == '\"') && (*end == '\"'))
  {
    ++begin;
    *end = '\0';
  }

  return begin;
}


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
