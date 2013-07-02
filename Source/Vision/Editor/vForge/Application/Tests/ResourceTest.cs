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
using NUnit.Core;
using NUnit.Framework;
using CSharpFramework;
using CSharpFramework.Shapes;
using System.IO;
using ManagedFramework;
using System.Collections.Specialized;
using CSharpFramework.AssetManagement;

namespace Editor.Tests
{
  [TestFixture]
  public class ResourceTest
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


    [Test]
    public void TestShadingModes()
    {
      CommonSubtestInit(@"HavokBinding\JointMuseum.scene");
      int iFrameCount = 20;

      AssetInfo[] allAssets = VisionAssetManager.GetAllResources();

      // render frames
      foreach (AssetInfo asset in allAssets)
      {
        EditorManager.EngineManager.SetResourcePreview(asset.NativePtr, asset.AssetType.TypeName, true);
        for (int i = 0; i < iFrameCount; i++)
        {
          EditorManager.ActiveView.UpdateView(true);
        }
      }
      EditorManager.EngineManager.SetResourcePreview(IntPtr.Zero, null, false);
      CommonSubtestDeInit();
    }


    // Deactivated test, as since the introduction of the asset system, generation of 
    // asset thumbnails via vForge is no relevant functionality any more.
    //[Test]
    public void DontTestThumbnailGeneration()
    {
      CommonSubtestInit(@"HavokBinding\JointMuseum.scene");

      AssetInfo[] allAssets = VisionAssetManager.GetAllResources();

      // delete all thumbnails
      foreach (AssetInfo asset in allAssets)
        EditorManager.EngineManager.DeleteThumbnail(asset.NativePtr, asset.AssetType.TypeName, true);

      // begin automated thumbnail generation
      EditorManager.EngineManager.BeginThumbnailGeneration();

      // automatically generate and save thumbnails
      foreach (AssetInfo asset in allAssets)
        EditorManager.EngineManager.DoThumbnailGeneration(asset.NativePtr, asset.AssetType.TypeName, true);

      // end automated thumbnail generation
      EditorManager.EngineManager.EndThumbnailGeneration();

      // check if thumbnails have been saved
      String filename = "";
      foreach (AssetInfo asset in allAssets)
      {
        EditorManager.EngineManager.GetThumbnailFilename(asset.NativePtr, ref filename, asset.AssetType.TypeName, true);
        if (filename != "")
          Assert.IsTrue(System.IO.File.Exists(filename));
      }

      CommonSubtestDeInit();
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
