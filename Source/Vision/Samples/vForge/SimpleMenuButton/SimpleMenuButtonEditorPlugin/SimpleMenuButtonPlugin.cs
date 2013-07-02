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

namespace SimpleMenuButtonPlugin
{
  //class name has to be <<EditorPlugin>>
  public class EditorPlugin : IEditorPluginModule
  {
    private SimpleMenuItemPlugin _menuItemPlugin = new SimpleMenuItemPlugin();

    public EditorPlugin()
    {
      _version = 1;
      _name = "SimpleMenuButtonPlugin";
    }

    public override bool InitPluginModule()
    {
      // register our plugin
      EditorManager.AddMenuItemPlugin(_menuItemPlugin);
      return true;
    }

    public override bool DeInitPluginModule()
    {
      // de-register our plugin
      EditorManager.RemoveMenuItemPlugin(_menuItemPlugin);
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

  }



  public class SimpleMenuItemPlugin : IMenuItemPlugin
  {
    #region Implement IMenuItemPlugin

    private ToolStripMenuItem _menuItem = new ToolStripMenuItem("Sample Menu Item");

    /// <summary>
    /// Gets the parent toolbar button the item will be added to
    /// </summary>
    override public IMenuItemPlugin.ToolbarCategory ParentToolbarButton
    {
      get { return IMenuItemPlugin.ToolbarCategory.None; }
    }

    /// <summary>
    /// The Toolbar item which is provided by this plugin
    /// </summary>
    public override System.Windows.Forms.ToolStripItem ToolbarItem
    {
      get { return null; }
    }

    public override ToolStripMenuItem MenuItem
    {
      get { return _menuItem; }
    }

    public override string Name
    {
      get { return "Sample Menu Item"; }
    }

    public override Category ParentMenu
    {
      get { return Category.Extras; }
    }

    #endregion

    #region Menu handling

    /// <summary>
    /// Initialize the plugin
    /// </summary>
    public override void OnInitialize(ToolStripMenuItem parentMenu)
    {
      // add event handler for popup
      parentMenu.DropDownOpening += new EventHandler(parentMenu_Popup);
      _menuItem.Click += new EventHandler(_menuItem_Click);
    }

    /// <summary>
    /// Deinitialize the plugin
    /// </summary>
    public override void OnDeinitialize(ToolStripMenuItem parentMenu)
    {
      // remove event handler for popup
      parentMenu.DropDownOpening -= new EventHandler(parentMenu_Popup);
      _menuItem.Click -= new EventHandler(_menuItem_Click);
    }

    void parentMenu_Popup(object sender, EventArgs e)
    {
      _menuItem.Enabled = EditorManager.Scene != null;
    }

    /// <summary>
    /// User selected the plugin, so execute it
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void _menuItem_Click(object sender, EventArgs e)
    {
      MessageBox.Show("Sample Menu Item has been clicked");
    }

    #endregion
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
