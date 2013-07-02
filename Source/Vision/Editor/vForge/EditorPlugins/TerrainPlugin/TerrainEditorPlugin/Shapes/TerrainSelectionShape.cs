/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections.Generic;
using System.Text;
using CSharpFramework.Shapes;
using TerrainBase.Editing;
using CSharpFramework.View;
using CSharpFramework;
using System.ComponentModel;
using CSharpFramework.Clipboard;
using System.Drawing;
using CSharpFramework.Controls;
using CSharpFramework.Math;
using CSharpFramework.Actions;
using CSharpFramework.Scene;

namespace TerrainEditorPlugin.Shapes
{
  [Serializable]
  public class TerrainSelectionShape : Shape3D
  {

    public TerrainSelectionShape(string name)
      : base(name)
    {
      AddHint(HintFlags_e.Invisible);
      AddHint(HintFlags_e.NonSerialized);
      AddHint(HintFlags_e.NonRemovable);
      AddHint(HintFlags_e.NoChildren);
      AddHint(HintFlags_e.NoRotation);

      float s = EditorManager.Settings.GlobalUnitScaling;
      _fMinX *= s;
      _fMinY *= s;
      _fMaxX *= s;
      _fMaxY *= s;
    }

    [Browsable(false)]
    public TerrainSelection CurrentSelection
    {
      get {return  _selection; }
    }

    [Browsable(false)]
    public TerrainShape OwnerTerrain
    {
      get { return (TerrainShape)this.Parent; }
    }

    void UpdateSelectionRect()
    {
      Vector3F pos = this.Position;

      _selection.SetExtent(
        pos.X - _fMinX * ScaleX,
        pos.Y - _fMinY * ScaleY,
        pos.X + _fMaxX * ScaleX,
        pos.Y + _fMaxY * ScaleY
        );
      _selection.DisplayZ = pos.Z;

      if (_hotspotMinX != null)
      {
        _hotspotMinX.DisplayScaling = ScaleX;
        _hotspotMaxX.DisplayScaling = ScaleX;
        _hotspotMinY.DisplayScaling = ScaleY;
        _hotspotMaxY.DisplayScaling = ScaleY;
      }
      Scene2DView.Update2DViews();
    }


    TerrainSelection _selection = new TerrainSelection();

    #region Shape overrides

    public override void OnAddedToScene()
    {
      EditorClipboard.OnClipboardDataChanged += new EventHandler(EditorClipboard_OnClipboardDataChanged);
      base.OnAddedToScene();


      UpdateSelectionRect();
    }


    void SafeRemoveHotSpot(HotSpotBase hotSpot)
    {
      if (hotSpot == null) return;
      EditorManager.ActiveView.HotSpots.Remove(hotSpot);
      hotSpot.Remove();
    }

    void CreateAllHotspots()
    {
      if (_hotspotMinX != null) // hotspots are there already
        return;
      _hotspotMinX = new HotSpotDistance(this, @"textures\Hotspot_Rect.dds", VisionColors.Yellow, HotSpotBase.PickType.Square, 6.0f);
      _hotspotMinX.StartDistance = _fMinX;
      _hotspotMinX.Axis = -Vector3F.XAxis;
      _hotspotMinX.ToolTipText = "Selection Range";
      EditorManager.ActiveView.HotSpots.Add(_hotspotMinX);

      _hotspotMinY = new HotSpotDistance(this, @"textures\Hotspot_Rect.dds", VisionColors.Yellow, HotSpotBase.PickType.Square, 6.0f);
      _hotspotMinY.StartDistance = _fMinY;
      _hotspotMinY.Axis = -Vector3F.YAxis;
      _hotspotMinY.ToolTipText = "Selection Range";
      EditorManager.ActiveView.HotSpots.Add(_hotspotMinY);

      _hotspotMaxX = new HotSpotDistance(this, @"textures\Hotspot_Rect.dds", VisionColors.Yellow, HotSpotBase.PickType.Square, 6.0f);
      _hotspotMaxX.StartDistance = _fMaxX;
      _hotspotMaxX.Axis = Vector3F.XAxis;
      _hotspotMaxX.ToolTipText = "Selection Range";
      EditorManager.ActiveView.HotSpots.Add(_hotspotMaxX);

      _hotspotMaxY = new HotSpotDistance(this, @"textures\Hotspot_Rect.dds", VisionColors.Yellow, HotSpotBase.PickType.Square, 6.0f);
      _hotspotMaxY.StartDistance = _fMaxY;
      _hotspotMaxY.Axis = Vector3F.YAxis;
      _hotspotMaxY.ToolTipText = "Selection Range";
      EditorManager.ActiveView.HotSpots.Add(_hotspotMaxY);

      _hotspotMinX.DisplayScaling = ScaleX;
      _hotspotMaxX.DisplayScaling = ScaleX;
      _hotspotMinY.DisplayScaling = ScaleY;
      _hotspotMaxY.DisplayScaling = ScaleY;

      _hotspotCopy = new CopySelectionHotspot(this);
      _hotspotCopy.Set2DOffset(-16.0f, 20.0f);
      EditorManager.ActiveView.HotSpots.Add(_hotspotCopy);

      _hotspotPaste = new PasteSelectionHotspot(this);
      _hotspotPaste.Set2DOffset(-16.0f, 56.0f);
      EditorManager.ActiveView.HotSpots.Add(_hotspotPaste);

      _hotspotAdjustSize = new AdjustPasteSizeHotspot(this);
      _hotspotAdjustSize.Set2DOffset(20.0f, 56.0f);
      EditorManager.ActiveView.HotSpots.Add(_hotspotAdjustSize);

      UpdateHotspotVisibility();
    }

    void RemoveAllHotspots()
    {
      SafeRemoveHotSpot(_hotspotMinX); _hotspotMinX = null;
      SafeRemoveHotSpot(_hotspotMinY); _hotspotMinY = null;
      SafeRemoveHotSpot(_hotspotMaxX); _hotspotMaxX = null;
      SafeRemoveHotSpot(_hotspotMaxY); _hotspotMaxY = null;

      SafeRemoveHotSpot(_hotspotCopy); _hotspotCopy = null;
      SafeRemoveHotSpot(_hotspotPaste); _hotspotPaste = null;
      SafeRemoveHotSpot(_hotspotAdjustSize); _hotspotAdjustSize = null;
    }


    public override void OnRemoveFromScene()
    {
      EditorClipboard.OnClipboardDataChanged -= new EventHandler(EditorClipboard_OnClipboardDataChanged);
      base.OnRemoveFromScene();
    }


    public override void OnTransformationChanged()
    {
      base.OnTransformationChanged();
      UpdateSelectionRect();
    }

    public override void RenderShape(CSharpFramework.VisionViewBase view, ShapeRenderMode mode)
    {
      base.RenderShape(view, mode);
      if (mode == ShapeRenderMode.Selected)
        _selection.RenderSelection(view, mode);
    }

    public override void OnSelected()
    {
 	    base.OnSelected();
      UpdateSelectionRect();
      UpdateHotspotVisibility();
      Scene2DRenderingEnabled = true;
      CreateAllHotspots();
    }

    public override void OnUnSelected()
    {
      base.OnUnSelected();
      UpdateHotspotVisibility();
      Scene2DRenderingEnabled = false;
      RemoveAllHotspots();
    }

    public override bool SupportsComponents
    {
      get
      {
        return false;
      }
    }


    public override bool CanCopyPaste
    {
      get
      {
        return false;
      }
    }


    #endregion

    #region Shape Properties

    float _fMinX = 1000.0f;
    float _fMaxX = 1000.0f;
    float _fMinY = 1000.0f;
    float _fMaxY = 1000.0f;

    [Description("Relative extent of the selection")]
    [SortedCategory("Selection", Shape3D.LAST_CATEGORY_ORDER_ID + 1), PropertyOrder(1)]
    public float MinX
    {
      get { return _fMinX; }
      set 
      { 
        _fMinX = System.Math.Max(value,0.0f);
        if (_hotspotMinX != null)
          _hotspotMinX.StartDistance = _fMinX;
        UpdateSelectionRect(); 
      }
    }

    [Description("Relative extent of the selection")]
    [SortedCategory("Selection", Shape3D.LAST_CATEGORY_ORDER_ID + 1), PropertyOrder(2)]
    public float MinY
    {
      get { return _fMinY; }
      set
      {
        _fMinY = System.Math.Max(value, 0.0f);
        if (_hotspotMinY != null)
          _hotspotMinY.StartDistance = _fMinY;
        UpdateSelectionRect();
      }
    }

    [Description("Relative extent of the selection")]
    [SortedCategory("Selection", Shape3D.LAST_CATEGORY_ORDER_ID + 1), PropertyOrder(3)]
    public float MaxX
    {
      get { return _fMaxX; }
      set
      {
        _fMaxX = System.Math.Max(value, 0.0f);
        if (_hotspotMaxX != null)
          _hotspotMaxX.StartDistance = _fMaxX;
        UpdateSelectionRect();
      }
    }

    [Description("Relative extent of the selection")]
    [SortedCategory("Selection", Shape3D.LAST_CATEGORY_ORDER_ID + 1), PropertyOrder(4)]
    public float MaxY
    {
      get { return _fMaxY; }
      set
      {
        _fMaxY = System.Math.Max(value, 0.0f);
        if (_hotspotMaxY != null)
          _hotspotMaxY.StartDistance = _fMaxY;
        UpdateSelectionRect();
      }
    }

    [Description("Filter bitmask. Only channels enabled here are copied or pasted.")]
    [SortedCategory("Selection", Shape3D.LAST_CATEGORY_ORDER_ID + 1), PropertyOrder(5)]
    public TerrainSelection.SelectionFilter_e SelectionFilter
    {
      get
      {
        return _selection.SelectionFilter;
      }
      set
      {
        _selection.SelectionFilter = value;
      }
    }

    #endregion

    #region Hotspots


    void UpdateHotspotVisibility()
    {
      if (_hotspotMinX == null)
        return;
      bool bVisible = this.Selected;
      _hotspotMinX.Visible = bVisible;
      _hotspotMinY.Visible = bVisible;
      _hotspotMaxX.Visible = bVisible;
      _hotspotMaxY.Visible = bVisible;
      _hotspotCopy.Visible = bVisible;
      bool bHasTerrainClipboard = bVisible && EditorManager.ActiveComponent.Clipboard!=null && EditorManager.ActiveComponent.Clipboard.Data is ITerrainClipboardObject;
      _hotspotPaste.Visible = bHasTerrainClipboard;
      _hotspotAdjustSize.Visible = bHasTerrainClipboard;
    }


    #region Nested Hotspot classes

    public class CopySelectionHotspot : HotSpotPushButton
    {
      public CopySelectionHotspot(TerrainSelectionShape selShape)
        : base(selShape, @"textures\copy32.png", VisionColors.RGBA(230, 230, 230, 255), VisionColors.White, 16.0f)
      {
        this.ToolTipText = "Copies the current selection into the clipboard";
      }

      public override void EvaluatePosition()
      {
        base.EvaluatePosition();
        this.Position = Owner.Position;
      }

      public override void OnClicked(VisionViewBase view)
      {
        base.OnClicked(view);
        ITerrainClipboardObject clip = ((TerrainSelectionShape)Owner).OwnerTerrain.CopySelection(null);
        if (clip != null)
          EditorManager.Clipboard.Data = clip;
      }
    }

    public class PasteSelectionHotspot : HotSpotPushButton
    {
      public PasteSelectionHotspot(TerrainSelectionShape selShape)
        : base(selShape, @"textures\paste32.png", VisionColors.RGBA(230, 230, 230, 255), VisionColors.White, 16.0f)
      {
        this.ToolTipText = "Pastes the current clipboard content into this selection (uses scaling)";
      }

      public override void EvaluatePosition()
      {
        base.EvaluatePosition();
        this.Position = Owner.Position;
      }

      public override void OnClicked(VisionViewBase view)
      {
        base.OnClicked(view);
        if (TerrainEditor.CurrentSelection == null || TerrainEditor.CurrentTerrain == null || TerrainEditor.CurrentTerrain.EngineTerrain == null)
          return;
        ITerrainClipboardObject data = (EditorManager.ActiveComponent.Clipboard == null) ? null
          : EditorManager.ActiveComponent.Clipboard.Data as ITerrainClipboardObject;
        
        if (data == null)
          return;
        data.TryPaste(TerrainEditor.CurrentTerrain.EngineTerrain);
      }
    }

    public class AdjustPasteSizeHotspot : HotSpotPushButton
    {
      public AdjustPasteSizeHotspot(TerrainSelectionShape selShape)
        : base(selShape, @"textures\fit_to_size32.png", VisionColors.RGBA(230, 230, 230, 255), VisionColors.White, 16.0f)
      {
        this.ToolTipText = "Take the selection size from the clipboard data (same size in world units)";
      }

      public override void EvaluatePosition()
      {
        base.EvaluatePosition();
        this.Position = Owner.Position;
      }

      public override void OnClicked(VisionViewBase view)
      {
        base.OnClicked(view);
        if (TerrainEditor.CurrentSelection == null)
          return;
        ITerrainClipboardObject data = (EditorManager.ActiveComponent.Clipboard == null) ? null
          : EditorManager.ActiveComponent.Clipboard.Data as ITerrainClipboardObject;
        
        if (data == null)
          return;

        TerrainSelectionShape selShape = (TerrainSelectionShape)this.Owner;
        float fRadX = data.OriginalExtent.GetSizeX() * 0.5f;
        float fRadY = data.OriginalExtent.GetSizeY() * 0.5f;

        GroupAction action = new GroupAction("adjust selection size");
        action.Add(SetPropertyAction.CreateSetPropertyAction(selShape, "MinX", fRadX)); // min values are also positive
        action.Add(SetPropertyAction.CreateSetPropertyAction(selShape, "MinY", fRadY));
        action.Add(SetPropertyAction.CreateSetPropertyAction(selShape, "MaxX", fRadX));
        action.Add(SetPropertyAction.CreateSetPropertyAction(selShape, "MaxY", fRadY));
        EditorManager.Actions.Add(action);
      }
    }

    #endregion

    HotSpotDistance _hotspotMinX;
    HotSpotDistance _hotspotMinY;
    HotSpotDistance _hotspotMaxX;
    HotSpotDistance _hotspotMaxY;
    CopySelectionHotspot _hotspotCopy;
    PasteSelectionHotspot _hotspotPaste;
    AdjustPasteSizeHotspot _hotspotAdjustSize;

    public override void OnHotSpotDrag(HotSpotBase hotSpot, VisionViewBase view, float fDeltaX, float fDeltaY)
    {
      base.OnHotSpotDrag(hotSpot, view, fDeltaX, fDeltaY);

      _fMinX = _hotspotMinX.CurrentDistance;
      _fMinY = _hotspotMinY.CurrentDistance;
      _fMaxX = _hotspotMaxX.CurrentDistance;
      _fMaxY = _hotspotMaxY.CurrentDistance;

      UpdateSelectionRect();
    }

    void EditorClipboard_OnClipboardDataChanged(object sender, EventArgs e)
    {
      UpdateHotspotVisibility();
    }

    #endregion

    #region 2D View

    static string VA_AS_SELECTION = @"Terrain selection\As selection";

    public override void Get2DViewActions(Scene2DView view, Scene2DActions actions)
    {
      base.Get2DViewActions(view, actions);
      if (!this.Modifiable || !view.SelectionMarqueeWorldBox.Valid)
        return;
      actions.AddAction(this, VA_AS_SELECTION);
    }

    public override void Perform2DViewAction(Scene2DView view, GroupAction parent, string action)
    {
      base.Perform2DViewAction(view, parent, action);
      BoundingBox box = view.SelectionMarqueeWorldBox;
      if (action == VA_AS_SELECTION && box.Valid)
      {
        Vector3F center = box.Center;
        float fRadX = box.SizeX * 0.5f;
        float fRadY = box.SizeY * 0.5f;
        parent.Add(new MoveShapeAction(this, this.Position, center));
        parent.Add(SetPropertyAction.CreateSetPropertyAction(this, "MinX", fRadX)); // min values are also positive
        parent.Add(SetPropertyAction.CreateSetPropertyAction(this, "MinY", fRadY));
        parent.Add(SetPropertyAction.CreateSetPropertyAction(this, "MaxX", fRadX));
        parent.Add(SetPropertyAction.CreateSetPropertyAction(this, "MaxY", fRadY));
      }
    }

    static Scene2DViewFilterCategory FilterCat = null;

    public override Scene2DViewFilterCategory GetScene2DRenderingFilter(Scene2DView view)
    {
      if (FilterCat == null)
      {
        FilterCat = view.CreateFilterCategory("Terrain selection");
        if (IconIndex > 0)
          FilterCat.Image = EditorManager.GUI.ShapeTreeImages.ImageList.Images[IconIndex];
      }
      return FilterCat;
    }

    static Pen _pen = new Pen(Color.Beige, 2.0f);

    public override void OnPaintScene2DView(CSharpFramework.Controls.Scene2DView view, System.Windows.Forms.PaintEventArgs e)
    {
      _pen.DashStyle = System.Drawing.Drawing2D.DashStyle.Dash;
      Point p1 = view.Transform(new Vector3F(this._selection.WorldSpaceExtent.X1, this._selection.WorldSpaceExtent.Y1, this._selection.DisplayZ));
      Point p2 = view.Transform(new Vector3F(this._selection.WorldSpaceExtent.X2, this._selection.WorldSpaceExtent.Y2, this._selection.DisplayZ));
      Rectangle r = view.MakeValidRect(p1, p2);
      e.Graphics.DrawRectangle(_pen, r);
    }

    public override float GetScene2DRenderingSortingKey(CSharpFramework.Controls.Scene2DView view)
    {
      return 100000000000.0f; // render last
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
