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
using TerrainBase.Editing;
using System.ComponentModel;
using TerrainEditorPlugin.Editing;
using System.Drawing.Design;
using TerrainEditorPlugin.Shapes;
using CSharpFramework.PropertyEditors;
using ManagedBase;
using System.Drawing;

namespace TerrainEditorPlugin.Filter
{
  #region Clear decoration

  class DecorationClearFilter : IDecorationFilter
  {
    public override string Name
    {
      get { return "Clear"; }
    }

    public override float GetDensityAt(int x, int y)
    {
      return _fClearValue;
    }

    [Browsable(false)]
    public override bool HasCustomProperties
    {
      get
      {
        return true;
      }
    }

    float _fClearValue = 0.0f;


    [Description("Value (0..1) used to clear the density")]
    public float ClearValue
    {
      get { return _fClearValue; }
      set { _fClearValue = value; }
    }
  }

  #endregion

  #region Decoration from texture

  class DecorationFromDetailTextureFilter : IDecorationFilter, ITerrainObject
  {
    public override string Name
    {
      get { return "From detail texture"; }
    }

    public override float GetDensityAt(int x, int y)
    {
      return OwnerTerrain.EngineTerrain.GetDetailTextureDensity(_sourceTexture, x, y, DecorationModel.iDensityResX, DecorationModel.iDensityResY) * _fScaling;
    }

    [Browsable(false)]
    public override bool HasCustomProperties
    {
      get
      {
        return true;
      }
    }

    public override bool Valid
    {
      get
      {
        return base.Valid && _sourceTexture != null && OwnerTerrain != null;
      }
    }

    float _fScaling = 1.0f;
    DetailTextureResource _sourceTexture = null;

    [Description("The detail texture to use the density for")]
    [EditorAttribute(typeof(DetailTextureComboBoxEditor), typeof(UITypeEditor))]
    public DetailTextureResource SourceTexture
    {
      get { return _sourceTexture; }
      set { _sourceTexture = value;}
    }


    [Description("Scale value to multiply the detail texture's density")]
    public float Scaling
    {
      get { return _fScaling; }
      set { _fScaling = value; }
    }

    public override bool OnStart()
    {
      if (!base.OnStart())
        return false;
      TerrainShape terrain = OwnerTerrain;
      if (terrain == null || terrain.EngineTerrain==null || _sourceTexture == null || DecorationModel==null)
        return false;
      if (!terrain.EngineTerrain.GetDecorationModelDensityResolution(this.DecorationModel))
        return false;
      return true;
    }



    #region ITerrainObject Members

    [Browsable(false)]
    public TerrainShape OwnerTerrain
    {
      get { return TerrainEditor.CurrentTerrain; }
    }

    #endregion
  }

  #endregion

  #region Decoration Import from luminance

  class DecorationImportFromLuminanceFilter : IDecorationFilter, ITerrainObject
  {
    public override string Name
    {
      get { return "Import from luminance texture"; }
    }

    public override float GetDensityAt(int x, int y)
    {
      if (_bitmap != null)
      {
        int iX = x * _bitmap.Width / iSampleCountX;
        int iY = y * _bitmap.Height / iSampleCountY;
        if (iX < 0) iX = 0; else if (iX >= _bitmap.Width) iX = _bitmap.Width - 1;
        if (iY < 0) iY = 0; else if (iY >= _bitmap.Height) iY = _bitmap.Height - 1;
        Color luminance = _bitmap.GetPixel(iX, iY);
        float fVal = (luminance.R + luminance.G + luminance.B) * (1.0f / 765.0f);
        return fVal * _fScaling;
      }
      return 0.0f;
    }

    [Browsable(false)]
    public override bool HasCustomProperties
    {
      get
      {
        return true;
      }
    }

    public override bool Valid
    {
      get
      {
        return base.Valid && !string.IsNullOrEmpty(_luminanceFilename) && OwnerTerrain != null;
      }
    }

    float _fScaling = 1.0f;
    string _luminanceFilename = null;
    Bitmap _bitmap;

    [Description("The luminance texture that should be imported as vegetation density")]
    [EditorAttribute(typeof(BitmapBrowserEditor), typeof(UITypeEditor))]
    public string LuminanceTexture
    {
      get { return _luminanceFilename; }
      set { _luminanceFilename = value; }
    }

    [Description("Scale value to multiply the density (1.0 is default)")]
    public float Scaling
    {
      get { return _fScaling; }
      set { _fScaling = value; }
    }

    int iSampleCountX = 1;
    int iSampleCountY = 1;

    public override bool OnStart()
    {
      if (!base.OnStart())
        return false;
      TerrainShape terrain = OwnerTerrain;
      if (terrain == null || terrain.EngineTerrain == null || DecorationModel == null)
        return false;
      if (!terrain.EngineTerrain.GetDecorationModelDensityResolution(DecorationModel))
        return false;
      iSampleCountX = DecorationModel.iDensityResX * terrain.Config.SectorCountX;
      iSampleCountY = DecorationModel.iDensityResY * terrain.Config.SectorCountY;
      string info;
      _bitmap = ImageLoader.LoadImage(_luminanceFilename, out info) as Bitmap;
      _bitmap.RotateFlip(RotateFlipType.RotateNoneFlipY);
      return _bitmap != null;
    }

    public override void OnFinish()
    {
      base.OnFinish();
      _bitmap = null;
    }

    #region ITerrainObject Members

    [Browsable(false)]
    public TerrainShape OwnerTerrain
    {
      get { return TerrainEditor.CurrentTerrain; }
    }

    #endregion
  }

  #endregion

  #region Decoration from slope

  class DecorationFromSlopeFilter : IDecorationFilter, ITerrainObject
  {
    public override string Name
    {
      get { return "From slope"; }
    }

    public override float GetDensityAt(int x, int y)
    {
      float fNrmlZ = OwnerTerrain.EngineTerrain.GetAverageNormalZ(x, y, DecorationModel.iDensityResX, DecorationModel.iDensityResY);
      if (fNrmlZ < _fMaxAngleCos || fNrmlZ > _fMinAngleCos) // note that this is vice versa for cos
        return 0.0f;

      float fSmooth = 0.0f;
      if (fInvRange > 0.0f && fSmooth>0.0f)
      {
        fNrmlZ = (fNrmlZ - _fMaxAngleCos) * fInvRange;
        if (fNrmlZ < fSmooth)
        {
          float f = (float)(0.5 - 0.5 * Math.Cos(fNrmlZ / fSmooth * Math.PI));
          return f * _fMaxDensity;
        }
        if (fNrmlZ > (1.0f-fSmooth))
        {
          float f = (float)(0.5 + 0.5 * Math.Cos((fNrmlZ-(1.0f-fSmooth)) / fSmooth * Math.PI));
          return f * _fMaxDensity;
        }
      }

      return _fMaxDensity;
    }

    [Browsable(false)]
    public override bool HasCustomProperties
    {
      get
      {
        return true;
      }
    }

    public override bool Valid
    {
      get
      {
        return base.Valid && OwnerTerrain != null;
      }
    }

    float _fMaxDensity = 1.0f;
    float _fMinAngle = 0.0f;
    float _fMaxAngle = 10.0f;
    float _fMinAngleCos = 0.0f;
    float _fMaxAngleCos = 0.0f;
    float fInvRange = 0.0f;

    [Description("The maximum density value")]
    public float MaxDensity
    {
      get { return _fMaxDensity; }
      set { _fMaxDensity = value; }
    }


    [Description("The minimum terrain slope angle (in degrees). Slopes below this angle will not receive decoration")]
    public float MinAngle
    {
      get { return _fMinAngle; }
      set 
      { 
        _fMinAngle = value;
        if (_fMinAngle < 0.0f)
          _fMinAngle = 0.0f;
        if (_fMinAngle > 90.0f)
          _fMinAngle = 90.0f;
      }
    }

    [Description("The maximum terrain slope angle (in degrees). Slopes larger than this angle will not receive decoration")]
    public float MaxAngle
    {
      get { return _fMaxAngle; }
      set
      {
        _fMaxAngle = value;
        if (_fMaxAngle < 0.0f)
          _fMaxAngle = 0.0f;
        if (_fMaxAngle > 90.0f)
          _fMaxAngle = 90.0f;
      }
    }
    public override bool OnStart()
    {
      if (!base.OnStart())
        return false;
      TerrainShape terrain = OwnerTerrain;
      if (terrain == null || terrain.EngineTerrain == null || DecorationModel == null)
        return false;
      if (!terrain.EngineTerrain.GetDecorationModelDensityResolution(this.DecorationModel))
        return false;

      _fMinAngleCos = (float)Math.Cos(_fMinAngle * Math.PI / 180.0f);
      _fMaxAngleCos = (float)Math.Cos(_fMaxAngle * Math.PI / 180.0f);
      fInvRange = _fMinAngleCos - _fMaxAngleCos;
      if (fInvRange > 0.0f)
        fInvRange = 1.0f / fInvRange;

      return true;
    }



    #region ITerrainObject Members

    [Browsable(false)]
    public TerrainShape OwnerTerrain
    {
      get { return TerrainEditor.CurrentTerrain; }
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
