
#ifndef VLUA_APIDOC

%nodefaultctor VGUIManager;
%nodefaultdtor VGUIManager;

class VGUIManager
{
public:
  int GetID(const char *szIDName);
  const char* GetIDName(int iID);
  
  bool LoadResourceFile(const char *szResourceFile);
  
  %extend{
    
    VDialog *ShowDialog(const char * szXmlFile)
    {
      if(szXmlFile==NULL)
        return NULL;
        
      VSWIG_ENSURE_GUI_CONTEXT(self, pContext);
      return pContext->ShowDialog(szXmlFile);
    }
    
    bool ShowDialog(VDialog *pDialog)
    {
      if(pDialog==NULL)
        return false;
      
      VASSERT_MSG(pDialog->GetContext()!=NULL, "No dialog context present!")
      
      pDialog->GetContext()->ShowDialog(pDialog);
      return true;
    }
    
    bool CloseDialog(VDialog *pDialog)
    {
      if(pDialog==NULL)
        return false;
      
      VASSERT_MSG(pDialog->GetContext()!=NULL, "No dialog context present!")
      
      pDialog->GetContext()->CloseDialog(pDialog);
      return true;
    }
    
    int ShowModalDialog(const char * szXmlFile, float x = 0, float y = 0)
    {
      if(szXmlFile==NULL)
        return false;
        
      VSWIG_ENSURE_GUI_CONTEXT(self, pContext);
      return pContext->ShowDialogModal(NULL, szXmlFile, hkvVec2(x,y));
    }
    
    int ShowModalDialog(VDialog *pDialog)
    {
      if(pDialog==NULL)
        return false;
      VASSERT_MSG(pDialog->GetContext()!=NULL, "No dialog context present!")
      return pDialog->GetContext()->ShowDialogModal(pDialog);
    }
    
    void SetCursorVisible(bool bVisible)
    {
      VSWIG_ENSURE_GUI_CONTEXT(self, pContext);
      pContext->SetShowCursor(bVisible);
    }
    
    bool IsCursorVisible()
    {
      VSWIG_ENSURE_GUI_CONTEXT(self, pContext);   
      return pContext->GetShowCursor();
    }
  }
};

#else

/// \brief The GUI Manager transforms GUI ID strings. Present in Lua as \b GUI.
/// \par Example
///   \code
///     -- called when an item in the dialog is clicked
///     function OnItemClicked(self, uiItem, buttons, mousePosX, mousePosY)
///       if buttons[1] and uiItem:GetID() == GUI:GetID("LOGIN") then
///         ProcessLogin(self)
///       end
///     end
///   \endcode
class VGUIManager {
public:

  /// @name Resource and ID Functions
  /// @{
 
  /// \brief Returns the ID of a certain window control's name as defined in its XML file.
  /// \param name The ID name of the window control.
  /// \return The ID of the window control. 
  /// \par Example
  ///   \code
  ///    function OnItemClicked(self, uiItem, buttons, mousePosX, mousePosY)
  ///      if uiItem:GetID() == GUI:GetID("HIDE_DIALOG") then
  ///        self:SetVisible(false)
  ///      end
  ///    end
  ///   \endcode
  number GetID(string name);
    
  /// \brief Retrieves the name of a certain window control by its ID. 
  /// \param id The ID of the window control.
  /// \return The ID name of the window control.
  string GetIDName(number id);
  
  /// \brief Loads an XML file that can contain multiple resources
  /// \param xmlResourceFile The XML file to load.
  /// \return true on success, otherwise false.
  boolean LoadResourceFile(string xmlResourceFile);
  
  /// @}
  /// @name Dialog Functions
  /// @{
  
  /// \brief Loads and shows a dialog from a XML file
  /// \note This function will use the first available GUI context of the GUIManager.
  /// If there is no GUI context it will internally create a new GUI context.
  /// \param xmlResourceFile The XML file containing the dialog definition.
  /// \return A dialog instance on success, otherwise nil.
  VDialog ShowDialog(string xmlResourceFile);

  /// \brief Shows the specified dialog.
  /// \note This function will use the dialog's associated GUI context.
  /// \param dialog The dialog to display.
  /// \return true on success, otherwise false.
  boolean ShowDialog(VDialog dialog);
  
  /// \brief Close the specified dialog.
  /// \note This function will use the dialog's associated GUI context.
  /// \param dialog The dialog to close.
  /// \return true on success, otherwise false.
  boolean CloseDialog(VDialog dialog);
  
  /// \brief Loads and shows a modal dialog from a XML file.
  /// \note This function will use the first available GUI context of the GUIManager.
  /// If there is no GUI context it will internally create a new GUI context.
  /// \param xmlResourceFile The XML file containing the dialog definition.
  /// \param x [\b optional} Additional x pixel offset of the upper left corner of the dialog.
  /// \param y [\b optional} Additional y pixel offset of the upper left corner of the dialog.
  /// \return Returns the dialog result as soon as the dialog is closed.
  number ShowModalDialog(string xmlResourceFile, number x = 0, number y = 0);

  /// \brief Shows a modal dialog.
  /// \note This function will use the dialog's associated GUI context.
  /// \param dialog The dialog to show.
  /// \return Returns the dialog result as soon as the dialog is closed.
  number ShowModalDialog(VDialog dialog);
  
  /// @}
  /// @name Misc. Functions
  /// @{
  
  /// \brief Shows or hides the mouse cursor (if loaded via a resource file!)
  /// \note This function will use the first available GUI context of the GUIManager.
  /// If there is no GUI context it will internally create a new GUI context.
  /// \param visible Use true to show the mouse cursor, false to hide it.
  void SetCursorVisible(bool visible);
  
  /// \brief Query if the mouse cursor is in visible state (mouse cursors need to be loaded vua a resource file).
  /// \note This function will use the first available GUI context of the GUIManager.
  /// If there is no GUI context it will internally create a new GUI context.
  /// \return true if visible, otherwise false.
  bool IsCursorVisible();
  
  /// @}
};

#endif