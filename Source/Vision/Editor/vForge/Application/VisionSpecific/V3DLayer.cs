/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Design;
using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using System.Windows.Forms;
using System.Windows.Forms.Design;
using CSharpFramework;
using CSharpFramework.Actions;
using CSharpFramework.BaseTypes;
using CSharpFramework.Clipboard;
using CSharpFramework.Dialogs;
using CSharpFramework.DynamicProperties;
using CSharpFramework.Math;
using CSharpFramework.PropertyEditors;
using CSharpFramework.Scene;
using CSharpFramework.Serialization;
using CSharpFramework.Shapes;
using CSharpFramework.UndoRedo;
using CSharpFramework.View;
using CSharpFramework.Visitors;
using Editor.Dialogs;
using Editor.VisionSpecific;
using ManagedBase;
using ManagedFramework;
using VisionEditorPlugin.Shapes;
using CSharpFramework.AssetManagement;

namespace Editor
{
  #region Class RendererSetupTypeEditor

  /// <summary>
  /// UITypeEditor to support texture atlas editing to a string filename property
  /// </summary>
  public class RendererSetupTypeEditor : UITypeEditor
  {
    /// <summary>
    /// Constructor
    /// </summary>
    public RendererSetupTypeEditor()
    {
      _listBox.SelectedIndexChanged += new EventHandler(_listBox_SelectedIndexChanged);
    }

    #region overridden UITypeEditor functions

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="context"></param>
    /// <returns></returns>
    [System.Security.Permissions.PermissionSet(System.Security.Permissions.SecurityAction.Demand, Name = "FullTrust")]
    public override UITypeEditorEditStyle GetEditStyle(System.ComponentModel.ITypeDescriptorContext context)
    {
      return UITypeEditorEditStyle.DropDown;
    }

    /// <summary>
    /// Service instances passed to the EditValue function
    /// </summary>
    private IWindowsFormsEditorService editorService = null;

    private ListBox _listBox = new ListBox();
    private int ITEM_COPY = -2;
    private int ITEM_PASTE = -2;
    private int ITEM_LOAD = -2;
    private int ITEM_SAVE = -2;
    private int ITEM_EXPORT = -2;
    RendererSetup _rendererSetup;
    V3DLayer _layer;
    RendererSetup _clipboardVersion = null;

    /// <summary>
    /// Overriden Edit value function
    /// </summary>
    /// <param name="context"></param>
    /// <param name="provider"></param>
    /// <param name="value"></param>
    /// <returns></returns>
    [RefreshProperties(RefreshProperties.All)]
    [System.Security.Permissions.PermissionSet(System.Security.Permissions.SecurityAction.Demand, Name = "FullTrust")]
    public override object EditValue(System.ComponentModel.ITypeDescriptorContext context, IServiceProvider provider, object value)
    {
      editorService = provider.GetService(typeof(IWindowsFormsEditorService)) as IWindowsFormsEditorService;
      _rendererSetup = value as RendererSetup;
      _layer = context.Instance as V3DLayer;

      _listBox.Items.Clear();

      if (_rendererSetup == null || _layer == null)
        return value;

      bool bHasNodeClass = !string.IsNullOrEmpty(_rendererSetup._rendererNodeClass) && _rendererSetup._rendererNodeClass != EditorManager.RendererNodeManager.DefaultRendererNodeClass;
      _clipboardVersion = EditorManager.Clipboard.DataObject as RendererSetup;

      if (bHasNodeClass)
        ITEM_COPY = _listBox.Items.Add("Copy");
      if (_clipboardVersion != null)
        ITEM_PASTE = _listBox.Items.Add("Paste");
      ITEM_LOAD = _listBox.Items.Add("Load from external file");
      if (bHasNodeClass)
      {
        ITEM_SAVE = _listBox.Items.Add("Save to external file");
        ITEM_EXPORT = _listBox.Items.Add("Export to engine file");
      }

      editorService.DropDownControl(_listBox);
      return _rendererSetup; // might have changed
    }

    #endregion

    #region ListBox

    private void _listBox_SelectedIndexChanged(object sender, EventArgs e)
    {
      int iSelIndex = _listBox.SelectedIndex;

      if (iSelIndex == ITEM_COPY)
      {
        EditorManager.Clipboard.Data = new RendererSetupClipboardData((RendererSetup)_rendererSetup.Clone());
      }
      if (iSelIndex == ITEM_PASTE)
      {
        _rendererSetup = (RendererSetup)_clipboardVersion.Clone();  // do not need to send action - just return differnt return value
        //EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(_layer,"RendererSetup",_clipboardVersion.Clone()));
      }
      if (iSelIndex == ITEM_LOAD)
      {
        LoadRendererSettings();
      }
      if (iSelIndex == ITEM_SAVE)
      {
        SaveRendererSettings();
      }
      if (iSelIndex == ITEM_EXPORT)
      {
        ExportRendererSettings();
      }

      editorService.CloseDropDown();
    }

    #endregion

    #region Load, Save and Export Settings

    void SaveRendererSettings()
    {
      SaveFileDialog dlg = new SaveFileDialog();
      dlg.InitialDirectory = EditorManager.Project.ProjectDir;
      dlg.FileName = null;
      dlg.Filter = "Renderer Setup|*.Renderer";
      dlg.Title = "Save this renderer setup to editor format";

      if (dlg.ShowDialog() != DialogResult.OK)
        return;

      string filename = dlg.FileName;
      dlg.Dispose();

      try
      {
        BinaryFormatter fmt = SerializationHelper.BINARY_FORMATTER;
        FileStream fs = new FileStream(filename, FileMode.Create);
        fmt.Serialize(fs, _rendererSetup);
        fs.Close();
      }
      catch (Exception ex)
      {
        EditorManager.ShowMessageBox("Failed to save renderer setup to file:\n\n" + ex.Message, "Error saving file", MessageBoxButtons.OK, MessageBoxIcon.Error);
      }
    }

    void LoadRendererSettings()
    {
      OpenFileDialog dlg = new OpenFileDialog();
      dlg.InitialDirectory = EditorManager.Project.ProjectDir;
      dlg.FileName = null;
      dlg.Filter = "Renderer Setup|*.Renderer";
      dlg.Title = "Load renderer setup from file";

      if (dlg.ShowDialog() != DialogResult.OK)
        return;

      string filename = dlg.FileName;
      dlg.Dispose();

      RendererSetup _setup = null;
      try
      {
        BinaryFormatter fmt = SerializationHelper.BINARY_FORMATTER;
        FileStream fs = new FileStream(filename, FileMode.Open, FileAccess.Read);
        _setup = (RendererSetup)fmt.Deserialize(fs);
        fs.Close();
      }
      catch (Exception ex)
      {
        EditorManager.ShowMessageBox("Failed to load renderer setup from file:\n\n" + ex.Message, "Error loading file", MessageBoxButtons.OK, MessageBoxIcon.Error);
      }
      if (_setup != null)
        _rendererSetup = _setup; // new return value
    }

    void ExportRendererSettings()
    {
      SaveFileDialog dlg = new SaveFileDialog();
      dlg.InitialDirectory = EditorManager.Project.ProjectDir;
      dlg.FileName = null;
      dlg.Filter = "Exported Renderer setup|*.VRenderer";
      dlg.Title = "Export renderer setup to engine-readable archive file";

      if (dlg.ShowDialog() != DialogResult.OK)
        return;

      string filename = dlg.FileName;
      dlg.Dispose();
      try
      {
        if (!EditorManager.RendererNodeManager.ExportRendererNode(filename))
          throw new Exception("Native serialization failed");
      }
      catch (Exception ex)
      {
        EditorManager.ShowMessageBox("Failed to export renderer setup to file:\n\n" + ex.Message, "Error asving file", MessageBoxButtons.OK, MessageBoxIcon.Error);
      }
    }

    #endregion
  }

  #endregion

  #region Class RendererSetupClipboardData

  /// <summary>
  /// Custom clipboard object that can paste
  /// </summary>
  [Serializable]
  public class RendererSetupClipboardData : ClipboardData
  {
    public RendererSetupClipboardData(RendererSetup renderer)
      : base(renderer, "Renderer Setup")
    {
    }

    public override bool TryPaste(object opHint)
    {
      if (base.TryPaste(opHint))
        return true;

      if (EditorManager.Scene == null || this.Object == null)
        return false;

      foreach (Layer layer in EditorManager.Scene.Layers)
      {
        if (layer.Modifiable && (layer is V3DLayer))
        {
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(layer, "RendererSetup", this.Object));
          return true;
        }

      }
      return false;
    }
  }

  #endregion

  /// <summary>
  /// Custom layer type for "Main Layer" with some specific properties. There is one layer of this type in every scene.
  /// </summary>
  [Serializable]
  public class V3DLayer : Layer
  {
    ProfileSpecificSettings _currentSettings = null;
    // Override that prevents early out in EnableTimeOfDay and RendererNodeClass properties
    bool _forceSetSettings = false;

    #region Constructor

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="name">layer name</param>
    public V3DLayer(string name)
      : base(name)
    {
      _currentSettings = new ProfileSpecificSettings();
      _currentSettings._skyConfig = new VisionSky();
    }

    #endregion

    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_V3D = "Map";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_V3D = Layer.LAST_CATEGORY_ORDER_ID + 1;


    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_VIEW = "View";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_VIEW = Layer.LAST_CATEGORY_ORDER_ID + 2;

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_ATMOSPHERE = "Atmospherical Effects";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_ATMOSPHERE = Layer.LAST_CATEGORY_ORDER_ID + 3;

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_RENDERER = "Renderer";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_RENDERER = Layer.LAST_CATEGORY_ORDER_ID + 4;

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_SCRIPTING = "Scripting";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_SCRIPTING = Layer.LAST_CATEGORY_ORDER_ID + 5;

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_LOD_HYSTERESIS = "LOD Hysteresis";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_LOD_HYSTERESIS = Layer.LAST_CATEGORY_ORDER_ID + 6;

    /// <summary>
    /// last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_LOD_HYSTERESIS;

    #endregion

    void IProfileManager_ActiveProfileChanged(object sender, ActiveProfileChangedArgs e)
    {
      _forceSetSettings = true;

      try
      {
        ProfileSpecificSettings newSettings = e._activeProfile.GetProfileSettings();
        _currentSettings._eSRGBMode = newSettings._eSRGBMode;
        _currentSettings._bWantsTimeOfDay = newSettings._bWantsTimeOfDay;
        _currentSettings._defaultLightColor = newSettings._defaultLightColor;

        NearClipDistance = newSettings._fNearClip;
        FarClipDistance = newSettings._fFarClip;
        FOV = newSettings._fFovX;

        // Sky
        SkyConfig = newSettings._skyConfig;
        if (SkyConfig == null)
        {
          SkyConfig = new VisionSky();
        }

        // Fog
        FogSetup = newSettings._fogSetup;
        if (FogSetup == null)
        {
          FogSetup = new FogSetup();
        }

        // Time of Day
        TimeOfDay = newSettings._timeOfDay;
        EditorManager.RendererNodeManager.UpdateTimeOfDay(TimeOfDay);

        // Renderer
        RendererNodeClass = newSettings._rendererSetup._rendererNodeClass;
        RendererProperties = newSettings._rendererSetup._rendererProperties;
        Postprocessors = newSettings._rendererSetup._rendererComponents;

        EditorManager.EngineManager.SetFogParameters(FogSetup);
        EditorManager.EngineManager.SetSRGBMode(newSettings._eSRGBMode);

        //this event is necessary to update the property flags
        IScene.SendLayerChangedEvent(new LayerChangedArgs(this, null, LayerChangedArgs.Action.PropertyChanged));
      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex, true);
      }

      _forceSetSettings = false;
    }

    void IProfileManager_GetCurrentSettings(object sender, GetCurrentSettingsCallbackArgs e)
    {
      e._settings = _currentSettings;
    }

    #region Overridden functions

    /// <summary>
    /// this is called by assigning a new scene, e.g. after it has been loaded
    /// </summary>
    public override void OnActivateScene()
    {
      base.OnActivateScene();

      // default light color
      EditorManager.EngineManager.SetDefaultLightColor(VisionColors.Get(_currentSettings._defaultLightColor));

      // load V3D without recreating the engine instances, since this will be done
      // by EditorApp.Scene after all layers have been loaded
      if (!string.IsNullOrEmpty(_v3dFilename))
      {
        EditorManager.ShowMessageBox("The scene referenced a V3D file called '" + _v3dFilename + "'.\n" +
                        "Loading of V3D geometry is not supported anymore so the V3D has been removed from scene.\n\n" +
                        "Please re-export geometry to static meshes or use the conversion tool",
                        "V3D support discontinued", MessageBoxButtons.OK, MessageBoxIcon.Information);
        _v3dFilename = null;
        this.Dirty = true;
      }

      if (ShapeBase.ScriptPropertyMigrated)
      {
        ShapeBase.ScriptPropertyMigrated = false;
        EditorManager.ShowMessageBox("Lua script properties have been converted into components that are attached to the object(s).\n\nPlease switch to 'Component Editing Layout' to see/edit the script components", "Migration from 8.0 scenes", MessageBoxButtons.OK, MessageBoxIcon.Information);
      }

      // set the new camera position
      EditorManager.ActiveView.SetCameraPosition(EditorApp.Scene.Settings.CameraPosition);
      EditorManager.ActiveView.SetCameraRotation(EditorApp.Scene.Settings.CameraAngles);

      // set LOD hysteresis params
      EditorManager.EngineManager.SetWorldGeometryThreshold(_fWorldGeometryThreshold);
      EditorManager.EngineManager.SetEntityThreshold(_fEntityThreshold);

      // also update the sky
      if (SkyConfig != null)
      {
        SkyConfig.Parent = this;
        SkyConfig.Active = true;
        SkyConfig.Update();
      }

      //..and fog
      if (_currentSettings._fogSetup == null)
      {
        _currentSettings._fogSetup = new FogSetup();
        _currentSettings._fogSetup.FogSetupChanged += new CSharpFramework.Scene.FogSetup.FogSetupChangedHandler(OnFogSetupChanged);
      }

      if (_currentSettings._timeOfDay != null)
      {
        _currentSettings._timeOfDay.Owner = this;
      }

      try
      {
        if (Modifiable)
        {
          MigratePostprocessingShapes();

          // old scene import?
          if (_renderer != null && RendererNodeClass == IRendererNodeManager.RENDERERNODECLASS_NONE)
          {
            CheckRendererNodeRequirementVisitor visitor = new CheckRendererNodeRequirementVisitor();
            EditorManager.Scene.RunShapeVisitor(visitor);
            if (visitor.NeedsRendererNodeClass)
              RendererNodeClass = IRendererNodeManager.RENDERERNODECLASS_FORWARD;
            else
              RendererNodeClass = IRendererNodeManager.RENDERERNODECLASS_SIMPLE;  // Always use a simple renderer node rather than none
          }
        }
      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex, true);
      }

      // First make sure there is a renderer node, then check if it supports time of day, then recreate it with the time of day settings
      EditorManager.RendererNodeManager.UpdateRendererNode(RendererProperties, Postprocessors);
      EditorManager.RendererNodeManager.UpdateTimeOfDay(FinalEnableTimeOfDay ? this.TimeOfDay : null);
      EditorManager.RendererNodeManager.SetCurrentTime(CurrentTime);
      EditorManager.RendererNodeManager.UpdateRendererNode(RendererProperties, Postprocessors);
      _currentSettings._fogSetup.Update();
      // sky may depend on the renderer node, so update it again
      if (SkyConfig != null)
      {
        SkyConfig.Update();
      }
      EditorManager.EngineManager.SetSRGBMode(_currentSettings._eSRGBMode);

      // set clip distances
      ViewSettings view = EditorManager.ActiveView.DefaultViewSettings;
      view.NearClipDistance = _currentSettings._fNearClip;
      view.FarClipDistance = _currentSettings._fFarClip;
      view.FOV = new Vector2F(_currentSettings._fFovX, 0.0f);
      EditorManager.ActiveView.SetViewSettings(view, true);

      // now performed on each engine instance creation
      //ScriptManager.SetSceneScriptFile(_sceneScriptFile);

      IScene.SendLayerChangedEvent(new LayerChangedArgs(this, null, LayerChangedArgs.Action.PropertyChanged)); // this event is necessary to update the property flags

      // execute pending actions
      if (_pendingActions != null)
      {
        if (this.Modifiable)
        {
          foreach (IAction action in _pendingActions)
            EditorManager.Actions.Add(action);
        }
        else
        {
          EditorManager.ShowMessageBox("Scene migration from older versions cannot be completed since the main layer is locked.\nPlease make the main layer editable and reload the scene", "Scene migration incomplete", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        }
        _pendingActions = null;
      }

      // In case this is a newly created scene or an old scene that needs to be migrated to the new format we do so here.
      if (_currentSettings._needsMigration)
      {
        EditorManager.ProfileManager.LoadProfileSpecificSettings(EditorManager.Project.Scene.AbsoluteFileName + ".Layers\\", _currentSettings, true);
      }
    }


    protected override void DisposeObject()
    {
      if (ParentScene != null)
      {
        EditorManager.ProfileManager.ActiveProfileChanged -= new IProfileManager.ActiveProfileChangedEventHandler(IProfileManager_ActiveProfileChanged);
        EditorManager.ProfileManager.GetCurrentSettings -= new IProfileManager.GetCurrentSettingsEventHandler(IProfileManager_GetCurrentSettings);
        IScene.PropertyChanged -= new CSharpFramework.PropertyChangedEventHandler(IScene_PropertyChanged);
      }

      if (_currentSettings != null && _currentSettings._skyConfig != null)
      {
        _currentSettings._skyConfig.Dispose();
      }
      _currentSettings = null;

      base.DisposeObject();
    }


    public override void OnReassignAllLinks()
    {
      base.OnReassignAllLinks();

      // in VSceneLoader the scene script also set after shapes .
      // OnReassignAllLinks is a good entry point, because all engine instance have been created
      ScriptManager.SetSceneScriptFile(_sceneScriptFile);
      ScriptManager.SetThinkInterval(_scriptThinkInterval);
    }

    void IScene_PropertyChanged(object sender, PropertyChangedArgs e)
    {
      if (object.ReferenceEquals(e._component, _currentSettings._rendererSetup._rendererProperties) && _currentSettings._rendererSetup._rendererProperties != null)
        EditorManager.RendererNodeManager.UpdateRendererNodeProperty(_currentSettings._rendererSetup._rendererProperties.GetPropertyByName(e._propertyName));
    }

    public override void OnLayerActionTriggered(LayerChangedArgs.Action action)
    {
      base.OnLayerActionTriggered(action);
      // we do not listen to the Clicked event anymore because that one is sent too often. [#3614]
      if (/*action == LayerChangedArgs.Action.Clicked || */action == LayerChangedArgs.Action.Selected)
      {
        EditorManager.ActiveView.SetViewSettings(EditorManager.ActiveView.DefaultViewSettings, true); // set back default view settings
        EditorManager.ActiveView.UpdateView(false);
      }
    }

    /// <summary>
    /// This main layer can never be attached to any zone
    /// </summary>
    /// <param name="zone"></param>
    /// <returns></returns>
    public override bool CanAttachToZone(Zone zone)
    {
      return false;
    }
    /*
        /// <summary>
        /// Initiates the export of all engine instances in this layer
        /// </summary>
        /// <returns></returns>
        public override bool OnExport(SceneExportInfo info)
        {
          return _renderer.Export(info);
        }
    */
    /// <summary>
    /// Additionally gets dependencies  for sky etc.
    /// </summary>
    /// <param name="info"></param>
    public override void GetDependencies(ResourceSnapshotInfo info)
    {
      base.GetDependencies(info);
      if (SkyConfig != null)
        SkyConfig.GetDependencies(info);
    }


    public override PropertyFlags_e GetPropertyFlags(PropertyFlagsProviderInfo pd)
    {
      PropertyFlags_e flags = base.GetPropertyFlags(pd);
      EditorSettingsBase.CoordinateDisplayMode_e cd = EditorManager.Settings.CoordinateDisplay;
      if ((cd & EditorSettingsBase.CoordinateDisplayMode_e.Geodetic) == 0)
      {
        if (pd.Name == "Location_Lat" || pd.Name == "Location_Long" || pd.Name == "Location_Alt")
          return PropertyFlags_e.Hidden;
      }

      if (pd.Name == "RendererProperties" || pd.Name == "Postprocessors")
      {
        if (!HasRendererNode)
          return PropertyFlags_e.Hidden;
      }
      else if (pd.Name == "TimeOfDay" || pd.Name == "CurrentTime")
      {
        if (!FinalEnableTimeOfDay)
          return PropertyFlags_e.Hidden;
      }
      else if (pd.Name == "EnableTimeOfDay")
      {
        if (!EditorManager.RendererNodeManager.SupportsTimeOfDay() || !HasRendererNode)
          return PropertyFlags_e.Readonly;
      }
      else if (pd.Name == "DepthFog") // fog is used from ToD-curves instead
      {
        if (FinalEnableTimeOfDay)
          return PropertyFlags_e.Hidden;
      }
      else if (pd.Name == "EnableDepthFog")
      {
        if (!FinalEnableTimeOfDay)
          return PropertyFlags_e.Hidden;
      }
      else if (pd.Name == "AssumeSRGB")
      {
        if (!HasRendererNode)
          return PropertyFlags_e.Hidden;
      }
      return flags;
    }

    #endregion

    #region V3D loading related

    string _v3dFilename;

    /// <summary>
    /// Defines the default light color that is used when there is no lightmap file (.lit) or light grid (.vlg)
    /// </summary>
    [SortedCategory(CAT_V3D, CATORDER_V3D), PropertyOrder(2),
    Description("Defines the default light color that is used when there is no lightmap file (.lit) or light grid (.vlg)")]
    [EditorAttribute(typeof(StandardColorDropDownEditor), typeof(UITypeEditor)), DefaultValue(typeof(Color), "255,255,255")]
    public Color DefaultLightColor
    {
      get { return _currentSettings._defaultLightColor; }
      set
      {
        if (_currentSettings._defaultLightColor == value) return;
        _currentSettings._defaultLightColor = value;
        EditorManager.EngineManager.SetDefaultLightColor(VisionColors.Get(_currentSettings._defaultLightColor));
      }
    }


    #endregion

    #region Scene Reference Position

    Vector3D _sceneReferenceLocation = Vector3D.Zero; // LatLongAlt coordinates

    /// <summary>
    /// Set the lat long alt value of the scene reference position
    /// </summary>
    [Browsable(false)]
    public Vector3D SceneReferenceLocation
    {
      get
      {
        return _sceneReferenceLocation;
      }
      set
      {
        _sceneReferenceLocation = value;
        CSConversionHelpers.SetSceneReferencePosition(_sceneReferenceLocation.X, _sceneReferenceLocation.Y, _sceneReferenceLocation.Z);
      }
    }

    [SortedCategory(CAT_V3D, CATORDER_V3D), PropertyOrder(50)]
    [Description("Reference coordinate of the scene's pivot")]
    public string Location_Lat
    {
      get
      {
        return AngleToStringConversion.AngleToString(SceneReferenceLocation.X);
      }
      set
      {
        SceneReferenceLocation = new Vector3D(AngleToStringConversion.StringToAngle(value), _sceneReferenceLocation.Y, _sceneReferenceLocation.Z);
      }
    }

    [SortedCategory(CAT_V3D, CATORDER_V3D), PropertyOrder(51)]
    [Description("Reference coordinate of the scene's pivot")]
    public string Location_Long
    {
      get
      {
        return AngleToStringConversion.AngleToString(SceneReferenceLocation.Y);
      }
      set
      {
        SceneReferenceLocation = new Vector3D(_sceneReferenceLocation.X, AngleToStringConversion.StringToAngle(value), _sceneReferenceLocation.Z);
      }
    }

    [SortedCategory(CAT_V3D, CATORDER_V3D), PropertyOrder(52)]
    [Description("Reference coordinate of the scene's pivot")]
    public double Location_Alt
    {
      get
      {
        return SceneReferenceLocation.Z;
      }
      set
      {
        SceneReferenceLocation = new Vector3D(_sceneReferenceLocation.X, _sceneReferenceLocation.Y, value);
      }
    }

    #endregion

    #region Scripting

    string _sceneScriptFile = null;

    /// <summary>
    /// Defines the script file that is executed per scene
    /// </summary>
    [SortedCategory(CAT_SCRIPTING, CATORDER_SCRIPTING), PropertyOrder(1)]
    [EditorAttribute(typeof(ScriptFilenameEditor), typeof(UITypeEditor))]
    [Description("Defines the script file that is executed per scene")]
    public string SceneScriptFile
    {
      get { return _sceneScriptFile; }
      set
      {
        if (_sceneScriptFile == value)
          return;
        _sceneScriptFile = value;
        ScriptManager.SetSceneScriptFile(_sceneScriptFile);
      }
    }

    float _scriptThinkInterval = 0;

    /// <summary>
    /// Defines the think intervall of all scripts
    /// </summary>
    [SortedCategory(CAT_SCRIPTING, CATORDER_SCRIPTING), PropertyOrder(2)]
    [Description("Defines the think intervall of all scripts managed by the VScriptResourceManager. Use 0 for every frame."), DefaultValue(0.0f)]
    public float ThinkIntervall
    {
      get { return _scriptThinkInterval; }
      set
      {
        if (_scriptThinkInterval == value)
          return;
        _scriptThinkInterval = value;
        ScriptManager.SetThinkInterval(_scriptThinkInterval);
      }
    }

    #endregion

    #region Sky config related

    /// <summary>
    /// get or set sky config of the layer
    /// </summary>
    [TypeConverter(typeof(UndoableObjectConverter))]
    [EditorAttribute(typeof(SkyConfigEditor), typeof(UITypeEditor)),
    SortedCategory(CAT_ATMOSPHERE, CATORDER_ATMOSPHERE),
    PropertyOrder(1),
    Description("Sky configuration")]
    public SkyConfig SkyConfig
    {
      get { return _currentSettings._skyConfig; }
      set
      {
        if (_currentSettings._skyConfig == value)
          return;

        if (_currentSettings._skyConfig != null)
        {
          _currentSettings._skyConfig.Dispose();
        }
        _currentSettings._skyConfig = value;

        if (_currentSettings._skyConfig != null)
        {
          _currentSettings._skyConfig.Parent = this;
          _currentSettings._skyConfig.Active = true;
        }
        if (_currentSettings._timeOfDay != null)
          _currentSettings._timeOfDay.SceneSky = this.SkyConfig;

      }
    }



    #endregion

    #region Fog config related


    #endregion

    #region Clip Planes related


    private void CheckNearFarClipRatio()
    {
      if (FarClipDistance / NearClipDistance > 12000)
      {
        EditorManager.ShowMessageBox("The ratio of NearClipDistance to FarClipDistance is very high. This can break many rendering effects (such as SSAO, Shadow Mapping, Lighting, etc.). If you must use such a high ratio, please ensure that all rendering effects still work as expected. However it is highly advised to reduce it.\nEither increase the value of NearClipDistance, or decrease the value for FarClipDistance.", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
      }
    }

    /// <summary>
    /// Gets or sets the near clip distance used for rendering
    /// </summary>
    [SortedCategory(CAT_VIEW, CATORDER_VIEW), PropertyOrder(1),
    Description("Near clip distance for the rendering"), DefaultValue(5.0f)]
    public float NearClipDistance
    {
      get { return _currentSettings._fNearClip; }
      set
      {
        if (_currentSettings._fNearClip == value)
          return;

        _currentSettings._fNearClip = value;
        if (_currentSettings._fNearClip <= 0.0f)
          _currentSettings._fNearClip = 0.001f;
        if (_currentSettings._fNearClip >= _currentSettings._fFarClip)
          FarClipDistance = _currentSettings._fFarClip; // this fixes the problem
        else
        {

          ViewSettings view = EditorManager.ActiveView.DefaultViewSettings;
          view.NearClipDistance = _currentSettings._fNearClip;
          view.FarClipDistance = _currentSettings._fFarClip;
          EditorManager.ActiveView.SetViewSettings(view, true);
          EditorManager.ActiveView.UpdateView(false);
          //Renderer.Update();
        }

        CheckNearFarClipRatio();
      }
    }

    /// <summary>
    /// Gets or sets the far clip distance used for rendering
    /// </summary>
    [SortedCategory(CAT_VIEW, CATORDER_VIEW), PropertyOrder(2),
    Description("Far clip distance for the rendering"), DefaultValue(32000.0f)]
    public float FarClipDistance
    {
      get { return _currentSettings._fFarClip; }
      set
      {
        if (_currentSettings._fFarClip == value)
          return;

        _currentSettings._fFarClip = value;
        if (_currentSettings._fFarClip <= _currentSettings._fNearClip)
          _currentSettings._fFarClip = _currentSettings._fNearClip + 1.0f;
        ViewSettings view = EditorManager.ActiveView.DefaultViewSettings;
        view.NearClipDistance = _currentSettings._fNearClip;
        view.FarClipDistance = _currentSettings._fFarClip;
        EditorManager.ActiveView.SetViewSettings(view, true);
        EditorManager.ActiveView.UpdateView(false);
        //Renderer.Update();

        CheckNearFarClipRatio();
      }
    }

    [Description("Sets the angle of the horizontal field-of-view in degrees. Dramatically decreasing this value might lead to depth buffer related artifacts.")]
    [SortedCategory(CAT_VIEW, CATORDER_VIEW), PropertyOrder(3)]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRangeAttribute(1.0f, 170.0f, 169), PropertyLiveUpdate("FOVLiveUpdate"),
    DefaultValue(90f)]
    public float FOV
    {
      get
      {
        return _currentSettings._fFovX;
      }
      set
      {
        _currentSettings._fFovX = value;
        ApplyFOV(_currentSettings._fFovX);
        _currentSettings._fFovX = EditorManager.ActiveView.CurrentViewSettings.FOV.X; // get back clamped value
        if (this.Modifiable)
          Modified = true;
      }
    }

    [Browsable(false)]
    public object FOVLiveUpdate
    {
      set
      {
        float fFOV = _currentSettings._fFovX;
        if (value is float)
          fFOV = (float)value;
        ApplyFOV(fFOV);
      }
    }

    void ApplyFOV(float fFOV)
    {
      ViewSettings view = EditorManager.ActiveView.CurrentViewSettings;
      view.FOV = new Vector2F(fFOV, 0.0f);
      EditorManager.ActiveView.SetViewSettings(view, true);
      EditorManager.ActiveView.UpdateView(false);
    }

    OrthographicViewBoxShape _orthographicViewBox;


    [Browsable(false)]
    public OrthographicViewBoxShape OrthographicViewBox
    {
      get
      {
        if (_orthographicViewBox == null)
        {
          OrthographicViewBoxShape box = new OrthographicViewBoxShape("Orthographic View");
          this.AddShape(box, null);
          _orthographicViewBox = box; // set here to to avoid recursion in SceneExtents
        }
        return _orthographicViewBox;
      }
    }

    [Browsable(false)]
    public bool HasValidOrthographicViewBox
    {
      get
      {
        return _orthographicViewBox != null && _orthographicViewBox.AbsBoundingBox.Valid;
      }
    }

    #endregion

    #region LOD Hysteresis related

    float _fWorldGeometryThreshold = 0.0f;
    float _fEntityThreshold = 0.0f;

    /// <summary>
    /// LOD hysteresis threshold for world geometry; 0 disables this feature
    /// </summary>
    [SortedCategory(CAT_LOD_HYSTERESIS, CATORDER_LOD_HYSTERESIS), PropertyOrder(1),
    Description("LOD hysteresis threshold for world geometry; 0 disables this feature"), DefaultValue(0.0f)]
    public float WorldGeometryThreshold
    {
      get { return _fWorldGeometryThreshold; }
      set
      {
        _fWorldGeometryThreshold = value;
        if (_fWorldGeometryThreshold < 0.0f)
          _fWorldGeometryThreshold = 0.0f;
        EditorManager.EngineManager.SetWorldGeometryThreshold(_fWorldGeometryThreshold);
        EditorManager.ActiveView.UpdateView(false);
      }
    }

    /// <summary>
    /// LOD hysteresis threshold for entities; 0 disables this feature
    /// </summary>
    [SortedCategory(CAT_LOD_HYSTERESIS, CATORDER_LOD_HYSTERESIS), PropertyOrder(2),
    Description("LOD hysteresis threshold for entities; 0 disables this feature"), DefaultValue(0.0f)]
    public float EntityThreshold
    {
      get { return _fEntityThreshold; }
      set
      {
        _fEntityThreshold = value;
        if (_fEntityThreshold < 0.0f)
          _fEntityThreshold = 0.0f;
        EditorManager.EngineManager.SetEntityThreshold(_fEntityThreshold);
        EditorManager.ActiveView.UpdateView(false);
      }
    }

    #endregion


    #region Renderer node system


    // the following collections remember the settings when switching
    Dictionary<string, DynamicPropertyCollection> _cachedRenderNodeProperties = new Dictionary<string, DynamicPropertyCollection>();
    Dictionary<string, ShapeComponentCollection> _cachedRenderNodePostProcessors = new Dictionary<string, ShapeComponentCollection>();


    [Browsable(true)]
    [PrefabRelevant(false)]
    [SortedCategory(CAT_RENDERER, CATORDER_RENDERER), PropertyOrder(1)]
    [EditorAttribute(typeof(RendererSetupTypeEditor), typeof(UITypeEditor))]
    public RendererSetup RendererSetup
    {
      get
      {
        _currentSettings._rendererSetup._Owner = this;
        return _currentSettings._rendererSetup;
      }
      set
      {
        if (_currentSettings._rendererSetup == value)
          return;
        if (value == null)
        {
          RendererNodeClass = null;
        }
        else
        {
          _currentSettings._rendererSetup = (RendererSetup)value.Clone();
          _currentSettings._rendererSetup._Owner = this;
          RendererNodeClass = value._rendererNodeClass;
          RendererProperties = value._rendererProperties;
          Postprocessors = value._rendererComponents;
        }
      }
    }

    [Browsable(true)]
    [SortedCategory(CAT_RENDERER, CATORDER_RENDERER), PropertyOrder(10)]
    [Description("Renderer node type")]
    [DependentProperties(new string[] { "RendererProperties", "Postprocessors" })]
    [RefreshProperties(RefreshProperties.All)]
    [TypeConverter(typeof(RendererNodeClassNameConverter))]
    public string RendererNodeClass
    {
      get
      {
        if (string.IsNullOrEmpty(_currentSettings._rendererSetup._rendererNodeClass))
          _currentSettings._rendererSetup._rendererNodeClass = EditorManager.RendererNodeManager.DefaultRendererNodeClass;
        return _currentSettings._rendererSetup._rendererNodeClass;
      }
      set
      {
        string old = RendererNodeClass;
        _currentSettings._rendererSetup._rendererNodeClass = value;
        if (RendererNodeClass == old && !_forceSetSettings)
          return;

        // remember old settings
        if (!string.IsNullOrEmpty(old))
        {
          _cachedRenderNodeProperties[old] = _currentSettings._rendererSetup._rendererProperties;
          _cachedRenderNodePostProcessors[old] = _currentSettings._rendererSetup._rendererComponents;
        }

        // properties set before?
        if (!_cachedRenderNodeProperties.TryGetValue(_currentSettings._rendererSetup._rendererNodeClass, out _currentSettings._rendererSetup._rendererProperties))
        {
          if (RendererNodeClass == IRendererNodeManager.RENDERERNODECLASS_NONE)
            _currentSettings._rendererSetup._rendererProperties = null;
          else
            _currentSettings._rendererSetup._rendererProperties = EditorManager.RendererNodeManager.NodeClassTypeManager.CreateCollection(this, RendererNodeClass);
        }

        // must be called before attaching postprocessors, otherwise CanAttachPostprocessor returns false
        EditorManager.RendererNodeManager.UpdateRendererNode(RendererProperties, null);
        EditorManager.RendererNodeManager.UpdateTimeOfDay(FinalEnableTimeOfDay ? this.TimeOfDay : null);

        // re-apply view settings
        ViewSettings view = EditorManager.ActiveView.DefaultViewSettings;
        view.NearClipDistance = _currentSettings._fNearClip;
        view.FarClipDistance = _currentSettings._fFarClip;
        view.FOV = new Vector2F(_currentSettings._fFovX, 0.0f);
        EditorManager.ActiveView.SetViewSettings(view, true);

        // check which postprocessors shall be attached by default
        ShapeComponentCollection newPostProcessors = new ShapeComponentCollection();
        ShapeComponentCollection oldPostProcessors = Postprocessors;
        ShapeComponentCollection cachedPostProcessors = null;
        _cachedRenderNodePostProcessors.TryGetValue(_currentSettings._rendererSetup._rendererNodeClass, out cachedPostProcessors);

        ShapeComponentType[] postProcessorTypes = EditorManager.RendererNodeManager.ComponentTypes;
        foreach (ShapeComponentType compType in postProcessorTypes)
        {
          if (compType.Hidden)
            continue;
          if (!EditorManager.RendererNodeManager.CanAttachPostprocessor(compType.ProbeComponent))
            continue;
          ShapeComponent comp = null;
          // if possible, migrate from the old renderer assignment:
          if (oldPostProcessors != null)
            comp = oldPostProcessors.GetComponentByType(compType);
          if (comp == null)
          {
            // then see if there is an earlier version of it
            if (cachedPostProcessors != null)
              comp = cachedPostProcessors.GetComponentByType(compType);

            // still null? Then create a new instance
            if (comp == null)
            {
              if (!EditorManager.RendererNodeManager.WantsDefaultPostprocessor(compType.ProbeComponent))
                continue;
              comp = (ShapeComponent)compType.CreateInstance(this);
            }
          }
          newPostProcessors.Add(comp);
        }

        // set new collection of postprocessors (calls UpdateRendererNode again which is important). Note this list is sorted already
        Postprocessors = newPostProcessors;

        if (SkyConfig != null)
          SkyConfig.Update();

        if (_currentSettings._fogSetup != null)
          _currentSettings._fogSetup.Update();


        EditorManager.TriggerSceneEvent(SceneEventArgs.Action.AfterRendererNodeChanged, true);
        // Don't send out an event during layer de-serialization.
        if (this.ParentScene != null)
        {
          IScene.SendLayerChangedEvent(new LayerChangedArgs(this, null, LayerChangedArgs.Action.PropertyChanged)); // this event is necessary to update the property flags
          IScene.SendShaderResourceChangedEvent(new EventArgs()); // this event is necessary to ensure that the material editor gets informed about the renderer node change
        }
      }
    }

    [Browsable(false)]
    public bool HasRendererNode
    {
      get
      {
        return RendererNodeClass != IRendererNodeManager.RENDERERNODECLASS_NONE;
      }
    }


    [Browsable(true)]
    [SortedCategory(CAT_RENDERER, CATORDER_RENDERER), PropertyOrder(11)]
    [TypeConverter(typeof(DynamicPropertyConverter))]
    [Description("Properties associated with the renderer node")]
    public DynamicPropertyCollection RendererProperties
    {
      get { return _currentSettings._rendererSetup._rendererProperties; }
      set
      {
        _currentSettings._rendererSetup._rendererProperties = value;
      }
    }

    [Browsable(true)]
    [SortedCategory(CAT_RENDERER, CATORDER_RENDERER), PropertyOrder(12)]
    [Description("Postprocessors attached to the renderer node")]
    [TypeConverter(typeof(ShapeComponentTypeConverter))]
    [EditorAttribute(typeof(RendererComponentEditor), typeof(UITypeEditor))]
    public ShapeComponentCollection Postprocessors
    {
      get { return _currentSettings._rendererSetup._rendererComponents; }
      set
      {
        _currentSettings._rendererSetup._rendererComponents = value;

        if (_currentSettings._rendererSetup._rendererComponents != null)
          foreach (ShapeComponent c in _currentSettings._rendererSetup._rendererComponents)
            c.Owner = this;

        EditorManager.RendererNodeManager.UpdateRendererNode(RendererProperties, Postprocessors);
      }
    }

    [SortedCategory(CAT_RENDERER, CATORDER_RENDERER), PropertyOrder(13)]
    [Description("Sets how the Engine handles textures with regard to sRGB. If set to \"Disable\", no adjustments are made. If set to \"AssumeForDiffuse\", all diffuse textures are assumed to be in sRGB color space. If set to \"FromMetadata\", the color space is determined by the setting in each texture's asset metadata.")]
    public SRGBMode_e SRGBMode
    {
      get { return _currentSettings._eSRGBMode; }
      set
      {
        DynamicProperty gammaCorrectionProperty = null;
        if (_currentSettings._eSRGBMode != value)
        {
          gammaCorrectionProperty = RendererProperties.GetPropertyByName("GammaCorrection");
        }

        if (gammaCorrectionProperty != null)
        {
          object[] values = gammaCorrectionProperty.PropertyType.GetEnumValues();
          int enumIndex = 0;
          foreach (object val in values)
          {
            if (val.Equals(gammaCorrectionProperty.Value))
            {
              break;
            }
            enumIndex++;
          }

          bool disable = (value == SRGBMode_e.Disable);
          if ((enumIndex == 0) != disable)
          {
            string action = disable ? "disable" : "enable";
            DialogResult result = MessageBox.Show("The texture color space setting has been modified. Would you like to " + action + " gamma correction?", "Texture space setting changed.", MessageBoxButtons.YesNoCancel);

            switch (result)
            {
              case DialogResult.Yes:
                RendererProperties.SetPropertyValue("GammaCorrection", values[disable ? 0 : 1]);
                EditorManager.RendererNodeManager.UpdateRendererNode(RendererProperties, Postprocessors);
                break;
              case DialogResult.No:
                break;
              case DialogResult.Cancel:
                return;
            }
          }
        }

        _currentSettings._eSRGBMode = value;
        EditorManager.EngineManager.SetSRGBMode(_currentSettings._eSRGBMode);
      }
    }

    [Browsable(true)]
    [SortedCategory(CAT_ATMOSPHERE, CATORDER_ATMOSPHERE), PropertyOrder(23)]
    [EditorAttribute(typeof(TimeOfDayEditor), typeof(UITypeEditor))]
    [Description("Enables time of day handling for the renderer node (if supported)")]
    public bool EnableTimeOfDay
    {
      get
      {
        return _currentSettings._bWantsTimeOfDay;
      }
      set
      {
        if (_currentSettings._bWantsTimeOfDay == value && !_forceSetSettings)
          return;
        _currentSettings._bWantsTimeOfDay = value;
        if (FinalEnableTimeOfDay)
        {
          EditorManager.RendererNodeManager.UpdateTimeOfDay(this.TimeOfDay);
          EditorManager.RendererNodeManager.SetCurrentTime(CurrentTime);
        }
        else
        {
          EditorManager.RendererNodeManager.UpdateTimeOfDay(null);
          if (_currentSettings._skyConfig != null)
            _currentSettings._skyConfig.Update();
          if (_currentSettings._fogSetup != null)
            _currentSettings._fogSetup.Update();
        }
        TimeOfDay.TriggerTimeOfDayEnabledChanged();
        EditorManager.RendererNodeManager.UpdateRendererNode(RendererProperties, Postprocessors);


        if (!FinalEnableTimeOfDay)
        {
          // Resubmit light shape properties to reset time-of-day-controlled properties to the displayed PropertyGrid values
          ShapeComponentType componentType = (ShapeComponentType)EditorManager.EngineManager.ComponentClassManager.GetCollectionType("VTimeOfDayComponent");

          if (componentType != null)
          {
            ShapeCollection allShapes = EditorManager.Scene.AllShapesOfType(typeof(DynLightShape));
            foreach (Shape3D shape in allShapes)
            {
              DynLightShape light = shape as DynLightShape;

              // Not entirely sure how the component container can be null, but it happened in one scene causing a crash
              if (light.Components == null)
                continue;

              ShapeComponent timeOfDayAttachment = light.Components.GetComponentByType(componentType);
              if (timeOfDayAttachment != null)
                light.SetEngineInstanceBaseProperties();
            }
          }
        }
      }
    }

    [Browsable(false)]
    public bool FinalEnableTimeOfDay
    {
      get
      {
        return _currentSettings._bWantsTimeOfDay && EditorManager.RendererNodeManager.SupportsTimeOfDay();
      }
    }

    [Browsable(true)]
    [SortedCategory(CAT_ATMOSPHERE, CATORDER_ATMOSPHERE), PropertyOrder(21)]
    [EditorAttribute(typeof(TimeOfDayEditor), typeof(UITypeEditor))]
    [Description("All time-of-day properties")]
    public TimeOfDay TimeOfDay
    {
      get
      {
        if (_currentSettings._timeOfDay == null)
          TimeOfDay = new TimeOfDay(this);
        return _currentSettings._timeOfDay;
      }
      set
      {
        _currentSettings._timeOfDay = value;
        if (_currentSettings._timeOfDay != null)
        {
          _currentSettings._timeOfDay.Owner = this;
          _currentSettings._timeOfDay.SceneSky = this.SkyConfig;
        }
        if (_sunPathShape != null)
          _sunPathShape.TimeOfDay = _currentSettings._timeOfDay;
      }
    }

    TimeOfDaySunPathShape _sunPathShape;

    [Browsable(false)]
    public TimeOfDaySunPathShape SunPath
    {
      get
      {
        if (false && _sunPathShape == null)
        {
          _sunPathShape = new TimeOfDaySunPathShape("Sun path");
          _sunPathShape.TimeOfDay = TimeOfDay;
          this.AddShape(_sunPathShape, Root);
          EditorManager.ActiveView.Gizmo.SetSingleShape(_sunPathShape);
        }
        return _sunPathShape;
      }
    }

    [Browsable(true)]
    [SortedCategory(CAT_ATMOSPHERE, CATORDER_ATMOSPHERE), PropertyOrder(22)]
    [Description("Current time in the time-of-day system")]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.0f, 1.0f, 1440), PropertyLiveUpdate("CurrentTimeLiveUpdate"),
    DefaultValue(0.5f)]
    public float CurrentTime
    {
      get { return _currentSettings._currentTime; }
      set
      {
        _currentSettings._currentTime = value;
        EditorManager.RendererNodeManager.SetCurrentTime(_currentSettings._currentTime);
      }
    }

    /// <summary>
    /// Proxy property used for color live update (See PropertyLiveUpdate attribute in LightColor property)
    /// </summary>
    [Browsable(false)]
    public object CurrentTimeLiveUpdate
    {
      set
      {
        float fTime = _currentSettings._currentTime;
        if (value is float)
          fTime = (float)value;
        EditorManager.RendererNodeManager.SetCurrentTime(fTime);
        EditorManager.ActiveView.UpdateView(false);
      }
    }

    /// <summary>
    /// Fog setup
    /// </summary>
    [Browsable(false)]
    public FogSetup FogSetup
    {
      get
      {
        return _currentSettings._fogSetup;
      }
      set
      {
        if (_currentSettings._fogSetup != null)
        {
          _currentSettings._fogSetup.FogSetupChanged -= new CSharpFramework.Scene.FogSetup.FogSetupChangedHandler(OnFogSetupChanged);
        }

        _currentSettings._fogSetup = value;

        if (value != null)
        {
          _currentSettings._fogSetup.FogSetupChanged += new CSharpFramework.Scene.FogSetup.FogSetupChangedHandler(OnFogSetupChanged);
          _currentSettings._fogSetup.Update();
        }

        EditorManager.ActiveView.UpdateView(false);
      }
    }

    private void SetupFogData(DepthFogConfig legacyFogConfig)
    {
      _currentSettings._fogSetup = new FogSetup();
      _currentSettings._fogSetup.DepthMode = _currentSettings._fogSetup.HeightFogMode = FogSetup.Mode.Off;

      // populate the fog struct
      bool bFoundComponent = false;

      ShapeComponentCollection ps = Postprocessors;
      if (ps != null)
      {
        foreach (ShapeComponent comp in ps)
        {
          if (comp.CollectionType.UniqueName == "VGlobalFogPostprocess")
          {
            // Extract settings from the postprocessor

            // -----------------------------------------------------------------
            // Depth
            // -----------------------------------------------------------------
            DynamicProperty propDepthEnable = comp.GetProperty("Depth.Enable");
            DynamicProperty propDepthOverride = comp.GetProperty("Depth.OverrideSettings");
            DynamicProperty propDepthBaseColor = comp.GetProperty("Depth.BaseColor");
            DynamicProperty propDepthFogStart = comp.GetProperty("Depth.FogStart");
            DynamicProperty propDepthFogDepth = comp.GetProperty("Depth.FogDepth");

            // Flags
            bool depthEnable = (bool)propDepthEnable.Value;
            if (depthEnable)
            {
              _currentSettings._fogSetup.DepthMode = ((string)propDepthOverride.Value == "Off") ? FogSetup.Mode.On : FogSetup.Mode.TimeOfDay;
            }
            else
            {
              _currentSettings._fogSetup.DepthMode = FogSetup.Mode.Off;
            }

            // Values
            _currentSettings._fogSetup.DepthColor = ((Color4)propDepthBaseColor.Value).Color;
            _currentSettings._fogSetup.DepthStart = (float)propDepthFogStart.Value;
            _currentSettings._fogSetup.DepthRange = (float)propDepthFogDepth.Value;

            // -----------------------------------------------------------------
            // Height
            // -----------------------------------------------------------------
            DynamicProperty propHeightEnable = comp.GetProperty("Layered.Enable");
            DynamicProperty propHeightOverride = comp.GetProperty("Layered.OverrideSettings");
            DynamicProperty propHeightDensity = comp.GetProperty("Layered.Density");
            DynamicProperty propHeightAddScattering = comp.GetProperty("Layered.AddAtmosphericScattering");
            DynamicProperty propHeightBaseColor = comp.GetProperty("Layered.BaseColor");
            DynamicProperty propHeightGroundOffset = comp.GetProperty("Layered.GroundOffset");
            DynamicProperty propHeightHalfDensityHeight = comp.GetProperty("Layered.HalfDensityHeight");
            DynamicProperty propHeightFogStart = comp.GetProperty("Layered.FogStart");
            DynamicProperty propHeightFogDepth = comp.GetProperty("Layered.FogDepth");

            // Flags
            bool heightEnable = (bool)propHeightEnable.Value;
            if (heightEnable)
            {
              _currentSettings._fogSetup.HeightFogMode = ((string)propHeightOverride.Value == "Off") ? FogSetup.Mode.On : FogSetup.Mode.TimeOfDay;
            }
            else
            {
              _currentSettings._fogSetup.HeightFogMode = FogSetup.Mode.Off;
            }

            // Values
            _currentSettings._fogSetup.HeightFogDensity = (float)propHeightDensity.Value;
            _currentSettings._fogSetup.HeightFogAddScattering = (bool)propHeightAddScattering.Value;
            _currentSettings._fogSetup.HeightFogColor = ((Color4)propHeightBaseColor.Value).Color;
            _currentSettings._fogSetup.HeightFogOffset = (float)propHeightGroundOffset.Value;

            if (propHeightHalfDensityHeight != null)
            {
              _currentSettings._fogSetup.HeightFogHalfDensityHeight = (float)propHeightHalfDensityHeight.Value;
            }
            else
            {
              DynamicProperty propHeightFalloff = comp.GetProperty("Layered.HeightFalloff");
              if (propHeightFalloff == null)
              {
                _currentSettings._fogSetup.HeightFogHalfDensityHeight = 1000;
              }
              else
              {
                _currentSettings._fogSetup.HeightFogHalfDensityHeight = (float)(System.Math.Log(2) / (float)propHeightFalloff.Value);
              }
            }
            _currentSettings._fogSetup.HeightFogStart = (float)propHeightFogStart.Value;
            _currentSettings._fogSetup.HeightFogRange = (float)propHeightFogDepth.Value;

            bFoundComponent = true;
            break;
          }
        }
      }

      if (!bFoundComponent)
      {
        _currentSettings._fogSetup.DepthMode = (legacyFogConfig.Enabled ? (FinalEnableTimeOfDay ? FogSetup.Mode.TimeOfDay : FogSetup.Mode.On) : FogSetup.Mode.Off);
        _currentSettings._fogSetup.DepthFogColor = legacyFogConfig.FogColor;
        _currentSettings._fogSetup.DepthStart = legacyFogConfig.StartDistance;
        _currentSettings._fogSetup.DepthRange = legacyFogConfig.EndDistance - _currentSettings._fogSetup.DepthStart;
      }
    }

    void OnFogSetupChanged(FogSetup sender)
    {
      EditorManager.ActiveView.UpdateView(false);
    }

    #endregion


    #region Overridden Properties

    public override IScene ParentScene
    {
      set
      {
        if (base.ParentScene != null)
        {
          EditorManager.ProfileManager.ActiveProfileChanged -= new IProfileManager.ActiveProfileChangedEventHandler(IProfileManager_ActiveProfileChanged);
          EditorManager.ProfileManager.GetCurrentSettings -= new IProfileManager.GetCurrentSettingsEventHandler(IProfileManager_GetCurrentSettings);
          IScene.PropertyChanged -= new CSharpFramework.PropertyChangedEventHandler(IScene_PropertyChanged);
        }

        base.ParentScene = value;

        if (base.ParentScene != null)
        {
          EditorManager.ProfileManager.ActiveProfileChanged += new IProfileManager.ActiveProfileChangedEventHandler(IProfileManager_ActiveProfileChanged);
          EditorManager.ProfileManager.GetCurrentSettings += new IProfileManager.GetCurrentSettingsEventHandler(IProfileManager_GetCurrentSettings);
          IScene.PropertyChanged += new CSharpFramework.PropertyChangedEventHandler(IScene_PropertyChanged);
        }
      }
    }

    #endregion

    #region Layer Members

    /// <summary>
    /// a v3d layer is not removable
    /// </summary>
    [BrowsableAttribute(false)]
    public override bool Removable { get { return false; } }

    #endregion

    #region ISerializable

    private void LoadCustomObjectsFromFile(FileInfo fileInfo)
    {
      string pluginName = fileInfo.Name.Replace(".PluginData", "");
      if (EditorManager.GetPluginByName(pluginName) == null)
        return;

      BinaryFormatter fmt = SerializationHelper.BINARY_FORMATTER;
      FileStream fs = fileInfo.OpenRead();
      ArrayList objList = (ArrayList)fmt.Deserialize(fs);
      fs.Close();

      ArrayList targetList = _customSceneObjects.GetObjects(pluginName, true);
      targetList.AddRange(objList);
    }

    private void LoadCustomObjectsFromFiles(string pathName)
    {
      try
      {
        DirectoryInfo dirInfo = new DirectoryInfo(pathName);
        foreach (FileInfo fileInfo in dirInfo.GetFiles("*.PluginData", SearchOption.TopDirectoryOnly))
        {
          LoadCustomObjectsFromFile(fileInfo);
        }
      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex);
        EditorManager.ShowMessageBox("Error ", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
      }
    }

    private bool SaveCustomObjectsToFile(string fileName, ArrayList customObjects)
    {
      ManagedBase.RCS.GetProvider().EditFile(fileName);
      BinaryFormatter fmt = SerializationHelper.BINARY_FORMATTER;

      try
      {
        FileStream fs = File.Create(fileName);
        if (fs == null)
          return false;
        fmt.Serialize(fs, customObjects);
        fs.Close();

        ManagedBase.RCS.GetProvider().AddFile(fileName, true /* Binary file */);
      }
      catch (System.UnauthorizedAccessException)
      {
        EditorManager.ShowMessageBox("Unable to write to the plug-in settings file: " + fileName + ". Please make sure the file is writable (and not locked by source control).", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        return false;
      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex);
        EditorManager.ShowMessageBox("Error writing to the plug-in settings file: " + fileName, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        return false;
      }

      return true;
    }


    private CustomSceneObjectCollection _customSceneObjects = null;

    public override bool SaveToFile()
    {
      if (!base.SaveToFile())
      {
        return false;
      }

      // No need to continue if no scene (but, can this ever happen?)
      if (EditorManager.Scene == null)
      {
        return true;
      }

      if (_currentSettings._skyConfig != null && EditorManager.Scene != null)
      {
        _currentSettings._skyConfig.SaveSky(EditorManager.Scene.AbsoluteFileName + ".Layers\\");
      }

      // Collect any custom scene objects from plug-ins...
      _customSceneObjects = new CustomSceneObjectCollection(true, this);
      CustomSceneSerializationArgs customObjArg = new CustomSceneSerializationArgs(_customSceneObjects);
      EditorManager.OnCustomSceneSerialization(customObjArg); // collect the objects

      // ...and save them to separate files (one for each plug-in)
      bool bResult = true;
      foreach (KeyValuePair<string, ArrayList> pair in _customSceneObjects)
      {
        if (String.IsNullOrEmpty(pair.Key) || pair.Value == null || pair.Value.Count < 1)
        {
          continue;
        }

        string customObjectFileName = EditorManager.Scene.AbsoluteFileName + ".Layers\\" + pair.Key + ".PluginData";
        if (!SaveCustomObjectsToFile(customObjectFileName, pair.Value))
        {
          bResult = false;
          break;
        }
      }
      _customSceneObjects = null;

      return bResult;
    }

    // old 7.6.X version
    Renderer _renderer = null;

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected V3DLayer(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      _v3dFilename = info.GetString("_v3dFilename"); // deprecated

      _currentSettings = new ProfileSpecificSettings();

      // Determine the scene to use as reference for loading. If the layer is loaded during project load,
      // it is the scene saved temporarily as currently loading scene. Otherwise (e.g., when a layer is
      // reloaded after a change), this will likely be the scene active in the editor. Note that we have
      // no guarantee that currentScene will never be null - so always check before using it.
      IScene currentScene = EditorManager.Project.CurrentlyLoadingScene;
      if (currentScene == null)
      {
        currentScene = EditorManager.Project.Scene;
      }

      // We use _skyConfig to determine whether we have an old file that stores
      // the profile specific data in the main layer or a new one that stores it externally.
      if (SerializationHelper.HasElement(info, "_skyConfig"))
      {
        _currentSettings._skyConfig = (SkyConfig)info.GetValue("_skyConfig", typeof(SkyConfig));
        if (SerializationHelper.HasElement(info, "_defaultLightColor"))
          _currentSettings._defaultLightColor = (Color)info.GetValue("_defaultLightColor", typeof(Color));
        if (SerializationHelper.HasElement(info, "_fNearClip"))
          _currentSettings._fNearClip = info.GetSingle("_fNearClip");
        if (SerializationHelper.HasElement(info, "_fFarClip"))
          _currentSettings._fFarClip = info.GetSingle("_fFarClip");

        // view settings:
        if (SerializationHelper.HasElement(info, "_fovX"))
          _currentSettings._fFovX = info.GetSingle("_fovX");

        // old version
        if (SerializationHelper.HasElement(info, "_renderer"))
          _renderer = (Renderer)info.GetValue("_renderer", typeof(Renderer));

        // new version:
        if (SerializationHelper.HasElement(info, "_rendererNodeClass"))
        {
          _currentSettings._rendererSetup._rendererNodeClass = info.GetString("_rendererNodeClass");
          _currentSettings._rendererSetup._rendererProperties = (DynamicPropertyCollection)info.GetValue("_rendererProperties", typeof(DynamicPropertyCollection));
          _currentSettings._rendererSetup._rendererComponents = (ShapeComponentCollection)info.GetValue("_rendererComponents", typeof(ShapeComponentCollection));
        }

        if (SerializationHelper.HasElement(info, "_fogSetup"))
        {
          FogSetup = (FogSetup)info.GetValue("_fogSetup", typeof(FogSetup));
        }
        else if (SerializationHelper.HasElement(info, "_fogConfig"))
        {
          DepthFogConfig fogConfig = (DepthFogConfig)info.GetValue("_fogConfig", typeof(DepthFogConfig));
          SetupFogData(fogConfig);
        }

        if (SerializationHelper.HasElement(info, "_timeOfDay"))
        {
          _currentSettings._timeOfDay = (TimeOfDay)info.GetValue("_timeOfDay", typeof(TimeOfDay));
          if (_currentSettings._timeOfDay != null)
          {
            _currentSettings._timeOfDay.Owner = this;
            _currentSettings._timeOfDay.SceneSky = _currentSettings._skyConfig;
          }
        }

        if (SerializationHelper.HasElement(info, "_currentTime"))
        {
          _currentSettings._currentTime = info.GetSingle("_currentTime");
        }

        if (SerializationHelper.HasElement(info, "_bWantsTimeOfDay"))
        {
          _currentSettings._bWantsTimeOfDay = info.GetBoolean("_bWantsTimeOfDay");
        }

        if (SerializationHelper.HasElement(info, "_bAssumeSRGB"))
        {
          _currentSettings._eSRGBMode = info.GetBoolean("_bAssumeSRGB") ? SRGBMode_e.AssumeForDiffuse : SRGBMode_e.Disable;
          EditorManager.EngineManager.SetSRGBMode(_currentSettings._eSRGBMode);
        }

        // The settings from the main layer are migrated to the profiles in OnActivateScene
      }
      else //New version that stores profile settings externally
      {
        if (currentScene != null)
        {
          _currentSettings._skyConfig = new VisionSky();
          EditorManager.ProfileManager.LoadProfileSpecificSettings(currentScene.AbsoluteFileName + ".Layers\\", _currentSettings, false);
          _currentSettings._skyConfig.Dispose();
        }
        _currentSettings = EditorManager.ProfileManager.GetActiveProfileSettings();
      }

      // not profile specific:
      if (SerializationHelper.HasElement(info, "_sceneScriptFile"))
        _sceneScriptFile = info.GetString("_sceneScriptFile");

      if (SerializationHelper.HasElement(info, "_scriptThinkInterval"))
        _scriptThinkInterval = info.GetSingle("_scriptThinkInterval");

      if (SerializationHelper.HasElement(info, "_orthographicViewBox"))
        _orthographicViewBox = (OrthographicViewBoxShape)info.GetValue("_orthographicViewBox", typeof(OrthographicViewBoxShape));

      if (SerializationHelper.HasElement(info, "_customSceneObjects"))
      {
        try
        {
          ArrayList objList = (ArrayList)info.GetValue("_customSceneObjects", typeof(ArrayList));
          _customSceneObjects = new CustomSceneObjectCollection(objList, this);
        }
        catch (Exception e)
        {
          _customSceneObjects = null;
          EditorManager.DumpException(e);
          string msg = "Failed to load custom plugin objects in the scene file.\nPossible cause might be that a plugin is missing or is not compatible.\nThe scene has been loaded, but some information might be missing.\n\nDetailed information:\n" + e.ToString();
          EditorManager.ShowMessageBox(msg, "Serialization exception", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
      }
      else
      {
        _customSceneObjects = new CustomSceneObjectCollection(false, this);
        if (currentScene != null)
        {
          LoadCustomObjectsFromFiles(currentScene.AbsoluteFileName + ".Layers");
        }
      }

      if (SerializationHelper.HasElement(info, "SceneReferenceLocation"))
        SceneReferenceLocation = (Vector3D)info.GetValue("SceneReferenceLocation", typeof(Vector3D));
      else
        SceneReferenceLocation = Vector3D.Zero;

      if (SerializationHelper.HasElement(info, "_fWorldGeometryThreshold"))
        _fWorldGeometryThreshold = info.GetSingle("_fWorldGeometryThreshold");
      if (SerializationHelper.HasElement(info, "_fEntityThreshold"))
        _fEntityThreshold = info.GetSingle("_fEntityThreshold");
    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);
      info.AddValue("_v3dFilename", _v3dFilename);
      info.AddValue("_orthographicViewBox", _orthographicViewBox);
      info.AddValue("_sceneScriptFile", _sceneScriptFile);
      info.AddValue("_scriptThinkInterval", _scriptThinkInterval);
      info.AddValue("SceneReferenceLocation", SceneReferenceLocation);
      info.AddValue("_fWorldGeometryThreshold", _fWorldGeometryThreshold);
      info.AddValue("_fEntityThreshold", _fEntityThreshold);
    }


    List<IAction> _pendingActions = null;
    void AddPendingAction(IAction action)
    {
      if (_pendingActions == null)
        _pendingActions = new List<IAction>();
      _pendingActions.Add(action);
    }

    /// <summary>
    /// Overridden version
    /// </summary>
    public override void OnDeserialization()
    {
      base.OnDeserialization();
      if (_renderer != null)
        _renderer.Owner = this;

      if (_customSceneObjects != null)
      {
        CustomSceneSerializationArgs customObjArg = new CustomSceneSerializationArgs(_customSceneObjects);
        EditorManager.OnCustomSceneSerialization(customObjArg); // notify the tools
        _customSceneObjects = null;
      }

      try
      {
        MigrateRendererProperties(_renderer); // this can be removed later

        // new renderer type properties
        if (RendererNodeClass == IRendererNodeManager.RENDERERNODECLASS_NONE)
        {
          RendererNodeClass = IRendererNodeManager.RENDERERNODECLASS_SIMPLE;
        }

        {
          DynamicPropertyCollection oldProperties = _currentSettings._rendererSetup._rendererProperties;
          DynamicPropertyCollectionType propertiesType = EditorManager.RendererNodeManager.NodeClassTypeManager.GetCollectionType(RendererNodeClass);
          bool bAddSun = false;
          if (propertiesType != null)
          {
            if (_currentSettings._rendererSetup._rendererProperties == null)
              _currentSettings._rendererSetup._rendererProperties = propertiesType.CreateInstance(this);
            else
              _currentSettings._rendererSetup._rendererProperties = propertiesType.CreateMigratedCollection(oldProperties);
          }
          else
            _currentSettings._rendererSetup._rendererProperties = EditorManager.RendererNodeManager.NodeClassTypeManager.CreateMigratedCollection(oldProperties);
          if (_currentSettings._rendererSetup._rendererProperties != null)
            _currentSettings._rendererSetup._rendererProperties.Owner = this;

          // Vision 8.1: Migrate old time of day properties to a light source that is added to the scene
          if (RendererNodeClass == IRendererNodeManager.RENDERERNODECLASS_DEFERRED && oldProperties != null)
          {
            // migrate old depth fog
            MigrateFogProperties();

            object sun = oldProperties.GetPropertyValue("SunLight", false);
            if (sun is bool) // seems an 8.0 scene
            {
              bAddSun = (bool)sun;
              if (bAddSun)
              {
                // assume that the other properties are there as well:
                DynLightShape lightShape = new DynLightShape("Sun Light");
                lightShape.MakeTimeOfDayLight(
                  (float)oldProperties.GetPropertyValue("SunIntensity", false),
                  (bool)oldProperties.GetPropertyValue("Corona", false),
                  (string)oldProperties.GetPropertyValue("CoronaFilename", false)
                  );
                // add shadowmaps?
                object sunShadow = oldProperties.GetPropertyValue("SunShadowsEnabled", false);
                if ((sunShadow is bool) && ((bool)sunShadow) == true)
                {
                  ShapeComponentType t = (ShapeComponentType)EditorManager.EngineManager.ComponentClassManager.GetCollectionType("VShadowMapComponentSpotDirectional");
                  System.Diagnostics.Debug.Assert(t != null, "Cannot create component of type VShadowMapComponentSpotDirectional");
                  if (t != null)
                  {
                    ShapeComponent comp = (ShapeComponent)t.CreateInstance(this);
                    float fRange = (float)oldProperties.GetPropertyValue("ShadowRange", false);
                    object oldCascades = oldProperties.GetPropertyValue("ShadowCascades", false); // there might be old maps without this property
                    int iCascades = (oldCascades == null) ? 1 : (int)oldCascades;
                    float fOffset = (float)oldProperties.GetPropertyValue("ShadowOffset", false);
                    comp.SetPropertyValue("CascadeCount", iCascades);
                    if (iCascades == 1) fRange *= 2.0f;
                    comp.SetPropertyValue("CascadeRange[0]", fRange * 0.5f);
                    comp.SetPropertyValue("CascadeRange[1]", fRange);
                    object oldInterval = oldProperties.GetPropertyValue("ShadowCenterUpdateDistance", false);
                    if (oldInterval is float)
                      comp.SetPropertyValue("CameraUpdateInterval", (float)oldInterval, false);
                    comp.SetPropertyValue("ShadowMapSize", (int)oldProperties.GetPropertyValue("ShadowTextureSize", false), false);
                    comp.SetPropertyValue("Bias[0]", fOffset, false);
                    comp.SetPropertyValue("Bias[1]", fOffset, false);
                    object oldMask = oldProperties.GetPropertyValue("ShadowContextFilterBitmask", false);
                    if (oldMask is int)
                      comp.SetPropertyValue("FilterBitmask", (int)oldMask, false);
                    // more properties from lighting pp component?

                    lightShape.AddComponentInternal(comp);
                  }
                }

                AddPendingAction(AddShapeAction.CreateAddShapeAction(lightShape, this.Root, this, true));
              }
            }
            object backLight = oldProperties.GetPropertyValue("BackLight", false);
            if ((backLight is bool) && ((bool)backLight) == true)
            {
              DynLightShape backLightShape = new DynLightShape("Back Light");
              AddPendingAction(AddShapeAction.CreateAddShapeAction(backLightShape, this.Root, this, true));
            }

          }
        }

      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex, true);
      }

      if (_currentSettings._rendererSetup._rendererComponents != null)
        _currentSettings._rendererSetup._rendererComponents.MigrateProperties(this, EditorManager.EngineManager.ComponentClassManager);


    }

    #region Old Renderer properties migration

    private void MigrateFogProperties()
    {
      foreach (ShapeComponent comp in _currentSettings._rendererSetup._rendererComponents)
      {
        if (comp.DisplayName == "VDeferredShadingDepthFog")
        {
          ShapeComponentType fogType = (ShapeComponentType)EditorManager.EngineManager.ComponentClassManager.GetCollectionType("VGlobalFogPostprocess");
          System.Diagnostics.Debug.Assert(fogType != null, "Cannot create component of type VGlobalFogPostprocess");
          if (fogType != null)
          {
            ShapeComponent newComp = null;

            foreach (ShapeComponent fogComp in _currentSettings._rendererSetup._rendererComponents)
            {
              if (fogComp.DisplayName == "VGlobalFogPostprocess")
              {
                newComp = fogComp;
                break;
              }
            }

            if (newComp == null)
            {
              newComp = (ShapeComponent)fogType.CreateInstance(this);
              _currentSettings._rendererSetup._rendererComponents.Add(newComp);
            }

            newComp.Active = comp.Active;
            _currentSettings._rendererSetup._rendererComponents.Remove(comp);
          }

          if (comp.Active)
          {
            EditorManager.ShowMessageBox(
                      "The VDeferredShadingDepthFog postprocessor has been deprecated, and will be replaced by VGlobalFogPostprocess.",
                      "Automatic conversion of fog postprocessor", MessageBoxButtons.OK, MessageBoxIcon.Information);
          }
          break;
        }
      }
    }

    string SampleCount2Enum(Renderer.DeferredRendererProperties.SSAOProperties.Samples_e samples)
    {
      switch (samples)
      {
        case Renderer.DeferredRendererProperties.SSAOProperties.Samples_e._8_: return "8";
        case Renderer.DeferredRendererProperties.SSAOProperties.Samples_e._16_: return "16";
        case Renderer.DeferredRendererProperties.SSAOProperties.Samples_e._32_: return "32";
        case Renderer.DeferredRendererProperties.SSAOProperties.Samples_e._64_: return "64";
      }

      return "32";
    }

    string FilterTechnique2Enum(Renderer.DeferredRendererProperties.SSAOProperties.Filter_e filter)
    {
      switch (filter)
      {
        case Renderer.DeferredRendererProperties.SSAOProperties.Filter_e.Gaussian: return "GAUSSIAN";
        case Renderer.DeferredRendererProperties.SSAOProperties.Filter_e.Bilateral5Samples: return "BILATERAL_5_SAMPLES";
        case Renderer.DeferredRendererProperties.SSAOProperties.Filter_e.Bilateral9Samples: return "BILATERAL_9_SAMPLES";
        case Renderer.DeferredRendererProperties.SSAOProperties.Filter_e.Adaptive: return "ADAPTIVE";
      }
      return "ADAPTIVE";
    }

    string ToneMapType2Enum(Renderer.DeferredRendererProperties.ToneMapperProperties.ToneMapperMode_e mode)
    {
      switch (mode)
      {
        case Renderer.DeferredRendererProperties.ToneMapperProperties.ToneMapperMode_e.None: return "OFF";
        case Renderer.DeferredRendererProperties.ToneMapperProperties.ToneMapperMode_e.Scale: return "SCALE";
        case Renderer.DeferredRendererProperties.ToneMapperProperties.ToneMapperMode_e.ScaleMap: return "SCALE_MAP";
      }
      return "OFF";
    }

    string ShadowQuality2Enum(Renderer.DeferredRendererProperties.TimeOfDayProperties.ShadowQuality_e quality)
    {
      switch (quality)
      {
        case Renderer.DeferredRendererProperties.TimeOfDayProperties.ShadowQuality_e.PCF4: return "PCF4";
        case Renderer.DeferredRendererProperties.TimeOfDayProperties.ShadowQuality_e.PCF8: return "PCF8";
        case Renderer.DeferredRendererProperties.TimeOfDayProperties.ShadowQuality_e.PCF16: return "PCF16";
        case Renderer.DeferredRendererProperties.TimeOfDayProperties.ShadowQuality_e.PCSS16: return "PCSS16";
      }
      return "PCF4";
    }

    /// <summary>
    /// From old to new system. Assumes there is no definition of new style
    /// </summary>
    /// <param name="renderer"></param>
    void MigrateRendererProperties(Renderer renderer)
    {
      if (renderer == null)
        return;

      // forward renderer
      if (renderer.RendererType == Renderer.RendererType_e.ForwardRenderer)
      {
        // class name string
        this._currentSettings._rendererSetup._rendererNodeClass = IRendererNodeManager.RENDERERNODECLASS_SIMPLE;

        /*
        DynamicPropertyCollectionType propertiesType = EditorManager.RendererNodeManager.NodeClassTypeManager.GetCollectionType(_rendererSetup._rendererNodeClass);
        if (propertiesType != null)
          _rendererSetup._rendererProperties = propertiesType.CreateInstance(this);
        */
        return; // no more properties to migrate
      }

      // deferred renderer
      if (renderer.RendererType == Renderer.RendererType_e.DeferredRenderer)
      {
        // class name string
        this._currentSettings._rendererSetup._rendererNodeClass = IRendererNodeManager.RENDERERNODECLASS_DEFERRED;
        DynamicPropertyCollectionType propertiesType = EditorManager.RendererNodeManager.NodeClassTypeManager.GetCollectionType(_currentSettings._rendererSetup._rendererNodeClass);
        if (propertiesType == null)
          return;
        _currentSettings._rendererSetup._rendererProperties = propertiesType.CreateInstance(this);
        Renderer.DeferredRendererProperties oldProp = (Renderer.DeferredRendererProperties)renderer.RendererProperties;

        // migrate root properties
        _currentSettings._rendererSetup._rendererProperties.SetPropertyValue("UseHDR", oldProp.HDR, false);
        _currentSettings._rendererSetup._rendererProperties.SetPropertyValue("UseLightmaps", oldProp.StaticLighting, false);
        _currentSettings._rendererSetup._rendererProperties.SetPropertyValue("SunLight", oldProp.TimeOfDay.Sunlight, false);
        _currentSettings._rendererSetup._rendererProperties.SetPropertyValue("SunIntensity", oldProp.TimeOfDay.SunlightIntensity, false);
        _currentSettings._rendererSetup._rendererProperties.SetPropertyValue("SunShadowsEnabled", oldProp.TimeOfDay.SunlightShadows, false);
        _currentSettings._rendererSetup._rendererProperties.SetPropertyValue("Corona", !string.IsNullOrEmpty(oldProp.TimeOfDay.SunCoronaTexture), false);
        _currentSettings._rendererSetup._rendererProperties.SetPropertyValue("CoronaFilename", oldProp.TimeOfDay.SunCoronaTexture, false);
        _currentSettings._rendererSetup._rendererProperties.SetPropertyValue("ShadowRange", oldProp.TimeOfDay.ShadowTextureRange, false);
        _currentSettings._rendererSetup._rendererProperties.SetPropertyValue("ShadowTextureSize", oldProp.TimeOfDay.ShadowTextureSize, false);
        _currentSettings._rendererSetup._rendererProperties.SetPropertyValue("ShadowOffset", oldProp.TimeOfDay.ShadowOffset, false);
        _currentSettings._rendererSetup._rendererProperties.SetPropertyValue("BackLight", oldProp.TimeOfDay.Backlight, false);

        // migrate SSAO properties
        ShapeComponent ssao = CreatePostprocessor("VPostProcessSSAO");
        if (ssao != null)
        {
          Renderer.DeferredRendererProperties.SSAOProperties old = oldProp.SSAO;
          ssao.Active = old.Enable;
          ssao.SetPropertyValue("Samples", SampleCount2Enum(old.Samples), false);
          ssao.SetPropertyValue("FilterTechnique", FilterTechnique2Enum(old.Filter), false);
          ssao.SetPropertyValue("Range", old.Range, false);
          ssao.SetPropertyValue("RangeFactor", old.RangeFactor, false);
          ssao.SetPropertyValue("Offset", old.Offset, false);
          ssao.SetPropertyValue("NumBlurPasses", old.BlurPasses, false);
        }

        // migrate rim light properties
        ShapeComponent rim = CreatePostprocessor("VDeferredShadingRimLight");
        if (rim != null)
        {
          Renderer.DeferredRendererProperties.RimLightProperties old = oldProp.RimLight;
          rim.Active = old.Enable;
          // need Color4 rather than Color4F otherwise dynamic property sytem gest out of sync
          Color4 col = new Color4(old.RimLightColor.R255, old.RimLightColor.G255, old.RimLightColor.B255, old.RimLightColor.A255);
          rim.SetPropertyValue("RimLightColor", col, false);
          rim.SetPropertyValue("RimLightExp", old.RimLightExponent, false);
          rim.SetPropertyValue("RimLightScale", old.RimLightScale, false);
        }

        // migrate dof properties
        ShapeComponent dof = CreatePostprocessor("VPostProcessDepthOfField");
        if (dof != null)
        {
          Renderer.DeferredRendererProperties.DepthofFieldProperties old = oldProp.DepthOfField;
          dof.Active = old.Enable;
          dof.SetPropertyValue("QuarterSize", old.QuarterSize, false);
          dof.SetPropertyValue("FocusPoint", old.FocusPoint, false);
          dof.SetPropertyValue("FocusDepth", old.FocusDepth, false);
          dof.SetPropertyValue("BlurRadius", old.BlurRadius, false);
        }

        // migrate anti alias properties
        ShapeComponent aa = CreatePostprocessor("VDeferredShadingAntiAlias");
        if (aa != null)
        {
          Renderer.DeferredRendererProperties.AntiAliasingProperties old = oldProp.AntiAliasing;
          aa.Active = old.Enable;
          aa.SetPropertyValue("Strength", old.Strength, false);
          aa.SetPropertyValue("Threshold[0]", old.NormalThreshold, false);
          aa.SetPropertyValue("Threshold[1]", old.NormalThreshold, false);
          aa.SetPropertyValue("Weights[0]", old.NormalWeight, false);
          aa.SetPropertyValue("Weights[1]", old.NormalWeight, false);
        }

        // migrate glow properties
        ShapeComponent glow = CreatePostprocessor("VPostProcessGlow");
        if (glow != null)
        {
          Renderer.DeferredRendererProperties.GlowProperties old = oldProp.Glow;
          glow.Active = old.Enable;
          glow.SetPropertyValue("DownscaleMode", old.Downscale == Renderer.DeferredRendererProperties.GlowProperties.Downscale_e._2x_ ? "DOWNSCALE_2X" : "DOWNSCALE_4X", false);
          glow.SetPropertyValue("Bias", old.Bias, false);
          glow.SetPropertyValue("Exponent", old.Exponent, false);
          glow.SetPropertyValue("Scale", old.Scale, false);
          glow.SetPropertyValue("BlurPasses", old.BlurPasses, false);
        }

        // migrate tone mapping properties
        ShapeComponent tone = CreatePostprocessor("VPostProcessToneMapping");
        if (tone != null)
        {
          Renderer.DeferredRendererProperties.ToneMapperProperties old = oldProp.ToneMapper;
          tone.Active = old.ToneMapperMode != Renderer.DeferredRendererProperties.ToneMapperProperties.ToneMapperMode_e.None;
          tone.SetPropertyValue("ToneMapType", ToneMapType2Enum(old.ToneMapperMode), false);
          tone.SetPropertyValue("Saturation", old.Saturation, false);
          tone.SetPropertyValue("Contrast", old.Contrast, false);
          tone.SetPropertyValue("Brightness", old.Brightness, false);
          tone.SetPropertyValue("MotionBlurFeedback", old.MotionBlurFeedback, false);
        }

        // migrate lighting properties (always there)
        ShapeComponent light = CreatePostprocessor("VDeferredShadingLights");
        if (light != null)
        {
          light.SetPropertyValue("ShadowQuality", ShadowQuality2Enum(oldProp.TimeOfDay.ShadowQuality), false);
          light.SetPropertyValue("IgnoreStaticSceneLights", oldProp.StaticLighting, false); // uses StaticLighting again
          light.SetPropertyValue("SampleRadius[0]", oldProp.TimeOfDay.ShadowSampleRadius, false);
          light.SetPropertyValue("SampleRadius[1]", oldProp.TimeOfDay.ShadowSampleRadius * 0.33333f, false);
          //light.SetPropertyValue("SunSize", 2.0f, false); //  no corresponding item
        }


        // fog is activated by default
        ShapeComponent fog = CreatePostprocessor("VDeferredShadingDepthFog");

        // migrate (remaining) time-of-day properties
        TimeOfDay newT = TimeOfDay;
        Renderer.DeferredRendererProperties.TimeOfDayProperties oldT = oldProp.TimeOfDay;
        newT.AmbientColor = oldT.AmbientColor;
        newT.DawnSkyFade = oldT.DawnSkyFade;
        newT.DuskSkyFade = oldT.DuskSkyFade;
        newT.DepthFog.Color = oldT.FogColor;
        newT.DepthFog.End = oldT.FogEnd;
        newT.DepthFog.Range = oldT.FogRange * 8.0f; // magic multiplier used in old DeferredRenderingSystem_cl::UpdateSun(). However, fog start used 3.0 but we ignore this here
        newT.DepthFog.Start = oldT.FogStart;
        newT.NightSkyFade = oldT.NightSkyFade;
        newT.SunColor = oldT.SunColor;

        _currentSettings._currentTime = oldT.CurrentTime;
      }
    }

    public class GetPostprossingShapesVisitor : IShapeVisitor
    {
      public override IShapeVisitor.VisitResult Visit(ShapeBase shape)
      {
        PostProcessingShape pshape = shape as PostProcessingShape;
        if (shape.Modifiable && pshape != null)
        {
          if (FirstActive == null || pshape.Active)
            FirstActive = pshape;
          Found.Add(pshape);
        }

        return VisitResult.VisitOk;
      }

      public PostProcessingShape FirstActive = null;
      public ShapeCollection Found = new ShapeCollection();
    }


    public class CheckRendererNodeRequirementVisitor : IShapeVisitor
    {
      public override IShapeVisitor.VisitResult Visit(ShapeBase shape)
      {
#if !HK_ANARCHY
        if (shape is WaterShape || shape is SunglareShape)
        {
          NeedsRendererNodeClass = true;
          return VisitResult.VisitStop;
        }
#endif
        return VisitResult.VisitOk;
      }
      public bool NeedsRendererNodeClass = false;
    }

    public string NewToneMappingType(string old)
    {
      if (string.Compare(old, "none", true) == 0)
        return "OFF";
      if (string.Compare(old, "scale_map", true) == 0)
        return "SCALE_MAP";
      return "SCALE";
    }

    private void MigratePostprocessingShapes()
    {
      GetPostprossingShapesVisitor vis = new GetPostprossingShapesVisitor();
      EditorManager.Scene.RunShapeVisitor(vis);
      if (vis.Found.Count == 0)
        return;

      GroupAction action = new GroupAction("Remove postprocessing Shape(s)");
      foreach (ShapeBase shape in vis.Found)
      {
        action.Add(RemoveShapeAction.CreateRemoveShapeAction(shape));
      }

      // migrate the VPostProcessingHDRBloom effect
      if (vis.FirstActive != null && vis.FirstActive.EffectClass == "VPostProcessingHDRBloom")
      {
        if (RendererNodeClass == EditorManager.RendererNodeManager.DefaultRendererNodeClass)
          RendererNodeClass = IRendererNodeManager.RENDERERNODECLASS_FORWARD;

        DynamicPropertyCollection oldProp = vis.FirstActive.EffectProperties;
        int iGlowPasses = int.Parse(oldProp.GetPropertyValue("GlowPasses", false).ToString());
        float fGlowExponent = (float)oldProp.GetPropertyValue("GlowExponent", false);
        float fGlowScale = (float)oldProp.GetPropertyValue("GlowScale", false);
        float fGlowBias = (float)oldProp.GetPropertyValue("GlowBias", false);
        float fGlowOffset = (float)oldProp.GetPropertyValue("GlowOffset", false);
        string toneTechnique = oldProp.GetPropertyValue("ToneMappingTechnique", false).ToString();
        float fToneSaturation = (float)oldProp.GetPropertyValue("ToneSaturation", false);
        float fToneContrast = (float)oldProp.GetPropertyValue("ToneContrast", false);
        float fToneBrightness = (float)oldProp.GetPropertyValue("ToneBrightness", false);
        float fMotionBlurFeedback = (float)oldProp.GetPropertyValue("MotionBlurFeedback", false);

        ShapeComponent bloom = CreatePostprocessor("VPostProcessGlow");
        if (bloom != null)
        {
          //bloom.SetPropertyValue("DownscaleMode",  , false);
          bloom.SetPropertyValue("Bias", fGlowBias, false);
          bloom.SetPropertyValue("Exponent", fGlowExponent, false);
          bloom.SetPropertyValue("Scale", fGlowScale, false);
          bloom.SetPropertyValue("BlurPasses", iGlowPasses, false);
        }
        ShapeComponent tm = CreatePostprocessor("VPostProcessToneMapping");
        if (tm != null)
        {
          tm.SetPropertyValue("ToneMapType", NewToneMappingType(toneTechnique), false);
          tm.SetPropertyValue("Saturation", fToneSaturation, false);
          tm.SetPropertyValue("Contrast", fToneContrast, false);
          tm.SetPropertyValue("Brightness", fToneBrightness, false);
          tm.SetPropertyValue("MotionBlurFeedback", fMotionBlurFeedback, false);
        }
      }

      EditorManager.Actions.Add(action);
      Modified = true;
    }


    ShapeComponent CreatePostprocessor(string nativeClass)
    {
      ShapeComponentType t = (ShapeComponentType)EditorManager.EngineManager.ComponentClassManager.GetCollectionType(nativeClass);
      if (t == null)
        return null;

      // component of this type already attached?
      ShapeComponent comp = null;
      if (_currentSettings._rendererSetup._rendererComponents != null)
      {
        comp = _currentSettings._rendererSetup._rendererComponents.GetComponentByType(t);
        if (comp != null)
          return comp;
      }
      else
        _currentSettings._rendererSetup._rendererComponents = new ShapeComponentCollection();

      comp = (ShapeComponent)t.CreateInstance(this);
      _currentSettings._rendererSetup._rendererComponents.Add(comp);
      return comp;
    }

    #endregion // Old Renderer properties migration

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
