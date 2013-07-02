/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Design;
using System.IO;
using System.Runtime.Serialization;
using CSharpFramework;
using CSharpFramework.Actions;
using CSharpFramework.Math;
using CSharpFramework.PropertyEditors;
using CSharpFramework.Shapes;
using CSharpFramework.View;
using CSharpFramework.Serialization;

namespace HavokAiEditorPlugin.Shapes
{
    #region class HavokNavMeshTestPathShape

    [Serializable, ShapeTypeNiceName("Havok Nav Mesh Test Path")]
    public class HavokNavMeshTestPathShape : Shape3D
    {
        #region Category Sorting Definitions

        protected const string CAT_TESTPATHPROPS = "Test Path Properties";
        protected const int CATORDER_TESTPATHPROPS = Shape3D.LAST_CATEGORY_ORDER_ID + 1;
        public new const int LAST_CATEGORY_ORDER_ID = CATORDER_TESTPATHPROPS;

        #endregion
       
        #region Constructor

        public HavokNavMeshTestPathShape(string name) : base(name)
        {
            AddHint(HintFlags_e.NoScale | HintFlags_e.NoRotation);
        }

        #endregion

        #region Icon

        static int iIconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_navmeshtestpath.png"), Color.Magenta);
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
            if (FinalVisibleState == true && EditorManager.Scene.AllShapesOfType(typeof(HavokAiEditorPlugin.Shapes.HavokNavMeshShape)).Count != 0)
            {
                HavokAiManaged.ManagedModule.ComputeAndDrawPath(view, _vStartPoint, _vEndPoint, _fCharacterRadius, _fCharacterHeight, _fpathDisplayOffset * _fCharacterHeight, VisionColors.Get(_pathColor));
            }

            base.RenderShape(view, mode);
        }

        #endregion

        #region Extra Properties

        Vector3F _vStartPoint = -150.0f * Vector3F.YAxis;
        Vector3F _vEndPoint = +150.0f * Vector3F.YAxis;
        float _fCharacterRadius = 20.0f;    // 40cm using the default conversion factor
        float _fCharacterHeight = 175.0f;
        Color _pathColor = System.Drawing.Color.Yellow;
        float _fpathDisplayOffset = 0.08f;

        [PropertyOrder(1)]
        [SortedCategory(CAT_TESTPATHPROPS, CATORDER_TESTPATHPROPS)]
        public Vector3F StartPoint
        {
            get { return _vStartPoint; }
            set
            {
                _vStartPoint = value;
                if (_hotSpotStartPoint != null)
                    _hotSpotStartPoint.StartPosition = _vStartPoint - Position;
            }
        }

        [PropertyOrder(2)]
        [SortedCategory(CAT_TESTPATHPROPS, CATORDER_TESTPATHPROPS)]
        public Vector3F EndPoint
        {
            get { return _vEndPoint; }
            set
            {
                _vEndPoint = value;
                if (_hotSpotEndPoint != null)
                    _hotSpotEndPoint.StartPosition = _vEndPoint - Position;
            }
        }

        [PropertyOrder(3)]
        [SortedCategory(CAT_TESTPATHPROPS, CATORDER_TESTPATHPROPS)]
        public float CharacterRadius
        {
            get { return _fCharacterRadius; }
            set
            {
                _fCharacterRadius = value;
                if (_fCharacterRadius < 0.00001f)
                    _fCharacterRadius = 0.00001f;
            }
        }

        [PropertyOrder(4)]
        [SortedCategory(CAT_TESTPATHPROPS, CATORDER_TESTPATHPROPS)]
        public float CharacterHeight
        {
            get { return _fCharacterHeight; }
            set
            {
                _fCharacterHeight = value;
                if (_fCharacterHeight < 0.00001f)
                    _fCharacterHeight = 0.00001f;
            }
        }

        [PropertyOrder(5)]
        [SortedCategory(CAT_TESTPATHPROPS, CATORDER_TESTPATHPROPS)]
        [EditorAttribute(typeof(StandardColorDropDownEditor), typeof(UITypeEditor))]
        public Color PathColor
        {
            get { return _pathColor; }
            set { _pathColor = value; }
        }

        [PropertyOrder(6)]
        [SortedCategory(CAT_TESTPATHPROPS, CATORDER_TESTPATHPROPS)]
        public float PathDisplayOffset
        {
            get { return _fpathDisplayOffset; }
            set
            {
                _fpathDisplayOffset = value;
                if (_fpathDisplayOffset < 0.00001f)
                    _fpathDisplayOffset = 0.00001f;
            }
        }

        #endregion

        #region Serialization and Export

        protected HavokNavMeshTestPathShape(SerializationInfo info, StreamingContext context) : base(info, context)
        {
            AddHint(HintFlags_e.NoScale | HintFlags_e.NoRotation);

            _vStartPoint = (Vector3F)info.GetValue("_vStartPoint", typeof(Vector3F));
            _vEndPoint = (Vector3F)info.GetValue("_vEndPoint", typeof(Vector3F));
            _fCharacterRadius = info.GetSingle("_fCharacterRadius");
            if (SerializationHelper.HasElement(info, "_pathColor"))
                _pathColor = (Color)info.GetValue("_pathColor", typeof(Color));
            if (SerializationHelper.HasElement(info, "_fpathDisplayOffset"))
                _fpathDisplayOffset = info.GetSingle("_fpathDisplayOffset");
        }

        public override CSharpFramework.Serialization.EditorPluginInfo GetPluginInformation()
        {
            return EditorPlugin.EDITOR_PLUGIN_INFO;
        }

        public override void GetObjectData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context)
        {
            base.GetObjectData(info, context);
            info.AddValue("_vStartPoint", _vStartPoint);
            info.AddValue("_vEndPoint", _vEndPoint);
            info.AddValue("_fCharacterRadius", _fCharacterRadius);
            info.AddValue("_pathColor", _pathColor);
            info.AddValue("_fpathDisplayOffset", _fpathDisplayOffset);
        }

        public override bool OnExport(CSharpFramework.Scene.SceneExportInfo info)
        {
            return base.OnExport (info);
        }

        #endregion

        #region Selection and HotSpots

        HotSpot3D _hotSpotStartPoint = null;
        HotSpot3D _hotSpotEndPoint = null;

        void UpdateTangentHotSpots()
        {
            bool bRemoveIn = true;
            bool bRemoveOut = true;
            if (Selected)
            {
                bRemoveIn = false;
                bRemoveOut = false;
            }

            if (bRemoveIn && _hotSpotStartPoint != null)
            {
                EditorManager.ActiveView.HotSpots.Remove(_hotSpotStartPoint);
                _hotSpotStartPoint = null;
            }
            else if (!bRemoveIn && _hotSpotStartPoint == null)
            {
               _hotSpotStartPoint = new HotSpot3D(this, 32.0f);
               _hotSpotStartPoint.ToolTipText = "Start point";
               _hotSpotStartPoint.StartPosition = _vStartPoint - Position;
               EditorManager.ActiveView.HotSpots.Add(_hotSpotStartPoint);
            }

            if (bRemoveOut && _hotSpotEndPoint != null)
            {
                EditorManager.ActiveView.HotSpots.Remove(_hotSpotEndPoint);
                _hotSpotEndPoint = null;
            }
            else if (!bRemoveOut && _hotSpotEndPoint == null)
            {
                _hotSpotEndPoint = new HotSpot3D(this, 32.0f);
                _hotSpotEndPoint.ToolTipText = "End point";
                _hotSpotEndPoint.StartPosition = _vEndPoint - Position;
                EditorManager.ActiveView.HotSpots.Add(_hotSpotEndPoint);
            }
        }

        public override void OnSelected()
        {
            base.OnSelected();
            UpdateTangentHotSpots();
        }

        public override void OnUnSelected()
        {
            base.OnUnSelected();
            UpdateTangentHotSpots();
        }

        public override void OnHotSpotDragBegin(HotSpotBase hotSpot, VisionViewBase view)
        {
            base.OnHotSpotDragBegin(hotSpot, view);
        }

        public override void OnHotSpotDrag(HotSpotBase hotSpot, VisionViewBase view, float fDeltaX, float fDeltaY)
        {
            base.OnHotSpotDrag(hotSpot, view, fDeltaX, fDeltaY);
        }

        public override void OnHotSpotDragEnd(HotSpotBase hotSpot, VisionViewBase view)
        {
            base.OnHotSpotDragEnd(hotSpot, view);
            if (hotSpot == _hotSpotStartPoint)
            {
                Vector3F vNewPos = _hotSpotStartPoint.CurrentPosition + Position;
                _vStartPoint = _hotSpotStartPoint.StartPosition; // set old value for the action
                EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this, "StartPoint", vNewPos)); // send an action which sets the property from old value to new one
            }
            else if (hotSpot == _hotSpotEndPoint)
            {
                Vector3F vNewPos = _hotSpotEndPoint.CurrentPosition + Position;
                _vEndPoint = _hotSpotEndPoint.StartPosition; // set old value for the action
                EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this, "EndPoint", vNewPos)); // send an action which sets the property from old value to new one
            }
        }

        public override void OnHotSpotEvaluatePosition(HotSpotBase hotSpot)
        {
            base.OnHotSpotEvaluatePosition(hotSpot);
            if (hotSpot == _hotSpotStartPoint && _hotSpotStartPoint.IsDragging)
            {
                StartPoint = _hotSpotStartPoint.CurrentPosition + Position;
            }
            else if (hotSpot == _hotSpotEndPoint && _hotSpotEndPoint.IsDragging)
            {
                EndPoint = _hotSpotEndPoint.CurrentPosition + Position;
            }
        }

        public override void OnHotSpotRender(HotSpotBase hotSpot, VisionViewBase view, ShapeRenderMode mode)
        {
            base.OnHotSpotRender(hotSpot, view, mode);
            view.RenderLine(_hotSpotStartPoint.CurrentPosition + Position, _hotSpotEndPoint.CurrentPosition + Position, VisionColors.Grey, 2.0f);
        }

        public override void OnTransformationChanged()
        {
            base.OnTransformationChanged();
            if (_hotSpotStartPoint != null)
            {
                StartPoint = _hotSpotStartPoint.CurrentPosition;
            }
            StartPoint += Position;
            if (_hotSpotEndPoint != null)
            {
                EndPoint = _hotSpotEndPoint.CurrentPosition;
            }
            EndPoint += Position;
        }

        #endregion
    }

    #endregion

    #region Shape Creator Plugin

    class HavokNavMeshTestPathShapeCreator : CSharpFramework.IShapeCreatorPlugin
    {
        public HavokNavMeshTestPathShapeCreator()
        {
            IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_navmeshtestpath.png"), Color.Magenta);
        }

        public override string GetPluginName()
        {
            return "Nav Mesh Test Path";
        }

        public override string GetPluginCategory()
        {
            return "Havok AI";
        }

        public override string GetPluginDescription()
        {
            return "Creates a shape that represents a path on a navigation mesh. This is useful for validating nav mesh generation results.";
        }

        public override ShapeBase CreateShapeInstance()
        {
            HavokNavMeshTestPathShape shape = new HavokNavMeshTestPathShape("NavMesh_TestPath");
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
