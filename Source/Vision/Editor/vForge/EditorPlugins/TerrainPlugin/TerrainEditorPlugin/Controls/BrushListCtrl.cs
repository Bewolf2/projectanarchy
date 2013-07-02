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
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using TerrainBase.Editing;
using System.Collections;
using CSharpFramework.View;
using CSharpFramework;
using CSharpFramework.Helper;
using System.IO;

namespace TerrainEditorPlugin.Controls
{
  public partial class BrushListCtrl : UserControl
  {
    public BrushListCtrl()
    {
      InitializeComponent();

      if (EditorManager.DesignMode)
        return;

      TerrainEditor.OnBrushCollectionChanged += new EventHandler(TerrainEditor_OnBrushCollectionChanged);
      TerrainEditor.OnCurrentBrushChanged += new EventHandler(TerrainEditor_OnCurrentBrushChanged);
      Cursor3DProperties.OnCursor3DPropertiesChanged += new EventHandler(TerrainEditor_OnCursor3DPropertiesChanged);
      listView.LargeImageList = imageList.ImageList;

      listView.DragEnter += new DragEventHandler(listView_DragEnter);
      listView.DragDrop += new DragEventHandler(listView_DragDrop);
    }

    #region Drag&Drop Image files

    private string[] GetDraggedBitmapFile(DragEventArgs e)
    {
      if (!e.Data.GetDataPresent("FileNameW"))
        return null;
      string[] filenames = e.Data.GetData("FileNameW") as string[];
      if (filenames == null)
        return null;
      ArrayList arr = new ArrayList();
      foreach (string filename in filenames)
      {
        if (filename == null)
          continue;
        string ext = System.IO.Path.GetExtension(filename);
        if (ext == null)
          continue;
        if (string.Compare(ext, ".bmp", true) == 0/* || string.Compare(ext, ".tga", true) == 0*/)
          arr.Add(filename);
      }
      if (arr.Count == 0)
        return null;
      return (string[])arr.ToArray(typeof(String));
    }

    void listView_DragEnter(object sender, DragEventArgs e)
    {
      if (GetDraggedBitmapFile(e)!=null)
        e.Effect = DragDropEffects.Copy;
    }

    void listView_DragDrop(object sender, DragEventArgs e)
    {
      string[] imgFiles = GetDraggedBitmapFile(e);
      if (imgFiles == null)
        return;

      string dir = TerrainEditor.AbsoluteBrushFilePath;
      bool bAnyExists = false;
      string exists = "";
      string[] tgtNames = new string[imgFiles.Length];
      int i = 0;
      foreach (string filename in imgFiles)
      {
        string relname = Path.GetFileName(filename);
        tgtNames[i] = Path.Combine(dir, relname);
        if (File.Exists(tgtNames[i]))
        {
          bAnyExists = true;
          exists += "\n"+relname;
        }
        i++;
      }

      if (bAnyExists)
      {
        if (EditorManager.ShowMessageBox("A file named\n"+exists+"\n\nalready exists in the target directory.\n\nSelect 'Yes' to overwrite or 'No' to abort","Overwrite files?",MessageBoxButtons.YesNo,MessageBoxIcon.Question)!=DialogResult.Yes)
          return;
      }

      // copy the files
      try
      {
        for (i = 0; i < tgtNames.Length; i++)
          System.IO.File.Copy(imgFiles[i], tgtNames[i], true);
      }
      catch (Exception ex)
      {
        EditorManager.ShowMessageBox("An error occurred while copying file(s):\n"+ex.Message,"File copy error",MessageBoxButtons.OK,MessageBoxIcon.Error);
        EditorManager.DumpException(ex);
        return;
      }

      TerrainEditor.LoadNewBrushFiles(tgtNames);
    }

    #endregion

    #region Update List

    GUI.DynamicImageList imageList = new GUI.DynamicImageList(64);
    void TerrainEditor_OnBrushCollectionChanged(object sender, EventArgs e)
    {
      UpdateListView();
    }

    int _iIgnoreSelectionChange = 0;


    //Made by Benjamin for automated test
    public void SelectBrushById(int id)
    {
      if (id >= listView.Items.Count) 
        id = listView.Items.Count - 1;

      listView.SelectedIndices.Clear();
      listView.SelectedIndices.Add(id);
    }


    void SelectBrushByName(string fullname)
    {
      _iIgnoreSelectionChange++;
      listView.SelectedIndices.Clear();
      if (string.IsNullOrEmpty(fullname))
      {
        _iIgnoreSelectionChange--;
        return;
      }
      foreach (ListViewItem item in this.listView.Items)
      {
        TerrainBrush b = (TerrainBrush)item.Tag;
        if (string.Compare(b.Name, fullname, true)==0)
        {
          listView.SelectedIndices.Add(item.Index);
          break;
        }
      }
      _iIgnoreSelectionChange--;
    }


    void TerrainEditor_OnCurrentBrushChanged(object sender, EventArgs e)
    {
      if (TerrainEditor.CurrentBrush == null)
        SelectBrushByName(null);
      else
        SelectBrushByName(TerrainEditor.CurrentBrush.Name);
    }


    public void UpdateListView()
    {
      _iIgnoreSelectionChange++;
      string oldName = (TerrainEditor.CurrentBrush != null) ? TerrainEditor.CurrentBrush.Name : null;

      listView.BeginUpdate();
      listView.Items.Clear();
      ArrayList all = TerrainEditor.AllBrushes;
      foreach (TerrainBrush b in all)
      {
        ListViewItem item = new ListViewItem(b.DisplayName);
        item.ImageIndex = imageList.AddBitmap(b.Name, Color.Magenta); // no transparency...
        item.Tag = b;
        listView.Items.Add(item);
      }
      listView.EndUpdate();

      SelectBrushByName(oldName); // restore selection
      _iIgnoreSelectionChange--;
    }

    private void listView_SelectedIndexChanged(object sender, EventArgs e)
    {
      if (listView.SelectedItems.Count > 0 && _iIgnoreSelectionChange==0)
        TerrainEditor.CurrentBrush = listView.SelectedItems[0].Tag as TerrainBrush;

      if (listView.SelectedItems.Count == 1)
      {
        listView.SelectedItems[0].EnsureVisible();
      }
    }

    #endregion

    #region Cursor Properties

    void TerrainEditor_OnCursor3DPropertiesChanged(object sender, EventArgs e)
    {
      this.propertyGrid_Cursor.SelectedObject = TerrainEditor.Cursor3DProperties;
    }

    #endregion

    private void propertyGrid_Cursor_Click(object sender, EventArgs e)
    {

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
