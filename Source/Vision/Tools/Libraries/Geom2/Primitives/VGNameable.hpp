/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once



/// \brief
///   Base class for nameable objects/entities
class VGEOM2_IMPEXP_CLASS VGNameable
{
public:
  VGNameable()                                                { }
  VGNameable(const VGNameable& other) : m_name(other.m_name)  { }
  VGNameable(const VString& name)     : m_name(name)          { }
  virtual ~VGNameable() = 0                                   { }



  /// \brief
  ///   Gets the object's name
  ///
  /// \return
  ///   Currently set name
  inline const VString& GetName() const throw()               { return m_name; }

  /// \brief
  ///   Sets the object's name
  ///
  /// \param s
  ///   Name to set
  inline void           SetName(const VString& s)             { m_name = s; }



  /// \brief
  ///   Assignment operator
  ///
  /// \param rhs
  ///   Nameable to assign
  ///
  /// \return
  ///   Reference to this nameable object
  inline VGNameable&    operator=(const VGNameable& rhs)      { m_name = rhs.m_name; return *this; }



protected:
  VString m_name;
};

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
