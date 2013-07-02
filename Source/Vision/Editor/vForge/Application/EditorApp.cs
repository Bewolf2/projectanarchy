/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Windows.Forms;
using CSharpFramework;
using ManagedFramework;
using System.Diagnostics;
using CSharpFramework.Clipboard;
using CSharpFramework.Scene;
using Editor.Tests;
using Editor.Shapes;
using Editor.Factory;
using CSharpFramework.Shapes;
using CSharpFramework.Editor;
using CSharpFramework.Helper;
using CSharpFramework.Serialization;
using ManagedBase;
using AssetManagementManaged;

namespace Editor
{
	/// <summary>
	/// This singleton class forms the main customer modifiable application.
  /// It is similar to the EditorManager class but handles game specific
  /// implementations of the project and scene.
  /// 
  /// We use the EditorManager class directly to make our code portable 
  /// between different game modules.
  /// e.g. Use EditorManager.Project.Save() instead of EditorApp.Project.Save()
  /// 
  /// Only in cases where you need game specific classes should you
  /// use EditorApp.Project and EditorApp.Scene directly.
	/// </summary>
	public class EditorApp
  {
    #region Init/DeInit

    /// <summary>
    ///  Prevent creation
    /// </summary>
    private EditorApp()	 {}

    /// <summary>
    ///  Initialize our editor
    /// </summary>
    /// <remarks>
    /// -Called from the form constructor so that the mainform is
    ///  available when the plugins are initialized
    /// </remarks>
    public static void InitEditorApp(Form mainForm)
    {
      EditorManager.MainForm = mainForm;

      // set the asset profile first because everything from here on already needs file access
      ManagedBase.AssetHelper.SetAssetProfile("vForge", "vForge");
      ManagedBase.AssetHelper.AddVariantKey("vForge");

      // add the default shapes from the editor
      EditorManager.ShapeCreatorPlugins.Add(new GroupCreator());
      EditorManager.ShapeCreatorPlugins.Add(new Group3DShapeCreator());
      EditorManager.ShapeCreatorPlugins.Add(new AnchorShapeCreator());
      

      //Initialize the editor manager
      //EditorManager.InitPlugins(Application.StartupPath); // do this later...
      EditorManager.SceneChanged +=new SceneChangedEventHandler(EditorManager_SceneChanged);
      IProject.NewProjectLoaded += new EventHandler(IProject_ProjectChanged);
      IProject.ProjectUnloaded += new EventHandler(IProject_ProjectChanged);

      EditorManager.ProcessExternalFileDrop += new ExternalFileDropHandler(EditorManager_ProcessExternalFileDrop);
      EditorManager.EditorModeChanged += new EditorModeChangedEventHandler(EditorManager_EditorModeChanged);
      _visionEngine =  new VisionEngineManager();
      EditorManager.EngineManager = _visionEngine;
      EditorManager.ShapeFactory = new VForgeShapeFactory();
      EditorManager.SceneFactory = new EditorSceneFactory();
      EditorManager.ProjectFactory = new EditorProjectFactory();
      EditorManager.LayerFactory = new EditorLayerFactory();
      EditorManager.ZoneFactory = new EditorZoneFactory();
      EditorManager.AddShapeActionFactory = new EditorAddShapeActionFactory();
      EditorManager.RemoveShapeActionFactory = new EditorRemoveShapeActionFactory();
      EditorManager.AddShapesActionFactory = new EditorAddShapesActionFactory();
      EditorManager.RemoveShapesActionFactory = new EditorRemoveShapesActionFactory();
      EditorManager.SetPropertyActionFactory = new EditorSetPropertyActionFactory();
      EditorManager.SkyConfigChangedActionFactory = new EditorSkyConfigChangedActionFactory();
      EditorManager.VisibilityBuilder = new VisionVisibilityBuilder();
      EditorManager.AppInterface = new EditorAppInterface();
      EditorManager.AssetManager = new AssetManager();
      EditorManager.ProfileManager = new ProfileManager();
      (EditorManager.ProfileManager as ProfileManager).InitEventHandler();
      (EditorManager.AssetManager as AssetManager).InitEventHandler();
      EditorManager.RendererNodeManager = new VisionRendererNodeManager();
      EditorManager.FileLockFactory = new DefaultDiskLockFactory();

      // Create the test helper utilities
      TestManager.Helpers = new TestHelpers();

      UpdateTitleBar();
    }


    /// <summary>
    /// Exits the editor application via Application.Exit(), but optionally checks for scene dirty flag
    /// </summary>
    /// <param name="bPrompt"></param>
    /// <returns>true, if application is closed, false if the user clicked CANCEL in the prompt</returns>
    public static bool ExitEditorApp(bool bPrompt)
    {
      //Make sure the project is saved
      if (bPrompt)
      {
        if (EditorManager.Scene!=null)
        {
          if (PromptSaveScene() == DialogResult.Cancel)
            return false;
        } 
        else
        {
          if (PromptSaveProject() == DialogResult.Cancel)
            return false;
        }
      }

      // Call the BeforeEditorAppShutdown callback to allow unmanaged data to be freed before the engine shutdown.
      EventArgs e = new EventArgs();
      EditorManager.OnBeforeEditorAppShutdown(e);

      EditorManager.SceneChanged -= new SceneChangedEventHandler(EditorManager_SceneChanged);
      IProject.NewProjectLoaded -= new EventHandler(IProject_ProjectChanged);
      IProject.ProjectUnloaded -= new EventHandler(IProject_ProjectChanged);

      EditorManager.ProcessExternalFileDrop -= new ExternalFileDropHandler(EditorManager_ProcessExternalFileDrop);
      EditorManager.EditorModeChanged -= new EditorModeChangedEventHandler(EditorManager_EditorModeChanged);
    
      if (EditorManager.Project!=null)
        EditorManager.Project.Close();

      if (EditorManager.ProfileManager != null)
      {
        EditorManager.ProfileManager.DeInit();
        EditorManager.ProfileManager = null;
      }
      if (EditorManager.AssetManager != null)
      {
        EditorManager.AssetManager.Dispose();
        EditorManager.AssetManager = null;
      }

      ManagedFramework.ManagedFramework.OneTimeDeInit();

      if (_visionEngine != null)
          _visionEngine.Dispose();

      //Exit
      Application.Exit();
      Application.DoEvents();
      return true;
    }
    
    /// <summary>
    /// //Deinitialize our editor
    /// </summary>
    /// <remarks>
    /// -Called from Main after the application has run
    /// </remarks>
    public static void DeinitEditorApp()
    {
      EditorManager.EngineManager.LogWindow = null;
      EditorManager.MainForm = null;
      EditorManager.ProfileManager = null;
      EditorManager.AssetManager = null;

      //Deinitialize the editor manager
      EditorManager.Deinit();
    }

    #endregion

    #region Access Project as EditorProject

    /// <summary>
    /// Returns the project as our custom type
    /// </summary>
    public static EditorProject Project
    {
      get
      {
        // don't cast since it might be of ther type than EditorProject
        return EditorManager.Project as EditorProject;
      }
      set {EditorManager.Project = value;}
    }

    #endregion

    #region Access Scene as EditorScene

    /// <summary>
    /// Returns the scene as our custom type
    /// </summary>
    public static EditorScene Scene
    {
      get
      {
        Debug.Assert(EditorManager.Scene!=null);
        return (EditorScene)EditorManager.Scene;
      }
      set
      {
        EditorManager.EngineManager.MaterialProviderList = null;  // do this before engine intance are created
        EditorManager.Scene = value;

        // the following is important to load the V3D
        if (value!=null)
        {
          EditorManager.Scene.IsInitialising = true;

          GizmoBase.GridSettings = value.Settings.GridSettings;

          EditorManager.EngineManager.InitNewScene();

          foreach (Layer layer in value.Layers)
            layer.OnActivateScene();
          value.OnCreateAllEngineInstances(EditorManager.Progress);

          EditorManager.EngineManager.MaterialProviderDirty = true;
          EditorManager.Scene.IsInitialising = false;
        }
        else // load the empty scene
        {
          GizmoBase.GridSettings = null;

          if (EditorManager.EngineManager.IsInitialized())
            EditorManager.EngineManager.DeInitScene();
        }
        EditorManager.VisibilityBuilder.Build(value, null);
        if (!TestManager.IsRunning)
          EditorManager.EngineManager.ShowErrorReportDialog();

        if (value != null)
          EditorManager.OnSceneEvent(new SceneEventArgs(SceneEventArgs.Action.AfterLoading, true));
      }
    }

    #endregion

    #region View

    /// <summary>
    /// Returns the engine view
    /// </summary>
    public static VisionView ActiveView
    {
      get {return _view;}
      set 
      {
        _view = value;
        EditorManager.ActiveView = value;
      }
    }
    static VisionView _view;

    #endregion

    #region EngineManager

    static VisionEngineManager _visionEngine;

    /// <summary>
    /// global function to get the engine manager
    /// </summary>
    static public VisionEngineManager EngineManager {get {return _visionEngine;}}

    #endregion

    #region Main Form helpers

    /// <summary>
    /// Class used to defined some delegates to access to optionnal top-level form functionnalities
    /// </summary>
    public static EditorAppDelegates EditorAppDelegates = new EditorAppDelegates();

    /// <summary>
    /// Updates the title bar of the main form (scene name + project path)
    /// </summary>
    public static void UpdateTitleBar()
    {
      string Identifier = "vForge";
      if (IntPtr.Size == 8)
        Identifier += " x64";

      #if _VR_DX11
        Identifier += " (DX11)";
      #endif

      if (EditorAppDelegates.SetFormTextDelegate != null)
      {
        if (EditorManager.Project == null)
        {
          EditorAppDelegates.SetFormTextDelegate("[no project] - " + Identifier);
        return;
      }

        if (EditorManager.Scene == null)
      {
          EditorAppDelegates.SetFormTextDelegate(" [no scene] - " + EditorManager.Project.ProjectDir + " - " + Identifier);
        return;
      }

        EditorAppDelegates.SetFormTextDelegate(" [" + EditorManager.Scene.FileName + "] - " + EditorManager.Project.ProjectDir + " - " + Identifier);
    }
    }

    #endregion

    #region Event handlers

    private static void EditorManager_SceneChanged(object sender, SceneChangedArgs e)
    {
      UpdateTitleBar();
    }

    static void IProject_ProjectChanged(object sender, EventArgs e)
    {
      UpdateTitleBar();
    }


    private static void EditorManager_ProcessExternalFileDrop(object sender, ExternalFileDropArgs e)
    {
      if (!e.Data.GetDataPresent(DataFormats.FileDrop))
        return;
      
      string[] res = e.Data.GetData(DataFormats.FileDrop) as string[];
      if (res == null || res.GetLength(0) == 0)
        return;

      string file = res[0];

      string extension = file.Substring(file.LastIndexOf('.') + 1).ToLower();

      if (extension == "project" || extension == "scene")
      {
        //Make sure the current project is saved
        if (EditorApp.PromptSaveProject() == DialogResult.Cancel) 
          return;
      }
      else
          return; //The file extension was not recognized

      if (extension == "project" && EditorAppDelegates.LoadProjectDelegate != null)
      {
        // now load the project
        EditorAppDelegates.LoadProjectDelegate(file, true);
      }
      else if (extension == "scene" && EditorAppDelegates.LoadSceneDelegate != null)
      {
        // loads the scene if possible

        /* TODO:
         * Currently the project gets reloaded every time regardless whether it already is loaded.
         * Maybe it would be faster to check for this case first.
         */

        EditorAppDelegates.LoadSceneDelegate(file, true);
      }

      e.Processed = true;
    }

    static void EditorManager_EditorModeChanged(object sender, EditorModeChangedArgs e)
    {
      if(e._newMode == EditorManager.Mode.EM_PLAYING_IN_EDITOR || e._newMode == EditorManager.Mode.EM_PLAYING_IN_GAME)
      {
        // Set settings in the settings map
        ManagedBase.SettingsMap.SetSetting("AskForDebuggerOnScriptError", EditorManager.Settings.AskForDebuggerOnScriptError.ToString());
      }
    }

    #endregion

    #region Scene helpers (save,...)

    /// <summary>
    /// Prompt the user about saving the project and also does the saving.
    /// Afterwards you just need to check if it was canceled or not.
    /// </summary>
    /// <returns></returns>
    public static DialogResult PromptSaveProject()
    {
      if (EditorManager.Project == null) return DialogResult.No;        //No project, nothing to save
      if (EditorManager.SilentMode == true) return DialogResult.No; //Ignore prompt in silent mode
      // we have a project
      if (!EditorManager.WantsToCloseScene())
        return DialogResult.Cancel; // any other plugin reported that there is unsaved data and the user clicked Cancel

      if (EditorManager.Project.Dirty == false) return DialogResult.No; //Project not dirty, nothing to save
      while (true)
      {
        //Show a dialog box
        DialogResult res = EditorManager.ShowMessageBox("The project has unsaved changes. Do you want to save the modified files?", "Confirm project changes", MessageBoxButtons.YesNoCancel, MessageBoxIcon.Question);
        if (res == DialogResult.Yes)
        {
          if (!EditorManager.Project.Save())
          {
            EditorManager.ShowMessageBox("Error saving the project", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            continue;  // Save failed? Prompt the user again
          }
        }

        //For No or Cancel, we just return the result
        return res;
      }

      //Should never get here...
    }

    /// <summary>
    /// Prompt the user about saving the scene and also does the saving.
    /// Afterwards you just need to check if it was canceled or not
    /// </summary>
    /// <returns></returns>
    public static DialogResult PromptSaveScene()
    {
      if (EditorManager.Project == null) return DialogResult.No;  //No project, nothing to save
      if (EditorManager.Scene == null)   return DialogResult.No;  //No scene, nothing to save
      if (EditorManager.SilentMode == true) return DialogResult.No; //Ignore prompt in silent mode
      // we have a scene
      if (!EditorManager.WantsToCloseScene())
        return DialogResult.Cancel; // any other plugin reported that there is unsaved data and the user clicked Cancel

      if (EditorManager.Scene.Dirty == false) return DialogResult.No;   //Scene not dirty, nothing to save
      while (true)
      {
        //Show a dialog box
        DialogResult res = EditorManager.ShowMessageBox("The scene has unsaved changes. Do you want to save the modified scene?", "Confirm scene changes", MessageBoxButtons.YesNoCancel, MessageBoxIcon.Question);
        if (res == DialogResult.Yes)
        {
          if (!EditorManager.Scene.Save())
          {
            EditorManager.ShowMessageBox("Error saving the scene", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            
            //Danie: This happened once somehow.. Lets get out of the loop instead of crashing
            if (EditorManager.Scene == null)
                return DialogResult.No;

            continue;  // Save failed? Prompt the user again
          }
        }

        //For No or Cancel, we just return the result
        return res;
      }

      //Should never get here...
    }
    #endregion
  }


  #region class EditorAppInterface

  /// <summary>
  /// Implements the IAppInterface interface
  /// </summary>
  public class EditorAppInterface : IAppInterface
  {
    public override bool Shutdown(bool bPrompt)
    {
      return EditorApp.ExitEditorApp(bPrompt);
    }
  }

  #endregion
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
