/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Windows.Forms;
using System.Drawing;
using CSharpFramework;
using CSharpFramework.Math;
using CSharpFramework.Shapes;
using CSharpFramework.Actions;
using NodeManaged;
using System.Runtime.Serialization;
using ManagedFramework;
using System.IO;

namespace CustomShapeEditorPlugin
{
  #region Node Link classes

  /// <summary>
  /// Link source class for a node
  /// </summary>
  [Serializable]
  public class BidirectionalNodeLink : LinkBidirectional
  {
    public BidirectionalNodeLink(Shape3D ownerShape) : base(ownerShape,"links to", "NodeSource_1")
    {
    }

    #region ISerializable Members

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected BidirectionalNodeLink(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      // nothing in addition to LinkSource
    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info,context);
      // nothing in addition to LinkSource
    }

    #endregion
  }

  #endregion

  #region class NodeShape
	/// <summary>
	/// NodeShape : This is the class that represents the shape in the editor. It has an engine instance that handles the
	/// native code. The engine instance code in located in the NodeManaged project (managed C++ class library)
	/// </summary>
  [Serializable]
  public class NodeShape : Shape3D
	{
    #region Constructor

    /// <summary>
    /// The constructor of the node shape, just takes the node name
    /// </summary>
    /// <param name="name">Name of the shape in the shape tree</param>
		public NodeShape(string name) : base(name)
		{
    }

    #endregion

    #region Icon

    /// <summary>
    /// Get an index once (executed the first time an instance of this shape is created)
    /// </summary>
    static int iIconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\Node.bmp"), Color.Magenta);

    /// <summary>
    /// Return the icon index in the image list for the shape tree view. For this, use a static variable for this class.
    /// </summary>
    public override int GetIconIndex()
    {
      return iIconIndex;
    }

    /// <summary>
    /// Gets the icon index for selected icons in the tree view. Simply pass the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetSelectedIconIndex()
    {
      return iIconIndex;
    }

    #endregion

    #region Engine Instance

    /// <summary>
    /// Function to create the engine instance of the shape. The engine instance is of type EngineInstanceNode
    /// and located in the managed code library.
    /// </summary>
    /// <param name="bCreateChildren">relevant for the base class to create instances for children</param>
    public override void CreateEngineInstance(bool bCreateChildren)
    {
      base.CreateEngineInstance (bCreateChildren);
      _engineInstance = new EngineInstanceNode();
      SetEngineInstanceBaseProperties(); // sets the position etc.
    }

    /// <summary>
    /// Removes the engine instance
    /// </summary>
    /// <param name="bRemoveChildren">relevant for the base class to cleanup its children</param>
    public override void RemoveEngineInstance(bool bRemoveChildren)
    {
      base.RemoveEngineInstance(bRemoveChildren);

      // nothing else to do here (_engineInstance already destroyed in base.RemoveEngineInstance
    }

    /// <summary>
    /// Helper field to access the engine instance as casted class to perform specfic operations on it
    /// </summary>
    EngineInstanceNode EngineNode {get {return (EngineInstanceNode)_engineInstance;}}

    /// <summary>
    /// Overridden render function: Let the engine instance render itself and render a box
    /// </summary>
    /// <param name="view"></param>
    /// <param name="mode"></param>
    public override void RenderShape(VisionViewBase view, ShapeRenderMode mode)
    {
      EngineNode.RenderShape(view,mode);
      base.RenderShape(view, mode);
    }

    /// <summary>
    /// Set all properties on the engine instance. The base implementation already sets position and orientation,
    /// so there is nothing else to do here
    /// </summary>
    public override void SetEngineInstanceBaseProperties()
    {
      base.SetEngineInstanceBaseProperties ();
      // set other properties here : EngineNode.SetXYZ()
    }

    /// <summary>
    /// Pick the shape in the view. Use helper function trace the local bounding box.
    /// </summary>
    /// <param name="rayStart">trace ray start</param>
    /// <param name="rayEnd">trace ray end</param>
    /// <param name="result">esult structure to fill in</param>
    public override void OnTraceShape(ShapeTraceMode_e mode, Vector3F rayStart, Vector3F rayEnd, ref ShapeTraceResult result)
    {
      if (ConversionUtils.TraceOrientedBoundingBox(LocalBoundingBox,Position,RotationMatrix, rayStart,rayEnd, ref result))
        result.hitShape = this;
    }


    #endregion

    #region Linking to other Nodes

    /// <summary>
    /// Create the links used by this shape type
    /// </summary>
    public override void CreateAllLinks()
    {
      base.CreateAllLinks ();

      // add a bi-directional link type to this shape so we can link it to other nodes. We use bi-directional links here
      // because the relationship between nodes is symmetrical and we do not care which one is source or target.
      this.LinkBidirections.Add(new BidirectionalNodeLink(this));
    }

    /// <summary>
    /// Inidcates whether a shape can be linked to this shape
    /// </summary>
    /// <param name="child">Shape instance to test</param>
    /// <returns>true, if the shape can be linked</returns>
    public override bool CanLink(ShapeLink src, ShapeLink target)
    {
      if (base.CanLink (src, target))
        return true;

      // check whether we are linking two nodes. The node can be a target or source.
      // we always link in both directions node1 <-> node2
      return (src is BidirectionalNodeLink) && (target is BidirectionalNodeLink);
    }


    /// <summary>
    /// Performs the linking. Before, it has been checked whether it can be linked (CanLink)
    /// </summary>
    /// <param name="child"></param>
    /// <returns></returns>
    public override void OnLink(ShapeLink src, ShapeLink target)
    {
      base.OnLink (src, target);
      // forward the linking to the engine instance so that the native class can link to other native instances
      if (src.OwnerShape==this && (target is BidirectionalNodeLink))
        this.EngineNode.LinkNode((EngineInstanceNode)target.OwnerShape._engineInstance);
      if (target.OwnerShape==this && (src is BidirectionalNodeLink))
        this.EngineNode.LinkNode((EngineInstanceNode)src.OwnerShape._engineInstance);
    }


    /// <summary>
    /// Performs the unlinking
    /// </summary>
    /// <param name="src"></param>
    /// <param name="target"></param>
    public override void OnUnlink(ShapeLink src, ShapeLink target)
    {
      base.OnUnlink (src, target);
      // forward the unlinking to the engine instance so that the native class can unlink from other native instances
      if (src.OwnerShape==this && (target is BidirectionalNodeLink))
        this.EngineNode.UnLinkNode((EngineInstanceNode)target.OwnerShape._engineInstance);
      if (target.OwnerShape==this && (src is BidirectionalNodeLink))
        this.EngineNode.UnLinkNode((EngineInstanceNode)src.OwnerShape._engineInstance);
    }

    #endregion

    #region Serialization and Export

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected NodeShape(SerializationInfo info, StreamingContext context) : base(info, context)
    {
      //..nothing to do
    }

    /// <summary>
    /// Called during export to collect native plugin information. In this case, return the global instance that applies for all shpes in this plugin
    /// </summary>
    /// <returns></returns>
    public override CSharpFramework.Serialization.EditorPluginInfo GetPluginInformation()
    {
      return EditorPlugin.EDITOR_PLUGIN_INFO;
    }
  
    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context)
    {
      base.GetObjectData (info, context);
      // nothing else to do...
    }

    /// <summary>
    /// Called when exporting the scene to engine archive. base implemenation calls function on engine object which in turn
    /// serializes itself
    /// </summary>
    /// <returns></returns>
    public override bool OnExport(CSharpFramework.Scene.SceneExportInfo info)
    {
      return base.OnExport (info);
    }

    #endregion
  }
  #endregion

  #region Shape Creator Plugin


  /// <summary>
  /// Creator class. An instance of the creator is registerd in the plugin init function. Thus the creator shows
  /// up in the "Create" menu of the editor
  /// </summary>
  class NodeShapeCreator : CSharpFramework.IShapeCreatorPlugin
  {

    /// <summary>
    /// Constructor
    /// </summary>
    public NodeShapeCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\Node.bmp"), Color.Magenta);
    }

    /// <summary>
    /// Get the name of the plugin, for instance the shape name. This name apears in the "create" menu
    /// </summary>
    /// <returns>creator name</returns>
    public override string GetPluginName()
    {
      return "Node";
    }

    /// <summary>
    /// Get the plugin category name to sort the plugin name. This is useful to group creators. A null string can
    /// be returned to put the creator in the root
    /// </summary>
    /// <returns></returns>
    public override string GetPluginCategory()
    {
      return "Misc";
    }

    /// <summary>
    /// Returns a short description text
    /// </summary>
    /// <returns></returns>
    public override string GetPluginDescription()
    {
      return "Creates a node instance that can be linked to other nodes.\n"+
        "This shape is just a sample for custom vForge plugins.";
    }

    public override ShapeBase CreateShapeInstance()
    {
      NodeShape shape = new NodeShape("node");
      shape.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
      return shape;
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
