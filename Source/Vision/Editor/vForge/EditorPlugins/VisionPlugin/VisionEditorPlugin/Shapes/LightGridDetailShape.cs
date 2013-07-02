/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.IO;
using System.Drawing.Design;
using System.ComponentModel;
using System.Globalization;
using System.Runtime.Serialization;
using CSharpFramework;
using ManagedFramework;
using CSharpFramework.Math;
using CSharpFramework.Shapes;
using CSharpFramework.PropertyEditors;
using CSharpFramework.DynamicProperties;
using CSharpFramework.UndoRedo;
using CSharpFramework.Actions;
using CSharpFramework.Scene;
using CSharpFramework.View;
using VisionManaged;
using CSharpFramework.Serialization;
using System.Drawing;

namespace VisionEditorPlugin.Shapes
{
	/// <summary>
	/// LightGridDetailShape : shape that adds detail subdivision to the light grid
	/// </summary>
  [Serializable, ShapeTypeNiceName("Lightgrid Detail Box")]
	public class LightGridDetailShape : Shape3D
	{

    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_BOXSIZE      = "Box Size";

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_BOXSUBDIV    = "Subdivisions";

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_GLOBAL    = "Global";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_BOXSIZE    = Shape3D.LAST_CATEGORY_ORDER_ID + 1;

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_BOXSUBDIV  = Shape3D.LAST_CATEGORY_ORDER_ID + 2;

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_GLOBAL  = Shape3D.LAST_CATEGORY_ORDER_ID + 3;

    /// <summary>
    /// Last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_GLOBAL;

    #endregion

    #region Constructor

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="nodeName"></param>
		public LightGridDetailShape(string nodeName) : base(nodeName)
		{
			AddHint(HintFlags_e.NoRotation);
      this._vBoxSize *= EditorManager.Settings.GlobalUnitScaling;
    }
    #endregion

    #region GUI

    /// <summary>
    /// Overridden function to get the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetIconIndex()
    {
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\lightgridbox.bmp"), Color.Magenta);
    }

    #endregion

    #region ShapeBase Members


    /// <summary>
    /// Sets parenting mode to scene graph
    /// </summary>
    protected override Shape3D.ParentingModes ParentingMode
    {
      get
      {
        return ParentingModes.SceneGraphScaled;
      }
    }

    /// <summary>
    /// Create the engine instance
    /// </summary>
    /// <param name="bCreateChildren"></param>
    public override void CreateEngineInstance(bool bCreateChildren)
    {
      base.CreateEngineInstance(bCreateChildren);

      this._engineInstance = new EngineInstanceLightGridDetailBox();
      SetEngineInstanceBaseProperties(); // sets the position etc.
    }

    // accesses the engine instance as an object of type EngineInstanceLightGridDetailBox
    EngineInstanceLightGridDetailBox EngineBox {get {return (EngineInstanceLightGridDetailBox)_engineInstance;}}

    /// <summary>
    /// overridden function to set/update all specific properties on the engine instance
    /// </summary>
    public override void SetEngineInstanceBaseProperties()
    {
      if (_engineInstance==null)
        return;

      base.SetEngineInstanceBaseProperties ();
      UpdateBox();
      EngineInstanceLightGridDetailBox.SetDisplayGrid(_bPreviewCells);
    }


    /// <summary>
    /// overridden function to render the shape
    /// </summary>
    /// <param name="view">destination view</param>
    /// <param name="mode">render mode</param>
    public override void RenderShape(VisionViewBase view, ShapeRenderMode mode)
    {
      base.RenderShape(view, mode);

      if (_engineInstance == null || mode != ShapeRenderMode.Selected)
        return;
      if (this.IsHintSet(HintFlags_e.Virtual)) // in prefabs it is disturbing
        return;
      EngineBox.RenderShape(view,mode);
    }

    public override void OnTransformationChanged()
    {
      base.OnTransformationChanged();

      // scaling might have changed:
      UpdateBox();

      if (_hotSpotSizeX != null)
        _hotSpotSizeX.DisplayScaling = 0.5f * ScaleX;
      if (_hotSpotSizeY != null)
        _hotSpotSizeY.DisplayScaling = 0.5f * ScaleY;
      if (_hotSpotSizeZ != null)
        _hotSpotSizeZ.DisplayScaling = 0.5f * ScaleZ;

    }

    public override bool GetLocalBoundingBox(ref BoundingBox bbox)
    {
      bbox = new BoundingBox(_vBoxSize * -0.5f, _vBoxSize * 0.5f);
      return true;
    }

    #endregion

    #region Light grid box properties

    Vector3F _vBoxSize = new Vector3F(100,100,100);
    int _iSubDivX = 4;
    int _iSubDivY = 4;
    int _iSubDivZ = 2;
    static bool _bPreviewCells = true; // globally enabled or disabled

    /// <summary>
    /// Get the absolute bounding box
    /// </summary>
    /// <returns></returns>
    public BoundingBox GetGridBox()
    {
      Vector3F scaled = new Vector3F(_vBoxSize.X * 0.5f * ScaleX, _vBoxSize.Y * 0.5f * ScaleY, _vBoxSize.Z * 0.5f * ScaleZ);
      return new BoundingBox(Position - scaled, Position + scaled);
    }


    void UpdateBox()
    {
      if (_engineInstance==null) return;
      EngineBox.SetBoxDimensions(_vBoxSize.X * ScaleX, _vBoxSize.Y * ScaleY, _vBoxSize.Z * ScaleZ);
      EngineBox.SetBoxSubdivisions(_iSubDivX, _iSubDivY, _iSubDivZ);
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
    /// Gets or sets the status of displaying the grid preview (for all detail box shapes)
    /// </summary>
    [SortedCategory(CAT_GLOBAL, CATORDER_GLOBAL),
    PropertyOrder(1),
    Description("Status of displaying the grid preview")]
    public bool Preview
    {
      get {return _bPreviewCells;}
      set
      {
        _bPreviewCells=value;
        EngineInstanceLightGridDetailBox.SetDisplayGrid(_bPreviewCells);
      }
    }

    /// <summary>
    /// get or set the box dimension
    /// </summary>
    [SortedCategory(CAT_BOXSIZE, CATORDER_BOXSIZE),
    PropertyOrder(1),
    Description("Size of the bounding box in x-direction")]
    public float BoxSizeX
    {
      get {return _vBoxSize.X;}
      set { _vBoxSize.X = value;UpdateBox();}
    }

    /// <summary>
    /// get or set the box dimension
    /// </summary>
    [SortedCategory(CAT_BOXSIZE, CATORDER_BOXSIZE),
    PropertyOrder(2),
    Description("Size of the bounding box in y-direction")]
    public float BoxSizeY
    {
      get {return _vBoxSize.Y;}
      set { _vBoxSize.Y = value;UpdateBox();}
    }
    
    /// <summary>
    /// get or set the box dimension
    /// </summary>
    [SortedCategory(CAT_BOXSIZE, CATORDER_BOXSIZE),
    PropertyOrder(3),
    Description("Size of the bounding box in z-direction")]
    public float BoxSizeZ
    {
      get {return _vBoxSize.Z;}
      set { _vBoxSize.Z = value;UpdateBox();}
    }

    /// <summary>
    /// get or set the number of grid subdivisions (x)
    /// </summary>
    [SortedCategory(CAT_BOXSUBDIV, CATORDER_BOXSUBDIV),
    PropertyOrder(1),
    Description("Number of cell subdivisions for cells that intersect")]
    public int SubDivX
    {
      get {return _iSubDivX;}
      set 
      {
        _iSubDivX = value;
        if (_iSubDivX<1) _iSubDivX = 1;
        if (_iSubDivX>255) _iSubDivX = 255;
        UpdateBox();
      }
    }

    /// <summary>
    /// get or set the number of grid subdivisions (y)
    /// </summary>
    [SortedCategory(CAT_BOXSUBDIV, CATORDER_BOXSUBDIV),
    PropertyOrder(2),
    Description("Number of cell subdivisions for cells that intersect")]
    public int SubDivY
    {
      get {return _iSubDivY;}
      set 
      {
        _iSubDivY = value;
        if (_iSubDivY<1) _iSubDivY = 1;
        if (_iSubDivY>255) _iSubDivY = 255;
        UpdateBox();
      }
    }

    /// <summary>
    /// get or set the number of grid subdivisions (z)
    /// </summary>
    [SortedCategory(CAT_BOXSUBDIV, CATORDER_BOXSUBDIV),
    PropertyOrder(3),
    Description("Number of cell subdivisions for cells that intersect")]
    public int SubDivZ
    {
      get {return _iSubDivZ;}
      set 
      {
        _iSubDivZ = value;
        if (_iSubDivZ<1) _iSubDivZ = 1;
        if (_iSubDivZ>255) _iSubDivZ = 255;
        UpdateBox();
      }
    }

    #endregion
    
    #region ISerializable

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected LightGridDetailShape(SerializationInfo info, StreamingContext context) : base(info, context)
    {
      _vBoxSize = (Vector3F)info.GetValue( "_vBoxSize", typeof(Vector3F) );
      _iSubDivX = info.GetInt32( "_iSubDivX" );
      _iSubDivY = info.GetInt32( "_iSubDivY" );
      _iSubDivZ = info.GetInt32( "_iSubDivZ" );
      if (SerializationHelper.HasElement(info,"_bPreviewCells"))
        _bPreviewCells = info.GetBoolean("_bPreviewCells");

      // backwards compatibility
      if (!SerializationHelper.HasElement(info, "SupportScaling"))
        SetScaling_Internal(1.0f, 1.0f, 1.0f);

			AddHint(HintFlags_e.NoRotation);

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
      info.AddValue("_iSubDivX", _iSubDivX);
      info.AddValue("_iSubDivY", _iSubDivY);
      info.AddValue("_iSubDivZ", _iSubDivZ);
      info.AddValue("_bPreviewCells",_bPreviewCells);
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
      LightGridDetailShape copy = (LightGridDetailShape)base.Clone();
      System.Diagnostics.Debug.Assert(!copy.Selected); // turned off in ShapeBase

      copy._hotSpotSizeX = null;
      copy._hotSpotSizeY = null;
      copy._hotSpotSizeZ = null;

      return copy;
    }

    #endregion

    #region light info update

    /// <summary>
    /// virtual function to collect information for static light update
    /// </summary>
    /// <param name="info"></param>
    protected override void GetStaticLightInfo(StaticLightInfo info)
    {
      info.SupplierShapes.Add(this);

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
      _hotSpotSizeX.Axis = Vector3F.XAxis;
      _hotSpotSizeX.ToolTipText = "size x direction";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotSizeX);

      // hotspot for size Y
      _hotSpotSizeY = new HotSpotDistance(this,@"textures\Hotspot_Rect.dds",VisionColors.Yellow,HotSpotBase.PickType.Square,4.0f);
      _hotSpotSizeY.StartDistance = this.BoxSizeY;
      _hotSpotSizeY.DisplayScaling = 0.5f * ScaleY;
      _hotSpotSizeY.Axis = Vector3F.YAxis;
      _hotSpotSizeY.ToolTipText = "size y direction";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotSizeY);
      
      // hotspot for size Z
      _hotSpotSizeZ = new HotSpotDistance(this,@"textures\Hotspot_Rect.dds",VisionColors.Yellow,HotSpotBase.PickType.Square,4.0f);
      _hotSpotSizeZ.StartDistance = this.BoxSizeZ;
      _hotSpotSizeZ.DisplayScaling = 0.5f * ScaleZ;
      _hotSpotSizeZ.Axis = Vector3F.ZAxis;
      _hotSpotSizeZ.ToolTipText = "size z direction";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotSizeZ);


      // add more hotspots here...
    }


    void SafeRemoveHotSpot(HotSpotBase hotSpot)
    {
      if (hotSpot==null) return;
      EditorManager.ActiveView.HotSpots.Remove(hotSpot);
      hotSpot.Remove();
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
        _hotSpotSizeX.Axis = Vector3F.XAxis;
        if (!_hotSpotSizeX.IsDragging)
          _hotSpotSizeX.StartDistance = this.BoxSizeX;
      }
      if (hotSpot==_hotSpotSizeY)
      {
        _hotSpotSizeY.Axis = Vector3F.YAxis;
        if (!_hotSpotSizeY.IsDragging)
          _hotSpotSizeY.StartDistance = this.BoxSizeY;
      }
      if (hotSpot==_hotSpotSizeZ)
      {
        _hotSpotSizeZ.Axis = Vector3F.ZAxis;
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

  #region Lightgrid box Creator Plugin

  class LightGridDetailBoxCreator : CSharpFramework.IShapeCreatorPlugin
  {
    public LightGridDetailBoxCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\lightgridbox.bmp"), Color.Magenta);
    }

    public override string GetPluginName()
    {
      return "Lightgrid - Detail Box";
    }

    public override string GetPluginCategory()
    {
      return "Lighting";
    }

    /// <summary>
    /// Returns a short description text
    /// </summary>
    /// <returns></returns>
    public override string GetPluginDescription()
    {
      return "Adds a light grid detail box to the scene.\n"+
        "This box increases light grid resolution of the light grid cells it touches.\n"+
        "The shape is only relevant for re-lighting and will thus not be exported.";
    }


    public override ShapeBase CreateShapeInstance()
    {
      LightGridDetailShape box = new LightGridDetailShape("LightGridDetail");
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
