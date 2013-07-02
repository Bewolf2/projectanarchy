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
using System.ComponentModel;
using CSharpFramework;
using CSharpFramework.PropertyEditors;
using System.Drawing.Design;
using System.Runtime.Serialization;

namespace TerrainBase.Editing
{
  /// <summary>
  /// Base class for 3D terrain cursor properties
  /// </summary>
  [Serializable]
	public class Cursor3DProperties : ISerializable
	{
    public Cursor3DProperties()
    {
      _radius *= EditorManager.Settings.GlobalUnitScaling;
    }

    protected bool _enabled = true;
    protected float _radius = 1000;
    protected float _intensity = 10.0f;
    protected float _rotationAngle = 0.0f;

    bool _bActive = false;
    static public event EventHandler OnCursor3DPropertiesChanged = null;

    [Browsable(false)]
    public bool Active
    {
      get { return _bActive; }
      set { _bActive = value; }
    }

    public static void TriggerOnCursor3DPropertiesChanged()
    {
      if (OnCursor3DPropertiesChanged != null)
        OnCursor3DPropertiesChanged(null, EventArgs.Empty);
    }

    protected void TriggerUpdate()
    {
      if (!Active)
        return;
      TriggerOnCursor3DPropertiesChanged();
    }

    [Browsable(false)]
    public bool Enabled
    {
      get { return _enabled; }
      set 
      {
        if (_enabled == value)
          return;
        _enabled = value;
        TriggerUpdate();
      }
    }

    [SortedCategory("Brush Properties", 1), PropertyOrder(1)]
    [Description("The radius of the cursor in world units. Hold down 'B' key in th view to resize.")]
    public float Radius
    {
      get { return _radius; }
      set 
      {
        if (_radius == value)
          return;
        _radius = value;
        TriggerUpdate(); 
      }
    }

    [SortedCategory("Brush Properties", 1), PropertyOrder(2)]
    [Description("The brush strength for painting")]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.0f, 100.0f, 500)]
    public float Intensity
    {
      get { return _intensity; }
      set 
      {
        if (_intensity == value)
          return;
        _intensity = value;
        TriggerUpdate();
      }
    }

    [SortedCategory("Brush Properties", 1), PropertyOrder(3)]
    [Description("Rotation of the brush in degrees")]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.0f, 360.0f, 360)]
    public float Rotation
    {
      get { return _rotationAngle; }
      set
      {
        if (_rotationAngle == value)
          return;
        _rotationAngle = value;
        TriggerUpdate();
      }
    }

    #region ISerializable Members

    protected Cursor3DProperties(SerializationInfo info, StreamingContext context)
    {
      _enabled = info.GetBoolean("_enabled");
      _radius = info.GetSingle("_radius");
      _intensity = info.GetSingle("_intensity");
      _rotationAngle = info.GetSingle("_rotationAngle");
  }

    public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      info.AddValue("_enabled", _enabled);
      info.AddValue("_radius", _radius);
      info.AddValue("_intensity", _intensity);
      info.AddValue("_rotationAngle", _rotationAngle);
    }

    #endregion
  }

  /// <summary>
  /// Derived cursor properties with heightmap specific properties
  /// </summary>
  [Serializable]
  public class HeightmapCursorProperties : Cursor3DProperties
  {
    public HeightmapCursorProperties()
    {
    }

    protected float _height = 0.0f;

    [SortedCategory("Brush Properties", 1), PropertyOrder(10)]
    [Description("The absolute height for flattening mode")]
    public float AbsoluteHeight
    {
      get { return _height; }
      set 
      {
        if (_height == value)
          return;
        _height = value;
        TriggerUpdate();
      }
    }

    #region ISerializable Members

    protected HeightmapCursorProperties(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      _height = info.GetSingle("_height");
  }

    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);
      info.AddValue("_height", _height);
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
