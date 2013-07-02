/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/VLensFlareComponent.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/VLensFlareManager.hpp>
#include <Vision/Runtime/Engine/Renderer/Texture/VisApiTextureManager.hpp> 
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

/////////////////////////////////////////////////////////////////////////////
// VOcclusionQueryObjectPixelCounterLensFlare: public functions
/////////////////////////////////////////////////////////////////////////////

VOcclusionQueryObjectPixelCounterLensFlare::VOcclusionQueryObjectPixelCounterLensFlare()
: VOcclusionQueryObjectPixelCounter(), m_pLensFlare(NULL)
{
  VisRenderContext_cl::SetPixelCounterResultInAllContexts(GetNumber(), 0);
}

VOcclusionQueryObjectPixelCounterLensFlare::~VOcclusionQueryObjectPixelCounterLensFlare()
{
}

bool VOcclusionQueryObjectPixelCounterLensFlare::Render(VOcclusionQuery &query, const hkvAlignedBBox &safeBox)
{
  if (m_pLensFlare != NULL && m_pLensFlare->GetOwner() != NULL)
  {
    SetState(VISQUERY_RENDERSTATE_BILLBOARD);
    
    VisRenderContext_cl* pContext = VisRenderContext_cl::GetCurrentContext();

    VisLightSource_cl* pLight = (VisLightSource_cl*)m_pLensFlare->GetOwner();
    hkvVec3 vPos(hkvNoInitialization);
    pLight->GetVirtualPosition(vPos, pContext);

    hkvVec3 vCameraDir = pContext->GetCamera()->GetPosition() - vPos;
    vCameraDir /= hkvMath::Max(vCameraDir.getLength(), HKVMATH_LARGE_EPSILON);

    vPos += vCameraDir * m_pLensFlare->GetDepthBias();

    query.DoHardwareOcclusionTest_Billboard(&vPos.x, m_pLensFlare->GetCheckBlockSize());
    return true;
  }
  return false;
}


/////////////////////////////////////////////////////////////////////////////
// VLensFlareComponent: Constructor functions
/////////////////////////////////////////////////////////////////////////////

VLensFlareComponent::VLensFlareComponent(int iComponentFlags)
  : IVObjectComponent(0, iComponentFlags), m_iIndex(-1),
  PreGlowMS(0), AfterGlowMS(0), DepthBias(0.0f), FadeOutStart(0.0f), FadeOutEnd(0.0f), QueryRadius(8), Enabled(true)

{
  m_LensFlarePixelCounter.m_pLensFlare = this;
  InitFlares();
}


VLensFlareComponent::~VLensFlareComponent()
{
  for (int iFlare = 0; iFlare < MAX_NUM_LENS_FLARES; ++iFlare)
  {
    m_spTextures[iFlare] = NULL;
  }
}


/////////////////////////////////////////////////////////////////////////////
// VLensFlareComponent: Lens Flare Properties
/////////////////////////////////////////////////////////////////////////////

bool VLensFlareComponent::SetLensFlareDescriptor (BYTE iIndex, const VLensFlareDescriptor& desc)
{
  if (iIndex < MAX_NUM_LENS_FLARES)
  {
// Prevent texture loading on platforms not supporting occlusion queries
#ifdef SUPPORTS_OCCLUSION_QUERY
    if (m_Flares[iIndex].m_sFlareTexture != desc.m_sFlareTexture)
    {
      m_spTextures[iIndex] = Vision::TextureManager.Load2DTexture(desc.m_sFlareTexture, VTM_FLAG_NO_MIPMAPS | VTM_FLAG_ASSUME_SRGB);
    }
#endif
    m_Flares[iIndex] = desc;

    return true;
  }
  return false;
}


bool VLensFlareComponent::GetLensFlareDescriptor (BYTE iIndex, VLensFlareDescriptor& desc) const
{
  if (iIndex < MAX_NUM_LENS_FLARES)
  {
    desc = m_Flares[iIndex];
    return true;
  }
  return false;
}


VTextureObject* VLensFlareComponent::GetLensFlareTexture (BYTE iIndex)
{
  if (iIndex < MAX_NUM_LENS_FLARES)
  {
    return m_spTextures[iIndex];
  }
  return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// VLensFlareComponent: Update functions
/////////////////////////////////////////////////////////////////////////////

void VLensFlareComponent::UpdateVisibility (float& fLastVisibilityQuery, float& fCurrentVisibility)
{
  // Make sure we are actually attached to an object
  if (GetOwner())
  {
    // Get Camera
    VisRenderContext_cl* pContext = VisRenderContext_cl::GetCurrentContext();
    const hkvVec3 vCameraPos = pContext->GetCamera()->GetPosition();

    // Get Light
    VisLightSource_cl* pLight = (VisLightSource_cl*)GetOwner();
    hkvVec3 vPos;
    pLight->GetVirtualPosition( vPos, pContext );

    hkvVec3 vDist = vCameraPos - vPos;
    float fDist = vDist.getLength();
    float fFactor = 1.0f;

    // Distance FadeOut
    if (FadeOutEnd != 0 && FadeOutStart < FadeOutEnd)
    {
      if (fDist > FadeOutEnd)
      {
        fFactor = 0.0f;
      }
      else if (fDist > FadeOutStart)
      {
        fFactor = 1.0f - (fDist - FadeOutStart) / (FadeOutEnd - FadeOutStart);
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
// VLensFlareComponent: IVObjectComponent Overrides
/////////////////////////////////////////////////////////////////////////////

void VLensFlareComponent::SetOwner(VisTypedEngineObject_cl *pOwner)
{
  IVObjectComponent::SetOwner(pOwner);

  // add or remove from manager according to whether we have an owner or not
  if (pOwner)
  {
    m_iIndex = VLensFlareManager::GlobalManager().AddLensFlare(this);
  }
  else
  {
    VLensFlareManager::GlobalManager().RemoveLensFlare(this);
    m_iIndex = -1;
  }
}


BOOL VLensFlareComponent::CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
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

void VLensFlareComponent::GetVariableAttributes(VisVariable_cl *pVariable, VVariableAttributeInfo &destInfo)
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

void VLensFlareComponent::OnVariableValueChanged(VisVariable_cl *pVar, const char * value)
{
  VASSERT (pVar);
  VASSERT (value);

  if (pVar->category && strcmp(pVar->category, "LensFlareProperties") == 0)
  {
    if (pVar->name && strcmp(pVar->name, "QueryRadius") == 0)
    {
      int iQueryRadius = QueryRadius;
      if (sscanf (value, "%d", &iQueryRadius) == 1)
      {
        QueryRadius = hkvMath::Min( hkvMath::Max(iQueryRadius, 1), 255);
      }
    }
  }
  else if (pVar->category && strcmp(pVar->category, "LensFlares") == 0)
  {
    int iFlareIndex = -1;

    VLensFlareDescriptor desc;

    if (sscanf(pVar->name,"FlareTexture[%d]",&iFlareIndex) == 1)
    {
      VASSERT (iFlareIndex >= 0 && iFlareIndex < MAX_NUM_LENS_FLARES);
      GetLensFlareDescriptor(iFlareIndex, desc);
      desc.m_sFlareTexture = value;
      SetLensFlareDescriptor(iFlareIndex, desc);
    }
    else if (sscanf(pVar->name,"Scale[%d]",&iFlareIndex) == 1)
    {
      VASSERT (iFlareIndex >= 0 && iFlareIndex < MAX_NUM_LENS_FLARES);
      GetLensFlareDescriptor(iFlareIndex, desc);
      if (sscanf (value, "%f", &desc.m_fScale) == 1)
      {
        SetLensFlareDescriptor(iFlareIndex, desc);
      }
    }
    else if (sscanf(pVar->name,"PositionOnRay[%d]",&iFlareIndex) == 1)
    {
      VASSERT (iFlareIndex >= 0 && iFlareIndex < MAX_NUM_LENS_FLARES);
      GetLensFlareDescriptor(iFlareIndex, desc);
      if (sscanf (value, "%f", &desc.m_fPositionOnRay) == 1)
      {
        SetLensFlareDescriptor(iFlareIndex, desc);
      }
    }
    else
      VASSERT_MSG(false, "Unknown flare property inside \"LensFlares\" category.")
  }
}


/////////////////////////////////////////////////////////////////////////////
// VLensFlareComponent: Serialization
/////////////////////////////////////////////////////////////////////////////
#define LENSFLARE_SERIALIZATION_VERSION_0         0 // initial version
#define LENSFLARE_SERIALIZATION_VERSION_1         1 // Added OnlyVisibleInSpotLight
#define LENSFLARE_SERIALIZATION_VERSION_CURRENT   LENSFLARE_SERIALIZATION_VERSION_1

V_IMPLEMENT_SERIAL(VLensFlareComponent,IVObjectComponent,0,&g_VisionEngineModule);
void VLensFlareComponent::Serialize( VArchive &ar )
{
  char iLocalVersion = LENSFLARE_SERIALIZATION_VERSION_CURRENT;

  IVObjectComponent::Serialize(ar);
  if (ar.IsLoading())
  {
    ar >> iLocalVersion;
    VASSERT_MSG(iLocalVersion<=LENSFLARE_SERIALIZATION_VERSION_CURRENT, "Invalid local version. Please re-export");
    ar >> PreGlowMS;
    ar >> AfterGlowMS;
    ar >> DepthBias;
    ar >> FadeOutStart;
    ar >> FadeOutEnd;
    ar >> QueryRadius;
    ar >> Enabled;
    if (iLocalVersion >= LENSFLARE_SERIALIZATION_VERSION_1)
    {
      ar >> OnlyVisibleInSpotLight;
    }

    BYTE iFlares;
    ar >> iFlares;
    VASSERT(iFlares <= MAX_NUM_LENS_FLARES);
    VLensFlareDescriptor desc;
    for (BYTE i=0; i < iFlares; ++i)
    {
      BYTE iFlareIndex;
      ar >> iFlareIndex;
      VASSERT(iFlareIndex < MAX_NUM_LENS_FLARES);
      ar >> desc.m_fPositionOnRay;
      ar >> desc.m_fScale;
      ar >> desc.m_sFlareTexture;
      SetLensFlareDescriptor(iFlareIndex, desc);
    }
   
  }
  else
  {
    ar << iLocalVersion;
    ar << PreGlowMS;
    ar << AfterGlowMS;
    ar << DepthBias;
    ar << FadeOutStart;
    ar << FadeOutEnd;
    ar << QueryRadius;
    ar << Enabled;
    ar << OnlyVisibleInSpotLight;

    BYTE iFlares = GetNumberOfUsedFlares();
    ar << iFlares;
    for (BYTE i=0; i < MAX_NUM_LENS_FLARES; ++i)
    {
      if (!m_Flares[i].m_sFlareTexture.IsEmpty())
      {
        ar << i;
        ar << m_Flares[i].m_fPositionOnRay;
        ar << m_Flares[i].m_fScale;
        ar << m_Flares[i].m_sFlareTexture;
      }
    }
  }
}


bool VLensFlareComponent::IsValidCandidate(VisRenderContext_cl* pContext) const
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
  if ( (FadeOutEnd != 0.0f) && (FadeOutEnd * FadeOutEnd < vDistance.getLengthSquared()) )
    return false;

  return true;
}

void VLensFlareComponent::InitFlares()
{
  VLensFlareDescriptor lfd;
  lfd.m_sFlareTexture = "\\flare12.dds";
  lfd.m_fScale = 0.02f;
  lfd.m_fPositionOnRay = 0.298f;
  SetLensFlareDescriptor(0, lfd);

  lfd.m_sFlareTexture = "\\flare11.dds";
    lfd.m_fScale = 0.04f;
  lfd.m_fPositionOnRay = 0.596f;
  SetLensFlareDescriptor(1, lfd);

  lfd.m_sFlareTexture = "\\flare10.dds";
    lfd.m_fScale = 0.07f;
  lfd.m_fPositionOnRay = 0.895f;
  SetLensFlareDescriptor(2, lfd);

  lfd.m_sFlareTexture = "\\flare09.dds";
    lfd.m_fScale = 0.018f;
  lfd.m_fPositionOnRay = 1.193f;
  SetLensFlareDescriptor(3, lfd);

  lfd.m_sFlareTexture = "\\flare08.dds";
    lfd.m_fScale = 0.01f;
  lfd.m_fPositionOnRay = 1.491f;
  SetLensFlareDescriptor(4, lfd);

  lfd.m_sFlareTexture = "\\flare07.dds";
    lfd.m_fScale = 0.01f;
  lfd.m_fPositionOnRay = 1.789f;
  SetLensFlareDescriptor(5, lfd);

  lfd.m_sFlareTexture = "\\flare06.dds";
    lfd.m_fScale = 0.02f;
  lfd.m_fPositionOnRay = 2.097f;
  SetLensFlareDescriptor(6, lfd);

  lfd.m_sFlareTexture = "\\flare05.dds";
    lfd.m_fScale = 0.07f;
  lfd.m_fPositionOnRay = 2.386f;
  SetLensFlareDescriptor(7, lfd);

  lfd.m_sFlareTexture = "\\flare04.dds";
    lfd.m_fScale = 0.07f;
  lfd.m_fPositionOnRay = 2.684f;
  SetLensFlareDescriptor(8, lfd);

  lfd.m_sFlareTexture = "\\flare03.dds";
    lfd.m_fScale = 0.035f;
  lfd.m_fPositionOnRay = 2.982f;
  SetLensFlareDescriptor(9, lfd);

  lfd.m_sFlareTexture = "\\flare02.dds";
    lfd.m_fScale = 0.07f;
  lfd.m_fPositionOnRay = 3.28f;
  SetLensFlareDescriptor(10, lfd);

  lfd.m_sFlareTexture = "\\flare01.dds";
    lfd.m_fScale = 0.14f;
  lfd.m_fPositionOnRay = 3.578f;
  SetLensFlareDescriptor(11, lfd);

  lfd.m_sFlareTexture = "\\flare12.dds";
    lfd.m_fScale = 0.02f;
  lfd.m_fPositionOnRay = 3.877f;
  SetLensFlareDescriptor(12, lfd);

  lfd.m_sFlareTexture = "\\flare11.dds";
    lfd.m_fScale = 0.04f;
  lfd.m_fPositionOnRay = 4.175f;
  SetLensFlareDescriptor(13, lfd);

  lfd.m_sFlareTexture = "\\flare10.dds";
    lfd.m_fScale = 0.075f;
  lfd.m_fPositionOnRay = 4.473f;
  SetLensFlareDescriptor(14, lfd);

  lfd.m_sFlareTexture = "";
    lfd.m_fScale = 0.1f;
  lfd.m_fPositionOnRay = 4.8f;
  SetLensFlareDescriptor(15, lfd);
}

BYTE VLensFlareComponent::GetNumberOfUsedFlares() const
{
  BYTE iUsedFlares = 0;
  BYTE iFlares = MAX_NUM_LENS_FLARES;
  for (BYTE i=0; i < iFlares; ++i)
  {
    if (!m_Flares[i].m_sFlareTexture.IsEmpty())
      ++iUsedFlares;
  }
  return iUsedFlares;
}

START_VAR_TABLE(VLensFlareComponent,IVObjectComponent,"Lens flare component",VVARIABLELIST_FLAGS_NONE, "Lens Flare" )
  DEFINE_VAR_BOOL(VLensFlareComponent, Enabled, "Enable or disable component", "TRUE", 0, NULL);

  DEFINE_CATEGORY (VLensFlareComponent, "LensFlareProperties")
    varList->Add (VisVariable_cl::NewVariable ("QueryRadius", "The size in pixels if the occlusion query window", VULPTYPE_UINT, -1, "8", (VDisplayHintFlags_e)0, "Clamp(1,255)", 0) );
    DEFINE_VAR_UINT(VLensFlareComponent, PreGlowMS, "Fade-in time in ms. If specified, the effect will not appear immediately, but fade in smoothly.", "0", 0, 0);
    DEFINE_VAR_UINT(VLensFlareComponent, AfterGlowMS, "Fade-out time in ms.If specified, the effect will fade out smoothly after disappearing.", "0", 0, 0);
    DEFINE_VAR_FLOAT(VLensFlareComponent, DepthBias, "Depth bias for occlusion test and corona-type flare rendering. Positive values will move the query closer to the camera. Negative values will move the query away from the camera.", "0", 0, 0);
    DEFINE_VAR_FLOAT(VLensFlareComponent, FadeOutStart, "The distance (in world units) at which the effect has full intensity.", "0", 0, 0);
    DEFINE_VAR_FLOAT(VLensFlareComponent, FadeOutEnd, "The distance (in world units) at which the effect has no intensity. Set to zero to disable fading.", "0", 0, 0);
    DEFINE_VAR_BOOL(VLensFlareComponent, OnlyVisibleInSpotLight, "This property only influences the Lenseflare, when it is attached to a spotlight. If it is true, the Corona is only visible if the camera is inside the cone defined by the spotlight. Please also note that the projected texture is not used to calculate the intensity of the corona. Instead a simple falloff function is used.", 0, 0, 0);

  DEFINE_CATEGORY (VLensFlareComponent, "LensFlares")
    varList->Add (VisVariable_cl::NewVariable ("FlareTexture[0]", "Filename", VULPTYPE_VSTRING, -1, "\\flare12.dds", (VDisplayHintFlags_e)DISPLAY_HINT_TEXTUREFILE, 0, 0)  );
    varList->Add (VisVariable_cl::NewVariable ("Scale[0]", "Scale of the flare relative to the window height. A scale of 1.0 results in a flare with the same size as the window height.", VULPTYPE_FLOAT, -1, "0.02", (VDisplayHintFlags_e)0, "Clamp(0.001, 1.0)", 0)  );
    varList->Add (VisVariable_cl::NewVariable ("PositionOnRay[0]", "Position on the ray between light position (0.0) and image center (1.0). Values can be greater than 1.0 to move beyond the image center.", VULPTYPE_FLOAT, -1, "0.298", (VDisplayHintFlags_e)0, 0, 0)  );

    varList->Add (VisVariable_cl::NewVariable ("FlareTexture[1]", "Filename", VULPTYPE_VSTRING, -1, "\\flare11.dds", (VDisplayHintFlags_e)DISPLAY_HINT_TEXTUREFILE, 0, 0)  );
    varList->Add (VisVariable_cl::NewVariable ("Scale[1]", "Scale of the flare relative to the window height. A scale of 1.0 results in a flare with the same size as the window height.", VULPTYPE_FLOAT, -1, "0.04", (VDisplayHintFlags_e)0, "Clamp(0.001, 1.0)", 0)  );
    varList->Add (VisVariable_cl::NewVariable ("PositionOnRay[1]", "Position on the ray between light position (0.0) and image center (1.0). Values can be greater than 1.0 to move beyond the image center.", VULPTYPE_FLOAT, -1, "0.596", (VDisplayHintFlags_e)0, 0, 0)  );

    varList->Add (VisVariable_cl::NewVariable ("FlareTexture[2]", "Filename", VULPTYPE_VSTRING, -1, "\\flare10.dds", (VDisplayHintFlags_e)DISPLAY_HINT_TEXTUREFILE, 0, 0)  );
    varList->Add (VisVariable_cl::NewVariable ("Scale[2]", "Scale of the flare relative to the window height. A scale of 1.0 results in a flare with the same size as the window height.", VULPTYPE_FLOAT, -1, "0.07", (VDisplayHintFlags_e)0, "Clamp(0.001, 1.0)", 0)  );
    varList->Add (VisVariable_cl::NewVariable ("PositionOnRay[2]", "Position on the ray between light position (0.0) and image center (1.0). Values can be greater than 1.0 to move beyond the image center.", VULPTYPE_FLOAT, -1, "0.895", (VDisplayHintFlags_e)0, 0, 0)  );

    varList->Add (VisVariable_cl::NewVariable ("FlareTexture[3]", "Filename", VULPTYPE_VSTRING, -1, "\\flare09.dds", (VDisplayHintFlags_e)DISPLAY_HINT_TEXTUREFILE, 0, 0)  );
    varList->Add (VisVariable_cl::NewVariable ("Scale[3]", "Scale of the flare relative to the window height. A scale of 1.0 results in a flare with the same size as the window height.", VULPTYPE_FLOAT, -1, "0.018", (VDisplayHintFlags_e)0, "Clamp(0.001, 1.0)", 0)  );
    varList->Add (VisVariable_cl::NewVariable ("PositionOnRay[3]", "Position on the ray between light position (0.0) and image center (1.0). Values can be greater than 1.0 to move beyond the image center.", VULPTYPE_FLOAT, -1, "1.193", (VDisplayHintFlags_e)0, 0, 0)  );

    varList->Add (VisVariable_cl::NewVariable ("FlareTexture[4]", "Filename", VULPTYPE_VSTRING, -1, "\\flare08.dds", (VDisplayHintFlags_e)DISPLAY_HINT_TEXTUREFILE, 0, 0)  );
    varList->Add (VisVariable_cl::NewVariable ("Scale[4]", "Scale of the flare relative to the window height. A scale of 1.0 results in a flare with the same size as the window height.", VULPTYPE_FLOAT, -1, "0.01", (VDisplayHintFlags_e)0, "Clamp(0.001, 1.0)", 0)  );
    varList->Add (VisVariable_cl::NewVariable ("PositionOnRay[4]", "Position on the ray between light position (0.0) and image center (1.0). Values can be greater than 1.0 to move beyond the image center.", VULPTYPE_FLOAT, -1, "1.491", (VDisplayHintFlags_e)0, 0, 0)  );

    varList->Add (VisVariable_cl::NewVariable ("FlareTexture[5]", "Filename", VULPTYPE_VSTRING, -1, "\\flare07.dds", (VDisplayHintFlags_e)DISPLAY_HINT_TEXTUREFILE, 0, 0)  );
    varList->Add (VisVariable_cl::NewVariable ("Scale[5]", "Scale of the flare relative to the window height. A scale of 1.0 results in a flare with the same size as the window height.", VULPTYPE_FLOAT, -1, "0.01", (VDisplayHintFlags_e)0, "Clamp(0.001, 1.0)", 0)  );
    varList->Add (VisVariable_cl::NewVariable ("PositionOnRay[5]", "Position on the ray between light position (0.0) and image center (1.0). Values can be greater than 1.0 to move beyond the image center.", VULPTYPE_FLOAT, -1, "1.789", (VDisplayHintFlags_e)0, 0, 0)  );

    varList->Add (VisVariable_cl::NewVariable ("FlareTexture[6]", "Filename", VULPTYPE_VSTRING, -1, "\\flare06.dds", (VDisplayHintFlags_e)DISPLAY_HINT_TEXTUREFILE, 0, 0)  );
    varList->Add (VisVariable_cl::NewVariable ("Scale[6]", "Scale of the flare relative to the window height. A scale of 1.0 results in a flare with the same size as the window height.", VULPTYPE_FLOAT, -1, "0.02", (VDisplayHintFlags_e)0, "Clamp(0.001, 1.0)", 0)  );
    varList->Add (VisVariable_cl::NewVariable ("PositionOnRay[6]", "Position on the ray between light position (0.0) and image center (1.0). Values can be greater than 1.0 to move beyond the image center.", VULPTYPE_FLOAT, -1, "2.097", (VDisplayHintFlags_e)0, 0, 0)  );

    varList->Add (VisVariable_cl::NewVariable ("FlareTexture[7]", "Filename", VULPTYPE_VSTRING, -1, "\\flare05.dds", (VDisplayHintFlags_e)DISPLAY_HINT_TEXTUREFILE, 0, 0)  );
    varList->Add (VisVariable_cl::NewVariable ("Scale[7]", "Scale of the flare relative to the window height. A scale of 1.0 results in a flare with the same size as the window height.", VULPTYPE_FLOAT, -1, "0.07", (VDisplayHintFlags_e)0, "Clamp(0.001, 1.0)", 0)  );
    varList->Add (VisVariable_cl::NewVariable ("PositionOnRay[7]", "Position on the ray between light position (0.0) and image center (1.0). Values can be greater than 1.0 to move beyond the image center.", VULPTYPE_FLOAT, -1, "2.386", (VDisplayHintFlags_e)0, 0, 0)  );

    varList->Add (VisVariable_cl::NewVariable ("FlareTexture[8]", "Filename", VULPTYPE_VSTRING, -1, "\\flare04.dds", (VDisplayHintFlags_e)DISPLAY_HINT_TEXTUREFILE, 0, 0)  );
    varList->Add (VisVariable_cl::NewVariable ("Scale[8]", "Scale of the flare relative to the window height. A scale of 1.0 results in a flare with the same size as the window height.", VULPTYPE_FLOAT, -1, "0.07", (VDisplayHintFlags_e)0, "Clamp(0.001, 1.0)", 0)  );
    varList->Add (VisVariable_cl::NewVariable ("PositionOnRay[8]", "Position on the ray between light position (0.0) and image center (1.0). Values can be greater than 1.0 to move beyond the image center.", VULPTYPE_FLOAT, -1, "2.684", (VDisplayHintFlags_e)0, 0, 0)  );

    varList->Add (VisVariable_cl::NewVariable ("FlareTexture[9]", "Filename", VULPTYPE_VSTRING, -1, "\\flare03.dds", (VDisplayHintFlags_e)DISPLAY_HINT_TEXTUREFILE, 0, 0)  );
    varList->Add (VisVariable_cl::NewVariable ("Scale[9]", "Scale of the flare relative to the window height. A scale of 1.0 results in a flare with the same size as the window height.", VULPTYPE_FLOAT, -1, "0.035", (VDisplayHintFlags_e)0, "Clamp(0.001, 1.0)", 0)  );
    varList->Add (VisVariable_cl::NewVariable ("PositionOnRay[9]", "Position on the ray between light position (0.0) and image center (1.0). Values can be greater than 1.0 to move beyond the image center.", VULPTYPE_FLOAT, -1, "2.982", (VDisplayHintFlags_e)0, 0, 0)  );

    varList->Add (VisVariable_cl::NewVariable ("FlareTexture[10]", "Filename", VULPTYPE_VSTRING, -1, "\\flare02.dds", (VDisplayHintFlags_e)DISPLAY_HINT_TEXTUREFILE, 0, 0)  );
    varList->Add (VisVariable_cl::NewVariable ("Scale[10]", "Scale of the flare relative to the window height. A scale of 1.0 results in a flare with the same size as the window height.", VULPTYPE_FLOAT, -1, "0.07", (VDisplayHintFlags_e)0, "Clamp(0.001, 1.0)", 0)  );
    varList->Add (VisVariable_cl::NewVariable ("PositionOnRay[10]", "Position on the ray between light position (0.0) and image center (1.0). Values can be greater than 1.0 to move beyond the image center.", VULPTYPE_FLOAT, -1, "3.28", (VDisplayHintFlags_e)0, 0, 0)  );

    varList->Add (VisVariable_cl::NewVariable ("FlareTexture[11]", "Filename", VULPTYPE_VSTRING, -1, "\\flare01.dds", (VDisplayHintFlags_e)DISPLAY_HINT_TEXTUREFILE, 0, 0)  );
    varList->Add (VisVariable_cl::NewVariable ("Scale[11]", "Scale of the flare relative to the window height. A scale of 1.0 results in a flare with the same size as the window height.", VULPTYPE_FLOAT, -1, "0.14", (VDisplayHintFlags_e)0, "Clamp(0.001, 1.0)", 0)  );
    varList->Add (VisVariable_cl::NewVariable ("PositionOnRay[11]", "Position on the ray between light position (0.0) and image center (1.0). Values can be greater than 1.0 to move beyond the image center.", VULPTYPE_FLOAT, -1, "3.578", (VDisplayHintFlags_e)0, 0, 0)  );

    varList->Add (VisVariable_cl::NewVariable ("FlareTexture[12]", "Filename", VULPTYPE_VSTRING, -1, "\\flare12.dds", (VDisplayHintFlags_e)DISPLAY_HINT_TEXTUREFILE, 0, 0)  );
    varList->Add (VisVariable_cl::NewVariable ("Scale[12]", "Scale of the flare relative to the window height. A scale of 1.0 results in a flare with the same size as the window height.", VULPTYPE_FLOAT, -1, "0.02", (VDisplayHintFlags_e)0, "Clamp(0.001, 1.0)", 0)  );
    varList->Add (VisVariable_cl::NewVariable ("PositionOnRay[12]", "Position on the ray between light position (0.0) and image center (1.0). Values can be greater than 1.0 to move beyond the image center.", VULPTYPE_FLOAT, -1, "3.877", (VDisplayHintFlags_e)0, 0, 0)  );

    varList->Add (VisVariable_cl::NewVariable ("FlareTexture[13]", "Filename", VULPTYPE_VSTRING, -1, "\\flare11.dds", (VDisplayHintFlags_e)DISPLAY_HINT_TEXTUREFILE, 0, 0)  );
    varList->Add (VisVariable_cl::NewVariable ("Scale[13]", "Scale of the flare relative to the window height. A scale of 1.0 results in a flare with the same size as the window height.", VULPTYPE_FLOAT, -1, "0.04", (VDisplayHintFlags_e)0, "Clamp(0.001, 1.0)", 0)  );
    varList->Add (VisVariable_cl::NewVariable ("PositionOnRay[13]", "Position on the ray between light position (0.0) and image center (1.0). Values can be greater than 1.0 to move beyond the image center.", VULPTYPE_FLOAT, -1, "4.175", (VDisplayHintFlags_e)0, 0, 0)  );

    varList->Add (VisVariable_cl::NewVariable ("FlareTexture[14]", "Filename", VULPTYPE_VSTRING, -1, "\\flare10.dds", (VDisplayHintFlags_e)DISPLAY_HINT_TEXTUREFILE, 0, 0)  );
    varList->Add (VisVariable_cl::NewVariable ("Scale[14]", "Scale of the flare relative to the window height. A scale of 1.0 results in a flare with the same size as the window height.", VULPTYPE_FLOAT, -1, "0.075", (VDisplayHintFlags_e)0, "Clamp(0.001, 1.0)", 0)  );
    varList->Add (VisVariable_cl::NewVariable ("PositionOnRay[14]", "Position on the ray between light position (0.0) and image center (1.0). Values can be greater than 1.0 to move beyond the image center.", VULPTYPE_FLOAT, -1, "4.473", (VDisplayHintFlags_e)0, 0, 0)  );

    varList->Add (VisVariable_cl::NewVariable ("FlareTexture[15]", "Filename", VULPTYPE_VSTRING, -1, "", (VDisplayHintFlags_e)DISPLAY_HINT_TEXTUREFILE, 0, 0)  );
    varList->Add (VisVariable_cl::NewVariable ("Scale[15]", "Scale of the flare relative to the window height. A scale of 1.0 results in a flare with the same size as the window height.", VULPTYPE_FLOAT, -1, "0.1", (VDisplayHintFlags_e)0, "Clamp(0.001, 1.0)", 0)  );
    varList->Add (VisVariable_cl::NewVariable ("PositionOnRay[15]", "Position on the ray between light position (0.0) and image center (1.0). Values can be greater than 1.0 to move beyond the image center.", VULPTYPE_FLOAT, -1, "4.8", (VDisplayHintFlags_e)0, 0, 0)  );
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
