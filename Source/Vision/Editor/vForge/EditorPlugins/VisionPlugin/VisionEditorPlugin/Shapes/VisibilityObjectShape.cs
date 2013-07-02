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
using System.Collections;
using CSharpFramework.Visitors;



namespace VisionEditorPlugin.Shapes
{

  #region Linking


  #region Visibility link source

  /// <summary>
  /// Link source class for a visibility object
  /// </summary>
  [Serializable]
  public class LinkSourceVisiblity : LinkSourceObject3D
  {
    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="ownerShape"></param>
    /// <param name="primaryStringId"></param>
    public LinkSourceVisiblity(Shape3D ownerShape, string primaryStringId) : base(ownerShape,"affects",primaryStringId)
    {
    }

    /// <summary>
    /// Default constructor used by the factory class
    /// </summary>
    /// <param name="ownerShape"></param>
    /// <param name="linkName"></param>
    /// <param name="primaryStringId"></param>
    public LinkSourceVisiblity(Shape3D ownerShape, string linkName, string primaryStringId)
      : base(ownerShape, linkName, primaryStringId)
    {
    }

    #region ISerializable Members

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected LinkSourceVisiblity(SerializationInfo info, StreamingContext context) : base(info, context)
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
      get {return @"Bitmaps\Shapes\find.png";} // constraint  icon
    }

    #endregion
  }

  #endregion

  #region Visibility link target

  /// <summary>
  /// Link target class for constraint target
  /// </summary>
  [Serializable]
  public class LinkTargetVisiblity : LinkTargetObject3D
  {
    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="ownerShape"></param>
    /// <param name="primaryStringId"></param>
    public LinkTargetVisiblity(Shape3D ownerShape, string primaryStringId) : base(ownerShape,"visibility determined by",primaryStringId)
    {
    }

    /// <summary>
    /// Default constructor used by the factory class
    /// </summary>
    /// <param name="ownerShape"></param>
    /// <param name="linkName"></param>
    /// <param name="primaryStringId"></param>
    public LinkTargetVisiblity(Shape3D ownerShape, string linkName, string primaryStringId)
      : base(ownerShape, linkName, primaryStringId)
    {
    }

    #region ISerializable Members

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected LinkTargetVisiblity(SerializationInfo info, StreamingContext context) : base(info, context)
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
      get {return @"Bitmaps\Shapes\find.png";} // constraint icon
    }

    #endregion

    #endregion

  }

  #endregion
  
  
  /// <summary>
  /// VisibilityObjectShape : corresponds to VisVisibilityObject_cl
  /// </summary>
  [Serializable, ShapeTypeNiceName("Visibility Object")]
  public class VisibilityObjectShape : ShapeObject3D
	{

    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_BOXSIZE      = "Box Size";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_BOXSIZE    = Shape3D.LAST_CATEGORY_ORDER_ID + 1;

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_TESTFLAGS      = "Visibility Test Flags";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_TESTFLAGS    = Shape3D.LAST_CATEGORY_ORDER_ID + 2;

    /// <summary>
    /// Last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_TESTFLAGS;

    #endregion

    #region constructor 

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="nodeName"></param>
		public VisibilityObjectShape(string nodeName) : base(nodeName)
		{
      this._vBoxSize *= EditorManager.Settings.GlobalUnitScaling;
			AddHint(HintFlags_e.NoRotation);
		}

    #endregion

    #region GUI

    /// <summary>
    /// Overridden function to get the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetIconIndex()
    {
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\Find.bmp"), Color.Magenta);
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

      this._engineInstance = new EngineInstanceVisibilityObject();
      SetEngineInstanceBaseProperties(); // sets the position etc.
    }


    // accesses the engine instance as an object of type EngineInstanceVisibilityObject
    EngineInstanceVisibilityObject EngineBox {get {return (EngineInstanceVisibilityObject)_engineInstance;}}

    /// <summary>
    /// overridden function to set/update all specific properties on the engine instance
    /// </summary>
    public override void SetEngineInstanceBaseProperties()
    {
      if (_engineInstance==null)
        return;

      base.SetEngineInstanceBaseProperties ();
      UpdateBox();
      UpdateFlags();
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

    #region Visibility object properties

    Vector3F _vBoxSize = new Vector3F(100,100,100);
    bool _bFrustumTest=true;
    bool _bPortalTest=false;
    bool _bHOCQTest=false;
    float _fFarClipDist = -1.0f;
    float _fNearClipDist = -1.0f;
    bool _bActive=true;
    uint _iContextBitMask = 1;

    void UpdateBox()
    {
      if (_engineInstance==null) return;
      EngineBox.SetBoxDimensions(_vBoxSize.X * ScaleX, _vBoxSize.Y * ScaleY, _vBoxSize.Z * ScaleZ);
    }

    void UpdateFlags()
    {
      if (_engineInstance==null) return;
      EngineBox.SetTestFlags(_bFrustumTest, _bPortalTest, _bHOCQTest, _fNearClipDist, _fFarClipDist, _bActive);
      EngineBox.SetContextBitmask(_iContextBitMask);
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

    #endregion

    #region Test flags

    /// <summary>
    /// If enabled, this visibility object performs hardware occlusion test
    /// </summary>
    [SortedCategory(CAT_TESTFLAGS, CATORDER_TESTFLAGS),
    PropertyOrder(1),
    Description("If enabled, this visibility object actively performs visibility tests every frame")]
    public bool Active
    {
      get {return _bActive;}
      set { _bActive = value;UpdateFlags();}
    }

    /// <summary>
    /// If enabled, this visibility object performs a frustum test
    /// </summary>
    [SortedCategory(CAT_TESTFLAGS, CATORDER_TESTFLAGS),
    PropertyOrder(2),
    Description("If enabled, this visibility object performs a frustum test")]
    public bool FrustumTest
    {
      get {return _bFrustumTest;}
      set { _bFrustumTest= value;UpdateFlags();}
    }

    /// <summary>
    /// If enabled, this visibility object performs portal tests
    /// </summary>
    [SortedCategory(CAT_TESTFLAGS, CATORDER_TESTFLAGS),
    PropertyOrder(3),
    Description("If enabled, this visibility object performs portal tests")]
    public bool PortalTest
    {
      get {return _bPortalTest;}
      set { _bPortalTest= value;UpdateFlags();}
    }

    /// <summary>
    /// If enabled, this visibility object performs hardware occlusion test
    /// </summary>
    [SortedCategory(CAT_TESTFLAGS, CATORDER_TESTFLAGS),
    PropertyOrder(4),
    Description("If enabled, this visibility object performs hardware occlusion test")]
    public bool HardwareOcclusionTest
    {
      get {return _bHOCQTest;}
      set { _bHOCQTest = value;UpdateFlags();}
    }

    /// <summary>
    /// Determines the distance to camera at which this visibility object is clipped. A value of smaller than zero will not clip the object at distance.
    /// </summary>
    [SortedCategory(CAT_TESTFLAGS, CATORDER_TESTFLAGS),
    PropertyOrder(5),
    Description("Determines the near distance to camera at which this visibility object is clipped. A value of <=0 will not clip the object at distance.")]
    public float NearclipDistance
    {
      get { return _fNearClipDist; }
      set { _fNearClipDist = value; UpdateFlags(); }
    }

    /// <summary>
    /// Determines the distance to camera at which this visibility object is clipped. A value of smaller than zero will not clip the object at distance.
    /// </summary>
    [SortedCategory(CAT_TESTFLAGS, CATORDER_TESTFLAGS),
    PropertyOrder(6),
    Description("Determines the far distance to camera at which this visibility object is clipped. A value of <=0 will not clip the object at distance.")]
    public float FarclipDistance
    {
      get {return _fFarClipDist;}
      set { _fFarClipDist = value;UpdateFlags();}
    }

    /// <summary>
    /// Set the bitmask that is used to dtermine in which render context this object is active, e.g. 1 for main view.
    /// </summary>
    [SortedCategory(CAT_TESTFLAGS, CATORDER_TESTFLAGS),
    PropertyOrder(7),
    Description("This bitmask is compared against the render context's filter bitmask to determine whether this visibilty object is visible in specific contexts.")]
    public FlagsInt32_e ContextBitmask
    {
      get {return (FlagsInt32_e)_iContextBitMask;}
      set { _iContextBitMask = (uint)value;UpdateFlags();}
    }


    #endregion

    #endregion
    
    #region ISerializable

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected VisibilityObjectShape(SerializationInfo info, StreamingContext context) : base(info, context)
    {
      _vBoxSize = (Vector3F)info.GetValue( "_vBoxSize", typeof(Vector3F) );
      _bFrustumTest = info.GetBoolean("_bFrustumTest");
      _bPortalTest = info.GetBoolean("_bPortalTest");
      _bHOCQTest = info.GetBoolean("_bHOCQTest");
      _bActive = info.GetBoolean("_bActive");
      _fFarClipDist = info.GetSingle("_fFarClipDist");
      if (SerializationHelper.HasElement(info, "_fNearClipDist"))
        _fNearClipDist = info.GetSingle("_fNearClipDist");
      _iContextBitMask = info.GetUInt32("_iContextBitMask");
			AddHint(HintFlags_e.NoRotation);

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
      info.AddValue("_bFrustumTest",_bFrustumTest);
      info.AddValue("_bPortalTest",_bPortalTest);
      info.AddValue("_bHOCQTest",_bHOCQTest);
      info.AddValue("_fFarClipDist",_fFarClipDist);
      info.AddValue("_fNearClipDist", _fNearClipDist);
      info.AddValue("_bActive",_bActive);
      info.AddValue("_iContextBitMask",_iContextBitMask);
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
      VisibilityObjectShape copy = (VisibilityObjectShape)base.Clone();
      System.Diagnostics.Debug.Assert(!copy.Selected); // turned off in ShapeBase

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

    #region Linking

    public static string LINK_SOURCE_ID = "VisibilitySource_1";

    /// <summary>
    /// Create all link sources and targets provided by this shape
    /// </summary>
    public override void CreateAllLinks()
    {
      base.CreateAllLinks ();
      LinkSources.Add(new LinkSourceVisiblity(this, LINK_SOURCE_ID));
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
        if (target is LinkTargetVisiblity) 
          return true;
        if (target is LinkTargetObject3D) // allow for linking with custom entity links
          return target.OwnerShape is EntityShape;
      }
      return false;
    }

    public LinkSourceVisiblity GetVisibilityLinkSource() 
    {
      return (LinkSourceVisiblity)LinkSources.GetLinkByPrimaryId(LINK_SOURCE_ID);
    }

    #endregion

    #region Relevant Operations

    static string RO_ADJUST_BBOX = "Adjust bounding box to linked objects";
    static string RO_LINK_TO_TOUCHING_SHAPES = "Link to all shapes that touch the bounding box";
    static string RO_LINK_TO_SHAPES_INSIDE = "Link to all shapes that are inside the bounding box";

    #region Nested class : EvaluateBBoxLinksVisitor

    public class EvaluateBBoxLinksVisitor : IShapeVisitor
    {
      public EvaluateBBoxLinksVisitor(VisibilityObjectShape visShape, bool bTouching)
      {
        _visShape = visShape;
        _srcLink = (LinkSource)_visShape.GetVisibilityLinkSource();
        _bTouching = bTouching;
        visBBox = _visShape.AbsoluteBoundingBox;
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
      VisibilityObjectShape _visShape;
      LinkSource _srcLink;
      BoundingBox visBBox;
      public LinkCollection RelevantTargets = new LinkCollection();
    }

    #endregion

    public override ArrayList RelevantOperations
    {
      get
      {
        ArrayList arr = base.RelevantOperations;
        if (arr == null)
          arr = new ArrayList(4);
        ShapeLink src = GetVisibilityLinkSource();
        if (src != null && src.Links.Count > 0)
          arr.Add(RO_ADJUST_BBOX);

        arr.Add(RO_LINK_TO_TOUCHING_SHAPES);
        arr.Add(RO_LINK_TO_SHAPES_INSIDE);
        return arr;
      }
    }

    public override void PerformRelevantOperation(string name, int iShapeIndex, int iShapeCount)
    {
      base.PerformRelevantOperation(name, iShapeIndex, iShapeCount);


      if (name == RO_ADJUST_BBOX)
      {
        ShapeLink src = GetVisibilityLinkSource();
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
          LinkSource linkSrc = GetVisibilityLinkSource();
          EditorManager.Actions.StartGroup(name);
          foreach (LinkTarget target in visitor.RelevantTargets)
            EditorManager.Actions.Add(new LinkAction(linkSrc, target));

          EditorManager.Actions.EndGroup();
        }
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

  #region Visibility object Creator Plugin

  class VisibilityObjectCreator : CSharpFramework.IShapeCreatorPlugin
  {

    public VisibilityObjectCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\Find.bmp"), Color.Magenta);
      CategoryIconIndex = IconIndex;
    }

    public override string GetPluginName()
    {
      return "Visibility Object";
    }

    public override string GetPluginCategory()
    {
      return "Visibility";
    }

    /// <summary>
    /// Returns a short description text
    /// </summary>
    /// <returns></returns>
    public override string GetPluginDescription()
    {
      return "Adds an engine visibility object box to the scene.\n"+
        "Some shapes (such as the mirror shape) use a list of visibility bounding boxes to determine their own visible status.\n"+
        "Visibility objects can be linked to these shapes.";
    }

    public override ShapeBase CreateShapeInstance()
    {
      VisibilityObjectShape box = new VisibilityObjectShape("Visibility object");
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
