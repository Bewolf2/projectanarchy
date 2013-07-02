
#ifndef VLUA_APIDOC

%nodefaultctor VWindowBase;
%nodefaultdtor VWindowBase;

class VWindowBase : public VisTypedEngineObject_cl //hide inheritance: public IVRenderableItem, public VRefCounter, public VUserDataObj
{
public:

  inline VWindowBase *GetParent() const;
  //void SetParent(VWindowBase *pOwner);
  
  inline int GetID() const;

  virtual int GetDialogResult() const;
  virtual void SetDialogResult(int iResult);

  virtual void Refresh();

  bool IsVisible() const;
  void SetVisible(bool bStatus);
  
  bool IsEnabled() const;
  void SetEnabled(bool bStatus);
  
  bool IsStatic() const;
  void SetSelected(bool bStatus);

  bool IsMouseOver() const;
  bool HasFocus() const;
  bool IsSelected() const;
  
  //todo: Use macros instead!
  %extend{
  
    VBitmask GetStatusBitmask() const
    {
      return VBitmask((unsigned int)self->m_iStatus);
    }
    
    void SetStatusBitmask(VBitmask *pBitmask)
    {
      if(pBitmask)
        self->m_iStatus = pBitmask->Get();
    }
  } 

  void SetPosition(float x, float y);
  void SetSize(float dx, float dy);

  void SetOrder(int iOrder);
  inline int GetOrder() const;
  
  %extend{
  
    void SetTooltip(const char *szText)
    {
      self->SetTooltipText(szText);
    }
    
    void SetText(const char *szText)
    {
      self->SetText(szText);
    }
  }

  
  //will follow later
  //virtual const VGUICharType *GetText() const ;
  //virtual VTooltip* GetTooltip(VGUIUserInfo_t &user);

  inline void SetTabOrder(int iValue);
  inline int GetTabOrder() const;

};

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(VWindowBase, 128, "[%d:'%s']", self->GetID(), VGUIManager::GlobalManager().GetIDName(self->GetID()))
VSWIG_CREATE_TOSTRING(VWindowBase, "VWindowBase: Id: %d '%s'", self->GetID(), VGUIManager::GlobalManager().GetIDName(self->GetID()))

//implement GetPosition native becuase we would like to return two values at once
%native(GetPosition) int VWindowBase_GetPosition(lua_State *L);
%{
  SWIGINTERN int VWindowBase_GetPosition(lua_State *L)
  {
    IS_MEMBER_OF(VWindowBase) //this will move this function to the method table of the specified class
  
    SWIG_CONVERT_POINTER(L, 1, VWindowBase, pWindow);
    
    lua_settop(L,0);
    
    hkvVec2 pos = pWindow->GetPosition();
    
    lua_pushnumber(L, (lua_Number)pos.x);
    lua_pushnumber(L, (lua_Number)pos.y);

    return 2; //in every case we leave two values at the stack
  }
%}

//implement GetAbsPosition native becuase we would like to return two values at once
%native(GetAbsPosition) int VWindowBase_GetAbsPosition(lua_State *L);
%{
  SWIGINTERN int VWindowBase_GetAbsPosition(lua_State *L)
  {
    IS_MEMBER_OF(VWindowBase) //this will move this function to the method table of the specified class
  
    SWIG_CONVERT_POINTER(L, 1, VWindowBase, pWindow);
    
    lua_settop(L,0);
    
    hkvVec2 pos = pWindow->GetAbsPosition();
    
    lua_pushnumber(L, (lua_Number)pos.x);
    lua_pushnumber(L, (lua_Number)pos.y);

    return 2; //in every case we leave two values at the stack
  }
%}

//implement GetAbsPosition native becuase we would like to return two values at once
%native(GetSize) int VWindowBase_GetSize(lua_State *L);
%{
  SWIGINTERN int VWindowBase_GetSize(lua_State *L)
  {
    IS_MEMBER_OF(VWindowBase) //this will move this function to the method table of the specified class
  
    SWIG_CONVERT_POINTER(L, 1, VWindowBase, pWindow);
    
    lua_settop(L,0);
    
    hkvVec2 pos = pWindow->GetSize();
    
    lua_pushnumber(L, (lua_Number)pos.x);
    lua_pushnumber(L, (lua_Number)pos.y);

    return 2; //in every case we leave two values at the stack
  }
%}


#else

/// \brief Base class of a window instance.
class VWindowBase : public VisTypedEngineObject_cl {
public:

  /// @name Appearance
  /// @{

  /// \brief Sets the tooltip text of this control. It shows when hovering over the control with the mouse.
  /// \param text The tooltip text to set.
  /// \par Example
  ///   \code
  ///     function OnActivate(self)
  ///       self.activation = os.clock()
  ///     end
  ///  
  ///     function OnMouseEnter(self)
  ///       self:SetTooltip(string.format("Elapsed time since activation: %.2f", os.clock() - self.activation))
  ///     end
  ///   \endcode
  void SetTooltip(string text);
  
  /// \brief Sets the text of this control. Where the text is shown depends on the type of the control.
  /// \param text The text to set in the control.
  /// \par Example
  ///   \code
  ///     function OnActivate(self)
  ///       self:SetTooltip(os.date("Today is %A, in %B")) -- %A weekday; %B month;
  ///     end
  ///   \endcode
  void SetText(string text);

  /// \brief Returns whether the control is visible.
  /// \return true = visible; false = invisible.
  boolean IsVisible();

  /// \brief Makes the control visible or invisible.
  /// \param visible true = visible; false = invisible.
  void SetVisible(boolean visible);
  
  /// \brief Sets a new position for this control, always relative to the control's parent.
  /// \param x The x coordinate of the new position.
  /// \param y The y coordinate of the new position.
  void SetPosition(number x, number y);
  
  /// \brief Returns the current position of the upper left corner of the control, relative to its parent.
  /// \return Two numbers: the x and y coordinate of the current position, respectively.
  multiple GetPosition();
  
  /// \brief Returns the current absolute screen position of the upper left corner of the control.
  /// \return Two numbers: the x and y coordinate of the current position, respectively.
  multiple GetAbsPosition();
  
  /// \brief Sets a new size for this control.
  /// \param x The new size in x direction.
  /// \param y The new size in y direction.
  void SetSize(number x, number y);
  
  /// \brief Returns the current dimensions of the control.
  /// \return Two numbers: the size in x and in y direction, respectively.
  /// \par Example
  ///   \code
  ///     local x,y = self:GetSize()
  ///   \endcode
  multiple GetSize();
  
  /// @}
  /// @name Common Functions
  /// @{
  
  /// \brief Gets the ID of this control as defined in the XML. 
  /// \return The ID of this control. 
  number GetID();
  
  /// \brief Gets the parent control of this control.
  /// \return The parent of this control.
  /// \par Example
  ///   \code
  ///     function OnActivate(self)
  ///       local parent self:GetParent()
  ///       if parent ~= nil then
  ///         self:SetText("My parent is "..GUI:GetIDName(parent:GetID()))
  ///       else
  ///         self:SetText("I have no parent...")
  ///       end
  ///     end
  ///   \endcode
  VWindowBase GetParent();
  
  /// \brief Gets the dialog result of the control. For dialog controls this is the ID that they return if the control is clicked. 
  /// \return The dialog result of the last action.
  number GetDialogResult();
  
  /// \brief Sets the new dialog result. Can be used to close and open dialogs.
  /// \param result 
  void SetDialogResult(number result);
  
  /// \brief The dialog and all its children will refresh their data. Has to be implemented by the user. 
  void Refresh();
  
  /// \brief Returns whether the control reacts on input.
  /// \return true = yes; false = no.
  /// \par Example
  ///   \code
  ///     function OnActivate(self)
  ///       if not self:IsEnabled() then
  ///         Debug:Log(GUI:GetIDName(self:GetID()).." is disabled!")
  ///       end
  ///     end
  ///   \endcode
  boolean IsEnabled();
  
  /// \brief Sets whether the control acts on input or not.
  /// \param enable true = yes; false = no.
  void SetEnabled(boolean enable);
  
  /// \brief Indicates whether control is static (i.e. has ID VGUIManager::ID_STATIC).
  /// \return true if the cotrol has a predefined (static) id, otherwise false.
  boolean IsStatic();
  
  /// \brief Returns whether the control is currently selected.
  /// \return true = yes; false = no.
  boolean IsSelected();
    
  /// \brief Selects the control. The control has to support this call (e.g. a checkbox)
  /// \param selected true = yes; false = no.
  void SetSelected(boolean selected);

  /// \brief Returns whether the mouse pointer is over the control.
  /// \return true = yes; false = no.
  boolean IsMouseOver();
  
  /// \brief Returns whether the control currently has input focus.
  /// \return true = yes; false = no.
  boolean HasFocus();

  /// @}
  /// @name Control State / Order / Tab order
  /// @{
  
  /// \brief Get the status of the control.
  /// \note The status bitmask (m_iStatus) has the following bit setup:
  ///   - 0x00000001 ITEMSTATUS_VISIBLE          
  ///   - 0x00000002 ITEMSTATUS_ENABLED          
  ///   - 0x00000008 ITEMSTATUS_HASFOCUS         
  ///   - 0x00000010 ITEMSTATUS_SELECTED         
  ///   - 0x00000020 ITEMSTATUS_STAYSELECTED     
  ///   - 0x00000100 ITEMSTATUS_MOUSEOVER_USER0  
  ///   - 0x00000200 ITEMSTATUS_MOUSEOVER_USER1  
  ///   - 0x00000400 ITEMSTATUS_MOUSEOVER_USER2  
  ///   - 0x00000800 ITEMSTATUS_MOUSEOVER_USER3 
  ///   - 0x00010000 ITEMSTATUS_VALIDPOSITION    
  ///   - 0x00020000 ITEMSTATUS_VALIDSIZE        
  /// \return A bitmask containing the status.
  /// \see SetStatusBitmask
  VBitmask GetStatusBitmask();
    
  /// \brief Set the status of the control.
  /// \note The status bitmask (m_iStatus) has the following bit setup:
  ///   - 0x00000001 ITEMSTATUS_VISIBLE          
  ///   - 0x00000002 ITEMSTATUS_ENABLED          
  ///   - 0x00000008 ITEMSTATUS_HASFOCUS         
  ///   - 0x00000010 ITEMSTATUS_SELECTED         
  ///   - 0x00000020 ITEMSTATUS_STAYSELECTED     
  ///   - 0x00000100 ITEMSTATUS_MOUSEOVER_USER0  
  ///   - 0x00000200 ITEMSTATUS_MOUSEOVER_USER1  
  ///   - 0x00000400 ITEMSTATUS_MOUSEOVER_USER2  
  ///   - 0x00000800 ITEMSTATUS_MOUSEOVER_USER3 
  ///   - 0x00010000 ITEMSTATUS_VALIDPOSITION    
  ///   - 0x00020000 ITEMSTATUS_VALIDSIZE        
  /// \param bitmask The new status bitmask.
  /// \see GetStatusBitmask
  void SetStatusBitmask(VBitmask bitmask);

  /// \brief Gets the order that has been set via SetOrder (resp. with the XML attribute). 
  /// \return The numeric order value.
  number GetOrder();
  
  /// \brief Sets a new order key value for this window.
  /// Items are sorted back to front at dialog activation time.
  /// Open dialogs are sorted when a new dialog is opened.
  /// \param order The new order value.
  void SetOrder(number order);

  /// \brief Get the tab order value.  
  /// \return The tab order of this element.
  number GetTabOrder();  
  
  /// \brief Set the tab order value, i.e. the sorting key when cycling through the items of a dialog. 
  /// \param tabValue The new tab order value (needs to be unique)
  void SetTabOrder(number tabValue);
  
  /// @}
  /// @name Callbacks
  /// @{
  
  /// \brief Overridable callback function. Called when the control is activated by the game code.
  /// \param self The object to which this script is attached.
  /// \par Example
  ///   \code
  ///     function OnActivate(self)
  ///       Debug:Log("Item '"..GUI:GetIDName(self:GetID()).."'activated!")
  ///     end
  ///   \endcode
  void OnActivate(VWindowBase self);

  /// \brief Overridable callback function. Called when the control is deactivated by the game code.
  /// \param self The object to which this script is attached.
  void OnDeactivate(VWindowBase self);

  /// \brief Overridable callback function. Called when the mouse comes over the control.
  /// \param self The object to which this script is attached.
  void OnMouseEnter(VWindowBase self);

  /// \brief Overridable callback function. Called when the mouse leaves the control.
  /// \param self The object to which this script is attached.
  void OnMouseLeave(VWindowBase self);

  /// \brief Overridable callback function. Called when the position of the control changed.
  /// \param self The object to which this script is attached.
  void OnPositionChanged(VWindowBase self);

  /// \brief Overridable callback function. Called when the size of the control changed.
  /// \param self The object to which this script is attached.
  /// \par Example
  ///   \code
  ///     function OnSizeChanged(self)
  ///       local x,y = self:GetSize()
  ///       local name = GUI:GetIDName(self:GetID())
  ///       Debug:Log("New dimension of '"..name.."' is "..x..", "..y)
  ///     end
  ///   \endcode
  void OnSizeChanged(VWindowBase self);

  /// \brief Overridable callback function. Called when a mouse-click occurs on the object.
  /// \param self The object to which this script is attached.
  /// \param button A number that tells which buttons were clicked: Vision.BUTTON_LEFT, Vision.BUTTON_MIDDLE, Vision.BUTTON_RIGHT.
  /// \param posX The screen x position of the click.
  /// \param posY The screen y position of the click.
  /// \par Example
  ///   \code
  ///     function OnClick(self, button, number x, number y)
  ///       if button == Vision.BUTTON_RIGHT then
  ///         self:SetVisible(false)
  ///       end
  ///     end
  ///   \endcode    
  void OnClick(VWindowBase self, number button, number posX, number posY);

  /// \brief Overridable callback function. Called when a double-click occurs on the object.
  /// \param self The object to which this script is attached.
  /// \param button A number that tells which buttons were clicked: Vision.BUTTON_LEFT, Vision.BUTTON_MIDDLE, Vision.BUTTON_RIGHT.
  /// \param posX The screen x position of the click.
  /// \param posY The screen y position of the click.
  void OnDoubleClick(VWindowBase self, number button, number posX, number posY);
  
  /// \brief Overridable callback function. Called when a double-click occurs on one of the controls in the dialog.
  /// \param self The object to which this script is attached.
  /// \param key The key which has been pressed (Lookup KEY_* in the Vision module).
  /// \param controlKeys A bitmask that tells which control keys were held. 0 = left shift, 1 = right shift, 2 = left control, 3 = right control, 4 = left alt, 5 = right alt.
  /// \see VScriptInput_wrapper
  void OnKeyPressed(VDialog self, number key, VBitmask controlKeys);
  
  /// @}
};

#endif