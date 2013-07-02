/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VPostProcessTranslucencies.hpp

#ifndef POSTPROCESS_TRANSLUCENCIES_HPP_INCLUDED
#define POSTPROCESS_TRANSLUCENCIES_HPP_INCLUDED

class VRendererNodeCommon;
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/PostProcessBase.hpp>

/// \brief
///   Used internally to render translucent parts of the scene.
///
/// \note
///  It is assumed that translucent scene elements which do not directly depend on a render hook
///  (i.e., entities and static geometry instances) have a "post-basepass" shader applied. If this
///  is not the case, they will be rendered in the initial pass render loop, thus producing visually
///  incorrect results!
class VPostProcessTranslucencies : public VPostProcessingBaseComponent
{
public:

  /// \brief
	/// Constructor of the translucency post processor.
	/// 
	/// \param bQuarterSizeParticles
  /// If true, certain particle and mesh buffer effects (e.g. volumetric cones) will be rendered into a
  /// quarter-size render target, thus improving up performance. The down sampled render target will be upscaled
  /// and blended with the full-size accumulation buffer.
  /// This only works on PS3 and Xbox360, not on PC.
	/// 
	VPostProcessTranslucencies(bool bQuarterSizeParticles);

  /// \brief
	/// Destructor of the translucency post-processor.
  ~VPostProcessTranslucencies();

  /// \brief
	/// Overridden Execute function
	EFFECTS_IMPEXP virtual void Execute() HKV_OVERRIDE;

  /// \brief
  /// Returns if particles are rendered quarter size
  ///
  EFFECTS_IMPEXP bool IsQuarterSizeParticles() const { return m_bQuarterSizeParticles; }

protected:

  EFFECTS_IMPEXP virtual void InitializePostProcessor() HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual void DeInitializePostProcessor() HKV_OVERRIDE;

  void DrawEntitiesShaders(const VisEntityCollection_cl &EntityCollection, VPassType_e ePassType);

  // Renders all particles, handling quarter-size rendering
  void RenderParticles( const VisMeshBufferObjectCollection_cl * pVisibleMeshBuffer, const VisParticleGroupCollection_cl * pVisibleParticleGroups );

#ifndef _VISION_MOBILE
  void CreateQuarterSizeShadersAndContext();
  void SwitchToLowResContext();
  void RestorePreviousContext();
  void BlendQuarterSizeIntoTarget(VCompiledTechnique* pTechnique);
#endif

  void RenderDeferredParticles(const VisParticleGroupCollection_cl *pVisibleParticleGroups, unsigned int iOrder);

  void MaskOutForegroundEntities(const VisEntityCollection_cl &EntityCollection);
  void DrawTransparentForegroundEntities(const VisEntityCollection_cl &EntityCollection);

  VisRenderContext_cl *m_pOldContext;

  bool m_bQuarterSizeParticles;
#ifndef _VISION_MOBILE
  VisRenderContextPtr m_spLowResContext;
  VisRenderableTexturePtr m_spLowResColorTexture;
  VisRenderableTexturePtr m_spLowResDepthTexture;
#endif
  VCompiledTechniquePtr m_spCopyTranslucenciesAlphaTechnique;
  VCompiledTechniquePtr m_spCopyTranslucenciesAdditiveTechnique;
  VCompiledTechniquePtr m_spForegroundMaskTechnique;
  VCompiledTechniquePtr m_spForegroundFillPassTechnique;

  VVisibilityObjectCollector m_VisibilityObjectCollector;

  VisParticleGroupCollection_cl *m_TempParticleGroupCollection;
  VisMeshBufferObjectCollection_cl *m_TempMeshBufferObjectCollection;


public:
  // VTypedObject
  V_DECLARE_DYNAMIC_DLLEXP(VPostProcessTranslucencies,EFFECTS_IMPEXP)

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
