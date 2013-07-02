/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VGUIContext.hpp

#ifndef VGUICONTEXT_HPP_INCLUDED
#define VGUICONTEXT_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VDialog.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VTooltip.hpp>


extern VModule g_VisionEngineModule;

class VGUIManager;
class IVGUIContext;
class VGUIMainContext;
typedef VSmartPtr<IVGUIContext> IVGUIContextPtr;
typedef VSmartPtr<VGUIMainContext> VGUIMainContextPtr;

class VMenuEventDataObject;
class VCursor;
class IVMenuSystemResource;
class VDialogResource;
class VGUIManager;
class VDialog;
class VisFont_cl;

typedef VSmartPtr<VCursor> VCursorPtr;
typedef VSmartPtr<VDialog> VDialogPtr;


/// \brief
///   Class that represents a GUI context bound to a render target
/// 
/// A GUI context represents on instance of a GUI state that is running independently from other
/// GUI instances.
/// 
/// For instance the HUD is a GUI context that runs on the main view or a monitor texture in the
/// game is a GUI context.
/// 
/// However, GUI contexts can share the same resources (dialog resources etc.) if they are using
/// the same VGUIManager.
class IVGUIContext : public VRefCounter, public IVisCallbackHandler_cl, public VUserDataObj
{
public:

  /// \brief
  ///   Constructor of the GUI context instance
  GUI_IMPEXP IVGUIContext(VGUIManager *pManager);

  /// \brief
  ///   Destructor
  GUI_IMPEXP virtual ~IVGUIContext();

  /// \brief
  ///   Allows for changing the entry point in renderloops for rendering. Can be any value of VRenderHook_e. The default is
  ///   VRH_GUI.
  inline void SetRenderHookConstant(int iEntryPoint=VRH_GUI)
  {
    m_iRenderHookConstant = iEntryPoint;
  }

  /// \brief
  ///   Set the filter bitmask for rendering this GUI only in some render contexts
  /// 
  /// This GUI will only be rendered in render contexts, if the result of the logical AND
  /// operation of this bitmask and the context's bitmask (see VisRenderContext_cl::SetRenderFilterMask) 
  /// is not zero.
  ///
  /// @param iMask
  ///   the visibility bitmask to set
  inline void SetVisibleBitmask(unsigned int iMask)
  {
    m_iVisibleBitmask = iMask;
  }

  /// \brief
  ///   Gets the filter bitmask that has been set via SetVisibleBitmask
  inline unsigned int GetVisibleBitmask() const 
  { 
    return m_iVisibleBitmask; 
  }

  /// \brief
  ///   Called once per frame
  GUI_IMPEXP virtual void OnTickFunction(float fTimeDelta);

  /// \brief
  ///   Virtual function that can be overidden to update the (mouse) cursor's 
  ///   position for this GUI context. 
  //    Called by the context's OnTickFunction
  GUI_IMPEXP virtual void UpdateCursorPos(VGUIUserInfo_t &user) {}

  /// \brief
  ///   Pure virtual function that must be overidden to evaluate the mouse button bit mask of
  ///   currently pressed buttons (BUTTON_LMOUSE,...)
  GUI_IMPEXP virtual int GetButtonMask(VGUIUserInfo_t &user) = 0;

  /// \brief
  ///   Overridable to render all open dialogs
  GUI_IMPEXP virtual void OnRender();

  /// \brief
  ///   This virtual function gets called from OnRender and paints the open dialogs, mouse cursor
  ///   etc.
  GUI_IMPEXP virtual void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState);

  /// \brief
  ///   Get the GUI manager
  inline VGUIManager *GetManager() const {return m_pManager;}

  /// \brief
  ///   Activate or deactivate this context
  ///
  /// Please note that deactivating the context will cleanup its internal data and thus
  /// for example clear the list of dialogs assigned to this context. Deactivating a context
  /// is thus typically only done before the context is released/destroyed.
  ///
  /// \param bStatus
  ///   TRUE for activating, FALSE for deactivating
  GUI_IMPEXP virtual void SetActivate(bool bStatus);
  
  /// \brief
  ///   Returns whether this context is active
  inline bool IsActive() const { return m_bActive; }

  /// \brief
  ///   Shows or hides the mouse cursor for this context
  inline void SetShowCursor(bool bStatus) {m_bShowCursor=bStatus;}

  /// \brief
  ///   Shows or hides the mouse cursor for this context
  inline bool GetShowCursor() const {return m_bShowCursor;}

  /// \brief
  ///   If enabled, this context processes keyboard input (if available) and forwards key values to
  ///   the controls. Not recommended for HUD
  inline void SetHandleKeyboard(bool bStatus) {m_bHandleKeyboard=bStatus;}

  /// \brief
  ///   Returns the current keyboard status
  inline bool GetHandleKeyboard() const {return m_bHandleKeyboard;}

  /// \brief
  ///   Activate or deactivate that keyboard enter is treated as mouse right click
  inline void SetKeyboardEnterAsMouseLeftClick(bool bStatus) {m_bKeyboardEnterAsMouseLeftClick = bStatus;}

  /// \brief
  ///   returns if keyboard enter is treated as mouse right click mode is activated
  inline bool GetKeyboardEnterAsMouseLeftClick() const {return m_bKeyboardEnterAsMouseLeftClick;}

  
  /// \brief
  ///   Gets the current mouse position (clamped to client rect)
  inline const hkvVec2& GetCurrentMousePos(const VGUIUserInfo_t *pUser=NULL) const 
  {
    int i = (pUser!=NULL) ? pUser->m_iID : 0;
    return m_UserStates[i].m_vMousePos;
  }

  /// \brief
  ///   Sets a new mouse position. No validation takes place.
  inline void SetCurrentMousePos(const hkvVec2& vNewPos, const VGUIUserInfo_t *pUser=NULL) 
  {
    int i = (pUser!=NULL) ? pUser->m_iID : 0;
    VGUIUserInfo_t &userState(m_UserStates[i]);
    userState.m_vMousePosAccum = vNewPos;
    userState.m_vMousePos.set(floorf(vNewPos.x), floorf(vNewPos.y));
  }

  /// \brief
  ///   Get the bimask of pressed mouse buttons (BUTTON_LMOUSE,...)
  inline int GetCurrentButtonMask(const VGUIUserInfo_t *pUser=NULL) const 
  {
    int i = (pUser!=NULL) ? pUser->m_iID : 0;
    return m_UserStates[i].m_iButtonMask;
  }

  /// \brief
  ///   Returns the client rectangle of the context, e.g. screen rectangle (0/0/resX/resY)
  inline const VRectanglef& GetClientRect() const 
  { 
    VASSERT(m_ClientRect.IsValid());
    return m_ClientRect;
  }

  /// \brief
  ///   Sets the client rectangle of the context.
  inline void SetClientRect(const VRectanglef& rect)
  {
    VASSERT(rect.IsValid());
    m_ClientRect = rect;
  }

  /// \brief
  ///   Sets the item that is under the mouse cursor
  GUI_IMPEXP void SetMouseOverItem(VGUIUserInfo_t &user, VWindowBase *pItem);

  /// \brief
  ///   Indicates whether constex is currently in dragging mode
  inline bool IsDragging(const VGUIUserInfo_t *pUser=NULL) const 
  {
    int i = (pUser!=NULL) ? pUser->m_iID : 0;
    return m_UserStates[i].m_spDragItem!=NULL;
  }

  /// \brief
  ///   Returns the current set drag threshold
  inline float GetDragThreshold() const
  {
    return m_fDragThreshold;
  }

  /// \brief
  ///   Sets the drag threshold (input movement distance after which consectuive actions aren't considered to be on the same spot)
  inline void SetDragThreshold(float fDragThreshold)
  {
    m_fDragThreshold = fDragThreshold;
  }

  /// \brief
  ///   Gets the current cursor
  inline VCursor *GetCurrentCursor(const VGUIUserInfo_t *pUser=NULL) const 
  {
    int i = (pUser!=NULL) ? pUser->m_iID : 0;
    return m_UserStates[i].m_pCurrentCursor;
  }

  /// \brief
  ///   Sets an override cursor that is used instead of the manager's default cursor. Use
  ///   SetCurrentCursor(NULL) to reset the setting
  GUI_IMPEXP void SetCurrentCursor(VCursor *pNewCursor);


  /// \brief
  ///   Sets the dialog that has the focus
  GUI_IMPEXP void SetFocus(VDialog *pDialog);

  /// \brief
  ///   Shows a dialog in modal mode. It calls the Run function of the passed application until the
  ///   dialog returns a dialog result.
  ///
  /// The dialog will run as long as GetDialogResult() returns zero.
  /// 
  /// \param pParent
  ///   Parent dialog, can be NULL
  /// 
  /// \param szDialogResource
  ///   Resource filename (.xml) used to create the dialog
  /// 
  /// \param vPos
  ///   Start position on scren, added to the StartPosition of the dialog
  /// 
  /// \param pRunApp
  ///   Application used to run this dialog in modal mode. Can be NULL to use the current
  ///   application.
  ///
  /// \sa SetDialogResult
  GUI_IMPEXP int ShowDialogModal(VDialog *pParent, const char *szDialogResource, const hkvVec2 &vPos, IVisApp_cl *pRunApp=NULL);

  /// \brief
  ///   This version of ShowDialogModal uses a dialog instance directly
  GUI_IMPEXP int ShowDialogModal(VDialog *pDialog, IVisApp_cl *pRunApp=NULL);

  /// \brief
  ///   Show a dialog in non-modal mode. The dialog is created from the resource
  GUI_IMPEXP VDialog* ShowDialog(const char *szDialogResource);

  /// \brief
  ///   This version of ShowDialog takes an existing instance of a dialog
  GUI_IMPEXP void ShowDialog(VDialog* pDialog);

  /// \brief
  ///   CLoses a dialog that has been enabled via ShowDialog. Modal dialogs should be closed using
  ///   pDialog->SetDialogResult instead
  GUI_IMPEXP void CloseDialog(VDialog* pDialog);

  /// \brief
  ///   Shows a tooltip
  GUI_IMPEXP void SetTooltip(VTooltip *pTooltip);

  /// \brief
  ///   Shows a tooltip text message. This version creates a default tooltip object.
  inline void SetTooltipText(const char *szText)
  {
    SetTooltip(CreateTooltip(szText));
  }

  /// \brief
  ///   Creates a new default tooltip instance but does not show it. This function is called by the base implementation VWindowBase::GetTooltip()
  GUI_IMPEXP VTooltip* CreateTooltip(const char *szText);

  /// \brief
  ///   Sets one global instance of a tooltip that is used as a template for all default tooltips. Uses virtual VTooltip::CloneTooltip() function
  inline void SetTooltipTemplate(VTooltip *pTooltip)
  {
    m_spTooltipFactory = pTooltip;
  }

  /// \brief
  ///   Sets a bitmask that defines the active users. Each bit is of type 1<<VGUIUserInfo_t::GUIUserX
  inline void SetActiveUsers(int iUserMask=(1<<VGUIUserInfo_t::GUIUser0))
  {
    m_iUserIDMask = iUserMask;
  }

  /// \brief
  ///   Returns the current bitmask of active users
  inline int GetActiveUsers() const
  {
    return m_iUserIDMask;
  }

  //inline VGUIUserInfo_t& CurrentUser() const {return *m_pCurrentUser;}
  
  /// \brief
  ///   Returns the user specific data structure for the passed ID. Returns NULL if the user is not active.
  inline VGUIUserInfo_t* GetUser(VGUIUserInfo_t::VGUIUserID_e iID)
  {
    if ((m_iUserIDMask&(1<<iID))==0)
      return NULL;
    return &m_UserStates[iID];
  }

  /// \brief
  ///   Internal function
  GUI_IMPEXP void ResetKeyboardStates();

  /// \brief
  ///   Internal function
  GUI_IMPEXP bool HandleSingleKey(int iKey);

  /// \brief
  ///   Accesses the collection of open dialogs
  VDialogCollection m_OpenDialogs;

  /// \brief
  ///   Return the dialog that currently has the focus
  GUI_IMPEXP VDialog* GetFocusDialog() const {return m_spFocusDlg;}

  /// \brief
  ///   Callback event that is triggered if a control is clicked on. The data object can be casted
  ///   to VMenuEventDataObject
  VisCallback_cl OnItemClick;

  /// \brief
  ///   Callback event that is triggered if a control is double clicked. The data object can be
  ///   casted to VMenuEventDataObject
  VisCallback_cl OnItemDoubleClick;

  /// \brief
  ///   Callback event that is triggered when a pointer goes down on a control. The data object can be
  ///   casted to VMenuEventDataObject
  VisCallback_cl OnPointerDown;

  /// \brief
  ///   Callback event that is triggered when a pointer goes up on a control. The data object can be
  ///   casted to VMenuEventDataObject
  VisCallback_cl OnPointerUp;

  /// \brief
  ///   Callback event that is triggered if a control changes its value, e.g. a slider or checkbox.
  ///   The data object can be casted to VItemValueChangedEvent
  VisCallback_cl OnItemValueChanged;


  /// \brief
  ///   Returns an application instance that does not do anything game loop specific, e.g. entity
  ///   thinking. It can thus be used to pass it to modal dialogs
  GUI_IMPEXP IVisApp_cl* GetNullApp();

#if defined(_VISION_WIIU)
  /// \brief
  ///   Sets the index of the DRC that should be used by the context. This can be either V_DRC_FIRST or V_DRC_SECOND.
  void SetDrcIndex(VWiiUDRC drcIndex) { m_drcIndex = drcIndex; }

  /// \brief
  ///   Returns the index of the DRC that is used by the context. This can be either V_DRC_FIRST or V_DRC_SECOND.
  VWiiUDRC GetDrcIndex() const { return m_drcIndex; }
#endif

protected:
  GUI_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData);
  GUI_IMPEXP void FillEvent(VMenuEventDataObject &data);

protected:
  VGUIManager *m_pManager;
  
  VRectanglef m_ClientRect;
  VDialogPtr m_spFocusDlg;
  VTooltipPtr m_spTooltip;
  VTooltipPtr m_spTooltipFactory;
  IVisAppPtr m_spNullApp;
  bool m_bKeyboardEnterAsMouseLeftClick;

#ifdef SUPPORTS_KEYBOARD
  bool m_bKeyPressed[256]; ///< keyboard status
#endif
  bool m_bActive, m_bShowCursor, m_bHandleKeyboard;

  VCursorPtr m_spOverrideCursor;
  int m_iRenderHookConstant;
  unsigned int m_iVisibleBitmask;

  //VGUIUserInfo_t *m_pCurrentUser;
  int m_iUserIDMask;

  VGUIUserInfo_t m_UserStates[VGUIUserInfo_t::GUIMaxUser];

#if defined(_VISION_WIIU)
  VWiiUDRC m_drcIndex;
#endif

  float m_fDragThreshold;
};


/// \brief
///   Collection of GUI contexts (refcounted)
class VGUIContextCollection : public  VRefCountedCollection<IVGUIContext>
{
};


/// \brief
///   Derived class that implements the UpdateCursorPos() and GetButtonMask() using
///   the engine's input controller classes. Can be used for a main GUI
class VGUIMainContext : public IVGUIContext
{
public:

  /// \brief
  ///   Context constructor
  GUI_IMPEXP VGUIMainContext(VGUIManager *pManager);

  /// \brief
  ///   Context destructor
  GUI_IMPEXP virtual ~VGUIMainContext();

  /// \brief
  ///   Update the cursor's position using the engine's input controller classes
  GUI_IMPEXP virtual void UpdateCursorPos(VGUIUserInfo_t &user);

  /// \brief
  ///   Implementation that returns a bitmask of mouse buttons.
  GUI_IMPEXP virtual int GetButtonMask(VGUIUserInfo_t &user);

  /// \brief
  ///   Listen to some more callbacks, e.g. Video settings changed
  GUI_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData);
  
 
  /// \brief Describes the different modes that a cursor can have.
  enum CURSOR_MODE
  { 
    CURSOR_NONE   = 0,    ///< Don't update cursor position
    CURSOR_DELTAS = 1,    ///< DirectX/Controllers: Accumulate deltas with sensitivity
    CURSOR_ABSPOS = 2     ///< Windows Input: Map to the Windows cursor position directly
  };
  
  CURSOR_MODE m_eCursorMode;

  /// \brief
  ///   Helper for internal use that evaluates the cursor delta from the controllers
  GUI_IMPEXP void GetCursorDelta(VGUIUserInfo_t &user, float &fDeltaX, float &fDeltaY);
  
};

#endif

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
