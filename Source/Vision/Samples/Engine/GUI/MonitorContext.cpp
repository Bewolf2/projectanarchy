/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Samples/Engine/GUI/GUIPCH.h>         // precompiled header
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>
#include <Vision/Samples/Engine/GUI/MonitorContext.hpp>

  // constructor that takes an entity that it attaches to along with the surface name that it uses for picking and displaying
VMonitorGUIContext::VMonitorGUIContext(VGUIManager *pManager, VisSurface_cl *pScreenSrf)
  : IVGUIContext(pManager)
  , m_fTraceRange(100.f)
{
  m_pGUISurface = pScreenSrf;

  // create a render context texture to render the GUI into
  VisRenderableTextureConfig_t config;

  config.m_iHeight = 512;
  config.m_iWidth  = 512;
  config.m_eFormat = VTextureLoader::DEFAULT_RENDERTARGET_FORMAT_32BPP;
  

  VisRenderableTexture_cl *pTex =  Vision::TextureManager.CreateRenderableTexture("<Monitor>",config);
  m_ClientRect.Set(0,0,(float)config.m_iWidth,(float)config.m_iHeight);
  
  config.m_eFormat = VVideo::GetSupportedDepthStencilFormat(VTextureLoader::D24S8, *Vision::Video.GetCurrentConfig());
  config.m_bRenderTargetOnly = true;
  config.m_bIsDepthStencilTarget = true;
  VisRenderableTexture_cl *pDepthStencilTarget =  Vision::TextureManager.CreateRenderableTexture("<MonitorDS>",config);

  m_spRenderContext = new VisRenderContext_cl();
  m_spRenderContext->SetRenderLoop(new MonitorRenderLoop(this));
  m_spRenderContext->SetRenderTarget(0, pTex);
  m_spRenderContext->SetDepthStencilTarget(pDepthStencilTarget);
  m_spRenderContext->SetCamera(new VisContextCamera_cl());
  m_spRenderContext->SetVisibilityCollector(new VisionVisibilityCollector_cl());

  m_spRenderContext->SetRenderFilterMask(VIS_ENTITY_VISIBLE_IN_TEXTURE);
  SetVisibleBitmask(VIS_ENTITY_VISIBLE_IN_TEXTURE);

  // register the context
  Vision::Contexts.AddContext(m_spRenderContext);

  // replace the base texture of this surface by GUI context texture
  pScreenSrf->SetBaseTexture(pTex);

  // we need the base uv coordinates to detect mouse movement on the surface
  Vision::CollisionToolkit.SetTraceLineSettings(VIS_TRACESETTINGS_DEFAULT|VIS_TRACESETTINGS_STOREBASEUV);

  OnItemClick += this; // listen to the item click event
}


VMonitorGUIContext::~VMonitorGUIContext()
{
  OnItemClick -= this;
  Vision::Contexts.RemoveContext(m_spRenderContext);
}


// traces the entity and calculates the mouse move delta on screen
void VMonitorGUIContext::TestPicking()
{
  m_vTraceDelta.setZero();
  m_bPicked = false;

  hkvVec3 vStart = Vision::Camera.GetCurrentCameraPosition();
#if defined(_VISION_MOBILE)
  hkvVec3 vDir(0.0f, 0.0f, 0.0f);
  if(VInputManager::GetTouchScreen().IsActiveTouch(0))
  {
    const float fX = VInputManager::GetTouchScreen().GetTouchPointValue(0, CT_TOUCH_ABS_X, false);
    const float fY = VInputManager::GetTouchScreen().GetTouchPointValue(0, CT_TOUCH_ABS_Y, false);

    VisRenderContext_cl::GetMainRenderContext()->GetTraceDirFromScreenPos(fX, fY, vDir, m_fTraceRange);
  }
  else
  {
    return;
  }

  hkvVec3 vEnd = vStart + vDir;
#else
  hkvVec3 vEnd = vStart + Vision::Camera.GetCurrentCameraDirection()*m_fTraceRange;
#endif
  VisTraceLineInfo_t traceInfo;

  Vision::CollisionToolkit.TraceLine(vStart, vEnd, (ULONG)VIS_TRACE_ALL, VIS_TRACE_NONE, NULL, NULL, &traceInfo,
                                     VIS_TRACESETTINGS_FORCEOBJ_POLYGONACC|VIS_TRACESETTINGS_STOREBASEUV|VIS_TRACESETTINGS_STORESURFACEINFO);

  if (!traceInfo.detected || traceInfo.pSurface!=m_pGUISurface)
    return;

  hkvVec2 vNewPos;
  vNewPos.x = m_ClientRect.m_vMin.x + traceInfo.baseUV.x*m_ClientRect.GetSizeX();
  vNewPos.y = m_ClientRect.m_vMin.y + traceInfo.baseUV.y*m_ClientRect.GetSizeY();

  m_vTraceDelta = vNewPos-m_vMouseTracePos;
  m_vMouseTracePos = vNewPos;
  m_bPicked = true;
}


// overridden IVGUIContext function
void VMonitorGUIContext::OnTickFunction(float fTimeDelta)
{
  TestPicking();
  IVGUIContext::OnTickFunction(fTimeDelta);
}


// actual implementation of IVGUIContext interface
void VMonitorGUIContext::UpdateCursorPos(VGUIUserInfo_t &userState)
{
  //Update the cursor to the traced position
  userState.m_vMousePos = m_vMouseTracePos;
}

// actual implementation of IVGUIContext interface
int VMonitorGUIContext::GetButtonMask(VGUIUserInfo_t &user)
{
  if (!m_bPicked) return 0;
  int iMask = 0;

  if (VisSampleApp::GetInputMap()->GetTrigger(GUI_DEMO_ACTION_1)) iMask |= BUTTON_LMOUSE;
  if (VisSampleApp::GetInputMap()->GetTrigger(GUI_DEMO_ACTION_2)) iMask |= BUTTON_RMOUSE;
  if (VisSampleApp::GetInputMap()->GetTrigger(GUI_DEMO_ACTION_3)) iMask |= BUTTON_MMOUSE;

#if defined(_VISION_MOBILE)
  if(VInputManager::GetTouchScreen().GetControlValue(CT_TOUCH_ANY, 0.0f) > 0.0f)
    iMask |= BUTTON_LMOUSE;
#endif

  return iMask;
}


// callback listener. Implemented here to listen to the button click event
void VMonitorGUIContext::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  IVGUIContext::OnHandleCallback(pData);
  if (pData->m_pSender==&OnItemClick)
  {
    // this is just a demo to listen to the events in this context
    VMenuEventDataObject *pClickEvent = (VMenuEventDataObject *)pData;
    pClickEvent->m_pItem->SetText("Door unlocked");
    pClickEvent->m_pItem->SetEnabled(false);
    Vision::Message.Add("Trigger open door event");
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
