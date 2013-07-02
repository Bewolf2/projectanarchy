/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/VisionManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/EngineInstances/EngineInstanceMirror.hpp>
#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/EngineInstances/EngineInstanceVisibilityObject.hpp>

#if !defined( HK_ANARCHY )
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/Water.hpp>
#endif


#using <mscorlib.dll>
using namespace ManagedFramework;
using namespace System::Diagnostics;


namespace VisionManaged
{

  EngineInstanceMirror::EngineInstanceMirror(MirrorType eType)
  {
    m_eMirrorType = eType;
    m_pMirror = nullptr;
    m_iResolution = 512;
    m_bUseHDR = false;
    m_bVisible = true;
    m_iVisibleBitmask = 0xffffffff;
    m_bUseRadialGrid = false;
    m_iNumSubdivisionsU = m_iNumSubdivisionsV = 128; 
    m_bMoveWithCamera = false;
    m_bCompatible = true;
  }

  void EngineInstanceMirror::CreateMirrorInstance()
  {
    DisposeObject(); // remove the old one

    VisMirrorManager_cl *pManager = &VisMirrorManager_cl::GlobalManager();
    switch (m_eMirrorType)
    {
    case MirrorType::Mirror:
      m_pMirror = new VisMirror_cl(pManager,m_iResolution, m_bUseHDR);
      break;
#if !defined( HK_ANARCHY )
    case MirrorType::PlanarWater:
      m_pMirror = new PlanarWater_cl(pManager,m_iResolution,m_bMoveWithCamera, m_bUseHDR);
      break;
    case MirrorType::DisplacementWater:
      m_pMirror = new DisplacementWater_cl(pManager,m_iResolution,m_bUseRadialGrid, m_iNumSubdivisionsU, m_iNumSubdivisionsV, m_bMoveWithCamera, m_bUseHDR);
      break;
#endif
    default:
      System::Diagnostics::Debug::Assert(false,"Invalid mirror type");
    }
    m_pMirror->Init();
    m_pMirror->AddRef();
  }


  void EngineInstanceMirror::DisposeObject()
  {
    V_SAFE_DISPOSEANDRELEASE(m_pMirror)
  }

  void EngineInstanceMirror::UpdateVisibleStatus()
  {
    if (m_pMirror==nullptr)
      return;

    unsigned int iMask = (m_bVisible && m_bCompatible) ? m_iVisibleBitmask : 0;
    m_pMirror->SetVisibleBitmask(iMask);
    m_pMirror->SetActive(iMask>0);
  }

  bool EngineInstanceMirror::GetLocalBoundingBox(BoundingBox ^%bbox)
  {
    if (!m_pMirror)
      return false;
    float x = m_pMirror->GetSizeX()*0.5f;
    float y = m_pMirror->GetSizeY()*0.5f;
    (*bbox).Set(-x,-y,-1.f,x,y,1.f);

    return true;
  }

  void EngineInstanceMirror::TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result)
  {
    float fDist;
    fDist = m_pMirror->TraceMirror(
      hkvVec3(rayStart.X,rayStart.Y,rayStart.Z),
      hkvVec3(rayEnd.X,rayEnd.Y,rayEnd.Z),false);
        
    if (fDist<0.f)
      return;

    (*result).SetShapeHit_IfCloser(ownerShape,fDist);
  }


  bool EngineInstanceMirror::OnExport(SceneExportInfo ^info) 
  {
    Debug::Assert( m_pMirror != nullptr );
    VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
    if (m_bCompatible)
      ar << m_pMirror;
    return true;
  }


  void EngineInstanceMirror::RenderShape(VisionViewBase ^view, ShapeRenderMode mode)
  {
    IVRenderInterface *pRenderer = (static_cast<VisionView ^>(view))->GetRenderInterface();
    if (mode==ShapeRenderMode::Selected) 
      m_pMirror->DebugRender(pRenderer);
  }

  void EngineInstanceMirror::SetShaderEffect(ShaderEffectConfig ^fxConfig)
  {
    VCompiledEffect *pEffect = ConversionUtils::GetShaderEffect(fxConfig,true,true);// force unique!
    m_pMirror->SetEffect(pEffect); 
  }

  void EngineInstanceMirror::SetModel(String ^modelFile)
  {
    VString sModelFile;
    ConversionUtils::StringToVString( modelFile, sModelFile );
    m_pMirror->SetModelFile(sModelFile);
  }

  void EngineInstanceMirror::AddVisibilityObject(EngineInstanceVisibilityObject ^pInst)
  {
    m_pMirror->AddVisibilityObject(pInst->GetVisibilityObject());
  }

  void EngineInstanceMirror::RemoveVisibilityObject(EngineInstanceVisibilityObject ^pInst)
  {
    m_pMirror->RemoveVisibilityObject(pInst->GetVisibilityObject());
  }

  
  bool EngineInstanceMirror::ShaderUsesDisplacement(ShaderEffectConfig ^fxConfig)
  {
    VCompiledEffect *pEffect = ConversionUtils::GetShaderEffect(fxConfig,true,false);
    if (pEffect==nullptr || pEffect->GetDefaultTechnique()==nullptr)
      return false;

    VCompiledTechnique *pTech = pEffect->GetDefaultTechnique();
    for (int i=0;i<pTech->GetShaderCount();i++)
    {
      const VCompiledShaderPass *pShader = pTech->GetShader(i);
      // use this as criteria for displacements:
      if (pShader->GetActiveSamplerCount(VSS_VertexShader) > 0)
        return true;
    }
    return false;
  }

  void EngineInstanceMirror::SetSpecularLight(String ^key)
  {
#if defined( HK_ANARCHY )
    return;
#else
    if (m_pMirror==nullptr || (m_eMirrorType!=MirrorType::PlanarWater && m_eMirrorType!=MirrorType::DisplacementWater))
      return;
    PlanarWater_cl *pWater = (PlanarWater_cl *)m_pMirror;
    if (String::IsNullOrEmpty(key))
    {
      pWater->SetSpecularLight(NULL);
      return;
    }
    VString sKey;
    ConversionUtils::StringToVString(key,sKey);
    VisLightSource_cl *pLight = Vision::Game.SearchLightSource(sKey);
    pWater->SetSpecularLight(pLight);
#endif
  }

  bool EngineInstanceMirror::CheckRendererNodeCompatibility()
  {
    if (m_eMirrorType==MirrorType::Mirror) // no special requirements
      m_bCompatible = true;
    else
    {
      IVRendererNode *pRendererNode = Vision::Renderer.GetRendererNode(0);
      m_bCompatible = pRendererNode!=nullptr;
    }
    if (!m_bCompatible)
      Vision::Error.Warning("This scene contains at least one mirror/water shape. This requires a valid renderer node (Main Layer properties)");
    UpdateVisibleStatus();
    return m_bCompatible;
  }

  void EngineInstanceMirror::SetUnderWaterFogProperties(bool bEnable, unsigned int iFogColor, float fFogStart, float fFogEnd, float fFogOpacity)
  {
#if defined( HK_ANARCHY )
    return;
#else
    if (m_pMirror==nullptr || (m_eMirrorType!=MirrorType::PlanarWater && m_eMirrorType!=MirrorType::DisplacementWater))
      return;
    PlanarWater_cl *pWater = (PlanarWater_cl *)m_pMirror;
    VColorRef fogColor;
    fogColor.SetRGBA(iFogColor);
    fogColor.a = (int)(fFogOpacity*255.99f); // If accuracy is not enough here, make it a separate float value
    pWater->SetUnderWaterFogProperties(bEnable, fogColor, fFogStart, fFogEnd);
#endif
  }

  void EngineInstanceMirror::SetResolveColorBuffer(bool bResolve)
  {
#if defined( HK_ANARCHY )
    return;
#else
    if (m_pMirror==nullptr || (m_eMirrorType!=MirrorType::PlanarWater && m_eMirrorType!=MirrorType::DisplacementWater))
      return;
    PlanarWater_cl *pWater = (PlanarWater_cl *)m_pMirror;
    pWater->SetResolveColorBuffer(bResolve);
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
