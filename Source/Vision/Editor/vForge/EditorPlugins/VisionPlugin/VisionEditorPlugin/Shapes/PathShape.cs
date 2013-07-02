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
using System.Collections;
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
using CSharpFramework.PickHandlers;

namespace VisionEditorPlugin.Shapes
{

  #region Add Node Hotspot button

  /// <summary>
  /// Hot spot button that inserts a path node
  /// </summary>
  class HotSpotAddPathNode : HotSpotPushButton
  {
    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="owner">owner shape</param>
    /// <param name="iInsert">node index to insert at</param>
    /// <param name="t">time value [0..1] position on the path</param>
    public HotSpotAddPathNode(PathShape owner, int iInsert, float t) 
      : base(owner,@"Textures\Hotspot_add.dds",VisionColors.RGBA(255,255,255,180),VisionColors.White,8.0f)
    {
      _t = t;
      _iInsert = iInsert;
      if (_t<0.0f || t>1.0f)
        ToolTipText = "Append a path node here";
      else
        ToolTipText = "Insert a path node here";
    }

    /// <summary>
    /// returns the owner as a path shape
    /// </summary>
    PathShape Path {get {return (PathShape)Owner;}}

    /// <summary>
    /// overridden function that evaluates the position on the path
    /// </summary>
    public override void EvaluatePosition()
    {
      if (_t<0.0f)
        Position = Path.BeforeFirstNodePosition; // insert before first
      else if (_t>1.0f)
        Position = Path.AfterLastNodePosition; // insert after last
      else
        Position = Path.EvaluatePathPosition(_t);
    }

    /// <summary>
    /// Overridden function to insert the path node
    /// </summary>
    /// <param name="view"></param>
    public override void OnClicked(VisionViewBase view)
    {
      base.OnClicked (view);
      PathNodeShape newNode = new PathNodeShape();
      newNode.Position = Position;// - Path.Position;
      newNode.ChildIndex = _iInsert;
      EditorManager.Actions.Add(AddShapeAction.CreateAddShapeAction(newNode, Path, Path.ParentLayer, false));
      newNode.SmoothTangents();

      string nodeName;
      for (int i = 0; true; i++)
      {
        nodeName = "node" + i;
        bool found = false;
        foreach (PathNodeShape node in Path.ChildCollection)
        {
          if (node.ShapeName == nodeName)
            found = true;
        }
        if (!found)
          break;
      }
      newNode.ShapeName = nodeName;

      EditorManager.ActiveView.Gizmo.SetSingleShape(newNode, false);
    }


    int _iInsert = 0;
    float _t = 0.0f;
  }

  #endregion

  #region Linking

  /// <summary>
  /// Link target class for paths
  /// </summary>
  [Serializable]
  public class LinkTargetPath : LinkTargetObject3D
  {
    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="ownerShape"></param>
    /// <param name="primaryStringId"></param>
    public LinkTargetPath(PathShape ownerShape, string primaryStringId)
      : base(ownerShape, "use path", primaryStringId)
    {
      this.iIndex = 0; // -1 would discard this in the entity. We don't need the index here
    }

    /// <summary>
    /// Default constructor used by the factory class
    /// </summary>
    /// <param name="ownerShape"></param>
    /// <param name="linkName"></param>
    /// <param name="primaryStringId"></param>
    public LinkTargetPath(Shape3D ownerShape, string linkName, string primaryStringId)
      : base(ownerShape, linkName, primaryStringId)
    {
      this.iIndex = 0; // -1 would discard this in the entity. We don't need the index here
    }


    #region ISerializable Members

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected LinkTargetPath(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      this.iIndex = 0; // -1 would discard this in the entity. We don't need the index here
    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);
    }

    #endregion
    /*
    #region Hotspot

    /// <summary>
    /// get hot spot icon filename
    /// </summary>
    public override string DefaultHotspotIconFilename
    {
      get { return @"Bitmaps\Shapes\path.bmp"; }
    }

    #endregion
    */
  }

  #endregion


  #region Path Shape
	/// <summary>
	/// Spline path shape.
	/// </summary>
  [DefaultProperty("Engine Class"),
   Serializable,
   ShapeTypeNiceName("Path")
  ]
  public class PathShape : ShapeObject3D
	{
    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_PATH = "Path Properties";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_PATH = Shape3D.LAST_CATEGORY_ORDER_ID + 1;

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_DISPLAY = "Display";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_DISPLAY = Shape3D.LAST_CATEGORY_ORDER_ID + 2;

    /// <summary>
    /// last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_DISPLAY;

    #endregion

    #region Constructor

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="name"></param>
    public PathShape(string name) : base(name)
		{
		}

    protected override void DisposeObject()
    {
      base.DisposeObject();
    }
  
    #endregion

    #region GUI

    /// <summary>
    /// Overridden function to get the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetIconIndex()
    {
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\path.bmp"), Color.Magenta);
    }

    #endregion

    #region ShapeBase Members

    /// <summary>
    /// Create the engine instance
    /// </summary>
    /// <param name="bCreateChildren"></param>
    public override void CreateEngineInstance(bool bCreateChildren)
    {
      // create all children (important here, because the path node needs the children path nodes)
      this._engineInstance = new EngineInstancePath();
      base.CreateEngineInstance(bCreateChildren);
      
      SetEngineInstanceBaseProperties(); // sets the position etc.
      Update();
      UpdateEngineInstanceAttachment(); // this was not called due to the order of creation above
    }


    public override bool GetLocalBoundingBox(ref BoundingBox bbox)
    {
      PathNodeShape[] nodes = this.PathNodes;
      if (nodes == null || nodes.Length == 0)
        return false;
      bbox = new BoundingBox();
      foreach (PathNodeShape node in nodes)
        bbox.AddPoint(node.LocalSpacePosition);
      bbox.AddBorder(5.0f * EditorManager.Settings.GlobalUnitScaling);
      return true;
    }

    /// <summary>
    /// accesses the engine instance as an object of type EngineInstancePath
    /// </summary>
    [BrowsableAttribute(false)]
    public EngineInstancePath EnginePath {get {return (EngineInstancePath)_engineInstance;}}

    /// <summary>
    /// overridden function to set/update all specific properties on the engine instance
    /// </summary>
    public override void SetEngineInstanceBaseProperties()
    {
      if (_engineInstance==null)
        return;

      base.SetEngineInstanceBaseProperties ();
      EnginePath.SetClosed(_bClosed);
      //EnginePath.SetKey(_pathKey);
      EnginePath.SetDisplayColor(VisionColors.Get(_displayColor));
      EnginePath.SetLineDisplayMode(_lineDisplayMode);
    }

    public override void CreateAllLinks()
    {
      base.CreateAllLinks();
      LinkTargets.Add(new LinkTargetPath(this, "usepath"));
    }


    public override bool CanLink(ShapeLink src, ShapeLink target)
    {
      if (target.OwnerShape == this && (target is LinkTargetPath))
        return true;
      return base.CanLink(src, target);
    }


    /// <summary>
    /// overridden function to render the shape
    /// </summary>
    /// <param name="view">destination view</param>
    /// <param name="mode">render mode</param>
    public override void RenderShape(VisionViewBase view, ShapeRenderMode mode)
    {
      if (_engineInstance==null)
        return;
      EnginePath.RenderShape(view,mode);
      base.RenderShape(view,mode); // also render bounding box

      if (this._fShowPathTimeHelper >= 0.0f)
      {
        Vector3F showWidget = new Vector3F();
        EnginePath.EvaluatePathPositionSmooth(_fShowPathTimeHelper, ref showWidget);
        float r = 2.0f*EditorManager.Settings.GlobalUnitScaling;
        view.RenderLineBox(showWidget.X - r, showWidget.Y - r, showWidget.Z - r, showWidget.X + r, showWidget.Y + r, showWidget.Z + r, VisionColors.Cyan, 1.0f);
      }
    }

    protected override Shape3D.ParentingModes ParentingMode
    {
      get
      {
        return Shape3D.ParentingModes.SceneGraphScaled;
      }
    }

    public override void OnAddedToScene()
    {
      base.OnAddedToScene();

      EditorManager.BeginPickShape += new BeginPickShapeEventHandler(OnBeginPickShape);
      EditorManager.EndPickShape += new EndPickShapeEventHandler(OnEndPickShape);
    }

    public override void OnRemoveFromScene()
    {
      base.OnRemoveFromScene();

      EditorManager.BeginPickShape -= new BeginPickShapeEventHandler(OnBeginPickShape);
      EditorManager.EndPickShape -= new EndPickShapeEventHandler(OnEndPickShape);
    }

    #endregion

    #region ISerializable

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected PathShape(SerializationInfo info, StreamingContext context) : base(info, context)
    {
      _bClosed = info.GetBoolean("_bClosed");

      if (SerializationHelper.HasElement(info, "_pathKey")) // OLD version maps to base member
        _objectKey = info.GetString("_pathKey");

      _pathNodes = null;
      if (SerializationHelper.HasElement(info,"_displayColor"))
        _displayColor = (Color)info.GetValue("_displayColor",typeof(Color));
      if (SerializationHelper.HasElement(info,"_lineDisplayMode"))
        _lineDisplayMode = (EngineInstancePath.PathLineDisplayMode_e)info.GetValue("_lineDisplayMode",typeof(EngineInstancePath.PathLineDisplayMode_e));
    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context) 
    {
      base.GetObjectData(info,context);

      info.AddValue("_bClosed",_bClosed);
      //info.AddValue("_pathKey",_pathKey);
      info.AddValue("_displayColor",_displayColor);
      info.AddValue("_lineDisplayMode",_lineDisplayMode);
    }

    public override void OnDeserialization()
    {
      base.OnDeserialization();

      // add new link
      if (LinkTargets.GetLinkByPrimaryId("usepath") == null)
        LinkTargets.Add(new LinkTargetPath(this, "usepath"));

    }


    #endregion

    #region Callbacks

    void OnBeginPickShape(object sender, BeginPickShapeArgs e)
    {
      if (EditorManager.EngineManager.TypeManager_AllowShapeReference(e.OwnerObject, e.Property, this))
      {
        EnginePath.SetPickRendering(true);
      }
    }

    void OnEndPickShape(object sender, EndPickShapeArgs e)
    {
      if (object.Equals(this, e.Shape))
      {
        // TODO: animate path shape in some way
      }

      EnginePath.SetPickRendering(false);
    }

    #endregion

    #region Tracing

    /// <summary>
    /// Overridden trace function
    /// </summary>
    /// <param name="mode"></param>
    /// <param name="rayStart">ray start position</param>
    /// <param name="rayEnd">ray end position</param>
    /// <param name="result">trace result</param>
    public override void OnTraceShape(ShapeTraceMode_e mode, Vector3F rayStart, Vector3F rayEnd, ref ShapeTraceResult result)
    {
      if (mode == ShapeTraceMode_e.ShapePicking) // accurate picking should only be implemented for picking, not drop to floor
        base.OnTraceShape(mode, rayStart, rayEnd, ref result);
    }

    #endregion

    #region Path properties

    bool _bClosed = false;
    //string _pathKey = null;
    Color _displayColor = Color.Green;
    EngineInstancePath.PathLineDisplayMode_e _lineDisplayMode = EngineInstancePath.PathLineDisplayMode_e.Always;
    float _fShowPathTimeHelper = -1.0f;

    /// <summary>
    /// Determines whether the path is closed or not
    /// </summary>
    [SortedCategory(CAT_PATH, CATORDER_PATH), PropertyOrder(1),
    Description("Determines whether path is closed")]
    public bool Closed
    {
      get {return _bClosed;}
      set 
      {
        if (_bClosed == value)
          return;
        _bClosed = value;
        if (HasEngineInstance())
          Update(); // since close flag changes node layout
      }
    }

    /// <summary>
    /// Gets or sets the path key
    /// </summary>
    [Browsable(false)]
    public string PathKey
    {
      get {return ObjectKey;}
      set 
      {
        ObjectKey = value;
      }
    }

    /// <summary>
    /// Gets or sets the display color of the path (in vForge)
    /// </summary>
    [SortedCategory(CAT_DISPLAY, CATORDER_DISPLAY), PropertyOrder(1),
    Description("The display color of the path (in vForge only)")]
    [EditorAttribute(typeof(StandardColorDropDownEditor), typeof(UITypeEditor))]
    [PropertyLiveUpdate("DisplayColorLiveUpdate")]
    public Color DisplayColor
    {
      get {return _displayColor;}
      set 
      {
        _displayColor = value;
        if (HasEngineInstance())
          EnginePath.SetDisplayColor(VisionColors.Get(_displayColor));
      }
    }

    /// <summary>
    /// Proxy property used for color live update (See PropertyLiveUpdate attribute in LightColor property)
    /// </summary>
    [Browsable(false)]
    public object DisplayColorLiveUpdate
    {
      set
      {
        Color col = _displayColor;
        if (value is Color)
          col = (Color)value;
        if (HasEngineInstance())
          EnginePath.SetDisplayColor(VisionColors.Get(col));
        EditorManager.ActiveView.UpdateView(false);
      }
    }


    /// <summary>
    /// Gets or sets the display color of the path (in vForge)
    /// </summary>
    [SortedCategory(CAT_DISPLAY, CATORDER_DISPLAY), PropertyOrder(1),
    Description("The display color of the path (in vForge only)")]
    public EngineInstancePath.PathLineDisplayMode_e LineDisplay
    {
      get {return _lineDisplayMode;}
      set 
      {
        _lineDisplayMode = value;
        if (HasEngineInstance())
          EnginePath.SetLineDisplayMode(_lineDisplayMode);
      }
    }


    [PrefabRelevant(false)]
    [SortedCategory(CAT_DISPLAY, CATORDER_DISPLAY), PropertyOrder(1)]
    [Description("Helper visualization value. if set to range [0..1] then a cyan box will indicate the actual position on the path for this parameter. Can be picked via the 'Pick path time' relevant operation.")]
    public float ShowPathTimeHelper
    {
      get { return _fShowPathTimeHelper; }
      set 
      { 
        _fShowPathTimeHelper = value;
        if (_fShowPathTimeHelper < 0.0f)
          _fShowPathTimeHelper = -1.0f;
        else if (_fShowPathTimeHelper > 1.0f)
          _fShowPathTimeHelper = 1.0f;
      }
    }



    #endregion

    #region Path update

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="child"></param>
    public override void AddChild(ShapeBase child)
    {
      base.AddChild (child);
      if (child is PathNodeShape)
        Update();
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="child"></param>
    public override void RemoveChild(ShapeBase child)
    {
      base.RemoveChild (child);
      if (child is PathNodeShape)
        Update();
    }


    /// <summary>
    /// Re-creates the engine instance
    /// </summary>
    public void Update()
    {
      _pathNodes = null; // invalidate the cached array
      if (_engineInstance==null)
        return;

      RemoveInsertNodeHotSpots();

      // build new engine path nodes
      EnginePath.Reset();
      PathNodeShape [] nodes = PathNodes;
      foreach (PathNodeShape shape in nodes)
      {
        shape.EnsureHasEngineInstance();
        EnginePath.AddPathNode(shape.EnginePathNode);

        // FIX: Set node properties after engine path instance has been set
        shape.SetEngineInstanceBaseProperties();
      }

      EnginePath.SetClosed(_bClosed); // has to be set again
      EnginePath.InvalidateBoundingBox();
      OnSingleNodeChangedPosition(null);
    }


    /// <summary>
    /// Must be called from each node when it changed position
    /// </summary>
    public void OnSingleNodeChangedPosition(PathNodeShape node)
    {
      PathNodeShape [] nodes = PathNodes;
      foreach (PathNodeShape shape in nodes)
        shape.UpdateNode();
      if (HasEngineInstance())
        EnginePath.InvalidateBoundingBox();
    }

    public override void OnZoneRepositioned()
    {
      base.OnZoneRepositioned();
      EnginePath.InvalidateBoundingBox();
      OnSingleNodeChangedPosition(null);
    }

    #endregion

    #region Path evaluation

    /// <summary>
    /// Returns the absolute position at the specified time value
    /// </summary>
    /// <param name="t"></param>
    /// <returns></returns>
    public Vector3F EvaluatePathPosition(float t)
    {
      if (_engineInstance==null || PathNodeCount<1)
        return Position;

      Vector3F pos = new Vector3F();
      EnginePath.EvaluatePathPosition(t, ref pos);
      return pos;
    }

    /// <summary>
    /// Returns the normalized path direction vector at the specified time value
    /// </summary>
    /// <param name="t"></param>
    /// <returns></returns>
    public Vector3F EvaluatePathDirection(float t)
    {
      if (_engineInstance == null || PathNodeCount < 2) // we need two nodes for path direction
        return Vector3F.Zero;

      Vector3F dir = new Vector3F();
      EnginePath.EvaluatePathDirection(t, ref dir);
      return dir;
    }

    /// <summary>
    /// Returns an extrapolated position before the first node (for the hotspot)
    /// </summary>
    [BrowsableAttribute(false)]
    public Vector3F BeforeFirstNodePosition
    {
      get
      {
        return EvaluatePathPosition(0.0f) - 40.0f*EvaluatePathDirection(0.0f);
      }
    }

    /// <summary>
    /// Returns an extrapolated position after the last node (for the hotspot)
    /// </summary>
    [BrowsableAttribute(false)]
    public Vector3F AfterLastNodePosition
    {
      get
      {
        return EvaluatePathPosition(1.0f) + 40.0f*EvaluatePathDirection(0.998f);
      }
    }

    /// <summary>
    /// Counts the number of path nodes in the child collection
    /// </summary>
    [BrowsableAttribute(false)]
    public int PathNodeCount
    {
      get {return PathNodes.Length;}
    }

    PathNodeShape[] _pathNodes = null; // cached array

    /// <summary>
    /// Returns an array of path nodes in the child collection
    /// </summary>
    [BrowsableAttribute(false)]
    public PathNodeShape[] PathNodes
    {
      get 
      {
        if (_pathNodes==null) // rebuild
        {
          int iCount = 0;
          foreach (ShapeBase shape in ChildCollection) 
            if (shape is PathNodeShape)
              iCount++;

          if (this.Closed && iCount>0) // duplicate node at the end
            iCount++;

          _pathNodes = new PathNodeShape[iCount];
          iCount = 0;
          foreach (ShapeBase shape in ChildCollection)
            if (shape is PathNodeShape)
            {
              _pathNodes[iCount] = (PathNodeShape)shape;
              _pathNodes[iCount]._iPathNodeIndex = iCount;
              iCount++;
            }

          if (this.Closed && iCount > 0)
            _pathNodes[iCount] = _pathNodes[0];
 
        }
        return _pathNodes;
      }
    }


    /// <summary>
    /// Smooths all path node tangents. Optionally via an action
    /// </summary>
    /// <param name="bViaAction"></param>
    public void SmoothAll(bool bViaAction)
    {
      if (bViaAction)
         EditorManager.Actions.StartGroup("Path node handles : Smooth");
      foreach (PathNodeShape node in PathNodes)
      {
        Vector3F tanIn = node.SmoothInTangent;
        Vector3F tanOut = node.SmoothOutTangent;

        if (bViaAction)
        {
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(node,"TangentIn",tanIn));
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(node,"TangentOut",tanOut));
        }
        else
        {
          node.TangentIn = tanIn;
          node.TangentOut = tanOut;
        }
      }
      if (bViaAction)
        EditorManager.Actions.EndGroup();
    }

    #endregion

    #region Relevant operations

    static string RO_PICK_PATHTIME = @"Pick path time";
    static string RO_RECENTER_PIVOT = @"Recenter pivot";

    /// <summary>
    /// Get the relevant operations
    /// </summary>
    public override System.Collections.ArrayList RelevantOperations
    {
      get
      {
        ArrayList op = base.RelevantOperations;
        if (op == null) op = new ArrayList(4);
        if (HasEngineInstance())
        {
          op.Add(RO_PICK_PATHTIME);
          op.Add(RO_RECENTER_PIVOT);
        }
        return op;
      }
    }

        /// <summary>
    /// Perform the relevant operation
    /// </summary>
    /// <param name="name"></param>
    /// <param name="iShapeIndex"></param>
    /// <param name="iShapeCount"></param>
    public override void PerformRelevantOperation(string name, int iShapeIndex, int iShapeCount)
    {
      if (name == RO_PICK_PATHTIME)
      {
        if (iShapeIndex == 0)
          EditorManager.ActiveView.PickHandler = new PathTimePicker(this);
        return;
      }
      else if (name == RO_RECENTER_PIVOT)
      {
        EditorManager.Actions.Add(new RecenterPivotAction(this));
        return;
      }
      base.PerformRelevantOperation(name, iShapeIndex, iShapeCount);
    }

    #endregion

    #region Selection and HotSpots

    /// <summary>
    /// Overridden clone function
    /// </summary>
    /// <returns></returns>
    public override ShapeBase Clone()
    {
      PathNodeShape.IsPathCloning = true; // otherwise path nodes would not be cloned along with the path
      PathShape clone = (PathShape)base.Clone();
      PathNodeShape.IsPathCloning = false;
      clone._hotSpotNewNodes = null;
      clone._pathNodes = null;

      return clone;
    }


    ArrayList _hotSpotNewNodes = null;

    /// <summary>
    /// Create all hotspot buttons that allow to add nodes
    /// </summary>
    public void CreateInsertNodeHotSpots()
    {
      if (_hotSpotNewNodes!=null) // already created
        return;

      PathNodeShape[] nodes = PathNodes;
      int iCount = nodes.Length;
      _hotSpotNewNodes = new ArrayList(iCount+1);
      HotSpotAddPathNode hs;
      for (int i=1;i<iCount;i++)
      {
        // insert hot spots in between the nodes
        hs = new HotSpotAddPathNode(this,i,((float)i-0.5f)/(float)(iCount-1));
        EditorManager.ActiveView.HotSpots.Add(hs);
        _hotSpotNewNodes.Add(hs);
      }

      if (!_bClosed)
      {
        // insert before first
        hs = new HotSpotAddPathNode(this,0,-1.0f);
        EditorManager.ActiveView.HotSpots.Add(hs);
        _hotSpotNewNodes.Add(hs);

        // insert after last
        hs = new HotSpotAddPathNode(this,iCount,2.0f);
        EditorManager.ActiveView.HotSpots.Add(hs);
        _hotSpotNewNodes.Add(hs);
      }
    }

    /// <summary>
    /// Remove all hotspot buttons that allow to add nodes
    /// </summary>
    public void RemoveInsertNodeHotSpots()
    {
      if (_hotSpotNewNodes==null)
        return;
      foreach (HotSpotAddPathNode hs in _hotSpotNewNodes)
        EditorManager.ActiveView.HotSpots.Remove(hs);
      _hotSpotNewNodes = null;
    }

    /// <summary>
    /// Overridden OnSelected function
    /// </summary>
    public override void OnSelected()
    {
      base.OnSelected ();
      CreateInsertNodeHotSpots();
    }

    /// <summary>
    /// Overridden OnUnSelected function
    /// </summary>
    public override void OnUnSelected()
    {
      base.OnUnSelected ();
      RemoveInsertNodeHotSpots();
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

  #region Path Node Shape

  /// <summary>
  /// Shape that represents a single path node
  /// </summary>
  [DefaultProperty("Engine Class"),
  Serializable
  ]
  public class PathNodeShape : Shape3D
  {

    /// <summary>
    /// Enum type for path node in and out type.
    /// </summary>
    public enum PathNodeInOutType
    {
      /// <summary>
      /// User defined tangent
      /// </summary>
      Custom = 0,

      /// <summary>
      /// Always linear type
      /// </summary>
      Linear = 1,

      /// <summary>
      /// Always smooth bezier type
      /// </summary>
      Smooth = 2
    }


    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_TANGENTS = "Tangents";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_TANGENTS = Shape3D.LAST_CATEGORY_ORDER_ID + 1;

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_DISPLAY = "Global Display Mode";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_DISPLAY = Shape3D.LAST_CATEGORY_ORDER_ID + 2;

    /// <summary>
    /// last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_DISPLAY;

    #endregion

    #region Constructor

    /// <summary>
    /// Constructor
    /// </summary>
    public PathNodeShape() : base("node")
    {
    }

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="name"></param>
    public PathNodeShape(string name)
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
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\pin_green.bmp"), Color.Magenta);
    }

    #endregion

    #region Tracing

    /// <summary>
    /// Overridden trace function
    /// </summary>
    /// <param name="mode"></param>
    /// <param name="rayStart">ray start position</param>
    /// <param name="rayEnd">ray end position</param>
    /// <param name="result">trace result</param>
    public override void OnTraceShape(ShapeTraceMode_e mode, Vector3F rayStart, Vector3F rayEnd, ref ShapeTraceResult result)
    {
      if (mode != ShapeTraceMode_e.ShapePicking || !HasEngineInstance())
        return;
      if (ConversionUtils.TraceOrientedBoundingBox(LocalBoundingBox, Position, RotationMatrix, rayStart, rayEnd, ref result))
        result.hitShape = this;
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

      if (_engineInstance==null)
      {
        this._engineInstance = new EngineInstancePathNode();
        SetEngineInstanceBaseProperties(); // sets the position etc.
      }
    }

    /// <summary>
    /// Helper function to guarantee that the node has a valid engine instance
    /// </summary>
    public void EnsureHasEngineInstance()
    {
      if (_engineInstance==null)
        CreateEngineInstance(false);
    }


    /// <summary>
    /// accesses the engine instance as an object of type EngineInstancePathNode
    /// </summary>
    [BrowsableAttribute(false)]
    public EngineInstancePathNode EnginePathNode {get {return (EngineInstancePathNode)_engineInstance;}}

    /// <summary>
    /// overridden function to set/update all specific properties on the engine instance
    /// </summary>
    public override void SetEngineInstanceBaseProperties()
    {
      if (_engineInstance==null)
        return;

      base.SetEngineInstanceBaseProperties (); // position etc.
      UpdateEngineInstanceTangents();
      EnginePathNode.SetLinearIn(_inType==PathNodeInOutType.Linear);
      EnginePathNode.SetLinearOut(_outType==PathNodeInOutType.Linear);
      EnginePathNode.SetObjectKey(_objectKey);

      EngineInstancePathNode.SetNodeDisplayMode(_displayMode);
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
        EnginePathNode.RenderShape(view,mode);
    }

    /// <summary>
    /// Indicates whether the path is currently inside a cloning operation
    /// </summary>
    public static bool IsPathCloning = false;

    /// <summary>
    /// This shape type can not be copied or pasted
    /// </summary>
    public override bool CanCopyPaste { get { return IsPathCloning; } }

    /// <summary>
    /// This shape type cannot be dragged in the tree view
    /// </summary>
    /// <param name="newParent"></param>
    /// <returns></returns>
    public override bool AllowsToSetParent(ShapeBase newParent)
    {
      return false;
    }

    protected override Shape3D.ParentingModes ParentingMode
    {
      get
      {
        return Shape3D.ParentingModes.SceneGraphScaled;
      }
    }

    public override PropertyFlags_e GetPropertyFlags(PropertyFlagsProviderInfo pd)
    {
      if (_inType != PathNodeInOutType.Custom && pd.Name == "TangentIn")
        return PropertyFlags_e.Readonly;
      if (_outType != PathNodeInOutType.Custom && pd.Name == "TangentOut")
        return PropertyFlags_e.Readonly;
      return base.GetPropertyFlags(pd);
    }


    public override void OnAddedToScene()
    {
      base.OnAddedToScene();
      this.AddHint(HintFlags_e.NoScale);
    }

    #endregion

    #region ISerializable

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected PathNodeShape(SerializationInfo info, StreamingContext context) : base(info, context)
    {
      _tangentIn = (Vector3F)info.GetValue("_tangentIn",typeof(Vector3F));
      _tangentOut = (Vector3F)info.GetValue("_tangentOut",typeof(Vector3F));
      _inType = (PathNodeInOutType)info.GetValue("_inType",typeof(PathNodeInOutType));
      _outType = (PathNodeInOutType)info.GetValue("_outType",typeof(PathNodeInOutType));

      if (SerializationHelper.HasElement(info,"_displayMode"))
        _displayMode = (EngineInstancePathNode.PathNodeDisplayMode_e)info.GetValue("_displayMode",typeof(EngineInstancePathNode.PathNodeDisplayMode_e));
      if (SerializationHelper.HasElement(info, "_objectKey"))
        _objectKey = info.GetString("_objectKey");
    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context) 
    {
      base.GetObjectData(info,context);

      info.AddValue("_tangentIn",_tangentIn);
      info.AddValue("_tangentOut",_tangentOut);
      info.AddValue("_inType",_inType);
      info.AddValue("_outType",_outType);

      info.AddValue("_displayMode",_displayMode);
      info.AddValue("_objectKey", _objectKey);
    }

    #endregion

    #region Parent Path

    /// <summary>
    /// Returns the owner path shape
    /// </summary>
    [BrowsableAttribute(false)]
    public PathShape ParentPath {get {return (PathShape)this.Parent;}}

    #endregion

    #region Relevant operations

    static string RO_SHAPENAME_AS_KEY = @"Use Shape Name as Key";
    static string RO_CP_SMOOTH = @"Control Points - Smooth";
    static string RO_CP_BEZIER_IN = @"Control Points - Make Bezier in";
    static string RO_CP_BEZIER_OUT = @"Control Points - Make Bezier out";

    /// <summary>
    /// Get the relevant operations
    /// </summary>
    public override System.Collections.ArrayList RelevantOperations
    {
      get
      {
        ArrayList op = base.RelevantOperations;
        if (op==null) op=new ArrayList(4);
        op.Add(RO_SHAPENAME_AS_KEY);
        op.Add(RO_CP_SMOOTH);
        op.Add(RO_CP_BEZIER_IN);
        op.Add(RO_CP_BEZIER_OUT);
         return op;
      }
    }

    /// <summary>
    /// Perform the relevant operation
    /// </summary>
    /// <param name="name"></param>
    /// <param name="iShapeIndex"></param>
    /// <param name="iShapeCount"></param>
    public override void PerformRelevantOperation(string name, int iShapeIndex, int iShapeCount)
    {
      // Use shape name as key
      if (name == RO_SHAPENAME_AS_KEY)
      {
        // start a group
        if (iShapeIndex == 0) EditorManager.Actions.StartGroup("Name as Key");

        EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this, "ObjectKey", ShapeName));

        // close group action
        if (iShapeIndex == iShapeCount - 1) EditorManager.Actions.EndGroup();
        return;
      }

      // make the control vertex handles smooth
      if (name==RO_CP_SMOOTH)
      {
        if (iShapeIndex==0)
          EditorManager.Actions.StartGroup("Path node handles : Smooth");

        EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"TangentIn",this.SmoothInTangent));
        EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"TangentOut",this.SmoothOutTangent));

        if (iShapeIndex==iShapeCount-1)
          EditorManager.Actions.EndGroup();
          
        return;
      }

      // calculate bezier control vertex handles
      if (name == RO_CP_BEZIER_IN)
      {
        if (iShapeIndex==0)
          EditorManager.Actions.StartGroup("Path node handles : Bezier in");
        EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"TangentIn",this.BezierInTangent));
        if (iShapeIndex==iShapeCount-1)
          EditorManager.Actions.EndGroup();   
        return;
      }

      // calculate bezier control vertex handles
      if (name == RO_CP_BEZIER_OUT)
      {
        if (iShapeIndex==0)
          EditorManager.Actions.StartGroup("Path node handles : Bezier out");
        EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"TangentOut",this.BezierOutTangent));
        if (iShapeIndex==iShapeCount-1)
          EditorManager.Actions.EndGroup();   
        return;
      }

      base.PerformRelevantOperation (name, iShapeIndex, iShapeCount);
    }
    
    #endregion

    #region Selection and HotSpots

    /// <summary>
    /// Overridden clone function
    /// </summary>
    /// <returns></returns>
    public override ShapeBase Clone()
    {
      //return null; // cannot clone this
      PathNodeShape clone = (PathNodeShape)base.Clone ();
      clone._parentShape = this._parentShape;
      clone._hotSpotTangentIn = null;
      clone._hotSpotTangentOut = null;

      return clone;
    }

    HotSpot3D _hotSpotTangentIn = null;
    HotSpot3D _hotSpotTangentOut = null;

    /// <summary>
    /// Creates or removes the hotspots. Important when mode changes
    /// </summary>
    void UpdateTangentHotSpots()
    {
      bool bRemoveIn = true;
      bool bRemoveOut = true;
      if (Selected)
      {
        // custom is the only type that allows to edit the tangents via the hotspots
        bRemoveIn = (_inType!=PathNodeInOutType.Custom);
        bRemoveOut = (_outType!=PathNodeInOutType.Custom);
      }

      if (bRemoveIn && _hotSpotTangentIn!=null)
      {
        EditorManager.ActiveView.HotSpots.Remove(_hotSpotTangentIn);
        _hotSpotTangentIn = null;
      } 
      else
        if (!bRemoveIn && _hotSpotTangentIn==null)
      {
        _hotSpotTangentIn = new HotSpot3D(this,16.0f);
        _hotSpotTangentIn.ToolTipText = "tangent in";
        _hotSpotTangentIn.StartPosition = TransformToWorldSpace(_tangentIn, false);
        EditorManager.ActiveView.HotSpots.Add(_hotSpotTangentIn);
      }

      if (bRemoveOut && _hotSpotTangentOut!=null)
      {
        EditorManager.ActiveView.HotSpots.Remove(_hotSpotTangentOut);
        _hotSpotTangentOut = null;
      } 
      else
        if (!bRemoveOut && _hotSpotTangentOut==null)
      {
        _hotSpotTangentOut = new HotSpot3D(this,16.0f);
        _hotSpotTangentOut.ToolTipText = "tangent out";
        _hotSpotTangentOut.StartPosition = TransformToWorldSpace(_tangentOut, false);
        EditorManager.ActiveView.HotSpots.Add(_hotSpotTangentOut);
      }
    }

    /// <summary>
    /// Overridden OnSelected function
    /// </summary>
    public override void OnSelected()
    {
      base.OnSelected ();
      UpdateTangentHotSpots();
      this.ParentPath.CreateInsertNodeHotSpots();
    }

    /// <summary>
    /// Overridden OnUnSelected function
    /// </summary>
    public override void OnUnSelected()
    {
      base.OnUnSelected ();
      UpdateTangentHotSpots();

      if (ParentPath!=null && !ParentPath.Selected)
        this.ParentPath.RemoveInsertNodeHotSpots();
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragBegin(HotSpotBase hotSpot, VisionViewBase view)
    {
      base.OnHotSpotDragBegin (hotSpot, view);
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    /// <param name="fDeltaX"></param>
    /// <param name="fDeltaY"></param>
    public override void OnHotSpotDrag(HotSpotBase hotSpot, VisionViewBase view, float fDeltaX, float fDeltaY)
    {
      base.OnHotSpotDrag (hotSpot, view, fDeltaX, fDeltaY);
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragEnd(HotSpotBase hotSpot, VisionViewBase view)
    {
      base.OnHotSpotDragEnd (hotSpot, view);
      if (hotSpot==_hotSpotTangentIn)
      {
        Vector3F vNewPos = TransformToLocalSpace(_hotSpotTangentIn.CurrentPosition,false);
        _tangentIn = TransformToLocalSpace(_hotSpotTangentIn.StartPosition,false); // set old value for the action
        EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"TangentIn",vNewPos)); // send an action which sets the property from old value to new one
      } 
      else
        if (hotSpot==_hotSpotTangentOut)
      {
        Vector3F vNewPos = TransformToLocalSpace(_hotSpotTangentOut.CurrentPosition,false);
        _tangentOut = TransformToLocalSpace(_hotSpotTangentOut.StartPosition,false); // set old value for the action
        EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"TangentOut",vNewPos)); // send an action which sets the property from old value to new one
      }
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="hotSpot"></param>
    public override void OnHotSpotEvaluatePosition(HotSpotBase hotSpot)
    {
      base.OnHotSpotEvaluatePosition (hotSpot);

      if (hotSpot==_hotSpotTangentIn)
      {
        if (_hotSpotTangentIn.IsDragging) // update while dragging
          TangentIn = TransformToLocalSpace(_hotSpotTangentIn.CurrentPosition, false);
        else
          _hotSpotTangentIn.StartPosition = TransformToWorldSpace(_tangentIn, false);
        return;
      }
      if (hotSpot==_hotSpotTangentOut)
      {
        if (_hotSpotTangentOut.IsDragging) // update while dragging
          TangentOut = TransformToLocalSpace(_hotSpotTangentOut.CurrentPosition, false);
        else
          _hotSpotTangentOut.StartPosition = TransformToWorldSpace(_tangentOut, false);
        return;
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
      if (hotSpot==_hotSpotTangentIn)
      {
        view.RenderLine(Position,Position+_hotSpotTangentIn.CurrentPosition, VisionColors.Grey, 2.0f);
        return;
      }
      if (hotSpot==_hotSpotTangentOut)
      {
        view.RenderLine(Position,Position+_hotSpotTangentOut.CurrentPosition, VisionColors.Grey, 2.0f);
        return;
      }
    }

    #endregion

    #region Path node properties

    Vector3F _tangentIn = 5.0f*Vector3F.YAxis;
    Vector3F _tangentOut = -50.0f*Vector3F.YAxis;
    PathNodeInOutType _inType = PathNodeInOutType.Smooth;
    PathNodeInOutType _outType = PathNodeInOutType.Smooth;
    static EngineInstancePathNode.PathNodeDisplayMode_e _displayMode = EngineInstancePathNode.PathNodeDisplayMode_e.Unscaled;
    protected string _objectKey = null;

    #region Object key

    /// <summary>
    /// Gets or sets the object key, so the object can be searched in the engine
    /// </summary>
    [Description("Arbitrary key string to identify this instance inside the game application")]
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

    /// <summary>
    /// Internal value, do not use (use PathNodeIndex property instead)
    /// </summary>
    public int _iPathNodeIndex = -1; // temp value, gets set by the parent path

    /// <summary>
    /// Returns the index of this path node inside the parent's path
    /// </summary>
    [BrowsableAttribute(false)]
    public int PathNodeIndex
    {
      get 
      {
        PathNodeShape[] arr = ParentPath.PathNodes; // ensure the index is calculated
        System.Diagnostics.Debug.Assert(_iPathNodeIndex>=0); // otherwise this node does not belong to the parent
        return _iPathNodeIndex;
      }
    }

    /// <summary>
    /// Sets the tangent in-type to bezier (or linear)
    /// </summary>
    [SortedCategory(CAT_TANGENTS, CATORDER_TANGENTS), PropertyOrder(1),
    Description("Type of in-tangent (custom for editable tangents)")]
    [DependentProperties(new string[] { "TangentIn" })]
    public PathNodeInOutType InType
    {
      get { return _inType; }
      set
      {
        _inType = value;
        if (ParentPath != null)
          ParentPath.OnSingleNodeChangedPosition(this);
        if (_engineInstance != null)
        {
          EnginePathNode.SetLinearIn(_inType == PathNodeInOutType.Linear);
          UpdateTangentHotSpots();
        }
      }
    }

    /// <summary>
    /// Sets the tangent in-type to bezier (or linear)
    /// </summary>
    [SortedCategory(CAT_TANGENTS, CATORDER_TANGENTS), PropertyOrder(2),
    Description("Type of in-tangent (custom for editable tangents)")]
    [DependentProperties(new string[] { "TangentOut" })]
    public PathNodeInOutType OutType
    {
      get {return _outType;}
      set 
      {
        _outType=value;
        if (ParentPath!=null)
          ParentPath.OnSingleNodeChangedPosition(this);
        if (_engineInstance!=null)
        {
          EnginePathNode.SetLinearOut(_outType==PathNodeInOutType.Linear);
          UpdateTangentHotSpots();
        }
      }
    }


    /// <summary>
    /// The tangent-in vector (node local space)
    /// </summary>
    [SortedCategory(CAT_TANGENTS, CATORDER_TANGENTS), PropertyOrder(3),
    Description("In tangent vector")]
    public Vector3F TangentIn
    {
      get {return _tangentIn;}
      set 
      {
        _tangentIn = value;
        UpdateEngineInstanceTangents();
      }
    }

    /// <summary>
    /// The tangent-out vector (node local space)
    /// </summary>
    [SortedCategory(CAT_TANGENTS, CATORDER_TANGENTS), PropertyOrder(4),
    Description("Out tangent vector")]
    public Vector3F TangentOut
    {
      get {return _tangentOut;}
      set 
      {
        _tangentOut = value;
        UpdateEngineInstanceTangents();

      }
    }


    /// <summary>
    /// Editor display mode of path nodes (valid for all nodes)
    /// </summary>
    [SortedCategory(CAT_DISPLAY, CATORDER_DISPLAY), PropertyOrder(1),
    Description("Editor display mode of path nodes (valid for all nodes)")]
    public EngineInstancePathNode.PathNodeDisplayMode_e DisplayMode
    {
      get {return _displayMode;}
      set 
      {
        _displayMode = value;
        EngineInstancePathNode.SetNodeDisplayMode(_displayMode);
      }
    }

    void UpdateEngineInstanceTangents()
    {
      if (_engineInstance == null)
        return;

      Vector3F posOut = this.OutPosition;
      Vector3F posIn = this.InPosition;
      EnginePathNode.SetTangentOut(posOut.X, posOut.Y, posOut.Z);
      EnginePathNode.SetTangentIn(posIn.X, posIn.Y, posIn.Z);
    }

    #endregion

    #region Path node update

    /// <summary>
    /// Updates the node's tangents because it changed position or a neighbour changed position
    /// </summary>
    public void UpdateNode()
    {
      // the other modes (custom and linear) are not affected
      if (_inType==PathNodeInOutType.Smooth)
        this.TangentIn = this.SmoothInTangent;
      if (_outType==PathNodeInOutType.Smooth)
        this.TangentOut = this.SmoothOutTangent;
      this.UpdateEngineInstanceTangents();
    }

    /// <summary>
    /// Overridden function to notify the parent path
    /// </summary>
    public override void OnTransformationChanged()
    {
      base.OnTransformationChanged();
      if (ParentPath!=null)
        ParentPath.OnSingleNodeChangedPosition(this);
    }

    #endregion

    #region Neighbour Nodes

    /// <summary>
    /// Returns the previous node in the path
    /// </summary>
    [BrowsableAttribute(false)]
    public PathNodeShape PreviousNode
    {
      get 
      {
        PathNodeShape[] nodes = ParentPath.PathNodes;
        int iThisIndex = PathNodeIndex;
        if (iThisIndex>0)
          return nodes[iThisIndex-1];
        if (ParentPath.Closed)
          return nodes[nodes.Length-1];
        return null;
      }
    }

    /// <summary>
    /// Returns the next node in the path
    /// </summary>
    [BrowsableAttribute(false)]
    public PathNodeShape NextNode
    {
      get 
      {
        PathNodeShape[] nodes = ParentPath.PathNodes;
        int iThisIndex = PathNodeIndex;
        if (iThisIndex<nodes.Length-1)
          return nodes[iThisIndex+1];
        if (ParentPath.Closed)
          return nodes[0];
        return null;
      }
    }

    protected Vector3F TransformToWorldSpace(Vector3F localPos, bool bAddTranslation)
    {
      PathShape path = ParentPath;
      if (path == null)
        return RenderPosition + localPos;

      Vector3F lp = new Vector3F(localPos.X*path.ScaleX, localPos.Y*path.ScaleY, localPos.Z*path.ScaleZ);
      Matrix3F pathRotation = this.RotationMatrix;
      if (bAddTranslation)
        return RenderPosition + pathRotation * lp;
      return pathRotation * lp;
    }

    protected Vector3F TransformToLocalSpace(Vector3F wsPos, bool bHasTranslation)
    {
      if (bHasTranslation)
        wsPos -= RenderPosition;
      PathShape path = ParentPath;
      if (path == null || path.ScaleX == 0.0f || path.ScaleY == 0.0f || path.ScaleZ == 0.0f)
        return wsPos;

      Vector3F lp = new Vector3F(wsPos.X / path.ScaleX, wsPos.Y / path.ScaleY, wsPos.Z / path.ScaleZ);
      Matrix3F invRotation = Matrix3F.Transpose(this.RotationMatrix);
      return invRotation * lp;
    }

    /// <summary>
    /// Return absolute position of the in-control point
    /// </summary>
    [BrowsableAttribute(false)]
    public Vector3F InPosition
    {
      get { return TransformToWorldSpace(TangentIn, true); }
    }

    /// <summary>
    /// Return absolute position of the out-control point
    /// </summary>
    [BrowsableAttribute(false)]
    public Vector3F OutPosition
    {
      get { return TransformToWorldSpace(TangentOut, true); }
    }

    /// <summary>
    /// Calculates the new tangent in vector for smooth mode
    /// </summary>
    [BrowsableAttribute(false)]
    public Vector3F SmoothInTangent
    {
      get {return -SmoothOutTangent;}
    }

    /// <summary>
    /// Calculates the new tangent out vector for smooth mode
    /// </summary>
    [BrowsableAttribute(false)]
    public Vector3F SmoothOutTangent
    {
      get
      {
        if (EditorManager.Scene == null)
        {
          // EditorManager.Scene is required to resolve the RenderPosition property
          // Return zero vector in case it's not available: Should only happen for prefab deserialization and will be fixed up during engine instance creation
          return new Vector3F(0, 0, 0);
        }

        PathNodeShape prevNode = this.PreviousNode;
        PathNodeShape nextNode = this.NextNode;
        if (prevNode==null) prevNode=this;
        if (nextNode==null) nextNode=this;
        Vector3F v1 = RenderPosition - prevNode.RenderPosition;
        Vector3F v2 = nextNode.RenderPosition - RenderPosition;
        Vector3F d = (v1 + v2) / 6.0f;

        // d this is world space, so make it local space
        return TransformToLocalSpace(d, false);
      }
    }

    /// <summary>
    /// Calculates the new tangent in vector for bezier mode
    /// </summary>
    [BrowsableAttribute(false)]
    public Vector3F BezierInTangent
    {
      get
      {
        const float fEpsilon = 0.0001f;
        float outLen = TangentOut.GetLength();
        if (Math.Abs(outLen) > fEpsilon)
          return TransformToLocalSpace((RenderPosition - OutPosition) * ((InPosition - RenderPosition).GetLength() / outLen), false);
        return Vector3F.Zero;
      }
    }

    /// <summary>
    /// Calculates the new tangent out vector for bezier mode
    /// </summary>
    [BrowsableAttribute(false)]
    public Vector3F BezierOutTangent
    {
      get
      {
        const float fEpsilon = 0.0001f;
        float inLen = TangentIn.GetLength();
        if (Math.Abs(inLen) > fEpsilon)
          return TransformToLocalSpace((RenderPosition - InPosition) * ((OutPosition - RenderPosition).GetLength() / inLen), false);
        return Vector3F.Zero;
      }
    }

    /// <summary>
    /// Smooth the in/out tangent (without going through an action)
    /// </summary>
    public void SmoothTangents()
    {
      TangentIn = this.SmoothInTangent;
      TangentOut = this.SmoothOutTangent;
    }

    #endregion
  }

  #endregion

  #region Class PathTimePicker

  public class PathTimePicker : IPickHandler
  {
    public PathTimePicker(PathShape pathshape)
    {
      _pathShape = pathshape;
    }

    PathShape _pathShape;

    public override bool OnPick(System.Windows.Forms.MouseEventArgs e, CSharpFramework.Contexts.KeyModifier eKeyMod)
    {
      float fT = _pathShape.EnginePath.GetPathTimeAtScreenPos((float)e.X, (float)e.Y, 0.0f, 1.0f, 3);
      EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(_pathShape,"ShowPathTimeHelper",fT));
      return base.OnPick(e, eKeyMod);
    }

    public override string ScreenMessage
    {
      get
      {
        return "Click close to the path to evaluate the path time parameter";
      }
    }
  }


  #endregion


  #region Creator Plugin

  #region Path Shape

  class PathShapeCreator : CSharpFramework.IShapeCreatorPlugin
  {
    public PathShapeCreator()
    {
      // node icon
      _nodeIconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\pin_green.bmp"), Color.Magenta);

      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\path.bmp"), Color.Magenta);
    }

    /// <summary>
    /// Get the name of the creator
    /// </summary>
    /// <returns></returns>
    public override string GetPluginName()
    {
      return "Path (Line)";
    }

    /// <summary>
    /// Get the category's name
    /// </summary>
    /// <returns></returns>
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
      return "Adds a bezier spline path to the scene.\n"+
        "The path can have an arbitrary number of nodes and can be used for AI or cinematics.\n"+
        "Programmers can identify a path by its key in the application.";
    }

    /// <summary>
    /// Overridden function to create the shape instance
    /// </summary>
    /// <returns></returns>
    public override ShapeBase CreateShapeInstance()
    {
      PathShape shape = new PathShape("Path");
      shape.Position = EditorManager.Scene.CurrentShapeSpawnPosition;

      PathNodeShape node;

      for (int i=0;i<4;i++)
      {
        node = new PathNodeShape();
        node.Position = shape.Position;
        node.y += ((float)i - 1.5f) * 200.0f * EditorManager.Settings.GlobalUnitScaling;
        node.ShapeName = "node" + i;
        node.SetParent(shape);
      }
      shape.SmoothAll(false);
      return shape;
    }

    int _nodeIconIndex = -1;

    /// <summary>
    /// Returns the icon index for path nodes
    /// </summary>
    [Browsable(false)]
    public int PathNodeIconIndex
    {
      get {return _nodeIconIndex;}
    }
  }

  #endregion

  #region Circle Shape

  class CircleShapeCreator : CSharpFramework.IShapeCreatorPlugin
  {
    public CircleShapeCreator()
    {
      // node icon
      _nodeIconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\pin_green.bmp"), Color.Magenta);

      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\path.bmp"), Color.Magenta);
    }

    /// <summary>
    /// Get the name of the creator
    /// </summary>
    /// <returns></returns>
    public override string GetPluginName()
    {
      return "Path (Circle)";
    }

    /// <summary>
    /// Get the category's name
    /// </summary>
    /// <returns></returns>
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
      return "Adds a bezier spline path to the scene.\n" +
        "The path can have an arbitrary number of nodes and can be used for AI or cinematics.\n" +
        "Programmers can identify a path by its key in the application.";
    }

    /// <summary>
    /// Overridden function to create the shape instance
    /// </summary>
    /// <returns></returns>
    public override ShapeBase CreateShapeInstance()
    {
      PathShape shape = new PathShape("Circle Path");
      shape.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
      shape.Closed = true;

      // Magic number: http://www.tinaja.com/glib/ellipse4.pdf
      float fMagic = 0.551784f;
      float fRadius = 200.0f * EditorManager.Settings.GlobalUnitScaling;
      
      Vector3F rotCenter = shape.Position;
      Vector3F rotAngle = new Vector3F(0.0f, 0.0f, 0.0f);
      Matrix3F rotMatrix = new Matrix3F();
      rotMatrix.FromEuler(rotAngle.X, rotAngle.Y, rotAngle.Z);
      PathNodeShape node;

      for (int i = 0; i < 4; i++)
      {
        node = new PathNodeShape();
        node.Position = shape.Position;
        node.y -= fRadius;
        
        node.TangentIn = new Vector3F(node.TangentIn.X - fRadius * fMagic, 0.0f, 0.0f);
        node.TangentOut = new Vector3F(node.TangentOut.X + fRadius * fMagic, 0.0f, 0.0f);   

        node.Position = shape.Position + rotMatrix * (node.Position - rotCenter);
        node.TangentIn = rotMatrix * (node.TangentIn);
        node.TangentOut = rotMatrix * (node.TangentOut);

        node.InType = PathNodeShape.PathNodeInOutType.Custom;
        node.OutType = PathNodeShape.PathNodeInOutType.Custom;

        node.ShapeName = "node" + i;
        
        node.SetParent(shape);
        
        rotAngle.X += 90.0f;
        rotMatrix.FromEuler(rotAngle.X, rotAngle.Y, rotAngle.Z);
      }      

      return shape;
    }

    int _nodeIconIndex = -1;

    /// <summary>
    /// Returns the icon index for path nodes
    /// </summary>
    [Browsable(false)]
    public int PathNodeIconIndex
    {
      get { return _nodeIconIndex; }
    }
  }

  #endregion

  #endregion

  #region Action - Recenter Pivot Action

  /// <summary>
  /// Action that recenter the pivot of the path
  /// </summary>
  [Serializable]
  public class RecenterPivotAction : GroupAction
  {

    public RecenterPivotAction(PathShape pathShape) : base(null)
    {
      Vector3F oldPathPos = pathShape.Position.Clone();
      Vector3F newPathPos = new Vector3F();

      // Calculate new position (center of all path nodes)
      for (int i = 1; i < pathShape.PathNodeCount; i++)
        newPathPos = newPathPos + pathShape.PathNodes[i].Position;
      newPathPos = newPathPos / (pathShape.PathNodeCount - 1);

      // Reposition path nodes
      for (int i = 1; i < pathShape.PathNodeCount; i++)
      {
        Vector3F oldPos = pathShape.PathNodes[i].Position.Clone();
        Vector3F newPos = (oldPathPos - newPathPos) + oldPos;
        this.Add(new MoveShapeAction(pathShape.PathNodes[i], oldPos, newPos));
      }

      // Reposition path shape
      this.Add(new MoveShapeAction(pathShape, oldPathPos, newPathPos));

      // Action name
      GroupName = "Recenter Path Pivot";
    }

    #region ISerializable Members

    protected RecenterPivotAction(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
    }

    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);
    }

    #endregion

    #region Icon

    /// <summary>
    /// get the icon
    /// </summary>
    static int iImageIcon = EditorManager.GUI.ActionImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Actions\MoveShape.bmp"), System.Drawing.Color.Magenta);

    /// <summary>
    /// Get the layer icon that is also used in the layer list view
    /// </summary>
    public override int IconIndex { get { return iImageIcon; } }


    #endregion
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
