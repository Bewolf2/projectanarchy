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
    #region class HavokNavMeshCarverShape

    [Serializable, ShapeTypeNiceName("Havok Nav Mesh Carver")]
    public class HavokNavMeshCarverShape : Shape3D
    {
        #region Category Sorting Definitions

        protected const string CAT_CARVERPROPS = "Carver Properties";
        protected const int CATORDER_CARVERPROPS = Shape3D.LAST_CATEGORY_ORDER_ID + 1;
        public new const int LAST_CATEGORY_ORDER_ID = CATORDER_CARVERPROPS;

        #endregion

        #region Constructor

        public HavokNavMeshCarverShape(string name) : base(name)
        {
        }

        #endregion

        #region Icon

        static int iIconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_navmeshcarver.png"), Color.Magenta);
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

            // Don't draw carvers if "playing the game" or 'Visible' property is false
            if (EditorManager.InPlayingMode || FinalVisibleState == false)
                return;

            Vector3F size = new Vector3F(_vBoxSize.X * ScaleX, _vBoxSize.Y * ScaleY, _vBoxSize.Z * ScaleZ);
            BoundingBox bbox = new BoundingBox(-size * 0.5f, size * 0.5f);

            if (_bInverted)
            {
                bbox.Translate(Position);
                uint color = (mode == ShapeRenderMode.Selected) ? (uint)0x6000ff80 : (uint)0x3000ff80;
                view.RenderSolidBox(bbox, color);
            }
            else
            {
                uint color = (mode == ShapeRenderMode.Selected) ? (uint)0x600080ff : (uint)0x300080ff;
                view.RenderOrientedSolidBox(bbox, Position, RotationMatrix, color);
            }
        }

        void UpdateHints()
        {
            if (_bInverted)
            {
                AddHint(HintFlags_e.NoRotation);
                Roll = 0.0f;
                Pitch = 0.0f;
                Yaw = 0.0f;
            }
            else
            {
                RemoveHint(HintFlags_e.NoRotation);
            }
        }

        public override void OnAddedToScene()
        {
            base.OnAddedToScene();
            UpdateHints();
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

        public bool IsInverted()
        {
            return _bInverted;
        }

        #endregion

        #region Cloning

        public override ShapeBase Clone()
        {
            HavokNavMeshCarverShape copy = base.Clone() as HavokNavMeshCarverShape;

            copy._hotSpotSizeX = null;
            copy._hotSpotSizeY = null;
            copy._hotSpotSizeZ = null;
            System.Diagnostics.Debug.Assert(!copy.Selected); // turned off in ShapeBase

            return copy;
        }

        #endregion

        #region Extra Properties

        Vector3F _vBoxSize = new Vector3F(300,300,200);
        bool _bInverted = false;

        [PropertyOrder(1)]
        [SortedCategory(CAT_CARVERPROPS, CATORDER_CARVERPROPS)]
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
        [SortedCategory(CAT_CARVERPROPS, CATORDER_CARVERPROPS)]
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
        [SortedCategory(CAT_CARVERPROPS, CATORDER_CARVERPROPS)]
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
        [SortedCategory(CAT_CARVERPROPS, CATORDER_CARVERPROPS)]
        public bool Inverted
        {
            get { return _bInverted; }
            set
            {
                if (_bInverted == value) return;
                _bInverted = value;
                UpdateHints();
            }
        }

        #endregion
        
        #region Serialization and Export

        protected HavokNavMeshCarverShape(SerializationInfo info, StreamingContext context) : base(info, context)
        {
            _vBoxSize = (Vector3F)info.GetValue("_vBoxSize", typeof(Vector3F));
            _bInverted = info.GetBoolean("_bInverted");
        }

        public override CSharpFramework.Serialization.EditorPluginInfo GetPluginInformation()
        {
            return EditorPlugin.EDITOR_PLUGIN_INFO;
        }

        public override void GetObjectData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context)
        {
            base.GetObjectData (info, context);
            info.AddValue("_vBoxSize", _vBoxSize);
            info.AddValue("_bInverted", _bInverted);
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

    class HavokNavMeshCarverShapeCreator : CSharpFramework.IShapeCreatorPlugin
    {
        public HavokNavMeshCarverShapeCreator()
        {
            IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_navmeshcarver.png"), Color.Magenta);
        }

        public override string GetPluginName()
        {
            return "Nav Mesh Carver";
        }

        public override string GetPluginCategory()
        {
            return "Havok AI";
        }

        public override string GetPluginDescription()
        {
            return "Creates a shape that can be used to carve box-shaped holes in a navigation mesh.";
        }

        public override ShapeBase CreateShapeInstance()
        {
            HavokNavMeshCarverShape shape = new HavokNavMeshCarverShape("NavMesh_Carver");
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
