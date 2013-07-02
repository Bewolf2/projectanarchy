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
using CSharpFramework;
using CSharpFramework.Shapes;
using System.Windows.Forms;
using CSharpFramework.DynamicProperties;
using CSharpFramework.Actions;
using System.Reflection;

namespace UndoableGroupAction
{
  //class name has to be <<EditorPlugin>>
  public class EditorPlugin : IEditorPluginModule
  {

    //this plugin renames the text label of shapes
    public EditorPlugin()
    {
      _version = 1;
      _name = "UndoableGroupActionPlugin";
    }

    private ToolStripMenuItem _undoMenuItem = null;

    public override bool InitPluginModule()
    {
      // register our plugin
      //Add a menu entry for the Perforce settings
      ToolStripMenuItem samplesMenuItem = FindOrAddMenuItem(EditorManager.GUI.MainMenu.Items, "vForgeSamples");
      _undoMenuItem = new ToolStripMenuItem("Undoable Group Action", null, new EventHandler(_menuItem_Click));
      samplesMenuItem.DropDownItems.Add(_undoMenuItem);
      return true;
    }

    public override bool DeInitPluginModule()
    {
      // de-register our plugin
      ToolStripMenuItem samplesMenuItem = FindOrAddMenuItem(EditorManager.GUI.MainMenu.Items, "vForgeSamples");
      samplesMenuItem.DropDownItems.Remove(_undoMenuItem);
      if (samplesMenuItem.DropDownItems.Count == 0) //also remove "vForgeSamples" item it is has no subitems lef
      {
        EditorManager.GUI.MainMenu.Items.Remove(samplesMenuItem);
      }
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

    /// <summary>
    /// User selected the plugin, so execute it
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void _menuItem_Click(object sender, EventArgs e)
    {

      //create new groupaction
      GroupAction actions = new GroupAction("Rename Textlabel");

      int i = 0;
      //search for shapes in layer (do not search recursively)
      foreach (Layer layer in EditorManager.Scene.Layers)
      {
        foreach (ShapeBase shape in layer.Root.ChildCollection)
        {
          Shape3D shape3d = shape as Shape3D;
          if (shape3d == null || !shape3d.Modifiable)
            continue;
          actions.Add(new SetPropertyAction(shape3d, "TextLabel", "Object" + i));
          i++;
        }
      }

      // spawn an undoable action. To trigger it silently, just call actions.Do();
      if (actions.Valid)
        EditorManager.Actions.Add(actions);


    }

    //helper function to find or add a menu item
    private ToolStripMenuItem FindOrAddMenuItem(ToolStripItemCollection list, string name)
    {
      int iCount = list.Count;
      for (int i = 0; i < iCount; i++)
      {
        ToolStripMenuItem item = (ToolStripMenuItem)list[i];
        if (string.Compare(item.Text, name, true) == 0)
          return item;
      }
      //not found, so create menu item
      ToolStripMenuItem itemToAdd = new ToolStripMenuItem(name);
      list.Add(itemToAdd);
      return itemToAdd;
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
