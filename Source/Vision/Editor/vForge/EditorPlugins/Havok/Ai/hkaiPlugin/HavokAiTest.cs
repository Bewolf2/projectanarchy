/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using CSharpFramework;
using NUnit.Core;
using NUnit.Framework;
using System.Threading;
using CSharpFramework.Shapes;
using CSharpFramework.Math;
using HavokAiEditorPlugin.Shapes;
using HavokAiManaged;

namespace HavokAiEditorPlugin.Tests
{
  [TestFixture]
  public class HavokAiTest
  {
    /// <summary>
    /// Test function
    /// </summary>
    [SetUp]
    public void SetUp()
    {
      TestManager.Helpers.OpenSceneFromFile(Path.Combine(TestManager.Helpers.TestDataDir, @"HavokAiBinding\JointMuseum.scene"));
    }

    /// <summary>
    /// Clears the test data
    /// </summary>
    [TearDown]
    public void TearDown()
    {
      TestManager.Helpers.CloseActiveProject();
    }

    /// <summary>
    /// Test whether nav mesh generation is working as it should.
    /// </summary>
    [Test]
    public void TestNavMeshGeneration()
    {
      // Create a nav mesh shape
      EditorManager.GetShapeCreatorPluginByName("Nav Mesh").ExecutePlugin();
      HavokNavMeshShape navMeshShape = (HavokAiEditorPlugin.Shapes.HavokNavMeshShape)EditorManager.Scene.FindFirstShapeOfType(typeof(HavokNavMeshShape), true);
      Assert.IsNotNull(navMeshShape);

      // Build the nav mesh
      ShapeCollection staticGeometries = new ShapeCollection();
      int numGeometryVertices = 0, numGeometryTriangles = 0, numEntities = 0, numStaticMeshes = 0, numTerrains = 0;
      int numCarvers = 0, numSeedPoints = 0, numLocalSettings = 0;
      HavokAiPanelDialogs.HavokAiPanel.gatherGeometricShapes(ref staticGeometries, ref numEntities, ref numStaticMeshes, ref numTerrains, ref numCarvers, ref numSeedPoints, ref numLocalSettings);
      bool built = navMeshShape.Build(staticGeometries, ref numGeometryVertices, ref numGeometryTriangles);
      Assert.IsTrue(built);
      
      // Add Navmesh to world
      navMeshShape.AddNavMeshToWorld();

      // Create a path shape
      EditorManager.GetShapeCreatorPluginByName("Nav Mesh Test Path").ExecutePlugin();
      HavokNavMeshTestPathShape pathShape = (HavokAiEditorPlugin.Shapes.HavokNavMeshTestPathShape)EditorManager.Scene.FindFirstShapeOfType(typeof(HavokNavMeshTestPathShape), true);
      Assert.IsNotNull(pathShape);

      // Verify that a known path exists on the nav mesh
      pathShape.StartPoint = new Vector3F(-500.0f, 0.0f, 0.0f);
      pathShape.EndPoint = new Vector3F(500.0f, 0.0f, 0.0f);
      pathShape.CharacterRadius = 20.0f;
      bool foundPath = HavokAiManaged.ManagedModule.ComputeAndDrawPath(EditorManager.ActiveView, pathShape.StartPoint, pathShape.EndPoint, pathShape.CharacterRadius, pathShape.CharacterHeight, 0.08f*pathShape.CharacterHeight, VisionColors.Get(pathShape.PathColor));
      EditorManager.ActiveView.UpdateView(true);
      Assert.IsTrue(foundPath);

      // Create a carver shape
      EditorManager.GetShapeCreatorPluginByName("Nav Mesh Carver").ExecutePlugin();
      HavokNavMeshCarverShape carverShape = (HavokAiEditorPlugin.Shapes.HavokNavMeshCarverShape)EditorManager.Scene.FindFirstShapeOfType(typeof(HavokNavMeshCarverShape), true);
      Assert.IsNotNull(carverShape);

      // Make the carver block the previous path and test by rebuilding the nav mesh and verifying the path is blocked
      carverShape.BoxSizeX = 300.0f;
      carverShape.BoxSizeY = 2000.0f;
      carverShape.BoxSizeZ = 200.0f;
      built = navMeshShape.Build(staticGeometries, ref numGeometryVertices, ref numGeometryTriangles);
      Assert.IsTrue(built);
      
      // Add Navmesh to world
      navMeshShape.AddNavMeshToWorld();
      
      foundPath = HavokAiManaged.ManagedModule.ComputeAndDrawPath(EditorManager.ActiveView, pathShape.StartPoint, pathShape.EndPoint, pathShape.CharacterRadius, pathShape.CharacterHeight, 0.08f * pathShape.CharacterHeight, VisionColors.Get(pathShape.PathColor));
      EditorManager.ActiveView.UpdateView(true);
      Assert.IsFalse(foundPath);

      // Resize the carver again, this time so that only character's of a certain radius can traverse the path
      carverShape.BoxSizeY = 1400.0f;
      built = navMeshShape.Build(staticGeometries, ref numGeometryVertices, ref numGeometryTriangles);
      Assert.IsTrue(built);

      // Add Navmesh to world
      navMeshShape.AddNavMeshToWorld();
      
      foundPath = HavokAiManaged.ManagedModule.ComputeAndDrawPath(EditorManager.ActiveView, pathShape.StartPoint, pathShape.EndPoint, pathShape.CharacterRadius, pathShape.CharacterHeight, 0.08f * pathShape.CharacterHeight, VisionColors.Get(pathShape.PathColor));
      EditorManager.ActiveView.UpdateView(true);
      Assert.IsFalse(foundPath);
      pathShape.CharacterRadius = 10.0f;
      foundPath = HavokAiManaged.ManagedModule.ComputeAndDrawPath(EditorManager.ActiveView, pathShape.StartPoint, pathShape.EndPoint, pathShape.CharacterRadius, pathShape.CharacterHeight, 0.08f*pathShape.CharacterHeight, VisionColors.Get(pathShape.PathColor));
      EditorManager.ActiveView.UpdateView(true);
      Assert.IsTrue(foundPath);

      // Save NavMesh to file
      bool saved = navMeshShape.SaveNavMeshesToFile();
      Assert.IsTrue(saved);
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
