/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Runtime.Serialization;
using System.Text;

using CSharpFramework;
using CSharpFramework.Actions;
using CSharpFramework.Math;
using CSharpFramework.Scene;
using CSharpFramework.Serialization;
using CSharpFramework.Shapes;
using CSharpFramework.UndoRedo;

using HavokManaged;

using VisionEditorPlugin.Shapes;

using VisionManaged;
using CSharpFramework.Tests;


namespace HavokEditorPlugin.Shapes
{
  #region Class HavokConstraintChainShape

  /// <summary>
  /// Shape which represents a Havok Constraint Chain.
  /// Provides a link source for attaching (up to two) Havok anchors and takes 
  /// care of the different constraint chain types and the different 
  /// visualizations.
  /// </summary>
  [Serializable, ShapeTypeNiceName("Havok Constraint Chain")]
  public class HavokConstraintChainShape : Shape3D
  {
    #region Category Sorting Definitions

    /// <summary>
    /// Category string for chain settings
    /// </summary>
    protected const string CAT_CONSTRAINT_CHAIN_TYPE = "Constraint Chain Type";

    /// <summary>
    /// Category string for chain quality
    /// </summary>
    protected const string CAT_CONSTRAINT_CHAIN_QUALITY = "Constraint Chain Quality";


    /// <summary>
    /// Category ID for chain settings
    /// </summary>
    protected const int CATORDER_CONSTRAINT_CHAIN_TYPE = Shape3D.LAST_CATEGORY_ORDER_ID + 1;

    /// <summary>
    /// Category ID for chain quality
    /// </summary>
    protected const int CATORDER_CONSTRAINT_CHAIN_QUALITY = CATORDER_CONSTRAINT_CHAIN_TYPE + 1;


    /// <summary>
    /// last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_CONSTRAINT_CHAIN_QUALITY;

    #endregion

    #region Constructor

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="nodeName"></param>
    public HavokConstraintChainShape(string nodeName)
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
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\link.png"), Color.Black);
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
      this._engineInstance = new EngineInstanceHavokConstraintChain(this);

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

    // Accesses the engine instance as an object of type EngineInstanceHavokConstraintChain
    EngineInstanceHavokConstraintChain EngineConstraintChainInstance
    {
      get { return (EngineInstanceHavokConstraintChain)_engineInstance; }
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
      EngineConstraintChainInstance.SetConstraintChainType(_constraintChainType, _typeProperties);

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
      if (EngineConstraintChainInstance != null && (!EngineConstraintChainInstance.IsConfigurationValid()))
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

    #region Constraint Chain Type

    HavokConstraintChainType_e _constraintChainType = HavokConstraintChainType_e.BallAndSocket;
    ConstraintChainStateBase _typeProperties = null;

    /// <summary>
    /// Get or set the type of the constraint chain
    /// </summary>
    [SortedCategory(CAT_CONSTRAINT_CHAIN_TYPE, CATORDER_CONSTRAINT_CHAIN_TYPE),
    Description("The constraint chain type (based on the types available in Havok)"),
    PropertyOrder(1),
    DependentProperties(new string[] { "TypeProperties" })]
    public HavokConstraintChainType_e ConstraintChainType
    {
      get { return _constraintChainType; }
      set
      {
        if (_constraintChainType == value)
          return;

        _constraintChainType = value;

        // Create a new type property object which matches the new constraint type
        UpdateTypePropertyClass();

        // The rest sets properties on the engine instance
        if (_engineInstance == null)
          return;

        // Set the new constraint type on the engine instance
        EngineConstraintChainInstance.SetConstraintChainType(_constraintChainType, _typeProperties);
      }
    }

    /// <summary>
    ///Get or set the constraint type specific sub-properties
    /// </summary>
    [SortedCategory(CAT_CONSTRAINT_CHAIN_TYPE, CATORDER_CONSTRAINT_CHAIN_TYPE),
    PropertyOrder(2),
    TypeConverter(typeof(UndoableObjectConverter))]
    public ConstraintChainStateBase TypeProperties
    {
      get { return _typeProperties; }
      set { _typeProperties = value; }
    }

    /// <summary>
    /// Creates a new type properties object for the currently configured constraint type,
    /// and sets the type properties object in the shape.
    /// </summary>
    void UpdateTypePropertyClass()
    {
      switch (_constraintChainType)
      {
        case HavokConstraintChainType_e.BallAndSocket: 
          _typeProperties = new BallAndSocketConstraintChainState(); 
          break;
        case HavokConstraintChainType_e.StiffSpring: 
          _typeProperties = new StiffSpringConstraintChainState(); 
          break;
      }

      if (_typeProperties == null)
        return;

      if (_typeProperties.SupportsRotation())
        this.RemoveHint(HintFlags_e.NoRotation);
      else
        this.AddHint(HintFlags_e.NoRotation);
    }

    #endregion

    #region Constraint Chain Quality

    [SortedCategory(CAT_CONSTRAINT_CHAIN_QUALITY, CATORDER_CONSTRAINT_CHAIN_QUALITY),
     PropertyOrder(1),
     Description("Number of links in the constraint chain")]
    public float NumLinks
    {
      get
      {
        return (EngineConstraintChainInstance != null) ? EngineConstraintChainInstance.GetNumLinks() : 0;
      }
    }

    [SortedCategory(CAT_CONSTRAINT_CHAIN_QUALITY, CATORDER_CONSTRAINT_CHAIN_QUALITY),
     PropertyOrder(2),
     Description("The average error of the approximation of the path with the chain links.")]
    public String LinkApproximationError
    {
      get
      {
        if (EngineConstraintChainInstance != null)
          return String.Format("{0:F2} %", EngineConstraintChainInstance.GetApproximationError() * 100);
        else
          return "---";
      }
    }

    #endregion

    #region ISerializable

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected HavokConstraintChainShape(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      AddHint(HintFlags_e.NoScale);

      // Write dynamic constraint properties
      _constraintChainType = (HavokConstraintChainType_e)info.GetValue("_constraintChainType", typeof(HavokConstraintChainType_e));
      _typeProperties = (ConstraintChainStateBase)info.GetValue("_typeProperties", typeof(ConstraintChainStateBase));

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

      info.AddValue("_constraintChainType", _constraintChainType);
      info.AddValue("_typeProperties", _typeProperties);

      info.AddValue("_objectKey", _objectKey);
    }

    #endregion

    #region Cloning

    /// <summary>
    /// This function must be overridden, since we have to reset the hotspots
    /// </summary>
    /// <returns></returns>
    public override ShapeBase Clone()
    {
      HavokConstraintChainShape copy = (HavokConstraintChainShape)base.Clone();

      // Create a deep copy of the type properties
      copy._typeProperties = _typeProperties.Clone() as ConstraintChainStateBase;

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
        bool bResult = EngineConstraintChainInstance.AddEntityAnchor(
          (long)anchorShape.UniqueID, entityInstance, anchorPositionLS);
        Debug.Assert(bResult == true);
      }
      else
      {
        // Our anchor is statically attached to the world.
        // Add the static world anchor to the constraint. Pass the world space position of the anchor
        // for this purpose.
        Vector3F anchorPositionWS = anchorShape.Position;
        bool bResult = EngineConstraintChainInstance.AddWorldAnchor((long)anchorShape.UniqueID, anchorShape);
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

      bool bResult = EngineConstraintChainInstance.RemoveAnchor((long)anchorShape.UniqueID);
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

    #region Scene Event Handlers

    private void IScene_PropertyChanged(object sender, PropertyChangedArgs e)
    {
      // Check whether a constraint property has been changed, so that we can update the native
      // properties (useful when running the game in editor). Situations to cover:
      // - User directly modified a property of the typeProperty instance: can be explicitly checked
      // - User modified the value of a sub-property: difficult to check. In that case we assume that
      //   any component belonging to the HavokEditorPlugin/HavokManaged assemblies is relevant for us.
      if (e._component == _typeProperties ||
          e._component.GetType().Assembly == this.GetType().Assembly ||
          e._component.GetType().Assembly == typeof(HavokManaged.ConstraintChainStateBase).Assembly)
      {
        if (_engineInstance != null && ParentLayer.Modifiable)
        {
          // Set the constraint properties on the engine instance, since the user modified them
          EngineConstraintChainInstance.SetConstraintChainProperties(_typeProperties);
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

  #region HavokConstraintChainShape Creator Plugin

  [PrefabTest.IgnoreForPrefabTests]
  class HavokConstraintChainShapeCreator : CSharpFramework.IShapeCreatorPlugin
  {
    public HavokConstraintChainShapeCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\link.png"), Color.Black);
    }

    public override string GetPluginName()
    {
      return "Constraint Chain";
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
      return "This shape describes various sub-types of Havok constraint chains. To establish a constraint chain, two anchor shapes can be connected using the linking mode.";
    }


    public override ShapeBase CreateShapeInstance()
    {
      HavokConstraintChainShape shape = new HavokConstraintChainShape("Constraint Chain");
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
