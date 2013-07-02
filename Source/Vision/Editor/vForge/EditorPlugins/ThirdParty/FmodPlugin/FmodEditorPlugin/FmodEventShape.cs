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
using System.IO;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Design;
using System.ComponentModel;
using System.Globalization;
using CSharpFramework;
using CSharpFramework.Shapes;
using CSharpFramework.Actions;
using FmodManaged;
using System.Runtime.Serialization;
using ManagedFramework;
using CSharpFramework.PropertyEditors;
using CSharpFramework.View;
using CSharpFramework.Serialization;
using CSharpFramework.Math;
using CSharpFramework.Scene;
using System.Windows.Forms.Design;

namespace FmodEditorPlugin
{

  #region class FmodEventShape
  /// <summary>
  /// FmodEventShape : This is the class that represents the event shape in the editor. It has an engine instance that handles the
  /// native code. The engine instance code is located in the FmodManaged project (managed C++ class library)
  /// </summary>
  [Serializable]
  public class FmodEventShape : ShapeObject3D
  {
    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_EVENTRES = "Event Resource";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_EVENTRES = Shape3D.LAST_CATEGORY_ORDER_ID + 1;

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_EVENT = "Event Instance";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_EVENT = Shape3D.LAST_CATEGORY_ORDER_ID + 2;

    /// <summary>
    /// last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_EVENT;

    #endregion

    #region Constructor

    /// <summary>
    /// The constructor of the Fmod Event shape, just takes the Event name
    /// </summary>
    /// <param name="name">Name of the shape in the shape tree</param>
    public FmodEventShape(string name)
      : base(name)
    {
    }

    #endregion

    #region Base Overrides

    protected override Shape3D.ParentingModes ParentingMode
    {
      get
      {
        return ParentingModes.SceneGraphScaled;
      }
    }

    public override PropertyFlags_e GetPropertyFlags(PropertyFlagsProviderInfo pd)
    {
      PropertyFlags_e flags = base.GetPropertyFlags(pd);

      if (pd.Name == "EventGroup")
      {
        // if _eventProjectPath points to a invalid event-project, make the "EventGroup" property read-only
        if (!FmodManaged.ManagedModule.GetEventGroupTree(_eventProjectPath, null))
          return PropertyFlags_e.Readonly;
      }
      else if(pd.Name == "Event")
      {
        // if _eventProjectPath points to a invalid event-project, respectively _eventGroupName points to a invalid
        // event-group, make the "Event" property read-only
        if (!FmodManaged.ManagedModule.GetEventGroupTree(_eventProjectPath, null))
          return PropertyFlags_e.Readonly;
        if (!FmodManaged.ManagedModule.GetEventTree(_eventProjectPath, _eventGroupName, null))
          return PropertyFlags_e.Readonly;
      }
      return flags;
    }

    #endregion

    #region GUI

    /// <summary>
    /// Overridden function to get the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetIconIndex()
    {
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\event_loudspeaker.bmp"), Color.Magenta);
    }

    #endregion

    #region Engine Instance

    /// <summary>
    /// Function to create the engine instance of the shape. The engine instance is of type EngineInstanceFmodEvent
    /// and located in the managed code library.
    /// </summary>
    /// <param name="bCreateChildren">relevant for the base class to create instances for children</param>
    public override void CreateEngineInstance(bool bCreateChildren)
    {
      _engineInstance = new EngineInstanceFmodEvent();
      SetEngineInstanceBaseProperties(); // sets the position etc.

      // Call the base function to setup attachments and process the children. The engine instance
      // has to exist at this point.
      base.CreateEngineInstance(bCreateChildren);
    }

    /// <summary>
    /// Helper field to access the engine instance as casted class to perform specific operations on it
    /// </summary>
    EngineInstanceFmodEvent EngineFmodEvent { get { return (EngineInstanceFmodEvent)_engineInstance; } }

    /// <summary>
    /// Overridden render function: Let the engine instance render itself
    /// </summary>
    /// <param name="view"></param>
    /// <param name="mode"></param>
    public override void RenderShape(VisionViewBase view, ShapeRenderMode mode)
    {
      base.RenderShape(view, mode);
      if (HasEngineInstance())
      {
        EngineFmodEvent.RenderShape(view, mode);

        if (_hotSpotStartEvent != null)
        {
          // render again to keep the button state updated
          if (EngineFmodEvent.IsPlaying())
            EditorManager.ActiveView.UpdateView(false);
          UpdatePlayIcon();
        }
      }
    }

    /// <summary>
    /// Set all properties on the engine instance. The base implementation already sets position and orientation,
    /// so there is nothing else to do here
    /// </summary>
    public override void SetEngineInstanceBaseProperties()
    {
      if (_engineInstance == null)
        return;
      EngineFmodEvent.InitEvent(_eventProjectPath, _eventGroupName, _eventName, EventFlags | FmodCreationFlags_e.Paused);
      base.SetEngineInstanceBaseProperties(); // now set position etc.

      // Only start the event in case we are in play mode (not in editing mode)
      if ((EventFlags & FmodCreationFlags_e.Paused) == 0)
      {
        EngineFmodEvent.Start(false);

        // The native class will not start the event if we are in editing mode, and thus
        // keep the event paused. Make sure that it is marked as playing, so that the flag
        // is exported correctly.
        EngineFmodEvent.RemoveFlag(FmodCreationFlags_e.Paused);
      }

      UpdateHotspotVisibility();
      RecreateCustomLinks();
    }

    void OnEventPropertyChanged()
    {
      RemoveNativeComponents(); // since the native object will change, also re-assign new native components
      SetEngineInstanceBaseProperties();
      ReAttachComponents();
      ReassignAllLinks();
    }

    /// <summary>
    /// Pick the shape in the view. Use helper function to trace the local bounding box.
    /// </summary>
    /// <param name="rayStart">trace ray start</param>
    /// <param name="rayEnd">trace ray end</param>
    /// <param name="result">result structure to fill in</param>
    public override void OnTraceShape(ShapeTraceMode_e mode, Vector3F rayStart, Vector3F rayEnd, ref ShapeTraceResult result)
    {
      if (mode != ShapeTraceMode_e.ShapePicking)
        return;
      if (ConversionUtils.TraceOrientedBoundingBox(LocalBoundingBox, Position, RotationMatrix, rayStart, rayEnd, ref result))
        result.hitShape = this;
    }

    #endregion

    #region Cloning

    /// <summary>
    /// This function must be overridden, since we have to reset the hotspot
    /// </summary>
    /// <returns></returns>
    public override ShapeBase Clone()
    {
      FmodEventShape copy = (FmodEventShape)base.Clone();
      System.Diagnostics.Debug.Assert(!copy.Selected); // turned off in ShapeBase

      copy._hotSpotStartEvent = null;

      return copy;
    }

    #endregion

    #region Event properties

    string _eventProjectPath = "";
    string _eventGroupName = "";
    string _eventName = "";
    bool _bStartPaused = false;
    bool _bDisposeWhenFinished = true;

    [Browsable(false)]
    protected FmodCreationFlags_e EventFlags
    {
      get
      {
        FmodCreationFlags_e flags = FmodCreationFlags_e.None;
        if (_bStartPaused) flags |= FmodCreationFlags_e.Paused;
        if (!_bDisposeWhenFinished) flags |= FmodCreationFlags_e.NoDispose;
        return flags;
      }
    }

    /// <summary>
    /// Gets or sets the path of the event project
    /// </summary>
    [PrefabResolveFilename]
    [SortedCategory(CAT_EVENTRES, CATORDER_EVENTRES),
    PropertyOrder(1),
    EditorAttribute(typeof(FilenameEditor), typeof(UITypeEditor)),
    FileDialogFilter(new string[] { ".fdp" })]
    [Description("The working event project file, which had been created in the Fmod Designer tool")]
    [DependentProperties(new string[] { "EventGroup", "Event" })]
    public string EventProject
    {
      get { return _eventProjectPath; }
      set
      {
        // first remove ".fdp" extension
        string pathNoExt = Path.ChangeExtension(value, null);
        
        if (_eventProjectPath == pathNoExt) return;
        _eventProjectPath = pathNoExt;
        _eventGroupName = null;
        _eventName = null;
        OnEventPropertyChanged(); // set everything because an event instance is to be created
      }
    }

    /// <summary>
    /// Gets or sets the name of the event group
    /// </summary>
    [PrefabResolveFilename]
    [SortedCategory(CAT_EVENTRES, CATORDER_EVENTRES),
    PropertyOrder(2),
    EditorAttribute(typeof(EventGroupSelectionEditor), typeof(UITypeEditor)),
    FileDialogFilter(new string[] { })]
    [Description("The name of the event group, relative to the event project")]
    [DependentProperties(new string[] {"Event"})]
    public string EventGroup 
    {
      get { return _eventGroupName; }
      set
      {
        if (_eventGroupName == value) return;
        _eventGroupName = value;
        _eventName = null;
        OnEventPropertyChanged(); // set everything because an event instance is to be created
      }
    }

    /// <summary>
    /// Gets or sets the name of the event 
    /// </summary>
    [PrefabResolveFilename]
    [SortedCategory(CAT_EVENT, CATORDER_EVENT),
    PropertyOrder(3),
    EditorAttribute(typeof(EventSelectionEditor), typeof(UITypeEditor)),
    FileDialogFilter(new string[] { })]
    [Description("The name of the event to be played, relative to the set EventGroup")]
    public string Event
    {
      get { return _eventName; }
      set
      {
        if (_eventName == value) return;
        _eventName = value;
        OnEventPropertyChanged(); // set everything because an event instance is to be created
      }
    }

    /// <summary>
    /// If enabled, the event is paused after creation and has to be activated via program code or trigger links
    /// </summary>
    [SortedCategory(CAT_EVENT, CATORDER_EVENT),
    PropertyOrder(4),
    Description("If enabled, the event is paused after creation and has to be activated via program code or trigger links.")]
    public bool StartPaused
    {
      get { return _bStartPaused; }
      set
      {
        if (_bStartPaused == value) return;
        _bStartPaused = value;
        OnEventPropertyChanged(); // set everything because an event instance is to be created
      }
    }

    /// <summary>
    /// If enabled, the engine event object is disposed automatically when finished playing. Otherwise it can be re-used via Play/ Pause within the life cycle of a scene
    /// </summary>
    [SortedCategory(CAT_EVENT, CATORDER_EVENT),
    PropertyOrder(5),
    Description("If enabled, the engine event object is disposed automatically when finished playing. Otherwise it can be re-used via Play/ Pause within the life cycle of a scene")]
    public bool DisposeWhenFinished
    {
      get { return _bDisposeWhenFinished; }
      set
      {
        if (_bDisposeWhenFinished == value) return;
        _bDisposeWhenFinished = value;
        OnEventPropertyChanged(); // set everything because an event instance is to be created
      }
    }

    #endregion

    #region Selection and hot spots

    HotSpotPlayButton _hotSpotStartEvent; // start the event

    public override void OnAddedToScene()
    {
      base.OnAddedToScene();
      AddHint(HintFlags_e.OnlyUniformScale);
    }

    void UpdatePlayIcon()
    {
      if (!HasEngineInstance() || _hotSpotStartEvent == null)
        return;
      _hotSpotStartEvent.ShowStopIcon = EngineFmodEvent.IsPlaying();
    }

    /// <summary>
    ///  Start the event in the editor (e.g. hotspot button pressed)
    /// </summary>
    public void ToggleStartEvent()
    {
      if (!HasEngineInstance())
        return;
      if (EngineFmodEvent.IsPlaying())
      {
        EngineFmodEvent.Stop();
      }
      else
      {
        // Force the event to start, even if we are not in play mode
        EngineFmodEvent.Start(true);
      }
      UpdatePlayIcon();
    }

    void UpdateHotspotVisibility()
    {
      if (_hotSpotStartEvent != null)
        _hotSpotStartEvent.Visible = HasEngineInstance() && EngineFmodEvent.IsValid();
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    public override void OnSelected()
    {
      base.OnSelected();

      // start button
      _hotSpotStartEvent = new HotSpotPlayButton(this, @"textures\Media_play.TGA", VisionColors.RGBA(200, 200, 200, 255), VisionColors.White, 15.0f);
      _hotSpotStartEvent.ToolTipText = "Start event";
      _hotSpotStartEvent.Set2DOffset(0.0f, 30.0f);
      EditorManager.ActiveView.HotSpots.Add(_hotSpotStartEvent);

      // Update the engine instance, in order to offer stolen info-only events the possibility to convert into a playable event instance. Thus hotspot button
      // will be always available for all event instances. Please note: All stolen event instances, which have a "Max playbacks behavior" without failing
      // (1 = steal oldest, 2 = steal newest, 3 = steal quietest), will not recover. This is absolute necessary, in order to avoid an endless cycle of stealing 
      // and recovering of such event instances, as none of the requests for a not info-only event will fail. Thus for these events, the hotspot button will 
      // not be available.
      EngineFmodEvent.Update();

      UpdateHotspotVisibility();
    }

    void SafeRemoveHotSpot(HotSpotBase hotSpot)
    {
      if (hotSpot == null)
        return;
      EditorManager.ActiveView.HotSpots.Remove(hotSpot);
      hotSpot.Remove();
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    public override void OnUnSelected()
    {
      // release the hotspot again:
      SafeRemoveHotSpot(_hotSpotStartEvent); _hotSpotStartEvent = null;

      base.OnUnSelected();
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragBegin(CSharpFramework.View.HotSpotBase hotSpot, VisionViewBase view)
    {
      if (hotSpot == _hotSpotStartEvent) // button pressed
        ToggleStartEvent();
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    /// <param name="mode"></param>
    public override void OnHotSpotRender(HotSpotBase hotSpot, VisionViewBase view, ShapeRenderMode mode)
    {
      base.OnHotSpotRender(hotSpot, view, mode);
      UpdateHotspotVisibility();
    }

    #endregion

    #region Serialization and Export

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected FmodEventShape(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      _eventProjectPath = info.GetString("_eventProjectPath");
      _eventGroupName = info.GetString("_eventGroupName");
      _eventName = info.GetString("_eventName");
      _bStartPaused = info.GetBoolean("_bStartPaused");
      _bDisposeWhenFinished = info.GetBoolean("_bDisposeWhenFinished");
    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context)
    {
      base.GetObjectData(info, context);

      info.AddValue("_eventProjectPath", _eventProjectPath);
      info.AddValue("_eventGroupName", _eventGroupName);
      info.AddValue("_eventName", _eventName);
      info.AddValue("_bStartPaused", _bStartPaused);
      info.AddValue("_bDisposeWhenFinished", _bDisposeWhenFinished);
    }

    #endregion

    #region Plugin information

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <returns></returns>
    public override EditorPluginInfo GetPluginInformation()
    {
      return EditorPlugin.EDITOR_PLUGIN_INFO;
    }

    #endregion

  }

  #endregion

  #region Shape Creator Plugin

  /// <summary>
  /// Creator class. An instance of the creator is registered in the plugin init function. Thus the creator shows
  /// up in the "Create" menu of the editor
  /// </summary>
  class FmodEventShapeCreator : CSharpFramework.IShapeCreatorPlugin
  {

    /// <summary>
    /// Constructor
    /// </summary>
    public FmodEventShapeCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\event_loudspeaker.bmp"), Color.Magenta);
    }

    /// <summary>
    /// Get the name of the plugin, for instance the shape name. This name appears in the "create" menu
    /// </summary>
    /// <returns>creator name</returns>
    public override string GetPluginName()
    {
      return "Event";
    }

    /// <summary>
    /// Gets the plugin description 
    /// </summary>
    /// <returns></returns>
    public override string GetPluginDescription()
    {
      return "This shape represents a event instance in the world.";
    }

    /// <summary>
    /// Get the plugin category name to sort the plugin name. This is useful to group creators. A null string can
    /// be returned to put the creator in the root
    /// </summary>
    /// <returns></returns>
    public override string GetPluginCategory()
    {
      return "Fmod";
    }


    /// <summary>
    /// Create a shape instance
    /// </summary>
    /// <returns></returns>
    public override ShapeBase CreateShapeInstance()
    {
      // create a shape
      FmodEventShape Event = new FmodEventShape("Fmod - Event");
      Event.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
      return Event;
    }
  }

  #endregion

  #region EventGroupSelectionEditor

  public class EventGroupSelectionEditor : UITypeEditor
  {
    public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
    {
      return UITypeEditorEditStyle.DropDown;
    }

    private IWindowsFormsEditorService editorService = null;
    private TreeView treeView;

    private void treeView_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
    {
      editorService.CloseDropDown();
    }

    public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
    {
      editorService = provider.GetService(typeof(IWindowsFormsEditorService)) as IWindowsFormsEditorService;
      if (editorService == null)
        return value;

      FmodEventShape Shape = context.Instance as FmodEventShape;
      System.Diagnostics.Debug.Assert(Shape != null, "EventGroupSelectionEditor only valid for FmodEventShapes!");

      // Create a TreeView and populate it 
      treeView = new TreeView();
      treeView.ImageList = EditorManager.GUI.ShapeTreeImages.ImageList;
      treeView.Bounds = new Rectangle(0, 0, 400, 500);
      treeView.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.treeView_NodeMouseClick);
      FmodManaged.ManagedModule.GetEventGroupTree(Shape.EventProject, treeView);
      treeView.ExpandAll();

      // Show our listbox as a DropDownControl. 
      // This methods returns, when the DropDownControl is closed.
      editorService.DropDownControl(treeView);

      TreeNode selectedNode = treeView.SelectedNode;
      if (selectedNode != null)
      {
        string fullPath = selectedNode.FullPath;
        return fullPath.Replace("\\", "/");
      }
      else
        return Shape.EventGroup;
    }
  }

  #endregion

  #region EventSelectionEditor

  public class EventSelectionEditor : UITypeEditor
  {
    public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
    {
      return UITypeEditorEditStyle.DropDown;
    }

    private IWindowsFormsEditorService editorService = null;
    private TreeView treeView;
    private bool bIsEvent = false;

    private void treeView_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
    {
      editorService.CloseDropDown();
    }

    private void treeView_AfterSelect(object sender, TreeViewEventArgs e)
    {
      bIsEvent = (e.Node.Nodes.Count == 0);
    }

    public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
    {
      editorService = provider.GetService(typeof(IWindowsFormsEditorService)) as IWindowsFormsEditorService;
      if (editorService == null)
        return value;

      FmodEventShape Shape = context.Instance as FmodEventShape;
      System.Diagnostics.Debug.Assert(Shape != null, "EventSelectionEditor only valid for FmodEventShapes!");

      // Create a TreeView and populate it 
      treeView = new TreeView();
      treeView.ImageList = EditorManager.GUI.ShapeTreeImages.ImageList;
      treeView.Bounds = new Rectangle(0, 0, 400, 500);
      treeView.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(treeView_NodeMouseClick);
      treeView.AfterSelect += new TreeViewEventHandler(treeView_AfterSelect);
      FmodManaged.ManagedModule.GetEventTree(Shape.EventProject, Shape.EventGroup, treeView);
      treeView.ExpandAll();

      // Show our listbox as a DropDownControl. 
      // This methods returns, when the DropDownControl is closed.
      editorService.DropDownControl(treeView);

      if (!bIsEvent)
        return Shape.Event;

      TreeNode selectedNode = treeView.SelectedNode;
      if (selectedNode != null)
      {
        string fullPath = selectedNode.FullPath;
        return fullPath.Replace("\\", "/");
      }
      else
        return Shape.Event;
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
