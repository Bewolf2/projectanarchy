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
using System.IO;
using System.Windows.Forms;
using CSharpFramework;
using CSharpFramework.Scene;
using CSharpFramework.Dialogs;

namespace Editor.Dialogs
{
  /// <summary>
  /// A dialog used to export all scene files in a given directory.
  /// </summary>
  public partial class BatchExportDlg : Form
  {
    public enum State_e {None, Running, Cancelling};

    // Current state
    private State_e _state;

    // Total number of directories found to scan for scene files
    private int _dirsFound;

    // Number of directories scanned so far
    private int _dirsScanned;

    // Number of scenes found and processed
    private int _scenesFound;
    private int _scenesProcessed;

    // Queue of unprocessed directories and files
    private List<string> _dirsToScan;
    private List<string> _scenesToProcess;

    // Background worker to scan directories
    private BackgroundWorker _worker;

    // Current scene file name
    private string _currentScene = null;

    // The selected directory
    public string SelectedPath = "";

    public BatchExportDlg()
    {
      InitializeComponent();
    }

    /// <summary>
    /// The current state of the export process.
    /// </summary>
    private State_e State
    {
      get { return _state; }
      set
      {
        if (_state != value)
        {
          _state = value;
          UpdateUI();
        }
      }
    }

    private void BatchExportDlg_Load(object sender, EventArgs e)
    {
      textBoxDirectory.Text = SelectedPath;

      EditorManager.Progress.ProgressChanged += new ProgressStatus.ProgressChangedEventHandler(this.on_ProgressChanged);
      EditorManager.Progress.StatusStringChanged += new ProgressStatus.ProgressChangedEventHandler(this.on_ProgressChanged);
    }

    //Fixme
    private void BatchExportDlg_UnLoad(object sender, EventArgs e)
    {
      EditorManager.Progress.ProgressChanged -= new ProgressStatus.ProgressChangedEventHandler(this.on_ProgressChanged);
      EditorManager.Progress.StatusStringChanged -= new ProgressStatus.ProgressChangedEventHandler(this.on_ProgressChanged);
    }

    private void buttonDirectory_Click(object sender, EventArgs e)
    {
      // Prompt the user to select a folder
      FolderBrowserDialog dialog = new FolderBrowserDialog();
      dialog.SelectedPath = this.SelectedPath;
      dialog.ShowNewFolderButton = false;

      if (dialog.ShowDialog() != DialogResult.OK)
        return;

      // Set the folder
      textBoxDirectory.Text = dialog.SelectedPath;
      this.SelectedPath = dialog.SelectedPath;
    }

    private void buttonCancel_Click(object sender, EventArgs e)
    {
      // If we have a canceable subprocess running, notify it
      if(EditorManager.Progress.IsAbortable)
        EditorManager.Progress.WantsAbort = true;

      // If we're doing something, set to cancelling, otherwise close immediately
      if (_state != State_e.None)
      {
        State = State_e.Cancelling;
      }
      else
      {
        this.Close();
      }
    }

    private void buttonRun_Click(object sender, EventArgs e)
    {
      // Clear all info
      _dirsToScan = new List<string>();
      _scenesToProcess = new List<string>();

      _dirsScanned = 0;
      _dirsFound = 1;

      _dirsToScan.Add(SelectedPath);

      _scenesFound = 0;
      _scenesProcessed = 0;

      State = State_e.Running;

      // Scan for all scene files with a BackgroundWorker
      EditorManager.Progress.Percentage = 0;
      EditorManager.Progress.IsAbortable = true;
      EditorManager.Progress.ProgressDialogEnabled = false;
      EditorManager.Progress.StatusString = "Scanning for scene files...";

      _worker = new BackgroundWorker();
      _worker.WorkerReportsProgress = true;
      _worker.DoWork += new DoWorkEventHandler(this.on_DoDirScan);
      _worker.RunWorkerCompleted += new RunWorkerCompletedEventHandler(this.on_DirScanCompleted);
      _worker.ProgressChanged += new ProgressChangedEventHandler(this.on_DirScanProgress);
      _worker.RunWorkerAsync();

      // ... Continued in on_DirScanCompleted
    }

    /// <summary>
    /// Exports all found scene files
    /// </summary>
    private void DoExport()
    {
      // Enter the testing mode (since we don't want any user dialogs etc.)
      TestManager.IsRunning = true;

      while (_scenesToProcess.Count > 0 && _state != State_e.Cancelling)
      {
        // Get one scene from the queue
        _currentScene = _scenesToProcess[0];
        _scenesToProcess.RemoveAt(0);

        textBoxLog.Text += string.Format("Processing {0}: ", _currentScene);

        // Load the scene
        TestManager.Helpers.OpenSceneFromFile(_currentScene);
        TestManager.Helpers.ProcessEvents();

        // Force the folder type to version controlled, since we didn't have a chance to when loading the scene
        EditorManager.Project.FolderType = IProject.FolderType_e.RCS;
        EditorManager.Project.UpdateRCSStatus(true);
        TestManager.Helpers.ProcessEvents();

        // Actual export unless cancel has been clicked while loading
        if (_state != State_e.Cancelling)
        {
          // Check if the exported scene file name is correct
          string exportedSceneFile = Path.GetFileName(EditorManager.Scene.CurrentExportProfile.ExportPath);
          string defaultExportName = Path.GetFileName(Path.ChangeExtension(_currentScene, CSharpFramework.Scene.SceneExportProfile.FILE_EXTENSION_EXPORT));

          bool showExportDialog = false;

          if (exportedSceneFile != defaultExportName)
          {
            textBoxLog.Text += string.Format("\r\nScene name mismatch: {0} instead of {1}: ", exportedSceneFile, defaultExportName);

            if (radioMismatch_Dialog.Checked)
            {
              showExportDialog = true;
            }
            else if (radioMismatch_Revert.Checked)
            {
              string subDir = Path.GetDirectoryName(EditorManager.Scene.CurrentExportProfile.ExportPath);
              EditorManager.Scene.CurrentExportProfile.ExportPath = Path.Combine(subDir, defaultExportName);
              textBoxLog.Text += string.Format("Reverted.\r\n");
            }
            else
            {
              textBoxLog.Text += string.Format("Ignoring.\r\n");
            }
          }

          // Always export all zones
          foreach (Zone zone in EditorManager.Scene.Zones)
          {
            zone.Export = true;
          }

          if (checkBoxMoveToCamera.Checked)
          {
            // TODO: Move camera to a sensible position
          }

          EditorManager.Progress.Percentage = 0;

          // Export the scene
          EditorManager.GUI.UIUpdateLock++;
          EditorManager.Scene.ExportScene(null, showExportDialog);
          EditorManager.GUI.UIUpdateLock--;

          if (showExportDialog)
            textBoxLog.Text += string.Format("Manually set to {0}.\r\n", Path.GetFileName(EditorManager.Scene.CurrentExportProfile.ExportPath));
          textBoxLog.Text += string.Format("Export done. ");
          TestManager.Helpers.ProcessEvents();

          // Resave scene
          if (checkBoxResaveScene.Checked)
          {
            EditorManager.Scene.Save();
            textBoxLog.Text += string.Format("Scene file resaved. ");
            TestManager.Helpers.ProcessEvents();
          }

          textBoxLog.Text += string.Format("\r\n");
        }

        // Close the project
        TestManager.Helpers.CloseActiveProject();
        TestManager.Helpers.ProcessEvents();

        _scenesProcessed++;
      }

      if (State == State_e.Cancelling)
      {
        textBoxLog.Text += string.Format("\r\nAborted.\r\n");
      }

      // Reset state
      _currentScene = null;
      State = State_e.None;
      EditorManager.Progress.StatusString = "";
      EditorManager.Progress.Percentage = 0;
      EditorManager.Progress.ProgressDialogEnabled = true;

      // Go back to normal operation mode
      TestManager.IsRunning = false;
    }

    /// <summary>
    /// Update UI enable status etc.
    /// </summary>
    private void UpdateUI()
    {
      switch(_state)
      {
        case State_e.None:
          textBoxDirectory.Enabled = true;
          buttonDirectory.Enabled = true;

          groupBoxSettings.Enabled = true;

          buttonRun.Enabled = true;

          buttonCancel.Enabled = true;
          buttonCancel.Text = "Cancel";
          break;

        case State_e.Running:
          textBoxDirectory.Enabled = false;
          buttonDirectory.Enabled = false;

          groupBoxSettings.Enabled = false;

          buttonRun.Enabled = false;

          buttonCancel.Enabled = true;
          buttonCancel.Text = "Cancel";
          break;

        case State_e.Cancelling:
          textBoxDirectory.Enabled = false;
          buttonDirectory.Enabled = false;

          groupBoxSettings.Enabled = false;

          buttonRun.Enabled = false;

          buttonCancel.Enabled = false;
          buttonCancel.Text = "Cancelling...";
          break;
      }
    }

    #region Directory scan background worker

    private void on_DirScanCompleted(object sender, RunWorkerCompletedEventArgs e)
    {
      _worker.DoWork -= new DoWorkEventHandler(this.on_DoDirScan);
      _worker.RunWorkerCompleted -= new RunWorkerCompletedEventHandler(this.on_DirScanCompleted);
      _worker.ProgressChanged -= new ProgressChangedEventHandler(this.on_DirScanProgress);

      textBoxLog.Text = "";

      EditorManager.Progress.HideProgressDialog();
      EditorManager.Progress.Reset();

      // Aborted?
      if (_dirsScanned != _dirsFound)
      {
        State = State_e.None;
        return;
      }

      _scenesFound = _scenesToProcess.Count;

      DoExport();
    }

    private void on_DirScanProgress(object sender, ProgressChangedEventArgs e)
    {
      EditorManager.Progress.Percentage = e.ProgressPercentage;
    }

    private void on_DoDirScan(object sender, DoWorkEventArgs e)
    {
      EditorManager.Progress.WantsAbort = false;

      while (_dirsToScan.Count > 0)
      {
        string dir = _dirsToScan[0];
        _dirsToScan.RemoveAt(0);

        try
        {
          string[] subDirs = Directory.GetDirectories(dir);
          _dirsFound += subDirs.Length;
          _dirsToScan.AddRange(subDirs);
          _scenesToProcess.AddRange(Directory.GetFiles(dir, "*.scene"));
        }
        catch (System.IO.IOException)
        {
          // Ignore
        }

        _dirsScanned++;

        _worker.ReportProgress((int)(_dirsScanned * 100.0 / _dirsFound));

        if (EditorManager.Progress.WantsAbort)
          return;
      }
    }

#endregion

#region ProgressStatus event handlers

    private void on_ProgressChanged(object sender, ProgressStatus.ProgressChangedArgs e)
    {
       progressBar.Value = (int)e.Progress.Percentage;

       if (_currentScene != null)
         statusString.Text = string.Format("{0} ({1}/{2}): {3}",
           Path.GetFileName(_currentScene), _scenesProcessed + 1, _scenesFound, e.Progress.StatusString);
       else
         statusString.Text = e.Progress.StatusString;
       Refresh();
       TestManager.Helpers.ProcessEvents();
    }

#endregion

    private void textBoxDirectory_TextChanged(object sender, EventArgs e)
    {
      SelectedPath = textBoxDirectory.Text;
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
