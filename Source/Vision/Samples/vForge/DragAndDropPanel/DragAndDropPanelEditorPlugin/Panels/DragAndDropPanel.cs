/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using CSharpFramework.Docking;
using CSharpFramework;
using DragAndDropPanelPlugin.Classes;

namespace DragAndDropPanelPlugin.Panels
{
  public partial class DragAndDropPanel : DockableForm
  {
    #region Private Members

    /// <summary>
    /// Stores all entries for the listview, this is kept here so we can filter the listview easily
    /// </summary>
    List<DragAndDropPanelEntry> _Entries = new List<DragAndDropPanelEntry>();

    #endregion


    #region Constructor

    /// <summary>
    /// Default constructor
    /// </summary>
    /// <param name="container"></param>
    public DragAndDropPanel(DockingContainer container)
      : base(container)
    {
      Text = "Meshes And Models";
      TabText = "Meshes And Models";

      InitializeComponent();

      // register event handlers
      listViewEntities.ItemDrag += new ItemDragEventHandler(listViewEntities_ItemDrag);
      listViewEntities.ItemMouseHover += new ListViewItemMouseHoverEventHandler(listViewEntities_ItemMouseHover);

      EditorManager.SceneChanged += new SceneChangedEventHandler(EditorManager_SceneChanged);

      toolStripButtonMeshes.Checked = true;
      toolStripButtonModels.Checked = true;

      UpdateStatus();
    }


    public void DeregisterEventHandler()
    {
      listViewEntities.ItemDrag -= new ItemDragEventHandler(listViewEntities_ItemDrag);
      listViewEntities.ItemMouseHover -= new ListViewItemMouseHoverEventHandler(listViewEntities_ItemMouseHover);

      EditorManager.SceneChanged -= new SceneChangedEventHandler(EditorManager_SceneChanged);
    }

    /// <summary>
    /// Called when the scene changes
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void EditorManager_SceneChanged(object sender, SceneChangedArgs e)
    {
      UpdateStatus();
      
      if (e.newscene == null)
      {
        listViewEntities.Clear();
        _Entries.Clear();
      }
      else if (e.newscene != null)
        PopulateList(EditorManager.Project.ProjectDir);
    }

    #endregion

    #region Event Handling

    void listViewEntities_ItemMouseHover(object sender, ListViewItemMouseHoverEventArgs e)
    {
      // TODO: When the mouse hovers over an item it would be nice to show a preview of the resource in the engine view
      // (This needs some modifications on managed c++ code and is thus not included in this version of the entity panel)
    }

    /// <summary>
    /// Event handler for item dragging
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void listViewEntities_ItemDrag(object sender, ItemDragEventArgs e)
    {
      if(e.Item == null)
        return;

      ListViewItem Item = e.Item as ListViewItem;

      if(Item == null || Item.Tag == null || !(Item.Tag is DragAndDropPanelEntry))
        return;

      DragAndDropPanelEntry Entry = Item.Tag as DragAndDropPanelEntry;

      switch (Entry.Type)
      {
        case DragAndDropPanelEntry.EntryType.Model:
          DoDragDrop(new ModelShapeCreator(EditorManager.Project.MakeRelative(Entry.Path)), DragDropEffects.Copy | DragDropEffects.Scroll);
          break;
        case DragAndDropPanelEntry.EntryType.StaticMesh:
          DoDragDrop(new StaticMeshShapeCreator(EditorManager.Project.MakeRelative(Entry.Path)), DragDropEffects.Copy | DragDropEffects.Scroll);
          break;
      }
    }

    private void toolStripButtonModels_Click(object sender, EventArgs e)
    {
      toolStripButtonModels.Checked = !toolStripButtonModels.Checked;
      FilterList();
    }

    private void toolStripButtonMeshes_Click(object sender, EventArgs e)
    {
      toolStripButtonMeshes.Checked = !toolStripButtonMeshes.Checked;
      FilterList();
    }

    private void toolStripTextBoxFilter_TextChanged(object sender, EventArgs e)
    {
      FilterList();
    }

    private void toolStripButtonClearText_Click(object sender, EventArgs e)
    {
      toolStripTextBoxFilter.Text = String.Empty;
    }

    #endregion

    #region Helper Methods

    /// <summary>
    /// Updates the status of the engine panel
    /// </summary>
    private void UpdateStatus()
    {
      bool HasOpenScene = EditorManager.Scene != null;
      Enabled = HasOpenScene;

      if (!HasOpenScene)
      {
        toolStripTextBoxFilter.Text = "";
      }
    }

    /// <summary>
    /// Populates the listview with all models
    /// </summary>
    /// <param name="Path"></param>
    private void PopulateList(string Path)
    {
      _Entries.Clear();

      Stack<string> Directories = new Stack<string>(32);
      Directories.Push(Path);

      // Iterate over all elements in the path
      while (Directories.Count > 0)
      {
        string CurrentDir = Directories.Pop();
        
        // add all subdirectories to the stack
        string[] SubDirectories = System.IO.Directory.GetDirectories(CurrentDir);

        foreach (string SubDirectory in SubDirectories)
          Directories.Push(SubDirectory);

        // add all models to the entry list
        string[] Models = System.IO.Directory.GetFiles(CurrentDir, "*.model");
        foreach (string Model in Models)
          AddModel(Model);

        // add all static meshes to the entry list
        string[] Meshes = System.IO.Directory.GetFiles(CurrentDir, "*.vmesh");
        foreach (string Mesh in Meshes)
          AddMesh(Mesh);
      }

      // By default show models and static meshes
      toolStripTextBoxFilter.Text = "";
      FilterList();
    }

    /// <summary>
    /// Helper function to filter the list view
    /// </summary>
    private void FilterList()
    {
      if (!Enabled)
        return;

      listViewEntities.Clear();
      listViewEntities.BeginUpdate();

      string SearchText = toolStripTextBoxFilter.Text.ToLower();
      bool ShowModels = toolStripButtonModels.Checked;
      bool ShowMeshes = toolStripButtonMeshes.Checked;

      if (SearchText == String.Empty)
      {
        foreach (DragAndDropPanelEntry Entry in _Entries)
        {
          if (Entry.Type == DragAndDropPanelEntry.EntryType.Model && ShowModels)
          {
            AddEntry(Entry);
          }
          else if (Entry.Type == DragAndDropPanelEntry.EntryType.StaticMesh && ShowMeshes)
          {
            AddEntry(Entry);
          }
        }
      }
      else
      {
        foreach (DragAndDropPanelEntry Entry in _Entries)
        {
          if (Entry.DisplayText.ToLower().IndexOf(SearchText) != -1)
          {
            if (Entry.Type == DragAndDropPanelEntry.EntryType.Model && ShowModels)
            {
              AddEntry(Entry);
            }
            else if (Entry.Type == DragAndDropPanelEntry.EntryType.StaticMesh && ShowMeshes)
            {
              AddEntry(Entry);
            }
          }
        }
      }

      listViewEntities.EndUpdate();
    }

    /// <summary>
    /// Adds a model to the entry list
    /// </summary>
    /// <param name="Path">The absolute model path</param>
    private void AddModel(string Path)
    {
      string ModelName = System.IO.Path.GetFileNameWithoutExtension(Path);

      // Add thumbnail image if it exists
      string ThumbnailPath = System.IO.Path.Combine(System.IO.Path.GetDirectoryName(Path), ModelName + @".model_data\Thumbnail.jpg");

      if (System.IO.File.Exists(ThumbnailPath))
      {
        AddImage(ThumbnailPath, ModelName);
      }

      _Entries.Add(new DragAndDropPanelEntry(DragAndDropPanelEntry.EntryType.Model, Path, ModelName, ModelName));
    }

    /// <summary>
    /// Adds a static mesh to the entry list
    /// </summary>
    /// <param name="Path">The absolute mesh path</param>
    private void AddMesh(string Path)
    {
      string MeshName = System.IO.Path.GetFileNameWithoutExtension(Path);

      // Add thumbnail image if it exists
      string ThumbnailPath = System.IO.Path.Combine(System.IO.Path.GetDirectoryName(Path), MeshName + @".vmesh_data\Thumbnail.jpg");

      if (System.IO.File.Exists(ThumbnailPath))
      {
        AddImage(ThumbnailPath, MeshName);
      }

      _Entries.Add(new DragAndDropPanelEntry(DragAndDropPanelEntry.EntryType.StaticMesh, Path, MeshName, MeshName));
    }

    /// <summary>
    /// Helper function to add the actual listview entry
    /// </summary>
    /// <param name="Text"></param>
    /// <param name="Path"></param>
    /// <param name="Key"></param>
    private void AddEntry(DragAndDropPanelEntry Entry)
    {
      ListViewItem LVI = new ListViewItem(Entry.DisplayText);

      string Key = Entry.Thumbnailkey;

      if (!imageList.Images.ContainsKey(Key))
        Key = "<NoThumbnail>";

      LVI.ImageKey = Key;
      LVI.Tag = Entry;
      LVI.ToolTipText = EditorManager.Project.MakeRelative(Entry.Path);
      LVI.ForeColor = Entry.Type == DragAndDropPanelEntry.EntryType.Model ? Color.Navy : Color.Firebrick;

      listViewEntities.Items.Add(LVI);
    }

    /// <summary>
    /// Helper function to add an image to the imagelist
    /// </summary>
    /// <param name="Path"></param>
    /// <param name="Key"></param>
    private void AddImage(string Path, string Key)
    {
      using (Image Image = Bitmap.FromFile(Path))
      {
        imageList.Images.Add(Key, Image);
      }
    }

    #endregion

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
