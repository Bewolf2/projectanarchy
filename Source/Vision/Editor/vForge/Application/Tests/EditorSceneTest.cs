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
using Editor.Actions;

namespace Editor.Tests
{
	/// <summary>
	/// Summary description for EditorSceneTest.
	/// </summary>

  [TestFixture]
  public class EditorSceneTest
	{
    /// <summary>
    /// Constructor
    /// </summary>
		public EditorSceneTest()
		{
		}

    /// <summary>
    /// Test function
    /// </summary>
    [Test]
    public void LayerTest()
    {
      TestManager.Helpers.CreateTestScene("LayerTest.scene");
      IScene scene = EditorManager.Scene;

      Assert.IsTrue(scene.Save());
      Assert.AreEqual(1,scene.Layers.Count);

      // default layer should be active
      Assert.IsNotNull(scene.ActiveLayer);
      Assert.AreEqual(1,scene.Layers.Count);

      Layer layer1 = new Layer("added1");
      scene.AddLayer(layer1,false);
      Assert.AreEqual(2,scene.Layers.Count);

      Layer layer2 = new Layer("added2");
      scene.AddLayer(layer2,true);
      Assert.AreEqual(3,scene.Layers.Count);
      Assert.AreEqual(layer2,scene.ActiveLayer);

      Assert.IsTrue(scene.RemoveLayer(layer2));
      Assert.IsNotNull(scene.ActiveLayer);
      Assert.IsFalse(layer2==scene.ActiveLayer);

      TestManager.Helpers.CloseTestProject();
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
