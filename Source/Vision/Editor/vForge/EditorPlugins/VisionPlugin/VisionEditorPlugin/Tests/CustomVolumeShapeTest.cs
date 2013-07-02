/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using CSharpFramework;
using CSharpFramework.Tests;
using NUnit.Framework;
using System.IO;
using VisionEditorPlugin.Shapes;
using CSharpFramework.Actions;
using CSharpFramework.Math;
using CSharpFramework.Scene;
using VisionManaged;

namespace VisionEditorPlugin.Tests
{
  [TestFixture]
  public class CustomVolumeAndLightClippingVolumeTest
  {
    /// <summary>
    /// Test function
    /// </summary>
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

    void AddVolumeVertex(CustomVolumeShape volume, Vector3F position)
    {
      CustomVolumeVertex vertex = new CustomVolumeVertex();
      EditorManager.Actions.Add(AddShapeAction.CreateAddShapeAction(vertex, volume, volume.ParentLayer, true));
      vertex.CreateEngineInstance(true);
      vertex.Position = position;
    }

    ShapeComponent CreateLightClippingComponent(CustomVolumeShape volume)
    {
      ShapeComponentType t = (ShapeComponentType)EditorManager.EngineManager.ComponentClassManager.GetCollectionType("VLightClippingVolumeComponent");
      Assert.IsNotNull(t);
      if (t == null)
        return null;

      ShapeComponent comp = (ShapeComponent)t.CreateInstance(null);
      comp.SetPropertyValue("Volume", volume);

      return comp;
    }

    [Test]
    public void TestVolumeCreation()
    {
      TestManager.Helpers.OpenSceneFromFile(Path.Combine(TestManager.Helpers.TestDataDir, @"CustomVolumeShapeTests\empty.scene"));

      CustomVolumeShape volume = new CustomVolumeShape();
      Layer layer = EditorManager.Scene.ActiveLayer;
      EditorManager.Actions.Add(new AddShapeAction(volume, null, layer, false));
      volume.Preview = true;

      AddVolumeVertex(volume, new Vector3F(-200.0f, -200.0f, -136.0f));
      EditorManager.ActiveView.UpdateView(true);
      AddVolumeVertex(volume, new Vector3F( 200.0f, -200.0f, -136.0f));
      EditorManager.ActiveView.UpdateView(true);
      AddVolumeVertex(volume, new Vector3F( 200.0f,  200.0f, -136.0f));
      EditorManager.ActiveView.UpdateView(true);
      AddVolumeVertex(volume, new Vector3F(   0.0f,  250.0f, -136.0f));
      EditorManager.ActiveView.UpdateView(true);
      AddVolumeVertex(volume, new Vector3F(-200.0f,  200.0f, -136.0f));
      EditorManager.ActiveView.UpdateView(true);

      volume.Height = 100.0f;
      EditorManager.ActiveView.UpdateView(true);

      DynLightShape light = new DynLightShape("test light");
      light.LightType = LightSourceType_e.Point;
      light.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
      EditorManager.Actions.Add(AddShapeAction.CreateAddShapeAction(light, null, layer, false));

      ShapeComponent component = CreateLightClippingComponent(volume);
      EditorManager.Actions.Add(new AddShapeComponentAction(light, component));

      for (int i = 0; i < 60; i++)
      {
        EditorManager.ActiveView.UpdateView(true);
      }

      bool success = EditorManager.Scene.SaveAs("volume.scene");
      Assert.IsTrue(success);
      EditorManager.Scene.Close();
      TestManager.Helpers.OpenSceneFromFile(Path.Combine(TestManager.Helpers.TestDataDir, @"CustomVolumeShapeTests\volume.scene"));

      for (int i = 0; i < 60; i++)
      {
        EditorManager.EngineManager.WriteText2D(10.0f, 10.0f, "This is the saved version", VisionColors.White);
        EditorManager.ActiveView.UpdateView(true);
      }

      EditorManager.EditorMode = EditorManager.Mode.EM_PLAYING_IN_EDITOR;
      for (int i = 0; i < 30; i++)
      {
        EditorManager.EngineManager.WriteText2D(10.0f, 10.0f, "Play in editor", VisionColors.White);
        EditorManager.ActiveView.UpdateView(true);
      }
      EditorManager.EditorMode = EditorManager.Mode.EM_NONE;

      EditorManager.Scene.ExportScene(null, false);
      EditorManager.Scene.Close();
      EditorManager.Scene = null;
      TestManager.Helpers.LoadExportedScene("volume.vscene");
      for (int i = 0; i < 60; i++)
      {
        EditorManager.EngineManager.WriteText2D(10.0f, 10.0f, "This is the exported version", VisionColors.White);
        EditorManager.ActiveView.UpdateView(true);
      }

      EditorManager.EditorMode = EditorManager.Mode.EM_NONE;
      TestManager.Helpers.CloseExportedScene();
      TestManager.Helpers.CloseActiveProject();
    }

    [Test]
    public void TestVolumeStaticMesh()
    {
      TestManager.Helpers.OpenSceneFromFile(Path.Combine(TestManager.Helpers.TestDataDir, @"CustomVolumeShapeTests\empty.scene"));

      CustomVolumeShape volume = new CustomVolumeShape();
      Layer layer = EditorManager.Scene.ActiveLayer;
      EditorManager.Actions.Add(AddShapeAction.CreateAddShapeAction(volume, null, layer, false));
      volume.Preview = true;
      volume.CustomStaticMesh = true;
      volume.StaticMeshPath = "Meshes\\kugel.vmesh";
      volume.Scaling = new Vector3F(20.0f, 20.0f, 20.0f);

      DynLightShape light = new DynLightShape("test light");
      light.LightType = LightSourceType_e.Point;
      light.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
      EditorManager.Actions.Add(AddShapeAction.CreateAddShapeAction(light, null, layer, false));

      ShapeComponent component = CreateLightClippingComponent(volume);
      EditorManager.Actions.Add(new AddShapeComponentAction(light, component));

      for (int i = 0; i < 60; i++)
      {
        EditorManager.ActiveView.UpdateView(true);
      }

      bool success = EditorManager.Scene.SaveAs("volume.scene");
      Assert.IsTrue(success);
      EditorManager.Scene.Close();
      TestManager.Helpers.OpenSceneFromFile(Path.Combine(TestManager.Helpers.TestDataDir, @"CustomVolumeShapeTests\volume.scene"));

      for (int i = 0; i < 60; i++)
      {
        EditorManager.EngineManager.WriteText2D(10.0f, 10.0f, "This is the saved version", VisionColors.White);
        EditorManager.ActiveView.UpdateView(true);
      }

      EditorManager.Scene.ExportScene(null, false);
      EditorManager.Scene.Close();
      EditorManager.Scene = null;
      TestManager.Helpers.LoadExportedScene("volume.vscene");
      for (int i = 0; i < 60; i++)
      {
        EditorManager.EngineManager.WriteText2D(10.0f, 10.0f, "This is the exported version", VisionColors.White);
        EditorManager.ActiveView.UpdateView(true);
      }

      EditorManager.EditorMode = EditorManager.Mode.EM_NONE;
      TestManager.Helpers.CloseExportedScene();
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
