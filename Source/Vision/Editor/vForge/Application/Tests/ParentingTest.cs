/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using NUnit.Framework;
using CSharpFramework;
using CSharpFramework.Tests;
using CSharpFramework.Shapes;
using CSharpFramework.Math;
using System.Runtime.Serialization;
using System.IO;
using System.Drawing;

namespace Editor.Tests
{
	/// <summary>
	/// Summary description for ParentingTest.
	/// </summary>
	[TestFixture]
	public class ParentingTest
	{
    /// <summary>
    /// Test function
    /// </summary>
    [Test]
    public void TestParenting()
		{
      TestManager.Helpers.CreateTestScene("TestParenting");

#if HK_ANARCHY
      String[] shapesToTest = new String[] { "Entity", "Lights (Dynamic) - Omni Light", "Lights (Dynamic) - Spot Light", "Particle System", "Sphere", "GroundPlane", "Plane", "XAxis", "YAxis", "ZAxis", "AABox", "Fan", "Cyclone", "Point", "Mirror" };
#else
      String[] shapesToTest = new String[] { "Entity", "Lights (Dynamic) - Omni Light", "Lights (Dynamic) - Spot Light", "Volumetric Cone", "Particle System", "Sphere", "GroundPlane", "Plane", "XAxis", "YAxis", "ZAxis", "AABox", "Fan", "Cyclone", "Point", "Mirror" };
#endif

      // test all possibly parenting combinations
      foreach (string parentShapeName in shapesToTest)
        foreach (string childShapeName in shapesToTest)
          PerformShapeParentingTest(parentShapeName, childShapeName);

      //test ParentingModes.NoParentTransformation using shapes with this property as child and test it with String[] shapesToTest as parents
      NoParentTransformShapeCreator noParentTransformShapeCreator  = new NoParentTransformShapeCreator();
      EditorManager.ShapeCreatorPlugins.Add(noParentTransformShapeCreator); //register NoParentTransformShape creatorplugin
      foreach (string parentShapeName in shapesToTest)
        PerformShapeParentingNoTransformationTest(parentShapeName, "NoParentTransformShape");
      EditorManager.ShapeCreatorPlugins.Remove(noParentTransformShapeCreator);

      TestManager.Helpers.CloseTestProject();
		}


    void PerformScalingTest(Shape3D parent)
    {
      Shape3D child = (Shape3D)parent.ChildCollection[0];
      Vector3F pos = child.Position;

      // modify scaling
      parent.UniformScaling = 3.0f;

      // now children should be affected as well
      Assert.AreEqual(child.UniformScaling, parent.UniformScaling, "Children must be affected by scaling");
      Assert.IsFalse(child.Position == pos, "Position must have changed through scaling");
    }

    [Test]
    public void InheritScalingTest()
    {
      TestManager.Helpers.OpenSceneFromFile(Path.Combine(TestManager.Helpers.TestDataDir, @"MiscScenes\InheritScaling.scene"));

      ShapeBase cam = EditorManager.Scene.FindShapeByName("CameraPosition");
      if (cam != null)
        cam.OnDoubleClick();
      Shape3D mesh = (Shape3D)EditorManager.Scene.FindShapeByName("StaticMeshParent");
      Shape3D entity = (Shape3D)EditorManager.Scene.FindShapeByName("EntityParent");
      PerformScalingTest(mesh);
      PerformScalingTest(entity);
      TestManager.Helpers.CloseTestProject();
    }

    /// <summary>
    /// Test whether attaching the childShape to the parentShape works correctly
    /// </summary>
    /// <param name="parentShapeName">name of shapetype used as parent shape</param>
    /// <param name="childShapeName">name of shapetype used as child shape</param>
    protected void PerformShapeParentingTest(string parentShapeName, string childShapeName)
    {
      // create test shapes
      EditorManager.GetShapeCreatorPluginByName(childShapeName).ExecutePlugin();
      EditorManager.GetShapeCreatorPluginByName(parentShapeName).ExecutePlugin();
      Shape3D parentShape = (Shape3D) EditorManager.Scene.ActiveLayer.Root.ChildCollection[1];
      Shape3D childShape = (Shape3D) EditorManager.Scene.ActiveLayer.Root.ChildCollection[0];

      parentShape.Position = new Vector3F(100, 100, 100);
      childShape.Position = new Vector3F(200, 100, 100);
      childShape.Orientation = new Vector3F(0, 0, 0);
      
      // attach to parent
      childShape.SetParent( parentShape );
      Assert.AreSame( parentShape, childShape.Parent );
      Assert.AreEqual( childShape.Position , new Vector3F(200, 100, 100) );
      Assert.AreEqual( childShape.Orientation, new Vector3F(0, 0, 0) );

      // verify that child moves when parent moves
      parentShape.x += 100;
      Assert.AreEqual( childShape.Position , new Vector3F(300, 100, 100) );

      // test orientation only if supported
      if ((!parentShape.IsHintSet(ShapeBase.HintFlags_e.NoRotation)) && (!childShape.IsHintSet(ShapeBase.HintFlags_e.NoRotation)))
      {
        // verify orientation 
        parentShape.Yaw += 90;
        const float ORIENTATION_EPSILON = 0.0001f;
        Assert.AreEqual( childShape.Orientation.X, 90, ORIENTATION_EPSILON );
        Assert.AreEqual( childShape.Orientation.Y, 0, ORIENTATION_EPSILON );
        Assert.AreEqual( childShape.Orientation.Z, 0, ORIENTATION_EPSILON );
      }

      // perform some rendering in-between to proof it does not crash while rendering
      EditorManager.ActiveView.UpdateView(true);

      // cleanup
      EditorManager.Scene.ActiveLayer.RemoveShape( parentShape );
      Assert.AreEqual( 0, EditorManager.Scene.ActiveLayer.Root.ChildCollection.Count );

      EditorManager.ActiveView.UpdateView(true);
    }


    /// <summary>
    /// Test whether attaching the childShape to the parentShape works correctly. The childshape should not move here when the parent moves
    /// </summary>
    /// <param name="parentShapeName">name of shapetype used as parent shape</param>
    /// <param name="childShapeName">name of shapetype used as child shape</param>
    protected void PerformShapeParentingNoTransformationTest(string parentShapeName, string childShapeName)
    {
      // create test shapes
      EditorManager.GetShapeCreatorPluginByName(childShapeName).ExecutePlugin();
      EditorManager.GetShapeCreatorPluginByName(parentShapeName).ExecutePlugin();
      Shape3D parentShape = (Shape3D)EditorManager.Scene.ActiveLayer.Root.ChildCollection[1];
      Shape3D childShape = (Shape3D)EditorManager.Scene.ActiveLayer.Root.ChildCollection[0];

      parentShape.Position = new Vector3F(100, 100, 100);
      childShape.Position = new Vector3F(200, 100, 100);
      childShape.Orientation = new Vector3F(0, 0, 0);

      // attach to parent
      childShape.SetParent(parentShape);
      Assert.AreSame(parentShape, childShape.Parent);
      Assert.AreEqual(childShape.Position, new Vector3F(200, 100, 100));
      Assert.AreEqual(childShape.Orientation, new Vector3F(0, 0, 0));

      // verify that child does not move when parent moves
      parentShape.x += 100;
      Assert.AreEqual(childShape.Position, new Vector3F(200, 100, 100));

      // test orientation only if supported. Orientation must not change
      if ((!parentShape.IsHintSet(ShapeBase.HintFlags_e.NoRotation)) && (!childShape.IsHintSet(ShapeBase.HintFlags_e.NoRotation)))
      {
        // verify orientation 
        parentShape.Yaw += 90;
        const float ORIENTATION_EPSILON = 0.0001f;
        Assert.AreEqual(childShape.Orientation.X, 0, ORIENTATION_EPSILON);
        Assert.AreEqual(childShape.Orientation.Y, 0, ORIENTATION_EPSILON);
        Assert.AreEqual(childShape.Orientation.Z, 0, ORIENTATION_EPSILON);
      }

      // perform some rendering in-between to proof it does not crash while rendering
      EditorManager.ActiveView.UpdateView(true);

      // cleanup
      EditorManager.Scene.ActiveLayer.RemoveShape(parentShape);
      Assert.AreEqual(0, EditorManager.Scene.ActiveLayer.Root.ChildCollection.Count);

      EditorManager.ActiveView.UpdateView(true);
    }
  }



  #region class NoParentTransformShape
  /// <summary>
  /// HotSpotShape : This is the class that represents the shape in the editor. It has no engine instance that handles the
  /// native code and will not be shown in engine view. It just shows how to add Hotspots to a shape
  /// </summary>
  [Serializable]
  public class NoParentTransformShape : Shape3D
  {
    #region Constructor

    /// <summary>
    /// The constructor of the node shape, just takes the node name
    /// </summary>
    /// <param name="name">Name of the shape in the shape tree</param>
    public NoParentTransformShape(string name)
      : base(name)
    {
    }

    #endregion

    /// <summary>
    /// Overridden function. This function draws a blue frustum. If the shape is selected the frustum switches to red
    /// </summary>
    public override void RenderShape(VisionViewBase view, ShapeRenderMode mode)
    {
      base.RenderShape(view, mode);

      if (this.Selected)
        view.RenderLineFrustum(this.Position, this.RotationMatrix, 45.0f, 45.0f, 2000.0f * EditorManager.Settings.GlobalUnitScaling, VisionColors.Red, 2.0f);
      else
        view.RenderLineFrustum(this.Position, this.RotationMatrix, 45.0f, 45.0f, 2000.0f * EditorManager.Settings.GlobalUnitScaling, VisionColors.Blue, 2.0f);
    }


    /// <summary>
    /// Sets parenting mode to scene graph
    /// </summary>
    protected override Shape3D.ParentingModes ParentingMode
    {
      get
      {
        return ParentingModes.NoParentTransformation;
      }
    }



    #region Serialization and Export

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected NoParentTransformShape(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context)
    {
      base.GetObjectData(info, context);
      // nothing else to do...
    }

    #endregion


  }
  #endregion

  #region NoParentTransformShape Creator Plugin


  /// <summary>
  /// Creator class. An instance of the creator is registered in the plugin init function. Thus the creator shows
  /// up in the "Create" menu of the editor
  /// </summary>
  class NoParentTransformShapeCreator : CSharpFramework.IShapeCreatorPlugin
  {

    /// <summary>
    /// Constructor
    /// </summary>
    public NoParentTransformShapeCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\Node.bmp"), Color.Magenta);
    }

    /// <summary>
    /// Get the name of the plugin, for instance the shape name. This name apears in the "create" menu
    /// </summary>
    /// <returns>creator name</returns>
    public override string GetPluginName()
    {
      return "NoParentTransformShape";
    }

    /// <summary>
    /// Get the plugin category name to sort the plugin name. This is useful to group creators. A null string can
    /// be returned to put the creator in the root
    /// </summary>
    /// <returns></returns>
    public override string GetPluginCategory()
    {
      return "Misc";
    }

    /// <summary>
    /// Returns a short description text
    /// </summary>
    /// <returns></returns>
    public override string GetPluginDescription()
    {
      return "Creates a NoParentTransformShape instance.\n" +
        "This shape is just for testing.";
    }

    public override ShapeBase CreateShapeInstance()
    {
      NoParentTransformShape shape = new NoParentTransformShape("NoParentTransformShape");
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
