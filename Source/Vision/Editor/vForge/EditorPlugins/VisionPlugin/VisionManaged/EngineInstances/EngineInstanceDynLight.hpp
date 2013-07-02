/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace CSharpFramework;
using namespace CSharpFramework::Math;
using namespace CSharpFramework::Shapes;
using namespace ManagedFramework;
using namespace CSharpFramework::Scene;
using namespace CSharpFramework::PropertyEditors;

// include used for the enum
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VShadowMapGenerator.hpp>

namespace VisionManaged
{

  /// <summary>
  /// Light type
  /// </summary>
  public enum class LightSourceType_e
  {
    /// <summary>
    /// Light is an omni light
    /// </summary>
    Point,

    /// <summary>
    /// Light is a spotlight
    /// </summary>
    Spotlight,

    /// <summary>
    /// Light is directional
    /// </summary>
    Directional,
  };

  /// <summary>
  /// Engine constants
  /// </summary>
  public enum class NativeLightSourceType_e
  {
    /// <summary>
    /// Light is an omni light
    /// </summary>
    Point = VIS_LIGHT_POINT,

    /// <summary>
    /// Light is a spotlight
    /// </summary>
    Spotlight = VIS_LIGHT_SPOTLIGHT,

    /// <summary>
    /// Light is directional
    /// </summary>
    Directional =VIS_LIGHT_DIRECTED,
  };

 
  [Flags]
  public enum class CoronaScaleModeFlags_e // matches VisCoronaScaleMode_e
  {
    None = VIS_CORONASCALE_NONE,
    Distance = VIS_CORONASCALE_DISTANCE,
    VisibleArea = VIS_CORONASCALE_VISIBLEAREA,
    GlobalFadeOut = VIS_CORONASCALE_USEFADEOUT,
    Rotate = VIS_CORONASCALE_ROTATING
  };

  
  [Flags, EditorAttribute(FlagsEditor::typeid, System::Drawing::Design::UITypeEditor::typeid)]
  public enum class ShadowCasterGeometryTypes_e // matches VShadowCastingGeometry_e, used for the VARTABLE entry
  {
    [NonEditableFlag]
    None = 0,

    Terrain = SHADOW_CASTER_TERRAIN,
    StaticMeshes = SHADOW_CASTER_STATICMESHES,
    Entities = SHADOW_CASTER_ENTITIES,
    Speedtrees = SHADOW_CASTER_SPEEDTREES,

    [NonEditableFlag]
    All = Terrain|StaticMeshes|Entities|Speedtrees
  };


  public ref class EngineInstanceDynLight : public IEngineInstanceObject3D
  {
  public:     
    enum class LightAttenuationType_e
    {
      ATTEN_1OVERX = 0,
      ATTEN_CUSTOM = 1
    };
    
    EngineInstanceDynLight();
    VOVERRIDE void DisposeObject() override;

    // overridden IEngineShapeInstance functions
    VOVERRIDE void SetVisible(bool bStatus) override  
    {
      m_bEnabled=bStatus;
      UpdateVisibleStatus();
    }


    VOVERRIDE void SetScaling(float x,float y, float z) override  {}

    VOVERRIDE void TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result) override;
    VOVERRIDE bool GetLocalBoundingBox(BoundingBox ^%bbox) override;
    VOVERRIDE bool OnExport(SceneExportInfo ^info) override;

    // Overridden IEngineInstanceObject3D functions
    VOVERRIDE void *GetObject3D() override;

    // special functions
    void HighlightShape(VisionViewBase ^view,float fSpotAngle);

    void SetAnimateScene(bool bStatus);
    void UpdateVisibleStatus();


    void SetLightType(LightSourceType_e eType, float fSpotAngle, String ^sProjectedTex);
    void SetIntensity(float fIntensity);
    void SetLightColor(UINT col)
    {
      m_iColor=col;
      VColorRef iColor;
      iColor.SetRGBA(m_iColor);
      m_pLight->SetColor(iColor);
      UpdateAnimation();
    }

    void SetCastShadows(bool bWorld, bool bModel);
    void SetInfluenceBitMasks(int iWorld, int iObject) {if (m_pLight) m_pLight->SetLightInfluenceBitMasks(iWorld,iObject);}
    void SetVisibleBitmask(unsigned int iMask)
    {
      m_iVisibleBitmask=iMask;
      UpdateVisibleStatus();
    }

    void SetAttenuation(LightAttenuationType_e iMode, String ^sTextureFile);
    void SetMultiplier(float fMultiplier);
    void SetFadeDistances(float fFadeStart, float fFadeEnd) {if (m_pLight) m_pLight->SetFadeDistances(fFadeStart, fFadeEnd);}
    void SetTriggered(bool bStatus) {if (m_pLight && m_pLight->GetTriggered()!=(BOOL)bStatus) m_pLight->Trigger();}
    void SetUseSpecular(bool bSpecular) {if (m_pLight) m_pLight->SetUseSpecular(bSpecular);}

    void UpdateAnimation();
    void SetAnimationLookup(String ^sTextureFile);
    void SetAnimationTiming(float fTime);
    void SetAnimationPhase(float fPhase);
    void SetRemoveLightWhenFinished(bool bState);

    void SetExportAsStatic(bool bStatus) {m_bExportAsStatic=bStatus;UpdateVisibleStatus();}
    void SetPreviewStatic(bool bStatus) {m_bPreviewStatic=bStatus;UpdateVisibleStatus();}
    void SetUsePreviewShadowComponent(bool bStatus);
    void SetExportWhenNotRelevant(bool bStatus) {m_bExportWhenNotRelevant=bStatus;}
    void ResetStaticGeometryInstanceList() {if (m_pLight) m_pLight->ResetStaticGeometryInstanceList();}
    void RemovePreviewComponents(VType *pType);

    // static helpers:
    static bool IsOccluderForPointLight(Vector3F pos, float fRadius, BoundingBox ^occluderBox, BoundingBox ^receiverBox);
  private:
    // these values have also to be stored here because of color animation (e.g. GetColor only returns the *current* light color)
    unsigned int m_iColor; // used as VColorRef
    float m_fIntensity;
    unsigned int m_iVisibleBitmask;
    bool m_bAnimated, m_bSceneAnimated, m_bEnabled, m_bShowCorona, m_bExportAsStatic, m_bPreviewStatic, m_bExportWhenNotRelevant;
    float m_fAnimTime, m_fAnimPhase;
    float m_fCoronaScale;
    bool m_bRemoveLightAfterAnim;
    CoronaScaleModeFlags_e m_eCoronaFlags;
    VString *m_pColorLookUpFile;
    VisLightSource_cl *m_pLight;
  };
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
