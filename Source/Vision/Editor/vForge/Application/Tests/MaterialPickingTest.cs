/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections.Generic;
using System.Text;
using CSharpFramework;
using NUnit.Framework;
using System.IO;
using CSharpFramework.Shapes;
using ManagedBase;
using CSharpFramework.Scene;

namespace VisionEditorPlugin.Tests
{
  [TestFixture]
  public class MaterialPickingTest
  {
    /// <summary>
    /// Test function
    /// </summary>
    [SetUp]
    public void SetUp()
    {
    }


    void PickMaterials()
    {
      List<Material> pickedMaterials = new List<Material>();
      int iResX = EditorManager.ActiveView.ClientSize.Width;
      int iResY = EditorManager.ActiveView.ClientSize.Height;

      int iSubDiv = 50;
      for (int y = 0; y<iSubDiv;y++)
        for (int x = 0; x<iSubDiv;x++)
        {
          float sx = (float)x * (float)iResX / (float)iSubDiv;
          float sy = (float)y * (float)iResY / (float)iSubDiv;
          // render cross
          EditorManager.EngineManager.RenderLine2D(sx - 5.0f, sy, sx + 5.0f, sy, VisionColors.Cyan, 1.0f);
          EditorManager.EngineManager.RenderLine2D(sx, sy - 5.0f, sx, sy + 5.0f, VisionColors.Cyan, 1.0f);

          Material mat = EditorManager.EngineManager.PickMaterial(sx, sy, true, true, true);
          // there are holes in the map so dont assert here:
          if (mat == null)
            continue;

//          Assert.IsNotNull(mat, "There must be a material under the mouse: mx=" + ((int)sx).ToString() + ", my=" + ((int)sy).ToString());
          if (!pickedMaterials.Contains(mat))
            pickedMaterials.Add(mat);

          int yk = 32;
          EditorManager.EngineManager.WriteText2D(10.0f, 10.0f, "Picked materials:", VisionColors.White);
          foreach (Material m in pickedMaterials)
          {
            EditorManager.EngineManager.WriteText2D(10.0f, (float)yk, m.Name, VisionColors.Yellow);
            yk += 12;
          }

          EditorManager.ActiveView.UpdateView(true);
        }

      Assert.IsTrue(pickedMaterials.Count > 10);   
    }

    [Test]
    public void TestPickMaterials()
    {
      CommonSubtestInit(@"HavokBinding\JointMuseum_Deferred.scene"); 
      PickMaterials();
      CommonSubtestDeInit();

      CommonSubtestInit(@"HavokBinding\JointMuseum.scene");
      PickMaterials();
      CommonSubtestDeInit();
    }


    /// <summary>
    /// Clears the test data
    /// </summary>
    [TearDown]
    public void TearDown()
    {
      if (EditorManager.Project != null)
        TestManager.Helpers.CloseTestProject();
    }



    void CommonSubtestInit(string sceneName)
    {
      TestManager.Helpers.OpenSceneFromFile(Path.Combine(TestManager.Helpers.TestDataDir, sceneName));

      ShapeBase cam = EditorManager.Scene.FindShapeByName("CameraPosition");
      if (cam != null)
        cam.OnDoubleClick();

      // Start the simulation
      EditorManager.EditorMode = EditorManager.Mode.EM_ANIMATING;

      // Do NOT call ProcessEvents here. This will cause vForge to
      // get stuck in a timer based UpdateView loop. Everything runs fine
      // in normal operation, thus ProcessEvent seems be evil at this
      // point in time
      // [REMOVED]: TestManager.Helpers.ProcessEvents();
    }

    void CommonSubtestDeInit()
    {
      // Stop simulation, set back
      EditorManager.EditorMode = EditorManager.Mode.EM_NONE;
      EditorManager.VisibilityBuilder.UseInEngine = false;

      TestManager.Helpers.CloseActiveProject();
    }

  }
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
