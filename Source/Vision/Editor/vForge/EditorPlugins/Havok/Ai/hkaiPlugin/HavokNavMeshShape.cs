/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Drawing;
using System.IO;
using System.Runtime.Serialization;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing.Design;
using CSharpFramework;
using CSharpFramework.Shapes;
using CSharpFramework.Math;
using CSharpFramework.Scene;
using CSharpFramework.PropertyEditors;
using CSharpFramework.Serialization;
using ManagedFramework;
using HavokAiManaged;
using VisionEditorPlugin.Shapes;
using TerrainEditorPlugin.Shapes;

using System.Collections.Generic;
using System.Collections.Specialized;
using CSharpFramework.BaseTypes;
using CSharpFramework.Actions;
using CSharpFramework.DynamicProperties;
using HavokAiPanelDialogs;

#if USE_SPEEDTREE
using Speedtree5EditorPlugin;
using Speedtree5Managed;
using Speedtree6EditorPlugin;
using Speedtree6Managed;
#endif


namespace HavokAiEditorPlugin.Shapes
{
  #region class HavokNavMeshGlobalSettingsConverter

  /// <summary>
  /// Class that provides the dynamic dropdown list for the available navmesh global settings in scene classes
  /// </summary>
  public class HavokNavMeshGlobalSettingsConverter : StringConverter
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
      // get the AI panel
      string[] settingNames = null;
      HavokAiPanel aiPanel = HavokAiPanel.GetInstance();
      System.Diagnostics.Debug.Assert(aiPanel != null);

      settingNames = aiPanel.GetGlobalSettingsNameCollection();
      return new StandardValuesCollection(settingNames);
    }

    public override bool GetStandardValuesExclusive(ITypeDescriptorContext context)
    {
      return false;
    }
  }

  #endregion

  #region class HavokNavMeshShape

  [Serializable, ShapeTypeNiceName("Havok Navigation Mesh")]
  public class HavokNavMeshShape : Shape3D
  {
    #region Category Sorting Definitions

    protected const string CAT_NAVMESHGEN = "Nav Mesh Output Settings";
    protected const int CATORDER_NAVMESHGEN = Shape3D.LAST_CATEGORY_ORDER_ID + 1;
    protected const string CAT_NAVMESHGEN_MISC = "Nav Mesh Debug Settings";
    protected const int CATORDER_NAVMESHGEN_MISC = CATORDER_NAVMESHGEN + 1;
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_NAVMESHGEN_MISC;

    #endregion

    #region Constructor

    public HavokNavMeshShape(string name)
      : base(name)
    {
      AddHint(HintFlags_e.NoRotation);
      AddHint(HintFlags_e.NoScale);
      AddHint(HintFlags_e.RetainPositionAtCreation);
      AddHint(HintFlags_e.HideGizmo);
      AddHint(HintFlags_e.NoUserTransform);

      // check to see if there's HavokNavMeshGlobalSettings and default to that shape
      // get the AI panel
      HavokAiPanel aiPanel = HavokAiPanel.GetInstance();
      System.Diagnostics.Debug.Assert(aiPanel != null);

      HavokNavMeshGlobalSettings settings = aiPanel.GetOrCreateDefaultGlobalSettings();
      m_navMeshGlobalSettingsName = settings.Name;
    }

    #endregion

    #region Icon

    static int iIconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_navmesh.png"), Color.Magenta);
    public override int GetIconIndex()
    {
      return iIconIndex;
    }

    public override int GetSelectedIconIndex()
    {
      return iIconIndex;
    }

    #endregion

    #region Engine Instance

    EngineInstanceHavokNavMesh EngineNavMesh { get { return (EngineInstanceHavokNavMesh)_engineInstance; } }

    public override void CreateEngineInstance(bool bCreateChildren)
    {
      if (_engineInstance == null)
      {
        _engineInstance = new EngineInstanceHavokNavMesh();
        SetEngineInstanceBaseProperties(); // sets the navmesh generation settings

        // try to load navmesh
        EngineNavMesh.SetFilenameAndLoadNavMesh(m_navMeshFilename);
        EngineNavMesh.AddNavMeshToWorld();
      }

      base.CreateEngineInstance(bCreateChildren);
    }

    public override void RemoveEngineInstance(bool bRemoveChildren)
    {
      // only unload if the part of the zone unload, otherwise unload when removed from scene which should catch all other possibilities
      if (this.ParentZone != null && this.ParentZone.CurrentStatus == Zone.ZoneStatus.Unloading)
      {
        base.RemoveEngineInstance(bRemoveChildren);
      }

      // pretend that we have no instance to keep the instance. It is removed in OnRemoveFromScene
      IEngineShapeInstance _keepInst = _engineInstance;
      _engineInstance = null;

      base.RemoveEngineInstance(bRemoveChildren);

      _engineInstance = _keepInst;
    }

    public override void OnRemoveFromScene()
    {
      // finally delete our engine instance here
      if (this._engineInstance != null)
      {
        // Important: We must call the base implementation here because the overload in this class prevents the deletion
        base.RemoveEngineInstance(false);
      }

      base.OnRemoveFromScene();
    }

    public override ShapeBase Clone()
    {
      HavokNavMeshShape copy = (HavokNavMeshShape)base.Clone();
      copy.m_navMeshFilename = null; // don't copy the filename since it has to be unique
      return copy;
    }

    public override void SetEngineInstanceBaseProperties()
    {
      base.SetEngineInstanceBaseProperties();
      if (!HasEngineInstance())
        return;

      // Nav Mesh Generation Settings
      HavokNavMeshGlobalSettings globalSettings = GetGlobalSettings();
      if (globalSettings == null)
        return;

      EngineNavMesh.m_characterHeight = globalSettings.CharacterHeight;
      EngineNavMesh.m_maxWalkableSlope = globalSettings.MaxWalkableSlope / 180.0f * 3.14159f;
      EngineNavMesh.m_minRegionArea = globalSettings.MinRegionArea;
      EngineNavMesh.m_minDistanceToSeedPoints = globalSettings.MinDistanceToSeedPoints;
      EngineNavMesh.m_borderPreservationTolerance = globalSettings.BorderPreservationTolerance;
      EngineNavMesh.m_minCharacterWidth = globalSettings.MinCharacterWidth;
      EngineNavMesh.m_characterWidthUsage = (int)globalSettings.CharacterWidthUsage;
      EngineNavMesh.m_terrainLOD = (int)globalSettings.TerrainLevelOfDetail;

      // Nav Mesh Generation Settings (Advanced)
      EngineNavMesh.m_quantizationGridSize = globalSettings.QuantizationGridSize;
      EngineNavMesh.m_degenerateAreaThreshold = globalSettings.DegenerateAreaThreshold;
      EngineNavMesh.m_degenerateWidthThreshold = globalSettings.DegenerateWidthThreshold;
      EngineNavMesh.m_convexThreshold = globalSettings.ConvexThreshold;
      EngineNavMesh.m_maxNumEdgesPerFace = globalSettings.MaxNumEdgesPerFace;
      EngineNavMesh.m_weldInputVertices = globalSettings.WeldInputVertices;
      EngineNavMesh.m_weldThreshold = globalSettings.WeldThreshold;

      // Nav Mesh Edge Matching Settings
      EngineNavMesh.m_edgeConnectionIterations = globalSettings.EdgeConnectionIterations;
      EngineNavMesh.m_maxStepHeight = globalSettings.MaxStepHeight;
      EngineNavMesh.m_maxSeparation = globalSettings.MaxSeparation;
      EngineNavMesh.m_maxOverhang = globalSettings.MaxOverhang;
      EngineNavMesh.m_cosPlanarAlignmentAngle = (float)Math.Cos(globalSettings.PlanarAlignmentAngle / 180.0f * 3.14159f);
      EngineNavMesh.m_cosVerticalAlignmentAngle = (float)Math.Cos(globalSettings.VerticalAlignmentAngle / 180.0f * 3.14159f);
      EngineNavMesh.m_minEdgeOverlap = globalSettings.MinEdgeOverlap;

      // Nav Mesh Simplification Settings
      EngineNavMesh.m_enableSimplification = globalSettings.EnableSimplification;
      EngineNavMesh.m_maxBorderSimplifyArea = globalSettings.MaxBorderSimplifyArea;
      EngineNavMesh.m_maxConcaveBorderSimplifyArea = globalSettings.MaxConcaveBorderSimplifyArea;
      EngineNavMesh.m_useHeightPartitioning = globalSettings.UseHeightPartitioning;
      EngineNavMesh.m_maxPartitionHeightError = globalSettings.MaxPartitionHeightError;

      // Nav Mesh Simplification Settings (Advanced)
      EngineNavMesh.m_minCorridorWidth = globalSettings.MinCorridorWidth;
      EngineNavMesh.m_maxCorridorWidth = globalSettings.MaxCorridorWidth;
      EngineNavMesh.m_holeReplacementArea = globalSettings.HoleReplacementArea;
      EngineNavMesh.m_maxLoopShrinkFraction = globalSettings.MaxLoopShrinkFraction;
      EngineNavMesh.m_maxBorderHeightError = globalSettings.MaxBorderHeightError;
      EngineNavMesh.m_maxBorderDistanceError = globalSettings.MaxBorderDistanceError;
      EngineNavMesh.m_maxPartitionSize = globalSettings.MaxPartitionSize;
      EngineNavMesh.m_useConservativeHeightPartitioning = globalSettings.UseConservativeHeightPartitioning;
      EngineNavMesh.m_hertelMehlhornHeightError = globalSettings.HertelMehlhornHeightError;
      EngineNavMesh.m_cosPlanarityThreshold = (float)Math.Cos(globalSettings.PlanarityThreshold / 180 * 3.14159f);
      EngineNavMesh.m_nonconvexityThreshold = globalSettings.NonconvexityThreshold;
      EngineNavMesh.m_boundaryEdgeFilterThreshold = globalSettings.BoundaryEdgeFilterThreshold;
      EngineNavMesh.m_maxSharedVertexHorizontalError = globalSettings.MaxSharedVertexHorizontalError;
      EngineNavMesh.m_maxSharedVertexVerticalError = globalSettings.MaxSharedVertexVerticalError;
      EngineNavMesh.m_maxBoundaryVertexHorizontalError = globalSettings.MaxBoundaryVertexHorizontalError;
      EngineNavMesh.m_maxBoundaryVertexVerticalError = globalSettings.MaxBoundaryVertexVerticalError;
      EngineNavMesh.m_mergeLongestEdgesFirst = globalSettings.MergeLongestEdgesFirst;

      // Nav Mesh Split Generation Settings
      EngineNavMesh.m_numTilesX = globalSettings.NumTilesX;
      EngineNavMesh.m_numTilesY = globalSettings.NumTilesY;
      EngineNavMesh.m_splitGenerationMethod = (int)globalSettings.SplitGenerationMethod;
      EngineNavMesh.m_shelverType = (int)globalSettings.ShelverType;
    }

    #endregion

    #region Serialization and Export

    protected HavokNavMeshShape(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      m_saveInputSnapshot = info.GetBoolean("SaveInputSnapshot");
      m_snapshotFilename = info.GetString("SnapshotFilename");
      if (SerializationHelper.HasElement(info, "NavmeshFilename"))
        m_navMeshFilename = info.GetString("NavmeshFilename");
      if (SerializationHelper.HasElement(info, "DebugRenderOffset"))
        m_debugRenderOffset = info.GetSingle("DebugRenderOffset");

      // check if it has a reference to a global settings shape already
      if (SerializationHelper.HasElement(info, "NavMeshGlobalSettingsName"))
      {
        m_navMeshGlobalSettingsName = info.GetString("NavMeshGlobalSettingsName");
      }
      else
      {
        // get the AI panel
        HavokAiPanel aiPanel = HavokAiPanel.GetInstance();
        System.Diagnostics.Debug.Assert(aiPanel != null);

        // if not, this is from an old version of the scene, so create a new settings struct and transfer all settings to this struct
        HavokNavMeshGlobalSettings globalSettings = aiPanel.CreateGlobalSettings(this.ToString() + "_GlobalSettings", false);
        m_navMeshGlobalSettingsName = globalSettings.Name;
        System.Diagnostics.Debug.Assert(globalSettings != null);

        // Nav Mesh Generation Settings
        globalSettings.CharacterHeight = info.GetSingle("CharacterHeight");
        globalSettings.MaxWalkableSlope = info.GetSingle("MaxWalkableSlope");
        globalSettings.MinRegionArea = info.GetSingle("MinRegionArea");
        if (SerializationHelper.HasElement(info, "MinDistanceToSeedPoints"))
          globalSettings.MinDistanceToSeedPoints = info.GetSingle("MinDistanceToSeedPoints");
        if (SerializationHelper.HasElement(info, "BorderPreservationTolerance"))
          globalSettings.BorderPreservationTolerance = info.GetSingle("BorderPreservationTolerance");
        globalSettings.MinCharacterWidth = info.GetSingle("MinCharacterWidth");
        globalSettings.CharacterWidthUsage = (eCharacterWidthUsage)info.GetValue("CharacterWidthUsage", typeof(eCharacterWidthUsage));
        if (SerializationHelper.HasElement(info, "RestrictToZoneGeometry"))
          globalSettings.RestrictToInputGeometryFromSameZone = info.GetBoolean("RestrictToZoneGeometry");

        // Nav Mesh Generation Settings (Advanced)
        globalSettings.QuantizationGridSize = info.GetSingle("QuantizationGridSize");
        globalSettings.DegenerateAreaThreshold = info.GetSingle("DegenerateAreaThreshold");
        globalSettings.DegenerateWidthThreshold = info.GetSingle("DegenerateWidthThreshold");
        globalSettings.ConvexThreshold = info.GetSingle("ConvexThreshold");
        globalSettings.MaxNumEdgesPerFace = info.GetInt32("MaxNumEdgesPerFace");
        globalSettings.WeldInputVertices = info.GetBoolean("WeldInputVertices");
        globalSettings.WeldThreshold = info.GetSingle("WeldThreshold");

        // Nav Mesh Edge Matching Settings
        globalSettings.EdgeConnectionIterations = info.GetInt32("EdgeConnectionIterations");
        globalSettings.EdgeMatchingMetric = (eEdgeMatchingMetric)info.GetValue("EdgeMatchingMetric", typeof(eEdgeMatchingMetric));
        globalSettings.MaxStepHeight = info.GetSingle("MaxStepHeight");
        globalSettings.MaxSeparation = info.GetSingle("MaxSeparation");
        globalSettings.MaxOverhang = info.GetSingle("MaxOverhang");
        globalSettings.PlanarAlignmentAngle = info.GetSingle("PlanarAlignmentAngle");
        globalSettings.VerticalAlignmentAngle = info.GetSingle("VerticalAlignmentAngle");
        globalSettings.MinEdgeOverlap = info.GetSingle("MinEdgeOverlap");

        // Nav Mesh Simplification Settings
        globalSettings.EnableSimplification = info.GetBoolean("EnableSimplification");
        globalSettings.MaxBorderSimplifyArea = info.GetSingle("MaxBorderSimplifyArea");
        globalSettings.MaxConcaveBorderSimplifyArea = info.GetSingle("MaxConcaveBorderSimplifyArea");
        globalSettings.UseHeightPartitioning = info.GetBoolean("UseHeightPartitioning");
        globalSettings.MaxPartitionHeightError = info.GetSingle("MaxPartitionHeightError");

        // Nav Mesh Simplification Settings (Advanced)
        globalSettings.MinCorridorWidth = info.GetSingle("MinCorridorWidth");
        globalSettings.MaxCorridorWidth = info.GetSingle("MaxCorridorWidth");
        globalSettings.HoleReplacementArea = info.GetSingle("HoleReplacementArea");
        globalSettings.MaxLoopShrinkFraction = info.GetSingle("MaxLoopShrinkFraction");
        globalSettings.MaxBorderHeightError = info.GetSingle("MaxBorderHeightError");
        globalSettings.MaxBorderDistanceError = info.GetSingle("MaxBorderDistanceError");
        globalSettings.MaxPartitionSize = info.GetInt32("MaxPartitionSize");
        globalSettings.UseConservativeHeightPartitioning = info.GetBoolean("UseConservativeHeightPartitioning");
        globalSettings.HertelMehlhornHeightError = info.GetSingle("HertelMehlhornHeightError");
        globalSettings.PlanarityThreshold = info.GetSingle("PlanarityThreshold");
        globalSettings.NonconvexityThreshold = info.GetSingle("NonconvexityThreshold");
        globalSettings.BoundaryEdgeFilterThreshold = info.GetSingle("BoundaryEdgeFilterThreshold");
        globalSettings.MaxSharedVertexHorizontalError = info.GetSingle("MaxSharedVertexHorizontalError");
        globalSettings.MaxSharedVertexVerticalError = info.GetSingle("MaxSharedVertexVerticalError");
        globalSettings.MaxBoundaryVertexHorizontalError = info.GetSingle("MaxBoundaryVertexHorizontalError");
        globalSettings.MaxBoundaryVertexVerticalError = info.GetSingle("MaxBoundaryVertexVerticalError");
        globalSettings.MergeLongestEdgesFirst = info.GetBoolean("MergeLongestEdgesFirst");

        // Nav Mesh Split Generation Settings
        if (SerializationHelper.HasElement(info, "NumTilesX"))
          globalSettings.NumTilesX = info.GetInt32("NumTilesX");
        if (SerializationHelper.HasElement(info, "NumTilesY"))
          globalSettings.NumTilesY = info.GetInt32("NumTilesY");
        if (SerializationHelper.HasElement(info, "SplitGenerationMethod"))
          globalSettings.SplitGenerationMethod = (eSplitGenerationMethod)info.GetValue("SplitGenerationMethod", typeof(eSplitGenerationMethod));
        if (SerializationHelper.HasElement(info, "ShelverType"))
          globalSettings.ShelverType = (eShelverType)info.GetValue("ShelverType", typeof(eShelverType));

        // Nav Mesh Link Settings
        if (SerializationHelper.HasElement(info, "LinkEdgeMatchTolerance"))
          globalSettings.LinkEdgeMatchTolerance = info.GetSingle("LinkEdgeMatchTolerance");

        if (SerializationHelper.HasElement(info, "LinkMaxStepHeight"))
          globalSettings.LinkMaxStepHeight = info.GetSingle("LinkMaxStepHeight");

        if (SerializationHelper.HasElement(info, "LinkMaxSeparation"))
          globalSettings.LinkMaxSeparation = info.GetSingle("LinkMaxSeparation");

        if (SerializationHelper.HasElement(info, "LinkMaxOverhang"))
          globalSettings.LinkMaxOverhang = info.GetSingle("LinkMaxOverhang");     

        if (SerializationHelper.HasElement(info, "LinkPlanarAlignmentAngle"))
          globalSettings.LinkPlanarAlignmentAngle = info.GetSingle("LinkPlanarAlignmentAngle");

        if (SerializationHelper.HasElement(info, "LinkVerticalAlignmentAngle"))
          globalSettings.LinkVerticalAlignmentAngle = info.GetSingle("LinkVerticalAlignmentAngle");

        if (SerializationHelper.HasElement(info, "LinkMinEdgeOverlap"))
          globalSettings.LinkMinEdgeOverlap = info.GetSingle("LinkMinEdgeOverlap");
      }
    }

    public override CSharpFramework.Serialization.EditorPluginInfo GetPluginInformation()
    {
      return EditorPlugin.EDITOR_PLUGIN_INFO;
    }

    public override void GetObjectData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context)
    {
      base.GetObjectData(info, context);

      // Nav Mesh Generation Settings
      info.AddValue("NavmeshFilename", m_navMeshFilename);
      info.AddValue("NavMeshGlobalSettingsName", m_navMeshGlobalSettingsName);

      // debug snapshot
      info.AddValue("SaveInputSnapshot", m_saveInputSnapshot);
      info.AddValue("SnapshotFilename", m_snapshotFilename);

      // debug display
      info.AddValue("DebugRenderOffset", m_debugRenderOffset);
    }

    public override bool OnExport(CSharpFramework.Scene.SceneExportInfo info)
    {
      return base.OnExport(info);
    }

    #endregion

    #region Properties

    // Nav Mesh Generation Settings
    string m_navMeshFilename = @""; // gets autogenerated  whenever it gets saved to disk
    string m_navMeshGlobalSettingsName = @"";
    bool m_saveInputSnapshot = false;
    string m_snapshotFilename = @"snapshot.hkt";
    float m_debugRenderOffset = 1.75f * 0.05f;

    [PropertyOrder(1)]
    [Description("Base filename of the output navmesh(es).")]
    [SortedCategory(CAT_NAVMESHGEN, CATORDER_NAVMESHGEN)]
    public string NavmeshFilename
    {
      get { return m_navMeshFilename; }
      set 
      {
        m_navMeshFilename = value;

        // reload from disk
        if (HasEngineInstance())
        {
          EngineNavMesh.SetFilenameAndLoadNavMesh(m_navMeshFilename);
          EngineNavMesh.AddNavMeshToWorld();
        }
      }
    }

    [SortedCategory(CAT_NAVMESHGEN, CATORDER_NAVMESHGEN),
    PropertyOrder(2),
    TypeConverter(typeof(HavokNavMeshGlobalSettingsConverter)),
    Description("Specify the nav mesh global settings configuration you want to use")]
    public string NavMeshGlobalSettingsKey
    {
      get { return m_navMeshGlobalSettingsName; }
      set { m_navMeshGlobalSettingsName = value; }
    }

    [PropertyOrder(3)]
    [Description("If this flag is set, the input data (hkGeometry and settings) will be serialized out.")]
    [SortedCategory(CAT_NAVMESHGEN_MISC, CATORDER_NAVMESHGEN_MISC)]
    public bool SaveInputSnapshot
    {
      get { return m_saveInputSnapshot; }
      set { m_saveInputSnapshot = value; }
    }

    [PropertyOrder(4)]
    [Description("Base filename of the saved input snapshot.")]
    [SortedCategory(CAT_NAVMESHGEN_MISC, CATORDER_NAVMESHGEN_MISC)]
    [EditorAttribute(typeof(FilenameCreator), typeof(UITypeEditor)), FileDialogFilter(new string[] { ".hkt" }), FileCreateDialogAllowOverwrite(true)]
    public string SnapshotFilename
    {
        get { return m_snapshotFilename; }
        set { m_snapshotFilename = value; }
    }

    [PropertyOrder(5)]
    [Description("The amount (in meters) to vertically offset the debug rendering of the nav mesh")]
    [SortedCategory(CAT_NAVMESHGEN_MISC, CATORDER_NAVMESHGEN_MISC)]
    public float DebugRenderOffset
    {
      get { return m_debugRenderOffset; }
      set { m_debugRenderOffset = value; }
    }

    private HavokNavMeshGlobalSettings GetGlobalSettings()
    {
      HavokNavMeshGlobalSettings settings = null;

      // get the AI panel
      HavokAiPanel aiPanel = HavokAiPanel.GetInstance();
      System.Diagnostics.Debug.Assert(aiPanel != null);

      settings = aiPanel.GetGlobalSettings(m_navMeshGlobalSettingsName);
      return settings;
    }

    public override List<PropertyDescriptor> GetAdditionalRootProperties(ITypeDescriptorContext context)
    {
      List<PropertyDescriptor> list = base.GetAdditionalRootProperties(context);
      if (this.NavMeshGlobalSettingsKey != null)
      {
        // add the additional properties from the NavMeshGlobalSettings
        HavokNavMeshGlobalSettings globalSettings = GetGlobalSettings();
        List<PropertyDescriptor> settingsList = globalSettings.GetCachedPropertiesList();
        if (settingsList != null)
        {
          if (list == null)
          {
            list = settingsList;
          }
          else
          {
            list.AddRange(settingsList);
          }
        }
      }
      return list;
    }

    #endregion

    #region Build nav mesh

    public bool Build(ShapeCollection staticGeometries, ref int numGeometryVertices, ref int numGeometryTriangles)
    {
      if (!HasEngineInstance())
        return false;

      EngineNavMesh.ClearNavMesh();
      EngineNavMesh.ClearGeometry();
      EngineNavMesh.ClearCarvers();
      EngineNavMesh.ClearSeedPoints();
      EngineNavMesh.ClearLocalSettings();
      EngineNavMesh.ClearDecorationCapsules();
      SetEngineInstanceBaseProperties();

      HavokNavMeshGlobalSettings globalSettings = GetGlobalSettings();
      if (globalSettings == null)
      {
        
        return false;
      }

      BoundingBox parentZoneBbox = new BoundingBox();
      if (globalSettings.RestrictToInputGeometryFromSameZone && ParentZone != null)
      {
        parentZoneBbox = ParentZone.CalculateBoundingBox();
      }

      // check if there's a parent zone
      foreach (ShapeBase shape in staticGeometries)
      {
        // treat as cutter if flag is set and if in different zone
        bool potentiallyTreatAsCutter = globalSettings.RestrictToInputGeometryFromSameZone && (shape.ParentZone != ParentZone);

        if (shape is EntityShape)
        {
          EntityShape entity = shape as EntityShape;
          eNavMeshUsage usage = entity.GetNavMeshUsage();

          // exclude entities that have a vHavokRigidBody component with "Motion Type" != "Fixed"
          ShapeComponentType compType = (ShapeComponentType)EditorManager.EngineManager.ComponentClassManager.GetCollectionType("vHavokRigidBody");
          if (compType != null && entity.Components != null)
          {
            ShapeComponent comp = entity.Components.GetComponentByType(compType);
            if (comp != null)
            {
              string propValue = comp.GetPropertyValue("Motion Type") as string;
              if (string.Compare(propValue, "Fixed") != 0)
              {
                usage = eNavMeshUsage.ExcludeFromNavMesh;
              }
            }
          }

          // potentially override usage 
          if (potentiallyTreatAsCutter && (usage == eNavMeshUsage.IncludeInNavMesh))
          {
            usage = eNavMeshUsage.CutterOnly;
          }

          if (usage != eNavMeshUsage.ExcludeFromNavMesh)
          {
            EngineNavMesh.AddEntityGeometry(entity.EngineEntity.GetNativeObject(), (int)usage, parentZoneBbox);
          }
        }
        else if (shape is StaticMeshShape)
        {
          StaticMeshShape staticMesh = shape as StaticMeshShape;
          eNavMeshUsage usage = staticMesh.GetNavMeshUsage();

          // potentially override usage 
          if (potentiallyTreatAsCutter && (usage == eNavMeshUsage.IncludeInNavMesh))
          {
            usage = eNavMeshUsage.CutterOnly;
          }


          if (usage != eNavMeshUsage.ExcludeFromNavMesh)
          {
            EngineNavMesh.AddStaticMeshGeometry(staticMesh.EngineMesh.GetNativeObject(), (int)usage, parentZoneBbox);
          }
        }
        else if (shape is TerrainShape)
        {
          TerrainShape terrain = shape as TerrainShape;
          eNavMeshUsage usage = terrain.GetNavMeshUsage();

          // potentially override usage 
          if (potentiallyTreatAsCutter && (usage == eNavMeshUsage.IncludeInNavMesh))
          {
            usage = eNavMeshUsage.CutterOnly;
          }


          if (usage != eNavMeshUsage.ExcludeFromNavMesh)
          {
            EngineNavMesh.AddTerrainGeometry(terrain.EngineTerrain.GetNativeObject(), (int)usage, parentZoneBbox);
          }
        }
#if USE_SPEEDTREE
        else if (shape is SpeedTree5GroupShape)
        {
          SpeedTree5GroupShape trees = shape as SpeedTree5GroupShape;
          if (trees.EnableCollisions)
          {
            EngineNavMesh.AddSpeedTree5Capsules(trees.EngineGroup.GetGroupsObject());
          }
        }
        else if (shape is Speedtree6GroupShape)
        {
          Speedtree6GroupShape trees = shape as Speedtree6GroupShape;
          if (trees.EnableCollisions)
          {
            EngineNavMesh.AddSpeedTree6Capsules(trees.EngineGroup.GetGroupsObject());
          }
        }
#endif
      }
      numGeometryVertices = EngineNavMesh.GetNumGeometryVertices();
      numGeometryTriangles = EngineNavMesh.GetNumGeometryTriangles();

      // Add carvers
      ShapeCollection carvers = EditorManager.Scene.AllShapesOfType(typeof(HavokNavMeshCarverShape));
      foreach (ShapeBase shape in carvers)
      {
        HavokNavMeshCarverShape carver = shape as HavokNavMeshCarverShape;
        BoundingBox localBbox = null;
        carver.GetLocalBoundingBox(ref localBbox);
        localBbox.vMin.X *= carver.ScaleX;
        localBbox.vMin.Y *= carver.ScaleY;
        localBbox.vMin.Z *= carver.ScaleZ;
        localBbox.vMax.X *= carver.ScaleX;
        localBbox.vMax.Y *= carver.ScaleY;
        localBbox.vMax.Z *= carver.ScaleZ;
        EngineNavMesh.AddBoxCarver(localBbox.vMin, localBbox.vMax, carver.Position, carver.RotationMatrix, carver.IsInverted());
      }

      // Add seed points
      ShapeCollection seedPoints = EditorManager.Scene.AllShapesOfType(typeof(HavokNavMeshSeedPointShape));
      foreach (ShapeBase shape in seedPoints)
      {
        HavokNavMeshSeedPointShape seedPoint = shape as HavokNavMeshSeedPointShape;
        EngineNavMesh.AddSeedPoint(seedPoint.Position);
      }

      // Add local settings
      ShapeCollection localSettings = EditorManager.Scene.AllShapesOfType(typeof(HavokNavMeshLocalSettingsShape));
      foreach (ShapeBase shape in localSettings)
      {
        HavokNavMeshLocalSettingsShape ls = shape as HavokNavMeshLocalSettingsShape;
        BoundingBox bbox = null;
        ls.GetLocalBoundingBox(ref bbox);
        bbox.vMin.X *= ls.ScaleX;
        bbox.vMin.Y *= ls.ScaleY;
        bbox.vMin.Z *= ls.ScaleZ;
        bbox.vMax.X *= ls.ScaleX;
        bbox.vMax.Y *= ls.ScaleY;
        bbox.vMax.Z *= ls.ScaleZ;

        // Nav Mesh Generation Settings
        EngineNavMesh.m_maxWalkableSlope = ls.MaxWalkableSlope / 180.0f * 3.14159f;

        // Nav Mesh Edge Matching Settings
        EngineNavMesh.m_maxStepHeight = ls.MaxStepHeight;
        EngineNavMesh.m_maxSeparation = ls.MaxSeparation;
        EngineNavMesh.m_maxOverhang = ls.MaxOverhang;
        EngineNavMesh.m_cosPlanarAlignmentAngle = (float)Math.Cos(ls.PlanarAlignmentAngle / 180.0f * 3.14159f);
        EngineNavMesh.m_cosVerticalAlignmentAngle = (float)Math.Cos(ls.VerticalAlignmentAngle / 180.0f * 3.14159f);
        EngineNavMesh.m_minEdgeOverlap = ls.MinEdgeOverlap;

        // Nav Mesh Simplification Settings
        EngineNavMesh.m_maxBorderSimplifyArea = ls.MaxBorderSimplifyArea;
        EngineNavMesh.m_maxConcaveBorderSimplifyArea = ls.MaxConcaveBorderSimplifyArea;
        EngineNavMesh.m_useHeightPartitioning = ls.UseHeightPartitioning;
        EngineNavMesh.m_maxPartitionHeightError = ls.MaxPartitionHeightError;

        // Nav Mesh Simplification Settings (Advanced)
        EngineNavMesh.m_minCorridorWidth = ls.MinCorridorWidth;
        EngineNavMesh.m_maxCorridorWidth = ls.MaxCorridorWidth;
        EngineNavMesh.m_holeReplacementArea = ls.HoleReplacementArea;
        EngineNavMesh.m_maxLoopShrinkFraction = ls.MaxLoopShrinkFraction;
        EngineNavMesh.m_maxBorderHeightError = ls.MaxBorderHeightError;
        EngineNavMesh.m_maxBorderDistanceError = ls.MaxBorderDistanceError;
        EngineNavMesh.m_maxPartitionSize = ls.MaxPartitionSize;
        EngineNavMesh.m_useConservativeHeightPartitioning = ls.UseConservativeHeightPartitioning;
        EngineNavMesh.m_hertelMehlhornHeightError = ls.HertelMehlhornHeightError;
        EngineNavMesh.m_cosPlanarityThreshold = (float)Math.Cos(ls.PlanarityThreshold / 180 * 3.14159f);
        EngineNavMesh.m_nonconvexityThreshold = ls.NonconvexityThreshold;
        EngineNavMesh.m_boundaryEdgeFilterThreshold = ls.BoundaryEdgeFilterThreshold;
        EngineNavMesh.m_maxSharedVertexHorizontalError = ls.MaxSharedVertexHorizontalError;
        EngineNavMesh.m_maxSharedVertexVerticalError = ls.MaxSharedVertexVerticalError;
        EngineNavMesh.m_maxBoundaryVertexHorizontalError = ls.MaxBoundaryVertexHorizontalError;
        EngineNavMesh.m_maxBoundaryVertexVerticalError = ls.MaxBoundaryVertexVerticalError;
        EngineNavMesh.m_mergeLongestEdgesFirst = ls.MergeLongestEdgesFirst;

        EngineNavMesh.AddLocalSettings(bbox.vMin, bbox.vMax, ls.Position, ls.RotationMatrix);
      }

      // todo: this is a hack until I figure out how to pass class instances between here and EngineNavMesh.
      // basically the settings members of EngineNavMesh are reused for transferring the local settings to
      // EngineNavMesh. this should be harmless due to the following call which will revert any changes.
      SetEngineInstanceBaseProperties();

      string fullSnapshotPath = Path.Combine(CSharpFramework.EditorManager.Scene.Project.ProjectDir, m_snapshotFilename);
      bool ret = EngineNavMesh.BuildNavMeshFromGeometry(m_saveInputSnapshot, fullSnapshotPath);

      EngineNavMesh.ClearGeometry();
      EngineNavMesh.ClearCarvers();
      EngineNavMesh.ClearSeedPoints();
      EngineNavMesh.ClearLocalSettings();
      EngineNavMesh.ClearDecorationCapsules();
      return ret;
    }

    #endregion

    #region Save nav mesh to file

    public bool SaveNavMeshesToFile()
    {
      if (!HasEngineInstance())
        return false;


      if (m_navMeshFilename == null || m_navMeshFilename.Length == 0)
      {
        // autogenerate a name based on shape UniqueID

        // define navmesh directory relative to project
        string navMeshDirectoryRelativeToProjectDir = EditorManager.Scene.FileNameNoExt + ".NavMeshData\\";

        // create directory if it doesn't exist
        string navMeshDirectoryAbsolute = Path.Combine(EditorManager.Project.ProjectDir, navMeshDirectoryRelativeToProjectDir);
        if (!Directory.Exists(navMeshDirectoryAbsolute))
        {
          Directory.CreateDirectory(navMeshDirectoryAbsolute);
        }

        string fileName = String.Format("navmesh_{0:X}.hkt", UniqueID);

        // store filepath relative to project dir since project dir is one of the standard data directories that get searched for loading files later
        m_navMeshFilename = navMeshDirectoryRelativeToProjectDir + fileName;
      }
      bool success = EngineNavMesh.SetFilenameAndSaveNavMesh(m_navMeshFilename, EditorManager.Project.ProjectDir);

      if (success)
      {
        // call this since we change the filename.
        Modified = true;
      }
      return success;
    }

    #endregion

    #region Add nav mesh to ai world

    public void AddNavMeshToWorld()
    {
      if (HasEngineInstance())
      {
        EngineNavMesh.AddNavMeshToWorld();
      }
    }

    #endregion

    #region Misc

    public override void RenderShape(VisionViewBase view, ShapeRenderMode mode)
    {
      if (HasEngineInstance() && FinalVisibleState == true)
      {
        EngineNavMesh.DebugRender(m_debugRenderOffset);
      }

      base.RenderShape(view, mode);
    }

    public int GetNumNavMeshVertices() { return HasEngineInstance() ? EngineNavMesh.GetNumNavMeshVertices() : 0; }
    public int GetNumNavMeshEdges() { return HasEngineInstance() ? EngineNavMesh.GetNumNavMeshEdges() : 0; }
    public int GetNumNavMeshFaces() { return HasEngineInstance() ? EngineNavMesh.GetNumNavMeshFaces() : 0; }
    public int GetNavMeshVertexSize() { return HasEngineInstance() ? EngineNavMesh.GetNavMeshVertexSize() : 0; }
    public int GetNavMeshEdgeSize() { return HasEngineInstance() ? EngineNavMesh.GetNavMeshEdgeSize() : 0; }
    public int GetNavMeshFaceSize() { return HasEngineInstance() ? EngineNavMesh.GetNavMeshFaceSize() : 0; }
    public int GetNavMeshStructSize() { return HasEngineInstance() ? EngineNavMesh.GetNavMeshStructSize() : 0; }

    #endregion
  }

  #endregion

  #region Shape Creator Plugin

  class HavokNavMeshShapeCreator : CSharpFramework.IShapeCreatorPlugin
  {
    public HavokNavMeshShapeCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_navmesh.png"), Color.Magenta);
      CategoryIconIndex = IconIndex;
    }

    public override string GetPluginName()
    {
      return "Nav Mesh";
    }

    public override string GetPluginCategory()
    {
      return "Havok AI";
    }

    public override string GetPluginDescription()
    {
      return "Creates a shape that can be used to build a navigation mesh.";
    }

    public override ShapeBase CreateShapeInstance()
    {
      HavokNavMeshShape shape = new HavokNavMeshShape("NavMesh");
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
