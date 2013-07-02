/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/VisionManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/EngineInstances/EngineInstanceDynLight.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/IVShadowMapComponent.hpp>

#if defined( HK_ANARCHY )
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VMobileShadowMapComponentSpotDirectional.hpp>
  #define SHADOW_MAP_COMPONENT_SPOT_DIRECTIONAL VMobileShadowMapComponentSpotDirectional
#else
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VShadowMapComponentSpotDirectional.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VShadowMapComponentPoint.hpp>
  #define SHADOW_MAP_COMPONENT_SPOT_DIRECTIONAL VShadowMapComponentSpotDirectional
#endif

#using <mscorlib.dll>
using namespace ManagedFramework;
using namespace System::Diagnostics;


namespace VisionManaged
{

  EngineInstanceDynLight::EngineInstanceDynLight()
  {
    m_bEnabled = true;
    m_iVisibleBitmask = 0xffffffff;
    m_pColorLookUpFile = new VString(); // just a member is not allowed
    m_iColor = V_RGBA_WHITE.GetRGBA();
    m_fIntensity = 1000.f;
    m_fAnimTime = 1.f;
    m_fAnimPhase = 0.f;
    m_fCoronaScale = 1.0f;
    m_eCoronaFlags = CoronaScaleModeFlags_e::Rotate;
    m_bAnimated = false;
    m_bSceneAnimated = true;
    m_bExportAsStatic = false;
    m_bPreviewStatic = true;
    m_bExportWhenNotRelevant = false;
    m_bShowCorona = false;
    m_pLight = new VisLightSource_cl(VIS_LIGHT_POINT, 0.f);
    m_pLight->AddRef();

    // add trigger target components - only inside vForge. They are only serialized when used.
    VisTriggerTargetComponent_cl *pComp;

    // pause target
    pComp = new VisTriggerTargetComponent_cl(VISLIGHTSOURCETRIGGER_TOGGLEON, VIS_OBJECTCOMPONENTFLAG_SERIALIZEWHENRELEVANT);
    m_pLight->AddComponent(pComp);

    // unpause target
    pComp = new VisTriggerTargetComponent_cl(VISLIGHTSOURCETRIGGER_TOGGLEOFF, VIS_OBJECTCOMPONENTFLAG_SERIALIZEWHENRELEVANT);
    m_pLight->AddComponent(pComp);
  }

  void EngineInstanceDynLight::DisposeObject()
  {
    V_SAFE_DELETE(m_pColorLookUpFile);
    V_SAFE_DISPOSEANDRELEASE(m_pLight);
  }

  void *EngineInstanceDynLight::GetObject3D()
  {
    VisObject3D_cl *pObj3D = (VisObject3D_cl *) m_pLight;
    return pObj3D;
  }

  void EngineInstanceDynLight::SetAnimateScene(bool bStatus)
  {
    // m_bSceneAnimated = bStatus; first implement the event listener in the dynlight shape
    UpdateAnimation();
  }

  void EngineInstanceDynLight::UpdateVisibleStatus() 
  {
    if (m_pLight==nullptr)
      return;

    m_pLight->SetVisibleBitmask(m_bEnabled ? m_iVisibleBitmask:0);
    m_pLight->FlagAsStatic(m_bExportAsStatic && !m_bPreviewStatic);    
    UpdateAnimation();
  }


  void EngineInstanceDynLight::HighlightShape(VisionViewBase ^view, float fSpotAngle)
  {
    IVRenderInterface *pRenderer = (static_cast<VisionView ^>(view))->GetRenderInterface();

    hkvMat3 rotMat;
    hkvVec3 vDir, vRight, vUp;
    m_pLight->GetRotationMatrix(rotMat);
    rotMat.getAxisXYZ(&vDir, &vRight, &vUp);

    hkvVec3 vPos = m_pLight->GetPosition();
    float fRange = m_fIntensity;//m_pLight->GetRadius(); // engine's radius might be 0.0 for static lights
    VColorRef iColor;
    iColor.SetRGBA(m_iColor);
    iColor.a = 80;
    VSimpleRenderState_t state(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_FRONTFACE);

    int iFlags = RENDERSHAPEFLAGS_LINES/*|RENDERSHAPEFLAGS_SOLID*/;

    VisLightSourceType_e eType = m_pLight->GetType();

    if (eType==VIS_LIGHT_SPOTLIGHT) // spot or projected
    {
      if (m_pLight->GetProjectionTexture()!=nullptr)
        pRenderer->RenderFrustum((hkvVec3& )vPos,(hkvVec3& )vDir,(hkvVec3& )vUp,fSpotAngle,fSpotAngle, 0.f, fRange, iColor,state,iFlags);
      else
        pRenderer->RenderCone((hkvVec3& )vPos,(hkvVec3& )vDir,fSpotAngle,fRange,iColor,state,iFlags);
    }
    else if (eType==VIS_LIGHT_POINT) // point
    {
      pRenderer->RenderSphere((hkvVec3& )vPos,fRange,iColor,state,iFlags);
    }
    else if (eType==VIS_LIGHT_DIRECTED)
    {
      float fUnitScaling = EditorManager::Settings->GlobalUnitScaling;
      for (int x=-4;x<=4;x++)
        for (int y=-4;y<=4;y++)
        {
          hkvVec3 vStart = vPos + vRight*((float)x*25.f*fUnitScaling) + vUp*((float)y*25.f*fUnitScaling);
          hkvVec3 vEnd = vStart + vDir*200.f*fUnitScaling;
          pRenderer->DrawLine((hkvVec3& )vStart,(hkvVec3& )vEnd,iColor,1.f);
        }
    }
  }

  void EngineInstanceDynLight::TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result)
  {
    ShapeTraceResult ^pResult = result;
    hkvVec3 vStart(rayStart.X,rayStart.Y,rayStart.Z);
    hkvVec3 vEnd(rayEnd.X,rayEnd.Y,rayEnd.Z);
  }



  bool EngineInstanceDynLight::GetLocalBoundingBox(BoundingBox ^%bbox)
  {
    float fSize = 30.f * EditorManager::Settings->GlobalUnitScaling;
    (*bbox).Set(-fSize,-fSize,-fSize,fSize,fSize,fSize);
    return true;
  }

  void EngineInstanceDynLight::SetLightType(LightSourceType_e eType, float fSpotAngle, String ^sProjectedTex)
  {
    VString sFilename;
    ConversionUtils::StringToVString(sProjectedTex, sFilename);

    const char *szFilename = nullptr;
    if (!sFilename.IsEmpty() && sFilename!="default")
      szFilename = sFilename.AsChar();

    VisLightSourceType_e nativeType = VIS_LIGHT_POINT;
    switch (eType)
    {
      case LightSourceType_e::Point : nativeType = VIS_LIGHT_POINT;break;
      case LightSourceType_e::Spotlight : nativeType = VIS_LIGHT_SPOTLIGHT;break;
      case LightSourceType_e::Directional : nativeType = VIS_LIGHT_DIRECTED;break;
    }

    m_pLight->SetType(nativeType);
    m_pLight->SetProjectionAngle(fSpotAngle);
    m_pLight->SetProjectionTexture(szFilename);
  }

  /*
  void EngineInstanceDynLight::SetLightKey(String ^sKey)
  {
    VString sNewKey;
    ConversionUtils::StringToVString(sKey,sNewKey);
    if (sNewKey.IsEmpty())
      m_pLight->SetKey(NULL);
    else
      m_pLight->SetKey(sNewKey);
  }
*/


  void EngineInstanceDynLight::SetAnimationLookup(String ^sTextureFile)
  {
    ConversionUtils::StringToVString(sTextureFile, *m_pColorLookUpFile);
    if (!m_pColorLookUpFile || m_pColorLookUpFile->IsEmpty() || (*m_pColorLookUpFile)=="none")
      m_bAnimated = false;
    else
      m_bAnimated = true;

    UpdateAnimation();
  }

  void EngineInstanceDynLight::SetAnimationTiming(float fTime)
  {
    m_fAnimTime = fTime;
    UpdateAnimation();
  }

  void EngineInstanceDynLight::SetAnimationPhase(float fPhase)
  {
    m_fAnimPhase = fPhase;
    UpdateAnimation();
  }

  void EngineInstanceDynLight::SetRemoveLightWhenFinished(bool bState)
  {
    m_bRemoveLightAfterAnim = bState;
    UpdateAnimation();
  }

  void EngineInstanceDynLight::UpdateAnimation()
  {
    if ( !m_pLight )
      return;

    if (m_bAnimated)
    {
      float fPhase = m_fAnimPhase*(1.f/100.f); // percentage value
      m_pLight->EnableColorAnimations();
      VisLightSrc_AnimColor_cl *pAnim = m_pLight->GetColorAnimation();
      pAnim->SetAnimCurve(*m_pColorLookUpFile,m_bSceneAnimated ? m_fAnimTime : 0.f,fPhase);
      pAnim->SetAnimPhaseShift(fPhase);
      pAnim->SetAnimMaxIntensity(m_bEnabled ? m_fIntensity:0.f);
      pAnim->SetRemoveLightWhenFinished(m_bRemoveLightAfterAnim);
      VColorRef iColor;
      iColor.SetRGBA(m_iColor);
      pAnim->SetAnimColor(iColor);
    } else
    {
      m_pLight->DisableColorAnimations();
    }
  }


  void EngineInstanceDynLight::SetIntensity(float fIntensity)
  { 
    m_fIntensity=fIntensity;
    if (m_pLight)
      m_pLight->SetIntensity(m_fIntensity);
  }

  void EngineInstanceDynLight::SetAttenuation(LightAttenuationType_e eMode, String ^sTextureFile)
  {
    m_pLight->SetAttenuation((VisLightAttenuationType_e)eMode);
    if (eMode==LightAttenuationType_e::ATTEN_CUSTOM)
    {
      VString sFilename;
      ConversionUtils::StringToVString(sTextureFile, sFilename);
      m_pLight->SetAttenuationTexture(sFilename);
    }
  }
  
  void EngineInstanceDynLight::SetMultiplier(float fMultiplier)
  {
    m_pLight->SetMultiplier(fMultiplier);
  }

  void EngineInstanceDynLight::SetCastShadows(bool bWorld, bool bModel) 
  {
    //FIXME if (m_pLight) m_pLight->SetFlag_CastShadows(bWorld,bModel);
  }

  bool EngineInstanceDynLight::OnExport(SceneExportInfo ^info) 
  {
    if (!m_pLight)
      return true;

    Debug::Assert( m_pLight != nullptr );
    VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());

    if (m_bExportAsStatic) // do some special treatment
    {
      bool bForceSave = m_bExportWhenNotRelevant;

      if (m_pLight->IsRelevantForExport ())
        bForceSave = true;
      // the following properties cause a light source to be exported:
      if (m_bAnimated)
        bForceSave = true;

      if (bForceSave)
      {
        UpdateAnimation();
        m_pLight->FlagAsStatic(m_bExportAsStatic); // ignore the preview flag
        ar << m_pLight;
        UpdateVisibleStatus();
      }
    }
    else
    {
      ar << m_pLight;
    }

    return true;
  }


  bool EngineInstanceDynLight::IsOccluderForPointLight(Vector3F pos, float fRadius, BoundingBox ^occluderBox, BoundingBox ^receiverBox)
  {
    hkvVec3 vPos(pos.X,pos.Y,pos.Z);
    hkvAlignedBBox occBBox (hkvVec3 (occluderBox->X1,occluderBox->Y1,occluderBox->Z1), hkvVec3 (occluderBox->X2,occluderBox->Y2,occluderBox->Z2));
    hkvAlignedBBox recBBox (hkvVec3 (receiverBox->X1,receiverBox->Y1,receiverBox->Z1), hkvVec3 (receiverBox->X2,receiverBox->Y2,receiverBox->Z2));

    // overlapping boxes -> affects
    if (occBBox.overlaps (recBBox))
      return true;

    // light sticks in the receiver -> never affects
    if (recBBox.contains (vPos))
      return false;

    // light sticks in the occluder
    if (occBBox.contains (vPos))
    {
      float fDist = recBBox.getDistanceTo (vPos);
      return fDist<fRadius;
    }

    static VisFrustum_cl frustum;
    frustum.Set(vPos,recBBox,false,fRadius);
    if (frustum.Overlaps(occBBox))
      return true;
    return false;
  }


#if !defined( HK_ANARCHY )
  class VShadowMapComponentPoint_PREVIEW : public VShadowMapComponentPoint
  {
  public:
    VShadowMapComponentPoint_PREVIEW() : VShadowMapComponentPoint(0) {}
    VOVERRIDE bool IsRelevantForSerialization() const {return false;} // we dont want this component to be exported
    V_DECLARE_DYNAMIC(VShadowMapComponentPoint_PREVIEW);
  };
  V_IMPLEMENT_DYNAMIC(VShadowMapComponentPoint_PREVIEW,VShadowMapComponentPoint, Vision::GetEngineModule());
#endif

  class VShadowMapComponentSpotDirectional_PREVIEW : public SHADOW_MAP_COMPONENT_SPOT_DIRECTIONAL
  {
  public:
    VShadowMapComponentSpotDirectional_PREVIEW() : SHADOW_MAP_COMPONENT_SPOT_DIRECTIONAL(0) {}
    VOVERRIDE bool IsRelevantForSerialization() const {return false;} // we dont want this component to be exported
    V_DECLARE_DYNAMIC(VShadowMapComponentSpotDirectional_PREVIEW);
  };
  V_IMPLEMENT_DYNAMIC(VShadowMapComponentSpotDirectional_PREVIEW, SHADOW_MAP_COMPONENT_SPOT_DIRECTIONAL, Vision::GetEngineModule());


  void EngineInstanceDynLight::RemovePreviewComponents(VType *pType)
  {
    if (!m_pLight)
      return;
    
    while (true)
    {
      IVObjectComponent *pComp = m_pLight->Components().GetComponentOfType(pType);
      if (pComp==NULL) break;
      m_pLight->RemoveComponent(pComp);
    }

  }


  void EngineInstanceDynLight::SetUsePreviewShadowComponent(bool bStatus)
  {
    if (!m_pLight)
      return;
    // if a shadowmap component is set by the user, then remove the preview component, regardless of enabled state
    if (!bStatus 
#if !defined( HK_ANARCHY )
      || m_pLight->Components().GetComponentOfType<VShadowMapComponentPoint>()!=NULL 
#endif
      || m_pLight->Components().GetComponentOfType<SHADOW_MAP_COMPONENT_SPOT_DIRECTIONAL>()!=NULL)
    {
#if !defined( HK_ANARCHY )
      RemovePreviewComponents(VShadowMapComponentPoint_PREVIEW::GetClassTypeId());
#endif
      RemovePreviewComponents(VShadowMapComponentSpotDirectional_PREVIEW::GetClassTypeId());
      return;
    }

    if ( m_pLight->GetType() != VIS_LIGHT_POINT )
    {
#if !defined( HK_ANARCHY )
      RemovePreviewComponents(VShadowMapComponentPoint_PREVIEW::GetClassTypeId());
#endif
      VShadowMapComponentSpotDirectional_PREVIEW *pComp = m_pLight->Components().GetComponentOfType<VShadowMapComponentSpotDirectional_PREVIEW>();
      if (pComp==NULL)
      {
        pComp = new VShadowMapComponentSpotDirectional_PREVIEW();
        pComp->SetShadowMapSize(256); // low resolution
        pComp->SetCascadeCount(1);
        m_pLight->AddComponent(pComp);
        if (pComp->GetShadowMapGenerator()!=NULL) // the only reason why this may be NULL is that the renderer failed to initialize (e.g. no renderer node)
          pComp->GetShadowMapGenerator()->SetConsiderCastShadowFlag(false);
      }
    }
#if !defined( HK_ANARCHY )
    else
    {
      RemovePreviewComponents(VShadowMapComponentSpotDirectional_PREVIEW::GetClassTypeId());
      VShadowMapComponentPoint_PREVIEW *pComp = m_pLight->Components().GetComponentOfType<VShadowMapComponentPoint_PREVIEW>();
      if (pComp==NULL)
      {
        pComp = new VShadowMapComponentPoint_PREVIEW();
        pComp->SetShadowMapSize(128); // low resolution
        m_pLight->AddComponent(pComp);
        if (pComp->GetShadowMapGenerator()!=NULL) // the only reason why this may be NULL is that the renderer failed to initialize (e.g. no renderer node)
          pComp->GetShadowMapGenerator()->SetConsiderCastShadowFlag(false);
      }
    } 
#endif
  }

}

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
