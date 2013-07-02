/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Drawing;
using System.Drawing.Design;
using CSharpFramework;
using System.ComponentModel;
using CSharpFramework.Math;
using CSharpFramework.UndoRedo;
using CSharpFramework.Scene;
using CSharpFramework.Serialization;
using CSharpFramework.PropertyEditors;
using System.Runtime.Serialization;

namespace Editor.VisionSpecific
{
	/// <summary>
	/// Configuration for depth fog
	/// </summary>
  [Serializable]
  [TypeConverter(typeof(UndoableObjectConverter))]
  public class DepthFogConfig : ISceneObject, ISerializable
	{
    /// <summary>
    /// Constructor
    /// </summary>
		public DepthFogConfig()
		{
		}

    /// <summary>
    /// Updates the fog in the engine. Optionally redraws the view
    /// </summary>
    /// <param name="bRedraw"></param>
    public void Update(bool bRedraw)
    {
      EditorManager.EngineManager.SetLinearDepthFog(_bEnabled,_fStartDist,_fStartEnd,VisionColors.Get(_color));
      if (bRedraw)
        EditorManager.ActiveView.UpdateView(false);

      // update the dependedncies
      V3DLayer parentLayer = (V3DLayer)OwnerSceneObject;
      if (parentLayer != null)
      {
        // update the current renderer
        //parentLayer.Renderer.Update();
      }
    }

    /// <summary>
    /// The object this config is attached to
    /// </summary>
    public ISceneObject OwnerSceneObject = null;

    // properties
    bool _bEnabled = true;
    float _fStartDist = 1000.0f;
    float _fStartEnd = 5000.0f; // typo in this variable that isn't fixed because of serialization
    Color _color = Color.Black;

    /// <summary>
    /// Enables or disables the depth fog
    /// </summary>
    [Description("Enables or disables the depth fog")]
    [PropertyOrder(1),
    DefaultValue(true)]
    public bool Enabled
    {
      get {return _bEnabled;}
      set 
      {
        _bEnabled=value;
        Modified = true;
        Update(true);
      }
    }

    /// <summary>
    /// Sets the start distance (world units) for the linear depth fog 
    /// </summary>
    [Description("Sets the start distance (world units) for the linear depth fog")]
    [PropertyOrder(2),
    DefaultValue(1000.0f)]
    public float StartDistance
    {
      get {return _fStartDist;}
      set 
      {
        _fStartDist=value;
        if (_fStartDist>_fStartEnd-1.0f)
          _fStartDist=_fStartEnd-1.0f;
        Modified = true;
        Update(true);
      }
    }

    /// <summary>
    /// Sets the end distance (world units) for the linear depth fog 
    /// </summary>
    [Description("Sets the end distance (world units) for the linear depth fog")]
    [PropertyOrder(3),
    DefaultValue(5000.0f)]
    public float EndDistance
    {
      get {return _fStartEnd;}
      set 
      {
        _fStartEnd=value;
        if (_fStartEnd<_fStartDist+1.0f)
          _fStartEnd=_fStartDist+1.0f;
        Modified = true;
        Update(true);
      }
    }


    /// <summary>
    /// Get or set the color of the fog
    /// </summary>
    [Description("The color of the fog")]
	  [EditorAttribute(typeof(StandardColorDropDownEditor), typeof(UITypeEditor))]
    [PropertyOrder(4),
    DefaultValue(typeof(Color), "0,0,0")]
    public Color FogColor
    {
      get {return _color;}
      set 
      {
        _color=value;
        Modified = true;
        Update(true);
      }
    }

    #region ISceneObject Members

    /// <summary>
    /// Overridden function
    /// </summary>
    [Browsable(false)]
    public bool Modifiable
    {
      get
      {
        if (OwnerSceneObject!=null)
          return OwnerSceneObject.Modifiable;
        return true;
      }
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    public bool Modified
    {
      set
      {
        if (OwnerSceneObject!=null)
          OwnerSceneObject.Modified = value;
      }
    }

    #endregion

    #region ISerializable Members

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected DepthFogConfig(SerializationInfo info, StreamingContext context)
    {
      _bEnabled = info.GetBoolean("_bEnabled");
      _fStartDist = info.GetSingle("_fStartDist");
      _fStartEnd = info.GetSingle("_fStartEnd");
      _color = (Color)info.GetValue("_color", typeof(Color));
    }

    /// <summary>
    /// Called when de-serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      info.AddValue("_bEnabled", _bEnabled);
      info.AddValue("_fStartDist", _fStartDist);
      info.AddValue("_fStartEnd", _fStartEnd);
      info.AddValue("_color", _color);
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
