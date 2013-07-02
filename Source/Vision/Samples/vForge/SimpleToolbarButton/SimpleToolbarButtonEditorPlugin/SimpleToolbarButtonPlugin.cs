/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using CSharpFramework;

namespace SimpleToolbarButtonPlugin
{
  //class name has to be <<EditorPlugin>>
  public class EditorPlugin : IEditorPluginModule
  {
    private ToolStripButton _toolbarButton;

    public EditorPlugin()
    {
      _version = 1;
      _name = "SimpleToolbarButtonPlugin";
    }

    public override bool InitPluginModule()
    {
      _toolbarButton = new ToolStripButton();
      _toolbarButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      _toolbarButton.Image = Resources.cup;
      _toolbarButton.ToolTipText = "Simple Toolbar Button that has no function";
      _toolbarButton.Visible = true;
      _toolbarButton.Enabled = false;
      _toolbarButton.Click += new EventHandler(Button_Toolbar_Click);

      //add this button to the EnginePanelToolbar
      EditorManager.GUI.EnginePanelToolBar.Items.Add(_toolbarButton);

      // Listen to scene changes, so that we can disable the button when no scene is active
      EditorManager.SceneChanged += new SceneChangedEventHandler(EditorManager_SceneChanged);

      return true;
    }

    public override bool DeInitPluginModule()
    {
      EditorManager.SceneChanged -= new SceneChangedEventHandler(EditorManager_SceneChanged);
      _toolbarButton.Click -= new EventHandler(Button_Toolbar_Click);
      EditorManager.GUI.EnginePanelToolBar.Items.Remove(_toolbarButton);
      _toolbarButton = null;
      return true;
    }

    /// <summary>
    /// Overridable that determines whether this is a standard plugin that is always loaded and initialized
    /// </summary>
    public override bool IsStandardPlugin
    {
      get
      {
        return false;
      }
    }

    private void EditorManager_SceneChanged(object sender, SceneChangedArgs args)
    {
      // Allow enabled button only in case a scene is loaded
      _toolbarButton.Enabled = (EditorManager.Scene != null);
    }

    private void Button_Toolbar_Click(object sender, EventArgs e)
    {
      MessageBox.Show("You have clicked the sample button");
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
