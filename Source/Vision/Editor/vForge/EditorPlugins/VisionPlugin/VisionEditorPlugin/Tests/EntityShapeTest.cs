/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using CSharpFramework;
using NUnit.Framework;
using System.Reflection;
using VisionEditorPlugin.Shapes;
using CSharpFramework.Actions;
using CSharpFramework.Tests;
using CSharpFramework.Helper;
using System.IO;
using CSharpFramework.Shapes;
using CSharpFramework.Math;

namespace VisionEditorPlugin.Tests
{
	/// <summary>
	/// Test for entity shape
	/// </summary>
	[TestFixture]
	public class EntityShapeTest
	{
    private EntityShape _entityShape;

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
      TestManager.Helpers.CloseTestProject();
    }

    void LoadEntityShapeTestScene()
    {
      TestManager.Helpers.OpenSceneFromFile(Path.Combine(TestManager.Helpers.TestDataDir, @"EntityShapeTest\EntityShapeTest.scene"));

      _entityShape = new EntityShape("TestEntity");
      Layer layer = EditorManager.Scene.ActiveLayer;
      EditorManager.Actions.Add(AddShapeAction.CreateAddShapeAction(_entityShape, null, layer, false));
    }
    
    /// <summary>
    /// Test function
    /// </summary>
    [Test]
		public void testClassNameUndo()
		{
      LoadEntityShapeTestScene();

      // verify that undoing EntityClass modifications does correctly restore the EntityProperties
      object oldEntityProperties = _entityShape.EntityProperties;
      IAction myAction = SetPropertyAction.CreateSetPropertyAction( _entityShape, "EntityClass", "CubeMapHandle_cl");
      
      myAction.Do();
      Assert.IsTrue(!object.ReferenceEquals(_entityShape.EntityProperties, oldEntityProperties));
      object newEntityProperties = _entityShape.EntityProperties;

      myAction.Undo();
      Assert.AreSame( oldEntityProperties, _entityShape.EntityProperties );

      myAction.Do();
      Assert.AreSame( newEntityProperties, _entityShape.EntityProperties );
		}

    [Test]
    public void TestPropertiesFromXML()
    {
      LoadEntityShapeTestScene();

      _entityShape.ClassDefinitionTemplate = "MyEntityTemplate.xml";
      Assert.IsTrue(string.Compare(_entityShape.EntityProperties.CollectionType.UniqueName, _entityShape.ClassDefinitionTemplate)==0, "Unique name must match the loaded XML file");

      string exp = "Duration=1.5;Scriptfile=bla;";
      string act = _entityShape.EntityPropertyString;
      bool bSame = string.Compare(exp, act, true)==0;
      Assert.IsTrue(bSame, "The new parameter string must match the (default) properties from XML.\n\nExpected: {0}\nActual: {1}",
        new object[2] { exp, act });

      // reset
      _entityShape.ClassDefinitionTemplate = null;
      Assert.IsTrue(string.Compare(_entityShape.EntityProperties.CollectionType.UniqueName, "VisBaseEntity_cl") == 0, "Unique name must be base entity again");
    }

    [Test]
    public void TestBoneAttachment()
    {
      // load a different scene for this test
      TestManager.Helpers.OpenSceneFromFile(Path.Combine(TestManager.Helpers.TestDataDir, @"EntityShapeTest\BoneAttachment.scene"));

      //playback a bit:
      EditorManager.EditorMode = EditorManager.Mode.EM_PLAYING_IN_EDITOR;

      ShapeBase shape = EditorManager.Scene.FindShapeByName("ball");
      Assert.IsNotNull(shape);

      Vector3F oldPos = Vector3F.Zero;
      Vector3F newPos = Vector3F.Zero;
      shape._engineInstance.GetPosition(ref oldPos);
      bool bEverChanged = false;

      for (int i = 0; i < 1000; i++)
      {
        EditorManager.ActiveView.UpdateView(true);

        // does one attached object actually move?
        shape._engineInstance.GetPosition(ref newPos);
        if (oldPos != newPos)
          bEverChanged = true;
        oldPos = newPos;
      }

      Assert.IsTrue(bEverChanged);

      // now export and see whether attachment survives the export
      EditorManager.Scene.ExportScene(null, false);

      EditorManager.Scene.Close();
      TestManager.Helpers.LoadExportedScene("BoneAttachment.vscene");
      for (int i = 0; i < 1000; i++)
      {
        EditorManager.EngineManager.WriteText2D(10.0f, 10.0f, "This is the exported version", VisionColors.White);
        EditorManager.ActiveView.UpdateView(true);
      }

      EditorManager.EditorMode = EditorManager.Mode.EM_NONE;
      TestManager.Helpers.CloseExportedScene();
    }


    [Test]
    public void TestRestoreLinks()
    {
      // load a different scene for this test
      TestManager.Helpers.OpenSceneFromFile(Path.Combine(TestManager.Helpers.TestDataDir, @"EntityShapeTest\RestoreLink.scene"));

      EntityShape entity = EditorManager.Scene.FindShapeByName("Entity") as EntityShape;
      Assert.IsNotNull(entity);

      // the map has an anim entity which is attached to a path
      Assert.AreEqual(entity.EntityClass, "AnimEntity_cl");
      Assert.AreEqual(entity.LinkSources.Count, 1);
      Assert.AreEqual(entity.LinkTargets.Count, 0);
      Assert.AreEqual(entity.LinkBidirections.Count, 0);

      // verify the link is there
      LinkSource linkSrc = (LinkSource)entity.LinkSources[0];
      Assert.AreEqual(linkSrc.Links.Count, 1);
      LinkTarget linkTgt = (LinkTarget)linkSrc.Links[0];
      PathShape pathShape = linkTgt.OwnerShape as PathShape;
      Assert.IsNotNull(pathShape);
      Assert.AreEqual(pathShape.LinkTargets.Count, 1);
      Assert.AreEqual(pathShape.LinkTargets[0].Links.Count, 1);

      // setting a new entity class removes the link
      IAction myAction = SetPropertyAction.CreateSetPropertyAction(entity, "EntityClass", "VisBaseEntity_cl");
      myAction.Do();
      Assert.AreEqual(entity.LinkSources.Count, 0);
      Assert.AreEqual(pathShape.LinkTargets.Count, 1);
      Assert.AreEqual(pathShape.LinkTargets[0].Links.Count, 0); // not conected to entity anymore

      // now the link should be restored again
      myAction.Undo();
      Assert.AreEqual(entity.LinkSources.Count, 1);
      Assert.AreEqual(entity.LinkSources[0].Links.Count, 1);
      Assert.AreEqual(entity.LinkSources[0].Links[0].OwnerShape, pathShape);
      Assert.AreEqual(pathShape.LinkTargets.Count, 1);
      Assert.AreEqual(pathShape.LinkTargets[0].Links.Count, 1);
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
