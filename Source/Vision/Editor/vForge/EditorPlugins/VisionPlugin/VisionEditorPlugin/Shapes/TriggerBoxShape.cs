/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.IO;
using System.Collections.Generic;
using System.Text;
using CSharpFramework.Shapes;
using VisionManaged;
using System.ComponentModel;
using CSharpFramework;
using System.Runtime.Serialization;
using CSharpFramework.View;
using System.Drawing;
using CSharpFramework.Actions;
using CSharpFramework.PropertyEditors;
using CSharpFramework.Serialization;
using CSharpFramework.Math;
using CSharpFramework.Controls;
using System.Drawing.Design;
using CSharpFramework.Scene;

namespace VisionEditorPlugin.Shapes
{
  #region Class TriggerBoxShape

  [Serializable, ShapeTypeNiceName("Trigger Box")]
  public class TriggerBoxShape : ShapeObject3D
  {

    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_BOXSIZE      = "Trigger Shape";

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_BEHAVIOR    = "Behavior";

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_PREVIEW = "Preview";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_BOXSIZE    = Shape3D.LAST_CATEGORY_ORDER_ID + 1;

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_BEHAVIOR = Shape3D.LAST_CATEGORY_ORDER_ID + 2;

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_PREVIEW = Shape3D.LAST_CATEGORY_ORDER_ID + 3;

    /// <summary>
    /// Last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_PREVIEW;

    #endregion

    #region Constructor

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="nodeName"></param>
		public TriggerBoxShape(string nodeName) : base(nodeName)
		{
			//AddHint(HintFlags_e.NoRotation);
      _vBoxSize *= EditorManager.Settings.GlobalUnitScaling;
		}
    #endregion

    #region GUI

    /// <summary>
    /// Overridden function to get the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetIconIndex()
    {
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_triggerbox.bmp"), Color.Magenta);
    }

    #endregion

    #region ShapeBase Members

    /// <summary>
    /// Create the engine instance
    /// </summary>
    /// <param name="bCreateChildren"></param>
    public override void CreateEngineInstance(bool bCreateChildren)
    {
      this._engineInstance = new EngineInstanceTriggerBox();
      SetEngineInstanceBaseProperties(); // sets the position etc.

      base.CreateEngineInstance(bCreateChildren);
    }

    // accesses the engine instance as an object of type EngineInstanceTriggerBox
    EngineInstanceTriggerBox EngineBox { get { return (EngineInstanceTriggerBox)_engineInstance; } }

    /// <summary>
    /// overridden function to set/update all specific properties on the engine instance
    /// </summary>
    public override void SetEngineInstanceBaseProperties()
    {
      if (_engineInstance==null)
        return;

      base.SetEngineInstanceBaseProperties ();
      UpdateBox();
      EngineBox.SetObserverFlags(_iObserverFlags);
      EngineBox.SetDestroyOnTrigger(_bDestroyOnTrigger);
    }


    /// <summary>
    /// overridden function to render the shape
    /// </summary>
    /// <param name="view">destination view</param>
    /// <param name="mode">render mode</param>
    public override void RenderShape(VisionViewBase view, ShapeRenderMode mode)
    {
      base.RenderShape(view, mode);

      if (_engineInstance!=null)
      {
        if (mode == ShapeRenderMode.Selected || (ParentLayer == ParentLayer.ParentScene.ActiveLayer && _bPreview3D))
        {
          EngineBox.RenderShape(view, mode, VisionColors.Get(_previewColor));
        }
      }
    }

    public override bool GetLocalBoundingBox(ref BoundingBox bbox)
    {
      float r = SphereRadius;
      if (_shapeType == TriggerShape_e.Sphere)
        bbox = new BoundingBox(-r, -r, -r, r, r, r);
      else
        bbox = new BoundingBox(_vBoxSize * -0.5f, _vBoxSize * 0.5f);
      return true;
    }

    public override void OnAddedToScene()
    {
      base.OnAddedToScene();
      this.Scene2DRenderingEnabled = this._bPreview2D;
      UpdateTypeFlags();
    }

    public override void OnTransformationChanged()
    {
      base.OnTransformationChanged();
      TriggerRepaint2D(false);

      // scaling might have changed:
      UpdateBox();
      if (_hotSpotSizeX != null)
        _hotSpotSizeX.DisplayScaling = (TriggerShape == TriggerShape_e.Sphere) ? SphereRadius * UniformScaling : 0.5f * ScaleX;
      if (_hotSpotSizeY != null)
        _hotSpotSizeY.DisplayScaling = 0.5f * ScaleY;
      if (_hotSpotSizeZ != null)
        _hotSpotSizeZ.DisplayScaling = 0.5f * ScaleZ;

    }


    void TriggerRepaint2D(bool bForce)
    {
      if (_bPreview2D || bForce)
        Scene2DView.Update2DViews();
    }


    public override PropertyFlags_e GetPropertyFlags(PropertyFlagsProviderInfo pd)
    {
      if (TriggerShape == TriggerShape_e.Sphere)
      {
        if (pd.Name == "BoxSizeX" || pd.Name == "BoxSizeY" || pd.Name == "BoxSizeZ")
          return PropertyFlags_e.Hidden;
      }
      else
      if (TriggerShape == TriggerShape_e.Box)
      {
        if (pd.Name == "SphereRadius")
          return PropertyFlags_e.Hidden;
      }

      return base.GetPropertyFlags(pd);
    }

    #endregion

    #region 2D rendering

    static SolidBrush zoneBrush = new SolidBrush(Color.DarkGreen);
    static Pen zonePen = new Pen(Color.DarkGreen);

    public override void OnPaintScene2DView(CSharpFramework.Controls.Scene2DView view, System.Windows.Forms.PaintEventArgs e)
    {
      base.OnPaintScene2DView(view, e);

      BoundingBox absBox = this.AbsoluteBoundingBox;
      Point p1 = view.Transform(absBox.vMin);
      Point p2 = view.Transform(absBox.vMax);
      Rectangle rect = view.MakeValidRect(p1, p2);
      zoneBrush.Color = Color.FromArgb(120, _previewColor.R, _previewColor.G, _previewColor.B);
      zonePen.Color = Color.FromArgb(255, _previewColor.R, _previewColor.G, _previewColor.B);
      e.Graphics.FillRectangle(zoneBrush, rect);
      e.Graphics.DrawRectangle(zonePen, rect);
    }


    public override float GetScene2DRenderingSortingKey(CSharpFramework.Controls.Scene2DView view)
    {
      return AbsoluteBoundingBox.Z1; // bottom of box
    }

    static Scene2DViewFilterCategory FilterCat = null;
    public override Scene2DViewFilterCategory GetScene2DRenderingFilter(Scene2DView view)
    {
      if (FilterCat == null)
      {
        FilterCat = view.CreateFilterCategory("Trigger Boxes");
        if (IconIndex > 0)
          FilterCat.Image = EditorManager.GUI.ShapeTreeImages.ImageList.Images[IconIndex];
      }
      return FilterCat;
    }

    #endregion

    #region Box properties

    Vector3F _vBoxSize = new Vector3F(500,500,250);
    bool _bDestroyOnTrigger = false;
    TriggerBoxObserverFlags_e _iObserverFlags = TriggerBoxObserverFlags_e.MainCamera;
    Color _previewColor = Color.FromArgb(160,190,255);
    bool _bPreview3D = true;
    bool _bPreview2D = true;
    bool _bEnabled = true;
    TriggerShape_e _shapeType = TriggerShape_e.Box;

    [SortedCategory(CAT_BEHAVIOR, CATORDER_BEHAVIOR), PropertyOrder(1)]
    [Description("Defines whether callbacks should be triggered"), DefaultValue(true)]
    public bool Enabled
    {
      get { return _bEnabled; }
      set
      {
        _bEnabled = value;
        if (HasEngineInstance())
          EngineBox.SetEnabled(value);
      }
    }

    [Editor(typeof(FlagsEditor),typeof(System.Drawing.Design.UITypeEditor))]
    [SortedCategory(CAT_BEHAVIOR, CATORDER_BEHAVIOR), PropertyOrder(2)]
    [Description("Defines which objects trigger an event"), DefaultValue(TriggerBoxObserverFlags_e.MainCamera)]
    public TriggerBoxObserverFlags_e ObserverFlags
    {
      get { return _iObserverFlags; }
      set
      {
        _iObserverFlags = value;
        if (HasEngineInstance())
          EngineBox.SetObserverFlags(_iObserverFlags);
      }
    }

    [SortedCategory(CAT_BEHAVIOR, CATORDER_BEHAVIOR), PropertyOrder(3)]
    [Description("If enabled, the trigger destroys itself after triggering"), DefaultValue(false)]
    public bool DestroyOnTrigger
    {
      get { return _bDestroyOnTrigger; }
      set 
      { 
        _bDestroyOnTrigger = value;
        if (HasEngineInstance())
          EngineBox.SetDestroyOnTrigger(_bDestroyOnTrigger);
      }
    }

    void UpdateBox()
    {
      if (_engineInstance==null) 
        return;
      if (TriggerShape == TriggerShape_e.Box)
        EngineBox.SetBoxDimensions(_shapeType, _vBoxSize.X * ScaleX * 0.5f, _vBoxSize.Y * ScaleY * 0.5f, _vBoxSize.Z * ScaleZ * 0.5f);
      else
      {
        float r = SphereRadius * UniformScaling;
        EngineBox.SetBoxDimensions(_shapeType, r, r, r);
      }
      TriggerRepaint2D(false);
    }

    /// <summary>
    /// get or set the box dimension
    /// </summary>
    [BrowsableAttribute(false)]
    public Vector3F BoxSize
    {
      get {return _vBoxSize;}
      set {_vBoxSize = value;UpdateBox();}
    }

    /// <summary>
    /// get or set the box dimension
    /// </summary>
    [SortedCategory(CAT_BOXSIZE, CATORDER_BOXSIZE),
    PropertyOrder(1),
    Description("Defines the shape of the trigger volume"), DefaultValue(TriggerShape_e.Box)]
    public TriggerShape_e TriggerShape
    {
      get { return _shapeType; }
      set 
      { 
        _shapeType = value; 
        UpdateBox();
        UpdateHotspotVisibility();
        UpdateTypeFlags();
      }
    }

    /// <summary>
    /// get or set the box dimension
    /// </summary>
    [SortedCategory(CAT_BOXSIZE, CATORDER_BOXSIZE),
    PropertyOrder(10),
    Description("Size of the bounding box in x-direction"), DefaultValue(500f)]
    public float BoxSizeX
    {
      get {return _vBoxSize.X;}
      set { _vBoxSize.X = value;UpdateBox();}
    }

    /// <summary>
    /// get or set the box dimension
    /// </summary>
    [SortedCategory(CAT_BOXSIZE, CATORDER_BOXSIZE),
    PropertyOrder(11),
    Description("Size of the bounding box in y-direction"), DefaultValue(500f)]
    public float BoxSizeY
    {
      get {return _vBoxSize.Y;}
      set { _vBoxSize.Y = value;UpdateBox();}
    }
    
    /// <summary>
    /// get or set the box dimension
    /// </summary>
    [SortedCategory(CAT_BOXSIZE, CATORDER_BOXSIZE),
    PropertyOrder(12),
    Description("Size of the bounding box in z-direction"), DefaultValue(250f)]
    public float BoxSizeZ
    {
      get {return _vBoxSize.Z;}
      set { _vBoxSize.Z = value;UpdateBox();}
    }

    /// <summary>
    /// Radius of the trigger sphere. Maps to box x size
    /// </summary>
    [PrefabRelevant(false)]
    [SortedCategory(CAT_BOXSIZE, CATORDER_BOXSIZE),
    PropertyOrder(13),
    Description("Radius of the trigger sphere"), DefaultValue(250f)]
    public float SphereRadius
    {
      get { return _vBoxSize.X * 0.5f; }
      set { _vBoxSize.X = value * 2.0f; UpdateBox(); }
    }

    /// <summary>
    /// Indicates whether box should be shown in 3D view also when not selected
    /// </summary>
    [SortedCategory(CAT_PREVIEW, CATORDER_PREVIEW),PropertyOrder(1)]
    [Description("Indicates whether box should be shown in 3D view also when not selected"), DefaultValue(true)]
    public bool Preview3D
    {
      get { return _bPreview3D; }
      set 
      { 
        _bPreview3D = value; 
      }
    }


    /// <summary>
    /// Indicates whether box should be shown in the 2D overview map
    /// </summary>
    [SortedCategory(CAT_PREVIEW, CATORDER_PREVIEW), PropertyOrder(2)]
    [Description("Indicates whether box should be shown in the 2D overview map"), DefaultValue(true)]
    public bool Preview2D
    {
      get { return _bPreview2D; }
      set
      {
        _bPreview2D = value;
        this.Scene2DRenderingEnabled = _bPreview2D;
        TriggerRepaint2D(true);
      }
    }

    /// <summary>
    /// The color used to preview the box (both 3D and 2D)
    /// </summary>
    [SortedCategory(CAT_PREVIEW, CATORDER_PREVIEW), PropertyOrder(3)]
    [Description("The color used to preview the box (both 3D and 2D)")]
    [EditorAttribute(typeof(StandardColorDropDownEditor), typeof(UITypeEditor)), DefaultValue(typeof(Color), "160,190,255")]
    public Color PreviewColor
    {
      get { return _previewColor; }
      set
      {
        _previewColor = value;
        TriggerRepaint2D(false);
      }
    }
  

    #endregion
    
    #region ISerializable

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected TriggerBoxShape(SerializationInfo info, StreamingContext context) : base(info, context)
    {
      _vBoxSize = (Vector3F)info.GetValue( "_vBoxSize", typeof(Vector3F) );
			RemoveHint(HintFlags_e.NoRotation);
      _bDestroyOnTrigger = info.GetBoolean("_bDestroyOnTrigger");
      _iObserverFlags = (TriggerBoxObserverFlags_e)info.GetValue("_iObserverFlags", typeof(TriggerBoxObserverFlags_e));

      if (SerializationHelper.HasElement(info, "_bEnabled"))
        _bEnabled = info.GetBoolean("_bEnabled");

      if (SerializationHelper.HasElement(info, "_previewColor"))
        _previewColor = (Color)info.GetValue("_previewColor", typeof(Color));
      if (SerializationHelper.HasElement(info, "_bPreview2D"))
        _bPreview2D = info.GetBoolean("_bPreview2D");
      if (SerializationHelper.HasElement(info, "_bPreview3D"))
        _bPreview3D = info.GetBoolean("_bPreview3D");
      if (SerializationHelper.HasElement(info, "_shapeType"))
        _shapeType = (TriggerShape_e)info.GetValue("_shapeType",typeof(TriggerShape_e));
      // backwards compatibility
      if (!SerializationHelper.HasElement(info, "SupportScaling"))
        SetScaling_Internal(1.0f, 1.0f, 1.0f);
    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context) 
    {
      base.GetObjectData(info,context);

      info.AddValue("_vBoxSize", _vBoxSize);
      info.AddValue("_bDestroyOnTrigger", _bDestroyOnTrigger);
      info.AddValue("_iObserverFlags", _iObserverFlags);
      info.AddValue("_bEnabled", _bEnabled);
      info.AddValue("_bPreview2D", _bPreview2D);
      info.AddValue("_bPreview3D", _bPreview3D);
      info.AddValue("_previewColor", _previewColor);
      info.AddValue("_shapeType", _shapeType);
      info.AddValue("SupportScaling", true);
    }

    #endregion

    #region Cloning

    /// <summary>
    /// This function must be overridden, since we have to reset the hotspots
    /// </summary>
    /// <returns></returns>
    public override ShapeBase Clone()
    {
      TriggerBoxShape copy = (TriggerBoxShape)base.Clone();

      copy._hotSpotSizeX = null;
      copy._hotSpotSizeY = null;
      copy._hotSpotSizeZ = null;

      return copy;
    }

    #endregion

    #region Hotspot

    HotSpotDistance _hotSpotSizeX;        // a hotspot for the BoxSize X
    HotSpotDistance _hotSpotSizeY;        // a hotspot for the BoxSize Y
    HotSpotDistance _hotSpotSizeZ;        // a hotspot for the BoxSize Z

    /// <summary>
    /// Called when the shape is selected
    /// </summary>
    public override void OnSelected()
    {
      base.OnSelected ();
      System.Diagnostics.Debug.Assert(_hotSpotSizeX==null);
      System.Diagnostics.Debug.Assert(_hotSpotSizeY==null);
      System.Diagnostics.Debug.Assert(_hotSpotSizeZ==null);

      // create the hotspots:

      // hotspot for size X
      _hotSpotSizeX = new HotSpotDistance(this,@"textures\Hotspot_Rect.dds",VisionColors.Yellow,HotSpotBase.PickType.Square,4.0f);
      _hotSpotSizeX.StartDistance = this.BoxSizeX;
      _hotSpotSizeX.DisplayScaling = 0.5f * ScaleX;
      _hotSpotSizeX.Axis = XAxis;
      _hotSpotSizeX.ToolTipText = "size x direction";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotSizeX);

      // hotspot for size Y
      _hotSpotSizeY = new HotSpotDistance(this,@"textures\Hotspot_Rect.dds",VisionColors.Yellow,HotSpotBase.PickType.Square,4.0f);
      _hotSpotSizeY.StartDistance = this.BoxSizeY;
      _hotSpotSizeY.DisplayScaling = 0.5f * ScaleY;
      _hotSpotSizeY.Axis = YAxis;
      _hotSpotSizeY.ToolTipText = "size y direction";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotSizeY);
      
      // hotspot for size Z
      _hotSpotSizeZ = new HotSpotDistance(this,@"textures\Hotspot_Rect.dds",VisionColors.Yellow,HotSpotBase.PickType.Square,4.0f);
      _hotSpotSizeZ.StartDistance = this.BoxSizeZ;
      _hotSpotSizeZ.DisplayScaling = 0.5f * ScaleZ;
      _hotSpotSizeZ.Axis = ZAxis;
      _hotSpotSizeZ.ToolTipText = "size z direction";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotSizeZ);

      UpdateHotspotVisibility();

      // add more hotspots here...
    }


    void SafeRemoveHotSpot(HotSpotBase hotSpot)
    {
      if (hotSpot==null) return;
      EditorManager.ActiveView.HotSpots.Remove(hotSpot);
      hotSpot.Remove();
    }


    void UpdateHotspotVisibility()
    {
      if (_hotSpotSizeY != null)
        _hotSpotSizeY.Visible = TriggerShape == TriggerShape_e.Box;
      if (_hotSpotSizeZ != null)
        _hotSpotSizeZ.Visible = TriggerShape == TriggerShape_e.Box;
    }


    void UpdateTypeFlags()
    {
      if (TriggerShape == TriggerShape_e.Box)
        RemoveHint(HintFlags_e.OnlyUniformScale);
      else if (TriggerShape == TriggerShape_e.Sphere)
        AddHint(HintFlags_e.OnlyUniformScale);

    }

    /// <summary>
    /// Called when the shape is unselected
    /// </summary>
    public override void OnUnSelected()
    {
      SafeRemoveHotSpot(_hotSpotSizeX);_hotSpotSizeX=null;
      SafeRemoveHotSpot(_hotSpotSizeY);_hotSpotSizeY=null;
      SafeRemoveHotSpot(_hotSpotSizeZ);_hotSpotSizeZ=null;

      base.OnUnSelected ();
    }


    /// <summary>
    /// Called when user starts dragging a hotspot that belongs to this shape
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragBegin(HotSpotBase hotSpot, VisionViewBase view)
    {
      if (hotSpot==_hotSpotSizeX)
        _hotSpotSizeX.StartDistance = this.BoxSizeX;
      if (hotSpot==_hotSpotSizeY)
        _hotSpotSizeY.StartDistance = this.BoxSizeY;
      if (hotSpot==_hotSpotSizeZ)
        _hotSpotSizeZ.StartDistance = this.BoxSizeZ;
    }

    /// <summary>
    /// Called while the user drags a hotspot that belongs to this shape
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    /// <param name="fDeltaX"></param>
    /// <param name="fDeltaY"></param>
    public override void OnHotSpotDrag(HotSpotBase hotSpot, VisionViewBase view, float fDeltaX, float fDeltaY)
    {
      if (hotSpot==_hotSpotSizeX)
        this.BoxSizeX = _hotSpotSizeX.CurrentDistance; 
      if (hotSpot==_hotSpotSizeY)
        this.BoxSizeY = _hotSpotSizeY.CurrentDistance; 
      if (hotSpot==_hotSpotSizeZ)
        this.BoxSizeZ = _hotSpotSizeZ.CurrentDistance; 
    }

    /// <summary>
    /// Called when the user releases the mouse button on a hotspot
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragEnd(HotSpotBase hotSpot, VisionViewBase view)
    {
      if (hotSpot==_hotSpotSizeX)
      {
        if (_hotSpotSizeX.HasChanged)
        {
          float fNewSize = _hotSpotSizeX.CurrentDistance;
          this.BoxSizeX = _hotSpotSizeX.StartDistance; // set old value for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"BoxSizeX",fNewSize)); // send an action which sets the property from old value to new one
        }
      }
      if (hotSpot==_hotSpotSizeY)
      {
        if (_hotSpotSizeY.HasChanged)
        {
          float fNewSize = _hotSpotSizeY.CurrentDistance;
          this.BoxSizeY = _hotSpotSizeY.StartDistance; // set old value for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"BoxSizeY",fNewSize)); // send an action which sets the property from old value to new one
        }
      }
      if (hotSpot==_hotSpotSizeZ)
      {
        if (_hotSpotSizeZ.HasChanged)
        {
          float fNewSize = _hotSpotSizeZ.CurrentDistance;
          this.BoxSizeZ = _hotSpotSizeZ.StartDistance; // set old value for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"BoxSizeZ",fNewSize)); // send an action which sets the property from old value to new one
        }
      }
    }

    /// <summary>
    /// Called every frame and per hotspot that belongs to the shape
    /// </summary>
    /// <param name="hotSpot"></param>
    public override void OnHotSpotEvaluatePosition(HotSpotBase hotSpot)
    {
      if (hotSpot==_hotSpotSizeX)
      {
        _hotSpotSizeX.Axis = XAxis;
        if (!_hotSpotSizeX.IsDragging)
          _hotSpotSizeX.StartDistance = this.BoxSizeX;
      }
      if (hotSpot==_hotSpotSizeY)
      {
        _hotSpotSizeY.Axis = YAxis;
        if (!_hotSpotSizeY.IsDragging)
          _hotSpotSizeY.StartDistance = this.BoxSizeY;
      }
      if (hotSpot==_hotSpotSizeZ)
      {
        _hotSpotSizeZ.Axis = ZAxis;
        if (!_hotSpotSizeZ.IsDragging)
          _hotSpotSizeZ.StartDistance = this.BoxSizeZ;
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

  #region Trigger box Creator Plugin

  class TriggerBoxShapeCreator : CSharpFramework.IShapeCreatorPlugin
  {
    public TriggerBoxShapeCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_triggerbox.bmp"), Color.Magenta);
    }

    public override string GetPluginName()
    {
      return "Trigger Box";
    }

    public override string GetPluginCategory()
    {
      return "Objects";
    }

    /// <summary>
    /// Returns a short description text
    /// </summary>
    /// <returns></returns>
    public override string GetPluginDescription()
    {
      return "This shape represents an invisible, static bounding box that can respond to objects (or the camera) entering or leaving the box. Event listeners can be added using the linking mode.";
    }


    public override ShapeBase CreateShapeInstance()
    {
      TriggerBoxShape box = new TriggerBoxShape("TriggerBox");
      box.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
      return box;
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
