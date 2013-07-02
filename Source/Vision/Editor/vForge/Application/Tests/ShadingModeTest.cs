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
using CSharpFramework.Shapes;
using System.IO;
using ManagedFramework;
using System.Collections.Specialized;

namespace Editor.Tests
{
  [TestFixture]
  public class ShadingModeTest
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
      int iFrameCount = 1000;

      VisionEngineManager em = (VisionEngineManager)EditorManager.EngineManager;
      StringCollection names = new StringCollection();
      em.GetReplacementRenderLoopEffects(names);

      // render frames
      for (int iMode = 0; iMode < names.Count; iMode++)
      {
        em.SetReplacementRenderLoopEffect(iMode);
        for (int i = 0; i < iFrameCount; i++)
        {
          EditorManager.EngineManager.WriteText2D(10.0f, 10.0f, "Shading mode is now: " + names[iMode], VisionColors.White);
          EditorManager.ActiveView.UpdateView(true);
        }
      }
      em.SetReplacementRenderLoopEffect(-1);
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
