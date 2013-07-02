/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/VCoronaComponent.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/VCoronaManager.hpp>
#include <Vision/Runtime/Engine/Renderer/Texture/VisApiTextureManager.hpp> 
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

/////////////////////////////////////////////////////////////////////////////
// VOcclusionQueryObjectPixelCounterCorona: public functions
/////////////////////////////////////////////////////////////////////////////

VOcclusionQueryObjectPixelCounterCorona::VOcclusionQueryObjectPixelCounterCorona()
: VOcclusionQueryObjectPixelCounter(), m_pCorona(NULL)
{
  VisRenderContext_cl::SetPixelCounterResultInAllContexts(GetNumber(), 0);
}

VOcclusionQueryObjectPixelCounterCorona::~VOcclusionQueryObjectPixelCounterCorona()
{
}

bool VOcclusionQueryObjectPixelCounterCorona::Render(VOcclusionQuery &query, const hkvAlignedBBox &safeBox)
{
  if (m_pCorona != NULL && m_pCorona->GetOwner() != NULL)
  {
    SetState(VISQUERY_RENDERSTATE_BILLBOARD);
    
    VisRenderContext_cl* pContext = VisRenderContext_cl::GetCurrentContext();
    
    VisLightSource_cl* pLight = (VisLightSource_cl*)m_pCorona->GetOwner();
    hkvVec3 vPos(hkvNoInitialization);
    pLight->GetVirtualPosition(vPos, pContext);

    hkvVec3 vCameraDir = pContext->GetCamera()->GetPosition() - vPos;
    vCameraDir /= hkvMath::Max(vCameraDir.getLength(), HKVMATH_LARGE_EPSILON);

    vPos += vCameraDir * m_pCorona->GetDepthBias();

    query.DoHardwareOcclusionTest_Billboard(&vPos.x, m_pCorona->GetCheckBlockSize());
    return true;
  }
  return false;
}


/////////////////////////////////////////////////////////////////////////////
// VCoronaComponent: Constructor functions
/////////////////////////////////////////////////////////////////////////////

float VCoronaComponent::s_fGlobalFadeOutStart = 0;
float VCoronaComponent::s_fGlobalFadeOutEnd = 0;

VCoronaComponent::VCoronaComponent(int iComponentFlags)
  : IVObjectComponent(0, iComponentFlags), m_iIndex(-1),
  CoronaScaling(1.0f), PreGlowMS(0), AfterGlowMS(0), DepthBias(0.0f), FadeOutStart(0.0f), FadeOutEnd(0.0f), VisibleBitmask(0xffffffff), QueryRadius(8),
  CoronaFlags(VIS_CORONASCALE_NONE|VIS_CORONASCALE_ROTATING), Enabled(true)

{
  m_CoronaPixelCounter.m_pCorona = this;
  SetCoronaTexture(NULL);
}


VCoronaComponent::~VCoronaComponent()
{
  m_spTexture = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// VCoronaComponent: Corona Properties
/////////////////////////////////////////////////////////////////////////////

void VCoronaComponent::SetCoronaTexture(const char *textureFile)
{
  if (textureFile==NULL || textureFile[0]==0)
    textureFile = "\\mainlens.dds";

  m_spTexture = Vision::TextureManager.Load2DTexture(textureFile, VTM_FLAG_DEFAULT_NON_MIPMAPPED | VTM_FLAG_ASSUME_SRGB);
}

const char * VCoronaComponent::GetCoronaTextureFileName() const
{
  return CoronaTexture.AsChar();
}

VTextureObject* VCoronaComponent::GetCoronaTexture ()
{
  return m_spTexture;
}


/////////////////////////////////////////////////////////////////////////////
// VCoronaComponent: Update functions
/////////////////////////////////////////////////////////////////////////////

void VCoronaComponent::UpdateVisibility (float& fLastVisibilityQuery, float& fCurrentVisibility)
{
  // Make sure we are actually attached to an object
  if (GetOwner())
  {
    // Get Camera
    VisRenderContext_cl* pContext = VisRenderContext_cl::GetCurrentContext();
    const hkvVec3 vCameraPos = pContext->GetCamera()->GetPosition();

    // Get Light
    VisLightSource_cl* pLight = (VisLightSource_cl*)GetOwner();
    const hkvVec3 vPos = pLight->GetPosition();

    hkvVec3 vDist = vCameraPos - vPos;
    float fDist = vDist.getLength();
    float fFactor = 1.0f;

    // Distance FadeOut
    float fFadeOutStart = FadeOutStart;
    float fFadeOutEnd = FadeOutEnd;
    if (CoronaFlags & VIS_CORONASCALE_USEFADEOUT)
    {
      GetGlobalCoronaFadeOutDistance(fFadeOutStart, fFadeOutEnd);
    }

    if (fFadeOutEnd != 0 && fFadeOutStart < fFadeOutEnd)
    {
      if (fDist > fFadeOutEnd)
      {
        fFactor = 0.0f;
      }
      else if (fDist > fFadeOutStart)
      {
        fFactor = 1.0f - (fDist - fFadeOutStart) / (fFadeOutEnd - fFadeOutStart);
      }
    }

    // Apply distance fade out
    fLastVisibilityQuery *= fFactor;

    // PreGlow/AfterGlow
    if (fLastVisibilityQuery > fCurrentVisibility)
    {
      float fSpeed = Vision::GetTimer()->GetTimeDifference() / ((PreGlowMS + 1) * 0.001f);
      fCurrentVisibility = hkvMath::Min(fCurrentVisibility + fSpeed, fLastVisibilityQuery);
    }
    else if (fLastVisibilityQuery < fCurrentVisibility)
    {
      float fSpeed = Vision::GetTimer()->GetTimeDifference() / ((AfterGlowMS + 1) * 0.001f);
      fCurrentVisibility = hkvMath::Max(fCurrentVisibility - fSpeed, fLastVisibilityQuery);
    }

    fCurrentVisibility = hkvMath::clamp(fCurrentVisibility, 0.0f, 1.0f);
  } 
}


/////////////////////////////////////////////////////////////////////////////
// VCoronaComponent: IVObjectComponent Overrides
/////////////////////////////////////////////////////////////////////////////

void VCoronaComponent::SetOwner(VisTypedEngineObject_cl *pOwner)
{
  IVObjectComponent::SetOwner(pOwner);

  // add or remove from manager according to whether we have an owner or not
  if (pOwner)
  {
    m_iIndex = VCoronaManager::GlobalManager().AddCorona(this);
  }
  else
  {
    VCoronaManager::GlobalManager().RemoveCorona(this);
    m_iIndex = -1;
  }
}


BOOL VCoronaComponent::CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
#if defined( HK_ANARCHY )
  sErrorMsgOut = "Component type not supported in Anarchy.";
  return FALSE;
#else
  if (!IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut))
    return FALSE;

  if (!pObject->IsOfType(V_RUNTIME_CLASS(VisLightSource_cl)))
  {
    sErrorMsgOut = "Component can only be added to instances of VisLightSource_cl or derived classes.";
    return FALSE;
  }

  return TRUE;
#endif
}

#if defined(WIN32) || defined(_VISION_DOC)

void VCoronaComponent::GetVariableAttributes(VisVariable_cl *pVariable, VVariableAttributeInfo &destInfo)
{
  // Check whether we need to display the OnlyVisibleInSpotLight property. It should be only visible when the owner is a spotlight
  if (!strcmp(pVariable->GetName(),"OnlyVisibleInSpotLight"))
  {
    VisLightSource_cl* pLight = (VisLightSource_cl*)GetOwner();
    if(pLight && pLight->GetType() == VIS_LIGHT_SPOTLIGHT)
      destInfo.m_bHidden = false;
    else
      destInfo.m_bHidden = true;
  }
}

#endif

void VCoronaComponent::OnVariableValueChanged(VisVariable_cl *pVar, const char * value)
{
  VASSERT (pVar);
  VASSERT (value);

  if (pVar->name && strcmp(pVar->name, "QueryRadius") == 0)
  {
    int iQueryRadius = QueryRadius;
    if (sscanf (value, "%d", &iQueryRadius) == 1)
    {
      QueryRadius = hkvMath::Min( hkvMath::Max(iQueryRadius, 1), 255);
    }
  }   
  else if (pVar->name != NULL && strcmp(pVar->name, "CoronaTexture") == 0)
  {
    CoronaTexture = value;
    SetCoronaTexture(value);
  }
  else if (pVar->name != NULL && strcmp(pVar->name, "CoronaFixedSize") == 0)
  {
    if (value != NULL && strcmp(value, "True") == 0)
      CoronaFlags &= ~VIS_CORONASCALE_DISTANCE;
    else
      CoronaFlags |= VIS_CORONASCALE_DISTANCE;
  }
  else if (pVar->name != NULL && strcmp(pVar->name, "CoronaScaleWithIntensity") == 0)
  {
    if (value != NULL && strcmp(value, "True") == 0)
      CoronaFlags |= VIS_CORONASCALE_VISIBLEAREA;
    else
      CoronaFlags &= ~VIS_CORONASCALE_VISIBLEAREA;
  }
  else if (pVar->name != NULL && strcmp(pVar->name, "CoronaGlobalFadeOut") == 0)
  {
    if (value != NULL && strcmp(value, "True") == 0)
      CoronaFlags |= VIS_CORONASCALE_USEFADEOUT;
    else
      CoronaFlags &= ~VIS_CORONASCALE_USEFADEOUT;
  }
  else if (pVar->name != NULL && strcmp(pVar->name, "CoronaRotate") == 0)
  {
    if (value != NULL && strcmp(value, "True") == 0)
      CoronaFlags |= VIS_CORONASCALE_ROTATING;
    else
      CoronaFlags &= ~VIS_CORONASCALE_ROTATING;
  }
}


/////////////////////////////////////////////////////////////////////////////
// VCoronaComponent: Serialization
/////////////////////////////////////////////////////////////////////////////
#define CORONA_SERIALIZATION_VERSION_0         0 // initial version
#define CORONA_SERIALIZATION_VERSION_1         1 // Added VisibilityBitmask (2012.1)
#define CORONA_SERIALIZATION_VERSION_2         2 // Added OnlyVisibleInSpotLight
#define CORONA_SERIALIZATION_VERSION_CURRENT   CORONA_SERIALIZATION_VERSION_2

V_IMPLEMENT_SERIAL(VCoronaComponent,IVObjectComponent,0,&g_VisionEngineModule);
void VCoronaComponent::Serialize( VArchive &ar )
{
  char iLocalVersion = CORONA_SERIALIZATION_VERSION_CURRENT;

  IVObjectComponent::Serialize(ar);
  if (ar.IsLoading())
  {
    ar >> iLocalVersion;
    VASSERT_MSG(iLocalVersion<=CORONA_SERIALIZATION_VERSION_CURRENT, "Invalid local VCoronaComponent version. Please re-export");
    VMemoryTempBuffer<FS_MAX_PATH> sTextureFile;
    const char *szFilename = sTextureFile.ReadStringBinary(ar);
    SetCoronaTexture(szFilename);
    ar >> CoronaScaling;
    ar >> PreGlowMS;
    ar >> AfterGlowMS;
    ar >> DepthBias;
    ar >> FadeOutStart;
    ar >> FadeOutEnd;
    ar >> QueryRadius;
    ar >> CoronaFlags;
    ar >> Enabled;
    if (iLocalVersion >= CORONA_SERIALIZATION_VERSION_1)
    {
      ar >> VisibleBitmask;
    }
    if (iLocalVersion >= CORONA_SERIALIZATION_VERSION_2)
    {
      ar >> OnlyVisibleInSpotLight;
    }
  }
  else
  {
    ar << iLocalVersion;
    ar << CoronaTexture;
    ar << CoronaScaling;
    ar << PreGlowMS;
    ar << AfterGlowMS;
    ar << DepthBias;
    ar << FadeOutStart;
    ar << FadeOutEnd;
    ar << QueryRadius;
    ar << CoronaFlags;
    ar << Enabled;
    ar << VisibleBitmask;
    ar << OnlyVisibleInSpotLight;
  }
}


bool VCoronaComponent::IsValidCandidate(VisRenderContext_cl* pContext) const
{
  // Check enabled flag
  if (!Enabled)
    return false;

  // Check viewport overlap
  VisLightSource_cl* pLight = (VisLightSource_cl*)GetOwner();

  hkvVec3 vLightPos(hkvNoInitialization);
  pLight->GetVirtualPosition(vLightPos, pContext);

  float fX, fY;
  // Tests whether the point is in front of the camera.
  if (!pContext->Project2D(vLightPos, fX, fY))
    return false;

  int iX = int(fX);
  int iY = int(fY);

  int iPosX, iPosY, iWidth, iHeight;
  pContext->GetViewport(iPosX, iPosY, iWidth, iHeight);

  int iDistX = hkvMath::Abs (iX - (iPosX + iWidth/2)) - iWidth/2;
  int iDistY = hkvMath::Abs (iY - (iPosY + iHeight/2)) - iHeight/2;
  int iMaxDist = hkvMath::Max(iDistX, iDistY);

  if (iMaxDist > QueryRadius)
    return false;
 
  // Check distance
  const hkvVec3 vCameraPos = pContext->GetCamera()->GetPosition();
  hkvVec3 vDistance = pLight->GetPosition() - vCameraPos;

  float fFadeOutStart = FadeOutStart;
  float fFadeOutEnd = FadeOutEnd;
  if (CoronaFlags & VIS_CORONASCALE_USEFADEOUT)
  {
    GetGlobalCoronaFadeOutDistance(fFadeOutStart, fFadeOutEnd);
  }

  if ( (fFadeOutEnd != 0.0f) && (fFadeOutEnd * fFadeOutEnd < vDistance.getLengthSquared()) )
    return false;

  return true;
}

bool VCoronaComponent::SetGlobalCoronaFadeOutDistance( float fStartDistance, float fEndDistance )
{
  if (fStartDistance<0 || fEndDistance<0 || fStartDistance>=fEndDistance) 
    return false;
  s_fGlobalFadeOutStart = fStartDistance;
  s_fGlobalFadeOutEnd = fEndDistance;
  return true;
}

void VCoronaComponent::GetGlobalCoronaFadeOutDistance(float &out_fStartDistance, float &out_fEndDistance)
{
  out_fStartDistance = s_fGlobalFadeOutStart;
  out_fEndDistance = s_fGlobalFadeOutEnd;
}


START_VAR_TABLE(VCoronaComponent,IVObjectComponent,"Corona component",VVARIABLELIST_FLAGS_NONE, "Corona" )
  DEFINE_VAR_BOOL(VCoronaComponent, Enabled, "Enable or disable component", "TRUE", 0, NULL);

  DEFINE_CATEGORY (VCoronaComponent, "CoronaAppearance")
    varList->Add (VisVariable_cl::NewVariable ("CoronaTexture", "Custom corona texture filename. Empty string for default corona.", VULPTYPE_VSTRING, -1, "", (VDisplayHintFlags_e)DISPLAY_HINT_TEXTUREFILE, 0, 0)  );
    DEFINE_VAR_FLOAT(VCoronaComponent, CoronaScaling, "Relative scaling of the corona texture (1.0 for full size)", "1.0", 0, 0);
    varList->Add (VisVariable_cl::NewVariable ("CoronaFixedSize", "If enabled, the corona has fixed size on-screen, otherwise it scales with distance like normal geometry", VULPTYPE_BOOL, -1, "True", (VDisplayHintFlags_e)0, 0, 0)  );
    varList->Add (VisVariable_cl::NewVariable ("CoronaScaleWithIntensity", "If enabled, the corona's size scales with the intensity (i.e. how much it is covered)", VULPTYPE_BOOL, -1, "False", (VDisplayHintFlags_e)0, 0, 0)  );
    varList->Add (VisVariable_cl::NewVariable ("CoronaGlobalFadeOut", "If enabled, the corona uses the engine's global fade out range", VULPTYPE_BOOL, -1, "False", (VDisplayHintFlags_e)0, 0, 0)  );
    varList->Add (VisVariable_cl::NewVariable ("CoronaRotate", "If enabled, the corona rotates based on distance", VULPTYPE_BOOL, -1, "True", (VDisplayHintFlags_e)0, 0, 0)  );

  DEFINE_CATEGORY (VCoronaComponent, "CoronaProperties")
    varList->Add (VisVariable_cl::NewVariable ("QueryRadius", "The size in pixels of the occlusion query window", VULPTYPE_UINT, -1, "8", (VDisplayHintFlags_e)0, "Clamp(1,255)", 0) );
    DEFINE_VAR_UINT(VCoronaComponent, PreGlowMS, "Fade-in time in ms. If specified, the effect will not appear immediately, but fade in smoothly.", "0", 0, 0);
    DEFINE_VAR_UINT(VCoronaComponent, AfterGlowMS, "Fade-out time in ms.If specified, the effect will fade out smoothly after disappearing.", "0", 0, 0);
    DEFINE_VAR_FLOAT(VCoronaComponent, DepthBias, "Depth bias for occlusion test and corona-type flare rendering. Positive values will move the query closer to the camera. Negative values will move the query away from the camera.", "0", 0, 0);
    DEFINE_VAR_FLOAT(VCoronaComponent, FadeOutStart, "The distance (in world units) at which the effect has full intensity.", "0", 0, 0);
    DEFINE_VAR_FLOAT(VCoronaComponent, FadeOutEnd, "The distance (in world units) at which the effect has no intensity. Set to zero to disable fading.", "0", 0, 0);
    DEFINE_VAR_BITMASK(VCoronaComponent, VisibleBitmask, "Light visibility bitmask that determines the visible status in the rendering contexts.", "4294967295", 0, 0);
    DEFINE_VAR_BOOL(VCoronaComponent, OnlyVisibleInSpotLight, "This property only influences the Corona, when it is attached to a spotlight. If it is true, the Corona is only visible if the camera is inside the cone defined by the spotlight. Please also note that the projected texture is not used to calculate the intensity of the corona. Instead a simple falloff function is used.", 0, 0, 0);

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
