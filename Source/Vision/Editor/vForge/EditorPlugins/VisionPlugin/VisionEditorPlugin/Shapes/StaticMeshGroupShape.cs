/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//#define MESHGROUP_USE_LINKING // also define in StaticMeshShape.cs

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
using System.Collections;
using CSharpFramework.Visitors;
using System.Reflection;


namespace VisionEditorPlugin.Shapes
{
  #if (MESHGROUP_USE_LINKING)

  #region Linking


  #region Visibility link source

  /// <summary>
  /// Link source class for a visibility object
  /// </summary>
  [Serializable]
  public class LinkSourceGroupStaticMeshes : LinkSource
  {
    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="ownerShape"></param>
    /// <param name="primaryStringId"></param>
    public LinkSourceGroupStaticMeshes(Shape3D ownerShape, string primaryStringId) : base(ownerShape,"affects",primaryStringId)
    {
    }

    /// <summary>
    /// Default constructor used by the factory class
    /// </summary>
    /// <param name="ownerShape"></param>
    /// <param name="linkName"></param>
    /// <param name="primaryStringId"></param>
    public LinkSourceGroupStaticMeshes(Shape3D ownerShape, string linkName, string primaryStringId)
      : base(ownerShape, linkName, primaryStringId)
    {
    }

    #region ISerializable Members

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected LinkSourceGroupStaticMeshes(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info,context);
    }

    #endregion

    #region Hotspot

    /// <summary>
    /// Get the icon filename
    /// </summary>
    public override string DefaultHotspotIconFilename
    {
      get {return @"Bitmaps\Shapes\houses.png";}
    }

    #endregion
  }

  #endregion

  #region Visibility link target

  /// <summary>
  /// Link target class for constraint target
  /// </summary>
  [Serializable]
  public class LinkTargetGroupStaticMeshes : LinkTarget
  {
    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="ownerShape"></param>
    /// <param name="primaryStringId"></param>
    public LinkTargetGroupStaticMeshes(Shape3D ownerShape, string primaryStringId) : base(ownerShape,"visibility determined by",primaryStringId)
    {
    }

    /// <summary>
    /// Default constructor used by the factory class
    /// </summary>
    /// <param name="ownerShape"></param>
    /// <param name="linkName"></param>
    /// <param name="primaryStringId"></param>
    public LinkTargetGroupStaticMeshes(Shape3D ownerShape, string linkName, string primaryStringId)
      : base(ownerShape, linkName, primaryStringId)
    {
    }

    #region ISerializable Members

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected LinkTargetGroupStaticMeshes(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info,context);
    }

    #endregion

    #region Hotspot

    /// <summary>
    /// get hot spot icon filename
    /// </summary>
    public override string DefaultHotspotIconFilename
    {
      get {return @"Bitmaps\Shapes\houses.png";}
    }

    #endregion

  #endregion

  }

  #endregion
  
#endif

  /// <summary>
  /// StaticMeshGroupShape : Bakes multiple static meshes into one mesh
  /// </summary>
  [Serializable, ShapeTypeNiceName("Static Mesh Group")]
  public class StaticMeshGroupShape : Shape3D
	{

    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_CONVERT = "Mesh Baking";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_CONVERT = Shape3D.LAST_CATEGORY_ORDER_ID + 2;

  #if (MESHGROUP_USE_LINKING)

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_BOXSIZE      = "Box Size";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_BOXSIZE    = Shape3D.LAST_CATEGORY_ORDER_ID + 1;

  #endif

    /// <summary>
    /// Last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_CONVERT;

    #endregion

    #region constructor 

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="nodeName"></param>
		public StaticMeshGroupShape(string nodeName) : base(nodeName)
		{
    #if (MESHGROUP_USE_LINKING)
      this._vBoxSize *= EditorManager.Settings.GlobalUnitScaling;
    #endif
      //AddHint(HintFlags_e.NoRotation);
      AddHint(HintFlags_e.NoScale);
		}

    #endregion

    #region GUI

    /// <summary>
    /// Overridden function to get the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetIconIndex()
    {
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_meshgroup.bmp"), Color.Magenta);
    }

    #endregion

    #region ShapeBase Members
    

    /// <summary>
    /// Create the engine instance
    /// </summary>
    /// <param name="bCreateChildren"></param>
    public override void CreateEngineInstance(bool bCreateChildren)
    {
      base.CreateEngineInstance(bCreateChildren);

      this._engineInstance = new EngineInstanceStaticMeshGroup();
      SetEngineInstanceBaseProperties(); // sets the position etc.
    }

    public override bool GetLocalBoundingBox(ref BoundingBox bbox)
    {
  #if (MESHGROUP_USE_LINKING)
      bbox = new BoundingBox(
         - _vBoxSize.X * 0.5f,  - _vBoxSize.Y * 0.5f,  - _vBoxSize.Z * 0.5f,
         + _vBoxSize.X * 0.5f,  + _vBoxSize.Y * 0.5f,  + _vBoxSize.Z * 0.5f);
      return true;
  #endif
      return false;
    }

    // accesses the engine instance as an object of type EngineInstanceStaticMeshGroup
    EngineInstanceStaticMeshGroup EngineBox { get { return (EngineInstanceStaticMeshGroup)_engineInstance; } }

    /// <summary>
    /// overridden function to set/update all specific properties on the engine instance
    /// </summary>
    public override void SetEngineInstanceBaseProperties()
    {
      if (_engineInstance==null)
        return;

      _engineInstance.SetObjectKey(_objectKey);
      base.SetEngineInstanceBaseProperties ();
    }


    /// <summary>
    /// overridden function to render the shape
    /// </summary>
    /// <param name="view">destination view</param>
    /// <param name="mode">render mode</param>
    public override void RenderShape(VisionViewBase view, ShapeRenderMode mode)
    {
      base.RenderShape(view, mode);

      if (_engineInstance != null && mode == ShapeRenderMode.Selected)
        EngineBox.RenderShape(view,mode,AbsoluteBoundingBox);
    }

    public override void OnAddedToScene()
    {
      base.OnAddedToScene();
    }


    public override void OnRemoveFromScene()
    {
      base.OnRemoveFromScene();
    }

    public override void OnChildAdded(ShapeBase childShape, int iDepth)
    {
      base.OnChildAdded(childShape, iDepth);
      this._relevantShapes = null;
    }

    public override void OnChildRemoved(ShapeBase childShape, int iDepth)
    {
      base.OnChildRemoved(childShape, iDepth);
      this._relevantShapes = null;
    }

    #endregion

    #region Group properties

    bool _bActive = true;

  #if (MESHGROUP_USE_LINKING)
    Vector3F _vBoxSize = new Vector3F(100,100,100);

    /// <summary>
    /// get or set the box dimension
    /// </summary>
    [BrowsableAttribute(false)]
    public Vector3F BoxSize
    {
      get {return _vBoxSize;}
      set {_vBoxSize = value;}
    }

    #region box size

    /// <summary>
    /// get or set the box dimension
    /// </summary>
    [SortedCategory(CAT_BOXSIZE, CATORDER_BOXSIZE),
    PropertyOrder(1),
    Description("Size of the bounding box in x-direction")]
    public float BoxSizeX
    {
      get {return _vBoxSize.X;}
      set { _vBoxSize.X = value;}
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
      set { _vBoxSize.Y = value;}
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
      set { _vBoxSize.Z = value;}
    }

    #endregion

  #endif

    #region Object key

    /// <summary>
    /// Object key string
    /// </summary>
    protected string _objectKey = null;

    /// <summary>
    /// Gets or sets the object key, so the object can be searched in the engine
    /// </summary>
    [Description("The new key used for this group instance mesh")]
    [SortedCategory(CAT_SHAPE, CATORDER_SHAPE), PropertyOrder(100)]
    public string ObjectKey
    {
      get { return _objectKey; }
      set
      {
        _objectKey = value;
        if (HasEngineInstance())
          _engineInstance.SetObjectKey(_objectKey);
      }
    }

    #endregion
    #region Active

    /// <summary>
    /// If enabled, this visibility object performs hardware occlusion test
    /// </summary>
    [SortedCategory(CAT_CONVERT, CATORDER_CONVERT),
    PropertyOrder(1),
    Description("If enabled, static meshes are actually converted")]
    public bool Active
    {
      get {return _bActive;}
      set { _bActive = value;}
    }

    #endregion
      
      #region ExportFileName
    /// <summary>
    /// Object key string
    /// </summary>
    protected string _exportFileName = null;

    /// <summary>
    /// Gets or sets the object key, so the object can be searched in the engine
    /// </summary>
    [Description("The new file name used after export of the group instance mesh. If it is empty the uniqueID is used.")]
    [SortedCategory(CAT_CONVERT, CATORDER_CONVERT)]
    public string ExportFileName
    {
      get { return _exportFileName; }
      set
      {
        //make sure that name is only unique if not inside a prefab
        if (IsAddedToScene)
        {
          _exportFileName = GetUniqueExportFileName(value);
        } 
        else
        {
          _exportFileName = value;
        }
        
          
      }
    }


    /// <summary>
    /// gets a unique export filename 
    /// </summary>
    /// <param name="wantedFileName">The filename that should be make unique</param>
    /// <returns>The new unique filename. May be equal the parameter wantedFileName when the name is already unique</returns>
    protected string GetUniqueExportFileName(string wantedFileName)
    {
      if (string.IsNullOrEmpty(wantedFileName))
      {
        return wantedFileName;
      }
      CheckExportFileNameUniqueVisitor isUniqueVisitor = new CheckExportFileNameUniqueVisitor(wantedFileName);
      EditorManager.Scene.RunShapeVisitor(isUniqueVisitor);
      if (isUniqueVisitor.IsUnique) 
      {
        return wantedFileName;
      }
      //export filename is not unique, so create new unique one
      string searchName;

      char[] chDigits = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
      searchName = wantedFileName.TrimEnd(chDigits);

      GetExportFileNameMaxIndexVisitor maxIndexVisitor = new GetExportFileNameMaxIndexVisitor(searchName, this);
      EditorManager.Scene.RunShapeVisitor(maxIndexVisitor);

      return searchName + (maxIndexVisitor.MaxIndex + 1).ToString();   
    }

    /// <summary>
    /// Visitor that checks if the export file name is already used for any StaticMeshGroupShape in the scene
    /// </summary>
    protected class CheckExportFileNameUniqueVisitor : IShapeVisitor
    {
      public CheckExportFileNameUniqueVisitor(string searchExportFileName)
      {
        _searchExportFileName = searchExportFileName;
      }

      public override IShapeVisitor.VisitResult Visit(ShapeBase shape)
      {
        if (shape is StaticMeshGroupShape)
        {
          if (((StaticMeshGroupShape)shape).ExportFileName == _searchExportFileName)
          {
            _isUnique = false;
            return VisitResult.VisitStop;
          }      
        }
        return VisitResult.VisitOk;
      }

      private bool _isUnique = true;
      public bool IsUnique
      {
        get { return _isUnique; }
      }
      private string _searchExportFileName = "";
    }


    /// <summary>
    /// Visitor that searches for the max index for a specific ExportFileName property
    /// </summary>
    protected class GetExportFileNameMaxIndexVisitor : IShapeVisitor
    {
      public GetExportFileNameMaxIndexVisitor(string searchExportFileName, ShapeBase ignoreShape)
      {
        _searchExportFileName = searchExportFileName;
        _ignoreShape = ignoreShape;
      }

      public override IShapeVisitor.VisitResult Visit(ShapeBase shape)
      {
        if (shape is StaticMeshGroupShape)
        {
          StaticMeshGroupShape groupShape = shape as StaticMeshGroupShape;

          if (groupShape == _ignoreShape || groupShape.ExportFileName == null || groupShape.ExportFileName == "")
          {
            return VisitResult.VisitOk;
          }

          if (groupShape.ExportFileName.StartsWith(_searchExportFileName))
          {
            int newIndex = -1;
            string remaining = groupShape.ExportFileName.Substring(_searchExportFileName.Length);
            if (remaining.Length > 0)
            {
              try
              {
                newIndex = Convert.ToInt32(remaining);
              }
              catch (Exception)
              {
                newIndex = -1;
              }
            }
            if (newIndex > _maxIndex)
            {
              _maxIndex = newIndex;
            }
          }
        }
        return VisitResult.VisitOk;
      }  

      private int _maxIndex = -1;
      public int MaxIndex
      {
        get { return _maxIndex; }
      }
      private string _searchExportFileName = "";
      private ShapeBase _ignoreShape;
    }



    #endregion

    #endregion

    #region ISerializable

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected StaticMeshGroupShape(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
     #if (MESHGROUP_USE_LINKING)
      if (SerializationHelper.HasElement(info,"_vBoxSize"))
      _vBoxSize = (Vector3F)info.GetValue( "_vBoxSize", typeof(Vector3F) );
     #endif
      _bActive = info.GetBoolean("_bActive");
      _objectKey = info.GetString("_objectKey");
      if (SerializationHelper.HasElement(info, "_exportFileName"))
      {
        _exportFileName = info.GetString("_exportFileName");
      }
      
      //AddHint(HintFlags_e.NoRotation);
      RemoveHint(HintFlags_e.NoRotation);
      AddHint(HintFlags_e.NoScale);
    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context) 
    {
      base.GetObjectData(info,context);

     #if (MESHGROUP_USE_LINKING)
      info.AddValue("_vBoxSize", _vBoxSize);
     #endif
      info.AddValue("_bActive", _bActive);
      info.AddValue("_objectKey", _objectKey);
      info.AddValue("_exportFileName", _exportFileName);
    }

    #endregion

    #region Cloning

    /// <summary>
    /// This function must be overridden, since we have to reset the hotspots
    /// </summary>
    /// <returns></returns>
    public override ShapeBase Clone()
    {
      StaticMeshGroupShape copy = (StaticMeshGroupShape)base.Clone();
      System.Diagnostics.Debug.Assert(!copy.Selected); // turned off in ShapeBase
      copy.ExportFileName = null;
      
     #if (MESHGROUP_USE_LINKING)
      copy._hotSpotSizeX = null;
      copy._hotSpotSizeY = null;
      copy._hotSpotSizeZ = null;
    #endif
      return copy;
    }

    #endregion

    #region Hotspot
    
  #if (MESHGROUP_USE_LINKING)
    HotSpotDistance _hotSpotSizeX;        // a hotspot for the BoxSize X
    HotSpotDistance _hotSpotSizeY;        // a hotspot for the BoxSize Y
    HotSpotDistance _hotSpotSizeZ;        // a hotspot for the BoxSize Z
  #endif

    /// <summary>
    /// Called when the shape is selected
    /// </summary>
    public override void OnSelected()
    {
      base.OnSelected ();
     #if (MESHGROUP_USE_LINKING)
      System.Diagnostics.Debug.Assert(_hotSpotSizeX==null);
      System.Diagnostics.Debug.Assert(_hotSpotSizeY==null);
      System.Diagnostics.Debug.Assert(_hotSpotSizeZ==null);

      // create the hotspots:

      // hotspot for size X
      _hotSpotSizeX = new HotSpotDistance(this,@"textures\Hotspot_Rect.dds",VisionColors.Yellow,HotSpotBase.PickType.Square,4.0f);
      _hotSpotSizeX.StartDistance = this.BoxSizeX;
      _hotSpotSizeX.DisplayScaling = 0.5f;
      _hotSpotSizeX.Axis = Vector3F.XAxis;
      _hotSpotSizeX.ToolTipText = "size x direction";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotSizeX);

      // hotspot for size Y
      _hotSpotSizeY = new HotSpotDistance(this,@"textures\Hotspot_Rect.dds",VisionColors.Yellow,HotSpotBase.PickType.Square,4.0f);
      _hotSpotSizeY.StartDistance = this.BoxSizeY;
      _hotSpotSizeY.DisplayScaling = 0.5f;
      _hotSpotSizeY.Axis = Vector3F.YAxis;
      _hotSpotSizeY.ToolTipText = "size y direction";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotSizeY);
      
      // hotspot for size Z
      _hotSpotSizeZ = new HotSpotDistance(this,@"textures\Hotspot_Rect.dds",VisionColors.Yellow,HotSpotBase.PickType.Square,4.0f);
      _hotSpotSizeZ.StartDistance = this.BoxSizeZ;
      _hotSpotSizeZ.DisplayScaling = 0.5f;
      _hotSpotSizeZ.Axis = Vector3F.ZAxis;
      _hotSpotSizeZ.ToolTipText = "size z direction";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotSizeZ);
    #endif

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
     #if (MESHGROUP_USE_LINKING)
      SafeRemoveHotSpot(_hotSpotSizeX);_hotSpotSizeX=null;
      SafeRemoveHotSpot(_hotSpotSizeY);_hotSpotSizeY=null;
      SafeRemoveHotSpot(_hotSpotSizeZ);_hotSpotSizeZ=null;
     #endif
      base.OnUnSelected ();
    }


    /// <summary>
    /// Called when user starts dragging a hotspot that belongs to this shape
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragBegin(HotSpotBase hotSpot, VisionViewBase view)
    {
     #if (MESHGROUP_USE_LINKING)
      if (hotSpot==_hotSpotSizeX)
        _hotSpotSizeX.StartDistance = this.BoxSizeX;
      if (hotSpot==_hotSpotSizeY)
        _hotSpotSizeY.StartDistance = this.BoxSizeY;
      if (hotSpot==_hotSpotSizeZ)
        _hotSpotSizeZ.StartDistance = this.BoxSizeZ;
     #endif
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
     #if (MESHGROUP_USE_LINKING)
      if (hotSpot==_hotSpotSizeX)
        this.BoxSizeX = _hotSpotSizeX.CurrentDistance; 
      if (hotSpot==_hotSpotSizeY)
        this.BoxSizeY = _hotSpotSizeY.CurrentDistance; 
      if (hotSpot==_hotSpotSizeZ)
        this.BoxSizeZ = _hotSpotSizeZ.CurrentDistance; 
     #endif
    }

    /// <summary>
    /// Called when the user releases the mouse button on a hotspot
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragEnd(HotSpotBase hotSpot, VisionViewBase view)
    {
     #if (MESHGROUP_USE_LINKING)
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
     #endif
    }

    /// <summary>
    /// Called every frame and per hotspot that belongs to the shape
    /// </summary>
    /// <param name="hotSpot"></param>
    public override void OnHotSpotEvaluatePosition(HotSpotBase hotSpot)
    {
     #if (MESHGROUP_USE_LINKING)
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
     #endif
    }

    #endregion

    #region Linking

    #if (MESHGROUP_USE_LINKING)

    public static string LINK_SOURCE_ID = "GroupMeshesSource_1";

    /// <summary>
    /// Create all link sources and targets provided by this shape
    /// </summary>
    public override void CreateAllLinks()
    {
      base.CreateAllLinks ();
      LinkSources.Add(new LinkSourceGroupStaticMeshes(this, LINK_SOURCE_ID));
    }

    /// <summary>
    /// Check whether src can be linked to target. Either src or target has this shape as owner
    /// </summary>
    /// <param name="src">the link source</param>
    /// <param name="target">the link target</param>
    /// <returns></returns>
    public override bool CanLink(ShapeLink src, ShapeLink target)
    {
      if (base.CanLink(src,target))
        return true;
      if (src.OwnerShape==this)
      {
        if (target is LinkTargetGroupStaticMeshes) 
          return true;
      }
      return false;
    }

    public override void OnLink(ShapeLink src, ShapeLink target)
    {
      base.OnLink(src, target);
      _relevantShapes = null;
    }

    public LinkSourceGroupStaticMeshes GetGroupStaticMeshesLinkSource() 
    {
      return (LinkSourceGroupStaticMeshes)LinkSources.GetLinkByPrimaryId(LINK_SOURCE_ID);
    }

#endif

    #endregion

    #region Relevant Operations

#if (MESHGROUP_USE_LINKING)
    static string RO_ADJUST_BBOX = "Adjust bounding box to linked objects";
    static string RO_LINK_TO_TOUCHING_SHAPES = "Link to all shapes that touch the bounding box";
    static string RO_LINK_TO_SHAPES_INSIDE = "Link to all shapes that are inside the bounding box";

    #region Nested class : EvaluateBBoxLinksVisitor

    public class EvaluateBBoxLinksVisitor : IShapeVisitor
    {
      public EvaluateBBoxLinksVisitor(StaticMeshGroupShape shape, bool bTouching)
      {
        _shape = shape;
        _srcLink = (LinkSource)_shape.GetGroupStaticMeshesLinkSource();
        _bTouching = bTouching;
        visBBox = _shape.AbsoluteBoundingBox;
      }

      public override IShapeVisitor.VisitResult Visit(ShapeBase shape)
      {
        BoundingBox bbox = shape.AbsoluteBoundingBox;
        if (bbox==null || !bbox.Valid)
          return VisitResult.VisitOk;

        if (_bTouching)
        {
          if (!visBBox.Intersects(bbox))
            return VisitResult.VisitOk;
        }
        else
        {
          if (!visBBox.IsInside(bbox))
            return VisitResult.VisitOk;
        }

        foreach (LinkTarget tgt in shape.LinkTargets)
          if (shape.CanLink(_srcLink, tgt) && !shape.IsLinkedBySource(_srcLink))
            RelevantTargets.Add(tgt);

        return VisitResult.VisitOk;
      }

      bool _bTouching;
      StaticMeshGroupShape _shape;
      LinkSource _srcLink;
      BoundingBox visBBox;
      public LinkCollection RelevantTargets = new LinkCollection();
    }

    #endregion

#endif

    static string RO_RECENTER= "Re-center group pivot";

    public override ArrayList RelevantOperations
    {
      get
      {
        ArrayList arr = base.RelevantOperations;
        if (arr == null)
          arr = new ArrayList(4);
     #if (MESHGROUP_USE_LINKING)
        ShapeLink src = GetGroupStaticMeshesLinkSource();
        if (src != null && src.Links.Count > 0)
          arr.Add(RO_ADJUST_BBOX);

        arr.Add(RO_LINK_TO_TOUCHING_SHAPES);
        arr.Add(RO_LINK_TO_SHAPES_INSIDE);
     #endif
        ShapeCollection shapes = GetRelevantShapes();
        if (shapes != null && shapes.Count > 0)
          arr.Add(RO_RECENTER);
        return arr;
      }
    }

    public override void PerformRelevantOperation(string name, int iShapeIndex, int iShapeCount)
    {
      base.PerformRelevantOperation(name, iShapeIndex, iShapeCount);

      if (name == RO_RECENTER)
      {
        ShapeCollection shapes = GetRelevantShapes();
        TriggerRecenterPosition(shapes);
        return;
      }

     #if (MESHGROUP_USE_LINKING)
      if (name == RO_ADJUST_BBOX)
      {
        ShapeLink src = GetGroupStaticMeshesLinkSource();
        if (src == null || src.Links.Count == 0)
          return;

        BoundingBox newbbox = new BoundingBox();
        foreach (LinkTarget tgt in src.Links)
        {
          BoundingBox bbox = tgt.OwnerShape.AbsoluteBoundingBox;
          if (bbox != null && bbox.Valid)
            newbbox.AddBox(bbox);
        }
        if (newbbox.Valid)
        {
          EditorManager.Actions.StartGroup(RO_ADJUST_BBOX);
          Vector3F size = new Vector3F(newbbox.SizeX,newbbox.SizeY,newbbox.SizeZ);
          EditorManager.Actions.Add(new MoveShapeAction(this,this.Position,newbbox.Center));
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this, "BoxSize", size));

          EditorManager.Actions.EndGroup();
        }
      }

      if (name == RO_LINK_TO_TOUCHING_SHAPES || name == RO_LINK_TO_SHAPES_INSIDE)
      {
        EvaluateBBoxLinksVisitor visitor = new EvaluateBBoxLinksVisitor(this, name == RO_LINK_TO_TOUCHING_SHAPES);
        foreach (Layer layer in EditorManager.Scene.Layers)
          if (layer.Modifiable)
            layer.Root.RunVisitor(visitor);

        if (visitor.RelevantTargets.Count > 0)
        {
          LinkSource linkSrc = GetGroupStaticMeshesLinkSource();
          EditorManager.Actions.StartGroup(name);
          foreach (LinkTarget target in visitor.RelevantTargets)
            EditorManager.Actions.Add(new LinkAction(linkSrc, target));

          EditorManager.Actions.EndGroup();
        }
      }
     #endif
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

    #region Export

    ShapeCollection _relevantShapes = null;

    void RecursiveAddRelevantChildren(ShapeCollection childList)
    {
      foreach (ShapeBase child in childList)
      {
        if (child is StaticMeshShape)
          _relevantShapes.Add(child);
        if (child.HasChildren())
          RecursiveAddRelevantChildren(child.ChildCollection);
      }
    }

    ShapeCollection GetRelevantShapes()
    {
      if (_relevantShapes == null)
      {
        _relevantShapes = new ShapeCollection();
       #if (MESHGROUP_USE_LINKING)
        LinkCollection links = GetGroupStaticMeshesLinkSource().Links;
        foreach (LinkTarget t in links)
          if (t.OwnerShape is StaticMeshShape)
            _relevantShapes.Add(t.OwnerShape);
       #else
        RecursiveAddRelevantChildren(ChildCollection);
       #endif
      }
      return _relevantShapes;
    }

    public override void OnSceneEvent(SceneEventArgs e)
    {
      base.OnSceneEvent(e);
      if (!Active || !ExportShape || EngineBox == null)
        return;

      if (e.action == SceneEventArgs.Action.BeforeExport || e.action == SceneEventArgs.Action.BeforePrefabBinaryExport)
      {
        ShapeCollection shapes = GetRelevantShapes();
        if (shapes == null || shapes.Count == 0)
          return;
        string name;
        string fileextension = ".vmesh";
        if (string.IsNullOrEmpty(_exportFileName))
        {
          //use UID
          name = string.Format("SM_{0:x8}_{1:x8}", ParentLayer.LayerID, this.LocalID);
        }
        else
        {
          name = _exportFileName;
        }
        name += fileextension;

        string filename = null;
        if (e is ExportSceneEventArgs)
          filename = Path.Combine(((ExportSceneEventArgs)e).ExportInfo.AbsoluteExportDataFolder, name);
        else
        {
          // this path is taken by BeforePrefabBinaryExport
          PropertyInfo property = EditorManager.Scene.GetType().GetProperty("AbsoluteZoneExportFolder");
          if (property != null)
            filename = property.GetValue(EditorManager.Scene, null) as string;
          if (string.IsNullOrEmpty(filename))
            return;
          filename = Path.Combine(filename, name);
        }
        filename = EditorManager.Project.MakeRelative(filename);

        EngineBox.fCombinedFarClipDist = -1.0f;
        EngineBox.iCombinedVisibleBitmask = 0;
        EngineBox.iCombinedCollisionBitmask = 0;
        EngineBox.bCombinedStaticShadows = false;
        EngineBox.bCombinedDynamicShadows = false;

        foreach (StaticMeshShape shape in shapes)
        {
          if (StaticMeshShape.UsesCollisionGroups)
          {
            EngineBox.iCombinedCollisionBitmask = shape.EngineMesh.GetCollisionFilter();
          }
          else
            EngineBox.iCombinedCollisionBitmask |= (uint)shape.CollisionBitmask;
          EngineBox.iCombinedVisibleBitmask |= (uint)shape.VisibleBitmask;
          EngineBox.fCombinedFarClipDist = Math.Max(EngineBox.fCombinedFarClipDist, shape.FarClipDistance);
          EngineBox.bCombinedStaticShadows |= shape.CastStaticShadows;
          EngineBox.bCombinedDynamicShadows |= shape.CastDynamicShadows;
        }

        EngineBox.BeginExport(shapes, filename);
        return;
      }
      if (e.action == SceneEventArgs.Action.AfterExport || e.action == SceneEventArgs.Action.AfterPrefabBinaryExport)
      {
        EngineBox.EndExport();
        return;
      }
    }


    #endregion

  }

  #region StaticMeshGroupShape Creator Plugin

  class StaticMeshGroupShapeCreator : CSharpFramework.IShapeCreatorPlugin
  {

    public StaticMeshGroupShapeCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_meshgroup.bmp"), Color.Magenta);
    }

    public override string GetPluginName()
    {
      return "Static Mesh Group";
    }

    public override string GetPluginCategory()
    {
      return "Geometry";
    }

    /// <summary>
    /// Returns a short description text
    /// </summary>
    /// <returns></returns>
    public override string GetPluginDescription()
    {
      return "Adds a static mesh group to the scene.\n" +
        "Static meshes that are children of this group are baked into one big mesh at export time.\n"+
        "Can be used to optimize a larger number of low poly meshes (e.g. fence segments).\n";
    }

    public override ShapeBase CreateShapeInstance()
    {
      StaticMeshGroupShape group = new StaticMeshGroupShape("StaticMeshGroup");
      group.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
      return group;
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
