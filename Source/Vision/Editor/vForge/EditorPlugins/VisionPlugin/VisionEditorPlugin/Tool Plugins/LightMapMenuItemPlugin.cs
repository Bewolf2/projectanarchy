/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Windows.Forms;
using CSharpFramework;
using VisionEditorPlugin.Dialogs;


namespace VisionEditorPlugin
{
	/// <summary>
	/// Plugin for adding the lightmap tweaking tool to the main menu
	/// </summary>
  public class LightmapMenuItemPlugin : IMenuItemPlugin
  {
    #region IMenuItemPlugin Implementation

    /// <summary>
    /// Constructor.
    /// </summary>
    public LightmapMenuItemPlugin()
    {
      _lightmapDlg.Hide();
      _itemLightmapTweaking.Click += new EventHandler(_itemLightmapTweaking_Click);
    }

    /// <summary>
    /// The name of the plugin
    /// </summary>
    override public string Name
    {
      get { return "LightmapTool"; }
    }

    /// <summary>
    /// Gets the parent menu the item will be added to
    /// </summary>
    override public IMenuItemPlugin.Category ParentMenu
    {
      get { return IMenuItemPlugin.Category.StaticLighting; }
    }

    /// <summary>
    /// Gets the parent toolbar button the item will be added to
    /// </summary>
    override public IMenuItemPlugin.ToolbarCategory ParentToolbarButton
    {
      get { return IMenuItemPlugin.ToolbarCategory.None; }
    }

    /// <summary>
    /// The MenuItem which is provided by this plugin
    /// </summary>
    override public ToolStripMenuItem MenuItem
    {
      get { return _itemLightmapTweaking; }
    }

    /// <summary>
    /// The Toolbar item which is provided by this plugin
    /// </summary>
    public override System.Windows.Forms.ToolStripItem ToolbarItem
    {
      get { return null; }
    }

    /// <summary>
    /// Initialize the plugin
    /// </summary>
    public override void OnInitialize(ToolStripMenuItem parent)
    {
      // add event handler for popup
      parent.DropDownOpening += new EventHandler(parentMenu_Popup);
    }

    /// <summary>
    /// Deinitialize the plugin
    /// </summary>
    public override void OnDeinitialize(ToolStripMenuItem parentMenu)
    {
      // remove event handler for popup
      parentMenu.DropDownOpening -= new EventHandler(parentMenu_Popup);
    }

    #endregion

    #region Event Handlers

    private void _itemLightmapTweaking_Click(object sender, EventArgs e)
    {
      _lightmapDlg.Active = !_lightmapDlg.Active;
    }

    private void parentMenu_Popup(object sender, EventArgs e)
    {
      _itemLightmapTweaking.Enabled = (EditorManager.Scene!=null);
      _itemLightmapTweaking.Checked = _lightmapDlg.Active;
    }

    #endregion

    #region Public Script Functions

    /// <summary>
    ///  Open this tool
    /// </summary>
    public void OpenTool()
    {
      _lightmapDlg.Active = true;
    }

    #endregion

    #region Private Data

    LightmapToolDlg _lightmapDlg = new LightmapToolDlg();
    ToolStripMenuItem _itemLightmapTweaking = new ToolStripMenuItem("&Tweak lighting...");

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
