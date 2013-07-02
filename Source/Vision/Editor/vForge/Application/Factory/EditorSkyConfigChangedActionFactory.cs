/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System.Diagnostics;
using CSharpFramework;
using CSharpFramework.Helper;
using CSharpFramework.Scene;
using Editor.Actions;

namespace Editor.Factory
{
  class EditorSkyConfigChangedActionFactory : IClassFactory<IAction>
  {
    public IAction CreateInstance(object[] parameters)
    {
      int parameterLength = parameters.GetLength(0);
      Debug.Assert(parameterLength == 1, "Wrong parameter count.", "There should be 1 parameter.");

      V3DLayer mainLayer = null;
      foreach (Layer layer in EditorManager.Scene.Layers)
      {
        mainLayer = layer as V3DLayer;
        if (mainLayer != null)
          break;
      }
      return new SkyConfigChangedAction(mainLayer, (SkyConfig)parameters.GetValue(0));
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
