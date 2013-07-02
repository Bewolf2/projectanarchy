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
using CSharpFramework;
using System.Windows.Forms;
using CSharpFramework.Contexts;
using CSharpFramework.Math;

namespace Editor.Contexts
{
  #region MeasureToolPickerBase

  class MeasureToolPickerBase : IPickHandler
  {
    void UpdateTraceResult(int x, int y)
    {
      EditorManager.EngineManager.GetRayAtScreenPos(out startRay, out endRay, x, y, EditorManager.Settings.MaxPickingDistance);
      bResultValid = EditorManager.EngineManager.GetTraceHitWithNormal(startRay, endRay, ref hitPoint, ref hitNormal);
    }

    public void ShowTraceResult(VisionViewBase view)
    {
      if (bResultValid)
      {
        float cx = 0, cy = 0, cz = 0;
        float r = EditorManager.Settings.GlobalUnitScaling * 5.0f;
        if (view.Project2D(hitPoint, ref cx, ref cy, ref cz))
          view.WriteText2D(cx, cy, hitPoint.ToString(), VisionColors.Yellow);
        view.RenderCross(hitPoint, r, VisionColors.Yellow, 1.0f);
        view.RenderLine(hitPoint, hitPoint + hitNormal * r * 3.0f, VisionColors.RGB(100, 255, 100), 1.0f);
      }
    }

    public override void ScreenOutput(VisionViewBase view)
    {
      base.ScreenOutput(view);
      int x = view.MouseX;
      int y = view.MouseY;
      UpdateTraceResult(x,y);
      ShowTraceResult(view);

      view.UpdateView(false);
    }


    public bool bResultValid = false;
    public Vector3F startRay, endRay;
    public Vector3F hitPoint = new Vector3F();
    public Vector3F hitNormal = new Vector3F();

  }
  #endregion

  #region MeasureToolPickerStart

  class MeasureToolPickerStart : MeasureToolPickerBase
  {
    public override string ScreenMessage
    {
      get
      {
        return "Click on the geometry to mark the first point";
      }
    }

    public override void ScreenOutput(VisionViewBase view)
    {
      base.ScreenOutput(view);


    }

    public override bool OnPick(MouseEventArgs e, KeyModifier eKeyMod)
    {
      base.OnPick(e, eKeyMod);
      if (!bResultValid)
        return true;

      // escalate to the next click
      EditorManager.ActiveView.PickHandler = new MeasureToolPickerEnd(this);
      return true;
    }
  }

  #endregion

  #region MeasureToolPickerEnd

  class MeasureToolPickerEnd : MeasureToolPickerBase
  {
    public MeasureToolPickerEnd(MeasureToolPickerStart start)
    {
      _start = start;
    }

    public override string ScreenMessage
    {
      get
      {
        return "Click on the geometry to mark the second point";
      }
    }

    public void ShowDistance(VisionViewBase view)
    {
      _start.ShowTraceResult(view);
      if (!bResultValid)
        return;

      view.RenderArrow(_start.hitPoint, hitPoint, VisionColors.Blue, 1.0f, 5.0f * EditorManager.Settings.GlobalUnitScaling);
      Vector3F diff = hitPoint - _start.hitPoint;
      float cx = 0, cy = 0, cz = 0;
      if (view.Project2D(_start.hitPoint + diff * 0.5f, ref cx, ref cy, ref cz))
        view.WriteText2D(cx, cy, string.Format("Distance: {0}", diff.GetLength()), VisionColors.RGB(255,255,100));
    }

    public void ShowCombinedTraceResult(VisionViewBase view)
    {
      ShowTraceResult(view);
      ShowDistance(view);
    }

    public override void ScreenOutput(VisionViewBase view)
    {
      base.ScreenOutput(view);
      ShowDistance(view);
    }

    public override bool OnPick(MouseEventArgs e, KeyModifier eKeyMod)
    {
      base.OnPick(e, eKeyMod);
      if (bResultValid)
        EditorManager.ActiveView.PickHandler = new MeasureToolPickerViewResult(_start, this, "Click into view or press ESC to abort");
      return true;
    }

    MeasureToolPickerStart _start;
  }

  #endregion

  #region MeasureToolPickerViewResult

  class MeasureToolPickerViewResult : IPickHandler
  {
    public MeasureToolPickerViewResult(MeasureToolPickerStart start, MeasureToolPickerEnd end, string msg)
    {
      _start = start;
      _end = end;
      _msg = msg;
    }

    string _msg;
    MeasureToolPickerStart _start;
    MeasureToolPickerEnd _end;

    public override string ScreenMessage
    {
      get
      {
        return _msg;
      }
    }

    public override void ScreenOutput(VisionViewBase view)
    {
      base.ScreenOutput(view);
      _end.ShowCombinedTraceResult(view);
    }
  }

  #endregion

  #region MeasureToolCamera

  public class MeasureToolCamera : CameraMoveContext
  {
    int iClickCount = 0;

    public MeasureToolCamera()
    {
      //this.AllowShapeSelection = false;
    }

    public override bool OnMouseUp(MouseEventArgs e, KeyModifier eKeyMod, int iOldX, int iOldY)
    {
      base.OnMouseUp(e, eKeyMod, iOldX, iOldY);
      if (e.Button == MouseButtons.Left)// && eKeyMod != KeyModifier.None)
        HandleClick();
      return true;
    }

    void HandleClick()
    {
      iClickCount++;

      switch (iClickCount)
      {
        case 1: _end = new MeasureToolPickerEnd(_start); break;
        case 2: _result = new MeasureToolPickerViewResult(_start, _end,"Click again to restart or ESC to abort"); break;
        default:
          iClickCount = 0; // retart
          break;
      }
    }

    public override void PreRenderTickFunction(float fTimeDelta)
    {
      base.PostRenderTickFunction(fTimeDelta);
      switch (iClickCount)
      {
        case 0: _start.ScreenOutput(View); break;
        case 1: _end.ScreenOutput(View); break;
        case 2: _result.ScreenOutput(View); break;
      }
    }

    public override void OnKeyDown(KeyEventArgs e)
    {
      if (e.KeyCode == Keys.Escape)
      {
        View.SetDefaultContext();
        e.Handled = true;
      }

      if (!e.Handled)
        base.OnKeyDown(e);
    }

    MeasureToolPickerStart _start = new MeasureToolPickerStart();
    MeasureToolPickerEnd _end = null;
    MeasureToolPickerViewResult _result = null;
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
