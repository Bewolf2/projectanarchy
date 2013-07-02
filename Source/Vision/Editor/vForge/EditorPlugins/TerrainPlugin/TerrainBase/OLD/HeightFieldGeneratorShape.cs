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
using System.Windows.Forms;
using CSharpFramework;
using ManagedFramework;
using CSharpFramework.Math;
using CSharpFramework.Shapes;
using CSharpFramework.PropertyEditors;
using CSharpFramework.UndoRedo;
using System.Drawing;
using System.Collections;
using CSharpFramework.Actions;
using CSharpFramework.Serialization;
using CSharpFramework.View;
using VisionManaged;


namespace TerrainBase.Shapes
{


	/// <summary>
	/// The height field generator shape class
	/// </summary>
  [Serializable, ShapeTypeNiceName("Heightfield Generator")]
  public class HeightFieldGeneratorShape : Shape3D
  {

    #region Enumerators

    public enum Lightmapresolution_e
    {
      none = 0,
      _128x128_ = 128,
      _256x256_ = 256,
      _512x512_ = 512,
      _1024x1024_ = 1024
    };

    public enum Heightmapsize_e
    {
      _33x33_ = 33,
      _65x65_ = 65,
      _129x129_ = 129,
      _257x257_ = 257,
      _513x513_ = 513,
      _1025x1025_ = 1025
    };

    #endregion Enumerators

    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_DIMENSION = "Dimension";

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_HEIGHTMAP = "Heightmap";

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_BASETEXTURE = "Basetexture";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_DIMENSION = Shape3D.LAST_CATEGORY_ORDER_ID + 1;

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_HEIGHTMAP = Shape3D.LAST_CATEGORY_ORDER_ID + 2;

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_BASETEXTURE = Shape3D.LAST_CATEGORY_ORDER_ID + 3;

    /// <summary>
    /// Last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_DIMENSION;

    #endregion

    #region Constructor

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="nodeName">name of the node</param>
    /// <param name="_type">constarint type. Can be changed but is used for some special init sizes</param>
    public HeightFieldGeneratorShape(string nodeName) : base(nodeName)
    {
    }

    #endregion

    #region GUI

    /// <summary>
    /// Overridden function to get the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetIconIndex()
    {
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"TerrainPlugin\Icons\Heightfield.BMP"), Color.Magenta);
    }

    #endregion

    #region Shape members

    /// <summary>
    /// Create the engine instance
    /// </summary>
    /// <param name="bCreateChildren"></param>
    public override void CreateEngineInstance(bool bCreateChildren)
    {
      this._engineInstance = new EngineInstanceHeightfieldGenerator();
      SetEngineInstanceBaseProperties(); // sets the position etc.

      // Call the base function to setup attachments and process the children. The engine instance
      // has to exist at this point.
      base.CreateEngineInstance(bCreateChildren);
    }

    public EngineInstanceHeightfieldGenerator EngineHeightfieldGenerator{get {return _engineInstance as EngineInstanceHeightfieldGenerator;}}

    /// <summary>
    /// overridden function to render the shape
    /// </summary>
    /// <param name="view">destination view</param>
    /// <param name="mode">render mode</param>
    public override void RenderShape(VisionViewBase view, ShapeRenderMode mode)
    {
      base.RenderShape(view, mode);

      if (EngineHeightfieldGenerator == null)
        return;

      EngineHeightfieldGenerator.RenderShape(view,mode);
    }

    /// <summary>
    /// overridden function to set/update all specific properties on the engine instance
    /// </summary>
    public override void SetEngineInstanceBaseProperties()
    {
      if (_engineInstance==null) return;
      base.SetEngineInstanceBaseProperties();
      EngineHeightfieldGenerator.SetBoxSize(_vBoxSize.X,_vBoxSize.Y,_vBoxSize.Z);
    }

    #endregion

    #region Cloning

    /// <summary>
    /// This function must be overridden, since we have to reset the hotspots
    /// </summary>
    /// <returns></returns>
    public override ShapeBase Clone()
    {
      HeightFieldGeneratorShape copy = base.Clone() as HeightFieldGeneratorShape;

      copy._hotSpotSizeX = null;
      copy._hotSpotSizeY = null;
      copy._hotSpotSizeZ = null;
      System.Diagnostics.Debug.Assert(!copy.Selected); // turned off in ShapeBase

      return copy;
    }

    #endregion

    #region Properties

    Vector3F _vBoxSize = new Vector3F(16000,16000,8000);
    string _heightmapfile = "Textures/heightmap_1.tex";
    Heightmapsize_e _heightmapsize = Heightmapsize_e._513x513_;
    string _basetexturefile = "";
    Lightmapresolution_e _basetextureLightmapresolution = Lightmapresolution_e._512x512_;

    /// <summary>
    /// box dimension
    /// </summary>
    [BrowsableAttribute(false)]
    public Vector3F BoxSize
    {
      get {return _vBoxSize;}
      set 
      {
        _vBoxSize = value.Clone();
        if (_engineInstance!=null)
          EngineHeightfieldGenerator.SetBoxSize(_vBoxSize.X,_vBoxSize.Y,_vBoxSize.Z);
      }
    }

    /// <summary>
    /// box size x
    /// </summary>
    [SortedCategory(CAT_DIMENSION, CATORDER_DIMENSION),
    Description("Heightfield SizeX"),
    PropertyOrder(1)]
    public float SizeX
    {
      get {return _vBoxSize.X;}
      set 
      {
        if (_vBoxSize.X == value) return;
        _vBoxSize.X = value;
        if (_engineInstance!=null)
          EngineHeightfieldGenerator.SetBoxSize(_vBoxSize.X,_vBoxSize.Y,_vBoxSize.Z);
      }
    }

    /// <summary>
    /// box size y
    /// </summary>
    [SortedCategory(CAT_DIMENSION, CATORDER_DIMENSION),
    Description("Heightfield SizeY"),
    PropertyOrder(2)]
    public float SizeY
    {
      get {return _vBoxSize.Y;}
      set 
      {
        if (_vBoxSize.Y == value) return;
        _vBoxSize.Y = value;
        if (_engineInstance!=null)
          EngineHeightfieldGenerator.SetBoxSize(_vBoxSize.X,_vBoxSize.Y,_vBoxSize.Z);
      }
    }

    /// <summary>
    /// box size z
    /// </summary>
    [SortedCategory(CAT_DIMENSION, CATORDER_DIMENSION),
    Description("Heightfield SizeZ"),
    PropertyOrder(3)]
    public float SizeZ
    {
      get {return _vBoxSize.Z;}
      set 
      {
        if (_vBoxSize.Z == value) return;
        _vBoxSize.Z = value;
        if (_engineInstance!=null)
          EngineHeightfieldGenerator.SetBoxSize(_vBoxSize.X,_vBoxSize.Y,_vBoxSize.Z);
      }
    }

    /// <summary>
    /// get or set the heightmap filename
    /// </summary>
    [SortedCategory(CAT_HEIGHTMAP, CATORDER_HEIGHTMAP),
    Description("Heightmap file"),
    PropertyOrder(1),
    EditorAttribute(typeof(FilenameCreator), typeof(UITypeEditor)), 
    FileDialogFilter(new string[] {".tex"}),
    FileCreateDialogAllowOverwrite(true)]
    public string Heightmapfile {get {return _heightmapfile;} set {_heightmapfile=value;}}

    /// <summary>
    /// get the full path of the heightmap file
    /// </summary>
    [Browsable(false)]
    public string HeightmapfileFullPath 
    {
      get 
      {
        return Path.Combine(CSharpFramework.EditorManager.Scene.Project.ProjectDir, _heightmapfile);
      }
    }

    /// <summary>
    /// get or set the heightmap size
    /// </summary>
    [SortedCategory(CAT_HEIGHTMAP, CATORDER_HEIGHTMAP),
    Description("Heightmap size"),
    PropertyOrder(2)]
    public Heightmapsize_e Heightmapsize {get {return _heightmapsize;} set {_heightmapsize=value;}}

    /// <summary>
    /// get or set the basetexture filename
    /// </summary>
    [SortedCategory(CAT_BASETEXTURE, CATORDER_BASETEXTURE),
    Description("Basetexture Filename"),
    PropertyOrder(1),
    EditorAttribute(typeof(BitmapBrowserEditor), typeof(UITypeEditor))]
    public string Basetexturefile {get {return _basetexturefile;} set {_basetexturefile=value;}}

    /// <summary>
    /// get the full path of the Basetexture file
    /// </summary>
    [Browsable(false)]
    public string BasetexturefileFullPath 
    {
      get 
      {
        return Path.Combine(CSharpFramework.EditorManager.Scene.Project.ProjectDir, _basetexturefile);
      }
    }

    /// <summary>
    /// get or set the basetexture lightmap resolution
    /// </summary>
    [SortedCategory(CAT_BASETEXTURE, CATORDER_BASETEXTURE),
    Description("Basetexture lightmap resolution"),
    PropertyOrder(3)]
    public Lightmapresolution_e BasetextureLightmapresolution {get {return _basetextureLightmapresolution;} set {_basetextureLightmapresolution=value;}}

    #endregion

    #region ISerializable

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected HeightFieldGeneratorShape(SerializationInfo info, StreamingContext context) : base(info, context)
    {
      _vBoxSize = (Vector3F) info.GetValue( "_vBoxSize", typeof(Vector3F) );
      _heightmapfile = info.GetString("_heightmapfile");
      _heightmapsize = (Heightmapsize_e)info.GetValue( "_heightmapsize", typeof(Heightmapsize_e) );
      _basetexturefile = info.GetString("_basetexturefile");
      _basetextureLightmapresolution = (Lightmapresolution_e)info.GetValue("_basetextureLightmapresolution", typeof(Lightmapresolution_e) );      
    }

  /// <summary>
  /// Called when serializing
  /// </summary>
  /// <param name="info"></param>
  /// <param name="context"></param>
  public override void GetObjectData(SerializationInfo info, StreamingContext context) 
  {
    base.GetObjectData(info,context);
    info.AddValue("_vBoxSize", _vBoxSize);
    info.AddValue("_heightmapfile", _heightmapfile);
    info.AddValue("_heightmapsize", _heightmapsize);
    info.AddValue("_basetexturefile", _basetexturefile);
    info.AddValue("_basetextureLightmapresolution", _basetextureLightmapresolution);
  }

  #endregion

    #region Hotspot

    HotSpotDistance _hotSpotSizeX;        // a hotspot for the AABox
    HotSpotDistance _hotSpotSizeY;        // a hotspot for the AABox
    HotSpotDistance _hotSpotSizeZ;        // a hotspot for the AABox

    /// <summary>
    /// Called when the shape is selected
    /// </summary>
    public override void OnSelected()
    {
      base.OnSelected ();
      System.Diagnostics.Debug.Assert(_hotSpotSizeX==null);
      System.Diagnostics.Debug.Assert(_hotSpotSizeY==null);
      System.Diagnostics.Debug.Assert(_hotSpotSizeZ==null);

      // create the hotspots:

      // hotspot for size X
      _hotSpotSizeX = new HotSpotDistance(this,@"textures\Hotspot_Rect.dds",VisionColors.Yellow,HotSpotBase.PickType.Square,4.0f);
      _hotSpotSizeX.StartDistance = this.SizeX;
      _hotSpotSizeX.DisplayScaling = 0.5f;
      _hotSpotSizeX.Axis = this.XAxis;
      _hotSpotSizeX.ToolTipText = "size x direction";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotSizeX);

      // hotspot for size Y
      _hotSpotSizeY = new HotSpotDistance(this,@"textures\Hotspot_Rect.dds",VisionColors.Yellow,HotSpotBase.PickType.Square,4.0f);
      _hotSpotSizeY.StartDistance = this.SizeY;
      _hotSpotSizeY.DisplayScaling = 0.5f;
      _hotSpotSizeY.Axis = this.YAxis;
      _hotSpotSizeY.ToolTipText = "size y direction";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotSizeY);
      
      // hotspot for size Z
      _hotSpotSizeZ = new HotSpotDistance(this,@"textures\Hotspot_Rect.dds",VisionColors.Yellow,HotSpotBase.PickType.Square,4.0f);
      _hotSpotSizeZ.StartDistance = this.SizeZ;
      _hotSpotSizeZ.DisplayScaling = 0.5f;
      _hotSpotSizeZ.Axis = this.ZAxis;
      _hotSpotSizeZ.ToolTipText = "size z direction";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotSizeZ);

      // add more hotspots here...
    }


    void SafeRemoveHotSpot(HotSpotBase hotSpot)
    {
      if (hotSpot==null) return;
      EditorManager.ActiveView.HotSpots.Remove(hotSpot);
      hotSpot.Remove();
    }

    /// <summary>
    /// Called when the shape is unselected
    /// </summary>
    public override void OnUnSelected()
    {
      // release the hotspots again:
      SafeRemoveHotSpot(_hotSpotSizeX);_hotSpotSizeX=null;
      SafeRemoveHotSpot(_hotSpotSizeY);_hotSpotSizeY=null;
      SafeRemoveHotSpot(_hotSpotSizeZ);_hotSpotSizeZ=null;

      base.OnUnSelected ();
    }


    /// <summary>
    /// Called when user starts dragging a hotspot that belongs to this shape
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragBegin(HotSpotBase hotSpot, VisionViewBase view)
    {
      if (hotSpot==_hotSpotSizeX)
        _hotSpotSizeX.StartDistance = this.SizeX;
      if (hotSpot==_hotSpotSizeY)
        _hotSpotSizeY.StartDistance = this.SizeY;
      if (hotSpot==_hotSpotSizeZ)
        _hotSpotSizeZ.StartDistance = this.SizeZ;
    }

    /// <summary>
    /// Called while the user drags a hotspot that belongs to this shape
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    /// <param name="fDeltaX"></param>
    /// <param name="fDeltaY"></param>
    public override void OnHotSpotDrag(HotSpotBase hotSpot, VisionViewBase view, float fDeltaX, float fDeltaY)
    {
      if (hotSpot==_hotSpotSizeX)
        this.SizeX = _hotSpotSizeX.CurrentDistance; 
      if (hotSpot==_hotSpotSizeY)
        this.SizeY = _hotSpotSizeY.CurrentDistance; 
      if (hotSpot==_hotSpotSizeZ)
        this.SizeZ = _hotSpotSizeZ.CurrentDistance; 
    }

    /// <summary>
    /// Called when the user releases the mouse button on a hotspot
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragEnd(HotSpotBase hotSpot, VisionViewBase view)
    {
      if (hotSpot==_hotSpotSizeX)
      {
        if (_hotSpotSizeX.HasChanged)
        {
          float fNewSize = _hotSpotSizeX.CurrentDistance;
          this.SizeX = _hotSpotSizeX.StartDistance; // set old value for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"SizeX",fNewSize)); // send an action which sets the property from old value to new one
        }
      }
      if (hotSpot==_hotSpotSizeY)
      {
        if (_hotSpotSizeY.HasChanged)
        {
          float fNewSize = _hotSpotSizeY.CurrentDistance;
          this.SizeY = _hotSpotSizeY.StartDistance; // set old value for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"SizeY",fNewSize)); // send an action which sets the property from old value to new one
        }
      }
      if (hotSpot==_hotSpotSizeZ)
      {
        if (_hotSpotSizeZ.HasChanged)
        {
          float fNewSize = _hotSpotSizeZ.CurrentDistance;
          this.SizeZ = _hotSpotSizeZ.StartDistance; // set old value for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"SizeZ",fNewSize)); // send an action which sets the property from old value to new one
        }
      }
    }

    /// <summary>
    /// Called every frame and per hotspot that belongs to the shape
    /// </summary>
    /// <param name="hotSpot"></param>
    public override void OnHotSpotEvaluatePosition(HotSpotBase hotSpot)
    {
      if (hotSpot==_hotSpotSizeX)
      {
        _hotSpotSizeX.Axis = this.XAxis;
        _hotSpotSizeX.Visible = true;
        if (!_hotSpotSizeX.IsDragging)
          _hotSpotSizeX.StartDistance = this.SizeX;
      }
      if (hotSpot==_hotSpotSizeY)
      {
        _hotSpotSizeY.Axis = this.YAxis;
        _hotSpotSizeY.Visible = true;
        if (!_hotSpotSizeY.IsDragging)
          _hotSpotSizeY.StartDistance = this.SizeY;
      }
      if (hotSpot==_hotSpotSizeZ)
      {
        _hotSpotSizeZ.Axis = this.ZAxis;
        _hotSpotSizeZ.Visible = true;
        if (!_hotSpotSizeZ.IsDragging)
          _hotSpotSizeZ.StartDistance = this.SizeZ;
      }
    }

    #endregion

    #region relevant Operation

    /// <summary>
    /// Return relevant operations for heightfield shape
    /// </summary>
    public override ArrayList RelevantOperations
    {
      get
      {
        ArrayList op = base.RelevantOperations;
        if (op==null) op = new ArrayList(4);
        op.Add(@"Export V3D");
        op.Add(@"Export V3D and add to main layer");
        return op;
      }
    }

    public override void PerformRelevantOperation(string name, int iShapeIndex, int iShapeCount)
    {
      base.PerformRelevantOperation (name, iShapeIndex, iShapeCount);
      if (name=="Export V3D")
      {
        if (OnTriggerExport != null)
        {
          OnTriggerExport(this, new OnTriggerExportEventArgs(false));
        }
      }
      else if (name=="Export V3D and add to main layer")
      {
        if (OnTriggerExport != null)
        {
          OnTriggerExport(this, new OnTriggerExportEventArgs(true));
        }
      }
    }


    #endregion

    #region events

    /// <summary>
    /// Delegate
    /// </summary>
    public delegate void OnTriggerExportEventHandler(object sender, OnTriggerExportEventArgs e);

    /// <summary>
    /// event that is triggered when the value changed
    /// </summary>
    public static event OnTriggerExportEventHandler OnTriggerExport;

    /// <summary>
    /// Event args for the export trigger
    /// </summary>
    public class OnTriggerExportEventArgs : EventArgs
    {
      /// <summary>
      /// constructor
      /// </summary>
      public OnTriggerExportEventArgs()
      {
      }

      /// <summary>
      /// constructor
      /// </summary>
      /// <param name="_openV3D"></param>
      public OnTriggerExportEventArgs(bool _openV3D)
      {
        OpenV3D = _openV3D;
      }

      public bool OpenV3D = false;
    }

    #endregion events

  }

#region HeightFieldGeneratorShapeCreator Plugins

  public class HeightFieldGeneratorShapeCreator : CSharpFramework.IShapeCreatorPlugin
  {

    // create a HeightFieldGeneratorShapeCreator
    public HeightFieldGeneratorShapeCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"TerrainPlugin\Icons\Heightfield.BMP"), Color.Magenta);
    }
    
    public override string GetPluginName() 
    {
      return "Heightfield Generator";
    }

    /// <summary>
    /// Returns a short description text
    /// </summary>
    /// <returns></returns>
    public override string GetPluginDescription()
    {
      return "Adds a height field generator shape to the scene.\n"+
        "This shape can generate a new V3D file with just a height field in it.\n"+
        "For more information, please refer to the Terrain Editor documentation.";
    }

    public override ShapeBase CreateShapeInstance()
    {
      ShapeCollection allGenerators = EditorManager.Scene.AllShapesOfType(typeof(HeightFieldGeneratorShape));
      if (allGenerators.Count == 0)
      {
        HeightFieldGeneratorShape generator = new HeightFieldGeneratorShape(GetPluginName());
        generator.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
        return generator;
      }

      EditorManager.ShowMessageBox("Only one height field generator shape per scene can be set.", "Warning", MessageBoxButtons.OK); 
      return null;
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
