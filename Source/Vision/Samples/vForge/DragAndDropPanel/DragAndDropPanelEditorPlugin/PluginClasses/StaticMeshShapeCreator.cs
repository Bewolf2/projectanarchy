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
using CSharpFramework;
using CSharpFramework.Shapes;
using VisionEditorPlugin.Shapes;

namespace DragAndDropPanelPlugin.Classes
{
  /// <summary>
  /// Shape creator plugin implementation to create a static mesh via drag-and-drop
  /// </summary>
  public class StaticMeshShapeCreator : IShapeCreatorPlugin
  {
    private string _MeshPath = "";

    public StaticMeshShapeCreator(string ModelPath)
    {
      _MeshPath = ModelPath;
    }

    public override ShapeBase CreateShapeInstance()
    {
      string ShapeName = System.IO.Path.GetFileNameWithoutExtension(_MeshPath);
      if (ShapeName == string.Empty)
        ShapeName = "Model";

      StaticMeshShape SMS = new StaticMeshShape(ShapeName);
      SMS.MeshFileName = _MeshPath;

      return SMS;
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
