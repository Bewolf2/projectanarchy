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
using NUnit.Framework;
using CSharpFramework;
using Editor.Actions;
using CSharpFramework.Layout;
using System.Windows.Forms;

namespace Editor.Tests
{
  /// <summary>
  /// A test for the layout functionality
  /// </summary>
  [TestFixture]
  public class LayoutsTest
  {
    /// <summary>
    /// The test
    /// </summary>
    [Test]
    public void LayoutTest()
    {
      // Load a scene
      TestManager.Helpers.OpenSceneFromFile(TestManager.Helpers.TestDataDir + @"\PhysXBinding\JointMuseum.scene");

      // Store the current layout
      Layout OldLayout = EditorManager.ApplicationLayout.ActiveLayout;

      // get layouts
      Layout[] Layouts = EditorManager.ApplicationLayout.AvailableLayouts;

      // first go through all layouts once
      for (int i = 0; i < Layouts.Length; i++)
      {
        EditorManager.ApplicationLayout.ActiveLayout = Layouts[i];
        Application.DoEvents();
      }

      // now do a stress test by going through the layouts randomly some time
      Random R = new Random(42); // same seed everytime so the tests are reproducable
      for (int i = 0; i < 30; i++)
      {
        EditorManager.ApplicationLayout.ActiveLayout = Layouts[R.Next(Layouts.Length)];
        Application.DoEvents();
      }

      // load a deferred scene to make sure the deferred renderer behaves nicely as well
      TestManager.Helpers.OpenSceneFromFile(TestManager.Helpers.TestDataDir + @"\PhysXBinding\JointMuseum_Deferred.scene");

      // first go through all layouts once
      for (int i = 0; i < Layouts.Length; i++)
      {
        EditorManager.ApplicationLayout.ActiveLayout = Layouts[i];
        Application.DoEvents();
      }

      // now do a stress test by going through the layouts randomly some time
      for (int i = 0; i < 30; i++)
      {
        EditorManager.ApplicationLayout.ActiveLayout = Layouts[R.Next(Layouts.Length)];
        Application.DoEvents();
      }

      // cleanup
      TestManager.Helpers.CloseActiveProject();

      if (OldLayout != null)
        EditorManager.ApplicationLayout.ActiveLayout = OldLayout;
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
