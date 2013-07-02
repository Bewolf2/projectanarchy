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
using ManagedBase;

namespace VisionEditorPlugin.Dialogs
{
  public partial class ColorGradingToolDlg : Form
  {
    public ColorGradingToolDlg()
    {
      InitializeComponent();
      this.comboBox_Size.SelectedIndex = 1;
      UpdateButtonStates();
    }

    void BrowseImage(ref string filename, PictureBox image)
    {
      OpenFileDialog dlg = new OpenFileDialog();
      dlg.InitialDirectory = EditorManager.Project.ProjectDir;
      dlg.FileName = filename;
      dlg.Filter = "(Lossless) image files|*.bmp;*.tga;";
      if (dlg.ShowDialog(this) == DialogResult.OK)
        filename = dlg.FileName;
      string info;
      if (string.IsNullOrEmpty(filename))
        image.Image = null;
      else
        image.Image = ImageLoader.LoadImage(filename, out info);
      UpdateButtonStates();
    }

    string _filenameOrig = null;
    string _filenameTarget = null;
    string _outFile = null;

    private void pictureBox_Orig_Click(object sender, EventArgs e)
    {
      BrowseImage(ref _filenameOrig, pictureBox_Orig);
    }

    private void pictureBox_Target_Click(object sender, EventArgs e)
    {
      BrowseImage(ref _filenameTarget, pictureBox_Target);
    }

    void UpdateButtonStates()
    {
      bool bValid = !string.IsNullOrEmpty(_filenameOrig) && !string.IsNullOrEmpty(_filenameTarget);
      this.button_Save.Enabled = bValid;
      label_Click1.Visible = pictureBox_Orig.Image == null;
      label_Click2.Visible = pictureBox_Target.Image == null;

    }

    private void button_Save_Click(object sender, EventArgs e)
    {
      SaveFileDialog dlg = new SaveFileDialog();
      dlg.InitialDirectory = EditorManager.Project.ProjectDir;
      dlg.Filter = "dds files|*.dds";
      dlg.FileName = _outFile;
      if (dlg.ShowDialog(this) != DialogResult.OK)
        return;
      int iSize = int.Parse(comboBox_Size.Text);
      _outFile = dlg.FileName;
      string error = ColorGradingTool.CreateGradingLUT(_filenameOrig, _filenameTarget, _outFile, iSize);
      if (!string.IsNullOrEmpty(error))
        EditorManager.ShowMessageBox("Cannot generate 3D texture : \n\n" + error, "Operation failed", MessageBoxButtons.OK, MessageBoxIcon.Error);


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
