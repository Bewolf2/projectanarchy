/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vstrlist.hpp

// ---------------------------------------------------------------------------------
// File:        VStrList.hpp
// Description: Header file for the VStrList class
//              This class is handy for working with a list of strings.
//
// Notes!:      Searching is case insensitive!
//              For convenience adding of NULL or empty strings is allowed, but ignored
//
// Example usage : Building & storing a list of unique file names
// ---------------------------------------------------------------------------------

#ifndef DC_VBASE_VSTRLIST_HPP
#define DC_VBASE_VSTRLIST_HPP

#include <Vision/Runtime/Base/System/IO/Serialization/VArchive.hpp>

class VArchive;

/// \brief
///   This class represents a string list container
class VStrList
{
  V_DECLARE_SERIALX( VStrList, VBASE_IMPEXP_CLASS )

public:
  /// \brief
  ///   Constructor.
  inline VStrList() { }

  inline VStrList(const VStrList& src) { Copy(src); }

  /// \brief
  ///   Constructor which initializes the list to the passed size.
  inline VStrList(int iSize) : m_list(iSize)  {}

  // Assignment operator.
  inline VStrList& operator=(const VStrList& rhs)   { Copy(rhs); return *this; }

  /// \brief
  ///   Destructor.
  inline ~VStrList() { Reset(); }

  /// \brief
  ///   Copies the list. This method allocates memory.
  VBASE_IMPEXP void Copy(const VStrList& src);

  /// \brief
  ///   Resets the list. This method removes all the strings from the list and frees the memory.
  VBASE_IMPEXP void Reset();
  
  /// \brief
  ///   Adds string to the list. This method adds a copy of the input string to the list.
  VBASE_IMPEXP int AddString( const char* szString );

  /// \brief
  ///   Removes the specified element from the list. This method removes the string at the specified position from the list.
  VBASE_IMPEXP void Remove( int i );
  
  /// \brief
  ///   Returns the number of strings in the list.
  /// 
  /// \return
  ///   Number of strings in the list
  inline int GetLength() const { return m_list.GetLength(); } 

  /// \brief
  ///   Returns an element of the list.
  /// 
  /// This method returns the string at the specified position (index) in the list.
  /// 
  /// \param i
  ///   Index value. Has to be in the range 0..GetLength().
  /// 
  /// \return
  ///   const char *pszString: string at the specified position 
  inline const char* GetString( int i ) const { return (const char*) m_list[i]; } 

  /// \brief
  ///   Identical to GetString().
  inline const char* Get( int i )       const { return GetString(i); }
  
  /// \brief
  ///   Identical to GetString().
  inline const char* operator[]( int i ) { return GetString( i ); }

  /// \brief
  ///   Searches the specified string in the list and returns its index (or -1 if not found).
  VBASE_IMPEXP int FindString( const char* szString ) const;
  
  /// \brief
  ///   Adds a string to the list and returns its index. Unlike AddString,
  ///   this will only add the String if it is not contained in the list yet.
  VBASE_IMPEXP int AddUniqueString( const char* szString );

  /// \brief
  ///   Sets the string at the specified position. The position must be valid, and the
  ///   value to set must be neither NULL nor empty.
  VBASE_IMPEXP void SetString( int i, const char* szString );

  VBASE_IMPEXP void SerializeX( VArchive& ar );

protected:
  VPList m_list;
};


// Alternative: VString pointer list. For people who want real VStrings.
DECLARE_LIST(VStringList,VString,VBASE_IMPEXP)

#endif // DC_VBASE_VSTRLIST_HPP

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
