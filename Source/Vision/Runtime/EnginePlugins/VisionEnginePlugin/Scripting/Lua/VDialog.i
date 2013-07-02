
#ifndef VLUA_APIDOC

%nodefaultctor VWindowBase;
%nodefaultdtor VWindowBase;

class VDialog : public VWindowBase
{
public:
  
  void BringToFront();
  void BringToBack();

  bool IsModal() const;
  bool IsFullscreen() const;

  //would be useful
  //virtual void AddControl(VDlgControlBase *pItem);
  //inline VMenuItemCollection& Items();
  //VCursor* GetCurrentCursor(VGUIUserInfo_t &user);
  
  void SetFocusItem(VWindowBase *pNewItem);
  inline VWindowBase *GetFocusItem() const;


  //BOOL SaveToBinaryFile(const char *szFilename);
  //static VDialog* LoadFromBinaryFile(const char *szFilename);

};

//add lua tostring and concat operators
VSWIG_CREATE_TOSTRING(VDialog, "VDialog: Id: %d '%s'", self->GetID(),VGUIManager::GlobalManager().GetIDName(self->GetID()))

//implement GetPosition native becuase we would like to return two values at once
%native(VDialog_GetStartPosition) int VDialog_GetStartPosition(lua_State *L);
%{
  SWIGINTERN int VDialog_GetStartPosition(lua_State *L)
  {
    IS_MEMBER_OF(VDialog) //this will move this function to the method table of the specified class
  
    SWIG_CONVERT_POINTER(L, 1, VDialog, pDialog);
    
    lua_settop(L,0);
    
    hkvVec2 pos = pDialog->GetStartPosition();
    
    lua_pushnumber(L, (lua_Number)pos.x);
    lua_pushnumber(L, (lua_Number)pos.y);

    return 2;
  }
%}

#else

/// \brief VDialog wrapper class.
class VDialog : public VWindowBase {
public:

  /// @name Common Dialog Functions
  /// @{

  /// \brief Brings this dialog to front.     
  void BringToFront();
  
  /// \brief Brings this dialog to back. 
  void BringToBack();

  /// \brief Indicates whether this dialog runs in modal mode. 
  /// \return true if this dialog is modal, otherwise false.
  boolean IsModal();
  
  /// \brief Indicates whether this dialog covers the whole view (e.g. main menu). 
  /// \return true if it is a full screen dialog, otherwise false.
  boolean IsFullscreen();
  
  /// \brief Focus control on an item.
  /// \param item The item which should receive the focus.
  void SetFocusItem(VWindowBase item);
  
  /// \brief Returns the item that has currently the focus. 
  /// \return The item which currently has the input focus, or nil if it has never been set.
  VWindowBase GetFocusItem();
  
  /// \brief Returns the screen coordinates of the default dialog start position (e.g. screen center). 
  /// \return Two numbers containing the x and y start position.
  multiple GetStartPosition();

  /// @}
  /// @name Callbacks
  /// @{
  
  /// \brief Overridable callback function. Called when a mouse-click occurs on one of the controls in the dialog.
  /// \param self The object to which this script is attached.
  /// \param item The control on which the mouse-click occurred.
  /// \param button A number that tells which buttons were clicked: Vision.BUTTON_LEFT, Vision.BUTTON_MIDDLE, Vision.BUTTON_RIGHT.
  /// \param posX The screen x position of the click.
  /// \param posY The screen y position of the click.
  /// \see VScriptInput_wrapper
  void OnItemClicked(VDialog self, VWindowBase item, number button, number posX, number posY);

  /// \brief Overridable callback function. Called when a double-click occurs on one of the controls in the dialog.
  /// \param self The object to which this script is attached.
  /// \param item The control on which the double-click occurred.
  /// \param button A number that tells which buttons were clicked: Vision.BUTTON_LEFT, Vision.BUTTON_MIDDLE, Vision.BUTTON_RIGHT.
  /// \param posX The screen x position of the click.
  /// \param posY The screen y position of the click.
  /// \see VScriptInput_wrapper
  void OnItemDoubleClicked(VDialog self, VWindowBase item, number button, number posX, number posY);
  
  /// @}
  
};

#endif