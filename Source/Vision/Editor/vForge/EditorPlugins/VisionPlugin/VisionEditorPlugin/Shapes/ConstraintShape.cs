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
using CSharpFramework.UndoRedo;
using System.Drawing;
using System.Collections;
using CSharpFramework.Actions;
using CSharpFramework.Serialization;
using CSharpFramework.View;
using VisionManaged;
using CSharpFramework.Scene;
using CSharpFramework.DynamicProperties;

namespace VisionEditorPlugin.Shapes
{

  #region Link

  /// <summary>
  /// Link source class for a constraint
  /// </summary>
  [Serializable]
  public class LinkSourceConstraint : LinkSource
  {
    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="ownerShape"></param>
    /// <param name="primaryStringId"></param>
    public LinkSourceConstraint(Shape3D ownerShape, string primaryStringId) : base(ownerShape,"affects",primaryStringId)
    {
    }

    /// <summary>
    /// Default constructor used by the factory class
    /// </summary>
    /// <param name="ownerShape"></param>
    /// <param name="linkName"></param>
    /// <param name="primaryStringId"></param>
    public LinkSourceConstraint(Shape3D ownerShape, string linkName, string primaryStringId)
      : base(ownerShape, linkName, primaryStringId)
    {
    }


    #region ISerializable Members

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected LinkSourceConstraint(SerializationInfo info, StreamingContext context) : base(info, context)
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
    /// Get the icon filename
    /// </summary>
    public override string DefaultHotspotIconFilename
    {
      get {return @"Bitmaps\Shapes\cube_molecule.png";} // constraint  icon
    }

    #endregion
  }

  #endregion

	/// <summary>
	/// The constraint shape class
	/// </summary>
	[Serializable, ShapeTypeNiceName("Constraint")]
  public class ConstraintShape : ShapeObject3D
	{
    #region Nested Classes: Constraint specific properties
    
    /// <summary>
    /// ConstraintPropertiesBase : Base class of specific constraint properties. Allows to set the properties of the parent,
    /// thus it needs the owner parent in the constructor
    /// </summary>
    public class ConstraintPropertiesBase
    {
      /// <summary>
      /// Constructor
      /// </summary>
      /// <param name="_owner"></param>
      public ConstraintPropertiesBase(ConstraintShape _owner) {Owner=_owner;}

      /// <summary>
      /// Owner constraint
      /// </summary>
      protected ConstraintShape Owner;
    }

    /// <summary>
    /// plane specific constraint properties
    /// </summary>
    public class ConstraintProperties_GroundPlane : ConstraintPropertiesBase
    {
      /// <summary>
      /// Constructor
      /// </summary>
      /// <param name="_owner"></param>
      public ConstraintProperties_GroundPlane(ConstraintShape _owner) : base(_owner) { }
    }

    /// <summary>
    /// plane specific constraint properties
    /// </summary>
    public class ConstraintProperties_Plane : ConstraintPropertiesBase
    {
      /// <summary>
      /// Constructor
      /// </summary>
      /// <param name="_owner"></param>
      public ConstraintProperties_Plane(ConstraintShape _owner) : base(_owner) { }

           /// <summary>
      /// plane size property
      /// </summary>
      [PropertyOrder(1)]
      [Description("If true, the plane is considered to be infinitely large. This is slightly faster.")]
      public bool InfiniteExtent { get { return Owner.InfiniteExtent; } set { Owner.InfiniteExtent = value; } }

      /// <summary>
      /// plane size property
      /// </summary>
      [PropertyOrder(2)]
      [Description("If InfiniteExtent=false, this defines the size of the plane in one direction")]
      public float PlaneSizeX { get { return Owner.BoxSizeX; } set { Owner.BoxSizeX = value; } }


      /// <summary>
      /// plane size property
      /// </summary>
      [PropertyOrder(3)]
      [Description("If InfiniteExtent=false, this defines the size of the plane in one direction")]
      public float PlaneSizeY { get { return Owner.BoxSizeY; } set { Owner.BoxSizeY = value; } }


      /// <summary>
      /// plane size property
      /// </summary>
      [PropertyOrder(4)]
      [Description("If InfiniteExtent=false, this defines the size of the margin of the plane that still influences particles")]
      public float PlaneThickness { get { return Owner.BoxSizeZ; } set { Owner.BoxSizeZ = value; } }

      /// <summary>
      /// Perturbation noise for reflection
      /// </summary>
      [PropertyOrder(5)]
      [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.0f, 1.0f, 100)]
      [Description("If >0, this value defines the perturbation noise for reflecting particles.")]
      public float ReflectionNoise { get { return Owner.ReflectionNoise; } set { Owner.ReflectionNoise = value; } }

    }

    /// <summary>
    /// sphere specific constraint properties (also used for cylinders)
    /// </summary>
    public class ConstraintProperties_Sphere : ConstraintPropertiesBase
    {
      /// <summary>
      /// Constructor
      /// </summary>
      /// <param name="_owner"></param>
      public ConstraintProperties_Sphere(ConstraintShape _owner) : base (_owner) {}

      /// <summary>
      /// Radius property
      /// </summary>
      [PropertyOrder(1)]
      public float Radius {get {return Owner.Radius;} set {Owner.Radius=value;}}

      /// <summary>
      /// Inside property
      /// </summary>
      [PropertyOrder(2)]
      public bool Inside {get {return Owner.Inside;} set {Owner.Inside=value;}}
    }

    /// <summary>
    /// point specific constraint properties (also used for cylinders)
    /// </summary>
    public class ConstraintProperties_Point : ConstraintPropertiesBase
    {
      /// <summary>
      /// Constructor
      /// </summary>
      /// <param name="_owner"></param>
      public ConstraintProperties_Point(ConstraintShape _owner) : base (_owner) {}

      /// <summary>
      /// Radius property
      /// </summary>
      [PropertyOrder(1)]
      public float SearchRadius {get {return Owner.Radius;} set {Owner.Radius=value;}}
    }

    /// <summary>
    /// box specific constraint properties
    /// </summary>
    public class ConstraintProperties_Box : ConstraintPropertiesBase
    {
      /// <summary>
      /// Constructor
      /// </summary>
      /// <param name="_owner"></param>
      public ConstraintProperties_Box(ConstraintShape _owner) : base (_owner) {}

      /// <summary>
      /// box size property
      /// </summary>
      [BrowsableAttribute(false)]
      public Vector3F BoxSize {get {return Owner.BoxSize;} set {BoxSize=value;}}

      /// <summary>
      /// box size property
      /// </summary>
      [PropertyOrder(1)]
      public float SizeX {get {return Owner.BoxSizeX;} set {Owner.BoxSizeX=value;}}

      /// <summary>
      /// box size property
      /// </summary>
      [PropertyOrder(2)]
      public float SizeY {get {return Owner.BoxSizeY;} set {Owner.BoxSizeY=value;}}

      /// <summary>
      /// box size property
      /// </summary>
      [PropertyOrder(3)]
      public float SizeZ {get {return Owner.BoxSizeZ;} set {Owner.BoxSizeZ=value;}}


      /// <summary>
      /// inside flag
      /// </summary>
      [PropertyOrder(4)]
      public bool Inside {get {return Owner.Inside;} set {Owner.Inside=value;}}
    }

    /// <summary>
    /// fan specific constraint properties
    /// </summary>
    public class ConstraintProperties_Fan : ConstraintPropertiesBase
    {
      /// <summary>
      /// Constructor
      /// </summary>
      /// <param name="_owner"></param>
      public ConstraintProperties_Fan(ConstraintShape _owner) : base (_owner) {}


      /// <summary>
      /// intensity property
      /// </summary>
      [PropertyOrder(1)]
      public float Intensity {get {return Owner.Intensity;} set {Owner.Intensity=value;}}

      /// <summary>
      /// cone angle property
      /// </summary>
      [PropertyOrder(2)]
      public float ConeAngle {get {return Owner.ConeAngle;} set {Owner.ConeAngle=value;}}

      /// <summary>
      /// cone angle property
      /// </summary>
      [PropertyOrder(3)]
      public float Randomness {get {return Owner.Randomness;} set {Owner.Randomness=value;}}
    }

    /// <summary>
    /// cyclone specific constraint properties
    /// </summary>
    public class ConstraintProperties_Cyclone : ConstraintPropertiesBase
    {
      /// <summary>
      /// Constructor
      /// </summary>
      /// <param name="_owner"></param>
      public ConstraintProperties_Cyclone(ConstraintShape _owner) : base (_owner) {}


      /// <summary>
      /// Length property
      /// </summary>
      [PropertyOrder(1)]
      public float Length {get {return Owner.Length;} set {Owner.Length=value;}}

      /// <summary>
      /// Radius property
      /// </summary>
      [PropertyOrder(2)]
      public float Radius {get {return Owner.Radius;} set {Owner.Radius=value;}}

      /// <summary>
      /// Intensity property
      /// </summary>
      [PropertyOrder(3)]
      public float Intensity {get {return Owner.Intensity;} set {Owner.Intensity=value;}}
    }

    /// <summary>
    /// sphere specific constraint properties (also used for cylinders)
    /// </summary>
    public class ConstraintProperties_GravityPoint : ConstraintPropertiesBase
    {
      /// <summary>
      /// Constructor
      /// </summary>
      /// <param name="_owner"></param>
      public ConstraintProperties_GravityPoint(ConstraintShape _owner) : base (_owner) {}

      /// <summary>
      /// Radius property
      /// </summary>
      [PropertyOrder(1)]
      public float Radius {get {return Owner.Radius;} set {Owner.Radius=value;}}

      /// <summary>
      /// Inside property
      /// </summary>
      [PropertyOrder(2)]
      public float Intensity {get {return Owner.Intensity;} set {Owner.Intensity=value;}}
    }

    /// <summary>
    /// sphere specific constraint properties (also used for cylinders)
    /// </summary>
    public class ConstraintProperties_CameraBox : ConstraintPropertiesBase
    {
      /// <summary>
      /// Constructor
      /// </summary>
      /// <param name="_owner"></param>
      public ConstraintProperties_CameraBox(ConstraintShape _owner) : base(_owner) { }

      /// <summary>
      /// box size property
      /// </summary>
      [Description("The size of the box around the camera")]
      [PropertyOrder(1)]
      public float SizeX { get { return Owner.BoxSizeX; } set { Owner.BoxSizeX = value; } }

      /// <summary>
      /// box size property
      /// </summary>
      [Description("The size of the box around the camera")]
      [PropertyOrder(2)]
      public float SizeY { get { return Owner.BoxSizeY; } set { Owner.BoxSizeY = value; } }

      /// <summary>
      /// box size property
      /// </summary>
      [Description("The size of the box around the camera")]
      [PropertyOrder(3)]
      public float SizeZ { get { return Owner.BoxSizeZ; } set { Owner.BoxSizeZ = value; } }
    }

    #endregion

    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_TYPE         = "Type";

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_PROPERTIES   = "Properties";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_TYPE       = Shape3D.LAST_CATEGORY_ORDER_ID + 1;

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_PROPERTIES = Shape3D.LAST_CATEGORY_ORDER_ID + 2;

    /// <summary>
    /// Last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_PROPERTIES;

    #endregion

    #region Constructor

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="nodeName">name of the node</param>
    /// <param name="_type">constarint type. Can be changed but is used for some special init sizes</param>
    public ConstraintShape(string nodeName, EngineInstanceConstraint.ConstraintType_e _type) : base(nodeName)
    {
      _fRadius = 50.0f; // spheres etc.
      if (_type==EngineInstanceConstraint.ConstraintType_e.Point)
        _fRadius = 4.0f;
      ConstraintType = _type;
      ScaleForUI();
    }

    /// <summary>
    /// Base constructor using name only (important for prefabs)
    /// </summary>
    /// <param name="node"></param>
    public ConstraintShape(string node)
      : base(node)
    {
      _fRadius = 50.0f; // spheres etc.
      ConstraintType = EngineInstanceConstraint.ConstraintType_e.GroundPlane;
      ScaleForUI();
    }


    void ScaleForUI()
    {
      _fRadius *= EditorManager.Settings.GlobalUnitScaling;
      _vBoxSize *= EditorManager.Settings.GlobalUnitScaling;
      _fLength *= EditorManager.Settings.GlobalUnitScaling;
    }

    #endregion

    #region GUI

    /// <summary>
    /// Overridden function to get the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetIconIndex()
    {
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\Constraint.bmp"), Color.Magenta);
    }

    #endregion

    #region Shape members

    /// <summary>
    /// Create the engine instance
    /// </summary>
    /// <param name="bCreateChildren"></param>
    public override void CreateEngineInstance(bool bCreateChildren)
    {
      this._engineInstance = new EngineInstanceConstraint();
      SetEngineInstanceBaseProperties(); // sets the position etc.
      SetSpecificProperties(); // sets the specific object in the property grid

      // Call the base function to setup attachments and process the children. The engine instance
      // has to exist at this point.
      base.CreateEngineInstance(bCreateChildren);
    }

 
    EngineInstanceConstraint EngineConstraint {get {return _engineInstance as EngineInstanceConstraint;}}

    /// <summary>
    /// overridden function to render the shape
    /// </summary>
    /// <param name="view">destination view</param>
    /// <param name="mode">render mode</param>
    public override void RenderShape(VisionViewBase view, ShapeRenderMode mode)
    {
      base.RenderShape(view, mode);
      if (!HasEngineInstance())
        return;
      if (mode == ShapeRenderMode.Selected)
        EngineConstraint.HighlightShape(view);
      // else render more transparent?
    }


    /// <summary>
    /// overridden function to set/update all specific properties on the engine instance
    /// </summary>
    public override void SetEngineInstanceBaseProperties()
    {
      if (_engineInstance==null) return;
      EngineConstraint.SetType(_eType);
      base.SetEngineInstanceBaseProperties();
      EngineConstraint.SetReflection(_eReflection,_fPersistance);
      EngineConstraint.SetRadius(_fRadius * UniformScaling);
      EngineConstraint.SetLength(_fLength * UniformScaling);
      EngineConstraint.SetInside(_bInside);
      EngineConstraint.SetInfiniteExtent(_bInfiniteExtent);
      EngineConstraint.SetIntensity(_fIntensity * UniformScaling);
      EngineConstraint.SetConeAngle(_fConeAngle);
      EngineConstraint.SetBoxSize(_vBoxSize.X * ScaleX, _vBoxSize.Y * ScaleY, _vBoxSize.Z * ScaleZ);
      EngineConstraint.SetRandomness(_fRandomness);
      EngineConstraint.SetAffectBitmask((uint)_iAffectBitmask);
      EngineConstraint.SetReflectionNoise(_fReflectionNoise);
    }

    public override void OnTransformationChanged()
    {
      base.OnTransformationChanged();

      // scaling might have changed:
      if (HasEngineInstance())
      {
        EngineConstraint.SetRadius(_fRadius * UniformScaling);
        EngineConstraint.SetLength(_fLength * UniformScaling);
        EngineConstraint.SetIntensity(_fIntensity * UniformScaling);
        EngineConstraint.SetBoxSize(_vBoxSize.X * ScaleX, _vBoxSize.Y * ScaleY, _vBoxSize.Z * ScaleZ);
      }
      float fBoxScaleXY = ConstraintType == EngineInstanceConstraint.ConstraintType_e.Plane ? 1.0f : 0.5f;

      if (_hotSpotFanIntensity != null)
        _hotSpotFanIntensity.DisplayScaling = EditorManager.Settings.GlobalUnitScaling * UniformScaling;
      if (_hotSpotRadius != null)
        _hotSpotRadius.DisplayScaling = UniformScaling;
      if (_hotSpotLength != null)
        _hotSpotLength.DisplayScaling = UniformScaling;
      if (_hotSpotSizeX != null)
        _hotSpotSizeX.DisplayScaling = fBoxScaleXY * ScaleX;
      if (_hotSpotSizeY != null)
        _hotSpotSizeY.DisplayScaling = fBoxScaleXY * ScaleY;
      if (_hotSpotSizeZ != null)
        _hotSpotSizeZ.DisplayScaling = 0.5f * ScaleZ;

    }


    public override bool GetLocalBoundingBox(ref BoundingBox bbox)
    {
      float fGS = EditorManager.Settings.GlobalUnitScaling;
      switch (_eType)
      {
        case EngineInstanceConstraint.ConstraintType_e.AABox:
          bbox = new BoundingBox(-_vBoxSize * 0.5f, _vBoxSize * 0.5f); return true;
        case EngineInstanceConstraint.ConstraintType_e.Fan:
          return base.GetLocalBoundingBox(ref bbox);
        case EngineInstanceConstraint.ConstraintType_e.Plane:
          if (InfiniteExtent)
            return false;
          bbox = new BoundingBox(-_vBoxSize.X, -_vBoxSize.Y, -1.0f * fGS, _vBoxSize.X, _vBoxSize.Y, 1.0f * fGS); 
          return true;
        case EngineInstanceConstraint.ConstraintType_e.Point:
        case EngineInstanceConstraint.ConstraintType_e.Sphere:
        case EngineInstanceConstraint.ConstraintType_e.GravityPoint:
          bbox = new BoundingBox(-Radius, -Radius, -Radius, Radius, Radius, Radius);
          return true;

        case EngineInstanceConstraint.ConstraintType_e.Cyclone:
          bbox = new BoundingBox(0.0f, -Radius, -Radius, Length, Radius, Radius);
          return true;

        case EngineInstanceConstraint.ConstraintType_e.GroundPlane:
        case EngineInstanceConstraint.ConstraintType_e.XAxis:
        case EngineInstanceConstraint.ConstraintType_e.YAxis:
        case EngineInstanceConstraint.ConstraintType_e.ZAxis:
          return false;
      }
      return false;
    }

    #endregion

    #region Cloning

    /// <summary>
    /// This function must be overridden, since we have to reset the hotspots
    /// </summary>
    /// <returns></returns>
    public override ShapeBase Clone()
    {
      ConstraintShape copy = base.Clone() as ConstraintShape;

      copy._hotSpotFanAngle = null;
      copy._hotSpotFanIntensity = null;
      copy._hotSpotRadius = null;
      copy._hotSpotLength = null;
      copy._hotSpotSizeX = null;
      copy._hotSpotSizeY = null;
      copy._hotSpotSizeZ = null;
      System.Diagnostics.Debug.Assert(!copy.Selected); // turned off in ShapeBase

      return copy;
    }

    #endregion

    #region Constraint Type


    EngineInstanceConstraint.ConstraintType_e _eType = EngineInstanceConstraint.ConstraintType_e.GroundPlane;

    /// <summary>
    /// Get the constraint type
    /// </summary>
    [SortedCategory(CAT_TYPE, CATORDER_TYPE),PropertyOrder(1)]
    [RefreshProperties(RefreshProperties.All)]
    [Description("Pre-defined constraint shape type / behavior")]
    public EngineInstanceConstraint.ConstraintType_e ConstraintType
    {
      get {return _eType;}
      set 
      {
        if (_eType==value)
          return;
        _eType = value;
        SetSpecificProperties();
       
        SetEngineInstanceBaseProperties(); // sets the position etc.
        ReassignAllLinks(); // ..because the actual engine instances changed
      }
    }


    public override void OnLink(ShapeLink src, ShapeLink target)
    {
      base.OnLink(src, target);

      // additionally notify target...
      if (src.OwnerShape == this)
      {
        target.OwnerShape.OnLink(src, target);
      }
    }


    EngineInstanceConstraint.ConstraintReflectionType_e _eReflection = EngineInstanceConstraint.ConstraintReflectionType_e.Bounce;

    /// <summary>
    /// Get the OnCollide behaviour
    /// </summary>
    [SortedCategory(CAT_TYPE, CATORDER_TYPE),
    PropertyOrder(3)]
    [Description("Defines the particle behavior on collision with this constraint")]
    public EngineInstanceConstraint.ConstraintReflectionType_e OnCollide
    {
      get {return _eReflection;}
      set 
      {
        if (_eReflection==value) return;
        _eReflection = value;
        if (_engineInstance!=null)
          EngineConstraint.SetReflection(_eReflection,_fPersistance);
      }
    }
    float _fPersistance = 0.2f;

    /// <summary>
    /// Get the persistance
    /// </summary>
    [SortedCategory(CAT_TYPE, CATORDER_TYPE),
    PropertyOrder(2)]
    [Description("Defines the reflectivity of particles when they collide with this constraint (only relevant in bounce mode)")]
    public float Persistance
    {
      get {return _fPersistance;}
      set 
      {
        if (_fPersistance==value) return;
        _fPersistance = value;
        if (_engineInstance!=null)
          EngineConstraint.SetReflection(_eReflection,_fPersistance);
      }
    }

    /// <summary>
    /// Get the bitmask for affecting particle layers
    /// </summary>
    [SortedCategory(CAT_TYPE, CATORDER_TYPE),
    PropertyOrder(4)]
    [Description("Get the bitmask for affecting particle layers. This value is logically ANDed with the particle effect's layer bitmask")]
    public FlagsInt32_e AffectBitmask
    {
      get {return _iAffectBitmask;}
      set 
      {
        if (_iAffectBitmask==value) return;
        _iAffectBitmask = value;
        if (_engineInstance!=null)
          EngineConstraint.SetAffectBitmask((uint)_iAffectBitmask);
      }
    }

    /* future use:

    string _constraintClass = null;
    DynamicPropertyCollection _dynamicConstraintProperties = null;


    [SortedCategory(CAT_TYPE, CATORDER_TYPE),
    PropertyOrder(0),
    DependentProperties(new string[] { "ConstraintProperties" }),
    TypeConverter(typeof(ConstraintClassConverter)),  
    Description("The engine class of the constraint"),
    RefreshProperties(System.ComponentModel.RefreshProperties.All) ]
    public string ConstraintClass
    {
      get { return _constraintClass; }
      set 
      { 
        _constraintClass = value;
        DynamicPropertyCollectionType type = NativeConstraintClasses.GetCollectionType(_constraintClass);
        if (type != null)
          _dynamicConstraintProperties = type.CreateMigratedCollection(_dynamicConstraintProperties);
        else
          _dynamicConstraintProperties = null;
      }
    }
     
    */

    #endregion

    #region Special Constraint Properties

    void SetSpecificProperties()
    {
      bool bScale = true;
      bool bOnlyUniformScale = true;
      bool bRotation = true;

      switch (_eType)
      {
        case EngineInstanceConstraint.ConstraintType_e.AABox: _constraintProperties = new ConstraintProperties_Box(this); bOnlyUniformScale = false; bRotation = false; break;
        case EngineInstanceConstraint.ConstraintType_e.Fan: _constraintProperties = new ConstraintProperties_Fan(this);break;
        case EngineInstanceConstraint.ConstraintType_e.GroundPlane: _constraintProperties = new ConstraintProperties_GroundPlane(this); bScale = false; bRotation = false; break;
        case EngineInstanceConstraint.ConstraintType_e.Plane: _constraintProperties = new ConstraintProperties_Plane(this); bOnlyUniformScale = false; break;
        case EngineInstanceConstraint.ConstraintType_e.Point: _constraintProperties = new ConstraintProperties_Point(this); bRotation = false; break;
        case EngineInstanceConstraint.ConstraintType_e.Sphere: _constraintProperties = new ConstraintProperties_Sphere(this); bRotation = false; break;
        case EngineInstanceConstraint.ConstraintType_e.XAxis:
        case EngineInstanceConstraint.ConstraintType_e.YAxis:
        case EngineInstanceConstraint.ConstraintType_e.ZAxis: _constraintProperties = new ConstraintProperties_Sphere(this); bRotation = false; break;
        case EngineInstanceConstraint.ConstraintType_e.Cyclone: _constraintProperties = new ConstraintProperties_Cyclone(this);break;
        case EngineInstanceConstraint.ConstraintType_e.GravityPoint: _constraintProperties = new ConstraintProperties_GravityPoint(this); bRotation = false; break;
        case EngineInstanceConstraint.ConstraintType_e.CameraBox: _constraintProperties = new ConstraintProperties_CameraBox(this); bRotation = false; break;
        default:
          _constraintProperties = null;
          break;
      }

      SetHint(HintFlags_e.NoScale, !bScale);
      SetHint(HintFlags_e.OnlyUniformScale, bOnlyUniformScale);
      SetHint(HintFlags_e.NoRotation, !bRotation);
    }

    ConstraintPropertiesBase _constraintProperties = null;

    /// <summary>
    /// Get the type specific sub-properties
    /// </summary>
    [PrefabRelevant(false)]
    [SortedCategory(CAT_TYPE, CATORDER_TYPE),
    PropertyOrder(4),
    TypeConverter(typeof(UndoableObjectConverter))]
    public ConstraintPropertiesBase ConstraintProperties
    {
      get {return _constraintProperties;}
      set {_constraintProperties = value;}
    }

    // these members are merged from all possible constraint types. The radius for instance is used for sphere and cylinder
    float _fRadius = 50.0f;
    bool _bInside = false;
    float _fIntensity = 100.0f;
    float _fConeAngle = 45.0f;
    Vector3F _vBoxSize = new Vector3F(150,150,50);
    float _fLength = 200.0f;
    float _fRandomness = 0.0f;
    FlagsInt32_e _iAffectBitmask = FlagsInt32_e.All;
    bool _bInfiniteExtent = true;
    float _fReflectionNoise = 0.0f;

    // the according fields are set by the _constraintProperties object and not visible here.

    /// <summary>
    /// Constraint radius
    /// </summary>
    [BrowsableAttribute(false)]
    [PrefabRelevant(true)]
    public float Radius
    {
      get {return _fRadius;}
      set 
      {
        if (_fRadius==value) return;
        _fRadius = value;
        if (_engineInstance!=null)
          EngineConstraint.SetRadius(_fRadius * UniformScaling);
      }
    }

    /// <summary>
    /// Inside flag
    /// </summary>
    [PrefabRelevant(true)]
    [BrowsableAttribute(false)]
    public bool Inside
    {
      get {return _bInside;}
      set 
      {
        if (_bInside==value) return;
        _bInside = value;
        if (_engineInstance!=null)
          EngineConstraint.SetInside(_bInside);
      }
    }

    /// <summary>
    /// Inside flag
    /// </summary>
    [PrefabRelevant(true)]
    [BrowsableAttribute(false)]
    public bool InfiniteExtent
    {
      get { return _bInfiniteExtent; }
      set
      {
        if (_bInfiniteExtent == value) return;
        _bInfiniteExtent = value;
        if (_engineInstance != null)
          EngineConstraint.SetInfiniteExtent(_bInfiniteExtent);
      }
    }

    /// <summary>
    /// Constraint intensity
    /// </summary>
    [PrefabRelevant(true)]
    [BrowsableAttribute(false)]
    public float Intensity
    {
      get {return _fIntensity;}
      set 
      {
        if (_fIntensity==value) return;
        _fIntensity = value;
        if (_engineInstance!=null)
          EngineConstraint.SetIntensity(_fIntensity * UniformScaling);
      }
    }

    /// <summary>
    /// Constraint cone angle
    /// </summary>
    [PrefabRelevant(true)]
    [BrowsableAttribute(false)]
    public float ConeAngle
    {
      get {return _fConeAngle;}
      set 
      {
        if (_fConeAngle==value) return;
        _fConeAngle = value;
        if (_engineInstance!=null)
          EngineConstraint.SetConeAngle(_fConeAngle);
      }
    }

    /// <summary>
    /// Constraint length
    /// </summary>
    [PrefabRelevant(true)]
    [BrowsableAttribute(false)]
    public float Length
    {
      get {return _fLength;}
      set 
      {
        if (_fLength==value) return;
        _fLength = value;
        if (_engineInstance!=null)
          EngineConstraint.SetLength(_fLength * UniformScaling);
      }
    }
    /// <summary>
    /// Constraint length
    /// </summary>
    [PrefabRelevant(true)]
    [BrowsableAttribute(false)]
    public float Randomness
    {
      get {return _fRandomness;}
      set 
      {
        if (_fRandomness==value) return;
        _fRandomness = value;
        if (_fRandomness<0.0f) _fRandomness = 0.0f;
          else if (_fRandomness>1.0f) _fRandomness = 1.0f;
        if (_engineInstance!=null)
          EngineConstraint.SetRandomness(_fRandomness);
      }
    }
    
    /// <summary>
    /// Constraint box dimension
    /// </summary>
    [PrefabRelevant(true)]
    [BrowsableAttribute(false)]
    public Vector3F BoxSize
    {
      get {return _vBoxSize;}
      set 
      {
        _vBoxSize = value.Clone();
        if (_engineInstance!=null)
          EngineConstraint.SetBoxSize(_vBoxSize.X * ScaleX, _vBoxSize.Y * ScaleY, _vBoxSize.Z * ScaleZ);
      }
    }

    /// <summary>
    /// Constraint box size x
    /// </summary>
    [BrowsableAttribute(false)]
    public float BoxSizeX
    {
      get {return _vBoxSize.X;}
      set 
      {
        if (_vBoxSize.X == value) return;
        _vBoxSize.X = value;
        if (_engineInstance!=null)
          EngineConstraint.SetBoxSize(_vBoxSize.X * ScaleX, _vBoxSize.Y * ScaleY, _vBoxSize.Z * ScaleZ);
      }
    }

    /// <summary>
    /// Constraint box size y
    /// </summary>
    [BrowsableAttribute(false)]
    public float BoxSizeY
    {
      get {return _vBoxSize.Y;}
      set 
      {
        if (_vBoxSize.Y == value) return;
        _vBoxSize.Y = value;
        if (_engineInstance!=null)
          EngineConstraint.SetBoxSize(_vBoxSize.X * ScaleX, _vBoxSize.Y * ScaleY, _vBoxSize.Z * ScaleZ);
      }
    }

    /// <summary>
    /// Constraint box size z
    /// </summary>
    [BrowsableAttribute(false)]
    public float BoxSizeZ
    {
      get {return _vBoxSize.Z;}
      set 
      {
        if (_vBoxSize.Z == value) return;
        _vBoxSize.Z = value;
        if (_engineInstance!=null)
          EngineConstraint.SetBoxSize(_vBoxSize.X * ScaleX, _vBoxSize.Y * ScaleY, _vBoxSize.Z * ScaleZ);
      }
    }

    /// <summary>
    /// Perturbation noise for reflection
    /// </summary>
    [BrowsableAttribute(false)]
    [PrefabRelevant(true)]
    public float ReflectionNoise
    {
      get { return _fReflectionNoise; }
      set
      {
        if (_fReflectionNoise == value) return;
        _fReflectionNoise = value;
        if (_fReflectionNoise < 0.0f)
          _fReflectionNoise = 0.0f;
        else if (_fReflectionNoise > 1.0f)
          _fReflectionNoise = 1.0f;
        if (_engineInstance != null)
          EngineConstraint.SetReflectionNoise(_fReflectionNoise);
      }
    }


    #endregion

    #region ISerializable

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected ConstraintShape(SerializationInfo info, StreamingContext context) : base(info, context)
    {
      _bInside = info.GetBoolean( "_bInside" );
      _eReflection = (EngineInstanceConstraint.ConstraintReflectionType_e) info.GetValue( "_eReflection", typeof(EngineInstanceConstraint.ConstraintReflectionType_e) );
      _eType  = (EngineInstanceConstraint.ConstraintType_e) info.GetValue( "_eType" ,typeof(EngineInstanceConstraint.ConstraintType_e) );
      _fConeAngle = info.GetSingle( "_fConeAngle" );
      _fIntensity = info.GetSingle( "_fIntensity" );
      _fPersistance = info.GetSingle( "_fPersistance" );
      _fRadius = info.GetSingle( "_fRadius" );
      if (SerializationHelper.HasElement(info,"_fLength")) _fLength = info.GetSingle( "_fLength" );
      _vBoxSize = (Vector3F) info.GetValue( "_vBoxSize", typeof(Vector3F) );
      if (SerializationHelper.HasElement(info,"_fRandomness")) 
        _fRandomness = info.GetSingle( "_fRandomness" );
      if (SerializationHelper.HasElement(info,"_iAffectBitmask"))
        _iAffectBitmask = (FlagsInt32_e)info.GetValue("_iAffectBitmask", typeof(FlagsInt32_e));
      if (SerializationHelper.HasElement(info, "_bInfiniteExtension"))
        _bInfiniteExtent = info.GetBoolean("_bInfiniteExtension");
      if (SerializationHelper.HasElement(info, "_fReflectionNoise"))
        _fReflectionNoise = info.GetSingle("_fReflectionNoise");
      // backwards compatibility
      if (!SerializationHelper.HasElement(info, "SupportScaling"))
        SetScaling_Internal(1.0f, 1.0f, 1.0f);
      
    }

  /// <summary>
  /// Called when serializing
  /// </summary>
  /// <param name="info"></param>
  /// <param name="context"></param>
  public override void GetObjectData(SerializationInfo info, StreamingContext context) 
  {
    base.GetObjectData(info,context);
    info.AddValue("_bInside", _bInside);
    info.AddValue("_eReflection", _eReflection);
    info.AddValue("_eType", _eType);
    info.AddValue("_fConeAngle", _fConeAngle);
    info.AddValue("_fIntensity", _fIntensity);
    info.AddValue("_fPersistance", _fPersistance);
    info.AddValue("_fRadius", _fRadius);
    info.AddValue("_vBoxSize", _vBoxSize);
    info.AddValue("_fLength", _fLength);
    info.AddValue("_fRandomness", _fRandomness);
    info.AddValue("_iAffectBitmask", _iAffectBitmask);
    info.AddValue("_bInfiniteExtension", _bInfiniteExtent);
    info.AddValue("_fReflectionNoise", _fReflectionNoise);
    info.AddValue("SupportScaling", true);
  }

  #endregion

    #region Hotspot

    HotSpotConeAngle _hotSpotFanAngle;    // a hotspot for the fan cone angle
    HotSpotDistance _hotSpotFanIntensity; // a hotspot for the fan intensity
    HotSpotDistance _hotSpotRadius;       // a hotspot for the sphere, cyclone and axis constraints
    HotSpotDistance _hotSpotLength;       // a hotspot for the cyclone length
    HotSpotDistance _hotSpotSizeX;        // a hotspot for the AABox
    HotSpotDistance _hotSpotSizeY;        // a hotspot for the AABox
    HotSpotDistance _hotSpotSizeZ;        // a hotspot for the AABox

    /// <summary>
    /// Called when the shape is selected
    /// </summary>
    public override void OnSelected()
    {
      base.OnSelected ();
      System.Diagnostics.Debug.Assert(_hotSpotFanAngle==null);
      System.Diagnostics.Debug.Assert(_hotSpotFanIntensity==null);
      System.Diagnostics.Debug.Assert(_hotSpotRadius==null);
      System.Diagnostics.Debug.Assert(_hotSpotLength==null);
      System.Diagnostics.Debug.Assert(_hotSpotSizeX==null);
      System.Diagnostics.Debug.Assert(_hotSpotSizeY==null);
      System.Diagnostics.Debug.Assert(_hotSpotSizeZ==null);

      // create the hotspots:

      // hotspot for fan cone angle
      _hotSpotFanAngle = new HotSpotConeAngle(this,@"textures\Hotspot_A.tga",VisionColors.Red,HotSpotBase.PickType.Square,4.0f);
      _hotSpotFanAngle.Distance = 50.0f * EditorManager.Settings.GlobalUnitScaling;
      _hotSpotFanAngle.StartAngle = this.ConeAngle;
      _hotSpotFanAngle.ToolTipText = "cone angle";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotFanAngle);

      // hotspot for fan intensity
      _hotSpotFanIntensity = new HotSpotDistance(this,@"textures\Hotspot_I.tga",VisionColors.Yellow,HotSpotBase.PickType.Square,4.0f);
      _hotSpotFanIntensity.StartDistance = this.Intensity;
      _hotSpotFanIntensity.Axis = this.XAxis;
      _hotSpotFanIntensity.DisplayScaling = EditorManager.Settings.GlobalUnitScaling * UniformScaling;
      _hotSpotFanIntensity.ToolTipText = "intensity";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotFanIntensity);

      // hotspot for radius
      _hotSpotRadius = new HotSpotDistance(this,@"textures\Hotspot_R.tga",VisionColors.Green,HotSpotBase.PickType.Square,4.0f);
      _hotSpotRadius.StartDistance = this.Radius;
      _hotSpotRadius.Axis = this.ZAxis;
      _hotSpotRadius.DisplayScaling = UniformScaling;
      _hotSpotRadius.ToolTipText = "radius";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotRadius);

      // hotspot for length
      _hotSpotLength = new HotSpotDistance(this,@"textures\Hotspot_L.tga",VisionColors.Blue,HotSpotBase.PickType.Square,4.0f);
      _hotSpotLength.StartDistance = this.Length;
      _hotSpotLength.Axis = this.XAxis;
      _hotSpotLength.DisplayScaling = UniformScaling;
      _hotSpotLength.ToolTipText = "length";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotLength);

      float fBoxScaleXY = ConstraintType == EngineInstanceConstraint.ConstraintType_e.Plane ? 1.0f : 0.5f;
      // hotspot for size X
      _hotSpotSizeX = new HotSpotDistance(this,@"textures\Hotspot_Rect.dds",VisionColors.Yellow,HotSpotBase.PickType.Square,4.0f);
      _hotSpotSizeX.StartDistance = this.BoxSizeX;
      _hotSpotSizeX.DisplayScaling = fBoxScaleXY * ScaleX;
      _hotSpotSizeX.Axis = this.XAxis;
      _hotSpotSizeX.ToolTipText = "size x direction";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotSizeX);

      // hotspot for size Y
      _hotSpotSizeY = new HotSpotDistance(this,@"textures\Hotspot_Rect.dds",VisionColors.Yellow,HotSpotBase.PickType.Square,4.0f);
      _hotSpotSizeY.StartDistance = this.BoxSizeY;
      _hotSpotSizeY.DisplayScaling = fBoxScaleXY * ScaleY;
      _hotSpotSizeY.Axis = this.YAxis;
      _hotSpotSizeY.ToolTipText = "size y direction";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotSizeY);
      
      // hotspot for size Z
      _hotSpotSizeZ = new HotSpotDistance(this,@"textures\Hotspot_Rect.dds",VisionColors.Yellow,HotSpotBase.PickType.Square,4.0f);
      _hotSpotSizeZ.StartDistance = this.BoxSizeZ;
      _hotSpotSizeZ.DisplayScaling = 0.5f * ScaleZ;
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
      SafeRemoveHotSpot(_hotSpotFanAngle);_hotSpotFanAngle=null;
      SafeRemoveHotSpot(_hotSpotFanIntensity);_hotSpotFanIntensity=null;
      SafeRemoveHotSpot(_hotSpotRadius);_hotSpotRadius=null;
      SafeRemoveHotSpot(_hotSpotLength);_hotSpotLength=null;
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
      if (hotSpot==_hotSpotFanAngle)
        _hotSpotFanAngle.StartAngle = this.ConeAngle;
      if (hotSpot==_hotSpotFanIntensity)
        _hotSpotFanIntensity.StartDistance = this.Intensity;
      if (hotSpot==_hotSpotRadius)
        _hotSpotRadius.StartDistance = this.Radius;
      if (hotSpot==_hotSpotLength)
        _hotSpotLength.StartDistance = this.Length;
      if (hotSpot==_hotSpotSizeX)
        _hotSpotSizeX.StartDistance = this.BoxSizeX;
      if (hotSpot==_hotSpotSizeY)
        _hotSpotSizeY.StartDistance = this.BoxSizeY;
      if (hotSpot==_hotSpotSizeZ)
        _hotSpotSizeZ.StartDistance = this.BoxSizeZ;
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
      if (hotSpot==_hotSpotFanAngle)
        this.ConeAngle = _hotSpotFanAngle.CurrentAngle; // update for the view
      if (hotSpot==_hotSpotFanIntensity)
        this.Intensity = _hotSpotFanIntensity.CurrentDistance; 
      if (hotSpot==_hotSpotRadius)
        this.Radius = _hotSpotRadius.CurrentDistance; 
      if (hotSpot==_hotSpotLength)
        this.Length = _hotSpotLength.CurrentDistance; 
      if (hotSpot==_hotSpotSizeX)
        this.BoxSizeX = _hotSpotSizeX.CurrentDistance; 
      if (hotSpot==_hotSpotSizeY)
        this.BoxSizeY = _hotSpotSizeY.CurrentDistance; 
      if (hotSpot==_hotSpotSizeZ)
        this.BoxSizeZ = _hotSpotSizeZ.CurrentDistance; 
    }

    /// <summary>
    /// Called when the user releases the mouse button on a hotspot
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragEnd(HotSpotBase hotSpot, VisionViewBase view)
    {
      if (hotSpot==_hotSpotFanAngle)
      {
        if (_hotSpotFanAngle.HasChanged)
        {
          float fNewAngle = _hotSpotFanAngle.CurrentAngle;
          this.ConeAngle = _hotSpotFanAngle.StartAngle; // set old angle for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"ConeAngle",fNewAngle)); // send an action which sets the property from old value to new one
        }
      }
      if (hotSpot==_hotSpotFanIntensity)
      {
        if (_hotSpotFanIntensity.HasChanged)
        {
          float fNewIntensity = _hotSpotFanIntensity.CurrentDistance;
          this.Intensity = _hotSpotFanIntensity.StartDistance; // set old value for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"Intensity",fNewIntensity)); // send an action which sets the property from old value to new one
        }
      }
      if (hotSpot==_hotSpotRadius)
      {
        if (_hotSpotRadius.HasChanged)
        {
          float fNewRadius = _hotSpotRadius.CurrentDistance;
          this.Radius = _hotSpotRadius.StartDistance; // set old value for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"Radius",fNewRadius)); // send an action which sets the property from old value to new one
        }
      }
      if (hotSpot==_hotSpotLength)
      {
        if (_hotSpotLength.HasChanged)
        {
          float fNewLength = _hotSpotLength.CurrentDistance;
          this.Length = _hotSpotLength.StartDistance; // set old value for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"Length",fNewLength)); // send an action which sets the property from old value to new one
        }
      }
      if (hotSpot==_hotSpotSizeX)
      {
        if (_hotSpotSizeX.HasChanged)
        {
          float fNewSize = _hotSpotSizeX.CurrentDistance;
          this.BoxSizeX = _hotSpotSizeX.StartDistance; // set old value for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"BoxSizeX",fNewSize)); // send an action which sets the property from old value to new one
        }
      }
      if (hotSpot==_hotSpotSizeY)
      {
        if (_hotSpotSizeY.HasChanged)
        {
          float fNewSize = _hotSpotSizeY.CurrentDistance;
          this.BoxSizeY = _hotSpotSizeY.StartDistance; // set old value for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"BoxSizeY",fNewSize)); // send an action which sets the property from old value to new one
        }
      }
      if (hotSpot==_hotSpotSizeZ)
      {
        if (_hotSpotSizeZ.HasChanged)
        {
          float fNewSize = _hotSpotSizeZ.CurrentDistance;
          this.BoxSizeZ = _hotSpotSizeZ.StartDistance; // set old value for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"BoxSizeZ",fNewSize)); // send an action which sets the property from old value to new one
        }
      }
    }

    /// <summary>
    /// Called every frame and per hotspot that belongs to the shape
    /// </summary>
    /// <param name="hotSpot"></param>
    public override void OnHotSpotEvaluatePosition(HotSpotBase hotSpot)
    {
      if (hotSpot==_hotSpotFanAngle)
      {
        _hotSpotFanAngle.Distance = 50.0f * EditorManager.Settings.GlobalUnitScaling;
        if (!_hotSpotFanAngle.IsDragging)
          _hotSpotFanAngle.StartAngle = _fConeAngle;
        _hotSpotFanAngle.Visible = (ConstraintType == EngineInstanceConstraint.ConstraintType_e.Fan);
      }
      if (hotSpot==_hotSpotFanIntensity)
      {
        _hotSpotFanIntensity.Axis = this.XAxis;
        if (!_hotSpotFanIntensity.IsDragging)
          _hotSpotFanIntensity.StartDistance = Intensity;

        _hotSpotFanIntensity.Visible = ((ConstraintType == EngineInstanceConstraint.ConstraintType_e.Fan)
                                     || (ConstraintType == EngineInstanceConstraint.ConstraintType_e.GravityPoint)
                                     || (ConstraintType == EngineInstanceConstraint.ConstraintType_e.Cyclone));
      }
      if (hotSpot==_hotSpotRadius)
      {
        if (ConstraintType == EngineInstanceConstraint.ConstraintType_e.YAxis)
          _hotSpotRadius.Axis = this.XAxis;
        else
          _hotSpotRadius.Axis = this.YAxis;
        _hotSpotRadius.Visible = ((ConstraintType == EngineInstanceConstraint.ConstraintType_e.Sphere) 
                              || (ConstraintType == EngineInstanceConstraint.ConstraintType_e.Point)
                              || (ConstraintType == EngineInstanceConstraint.ConstraintType_e.XAxis)
                              || (ConstraintType == EngineInstanceConstraint.ConstraintType_e.YAxis)
                              || (ConstraintType == EngineInstanceConstraint.ConstraintType_e.ZAxis)
                              || (ConstraintType == EngineInstanceConstraint.ConstraintType_e.Cyclone)
                              || (ConstraintType == EngineInstanceConstraint.ConstraintType_e.GravityPoint));
        if (!_hotSpotRadius.IsDragging)
          _hotSpotRadius.StartDistance = this.Radius;
      }
      if (hotSpot==_hotSpotLength)
      {
        _hotSpotLength.Axis = this.XAxis;
        _hotSpotLength.Visible = (ConstraintType == EngineInstanceConstraint.ConstraintType_e.Cyclone);
        if (!_hotSpotLength.IsDragging)
          _hotSpotLength.StartDistance = this.Length;
      }

      bool bBoxXY = ConstraintType == EngineInstanceConstraint.ConstraintType_e.AABox || ConstraintType == EngineInstanceConstraint.ConstraintType_e.CameraBox;
      if (ConstraintType == EngineInstanceConstraint.ConstraintType_e.Plane && !InfiniteExtent)
        bBoxXY = true;
      if (hotSpot==_hotSpotSizeX)
      {
        _hotSpotSizeX.Axis = this.XAxis;
        _hotSpotSizeX.Visible = bBoxXY;
        if (!_hotSpotSizeX.IsDragging)
          _hotSpotSizeX.StartDistance = this.BoxSizeX;
      }
      if (hotSpot==_hotSpotSizeY)
      {
        _hotSpotSizeY.Axis = this.YAxis;
        _hotSpotSizeY.Visible = bBoxXY;
        if (!_hotSpotSizeY.IsDragging)
          _hotSpotSizeY.StartDistance = this.BoxSizeY;
      }
      if (hotSpot==_hotSpotSizeZ)
      {
        _hotSpotSizeZ.Axis = this.ZAxis;
        _hotSpotSizeZ.Visible = (ConstraintType == EngineInstanceConstraint.ConstraintType_e.AABox || ConstraintType == EngineInstanceConstraint.ConstraintType_e.CameraBox);
        if (!_hotSpotSizeZ.IsDragging)
          _hotSpotSizeZ.StartDistance = this.BoxSizeZ;
      }
    }

    /// <summary>
    /// Called every frame and per hotspot that belongs to the shape
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    /// <param name="mode"></param>
    public override void OnHotSpotRender(HotSpotBase hotSpot, VisionViewBase view, ShapeRenderMode mode)
    {
      base.OnHotSpotRender (hotSpot, view, mode);

      Vector3F vStart = Position;
      Vector3F vFanInt = this._hotSpotFanIntensity.Position;
      Vector3F vFanAng = this._hotSpotFanAngle.Position;
      Vector3F vCycLen = this._hotSpotLength.Position;
      Vector3F vRad = this._hotSpotRadius.Position;
      
      if (ConstraintType == EngineInstanceConstraint.ConstraintType_e.Cyclone)
        view.RenderLine(vStart, vCycLen, VisionColors.Yellow, 1.0f);

      if ((ConstraintType == EngineInstanceConstraint.ConstraintType_e.Cyclone)
        ||(ConstraintType == EngineInstanceConstraint.ConstraintType_e.Point)
        ||(ConstraintType == EngineInstanceConstraint.ConstraintType_e.Sphere)
        ||(ConstraintType == EngineInstanceConstraint.ConstraintType_e.XAxis)
        ||(ConstraintType == EngineInstanceConstraint.ConstraintType_e.YAxis)
        ||(ConstraintType == EngineInstanceConstraint.ConstraintType_e.ZAxis))
        view.RenderLine(vStart, vRad, VisionColors.Yellow, 1.0f);
    }

    #endregion

    #region Links

    /// <summary>
    /// Create all link sources and targets provided by this shape
    /// </summary>
    public override void CreateAllLinks()
    {
      base.CreateAllLinks();
      LinkSources.Add(new LinkSourceConstraint(this, "ConstraintSource_1")); // add a link source for applying this constraint to particle groups etc.
    }

    /// <summary>
    /// Check whether src can be linked to target
    /// </summary>
    /// <param name="src"></param>
    /// <param name="target"></param>
    /// <returns></returns>
    public override bool CanLink(ShapeLink src, ShapeLink target)
    {
      if (base.CanLink(src,target))
        return true;

      if (src.OwnerShape==this)
      {
        if (target is LinkTargetConstraint) return true;
      }
      return false;
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

    #region Native Constraint class manager

    static DynamicPropertyManager _nativeConstraintClasses = null;
    static string[] _nativeConstraintClassNames = null;

    public static DynamicPropertyManager NativeConstraintClasses
    {
      get
      {
        if (_nativeConstraintClasses == null)
        {
          _nativeConstraintClasses = new DynamicPropertyManager();
          EditorManager.EngineManager.TypeManager_PopulateClassManager(_nativeConstraintClasses, "VisParticleConstraint_cl", true);
        }
        return _nativeConstraintClasses;
      }
    }

    public static string[] ConstraintClassNames
    {
      get
      {
        if (_nativeConstraintClassNames == null)
        {
          DynamicPropertyCollectionType[] types = NativeConstraintClasses.CollectionTypes;
          _nativeConstraintClassNames = new string[types.Length];
          int i = 0;
          foreach (DynamicPropertyCollectionType t in types)
            _nativeConstraintClassNames[i++] = t.DisplayName;
        }
        return _nativeConstraintClassNames;
      }
    }

    public static void InvalidateConstraintClassNames()
    {
      _nativeConstraintClassNames = null;
    }


    #region ConstraintClassConverter

    /// <summary>
    /// Class that provides the dynamic dropdown list for the entity classes
    /// </summary>
    public class ConstraintClassConverter : StringConverter
    {
      /// <summary>
      /// Overridden StringConverter function: We provide standard values
      /// </summary>
      /// <param name="context">The descriptor context</param>
      /// <returns>In our case : true</returns>
      public override bool GetStandardValuesSupported(ITypeDescriptorContext context)
      {
        return true;
      }

      /// <summary>
      /// Overridden StringConverter function: Returns the standard values that the dropdown should have
      /// </summary>
      /// <param name="context">The descriptor context</param>
      /// <returns>Collection of standard values</returns>
      public override StandardValuesCollection GetStandardValues(ITypeDescriptorContext context)
      {
        string[] classNames = ConstraintClassNames;
        return new StandardValuesCollection(classNames);
      }

      /// <summary>
      /// Overridden StringConverter function: 
      /// We also allow non-standard values (i.e. when an entity class is missing/unknown)
      /// </summary>
      /// <param name="context">The descriptor context</param>
      /// <returns>In our case : false</returns>
      public override bool GetStandardValuesExclusive(ITypeDescriptorContext context)
      {
        return false;
      }
    }

    #endregion

    #endregion

  }

  #region Constraint Creator Plugins

  class ConstraintCreator : CSharpFramework.IShapeCreatorPlugin
  {

    // create a constraint of this type
    public ConstraintCreator(EngineInstanceConstraint.ConstraintType_e type)
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\Constraint.bmp"), Color.Magenta);
      CategoryIconIndex = IconIndex;
      _type = type;
    }

    EngineInstanceConstraint.ConstraintType_e _type;

    public override string GetPluginCategory()
    {
      return "Constraints";
    }
    
    public override string GetPluginName() 
    {
      return _type.ToString(); // I love C#
    }

    /// <summary>
    /// Returns a short description text
    /// </summary>
    /// <returns></returns>
    public override string GetPluginDescription()
    {
      return "Adds a constraint of type '"+_type.ToString()+"' to the scene.\n"+
        "A constraint can be linked to shapes such as particles or cloth to affect their behaviour.";
    }


    public override ShapeBase CreateShapeInstance()
    {
      ConstraintShape constraint = new ConstraintShape("Constraint_"+GetPluginName(),_type);
      constraint.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
      return constraint;
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
