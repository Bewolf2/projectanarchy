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
using CSharpFramework;
using CSharpFramework.Contexts;
using TerrainEditorPlugin.Dialogs;
using TerrainEditorPlugin.Shapes;
using System.Windows.Forms;
using CSharpFramework.Scene;
using CSharpFramework.Math;

namespace TerrainEditorPlugin.Contexts
{
	/// <summary>
	/// Base class for terrain editing
	/// </summary>
	public class TerrainEditingBaseContext : CameraMoveContext
	{
		public TerrainEditingBaseContext()
		{
			AllowShapeSelection = false;
      ShowContextMenuOnRMB = false;
      AllowRecenterMouse = false;
		}

		#region Per frame handling

    public override void PreRenderTickFunction(float fTimeDelta)
		{
      base.PreRenderTickFunction(fTimeDelta);

			TerrainShape terrain = TerrainEditor.CurrentTerrain;
			if (terrain != null)
			{
				terrain.EngineTerrain.Update3DCursor(TerrainEditor.Cursor3DProperties, (float)MouseX, (float)MouseY);

        if (this._resizeBrushKeyDown)
        {
          Vector3F vDist = terrain.EngineTerrain.GetCurrentCursorCenter()  - this._vBrushResizeCenter;
          vDist.Z = 0.0f;
          float fDist = vDist.GetLength();
          if (fDist > 1.0f)
          {
            // calc angle
            vDist.Normalize();
            float fAngle = (float)(Math.Acos((double)vDist.Y) * 180 / Math.PI);
            if (vDist.X > 0.0f)
              fAngle = 360.0f - fAngle;

            TerrainEditor.Cursor3DProperties.Radius = fDist;
            TerrainEditor.Cursor3DProperties.Rotation = fAngle;
            // restore position again
            terrain.EngineTerrain.Update3DCursor(TerrainEditor.Cursor3DProperties, (float)_iBrushResizeMouseX, (float)_iBrushResizeMouseY);

            float fWgt = 0.8f;
            float fMiddleX = (float)MouseX * fWgt + (float)_iBrushResizeMouseX * (1.0f - fWgt);
            float fMiddleY = (float)MouseY * fWgt + (float)_iBrushResizeMouseY * (1.0f - fWgt);
            char degreeChar = (char)0xB0;
            this.View.RenderLine2D((float)MouseX, (float)MouseY, (float)_iBrushResizeMouseX, (float)_iBrushResizeMouseY, VisionColors.Yellow, 1.0f);
            this.View.WriteText2D(fMiddleX, fMiddleY, fDist.ToString("N2"), VisionColors.Yellow);
            this.View.WriteText2D(_iBrushResizeMouseX, _iBrushResizeMouseY, fAngle.ToString("N1") + degreeChar, VisionColors.Yellow);
          }
        }
        

			}

		}

		#endregion

    bool _bIsPainting = false; 

    public bool IsEditingKeyPressed(KeyModifier eKeyMod)
    {
      bool bAnyModifier = eKeyMod == KeyModifier.Alt || eKeyMod == KeyModifier.Ctrl || eKeyMod == KeyModifier.Shift;
      if (EditorManager.Settings.CameraStyle==EditorSettingsBase.CameraStyle_e.MaxStyle)
        return !bAnyModifier;
      return bAnyModifier;
    }


    public override bool OnMouseDown(System.Windows.Forms.MouseEventArgs e, KeyModifier eKeyMod, int iOldX, int iOldY)
    {
      _bIsPainting = true;

      if (_resizeBrushKeyDown)
      {
        _bIsPainting = false;
      }

      if (_bIsPainting)
      {
        TerrainEditor.BeginUpdatePainting();
      }

      return base.OnMouseDown(e, eKeyMod, iOldX, iOldY);
    }

    public override bool OnMouseUp(System.Windows.Forms.MouseEventArgs e, KeyModifier eKeyMod, int iOldX, int iOldY)
    {
      if (_bIsPainting)
        TerrainEditor.FinishUpdatePainting();

      return base.OnMouseUp(e, eKeyMod, iOldX, iOldY);
    }

    bool _resizeBrushKeyDown = false;
    int _iBrushResizeMouseX = 0;
    int _iBrushResizeMouseY = 0;

    Vector3F _vBrushResizeCenter;
    
    public override void OnKeyDown(System.Windows.Forms.KeyEventArgs e)
    {
      if (e.KeyCode == Keys.B && !_resizeBrushKeyDown)
      {
        //EditorManager.EngineManager.GetRayAtScreenPos(_vTraceStart,_vTraceEnd,MouseX,MouseY,EditorManager.Settings.MaxPickingDistance);
			  // TerrainShape terrain = TerrainEditor.CurrentTerrain;
        //terrain.EngineTerrain.TraceTerrain(_vTraceStart,_vTraceEnd,ref _vBrushCenter);
        _vBrushResizeCenter = TerrainEditor.CurrentTerrain.EngineTerrain.GetCurrentCursorCenter();
        _resizeBrushKeyDown = true;
        _iBrushResizeMouseX = MouseX;
        _iBrushResizeMouseY = MouseY;
        e.Handled = true;
      }
      base.OnKeyDown(e);
    }

    public override void OnKeyUp(System.Windows.Forms.KeyEventArgs e)
    {
      if (e.KeyCode == Keys.B)
      {
        _resizeBrushKeyDown = false;
        EditorManager.ActiveView.Dirty = true;
        e.Handled = true;
      }
      base.OnKeyUp(e);
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
