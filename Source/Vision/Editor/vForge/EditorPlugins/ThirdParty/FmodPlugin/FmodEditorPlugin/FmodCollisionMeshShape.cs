/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.IO;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Design;
using System.ComponentModel;
using System.Globalization;
using CSharpFramework;
using CSharpFramework.Shapes;
using CSharpFramework.Actions;
using FmodManaged;
using System.Runtime.Serialization;
using ManagedFramework;
using CSharpFramework.PropertyEditors;
using CSharpFramework.View;
using CSharpFramework.Serialization;
using CSharpFramework.Math;
using CSharpFramework.Scene;

namespace FmodEditorPlugin
{
  #region FmodCollisionMeshShape

	/// <summary>
	/// Shape that defines a collision mesh for sounds/ events. 
	/// </summary>
  [Serializable]
  public class FmodCollisionMeshShape : ShapeObject3D, IPropertyValueChangingCallback
	{
    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_COLLMESH = "CollisionMesh";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_COLLMESH = Shape3D.LAST_CATEGORY_ORDER_ID + 1;

    /// <summary>
    /// last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_COLLMESH;

    #endregion
    
    #region Constructor

    /// <summary>
    /// Constructor: Takes the name of the shape
    /// </summary>
    /// <param name="name"></param>
    public FmodCollisionMeshShape(string name) : base(name)
		{
		}
  
    #endregion

    #region Base Overrides

    protected override Shape3D.ParentingModes ParentingMode
    {
      get
      {
        return ParentingModes.SceneGraphScaled;
      }
    }

    #endregion

    #region GUI

    /// <summary>
    /// Overridden function to get the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetIconIndex()
    {
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_soundcollision.bmp"), Color.Magenta);
    }

    #endregion

    #region Engine Instance

    /// <summary>
    /// Function to create the engine instance of the shape. The engine instance is of type EngineInstanceFmodCollisionMesh
    /// and located in the managed code library.
    /// </summary>
    /// <param name="bCreateChildren">relevant for the base class to create instances for children</param>
    public override void CreateEngineInstance(bool bCreateChildren)
    {
      _engineInstance = new EngineInstanceFmodCollisionMesh();
      SetEngineInstanceBaseProperties(); // sets the position etc.
      base.CreateEngineInstance (bCreateChildren);
    }

    /// <summary>
    /// Helper field to access the engine instance as casted class to perform specific operations on it
    /// </summary>
    EngineInstanceFmodCollisionMesh EngineFmodCollisionMesh {get {return (EngineInstanceFmodCollisionMesh)_engineInstance;}}

    /// <summary>
    /// Overridden render function: Let the engine instance render itself
    /// </summary>
    /// <param name="view"></param>
    /// <param name="mode"></param>
    public override void RenderShape(VisionViewBase view, ShapeRenderMode mode)
    {
      base.RenderShape(view, mode);
    }

    /// <summary>
    /// Set all properties on the engine instance. The base implementation already sets position and orientation,
    /// so there is nothing else to do here
    /// </summary>
    public override void SetEngineInstanceBaseProperties()
    {
      if (_engineInstance==null)
        return;
      base.SetEngineInstanceBaseProperties (); // now set position etc.
      EngineFmodCollisionMesh.SetCollisionMesh(_meshFilename, _fDirectOcclusion, _fReverbOcclusion);
      EngineFmodCollisionMesh.SetSelected(Selected);
    }

    void OnCollisionMeshPropertyChanged()
    {
      RemoveNativeComponents(); // since the native object will change, also re-assign new native components
      SetEngineInstanceBaseProperties();
      ReAttachComponents();
    }

    /// <summary>
    /// Pick the shape in the view. Use helper function to trace the local bounding box.
    /// </summary>
    /// <param name="rayStart">trace ray start</param>
    /// <param name="rayEnd">trace ray end</param>
    /// <param name="result">result structure to fill in</param>
    public override void OnTraceShape(ShapeTraceMode_e mode, Vector3F rayStart, Vector3F rayEnd, ref ShapeTraceResult result)
    {
      if (mode!=ShapeTraceMode_e.ShapePicking)
        return;
      if (ConversionUtils.TraceOrientedBoundingBox(LocalBoundingBox, Position, RotationMatrix, rayStart, rayEnd, ref result))
        result.hitShape = this;
    }

    #endregion

    #region Cloning

    public override ShapeBase Clone()
    {
      FmodCollisionMeshShape copy = (FmodCollisionMeshShape)base.Clone();
      System.Diagnostics.Debug.Assert(!copy.Selected); // turned off in ShapeBase

      return copy;
    }

    #endregion

    #region Collision Mesh properties

    string _meshFilename = "";
    float _fDirectOcclusion = 1.0f;
    float _fReverbOcclusion = 1.0f;
    bool _bActive = true;

    /// <summary>
    /// Gets or sets the mesh file that is used as a collision mesh
    /// </summary>
    [PrefabResolveFilename]
    [SortedCategory(CAT_COLLMESH, CATORDER_COLLMESH),
    PropertyOrder(1),
    EditorAttribute(typeof(FilenameEditor), typeof(UITypeEditor)), 
    FileDialogFilter(new string[] {".model", ".vmesh"})]
    [ThumbnailFormat(new string[] { @"{0}_data\Thumbnail.bmp", @"{0}_data\Thumbnail.jpg" })]
    [Description("The model/ static mesh that defines the Fmod collision mesh")]
    public string MeshFilename
    {
      get {return _meshFilename;}
      set 
      {
        if (_meshFilename==value) return;
        _meshFilename = value;
        OnCollisionMeshPropertyChanged();
      }
    }

    /// <summary>
    /// Defines the parameter for direct occlusion [0..1], where 0.0 means transparent and 1.0 means full occlusion
    /// </summary>
    [SortedCategory(CAT_COLLMESH, CATORDER_COLLMESH),
    PropertyOrder(2),
    Description("Defines the parameter for direct occlusion [0..1], where 0.0 means transparent and 1.0 means full occlusion.")]
    [EditorAttribute(typeof(SliderEditor),typeof(UITypeEditor)), SliderRange(0.0f, 1.0f, 100)]
    public float DirectOcclusion
    {
      get {return _fDirectOcclusion;}
      set
      {
        if (_fDirectOcclusion==value) return;
        _fDirectOcclusion = value;
        if (_fDirectOcclusion<0.0f) _fDirectOcclusion=0.0f;
        else if (_fDirectOcclusion>1.0f) _fDirectOcclusion=1.0f;
        if (HasEngineInstance())
          EngineFmodCollisionMesh.SetOcclusion(_fDirectOcclusion, _fReverbOcclusion);
      }
    }

    /// <summary>
    /// Defines the parameter for reverb reflection occlusion [0..1], where 0.0 means transparent and 1.0 means full occlusion
    /// </summary>
    [SortedCategory(CAT_COLLMESH, CATORDER_COLLMESH),
    PropertyOrder(2),
    Description("Defines the parameter for reverb reflection occlusion [0..1], where 0.0 means transparent and 1.0 means full occlusion.")]
    [EditorAttribute(typeof(SliderEditor),typeof(UITypeEditor)), SliderRange(0.0f, 1.0f, 100)]
    public float ReverbOcclusion
    {
      get {return _fReverbOcclusion;}
      set
      {
        if (_fReverbOcclusion==value) return;
        _fReverbOcclusion = value;
        if (_fReverbOcclusion<0.0f) _fReverbOcclusion=0.0f;
        else if (_fReverbOcclusion>1.0f) _fReverbOcclusion=1.0f;
        if (HasEngineInstance())
          EngineFmodCollisionMesh.SetOcclusion(_fDirectOcclusion, _fReverbOcclusion);
      }
    }
    
    #endregion

    #region IPropertyValueChangingCallback Members

    public void OnPropertyValueChanging(object Instance, PropertyDescriptor propertyDesc, object newValue)
    {
      if (object.ReferenceEquals(Instance, this) && HasEngineInstance())
      {
        switch (propertyDesc.Name)
        {
          case "DirectOcclusion":
            DirectOcclusion = (float)newValue;
            break;

          case "ReverbOcclusion":
            ReverbOcclusion = (float)newValue;
            break;
        }
      }
    }

    #endregion

    #region Selection

    public override void OnSelected()
    {
      base.OnSelected ();
      if (_engineInstance!=null)
        EngineFmodCollisionMesh.SetSelected(true);
    }

    public override void OnUnSelected()
    {
      base.OnUnSelected ();
      if (_engineInstance!=null)
        EngineFmodCollisionMesh.SetSelected(false);
    }

    #endregion

    #region Serialization and Export

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected FmodCollisionMeshShape(SerializationInfo info, StreamingContext context) : base(info, context)
    {
      _meshFilename = info.GetString("_meshFilename");
      _fDirectOcclusion = info.GetSingle("_fDirectOcclusion");
      _fReverbOcclusion = info.GetSingle("_fReverbOcclusion");
      _bActive = info.GetBoolean("_bActive");
    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context)
    {
      base.GetObjectData (info, context);

      info.AddValue("_meshFilename",_meshFilename);
      info.AddValue("_fDirectOcclusion", _fDirectOcclusion);
      info.AddValue("_fReverbOcclusion", _fReverbOcclusion);
      info.AddValue("_bActive", _bActive);
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

  #region Shape Creator Plugin

  /// <summary>
  /// Creator class. An instance of the creator is registered in the plugin init function. Thus the creator shows
  /// up in the "Create" menu of the editor
  /// </summary>
  class FmodCollisionMeshShapeCreator : CSharpFramework.IShapeCreatorPlugin
  {

    /// <summary>
    /// Constructor
    /// </summary>
    public FmodCollisionMeshShapeCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_soundcollision.bmp"), Color.Magenta);
    }

    /// <summary>
    /// Get the name of the plugin, for instance the shape name. This name appears in the "create" menu
    /// </summary>
    /// <returns>creator name</returns>
    public override string GetPluginName()
    {
      return "Collision Mesh";
    }

    /// <summary>
    /// Get the plugin category name to sort the plugin name. This is useful to group creators. A null string can
    /// be returned to put the creator in the root
    /// </summary>
    /// <returns></returns>
    public override string GetPluginCategory()
    {
      return "Fmod";
    }

    /// <summary>
    /// Gets the plugin description
    /// </summary>
    /// <returns></returns>
    public override string GetPluginDescription()
    {
      return "This shape represents a Fmod collision mesh in the engine. Fmod collision meshes block or dampen sounds/ events which are behind the collision mesh.";
    }

    /// <summary>
    /// Create a shape instance
    /// </summary>
    /// <returns></returns>
    public override ShapeBase CreateShapeInstance()
    {
      // create a shape
      FmodCollisionMeshShape shape = new FmodCollisionMeshShape("Fmod - CollisionMesh");
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
