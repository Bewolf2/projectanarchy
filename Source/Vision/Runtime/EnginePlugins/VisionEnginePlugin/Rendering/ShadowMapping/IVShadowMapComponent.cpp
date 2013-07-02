/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/IVShadowMapComponent.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/MobileForwardRenderer/VMobileForwardRenderer.hpp>

V_IMPLEMENT_SERIAL_ABSTRACT(IVShadowMapComponent, IVObjectComponent, 0, &g_VisionEngineModule);


IVShadowMapComponent::IVShadowMapComponent() :
  Enabled(TRUE), UseQuarterSizeShadowTexture(FALSE), ShadowMappingMode(SHADOW_MAPPING_MODE_PCF4),
  ShadowMapSize(512), 
  SampleRadius(2.0f),
  SampleRadiusScaleWithDistance(0.0f),
  MaxDistanceToBlocker(1000.0f),
  UseSurfaceSpecificShadowShaders(FALSE), 
  FilterBitmask(65535), 
  NearClip(10.0f), 
  AmbientColor(128,128,128),
  ShadowBoxExtrudeMultiplier(1.5f),
  GeometryTypes(SHADOW_CASTER_STATICMESHES|SHADOW_CASTER_ENTITIES|SHADOW_CASTER_SPEEDTREES), 
  FrontFacingShadows(FALSE),
  m_pLightSource(NULL), 
  m_pRendererNode(NULL), m_iRendererNodeIndex(0), 
  m_pLightVolumeMeshBuffer(NULL),
  m_bIsInitialized(false)
{
  Bias[0] = 2.0f;
  Bias[1] = 6.0f;
  Bias[2] = 18.0f;
  Bias[3] = 54.0f;
  SlopeScaled[0] = SlopeScaled[1] = SlopeScaled[2] = SlopeScaled[3] = 0.0f;

  Vision::Callbacks.OnVisibilityPerformed += this;
  Vision::Callbacks.OnRendererNodeChanged += this;
  Vision::Callbacks.OnVideoChanged        += this;
  Vision::Callbacks.OnReassignShaders     += this;
  Vision::Callbacks.OnReferenceContextChanged += this;
#if defined (HK_DEBUG_SLOW)
  Vision::Callbacks.OnUpdateSceneBegin += this;
#endif
}

IVShadowMapComponent::IVShadowMapComponent(int iRendererNodeIndex) :
  Enabled(TRUE), UseQuarterSizeShadowTexture(FALSE), ShadowMappingMode(SHADOW_MAPPING_MODE_PCF4),
  ShadowMapSize(512),
  SampleRadius(2.0f),
  SampleRadiusScaleWithDistance(0.0f),
  MaxDistanceToBlocker(1000.0f),
  UseSurfaceSpecificShadowShaders(FALSE),
  FilterBitmask(65535),
  NearClip(10.0f),
  AmbientColor(128,128,128),
  ShadowBoxExtrudeMultiplier(1.5f),
  GeometryTypes(SHADOW_CASTER_STATICMESHES|SHADOW_CASTER_ENTITIES|SHADOW_CASTER_SPEEDTREES),
  FrontFacingShadows(FALSE),
  m_pLightSource(NULL),
  m_pLightVolumeMeshBuffer(NULL),
  m_bIsInitialized(false)
{
  m_iRendererNodeIndex = iRendererNodeIndex;

  Bias[0] = 2.0f;
  Bias[1] = 6.0f;
  Bias[2] = 18.0f;
  Bias[3] = 54.0f;
  SlopeScaled[0] = SlopeScaled[1] = SlopeScaled[2] = SlopeScaled[3] = 0.0f;

  Vision::Callbacks.OnVisibilityPerformed += this;
  Vision::Callbacks.OnRendererNodeChanged += this;
  Vision::Callbacks.OnVideoChanged        += this;
  Vision::Callbacks.OnReassignShaders     += this;
  Vision::Callbacks.OnReferenceContextChanged += this;
#if defined (HK_DEBUG_SLOW)
  Vision::Callbacks.OnUpdateSceneBegin += this;
#endif
}

IVShadowMapComponent::~IVShadowMapComponent()
{
  Vision::Callbacks.OnVisibilityPerformed -= this;
  Vision::Callbacks.OnRendererNodeChanged -= this;
  Vision::Callbacks.OnVideoChanged        -= this;
  Vision::Callbacks.OnReassignShaders     -= this;
  Vision::Callbacks.OnReferenceContextChanged -= this;
#if defined (HK_DEBUG_SLOW)
  Vision::Callbacks.OnUpdateSceneBegin -= this;
#endif

  m_pRendererNode = NULL;
  DeInitializeRenderer();
}

bool IVShadowMapComponent::InitializeRenderer()
{ 
  if (m_bIsInitialized)
    return true;

  if (!Enabled)
    return false;

  if (m_pLightSource == NULL)
    return false;

  m_pRendererNode = vdynamic_cast<VRendererNodeCommon*>(Vision::Renderer.GetRendererNode(m_iRendererNodeIndex));
  if (m_pRendererNode == NULL)
  {
    Vision::Error.Warning("No VRendererNodeCommon set. Shadows will not work.");
    return false;
  }

  if (ShadowMappingMode > SHADOW_MAPPING_MODE_LAST_VALID)
  {
    if (m_pLightSource->GetType() == VIS_LIGHT_DIRECTED)
    {
      ShadowMappingMode = SHADOW_MAPPING_MODE_DEBUG_SHOW_CASCADES;
    }
    else
    {
      Vision::Error.Warning("Debug mode only works with directional lights, reverting to 'SHADOW_MAPPING_MODE_PCF4'.");
      ShadowMappingMode = SHADOW_MAPPING_MODE_PCF4;
    }
  }

  //clamp near clip value to avoid crash when reloading the scene and nearclip was set to 0 [#3111]
  NearClip = ClampNearClip(NearClip);

  return true;
}

void IVShadowMapComponent::DeInitializeRenderer()
{
  if (!m_bIsInitialized)
    return;

  m_spShadowMapGenerator = NULL;
  m_pRendererNode = NULL;

  m_bIsInitialized = false;
}

IVShadowMapFormat* IVShadowMapComponent::GetShadowMapFormat() const 
{
  if ((ShadowMappingMode >= SHADOW_MAPPING_MODE_PCF4 && ShadowMappingMode <= SHADOW_MAPPING_MODE_CHS) ||
    ShadowMappingMode == SHADOW_MAPPING_MODE_DEBUG_SHOW_CASCADES)
  {
    return new VShadowMapFormatDepthOnly();
  }
  else
  {
    VASSERT(FALSE && "Unsupported shadow generator type requested!");
    return NULL;
  }
}

void IVShadowMapComponent::SetOwner(VisTypedEngineObject_cl *pOwner)
{
  VASSERT(pOwner == NULL || pOwner->IsOfType(V_RUNTIME_CLASS(VisLightSource_cl)));

  if (m_bIsInitialized && pOwner != m_pOwner)
  {
    DeInitializeRenderer();
  }

  IVObjectComponent::SetOwner(pOwner);

  if (pOwner != NULL)
  {
    m_pLightSource = (VisLightSource_cl *)pOwner;
    InitializeRenderer();

    #ifdef SHOW_SHADOW_MAP
      GetShadowMapGenerator()->SetupDebugScreenMask();
    #endif
  }
}

BOOL IVShadowMapComponent::CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
  if (!IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut))
    return FALSE;

  if (!pObject->IsOfType(V_RUNTIME_CLASS(VisLightSource_cl)))
  {
    sErrorMsgOut = "VShadowMapComponent can only be attached to objects of type VisLightSource_cl!";
    return FALSE;
  }

  return TRUE;
}

void IVShadowMapComponent::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (m_pLightSource == NULL)
    return;

  if (pData->m_pSender == &Vision::Callbacks.OnVideoChanged || pData->m_pSender == &Vision::Callbacks.OnReassignShaders ||
    pData->m_pSender == &Vision::Callbacks.OnReferenceContextChanged)
  {
    DeInitializeRenderer();
    InitializeRenderer();
  }
  else if (pData->m_pSender == &Vision::Callbacks.OnRendererNodeChanged)
  {
    VisRendererNodeChangedDataObject_cl *pRendererInfo = (VisRendererNodeChangedDataObject_cl *)pData;

    IVRendererNode* pOldNode = pRendererInfo->m_spRemovedNode.GetPtr();

    if (m_pRendererNode == pOldNode || m_pRendererNode == NULL)
    {
      DeInitializeRenderer();

      m_pRendererNode = vdynamic_cast<VRendererNodeCommon*>(Vision::Renderer.GetRendererNode(m_iRendererNodeIndex));

      if (m_pRendererNode != NULL)
      {
        InitializeRenderer();
      }
    }
  }

#if defined (HK_DEBUG_SLOW)
  else if (pData->m_pSender == &Vision::Callbacks.OnUpdateSceneBegin && m_bIsInitialized && !Vision::Editor.IsInEditor())
  {
    VString errOut;
    VASSERT(CanAttachToObject(m_pLightSource, errOut));
  }
#endif
}

bool IVShadowMapComponent::SetVariable(const char *szName, const char *szValue)
{
  IVObjectComponent::SetVariable(szName, szValue);

  if (!strcmp(szName, "Enabled"))
    SetEnabled(Enabled); 
  else
  {
    if (m_bIsInitialized)
    {
      if (!strcmp(szName, "ShadowMapSize"))
        SetShadowMapSize(ShadowMapSize);
      else if (!strncmp(szName, "Bias", 4))
        SetBias(Bias, 4);
      else if (!strncmp(szName, "SlopeScaled", 11))
        SetSlopeScaled(SlopeScaled, 4);  
      else if (!strcmp(szName, "FilterBitmask"))
        SetFilterBitmask(FilterBitmask);
      else if (!strcmp(szName, "GeometryTypes"))
        SetGeometryTypes(GeometryTypes);
      else if (!strcmp(szName, "UseQuarterSizeShadowTexture"))
        SetUseQuarterSizeShadowTexture(UseQuarterSizeShadowTexture);  
      else if (!strcmp(szName, "ShadowMappingMode"))
        SetShadowMappingMode(ShadowMappingMode);  
      else if (!strcmp(szName, "SampleRadius"))
        SetSampleRadius(SampleRadius);
      else if (!strcmp(szName, "UseSurfaceSpecificShadowShaders"))
        SetUseSurfaceSpecificShadowShaders(UseSurfaceSpecificShadowShaders);
      else if (!strcmp(szName, "NearClip"))
        SetNearClip(NearClip);
      else if (!strcmp(szName, "FrontFacingShadows"))
        SetFrontFacingShadows(FrontFacingShadows);
    }
  }

  return true;
}

#if defined(WIN32) || defined(_VISION_DOC)

void IVShadowMapComponent::GetVariableAttributes(VisVariable_cl* pVariable, VVariableAttributeInfo& destInfo)
{
  if (!strcmp(pVariable->GetName(), "Enabled"))
  {
    VString dummy;
    if (!CanAttachToObject(m_pOwner, dummy))
      destInfo.m_bReadOnly = true;
  }
  if (m_pLightSource->IsDynamic())
  {
    if (!strcmp(pVariable->GetName(), "AmbientColor") || 
      !strcmp(pVariable->GetName(), "ShadowBoxExtrudeMultiplier"))
      destInfo.m_bHidden = true;
  }
  if (ShadowMappingMode != SHADOW_MAPPING_MODE_PCSS16 && ShadowMappingMode != SHADOW_MAPPING_MODE_PCSS16_RANDOMIZED)
  {
    if (!strcmp(pVariable->GetName(), "MaxDistanceToBlocker"))
      destInfo.m_bHidden = true;
  }
}

#endif

void IVShadowMapComponent::Serialize(VArchive &ar)
{
  m_iLocalVersion = SHADOWMAP_COMPONENT_CURRENT_VERSION;
  IVObjectComponent::Serialize(ar);

  if (ar.IsLoading())
  {
    int val32;
    ar >> m_iLocalVersion; 
    VASSERT(m_iLocalVersion <= SHADOWMAP_COMPONENT_CURRENT_VERSION);

    if (m_iLocalVersion > SHADOWMAP_COMPONENT_VERSION_0)
    {
      ar >> Enabled;
      if (m_iLocalVersion>=SHADOWMAP_COMPONENT_VERSION_2)
        ar >> GeometryTypes;
      ar >> UseQuarterSizeShadowTexture;
      ar >> val32;
      ShadowMappingMode = (VShadowMappingMode_e)val32;
    #ifndef _VR_DX11
      // see IVShadowMapComponent::SetShadowMappingMode
      if (ShadowMappingMode == SHADOW_MAPPING_MODE_CHS)
        ShadowMappingMode = SHADOW_MAPPING_MODE_PCSS16;
    #endif
      if (m_iLocalVersion < SHADOWMAP_COMPONENT_VERSION_5)
      {
        unsigned int uiTmp;
        ar >> uiTmp;
      }
      ar >> ShadowMapSize;
      ar >> SampleRadius;
      if (m_iLocalVersion >= SHADOWMAP_COMPONENT_VERSION_6)
      {
        ar >> SampleRadiusScaleWithDistance;
      }

      if (m_iLocalVersion >= SHADOWMAP_COMPONENT_VERSION_7)
      {
        ar >> MaxDistanceToBlocker;
      }

      ar >> UseSurfaceSpecificShadowShaders;
      if (m_iLocalVersion < SHADOWMAP_COMPONENT_VERSION_5)
      {
        BOOL bTmp;
        ar >> bTmp;
      }
      ar >> Bias[0];
      ar >> Bias[1];
      ar >> Bias[2];
      ar >> Bias[3];
      ar >> SlopeScaled[0];      
      ar >> SlopeScaled[1];      
      ar >> SlopeScaled[2];    
      ar >> SlopeScaled[3];
      ar >> FilterBitmask;
      ar >> NearClip;
      ar >> AmbientColor;
      ar >> ShadowBoxExtrudeMultiplier;

      if (m_iLocalVersion>=SHADOWMAP_COMPONENT_VERSION_3)
        ar >> FrontFacingShadows;
    }
  }
  else
  {
    ar << m_iLocalVersion;
    ar << Enabled;
    ar << GeometryTypes; // version 2
    ar << UseQuarterSizeShadowTexture;
    ar << (int)ShadowMappingMode;
    ar << ShadowMapSize;
    ar << SampleRadius;
    ar << SampleRadiusScaleWithDistance;
    ar << MaxDistanceToBlocker;
    ar << UseSurfaceSpecificShadowShaders;
    ar << Bias[0];
    ar << Bias[1];
    ar << Bias[2];
    ar << Bias[3];
    ar << SlopeScaled[0];
    ar << SlopeScaled[1];    
    ar << SlopeScaled[2];    
    ar << SlopeScaled[3];
    ar << FilterBitmask;
    ar << NearClip;
    ar << AmbientColor;
    ar << ShadowBoxExtrudeMultiplier;
    ar << FrontFacingShadows;
  }
}

void IVShadowMapComponent::SetFrontFacingShadows(BOOL bFrontFacingShadows)
{
  FrontFacingShadows = bFrontFacingShadows;
  if (m_bIsInitialized)
    m_spShadowMapGenerator->SetFrontFacesCastShadows(FrontFacingShadows==TRUE ? true : false);
}


void IVShadowMapComponent::SetUseQuarterSizeShadowTexture(BOOL use)
{
  UseQuarterSizeShadowTexture = use;
  if (m_bIsInitialized)
  {
    DeInitializeRenderer();
    InitializeRenderer();
  }
}

void IVShadowMapComponent::SetShadowMappingMode(VShadowMappingMode_e mode)
{
  if (mode > SHADOW_MAPPING_MODE_LAST_VALID)
  {
    mode = SHADOW_MAPPING_MODE_DEBUG_SHOW_CASCADES;
  }

#ifndef _VR_DX11
  if (mode == SHADOW_MAPPING_MODE_CHS)
  {
    Vision::Error.Warning("Contact hardening shadows (CHS) are DX11 only.");
    mode = SHADOW_MAPPING_MODE_PCSS16;
  }
#else
  if ( mode == SHADOW_MAPPING_MODE_CHS && VVideo::GetDXFeatureLevel() < D3D_FEATURE_LEVEL_10_0 )
  {
    Vision::Error.Warning( "Contact hardening shadows (CHS) are not supported for 9.3 downlevel DX11." );
    mode = SHADOW_MAPPING_MODE_PCSS16;
  }
#endif
  ShadowMappingMode = mode;
  if (m_bIsInitialized)
  {
    DeInitializeRenderer();
    InitializeRenderer();
  }
}


void IVShadowMapComponent::SetShadowMapSize(unsigned int size)
{
  ShadowMapSize = size;
  if (m_bIsInitialized)
  {
    DeInitializeRenderer();
    InitializeRenderer();
  }
}

void IVShadowMapComponent::SetSampleRadius(float radius)
{
  SampleRadius = radius;
}

float IVShadowMapComponent::GetMaxDistanceToBlockerInLightSpace() const
{
  float fNear, fFar;
  m_spShadowMapGenerator->GetCascadeInfo(0).GetRenderContext()->GetClipPlanes(fNear, fFar);

  return MaxDistanceToBlocker / fFar;
}

void IVShadowMapComponent::SetUseSurfaceSpecificShadowShaders(BOOL use)
{
  UseSurfaceSpecificShadowShaders = use;
}

void IVShadowMapComponent::SetBias(int cascade, float bias)
{
  VASSERT(cascade >= 0 && cascade < MAX_SHADOW_PARTS_COUNT);
  Bias[cascade] = bias;
}

void IVShadowMapComponent::SetBias(float* bias, int size)
{
  VASSERT(size > 0 && size <= MAX_SHADOW_PARTS_COUNT);
  memcpy(Bias, bias, sizeof(float) * size);
}

void IVShadowMapComponent::SetSlopeScaled(int cascade, float slope)
{
  VASSERT(cascade >= 0 && cascade < MAX_SHADOW_PARTS_COUNT);
  SlopeScaled[cascade] = slope;
}

void IVShadowMapComponent::SetSlopeScaled(float* slope, int size)
{
  VASSERT(size > 0 && size <= MAX_SHADOW_PARTS_COUNT);
  memcpy(SlopeScaled, slope, sizeof(float) * size);
}

void IVShadowMapComponent::SetFilterBitmask(unsigned int bitmask)
{
  FilterBitmask = bitmask;
}


void IVShadowMapComponent::SetGeometryTypes(int iFlags)
{
  GeometryTypes = iFlags;
}

float IVShadowMapComponent::ClampNearClip(float nearClip)
{
  return (nearClip < 0.001f) ? 0.001f : nearClip;
}

void IVShadowMapComponent::SetNearClip(float nearClip)
{
  NearClip = ClampNearClip(nearClip);
  if (m_bIsInitialized)
  {
    m_spShadowMapGenerator->Update(true);
  }
}

void IVShadowMapComponent::Enable()
{
  if (m_pOwner == NULL)
    return;

  VString dummy;
  if (!CanAttachToObject(m_pOwner, dummy))
    return;

  Enabled = TRUE;
  Enabled = InitializeRenderer() ? TRUE : FALSE;  
}

void IVShadowMapComponent::Disable()
{
  DeInitializeRenderer();
  Enabled = FALSE;
}

void IVShadowMapComponent::SetEnabled(BOOL enabled)
{
  if (enabled)
    Enable();
  else
    Disable();
}

VisMeshBuffer_cl* IVShadowMapComponent::GetLightVolumeMeshBuffer()
{
  return m_pLightVolumeMeshBuffer;
}

void IVShadowMapComponent::GetLightVolumeMeshBufferIndexAndCount(int iCascade, int& iIndex, int& iCount)
{
  iIndex = 0; 
  iCount = m_pLightVolumeMeshBuffer->GetIndexCount() / 3;
}

IVShadowMapComponent *IVShadowMapComponent::GetShadowMapComponent(VisLightSource_cl *pLight, VRendererNodeCommon *pRendererNode)
{
#if defined (WIN32)
  if (Vision::Editor.GetIgnoreAdvancedEffects())
    return NULL;
#endif

  int iNumComponents = pLight->Components().Count();
  IVObjectComponent **pComponents = pLight->Components().GetPtrs();
  for (int i=0; i<iNumComponents; i++, pComponents++)
  {
    if (IVShadowMapComponent *pShadowMapComponent = vdynamic_cast<IVShadowMapComponent*>(*pComponents))
    {
      if (pShadowMapComponent->IsEnabled() && pShadowMapComponent->GetRendererNode() == pRendererNode)
      {
        return pShadowMapComponent;
      }
    }
  }

  return NULL;
}


START_VAR_TABLE(IVShadowMapComponent, IVObjectComponent, "Abstract ShadowMap Component", VFORGE_HIDECLASS, "Shadow Map Component")  
  DEFINE_VAR_BOOL(IVShadowMapComponent, Enabled, "Enable or disable component/shadows", "TRUE", 0, 0);
  DEFINE_VAR_INT(IVShadowMapComponent, GeometryTypes, "Flags that determine which geometry may cast shadow", "14", 0, "CSharpType(ShadowCasterGeometryTypes_e)");
  DEFINE_VAR_BOOL(IVShadowMapComponent, UseQuarterSizeShadowTexture, "Use quarter screen resolution for shadow calculation to increase performance", "FALSE", 0, 0);
  DEFINE_VAR_ENUM(IVShadowMapComponent, ShadowMappingMode, "Shadow-Mapping Mode", "PCF4", "PCF4,PCF8,PCF8 randomized,PCF16,PCF16 randomized,PCSS16,PCSS16 randomized,CHS,DEBUG", 0, 0);
  DEFINE_VAR_INT(IVShadowMapComponent, ShadowMapSize, "Shadow Map Resolution", "512", 0, "Clamp(1,65536)");
  DEFINE_VAR_FLOAT(IVShadowMapComponent, SampleRadius, "A higher Sample Radius results in softer shadows but can introduce more artefacts", "2", 0, 0);
  DEFINE_VAR_FLOAT(IVShadowMapComponent, SampleRadiusScaleWithDistance, "Scale factor to increase the sample radius with the distance to the camera", "0", 0, 0);
  DEFINE_VAR_FLOAT(IVShadowMapComponent, MaxDistanceToBlocker, "Maximum distance to blocker at which the sample radius reaches it's full size", "1000", 0, 0);
  DEFINE_VAR_BOOL(IVShadowMapComponent, UseSurfaceSpecificShadowShaders, "Use surface specific shadowmap shader", "FALSE", 0, 0);
  DEFINE_VAR_FLOAT(IVShadowMapComponent, Bias[0], "Depth Bias for Cascade 0", "2.0", 0, 0);
  DEFINE_VAR_FLOAT(IVShadowMapComponent, SlopeScaled[0], "Slope-scale Depth Bias for Cascade 0", "0.0", 0, 0);
  DEFINE_VAR_FLOAT(IVShadowMapComponent, Bias[1], "Depth Bias for Cascade 1", "6.0", 0, 0);
  DEFINE_VAR_FLOAT(IVShadowMapComponent, SlopeScaled[1], "Slope-scale Depth Bias for Cascade 1", "0.0", 0, 0);
  DEFINE_VAR_FLOAT(IVShadowMapComponent, Bias[2], "Depth Bias for Cascade 2", "18.0", 0, 0);
  DEFINE_VAR_FLOAT(IVShadowMapComponent, SlopeScaled[2], "Slope-scale Depth Bias for Cascade 2", "0.0", 0, 0);
  DEFINE_VAR_FLOAT(IVShadowMapComponent, Bias[3], "Depth Bias for Cascade 3", "54.0", 0, 0);
  DEFINE_VAR_FLOAT(IVShadowMapComponent, SlopeScaled[3], "Slope-scale Depth Bias for Cascade 3", "0.0", 0, 0);
  DEFINE_VAR_INT(IVShadowMapComponent, FilterBitmask, "Context filter bitmask", "65535", 0, "Bitmask");
  DEFINE_VAR_FLOAT(IVShadowMapComponent, NearClip, "Near Clipping Distance", "10.0", 0, "Clamp(0.001, 32000)");
  DEFINE_VAR_COLORREF(IVShadowMapComponent, AmbientColor, "Ambient Color for combining static and dynamic Shadows", "128,128,128,255", 0, 0);
  DEFINE_VAR_FLOAT(IVShadowMapComponent, ShadowBoxExtrudeMultiplier, "This factor multiplied by the height of a shadow caster defines how far the bounding box is extruded to determine shadow receiver", "1.5", 0, 0);
  DEFINE_VAR_BOOL(IVShadowMapComponent, FrontFacingShadows, "Triangles facing the light source (instead of away from it) cast shadows", "FALSE", 0, 0);
END_VAR_TABLE

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
