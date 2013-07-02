/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VFakeSpecularGenerator.hpp

#ifndef VFAKE_SPECULAR_GENERATOR_HPP_INCLUDED
#define VFAKE_SPECULAR_GENERATOR_HPP_INCLUDED

/// \brief
///  A Render loop that renders the specular component of each light into a cubemap face.
///
/// The resulting cubemap can be used for the FakeSpecular shader.
class VFakeSpecularGenerator : public IVisRenderLoop_cl
{
public:
  /// \brief Constructor.
  EFFECTS_IMPEXP VFakeSpecularGenerator();

  /// \brief Destructor.
  EFFECTS_IMPEXP virtual ~VFakeSpecularGenerator();

  /// \brief Overriden method. See base class.
  EFFECTS_IMPEXP virtual void OnDoRenderLoop(void *pUserData) HKV_OVERRIDE;

  /// \brief Sets the specular power to use for rendering the light sources.
  inline EFFECTS_IMPEXP void SetSpecularPower(float fSpecularPower) 
  {
    m_fSpecularPower = fSpecularPower;
  }

#if !defined(_VISION_DOC)
  V_DECLARE_DYNAMIC_DLLEXP(VFakeSpecularGenerator, EFFECTS_IMPEXP);
#endif

private:
  VisMeshBufferPtr m_spMeshBuffer;
  float m_fSpecularPower;
  VCompiledShaderPassPtr m_spShader;
};

#endif // VFAKE_SPECULAR_GENERATOR_HPP_INCLUDED

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
