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
using System.Drawing;
using CSharpFramework.Actions;
using System.Diagnostics;
using VisionManaged;
using System.Collections;
using System.Windows.Forms;
using CSharpFramework.Serialization;
using CSharpFramework.Scene;
using CSharpFramework.View;
using CSharpFramework.Visitors;
using CSharpFramework.BaseTypes;
using CSharpFramework.Helper;

namespace VisionEditorPlugin.Shapes
{

  #region Linking

  /// <summary>
  /// Link target class for constraint target
  /// </summary>
  [Serializable]
  public class LinkTargetConstraint : LinkTarget
  {
    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="ownerShape"></param>
    /// <param name="primaryStringId"></param>
    public LinkTargetConstraint(Shape3D ownerShape, string primaryStringId) : base(ownerShape,"affected by",primaryStringId)
    {
    }

    /// <summary>
    /// Default constructor used by the factory class
    /// </summary>
    /// <param name="ownerShape"></param>
    /// <param name="linkName"></param>
    /// <param name="primaryStringId"></param>
    public LinkTargetConstraint(Shape3D ownerShape, string linkName, string primaryStringId)
      : base(ownerShape, linkName, primaryStringId)
    {
    }


    #region ISerializable Members

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected LinkTargetConstraint(SerializationInfo info, StreamingContext context) : base(info, context)
    {
    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info,context);
    }

    #endregion

    #region Hotspot

    /// <summary>
    /// get hot spot icon filename
    /// </summary>
    public override string DefaultHotspotIconFilename
    {
      get {return @"Bitmaps\Shapes\cube_molecule.png";} // constraint  icon
    }

    #endregion

  }

  #endregion

  #region editor args

  /// <summary>
  /// Event args for the particle editor
  /// </summary>
  public class ParticleEditorArgs : EventArgs
  {
    /// <summary>
    /// constructor
    /// </summary>
    /// <param name="_shape"></param>
    /// <param name="_action"></param>
    public ParticleEditorArgs(ParticleGroupShape _shape, Action _action)
    {
      action = _action;
      ParticleShape = _shape;
    }

    /// <summary>
    /// defines the action to be performed in the editor
    /// </summary>
    public enum Action 
    {
      /// <summary>
      /// Informs the editor, that the template of this particle shape should be edited
      /// </summary>
      Edit,

      /// <summary>
      /// Informs the editor, that the UI has to be updated
      /// </summary>
      UpdateUI

    };

    /// <summary>
    /// defines the action to be performed in the editor
    /// </summary>
    public Action action;

    /// <summary>
    /// defines the particle shape that triggered the event
    /// </summary>
    public ParticleGroupShape ParticleShape;

    /// <summary>
    /// Should be set to true by the editor
    /// </summary>
    public bool Success = false;

    /// <summary>
    /// Should be set to error string if Success==false
    /// </summary>
    public string ErrorString = null;
  }

  #endregion

	/// <summary>
	/// Particle emitter shape.
	/// </summary>
  [Serializable, ShapeTypeNiceName("Particle System")]
  public class ParticleGroupShape : ShapeObject3D, IShapeReferenceOwner
  {
    #region Nested class : LightingChangedVisitor

    public class LightingChangedVisitor : IShapeVisitor
    {
      public override IShapeVisitor.VisitResult Visit(ShapeBase shape)
      {
        if ((shape is ParticleGroupShape) && shape.HasEngineInstance())
          ((ParticleGroupShape)shape).EnginePGroup.UpdateParticleColors();
        return VisitResult.VisitOk;
      }
    }

    public static LightingChangedVisitor LIGHTING_CHANGED = new LightingChangedVisitor();

    #endregion

    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_PROPERTIES   = "Properties";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_PROPERTIES = Shape3D.LAST_CATEGORY_ORDER_ID + 1;

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_WIND = "Wind";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_WIND = Shape3D.LAST_CATEGORY_ORDER_ID + 2;

    /// <summary>
    /// last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_WIND;

    #endregion

    #region Constructor

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="nodeName">shape name</param>
    public ParticleGroupShape(string nodeName) : base(nodeName)
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
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_particle.bmp"), Color.Magenta);
    }

    #endregion

    #region Shape members

    /// <summary>
    /// Create the engine instance
    /// </summary>
    /// <param name="bCreateChildren"></param>
    public override void CreateEngineInstance(bool bCreateChildren)
    {
      this._engineInstance = new EngineInstanceParticleGroup();
      SetEngineInstanceBaseProperties(); // sets the position etc.

      // Call the base function to setup attachments and process the children. The engine instance
      // has to exist at this point.
      base.CreateEngineInstance(bCreateChildren);
    }

    /// <summary>
    /// Return the engine instance casted to EngineInstanceParticleGroup
    /// </summary>
    [BrowsableAttribute(false)]
    public EngineInstanceParticleGroup EnginePGroup {get {return _engineInstance as EngineInstanceParticleGroup;}}

    void UpdateMeshEmitterEntity()
    {
      if (!HasEngineInstance())
        return;

      EngineInstanceEntity _inst = null;
      if (_emitterMeshEntity != null && _emitterMeshEntity.Shape != null)
      {
        IEngineShapeInstance targetShapeInstance = _emitterMeshEntity.Shape._engineInstance;

        // Only IEngineShapeInstance that are also EngineInstanceEntity are accepted
        if (targetShapeInstance == null || targetShapeInstance is EngineInstanceEntity)
        {
          _inst = targetShapeInstance as EngineInstanceEntity;
        }
        else
        {
          _emitterMeshEntity = null;
        }
      }
      EnginePGroup.SetMeshEmitterEntity(_inst);
    }

    /// <summary>
    /// overridden function to set/update all specific properties on the engine instance
    /// </summary>
    public override void SetEngineInstanceBaseProperties()
    {
      if (_engineInstance==null)
        return;
      EnginePGroup.BeginUpdateParams();
      EnginePGroup.SetRandomBaseSeed(_uiRandomBaseSeed);
      EnginePGroup.SetEffectFile(_effectFile); // first set the effect...
      base.SetEngineInstanceBaseProperties(); // ...then position etc.
      EnginePGroup.SetVisibleBitmask((uint)_iVisibleBitmask);
      EnginePGroup.SetAmbientColor(VisionColors.Get(_ambientColor));
      EnginePGroup.SetPaused(_bPaused);
      EnginePGroup.SetHalted(_bHalted);
      EnginePGroup.SetWindSpeed(_windSpeed, _bLocalSpaceWind);
      EnginePGroup.UpdateParticleColors();
      EnginePGroup.SetIntensity(_fIntensity);
      EnginePGroup.SetApplyTimeOfDayLight(_bApplyTimeOfDayLighting);
      UpdateMeshEmitterEntity();

      EnginePGroup.EndUpdateParams();
    }

    public override void PerformPostEngineInstanceCreationSetup(bool bCreateChildren)
    {
      base.PerformPostEngineInstanceCreationSetup(bCreateChildren);
    }

    public override void ReassignAllLinks()
    {
      base.ReassignAllLinks();
      UpdateMeshEmitterEntity();
    }

    public override void OnAddedToScene()
    {
      base.OnAddedToScene();
      AddHint(HintFlags_e.OnlyUniformScale);
    }

    public override void OnRemoveFromScene()
    {
      base.OnRemoveFromScene();
      if (_emitterMeshEntity != null)
        _emitterMeshEntity.OnRemoveFromScene();
    }

    /// <summary>
    /// overridden function to render the shape
    /// </summary>
    /// <param name="view">destination view</param>
    /// <param name="mode">render mode</param>
    public override void RenderShape(VisionViewBase view, ShapeRenderMode mode)
    {
      base.RenderShape(view,mode); // render the bounding box

      // render direction vector
      if (_engineInstance!=null)
        EnginePGroup.RenderShape(view,mode);
    }

    [BrowsableAttribute(false)]
    public override object ToolTip
    {
      get
      {
        string _text = HasEffect ? "Effect : " + EffectFile : "No Effect";
        return _text;
      }
    }

    public override float GetFarClipDistance()
    {
      // TODO: far clip from the effect resource
      return base.GetFarClipDistance();
    }


    public override ShapeBase Clone()
    {
      ParticleGroupShape copy = (ParticleGroupShape)base.Clone();
      copy._hotSpotWindSpeed = null;
      if (_emitterMeshEntity != null)
        copy._emitterMeshEntity = _emitterMeshEntity.Clone() as ShapeReference;
      return copy;
    }

    #endregion

    #region Relevant operations

    /// <summary>
    /// Overridden function to get the relevant operations for this shape type
    /// </summary>
    public override ArrayList RelevantOperations
    {
      get
      {
        ArrayList operations = base.RelevantOperations;
        if (operations==null)
          operations = new ArrayList(4);
        if (Editor != null && HasEffect)
          operations.Add("Edit Effect");
        return operations;
      }
    }

    /// <summary>
    /// Overridden function to perform the relevant operation
    /// </summary>
    /// <param name="name"></param>
    /// <param name="iShapeIndex"></param>
    /// <param name="iShapeCount"></param>
    public override void PerformRelevantOperation(string name, int iShapeIndex, int iShapeCount)
    {
      base.PerformRelevantOperation(name, iShapeIndex, iShapeCount);

      if (name=="Edit Effect" && iShapeIndex==0)
      {
        if (Editor!=null)
        {
          ParticleEditorArgs arg = new ParticleEditorArgs(this,ParticleEditorArgs.Action.Edit);
          Editor(this,arg);
          if (!arg.Success)
            EditorManager.ShowMessageBox("Error loading the particle template into editor:\n" + arg.ErrorString, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
        return;
      }
    }


    #endregion

    #region Tracing

    /// <summary>
    /// Overridden trace function
    /// </summary>
    /// <param name="mode"></param>
    /// <param name="rayStart">ray start position</param>
    /// <param name="rayEnd">ray end position</param>
    /// <param name="result">trace result</param>
    public override void OnTraceShape(ShapeTraceMode_e mode, Vector3F rayStart,Vector3F rayEnd, ref ShapeTraceResult result)
    {
      if (mode!=ShapeTraceMode_e.ShapePicking)
        return;
      if (ConversionUtils.TraceOrientedBoundingBox(LocalBoundingBox,Position,RotationMatrix, rayStart,rayEnd, ref result))
        result.hitShape = this;
    }

    #endregion

    #region Linking

    /// <summary>
    /// Create all link sources and targets provided by this shape
    /// </summary>
    public override void CreateAllLinks()
    {
      base.CreateAllLinks ();
      LinkTargets.Add(new LinkTargetConstraint(this, "ConstraintTarget_1")); // add link target that allows to assign the constraint to particle groups
    }

    /// <summary>
    /// Check whether src can be linked to target. Either src or target has this shape as owner
    /// </summary>
    /// <param name="src">the link source</param>
    /// <param name="target">the link target</param>
    /// <returns>true, if the two can be linked</returns>
    public override bool CanLink(ShapeLink src, ShapeLink target)
    {
      if (base.CanLink(src,target))
        return true;
      if (target.OwnerShape==this)
        if (src is LinkSourceConstraint) return true;
      return false;
    }


    /// <summary>
    /// Perform the actual linking. Either src or target has this shape as owner
    /// </summary>
    /// <param name="src">the link source</param>
    /// <param name="target">the link target</param>
    public override void OnLink(ShapeLink src, ShapeLink target)
    {
      base.OnLink (src, target); // adds to collections

      // perform the linking on the engine objects
      if (target.OwnerShape==this)
      {
        ConstraintShape constraint = src.OwnerShape as ConstraintShape;
        if (constraint != null && HasEngineInstance() && constraint.HasEngineInstance()) // at least the engine instance of the constraint can be null
          EnginePGroup.AddConstraint(constraint._engineInstance as EngineInstanceConstraint);
      }
    }

    /// <summary>
    /// unlinks a target from a source. Either src or target has this shape as owner
    /// </summary>
    /// <param name="src">the link source</param>
    /// <param name="target">the link target</param>
    public override void OnUnlink(ShapeLink src, ShapeLink target)
    {
      base.OnUnlink (src, target);
      // perform the unlinking on the engine objects
      if (target.OwnerShape==this)
      {
        ConstraintShape constraint = src.OwnerShape as ConstraintShape;
        if (constraint != null && HasEngineInstance() && constraint.HasEngineInstance()) // at least the engine instance of the constraint can be null
          EnginePGroup.RemoveConstraint(constraint._engineInstance as EngineInstanceConstraint);
      }
    }

    #endregion

    #region Particle Group Properties

    string _effectFile = "";
    //string _effectKey = null;
    FlagsInt32_e _iVisibleBitmask = FlagsInt32_e.All;
    Color _ambientColor = Color.Black;
    bool _bPaused = false;
    bool _bHalted = false;
    private Vector3F _windSpeed = Vector3F.Zero;
    bool _bLocalSpaceWind = false;
    float _fIntensity = 1.0f;
    bool _bApplyTimeOfDayLighting = false;
    uint _uiRandomBaseSeed = 0;
    ShapeReference _emitterMeshEntity = null;


    /// <summary>
    /// get or set particle effect XML file
    /// </summary>
    [PrefabResolveFilename]
    [SortedCategory(CAT_PROPERTIES, CATORDER_PROPERTIES),
    PropertyOrder(1),
    EditorAttribute(typeof(FilenameEditor), typeof(UITypeEditor)), 
    FileDialogFilter(new string[] {".xml"})]
    [ThumbnailFormat(new string[] { @"{0}.Thumbnail.bmp", @"{0}.Thumbnail.jpg" })]
    [DependentProperties(new string[] { "AutoRenaming" })]
    public string EffectFile
    {
      get {return _effectFile;}
      set 
      {
        if (_effectFile==value) return;
        _effectFile = value;

        // Set name automatically unless the shape has been renamed manually
        if (AutoRenaming && EditorManager.Scene != null)
        {
          string name = Path.GetFileNameWithoutExtension(_effectFile);
          ShapeName = String.IsNullOrEmpty(name) ? "ParticleGroup" : name;
        }

        //using following function to reattach components
        //to avoid crash when changing the engine instance when at least one component is attached [#2784]
        SetEngineInstanceBaseProperties();
        ReAttachComponents();
        
        if (Editor!=null)
          Editor(this, new ParticleEditorArgs(this, ParticleEditorArgs.Action.UpdateUI));
      }
    }

    /// <summary>
    /// Indicates whether this instance has an effect
    /// </summary>
    [Browsable(false)]
    public bool HasEffect
    {
      get { return _effectFile != null && _effectFile != ""; }
    }


    /// <summary>
    /// Effect visibility bitmask that determines the visible status in the rendering contexts
    /// </summary>
    [SortedCategory(CAT_PROPERTIES, CATORDER_PROPERTIES), PropertyOrder(2)]
    [Description("Effect visibility bitmask that determines the visible status in the rendering contexts"), DefaultValue(FlagsInt32_e.All)]
    public FlagsInt32_e VisibleBitmask
    {
      get {return _iVisibleBitmask;}
      set
      {
        _iVisibleBitmask = value;
        if (HasEngineInstance())
          EnginePGroup.SetVisibleBitmask((uint)_iVisibleBitmask);
      }
    }

    /*
    /// <summary>
    /// get or set the key string of this instance. The plugin API provides a function to search for a key
    /// </summary>
    [SortedCategory(CAT_PROPERTIES, CATORDER_PROPERTIES),PropertyOrder(3)]
    [Description("Arbitrary key to identify this instance in the engine")]
    //[TypeConverter(typeof(SceneObjectKeyConverter)), SceneObjectKeyType(IEngineManager.ObjectKeyType_e.Path)]
    public string EffectKey
    {
      get {return _effectKey;}
      set 
      {
        if (_effectKey==value) return;
        _effectKey = value;
        if (HasEngineInstance())
          EnginePGroup.SetEffectKey(_effectKey);
      }
    }
    */

    /// <summary>
    /// Defines the individual effect ambient lighting color
    /// </summary>
    [SortedCategory(CAT_PROPERTIES, CATORDER_PROPERTIES), PropertyOrder(4)]
    [Description("Defines the individual effect instance ambient color. The ambient color is added to the particle's colors")]
    [EditorAttribute(typeof(StandardColorDropDownEditor), typeof(UITypeEditor))]
    [PropertyLiveUpdate("AmbientColorLiveUpdate"), DefaultValue(typeof(Color), "0,0,0")]
    public Color AmbientColor
    {
      get { return _ambientColor; }
      set
      {
        _ambientColor = value;
        if (HasEngineInstance())
          EnginePGroup.SetAmbientColor(VisionColors.Get(_ambientColor));
      }
    }


    /// <summary>
    /// Proxy property used for color live update (See PropertyLiveUpdate attribute in LightColor property)
    /// </summary>
    [Browsable(false)]
    public object AmbientColorLiveUpdate
    {
      set
      {
        Color col = _ambientColor;
        if (value is Color)
          col = (Color)value;
        if (HasEngineInstance())
          EnginePGroup.SetAmbientColor(VisionColors.Get(col));
        EditorManager.ActiveView.UpdateView(false);
      }
    }


    /// <summary>
    /// Gets or sets the paused status
    /// </summary>
    [Description("Defines the paused status of this instance. Particles are still moving but the emitter does not spawn new ones.")]
    [SortedCategory(CAT_PROPERTIES, CATORDER_PROPERTIES), PropertyOrder(5), DefaultValue(false)]
    public bool Paused
    {
      get { return _bPaused; }
      set
      {
        _bPaused = value;
        if (HasEngineInstance())
          EnginePGroup.SetPaused(_bPaused);
      }
    }

    /// <summary>
    /// Gets or sets the paused status
    /// </summary>
    [Description("Defines the halted status of this instance. If enabled, the full simulation is frozen, including moving particles. Therefore it does not cost any performance.")]
    [SortedCategory(CAT_PROPERTIES, CATORDER_PROPERTIES), PropertyOrder(6), DefaultValue(false)]
    public bool Halted
    {
      get { return _bHalted; }
      set
      {
        _bHalted = value;
        if (HasEngineInstance())
          EnginePGroup.SetHalted(_bHalted);
      }
    }


    [Description("If enabled, the Time-of-Day Light is added to the Particle Color. The used particle-effect needs to have the 'Apply Brightness' state checked.")]
    [SortedCategory(CAT_PROPERTIES, CATORDER_PROPERTIES), PropertyOrder(7), DefaultValue(false)]
    public bool ApplyTimeOfDayLighting
    {
      get
      {
        return _bApplyTimeOfDayLighting;
      }
      set
      {
        _bApplyTimeOfDayLighting = value;
        if (HasEngineInstance())
          EnginePGroup.SetApplyTimeOfDayLight(_bApplyTimeOfDayLighting);
      }
    }

    /// <summary>
    /// Gets or sets the custom random number generator seed
    /// </summary>
    [Description("Sets the random number generator base seed for this emitter. Set to 0 (default) for non-synchronized random behavior. Set to non-zero values for deterministic behavior across loads of the scene.")]
    [SortedCategory(CAT_PROPERTIES, CATORDER_PROPERTIES), PropertyOrder(8)]
    public uint RandomBaseSeed
    {
      get { return _uiRandomBaseSeed; }
      set
      {
        _uiRandomBaseSeed = value;
        if (HasEngineInstance())
          EnginePGroup.SetRandomBaseSeed(_uiRandomBaseSeed);
      }
    }

    [PrefabRelevant(false)]
    [Description("Pick a shape reference to an entity shape that should be used as an animated mesh emitter. Used for all particle layers of emitter type 'mesh'")]
    [SortedCategory(CAT_PROPERTIES, CATORDER_PROPERTIES), PropertyOrder(30)]
    [EditorAttribute(typeof(ShapeReferencePicker), typeof(UITypeEditor))]
    public ShapeReference EmitterMeshEntity
    {
      get
      {
        return _emitterMeshEntity;
      }
      set
      {
        if (_emitterMeshEntity != null)
          _emitterMeshEntity.OnRemoveFromScene();
        _emitterMeshEntity = value;
        UpdateMeshEmitterEntity();
      }
    }

    [Description("If enabled, wind is applied in local space of the effect, i.e. it rotates with it. Otherwise it is added in global space like gravity.")]
    [PrefabRelevant(false)]
    [SortedCategory(CAT_WIND, CATORDER_WIND), PropertyOrder(0),DefaultValue(false)]
    public bool WindInLocalSpace
    {
      get
      {
        return _bLocalSpaceWind;
      }
      set
      {
        _bLocalSpaceWind = value;
        if (HasEngineInstance())
          EnginePGroup.SetWindSpeed(_windSpeed, _bLocalSpaceWind);
      }
    }

    [Browsable(false)]
    [PrefabRelevant(true)]
    public Vector3F WindSpeed
    {
      get { return _windSpeed; }
      set
      {
        _windSpeed = value;
        if (HasEngineInstance())
          EnginePGroup.SetWindSpeed(_windSpeed, _bLocalSpaceWind);
        //if (_hotSpotWindSpeed != null && !_hotSpotWindSpeed.IsDragging)
        //  _hotSpotWindSpeed.CurrentPosition = _windSpeed;
      }
    }


    [Description("Sets the x-component of the individual per instance wind speed")]
    [PrefabRelevant(false)]
    [SortedCategory(CAT_WIND, CATORDER_WIND), PropertyOrder(1), DefaultValue(0f)]
    public float WindSpeedX
    {
      get
      {
        return _windSpeed.X;
      }
      set
      {
        WindSpeed = new Vector3F(value, _windSpeed.Y, _windSpeed.Z);
      }
    }

    [Description("Sets the y-component of the individual per instance wind speed")]
    [PrefabRelevant(false)]
    [SortedCategory(CAT_WIND, CATORDER_WIND), PropertyOrder(2), DefaultValue(0f)]
    public float WindSpeedY
    {
      get
      {
        return _windSpeed.Y;
      }
      set
      {
        WindSpeed = new Vector3F(_windSpeed.X, value, _windSpeed.Z);
      }
    }

    [Description("Sets the z-component of the individual per instance wind speed")]
    [PrefabRelevant(false)]
    [SortedCategory(CAT_WIND, CATORDER_WIND), PropertyOrder(3), DefaultValue(0f)]
    public float WindSpeedZ
    {
      get
      {
        return _windSpeed.Z;
      }
      set
      {
        WindSpeed = new Vector3F(_windSpeed.X, _windSpeed.Y, value);
      }
    }

    /// <summary>
    /// Individual intensity of the instance
    /// </summary>
    [Browsable(false)]
    [Description("Individual intensity of the instance (relative value between 0 and 1)")]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.0f, 1.0f, 100)]
    public float Intensity
    {
      get
      {
        return _fIntensity;
      }
      set
      {
        _fIntensity = value;
        if (_fIntensity < 0.0f) _fIntensity = 0.0f;
          else if (_fIntensity > 1.0f) _fIntensity = 1.0f;
        if (HasEngineInstance())
          EnginePGroup.SetIntensity(_fIntensity);
      }
    }

    #endregion

    #region Hotspots

    HotSpot3D _hotSpotWindSpeed = null;

    public override void OnSelected()
    {
      base.OnSelected();

      Debug.Assert(_hotSpotWindSpeed == null);
      _hotSpotWindSpeed = new HotSpot3D(this, 16.0f);
      _hotSpotWindSpeed.ToolTipText = "Wind speed vector";
      _hotSpotWindSpeed.StartPosition = WindSpeed;
      EditorManager.ActiveView.HotSpots.Add(_hotSpotWindSpeed);
    }

    public override void OnUnSelected()
    {
      base.OnUnSelected();

      Debug.Assert(_hotSpotWindSpeed != null);
      EditorManager.ActiveView.HotSpots.Remove(_hotSpotWindSpeed);
      _hotSpotWindSpeed = null;
    }


    public override void OnHotSpotRender(HotSpotBase hotSpot, VisionViewBase view, ShapeRenderMode mode)
    {
      base.OnHotSpotRender(hotSpot, view, mode);
      if (hotSpot == _hotSpotWindSpeed)
      {
        view.RenderArrow(this.RenderPosition, _hotSpotWindSpeed.CurrentPosition + RenderPosition, VisionColors.White, 1.0f, 4.0f);
      }
    }

    public override void OnHotSpotEvaluatePosition(HotSpotBase hotSpot)
    {
      base.OnHotSpotEvaluatePosition(hotSpot);
      if (hotSpot == _hotSpotWindSpeed)
      {
        if (!_hotSpotWindSpeed.IsDragging)
        {
          if (WindInLocalSpace)
            _hotSpotWindSpeed.StartPosition = RotationMatrix * _windSpeed;
          else
          _hotSpotWindSpeed.StartPosition = _windSpeed;
      }
    }
    }

    public override void OnHotSpotDrag(HotSpotBase hotSpot, VisionViewBase view, float fDeltaX, float fDeltaY)
    {
      base.OnHotSpotDrag(hotSpot, view, fDeltaX, fDeltaY);
      if (hotSpot == _hotSpotWindSpeed)
      {
        // set on engine instance while dragging to see the effect
        if (HasEngineInstance())
          EnginePGroup.SetWindSpeed(_hotSpotWindSpeed.CurrentPosition, _bLocalSpaceWind);
      }
    }

    public override void OnHotSpotDragEnd(HotSpotBase hotSpot, VisionViewBase view)
    {
      base.OnHotSpotDragEnd(hotSpot, view);

      if (hotSpot == _hotSpotWindSpeed)
      {
        Vector3F vNewSpeed = _hotSpotWindSpeed.CurrentPosition;
        Vector3F vOldSpeed = _hotSpotWindSpeed.StartPosition;
        if (WindInLocalSpace)
        {
          Matrix3F rot = Matrix3F.Transpose(RotationMatrix);
          vNewSpeed = Matrix3F.Transform(rot, vNewSpeed);
          vOldSpeed = Matrix3F.Transform(rot, vOldSpeed);
        }
        WindSpeed = vOldSpeed; // set old value for the action
        EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this, "WindSpeed", vNewSpeed)); // send an action which sets the property from old value to new one
      }
    }



    #endregion

    #region ISerializable

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected ParticleGroupShape(SerializationInfo info, StreamingContext context) : base(info, context)
    {
      _effectFile = info.GetString( "_effectFile" );
      if (SerializationHelper.HasElement(info,"_iVisibleBitmask"))
        _iVisibleBitmask = (FlagsInt32_e)info.GetValue("_iVisibleBitmask", typeof(FlagsInt32_e));
      if (SerializationHelper.HasElement(info,"_effectKey"))
        _objectKey = info.GetString("_effectKey"); // assign to new object key
      if (SerializationHelper.HasElement(info, "_ambientColor"))
        _ambientColor = (Color)info.GetValue("_ambientColor", typeof(Color));
      if (SerializationHelper.HasElement(info, "_bPaused"))
        _bPaused = info.GetBoolean("_bPaused");
      if (SerializationHelper.HasElement(info, "_bHalted"))
        _bHalted = info.GetBoolean("_bHalted");
      if (SerializationHelper.HasElement(info, "_bApplyTimeOfDayLighting"))
        _bApplyTimeOfDayLighting = info.GetBoolean("_bApplyTimeOfDayLighting");
      if (SerializationHelper.HasElement(info, "_uiRandomBaseSeed"))
        _uiRandomBaseSeed = (uint)info.GetValue("_uiRandomBaseSeed", typeof(uint));
      if (SerializationHelper.HasElement(info, "_windSpeed"))
        _windSpeed = (Vector3F)info.GetValue("_windSpeed", typeof(Vector3F));
      if (SerializationHelper.HasElement(info, "_bLocalSpaceWind"))
        _bLocalSpaceWind = info.GetBoolean("_bLocalSpaceWind");
      if (SerializationHelper.HasElement(info, "_fIntensity"))
        _fIntensity = info.GetSingle("_fIntensity");
      if (SerializationHelper.HasElement(info, "_emitterMeshEntity"))
        _emitterMeshEntity = (ShapeReference)info.GetValue("_emitterMeshEntity", typeof(ShapeReference));
      if (_emitterMeshEntity != null)
        _emitterMeshEntity.Owner = this; // old files
    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context) 
    {
      base.GetObjectData(info,context);

      info.AddValue("_effectFile", _effectFile);
      info.AddValue("_iVisibleBitmask",_iVisibleBitmask);
      //info.AddValue("_effectKey",_effectKey);
      info.AddValue("_ambientColor", _ambientColor);
      info.AddValue("_bPaused", _bPaused);
      info.AddValue("_bHalted", _bHalted);
      info.AddValue("_bApplyTimeOfDayLighting", _bApplyTimeOfDayLighting);
      info.AddValue("_windSpeed", _windSpeed);
      info.AddValue("_bLocalSpaceWind", _bLocalSpaceWind);
      info.AddValue("_fIntensity", _fIntensity);
      info.AddValue("_uiRandomBaseSeed", _uiRandomBaseSeed);
      info.AddValue("_emitterMeshEntity", _emitterMeshEntity);
    }

    #endregion

    #region Particle editor relevant

    /// <summary>
    /// Delegate declaration
    /// </summary>
    public delegate void EditorEventHandler(object sender, ParticleEditorArgs e);

    /// <summary>
    /// This event gets triggered if something happed that might or should affect the particle editor
    /// which listens to this event
    /// </summary>
    static public event EditorEventHandler Editor;


    static ParticlePreviewType_e _previewType = ParticlePreviewType_e.None;

    /// <summary>
    /// This global property should be set by the particle editor so that the engine instance can render some preview properties
    /// </summary>
    [PrefabRelevant(false)]
    public static ParticlePreviewType_e PreviewType
    {
      get {return _previewType;}
      set
      {
        if (_previewType==value) return;
        _previewType = value;
        EngineInstanceParticleGroup.SetPreviewType(value);
        EditorManager.ActiveView.UpdateView(false);
      }
    }

    /// <summary>
    /// Static instance of user-installable string editor. This string editor is used to edit the description of the layer
    /// </summary>
    public static IStringEditor LayerDescriptionEditor = null;

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

    #region internal setter

    public void SetRemoveWhenFinished(bool value)
    {
      if (HasEngineInstance())
        EnginePGroup.SetRemoveWhenFinished(value);
    }

    public void Restart()
    {
      if (HasEngineInstance())
        EnginePGroup.Restart();
    }

    #endregion

    #region IShapeReferenceOwner Members

    public void OnTargetReferenceRemoved(ShapeReference shaperef)
    {
      if (!object.ReferenceEquals(shaperef, _emitterMeshEntity))
        return;
      shaperef.CachedShape = null; // make sure the shape is not set anymore
      UpdateMeshEmitterEntity();
    }

    public void OnTargetReferenceReadded(ShapeReference shaperef)
    {
      if (!object.ReferenceEquals(shaperef, _emitterMeshEntity))
        return;
      UpdateMeshEmitterEntity();
    }

    #endregion
  }

  #region Particle System Creator Plugin

  class ParticleSystemCreator : CSharpFramework.IShapeCreatorPlugin
  {
    public ParticleSystemCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_particle.bmp"), Color.Magenta);
      CategoryIconIndex = IconIndex;
    }

    public override string GetPluginName()
    {
      return "Particle System";
    }

    public override string GetPluginCategory()
    {
      return "Effects";
    }

    /// <summary>
    /// Returns a short description text
    /// </summary>
    /// <returns></returns>
    public override string GetPluginDescription()
    {
      return "Adds a particle emitter instance to the scene.\n"+
        "The particle effect itself can be edited in the 'Particle Editor Layout'.\n"+
        "Particle emitter instances can be affected by constraints.";
    }

    public override ShapeBase CreateShapeInstance()
    {
      ParticleGroupShape pgroup = new ParticleGroupShape("ParticleGroup");
      //pgroup.Pitch = -90.0f; // default orientation
      pgroup.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
      return pgroup;
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
