/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


// @@@
//     The engine supports _one_ lightgrid box and _multiple_ detail boxes - we could use a class recursively
//     for flexibility, backends for the vision prefab format will have to convert it, then.

/// \brief
///   Light grid class (basically storing a box and precision/subdivision factors)
class VGEOM2_IMPEXP_CLASS VGLightGrid : public VGNameable/*@@@ replace with VGNode?*/
{
public:
  VGLightGrid();
  VGLightGrid(const VGLightGrid& other);
  virtual ~VGLightGrid();



  /// \brief
  ///   Gets the light grid's shape
  ///
  /// \return
  ///   Light grid shape
  inline const hkvAlignedBBox&  GetShape() const                                                        { return m_shape; }

  /// \brief
  ///   Sets the light grid's shape
  ///
  /// \param shape
  ///   Light grid shape
  inline void           SetShape(const hkvAlignedBBox& shape)                                           { m_shape = shape; }



  /// \brief
  ///   Gets the light grid's subdivision factors
  ///
  /// \param axis
  ///   Axis to get subdivision factor for (0 = x, 1 = y, 2 = z)
  ///
  /// \return
  ///   Subdivision factor
  inline unsigned int   GetSubdivisionFactor(unsigned int axis) const                                   { VASSERT(axis < 3); return m_subdivisionFactors[axis]; }

  /// \brief
  ///   Sets the light grid's subdivision factors
  ///
  /// \param x
  ///   Subdivision along x
  ///
  /// \param y
  ///   Subdivision along y
  ///
  /// \param z
  ///   Subdivision along z
  inline void           SetSubdivisionFactors(unsigned int x, unsigned int y, unsigned int z) throw()   { m_subdivisionFactors[0] = x; m_subdivisionFactors[1] = y; m_subdivisionFactors[2] = z; }



  /// \brief
  ///   Gets whether this light grid shape is a detail shape
  ///
  /// \return
  ///   Detail shape indicator
  inline bool           GetIsDetailShape() const throw()                                                { return m_isDetailShape; }

  /// \brief
  ///   Sets whether this light grid shape should be used as a detail shape
  ///
  /// \param b
  ///   Detail shape indicator
  inline void           SetIsDetailShape(bool b) throw()                                                { m_isDetailShape = b; }



  /// \brief
  ///   Gets this light grid's user data string
  ///
  /// \return
  ///   Light grid's user data string
  inline const VString& GetUserDataString() const throw()                                               { return m_userDataString; }

  /// \brief
  ///   Sets this light grid's user data string
  ///
  /// \param userData
  ///   User data string to assign
  inline void           SetUserDataString(const VString& userData)                                      { m_userDataString = userData; }



  /// \brief
  ///   Assignment operator
  ///
  /// \param rhs
  ///   Light grid to assign
  ///
  /// \return
  ///   Reference to this light grid
  VGLightGrid&          operator=(const VGLightGrid& rhs);



private:
  hkvAlignedBBox m_shape;
  bool           m_isDetailShape;
  VString        m_userDataString;
  unsigned int   m_subdivisionFactors[3];
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
