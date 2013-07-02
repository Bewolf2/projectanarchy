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

namespace TerrainEditorPlugin.Filter
{
  #region HeightmapClearFilter

  public class HeightmapClearFilter : IHeightmapFilter
  {
    public override string Name
    {
      get { return "Clear"; }
    }

    public override float GetHeightAt(int x, int y)
    {
      return _fDestHeight;
    }

    public override void ResetParameter()
    {
      base.ResetParameter();
      _fDestHeight = 0.0f;
    }

    float _fDestHeight = 0.0f;

    [Description("Specifies the the height (in world units) the height map should be set")]
    public float ClearHeight
    {
      get { return _fDestHeight; }
      set { _fDestHeight = value; }
    }

  }

  #endregion

  #region HeightmapScaleElevateFilter

  public class HeightmapScaleElevateFilter : IHeightmapFilter
  {
    public override string Name
    {
      get { return "Scale and elevate"; }
    }

    public override float GetHeightAt(int x, int y)
    {
      return this._originalValues.ValueAt(x, y) * _fScale + _fDelta;
    }

    public override bool Valid
    {
      get
      {
        return base.Valid && (_fDelta!=0.0f || _fScale!=1.0f);
      }
    }

    public override void ResetParameter()
    {
      base.ResetParameter();
      _fDelta = 0.0f;
      _fScale = 1.0f;
    }

    float _fDelta = 0.0f;
    float _fScale = 1.0f;

    [Description("Specifies the number of units the terrain should be elevated up or down")]
    public float HeightDelta
    {
      get { return _fDelta; }
      set { _fDelta = value; }
    }

    [Description("Specifies the scaling factor for original height values")]
    public float HeightScale
    {
      get { return _fScale; }
      set { _fScale = value; }
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
