/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;
using CSharpFramework;
using CSharpFramework.Shapes;
using CSharpFramework.Math;

namespace Editor.Dialogs
{
  public partial class ExportReportDlg : Form
  {
    public ExportReportDlg()
    {
      InitializeComponent();
    }

    public ExportReportDlg(List<ShapeBase> unsupportedShapes)
    {
      InitializeComponent();

      foreach (ShapeBase shape in unsupportedShapes)
      {
        ListViewItem item = new ListViewItem();
        item.Text = "'" + shape.ShapeName + "' (" + shape.GetType() + 
          ") is not supported on Mobile Platforms and was not exported";
        item.ToolTipText = "Jump to shape by double clicking on this entry";
        item.Tag = shape;
        item.ForeColor = Color.Red;

        reportlist.Items.Add(item);
      }

      reportlist.Columns[0].AutoResize(ColumnHeaderAutoResizeStyle.ColumnContent);
      if (reportlist.Columns[0].Width < reportlist.Width)
        reportlist.Columns[0].Width = reportlist.Width;
    }

    private void btnOK_Click(object sender, System.EventArgs e)
    {
      Close();
    }

    private void reportlist_DoubleClick(object sender, System.EventArgs e)
    {
      ShapeBase shape = (ShapeBase)reportlist.SelectedItems[0].Tag;
      ShapeCollection shapes = new ShapeCollection();
      shapes.Add(shape);

      EditorManager.SelectedShapes = shapes;

      //jump to selection
      BoundingBox mergedBox = EditorManager.SelectedShapes.BoundingBox;
      if (!mergedBox.Valid)
        return;

      // make it local again
      Vector3F center = mergedBox.Center;
      mergedBox.Translate(-center);

      EditorManager.ActiveView.LookAt(center, mergedBox);
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
