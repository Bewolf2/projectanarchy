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
using CSharpFramework.Math;
using CSharpFramework.Shapes;
using System.IO;
using System.Drawing;

namespace UndoableCustomActionPlugin
{
  //class name has to be <<EditorPlugin>>
  public class EditorPlugin : IEditorPluginModule
  {

    public EditorPlugin()
    {
      _version = 1;
      _name = "UndoableCustomActionPlugin";
    }

    private ToolStripMenuItem _undoMenuItem = null;

    public override bool InitPluginModule()
    {
      //Add a menu entry for the Perforce settings
      ToolStripMenuItem samplesMenuItem = FindOrAddMenuItem(EditorManager.GUI.MainMenu.Items, "vForgeSamples");
      _undoMenuItem = new ToolStripMenuItem("Undoable Custom Action", null, new EventHandler(_menuItem_Click));
      samplesMenuItem.DropDownItems.Add(_undoMenuItem); 
      return true;
    }

    public override bool DeInitPluginModule()
    {
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
      ShapeCollection selectedShapes = EditorManager.SelectedShapes;
      if (selectedShapes == null || selectedShapes.Count <= 0)
      {
        MessageBox.Show("Please select a shape before you execute this operation");
        return;
      }

      // Move the selected shapes
      foreach (ShapeBase shape in selectedShapes)
      {
        Shape3D shape3d = shape as Shape3D;
        if (shape3d == null || !shape3d.Modifiable)
         continue;
        IAction customAction = new MoveShapeXAction(shape3d, 30);
        EditorManager.Actions.Add(customAction);
        MessageBox.Show("Shape " + shape3d.ShapeName + " has been moved. You can undo this operation");
        return;
      }
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



  /// <summary>
  /// Action to move a single shape
  /// </summary>
  public class MoveShapeXAction : IAction
  {
    #region constructor

    /// <summary>
    /// Constructor that takes the shape to move and the move positions
    /// </summary>
    /// <param name="_shape">The shape to move</param>
    /// <param name="_oldPos">The old position of the shape</param>
    public MoveShapeXAction(Shape3D shape, float shiftXValue)
    {
      
      _shape = shape;
      _oldPos = _shape.Position;
      _shiftXValue = shiftXValue;
    }
    Shape3D _shape;
    Vector3F _oldPos;
    float _shiftXValue;

    #endregion

    #region IAction Members

    /// <summary>
    /// Sets the old position
    /// </summary>
    public override void Undo()
    {
      _shape.Position = _oldPos;
      _shape.Modified = true;
      IScene.SendPropertyChangedEvent(new PropertyChangedArgs(_shape, "Position"));
    }

    /// <summary>
    /// Sets the new position
    /// </summary>
    public override void Do()
    {
      Vector3F newPos = _shape.Position;
      newPos.X += _shiftXValue;
      _shape.Position = newPos;
      _shape.Modified = true;
      IScene.SendPropertyChangedEvent(new PropertyChangedArgs(_shape, "Position"));
    }

    /// <summary>
    /// Short name of the action, to show in the menu
    /// </summary>
    public override string ShortName { get { return "move shape x"; } }

    /// <summary>
    /// Long name of the action, to show in the history
    /// </summary>
    public override string LongName
    {
      get { return string.Format("shape '{0}' position x shifted by '{1}'", _shape.ShapeName, _shiftXValue); }
    }

    /// <summary>
    /// Overridden valid function (checks layer editable flag)
    /// </summary>
    public override bool Valid
    {
      get
      {
        if (!base.Valid)
          return false;
        if (_shape.IsHintSet(ShapeBase.HintFlags_e.NoUserTransform))
          return false;
        return _shape.Modifiable;
      }
    }

    #endregion

    #region Icon Index

    public static int iIconIndex = EditorManager.GUI.ActionImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Actions\MoveShape.bmp"), System.Drawing.Color.Magenta);

    /// <summary>
    /// Static function to return the image used to display this type of action in the history
    /// </summary>
    public static Image IconImage
    {
      get
      {
        return EditorManager.GUI.ActionImages.ImageList.Images[iIconIndex];
      }
    }

    /// <summary>
    /// Return the icon index for this action
    /// </summary>
    public override int IconIndex { get { return iIconIndex; } }

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
