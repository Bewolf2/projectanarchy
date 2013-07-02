/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VMobileShadowMapComponentSpotDirectional.hpp

#ifndef VMOBILESHADOWMAPCOMPONENT_SPOTDIRECTIONAL_HPP_INCLUDED
#define VMOBILESHADOWMAPCOMPONENT_SPOTDIRECTIONAL_HPP_INCLUDED

#include <Vision/Runtime/Base/Graphics/Shader/vShaderIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VBaseShadowMapComponentSpotDirectional.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>

#define MOBILE_MAX_SHADOW_PARTS_COUNT 1

#define SHADOWMAP_COMPONENT_MOBILE_SPOTDIR_VERSION_0           0 // Initial version
#define SHADOWMAP_COMPONENT_MOBILE_SPOTDIR_CURRENT_VERSION     SHADOWMAP_COMPONENT_MOBILE_SPOTDIR_VERSION_0 // Current version

class VMobileShadowMapComponentSpotDirectional;
typedef VSmartPtr<VMobileShadowMapComponentSpotDirectional> VMobileShadowMapComponentSpotDirectionalPtr;

class VMobileDynamicLightShader;

/// \brief
///   Shadow map component for spotlights and directional lights on mobile devices.
///
/// This class represents an implementation of the VBaseShadowMapComponentSpotDirectional base class to be used for spot and
/// directional lights on mobile devices.
/// 
/// In contrast to VShadowMapComponentSpotDirectional, here shadow maps for directional and spot lights do not use a parallel-split
/// cascaded approach. Furthermore instead of using a shadow mask, the generated shadow map is directly used inside the lighting step.
/// 
/// \sa IVShadowMapComponent
class VMobileShadowMapComponentSpotDirectional : public VBaseShadowMapComponentSpotDirectional
{
public:
  ///
  /// @name Initialization
  /// @{
  ///

  /// \brief
  ///   Constructor taking the associated renderer node index as input.
  EFFECTS_IMPEXP VMobileShadowMapComponentSpotDirectional(int iRendererNodeIndex);

  /// \brief
  ///   Constructor.
  EFFECTS_IMPEXP VMobileShadowMapComponentSpotDirectional();

  /// \brief
  ///   Destructor.
  EFFECTS_IMPEXP virtual ~VMobileShadowMapComponentSpotDirectional();

  /// \brief
  ///   Used internally.
  EFFECTS_IMPEXP virtual bool InitializeRenderer() HKV_OVERRIDE;

  ///
  /// @}
  ///

  ///
  /// @name Overridden functions
  /// @{
  ///

  /// \brief
  ///   This implementation simply calculates shadow mapping related shader-parameters
  EFFECTS_IMPEXP virtual void Render(bool bUseLightClippingVolume, bool bClearTexture) HKV_OVERRIDE;

  /// \brief
  ///   Returns the shadow map.
  EFFECTS_IMPEXP virtual VTextureObject *GetShadowTexture() const HKV_OVERRIDE
  {
    return GetShadowMapGenerator()->GetShadowMap();
  }

#if defined(WIN32) || defined(_VISION_DOC)

  /// \brief
  ///   Overridden from base class.
  EFFECTS_IMPEXP virtual void GetVariableAttributes(VisVariable_cl* pVariable, VVariableAttributeInfo& destInfo) HKV_OVERRIDE;

#endif

  /// \brief
  ///   Overridden from base class.
  EFFECTS_IMPEXP virtual void SetCascadeCount(unsigned int iCount) HKV_OVERRIDE;

  /// \brief
  ///   Overridden from base class.
  EFFECTS_IMPEXP virtual void SetCascadeSelection(VCascadeSelectionMethod_e cascadeSelection) HKV_OVERRIDE;

  /// \brief
  ///   Overridden from base class.
  EFFECTS_IMPEXP virtual void SetCascadeRange(int iCascade, float fRange) HKV_OVERRIDE;

  /// \brief
  ///   Overridden from base class.
  EFFECTS_IMPEXP virtual void SetOverestimateCascades(BOOL bOverestimate) HKV_OVERRIDE;

  /// \brief
  ///   Overridden from base class.
  EFFECTS_IMPEXP virtual void Serialize(VArchive &ar) HKV_OVERRIDE;

  ///
  /// @}
  ///


  ///
  /// @name Shadow Properties
  /// @{
  ///

  /// \brief
  ///   Sets the maximum intensity [0.0-1.0] of subtractive shadows.
  EFFECTS_IMPEXP void SetMaxIntensity(float fMaxIntensity);

  /// \brief
  ///   Returns the maximum intensity of subtractive shadows.
  inline float GetMaxIntensity() const { return MaxIntensity; }

  ///
  /// @}
  ///


  ///
  /// @name Update
  /// @{
  ///

  /// \brief
  ///   Updates shadow mapping related parameters of specified light shader.
  void UpdateLightShader(VMobileDynamicLightShader *pPass);

  ///
  /// @}
  ///

  #ifndef _VISION_DOC

  EFFECTS_IMPEXP virtual void SetCascadeRangePtr(float* range, int size) HKV_OVERRIDE;

  V_DECLARE_VARTABLE(VMobileShadowMapComponentSpotDirectional, EFFECTS_IMPEXP);
  V_DECLARE_SERIAL_DLLEXP(VMobileShadowMapComponentSpotDirectional, EFFECTS_IMPEXP);

  #endif

protected:
  // Vartable Properties
  float MaxIntensity;  

  hkvMat4 m_LightProjection[4];
  float m_fFadeOutStart, m_fFadeOutEnd;
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
