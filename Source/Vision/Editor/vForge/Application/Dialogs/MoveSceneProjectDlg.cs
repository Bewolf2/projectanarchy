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

namespace Editor.Dialogs
{
  public partial class MoveSceneProjectDlg : Form
  {
    public MoveSceneProjectDlg()
    {
      InitializeComponent();
    }


    public string OldProjectPath
    {
      set
      {
        this.textBox_OldProj.Text = value;
        if (string.IsNullOrEmpty(textBox_newProj.Text))
          textBox_newProj.Text = value;
      }
    }

    public string NewProjectPath
    {
      set
      {
        textBox_newProj.Text = value;
      }
      get
      {
        return textBox_newProj.Text;
      }
    }

    private void button_ProjBrowse_Click(object sender, EventArgs e)
    {
      FileDialog fileDlg = new OpenFileDialog();

      fileDlg.Title = "Select new project file";
      fileDlg.InitialDirectory = NewProjectPath;

      fileDlg.Filter = "Project files (*.Project)|*.Project";
      fileDlg.RestoreDirectory = true;
      if (fileDlg.ShowDialog() != DialogResult.OK)
        return;
      NewProjectPath = fileDlg.FileName;
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
