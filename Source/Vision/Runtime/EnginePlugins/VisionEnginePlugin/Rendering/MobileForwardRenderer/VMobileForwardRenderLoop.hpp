/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VMobileForwardRenderLoop.hpp

#ifndef VMOBILE_FORWARDRENDERLOOP_HPP_INCLUDED
#define VMOBILE_FORWARDRENDERLOOP_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/MobileForwardRenderer/VMobileForwardRenderer.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/VLightClippingVolumeRenderer.hpp>

class IVShadowMapComponent;

/// \brief
///   Forward render loop for mobile devices (Android, iOS).
class VMobileForwardRenderLoop : public VisionRenderLoop_cl
{
public:

  /// \brief
  /// Constructor of the mobile forward shading render loop.
  /// 
  /// \param pRenderer
  ///   The renderer node associated with the loop
  EFFECTS_IMPEXP VMobileForwardRenderLoop(VMobileForwardRenderingSystem *pRenderer);

  /// \brief
  ///   Overriden InitRenderLoop
  EFFECTS_IMPEXP virtual void InitRenderLoop() HKV_OVERRIDE;

  /// \brief
  ///   Overriden DeInitRenderLoop
  EFFECTS_IMPEXP virtual void DeInitRenderLoop() HKV_OVERRIDE;

  /// \brief
  /// Overridden OnDoRenderLoop function.
  /// 
  /// \param pUserData
  ///   User data, typically the render context.
  /// 
  EFFECTS_IMPEXP virtual void OnDoRenderLoop(void *pUserData) HKV_OVERRIDE;

  /// \brief
  ///  Overridden DrawDynamicLight function.
  /// 
  EFFECTS_IMPEXP virtual void DrawDynamicLight() HKV_OVERRIDE;

  /// \brief
  ///  Renders dynamically lit geometry.
  EFFECTS_IMPEXP void RenderLitGeometry(VisLightSource_cl *pLight, IVShadowMapComponent *pShadowMapComponent, bool bBasePass, bool bUsesLightClippingVolume, bool bEntities, bool bStaticGeometry);

  /// \brief
  /// Prepares a lighting pass, i.e. renders light clipping volume and shadow map. 
  EFFECTS_IMPEXP IVShadowMapComponent *PrepareLightingPass(VisLightSource_cl *pLight, bool bBasePass, bool &bUsesLightClippingVolume);

  /// \brief
  /// Finalizes the lighting pass, i.e. clears the light clipping volume. Do not rely on shadow map information or the light clipping volume previously generated
  /// with PrepareLightingPass to be set up after calling this function.
  EFFECTS_IMPEXP void FinalizeLightingPass(VisLightSource_cl *pLight, bool bUsesLightClippingVolume);


  /// \brief
  /// Returns a shader appropriate for mobile forward rendering
  ///
  /// \param pLight
  ///   the light source to get the shader for
  ///
  /// \param bBasePass
  ///   If true a lighting shader will be returned that renders the light in the base pass
  ///
  /// \param pSurface
  ///   the surface to be used
  ///
  /// \param fFade
  ///   the fade out factor of the light
  /// 
  /// \param pShadowMapComponent
  ///   the shadow map component, that is potentially attached to the light
  EFFECTS_IMPEXP VCompiledTechnique *GetLightShader(VisLightSource_cl *pLight, bool bBasePass, const VisSurface_cl *pSurface, 
                                                    float fFade, IVShadowMapComponent *pShadowMapComponent);

protected:
  #ifndef _VISION_DOC

  VMobileForwardRenderingSystem *m_pRenderer;

  VLightClippingVolumeRenderer m_lightClippingVolumeRenderer;
  VStateGroupDepthStencil m_lightClippingStencilStatesRead[2];

  // If specified light has a shadow map component attached that is compatible to VMobileForwardRenderingSystem, 
  // then the corresponding pointer to the component is returned, otherwise NULL. 
  IVShadowMapComponent* GetCompatibleShadowMapComponent(VisLightSource_cl *pLight);
  
  // Calculates approximated light influence area in screen space
  int GetLightInfluenceArea(VisLightSource_cl *pLight);

  // The light with the highest priority will be rendered in the base pass.
  int GetLightPriority(VisLightSource_cl *pLight);

  // Determines which lights have to rendered in current frame
  void DetermineRelevantLights();

  // Temporary collection for all lights that have to be rendered in current frame
  VisLightSrcCollection_cl m_DynamicLightCollection;

  // All geometry that would be effected by the base pass light but contains non primary opaque surfaces 
  // will be rendered additively.
  VisStaticGeometryInstanceCollection_cl m_AdditiveLitGeoInstanceCollection;
  VisEntityCollection_cl m_AdditiveLitEntityCollection;

  VisLightSource_cl *m_pBasePassLight; // dynamic light rendered in the base pass 
  int m_iBasePassLightPriority;        // light with highest priority will be rendered in base pass

  #endif  //_VISION_DOC
};

#endif // VMOBILE_FORWARDRENDERLOOP_HPP_INCLUDED

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
