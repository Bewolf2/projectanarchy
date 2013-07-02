/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   Scene bone filter
///
/// Filters bones according to some user-settable knobs
class VGEOM2_IMPEXP_CLASS VGProcessor_BoneFilter : public VGDynamicProcessor<VGProcessor_BoneFilter>//@@@ test
{
public:
  VGProcessor_BoneFilter();
  virtual ~VGProcessor_BoneFilter();


  enum FilterMode
  {
    VBFM_FILTER_UNUSED_LEAF_BONES
  };


  /// \brief
  ///   Processes a scene - bakes the scene depending on some flags
  ///
  /// \param scene
  ///   Scene containing data to bake
  ///
  /// \return
  ///   False if baking failed
  virtual bool            Process(VGScene& scene) const;//@@@ test



  /// \brief
  ///   Sets bone mask to specify which bones to work on
  ///
  /// \param m
  ///   Mask
  inline void             SetMask(VGBone::BoneMask m) throw()   { m_mask = m; }//@@@ test

  /// \brief
  ///   Gets the currently set bake flags
  ///
  /// \return
  ///   Currently set bake flags
  inline VGBone::BoneMask GetMask() const throw()               { return m_mask; }//@@@ test



  /// \brief
  ///   Sets the desired mode
  ///
  /// \param m
  ///   Bone filter mode
  inline void             SetMode(FilterMode m) throw()         { m_mode = m; }//@@@ test

  /// \brief
  ///   Gets the currently set filter mode
  ///
  /// \return
  ///   Currently set bone filter mode
  inline FilterMode       GetMode() const throw()               { return m_mode; }//@@@ test



private:
  VGBone::BoneMask m_mask;
  FilterMode       m_mode;
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
