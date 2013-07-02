/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file MirrorRenderLoop.hpp

#ifndef MIRRORRENDERLOOP_HPP_INCLUDED
#define MIRRORRENDERLOOP_HPP_INCLUDED

/// \brief
///   Render loop for rendering the scene in reflection contexts (e.g. mirror, water)
///
/// This render loop draws a simplified representation of the scene. Instead of the default, "detailed" shaders,
/// it does not use features such as normal, specular, and parallax mapping. It does, however, take dynamic
/// illumination into account.
class MirrorRenderLoop_cl : public VisionRenderLoop_cl
{
public:

  /// \brief
  ///   Constructor of the render loop, taking the mirror/water instance it will be used for as input.
  MirrorRenderLoop_cl(VisMirror_cl *pMirror);

  /// \brief
  ///   Destructor of the mirror render loop.
  virtual ~MirrorRenderLoop_cl();

  /// \brief
  ///   Overridden render loop callback.
  virtual void OnDoRenderLoop(void *pUserData) HKV_OVERRIDE;

  /// \brief
  ///   Callback handler implementation.
  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

protected:

  /// \brief
  ///   Helper function that renders a simplified dynamic light pass on the scene geometry.
  void DrawDynamicLight();

  /// \brief
  ///   Helper function that renders a simplified variant of the static geometry surface shaders.
  void DrawStaticGeometry(const VisStaticGeometryInstanceCollection_cl &collection, int iPassType);

  /// \brief
  ///   Returns the appropriate "simplified" shader version for rendering the passed material in reflection textures.
  VCompiledShaderPass *GetMirrorShader(const VisSurface_cl *pMaterial, VisMirror_cl::VReflectionShaderSets_e Mode);

  /// \brief
  ///   Returns the appropriate "simplified" shader version for rendering the passed light on the passed geometry in the mirror.
  void GetLightShader (const VisLightSource_cl *pLight, VisStaticGeometryInstance_cl *pGI, VisMirror_cl::VReflectionShaderSets_e Mode,
    VisSurface_cl* pLastSurface, VCompiledTechnique* pLastTechnique, VisSurface_cl*& out_pNewSurface, VCompiledTechnique*& out_pNewTechnique);

  /// \brief
  ///   Creates the shader objects for the simple lighting techniques.
  void CreateSimpleShaders();
  
  VisSurface_cl m_dummySurface;

  VisMirror_cl *m_pMirror;

  VCompiledTechniquePtr m_spDefaultLightGrid;
  VCompiledTechniquePtr m_spDefaultLightMapping;

  VisStaticGeometryInstanceCollection_cl m_visiblePrimaryOpaquePassGeoInstances;
  VisStaticGeometryInstanceCollection_cl m_visibleSecondaryOpaquePassGeoInstances;
  VisStaticGeometryInstanceCollection_cl m_visibleTransparentOpaquePassGeoInstances;
  VisStaticGeometryInstanceCollection_cl m_CustomGeoInstances;
  VisStaticGeometryInstanceCollection_cl m_visOpaqueGeoInstancesNoCustom;
  VisEntityCollection_cl m_visEntities;

  VisFrustum_cl m_Frustum[MAX_SEPARATE_FRUSTA];

  VStateGroupDepthStencil m_dynLightDefaultState;

public:
  // VTypedObject
  V_DECLARE_DYNAMIC_DLLEXP(MirrorRenderLoop_cl,EFFECTS_IMPEXP)
};


#endif    //MIRRORRENDERLOOP_HPP_INCLUDED

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
