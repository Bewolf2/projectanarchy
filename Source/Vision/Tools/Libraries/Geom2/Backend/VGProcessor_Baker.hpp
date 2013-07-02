/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   Scene baker
///
/// Bakes the scene depending on some parameters (see BakeParams enum for details)
class VGEOM2_IMPEXP_CLASS VGProcessor_Baker : public VGDynamicProcessor<VGProcessor_Baker>
{
public:
  VGProcessor_Baker();
  virtual ~VGProcessor_Baker();


  enum BakeFlags
  {
    VGBF_NOTHING    = 0,
    VGBF_INSTANCES  = V_BIT(0), // Turns instances in scene into copies (see VGScene::BakeInstances for details)
    VGBF_TRANSFORMS = V_BIT(1), // Applies mesh transformation to scene meshes (see VGMesh::BakeTransformation for details)
    VGBF_UV_MAPPING = V_BIT(2)  // Applies material's mapping scale/rotation/offset to UV coordinates (applies to first UV channel, only - uses mapping algorithm specified in material) @@@ test
  };


  /// \brief
  ///   Processes a scene - bakes the scene depending on some flags
  ///
  /// \param scene
  ///   Scene containing data to bake
  ///
  /// \return
  ///   False if baking failed
  virtual bool      Process(VGScene& scene) const;



  /// \brief
  ///   Sets the desired bake flags
  ///
  /// \param e
  ///   Bake flags
  inline void       SetFlags(BakeFlags e) throw()       { m_flags = e; }

  /// \brief
  ///   Gets the currently set bake flags
  ///
  /// \return
  ///   Currently set bake flags
  inline BakeFlags  GetFlags() const throw()            { return m_flags; }



private:
  BakeFlags m_flags;
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
