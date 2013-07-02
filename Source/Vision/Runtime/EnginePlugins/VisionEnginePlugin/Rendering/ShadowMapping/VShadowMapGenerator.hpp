/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VShadowMapGenerator.hpp

#ifndef VSHADOWMAPGENERATORBASE_HPP_INCLUDED
#define VSHADOWMAPGENERATORBASE_HPP_INCLUDED


#include <Vision/Runtime/Base/Graphics/Shader/vShaderIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>

#define MAX_SHADOW_PARTS_COUNT 4

class VShadowMapPart;
typedef VSmartPtr<VShadowMapPart> VShadowCascadePtr;

class VShadowMapGenerator;
typedef VSmartPtr<VShadowMapGenerator> VShadowMapGeneratorBasePtr;

class VShadowMapApplyShaderPass;
typedef VSmartPtr<VShadowMapApplyShaderPass> VShadowMapApplyShaderPassPtr;

#ifndef _VISION_DOC

enum VShadowProjectionType_e
{
  SHADOW_PROJECTION_ORTHOGRAPHIC,
  SHADOW_PROJECTION_PERSPECTIVE
};

enum VShadowViewFrustumSubdivisionType_e
{
  CSM_SUBDIVISION_PARALLEL_SPLIT = 0,
  CSM_SUBDIVISION_CONCENTRIC = 1
};

enum VShadowCastingGeometry_e
{
  SHADOW_CASTER_TERRAIN = 1,
  SHADOW_CASTER_STATICMESHES = 2,
  SHADOW_CASTER_ENTITIES = 4,
  SHADOW_CASTER_SPEEDTREES = 8,
  SHADOW_CASTER_ALL = 0xFF
};

#endif //_VISION_DOC


/// \brief
///   Data object for shadow renderer callbacks. Used by VShadowMapGenerator::OnRenderShadowMap
class VShadowRendererDataObject : public IVisCallbackDataObject_cl
{
public:

  /// \brief
  ///   Constructor of the VShadowRendererDataObject class.
  inline VShadowRendererDataObject(VCallback *pSender, VShadowMapGenerator *pGenerator, int iCascadeIndex) : IVisCallbackDataObject_cl(pSender)
  {
    m_pGenerator = pGenerator;
    m_iCascadeIndex = iCascadeIndex;
  }

  int m_iCascadeIndex;                ///< The cascade index that is currently rendered
  VShadowMapGenerator *m_pGenerator;  ///< The shadow map generator
};

/// \brief
///   Internal class to store properties of the shadow cascade (for directional and spot lights) / the tetrahedron face (for point lights)
class VShadowMapPart : public VRefCounter
{
public:

  EFFECTS_IMPEXP VShadowMapPart();
  EFFECTS_IMPEXP ~VShadowMapPart();
  
  EFFECTS_IMPEXP void Reset();
  EFFECTS_IMPEXP void GetViewport(int &iPosX, int &iPosY, int &iWidth, int &iHeight) const;
  EFFECTS_IMPEXP void Update();
  
  inline void SetShadowMapGenerator(VShadowMapGenerator *pGen)
  {
    m_pShadowMapGenerator = pGen;
  }

  inline void SetRenderContext(VisRenderContext_cl *pContext)
  {
    m_spRenderContext = pContext;
  }

  inline VisRenderContext_cl *GetRenderContext() const
  {
    return m_spRenderContext;
  }

  inline void SetCamera(VisContextCamera_cl *pCam)
  {
    m_spRenderContext->SetCamera(pCam);
  }

  inline VisContextCamera_cl *GetCamera() const
  {
    return m_spRenderContext->GetCamera();
  }

  static void GetViewFrustumCornersAtDistance(const VisRenderContext_cl *pContext, float fDistance, hkvVec3* pVectors);

  EFFECTS_IMPEXP void ComputeOffset(float fStartDistance, float fEndDistance);
  EFFECTS_IMPEXP void ComputeFrustumMesh();
  EFFECTS_IMPEXP void RenderFrustumMesh(VColorRef iColor);

public:

  VShadowMapGenerator* m_pShadowMapGenerator;
  VisRenderContextPtr m_spRenderContext;
  hkvMat4 m_mTexMatrix;
  hkvVec2 m_vTextureOffset;
  float m_fCenterOffset;
  float m_fRadius;
  float m_fCullDistance;
  hkvVec3 m_vFrustumMesh[8];
  VStateGroupDepthStencil* m_sgDepthStencil;

};

/// \brief
///   Internal structure to hold information for VShadowMapGenerator::RenderProfilingData()
struct VShadowProfilingData_t
{
  int iEntitiesPassedToRenderLoop[4];
  int iEntitiesRendered[4];
  int iStaticGIPassedToRenderLoop[4];
  int iStaticGIRendered[4];
};


/// \brief
///   Internal class to provide texture formats and shaders for the various implementations of the shadow map generators.
class IVShadowMapFormat : public VRefCounter
{
public:
  virtual ~IVShadowMapFormat() {}

  EFFECTS_IMPEXP virtual void Initialize(VShadowMapGenerator* pGenerator) { m_pGenerator = pGenerator; }

  EFFECTS_IMPEXP virtual VTextureLoader::VTextureFormat_e GetShadowMapFormat() = 0;
  EFFECTS_IMPEXP virtual VTextureLoader::VTextureFormat_e GetShadowMapFormatDepthStencil() = 0;
  EFFECTS_IMPEXP virtual VColorRef GetClearColor() const { return VColorRef(255,255,255,255); }
  EFFECTS_IMPEXP virtual int GetUsageHint() const { return VIS_CONTEXTUSAGE_DEPTHSHADOW; }
  EFFECTS_IMPEXP virtual void PostProcess() {}

  inline VCompiledTechnique* GetOpaqueFillTechnique(bool bDoubleSided = false) const { return bDoubleSided ? m_spOpaqueFillTechniqueDoubleSided : m_spOpaqueFillTechnique; }
  inline VCompiledTechnique* GetAlphaFillTechnique(bool bDoubleSided = false) const { return bDoubleSided ? m_spAlphatestFillTechniqueDoubleSided : m_spAlphatestFillTechnique; }
  inline VCompiledTechnique* GetTerrainFillTechnique() const { return m_spTerrainFillTechnique; }

protected:
  VShadowMapGenerator* m_pGenerator;
  VCompiledTechniquePtr m_spTerrainFillTechnique;
  VCompiledTechniquePtr m_spOpaqueFillTechnique;
  VCompiledTechniquePtr m_spAlphatestFillTechnique;
  VCompiledTechniquePtr m_spOpaqueFillTechniqueDoubleSided;
  VCompiledTechniquePtr m_spAlphatestFillTechniqueDoubleSided;
};

typedef VSmartPtr<IVShadowMapFormat> IVShadowMapFormatPtr;

class IVShadowMapComponent;

/// \brief
///   Base class for a shadow map generator. Used internally by the shadow map component classes.
///
/// Typically, you do not have to access the methods exposed by this class directly. Instead, you should go through
/// the functionality provided by the shadow map components (IVShadowMapComponents and its subclasses).
class VShadowMapGenerator : public VRefCounter, public IVisCallbackHandler_cl
{
  friend class VShadowMapRenderLoop;
public:

  ///
  /// @name Initialization, Construction and Destruction
  /// @{
  ///

  /// \brief
  ///   Constructor
  ///
  /// \param pRendererNode
  ///   The renderer node that this generator is associated with
  ///
  /// \param pRefLight
  ///   The shadow casting light source
  ///
  /// \param pShadowMapFormatProvider
  ///   Shadow map format
  ///
  /// \param pComponent
  ///   The shadow map component that this generator is associated with
  ///
  /// \param iNumCascades
  ///   Number of cascades
  ///
  /// \param iMaxNumCascades
  ///   Maximum number of cascades that can be used
  EFFECTS_IMPEXP VShadowMapGenerator(IVRendererNode *pRendererNode, VisLightSource_cl *pRefLight, 
    IVShadowMapFormat* pShadowMapFormatProvider, IVShadowMapComponent* pComponent, int iNumCascades, int iMaxNumCascades);

  /// \brief
  ///   Destructor
  EFFECTS_IMPEXP virtual ~VShadowMapGenerator();

  /// \brief
  ///   Internal use only. Initializes the shadow map generator.
  EFFECTS_IMPEXP virtual bool Initialize();

  /// \brief
  ///   Internal use only. De-initializes the shadow map generator.
  EFFECTS_IMPEXP virtual void DeInitialize();

  ///
  /// @}
  ///

  ///
  /// @name Update
  /// @{
  ///

  /// \brief
  ///   Overridden from base class.
  EFFECTS_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData);

  /// \brief
  ///   Tick function (called once per frame). Internal use only.
  EFFECTS_IMPEXP virtual void Update(bool force = false) = 0;

  /// \brief
  ///   Update the passed shader with the light's properties. Internal use only.
  EFFECTS_IMPEXP virtual void UpdateLightShader(VShadowMapApplyShaderPass* pPass, hkvMat4 (&mLightProjection)[4], hkvVec4& vCascadeScale);

  /// \brief
  ///   Computes the neccessary transformations and cascade scales
  EFFECTS_IMPEXP virtual void ComputeCascadeParams(hkvMat4 (&mLightProjection)[4], hkvVec4& vCascadeScale, hkvVec4& vCascadeEnd);

  /// \brief
  ///   Updates the default depth fill techniques with cascade-specific information.
  EFFECTS_IMPEXP void UpdateDepthFillTechniques(int iCascade);

  ///
  /// @}
  ///

  ///
  /// @name Properties
  /// @{
  ///

  /// \brief
  ///   Associate this shadow map generator with a light source
  EFFECTS_IMPEXP void SetLightSource(VisLightSource_cl *pLight);
 
  /// \brief
  ///   Returns the current light source
  inline const VisLightSource_cl *GetLightSource() const 
  { 
    return m_pLightSource; 
  }

  /// \brief
  ///   Returns the current shadow map resolution (per cascade)
  EFFECTS_IMPEXP int GetShadowMapSize() const; 
  
  ///   Sets the number of cascades
  EFFECTS_IMPEXP void SetCascadeCount(int iNumParts);

  /// \brief
  ///   Returns the number of cascades
  inline int GetCascadeCount() const 
  { 
    return m_iNumParts; 
  }

  /// \brief
  ///   Retrieve information for the passed cascade
  inline VShadowMapPart& GetCascadeInfo(int iCascade) 
  { 
    VASSERT(iCascade >= 0 && iCascade < MAX_SHADOW_PARTS_COUNT);
    return m_pParts[iCascade]; 
  }

  /// \brief
  ///   Returns the shadow projection type (orthographic vs. projected)
  inline VShadowProjectionType_e GetProjectionType() const 
  { 
    return m_eProjectionType; 
  }

  /// \brief
  ///   Sets supported geometry types, where each bit is a type in defined in VShadowCastingGeometry_e
  EFFECTS_IMPEXP void SetGeometryTypes(int iFlags);

  /// \brief
  ///   Enables or disables rendering
  EFFECTS_IMPEXP void SetUpdateEnabled(bool bStatus);

  /// \brief
  ///   Returns whether update is enabled
  inline bool GetUpdateEnabled() const { return m_bUpdateEnabled; }

  /// \brief
  ///   Sets the render filter bitmask to filter relevant scene elements for shadow casting
  EFFECTS_IMPEXP void SetFilterBitmask(int iMask);
  
  /// \brief
  ///   Returns the cascade index (or -1) that uses the passed render context
  EFFECTS_IMPEXP int GetCascadeIndexFromRenderContext(VisRenderContext_cl *pContext) const;

  /// \brief
  ///   Gets the actual render target texture size (which is a multiple of the cascade resolution)
  inline void GetTextureSize(int &iWidth, int &iHeight) const
  {
    iWidth = m_iTextureSize[0];
    iHeight = m_iTextureSize[1];
  }

  /// \brief
  ///   Return the position of the light
  inline const hkvVec3 GetPosition() const 
  { 
    return m_pLightSource->GetPosition(); 
  }

  /// \brief
  ///   Return the direction of the light
  inline hkvVec3 GetDirection() const 
  { 
    return m_pLightSource->GetDirection(); 
  }

  /// \brief
  ///   Returns a render frustum which is updated from the render context in the Update function
  inline const VisFrustum_cl* GetMainFrustum() const 
  { 
    return &m_MainViewFrustum; 
  }

  /// \brief
  ///   Returns the renderer node that this generator is associated with
  inline IVRendererNode *GetRendererNode() 
  { 
    return m_pRendererNode; 
  }

  /// \brief
  ///   Returns the shadow map component that this generator is associated with
  inline IVShadowMapComponent* GetShadowMapComponent() { return m_pShadowComponent; }

  /// \brief
  ///   Returns the shadow map texture that contains all cascades
  inline VTextureObject* GetShadowMap() const 
  {
#if defined( _VR_DX11 )
    if (VVideo::GetDXFeatureLevel() < D3D_FEATURE_LEVEL_10_0 && m_spShadowMap.GetPtr() != NULL)
      return m_spShadowMap;
    else
      return m_spShadowMapDepthStencil;
#elif defined(WIN32) && defined(_VR_DX9)
      return m_spShadowMap->GetTextureFormat() != VTextureLoader::NONE ? m_spShadowMap : m_spShadowMapDepthStencil; 
#else
    return m_spShadowMapDepthStencil;
#endif
  }

#ifdef _VISION_PS3
  /// \brief
  ///   Returns the patched shadow map texture
  inline VTextureObject* GetPatchedShadowMap() const 
  { 
    return m_spPatchedShadowMapDepthStencil;
  }
#endif

  /// \brief
  ///   Returns the depth stencil version of the shadow map texture that contains all cascades
  inline VisRenderableTexture_cl *GetShadowMapDepthStencil() const 
  { 
    return m_spShadowMapDepthStencil; 
  }

  /// \brief
  ///   Access rendering properties for a cascade
  EFFECTS_IMPEXP void GetDepthFillShaderConstantValues(int iCascade, float &fDepthBias, float &fSlopScaleDepthBias, hkvVec3& vClipPlanes);

  /// \brief
  ///   If enabled (default) the shadow map will only consider meshes that have the "CastShadows" flag set, otherwise that flag is ignored
  inline void SetConsiderCastShadowFlag(bool bStatus)
  {
    m_bConsiderCastShadowFlag = bStatus;
  }

  /// \brief
  ///   Returns the status that has been set via SetConsiderCastShadowFlag
  inline bool GetConsiderCastShadowFlag() const
  {
    return m_bConsiderCastShadowFlag;
  }

  /// \brief
  ///   Returns whether the shadow map is rendered interleaved or at the beginning of the frame
  inline bool IsRenderedInterleaved() const
  {
    return m_bIsRenderedInterleaved;
  }

  ///
  /// @}
  ///

  ///
  /// @name Debugging and Profiling
  /// @{
  ///

  /// \brief
  ///   Enable a screen overlay that displays the shadow map result
  EFFECTS_IMPEXP void SetupDebugScreenMask();

  /// \brief
  ///   Enables or disables light volume rendering
  inline void SetRenderShadowVolumes(bool bStatus) 
  { 
    m_bRenderShadowVolumes = bStatus; 
  }

  /// \brief
  ///   Indicates whether light volume rendering is enabled
  inline bool GetRenderShadowVolumes() const
  { 
    return m_bRenderShadowVolumes; 
  }

  /// \brief
  ///   Enables or disables profiling data text overlay
  inline void SetShowProfilingData(bool bStatus) 
  { 
    m_bShowProfilingData = bStatus; 
  }

  /// \brief
  ///   Indicates whether profiling data text is shown
  inline bool GetShowProfilingData() const
  {
    return m_bShowProfilingData; 
  }

  ///
  /// @}
  ///

  ///
  /// @name Render Status
  /// @{
  ///

  /// \brief
  ///   Used for internal state handling
  inline void SetCascadeRendered(int iCascadeIndex) 
  { 
    VASSERT(iCascadeIndex < MAX_SHADOW_PARTS_COUNT); 
    m_iPartsRendered |= (1 << iCascadeIndex); 
  }

  /// \brief
  ///   Used for internal state handling
  EFFECTS_IMPEXP bool AreAllCascadeRendered() const;

  /// \brief
  ///   Used for internal state handling
  inline bool AreNoCascadeRendered() const 
  { 
    return m_iPartsRendered == 0; 
  }

  ///
  /// @}
  ///

  ///
  /// @name Callbacks
  /// @{
  ///

  /// \brief
  ///   Static callback that can be used to hook into shadow map rendering, e.g. to render custom geometry.
  ///
  /// The purpose of this callback is to provide a mechanism for custom geometry to be taken into account when
  /// rendering the shadow map. For instance, the SpeedTree plugin registers an OnRenderShadowMap callback and
  /// uses the callback to render its custom geometry into the shadow map.
  ///
  /// The data object can be cast to VShadowRendererDataObject which provides information about the shadow map
  /// and the cascade being rendered. 
  EFFECTS_IMPEXP static VCallback OnRenderShadowMap;

  ///
  /// @}
  ///

  // Internal:
  EFFECTS_IMPEXP void UpdateFacing(VCompiledTechnique *pTech, bool bStatus);
  EFFECTS_IMPEXP void SetFrontFacesCastShadows(bool bStatus);
  inline const hkvVec3 GetCascadeLightPosition(int iCascadeIndex) const { return m_pParts[iCascadeIndex].GetCamera()->GetPosition(); }
  VShadowProfilingData_t m_ShadowProfilingData;
  inline VCompiledTechnique* GetOpaqueFillTechnique(bool bDoubleSided = false) const { return m_spShadowMapFormat->GetOpaqueFillTechnique(bDoubleSided); }
  inline VCompiledTechnique* GetAlphaFillTechnique(bool bDoubleSided = false) const { return m_spShadowMapFormat->GetAlphaFillTechnique(bDoubleSided); }
  inline VCompiledTechnique* GetTerrainFillTechnique() const { return m_spShadowMapFormat->GetTerrainFillTechnique(); }
  inline void PostProcess() { m_spShadowMapFormat->PostProcess(); m_iPartsRendered = 0; }
  EFFECTS_IMPEXP virtual void PreProcess();
  inline IVShadowMapFormat* GetFormatProvider() const { return m_spShadowMapFormat.GetPtr(); }
  inline VCompiledTechnique* GetClearTechnique() const { return m_spClearTechnique; }
  inline VisEntityCollection_cl& GetShadowCasterEntities() { return m_ShadowCasterEntities; }

  static int PROFILING_RENDER_SHADOWMAP;
  static int PROFILING_POSTPROCESS_SHADOWMAP;
  static int PROFILING_SORT_SCENEELEMENTS;
  static int PROFILING_POSTFILER_SCENEELEMENTS;

protected:
  void UpdateLightSourceProperties();
  void RenderProfilingData();
  void DrawTextHelper(IVRenderInterface *pRi, float x, float y, const char* msg, ...);
  void InitProfiling();

  // Properties:
  VisLightSource_cl *m_pLightSource;
  VShadowProjectionType_e m_eProjectionType;      // This is set automatically from the light settings
  
  int m_iNumParts;
  IVRendererNode *m_pRendererNode;

  IVShadowMapFormatPtr m_spShadowMapFormat;

  IVShadowMapComponent* m_pShadowComponent;

  int m_iTextureSize[2];

#if defined(WIN32)
  VisRenderableTexturePtr m_spShadowMap;
#endif
  VisRenderableTexturePtr m_spShadowMapDepthStencil;

  VisEntityCollection_cl m_ShadowCasterEntities;


#if defined (_VISION_PS3)
  VTextureObjectPtr m_spPatchedShadowMapDepthStencil;
#endif

  VCompiledTechniquePtr m_spClearTechnique;
 
  VShadowMapPart *m_pParts;

  VisFrustum_cl m_MainViewFrustum;

  // for debugging: Show the depth texture as overlay
  VisScreenMaskPtr m_spDebugMask;

  bool  m_bUpdateEnabled;
  int   m_iPartsRendered;

  bool  m_bIsRenderedInterleaved;

  bool  m_bIsInitialized;
  bool  m_bPropertiesDirty;
  bool  m_bRenderShadowVolumes;
  bool  m_bShowProfilingData;
  
  bool  m_bConsiderCastShadowFlag;
  hkvVec3 m_vLastLightPos;
  float m_fLastRadius;
};


/// \brief
///   Implements IVShadowMapFormat
///
/// To be used for shadow map generators which exclusively create a depth-stencil texture for the shadow map.
/// The actual texture format may be platform-specific.
class VShadowMapFormatDepthOnly : public IVShadowMapFormat
{
public:
  EFFECTS_IMPEXP VShadowMapFormatDepthOnly();
  EFFECTS_IMPEXP virtual ~VShadowMapFormatDepthOnly();

  EFFECTS_IMPEXP virtual void Initialize(VShadowMapGenerator* pGenerator);

protected:
  EFFECTS_IMPEXP virtual VTextureLoader::VTextureFormat_e GetShadowMapFormat();
  EFFECTS_IMPEXP virtual VTextureLoader::VTextureFormat_e GetShadowMapFormatDepthStencil();
  EFFECTS_IMPEXP virtual int GetUsageHint() const;
  EFFECTS_IMPEXP virtual void PostProcess();
};


/// \brief
///   Shader class for rendering geometry into the shadow map.
///
/// All shadow map fill shaders (custom or default) have to use this class, since the shadow map generator uses
/// it to provide cascade-specific properties to the shader (depth bias, clip planes).
class VShadowMapFillShaderPass : public VCompiledShaderPass
{
public:

  inline void SetDepthBias(float fBias, float fSlopeScale, bool bOrthographic)
  {
    float fVals[4] = { fBias, fSlopeScale, 0.0f, bOrthographic ? 1.0f : 0.0f };
    m_RegDepthBias.SetRegisterValueSafeF(this, fVals);
    if (GetRenderState ()->GetRasterizerState ().m_fSlopeScaledDepthBias != fSlopeScale)
    {
      GetRenderState ()->GetRasterizerState ().m_fDepthBias = 0.0f;
      GetRenderState ()->GetRasterizerState ().m_fDepthBiasClamp = 1024.0f;
      GetRenderState ()->GetRasterizerState ().m_fSlopeScaledDepthBias = fSlopeScale;
      GetRenderState ()->GetRasterizerState ().ComputeHash();
    }
  }

  inline void SetClipPlanes(float fNear, float fFar, float fTotalRange)
  {
    float fVals[4] = { fNear, fFar, fTotalRange, 0.0f };
    m_RegClipPlanes.SetRegisterValueSafeF(this, fVals);
  }

  V_DECLARE_SERIAL_DLLEXP( VShadowMapFillShaderPass, EFFECTS_IMPEXP )
  EFFECTS_IMPEXP virtual void PostCompileFunction(VShaderEffectResource *pSourceFX, VShaderPassResource *pSourceShader);

private:

  VConstantBufferRegister m_RegDepthBias;
  VConstantBufferRegister m_RegClipPlanes;
};


/// \brief
///   Shader class for rendering shadow-receiving geometry for static lights in forward rendering mode.
class VShadowMapApplyShaderPass : public VCompiledShaderPass
{
public:
 
  inline void SetTextureSize(int iWidth, int iHeight)
  {
    float fVals[4] = { 1.0f/(float)iWidth, 1.0f/(float)iHeight, 0.0f, 0.0f };
    m_RegInvTexSize.SetRegisterValueSafeF(this, fVals);
  }

  inline void SetLightPosition(const hkvVec3& vLightPos, const hkvMat3& mWorldRotation)
  {
    hkvVec3 eyedir = mWorldRotation.transformDirection(vLightPos);

    // TODO (Jan): This should be USES_OPENGL_COORDINATE_SYSTEM in all cases. For PSP2, there is currently a shader hack in place
    // to map it to DirectX behavior -> refactor this!
    #if defined(_VISION_PSP2)
      eyedir[2] = -eyedir[2];
    #endif

    m_RegLightWorldSpacePos.SetRegisterValueSafeF(this, vLightPos.getAsPosition ().data);
    m_RegLightEyeSpacePos.SetRegisterValueSafeF(this, eyedir.getAsDirection ().data);
  }

  inline void SetLightDirection(const hkvVec3& lightDir)
  {
    m_RegLightDirection.SetRegisterValueSafeF(this, lightDir.getAsDirection ().data);
  }

  inline void SetShadowMap(VTextureObject *pShadowMap)
  {
    VASSERT(pShadowMap != NULL);
    m_RegShadowMap.SetSamplerTextureSafe(this, pShadowMap);
  }

  inline void SetCascadeParameters(const hkvVec4& vCascadeEnd, const hkvVec4& vCascadeScale)
  {
    m_RegCascadeSplitsEnd.SetRegisterValueSafeF(this, vCascadeEnd.data);
    m_RegCascadeScale.SetRegisterValueSafeF(this, vCascadeScale.data);
  }

  inline void SetLightProjection(int iNumCascades, const hkvMat4* lightProjection)
  {
    if (m_RegLightProjection.IsValid())
    {
      m_RegLightProjection.SetRegisterValuesMatrix(this, lightProjection, iNumCascades);
    }
  }

  inline void SetFadeoutParameters(float fFadeoutStart, float fFadeoutEnd)
  {
    float pParams[4] = { fFadeoutEnd, 1.0f / (fFadeoutEnd - fFadeoutStart), 0.0f, 0.0f };
    m_RegFadeoutParams.SetRegisterValueSafeF(this, pParams);
  }

  V_DECLARE_DYNCREATE_DLLEXP( VShadowMapApplyShaderPass, EFFECTS_IMPEXP );
  EFFECTS_IMPEXP virtual void PostCompileFunction(VShaderEffectResource *pSourceFX, VShaderPassResource *pSourceShader);

private:

  VConstantBufferRegister m_RegInvTexSize;
  VConstantBufferRegister m_RegLightWorldSpacePos;
  VConstantBufferRegister m_RegLightEyeSpacePos;
  VConstantBufferRegister m_RegLightDirection;

  VConstantBufferRegister m_RegCascadeScale;
  VConstantBufferRegister m_RegCascadeSplitsEnd;
  VConstantBufferRegister m_RegLightProjection;

  VConstantBufferRegister m_RegFadeoutParams;

  VConstantBufferRegister m_RegShadowMap;
};



/// \brief
///   Shadowmap renderloop class
class VShadowMapRenderLoop : public IVisRenderLoop_cl
{
public:
  EFFECTS_IMPEXP VShadowMapRenderLoop(VShadowMapGenerator *pShadowMapGenerator);
  EFFECTS_IMPEXP virtual void OnDoRenderLoop(void *pUserData);

protected:
  VisEntityCollection_cl *PostFilterByViewFrustum(int iCascadeIndex, const VisEntityCollection_cl *pInputCollection);
  VisStaticGeometryInstanceCollection_cl *PostFilterByViewFrustum(int iCascadeIndex, const VisStaticGeometryInstanceCollection_cl *pInputCollection);

  void SplitByRenderState(const VisEntityCollection_cl *pEntities);
  void SplitByRenderState(const VisStaticGeometryInstanceCollection_cl *pStaticGI);
  static int SortEntityRenderState( const void *elem1, const void *elem2);

  VisStaticGeometryInstanceCollection_cl m_GICollection;
  VisEntityCollection_cl m_EntityCollection;

  VisStaticGeometryInstanceCollection_cl m_AlphaGICollection;
  VisStaticGeometryInstanceCollection_cl m_OpaqueGICollection;
  VisStaticGeometryInstanceCollection_cl m_AlphaGICollectionDoubleSided;
  VisStaticGeometryInstanceCollection_cl m_OpaqueGICollectionDoubleSided;
  VisStaticGeometryInstanceCollection_cl m_TerrainGICollection;
  VisStaticGeometryInstanceCollection_cl m_SurfaceSpecificGICollection;

  VisEntityCollection_cl m_MixedEntityCollection;
  VisEntityCollection_cl m_AlphaEntityCollection;
  VisEntityCollection_cl m_OpaqueEntityCollection;

  VShadowMapGenerator *m_pGenerator;

private:
  hkvAlignedBBox m_ShadowCasterBBox; // axis-aligned bounding box that encloses all shadow casting geometry

public:
  // VTypedObject
  V_DECLARE_DYNAMIC_DLLEXP(VShadowMapRenderLoop, EFFECTS_IMPEXP)
};


/// \brief
///   Shadowmap visibility collector class
class VShadowmapVisibilityCollector : public VisionVisibilityCollector_cl
{
public:
  VShadowMapGenerator *m_pSMGenerator;
  V_DECLARE_DYNAMIC_DLLEXP(VShadowmapVisibilityCollector, EFFECTS_IMPEXP)
};


#endif  // VSHADOWMAPGENERATORBASE_HPP_INCLUDED

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
