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
using CSharpFramework.Actions;
using CSharpFramework.View;
using ManagedFramework;
using HavokAiManaged;
using VisionEditorPlugin.Shapes;

namespace HavokAiEditorPlugin.Shapes
{
    #region class HavokNavMeshLocalSettingsShape

    [Serializable, ShapeTypeNiceName("Havok Nav Mesh Local Settings")]
    public class HavokNavMeshLocalSettingsShape : Shape3D
    {
        #region Category Sorting Definitions

        protected const string CAT_LS_PROPS = "Local Settings Properties";
        protected const int CATORDER_LS_PROPS = Shape3D.LAST_CATEGORY_ORDER_ID + 1;
        protected const string CAT_LS_NAVMESHGEN = "Nav Mesh Generation Settings";
        protected const int CATORDER_LS_NAVMESHGEN = Shape3D.LAST_CATEGORY_ORDER_ID + 2;
        protected const string CAT_LS_NAVMESHEDGE = "Nav Mesh Edge Matching Settings";
        protected const int CATORDER_LS_NAVMESHEDGE = Shape3D.LAST_CATEGORY_ORDER_ID + 3;
        protected const string CAT_LS_NAVMESHSIMP = "Nav Mesh Simplification Settings";
        protected const int CATORDER_LS_NAVMESHSIMP = Shape3D.LAST_CATEGORY_ORDER_ID + 4;
        protected const string CAT_LS_NAVMESHSIMPADV = "Nav Mesh Simplification Settings (Advanced)";
        protected const int CATORDER_LS_NAVMESHSIMPADV = Shape3D.LAST_CATEGORY_ORDER_ID + 5;
        public new const int LAST_CATEGORY_ORDER_ID = CATORDER_LS_NAVMESHSIMPADV;

        #endregion

        #region Constructor

        public HavokNavMeshLocalSettingsShape(string name) : base(name)
        {
        }

        #endregion

        #region Icon

        static int iIconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_navmeshlocalsettings.png"), Color.Magenta);
        public override int GetIconIndex()
        {
            return iIconIndex;
        }

        public override int GetSelectedIconIndex()
        {
            return iIconIndex;
        }

        #endregion

        #region Misc functions

        public override void RenderShape(VisionViewBase view, ShapeRenderMode mode)
        {
            base.RenderShape(view, mode);

            // Don't draw local settings if "playing the game" or 'Visible' property is false
            if (EditorManager.InPlayingMode || FinalVisibleState == false)
                return;

            Vector3F size = new Vector3F(_vBoxSize.X * ScaleX, _vBoxSize.Y * ScaleY, _vBoxSize.Z * ScaleZ);
            BoundingBox bbox = new BoundingBox(-size * 0.5f, size * 0.5f);

            uint color = (mode == ShapeRenderMode.Selected) ? (uint)0x60ff8000 : (uint)0x30ff8000;
            view.RenderOrientedSolidBox(bbox, Position, RotationMatrix, color);
        }

        public override void OnTransformationChanged()
        {
            base.OnTransformationChanged();

            if (_hotSpotSizeX != null)
                _hotSpotSizeX.DisplayScaling = 0.5f * ScaleX;
            if (_hotSpotSizeY != null)
                _hotSpotSizeY.DisplayScaling = 0.5f * ScaleY;
            if (_hotSpotSizeZ != null)
                _hotSpotSizeZ.DisplayScaling = 0.5f * ScaleZ;
        }

        public override bool GetLocalBoundingBox(ref BoundingBox bbox)
        {
            bbox = new BoundingBox(_vBoxSize * -0.5f, _vBoxSize * 0.5f);
            return true;
        }

        #endregion

        #region Cloning

        public override ShapeBase Clone()
        {
            HavokNavMeshLocalSettingsShape copy = base.Clone() as HavokNavMeshLocalSettingsShape;

            copy._hotSpotSizeX = null;
            copy._hotSpotSizeY = null;
            copy._hotSpotSizeZ = null;
            System.Diagnostics.Debug.Assert(!copy.Selected); // turned off in ShapeBase

            return copy;
        }

        #endregion

        #region Extra Properties

        Vector3F _vBoxSize = new Vector3F(300, 300, 200);

        // Nav Mesh Generation Settings
        float m_maxWalkableSlopeDeg = 60.0f;

        // Nav Mesh Edge Matching Settings
        float m_maxStepHeight = 0.5f;
        float m_maxSeparation = 0.1f;
        float m_maxOverhang = 1e-2f;
        float m_planarAlignmentAngleDeg = 5.0f;
        float m_verticalAlignmentAngleDeg = 60.0f;
        float m_minEdgeOverlap = 0.02f;

        // Nav Mesh Simplification Settings
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

        [PropertyOrder(1)]
        [SortedCategory(CAT_LS_PROPS, CATORDER_LS_PROPS)]
        public float BoxSizeX
        {
            get { return _vBoxSize.X; }
            set
            {
                if (_vBoxSize.X == value) return;
                _vBoxSize.X = value;
            }
        }

        [PropertyOrder(2)]
        [SortedCategory(CAT_LS_PROPS, CATORDER_LS_PROPS)]
        public float BoxSizeY
        {
            get { return _vBoxSize.Y; }
            set
            {
                if (_vBoxSize.Y == value) return;
                _vBoxSize.Y = value;
            }
        }

        [PropertyOrder(3)]
        [SortedCategory(CAT_LS_PROPS, CATORDER_LS_PROPS)]
        public float BoxSizeZ
        {
            get { return _vBoxSize.Z; }
            set
            {
                if (_vBoxSize.Z == value) return;
                _vBoxSize.Z = value;
            }
        }

        [PropertyOrder(4)]
        [Description("The maximum allowable surface slope in degrees.")]
        [SortedCategory(CAT_LS_NAVMESHGEN, CATORDER_LS_NAVMESHGEN)]
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
                SetEngineInstanceBaseProperties();
            }
        }

        [PropertyOrder(5)]
        [Description("Maximum step height (in meters).")]
        [SortedCategory(CAT_LS_NAVMESHEDGE, CATORDER_LS_NAVMESHEDGE)]
        public float MaxStepHeight
        {
            get { return m_maxStepHeight; }
            set { m_maxStepHeight = value; SetEngineInstanceBaseProperties(); }
        }

        [PropertyOrder(6)]
        [Description("Maximum allowed separation (in meters) when considering connecting a pair of edges.")]
        [SortedCategory(CAT_LS_NAVMESHEDGE, CATORDER_LS_NAVMESHEDGE)]
        public float MaxSeparation
        {
            get { return m_maxSeparation; }
            set { m_maxSeparation = value; SetEngineInstanceBaseProperties(); }
        }

        [PropertyOrder(7)]
        [Description("Maximum allowed overhang (in meters) when considering connecting a pair of edges.")]
        [SortedCategory(CAT_LS_NAVMESHEDGE, CATORDER_LS_NAVMESHEDGE)]
        public float MaxOverhang
        {
            get { return m_maxOverhang; }
            set { m_maxOverhang = value; SetEngineInstanceBaseProperties(); } 
        }

        [PropertyOrder(8)]
        [Description("Minimum planar alignment required when considering connecting a pair of edges (degrees).")]
        [SortedCategory(CAT_LS_NAVMESHEDGE, CATORDER_LS_NAVMESHEDGE)]
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
                SetEngineInstanceBaseProperties();
            }
        }

        [PropertyOrder(9)]
        [Description("Minimum vertical alignment required when considering connecting a pair of edges (degrees).")]
        [SortedCategory(CAT_LS_NAVMESHEDGE, CATORDER_LS_NAVMESHEDGE)]
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
                SetEngineInstanceBaseProperties();
            }
        }

        [PropertyOrder(10)]
        [Description("Minimum overlap (in meters) required when considering connecting a pair of edges.")]
        [SortedCategory(CAT_LS_NAVMESHEDGE, CATORDER_LS_NAVMESHEDGE)]
        public float MinEdgeOverlap
        {
            get { return m_minEdgeOverlap; }
            set { m_minEdgeOverlap = value; SetEngineInstanceBaseProperties(); }
        }

        [PropertyOrder(11)]
        [Description("This controls the amount of simplification applied to internal segments and convex boundary segments.")]
        [SortedCategory(CAT_LS_NAVMESHSIMP, CATORDER_LS_NAVMESHSIMP)]
        public float MaxBorderSimplifyArea
        {
            get { return m_maxBorderSimplifyArea; }
            set { m_maxBorderSimplifyArea = value; SetEngineInstanceBaseProperties(); }
        }

        [PropertyOrder(12)]
        [Description("This controls the amount of simplification applied to concave boundary segments.")]
        [SortedCategory(CAT_LS_NAVMESHSIMP, CATORDER_LS_NAVMESHSIMP)]
        public float MaxConcaveBorderSimplifyArea
        {
            get { return m_maxConcaveBorderSimplifyArea; }
            set { m_maxConcaveBorderSimplifyArea = value; SetEngineInstanceBaseProperties(); }
        }

        [PropertyOrder(13)]
        [Description("Whether or not to partition by height.")]
        [SortedCategory(CAT_LS_NAVMESHSIMP, CATORDER_LS_NAVMESHSIMP)]
        public bool UseHeightPartitioning
        {
            get { return m_useHeightPartitioning; }
            set { m_useHeightPartitioning = value; SetEngineInstanceBaseProperties(); }
        }

        [PropertyOrder(14)]
        [Description("When partitioning by height, the maximum deviation (in meters) allowed for a given partition.")]
        [SortedCategory(CAT_LS_NAVMESHSIMP, CATORDER_LS_NAVMESHSIMP)]
        public float MaxPartitionHeightError
        {
            get { return m_maxPartitionHeightError; }
            set { m_maxPartitionHeightError = value; SetEngineInstanceBaseProperties(); }
        }

        [PropertyOrder(15)]
        [Description("Corridors narrower than this value may be collapsed freely.")]
        [SortedCategory(CAT_LS_NAVMESHSIMPADV, CATORDER_LS_NAVMESHSIMPADV)]
        public float MinCorridorWidth
        {
            get { return m_minCorridorWidth; }
            set { m_minCorridorWidth = value; SetEngineInstanceBaseProperties(); }
        }

        [PropertyOrder(16)]
        [Description("Corridors wider than this value may be collapsed down to this.")]
        [SortedCategory(CAT_LS_NAVMESHSIMPADV, CATORDER_LS_NAVMESHSIMPADV)]
        public float MaxCorridorWidth
        {
            get { return m_maxCorridorWidth; }
            set { m_maxCorridorWidth = value; SetEngineInstanceBaseProperties(); }
        }

        [PropertyOrder(17)]
        [Description("Holes with area less than this are candidates for conversion to quads.")]
        [SortedCategory(CAT_LS_NAVMESHSIMPADV, CATORDER_LS_NAVMESHSIMPADV)]
        public float HoleReplacementArea
        {
            get { return m_holeReplacementArea; }
            set { m_holeReplacementArea = value; SetEngineInstanceBaseProperties(); }
        }

        [PropertyOrder(18)]
        [Description("The area of any partition loop will not be reduced by more than this fraction.")]
        [SortedCategory(CAT_LS_NAVMESHSIMPADV, CATORDER_LS_NAVMESHSIMPADV)]
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
                SetEngineInstanceBaseProperties();
            }
        }

        [PropertyOrder(19)]
        [Description("The maximum allowable border height error when simplifying segments.")]
        [SortedCategory(CAT_LS_NAVMESHSIMPADV, CATORDER_LS_NAVMESHSIMPADV)]
        public float MaxBorderHeightError
        {
            get { return m_maxBorderHeightError; }
            set { m_maxBorderHeightError = value; SetEngineInstanceBaseProperties(); }
        }

        [PropertyOrder(20)]
        [Description("When simplifying internal segments and convex boundary segments, this controls the maximum projected distance by which the simplified segment can deviate from the unsimplified segment.")]
        [SortedCategory(CAT_LS_NAVMESHSIMPADV, CATORDER_LS_NAVMESHSIMPADV)]
        public float MaxBorderDistanceError
        {
            get { return m_maxBorderDistanceError; }
            set { m_maxBorderDistanceError = value; SetEngineInstanceBaseProperties(); }
        }

        [PropertyOrder(21)]
        [Description("The maximum number of faces that can be included in a partition. Increasing this will result in fewer internal edges in large open areas, but may slow down generation times slightly.")]
        [SortedCategory(CAT_LS_NAVMESHSIMPADV, CATORDER_LS_NAVMESHSIMPADV)]
        public int MaxPartitionSize
        {
            get { return m_maxPartitionSize; }
            set { m_maxPartitionSize = value; SetEngineInstanceBaseProperties(); }
        }

        [PropertyOrder(22)]
        [Description("If this is true (and UseHeightPartitioning is true), a more conservative algorithm will be used to guarantee that the total vertical deviation between the simplified and unsimplified nav meshes does not exceed (MaxPartitionHeightError + HertelMehlhornHeightError). This will generally result is less simplification however.")]
        [SortedCategory(CAT_LS_NAVMESHSIMPADV, CATORDER_LS_NAVMESHSIMPADV)]
        public bool UseConservativeHeightPartitioning
        {
            get { return m_useConservativeHeightPartitioning; }
            set { m_useConservativeHeightPartitioning = value; SetEngineInstanceBaseProperties(); }
        }

        [PropertyOrder(23)]
        [Description("Additional height error allowed when merging faces into convex pieces. This is only used if UseConservativeHeightPartitioning is true.")]
        [SortedCategory(CAT_LS_NAVMESHSIMPADV, CATORDER_LS_NAVMESHSIMPADV)]
        public float HertelMehlhornHeightError
        {
            get { return m_hertelMehlhornHeightError; }
            set { m_hertelMehlhornHeightError = value; SetEngineInstanceBaseProperties(); }
        }

        [PropertyOrder(24)]
        [Description("Faces will only be merged if the angle between their normals is greater than this.")]
        [SortedCategory(CAT_LS_NAVMESHSIMPADV, CATORDER_LS_NAVMESHSIMPADV)]
        public float PlanarityThreshold
        {
            get { return m_planarityThresholdDeg; }
            set { m_planarityThresholdDeg = value; SetEngineInstanceBaseProperties(); }
        }

        [PropertyOrder(25)]
        [Description("Amount of non-convexity (in degrees) allowed when merging faces.")]
        [SortedCategory(CAT_LS_NAVMESHSIMPADV, CATORDER_LS_NAVMESHSIMPADV)]
        public float NonconvexityThreshold
        {
            get { return m_nonconvexityThreshold; }
            set { m_nonconvexityThreshold = value; SetEngineInstanceBaseProperties(); }
        }

        [PropertyOrder(26)]
        [Description("Connected sequences of boundary segments contained within a projected OBB of area less than this threshold will be removed if safe to do so.")]
        [SortedCategory(CAT_LS_NAVMESHSIMPADV, CATORDER_LS_NAVMESHSIMPADV)]
        public float BoundaryEdgeFilterThreshold
        {
            get { return m_boundaryEdgeFilterThreshold; }
            set { m_boundaryEdgeFilterThreshold = value; SetEngineInstanceBaseProperties(); }
        }

        [PropertyOrder(27)]
        [Description("The maximum allowable projected distance a vertex that isn't used by a boundary edge can be moved by when merging shared vertices.")]
        [SortedCategory(CAT_LS_NAVMESHSIMPADV, CATORDER_LS_NAVMESHSIMPADV)]
        public float MaxSharedVertexHorizontalError
        {
            get { return m_maxSharedVertexHorizontalError; }
            set { m_maxSharedVertexHorizontalError = value; SetEngineInstanceBaseProperties(); }
        }

        [PropertyOrder(28)]
        [Description("The maximum allowable vertical distance a vertex that isn't used by a boundary edge can be moved by when merging shared vertices.")]
        [SortedCategory(CAT_LS_NAVMESHSIMPADV, CATORDER_LS_NAVMESHSIMPADV)]
        public float MaxSharedVertexVerticalError
        {
            get { return m_maxSharedVertexVerticalError; }
            set { m_maxSharedVertexVerticalError = value; SetEngineInstanceBaseProperties(); }
        }

        [PropertyOrder(29)]
        [Description("The maximum allowable projected distance a vertex that is used by a boundary edge can be moved by when merging shared vertices.")]
        [SortedCategory(CAT_LS_NAVMESHSIMPADV, CATORDER_LS_NAVMESHSIMPADV)]
        public float MaxBoundaryVertexHorizontalError
        {
            get { return m_maxBoundaryVertexHorizontalError; }
            set { m_maxBoundaryVertexHorizontalError = value; SetEngineInstanceBaseProperties(); }
        }

        [PropertyOrder(30)]
        [Description("The maximum allowable vertical distance a vertex that is used by a boundary edge can be moved by when merging shared vertices.")]
        [SortedCategory(CAT_LS_NAVMESHSIMPADV, CATORDER_LS_NAVMESHSIMPADV)]
        public float MaxBoundaryVertexVerticalError
        {
            get { return m_maxBoundaryVertexVerticalError; }
            set { m_maxBoundaryVertexVerticalError = value; SetEngineInstanceBaseProperties(); }
        }

        [PropertyOrder(31)]
        [Description("Whether or not to merge longest edges first when performing Hertel-Mehlhorn. This can result in nicer looking nav meshes.")]
        [SortedCategory(CAT_LS_NAVMESHSIMPADV, CATORDER_LS_NAVMESHSIMPADV)]
        public bool MergeLongestEdgesFirst
        {
            get { return m_mergeLongestEdgesFirst; }
            set { m_mergeLongestEdgesFirst = value; SetEngineInstanceBaseProperties(); }
        }

        #endregion
        
        #region Serialization and Export

        protected HavokNavMeshLocalSettingsShape(SerializationInfo info, StreamingContext context) : base(info, context)
        {
            _vBoxSize = (Vector3F)info.GetValue("_vBoxSize", typeof(Vector3F));

            // Nav Mesh Generation Settings
            m_maxWalkableSlopeDeg = info.GetSingle("MaxWalkableSlope");

            // Nav Mesh Edge Matching Settings
            m_maxStepHeight = info.GetSingle("MaxStepHeight");
            m_maxSeparation = info.GetSingle("MaxSeparation");
            m_maxOverhang = info.GetSingle("MaxOverhang");
            m_planarAlignmentAngleDeg = info.GetSingle("PlanarAlignmentAngle");
            m_verticalAlignmentAngleDeg = info.GetSingle("VerticalAlignmentAngle");
            m_minEdgeOverlap = info.GetSingle("MinEdgeOverlap");

            // Nav Mesh Simplification Settings
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
        }

        public override CSharpFramework.Serialization.EditorPluginInfo GetPluginInformation()
        {
            return EditorPlugin.EDITOR_PLUGIN_INFO;
        }

        public override void GetObjectData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context)
        {
            base.GetObjectData (info, context);

            info.AddValue("_vBoxSize", _vBoxSize);

            // Nav Mesh Generation Settings
            info.AddValue("MaxWalkableSlope", m_maxWalkableSlopeDeg);

            // Nav Mesh Edge Matching Settings
            info.AddValue("MaxStepHeight", m_maxStepHeight);
            info.AddValue("MaxSeparation", m_maxSeparation);
            info.AddValue("MaxOverhang", m_maxOverhang);
            info.AddValue("PlanarAlignmentAngle", m_planarAlignmentAngleDeg);
            info.AddValue("VerticalAlignmentAngle", m_verticalAlignmentAngleDeg);
            info.AddValue("MinEdgeOverlap", m_minEdgeOverlap);

            // Nav Mesh Simplification Settings
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
        }

        public override bool OnExport(CSharpFramework.Scene.SceneExportInfo info)
        {
            return base.OnExport (info);
        }

        #endregion

        #region Hotspot

        HotSpotDistance _hotSpotSizeX;
        HotSpotDistance _hotSpotSizeY;
        HotSpotDistance _hotSpotSizeZ;

        public override void OnSelected()
        {
            base.OnSelected();
            System.Diagnostics.Debug.Assert(_hotSpotSizeX == null);
            System.Diagnostics.Debug.Assert(_hotSpotSizeY == null);
            System.Diagnostics.Debug.Assert(_hotSpotSizeZ == null);

            // hotspot for size X
            _hotSpotSizeX = new HotSpotDistance(this, @"textures\Hotspot_Rect.dds", VisionColors.Yellow, HotSpotBase.PickType.Square, 4.0f);
            _hotSpotSizeX.StartDistance = this.BoxSizeX;
            _hotSpotSizeX.DisplayScaling = 0.5f * ScaleX;
            _hotSpotSizeX.Axis = this.XAxis;
            _hotSpotSizeX.ToolTipText = "size x direction";
            EditorManager.ActiveView.HotSpots.Add(_hotSpotSizeX);

            // hotspot for size Y
            _hotSpotSizeY = new HotSpotDistance(this, @"textures\Hotspot_Rect.dds", VisionColors.Yellow, HotSpotBase.PickType.Square, 4.0f);
            _hotSpotSizeY.StartDistance = this.BoxSizeY;
            _hotSpotSizeY.DisplayScaling = 0.5f * ScaleY;
            _hotSpotSizeY.Axis = this.YAxis;
            _hotSpotSizeY.ToolTipText = "size y direction";
            EditorManager.ActiveView.HotSpots.Add(_hotSpotSizeY);

            // hotspot for size Z
            _hotSpotSizeZ = new HotSpotDistance(this, @"textures\Hotspot_Rect.dds", VisionColors.Yellow, HotSpotBase.PickType.Square, 4.0f);
            _hotSpotSizeZ.StartDistance = this.BoxSizeZ;
            _hotSpotSizeZ.DisplayScaling = 0.5f * ScaleZ;
            _hotSpotSizeZ.Axis = this.ZAxis;
            _hotSpotSizeZ.ToolTipText = "size z direction";
            EditorManager.ActiveView.HotSpots.Add(_hotSpotSizeZ);
        }

        void SafeRemoveHotSpot(HotSpotBase hotSpot)
        {
            if (hotSpot == null) return;
            EditorManager.ActiveView.HotSpots.Remove(hotSpot);
            hotSpot.Remove();
        }

        public override void OnUnSelected()
        {
            SafeRemoveHotSpot(_hotSpotSizeX); _hotSpotSizeX = null;
            SafeRemoveHotSpot(_hotSpotSizeY); _hotSpotSizeY = null;
            SafeRemoveHotSpot(_hotSpotSizeZ); _hotSpotSizeZ = null;

            base.OnUnSelected();
        }

        public override void OnHotSpotDragBegin(HotSpotBase hotSpot, VisionViewBase view)
        {
            if (hotSpot == _hotSpotSizeX)
                _hotSpotSizeX.StartDistance = this.BoxSizeX;
            if (hotSpot == _hotSpotSizeY)
                _hotSpotSizeY.StartDistance = this.BoxSizeY;
            if (hotSpot == _hotSpotSizeZ)
                _hotSpotSizeZ.StartDistance = this.BoxSizeZ;
        }

        public override void OnHotSpotDrag(HotSpotBase hotSpot, VisionViewBase view, float fDeltaX, float fDeltaY)
        {
            if (hotSpot == _hotSpotSizeX)
                this.BoxSizeX = _hotSpotSizeX.CurrentDistance;
            if (hotSpot == _hotSpotSizeY)
                this.BoxSizeY = _hotSpotSizeY.CurrentDistance;
            if (hotSpot == _hotSpotSizeZ)
                this.BoxSizeZ = _hotSpotSizeZ.CurrentDistance;
        }

        public override void OnHotSpotDragEnd(HotSpotBase hotSpot, VisionViewBase view)
        {
            if (hotSpot == _hotSpotSizeX)
            {
                if (_hotSpotSizeX.HasChanged)
                {
                    float fNewSize = _hotSpotSizeX.CurrentDistance;
                    this.BoxSizeX = _hotSpotSizeX.StartDistance; // set old value for the action
                    EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this, "BoxSizeX", fNewSize)); // send an action which sets the property from old value to new one
                }
            }
            if (hotSpot == _hotSpotSizeY)
            {
                if (_hotSpotSizeY.HasChanged)
                {
                    float fNewSize = _hotSpotSizeY.CurrentDistance;
                    this.BoxSizeY = _hotSpotSizeY.StartDistance; // set old value for the action
                    EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this, "BoxSizeY", fNewSize)); // send an action which sets the property from old value to new one
                }
            }
            if (hotSpot == _hotSpotSizeZ)
            {
                if (_hotSpotSizeZ.HasChanged)
                {
                    float fNewSize = _hotSpotSizeZ.CurrentDistance;
                    this.BoxSizeZ = _hotSpotSizeZ.StartDistance; // set old value for the action
                    EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this, "BoxSizeZ", fNewSize)); // send an action which sets the property from old value to new one
                }
            }
        }

        public override void OnHotSpotEvaluatePosition(HotSpotBase hotSpot)
        {
            if (hotSpot == _hotSpotSizeX)
            {
                _hotSpotSizeX.Axis = this.XAxis;
                _hotSpotSizeX.Visible = true;
                if (!_hotSpotSizeX.IsDragging)
                    _hotSpotSizeX.StartDistance = this.BoxSizeX;
            }
            if (hotSpot == _hotSpotSizeY)
            {
                _hotSpotSizeY.Axis = this.YAxis;
                _hotSpotSizeY.Visible = true;
                if (!_hotSpotSizeY.IsDragging)
                    _hotSpotSizeY.StartDistance = this.BoxSizeY;
            }
            if (hotSpot == _hotSpotSizeZ)
            {
                _hotSpotSizeZ.Axis = this.ZAxis;
                _hotSpotSizeZ.Visible = true;
                if (!_hotSpotSizeZ.IsDragging)
                    _hotSpotSizeZ.StartDistance = this.BoxSizeZ;
            }
        }

        public override void OnHotSpotRender(HotSpotBase hotSpot, VisionViewBase view, ShapeRenderMode mode)
        {
            base.OnHotSpotRender(hotSpot, view, mode);
        }

        #endregion
    }

    #endregion

    #region Shape Creator Plugin

    class HavokNavMeshLocalSettingsShapeCreator : CSharpFramework.IShapeCreatorPlugin
    {
        public HavokNavMeshLocalSettingsShapeCreator()
        {
            IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_navmeshlocalsettings.png"), Color.Magenta);
        }

        public override string GetPluginName()
        {
            return "Nav Mesh Local Settings";
        }

        public override string GetPluginCategory()
        {
            return "Havok AI";
        }

        public override string GetPluginDescription()
        {
            return "Creates a shape that can be used to override navigation mesh settings in a local region.";
        }

        public override ShapeBase CreateShapeInstance()
        {
            HavokNavMeshLocalSettingsShape shape = new HavokNavMeshLocalSettingsShape("NavMesh_LocalSettings");
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
