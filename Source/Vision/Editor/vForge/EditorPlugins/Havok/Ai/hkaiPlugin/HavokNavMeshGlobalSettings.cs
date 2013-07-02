/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.ComponentModel;
using System.Runtime.Serialization;
using System.Collections;
using System.Collections.Generic;
using CSharpFramework;
using CSharpFramework.PropertyEditors;
using CSharpFramework.UndoRedo;
using CSharpFramework.Scene;
using CSharpFramework.Serialization;

namespace HavokAiEditorPlugin.Shapes
{
  #region Nav Mesh Generation Enums

  public enum eCharacterWidthUsage
  {
    [NonEditableFlag]
    None = 0,
    BlockEdges
  }

  public enum eEdgeMatchingMetric
  {
    [NonEditableFlag]
    OrderByOverlap = 1,
    OrderByDistance
  }

  public enum eSplitGenerationMethod
  {
    [NonEditableFlag]
    SimplifyIndividually = 1,
    SimplifyAllAtOnce
  }

  public enum eShelverType
  {
    [NonEditableFlag]
    StoreInMemory = 0,
    StoreOnDisk
  }

  public enum eTerrainLOD
  {
    [NonEditableFlag]
    Original = 0,
    High = 1,
    Medium = 2,
    Low = 3
  }
  #endregion

  #region class HavokNavMeshGlobalSettings

  [Serializable]
  public class HavokNavMeshGlobalSettings : ICloneable, ISerializable, ISceneObject
  {
    #region Category Sorting Definitions

    protected const int CATORDER_FIRST = HavokNavMeshShape.LAST_CATEGORY_ORDER_ID + 1;
    protected const string CAT_NAVMESHGEN = "Nav Mesh Generation Settings";
    protected const int CATORDER_NAVMESHGEN = CATORDER_FIRST + 1;
    protected const string CAT_NAVMESHGENADV = "Nav Mesh Generation Settings (Advanced)";
    protected const int CATORDER_NAVMESHGENADV = CATORDER_FIRST + 2;
    protected const string CAT_NAVMESHEDGE = "Nav Mesh Edge Matching Settings";
    protected const int CATORDER_NAVMESHEDGE = CATORDER_FIRST + 3;
    protected const string CAT_NAVMESHSIMP = "Nav Mesh Simplification Settings";
    protected const int CATORDER_NAVMESHSIMP = CATORDER_FIRST + 4;
    protected const string CAT_NAVMESHSIMPADV = "Nav Mesh Simplification Settings (Advanced)";
    protected const int CATORDER_NAVMESHSIMPADV = CATORDER_FIRST + 5;
    protected const string CAT_NAVMESHSPLITGEN = "Nav Mesh Split Generation Settings";
    protected const int CATORDER_NAVMESHSPLITGEN = CATORDER_FIRST + 6;
    protected const string CAT_NAVMESHLINK = "Nav Mesh Link Settings - These values are used when nav meshes are merged. (E.g: when uses Multi Tiles or Zones)";
    protected const int CATORDER_NAVMESHLINK = CATORDER_FIRST + 7;
    protected const string CAT_NAVMESHRUNTIME = "Nav Mesh Runtime Settings";
    protected const int CATORDER_NAVMESHRUNTIME = CATORDER_FIRST + 8;
    //public new const int LAST_CATEGORY_ORDER_ID = CATORDER_NAVMESHSPLITGEN;

    #endregion

    #region Constructor

    public HavokNavMeshGlobalSettings(ISceneObject parent, string name)
    {
      m_parent = parent;
      m_name = name;
    }

    public override string ToString()
    {
      return Name;
    }

    #endregion

    #region ISerializable members

    protected HavokNavMeshGlobalSettings(SerializationInfo info, StreamingContext context)
    {
      // Nav Mesh Generation Settings
      m_parent = (ISceneObject)info.GetValue("Parent", typeof(ISceneObject));
      m_name = info.GetString("Name");
      m_characterHeight = info.GetSingle("CharacterHeight");
      m_maxWalkableSlopeDeg = info.GetSingle("MaxWalkableSlope");
      m_minRegionArea = info.GetSingle("MinRegionArea");
      m_minDistanceToSeedPoints = info.GetSingle("MinDistanceToSeedPoints");
      m_borderPreservationTolerance = info.GetSingle("BorderPreservationTolerance");
      m_minCharacterWidth = info.GetSingle("MinCharacterWidth");
      
      m_characterWidthUsage = (eCharacterWidthUsage)info.GetValue("CharacterWidthUsage", typeof(eCharacterWidthUsage));
      
      if (m_characterWidthUsage > eCharacterWidthUsage.BlockEdges)
      {
          m_characterWidthUsage = eCharacterWidthUsage.BlockEdges;
      }
      
      m_restrictToGeometryFromSameZoneOnly = info.GetBoolean("RestrictToZoneGeometry");
      if (SerializationHelper.HasElement(info, "TerrainLOD"))
        m_terrainLOD = (eTerrainLOD)info.GetValue("TerrainLOD", typeof(eTerrainLOD));

      // Nav Mesh Generation Settings (Advanced)
      m_quantizationGridSize = info.GetSingle("QuantizationGridSize");
      m_degenerateAreaThreshold = info.GetSingle("DegenerateAreaThreshold");
      m_degenerateWidthThreshold = info.GetSingle("DegenerateWidthThreshold");
      m_convexThreshold = info.GetSingle("ConvexThreshold");
      m_maxNumEdgesPerFace = info.GetInt32("MaxNumEdgesPerFace");
      m_weldInputVertices = info.GetBoolean("WeldInputVertices");
      m_weldThreshold = info.GetSingle("WeldThreshold");

      // Nav Mesh Edge Matching Settings
      m_edgeConnectionIterations = info.GetInt32("EdgeConnectionIterations");
      m_edgeMatchingMetric = (eEdgeMatchingMetric)info.GetValue("EdgeMatchingMetric", typeof(eEdgeMatchingMetric));
      m_maxStepHeight = info.GetSingle("MaxStepHeight");
      m_maxSeparation = info.GetSingle("MaxSeparation");
      m_maxOverhang = info.GetSingle("MaxOverhang");
      m_planarAlignmentAngleDeg = info.GetSingle("PlanarAlignmentAngle");
      m_verticalAlignmentAngleDeg = info.GetSingle("VerticalAlignmentAngle");
      m_minEdgeOverlap = info.GetSingle("MinEdgeOverlap");

      // Nav Mesh Simplification Settings
      m_enableSimplification = info.GetBoolean("EnableSimplification");
      m_maxBorderSimplifyArea = info.GetSingle("MaxBorderSimplifyArea");
      m_maxConcaveBorderSimplifyArea = info.GetSingle("MaxConcaveBorderSimplifyArea");
      m_useHeightPartitioning = info.GetBoolean("UseHeightPartitioning");
      m_maxPartitionHeightError = info.GetSingle("MaxPartitionHeightError");

      // Nav Mesh Simplification Settings (Advanced)
      m_minCorridorWidth = info.GetSingle("MinCorridorWidth");
      m_maxCorridorWidth = info.GetSingle("MaxCorridorWidth");
      m_holeReplacementArea = info.GetSingle("HoleReplacementArea");
      m_maxLoopShrinkFraction = info.GetSingle("MaxLoopShrinkFraction");
      m_maxBorderHeightError = info.GetSingle("MaxBorderHeightError");
      m_maxBorderDistanceError = info.GetSingle("MaxBorderDistanceError");
      m_maxPartitionSize = info.GetInt32("MaxPartitionSize");
      m_useConservativeHeightPartitioning = info.GetBoolean("UseConservativeHeightPartitioning");
      m_hertelMehlhornHeightError = info.GetSingle("HertelMehlhornHeightError");
      m_planarityThresholdDeg = info.GetSingle("PlanarityThreshold");
      m_nonconvexityThreshold = info.GetSingle("NonconvexityThreshold");
      m_boundaryEdgeFilterThreshold = info.GetSingle("BoundaryEdgeFilterThreshold");
      m_maxSharedVertexHorizontalError = info.GetSingle("MaxSharedVertexHorizontalError");
      m_maxSharedVertexVerticalError = info.GetSingle("MaxSharedVertexVerticalError");
      m_maxBoundaryVertexHorizontalError = info.GetSingle("MaxBoundaryVertexHorizontalError");
      m_maxBoundaryVertexVerticalError = info.GetSingle("MaxBoundaryVertexVerticalError");
      m_mergeLongestEdgesFirst = info.GetBoolean("MergeLongestEdgesFirst");

      // Nav Mesh Split Generation Settings
      m_numTilesX = info.GetInt32("NumTilesX");
      m_numTilesY = info.GetInt32("NumTilesY");
      m_splitGenerationMethod = (eSplitGenerationMethod)info.GetValue("SplitGenerationMethod", typeof(eSplitGenerationMethod));
      m_shelverType = (eShelverType)info.GetValue("ShelverType", typeof(eShelverType));

      // Nav Mesh Link Settings
      if (SerializationHelper.HasElement(info, "LinkEdgeMatchTolerance"))
        m_linkEdgeMatchTolerance = info.GetSingle("LinkEdgeMatchTolerance");

      if (SerializationHelper.HasElement(info, "LinkMaxStepHeight"))
        m_linkMaxStepHeight = info.GetSingle("LinkMaxStepHeight");

      if (SerializationHelper.HasElement(info, "LinkMaxSeparation"))
        m_linkMaxSeparation = info.GetSingle("LinkMaxSeparation");

      if (SerializationHelper.HasElement(info, "LinkMaxOverhang"))
        m_linkMaxOverhang = info.GetSingle("LinkMaxOverhang");     

      if (SerializationHelper.HasElement(info, "LinkPlanarAlignmentAngle"))
        m_linkPlanarAlignmentAngle = info.GetSingle("LinkPlanarAlignmentAngle");

      if (SerializationHelper.HasElement(info, "LinkVerticalAlignmentAngle"))
        m_linkVerticalAlignmentAngle = info.GetSingle("LinkVerticalAlignmentAngle");

      if (SerializationHelper.HasElement(info, "LinkMinEdgeOverlap"))
        m_linkMinEdgeOverlap = info.GetSingle("LinkMinEdgeOverlap");

      if (SerializationHelper.HasElement(info, "ConnectNavToPhysics"))
        m_connectNavToPhysics = info.GetBoolean("ConnectNavToPhysics");
      
    }

    public void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      // Nav Mesh Generation Settings
      info.AddValue("Parent", m_parent);
      info.AddValue("Name", m_name);
      info.AddValue("CharacterHeight", m_characterHeight);
      info.AddValue("MaxWalkableSlope", m_maxWalkableSlopeDeg);
      info.AddValue("MinRegionArea", m_minRegionArea);
      info.AddValue("MinDistanceToSeedPoints", m_minDistanceToSeedPoints);
      info.AddValue("BorderPreservationTolerance", m_borderPreservationTolerance);
      info.AddValue("MinCharacterWidth", m_minCharacterWidth);
      info.AddValue("CharacterWidthUsage", m_characterWidthUsage);
      info.AddValue("RestrictToZoneGeometry", m_restrictToGeometryFromSameZoneOnly);
      info.AddValue("TerrainLOD", m_terrainLOD);

      // Nav Mesh Generation Settings (Advanced)
      info.AddValue("QuantizationGridSize", m_quantizationGridSize);
      info.AddValue("DegenerateAreaThreshold", m_degenerateAreaThreshold);
      info.AddValue("DegenerateWidthThreshold", m_degenerateWidthThreshold);
      info.AddValue("ConvexThreshold", m_convexThreshold);
      info.AddValue("MaxNumEdgesPerFace", m_maxNumEdgesPerFace);
      info.AddValue("WeldInputVertices", m_weldInputVertices);
      info.AddValue("WeldThreshold", m_weldThreshold);

      // Nav Mesh Edge Matching Settings
      info.AddValue("EdgeConnectionIterations", m_edgeConnectionIterations);
      info.AddValue("EdgeMatchingMetric", m_edgeMatchingMetric);
      info.AddValue("MaxStepHeight", m_maxStepHeight);
      info.AddValue("MaxSeparation", m_maxSeparation);
      info.AddValue("MaxOverhang", m_maxOverhang);
      info.AddValue("PlanarAlignmentAngle", m_planarAlignmentAngleDeg);
      info.AddValue("VerticalAlignmentAngle", m_verticalAlignmentAngleDeg);
      info.AddValue("MinEdgeOverlap", m_minEdgeOverlap);

      // Nav Mesh Simplification Settings
      info.AddValue("EnableSimplification", m_enableSimplification);
      info.AddValue("MaxBorderSimplifyArea", m_maxBorderSimplifyArea);
      info.AddValue("MaxConcaveBorderSimplifyArea", m_maxConcaveBorderSimplifyArea);
      info.AddValue("UseHeightPartitioning", m_useHeightPartitioning);
      info.AddValue("MaxPartitionHeightError", m_maxPartitionHeightError);

      // Nav Mesh Simplification Settings (Advanced)
      info.AddValue("MinCorridorWidth", m_minCorridorWidth);
      info.AddValue("MaxCorridorWidth", m_maxCorridorWidth);
      info.AddValue("HoleReplacementArea", m_holeReplacementArea);
      info.AddValue("MaxLoopShrinkFraction", m_maxLoopShrinkFraction);
      info.AddValue("MaxBorderHeightError", m_maxBorderHeightError);
      info.AddValue("MaxBorderDistanceError", m_maxBorderDistanceError);
      info.AddValue("MaxPartitionSize", m_maxPartitionSize);
      info.AddValue("UseConservativeHeightPartitioning", m_useConservativeHeightPartitioning);
      info.AddValue("HertelMehlhornHeightError", m_hertelMehlhornHeightError);
      info.AddValue("PlanarityThreshold", m_planarityThresholdDeg);
      info.AddValue("NonconvexityThreshold", m_nonconvexityThreshold);
      info.AddValue("BoundaryEdgeFilterThreshold", m_boundaryEdgeFilterThreshold);
      info.AddValue("MaxSharedVertexHorizontalError", m_maxSharedVertexHorizontalError);
      info.AddValue("MaxSharedVertexVerticalError", m_maxSharedVertexVerticalError);
      info.AddValue("MaxBoundaryVertexHorizontalError", m_maxBoundaryVertexHorizontalError);
      info.AddValue("MaxBoundaryVertexVerticalError", m_maxBoundaryVertexVerticalError);
      info.AddValue("MergeLongestEdgesFirst", m_mergeLongestEdgesFirst);

      // Nav Mesh Split Generation Settings
      info.AddValue("NumTilesX", m_numTilesX);
      info.AddValue("NumTilesY", m_numTilesY);
      info.AddValue("SplitGenerationMethod", m_splitGenerationMethod);
      info.AddValue("ShelverType", m_shelverType);

      // Nav Mesh Link Settings
      info.AddValue("LinkEdgeMatchTolerance", m_linkEdgeMatchTolerance);
      info.AddValue("LinkMaxStepHeight", m_linkMaxStepHeight);
      info.AddValue("LinkMaxSeparation", m_linkMaxSeparation);
      info.AddValue("LinkMaxOverhang", m_linkMaxOverhang);
      info.AddValue("LinkPlanarAlignmentAngle", m_linkPlanarAlignmentAngle);
      info.AddValue("LinkVerticalAlignmentAngle", m_linkVerticalAlignmentAngle);
      info.AddValue("LinkMinEdgeOverlap", m_linkMinEdgeOverlap);

      // Nav Mesh Runtime Settings
      info.AddValue("ConnectNavToPhysics", m_connectNavToPhysics);
    }

    #endregion

    #region ICloneable members

    object ICloneable.Clone()
    {
      // simply delegate to our type-safe cousin
      return this.Clone();
    }

    public HavokNavMeshGlobalSettings Clone()
    {
      // Start with a flat, memberwise copy
      HavokNavMeshGlobalSettings copyObj = this.MemberwiseClone() as HavokNavMeshGlobalSettings;
      return copyObj;
    }

    #endregion

    #region ISceneObject members

    [Browsable(false)]
    public bool Modifiable { get { return true; } }

    [Browsable(false)]
    public bool Modified 
    { 
      set 
      { 
        if (value && m_parent != null
          && m_parent.Modifiable)
        {
          m_parent.Modified = value;
        }
      } 
    }

    [Browsable(false)]
    public ISceneObject Parent
    {
      get { return m_parent; }
      set { m_parent = value; }
    }

    #endregion

    #region Private Member Variables

    // just for Modification notification
    ISceneObject m_parent = null;
    // Nav Mesh Generation Settings - These are in Havok units
    string m_name = @"";
    float m_characterHeight = 1.75f;
    float m_maxWalkableSlopeDeg = 60.0f;
    float m_minRegionArea = 5.0f;
    float m_minDistanceToSeedPoints = 1.0f;
    float m_borderPreservationTolerance = 0.0f;
    float m_minCharacterWidth = 0.0f;
    eCharacterWidthUsage m_characterWidthUsage = eCharacterWidthUsage.BlockEdges;
    bool m_restrictToGeometryFromSameZoneOnly = true;
    eTerrainLOD m_terrainLOD = eTerrainLOD.High;

    // Nav Mesh Generation Settings (Advanced)
    float m_quantizationGridSize = 0.0078125f;
    float m_degenerateAreaThreshold = 0.0f;
    float m_degenerateWidthThreshold = 5e-3f;
    float m_convexThreshold = 0.1f;
    int m_maxNumEdgesPerFace = 255;
    bool m_weldInputVertices = true;
    float m_weldThreshold = 0.01f;

    // Nav Mesh Edge Matching Settings
    int m_edgeConnectionIterations = 2;
    eEdgeMatchingMetric m_edgeMatchingMetric = eEdgeMatchingMetric.OrderByDistance;
    float m_maxStepHeight = 0.5f;
    float m_maxSeparation = 0.1f;
    float m_maxOverhang = 1e-2f;
    float m_planarAlignmentAngleDeg = 5.0f;
    float m_verticalAlignmentAngleDeg = 60.0f;
    float m_minEdgeOverlap = 0.02f;

    // Nav Mesh Simplification Settings
    bool m_enableSimplification = true;
    float m_maxBorderSimplifyArea = 1.5f;
    float m_maxConcaveBorderSimplifyArea = 0.15f;
    bool m_useHeightPartitioning = false;
    float m_maxPartitionHeightError = 100.0f;

    // Nav Mesh Simplification Settings (Advanced)
    float m_minCorridorWidth = 0.4f;
    float m_maxCorridorWidth = 0.6f;
    float m_holeReplacementArea = 1.0f;
    float m_maxLoopShrinkFraction = 0.75f;
    float m_maxBorderHeightError = 1.5f;
    float m_maxBorderDistanceError = 2.0f;
    int m_maxPartitionSize = 40000;
    bool m_useConservativeHeightPartitioning = false;
    float m_hertelMehlhornHeightError = 0.01f;
    float m_planarityThresholdDeg = 8.11f;
    float m_nonconvexityThreshold = 0.1f;
    float m_boundaryEdgeFilterThreshold = 0.01f;
    float m_maxSharedVertexHorizontalError = 1.0f;
    float m_maxSharedVertexVerticalError = 1.0f;
    float m_maxBoundaryVertexHorizontalError = 1.0f;
    float m_maxBoundaryVertexVerticalError = 1.0f;
    bool m_mergeLongestEdgesFirst = true;

    // Nav Mesh Split Generation Settings
    int m_numTilesX = 1;
    int m_numTilesY = 1;
    eSplitGenerationMethod m_splitGenerationMethod = eSplitGenerationMethod.SimplifyIndividually;
    eShelverType m_shelverType = eShelverType.StoreInMemory;

    // Nav Mesh Link Setting
    float m_linkEdgeMatchTolerance = 0.1f;
    float m_linkMaxStepHeight = 0.5f;
    float m_linkMaxSeparation = 0.1f;
    float m_linkMaxOverhang = 1e-2f;
    float m_linkPlanarAlignmentAngle = 5.0f;
    float m_linkVerticalAlignmentAngle = 60.0f;
    float m_linkMinEdgeOverlap = 0.02f;

    // Nav Mesh Runtime Settings
    bool m_connectNavToPhysics = false;

    #endregion Private Member Variables

    #region Properties: NAVMESHGEN

    [PropertyOrder(1)]
    [Description("Name of the configuration settings.  Unique for identification purposes")]
    [SortedCategory(CAT_NAVMESHGEN, CATORDER_NAVMESHGEN)]
    public string Name
    {
      get { return m_name; }
      set { m_name = value; }
    }

    [PropertyOrder(2)]
    [Description("The height of the character (in meters) used to extrude the geometry when generating the nav mesh. The resulting nav mesh AABB is also expanded by this amount.")]
    [SortedCategory(CAT_NAVMESHGEN, CATORDER_NAVMESHGEN)]
    public float CharacterHeight
    {
      get { return m_characterHeight; }
      set { m_characterHeight = value; }
    }

    [PropertyOrder(2)]
    [Description("The maximum allowable surface slope in degrees.")]
    [SortedCategory(CAT_NAVMESHGEN, CATORDER_NAVMESHGEN)]
    public float MaxWalkableSlope
    {
      get { return m_maxWalkableSlopeDeg; }
      set
      {
        m_maxWalkableSlopeDeg = value;
        if (m_maxWalkableSlopeDeg < 0.0f)
          m_maxWalkableSlopeDeg = 0.0f;
        else if (m_maxWalkableSlopeDeg > 90.0f)
          m_maxWalkableSlopeDeg = 90.0f;

      }
    }

    [PropertyOrder(3)]
    [Description("Any regions with area (in meters squared) less than this threshold will be automatically removed.")]
    [SortedCategory(CAT_NAVMESHGEN, CATORDER_NAVMESHGEN)]
    public float MinRegionArea
    {
      get { return m_minRegionArea; }
      set { m_minRegionArea = value; }
    }

    [PropertyOrder(4)]
    [Description("Minimum acceptable distance (in meters) to the seed points. If no face in a region is within the minimum distance to at least one seed point, the region will be removed.")]
    [SortedCategory(CAT_NAVMESHGEN, CATORDER_NAVMESHGEN)]
    public float MinDistanceToSeedPoints
    {
      get { return m_minDistanceToSeedPoints; }
      set { m_minDistanceToSeedPoints = value; }
    }

    [PropertyOrder(5)]
    [Description("Tolerance (in meters) to preserve small regions near the boundary AABB of the nav mesh. Any region within this distance of the nav mesh AABB will always be preserved. It is advisable to set this to a non-zero value when building streamable nav meshes as small regions may connect to adjacent tiles.")]
    [SortedCategory(CAT_NAVMESHGEN, CATORDER_NAVMESHGEN)]
    public float BorderPreservationTolerance
    {
      get { return m_borderPreservationTolerance; }
      set { m_borderPreservationTolerance = value; }
    }

    [PropertyOrder(6)]
    [Description("The minimum character diameter (in meters) that will be used on this nav mesh. This value is used to prune the nav mesh, removing regions that are untraversable based on this value, or, if erosion is enabled, to shrink the nav mesh by half of the specified value.")]
    [SortedCategory(CAT_NAVMESHGEN, CATORDER_NAVMESHGEN)]
    public float MinCharacterWidth
    {
      get { return m_minCharacterWidth; }
      set { m_minCharacterWidth = value; }
    }

    [PropertyOrder(7)]
    [Description("How the minimum character width (in meters) is used to simplify the nav mesh.")]
    [SortedCategory(CAT_NAVMESHGEN, CATORDER_NAVMESHGEN)]
    [Editor(typeof(EnumEditor), typeof(System.Drawing.Design.UITypeEditor))]
    public eCharacterWidthUsage CharacterWidthUsage
    {
      get { return m_characterWidthUsage; }
      set { m_characterWidthUsage = value; }
    }

    [PropertyOrder(8)]
    [Description("If true, only geometry associated with the same zone as the NavMeshShape will be considered walkable geometry for nav mesh generation. If the NavMeshShape is not in a zone, it will include all geometry not associated with any zones.  Set to false to include all geometry regardless of zone association.")]
    [SortedCategory(CAT_NAVMESHGEN, CATORDER_NAVMESHGEN)]
    public bool RestrictToInputGeometryFromSameZone
    {
      get { return m_restrictToGeometryFromSameZoneOnly; }
      set { m_restrictToGeometryFromSameZoneOnly = value; }
    }

    [PropertyOrder(9)]
    [Description("Set the desired terrain level of detail to use for nav mesh generation.  With the Original setting, all terrain samples are used.  At High/Medium/Low, one quarter/one sixteenth/one sixty-fourth of the terrain samples are used.")]
    [SortedCategory(CAT_NAVMESHGEN, CATORDER_NAVMESHGEN)]
    [Editor(typeof(EnumEditor), typeof(System.Drawing.Design.UITypeEditor))]
    public eTerrainLOD TerrainLevelOfDetail
    {
      get { return m_terrainLOD; }
      set { m_terrainLOD = value; }
    }

    #endregion Properties: NAVMESHGEN

    #region Properties: NAVMESHGENADV

    [PropertyOrder(12)]
    [Description("The size of the grid to which the input vertices are snapped.")]
    [SortedCategory(CAT_NAVMESHGENADV, CATORDER_NAVMESHGENADV)]
    public float QuantizationGridSize
    {
      get { return m_quantizationGridSize; }
      set { m_quantizationGridSize = value; }
    }


    [PropertyOrder(13)]
    [Description("Threshold (in square meters) used for removal of degenerate triangles after triangulation.")]
    [SortedCategory(CAT_NAVMESHGENADV, CATORDER_NAVMESHGENADV)]
    public float DegenerateAreaThreshold
    {
      get { return m_degenerateAreaThreshold; }
      set { m_degenerateAreaThreshold = value; }
    }

    [PropertyOrder(14)]
    [Description("Threshold (in meters) used for removal of degenerate triangles.")]
    [SortedCategory(CAT_NAVMESHGENADV, CATORDER_NAVMESHGENADV)]
    public float DegenerateWidthThreshold
    {
      get { return m_degenerateWidthThreshold; }
      set { m_degenerateWidthThreshold = value; }
    }

    [PropertyOrder(15)]
    [Description("Threshold used for convex piece merging (Hertel-Mehlhorn). It specifies the maximum (reflex) angle in degrees allowed between adjacent edges.")]
    [SortedCategory(CAT_NAVMESHGENADV, CATORDER_NAVMESHGENADV)]
    public float ConvexThreshold
    {
      get { return m_convexThreshold; }
      set { m_convexThreshold = value; }
    }

    [PropertyOrder(16)]
    [Description("For convex piece merging (Hertel-Mehlhorn), only merges that result in faces with less than this parameter will be accepted. Note that in the final mesh, faces with more edges may be created due to splitting of edges during edge matching or generating streaming information.")]
    [SortedCategory(CAT_NAVMESHGENADV, CATORDER_NAVMESHGENADV)]
    public int MaxNumEdgesPerFace
    {
      get { return m_maxNumEdgesPerFace; }
      set { m_maxNumEdgesPerFace = value; }
    }

    [PropertyOrder(17)]
    [Description("Specifies if the input vertices should be welded before nav mesh construction.")]
    [SortedCategory(CAT_NAVMESHGENADV, CATORDER_NAVMESHGENADV)]
    public bool WeldInputVertices
    {
      get { return m_weldInputVertices; }
      set { m_weldInputVertices = value; }
    }

    [PropertyOrder(18)]
    [Description("Threshold (in meters) used when welding input geometry.")]
    [SortedCategory(CAT_NAVMESHGENADV, CATORDER_NAVMESHGENADV)]
    public float WeldThreshold
    {
      get { return m_weldThreshold; }
      set { m_weldThreshold = value; }
    }

    #endregion Properties: NAVMESHGENADV

    #region Properties: NAVMESHEDGE

    [PropertyOrder(19)]
    [Description("Number of passes of the edge matching algorithm.")]
    [SortedCategory(CAT_NAVMESHEDGE, CATORDER_NAVMESHEDGE)]
    public int EdgeConnectionIterations
    {
      get { return m_edgeConnectionIterations; }
      set { m_edgeConnectionIterations = value; }
    }

    [PropertyOrder(20)]
    [Description("The order by which potential edge pairs are sorted.")]
    [SortedCategory(CAT_NAVMESHEDGE, CATORDER_NAVMESHEDGE)]
    [Editor(typeof(EnumEditor), typeof(System.Drawing.Design.UITypeEditor))]
    public eEdgeMatchingMetric EdgeMatchingMetric
    {
      get { return m_edgeMatchingMetric; }
      set { m_edgeMatchingMetric = value; }
    }

    [PropertyOrder(21)]
    [Description("Maximum step height (in meters).")]
    [SortedCategory(CAT_NAVMESHEDGE, CATORDER_NAVMESHEDGE)]
    public float MaxStepHeight
    {
      get { return m_maxStepHeight; }
      set { m_maxStepHeight = value; }
    }

    [PropertyOrder(22)]
    [Description("Maximum allowed separation (in meters) when considering connecting a pair of edges.")]
    [SortedCategory(CAT_NAVMESHEDGE, CATORDER_NAVMESHEDGE)]
    public float MaxSeparation
    {
      get { return m_maxSeparation; }
      set { m_maxSeparation = value; }
    }

    [PropertyOrder(23)]
    [Description("Maximum allowed overhang (in meters) when considering connecting a pair of edges.")]
    [SortedCategory(CAT_NAVMESHEDGE, CATORDER_NAVMESHEDGE)]
    public float MaxOverhang
    {
      get { return m_maxOverhang; }
      set { m_maxOverhang = value; }
    }

    [PropertyOrder(24)]
    [Description("Minimum planar alignment required when considering connecting a pair of edges (degrees).")]
    [SortedCategory(CAT_NAVMESHEDGE, CATORDER_NAVMESHEDGE)]
    public float PlanarAlignmentAngle
    {
      get { return m_planarAlignmentAngleDeg; }
      set
      {
        m_planarAlignmentAngleDeg = value;
        if (m_planarAlignmentAngleDeg < 0.0f)
          m_planarAlignmentAngleDeg = 0.0f;
        else if (m_planarAlignmentAngleDeg > 180.0f)
          m_planarAlignmentAngleDeg = 180.0f;

      }
    }

    [PropertyOrder(25)]
    [Description("Minimum vertical alignment required when considering connecting a pair of edges (degrees).")]
    [SortedCategory(CAT_NAVMESHEDGE, CATORDER_NAVMESHEDGE)]
    public float VerticalAlignmentAngle
    {
      get { return m_verticalAlignmentAngleDeg; }
      set
      {
        m_verticalAlignmentAngleDeg = value;
        if (m_verticalAlignmentAngleDeg < 0.0f)
          m_verticalAlignmentAngleDeg = 0.0f;
        else if (m_verticalAlignmentAngleDeg > 180.0f)
          m_verticalAlignmentAngleDeg = 180.0f;

      }
    }

    [PropertyOrder(26)]
    [Description("Minimum overlap (in meters) required when considering connecting a pair of edges.")]
    [SortedCategory(CAT_NAVMESHEDGE, CATORDER_NAVMESHEDGE)]
    public float MinEdgeOverlap
    {
      get { return m_minEdgeOverlap; }
      set { m_minEdgeOverlap = value; }
    }

    #endregion Properties: NAVMESHEDGE

    #region Properties: NAVMESHSIMP

    [PropertyOrder(27)]
    [Description("Whether or not to apply the nav mesh simplification algorithm.")]
    [SortedCategory(CAT_NAVMESHSIMP, CATORDER_NAVMESHSIMP)]
    public bool EnableSimplification
    {
      get { return m_enableSimplification; }
      set { m_enableSimplification = value; }
    }

    [PropertyOrder(28)]
    [Description("This controls the amount of simplification applied to internal segments and convex boundary segments.")]
    [SortedCategory(CAT_NAVMESHSIMP, CATORDER_NAVMESHSIMP)]
    public float MaxBorderSimplifyArea
    {
      get { return m_maxBorderSimplifyArea; }
      set { m_maxBorderSimplifyArea = value; }
    }

    [PropertyOrder(29)]
    [Description("This controls the amount of simplification applied to concave boundary segments.")]
    [SortedCategory(CAT_NAVMESHSIMP, CATORDER_NAVMESHSIMP)]
    public float MaxConcaveBorderSimplifyArea
    {
      get { return m_maxConcaveBorderSimplifyArea; }
      set { m_maxConcaveBorderSimplifyArea = value; }
    }

    [PropertyOrder(30)]
    [Description("Whether or not to partition by height.")]
    [SortedCategory(CAT_NAVMESHSIMP, CATORDER_NAVMESHSIMP)]
    public bool UseHeightPartitioning
    {
      get { return m_useHeightPartitioning; }
      set { m_useHeightPartitioning = value; }
    }

    [PropertyOrder(31)]
    [Description("When partitioning by height, the maximum deviation (in meters) allowed for a given partition.")]
    [SortedCategory(CAT_NAVMESHSIMP, CATORDER_NAVMESHSIMP)]
    public float MaxPartitionHeightError
    {
      get { return m_maxPartitionHeightError; }
      set { m_maxPartitionHeightError = value; }
    }

    #endregion Properties: NAVMESHSIMP

    #region Properties: NAVMESHSIMPADV

    [PropertyOrder(32)]
    [Description("Corridors narrower than this value may be collapsed freely.")]
    [SortedCategory(CAT_NAVMESHSIMPADV, CATORDER_NAVMESHSIMPADV)]
    public float MinCorridorWidth
    {
      get { return m_minCorridorWidth; }
      set { m_minCorridorWidth = value; }
    }

    [PropertyOrder(33)]
    [Description("Corridors wider than this value may be collapsed down to this.")]
    [SortedCategory(CAT_NAVMESHSIMPADV, CATORDER_NAVMESHSIMPADV)]
    public float MaxCorridorWidth
    {
      get { return m_maxCorridorWidth; }
      set { m_maxCorridorWidth = value; }
    }

    [PropertyOrder(34)]
    [Description("Holes with area (in square meters) less than this are candidates for conversion to quads.")]
    [SortedCategory(CAT_NAVMESHSIMPADV, CATORDER_NAVMESHSIMPADV)]
    public float HoleReplacementArea
    {
      get { return m_holeReplacementArea; }
      set { m_holeReplacementArea = value; }
    }

    [PropertyOrder(35)]
    [Description("The area of any partition loop will not be reduced by more than this fraction.")]
    [SortedCategory(CAT_NAVMESHSIMPADV, CATORDER_NAVMESHSIMPADV)]
    public float MaxLoopShrinkFraction
    {
      get { return m_maxLoopShrinkFraction; }
      set
      {
        m_maxLoopShrinkFraction = value;
        if (m_maxLoopShrinkFraction < 0.0f)
          m_maxLoopShrinkFraction = 0.0f;
        else if (m_maxLoopShrinkFraction > 1.0f)
          m_maxLoopShrinkFraction = 1.0f;

      }
    }

    [PropertyOrder(36)]
    [Description("The maximum allowable border height error when simplifying segments.")]
    [SortedCategory(CAT_NAVMESHSIMPADV, CATORDER_NAVMESHSIMPADV)]
    public float MaxBorderHeightError
    {
      get { return m_maxBorderHeightError; }
      set { m_maxBorderHeightError = value; }
    }

    [PropertyOrder(37)]
    [Description("When simplifying internal segments and convex boundary segments, this controls the maximum projected distance by which the simplified segment can deviate from the unsimplified segment.")]
    [SortedCategory(CAT_NAVMESHSIMPADV, CATORDER_NAVMESHSIMPADV)]
    public float MaxBorderDistanceError
    {
      get { return m_maxBorderDistanceError; }
      set { m_maxBorderDistanceError = value; }
    }

    [PropertyOrder(38)]
    [Description("The maximum number of faces that can be included in a partition. Increasing this will result in fewer internal edges in large open areas, but may slow down generation times slightly.")]
    [SortedCategory(CAT_NAVMESHSIMPADV, CATORDER_NAVMESHSIMPADV)]
    public int MaxPartitionSize
    {
      get { return m_maxPartitionSize; }
      set { m_maxPartitionSize = value; }
    }

    [PropertyOrder(39)]
    [Description("If this is true (and UseHeightPartitioning is true), a more conservative algorithm will be used to guarantee that the total vertical deviation between the simplified and unsimplified nav meshes does not exceed (MaxPartitionHeightError + HertelMehlhornHeightError). This will generally result is less simplification however.")]
    [SortedCategory(CAT_NAVMESHSIMPADV, CATORDER_NAVMESHSIMPADV)]
    public bool UseConservativeHeightPartitioning
    {
      get { return m_useConservativeHeightPartitioning; }
      set { m_useConservativeHeightPartitioning = value; }
    }

    [PropertyOrder(40)]
    [Description("Additional height error allowed when merging faces into convex pieces. This is only used if UseConservativeHeightPartitioning is true.")]
    [SortedCategory(CAT_NAVMESHSIMPADV, CATORDER_NAVMESHSIMPADV)]
    public float HertelMehlhornHeightError
    {
      get { return m_hertelMehlhornHeightError; }
      set { m_hertelMehlhornHeightError = value; }
    }

    [PropertyOrder(41)]
    [Description("Faces will only be merged if the angle (in degrees) between their normals is greater than this.")]
    [SortedCategory(CAT_NAVMESHSIMPADV, CATORDER_NAVMESHSIMPADV)]
    public float PlanarityThreshold
    {
      get { return m_planarityThresholdDeg; }
      set { m_planarityThresholdDeg = value; }
    }

    [PropertyOrder(42)]
    [Description("Amount of non-convexity (in degrees) allowed when merging faces.")]
    [SortedCategory(CAT_NAVMESHSIMPADV, CATORDER_NAVMESHSIMPADV)]
    public float NonconvexityThreshold
    {
      get { return m_nonconvexityThreshold; }
      set { m_nonconvexityThreshold = value; }
    }

    [PropertyOrder(43)]
    [Description("Connected sequences of boundary segments contained within a projected OBB of area less than this threshold will be removed if safe to do so.")]
    [SortedCategory(CAT_NAVMESHSIMPADV, CATORDER_NAVMESHSIMPADV)]
    public float BoundaryEdgeFilterThreshold
    {
      get { return m_boundaryEdgeFilterThreshold; }
      set { m_boundaryEdgeFilterThreshold = value; }
    }

    [PropertyOrder(44)]
    [Description("The maximum allowable projected distance a vertex that isn't used by a boundary edge can be moved by when merging shared vertices.")]
    [SortedCategory(CAT_NAVMESHSIMPADV, CATORDER_NAVMESHSIMPADV)]
    public float MaxSharedVertexHorizontalError
    {
      get { return m_maxSharedVertexHorizontalError; }
      set { m_maxSharedVertexHorizontalError = value; }
    }

    [PropertyOrder(45)]
    [Description("The maximum allowable vertical distance a vertex that isn't used by a boundary edge can be moved by when merging shared vertices.")]
    [SortedCategory(CAT_NAVMESHSIMPADV, CATORDER_NAVMESHSIMPADV)]
    public float MaxSharedVertexVerticalError
    {
      get { return m_maxSharedVertexVerticalError; }
      set { m_maxSharedVertexVerticalError = value; }
    }

    [PropertyOrder(46)]
    [Description("The maximum allowable projected distance a vertex that is used by a boundary edge can be moved by when merging shared vertices.")]
    [SortedCategory(CAT_NAVMESHSIMPADV, CATORDER_NAVMESHSIMPADV)]
    public float MaxBoundaryVertexHorizontalError
    {
      get { return m_maxBoundaryVertexHorizontalError; }
      set { m_maxBoundaryVertexHorizontalError = value; }
    }

    [PropertyOrder(47)]
    [Description("The maximum allowable vertical distance a vertex that is used by a boundary edge can be moved by when merging shared vertices.")]
    [SortedCategory(CAT_NAVMESHSIMPADV, CATORDER_NAVMESHSIMPADV)]
    public float MaxBoundaryVertexVerticalError
    {
      get { return m_maxBoundaryVertexVerticalError; }
      set { m_maxBoundaryVertexVerticalError = value; }
    }

    [PropertyOrder(48)]
    [Description("Whether or not to merge longest edges first when performing Hertel-Mehlhorn. This can result in nicer looking nav meshes.")]
    [SortedCategory(CAT_NAVMESHSIMPADV, CATORDER_NAVMESHSIMPADV)]
    public bool MergeLongestEdgesFirst
    {
      get { return m_mergeLongestEdgesFirst; }
      set { m_mergeLongestEdgesFirst = value; }
    }

    #endregion Properties: NAVMESHSIMPADV

    #region Properties: NAVMESHSPLITGEN

    [PropertyOrder(49)]
    [Description("Number of desired input geometry tiles in the X direction.  Helpful to reduce peak memory usage during navmesh generation.  A single Havok AI navmesh is still generated for each navmesh shape in the scene.")]
    [SortedCategory(CAT_NAVMESHSPLITGEN, CATORDER_NAVMESHSPLITGEN)]
    public int NumTilesX
    {
      get { return m_numTilesX; }
      set
      {
        m_numTilesX = value;
        if (m_numTilesX < 1)
          m_numTilesX = 1;

      }
    }

    [PropertyOrder(50)]
    [Description("Number of desired input geometry tiles in the Y direction.  Helpful to reduce peak memory usage during navmesh generation.  A single Havok AI navmesh is still generated for each navmesh shape in the scene.")]
    [SortedCategory(CAT_NAVMESHSPLITGEN, CATORDER_NAVMESHSPLITGEN)]
    public int NumTilesY
    {
      get { return m_numTilesY; }
      set
      {
        m_numTilesY = value;
        if (m_numTilesY < 1)
          m_numTilesY = 1;

      }
    }

    [PropertyOrder(51)]
    [Description("Describes whether simplification is done separately per intermediate navmesh tile or all at once.")]
    [SortedCategory(CAT_NAVMESHSPLITGEN, CATORDER_NAVMESHSPLITGEN)]
    [Editor(typeof(EnumEditor), typeof(System.Drawing.Design.UITypeEditor))]
    public eSplitGenerationMethod SplitGenerationMethod
    {
      get { return m_splitGenerationMethod; }
      set { m_splitGenerationMethod = value; }
    }

    [PropertyOrder(52)]
    [Description("Describes how tiles should be \"shelved\", either by keeping them in memory (for greatest speed) or by serializing them out to disk (to conserve memory).")]
    [SortedCategory(CAT_NAVMESHSPLITGEN, CATORDER_NAVMESHSPLITGEN)]
    [Editor(typeof(EnumEditor), typeof(System.Drawing.Design.UITypeEditor))]
    public eShelverType ShelverType
    {
      get { return m_shelverType; }
      set { m_shelverType = value; }
    }

    #endregion Properties: NAVMESHSPLITGEN

    #region Properties: NAVMESHLINK

    [PropertyOrder(60)]
    [Description("AABB expansion tolerance used when detecting nearby edges.")]
    [SortedCategory(CAT_NAVMESHLINK, CATORDER_NAVMESHLINK)]
    public float LinkEdgeMatchTolerance
    {
      get { return m_linkEdgeMatchTolerance; }
      set { m_linkEdgeMatchTolerance = value; }
    }

    [PropertyOrder(61)]
    [Description("Maximum step height (in meters).")]
    [SortedCategory(CAT_NAVMESHLINK, CATORDER_NAVMESHLINK)]
    public float LinkMaxStepHeight
    {
      get { return m_linkMaxStepHeight; }
      set { m_linkMaxStepHeight = value; }
    }

    [PropertyOrder(62)]
    [Description("Maximum allowed separation (in meters) when considering connecting a pair of edges.")]
    [SortedCategory(CAT_NAVMESHLINK, CATORDER_NAVMESHLINK)]
    public float LinkMaxSeparation
    {
      get { return m_linkMaxSeparation; }
      set { m_linkMaxSeparation = value; }
    }

    [PropertyOrder(63)]
    [Description("Maximum allowed overhang (in meters) when considering connecting a pair of edges.")]
    [SortedCategory(CAT_NAVMESHLINK, CATORDER_NAVMESHLINK)]
    public float LinkMaxOverhang
    {
      get { return m_linkMaxOverhang; }
      set { m_linkMaxOverhang = value; }
    }

    [PropertyOrder(64)]
    [Description("Minimum planar alignment required when considering connecting a pair of edges (degrees).")]
    [SortedCategory(CAT_NAVMESHLINK, CATORDER_NAVMESHLINK)]
    public float LinkPlanarAlignmentAngle
    {
      get { return m_linkPlanarAlignmentAngle; }
      set { m_linkPlanarAlignmentAngle = value; }
    }

    [PropertyOrder(65)]
    [Description("Minimum vertical alignment required when considering connecting a pair of edges (degrees).")]
    [SortedCategory(CAT_NAVMESHLINK, CATORDER_NAVMESHLINK)]
    public float LinkVerticalAlignmentAngle
    {
      get { return m_linkVerticalAlignmentAngle; }
      set { m_linkVerticalAlignmentAngle = value; }
    }

    [PropertyOrder(66)]
    [Description("Minimum overlap (in meters) required when considering connecting a pair of edges.")]
    [SortedCategory(CAT_NAVMESHLINK, CATORDER_NAVMESHLINK)]
    public float LinkMinEdgeOverlap
    {
      get { return m_minEdgeOverlap; }
      set { m_minEdgeOverlap = value; }
    }

    #endregion Properties: NAVMESHLINK

    #region Properties: NAVMESHRUNTIME

    [PropertyOrder(67)]
    [Description("Whether the nav mesh should be connected to the Havok physics world at runtime. If true, physics objects will modify the nav mesh at runtime.")]
    [SortedCategory(CAT_NAVMESHRUNTIME, CATORDER_NAVMESHRUNTIME)]
    public bool ConnectToPhysics
    {
      get { return m_connectNavToPhysics; }
      set { m_connectNavToPhysics = value; }
    }

    #endregion Properties: NAVMESHRUNTIME

    #region Custom property descriptor list

    class NavMeshSettingsUndoRedoPropertyDescriptor : UndoRedoPropertyDescriptor
    {
      public NavMeshSettingsUndoRedoPropertyDescriptor(PropertyDescriptor srcDescripter, PropertyFlags_e propFlags, HavokNavMeshGlobalSettings settings)
        : base(srcDescripter, propFlags, null, settings)
      {

      }

      public override object GetValue(object component)
      {
        return base.GetValue(_customComponent);
      }

      public override void SetValue(object component, object value)
      {
        base.SetValue(_customComponent, value);
      }
    }

    List<PropertyDescriptor> m_cachedPropertyDescripterList = null;

    public List<PropertyDescriptor> GetCachedPropertiesList()
    {
      if (m_cachedPropertyDescripterList == null)
      {
        // create list once
        m_cachedPropertyDescripterList = new List<PropertyDescriptor>();

        PropertyDescriptorCollection properties = TypeDescriptor.GetProperties(this);
        IEnumerator propEnumerator = properties.GetEnumerator();
        while (propEnumerator.MoveNext())
        {
          PropertyDescriptor prop = propEnumerator.Current as PropertyDescriptor;
          PropertyFlags_e flags = (prop.Name == "Name" || prop.Name == "Parent") ? PropertyFlags_e.Readonly : PropertyFlags_e.None;
          m_cachedPropertyDescripterList.Add(new NavMeshSettingsUndoRedoPropertyDescriptor(prop, flags, this));
        }
      }
      return m_cachedPropertyDescripterList;
    }

    #endregion
  }

  #endregion

  #region class HavokNavMeshGlobalSettingsDictionary

  [Serializable]
  public class HavokNavMeshGlobalSettingsDictionary : ISerializable, ISceneObject
  {
    #region constructor

    public HavokNavMeshGlobalSettingsDictionary(ISceneObject parent)
    {
      m_parent = parent;
      m_settingsDictionary = new Dictionary<string, HavokNavMeshGlobalSettings>();
    }

    #endregion

    #region Serialization and export

    protected HavokNavMeshGlobalSettingsDictionary(SerializationInfo info, StreamingContext context)
    {
      m_settingsDictionary = (Dictionary<string, HavokNavMeshGlobalSettings>)info.GetValue("SettingsDictionary", typeof(Dictionary<string, HavokNavMeshGlobalSettings>));
      m_defaultSettingsName = info.GetString("DefaultSettingsName");
    }

    public void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      info.AddValue("SettingsDictionary", m_settingsDictionary);
      info.AddValue("DefaultSettingsName", m_defaultSettingsName);
    }

    #endregion

    #region ISceneObject implementation

    public bool Modifiable { get { return true; } }

    public bool Modified
    {
      set
      {
        if (value && m_parent != null
          && m_parent.Modifiable)
        {
          m_parent.Modified = value;
        }
      }
    }

    public ISceneObject Parent
    {
      get { return m_parent; }
      set { m_parent = value; }
    }

    #endregion

    #region Properties

    ISceneObject m_parent = null;
    Dictionary<string, HavokNavMeshGlobalSettings> m_settingsDictionary;
    string m_defaultSettingsName = null;

    public string DefaultSettingsName
    {
      get
      {
        HavokNavMeshGlobalSettings debugSettings;
        System.Diagnostics.Debug.Assert(m_defaultSettingsName == null || m_settingsDictionary.TryGetValue(m_defaultSettingsName, out debugSettings));
        return m_defaultSettingsName;
      }
    }

    public int Count
    {
      get { return m_settingsDictionary.Count; }
    }

    #endregion

    #region Create/add/remove

    public bool AddSettingsInstance(string newSettingsName, HavokNavMeshGlobalSettings newSettings)
    {
      if (newSettingsName == null || newSettingsName.Length == 0)
      {
        return false;
      }

      HavokNavMeshGlobalSettings existingSettings;
      if (!m_settingsDictionary.TryGetValue(newSettingsName, out existingSettings))
      {
        newSettings.Parent = this;
        m_settingsDictionary.Add(newSettingsName, newSettings);
        // if this is the first and only settings instance, update the default settings to refer to this
        if (m_settingsDictionary.Count == 1)
        {
          m_defaultSettingsName = newSettingsName;
        }
        return true;
      }
      return false;
    }

    public bool DeleteSettingsInstance(string settingsName)
    {
      HavokNavMeshGlobalSettings settings = null;
      if (m_settingsDictionary.TryGetValue(settingsName, out settings))
      {
        m_settingsDictionary.Remove(settingsName);

        if (m_defaultSettingsName == settingsName)
        {
          m_defaultSettingsName = null;

          // update settingsName to just be the first
          if (m_settingsDictionary.Count > 0)
          {
            IEnumerator keys = m_settingsDictionary.Keys.GetEnumerator();
            keys.MoveNext();
            m_defaultSettingsName = keys.Current as string;
          }
        }
        return true;
      }
      return false;
    }

    public HavokNavMeshGlobalSettings GetSettingsInstance(string settingsName)
    {
      HavokNavMeshGlobalSettings settings = null;

      // try to retrieve associated settings if string name not null
      if (settingsName != null)
      {
        m_settingsDictionary.TryGetValue(settingsName, out settings);
      }
      return settings;
    }

    public HavokNavMeshGlobalSettings CreateAndAddSettingsInstance(string settingsName)
    {
      HavokNavMeshGlobalSettings tempSettings = null;
      if (m_settingsDictionary.TryGetValue(settingsName, out tempSettings))
      {
        // settings object already exists!
        // return null to signal failure to create new one
        return null;
      }

      // sanity check name
      if (settingsName != null && settingsName.Length > 0)
      {
        HavokNavMeshGlobalSettings newSettings = new HavokNavMeshGlobalSettings(this, settingsName);
        m_settingsDictionary.Add(settingsName, newSettings);

        // if this is the first and only settings instance, update the default settings to refer to this
        if (m_settingsDictionary.Count == 1)
        {
          m_defaultSettingsName = settingsName;
        }
        return newSettings;
      }

      return null;
    }

    public HavokNavMeshGlobalSettings GetDefaultSettingsInstance()
    {
      if (m_settingsDictionary.Count == 0)
      {
        return null;
      }

      // basically if settings count is non zero, then default should be set
      System.Diagnostics.Debug.Assert(m_defaultSettingsName != null);
      HavokNavMeshGlobalSettings settings = GetSettingsInstance(m_defaultSettingsName);
      System.Diagnostics.Debug.Assert(settings != null);
      return settings;
    }

    public bool SetDefaultSettingsInstance(string settingsName)
    {
      HavokNavMeshGlobalSettings settings;
      if (m_settingsDictionary.TryGetValue(settingsName, out settings))
      {
        // if it exists already, then 
        m_defaultSettingsName = settingsName;
        return true;
      }
      return false;
    }

    #endregion

    #region Misc

    public string GenerateUniqueSettingsName(string nameBase)
    {
      // check for uniqueness
      int index = 0;
      string settingsName = nameBase;
      HavokNavMeshGlobalSettings tempSettings;
      while (m_settingsDictionary.TryGetValue(settingsName, out tempSettings))
      {
        settingsName = nameBase + index;
        index++;
      }
      return settingsName;
    }

    public string[] GetAllSettingsNames()
    {
      // build array
      ArrayList collectionNameList = new ArrayList();
      foreach (System.Collections.Generic.KeyValuePair<string, HavokNavMeshGlobalSettings> pair in m_settingsDictionary)
      {
        collectionNameList.Add(pair.Key);
      }

      collectionNameList.Sort(); // sort list alphabetically

      return (string[])collectionNameList.ToArray(typeof(string));
    }

    #endregion
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
