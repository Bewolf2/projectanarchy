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
using CSharpFramework.Scene;
using System.ComponentModel;
using System.Drawing;
using CSharpFramework;
using CSharpFramework.Math;

namespace TerrainBase.Editing
{
  /// <summary>
  /// Interface for importing a heightmap. Special case of a terrain filter
  /// </summary>
  public abstract class IHeightmapImporter : IHeightmapFilter
  {
    public enum StretchMode_e
    {
      Canvas = 0, // only import the source size
      Stretch = 1, // stretch the source so it fits onto the terrain
      Repeat = 2, // repeat the source terrain
    }

    [Browsable(false)]
    public virtual string[] SupportedFileExtensions { get { return null; } }

    /// <summary>
    /// Custom filter property
    /// </summary>
    [SortedCategory("File", 1), PropertyOrder(1)]
    [Description("Filename of the heightmap to import")]
    public virtual string FileName
    {
      get { return _FileName; }
      set { _FileName = value; }
    }

    /// <summary>
    /// Custom filter property
    /// </summary>
    [SortedCategory("Import", 2), PropertyOrder(1)]
    [Description("Defines the vertical scaling factor to map source values to world units")]
    public virtual float HeightScaling
    {
      get
      {
        return _fScaling;
      }
      set
      {
        _fScaling = value;
      }
    }

    [SortedCategory("Import", 2), PropertyOrder(10)]
    [Description("If enabled, the imported image will be flipped vertically")]
    public bool FlipVertically
    {
      get
      {
        return _bFlipV;
      }
      set
      {
        _bFlipV = value;
      }
    }

    [SortedCategory("Import", 2), PropertyOrder(11)]
    [Description("If enabled, the imported image will be flipped horizontally")]
    public bool FlipHorizontally
    {
      get
      {
        return _bFlipH;
      }
      set
      {
        _bFlipH = value;
      }
    }


    /// <summary>
    /// If the import format has information about the heightmap dimension, this value specifies the spacing between samples. Otherwise it is (0,0)
    /// </summary>
    [SortedCategory("Import", 2), PropertyOrder(3)]
    [Description("If the import format has information about the heightmap dimension, this value specifies the spacing between samples. Otherwise it is (0,0)")]
    public virtual Vector2F SuggestedSampleSpacing
    {
      get
      {
        return Vector2F.Zero;
      }
    }

    /// <summary>
    /// Custom filter property
    /// </summary>
    [SortedCategory("Import", 2), PropertyOrder(2)]
    [Description("Defines how the source data is mapped onto the terrain if the size differs")]
    public StretchMode_e StretchMode
    {
      get { return _stretchMode; }
      set { _stretchMode = value; }
    }

    /// <summary>
    /// Importer should implement this function as it retrieves the scaled values (StretchMode)
    /// </summary>
    /// <param name="x"></param>
    /// <param name="y"></param>
    /// <returns></returns>
    protected abstract float GetHeightAtInternal(int x, int y);

    [SortedCategory("File", 1), PropertyOrder(10)]
    [Description("Width of the imported file in heightmap samples")]
    public abstract int SizeX { get;}
    [SortedCategory("File", 1), PropertyOrder(11)]
    [Description("Height of the imported file in heightmap samples")]
    public abstract int SizeY { get;}


    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="originalValues"></param>
    /// <param name="x1"></param>
    /// <param name="y1"></param>
    /// <param name="x2"></param>
    /// <param name="y2"></param>
    /// <returns></returns>
    public override bool OnStart(IValueProvider originalValues, int x1, int y1, int x2, int y2)
    {
      if (!base.OnStart(originalValues, x1, y1, x2, y2))
        return false;

      if (StretchMode == StretchMode_e.Canvas)
      {
        _x2 = Math.Min(X2, SizeX - 1);
        _y2 = Math.Min(Y2, SizeY - 1);
      }
      else if (StretchMode == StretchMode_e.Stretch)
      {
        _fScaleX = (float)this.SizeX / (float)originalValues.SizeX;
        _fScaleY = (float)this.SizeY / (float)originalValues.SizeY;
      }
      return true;
    }


    /// <summary>
    /// Implements the height value function and interpolates
    /// </summary>
    /// <param name="x"></param>
    /// <param name="y"></param>
    /// <returns></returns>
    public override float GetHeightAt(int x, int y)
    {
      if (FlipHorizontally)
        x = X2 - 1 - x;
      if (!FlipVertically)
        y = Y2 - 1 - y;

      switch (StretchMode)
      {
        case StretchMode_e.Canvas:
          return GetHeightAtInternal(x, y);
        case StretchMode_e.Repeat:
          return GetHeightAtInternal(x % SizeX, y % SizeY);
      }

      // stretch mode:
      float fx = (((float)x + 0.5f) * _fScaleX) - 0.5f;
      float fy = (((float)y + 0.5f) * _fScaleY) - 0.5f;
      int ix = (int)Math.Floor(fx);
      int iy = (int)Math.Floor(fy);
      float h00 = GetHeightAtInternal(ix, iy);
      float h01 = GetHeightAtInternal(ix, iy+1);
      float h10 = GetHeightAtInternal(ix + 1, iy);
      float h11 = GetHeightAtInternal(ix + 1, iy + 1);
      float dx = fx - (float)ix;
      float dy = fy - (float)iy;

      // bi-lerp
      float h = (h00 * (1.0f - dx) + h10 * dx) *(1.0f-dy) + (h01 * (1.0f - dx) + h11 * dx) * dy;
      return h;
    }

    public override void ResetParameter()
    {
      base.ResetParameter();
      _fScaling = 0.1f;
      _stretchMode = StretchMode_e.Stretch;
    }

    protected string _FileName = null;
    protected float _fScaling = 0.1f;
    protected bool _bFlipV = false;
    protected bool _bFlipH = false;
    StretchMode_e _stretchMode = StretchMode_e.Stretch;
    float _fScaleX = 0.0f;
    float _fScaleY = 0.0f; 
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
