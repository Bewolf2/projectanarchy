/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VSTRINGBUFFER_HPP_INCLUDED
#define VSTRINGBUFFER_HPP_INCLUDED

#include <Vision/Tools/Libraries/ShaderLibExport/vImpExp.hpp>

/// \brief Local Helper class to speed up many string-concatenations.
// About 50 times faster than using VString.
class VSHADERLIBEXPORT_IMPEXP VStringBuffer
{
public:
  /// \brief Constructor: Initializes the string to "".
  VStringBuffer ();
  /// \brief Constructor: Copies the given c-string.
  VStringBuffer (const char* cc);
  /// \brief Constructor: Copies the given string.
  VStringBuffer (const VStringBuffer& cc);
  /// \brief Destructor: Frees all allocated data.
  ~VStringBuffer ();

  /// \brief Makes sure the string can hold up to uiSize characters. Speeds up subsequent string concatenations.
  void reserve (unsigned int uiSize);
  /// \brief Resizes the string to the given size.
  void resize (unsigned int uiSize);

  /// \brief Returns the string length. Cached value, not recomputed every time.
  unsigned int GetLength (void) const {return (m_uiLength);}
  /// \brief Returns whether the string length is zero.
  bool IsEmpty (void) const { return (m_uiLength == 0); }

  /// \brief Clears the string to be empty.
  void Reset (void) { m_uiLength = 0; if (m_uiCapacity > 0) m_pData[0] = '\0'; }

  /// \brief Appends the given string.
  void Append (const char* sz);
  /// \brief Appends the given string.
  void Append (const VStringBuffer& cc);
  /// \brief Appends the given character.
  void Append (char c);

  /// \brief Accesses the n-th character. (Read-only).
  const char& operator[] (unsigned int ix) const {return (m_pData[ix]);}
  /// \brief Accesses the n-th character.
  char& operator[] (unsigned int ix) {return (m_pData[ix]);}

  /// \brief Returns a read-only pointer to the string.
  const char* AsChar () const {return (m_pData);}

  /// \brief Assigns the given string to this object.
  void operator= (const char* rhs);
  /// \brief Assigns the given string to this object.
  void operator= (const VStringBuffer& rhs);
  /// \brief Appends the given string.
  void operator+= (const char* rhs) { Append (rhs); }
  /// \brief Appends the given character.
  void operator+= (char rhs) { Append (rhs); }
  /// \brief Appends the given string.
  void operator+= (const VStringBuffer& rhs) { Append (rhs); }

private:
  unsigned int m_uiCapacity;
  unsigned int m_uiLength;

  char* m_pData;
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
