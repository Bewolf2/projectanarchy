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
using CSharpFramework;

namespace TerrainEditorPlugin.Dialogs
{
  public partial class ImportSplitTexturesDlg : Form
  {
    public ImportSplitTexturesDlg()
    {
      InitializeComponent();
      UpdateButtonStates();
    }

    void UpdateButtonStates()
    {
      this.button_OK.Enabled = AnyValid;
    }


    #region Texture filenames

    public bool AnyValid
    {
      get
      {
        if (!string.IsNullOrEmpty(DiffuseFilename))
          return true;
        if (!string.IsNullOrEmpty(NormalmapFilename))
          return true;
        if (!string.IsNullOrEmpty(SpecularFilename))
          return true;
        return false;
      }
    }

    public string DiffuseFilename
    {
      get
      {
        return this.textBox_Diffuse.Text;
      }
      set
      {
        this.textBox_Diffuse.Text = value;
        UpdateButtonStates();
      }
    }

    public string NormalmapFilename
    {
      get
      {
        return this.textBox_Normalmap.Text;
      }
      set
      {
        this.textBox_Normalmap.Text = value;
        UpdateButtonStates();
      }
    }

    public string SpecularFilename
    {
      get
      {
        return this.textBox_Specular.Text;
      }
      set
      {
        this.textBox_Specular.Text = value;
        UpdateButtonStates();
      }
    }

    public bool FilteringBorder
    {
      get
      {
        return this.checkBox_Border.Checked;
      }
      set
      {
        this.checkBox_Border.Checked = value;
      }
    }

    #endregion

    #region Text fields

    private void textBox_Diffuse_TextChanged(object sender, EventArgs e)
    {
      UpdateButtonStates();
    }

    private void textBox_Normalmap_TextChanged(object sender, EventArgs e)
    {
      UpdateButtonStates();
    }

    private void textBox_Specular_TextChanged(object sender, EventArgs e)
    {
      UpdateButtonStates();
    }

    #endregion

    #region Browse files

    string BrowseFilename(string oldFilename)
    {
      OpenFileDialog dlg = new OpenFileDialog();
      dlg.InitialDirectory = EditorManager.Project.ProjectDir;
      dlg.FileName = oldFilename;
      dlg.Filter = "Texture files|*.bmp;*.jpg;*.jpeg;*.png;*.dds;*.tex";
      if (dlg.ShowDialog(this) != DialogResult.OK)
        return oldFilename;
      if (string.IsNullOrEmpty(dlg.FileName))
        return oldFilename;
      return dlg.FileName;
    }


    private void button_ImportDiffuse_Click(object sender, EventArgs e)
    {
      DiffuseFilename = BrowseFilename(DiffuseFilename);
    }

    private void button_ImportNormalmap_Click(object sender, EventArgs e)
    {
      NormalmapFilename = BrowseFilename(NormalmapFilename);
    }

    private void button_ImportSpecular_Click(object sender, EventArgs e)
    {
      SpecularFilename = BrowseFilename(SpecularFilename);
    }

    #endregion

    private void button_OK_Click(object sender, EventArgs e)
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
