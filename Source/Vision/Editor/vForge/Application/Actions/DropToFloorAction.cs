/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using CSharpFramework;
using CSharpFramework.Shapes;
using CSharpFramework.Actions;
using CSharpFramework.Math;
using System.IO;
using System.Runtime.Serialization;

namespace Editor.Actions
{
  #region DropToFloorAction

  /// <summary>
	/// Action to drop a collection of shapes to floor.
	/// </summary>
  [Serializable]
  public class DropToFloorAction : GroupAction
  {

    #region Constructor

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="shapes"></param>
    /// <param name="mode"></param>
    /// <param name="includeShapes"></param>
    public DropToFloorAction(ShapeCollection shapes, Shape3D.DropToFloorMode mode, Vector3F axis, bool includeShapes)
      : base("Drop to Floor")
    {
      _shapes = shapes;
      _mode = mode;
      _axis = axis;
      _includeShapes = includeShapes;
    }

    /// <summary>
    /// Alternative constructor that takes a single shape
    /// </summary>
    /// <param name="shape"></param>
    /// <param name="mode"></param>
    /// <param name="axis"></param>
    /// <param name="includeShapes"></param>
    public DropToFloorAction(Shape3D shape, Shape3D.DropToFloorMode mode, Vector3F axis, bool includeShapes)
      : base("Drop to Floor")
    {
      _shapes = new ShapeCollection();
      _shapes.Add(shape);
      _mode = mode;
      _axis = axis;
      _includeShapes = includeShapes;
    }

    #endregion

    #region Helper

    void CommonInit(ShapeCollection shapes, Shape3D.DropToFloorMode mode, Vector3F axis, bool includeShapes)
		{
      for (int i=0;i<shapes.Count;i++)
      {
        Shape3D shape = shapes[i] as Shape3D;
        if (shape==null || !shape.Traceable || !shape.Modifiable) continue;

        
        ShapeCollection colliderShapes = null;
        if (includeShapes)
        {
          colliderShapes = EditorManager.Scene.AllShapesOfType(typeof(Shape3D), true);
          if (colliderShapes.Contains(shape))
            colliderShapes.Remove(shape);
        }

        // use a replacement action? (e.g. billboards rather want to drop every single instance)
        IAction custom = shape.GetCustomDropToFloorAction(mode, axis, colliderShapes);
        if (custom != null)
        {
          this.Add(custom);
          continue;
        }

        float fHeight = EditorApp.ActiveView.EngineManager.GetDropToFloorHeight(shape,mode,axis,colliderShapes);
        if (fHeight==0.0f) continue;
        this.Add(new MoveShapeAction(shape,shape.Position, shape.Position + axis * fHeight));
      }
    }

    #endregion

    #region ISerializable Members

    protected DropToFloorAction(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
    }

    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);    
    }

    #endregion

    #region IAction Members

    /// <summary>
    /// Sets the new position using the helper function
    /// </summary>
    public override void Do()
    {
      CommonInit(_shapes, _mode, _axis, _includeShapes);
      if (base.Valid)
        base.Do();
    }

    /// <summary>
    /// Returns true if this action has any shapes
    /// </summary>
    public override bool Valid
    {
      get
      {
        if (_shapes != null && _shapes.Count > 0)
          return true;
        return false;
      }
    }

    #endregion

    #region Icon Index

    public static int iIconIndex = EditorManager.GUI.ActionImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Actions\arrow_down_blue.bmp"), System.Drawing.Color.Magenta);

    /// <summary>
    /// Get the icon index for this action
    /// </summary>
    public override int IconIndex { get { return iIconIndex; } }

    #endregion

    #region Private Data

    private ShapeCollection _shapes;
    private Shape3D.DropToFloorMode _mode;
    private Vector3F _axis;
    private bool _includeShapes;

    #endregion

  }

  #endregion

  #region Misc derived wrapper classes

  /// <summary>
  /// This action simply wraps around DropToFloorAction. The type can easier be identified.
  /// </summary>
  [Serializable]
  public class DropToFloorPivotAction : DropToFloorAction
  {
    public DropToFloorPivotAction(Shape3D shape, Vector3F axis, bool includeShapes)
      : base(shape, Shape3D.DropToFloorMode.Pivot, axis, includeShapes)
    {
    }

    #region ISerializable Members

    protected DropToFloorPivotAction(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
    }

    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);    
    }

    #endregion

  }

  /// <summary>
  /// This action simply wraps around DropToFloorAction. The type can easier be identified.
  /// </summary>
  [Serializable]
  public class DropToFloorBoxAction : DropToFloorAction
  {
    public DropToFloorBoxAction(Shape3D shape, Vector3F axis, bool includeShapes)
      : base(shape, Shape3D.DropToFloorMode.Pivot, axis, includeShapes)
    {
    }
        
    #region ISerializable Members

    protected DropToFloorBoxAction(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
    }

    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);    
    }

    #endregion

  }

  /// <summary>
  /// This action simply wraps around DropToFloorAction. The type can easier be identified.
  /// </summary>
  [Serializable]
  public class DropToFloorBottomCenterAction : DropToFloorAction
  {
    public DropToFloorBottomCenterAction(Shape3D shape, Vector3F axis, bool includeShapes)
      : base(shape, Shape3D.DropToFloorMode.Pivot, axis, includeShapes)
    {
    }
        
    #region ISerializable Members

    protected DropToFloorBottomCenterAction(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
    }

    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);    
    }

    #endregion

  }

  #endregion
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
