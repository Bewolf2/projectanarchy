/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.IO;
using System.Drawing.Design;
using System.ComponentModel;
using System.Globalization;
using System.Runtime.Serialization;
using CSharpFramework;
using ManagedFramework;
using CSharpFramework.Math;
using CSharpFramework.Shapes;
using CSharpFramework.PropertyEditors;
using CSharpFramework.DynamicProperties;
using CSharpFramework.UndoRedo;
using CSharpFramework.Actions;
using CSharpFramework.Scene;
using CSharpFramework.View;
using VisionManaged;
using CSharpFramework.Serialization;
using System.Drawing;

namespace VisionEditorPlugin.Shapes
{
	/// <summary>
	/// Shape for displaying light grid colors at specific position.
	/// </summary>
  [Serializable, ShapeTypeNiceName("Lightgrid Indicator")]
  public class LightGridIndicatorShape : Shape3D
	{
    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_PREVIEW      = "Preview";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_PREVIEW    = Shape3D.LAST_CATEGORY_ORDER_ID + 1;


    /// <summary>
    /// Last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_PREVIEW;

    #endregion

    #region Constructor

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="nodeName"></param>
    public LightGridIndicatorShape(string nodeName) : base(nodeName)
    {
			AddHint(HintFlags_e.NoRotation|HintFlags_e.NoScale);
      this._fDisplayRadius *= EditorManager.Settings.GlobalUnitScaling;
    }

    #endregion

    #region GUI

    /// <summary>
    /// Overridden function to get the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetIconIndex()
    {
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\Node.bmp"), Color.Magenta);
    }

    #endregion

    #region ShapeBase Members

    /// <summary>
    /// Create the engine instance
    /// </summary>
    /// <param name="bCreateChildren"></param>
    public override void CreateEngineInstance(bool bCreateChildren)
    {
      base.CreateEngineInstance(bCreateChildren);

      this._engineInstance = new EngineInstanceLightGridIndicator();
      SetEngineInstanceBaseProperties(); // sets the position etc.
    }

    // accesses the engine instance as an object of type EngineInstanceLightGridDetailBox
    EngineInstanceLightGridIndicator EngineIndicator {get {return (EngineInstanceLightGridIndicator)_engineInstance;}}

    /// <summary>
    /// overridden function to set/update all specific properties on the engine instance
    /// </summary>
    public override void SetEngineInstanceBaseProperties()
    {
      if (_engineInstance==null)
        return;

      base.SetEngineInstanceBaseProperties ();
      if (!HasEngineInstance())
        return;
      EngineIndicator.SetRadius(_fDisplayRadius);
      EngineIndicator.SetRenderGrid(_bRenderGrid);
    }


    /// <summary>
    /// overridden function to render the shape
    /// </summary>
    /// <param name="view">destination view</param>
    /// <param name="mode">render mode</param>
    public override void RenderShape(VisionViewBase view, ShapeRenderMode mode)
    {
      base.RenderShape(view, mode);

      if (_engineInstance!=null)
        EngineIndicator.RenderShape(view,mode);
    }

    #endregion

    #region ISerializable

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected LightGridIndicatorShape(SerializationInfo info, StreamingContext context) : base(info, context)
    {
      _fDisplayRadius = info.GetSingle("_fDisplayRadius");
      _bRenderGrid = info.GetBoolean("_bRenderGrid");
			AddHint(HintFlags_e.NoRotation|HintFlags_e.NoScale);
		}

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context) 
    {
      base.GetObjectData(info,context);
      info.AddValue("_fDisplayRadius",_fDisplayRadius);
      info.AddValue("_bRenderGrid",_bRenderGrid);
    }

    #endregion

    #region Indicator properties

    float _fDisplayRadius = 20.0f;
    bool _bRenderGrid = true;

    /// <summary>
    /// Gets or sets the display radius
    /// </summary>
    [SortedCategory(CAT_PREVIEW, CATORDER_PREVIEW), PropertyOrder(1)]
    [Description("Determines the display radius of the indicator. Brightness is taken from the center.")]
    public float DisplayRadius
    {
      get {return _fDisplayRadius;}
      set
      {
        _fDisplayRadius = value;
        if (HasEngineInstance())
          EngineIndicator.SetRadius(_fDisplayRadius);
      }
    }


    /// <summary>
    /// Flag that determines whether the light grid should be rendered around the indicator position
    /// </summary>
    [SortedCategory(CAT_PREVIEW, CATORDER_PREVIEW), PropertyOrder(2)]
    [Description("Flag that determines whether the light grid should be rendered around the indicator position.")]
    public bool RenderGrid
    {
      get {return _bRenderGrid;}
      set
      {
        _bRenderGrid = value;
        if (HasEngineInstance())
          EngineIndicator.SetRenderGrid(_bRenderGrid);
      }
    }

    #endregion

    #region Plugin information

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <returns></returns>
    public override EditorPluginInfo GetPluginInformation()
    {
      return EditorPlugin.EDITOR_PLUGIN_INFO;
    }

    #endregion

  }

  #region Class : Creator plugin

  class LightGridIndicatorCreator : CSharpFramework.IShapeCreatorPlugin
  {
    public LightGridIndicatorCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\Node.bmp"), Color.Magenta);
    }

    public override string GetPluginName()
    {
      return "Lightgrid - Indicator";
    }

    public override string GetPluginCategory()
    {
      return "Lighting";
    }

    /// <summary>
    /// Returns a short description text
    /// </summary>
    /// <returns></returns>
    public override string GetPluginDescription()
    {
      return "A helper shape to evaluate the light grid colors at the shape's position.\n"+
        "The shape will not be exported, istead it can only be used to debug the lightgrid outcome.";
    }

    public override ShapeBase CreateShapeInstance()
    {
      LightGridIndicatorShape shape = new LightGridIndicatorShape("LightIndicator");
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
