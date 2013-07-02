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
using System.Linq;
using System.Text;
using System.Windows.Forms;

using CSharpFramework;

namespace Editor.Dialogs
{
  public partial class StartupDlg : Form
  {
    #region Custom DialogResult

    public enum StartupResult_e
    {
      Close,
      LoadStartupGuide,
      LoadStartupProject,
      CreateNewProject,
      OpenProject,
      OpenScene,
      OpenRecentProject,
      OpenRecentScene,
      OpenSampleMap
    }

    #endregion

    #region Constructor 

    public StartupDlg()
    {
      InitializeComponent();
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;

      _mruListProjects = new List<string>();
      _mruListScenes = new List<string>();
    }

    #endregion

    #region Events

    private void StartupDlg_Load(object sender, EventArgs e)
    {
      checkBox_ShowDialog.Checked = EditorManager.Settings.OpenStartupDialog;
      panel_Start.Dock = DockStyle.Fill;
      panel_Recent.Dock = DockStyle.Fill;
      UpdateMRUListView();
      UpdateStartup();
    }

    private void listView_Projects_SelectedIndexChanged(object sender, EventArgs e)
    {
      if (listView_Projects.SelectedItems.Count > 0)
      {
        _selectedFile = listView_Projects.SelectedItems[0].ToolTipText;
      }
    }

    private void listView_Scenes_SelectedIndexChanged(object sender, EventArgs e)
    {
      if (listView_Scenes.SelectedItems.Count > 0)
      {
        _selectedFile = listView_Scenes.SelectedItems[0].ToolTipText;
      }
    }

    private void listView_Projects_DoubleClick(object sender, EventArgs e)
    {
      if (listView_Projects.SelectedItems.Count > 0)
      {
        _selectedFile = listView_Projects.SelectedItems[0].ToolTipText;
        _startupResult = StartupResult_e.OpenRecentProject;
        this.Close();
      }
    }

    private void listView_Scenes_DoubleClick(object sender, EventArgs e)
    {
      if (listView_Scenes.SelectedItems.Count > 0)
      {
        _selectedFile = listView_Scenes.SelectedItems[0].ToolTipText;
        _startupResult = StartupResult_e.OpenRecentScene;
        this.Close();
      }
    }

    private void checkBox_ShowDialog_CheckedChanged(object sender, EventArgs e)
    {
      EditorManager.Settings.OpenStartupDialog = checkBox_ShowDialog.Checked;
    }

    private void button_Close_Click(object sender, EventArgs e)
    {
      _startupResult = StartupResult_e.Close;
      this.Close();
    }

    private void button_Switch_Click(object sender, EventArgs e)
    {
      EditorManager.Settings.ShowRecentFilesFirst = button_Switch.Text.Equals("Recent Files");
      UpdateStartup();
    }

    private void button_NewProject_Click(object sender, EventArgs e)
    {
      _startupResult = StartupResult_e.CreateNewProject;
      this.Close();
    }

    private void button_OpenProject_Click(object sender, EventArgs e)
    {
      _startupResult = StartupResult_e.OpenProject;
      this.Close();
    }

    private void button_OpenScene_Click(object sender, EventArgs e)
    {
      _startupResult = StartupResult_e.OpenScene;
      this.Close();
    }

    private void button_StartupDoc_Click(object sender, EventArgs e)
    {
      _startupResult = StartupResult_e.LoadStartupGuide;
      this.Close();
    }

    private void button_StartupProject_Click(object sender, EventArgs e)
    {
      _startupResult = StartupResult_e.LoadStartupProject;
      this.Close();
    }

    private void button_SampleMaps_Click(object sender, EventArgs e)
    {
      _startupResult = StartupResult_e.OpenSampleMap;
      this.Close();
    }

    protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
    {
      if (keyData == Keys.Escape)
      {
        _startupResult = StartupResult_e.Close;
        this.Close();
      }

      return base.ProcessCmdKey(ref msg, keyData);
    }

    #endregion

    #region Helper

    private void UpdateStartup()
    {
      this.SuspendLayout();

      panel_Start.Visible = panel_Start.Enabled = false;
      panel_Recent.Visible = panel_Recent.Enabled = false;
      button_Switch.Text = EditorManager.Settings.ShowRecentFilesFirst ? "Getting Started" : "Recent Files";

      if (button_Switch.Text.Equals("Recent Files"))
      {
        panel_Start.Visible = panel_Start.Enabled = true;
        panel_Start.Dock = DockStyle.Fill;
      }
      else
      {
        panel_Recent.Visible = panel_Recent.Enabled = true;
        panel_Recent.Dock = DockStyle.Fill;
      }

      this.ResumeLayout();
    }

    private void UpdateMRUListView()
    {
      // Fill MRU lists
      listView_Projects.Clear();
      listView_Scenes.Clear();

      foreach (string filepath in _mruListProjects)
      {
        string filename = System.IO.Path.GetFileName(filepath);
        string dir = System.IO.Directory.GetParent(filepath).Name; 
        ListViewItem item = new ListViewItem(dir + "\\" + filename);
        item.ToolTipText = filepath;
        item.ImageIndex = 0;
        listView_Projects.Items.Add(item);
      }

      foreach (string filepath in _mruListScenes)
      {
        string filename = System.IO.Path.GetFileName(filepath);
        string dir = System.IO.Directory.GetParent(filepath).Name; 
        ListViewItem item = new ListViewItem(dir + "\\" + filename);
        item.ToolTipText = filepath;
        item.ImageIndex = 1;
        listView_Scenes.Items.Add(item);
      }
    }

    #endregion

    #region Public Properties

    /// <summary>
    /// Custom dialog result of startup dialog
    /// </summary>
    public StartupResult_e StartupDialogResult
    {
      get { return _startupResult; }
    }

    public List<string> RecentProjects
    {
      get { return _mruListProjects; }
      set { _mruListProjects = value; }
    }

    public List<string> RecentScenes
    {
      get { return _mruListScenes; }
      set { _mruListScenes = value; }
    }

    public string SelectedFile
    {
      get { return _selectedFile; }
    }

    #endregion

    #region Private Data

    private StartupResult_e _startupResult = StartupResult_e.Close;
    private List<string> _mruListProjects;
    private List<string> _mruListScenes;
    private string _selectedFile;

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
