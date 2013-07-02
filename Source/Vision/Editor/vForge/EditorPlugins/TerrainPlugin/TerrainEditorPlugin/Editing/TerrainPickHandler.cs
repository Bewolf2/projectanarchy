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
using CSharpFramework.PickHandlers;
using TerrainEditorPlugin.Shapes;
using TerrainBase.Editing;
using CSharpFramework;
using System.Windows.Forms;
using CSharpFramework.Math;

namespace TerrainEditorPlugin.Editing
{
  /// <summary>
  /// Implements a pick handler that picks the absolute height in the current terrain
  /// </summary>
  public class TerrainPickHandler : IPickHandler
  {
    public TerrainPickHandler(TerrainShape terrain, HeightmapCursorProperties dest)
    {
      _terrain = terrain;
      _destHeight = dest;
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="e"></param>
    /// <param name="eKeyMod"></param>
    /// <returns></returns>
    public override bool OnPick(System.Windows.Forms.MouseEventArgs e, CSharpFramework.Contexts.KeyModifier eKeyMod)
    {
      base.OnPick(e, eKeyMod); // releases the handler
      if (_destHeight == null || _terrain == null)
        return false;

      Vector3F vStart = new Vector3F();
      Vector3F vEnd = new Vector3F();
      Vector3F touchPoint = new Vector3F();

      EditorManager.EngineManager.GetRayAtScreenPos(out vStart, out vEnd, e.X, e.Y, EditorManager.Settings.MaxPickingDistance);
      if (!_terrain.TraceTerrain(vStart, vEnd, ref touchPoint))
        return false;

      _destHeight.AbsoluteHeight = _terrain.TerrainHeightAtPosition(touchPoint);
      return true;
    }

    public override string ScreenMessage
    {
      get
      {
        return "Click on the terrain to select its absolute height for the terrain flatten mode";
      }
    }

    TerrainShape _terrain;
    HeightmapCursorProperties _destHeight;
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
