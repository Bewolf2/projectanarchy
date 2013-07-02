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
using CSharpFramework;
using CSharpFramework.Math;
using CSharpFramework.View;
using System.Drawing;
using System.ComponentModel;
using CSharpFramework.PropertyEditors;
using System.Drawing.Design;
using System.Runtime.Serialization;

namespace TerrainEditorPlugin.Shapes
{
  [Serializable]
  [ShapeSearchableAttribute(false)]
  public class TerrainDirectionalLight : Shape3D
  {
    public TerrainDirectionalLight(string name)
      : base(name)
    {
      AddHint(HintFlags_e.Invisible);
      //AddHint(HintFlags_e.NonSerialized);
      AddHint(HintFlags_e.NonRemovable);
      AddHint(HintFlags_e.NoChildren);

      //AddHint(HintFlags_e.Virtual); // still modifiable

      // default orientation of the light
      this.Yaw = 209.0f;
      this.Pitch = 31.0f;
      this.Roll = 196.0f;
    }

    #region Shape Overrides

    public override bool GetLocalBoundingBox(ref CSharpFramework.Math.BoundingBox bbox)
    {
      float d = 50.0f * EditorManager.Settings.GlobalUnitScaling;
      bbox = new BoundingBox(-d, -d, -d, d, d, d);
      return true;
    }

    public override void RenderShape(CSharpFramework.VisionViewBase view, ShapeRenderMode mode)
    {
      base.RenderShape(view, mode);
      float fScale = EditorManager.Settings.GlobalUnitScaling;
      if (mode == ShapeRenderMode.Selected)
        view.RenderArrow(RenderPosition, RenderPosition + this.XAxis * 500.0f * fScale, VisionColors.White, 1.0f, 10.0f * fScale);
    }
    public override void OnTransformationChanged()
    {
      base.OnTransformationChanged();
      UpdateLightProperties(); // rotation is the direction
    }

    public override void OnAddedToScene()
    {
      base.OnAddedToScene();
      UpdateLightProperties();
    }

    public override bool CanCopyPaste
    {
      get
      {
        return false;
      }
    }


    public override bool Traceable
    {
      get
      {
        return false;
      }
    }

    #endregion

    #region Selection & Hotspots

    HotSpotColorButton _hotSpotDirColor; // a color button
    HotSpotColorButton _hotSpotAmbColor; // a color button

    public override void OnSelected()
    {
      base.OnSelected();

      //color hotspots
      _hotSpotAmbColor = new HotSpotColorButton(this, "AmbientColor");
      _hotSpotAmbColor.ToolTipText = "Change the ambient color";
      _hotSpotAmbColor.Set2DOffset(10.0f, 10.0f);
      EditorManager.ActiveView.HotSpots.Add(_hotSpotAmbColor);

      _hotSpotDirColor = new HotSpotColorButton(this, "DirectionalLightColor");
      _hotSpotDirColor.ToolTipText = "Change the light's color";
      _hotSpotDirColor.Set2DOffset(10.0f, 24.0f);
      EditorManager.ActiveView.HotSpots.Add(_hotSpotDirColor);

    }

    public override void OnUnSelected()
    {
      base.OnUnSelected();
      if (_hotSpotAmbColor != null)
      {
        EditorManager.ActiveView.HotSpots.Remove(_hotSpotAmbColor);
        _hotSpotAmbColor.Remove();
        _hotSpotAmbColor = null;
      }
      if (_hotSpotDirColor != null)
      {
        EditorManager.ActiveView.HotSpots.Remove(_hotSpotDirColor);
        _hotSpotDirColor.Remove();
        _hotSpotDirColor = null;
      }

    }

    #endregion

    void UpdateLightProperties()
    {
      if (OwnerTerrain == null || OwnerTerrain.EngineTerrain == null)
        return;
      OwnerTerrain.EngineTerrain.SetDirectionalLightProperties(XAxis, _ambColorDisplay, _dirColorDisplay);
    }

    #region Members & Properties

    Color _ambColor = Color.FromArgb(50, 50, 50);
    Color _dirColor = Color.FromArgb(210, 210, 210);
    Color _ambColorDisplay = Color.FromArgb(50, 50, 50);
    Color _dirColorDisplay = Color.FromArgb(210, 210, 210);

    [Description("Ambient color for directional lighting preview")]
    [SortedCategory("Directional Light", Shape3D.LAST_CATEGORY_ORDER_ID+1), PropertyOrder(1)]
    [EditorAttribute(typeof(StandardColorDropDownEditor), typeof(UITypeEditor))]
    [PropertyLiveUpdate("AmbientColorLiveUpdate")]
    public Color AmbientColor
    {
      get { return _ambColor; }
      set { _ambColor = _ambColorDisplay = value; UpdateLightProperties(); }
    }

    [Description("Light color for directional lighting preview")]
    [SortedCategory("Directional Light", Shape3D.LAST_CATEGORY_ORDER_ID + 1), PropertyOrder(2)]
    [EditorAttribute(typeof(StandardColorDropDownEditor), typeof(UITypeEditor))]
    [PropertyLiveUpdate("DirectionalLightColorLiveUpdate")]
    public Color DirectionalLightColor
    {
      get { return _dirColor; }
      set { _dirColor = _dirColorDisplay = value; UpdateLightProperties(); }
    }


    /// <summary>
    /// Proxy property used for color live update (See PropertyLiveUpdate attribute in LightColor property)
    /// </summary>
    [Browsable(false)]
    public object AmbientColorLiveUpdate
    {
      set
      {
        _ambColorDisplay = _ambColor;
        if (value is Color)
          _ambColorDisplay = (Color)value;
        UpdateLightProperties();
        EditorManager.ActiveView.UpdateView(false);
      }
    }

    /// <summary>
    /// Proxy property used for color live update (See PropertyLiveUpdate attribute in LightColor property)
    /// </summary>
    [Browsable(false)]
    public object DirectionalLightColorLiveUpdate
    {
      set
      {
        _dirColorDisplay = _dirColor;
        if (value is Color)
          _dirColorDisplay = (Color)value;
        UpdateLightProperties();
        EditorManager.ActiveView.UpdateView(false);
      }
    }
    public TerrainShape OwnerTerrain;

    #endregion

    #region ISerializable

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected TerrainDirectionalLight(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      _ambColor = (Color)info.GetValue("_ambColor", typeof(Color));
      _dirColor = (Color)info.GetValue("_dirColor", typeof(Color));
    }


    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context) 
    {
      base.GetObjectData(info,context);
      info.AddValue("_ambColor", _ambColor);
      info.AddValue("_dirColor", _dirColor);
    }

    public override void OnDeserialization()
    {
      base.OnDeserialization();
      _ambColorDisplay = _ambColor;
      _dirColorDisplay = _dirColor;
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
