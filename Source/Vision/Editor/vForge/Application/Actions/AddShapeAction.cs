/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/*
using System;
using CSharpFramework;
using CSharpFramework.Shapes;

namespace Editor.Actions
{
	/// <summary>
	/// Action that adds a new shape
	/// </summary>
	public class AddShapeAction : IAction
	{
		public AddShapeAction(ShapeBase shape, ShapeBase parent, Layer layer, bool bCreateUniqueName)
		{
      _shape = shape;
      _parent = parent;
      _layer = layer;
      bUniqueName = bCreateUniqueName;
    }

    #region IAction Members

    public override void Undo()
    {
      _layer.RemoveShape(_shape);
    }

    public override void Do()
    {
      if (bUniqueName) // make sure this one does not spawn a rename action
        _shape.ShapeName = EditorApp.Scene.CreateNewShapeName(_shape.ShapeName,true,null);

      _layer.AddShape(_shape,_parent);
      _shape.SetAnimate(EditorApp.Scene.Animated);
    }

    public override string ShortName {get {return "add shape";}}

    #endregion

    ShapeBase _shape,_parent;
    Layer _layer;
    bool bUniqueName = false;
  }


  /// <summary>
  /// Action that removes a shape
  /// </summary>
  public class RemoveShapeAction : IAction
  {
    public RemoveShapeAction(ShapeBase shape)
    {
      _shape = shape;
      _parent = shape.parentShape;
      _layer = shape.ParentLayer;
    }

    #region IAction Members

    public override void Undo()
    {
      _layer.AddShape(_shape,_parent);
    }

    public override void Do()
    {
      _layer.RemoveShape(_shape);
    }

    public override string ShortName {get {return "remove shape";}}

    #endregion

    ShapeBase _shape,_parent;
    Layer _layer;
  }

}

*/

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
