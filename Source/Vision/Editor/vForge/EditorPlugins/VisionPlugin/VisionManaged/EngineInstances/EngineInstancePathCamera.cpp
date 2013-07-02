/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/VisionManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/EngineInstances/EngineInstancePathCamera.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiBaseEntity.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/PathCameraEntity.hpp>

#using <mscorlib.dll>
using namespace ManagedFramework;
using namespace System::Diagnostics;

#ifdef _MSC_VER
// disable warning C4996: deprecated function
#pragma warning( disable : 4996)
#endif

namespace VisionManaged
{

  EngineInstancePathCamera::EngineInstancePathCamera()
  {
    VisBaseEntity_cl *pEntity = Vision::Game.CreateEntity("PathCameraEntity",hkvVec3(0,0,0));
    Debug::Assert(pEntity!=nullptr,"Could not create Cloth entity!");

    // reference the entity with a weak pointer. This will make sure that we correctly get a null pointer
    // if the entity gets deleted in the engine
    m_pEntityWP = new VWeakPtr<VisBaseEntity_cl>(pEntity->GetWeakReference());

    m_pSpriteTex = Vision::TextureManager.Load2DTexture("textures\\videoCamera.dds",VTM_FLAG_DEFAULT_NON_MIPMAPPED);
    m_pSpriteTex->AddRef();

  }

  void EngineInstancePathCamera::DisposeObject()
  {
    VisBaseEntity_cl *pEntity = GetEntity();
    V_SAFE_DISPOSEOBJECT(pEntity);
    V_SAFE_DELETE( m_pEntityWP );
    V_SAFE_RELEASE(m_pSpriteTex);
  }



  void EngineInstancePathCamera::SetPosition(float x,float y,float z)
  {
    if (GetEntity())
      GetEntity()->SetPosition(hkvVec3(x,y,z));
  }

  void EngineInstancePathCamera::SetOrientation(float yaw,float pitch,float roll)
  {
    if (GetEntity())
      GetEntity()->SetOrientation(hkvVec3(yaw,pitch,roll));
  }

  bool EngineInstancePathCamera::GetLocalBoundingBox(BoundingBox ^%bbox)
  {
    if ( GetEntity() == nullptr )
      return false;

    float r = 7.f * EditorManager::Settings->GlobalUnitScaling;
    
    (*bbox).Set(-r,-r,-r, r,r,r);
    return true;
  }

  void EngineInstancePathCamera::TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result)
  {
  }

  void EngineInstancePathCamera::RenderShape(VisionViewBase ^view, ShapeRenderMode mode)
  {
    if (Vision::Editor.IsAnimatingOrPlaying() || !GetEntity())
      return;
    IVRenderInterface *pRenderer = (static_cast<VisionView ^>(view))->GetRenderInterface();
  
    hkvVec3 vPos = GetEntity()->GetPosition();

    if (mode==ShapeRenderMode::Selected)
    {
      const float fRad = 7.f * EditorManager::Settings->GlobalUnitScaling;
      hkvAlignedBBox bbox(hkvVec3 (vPos.x-fRad,vPos.y-fRad,vPos.z-fRad), hkvVec3 (vPos.x+fRad,vPos.y+fRad,vPos.z+fRad));
      pRenderer->DrawLineBox(bbox,V_RGBA_YELLOW,1.f);
    }

    VSimpleRenderState_t state(VIS_TRANSP_ALPHA);

    Vector3F vCamPos = view->CameraPosition;
    hkvVec3 diff(vPos.x-vCamPos.X, vPos.y-vCamPos.Y,vPos.z-vCamPos.Z);
    float fSize = 0.05f*diff.getLength();

    pRenderer->DrawSprite((hkvVec3& )vPos,m_pSpriteTex,V_RGBA_WHITE,state,fSize,fSize);
  }

  void EngineInstancePathCamera::SetScriptFile(String ^filename)
  {
    if (!GetEntity())
      return;
    VString sScriptFile;
    ConversionUtils::StringToVString(filename,sScriptFile);
    ((PathCameraEntity *)GetEntity())->SetScriptFilename(sScriptFile);
  }


  bool EngineInstancePathCamera::OnExport(SceneExportInfo ^info) 
  {
    Debug::Assert( GetEntity() != nullptr );
    VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
    ar << GetEntity();
    return true;
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
