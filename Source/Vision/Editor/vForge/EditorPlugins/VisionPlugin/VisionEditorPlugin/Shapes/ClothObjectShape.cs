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
using System.Drawing.Design;
using System.ComponentModel;
using System.Globalization;
using System.Runtime.Serialization;
using CSharpFramework;
using ManagedFramework;
using CSharpFramework.Math;
using CSharpFramework.Shapes;
using CSharpFramework.PropertyEditors;
using CSharpFramework.UndoRedo;
using System.Drawing;
using System.Collections;
using CSharpFramework.Actions;
using CSharpFramework.Serialization;
using CSharpFramework.View;
using CSharpFramework.Dialogs;
using VisionManaged;
using System.Windows;
using System.Windows.Forms;
using CSharpFramework.Helper;
using CSharpFramework.Scene;

namespace VisionEditorPlugin.Shapes
{
  #region Class : Cloth object

	/// <summary>
	/// This shape represents a cloth object in the engine
	/// </summary>
	[DefaultProperty("Engine Class"), Serializable, ShapeTypeNiceName("Cloth Object")]
  public class ClothObjectShape : ShapeObject3D
	{
    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_CLOTHMODEL   = "Cloth Model";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_CLOTHMODEL = Shape3D.LAST_CATEGORY_ORDER_ID + 1;

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_CLOTHPHYSICS   = "Cloth Physics";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_CLOTHPHYSICS = Shape3D.LAST_CATEGORY_ORDER_ID + 2;

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_LIGHTING = "Lighting";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_LIGHTING = Shape3D.LAST_CATEGORY_ORDER_ID + 3;

    /// <summary>
    /// last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_LIGHTING;

    #endregion


    #region Constructor

    /// <summary>
    /// Shape contructor
    /// </summary>
    /// <param name="name">name of the shape</param>
    /// <param name="modelFile">Initial model file. Can be null</param>
		public ClothObjectShape(string name, string modelFile) : base(name)
		{
      _modelFile = modelFile;
		}

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="name"></param>
    public ClothObjectShape(string name)
      : base(name)
    {
    }

    #endregion

    #region GUI

    /// <summary>
    /// Overridden function to get the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetIconIndex()
    {
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_cloth.bmp"), Color.Magenta);
    }

    #endregion

    #region Engine instance

    /// <summary>
    /// Create the engine instance
    /// </summary>
    /// <param name="bCreateChildren"></param>
    public override void CreateEngineInstance(bool bCreateChildren)
    {
      base.CreateEngineInstance(bCreateChildren);

      this._engineInstance = new EngineInstanceClothObject();
      SetEngineInstanceBaseProperties(); // sets the position etc.
    }

    /// <summary>
    /// Access the engine instance as EngineInstanceClothObject
    /// </summary>
    [BrowsableAttribute(false)]
    public EngineInstanceClothObject EngineClothObj {get {return (EngineInstanceClothObject)_engineInstance;}}

    /// <summary>
    /// Sets all properties of the engine instance
    /// </summary>
    public override void SetEngineInstanceBaseProperties()
    {
      if (_engineInstance==null)
        return;
      string error="unknown error";
      bool bResult = EngineClothObj.SetModelFile(_modelFile, this.Scaling, ref error);
      if (!bResult)
      {
        EditorManager.ShowMessageBox(
          "The specified model file '"+_modelFile+"' could not be converted into a physics mesh.\n"+
          "Detailed error message:\n"+error+
          "\n\nPlease use an appropriate model file such as a planar mesh.",
          "Failed", MessageBoxButtons.OK,MessageBoxIcon.Error);
      }

      base.SetEngineInstanceBaseProperties ();
      EngineClothObj.SetPhysicsProperties(_iPhysicsTicks,_bSimulateWhenVisible,_fGravity,_iInitialPhysicsTickCount);
      EngineClothObj.SetLightgridSamplePosition(_vLightGridOfs.X,_vLightGridOfs.Y,_vLightGridOfs.Z);
      EngineClothObj.SetCastShadows(_bCastDynamicShadows);
      EngineClothObj.SetVisibleBitmask((uint)_iVisibleBitmask);
      EngineClothObj.SetLightInfluenceBitmask((uint)_iLightInfluenceBitmask);
      //EngineClothObj.SetEntityKey(_entityKey);
    }

    #endregion

    #region ISerializable

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected ClothObjectShape(SerializationInfo info, StreamingContext context) : base(info, context)
    {
      _modelFile = info.GetString("_modelFile");
      _iPhysicsTicks = info.GetInt32("_iPhysicsTicks");
      _fGravity = info.GetSingle("_fGravity");
      _bSimulateWhenVisible = info.GetBoolean("_bSimulateWhenVisible");
      if (SerializationHelper.HasElement(info,"_vLightGridOfs"))
        _vLightGridOfs = (Vector3F)info.GetValue("_vLightGridOfs", typeof(Vector3F));
      if (SerializationHelper.HasElement(info,"_bCastDynamicShadows"))
        _bCastDynamicShadows = info.GetBoolean("_bCastDynamicShadows");
      if (SerializationHelper.HasElement(info,"_iVisibleBitmask"))
        _iVisibleBitmask = (FlagsInt32_e)info.GetValue("_iVisibleBitmask", typeof(FlagsInt32_e));
      if (SerializationHelper.HasElement(info,"_iLightInfluenceBitmask"))
        _iLightInfluenceBitmask = (FlagsInt32_e)info.GetValue("_iLightInfluenceBitmask", typeof(FlagsInt32_e));
      if (SerializationHelper.HasElement(info,"_entityKey")) // OLD version, map to base member
        _objectKey = info.GetString("_entityKey");
    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context) 
    {
      base.GetObjectData(info,context);
      info.AddValue("_modelFile",_modelFile);
      info.AddValue("_iPhysicsTicks",_iPhysicsTicks);
      info.AddValue("_fGravity",_fGravity);
      info.AddValue("_bSimulateWhenVisible",_bSimulateWhenVisible);
      info.AddValue("_vLightGridOfs",_vLightGridOfs);
      info.AddValue("_bCastDynamicShadows",_bCastDynamicShadows);
      info.AddValue("_iVisibleBitmask",_iVisibleBitmask);
      info.AddValue("_iLightInfluenceBitmask", _iLightInfluenceBitmask);
      //info.AddValue("_entityKey",_entityKey); // base class
    }

    #endregion

    #region Cloning

    /// <summary>
    /// This function must be overridden, since we have to reset the hotspots
    /// </summary>
    /// <returns></returns>
    public override ShapeBase Clone()
    {
      ClothObjectShape copy = (ClothObjectShape)base.Clone();
      copy._hotSpotGravity = null;
      copy._hotSpotLightGridOfs = null;
      System.Diagnostics.Debug.Assert(!copy.Selected); // turned off in ShapeBase

      return copy;
    }

    #endregion

    #region Cloth Object properties

    string _modelFile = @"";
    int _iPhysicsTicks = 50;
    float _fGravity = 100.0f;
    bool _bSimulateWhenVisible=true;
    FlagsInt32_e _iVisibleBitmask = FlagsInt32_e.All;
    FlagsInt32_e _iLightInfluenceBitmask = FlagsInt32_e.Bit0;
    //string _entityKey = null;

    /// <summary>
    /// Gets or sets the v3o model file that is used to generate a cloth mesh
    /// </summary>
    [PrefabResolveFilename]
    [SortedCategory(CAT_CLOTHMODEL, CATORDER_CLOTHMODEL),
    PropertyOrder(1),
    EditorAttribute(typeof(AssetEditor), typeof(UITypeEditor)),
    AssetDialogFilter(new string[] { "Model" })]
    [ThumbnailFormat(new string[] { @"{0}_data\Thumbnail.bmp", @"{0}_data\Thumbnail.jpg" })]
    [Description("The model to generate a cloth mesh from.")]
    public string ModelFile
    {
      get {return _modelFile;}
      set 
      {
        if (_modelFile==value) return;
        _modelFile = value;
        SetEngineInstanceBaseProperties(); // set everything because a new model is created
      }
    }

    [Browsable(false)]
    public string EntityKey
    {
      get {return ObjectKey;}
      set 
      {
        ObjectKey = value;
      }
    }

    /// <summary>
    /// Cloth Entity visibility bitmask that determines the visible status in the rendering contexts
    /// </summary>
    [SortedCategory(CAT_CLOTHMODEL, CATORDER_CLOTHMODEL), PropertyOrder(2)]
    [Description("Cloth Entity visibility bitmask that determines the visible status in the rendering contexts"), DefaultValue(FlagsInt32_e.All)]
    public FlagsInt32_e VisibleBitmask
    {
      get {return _iVisibleBitmask;}
      set
      {
        _iVisibleBitmask = value;
        if (HasEngineInstance())
          EngineClothObj.SetVisibleBitmask((uint)_iVisibleBitmask);
      }
    }


    /// <summary>
    /// Get or set the number of physics simulation ticks per second. 0 for each frame
    /// </summary>
    [SortedCategory(CAT_CLOTHPHYSICS, CATORDER_CLOTHPHYSICS), PropertyOrder(1)]
    [Description("Defines the number of physics simulation ticks per second. 0 for each frame. Smaller values >0 save CPU performance"), DefaultValue(50)]
    public int SimulationTicks
    {
      get {return _iPhysicsTicks;}
      set 
      {
        _iPhysicsTicks = value;
        if (_iPhysicsTicks<0) _iPhysicsTicks=0;
        if (HasEngineInstance())
          EngineClothObj.SetPhysicsProperties(_iPhysicsTicks, _bSimulateWhenVisible, _fGravity, _iInitialPhysicsTickCount);
      }
    }

    /// <summary>
    /// Get or set the gravity constant
    /// </summary>
    [SortedCategory(CAT_CLOTHPHYSICS, CATORDER_CLOTHPHYSICS), PropertyOrder(2)]
    [Description("Defines the gravity constant"), DefaultValue(100f)]
    public float Gravity
    {
      get {return _fGravity;}
      set 
      {
        _fGravity = value;
        if (HasEngineInstance())
          EngineClothObj.SetPhysicsProperties(_iPhysicsTicks, _bSimulateWhenVisible, _fGravity, _iInitialPhysicsTickCount);
      }
    }

    /// <summary>
    /// Get or set the status for simulation
    /// </summary>
    [SortedCategory(CAT_CLOTHPHYSICS, CATORDER_CLOTHPHYSICS), PropertyOrder(3)]
    [Description("If enabled, physics simulation only takes place if the cloth mesh is visible. This saves CPU performance."), DefaultValue(true)]
    public bool SimulateWhenVisible
    {
      get {return _bSimulateWhenVisible;}
      set 
      {
        _bSimulateWhenVisible = value;
        if (HasEngineInstance())
          EngineClothObj.SetPhysicsProperties(_iPhysicsTicks, _bSimulateWhenVisible, _fGravity, _iInitialPhysicsTickCount);
      }
    }

    /// <summary>
    /// Defines the number of physics simulation ticks that are performed at startup to avoid artifacts
    /// </summary>
    [SortedCategory(CAT_CLOTHPHYSICS, CATORDER_CLOTHPHYSICS), PropertyOrder(4)]
    [Description("Defines the number of physics simulation ticks that are performed at startup to avoid artifacts"), DefaultValue(0)]
    public int InitialPhysicsTickCount
    {
      get {return _iInitialPhysicsTickCount;}
      set 
      {
        _iInitialPhysicsTickCount = value;
        if (_iInitialPhysicsTickCount<0) _iInitialPhysicsTickCount=0;
        if (HasEngineInstance())
          EngineClothObj.SetPhysicsProperties(_iPhysicsTicks, _bSimulateWhenVisible, _fGravity, _iInitialPhysicsTickCount);
      }
    }

    #region Lightgrid

    Vector3F _vLightGridOfs = Vector3F.Zero;
    bool _bCastDynamicShadows=false;
    static bool _bShowLightgridOfsWidget = false;
    int _iInitialPhysicsTickCount = 0;

    /// <summary>
    /// Defines the relative position offset that is used to gather the light grid sample
    /// </summary>
    [SortedCategory(CAT_LIGHTING, CATORDER_LIGHTING), PropertyOrder(3),
    Description("Defines the relative position offset that is used to gather the light grid sample")]
    [BrowsableAttribute(false)]
    public Vector3F LightGridSampleOfs
    {
      get {return _vLightGridOfs;}
      set 
      {
        _vLightGridOfs=value;
        if (HasEngineInstance())
          EngineClothObj.SetLightgridSamplePosition(_vLightGridOfs.X, _vLightGridOfs.Y, _vLightGridOfs.Z);
      }
    }

    
    /// <summary>
    /// If enabled, the cloth mesh casts a dynamic stencil shadow for dynamic lights
    /// </summary>
    [SortedCategory(CAT_LIGHTING, CATORDER_LIGHTING), PropertyOrder(1)]
    [Description("If enabled, the cloth mesh casts a dynamic stencil shadow for dynamic lights"), DefaultValue(false)]
    public bool CastDynamicShadows
    {
      get {return _bCastDynamicShadows;}
      set 
      {
        _bCastDynamicShadows = value;
        if (HasEngineInstance())
          EngineClothObj.SetCastShadows(_bCastDynamicShadows);
      }
    }

    /// <summary>
    /// Bitflag filter mask that determines which dynamic lights influence this entity. See also dynamic light's ObjectInfluenceMask property
    /// </summary>
    [SortedCategory(CAT_LIGHTING, CATORDER_LIGHTING), PropertyOrder(2)]
    [Description("Bitflag filter mask that determines which dynamic lights influence this entity. See also dynamic light's ObjectInfluenceMask property"), DefaultValue(FlagsInt32_e.Bit0)]
    public FlagsInt32_e LightInfluenceBitmask
    {
      get {return _iLightInfluenceBitmask;}
      set
      {
        _iLightInfluenceBitmask = value;
        if (HasEngineInstance())
          EngineClothObj.SetLightInfluenceBitmask((uint)_iLightInfluenceBitmask);
      }
    }


    /// <summary>
    /// If enabled, a 3D widget will be rendered that allows to modify the offset in the 3D view. This property wraps around a static bool so it affects all entities
    /// </summary>
    [SortedCategory(CAT_LIGHTING, CATORDER_LIGHTING), PropertyOrder(4)]
    [Description("If enabled, a 3D widget will be rendered that allows to modify the offset in the 3D view"), DefaultValue(false)]
    public bool ShowLightgridWidget
    {
      get {return _bShowLightgridOfsWidget;}
      set {_bShowLightgridOfsWidget=value;}
    }

    /// <summary>
    /// Gets or sets the relative x position offset that is used to gather the light grid sample
    /// </summary>
    [SortedCategory(CAT_LIGHTING, CATORDER_LIGHTING), PropertyOrder(5)]
    [Description("Defines the relative x position offset that is used to gather the light grid sample"), DefaultValue(0f)]
    public float SampleOfsX
    {
      get {return _vLightGridOfs.X;}
      set 
      {
        _vLightGridOfs.X=value;
        if (HasEngineInstance())
          EngineClothObj.SetLightgridSamplePosition(_vLightGridOfs.X, _vLightGridOfs.Y, _vLightGridOfs.Z);
      }
    }

    /// <summary>
    /// Gets or sets the relative y position offset that is used to gather the light grid sample
    /// </summary>
    [SortedCategory(CAT_LIGHTING, CATORDER_LIGHTING), PropertyOrder(6)]
    [Description("Defines the relative y position offset that is used to gather the light grid sample"), DefaultValue(0f)]
    public float SampleOfsY
    {
      get {return _vLightGridOfs.Y;}
      set 
      {
        _vLightGridOfs.Y=value;
        if (HasEngineInstance())
          EngineClothObj.SetLightgridSamplePosition(_vLightGridOfs.X, _vLightGridOfs.Y, _vLightGridOfs.Z);
      }
    }

    /// <summary>
    /// Gets or sets the relative z position offset that is used to gather the light grid sample
    /// </summary>
    [SortedCategory(CAT_LIGHTING, CATORDER_LIGHTING), PropertyOrder(7)]
    [Description("Defines the relative z position offset that is used to gather the light grid sample"), DefaultValue(0f)]
    public float SampleOfsZ
    {
      get {return _vLightGridOfs.Z;}
      set 
      {
        _vLightGridOfs.Z=value;
        if (HasEngineInstance())
          EngineClothObj.SetLightgridSamplePosition(_vLightGridOfs.X, _vLightGridOfs.Y, _vLightGridOfs.Z);
      }
    }

    #endregion


    #endregion

    #region Links

    /// <summary>
    /// Create all link sources and targets provided by this shape
    /// </summary>
    public override void CreateAllLinks()
    {
      base.CreateAllLinks();
      LinkTargets.Add(new LinkTargetConstraint(this, "ConstraintTarget_1")); // add link target that allows to assign the constraint to particle groups
    }

    /// <summary>
    /// Check whether src can be linked to target
    /// </summary>
    /// <param name="src"></param>
    /// <param name="target"></param>
    /// <returns></returns>
    public override bool CanLink(ShapeLink src, ShapeLink target)
    {
      if (base.CanLink(src,target))
        return true;
      if (target.OwnerShape==this)
      {
        if (src is LinkSourceConstraint) return true;
      }
      return false;
    }

    /// <summary>
    /// Performs the actal linking on engine instances
    /// </summary>
    /// <param name="src"></param>
    /// <param name="target"></param>
    public override void OnLink(ShapeLink src, ShapeLink target)
    {
      base.OnLink (src, target);

      // perform the linking on the engine objects
      if (target.OwnerShape==this)
      {
        ConstraintShape constraint = src.OwnerShape as ConstraintShape;
        if (constraint!=null && constraint._engineInstance!=null && this.HasEngineInstance()) // at least the engine instance of the constraint can be null
          EngineClothObj.AddConstraint(constraint._engineInstance as EngineInstanceConstraint);
      }
    }

    /// <summary>
    /// unlinks a target from a source. Either src or target has this shape as owner
    /// </summary>
    /// <param name="src">the link source</param>
    /// <param name="target">the link target</param>
    public override void OnUnlink(ShapeLink src, ShapeLink target)
    {
      base.OnUnlink (src, target);
      // perform the unlinking on the engine objects
      if (target.OwnerShape==this)
      {
        ConstraintShape constraint = src.OwnerShape as ConstraintShape;
        if (constraint != null && constraint._engineInstance != null && this.HasEngineInstance()) // at least the engine instance of the constraint can be null
          EngineClothObj.RemoveConstraint(constraint._engineInstance as EngineInstanceConstraint);
      }
    }


    #endregion

    #region Selection and hot spots

    HotSpotDistance _hotSpotGravity; // a hotspot for applying gravity
    HotSpot3D _hotSpotLightGridOfs = null;

    /// <summary>
    /// Overridden function
    /// </summary>
    public override void OnSelected()
    {
      base.OnSelected ();

      System.Diagnostics.Debug.Assert(_hotSpotGravity==null);
      System.Diagnostics.Debug.Assert(_hotSpotLightGridOfs==null);

      // hotspot for gravity
      _hotSpotGravity = new HotSpotDistance(this,@"textures\Hotspot_G.tga",VisionColors.Yellow,HotSpotBase.PickType.Square,4.0f);
      _hotSpotGravity.StartDistance = this.Gravity;
      _hotSpotGravity.Axis = -Vector3F.ZAxis; // always point down in world space
      _hotSpotGravity.ToolTipText = "Gravity";
      _hotSpotGravity.DisplayScaling = EditorManager.Settings.GlobalUnitScaling;
      EditorManager.ActiveView.HotSpots.Add(_hotSpotGravity);

      // light grid ofs
      Debug.Assert(_hotSpotLightGridOfs==null);
      _hotSpotLightGridOfs = new HotSpot3D(this,16.0f);
      _hotSpotLightGridOfs.ToolTipText = "light grid offset";
      _hotSpotLightGridOfs.StartPosition = LightGridSampleOfs;
      EditorManager.ActiveView.HotSpots.Add(_hotSpotLightGridOfs);

    }

    void SafeRemoveHotSpot(HotSpotBase hotSpot)
    {
      if (hotSpot==null) return;
      EditorManager.ActiveView.HotSpots.Remove(hotSpot);
      hotSpot.Remove();
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    public override void OnUnSelected()
    {
      // release the hotspots again:
      SafeRemoveHotSpot(_hotSpotGravity);  _hotSpotGravity = null;
      SafeRemoveHotSpot(_hotSpotLightGridOfs);  _hotSpotLightGridOfs = null;

      base.OnUnSelected ();
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    /// <param name="fDeltaX"></param>
    /// <param name="fDeltaY"></param>
    public override void OnHotSpotDrag(CSharpFramework.View.HotSpotBase hotSpot, VisionViewBase view, float fDeltaX, float fDeltaY)
    {
      if (hotSpot==_hotSpotGravity)
        Gravity = _hotSpotGravity.CurrentDistance; 
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragBegin(CSharpFramework.View.HotSpotBase hotSpot, VisionViewBase view)
    {
      if (hotSpot==_hotSpotGravity)
        _hotSpotGravity.StartDistance = this.Gravity;
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragEnd(CSharpFramework.View.HotSpotBase hotSpot, VisionViewBase view)
    {
      if (hotSpot==_hotSpotGravity)
      {
        if (_hotSpotGravity.HasChanged)
        {
          float fNewVal = _hotSpotGravity.CurrentDistance;
          Gravity = _hotSpotGravity.StartDistance; // set old value for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"Gravity",fNewVal)); 
        }
      }
      if (hotSpot==_hotSpotLightGridOfs)
      {
        Vector3F vNewPos = _hotSpotLightGridOfs.CurrentPosition;
        LightGridSampleOfs = _hotSpotLightGridOfs.StartPosition; // set old value for the action
        EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"LightGridSampleOfs",vNewPos)); // send an action which sets the property from old value to new one
      }

    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="hotSpot"></param>
    public override void OnHotSpotEvaluatePosition(CSharpFramework.View.HotSpotBase hotSpot)
    {
      if (hotSpot==_hotSpotGravity)
      {
        _hotSpotGravity.Axis = -Vector3F.ZAxis;
        if (!_hotSpotGravity.IsDragging)
          _hotSpotGravity.StartDistance = Gravity;
      }
      if (hotSpot==_hotSpotLightGridOfs)
      {
        _hotSpotLightGridOfs.Visible = this.ShowLightgridWidget;

        if (_hotSpotLightGridOfs.IsDragging) // update while dragging
          LightGridSampleOfs = _hotSpotLightGridOfs.CurrentPosition;
        else
          _hotSpotLightGridOfs.StartPosition = LightGridSampleOfs;
      }
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    /// <param name="mode"></param>
    public override void OnHotSpotRender(HotSpotBase hotSpot, VisionViewBase view, ShapeRenderMode mode)
    {
      base.OnHotSpotRender (hotSpot, view, mode);

      // render gravity as arrow
      if (hotSpot==_hotSpotGravity)
      {
        view.RenderArrow(Position, _hotSpotGravity.Position, VisionColors.Yellow, 4.0f, 2.0f * EditorManager.Settings.GlobalUnitScaling);
      }
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

  #region Cloth object creator Plugin

  class ClothObjectCreator : CSharpFramework.IShapeCreatorPlugin
  {
    public ClothObjectCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_cloth.bmp"), Color.Magenta);
    }

    public override string GetPluginName()
    {
      return "Cloth Object";
    }

    public override string GetPluginCategory()
    {
      return "Effects";
    }

    /// <summary>
    /// Returns a short description text
    /// </summary>
    /// <returns></returns>
    public override string GetPluginDescription()
    {
      return "Adds a cloth object to the scene.\n"+
        "A cloth object requires a .model file suitable for cloth simulation (e.g. a plane with subdivisions).\n"+
        "The engine performs physics simulation on the vertices. Constraints can be linked to the cloth.";
    }

    public override ShapeBase CreateShapeInstance()
    {
      ClothObjectShape shape = new ClothObjectShape("ClothObject","");
      return shape;
    }

    public override void ExecutePlugin()
    {
      string modelFile = null;

      // when running tests we don't want file selection dialog to pop up.
      if (!TestManager.IsRunning)
      {
        modelFile = @"Models\curtainmedi.V3O"; // default model file
        if (!EditorManager.EngineManager.File_Exists(modelFile))
        {
          // show a file open dialog to select a model file
          OpenFileDlg fileDlg = new OpenFileDlg();
          fileDlg.Caption = "Selecting the Cloth Model File";
          fileDlg.Description = "Please select the cloth model file you want to use and press OK to continue.";
          fileDlg.InitialDirectory = EditorManager.Project.ProjectDir;
          fileDlg.Filter = new string[] {".model"};
          if (fileDlg.ShowDialog() != DialogResult.OK)
            return;
          modelFile = fileDlg.FileName;
        }
      }
      ClothObjectShape shape = new ClothObjectShape("ClothObject",modelFile);
      shape.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
      Layer layer = EditorManager.Scene.ActiveLayer;
      EditorManager.Actions.Add(AddShapeAction.CreateAddShapeAction(shape, layer.ActiveShape, layer, true));
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
