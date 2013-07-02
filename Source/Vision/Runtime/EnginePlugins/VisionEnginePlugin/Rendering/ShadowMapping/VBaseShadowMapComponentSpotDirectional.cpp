/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VBaseShadowMapComponentSpotDirectional.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

V_IMPLEMENT_SERIAL(VBaseShadowMapComponentSpotDirectional, IVShadowMapComponent, 0, &g_VisionEngineModule);

VBaseShadowMapComponentSpotDirectional::VBaseShadowMapComponentSpotDirectional() : 
  IVShadowMapComponent()
{
  CascadeCount = 1;
  CascadeSelection = CSM_NO_SELECTION;
  OverestimateCascades = TRUE;
  CameraUpdateInterval = 200.0f;
  CameraUpdateAngle = 5.0f;
  FadeOutStart = -1.0f;
  FadeOutEnd = -1.0f;
  CascadeRange[0] = 1000.0f;
  CascadeRange[1] = 3000.0f;
  CascadeRange[2] = 9000.0f;
  CascadeRange[3] = 27000.0f;
}

VBaseShadowMapComponentSpotDirectional::VBaseShadowMapComponentSpotDirectional(int iRendererNodeIndex) : 
  IVShadowMapComponent(iRendererNodeIndex)
{
  CascadeCount = 1;
  CascadeSelection = CSM_NO_SELECTION;
  OverestimateCascades = TRUE;
  CameraUpdateInterval = 200.0f;
  CameraUpdateAngle = 5.0f;
  FadeOutStart = -1.0f;
  FadeOutEnd = -1.0f;
  CascadeRange[0] = 1000.0f;
  CascadeRange[1] = 3000.0f;
  CascadeRange[2] = 9000.0f;
  CascadeRange[3] = 27000.0f;
}

VBaseShadowMapComponentSpotDirectional::~VBaseShadowMapComponentSpotDirectional()
{ 
}

BOOL VBaseShadowMapComponentSpotDirectional::CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
  if (!IVShadowMapComponent::CanAttachToObject(pObject, sErrorMsgOut))
    return FALSE;

  VisLightSource_cl *pLightSource = (VisLightSource_cl*)pObject;
  if (pLightSource->GetType() != VIS_LIGHT_DIRECTED && pLightSource->GetType() != VIS_LIGHT_SPOTLIGHT)
  {
    sErrorMsgOut = "VBaseShadowMapComponentSpotDirectional can only be attached to directional Lights or Spotlights!";
    return FALSE;
  }

  return TRUE;
}

bool VBaseShadowMapComponentSpotDirectional::SetVariable(const char *szName, const char *szValue)
{
  IVShadowMapComponent::SetVariable(szName, szValue);

  if (m_bIsInitialized)
  {
    if (!strcmp(szName, "CascadeCount"))
      SetCascadeCount(CascadeCount);
    else if (!strcmp(szName, "CascadeSelection"))
      SetCascadeSelection(CascadeSelection);  
    else if (!strcmp(szName, "OverestimateCascades"))
      SetOverestimateCascades(OverestimateCascades);  
    else if (!strncmp(szName, "CascadeRange", 12))
      SetCascadeRangePtr(CascadeRange, 4);  
    else if (!strcmp(szName, "CameraUpdateInterval"))
      SetShadowMapCameraUpdateInterval(CameraUpdateInterval);  
    else if (!strcmp(szName, "CameraUpdateAngle"))
      SetShadowMapCameraUpdateAngle(CameraUpdateAngle);
  }

  return true;
}

void VBaseShadowMapComponentSpotDirectional::SetShadowMapCameraUpdateAngle(float fAngle) 
{
  CameraUpdateAngle = fAngle;

  if (m_bIsInitialized)
    m_spShadowMapGenerator->Update(true);
}

void VBaseShadowMapComponentSpotDirectional::SetShadowMapCameraUpdateInterval(float fInterval) 
{ 
  CameraUpdateInterval = fInterval;

  if (m_bIsInitialized)
    m_spShadowMapGenerator->Update(true);
}

void VBaseShadowMapComponentSpotDirectional::Serialize( VArchive &ar )
{
  IVShadowMapComponent::Serialize(ar);

  if (ar.IsLoading())
  {
    ar >> CascadeCount;
    if (m_iLocalVersion >= SHADOWMAP_COMPONENT_VERSION_4)
    {
      int tmp;
      ar >> tmp;
      CascadeSelection = (VCascadeSelectionMethod_e)tmp;
    }
    ar >> CascadeRange[0];
    ar >> CascadeRange[1];
    ar >> CascadeRange[2];
    ar >> CascadeRange[3];
    ar >> OverestimateCascades;     
    ar >> CameraUpdateInterval;
    ar >> CameraUpdateAngle;
    if (m_iLocalVersion >= SHADOWMAP_COMPONENT_VERSION_4)
    {
      ar >> FadeOutStart;
      ar >> FadeOutEnd;
    }
  }
  else
  {
    ar << CascadeCount;
    ar << (int)CascadeSelection;
    ar << CascadeRange[0];
    ar << CascadeRange[1];
    ar << CascadeRange[2];
    ar << CascadeRange[3];
    ar << OverestimateCascades;    
    ar << CameraUpdateInterval;
    ar << CameraUpdateAngle;
    ar << FadeOutStart;
    ar << FadeOutEnd;
  }
}

#if defined(WIN32) || defined(_VISION_DOC)

void VBaseShadowMapComponentSpotDirectional::GetVariableAttributes(VisVariable_cl* pVariable, VVariableAttributeInfo& destInfo)
{
  IVShadowMapComponent::GetVariableAttributes(pVariable, destInfo);
  if (m_pLightSource->GetType() == VIS_LIGHT_SPOTLIGHT)
  {
    if (!strcmp(pVariable->GetName(), "CascadeCount"))
      destInfo.m_bHidden = true;
    else if (!strcmp(pVariable->GetName(), "CascadeSelection"))
      destInfo.m_bHidden = true;
    else if (!strncmp(pVariable->GetName(), "CascadeRange", 12))
      destInfo.m_bHidden = true;
    else if (!strncmp(pVariable->GetName(), "Bias", 4) && pVariable->GetName()[5] != '0')
      destInfo.m_bHidden = true; 
    else if (!strncmp(pVariable->GetName(), "SlopeScaled", 11) && pVariable->GetName()[12] != '0')
      destInfo.m_bHidden = true; 
    else if (!strcmp(pVariable->GetName(), "OverestimateCascades"))
      destInfo.m_bHidden = true; 
    else if (!strcmp(pVariable->GetName(), "CameraUpdateInterval"))
      destInfo.m_bHidden = true; 
    else if (!strcmp(pVariable->GetName(), "CameraUpdateAngle"))
      destInfo.m_bHidden = true;
    else if (!strcmp(pVariable->GetName(), "FadeOutStart"))
      destInfo.m_bHidden = true;
    else if (!strcmp(pVariable->GetName(), "FadeOutEnd"))
      destInfo.m_bHidden = true;
  }
  else if (m_pLightSource->GetType() == VIS_LIGHT_DIRECTED)
  {
    if (!strcmp(pVariable->GetName(), "NearClip"))
      destInfo.m_bHidden = true;
  }
  else if (!OverestimateCascades)
  {
    if (!strcmp(pVariable->GetName(), "CameraUpdateInterval"))
      destInfo.m_bReadOnly = true;
    else if (!strcmp(pVariable->GetName(), "CameraUpdateAngle"))
      destInfo.m_bReadOnly = true;
  }
}

#endif

void VBaseShadowMapComponentSpotDirectional::SetCascadeCount(unsigned int count)
{
  count = hkvMath::Min ((unsigned int) MAX_SHADOW_PARTS_COUNT, hkvMath::Max (1U, count));

  if (m_pLightSource != NULL && m_pLightSource->GetType() == VIS_LIGHT_SPOTLIGHT)
    count = 1;
  CascadeCount = count;
  if (m_bIsInitialized)
  {
    DeInitializeRenderer();
    InitializeRenderer();
  }
}

void VBaseShadowMapComponentSpotDirectional::SetCascadeSelection(VCascadeSelectionMethod_e cascadeSelection)
{
  CascadeSelection = cascadeSelection;
  if (m_bIsInitialized)
  {
    DeInitializeRenderer();
    InitializeRenderer();
  }
}

void VBaseShadowMapComponentSpotDirectional::SetCascadeRange(int iCascade, float fRange)
{
  VASSERT(iCascade >= 0 && iCascade < MAX_SHADOW_PARTS_COUNT);
  CascadeRange[iCascade] = fRange;
  if (m_bIsInitialized)
  {
    float fNear, fFar;
    m_pRendererNode->GetReferenceContext()->GetClipPlanes(fNear, fFar);
    float fStart = iCascade == 0 ? fNear : CascadeRange[iCascade - 1] + fNear;
    m_spShadowMapGenerator->GetCascadeInfo(iCascade).ComputeOffset(fStart, fRange + fNear);
    m_spShadowMapGenerator->Update(true);
  }
}

void VBaseShadowMapComponentSpotDirectional::SetCascadeRangePtr(float* range, int size)
{
  VASSERT(size > 0 && size <= MAX_SHADOW_PARTS_COUNT);
  memcpy(CascadeRange, range, sizeof(float) * size);
  if (m_bIsInitialized)
  {
    float fNear, fFar;
    m_pRendererNode->GetReferenceContext()->GetClipPlanes(fNear, fFar);
    float start = fNear;
    for (int i = 0; i < (int)CascadeCount; i++)
    {
      m_spShadowMapGenerator->GetCascadeInfo(i).ComputeOffset(start, CascadeRange[i] + fNear);
      start = CascadeRange[i] + fNear;
    }
    m_spShadowMapGenerator->Update(true);
  }
}

void VBaseShadowMapComponentSpotDirectional::SetOverestimateCascades(BOOL overestimate)
{
  OverestimateCascades = overestimate;
  if (m_bIsInitialized)
  {
    DeInitializeRenderer();
    InitializeRenderer();
  }
}

void VBaseShadowMapComponentSpotDirectional::GetLightVolumeMeshBufferIndexAndCount(int iCascade, int& iIndex, int& iCount)
{
  iIndex = 0; 
  if (m_pLightSource->GetType() == VIS_LIGHT_SPOTLIGHT)
    iCount = m_pLightVolumeMeshBuffer->GetIndexCount() / 3;
  else
    iCount = 2;
}

START_VAR_TABLE(VBaseShadowMapComponentSpotDirectional, IVShadowMapComponent, "Base ShadowMap Component for Spotlights or directional lights", VCOMPONENT_ALLOW_MULTIPLE | VFORGE_HIDECLASS, "Shadow Map Component Spot/Directional")  
DEFINE_VAR_INT(VBaseShadowMapComponentSpotDirectional, CascadeCount, "Number of Cascades (1..4)", "2", 0, 0);
DEFINE_VAR_ENUM(VBaseShadowMapComponentSpotDirectional, CascadeSelection, "Cascade Selection Method", "By Interval", "By Interval,By Bounding Box", 0, 0);
DEFINE_VAR_FLOAT(VBaseShadowMapComponentSpotDirectional, CascadeRange[0], "Cascade 0 range", "1000", 0, 0);
DEFINE_VAR_FLOAT(VBaseShadowMapComponentSpotDirectional, CascadeRange[1], "Cascade 1 range", "3000", 0, 0);
DEFINE_VAR_FLOAT(VBaseShadowMapComponentSpotDirectional, CascadeRange[2], "Cascade 2 range", "9000", 0, 0);
DEFINE_VAR_FLOAT(VBaseShadowMapComponentSpotDirectional, CascadeRange[3], "Cascade 3 range", "27000", 0, 0);
DEFINE_VAR_BOOL(VBaseShadowMapComponentSpotDirectional, OverestimateCascades, "Uses more shadow map space but provides a more stable result", "TRUE", 0, 0);
DEFINE_VAR_FLOAT(VBaseShadowMapComponentSpotDirectional, CameraUpdateInterval, "Minimum distance the camera needs to travel before the shadow map camera is updated", "200.0", 0, 0);
DEFINE_VAR_FLOAT(VBaseShadowMapComponentSpotDirectional, CameraUpdateAngle, "Minimum angle the camera needs to turn before the shadow map camera is updated", "5.0", 0, 0);
DEFINE_VAR_FLOAT(VBaseShadowMapComponentSpotDirectional, FadeOutStart, "Distance to start fading out the shadows. If set to negative values this is calculated automatically", "-1.0f", 0, 0);
DEFINE_VAR_FLOAT(VBaseShadowMapComponentSpotDirectional, FadeOutEnd, "Distance to end fading out the shadows. If set to negative values this is calculated automatically", "-1.0f", 0, 0);
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
