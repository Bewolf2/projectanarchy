/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

using namespace System;
using namespace CSharpFramework;
using namespace CSharpFramework::Math;
using namespace CSharpFramework::Shapes;
using namespace CSharpFramework::Scene;
using namespace CSharpFramework::PropertyEditors;
using namespace ManagedFramework;

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/VWallmarkManager.hpp>


namespace VisionManaged
{
  [Flags]
  public enum class GeometryTypeMask
  {
    [NonEditableFlagAttribute]
    None = 0,
    StaticMeshes  = PROJECTOR_AFFECTS_STATICMESHES,
    Terrain       = PROJECTOR_AFFECTS_TERRAIN,
    Entities      = PROJECTOR_AFFECTS_ENTITIES,

    [NonEditableFlagAttribute]
    All = StaticMeshes|Terrain|Entities
  };

  public ref class EngineInstanceProjector : public IEngineShapeInstance
  {
  public:     

    EngineInstanceProjector();
    VOVERRIDE void DisposeObject() override;
    VOVERRIDE IntPtr GetNativeObject() override  {return System::IntPtr((void*)m_pWallmark);}
    inline VisTypedEngineObject_cl* GetNativeProjector() {return m_pWallmark;}

    // overridden IEngineShapeInstance functions
    VOVERRIDE void SetParentZone(Zone ^zone) override;
    VOVERRIDE void SetVisible(bool bStatus) override {m_pWallmark->SetVisibleBitmask(bStatus ? 0xffffffff:0);}
    VOVERRIDE void SetPosition(float x,float y,float z) override;
    VOVERRIDE void SetOrientation(float yaw,float pitch,float roll) override;
    VOVERRIDE void SetScaling(float x,float y, float z) override  {}
    VOVERRIDE void SetUniqueID(unsigned __int64 iID) override {m_pWallmark->SetUniqueID(iID);}

    VOVERRIDE void TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result) override;
    VOVERRIDE bool GetLocalBoundingBox(BoundingBox ^%bbox) override;
    VOVERRIDE bool OnExport(SceneExportInfo ^info) override;
    VOVERRIDE void OnPostEngineInstanceCreation() override {ConversionUtils::CallOnDeserializationFunction(m_pWallmark);}
    VOVERRIDE void OnBeforeExport(SceneExportInfo ^info) override {if (m_pWallmark!=NULL) m_pWallmark->SetMarkInsideSerializationSession(true);}
    VOVERRIDE void OnAfterExport(SceneExportInfo ^info) override {if (m_pWallmark!=NULL) m_pWallmark->SetMarkInsideSerializationSession(false);}

    // special functions
    void SetInfluenceBitmask(UINT iMask)  {m_pWallmark->SetInfluenceBitmask(iMask);}

    void RenderShape(VisionViewBase ^view, ShapeRenderMode mode);

    void SetConeAngles(float fAngleX, float fAngleY) {m_pWallmark->SetConeAngles(fAngleX,fAngleY);m_fAngleX=fAngleX;m_fAngleY=fAngleY;}
    void SetTexture(String ^filename);
    void SetTransparency(TransparencyType transp) {m_pWallmark->SetTransparency((VIS_TransparencyType)transp);}
    void SetLength(float fLen) {m_pWallmark->SetLength(fLen);}
    void SetFadeOutRange(float fLen) {m_pWallmark->SetFadeOutRange(fLen);}
    void SetColor(UINT iColor) {VColorRef color; color.SetRGBA(iColor);m_pWallmark->SetColor(color);}
    void SetLightmapped(bool bStatus) {m_pWallmark->SetLightmapped(bStatus);}
    void SetGeometryTypeMask(GeometryTypeMask iMask) {m_pWallmark->SetGeometryTypeFilterMask((int)iMask);}
    void SetFarClipDistance(float fDist) {m_pWallmark->SetFarClipDistance(fDist);}
    void SetCustomShaderEffect(ShaderEffectConfig ^fxConfig);

    // components:
    VOVERRIDE property bool SupportsComponents
    {
      bool get() override
      {
        return true;
      }
    }

    VOVERRIDE bool CanAttachComponent(ShapeComponent ^component, String ^%sError) override {return ConversionUtils::CanAttachComponent(GetNativeProjector(),component,sError);}
    VOVERRIDE void OnAttachComponent(ShapeComponent ^component) override {ConversionUtils::OnAttachComponent(GetNativeProjector(),component);}
    VOVERRIDE void OnRemoveComponent(ShapeComponent ^component) override {ConversionUtils::OnRemoveComponent(GetNativeProjector(),component);}

  private:
    float m_fAngleX,m_fAngleY;
    VProjectedWallmark *m_pWallmark;
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
