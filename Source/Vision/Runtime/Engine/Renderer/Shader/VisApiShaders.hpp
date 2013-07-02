/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiShaders.hpp

#ifndef DEFINE_VISAPISHADERS
#define DEFINE_VISAPISHADERS

#include <Vision/Runtime/Engine/Renderer/Shader/VisApiShaderEffect.hpp>



class VisShaderFXLibManager_cl;
class VisRenderContext_cl;
class IVShadowMapComponent;

/// \brief
///   DEPRECATED
typedef char* (VISION_FASTCALL * VISTEXTUREMODIFIERFUNC2) (VTextureObject *pTexture, int colordepth, char *src_data, int xofs, int yofs, int width, int height, int miplevel, unsigned int &newinternalformat, float modParam);


/// \brief
///   callback function for custom base textures (DEPRECATED)
/// 
/// \param iPrimNum
///   world primitive number for the queried base texture.
/// 
/// \return
///   VTextureObject* pTexture : The base texture to render the primitive with. If NULL, use the
///   standard base texture from the surface.
/// 
/// \sa VisShaders_cl::RegisterPrimitiveBaseTextureCallback
typedef VTextureObject* (VISION_FASTCALL * VISBASETEXTURECALLBACK) (int iPrimNum);


// flags for loading shader library
#define SHADERLIBFLAG_NONE        0x00000000
#define SHADERLIBFLAG_HIDDEN      0x00000001
#define SHADERLIBFLAG_NO_UNLOAD   0x00000002



/// \brief
///   Class for managing compiled shaders.
/// 
/// VisCompiledShaderManager_cl provides callbacks for managing compiled shaders and their
/// properties.
/// 
/// Currently, it is used for unbinding elements in DX10/11 (e.g. state groups) which are currently
/// still set in the pipeline.
class VisCompiledShaderManager_cl : public VCompiledShaderManager
{
public:
  VisCompiledShaderManager_cl(IVTextureLoader &textureLoader, VTypeManager &typeManager)
    : VCompiledShaderManager(textureLoader, typeManager)
  {
  }

  VOVERRIDE void OnShaderPassDestroyed(VCompiledShaderPass *pPass);
};



/// \brief
///   Class for accessing the Vision Shader System.
/// 
/// The global instance of this class can be accessed via Vision::Shader.
/// 
/// This class wraps around some useful shader library accessing and technique creation functions.
class VisShaders_cl
{
public:

  // internal
  VISION_APIFUNC void Init();
  VISION_APIFUNC void DeInit();

  ///
  /// @name Shader Creation Related
  /// @{
  ///

  /// \brief
  ///   Access the global resource manager that handles the loaded shader libraries
  /// 
  /// This resource manager can be used to directly access shader libraries and effect resources.
  /// 
  /// \return
  ///   VisShaderFXLibManager_cl& libManager : Reference to the global shader library manager; See
  ///   class VisShaderFXLibManager_cl
  VISION_APIFUNC VisShaderFXLibManager_cl& GetShaderFXLibManager() const;

  /// \brief
  ///   Loads a shader library file (.ShaderLib)
  /// 
  ///  If the library is already loaded, it returns the cached one.
  /// 
  /// This function wraps around GetShaderFXLibManager()->LoadShaderLibrary.
  /// 
  /// The resource manager that keeps track of loaded libraries is purged when unloading a scene.
  /// 
  /// The filename must contain the project relative path to the shader library.
  /// 
  /// \param szFilename
  ///   filename of the library including .ShaderLib extension (accessed through standard file
  ///   filemanager)
  /// 
  /// \param iFlags
  ///   Optional loading flags. Supported flags are:
  ///   \li SHADERLIBFLAG_NONE : No specific flags
  /// 
  ///   \li SHADERLIBFLAG_HIDDEN : The loaded library will not show up in the shader editor. Useful
  ///     for internal shader libraries
  /// 
  /// \return
  ///   VShaderEffectLib* pLib : Pointer to the shader library (or NULL, if library could not be
  ///   loaded)
  VISION_APIFUNC VShaderEffectLib* LoadShaderLibrary(const char *szFilename, int iFlags=SHADERLIBFLAG_NONE);

  /// \brief
  ///   Creates a compiled effect using a parameter string to modify the effect resource.
  /// 
  /// This function finds a shader effect in any currently loaded shader library (or in the passed one) and wraps around
  /// CompileEffect on the found effect resource.
  /// 
  /// The function returns a NULL pointer if either the effect could not be found in any loaded
  /// library or if the compiling failed.
  /// 
  /// \param szEffectName
  ///   Name of the effect (as set in the shader editor)
  /// 
  /// \param szParamString
  ///   Parameter string with varname=value pairs that is used to modify the compiled shaders,
  ///   e.g. "Color=1,1,1,0;Glow=0.5;"
  /// 
  /// \param iFlags
  ///   Additional flags for creating the effect. Supported bit constants are:
  ///   \li EFFECTCREATEFLAG_NONE: No specific flags.
  /// 
  ///   \li EFFECTCREATEFLAG_FORMODELS: The vertex shader for models should be used (if available).
  /// 
  ///   \li EFFECTCREATEFLAG_FORCEUNIQUE: If set, both the effect instance and the shader instances
  ///     are neither taken from the cache nor re-used for other compiling operations.
  /// 
  ///   \li EFFECTCREATEFLAG_NOREPORTONFAIL : If set, the error report dialog ignores this call if
  ///     it fails (e.g. the effect name is not there).
  /// 
  ///   \li EFFECTCREATEFLAG_NOCONSTANTBUFFER: If set, all shader instances in this effect will
  ///     have no own vertex and pixel shader constant tables. Instead, all constants have to be
  ///     set manually using VisRenderStates_cl::SetVertexShaderConstant in a custom render loop.
  /// 
  /// \param pFromLib
  ///   enforces to use a library to create the effect from (avoids effect naming collisions). Can be NULL to find the effect in any loaded library
  ///
  /// \return
  ///   VCompiledEffect *pEffect : Compiled effect instance (or NULL). This effect contains one or
  ///   more compiled techniques.
  VISION_APIFUNC VCompiledEffect* CreateEffect(const char *szEffectName, const char *szParamString, int iFlags=EFFECTCREATEFLAG_NONE, VShaderEffectLib *pFromLib=NULL);

  /// \brief
  ///   This function wraps around CreateEffect and returns a compatible shader technique
  /// 
  /// \param szEffectName
  ///   Name of the effect (as set in the shader editor)
  /// 
  /// \param szParamString
  ///   Parameter string with varname=value pairs that is used to modify the compiled shaders,
  ///   e.g. "Color=1,1,1,0;Glow=0.5;"
  /// 
  /// \param pConfig
  ///   user defined config to test compatibility against. This can be NULL to use global
  ///   exclusion/inclusion tags set via SetGlobalTechniqueConfig.
  /// 
  /// \param iFlags
  ///   Additional flags for creating the effect. Supported bit constants are:
  ///   \li EFFECTCREATEFLAG_NONE: No specific flags,
  /// 
  ///   \li EFFECTCREATEFLAG_FORMODELS: The vertex shader for models should be used (if available),
  /// 
  ///   \li EFFECTCREATEFLAG_FORCEUNIQUE: If set, both the effect instance and the shader instances
  ///     are neither taken from the cache nor re-used for other compiling operations
  /// 
  ///   \li EFFECTCREATEFLAG_NOREPORTONFAIL : If set, the error report dialog ignores this call if
  ///     it fails (e.g. the effect name is not there)
  /// 
  ///   \li EFFECTCREATEFLAG_NOCONSTANTBUFFER: If set, all shader instances in this technique will
  ///     have no own vertex and pixel shader constant tables. Instead, all constants have to be
  ///     set manually using VisRenderStates_cl::SetVertexShaderConstant in a custom render loop.
  ///
  /// \param pFromLib
  ///   enforces a library to create the effect from (avoids effect naming collisions). Can be NULL to find the effect in any loaded library
  ///
  /// \return
  ///   VCompiledTechnique *pTechnique : Compiled technique instance (or NULL).
  VISION_APIFUNC VCompiledTechnique* CreateTechnique(const char *szEffectName, const char *szParamString, VTechniqueConfig *pConfig=NULL, int iFlags=EFFECTCREATEFLAG_NONE, VShaderEffectLib *pFromLib=NULL);

  /// \brief
  ///   This function returns unique cloned technique from given technique.
  ///
  /// \param pTechToClone
  ///   Valid technique to clone.
  ///
  /// \return
  ///   Unique clone from input technique.
  VISION_APIFUNC VCompiledTechnique* CloneTechnique(VCompiledTechnique* pTechToClone);

  ///
  /// @}
  ///

  ///
  /// @name Exclusion/inclusion Tags
  /// @{
  ///

  /// \brief
  ///   Sets a technique config with inclusion/exclusion tags that is used whenever NULL is passed
  ///   as the pConfig parameter in CreateTechnique.
  /// 
  /// The technique config is used as a filter to get a specific compiled technique from a shader
  /// effect (e.g. with fog/without fog,...).
  /// 
  /// Since all engine-internal technique creation functions use a NULL pointer for pConfig, you
  /// can control technique fallbacks used for shader creation with this function.
  /// 
  /// For instance, light grid shaders can be forced to vertex lighting only.
  /// 
  /// A NULL pointer can be passed to this function to reset the config (i.e. disable the filter)
  /// 
  /// \param pConfig
  ///   New config (will be copied inside this function). Can be NULL to reset the global config so
  ///   no filtering takes place (default).
  VISION_APIFUNC void SetGlobalTechniqueConfig(VTechniqueConfig *pConfig);

  /// \brief
  ///   This version of SetGlobalTechniqueConfig parses an inclusion/exclusion tag string. See
  ///   overload that takes a VTechniqueConfig pointer. 
  VISION_APIFUNC void SetGlobalTechniqueConfig(const char *szInclusionTags, const char *szExclusionTags);

  /// \brief
  ///   Returns the global instance of VTechniqueConfig that is used fo filtering technique
  ///   fallbacks. It is always != NULL
  VISION_APIFUNC VTechniqueConfig* GetGlobalTechniqueConfig();
 

  ///
  /// @}
  ///

  ///
  /// @name Debugging Functionality
  /// @{
  ///

  /// \brief
  ///   Displays the passed texture as a screen overlay. For debugging purposes.
  VISION_APIFUNC void DisplayDebugTexture(VTextureObject *pTex);

  /// \brief
  ///   Reloads all shader libraries that have changed (according to timestamp) and re-applies
  ///   affected shaders
  /// 
  /// This function re-applies changed shader effects to known surfaces, i.e. all surfaces of
  /// static meshes and entity models
  /// 
  /// To respond to changes for custom materials, you can listen to the
  /// Vision::Callbacks.OnReassignShaders callback.
  /// 
  /// The return value corresponds to the number of changed library files.
  VISION_APIFUNC int ReloadChangedShaderLibraries();

  /// \brief
  ///   Reloads all (known) shader assignment material files and reassigns the shaders. Known material files include models and static meshes.
  VISION_APIFUNC void ReloadAllShaderAssignmentFiles();

  ///
  /// @}
  ///

  ///
  /// @name Per-primitive Callbacks
  /// @{
  ///

  /// \brief
  ///   Registers a per-primitive-callback function for shaders callbacks.
  /// 
  /// This callback function can be assigned to a shader in the shader editor (see documentation).
  /// 
  /// A callback function can be registered after Vision::Init and remains valid until
  /// deinitialisation of the engine. Callback registrations "survive" loading scenes.
  /// 
  /// Any shader that will be assigned within LoadWorld (e.g. Shaders from vEdit surfaces) require
  /// respective callbacks to be available.
  /// 
  /// \param szFunctionName
  ///   function name to be accessible in the shader editor as a function name
  /// 
  /// \param functionPointer
  ///   pointer to the function
  /// 
  /// \return
  ///   BOOL bSuccess: TRUE if the function was registered successfully
  /// 
  /// \sa VISSHADERCALLBACKFUNC
  /// 
  /// \example
  ///   \code
  ///   test samplecode:
  ///   VCallbackRetVal_e __fastcall MyTestCallback(VCallbackGeometryType_e sendertype, void *pElement, VCompiledShaderPass *shader) 
  ///   {
  ///     return CALLBACK_CONTINUE;
  ///   }
  ///   ...
  ///   Vision::Shaders.RegisterShaderCallbackPerPrimitive("MyTestCallback",MyTestCallback);
  ///   \endcode
  VISION_APIFUNC BOOL RegisterShaderCallbackPerPrimitive(const char *szFunctionName, VISSHADERCALLBACKFUNC functionPointer);

  /// \brief
  ///   Return the number of registered shader callback functions 
  VISION_APIFUNC int GetPerPrimitiveCallbacksCount();

  /// \brief
  ///   Access a registered shader callback functions by index, where index must be
  ///   [0..GetPerPrimitiveCallbacksCount()]
  VISION_APIFUNC BOOL GetPerPrimitiveCallbackFunction(int index, char **pName, void **pFuncPtr);

  /// \brief
  ///   DEPRECATED, should not be used anymore
  VISION_APIFUNC BOOL RegisterPrimitiveBaseTextureCallback(VISBASETEXTURECALLBACK funcPtr);

  ///
  /// @}
  ///

private:
  VTechniqueConfig m_ExclusionTags;
};

/// \brief
///   Resource manager class for managing shader library files
/// 
/// In addition to its VisResourceManager_cl base class it provides some useful functions to lookup
/// effect resources etc.
/// 
/// Furthermore, it has its own VisCompiledShaderManager_cl instance for keeping cached shader
/// instances.
class VisShaderFXLibManager_cl : public VisResourceManager_cl
{
public:


  /// \brief
  ///   Constructor
  VisShaderFXLibManager_cl();

  /// \brief
  ///   Load a shader library file (.ShaderLib file). See VisShaders_cl::LoadShaderLibrary
  VISION_APIFUNC VShaderEffectLib* LoadShaderLibrary(const char *szFilename, int iFlags=SHADERLIBFLAG_NONE);

  /// \brief
  ///   Loops through all loaded shader libraries and returns the first effect resource with
  ///   specified effect name
  VISION_APIFUNC VShaderEffectResource* FindEffect(const char *szEffectName) const;

  /// \brief
  ///   Loops through all loaded shader libraries and calls VShaderEffectLib::FreeCompiledEffects
  ///   on each
  VISION_APIFUNC void ResetCompiledEffectCaches();

  /// \brief
  ///   Overridden resource manager function
  VISION_APIFUNC VOVERRIDE VManagedResource *CreateResource(const char *szFilename, VResourceSnapshotEntry *pExtraInfo);

  /// \brief
  ///   Return the respective shaderbin file rather than shaderlib
  VISION_APIFUNC VOVERRIDE const char *GetStreamingReplacementFilename(VResourceSnapshotEntry &resourceDesc, const char *szResolvedFilename, char *szBuffer);

  VISION_APIFUNC VOVERRIDE void OnAfterUnusedResourcesPurged();

  VisCompiledShaderManager_cl m_ShaderInstances;
  ///
  /// @}
  ///

};


/// \brief
///   Base class for dynamic light shaders.
///
/// This class exposes functionality commonly present in shaders for dynamic illumination. It assumes the
/// shader constants used for the dynamic lighting parameters have certain names (refer to the implementation
/// of the PostCompileFunction for details).
///
class VDynamicLightShaderBase : public VCompiledShaderPass
{
public:

  /// \brief
  ///   Stores the light source position in the shader's position register.
  ///
  /// \param vPos
  ///   Position of the light source in world space.
  ///
  inline void SetPosition(const hkvVec3& vPos)
  {
    float f[4]={vPos.x,vPos.y,vPos.z,1.f};
    m_RegPos.SetRegisterValueSafeF(this,f);
  }

  /// \brief
  ///   For directional lights only:  stores the light direction in the shader's position register.
  ///
  /// \param vDir
  ///   Light direction
  ///
  inline void SetDirection(const hkvVec3& vDir)
  {
    float f[4]={vDir.x,vDir.y,vDir.z,0.0f};
    m_RegDirection.SetRegisterValueSafeF(this,f);
    m_RegDirectionPS.SetRegisterValueSafeF(this,f);
  }

  /// \brief
  ///   Stores the light source radius in the shader's radius/range register.
  ///
  /// \param fRadius
  ///   Radius of the light source in units.
  ///
  inline void SetRadius(float fRadius)
  {
    float fInvRad = 1.f/fRadius;
    float f[4]={fRadius,fInvRad,fRadius*fRadius, fInvRad*fInvRad}; ///< track useful values: r, 1/r,  r*r, 1/r*r
    m_RegRadius.SetRegisterValueSafeF(this,f);
  }

  /// \brief
  ///   Stores the light source projection angle in the shader's angle register.
  ///
  /// \param fAngle
  ///   Projection angle of the light source in degrees.
  ///
  inline void SetProjectionAngle(float fAngle)
  {
    float fCos = hkvMath::cosDeg (fAngle * 0.5f);
    float f[4] = {fCos, 1.0f/(1.0f - fCos), 0.0f, 0.0f};
    m_RegAngle.SetRegisterValueSafeF(this, f);
  }

  /// \brief
  ///   Stores the light source color in the shader's color register.
  ///
  /// \param vColor
  ///   Color of the light source.
  ///
  /// \param fMultiplier
  ///   Brightness multiplier of the light source.
  ///
  inline void SetColor(const hkvVec3& vColor, float fMultiplier)
  {
    float f[4]={vColor.x*fMultiplier,vColor.y*fMultiplier,vColor.z*fMultiplier,1.f};
    m_RegColor.SetRegisterValueSafeF(this,f);
  }

  /// \brief
  ///   Stores the passed attenuation texture in the shader's attenuation texture sampler.
  ///
  /// \param pTex
  ///   Attenuation texture to use.
  ///
  inline void SetAttenuationTexture(VTextureObject *pTex)
  {
    if (m_iSamplerAttenuation>=0)
    {
      VStateGroupTexture *pStateGroupTexture = GetStateGroupTexture(VSS_PixelShader, m_iSamplerAttenuation);
      if (pStateGroupTexture != NULL)
        pStateGroupTexture->m_spCustomTex = pTex;
    }
  }

  /// \brief
  ///  For shadowmap lights only:  sets the shadowmap texture that will be used to modulate the light's framebuffer contribution.
  inline void SetShadowTexture(VTextureObject *pTex)
  {
    if (m_iSamplerShadowTexture>=0)
    {
      VStateGroupTexture *pStateGroupTexture = GetStateGroupTexture(VSS_PixelShader, m_iSamplerShadowTexture);
      if (pStateGroupTexture != NULL)
        pStateGroupTexture->m_spCustomTex = pTex;
    }
  }

  /// \brief
  ///  Gets called every time before a light gets rendered
  VISION_APIDATA virtual void OnBeforeLightRendered(const VisLightSource_cl* pLight){};

#ifndef _VISION_DOC

  VConstantBufferRegister m_RegPos, m_RegRadius, m_RegAngle, m_RegColor, m_RegDirection, m_RegDirectionPS;
  int m_iSamplerAttenuation, m_iSamplerShadowTexture;

  // shader overrides
  V_DECLARE_SERIAL_DLLEXP( VDynamicLightShaderBase, VISION_APIDATA )
  VISION_APIDATA virtual void PostCompileFunction(VShaderEffectResource *pSourceFX,VShaderPassResource *pSourceShader) HKV_OVERRIDE;

#endif

};


/// \brief
///   Extended class for dynamic light shaders.
///
/// This class extends the base class functionality with some specific methods.
///
class VDynamicLightShader : public VDynamicLightShaderBase
{
public:

  /// \brief
  ///   For spot ligths only: Stores the spot light's projection planes in the respective sampler.
  ///
  /// \param px
  ///   Projection plane in u direction.
  ///
  /// \param py
  ///   Projection plane in v direction.
  ///
  /// \param dist
  ///   Projection plane in light direction.
  ///
  inline void SetProjectionPlanes(const hkvPlane& px, const hkvPlane& py, const hkvPlane& dist)
  {
    m_RegPlanes[0].SetRegisterValueSafeF(this,&px.m_vNormal.x);
    m_RegPlanes[1].SetRegisterValueSafeF(this,&py.m_vNormal.x);
    m_RegPlanes[2].SetRegisterValueSafeF(this,&dist.m_vNormal.x);
  }

  /// \brief
  ///   For spot lights and projected point lights only: Stores the light's projection texture in the respective sampler.
  ///
  /// \param pTex
  ///   Projection texture to use.
  ///
  inline void SetProjectedTexture(VTextureObject *pTex)
  {
    if (m_iSamplerProjection>=0)
    {
      VStateGroupTexture *pStateGroupTexture = GetStateGroupTexture(VSS_PixelShader, m_iSamplerProjection);
      if (pStateGroupTexture != NULL)
        pStateGroupTexture->m_spCustomTex = pTex;
    }
  }

  /// \brief
  ///  Sets the screen resolution
  inline void SetScreenSize(int iWidth, int iHeight)
  {
    // \NOTE: An ICE was occurring if we make use of the two float array in this way. So we have to declare it as static and then assign the values.
    //float f[2] = { 1.0f / iWidth, 1.0f / iHeight };
    static float f[2];
    f[0] = 1.0f / iWidth;
    f[1] = 1.0f / iHeight;
    m_RegInvScreenRes.SetRegisterValueSafeF(this,f);
  }

  #ifndef _VISION_DOC

  VConstantBufferRegister m_RegPlanes[3], m_RegInvScreenRes;
  int m_iSamplerProjection;

  // shader overrides
  V_DECLARE_SERIAL_DLLEXP( VDynamicLightShader, VISION_APIDATA )
  VISION_APIDATA virtual void PostCompileFunction(VShaderEffectResource *pSourceFX,VShaderPassResource *pSourceShader) HKV_OVERRIDE;

  #endif

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
