/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

using namespace System;
using namespace System::Diagnostics;
using namespace CSharpFramework;
using namespace CSharpFramework::Math;
using namespace CSharpFramework::Shapes;
using namespace CSharpFramework::Scene;
using namespace ManagedFramework;

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/Mirror.hpp>


namespace VisionManaged
{
  /*public */ref class EngineInstanceVisibilityObject; // forward decl.

  public enum class ReflectionShaderMode
  {
    AlwaysSimple = VisMirror_cl::AlwaysSimple,
    SimpleForAUTO = VisMirror_cl::SimpleForAUTO,
    AlwaysSurfaceShaders = VisMirror_cl::AlwaysSurfaceShaders
  };

  public enum class MirrorType
  {
    Mirror,
    PlanarWater,
    DisplacementWater
  };

  public ref class EngineInstanceMirror : public IEngineInstanceObject3D
  {
  public:     

    EngineInstanceMirror(MirrorType eType);
    void CreateMirrorInstance();
    VOVERRIDE void DisposeObject() override;
    VOVERRIDE void *GetObject3D() override  {return m_pMirror;}

    // overridden IEngineShapeInstance functions
    VOVERRIDE void SetVisible(bool bStatus) override {m_bVisible=bStatus;UpdateVisibleStatus();}
    VOVERRIDE void SetVisibleBitmask(unsigned int iMask) {m_iVisibleBitmask=iMask;UpdateVisibleStatus();}
    VOVERRIDE void SetScaling(float x,float y, float z) override  {m_pMirror->SetModelScale(x,y,z);}

    VOVERRIDE void TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result) override;
    VOVERRIDE bool GetLocalBoundingBox(BoundingBox ^%bbox) override;
    VOVERRIDE bool OnExport(SceneExportInfo ^info) override;

    // special functions
    void RenderShape(VisionViewBase ^view, ShapeRenderMode mode);
    void SetSize(float x, float y) {m_pMirror->SetSize(x,y);}
    void SetResolution(int iRes) {m_iResolution = iRes;}
    void SetUseHDR(bool bUseHDR) {m_bUseHDR = bUseHDR;}
    void SetShaderEffect(ShaderEffectConfig ^fxConfig);
    void SetModel(String ^modelFile);
    void SetFarclipDist(float fFar) {m_pMirror->SetFarClipDistance(fFar);}
    void SetReflectionShaderMode(ReflectionShaderMode mode) {m_pMirror->SetReflectionShaderMode((VisMirror_cl::VReflectionShaderSets_e)mode);}
    void SetFovScale(float fScale) {m_pMirror->SetFovScale(fScale);}
    void SetObliqueClippingPlaneOffset(float fOfs) {m_pMirror->SetObliqueClippingPlaneOffset(fOfs);}
    void SetUseLODFromRefContext(bool bUse) {m_pMirror->SetUseLODFromRefContext(bUse);}
    void SetSpecularLight(String ^key);

    // visibility
    void UpdateVisibleStatus();
    void AddVisibilityObject(EngineInstanceVisibilityObject ^pInst);
    void RemoveVisibilityObject(EngineInstanceVisibilityObject ^pInst);
    void SetRenderFilterMask(unsigned int iMask, bool bRenderExtObj) 
    {
      m_pMirror->SetRenderFilterMask(iMask);
      m_pMirror->SetExecuteRenderHooks(bRenderExtObj);
    }

    void SetRenderHook(unsigned int uiRenderHook) { m_pMirror->SetRenderHook(uiRenderHook); }

    // water
    static bool ShaderUsesDisplacement(ShaderEffectConfig ^fxConfig);
    bool CheckRendererNodeCompatibility();
    void SetUnderWaterFogProperties(bool bEnable, unsigned int iFogColor, float fFogStart, float fFogEnd, float fFogOpacity);
    inline VisTypedEngineObject_cl *GetNativeMirror() {return m_pMirror;}

    void SetResolveColorBuffer(bool bResolve);

  private:
    MirrorType m_eMirrorType;
    VisMirror_cl *m_pMirror;
    bool m_bVisible, m_bCompatible;
    int m_iResolution;
    bool m_bUseHDR;
    unsigned int m_iVisibleBitmask;

  public:
    // water properties:
    bool m_bUseRadialGrid, m_bMoveWithCamera;
    int m_iNumSubdivisionsU, m_iNumSubdivisionsV;
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
