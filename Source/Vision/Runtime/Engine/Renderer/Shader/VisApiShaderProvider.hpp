/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiShaderProvider.hpp

#ifndef VISAPISHADERPROVIDER_HPP_INCLUDED
#define VISAPISHADERPROVIDER_HPP_INCLUDED

class VisSurface_cl;

//-----------------------------------------------------------------------------------

/// \brief
///   Interface that can be installed in the application to supply all materials with appropriate
///   lighting shaders (static and dynamic)
/// 
/// Custom implementations can thus change the engine's default material lighting behavior.
/// 
/// A shader provider instance can be installed using IVisApp_cl::SetShaderProvider.
/// 
/// By default, the VisionShaderProvider_cl class is used.
class IVisShaderProvider_cl : public VTypedObject, public VRefCounter
{
protected:
  VISION_APIFUNC IVisShaderProvider_cl();
  VISION_APIFUNC virtual ~IVisShaderProvider_cl();

public:
  V_DECLARE_DYNAMIC_DLLEXP(IVisShaderProvider_cl, VISION_APIDATA);

  ///
  /// @name General Shader Assignment
  /// @{
  ///

  /// \brief
  ///   SetEffect assigns a given effect to a surface. It filters out the individual shader techniques
  ///   and assigns them to the corresponding properties of the passed surface.
  /// 
  /// A VisSurface_cl object can possess a number of rendering techniques used for different purposes. In order
  /// to be able to access these techniques quickly, the individual techniques should be stored in the various
  /// shader technique pointers provided by the VisSurface_cl class. This default implementation of the SetEffect
  /// method uses the shader techniques' include tags to create the following techniques from the passed effect:
  /// \li "LIGHTMAPDOT3", "LIGHTMAP", "LIGHTGRID": Static lighting techniques
  /// \li "ALPHATEST": Additional include flag for alpha-tested materials
  /// \li "MIRROR;LIGHTMAP", "MIRROR;LIGHTGRID", "MIRROR" (if none of the other two tags is set): Technique to use
  ///   in order to render this material in mirrors/water surfaces
  /// \li "POINTLIGHT", "SPOTLIGHT", "PROJPOINTLIGHT", "DIRECTIONALLIGHT", "SHADOWMAP": Dynamic lighting
  ///   techniques.
  /// \li "CUSTOM0"..."CUSTOM7": Additional include flag for custom technique assignment (stores the technique
  ///   in the user-definable m_spCustomTechniques[] array of the passed surface)
  /// Derived implementations of this method can be used to extract different techniques from the passed effect,
  /// storing them in the user-definable m_spCustomTechniques[] array. This facilitates the implementation of
  /// custom render loops with custom rendering techniques requiring special shaders.
  ///
  /// \param pSurface
  ///   The surface into which to set the specified effect. May not be NULL.
  /// 
  /// \param pFX
  ///   Pointer to the effect to set. This can be NULL; in this case, the shader provider's CreateMaterialEffect()
  ///   function will be called to create a default effect for the passed surface.
  /// \param pDefaultConfig
  ///   Default config that can be set if additional inclusion or exclusion tags are needed for this effect.
  VISION_APIFUNC virtual void SetEffect(VisSurface_cl *pSurface, VCompiledEffect *pFX, 
    VTechniqueConfig* pDefaultConfig = NULL);

  ///
  /// @}
  ///
  ///

  /// @name Static Lighting Effects
  /// @{
  ///
  
  /// \brief
  ///   This is a key function of the shader provider: Return the appropriate shader effect for a
  ///   surface
  /// 
  /// This function has to be overridden to return a valid effect.
  /// 
  /// It might analyse the surface's properties (specular properties etc.) and create the effect
  /// from it.
  /// 
  /// This function is called by the engine at loading time to gather the default (static lighting)
  /// effect for a material.
  /// 
  /// \param pSurface
  ///   The surface the engine queries the effect for.
  /// 
  /// \param iFlags
  ///   additional creation flags. Currently the SURFACEFLAGS_NODEFAULTSHADERS is supported. See
  ///   Default implementation.
  /// 
  /// \return
  ///   VCompiledEffect* pEffect: Result effect. Should not be NULL.
  virtual VCompiledEffect* CreateMaterialEffect(VisSurface_cl *pSurface, int iFlags=0) = 0;

  /// \brief
  ///   Pure virtual function to set the default lighting color of the scene (used if lightmap or
  ///   lightgrid is not available)
  virtual void SetDefaultLightingColor(VColorRef iColor) = 0;

  /// \brief
  ///   Pure virtual function that applies lighting shader effects to all models
  virtual void CreateLightingShaderForAllModels(bool bIncludeManualAssignments = false) = 0;

  /// \brief
  ///   Pure virtual function that applies lighting shader effects to the passed model. May be
  ///   called by CreateLightingShaderForAllModels
  virtual void CreateLightingShaderForModel(VDynamicMesh *pMesh, bool bIncludeManualAssignments = false) = 0;

  // do not convert
  // ** * CreateLightingShaderForMesh ** *
  //
  // Syn opsis:
  //    - Pure virtual function that applies lighting shader effects to the passed mesh.
  //
  //virtual void CreateLightingShaderForMesh(VisStaticMesh_cl *pStaticMesh) = 0;

  
  /// \brief
  ///   Pure virtual function that applies static lighting effects to all world surfaces.
  virtual void CreateLightingShaderForWorld(bool bIncludeManualAssignments = false) = 0;

  /// \brief
  ///   Extracts a default parameter string from the passed surface
  ///
  /// This function is called to extract default parameters from a VisSurface_cl object and put them into
  /// a parameter string. The parameter string can then be used to create effects/techniques. The default
  /// implementation extracts the following parameters from the surface data: CullMode, DepthWrite,
  /// PassType, DepthBias, DepthBiasClamp, SlopeScaledDepthBias, MaterialParams, and AlphaThreshold.
  /// Derived implementations may choose to extract different or additional information.
  ///
  /// \param pSurface
  ///  The VisSurface_cl object for which to generate the parameter string.
  ///
  /// \param pszParamString
  ///  Pointer to the parameter string. Has to be at least 1024 bytes in length.
  ///
  /// \returns
  ///  Pointer to the end of the parameter string.
  VISION_APIFUNC virtual char *GetDefaultParamStringForSurface(VisSurface_cl *pSurface, char *pszParamString);

  ///
  /// @}
  ///

  ///
  /// @name Dynamic Lighting
  /// @{
  ///

  
  /// \brief
  ///   Pure virtual function that returns a shader technique used for dynamically lighting
  ///   material pSurface with light pLight
  /// 
  /// All shader passes in the returnded technique have to be ready for rendering, i.e. have the
  /// light position/color etc. updated.
  /// 
  /// As performance is quite critical for this function, this function should use the cached
  /// surface techniques (e.g. pSurface->m_spDynPointLight) and just modify the passes in it.
  /// 
  /// \param pLight
  ///   The light source
  /// 
  /// \param pSurface
  ///   The surface to be lit dynamically
  /// 
  /// \param bForEntity
  ///   effect used for models or not (same as !pSurface->IsWorldSurface())
  /// 
  /// \return
  ///   VCompiledEffect* pEffect: Result effect. Should not be NULL.
  virtual VCompiledTechnique *GetDynamicLightShader(const VisLightSource_cl *pLight, const VisSurface_cl *pSurface, bool bForEntity) = 0;

  ///
  /// @}
  ///

  ///
  /// @name Vforge
  /// @{
  ///


  /// \brief
  ///   vForge calls this to set a shader effect that should be used for alternative geometry
  ///   rendering
  /// 
  /// Thus custom rendering plugins can check this value (GetReplacementEffect) and use it to
  /// render custom geometry with the same shading type.
  /// 
  /// This feature is only relevant inside vForge.
  ///
  /// \param pEffect
  ///  The replacement effect to set
  virtual void SetReplacementEffect(VCompiledEffect *pEffect)
  {
    m_spReplacementEffect=pEffect;
  }

  /// \brief
  ///   Returns the effect that is set via SetReplacementEffect. This is NULL for normal rendering
  ///   and outside vForge
  inline VCompiledEffect *GetReplacementEffect() const
  {
    return m_spReplacementEffect;
  }

  ///
  /// @}
  ///

  ///
  /// @name Custom Material Properties
  /// @{
  ///

  /// \brief
  ///   Overridable that is called for every surface after loading (or before saving) material XML
  ///   files.
  /// 
  /// This function can be overridden to save custom properties along with an XML file.
  /// 
  /// To store custom info in a VisSurface_cl, the user data void pointer can be used.
  /// 
  /// For convenience, use the XMLHelper::Exchange_xyz functions to get the data from or into the
  /// node.
  /// 
  /// \param pMaterial
  ///   The material that is loaded/saved
  /// 
  /// \param pMaterialNode
  ///   The xml node of the material that contains custom properties (loading) or should receive
  ///   custom properties (saving)
  /// 
  /// \param bWrite
  ///   true for saving mode (put material properties into XML) or false for loading (put XML
  ///   properties into material)
  virtual void MaterialXMLDataExchange(VisSurface_cl* pMaterial, TiXmlElement *pMaterialNode, bool bWrite)
  {
  }



  ///
  /// @}
  ///

  ///
  /// @name Misc
  /// @{
  ///


  /// \brief
  ///   Pure virtual function that gets called by the renderloop implementation before actual
  ///   rendering
  virtual void ResetCache() = 0;


  ///
  /// @}
  ///

  ///
  /// @name Load specific Shader base Libraries
  /// @{
  ///

  /// \brief
  ///   This function can be overridden to return the 'BaseShaders.ShaderLib' shader library from a custom path.
  VISION_APIFUNC virtual VShaderEffectLib *LoadBaseShaderLib();

  /// \brief
  ///   This function can be overridden to return the 'FixedFunctionShaders.ShaderLib' shader library from a custom path.
  VISION_APIFUNC virtual VShaderEffectLib *LoadFixedFunctionShaderLib();

  /// \brief
  ///   This function can be overridden to return the 'Particles.ShaderLib' shader library from a custom path.
  VISION_APIFUNC virtual VShaderEffectLib *LoadDefaultParticleShaderLib();

  ///
  /// @}
  ///

  // internal handling functions
  VCompiledEffectPtr m_spReplacementEffect;

protected:

  friend class VisSurface_cl;

  VISION_APIFUNC void SetEffect_Internal(VisSurface_cl *pSurface, VCompiledEffect *pFX, 
    bool bUseSimpleShaders, VTechniqueConfig* pDefaultConfig);

  VISION_APIFUNC void AssignCustomTechniques(VisSurface_cl *pSurface, VCompiledEffect *pFX);
  short m_iCustomTagIndicies[8];

  // Helpers
  /// \brief
  ///   Tries to find a compatible technique with several fallbacks.
  ///
  /// Specific fallbacks can be provided in the pConfigChain array. If no technique
  /// compatible with any of these fallbacks can be found, the auto-assignment effect 
  /// is used to search for a compatible technique.
  /// 
  /// \param pSurface
  ///   The surface to create the technique for.
  /// \param pFX
  ///   The effect to be searched.
  /// \param pConfigChain
  ///   Array of technique configs to search a compatible technique for.
  /// \param iNumConfigsInChain
  ///   The length of the technique config chain.
  /// \param pGlobalConfig
  ///   Global config override, can be NULL.
  VISION_APIFUNC VCompiledTechnique* SafeCreateTechnique(
    VisSurface_cl *pSurface, VCompiledEffect *pFX, 
    const VTechniqueConfig* pConfigChain, int iNumConfigsInChain, 
    const VTechniqueConfig* pGlobalConfig);
};

typedef VSmartPtr<IVisShaderProvider_cl> IVisShaderProviderPtr; 

//-----------------------------------------------------------------------------------

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
