/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once
#ifndef V_LIGHT_CLIPPING_VOLUME_RENDERER_HPP
#define V_LIGHT_CLIPPING_VOLUME_RENDERER_HPP

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>

/// \brief render helper to render light clipping volumes
class VLightClippingVolumeRenderer
{
public:

  /// \brief constructor
  VLightClippingVolumeRenderer() {}

  /// \brief light clipping volume constants
  enum { 
    V_LIGHT_CLIPPING_STENCIL_BIT = 5 ///< which stencil bit should be used by the light clipping volume renderer
  };

  /// \brief Initializes the Light Clipping Volume Renderer
  void Init();

  /// \brief Deinitializes the Light Clipping Volume Renderer
  void DeInit();

  /// \brief
  ///   Renders the light clipping volume for the given light source.
  ///   Writes to the stencil buffer for that.
  ///   Use DepthStencil states patched with the PatchReadDepthStancilState function
  ///   to obtain the results.
  ///
  /// \param pLight
  ///   the light to render the clipping volume for
  bool RenderLightClippingVolume(VisLightSource_cl *pLight);

  /// \brief
  ///   Clears the data that has been written to the stencil buffer for the given light source
  ///   Needs to be called when the stencil data is no longer needed
  ///
  /// \param pLight
  ///   the light to clear the clipping volume for
  void ClearLightClippingVolume(VisLightSource_cl *pLight);

  /// \brief
  ///   Patches a depth stencil state so it can read light clipping volume results.
  ///
  /// \param inputState
  ///   The input depth-stencil state to be patched. 
  ///
  /// \param pOutputStates
  ///   Pointer to an array of the 2 depth-stencil states that will contain the patched results.
  ///   (1. depth-stencil state for VLightClippingVolumeComponent::VCH_Outside, 2. depth-stencil state for VLightClippingVolumeComponent::VCH_Inside)
  ///
  /// \param bEnableStencil
  ///   If the stencil test should be enabled on the state or not.
  static void PatchReadDepthStencilState(const VStateGroupDepthStencil& inputState, VStateGroupDepthStencil* pOutputStates, bool bEnableStencil = true);

private:

  //non-copyable
  VLightClippingVolumeRenderer(const VLightClippingVolumeRenderer& rh);
  void operator = (const VLightClippingVolumeRenderer& rh);

  bool RenderLightClippingVolumeHelper(VisLightSource_cl *pLight, VStateGroupDepthStencil& sg);

  VCompiledTechniquePtr m_spLightClippingVolumeStencilFill;
  VStateGroupDepthStencil m_LightClippingStencilStateWrite;
  VStateGroupDepthStencil m_LightClippingStencilStateClear;
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
