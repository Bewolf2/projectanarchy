/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using CSharpFramework;
using CSharpFramework.Contexts;
using CSharpFramework.Shapes;
using CSharpFramework.View;
using CSharpFramework.Clipboard;
using CSharpFramework.Docking;
using CSharpFramework.Math;
using VisionEditorPlugin.Shapes;
using VisionManaged;

namespace VisionEditorPlugin.Dialogs
{
  public partial class DecorationInstanceContextDlg : Form
  {
#if !HK_ANARCHY
    public DecorationInstanceContextDlg()
    {
      InitializeComponent();
      VisionViewBase.MouseContextChanged += new ContextChangedEventHandler(VisionViewBase_MouseContextChanged);

      numericUpDown_Scale.Value = (decimal)_contextAdd.UniformScaling;
      trackBar_ScaleVariation.Value = (int)(_contextAdd.ScaleVariation * (float)trackBar_ScaleVariation.Maximum);
      UpdateToolbar();
    }

    void VisionViewBase_MouseContextChanged(object sender, ContextChangedArgs e)
    {
      UpdateToolbar();
    }


    void UpdateToolbar()
    {
      IMouseContext context = EditorManager.ActiveView.CurrentContext;
      this.toolStripButton_ContextAdd.Checked = context == _contextAdd;

      if (this.toolStripButton_ContextAdd.Checked)
        StatusText = "Hold down CTRL and click into the view to add new decoration instances";
      else
        StatusText = null;

      groupBox_Create.Visible = this.toolStripButton_ContextAdd.Checked;
      if (groupBox_Create.Visible)
        this.Size = this.MaximumSize;
      else
        this.Size = this.MinimumSize;
    }

    public string StatusText
    {
      set
      {
        this.toolStripStatusLabel1.Text = value;
      }
    }


    public void SetDecorationAddContext()
    {
      EditorManager.ActiveView.CurrentContext = _contextAdd;
    }


    #region Dialog overrides

    protected override void OnVisibleChanged(EventArgs e)
    {
      base.OnVisibleChanged(e);

      if (!Visible)
      {
        DecorationGroupShape.CurrentDecorationGroup = null;
        if (EditorManager.ActiveView.CurrentContext is DecorationBaseContext)
          EditorManager.ActiveView.SetDefaultContext();
      }
      else
      {
        UpdateToolbar();
      }
    }


    protected override void OnClosing(CancelEventArgs e)
    {
      DecorationGroupShape.CurrentDecorationGroup = null;
      e.Cancel = true;
    }

    protected override void OnGotFocus(EventArgs e)
    {
      base.OnGotFocus(e);
    }


    protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
    {
      // We don't want this form to handle cut/copy/paste commands.
      // Instead forward to the active component to process it.
      // Don't forward delete, as we need it in our control.
      if (keyData != Keys.Delete && EditorManager.EditorComponentKeyDown(null, keyData))
      {
        return true;
      }

      return base.ProcessCmdKey(ref msg, keyData);
    }

    #endregion

    static DecorationAddContext _contextAdd = new DecorationAddContext();
 
    /// <summary>
    /// Returns either selected instances or all instances
    /// </summary>
    static DecorationInstanceShape[] RelevantSelection
    {
      get
      {
        if (DecorationGroupShape.CurrentDecorationGroup == null)
          return null;
        DecorationInstanceShape[] sel = DecorationGroupShape.CurrentDecorationGroup.CurrentSelection;
        if (sel!=null)
          return sel;
        return DecorationGroupShape.CurrentDecorationGroup.Instances;
      }
    }


    private void toolStripButton_ContextAdd_Click(object sender, EventArgs e)
    {
      // toggle mode
      if (EditorManager.ActiveView.CurrentContext == _contextAdd)
        EditorManager.ActiveView.SetDefaultContext();
      else
        EditorManager.ActiveView.CurrentContext = _contextAdd;
    }

    private void numericUpDown_Scale_ValueChanged(object sender, EventArgs e)
    {
      _contextAdd.UniformScaling = (float)numericUpDown_Scale.Value;
    }

    private void trackBar_ScaleVariation_Scroll(object sender, EventArgs e)
    {
      _contextAdd.ScaleVariation = (float)trackBar_ScaleVariation.Value / (float)trackBar_ScaleVariation.Maximum;
    }

    private void toolStripButton_Recenter_Click(object sender, EventArgs e)
    {
      if (DecorationGroupShape.CurrentDecorationGroup != null)
        DecorationGroupShape.CurrentDecorationGroup.PerformRelevantOperation(DecorationGroupShape.RO_RECENTER, 0, 1);
    }

    private void toolStripButton_DropToFloor_Click(object sender, EventArgs e)
    {
      DecorationInstanceShape[] sel = RelevantSelection;
      if (sel == null)
        return;
      EditorManager.Actions.Add(new DecorationDropToFloorAction(DecorationGroupShape.CurrentDecorationGroup, sel, Shape3D.DropToFloorMode.Pivot, -Vector3F.ZAxis, null));
    }

    private void toolStripButton_Relax_Click(object sender, EventArgs e)
    {
      DecorationInstanceShape[] sel = RelevantSelection;
      if (sel == null)
        return;
      DecorationGroupShape.CurrentDecorationGroup.RelaxPositions(sel);
    }

    private void toolStripButton_RemoveOverlapping_Click(object sender, EventArgs e)
    {
      DecorationInstanceShape[] sel = RelevantSelection;
      if (sel == null)
        return;
      DecorationGroupShape.CurrentDecorationGroup.RemoveOverlappingInstances(sel);
    }

#else

    // As we can't exclude this dialog's .Designer.cs file from the build system,
    // we have to provide empty event handlers in order to keep things compiling.
    private void toolStripButton_ContextAdd_Click(object sender, EventArgs e)
    {
    }

    private void toolStripButton_Recenter_Click(object sender, EventArgs e)
    {
    }

    private void toolStripButton_DropToFloor_Click(object sender, EventArgs e)
    {
    }

    private void toolStripButton_Relax_Click(object sender, EventArgs e)
    {
    }

    private void toolStripButton_RemoveOverlapping_Click(object sender, EventArgs e)
    {
    }

    private void trackBar_ScaleVariation_Scroll(object sender, EventArgs e)
    {
    }

    private void numericUpDown_Scale_ValueChanged(object sender, EventArgs e)
    {
    }


#endif
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
