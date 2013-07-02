/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections;
using System.Drawing;
using System.IO;
using System.Windows.Forms;
using NUnit.Core;
using CSharpFramework;
using ManagedFramework;
using CSharpFramework.Scene;
using CSharpFramework.Serialization;
using HavokAiEditorPlugin.Shapes;
using VisionEditorPlugin.Shapes;
using CSharpFramework.Shapes;
using CSharpFramework.Actions;
using CSharpFramework.DynamicProperties;
using CSharpFramework.Math;
using HavokAiPanelDialogs;


namespace HavokAiEditorPlugin
{
  /// <summary>
  /// Main plugin class
  /// </summary>
  public class EditorPlugin : IEditorPluginModule
  {
    /// <summary>
    /// Constructor of EditorPlugin that sets up plugin information
    /// </summary>
    public EditorPlugin()
    {
      _version = 0;                   //version used for serialization
      _name = "HavokAiEditorPlugin";
    }

    /// <summary>
    /// This scope holds one global instance of the plugin info. It is returned by all shape's overridden GetPluginInformation function.
    /// This global info is filled inside InitPluginModule
    /// </summary>
    public static EditorPluginInfo EDITOR_PLUGIN_INFO = new EditorPluginInfo();

    IShapeCreatorPlugin[] creators = null;
    HavokAiPanel _panel = null;

    public HavokAiPanel AiPanel
    {
      get { return _panel; }
    }

    #region InitPluginModule : register the creator plugins

    /// <summary>
    /// Overridden function that gets called when the plugin is loaded. Registers all creator plugins
    /// </summary>
    /// <returns></returns>
    public override bool InitPluginModule()
    {
      EDITOR_PLUGIN_INFO.NativePluginNames = new string[] { "Havok Ai" };
      HavokAiManaged.ManagedModule.InitManagedModule();

      // register shape class
      creators = new IShapeCreatorPlugin[]
      {
          new HavokNavMeshShapeCreator(),
          new HavokNavMeshCarverShapeCreator(),
          new HavokNavMeshSeedPointShapeCreator(),
          new HavokNavMeshLocalSettingsShapeCreator(),
          new HavokNavMeshTestPathShapeCreator()
      };
      foreach (IShapeCreatorPlugin plugin in creators)
      {
        EditorManager.ShapeCreatorPlugins.Add(plugin);
      }

      // create and activate panel
      _panel = new HavokAiPanel(EditorManager.ApplicationLayout.DockingArea);
      _panel.ShowDockable();
      _panel.Enabled = false;

      // register callbacks
      IScene.ShapeChanged += new ShapeChangedEventHandler(IScene_ShapeChanged);
      IScene.PropertyChanged += new PropertyChangedEventHandler(IScene_PropertyChanged);
      IScene.EngineInstancesChanged += new EngineInstancesChangedEventHandler(IScene_EngineInstancesChanged);
      EditorManager.SceneChanged += new SceneChangedEventHandler(EditorManager_SceneChanged);
      EditorManager.ShapeSelectionChanged += new ShapeSelectionChangedEventHandler(EditorManager_ShapeSelectionChanged);
      EditorManager.EditorModeChanged += new EditorModeChangedEventHandler(EditorManager_EditorModeChanged);

      // Register automated tests
      TestSuiteBuilder testBuilder = new TestSuiteBuilder();
      TestSuite testSuite = testBuilder.Build(typeof(EditorPlugin).Assembly.FullName);
      TestManager.AddTestSuite(testSuite);

      return true;
    }

    #endregion

    #region DeInitPluginModule

    /// <summary>
    /// Overridden function that gets called when the plugin is loaded. Registers all creator plugins
    /// </summary>
    /// <returns></returns>
    public override bool DeInitPluginModule()
    {
      HavokAiManaged.ManagedModule.DeInitManagedModule();

      foreach (IShapeCreatorPlugin plugin in creators)
      {
        EditorManager.ShapeCreatorPlugins.Remove(plugin);
      }

      _panel.Close();
      _panel.Dispose();
      _panel = null;

      EditorManager.EditorModeChanged -= new EditorModeChangedEventHandler(EditorManager_EditorModeChanged);
      EditorManager.ShapeSelectionChanged -= new ShapeSelectionChangedEventHandler(EditorManager_ShapeSelectionChanged);
      EditorManager.SceneChanged -= new SceneChangedEventHandler(EditorManager_SceneChanged);
      IScene.EngineInstancesChanged -= new EngineInstancesChangedEventHandler(IScene_EngineInstancesChanged);
      IScene.PropertyChanged -= new PropertyChangedEventHandler(IScene_PropertyChanged);
      IScene.ShapeChanged -= new ShapeChangedEventHandler(IScene_ShapeChanged);

      return true;
    }

    #endregion

    void EditorManager_SceneChanged(object sender, SceneChangedArgs e)
    {
      if ((EditorManager.Scene == null) || (EditorManager.Scene.AllShapesOfType(typeof(HavokAiEditorPlugin.Shapes.HavokNavMeshShape)).Count == 0))
      {
        _panel.Enabled = false;
      }
      else
      {
        _panel.Enabled = true;

        // If the cached nav mesh file was loaded then enable the panel controls
        HavokAiEditorPlugin.Shapes.HavokNavMeshShape shape = (HavokAiEditorPlugin.Shapes.HavokNavMeshShape)EditorManager.Scene.FindFirstShapeOfType(typeof(HavokAiEditorPlugin.Shapes.HavokNavMeshShape), true);
        if (shape != null)
        {
          _panel.EnableNavMeshDependentControls(true);
        }
      }
    }

    void IScene_EngineInstancesChanged(object sender, EngineInstancesChangedArgs e)
    {
      if ((EditorManager.Scene == null) || (EditorManager.Scene.AllShapesOfType(typeof(HavokAiEditorPlugin.Shapes.HavokNavMeshShape)).Count == 0))
        return;
    }

    void IScene_ShapeChanged(object sender, CSharpFramework.Shapes.ShapeChangedArgs e)
    {
      if (e.shape is HavokAiEditorPlugin.Shapes.HavokNavMeshShape)
      {
        if ((e.action == CSharpFramework.Shapes.ShapeChangedArgs.Action.Added) || (e.action == CSharpFramework.Shapes.ShapeChangedArgs.Action.Removed))
        {
          if (EditorManager.Scene.AllShapesOfType(typeof(HavokAiEditorPlugin.Shapes.HavokNavMeshShape)).Count == 0)
          {
            _panel.Enabled = false;
          }
          else
          {
            _panel.Enabled = true;
            _panel.EnableBuildButtonAsterisk(true);
          }
        }
      }
      else if (e.shape is HavokAiEditorPlugin.Shapes.HavokNavMeshTestPathShape)
      {
        if (e.action == CSharpFramework.Shapes.ShapeChangedArgs.Action.Removed)
        {
          _panel.EnablePathDependentControls(false, false);
        }
      }
    }

    void IScene_PropertyChanged(object sender, CSharpFramework.PropertyChangedArgs e)
    {
      if (e._component is HavokAiEditorPlugin.Shapes.HavokNavMeshShape)
      {
        if (e._propertyName != "UserDataString" && e._propertyName != "ExportShape" && e._propertyName != "Visible" && e._propertyName != "TextLabel")
        {
          _panel.EnableBuildButtonAsterisk(true);
        }
      }
    }

    void EditorManager_ShapeSelectionChanged(object sender, ShapeSelectionChangedArgs e)
    {
      if ((EditorManager.Scene == null) || (EditorManager.Scene.AllShapesOfType(typeof(HavokAiEditorPlugin.Shapes.HavokNavMeshShape)).Count == 0))
        return;

      int numPaths = 0;
      bool showPath = false;
      foreach (ShapeBase shape in e.NewSelection)
      {
        if (shape is HavokAiEditorPlugin.Shapes.HavokNavMeshTestPathShape)
        {
          numPaths++;
          HavokAiEditorPlugin.Shapes.HavokNavMeshTestPathShape path = shape as HavokAiEditorPlugin.Shapes.HavokNavMeshTestPathShape;
          showPath = path.Visible;
        }
      }
      _panel.EnablePathDependentControls(numPaths == 1, showPath);
    }

    void EditorManager_EditorModeChanged(object sender, EditorModeChangedArgs e)
    {
      if ((EditorManager.Scene == null) || (EditorManager.Scene.AllShapesOfType(typeof(HavokAiEditorPlugin.Shapes.HavokNavMeshShape)).Count == 0))
        return;

      if (e.action == EditorModeChangedArgs.Action.ModeChanged)
      {
        if (e._newMode == EditorManager.Mode.EM_NONE)
        {
          HavokAiManaged.ManagedModule.SetConnectToPhysicsWorld(false);
        }
        else if (e._newMode == EditorManager.Mode.EM_PLAYING_IN_EDITOR || e._newMode == EditorManager.Mode.EM_PLAYING_IN_GAME)
        {
          if (_panel.WantPhysicsConnection())
          {
            HavokAiManaged.ManagedModule.SetConnectToPhysicsWorld(true);
          }
        }
      }
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
