/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VArgList.hpp

#ifndef _VISION_VARGLIST_HPP
#define _VISION_VARGLIST_HPP


/// \brief
///   Manages a list of arguments
/// 
/// The VArgList class takes one or more strings as an argument string. The argument string is
/// tokenized; i.e., it is split into arguments separated by spaces.
/// 
/// The class provides functions to get the count of the arguments, the type and the value. 
/// 
/// The list of arguments is an ONE index based array.
/// 
/// Text between quotes is not split up into arguments.
/// 
/// Flags are special arguments. Flags starts with a -. You can query the position of a flag in the
/// argumentlist with VArgList::GetFlagIndex(). Pass a short and a long name for the flag to
/// VArgList::GetFlagIndex and you get back the argument item index or zero if the flag is not
/// found. The passes strings must not contain the -.
/// 
/// \note
///   You can NOT place quotes in a argument like in C++!
/// 
/// \note
///   The list of arguments is ONE index based!
class VArgList
{
public:
  /// \brief
  ///   Constructor
  VBASE_IMPEXP VArgList();
  /// \brief
  ///   Destructor
  VBASE_IMPEXP ~VArgList();

  /// \brief
  ///   Parses the passed string into the many arguments.
  /// 
  /// \param argumentstring
  ///   String to parse
  ///
  /// \note
  ///   Recognition of inner quotes is limited to one level; thus:
  ///   \verbatim shell "copy \"c:\My Documents\text.txt\" \"d:\Program Files\Test.txt\"" \endverbatim
  ///   is parsed into the following two arguments:
  ///   -# \verbatim shell \endverbatim
  ///   -# \verbatim copy "c:\My Documents\text.txt\" \"d:\Program Files\Test.txt" \endverbatim
  VBASE_IMPEXP VBool AddArgumentString(const char *argumentstring);

  /// \brief
  ///   Deletes all arguments
  VBASE_IMPEXP void ClearArgs();
  /// \brief
  ///   Return the number of arguments
  VBASE_IMPEXP int GetArgCount() const;
  /// \brief
  ///   Lookup flag name and return its index
  VBASE_IMPEXP int GetFlagIndex(const char *shortflagname, const char *longflagname = NULL) const;

  /// \brief
  ///   Indicates whether argument at specified index is a flag
  VBASE_IMPEXP VBool IsFlag(int index) const;
  /// \brief
  ///   Indicates whether argument at specified index is a string
  VBASE_IMPEXP VBool IsString(int index) const;
  /// \brief
  ///   Indicates whether argument at specified index is a bool
  VBASE_IMPEXP VBool IsBool(int index) const;
  /// \brief
  ///   Indicates whether argument at specified index is an integer
  VBASE_IMPEXP VBool IsInt(int index) const;
  /// \brief
  ///   Indicates whether argument at specified index is a float
  VBASE_IMPEXP VBool IsFloat(int index) const;
  /// \brief
  ///   Indicates whether argument at specified index is a double
  VBASE_IMPEXP VBool IsDouble(int index) const;
  /// \brief
  ///   Indicates whether argument at specified index is a 3D vector
  VBASE_IMPEXP VBool IsVertex3f(int index) const;
  /// \brief
  ///   Indicates whether argument at specified index is a matrix
  VBASE_IMPEXP VBool IsMatrix4(int index) const;
  
  
  /// \brief
  ///   Tries to convert the argument at specified index to string
  VBASE_IMPEXP const char* AsString(int index) const;
  /// \brief
  ///   Tries to convert the argument at specified index to string
  VBASE_IMPEXP VString AsRawString(int index) const;
  /// \brief
  ///   Tries to convert the argument at specified index to string. Puts quotes around the string
  VBASE_IMPEXP VString AsQuotedString(int index) const;
  /// \brief
  ///   Tries to convert the argument at specified index to bool
  VBASE_IMPEXP VBool AsBool(int index) const;
  /// \brief
  ///   Tries to convert the argument at specified index to int
  VBASE_IMPEXP int AsInt(int index) const;
  /// \brief
  ///   Tries to convert the argument at specified index to float
  VBASE_IMPEXP float AsFloat(int index) const;
  /// \brief
  ///   Tries to convert the argument at specified index to double
  VBASE_IMPEXP double AsDouble(int index) const;
  /// \brief
  ///   Tries to convert the argument at specified index to 3D vector
  VBASE_IMPEXP hkvVec3 AsVertex3f(int index) const;

  /// \brief
  ///   Get the full argument string. Strings are put into quotes
  VBASE_IMPEXP VBool GetArgumentString(VString &sArgumentString) const;
  /// \brief
  ///   Get the full argument string
  VBASE_IMPEXP VBool GetRawArgumentString(VString &sArgumentString) const;
  /// \brief
  ///   Sets the item at specified index
  VBASE_IMPEXP VBool SetAsString(int index, const char *newValue);

  /// \brief
  ///   Copy operator
  VBASE_IMPEXP VArgList &operator= (const VArgList &al);

  /// \brief
  ///   Changes occurrences of double quotes (\") to balanced pairs of opening and
  ///   closing braces ({ and }).
  ///
  /// If the number of quotes in the string is not even, the function fails and the
  /// string is not changed.
  ///
  /// \param str
  ///   a reference to the string in which to change the quote characters
  /// \return
  ///   \c TRUE if the function succeeded; \c FALSE if the function failed.
  VBASE_IMPEXP static VBool ReduceInnerQuotes(VString &str);

  /// \brief
  ///   Changes occurrences of balanced pairs of opening and closing braces ({ and }) 
  ///   to double quotes (\").
  ///
  /// If the braces in the string are not properly balanced, the function fails and the
  /// string is not changed.
  ///
  /// \param str
  ///   a reference to the string in which to change the quote characters
  /// \return
  ///   \c TRUE if the function succeeded; \c FALSE if the function failed.
  VBASE_IMPEXP static VBool ExpandInnerQuotes(VString &str);
  

private:
  
  /// \brief
  ///   Returns true if the char is " or ' and not \ is in front it it
  /// 
  /// \param pChar
  /// 
  /// \return
  ///   \c true if the char is a non-escaped quote; false otherwise
  VBASE_IMPEXP static VBool IsCharQuote(const char* pChar);

  /// \brief
  ///   Returns true if the char is \" or \'
  /// 
  /// \param pChar
  /// 
  /// \return
  ///   \c true if the char is an escaped quote; false otherwise
  VBASE_IMPEXP static VBool IsCharInnerQuote(const char* pChar);

  VString m_szArgs; ///< internal
  VPList m_pArgs;   ///< internal
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
