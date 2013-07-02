/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   Tangent generator
///
/// Generates tangents for meshes in a scene.
class VGEOM2_IMPEXP_CLASS VGProcessor_TangentGenerator : public VGDynamicProcessor<VGProcessor_TangentGenerator>
{
public:
  VGProcessor_TangentGenerator();
  virtual ~VGProcessor_TangentGenerator();



  /// \brief
  ///   Processes a scene - generates tangents
  ///
  /// \param scene
  ///   Scene containing meshes to generate tangents for
  ///
  /// \return
  ///   False if generation failed
  virtual bool Process(VGScene& scene) const;



  /// \brief
  ///   Sets a flag indicating whether the UV coordinates of the model to be preprocessed are
  ///   between 0 and 1 (wrapped) or not - this influences the tangent creation
  ///
  /// \param i
  ///   0 = not wrapped, 1 = wrapped along U, 2 = wrapped along V, 3 = wrapped along U and V
  inline void         SetWrapUV(unsigned int i) throw()   { m_wrapUV = i; }

  /// \brief
  ///   Gets the currently set flag whether the UV coordinates of the model to be preprocessed
  ///   are between 0 and 1 (wrapped) or not
  ///
  /// \return
  ///   0 = not wrapped, 1 = wrapped along U, 2 = wrapped along V, 3 = wrapped along U and V
  inline unsigned int GetWrapUV() const throw()           { return m_wrapUV; }



private:
  unsigned int m_wrapUV;
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
