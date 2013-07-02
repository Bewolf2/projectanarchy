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
using CSharpFramework;
using CSharpFramework.Math;
using CSharpFramework.Scene;
using CSharpFramework.Actions;
using CSharpFramework.Dialogs;
using CSharpFramework.PropertyEditors;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Design;
using System.Runtime.Serialization;

namespace Editor.Actions
{
  #region ZoneGridProperties

  public class ZoneGridProperties : ICloneable
  {
    #region Properties

    int _iSubDivX = 4;
    int _iSubDivY = 4;
    BoundingBox _area = new BoundingBox(-10000, -10000, -10000, 10000, 10000, 10000);
    string _zoneNames = "GridZone";
    string _layerNames = "GridLayer";
    float _fLoadedRadius = -1;
    float _fCacheInMargin = 1000.0f;
    float _fCacheOutMargin = 1000.0f;
    bool _bCreateLoaded = false;
    Color _color = Color.Aquamarine;

    [Browsable(false)]
    public BoundingBox Area
    {
      get { return _area; }
      set 
      { 
        _area = value;
        _area.vMin.Z = Math.Min(_area.vMin.Z, -1000000.0f);
        _area.vMax.Z = Math.Max(_area.vMax.Z, 1000000.0f);

      }
    }

    [Description("Position of the first zone (min value of the scene extent bounding box)")]
    [SortedCategory("Size and subdivision", 1), PropertyOrder(1)]
    public Vector3F AreaStart
    {
      get { return _area.vMin; }
      set { _area.vMin = value; }
    }

    [Description("Position of the last zone (max value of the scene extent bounding box)")]
    [SortedCategory("Size and subdivision", 1), PropertyOrder(2)]
    public Vector3F AreaEnd
    {
      get { return _area.vMax; }
      set { _area.vMax = value; }
    }

    [Description("Number of zone subdivisions in x-direction")]
    [SortedCategory("Size and subdivision", 1), PropertyOrder(3)]
    public int SubdivisionsX
    {
      get { return _iSubDivX; }
      set 
      { 
        _iSubDivX = value;
        if (_iSubDivX < 2) _iSubDivX = 2;
        if (_iSubDivX > 100) _iSubDivX = 100;
      }
    }

    [Description("Number of zone subdivisions in y-direction")]
    [SortedCategory("Size and subdivision", 1), PropertyOrder(4)]
    public int SubdivisionsY
    {
      get { return _iSubDivY; }
      set
      {
        _iSubDivY = value;
        if (_iSubDivY < 2) _iSubDivY = 2;
        if (_iSubDivY > 100) _iSubDivY = 100;
      }
    }

    [Description("Default loaded radius for all zones (can be changed later)")]
    [SortedCategory("Zone defaults", 2), PropertyOrder(1)]
    public float LoadedRadius
    {
      get { return _fLoadedRadius; }
      set { _fLoadedRadius = value; }
    }

    [Description("Default cache-in radius for all zones (can be changed later)")]
    [SortedCategory("Zone defaults", 2), PropertyOrder(2)]
    public float CacheInMargin
    {
      get { return _fCacheInMargin; }
      set { _fCacheInMargin = value; }
    }

    [Description("Default cache-out radius for all zones (can be changed later)")]
    [SortedCategory("Zone defaults", 2), PropertyOrder(3)]
    public float CacheOutMargin
    {
      get { return _fCacheOutMargin; }
      set { _fCacheOutMargin = value; }
    }

    [Description("Color of the zones in the grid (2D view)"),
	EditorAttribute(typeof(StandardColorDropDownEditor), typeof(UITypeEditor))]
    [SortedCategory("Zone defaults", 2), PropertyOrder(10)]
    public Color ZoneColors
    {
      get { return _color; }
      set { _color = value; }
    }

    [Description("Name of the zones. The individual zone name will be <name>_xx_yy.")]
    [SortedCategory("Naming convenstion", 3), PropertyOrder(1)]
    public string ZoneNames
    {
      get { return _zoneNames; }
      set { _zoneNames = value; }
    }

    [Description("Name of the layer file that is attached to each zone. The individual layer name will be <name>_xx_yy.")]
    [SortedCategory("Naming convenstion", 3), PropertyOrder(2)]
    public string LayerNames
    {
      get { return _layerNames; }
      set { _layerNames = value; }
    }



    #endregion

    #region Create Helpers

    public Zone CreateNewZone(int iIndexX, int iIndexY)
    {
      string name = string.Format("{0}_{1}_{2}", ZoneNames, iIndexX, iIndexY);
      Zone z = new GridZone(name, iIndexX, iIndexY);
      z.LoadedDistance = LoadedRadius;
      z.CacheInMargin = CacheInMargin;
      z.CacheOutMargin = CacheOutMargin;

      Vector3F vStep = new Vector3F(_area.SizeX/(float)SubdivisionsX,_area.SizeY/(float)SubdivisionsY,_area.SizeZ);
      Vector3F vOrigin = _area.vMin + new Vector3F(vStep.X*(float)iIndexX,vStep.Y*(float)iIndexY,0.0f);
      BoundingBox bbox = new BoundingBox(vOrigin, vOrigin + vStep);
      z.AssignmentBoundingBox = bbox;
      z.DebugColor = ZoneColors;
      return z;
    }

    public Layer CreateNewLayer(int iIndexX, int iIndexY)
    {
      string name = string.Format("{0}_{1}_{2}", LayerNames, iIndexX, iIndexY);
      Object[] parameters = {name};
      Layer l = EditorManager.LayerFactory.CreateInstance(parameters);
      return l;
    }


    public bool CreateGrid(GroupAction parent)
    {
      if (!_area.Valid)
        return false;
      for (int y = 0; y < SubdivisionsY; y++)
      {
        for (int x = 0; x < SubdivisionsX; x++)
        {
          Zone z = CreateNewZone(x, y);
          Layer l = CreateNewLayer(x, y);
          parent.Add(new AddZoneAction(z, false));
          l.AttachToZone(z);
          parent.Add(new AddLayerAction(l, false));
          if (_bCreateLoaded != z.Loaded)
            parent.Add(new SetZoneLoadedStatusAction(z, _bCreateLoaded));
        }
      }
      return true;
    }

    #endregion


    #region ICloneable Members

    public object Clone()
    {
      ZoneGridProperties copy = (ZoneGridProperties)this.MemberwiseClone();
      copy._area = (BoundingBox)_area.Clone();
      return copy;
    }

    #endregion
  }

  #endregion

  #region class CreateZoneGridAction

  [Serializable]
  public class CreateZoneGridAction : GroupAction
  {
    public CreateZoneGridAction(ZoneGridProperties properties)
      : base("Create zone grid")
    {
      properties.CreateGrid(this);
    }

    bool _bForceInvalid = false;

    public override bool  Valid
    {
      get 
      {
        if (_bForceInvalid)
          return false;
	      return base.Valid;
      }
    }

    #region ISerializable Members

    protected CreateZoneGridAction(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
    }

    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);
    }

    #endregion
  }
  
  #endregion

  #region class ZoneActionProvider

  public class ZoneActionProvider : IScene2DViewRenderable
  {
    public static ZoneActionProvider ACTION_PROVIDER = new ZoneActionProvider();

    #region IScene2DViewRenderable Members

    public float GetScene2DRenderingSortingKey(CSharpFramework.Controls.Scene2DView view)
    {
      return 0;
    }

    public void OnPaintScene2DView(CSharpFramework.Controls.Scene2DView view, PaintEventArgs e)
    {
    }

    public BoundingBox GetAbsoluteBoundingBox(CSharpFramework.Controls.Scene2DView view)
    {
      return null;
    }

    public Scene2DViewFilterCategory GetScene2DRenderingFilter(CSharpFramework.Controls.Scene2DView view)
    {
      return null;
    }


    static string RO_CREATEZONEGRID = @"Zones\Create Zone Grid";

    public void Get2DViewActions(CSharpFramework.Controls.Scene2DView view, Scene2DActions actions)
    {
      if (view.HasSelectionMarquee)
        actions.AddAction(this, RO_CREATEZONEGRID);
    }

    public void Perform2DViewAction(CSharpFramework.Controls.Scene2DView view, GroupAction parent, string action)
    {
      if (action == RO_CREATEZONEGRID)
      {
        ZoneGridProperties properties = new ZoneGridProperties();
        properties.Area = view.SelectionMarqueeWorldBox;

        using (PropertyGridDlg dlg = new PropertyGridDlg("Create a grid of zones", "Procedurally generate a grid of m times n streaming zones"))
        {
          dlg.DataObjectNoClone = properties;
          if (dlg.ShowDialog() != DialogResult.OK)
            return;
          CreateZoneGridAction createaction = new CreateZoneGridAction(properties);
          EditorManager.Actions.Add(createaction);
        }
      }
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
