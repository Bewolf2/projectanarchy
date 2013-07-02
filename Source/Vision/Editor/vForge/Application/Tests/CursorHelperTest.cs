/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using CSharpFramework;
using NUnit.Core;
using NUnit.Framework;
using System.Windows.Forms;
using CSharpFramework.Math;
using CSharpFramework.Shapes;
using VisionEditorPlugin.Shapes;
using CSharpFramework.Contexts;
using System.Threading;

namespace Editor.Tests
{
  /// <summary>
  /// Testing the Wrapper functionality for the mouse movement/press-states.
  /// </summary>
  [TestFixture]
  public class CursorHelperTest
  {
    [Test]
    public void TestCursor()
    {
      TestManager.Helpers.CreateTestScene("CursorTest.scene");

      ShapeBase staticmesh = new CubemapShape("CubemapShape");
      EditorManager.Scene.Layers[0].AddShape(staticmesh, null);

      TestManager.Helpers.CursorMoveTo(100, 100);
      
      EditorManager.ActiveView.UpdateView(true);
      Thread.Sleep(100);

      TestManager.Helpers.CursorMoveTo3D(new Vector3F(0,0,0));

      TestManager.Helpers.SetButtonState(MouseButtons.Left, true, KeyModifier.Ctrl);
      TestManager.Helpers.SetButtonState(MouseButtons.Left, false, KeyModifier.Ctrl);
      
      EditorManager.ActiveView.UpdateView(true);
      Thread.Sleep(100);

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
