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
using CSharpFramework.View;
using System.ComponentModel;
using CSharpFramework.PropertyEditors;
using System.Drawing.Design;

namespace HotSpotShapePlugin
{

  #region class HotSpotShape
  /// <summary>
  /// HotSpotShape : This is the class that represents the shape in the editor. It has no engine instance that handles the
  /// native code and will not be shown in engine view. It just shows how to add Hotspots to a shape
	/// </summary>
  [Serializable]
  public class HotSpotShape : Shape3D
	{

    #region Constructor

    /// <summary>
    /// The constructor of the node shape, just takes the node name
    /// </summary>
    /// <param name="name">Name of the shape in the shape tree</param>
		public HotSpotShape(string name) : base(name)
		{
    }

    #endregion

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
    protected HotSpotShape(SerializationInfo info, StreamingContext context) : base(info, context)
    {
      //..nothing to do
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

    #region Hotspots

    HotSpotConeAngle _hotSpotAngle;
    HotSpotColorButton _hotSpotColor;

    /// <summary>
    /// Called when the shape is selected
    /// </summary>
    public override void OnSelected()
    {
      base.OnSelected();

      // create the hotspots:

      // hotspot for spotangle
      _hotSpotAngle = new HotSpotConeAngle(this, @"textures\Hotspot_A.tga", VisionColors.Red, HotSpotBase.PickType.Square, 4.0f);
      _hotSpotAngle.Distance = 200.0f;
      _hotSpotAngle.StartAngle = HotSpotAngleValue;
      _hotSpotAngle.ToolTipText = "cone angle";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotAngle); // register it so vForge can respond to it

      //hotspot for color
      _hotSpotColor = new HotSpotColorButton(this, "ShapeColor");
      _hotSpotColor.ToolTipText = "color";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotColor); // register it so vForge can respond to it
    }

    public override void OnUnSelected()
    {
      base.OnUnSelected();

      // release the hotspots again:
      EditorManager.ActiveView.HotSpots.Remove(_hotSpotAngle);
      EditorManager.ActiveView.HotSpots.Remove(_hotSpotColor);
      _hotSpotAngle.Remove();
      _hotSpotColor.Remove();
      _hotSpotAngle = null;
      _hotSpotColor = null;
    }


    /// <summary>
    /// Called when the user releases the mouse button on a hotspot
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragEnd(HotSpotBase hotSpot, VisionViewBase view)
    {
      if (hotSpot == _hotSpotAngle)
      {
        if (_hotSpotAngle.HasChanged)
        {
          float fNewAngle = _hotSpotAngle.CurrentAngle;
          HotSpotAngleValue = _hotSpotAngle.StartAngle; // set old angle for the action
          EditorManager.Actions.Add(new SetPropertyAction(this, "HotSpotAngleValue", fNewAngle)); // send an action which sets the property from old value to new one
        }
      }
    }



    #endregion

    #region Properties

    Color _color = Color.Black;
    float _hotSpotAngleValue = 0;


    [Description("Color that can be changed with Hotspot of Shape")]
    [EditorAttribute(typeof(StandardColorDropDownEditor), typeof(UITypeEditor))]
    public Color ShapeColor
    {
      get { return _color; }
      set { _color = value;}
    }

    [Description("Angle that can be changed with Hotspot of Shape")]
    public float HotSpotAngleValue
    {
      get { return _hotSpotAngleValue; }
      set 
      {
        if (value > 179)
          _hotSpotAngleValue = 179;
        else if (value < 0) 
          _hotSpotAngleValue = 0;
        else
          _hotSpotAngleValue = value;

        if (_hotSpotAngle != null)
        {
          _hotSpotAngle.StartAngle = _hotSpotAngleValue;
        }
      }
    }

    #endregion
  }
  #endregion

  #region HotSpotShape Creator Plugin


  /// <summary>
  /// Creator class. An instance of the creator is registerd in the plugin init function. Thus the creator shows
  /// up in the "Create" menu of the editor
  /// </summary>
  class HotSpotShapeCreator : CSharpFramework.IShapeCreatorPlugin
  {

    /// <summary>
    /// Constructor
    /// </summary>
    public HotSpotShapeCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\Node.bmp"), Color.Magenta);
    }

    /// <summary>
    /// Get the name of the plugin, for instance the shape name. This name apears in the "create" menu
    /// </summary>
    /// <returns>creator name</returns>
    public override string GetPluginName()
    {
      return "HotSpotShape";
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
      return "Creates a HotSpotShape instance.\n" +
        "This shape is just a sample for custom vForge Hotspots. It does not have any engine instance and will not be shown in engine view.";
    }

    public override ShapeBase CreateShapeInstance()
    {
      HotSpotShape shape = new HotSpotShape("HotSpotShape");
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
