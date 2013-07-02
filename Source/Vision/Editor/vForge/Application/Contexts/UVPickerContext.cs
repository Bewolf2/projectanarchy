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
using ManagedFramework;
using CSharpFramework;

namespace Editor.Contexts
{
  #region Class UVPickerContextBase

  /// <summary>
  /// Common base class for UV0PickerContext and UV1PickerContext
  /// </summary>
  public class UVPickerContextBase : CameraMoveContext
  {
    protected static EnginePickingHelper Picking = new EnginePickingHelper();

    public override void OnActivate(CSharpFramework.VisionViewBase currentView)
    {
      base.OnActivate(currentView);
      Picking.Init();

      IProject.ProjectUnloaded += new EventHandler(IProject_ProjectUnloaded);
    }

    public override void OnDeactivate()
    {
      base.OnDeactivate();
      Picking.DeInit();

      IProject.ProjectUnloaded -= new EventHandler(IProject_ProjectUnloaded);
    }


    float fZoom = 1.0f;

    public override bool OnMouseWheel(System.Windows.Forms.MouseEventArgs e, KeyModifier eKeyMod, int iOldX, int iOldY)
    {
      if (eKeyMod == KeyModifier.Ctrl)
      {
        float fZoomScale = e.Delta > 0 ? 2.0f : 0.5f;
        fZoom *= fZoomScale;
        if (fZoom < 0.125f)
          fZoom = 0.125f;
        if (fZoom > 8.0f)
          fZoom = 8.0f;
        Picking.SetZoom(fZoom);
      }
      else
        base.OnMouseWheel(e, eKeyMod, iOldX, iOldY); // move camera

      return true;
    }


    public override void OnKeyDown(System.Windows.Forms.KeyEventArgs e)
    {
      if (e.Control)
      {
        Picking.SetUsePixelSize(true);
        View.UpdateView(false);
      }
      if ((e.KeyCode & System.Windows.Forms.Keys.Escape) > 0)
      {
        // cancel this context.
        View.SetDefaultContext();
        e.Handled = true;
      }

      if (e.Handled)
        base.OnKeyDown(e);
    }

    public override void OnKeyUp(System.Windows.Forms.KeyEventArgs e)
    {
      base.OnKeyUp(e);
      Picking.SetUsePixelSize(false);
      View.UpdateView(false);

      fZoom = 1.0f;
      Picking.SetZoom(fZoom);
    }

    void IProject_ProjectUnloaded(object sender, EventArgs e)
    {
      this.View.SetDefaultContext(); // this context seems still active
    }

  }

  #endregion

  #region Class UV0PickerContext

  public class UV0PickerContext : UVPickerContextBase
  {
    public override void PreRenderTickFunction(float fTimeDelta)
    {
      base.PreRenderTickFunction(fTimeDelta);

      Picking.PickUV0((float)this.MouseX, (float)this.MouseY);
    }

  }

  #endregion

  #region Class UV1PickerContext

  public class UV1PickerContext : UVPickerContextBase
  {
    public override void PreRenderTickFunction(float fTimeDelta)
    {
      base.PreRenderTickFunction(fTimeDelta);

      Picking.PickUV1((float)this.MouseX, (float)this.MouseY);
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
