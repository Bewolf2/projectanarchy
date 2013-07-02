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
using CSharpFramework;
using CSharpFramework.Shapes;

namespace HavokAiEditorPlugin.Shapes
{
    #region class HavokNavMeshSeedPointShape

    [Serializable, ShapeTypeNiceName("Havok Nav Mesh Seed Point")]
    public class HavokNavMeshSeedPointShape : Shape3D
    {
        #region Constructor

        public HavokNavMeshSeedPointShape(string name) : base(name)
        {
            AddHint(HintFlags_e.NoScale | HintFlags_e.NoRotation);
        }

        #endregion

        #region Icon

        static int iIconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_navmeshseedpoint.png"), Color.Magenta);
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

            // Don't draw seed points if "playing the game" or 'Visible' property is false
            if (EditorManager.InPlayingMode || FinalVisibleState == false)
                return;

            if (mode == ShapeRenderMode.Selected || (mode == ShapeRenderMode.Normal && this.ParentLayer == EditorManager.Scene.ActiveLayer))
            {
                float fSize = 10.0f * EditorManager.Settings.GlobalUnitScaling;
                view.EngineManager.RenderSprite(this.RenderPosition, @"textures\pin_green32.dds", VisionColors.White, fSize, fSize);
            }
        }

        #endregion
        
        #region Serialization and Export

        protected HavokNavMeshSeedPointShape(SerializationInfo info, StreamingContext context) : base(info, context)
        {
            AddHint(HintFlags_e.NoScale | HintFlags_e.NoRotation);
        }

        public override CSharpFramework.Serialization.EditorPluginInfo GetPluginInformation()
        {
            return EditorPlugin.EDITOR_PLUGIN_INFO;
        }

        #endregion
    }

    #endregion

    #region Shape Creator Plugin

    class HavokNavMeshSeedPointShapeCreator : CSharpFramework.IShapeCreatorPlugin
    {
        public HavokNavMeshSeedPointShapeCreator()
        {
            IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_navmeshseedpoint.png"), Color.Magenta);
        }

        public override string GetPluginName()
        {
            return "Nav Mesh Seed Point";
        }

        public override string GetPluginCategory()
        {
            return "Havok AI";
        }

        public override string GetPluginDescription()
        {
            return "Creates a shape that represents a seed point used for region pruning during navigation mesh generation.";
        }

        public override ShapeBase CreateShapeInstance()
        {
            HavokNavMeshSeedPointShape shape = new HavokNavMeshSeedPointShape("NavMesh_SeedPoint");
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
