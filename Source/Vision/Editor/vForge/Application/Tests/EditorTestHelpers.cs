/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.IO;
using System.Diagnostics;
using System.Windows.Forms;
using CSharpFramework;
using CSharpFramework.Helper;
using CSharpFramework.Tests;
using ManagedFramework;
using CSharpFramework.Actions;
using CSharpFramework.ActionRecording;
using System.Threading;
using System.Drawing;
using CSharpFramework.Math;
using CSharpFramework.Contexts;

namespace Editor.Tests
{
  /// <summary>
  /// Application specific implementation of the test helper utilities.
  /// See ITestHelper interface for detailed comments of the various members.
  /// You can access the test helpers via the TestManager.Helpers property.
  /// </summary>
  public class TestHelpers : ITestHelpers
  {
    #region ITestHelpers Members

    /// <summary>
    /// see ITestHelpers.ProcessEvents
    /// </summary>
    public void ProcessEvents()
    {
      System.Windows.Forms.Application.DoEvents();
    }

    /// <summary>
    /// see ITestHelpers.TestDataDirExists
    /// </summary>
    public bool TestDataDirExists
    {
      get
      {
        return System.IO.Directory.Exists(TestDataSourceDir);
      }
    }

    /// <summary>
    /// Returns the absolute path where to find the locally copied test data
    /// </summary>
    public string TestDataDir
    {
      get
      {
        string path = System.Windows.Forms.Application.StartupPath;
        return FileHelper.ResolveFilename(Path.Combine(path,@"..\..\..\..\Test\Vision\Editor\vForge\DataCopy"));
        //return TestDataSourceDir;
      }
    }

    /// <summary>
    /// Returns the directory containing the original test data.
    /// </summary>
    public string TestDataSourceDir
    {
      get
      {
        string path = System.Windows.Forms.Application.StartupPath;
        return FileHelper.ResolveFilename(Path.Combine(path, @"..\..\..\..\Test\Vision\Editor\vForge\Data"));
      }
    }

    /// <summary>
    /// see ITestHelpers.TestDirectory
    /// </summary>
    public string TestDirectory
    {
      get
      {
        string combined = Path.Combine(System.Windows.Forms.Application.StartupPath,@"..\..\..\..\Test\Vision\Editor\vForge\DataDir"); 
        combined = FileHelper.ResolveFilename(combined);
        return combined;
      }
    }

    /// <summary>
    /// see ITestHelpers.CreateTestScene
    /// </summary>
    /// <param name="sceneName"></param>
    public void CreateTestScene(string sceneName)
    {
      // create project
      const string projectName = "TestProject";
      EditorProject project = EditorProject.CreateProject( TestDirectory, projectName );
      EditorManager.Project = project;

      // Step 1: copy default files (what customers also get)
      FileHelper.CopyFiles(
        new DirectoryInfo(Path.Combine(EditorManager.AppDataDir, @"NewProjectData")),"*.*",
        new DirectoryInfo(project.ProjectDir),true,false,FileAttributes.Hidden,false);

      // Step 2: copy generic test data for automated tests
      FileHelper.CopyFiles(
        new DirectoryInfo(Path.Combine(EditorManager.AppDataDir, @"..\..\..\..\Test\Vision\Editor\vForge\Data\AutomatedTests")), "*.*",
        new DirectoryInfo(project.ProjectDir), true, false, FileAttributes.Hidden, false);

      // open scene
      bool bResult = project.NewScene( sceneName );
      Debug.Assert( bResult == true );
    }

    /// <summary>
    /// Copies all files in the specified sub-directory to a local dir.
    /// </summary>
    /// <param name="subDir"></param>
    public void CreateLocalCopy(string subDir)
    {
      FileHelper.CopyFiles(
        new DirectoryInfo(Path.Combine(TestManager.Helpers.TestDataSourceDir, subDir)), "*.*", 
        new DirectoryInfo(Path.Combine(TestManager.Helpers.TestDataDir, subDir)), true, true, FileAttributes.Hidden, true);

      // since the checked out files are write protected, remove the write protection in the copy
      Process attribExec = new Process();
      attribExec.StartInfo.FileName = "cmd";
      attribExec.StartInfo.Arguments = "/c attrib /s -r " + Path.Combine(TestManager.Helpers.TestDataDir, subDir) + "\\*.*";
      attribExec.StartInfo.UseShellExecute = false;
      attribExec.StartInfo.CreateNoWindow = true;
      attribExec.StartInfo.RedirectStandardOutput = true;
      attribExec.StartInfo.RedirectStandardError = true;
      try
      {
        attribExec.Start();
        attribExec.WaitForExit();
        if (attribExec.ExitCode != 0)
        {
          //System.Windows.Forms.MessageBox.Show("Executing 'erase /s /q /f '" + TestDirectory + "' did not work. Error Code: " + eraseExec.ExitCode.ToString());
        }
      }
      catch (System.Exception)
      {
        //System.Windows.Forms.MessageBox.Show(ex.Message);
      }

    }

    /// <summary>
    /// see ITestHelpers.CloseTestProject
    /// </summary>
    public void CloseTestProject()
    {
      try
      {
        if (EditorManager.Scene!=null)
          EditorManager.Scene.Close();
        EditorApp.Project.Close();
        EditorManager.Project = null;
        EditorManager.Scene = null;
        ProcessEvents();

        if ( Directory.Exists(TestDirectory) )
        {
  
          Process eraseExec = new Process();
          eraseExec.StartInfo.FileName = "cmd";
          eraseExec.StartInfo.Arguments = "/c erase /s /q /f " + TestDirectory;
          eraseExec.StartInfo.UseShellExecute = false;
          eraseExec.StartInfo.CreateNoWindow = true;
          try
          {
            eraseExec.Start();
            eraseExec.WaitForExit();
            if (eraseExec.ExitCode != 0)
            {
              //System.Windows.Forms.MessageBox.Show("Executing 'erase /s /q /f '" + TestDirectory + "' did not work. Error Code: " + eraseExec.ExitCode.ToString());
            }
          }
          catch (System.Exception)
          {
            //System.Windows.Forms.MessageBox.Show(ex.Message);
          }
          Directory.Delete( TestDirectory, true );
        }
      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex,true);
      }    
    }

    public void OpenSceneFromFile(string sceneFileName)
    {
      EditorApp.EditorAppDelegates.LoadSceneDelegate(sceneFileName, true);
    }

    public void CloseActiveProject()
    {
      if (EditorManager.Project != null)
        EditorManager.Project.Close();      
    }

    public void LoadExportedScene(string vsceneFile)
    {
      EditorApp.EngineManager.DisableRendering();
      ExportHelper.LoadExportedScene(vsceneFile);
      EditorApp.EngineManager.EnableRendering();
    }

    public void CloseExportedScene()
    {
      ExportHelper.LoadExportedScene(null);
    }

    /// <summary>
    /// Moves the cursor to the new position in x,y.
    /// </summary>
    /// <param name="x">x-coordinate (screen-space)</param>
    /// <param name="y">y-coordinate (screen-space)</param>
    public void CursorMoveTo(int x, int y)
    {
      Cursor.Position = new Point(x, y);
    }

    /// <summary>
    /// Projects a 3D Vector into 2D space and sets the cursor to the new location.
    /// </summary>
    /// <param name="pos">3D-position to project to 2D.</param>
    public void CursorMoveTo3D(Vector3F pos)
    {
      Vector2F viewPos = EditorManager.EngineManager.Project2D(pos.X, pos.Y, pos.Z);
      Cursor.Position = EditorApp.ActiveView.PointToScreen(new Point((int)viewPos.X, (int)viewPos.Y));
    }

    /// <summary>
    /// Sets the buttonstate and calls the OnMouse*() events for the ActiveView.
    /// </summary>
    /// <param name="button">button to press/release.</param>
    /// <param name="buttonState">new state of the button.</param>
    /// <param name="keyModifier">the key modifier to use.</param>
    public void SetButtonState(MouseButtons button, bool buttonPressed, KeyModifier keyModifier)
    {
      Point screenPos = EditorApp.ActiveView.PointToClient(Cursor.Position);
      MouseEventArgs args = new MouseEventArgs(button, 1, screenPos.X, screenPos.Y, 0);

      switch (buttonPressed)
      {
        case true:
          EditorApp.ActiveView.CurrentContext.OnMouseDown(args, keyModifier, screenPos.X, screenPos.Y);
          break;
        case false:
          EditorApp.ActiveView.CurrentContext.OnMouseUp(args, keyModifier, screenPos.X, screenPos.Y);
          break;
      }
    }

    class TestRecordReport : IRecordReport
    {
      #region IRecordReport Members

      public void AddWarning(string text, string detailed)
      {
      }

      public string LastError;
      public string LastErrorDetailed;
      bool _bWantsQuit = false;


      public void AddError(string text, string detailed)
      {
        if (detailed == null) detailed = "";
        LastError = text;
        LastErrorDetailed = detailed;
        _bWantsQuit = true;
      }
      
      IRecordItem _item;

      public IRecordItem CurrentItem
      {
        get { return _item; }
        set { _item = value; }
      }

      public bool WantsQuit
      {
        get { return _bWantsQuit; }
      }

      #endregion
    }

    public void PlaybackSession(string filename)
    {
      CloseActiveProject();

      string absFilename = filename;
      if (!FileHelper.IsAbsolute(absFilename))
        absFilename = Path.Combine(TestDataDir, filename);
        
      string projDir = EditorProject.FindProjectFor(absFilename);
      EditorProject proj = new EditorProject();
      if (projDir == null || !proj.Load(projDir))
        throw new FileNotFoundException("Project file not found for " + absFilename);

      EditorManager.Project = proj;

      RecordSession session = RecordSession.LoadFromFile(proj.MakeRelative(absFilename));
      if (session == null)
        throw new FileNotFoundException("Session file not found", absFilename);

      TestRecordReport report = new TestRecordReport();
      if (!session.Playback(report, true))
        throw new Exception("Record session '" + session.Filename + "' failed.\nItem '" + report.CurrentItem + "' triggered the following error:\n\n" + report.LastError + "\n\n" + report.LastErrorDetailed);

    }

    public void PlaybackSessionsInProject(string projectDir, string subdir)
    {
      CloseActiveProject();

      string absFilename = projectDir;
      if (!FileHelper.IsAbsolute(absFilename))
        absFilename = Path.Combine(TestDataDir, projectDir);

      string projDir = EditorProject.FindProjectFor(Path.Combine(absFilename, "dummy.xyz"));
      EditorProject proj = new EditorProject();
      if (projDir == null || !proj.Load(projDir))
        throw new FileNotFoundException("Project file not found for " + absFilename);

      EditorManager.Project = proj;
              
      DirectoryInfo folder = new DirectoryInfo(proj.MakeAbsolute(subdir));
      FileInfo[] files = folder.GetFiles("*.record");
      foreach (FileInfo file in files)
      {
        RecordSession session = RecordSession.LoadFromFile(Path.Combine(subdir, file.Name));
        if (session == null)
          throw new FileNotFoundException("Session file not found", absFilename);

        TestRecordReport report = new TestRecordReport();
        if (!session.Playback(report, true))
          throw new Exception("Record session '" + session.Filename + "' failed.\nItem '" + report.CurrentItem + "' triggered the following error:\n\n" + report.LastError + "\n\n" + report.LastErrorDetailed);
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
