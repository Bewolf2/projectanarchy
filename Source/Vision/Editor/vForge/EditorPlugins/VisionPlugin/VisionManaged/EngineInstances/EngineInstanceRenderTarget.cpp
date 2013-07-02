/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/VisionManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/EngineInstances/EngineInstanceRenderTarget.hpp>

#using <mscorlib.dll>
using namespace ManagedFramework;
using namespace System::Diagnostics;


namespace VisionManaged
{

  EngineInstanceRenderTarget::EngineInstanceRenderTarget()
  {
    upx=upy=upz=dirx=diry=dirz=posx=posy=posz=yaw=pitch=roll = 0.f;
    m_iResX=m_iResY=-1;
    m_eType = RenderTargetType_e::ColorBuffer;
    m_bVisible = true;
    m_pCameraModel = Vision::Game.CreateEntity("VisBaseEntity_cl",hkvVec3(0,0,0),"Models\\camera.model");

    //m_iContextPos = Vision::Contexts.GetFreeContextPos();
    m_pContext = new VisRenderContext_cl();
    int iFlags = VIS_RENDERCONTEXT_FLAG_RENDER_CORONAS|VIS_RENDERCONTEXT_FLAG_USE_OCCLUSIONQUERY;
    m_pContext->SetRenderFlags(iFlags);
    VisContextCamera_cl *pCamera = new VisContextCamera_cl();
    m_pContext->SetCamera(pCamera);
    m_pContext->SetRenderLoop(new VisionRenderLoop_cl());

    // register the context
    //Vision::Contexts.SetReverseRenderOrder(true);
    //Vision::Contexts.SetContext(m_iContextPos,m_pContext);
    Vision::Contexts.AddContext(m_pContext);
    m_pContext->SetPriority(VIS_RENDERCONTEXTPRIORITY_MIRROR);
    //Vision::Contexts.SetActivateRendering(m_iContextPos,false);
  }

  void EngineInstanceRenderTarget::DisposeObject()
  {
    V_SAFE_REMOVE(m_pCameraModel);
    //Vision::Contexts.SetContext(m_iContextPos,NULL);
    Vision::Contexts.RemoveContext(m_pContext);
  }

  void EngineInstanceRenderTarget::CreateRenderTarget(int iResX, int iResY, RenderTargetType_e iType, String ^sName)
  {
    // [SHADER] TODO
/*
    if (m_iResX==iResX && m_iResY==iResY && m_eType==iType)
    {
      SetRenderTargetName(sName);
      return;
    }

    m_iResX=iResX;
    m_iResY=iResY;
    m_eType=iType;

    // set a render target:
    VisPBufferConfig_t config;
    config.m_iHeight = m_iResX;
    config.m_iWidth  = m_iResY;
    switch (m_eType)
    {
      case ColorBuffer:
        break;
      case DepthBuffer:
        config.m_bDepthLuminanceTexture = TRUE;
        break;
    }


    VTextureObject *pTex =  Vision::TextureManager.CreatePBuffer("",config);
    pTex->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
    m_pContext->SetRenderTarget(pTex);
    SetRenderTargetName(sName);
*/
  }

  void EngineInstanceRenderTarget::SetRenderTargetName(String ^name)
  {
    VString sName;
    ConversionUtils::StringToVString(name,sName);
    if (sName.IsEmpty()||!m_pContext->GetRenderTarget())
      return;
    m_pContext->GetRenderTarget()->SetFilename(sName);
  }


  bool EngineInstanceRenderTarget::GetLocalBoundingBox(BoundingBox ^%bbox)
  {
    const float fSize = 5.f * EditorManager::Settings->GlobalUnitScaling;
    (*bbox).Set(-fSize,-fSize,-fSize,fSize,fSize,fSize);
    return true;
  }

  void EngineInstanceRenderTarget::SetVisible(bool bStatus)
  {
    m_bVisible=bStatus;
  }

  void EngineInstanceRenderTarget::UpdateCameraPosition()
  {
    hkvVec3 vPos(posx,posy,posz),vDir,vUp,dummy;
    hkvMat3 rotMat (hkvNoInitialization);
    rotMat.setFromEulerAngles (roll, pitch, yaw);

    m_pCameraModel->SetOrientation(yaw,pitch,roll);
    m_pCameraModel->SetPosition(vPos);
    m_pContext->GetCamera()->Set(rotMat,vPos);

    rotMat.getAxisXYZ(&vDir,&dummy,&vUp);
    dirx=vDir.x;
    diry=vDir.y;
    dirz=vDir.z;
    upx=vUp.x;
    upy=vUp.y;
    upz=vUp.z;
  }

  void EngineInstanceRenderTarget::SetOrientation(float _yaw,float _pitch,float _roll)
  {
    yaw=_yaw;pitch=_pitch;roll=_roll;
    UpdateCameraPosition();
  }

  void EngineInstanceRenderTarget::SetPosition(float x,float y,float z) 
  {
    posx=x;posy=y;posz=z;
    UpdateCameraPosition();
  }

  void EngineInstanceRenderTarget::SetFOV(float fx, float fy)
  {
    m_pContext->SetFOV(fx,fy);
  }

  void EngineInstanceRenderTarget::SetClipPlanes(float fNear, float fFar)
  {
    m_pContext->SetClipPlanes(fNear,fFar);
  }


 void EngineInstanceRenderTarget::TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result)
  {
    ShapeTraceResult ^pResult = result;
    hkvVec3 vStart(rayStart.X,rayStart.Y,rayStart.Z);
    hkvVec3 vEnd(rayEnd.X,rayEnd.Y,rayEnd.Z);

    VisBaseEntity_cl *list = m_pCameraModel;
    VisEntityCollection_cl traceCollection(1, &list, 1, 1);
    static VisTraceLineInfo_t traceInfo;//@@@@
    int iFlags = VIS_TRACESETTINGS_FORCEOBJ_POLYGONACC|VIS_TRACESETTINGS_STORESURFACEINFO;
    int iTraceCount = Vision::CollisionToolkit.TraceLineEntities(vStart,vEnd,traceCollection, &traceInfo, iFlags);
    if (iTraceCount<1) return;

    pResult->SetShapeHit_IfCloser(ownerShape,traceInfo.distance);
  }

  void EngineInstanceRenderTarget::RenderShape(VisionViewBase ^view, ShapeRenderMode mode)
  {
    if (mode==ShapeRenderMode::Selected)
    {
      VTextureObject *pTargetTex = m_pContext->GetRenderTarget();
      IVRenderInterface *pRenderer = (static_cast<VisionView ^>(view))->GetRenderInterface();
      VSimpleRenderState_t state(VIS_TRANSP_ALPHA,RENDERSTATEFLAG_FRONTFACE|RENDERSTATEFLAG_ALWAYSVISIBLE);

      // render target preview
      float fSize = 8.f * EditorManager::Settings->GlobalUnitScaling;
      const float fDist = 25.f * EditorManager::Settings->GlobalUnitScaling;
      hkvVec3 vPos(posx-fDist*dirx,posy-fDist*diry,posz-fDist*dirz);
      pRenderer->DrawSprite(vPos,pTargetTex,V_RGBA_WHITE,state,fSize,fSize);

      // render frustum
      VSimpleRenderState_t fstate(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_FRONTFACE);
      float fx,fy,nc,fc;
      m_pContext->GetFOV(fx,fy);
      m_pContext->GetClipPlanes(nc,fc);
      pRenderer->RenderFrustum(
        hkvVec3(posx,posy,posz),hkvVec3(dirx,diry,dirz),hkvVec3(upx,upy,upz),
          fx,fy,nc,fc,VColorRef(255,0,255,100),fstate);

    }
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
