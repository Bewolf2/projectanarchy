/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>         // precompiled header
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VGUIContext.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

#define THUMBSTICK_DEADZONE 5000
#define DOUBLECLICK_DELAY 0.2f
#define INVALIDATE_DOUBLECLICK {userState.m_fLastClickTime=-1.f;userState.m_iLastClickedMask=0;}

//#undef SUPPORTS_MOUSE
//#undef SUPPORTS_KEYBOARD
//#undef SUPPORTS_MULTITOUCH

#define DO_NOT_USE_VGL
#undef DO_NOT_USE_VGL

// NULL App class that does not perform anything game loop specific
class VNullApp : public VisionApp_cl
{
public:
  virtual void OnUpdateScene() HKV_OVERRIDE
  {
    //Timer is not longer updated here, because it needs to be updated right after the frame flip
    Vision::Callbacks.OnFrameUpdatePreRender.TriggerCallbacks();
    const float dtime = Vision::GetTimer()->GetTimeDifference();

    //Handle render contexts
    VisRenderContext_cl::HandleAllRenderContexts(dtime);

    Vision::Profiling.Update();
  }
  VNullApp()
  {
    //Vision::SetSceneManager(new VNullSceneManager);
  }
};


IVGUIContext::IVGUIContext(VGUIManager *pManager)
{
  if (!pManager) // attach to global manager
    pManager = &VGUIManager::GlobalManager();

  m_pManager = pManager;
  m_bActive = false;
  m_bShowCursor = true;
  m_bHandleKeyboard = false; // true in main context
  pManager->Contexts().Add(this);
  m_iRenderHookConstant = VRH_GUI;
  m_iVisibleBitmask = VIS_ENTITY_VISIBLE_IN_WORLD;
  m_bKeyboardEnterAsMouseLeftClick = true;
  for (int i=0;i<VGUIUserInfo_t::GUIMaxUser;i++)
    m_UserStates[i].m_iID = (VGUIUserInfo_t::VGUIUserID_e)i;
  m_iUserIDMask = 1<<VGUIUserInfo_t::GUIUser0;

  m_fDragThreshold = 0.0f;

  #if defined(VISION_GUI_USE_WINDOWS_INPUT)
  VInputCallbacks::OnPreTranslateMessage += this;
  #endif

#if defined(SUPPORTS_MULTITOUCH) && defined(_VISION_PSP2)
  VMultiTouchInputPSP2 *pMultiTouch = (VMultiTouchInputPSP2*)&VInputManagerPSP2::GetInputDevice(INPUT_DEVICE_TOUCHSCREEN);
  pMultiTouch->SetEnabled(true);
#endif

#if defined(_VISION_WIIU)
  m_drcIndex = V_DRC_FIRST;
#endif
}


IVGUIContext::~IVGUIContext()
{
  VASSERT(!m_bActive && "Context must be deactivated before destroying")
  #if defined(VISION_GUI_USE_WINDOWS_INPUT)
  VInputCallbacks::OnPreTranslateMessage -= this;
  #endif
}

  
void IVGUIContext::ResetKeyboardStates()
{
#ifdef SUPPORTS_KEYBOARD
  memset(m_bKeyPressed,0,sizeof(m_bKeyPressed));
#endif
}


void IVGUIContext::SetActivate(bool bStatus)
{
  if (m_bActive==bStatus)
    return;

  m_bActive = bStatus;
  if (m_bActive)
  {
    ResetKeyboardStates();
    Vision::Callbacks.OnFrameUpdatePreRender += this;
    Vision::Callbacks.OnRenderHook += this;
  }
  else
  {
    Vision::Callbacks.OnFrameUpdatePreRender -= this;
    Vision::Callbacks.OnRenderHook -= this;

    // free some smart pointers
    for (int i=0;i<VGUIUserInfo_t::GUIMaxUser;i++)
    {
      VGUIUserInfo_t *pUser = GetUser((VGUIUserInfo_t::VGUIUserID_e)i);
      if (!pUser) continue;
      SetMouseOverItem(*pUser, NULL);
      pUser->m_spDragItem = NULL;
      pUser->m_spMouseDownItem = NULL;
      pUser->m_pCurrentCursor = NULL;
    }
    m_spFocusDlg = NULL;
    m_OpenDialogs.Clear();
    m_spOverrideCursor = NULL;
    m_spTooltip = NULL;
  }
}


void IVGUIContext::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender==&Vision::Callbacks.OnRenderHook)
  {
    // only perform the rendering in our context and only at the defined entry point (end of rendering by default)
    if (((VisRenderHookDataObject_cl*)pData)->m_iEntryConst != m_iRenderHookConstant)
      return;

    if (Vision::Contexts.GetCurrentContext()->GetRenderFilterMask() & m_iVisibleBitmask)
    {
      OnRender();
    }
  }
  else if (pData->m_pSender==&Vision::Callbacks.OnFrameUpdatePreRender)
  {
    if (m_bActive)
      OnTickFunction(Vision::GetUITimer()->GetTimeDifference());
  }
  
  #if defined(VISION_GUI_USE_WINDOWS_INPUT)
  else if(pData->m_pSender==&VInputCallbacks::OnPreTranslateMessage)
  {
    VWindowsMessageDataObject* pObj = (VWindowsMessageDataObject*)pData;
    if(pObj->m_Msg.message == WM_CHAR)
    {
      if (m_spFocusDlg!=NULL)
        m_spFocusDlg->OnKeyPressed(pObj->m_Msg.wParam, 0);
    }
    
  }
  #endif

}



bool IVGUIContext::HandleSingleKey(int iKey)
{
#ifdef SUPPORTS_KEYBOARD
  #ifdef DO_NOT_USE_VGL
    if (VInputManagerPC::GetKeyboard().GetRawControlValue(iKey+CT_KB_UNKNOWN)==0)
  #else
    if (!VGLIsKeyPressed(iKey))
  #endif
  {
    m_bKeyPressed[iKey] = false;
    return false;
  }

  if (m_bKeyPressed[iKey]) // single hit
    return false;

  m_bKeyPressed[iKey] = true;
  return true;
#else
  return false;
#endif
}


void IVGUIContext::OnTickFunction(float fTimeDelta)
{
  VISION_PROFILE_FUNCTION(VGUIManager::PROFILING_TICKFUNCTIONS);

  for (int i=0;i<VGUIUserInfo_t::GUIMaxUser;i++)
  {
    if ((m_iUserIDMask&(1<<i))==0)
      continue; // user active?
    VGUIUserInfo_t &userState(m_UserStates[i]);
    int iOldButton = userState.m_iButtonMask;
    hkvVec2 vMouseDelta;
    hkvVec2 vOldMousePos = userState.m_vMousePos;
    
    UpdateCursorPos(userState);

    vMouseDelta = userState.m_vMousePos-vOldMousePos; // both coordinates are integer based

    bool bMouseMoving = vMouseDelta.getLength() > m_fDragThreshold;

    if (bMouseMoving)
      INVALIDATE_DOUBLECLICK;

    VWindowBase *pOldMouseOver = userState.m_spMouseOverItem;

#if defined(SUPPORTS_MULTITOUCH) || defined(_VISION_WIIU)
    userState.m_iButtonMask = userState.m_iNextButtonMask;
    userState.m_iNextButtonMask = GetButtonMask(userState);
#else
    userState.m_iButtonMask = GetButtonMask(userState);
#endif

    // tick function for all dialogs and tooltip
    m_OpenDialogs.OnTickFunction(fTimeDelta);
    if (m_spTooltip)
      m_spTooltip->OnTick(fTimeDelta);

    // dragging finished?
    bool bDragging = IsDragging();
    if (userState.m_spDragItem && userState.m_iButtonMask==0)
    {
      userState.m_spDragItem->OnDragEnd(userState.m_spMouseOverItem);
      userState.m_spDragItem = NULL;
    }

    // if no focus dialog set, use the first in hierarchy
    if (m_OpenDialogs.Count()>0)
    {
      if (!m_spFocusDlg)
        m_spFocusDlg = m_OpenDialogs.GetAt(0);
      userState.m_pCurrentCursor = m_spFocusDlg->GetCurrentCursor(userState);
    }
    else
    {
      m_spFocusDlg = NULL;
      userState.m_pCurrentCursor = NULL;
    }

    if (!userState.m_pCurrentCursor && m_bShowCursor)
      userState.m_pCurrentCursor = (m_spOverrideCursor!=NULL) ? m_spOverrideCursor.GetPtr() : GetManager()->GetDefaultCursor();
      
    // handle click event
    int iClicked = (userState.m_iButtonMask & (~iOldButton)) & BUTTONMASK_CLICKEVENTS;
    int iReleased = (iOldButton & (~userState.m_iButtonMask)) & BUTTONMASK_CLICKEVENTS;

    // Handle pointer down / up events
    if(iClicked)
    {
      VMenuEventDataObject data(&OnPointerDown, this, NULL, NULL, userState, iClicked);
      FillEvent(data);

      if (m_spFocusDlg != NULL) 
        m_spFocusDlg->OnPointerDown(&data);

      if(data.m_pItem != NULL)
        data.m_pItem->OnPointerDown(&data);

      OnPointerDown.TriggerCallbacks(&data);
    }

    if(iReleased)
    {
      VMenuEventDataObject data(&OnPointerUp, this, NULL, NULL, userState, iReleased);
      FillEvent(data);

      if (m_spFocusDlg != NULL) 
        m_spFocusDlg->OnPointerUp(&data);

      if(data.m_pItem != NULL)
        data.m_pItem->OnPointerUp(&data);

      OnPointerUp.TriggerCallbacks(&data);
    }

    // Handle OnItemClicked via pointer release
    if (iReleased)
    {
      // first check the focused window for a click
      VMenuEventDataObject data(&OnItemClick, this, NULL, NULL, userState, iReleased);
      FillEvent(data);

      // first, notify the dialog
      if (m_spFocusDlg != NULL) 
        m_spFocusDlg->OnClick(&data);

      if (data.m_pItem != NULL && data.m_pItem == userState.m_spMouseDownItem)
      {
        VASSERT(data.m_pDialog);
        data.m_pDialog->OnItemClicked(&data); // this one forwards it to the item
        OnItemClick.TriggerCallbacks(&data);
      }

      userState.m_fLastClickTime = 0.f;
      userState.m_iLastClickedMask = iReleased;
    }

    // check for click and doubleclick
    if (userState.m_fLastClickTime>=0.f) userState.m_fLastClickTime+=fTimeDelta;
    if (iClicked && !bDragging)
    {
      userState.m_spMouseDownItem = userState.m_spMouseOverItem;
      if (userState.m_spMouseDownItem != NULL)
        userState.m_spMouseDownItem->SetStatus(ITEMSTATUS_SELECTED); // highlight item under mouse

      if (iClicked == userState.m_iLastClickedMask)
      {
        if (userState.m_fLastClickTime>=0.f && userState.m_fLastClickTime<DOUBLECLICK_DELAY)
        {
          VMenuEventDataObject data(&OnItemDoubleClick, this, NULL,NULL,userState,iReleased);
          if (m_spFocusDlg) // first, notify the dialog
            m_spFocusDlg->OnDoubleClick(&data);
          FillEvent(data);
          if (data.m_pItem)
          {
            VASSERT(data.m_pDialog);
            data.m_pDialog->OnItemDoubleClicked(&data); // forwards it to the item
            OnItemDoubleClick.TriggerCallbacks(&data);
          }
        }
      }
      else
        INVALIDATE_DOUBLECLICK;
    }

    // mouse over
    if (userState.m_iButtonMask == 0 && m_spFocusDlg != NULL)
    {
      if (userState.m_spMouseOverItem != NULL)
        userState.m_spMouseOverItem->RemoveStatus(ITEMSTATUS_SELECTED);

      VMenuEventDataObject data(NULL, this, NULL, NULL, userState, userState.m_iButtonMask);
      m_spFocusDlg->FillEvent(&data); 

      SetMouseOverItem(userState, data.m_pItem);
    }

    // dragging?
    if (userState.m_iButtonMask && bMouseMoving)
    {
      if (!userState.m_spDragItem && userState.m_spMouseOverItem)
      {
        userState.m_spDragItem = userState.m_spMouseOverItem;
        userState.m_spDragItem->OnDragBegin(vOldMousePos, userState.m_iButtonMask); // pass the old mouse pos (where it started dragging)
      }
      if (userState.m_spDragItem)
        userState.m_spDragItem->OnDragging(vMouseDelta);
    }

    // show tooltip
    if (bMouseMoving)
    {
      SetTooltip(NULL); // remove tooltip if mouse is moving
    } 
    else if (userState.m_spMouseOverItem && m_spTooltip==NULL)
    {
      VTooltip *pTT = userState.m_spMouseOverItem->GetTooltip(userState);
      SetTooltip(pTT);
    }

    if (m_bHandleKeyboard)
    {
      int iModifier = 0;

    #ifdef SUPPORTS_KEYBOARD
      #ifdef DO_NOT_USE_VGL
          if (VInputManagerPC::GetKeyboard().GetRawControlValue(CT_KB_LSHIFT)) iModifier|=KEYMODIFIER_LSHIFT;
          if (VInputManagerPC::GetKeyboard().GetRawControlValue(CT_KB_RSHIFT)) iModifier|=KEYMODIFIER_RSHIFT;
          if (VInputManagerPC::GetKeyboard().GetRawControlValue(CT_KB_LCTRL)) iModifier|=KEYMODIFIER_LCTRL;
          if (VInputManagerPC::GetKeyboard().GetRawControlValue(CT_KB_RCTRL)) iModifier|=KEYMODIFIER_RCTRL;
          if (VInputManagerPC::GetKeyboard().GetRawControlValue(CT_KB_LALT)) iModifier|=KEYMODIFIER_LALT;
          if (VInputManagerPC::GetKeyboard().GetRawControlValue(CT_KB_RALT)) iModifier|=KEYMODIFIER_RALT;
      #else
          if (VGLIsKeyPressed(VGLK_LSHIFT)) iModifier|=KEYMODIFIER_LSHIFT;
          if (VGLIsKeyPressed(VGLK_RSHIFT)) iModifier|=KEYMODIFIER_RSHIFT;
          if (VGLIsKeyPressed(VGLK_LCTRL)) iModifier|=KEYMODIFIER_LCTRL;
          if (VGLIsKeyPressed(VGLK_RCTRL)) iModifier|=KEYMODIFIER_RCTRL;
          if (VGLIsKeyPressed(VGLK_LALT)) iModifier|=KEYMODIFIER_LALT;
          if (VGLIsKeyPressed(VGLK_RALT)) iModifier|=KEYMODIFIER_RALT;
      #endif
    #endif

    

      #if !defined(VISION_GUI_USE_WINDOWS_INPUT)
      // handle keyboard
      if (m_spFocusDlg)
      {
        for (int iKey=1;iKey<255;iKey++)
          if (HandleSingleKey(iKey))
            m_spFocusDlg->OnKeyPressed(iKey, iModifier);
      }
      #endif
    }
  }
}



void IVGUIContext::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  // render all dialogs in correct order
  m_OpenDialogs.RenderAll(Graphics,parentState);

  // render tooltip
  if (m_spTooltip)
    m_spTooltip->OnPaint(Graphics,parentState);

  for (int i=0;i<VGUIUserInfo_t::GUIMaxUser;i++)
  {
    VGUIUserInfo_t *pUser = GetUser((VGUIUserInfo_t::VGUIUserID_e)i);
    if (!pUser) continue;
    // render cursor on top
    Graphics.m_pCurrentUser = pUser;
    if (pUser->m_pCurrentCursor && m_bShowCursor)
      pUser->m_pCurrentCursor->OnPaint(Graphics,parentState);
  }
}


void IVGUIContext::OnRender()
{
  INSERT_PERF_MARKER_SCOPE("GUI Rendering");
  VISION_PROFILE_FUNCTION(VGUIManager::PROFILING_RENDERING);

  IVRender2DInterface* pRenderer = Vision::RenderLoopHelper.BeginOverlayRendering();

  VItemRenderInfo defaultState(this, NULL);
  VGraphicsInfo Graphics(*pRenderer, *this);
  OnPaint(Graphics,defaultState);

  Vision::RenderLoopHelper.EndOverlayRendering();
}



int IVGUIContext::ShowDialogModal(VDialog *pDialog, IVisApp_cl *pRunApp)
{
  pDialog->m_pContext = this;
  m_OpenDialogs.Add(pDialog);
  SetFocus(pDialog);
  m_OpenDialogs.SortByOrder();

  if (!pRunApp)
  {
    if (pDialog->m_pOwner) // try to get the same application as the parent dialog
      pRunApp = ((VDialog *)pDialog->m_pOwner)->GetModalApp();
    if (!pRunApp) // or get the global game application
      pRunApp = Vision::GetApplication();
  }
  VASSERT(pRunApp);
  pDialog->SetModalApp(pRunApp);

  // modal loop
  int iDlgResult = 0;
  while (1)
  {
    if (!pRunApp->Run()) // the app can also quit the dialog(?)
      break; 
    float dtime = Vision::GetUITimer()->GetTimeDifference();
    // OnTickFunction(dtime); // the OnFrameUpdate callback triggers this update so don't do it here
    iDlgResult = pDialog->GetDialogResult();
    if (iDlgResult) // wait until the dialog has a result
      break;
  }

  SetFocus(NULL); // otherwise it refuses to set the next one as focus dialog
  SetFocus((VDialog *)pDialog->m_pOwner);
  pDialog->SetModalApp(NULL);
  m_OpenDialogs.Remove(pDialog);

  return iDlgResult;
}


int IVGUIContext::ShowDialogModal(VDialog *pParent, const char *szDialogResource, const hkvVec2 &vPos, IVisApp_cl *pRunApp)
{
  VDialogPtr spDlg = GetManager()->CreateDialogInstance(szDialogResource,this,pParent,DIALOGFLAGS_MODAL);
  if (spDlg==NULL)
    return -1;

  hkvVec2 v = vPos + spDlg->GetStartPosition();

  spDlg->SetPosition(v.x,v.y);
  //spDlg->SetUseCaching(true); // [REMOVE]

  return ShowDialogModal(spDlg,pRunApp);
}


void IVGUIContext::ShowDialog(VDialog* pDialog)
{
  pDialog->m_pContext = this;
  m_OpenDialogs.Add(pDialog);
  SetFocus(pDialog);
  m_OpenDialogs.SortByOrder();
}


VDialog* IVGUIContext::ShowDialog(const char *szDialogResource)
{
  VDialog* pDlg = GetManager()->CreateDialogInstance(szDialogResource,this);
  if (!pDlg)
    return NULL;


  hkvVec2 vPos = pDlg->GetStartPosition();
  pDlg->SetPosition(vPos.x,vPos.y);
  ShowDialog(pDlg);
  return pDlg;
}


void IVGUIContext::CloseDialog(VDialog* pDialog)
{
  VASSERT(pDialog!=NULL && m_OpenDialogs.Contains(pDialog));
  if (m_spFocusDlg == pDialog)
  {
    SetFocus(NULL); // otherwise it refuses to set the next one as focus dialog
    SetFocus(static_cast<VDialog*>(pDialog->m_pOwner));
  }
  m_OpenDialogs.Remove(pDialog);
}


void IVGUIContext::SetFocus(VDialog *pDialog)
{
  if (pDialog == m_spFocusDlg) return;
  if (m_spFocusDlg != NULL && m_spFocusDlg->IsModal()) // cannot set because current one is modal
  {
    if (pDialog != NULL && !pDialog->IsModal())
      return;
  }

  if (m_spFocusDlg != NULL) // old dlg
  {
    m_spFocusDlg->OnSetFocus(false);

    // Check if dialog refuses to lose focus
    if (m_spFocusDlg->HasFocus())
      return;
  }

  if (pDialog != NULL) // new dlg
  {
    pDialog->OnSetFocus(true);

    // Check if dialog refuses to get focus
    if (!pDialog->HasFocus())
    {
      if (m_spFocusDlg != NULL)
        m_spFocusDlg->OnSetFocus(true);
      return;
    }

  #if !defined(SUPPORTS_MOUSE) && !defined(SUPPORTS_MULTITOUCH)
    // initially set the control with tab order 0
    int iIndex = pDialog->Items().GetItemIndexWithTabOrder(0);
    if (iIndex>=0)
    {
      for (int i=0;i<VGUIUserInfo_t::GUIMaxUser;i++)
      {
        VGUIUserInfo_t *pUser = GetUser((VGUIUserInfo_t::VGUIUserID_e)i);
        if (!pUser) continue;
        SetMouseOverItem(*pUser,pDialog->Items().GetAt(iIndex));
      }
    }
  #endif
  }
  else
  {
    for (int i=0;i<VGUIUserInfo_t::GUIMaxUser;i++)
    {
      VGUIUserInfo_t *pUser = GetUser((VGUIUserInfo_t::VGUIUserID_e)i);
      if (!pUser) continue;
      
      pUser->m_spMouseOverItem = NULL;
      if (pUser->m_spMouseDownItem != NULL)
        pUser->m_spMouseDownItem->RemoveStatus(ITEMSTATUS_SELECTED);
      pUser->m_spMouseDownItem = NULL;
    }
  }

  m_spFocusDlg = pDialog;
}


void IVGUIContext::SetMouseOverItem(VGUIUserInfo_t &user, VWindowBase *pItem)
{
  if (user.m_spMouseOverItem==pItem) return;
  if (user.m_spMouseOverItem)
    user.m_spMouseOverItem->OnMouseLeave(user);
  user.m_spMouseOverItem = pItem;
  if (pItem)
    pItem->OnMouseEnter(user);
}


void IVGUIContext::SetCurrentCursor(VCursor *pNewCursor) 
{
  if (pNewCursor) 
  {
    for (int i=0;i<VGUIUserInfo_t::GUIMaxUser;i++)
    {
      VGUIUserInfo_t *pUser = GetUser((VGUIUserInfo_t::VGUIUserID_e)i);
      if (!pUser) continue;
      pUser->m_pCurrentCursor = pNewCursor;
    }
  }

  m_spOverrideCursor = pNewCursor;
}


void IVGUIContext::FillEvent(VMenuEventDataObject &data)
{
  // focus dialog has priority
  if (m_spFocusDlg)
  {
    m_spFocusDlg->FillEvent(&data);
    if (data.m_bProcessed || m_spFocusDlg->IsModal())
      return;
  }

  // test all other dialogs whether clicked inside the dialog
  for (int i=0;i<m_OpenDialogs.Count() && !data.m_bProcessed;i++) 
  {
    if (m_OpenDialogs.GetAt(i)==m_spFocusDlg) continue;
    m_OpenDialogs.GetAt(i)->FillEvent(&data);
    if (data.m_bProcessed)
    {
      SetFocus(data.m_pDialog);
      return;
    }
  }
}



void IVGUIContext::SetTooltip(VTooltip *pTooltip) 
{
  m_spTooltip=pTooltip;
}


VTooltip* IVGUIContext::CreateTooltip(const char *szText)
{
  VTooltip *pTooltip;
  if (m_spTooltipFactory)
    pTooltip = m_spTooltipFactory->CloneTooltip();
  else
    pTooltip = new VTooltip(this);

  pTooltip->m_pContext = this;
  pTooltip->SetText(szText);
  return pTooltip;
}



IVisApp_cl* IVGUIContext::GetNullApp()
{
  if (!m_spNullApp)
    m_spNullApp = new VNullApp;
  return m_spNullApp;
}




///////////////////////////////////////////////////////////////////////////////////////////////
// VGUIMainContext
///////////////////////////////////////////////////////////////////////////////////////////////

VGUIMainContext::VGUIMainContext(VGUIManager *pManager) : IVGUIContext(pManager)
{      
  m_ClientRect.Set(0.f,0.f, (float)Vision::Video.GetXRes(), (float)Vision::Video.GetYRes());
  Vision::Callbacks.OnVideoChanged += this;
  m_bHandleKeyboard = true; // only if supported

  m_eCursorMode = CURSOR_DELTAS;  //work with deltas  (Controllers, Windows Input)

#if defined(WIN32) && defined(SUPPORTS_MOUSE)
  if (VGLGetMouseMethod()==VGL_DIRECTINPUT)
    m_eCursorMode = CURSOR_ABSPOS;  //map directly to cursor pos
#endif


#if defined(SUPPORTS_MULTITOUCH) || defined(_VISION_WIIU)
  m_eCursorMode = CURSOR_ABSPOS;  // always work with absolute cursor positions
#endif


}

VGUIMainContext::~VGUIMainContext()
{
  Vision::Callbacks.OnVideoChanged -= this;
}


void VGUIMainContext::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  IVGUIContext::OnHandleCallback(pData);

  // listen to this callback to update client area
  if (pData->m_pSender==&Vision::Callbacks.OnVideoChanged)
  {
    VisVideoChangedDataObject_cl *pVideo = (VisVideoChangedDataObject_cl*)pData;
    m_ClientRect.Set(0.f,0.f, (float)pVideo->m_pConfig->m_iXRes, (float)pVideo->m_pConfig->m_iYRes);
  }
}

#if defined(_VISION_XENON)
  #define VGUI_CURSORDELTA_RAW_MULTIPLIER 0.00045f
#elif defined(_VISION_PS3)
  #define VGUI_CURSORDELTA_RAW_MULTIPLIER 0.1f
#elif defined(_VISION_PSP2)
  #define VGUI_CURSORDELTA_RAW_MULTIPLIER 0.05f
#elif defined(_VISION_WIIU)
  #define VGUI_CURSORDELTA_RAW_MULTIPLIER 0.05f
#endif

//Helper function
void VGUIMainContext::GetCursorDelta(VGUIUserInfo_t &user, float &fDeltaX, float &fDeltaY)
{
#ifdef SUPPORTS_MOUSE
  fDeltaX = (float)VInputManager::GetMouse().GetRawControlValue(CT_MOUSE_ABS_DELTA_X);
  fDeltaY = (float)VInputManager::GetMouse().GetRawControlValue(CT_MOUSE_ABS_DELTA_Y);
#else
  fDeltaX = fDeltaY = 0.0f;
#endif


#if defined(_VISION_XENON) || defined(_VISION_PS3) || defined(_VISION_PSP2) || defined(_VISION_WIIU)
  float fTimeDiff = Vision::GetUITimer()->GetTimeDifference();

  float fDeltaXPad;
  float fDeltaYPad;

#if defined(_VISION_WIIU)
  fDeltaXPad = (float)VInputManager::GetDRC(m_drcIndex).GetRawControlValue(CT_PAD_LEFT_THUMB_STICK_UNTRIGGERABLE_RAW_X) * VGUI_CURSORDELTA_RAW_MULTIPLIER;
  fDeltaYPad = (float)-VInputManager::GetDRC(m_drcIndex).GetRawControlValue(CT_PAD_LEFT_THUMB_STICK_UNTRIGGERABLE_RAW_Y) * VGUI_CURSORDELTA_RAW_MULTIPLIER;
#else
  fDeltaXPad = (float)VInputManager::GetPad(0).GetRawControlValue(CT_PAD_LEFT_THUMB_STICK_UNTRIGGERABLE_RAW_X) * VGUI_CURSORDELTA_RAW_MULTIPLIER;
  fDeltaYPad = (float)-VInputManager::GetPad(0).GetRawControlValue(CT_PAD_LEFT_THUMB_STICK_UNTRIGGERABLE_RAW_Y) * VGUI_CURSORDELTA_RAW_MULTIPLIER;
#endif

  if(fDeltaXPad<=3.0f && fDeltaXPad>=-3.0f){
    fDeltaXPad = 0.0f;
  } else {
      fDeltaXPad *= fDeltaXPad*fDeltaXPad*fTimeDiff;
  }

  if(fDeltaYPad<=3.0f && fDeltaYPad>=-3.0f){
    fDeltaYPad = 0.0f;
  } else {
    fDeltaYPad *= fDeltaYPad*fDeltaYPad*fTimeDiff;
  }

  fDeltaX += fDeltaXPad;
  fDeltaY += fDeltaYPad;
#endif
}

//Update the cursor's position using the engine's input controller classes
void VGUIMainContext::UpdateCursorPos(VGUIUserInfo_t &userState)
{
#if defined(SUPPORTS_MULTITOUCH)
  IVMultiTouchInput* pMultiTouch = NULL;
  if( VInputManager::IsInitialized() )
    pMultiTouch = static_cast<IVMultiTouchInput*>(&VInputManager::GetInputDevice(INPUT_DEVICE_TOUCHSCREEN));

  // Use both the touchscreen and the thumbstick on PSP2
  #if defined(_VISION_PSP2)
    if (pMultiTouch != NULL && !pMultiTouch->GetNumberOfTouchPoints())
    {
      m_eCursorMode = CURSOR_DELTAS;
    }
  #endif
#elif defined(_VISION_WIIU)
  if (!VInputManager::GetDRC(m_drcIndex).GetControlValue(CT_PAD_WIIU_TOUCH_ANY, 0.0f))
  {
    m_eCursorMode = CURSOR_DELTAS;
  }
#endif

  if (m_eCursorMode == CURSOR_DELTAS)
  {
    // When we use Windows Input or console controllers, we accumulate deltas with sensitivity
    hkvVec2 vMouseDelta;
    GetCursorDelta(userState, vMouseDelta.x, vMouseDelta.y);
    userState.m_vMousePosAccum += vMouseDelta;
  } 
  
#if defined(SUPPORTS_MULTITOUCH) || defined(_VISION_WIIU)
  // Use both the touchscreen and the thumbstick on PSP2 and WiiU
  m_eCursorMode = CURSOR_ABSPOS;
#endif

  if (m_eCursorMode == CURSOR_ABSPOS)
  {
    // Use the Windows cursor position directly
#ifdef SUPPORTS_MOUSE
    if(VInputManager::GetMouse().IsActive())
    {
      userState.m_vMousePosAccum.set(
          (float)VInputManager::GetMouse().GetRawControlValue(CT_MOUSE_RAW_CURSOR_X),
          (float)VInputManager::GetMouse().GetRawControlValue(CT_MOUSE_RAW_CURSOR_Y)
        );
    }

#endif

#if defined(SUPPORTS_MULTITOUCH)
    if (pMultiTouch != NULL && pMultiTouch->GetNumberOfTouchPoints())
    {
      int sx, sy;
      VisRenderContext_cl::GetMainRenderContext()->GetSize(sx, sy);
      userState.m_vMousePosAccum.x = pMultiTouch->GetControlValue(CT_TOUCH_POINT_0_X, 0) * float(sx);
      userState.m_vMousePosAccum.y = pMultiTouch->GetControlValue(CT_TOUCH_POINT_0_Y, 0) * float(sy);
  
      // assume the fingertip width is about a tenth of the screen
      //userState.m_vMousePosAccum.x -= sx * 0.1f;
      //userState.m_vMousePosAccum.y -= sy * 0.1f;
    }

#elif defined(_VISION_WIIU)
    if (VInputManager::GetDRC(m_drcIndex).GetControlValue(CT_PAD_WIIU_TOUCH_ANY, 0.0f))
    {
      const float sx = m_ClientRect.GetSizeX();
      const float sy = m_ClientRect.GetSizeY();

      // Raw touch intput resolution on the WiiU is 1280x720, thus we have to scale the input to the size of the context the GUI is displayed in
      userState.m_vMousePosAccum.x = (VInputManager::GetDRC(m_drcIndex).GetControlValue(CT_PAD_WIIU_TOUCH_RAW_X, 0.0f) / 1280.0f) * sx;
      userState.m_vMousePosAccum.y = (VInputManager::GetDRC(m_drcIndex).GetControlValue(CT_PAD_WIIU_TOUCH_RAW_Y, 0.0f) / 720.0f) * sy;
    }

#endif
  }

  // cursor positions should always be integer because of texture filtering
  GetClientRect().Clamp(userState.m_vMousePosAccum);
  userState.m_vMousePos.x = floorf(userState.m_vMousePosAccum.x);
  userState.m_vMousePos.y = floorf(userState.m_vMousePosAccum.y);
}


int VGUIMainContext::GetButtonMask(VGUIUserInfo_t &user)
{
  int iMask = 0;
#ifdef SUPPORTS_KEYBOARD
  if (VInputManager::GetKeyboard().GetRawControlValue(CT_KB_UP))      iMask |= BUTTON_PREV;
  if (VInputManager::GetKeyboard().GetRawControlValue(CT_KB_DOWN))    iMask |= BUTTON_NEXT;
  if (m_bKeyboardEnterAsMouseLeftClick)
  {
    if (VInputManager::GetKeyboard().GetRawControlValue(CT_KB_ENTER))   iMask |= BUTTON_LMOUSE;
  }
#endif

#if defined (SUPPORTS_MOUSE)
  if (VInputManager::GetMouse().GetRawControlValue(CT_MOUSE_LEFT_BUTTON)) iMask |= BUTTON_LMOUSE;
  if (VInputManager::GetMouse().GetRawControlValue(CT_MOUSE_RIGHT_BUTTON)) iMask |= BUTTON_RMOUSE;
  if (VInputManager::GetMouse().GetRawControlValue(CT_MOUSE_MIDDLE_BUTTON)) iMask |= BUTTON_MMOUSE;
#endif


#if defined (_VISION_XENON)
  if (VInputManagerXenon::GetPad(0).GetRawControlValue(CT_PAD_A) || VInputManagerXenon::GetPad(0).GetControlValue(CT_PAD_LEFT_THUMB, 0.24f)) iMask |= BUTTON_LMOUSE;
  if (VInputManagerXenon::GetPad(0).GetRawControlValue(CT_PAD_B)) iMask |= BUTTON_RMOUSE;
  if (VInputManagerXenon::GetPad(0).GetRawControlValue(CT_PAD_X)) iMask |= BUTTON_MMOUSE;
#endif



#if defined (_VISION_PS3)
  if (VInputManagerPS3::GetPad(0).GetRawControlValue(CT_PAD_CROSS) || VInputManagerPS3::GetPad(0).GetControlValue(CT_PAD_LEFT_THUMB, 0.38f)) iMask |= BUTTON_LMOUSE;
  if (VInputManagerPS3::GetPad(0).GetRawControlValue(CT_PAD_CIRCLE)) iMask |= BUTTON_RMOUSE;
  if (VInputManagerPS3::GetPad(0).GetRawControlValue(CT_PAD_SQUARE)) iMask |= BUTTON_MMOUSE;
#endif



#if defined (_VISION_PSP2)
  if (VInputManagerPSP2::GetPad(0).GetRawControlValue(CT_PAD_CROSS) || VInputManagerPSP2::GetPad(0).GetControlValue(CT_PAD_LEFT_THUMB, 0.38f)) iMask |= BUTTON_LMOUSE;
  if (VInputManagerPSP2::GetPad(0).GetRawControlValue(CT_PAD_CIRCLE)) iMask |= BUTTON_RMOUSE;
  if (VInputManagerPSP2::GetPad(0).GetRawControlValue(CT_PAD_SQUARE)) iMask |= BUTTON_MMOUSE;
#endif



#if defined (_VISION_WIIU)
  if (VInputManagerWiiU::GetDRC(m_drcIndex).GetRawControlValue(CT_PAD_B)) iMask |= BUTTON_LMOUSE;
  if (VInputManagerWiiU::GetDRC(m_drcIndex).GetControlValue(CT_PAD_WIIU_TOUCH_ANY, 0.0f)) iMask |= BUTTON_LMOUSE;
  if (VInputManagerWiiU::GetDRC(m_drcIndex).GetRawControlValue(CT_PAD_A)) iMask |= BUTTON_RMOUSE;
  if (VInputManagerWiiU::GetDRC(m_drcIndex).GetRawControlValue(CT_PAD_Y)) iMask |= BUTTON_MMOUSE;
#endif



#if defined(SUPPORTS_MULTITOUCH)
  if(VInputManager::IsInitialized())
  {
    IVMultiTouchInput* pMultiTouch = static_cast<IVMultiTouchInput*>(&VInputManager::GetInputDevice(INPUT_DEVICE_TOUCHSCREEN));
    if (!pMultiTouch->GetNumberOfTouchPoints())
    {
      //iMask &= ~BUTTON_LMOUSE;
    }
    else
    {
      iMask |= BUTTON_LMOUSE;
    }
  }
#endif


  return iMask;
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
