/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.IO;
using System.Collections.Generic;
using System.Text;
using CSharpFramework.Shapes;
using CSharpFramework;
using System.Drawing;
using System.Runtime.Serialization;
using HavokManaged;
using System.Diagnostics;
using VisionEditorPlugin.Shapes;
using VisionManaged;
using CSharpFramework.Scene;
using CSharpFramework.UndoRedo;
using System.ComponentModel;
using CSharpFramework.Serialization;
using CSharpFramework.Actions;
using CSharpFramework.Math;
using CSharpFramework.Tests;
using CSharpFramework.View;

namespace HavokEditorPlugin.Shapes
{
  #region Class HavokConstraintShape

  /// <summary>
  /// Shape which represents a Havok Constraint.
  /// Provides a link source for attaching (up to two) Havok anchors and takes care of
  /// the different constraint types and their properties.
  /// </summary>
  [Serializable, ShapeTypeNiceName("Havok Constraint")]
  public class HavokConstraintShape : Shape3D
  {
    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_CONSTRAINT_TYPE = "Constraint Type";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_CONSTRAINT_TYPE = Shape3D.LAST_CATEGORY_ORDER_ID + 1;

    /// <summary>
    /// last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_CONSTRAINT_TYPE;

    #endregion

    #region Constructor

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="nodeName"></param>
    public HavokConstraintShape(string nodeName)
      : base(nodeName)
    {
      AddHint(HintFlags_e.NoScale | HintFlags_e.NoRotation);

      UpdateTypePropertyClass();
    }

    #endregion

    #region GUI

    /// <summary>
    /// Overridden function to get the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetIconIndex()
    {
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\carabiner.png"), Color.Black);
    }

    #endregion

    #region ShapeBase Members

    /// <summary>
    /// Sets parenting mode to scene graph
    /// </summary>
    protected override Shape3D.ParentingModes ParentingMode
    {
      get
      {
        return Shape3D.ParentingModes.SceneGraphScaled;
      }
    }

    /// <summary>
    /// Create the engine instance
    /// </summary>
    /// <param name="bCreateChildren"></param>
    public override void CreateEngineInstance(bool bCreateChildren)
    {
      this._engineInstance = new EngineInstanceHavokConstraint();

      SetEngineInstanceBaseProperties(); // sets the position etc.

      // Listen for changes of the dynamic constraint type properties
      IScene.PropertyChanged += new CSharpFramework.PropertyChangedEventHandler(IScene_PropertyChanged);

      base.CreateEngineInstance(bCreateChildren);
    }

    /// <summary>
    /// Remove the engine instance
    /// </summary>
    /// <param name="bRemoveChildren"></param>
    public override void RemoveEngineInstance(bool bRemoveChildren)
    {
      base.RemoveEngineInstance(bRemoveChildren);

      IScene.PropertyChanged -= new CSharpFramework.PropertyChangedEventHandler(IScene_PropertyChanged);
    }

    // Accesses the engine instance as an object of type EngineInstanceHavokConstraint
    EngineInstanceHavokConstraint EngineConstraintInstance 
    { 
      get { return (EngineInstanceHavokConstraint)_engineInstance; } 
    }

    /// <summary>
    /// overridden function to set/update all specific properties on the engine instance
    /// </summary>
    public override void SetEngineInstanceBaseProperties()
    {
      if (_engineInstance == null)
        return;

      base.SetEngineInstanceBaseProperties();

      // If we don't have type properties yet, create them initially
      if (_typeProperties == null)
        UpdateTypePropertyClass();
      EngineConstraintInstance.SetConstraintType(_constraintType, _typeProperties);

      _engineInstance.SetObjectKey(_objectKey);
    }


    /// <summary>
    /// overridden function to render the shape
    /// </summary>
    /// <param name="view">destination view</param>
    /// <param name="mode">render mode</param>
    public override void RenderShape(VisionViewBase view, ShapeRenderMode mode)
    {
      base.RenderShape(view, mode);

      // First check whether we have a fully configured constraint with an invalid configuration.
      // In that case we display an error sign to inform the user about the invalid configuration
      if (EngineConstraintInstance != null && (!EngineConstraintInstance.IsConfigurationValid()))
      {
        float fSize = 10.0f * EditorManager.Settings.GlobalUnitScaling;
        view.EngineManager.RenderSprite(this.RenderPosition, @"textures\warning32.dds", VisionColors.White, fSize, fSize);
        return;
      }

      if (mode == ShapeRenderMode.Selected || (mode == ShapeRenderMode.Normal && this.ParentLayer == EditorManager.Scene.ActiveLayer))
      {
        float fSize = 3.0f * EditorManager.Settings.GlobalUnitScaling;
        view.EngineManager.RenderSprite(this.RenderPosition, @"textures\carabiner32.dds", VisionColors.White, fSize, fSize);

        if (_engineInstance == null)
          return;
        if (this.IsHintSet(HintFlags_e.Virtual)) // in prefabs it is disturbing
          return;

        if (mode == ShapeRenderMode.Selected)
          EngineConstraintInstance.VisualizeConstraintState(view, mode);
      }
    }

    #endregion

    #region Object key

    /// <summary>
    /// Object key string
    /// </summary>
    protected string _objectKey = null;

    /// <summary>
    /// Gets or sets the object key, so the object can be searched in the engine
    /// </summary>
    [Description("Arbitrary key string to identify this instance inside the game application")]
    [SortedCategory(CAT_SHAPE, CATORDER_SHAPE), PropertyOrder(100)]
    public string ObjectKey
    {
      get { return _objectKey; }
      set
      {
        _objectKey = value;
        if (HasEngineInstance())
          _engineInstance.SetObjectKey(_objectKey);
      }
    }

    #endregion

    #region Constraint Type

    HavokConstraintType_e _constraintType = HavokConstraintType_e.BallAndSocket;
    ConstraintStateBase _typeProperties = null;

    /// <summary>
    /// Get or set the type of the constraint
    /// </summary>
    [SortedCategory(CAT_CONSTRAINT_TYPE, CATORDER_CONSTRAINT_TYPE),
    Description("The constraint type (based on the types available in Havok)"),
    PropertyOrder(1),
    DependentProperties(new string[] { "TypeProperties" })]
    public HavokConstraintType_e ConstraintType
    {
      get { return _constraintType; }
      set
      {
        if (_constraintType == value)
          return;

        _constraintType = value;

        // Create a new type property object which matches the new constraint type
        UpdateTypePropertyClass();

        // The rest sets properties on the engine instance
        if (_engineInstance == null)
          return;

        // Set the new constraint type on the engine instance
        EngineConstraintInstance.SetConstraintType(_constraintType, _typeProperties);
      }
    }

    /// <summary>
    ///Get or set the constraint type specific sub-properties
    /// </summary>
    [SortedCategory(CAT_CONSTRAINT_TYPE, CATORDER_CONSTRAINT_TYPE),
    PropertyOrder(2),
    TypeConverter(typeof(UndoableObjectConverter))]
    public ConstraintStateBase TypeProperties
    {
      get { return _typeProperties; }
      set {
        if (Selected && (_typeProperties != null))
          _typeProperties.DestroyHotspots(this);
        
        _typeProperties = value;

        if (Selected && (_typeProperties != null))
          _typeProperties.CreateHotspots(this);
      }
    }

    /// <summary>
    /// Creates a new type properties object for the currently configured constraint type,
    /// and sets the type properties object in the shape.
    /// </summary>
    void UpdateTypePropertyClass()
    {
      if (Selected && (_typeProperties != null))
        _typeProperties.DestroyHotspots(this);

      switch (_constraintType)
      {
        case HavokConstraintType_e.BallAndSocket: _typeProperties = new BallAndSocketConstraintState(); break;
        case HavokConstraintType_e.Hinge: _typeProperties = new HingeConstraintState(); break;
        case HavokConstraintType_e.Ragdoll: _typeProperties = new RagdollConstraintState(); break;
        case HavokConstraintType_e.StiffSpring: _typeProperties = new StiffSpringConstraintState(); break;
        case HavokConstraintType_e.Fixed: _typeProperties = new FixedConstraintState(); break;
        case HavokConstraintType_e.Prismatic: _typeProperties = new PrismaticConstraintState(false); break;
        case HavokConstraintType_e.Cylindrical: _typeProperties = new PrismaticConstraintState(true); break;
        case HavokConstraintType_e.PointToPlane: _typeProperties = new PointToPlaneConstraintState(); break;
        case HavokConstraintType_e.PointToPath: _typeProperties = new PointToPathConstraintState(); break;
        case HavokConstraintType_e.Pulley: _typeProperties = new PulleyConstraintState(); break;
      }

      if (_typeProperties == null)
        return;

      if (_typeProperties.SupportsRotation())
        this.RemoveHint(HintFlags_e.NoRotation);
      else
        this.AddHint(HintFlags_e.NoRotation);

      if (Selected && (_typeProperties != null))
        _typeProperties.CreateHotspots(this);
    }

    #endregion

    #region ISerializable

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected HavokConstraintShape(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      AddHint(HintFlags_e.NoScale);

      // Read dynamic constraint properties
      _constraintType = (HavokConstraintType_e)info.GetValue("_constraintType", typeof(HavokConstraintType_e));
      _typeProperties = (ConstraintStateBase)info.GetValue("_typeProperties", typeof(ConstraintStateBase));

      if ((_typeProperties != null) && _typeProperties.SupportsRotation())
        this.RemoveHint(HintFlags_e.NoRotation);
      else
        this.AddHint(HintFlags_e.NoRotation);

      if (SerializationHelper.HasElement(info, "_objectKey"))
        _objectKey = info.GetString("_objectKey");
    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);

      info.AddValue("_constraintType", _constraintType);
      info.AddValue("_typeProperties", _typeProperties);

      info.AddValue("_objectKey", _objectKey);
    }

    public override void OnDeserialization()
    {
      base.OnDeserialization();

      // fixup broken saves (might have happend in early versions)
      switch (_constraintType)
      {
        case HavokConstraintType_e.BallAndSocket: if (!(_typeProperties is BallAndSocketConstraintState)) _typeProperties = null; break;
        case HavokConstraintType_e.Hinge: if (!(_typeProperties is HingeConstraintState)) _typeProperties = null; break;
        case HavokConstraintType_e.Ragdoll: if (!(_typeProperties is RagdollConstraintState)) _typeProperties = null; break;
        case HavokConstraintType_e.StiffSpring: if (!(_typeProperties is StiffSpringConstraintState)) _typeProperties = null; break;
        case HavokConstraintType_e.Fixed: if (!(_typeProperties is FixedConstraintState)) _typeProperties = null; break;
        case HavokConstraintType_e.Prismatic: if (!(_typeProperties is PrismaticConstraintState)) _typeProperties = null; break;
        case HavokConstraintType_e.Cylindrical: if (!(_typeProperties is PrismaticConstraintState)) _typeProperties = null; break;
        case HavokConstraintType_e.PointToPlane: if (!(_typeProperties is PointToPlaneConstraintState)) _typeProperties = null; break;
        case HavokConstraintType_e.PointToPath: if (!(_typeProperties is PointToPathConstraintState)) _typeProperties = null; break;
        case HavokConstraintType_e.Pulley: if (!(_typeProperties is PulleyConstraintState)) _typeProperties = null; break;
      }
      if (_typeProperties == null)
        UpdateTypePropertyClass();
    }

    #endregion

    #region Cloning

    /// <summary>
    /// This function must be overridden, since we have to reset the hotspots
    /// </summary>
    /// <returns></returns>
    public override ShapeBase Clone()
    {
      HavokConstraintShape copy = (HavokConstraintShape)base.Clone();

      // Create a deep copy of the type properties
      copy._typeProperties = _typeProperties.Clone() as ConstraintStateBase;

      return copy;
    }

    #endregion

    #region Linking

    public override void CreateAllLinks()
    {
      base.CreateAllLinks();
      LinkSources.Add(new LinkSourceHavokConstraint(this));
    }

    /// <summary>
    /// Performs the actual linking on engine instances
    /// </summary>
    /// <param name="src"></param>
    /// <param name="target"></param>
    public override void OnLink(ShapeLink src, ShapeLink target)
    {
      base.OnLink(src, target);

      // Ignore links which don't belong to us
      if (src.OwnerShape != this)
        return;

      // Get the anchor the target link points to 
      AnchorShape anchorShape = GetAnchorFromLink(target);
      if (anchorShape == null || !HasEngineInstance())
        return;

      // Get the entity instance the target points to
      EngineInstanceEntity entityInstance = GetEntityFromAnchor(anchorShape);

      // Add the actor to the constraint
      if (entityInstance != null)
      {
        // We have an anchor which belongs to an entity. Add the entity to the constraint
        // and pass the anchor position, relative to the parent entity.
        Vector3F anchorPositionLS = anchorShape.LocalSpacePosition;
        bool bResult = EngineConstraintInstance.AddEntityAnchor(
          (long)anchorShape.UniqueID, entityInstance, anchorPositionLS);
        Debug.Assert(bResult == true);
      }
      else
      {
        // Our anchor is statically attached to the world.
        // Add the static world anchor to the constraint. Pass the world space position of the anchor
        // for this purpose.
        Vector3F anchorPositionWS = anchorShape.Position;
        bool bResult = EngineConstraintInstance.AddWorldAnchor((long)anchorShape.UniqueID, anchorShape);
        Debug.Assert(bResult == true);
      }
    }

    /// <summary>
    /// unlinks a target from a source. Either src or target has this shape as owner
    /// </summary>
    /// <param name="src">the link source</param>
    /// <param name="target">the link target</param>
    public override void OnUnlink(ShapeLink src, ShapeLink target)
    {
      base.OnUnlink(src, target);

      // Ignore links which don't belong to us
      if (src.OwnerShape != this)
        return;

      // Get the anchor the target link points to 
      AnchorShape anchorShape = GetAnchorFromLink(target);
      if (anchorShape == null || !HasEngineInstance())
        return;

      bool bResult = EngineConstraintInstance.RemoveAnchor((long)anchorShape.UniqueID);
      Debug.Assert(bResult == true);
    }

    /// <summary>
    /// Gets the anchor which belongs to the passed link target
    /// </summary>
    /// <param name="linkTarget">link target</param>
    /// <returns>anchor instance</returns>
    private AnchorShape GetAnchorFromLink(ShapeLink linkTarget)
    {
      // Get the anchor shape which is connected to the passed link
      AnchorShape anchorShape = linkTarget.OwnerShape as AnchorShape;
      Debug.Assert(anchorShape != null);

      return anchorShape;
    }

    /// <summary>
    /// Gets the engine instance which belongs to the passed anchor, or NULL if the anchor
    /// is not attached to an entity.
    /// </summary>
    /// <param name="linkTarget">link target</param>
    /// <returns>Entity engine instance, or NULL if anchor is not attached to an entity</returns>
    private EngineInstanceEntity GetEntityFromAnchor(AnchorShape anchorShape)
    {
      // Get the entity shape which holds the anchor
      EntityShape entityShape = anchorShape.Parent as EntityShape;
      if (entityShape == null)
        return null;

      // Next get the engine instance of the entity shape
      EngineInstanceEntity engineEntityInstance = entityShape._engineInstance as EngineInstanceEntity;
      if (engineEntityInstance == null)
        return null;

      return engineEntityInstance;
    }

    #endregion

    #region Hotspot

    /// <summary>
    /// Called when the shape is selected
    /// </summary>
    public override void OnSelected()
    {
      base.OnSelected();
      if (_typeProperties != null)
        _typeProperties.CreateHotspots(this);
    }

    /// <summary>
    /// Called when the shape is unselected
    /// </summary>
    public override void OnUnSelected()
    {
      if (_typeProperties != null)
        _typeProperties.DestroyHotspots(this);
      base.OnUnSelected();
    }

    /// <summary>
    /// Called when user starts dragging a hotspot that belongs to this shape
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragBegin(HotSpotBase hotSpot, VisionViewBase view)
    {
      if (_typeProperties != null)
        _typeProperties.OnHotSpotDragBegin(hotSpot, view);
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
      if (_typeProperties != null)
        _typeProperties.OnHotSpotDrag(hotSpot, view, fDeltaX, fDeltaY);
    }

    /// <summary>
    /// Called when the user releases the mouse button on a hotspot
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragEnd(HotSpotBase hotSpot, VisionViewBase view)
    {
      if (_typeProperties != null)
        _typeProperties.OnHotSpotDragEnd(hotSpot, view);
    }

    /// <summary>
    /// Called every frame and per hotspot that belongs to the shape
    /// </summary>
    /// <param name="hotSpot"></param>
    public override void OnHotSpotEvaluatePosition(HotSpotBase hotSpot)
    {
      if (_typeProperties != null)
        _typeProperties.OnHotSpotEvaluatePosition(hotSpot);
    }

    #endregion

    #region Scene Event Handlers

    private void IScene_PropertyChanged(object sender, PropertyChangedArgs e)
    {
      // Check whether a constraint property has been changed, so that we can update the native
      // properties (useful when running the game in editor). Situations to cover:
      // - User directly modified a property of the typeProperty instance: can be explicitly checked
      // - User modified the value of a sub-property: difficult to check. In that case we assume that
      // - The user moved/rotated/scaled a parent of the constraint shape
      //   any component belonging to the HavokEditorPlugin/HavokManaged assemblies is relevant for us.
      if (e._component == _typeProperties ||
          e._component.GetType().Assembly == this.GetType().Assembly ||
          e._component.GetType().Assembly == typeof(HavokManaged.ConstraintStateBase).Assembly ||
          (IsChildOrSubChildOf(e._component as ShapeBase) && (e._propertyName == "Position" || e._propertyName == "Orientation" || e._propertyName == "Scaling") ))
      {
        if (_engineInstance != null && ParentLayer.Modifiable)
        {
          // Set the constraint properties on the engine instance, since the user modified them
          EngineConstraintInstance.SetConstraintProperties(_typeProperties);
          this.ParentLayer.Modified = true;
        }
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

  #region HavokConstraintShape Creator Plugin

  [PrefabTest.IgnoreForPrefabTests]
  class HavokConstraintShapeCreator : CSharpFramework.IShapeCreatorPlugin
  {
    public HavokConstraintShapeCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\carabiner.png"), Color.Black);
      CategoryIconIndex = IconIndex;
    }

    public override string GetPluginName()
    {
      return "Constraint";
    }

    public override string GetPluginCategory()
    {
      return "Havok";
    }

    /// <summary>
    /// Returns a short description text
    /// </summary>
    /// <returns></returns>
    public override string GetPluginDescription()
    {
      return "This shape describes various sub-types of Havok constraints. To establish a constraint, two instances of constraint anchor shapes can be conected using the linking mode.";
    }


    public override ShapeBase CreateShapeInstance()
    {
      HavokConstraintShape shape = new HavokConstraintShape("Constraint");
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
