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
using NUnit.Core;
using NUnit.Framework;
using System.Threading;
using CSharpFramework.Shapes;
using CSharpFramework.Math;
using HavokEditorPlugin.Shapes;
using HavokManaged;

namespace HavokEditorPlugin.Tests
{
  [TestFixture]
  public class HavokConstraintShapeTest
  {
    /// <summary>
    /// Test function
    /// </summary>
    [SetUp]
    public void SetUp()
    {
      // create local copy of the directory.
      TestManager.Helpers.CreateLocalCopy("HavokBinding");

      TestManager.Helpers.OpenSceneFromFile(TestManager.Helpers.TestDataDir + @"\HavokBinding\JointMuseum.scene");
      TestManager.Helpers.ProcessEvents();
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
    /// Test whether all the constraints are performing as they should.
    /// </summary>
    [Test]
    public void TestConstraintsDoingWhatTheyShould()
    {
      // Start the simulation
      EditorManager.EditorMode = EditorManager.Mode.EM_PLAYING_IN_EDITOR;
      TestManager.Helpers.ProcessEvents();

      // Let the simulation run for 5 seconds       
      DateTime timeBefore = DateTime.Now;
      while (true)
      {
        TimeSpan passedTime = DateTime.Now - timeBefore;
        if (passedTime.TotalSeconds > 10)
          break;

        TestManager.Helpers.ProcessEvents();
      }

      try
      {
        { // Check the Ball and Socket constraint
          ShapeBase basLower = EditorManager.Scene.FindShapeByName("Ball and Socket - Lower barrel");
          Assert.IsNotNull(basLower, "Failed to find entity");
          Vector3F lowerPos = new Vector3F();
          Assert.IsTrue(basLower._engineInstance.GetPosition(ref lowerPos), "Failed to get position of entity");
          Assert.IsTrue(
            ((lowerPos.Z < 350) && (lowerPos.Z > 200)),
            "Z-Position should be between 200 and 350, but was {0}",
            new object[] { lowerPos.Z });
        }

        { // Limited hinge constraint
          ShapeBase basLowerMoving = EditorManager.Scene.FindShapeByName("Limited Hinge Barrel Moving");
          ShapeBase basLowerFixed = EditorManager.Scene.FindShapeByName("Limited Hinge Barrel Fixed");

          Assert.IsNotNull(basLowerMoving, "Failed to find entity");
          Assert.IsNotNull(basLowerFixed, "Failed to find entity");

          Vector3F lowerPosMoving = new Vector3F();
          Vector3F lowerPosFixed = new Vector3F();

          Assert.IsTrue(basLowerMoving._engineInstance.GetPosition(ref lowerPosMoving), "Failed to get position of entity");
          Assert.IsTrue(basLowerFixed._engineInstance.GetPosition(ref lowerPosFixed), "Failed to get position of entity");
          Assert.IsTrue(
            ((lowerPosMoving.Z > lowerPosFixed.Z) && (lowerPosMoving.Z < 310)),
            "Z-Position should be lower than the initial one but higher than the fixed barrel.");
        }

        { // Hinge constraint
          ShapeBase basLowerMoving = EditorManager.Scene.FindShapeByName("Unlimited Hinge Barrel Moving");
          ShapeBase basLowerFixed = EditorManager.Scene.FindShapeByName("Unlimited Hinge Barrel Fixed");

          Assert.IsNotNull(basLowerMoving, "Failed to find entity");
          Assert.IsNotNull(basLowerFixed, "Failed to find entity");

          Vector3F lowerPosMoving = new Vector3F();
          Vector3F lowerPosFixed = new Vector3F();

          Assert.IsTrue(basLowerMoving._engineInstance.GetPosition(ref lowerPosMoving), "Failed to get position of entity");
          Assert.IsTrue(basLowerFixed._engineInstance.GetPosition(ref lowerPosFixed), "Failed to get position of entity");
          Assert.IsTrue(
            ((lowerPosMoving.Z < lowerPosFixed.Z) && (lowerPosMoving.Z > 150)),
            "Z-Position should be lower than the initial and the fixed barrel but over 150 units away from the floor.");
        }

        { // Check the Stiff Spring constraint
          ShapeBase basUpper = EditorManager.Scene.FindShapeByName("Stiff Spring - Lower barrel");
            Assert.IsNotNull(basUpper, "Failed to find entity");
            Vector3F upperPos = new Vector3F();
            Assert.IsTrue(basUpper._engineInstance.GetPosition(ref upperPos), "Failed to get position of entity");
            Assert.IsTrue(
              ((upperPos.Z < 350) && (upperPos.Z > 50)),
              "Z-Position should be between 50 and 350, but was {0}",
              new object[] { upperPos.Z });
        }

        { // Check the Fixed constraint
          ShapeBase fixedBox = EditorManager.Scene.FindShapeByName("Fixed - Box");
          Assert.IsNotNull(fixedBox, "Failed to find entity");
          Vector3F boxPos = new Vector3F();
          Assert.IsTrue(fixedBox._engineInstance.GetPosition(ref boxPos), "Failed to get position of entity");
          Assert.IsTrue(
            ((boxPos.Z < 300) && (boxPos.Z > 200)),
            "Z-Position should be between 200 and 300, but was {0}",
            new object[] { boxPos.Z });
        }

        { // Check the Prismatic constraint
          ShapeBase prisUpper = EditorManager.Scene.FindShapeByName("Prismatic - Upper barrel");
          Assert.IsNotNull(prisUpper, "Failed to find entity");
          Vector3F upperPos = new Vector3F();
          Assert.IsTrue(prisUpper._engineInstance.GetPosition(ref upperPos), "Failed to get position of entity");
          Assert.IsTrue(
            ((upperPos.Z < 250) && (upperPos.Z > 150)),
            "Z-Position should be between 150 and 250, but was {0}",
            new object[] { upperPos.Z });
        }

        { // Check the Cylindrical constraint
          ShapeBase cylUpper = EditorManager.Scene.FindShapeByName("Cylindrical - Upper barrel");
          Assert.IsNotNull(cylUpper, "Failed to find entity");
          Vector3F upperPos = new Vector3F();
          Assert.IsTrue(cylUpper._engineInstance.GetPosition(ref upperPos), "Failed to get position of entity");
          Assert.IsTrue(
            ((upperPos.Z < 250) && (upperPos.Z > 150)),
            "Z-Position should be between 150 and 250, but was {0}",
            new object[] { upperPos.Z });
        }

        { // Check the PointToPlane constraint
          ShapeBase pointToPlaneBarrel = EditorManager.Scene.FindShapeByName("PointToPlane - Barrel");
          Assert.IsNotNull(pointToPlaneBarrel, "Failed to find entity");
          Vector3F barrelPos = new Vector3F();
          Assert.IsTrue(pointToPlaneBarrel._engineInstance.GetPosition(ref barrelPos), "Failed to get position of entity");
          Assert.IsTrue(
            ((barrelPos.Z < 300) && (barrelPos.Z > 200)),
            "Z-Position should be between 300 and 200, but was {0}",
            new object[] { barrelPos.Z });
        }

        { // Check the PointToPath constraint
          ShapeBase pointToPathLower = EditorManager.Scene.FindShapeByName("PointToPath - Lower ball");
          Assert.IsNotNull(pointToPathLower, "Failed to find entity");
          Vector3F lowerPos = new Vector3F();
          Assert.IsTrue(pointToPathLower._engineInstance.GetPosition(ref lowerPos), "Failed to get position of entity");
          Assert.IsTrue(
            ((lowerPos.Z < 300) && (lowerPos.Z > 200)),
            "Z-Position should be between 300 and 200, but was {0}",
            new object[] { lowerPos.Z });
        }

        { // Check the Pulley constraint
          ShapeBase pulleyBox = EditorManager.Scene.FindShapeByName("Pulley - Box");
          Assert.IsNotNull(pulleyBox, "Failed to find entity");
          Vector3F boxPos = new Vector3F();
          Assert.IsTrue(pulleyBox._engineInstance.GetPosition(ref boxPos), "Failed to get position of entity");
          Assert.IsTrue(
            ((boxPos.Z < 250) && (boxPos.Z > 50)),
            "Z-Position should be between 250 and 50, but was {0}",
            new object[] { boxPos.Z });
        }

        { // Check the Ragdoll constraint
          ShapeBase ragdollUpper = EditorManager.Scene.FindShapeByName("Ragdoll - Upper ball");
          Assert.IsNotNull(ragdollUpper, "Failed to find entity");
          Vector3F upperPos = new Vector3F();
          Assert.IsTrue(ragdollUpper._engineInstance.GetPosition(ref upperPos), "Failed to get position of entity");
          Assert.IsTrue(
            ((upperPos.Z < 300) && (upperPos.Z > 200)),
            "Z-Position should be between 300 and 200, but was {0}",
            new object[] { upperPos.Z });
        }
      }
      finally
      {
        // Stop simulation
        EditorManager.EditorMode = EditorManager.Mode.EM_NONE;
      }
    }

    /// <summary>
    /// Test whether the constraints are performing as they should when they are
    /// made breakable.
    /// </summary>
    [Test]
    public void TestConstraintsBreakable()
    {
      // Make the ball and socket constraint breakable
      ShapeBase constraintBase = EditorManager.Scene.FindShapeByName("Ball and Socket");
      Assert.IsTrue(constraintBase is HavokConstraintShape);
      HavokConstraintShape constraint = (HavokConstraintShape)constraintBase;
      constraint.TypeProperties.BreakableSettings.IsBreakable = true;
      constraint.TypeProperties.BreakableSettings.Threshold = 10;

      // Start the simulation
      EditorManager.EditorMode = EditorManager.Mode.EM_PLAYING_IN_EDITOR;
      TestManager.Helpers.ProcessEvents();

      // Let the simulation run for 5 seconds       
      DateTime timeBefore = DateTime.Now;
      while (true)
      {
        TimeSpan passedTime = DateTime.Now - timeBefore;
        if (passedTime.TotalSeconds > 10)
          break;

        TestManager.Helpers.ProcessEvents();
      }

      try
      {
        // Check the Ball and Socket constraint
        ShapeBase basLower = EditorManager.Scene.FindShapeByName("Ball and Socket - Lower barrel");
        Assert.IsNotNull(basLower, "Failed to find entity");
        Vector3F lowerPos = new Vector3F();
        Assert.IsTrue(basLower._engineInstance.GetPosition(ref lowerPos), "Failed to get position of entity");
        Assert.IsTrue(
          (lowerPos.Z < 150),
          "Z-Position should be less than 150, but was {0}",
          new object[] { lowerPos.Z });
      }
      finally
      {
        // Stop simulation
        EditorManager.EditorMode = EditorManager.Mode.EM_NONE;
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
