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
using NUnit.Core;
using NUnit.Framework;
using CSharpFramework;
using CSharpFramework.Shapes;
using System.IO;
using ManagedFramework;
using System.Collections.Specialized;

namespace Editor.Tests
{
#if _VR_DX9
  [TestFixture]
  public class VLuxTest
  {
    [SetUp]
    public void SetUp()
    {
    }

    /// <summary>
    /// Clears the test data
    /// </summary>
    [TearDown]
    public void TearDown()
    {
    }

    void CommonSubtestInit(string sceneName)
    {
      TestManager.Helpers.OpenSceneFromFile(Path.Combine(TestManager.Helpers.TestDataDir, sceneName));

      ShapeBase cam = EditorManager.Scene.FindShapeByName("CameraPosition");
      if (cam != null)
        cam.OnDoubleClick();

      // Start the simulation
      EditorManager.EditorMode = EditorManager.Mode.EM_ANIMATING;
      TestManager.Helpers.ProcessEvents();
    }

    void CommonSubtestDeInit()
    {
      // Stop simulation, set back
      EditorManager.EditorMode = EditorManager.Mode.EM_NONE;
      EditorManager.VisibilityBuilder.UseInEngine = false;

      TestManager.Helpers.CloseActiveProject();
    }

    void DeleteLitFileForScene(string sceneName)
    {
      string fileName = Path.Combine(TestManager.Helpers.TestDataDir, sceneName + IScene.LayerDirectoryExtension + @"\StaticLightingInfo.lit");
      if (File.Exists(fileName))
        File.Delete(fileName);
    }


    void LightAndRunScene(string sceneName)
    {
      string vsceneName = Path.ChangeExtension(Path.GetFileName(sceneName), ".vscene");

      DeleteLitFileForScene(sceneName); // before the scene starts
      CommonSubtestInit(sceneName);
      int iFrameCount = 2000;

      VisionEngineManager em = (VisionEngineManager)EditorManager.EngineManager;
      StringCollection names = new StringCollection();
      em.GetReplacementRenderLoopEffects(names);
      int iLightShading = names.IndexOf("Lighting Only");
      Assert.IsTrue(iLightShading >= 0, "Lighting only shading not available");

      // render frames
      EditorManager.Scene.ResetStaticLighting();
      for (int i = 0; i < iFrameCount; i++)
      {
        EditorManager.EngineManager.WriteText2D(10.0f, 10.0f, "Unlit version of the scene", VisionColors.White);
        EditorManager.ActiveView.UpdateView(true);
      }

      // get the currently set lighting tool (might be Beast or something)
      IStaticLightingTool lt = EditorManager.LightingTool;
      // set vLux as the current tool
      EditorManager.LightingTool = new VLux.VLuxLightingTool ();

      Assert.IsTrue(EditorApp.Scene.UpdateStaticLighting(false, null, true)); // without dialog

      // reset to the original lighting tool
      EditorManager.LightingTool = lt;

      for (int i = 0; i < iFrameCount; i++)
      {
        EditorManager.EngineManager.WriteText2D(10.0f, 10.0f, "..after vLux lighting", VisionColors.White);
        EditorManager.ActiveView.UpdateView(true);
      }
      em.SetReplacementRenderLoopEffect(iLightShading);
      for (int i = 0; i < iFrameCount; i++)
      {
        EditorManager.EngineManager.WriteText2D(10.0f, 10.0f, "..after vLux lighting light shading only", VisionColors.White);
        EditorManager.ActiveView.UpdateView(true);
      }
      em.SetReplacementRenderLoopEffect(-1);


      // now export and see whether projector looks identically
      EditorManager.Scene.ExportScene(null, false);
      EditorManager.Scene.Close();
      TestManager.Helpers.LoadExportedScene(vsceneName);
      for (int i = 0; i < iFrameCount; i++)
      {
        EditorManager.EngineManager.WriteText2D(10.0f, 10.0f, "This is the exported version", VisionColors.White);
        EditorManager.ActiveView.UpdateView(true);
      }
      TestManager.Helpers.CloseExportedScene();

      // reload the scene
      CommonSubtestInit(sceneName);
      // render frames
      EditorManager.Scene.ResetStaticLighting();
      for (int i = 0; i < iFrameCount; i++)
      {
        EditorManager.EngineManager.WriteText2D(10.0f, 10.0f, "Unlit version of the scene (again)", VisionColors.White);
        EditorManager.ActiveView.UpdateView(true);
      }
      // export again to restore SVN files
      EditorManager.Scene.ExportScene(null, false);


      CommonSubtestDeInit();
    }

#if !HK_ANARCHY
    [Test]
    public void ReferencedTerrain()
    {
      LightAndRunScene(@"MiscScenes\OriginalTerrain.scene");
      LightAndRunScene(@"MiscScenes\ReferencedTerrain.scene");
    }
#endif

    [Test]
    public void CustomRadiosityShader()
    {
      LightAndRunScene(@"MiscScenes\CustomRadiosityShader.scene");
    }

    [Test]
    public void TestModelLightmaps()
    {
      LightAndRunScene(@"ModelLightmap\Test.scene");
    }

    [Test]
    public void TestLightLandscape()
    {
      LightAndRunScene(@"MiscScenes\vLuxTest.scene");
    }

    
    [Test]
    public void TestLightNestedPrefabs()
    {
      string scene = @"MiscScenes\NestedPrefabsLM.scene";
      LightAndRunScene(scene);

      CommonSubtestInit(scene);
      Layer layer = EditorManager.Scene.Layers[0];
      uint iID1 = layer.GetCurrentShapeIDWatermark();

      // resave layer
      layer.Modified = true;
      Assert.IsTrue(layer.SaveToFile());

      // load scene again
      CommonSubtestInit(scene);
      layer = EditorManager.Scene.Layers[0];
      uint iID2 = layer.GetCurrentShapeIDWatermark();
      Assert.AreEqual(iID1, iID2);

      CommonSubtestDeInit();
    }
  }
#endif
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
