/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file RenderingOptimizationHelpers.hpp

#ifndef RENDERING_OPTIMIZATION_HELPERS_HPP_INCLUDED
#define RENDERING_OPTIMIZATION_HELPERS_HPP_INCLUDED

#include <Vision/Runtime/Engine/Renderer/RenderLoop/VisionRenderLoop.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>



/// \brief
///   Class with helper functionality for the rendering system.
///
/// This class contains various helper functions required by the rendering system. Some
/// functions are encapsulated in platform-specific .cpp files for reasons of confidentiality.
class RenderingOptimizationHelpers_cl
{
public:


  ///
  /// @name PLATFORM-SPECIFIC OPTIMISATION HELPERS
  /// @{
  ///

  /// \brief
	/// Sets the Shader Preference on Xbox360.
	/// 
	/// \param iNumPixelGPR
	/// Defines the number of GPRs dedicated to pixel processing. The remaining GPRs will be used for vertex
  /// processing.
	/// 
	/// \platforms
	/// Xbox360
	/// 
	EFFECTS_IMPEXP static void SetShaderPreference(int iNumPixelGPR);

  /// \brief
	/// Creates and patches a dummy texture for quick depth texture setup on PS3.
  ///
  /// This is a low-level trick to improve performance on PS3.
  /// On all other platforms it does nothing and just returns pRefTexture.
	/// 
	/// \param pRefTexture
	/// Reference texture to create the dummy texture from.
	/// 
	/// \returns
	/// Created dummy texture.
  ///
  /// \platforms
  /// PS3
	/// 
	EFFECTS_IMPEXP static VTextureObject *CreateAndPatchDummyTexture(VTextureObject *pRefTexture);

  /// \brief
  /// Configures HiStencil for early stencil rejection. Used e.g. for light rendering.
  ///
  /// \param bEnabled
  /// true to enable HiStencil, false to disable it.
  ///
  /// \param bForWriting
  /// true to enable HiStencil writing, false to enable HiStencil testing. Will have no effect if bEnabled is false.
  /// 
  /// \param iRef
  /// HiStencil reference value.
  /// 
  /// \platforms
  /// Xbox360
  /// 
  EFFECTS_IMPEXP static void SetHiStencilParameters(bool bEnabled, bool bForWriting, int iRef);

  /// \brief
  /// Converts a numeric anti-aliasing value to a human-readable string for use with shader inclusion tags.
  ///
  /// \param level
  /// MSAA level
  ///
  /// \returns
  /// Tag string
  ///
  /// \platforms
  /// DX11
  ///
  EFFECTS_IMPEXP static const VStaticString<7> &GetMSAAString(VVIDEO_Multisample level);

  /// \brief
  /// Converts a numeric anti-aliasing value to a shader permutation parameter string.
  EFFECTS_IMPEXP static const VStaticString<32> &GetMSAAString_Perm(VVIDEO_Multisample level);

  ///
  /// @}
  ///

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
