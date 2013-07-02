

#ifndef VLUA_APIDOC

%nodefaultctor IVConsoleManager;
%nodefaultdtor IVConsoleManager;

class IVConsoleManager
{
public:

  %rename OutputTextLine(const char *szText) PrintLine;
  virtual void OutputTextLine(const char *szText);

  virtual void SetAllowed(bool bAllow);

  virtual bool IsAllowed() const;

  %rename Show(bool bShow) SetVisible;
  virtual void Show(bool bShow);

  virtual bool IsVisible() const;
};


#else

/// \brief
///  Access the Game console via the global \b Console instance
/// \par Example
///   \code
///     if not Console:IsVisble() then
///       Console:SetVisible(true)
///       Console:PrintLine("Opening the console via script!")
///     else
///       Console:PrintLine("This is the console...")
///     end
///   \endcode
class IVConsoleManager {
public:

  /// @name Console Functions
  /// @{
  
  /// \brief  Print a single line to the console.
  /// \param message The message to print.
  /// \par Example
  ///   \code
  ///     Console:PrintLine("Hello World!")
  ///     Console:SetVisible(true)
  ///   \endcode
  void PrintLine(string message);

  /// \brief Set boolean flag if the user is allowed to request the console via the ^/` Key.
  /// \param allow Set to true (default) to allow user requests, otherwise to false.
  void SetAllowed(boolean allow);

  /// \brief Query if the user is allowed to request the console.
  /// \return true if it is allowed , otherwise to false.
  boolean IsAllowed();

  /// \brief Set the console visible or hide it again.
  /// \param visible Set to true to show the console, otherwise to false.
  void SetVisible(boolean visible);

  /// \brief Query if theconsole is currently visible.
  /// \return true if visible, otherwise false.
  /// \par Example
  ///   \code
  ///     if Console:IsVisble() then
  ///       Console:PrintLine("The console is opened now!")
  ///     end
  ///   \endcode
  boolean IsVisible();
  
  /// @}
  
};

#endif