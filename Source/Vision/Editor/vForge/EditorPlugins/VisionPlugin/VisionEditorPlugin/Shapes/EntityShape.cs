/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Design;
using System.IO;
using System.Reflection;
using System.Runtime.Serialization;
using System.Windows.Forms;
using System.Windows.Forms.Design;
using System.Xml;
using CSharpFramework;
using CSharpFramework.Actions;
using CSharpFramework.BaseTypes;
using CSharpFramework.DynamicProperties;
using CSharpFramework.Math;
using CSharpFramework.PropertyEditors;
using CSharpFramework.Scene;
using CSharpFramework.Serialization;
using CSharpFramework.Shapes;
using CSharpFramework.View;
using ManagedFramework;
using VisionManaged;
using CSharpFramework.AssetManagement;

namespace VisionEditorPlugin.Shapes
{

  #region EntityClassManager

  /// <summary>
  /// Holds RTTI information for all engine entity classes. An instance of this class can be accessed via EditorManager.EngineManager.EntityClassManager
  /// </summary>
  public class EntityClassManager : DynamicPropertyManager
  {
    public override DynamicPropertyCollectionType CreateNewType(string uniqueName, DynamicPropertyCollectionType.DynamicPropertyCollectionFlags_e flags)
    {
      return new EntityPropertyCollectionType(uniqueName, flags);
    }
  }


  /// <summary>
  /// Inherits from DynamicPropertyCollectionType to return entity specific instances
  /// </summary>
  [Serializable]
  public class EntityPropertyCollectionType : DynamicPropertyCollectionType, ISerializable
  {
    public EntityPropertyCollectionType(string uniqueName, DynamicPropertyCollectionFlags_e flags)
      : base(uniqueName, flags)
    {
    }

    protected EntityPropertyCollectionType(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
    }

    public override DynamicPropertyCollection CreateInstance(ISceneObject forObject)
    {
      return new EntityPropertyCollection(forObject, this);
    }
  }

  /// <summary>
  /// Inherits from DynamicPropertyCollection to add entity specific functionality
  /// </summary>
  [Serializable]
  public class EntityPropertyCollection : DynamicPropertyCollection, ISerializable
  {
    public EntityPropertyCollection(ISceneObject owner, DynamicPropertyCollectionType collectionType)
      : base(owner, collectionType)
    {
    }

    protected EntityPropertyCollection(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
    }

    public string AsEntityString(DynamicProperty p)
    {
      // see also EngineComponentManager::SetNativeComponentProperty:
      // note we are not checking p.PropertyType.IsEnum, which would be different
      if (p.PropertyType.NativeType.IsEnum)
      {
        int iAsInt = (int)p.Value;
        return iAsInt.ToString();
      }
      return SerializationHelper.GetEngineStringFromObject(p.Value);
    }

    public override void OnValueChanged(DynamicProperty prop)
    {
      base.OnValueChanged(prop);

      EntityShape entity = Owner as EntityShape;
      Debug.Assert(entity != null);

      if (entity.HasEngineInstance())
      {
        string varValue = AsEntityString(prop);
        entity.EngineEntity.SetVariable(prop.PropertyType.Name, varValue);
        if (entity.ParentLayer != null)
          entity.ParentLayer.Modified = true;
      }
    }
  }


  #endregion

  /// <summary>
	/// Shape that describes an entity instance.
	/// </summary>
	[DefaultProperty("Engine Class"),
   ShapeTypeNiceName("Entity"),
   Serializable
  ]
  public class EntityShape : ShapeObject3D, IStandardValueProvider, ISetPropertyActionProvider, IPropertyValueChangingCallback, IShapeReferenceOwner
	{
    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_SETTINGS   = "Properties";

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_LIGHTANDSHADOW = "Light and Shadow";

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_LIGHTGRID = "Light grid";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_SETTINGS = Shape3D.LAST_CATEGORY_ORDER_ID + 1;

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_LIGHTANDSHADOW = Shape3D.LAST_CATEGORY_ORDER_ID + 2;

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_LIGHTGRID = Shape3D.LAST_CATEGORY_ORDER_ID + 3;

    /// <summary>
    /// last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_LIGHTGRID;

    #endregion

    #region Constructor

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="nodeName"></param>
    public EntityShape(string nodeName)
      : base(nodeName)
		{
      EntityClass = "VisBaseEntity_cl";
    }

    #endregion

    #region GUI

    [Browsable(false)]
    public static string ICON_NAME
    {
      get
      {
        return Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\Entity.bmp");
      }
    }

    /// <summary>
    /// Overridden function to get the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetIconIndex()
    {
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(ICON_NAME, Color.Magenta);
    }

    #endregion

    #region Shape3D Members

    /// <summary>
    /// Create the engine instance
    /// </summary>
    /// <param name="bCreateChildren"></param>
    public override void CreateEngineInstance(bool bCreateChildren)
    {
      Debug.Assert(this._engineInstance == null);

      StringDictionary propertyDict = GetEntityPropertiesAsString();
      try
      {
        // Create the engine instance, but WITHOUT calling the entity's InitFunction.
        // We will take care of this in our OnBeforeCreateEngineInstanceChildren once all necessary
        // initialisations have been done (e.g. all components created).
        bool bCallInitFunction = false;
        this._engineInstance = new EngineInstanceEntity(EntityClass, ModelFile, this, propertyDict, bCallInitFunction);
      } 
      catch (Exception ex)
      {
        EditorManager.DumpException(ex, true);
      }

      // Set all the properties on the entity (before the InitFunction is called)
      SetEngineInstanceBaseProperties();

      // Call the base function to setup attachments and process the children. The engine instance
      // has to exist at this point.
      // This will also result in the OnBeforeCreateEngineInstanceChildren being called, and will
      // thus trigger the entity's InitFunction 
      base.CreateEngineInstance(bCreateChildren);
    }

    public override void OnBeforeCreateEngineInstanceChildren()
    {
      // Finally call the InitFunction on the entity manually
      if (_engineInstance != null)
        EngineEntity.CallInitFunction();
    }

    /// <summary>
    /// Remove the engine instance
    /// </summary>
    /// <param name="bRemoveChildren"></param>
    public override void RemoveEngineInstance(bool bRemoveChildren)
    {      
      base.RemoveEngineInstance(bRemoveChildren);
    }

    /// <summary>
    /// Return the engine instance casted to EngineInstanceEntity
    /// </summary>
    [BrowsableAttribute(false)]
    public EngineInstanceEntity EngineEntity {get {return _engineInstance as EngineInstanceEntity;}}

    /// <summary>
    /// For entities the parenting mode is usually SceneGraphScaled, unless there is a VFORGE_USE_NATIVE_PARENTING flag in the native VARTABLE
    /// </summary>
    protected virtual ParentingModes ParentingMode
    {
      get
      {
        if (IsHintSet(ShapeBase.HintFlags_e.ForceSceneGraphParenting))
          return ParentingModes.SceneGraphScaled;
        return ParentingModes.EngineInstance;
      }
    }

    public override void OnAddedToScene()
    {
      base.OnAddedToScene();
      _snappoints = StaticMeshShape.SNAPPOINTLIST_CACHE_INVALID; // force update next time
    }

    public override void OnRemoveFromScene()
    {
      base.OnRemoveFromScene();
      _snappoints = null;
      if (_entityProperties != null)
        _entityProperties.OnRemoveFromScene();
    }

    // this is very entity specific. See ticket [#24277]
    // It is used to store whether this entity shape is the original one or a copied version. The programmer can
    // respond to this in the messagefunction of the entity (VIS_MSG_EDITOR_CLONESOURCE)
    EntityShape _copySource = null;

    void ValidateCopySource()
    {
      if (_copySource == null)
        return;
      if (!_copySource.IsAddedToScene)
        _copySource = null;
    }

    /// <summary>
    /// overridden clone function
    /// </summary>
    /// <returns></returns>
    public override ShapeBase Clone()
    {
      // perform deep copy of the dynamic properties
      EntityShape clonedShape = base.Clone() as EntityShape;
      clonedShape._entityProperties = (DynamicPropertyCollection)_entityProperties.Clone();
      clonedShape._entityProperties.Owner = clonedShape;
      clonedShape._snappoints = StaticMeshShape.SNAPPOINTLIST_CACHE_INVALID; // force update next time;

      clonedShape._hotSpotLightGridOfs = null;
      clonedShape._cachedBoneList = null; // invalidate cache (and re-cache via children)
      clonedShape._copySource = this;
      while (clonedShape._copySource._copySource != null)
        clonedShape._copySource = clonedShape._copySource._copySource;
      ValidateCopySource();

      return clonedShape;
    }


    /// <summary>
    /// Overidden function that sets the shape's properties on the engine instance
    /// </summary>
    public override void SetEngineInstanceBaseProperties()
    {
      base.SetEngineInstanceBaseProperties();
      if (_engineInstance == null)
        return;
      EngineEntity.SetCastDynamicShadows(_bCastDynamicShadows);
      EngineEntity.SetCastStaticShadows(_bCastStaticShadows);
      EngineEntity.SetLightgridSamplePosition(_vLightGridOfs.X,_vLightGridOfs.Y,_vLightGridOfs.Z);
      EngineEntity.SetVisibleBitmask((uint)_iVisibleBitmask);
      EngineEntity.SetLightInfluenceBitmask((uint)_iLightInfluenceBitmask);
      //EngineEntity.SetLightmapGranularity(_fLightmapTiling);
      EngineEntity.SetAmbientColor(VisionColors.Get(_ambientColor));
      EngineEntity.SetClipDistances(_nearClipDistance, _farClipDistance);
      EngineEntity.SetPrimarySortingKey(_renderOrderPriority);

      if (_copySource != null)
      {
        if (_copySource.HasEngineInstance())
        EngineEntity.SetCopySource(_copySource.EngineEntity);
      }
      else
        EngineEntity.SetCopySource(null);
      if (EngineEntity.WantsNativeParenting())
        this.RemoveHint(HintFlags_e.ForceSceneGraphParenting);
      else
        this.AddHint(HintFlags_e.ForceSceneGraphParenting);

      UpdateBoneAttachement();
      ApplyCustomMaterialSet();
    }

    public override void PerformPostEngineInstanceCreationSetup(bool bCreateChildren)
    {
      base.PerformPostEngineInstanceCreationSetup(bCreateChildren);
      SynchronizeOwnComponents();
    }

    void SynchronizeOwnComponents()
    {
      if (Components == null)
        Components = new ShapeComponentCollection();
      EngineEntity.SynchronizeOwnComponents(this, Components);
      if (ComponentCount == 0)
        Components = null;
    }

    public override void ReassignAllLinks()
    {
      base.ReassignAllLinks();

      ValidateCopySource(); // clone source still in the scene?
      if (HasEngineInstance())
      {
        if (_copySource != null)
          EngineEntity.SetCopySource(_copySource.EngineEntity);
        else
          EngineEntity.SetCopySource(null);


        // Special case treatment for entity propertiesof type ShapeReference. These must be re-assigned now, because otherwise it is not guaranteed
        // that the referenced shape has a valid engine instance yet. (initialization order issue).
        // Similar code can be found in ShapeBase.ReassignAllLinks to update components as well
        if (_entityProperties != null)
        {
          foreach (DynamicProperty prop in _entityProperties.Properties)
            if (prop.PropertyType.NativeType == typeof(ShapeReference)) // only handle this type of property here - other properties are handled at engine instance creation time
              EngineEntity.SetEntityProperty(prop);
        }
      }
    }

    public override object ToolTip
    {
      get 
      {
        string _model = HasModel ? ", Model : "+ModelFile : ", No model";
        return this.EntityClass + _model;
      }
    }

    public override PropertyFlags_e GetPropertyFlags(PropertyFlagsProviderInfo pd)
    {
      // special checks on the dynamic property side of entities
      if (pd.DynamicProperty != null && HasEngineInstance())
      {
        return EngineEntity.GetPropertyFlags(pd.DynamicProperty);
      }

      if (pd.Name == "ExposedBones" && (!HasEngineInstance() || !EngineEntity.HasBones()))
        return PropertyFlags_e.Hidden;
        
      return base.GetPropertyFlags(pd);
    }


    public override void GetRelevantProperties(List<PropertyInfo> targetList, PropertyRelevantQueryReason_e context)
    {
      base.GetRelevantProperties(targetList, context);
      if (context == PropertyRelevantQueryReason_e.PrefabExposeParameter)
        targetList.Add(GetType().GetProperty("EntityProperties"));
    }


    // This is the code to display entity properties at root level:
    /*
    static PropertyFlagsProviderInfo _probeInfo = new PropertyFlagsProviderInfo();


    public override List<PropertyDescriptor> GetAdditionalRootProperties(ITypeDescriptorContext context)
    {
      List<PropertyDescriptor> list = base.GetAdditionalRootProperties(context);
      if (this._entityProperties == null || _entityProperties.Properties.Count == 0)
        return list;

      if (list == null)
        list = new List<PropertyDescriptor>();

      foreach (DynamicProperty prop in _entityProperties.Properties)
      {
        CategoryAttribute catAttr = (CategoryAttribute)prop.PropertyType.GetAttribute(typeof(CategoryAttribute), true);
        if (catAttr == null)
          prop.PropertyType.AddCategoryAttribute("Entity Properties"); // add a category atribute permanently to the type so it does not show up under "Misc"
        _probeInfo.Name = prop.PropertyType.Name;
        _probeInfo.DynamicProperty = prop;
        PropertyFlags_e propFlags = _entityProperties.GetPropertyFlags(_probeInfo);
        if ((propFlags & PropertyFlags_e.Hidden) > 0)
          continue;
        
        list.Add(new UndoRedoPropertyDescriptor(new DynamicPropertyDescriptor(prop), PropertyFlags_e.None, null, _entityProperties));
      }
      return list;
    }
    
    */
    #endregion 

    #region Model file

    /// <summary>
    /// Model File Property (uses filename editor)
    /// </summary>

    [PrefabResolveFilename]
    [SortedCategory(CAT_SETTINGS, CATORDER_SETTINGS), PropertyOrder(20)]
    [EditorAttribute(typeof(AssetEditor), typeof(UITypeEditor)), AssetDialogFilter(new string[] { "Model" })]
    [Description("Model used by this entity.")]
    [ThumbnailFormat(new string[] { @"{0}_data\Thumbnail.bmp", @"{0}_data\Thumbnail.jpg" })]
    [DependentProperties(new string[] { "AutoRenaming" })]
    public string ModelFile
    {
      get { return _modelFile; }
      set
      {
        if (_modelFile == value) 
          return;
        _modelFile = value;

        // Set name automatically unless the shape has been renamed manually
        if (AutoRenaming && EditorManager.Scene != null)
        {
          string name = Path.GetFileNameWithoutExtension(_modelFile);
          ShapeName = String.IsNullOrEmpty(name) ? "Entity" : name;
        }

        if (this.HasEngineInstance())
        {
          EngineEntity.SetModelFile(_modelFile);
        }
        InvalidateZoneBoundingBox();
        // invalidate cache
        _cachedBoneList = null;
        ApplyCustomMaterialSet();
        _snappoints = StaticMeshShape.SNAPPOINTLIST_CACHE_INVALID; // force update next time
      }
    }
    string _modelFile = "";


    /// <summary>
    /// Indicates whether this entity has a model or not
    /// </summary>
    [Browsable(false)]
    public bool HasModel
    {
      get
      {
        return !string.IsNullOrEmpty(_modelFile);
      }
    }

    /// <summary>
    /// gets the material provider of the entity's model
    /// </summary>
    [BrowsableAttribute(false)]
    public IMaterialProvider MaterialProvider
    {
      get { return EditorManager.EngineManager.FindMaterialProvider(ModelFile); }
      set 
      {
        if (value==null)
          ModelFile = "";
        else
          ModelFile = value.FileName;
      }

    }

    string _customMaterialSetName = null;

    [Description("Sets an individual material set on this instance. Variants of the mesh's material set can be created in the material editing layout. If left to <none> the mesh's default material set is used.")]
    [SortedCategory(CAT_SETTINGS, CATORDER_SETTINGS), PropertyOrder(21)]
    [TypeConverter(typeof(CustomMaterialSetConverter))]
    public string CustomMaterialSet
    {
      get 
      {
        if (string.IsNullOrEmpty(_customMaterialSetName))
          return CustomMaterialSetConverter.NONE;
        return _customMaterialSetName; 
      }
      set 
      {
        if (string.Compare(value, CustomMaterialSetConverter.NONE, true) == 0)
          _customMaterialSetName = null;
        else
          _customMaterialSetName = value;
        ApplyCustomMaterialSet();
      }
    }

    void ApplyCustomMaterialSet()
    {
      if (!HasEngineInstance())
        return;
      CustomMaterialSetProvider variant = null;
      if (!string.IsNullOrEmpty(_customMaterialSetName))
      {
        IMaterialProvider materials = this.MaterialProvider;
        if (materials != null)
          variant = materials.FindVariantByName(_customMaterialSetName) as CustomMaterialSetProvider;
      }
      EngineEntity.SetCustomMaterialSet(variant);
    }


    #endregion 

    #region Exposed Bones

    List<BoneProxyShape> _cachedBoneList = null;

    [PrefabRelevant(false)]
    [SortedCategory(CAT_SETTINGS, CATORDER_SETTINGS), PropertyOrder(30)]
    [EditorAttribute(typeof(SkeletalBoneSelectionEditor), typeof(UITypeEditor))]
    [TypeConverter(typeof(SkeletalBoneListConverter))]
    [Description("Collection of skeletal bones that are exposed as sub-shapes to attach other objects")]
    public List<BoneProxyShape> ExposedBones
    {
      get
      {
        if (_cachedBoneList == null)
        {
          _cachedBoneList = new List<BoneProxyShape>();
          foreach (ShapeBase child in ChildCollection)
            if (child is BoneProxyShape)
              _cachedBoneList.Add((BoneProxyShape)child);
        }
        return _cachedBoneList;
      }
      set
      {
        _cachedBoneList = value;
      }
    }

    [Browsable(false)]
    public bool HasExposedBones
    {
      get
      {
        List<BoneProxyShape> bones = ExposedBones;
        return bones != null && bones.Count > 0;
      }
    }

    public BoneProxyShape FindBoneProxyShape(string name)
    {
      foreach (ShapeBase shape in this.ChildCollection)
      {
        if (shape is BoneProxyShape && ((BoneProxyShape)shape).BoneName == name)
          return (BoneProxyShape)shape;
      }
      return null;
    }

    public BoneProxyShape CreateBoneProxyShape(string name, bool bUndoable)
    {
      BoneProxyShape shape = FindBoneProxyShape(name);
      if (shape == null)
      {
        shape = new BoneProxyShape(name);
        IAction action = AddShapeAction.CreateAddShapeAction(shape, this, ParentLayer, false);
        if (bUndoable)
          EditorManager.Actions.Add(action);
        else
          action.Do();
      }
      return shape;
    }



    public static List<SkeletalBoneDesc> tempBones = new List<SkeletalBoneDesc>();

    static SkeletalBoneDesc FindBoneDescByName(List<SkeletalBoneDesc> list, string name)
    {
      if (list != null)
        foreach (SkeletalBoneDesc desc in list)
          if (desc.Name == name)
            return desc;
      return null;
    }

    void RemoveBone(GroupAction actions, BoneProxyShape bone)
    {
      // attach to new parent
      if (bone.HasChildren())
        actions.Add(new SetShapesParentAction(bone.ChildCollection, bone.Parent));

      actions.Add(RemoveShapeAction.CreateRemoveShapeAction(bone));
    }

    void CollectModelChangedActions(string newModelFile, GroupAction actions)
    {
      tempBones.Clear();
      EngineInstanceEntity.GetBones(newModelFile, tempBones);

      List<BoneProxyShape> list = this.ExposedBones;

      if (list != null)
        foreach (BoneProxyShape bone in list)
          if (FindBoneDescByName(tempBones,bone.BoneName)==null) // remove bone shape
            RemoveBone(actions, bone);
          else
            bone.SetEngineInstanceBaseProperties();

    }

    public override void OnChildRemoved(ShapeBase childShape, int iDepth)
    {
      base.OnChildRemoved(childShape, iDepth);
      if (iDepth == 1 && childShape is BoneProxyShape)
        _cachedBoneList = null;
    }

    public override void OnChildAdded(ShapeBase childShape, int iDepth)
    {
      base.OnChildAdded(childShape, iDepth);
      if (iDepth == 1 && childShape is BoneProxyShape)
        _cachedBoneList = null;
    }

    void UpdateBoneAttachement()
    {
      List<BoneProxyShape> bones = ExposedBones;
      if (bones == null || bones.Count == 0)
        return;
      foreach (BoneProxyShape bone in bones)
      {
        if (bone.HasEngineInstance())
        {
          bone.RemoveEngineInstance(true);
          bone.CreateEngineInstance(true);
        }
      }
    }

    #endregion
 
    #region light info update

    /// <summary>
    /// virtual function to collect information for static light update
    /// </summary>
    /// <param name="info"></param>
    protected override void GetStaticLightInfo(StaticLightInfo info)
    {
      if (!HasModel || !HasEngineInstance())
        return;
      if (_bCastStaticShadows)
        info.RelevantOccluder.Add(this);
//      if (LightmapTiling > 1)
      if (EngineEntity.HasLightmappedMaterials())
        info.RelevantReceiver.Add(this);
    }

    #endregion

    #region Entity class

    string _entityClass;
    string _classDefTemplate;

    /// <summary>
    /// Helper fake property to backup and restore the components that are implicitly generated by the entity class
    /// </summary>
    [Browsable(false)]
    public ShapeComponentCollection OwnComponentBackups
    {
      get
      {
        if (ComponentCount == 0)
          return null;
        // backup all components with CreatedByNative flag
        ShapeComponentCollection list = null;
        foreach (ShapeComponent comp in Components)
        {
          if (!comp.CreatedByNative)
            continue;
          if (list == null)
            list = new ShapeComponentCollection();
          list.Add(comp);
        }
        return list;
      }
      set
      {
        if (value == null)
          return;
        int iIndex = 0;
        foreach (ShapeComponent comp in value)
        {
          if (Components == null)
            Components = new ShapeComponentCollection();

          // with the following trick we ensure that these components are prioritized compared to the ones that might be in there.
          // The reason is that EngineClass property is executed prior to this one which synchronizes already with new components
          Components.Insert(iIndex++, comp);
        }
        // matchup with native pointers
        SynchronizeOwnComponents();
      }
    }


    /// <summary>
    /// get or set entity class name
    /// </summary>
    [SortedCategory(CAT_SETTINGS, CATORDER_SETTINGS),
    PropertyOrder(10),
    DependentProperties(new string[] { "EntityProperties", "LinkBackups", "OwnComponentBackups" }),
    TypeConverter(typeof(EntityClassConverter )),  
    Description("The engine type of the entity"),
    RefreshProperties(System.ComponentModel.RefreshProperties.All), DefaultValue("VisBaseEntity_cl")]  //Causes a grid refresh after the class changes because we change the entity properties
    public string EntityClass
    {
      get
      {
        return _entityClass;
      }
      set 
      {
        string _oldclass = _entityClass;
        if ( _entityClass != value )
        {
          _entityClass = value;
          UpdateEntityProperties(true);

          if (HasEngineInstance())
          {
            ReCreateEngineInstance(false);
          }
        }
      }
    }

    [PrefabResolveFilename]
    [Description("Optional XML file reference that specifies the EntityProperties. If specified, it replaces the properties extracted from the entity class' VARTABLE")]
    [SortedCategory(CAT_SETTINGS, CATORDER_SETTINGS),PropertyOrder(39)]
    [EditorAttribute(typeof(FilenameEditor), typeof(UITypeEditor)), FileDialogFilter(new string[] {".xml"})]
    [DependentProperties(new string[] {"EntityProperties"})]
    public string ClassDefinitionTemplate
    {
      get {return _classDefTemplate;}
      set 
      {
        if (_classDefTemplate == value)
          return;
        _classDefTemplate = value;
        UpdateEntityProperties(true);
      }
    }


    void UpdateEntityProperties(bool bCreateInstance)
    {
      DynamicPropertyManager manager = EditorManager.EngineManager.EntityClassManager;
      // definition from XML or entity class?
      string relevantTemplate = _classDefTemplate;
      if (string.IsNullOrEmpty(relevantTemplate))
        relevantTemplate = _entityClass;
      else
      {
        // XML template not yet in the dynamic type manager?
        if (!manager.Exists(_classDefTemplate))
        {
          DynamicPropertyCollectionType newType = manager.CreateNewType(_classDefTemplate, DynamicPropertyCollectionType.DynamicPropertyCollectionFlags_e.None);
          LoadPropertyTemplateFile(_classDefTemplate, newType);
          manager.AddCollectionType(newType);
        }
      }

      if (!bCreateInstance)
        return;

      // recreate the dynamic properties
      try
      {
        _entityProperties = manager.CreateCollection(this, relevantTemplate);
      }
      catch
      {
        EditorManager.ShowMessageBox("Could not create specific entity properties.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
      }
    }

    void LoadPropertyTemplateFile(string xmlFilename, DynamicPropertyCollectionType target)
    {
      xmlFilename = EditorManager.Project.MakeAbsolute(xmlFilename);
      IShapeFactory factory = EditorManager.ShapeFactory;
      try
      {
        XmlDocument doc = new XmlDocument();
        XmlTextReader xmlReader = new XmlTextReader(xmlFilename);
        xmlReader.WhitespaceHandling = WhitespaceHandling.None;
        doc.Load(xmlReader);
        xmlReader.Close();

        IEnumerator nodes = doc.DocumentElement.GetEnumerator();
        while (nodes.MoveNext())
        {
          XmlElement node = nodes.Current as XmlElement;
          if (node == null)
            continue;
          if (node.Name != "property")
            continue;
          string propName = node.GetAttribute("name");
          string typeName = node.GetAttribute("type");
          string defaultStr = node.GetAttribute("default");
          string description = node.GetAttribute("description");
          if (string.IsNullOrEmpty(propName) || string.IsNullOrEmpty(typeName))
            continue;
          Type t;
          // some convenience type conversions
          if (string.Compare(typeName, "float", true) == 0) t = typeof(float);
          else if (string.Compare(typeName, "int", true) == 0) t = typeof(int);
          else if (string.Compare(typeName, "string", true) == 0) t = typeof(string);
          else t = SerializationHelper.GetType(typeName);
          if (t==null)
            continue;
          object defaultVal = SerializationHelper.GetObjectFromStringData(t, defaultStr);
          DynamicPropertyType newType = new DynamicPropertyType(propName, t, defaultVal, description, null);
          target.Add(newType);
        }
      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex);

      }
    }

    #endregion 

    #region EntityProperties

    /// <summary>
    /// Collection of dynamic entity properties
    /// </summary>
    private DynamicPropertyCollection _entityProperties;

    /// <summary>
    /// Old entity property collection, deserialized from old scene files.
    /// Temporary variable that is used to migrate the old data in the OnDeserialization event.
    /// </summary>
    private CloneableBase _oldEntityProperties;

    /// <summary>
    /// get or set entity properties
    /// </summary>
    [PrefabRelevant(false)]
    [SortedCategory(CAT_SETTINGS, CATORDER_SETTINGS),
    PropertyOrder(40),
    TypeConverter(typeof(DynamicPropertyConverter))]
    public DynamicPropertyCollection EntityProperties
    {
      get { return _entityProperties; }
      set { _entityProperties = value; }
    }

    /// <summary>
    /// Gets a string dictionary for all the entity properties. The key contains the name
    /// of the property whereas the value contains the engine string representation of it.
    /// </summary>
    /// <returns>String dictionary with VarName:StringValue pairs</returns>
    protected StringDictionary GetEntityPropertiesAsString()
    {
      if (_entityProperties==null)
        return null;
      return _entityProperties.GetPropertyDict(SerializationHelper.ENGINEVECTOR_SEPARATOR); // this is a new helper function
    }


    /// <summary>
    /// Gets or sets the full property string, where each property is separated by comma.
    /// This is a helper property so we can set parse the property string using the XML prefab system
    /// </summary>
    [PrefabRelevant(true)]
    [Browsable(false)]
    public string EntityPropertyString
    {
      get
      {
        if (_entityProperties == null)
          return null;
        return _entityProperties.GetPropertyParameterString(",");
      }

      set
      {
        if (_entityProperties != null && !string.IsNullOrEmpty(value))
          _entityProperties.ParseParameterString(value, ",");
        else
          _pendingEntityPropertyString = value;

      }
    }


    /// <summary>
    /// Invisible helper property to get and set the entity key
    /// </summary>
    [Browsable(false)]
    public string EntityKey
    {
      get 
      {
        return ObjectKey;
      }
      set
      {
        ObjectKey = value;
      }
    }

    #endregion

    #region Prefab handling

    string _pendingEntityPropertyString; // for prefabs

    public override void OnPostPrefabCreation(ShapeCollection allRootShapes, PrefabDesc prefab)
    {
      base.OnPostPrefabCreation(allRootShapes, prefab);
      if (_entityProperties != null)
      {
        if (!string.IsNullOrEmpty(_pendingEntityPropertyString))
          _entityProperties.ParseParameterString(_pendingEntityPropertyString, ",");
        _entityProperties.OnPostPrefabCreation(allRootShapes, prefab);
      }
      _pendingEntityPropertyString = null;
    }


    #endregion

    #region More entity properties

    FlagsInt32_e _iVisibleBitmask = FlagsInt32_e.All;
    FlagsInt32_e _iLightInfluenceBitmask = FlagsInt32_e.Bit0;
    bool _bCastDynamicShadows = false;
    bool _bCastStaticShadows = false;
    Vector3F _vLightGridOfs = Vector3F.Zero;
    //float _fLightmapTiling = 0.0f;
    Color _ambientColor = Color.Black;
    float _farClipDistance = 0.0f;
    float _nearClipDistance = 0.0f;
    sbyte _renderOrderPriority = 0;

    static bool _bShowLightgridOfsWidget = false;

    /// <summary>
    /// Override this property to add support for it. Note that loading/saving is handled through base class already
    /// </summary>
    eNavMeshUsage _navMeshUsage = eNavMeshUsage.IncludeInNavMesh;
    [Description("Determines how shape is used during nav mesh building")]
    public override eNavMeshUsage NavMeshUsage
    {
      get { return _navMeshUsage; }
      set { _navMeshUsage = value; }
    }

    /// <summary>
    /// Entity visibility bitmask that determines the visible status in the rendering contexts
    /// </summary>
    [SortedCategory(CAT_SETTINGS, CATORDER_SETTINGS), PropertyOrder(35)]
    [Description("Entity visibility bitmask that determines the visible status in the rendering contexts"), DefaultValue(FlagsInt32_e.All)]
    public FlagsInt32_e VisibleBitmask
    {
      get {return _iVisibleBitmask;}
      set
      {
        _iVisibleBitmask = value;
        if (_engineInstance!=null)
          EngineEntity.SetVisibleBitmask((uint)_iVisibleBitmask);
      }
    }

    /// <summary>
    /// Entity near clip distance in world units. Set to 0.0 for no distance clipping
    /// </summary>
    [SortedCategory(CAT_SETTINGS, CATORDER_SETTINGS), PropertyOrder(36)]
    [Description("Entity near clip distance in world units. Set to 0.0 for no near clipping"), DefaultValue(0f)]
    public float NearClipDistance
    {
      get { return _nearClipDistance; }
      set
      {
        _nearClipDistance = value;
        if (_nearClipDistance < 0.0f)
          _nearClipDistance = 0.0f;
        if (_engineInstance != null)
          EngineEntity.SetClipDistances(_nearClipDistance, _farClipDistance);
        InvalidateZoneBoundingBox();
      }
    }

    /// <summary>
    /// Entity far clip distance in world units. Set to 0.0 for no distance clipping
    /// </summary>
    [SortedCategory(CAT_SETTINGS, CATORDER_SETTINGS), PropertyOrder(37)]
    [Description("Entity far clip distance in world units. Set to 0.0 for no distance clipping"), DefaultValue(0f)]
    public float FarClipDistance
    {
      get { return _farClipDistance; }
      set
      {
        _farClipDistance = value;
        if (_farClipDistance < 0.0f)
          _farClipDistance = 0.0f;
        if (_engineInstance != null)
          EngineEntity.SetClipDistances(_nearClipDistance, _farClipDistance);
        InvalidateZoneBoundingBox();
      }
    }

   


    /// <summary>
    /// Overridden Shapebase function to get the entity's far clip distance
    /// </summary>
    /// <returns></returns>
    public override float GetFarClipDistance()
    {
      return _farClipDistance > 0.0f ? _farClipDistance : -1.0f;
    }


    /// <summary>
    /// Priority value for rendering. Entities with a priority priority will be rendered earlier than others independent of camera distance
    /// </summary>
    [SortedCategory(CAT_SETTINGS, CATORDER_SETTINGS), PropertyOrder(38)]
    [Description("Priority value for rendering. Entities with a high priority will be rendered earlier than others independent of camera distance"), DefaultValue(typeof(sbyte),"0")]
    public sbyte RenderOrderPriority
    {
      get { return _renderOrderPriority; }
      set
      {
        _renderOrderPriority = value;
        if (_engineInstance != null)
          EngineEntity.SetPrimarySortingKey(_renderOrderPriority);
      }
    }

    /// <summary>
    /// This flag determines whether the entity should cast dynamic shadows from dynamic lights
    /// or static lights with shadowmaps enabled.
    /// </summary>
    [SortedCategory(CAT_LIGHTANDSHADOW, CATORDER_LIGHTANDSHADOW), PropertyOrder(1),
    Description("This flag determines whether the entity should cast dynamic shadows from dynamic lights or static lights with shadowmaps enabled"), DefaultValue(false)]
    public bool CastDynamicShadows
    {
      get {return _bCastDynamicShadows;}
      set 
      {
        _bCastDynamicShadows=value;
        if (_engineInstance != null)
          EngineEntity.SetCastDynamicShadows(_bCastDynamicShadows);
      }
    }

    /// <summary>
    /// This flag determines whether the entity should be considered for static light computations (light maps)
    /// </summary>
    [SortedCategory(CAT_LIGHTANDSHADOW, CATORDER_LIGHTANDSHADOW), PropertyOrder(2),
    Description("This flag determines whether the entity should be considered for static light computations (light maps)"), DefaultValue(false)]
    public bool CastStaticShadows
    {
      get {return _bCastStaticShadows;}
      set 
      {
        _bCastStaticShadows=value; // the engine instance does not care
        if (_engineInstance != null)
          EngineEntity.SetCastStaticShadows(_bCastStaticShadows);
      }
    }

    /// <summary>
    /// Bitflag filter mask that determines which dynamic lights influence this entity. See also dynamic light's ObjectInfluenceMask property
    /// </summary>
    [SortedCategory(CAT_LIGHTANDSHADOW, CATORDER_LIGHTANDSHADOW), PropertyOrder(4)]
    [Description("Bitflag filter mask that determines which dynamic lights influence this entity. See also dynamic light's ObjectInfluenceMask property"), DefaultValue(FlagsInt32_e.Bit0)]
    public FlagsInt32_e LightInfluenceBitmask
    {
      get {return _iLightInfluenceBitmask;}
      set
      {
        _iLightInfluenceBitmask = value;
        if (_engineInstance!=null)
          EngineEntity.SetLightInfluenceBitmask((uint)_iLightInfluenceBitmask);
      }
    }

    /// <summary>
    /// Defines the relative position offset that is used to gather the light grid sample
    /// </summary>
    [SortedCategory(CAT_LIGHTANDSHADOW, CATORDER_LIGHTANDSHADOW), PropertyOrder(3),
    Description("Defines the relative position offset that is used to gather the light grid sample")]
    [BrowsableAttribute(false)]
    public Vector3F LightGridSampleOfs
    {
      get {return _vLightGridOfs;}
      set 
      {
        _vLightGridOfs=value;
        if (_engineInstance != null)
          EngineEntity.SetLightgridSamplePosition(_vLightGridOfs.X, _vLightGridOfs.Y, _vLightGridOfs.Z);
      }
    }


    /// <summary>
    /// Deprecated
    /// </summary>
    [Browsable(false)]
    [SortedCategory(CAT_LIGHTANDSHADOW, CATORDER_LIGHTANDSHADOW), PropertyOrder(5)]
    [Description("Deprecated")]
    [TypeConverter(typeof(IntListTypeConverter)),AllowedIntValues(new int[] {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16})]
    public int LightmapTiling
    {
      get {return 0;}
      set
      {
      }
    }

    /// <summary>
    /// Defines the individual entity ambient lighting color
    /// </summary>
    [SortedCategory(CAT_LIGHTANDSHADOW, CATORDER_LIGHTANDSHADOW), PropertyOrder(6)]
    [Description("Defines the individual entity ambient lighting color")]
    [EditorAttribute(typeof(StandardColorDropDownEditor), typeof(UITypeEditor))]
    [PropertyLiveUpdate("AmbientColorLiveUpdate"), DefaultValue(typeof(Color), "0,0,0")]
    public Color AmbientColor
    {
      get {return _ambientColor;}
      set
      {
        _ambientColor = value;
        if (_engineInstance != null)
          EngineEntity.SetAmbientColor(VisionColors.Get(_ambientColor));
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
        if (_engineInstance != null)
          EngineEntity.SetAmbientColor(VisionColors.Get(col));
        EditorManager.ActiveView.UpdateView(false);
      }
    }

    /// <summary>
    /// If enabled, a 3D widget will be rendered that allows to modify the offset in the 3D view. This property wraps around a static bool so it affects all entities
    /// </summary>
    [SortedCategory(CAT_LIGHTGRID, CATORDER_LIGHTGRID), PropertyOrder(1)]
    [Description("If enabled, a 3D widget will be rendered that allows to modify the offset in the 3D view"), DefaultValue(false)]
    public bool ShowLightgridWidget
    {
      get {return _bShowLightgridOfsWidget;}
      set {_bShowLightgridOfsWidget=value;}
    }

    /// <summary>
    /// Gets or sets the relative x position offset that is used to gather the light grid sample
    /// </summary>
    [SortedCategory(CAT_LIGHTGRID, CATORDER_LIGHTGRID), PropertyOrder(2)]
    [Description("Defines the relative x position offset that is used to gather the light grid sample"), DefaultValue(0f)]
    public float SampleOfsX
    {
      get {return _vLightGridOfs.X;}
      set 
      {
        _vLightGridOfs.X=value;
        if (_engineInstance != null)
          EngineEntity.SetLightgridSamplePosition(_vLightGridOfs.X, _vLightGridOfs.Y, _vLightGridOfs.Z);
      }
    }

    /// <summary>
    /// Gets or sets the relative y position offset that is used to gather the light grid sample
    /// </summary>
    [SortedCategory(CAT_LIGHTGRID, CATORDER_LIGHTGRID), PropertyOrder(3)]
    [Description("Defines the relative y position offset that is used to gather the light grid sample"), DefaultValue(0f)]
    public float SampleOfsY
    {
      get {return _vLightGridOfs.Y;}
      set 
      {
        _vLightGridOfs.Y=value;
        if (_engineInstance != null)
          EngineEntity.SetLightgridSamplePosition(_vLightGridOfs.X, _vLightGridOfs.Y, _vLightGridOfs.Z);
      }
    }

    /// <summary>
    /// Gets or sets the relative z position offset that is used to gather the light grid sample
    /// </summary>
    [SortedCategory(CAT_LIGHTGRID, CATORDER_LIGHTGRID), PropertyOrder(4)]
    [Description("Defines the relative z position offset that is used to gather the light grid sample"), DefaultValue(0f)]
    public float SampleOfsZ
    {
      get {return _vLightGridOfs.Z;}
      set 
      {
        _vLightGridOfs.Z=value;
        if (_engineInstance != null)
          EngineEntity.SetLightgridSamplePosition(_vLightGridOfs.X, _vLightGridOfs.Y, _vLightGridOfs.Z);
      }
    }
    
    #endregion

    #region linking

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="src"></param>
    /// <param name="target"></param>
    /// <returns></returns>
    public override bool CanLink(ShapeLink src, ShapeLink target)
    {
      if (base.CanLink (src, target))
        return true;

      // see if we can link to the custom sources/targets provided by the native entity class
      if (_engineInstance!=null)
      {
        IEngineInstanceObject3D targetInst = null;
        int iLinkIndex = -1;
        string otherName = null;
        if (src.OwnerShape == this)
        {
          if (src is LinkSourceObject3D)
            iLinkIndex = ((LinkSourceObject3D)src).iIndex;
          else if (src is LinkBiDirectionalObject3D)
            iLinkIndex = ((LinkBiDirectionalObject3D)src).iIndex;
          targetInst = target.OwnerShape._engineInstance as IEngineInstanceObject3D;
          otherName = target.PrimaryStringId;
        }
        else if (target.OwnerShape==this)
        {
          if (target is LinkTargetObject3D) 
            iLinkIndex = ((LinkTargetObject3D)target).iIndex;
          else if (target is LinkBiDirectionalObject3D)
            iLinkIndex = ((LinkBiDirectionalObject3D)target).iIndex;
          targetInst = src.OwnerShape._engineInstance as IEngineInstanceObject3D;
          otherName = src.PrimaryStringId;
        }
        if (iLinkIndex >= 0 && targetInst != null)
          if (EngineEntity.CanLink(iLinkIndex, targetInst, otherName))
            return true;
      }

      return false;
    }

    void SetLinkStatus(ShapeLink src, ShapeLink target, bool bStatus)
    {
      if (_engineInstance==null)
        return;
      // see if we can (un-)link to the custom sources/targets provided by the native entity class
      IEngineInstanceObject3D targetInst = null;
      int iLinkIndex = -1;
      string otherName = null;

      if (src.OwnerShape==this)
      {
        if (src is LinkSourceObject3D)
          iLinkIndex = ((LinkSourceObject3D)src).iIndex;
        else if (src is LinkBiDirectionalObject3D)
          iLinkIndex = ((LinkBiDirectionalObject3D)src).iIndex;
        targetInst = target.OwnerShape._engineInstance as IEngineInstanceObject3D;
        otherName = target.PrimaryStringId;
      }
      else if (target.OwnerShape==this)
      {
        if (target is LinkTargetObject3D)
          iLinkIndex = ((LinkTargetObject3D)target).iIndex;
        else if (target is LinkBiDirectionalObject3D)
          iLinkIndex = ((LinkBiDirectionalObject3D)target).iIndex;
        targetInst = src.OwnerShape._engineInstance as IEngineInstanceObject3D;
        otherName = src.PrimaryStringId;
      }

      if (iLinkIndex >= 0 && targetInst != null)
        EngineEntity.OnLink(iLinkIndex, targetInst, otherName, bStatus);
      
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="src"></param>
    /// <param name="target"></param>
    public override void OnLink(ShapeLink src, ShapeLink target)
    {
      base.OnLink (src, target); // handles trigger linking
      SetLinkStatus(src, target, true);
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="src"></param>
    /// <param name="target"></param>
    public override void OnUnlink(ShapeLink src, ShapeLink target)
    {
      base.OnUnlink(src, target); // handles trigger linking
      SetLinkStatus(src, target, false);
    }

    #endregion

    #region Snap Points

    List<ShapeSnapPoint> _snappoints = null;

    public override List<ShapeSnapPoint> GetSnapPointList()
    {
      if (object.ReferenceEquals(_snappoints, StaticMeshShape.SNAPPOINTLIST_CACHE_INVALID)) // cached result
      {
        LoadSnapPointFile();
      }
      return _snappoints;
    }


    public void LoadSnapPointFile()
    {
      _snappoints = base.GetSnapPointList();
      if (!HasModel)
        return;

      string filename = EditorManager.Project.MakeAbsolute(_modelFile + @"_data\snappoints.xml");
      if (!File.Exists(filename))
        return;
      List<ShapeSnapPoint> list = ShapeSnapPoint.LoadSnapPointConfigurationFile(filename, this);
      if (list != null)
      {
        if (_snappoints == null)
          _snappoints = list;
        else
          _snappoints.AddRange(list);
      }
    }

    #endregion


    #region Relevant operations

    private static string RO_CONVERT_TO_STATIC_MESH = @"Convert to\Static Mesh Shape";
    private static string RO_OPEN_IN_MV = @"Open in vModelViewer";

    /// <summary>
    /// Gets a list of relevant operations for entity shape
    /// </summary>
    public override ArrayList RelevantOperations
    {
      get
      {
        string sPath = ModelFile;

        ArrayList op = base.RelevantOperations;
        if (op==null)
          op = new ArrayList(5);
        op.Add(@"Set lightgrid offset\pivot");
        op.Add(@"Set lightgrid offset\box center");
        op.Add(@"Set lightgrid offset\box bottom");
        if (!string.IsNullOrEmpty(sPath))
        {
          if (ShaderAssignment!=null)
            op.Add(@"Shaders\Model surfaces");
          op.Add(RO_CONVERT_TO_STATIC_MESH);
          if (string.Compare(Path.GetExtension(sPath), ".Model", true) == 0)
            op.Add(RO_OPEN_IN_MV);
        }
        return op;
      }
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="name"></param>
    /// <param name="otherShape"></param>
    /// <returns></returns>
    public override bool CanGroupRelevantOperation(string name, ShapeBase otherShape)
    {
      if (name==@"Shaders\Model surfaces")
      {
        if (!(otherShape is EntityShape))
          return false;
        return string.Compare(((EntityShape)otherShape).ModelFile,this.ModelFile)==0;
      }
      if (name == RO_OPEN_IN_MV)
        return false;

      return base.CanGroupRelevantOperation (name, otherShape);
    }

    public override Image GetImageForRelevantOperation(string operationName)
    {
      if (operationName==RO_CONVERT_TO_STATIC_MESH)
      {
        // return image of replacement shape here
        int iIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(StaticMeshShape.ICON_NAME, Color.Magenta);
        if (iIndex>=0)
          return EditorManager.GUI.ShapeTreeImages.ImageList.Images[iIndex];
      }
      return base.GetImageForRelevantOperation(operationName);
    }


    /// <summary>
    /// Overridden function that performs a "relevant operation"
    /// </summary>
    /// <param name="name"></param>
    /// <param name="iShapeIndex"></param>
    /// <param name="iShapeCount"></param>
    public override void PerformRelevantOperation(string name, int iShapeIndex, int iShapeCount)
    {
      base.PerformRelevantOperation (name, iShapeIndex, iShapeCount);
      
      if (name==@"Set lightgrid offset\pivot" || name==@"Set lightgrid offset\box center" || name==@"Set lightgrid offset\box bottom")
      {
        // open a group action
        if (iShapeIndex==0) EditorManager.Actions.StartGroup("light grid : "+name);

        Vector3F vNewOfs = Vector3F.Zero;
        if (name == @"Set lightgrid offset\pivot") vNewOfs = Vector3F.Zero;
        else
        if (name==@"Set lightgrid offset\box center")
        {
          BoundingBox bbox = LocalScaledBoundingBox;
          if (bbox.Valid)
            vNewOfs = bbox.Center;
          }
          else
        if (name==@"Set lightgrid offset\box bottom")
        {
          BoundingBox bbox = LocalScaledBoundingBox;
          if (bbox.Valid)
          {
            vNewOfs = bbox.Center;
            vNewOfs.Z = bbox.vMin.Z;
          }
        }

        // set the offset
        EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"LightGridSampleOfs",vNewOfs));

        // close group action
        if (iShapeIndex==iShapeCount-1) EditorManager.Actions.EndGroup();
        return;
      }
      if (name==@"Shaders\Model surfaces")
      {
        if (ShaderAssignment!=null)
          ShaderAssignment(this,new ShaderAssignmentEventArgs(this));
        return;
      }
      if (name == RO_CONVERT_TO_STATIC_MESH)
      {
        if (iShapeCount > 1 && iShapeIndex == 0)
          EditorManager.Actions.StartGroup("Convert entities to meshes");

        // create a shape with the same relevant properties
        StaticMeshShape replacement = new StaticMeshShape(this.ShapeName);

        // remap all public browsable properties (except uniform scaling as this overwrites current scaling)
        StringCollection exclude = new StringCollection();
        exclude.Add("UniformScaling");
        SerializationHelper.CopyInstanceProperties(replacement, this, true,true,exclude);

        string sVMeshPath = this.ModelFile;

        // remap file name of the mesh
        if (!string.IsNullOrEmpty(sVMeshPath))
          replacement.MeshFileName = Path.ChangeExtension(sVMeshPath, ".vmesh");

        EditorManager.Actions.Add(new ReplaceShapeAction(this, replacement));

        if (iShapeCount > 1 && iShapeIndex == iShapeCount - 1)
          EditorManager.Actions.EndGroup();

        return;
      }
      if (name == RO_OPEN_IN_MV)
      {
        OpenModelInModelViewer();
        return;
      }
    }


    void OpenModelInModelViewer()
    {
      string sModelPath = ModelFile;

      if (string.IsNullOrEmpty(sModelPath) || string.Compare(Path.GetExtension(sModelPath), ".Model", true) != 0)
        return;

      try
      {
        Process vMV = new Process();
        vMV.StartInfo.FileName = EditorManager.Project.MakeAbsolute(sModelPath);
        vMV.StartInfo.UseShellExecute = true;
        vMV.Start();
      }
      catch (Exception ex)
      {
        EditorManager.ShowMessageBox("Model viewer failed to start.\n\nDetailed:\n"+ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        EditorManager.DumpException(ex);
      }

    }

    #endregion

    #region Shader Assignment


    /// <summary>
    /// Nested event arg class for notifying the shader assignment editor
    /// </summary>
    public class ShaderAssignmentEventArgs : EventArgs
    {
      /// <summary>
      /// Constructor
      /// </summary>
      /// <param name="_sender"></param>
      public ShaderAssignmentEventArgs(EntityShape _sender)
      {
        MaterialProvider = _sender.MaterialProvider;
      }

      /// <summary>
      /// Public model info (sender)
      /// </summary>
      public IMaterialProvider MaterialProvider;
    }

    /// <summary>
    /// Delegate declaration
    /// </summary>
    public delegate void ShaderAssignmentEventHandler(object sender, ShaderAssignmentEventArgs e);

    /// <summary>
    /// This event gets triggered if something happed that might or should affect the particle editor
    /// which listens to this event
    /// </summary>
    static public event ShaderAssignmentEventHandler ShaderAssignment;


    #endregion

    #region Selection and Hotspots

    HotSpot3D _hotSpotLightGridOfs = null;

    /// <summary>
    /// Overridden selection function. Adds the hotspots
    /// </summary>
    public override void OnSelected()
    {
      base.OnSelected ();
      Debug.Assert(_hotSpotLightGridOfs==null);
      _hotSpotLightGridOfs = new HotSpot3D(this,16.0f);
      _hotSpotLightGridOfs.ToolTipText = "light grid offset";
      _hotSpotLightGridOfs.StartPosition = LightGridSampleOfs;
      EditorManager.ActiveView.HotSpots.Add(_hotSpotLightGridOfs);
      WantsNativeRenderHookCallback = true;
    }

    /// <summary>
    /// Overridden un-selection function. Removes the hotspots
    /// </summary>
    public override void OnUnSelected()
    {
      base.OnUnSelected ();
      Debug.Assert(_hotSpotLightGridOfs!=null);
      EditorManager.ActiveView.HotSpots.Remove(_hotSpotLightGridOfs);
      _hotSpotLightGridOfs = null;
      WantsNativeRenderHookCallback = false;
    }


    /// <summary>
    /// Overridden hot spot handle function
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragBegin(HotSpotBase hotSpot, VisionViewBase view)
    {
      base.OnHotSpotDragBegin (hotSpot, view);
    }

    /// <summary>
    /// Overridden hot spot handle function
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    /// <param name="fDeltaX"></param>
    /// <param name="fDeltaY"></param>
    public override void OnHotSpotDrag(HotSpotBase hotSpot, VisionViewBase view, float fDeltaX, float fDeltaY)
    {
      base.OnHotSpotDrag (hotSpot, view, fDeltaX, fDeltaY);
    }

    /// <summary>
    /// Overridden hot spot handle function
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragEnd(HotSpotBase hotSpot, VisionViewBase view)
    {
      base.OnHotSpotDragEnd (hotSpot, view);
      if (hotSpot==_hotSpotLightGridOfs)
      {
        Vector3F vNewPos = _hotSpotLightGridOfs.CurrentPosition;
        LightGridSampleOfs = _hotSpotLightGridOfs.StartPosition; // set old value for the action
        EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"LightGridSampleOfs",vNewPos)); // send an action which sets the property from old value to new one
      }
    }

    /// <summary>
    /// Overridden hot spot handle function
    /// </summary>
    /// <param name="hotSpot"></param>
    public override void OnHotSpotEvaluatePosition(HotSpotBase hotSpot)
    {
      base.OnHotSpotEvaluatePosition (hotSpot);

      if (hotSpot==_hotSpotLightGridOfs)
      {
        _hotSpotLightGridOfs.Visible = this.ShowLightgridWidget;

        if (_hotSpotLightGridOfs.IsDragging) // update while dragging
          LightGridSampleOfs = _hotSpotLightGridOfs.CurrentPosition;
        else
          _hotSpotLightGridOfs.StartPosition = LightGridSampleOfs;
      }
    }


    #endregion

    #region ISerializable/IDeserializationCallback Members


    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected EntityShape(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      _entityClass = info.GetString( "_entityClass" );
      _modelFile   = info.GetString( "_modelFile" );

      // load entity properties
      object entityProps = (object) info.GetValue( "_entityProperties", typeof(object) );
      _entityProperties = entityProps as DynamicPropertyCollection;

      // Store the old property collection data in a member variable (when loading old scene files).
      // We'll migrate the property values in the OnDeserialization when all nested properties are fully loaded.
      if (_entityProperties == null)
      {
        _oldEntityProperties = entityProps as CloneableBase;
        UpdateEntityProperties(true);
        //_entityProperties = EditorManager.EngineManager.EntityClassManager.CreateCollection(this,_entityClass);
      }

      if (SerializationHelper.HasElement(info,"_bCastStaticShadows"))
        _bCastStaticShadows = info.GetBoolean("_bCastStaticShadows");
      if (SerializationHelper.HasElement(info,"_bCastDynamicShadows"))
        _bCastDynamicShadows = info.GetBoolean("_bCastDynamicShadows");
      if (SerializationHelper.HasElement(info,"_vLightGridOfs"))
        _vLightGridOfs = (Vector3F)info.GetValue("_vLightGridOfs", typeof(Vector3F));
      if (SerializationHelper.HasElement(info,"_iVisibleBitmask"))
        _iVisibleBitmask = (FlagsInt32_e)info.GetValue("_iVisibleBitmask", typeof(FlagsInt32_e));
      if (SerializationHelper.HasElement(info,"_iLightInfluenceBitmask"))
        _iLightInfluenceBitmask = (FlagsInt32_e)info.GetValue("_iLightInfluenceBitmask", typeof(FlagsInt32_e));
//      if (SerializationHelper.HasElement(info,"_fLightmapTiling"))
//        _fLightmapTiling = info.GetSingle("_fLightmapTiling");      
      if (SerializationHelper.HasElement(info,"_ambientColor"))
        _ambientColor = (Color)info.GetValue("_ambientColor", typeof(Color));
      if (SerializationHelper.HasElement(info, "_farClipDistance"))
        _farClipDistance = info.GetSingle("_farClipDistance");
      if (SerializationHelper.HasElement(info, "_nearClipDistance"))
        _nearClipDistance = info.GetSingle("_nearClipDistance");
      if (SerializationHelper.HasElement(info, "_renderOrderPriority"))
        _renderOrderPriority = info.GetSByte("_renderOrderPriority");
      if (SerializationHelper.HasElement(info, "_classDefTemplate"))
        _classDefTemplate = info.GetString("_classDefTemplate");
      if (SerializationHelper.HasElement(info, "_copySource"))
        _copySource = (EntityShape)info.GetValue("_copySource", typeof(EntityShape));
      if (SerializationHelper.HasElement(info, "_customMaterialSetName"))
        _customMaterialSetName = info.GetString("_customMaterialSetName");
       
    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context) 
    {
      base.GetObjectData(info,context);

      info.AddValue("_entityClass", _entityClass);
      info.AddValue("_entityProperties", _entityProperties);
      info.AddValue("_modelFile", _modelFile);
      info.AddValue("_bCastStaticShadows",_bCastStaticShadows);
      info.AddValue("_bCastDynamicShadows",_bCastDynamicShadows);
      info.AddValue("_vLightGridOfs",_vLightGridOfs);
      info.AddValue("_iVisibleBitmask",_iVisibleBitmask);
      info.AddValue("_iLightInfluenceBitmask", _iLightInfluenceBitmask);
      //info.AddValue("_fLightmapTiling",_fLightmapTiling);
      info.AddValue("_ambientColor",_ambientColor);
      info.AddValue("_farClipDistance", _farClipDistance);
      info.AddValue("_nearClipDistance", _nearClipDistance);
      info.AddValue("_renderOrderPriority", _renderOrderPriority);

      if (!string.IsNullOrEmpty(_classDefTemplate))
        info.AddValue("_classDefTemplate", _classDefTemplate);

      if (_copySource != null && _copySource.ParentLayer == this.ParentLayer)
        info.AddValue("_copySource", _copySource);
      if (!string.IsNullOrEmpty(_customMaterialSetName))
        info.AddValue("_customMaterialSetName", _customMaterialSetName);
    }

    /// <summary>
    /// Serialization helper function
    /// </summary>
    public override void OnDeserialization()
    {
      base.OnDeserialization();

      UpdateEntityProperties(false); // create collection type for XML files, no instance

      // Migrate the old property values to the new collection (when loading old scene files)
      if (_oldEntityProperties != null)
      {
        _entityProperties.CopyValuesFromOldCollection(_oldEntityProperties);
      }
      else
      {
        // migrate entity key to new style. Do this before migrating the collection as the key won't be in tehre anymore
        if (_entityProperties != null && _entityProperties.GetProperty("entityKey") != null)
        {
          _objectKey = _entityProperties.GetPropertyValue("entityKey", false) as string;
        }

      // recreate entity properties with latest entity class manager (take over old values)
        _entityProperties = EditorManager.EngineManager.EntityClassManager.CreateMigratedCollection(_entityProperties);
        if (_entityProperties != null)
          _entityProperties.Owner = this;
      }

    }

    #endregion

    #region Scene Event Handlers
    

    public override void OnSceneEvent(SceneEventArgs args)
    {
      base.OnSceneEvent(args);
      if (args.action == SceneEventArgs.Action.UpdateNativeVariable)
      {
        UpdateVariableEventArgs e = (UpdateVariableEventArgs)args;
        if (e.Success || !Modifiable || !HasEngineInstance() || !e.IsSubProperty || EntityProperties == null || !EngineEntity.IsNativeObject(e.NativeObject))
          return;
        DynamicProperty prop = EntityProperties.GetPropertyByDisplayName(e.VariableName);
        if (prop == null)
          return;
        object newVal = SerializationHelper.GetObjectFromStringData(prop.PropertyType.NativeType, e.VariableValue);
        IAction action = SetPropertyAction.CreateSetPropertyAction(EntityProperties, e.VariableName, newVal);
        if (e.SetViaAction)
          EditorManager.Actions.Add(action);
        else
        {
          action.Do();
          // Make sure that the UI gets informed about this change, since we bypass the undo/redo system
          IScene.SendPropertyChangedEvent(new PropertyChangedArgs(this, e.VariableName));
        }
        e.Success = true;
        return;
      }
    }


    #endregion 

    #region Plugin information

    // lookup entity class/info pair
    public static Dictionary<string,EditorPluginInfo>  _cachedEntitySourcePluginInfo = new Dictionary<string,EditorPluginInfo>();

    /// <summary>
    /// Overridden function.
    /// </summary>
    /// <returns></returns>
    public override EditorPluginInfo GetPluginInformation()
    {

      // Specifically for entities this version is a bit more complicated 
      // as the underlying entity class can be from a different plugin than this
      if (_cachedEntitySourcePluginInfo.ContainsKey(_entityClass))
        return _cachedEntitySourcePluginInfo[_entityClass];

      EditorPluginInfo newInfo = new EditorPluginInfo();
      _cachedEntitySourcePluginInfo.Add(_entityClass, newInfo);
      StringCollection uniqueNames = new StringCollection();
      EditorManager.EngineManager.TypeManager_GetClassPluginNames(_entityClass, uniqueNames);
      newInfo.NativePluginNames = new string[uniqueNames.Count];
      uniqueNames.CopyTo(newInfo.NativePluginNames,0);
      return newInfo;
    }

    #endregion


    #region IStandardValueProvider Members

    /// <summary>
    /// Gathers animation names for the "Animation" key
    /// </summary>
    /// <param name="key"></param>
    /// <returns></returns>
    public TypeConverter.StandardValuesCollection GetStandardValues(string key)
    {
      if (!HasEngineInstance())
        return null;
      StringCollection animNames = new StringCollection();
      EngineEntity.GetStandardValues(animNames,key);
      return new TypeConverter.StandardValuesCollection(animNames);
    }

    #endregion

    #region ISetPropertyActionProvider Members

    public IAction GetSetPropertyAction(string propertyName, object newValue)
    {
      if (propertyName == "ModelFile")
      {
        if (!HasExposedBones)
          return null;

        GroupAction actions = new GroupAction("Change Model");

        CollectModelChangedActions(newValue as string, actions);

        // add the change model action itself
        actions.Add(SetPropertyAction.CreateSetPropertyAction(this,propertyName,newValue));

        return actions;
      }

      if (propertyName == "ExposedBones")
      {
        GroupAction actions = new GroupAction("Change Exposed Bones");

        _cachedBoneList = newValue as List<BoneProxyShape>;
        foreach (ShapeBase child in ChildCollection)
          if ((child is BoneProxyShape) && (_cachedBoneList==null || !_cachedBoneList.Contains((BoneProxyShape)child)))
            RemoveBone(actions,(BoneProxyShape)child);

        _cachedBoneList = null; // re-evaluate

        return actions;
      }

      return null;
    }

    #endregion

    #region IPropertyValueChangingCallback Members

    public void OnPropertyValueChanging(object Instance, PropertyDescriptor propertyDesc, object newValue)
    {
      if (object.ReferenceEquals(Instance, _entityProperties) && HasEngineInstance())
      {
        EngineEntity.SendValueChangingMessage(propertyDesc.Name, newValue.ToString());
      }
    }

    #endregion

    #region IShapeReferenceOwner Members

    void UpdateShapeReference(ShapeReference shaperef)
    {
      if (!HasEngineInstance() || _entityProperties == null)
        return;
      foreach (DynamicProperty prop in _entityProperties.Properties)
        if (object.ReferenceEquals(prop.Value, shaperef))
        {
          string varValue = SerializationHelper.GetEngineStringFromObject(prop.Value);
          EngineEntity.SetVariable(prop.PropertyType.Name, varValue);
        }
    }

    public void OnTargetReferenceRemoved(ShapeReference shaperef)
    {
      shaperef.CachedShape = null;
      UpdateShapeReference(shaperef);
    }

    public void OnTargetReferenceReadded(ShapeReference shaperef)
    {
      UpdateShapeReference(shaperef);
    }

    #endregion
  } 

 
  #region Entity Creator Plugin

  class EntityCreator : CSharpFramework.IShapeCreatorPlugin
  {
    public EntityCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\Entity.bmp"), Color.Magenta);
      CategoryIconIndex = IconIndex;
    }

    public override string GetPluginName()
    {
      return "Entity";
    }

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
      return "Adds an entity instance to the scene.\n"+
        "An entity is a programmable scene object with an optional .model file.\n"+
        "Programmers can provide entity classes via plugins that define the entity's behaviour";
    }

    public override ShapeBase CreateShapeInstance()
    {
      EntityShape entity = new EntityShape("Entity");
      entity.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
      return entity;
    }
  }

  #endregion

  #region BoneProxyShape

  [Serializable]
  public class BoneProxyShape : Shape3D, ISetPropertyActionProvider
  {
    public BoneProxyShape(string name)
      : base(name)
    {
      _boneName = name;
    }

    string _boneName;
    public bool Tagged = false;

    [SortedCategory(CAT_SHAPE, CATORDER_SHAPE), PropertyOrder(20)]
    public string BoneName
    {
      get { return _boneName; }
      set { _boneName = value; }
    }

    protected override ParentingModes ParentingMode
    {
      get
      {
        return ParentingModes.NoParentTransformation;
      }
    }

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
    public virtual string ObjectKey
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


    #region Engine Instance

    public override void CreateEngineInstance(bool bCreateChildren)
    {
      _engineInstance = new EngineInstanceBoneProxy();
      this.SetEngineInstanceBaseProperties();

      base.CreateEngineInstance(bCreateChildren);
    }

    [Browsable(false)]
    public EngineInstanceBoneProxy EngineBone { get { return (EngineInstanceBoneProxy)_engineInstance; } }


    public override void SetEngineInstanceBaseProperties()
    {
      if (_engineInstance == null)
        return;
      EngineBone.AttachToEntity((EngineInstanceEntity)Parent._engineInstance, BoneName);
      EngineBone.SetObjectKey(_objectKey);
      base.AcquireEngineTransformation();
      base.SetEngineInstanceBaseProperties();
    }

    #endregion

    #region Serialization

    protected BoneProxyShape(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      _boneName = info.GetString("_boneName");
      if (SerializationHelper.HasElement(info, "_objectKey"))
        _objectKey = info.GetString("_objectKey");
    }

    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);
      info.AddValue("_boneName", _boneName);
      info.AddValue("_objectKey", _objectKey);
    }

    #endregion

    #region Shape overrides

    public override void OnAddedToScene()
    {
      base.OnAddedToScene();
      AddHint(HintFlags_e.NoScale);
      AddHint(HintFlags_e.NoUserTransform);
    }

    public override bool AllowsToSetParent(ShapeBase newParent)
    {
      return newParent is EntityShape;
    }

    public override bool CanCopyPaste
    {
      get
      {
        return false;
      }
    }
    public override bool SupportsComponents
    {
      get
      {
        return false;
      }
    }


    public override void RenderShape(VisionViewBase view, ShapeRenderMode mode)
    {
      base.RenderShape(view, mode);
      if (HasEngineInstance())
        EngineBone.RenderShape(view, mode);
    }

    public override EditorPluginInfo GetPluginInformation()
    {
      return EditorPlugin.EDITOR_PLUGIN_INFO;
    }

    public override PropertyFlags_e GetPropertyFlags(PropertyFlagsProviderInfo pd)
    {
      if (pd.Name == "BoneName")
        return PropertyFlags_e.Readonly;
      if (pd.Name == "Export" || pd.Name == "Visible")
        return PropertyFlags_e.Hidden;

      return base.GetPropertyFlags(pd);
    }

    public override object ToolTip
    {
      get
      {
        return "Bone: " + BoneName;
      }
    }


    public override void OnTransformationChanged()
    {
      base.OnTransformationChanged();
    }

    public override void OnDragEnd(ShapeDragMode mode)
    {
      base.OnDragEnd(mode);
      this.AcquireEngineTransformation(); // force back to bone position
    }

    #endregion

    #region GUI

    [Browsable(false)]
    public static string ICON_NAME
    {
      get
      {
        return Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\Bone.bmp");
      }
    }

    /// <summary>
    /// Overridden function to get the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetIconIndex()
    {
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(ICON_NAME, Color.Magenta);
    }

    #endregion

    #region static helpers

    public static BoneProxyShape FindBoneShape(List<BoneProxyShape> list, string bonename)
    {
      if (list == null)
        return null;
      foreach (BoneProxyShape shape in list)
        if (shape.BoneName == bonename)
          return shape;
      return null;
    }

    public static void SetTag(List<BoneProxyShape> list, bool bStatus)
    {
      if (list != null)
        foreach (BoneProxyShape shape in list)
          shape.Tagged = bStatus;
    }

    #endregion

    #region ISetPropertyActionProvider Members

    public IAction GetSetPropertyAction(string propertyName, object newValue)
    {
      return null;
    }

    #endregion
  }

  #endregion

  #region SkeletalBoneSelectionEditor

  public class SkeletalBoneSelectionEditor : UITypeEditor
  {
    public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
    {
      return UITypeEditorEditStyle.DropDown;
    }

    private IWindowsFormsEditorService editorService = null;
    private TreeView treeView;

    void GetSelectionRecursive(EntityShape entity, List<BoneProxyShape> list, TreeNodeCollection nodes)
    {
      if (nodes == null)
        return;
      foreach (TreeNode node in nodes)
      {
        GetSelectionRecursive(entity, list, node.Nodes);
        if (!node.Checked)
          continue;
        if (node.Tag != null) // shapes that were previously in the list, are stored in the tag value
          list.Add((BoneProxyShape)node.Tag);
        else
        {
          entity.CreateBoneProxyShape(node.Text, false);
        }
      }
    }

    public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
    {
      EntityShape entity = context.Instance as EntityShape;
      if (entity==null || !entity.HasEngineInstance())
        return base.EditValue(context, provider, value);

      if (!entity.EngineEntity.HasBones())
        return value;

		  editorService = provider.GetService(typeof(IWindowsFormsEditorService)) as IWindowsFormsEditorService;
		  if (editorService == null)
        return value;

			// Create a Treview and populate it with the bone names
      treeView = new TreeView();
      treeView.CheckBoxes = true;
      treeView.Bounds = new Rectangle(0, 0, 400, 500);
      List<BoneProxyShape> list = (List<BoneProxyShape>) value;
      EntityShape.tempBones.Clear();
      entity.EngineEntity.GetBones(EntityShape.tempBones);
      foreach (SkeletalBoneDesc boneDesc in EntityShape.tempBones)
      {
        TreeNodeCollection nodes = treeView.Nodes;
        if (boneDesc.iParent >= 0)
        {
          SkeletalBoneDesc parentDesc = EntityShape.tempBones[boneDesc.iParent];
          if (parentDesc.TreeNode != null)
            nodes = parentDesc.TreeNode.Nodes;
        }
        TreeNode node = nodes.Add(boneDesc.Name, boneDesc.Name); // name and display text
        boneDesc.TreeNode = node;
        BoneProxyShape found = BoneProxyShape.FindBoneShape(list, boneDesc.Name);
        node.Checked = found != null;
        node.Tag = found;
      }

      treeView.ExpandAll();

      // Show our listbox as a DropDownControl. 
      // this methods returns when the dropdowncontrol is closed
      editorService.DropDownControl(treeView);

      // get back string collection from selected items
      EntityShape.tempBones.Clear();
      list = new List<BoneProxyShape>();
      GetSelectionRecursive(entity, list, treeView.Nodes);

      return list;
    }
  }

  /// <summary>
  /// TypeConverter class that displays a nice string in the property grid for the bone collection
  /// </summary>
  class SkeletalBoneListConverter : TypeConverter
  {
    public override object ConvertTo(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value, Type destinationType)
    {
      if (destinationType == typeof(string))
      {
        List<BoneProxyShape> list = (List<BoneProxyShape>)value;
        if (list == null || list.Count == 0) return "none";
        string str = "";
        bool bFirst = true;
        foreach (BoneProxyShape bone in list)
        {
          if (!bFirst)
            str += ",";
          str += bone.BoneName;
          bFirst = false;
        }
        return str;
      }
      return base.ConvertTo(context, culture, value, destinationType);
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
