/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//#define MESHGROUP_USE_LINKING // also define in StaticMeshGroupShape.cs

using System;
using System.IO;
using System.Diagnostics;
using System.Collections;
using System.Drawing.Design;
using System.ComponentModel;
using System.Globalization;
using System.Runtime.Serialization;
using CSharpFramework;
using CSharpFramework.Math;
using CSharpFramework.Shapes;
using CSharpFramework.PropertyEditors;
using CSharpFramework.DynamicProperties;
using CSharpFramework.UndoRedo;
using CSharpFramework.Actions;
using CSharpFramework.Scene;
using CSharpFramework.View;
using CSharpFramework.Serialization;
using System.Drawing;
using VisionManaged;
using System.Collections.Specialized;
using System.Windows.Forms;
using ManagedFramework;
using System.Collections.Generic;
using System.Xml;
using CSharpFramework.Helper;
using CSharpFramework.Visitors;
using CSharpFramework.Dialogs;
using System.Reflection;
using System.Windows.Forms.Design;
using CSharpFramework.BaseTypes;
using ManagedBase;


/*
 LOD TODO:
 * - switch usage invalidates lightmaps
 * - unify UI/disk and internal representation. This allows for setting an overall farclip in the definition file
 * - MRU list must be mesh-specific
 * - OPTIMIZE: sort out meshes that are behind farclip
 * - OPTIMIZE: only need n-1 cached IDs
 * - WORKFLOW: Relevant operation: save deprecated 1-lod setup to definition file
 */
namespace VisionEditorPlugin.Shapes
{

  #region class MeshPositionSnapPoint

  class MeshPositionSnapPoint : ShapeSnapPoint
  {
    public MeshPositionSnapPoint(Shape3D shape)
      : base(shape)
    {
    }

    public override bool CanSnapTo(ShapeSnapPoint other)
    {
      if (!base.CanSnapTo(other))
        return false;
      return other is MeshPositionSnapPoint;
    }
  }

  #endregion

  #region Class CollisionLayerTypeConverter

  /// <summary>
  /// String converter class to enumerate the 32 collision layer names
  /// </summary>
  public class CollisionLayerTypeConverter : StringConverter
  {
    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="context"></param>
    /// <returns></returns>
    public override bool GetStandardValuesSupported(ITypeDescriptorContext context)
    {
      return true;
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="context"></param>
    /// <returns></returns>
    public override bool GetStandardValuesExclusive(ITypeDescriptorContext context)
    {
      return true;
    }

	// Please keep this in sync with the enum in Source/Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp
    public static string[] CollisionLayerNames = new string[] {
      "0", "Dynamic", "Static", "Terrain", "Controller", "5", 
      "6","Ragdoll",
      "Attachments","Foot_IK","10","Debris","12","13","14","15",
      "16","17","18","19","20","21","22","23",
      "24","25","26","Custom0","Custom1","Custom2","Custom3","Custom4"
    };

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="context"></param>
    /// <returns></returns>
    public override System.ComponentModel.TypeConverter.StandardValuesCollection GetStandardValues(ITypeDescriptorContext context)
    {
      return new StandardValuesCollection(CollisionLayerNames);
    }
  }

  #endregion

  #region Static Mesh Shape

  [Serializable, ShapeTypeNiceName("Static Mesh")]
  public class StaticMeshShape : Shape3D, ICustomPrefabSerialization
  {

    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_SETTINGS = "Mesh";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_SETTINGS = Shape3D.LAST_CATEGORY_ORDER_ID + 1;

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_LOD = "LOD";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_LOD = Shape3D.LAST_CATEGORY_ORDER_ID + 2;

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_LIGHTING = "Lighting";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_LIGHTING = Shape3D.LAST_CATEGORY_ORDER_ID + 3;

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_STATICLIGHTING = "Static Lighting";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_STATICLIGHTING = Shape3D.LAST_CATEGORY_ORDER_ID + 4;

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_PHYSICS = "Physics";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_PHYSICS = Shape3D.LAST_CATEGORY_ORDER_ID + 5;

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_PREVIEW = "Preview";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_PREVIEW = Shape3D.LAST_CATEGORY_ORDER_ID + 6;
    /// <summary>
    /// last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_PREVIEW;

    #endregion

    #region Constructor

    public StaticMeshShape(string name)
      : base(name)
    {
      _fLODSwitchDist *= EditorManager.Settings.GlobalUnitScaling;
    }

    #endregion

    #region Engine Instance

    bool _bInstanceHadLightmaps = false; //  to restore lightmap information after delete/undo

    public override void CreateEngineInstance(bool bCreateChildren)
    {
      if (_engineInstance == null)
      {
        _engineInstance = new EngineInstanceStaticMesh();
        SetEngineInstanceBaseProperties(); // sets the position etc.
      }

      // Call the base function, e.g. for adding components to the engine instance
      base.CreateEngineInstance(bCreateChildren);
    }


    public override void RemoveEngineInstance(bool bRemoveChildren)
    {
      // delete the engine instance if the reason is that the zone is unloaded
      if (this.ParentZone != null && this.ParentZone.CurrentStatus == Zone.ZoneStatus.Unloading)
      {
        base.RemoveEngineInstance(bRemoveChildren);
        return;
      }

      // pretend that we have no instance to keep the instance. It is removed in OnRemoveFromScene
      IEngineShapeInstance _keepInst = _engineInstance;
      _engineInstance = null;

      base.RemoveEngineInstance(bRemoveChildren);

      _engineInstance = _keepInst;
    }


    protected void RecursiveAssignOwnerShape(ShapeCollection shapes)
    {
      if (shapes == null)
        return;

      foreach (ShapeBase shape in shapes)
      {
        VisibilityZoneShape zone = shape as VisibilityZoneShape;
        if (zone!=null)
          zone.VisibilityOwnerMeshShape = this;

        RecursiveAssignOwnerShape(shape.ChildCollection);
      }
    }

    private ulong GetLowResID()
    {
      ulong iLowResID = (((ulong)ParentLayer.LayerID) << 32) | _iLocalIDLowRes;
      return iLowResID;
    }

    void RestoreLightmaps()
    {
      string absFilename;
      // see OnCreateAllEngineInstances in EditorScene.cs
      if (this.ParentZone == null)
        absFilename = Path.Combine(EditorManager.Scene.LayerDirectoryName, "StaticLightingInfo.lit");
      else
        absFilename = Path.Combine(EditorManager.Scene.LayerDirectoryName, "StaticLightingInfo_" + ParentZone.ZoneName + ".lit");

      EngineMesh.ReloadLightmapFile(absFilename);
    }

    public override void SetEngineInstanceBaseProperties()
    {
      base.SetEngineInstanceBaseProperties();
      if (!HasEngineInstance())
      {
        RecursiveAssignOwnerShape(_visibilityShapes);
        return;
      }

      // Set the mesh file but do not yet notify physics about the new instance. We will do this
      // later, once all properties have been set
      RemoveNativeComponents();
      SetNativeMeshes();
      ReAttachComponents();
      EngineMesh.SetVisibleBitmask(_iVisibleBitmask);
      EngineMesh.SetLightInfluenceBitmask(_iLightInfluenceBitmask);
      if (UsesCollisionGroups)
        EngineMesh.SetCollisionGroup(_collisionBehavior,_collisionLayer,_subSystemId, _subSystemDontCollideWith,_systemGroup);
      else
        EngineMesh.SetCollisionBitmask(_iCollisionBitmask);
      EngineMesh.SetWeldingType(_weldingType);
      EngineMesh.SetCastDynamicShadows(_bCastDynamicShadows);
      EngineMesh.SetCastStaticShadows(_bCastStaticShadows);
      EngineMesh.SetObjectKey(_objectKey);
      EngineMesh.SetLightgridSamplePosition(_vLightGridOfs.X, _vLightGridOfs.Y, _vLightGridOfs.Z);
      EngineMesh.SetLightmapMultiplier(_fLightmapMultiplier);
      EngineMesh.SetLightmapOverride(_lightmapOverride, false);

      // Let physics know about our static mesh now (since now all properties are set)
      EngineMesh.SendPhysicsOnStaticMeshCreatedEvent();
        
      RecursiveAssignOwnerShape(_visibilityShapes);

      if (_bInstanceHadLightmaps) // only true if the this shape was part of the scene earlier
      {
        RestoreLightmaps();
        _bInstanceHadLightmaps = false;
      }
      UpdateClipSettings();
      ApplyCustomMaterialSet();
    }


    [Browsable(false)]
    public EngineInstanceStaticMesh EngineMesh { get { return (EngineInstanceStaticMesh)_engineInstance; } }

    private bool IsResourceUsed(String resourceName)
    {
      if (String.IsNullOrEmpty(resourceName))
        return false;

      LODSetup setup = PrepareLODSetup();
      foreach (LODEntry entry in setup)
      {
        if (String.Compare(PathHelper.CanonicalizePath(entry._filename), resourceName, true) == 0)
        {
          return true;
        }
      }

      return false;
    }

    /// <summary>
    /// Listen to the global SceneEvent callback to respond to mesh reloading
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    public override void OnSceneEvent(SceneEventArgs e)
    {
      base.OnSceneEvent(e);

      // in this case we need to re-assign the mesh because the submesh layout may change (some info is cached on the engine instance)
      // In theory we only need to perform this for actual changed meshes, but this isn't critical
      if (e.action == SceneEventArgs.Action.AfterResourceUpdate)
      {
        string resourceName = e.CustomData as string;
        if (resourceName == null || e.Success && IsResourceUsed(resourceName))
        {
          if (_engineInstance != null)
            EngineMesh.RebuildSubMeshes();
        }
      }
      else if (e.action == SceneEventArgs.Action.AfterUniqueIDRepaired)
      {
        _iLocalIDLowRes = ParentLayer.CreateNewShapeID();
        if (_visibilityShapes != null)
          OnMeshFileChanged(); // reload visibility file
      }
      else if (e.action == SceneEventArgs.Action.AfterStaticLighting || e.action == SceneEventArgs.Action.StaticLightingReset || e.action == SceneEventArgs.Action.StaticLightingLoaded)
      {
        if (HasEngineInstance())
          EngineMesh.SetLightmapOverride(_lightmapOverride, false);
      }
    }


    #endregion

    #region Shape overrides

    protected override Shape3D.ParentingModes ParentingMode
    {
      get
      {
        return ParentingModes.SceneGraphScaled;
      }
    }

    static void RenderVisShapeRecursive(VisionViewBase view, ShapeBase shape, ShapeRenderMode mode)
    {
      if (shape is VisibilityZoneShape)
        ((VisibilityZoneShape)shape).RenderVisibilityZone(view, 0);
      else if (shape is PortalShape)
        ((PortalShape)shape).RenderPortal(view, 0);
      else
        shape.RenderShape(view, mode);

      if (shape.HasChildren())
        foreach (ShapeBase child in shape.ChildCollection)
          RenderVisShapeRecursive(view, child, mode);

    }

    public override void RenderShape(VisionViewBase view, ShapeRenderMode mode)
    {
      base.RenderShape(view, mode);
      if (_bPreviewVisibility && _visibilityShapes != null)
      {
        ShapeRenderMode vismode = (mode == ShapeRenderMode.None) ? ShapeRenderMode.None : ShapeRenderMode.Selected;
        foreach (ShapeBase shape in _visibilityShapes)
          RenderVisShapeRecursive(view, shape, vismode);
      }
    }


    public override ShapeBase Clone()
    {
      StaticMeshShape copy = (StaticMeshShape)base.Clone();
      copy._bInstanceHadLightmaps = false;
      copy._visibilityShapes = null;
      copy._hotSpotLightGridOfs = null;
      copy._iLocalIDLowRes = 0;
      copy._snappoints = SNAPPOINTLIST_CACHE_INVALID; // force update next time;
      copy._cachedLODSetup = null;
      copy._cachedLODIds = null;

      copy.OnMeshFileChanged();
      return copy;
    }


    protected override void GetStaticLightInfo(StaticLightInfo info)
    {
      if (!this.HasMeshFile)
        return;

      if (CastStaticShadows)
        info.RelevantOccluder.Add(this);

      if (!string.IsNullOrEmpty(LowResMeshFileName) && this._iLocalIDLowRes != 0)
        info.AddReceiverWithLOD(this, new ulong[1] { GetLowResID() });
      else
        info.RelevantReceiver.Add(this);
    }


    public override void OnAddedToScene()
    {
      base.OnAddedToScene();
      if (_iLocalIDLowRes == 0)
        _iLocalIDLowRes = ParentLayer.CreateNewShapeID();
      _snappoints = SNAPPOINTLIST_CACHE_INVALID; // force update next time
    }


    public override void OnRemoveFromScene()
    {
      // finally delete our engine instance here
      if (this._engineInstance != null)
      {
        this._bInstanceHadLightmaps = EngineMesh.HasLightmaps(); // backup flag for delete/undo

        // Important: We must call the base implementation here because the overload in this class prevents the deletion
        base.RemoveEngineInstance(false);
      }
      base.OnRemoveFromScene();
      _snappoints = null;
    }


    public override float GetFarClipDistance()
    {
      return _fFarClipDist > 0.0f ? _fFarClipDist : -1.0f;
    }

    public static string VISIBILITY_TARGET_ID = "VisibilityTarget_1";
  #if (MESHGROUP_USE_LINKING)
    public static string MESHGROUP_TARGET_ID = "GroupMeshesTarget_1";
  #endif
    public override void CreateAllLinks()
    {
      base.CreateAllLinks();
      SafeCreateAllLinks();
    }

    void SafeCreateAllLinks()
    {
      if (LinkTargets.GetLinkByPrimaryId(VISIBILITY_TARGET_ID) == null)
        LinkTargets.Add(new LinkTargetVisiblity(this, VISIBILITY_TARGET_ID));
    #if (MESHGROUP_USE_LINKING)
      if (LinkTargets.GetLinkByPrimaryId(MESHGROUP_TARGET_ID) == null)
        LinkTargets.Add(new LinkTargetGroupStaticMeshes(this, MESHGROUP_TARGET_ID));
    #endif
    }

    public override PropertyFlags_e GetPropertyFlags(PropertyFlagsProviderInfo pd)
    {
      switch (LODUsage)
      {
        case LODUsageType_e.DefinitionInMeshFile:
          if (pd.Name == "LowResMeshFileName" || pd.Name == "FarClipDistance" || pd.Name == "LODSwitchDistance" || pd.Name == "LODDefinitionFile" || pd.Name == "LODMeshes")
            return PropertyFlags_e.Hidden;
          break;
        case LODUsageType_e.FarClipOnly:
          if (pd.Name == "LowResMeshFileName" || pd.Name == "LODSwitchDistance" || pd.Name == "LODDefinitionFile" || pd.Name == "LODMeshes")
            return PropertyFlags_e.Hidden;
          break;
        case LODUsageType_e.LODDefinitionFile:
          if (pd.Name == "LowResMeshFileName" || pd.Name == "LODSwitchDistance")
          return PropertyFlags_e.Hidden;
          if (string.IsNullOrEmpty(MeshFileName) && pd.Name == "LODDefinitionFile")
            return PropertyFlags_e.Readonly;
          break;
        case LODUsageType_e.Deprecated_1LOD:
          if (pd.Name == "LODDefinitionFile" || pd.Name == "LODMeshes")
            return PropertyFlags_e.Hidden;
          break;
      }

      if (UsesCollisionGroups)
      {
        if (pd.Name == "CollisionBitmask")
          return PropertyFlags_e.Hidden;
        if (CollisionBehavior != CollisionBehavior_e.Custom)
        {
          if (pd.Name == "CollisionLayer" || pd.Name == "SystemGroup" || pd.Name == "SubSystemID" || pd.Name == "SubSystemDontCollideWith")
            return PropertyFlags_e.Hidden;
        }

      }
      else
      {
        if (pd.Name == "CollisionBehavior" || pd.Name == "CollisionLayer" || pd.Name == "SystemGroup" || pd.Name == "SubSystemID" || pd.Name == "SubSystemDontCollideWith")
          return PropertyFlags_e.Hidden;
      }

      return base.GetPropertyFlags(pd);
    }


    public override void OnUniqueIDChanged()
    {
      base.OnUniqueIDChanged();
      _cachedLODIds = null;

      RemoveNativeComponents();
      SetNativeMeshes(); // this is the only place that assigns unique IDs properly
      ReAttachComponents();
    }

    #endregion

    #region members

    string _meshFile = "";
    string _meshFileLOD1 = "";
    float _fLODSwitchDist = 10000.0f;
    float _fFarClipDist = 0.0f;
    uint _iLocalIDLowRes = 0; // for lightmapping, the lowres version gets its own ID
    
    // we use uint here and FlagsInt16_e/FlagsInt32_e in the properties so we can easily upgrade
    uint _iVisibleBitmask = (uint)FlagsInt32_e.All;
    uint _iLightInfluenceBitmask = (uint)FlagsInt16_e.All;
    bool _bCastStaticShadows = true;
    bool _bCastDynamicShadows = false;
    bool _bPreviewVisibility = false;
    Vector3F _vLightGridOfs = Vector3F.Zero;
    float _fLightmapMultiplier = 1.0f;

    static bool _bShowLightgridOfsWidget = false;

    uint _iCollisionBitmask = (uint)FlagsInt16_e.All;
    CollisionBehavior_e _collisionBehavior = CollisionBehavior_e.FromFile;
    byte _collisionLayer = 2;
    byte _subSystemId = 0;
    byte _subSystemDontCollideWith = 0;
    ushort _systemGroup = 0;
    WeldingType_e _weldingType = WeldingType_e.None;
    string _lightmapOverride = null;

    static bool _bUsesCollisionGroups = false;

    #endregion

    #region properties


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
    /// Mesh File Property (uses filename editor)
    /// </summary>
    [PrefabResolveFilename]
    [SortedCategory(CAT_SETTINGS, CATORDER_SETTINGS),
    PropertyOrder(1),
    EditorAttribute(typeof(AssetEditor), typeof(UITypeEditor)),
    AssetDialogFilter(new string[] { "StaticMesh" })]
    [ThumbnailFormat(new string[] { @"{0}_data\Thumbnail.bmp", @"{0}_data\Thumbnail.jpg" })]
    [Description("The static mesh used by this shape.")]
    [DependentProperties(new string[] { "AutoRenaming" })]
    public string MeshFileName
    {
      get { return _meshFile; }
      set 
      {
        if (_meshFile == value)
          return;

        // value will usually already be a uid
        _meshFile = value;

        // Set name automatically unless the shape has been renamed manually
        if (AutoRenaming && EditorManager.Scene != null)
        {
          string name = Path.GetFileNameWithoutExtension(_meshFile);
          ShapeName = String.IsNullOrEmpty(name) ? "StaticMesh" : name;
        }

        OnMeshFileChanged();
        
        SetEngineInstanceBaseProperties();
        //using following function to reattach components
        //to avoid crash when changing the engine instance when at least one component is attached [#2784]
        ReAttachComponents();
        InvalidateZoneBoundingBox();
        ReassignAllLinks(); // visibility shape linking update
        _snappoints = SNAPPOINTLIST_CACHE_INVALID; // force update next time
      }
    }

    /// <summary>
    /// gets the material provider of the mesh's model
    /// </summary>
    [BrowsableAttribute(false)]
    public IMaterialProvider MaterialProvider
    {
      get { return EditorManager.EngineManager.FindMaterialProvider(this.MeshFileName); }
    }

    string _customMaterialSetName = null;

    [Description("Sets an individual material set on this instance. Variants of the mesh's material set can be created in the material editing layout. If left to <none> the mesh's default material set is used.")]
    [SortedCategory(CAT_SETTINGS, CATORDER_SETTINGS), PropertyOrder(2)]
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
      EngineMesh.SetCustomMaterialSet(_customMaterialSetName);
    }

    #region LOD


    public enum LODUsageType_e
    {
      [Description("Uses near/farclip information from inside the vmesh file and assumes this single vmesh contains LOD structure. Farclip distance property is ignored.")]
      DefinitionInMeshFile,
      [Description("Uses the far clip distance property for this instance (all submeshes are clipped at once). No additional LOD")]
      FarClipOnly,
      [Description("Deprecated, do not use anymore. For backwards compatibility to support a mesh with one optioanl additional LOD mesh")]
      Deprecated_1LOD,
      [Description("LOD definition is taken from an XML file that is located inside the vmesh_data folder of the selected mesh")]
      LODDefinitionFile
    }

    LODSetup _cachedLODSetup = null;
    uint[] _cachedLODIds = null;
    LODUsageType_e _lodUsage = LODUsageType_e.FarClipOnly;
    string _lodDefinitionFile = null;

    [Description("Specified if and how LOD is used for this mesh instance. On the runtime side LOD meshes are separate vmesh files.")]
    [SortedCategory(CAT_LOD, CATORDER_LOD), PropertyOrder(1)]
    public LODUsageType_e LODUsage
    {
      get { return _lodUsage; }
      set
      {
        if (_lodUsage == value)
          return;
        _lodUsage = value;
        _cachedLODSetup = null;
        if (HasEngineInstance())
      {
          // need to set everything from scratch
          SetEngineInstanceBaseProperties();
        }
      }
      }

    [EditorAttribute(typeof(LODDefinitionFilenameEditor), typeof(UITypeEditor))]
    [Description("The LOD definition file to use. The LOD definition file is a file that resides in the mesh's vmesh_data sub-directory and which defines additional LOD meshes and the respective switch distance. This file is not needed by the runtime")]
    [SortedCategory(CAT_LOD, CATORDER_LOD), PropertyOrder(2)]
    public string LODDefinitionFile
    {
      get { return _lodDefinitionFile; }
      set
      {
        if (_lodDefinitionFile == value)
          return;
        _lodDefinitionFile = value;
        if (_lodUsage == LODUsageType_e.LODDefinitionFile)
        {
          // need to set everything from scratch
          _cachedLODSetup = null;
        if (HasEngineInstance())
            SetEngineInstanceBaseProperties();
      }
      }
    }


    [Description("A list of all additional LOD meshes that are currently used by this instance. These instances are used for rendering and not used for physics etc.")]
    [SortedCategory(CAT_LOD, CATORDER_LOD), PropertyOrder(50)]
    public string LODMeshes
    {
      get { return CurrentLODSetup.ToString(); }
    }

    /// <summary>
    /// This is a blind property that is only there for backwards compatibility with old prefab files
    /// </summary>
    [Browsable(false)]
    public bool UseLODFromFile
    {
      get 
      {
        return LODUsage == LODUsageType_e.DefinitionInMeshFile; 
      }
      set 
      {
        LODUsage = value ? LODUsageType_e.DefinitionInMeshFile : LODUsageType_e.FarClipOnly;
      }
    }

    /// <summary>
    /// deprecated feature
    /// </summary>
    [PrefabRelevant(false)]
    [PrefabResolveFilename]
    [SortedCategory(CAT_LOD, CATORDER_LOD),
    PropertyOrder(3),
    EditorAttribute(typeof(AssetEditor), typeof(UITypeEditor)),
    AssetDialogFilter(new string[] { "StaticMesh" })]
    [ThumbnailFormat(new string[] { @"{0}_data\Thumbnail.bmp", @"{0}_data\Thumbnail.jpg" })]
    [Description("Deprecated feature to maintain backwards compaibility. Use a LOD definition file instead.")]
    public string LowResMeshFileName
    {
      get { return _meshFileLOD1; }
      set
      {
        if (_meshFileLOD1 == value)
          return;
        _meshFileLOD1 = value;
        OnMeshFileChanged();
        SetEngineInstanceBaseProperties(); // update everything
        InvalidateZoneBoundingBox();
        ReassignAllLinks(); // visibility shape linking update
      }
    }


    [SortedCategory(CAT_LOD, CATORDER_LOD), PropertyOrder(4)]
    [Description("Mesh far clip distance in world units. Set to 0.0 for no distance clipping."), DefaultValue(0.0f)]
    public float FarClipDistance
    {
      get { return _fFarClipDist; }
      set 
      {
        _fFarClipDist = value;
        if (_fFarClipDist < 0.0f)
          _fFarClipDist = 0.0f;
        UpdateClipSettings();
      }
    }

    /// <summary>
    /// Deprecated feature
    /// </summary>
    [PrefabRelevant(false)]
    [SortedCategory(CAT_LOD, CATORDER_LOD), PropertyOrder(5)]
    [Description("Deprecated feature to maintain backwards compaibility. Use a LOD definition file instead."), DefaultValue(10000.0f)]
    public float LODSwitchDistance
    {
      get { return _fLODSwitchDist; }
      set
      {
        if (_fLODSwitchDist == value)
          return;
        _fLODSwitchDist = value;
        if (_fLODSwitchDist < 1.0f)
          _fLODSwitchDist = 1.0f;
        if (_cachedLODSetup is LODSetup_Deprecated1LOD) // important: Update inside the setup
          ((LODSetup_Deprecated1LOD)_cachedLODSetup).LODSwitchDistance = _fLODSwitchDist;
        UpdateClipSettings();
      }
    }
    /// <summary>
    /// Indicates whether this mesh as a mesh file assigned
    /// </summary>
    [Browsable(false)]
    public bool HasMeshFile
    {
      get
      {
        return !string.IsNullOrEmpty(_meshFile);
      }
    }


    /// <summary>
    /// Returns the current LOD setup that is used. LOD setups are typically shared, except the deprecated type for 1 additional LOD. The return value is always !=null
    /// </summary>
    [Browsable(false)]
    [SortedCategory(CAT_LOD, CATORDER_LOD), PropertyOrder(20)]
    public LODSetup CurrentLODSetup
    {
      get
      {
        if (_cachedLODSetup == null)
        {
          if (LODUsage == LODUsageType_e.Deprecated_1LOD && !string.IsNullOrEmpty(MeshFileName) && !string.IsNullOrEmpty(LowResMeshFileName))
            _cachedLODSetup = new LODSetup_Deprecated1LOD(this.MeshFileName, LowResMeshFileName, this.LODSwitchDistance);
          else if (LODUsage == LODUsageType_e.LODDefinitionFile && !string.IsNullOrEmpty(this._meshFile) && !string.IsNullOrEmpty(_lodDefinitionFile))
            _cachedLODSetup = LODManager.GetSetup(this._meshFile + "?" + _lodDefinitionFile);
          else
            _cachedLODSetup = LODManager.GetSetup(this._meshFile);
        }
        return _cachedLODSetup;
      }
    }

    /// <summary>
    /// Returns a list of local unique IDs that correspond to the items in the lod setup
    /// </summary>
    uint[] CurrentLODIDs
    {
      get
      {
        LODSetup setup = CurrentLODSetup;
        if (_cachedLODIds == null || _cachedLODIds.Length != setup.Count 
          || (_cachedLODIds.Length > 0 && _cachedLODIds[0] != this.LocalID)) // the last check is just for validation, should not happen
        {
          uint[] oldIDs = _cachedLODIds;
          int iOldCount = oldIDs != null ? oldIDs.Length : 0;
          _cachedLODIds = new uint[setup.Count];

          // reuse old IDs, if possible
          if (_iLocalIDLowRes != 0 && setup is LODSetup_Deprecated1LOD)
          {
            _cachedLODIds[1] = _iLocalIDLowRes;
          }
          else
          {
            for (int i = 1; i < setup.Count; i++)
              _cachedLODIds[i] = i < iOldCount ? oldIDs[i] : this.ParentLayer.CreateNewShapeID();
          }

          // first one is always the shape's own ID
          if (setup.Count > 0)
            _cachedLODIds[0] = this.LocalID;
        }

        return _cachedLODIds;
      }
    }


    /// <summary>
    /// Returns CurrentLODSetup but additionally fills out the temporary data. This version should be used to pass it to the engine
    /// </summary>
    /// <returns></returns>
    LODSetup PrepareLODSetup()
    {
      LODSetup setup = CurrentLODSetup;
      uint[] ids = CurrentLODIDs;
      UInt64 iLayerPart = (UInt64)ParentLayer.LayerID << 32;

      // prepare the temporary data
      for (int i = 0; i < setup.Count; i++)
      {
        LODEntry entry = setup[i];
        entry._uniqueID = iLayerPart | ids[i];
        // make a temporary copy of the far clip distance and min it with the shape's far clip
        entry._usedClipDistance = entry._clipDistance;
        if (_fFarClipDist > 0.0f)
        {
          if (entry._usedClipDistance <= 0.0f || _fFarClipDist < entry._usedClipDistance)
            entry._usedClipDistance = _fFarClipDist;
        }
      }
      return setup;
    }

    /// <summary>
    /// This function actually creates the native meshes for all LOD
    /// </summary>
    void SetNativeMeshes()
    {
      if (!HasEngineInstance())
        return;
      LODSetup setup = PrepareLODSetup();
      EngineMesh.SetLODChain(setup);
    }

    /// <summary>
    /// Internal update of LOD/clip settings
    /// </summary>
    void UpdateClipSettings()
    {
      if (!HasEngineInstance())
        return;
      LODSetup setup = PrepareLODSetup();
      EngineMesh.SetFarClipDistance(setup, _lodUsage == LODUsageType_e.DefinitionInMeshFile);
    }

    /// <summary>
    /// Called when the shared setup has changed, e.g. after re-save
    /// </summary>
    public void OnLODSetupChangedNotify()
    {
      if (HasEngineInstance())
        SetEngineInstanceBaseProperties();
    }

    #endregion

    /// <summary>
    /// Filter bitflags for context rendering
    /// </summary>
    [SortedCategory(CAT_LIGHTING, CATORDER_LIGHTING), PropertyOrder(1)]
    [Description("Filter bitflags for context rendering"), DefaultValue(FlagsInt32_e.All)]
    public FlagsInt32_e VisibleBitmask
    {
      get { return (FlagsInt32_e)_iVisibleBitmask; }
      set
      {
        if (_iVisibleBitmask == (uint)value)
          return;
        _iVisibleBitmask = (uint)value;
        if (HasEngineInstance())
          EngineMesh.SetVisibleBitmask(_iVisibleBitmask);
      }
    }

    /// <summary>
    /// Filter bitflags for static/dynamic light influence (0 for not affected by light)
    /// </summary>
    [SortedCategory(CAT_LIGHTING, CATORDER_LIGHTING), PropertyOrder(2)]
    [Description("Filter bitflags for static/dynamic light influence (0 for not affected by light)"), DefaultValue((FlagsInt16_e)((uint)FlagsInt16_e.All & 0xffff))]
    public FlagsInt16_e LightInfluenceBitmask
    {
      get { return (FlagsInt16_e)(_iLightInfluenceBitmask&0xffff); }
      set
      {
        if (_iLightInfluenceBitmask == (uint)value)
          return;
        _iLightInfluenceBitmask = (uint)value;
        if (HasEngineInstance())
          EngineMesh.SetLightInfluenceBitmask(_iLightInfluenceBitmask);
      }
    }

    /// <summary>
    /// Indicates whether this mesh is supposed to cast a static shadow
    /// </summary>
    [SortedCategory(CAT_LIGHTING, CATORDER_LIGHTING), PropertyOrder(4)]
    [Description("Indicates whether this mesh is supposed to cast a static shadow"), DefaultValue(true)]
    public bool CastStaticShadows
    {
      get { return _bCastStaticShadows; }
      set
      {
        if (_bCastStaticShadows == value)
          return;
        _bCastStaticShadows = value;
        if (HasEngineInstance())
          EngineMesh.SetCastStaticShadows(_bCastStaticShadows);
      }
    }

    /// <summary>
    /// Indicates whether this mesh is supposed to cast a dynamic stencil shadows
    /// </summary>
    [SortedCategory(CAT_LIGHTING, CATORDER_LIGHTING), PropertyOrder(5)]
    [Description("Indicates whether this mesh is supposed to cast a dynamic stencil shadows"), DefaultValue(false)]
    public bool CastDynamicShadows
    {
      get { return _bCastDynamicShadows; }
      set
      {
        if (_bCastDynamicShadows == value)
          return;
        _bCastDynamicShadows = value;
        if (HasEngineInstance())
          EngineMesh.SetCastDynamicShadows(_bCastDynamicShadows);
      }
    }

    /// <summary>
    /// If enabled, a 3D widget will be rendered that allows to modify the offset in the 3D view. This property wraps around a static bool so it affects all entities
    /// </summary>
    [SortedCategory(CAT_STATICLIGHTING, CATORDER_STATICLIGHTING), PropertyOrder(10)]
    [Description("If enabled, a 3D widget will be rendered that allows to modify the offset in the 3D view"), DefaultValue(false)]
    public bool ShowLightgridWidget
    {
      get 
      { 
        return _bShowLightgridOfsWidget/* && LightingMethod==LightingMethod_e.LightGrid*/; 
      }
      set { _bShowLightgridOfsWidget = value; }
    }

    /// <summary>
    /// Defines the relative position offset that is used to gather the light grid sample
    /// </summary>
    [BrowsableAttribute(false)]
    public Vector3F LightGridSampleOfs
    {
      get { return _vLightGridOfs; }
      set
      {
        _vLightGridOfs = value;
        if (_engineInstance != null)
          EngineMesh.SetLightgridSamplePosition(_vLightGridOfs.X, _vLightGridOfs.Y, _vLightGridOfs.Z);
      }
    }

    /// <summary>
    /// Gets or sets the relative x position offset that is used to gather the light grid sample
    /// </summary>
    [SortedCategory(CAT_STATICLIGHTING, CATORDER_STATICLIGHTING), PropertyOrder(11)]
    [Description("Defines the relative x position offset that is used to gather the light grid sample"), DefaultValue(0.0f)]
    public float SampleOfsX
    {
      get { return _vLightGridOfs.X; }
      set
      {
        _vLightGridOfs.X = value;
        if (_engineInstance != null)
          EngineMesh.SetLightgridSamplePosition(_vLightGridOfs.X, _vLightGridOfs.Y, _vLightGridOfs.Z);
      }
    }

    /// <summary>
    /// Gets or sets the relative y position offset that is used to gather the light grid sample
    /// </summary>
    [SortedCategory(CAT_STATICLIGHTING, CATORDER_STATICLIGHTING), PropertyOrder(12)]
    [Description("Defines the relative y position offset that is used to gather the light grid sample"), DefaultValue(0.0f)]
    public float SampleOfsY
    {
      get { return _vLightGridOfs.Y; }
      set
      {
        _vLightGridOfs.Y = value;
        if (_engineInstance != null)
          EngineMesh.SetLightgridSamplePosition(_vLightGridOfs.X, _vLightGridOfs.Y, _vLightGridOfs.Z);
      }
    }

    /// <summary>
    /// Gets or sets the relative z position offset that is used to gather the light grid sample
    /// </summary>
    [SortedCategory(CAT_STATICLIGHTING, CATORDER_STATICLIGHTING), PropertyOrder(13)]
    [Description("Defines the relative z position offset that is used to gather the light grid sample"), DefaultValue(0.0f)]
    public float SampleOfsZ
    {
      get { return _vLightGridOfs.Z; }
      set
      {
        _vLightGridOfs.Z = value;
        if (_engineInstance != null)
          EngineMesh.SetLightgridSamplePosition(_vLightGridOfs.X, _vLightGridOfs.Y, _vLightGridOfs.Z);
      }
    }

    /// <summary>
    /// When materials in this mesh are lightmapped, this value defines a relative lightmap size factor to increase/decrease lightmap resolution for this instance
    /// </summary>
    [SortedCategory(CAT_STATICLIGHTING, CATORDER_STATICLIGHTING), PropertyOrder(20)]
    [Description("When materials in this mesh are lightmapped, this value defines a relative lightmap size factor to increase/decrease lightmap resolution for this instance"), DefaultValue(1.0f)]
    public float LightmapSizeMultiplier
    {
      get { return _fLightmapMultiplier; }
      set
      {
        _fLightmapMultiplier = value;
        if (_fLightmapMultiplier < 0)
          _fLightmapMultiplier = 0.0f;
        else if (_fLightmapMultiplier > 16.0f)
          _fLightmapMultiplier = 16.0f;
        if (_engineInstance != null)
          EngineMesh.SetLightmapMultiplier(_fLightmapMultiplier);
      }
    }
    
    /// <summary>
    /// If specified, the selected lightmap texture will be set on this geometry and override the lightmap calculated through vLux or Beast. This allows for importing pre-generated lightmaps.
    /// </summary>
    [EditorAttribute(typeof(BitmapBrowserEditor), typeof(UITypeEditor))]
    [SortedCategory(CAT_STATICLIGHTING, CATORDER_STATICLIGHTING), PropertyOrder(30)]
    [Description("If specified, the selected lightmap texture will be set on this geometry and override the lightmap calculated through vLux or Beast. This allows for importing pre-generated lightmaps.")]
    public string CustomLightmap
    {
      get { return _lightmapOverride; }
      set
      {
        if (_lightmapOverride == value)
          return;
        _lightmapOverride = value;
        if (_engineInstance != null)
        {
          if (string.IsNullOrEmpty(_lightmapOverride))
            RestoreLightmaps();
          else
            EngineMesh.SetLightmapOverride(_lightmapOverride, true);
        }
      }
    }

    
        
    /// <summary>
    /// Filter bitflags for collision. Depends on physics implementation
    /// </summary>
    [SortedCategory(CAT_PHYSICS, CATORDER_PHYSICS), PropertyOrder(1)]
    [Description("Filter bitflags for collision. Depends on physics implementation"), DefaultValue((FlagsInt16_e)((uint)FlagsInt16_e.All & 0xffff))]
    public FlagsInt16_e CollisionBitmask
    {
      get { return (FlagsInt16_e)(_iCollisionBitmask & 0xffff); }
      set
      {
        if (_iCollisionBitmask == (uint)value)
          return;
        _iCollisionBitmask = (uint)value;
        if (HasEngineInstance() && !UsesCollisionGroups)
          EngineMesh.SetCollisionBitmask(_iCollisionBitmask);
      }
    }


    [Browsable(false)]
    public static bool UsesCollisionGroups
    {
      get { return StaticMeshShape._bUsesCollisionGroups; }
      set { StaticMeshShape._bUsesCollisionGroups = value; }
    }

    void UpdateEngineCollisionGroup()
    {
      // clamp to valid ranges
      if (_collisionLayer < 0) _collisionLayer = 0;
      if (_collisionLayer > 31) _collisionLayer = 31;
      if (_subSystemId < 0) _subSystemId = 0;
      if (_subSystemId > 31) _subSystemId = 31;
      if (_subSystemDontCollideWith < 0) _subSystemDontCollideWith = 0;
      if (_subSystemDontCollideWith > 31) _subSystemDontCollideWith = 31;

      if (!UsesCollisionGroups || !HasEngineInstance())
        return;

      EngineMesh.SetCollisionGroup(_collisionBehavior, _collisionLayer, _subSystemId, _subSystemDontCollideWith, _systemGroup);
    }

    [Description("Defines the collision behavior. Choose FromFile to use the collision groups as set in the exported mesh, Custom to override this instance with custom collision groups or ForceNoCollision to disable collision for this instance. Merged static meshes always use the collision groups as set in the exported mesh.")]
    [SortedCategory(CAT_PHYSICS, CATORDER_PHYSICS), PropertyOrder(10), DefaultValue(CollisionBehavior_e.FromFile)]
    public CollisionBehavior_e CollisionBehavior
    {
      get { return _collisionBehavior; }
      set { _collisionBehavior = value; UpdateEngineCollisionGroup(); }
    }

    [Browsable(false), PrefabRelevant(true)]
    [Description("The collision layer as defined in the physics plugin. See description in the respective physics binding.")]
    [SortedCategory(CAT_PHYSICS, CATORDER_PHYSICS), PropertyOrder(11), DefaultValue(2)]
    public byte CollisionLayerID
    {
      get { return _collisionLayer; }
      set { _collisionLayer = value; UpdateEngineCollisionGroup(); }
    }

    [TypeConverter(typeof(CollisionLayerTypeConverter))]
    [Browsable(true), PrefabRelevant(false)]
    [Description("The collision layer as defined in the physics plugin. See description in the respective physics binding.")]
    [SortedCategory(CAT_PHYSICS, CATORDER_PHYSICS), PropertyOrder(11), DefaultValue(0)]
    public string CollisionLayer
    {
      get { return CollisionLayerTypeConverter.CollisionLayerNames[_collisionLayer&31]; }
      set 
      {
        for (int i = 0; i < CollisionLayerTypeConverter.CollisionLayerNames.Length; i++)
          if (string.Compare(CollisionLayerTypeConverter.CollisionLayerNames[i], value, true) == 0)
            _collisionLayer = (byte)i;
        UpdateEngineCollisionGroup(); 
      }
    }

    [Description("The collision sub system ID as defined in the physics plugin. See description in the respective physics binding.")]
    [SortedCategory(CAT_PHYSICS, CATORDER_PHYSICS), PropertyOrder(12), DefaultValue(0)]
    public byte SubSystemID
    {
      get { return _subSystemId; }
      set { _subSystemId = value; UpdateEngineCollisionGroup(); }
    }

    [Description("The collision sub system ID that this instance should not collide with. See description in the respective physics binding.")]
    [SortedCategory(CAT_PHYSICS, CATORDER_PHYSICS), PropertyOrder(13), DefaultValue(0)]
    public byte SubSystemDontCollideWith
    {
      get { return _subSystemDontCollideWith; }
      set { _subSystemDontCollideWith = value; UpdateEngineCollisionGroup(); }
    }

    [Description("The collision system group as defined in the physics plugin. See description in the respective physics binding.")]
    [SortedCategory(CAT_PHYSICS, CATORDER_PHYSICS), PropertyOrder(14), DefaultValue(0)]
    public ushort SystemGroup
    {
      get { return _systemGroup; }
      set { _systemGroup = value; UpdateEngineCollisionGroup(); }
    }

    [Description("The welding type.")]
    [SortedCategory(CAT_PHYSICS, CATORDER_PHYSICS), PropertyOrder(15), DefaultValue(0)]
    public WeldingType_e WeldingType
    {
      get { return _weldingType; }
      set 
      { 
        _weldingType = value;
        if (HasEngineInstance())
          EngineMesh.SetWeldingType(_weldingType); 
      }
    }

    /// <summary>
    /// If enabled, visibility zones and portals are rendered along with the selected mesh
    /// </summary>
    [SortedCategory(CAT_PREVIEW, CATORDER_PREVIEW), PropertyOrder(4)]
    [Description("If enabled, visibility zones and portals are rendered along with the selected mesh"), DefaultValue(false)]
    public bool PreviewVisibility
    {
      get { return _bPreviewVisibility; }
      set { _bPreviewVisibility = value; }
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

    #region Selection and Hotspots

    HotSpot3D _hotSpotLightGridOfs = null;

    /// <summary>
    /// Overridden selection function. Adds the hotspots
    /// </summary>
    public override void OnSelected()
    {
      base.OnSelected();
      Debug.Assert(_hotSpotLightGridOfs == null);
      _hotSpotLightGridOfs = new HotSpot3D(this, 16.0f);
      _hotSpotLightGridOfs.ToolTipText = "light grid offset";
      _hotSpotLightGridOfs.StartPosition = LightGridSampleOfs;
      EditorManager.ActiveView.HotSpots.Add(_hotSpotLightGridOfs);
      this.WantsNativeRenderHookCallback = true;
    }

    /// <summary>
    /// Overridden un-selection function. Removes the hotspots
    /// </summary>
    public override void OnUnSelected()
    {
      base.OnUnSelected();
      Debug.Assert(_hotSpotLightGridOfs != null);
      EditorManager.ActiveView.HotSpots.Remove(_hotSpotLightGridOfs);
      _hotSpotLightGridOfs = null;
      this.WantsNativeRenderHookCallback = false;
    }


    /// <summary>
    /// Overridden hot spot handle function
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragBegin(HotSpotBase hotSpot, VisionViewBase view)
    {
      base.OnHotSpotDragBegin(hotSpot, view);
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
      base.OnHotSpotDrag(hotSpot, view, fDeltaX, fDeltaY);
    }

    /// <summary>
    /// Overridden hot spot handle function
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragEnd(HotSpotBase hotSpot, VisionViewBase view)
    {
      base.OnHotSpotDragEnd(hotSpot, view);
      if (hotSpot == _hotSpotLightGridOfs)
      {
        Vector3F vNewPos = _hotSpotLightGridOfs.CurrentPosition;
        LightGridSampleOfs = _hotSpotLightGridOfs.StartPosition; // set old value for the action
        EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this, "LightGridSampleOfs", vNewPos)); // send an action which sets the property from old value to new one
      }
    }

    /// <summary>
    /// Overridden hot spot handle function
    /// </summary>
    /// <param name="hotSpot"></param>
    public override void OnHotSpotEvaluatePosition(HotSpotBase hotSpot)
    {
      base.OnHotSpotEvaluatePosition(hotSpot);

      if (hotSpot == _hotSpotLightGridOfs)
      {
        _hotSpotLightGridOfs.Visible = this.ShowLightgridWidget;

        if (_hotSpotLightGridOfs.IsDragging) // update while dragging
          LightGridSampleOfs = _hotSpotLightGridOfs.CurrentPosition;
        else
          _hotSpotLightGridOfs.StartPosition = LightGridSampleOfs;
      }
    }


    #endregion

    #region Relevant operations

    protected static string RO_CONVERT_TO_ENTITY = @"Convert to\Entity";
    protected static string RO_CREATE_VISIBILITY_OBJECT = @"Create surrounding visibility object";
    protected static string RO_MOVE_INTO_MESHGROUP = @"Move into new mesh group";
    //protected static string RO_EDIT_LODSETUP = @"Edit LOD setup";

    static VisibilityObjectShape _pendingVisShape = null;
    static LinkCollection _pendingTargetLinks = null;
    static StaticMeshGroupShape _pendingMeshGroup = null;
    static ShapeCollection _pendingCollection = null;

    /// <summary>
    /// Overridden function to get the relevant operations for this shape type
    /// </summary>
    public override ArrayList RelevantOperations
    {
      get
      {
        ArrayList operations = base.RelevantOperations;
        if (operations == null)
          operations = new ArrayList(4);
        operations.Add(ShapeObject3D.RELEVANTOP_SHAPENAME_AS_KEY);
        operations.Add(RO_CONVERT_TO_ENTITY);
        if (AbsoluteBoundingBox != null && AbsoluteBoundingBox.Valid && GetVisibilityObject()==null)
          operations.Add(RO_CREATE_VISIBILITY_OBJECT);
        if (SetShapeParentAction.TestShapesLinkToDifferentZone(EditorManager.SelectedShapes, EditorManager.Scene.ActiveLayer.Root, false))
          operations.Add(RO_MOVE_INTO_MESHGROUP);
        //        if (this.LODUsage == LODUsageType_e.LODDefinitionFile && HasMeshFile)
        //          operations.Add(RO_EDIT_LODSETUP);
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

      if (name == ShapeObject3D.RELEVANTOP_SHAPENAME_AS_KEY)
      {
        // start a group
        if (iShapeIndex == 0) EditorManager.Actions.StartGroup("Name as Key");

        EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this, "ObjectKey", ShapeName));

        // close group action
        if (iShapeIndex == iShapeCount - 1) EditorManager.Actions.EndGroup();
        return;
      }

      if (name == RO_CONVERT_TO_ENTITY)
      {
        if (iShapeCount > 1 && iShapeIndex == 0)
          EditorManager.Actions.StartGroup("Convert meshes to entities");

        // create a shape with the same relevant properties
        EntityShape replacement = new EntityShape(this.ShapeName);

        // remap all public browsable properties (except uniform scaling as this overwrites current scaling)
        StringCollection exclude = new StringCollection();
        exclude.Add("UniformScaling");
        SerializationHelper.CopyInstanceProperties(replacement, this, true, true, exclude);

        // remap file name of the mesh
        if (!string.IsNullOrEmpty(MeshFileName))
          replacement.ModelFile = Path.ChangeExtension(this.MeshFileName, ".model");

        EditorManager.Actions.Add(new ReplaceShapeAction(this, replacement));

        if (iShapeCount > 1 && iShapeIndex == iShapeCount-1)
          EditorManager.Actions.EndGroup();

        return;
      }

      if (name == RO_CREATE_VISIBILITY_OBJECT)
      {
        if (iShapeIndex == 0)
        {
          _pendingVisShape = new VisibilityObjectShape("StaticMeshVisibility");
          _pendingVisShape.ContextBitmask = FlagsInt32_e.All;
          _pendingVisShape.HardwareOcclusionTest = true;
          _pendingTargetLinks = new LinkCollection();

          // fire this action already so the object is part of the scene
          EditorManager.Actions.Add(AddShapeAction.CreateAddShapeAction(_pendingVisShape, null, EditorManager.Scene.ActiveLayer, true));

          EditorManager.Actions.StartGroup(RO_CREATE_VISIBILITY_OBJECT);
        }

        ShapeLink target = LinkTargets.GetLinkByPrimaryId(VISIBILITY_TARGET_ID);
        if (target != null)
          _pendingTargetLinks.Add(target);

        if (iShapeIndex == iShapeCount - 1)
        {
          BoundingBox bbox = new BoundingBox();
          foreach (LinkTarget t in _pendingTargetLinks)
          {
            EditorManager.Actions.Add(new LinkAction(_pendingVisShape.GetVisibilityLinkSource(), t));
            bbox.AddBox(t.OwnerShape.AbsoluteBoundingBox); 
          }

          if (bbox.Valid)
          {
            _pendingVisShape.BoxSize = new Vector3F(bbox.SizeX, bbox.SizeY, bbox.SizeZ);
            _pendingVisShape.Position = bbox.Center;
          }

          EditorManager.Actions.EndGroup();
          _pendingVisShape = null;
          _pendingTargetLinks = null;
        }
      }

      if (name == RO_MOVE_INTO_MESHGROUP)
      {
        if (iShapeIndex == 0)
        {
          // fire this action already so the object is part of the scene
          _pendingCollection = new ShapeCollection();
          _pendingMeshGroup = new StaticMeshGroupShape("StaticMeshGroup");
          EditorManager.Actions.Add(AddShapeAction.CreateAddShapeAction(_pendingMeshGroup, null, EditorManager.Scene.ActiveLayer, true));
          EditorManager.Actions.StartGroup(RO_MOVE_INTO_MESHGROUP);
        }

        _pendingCollection.Add(this);
        EditorManager.Actions.Add(new SetShapeParentAction(this, _pendingMeshGroup));

        if (iShapeIndex == iShapeCount - 1)
        {
          _pendingMeshGroup.Position = _pendingCollection.Center;
          EditorManager.Actions.EndGroup();
          _pendingMeshGroup = null;
          _pendingCollection = null;
        }
      }
      /*
       * if (name == RO_EDIT_LODSETUP)
            {
              if (iShapeCount == 1)
                LODManager.CreateNewSetupForMesh(this.MeshFileName);
              return;
            }
       */
    }

    public override Image GetImageForRelevantOperation(string operationName)
    {
      if (operationName == RO_CONVERT_TO_ENTITY)
      {
        // return image of replacement shape here
        int iIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(EntityShape.ICON_NAME, Color.Magenta);
        if (iIndex >= 0)
          return EditorManager.GUI.ShapeTreeImages.ImageList.Images[iIndex];
      }
      return base.GetImageForRelevantOperation(operationName);
    }

    #endregion

    #region Visibility info

    ShapeCollection _visibilityShapes = null;

    protected void RecursiveScaleVisibilityShapes(ShapeCollection shapes)
    {
      if (shapes == null)
        return;
      foreach (ShapeBase shape in shapes)
      {
        if (shape is Shape3D)
          ((Shape3D)shape).Scaling = this.Scaling;
        if (shape.HasChildren())
          RecursiveScaleVisibilityShapes(shape.ChildCollection);
      }
    }

    protected void RecursiveFlagShapes(ShapeCollection shapes)
    {
      if (shapes == null)
        return;
      foreach (ShapeBase shape in shapes)
      {
        shape.ShapeVirtualCounter++;
        shape.ParentLayer = this.ParentLayer;
        if (shape.Parent == null)
        {
          shape.SetParentInternal(this); // pretend this is the parent so AddChild does not recompute world space
          this.AddChild(shape);
          if (HasEngineInstance())
            shape.OnAddedToScene();
        }

        if (shape.HasChildren())
          RecursiveFlagShapes(shape.ChildCollection);

        // flag the snap points so they'll forward snapping to this shape
        Shape3D s3d = shape as Shape3D;
        if (s3d == null) continue;
        List<ShapeSnapPoint> snapPoints = s3d.GetSnapPointList();
        if (snapPoints == null) continue;
        foreach (ShapeSnapPoint p in snapPoints)
          p.SnapShape = this; // the owner shape remains unchanged
      }
    }

    static Dictionary<string, bool> cacheFileExists = new Dictionary<string, bool>();
    void OnMeshFileChanged()
    {      
      _cachedLODSetup = null;

      // remove old shapes
      if (_visibilityShapes != null)
      {
        foreach (ShapeBase shape in _visibilityShapes)
        {
          this.ChildCollection.Remove(shape);
          shape.OnRemoveFromScene();
        }
      }
   
      _visibilityShapes = null;

      // load visibility shapes via prefab system
      if (!string.IsNullOrEmpty(_meshFile))
      {
        string visibilityFile = _meshFile + @"_data\visibility.xml";
        string absFile = EditorManager.Project.MakeAbsolute(visibilityFile);

        bool fileExists = false;
        if (!cacheFileExists.ContainsKey(absFile))
        {
          fileExists = File.Exists(absFile);
          cacheFileExists[absFile] = fileExists;
        }
        else
        {
          fileExists = cacheFileExists[absFile];
        }

        //if (File.Exists(absFile))
        if ( fileExists )
        {
          PrefabDesc desc = PrefabManager.CreatePrefab(visibilityFile);
          if (desc.Loaded)
          {
            _visibilityShapes = desc.CreateInstances(true, false);
            RecursiveFlagShapes(_visibilityShapes);
            this.OnTransformationChanged(); // validate children's position
          }
        }
      }
    }

    public override void OnTransformationChanged()
    {
      base.OnTransformationChanged();
      RecursiveScaleVisibilityShapes(_visibilityShapes);
      if (HasEngineInstance())
      {
        UpdateClipSettings();
        if (!EngineMesh._bDragging)
          EngineMesh.TriggerTransformationUpdated();
      }
      // also invalidate visibility if meshes without local visibility is moved
      EditorManager.VisibilityBuilder.Dirty = true;
    }

    public override void OnDragBegin(ShapeDragMode mode)
    {
      if (HasEngineInstance())
        EngineMesh._bDragging = true;
      base.OnDragBegin(mode);
    }

    public override void OnDragEnd(ShapeDragMode mode)
    {
      base.OnDragEnd(mode);
      if (HasEngineInstance())
        EngineMesh._bDragging = false;
    }


    public override void GetVisibilityInfo(IVisibilityBuilder info)
    {
      base.GetVisibilityInfo(info);

/*
 * if (_visibilityShapes != null)
      {
        // pretend these shapes are children
        foreach (ShapeBase shape in _visibilityShapes)
        {
          shape.GetVisibilityInfoRecursive(info);
        }
      }
 * */
    }


    public override void AssignGeometryToVisibiltyZone(VisibilityZoneShape zone)
    {
      base.AssignGeometryToVisibiltyZone(zone);

      // forward to engine instance to assign submeshes to native zone
      if (HasEngineInstance())
        EngineMesh.AssignGeometryToVisibiltyZone(zone);
    }

    #endregion

    #region Linking

    public VisibilityObjectShape GetVisibilityObject()
    {
      ShapeLink tgt = LinkTargets.GetLinkByPrimaryId(VISIBILITY_TARGET_ID);
      if (tgt == null || tgt.Links.Count == 0)
        return null;

      return tgt.Links[0].OwnerShape as VisibilityObjectShape;
    }

  #if (MESHGROUP_USE_LINKING)
    public StaticMeshGroupShape GetMeshGroup()
    {
      ShapeLink tgt = LinkTargets.GetLinkByPrimaryId(MESHGROUP_TARGET_ID);
      if (tgt == null || tgt.Links.Count == 0)
        return null;

      return tgt.Links[0].OwnerShape as StaticMeshGroupShape;
    }
  #endif

    public override bool CanLink(ShapeLink src, ShapeLink target)
    {
      if (base.CanLink(src, target))
        return true;
      if (target.OwnerShape == this)
      {
        if (src is LinkSourceVisiblity)
        {
          VisibilityObjectShape visShape = GetVisibilityObject();
          return visShape == null || visShape == src.OwnerShape;
        }
      #if (MESHGROUP_USE_LINKING)
        if (src is LinkSourceGroupStaticMeshes)
        {
          StaticMeshGroupShape group = GetMeshGroup();
          return group == null || group == src.OwnerShape;
        }
      #endif
      }
      return false;
    }

    public override void OnLink(ShapeLink src, ShapeLink target)
    {
      base.OnLink(src, target);
      // perform the linking on the native side
      if (target.OwnerShape == this)
      {
        VisibilityObjectShape visobjShape = src.OwnerShape as VisibilityObjectShape;
        if (visobjShape != null && _engineInstance!=null && visobjShape._engineInstance != null)
          EngineMesh.AssignVisibilityObject(visobjShape._engineInstance as EngineInstanceVisibilityObject);
      }
    }

    public override void OnUnlink(ShapeLink src, ShapeLink target)
    {
      base.OnUnlink(src, target);
      // perform the linking on the native side
      if (target.OwnerShape == this)
      {
        VisibilityObjectShape visobjShape = src.OwnerShape as VisibilityObjectShape;
        if (visobjShape != null && _engineInstance != null && visobjShape._engineInstance != null)
          EngineMesh.AssignVisibilityObject(null);
      }
    }

    #endregion

    #region Snap Points

    List<ShapeSnapPoint> _snappoints = null;

    public override List<ShapeSnapPoint> GetSnapPointList()
    {
      if (object.ReferenceEquals(_snappoints, SNAPPOINTLIST_CACHE_INVALID)) // cached result?
      {
        LoadSnapPointFile();
      }
      return _snappoints;
    }

    /// <summary>
    /// This empty collection is created to mark the _snappoints pointer as out-of-date as null is a valid return value.
    /// See GetSnapPointList
    /// </summary>
    public static readonly List<ShapeSnapPoint> SNAPPOINTLIST_CACHE_INVALID = new List<ShapeSnapPoint>();


    /// <summary>
    /// Load the snapshots.xml file
    /// </summary>
    public void LoadSnapPointFile()
    {
      _snappoints = base.GetSnapPointList();
      if (!HasMeshFile)
        return;

      string filename = EditorManager.Project.MakeAbsolute(_meshFile + @"_data\snappoints.xml");
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

    #region ISerializable

    /// <summary>
      /// Called when deserializing
      /// </summary>
      /// <param name="info"></param>
      /// <param name="context"></param>
    protected StaticMeshShape(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      _meshFile = info.GetString("_meshFile");
      _iVisibleBitmask = info.GetUInt32("_iVisibleBitmask");
      _iLightInfluenceBitmask = info.GetUInt32("_iLightInfluenceBitmask");
      _iCollisionBitmask = info.GetUInt32("_iCollisionBitmask");

      _bCastStaticShadows = info.GetBoolean("_bCastStaticShadows");
      _bCastDynamicShadows = info.GetBoolean("_bCastDynamicShadows");
      if (SerializationHelper.HasElement(info, "_bPreviewVisibility"))
        _bPreviewVisibility = info.GetBoolean("_bPreviewVisibility");
      if (SerializationHelper.HasElement(info, "_objectKey"))
        _objectKey = info.GetString("_objectKey");
      if (SerializationHelper.HasElement(info, "_vLightGridOfs"))
        _vLightGridOfs = (Vector3F)info.GetValue("_vLightGridOfs", typeof(Vector3F));
      if (SerializationHelper.HasElement(info, "_fLightmapMultiplier"))
        _fLightmapMultiplier = info.GetSingle("_fLightmapMultiplier");
      if (SerializationHelper.HasElement(info, "_lightmapOverride"))
        _lightmapOverride = info.GetString("_lightmapOverride");
      
      if (SerializationHelper.HasElement(info, "_meshFileLOD1"))
      {
        _meshFileLOD1 = info.GetString("_meshFileLOD1");
        if (!string.IsNullOrEmpty(_meshFileLOD1))
          _lodUsage = LODUsageType_e.Deprecated_1LOD;
        else
          _lodUsage = LODUsageType_e.FarClipOnly;
      }
      if (SerializationHelper.HasElement(info, "_fLODSwitchDist"))
        _fLODSwitchDist = info.GetSingle("_fLODSwitchDist");
      if (SerializationHelper.HasElement(info, "_fFarClipDist"))
        _fFarClipDist = info.GetSingle("_fFarClipDist");
      if (SerializationHelper.HasElement(info, "_iLocalIDLowRes2")) // version 2: must be unique
        _iLocalIDLowRes = info.GetUInt32("_iLocalIDLowRes2");

      if (SerializationHelper.HasElement(info, "_bUseLODFromFile"))
      {
        // migrate
        bool bUseLODFromFile = info.GetBoolean("_bUseLODFromFile");
        if (bUseLODFromFile)
          _lodUsage = LODUsageType_e.DefinitionInMeshFile;
      }
      else if (SerializationHelper.HasElement(info, "_lodUsage"))
      {
        _lodUsage = (LODUsageType_e)info.GetValue("_lodUsage", typeof(LODUsageType_e));
        _cachedLODIds = (uint[])info.GetValue("_cachedLODIds", typeof(uint[]));
        if (_cachedLODIds != null && ((_cachedLODIds.GetLength(0) == 0) || _cachedLODIds[0] != this.LocalID))
          _cachedLODIds = null; // fixup old, broken files
      }
      if (SerializationHelper.HasElement(info, "_lodDefinitionFile"))
        _lodDefinitionFile = info.GetString("_lodDefinitionFile");

      if (SerializationHelper.HasElement(info, "_collisionBehavior"))
      {
        _collisionBehavior = (CollisionBehavior_e)info.GetValue("_collisionBehavior", typeof(CollisionBehavior_e));
        _collisionLayer = info.GetByte("_collisionLayer");
        _subSystemId = info.GetByte("_subSystemId");
        _subSystemDontCollideWith = info.GetByte("_subSystemDontCollideWith");
        _systemGroup = info.GetUInt16("_systemGroup");
      }
      if (SerializationHelper.HasElement(info, "_weldingType"))
        _weldingType = (WeldingType_e)info.GetValue("_weldingType", typeof(WeldingType_e));
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

      info.AddValue("_meshFile", _meshFile);
      info.AddValue("_iVisibleBitmask", (uint)_iVisibleBitmask);
      info.AddValue("_iLightInfluenceBitmask", (uint)_iLightInfluenceBitmask);
      info.AddValue("_iCollisionBitmask", (uint)_iCollisionBitmask);

      info.AddValue("_bCastStaticShadows",_bCastStaticShadows);
      info.AddValue("_bCastDynamicShadows",_bCastDynamicShadows);
      info.AddValue("_bPreviewVisibility", _bPreviewVisibility);
      info.AddValue("_objectKey", _objectKey);
      info.AddValue("_vLightGridOfs", _vLightGridOfs);
      info.AddValue("_fLightmapMultiplier", _fLightmapMultiplier);

      if (!string.IsNullOrEmpty(_meshFileLOD1))
      info.AddValue("_meshFileLOD1", _meshFileLOD1);
      info.AddValue("_fLODSwitchDist", _fLODSwitchDist);
      info.AddValue("_fFarClipDist", _fFarClipDist);
      info.AddValue("_iLocalIDLowRes2", _iLocalIDLowRes); // use a new string (_iLocalIDLowRes2) because old version was not unique.
      info.AddValue("_lodUsage", _lodUsage);
      info.AddValue("_cachedLODIds", _cachedLODIds);
      if (!string.IsNullOrEmpty(_lodDefinitionFile))
        info.AddValue("_lodDefinitionFile", _lodDefinitionFile);

      info.AddValue("_collisionBehavior",_collisionBehavior);
      info.AddValue("_collisionLayer",_collisionLayer);
      info.AddValue("_subSystemId",_subSystemId);
      info.AddValue("_subSystemDontCollideWith",_subSystemDontCollideWith);
      info.AddValue("_systemGroup",_systemGroup);
      info.AddValue("_weldingType", _weldingType);

      if (!string.IsNullOrEmpty(_customMaterialSetName))
        info.AddValue("_customMaterialSetName", _customMaterialSetName);
      if (!string.IsNullOrEmpty(_lightmapOverride))
        info.AddValue("_lightmapOverride", _lightmapOverride);
      
    }

    public override void OnDeserialization()
    {
      base.OnDeserialization();
      if (EditorManager.Project != null) // may be null when it comes from clipboard
        OnMeshFileChanged();
      // not yet added?
      SafeCreateAllLinks();
    }

    #endregion

    #region GUI

    [Browsable(false)]
    public static string ICON_NAME
    {
      get
      {
        return Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\House.bmp");
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


    #region ICustomPrefabSerialization Members

    public object GetCustomPrefabSerializationObject()
    {
      return this._iLocalIDLowRes;
    }

    public void SetCustomPrefabSerializationObject(object customObj)
    {
      if (customObj is uint)
        _iLocalIDLowRes = (uint)customObj;
    }

    #endregion
  }

  #endregion

  #region LOD handling

  #region class LODSetup

  /// <summary>
  /// Defines a LOD setup for a static mesh. Essentially it is a list of LODEntry. It is shared across static mesh shapes. Usually the setup
  /// comes from a .lodxml file which saves the mesh files and switch distances in human readable form.
  /// 
  /// Inernally a LOD setup holds all meshes in the chain, including the original mesh file, but only the 1..n LOD instances are exposed to the UI and the file.
  /// </summary>
  public class LODSetup : List<LODEntry>
  {
    #region Nested class

    public class LODEntryDisplay : LODEntry
    {
      public LODEntryDisplay(string filename, float fClipDist)
      {
        _filename = filename;
        _clipDistance = fClipDist;
      }

      public LODEntryDisplay()
      {
      }

      public override string ToString()
      {
        if (string.IsNullOrEmpty(_filename))
          return "<no mesh selected>";
        return _filename;
      }

      [EditorAttribute(typeof(FilenameEditor), typeof(UITypeEditor))]
      [FileDialogFilter(new string[] { ".vmesh" })]
      [ThumbnailFormat(new string[] { @"{0}_data\Thumbnail.bmp", @"{0}_data\Thumbnail.jpg" })]
      [Description("Filename of the mesh file for this LOD")]
      public string Filename
      {
        get { return _filename; }
        set { _filename = value; }
      }

      [Description("Distance at which this mesh should be enabled")]
      public float SwitchDistance
      {
        get { return _clipDistance; }
        set { _clipDistance = value; }
      }
    }

    #endregion

    /// <summary>
    /// Constructor that takes the file ID. The file ID is a string either of type 'meshfile.vmesh' or 'meshfile.vmesh?definition.lodxml' (with question mark as separator)
    /// </summary>
    /// <param name="fileID"></param>
    public LODSetup(string fileID)
    {
      _fileID = fileID;
      CreateEntries();

    }

    /// <summary>
    /// Re-Creates all LOD entries in the list. If it is from file, it reloads the XML
    /// </summary>
    public void CreateEntries()
    {
      this.Clear();
      if (string.IsNullOrEmpty(_fileID))
        return;

      string[] meshAndLOD = _fileID.Split("?".ToCharArray());

      // the mesh itself is always LOD#0
      LODEntryDisplay entry = new LODEntryDisplay(meshAndLOD[0], 0.0f);
      this.Add(entry);

      if (meshAndLOD.Length == 2)
        LoadFromXML(meshAndLOD[0], meshAndLOD[1]);
    }

    /// <summary>
    /// Internal function to load the XML setup. Assumes the first entry is there
    /// </summary>
    /// <param name="meshfile"></param>
    /// <param name="xmlfile"></param>
    /// <returns></returns>
    bool LoadFromXML(string meshfile, string xmlfile)
    {
      string xmlFilename = EditorManager.Project.MakeAbsolute(meshfile + "_data\\" + xmlfile);
      if (!File.Exists(xmlFilename))
        return false;

      string meshpath = Path.GetDirectoryName(meshfile);
      XmlDocument doc = new XmlDocument();
      try
      {
        doc.Load(xmlFilename);
        XmlElement root = doc.DocumentElement;
        IEnumerator nodes = root.GetEnumerator();
        LODEntry previousEntry = this[0]; // must have been added before this function
        while (nodes.MoveNext())
        {
          XmlElement node = nodes.Current as XmlElement;
          if (node == null || node.Name != "LOD")
            continue;
          IEnumerator lodnodes = node.GetEnumerator();
          while (lodnodes.MoveNext())
          {
            XmlElement lodentry = lodnodes.Current as XmlElement;
            if (lodentry == null || lodentry.Name != "LODLevel")
              continue;
            string mesh = lodentry.GetAttribute("mesh");
            string distance = lodentry.GetAttribute("distance");
            float fDistance;
            if (string.IsNullOrEmpty(mesh) || string.IsNullOrEmpty(distance) || !float.TryParse(distance, out fDistance))
              continue;
            mesh = Path.Combine(meshpath, mesh);
            LODEntryDisplay entry = new LODEntryDisplay(mesh, 0.0f);
            previousEntry._clipDistance = fDistance; // this is the farclip of the previous entry
            this.Add(entry);
            previousEntry = entry;
          }
        }
      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex);
        return false;
      }

      Validate();
      return true;
    }

    /// <summary>
    /// Caled after editing to check whether the list is valid. Empty entries are sorted out
    /// </summary>
    public void Validate()
    {

      // sort out the empty meshes
      for (int i = Count - 1; i >= 0; i--)
        if (string.IsNullOrEmpty(this[i]._filename))
          RemoveAt(i);

      // guarantee that clip distances are ascending
      float fLastDist = 0.0f;
      for (int i = 0; i < Count; i++)
      {
        if (i == Count - 1) // the last one has no far clip by default
        {
          this[i]._clipDistance = 0.0f;
          continue;
        }
        if (this[i]._clipDistance < fLastDist)
          this[i]._clipDistance = fLastDist + 0.01f;
        fLastDist = this[i]._clipDistance;
      }
    }

    /// <summary>
    /// A special clone function that deep copies the list, removes the first entry and shifts the clip distance by one entry
    /// </summary>
    /// <returns></returns>
    public LODSetup CloneForEditing()
    {
      LODSetup copy = new LODSetup(null);
      copy.FileID = this.FileID;

      // leave out first one and shift the clip distances
      for (int i = 1; i < Count; i++)
        copy.Add(new LODEntryDisplay(this[i]._filename, this[i - 1]._clipDistance));

      return copy;
    }

    /// <summary>
    /// Save the setup to file
    /// </summary>
    /// <param name="meshfile"></param>
    /// <param name="xmlfile"></param>
    /// <returns></returns>
    public bool SaveToFile(string meshfile, string xmlfile)
    {
      this._fileID = meshfile + "?" + xmlfile;
      string xmlFilename = EditorManager.Project.MakeAbsolute(meshfile + "_data\\" + xmlfile);
      string meshpath = Path.GetDirectoryName(meshfile);
      string absmeshpath = Path.GetDirectoryName(EditorManager.Project.MakeAbsolute(meshfile));
      try
      {
        ManagedBase.RCS.GetProvider().EditFile(xmlFilename);
        XmlDocument doc = new XmlDocument();
        XmlElement root = doc.CreateElement("root");
        doc.AppendChild(root);
        XmlElement lodnodes = doc.CreateElement("LOD");

        for (int i = 1; i < Count; i++) // the first element is not written as it is the mesh itself
        {
          LODEntry e = this[i];
          XmlElement node = doc.CreateElement("LODLevel");

          // internally project relative, make it mesh relative
          string lodmesh = e._filename;
          if (!string.IsNullOrEmpty(lodmesh) && !lodmesh.StartsWith("\\"))
          {
            string newpath = FileHelper.MakePathRelative(absmeshpath, Path.GetDirectoryName(EditorManager.Project.MakeAbsolute(e._filename)));
            lodmesh = Path.Combine(newpath, Path.GetFileName(e._filename));
          }

          float fDistance = this[i - 1]._clipDistance; // far clip of the previous entry
          XmlAttribute a = doc.CreateAttribute("mesh");
          a.InnerText = lodmesh;
          node.Attributes.Append(a);
          a = doc.CreateAttribute("distance");
          a.InnerText = fDistance.ToString();
          node.Attributes.Append(a);

          lodnodes.AppendChild(node);
        }

        root.AppendChild(lodnodes);
        doc.Save(xmlFilename);
        ManagedBase.RCS.GetProvider().AddFile(xmlFilename, false /* Text file */);
      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex);
        return false;
      }
      return true;
    }

    /// <summary>
    /// Call this function whenever this setup has been changed externally. This fucntion will notify all static meshes
    /// </summary>
    public void OnChangedNotify()
    {
      LODManager.OnChangedNotify(this);
    }

    string _fileID;

    /// <summary>
    /// Return the file ID as passed to c'tor
    /// </summary>
    public string FileID
    {
      get { return _fileID; }
      set { _fileID = value; }
    }

    /// <summary>
    /// Definition name (e.g. default.lodxml) 
    /// </summary>
    [Browsable(false)]
    public string DefinitionName
    {
      get
      {
        string[] fileAndDef = _fileID.Split("?".ToCharArray());
        if (fileAndDef.Length == 2)
          return fileAndDef[1];
        return null;
      }
    }

    /// <summary>
    /// Mesh filename (project relative)
    /// </summary>
    [Browsable(false)]
    public string MeshFileName
    {
      get
      {
        string[] fileAndDef = _fileID.Split("?".ToCharArray());
        return fileAndDef[0];
      }
    }

    /// <summary>
    /// shows the chain as a string
    /// </summary>
    /// <returns></returns>
    public override string ToString()
    {
      string val = null;
      string separator = "";
      for (int i = 1; i < Count; i++)
      {
        val += string.Format("{0}{1} ({2})", separator, Path.GetFileName(this[i]._filename), this[i - 1]._clipDistance.ToString());
        separator = ", ";
      }
      return val;
    }
  }

  #region LODSetup_Deprecated1LOD

  /// <summary>
  ///  Helper class to distinguish an old-style compatibility setup (1 mesh and one LOD)
  /// </summary>
  public class LODSetup_Deprecated1LOD : LODSetup
  {
    public LODSetup_Deprecated1LOD(string meshfile, string lodfile, float fSwitchDist) :
      base(null)
    {
      Add(new LODEntryDisplay(meshfile, fSwitchDist));
      Add(new LODEntryDisplay(lodfile, 0.0f));
    }

    public float LODSwitchDistance
    {
      get
      {
        return this[0]._clipDistance;
      }
      set
      {
        this[0]._clipDistance = value;
      }
    }

    public string LowResFilename
    {
      get
      {
        return this[1]._filename;
      }
      set
      {
        this[1]._filename = value;
      }
    }
  }

  #endregion

  #endregion

  #region class LOD manager

  /// <summary>
  /// Singleton class with static helper functions to manage one global list of distinct LOD setups (at least one per static mesh)
  /// </summary>
  public class LODManager
  {
    private LODManager() { } // prevent instantiation


    /// <summary>
    /// Called upon new project load
    /// </summary>
    public static void ClearSetups()
    {
      Setups.Clear();
    }

    /// <summary>
    /// Find or create a setup with given ID
    /// </summary>
    /// <param name="fileID">A combination of mesh filename and definition name (separated by question mark)</param>
    /// <returns></returns>
    public static LODSetup GetSetup(string fileID)
    {
      LODSetup setup = null;
      if (Setups.TryGetValue(fileID, out setup))
        return setup;

      setup = new LODSetup(fileID);
      Setups.Add(fileID, setup);
      return setup;
    }


    #region Nested class Visitor

    /// <summary>
    /// Helper class to notify all static mesh instances in case a LOD setup has change (e.g. through editing)
    /// </summary>
    public class NotifyMeshesVisitor : IShapeVisitor
    {
      public NotifyMeshesVisitor(LODSetup setup)
      {
        _setup = setup;
      }
      LODSetup _setup;

      public override IShapeVisitor.VisitResult Visit(ShapeBase shape)
      {
        StaticMeshShape mesh = shape as StaticMeshShape;
        if (mesh != null && mesh.CurrentLODSetup == _setup)
          mesh.OnLODSetupChangedNotify();

        return VisitResult.VisitOk;
      }
    }

    #endregion

    /// <summary>
    /// Static function to invoke the visitor that notifies all static mesh shapes
    /// </summary>
    /// <param name="setup"></param>
    public static void OnChangedNotify(LODSetup setup)
    {
      NotifyMeshesVisitor visitor = new NotifyMeshesVisitor(setup);
      EditorManager.Scene.RunShapeVisitor(visitor, false, false);
    }

    #region class LODSetupEditingWrapper

    /// <summary>
    /// Helper construct to generate an object that has a LODSetup as a property. Needed to outsmart the collection editor
    /// </summary>
    public class LODSetupEditingWrapper
    {
      public LODSetup _setup = null;

      [EditorAttribute(typeof(TypedCollectionEditor), typeof(UITypeEditor)), EditableTypeAttribute(typeof(LODSetup.LODEntryDisplay))]
      public LODSetup LODSetup
      {
        get
        {
          return _setup;
        }
      }


      /// <summary>
      /// Wraps the whole editing process (open collection editor etc)
      /// </summary>
      /// <param name="existing"></param>
      /// <param name="meshfile"></param>
      /// <param name="xmlfile"></param>
      /// <returns></returns>
      public LODSetup EditCollection(LODSetup existing, string meshfile, string xmlfile)
      {
        if (existing != null)
          _setup = existing.CloneForEditing();
        else
          _setup = new LODSetup(null);

        PropertyDescriptor pd = TypeDescriptor.GetProperties(this)["LODSetup"];
        TypedCollectionEditor editor = (TypedCollectionEditor)pd.GetEditor(typeof(UITypeEditor));
        DefaultServiceProvider serviceProvider = new DefaultServiceProvider(this, pd);

        LODSetup editedSetup = (LODSetup)editor.EditValue(serviceProvider, serviceProvider, this.LODSetup);
        DialogResult res = editor.UsedCollectionForm.DialogResult;
        if (res != DialogResult.OK || editedSetup == null) // abort?
          return null;

        // this element #0 has to be there but it is typically not being edited
        if (editedSetup.Count == 0 || string.Compare(editedSetup[0]._filename, meshfile, true) != 0)
          editedSetup.Insert(0, new LODSetup.LODEntryDisplay(meshfile, 0.0f));

        // furthermore shift the distances so that clip distance means farclip distance
        for (int i = 0; i < editedSetup.Count - 1; i++)
          editedSetup[i]._clipDistance = editedSetup[i + 1]._clipDistance;
        editedSetup[editedSetup.Count - 1]._clipDistance = 0.0f; // last one up to infinity...

        editedSetup.Validate();
        return editedSetup;
      }
    }

    #endregion


    /// <summary>
    /// Static function to create and edit a new LOD definition file
    /// </summary>
    /// <param name="meshfile"></param>
    /// <param name="basedOn"></param>
    /// <returns></returns>
    public static LODSetup CreateNewSetupForMesh(string meshfile, LODSetup basedOn)
    {
      string meshPath = EditorManager.Project.MakeAbsolute(meshfile + "_data");

      // pick a new file in the vmesh_data subfolder
      CreateFileDlg dlg = new CreateFileDlg();
      dlg.Caption = "Create a new mesh LOD setup";
      dlg.Description = "Please enter a new filename for the target LOD definition file and click OK to continue";
      dlg.Text = "Create a new mesh LOD setup";
      dlg.InitialDirectory = meshPath;
      dlg.Ext = ".lodxml";
      dlg.Filter = new string[] { dlg.Ext };
      dlg.AllowOverwrite = true;
      if (dlg.ShowDialog() != DialogResult.OK)
        return null;

      string xmlfile = dlg.FileName;
      if (!xmlfile.StartsWith(meshPath))
        return null;

      xmlfile = xmlfile.Substring(meshPath.Length + 1);
      return EditSetup(meshfile, xmlfile, basedOn);
    }

    /// <summary>
    /// Static function to edit end resave the passed definition file
    /// </summary>
    /// <param name="meshfile"></param>
    /// <param name="xmlfile"></param>
    /// <param name="basedOn"></param>
    /// <returns></returns>
    public static LODSetup EditSetup(string meshfile, string xmlfile, LODSetup basedOn)
    {
      string fileID = meshfile + "?" + xmlfile;
      LODSetup existingSetup;
      if (!Setups.TryGetValue(fileID, out existingSetup))
        existingSetup = null;
      if (basedOn == null)
        basedOn = existingSetup;

      LODSetupEditingWrapper editor = new LODSetupEditingWrapper();
      LODSetup editedSetup = editor.EditCollection(basedOn, meshfile, xmlfile);
      if (editedSetup == null)
        return null;

      // save to file
      editedSetup.SaveToFile(meshfile, xmlfile);

      // ..and reload it (it must be loaded into the existing instance because that reference is shared by the shapes)
      if (existingSetup != null)
      {
        existingSetup.CreateEntries(); // force the existing instance to reload itself from file and notiffy all shapes
        OnChangedNotify(existingSetup);
        return existingSetup;
      }
      Setups.Add(editedSetup.FileID, editedSetup);
      return editedSetup;
    }

    static Dictionary<string, LODSetup> Setups = new Dictionary<string, LODSetup>();
  }

  #endregion

  #region LODDefinitionFilenameEditor

  /// <summary>
  /// UI editor for the LODDefinitionFilename property of the static mesh. Provides a dropdown with New, Browse and Edit capabilities
  /// </summary>
  public class LODDefinitionFilenameEditor : UITypeEditor
  {
    #region GroupFileMRUList

    class DefinitionFileMRUList
    {
      public DefinitionFileMRUList()
      {
        IProject.NewProjectLoaded += new EventHandler(IProject_NewProjectLoaded);
      }

      ~DefinitionFileMRUList()
      {
        IProject.NewProjectLoaded -= new EventHandler(IProject_NewProjectLoaded);
      }

      private void IProject_NewProjectLoaded(object sender, EventArgs e)
      {
        Filenames.Clear();
      }

      public StringCollection Filenames = new StringCollection();
    }

    #endregion

    /// <summary>
    /// Constructor
    /// </summary>
    public LODDefinitionFilenameEditor()
    {
      _listBox.SelectedIndexChanged += new EventHandler(_listBox_SelectedIndexChanged);
    }

    /// <summary>
    /// Default file extension for group files
    /// </summary>
    public static string DEFAULT_EXTENSION = ".lodxml";

    #region overridden UITypeEditor functions

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="context"></param>
    /// <returns></returns>
    [System.Security.Permissions.PermissionSet(System.Security.Permissions.SecurityAction.Demand, Name = "FullTrust")]
    public override UITypeEditorEditStyle GetEditStyle(System.ComponentModel.ITypeDescriptorContext context)
    {
      return UITypeEditorEditStyle.DropDown;
    }

    /// <summary>
    /// Service instances passed to the EditValue function
    /// </summary>
    private IWindowsFormsEditorService editorService = null;

    private ListBox _listBox = new ListBox();
    private int ITEM_NEW = -2;
    private int ITEM_BROWSE = -2;
    private int ITEM_EDIT = -2;
    private int ITEM_FIRST_MRUENTRY = -2;
    private string _fileName = null;
    private string _meshFilename = null;
    private LODSetup oldSetup = null;

    static DefinitionFileMRUList _mruList = new DefinitionFileMRUList();

    /// <summary>
    /// Overriden Edit value function
    /// </summary>
    /// <param name="context"></param>
    /// <param name="provider"></param>
    /// <param name="value"></param>
    /// <returns></returns>
    [RefreshProperties(RefreshProperties.All)]
    [System.Security.Permissions.PermissionSet(System.Security.Permissions.SecurityAction.Demand, Name = "FullTrust")]
    public override object EditValue(System.ComponentModel.ITypeDescriptorContext context, IServiceProvider provider, object value)
    {
      editorService = provider.GetService(typeof(IWindowsFormsEditorService)) as IWindowsFormsEditorService;
      _fileName = value as string;

      StaticMeshShape shape = context.Instance as StaticMeshShape;
      if (shape == null || string.IsNullOrEmpty(shape.MeshFileName))
        return value;
      _meshFilename = shape.MeshFileName;
      bool bInBaseData = _meshFilename.StartsWith("\\");
      oldSetup = shape.CurrentLODSetup;

      _listBox.Items.Clear();
      if (!bInBaseData)
        ITEM_NEW = _listBox.Items.Add("<New>");
      ITEM_BROWSE = _listBox.Items.Add("<Browse file>");
      if (!bInBaseData && !string.IsNullOrEmpty(_fileName))
        ITEM_EDIT = _listBox.Items.Add("<Edit>");

      _listBox.Items.Add("----------"); // separator
      foreach (string filename in _mruList.Filenames)
      {
        int iIndex = _listBox.Items.Add(filename);
        if (ITEM_FIRST_MRUENTRY < 0)
          ITEM_FIRST_MRUENTRY = iIndex;
      }

      editorService.DropDownControl(_listBox);
      return _fileName;
    }

    #endregion

    #region ListBox

    private void _listBox_SelectedIndexChanged(object sender, EventArgs e)
    {
      int iSelIndex = _listBox.SelectedIndex;

      // "create new" selected from dropdown 
      if (iSelIndex == ITEM_NEW)
      {
        editorService.CloseDropDown(); // do this first otherwise UI might hang

        LODSetup setup = LODManager.CreateNewSetupForMesh(_meshFilename, oldSetup);
        if (setup != null)
        {
          _fileName = setup.DefinitionName;
          AddFileNameToMRU(_fileName);
        }
        return;
      }

      // "browse file" selected from dropdown
      if (iSelIndex == ITEM_BROWSE)
      {
        editorService.CloseDropDown(); // do this first otherwise UI might hang
        string meshpath = EditorManager.Project.MakeAbsolute(_meshFilename + "_data");
        string relFilename = BrowseDefinitionFile(meshpath, _fileName);

        if (relFilename != null)
        {
          _fileName = relFilename;
          AddFileNameToMRU(_fileName);
        }
        return;
      }

      // edit the group file
      if (iSelIndex == ITEM_EDIT)
      {
        editorService.CloseDropDown(); // do this first otherwise UI might hang

        LODManager.EditSetup(_meshFilename, _fileName, null);
        return;
      }

      if (ITEM_FIRST_MRUENTRY > 0 && iSelIndex >= ITEM_FIRST_MRUENTRY)
      {
        _fileName = _mruList.Filenames[iSelIndex - ITEM_FIRST_MRUENTRY];
        AddFileNameToMRU(_fileName);
        editorService.CloseDropDown();
        return;
      }
    }

    #endregion

    #region Static helper functions

    public static string BrowseDefinitionFile(string abspath, string oldFilename)
    {
      OpenFileDlg dlg = new OpenFileDlg();
      dlg.Caption = "Open a LOD definition file";
      dlg.Description = "Select the LOD definition file";
      dlg.Text = "Open a LOD definition file";
      dlg.InitialDirectory = abspath;
      dlg.FileName = oldFilename;
      dlg.Filter = new string[] { DEFAULT_EXTENSION };
      dlg.ShowBaseData = false;
      if (dlg.ShowDialog() != DialogResult.OK)
        return null;
      if (!dlg.FileName.StartsWith(abspath))
      {
        EditorManager.ShowMessageBox("Can only select LOD definition files from the mesh's subdirectory", "Failed to load LOD definition file", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
        return null;
      }

      return dlg.RelativeFileName;
    }

    /// <summary>
    /// Helper function to add filename to MRU list
    /// </summary>
    /// <param name="filename"></param>
    static void AddFileNameToMRU(string filename)
    {
      if (_mruList.Filenames.Contains(filename))
      {
        _mruList.Filenames.Remove(filename);
      }
      _mruList.Filenames.Insert(0, filename);
    }

    #endregion
  }

  #endregion

  #endregion


  #region StaticMeshShape Creator Plugin

  class StaticMeshShapeCreator : CSharpFramework.IShapeCreatorPlugin
  {
    public StaticMeshShapeCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\House.bmp"), Color.Magenta);
      CategoryIconIndex = IconIndex;
    }

    public override string GetPluginName()
    {
      return "Static Mesh Instance";
    }

    public override string GetPluginCategory()
    {
      return "Geometry";
    }

    /// <summary>
    /// Returns a short description text
    /// </summary>
    /// <returns></returns>
    public override string GetPluginDescription()
    {
      return "This shape represents a single instance of a static mesh resource (.vmesh file)";
    }

    public override ShapeBase CreateShapeInstance()
    {
      StaticMeshShape shape = new StaticMeshShape("StaticMesh");
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
