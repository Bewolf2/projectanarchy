/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VWindowBase.hpp

#ifndef VWINDOW_HPP_INCLUDED
#define VWINDOW_HPP_INCLUDED

/// \brief
///   Status flags for UI items
enum VItemStatus
{
  ITEMSTATUS_VISIBLE             = V_BIT(0),    ///< Whether a UI item is visible at all
  ITEMSTATUS_ENABLED             = V_BIT(1),    ///< Whether a UI item is enabled and can be interacted with
  ITEMSTATUS_HASFOCUS            = V_BIT(3),    ///< Whether a UI item has the focus for keyboard input
  ITEMSTATUS_SELECTED            = V_BIT(4),    ///< Whether a UI item is currently selected
  ITEMSTATUS_STAYSELECTED        = V_BIT(5),    ///< Whether a UI item is supposed to stay selected
  ITEMSTATUS_MOUSEOVER_USER0     = V_BIT(8),    ///< Whether user 0 is hovering over the item
  ITEMSTATUS_MOUSEOVER_USER1     = V_BIT(9),    ///< Whether user 1 is hovering over the item
  ITEMSTATUS_MOUSEOVER_USER2     = V_BIT(10),   ///< Whether user 2 is hovering over the item
  ITEMSTATUS_MOUSEOVER_USER3     = V_BIT(11),   ///< Whether user 3 is hovering over the item
  ITEMSTATUS_MOUSEOVER           = (ITEMSTATUS_MOUSEOVER_USER0 | ITEMSTATUS_MOUSEOVER_USER1 | ITEMSTATUS_MOUSEOVER_USER2 | ITEMSTATUS_MOUSEOVER_USER3), ///< Whether any user is hovering over the item
  ITEMSTATUS_VALIDPOSITION       = V_BIT(16),   ///< Whether a UI item has a valid position
  ITEMSTATUS_VALIDSIZE           = V_BIT(17),   ///< Whether a UI item has a valid size
};

#define PARENTCHANGED_POSITION     V_BIT(0)
#define PARENTCHANGED_SIZE         V_BIT(1)

struct VGUIUserInfo_t;
class VTooltip;
class IVGUIContext;
class VCursor;
class VGUIManager;
class VWindowBase;
class VMenuEventDataObject;
class VItemValueChangedEvent;
typedef VSmartPtr<VWindowBase> VWindowBasePtr;
typedef VSmartPtr<VCursor> VCursorPtr;





/// \brief
///   Base class of a window instance. This common base class is used for dialogs, buttons or tooltips.
class VWindowBase : public VisTypedEngineObject_cl, public IVRenderableItem, public VRefCounter, public VUserDataObj
{
public:
  GUI_IMPEXP VWindowBase();
  GUI_IMPEXP virtual ~VWindowBase();

  /// \brief
  ///   Enum that defines the 4 common states of a window
  enum ControlState_e
  {
    DISABLED  = 0,  ///< the window is disabled (grayed out)
    NORMAL    = 1,  ///< normal state
    MOUSEOVER = 2,  ///< the mouse is over the control
    SELECTED  = 3,  ///< the control is selected or highlighted because the user clicks on it

    STATE_COUNT = 4 ///< for array lookups
  };


  /// \brief
  ///   Gets the parent window.
  inline VWindowBase *GetParent() const {return m_pOwner;}

  /// \brief
  ///   Sets the parent window.
  inline void SetParent(VWindowBase *pOwner) {m_pOwner=pOwner;if (pOwner) m_pContext=pOwner->m_pContext;}

  /// \brief
  ///   Gets the owner GUI manager.
  GUI_IMPEXP VGUIManager *GetMenuManager() const;

  /// \brief
  ///   Gets the owner GUI context.
  inline IVGUIContext *GetContext() const 
  {
    if (VISION_UNLIKELY(m_pContext==NULL && m_pOwner!=NULL))
      m_pContext = m_pOwner->GetContext(); // recurse
    return m_pContext;
  }

  /// \brief
  ///   Gets the ID of the window.
  inline int GetID() const {return m_iID;}

  /// \brief
  ///   Gets the dialog result of the control. For dialog controls this is the ID that they return
  ///   if the control is clicked.
  GUI_IMPEXP virtual int GetDialogResult() const {return m_iDialogResult;}

  /// \brief
  ///   Sets the new dialog result. Can be used to close an open dialog.
  GUI_IMPEXP virtual void SetDialogResult(int iResult) {m_iDialogResult=iResult;}

  /// \brief
  ///   The dialog and all its children will refresh their data. Has to be implemented by the user.
  GUI_IMPEXP virtual void Refresh() {InvalidateCache();}


  ///
  /// @name Window Status Functions
  /// @{
  ///


  /// \brief
  ///   Control visible status.
  inline bool IsVisible() const {return IsStatusSet(ITEMSTATUS_VISIBLE);}

  /// \brief
  ///   Control enabled status.
  inline bool IsEnabled() const {return IsStatusSet(ITEMSTATUS_ENABLED);}

  /// \brief
  ///   Status that indicates whether the cursor is over the control.
  inline bool IsMouseOver() const {return IsStatusSet(ITEMSTATUS_MOUSEOVER);}

  /// \brief
  ///   Control has focus status.
  inline bool HasFocus() const {return IsStatusSet(ITEMSTATUS_HASFOCUS);}

  /// \brief
  ///   Control selected status.
  inline bool IsSelected() const {return IsStatusSet(ITEMSTATUS_SELECTED|ITEMSTATUS_STAYSELECTED);}

  /// \brief
  ///   Indicates whether control is static (i.e. has ID VGUIManager::ID_STATIC).
  inline bool IsStatic() const {return m_iID==0;/*VGUIManager::ID_STATIC;*/}

  /// \brief
  ///   Translates the current states into a state mode.
  GUI_IMPEXP ControlState_e GetCurrentState(bool bForceDisabled=false) const;

  /// \brief
  ///   Query status bit.
  inline bool IsStatusSet(int iMask) const {return (m_iStatus&iMask)>0;}

  /// \brief
  ///   Set status bit.
  GUI_IMPEXP void SetStatus(int iMask, bool bSet=true);

  /// \brief
  ///   Remove status bit.
  GUI_IMPEXP void RemoveStatus(int iMask);


  /// \brief
  ///   Flags the control to stay selected
  inline void SetSelected(bool bStatus) {SetStatus(ITEMSTATUS_STAYSELECTED,bStatus);}

  /// \brief
  ///   Set the visible status
  inline void SetVisible(bool bStatus) {SetStatus(ITEMSTATUS_VISIBLE,bStatus);}

  /// \brief
  ///   Set the enabled status
  inline void SetEnabled(bool bStatus) {SetStatus(ITEMSTATUS_ENABLED,bStatus);}


  ///
  /// @}
  ///

  ///
  /// @name Position and Size
  /// @{
  ///



  /// \brief
  ///   Set the position of the control (always relative to parent control)
  GUI_IMPEXP void SetPosition(float x, float y);

  /// \brief
  ///   Set the size of the control
  GUI_IMPEXP void SetSize(float dx, float dy);

  /// \brief
  ///   Get the position of the control (always relative to parent control)
  inline const hkvVec2& GetPosition() const {return m_vPosition;}

  /// \brief
  ///   Get the absolute position of the control
  GUI_IMPEXP hkvVec2 GetAbsPosition() const;

  /// \brief
  ///   Get the size of the control
  inline const hkvVec2& GetSize() const {return m_vSize;}

  /// \brief
  ///   Get the absolute bounding box of the control
  GUI_IMPEXP VRectanglef GetBoundingBox() const;


  /// \brief
  ///   Get the border of the control in all 4 directions for clipping. The result is not a valid
  ///   rectangle but for instance (4,4,4,4)
  inline const VRectanglef& GetClientAreaBorder() const {return m_ClientAreaBorder;}


  /// \brief
  ///   Sets a new order key value for this window. Items are sorted back to front at dialog
  ///   activation time. Open dialogs are sorted when a new dialog is opened.
  inline void SetOrder(int iOrder) {m_iOrder=iOrder;}

  /// \brief
  ///   Gets the order that has been set via SetOrder (resp. with the XML attribute)
  inline int GetOrder() const {return m_iOrder;}

  ///
  /// @}
  ///

  ///
  /// @name New Virtual Functions
  /// @{
  ///


  /// \brief
  ///   Get the client rectangle (absolute positions) for clipping children. Default implementation
  ///   returns GetBoundingBox minus GetClientAreaBorder
  GUI_IMPEXP virtual VRectanglef GetClientRect() const;

  /// \brief
  ///   Returns the window that is under the mouse position. Default implementation return "this" if
  ///   mouse is inside the bounding box
  GUI_IMPEXP virtual VWindowBase* TestMouseOver(VGUIUserInfo_t &user, const hkvVec2 &vAbsMouse);

  /// \brief
  ///   Returns a cursor that is used when the mouse is over this window. Can be NULL to use the
  ///   context's default cursor.
  GUI_IMPEXP virtual VCursor *GetMouseOverCursor(VGUIUserInfo_t &user) {return NULL;}

  /// \brief
  ///   Returns a tooltip instance. By default returns a standard tooltip with m_sTooltipText text
  ///   message
  GUI_IMPEXP virtual VTooltip* GetTooltip(VGUIUserInfo_t &user);



  ///
  /// @}
  ///

  ///
  /// @name Activation/events
  /// @{
  ///


  /// \brief
  ///   Called when the window gets activated
  GUI_IMPEXP virtual void OnActivate() {TriggerScriptEvent("OnActivate");}

  /// \brief
  ///   Called when the window gets deactivated
  GUI_IMPEXP virtual void OnDeactivate() {TriggerScriptEvent("OnDeactivate");}

  /// \brief
  ///   Called when the parent window changes properties, e.g. PARENTCHANGED_POSITION,... 
  GUI_IMPEXP virtual void OnParentChanged(int iFlags) {}

  /// \brief
  ///   Called when mouse cursor enters the window
  GUI_IMPEXP virtual void OnMouseEnter(VGUIUserInfo_t &user); 

  /// \brief
  ///   Called when mouse cursor leaves the window
  GUI_IMPEXP virtual void OnMouseLeave(VGUIUserInfo_t &user); 

  /// \brief
  ///   Called when the position of the window changed (called in SetPosition)
  GUI_IMPEXP virtual void OnPositionChanged() {TriggerScriptEvent("OnPositionChanged");InvalidateCache();}

  /// \brief
  ///   Called when the size of the window changed (called in SetSize)
  GUI_IMPEXP virtual void OnSizeChanged() {TriggerScriptEvent("OnSizeChanged");InvalidateCache();}

  /// \brief
  ///   Called by SendValueChangedEvent when this control changed its value.
  GUI_IMPEXP virtual void OnValueChanged(VItemValueChangedEvent *pEvent) {}

  /// \brief
  ///   If this control has focus, it receives the key code.
  GUI_IMPEXP virtual void OnKeyPressed(int iKey, int iKeyModifier) 
  {
    TriggerScriptEvent("OnKeyPressed", "*im", iKey, iKeyModifier);
  }

  /// \brief
  ///   Sends a value changed event to the context and to this control
  GUI_IMPEXP void SendValueChangedEvent(VItemValueChangedEvent *pEvent);

  /// \brief
  ///   Called when this control gets or loses focus
  GUI_IMPEXP virtual void OnSetFocus(bool bStatus) 
  {
    SetStatus(ITEMSTATUS_HASFOCUS, bStatus); 
    TriggerScriptEvent("OnSetFocus", "*b", bStatus);
  }
  
  /// \brief
  ///   Called when any status flag changed (ITEMSTATUS_ENABLED,...). Triggered by SetStatus
  GUI_IMPEXP virtual void OnStatusFlagsChanged(int iOldFlags, int iNewFlags) {}


  ///
  /// @}
  ///

  ///
  /// @name Click and Pointer Events
  /// @{
  ///


  /// \brief
  ///   Called when clicked on this control
  GUI_IMPEXP virtual void OnClick(VMenuEventDataObject *pEvent);

  /// \brief
  ///   Called when double clicked on this control
  GUI_IMPEXP virtual void OnDoubleClick(VMenuEventDataObject *pEvent);

  /// \brief
  ///   Called when a pointer goes down on this control (e.g. mouse button down, touch begin)
  GUI_IMPEXP virtual void OnPointerDown(VMenuEventDataObject* pEvent);

  /// \brief
  ///   Called when a pointer goes up on this control (e.g. mouse button up, touch end)
  GUI_IMPEXP virtual void OnPointerUp(VMenuEventDataObject* pEvent);

  ///
  /// @}
  ///

  ///
  /// @name Dragging Events
  /// @{
  ///


  /// \brief
  ///   Called when mouse starts dragging this control
  GUI_IMPEXP virtual void OnDragBegin(const hkvVec2 &vMousePos, int iButtonMask) {TriggerScriptEvent("OnDragBegin", "*mff", iButtonMask, vMousePos.x, vMousePos.y);}

  /// \brief
  ///   Called while mouse drags this control
  GUI_IMPEXP virtual void OnDragging(const hkvVec2 &vMouseDelta) {TriggerScriptEvent("OnDragging", "*ff", vMouseDelta.x, vMouseDelta.y);}

  /// \brief
  ///   Called when mouse stops dragging this control
  GUI_IMPEXP virtual void OnDragEnd(VWindowBase *pOver) {TriggerScriptEvent("OnDragEnd", "*o", pOver);}


  ///
  /// @}
  ///

  ///
  /// @name Text
  /// @{
  ///


  /// \brief
  ///   Sets text on this window (very control specific)
  GUI_IMPEXP virtual void SetText(const char *szString) {InvalidateCache();}

  /// \brief
  ///   Gets text of this window (very control specific)
  GUI_IMPEXP virtual const char *GetText() const {return NULL;}

  /// \brief
  ///   Sets the default tooltip text
  inline void SetTooltipText(const char *szText) {m_sTooltipText=szText;}

  ///
  /// @}
  ///

  ///
  /// @name Order
  /// @{
  ///


  /// \brief
  ///   Set the tab order value, i.e. the sorting key when cycling through the items of a dialog
  inline void SetTabOrder(int iValue) {m_iTabOrder=iValue;}

  /// \brief
  ///   Get the tab order value
  inline int GetTabOrder() const {return m_iTabOrder;}

  ///
  /// @}
  ///

  ///
  /// @name Caching
  /// @{
  ///

  /// \brief Enable or disables caching.
  GUI_IMPEXP void SetUseCaching(bool bState);

  /// \brief Invalidates the current cache.
  GUI_IMPEXP void InvalidateCache();


  ///
  /// @}
  ///

  ///
  /// @name Misc
  /// @{
  ///


  /// \brief
  ///   Static helper function for parsing
  GUI_IMPEXP static ControlState_e GetState(const char *szState);


protected:
// serialization
  V_DECLARE_SERIAL_DLLEXP( VWindowBase, GUI_IMPEXP_DATA );
  GUI_IMPEXP virtual void Serialize( VArchive &ar );

// renderable
  GUI_IMPEXP virtual void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState);
  GUI_IMPEXP virtual bool Build(TiXmlElement *pNode, const char *szPath, bool bWrite);
  VGUICommandBufferPtr m_spCachedBuffer;

// member vars
  mutable IVGUIContext *m_pContext;
  VWindowBase *m_pOwner;

  int m_iID, m_iDialogResult;
  hkvVec2 m_vPosition, m_vSize;
  VRectanglef m_ClientAreaBorder;

  int m_iStatus, m_iOrder, m_iTabOrder;

  VString m_sTooltipText;
///
/// @}
///

};

/// \brief
///   This structure holds user specific information. A GUI context holds one of these structures per (active) user
struct VGUIUserInfo_t
{
  /// \brief
  ///   Enum that specifies a user index (0..3)
  enum VGUIUserID_e
  {
    GUIUser0   = 0,  ///< first user (user ID 0)
    GUIUser1   = 1,  ///< second user (user ID 1)
    GUIUser2   = 2,  ///< third user (user ID 2)
    GUIUser3   = 3,  ///< forth user (user ID 3)
    GUIMaxUser = 4   ///< maximum number of users
  };

  VGUIUserInfo_t()
  {
    m_iID = GUIUser0;
    m_iButtonMask = m_iLastClickedMask = 0;
    m_fLastClickTime = 0.f;
    m_pCurrentCursor = NULL;

#if defined(SUPPORTS_MULTITOUCH) || defined(_VISION_WIIU)
    m_iNextButtonMask = 0;
#endif
  }

  VGUIUserID_e m_iID;       ///< ID of this user
  hkvVec2 m_vMousePos;    ///< Current mouse cursor position of this user
  hkvVec2 m_vMousePosAccum; ///< Internal state for mouse coordinates that are not aligned to pixels

  int m_iButtonMask, m_iLastClickedMask;  ///< Internal state
  float m_fLastClickTime;   ///< Internal state
  VWindowBasePtr m_spMouseOverItem; ///< Internal state
  VWindowBasePtr m_spDragItem;      ///< Internal state
  VWindowBasePtr m_spMouseDownItem; ///< Internal state
  // Smart pointer does not work on the Wii here (the class is not fully defined)
  VCursor *m_pCurrentCursor; ///< Internal state

#if defined(SUPPORTS_MULTITOUCH) || defined(_VISION_WIIU)
  int m_iNextButtonMask; ///< Next button mask (one frame ahead of m_iButtonMask)
#endif
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
