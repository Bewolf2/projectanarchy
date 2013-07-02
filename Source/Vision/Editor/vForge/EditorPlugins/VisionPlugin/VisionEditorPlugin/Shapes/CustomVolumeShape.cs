/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.IO;
using System.Drawing;
using System.Diagnostics;
using System.Runtime.Serialization;
using System.ComponentModel;
using System.Windows.Forms;
using System.Collections;
using System.Drawing.Design;

using CSharpFramework.Shapes;
using CSharpFramework.Serialization;
using CSharpFramework;
using CSharpFramework.Math;
using CSharpFramework.Contexts;
using CSharpFramework.Actions;
using CSharpFramework.View;
using CSharpFramework.PropertyEditors;
using CSharpFramework.Scene;

using VisionManaged;
using VisionEditorPlugin.Dialogs;
using System.Collections.Generic;



namespace VisionEditorPlugin.Shapes
{
  #region Add vertex hotspot button

  class HotSpotAddVolumeVertex : HotSpotPushButton
  {
    /// <summary>
    /// constructor
    /// </summary>
    /// <param name="owner">the owning CustomVolumeShape</param>
    /// <param name="insert">at which place to insert</param>
    public HotSpotAddVolumeVertex( CustomVolumeShape owner, int insert)
      : base(owner, @"Textures\Hotspot_add.dds", VisionColors.RGBA(255, 255, 255, 180), VisionColors.White, 8.0f)
    {
      _insert = insert;
    }

    /// <summary>
    /// returns the owning CustomVolumeShape
    /// </summary>
    CustomVolumeShape Volume 
    { 
      get 
      { 
        return (CustomVolumeShape)Owner; 
      } 
    }

    /// <summary>
    /// overridden function that evaluates the position on the path
    /// </summary>
    public override void EvaluatePosition()
    {
      ShapeCollection vertices = Volume.ChildCollection;

      Shape3D vertex1;
      if (_insert == 0)
        vertex1 = (Shape3D)vertices[vertices.Count - 1];
      else
        vertex1 = (Shape3D)vertices[_insert - 1];
      Shape3D vertex2 = (Shape3D)vertices[_insert];
      Position = (vertex1.Position + vertex2.Position) * 0.5f;
    }

    /// <summary>
    /// Overridden function to insert a vertex
    /// </summary>
    /// <param name="view"></param>
    public override void OnClicked(VisionViewBase view)
    {
      base.OnClicked(view);

      CustomVolumeVertex newVertex = new CustomVolumeVertex();
      newVertex.ChildIndex = _insert;
      EditorManager.Actions.Add(AddShapeAction.CreateAddShapeAction(newVertex, Volume, Volume.ParentLayer, false));
      newVertex.Position = Position;

      EditorManager.ActiveView.Gizmo.SetSingleShape(newVertex, false);
    }

    int _insert;
  }

  #endregion

  #region Class : CustomVolumeShape

  [Serializable, ShapeTypeNiceName("Custom Volume")]
  class CustomVolumeShape : ShapeObject3D
  {
    bool _constructionFinished = false;
    float _height = 10.0f;
    string _staticMeshPath = "";
    bool _customStaticMesh = false;
    bool _selfSelected = false;
    bool _childSelected = false;
    bool _preview = false;
    bool _pickRendering = false;

    [Browsable(false)]
    public bool ConstructionFinished
    {
      set
      {
        _constructionFinished = value;
        if (value && EngineInstance != null)
          EngineInstance.OnCreationFinished();
      }
      get
      {
        return _constructionFinished;
      }
    }

    [Browsable(false)]
    public bool IsRendering
    {
      get { return _selfSelected || _childSelected || _preview || _pickRendering; }
    }

    #region Constructor

    /// <summary>
    /// Constructor
    /// </summary>
    public CustomVolumeShape()
      : this("volume")
    {
    }

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="name"></param>
    public CustomVolumeShape(string name)
      : this(name,true)
		{
		}

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="name"></param>
    /// <param name="constructionFinished"></param>
    public CustomVolumeShape(string name, bool constructionFinished)
      : base(name)
    {
      _selfSelected = !constructionFinished;
      _constructionFinished = constructionFinished;
    }

    protected override void DisposeObject()
    {
      base.DisposeObject();
    }
  
    #endregion

    #region ShapeBase Members

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

      // Deleting children in base.OnRemoveFromScene will select 'this' again in the EditorManager.
      // We can't have ourself selected as we are about to remove ourself.
      EditorManager.SelectedShapes = new ShapeCollection();

      //finally remove the engine instance
      if (this._engineInstance != null)
      {
        // Important: We must call the base implementation here because the overload in this class prevents the deletion
        base.RemoveEngineInstance(false);
      }
    }

    public override void OnTransformationChanged()
    {
      // Hide/Show hot spot
      if (_hotSpotHeight != null)
      {
        _hotSpotHeight.DisplayScaling = ScaleZ;
        _hotSpotHeight.Visible = (ScaleZ != 0);
      }

      base.OnTransformationChanged();
    }

    public override bool AllowsToAddChild(ShapeBase newChild)
    {
      if (!(newChild is CustomVolumeVertex))
      {
        return false;
      }
      return base.AllowsToAddChild(newChild);
    }

    #endregion

    #region ISerializable

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected CustomVolumeShape(SerializationInfo info, StreamingContext context) : base(info, context)
    {
      Height = (float)info.GetValue("Height", typeof(float));
      CustomStaticMesh = (bool)info.GetValue("CustomStaticMesh", typeof(bool));
      StaticMeshPath = (string)info.GetValue("StaticMeshPath", typeof(string));
      Preview = (bool)info.GetValue("Preview", typeof(bool));
      _constructionFinished = true;
    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context) 
    {
      base.GetObjectData(info,context);

      info.AddValue("Height", Height);
      info.AddValue("CustomStaticMesh", CustomStaticMesh);
      info.AddValue("StaticMeshPath", StaticMeshPath);
      info.AddValue("Preview", Preview);
    }

    #endregion

    #region Shape3D Overrides

    public override void CreateEngineInstance(bool bCreateChildren)
    {
      if (_engineInstance == null)
      {
        _engineInstance = new EngineInstanceCustomVolumeObject(this);
        SetEngineInstanceBaseProperties();

        if (ConstructionFinished)
        {
          if (!bCreateChildren)
          {
            foreach (Shape3D child in ChildCollection)
              child.CreateEngineInstance(false);
          }
          EngineInstance.OnCreationFinished();
        }
      }

      base.CreateEngineInstance(bCreateChildren);
    }

    public override void RemoveEngineInstance(bool bRemoveChildren)
    {
      // The editing context holds a reference to the shape, so we need to close it first (such as when undoing before finishing the shape)
      if (EditorManager.ActiveView.CurrentContext is AddVolumeVertexContext)
        EditorManager.ActiveView.SetDefaultContext();

      IEngineShapeInstance keepAlive = _engineInstance;
      _engineInstance = null;

      base.RemoveEngineInstance(bRemoveChildren);

      _engineInstance = keepAlive;
    }

    public override void SetEngineInstanceBaseProperties()
    {
      if (_engineInstance == null)
        return;

      base.SetEngineInstanceBaseProperties();
      _engineInstance.SetPosition(Position.X, Position.Y, Position.Z);
      _engineInstance.SetOrientation(Orientation.X, Orientation.Y, Orientation.Z);
      _engineInstance.SetScaling(ScaleX, ScaleY, ScaleZ);
      EngineInstance.SetHeight(Height);
      EngineInstance.SetCustomStaticMesh(_customStaticMesh);
      EngineInstance.SetStaticMeshPath(_staticMeshPath);
    }

    public override void RenderShape(VisionViewBase view, ShapeRenderMode mode)
    {
      base.RenderShape(view, mode);

      if(IsRendering && !CustomStaticMesh && EngineInstance != null)
        EngineInstance.RenderShape(view, mode);
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="child"></param>
    public override void AddChild(ShapeBase child)
    {
      base.AddChild(child);
      if (child is CustomVolumeVertex)
        Update();
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="child"></param>
    public override void RemoveChild(ShapeBase child)
    {
      base.RemoveChild(child);
      if (child is CustomVolumeVertex)
        Update();
    }


    /// <summary>
    /// Re-creates the engine instance
    /// </summary>
    public void Update()
    {
      if (_engineInstance == null)
        return;

      RemoveHotSpots();

      if (!CustomStaticMesh)
      {
        if (Selected && ConstructionFinished)
        {
          CreateHotSpots();
        }
      }
    }

    #endregion

    #region Callbacks

    void OnBeginPickShape(object sender, BeginPickShapeArgs e)
    {
      if (EditorManager.EngineManager.TypeManager_AllowShapeReference(e.OwnerObject, e.Property, this))
        _pickRendering = true;
    }

    void OnEndPickShape(object sender, EndPickShapeArgs e)
    {
      if (object.Equals(this, e.Shape))
      {
        // TODO: animate custom volume shape in some way
      }

      _pickRendering = false;
    }

    #endregion

    #region Helper functions

    /// <summary>
    /// accesses the engine instance as an object of type EngineInstanceCustomVolumeObject
    /// </summary>
    [BrowsableAttribute(false)]
    public EngineInstanceCustomVolumeObject EngineInstance { get { return (EngineInstanceCustomVolumeObject)_engineInstance; } }

    #endregion

    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_VOLUME = "Volume";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_VOLUME = Shape3D.LAST_CATEGORY_ORDER_ID + 1;


    /// <summary>
    /// last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_VOLUME;

    #endregion

    #region Properties
    [SortedCategory(CAT_VOLUME, CATORDER_VOLUME), PropertyOrder(1)]
    [Description("The height of the volume shape")]
    public float Height
    {
      get 
      {
        return _height;
      }
      set 
      {
        if (value <= 0.0f)
          _height = 1.0f;
        else
          _height = value;

        if (EngineInstance != null)
        {
          EngineInstance.SetHeight(_height);
        }
      }
    }

    [SortedCategory(CAT_VOLUME, CATORDER_VOLUME), PropertyOrder(4)]
    [Description("If the volume preview should be rendered always")]
    public bool Preview
    {
      get { return _preview; }
      set 
      { 
        _preview = value; 
        if(CustomStaticMesh && value)
          WantsNativeRenderHookCallback = true;
      }
    }

    [SortedCategory(CAT_VOLUME, CATORDER_VOLUME), PropertyOrder(3)]
    [Description("If the volume object should be using a custom .vmesh file or not")]
    public bool CustomStaticMesh
    {
      get { return _customStaticMesh; }
      set 
      {
        _customStaticMesh = value;
        if (EngineInstance != null)
          EngineInstance.SetCustomStaticMesh(value);
        if (ConstructionFinished && value && IsRendering)
          WantsNativeRenderHookCallback = true;
        if (ConstructionFinished && EngineInstance != null)
        {
          if (value)
            RemoveHotSpots();
          else
            CreateHotSpots();
        }
        if (value && StaticMeshPath != "" && EngineInstance != null)
          EngineInstance.SetStaticMeshPath(StaticMeshPath);
      }
    }

    [SortedCategory(CAT_VOLUME, CATORDER_VOLUME), PropertyOrder(4)]
    [Description("The static mesh which is used for the volume.")]
    [EditorAttribute(typeof(AssetEditor), typeof(UITypeEditor)), AssetDialogFilter(new string[] { "StaticMesh" }), RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    public string StaticMeshPath
    {
      get { return _staticMeshPath; }
      set 
      {
        _staticMeshPath = value;
        if (EngineInstance != null)
        {
          EngineInstance.SetStaticMeshPath(value);
          if (value != "")
          {
            string error = EngineInstance.CheckIsStaticMeshValid();
            if (error != null)
            {
              EditorManager.ShowMessageBox(error, "static mesh not suitable for volume");
              EngineInstance.SetStaticMeshPath("");
              _staticMeshPath = "";
            }
          }
        }
      }
    }
    #endregion

    public override PropertyFlags_e GetPropertyFlags(PropertyFlagsProviderInfo pd)
    {
      if (pd.Name == "StaticMeshPath")
      {
        if (CustomStaticMesh)
          return PropertyFlags_e.None;
        else
          return PropertyFlags_e.Hidden;
      }
      return base.GetPropertyFlags(pd);
    }

    #region Selection and HotSpots

    /// <summary>
    /// Overridden clone function
    /// </summary>
    /// <returns></returns>
    public override ShapeBase Clone()
    {
      CustomVolumeVertex.IsVolumeCloning = true;
      CustomVolumeShape clone = (CustomVolumeShape)base.Clone();
      CustomVolumeVertex.IsVolumeCloning = false;
      clone._hotSpotNewNodes = null;
      clone._hotSpotHeight = null;

      return clone;
    }


    List<HotSpotAddVolumeVertex> _hotSpotNewNodes = null;
    HotSpotDistance _hotSpotHeight = null;

    public void CreateHeightHotSpot()
    {
      // Create hotspot for height (but only if we have any vertices and scaling allows changing the height)
      if (_hotSpotHeight == null && ChildCollection.Count > 0 && ScaleZ != 0)
      {
        _hotSpotHeight = new HotSpotDistance(this, @"textures\Hotspot_H.tga", VisionColors.Yellow, HotSpotBase.PickType.Square, 10.0f);
        _hotSpotHeight.StartDistance = this.Height;
        _hotSpotHeight.DisplayScaling = ScaleZ;
        _hotSpotHeight.Axis = this.ZAxis;
        _hotSpotHeight.ToolTipText = "Volume height";
        EditorManager.ActiveView.HotSpots.Add(_hotSpotHeight);
      }
    }

    public void RemoveHeightHotSpot()
    {
      if (_hotSpotHeight != null)
      {
        EditorManager.ActiveView.HotSpots.Remove(_hotSpotHeight);
        _hotSpotHeight.Remove();
        _hotSpotHeight = null;
      }
    }

    /// <summary>
    /// Create all hotspot buttons that allow to add nodes
    /// </summary>
    public void CreateInsertNodeHotSpots()
    {
      if (_hotSpotNewNodes != null) // already created
        return;

      ShapeCollection vertices = this.ChildCollection;
      int iCount = vertices.Count;
      _hotSpotNewNodes = new List<HotSpotAddVolumeVertex>(iCount);
      HotSpotAddVolumeVertex hs;
      for (int i = 0; i < iCount; i++)
      {
        if (vertices[i] is CustomVolumeVertex)
        {
          // insert hot spots in between the nodes
          hs = new HotSpotAddVolumeVertex(this, i);
          EditorManager.ActiveView.HotSpots.Add(hs);
          _hotSpotNewNodes.Add(hs);
        }
      }
    }

    /// <summary>
    /// Remove all hotspot buttons that allow to add nodes
    /// </summary>
    public void RemoveInsertNodeHotSpots()
    {
      if (_hotSpotNewNodes != null)
      {
        foreach (HotSpotAddVolumeVertex hs in _hotSpotNewNodes)
          EditorManager.ActiveView.HotSpots.Remove(hs);
        _hotSpotNewNodes = null;
      }
    }

    public void CreateHotSpots()
    {
      CreateHeightHotSpot();
      CreateInsertNodeHotSpots();
    }

    public void RemoveHotSpots()
    {
      RemoveHeightHotSpot();
      RemoveInsertNodeHotSpots();
    }

		/// <summary>
		/// Called when user starts dragging a hotspot that belongs to this shape
		/// </summary>
		/// <param name="hotSpot"></param>
		/// <param name="view"></param>
		public override void OnHotSpotDragBegin(HotSpotBase hotSpot, VisionViewBase view)
		{
      if (hotSpot == _hotSpotHeight)
        _hotSpotHeight.StartDistance = Height;
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
			if (hotSpot==_hotSpotHeight)
        Height = _hotSpotHeight.CurrentDistance; // update for the view
		}

		/// <summary>
		/// Called when the user releases the mouse button on a hotspot
		/// </summary>
		/// <param name="hotSpot"></param>
		/// <param name="view"></param>
		public override void OnHotSpotDragEnd(HotSpotBase hotSpot, VisionViewBase view)
		{
      if (hotSpot == _hotSpotHeight)
			{
        if (_hotSpotHeight.HasChanged)
				{
          float fNewHeight = _hotSpotHeight.CurrentDistance;
          Height = _hotSpotHeight.StartDistance; // set old value for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this, "Height", fNewHeight)); // send an action which sets the property from old value to new one
				}
			}
		}

		/// <summary>
		/// Called every frame and per hotspot that belongs to the shape
		/// </summary>
		/// <param name="hotSpot"></param>
		public override void OnHotSpotEvaluatePosition(HotSpotBase hotSpot)
		{
			if (hotSpot==_hotSpotHeight)
			{
				if (!_hotSpotHeight.IsDragging)
          _hotSpotHeight.StartDistance = Height;
      }
		}

    /// <summary>
    /// Called every frame and per hotspot that belongs to the shape
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    /// <param name="mode"></param>
    public override void OnHotSpotRender(HotSpotBase hotSpot, VisionViewBase view, ShapeRenderMode mode)
    {
      base.OnHotSpotRender(hotSpot, view, mode);

      if (_hotSpotHeight.Visible)
      {
        Vector3F vStart = Position;
        Vector3F vEnd = _hotSpotHeight.Position;

        view.RenderLine(vStart, vEnd, VisionColors.Yellow, 1.0f);
      }
    }

    /// <summary>
    /// Overridden OnSelected function
    /// </summary>
    public override void OnSelected()
    {
      base.OnSelected();
      if (ConstructionFinished)
      {
        if (CustomStaticMesh)
        {
          WantsNativeRenderHookCallback = true;
        }
        else
        {
          CreateHotSpots();
        }
        _selfSelected = true;
      }
    }

    /// <summary>
    /// Overridden OnUnSelected function
    /// </summary>
    public override void OnUnSelected()
    {
      base.OnUnSelected();
      if (ConstructionFinished)
      {
        RemoveHotSpots();
        if (CustomStaticMesh && !_preview)
        {
          WantsNativeRenderHookCallback = false;
        }
        _selfSelected = false;
      }
    }

    public void OnChildSelected()
    {
      if (ConstructionFinished)
      {
        if (CustomStaticMesh)
        {
          WantsNativeRenderHookCallback = true;
        }
        else
        {
          CreateHotSpots();
        }
        _childSelected = true;
      }
    }

    public void OnChildUnSelected()
    {
      if (ConstructionFinished)
      {
        RemoveHotSpots();
        if (CustomStaticMesh && !_preview)
        {
          WantsNativeRenderHookCallback = false;
        }
        _childSelected = false;
      }
    }

    #endregion

    #region GUI

    /// <summary>
    /// Overridden function to get the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetIconIndex()
    {
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\cube_molecule.png"), Color.Black);
    }

    #endregion

    #region Tracing

    public override void OnTraceShape(ShapeTraceMode_e mode, Vector3F rayStart, Vector3F rayEnd, ref ShapeTraceResult result)
    {
      if (mode == ShapeTraceMode_e.ShapePicking && _engineInstance != null)
        _engineInstance.TraceShape(this, rayStart, rayEnd, ref result);
    }

    #endregion
  }

  #endregion

  #region Class : CustomVolumeVertex

  [Serializable]
  class CustomVolumeVertex : Shape3D
  {
    private CustomVolumeShape _lastParent;

    #region constructor

    public CustomVolumeVertex() 
      : this("vertex")
    {
    }

    public CustomVolumeVertex(string name)
      : base(name)
    {
      SetHint(HintFlags_e.NoScale, true);
      SetHint(HintFlags_e.NoRotation, true);
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

    #region Shape3D Overrides

    public override void CreateEngineInstance(bool bCreateChildren)
    {
      base.CreateEngineInstance(bCreateChildren);

      _lastParent = (CustomVolumeShape)Parent;

      if (_engineInstance == null)
      {
        Debug.Assert(Parent != null, "Need a parent to create the engine instance");
        _engineInstance = new EngineInstanceCustomVolumeVertex((Shape3D)Parent);
        _engineInstance.SetPosition(Position.X, Position.Y, Position.Z);
      }
    }

    public override bool AllowsToAddChild(ShapeBase newChild)
    {
      return false;
    }

    public override void RemoveEngineInstance(bool bRemoveChildren)
    {
      IEngineShapeInstance keepAlive = _engineInstance;
      _engineInstance = null;

      base.RemoveEngineInstance(bRemoveChildren);

      _engineInstance = keepAlive;
    }

    public override void RenderShape(VisionViewBase view, ShapeRenderMode mode)
    {
      base.RenderShape(view, mode);

      if( EngineInstance != null && Parent != null && ((CustomVolumeShape)Parent).IsRendering && !((CustomVolumeShape)Parent).CustomStaticMesh )
        EngineInstance.RenderShape(view, mode);
    }

    public override void OnRemoveFromScene()
    {
      base.OnRemoveFromScene();
      if (EditorManager.Scene != null && _lastParent != null && _lastParent.EngineInstance != null && !_lastParent.CustomStaticMesh)
      {
        _lastParent.EngineInstance.UpdateStaticMesh(EngineInstanceCustomVolumeObject.VUpdateType_e.VUT_UPDATE_RETRIANGULATE);

        //Select last parent to not loose focus
        ShapeCollection selection = new ShapeCollection();
        selection.Add(_lastParent);
        EditorManager.SelectedShapes = selection;
      }

      //finally remove the engine instance
      if (this._engineInstance != null)
      {
        // Important: We must call the base implementation here because the overload in this class prevents the deletion
        base.RemoveEngineInstance(false);
      }
    }

    /// <summary>
    /// Overridden OnSelected function
    /// </summary>
    public override void OnSelected()
    {
      base.OnSelected();
      if (Parent != null)
        ((CustomVolumeShape)Parent).OnChildSelected();
    }

    /// <summary>
    /// Overridden OnUnSelected function
    /// </summary>
    public override void OnUnSelected()
    {
      base.OnUnSelected();
      if (Parent != null)
        ((CustomVolumeShape)Parent).OnChildUnSelected();
    }

    /// <summary>
    /// Indicates whether the volume is currently inside a cloning operation
    /// </summary>
    public static bool IsVolumeCloning = false;

    /// <summary>
    /// This shape type can not be copied or pasted
    /// </summary>
    public override bool CanCopyPaste { get { return IsVolumeCloning; } }

    /// <summary>
    /// This shape type cannot be dragged in the tree view
    /// </summary>
    /// <param name="newParent"></param>
    /// <returns></returns>
    public override bool AllowsToSetParent(ShapeBase newParent)
    {
      return false;
    }

    /// <summary>
    /// overridden Clone function
    /// </summary>
    /// <returns>a clone</returns>
    public override ShapeBase Clone()
    {
      CustomVolumeVertex clone = (CustomVolumeVertex)base.Clone();
      clone._parentShape = this._parentShape;
      return clone;
    }

    #endregion

    #region Helper functions

    /// <summary>
    /// accesses the engine instance as an object of type EngineInstanceCustomVolumeVertex
    /// </summary>
    [BrowsableAttribute(false)]
    public EngineInstanceCustomVolumeVertex EngineInstance { get { return (EngineInstanceCustomVolumeVertex)_engineInstance; } }

    #endregion

    #region ISerializable

    /// <summary>
    /// Called when deserializing
    /// </summary>
    protected CustomVolumeVertex(SerializationInfo info, StreamingContext context) : base(info, context)
    {
      SetHint(HintFlags_e.NoScale, true);
    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    public override void GetObjectData(SerializationInfo info, StreamingContext context) 
    {
      base.GetObjectData(info,context);
    }

    #endregion

    #region Tracing

    public override void OnTraceShape(ShapeTraceMode_e mode, Vector3F rayStart, Vector3F rayEnd, ref ShapeTraceResult result)
    {
      if (mode == ShapeTraceMode_e.ShapePicking && _engineInstance != null)
        _engineInstance.TraceShape(this, rayStart, rayEnd, ref result);
    }

    #endregion
  }

  #endregion

  #region Class : CustomVolumeShapeCreator

  class CustomVolumeShapeCreator : CSharpFramework.IShapeCreatorPlugin
  {
    public CustomVolumeShapeCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\cube_molecule.png"), Color.Black);
    }

    /// <summary>
    /// Get the name of the creator
    /// </summary>
    /// <returns></returns>
    public override string GetPluginName()
    {
      return "Custom Volume";
    }

    /// <summary>
    /// Get the category's name
    /// </summary>
    /// <returns></returns>
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
      return "Adds a Custom Volume. A custom volume can be used to limit the area a deferred light can affect and to designate Havok trigger volumes.";
    }

    /// <summary>
    /// Overridden function to create the shape instance
    /// </summary>
    /// <returns></returns>
    public override ShapeBase CreateShapeInstance()
    {
      CustomVolumeShape shape = new CustomVolumeShape("Custom Volume Shape", false);
      shape.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
      EditorManager.ActiveView.CurrentContext = new AddVolumeVertexContext(shape);
      return shape;
    }
  }

  #endregion

  #region Contexts

  class AddVolumeVertexContext : CameraMoveContext
  {
    private CustomVolumeShape _shape;
    private KeyModifier _keyMod = KeyModifier.None;
    private CreateCustomVolumeDlg _contextDialog;

    public AddVolumeVertexContext(CustomVolumeShape shape)
    {
      this.AllowShapeSelection = false;
      this.AllowRecenterMouse = false;
      _shape = shape;
      _contextDialog = new CreateCustomVolumeDlg();
      _contextDialog.Show(EditorManager.ActiveView);
      _contextDialog.FormClosed += new FormClosedEventHandler(FormClosed);
      IScene.ShapeChanged += new ShapeChangedEventHandler(ShapeChanged);
      IScene.PropertyChanged += new CSharpFramework.PropertyChangedEventHandler(PropertyChanged);
      EditorManager.BeforeSceneClosing += new BeforeSceneClosing(SceneClosing);
      EditorManager.SceneEvent += new SceneEventHandler(EditorManager_SceneEvent);
      EditorManager.ActiveView.Focus();
    }

    void ShapeChanged(object sender, ShapeChangedArgs e)
    {
      if (e.action == ShapeChangedArgs.Action.Removed && e.shape == _shape)
      {
        // Will result in call to OnDeactivate and end the editing
        EditorManager.ActiveView.SetDefaultContext();
      }
    }

    void PropertyChanged(object sender, PropertyChangedArgs e)
    {
      if (e._component == _shape && _shape.CustomStaticMesh)
      {
        // Will result in call to OnDeactivate and end the editing
        EditorManager.ActiveView.SetDefaultContext();
      }
    }

    void FormClosed(object sender, FormClosedEventArgs e)
    {
      // Will result in call to OnDeactivate and end the editing
      EditorManager.ActiveView.SetDefaultContext();
    }

    void SceneClosing()
    {
      // Will result in call to OnDeactivate and end the editing
      EditorManager.ActiveView.SetDefaultContext();
    }

    void EditorManager_SceneEvent(object sender, SceneEventArgs e)
    {
      if (e.action == SceneEventArgs.Action.BeforeClosing)
      {
        // Will result in call to OnDeactivate and end the editing
        EditorManager.ActiveView.SetDefaultContext();
      }
    }

    public override void OnDeactivate()
    {
      EndEditing();
      base.OnDeactivate();
    }

    public override void OnKeyModifierChanged(KeyModifier eOldMod, KeyModifier eNewMod)
    {
      base.OnKeyModifierChanged(eOldMod, eNewMod);
      _keyMod = eNewMod;
    }

    public override bool OnMouseClick()
    {
      bool bUpdate = base.OnMouseClick();

      if (_keyMod == KeyModifier.Ctrl)
      {
        Vector3F startRay, endRay;
        Vector3F hitPoint = new Vector3F();
        EditorManager.EngineManager.GetRayAtScreenPos(out startRay, out endRay, MouseX, MouseY, EditorManager.Settings.MaxPickingDistance);
        if (EditorManager.EngineManager.GetTraceHit(startRay, endRay, ref hitPoint))
        {
          CustomVolumeVertex vertex = new CustomVolumeVertex();
          EditorManager.Actions.Add(AddShapeAction.CreateAddShapeAction(vertex, _shape, _shape.ParentLayer, true));
          vertex.CreateEngineInstance(true);
          vertex.Position = hitPoint;
          bUpdate = true;
        }
      }

      return bUpdate;
    }

    public override bool OnMouseDoubleClick()
    {
      bool bUpdate = base.OnMouseDoubleClick();

      // Will result in call to OnDeactivate and end the editing
      EditorManager.ActiveView.SetDefaultContext();

      return bUpdate;
    }

    void EndEditing()
    {
      _shape.ConstructionFinished = true;
      Cursor.Show();

      // Close dialog and remove callbacks
      _contextDialog.FormClosed -= new FormClosedEventHandler(FormClosed);
      IScene.ShapeChanged -= new ShapeChangedEventHandler(ShapeChanged);
      IScene.PropertyChanged -= new CSharpFramework.PropertyChangedEventHandler(PropertyChanged);
      EditorManager.BeforeSceneClosing -= new BeforeSceneClosing(SceneClosing);
      EditorManager.SceneEvent -= new SceneEventHandler(EditorManager_SceneEvent);
      _contextDialog.Close();

      // Set selection to the edited custom volume shape
      ShapeCollection selection = new ShapeCollection();
      selection.Add(_shape);
      EditorManager.SelectedShapes = selection;
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
