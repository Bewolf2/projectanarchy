/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/VisionManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/EngineInstances/EngineInstanceProjector.hpp>

#using <mscorlib.dll>
using namespace ManagedFramework;
using namespace System::Diagnostics;


namespace VisionManaged
{

  EngineInstanceProjector::EngineInstanceProjector()
  {
    m_fAngleX = m_fAngleY = 0.f;
    m_pWallmark = new VisProjectedWallmark_cl();
    m_pWallmark->AddRef();
  }

  void EngineInstanceProjector::DisposeObject()
  {
    V_SAFE_DISPOSEANDRELEASE(m_pWallmark)
  }

  void EngineInstanceProjector::SetParentZone(Zone ^zone)
  {
    if (!m_pWallmark)
      return;

    VisZoneResource_cl *pNativeZone = zone ? ((VisZoneResource_cl *)zone->NativeZonePtr.ToPointer()) : NULL;
    m_pWallmark->SetParentZone(pNativeZone);
  }


  void EngineInstanceProjector::SetPosition(float x,float y,float z)
  {
    m_pWallmark->SetPosition(hkvVec3(x,y,z));
  }

  void EngineInstanceProjector::SetOrientation(float yaw,float pitch,float roll)
  {
    hkvMat3 rotMat;
    ConversionUtils::CreateRotationMatrix (rotMat, yaw,pitch,roll);
    m_pWallmark->SetOrientation(rotMat);
  }

  void EngineInstanceProjector::RenderShape(VisionViewBase ^view, ShapeRenderMode mode)
  {
    if (!m_pWallmark)
      return;

    IVRenderInterface *pRenderer = (static_cast<VisionView ^>(view))->GetRenderInterface();
    if (mode==ShapeRenderMode::Selected && (EditorManager::EditorMode == EditorManager::Mode::EM_NONE))
    {
      VSimpleRenderState_t state(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_FRONTFACE);
      float fLen = m_pWallmark->GetLength();
      float fFade = m_pWallmark->GetFadeOutRange();

      pRenderer->RenderFrustum(
        (hkvVec3& )m_pWallmark->GetPosition(),
        (hkvVec3& )m_pWallmark->GetDirection(),(hkvVec3& )m_pWallmark->GetDirection_Up(),
        m_fAngleX,m_fAngleY,
        hkvMath::Max(fLen-fFade,0.f),fLen+fFade, 
        VColorRef(255,0,255,100),state);
    }
  }

  void EngineInstanceProjector::TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result)
  {
    ShapeTraceResult ^pResult = result;
    hkvVec3 vStart(rayStart.X,rayStart.Y,rayStart.Z);
    hkvVec3 vEnd(rayEnd.X,rayEnd.Y,rayEnd.Z);
  }



  bool EngineInstanceProjector::GetLocalBoundingBox(BoundingBox ^%bbox)
  {
    float fSize = 30.f * EditorManager::Settings->GlobalUnitScaling;
    (*bbox).Set(-fSize,-fSize,-fSize,fSize,fSize,fSize);
    return true;
  }


  bool EngineInstanceProjector::OnExport(SceneExportInfo ^info) 
  {
    Debug::Assert( m_pWallmark != nullptr );
    VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
    ar << m_pWallmark;
    return true;
  }


  void EngineInstanceProjector::SetTexture(String ^filename)
  {
    VString sFilename;
    ConversionUtils::StringToVString(filename,sFilename);
    m_pWallmark->SetTexture(sFilename);
  }


  void EngineInstanceProjector::SetCustomShaderEffect(ShaderEffectConfig ^fxConfig)
  {
    VCompiledEffect *pEffect = ConversionUtils::GetShaderEffect(fxConfig,true,false);
    m_pWallmark->SetCustomShaderEffect(pEffect); 
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
