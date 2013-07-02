/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VSimpleShaderProvider.hpp

#ifndef SIMPLESHADERPROVIDER_HPP
#define SIMPLESHADERPROVIDER_HPP


#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>


/// \brief
/// The class is used by the renderloop to manage the various lighting shaders.
///
/// It implements a specialized shader provider, that will choose reduced shaders for improved performance.
/// By default only surfaces with VSM_Auto set are affected. 
/// Choosing bOverrideManualShaders = true will extend this behavior to all surfaces.
/// 
/// Normally the VSimpleShaderProvider works by assigning vmDiffuse to all surfaces, 
/// but additionally it will search for techniques with the "SimpleShader" inclusion tag.
/// That way the user can write specific shaders that are only used on low-end hardware for instance.
///
/// If bUseSimpleShaders == false at creation the VSimpleShaderProvider will behave exactly as VisionShaderProvider_cl.

class VSimpleShaderProvider : public VisionShaderProvider_cl
{
public:
  EFFECTS_IMPEXP VSimpleShaderProvider(bool bOverrideManualShaders = false, bool bUseSimpleShaders = true);

  ///
  /// @name Overridden Functions
  /// @{
  ///

  EFFECTS_IMPEXP virtual void SetEffect(VisSurface_cl *pSurface, VCompiledEffect *pFX, VTechniqueConfig* pDefaultConfig = NULL) HKV_OVERRIDE;

  ///
  /// @}
  ///

protected:

  bool m_bOverrideManualShaders;
  bool m_bUseSimpleShaders;
 
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
