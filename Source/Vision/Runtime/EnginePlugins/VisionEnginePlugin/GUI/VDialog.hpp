/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VDialog.hpp

#ifndef VDIALOG_HPP_INCLUDED
#define VDIALOG_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/IVRenderableItem.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VDialogFrame.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VDlgControlBase.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VWindowBase.hpp>

class VMenuEventDataObject;
class VCursor;
class VGUIManager;
class VDialogResource;
typedef VSmartPtr<VDialogResource> VDialogResourcePtr;
class VImageStates;

#define DIALOG_ORDER_FRONT    -10000000
#define DIALOG_ORDER_BACK      10000000

#define DIALOGFLAGS_NONE          0x00000000
#define DIALOGFLAGS_FULLSCREEN    0x00001000
#define DIALOGFLAGS_MODAL         0x00010000

#define DIALOG_BINARYFILEEXT      "vDlg"

/// \brief
///   Class that represents a dialog instance
class VDialog : public VWindowBase
{
public:
  enum StartPositionType_e
  {
    AbsolutePos = 0,
    ScreenCenter = 1,
    ParentPos = 2,
    ParentCenter = 3
  };

  /// \brief
  ///   Empty constructor
  GUI_IMPEXP VDialog();
  GUI_IMPEXP virtual ~VDialog();

  /// \brief
  ///   Initialises the dialog instance with the controls of a template
  GUI_IMPEXP void InitDialog(IVGUIContext *pContext, VDialogResource *pRes, VDialog *pParent, int iFlags=DIALOGFLAGS_NONE);

  /// \brief
  ///   Return the dialog template that has been used to create this dialog
  inline VDialogResource* GetDialogResource() const {return m_spResource;}

  /// \brief
  ///   Brings this dialog to front
  GUI_IMPEXP void BringToFront();

  /// \brief
  ///   Brings this dialog to back
  GUI_IMPEXP void BringToBack();

  /// \brief
  ///   Indicates whether this dialog runs in modal mode
  inline bool IsModal() const {return (m_iDialogFlags&DIALOGFLAGS_MODAL)>0;}

  /// \brief
  ///   Indicates whether this dialog covers the whole view (e.g. main menu)
  inline bool IsFullscreen() const {return (m_iDialogFlags&DIALOGFLAGS_FULLSCREEN)>0;}
  
  /// \brief
  ///   Returns the screen coordinates of the default dialog start position (e.g. screen center)
  GUI_IMPEXP hkvVec2 GetStartPosition() const;

  /// \brief
  ///   Returns the instance of the dialog frame that is used to render the frame border
  inline const VDialogFrame& GetFrame() const {return m_Frame;}


  ///
  /// @name Overridden IVRenderableItem Functions
  /// @{
  ///



  /// \brief
  ///   Overridden OnPaint function to render this dialog instance
  GUI_IMPEXP virtual void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState);

  /// \brief
  ///   Overridden OnTick function update the dialog on a per-frame basis
  GUI_IMPEXP virtual void OnTick(float dtime);

  /// \brief
  ///   Overridden Build function build this dialog instance from an XML node
  GUI_IMPEXP virtual bool Build(TiXmlElement *pNode, const char *szPath, bool bWrite);


  ///
  /// @}
  ///

  ///
  /// @name Overridden VWindowBase Functions
  /// @{
  ///

  GUI_IMPEXP virtual VRectanglef GetClientRect() const HKV_OVERRIDE;
  GUI_IMPEXP virtual void OnPositionChanged() HKV_OVERRIDE;
  GUI_IMPEXP virtual void OnSizeChanged() HKV_OVERRIDE;
  GUI_IMPEXP virtual void OnActivate() HKV_OVERRIDE;
  GUI_IMPEXP virtual void OnDeactivate() HKV_OVERRIDE;
  GUI_IMPEXP virtual void Refresh() HKV_OVERRIDE;
  GUI_IMPEXP virtual void OnDragging(const hkvVec2 &vMouseDelta) HKV_OVERRIDE;
  GUI_IMPEXP virtual void OnValueChanged(VItemValueChangedEvent *pEvent) HKV_OVERRIDE;
  GUI_IMPEXP virtual void OnSetFocus(bool bStatus) HKV_OVERRIDE;
  GUI_IMPEXP virtual void OnKeyPressed(int iKey, int iKeyModifier) HKV_OVERRIDE {VWindowBase::OnKeyPressed(iKey,iKeyModifier); if (m_spFocusItem) m_spFocusItem->OnKeyPressed(iKey,iKeyModifier);}
  GUI_IMPEXP virtual VWindowBase* TestMouseOver(VGUIUserInfo_t &user, const hkvVec2 &vAbsMouse) HKV_OVERRIDE;


  ///
  /// @}
  ///

  ///
  /// @name New Virtual Functions
  /// @{
  ///


  /// \brief
  ///   Returns the current active cursor
  GUI_IMPEXP virtual VCursor* GetCurrentCursor(VGUIUserInfo_t &user);

  /// \brief
  ///   Called after the dialog has been initialized
  GUI_IMPEXP virtual void OnInitDialog() {TriggerScriptEvent("OnInitDialog");}

  /// \brief
  ///   Fills the event structure. Called by the context when an event is triggered.
  GUI_IMPEXP virtual void FillEvent(VMenuEventDataObject *pEvent);

  /// \brief
  ///   Called when the context detected that the user clicked on a GUI control. Base
  ///   implementation sets the dialog result and forwards this event.
  GUI_IMPEXP virtual void OnItemClicked(VMenuEventDataObject *pEvent);

  /// \brief
  ///   Called when the context detected that the user double-clicked on a GUI control. Base
  ///   implementation forwards this event.
  GUI_IMPEXP virtual void OnItemDoubleClicked(VMenuEventDataObject *pEvent);


  ///
  /// @}
  ///

  ///
  /// @name Control Collection
  /// @{
  ///


  /// \brief
  ///   Adds a control to this dialog. For manually building a dialog
  GUI_IMPEXP virtual void AddControl(VDlgControlBase *pItem);

  /// \brief
  ///   Recalculates the dimension of the dialog
  GUI_IMPEXP virtual void ResumeLayout();

  /// \brief
  ///   Returns the collection of controls in this dialog
  inline VMenuItemCollection& Items() {return m_Items;}

  /// \brief
  ///   Set the control that has input focus
  GUI_IMPEXP void SetFocusItem(VWindowBase *pNewItem);

  /// \brief
  ///   Returns the item (or NULL) that has currently focus
  inline VWindowBase *GetFocusItem() const {return m_spFocusItem;}
    
  ///
  /// @}
  ///

  ///
  /// @name Misc
  /// @{
  ///

  /// \brief
  ///   Sets the modal app for running in modal loop
  inline void SetModalApp(IVisApp_cl *pApp) {m_spModalApp=pApp;}

  /// \brief
  ///   Get the modal application instance
  inline IVisApp_cl *GetModalApp() const {return m_spModalApp;}

  /// \brief
  ///   Static helper function for parsing
  GUI_IMPEXP static StartPositionType_e GetStartPositionType(const char *szTypeStr);


  /// \brief
  ///   Saves this dialog instance to a binary file using serialization. The file can be loaded with LoadFromBinaryFile again. If no file extension is specified, the default extension .vDlg is used
  GUI_IMPEXP BOOL SaveToBinaryFile(const char *szFilename);

  /// \brief
  ///   Static load function that loads a dialog from binary file and returns an instance of the dialog class. The dialog can be saved to file using SaveToBinaryFile
  GUI_IMPEXP static VDialog* LoadFromBinaryFile(const char *szFilename);

  /// \brief
  ///   Static helper function for parsing
  inline void SetContextRecursive(IVGUIContext *pContext)
  {
    m_pContext = pContext;
    for (int i=0;i<m_Items.Count();i++)
      m_Items.GetAt(i)->SetParent(this);
  }
protected:
// serialization
  V_DECLARE_SERIAL_DLLEXP( VDialog, GUI_IMPEXP_DATA );
  GUI_IMPEXP virtual void Serialize( VArchive &ar );

protected:
  friend class VDialogCollection;
  friend class VGUIManager;

  // base properties
  VDialogResourcePtr m_spResource;

  int m_iDialogFlags;
  StartPositionType_e m_eStartPos;

  VMenuItemCollection m_Items;
  VDialogFrame m_Frame;
  VImageStates *m_pImage;
  float m_fFadeInPos, m_fFadeInSpeed;
  hkvVec2 m_vClientMin, m_vClientMax;

  // const background color
  VColorRef m_iBackColor;

  bool m_bFadeBack;
  VColorRef m_iBackFadeColor;
  float m_fBackFadePos, m_fBackFadeSpeed;

  // temporary properties
  VWindowBasePtr m_spMouseOverItem[VGUIUserInfo_t::GUIMaxUser];
  VWindowBasePtr m_spFocusItem;
  IVisAppPtr m_spModalApp;
  ///
  /// @}
  ///

};



/// \brief
///   Class for a collection of dialog instances (reference counted)
class VDialogCollection : public VRefCountedCollection<VDialog>
{
public:

  /// \brief
  ///   Sort the dialogs by sorting order back to front for rendering
  GUI_IMPEXP void SortByOrder();

  /// \brief
  ///   Take the current order and assign the index to each dialog's m_iOrder value
  GUI_IMPEXP void AssignNewOrder();

  /// \brief
  ///   Get the maximum order value in this collection
  GUI_IMPEXP int GetMaxOrder();

  /// \brief
  ///   Get the maximum order value in this collection
  GUI_IMPEXP int GetMinOrder();

  /// \brief
  ///   Render all dialogs in this collection
  GUI_IMPEXP void RenderAll(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState);


  /// \brief
  ///   Call the tick function on all dialogs in this collection
  GUI_IMPEXP void OnTickFunction(float fTimeDelta);

  ///
  /// @name Collection Functions
  /// @{
  ///

  /// \brief
  ///   Clears the collection
  GUI_IMPEXP void Clear();

  /// \brief
  ///   Add a dialog to the collection (calls the OnActivate function) 
  GUI_IMPEXP void Add(VDialog *pDialog);

  /// \brief
  ///   Removes the dialog from the collection (calls the OnDeactivate function)
  GUI_IMPEXP void Remove(VDialog *pDialog);

  /// \brief
  ///   Find a dialog with specified ID. The Dialog ID is specified in the dialog resource
  GUI_IMPEXP VDialog* FindDialog(int iID) const;

  ///
  /// @}
  ///

private:
  static int CompareDialogOrder( const void *arg1, const void *arg2);
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
