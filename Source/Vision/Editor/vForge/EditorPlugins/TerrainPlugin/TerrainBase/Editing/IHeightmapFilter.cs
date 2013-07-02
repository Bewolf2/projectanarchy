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
using System.Windows.Forms;

namespace TerrainBase.Editing
{
  public abstract class IHeightmapFilter : ITerrainFilter
  {
    /// <summary>
    /// Constructor; calls ResetParameter()
    /// </summary>
    public IHeightmapFilter()
    {
      ResetParameter();
    }



    /// <summary>
    /// Called before running the filter. Sets up the range stored in X1,Y1,X2,Y2
    /// </summary>
    /// <param name="originalValues"></param>
    /// <param name="x1"></param>
    /// <param name="y1"></param>
    /// <param name="x2"></param>
    /// <param name="y2"></param>
    /// <returns></returns>
    public virtual bool OnStart(IValueProvider originalValues, int x1, int y1, int x2, int y2) 
    {
      _originalValues = originalValues;
      _x1 = x1;
      _y1 = y1;
      _x2 = x2;
      _y2 = y2;
      if (_x2 < 0) _x2 = _originalValues.SizeX;
      if (_y2 < 0) _y2 = _originalValues.SizeY;
      return true;
    }

    /// <summary>
    /// Called after running the filter (also when failed)
    /// </summary>
    public virtual void OnFinish()
    {
    }

    /// <summary>
    /// Filter Range
    /// </summary>
    [Browsable(false)]
    public int X1 { get { return _x1; } }
    [Browsable(false)]
    public int Y1 { get { return _y1; } }
    [Browsable(false)]
    public int X2 { get { return _x2; } }
    [Browsable(false)]
    public int Y2 { get { return _y2; } }

    protected IValueProvider _originalValues; 
    protected int _x1 = 0;
    protected int _y1 = 0;
    protected int _x2 = -1;
    protected int _y2 = -1;

    /// <summary>
    /// This is the key function of the filter class. Return the processed value for specified sample position here
    /// </summary>
    /// <param name="x"></param>
    /// <param name="y"></param>
    /// <returns></returns>
    public abstract float GetHeightAt(int x, int y);



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
