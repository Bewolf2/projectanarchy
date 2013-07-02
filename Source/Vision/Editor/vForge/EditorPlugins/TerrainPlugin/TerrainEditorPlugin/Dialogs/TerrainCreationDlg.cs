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
using TerrainBase.Config;
using TerrainManaged;
using TerrainEditorPlugin.Shapes;
using CSharpFramework.Dialogs;
using CSharpFramework;
using System.IO;

namespace TerrainEditorPlugin.Dialogs
{
  public partial class TerrainCreationDlg : Form
  {
    public enum CreationMethod_e
    {
      NewTerrain,
      ExistingTerrain,
      //FromV3D
    }

    public TerrainCreationDlg()
    {
      InitializeComponent();

      CreationMethod = CreationMethod_e.NewTerrain;
    }

    private void TerrainCreationDlg_Load(object sender, EventArgs e)
    {

    }

    private void button_Create_Click(object sender, EventArgs e)
    {

      if (CreationMethod == CreationMethod_e.ExistingTerrain)
      {
        OpenFileDlg dlg = new OpenFileDlg();
        dlg.Caption = "Select existing terrain file";
        dlg.Description = "Select an existing terrain file that should be referenced by this scene";
        dlg.InitialDirectory = EditorManager.Project.ProjectDir;
        dlg.Filter = new string[] { ".vtc" };
        if (dlg.ShowDialog() == DialogResult.OK)
        {
          TerrainShape.TerrainConfig.TerrainFolder = System.IO.Path.GetDirectoryName(dlg.RelativeFileName);
          TerrainShape._bIsReference = true;
          TerrainShape._bBakeMode = true;
          this.DialogResult = DialogResult.OK;
        }
      }
      else if (CreationMethod == CreationMethod_e.NewTerrain)
      {
        TerrainConfigDlg dlg = new TerrainConfigDlg();
        dlg.Config = TerrainShape.TerrainConfig;
        dlg.Config._terrainFolder = TerrainConfig.CreateUniqueFolderName();
        dlg.NewTerrain = true;
        if (dlg.ShowDialog() != DialogResult.OK)
          return;

        string abspath = System.IO.Path.Combine(EditorManager.Project.ProjectDir, dlg.Config.TerrainFolder);
        if (Directory.Exists(abspath))
        {
          if (EditorManager.ShowMessageBox("The specified directory '" + dlg.Config.TerrainFolder + "' already exists.\n\nAny existing terrain inside this folder would be overwritten.\nContinue anyway?",
            "Create terrain", MessageBoxButtons.YesNo, MessageBoxIcon.Question) != DialogResult.Yes)
            return;
        }
        TerrainShape.TerrainConfig = dlg.Config;
        this.DialogResult = DialogResult.OK;
      }
    }

    private void buttonCancel_Click(object sender, EventArgs e)
    {

    }

    CreationMethod_e _creationMethod = CreationMethod_e.ExistingTerrain;

    public CreationMethod_e CreationMethod
    {
      get
      {
        return _creationMethod;
      }

      set
      {
        _creationMethod = value;
        radioButton_Existing.Checked = _creationMethod == CreationMethod_e.ExistingTerrain;
        radioButton_New.Checked = _creationMethod == CreationMethod_e.NewTerrain;
      }
    }

    private void radioButton_Existing_CheckedChanged(object sender, EventArgs e)
    {
      if (((RadioButton)sender).Checked)
        CreationMethod = CreationMethod_e.ExistingTerrain;
    }

    private void radioButton_New_CheckedChanged(object sender, EventArgs e)
    {
      if (((RadioButton)sender).Checked)
        CreationMethod = CreationMethod_e.NewTerrain;
    }


    #region Config

    //TerrainConfig _config = new TerrainConfig();
    TerrainShape _terrainShape = null;

    public TerrainShape TerrainShape
    {
      get { return _terrainShape; }
      set { _terrainShape = value; }
    }

      private void label1_Click(object sender, EventArgs e)
      {

      }

    /*
    public TerrainConfig Config
    {
      get { return _config; }
      set 
      { 
        _config=(TerrainConfig)value.Clone();
      }
    }
    */
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
