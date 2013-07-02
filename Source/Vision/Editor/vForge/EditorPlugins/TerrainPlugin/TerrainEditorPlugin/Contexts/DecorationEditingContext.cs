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
using CSharpFramework.Scene;

namespace TerrainEditorPlugin.Contexts
{
  /// <summary>
  /// Mouse camera context for decoration editing
  /// </summary>
  public class DecorationEditingContext : TerrainEditingBaseContext
  {

    bool _bPaint = false;
    bool _bSubtract = false;

    public override void PreRenderTickFunction(float fTimeDelta)
    {
      base.PreRenderTickFunction(fTimeDelta);
      if (_bPaint)
      {
        TerrainEditor.BlitDecorationBrush(_bSubtract);
        View.UpdateView(false); // trigger next update
      }
    }

    public override bool OnMouseDown(System.Windows.Forms.MouseEventArgs e, CSharpFramework.Contexts.KeyModifier eKeyMod, int iOldX, int iOldY)
    {
      if ((IsEditingKeyPressed(eKeyMod) && (EditorSettingsBase.CameraStyle_e.MayaStyle != EditorManager.Settings.CameraStyle)) || eKeyMod == KeyModifier.Ctrl)
      {
        _bPaint = true;
        _bSubtract = (e.Button == MouseButtons.Right || (EditorManager.Tablet != null && EditorManager.Tablet.IsEraserPressed));
      }
      return base.OnMouseDown(e, eKeyMod, iOldX, iOldY);
    }

    public override bool OnMouseUp(System.Windows.Forms.MouseEventArgs e, CSharpFramework.Contexts.KeyModifier eKeyMod, int iOldX, int iOldY)
    {
      _bPaint = false;
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
