/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   Node class (abstract - basically a base class for nameable and transformable classes)
class VGEOM2_IMPEXP_CLASS VGNode : public VGNameable, public VGTransformable
{
public:
  VGNode() : VGNameable("")                       { }
  VGNode(const VString& name) : VGNameable(name)  { }
  VGNode(const VGNode& other)                     { operator=(other); }
  virtual ~VGNode() = 0                           { }



  /// \brief
  ///   Assignment operator
  ///
  /// \param rhs
  ///   Proxy to assign
  ///
  /// \return
  ///   Reference to this proxy
  inline VGNode& operator=(const VGNode& rhs)     { m_customProperties = rhs.m_customProperties; VGNameable::operator=(rhs); VGTransformable::operator=(rhs); return *this; }



  /// \brief
  ///   Gets the node's custom property string
  ///
  /// \return
  ///   Currently set custom property string
  inline const VString& GetCustomProperties() const throw()       { return m_customProperties; }

  /// \brief
  ///   Sets the node's custom property string
  ///
  /// \param s
  ///   Custom property string to set
  inline void           SetCustomProperties(const VString& s)     { m_customProperties = s; }



private:
  VString m_customProperties; // Custom property string.
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
