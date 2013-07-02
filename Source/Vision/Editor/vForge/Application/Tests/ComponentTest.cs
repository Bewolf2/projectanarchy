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
using CSharpFramework.Math;
using CSharpFramework.Scene;
using CSharpFramework.Shapes;
using CSharpFramework.DynamicProperties;
using CSharpFramework.Actions;

namespace Editor.Tests
{
  /// <summary>
  /// Component Test class
  /// </summary>
  [TestFixture]
  public class ComponentTest
  {
    /// <summary>
    /// Test function
    /// </summary>
    [Test]
    public void AddComponents()
    {
      TestManager.Helpers.OpenSceneFromFile(TestManager.Helpers.TestDataDir + @"\ComponentTest\ComponentTest.scene");

      // add script-component to static mesh
      ShapeComponent staticMeshScriptComp = CreateScriptComponent(TestManager.Helpers.TestDataDir + @"\ComponentTest\Scripting\StaticMeshScript.lua");
      ShapeBase staticMesh = EditorManager.Scene.FindShapeByName("StaticMesh");
      EditorManager.Actions.Add(new AddShapeComponentAction(staticMesh, staticMeshScriptComp));

      // add script-component to terrain
      ShapeComponent terrainScriptComp = CreateScriptComponent(TestManager.Helpers.TestDataDir + @"\ComponentTest\Scripting\TerrainScript.lua");
      ShapeBase terrain = EditorManager.Scene.FindShapeByName("Terrain0");
      EditorManager.Actions.Add(new AddShapeComponentAction(terrain, terrainScriptComp));
      
      // Start the simulation
      EditorManager.EditorMode = EditorManager.Mode.EM_ANIMATING;
      TestManager.Helpers.ProcessEvents();

      int iFrameCount = 1000;
      for (int i = 0; i < iFrameCount; i++)
      {
        EditorManager.EngineManager.WriteText2D(300.0f, 10.0f, "Added script-component to staticMesh and terrain", VisionColors.White);
        EditorManager.ActiveView.UpdateView(true);
      }

      // Stop the simulation
      EditorManager.EditorMode = EditorManager.Mode.EM_NONE;

      EditorManager.Scene.Save();
      TestManager.Helpers.CloseTestProject();
    }

    /// <summary>
    /// Test function
    /// </summary>
    [Test]
    public void LoadAndExportScene()
    {
      TestManager.Helpers.OpenSceneFromFile(TestManager.Helpers.TestDataDir + @"\ComponentTest\ComponentTest.scene");

      // Start the simulation
      EditorManager.EditorMode = EditorManager.Mode.EM_ANIMATING;
      TestManager.Helpers.ProcessEvents();

      int iFrameCount = 1000;
      for (int i = 0; i < iFrameCount; i++)
      {
        EditorManager.EngineManager.WriteText2D(300.0f, 10.0f, "Reloaded saved scene", VisionColors.White);
        EditorManager.ActiveView.UpdateView(true);
      }

      // check, if script-component is attached to static mesh
      CheckScriptComponent("StaticMesh", TestManager.Helpers.TestDataDir + @"\ComponentTest\Scripting\StaticMeshScript.lua");
      
      // check, if script-component is attached to terrain
      CheckScriptComponent("Terrain0", TestManager.Helpers.TestDataDir + @"\ComponentTest\Scripting\TerrainScript.lua");

      // Stop the simulation
      EditorManager.EditorMode = EditorManager.Mode.EM_NONE;

      // export scene
      EditorManager.Scene.ExportScene(null, false);
    }

    /// <summary>
    /// Test function
    /// </summary>
    [Test]
    public void LoadVScene()
    {
      TestManager.Helpers.LoadExportedScene(TestManager.Helpers.TestDataDir + @"\ComponentTest\ComponentTest.vscene");

      // Start the simulation
      EditorManager.EditorMode = EditorManager.Mode.EM_ANIMATING;
      TestManager.Helpers.ProcessEvents();
      
      int iFrameCount = 1000;
      for (int i = 0; i < iFrameCount; i++)
      {
        EditorManager.EngineManager.WriteText2D(300.0f, 10.0f, "Loaded exported scene", VisionColors.White);
        EditorManager.ActiveView.UpdateView(true);
      }

      // check, if script-component is attached to static mesh
      CheckScriptComponent("StaticMesh", TestManager.Helpers.TestDataDir + @"\ComponentTest\Scripting\StaticMeshScript.lua");

      // check, if script-component is attached to terrain
      CheckScriptComponent("Terrain0", TestManager.Helpers.TestDataDir + @"\ComponentTest\Scripting\TerrainScript.lua");

      // Stop the simulation
      EditorManager.EditorMode = EditorManager.Mode.EM_NONE;

      TestManager.Helpers.CloseTestProject();
    }

    ShapeComponent CreateScriptComponent(string scriptFileName)
    {
      ShapeComponentType t = (ShapeComponentType)EditorManager.EngineManager.ComponentClassManager.GetCollectionType("VScriptComponent");
      if (t == null)
        return null;

      ShapeComponent comp = (ShapeComponent)t.CreateInstance(null);
      comp.SetPropertyValue("ScriptFile", scriptFileName);

      return comp;
    }

    void CheckScriptComponent(string shapeName, string scriptFileName)
    {
      ShapeBase shape = EditorManager.Scene.FindShapeByName(shapeName);
      Assert.IsTrue(shape.ComponentCount > 0);
      ShapeComponent shapeScriptComp = shape.Components.get_Item(0);
      Assert.IsNotNull(shapeScriptComp);
      DynamicProperty shapeProperty = shapeScriptComp.GetPropertyByName("ScriptFile");
      Assert.AreEqual(shapeProperty.Value, scriptFileName);
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
