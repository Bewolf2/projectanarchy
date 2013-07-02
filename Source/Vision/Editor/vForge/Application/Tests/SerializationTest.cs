/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Reflection;
using CSharpFramework;
using CSharpFramework.Serialization;
using CSharpFramework.Tests;
using CSharpFramework.Shapes;
using NUnit.Framework;
using ManagedFramework;
using CSharpFramework.DynamicProperties;
using System.IO;

namespace Editor.Tests
{
  /// <summary>
  /// Serialization Test class
  /// </summary>
  [TestFixture]
  public class SerializationTest
  {
    /// <summary>
    /// Test function
    /// </summary>
    [Test]
    public void TestShapeSerialization()
    {
      const string TEST_SCENENAME = "TestShapeSerialization.scene";

      // setup new test project/scene
      TestManager.Helpers.CreateTestScene(TEST_SCENENAME);
      EditorProject project = EditorApp.Project;

      try
      {
        // create all registered shapes
        foreach (IShapeCreatorPlugin plugin in EditorManager.ShapeCreatorPlugins)
        {
            plugin.ExecutePlugin();
        }
        string projectFile = project.PathName;
        int numCreatedShapes = CountShapeInstances(project.Scene);

        // cause engine view to refresh (to see that rendering the shapes doesn't cause a crash)
        TestManager.Helpers.ProcessEvents();

        // close map
        project.Save();

        ShapeCollection c1 = project.Scene.ActiveLayer.Root.ChildCollection;

        EditorManager.Scene.Close();
        project.Close();

        // reload map
        project = EditorManager.ProjectFactory.CreateInstance(null) as EditorProject;
        Assert.IsNotNull(project);

        project.Load(projectFile);
        EditorManager.Project = project;
        bool bResult = project.OpenScene(TEST_SCENENAME);
        Assert.IsTrue(bResult);

        // verify that shape instances exist
        int numLoadedShapes = CountShapeInstances(project.Scene);
        ShapeCollection c2 = project.Scene.ActiveLayer.Root.ChildCollection;

        for (int i = 0; i < c1.Count; i++)
        {
          ShapeBase s1 = c1[i];
          ShapeBase s2 = c2[i];
          Assert.AreEqual(s1.GetType(), s2.GetType());
        }

        Assert.AreEqual(numCreatedShapes, numLoadedShapes);

        TestManager.Helpers.CloseTestProject();
      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex, true);
      }
    }

    /// <summary>
    /// Test function
    /// </summary>
    [Test]
    public void TestEntityPropertySerialization()
    {
      try
      {
        const string TEST_SCENENAME = "TestEntityPropertySerialization";

        // setup new test project/scene
        TestManager.Helpers.CreateTestScene( TEST_SCENENAME );
        EditorProject project = EditorApp.Project;
        
        // create an entity shape
        EditorManager.GetShapeCreatorPluginByName("Entity").ExecutePlugin();
        ShapeBase entityShape = EditorManager.Scene.ActiveLayer.Root.ChildCollection[0];

        string projectFile = project.PathName;
        int numCreatedShapes = CountShapeInstances( project.Scene );

        // cause engine view to refresh (to see that rendering the shapes doesn't cause a crash)
        TestManager.Helpers.ProcessEvents();

        // close map
        project.Save();
        project.Close();

        // reload map
        project = EditorManager.ProjectFactory.CreateInstance(null) as EditorProject;
        Assert.IsNotNull(project);
        project.Load( projectFile );
        EditorManager.Project = project;
        bool bResult = project.OpenScene( TEST_SCENENAME );
        Assert.IsTrue( bResult );

        // verify that EntityProperty type is part of the active EntityClassManager assembly
        entityShape = EditorManager.Scene.ActiveLayer.Root.ChildCollection[0];
        DynamicPropertyCollection entityProperties = (DynamicPropertyCollection) entityShape.GetType().InvokeMember("EntityProperties", BindingFlags.GetProperty, null, entityShape, new object[]{});
        Assert.IsTrue( EditorManager.EngineManager.EntityClassManager.Exists(entityProperties.CollectionType.UniqueName) );

        TestManager.Helpers.CloseTestProject();

      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex,true);
        throw ex;
      }
    }

    /// <summary>
    /// helper function
    /// </summary>
    /// <param name="scene"></param>
    /// <returns></returns>
    private int CountShapeInstances( IScene scene )
    {
      int count = 0;
      foreach (Layer l in scene.Layers)
        count += l.Root.ChildCollection.Count;
      return count;
    }

    /// <summary>
    /// DEPRECATED since there is no scene locking anymore
    /// </summary>
    [Test]
    public void TestLayerLocking()
    {
      try
      {
        const string TEST_SCENENAME = "TestSceneLocking";
        const string TEST_SCENENAME2 = "TestSceneLocking_SaveAsTest";

        // Create a new project and scene
        TestManager.Helpers.CreateTestScene(TEST_SCENENAME);
        EditorProject project = EditorApp.Project;
        IScene scene = project.Scene;
        Layer firstLayer = scene.Layers[0];

        // First layer must be writeable
        Assert.IsTrue(firstLayer.OwnsLock);
        Assert.IsTrue(firstLayer.SaveToFile());

        // close scene
        string layerFilePath = firstLayer.AbsoluteLayerFilename;
        firstLayer = null;
        scene.Close();
        Assert.IsNull(project.Scene);

        // Get external lock on layer file
        IFileLock extFileLock = EditorManager.FileLockFactory.GetFileLock(layerFilePath);
        Assert.IsTrue(extFileLock.TryLock());

        // Reopen scene: must fail on saving layer (locked externally)
        Assert.IsTrue(project.OpenScene(TEST_SCENENAME));
        scene = project.Scene;
        firstLayer = scene.Layers[0];
        Assert.IsFalse(firstLayer.OwnsLock);

        // Release lock: OwnsWriteLock state must get updated
        extFileLock.Unlock();
        // Give windows time to inform the file watcher
        System.Threading.Thread.Sleep(1000); TestManager.Helpers.ProcessEvents();
        // And then give the file watcher time to send his queued/delayed notification
        System.Threading.Thread.Sleep(1000); TestManager.Helpers.ProcessEvents();
        Assert.IsTrue(firstLayer.LockStatus == Layer.LayerLockStatus_e.NotLocked);

        // Let the scene lock the layer again: saving file must be successful 
        firstLayer.TryLock(null, false);
        Assert.IsTrue(firstLayer.OwnsLock);
        Assert.IsTrue(firstLayer.SaveToFile());

        // Getting external lock must fail (since file is locked by scene)
        Assert.IsFalse(extFileLock.TryLock());

        // Rename the scene file
        Assert.IsTrue(scene.SaveAs(TEST_SCENENAME2));
        string renamedLayerFilePath = firstLayer.AbsoluteLayerFilename;
        Assert.IsTrue(renamedLayerFilePath != layerFilePath);

        // Getting external lock must work after scene-save-as
        Assert.IsTrue(firstLayer.OwnsLock);
        Assert.IsTrue(extFileLock.TryLock());
        extFileLock.Unlock();

        // Getting external lock on renamed file must fail
        IFileLock extFileLockRenamed = EditorManager.FileLockFactory.GetFileLock(renamedLayerFilePath);
        Assert.IsFalse(extFileLockRenamed.TryLock());

        // Verify that a scene can't be opened without closing it at first
        // (important, since we otherwise still have the lock on the old scene)
        Assert.IsFalse(project.OpenScene(TEST_SCENENAME));

        // Close scene
        scene.Close();
        Assert.IsNull(project.Scene);

        // Test correct behaviour for readonly layer files
        {
          // Set the layer as readonly (as done by some version control systems 
          // when using exclusive locking)
          File.SetAttributes(layerFilePath, File.GetAttributes(layerFilePath) | FileAttributes.ReadOnly);

          // Reopen scene
          Assert.IsTrue(project.OpenScene(TEST_SCENENAME));
          scene = project.Scene;
          firstLayer = scene.Layers[0];

          // Layer must not be locked due to being readonly.
          Assert.IsFalse(firstLayer.OwnsLock);
          Assert.AreEqual(Layer.LayerLockStatus_e.ReadOnly, firstLayer.LockStatus);

          // Close scene
          scene.Close();
          Assert.IsNull(project.Scene);

          // Set the layer as readonly (as done by some version control systems 
          // when using exclusive locking)
          File.SetAttributes(layerFilePath, File.GetAttributes(layerFilePath)  & (~FileAttributes.ReadOnly));
        }

        TestManager.Helpers.CloseTestProject();
      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex);
        throw ex;
      }
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
