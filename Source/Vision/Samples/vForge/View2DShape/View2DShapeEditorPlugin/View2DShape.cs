/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Windows.Forms;
using System.Drawing;
using CSharpFramework;
using CSharpFramework.Math;
using CSharpFramework.Shapes;
using CSharpFramework.Actions;
using System.Runtime.Serialization;
using ManagedFramework;
using System.IO;
using CSharpFramework.Scene;
using CSharpFramework.Controls;

namespace View2DShapePlugin
{
  #region class View2DShape
  /// <summary>
  /// HotSpotShape : This is the class that represents the shape in the editor. It has no engine instance that handles the
  /// native code and will not be shown in engine view. It just shows how to add Hotspots to a shape
	/// </summary>
  [Serializable]
  public class View2DShape : Shape3D
	{
    #region Constructor

    /// <summary>
    /// The constructor of the node shape, just takes the node name
    /// </summary>
    /// <param name="name">Name of the shape in the shape tree</param>
		public View2DShape(string name) : base(name)
		{
      Scene2DRenderingEnabled = true;
    }

    #endregion

    public override void OnRemoveFromScene()
    {
      base.OnRemoveFromScene();
      Scene2DRenderingEnabled = false;
    }

    public override void OnPaintScene2DView(CSharpFramework.Controls.Scene2DView view, PaintEventArgs e)
    {
      base.OnPaintScene2DView(view, e);

      Vector3F shapePos = Position;
      Point point = view.Transform(shapePos);
      Rectangle rectToDraw = new Rectangle(point, new Size(30, 30));
      e.Graphics.FillRectangle(Brushes.DarkGreen, rectToDraw);
      Pen pen = new Pen(Color.LightGreen, 1);
      e.Graphics.DrawRectangle(pen, rectToDraw);
      
    }




    #region Icon

    /// <summary>
    /// Get an index once (executed the first time an instance of this shape is created)
    /// </summary>
    static int iIconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\Node.bmp"), Color.Magenta);

    /// <summary>
    /// Return the icon index in the image list for the shape tree view. For this, use a static variable for this class.
    /// </summary>
    public override int GetIconIndex()
    {
      return iIconIndex;
    }

    /// <summary>
    /// Gets the icon index for selected icons in the tree view. Simply pass the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetSelectedIconIndex()
    {
      return iIconIndex;
    }

    #endregion

    #region Serialization and Export

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected View2DShape(SerializationInfo info, StreamingContext context) : base(info, context)
    {
      Scene2DRenderingEnabled = true;
    }
  
    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context)
    {
      base.GetObjectData (info, context);
      // nothing else to do...
    }

    /// <summary>
    /// Called when exporting the scene to engine archive. base implemenation calls function on engine object which in turn
    /// serializes itself
    /// </summary>
    /// <returns></returns>
    public override bool OnExport(CSharpFramework.Scene.SceneExportInfo info)
    {
      return base.OnExport (info);
    }

    #endregion

   
  }
  #endregion

  #region View2DShape Creator Plugin


  /// <summary>
  /// Creator class. An instance of the creator is registerd in the plugin init function. Thus the creator shows
  /// up in the "Create" menu of the editor
  /// </summary>
  class View2DShapeCreator : CSharpFramework.IShapeCreatorPlugin
  {

    /// <summary>
    /// Constructor
    /// </summary>
    public View2DShapeCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\Node.bmp"), Color.Magenta);
    }

    /// <summary>
    /// Get the name of the plugin, for instance the shape name. This name apears in the "create" menu
    /// </summary>
    /// <returns>creator name</returns>
    public override string GetPluginName()
    {
      return "View2DShape";
    }

    /// <summary>
    /// Get the plugin category name to sort the plugin name. This is useful to group creators. A null string can
    /// be returned to put the creator in the root
    /// </summary>
    /// <returns></returns>
    public override string GetPluginCategory()
    {
      return "Misc";
    }

    /// <summary>
    /// Returns a short description text
    /// </summary>
    /// <returns></returns>
    public override string GetPluginDescription()
    {
      return "Creates a View2DShape instance.\n" +
        "This shape is just a sample for custom vForge render to 2DView. It does not have any engine instance and will not be shown in engine view.";
    }

    public override ShapeBase CreateShapeInstance()
    {
      View2DShape shape = new View2DShape("View2DShape");
      shape.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
      return shape;
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
