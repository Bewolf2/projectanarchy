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
class VGEOM2_IMPEXP_CLASS VGTransformable
{
public:
  VGTransformable()                                               { m_m.setIdentity(); }
  VGTransformable(const VGTransformable& other) : m_m(other.m_m)  { }
  VGTransformable(const hkvMat4& xform)         : m_m(xform)      { }
  virtual ~VGTransformable() = 0                                  { }



  /// \brief
  ///   Gets the object's transformation matrix
  ///
  /// \return
  ///   Currently set transformation matrix (parent space)
  inline       hkvMat4& GetTransformation()       throw()       { return m_m; }
  inline const hkvMat4& GetTransformation() const throw()       { return m_m; }

  /// \brief
  ///   Sets the object's transformation matrix
  ///
  /// \param m
  ///   Transformation matrix to set (parent space)
  inline void           SetTransformation(const hkvMat4& m)     { m_m = m; }



  /// \brief
  ///   Assignment operator
  ///
  /// \param rhs
  ///   Nameable to assign
  ///
  /// \return
  ///   Reference to this nameable object
  VGTransformable& operator=(const VGTransformable& rhs)        { m_m = rhs.m_m; return *this; }



protected:
  hkvMat4 m_m;  // Transformation matrix.
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
