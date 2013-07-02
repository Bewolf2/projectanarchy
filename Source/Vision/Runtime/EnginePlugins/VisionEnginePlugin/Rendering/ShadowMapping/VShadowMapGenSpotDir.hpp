/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VShadowMapGenSpotDir.hpp

#ifndef VSHADOWMAPGENSPOTDIR_HPP_INCLUDED
#define VSHADOWMAPGENSPOTDIR_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VShadowMapGenerator.hpp>

/// \brief
///   Inherits from VShadowMapGenerator and implements shadow map rendering for spotlights and directional lights.
class VShadowMapGenSpotDir : public VShadowMapGenerator
{
  friend class VBaseShadowMapComponentSpotDirectional;
public:
  EFFECTS_IMPEXP VShadowMapGenSpotDir(IVRendererNode *pRendererNode, VisLightSource_cl *pRefLight, 
    IVShadowMapFormat* pShadowMapFormatProvider, IVShadowMapComponent* pComponent, int iNumParts = 2, int iMaxNumParts = 4);
  EFFECTS_IMPEXP virtual ~VShadowMapGenSpotDir();

  EFFECTS_IMPEXP virtual bool Initialize() HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual void Update(bool force) HKV_OVERRIDE;

  EFFECTS_IMPEXP virtual void UpdateLightShader(VShadowMapApplyShaderPass *pPass, hkvMat4 (&mLightProjection)[4], hkvVec4& vCascadeScale) HKV_OVERRIDE;

  EFFECTS_IMPEXP void ComputeFadeOutParams(float &fFadeOutStart, float &fFadeOutEnd);

  EFFECTS_IMPEXP float GetCascadeCullDistance(int iCascade) const;

  /// \brief
  ///   Updates axis-aligned bounding box in current light view-space that encloses the complete shadow volume
  ///   for orthographic shadows.
  EFFECTS_IMPEXP void UpdateOrthoShadowVolumeVS(const hkvAlignedBBox &boundingBoxWS);

  /// \brief
  ///   Checks whether specified entity is inside the current shadow volume. 
  ///   Only supported for orthographic shadows.
  EFFECTS_IMPEXP bool IsEntityInsideOrthoShadowVolume(const VisBaseEntity_cl *pEntity) const;

  /// \brief
  ///   Checks whether specified static mesh instance is inside the current shadow volume.
  ///   Only supported for orthographic shadows.
  EFFECTS_IMPEXP bool IsMeshInsideOrthoShadowVolume(const VisStaticGeometryInstance_cl *pStaticMeshInstance) const;

protected:
  hkvVec3 m_vLastCameraPos;
  hkvVec3 m_vLastCameraDir;
  hkvVec3 m_vLastLightDir;
  float m_fLastRadius;
  float m_fLastAngle;
  float m_fFovX;
  float m_fFovY;
  float m_fNear;

private:
  // Converts specified axis-aligned bounding box from world-space into view-space of current light
  void GetBoundingBoxVS(hkvAlignedBBox &boundingBox) const;

  hkvAlignedBBox m_ShadowVolumeVS; // axis-aligned bounding box in current light view-space that encloses the complete shadow volume

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
