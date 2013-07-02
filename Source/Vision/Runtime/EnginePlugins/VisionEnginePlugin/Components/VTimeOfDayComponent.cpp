/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Components/VTimeOfDayComponent.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

VTimeOfDayComponent::VTimeOfDayComponent(int iDeprecated, int iComponentFlags)
  : IVObjectComponent(0, iComponentFlags)
{
  AttachmentType = TIMEOFDAY_ATTACHMENT_SUNLIGHTSOURCE;
  Intensity = 1.0f;

  m_bIsLightClass = false;
  Vision::Callbacks.OnUpdateSceneFinished += this;
}

VTimeOfDayComponent::~VTimeOfDayComponent()
{
  Vision::Callbacks.OnUpdateSceneFinished -= this;
}

BOOL VTimeOfDayComponent::CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
  if (pObject->IsOfType(V_RUNTIME_CLASS(VisObject3D_cl)))
    return TRUE;

  return FALSE;
}

void VTimeOfDayComponent::SetOwner(VisTypedEngineObject_cl *pOwner)
{
  IVObjectComponent::SetOwner(pOwner);
  m_bIsLightClass = pOwner!=NULL && pOwner->IsOfType(V_RUNTIME_CLASS(VisLightSource_cl));
  if (m_bIsLightClass)
    m_iColor = ((VisLightSource_cl*)pOwner)->GetColor();
}

void VTimeOfDayComponent::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (GetOwner()!=NULL)
    UpdateParent();
}

void VTimeOfDayComponent::UpdateParent()
{
  IVTimeOfDay *pTimeOfDayInterface = Vision::Renderer.GetTimeOfDayHandler();
  if (pTimeOfDayInterface == NULL)
    return;

  VisObject3D_cl *pOwnerObject = (VisObject3D_cl *)m_pOwner;
  VASSERT(pOwnerObject);

  hkvVec3 vDirection(hkvNoInitialization);
  pTimeOfDayInterface->GetSunDirection(vDirection);
  vDirection.normalizeIfNotZero();

  if (AttachmentType == TIMEOFDAY_ATTACHMENT_MOONLIGHTSOURCE)
  {
    vDirection = -vDirection; 
  }
  else if(AttachmentType == TIMEOFDAY_ATTACHMENT_SUNBACKLIGHTSOURCE)
  {
    vDirection.x = -vDirection.x;
    vDirection.y = -vDirection.y;
  }
  
  if (AttachmentType != TIMEOFDAY_ATTACHMENT_ENABLEDATNIGHTLIGHTSOURCE)
  {
    pOwnerObject->SetDirection(vDirection);
  }

  if (AttachmentType == TIMEOFDAY_ATTACHMENT_CORONALIGHTSOURCE)
  {
    // TODO (multiple renderer nodes)
    IVRendererNode *pRenderer = Vision::Renderer.GetRendererNode(0);
    float fNear, fFar;
    pRenderer->GetReferenceContext()->GetClipPlanes(fNear, fFar);
    hkvVec3 vCamPos = pRenderer->GetReferenceContext()->GetCamera()->GetPosition();

    hkvVec3 vCoronaPos = -vDirection;
    vCoronaPos *= 0.95f * fFar;
    vCoronaPos += vCamPos;
    pOwnerObject->SetPosition(vCoronaPos);
  }

  if (m_bIsLightClass)
  {
    VisLightSource_cl *pLight = (VisLightSource_cl*)m_pOwner;
    VColorRef sunColor = pTimeOfDayInterface->GetSunColor();

    bool bSwitchable = (AttachmentType == TIMEOFDAY_ATTACHMENT_ENABLEDATNIGHTLIGHTSOURCE);
    float fBelowHorizonMultiplier = hkvMath::pow (hkvMath::Max(-vDirection.z+0.1f, 0.0f), bSwitchable ? 1.0f : 0.1f);
    fBelowHorizonMultiplier = hkvMath::Min(1.0f, fBelowHorizonMultiplier);

    if (bSwitchable && fBelowHorizonMultiplier < 1.0f && fBelowHorizonMultiplier > 0.f)
    {
      pLight->SetColor(m_iColor);
      pLight->SetMultiplier(Intensity * (1.0f - fBelowHorizonMultiplier));
    }
    else if (AttachmentType == TIMEOFDAY_ATTACHMENT_SUNLIGHTSOURCE)
    {
      pLight->SetColor(sunColor);
      pLight->SetMultiplier(Intensity * fBelowHorizonMultiplier);
    }
    else if ((AttachmentType == TIMEOFDAY_ATTACHMENT_MOONLIGHTSOURCE) ||
             (AttachmentType == TIMEOFDAY_ATTACHMENT_SUNBACKLIGHTSOURCE))
    {
      // TODO
      VColorRef negativeColor = V_RGBA_WHITE - sunColor;
      pLight->SetColor(negativeColor);
      pLight->SetMultiplier(Intensity * fBelowHorizonMultiplier * 0.333f);
    }  
    else if (AttachmentType == TIMEOFDAY_ATTACHMENT_CORONALIGHTSOURCE)
    {
      hkvVec3 vSunColorFloat = sunColor.ToFloat();
      float fLargestComponent = hkvMath::Max(hkvMath::Max(vSunColorFloat.x, vSunColorFloat.y), vSunColorFloat.z);
      if (fLargestComponent <= 0.0f)
        fLargestComponent = 1.0f;
      sunColor.FromFloat(vSunColorFloat * (1.0f / fLargestComponent));

      pLight->SetColor(sunColor * fBelowHorizonMultiplier);
      pLight->SetMultiplier(0.0f);
    }
  }
}



V_IMPLEMENT_SERIAL(VTimeOfDayComponent,IVObjectComponent,0,&g_VisionEngineModule);
void VTimeOfDayComponent::Serialize( VArchive &ar )
{
  char iLocalVersion = TIMEOFDAY_ATTACHMENT_VERSION_CURRENT;
  IVObjectComponent::Serialize(ar);
  if (ar.IsLoading())
  {
    ar >> iLocalVersion;
    VASSERT_MSG(iLocalVersion <= TIMEOFDAY_ATTACHMENT_VERSION_CURRENT,"TimeOfDay: Invalid version number");

    int iAttachmentType;
    ar >> iAttachmentType;
    AttachmentType = (VTimeOfDayAttachmentType_e)iAttachmentType;

    if(iLocalVersion>TIMEOFDAY_ATTACHMENT_VERSION_0)
      ar >> Intensity;
    else
      Intensity = 1.0f;
  } 
  else
  {
    ar << iLocalVersion;

    int iAttachmentType;
    iAttachmentType = (int)AttachmentType;
    ar << iAttachmentType;

    ar <<Intensity;
  }
}

START_VAR_TABLE(VTimeOfDayComponent, IVObjectComponent, "Time of Day Attachment Component. Can be attached to light sources (and other positional objects) in order to allow the object to obtain its position, direction, color, and intensity values (where applicable) from the global time of day object.", VVARIABLELIST_FLAGS_NONE, "Time of Day Attachment Component" )
  DEFINE_VAR_ENUM(VTimeOfDayComponent, AttachmentType, "Use as sun light, moon light, corona, sun-backlight or enabled at night light.", "SUNLIGHT", "SUNLIGHT,MOONLIGHT,CORONA,SUNBACKLIGHT,ENABLEDATNIGHT", 0, 0);
  DEFINE_VAR_FLOAT(VTimeOfDayComponent, Intensity, "Intensity of the light source (lights only)", "1.0", 0,0);
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
