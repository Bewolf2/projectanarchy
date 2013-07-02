/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.IO;
using System.Drawing;
using System.ComponentModel;
using CSharpFramework;
using ManagedFramework;
using CSharpFramework.Shapes;
using CSharpFramework.Actions;
using System.Runtime.Serialization;

namespace Editor.Shapes
{
	/// <summary>
	/// Simple shape that is used as a group
	/// </summary>
  [Serializable, ShapeTypeNiceName("Group")]
  public class GroupShape : ShapeBase
  {
    #region Constructor

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="nodeName"></param>
    public GroupShape(string nodeName) : base(nodeName)
    {
    }

    #endregion

    #region GUI

    static int _iGroupIconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_group.bmp"), Color.Magenta);

    /// <summary>
    /// Overridden function to get the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetIconIndex()
    {
      return _iGroupIconIndex;
    }

    #endregion

    #region ShapeBase Members

    /// <summary>
    /// not traceable
    /// </summary>
    public override bool Traceable {get {return false;}}


    /// <summary>
    /// Static version to globally access the folder icon index
    /// </summary>
    public static int GroupIconIndex {get {return _iGroupIconIndex;}}

    /// <summary>
    /// Gets called when the position, orientation or scaling of the next Shape3D parent has been modified.
    /// Forward the event to all children of this group.
    /// </summary>
    public override void OnParentTransformationChanged()
    {
      foreach (ShapeBase childBase in ChildCollection)
        childBase.OnParentTransformationChanged();
    }

    #endregion 
  
    #region ISerializable

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected GroupShape(SerializationInfo info, StreamingContext context) : base(info, context)
    {
    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context) 
    {
      base.GetObjectData(info,context);
    }

    #endregion
  }

  #region Group Creator Plugin

  class GroupCreator : CSharpFramework.IShapeCreatorPlugin
  {    
    public GroupCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_group.bmp"), Color.Magenta);
    }

    public override string GetPluginName() 
    {
      return "Group";
    }

    public override string GetPluginCategory()
    {
      return "Grouping";
    }

    /// <summary>
    /// Returns a short description text
    /// </summary>
    /// <returns></returns>
    public override string GetPluginDescription()
    {
      return "Simple group shape for the shape hierarchy. Will not be exported.";
    }

    public override ShapeBase CreateShapeInstance()
    {
      GroupShape group = new GroupShape("Group");
      return group;
    }
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
