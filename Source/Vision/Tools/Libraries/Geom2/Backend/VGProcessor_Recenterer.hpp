/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   Scene recenter process
///
/// Applies a transformation to all scene objects depending on given options - useful
/// for recentering the scene down the stack, after removal of some nodes, etc..
class VGEOM2_IMPEXP_CLASS VGProcessor_Recenterer : public VGDynamicProcessor<VGProcessor_Recenterer>//@@@ test
{
public:
  enum RecenteringMode
  {
    VGRF_NULL,         // Do nothing.
    VGRF_CUSTOM,       // Use custom user transformation.
    VGRF_BBOX_CENTER,  // Recenter the scene around global bounding box' center.
    VGRF_BBOX_FLOOR_Y, // Recenter the scene around global bounding box' floor center (assuming y-up).
    VGRF_BBOX_FLOOR_Z // Recenter the scene around global bounding box' floor center (assuming z-up).
  };


public:
  VGProcessor_Recenterer();
  virtual ~VGProcessor_Recenterer();


  /// \brief
  ///   Processes a scene - applies a transformation to all scene objects
  ///
  /// \param scene
  ///   Scene containing data to bake
  ///
  /// \return
  ///   False if transformation failed
  virtual bool              Process(VGScene& scene) const;



  /// \brief
  ///   Sets the desired custom transformation matrix
  ///
  /// \param m
  ///   Transformation matrix
  inline void               SetCustomTransformation(const hkvMat4& m)       { m_m = m; }

  /// \brief
  ///   Gets the currently set custom transformation matrix
  ///
  /// \return
  ///   Currently set transformation matrix
  inline const hkvMat4&   GetCustomTransformation() const throw()           { return m_m; }



  /// \brief
  ///   Sets the desired transformation mode for animations
  ///
  /// \param mode
  ///   Transformation mode for animation keys
  inline void               SetRecenteringMode(RecenteringMode mode) throw()  { m_mode = mode; }

  /// \brief
  ///   Gets the currently set transformation mode for animations
  ///
  /// \return
  ///   Currently set transformation mode for animation keys
  inline RecenteringMode    GetRecenteringMode() const throw()                { return m_mode; }


private:
  hkvMat4       m_m;
  RecenteringMode m_mode;
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
