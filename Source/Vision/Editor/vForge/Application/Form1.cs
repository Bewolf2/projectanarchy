/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Diagnostics;
using System.Drawing;
using System.Globalization;
using System.IO;
using System.Runtime.InteropServices;
using System.Runtime.Serialization.Formatters.Binary;
using System.Threading;
using System.Windows.Forms;
using CSharpFramework;
using CSharpFramework.Contexts;
using CSharpFramework.Controls;
using CSharpFramework.Dialogs;
using CSharpFramework.Docking;
using CSharpFramework.Helper;
using CSharpFramework.Layout;
using CSharpFramework.Math;
using CSharpFramework.Scene;
using CSharpFramework.Serialization;
using CSharpFramework.Shapes;
using CSharpFramework.ShortCuts;
using CSharpFramework.Visitors;
using Editor.Actions;
using Editor.Dialogs;
using Editor.View;
using Editor.VisionSpecific;
using ManagedFramework;
// NUnit includes
using NUnit.Core;
using CSharpFramework.View;
using AssetManagerManaged;
using ManagedBase;
using CSharpFramework.AssetManagement;
using Editor.Vision;

namespace Editor
{
  /// <summary>
  /// Main Form class.
  /// </summary>
  public class Form1 : System.Windows.Forms.Form
  {
    private Editor.PropertyPanel propertyPanel1;
    private Editor.EnginePanel enginePanel1;

    private static bool g_triggerTests = false;
    private CSharpFramework.Docking.DockingContainer dockContainer;
    private static bool g_testResult = false;
    private ProfileToolStripComboBox profileComboBox;
    private AssetPreviewModeButton assetPreviewModeButton;
    private ToolStripComboBox layoutComboBox;
    private ToolStripComboBox selectionSetComboBox;
    private ToolBarButton sep9;
    private MenuStrip mainMenu;
    private ToolStripMenuItem Menu_File;
    private ToolStripMenuItem Menu_File_New;
    private ToolStripMenuItem Menu_File_Open;
    private ToolStripMenuItem Menu_File_Save;
    private ToolStripMenuItem Menu_File_Export;
    private ToolStripSeparator toolStripMenuItem1;
    private ToolStripMenuItem Menu_File_ProjectHelpers;
    private ToolStripMenuItem Menu_File_SceneHelpers;
    private ToolStripSeparator toolStripMenuItem2;
    private ToolStripMenuItem RecentProjects;
    private ToolStripMenuItem RecentScenes;
    private ToolStripSeparator toolStripMenuItem3;
    private ToolStripMenuItem Menu_File_CloseProject;
    private ToolStripMenuItem Menu_File_StartPage;
    private ToolStripMenuItem Menu_File_Exit;
    private ToolStripMenuItem Menu_Edit;
    private ToolStripMenuItem Menu_View;
    private ToolStripMenuItem Menu_Create;
    private ToolStripMenuItem Menu_Play;
    private ToolStripMenuItem Menu_Engine;
    private ToolStripMenuItem Menu_StaticLighting;
    private ToolStripMenuItem Menu_File_New_Project;
    private ToolStripMenuItem Menu_File_New_Scene;
    private ToolStripMenuItem Menu_File_Open_Project;
    private ToolStripMenuItem Menu_File_Open_Scene;
    private ToolStripMenuItem Menu_File_Save_All;
    private ToolStripMenuItem Menu_File_Save_Scene;
    private ToolStripMenuItem Menu_File_Save_SceneAs;
    private ToolStripMenuItem Menu_File_Export_Scene;
    private ToolStripMenuItem Menu_File_Export_LayersAsPrefabs;
    private ToolStripMenuItem Menu_File_Export_LayersAsPrefabsIntoFolder;
    private ToolStripSeparator Menu_File_Export_Separator;
    private ToolStripMenuItem Menu_File_Export_BatchProject;
    private ToolStripMenuItem Menu_File_Export_BatchDirectory;
    private ToolStripMenuItem Menu_File_Project_OpenDirectory;
    private ToolStripMenuItem Menu_File_Project_Move;
    private ToolStripMenuItem Menu_Options;
    private ToolStripMenuItem Menu_Remote;
    private ToolStripMenuItem Menu_Help;
    private ToolStripMenuItem Menu_Tests;
    private ToolStripMenuItem Menu_File_Scene_Settings;
    private ToolStripMenuItem Menu_File_Scene_UniqueIDs;
    private ToolStripMenuItem Menu_File_Scene_Thumbnail;
    private ToolStripMenuItem Menu_File_Scene_ShapeLinkDebugFile;
    private ToolStripMenuItem Menu_File_Scene_MergeExternalLayers;
    private ToolStripMenuItem Menu_Edit_Undo;
    private ToolStripMenuItem Menu_Edit_Redo;
    private ToolStripSeparator toolStripMenuItem4;
    private ToolStripMenuItem Menu_Edit_Cut;
    private ToolStripMenuItem Menu_Edit_Copy;
    private ToolStripMenuItem Menu_Edit_Paste;
    private ToolStripMenuItem Menu_Edit_Delete;
    private ToolStripSeparator toolStripMenuItem5;
    private ToolStripMenuItem Menu_Edit_FindShapes;
    private ToolStripMenuItem Menu_Edit_FindDuplicates;
    private ToolStripMenuItem Menu_Create_NewLayer;
    private ToolStripMenuItem Menu_Create_NewZone;
    private ToolStripMenuItem Menu_Create_ZoneGrid;
    private ToolStripSeparator Menu_Create_Separator;
    private ToolStripMenuItem Menu_Create_Dummy;
    private ToolStripMenuItem Menu_Play_Animate;
    private ToolStripMenuItem Menu_Play_RunInEditor;
    private ToolStripMenuItem Menu_Play_PlayTheGame;
    private ToolStripSeparator toolStripMenuItem7;
    private ToolStripMenuItem Menu_Play_TimeScale;
    private ToolStripMenuItem Menu_Play_TS_100;
    private ToolStripSeparator toolStripMenuItem8;
    private ToolStripMenuItem Menu_Play_TS_10;
    private ToolStripMenuItem Menu_Play_TS_25;
    private ToolStripMenuItem Menu_Play_TS_50;
    private ToolStripMenuItem Menu_Play_TS_200;
    private ToolStripMenuItem Menu_Play_TS_500;
    private ToolStripMenuItem Menu_Engine_EditSky;
    private ToolStripSeparator toolStripMenuItem9;
    private ToolStripMenuItem Menu_Engine_CalculateVisibility;
    private ToolStripMenuItem Menu_Engine_UseVisibility;
    private ToolStripSeparator toolStripMenuItem15;
    private ToolStripMenuItem Menu_Engine_ReloadResources;
    private ToolStripMenuItem Menu_Engine_ShowResources;
    private ToolStripMenuItem Menu_Engine_PurgeResources;
    private ToolStripSeparator toolStripMenuItem16;
    private ToolStripMenuItem Menu_Engine_SaveScreenshot;
    private ToolStripMenuItem Menu_Engine_SaveScreenshotAs;
    private ToolStripMenuItem Menu_StaticLighting_ActiveTool;
    private ToolStripMenuItem Menu_StaticLighting_ActiveTool_Dummy;
    private ToolStripMenuItem Menu_StaticLighting_EditSettings;
    private ToolStripMenuItem Menu_StaticLighting_CalculateLighting;
    private ToolStripMenuItem Menu_StaticLighting_ResetLighting;
    private ToolStripMenuItem Menu_Options_Settings;
    private ToolStripMenuItem Menu_Options_Hotkeys;
    private ToolStripSeparator toolStripMenuItem17;
    private ToolStripMenuItem Menu_Options_ShowPlugins;
    private ToolStripSeparator toolStripMenuItem18;
    private ToolStripMenuItem Menu_Remote_XBOX360_Connect;
    private ToolStripMenuItem Menu_Remote_XBOX360_Launch;
    private ToolStripMenuItem Menu_Remote_XBOX360_Animate;
    private ToolStripMenuItem Menu_Remote_XBOX360_PlayTheGame;
    private ToolStripMenuItem Menu_Remote_XBOX360_Stop;
    private ToolStripSeparator Menu_Remote_XBOX360_Seperator;
    private ToolStripMenuItem Menu_Remote_PS3_Connect;
    private ToolStripMenuItem Menu_Remote_PS3_Animate;
    private ToolStripMenuItem Menu_Remote_PS3_PlayTheGame;
    private ToolStripMenuItem Menu_Remote_PS3_Stop;
    private ToolStripSeparator Menu_Remote_PS3_Seperator;
    private ToolStripMenuItem Menu_Remote_ExportAndRun;
    private ToolStripMenuItem Menu_Remote_Restart;
    private ToolStripMenuItem Menu_Remote_ReloadResources;
    private ToolStripMenuItem Menu_Help_iHelp;
    private ToolStripSeparator toolStripMenuItem21;
    private ToolStripMenuItem Menu_Help_About;
    private ToolStripMenuItem Menu_Help_VisionDoc;
    private ToolStripMenuItem Menu_Tests_RunTests;
    private ToolStripMenuItem Menu_Tests_VideoSize;
    private ToolStripSeparator toolStripMenuItem10;
    private ToolStrip mainToolBar;
    private ToolStripSplitButton ToolBar_New;
    private ToolStripSplitButton ToolBar_Open;
    private ToolStripSeparator ToolBar_Sep4;
    private ToolStripButton Toolbar_Cut;
    private ToolStripButton Toolbar_Copy;
    private ToolStripButton Toolbar_Paste;
    private ToolStripButton Toolbar_Delete;
    private ToolStripSeparator ToolBar_Sep2;
    private ToolStripButton Toolbar_Redo;
    private ToolStripButton ToolBar_Preferences;
    private ToolStripSeparator ToolBar_Sep3;
    private ToolStripButton ToolBar_Hotkeys;
    private ToolStripButton Toolbar_Undo;
    private ToolStripSeparator ToolBar_Sep1;
    private ToolStripSeparator ToolBar_Sep5;
    private ToolStripSplitButton ToolBar_Save;
    private ToolStripSplitButton ToolBar_Export;
    private ToolStripMenuItem ToolBar_NewProject;
    private ToolStripMenuItem ToolBar_NewScene;
    private ToolStripMenuItem ToolBar_SaveAll;
    private ToolStripMenuItem ToolBar_SaveScene;
    private ToolStripMenuItem ToolBar_SaveAs;
    private ToolStripMenuItem ToolBar_ExportSettings;
    private ToolStripMenuItem ToolBar_ExportQuick;
    private ToolStripMenuItem ToolBar_OpenProject;
    private ToolStripMenuItem ToolBar_OpenScene;
    private ToolStripSeparator toolStripMenuItem6;
    private ToolStripDropDownButton ToolBar_Panels;
    private ToolStripMenuItem Menu_Engine_EditTimeOfDay;
    private ToolStripDropDownButton ToolBar_Resources;
    private ToolStripMenuItem ToolBar_Resources_Reload;
    private ToolStripMenuItem ToolBar_Resources_Show;
    private ToolStripMenuItem ToolBar_Resources_Purge;
    private ToolStripMenuItem Menu_File_Scene_Description;
    private ToolStripDropDownButton ToolBar_Tools;
    private ToolStripMenuItem ToolBar_SkyEditor;
    private ToolStripMenuItem ToolBar_TimeOfDayEditor;
    private ToolStripMenuItem ToolBar_PostProcessor;
    private ToolStripSeparator toolStripMenuItem11;
    private ToolStripMenuItem ToolBar_FindShapes;
    private ToolStripMenuItem ToolBar_FindDuplicates;
    private ToolStripSeparator toolStripMenuItem12;
    private ToolStripMenuItem ToolBar_SaveScreenshot;
    private ToolStripMenuItem ToolBar_SaveScreenshotAs;
    private ToolStripDropDownButton ToolBar_Relight;
    private ToolStripMenuItem ToolBar_LightSettings;
    private ToolStripMenuItem ToolBar_LightCalc;
    private ToolStripMenuItem ToolBar_LightReset;
    private ToolStripDropDownButton ToolBar_Visibility;
    private ToolStripMenuItem ToolBar_CalcVisibility;
    private ToolStripMenuItem ToolBar_UseVisibility;
    private ToolStripSeparator ToolBar_Tools_LastSeparator;
    private ToolStripMenuItem Menu_Engine_EditPostProcessors;
    private ToolStripMenuItem Menu_Engine_EditFog;
    private ToolStripMenuItem editFogToolStripMenuItem;
    private ToolStripMenuItem Menu_Engine_ProfileSettings;
    private ToolStripMenuItem Menu_Engine_ManifestSettings;
    private ToolStripButton ToolBar_OpenAssetBrowser;
    private StatusStrip statusBar;
    private ToolStripStatusLabel statusPanel_Main;
    private TransformStatusToolStripItem statusPanel_TransformProgress;
    private ToolStripSeparator toolStripSeparator1;
    private ToolStripStatusLabel statusPanel_Layout;
    private ToolStripStatusLabel statusPanel_PlayMode;
    private ToolStripSeparator toolStripSeparator3;
    private ToolStripSeparator toolStripSeparator2;

    #region Nested Class : EditorCustomFileLoadingArgs

    /// <summary>
    /// Implements some virtual functions to load project etc.
    /// </summary>
    private class EditorCustomFileLoadingArgs : CustomFileLoadingArgs
    {
      public EditorCustomFileLoadingArgs(Form1 form, string absPathName) : base(absPathName)
      {
        _form = form;
      }

      /// <summary>
      /// Overridden function to load the editor project
      /// </summary>
      /// <returns></returns>
      public override IProject LoadProject(bool bShowErrorDlg)
      {
        string projectFile = EditorProject.FindProjectFor(this.AbsPathName);
        if (projectFile==null)
        {
          if (bShowErrorDlg)
            EditorManager.ShowMessageBox("Could not find a matching .project file for:\n"+AbsPathName+"\n\nPlease create a project in this folder!",
              "Error",MessageBoxButtons.OK,MessageBoxIcon.Error);
          return null;
        }

        bool bResult = _form.LoadProject(projectFile,false);

        // already displayed in LoadProject
        // if (!bResult && bShowErrorDlg)
        // EditorManager.ShowMessageBox("Failed to load matching .project file for:\n"+AbsPathName,"Error",MessageBoxButtons.OK,MessageBoxIcon.Error);

        return EditorManager.Project;
      }

      private Form1 _form;
    }

    #endregion

    #region Form functions

    /// <summary>
    /// Message filter for filtering out keyboard shortcuts before they are forwarded to the controls and forms.
    /// We use a message filter instead of registering hotkeys to avoid keys getting blocked in other applications.
    /// </summary>
    private ShortCutMessageFilter _scMsgFilter;

    
    #if HK_ANARCHY

    private GoogleAnalyticsTracker _GATracker;

    #endif

    /// <summary>
    /// Constructor
    /// </summary>
    public Form1()
    {
      //Init the editor manager before the engine view gets initialized (so that we have an engine manager)
      //Note: this could cause problems if plugins need the form to exist during init
      EditorApp.InitEditorApp(this);

      EditorApp.EditorAppDelegates.SetFormTextDelegate = new SetTextDelegate(this.SetText);
      EditorApp.EditorAppDelegates.LoadProjectDelegate = new LoadFileDelegate(this.LoadProject);
      EditorApp.EditorAppDelegates.LoadSceneDelegate = new LoadFileDelegate(this.LoadScene);

      // Create the app interface for the docking library. Used to retrieve application specific data from
      // within the isolated docking library.
      WeifenLuo.WinFormsUI.AppInterface.Instance = new DockingApp();      
      LoadSettings(); // load settings from XML file

      //
      // Required for Windows Form Designer support
      //
      InitializeComponent();
      InitializeMyComponent(); // init the engine view

      // We have to call the update titlebar here so we have the DX11 flag in the titlebar
      EditorApp.UpdateTitleBar();

      // remove the 'Tests' menu if the Test data directory is not available, i.e. when vForge is shipped
      if (!TestManager.Helpers.TestDataDirExists)
      {
        this.mainMenu.Items.Remove(Menu_Tests);
      }

      EditorManager.InitPlugins(Application.StartupPath);

      EditorManager.EngineManager.SetExecutablePath(Path.GetFullPath(Application.StartupPath));
      EditorManager.EngineManager.SetSDKPath(Path.GetFullPath(Application.StartupPath + @"\..\..\..\..\"));

      UpdateToolbars();
      
      EditorManager.Actions.ActionEvent += new ActionEventHandler(Actions_ActionEvent);
      EditorManager.SceneChanged += new SceneChangedEventHandler(EditorManager_SceneChanged);
      EditorManager.EditorModeChanged += new EditorModeChangedEventHandler(EditorManager_EditorModeChanged);
      EditorManager.ShapeSelectionChanged += new ShapeSelectionChangedEventHandler(EditorManager_ShapeSelectionChanged);
      EditorManager.ApplicationLayout.ActiveLayoutChanged += new LayoutManager.ActiveLayoutChangedEventHandler(ApplicationLayout_ActiveLayoutChanged);
      EditorManager.ApplicationLayout.AvailableLayoutsChanged += new CSharpFramework.Layout.LayoutManager.AvailableLayoutsChangedEventHandler(ApplicationLayout_AvailableLayoutsChanged);
      EditorManager.BeforeEditorAppShutdown += new EditorAppShutdownEventHandler(EditorManager_BeforeEditorAppShutdown);
      Load += new EventHandler(Form1_Load);
      IVisibilityBuilder.OnVisibilityStatusChanged += new EventHandler(IVisibilityBuilder_OnVisibilityStatusChanged);
      IVisibilityBuilder.OnVisibilityUseInEngineChanged += new EventHandler(IVisibilityBuilder_OnVisibilityUseInEngineChanged);
      IScene.OnDirtyFlagChanged += new EventHandler(IScene_OnDirtyFlagChanged);
      Application.Idle += new EventHandler(Application_Idle);

      Scene2DView.RelevantObjects.Add(ZoneActionProvider.ACTION_PROVIDER); // register one global instance

      // Initialize Remote Manager
      ManagedFramework.ManagedFramework.OneTimeInit();
      InitializeRemoteManager(); 
   
      // Initially update status bar
      UpdateStatusBar();

      // special case shapes
      if (EditorManager.Settings.ExposeVisibilityShapes)
      {
        EditorManager.ShapeCreatorPlugins.Add(new VisibilityZoneShapeCreator());
        EditorManager.ShapeCreatorPlugins.Add(new VisibilityZoneShapeCreator_HallwayX());
        EditorManager.ShapeCreatorPlugins.Add(new VisibilityZoneShapeCreator_HallwayY());
        EditorManager.ShapeCreatorPlugins.Add(new VisibilityZoneShapeCreator_Room());
        EditorManager.ShapeCreatorPlugins.Add(new PortalShapeCreator());
        
      }

      // hotkeys:
      ShortCutConfig shortcuts = EditorManager.ShortCuts;
      shortcuts.Add(new GizmoModeShortCut(Keys.W, ShapeDragMode.MOVE));
      shortcuts.Add(new GizmoModeShortCut(Keys.E, ShapeDragMode.ROTATE));
      shortcuts.Add(new GizmoModeShortCut(Keys.R, ShapeDragMode.UNIFORMSCALE));
      shortcuts.Add(new GizmoModeShortCut(Keys.None, ShapeDragMode.LINK));
      shortcuts.Add(new GizmoSwitchShortCut(Keys.Q));
      shortcuts.Add(new CameraStyleShortCut(Keys.None, EditorSettingsBase.CameraStyle_e.MaxStyle)); 
      shortcuts.Add(new CameraStyleShortCut(Keys.None, EditorSettingsBase.CameraStyle_e.MayaStyle)); 
      shortcuts.Add(new CameraStyleShortCut(Keys.None, EditorSettingsBase.CameraStyle_e.MiddleMouseOrbitSelection)); 
      shortcuts.Add(new CameraStyleShortCut(Keys.None, EditorSettingsBase.CameraStyle_e.MiddleMousePan)); 
      shortcuts.Add(new CameraStyleShortCut(Keys.None, EditorSettingsBase.CameraStyle_e.MiddleMousePanHorizontal));
      shortcuts.Add(new ShapeSelectionModeShortCut(Keys.None, EditorSettingsBase.ShapeSelectionMode_e.ActiveLayer));
      shortcuts.Add(new ShapeSelectionModeShortCut(Keys.None, EditorSettingsBase.ShapeSelectionMode_e.All));
      shortcuts.Add(new ShapeSelectionModeShortCut(Keys.None, EditorSettingsBase.ShapeSelectionMode_e.VisibleLayer));
      shortcuts.Add(new ToggleLocalOrientation(Keys.None));
      shortcuts.Add(new PerspectiveModeShortCut(VisionViewBase.ProjectionMode_e.Perspective, Keys.None));
      shortcuts.Add(new PerspectiveModeShortCut(VisionViewBase.ProjectionMode_e.Top, Keys.None));
      shortcuts.Add(new PerspectiveModeShortCut(VisionViewBase.ProjectionMode_e.Front, Keys.None));
      shortcuts.Add(new PerspectiveModeShortCut(VisionViewBase.ProjectionMode_e.Right, Keys.None));

      shortcuts.Add(new MenuItemShortCut(Menu_File_New_Scene, Keys.None));
      shortcuts.Add(new MenuItemShortCut(Menu_File_Open_Scene, Keys.None));
      shortcuts.Add(new MenuItemShortCut(Menu_File_Save_Scene, Keys.Control | Keys.S));
      shortcuts.Add(new MenuItemShortCut(Menu_File_Save_SceneAs, Keys.None));
      shortcuts.Add(new MenuItemShortCut(Menu_File_Export_Scene,Keys.None));
      shortcuts.Add(new MenuItemShortCut(Menu_Edit_FindShapes, Keys.Control | Keys.F));
      shortcuts.Add(new MenuItemShortCut(Menu_Play_Animate,Keys.None));
      shortcuts.Add(new MenuItemShortCut(Menu_Play_RunInEditor,Keys.None));
      shortcuts.Add(new MenuItemShortCut(Menu_Play_PlayTheGame,Keys.None));
      shortcuts.Add(new MenuItemShortCut(Menu_Engine_ReloadResources,Keys.None));
      shortcuts.Add(new MenuItemShortCut(Menu_Engine_SaveScreenshot,Keys.None));
      shortcuts.Add(new MenuItemShortCut(Menu_Engine_SaveScreenshotAs,Keys.None));
      shortcuts.Add(new MenuItemShortCut(Menu_StaticLighting_EditSettings, Keys.None));

      shortcuts.Add(new ReservedShortCut("Menu File", Keys.Alt | Keys.F));
      shortcuts.Add(new ReservedShortCut("Menu Edit", Keys.Alt | Keys.E));
      shortcuts.Add(new ReservedShortCut("Menu View", Keys.Alt | Keys.V));
      shortcuts.Add(new ReservedShortCut("Menu Create", Keys.Alt | Keys.C));
      shortcuts.Add(new ReservedShortCut("Menu Play", Keys.Alt | Keys.P));
      shortcuts.Add(new ReservedShortCut("Menu Engine", Keys.Alt | Keys.N));
      shortcuts.Add(new ReservedShortCut("Menu Static Lighting", Keys.Alt | Keys.S));
      shortcuts.Add(new ReservedShortCut("Menu Help", Keys.Alt | Keys.H));
      shortcuts.Add(new ReservedShortCut("Cut", (Keys)Shortcut.CtrlX));
      shortcuts.Add(new ReservedShortCut("Copy", (Keys)Shortcut.CtrlC));
      shortcuts.Add(new ReservedShortCut("Paste", (Keys)Shortcut.CtrlV));
      shortcuts.Add(new ReservedShortCut("Delete", (Keys)Shortcut.Del));
      shortcuts.Add(new ReservedShortCut("Insert", (Keys)Shortcut.Ins));
      shortcuts.Add(new ReservedShortCut("Undo", (Keys)Shortcut.CtrlZ));
      shortcuts.Add(new ReservedShortCut("Redo", (Keys)Shortcut.CtrlY));
      shortcuts.Add(new ReservedShortCut("Select All", (Keys)Shortcut.CtrlA));

      shortcuts.Add(SaveResourceThumbnailShortCut.Instance);

      // Add a message filter for processing the keyboard shortcuts
      _scMsgFilter= new ShortCutMessageFilter();
      Application.AddMessageFilter(_scMsgFilter);

      EditorManager.GUI.RecordableControls.Add("Menu->Engine->Reload Resources", Menu_Engine_ReloadResources);
      EditorManager.GUI.RecordableControls.Add("Menu->Engine->Purge Resources", Menu_Engine_PurgeResources);
      EditorManager.GUI.RecordableControls.Add("Menu->Engine->Save Screenshot", Menu_Engine_SaveScreenshot);
      EditorManager.GUI.RecordableControls.Add("Menu->Engine->Calculate Visibility", Menu_Engine_CalculateVisibility);
      EditorManager.GUI.RecordableControls.Add("Menu->Engine->Use Visibility", Menu_Engine_UseVisibility);
      
      // register to clipboard notification chain
      nextClipboardChainViewer = SetClipboardViewer(this.Handle);
    }

    /// <summary>
    /// Set form text. 
    /// </summary>
    /// <param name="text"></param>
    public void SetText(string text)
    {
        this.Text = text;
    }

    private bool _isDisposed = false;
 
    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    protected override void Dispose( bool disposing )
    {
      if (_isDisposed)
        return;

      if( disposing )
      {
        // Cannot access handle here anymore.
        ChangeClipboardChain(this.Handle, nextClipboardChainViewer); // remove from chain
        if (EditorManager.Clipboard.Data != null && !EditorManager.Clipboard.Data.CanSerialize)
          EditorManager.Clipboard.Data = null; // in this case, clean up

        EditorManager.BeforeEditorAppShutdown -= new EditorAppShutdownEventHandler(EditorManager_BeforeEditorAppShutdown);

        Load -= new EventHandler(Form1_Load);
        IVisibilityBuilder.OnVisibilityStatusChanged -= new EventHandler(IVisibilityBuilder_OnVisibilityStatusChanged);
        IVisibilityBuilder.OnVisibilityUseInEngineChanged -= new EventHandler(IVisibilityBuilder_OnVisibilityUseInEngineChanged);
        IScene.OnDirtyFlagChanged -= new EventHandler(IScene_OnDirtyFlagChanged);

        shapeCreatorPanel.SelectedCreatorObjectChanged -= new EventHandler(shapeCreatorPanel_SelectedCreatorObjectChanged);
        vResourceViewer.Classes.ResourceInfo.OnSelectedResourceChanged -= new EventHandler(ResourceInfo_OnSelectedResourceChanged);
        vResourceViewerBase80.Controls.MasterPanel.OnAutomatedResourcePreview -= new EventHandler(ResourceInfo_OnAutomatedResourcePreview);
        resourceViewerPanel.VisibleChanged -= new EventHandler(resourceViewerPanel_VisibleChanged);
        layoutComboBox.SelectedIndexChanged -= new EventHandler(layoutBox_SelectedIndexChanged);

        // Save layout
        EditorManager.ApplicationLayout.Save();
        if (EditorManager.ApplicationLayout.ActiveLayout != null)
          EditorManager.Settings.ActiveApplicationLayout = EditorManager.ApplicationLayout.ActiveLayout.Name;

        // write MRU lists to registry
        mruList_RecentProjects.SaveToRegistry();
        mruList_RecentScenes.SaveToRegistry();

        // Update window size for restore during next start
        switch (this.WindowState)
        {
          case System.Windows.Forms.FormWindowState.Normal:
            // When closing in normal window state, save all attributes
            EditorManager.Settings.WindowSizeOnRestore = this.Size;
            EditorManager.Settings.WindowLocationOnRestore = this.Location;
            EditorManager.Settings.WindowSizeOnRestoreMaximized = false;
            break;
          case System.Windows.Forms.FormWindowState.Maximized:
            // When closing maximized, just save to restart maximized
            EditorManager.Settings.WindowSizeOnRestoreMaximized = true;
            break;
          case System.Windows.Forms.FormWindowState.Minimized:
            // This is intentional as to not confuse users if the application starts minimized!
            EditorManager.Settings.WindowSizeOnRestoreMaximized = true;
            break;
        }

        SaveSettings(); // to XML file

        // do some engine cleanup here
        if (EditorManager.Scene!=null)
          EditorManager.Scene.OnRemoveAllEngineInstances();
      }

      _isDisposed = true;

      base.Dispose( disposing );
    }

    private void Form1_Load(object sender, EventArgs e)
    {
      // in the Main it was too early to call it (windows were not set up properly)
      if (_bFirstRun)
      {
        _bFirstRun = false;

        // Initialize the application's layout manager.
        // Note that the layout manager requires a fully initialized main form.
        // It won't work while the form is being constructed (main form constructor).
        EditorManager.ApplicationLayout.Initialize();
        Layout activeLayout = EditorManager.ApplicationLayout.FindLayout(EditorManager.Settings.ActiveApplicationLayout);
        if (activeLayout != null)
          EditorManager.ApplicationLayout.ActiveLayout = activeLayout;

        // Add dynamic panel/layout menu items
        Menu_View.DropDownItems.Add(EditorManager.ApplicationLayout.Menu);
        Menu_View.DropDownItems.Add(dockContainer.PanelsMenuItem);
        dockContainer.AddPanelItemsToToolbar(ToolBar_Panels);

        // Initialize layout selection toolbar button
        InitializeSelectionSetBox();
        InitializeLayoutButton();
        InitializeProfileComboBox();

        // Restore old window size / location
        // Put here instead of e.g. InitialializeMyComponent because
        // the layout is messed up if InitizalizeLayoutButton and InitializeSelectionBox 
        // is called with a very small window, so we do it directly after that
        if (!EditorManager.Settings.WindowSizeOnRestoreMaximized)
        {
          this.WindowState = System.Windows.Forms.FormWindowState.Normal;
          this.Size = EditorManager.Settings.WindowSizeOnRestore;
          Point restoreLocation = EditorManager.Settings.WindowLocationOnRestore;

          // This is to e.g. catch cases where vForge was closed with 2 screens and one
          // screen is removed
          if (restoreLocation.X > SystemInformation.VirtualScreen.Right - 10 ||
            restoreLocation.X < (SystemInformation.VirtualScreen.Left - Size.Width + 10))
            restoreLocation.X = SystemInformation.VirtualScreen.Left + 50;
          if (restoreLocation.Y > SystemInformation.VirtualScreen.Bottom - 10 ||
            restoreLocation.Y < (SystemInformation.VirtualScreen.Top - Size.Height + 10))
            restoreLocation.Y = SystemInformation.VirtualScreen.Top + 50;
          this.Location = restoreLocation;
        }

        // Run editor tests
        if (g_triggerTests)
        {
          EditorManager.EngineManager.InitEditorTests();
          EditorManager.ApplicationLayout.Initialize();

          // execute the tests and close the application
          g_testResult = TestManager.ExecuteTests(false, false);

          EditorApp.ExitEditorApp(false);
          return;
        }

        // load scene file...
        ParseArgs(CommandLineArgs);

        // show warning dialog if mixed debug/release assemblies are loaded
        AssemblyHelper.CheckAssemblyConsistency();
      }

#if HK_ANARCHY
      // Initialize the tracker which listens to the necessary editor events to ensure correct stats tracking
      _GATracker = new GoogleAnalyticsTracker();
#endif

      // Show startup dialog (when there are command line arguments do not show the startup dialog)
      if (!TestManager.IsRunning && !EditorManager.SilentMode && EditorManager.Settings.OpenStartupDialog && CommandLineArgs.Length == 0)
        OpenStartupDialog();
    }

    private void Form1_Closing(object sender, System.ComponentModel.CancelEventArgs e)
    {
      e.Cancel = !EditorApp.ExitEditorApp(true); // Cancel the Cancel event? (looks weird, but is correct)
    }

    #endregion

    #region Windows Form Designer generated code
    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
      this.sep9 = new System.Windows.Forms.ToolBarButton();
      this.mainMenu = new System.Windows.Forms.MenuStrip();
      this.Menu_File = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_File_New = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_File_New_Project = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_File_New_Scene = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_File_Open = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_File_Open_Project = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_File_Open_Scene = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_File_Save = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_File_Save_All = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_File_Save_Scene = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_File_Save_SceneAs = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_File_Export = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_File_Export_Scene = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_File_Export_LayersAsPrefabs = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_File_Export_LayersAsPrefabsIntoFolder = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_File_Export_Separator = new System.Windows.Forms.ToolStripSeparator();
      this.Menu_File_Export_BatchProject = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_File_Export_BatchDirectory = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripSeparator();
      this.Menu_File_ProjectHelpers = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_File_Project_OpenDirectory = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_File_Project_Move = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_File_SceneHelpers = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_File_Scene_Settings = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_File_Scene_Description = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_File_Scene_UniqueIDs = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_File_Scene_Thumbnail = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_File_Scene_ShapeLinkDebugFile = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_File_Scene_MergeExternalLayers = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem2 = new System.Windows.Forms.ToolStripSeparator();
      this.RecentProjects = new System.Windows.Forms.ToolStripMenuItem();
      this.RecentScenes = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem3 = new System.Windows.Forms.ToolStripSeparator();
      this.Menu_File_CloseProject = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_File_StartPage = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_File_Exit = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Edit = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Edit_Undo = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Edit_Redo = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem4 = new System.Windows.Forms.ToolStripSeparator();
      this.Menu_Edit_Cut = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Edit_Copy = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Edit_Paste = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Edit_Delete = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem5 = new System.Windows.Forms.ToolStripSeparator();
      this.Menu_Edit_FindShapes = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Edit_FindDuplicates = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_View = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Create = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Create_NewLayer = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Create_NewZone = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Create_ZoneGrid = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Create_Separator = new System.Windows.Forms.ToolStripSeparator();
      this.Menu_Create_Dummy = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Play = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Play_Animate = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Play_RunInEditor = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Play_PlayTheGame = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem7 = new System.Windows.Forms.ToolStripSeparator();
      this.Menu_Play_TimeScale = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Play_TS_100 = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem8 = new System.Windows.Forms.ToolStripSeparator();
      this.Menu_Play_TS_10 = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Play_TS_25 = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Play_TS_50 = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Play_TS_200 = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Play_TS_500 = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Engine = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Engine_EditSky = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Engine_EditTimeOfDay = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Engine_EditFog = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Engine_EditPostProcessors = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Engine_ProfileSettings = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Engine_ManifestSettings = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem9 = new System.Windows.Forms.ToolStripSeparator();
      this.Menu_Engine_CalculateVisibility = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Engine_UseVisibility = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem15 = new System.Windows.Forms.ToolStripSeparator();
      this.Menu_Engine_ReloadResources = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Engine_ShowResources = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Engine_PurgeResources = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem16 = new System.Windows.Forms.ToolStripSeparator();
      this.Menu_Engine_SaveScreenshot = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Engine_SaveScreenshotAs = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_StaticLighting = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_StaticLighting_ActiveTool = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_StaticLighting_ActiveTool_Dummy = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_StaticLighting_EditSettings = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem10 = new System.Windows.Forms.ToolStripSeparator();
      this.Menu_StaticLighting_CalculateLighting = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_StaticLighting_ResetLighting = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Options = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Options_Settings = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Options_Hotkeys = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem17 = new System.Windows.Forms.ToolStripSeparator();
      this.Menu_Options_ShowPlugins = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem18 = new System.Windows.Forms.ToolStripSeparator();
      this.Menu_Remote = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Remote_XBOX360_Connect = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Remote_XBOX360_Launch = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Remote_XBOX360_Animate = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Remote_XBOX360_PlayTheGame = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Remote_XBOX360_Stop = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Remote_XBOX360_Seperator = new System.Windows.Forms.ToolStripSeparator();
      this.Menu_Remote_PS3_Connect = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Remote_PS3_Animate = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Remote_PS3_PlayTheGame = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Remote_PS3_Stop = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Remote_PS3_Seperator = new System.Windows.Forms.ToolStripSeparator();
      this.Menu_Remote_ExportAndRun = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Remote_Restart = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Remote_ReloadResources = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Help = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Help_iHelp = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Help_VisionDoc = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem21 = new System.Windows.Forms.ToolStripSeparator();
      this.Menu_Help_About = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Tests = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Tests_RunTests = new System.Windows.Forms.ToolStripMenuItem();
      this.Menu_Tests_VideoSize = new System.Windows.Forms.ToolStripMenuItem();
      this.mainToolBar = new System.Windows.Forms.ToolStrip();
      this.ToolBar_Preferences = new System.Windows.Forms.ToolStripButton();
      this.ToolBar_Hotkeys = new System.Windows.Forms.ToolStripButton();
      this.ToolBar_Sep1 = new System.Windows.Forms.ToolStripSeparator();
      this.ToolBar_New = new System.Windows.Forms.ToolStripSplitButton();
      this.ToolBar_NewProject = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolBar_NewScene = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolBar_Open = new System.Windows.Forms.ToolStripSplitButton();
      this.ToolBar_OpenProject = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolBar_OpenScene = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem6 = new System.Windows.Forms.ToolStripSeparator();
      this.ToolBar_Save = new System.Windows.Forms.ToolStripSplitButton();
      this.ToolBar_SaveScene = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolBar_SaveAll = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolBar_SaveAs = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolBar_Export = new System.Windows.Forms.ToolStripSplitButton();
      this.ToolBar_ExportSettings = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolBar_ExportQuick = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolBar_Sep2 = new System.Windows.Forms.ToolStripSeparator();
      this.Toolbar_Cut = new System.Windows.Forms.ToolStripButton();
      this.Toolbar_Copy = new System.Windows.Forms.ToolStripButton();
      this.Toolbar_Paste = new System.Windows.Forms.ToolStripButton();
      this.Toolbar_Delete = new System.Windows.Forms.ToolStripButton();
      this.ToolBar_Sep3 = new System.Windows.Forms.ToolStripSeparator();
      this.Toolbar_Undo = new System.Windows.Forms.ToolStripButton();
      this.Toolbar_Redo = new System.Windows.Forms.ToolStripButton();
      this.ToolBar_Sep4 = new System.Windows.Forms.ToolStripSeparator();
      this.ToolBar_Panels = new System.Windows.Forms.ToolStripDropDownButton();
      this.ToolBar_OpenAssetBrowser = new System.Windows.Forms.ToolStripButton();
      this.ToolBar_Tools = new System.Windows.Forms.ToolStripDropDownButton();
      this.ToolBar_SkyEditor = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolBar_TimeOfDayEditor = new System.Windows.Forms.ToolStripMenuItem();
      this.editFogToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolBar_PostProcessor = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem11 = new System.Windows.Forms.ToolStripSeparator();
      this.ToolBar_FindShapes = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolBar_FindDuplicates = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem12 = new System.Windows.Forms.ToolStripSeparator();
      this.ToolBar_SaveScreenshot = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolBar_SaveScreenshotAs = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolBar_Tools_LastSeparator = new System.Windows.Forms.ToolStripSeparator();
      this.ToolBar_Resources = new System.Windows.Forms.ToolStripDropDownButton();
      this.ToolBar_Resources_Reload = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolBar_Resources_Show = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolBar_Resources_Purge = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolBar_Relight = new System.Windows.Forms.ToolStripDropDownButton();
      this.ToolBar_LightSettings = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolBar_LightCalc = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolBar_LightReset = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolBar_Visibility = new System.Windows.Forms.ToolStripDropDownButton();
      this.ToolBar_CalcVisibility = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolBar_UseVisibility = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolBar_Sep5 = new System.Windows.Forms.ToolStripSeparator();
      this.statusBar = new System.Windows.Forms.StatusStrip();
      this.statusPanel_Main = new System.Windows.Forms.ToolStripStatusLabel();
      this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
      this.statusPanel_TransformProgress = new CSharpFramework.Controls.TransformStatusToolStripItem();
      this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
      this.statusPanel_Layout = new System.Windows.Forms.ToolStripStatusLabel();
      this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
      this.statusPanel_PlayMode = new System.Windows.Forms.ToolStripStatusLabel();
      this.dockContainer = new CSharpFramework.Docking.DockingContainer();
      this.mainMenu.SuspendLayout();
      this.mainToolBar.SuspendLayout();
      this.statusBar.SuspendLayout();
      this.SuspendLayout();
      // 
      // sep9
      // 
      this.sep9.Name = "sep9";
      // 
      // mainMenu
      // 
      this.mainMenu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.Menu_File,
            this.Menu_Edit,
            this.Menu_View,
            this.Menu_Create,
            this.Menu_Play,
            this.Menu_Engine,
            this.Menu_StaticLighting,
            this.Menu_Options,
            this.Menu_Remote,
            this.Menu_Help,
            this.Menu_Tests});
      this.mainMenu.Location = new System.Drawing.Point(0, 0);
      this.mainMenu.Name = "mainMenu";
      this.mainMenu.Size = new System.Drawing.Size(1028, 24);
      this.mainMenu.TabIndex = 0;
      // 
      // Menu_File
      // 
      this.Menu_File.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.Menu_File_New,
            this.Menu_File_Open,
            this.Menu_File_Save,
            this.Menu_File_Export,
            this.toolStripMenuItem1,
            this.Menu_File_ProjectHelpers,
            this.Menu_File_SceneHelpers,
            this.toolStripMenuItem2,
            this.RecentProjects,
            this.RecentScenes,
            this.toolStripMenuItem3,
            this.Menu_File_CloseProject,
            this.Menu_File_StartPage,
            this.Menu_File_Exit});
      this.Menu_File.Name = "Menu_File";
      this.Menu_File.Size = new System.Drawing.Size(37, 20);
      this.Menu_File.Text = "&File";
      this.Menu_File.DropDownClosed += new System.EventHandler(this.Menu_DropDownClosed);
      this.Menu_File.DropDownOpening += new System.EventHandler(this.Menu_File_Popup);
      // 
      // Menu_File_New
      // 
      this.Menu_File_New.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.Menu_File_New_Project,
            this.Menu_File_New_Scene});
      this.Menu_File_New.Image = global::Editor.Properties.Resources.toolbar_new_project;
      this.Menu_File_New.Name = "Menu_File_New";
      this.Menu_File_New.Size = new System.Drawing.Size(155, 22);
      this.Menu_File_New.Text = "New";
      // 
      // Menu_File_New_Project
      // 
      this.Menu_File_New_Project.Image = global::Editor.Properties.Resources.toolbar_new_project;
      this.Menu_File_New_Project.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.Menu_File_New_Project.Name = "Menu_File_New_Project";
      this.Menu_File_New_Project.Size = new System.Drawing.Size(111, 22);
      this.Menu_File_New_Project.Text = "Project";
      this.Menu_File_New_Project.Click += new System.EventHandler(this.Menu_File_New_Project_Click);
      // 
      // Menu_File_New_Scene
      // 
      this.Menu_File_New_Scene.Image = global::Editor.Properties.Resources.toolbar_new_scene;
      this.Menu_File_New_Scene.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.Menu_File_New_Scene.Name = "Menu_File_New_Scene";
      this.Menu_File_New_Scene.Size = new System.Drawing.Size(111, 22);
      this.Menu_File_New_Scene.Text = "Scene";
      this.Menu_File_New_Scene.Click += new System.EventHandler(this.Menu_File_New_Scene_Click);
      // 
      // Menu_File_Open
      // 
      this.Menu_File_Open.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.Menu_File_Open_Project,
            this.Menu_File_Open_Scene});
      this.Menu_File_Open.Image = global::Editor.Properties.Resources.toolbar_open_project;
      this.Menu_File_Open.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.Menu_File_Open.Name = "Menu_File_Open";
      this.Menu_File_Open.Size = new System.Drawing.Size(155, 22);
      this.Menu_File_Open.Text = "Open";
      // 
      // Menu_File_Open_Project
      // 
      this.Menu_File_Open_Project.Image = global::Editor.Properties.Resources.toolbar_open_project;
      this.Menu_File_Open_Project.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.Menu_File_Open_Project.Name = "Menu_File_Open_Project";
      this.Menu_File_Open_Project.Size = new System.Drawing.Size(143, 22);
      this.Menu_File_Open_Project.Text = "Open Project";
      this.Menu_File_Open_Project.Click += new System.EventHandler(this.Menu_File_OpenProject_Click);
      // 
      // Menu_File_Open_Scene
      // 
      this.Menu_File_Open_Scene.Image = global::Editor.Properties.Resources.toolbar_open_scene;
      this.Menu_File_Open_Scene.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.Menu_File_Open_Scene.Name = "Menu_File_Open_Scene";
      this.Menu_File_Open_Scene.Size = new System.Drawing.Size(143, 22);
      this.Menu_File_Open_Scene.Text = "Open Scene";
      this.Menu_File_Open_Scene.Click += new System.EventHandler(this.Menu_File_OpenScene_Click);
      // 
      // Menu_File_Save
      // 
      this.Menu_File_Save.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.Menu_File_Save_All,
            this.Menu_File_Save_Scene,
            this.Menu_File_Save_SceneAs});
      this.Menu_File_Save.Image = global::Editor.Properties.Resources.toolbar_save_scene;
      this.Menu_File_Save.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.Menu_File_Save.Name = "Menu_File_Save";
      this.Menu_File_Save.Size = new System.Drawing.Size(155, 22);
      this.Menu_File_Save.Text = "Save";
      // 
      // Menu_File_Save_All
      // 
      this.Menu_File_Save_All.Image = global::Editor.Properties.Resources.toolbar_save_all;
      this.Menu_File_Save_All.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.Menu_File_Save_All.Name = "Menu_File_Save_All";
      this.Menu_File_Save_All.Size = new System.Drawing.Size(148, 22);
      this.Menu_File_Save_All.Text = "Save A&ll";
      this.Menu_File_Save_All.Click += new System.EventHandler(this.Menu_File_SaveAll_Click);
      // 
      // Menu_File_Save_Scene
      // 
      this.Menu_File_Save_Scene.Image = global::Editor.Properties.Resources.toolbar_save_scene;
      this.Menu_File_Save_Scene.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.Menu_File_Save_Scene.Name = "Menu_File_Save_Scene";
      this.Menu_File_Save_Scene.Size = new System.Drawing.Size(148, 22);
      this.Menu_File_Save_Scene.Text = "&Save Scene";
      this.Menu_File_Save_Scene.Click += new System.EventHandler(this.Menu_File_SaveScene_Click);
      // 
      // Menu_File_Save_SceneAs
      // 
      this.Menu_File_Save_SceneAs.Image = global::Editor.Properties.Resources.toolbar_save_as;
      this.Menu_File_Save_SceneAs.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.Menu_File_Save_SceneAs.Name = "Menu_File_Save_SceneAs";
      this.Menu_File_Save_SceneAs.Size = new System.Drawing.Size(148, 22);
      this.Menu_File_Save_SceneAs.Text = "Save Scene &As";
      this.Menu_File_Save_SceneAs.Click += new System.EventHandler(this.Menu_File_SaveSceneAs_Click);
      // 
      // Menu_File_Export
      // 
      this.Menu_File_Export.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.Menu_File_Export_Scene,
            this.Menu_File_Export_LayersAsPrefabs,
            this.Menu_File_Export_LayersAsPrefabsIntoFolder,
            this.Menu_File_Export_Separator,
            this.Menu_File_Export_BatchProject,
            this.Menu_File_Export_BatchDirectory});
      this.Menu_File_Export.Image = global::Editor.Properties.Resources.toolbar_export_quick;
      this.Menu_File_Export.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.Menu_File_Export.Name = "Menu_File_Export";
      this.Menu_File_Export.Size = new System.Drawing.Size(155, 22);
      this.Menu_File_Export.Text = "Export";
      this.Menu_File_Export.Click += new System.EventHandler(this.Menu_File_Export_Click);
      // 
      // Menu_File_Export_Scene
      // 
      this.Menu_File_Export_Scene.Image = global::Editor.Properties.Resources.toolbar_export_quick;
      this.Menu_File_Export_Scene.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.Menu_File_Export_Scene.Name = "Menu_File_Export_Scene";
      this.Menu_File_Export_Scene.Size = new System.Drawing.Size(259, 22);
      this.Menu_File_Export_Scene.Text = "Export Scene";
      this.Menu_File_Export_Scene.Click += new System.EventHandler(this.Menu_File_ExportScene_Click);
      // 
      // Menu_File_Export_LayersAsPrefabs
      // 
      this.Menu_File_Export_LayersAsPrefabs.Image = global::Editor.Properties.Resources.toolbar_export_quick;
      this.Menu_File_Export_LayersAsPrefabs.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.Menu_File_Export_LayersAsPrefabs.Name = "Menu_File_Export_LayersAsPrefabs";
      this.Menu_File_Export_LayersAsPrefabs.Size = new System.Drawing.Size(259, 22);
      this.Menu_File_Export_LayersAsPrefabs.Text = "Export Layers as Prefabs";
      this.Menu_File_Export_LayersAsPrefabs.Click += new System.EventHandler(this.Menu_File_ExportLayersAsPrefabs_Click);
      // 
      // Menu_File_Export_LayersAsPrefabsIntoFolder
      // 
      this.Menu_File_Export_LayersAsPrefabsIntoFolder.Image = global::Editor.Properties.Resources.toolbar_export_quick;
      this.Menu_File_Export_LayersAsPrefabsIntoFolder.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.Menu_File_Export_LayersAsPrefabsIntoFolder.Name = "Menu_File_Export_LayersAsPrefabsIntoFolder";
      this.Menu_File_Export_LayersAsPrefabsIntoFolder.Size = new System.Drawing.Size(259, 22);
      this.Menu_File_Export_LayersAsPrefabsIntoFolder.Text = "Export Layers as Prefabs into Folder";
      this.Menu_File_Export_LayersAsPrefabsIntoFolder.Click += new System.EventHandler(this.Menu_File_ExportLayersAsPrefabsIntoFolder_Click);
      // 
      // Menu_File_Export_Separator
      // 
      this.Menu_File_Export_Separator.Name = "Menu_File_Export_Separator";
      this.Menu_File_Export_Separator.Size = new System.Drawing.Size(256, 6);
      // 
      // Menu_File_Export_BatchProject
      // 
      this.Menu_File_Export_BatchProject.Image = global::Editor.Properties.Resources.toolbar_folder;
      this.Menu_File_Export_BatchProject.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.Menu_File_Export_BatchProject.Name = "Menu_File_Export_BatchProject";
      this.Menu_File_Export_BatchProject.Size = new System.Drawing.Size(259, 22);
      this.Menu_File_Export_BatchProject.Text = "Batch Export Project";
      this.Menu_File_Export_BatchProject.Click += new System.EventHandler(this.Menu_File_ExportBatchProject_Click);
      // 
      // Menu_File_Export_BatchDirectory
      // 
      this.Menu_File_Export_BatchDirectory.Image = global::Editor.Properties.Resources.toolbar_folder;
      this.Menu_File_Export_BatchDirectory.Name = "Menu_File_Export_BatchDirectory";
      this.Menu_File_Export_BatchDirectory.Size = new System.Drawing.Size(259, 22);
      this.Menu_File_Export_BatchDirectory.Text = "Batch Export Directory";
      this.Menu_File_Export_BatchDirectory.Click += new System.EventHandler(this.Menu_File_ExportBatchDirectory_Click);
      // 
      // toolStripMenuItem1
      // 
      this.toolStripMenuItem1.Name = "toolStripMenuItem1";
      this.toolStripMenuItem1.Size = new System.Drawing.Size(152, 6);
      // 
      // Menu_File_ProjectHelpers
      // 
      this.Menu_File_ProjectHelpers.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.Menu_File_Project_OpenDirectory,
            this.Menu_File_Project_Move});
      this.Menu_File_ProjectHelpers.Image = global::Editor.Properties.Resources.toolbar_folder;
      this.Menu_File_ProjectHelpers.Name = "Menu_File_ProjectHelpers";
      this.Menu_File_ProjectHelpers.Size = new System.Drawing.Size(155, 22);
      this.Menu_File_ProjectHelpers.Text = "Project Helpers";
      // 
      // Menu_File_Project_OpenDirectory
      // 
      this.Menu_File_Project_OpenDirectory.Image = global::Editor.Properties.Resources.toolbar_folder;
      this.Menu_File_Project_OpenDirectory.Name = "Menu_File_Project_OpenDirectory";
      this.Menu_File_Project_OpenDirectory.Size = new System.Drawing.Size(252, 22);
      this.Menu_File_Project_OpenDirectory.Text = "Open Project &Directory in Explorer";
      this.Menu_File_Project_OpenDirectory.Click += new System.EventHandler(this.menuItemOpenProjectDirectory_Click);
      // 
      // Menu_File_Project_Move
      // 
      this.Menu_File_Project_Move.Image = global::Editor.Properties.Resources.toolbar_folder_in;
      this.Menu_File_Project_Move.Name = "Menu_File_Project_Move";
      this.Menu_File_Project_Move.Size = new System.Drawing.Size(252, 22);
      this.Menu_File_Project_Move.Text = "Move Project";
      this.Menu_File_Project_Move.Click += new System.EventHandler(this.menuItem_MoveSceneProject_Click);
      // 
      // Menu_File_SceneHelpers
      // 
      this.Menu_File_SceneHelpers.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.Menu_File_Scene_Settings,
            this.Menu_File_Scene_Description,
            this.Menu_File_Scene_UniqueIDs,
            this.Menu_File_Scene_Thumbnail,
            this.Menu_File_Scene_ShapeLinkDebugFile,
            this.Menu_File_Scene_MergeExternalLayers});
      this.Menu_File_SceneHelpers.Image = global::Editor.Properties.Resources.toolbar_document;
      this.Menu_File_SceneHelpers.Name = "Menu_File_SceneHelpers";
      this.Menu_File_SceneHelpers.Size = new System.Drawing.Size(155, 22);
      this.Menu_File_SceneHelpers.Text = "Scene Helpers";
      // 
      // Menu_File_Scene_Settings
      // 
      this.Menu_File_Scene_Settings.Image = global::Editor.Properties.Resources.toolbar_document_settings;
      this.Menu_File_Scene_Settings.Name = "Menu_File_Scene_Settings";
      this.Menu_File_Scene_Settings.Size = new System.Drawing.Size(247, 22);
      this.Menu_File_Scene_Settings.Text = "Scene Settings";
      this.Menu_File_Scene_Settings.Click += new System.EventHandler(this.Menu_File_SceneSettings_Click);
      // 
      // Menu_File_Scene_Description
      // 
      this.Menu_File_Scene_Description.Image = global::Editor.Properties.Resources.toolbar_list;
      this.Menu_File_Scene_Description.Name = "Menu_File_Scene_Description";
      this.Menu_File_Scene_Description.Size = new System.Drawing.Size(247, 22);
      this.Menu_File_Scene_Description.Text = "Scene Description";
      this.Menu_File_Scene_Description.Click += new System.EventHandler(this.Menu_File_Scene_Description_Click);
      // 
      // Menu_File_Scene_UniqueIDs
      // 
      this.Menu_File_Scene_UniqueIDs.Image = global::Editor.Properties.Resources.toolbar_duplicates;
      this.Menu_File_Scene_UniqueIDs.Name = "Menu_File_Scene_UniqueIDs";
      this.Menu_File_Scene_UniqueIDs.Size = new System.Drawing.Size(247, 22);
      this.Menu_File_Scene_UniqueIDs.Text = "Validate Unique IDs";
      this.Menu_File_Scene_UniqueIDs.Click += new System.EventHandler(this.menuItem_ValidateUniqueIDs_Click);
      // 
      // Menu_File_Scene_Thumbnail
      // 
      this.Menu_File_Scene_Thumbnail.Image = global::Editor.Properties.Resources.toolbar_sky;
      this.Menu_File_Scene_Thumbnail.Name = "Menu_File_Scene_Thumbnail";
      this.Menu_File_Scene_Thumbnail.Size = new System.Drawing.Size(247, 22);
      this.Menu_File_Scene_Thumbnail.Text = "Save Scene Thumbnail";
      this.Menu_File_Scene_Thumbnail.Click += new System.EventHandler(this.Menu_File_SaveThumbnail_Click);
      // 
      // Menu_File_Scene_ShapeLinkDebugFile
      // 
      this.Menu_File_Scene_ShapeLinkDebugFile.Image = global::Editor.Properties.Resources.toolbar_document_links;
      this.Menu_File_Scene_ShapeLinkDebugFile.Name = "Menu_File_Scene_ShapeLinkDebugFile";
      this.Menu_File_Scene_ShapeLinkDebugFile.Size = new System.Drawing.Size(247, 22);
      this.Menu_File_Scene_ShapeLinkDebugFile.Text = "Save Shape Link Debug File";
      this.Menu_File_Scene_ShapeLinkDebugFile.Click += new System.EventHandler(this.menuItem_SaveShapeLinkFile_Click);
      // 
      // Menu_File_Scene_MergeExternalLayers
      // 
      this.Menu_File_Scene_MergeExternalLayers.Image = global::Editor.Properties.Resources.toolbar_layer_zones;
      this.Menu_File_Scene_MergeExternalLayers.Name = "Menu_File_Scene_MergeExternalLayers";
      this.Menu_File_Scene_MergeExternalLayers.Size = new System.Drawing.Size(247, 22);
      this.Menu_File_Scene_MergeExternalLayers.Text = "Merge External Layers into Zones";
      this.Menu_File_Scene_MergeExternalLayers.Click += new System.EventHandler(this.menuItem_MergeLayersIntoZones_Click);
      // 
      // toolStripMenuItem2
      // 
      this.toolStripMenuItem2.Name = "toolStripMenuItem2";
      this.toolStripMenuItem2.Size = new System.Drawing.Size(152, 6);
      // 
      // RecentProjects
      // 
      this.RecentProjects.Image = global::Editor.Properties.Resources.toolbar_recentproject;
      this.RecentProjects.Name = "RecentProjects";
      this.RecentProjects.Size = new System.Drawing.Size(155, 22);
      this.RecentProjects.Text = "Recent Projects";
      // 
      // RecentScenes
      // 
      this.RecentScenes.Image = global::Editor.Properties.Resources.toolbar_recentscenes;
      this.RecentScenes.Name = "RecentScenes";
      this.RecentScenes.Size = new System.Drawing.Size(155, 22);
      this.RecentScenes.Text = "Recent Scenes";
      // 
      // toolStripMenuItem3
      // 
      this.toolStripMenuItem3.Name = "toolStripMenuItem3";
      this.toolStripMenuItem3.Size = new System.Drawing.Size(152, 6);
      // 
      // Menu_File_CloseProject
      // 
      this.Menu_File_CloseProject.Image = global::Editor.Properties.Resources.toolbar_close;
      this.Menu_File_CloseProject.Name = "Menu_File_CloseProject";
      this.Menu_File_CloseProject.Size = new System.Drawing.Size(155, 22);
      this.Menu_File_CloseProject.Text = "&Close Project";
      this.Menu_File_CloseProject.Click += new System.EventHandler(this.Menu_File_CloseProject_Click);
      // 
      // Menu_File_StartPage
      // 
      this.Menu_File_StartPage.Image = global::Editor.Properties.Resources.toolbar_start;
      this.Menu_File_StartPage.Name = "Menu_File_StartPage";
      this.Menu_File_StartPage.Size = new System.Drawing.Size(155, 22);
      this.Menu_File_StartPage.Text = "Start &Page";
      this.Menu_File_StartPage.Click += new System.EventHandler(this.Menu_File_Start_Click);
      // 
      // Menu_File_Exit
      // 
      this.Menu_File_Exit.Image = global::Editor.Properties.Resources.toolbar_exit;
      this.Menu_File_Exit.Name = "Menu_File_Exit";
      this.Menu_File_Exit.Size = new System.Drawing.Size(155, 22);
      this.Menu_File_Exit.Text = "E&xit";
      this.Menu_File_Exit.Click += new System.EventHandler(this.Menu_File_Exit_Click);
      // 
      // Menu_Edit
      // 
      this.Menu_Edit.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.Menu_Edit_Undo,
            this.Menu_Edit_Redo,
            this.toolStripMenuItem4,
            this.Menu_Edit_Cut,
            this.Menu_Edit_Copy,
            this.Menu_Edit_Paste,
            this.Menu_Edit_Delete,
            this.toolStripMenuItem5,
            this.Menu_Edit_FindShapes,
            this.Menu_Edit_FindDuplicates});
      this.Menu_Edit.Name = "Menu_Edit";
      this.Menu_Edit.Size = new System.Drawing.Size(39, 20);
      this.Menu_Edit.Text = "&Edit";
      this.Menu_Edit.DropDownClosed += new System.EventHandler(this.Menu_DropDownClosed);
      this.Menu_Edit.DropDownOpening += new System.EventHandler(this.Menu_Edit_DropDownOpening);
      // 
      // Menu_Edit_Undo
      // 
      this.Menu_Edit_Undo.Image = global::Editor.Properties.Resources.toolbar_undo;
      this.Menu_Edit_Undo.Name = "Menu_Edit_Undo";
      this.Menu_Edit_Undo.Size = new System.Drawing.Size(155, 22);
      this.Menu_Edit_Undo.Text = "&Undo";
      this.Menu_Edit_Undo.Click += new System.EventHandler(this.Menu_Edit_Undo_Click);
      // 
      // Menu_Edit_Redo
      // 
      this.Menu_Edit_Redo.Image = global::Editor.Properties.Resources.toolbar_redo;
      this.Menu_Edit_Redo.Name = "Menu_Edit_Redo";
      this.Menu_Edit_Redo.Size = new System.Drawing.Size(155, 22);
      this.Menu_Edit_Redo.Text = "&Redo";
      this.Menu_Edit_Redo.Click += new System.EventHandler(this.Menu_Edit_Redo_Click);
      // 
      // toolStripMenuItem4
      // 
      this.toolStripMenuItem4.Name = "toolStripMenuItem4";
      this.toolStripMenuItem4.Size = new System.Drawing.Size(152, 6);
      // 
      // Menu_Edit_Cut
      // 
      this.Menu_Edit_Cut.Image = global::Editor.Properties.Resources.toolbar_cut;
      this.Menu_Edit_Cut.Name = "Menu_Edit_Cut";
      this.Menu_Edit_Cut.Size = new System.Drawing.Size(155, 22);
      this.Menu_Edit_Cut.Text = "Cu&t";
      this.Menu_Edit_Cut.Click += new System.EventHandler(this.Menu_Edit_Cut_Click);
      // 
      // Menu_Edit_Copy
      // 
      this.Menu_Edit_Copy.Image = global::Editor.Properties.Resources.toolbar_copy;
      this.Menu_Edit_Copy.Name = "Menu_Edit_Copy";
      this.Menu_Edit_Copy.Size = new System.Drawing.Size(155, 22);
      this.Menu_Edit_Copy.Text = "&Copy";
      this.Menu_Edit_Copy.Click += new System.EventHandler(this.Menu_Edit_Copy_Click);
      // 
      // Menu_Edit_Paste
      // 
      this.Menu_Edit_Paste.Image = global::Editor.Properties.Resources.toolbar_paste;
      this.Menu_Edit_Paste.Name = "Menu_Edit_Paste";
      this.Menu_Edit_Paste.Size = new System.Drawing.Size(155, 22);
      this.Menu_Edit_Paste.Text = "&Paste";
      this.Menu_Edit_Paste.Click += new System.EventHandler(this.Menu_Edit_Paste_Click);
      // 
      // Menu_Edit_Delete
      // 
      this.Menu_Edit_Delete.Image = global::Editor.Properties.Resources.toolbar_delete;
      this.Menu_Edit_Delete.Name = "Menu_Edit_Delete";
      this.Menu_Edit_Delete.Size = new System.Drawing.Size(155, 22);
      this.Menu_Edit_Delete.Text = "&Delete";
      this.Menu_Edit_Delete.Click += new System.EventHandler(this.Menu_Edit_Delete_Click);
      // 
      // toolStripMenuItem5
      // 
      this.toolStripMenuItem5.Name = "toolStripMenuItem5";
      this.toolStripMenuItem5.Size = new System.Drawing.Size(152, 6);
      // 
      // Menu_Edit_FindShapes
      // 
      this.Menu_Edit_FindShapes.Image = global::Editor.Properties.Resources.toolbar_search;
      this.Menu_Edit_FindShapes.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.Menu_Edit_FindShapes.Name = "Menu_Edit_FindShapes";
      this.Menu_Edit_FindShapes.Size = new System.Drawing.Size(155, 22);
      this.Menu_Edit_FindShapes.Text = "Find Shapes";
      this.Menu_Edit_FindShapes.Click += new System.EventHandler(this.Menu_Edit_FindShapes_Click);
      // 
      // Menu_Edit_FindDuplicates
      // 
      this.Menu_Edit_FindDuplicates.Image = global::Editor.Properties.Resources.toolbar_duplicates;
      this.Menu_Edit_FindDuplicates.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.Menu_Edit_FindDuplicates.Name = "Menu_Edit_FindDuplicates";
      this.Menu_Edit_FindDuplicates.Size = new System.Drawing.Size(155, 22);
      this.Menu_Edit_FindDuplicates.Text = "Find Duplicates";
      this.Menu_Edit_FindDuplicates.Click += new System.EventHandler(this.Menu_Edit_FindDuplicates_Click);
      // 
      // Menu_View
      // 
      this.Menu_View.Name = "Menu_View";
      this.Menu_View.Size = new System.Drawing.Size(44, 20);
      this.Menu_View.Text = "&View";
      // 
      // Menu_Create
      // 
      this.Menu_Create.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.Menu_Create_NewLayer,
            this.Menu_Create_NewZone,
            this.Menu_Create_ZoneGrid,
            this.Menu_Create_Separator,
            this.Menu_Create_Dummy});
      this.Menu_Create.Name = "Menu_Create";
      this.Menu_Create.Size = new System.Drawing.Size(53, 20);
      this.Menu_Create.Text = "&Create";
      this.Menu_Create.DropDownClosed += new System.EventHandler(this.Menu_DropDownClosed);
      this.Menu_Create.DropDownOpening += new System.EventHandler(this.Menu_Create_Popup);
      // 
      // Menu_Create_NewLayer
      // 
      this.Menu_Create_NewLayer.Name = "Menu_Create_NewLayer";
      this.Menu_Create_NewLayer.Size = new System.Drawing.Size(166, 22);
      this.Menu_Create_NewLayer.Text = "Create New Layer";
      this.Menu_Create_NewLayer.Click += new System.EventHandler(this.menu_CreateNewLayer_Click);
      // 
      // Menu_Create_NewZone
      // 
      this.Menu_Create_NewZone.Name = "Menu_Create_NewZone";
      this.Menu_Create_NewZone.Size = new System.Drawing.Size(166, 22);
      this.Menu_Create_NewZone.Text = "Create New Zone";
      this.Menu_Create_NewZone.Click += new System.EventHandler(this.menu_CreateNewZone_Click);
      // 
      // Menu_Create_ZoneGrid
      // 
      this.Menu_Create_ZoneGrid.Name = "Menu_Create_ZoneGrid";
      this.Menu_Create_ZoneGrid.Size = new System.Drawing.Size(166, 22);
      this.Menu_Create_ZoneGrid.Text = "Create Zone Grid";
      this.Menu_Create_ZoneGrid.Click += new System.EventHandler(this.menu_CreateZoneGrid_Click);
      // 
      // Menu_Create_Separator
      // 
      this.Menu_Create_Separator.Name = "Menu_Create_Separator";
      this.Menu_Create_Separator.Size = new System.Drawing.Size(163, 6);
      // 
      // Menu_Create_Dummy
      // 
      this.Menu_Create_Dummy.Name = "Menu_Create_Dummy";
      this.Menu_Create_Dummy.Size = new System.Drawing.Size(166, 22);
      this.Menu_Create_Dummy.Text = "Dummy";
      // 
      // Menu_Play
      // 
      this.Menu_Play.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.Menu_Play_Animate,
            this.Menu_Play_RunInEditor,
            this.Menu_Play_PlayTheGame,
            this.toolStripMenuItem7,
            this.Menu_Play_TimeScale});
      this.Menu_Play.Name = "Menu_Play";
      this.Menu_Play.Size = new System.Drawing.Size(41, 20);
      this.Menu_Play.Text = "&Play";
      this.Menu_Play.DropDownClosed += new System.EventHandler(this.Menu_DropDownClosed);
      this.Menu_Play.DropDownOpening += new System.EventHandler(this.Menu_Play_Popup);
      // 
      // Menu_Play_Animate
      // 
      this.Menu_Play_Animate.Image = global::Editor.Properties.Resources.toolbar_timeofday;
      this.Menu_Play_Animate.Name = "Menu_Play_Animate";
      this.Menu_Play_Animate.Size = new System.Drawing.Size(150, 22);
      this.Menu_Play_Animate.Text = "&Animate";
      this.Menu_Play_Animate.Click += new System.EventHandler(this.Menu_Play_Animate_Click);
      // 
      // Menu_Play_RunInEditor
      // 
      this.Menu_Play_RunInEditor.Image = global::Editor.Properties.Resources.toolbar_editor;
      this.Menu_Play_RunInEditor.Name = "Menu_Play_RunInEditor";
      this.Menu_Play_RunInEditor.Size = new System.Drawing.Size(150, 22);
      this.Menu_Play_RunInEditor.Text = "&Run in Editor";
      this.Menu_Play_RunInEditor.Click += new System.EventHandler(this.Menu_Play_RunInEditor_Click);
      // 
      // Menu_Play_PlayTheGame
      // 
      this.Menu_Play_PlayTheGame.Image = global::Editor.Properties.Resources.toolbar_gamepad;
      this.Menu_Play_PlayTheGame.Name = "Menu_Play_PlayTheGame";
      this.Menu_Play_PlayTheGame.Size = new System.Drawing.Size(150, 22);
      this.Menu_Play_PlayTheGame.Text = "&Play the Game";
      this.Menu_Play_PlayTheGame.Click += new System.EventHandler(this.Menu_Play_PlayTheGame_Click);
      // 
      // toolStripMenuItem7
      // 
      this.toolStripMenuItem7.Name = "toolStripMenuItem7";
      this.toolStripMenuItem7.Size = new System.Drawing.Size(147, 6);
      // 
      // Menu_Play_TimeScale
      // 
      this.Menu_Play_TimeScale.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.Menu_Play_TS_100,
            this.toolStripMenuItem8,
            this.Menu_Play_TS_10,
            this.Menu_Play_TS_25,
            this.Menu_Play_TS_50,
            this.Menu_Play_TS_200,
            this.Menu_Play_TS_500});
      this.Menu_Play_TimeScale.Name = "Menu_Play_TimeScale";
      this.Menu_Play_TimeScale.Size = new System.Drawing.Size(150, 22);
      this.Menu_Play_TimeScale.Text = "Time Scale";
      this.Menu_Play_TimeScale.DropDownOpening += new System.EventHandler(this.Menu_Play_TimeScale_Popup);
      // 
      // Menu_Play_TS_100
      // 
      this.Menu_Play_TS_100.Name = "Menu_Play_TS_100";
      this.Menu_Play_TS_100.Size = new System.Drawing.Size(165, 22);
      this.Menu_Play_TS_100.Text = "Real Time (100%)";
      this.Menu_Play_TS_100.Click += new System.EventHandler(this.Menu_Play_TS_100_Click);
      // 
      // toolStripMenuItem8
      // 
      this.toolStripMenuItem8.Name = "toolStripMenuItem8";
      this.toolStripMenuItem8.Size = new System.Drawing.Size(162, 6);
      // 
      // Menu_Play_TS_10
      // 
      this.Menu_Play_TS_10.Name = "Menu_Play_TS_10";
      this.Menu_Play_TS_10.Size = new System.Drawing.Size(165, 22);
      this.Menu_Play_TS_10.Text = "10%";
      this.Menu_Play_TS_10.Click += new System.EventHandler(this.Menu_Play_TS_10_Click);
      // 
      // Menu_Play_TS_25
      // 
      this.Menu_Play_TS_25.Name = "Menu_Play_TS_25";
      this.Menu_Play_TS_25.Size = new System.Drawing.Size(165, 22);
      this.Menu_Play_TS_25.Text = "25%";
      this.Menu_Play_TS_25.Click += new System.EventHandler(this.Menu_Play_TS_25_Click);
      // 
      // Menu_Play_TS_50
      // 
      this.Menu_Play_TS_50.Name = "Menu_Play_TS_50";
      this.Menu_Play_TS_50.Size = new System.Drawing.Size(165, 22);
      this.Menu_Play_TS_50.Text = "50%";
      this.Menu_Play_TS_50.Click += new System.EventHandler(this.Menu_Play_TS_50_Click);
      // 
      // Menu_Play_TS_200
      // 
      this.Menu_Play_TS_200.Name = "Menu_Play_TS_200";
      this.Menu_Play_TS_200.Size = new System.Drawing.Size(165, 22);
      this.Menu_Play_TS_200.Text = "200%";
      this.Menu_Play_TS_200.Click += new System.EventHandler(this.Menu_Play_TS_200_Click);
      // 
      // Menu_Play_TS_500
      // 
      this.Menu_Play_TS_500.Name = "Menu_Play_TS_500";
      this.Menu_Play_TS_500.Size = new System.Drawing.Size(165, 22);
      this.Menu_Play_TS_500.Text = "500%";
      this.Menu_Play_TS_500.Click += new System.EventHandler(this.Menu_Play_TS_500_Click);
      // 
      // Menu_Engine
      // 
      this.Menu_Engine.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.Menu_Engine_EditSky,
            this.Menu_Engine_EditTimeOfDay,
            this.Menu_Engine_EditFog,
            this.Menu_Engine_EditPostProcessors,
            this.Menu_Engine_ProfileSettings,
            this.Menu_Engine_ManifestSettings,
            this.toolStripMenuItem9,
            this.Menu_Engine_CalculateVisibility,
            this.Menu_Engine_UseVisibility,
            this.toolStripMenuItem15,
            this.Menu_Engine_ReloadResources,
            this.Menu_Engine_ShowResources,
            this.Menu_Engine_PurgeResources,
            this.toolStripMenuItem16,
            this.Menu_Engine_SaveScreenshot,
            this.Menu_Engine_SaveScreenshotAs});
      this.Menu_Engine.Name = "Menu_Engine";
      this.Menu_Engine.Size = new System.Drawing.Size(55, 20);
      this.Menu_Engine.Text = "E&ngine";
      this.Menu_Engine.DropDownClosed += new System.EventHandler(this.Menu_DropDownClosed);
      this.Menu_Engine.DropDownOpening += new System.EventHandler(this.Menu_Engine_Popup);
      // 
      // Menu_Engine_EditSky
      // 
      this.Menu_Engine_EditSky.Image = global::Editor.Properties.Resources.toolbar_sky;
      this.Menu_Engine_EditSky.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.Menu_Engine_EditSky.Name = "Menu_Engine_EditSky";
      this.Menu_Engine_EditSky.Size = new System.Drawing.Size(192, 22);
      this.Menu_Engine_EditSky.Text = "Edit Sky";
      this.Menu_Engine_EditSky.Click += new System.EventHandler(this.Menu_Engine_EditSky_Click);
      // 
      // Menu_Engine_EditTimeOfDay
      // 
      this.Menu_Engine_EditTimeOfDay.Image = global::Editor.Properties.Resources.toolbar_tod;
      this.Menu_Engine_EditTimeOfDay.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.Menu_Engine_EditTimeOfDay.Name = "Menu_Engine_EditTimeOfDay";
      this.Menu_Engine_EditTimeOfDay.Size = new System.Drawing.Size(192, 22);
      this.Menu_Engine_EditTimeOfDay.Text = "Edit Time of Day";
      this.Menu_Engine_EditTimeOfDay.Click += new System.EventHandler(this.Menu_Engine_EditTimeOfDay_Click);
      // 
      // Menu_Engine_EditFog
      // 
      this.Menu_Engine_EditFog.Image = global::Editor.Properties.Resources.toolbar_fog;
      this.Menu_Engine_EditFog.Name = "Menu_Engine_EditFog";
      this.Menu_Engine_EditFog.Size = new System.Drawing.Size(192, 22);
      this.Menu_Engine_EditFog.Text = "Edit Fog";
      this.Menu_Engine_EditFog.Click += new System.EventHandler(this.Menu_Engine_EditFog_Click);
      // 
      // Menu_Engine_EditPostProcessors
      // 
      this.Menu_Engine_EditPostProcessors.Image = global::Editor.Properties.Resources.toolbar_post;
      this.Menu_Engine_EditPostProcessors.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.Menu_Engine_EditPostProcessors.Name = "Menu_Engine_EditPostProcessors";
      this.Menu_Engine_EditPostProcessors.Size = new System.Drawing.Size(192, 22);
      this.Menu_Engine_EditPostProcessors.Text = "Post Processors";
      this.Menu_Engine_EditPostProcessors.Click += new System.EventHandler(this.Menu_Engine_EditPostProcessors_Click);
      // 
      // Menu_Engine_ProfileSettings
      // 
      this.Menu_Engine_ProfileSettings.Image = global::Editor.Properties.Resources.asset_profile_manager_16x16;
      this.Menu_Engine_ProfileSettings.Name = "Menu_Engine_ProfileSettings";
      this.Menu_Engine_ProfileSettings.Size = new System.Drawing.Size(192, 22);
      this.Menu_Engine_ProfileSettings.Text = "Asset Profile Manager";
      this.Menu_Engine_ProfileSettings.Click += new System.EventHandler(this.Menu_Engine_ProfileSettings_Click);
      // 
      // Menu_Engine_ManifestSettings
      // 
      this.Menu_Engine_ManifestSettings.Image = global::Editor.Properties.Resources.manifest_settings_16x16;
      this.Menu_Engine_ManifestSettings.Name = "Menu_Engine_ManifestSettings";
      this.Menu_Engine_ManifestSettings.Size = new System.Drawing.Size(192, 22);
      this.Menu_Engine_ManifestSettings.Text = "Manifest Settings";
      this.Menu_Engine_ManifestSettings.Click += new System.EventHandler(this.Menu_Engine_ManifestSettings_Click);
      // 
      // toolStripMenuItem9
      // 
      this.toolStripMenuItem9.Name = "toolStripMenuItem9";
      this.toolStripMenuItem9.Size = new System.Drawing.Size(189, 6);
      // 
      // Menu_Engine_CalculateVisibility
      // 
      this.Menu_Engine_CalculateVisibility.Image = global::Editor.Properties.Resources.toolbar_add;
      this.Menu_Engine_CalculateVisibility.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.Menu_Engine_CalculateVisibility.Name = "Menu_Engine_CalculateVisibility";
      this.Menu_Engine_CalculateVisibility.Size = new System.Drawing.Size(192, 22);
      this.Menu_Engine_CalculateVisibility.Text = "Calculate Visibility";
      this.Menu_Engine_CalculateVisibility.Click += new System.EventHandler(this.menu_CalculateVisibility_Click);
      // 
      // Menu_Engine_UseVisibility
      // 
      this.Menu_Engine_UseVisibility.Image = global::Editor.Properties.Resources.toolbar_check;
      this.Menu_Engine_UseVisibility.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.Menu_Engine_UseVisibility.Name = "Menu_Engine_UseVisibility";
      this.Menu_Engine_UseVisibility.Size = new System.Drawing.Size(192, 22);
      this.Menu_Engine_UseVisibility.Text = "Use Visibility in Engine";
      this.Menu_Engine_UseVisibility.Click += new System.EventHandler(this.menu_UseVisibility_Click);
      // 
      // toolStripMenuItem15
      // 
      this.toolStripMenuItem15.Name = "toolStripMenuItem15";
      this.toolStripMenuItem15.Size = new System.Drawing.Size(189, 6);
      // 
      // Menu_Engine_ReloadResources
      // 
      this.Menu_Engine_ReloadResources.Image = global::Editor.Properties.Resources.toolbar_refresh;
      this.Menu_Engine_ReloadResources.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.Menu_Engine_ReloadResources.Name = "Menu_Engine_ReloadResources";
      this.Menu_Engine_ReloadResources.Size = new System.Drawing.Size(192, 22);
      this.Menu_Engine_ReloadResources.Text = "Reload Resources";
      this.Menu_Engine_ReloadResources.Click += new System.EventHandler(this.Menu_Engine_ReloadResources_Click);
      // 
      // Menu_Engine_ShowResources
      // 
      this.Menu_Engine_ShowResources.Image = global::Editor.Properties.Resources.toolbar_resources_small;
      this.Menu_Engine_ShowResources.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.Menu_Engine_ShowResources.Name = "Menu_Engine_ShowResources";
      this.Menu_Engine_ShowResources.Size = new System.Drawing.Size(192, 22);
      this.Menu_Engine_ShowResources.Text = "Show Resources";
      this.Menu_Engine_ShowResources.Click += new System.EventHandler(this.Menu_Engine_ShowResources_Click);
      // 
      // Menu_Engine_PurgeResources
      // 
      this.Menu_Engine_PurgeResources.Image = global::Editor.Properties.Resources.toolbar_remove;
      this.Menu_Engine_PurgeResources.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.Menu_Engine_PurgeResources.Name = "Menu_Engine_PurgeResources";
      this.Menu_Engine_PurgeResources.Size = new System.Drawing.Size(192, 22);
      this.Menu_Engine_PurgeResources.Text = "Purge All Resources";
      this.Menu_Engine_PurgeResources.Click += new System.EventHandler(this.Menu_Engine_PurgeResources_Click);
      // 
      // toolStripMenuItem16
      // 
      this.toolStripMenuItem16.Name = "toolStripMenuItem16";
      this.toolStripMenuItem16.Size = new System.Drawing.Size(189, 6);
      // 
      // Menu_Engine_SaveScreenshot
      // 
      this.Menu_Engine_SaveScreenshot.Image = global::Editor.Properties.Resources.toolbar_screenshot;
      this.Menu_Engine_SaveScreenshot.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.Menu_Engine_SaveScreenshot.Name = "Menu_Engine_SaveScreenshot";
      this.Menu_Engine_SaveScreenshot.Size = new System.Drawing.Size(192, 22);
      this.Menu_Engine_SaveScreenshot.Text = "Save Screenshot";
      this.Menu_Engine_SaveScreenshot.Click += new System.EventHandler(this.Menu_Engine_Screenshot_Click);
      // 
      // Menu_Engine_SaveScreenshotAs
      // 
      this.Menu_Engine_SaveScreenshotAs.Image = global::Editor.Properties.Resources.toolbar_screenshot;
      this.Menu_Engine_SaveScreenshotAs.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.Menu_Engine_SaveScreenshotAs.Name = "Menu_Engine_SaveScreenshotAs";
      this.Menu_Engine_SaveScreenshotAs.Size = new System.Drawing.Size(192, 22);
      this.Menu_Engine_SaveScreenshotAs.Text = "Save Screenshot As";
      this.Menu_Engine_SaveScreenshotAs.Click += new System.EventHandler(this.Menu_Engine_ScreenshotAs_Click);
      // 
      // Menu_StaticLighting
      // 
      this.Menu_StaticLighting.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.Menu_StaticLighting_ActiveTool,
            this.Menu_StaticLighting_EditSettings,
            this.toolStripMenuItem10,
            this.Menu_StaticLighting_CalculateLighting,
            this.Menu_StaticLighting_ResetLighting});
      this.Menu_StaticLighting.Name = "Menu_StaticLighting";
      this.Menu_StaticLighting.Size = new System.Drawing.Size(63, 20);
      this.Menu_StaticLighting.Text = "&Lighting";
      this.Menu_StaticLighting.DropDownClosed += new System.EventHandler(this.Menu_DropDownClosed);
      this.Menu_StaticLighting.DropDownOpening += new System.EventHandler(this.Menu_Tools_Popup);
      // 
      // Menu_StaticLighting_ActiveTool
      // 
      this.Menu_StaticLighting_ActiveTool.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.Menu_StaticLighting_ActiveTool_Dummy});
      this.Menu_StaticLighting_ActiveTool.Image = global::Editor.Properties.Resources.toolbar_light_calc;
      this.Menu_StaticLighting_ActiveTool.Name = "Menu_StaticLighting_ActiveTool";
      this.Menu_StaticLighting_ActiveTool.Size = new System.Drawing.Size(181, 22);
      this.Menu_StaticLighting_ActiveTool.Text = "Active Lighting Tool";
      this.Menu_StaticLighting_ActiveTool.DropDownOpening += new System.EventHandler(this.Menu_Lighting_ActiveTool_Popup);
      // 
      // Menu_StaticLighting_ActiveTool_Dummy
      // 
      this.Menu_StaticLighting_ActiveTool_Dummy.Name = "Menu_StaticLighting_ActiveTool_Dummy";
      this.Menu_StaticLighting_ActiveTool_Dummy.Size = new System.Drawing.Size(117, 22);
      this.Menu_StaticLighting_ActiveTool_Dummy.Text = "Dummy";
      // 
      // Menu_StaticLighting_EditSettings
      // 
      this.Menu_StaticLighting_EditSettings.Image = global::Editor.Properties.Resources.toolbar_export_settings;
      this.Menu_StaticLighting_EditSettings.Name = "Menu_StaticLighting_EditSettings";
      this.Menu_StaticLighting_EditSettings.Size = new System.Drawing.Size(181, 22);
      this.Menu_StaticLighting_EditSettings.Text = "Edit Settings";
      this.Menu_StaticLighting_EditSettings.Click += new System.EventHandler(this.Menu_Engine_UpdateLighting_Click);
      // 
      // toolStripMenuItem10
      // 
      this.toolStripMenuItem10.Name = "toolStripMenuItem10";
      this.toolStripMenuItem10.Size = new System.Drawing.Size(178, 6);
      // 
      // Menu_StaticLighting_CalculateLighting
      // 
      this.Menu_StaticLighting_CalculateLighting.Image = global::Editor.Properties.Resources.toolbar_light_calc;
      this.Menu_StaticLighting_CalculateLighting.Name = "Menu_StaticLighting_CalculateLighting";
      this.Menu_StaticLighting_CalculateLighting.Size = new System.Drawing.Size(181, 22);
      this.Menu_StaticLighting_CalculateLighting.Text = "Calculate Lighting";
      this.Menu_StaticLighting_CalculateLighting.Click += new System.EventHandler(this.Menu_Tools_CalculateLighting_Click);
      // 
      // Menu_StaticLighting_ResetLighting
      // 
      this.Menu_StaticLighting_ResetLighting.Image = global::Editor.Properties.Resources.toolbar_light_reset;
      this.Menu_StaticLighting_ResetLighting.Name = "Menu_StaticLighting_ResetLighting";
      this.Menu_StaticLighting_ResetLighting.Size = new System.Drawing.Size(181, 22);
      this.Menu_StaticLighting_ResetLighting.Text = "Reset Lighting";
      this.Menu_StaticLighting_ResetLighting.Click += new System.EventHandler(this.Menu_Tools_ResetLighting_Click);
      // 
      // Menu_Options
      // 
      this.Menu_Options.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.Menu_Options_Settings,
            this.Menu_Options_Hotkeys,
            this.toolStripMenuItem17,
            this.Menu_Options_ShowPlugins,
            this.toolStripMenuItem18});
      this.Menu_Options.Name = "Menu_Options";
      this.Menu_Options.Size = new System.Drawing.Size(49, 20);
      this.Menu_Options.Text = "E&xtras";
      this.Menu_Options.DropDownClosed += new System.EventHandler(this.Menu_DropDownClosed);
      this.Menu_Options.DropDownOpening += new System.EventHandler(this.menu_Extras_Popup);
      // 
      // Menu_Options_Settings
      // 
      this.Menu_Options_Settings.Image = global::Editor.Properties.Resources.toolbar_settings;
      this.Menu_Options_Settings.Name = "Menu_Options_Settings";
      this.Menu_Options_Settings.Size = new System.Drawing.Size(145, 22);
      this.Menu_Options_Settings.Text = "Settings";
      this.Menu_Options_Settings.Click += new System.EventHandler(this.menu_Extras_Settings_Click);
      // 
      // Menu_Options_Hotkeys
      // 
      this.Menu_Options_Hotkeys.Image = global::Editor.Properties.Resources.toolbar_hotkeys;
      this.Menu_Options_Hotkeys.Name = "Menu_Options_Hotkeys";
      this.Menu_Options_Hotkeys.Size = new System.Drawing.Size(145, 22);
      this.Menu_Options_Hotkeys.Text = "Hotkeys";
      this.Menu_Options_Hotkeys.Click += new System.EventHandler(this.menu_Extras_Hotkeys_Click);
      // 
      // toolStripMenuItem17
      // 
      this.toolStripMenuItem17.Name = "toolStripMenuItem17";
      this.toolStripMenuItem17.Size = new System.Drawing.Size(142, 6);
      // 
      // Menu_Options_ShowPlugins
      // 
      this.Menu_Options_ShowPlugins.Image = global::Editor.Properties.Resources.toolbar_plugins;
      this.Menu_Options_ShowPlugins.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.Menu_Options_ShowPlugins.Name = "Menu_Options_ShowPlugins";
      this.Menu_Options_ShowPlugins.Size = new System.Drawing.Size(145, 22);
      this.Menu_Options_ShowPlugins.Text = "Show Plugins";
      this.Menu_Options_ShowPlugins.Click += new System.EventHandler(this.Menu_Engine_ShowPlugins_Click);
      // 
      // toolStripMenuItem18
      // 
      this.toolStripMenuItem18.Name = "toolStripMenuItem18";
      this.toolStripMenuItem18.Size = new System.Drawing.Size(142, 6);
      // 
      // Menu_Remote
      // 
      this.Menu_Remote.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.Menu_Remote_XBOX360_Connect,
            this.Menu_Remote_XBOX360_Launch,
            this.Menu_Remote_XBOX360_Animate,
            this.Menu_Remote_XBOX360_PlayTheGame,
            this.Menu_Remote_XBOX360_Stop,
            this.Menu_Remote_XBOX360_Seperator,
            this.Menu_Remote_PS3_Connect,
            this.Menu_Remote_PS3_Animate,
            this.Menu_Remote_PS3_PlayTheGame,
            this.Menu_Remote_PS3_Stop,
            this.Menu_Remote_PS3_Seperator,
            this.Menu_Remote_ExportAndRun,
            this.Menu_Remote_Restart,
            this.Menu_Remote_ReloadResources});
      this.Menu_Remote.Name = "Menu_Remote";
      this.Menu_Remote.Size = new System.Drawing.Size(60, 20);
      this.Menu_Remote.Text = "&Remote";
      this.Menu_Remote.DropDownClosed += new System.EventHandler(this.Menu_DropDownClosed);
      this.Menu_Remote.DropDownOpening += new System.EventHandler(this.menu_Remote_Popup);
      // 
      // Menu_Remote_XBOX360_Connect
      // 
      this.Menu_Remote_XBOX360_Connect.Name = "Menu_Remote_XBOX360_Connect";
      this.Menu_Remote_XBOX360_Connect.Size = new System.Drawing.Size(217, 22);
      this.Menu_Remote_XBOX360_Connect.Text = "Connect XBOX 360";
      this.Menu_Remote_XBOX360_Connect.Click += new System.EventHandler(this.Menu_Remote_Xbox360_Click);
      // 
      // Menu_Remote_XBOX360_Launch
      // 
      this.Menu_Remote_XBOX360_Launch.Enabled = false;
      this.Menu_Remote_XBOX360_Launch.Name = "Menu_Remote_XBOX360_Launch";
      this.Menu_Remote_XBOX360_Launch.Size = new System.Drawing.Size(217, 22);
      this.Menu_Remote_XBOX360_Launch.Text = "Launch XBOX 360 Client";
      this.Menu_Remote_XBOX360_Launch.Click += new System.EventHandler(this.Menu_Remote_Xbox360_Launch_Click);
      // 
      // Menu_Remote_XBOX360_Animate
      // 
      this.Menu_Remote_XBOX360_Animate.Name = "Menu_Remote_XBOX360_Animate";
      this.Menu_Remote_XBOX360_Animate.Size = new System.Drawing.Size(217, 22);
      this.Menu_Remote_XBOX360_Animate.Text = "Animate";
      this.Menu_Remote_XBOX360_Animate.Click += new System.EventHandler(this.Menu_Remote_Xbox360_Animate_Click);
      // 
      // Menu_Remote_XBOX360_PlayTheGame
      // 
      this.Menu_Remote_XBOX360_PlayTheGame.Name = "Menu_Remote_XBOX360_PlayTheGame";
      this.Menu_Remote_XBOX360_PlayTheGame.Size = new System.Drawing.Size(217, 22);
      this.Menu_Remote_XBOX360_PlayTheGame.Text = "Play The Game";
      this.Menu_Remote_XBOX360_PlayTheGame.Click += new System.EventHandler(this.Menu_Remote_Xbox360_PlayTheGame_Click);
      // 
      // Menu_Remote_XBOX360_Stop
      // 
      this.Menu_Remote_XBOX360_Stop.Name = "Menu_Remote_XBOX360_Stop";
      this.Menu_Remote_XBOX360_Stop.Size = new System.Drawing.Size(217, 22);
      this.Menu_Remote_XBOX360_Stop.Text = "Stop Playing";
      this.Menu_Remote_XBOX360_Stop.Click += new System.EventHandler(this.Menu_Remote_Xbox360_StopPlaying_Click);
      // 
      // Menu_Remote_XBOX360_Seperator
      // 
      this.Menu_Remote_XBOX360_Seperator.Name = "Menu_Remote_XBOX360_Seperator";
      this.Menu_Remote_XBOX360_Seperator.Size = new System.Drawing.Size(214, 6);
      // 
      // Menu_Remote_PS3_Connect
      // 
      this.Menu_Remote_PS3_Connect.Name = "Menu_Remote_PS3_Connect";
      this.Menu_Remote_PS3_Connect.Size = new System.Drawing.Size(217, 22);
      this.Menu_Remote_PS3_Connect.Text = "Connect PS3";
      this.Menu_Remote_PS3_Connect.Click += new System.EventHandler(this.Menu_Remote_PS3_Click);
      // 
      // Menu_Remote_PS3_Animate
      // 
      this.Menu_Remote_PS3_Animate.Name = "Menu_Remote_PS3_Animate";
      this.Menu_Remote_PS3_Animate.Size = new System.Drawing.Size(217, 22);
      this.Menu_Remote_PS3_Animate.Text = "Animate";
      this.Menu_Remote_PS3_Animate.Click += new System.EventHandler(this.Menu_Remote_PS3_Animate_Click);
      // 
      // Menu_Remote_PS3_PlayTheGame
      // 
      this.Menu_Remote_PS3_PlayTheGame.Name = "Menu_Remote_PS3_PlayTheGame";
      this.Menu_Remote_PS3_PlayTheGame.Size = new System.Drawing.Size(217, 22);
      this.Menu_Remote_PS3_PlayTheGame.Text = "Play The Game";
      this.Menu_Remote_PS3_PlayTheGame.Click += new System.EventHandler(this.Menu_Remote_PS3_PlayTheGame_Click);
      // 
      // Menu_Remote_PS3_Stop
      // 
      this.Menu_Remote_PS3_Stop.Name = "Menu_Remote_PS3_Stop";
      this.Menu_Remote_PS3_Stop.Size = new System.Drawing.Size(217, 22);
      this.Menu_Remote_PS3_Stop.Text = "Stop Playing";
      this.Menu_Remote_PS3_Stop.Click += new System.EventHandler(this.Menu_Remote_PS3_StopPlaying_Click);
      // 
      // Menu_Remote_PS3_Seperator
      // 
      this.Menu_Remote_PS3_Seperator.Name = "Menu_Remote_PS3_Seperator";
      this.Menu_Remote_PS3_Seperator.Size = new System.Drawing.Size(214, 6);
      // 
      // Menu_Remote_ExportAndRun
      // 
      this.Menu_Remote_ExportAndRun.Enabled = false;
      this.Menu_Remote_ExportAndRun.Name = "Menu_Remote_ExportAndRun";
      this.Menu_Remote_ExportAndRun.Size = new System.Drawing.Size(217, 22);
      this.Menu_Remote_ExportAndRun.Text = "Export and Run Scene";
      this.Menu_Remote_ExportAndRun.Click += new System.EventHandler(this.Menu_Remote_ExportRun_Click);
      // 
      // Menu_Remote_Restart
      // 
      this.Menu_Remote_Restart.Enabled = false;
      this.Menu_Remote_Restart.Name = "Menu_Remote_Restart";
      this.Menu_Remote_Restart.Size = new System.Drawing.Size(217, 22);
      this.Menu_Remote_Restart.Text = "Restart Exported Scene";
      this.Menu_Remote_Restart.Click += new System.EventHandler(this.Menu_Remote_Restart_Click);
      // 
      // Menu_Remote_ReloadResources
      // 
      this.Menu_Remote_ReloadResources.Enabled = false;
      this.Menu_Remote_ReloadResources.Name = "Menu_Remote_ReloadResources";
      this.Menu_Remote_ReloadResources.Size = new System.Drawing.Size(217, 22);
      this.Menu_Remote_ReloadResources.Text = "Reload Modified Resources";
      this.Menu_Remote_ReloadResources.Click += new System.EventHandler(this.Menu_Remote_ReloadResources_Click);
      // 
      // Menu_Help
      // 
      this.Menu_Help.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.Menu_Help_iHelp,
            this.Menu_Help_VisionDoc,
            this.toolStripMenuItem21,
            this.Menu_Help_About});
      this.Menu_Help.Name = "Menu_Help";
      this.Menu_Help.Size = new System.Drawing.Size(44, 20);
      this.Menu_Help.Text = "&Help";
      // 
      // Menu_Help_iHelp
      // 
      this.Menu_Help_iHelp.Enabled = false;
      this.Menu_Help_iHelp.Name = "Menu_Help_iHelp";
      this.Menu_Help_iHelp.Size = new System.Drawing.Size(192, 22);
      this.Menu_Help_iHelp.Text = "iHelp ";
      this.Menu_Help_iHelp.Click += new System.EventHandler(this.Menu_Help_iHelp_Click);
      // 
      // Menu_Help_VisionDoc
      // 
      this.Menu_Help_VisionDoc.Enabled = false;
      this.Menu_Help_VisionDoc.Name = "Menu_Help_VisionDoc";
      this.Menu_Help_VisionDoc.Size = new System.Drawing.Size(192, 22);
      this.Menu_Help_VisionDoc.Text = "Vision Documentation";
      this.Menu_Help_VisionDoc.Visible = false;
      // 
      // toolStripMenuItem21
      // 
      this.toolStripMenuItem21.Name = "toolStripMenuItem21";
      this.toolStripMenuItem21.Size = new System.Drawing.Size(189, 6);
      // 
      // Menu_Help_About
      // 
      this.Menu_Help_About.Name = "Menu_Help_About";
      this.Menu_Help_About.Size = new System.Drawing.Size(192, 22);
      this.Menu_Help_About.Text = "About";
      this.Menu_Help_About.Click += new System.EventHandler(this.Menu_Help_About_Click);
      // 
      // Menu_Tests
      // 
      this.Menu_Tests.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.Menu_Tests_RunTests,
            this.Menu_Tests_VideoSize});
      this.Menu_Tests.Name = "Menu_Tests";
      this.Menu_Tests.Size = new System.Drawing.Size(46, 20);
      this.Menu_Tests.Text = "&Tests";
      // 
      // Menu_Tests_RunTests
      // 
      this.Menu_Tests_RunTests.Name = "Menu_Tests_RunTests";
      this.Menu_Tests_RunTests.Size = new System.Drawing.Size(146, 22);
      this.Menu_Tests_RunTests.Text = "Run Tests";
      this.Menu_Tests_RunTests.Click += new System.EventHandler(this.menu_Tests_RunTests_Click);
      // 
      // Menu_Tests_VideoSize
      // 
      this.Menu_Tests_VideoSize.Name = "Menu_Tests_VideoSize";
      this.Menu_Tests_VideoSize.Size = new System.Drawing.Size(146, 22);
      this.Menu_Tests_VideoSize.Text = "Set Video Size";
      this.Menu_Tests_VideoSize.Click += new System.EventHandler(this.menu_Tests_VideoSize_Click);
      // 
      // mainToolBar
      // 
      this.mainToolBar.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
      this.mainToolBar.ImageScalingSize = new System.Drawing.Size(24, 24);
      this.mainToolBar.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolBar_Preferences,
            this.ToolBar_Hotkeys,
            this.ToolBar_Sep1,
            this.ToolBar_New,
            this.ToolBar_Open,
            this.ToolBar_Save,
            this.ToolBar_Export,
            this.ToolBar_Sep2,
            this.Toolbar_Cut,
            this.Toolbar_Copy,
            this.Toolbar_Paste,
            this.Toolbar_Delete,
            this.ToolBar_Sep3,
            this.Toolbar_Undo,
            this.Toolbar_Redo,
            this.ToolBar_Sep4,
            this.ToolBar_Panels,
            this.ToolBar_OpenAssetBrowser,
            this.ToolBar_Tools,
            this.ToolBar_Resources,
            this.ToolBar_Relight,
            this.ToolBar_Visibility,
            this.ToolBar_Sep5});
      this.mainToolBar.Location = new System.Drawing.Point(0, 24);
      this.mainToolBar.Name = "mainToolBar";
      this.mainToolBar.Size = new System.Drawing.Size(1028, 31);
      this.mainToolBar.TabIndex = 1;
      // 
      // ToolBar_Preferences
      // 
      this.ToolBar_Preferences.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolBar_Preferences.Image = global::Editor.Properties.Resources.toolbar_settings;
      this.ToolBar_Preferences.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolBar_Preferences.Name = "ToolBar_Preferences";
      this.ToolBar_Preferences.Size = new System.Drawing.Size(28, 28);
      this.ToolBar_Preferences.Text = "Settings";
      this.ToolBar_Preferences.Click += new System.EventHandler(this.menu_Extras_Settings_Click);
      // 
      // ToolBar_Hotkeys
      // 
      this.ToolBar_Hotkeys.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolBar_Hotkeys.Image = global::Editor.Properties.Resources.toolbar_hotkeys;
      this.ToolBar_Hotkeys.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolBar_Hotkeys.Name = "ToolBar_Hotkeys";
      this.ToolBar_Hotkeys.Size = new System.Drawing.Size(28, 28);
      this.ToolBar_Hotkeys.Text = "Hotkeys";
      this.ToolBar_Hotkeys.Click += new System.EventHandler(this.menu_Extras_Hotkeys_Click);
      // 
      // ToolBar_Sep1
      // 
      this.ToolBar_Sep1.Name = "ToolBar_Sep1";
      this.ToolBar_Sep1.Size = new System.Drawing.Size(6, 31);
      // 
      // ToolBar_New
      // 
      this.ToolBar_New.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolBar_New.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolBar_NewProject,
            this.ToolBar_NewScene});
      this.ToolBar_New.Image = global::Editor.Properties.Resources.toolbar_new;
      this.ToolBar_New.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolBar_New.Name = "ToolBar_New";
      this.ToolBar_New.Size = new System.Drawing.Size(40, 28);
      this.ToolBar_New.Text = "New";
      this.ToolBar_New.ToolTipText = "Create New Project or New Scene";
      this.ToolBar_New.ButtonClick += new System.EventHandler(this.ToolBar_New_ButtonClick);
      // 
      // ToolBar_NewProject
      // 
      this.ToolBar_NewProject.Image = global::Editor.Properties.Resources.toolbar_new_project;
      this.ToolBar_NewProject.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolBar_NewProject.Name = "ToolBar_NewProject";
      this.ToolBar_NewProject.Size = new System.Drawing.Size(138, 22);
      this.ToolBar_NewProject.Text = "New Project";
      this.ToolBar_NewProject.Click += new System.EventHandler(this.Menu_File_New_Project_Click);
      // 
      // ToolBar_NewScene
      // 
      this.ToolBar_NewScene.Image = global::Editor.Properties.Resources.toolbar_new_scene;
      this.ToolBar_NewScene.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolBar_NewScene.Name = "ToolBar_NewScene";
      this.ToolBar_NewScene.Size = new System.Drawing.Size(138, 22);
      this.ToolBar_NewScene.Text = "New Scene";
      this.ToolBar_NewScene.Click += new System.EventHandler(this.Menu_File_New_Scene_Click);
      // 
      // ToolBar_Open
      // 
      this.ToolBar_Open.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolBar_Open.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolBar_OpenProject,
            this.ToolBar_OpenScene,
            this.toolStripMenuItem6});
      this.ToolBar_Open.Image = global::Editor.Properties.Resources.toolbar_open;
      this.ToolBar_Open.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolBar_Open.Name = "ToolBar_Open";
      this.ToolBar_Open.Size = new System.Drawing.Size(40, 28);
      this.ToolBar_Open.Text = "Open";
      this.ToolBar_Open.ToolTipText = "Open Project or Scene";
      this.ToolBar_Open.ButtonClick += new System.EventHandler(this.ToolBar_Open_ButtonClick);
      this.ToolBar_Open.DropDownOpening += new System.EventHandler(this.ToolBar_Open_DropDownOpening);
      // 
      // ToolBar_OpenProject
      // 
      this.ToolBar_OpenProject.Image = global::Editor.Properties.Resources.toolbar_open_project;
      this.ToolBar_OpenProject.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolBar_OpenProject.Name = "ToolBar_OpenProject";
      this.ToolBar_OpenProject.Size = new System.Drawing.Size(143, 22);
      this.ToolBar_OpenProject.Text = "Open Project";
      this.ToolBar_OpenProject.Click += new System.EventHandler(this.Menu_File_OpenProject_Click);
      // 
      // ToolBar_OpenScene
      // 
      this.ToolBar_OpenScene.Image = global::Editor.Properties.Resources.toolbar_open_scene;
      this.ToolBar_OpenScene.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolBar_OpenScene.Name = "ToolBar_OpenScene";
      this.ToolBar_OpenScene.Size = new System.Drawing.Size(143, 22);
      this.ToolBar_OpenScene.Text = "Open Scene";
      this.ToolBar_OpenScene.Click += new System.EventHandler(this.Menu_File_OpenScene_Click);
      // 
      // toolStripMenuItem6
      // 
      this.toolStripMenuItem6.Name = "toolStripMenuItem6";
      this.toolStripMenuItem6.Size = new System.Drawing.Size(140, 6);
      // 
      // ToolBar_Save
      // 
      this.ToolBar_Save.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolBar_Save.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolBar_SaveScene,
            this.ToolBar_SaveAll,
            this.ToolBar_SaveAs});
      this.ToolBar_Save.Image = global::Editor.Properties.Resources.toolbar_save;
      this.ToolBar_Save.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolBar_Save.Name = "ToolBar_Save";
      this.ToolBar_Save.Size = new System.Drawing.Size(40, 28);
      this.ToolBar_Save.Text = "Save";
      this.ToolBar_Save.ButtonClick += new System.EventHandler(this.ToolBar_Save_ButtonClick);
      // 
      // ToolBar_SaveScene
      // 
      this.ToolBar_SaveScene.Image = global::Editor.Properties.Resources.toolbar_save_scene;
      this.ToolBar_SaveScene.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolBar_SaveScene.Name = "ToolBar_SaveScene";
      this.ToolBar_SaveScene.Size = new System.Drawing.Size(148, 22);
      this.ToolBar_SaveScene.Text = "Save Scene";
      this.ToolBar_SaveScene.Click += new System.EventHandler(this.Menu_File_SaveScene_Click);
      // 
      // ToolBar_SaveAll
      // 
      this.ToolBar_SaveAll.Image = global::Editor.Properties.Resources.toolbar_save_all;
      this.ToolBar_SaveAll.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolBar_SaveAll.Name = "ToolBar_SaveAll";
      this.ToolBar_SaveAll.Size = new System.Drawing.Size(148, 22);
      this.ToolBar_SaveAll.Text = "Save All";
      this.ToolBar_SaveAll.Click += new System.EventHandler(this.Menu_File_SaveAll_Click);
      // 
      // ToolBar_SaveAs
      // 
      this.ToolBar_SaveAs.Image = global::Editor.Properties.Resources.toolbar_save_as;
      this.ToolBar_SaveAs.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolBar_SaveAs.Name = "ToolBar_SaveAs";
      this.ToolBar_SaveAs.Size = new System.Drawing.Size(148, 22);
      this.ToolBar_SaveAs.Text = "Save Scene As";
      this.ToolBar_SaveAs.Click += new System.EventHandler(this.Menu_File_SaveSceneAs_Click);
      // 
      // ToolBar_Export
      // 
      this.ToolBar_Export.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolBar_Export.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolBar_ExportSettings,
            this.ToolBar_ExportQuick});
      this.ToolBar_Export.Image = global::Editor.Properties.Resources.toolbar_export;
      this.ToolBar_Export.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolBar_Export.Name = "ToolBar_Export";
      this.ToolBar_Export.Size = new System.Drawing.Size(40, 28);
      this.ToolBar_Export.Text = "Export";
      this.ToolBar_Export.ButtonClick += new System.EventHandler(this.Menu_File_ExportScene_Click);
      // 
      // ToolBar_ExportSettings
      // 
      this.ToolBar_ExportSettings.Image = global::Editor.Properties.Resources.toolbar_export_settings;
      this.ToolBar_ExportSettings.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolBar_ExportSettings.Name = "ToolBar_ExportSettings";
      this.ToolBar_ExportSettings.Size = new System.Drawing.Size(188, 22);
      this.ToolBar_ExportSettings.Text = "Export Settings";
      this.ToolBar_ExportSettings.Click += new System.EventHandler(this.Menu_File_ExportScene_Click);
      // 
      // ToolBar_ExportQuick
      // 
      this.ToolBar_ExportQuick.Image = global::Editor.Properties.Resources.toolbar_export_quick;
      this.ToolBar_ExportQuick.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolBar_ExportQuick.Name = "ToolBar_ExportQuick";
      this.ToolBar_ExportQuick.Size = new System.Drawing.Size(188, 22);
      this.ToolBar_ExportQuick.Text = "Quick Export and Run";
      this.ToolBar_ExportQuick.Click += new System.EventHandler(this.ToolBar_ExportQuick_Click);
      // 
      // ToolBar_Sep2
      // 
      this.ToolBar_Sep2.Name = "ToolBar_Sep2";
      this.ToolBar_Sep2.Size = new System.Drawing.Size(6, 31);
      // 
      // Toolbar_Cut
      // 
      this.Toolbar_Cut.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.Toolbar_Cut.Image = global::Editor.Properties.Resources.toolbar_cut;
      this.Toolbar_Cut.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.Toolbar_Cut.Name = "Toolbar_Cut";
      this.Toolbar_Cut.Size = new System.Drawing.Size(28, 28);
      this.Toolbar_Cut.Text = "Cut";
      this.Toolbar_Cut.Click += new System.EventHandler(this.Menu_Edit_Cut_Click);
      // 
      // Toolbar_Copy
      // 
      this.Toolbar_Copy.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.Toolbar_Copy.Image = global::Editor.Properties.Resources.toolbar_copy;
      this.Toolbar_Copy.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.Toolbar_Copy.Name = "Toolbar_Copy";
      this.Toolbar_Copy.Size = new System.Drawing.Size(28, 28);
      this.Toolbar_Copy.Text = "Copy";
      this.Toolbar_Copy.Click += new System.EventHandler(this.Menu_Edit_Copy_Click);
      // 
      // Toolbar_Paste
      // 
      this.Toolbar_Paste.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.Toolbar_Paste.Image = global::Editor.Properties.Resources.toolbar_paste;
      this.Toolbar_Paste.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.Toolbar_Paste.Name = "Toolbar_Paste";
      this.Toolbar_Paste.Size = new System.Drawing.Size(28, 28);
      this.Toolbar_Paste.Text = "Paste";
      this.Toolbar_Paste.Click += new System.EventHandler(this.Menu_Edit_Paste_Click);
      // 
      // Toolbar_Delete
      // 
      this.Toolbar_Delete.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.Toolbar_Delete.Image = global::Editor.Properties.Resources.toolbar_delete;
      this.Toolbar_Delete.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.Toolbar_Delete.Name = "Toolbar_Delete";
      this.Toolbar_Delete.Size = new System.Drawing.Size(28, 28);
      this.Toolbar_Delete.Text = "Delete";
      this.Toolbar_Delete.Click += new System.EventHandler(this.Menu_Edit_Delete_Click);
      // 
      // ToolBar_Sep3
      // 
      this.ToolBar_Sep3.Name = "ToolBar_Sep3";
      this.ToolBar_Sep3.Size = new System.Drawing.Size(6, 31);
      // 
      // Toolbar_Undo
      // 
      this.Toolbar_Undo.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.Toolbar_Undo.Image = global::Editor.Properties.Resources.toolbar_undo;
      this.Toolbar_Undo.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.Toolbar_Undo.Name = "Toolbar_Undo";
      this.Toolbar_Undo.Size = new System.Drawing.Size(28, 28);
      this.Toolbar_Undo.Text = "Undo";
      this.Toolbar_Undo.Click += new System.EventHandler(this.Menu_Edit_Undo_Click);
      // 
      // Toolbar_Redo
      // 
      this.Toolbar_Redo.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.Toolbar_Redo.Image = global::Editor.Properties.Resources.toolbar_redo;
      this.Toolbar_Redo.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.Toolbar_Redo.Name = "Toolbar_Redo";
      this.Toolbar_Redo.Size = new System.Drawing.Size(28, 28);
      this.Toolbar_Redo.Text = "Redo";
      this.Toolbar_Redo.Click += new System.EventHandler(this.Menu_Edit_Redo_Click);
      // 
      // ToolBar_Sep4
      // 
      this.ToolBar_Sep4.Name = "ToolBar_Sep4";
      this.ToolBar_Sep4.Size = new System.Drawing.Size(6, 31);
      // 
      // ToolBar_Panels
      // 
      this.ToolBar_Panels.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolBar_Panels.Image = global::Editor.Properties.Resources.toolbar_panels;
      this.ToolBar_Panels.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolBar_Panels.Name = "ToolBar_Panels";
      this.ToolBar_Panels.Size = new System.Drawing.Size(37, 28);
      this.ToolBar_Panels.Text = "Panels";
      // 
      // ToolBar_OpenAssetBrowser
      // 
      this.ToolBar_OpenAssetBrowser.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolBar_OpenAssetBrowser.Image = global::Editor.Properties.Resources.asset_24x24;
      this.ToolBar_OpenAssetBrowser.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolBar_OpenAssetBrowser.Name = "ToolBar_OpenAssetBrowser";
      this.ToolBar_OpenAssetBrowser.Size = new System.Drawing.Size(28, 28);
      this.ToolBar_OpenAssetBrowser.Text = "Open Asset Browser";
      this.ToolBar_OpenAssetBrowser.ToolTipText = "Shows the Asset Browser, if it is not visible already.";
      this.ToolBar_OpenAssetBrowser.Click += new System.EventHandler(this.OpenAssetBrowser_Click);
      // 
      // ToolBar_Tools
      // 
      this.ToolBar_Tools.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolBar_Tools.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolBar_SkyEditor,
            this.ToolBar_TimeOfDayEditor,
            this.editFogToolStripMenuItem,
            this.ToolBar_PostProcessor,
            this.toolStripMenuItem11,
            this.ToolBar_FindShapes,
            this.ToolBar_FindDuplicates,
            this.toolStripMenuItem12,
            this.ToolBar_SaveScreenshot,
            this.ToolBar_SaveScreenshotAs,
            this.ToolBar_Tools_LastSeparator});
      this.ToolBar_Tools.Image = global::Editor.Properties.Resources.toolbar_tool;
      this.ToolBar_Tools.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolBar_Tools.Name = "ToolBar_Tools";
      this.ToolBar_Tools.Size = new System.Drawing.Size(37, 28);
      this.ToolBar_Tools.Text = "Tools";
      this.ToolBar_Tools.DropDownOpening += new System.EventHandler(this.ToolBar_Tools_DropDownOpening);
      // 
      // ToolBar_SkyEditor
      // 
      this.ToolBar_SkyEditor.Image = global::Editor.Properties.Resources.toolbar_sky;
      this.ToolBar_SkyEditor.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolBar_SkyEditor.Name = "ToolBar_SkyEditor";
      this.ToolBar_SkyEditor.Size = new System.Drawing.Size(175, 22);
      this.ToolBar_SkyEditor.Text = "Edit Sky";
      this.ToolBar_SkyEditor.Click += new System.EventHandler(this.Menu_Engine_EditSky_Click);
      // 
      // ToolBar_TimeOfDayEditor
      // 
      this.ToolBar_TimeOfDayEditor.Image = global::Editor.Properties.Resources.toolbar_tod;
      this.ToolBar_TimeOfDayEditor.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolBar_TimeOfDayEditor.Name = "ToolBar_TimeOfDayEditor";
      this.ToolBar_TimeOfDayEditor.Size = new System.Drawing.Size(175, 22);
      this.ToolBar_TimeOfDayEditor.Text = "Edit Time of Day";
      this.ToolBar_TimeOfDayEditor.Click += new System.EventHandler(this.Menu_Engine_EditTimeOfDay_Click);
      // 
      // editFogToolStripMenuItem
      // 
      this.editFogToolStripMenuItem.Image = global::Editor.Properties.Resources.toolbar_fog;
      this.editFogToolStripMenuItem.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.editFogToolStripMenuItem.Name = "editFogToolStripMenuItem";
      this.editFogToolStripMenuItem.Size = new System.Drawing.Size(175, 22);
      this.editFogToolStripMenuItem.Text = "Edit Fog";
      this.editFogToolStripMenuItem.Click += new System.EventHandler(this.Menu_Engine_EditFog_Click);
      // 
      // ToolBar_PostProcessor
      // 
      this.ToolBar_PostProcessor.Image = global::Editor.Properties.Resources.toolbar_post;
      this.ToolBar_PostProcessor.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolBar_PostProcessor.Name = "ToolBar_PostProcessor";
      this.ToolBar_PostProcessor.Size = new System.Drawing.Size(175, 22);
      this.ToolBar_PostProcessor.Text = "Post Processors";
      this.ToolBar_PostProcessor.Click += new System.EventHandler(this.Menu_Engine_EditPostProcessors_Click);
      // 
      // toolStripMenuItem11
      // 
      this.toolStripMenuItem11.Name = "toolStripMenuItem11";
      this.toolStripMenuItem11.Size = new System.Drawing.Size(172, 6);
      // 
      // ToolBar_FindShapes
      // 
      this.ToolBar_FindShapes.Image = global::Editor.Properties.Resources.toolbar_search;
      this.ToolBar_FindShapes.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolBar_FindShapes.Name = "ToolBar_FindShapes";
      this.ToolBar_FindShapes.Size = new System.Drawing.Size(175, 22);
      this.ToolBar_FindShapes.Text = "Find Shapes";
      this.ToolBar_FindShapes.Click += new System.EventHandler(this.Menu_Edit_FindShapes_Click);
      // 
      // ToolBar_FindDuplicates
      // 
      this.ToolBar_FindDuplicates.Image = global::Editor.Properties.Resources.toolbar_duplicates;
      this.ToolBar_FindDuplicates.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolBar_FindDuplicates.Name = "ToolBar_FindDuplicates";
      this.ToolBar_FindDuplicates.Size = new System.Drawing.Size(175, 22);
      this.ToolBar_FindDuplicates.Text = "FindDuplicates";
      this.ToolBar_FindDuplicates.Click += new System.EventHandler(this.Menu_Edit_FindDuplicates_Click);
      // 
      // toolStripMenuItem12
      // 
      this.toolStripMenuItem12.Name = "toolStripMenuItem12";
      this.toolStripMenuItem12.Size = new System.Drawing.Size(172, 6);
      // 
      // ToolBar_SaveScreenshot
      // 
      this.ToolBar_SaveScreenshot.Image = global::Editor.Properties.Resources.toolbar_screenshot;
      this.ToolBar_SaveScreenshot.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolBar_SaveScreenshot.Name = "ToolBar_SaveScreenshot";
      this.ToolBar_SaveScreenshot.Size = new System.Drawing.Size(175, 22);
      this.ToolBar_SaveScreenshot.Text = "Save Screenshot";
      this.ToolBar_SaveScreenshot.Click += new System.EventHandler(this.Menu_Engine_Screenshot_Click);
      // 
      // ToolBar_SaveScreenshotAs
      // 
      this.ToolBar_SaveScreenshotAs.Image = global::Editor.Properties.Resources.toolbar_screenshot;
      this.ToolBar_SaveScreenshotAs.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolBar_SaveScreenshotAs.Name = "ToolBar_SaveScreenshotAs";
      this.ToolBar_SaveScreenshotAs.Size = new System.Drawing.Size(175, 22);
      this.ToolBar_SaveScreenshotAs.Text = "Save Screenshot As";
      this.ToolBar_SaveScreenshotAs.Click += new System.EventHandler(this.Menu_Engine_ScreenshotAs_Click);
      // 
      // ToolBar_Tools_LastSeparator
      // 
      this.ToolBar_Tools_LastSeparator.Name = "ToolBar_Tools_LastSeparator";
      this.ToolBar_Tools_LastSeparator.Size = new System.Drawing.Size(172, 6);
      this.ToolBar_Tools_LastSeparator.Visible = false;
      // 
      // ToolBar_Resources
      // 
      this.ToolBar_Resources.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolBar_Resources.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolBar_Resources_Reload,
            this.ToolBar_Resources_Show,
            this.ToolBar_Resources_Purge});
      this.ToolBar_Resources.Image = global::Editor.Properties.Resources.toolbar_resources;
      this.ToolBar_Resources.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolBar_Resources.Name = "ToolBar_Resources";
      this.ToolBar_Resources.Size = new System.Drawing.Size(37, 28);
      // 
      // ToolBar_Resources_Reload
      // 
      this.ToolBar_Resources_Reload.Image = global::Editor.Properties.Resources.toolbar_refresh;
      this.ToolBar_Resources_Reload.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolBar_Resources_Reload.Name = "ToolBar_Resources_Reload";
      this.ToolBar_Resources_Reload.Size = new System.Drawing.Size(178, 22);
      this.ToolBar_Resources_Reload.Text = "Reload Resources";
      this.ToolBar_Resources_Reload.Click += new System.EventHandler(this.Menu_Engine_ReloadResources_Click);
      // 
      // ToolBar_Resources_Show
      // 
      this.ToolBar_Resources_Show.Image = global::Editor.Properties.Resources.toolbar_resources_small;
      this.ToolBar_Resources_Show.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolBar_Resources_Show.Name = "ToolBar_Resources_Show";
      this.ToolBar_Resources_Show.Size = new System.Drawing.Size(178, 22);
      this.ToolBar_Resources_Show.Text = "Show Resources";
      this.ToolBar_Resources_Show.Click += new System.EventHandler(this.Menu_Engine_ShowResources_Click);
      // 
      // ToolBar_Resources_Purge
      // 
      this.ToolBar_Resources_Purge.Image = global::Editor.Properties.Resources.toolbar_remove;
      this.ToolBar_Resources_Purge.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolBar_Resources_Purge.Name = "ToolBar_Resources_Purge";
      this.ToolBar_Resources_Purge.Size = new System.Drawing.Size(178, 22);
      this.ToolBar_Resources_Purge.Text = "Purge All Reosurces";
      this.ToolBar_Resources_Purge.Click += new System.EventHandler(this.Menu_Engine_PurgeResources_Click);
      // 
      // ToolBar_Relight
      // 
      this.ToolBar_Relight.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolBar_Relight.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolBar_LightSettings,
            this.ToolBar_LightCalc,
            this.ToolBar_LightReset});
      this.ToolBar_Relight.Image = global::Editor.Properties.Resources.toolbar_light;
      this.ToolBar_Relight.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolBar_Relight.Name = "ToolBar_Relight";
      this.ToolBar_Relight.Size = new System.Drawing.Size(37, 28);
      this.ToolBar_Relight.Text = "Static Lighting";
      // 
      // ToolBar_LightSettings
      // 
      this.ToolBar_LightSettings.Image = global::Editor.Properties.Resources.toolbar_export_settings;
      this.ToolBar_LightSettings.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolBar_LightSettings.Name = "ToolBar_LightSettings";
      this.ToolBar_LightSettings.Size = new System.Drawing.Size(170, 22);
      this.ToolBar_LightSettings.Text = "Lighting Settings";
      this.ToolBar_LightSettings.Click += new System.EventHandler(this.Menu_Engine_UpdateLighting_Click);
      // 
      // ToolBar_LightCalc
      // 
      this.ToolBar_LightCalc.Image = global::Editor.Properties.Resources.toolbar_light_calc;
      this.ToolBar_LightCalc.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolBar_LightCalc.Name = "ToolBar_LightCalc";
      this.ToolBar_LightCalc.Size = new System.Drawing.Size(170, 22);
      this.ToolBar_LightCalc.Text = "Calculate Lighting";
      this.ToolBar_LightCalc.Click += new System.EventHandler(this.Menu_Tools_CalculateLighting_Click);
      // 
      // ToolBar_LightReset
      // 
      this.ToolBar_LightReset.Image = global::Editor.Properties.Resources.toolbar_light_reset;
      this.ToolBar_LightReset.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolBar_LightReset.Name = "ToolBar_LightReset";
      this.ToolBar_LightReset.Size = new System.Drawing.Size(170, 22);
      this.ToolBar_LightReset.Text = "Reset Lighting";
      this.ToolBar_LightReset.Click += new System.EventHandler(this.Menu_Tools_ResetLighting_Click);
      // 
      // ToolBar_Visibility
      // 
      this.ToolBar_Visibility.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolBar_Visibility.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolBar_CalcVisibility,
            this.ToolBar_UseVisibility});
      this.ToolBar_Visibility.Image = global::Editor.Properties.Resources.toolbar_visibility;
      this.ToolBar_Visibility.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolBar_Visibility.Name = "ToolBar_Visibility";
      this.ToolBar_Visibility.Size = new System.Drawing.Size(37, 28);
      this.ToolBar_Visibility.Text = "Visibility";
      // 
      // ToolBar_CalcVisibility
      // 
      this.ToolBar_CalcVisibility.Image = global::Editor.Properties.Resources.toolbar_add;
      this.ToolBar_CalcVisibility.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolBar_CalcVisibility.Name = "ToolBar_CalcVisibility";
      this.ToolBar_CalcVisibility.Size = new System.Drawing.Size(170, 22);
      this.ToolBar_CalcVisibility.Text = "Calculate Visibility";
      this.ToolBar_CalcVisibility.Click += new System.EventHandler(this.menu_CalculateVisibility_Click);
      // 
      // ToolBar_UseVisibility
      // 
      this.ToolBar_UseVisibility.Image = global::Editor.Properties.Resources.toolbar_check;
      this.ToolBar_UseVisibility.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolBar_UseVisibility.Name = "ToolBar_UseVisibility";
      this.ToolBar_UseVisibility.Size = new System.Drawing.Size(170, 22);
      this.ToolBar_UseVisibility.Text = "Use Visibility";
      this.ToolBar_UseVisibility.Click += new System.EventHandler(this.menu_UseVisibility_Click);
      // 
      // ToolBar_Sep5
      // 
      this.ToolBar_Sep5.Name = "ToolBar_Sep5";
      this.ToolBar_Sep5.Size = new System.Drawing.Size(6, 31);
      // 
      // statusBar
      // 
      this.statusBar.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.statusPanel_Main,
            this.toolStripSeparator1,
            this.statusPanel_TransformProgress,
            this.toolStripSeparator3,
            this.statusPanel_Layout,
            this.toolStripSeparator2,
            this.statusPanel_PlayMode});
      this.statusBar.Location = new System.Drawing.Point(0, 549);
      this.statusBar.Name = "statusBar";
      this.statusBar.Size = new System.Drawing.Size(1028, 23);
      this.statusBar.TabIndex = 3;
      this.statusBar.Text = "statusBar";
      // 
      // statusPanel_Main
      // 
      this.statusPanel_Main.Name = "statusPanel_Main";
      this.statusPanel_Main.Size = new System.Drawing.Size(474, 18);
      this.statusPanel_Main.Spring = true;
      this.statusPanel_Main.Text = "statusPanel_Main";
      // 
      // toolStripSeparator1
      // 
      this.toolStripSeparator1.Name = "toolStripSeparator1";
      this.toolStripSeparator1.Size = new System.Drawing.Size(6, 23);
      // 
      // statusPanel_TransformProgress
      // 
      this.statusPanel_TransformProgress.AutoSize = false;
      this.statusPanel_TransformProgress.Name = "statusPanel_TransformProgress";
      this.statusPanel_TransformProgress.Size = new System.Drawing.Size(220, 21);
      // 
      // toolStripSeparator3
      // 
      this.toolStripSeparator3.Name = "toolStripSeparator3";
      this.toolStripSeparator3.Size = new System.Drawing.Size(6, 23);
      // 
      // statusPanel_Layout
      // 
      this.statusPanel_Layout.AutoSize = false;
      this.statusPanel_Layout.Name = "statusPanel_Layout";
      this.statusPanel_Layout.Size = new System.Drawing.Size(150, 18);
      this.statusPanel_Layout.Text = "statusPanel_Layout";
      // 
      // toolStripSeparator2
      // 
      this.toolStripSeparator2.Name = "toolStripSeparator2";
      this.toolStripSeparator2.Size = new System.Drawing.Size(6, 23);
      // 
      // statusPanel_PlayMode
      // 
      this.statusPanel_PlayMode.AutoSize = false;
      this.statusPanel_PlayMode.Name = "statusPanel_PlayMode";
      this.statusPanel_PlayMode.Size = new System.Drawing.Size(120, 18);
      this.statusPanel_PlayMode.Text = "statusPanel_PlayMode";
      // 
      // dockContainer
      // 
      this.dockContainer.ActiveAutoHideContent = null;
      this.dockContainer.Dock = System.Windows.Forms.DockStyle.Fill;
      this.dockContainer.DocumentStyle = WeifenLuo.WinFormsUI.DocumentStyles.DockingWindow;
      this.dockContainer.Font = new System.Drawing.Font("Tahoma", 11F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.World);
      this.dockContainer.Location = new System.Drawing.Point(0, 55);
      this.dockContainer.Margin = new System.Windows.Forms.Padding(0);
      this.dockContainer.Name = "dockContainer";
      this.dockContainer.Size = new System.Drawing.Size(1028, 517);
      this.dockContainer.TabIndex = 13;
      // 
      // Form1
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(1028, 572);
      this.Controls.Add(this.statusBar);
      this.Controls.Add(this.dockContainer);
      this.Controls.Add(this.mainToolBar);
      this.Controls.Add(this.mainMenu);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.MainMenuStrip = this.mainMenu;
      this.Name = "Form1";
      this.Text = "vForge";
      this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
      this.Closing += new System.ComponentModel.CancelEventHandler(this.Form1_Closing);
      this.mainMenu.ResumeLayout(false);
      this.mainMenu.PerformLayout();
      this.mainToolBar.ResumeLayout(false);
      this.mainToolBar.PerformLayout();
      this.statusBar.ResumeLayout(false);
      this.statusBar.PerformLayout();
      this.ResumeLayout(false);
      this.PerformLayout();

    }
    #endregion


    #region WndProc

    [DllImport("User32.dll")]
    protected static extern IntPtr SetClipboardViewer(IntPtr hWndNewViewer);

    [DllImport("User32.dll", CharSet = CharSet.Auto)]
    public static extern bool ChangeClipboardChain(IntPtr hWndRemove, IntPtr hWndNewNext);

    [DllImport("user32.dll", CharSet = CharSet.Auto)]
    public static extern int SendMessage(IntPtr hwnd, int wMsg, IntPtr wParam, IntPtr lParam);

    IntPtr nextClipboardChainViewer;

    protected override void WndProc(ref Message m)
    {
      // defined in winuser.h
      const int WM_DRAWCLIPBOARD = 0x308;
      const int WM_CHANGECBCHAIN = 0x030D;

      switch (m.Msg)
      {
        case WM_DRAWCLIPBOARD:
          EditorManager.Clipboard.CheckClipboardContent();
          UpdateMainToolbarStates(); // update paste icon
          SendMessage(nextClipboardChainViewer, m.Msg, m.WParam, m.LParam); // propagate to next window in the chain
          break;
        case WM_CHANGECBCHAIN:
          // update chain
          if (m.WParam == nextClipboardChainViewer)
            nextClipboardChainViewer = m.LParam;
          else
            SendMessage(nextClipboardChainViewer, m.Msg, m.WParam, m.LParam);
          break;
        default:
          base.WndProc(ref m);
          break;
      }
    }

    #endregion

    #region MaxFPS Option

    // Custom struct for PeekMessage that is adapted to the output of the PeekMessage WinAPI call
    [StructLayout(LayoutKind.Sequential)]
    private struct PeekMsg
    {
      public IntPtr hWnd;
      public Message msg;
      public IntPtr wParam;
      public IntPtr lParam;
      public uint time;
      public System.Drawing.Point p;
    }

    /// <summary>
    /// P/Invoke to check for incoming messages
    /// </summary>
    /// <returns>True if messages are available</returns>
    [System.Security.SuppressUnmanagedCodeSecurity] // SpeedUp: Deactivate security checks when calling native code on this function
    [DllImport("User32.dll", CharSet = CharSet.Auto)]
    private static extern bool PeekMessage(out PeekMsg msg, IntPtr hWnd, uint messageFilterMin, uint messageFilterMax, uint flags);

    /// <summary>
    /// True if no messages have to be processed, False otherwise
    /// </summary>
    private bool AppStillIdle
    {
      get
      {
        // We don't handle any messages here, we just check if there are any
        PeekMsg msg;
        return !PeekMessage(out msg, IntPtr.Zero, 0, 0, 0);
      }
    }

    /// <summary>
    /// Event handler in case the application is idle
    /// We use this to re-render the scene in case no messages have to be processed
    /// so that we get the maximum frame rate possible (as opposed to the default behavior
    /// of 30 fps for the Animate/Run modes or the ~150 fps when moving the mouse over the engine view)
    /// </summary>
    private void Application_Idle_maxfps(object sender, EventArgs e)
    {
      while (AppStillIdle)
      {
        if (enginePanel1 != null)
        {
          // If the application is idle and the engine view is present, force rerender
          enginePanel1.View.UpdateView(true);
        }
      }
    }

    private void Application_Idle(object sender, EventArgs e)
    {
      UpdateCopyPasteToolbar();
    }

    #endregion
	
    #region Custom Components

    MRUList mruList_RecentProjects;
    MRUList mruList_RecentScenes;

    // docking panels; it is important to keep a reference to the panels
    ShapeCreatorPanel shapeCreatorPanel = null;
    UndoHistoryPanel undoHistoryPanel = null;
    ShapeTreePanel shapeTreePanel = null;
    ShapeSearchPanel shapeSearchPanel = null;
    FindDuplicatesPanel shapeDuplicatesPanel = null;
    VisibilityResultPanel visibilityResultPanel = null;
    ResourceViewerPanel resourceViewerPanel = null;
    ActionRecorderPanel actionRecorderPanel = null;
    AssetBrowserPanel assetBrowserPanel = null;

    //CollectionPanel collectionPanel = null; // deactivated for now, will come in a later release

    //InteractiveHelpPanel interactiveHelpPanel = null;

    // I put this into a separate function because VS messed it up too often!
    void InitializeMyComponent()
    {
      this.SuspendLayout();

      // Creating Docking Containers
      dockContainer.BringToFront();
      dockContainer.Parent = this;
      EditorManager.ApplicationLayout.DockingArea = dockContainer;

      // Create Engine Panel
      enginePanel1 = new Editor.EnginePanel(EditorManager.ApplicationLayout.DockingArea);
      enginePanel1.ShowDockable();

      // TEMP
      //interactiveHelpPanel = new InteractiveHelpPanel();
      //interactiveHelpPanel.ShowDockable();

      // create dockable undo redo history panel
      undoHistoryPanel = new UndoHistoryPanel(EditorManager.ApplicationLayout.DockingArea);
      undoHistoryPanel.ShowDockable();

      // TEMP
      shapeCreatorPanel = new ShapeCreatorPanel(EditorManager.ApplicationLayout.DockingArea);
      shapeCreatorPanel.ShowDockable();
      shapeCreatorPanel.SelectedCreatorObjectChanged += new EventHandler(shapeCreatorPanel_SelectedCreatorObjectChanged);

      // AssetBrowserPanel
      assetBrowserPanel = new AssetBrowserPanel(EditorManager.ApplicationLayout.DockingArea);
      assetBrowserPanel.ShowDockable();

      // deactivated for now, will come in a later release
      //collectionPanel = new CollectionPanel(EditorManager.ApplicationLayout.DockingArea);
      //collectionPanel.ShowDockable();

      // Create Dockable Property Panel
      propertyPanel1 = new Editor.PropertyPanel(EditorManager.ApplicationLayout.DockingArea);
      propertyPanel1.ShowDockable();

      // Create Dockable ShapeTree Panel
      shapeTreePanel = new ShapeTreePanel(EditorManager.ApplicationLayout.DockingArea);
      shapeTreePanel.ShowDockable();

      // Create Shape Search Panel
      shapeSearchPanel = new ShapeSearchPanel(EditorManager.ApplicationLayout.DockingArea);
      shapeSearchPanel.ShowDockable();

      shapeDuplicatesPanel = new FindDuplicatesPanel(EditorManager.ApplicationLayout.DockingArea);
      shapeDuplicatesPanel.ShowDockable();

      visibilityResultPanel = new VisibilityResultPanel(EditorManager.ApplicationLayout.DockingArea);
      visibilityResultPanel.ShowDockable();

      actionRecorderPanel = new ActionRecorderPanel(EditorManager.ApplicationLayout.DockingArea);
      actionRecorderPanel.ShowDockable();

      // Script Panel
      ScriptPanel scriptPanel = new VisionScriptPanel(EditorManager.ApplicationLayout.DockingArea);
      scriptPanel.ShowDockable();
      ScriptPanel.Instance = scriptPanel;

      // Component Panel
      ConfigureShapeComponentsPanel.PanelInstance.ShowDockable();

      try
      {
        resourceViewerPanel = new ResourceViewerPanel(EditorManager.ApplicationLayout.DockingArea);
        resourceViewerPanel.ShowDockable();
        EditorManager.EngineManager.LogWindow = resourceViewerPanel;
        vResourceViewer.Classes.ResourceInfo.OnSelectedResourceChanged += new EventHandler(ResourceInfo_OnSelectedResourceChanged);
        vResourceViewerBase80.Controls.MasterPanel.OnAutomatedResourcePreview += new EventHandler(ResourceInfo_OnAutomatedResourcePreview);
        resourceViewerPanel.VisibleChanged += new EventHandler(resourceViewerPanel_VisibleChanged);
        EditorManager.EditorConsole = resourceViewerPanel; // it implements IEditorConsole
      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex);
      }

      this.ResumeLayout(true);

      // set the references in the EditorManager
      EditorManager.GUI.MainMenu = this.mainMenu;
      EditorManager.GUI.MainToolBar = this.mainToolBar;
      EditorManager.GUI.EnginePanelToolBar = this.enginePanel1.Toolbar;

      // Create the MRU lists
      mruList_RecentProjects = new MRUList(RecentProjects);
      mruList_RecentProjects.MRUEvent += new MRUListEventHandler(this.Menu_File_Recent_Project_Click);
      mruList_RecentProjects.LoadFromRegistry(@"Software\Havok\VisionSDK\vEngineEditor\RecentProjects");

      mruList_RecentScenes = new MRUList(RecentScenes);
      mruList_RecentScenes.MRUEvent += new MRUListEventHandler(this.Menu_File_Recent_Scene_Click);
      mruList_RecentScenes.LoadFromRegistry(@"Software\Havok\VisionSDK\vEngineEditor\RecentScenes");

      // Check for a tablet
      CSharpFramework.Contexts.WintabTabletContext tablet = new CSharpFramework.Contexts.WintabTabletContext();
      if (tablet.IsAvailable && tablet.IsActive)
        EditorManager.Tablet = tablet;
      else
        EditorManager.Tablet = new NullTabletContext();

      // default lighting tool
      VLux.VLuxLightingTool vLuxTool = new VLux.VLuxLightingTool();
      EditorManager.SupportedLightingTools.Add(vLuxTool);
    }


    /// <summary>
    /// Creates a combobox control for switching layouts and adds it to the toolbar
    /// </summary>
    public void InitializeLayoutButton()
    {
      // Create combobox control
      layoutComboBox = new ToolStripComboBox();
      layoutComboBox.Width = _iDefaultComboBoxWidth;
      layoutComboBox.FlatStyle = FlatStyle.Standard; 
      layoutComboBox.DropDownHeight = 250; // so there is enough space for layouts without scrolling
      layoutComboBox.DropDownStyle = ComboBoxStyle.DropDownList;
      layoutComboBox.SelectedIndexChanged += new EventHandler(layoutBox_SelectedIndexChanged);

      // Add control to toolbar
      mainToolBar.Items.Add(layoutComboBox);
      mainToolBar.Items.Add(new ToolStripSeparator());

      // Fill content
      UpdateLayoutButtonContent();      
    }

    /// <summary>
    /// Updates the content of the layout dropdown list
    /// </summary>
    public void UpdateLayoutButtonContent()
    {
      if (layoutComboBox == null)
        return;

      layoutComboBox.Items.Clear();
      foreach(Layout layout in EditorManager.ApplicationLayout.AvailableLayouts)
        layoutComboBox.Items.Add(layout);

      UpdateLayoutButtonSelection();
    }

    /// <summary>
    /// Updates the current selection of the layout dropdown list
    /// </summary>
    public void UpdateLayoutButtonSelection()
    {
      if (layoutComboBox != null)
        layoutComboBox.SelectedItem = EditorManager.ApplicationLayout.ActiveLayout;
    }


    private void layoutBox_SelectedIndexChanged(object sender, EventArgs e)
    {
      EditorManager.ApplicationLayout.ActiveLayout = layoutComboBox.SelectedItem as Layout;
    }

    #endregion

    
    #region Asset Profile ComboBox

    /// <summary>
    /// Creates a combobox control for switching asset profiles and adds it to the toolbar
    /// </summary>
    public void InitializeProfileComboBox()
    {
      // Create combobox control
      profileComboBox = new ProfileToolStripComboBox();
      assetPreviewModeButton = new AssetPreviewModeButton();

      // Add control to toolbar
      mainToolBar.Items.Add(profileComboBox);
      mainToolBar.Items.Add(assetPreviewModeButton);
      mainToolBar.Items.Add(new ToolStripSeparator());
    }
    
    #endregion Asset Profile ComboBox


    #region SelectionSet ComboBox

    private int _iDefaultComboBoxWidth = 160;

    public void InitializeSelectionSetBox()
    {
      selectionSetComboBox = new ToolStripComboBox();
      selectionSetComboBox.Width = _iDefaultComboBoxWidth;
      selectionSetComboBox.DropDownStyle = ComboBoxStyle.DropDown;
      selectionSetComboBox.FlatStyle = FlatStyle.Standard; 
      selectionSetComboBox.KeyPress += new KeyPressEventHandler(selectionSetComboBox_KeyPress);
      selectionSetComboBox.SelectedIndexChanged += new EventHandler(selectionSetComboBox_SelectedValueChanged);
      selectionSetComboBox.Enabled = false;
      
      mainToolBar.Items.Add(selectionSetComboBox);
      mainToolBar.Items.Add(new ToolStripSeparator());

      UpdateSelectionSetButton();
    }

    void selectionSetComboBox_SelectedValueChanged(object sender, EventArgs e)
    {
      if (selectionSetComboBox.SelectedItem != null)
      {
        ShapeCollection list = (selectionSetComboBox.SelectedItem as SelectionSetItem).Shapes;
        ShapeCollection actualShapes = new ShapeCollection();
        foreach (ShapeBase shape in list)
          if (shape.IsAddedToScene)
            actualShapes.Add(shape);
        EditorManager.SelectedShapes = actualShapes;
      }
    }

    private void UpdateSelectionSetButton()
    {
      if (selectionSetComboBox != null)
      {
        // Reset the text to the default.
        selectionSetComboBox.Text = "Create Selection Set...";
      }   
    }

    void selectionSetComboBox_KeyPress(object sender, KeyPressEventArgs e)
    {
      if (e.KeyChar == 13) // represents Enter-key.
      {
        SelectionSetItem setItem = new SelectionSetItem(selectionSetComboBox.Text, EditorManager.SelectedShapes);
        if (!selectionSetComboBox.Items.Contains(setItem))
          selectionSetComboBox.Items.Add(setItem);
      }
    }



    #endregion

    #region Main

    bool _bFirstRun = true;

    /// <summary>
    /// Globally stores the command line arguments passed to the Main function
    /// </summary>
    public string[] CommandLineArgs;
    
    /// <summary>
    /// Parses the parameter strings passed to the application
    /// </summary>
    /// <param name="args"></param>
    protected void ParseArgs(string[] args)
    {
      bool bShutDown = false;
      bool bForceAllLayers = false;

      // the following parameters must be set before export etc. are triggered
      foreach (string arg in args)
      {
        if (string.Compare(arg, "-shutdown", true) == 0)
        {
          bShutDown = true;
          continue;
        }

        if (string.Compare(arg, "-silent", true) == 0)
        {
          EditorManager.SilentMode = true;
          continue;
        }

        if (string.Compare(arg, "-swrenderer", true) == 0)
        {
          EditorManager.EngineManager.UseSoftwareRenderer = true;
          continue;
        }

        if (string.Compare(arg, "-headless", true) == 0)
        {
          EditorManager.EngineManager.UseHeadlessMode = true;
          continue;
        }

        if (string.Compare(arg, "-forceAllLayers", true) == 0)
        {
          bForceAllLayers = true;
          continue;
        }
        
      }

      foreach (string arg in args)
      {
        // Ignore the parameter that starts the automated tests
        // (this one is handled by the Main function instead)
        if (string.Compare(arg,"-test",true)==0)
          continue;

        // parse some pre-defined switches

        // Max FPS switch (experimental)
        if (string.Compare(arg, "-maxfps", true) == 0)
        {
          // We register the form for the Idle event to rerender the scene with maximum framerate
          // This is based on http://www.codeproject.com/Articles/10991/Simple-Managed-DirectX-Render-Loop
          Application.Idle += new EventHandler(Application_Idle_maxfps);
          continue;
        }
		
        // export scene
        if ((arg.Length >= 7) && (string.Compare(arg.Substring(0, 7), "-export", true) == 0))
        {
          if (EditorApp.Scene == null)
          {
            EditorManager.ShowMessageBox("Invalid '-export' switch: No scene loaded to export.","Error",MessageBoxButtons.OK,MessageBoxIcon.Error);
            continue;
          }
          // make sure all cubemaps are updated...
          for (int i = 0; i < 3; i++)
            EditorManager.ActiveView.UpdateView(true);

          if (bForceAllLayers)
            foreach (Layer layer in EditorManager.Scene.Layers)
              layer.Export = true;
 
          // Check for export profile
          string sOptions = arg.Length > 7 ? arg.Substring(arg.IndexOf('[')) : null;
          if (sOptions != null)
          {
            string sExportProfile = sOptions.Substring(1, sOptions.Length - 2);
            SceneExportProfile exportProfile = SceneExportProfile.LoadProfile(EditorApp.Scene, sExportProfile, false);
            if (exportProfile != null)
            {
              exportProfile.FixupLayerNames();
              EditorApp.Scene.CurrentExportProfile = exportProfile;
              exportProfile.ExportedLayersToScene();
              
            }
          }

          // Export Scene
          if (!EditorApp.Scene.ExportScene(null, null))
            EditorManager.ShowMessageBox("Failed to export scene " + EditorApp.Scene.FileName, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
          continue;
        }

        // relight scene
        if (string.Compare(arg, "-relight", true) == 0)
        {
          if (EditorApp.Scene == null)
          {
            EditorManager.ShowMessageBox("Invalid '-relight' switch: No scene loaded to relight.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            continue;
          }
          // force every zone to be loaded and relight
          foreach (Zone zone in EditorManager.Scene.Zones)
          {
            zone.Loaded = true;
            zone.RunLighting = true;
          }

          if (!EditorApp.Scene.UpdateStaticLighting(false,null,true))
            EditorManager.ShowMessageBox("Failed to update static lighting for " + EditorApp.Scene.FileName, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
          continue;
        }

        // save scene
        if (string.Compare(arg, "-save", true) == 0)
        {
          if (EditorApp.Scene == null)
          {
            EditorManager.ShowMessageBox("Invalid '-save' switch: No scene loaded to save.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            continue;
          }
          SaveScene();
          continue;
        }
        // any other switch:
        if (arg.StartsWith("-"))
          continue;

        // analyze filenames:
        string ext = Path.GetExtension(arg).ToLower();
        if (ext==".scene")
        {
          string sceneFileName = arg;
          //replace frontslashes by backslashes
          sceneFileName = sceneFileName.Replace("/", Path.DirectorySeparatorChar.ToString()); 
          //make scenefilename absolute if possible
          if (!FileHelper.IsAbsolute(arg))
          {
            if (EditorApp.Project != null)
            {
              sceneFileName = EditorApp.Project.MakeAbsolute(arg);
            }
            else
            {
              EditorManager.ShowMessageBox("Failed to load file: \n" + arg + "\n\n- either use an absolute path for your scene file \n- or specify an absolute path to your project file and set your scene path relative to the project file", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
              return;
            } 
          }
          // loads the scene if possible
          LoadScene(sceneFileName, true);
        } 
        else if (ext==".project")
        {
          //replace frontslashes by backslashes
          string projectFileName = arg;
          projectFileName = projectFileName.Replace("/", Path.DirectorySeparatorChar.ToString());
          LoadProject(projectFileName, false);
        }
        else if (ext!=null) // unknown file extension -> try to load via tools
        {
          EditorCustomFileLoadingArgs e = new EditorCustomFileLoadingArgs(this,arg);
          EditorManager.OnLoadCustomFile(e);
          if (!e.Success)
          {
            if (!e.Supported)
              EditorManager.ShowMessageBox("Unsupported file type passed as an argument:\n"+arg,"Error",MessageBoxButtons.OK,MessageBoxIcon.Error);
            else
              EditorManager.ShowMessageBox("Failed to load file: \n"+arg,"Error",MessageBoxButtons.OK,MessageBoxIcon.Error);
          }
        }
      }

      // For each command line argument, trigger an event in EditorManager.
      // This can be used in e.g. editor plugins so that they may also react to command line arguments if desired.
      foreach (string arg in args)
      {
        EditorManager.OnCmdLineArgumentPassed(new CmdLineEventArgs(arg));
      }

      if (bShutDown)
        Menu_File_Exit.PerformClick();
    }

    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [STAThread]
    static int Main( string[] args ) 
    {
      // should be the first thing to call.
      // This feature activates Group display in list views for instance.
      // Observe in the future: Is this the cause for any SEHException or strange UI behavior?
      
      Application.EnableVisualStyles();
      //Application.DoEvents(); // recommended in the web to avoid misc problems

      // set all possible types of culture type to avoid decimal comma in UI and prefab(!) .ToString functions
      CultureInfo culture = new System.Globalization.CultureInfo("en-US");

      // some of these are probably duplicates, but anyway...
      System.Threading.Thread.CurrentThread.CurrentCulture = culture;
      Application.CurrentCulture = culture;
      Thread.CurrentThread.CurrentCulture = culture;
      Thread.CurrentThread.CurrentUICulture = culture; // this seems responsible for number.ToString problems

      // initialize DLLs
      // int retval = ManagedFramework.ManagedFramework.minitialize() ;
    
      // register ManagedFramework tests in test manager
      TestSuiteBuilder testBuilder = new TestSuiteBuilder();
      TestSuite testSuiteManagedFrameWork = testBuilder.Build( typeof(ManagedFramework.VisionEngineManager).Assembly.FullName );
      TestManager.AddTestSuite(testSuiteManagedFrameWork);

      // register editor tests in test manager
      testBuilder = new TestSuiteBuilder();
      TestSuite testSuite = testBuilder.Build( typeof(EditorApp).Assembly.FullName );
      TestManager.AddTestSuite(testSuite);

      // parse command line for automated tests
      if (args.Length == 1 && args[0] == "-test")
        g_triggerTests = true;

      // this is our workaround:
      EditorManager.DesignMode = false;

      // we must ensure that the CWD is the EXE dir, otherwise we get lots of problems (bitmap files etc.)
      string sEXEDir = Application.StartupPath;
      Directory.SetCurrentDirectory(sEXEDir);

      Form1 form = new Form1();
      form.CommandLineArgs = args; // store them and parse them a bit later (first time in OnVisibleChanged)

#if (!DEBUG)
      AppDomain.CurrentDomain.UnhandledException += new UnhandledExceptionEventHandler(Domain_UnhandledException);
      Application.ThreadException += new System.Threading.ThreadExceptionEventHandler(Application_ThreadException);

      try 
#endif
      {
        // run application
        Application.Run(form);
      }
#if (!DEBUG)
      catch (Exception ex)
      {
        EditorManager.DumpException(ex, true);
      }
#endif

      EditorApp.DeinitEditorApp();

      if (g_triggerTests)
        return (g_testResult ? 0 : -1);

      // deinit DLLs
      //ManagedFramework.ManagedFramework.mterminate();

      return 0;
    }

    #endregion

    #region Settings

    /// <summary>
    /// Combines the settings filename with user data path
    /// </summary>
    public string SettingsFilename {get {return Path.Combine(EditorManager.GetDirectory(EditorManager.DirectoryType.UserDataPath), "vForgeConfig.XML");}}

    /// <summary>
    /// Combines the default layout filename with application startup path
    /// </summary>
    public string DefaultLayoutFilename { get { return Path.Combine(EditorManager.GetDirectory(EditorManager.DirectoryType.StartupPath), "Layout.XML"); } }

    /// <summary>
    /// Saves the editor settings to XML file
    /// </summary>
    public void SaveSettings()
    {
      if (!EditorManager.SaveSettingsToFile(SettingsFilename))
        EditorManager.ShowMessageBox("Failed to save settings file 'vForgeConfig.XML'","Error",MessageBoxButtons.OK,MessageBoxIcon.Error);
      if (!EditorManager.ShortCuts.SaveToFile(null))
        EditorManager.ShowMessageBox("Failed to save hotkey settings file '"+ShortCutConfig.DEFAULT_SETTINGS_FILE+"'","Error",MessageBoxButtons.OK,MessageBoxIcon.Error);
    }

    /// <summary>
    /// Loads the editor settings from XML file
    /// </summary>
    public void LoadSettings()
    {
      //this mechanism is for backward compatibility only and we can drop it after 2013.1
      if(!File.Exists(SettingsFilename))
      {
        String appData = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData);
        EditorManager.LoadSettingsFromFile(appData + @"\Trinigy\vForge\vForgeConfig.XML");
      }
      else
      {
        EditorManager.LoadSettingsFromFile(SettingsFilename);
      }
      
      EditorManager.ShortCuts.LoadFromFile(null);
    }

    #endregion

    #region Remote Manager Initialization

    private void InitializeRemoteManager()
    {
      // Initialize Xbox 360 connection
      EditorManager.RemoteManagerXbox360 = null;
      IPlatformManager remoteManager = new VisionPlatformManager();
      if (remoteManager.Initialize(RemoteMachineType.XBOX360))
      {
        EditorManager.RemoteManagerXbox360 = remoteManager;
      }
      else
      {
        this.Menu_Remote_XBOX360_Connect.Visible = false;
        this.Menu_Remote_XBOX360_Launch.Visible = false;
        this.Menu_Remote_XBOX360_Stop.Visible = false;
        this.Menu_Remote_XBOX360_PlayTheGame.Visible = false;
        this.Menu_Remote_XBOX360_Animate.Visible = false;
        this.Menu_Remote_XBOX360_Seperator.Visible = false;
      }

      // Initialize PS3 connection
      EditorManager.RemoteManagerPS3 = null;

      // no remote access in debug because there is no debug version of dtlib_2005.lib
      remoteManager = new VisionPlatformManager();
      if (remoteManager.Initialize(RemoteMachineType.PS3))
      {
        EditorManager.RemoteManagerPS3 = remoteManager;
      }
      else

      {
        this.Menu_Remote_PS3_Connect.Visible = false;
        this.Menu_Remote_PS3_Stop.Visible = false;
        this.Menu_Remote_PS3_PlayTheGame.Visible = false;
        this.Menu_Remote_PS3_Animate.Visible = false;
        this.Menu_Remote_PS3_Seperator.Visible = false;
      }

      AddListenersToRemoteManager();

      if (EditorManager.RemoteManagerXbox360!=null && EditorManager.Settings.ConnectOnStartupXbox360)
      {
        String targetMachine = EditorManager.Settings.RemoteMachineNameXbox360;
        if (!EditorManager.RemoteManagerXbox360.Connect(targetMachine))
          EditorManager.ShowMessageBox(this, "Could not connect to Xbox 360 " + targetMachine +".\r\nPlease make sure that the Xbox 360 is switched on and connected to the network.\r\n" + EditorManager.RemoteManagerXbox360.GetLastError());
      }

      if (EditorManager.RemoteManagerPS3 != null && EditorManager.Settings.ConnectOnStartupPS3)
      {
        String targetMachine = EditorManager.Settings.RemoteMachineNamePS3;
        if (!EditorManager.RemoteManagerPS3.Connect(targetMachine))
          EditorManager.ShowMessageBox(this, "Could not connect to PS3 " + targetMachine + ".\r\nPlease make sure that the PS3 is switched on and connected to the network.\r\n" + EditorManager.RemoteManagerPS3.GetLastError());
      }

      if ((EditorManager.RemoteManagerPS3 == null) && (EditorManager.RemoteManagerXbox360 == null))
      {
        Menu_Remote.Visible = false;
      }
    }

    void RemoteManager_Connected_Changed(bool status)
    {
      // TODO: Add visual indicator of connection status to UI
    }

    void AddListenersToRemoteManager()
    {
      if (EditorManager.RemoteManagerXbox360 != null)
      {
        EditorManager.RemoteManagerXbox360.PlatformManagerConnected += new IPlatformManager.PlatformManagerConnectedHandler(this.RemoteManager_Connected_Changed);
        EditorManager.RemoteManagerXbox360.PlatformManagerMessageSent += new IPlatformManager.PlatformManagerMessageHandler(this.RemoteManager_Message_Sent);
      }
    }

    #endregion

    #region Listener functions

    static void Domain_UnhandledException(object sender, UnhandledExceptionEventArgs e)
    {
      EditorManager.DumpException((Exception)e.ExceptionObject, true);
    }

    static void Application_ThreadException(object sender, ThreadExceptionEventArgs e)
    {
      EditorManager.DumpException(e.Exception, true);
    }

    /// <summary>
    /// Respond to the scene change event
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void EditorManager_SceneChanged(object sender, SceneChangedArgs e)
    {
      EditorManager.AnimationSpeed = 1.0f;
      UpdateToolbars();
    }

    void IScene_OnDirtyFlagChanged(object sender, EventArgs e)
    {
      if (sender==EditorManager.Scene)
        UpdateToolbars();
    }

    /// <summary>
    /// Respond to play mode changed
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void EditorManager_EditorModeChanged(object sender, EditorModeChangedArgs e)
    {
      // update the play button (play/stop button image), the export button etc.
      if (e.action==EditorModeChangedArgs.Action.ModeChanged)
      {
        UpdateMainToolbarStates();
        UpdateStatusBar();
      }
    }

    /// <summary>
    /// Respond to changing shape selection
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void EditorManager_ShapeSelectionChanged(object sender, ShapeSelectionChangedArgs e)
    {
      this.UpdateCopyPasteToolbar();
      UpdateSelectionSetButton();

      // Show properties panel if option is enabled
      if (EditorManager.Settings.ShowPropertiesPanelOnSelection)
        propertyPanel1.ShowDockable();
    }

    private void EditorManager_BeforeEditorAppShutdown(object sender, EventArgs e)
    {
      // Shutdown the unmanaged code part of the browser panel.
      assetBrowserPanel.DeInit();

      // deactivated for now, will come in a later release
      //collectionPanel.DeInit();
    }

    private void ApplicationLayout_ActiveLayoutChanged(object sender, LayoutManager.ActiveLayoutChangedArgs e)
    {
      UpdateStatusBar();
      UpdateLayoutButtonSelection();
    }

    private void ApplicationLayout_AvailableLayoutsChanged(object sender, EventArgs e)
    {
      UpdateLayoutButtonContent();
    }

    private void shapeCreatorPanel_SelectedCreatorObjectChanged(object sender, EventArgs e)
    {
      this.propertyPanel1.SelectedObject = ((ShapeCreatorPanel)sender).SelectedCreatorObject;
    }

    //////////////////////////////////////////////////////
    // Actions
    //////////////////////////////////////////////////////

    private void Actions_ActionEvent(object sender, ActionEventArgs e)
    {
      switch (e.state)
      {
        case ActionEventArgs.State.BeginAdd:
        case ActionEventArgs.State.BeginRedo:
        case ActionEventArgs.State.BeginUndo:
          EditorManager.Scene.Dirty = true; // not necessary anymore since each layer has a dirty flag that also sets it on the scene
          ToolBar_SaveScene.Enabled = true;
          ToolBar_SaveAll.Enabled = true;
          break;
        case ActionEventArgs.State.EndAdd:
        case ActionEventArgs.State.EndRedo:
        case ActionEventArgs.State.EndUndo:
        case ActionEventArgs.State.Clear:
          UpdateMainToolbarStates();
          break;
      }
    }

    void IVisibilityBuilder_OnVisibilityStatusChanged(object sender, EventArgs e)
    {
      ToolBar_CalcVisibility.Enabled = EditorManager.Scene!=null && EditorManager.VisibilityBuilder.Dirty;
      EditorManager.ActiveView.UpdateView(false);
    }

    void IVisibilityBuilder_OnVisibilityUseInEngineChanged(object sender, EventArgs e)
    {
      ToolBar_UseVisibility.Checked = EditorManager.VisibilityBuilder.UseInEngine;
      EditorManager.ActiveView.UpdateView(false);
    }

    void ResourceInfo_OnSelectedResourceChanged(object sender, EventArgs e)
    {
      vResourceViewer.Classes.ResourceInfo resInfo = (vResourceViewer.Classes.ResourceInfo)sender;

      if (resInfo == null)
        EditorManager.EngineManager.SetResourcePreview(IntPtr.Zero, null, false);
      else
        EditorManager.EngineManager.SetResourcePreview((IntPtr)resInfo.resourceID, resInfo.ParentManager.Name, true);
    }

    void ResourceInfo_OnAutomatedResourcePreview(object sender, EventArgs e)
    {
      foreach (Control page in resourceViewerPanel.ResourceViewWindow.tabControl.SelectedTab.Controls)
      {
        if (page is vResourceViewer.Controls.ResourceManagerTabPage)
        {
          EditorManager.Progress.ShowProgressDialog("Automatic Thumbnail Generation", true); // abortable

          vResourceViewer.Controls.ResourceManagerTabPage tabPage = page as vResourceViewer.Controls.ResourceManagerTabPage;

          int i = 0;
          int iCount = tabPage.listView_Resources.Items.Count;

          // begin automated thumbnail generation
          EditorManager.EngineManager.BeginThumbnailGeneration(); 

          foreach (ListViewItem listViewItem in tabPage.listView_Resources.Items)
          {
            if (listViewItem is vResourceViewer.Controls.ResourceManagerTabPage.ResourceListItem)
            {
              vResourceViewer.Controls.ResourceManagerTabPage.ResourceListItem item = listViewItem as vResourceViewer.Controls.ResourceManagerTabPage.ResourceListItem;

              EditorManager.Progress.StatusString = "Processing " + item.Info._name;
              Application.DoEvents();

              // automatically generate thumbnail and save to file
              EditorManager.EngineManager.DoThumbnailGeneration((IntPtr)item.Info.resourceID, item.Info.ParentManager.Name, true);
            }

            i++;
            EditorManager.Progress.Percentage = ((float)i / (float)iCount) * 100.0f;
            Application.DoEvents();

            if (EditorManager.Progress.WantsAbort)
              break;
          }
          
          // end automated thumbnail generation and trigger view update
          EditorManager.EngineManager.EndThumbnailGeneration();
          EditorApp.ActiveView.UpdateView(false);

          EditorManager.Progress.HideProgressDialog();

          // No further search necessary if ResourceManagerTabPage has been found
          break;
        }
      }
    }

    void resourceViewerPanel_VisibleChanged(object sender, EventArgs e)
    {
      // hide resource preview if panel isn't visible
      if (!resourceViewerPanel.Visible)
        EditorManager.EngineManager.SetResourcePreview(IntPtr.Zero, null, false);

    }



    #endregion

    #region Toolbars

    #region Toolbar helper functions

    private static void SetButtonsStatus(ToolStripItemCollection buttons, bool bStatus)
    {
      foreach (ToolStripItem button in buttons)
        button.Enabled = bStatus;
    }

    private void UpdateToolbars()
    {
      UpdateMainToolbarStates();
    }

    #endregion

    #region Main Toolbar

    //////////////////////////////////////////////////////
    // Main Toolbar
    //////////////////////////////////////////////////////

    static DateTime _lastCopyPasteToolbarUpdate = DateTime.Now;

    private void UpdateCopyPasteToolbar()
    {
      if (DateTime.Now - _lastCopyPasteToolbarUpdate < TimeSpan.FromMilliseconds(250))
        return;

      _lastCopyPasteToolbarUpdate = DateTime.Now;

      IEditorComponent component = EditorManager.ActiveComponent;
      string dataType = component.GetDataTypeName();
      Toolbar_Cut.Enabled = component.CanCut();
      Toolbar_Cut.ToolTipText = "Cut " + dataType;
      Toolbar_Copy.Enabled = component.CanCopy();
      Toolbar_Copy.ToolTipText = "Copy " + dataType;
      Toolbar_Delete.Enabled = component.CanDelete();
      Toolbar_Delete.ToolTipText = "Delete " + dataType;

      bool bCanPaste = component.CanPaste();
      bool bHasClipboard = bCanPaste && component.Clipboard != null && component.Clipboard.HasData();
      Toolbar_Paste.Enabled = bCanPaste;

      if (bHasClipboard)
      {
        Toolbar_Paste.ToolTipText = "Paste " + dataType + " (" + component.Clipboard.DataName + ")";
      }
      else
      {
        Toolbar_Paste.ToolTipText = "Paste " + dataType;
      }
    }

    private void UpdateSaveToolbar()
    {
      if (_lastActiveSaveWasSaveAll)
      {
        ToolBar_Save.Image = global::Editor.Properties.Resources.toolbar_saveall;
        ToolBar_Save.ToolTipText = "Save All";
      }
      else
      {
        ToolBar_Save.Image = global::Editor.Properties.Resources.toolbar_save;
        ToolBar_Save.ToolTipText = "Save Scene";
      }

      ToolBar_Save.Enabled = EditorManager.Scene.Dirty;
      ToolBar_SaveScene.Enabled = EditorManager.Scene.Dirty;
      ToolBar_SaveAll.Enabled = EditorManager.Scene.Dirty;

      // also enable the menu item of Save Scene, so that the hotkey works
      Menu_File_Save_Scene.Enabled = EditorManager.Scene.Dirty;
      Menu_File_Save_All.Enabled = EditorManager.Scene.Dirty;
      Menu_File_Save_SceneAs.Enabled = EditorManager.Scene.Dirty;
    }

    private void UpdateMainToolbarStates()
    {
      bool bHasProject = (EditorManager.Project != null);
      bool bHasScene = (EditorManager.Scene != null);
      
      if (selectionSetComboBox != null)
        selectionSetComboBox.Enabled = bHasScene;

      if (profileComboBox != null)
        profileComboBox.Enabled = bHasProject;
      if (assetPreviewModeButton != null)
        assetPreviewModeButton.Enabled = bHasProject;

      if (layoutComboBox != null)
        layoutComboBox.Enabled = true;

      if (!bHasScene)
      {
        SetButtonsStatus(mainToolBar.Items, false); // all buttons grayed out
        this.ToolBar_New.Enabled = true;
        this.ToolBar_NewScene.Enabled = bHasProject;
        this.ToolBar_Open.Enabled = true;
        this.ToolBar_Preferences.Enabled = true;
        this.ToolBar_Hotkeys.Enabled = true;

        if (selectionSetComboBox != null)
          selectionSetComboBox.Items.Clear();

        if (layoutComboBox != null)
          layoutComboBox.Enabled = true;

        return;
      }
      
      UpdateCopyPasteToolbar();
      UpdateSaveToolbar();

      Toolbar_Undo.Enabled = EditorManager.Actions.UndoCount > 0;
      Toolbar_Redo.Enabled = EditorManager.Actions.RedoCount > 0;
      ToolBar_Export.Enabled = bHasScene && !EditorManager.InPlayingMode;
      ToolBar_Panels.Enabled = bHasScene;
      ToolBar_Tools.Enabled = bHasScene;
      ToolBar_Resources.Enabled = bHasScene;
      ToolBar_Relight.Enabled = bHasScene;
      ToolBar_Visibility.Enabled = bHasScene;
      ToolBar_CalcVisibility.Enabled = bHasScene && EditorManager.VisibilityBuilder.Dirty;
      ToolBar_UseVisibility.Enabled = bHasScene;
      ToolBar_OpenAssetBrowser.Enabled = bHasScene;
    }

    #endregion

    #endregion

    #region Main Menu

    private void EnableAllMenuItems(ToolStripMenuItem parent)
    {
      parent.Enabled = true;
      foreach (ToolStripItem item in parent.DropDownItems)
      {
        if (item is ToolStripMenuItem)
        {
          EnableAllMenuItems((ToolStripMenuItem)item);
        }     
      }
    }

    private void Menu_DropDownClosed(object sender, System.EventArgs e)
    {
      if (sender is ToolStripMenuItem)
      {
        EnableAllMenuItems((ToolStripMenuItem)sender);
      }
    }

    #region Menu : File

    //////////////////////////////////////////////////////
    // File menu
    //////////////////////////////////////////////////////

    //Show File menu popup
    private void Menu_File_Popup(object sender, System.EventArgs e)
    {
      bool bHasProject = (EditorManager.Project != null);
      bool bHasScene = (EditorManager.Scene != null);
      bool bExportEnabled = bHasScene && !EditorManager.InPlayingMode;

      Menu_File_CloseProject.Enabled = bHasProject;
      Menu_File_Save_All.Enabled = bHasScene && EditorManager.Scene.Dirty;

      Menu_File_New_Scene.Enabled = bHasProject;
      Menu_File_Open_Scene.Enabled = true; //always enabled so we can open the scene directly
      Menu_File_Save_Scene.Enabled = bHasScene && EditorManager.Scene.Dirty;
      Menu_File_Save_SceneAs.Enabled = bHasScene;
      Menu_File_Export.Enabled = true;
      Menu_File_Export_Scene.Enabled = bExportEnabled;
      Menu_File_Export_LayersAsPrefabs.Enabled = bExportEnabled;
      Menu_File_Export_LayersAsPrefabsIntoFolder.Enabled = bExportEnabled;

      bool bBatchExportVisible = TestManager.Helpers.TestDataDirExists;

      Menu_File_Export_Separator.Visible = bBatchExportVisible;

      Menu_File_Export_BatchProject.Enabled = bHasProject;
      Menu_File_Export_BatchProject.Visible = bBatchExportVisible;

      Menu_File_Export_BatchDirectory.Enabled = true;
      Menu_File_Export_BatchDirectory.Visible = bBatchExportVisible;

      Menu_File_Scene_Settings.Enabled = bHasScene;
      Menu_File_Scene_Thumbnail.Enabled = bHasScene;
      Menu_File_Scene_MergeExternalLayers.Enabled = bHasScene && EditorManager.Scene.Zones.Count > 0;
      Menu_File_Scene_UniqueIDs.Enabled = bHasScene;
      Menu_File_Scene_ShapeLinkDebugFile.Enabled = bHasScene;
      Menu_File_Scene_Description.Enabled = bHasScene && EditorApp.Scene.V3DLayer != null;
      Menu_File_Project_OpenDirectory.Enabled = bHasProject;
      Menu_File_Project_Move.Enabled = bHasScene;
      Menu_File_Project_Move.Visible = false; // for now, disable this item  
    }

    //Show File->New menu popup
    private void Menu_File_New_Popup(object sender, System.EventArgs e)
    {
      bool bHasProject = (EditorManager.Project != null);

      Menu_File_New_Scene.Enabled = bHasProject;
    }

    // When no project is loaded we create a new project, otherwise a new scene
    private void ToolBar_New_ButtonClick(object sender, EventArgs e)
    {
      if (EditorManager.Project == null)
        Menu_File_New_Project_Click(sender, e);
      else
        Menu_File_New_Scene_Click(sender, e);
      return;
    }

    //File->New->Project
    private void Menu_File_New_Project_Click(object sender, System.EventArgs e)
    {
       // open new project
       NewProject();
    }
    
    /// <summary>
    /// Create a new project
    /// </summary>
    public void NewProject()
    {
      // Get the project directory that was used last time
      string projectDir = null;
      if (mruList_RecentProjects.Count > 0)
      {
        string projectPath = mruList_RecentProjects.LatestPathEntry;
        projectDir = Path.GetDirectoryName(projectPath);
      }

      //Create a new project
      CreateProjectDlg dlg = new CreateProjectDlg(projectDir);
      if (dlg.ShowDialog(this) == DialogResult.OK)
      {
        //Make sure the current project is saved
        if (EditorApp.PromptSaveProject() == DialogResult.Cancel) return;

        // close any currently loaded project
        if (EditorManager.Project != null)
          EditorManager.Project.Close();

        EditorProject proj = EditorProject.CreateProject(dlg.Dir, dlg.ProjectName);
        if (proj != null)
        {
          EditorManager.Project = proj;
          this.mruList_RecentProjects.Add(proj.PathName);

          if (dlg.CreateMeshesFolder)
            System.IO.Directory.CreateDirectory(proj.ProjectDir + "Meshes");
          if (dlg.CreateModelsFolder)
            System.IO.Directory.CreateDirectory(proj.ProjectDir + "Models");
          if (dlg.CreateConfigsFolder)
            System.IO.Directory.CreateDirectory(proj.ProjectDir + "Configs");
          if (dlg.CreateParticlesFolder)
            System.IO.Directory.CreateDirectory(proj.ProjectDir + "Particles");
          if (dlg.CreateScenesFolder)
            System.IO.Directory.CreateDirectory(proj.ProjectDir + "Scenes");
          if (dlg.CreateShadersFolder)
            System.IO.Directory.CreateDirectory(proj.ProjectDir + "Shaders");
          if (dlg.CreatePrefabsFolder)
            System.IO.Directory.CreateDirectory(proj.ProjectDir + "Prefabs");
          if (dlg.CreateSoundsFolder)
            System.IO.Directory.CreateDirectory(proj.ProjectDir + "Sounds");
        }
      }
      UpdateToolbars();
    }

    //File->New->Scene
    private void Menu_File_New_Scene_Click(object sender, System.EventArgs e)
    {
      // open new scene
      if (EditorManager.Project!=null)
        NewScene();
    }

    /// <summary>
    /// Create a new scene
    /// </summary>		
    public void NewScene()
    {
      System.Diagnostics.Debug.Assert(EditorManager.Project!=null);
      //Show our own file create dialog 
      CreateFileDlg fileDlg = new CreateFileDlg();
      fileDlg.Caption = "Creating a new Scene";
      fileDlg.Description = "Enter the name of the new scene file and select the directory to create it in. Then press OK to continue.";
      fileDlg.InitialDirectory = EditorManager.Project.ProjectDir;
      fileDlg.Ext = ".scene";
      fileDlg.Filter = new string[] {".scene"};
      if (fileDlg.ShowDialog() != DialogResult.OK)
        return;

      //Make sure the current scene is saved
      if (EditorApp.PromptSaveScene() == DialogResult.Cancel) return;

      // close any currently loaded scene
      if (EditorManager.Project != null && EditorManager.Project.Scene != null)
        EditorManager.Project.Scene.Close();

      //Create a new scene
      if (!EditorManager.Project.NewScene( EditorManager.Project.MakeRelative(fileDlg.FileName) ))
        return;

      // Open Scene Wizard dialog if enabled
      if (EditorManager.Settings.ShowSceneWizardOnSceneCreation)
      {
        SceneWizardDlg dlg = new SceneWizardDlg();
        dlg.Scene = (EditorScene) EditorManager.Project.Scene;
        dlg.ShowDialog();
      }

      // show warning dialog if mixed debug/release assemblies are loaded
      AssemblyHelper.CheckAssemblyConsistency();

      mruList_RecentScenes.Add(fileDlg.FileName);
    }

    /// <summary>
    /// Loads a project file and sets it as active project
    /// </summary>
    /// <param name="filename"></param>
    /// <param name="bPromptSceneLoad"></param>
    /// <returns></returns>
    public bool LoadProject(string filename, bool bPromptSceneLoad)
    {
      // TODO: Check this. The problem is, if the following loading
      // of a project fails, the previous project is already closed and thats
      // propably not what we want
      if (EditorManager.Project!=null)
      {
        //Make sure the current project is saved
        if (EditorApp.PromptSaveProject() == DialogResult.Cancel) return false;
        EditorManager.Project.Close();
      }

      EditorProject proj = EditorManager.ProjectFactory.CreateInstance(null) as EditorProject;
      if (proj == null)
      {
        EditorManager.ShowMessageBox("Project must be derived from class 'EditorProject'", "Failed to create project", MessageBoxButtons.OK, MessageBoxIcon.Error);
        return false;
      }

      //Should we do this here? This sends a broadcast and that isn't very useful
      // since nothing is initialized yet. We also don't have a post load 
      // callback.
      // But we might need the pointer to be set while loading.
      // What about changing the editor to not send events when setting the ,
      // Project member but having separate events. (ProjectChange/ProjectChanged)
      //EditorManager.Project = proj;

      //Load project
      if (proj.Load(filename))
      {
        mruList_RecentProjects.Add(filename);
        EditorManager.Project = proj;
        //project loaded, now prompt the user for the scene
        if (bPromptSceneLoad)
          Menu_File_OpenScene_Click(null, null);

        UpdateToolbars();
        return true;
      }
      else
      {
        mruList_RecentProjects.Remove(filename);
        EditorManager.ShowMessageBox("Unable to load project file " + filename, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        // EditorManager.Project = null;
      }
      UpdateToolbars();
      return false;
    }


    /// <summary>
    /// Loads the scene file and optionally reloads the project
    /// </summary>
    /// <param name="sceneName"></param>
    /// <param name="bForceLoadProject"></param>
    /// <returns></returns>
    public bool LoadScene(string sceneName, bool bForceLoadProject)
    {

      //show progress bar
      EditorManager.Progress.ShowProgressDialog("Loading Scene file");
      EditorManager.Progress.Percentage = 0.0f;
      EditorManager.Progress.StatusString = "Initialize project";

      this.Refresh(); // redraw all windows before actually do anything

      // close any currently loaded scene
      if ( EditorManager.Project != null && EditorManager.Project.Scene != null )
        EditorManager.Project.Scene.Close();

      if (EditorManager.Project==null || bForceLoadProject) // no project loaded
      {
        //Find the project file for the scene
        string projectFile = EditorProject.FindProjectFor(sceneName);
        if (projectFile == null)
        {
          EditorManager.ShowMessageBox("Unable to find the project file for the scene " + sceneName, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
          EditorManager.Progress.HideProgressDialog();
          mruList_RecentScenes.Remove(sceneName);
          return false;
        }

        if (EditorManager.Project!=null)
        {
          //Make sure the current project is saved
          if (EditorApp.PromptSaveProject() == DialogResult.Cancel) return false;
          EditorManager.Project.Close();
        }

        if (!LoadProject(projectFile, false))
        {
          EditorManager.Progress.HideProgressDialog();
          mruList_RecentScenes.Remove(sceneName);
          return false;
        }
      }

      EditorManager.Progress.Percentage = 10.0f; // project initialized

      // now open the new scene
      if (!EditorManager.Project.OpenScene( EditorManager.Project.MakeRelative(sceneName) ))
      {
        EditorManager.Progress.HideProgressDialog();
        mruList_RecentScenes.Remove(sceneName);
        return false;
      }

      // show warning dialog if mixed debug/release assemblies are loaded
      AssemblyHelper.CheckAssemblyConsistency();

      UpdateToolbars();

      EditorManager.Progress.StatusString = "Finishing...";
      EditorManager.ActiveView.UpdateView(true); // redraw so that all vertex buffers are cached
      EditorManager.Progress.Percentage = 100.0f; // then it is finished...

      EditorManager.Progress.HideProgressDialog();
      mruList_RecentScenes.Add(sceneName);
      EditorApp.Scene.ShowDescriptionAtStartup();

      UpdateToolbars();

      return true;
    }

    private void OpenProjectDialog(string directory)
    {
      //Show project open dialog
      FileDialog fileDlg = new OpenFileDialog();

      fileDlg.Title = "Open Project";
      fileDlg.InitialDirectory = directory;

      fileDlg.Filter = "Project files (*.Project)|*.Project";
      fileDlg.RestoreDirectory = true;
      if (fileDlg.ShowDialog() != DialogResult.OK)
        return;


      // now load the project
      if (LoadProject(fileDlg.FileName, true))
      {
        // Save directory for the next time
        EditorManager.Settings.LastProjectDirectory = (new FileInfo(fileDlg.FileName)).DirectoryName;
      }
    }
    
    // If no project is loaded, we open a project, otherwise a scene
    private void ToolBar_Open_ButtonClick(object sender, EventArgs e)
    {
      if (EditorManager.Project == null)
        Menu_File_OpenProject_Click(sender, e);
      else
        Menu_File_OpenScene_Click(sender, e);
      return;
    }
    
    //File->Open Project
    private void Menu_File_OpenProject_Click(object sender, System.EventArgs e)
    {
      OpenProjectDialog(EditorManager.Settings.LastProjectDirectory);
    }

    private void ToolBar_Open_DropDownOpening(object sender, EventArgs e)
    {
      mruList_RecentScenes.BuildMenuItemList(ToolBar_Open.DropDownItems, "[Scene] ");
    }

    //File->Close Project
    private void Menu_File_CloseProject_Click(object sender, System.EventArgs e)
    {
      //Make sure the project is saved
      if (EditorApp.PromptSaveProject() == DialogResult.Cancel)
        return;

      //Close the project
      EditorManager.Project.Close();
      UpdateToolbars();
    }
    
    //File->Open Scene
    private void Menu_File_OpenScene_Click(object sender, System.EventArgs e)
    {
      string sceneName; //scene file name

      if (EditorManager.Project!=null)
      {
        //We have a project. Show our own file open dialog 
        OpenFileDlg fileDlg = new OpenFileDlg();
        fileDlg.ShowBaseData = false;
        fileDlg.ThumbnailProvider = new StandardThumbnailProvider(new string[] { @"{0}.Layers\Thumbnail.bmp", @"{0}.Layers\Thumbnail.jpg" });

        fileDlg.Caption = "Opening a Scene File";
        fileDlg.Description = "Please select the scene file you want to open and press OK to continue";
        fileDlg.InitialDirectory = EditorManager.Project.ProjectDir;

        fileDlg.Filter = new string[] {".scene"};
        if (fileDlg.ShowDialog() != DialogResult.OK)
          return;

        //Make sure the current scene is saved
        if (EditorApp.PromptSaveScene() == DialogResult.Cancel) return;

        sceneName = fileDlg.FileName;
      }  
      else
      {
        // No project ? Show the normal file open dialog and try to
        // figure out which project to load for it.
        FileDialog fileDlg = new OpenFileDialog();

        fileDlg.Title = "Open Scene";
        fileDlg.InitialDirectory = EditorManager.Settings.LastSceneDirectory;

        fileDlg.Filter = "Scene files (*.Scene)|*.Scene" ;
        fileDlg.RestoreDirectory = true;
        if (fileDlg.ShowDialog() != DialogResult.OK)
          return;

        sceneName = fileDlg.FileName;
      }

      // loads the scene if possible
      if (LoadScene(sceneName, false))
      {
        // Save project and scene directory for the next time
        EditorManager.Settings.LastSceneDirectory = (new FileInfo(sceneName)).DirectoryName;
        EditorManager.Settings.LastProjectDirectory = EditorManager.Project.ProjectDir;
      }

      UpdateToolbars();
    }

    /// <summary>
    /// Last active saving state, either save all or save scene. It will be indicated by
    /// changing the button icon depending on what saving "mode" the user used the last time.
    /// </summary>
    private bool _lastActiveSaveWasSaveAll = false;

    private void ToolBar_Save_ButtonClick(object sender, EventArgs e)
    {
      if (_lastActiveSaveWasSaveAll)
        Menu_File_SaveAll_Click(sender, e);
      else
        Menu_File_SaveScene_Click(sender, e);
      return;
    }

    //File->Save All
    private void Menu_File_SaveAll_Click(object sender, System.EventArgs e)
    {
      if (EditorManager.Project == null)
        return;
      _lastActiveSaveWasSaveAll = true;
      EditorManager.TriggerSaveResources();
      EditorManager.Project.Save(); // this also saves the scene
      UpdateToolbars();
    }

    //File->Save Scene
    private void Menu_File_SaveScene_Click(object sender, System.EventArgs e)
    {
      _lastActiveSaveWasSaveAll = false;
      SaveScene();
      UpdateToolbars();
    }

    //File->Save Scene As
    private void Menu_File_SaveSceneAs_Click(object sender, System.EventArgs e)
    {      
      if (EditorManager.Scene==null)
        return;

      //Show our own file open dialog 
      CreateFileDlg fileDlg = new CreateFileDlg();
      fileDlg.Caption = "Saving the Scene";
      fileDlg.Description = "Enter the name of the scene and select the directory to save it in. Then press OK to continue.";
      fileDlg.InitialDirectory = EditorManager.Project.ProjectDir;
      fileDlg.Ext = ".scene";
      fileDlg.Filter = new string[] {".scene"};
      if (fileDlg.ShowDialog() != DialogResult.OK)
        return;
    
      string relFilename = EditorManager.Project.MakeRelative(fileDlg.FileName);
      if (!EditorManager.Scene.SaveAs(relFilename))
        return;

      mruList_RecentScenes.Add(fileDlg.FileName);

      UpdateToolbars();
      
      // also update titlebar
      EditorApp.UpdateTitleBar();
    }

    //helper function that saves the scene.
    private void SaveScene()
    {
      if (EditorManager.Scene != null)
      {
        bool bResult = EditorManager.Scene.Save();
        if (!bResult)
          EditorManager.ShowMessageBox("Scene saving failed: " + EditorManager.Scene.LastSaveErrorMsg + ". Please take a look at the engine log for more information.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
      }
    }

    // export the scene
    private void Menu_File_ExportScene_Click(object sender, System.EventArgs e)
    {
      // Export the scene
      if (EditorManager.Scene==null)
        return;
      EditorManager.GUI.UIUpdateLock++; //lock the UI, so there will be no updates during export
      EditorManager.Scene.ExportScene(null, true);
      EditorManager.GUI.UIUpdateLock--; //unlock the UI again
    }

    private void ToolBar_ExportQuick_Click(object sender, EventArgs e)
    {
      // Export the scene
      if (EditorManager.Scene == null)
        return;

      // Cache the "run after export" settings as we are setting it to true temporarily
      bool bRunAfterExport = EditorManager.Scene.CurrentExportProfile.RunAfterExport;
      EditorManager.Scene.CurrentExportProfile.RunAfterExport = true;
      
      // Do the export
      EditorManager.GUI.UIUpdateLock++; //lock the UI, so there will be no updates during export
      EditorManager.Scene.ExportScene(null, false);
      EditorManager.GUI.UIUpdateLock--; //unlock the UI again

      // Restore the export profile
      EditorManager.Scene.CurrentExportProfile.RunAfterExport = bRunAfterExport;
    }
    
    private void ToolBar_Tools_DropDownOpening(object sender, EventArgs e)
    {
      bool bInitialized = EditorManager.EngineManager.IsInitialized();
      bool bHasScene = (EditorManager.Scene != null);
      V3DLayer v3dLayer = bHasScene ? EditorApp.Scene.V3DLayer : null;

      ToolBar_SkyEditor.Enabled = bInitialized && v3dLayer != null && v3dLayer.Modifiable;
      ToolBar_TimeOfDayEditor.Enabled = bInitialized && v3dLayer != null && v3dLayer.Modifiable && v3dLayer.HasRendererNode && v3dLayer.EnableTimeOfDay;
      ToolBar_PostProcessor.Enabled = bInitialized && v3dLayer != null && v3dLayer.Modifiable && v3dLayer.HasRendererNode;
      ToolBar_SaveScreenshot.Enabled = bInitialized;
      ToolBar_SaveScreenshotAs.Enabled = bInitialized;

      // Show or hide last separator
      if (ToolBar_Tools.DropDownItems[ToolBar_Tools.DropDownItems.Count - 1] == ToolBar_Tools_LastSeparator)
        ToolBar_Tools_LastSeparator.Visible = false;
      else
        ToolBar_Tools_LastSeparator.Visible = true;
    }

    private void Menu_File_ExportLayersAsPrefabs_Click(object sender, EventArgs e)
    {
      ExportLayersAsPrefabs(null);
    }


    private void Menu_File_SceneSettings_Click(object sender, System.EventArgs e)
    {
      using (PropertyGridDlg dlg = new PropertyGridDlg("Scene Settings", "Setup scene specific preferences that get saved along with the scene"))
      {
        dlg.DataObject = EditorApp.Scene.Settings; // clones it
        if (dlg.ShowDialog()!=DialogResult.OK)
          return;
        // change the settings via an action:
        EditorManager.Actions.Add(new SceneSettingsChangedAction(EditorApp.Scene.Settings,(EditorSceneSettings)dlg.DataObject));
      }
    }

    private void Menu_File_SaveThumbnail_Click(object sender, EventArgs e)
    {
      if (EditorManager.Scene==null)
        return;

      string absFilename = Path.Combine(EditorManager.Scene.LayerDirectoryName, "Thumbnail.jpg");

      if (RCS.GetProvider() != null)
        RCS.GetProvider().EditFile(absFilename);

      if (!EditorManager.EngineManager.SaveViewThumbnail(absFilename, EditorSettingsBase.DefaultThumbnailResolution, EditorSettingsBase.DefaultThumbnailResolution))
        EditorManager.ShowMessageBox("Saving scene thumbnail failed.", "Error writing file", MessageBoxButtons.OK, MessageBoxIcon.Error);
      else
        EditorManager.ShowMessageBox(this, "Scene thumbnail saved.", "Scene thumbnail", MessageBoxButtons.OK, MessageBoxIcon.Information);

      if (RCS.GetProvider() != null)
        RCS.GetProvider().AddFile(absFilename, true);
    }


    private void menuItem_MoveSceneProject_Click(object sender, EventArgs e)
    {
      if (EditorApp.Scene == null)
        return;
      string oldProject = Path.Combine(EditorManager.Project.ProjectDir, EditorManager.Project.FileName);

      MoveSceneProjectDlg dlg = new MoveSceneProjectDlg();
      dlg.OldProjectPath = oldProject;
      if (dlg.ShowDialog(this) != DialogResult.OK)
        return;

      string newProject = dlg.NewProjectPath;
      if (!File.Exists(newProject))
      {
        EditorManager.ShowMessageBox("The specified project file does not exist", "Move Scene Project", MessageBoxButtons.OK, MessageBoxIcon.Error);
        return;
      }

      string oldProjectPath = Path.GetDirectoryName(oldProject);
      string newProjectPath = Path.GetDirectoryName(newProject);

      if (string.Compare(newProjectPath, oldProjectPath, true) == 0)
      {
        EditorManager.ShowMessageBox("Nothing to convert since new project path equals current one", "Move Scene Project", MessageBoxButtons.OK, MessageBoxIcon.Information);
        return;
      }

      if (!oldProjectPath.ToLower().StartsWith(newProjectPath.ToLower()))
      {
        EditorManager.ShowMessageBox("The new project path must be a parent folder of the current folder", "Move Scene Project", MessageBoxButtons.OK, MessageBoxIcon.Error);
        return;
      }

      try
      {
        EditorApp.Scene.OnRemoveAllEngineInstances();
        EditorApp.Scene.OnSceneRelocation(oldProjectPath, newProjectPath);
        EditorApp.Scene.OnCreateAllEngineInstances(null);

//        EditorManager.Project.ProjectDir = newProjectPath + @"\";
//        EditorManager.Project.FileName = Path.GetFileName(newProject);
      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex);
        EditorManager.ShowMessageBox("Migration failed due to an exception.\n\nDetailed info:\n" + ex.Message, "Move Scene Project", MessageBoxButtons.OK, MessageBoxIcon.Error);
        return;
      }

      EditorManager.ShowMessageBox("Migration to new project file was successful.\n\nPlease delete old .project file so loading a scene file always finds the new project.", "Move Scene Project", MessageBoxButtons.OK, MessageBoxIcon.Information);
    }


    private void menuItem_MergeLayersIntoZones_Click(object sender, EventArgs e)
    {
      if (EditorManager.Scene == null || EditorManager.Scene.Zones.Count == 0)
        return;
      ZoneCollection zones = new ZoneCollection();
      foreach (Zone zone in EditorManager.Scene.Zones)
        if (zone.ConsiderForSortIntoZones)
          zones.Add(zone);
      if (zones.Count==0)
        return;

      OpenFileDialog dlg = new OpenFileDialog();
      dlg.InitialDirectory = EditorManager.Project.ProjectDir;
      dlg.Filter = "Layer files (*.Layer)|*.Layer";
      dlg.Multiselect = true;
      dlg.Title = "Select one or more layer files from external folders that should be merged into the zones of the currect scenes";
      if (dlg.ShowDialog() != DialogResult.OK)
        return;
      string[] names = dlg.FileNames;
      if (names == null || names.Length==0)
        return;

      float fProgressStep = 50.0f / (float)names.Length;
      EditorManager.Progress.ShowProgressDialog("Merging shapes...");

      EditorManager.ActiveView.Gizmo.LockCtr++; // do not update
      ShapeTreeView.UIUpdateLock++;

      foreach (string name in names)
      {
        EditorManager.Progress.StatusString = "Loading Layer" + Path.GetFileName(name);
        Application.DoEvents();
        Layer layer = null;
        BinaryFormatter fmt = SerializationHelper.BINARY_FORMATTER;
        try
        {
          // open the layer in read-only mode
          FileStream fs = new FileStream(name, FileMode.Open, FileAccess.Read);
          layer = (Layer)fmt.Deserialize(fs);
          fs.Close();
        }
        catch (Exception ex)
        {
          EditorManager.DumpException(ex, true);
        }
        if (layer == null || layer.Root == null)
          continue;
        layer.OnCreateAllEngineInstances(null); // we need the engine instances, because otherwise the bounding boxs are not correct
        EditorManager.Progress.Percentage += fProgressStep;
        EditorManager.Progress.StatusString = "Merging Layer" + Path.GetFileName(name);
        Application.DoEvents();
        SortShapesIntoZonesAction action = new SortShapesIntoZonesAction(layer.Root.ChildCollection, zones);

        // layer cleanup
        layer.OnRemoveAllEngineInstances();
        EditorManager.EngineManager.Resources_PurgeUnusedResources();

        EditorManager.Actions.Add(action);
        EditorManager.Progress.Percentage += fProgressStep;
        Application.DoEvents();
      }
      EditorManager.Progress.HideProgressDialog();
      EditorManager.ActiveView.Gizmo.LockCtr--; // do not update
      ShapeTreeView.UIUpdateLock--;
    }


    private void menuItem_ValidateUniqueIDs_Click(object sender, EventArgs e)
    {
      if (EditorManager.Scene == null)
        return;
      RepairShapeIDVisitor repair = new RepairShapeIDVisitor();
      if (repair.CheckScene(EditorManager.Scene))
      {
        EditorManager.ShowMessageBox(this, "All shapes in this scene have unique IDs.\n\nEverything is OK.", "Scene validated",MessageBoxButtons.OK, MessageBoxIcon.Information);
        return;
      }
      if (EditorManager.ShowMessageBox("Two or more shapes with overlapping IDs have been found.\n\nRepair scene now?", "Problem(s) found", MessageBoxButtons.YesNo, MessageBoxIcon.Exclamation) == DialogResult.No)
        return;
      repair.RepairScene(EditorManager.Scene, true);
      EditorManager.ShowMessageBox(this, "IDs have been repaired. Please save scene now.\n\nIf lightmapping is used, the scene must be re-lit.", "Scene repaired", MessageBoxButtons.OK, MessageBoxIcon.Information);
    }

    private void menuItem_SaveShapeLinkFile_Click(object sender, EventArgs e)
    {
      if (EditorManager.Scene == null)
        return;
      ShapeLinksToTextFile writer = new ShapeLinksToTextFile();
      string filename = Path.Combine(EditorManager.Scene.LayerDirectoryName, "AllShapeLinks.txt");
      if (!writer.WriteToFile(filename))
        EditorManager.ShowMessageBox(this, "Failed to save text file\n\n"+filename, "Failed to save text file", MessageBoxButtons.OK, MessageBoxIcon.Error);

    }



    // project from MRU list
    private void Menu_File_Recent_Project_Click(object sender, MRUListArgs e)
    {
      //Make sure the current scene is saved
      if (EditorApp.PromptSaveScene() == DialogResult.Cancel)
        return;
      LoadProject(e.item.PathName,true);
    }

    // scene from MRU list
    private void Menu_File_Recent_Scene_Click(object sender, MRUListArgs e)
    {
      //Make sure the current scene is saved
      if (EditorApp.PromptSaveScene() == DialogResult.Cancel)
        return;
      string sceneName = e.item.PathName;
      //loads scene if possible
      LoadScene(sceneName,true);
    }
    
    // File->Exit
    private void Menu_File_Exit_Click(object sender, System.EventArgs e)
    { 
      EditorApp.ExitEditorApp(true);
    }

    private void Menu_File_ExportLayersAsPrefabsIntoFolder_Click(object sender, EventArgs e)
    {
      if (EditorManager.Project != null)
      {
        //We have a project. Show our own file open dialog
        OpenFileDlg fileDlg = new OpenFileDlg();
        fileDlg.BrowseDirectoriesOnly = true;
        fileDlg.Caption = "Choose a folder";
        fileDlg.Description = "Please select the folder you want to save the layers as prefabs to and press OK to continue";
        fileDlg.InitialDirectory = EditorManager.Project.ProjectDir;
        if (fileDlg.ShowDialog() != DialogResult.OK)
          return;

        ExportLayersAsPrefabs(fileDlg.FileName);
        UpdateToolbars(); //update toolbars in case the user saves the prefabs to the prefabs folder. Then the shapecreator panel must be also updated to show the new prefabs
      }
    }

    private void Menu_File_ExportBatchProject_Click(object sender, EventArgs e)
    {
      if (EditorManager.Project == null)
        return;

      // Close current scene(after saving)
      if (EditorManager.Scene != null)
      {
        if (EditorApp.PromptSaveScene() == DialogResult.Cancel)
          return;

        EditorManager.Scene.Close();
        UpdateToolbars();
      }

      string projectFile = EditorApp.Project.PathName;

      BatchExportDlg dialog = new BatchExportDlg();
      dialog.SelectedPath = Path.GetDirectoryName(EditorApp.Project.PathName);
      dialog.ShowDialog(this);
    }

    private void Menu_File_ExportBatchDirectory_Click(object sender, EventArgs e)
    {
      if (EditorManager.Project != null)
      {
        // Close current project (after saving)
        if (EditorApp.PromptSaveProject() == DialogResult.Cancel)
          return;

        //Close the project
        EditorManager.Project.Close();
        UpdateToolbars();
      }

      // Start from the checked out engine samples folder if possible
      string defaultPath = null;
      string sdkDir = System.Environment.GetEnvironmentVariable("VISION_SDK_DIR");

      if (sdkDir != null)
        defaultPath = Path.Combine(sdkDir, "EngineSamples\\Data\\Maps");

      // Dir does not exist? Try installed SDK path
      if (!Directory.Exists(defaultPath))
      {
        sdkDir = System.Environment.GetEnvironmentVariable("VISION_SDK");
        if(sdkDir != null)
          defaultPath = Path.Combine(sdkDir, "EngineSamples\\Data\\Maps");
      }

      // Dir does not exist? Set to null
      if (!Directory.Exists(defaultPath))
        defaultPath = null;
      

      BatchExportDlg dialog = new BatchExportDlg();
      dialog.SelectedPath = defaultPath;
      dialog.ShowDialog(this);
    }

    private void menuItemOpenProjectDirectory_Click(object sender, EventArgs e)
    {
      if (EditorManager.Project == null)
        return;

      Process.Start(EditorManager.Project.ProjectDir);
    }

    #endregion


    #region Helper functions


    /// <summary>
    /// helper function to save layers as prefabs
    /// </summary>
    /// <param name="folderName">absolute path to the folder where the prefabs should be saved in. Set string to null if you want to use the default folder</param>
    private void ExportLayersAsPrefabs(string folderName)
    {
      if (EditorManager.Scene == null)
        return;
      int iCount = 0;

      bool bAsBinary = true;

      try
      {
        foreach (Layer layer in EditorManager.Scene.Layers)
        {
          string prefabName;
          if (folderName == null) //if no foldername is used 
          {
            prefabName = Path.ChangeExtension(layer.AbsoluteLayerFilename, ".prefab");
          }
          else
          {
            prefabName = Path.Combine(folderName, layer.LayerName + ".prefab");
          }
          PrefabDesc prefab = new PrefabDesc(prefabName);
          prefab.AutoSaveBinary = true; //activate autosave per default [#2112]
          prefab.Description = layer.Description;
          prefab.CreateFromInstances(layer.Root.ChildCollection, Vector3F.Zero, true, true);
          if (prefab.SaveToFile(null))
            iCount++;

          if (bAsBinary)
          {
            SceneExportInfo info = new SceneExportInfo();
            info.ExportType = SceneExportInfo.ExportType_e.VPrefab;
            info.AbsoluteFilename = EditorManager.Project.MakeAbsolute(prefab.BinaryFilename);
            info.RelevantShapes = SceneExportInfo.GetRelevantExportShapes(layer.Root);

            ExportHelper.StartPrefabExport(info);
            layer.Root.OnExportRecursive(info);
            ExportHelper.EndPrefabExport(info);
          }
        }
      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex);
        EditorManager.ShowMessageBox(this, "Failed to export layers. Detailed message:\n\n" + ex.Message, "Export Layers as Prefabs", MessageBoxButtons.OK, MessageBoxIcon.Error);
        return;
      }
      if (iCount == 0)
        EditorManager.ShowMessageBox(this, "No layer valid for export", "Export Layers as Prefabs", MessageBoxButtons.OK, MessageBoxIcon.Information);
      else
        EditorManager.ShowMessageBox(this, iCount.ToString() + " Layer(s) exported successfully", "Export Layers as Prefabs", MessageBoxButtons.OK, MessageBoxIcon.Information);
    }

    #endregion

    #region Menu : Edit

    //////////////////////////////////////////////////////
    // Edit menu
    //////////////////////////////////////////////////////

    //Show Edit menu popup
    private void Menu_Edit_DropDownOpening(object sender, EventArgs e)
    {
      bool bHasScene = (EditorManager.Scene != null);

      if (EditorManager.Actions.UndoCount>0)
      {
        Menu_Edit_Undo.Text = "Undo ("+EditorManager.Actions.UndoShortName+")";
        Menu_Edit_Undo.Enabled = true;
      } 
      else
      {
        Menu_Edit_Undo.Text = "Undo";
        Menu_Edit_Undo.Enabled = false;
      }
      
      if (EditorManager.Actions.RedoCount>0)
      {
        Menu_Edit_Redo.Text = "Redo ("+EditorManager.Actions.RedoShortName+")";
        Menu_Edit_Redo.Enabled = true;
      } 
      else
      {
        Menu_Edit_Redo.Text = "Redo";
        Menu_Edit_Redo.Enabled = false;
      }

      // copy+paste
      IEditorComponent component = EditorManager.ActiveComponent;

      string dataType = component.GetDataTypeName();
      Menu_Edit_Cut.Enabled = component.CanCut();
      Menu_Edit_Cut.Text = "Cut " + dataType;
      Menu_Edit_Copy.Enabled = component.CanCopy();
      Menu_Edit_Copy.Text = "Copy " + dataType;
      Menu_Edit_Delete.Enabled = component.CanDelete();
      Menu_Edit_Delete.Text = "Delete " + dataType;

      bool bCanPaste = component.CanPaste();
      Menu_Edit_Paste.Enabled = bCanPaste;
      Menu_Edit_Paste.Text = "Paste " + dataType;

      bool bHasClipboard = bCanPaste && component.Clipboard != null && component.Clipboard.HasData();
      if (bHasClipboard)
      {
        Menu_Edit_Paste.Text += " (" + component.Clipboard.DataName + ")";
      }

      Menu_Edit_FindShapes.Enabled = bHasScene;
      Menu_Edit_FindDuplicates.Enabled = bHasScene;
    }

    //Edit->Undo
    private void Menu_Edit_Undo_Click(object sender, System.EventArgs e)
    {
      EditorManager.DoUndo();
    }

    //Edit->Redo
    private void Menu_Edit_Redo_Click(object sender, System.EventArgs e)
    {
      EditorManager.DoRedo();
    }



    private void Menu_Edit_Cut_Click(object sender, System.EventArgs e)
    {
      EditorManager.DoCut();
    }

    private void Menu_Edit_Copy_Click(object sender, System.EventArgs e)
    {
      EditorManager.DoCopy();
    }

    private void Menu_Edit_Paste_Click(object sender, System.EventArgs e)
    {
      EditorManager.DoPaste();
    }

    private void Menu_Edit_Delete_Click(object sender, System.EventArgs e)
    {
      EditorManager.DoDelete();
    }
 
    private void Menu_Edit_FindShapes_Click(object sender, System.EventArgs e)
    {
      shapeSearchPanel.Show();
    }

    private void Menu_Edit_FindDuplicates_Click(object sender, EventArgs e)
    {
      shapeDuplicatesPanel.Show();
//      FindDuplicatesPanel dlg = new FindDuplicatesPanel();
//      dlg.ShowDialog(this);
    }


    #endregion

    #region Menu : View

    private void Menu_View_Camera_Origin_Click(object sender, System.EventArgs e)
    {
      EditorApp.ActiveView.SetCameraPosition(new Vector3F(0, 0, 0));
      EditorApp.ActiveView.SetCameraRotation(new Vector3F(0, 0, 0));
      EditorApp.ActiveView.UpdateView(false);
    }

    private void Menu_View_Camera_ShapeOrigin_Click(object sender, System.EventArgs e)
    {
      if (EditorApp.Scene==null)
        return;
      EditorApp.ActiveView.SetCameraPosition(EditorApp.Scene.CurrentShapeSpawnPosition);
      EditorApp.ActiveView.SetCameraRotation(new Vector3F(0, 0, 0));
      EditorApp.ActiveView.UpdateView(false);
    }

    private void Menu_View_Camera_Selection_Click(object sender, System.EventArgs e)
    {
      if (!EditorApp.ActiveView.Gizmo.Visible) // nothing selected
        return;
      Vector3F newPos = EditorApp.ActiveView.Gizmo.Position; // average selection pos
      Vector3F camDir = EditorApp.ActiveView.CameraDirection;
      EditorApp.ActiveView.SetCameraPosition(newPos - camDir * 50.0f);
      EditorApp.ActiveView.UpdateView(false);
    }

    #endregion

    #region Menu : Create

    private void Menu_Create_Popup(object sender, System.EventArgs e)
    {
      bool bHasScene = EditorManager.Scene != null;
      Menu_Create_NewLayer.Enabled = bHasScene;
      Menu_Create_NewZone.Enabled = bHasScene;

      // remove all items after the separator
      ToolStripItemCollection items = Menu_Create.DropDownItems;
      int iIndex = items.IndexOf(Menu_Create_Separator);
      while (items.Count > iIndex + 1)
        items.RemoveAt(iIndex + 1);
      // and fill again with creators
      EditorManager.BuildCreateMenu(this.Menu_Create);
    }

    private void menu_CreateNewLayer_Click(object sender, EventArgs e)
    {
      if (EditorManager.Scene != null)
        Layer.CreateLayer();
    }

    private void menu_CreateNewZone_Click(object sender, EventArgs e)
    {
      if (EditorManager.Scene != null)
        Zone.CreateZone();
    }

    private void menu_CreateZoneGrid_Click(object sender, EventArgs e)
    {
      if (EditorManager.Scene == null)
        return;

      ZoneGridProperties properties = new ZoneGridProperties();
      properties.Area = EditorManager.Scene.SceneExtents;

      using (PropertyGridDlg dlg = new PropertyGridDlg("Create a grid of zones", "Procedurally generate a grid of m times n streaming zones"))
      {
        dlg.DataObjectNoClone = properties;
        if (dlg.ShowDialog() != DialogResult.OK)
          return;

        CreateZoneGridAction action = new CreateZoneGridAction(properties);
        EditorManager.Actions.Add(action);
      }
    }

    #endregion

    #region Menu : Play
    
    private void Menu_Play_Popup(object sender, System.EventArgs e)
    {
      if (EditorManager.Scene == null)
      {
        Menu_Play_Animate.Enabled = Menu_Play_RunInEditor.Enabled = Menu_Play_PlayTheGame.Enabled = false;
        return;
      }
      else
      {
        Menu_Play_Animate.Enabled = Menu_Play_RunInEditor.Enabled = Menu_Play_PlayTheGame.Enabled = true;
      }

      if (EditorManager.EditorMode==EditorManager.Mode.EM_NONE)
      {
        Menu_Play_Animate.Text = "Animate";
        Menu_Play_RunInEditor.Enabled = true;
        Menu_Play_PlayTheGame.Enabled = true;
      } 
      else
      {
        Menu_Play_Animate.Text = "Stop";
        Menu_Play_RunInEditor.Enabled = false;
        Menu_Play_PlayTheGame.Enabled = false;
      }
    }

    private void Menu_Play_Animate_Click(object sender, System.EventArgs e)
    {
      enginePanel1.animateToolStripMenuItem_Animate_Click(sender, e);    
    }

    private void Menu_Play_RunInEditor_Click(object sender, System.EventArgs e)
    {
      enginePanel1.runInEditorToolStripMenuItem_RunInEditor_Click(sender, e);
    }

    private void Menu_Play_PlayTheGame_Click(object sender, System.EventArgs e)
    {
      enginePanel1.playTheGameToolStripMenuItem_PlayTheGame_Click(sender, e);
    }

    private void Menu_Play_TimeScale_Popup(object sender, System.EventArgs e)
    {
      float fSpeed = EditorManager.AnimationSpeed;
      Menu_Play_TS_10.Checked = (fSpeed==0.10f);
      Menu_Play_TS_25.Checked = (fSpeed==0.25f);
      Menu_Play_TS_50.Checked = (fSpeed==0.50f);
      Menu_Play_TS_100.Checked = (fSpeed==1.00f);
      Menu_Play_TS_200.Checked = (fSpeed==2.00f);
      Menu_Play_TS_500.Checked = (fSpeed==5.00f);
    }

    private void Menu_Play_TS_100_Click(object sender, System.EventArgs e)
    {
      EditorManager.AnimationSpeed = 1.0f;
    }

    private void Menu_Play_TS_10_Click(object sender, System.EventArgs e)
    {
      EditorManager.AnimationSpeed = 0.10f;
    }

    private void Menu_Play_TS_25_Click(object sender, System.EventArgs e)
    {
      EditorManager.AnimationSpeed = 0.25f;
    }

    private void Menu_Play_TS_50_Click(object sender, System.EventArgs e)
    {
      EditorManager.AnimationSpeed = 0.50f;
    }

    private void Menu_Play_TS_200_Click(object sender, System.EventArgs e)
    {
      EditorManager.AnimationSpeed = 2.0f;
    }

    private void Menu_Play_TS_500_Click(object sender, System.EventArgs e)
    {
      EditorManager.AnimationSpeed = 5.0f;
    }

    #endregion

    #region Menu : Help

    // Help->iHelp System
    private void Menu_Help_iHelp_Click(object sender, System.EventArgs e)
    {
      Menu_Help_iHelp.Checked = !Menu_Help_iHelp.Checked;
    }

    // Help->About this Scene
    private void Menu_File_Scene_Description_Click(object sender, System.EventArgs e)
    {
      EditorApp.Scene.ShowDescription();
    }

    // Help->About
    private void Menu_Help_About_Click(object sender, System.EventArgs e)
    {
      AboutDialog dlg = new AboutDialog();
      dlg.ShowDialog();
    }

    #endregion

    #region Menu : Run Tests

    private void menu_Tests_RunTests_Click(object sender, System.EventArgs e)
    {     
      // prevent from accidentally running the tests
      if (EditorManager.Scene!=null)
      {
        if (EditorManager.ShowMessageBox("A scene file is still open. Run tests anyway?","Run tests",MessageBoxButtons.YesNo,MessageBoxIcon.Question)==DialogResult.No)
          return;
        if (EditorApp.PromptSaveScene() == DialogResult.Cancel) return;
        EditorManager.Project.Close();
      }
      EditorManager.Scene = null;
      EditorManager.Project = null;

      if ( TestManager.ExecuteTests(true,true))
      {
        if (!TestManager.TestsCancelled)
          EditorManager.ShowMessageBox("All tests successful","Tests");
      }
      else
      {
        string msg = "Tests failed";
        EditorManager.ShowMessageBox(msg,"Tests");
      }
    }


    private void menu_Tests_VideoSize_Click(object sender, System.EventArgs e)
    {
      this.WindowState = System.Windows.Forms.FormWindowState.Normal;
      this.Size = new Size(960,720);
    }


    #endregion

    #region Menu : Engine

    private void Menu_Engine_Popup(object sender, System.EventArgs e)
    {
      bool bInitialized = EditorManager.EngineManager.IsInitialized();
      bool bHasScene = (EditorManager.Scene!=null);
      V3DLayer v3dLayer = null;
      if (bHasScene)
        v3dLayer = EditorApp.Scene.V3DLayer;

      Menu_Engine_EditSky.Enabled = bInitialized && v3dLayer != null && v3dLayer.Modifiable;
      Menu_Engine_EditTimeOfDay.Enabled = bInitialized && v3dLayer != null && v3dLayer.Modifiable && v3dLayer.HasRendererNode && v3dLayer.EnableTimeOfDay;
      Menu_Engine_EditPostProcessors.Enabled = bInitialized && v3dLayer != null && v3dLayer.Modifiable && v3dLayer.HasRendererNode;
      Menu_Engine_EditFog.Enabled = bInitialized && v3dLayer != null && v3dLayer.Modifiable;
      Menu_Engine_ProfileSettings.Enabled = bInitialized && v3dLayer != null;
      Menu_Engine_ManifestSettings.Enabled = bInitialized && EditorManager.Project != null;
      Menu_Engine_ReloadResources.Enabled = bHasScene;
      Menu_Engine_PurgeResources.Enabled = bHasScene;
      Menu_Engine_SaveScreenshot.Enabled = bHasScene;
      Menu_Engine_SaveScreenshotAs.Enabled = bHasScene;
      Menu_Engine_ShowResources.Enabled = bInitialized;
      Menu_Options_ShowPlugins.Enabled = true;
      
      Menu_Engine_CalculateVisibility.Enabled = bHasScene && EditorManager.VisibilityBuilder.Dirty;
      Menu_Engine_UseVisibility.Enabled = bHasScene;
      Menu_Engine_UseVisibility.Checked = EditorManager.VisibilityBuilder.UseInEngine;
    }

    private void Menu_Engine_EditSky_Click(object sender, System.EventArgs e)
    {
      V3DLayer layer = null;
      if (EditorManager.Scene!=null)
        layer = EditorApp.Scene.V3DLayer;
      if (layer==null || !layer.Modifiable)
        return;

      SkyConfigDlg dlg = new SkyConfigDlg();
      dlg.SkyConfig = layer.SkyConfig; // this assignment will clone the config

      if (dlg.ShowDialog()==DialogResult.OK)
      {
        EditorManager.Actions.Add(new SkyConfigChangedAction(layer,dlg.SkyConfig));
      } 
      else
      {
        layer.SkyConfig.Update(); // restore old sky
      }

      dlg.Dispose();
    }


    private void Menu_Engine_EditTimeOfDay_Click(object sender, EventArgs e)
    {
      V3DLayer layer = null;
      if (EditorManager.Scene != null)
        layer = EditorApp.Scene.V3DLayer;
      if (layer == null || !layer.Modifiable)
        return;

      TimeOfDayDlg dlg = new TimeOfDayDlg();
      dlg.TimeOfDay = layer.TimeOfDay; // this assignment will clone the config
      if (dlg.ShowDialog() == DialogResult.OK)
      {
        // Get changed time of day config and apply it to the layer via the appropriate action.
        EditorManager.Actions.Add(new TimeOfDayChangedAction(layer, (TimeOfDay)dlg.TimeOfDay.Clone()));
      }
      else
      {
        // Reapply original config to the engine.
        EditorManager.RendererNodeManager.UpdateTimeOfDay(layer.TimeOfDay);
      }
    }

    private void Menu_Engine_EditFog_Click(object sender, EventArgs e)
    {
      V3DLayer layer = null;
      if (EditorManager.Scene != null)
        layer = EditorApp.Scene.V3DLayer;
      if (layer == null || !layer.Modifiable)
        return;

      FogSetup oldSetup = layer.FogSetup.Clone() as FogSetup;

      bool bDeferred = (layer.RendererNodeClass == IRendererNodeManager.RENDERERNODECLASS_DEFERRED);
      bool bForward  = (layer.RendererNodeClass == IRendererNodeManager.RENDERERNODECLASS_FORWARD);
      bool bIR = (layer.RendererNodeClass == "VInfraredRendererNode");

      FogDlg dlg = new FogDlg(layer.FogSetup, bDeferred || bIR, bDeferred || bForward || bIR);

      if (dlg.ShowDialog() != DialogResult.OK)
      {
        // Revert changes to original state.
        layer.FogSetup = oldSetup;
        EditorManager.EngineManager.SetFogParameters(layer.FogSetup);
      }
      else
      {
        // Get changed fog setup and apply it to the layer via the appropriate action.
        EditorManager.Actions.Add(new FogSetupChangedAction(layer, (FogSetup)dlg.Setup.Clone()));
        layer.Modified = true;
      }
    }

    private void Menu_Engine_EditPostProcessors_Click(object sender, EventArgs e)
    {
      V3DLayer layer = null;
      if (EditorManager.Scene != null)
        layer = EditorApp.Scene.V3DLayer;
      if (layer == null || !layer.Modifiable)
        return;

      ConfigureRendererComponentsDlg dlg = new ConfigureRendererComponentsDlg();
      ShapeComponentCollection oldConfig = layer.Postprocessors.DeepClone(true);
      dlg.RendererComponents = oldConfig.DeepClone(true);

      if (dlg.ShowDialog() == DialogResult.OK)
        EditorManager.Actions.Add(new PostProcessorsChangedAction(layer, oldConfig, dlg.RendererComponents));
      else
        layer.Postprocessors = oldConfig;

      EditorManager.ActiveView.UpdateView(false);
    }

    private void Menu_Engine_ProfileSettings_Click(object sender, EventArgs e)
    {
      if (EditorManager.Project != null)
      {
        bool bHasScene = (EditorManager.Scene != null);
        V3DLayer v3dLayer = null;
        if (bHasScene)
          v3dLayer = EditorApp.Scene.V3DLayer;
        bool bModifiable = (v3dLayer == null) ? false : v3dLayer.Modifiable;

        AssetManagerManaged.ProfileSettingsDlg profileSettingsDlg = new ProfileSettingsDlg(bModifiable);

        if (profileSettingsDlg.ShowDialog() != DialogResult.OK)
          return;
      }
    }

    private void Menu_Engine_ManifestSettings_Click(object sender, EventArgs e)
    {
      if (EditorManager.Project != null)
      {
        AssetManagerManaged.ManifestSettingsDlg manifestSettingsDlg = new ManifestSettingsDlg();
        manifestSettingsDlg.ShowDialog();
      }
    }

    private void menu_CalculateVisibility_Click(object sender, EventArgs e)
    {
      if (EditorManager.Scene == null)
        return;
      EditorManager.VisibilityBuilder.Build(EditorManager.Scene, null);
    }

    private void menu_UseVisibility_Click(object sender, EventArgs e)
    {
      if (EditorManager.Scene == null)
        return;
      EditorManager.VisibilityBuilder.UseInEngine = !EditorManager.VisibilityBuilder.UseInEngine;
    }

    private void Menu_Engine_ReloadResources_Click(object sender, System.EventArgs e)
    {
      if (EditorManager.Scene == null)
        return;
      EditorApp.EngineManager.Resources_ReloadUpdatedResources();
      EditorManager.ActiveView.UpdateView(false);
    }

    private void Menu_Engine_ShowResources_Click(object sender, System.EventArgs e)
    {
      EngineResourceDlg dlg = new EngineResourceDlg();
      dlg.ShowDialog();
    }

    private void Menu_Engine_PurgeResources_Click(object sender, EventArgs e)
    {
      int iPurged = EditorApp.EngineManager.Resources_PurgeUnusedResources();
    }

    private void Menu_Engine_Screenshot_Click(object sender, System.EventArgs e)
    {
      if (!EditorManager.EngineManager.IsInitialized())
        return;
      
      try
      {
        EditorManager.EngineManager.SaveScreenShot(null);
        EditorApp.ActiveView.UpdateView(true); // redraws view port in order to cause a back buffer swap
        EditorManager.ShowMessageBox(this, "Screenshot saved.", "Saving screenshot", MessageBoxButtons.OK, MessageBoxIcon.Information);
      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex);
        EditorManager.ShowMessageBox(this, "Saving screenshot failed.\n\nDetailed message:\n" + ex.Message, "Saving screenshot", MessageBoxButtons.OK, MessageBoxIcon.Error);
      }
    }

    private void Menu_Engine_ScreenshotAs_Click(object sender, System.EventArgs e)
    {
      if (!EditorManager.EngineManager.IsInitialized())
        return;
      SaveFileDialog dlg = new SaveFileDialog();
 
      dlg.Filter = "BMP files (*.bmp)|*.bmp|JPEG files (*.jpg)|*.jpg"  ;
      dlg.FilterIndex = 1;
      dlg.RestoreDirectory = true;
      dlg.CheckPathExists = true;
      dlg.AddExtension = true;
      dlg.DefaultExt = "bmp";
      dlg.OverwritePrompt = true;
      dlg.Title = "Save Screenshot";

      if(dlg.ShowDialog() == DialogResult.OK)
      { 
        try
        {
          EditorManager.EngineManager.SaveScreenShot(dlg.FileName);
          EditorApp.ActiveView.UpdateView(true); // redraws view port in order to cause a back buffer swap
          EditorManager.ShowMessageBox(this, "Screenshot saved to:\n" + dlg.FileName, "Saving screenshot", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }
        catch (Exception ex)
        {
          EditorManager.DumpException(ex);
          EditorManager.ShowMessageBox(this, "Saving screenshot failed.\n\nDetailed message:\n" + ex.Message, "Saving screenshot", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
      }
    }

    #endregion

    #region Menu : Tools

    private void Menu_Tools_Popup(object sender, System.EventArgs e)
    {
      bool bInitialized = EditorManager.EngineManager.IsInitialized();
      bool bHasScene = (EditorManager.Scene!=null);
      Menu_StaticLighting_EditSettings.Enabled = bHasScene;
      Menu_StaticLighting_CalculateLighting.Enabled = bHasScene;
      Menu_StaticLighting_ResetLighting.Enabled = bHasScene;
    }

    private void Menu_Engine_UpdateLighting_Click(object sender, System.EventArgs e)
    {
      if (EditorManager.Scene==null)
        return;
      EditorApp.Scene.UpdateStaticLighting(true,null,true); // with dialog
    }

    private void Menu_Tools_CalculateLighting_Click(object sender, System.EventArgs e)
    {
      if (EditorManager.Scene==null)
        return;
      EditorApp.Scene.UpdateStaticLighting(false,null,true); // without dialog
    }

    private void Menu_Tools_ResetLighting_Click(object sender, EventArgs e)
    {
      if (EditorManager.Scene == null)
        return;
      if (EditorManager.ShowMessageBox(this, "Resetting the static lighting renames the .lit file to name.lit.bak and\ndefaults to constant scene color.\n\nAre you sure you want to continue?",
        "Reset Static Lighting", MessageBoxButtons.YesNo, MessageBoxIcon.Question) != DialogResult.Yes)
        return;

      EditorApp.Scene.ResetStaticLighting();

      IScene.SendShaderResourceChangedEvent(new EventArgs());
    }

    private void Menu_Lighting_ActiveTool_Popup(object sender, EventArgs e)
    {
      Menu_StaticLighting_ActiveTool.DropDownItems.Clear();
      foreach (IStaticLightingTool tool in EditorManager.SupportedLightingTools)
      {
        ToolStripMenuItem item = new ToolStripMenuItem(tool.Name, null, new EventHandler(Menu_Lighting_ActiveTool_Selected));
        item.Tag = tool;
        item.Checked = tool == EditorManager.LightingTool;
        Menu_StaticLighting_ActiveTool.DropDownItems.Add(item);
      }
      //
    }

    private void Menu_Lighting_ActiveTool_Selected(object sender, EventArgs e)
    {
      ToolStripItem item = (ToolStripItem)sender;
      EditorManager.LightingTool = item.Tag as IStaticLightingTool;
    }

    #endregion

    #region Menu : Extras

    private void menu_Extras_Settings_Click(object sender, System.EventArgs e)
    {
      EditorSettingsDlg dlg = new EditorSettingsDlg();
      dlg.Settings = EditorManager.Settings; // clones it
      if (dlg.ShowDialog()!=DialogResult.OK)
        return;
      EditorManager.Settings = dlg.Settings; // sets new one
      if (EditorManager.ActiveView != null)
        EditorManager.ActiveView.UpdateView(false);
      if (EditorManager.Project != null)
        EditorManager.EngineManager.UpdateMeshHighlightingTechnique();
      // Update the property grid as some settings affect what is shown there.
      IScene.SendPropertyChangedEvent(new PropertyChangedArgs(this, ""));

      SaveSettings(); // to file
    }

    private void menu_Extras_Hotkeys_Click(object sender, System.EventArgs e)
    {
      ShortCutSetupDlg dlg = new ShortCutSetupDlg();
      dlg.ShortCuts = EditorManager.ShortCuts;
      if (dlg.ShowDialog()!=DialogResult.OK)
        return;
      EditorManager.ShortCuts = dlg.ShortCuts;
    }

    private void Menu_Engine_ShowPlugins_Click(object sender, System.EventArgs e)
    {
      ShowPluginsDlg dlg = new ShowPluginsDlg();
      dlg.ShowDialog();
    }


    private void Menu_Engine_ManagePlugins_Click(object sender, EventArgs e)
    {
      if (EditorManager.Scene != null)
      {
        EditorManager.Scene.OnCreateAllEngineInstances(null);

        PluginLoaderDlg dlg = new PluginLoaderDlg();
        dlg.ShowDialog();
      }
      else
      {
        EditorManager.ShowMessageBox(this, "Please load a scene first before trying to manage the loaded plugins.");
      }
    }


    private void menu_Extras_Popup(object sender, EventArgs e)
    {
      bool bIsSceneLoaded = (EditorManager.Scene != null);
      
      //this.Menu_Options_ManagePlugins.Enabled = bIsSceneLoaded;
    }

    #endregion

    #region Menu : Remote

    private void SetRemoteCommonDirectory()
    {
      if ( EditorManager.RemoteManagerXbox360 != null )
      {
        if (EditorManager.Settings.RemoteFileSharingXbox360)
        {
          EditorManager.RemoteManagerXbox360.SetSharedDirectory(@"net:\smb\"+EditorManager.Settings.RemoteFileSharingCommonDirectoryXbox360);
        }
        else
        {
          EditorManager.RemoteManagerXbox360.SetSharedDirectory(@"D:\Data\Vision\Base");
        }
      }
    }

    private void Menu_Remote_Xbox360_Click(object sender, System.EventArgs e)
    {
      if (EditorManager.RemoteManagerXbox360 == null)
        return;

      if (EditorManager.RemoteManagerXbox360.IsConnected())
      {
        EditorManager.RemoteManagerXbox360.Disconnect();
      }
      else
      {
        String targetMachine = EditorManager.Settings.RemoteMachineNameXbox360;

        if (targetMachine == String.Empty)
        {
          EditorManager.ShowMessageBox(this, "Please specify the name of the remote Xbox360 in the vForge settings before trying to connect.");
          return;
        }

        if (!EditorManager.RemoteManagerXbox360.Connect(targetMachine))
          EditorManager.ShowMessageBox(this, "Could not connect to remote Xbox360 " + targetMachine +".\r\nPlease make sure that the remote Xbox360 defined in the settings is switched on and connected to the network.\r\n" + EditorManager.RemoteManagerXbox360.GetLastError());
      }
    }

    private void Menu_Remote_Xbox360_Launch_Click(object sender, System.EventArgs e)
    {
      if (EditorManager.RemoteManagerXbox360 == null)
        return;

      if (!EditorManager.RemoteManagerXbox360.IsConnected())
        return;

      if (EditorManager.Settings.RemoteApplicationXbox360 == String.Empty)
        return;

      if (!EditorManager.RemoteManagerXbox360.LaunchApp(EditorManager.Settings.RemoteApplicationXbox360))
        EditorManager.ShowMessageBox(this, "Could not launch client application on Xbox360:\r\n" + EditorManager.RemoteManagerXbox360.GetLastError());
    }

    private void Menu_Remote_PS3_Click(object sender, System.EventArgs e)
    {
      if (EditorManager.RemoteManagerPS3 == null)
        return;

      if (EditorManager.RemoteManagerPS3.IsConnected())
      {
        EditorManager.RemoteManagerPS3.Disconnect();
      }
      else
      {
        String targetMachine = EditorManager.Settings.RemoteMachineNamePS3;

        if (targetMachine == String.Empty)
        {
          EditorManager.ShowMessageBox(this, "Please specify the name of the remote PS3 in the vForge settings before trying to connect.");
          return;
        }
        
        if (!EditorManager.RemoteManagerPS3.Connect(targetMachine))
          EditorManager.ShowMessageBox(this, "Could not connect to remote PS3 " + targetMachine + ".\r\nPlease make sure that the remote PS3 defined in the settings is switched on and connected to the network.\r\n" + EditorManager.RemoteManagerPS3.GetLastError());
      }
    }

    private void Menu_Remote_ExportRun_Click(object sender, System.EventArgs e)
    {
      if (EditorManager.Scene == null)
      {
        EditorManager.ShowMessageBox(this, "Can't export scene: No scene loaded.");
        return;
      }
      
      if ((EditorManager.RemoteManagerPS3 != null) && EditorManager.RemoteManagerPS3.IsConnected())
      {
        ExportRun_PS3();
      }

      if ((EditorManager.RemoteManagerXbox360 != null) && EditorManager.RemoteManagerXbox360.IsConnected())
      {
        if (EditorManager.Settings.RemoteFileSharingXbox360)
        {
          ExportRun_Xbox360Shared();
        }
        else
        {
          ExportRun_Xbox360NonShared();
        }
      }

    }

    private IProfileManager.Profile DetermineExportRunProfile(TargetDevice_e targetPlatform, string targetPlatformName)
    {
      IProfileManager.Profile currentProfile = EditorManager.ProfileManager.GetActiveProfile();
      if (currentProfile == null)
        return null;

      bool haveTargetProfile = false;
      bool haveDX9Profile = false;
      foreach (IProfileManager.Profile profile in EditorManager.ProfileManager.GetProfiles())
      {
        if (profile._platform == targetPlatform)
          haveTargetProfile = true;
        if (profile._platform == TargetDevice_e.TARGETDEVICE_DX9)
          haveDX9Profile = true;
      }

      bool haveValidProfile = haveTargetProfile && (currentProfile._platform == targetPlatform);
      if (!haveTargetProfile)
        haveValidProfile = haveDX9Profile && (currentProfile._platform == TargetDevice_e.TARGETDEVICE_DX9);

      if (!haveValidProfile)
      {
        String msg;
        if (haveTargetProfile)
          msg = String.Format("The currently selected profile is not compatible with the {0}.\n\nPlease select a profile for the {0} platform to run the scene on the target device.", targetPlatformName);
        else if (haveDX9Profile)
          msg = String.Format("The currently selected profile is not compatible with the {0}.\n\nPlease select a DX9 profile or create a profile for the {0} platform to run the scene on the target device.", targetPlatformName);
        else
          msg = String.Format("The currently selected profile is not compatible with the {0}.\n\nPlease create a profile for DX9 or the {0} platform to run the scene on the target device.", targetPlatformName);
        EditorManager.ShowMessageBox(this, msg, "Remote", MessageBoxButtons.OK, MessageBoxIcon.Information);
        return null;
      }

      return currentProfile;
    }

    private void ExportRun_PS3()
    {
      IProfileManager.Profile exportProfile = DetermineExportRunProfile(TargetDevice_e.TARGETDEVICE_PS3, "PS3");
      if (exportProfile == null)
        return;

      String targetPath = EditorManager.Project.ProjectDir + Path.ChangeExtension(EditorManager.Scene.FileName, null) + @".vscene";
      bool bSuccess = EditorApp.Scene.ExportScene(targetPath, exportProfile._name);
      if (!bSuccess)
      {
        EditorManager.ShowMessageBox(this, "Could not export scene for PS3.");
        return;
      }

      bool bModeRes = EditorManager.RemoteManagerPS3.SetEditorMode(EditorManager.Mode.EM_NONE);
      if (!bModeRes)
      {
        EditorManager.ShowMessageBox(this, "Could not set remote Vision Engine mode to stopped on PS3.");
        return;
      }


      String vsceneFileName = Path.ChangeExtension(EditorApp.Scene.FileName, exportProfile._name + ".vscene");

      // we need to remove the last "\" of the scenedir else the following quote will not be recognized correctly
      String sceneDir = EditorManager.Project.ProjectDir;
      sceneDir = sceneDir.Remove(sceneDir.Length - 1);

      bool bLaunchRes = EditorManager.RemoteManagerPS3.LoadScene(sceneDir, vsceneFileName);
      if (!bLaunchRes)
      {
        EditorManager.ShowMessageBox(this, "Could not launch scene on PS3.");
        return;
      }

      bool bCamRes = EditorManager.RemoteManagerPS3.CreateCamera();
      if (!bCamRes)
      {
        EditorManager.ShowMessageBox(this, "Could not set initial Vision Engine camera state on PS3.");
        return;
      }

      Vector3F camPos = EditorManager.ActiveView.CameraPosition;
      Vector3F camOrientation = EditorManager.ActiveView.CameraAngles;
      bCamRes = EditorManager.RemoteManagerPS3.SetCamera(camPos, camOrientation);
      if (!bCamRes)
      {
        EditorManager.ShowMessageBox(this, "Could not set initial Vision Engine camera position on PS3.");
        return;
      }
    }

    private void ExportRun_Xbox360Shared()
    {
      IProfileManager.Profile exportProfile = DetermineExportRunProfile(TargetDevice_e.TARGETDEVICE_XBOX360, "Xbox360");
      if (exportProfile == null)
        return;

      String targetPath = EditorManager.Project.ProjectDir + Path.ChangeExtension(EditorManager.Scene.FileName, null) + @".vscene";
      bool bSuccess = EditorApp.Scene.ExportScene(targetPath, exportProfile._name);
      if (!bSuccess)
      {
        EditorManager.ShowMessageBox(this, "Could not export scene for Xbox360.");
        return;
      }

      bool bModeRes = EditorManager.RemoteManagerXbox360.SetEditorMode(EditorManager.Mode.EM_NONE);
      if (!bModeRes)
      {
        EditorManager.ShowMessageBox(this, "Could not set remote Vision Engine mode to stopped on Xbox360.");
        return;
      }

      SetRemoteCommonDirectory();

      String networkPath = @"net:\smb\"+EditorManager.Settings.RemoteFileSharingProjectDirectoryXbox360;
      String vsceneFileName = Path.ChangeExtension(EditorApp.Scene.FileName, exportProfile._name + ".vscene");

      bool bLaunchRes = EditorManager.RemoteManagerXbox360.LoadScene(networkPath, vsceneFileName);
      if (!bLaunchRes)
      {
        EditorManager.ShowMessageBox(this, "Could not launch scene on Xbox360.");
        return;
      }

      bool bCamRes = EditorManager.RemoteManagerXbox360.CreateCamera();
      if (!bCamRes)
      {
        EditorManager.ShowMessageBox(this, "Could not set initial Vision Engine camera state on Xbox360.");
        return;
      }

      Vector3F camPos = EditorManager.ActiveView.CameraPosition;
      Vector3F camOrientation = EditorManager.ActiveView.CameraAngles;
      bCamRes = EditorManager.RemoteManagerXbox360.SetCamera(camPos, camOrientation);
      if (!bCamRes)
      {
        EditorManager.ShowMessageBox(this, "Could not set initial Vision Engine camera position on Xbox360.");
        return;
      }
    }

    private void ExportRun_Xbox360NonShared()
    {
      IProfileManager.Profile exportProfile = DetermineExportRunProfile(TargetDevice_e.TARGETDEVICE_XBOX360, "Xbox360");
      if (exportProfile == null)
        return;

      String tempDir = Path.GetTempPath() + @"vForgeTempFiles";
      String vscenePath = tempDir + @"\" + Path.ChangeExtension(EditorManager.Scene.FileName, null) + @".vscene";
      string vsceneDir = Path.GetDirectoryName(vscenePath);

      try
      {
        if (Directory.Exists(tempDir))
          Directory.Delete(tempDir, true);
        Directory.CreateDirectory(vsceneDir);
      }
      catch
      {
        EditorManager.ShowMessageBox(this, "File operation failed - could not create temporary directory.");
        return;
      }

      bool bSuccess = EditorApp.Scene.ExportSceneNotSaveSettings(vscenePath, exportProfile._name);
      if (!bSuccess)
      {
        EditorManager.ShowMessageBox(this, "Could not export scene for Xbox360.");
        return;
      }

      String exportedPath = Path.ChangeExtension(vscenePath, exportProfile._name + ".vscene");

      String projectPath = Path.GetDirectoryName(EditorApp.Project.PathName) + @"\*.*";

      String projectTargetDirectory = EditorManager.Settings.GetProjectTargetDirectoryXbox360();
      String sceneTargetDirectory = Path.Combine(projectTargetDirectory, Path.GetDirectoryName(EditorApp.Project.EditorScene.CurrentExportProfile.ExportPath));

      EditorManager.RemoteManagerXbox360.CreateDir(projectTargetDirectory);
      EditorManager.RemoteManagerXbox360.CreateDir(sceneTargetDirectory);
      bool bResCopy1 = EditorManager.RemoteManagerXbox360.CopyFilesToRemote(projectPath, projectTargetDirectory, false, true);
      bool bResCopy2 = EditorManager.RemoteManagerXbox360.CopyFilesToRemote(exportedPath, sceneTargetDirectory, true, true);

      if (!bResCopy1 || !bResCopy2)
      {
        DialogResult dRes = EditorManager.ShowMessageBox(this, "Could not copy all of the files to the Xbox360.\r\nLaunch anyway?", "Error", MessageBoxButtons.YesNo);
        if (dRes != DialogResult.Yes)
          return;
      }

      String remoteDirectory = Path.Combine(EditorManager.Settings.RemoteTempDirectoryXbox360, Path.GetFileName(EditorApp.Project.PathName));
      remoteDirectory = Path.Combine(@"D:\", remoteDirectory);
      String vsceneFileName = Path.ChangeExtension(EditorApp.Scene.FileName, exportProfile._name + ".vscene");

      bool bModeRes = EditorManager.RemoteManagerXbox360.SetEditorMode(EditorManager.Mode.EM_NONE);
      if (!bModeRes)
      {
        EditorManager.ShowMessageBox(this, "Could not set remote Vision Engine mode to stopped on Xbox360.");
        return;
      }

      SetRemoteCommonDirectory();

      EditorManager.RemoteManagerXbox360.SetSharedDirectory(@"D:\Data\Vision\Base");
      bool bLaunchRes = EditorManager.RemoteManagerXbox360.LoadScene(remoteDirectory, vsceneFileName);
      if (!bLaunchRes)
      {
        EditorManager.ShowMessageBox(this, "Could not launch scene on Xbox360.");
        return;
      }

      bool bCamRes = EditorManager.RemoteManagerXbox360.CreateCamera();
      if (!bCamRes)
      {
        EditorManager.ShowMessageBox(this, "Could not set initial Vision Engine camera state on Xbox360.");
        return;
      }

      Vector3F camPos = EditorManager.ActiveView.CameraPosition;
      Vector3F camOrientation = EditorManager.ActiveView.CameraAngles;
      bCamRes = EditorManager.RemoteManagerXbox360.SetCamera(camPos, camOrientation);
      if (!bCamRes)
      {
        EditorManager.ShowMessageBox(this, "Could not set initial Vision Engine camera position on Xbox360.");
        return;
      }
    }


    private void Menu_Remote_Restart_Click(object sender, System.EventArgs e)
    {
      if ((EditorManager.RemoteManagerXbox360 != null) && EditorManager.RemoteManagerXbox360.IsConnected())
      {
        if (!EditorManager.RemoteManagerXbox360.RestartCurrentScene())
        {
          EditorManager.ShowMessageBox(this, "Could not restart scene on Xbox360.");
        }
      }

      if ((EditorManager.RemoteManagerPS3 != null) && EditorManager.RemoteManagerPS3.IsConnected())
      {
        if (!EditorManager.RemoteManagerPS3.RestartCurrentScene())
        {
          EditorManager.ShowMessageBox(this, "Could not restart scene on PS3.");
        }
      }

    }

    private void Menu_Remote_ReloadResources_Click(object sender, EventArgs e)
    {
      if ((EditorManager.RemoteManagerXbox360 != null) && EditorManager.RemoteManagerXbox360.IsConnected())
        if (!EditorManager.RemoteManagerXbox360.ReloadResources())
          EditorManager.ShowMessageBox(this, "Failed to reload resources on Xbox360");
      if ((EditorManager.RemoteManagerPS3 != null) && EditorManager.RemoteManagerPS3.IsConnected())
        if (!EditorManager.RemoteManagerPS3.ReloadResources())
          EditorManager.ShowMessageBox(this, "Failed to reload resources on PS3");
    }


    private void Menu_Remote_Xbox360_Animate_Click(object sender, System.EventArgs e)
    {
      bool bReloadScene = (EditorManager.RemoteManagerXbox360.GetEditorMode() == EditorManager.Mode.EM_PLAYING_IN_GAME);
      if (!EditorManager.RemoteManagerXbox360.SetEditorMode(EditorManager.Mode.EM_ANIMATING))
        EditorManager.ShowMessageBox(this, "Could not switch to animating mode on Xbox360.");
      else
      {
        if (bReloadScene)
        {
          SetRemoteCommonDirectory();
          EditorManager.RemoteManagerXbox360.RestartCurrentScene();
        }
      }
    }

    private void Menu_Remote_Xbox360_PlayTheGame_Click(object sender, System.EventArgs e)
    {
      bool bReloadScene = (EditorManager.RemoteManagerXbox360.GetEditorMode() == EditorManager.Mode.EM_PLAYING_IN_GAME);
      if (!EditorManager.RemoteManagerXbox360.SetEditorMode(EditorManager.Mode.EM_PLAYING_IN_GAME))
          EditorManager.ShowMessageBox(this, "Could not switch to play the game mode on Xbox360.");
      else
      {
        if (bReloadScene)
        {
          SetRemoteCommonDirectory();
          EditorManager.RemoteManagerXbox360.RestartCurrentScene();
        }
      }
    }

    private void Menu_Remote_Xbox360_StopPlaying_Click(object sender, System.EventArgs e)
    {
       bool bReloadScene = (EditorManager.RemoteManagerXbox360.GetEditorMode() == EditorManager.Mode.EM_PLAYING_IN_GAME);
       if (!EditorManager.RemoteManagerXbox360.SetEditorMode(EditorManager.Mode.EM_NONE))
         EditorManager.ShowMessageBox(this, "Could not stop playing on Xbox360.");
       else
       {
         if (bReloadScene)
         {
           SetRemoteCommonDirectory();
           EditorManager.RemoteManagerXbox360.RestartCurrentScene();
         }
       }
    }

    private void Menu_Remote_PS3_Animate_Click(object sender, System.EventArgs e)
    {
      bool bReloadScene = (EditorManager.RemoteManagerPS3.GetEditorMode() == EditorManager.Mode.EM_PLAYING_IN_GAME);
      if (!EditorManager.RemoteManagerPS3.SetEditorMode(EditorManager.Mode.EM_ANIMATING))
        EditorManager.ShowMessageBox(this, "Could not switch to animating mode on PS3.");
      else
      {
        if (bReloadScene)
        {
          EditorManager.RemoteManagerPS3.RestartCurrentScene();
        }
      }
    }

    private void Menu_Remote_PS3_PlayTheGame_Click(object sender, System.EventArgs e)
    {
      bool bReloadScene = (EditorManager.RemoteManagerPS3.GetEditorMode() == EditorManager.Mode.EM_PLAYING_IN_GAME);
      if (!EditorManager.RemoteManagerPS3.SetEditorMode(EditorManager.Mode.EM_PLAYING_IN_GAME))
          EditorManager.ShowMessageBox(this, "Could not switch to play the game mode on PS3.");
      else
      {
        if (bReloadScene)
        {
          EditorManager.RemoteManagerPS3.RestartCurrentScene();
        }
      }
    }

    private void Menu_Remote_PS3_StopPlaying_Click(object sender, System.EventArgs e)
    {
       bool bReloadScene = (EditorManager.RemoteManagerPS3.GetEditorMode() == EditorManager.Mode.EM_PLAYING_IN_GAME);
       if (!EditorManager.RemoteManagerPS3.SetEditorMode(EditorManager.Mode.EM_NONE))
         EditorManager.ShowMessageBox(this, "Could not stop playing on PS3.");
       else
       {
         if (bReloadScene)
         {
           EditorManager.RemoteManagerPS3.RestartCurrentScene();
         }
       }
    }

    private void menu_Remote_Popup(object sender, System.EventArgs e)
    {
      this.Menu_Remote_ExportAndRun.Enabled = false;
      this.Menu_Remote_Restart.Enabled = false;

      bool bSceneRunningOnRemote = false;

      if (EditorManager.RemoteManagerXbox360 != null)
      {
        this.Menu_Remote_XBOX360_Launch.Enabled = false;
        this.Menu_Remote_XBOX360_Stop.Enabled = false;
        this.Menu_Remote_XBOX360_PlayTheGame.Enabled = false;
        this.Menu_Remote_XBOX360_Animate.Enabled = false;

        if (EditorManager.RemoteManagerXbox360.IsConnected())
        {
          this.Menu_Remote_XBOX360_Connect.Text = "Disconnect Xbox360";
          this.Menu_Remote_XBOX360_Launch.Enabled = true;
          if (EditorManager.Scene != null)
          {
            bSceneRunningOnRemote = true;
            this.Menu_Remote_ExportAndRun.Enabled = true;
            if (EditorManager.RemoteManagerXbox360.CanRestart())
            {
              this.Menu_Remote_Restart.Enabled = true;

              switch (EditorManager.RemoteManagerXbox360.PlayMode)
              {
                case EditorManager.Mode.EM_NONE:
                  this.Menu_Remote_XBOX360_PlayTheGame.Enabled = true;
                  this.Menu_Remote_XBOX360_Animate.Enabled = true;
                  break;
                case EditorManager.Mode.EM_PLAYING_IN_GAME:
                  this.Menu_Remote_XBOX360_Stop.Enabled = true;
                  this.Menu_Remote_XBOX360_Animate.Enabled = true;
                  break;
                case EditorManager.Mode.EM_ANIMATING:
                  this.Menu_Remote_XBOX360_Stop.Enabled = true;
                  this.Menu_Remote_XBOX360_PlayTheGame.Enabled = true;
                  break;
              }
            }
          }
        }
        else
        {
          this.Menu_Remote_XBOX360_Connect.Text = "Connect Xbox360";
        }
      }

      if (EditorManager.RemoteManagerPS3 != null)
      {
        this.Menu_Remote_PS3_Stop.Enabled = false;
        this.Menu_Remote_PS3_PlayTheGame.Enabled = false;
        this.Menu_Remote_PS3_Animate.Enabled = false;

        if (EditorManager.RemoteManagerPS3.IsConnected())
        {
          this.Menu_Remote_PS3_Connect.Text = "Disconnect PS3";
          if (EditorManager.Scene != null)
          {
            bSceneRunningOnRemote = true;
            this.Menu_Remote_ExportAndRun.Enabled = true;
            if (EditorManager.RemoteManagerPS3.CanRestart())
            {
              this.Menu_Remote_Restart.Enabled = true;

              switch (EditorManager.RemoteManagerPS3.PlayMode)
              {
                case EditorManager.Mode.EM_NONE:
                  this.Menu_Remote_PS3_PlayTheGame.Enabled = true;
                  this.Menu_Remote_PS3_Animate.Enabled = true;
                  break;
                case EditorManager.Mode.EM_PLAYING_IN_GAME:
                  this.Menu_Remote_PS3_Stop.Enabled = true;
                  this.Menu_Remote_PS3_Animate.Enabled = true;
                  break;
                case EditorManager.Mode.EM_ANIMATING:
                  this.Menu_Remote_PS3_Stop.Enabled = true;
                  this.Menu_Remote_PS3_PlayTheGame.Enabled = true;
                  break;
              }
            }
          }
        }
        else
        {
          this.Menu_Remote_PS3_Connect.Text = "Connect PS3";
        }
      }

      Menu_Remote_ReloadResources.Enabled = bSceneRunningOnRemote;
    }


    #endregion

    #endregion

    #region Remote Error/Message Handling

    delegate void ShowRemoteErrorDelegate(String msg);

    void RemoteManager_Message_Sent(RemoteMessageType msgType, String msgText)
    {
      String logMessage = "REMOTE(" + EditorManager.Settings.RemoteMachineNameXbox360 + "): ";
      switch (msgType)
      {
        case RemoteMessageType.SYSTEM_CUSTOMMESSAGE:
          logMessage += "[CUSTOM]";
          break;
        case RemoteMessageType.SYSTEM_DEBUGSTR:
          logMessage += "[DEBUG]";
          break;
        case RemoteMessageType.SYSTEM_EXCEPTION:
          logMessage += "[EXCEPTION]";
          break;
        case RemoteMessageType.SYSTEM_ASSERT:
          logMessage += "[ASSERT]";
          break;
        case RemoteMessageType.UNKNOWN:
          logMessage += "[UNKNOWN]";
          break;
        case RemoteMessageType.VISION_ERROR:
          logMessage += "[VISION_ERROR]";
          break;
        case RemoteMessageType.VISION_WARNING:
          logMessage += "[VISION_WARNING]";
          break;
        case RemoteMessageType.VISION_MESSAGE:
          logMessage += "[VISION_MSG]";
          break;
      };
      logMessage += " " + msgText;
      EditorManager.EngineManager.LogPrint(logMessage);

      if (msgType == RemoteMessageType.VISION_ERROR || msgType == RemoteMessageType.SYSTEM_EXCEPTION)
      {
        this.Invoke(new ShowRemoteErrorDelegate(ShowRemoteErrorMessage), new object[]{msgText});
      }
    }

    void ShowRemoteErrorMessage(String errorMsg)
    {
      String dispErrMsg = "A fatal error occured on remote machine " + EditorManager.Settings.RemoteMachineNameXbox360 + ":\r\n";
      dispErrMsg += errorMsg;
      EditorManager.ShowMessageBox(this, dispErrMsg);
    }

    #endregion

    #region Status Bar

    /// <summary>
    /// Update size and content of the status bar
    /// </summary>
    public void UpdateStatusBar()
    {
      // update main panel content
      statusPanel_Main.Text = (EditorManager.EditorMode == EditorManager.Mode.EM_PLAYING_IN_GAME) ? "Press <ESC> to leave the play-the-game mode" : "";      

      // update playmode panel content
      string playModeMsg = "Stopped";
      switch (EditorManager.EditorMode)
      {
        case EditorManager.Mode.EM_ANIMATING:
          playModeMsg = "Animating"; break;
        case EditorManager.Mode.EM_PLAYING_IN_EDITOR:
          playModeMsg = "Running in Editor"; break;
        case EditorManager.Mode.EM_PLAYING_IN_GAME:
          playModeMsg = "Playing the Game"; break;
      }
      statusPanel_PlayMode.Text = playModeMsg;

      // update layout panel content
      statusPanel_Layout.Text = (EditorManager.ApplicationLayout.ActiveLayout != null) ? (EditorManager.ApplicationLayout.ActiveLayout.Name + " Layout") : "";
    }

    #endregion

    #region Startup Dialog

    private void Menu_File_Start_Click(object sender, EventArgs e)
    {
      OpenStartupDialog();
    }

    private void OpenStartupDialog()
    {
      // Create startup dialog
      StartupDlg startupDlg = new StartupDlg();

      // Set MRU list of the to dialog
      mruList_RecentProjects.BuildItemDictionary(startupDlg.RecentProjects);
      mruList_RecentScenes.BuildItemDictionary(startupDlg.RecentScenes);

      // Startup Guide and Startup Project
      string startupGuideDirectory = FileHelper.ResolveFilename(Application.StartupPath + "\\..\\..\\..\\..\\Docs\\Vision\\windows\\VisionEngine_StartupGuide.CHM");
      string startupProject = FileHelper.ResolveFilename(Application.StartupPath + "\\..\\..\\..\\..\\Docs\\Vision\\StartupGuide\\Assets\\Startup_Tutorial.project");
      string sampleDirectory = FileHelper.ResolveFilename(Application.StartupPath + "\\..\\..\\..\\..\\Data\\Vision\\Samples\\Engine\\Maps");

      // Show open file dialog
      startupDlg.ShowDialog();

      // Get dialog result
      StartupDlg.StartupResult_e result = startupDlg.StartupDialogResult;
      switch (result)
      {
        case StartupDlg.StartupResult_e.CreateNewProject:
          {
            Menu_File_New_Project_Click(null, EventArgs.Empty);
          }
          break;
        case StartupDlg.StartupResult_e.OpenProject:
          {
            Menu_File_OpenProject_Click(null, EventArgs.Empty);
          }
          break;
        case StartupDlg.StartupResult_e.OpenScene:
          {
            if (EditorManager.Project != null)
              Menu_File_CloseProject_Click(null, EventArgs.Empty);
            Menu_File_OpenScene_Click(null, EventArgs.Empty);
          }
          break;
        case StartupDlg.StartupResult_e.OpenRecentProject:
          {
            //Make sure the current scene is saved
            if (EditorApp.PromptSaveScene() == DialogResult.Cancel)
              return;
            LoadProject(startupDlg.SelectedFile, true);
          }
          break;
        case StartupDlg.StartupResult_e.OpenRecentScene:
          {
            //Make sure the current scene is saved
            if (EditorApp.PromptSaveScene() == DialogResult.Cancel)
              return;
            LoadScene(startupDlg.SelectedFile, true);
          }
          break;
        case StartupDlg.StartupResult_e.LoadStartupGuide:
          {
            if (System.IO.File.Exists(startupGuideDirectory))
              Process.Start(startupGuideDirectory);
            else
              EditorManager.ShowMessageBox(this, "The Startup Guide could not be found at the predefined location (\"" +  startupGuideDirectory + "\"). Please make sure the Vison SDK is installed properly.", "Loading Startup Guide Failed", MessageBoxButtons.OK, MessageBoxIcon.Warning);
          }
          break;
        case StartupDlg.StartupResult_e.LoadStartupProject:
          {
            if (System.IO.File.Exists(startupProject))
              LoadProject(startupProject, true);
            else
              EditorManager.ShowMessageBox(this, "The Startup Project could not be found at the predefined location. Please make sure the Vison SDK is installed properly.", "Loading Startup Project Failed", MessageBoxButtons.OK, MessageBoxIcon.Warning);
          }
          break;
        case StartupDlg.StartupResult_e.OpenSampleMap:
          {
            if (System.IO.Directory.Exists(sampleDirectory))
              OpenProjectDialog(sampleDirectory);
            else
              EditorManager.ShowMessageBox(this, "The Engine Sample Map directory could not be found at the predefined location. Please make sure the Vison Engine Samples are installed.", "Loading Engine Sample Maps Failed", MessageBoxButtons.OK, MessageBoxIcon.Warning);
          }
          break;
        case StartupDlg.StartupResult_e.Close:
        default:
          break;
      }
    }

    #endregion
    
    #region Properties

    /// <summary>
    /// The property editor panel
    /// </summary>
    public PropertyPanel PropertyEditorPanel
    {
      get
      {
        return propertyPanel1;
      }
    }

    /// <summary>
    /// The resource viewer panel
    /// </summary>
    public ResourceViewerPanel ResourceViewerPanel
    {
      get
      {
        return resourceViewerPanel;
      }
    }


    #endregion Properties

    private void Menu_File_Export_Click(object sender, EventArgs e)
    {

    }

    private void OpenAssetBrowser_Click(object sender, EventArgs e)
    {
      assetBrowserPanel.Show();
    }
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
