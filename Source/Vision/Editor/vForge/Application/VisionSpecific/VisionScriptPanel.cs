/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.IO;
using System.Diagnostics;
using System.Drawing;
using System.Collections;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Windows.Forms;
using Microsoft.Win32;
using CSharpFramework;
using CSharpFramework.BaseTypes;
using CSharpFramework.Shapes;
using CSharpFramework.Docking;
using CSharpFramework.Controls;
using CSharpFramework.View;
using WeifenLuo.WinFormsUI;
using CSharpFramework.PropertyEditors;
using CSharpFramework.Dialogs;
using ManagedFramework;
using ScintillaNet;
using CSharpFramework.Helper;


namespace Editor.VisionSpecific
{
	/// <summary>
	/// Panel for (Lua-)scripting.
	/// </summary>
	public class VisionScriptPanel : ScriptPanel
  {
    #region Members

    private bool _imagesRegistered = false;
    private bool _containsOnExpose = false;

    #endregion

    #region Constants

    /// <summary>
    /// file name for the Lua API reference help document
    /// </summary>
    const string SCRIPT_API_REFERENCE_FILE = @"Docs\\Vision\\windows\\ScriptingAPI.chm";

    /// <summary>
    /// pattern of Lua keywords in the Lua.syn file
    /// </summary>
    const string LUA_PATTERN_KEYWORDS = "Keywords";
    const int LUA_PATTERN_KEYWORDS_IDX = 0;

    /// <summary>
    /// pattern of predefined callbacks
    /// </summary>
    const string LUA_PATTERN_CALLBACKS_CLASSES = "Callbacks";
    const int LUA_PATTERN_CALLBACKS_CLASSES_IDX = 1;

    /// <summary>
    /// pattern of Luastatic classes (Debug, Game, Application, ...) in the Lua.syn file
    /// </summary>
    const string LUA_PATTERN_STATIC_CLASSES = "Statics";
    const int LUA_PATTERN_STATIC_CLASSES_IDX = 2;

    /// <summary>
    /// pattern of already present modules
    /// </summary>
    const string LUA_PATTERN_MODULE = "Module";
    const int LUA_PATTERN_MODULE_IDX = 3;

    /// <summary>
    /// pattern of special vars like self, _G, etc. in the Lua.syn file
    /// </summary>
    const string LUA_PATTERN_SPECIAL_VARS = "Special Variables";
    const int LUA_PATTERN_SPECIAL_VARS_IDX = 4;

 
    /// <summary>
    /// pattern of constructors of the simple data types
    /// </summary>
    const string LUA_PATTERN_CTORS = "Constructors";
    const int LUA_PATTERN_CTORS_IDX = 5;

    /// <summary>
    /// Array containing the style areas where the auto completion is prohibited (strings, comments, etc...)
    /// </summary>
    byte[] LUA_AVOID_AUTO_COMPLETE_IN_STYLE_AREA = new byte [] { 1, 2, 3, 4, 5, 6, 7, 8, 12};

    #endregion

    #region Constructor / Destructor

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="container"></param>
    public VisionScriptPanel(DockingContainer container) : base(container)
	{
      EditorManager.ShapeSelectionChanged += new ShapeSelectionChangedEventHandler(EditorManager_ShapeSelectionChanged);
    }

    /// <summary>
    /// Destructor
    /// </summary>
    ~VisionScriptPanel()
    {
      EditorManager.ShapeSelectionChanged -= new ShapeSelectionChangedEventHandler(EditorManager_ShapeSelectionChanged);
    }

    #endregion

    #region Listeners

    /// <summary>
    /// Listen to the selection change so that we can open a script document
    /// Same behavior as ScriptPanel_VisibleChanged.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void EditorManager_ShapeSelectionChanged(object sender, ShapeSelectionChangedArgs e)
    {
      if (!this.Visible || !GlobalSettings._bEditScriptOnShapeSelection)
        return;

      ShapeCollection sel = e.NewSelection;

      // select script only if we select a single shape with a script
      if (sel == null || sel.Count != 1)
        return;

      string script = sel[0].ScriptFile;
      if (string.IsNullOrEmpty(script))
        return;

      this.AddDocument(script);
  
    }

    #endregion

    #region Overridden Tool Actions

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="doc"></param>
    /// <returns></returns>
    public override bool ValidateScriptDocument(string doc)
    {
      ConsoleOutDlg dlg = new ConsoleOutDlg();
      bool bResult = ScriptManager.ValidateScript(doc, dlg.Console);
      if (bResult)
        dlg.Console.Print("Summary: Script has been successfully validated");
      else
        dlg.Console.PrintWarning("Summary: One or more errors occurred during validation. See entries above.");
      dlg.ShowDialog();
      return bResult;
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    public override void OpenHelpWindow()
    {
      string installDir = FileHelper.VisionInstallDir;
      if (installDir == null)
      {
        EditorManager.ShowMessageBox("The Vision SDK installation path wasn't found.", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        return;
      }

      string helpFilePath = Path.Combine(installDir, SCRIPT_API_REFERENCE_FILE);
      if (!File.Exists(helpFilePath))
      {
        EditorManager.ShowMessageBox("The script API reference help document wasn't found.", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        return;
      }

      //Process.Start(helpFilePath);
      String searchString = _currentDoc==null ? null : (_currentDoc.ScintillaControl.Selection == null ? null : _currentDoc.ScintillaControl.Selection.Text);

      if (searchString==null||searchString.Length<1)
        System.Windows.Forms.Help.ShowHelp(this, helpFilePath);
      else
        System.Windows.Forms.Help.ShowHelp(this, helpFilePath, HelpNavigator.Index, searchString);
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    protected override void OnSaved()
    {
      if (ConfigureShapeComponentsPanel.PanelInstance == null || !EditorManager.Settings.SyncOnExposeVarsAfterSaving)
        return;

      bool containsOnExposeNow = _currentDoc.ScintillaControl.Text.Contains("OnExpose");

      //don't sync OnExpose vars if there is and was no OnExpose Callback
      if (!_containsOnExpose && !containsOnExposeNow)
        return;

      _containsOnExpose = containsOnExposeNow;

      //find out if the right component is select in the component  panel
      CSharpFramework.Scene.ShapeComponent[] selection = ConfigureShapeComponentsPanel.PanelInstance.CurrentComponentSelection;

      //ensure that the right component is selected
      if (selection!=null && selection.Length == 1 && selection[0] != null && selection[0].GetProperty("ScriptFile") != null)
      {
        //update the ScriptFile property so that the OnExpose callback is invoked
        ConfigureShapeComponentsPanel.PanelInstance.propertyGrid_PropertyValueChanged("ScriptFile", null);
      }

    }

    #endregion

    #region Autolist

    /// <summary>
    /// Request for the autolist to be shown if possible
    /// </summary>
    /// <param name="manualOpen"></param>
    public override void RequestAutoList(bool manualOpen)
    {
      string sciWordBeforeOperator = "";

      Scintilla SciControl = _currentDoc.ScintillaControl;
      CaretInfo caret = SciControl.Caret;

      //skip auto list if there is no carret
      if (caret == null) return;
      
      SymbolCollection list = null;

      char charBeforeCursor = (char)0;
      int offset = 0;

      if (caret.Position >= 1)
      {
        //avoid auto completion in comments, strings, etc...
        byte style = SciControl.Styles.GetStyleAt(caret.Position);
        for (int i = 0; i < LUA_AVOID_AUTO_COMPLETE_IN_STYLE_AREA.Length;i++ )
        {
          if (style == LUA_AVOID_AUTO_COMPLETE_IN_STYLE_AREA[i]) return;
        }

        charBeforeCursor = SciControl.CharAt(caret.Position - 1);
      }
      
      if (charBeforeCursor != 0)
      {
        //get the word without the current character
        sciWordBeforeOperator = SciControl.GetWordFromPosition(caret.Position - 2);
        
        if (sciWordBeforeOperator != null)
        {
          // go back to last operator on manual open
          // e.g: Debug:PritLi ->-+
          //           ^--<--<--<-+
          if (manualOpen)
          {
            char op = charBeforeCursor;

            //also stop if the start of the current line has been reached
            while (op != 0 && op != '.' && op != ':' && op != ' ' && op != '\n' && op != '\r' && op != '\t')
            {
              offset++;
              op = SciControl.CharAt(caret.Position - 2 - offset);
            }
            //grab the word before the operator
            string newWord = SciControl.GetWordFromPosition(caret.Position - 3 - offset);

            //use it if valid
            if (newWord != null)
            {
              sciWordBeforeOperator = newWord;
              charBeforeCursor = op;
              offset++;
            }
          }

          //handle the different operators
          switch(charBeforeCursor)
          {
            case '.': list = HandleDotOperator(sciWordBeforeOperator, offset); break;
            case ':': list = HandleColonOperator(sciWordBeforeOperator, offset); break;
            case ' ': list = HandleBlank(sciWordBeforeOperator); break;
            default:  break;
          }
        }
      }

      // Ctrl-Space pressed and no '.' or ':' -> show global scope symbols, keywords and special variables
      if ((list==null || list.Count==0) && manualOpen)
      {
        //showList = true;
        list = ScriptManager.GetGlobalSymbols();
        sciWordBeforeOperator = SciControl.GetWordFromPosition(caret.Position - 1);

        string keywordString = SciControl.Lexing.Keywords[LUA_PATTERN_KEYWORDS_IDX];
        string[] keywords = keywordString.Split(' ');
        // get keywords and special variables from the Lua.syn file
        foreach (string keyword in keywords)
          list.Add(new SymbolInfo(keyword, SymbolType.KEYWORD));

        keywordString = SciControl.Lexing.Keywords[LUA_PATTERN_SPECIAL_VARS_IDX];
        keywords = keywordString.Split(' ');
        foreach (string keyword in keywords)
        {
          if (!keyword.StartsWith("_"))
            list.Add(new SymbolInfo(keyword, SymbolType.VARIABLE));
        }
      }

      if (list!=null && list.Count>0)
      {
        SciControl.AutoComplete.List.Clear();

        if (!_imagesRegistered)
        {
          ImageList imageList = new ImageList();

          imageList.Images.Add(Properties.Resources.lua_function);
          imageList.Images.Add(Properties.Resources.lua_table);
          imageList.Images.Add(Properties.Resources.lua_userdata);
          imageList.Images.Add(Properties.Resources.lua_local);
          imageList.Images.Add(Properties.Resources.lua_blank);

          SciControl.AutoComplete.RegisterImages(imageList, Color.Black);

          _imagesRegistered = true;
        }

        foreach (SymbolInfo symbol in list)
        {
          int icoNum = 4;
          switch(symbol._type)
          {
            case SymbolType.FUNCTION: icoNum = 0; break;
            case SymbolType.VARIABLE: icoNum = 1; break;
            case SymbolType.CLASS:    icoNum = 2; break;
            case SymbolType.ENUM:     icoNum = 3; break; //currently we do not use enums...
            default: break;
          }

          string name = symbol._name + "?" + icoNum;

          //avoid duplicates
          if(!SciControl.AutoComplete.List.Contains(name))
            SciControl.AutoComplete.List.Add(name);
        }
        //insert a dummy if required (otherwise SciControl behaves strange)
        if (SciControl.AutoComplete.List.Count == 1)
        {
          SciControl.AutoComplete.List.Add("?4"); //insert a blank fake item
          SciControl.AutoComplete.SelectedIndex = 0;
        }
        else
        {
          SciControl.AutoComplete.List.Sort();

          //highlight the best match in list
          if (offset > 0) SciControl.AutoComplete.SelectedText = SciControl.GetWordFromPosition(caret.Position - 2);
          else            SciControl.AutoComplete.SelectedIndex = 0;
        }

        //show auto complete box
        SciControl.AutoComplete.AutoHide = false;
        SciControl.AutoComplete.Show();
      }
      
    }

    /// <summary>
    /// Fill auto list with symbols when a blank appears.
    /// This is only required when preceded by the keyword 'function'.
    /// E.g: Function OnCreate...
    /// </summary>
    /// <param name="sciWordBeforeOperator">The word preceding the operator.</param>
    /// <returns>A list of symbols for the current request (maybe null).</returns>
    private SymbolCollection HandleBlank(string sciWordBeforeOperator)
    {
      Scintilla SciControl = _currentDoc.ScintillaControl;
      SymbolCollection list = null;

      if (sciWordBeforeOperator == "function")
      {
        list = new SymbolCollection();

        //add the callbacks which could be defined in addition (filter out already defined)
        foreach (String callback in SciControl.Lexing.Keywords[LUA_PATTERN_CALLBACKS_CLASSES_IDX].Split(new char[] { ' ', '\n', '\t' }, StringSplitOptions.RemoveEmptyEntries))
        {
          if (!SciControl.Text.Contains(callback))
            list.Add(new SymbolInfo(callback, SymbolType.CONSTANT));
        }
      }

      return list;
    }

    /// <summary>
    /// Fill auto list with symbols when a ':' appears.
    /// This is for "method" calls in Lua like self:DoSomething()
    /// </summary>
    /// <param name="sciWordBeforeOperator">The word preceding the operator.</param>
    /// <param offset>The offset/number of characters in front of the operator
    /// (e.g. is 6 for manual invocation of 'Debug:PrintL'). This is always 0 for automatic
    /// invocation at the position of the operator</param>
    /// <returns>A list of symbols for the current request (maybe null).</returns>
    private SymbolCollection HandleColonOperator(string sciWordBeforeOperator, int offset)
    {
      Scintilla SciControl = _currentDoc.ScintillaControl;
      CaretInfo caret = SciControl.Caret;
      SymbolCollection list = null;

      // access to a variable's members
      // for now, simply add every member function (note: no member variables) that were defined
      if (sciWordBeforeOperator == "self")
      {
        //note: we are not sure if 'self' is really a base entity
        list = ScriptManager.GetFunctionsForClass("VisBaseEntity_cl *");

        if (list == null) list = new SymbolCollection();

        //add the callbacks defined in the file (or which could be defined)
        foreach (String callback in SciControl.Lexing.Keywords[LUA_PATTERN_CALLBACKS_CLASSES_IDX].Split(' '))
        {
          if (SciControl.Text.Contains(callback))
            list.Add(new SymbolInfo(callback, SymbolType.FUNCTION));
          else
            list.Add(new SymbolInfo(callback, SymbolType.CONSTANT));
        }
      }
      else
      {
        //dig down like this: abc.efg.hij:blah()
        int iDepth = 1;
        string command = sciWordBeforeOperator;
        while (SciControl.CharAt(caret.Position - 1 - command.Length - iDepth - offset) == '.')
        {
          iDepth++;
          command = SciControl.GetWordFromPosition(caret.Position - 1 - command.Length - iDepth - offset) + "." + command;
        }

        list = ScriptManager.GetFunctionsFromGlobal(command);
      }

      if (list == null || list.Count == 0)
      {
        //add methods of 'self'
        list = ScriptManager.GetFunctionsFromMetaTables();
      }

      return list;
    }

    /// <summary>
    /// Fill auto list with symbols when a '.' appears.
    /// This is for access to the tables or to members of Vision objects.
    /// E.g: Render.Draw
    /// </summary>
    /// <param name="sciWordBeforeOperator">The word preceding the operator.</param>
    /// <param offset>The offset/number of characters in front of the operator
    /// (e.g. is 3 for manual invocation of 'Debug.Dra'). This is always 0 for automatic
    /// invocation at the position of the operator</param>
    /// <returns>A list of symbols for the current request (maybe null).</returns>
    private SymbolCollection HandleDotOperator(string sciWordBeforeOperator, int offset)
    {
      Scintilla SciControl = _currentDoc.ScintillaControl;
      CaretInfo caret = SciControl.Caret;
      SymbolCollection list = null;

      // access to a static function or enum constant
      if (SciControl.Lexing.Keywords[LUA_PATTERN_STATIC_CLASSES_IDX].Contains(sciWordBeforeOperator) ||
          SciControl.Lexing.Keywords[LUA_PATTERN_MODULE_IDX].Contains(sciWordBeforeOperator))
      {
        //drill down the hierarchy: Application.FileSystem.blah
        int iDepth = 1;
        string command = sciWordBeforeOperator;
        while (SciControl.CharAt(caret.Position - 1 - command.Length - iDepth - offset) == '.')
        {
          iDepth++;
          command = SciControl.GetWordFromPosition(caret.Position - 1 - command.Length - iDepth - offset) + "." + command;
        }

        list = ScriptManager.GetMembersForSymbol(command);

        //add static members (for modules)
        SymbolCollection additionals = ScriptManager.GetMembersFromMetaTable(sciWordBeforeOperator);
        if (list == null)
        {
          list = additionals;
        }
        else if (additionals != null)
        {
          foreach (SymbolInfo symbol in additionals)
            list.Add(symbol);
        }

        //remove unwanted duplicates of the vision module (inside other modules)
        if (sciWordBeforeOperator != "Vision" && additionals != null && list != null)
        {
          SymbolCollection unwanted = ScriptManager.GetMembersFromMetaTable("Vision");
          foreach (SymbolInfo symbol in unwanted)
            if (list.Contains(symbol))
              list.Remove(symbol);
        }
      }
      else if (SciControl.Lexing.Keywords[LUA_PATTERN_SPECIAL_VARS_IDX].Contains(sciWordBeforeOperator))
      {
        list = ScriptManager.GetMembersFromMetaTable(sciWordBeforeOperator);
      }

      return list;
    }

    #endregion
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
