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
using CSharpFramework.Shapes;
using Editor.Actions;
using CSharpFramework.Actions;
using CSharpFramework.Math;

namespace Editor.Tests
{
  /// <summary>
  /// TransformAction Test class.
  /// </summary>
  [TestFixture]
  public class TransformActionTests
  {
    private ShapeCollection _testShapes = new ShapeCollection();

    /// <summary>
    /// Test function
    /// </summary>
    [TestFixtureSetUp]
    public void SetUp()
    {
      TestManager.Helpers.CreateTestScene("TransformActionTests.scene");

      _testShapes.Add( new Shape3D("1") );
      _testShapes.Add( new Shape3D("2") );

      Layer layer = EditorManager.Scene.ActiveLayer;
      EditorManager.Actions.Add(AddShapesAction.CreateAddShapesAction(_testShapes, null,layer,false, "add"));
      
      // setup property changed listener
      ResetPropertyChanged();
      EditorScene.PropertyChanged += new PropertyChangedEventHandler(OnPropertyChanged);
    }

    /// <summary>
    /// Test function
    /// </summary>
    [TestFixtureTearDown]
    public void TearDown()
    {
      EditorScene.PropertyChanged -= new PropertyChangedEventHandler(OnPropertyChanged);

      TestManager.Helpers.CloseTestProject();
    }

    #region Test Functions

    /// <summary>
    /// Test function
    /// </summary>
    [Test]
		public void TestMove()
		{
      Vector3F[] oldPosition = _testShapes.StoreShapePositionArray();
      foreach (Shape3D shape in _testShapes)
        shape.Position = new Vector3F( 100.0f, 0.0f, 0.0f );
 
      Vector3F[] newPosition = _testShapes.StoreShapePositionArray();

      // spawn the action
      IAction moveAction = new MoveShapesAction( _testShapes, oldPosition, newPosition);

      ResetPropertyChanged();
      moveAction.Do();
      Assert.AreEqual( newPosition[0], (_testShapes[0] as Shape3D).Position );
      Assert.AreEqual( newPosition[1], (_testShapes[1] as Shape3D).Position );
      Assert.AreEqual( true, _propertyChanged );

      ResetPropertyChanged();
      moveAction.Undo();
      Assert.AreEqual( oldPosition[0], (_testShapes[0] as Shape3D).Position );
      Assert.AreEqual( oldPosition[1], (_testShapes[1] as Shape3D).Position );
      Assert.AreEqual( true, _propertyChanged );

      ResetPropertyChanged();
      moveAction.Do();
      Assert.AreEqual( newPosition[0], (_testShapes[0] as Shape3D).Position );
      Assert.AreEqual( newPosition[1], (_testShapes[1] as Shape3D).Position );
      Assert.AreEqual( true, _propertyChanged );
		}

    /// <summary>
    /// Test function
    /// </summary>
    [Test]
    public void TestRotate()
    {
      Vector3F[] oldOrientation = _testShapes.StoreShapeOrientationArray();
      Vector3F[] oldPosition = _testShapes.StoreShapePositionArray();
      foreach (Shape3D shape in _testShapes)
        shape.Orientation = new Vector3F( 100.0f, 0.0f, 0.0f );

      Vector3F[] newOrientation = _testShapes.StoreShapeOrientationArray();
      Vector3F[] newPosition = _testShapes.StoreShapePositionArray();
      IAction rotateAction = new RotateShapesAction( _testShapes, oldOrientation, newOrientation, oldPosition, newPosition);

      ResetPropertyChanged();
      rotateAction.Do();
      Assert.AreEqual( newOrientation[0], (_testShapes[0] as Shape3D).Orientation );
      Assert.AreEqual( newOrientation[1], (_testShapes[1] as Shape3D).Orientation );
      Assert.AreEqual( true, _propertyChanged );

      ResetPropertyChanged();
      rotateAction.Undo();
      Assert.AreEqual( oldOrientation[0], (_testShapes[0] as Shape3D).Orientation );
      Assert.AreEqual( oldOrientation[1], (_testShapes[1] as Shape3D).Orientation );
      Assert.AreEqual( true, _propertyChanged );

      ResetPropertyChanged();
      rotateAction.Do();
      Assert.AreEqual( newOrientation[0], (_testShapes[0] as Shape3D).Orientation );
      Assert.AreEqual( newOrientation[1], (_testShapes[1] as Shape3D).Orientation );
      Assert.AreEqual( true, _propertyChanged );
    }

    /// <summary>
    /// Test function
    /// </summary>
    [Test]
    public void TestScale()
    {
      Vector3F[] oldScale = _testShapes.StoreShapeScalingArray();
      foreach (Shape3D shape in _testShapes)
        shape.Scaling = new Vector3F( 3.0f, 1.0f, 1.0f );
      Vector3F[] newScale = _testShapes.StoreShapeScalingArray();
      IAction scaleAction = new ScaleShapesAction( _testShapes, oldScale, newScale );

      ResetPropertyChanged();
      scaleAction.Do();
      Assert.AreEqual( newScale[0], (_testShapes[0] as Shape3D).Scaling );
      Assert.AreEqual( newScale[1], (_testShapes[1] as Shape3D).Scaling );
      Assert.AreEqual( true, _propertyChanged );

      ResetPropertyChanged();
      scaleAction.Undo();
      Assert.AreEqual( oldScale[0], (_testShapes[0] as Shape3D).Scaling );
      Assert.AreEqual( oldScale[1], (_testShapes[1] as Shape3D).Scaling );
      Assert.AreEqual( true, _propertyChanged );

      ResetPropertyChanged();
      scaleAction.Do();
      Assert.AreEqual( newScale[0], (_testShapes[0] as Shape3D).Scaling );
      Assert.AreEqual( newScale[1], (_testShapes[1] as Shape3D).Scaling );
      Assert.AreEqual( true, _propertyChanged );
    }

    /// <summary>
    /// Tests whether the SetPositionOrientation function works correctly
    /// </summary>
    public void TestSetPositionOrientation()
    {
      Vector3F[] oldOrientation = _testShapes.StoreShapeOrientationArray();
      Vector3F[] oldPosition = _testShapes.StoreShapePositionArray();

      Vector3F newPos = new Vector3F(212.0f, 3.0f, 0.0f);
      Vector3F newOri = new Vector3F(37.0f, 20.0f, 0.0f);

      // Transform shape
      (_testShapes[0] as Shape3D).SetPositionOrientation(newPos, newOri);

      Assert.AreEqual(newPos, (_testShapes[0] as Shape3D).Position);
      Assert.AreEqual(newOri, (_testShapes[0] as Shape3D).Orientation);
    }

    #endregion

    #region PropertyChanged Event Listener

    private bool _propertyChanged = false;

    private void ResetPropertyChanged()
    {
      _propertyChanged = false;
    }

    private void OnPropertyChanged(object sender, PropertyChangedArgs e)
    {
      _propertyChanged = true;
    }

    #endregion
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
