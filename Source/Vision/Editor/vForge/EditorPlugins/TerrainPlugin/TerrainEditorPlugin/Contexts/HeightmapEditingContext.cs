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
using CSharpFramework.Contexts;
using CSharpFramework;
using System.Windows.Forms;
using TerrainManaged;
using TerrainBase.Editing;
using CSharpFramework.Math;
using CSharpFramework.Scene;

namespace TerrainEditorPlugin.Contexts
{

  /// <summary>
  /// Mouse camera context for heightmap editing
  /// </summary>
	class HeightmapEditingContext : TerrainEditingBaseContext
	{


		HeightmapEditMode_e CurrentMode = HeightmapEditMode_e.None;

    public override void PreRenderTickFunction(float fTimeDelta)
		{
      base.PreRenderTickFunction(fTimeDelta);
			if (CurrentMode != HeightmapEditMode_e.None)
			{
				TerrainEditor.BlitHeightmapBrush(CurrentMode);
				View.UpdateView(false); // trigger next update
			}
		}

		public override bool OnMouseDown(System.Windows.Forms.MouseEventArgs e, CSharpFramework.Contexts.KeyModifier eKeyMod, int iOldX, int iOldY)
		{
      if ((IsEditingKeyPressed(eKeyMod) && (EditorSettingsBase.CameraStyle_e.MayaStyle != EditorManager.Settings.CameraStyle)) || eKeyMod == KeyModifier.Ctrl)
      {
        CurrentMode = TerrainEditor.HeightmapEditMode;

        // apply variants locally to this context
        if (CurrentMode == HeightmapEditMode_e.Elevate && (e.Button == MouseButtons.Right || (EditorManager.Tablet != null && EditorManager.Tablet.IsEraserPressed)))
          CurrentMode = HeightmapEditMode_e.Subtract;
        else if (CurrentMode == HeightmapEditMode_e.AddHoles && (e.Button == MouseButtons.Right || (EditorManager.Tablet != null && EditorManager.Tablet.IsEraserPressed)))
          CurrentMode = HeightmapEditMode_e.RemoveHoles;

        // in flatten mode with RMB we pick the height first
        if (CurrentMode == HeightmapEditMode_e.Flatten && e.Button == MouseButtons.Right)
        {
          Vector3F cursorCenter = TerrainEditor.CurrentTerrain.EngineTerrain.GetCurrentCursorCenter();
          float fHeight = TerrainEditor.CurrentTerrain.TerrainHeightAtPosition(cursorCenter);
          HeightmapCursorProperties cursorProp = TerrainEditor.Cursor3DProperties as HeightmapCursorProperties;
          if (cursorProp != null)
            cursorProp.AbsoluteHeight = fHeight;
        } 
      }
			return base.OnMouseDown(e, eKeyMod, iOldX, iOldY);
		}

		public override bool OnMouseUp(System.Windows.Forms.MouseEventArgs e, CSharpFramework.Contexts.KeyModifier eKeyMod, int iOldX, int iOldY)
		{
			CurrentMode = HeightmapEditMode_e.None;
			return base.OnMouseUp(e, eKeyMod, iOldX, iOldY);
		}
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
