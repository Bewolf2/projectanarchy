/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using CSharpFramework;
using CSharpFramework.Serialization;
using CSharpFramework.View;
using CSharpFramework.Shapes;
using System.Windows.Forms;
using CSharpFramework.Actions;
using System.Collections;
using System.Reflection;
using CSharpFramework.Math;
using CSharpFramework.Scene;

namespace FmodEditorPlugin
{

  #region Hotspot

  class HotSpotPlayButton : HotSpotBase
  {
    public HotSpotPlayButton(Shape3D owner, string bitmapfile, uint iColorNormal, uint iColorHigh, float fSize)
      : base(owner, bitmapfile, iColorNormal, PickType.Square, fSize)
    {
      _iColNrml = iColorNormal;
      _iColHigh = iColorHigh;
    }

    uint _iColNrml, _iColHigh;
    bool _bStopIcon = false;

    public bool ShowStopIcon
    {
      set
      {
        if (_bStopIcon == value)
          return;
        _bStopIcon = value;
        if (value)
          IconFilename = @"textures\Media_stop.TGA";
        else
          IconFilename = @"textures\Media_play.TGA";
      }
    }

    public override void EvaluatePosition()
    {
      base.EvaluatePosition();
      Position = Owner.Position;
    }

    public override void OnDragBegin(VisionViewBase view)
    {
      base.OnDragBegin(view);
    }

    public override void OnDrag(VisionViewBase view, float fDeltaX, float fDeltaY)
    {
      base.OnDrag(view, fDeltaX, fDeltaY);
    }

    public override void OnDragEnd(VisionViewBase view)
    {
      if (this.IsMouseOver(view.CurrentContext.MouseX, view.CurrentContext.MouseY))
        base.OnDragEnd(view); // forward this only in this case, so we can handle the OnDragEnd as a button released event
    }

    public override Cursor MouseOverCursor
    {
      get
      {
        return Cursors.Hand;
      }
    }

    public override bool IsMouseOver(float fMouseX, float fMouseY)
    {
      bool bOver = base.IsMouseOver(fMouseX, fMouseY);
      Color = bOver ? _iColHigh : _iColNrml;
      return bOver;
    }

    public override bool MouseOver
    {
      set
      {
        base.MouseOver = value;
        Color = value ? _iColHigh : _iColNrml;
      }
    }
  }

  #endregion

  #region EditorPlugin

  /// <summary>
	/// EditorPlugin : this class must exist and implement the IEditorPluginModule functions to add the shape creators (for instance)
	/// Also, the namespace (FmodEditorPlugin) must be the same as the plugin dll name, in this case FmodEditorPlugin.EditorPlugin.dll
	/// This is because the editor plugin manager searches for the FmodEditorPlugin.EditorPlugin class when opening the 
	/// FmodEditorPlugin.EditorPlugin.dll assembly
	/// </summary>
	public class EditorPlugin : IEditorPluginModule
	{
    /// <summary>
    /// Constructor of EditorPlugin that sets up plugin information
    /// </summary>
    public EditorPlugin()
    {
      _version = 0;                 //version used for serialization
      _name = "FmodEditorPlugin";
    }

    /// <summary>
    /// This scope holds one global instance of the plugin info. It is returned by all shape's overridden GetPluginInformation function.
    /// This global info is filled inside InitPluginModule
    /// </summary>
    public static EditorPluginInfo EDITOR_PLUGIN_INFO = new EditorPluginInfo();

    /// <summary>
    /// InitPluginModule : called at plugin initialization time: Add the relevant shape creators here
    /// </summary>
    public override bool InitPluginModule()
    {
      EDITOR_PLUGIN_INFO.NativePluginNames = new string[] { "Fmod" };
      FmodManaged.ManagedModule.InitManagedModule();

      creators = new IShapeCreatorPlugin[]
                 {
                   new FmodSoundShapeCreator(),  
                   new FmodEventShapeCreator(),
                   new FmodCollisionMeshShapeCreator(),
                   new FmodReverbShapeCreator()
                 };

      // add them to the editor
      foreach (IShapeCreatorPlugin plugin in creators)
        EditorManager.ShapeCreatorPlugins.Add( plugin );

      EditorManager.SceneChanged += new SceneChangedEventHandler(EditorManager_SceneChanged);
      IScene.EngineInstancesChanged += new EngineInstancesChangedEventHandler(IScene_EngineInstancesChanged);

      _launchDesignerPlugin = new LaunchDesignerPlugin();
      EditorManager.AddMenuItemPlugin(_launchDesignerPlugin);
      return true;
    }


    /// <summary>
    /// DeInitPluginModule : called at plugin deinitialization time: Remove relevant data
    /// </summary>
    public override bool DeInitPluginModule()
    {
      FmodManaged.ManagedModule.DeInitManagedModule();

      // deregister the creator plugins again
      foreach (IShapeCreatorPlugin plugin in creators)
        EditorManager.ShapeCreatorPlugins.Remove( plugin );

      EditorManager.SceneChanged -= new SceneChangedEventHandler(EditorManager_SceneChanged);
      IScene.EngineInstancesChanged -= new EngineInstancesChangedEventHandler(IScene_EngineInstancesChanged);

      if (_launchDesignerPlugin != null)
        EditorManager.RemoveMenuItemPlugin(_launchDesignerPlugin);

      return true;
    }

    #region Sound to Fmod migration helpers

    #region Migrate Sound-Links Action

    /// <summary>
    /// Custom action that migrate link properties from old SoundShape to FmodSoundShape
    /// </summary>
    public class MigrateSoundLinksAction : IAction
    {
      public MigrateSoundLinksAction(ShapeObject3D oldShape, FmodSoundShape newShape)
      {
        _oldShape = oldShape;
        _newShape = newShape;
      }

      public override void Do()
      {
        GroupAction actions = new GroupAction("Migrate sounds links");
        foreach (LinkTarget oldLinkTarget in _oldShape.LinkTargets)
        {
          if ((oldLinkTarget.Name == "Pause") || (oldLinkTarget.Name == "Resume"))
          {
            // The _newShape.LinkTargets collection is empty unless the engine instance has been created. 
            // This is the case because the AddShape action has been executed prior to this.
            foreach (LinkTarget newLinkTarget in _newShape.LinkTargets)
            {
              if (newLinkTarget.Name == oldLinkTarget.Name)
              {
                foreach (ShapeLink oldShapeLink in oldLinkTarget.Links)
                {
                  // create links for new shape
                  actions.Add(new LinkAction(newLinkTarget, oldShapeLink));

                  foreach (ShapeLink ownerShapeLink in oldShapeLink.Links)
                  {
                    if (ownerShapeLink.OwnerShape.GetType().FullName == "SoundEditorPlugin.SoundShape") // old shape class?
                    {
                      if ((ownerShapeLink.Name == "Pause") || (ownerShapeLink.Name == "Resume"))
                      {
                        // delete links for owner of link (e.g. a Triggerbox) to old shape
                        actions.Add(new UnlinkAction(oldShapeLink, ownerShapeLink));

                        // create links for owner of link (e.g. a Triggerbox) to new shape 
                        actions.Add(new LinkAction(oldShapeLink, newLinkTarget));
                      }
                    }
                  }
                }
              }
            }
          }
        }
        // batch actions and execute in the end so we do not modify collections inside loops
        actions.Do();
      }

      public override void Undo()
      {
        // no undo for conversion!
      }

      /// <summary>
      /// Short name of the action, to show in the menu
      /// </summary>
      public override string ShortName
      {
        get { return "Migrate sound-links"; }
      }

      ShapeObject3D _oldShape;
      FmodSoundShape _newShape;
    }

    #endregion


    #region Migrate Children Action

    /// <summary>
    /// Custom action that migrate children from old shapes to new Fmod shape
    /// </summary>
    public class MigrateChildrenAction : IAction
    {
      public MigrateChildrenAction(ShapeObject3D oldShape, ShapeObject3D newShape)
      {
        _oldShape = oldShape;
        _newShape = newShape;
      }

      public override void Do()
      {
        _newShape.ChildCollection = (ShapeCollection)_oldShape.ChildCollection.ShallowCopy();
        _oldShape.ChildCollection.Clear();
      }

      public override void Undo()
      {
        // no undo for conversion!
      }

      /// <summary>
      /// Short name of the action, to show in the menu
      /// </summary>
      public override string ShortName
      {
        get { return "Migrate Children"; }
      }

      ShapeObject3D _oldShape;
      ShapeObject3D _newShape;
    }

    #endregion

    void AddSoundShapesRecursive(ShapeCollection shapeList, ShapeBase parent)
    {
      if (parent.ShapeVirtualCounter > 0 || !parent.Modifiable)
        return;

      ShapeBase soundShape = parent as ShapeObject3D;
      if (soundShape != null && (soundShape.GetType().FullName == "SoundEditorPlugin.SoundShape" || soundShape.GetType().FullName == "SoundEditorPlugin.SoundCollisionShape"))
        shapeList.Add(soundShape);

      if (parent.HasChildren())
        foreach (ShapeBase shape in parent.ChildCollection)
          AddSoundShapesRecursive(shapeList, shape);
    }

    void MigrateBaseProperties(ShapeObject3D oldShape, ShapeObject3D newShape)
    {
      // Shape settings 
      newShape.UserDataString = (string)oldShape.GetType().GetProperty("UserDataString").GetValue(oldShape, null);
      newShape.ExportShape = (bool)oldShape.GetType().GetProperty("ExportShape").GetValue(oldShape, null);
      newShape.Visible = (bool)oldShape.GetType().GetProperty("Visible").GetValue(oldShape, null);
      newShape.TextLabel = (string)oldShape.GetType().GetProperty("TextLabel").GetValue(oldShape, null);
      newShape.ObjectKey = (string)oldShape.GetType().GetProperty("ObjectKey").GetValue(oldShape, null);
      newShape.LocalID = oldShape.LocalID; // preserve the ID as sound instances might be identified by this in customer plugins [#5893]

      // Components
      if (oldShape.ComponentCount > 0)
      {
        foreach (ShapeComponent comp in oldShape.Components)
        {
          if (comp.Missing)
            continue;
          newShape.AddComponentInternal((ShapeComponent)comp.Clone());
        }
      }

      // Transformation
      newShape.Position = (Vector3F)oldShape.GetType().GetProperty("Position").GetValue(oldShape, null);
      newShape.Orientation = (Vector3F)oldShape.GetType().GetProperty("Orientation").GetValue(oldShape, null);
      newShape.Scaling = (Vector3F)oldShape.GetType().GetProperty("Scaling").GetValue(oldShape, null);
    }

    void MigrateSoundShapeProperties(ShapeObject3D oldShape, FmodSoundShape newShape)
    {
      MigrateBaseProperties(oldShape, newShape);
      
      // Sound specific
      newShape.Filename = (string)oldShape.GetType().GetProperty("Filename").GetValue(oldShape, null);
      newShape.Is3D = (bool)oldShape.GetType().GetProperty("Is3D").GetValue(oldShape, null);
      newShape.Streaming = (bool)oldShape.GetType().GetProperty("Streaming").GetValue(oldShape, null);
      newShape.Looped = (bool)oldShape.GetType().GetProperty("Looped").GetValue(oldShape, null);
      {
        PropertyInfo propInfo = oldShape.GetType().GetProperty("StartPaused");
        if (propInfo != null)
          newShape.StartPaused = (bool)propInfo.GetValue(oldShape, null);
      }
      {
        PropertyInfo propInfo = oldShape.GetType().GetProperty("DisposeWhenFinished");
        if (propInfo != null)
          newShape.DisposeWhenFinished = (bool)propInfo.GetValue(oldShape, null);
      }
      newShape.Volume = (float)oldShape.GetType().GetProperty("Volume").GetValue(oldShape, null);
      newShape.MinDistance = (float)oldShape.GetType().GetProperty("MinDistance").GetValue(oldShape, null);
      newShape.MaxDistance = (float)oldShape.GetType().GetProperty("MaxDistance").GetValue(oldShape, null);
      newShape.Pan = (float)oldShape.GetType().GetProperty("Pan").GetValue(oldShape, null);
      newShape.Priority = (int)oldShape.GetType().GetProperty("Priority").GetValue(oldShape, null);
      newShape.Directional = (bool)oldShape.GetType().GetProperty("Directional").GetValue(oldShape, null);
      newShape.ConeInside = (float)oldShape.GetType().GetProperty("ConeInside").GetValue(oldShape, null);
      newShape.ConeOutside = (float)oldShape.GetType().GetProperty("ConeOutside").GetValue(oldShape, null);
      {
        PropertyInfo propInfo = oldShape.GetType().GetProperty("BackgroundMusic");
        if (propInfo != null)
          newShape.BackgroundMusic = (bool)propInfo.GetValue(oldShape, null);
      }
    }

    void MigrateSoundCollisionShapeProperties(ShapeObject3D oldShape, FmodCollisionMeshShape newShape)
    {
      MigrateBaseProperties(oldShape, newShape);

      // Sound-Collision specific      
      newShape.MeshFilename = (string)oldShape.GetType().GetProperty("ModelFilename").GetValue(oldShape, null);
      newShape.DirectOcclusion = (float)oldShape.GetType().GetProperty("DirectOcclusion").GetValue(oldShape, null);
      newShape.ReverbOcclusion = (float)oldShape.GetType().GetProperty("ReverbOcclusion").GetValue(oldShape, null);
    }

    /// <summary>
    /// This function converts all shapes from the old SoundPlugin to the corresponding shapes of the new FmodPlugin
    /// </summary>
    void MigrateToFmodShapes()
    {
      // If old Sound plugin is not loaded, don't do anything
      IEditorPluginModule soundPlugin = EditorManager.GetPluginByName("SoundEditorPlugin.EditorPlugin");
      if (soundPlugin == null || !soundPlugin.Initialized)
        return;

      // collect all sound specific shapes
      ShapeCollection soundShapes = new ShapeCollection();
      foreach (Layer layer in EditorManager.Scene.Layers)
        if (layer.Modifiable && layer.Loaded)
          AddSoundShapesRecursive(soundShapes, layer.Root);

      if (soundShapes.Count == 0)
        return;

      // prompt a dialog
      DialogResult res = EditorManager.ShowMessageBox("Shapes from old Sound Plugin have been found in loaded layers.\n\nShould these be permanently converted to the corresponding shapes of the Fmod Plugin?", "Old Sound Plugin and Fmod Plugin are both loaded", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
      if (res != DialogResult.Yes)
        return;

      ShapeCollection newShapes = new ShapeCollection();
      int iConvertedCount = 0;
      if (soundShapes.Count > 0)
      {
        GroupAction actions = new GroupAction("Migrate Sound shapes");
        foreach (ShapeObject3D oldShape in soundShapes)
        {
          ShapeObject3D newShape = null;
          if (oldShape.GetType().FullName == "SoundEditorPlugin.SoundShape")
          {
            newShape = new FmodSoundShape(oldShape.ShapeName);
            MigrateSoundShapeProperties(oldShape, (FmodSoundShape)newShape);

            actions.Add(AddShapeAction.CreateAddShapeAction(newShape, oldShape.Parent, oldShape.ParentLayer, false)); 
        
            actions.Add(new MigrateSoundLinksAction(oldShape, (FmodSoundShape)newShape));
            actions.Add(new MigrateChildrenAction(oldShape, newShape));
            actions.Add(RemoveShapeAction.CreateRemoveShapeAction(oldShape));
            newShapes.Add(newShape);
          }
          else if (oldShape.GetType().FullName == "SoundEditorPlugin.SoundCollisionShape")
          {
            newShape = new FmodCollisionMeshShape(oldShape.ShapeName);
            MigrateSoundCollisionShapeProperties(oldShape, (FmodCollisionMeshShape)newShape);
            
            actions.Add(AddShapeAction.CreateAddShapeAction(newShape, oldShape.Parent, oldShape.ParentLayer, false));
            actions.Add(new MigrateChildrenAction(oldShape, newShape));
            actions.Add(RemoveShapeAction.CreateRemoveShapeAction(oldShape));
            newShapes.Add(newShape);
          }
          if (newShape == null)
            continue;

          iConvertedCount++;
        }

        // EditorManager.Actions.Add() is not used, in order to prevent a undo of the conversion
        actions.Do();
      }

      // ensure, that all migrated childs have valid engine instances
      foreach (ShapeBase shape in newShapes)
        foreach (ShapeBase child in shape.ChildCollection)
          child.ReCreateEngineInstance(true);

      EditorManager.ShowMessageBox(iConvertedCount.ToString() + " Shape(s) have been successfully converted.", "Sound to Fmod shapes conversion", MessageBoxButtons.OK, MessageBoxIcon.Information);
    }

    bool bNewScene = true;

    void EditorManager_SceneChanged(object sender, SceneChangedArgs e)
    {
      // don't convert here as engine instances are not there yet
      bNewScene = true;
    }

    void IScene_EngineInstancesChanged(object sender, EngineInstancesChangedArgs e)
    {
      if (e.action == EngineInstancesChangedArgs.Action.PostCreateAll && bNewScene)
      {
        bNewScene = false;
        try
        {
          MigrateToFmodShapes();
        }
        catch (Exception ex)
        {
          EditorManager.DumpException(ex, true);
        }
      }
    }

    #endregion

    #region class LaunchDesignerPlugin  

    class LaunchDesignerPlugin : IMenuItemPlugin
    {
      public LaunchDesignerPlugin()
      {
        _item = new ToolStripMenuItem("Launch Fmod Designer");
        _subItem1 = new ToolStripMenuItem("Last event project");
        _subItem2 = new ToolStripMenuItem("Selected event project");
        _item.DropDownItems.Add(_subItem1);
        _item.DropDownItems.Add(_subItem2);
      }

      public override void OnInitialize(ToolStripMenuItem parent)
      {
        base.OnInitialize(parent);
        _item.DropDownOpening += new EventHandler(_item_Popup);
        _subItem1.Click += new EventHandler(_subItem1_Click);
        _subItem2.Click += new EventHandler(_subItem2_Click);
      }

      public override void OnDeinitialize(ToolStripMenuItem parent)
      {
        base.OnDeinitialize(parent);
        _item.DropDownOpening -= new EventHandler(_item_Popup);
        _subItem1.Click -= new EventHandler(_subItem1_Click);
        _subItem1.Click -= new EventHandler(_subItem2_Click);
      }

      void _item_Popup(object sender, EventArgs e)
      { 
        // Connecting multiple instances of the Fmod Designer tool with the running application (vForge)
        // leads to a crash of the Fmod Designer tool. Therefore disable the menu-items, if already an 
        // instance of the Fmod Designer tool is running.
        bool bEnable = !FmodManaged.ManagedModule.FindDesignerProcess();
        _subItem1.Enabled = bEnable;
        _subItem2.Enabled = bEnable;
      }

      void _subItem1_Click(object sender, EventArgs e)
      {
        FmodManaged.ManagedModule.LaunchDesigner(null);  
      }

      void _subItem2_Click(object sender, EventArgs e)
      {
        //Show project open dialog
        FileDialog fileDlg = new OpenFileDialog();
        fileDlg.Title = "Open Event Project";
        fileDlg.Filter = "Event Project files (*.fdp)|*.fdp";
        fileDlg.RestoreDirectory = true;
        if (fileDlg.ShowDialog() != DialogResult.OK)
          return;

        // now load the event project
        FmodManaged.ManagedModule.LaunchDesigner(fileDlg.FileName);
      }

      public override string Name
      {
        get { return "Launch Fmod Designer"; }
      }

      ToolStripMenuItem _item, _subItem1, _subItem2;

      public override System.Windows.Forms.ToolStripMenuItem MenuItem
      {
        get { return _item; }
      }

      public override System.Windows.Forms.ToolStripItem ToolbarItem
      {
        get { return null; }
      }

      public override IMenuItemPlugin.Category ParentMenu
      {
        get { return Category.Extras; }
      }

      public override IMenuItemPlugin.ToolbarCategory ParentToolbarButton
      {
        get { return ToolbarCategory.None; }
      }
    }

    #endregion

    IShapeCreatorPlugin[] creators;
    LaunchDesignerPlugin _launchDesignerPlugin;
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
