/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.IO;
using System.Diagnostics;
using System.Collections;
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
  #region Class PathCameraShape

	/// <summary>
	/// Simple shape for path following.
	/// </summary>
  [Serializable, ShapeTypeNiceName("Path Camera")]
  public class PathCameraShape : ShapeObject3D
	{
    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_PATHCAMERA = "Path Camera";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_PATHCAMERA = Shape3D.LAST_CATEGORY_ORDER_ID + 1;

    /// <summary>
    /// last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_PATHCAMERA;

    #endregion

    #region Constructor

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="name"></param>
		public PathCameraShape(string name) : base(name)
		{
			AddHint(HintFlags_e.NoRotation | HintFlags_e.NoScale);
		}

    #endregion

    #region GUI

    /// <summary>
    /// Overridden function to get the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetIconIndex()
    {
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\VideoCamera.bmp"), Color.Magenta);
    }

    #endregion

    #region ShapeBase Members

    /// <summary>
    /// Create the engine instance
    /// </summary>
    /// <param name="bCreateChildren"></param>
    public override void CreateEngineInstance(bool bCreateChildren)
    {
      // create all children (important here, because the path node needs the children path nodes)
      base.CreateEngineInstance(bCreateChildren);

      this._engineInstance = new EngineInstancePathCamera();      
      SetEngineInstanceBaseProperties(); // sets the position etc.
    }

    /// <summary>
    /// accesses the engine instance as an object of type EngineInstancePathCamera
    /// </summary>
    [BrowsableAttribute(false)]
    public EngineInstancePathCamera EngineCamera {get {return (EngineInstancePathCamera)_engineInstance;}}

    /// <summary>
    /// overridden function to set/update all specific properties on the engine instance
    /// </summary>
    public override void SetEngineInstanceBaseProperties()
    {
      if (_engineInstance==null)
        return;

      base.SetEngineInstanceBaseProperties ();
      EngineCamera.SetScriptFile(_scriptFile);
    }

    #endregion

    #region ISerializable

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected PathCameraShape(SerializationInfo info, StreamingContext context) : base(info, context)
    {
      _scriptFile = info.GetString("_scriptFile");
			AddHint(HintFlags_e.NoRotation | HintFlags_e.NoScale);
		}

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context) 
    {
      base.GetObjectData(info,context);

      info.AddValue("_scriptFile",_scriptFile);
    }

    #endregion

    #region PathCamera Properties

    string _scriptFile = null;

    /// <summary>
    /// XML file that contains information about the camera actions
    /// </summary>
    [PrefabResolveFilename]
    [SortedCategory(CAT_PATHCAMERA, CATORDER_PATHCAMERA), PropertyOrder(1),
    EditorAttribute(typeof(FilenameEditor), typeof(UITypeEditor)), FileDialogFilter(new string[] {".xml"}),
    Description("XML file that contains information about the camera actions")]
    public string CameraScriptFile
    {
      get {return _scriptFile;}
      set 
      {
        _scriptFile=value;
        if (HasEngineInstance())
          EngineCamera.SetScriptFile(_scriptFile);
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

  #endregion

  #region Creator Plugin

  class PathCameraShapeCreator : CSharpFramework.IShapeCreatorPlugin
  {
    public PathCameraShapeCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\VideoCamera.bmp"), Color.Magenta);
    }

    /// <summary>
    /// Get the name of the creator
    /// </summary>
    /// <returns></returns>
    public override string GetPluginName()
    {
      return "Path Camera";
    }


    /// <summary>
    /// Get the category's name
    /// </summary>
    /// <returns></returns>
    public override string GetPluginCategory()
    {
      return "Objects";
    }

    /// <summary>
    /// Returns a short description text
    /// </summary>
    /// <returns></returns>
    public override string GetPluginDescription()
    {
      return "Adds a path camera shape to the scene that loads a simple xml file "+
        "with some playback instructions. The path camera can follow paths. Please refer to the sample map.\n";
    }


    /// <summary>
    /// Overridden function to create the shape instance
    /// </summary>
    /// <returns></returns>
    public override ShapeBase CreateShapeInstance()
    {
      PathCameraShape shape = new PathCameraShape("PathCamera");
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
