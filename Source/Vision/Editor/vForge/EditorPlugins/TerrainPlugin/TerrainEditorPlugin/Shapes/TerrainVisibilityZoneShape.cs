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
using CSharpFramework.Actions;
using CSharpFramework.Controls;
using CSharpFramework.Shapes;
using CSharpFramework.Math;
using TerrainBase.Config;
using TerrainBase.Terrain;
using System.Drawing;
using System.Drawing.Imaging;
using CSharpFramework;
using System.Windows.Forms;
using CSharpFramework.Scene;
using CSharpFramework.PropertyEditors;
using System.ComponentModel;
using TerrainManaged;

namespace TerrainEditorPlugin.Shapes
{
  #region class SetSectorPropertyAction

  public class SetSectorPropertyAction : SetPropertyAction
  {
    public SetSectorPropertyAction(TerrainVisibilityZoneShape sector, string propertyName, object newValue)
      : base(sector, propertyName, newValue)
    {
      _sector = sector;
    }

    TerrainVisibilityZoneShape _sector;

    public override void Do()
    {
      base.Do();
      Scene2DView.Update2DViews();
    }

    public override void Undo()
    {
      base.Undo();
      Scene2DView.Update2DViews();
    }

    /// <summary>
    /// This is an important override since the sector itself is not modifiable
    /// </summary>
    public override bool Valid
    {
      get
      {
        if (!_sector.OwnerTerrain.Modifiable)
          return false;
        return true; //  base.Valid;
      }
    }

  }

  #endregion

  #region TerrainVisibility shape

  public class TerrainVisibilityZoneShape : VisibilityZoneShape, ITerrainObject
  {
    public TerrainVisibilityZoneShape(TerrainShape owner, int iSectorX, int iSectorY)
      : base(string.Format("TerrainZone_{0}_{1}", iSectorX, iSectorX))
    {
      _iSectorX = iSectorX;
      _iSectorY = iSectorY;
      _ownerTerrain = owner;
      _createHotSpots = false;
    }

    int _iSectorX, _iSectorY;
    TerrainShape _ownerTerrain;
    // uint _SectorVisibleMask = 0xffffffff;

    public int SectorX { get { return _iSectorX; } }
    public int SectorY { get { return _iSectorY; } }

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_VISIBILTY = "Visibility";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_VISIBILITY = VisibilityZoneShape.LAST_CATEGORY_ORDER_ID + 1;

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_PHYSICS = "Physics";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_PHYSICS = VisibilityZoneShape.LAST_CATEGORY_ORDER_ID + 2;

    [SortedCategory(CAT_VISIBILTY, CATORDER_VISIBILITY),
    PropertyOrder(1)]
    [Description("Visibility bit-mask")]
    public FlagsInt32_e SectorVisibleMask
    {
      get
      {
        if (_ownerTerrain.EngineTerrain == null)
          return FlagsInt32_e.All;
        return (FlagsInt32_e)_ownerTerrain.EngineTerrain.GetSectorVisibleBitmask(_iSectorX, _iSectorY);
      }
      set
      {
        if (_ownerTerrain.EngineTerrain == null)
          return;
        _ownerTerrain.EngineTerrain.SetSectorVisibleBitmask(_iSectorX, _iSectorY, (uint)value);
      }
    }

    [SortedCategory(CAT_PHYSICS, CATORDER_PHYSICS),
    PropertyOrder(1)]
    [Description("Collision bit-mask")]
    public FlagsInt16_e SectorCollisionMask
    {
      get
      {
        if (_ownerTerrain.EngineTerrain == null)
          return FlagsInt16_e.All;
        return (FlagsInt16_e)_ownerTerrain.EngineTerrain.GetSectorCollisionBitmask(_iSectorX, _iSectorY);
      }
      set
      {
        if (_ownerTerrain.EngineTerrain == null)
          return;
        _ownerTerrain.EngineTerrain.SetSectorCollisionBitmask(_iSectorX, _iSectorY, (ushort)value);
      }
    }

    [SortedCategory(CAT_PHYSICS, CATORDER_PHYSICS),
    PropertyOrder(2)]
    [Description("Physics representation type. (Please note: Approximate can not be used for terrain sector with holes.)")]
    public SectorPhysicsType_e SectorPhysicsType
    {
      get
      {
        if (_ownerTerrain.EngineTerrain == null)
          return SectorPhysicsType_e.Precise;
        return _ownerTerrain.EngineTerrain.GetSectorPhysicsType(_iSectorX, _iSectorY);
      }
      set
      {
        if (_ownerTerrain.EngineTerrain == null)
          return;
        _ownerTerrain.EngineTerrain.SetSectorPhysicsType(_iSectorX, _iSectorY, value);
      }
    }

    public void UpdateSectorBoundingBox()
    {
      if (_ownerTerrain.EngineTerrain == null)
        return;
      BoundingBox box = new BoundingBox();
      if (_ownerTerrain.EngineTerrain.GetSectorBoundingBox(_iSectorX, _iSectorY, ref box))
      {
        this.BoundingBoxMin = box.vMin;
        this.BoundingBoxMax = box.vMax;
      }
    }

    public void SetSectorBoundingBox(BoundingBox box)
    {
      BoundingBoxMin = box.vMin;
      BoundingBoxMax = box.vMax;
      _ownerTerrain.EngineTerrain.SetSectorBoundingBox(_iSectorX, _iSectorY, box);
    }

    /// <summary>
    /// Returns a bounding box that defines the bounding box of this sector, not including potential margins 
    /// </summary>
    BoundingBox SectorExtent
    {
      get
      {
        TerrainConfig cfg = _ownerTerrain.Config;
        BoundingBox sectorBBox = new BoundingBox(
          cfg.GetSectorOriginX(_iSectorX), cfg.GetSectorOriginY(_iSectorY), 0.0f,
          cfg.GetSectorOriginX(_iSectorX + 1), cfg.GetSectorOriginY(_iSectorY + 1), 0.0f);
        return sectorBBox;
      }
    }

    /// <summary>
    /// Uses the visibility zone that is already used by the terrain
    /// </summary>
    public override void AcquireNativeZone()
    {
      base.AcquireNativeZone();

      if (_ownerTerrain.EngineTerrain == null)
        return;

      UpdateSectorBoundingBox(); // make sure bbounding box is up-to-date
      this.NativeZonePointer = _ownerTerrain.EngineTerrain.GetNativeVisibiltyZone(_iSectorX, _iSectorY);
    }

    public override void OnConnect(VisibilityZoneShape otherZone, PortalShape viaPortal)
    {
      base.OnConnect(otherZone, viaPortal);

      // increase the max z value so connected zones are completely inside, but not for terrain zones
      if (otherZone is TerrainVisibilityZoneShape)
        return;

      float fMaxZ = otherZone.FinalBoundingBox.vMax.Z;
      Vector3F pt = this.FinalBoundingBox.Center;
      pt.Z = fMaxZ;
      this.FinalBoundingBox.AddPoint(pt);
    }


    #region Native sector status

    SectorAction _currentStatus = SectorAction.Unloaded;

    public SectorAction CurrentStatus
    {
      get { return _currentStatus; }
      set
      {
        if (_currentStatus == value)
          return;
        _currentStatus = value;
        ViewBitmapDirty = true;
      }
    }

    #endregion

    #region Shape Overrides

    public override bool Traceable
    {
      get
      {
        if (EditorManager.Settings.AllowLockedShapesSelection)
          return true;
        return Modifiable;
      }
    }

    public override PropertyFlags_e GetPropertyFlags(PropertyFlagsProviderInfo pd)
    {
      if (pd.Name == "SectorVisibleMask" || pd.Name == "SectorCollisionMask" || pd.Name == "SectorPhysicsType")
        return base.GetPropertyFlags(pd);
      return PropertyFlags_e.Hidden;
    }

    public override void OnAddedToScene()
    {
      base.OnAddedToScene();
      AddHint(HintFlags_e.HideGizmo);
    }

    #endregion

    #region 2D rendering
          
    void UpdatePreviewBitmap()
    {
      if (_currentStatus == SectorAction.Unloaded)
        return;
      if (_2dViewBitmap == null)
          _2dViewBitmap = new Bitmap(_2dViewBitmapResolution, _2dViewBitmapResolution, PixelFormat.Format32bppArgb);

      BitmapData lockData = _2dViewBitmap.LockBits(new Rectangle(0, 0, _2dViewBitmapResolution, _2dViewBitmapResolution), ImageLockMode.WriteOnly, PixelFormat.Format32bppArgb);
      _ownerTerrain.EngineTerrain.CreateSectorMapPreview(_iSectorX, _iSectorY, lockData);
      _2dViewBitmap.UnlockBits(lockData);
    }

    /// <summary>
    /// Set this to true if the 2D view image should be updated
    /// </summary>
    public bool ViewBitmapDirty
    {
      set
      {
        _bViewBitmapDirty = value;
        Scene2DView.Update2DViews();
      }
    }

    bool _bViewBitmapDirty = true;
    Bitmap _2dViewBitmap = null;
    static ImageAttributes _2dViewAttr = new ImageAttributes();
    static Brush _selectionFill = new SolidBrush(Color.FromArgb(80, 255, 50, 50));
    static Pen _pen = new Pen(Color.Turquoise, 1);
    static Font _font = new Font("Arial", 12, FontStyle.Bold);
    const int _2dViewBitmapResolution = 32;

    public override BoundingBox GetAbsoluteBoundingBox(Scene2DView view)
    {
      return SectorExtent; // no margins
    }

    public override void OnPaintScene2DView(CSharpFramework.Controls.Scene2DView view, PaintEventArgs e)
    {
      if (SectorVisibleMask == 0)
        return;
      if (_bViewBitmapDirty)
      {
        UpdatePreviewBitmap();
        _bViewBitmapDirty = false;
      }
      if (_2dViewBitmap != null)
      {
        BoundingBox sectorBBox = SectorExtent;
        Point p1 = view.Transform(sectorBBox.vMin);
        Point p2 = view.Transform(sectorBBox.vMax);

        Rectangle dst = view.MakeValidRect(p1, p2);
        // darker images for unloaded sectors
        _2dViewAttr.SetGamma(CurrentStatus == SectorAction.Reloaded ? 1.0f : 4.0f);
        e.Graphics.DrawImage(_2dViewBitmap, dst, 0, _2dViewBitmapResolution - 1, _2dViewBitmapResolution, -_2dViewBitmapResolution, GraphicsUnit.Pixel, _2dViewAttr); // flip vertically

        // inside selection? then overlay with color
        if (view.TouchesSelectionMarquee(sectorBBox))
          e.Graphics.FillRectangle(_selectionFill, dst);


        _pen.Color = Color.Turquoise;
        if (CurrentStatus == SectorAction.Modified) _pen.Color = Color.Orange;
        if (CurrentStatus == SectorAction.Reloaded) _pen.Color = Color.Magenta;

        dst.Location = new Point(p1.X, p2.Y);
        dst.Size = new Size(p2.X - p1.X, p1.Y - p2.Y);
        e.Graphics.DrawRectangle(_pen, dst);

        //Display terrain status
        // Locked, Modified, Unmodified

        if (_ownerTerrain.EngineTerrain.GetSectorNetworkLock(_iSectorX, _iSectorY))
        {
          dst.Width = 10;
          dst.Height = 10;
          _pen.Color = Color.Red;
          e.Graphics.DrawRectangle(_pen, dst);
        }

        if (_ownerTerrain.EngineTerrain.GetSectorOwnsLock(_iSectorX, _iSectorY))
        {
          dst.Width = 10;
          dst.Height = 10;
          _pen.Color = Color.Green;
          e.Graphics.DrawRectangle(_pen, dst);
        }
      }
      else
        base.OnPaintScene2DView(view, e);

      if (FilterCollisionCat != null && FilterCollisionCat.Enabled)
      {
        if (SectorCollisionMask > 0)
        {
          BoundingBox sectorBBox = SectorExtent;
          Point p1 = view.Transform(sectorBBox.vMin);
          Point p2 = view.Transform(sectorBBox.vMax);

          Rectangle dst = view.MakeValidRect(p1, p2);
          e.Graphics.DrawString("C", _font, Brushes.White, dst);
        }
      }

    }

    public override float GetScene2DRenderingSortingKey(Scene2DView view)
    {
      return -100000000.0f; // below everything else
    }

    static Scene2DViewFilterCategory FilterCat = null;
    static Scene2DViewFilterCategory FilterCollisionCat = null;
    public override Scene2DViewFilterCategory GetScene2DRenderingFilter(Scene2DView view)
    {
      if (FilterCat == null)
      {
        FilterCat = view.CreateFilterCategory("Terrain");
        int iIndex = TerrainShape.GetTerrainIconIndex();
        if (iIndex >= 0)
          FilterCat.Image = EditorManager.GUI.ShapeTreeImages.ImageList.Images[iIndex];
      }
      if (FilterCollisionCat == null)
      {
        FilterCollisionCat = view.CreateFilterCategory("Terrain Collision");
        FilterCollisionCat.Image = FilterCat.Image;
        FilterCollisionCat.Enabled = false; // disabled by default
      }
      return FilterCat;
    }

    static string VA_UNLOCK_SECTORS = @"Terrain\Unlock sector(s)";
    static string VA_MAKE_VISIBLE = @"Terrain\Make sector(s) visible";
    static string VA_MAKE_INVISIBLE = @"Terrain\Make sector(s) invisible";
    static string VA_ENABLE_COLLISION = @"Terrain\Enable sector collision";
    static string VA_DISABLE_COLLISION = @"Terrain\Disable sector collision";

    public override void Get2DViewActions(Scene2DView view, Scene2DActions actions)
    {
      base.Get2DViewActions(view, actions);
      if (!view.TouchesSelectionMarquee(SectorExtent))
        return;
      // commented out until supported
      //actions.AddAction(this, VA_UNLOCK_SECTORS);
      if (SectorVisibleMask == 0)
        actions.AddAction(this, VA_MAKE_VISIBLE);
      else
        actions.AddAction(this, VA_MAKE_INVISIBLE);
      if (SectorCollisionMask == 0)
        actions.AddAction(this, VA_ENABLE_COLLISION);
      else
        actions.AddAction(this, VA_DISABLE_COLLISION);
    }


    public override void Perform2DViewAction(Scene2DView view, GroupAction parent, string action)
    {
      base.Perform2DViewAction(view, parent, action);
      if (action == VA_UNLOCK_SECTORS)
      {
        // do something here...
      }
      if (action == VA_MAKE_VISIBLE)
      {
        parent.Add(new SetSectorPropertyAction(this, "SectorVisibleMask", FlagsInt32_e.All));
      }
      else if (action == VA_MAKE_INVISIBLE)
      {
        parent.Add(new SetSectorPropertyAction(this, "SectorVisibleMask", FlagsInt32_e.None));
      }
      else if (action == VA_ENABLE_COLLISION)
      {
        parent.Add(new SetSectorPropertyAction(this, "SectorCollisionMask", FlagsInt16_e.All));
      }
      else if (action == VA_DISABLE_COLLISION)
      {
        parent.Add(new SetSectorPropertyAction(this, "SectorCollisionMask", FlagsInt16_e.None));
      }

    }

    #endregion


    #region ITerrainObject Members

    public TerrainShape OwnerTerrain
    {
      get { return _ownerTerrain; }
    }

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
