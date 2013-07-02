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
using System.Windows.Forms;
using VisionEditorPlugin.Dialogs;

namespace VisionEditorPlugin.Tool_Plugins
{
  class ColorGradingToolPlugin : IMenuItemPlugin
  {

    public ColorGradingToolPlugin()
    {
    }

    public override void OnInitialize(ToolStripMenuItem parent)
    {
      base.OnInitialize(parent);
      _menuItem.Click += new EventHandler(_item_Click);
      _toolbarItem.Click += new EventHandler(_item_Click);
      _menuItem.ImageScaling = ToolStripItemImageScaling.None;
      _toolbarItem.ImageScaling = ToolStripItemImageScaling.None;
      IProject.NewProjectLoaded += new EventHandler(IProject_ProjectChanged);
      IProject.ProjectUnloaded += new EventHandler(IProject_ProjectChanged);
      IProject_ProjectChanged(null, null);
    }


    public override void OnDeinitialize(ToolStripMenuItem parent)
    {
      base.OnDeinitialize(parent);
      _menuItem.Click -= new EventHandler(_item_Click);
      _toolbarItem.Click -= new EventHandler(_item_Click);
      IProject.NewProjectLoaded -= new EventHandler(IProject_ProjectChanged);
      IProject.ProjectUnloaded -= new EventHandler(IProject_ProjectChanged);
    }

    void IProject_ProjectChanged(object sender, EventArgs e)
    {
      _menuItem.Enabled = (EditorManager.Project != null);
      _toolbarItem.Enabled = (EditorManager.Project != null);
    }

    void _item_Click(object sender, EventArgs e)
    {
      if (EditorManager.Project == null)
        return;
      ColorGradingToolDlg dlg = new ColorGradingToolDlg();
      if (dlg.ShowDialog() != DialogResult.OK)
        return;
    }

    public override string Name
    {
      get { return "Generate Color Grading Texture"; }
    }

    ToolStripMenuItem _menuItem = new ToolStripMenuItem("Generate Color Grading Texture", global::VisionEditorPlugin.Properties.Resources.colors);
    ToolStripMenuItem _toolbarItem = new ToolStripMenuItem("Color Grading Dialog", global::VisionEditorPlugin.Properties.Resources.colors);

    public override System.Windows.Forms.ToolStripMenuItem MenuItem
    {
      get { return _menuItem; }
    }

    public override System.Windows.Forms.ToolStripItem ToolbarItem
    {
      get { return _toolbarItem; }
    }

    public override IMenuItemPlugin.Category ParentMenu
    {
      get {return Category.Extras; }
    }

    public override IMenuItemPlugin.ToolbarCategory ParentToolbarButton
    {
      get { return ToolbarCategory.Tools; }
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
