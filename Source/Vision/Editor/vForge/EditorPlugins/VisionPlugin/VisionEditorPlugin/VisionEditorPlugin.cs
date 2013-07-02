/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Windows.Forms;
using System.Drawing;
using CSharpFramework;
using System.Collections;
using ManagedFramework;
using VisionManaged;
using VisionEditorPlugin.Dialogs;
using CSharpFramework.Scene;
using NUnit.Core;
using NUnit.Framework;
using VisionEditorPlugin.Shapes;
using CSharpFramework.Serialization;
using CSharpFramework.Visitors;
using CSharpFramework.Actions;
using VisionEditorPlugin.Tool_Plugins;
using System.IO;
using CSharpFramework.Clipboard;
using CSharpFramework.Shapes;
using System.Text;

namespace VisionEditorPlugin
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
      _name = "VisionEditorPlugin";
    }

    /// <summary>
    /// This scope holds one global instance of the plugin info. It is returned by all shape's overridden GetPluginInformation function.
    /// This global info is filled inside InitPluginModule
    /// </summary>
    public static EditorPluginInfo EDITOR_PLUGIN_INFO = new EditorPluginInfo();

    #region InitPluginModule : register the creator plugins

    /// <summary>
    /// Overridden function that gets called when the plugin is loaded. Registers all creator plugins
    /// </summary>
    /// <returns></returns>
    public override bool InitPluginModule()
    {
      VisionManaged.ManagedModule.InitManagedModule();
      EDITOR_PLUGIN_INFO.NativePluginNames = new string[] { "VisionEnginePlugin" };

      // listen to the following events
      IProject.NewProjectLoaded += new EventHandler(IProject_NewProjectLoaded);
      IProject.ProjectUnloaded += new EventHandler(IProject_ProjectUnloaded);
      EditorManager.SceneChanged += new SceneChangedEventHandler(EditorManager_SceneChanged);
      EditorManager.SceneEvent += new SceneEventHandler(EditorManager_SceneEvent);
      EditorManager.ProcessExternalFileDrop += new ExternalFileDropHandler(EditorManager_ProcessExternalFileDrop);
      EditorManager.QueryDragDropContext += new QueryDragDropContextEventHandler(EditorManager_QueryDragDropContext);

      // make this globally available
      PrefabDesc.PrefabInstanceCreator = new PrefabShapeCreator();

      // register shape classes
      creators = new IShapeCreatorPlugin[]
                 {
                  new EntityCreator(),
                  new OmniLightCreator(false),
                  new SpotLightCreator(false),
                  new DirectionalLightCreator(false),
                  new OmniLightCreator(true),
                  new SpotLightCreator(true),
                  new DirectionalLightCreator(true),
                  new TimeOfDaySunCreator(),

                  new ParticleSystemCreator(),
                  new ConstraintCreator(EngineInstanceConstraint.ConstraintType_e.GroundPlane),
                  new ConstraintCreator(EngineInstanceConstraint.ConstraintType_e.Plane),
                  new ConstraintCreator(EngineInstanceConstraint.ConstraintType_e.Sphere),
                  new ConstraintCreator(EngineInstanceConstraint.ConstraintType_e.AABox),
                  new ConstraintCreator(EngineInstanceConstraint.ConstraintType_e.XAxis),
                  new ConstraintCreator(EngineInstanceConstraint.ConstraintType_e.YAxis),
                  new ConstraintCreator(EngineInstanceConstraint.ConstraintType_e.ZAxis),
                  new ConstraintCreator(EngineInstanceConstraint.ConstraintType_e.Fan),
                  new ConstraintCreator(EngineInstanceConstraint.ConstraintType_e.Cyclone),
                  new ConstraintCreator(EngineInstanceConstraint.ConstraintType_e.Point),
                  new ConstraintCreator(EngineInstanceConstraint.ConstraintType_e.GravityPoint),
                  new ConstraintCreator(EngineInstanceConstraint.ConstraintType_e.Terrain),
                  new ConstraintCreator(EngineInstanceConstraint.ConstraintType_e.CameraBox),

                  new ProjectorShapeCreator(),
                  new ClothObjectCreator(),
                  new MirrorCreator(),
                  new VisibilityObjectCreator(),
                  new LightGridBoxCreator(),
                  new LightGridDetailBoxCreator(),
                  new LightGridIndicatorCreator(),
                  new PathShapeCreator(),
                  new CircleShapeCreator(),
                  new PathCameraShapeCreator(),
                  new CameraPositionShapeCreator(),

                  //new RenderTargetShapeCreator(), // still port engine instance
                  new CubemapShapeCreator(),
                  //new PostProcessingShapeCreator(),

                  PrefabDesc.PrefabInstanceCreator,
                  new StaticMeshShapeCreator(),
                  new StaticMeshGroupShapeCreator(),
                  new TriggerBoxShapeCreator(),
                  new BillboardGroupShapeCreator(),
                  new CustomVolumeShapeCreator(),

#if !HK_ANARCHY
                  new CloudLayerCreator(),
                  new DecorationGroupCreator(),
                  new WaterCreator(),
                  new SunglareCreator(),
                  new VolumetricConeCreator(),
                  new FogObjectCreator(),
                  new ProjectedDecalCreator(),
#endif
                  };

      foreach (IShapeCreatorPlugin plugin in creators)
        EditorManager.ShapeCreatorPlugins.Add( plugin );

      // Add lightmap tweaking menu item
      // this is not supported ATM, because lightmaps are not in a central place anymore
      //lightmapMenuItemPlugin = new LightmapMenuItemPlugin();
      //EditorManager.AddMenuItemPlugin(lightmapMenuItemPlugin);

      _colorGradingTool = new ColorGradingToolPlugin();
      EditorManager.AddMenuItemPlugin(_colorGradingTool);

      PrefabManager.BINARY_SAVER = new VisionPrefabBinarySaver();
      VisionEngineManager.EntityClassManagerType = typeof(EntityClassManager);

      // register tests
      TestSuiteBuilder testBuilder = new TestSuiteBuilder();
      TestSuite testSuite = testBuilder.Build(typeof(EditorPlugin).Assembly.FullName);
      TestManager.AddTestSuite(testSuite);
      return true;
    }

    #endregion

    #region DeInitPluginModule

    /// <summary>
    /// Overridden function that gets called when the plugin is unloaded. Deregisters all creator plugins
    /// </summary>
    /// <returns></returns>
    public override bool DeInitPluginModule()
    {
      // listen to the following events
      IProject.NewProjectLoaded -= new EventHandler(IProject_NewProjectLoaded);
      IProject.ProjectUnloaded -= new EventHandler(IProject_ProjectUnloaded);
      EditorManager.SceneChanged -= new SceneChangedEventHandler(EditorManager_SceneChanged);
      EditorManager.SceneEvent -= new SceneEventHandler(EditorManager_SceneEvent);
      EditorManager.ProcessExternalFileDrop -= new ExternalFileDropHandler(EditorManager_ProcessExternalFileDrop);
      EditorManager.QueryDragDropContext -= new QueryDragDropContextEventHandler(EditorManager_QueryDragDropContext);

      // Remove menu item plugins
      //if (lightmapMenuItemPlugin!=null)
      //  EditorManager.RemoveMenuItemPlugin(lightmapMenuItemPlugin);
      if (_colorGradingTool!=null)
        EditorManager.RemoveMenuItemPlugin(_colorGradingTool);
      PrefabDesc.PrefabInstanceCreator = null;

      if (PrefabManager.BINARY_SAVER is VisionPrefabBinarySaver)
        PrefabManager.BINARY_SAVER = null;
      VisionEngineManager.EntityClassManagerType = null;

      VisionManaged.ManagedModule.DeInitManagedModule();
      foreach (IShapeCreatorPlugin plugin in creators)
        EditorManager.ShapeCreatorPlugins.Remove( plugin );

      return true;
    }

    IShapeCreatorPlugin[] creators;
    //IMenuItemPlugin lightmapMenuItemPlugin;
    ColorGradingToolPlugin _colorGradingTool;

    #endregion

    #region global listeners

    private void IProject_NewProjectLoaded(object sender, EventArgs e)
    {
      PostProcessingShape.PropertyManager = null; // force a rebuild of the property list
      EntityShape._cachedEntitySourcePluginInfo.Clear();
      LODManager.ClearSetups();
    }

    private void IProject_ProjectUnloaded(object sender, EventArgs e)
    {
      PostProcessingShape.PropertyManager = null; // force a rebuild of the property list
      EntityShape._cachedEntitySourcePluginInfo.Clear();
      LODManager.ClearSetups();
    }

    void EditorManager_SceneEvent(object sender, SceneEventArgs e)
    {
      if ((e.action == SceneEventArgs.Action.AfterStaticLighting || e.action == SceneEventArgs.Action.StaticLightingReset || e.action == SceneEventArgs.Action.StaticLightingLoaded) && e.Success)
      {
        foreach (Layer layer in EditorManager.Scene.Layers)
          layer.Root.RunVisitor(ParticleGroupShape.LIGHTING_CHANGED);
      }
    }

    void EditorManager_SceneChanged(object sender, SceneChangedArgs e)
    {
      if (e.newscene != null && DynLightShape.g_iCoronasConverted > 0)
      {
        EditorManager.ShowMessageBox(
          "One or more light shapes with coronas have been loaded and converted.\n\nCoronas are now enabled through a 'Corona' component attached to a light.", 
          "Automatic conversion of Corona properties", MessageBoxButtons.OK, MessageBoxIcon.Information);
        EditorManager.Scene.Dirty = true;
      }
      DynLightShape.g_iCoronasConverted = 0;
    }

    void EditorManager_ProcessExternalFileDrop(object sender, ExternalFileDropArgs e)
    {
      if (!e.Data.GetDataPresent(DataFormats.FileDrop))
        return;
      if (EditorManager.Scene == null)
        return;

      string filename = ((string[])e.Data.GetData(DataFormats.FileDrop))[0];
      if (!EditorManager.Project.CheckRelative(filename))
      {
        EditorManager.ShowMessageBox("Cannot drop file \n\n" + filename + "\n\n because it is not project relative", "Cannot drop object", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
        return;
      }
      filename = EditorManager.Project.MakeRelative(filename);
      string extension = Path.GetExtension(filename);
      string name = Path.GetFileNameWithoutExtension(filename);

      if (string.Compare(extension, ".model", true) == 0)
      {
        EntityShape shape = new EntityShape(name);
        shape.ModelFile = filename; 
        EditorManager.ActiveView.DropObject(shape, e);
      }
      else if (string.Compare(extension, ".vmesh", true) == 0)
      {
        StaticMeshShape shape = new StaticMeshShape(name);
        shape.MeshFileName = filename;
        EditorManager.ActiveView.DropObject(shape, e);
      }
      else if (string.Compare(extension, ".prefab", true) == 0)
      {
        PrefabShape shape = new PrefabShape(name);
        shape.Filename = filename;
        EditorManager.ActiveView.DropObject(shape, e);
      }
    }

    void EditorManager_QueryDragDropContext(object sender, QueryDragDropContextArgs e)
    {
      string[] formats = e.Data.GetFormats(false);
      if (!e.Data.GetDataPresent("application/assetPath"))
        return;

      string[] assetPaths = CSharpFramework.Contexts.IDropContext.GetAssetPaths(e);
      string[] assetTypes = CSharpFramework.Contexts.IDropContext.GetAssetTypes(e);
      if (assetPaths.Length == 0 || assetTypes.Length == 0)
        return;

      // We have a context type that can work with geometry assets so we set the context variable to an instance of it.
      // Currently we only support the first element of the drop operation.
      if (assetTypes[0] == "Model" || assetTypes[0] == "StaticMesh" || assetTypes[0] == "Prefab")
      {
        e._context = new Contexts.GeometryAsssetDropContext();
      }

    }

    #endregion


    #region Migrate : CastDynamicShadows in deferredmode

    class SetCastShadowFlagVisitor : IShapeVisitor
    {

      public override IShapeVisitor.VisitResult Visit(CSharpFramework.Shapes.ShapeBase shape)
      {
        if (!shape.Modifiable)
          return VisitResult.VisitOk;
        bool bAddAction = false;
        if (shape is EntityShape)
          if (!((EntityShape)shape).CastDynamicShadows)
            bAddAction = true;

        if (shape is StaticMeshShape)
          if (!((StaticMeshShape)shape).CastDynamicShadows)
            bAddAction = true;

        if (bAddAction)
          Action.Add(SetPropertyAction.CreateSetPropertyAction(shape, "CastDynamicShadows", true));

        return VisitResult.VisitOk;
      }

      public GroupAction Action = new GroupAction("Set CastDynamicShadows shadow flag");

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
