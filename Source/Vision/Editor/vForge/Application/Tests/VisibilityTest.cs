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

namespace Editor.Tests
{
  [TestFixture]
  public class VisibilityTest
  {
    [SetUp]
    public void SetUp()
    {
      EditorManager.EngineManager.OnBeforeRenderScene += new IEngineManager.RenderSceneEventHandler(EngineManager_OnBeforeRenderScene);
    }

    /// <summary>
    /// Clears the test data
    /// </summary>
    [TearDown]
    public void TearDown()
    {
      EditorManager.EngineManager.OnBeforeRenderScene -= new IEngineManager.RenderSceneEventHandler(EngineManager_OnBeforeRenderScene);
    }

    string TextToWrite = null;

    void EngineManager_OnBeforeRenderScene(object sender, EventArgs e)
    {
      if (string.IsNullOrEmpty(TextToWrite))
        return;
      EditorManager.EngineManager.WriteText2D(10.0f, 20.0f, TextToWrite, VisionColors.White);
    }

    /// <summary>
    /// Simple Scene that contains visibility info (from shapes)
    /// </summary>
    [Test]
    public void SceneWithVisibilityShapes()
    {
      TestManager.Helpers.OpenSceneFromFile(TestManager.Helpers.TestDataDir + @"\VisibilityTests\VisibilityShapes.scene");
      TestManager.Helpers.ProcessEvents();

      // Start the simulation
      EditorManager.EditorMode = EditorManager.Mode.EM_PLAYING_IN_GAME; // path camera replay
      TestManager.Helpers.ProcessEvents();

      // Let the simulation run for 25 seconds       
      DateTime timeBefore = DateTime.Now;
      while (true)
      {
        TimeSpan passedTime = DateTime.Now - timeBefore;
        if (passedTime.TotalSeconds > 25)
          break;
        if (passedTime.TotalSeconds < 10) // 10s is the cycle of a camera fly-through
        {
          TextToWrite = "No visibility used - all objects visible";
        }
        else
        {
          TextToWrite = "Visibility On - show only objects through portals";
          EditorManager.VisibilityBuilder.UseInEngine = true;
          EditorManager.EngineManager.DebugRenderFlags = ManagedBase.DebugRenderFlags_e.Portals; // | ManagedBase.DebugRenderFlags_e.ObjectVisibilityAssignment;
        }

        TestManager.Helpers.ProcessEvents();
      }

      // Stop simulation, set back
      EditorManager.EditorMode = EditorManager.Mode.EM_NONE;
      EditorManager.EngineManager.DebugRenderFlags = (ManagedBase.DebugRenderFlags_e)0;
      EditorManager.VisibilityBuilder.UseInEngine = false;

      TextToWrite = null;
      TestManager.Helpers.CloseActiveProject();

    }

    /// <summary>
    /// Simple Scene that contains visibility info (from shapes)
    /// </summary>
    [Test]
    public void SceneWithStaticMeshLODandVisObj()
    {
      TestManager.Helpers.OpenSceneFromFile(TestManager.Helpers.TestDataDir + @"\VisibilityTests\VisibilityObjLinking.scene");
      TestManager.Helpers.ProcessEvents();

      // Start the simulation
      EditorManager.EditorMode = EditorManager.Mode.EM_ANIMATING; // update view
      TestManager.Helpers.ProcessEvents();

      // Let the simulation run for some seconds
      DateTime timeBefore = DateTime.Now;
      EditorManager.ActiveView.SetCameraRotation(new Vector3F(90, 0, 0));

      while (true)
      {
        TimeSpan passedTime = DateTime.Now - timeBefore;
        if (passedTime.TotalSeconds > 5)
          break;
        Vector3F pos = new Vector3F(50.0f,(float)passedTime.TotalSeconds * -150.0f-150.0f,  120.0f);
        EditorManager.ActiveView.SetCameraPosition(pos);
        TestManager.Helpers.ProcessEvents();
      }

      // Stop simulation, set back
      EditorManager.EditorMode = EditorManager.Mode.EM_NONE;

      TextToWrite = null;
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
