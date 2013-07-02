/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using CSharpFramework.Math;

namespace VisionEditorPlugin.Tools
{
	/// <summary>
	/// An instance of this class defines relevant parameters for generating a color matrix
	/// </summary>
	public class ColorMatrixParams : ICloneable
	{
    /// <summary>
    /// Constructor
    /// </summary>
		public ColorMatrixParams()
		{
      Reset();
		}

    float _fBrightness = 0.0f;
    float _fContrast = 1.0f;
    float _fSaturation = 1.0f;
    Matrix4F _mat = new Matrix4F();
    bool _bHasChanged = false;

    /// <summary>
    /// Gets or sets the dirty flag
    /// </summary>
    public bool Dirty
    {
      get {return _bHasChanged;}
      set {_bHasChanged=value;}
    }

    #region Brightness

    /// <summary>
    /// Gets or sets the brightness
    /// </summary>
    public float Brightness
    {
      get {return _fBrightness;}
      set {_bHasChanged|=_fBrightness!=value;_fBrightness=value;}
    }

    #endregion

    #region Contrast

    /// <summary>
    /// Gets or sets the contrast
    /// </summary>
    public float Contrast
    {
      get {return _fContrast;}
      set {_bHasChanged|=_fContrast!=value;_fContrast=value;}
    }

    #endregion

    #region Saturation

    /// <summary>
    /// Gets or sets the saturation
    /// </summary>
    public float Saturation
    {
      get {return _fSaturation;}
      set {_bHasChanged|=_fSaturation!=value;_fSaturation=value;}
    }

    #endregion

    #region Color matrix

    /// <summary>
    /// Accesses the color transformation matrix. This accessor calculates the 4x4 matrix from the properties
    /// </summary>
    public Matrix4F ColorMatrix
    {
      get 
      {
        _mat = Matrix4F.Identity;
        
        // apply saturation
        const float lumR = 0.3086f;
        const float lumG = 0.6094f;
        const float lumB = 0.0820f;

        float satCompl = 1.0f - _fSaturation;
        float satComplR = lumR * satCompl;
        float satComplG = lumG * satCompl;
        float satComplB = lumB * satCompl;

        _mat.M11 = satComplR + _fSaturation;
        _mat.M21 = satComplR;
        _mat.M31 = satComplR;
        _mat.M12 = satComplG;
        _mat.M22 = satComplG + _fSaturation;
        _mat.M32 = satComplG;
        _mat.M13 = satComplB;
        _mat.M23 = satComplB;	
        _mat.M33 = satComplB + _fSaturation;

        // apply brightness and contrast
        float fFactor = 0.5f * (1.0f - _fContrast); 
        float fBias = fFactor+_fBrightness;
        Matrix4F _temp = Matrix4F.Identity;
        _temp.TranslationVector += new Vector3F(fBias, fBias, fBias);
        Vector4F scaling = _temp.Scaling;
        scaling.X *= _fContrast;
        scaling.Y *= _fContrast;
        scaling.Z *= _fContrast;
        _temp.Scaling = scaling;

        return _mat*_temp; 
      }
    }

    #endregion

    #region Reset parameter

    /// <summary>
    /// Sets the defaults
    /// </summary>
    public void Reset()
    {
      Brightness = 0.0f;
      Contrast = 1.0f;
      Saturation = 1.0f;
    }

    #endregion

    #region ICloneable Members

    /// <summary>
    /// Clones the settings
    /// </summary>
    /// <returns></returns>
    public object Clone()
    {
      ColorMatrixParams copy = (ColorMatrixParams)this.MemberwiseClone();
      copy.Dirty = false;
      return copy;
    }

    #endregion
  }

  /// <summary>
  /// Structure that defines a lighting configuration
  /// </summary>
  public class LightingTweakParameter
  {
    /// <summary>
    /// lighting information available at all
    /// </summary>
    public bool bAvailable = false;

    /// <summary>
    /// Defines whether base lighting is available
    /// </summary>
    public bool bBaseLightingAvailable = false;

    /// <summary>
    /// Defines whether extra radiosity is available
    /// </summary>
    public bool bRadiosityAvailable = false;

    /// <summary>
    /// Parameter Matrix for base lighting
    /// </summary>
    public ColorMatrixParams BaseLightingParams = new ColorMatrixParams();

    /// <summary>
    /// Parameter Matrix for radiosity lighting
    /// </summary>
    public ColorMatrixParams RadiosityParams = new ColorMatrixParams();
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
