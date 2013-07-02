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
using TerrainBase.Editing;
using CSharpFramework.Scene;
using CSharpFramework;
using TerrainManaged;
using TerrainEditorPlugin.Shapes;

namespace TerrainEditorPlugin.Dialogs
{
  public partial class ApplyHeightmapFilterDlg : Form
  {
    public ApplyHeightmapFilterDlg()
    {
      InitializeComponent();
      propertyGrid_Filter.PropertyValueChanged += new PropertyValueChangedEventHandler(propertyGrid_Filter_PropertyValueChanged);
      progressBar.Visible = false;
      button_Apply.Enabled = false;
      progress.ProgressChanged += new ProgressStatus.ProgressChangedEventHandler(progress_ProgressChanged);
    }

    void propertyGrid_Filter_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
    {
      this.button_Apply.Enabled = _filter != null && _filter.Valid;
    }

    protected override void OnClosing(CancelEventArgs e)
    {
      base.OnClosing(e);
      if (_filter != null)
        _filter.Active = false;
    }


    bool _bHasApplyButton = true;

    public bool HasApplyButton
    {
      get { return _bHasApplyButton; }
      set 
      {
        if (_bHasApplyButton == value)
          return;
        _bHasApplyButton = value;
        button_Apply.Text = _bHasApplyButton ? "Apply" : "OK";
      }
    }

    ITerrainFilter _filter = null;
    ProgressStatus progress = new ProgressStatus();
    public int x1 = 0;
    public int y1 = 0;
    public int x2 = -1;
    public int y2 = -1;

    public ITerrainFilter Filter
    {
      set
      {
        if (_filter != null)
          _filter.Active = false;
        _filter = value;
        if (_filter != null)
          _filter.Active = true;
        dialogCaptionBar1.Caption = _filter.Name;
        propertyGrid_Filter.SelectedObject = _filter;
        propertyGrid_Filter.Enabled = _filter.HasCustomProperties;
        button_Apply.Enabled = _filter.Valid;
      }
    }


    void progress_ProgressChanged(object sender, ProgressStatus.ProgressChangedArgs e)
    {
      progressBar.Value = (int)progress.Percentage;
    }

    private void button_Apply_Click(object sender, EventArgs e)
    {
      if (!_bHasApplyButton)
        return;

      progress.Percentage = 0.0f;
      progressBar.Visible = true;
      TerrainShape terrain = TerrainEditor.CurrentTerrain;

      // in case this is a heightmap filter...
      IHeightmapFilter hmFilter = _filter as IHeightmapFilter;
      if (hmFilter != null)
      {
        if (!hmFilter.OnStart(terrain.Heightmap, x1,y1,x2,y2))
        {
          string err = _filter.LastError;
          string msg = "An error occurred while running this action.";
          if (!string.IsNullOrEmpty(err))
            msg += "\n\nDetailed message: " + err;

          EditorManager.ShowMessageBox(msg, "Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
          hmFilter.OnFinish();
          return;
        }
        terrain.EngineTerrain.ApplyHeightmapFilter(hmFilter, progress);
        hmFilter.OnFinish();
        // trigger the action that has the backed-up height values
        EditorManager.Actions.Add(new TerrainChangedAction(terrain.EngineTerrain, "Import Heightmap : " + _filter.Name));
      }

      // ...or decoration filter
      IDecorationFilter decoFilter = _filter as IDecorationFilter;
      if (decoFilter != null)
      {
        if (!decoFilter.OnStart())
        {
          string err = decoFilter.LastError;
          string msg = "An error occurred while running this action.";
          if (!string.IsNullOrEmpty(err))
            msg += "\n\nDetailed message: " + err;

          EditorManager.ShowMessageBox(msg, "Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
          decoFilter.OnFinish();
          return;
        }
        terrain.EngineTerrain.ApplyDecorationFilter(decoFilter, progress);
        EditorManager.Actions.Add(new TerrainChangedAction(terrain.EngineTerrain, "Decoration : " + _filter.Name));
        decoFilter.OnFinish();
      }
    }


    /// <summary>
    /// Static function to open the filter dialog with additional sample range
    /// </summary>
    /// <param name="filter"></param>
    /// <param name="x1"></param>
    /// <param name="y1"></param>
    /// <param name="x2"></param>
    /// <param name="y2"></param>
    /// <returns></returns>
    public static bool RunFilter(IHeightmapFilter filter, int x1, int y1, int x2, int y2)
    {
      if (filter == null || TerrainEditor.CurrentTerrain == null || TerrainEditor.CurrentTerrain.EngineTerrain == null)
        return false;

      ApplyHeightmapFilterDlg dlg = new ApplyHeightmapFilterDlg();
      dlg.Filter = filter;
      dlg.x1 = x1;
      dlg.y1 = y1;
      dlg.x2 = x2;
      dlg.y2 = y2;
      return dlg.ShowDialog() == DialogResult.OK;
    }

    /// <summary>
    /// Static function to open the filter dialog
    /// </summary>
    /// <param name="filter"></param>
    /// <returns></returns>
    public static bool RunFilter(IHeightmapFilter filter)
    {
      return RunFilter(filter, 0,0,-1,-1);
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
