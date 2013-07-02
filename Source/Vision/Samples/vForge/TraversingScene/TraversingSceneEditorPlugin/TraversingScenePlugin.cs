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
using VisionEditorPlugin.Dialogs;
using VisionEditorPlugin.Shapes;
using CSharpFramework;
using CSharpFramework.Serialization;
using CSharpFramework.Visitors;
using CSharpFramework.Actions;
using CSharpFramework.Shapes;
using CSharpFramework.DynamicProperties;


namespace TraversingScenePlugin
{

  /// <summary>
  /// This is a tool sample that adds a menu item to the 'vForgeSamples' menu
  /// When it is selected, it iterates through the scene and modifies the entity properties for every entity of class "AnimEntity_cl":
  ///  - The PathTime is an integer value 1,2,3,.. that defines the sibling index of the entity in the shape group
  ///  - The PathKey is set to "group_xy" where xy is a numerical value that is a unique group ID for every new group that contains entities
  /// This is of course very special purpose but it showcases some important concepts: 
  ///  - hooking a menu plugin
  ///  - traversing the scene
  ///  - accessing entity properties
  ///  - trigger actions
  /// </summary>
  //class name has to be <<EditorPlugin>>
  public class EditorPlugin : IEditorPluginModule
  {
    public EditorPlugin()
    {
      _version = 1;
      _name = "TraversingScenePlugin";
    }

    private ToolStripMenuItem _traverseMenuItem = null;

    public override bool InitPluginModule()
    {
      // Register our plugin and add a new menu entry
      ToolStripMenuItem samplesMenuItem = FindOrAddMenuItem(EditorManager.GUI.MainMenu.Items, "vForgeSamples");
      _traverseMenuItem = new ToolStripMenuItem("Enumerate entities", null, new EventHandler(_menuItem_Click));
      samplesMenuItem.DropDownItems.Add(_traverseMenuItem); 
      return true;
    }

    public override bool DeInitPluginModule()
    {
      ToolStripMenuItem samplesMenuItem = FindOrAddMenuItem(EditorManager.GUI.MainMenu.Items, "vForgeSamples");
      samplesMenuItem.DropDownItems.Remove(_traverseMenuItem);
      if (samplesMenuItem.DropDownItems.Count == 0) // Also remove "vForgeSamples" item it is has no subitems lef
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
      // Execute the plugin recursively on all shapes in the scene
      GroupAction actions = new GroupAction("Enumerate entities");
      int iGroupID = 0;
      foreach (Layer layer in EditorManager.Scene.Layers)
        if (layer.Modifiable)
          FlagEntityRecursive(layer.Root.ChildCollection, actions, iGroupID);

      // Spawn an undoable action. To trigger it silently, just call actions.Do();
      if (actions.Valid)
        EditorManager.Actions.Add(actions);
    }

    private void FlagEntityRecursive(ShapeCollection siblings, GroupAction actions, int iGroupID)
    {
      EditorManager.Actions.StartGroup("EnumerateEntities");

      int iSibling = 0;
      foreach (ShapeBase shape in siblings)
      {
        EntityShape entity = shape as EntityShape;
        if (entity == null || !entity.Modifiable)
          continue;

        // Simply change the name to communicate a change to the user
        EditorManager.Actions.Add(new SetShapeNameAction(entity, string.Format("Group_{0}_Sibling_{1}", iGroupID, iSibling)));

        // When editing properties of custom entity classes use a SetPropertyAction
        // e.g.:
        // DynamicPropertyCollection properties = entity.EntityProperties;
        // actions.Add(new SetPropertyAction(properties, "PropertyName", string.Format("PropertyValue")));

        // Recurse through children
        FlagEntityRecursive(shape.ChildCollection, actions, iGroupID + 1);

        iSibling++; // Increase sibling index for neighboring entities
      }

      EditorManager.Actions.EndGroup();
    }

    // Helper function to find or add a menu item
    private ToolStripMenuItem FindOrAddMenuItem(ToolStripItemCollection list, string name)
    {
      int iCount = list.Count;
      for (int i = 0; i < iCount; i++)
      {
        ToolStripMenuItem item = (ToolStripMenuItem)list[i];
        if (string.Compare(item.Text, name, true) == 0)
          return item;
      }

      // Not found, so create menu item
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
