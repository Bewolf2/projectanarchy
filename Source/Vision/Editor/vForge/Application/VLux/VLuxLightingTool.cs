/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using CSharpFramework;
using CSharpFramework.Helper;
using CSharpFramework.Scene;
using Editor.Vision;

namespace Editor.VLux
{
  public class VLuxLightingTool : IStaticLightingTool
  {
    #region IStaticLightingTool Members

    public bool RunLighting(bool bShowDlg, ZoneCollection zones, bool bIncludeMainZone)
    {
      bool bExportInfoOnly = false;
      if (bShowDlg)
      {
        VLuxLightingDlg dlg = new VLuxLightingDlg();
        dlg.Settings = (EditorManager.Scene as EditorScene).Settings; // clone settings
        dlg.LightMainPart = bIncludeMainZone;

        if (dlg.ShowDialog() != DialogResult.OK)
          return false;
        // copy back settings:
        (EditorManager.Scene as EditorScene).Settings = dlg.Settings;

        if (!dlg.DoRelighting)
          return true;

        bExportInfoOnly = dlg.ExportInfoOnly;
      }

      // for progress bar
      int iRelevantCount = bIncludeMainZone ? 1 : 0;
      foreach (Zone zone in zones)
        if (zone.RunLighting)
          iRelevantCount++;

      // nothing to light
      if (iRelevantCount == 0)
        return true;

      Cursor oldCursor = EditorManager.MainForm.Cursor;
      EditorManager.MainForm.Cursor = Cursors.WaitCursor;

      VisionLightInfo info = new VisionLightInfo();

      EditorManager.Progress.ShowProgressDialog("Calculate static lighting");
      EditorManager.Progress.Percentage = 0.0f;
      float fPercentageStep = 100.0f / (float)iRelevantCount;

      // collect all infos
      foreach (Layer layer in EditorManager.Scene.Layers)
        layer.GetStaticLightInfo(info);

      // save all info files, for every zone and optionally main zone
      for (int i = 0; i <= zones.Count; i++)
      {
        Zone zone = null;
        if (i == zones.Count)
        {
          if (!bIncludeMainZone)
            continue;
        }
        else
        {
          zone = zones[i];
          if (!zone.RunLighting)
            continue;
        }

        info.FilterByZone(zone);

        // save file that contains static lights
        string basename = "StaticLightingInfo";
        if (zone != null)
        {
          zone.StaticLightInfo = (StaticLightInfo)info.Clone();
          basename += "_" + zone.ZoneName;
        }

        string absfilename = Path.Combine(EditorManager.Scene.LayerDirectoryName, basename + ".XML");
        if (!info.Save(absfilename))
        {
          EditorManager.MainForm.Cursor = oldCursor;
          EditorManager.Progress.HideProgressDialog();
          return false;
        }

        // also, collect static mesh information
        string meshRelFilename = Path.Combine(EditorManager.Scene.RelativeLayerDirectoryName, basename + ".temp");
        string meshAbsFilename = Path.Combine(EditorManager.Scene.LayerDirectoryName, basename + ".temp");
        EditorManager.EngineManager.CreateStaticLightingMeshes(meshAbsFilename, info);
        EditorManager.Progress.Percentage += fPercentageStep;
      }


      if (bExportInfoOnly)
      {
        EditorManager.MainForm.Cursor = oldCursor;
        EditorManager.Progress.HideProgressDialog();
        return true;
      }

      // trigger the event (maybe another before info.Save?)
      EditorManager.TriggerSceneEvent(SceneEventArgs.Action.BeforeStaticLighting, true);

      // put together vLux parameter string
      string projectDir = EditorManager.Project.ProjectDir;
      if (projectDir.EndsWith(@"\"))
        projectDir = projectDir.Remove(projectDir.Length - 1, 1);

      // restart progress bar
      EditorManager.Progress.Percentage = 0.0f;

      // launch vLux n times
      bool bSuccess = true;
      for (int i = 0; i <= zones.Count; i++)
      {
        bool bVLuxSuccess = true;
        Zone zone = null;
        if (i == zones.Count)
        {
          if (!bIncludeMainZone)
            continue;
        }
        else
        {
          zone = zones[i];
          if (!zone.Loaded || !zone.RunLighting)
            continue;
        }

        // save file that contains static lights
        string basename = "StaticLightingInfo";
        if (zone != null)
          basename += "_" + zone.ZoneName;

        string meshLitFilename = Path.Combine(EditorManager.Scene.LayerDirectoryName, basename + ".lit");



        // determine out filename (including relative path) and remove file extension
        string outFilename = EditorManager.Scene.FileName;
        string outFilenameExt = Path.GetExtension(outFilename);
        if (outFilenameExt != null)
          outFilename = outFilename.Remove(outFilename.Length - outFilenameExt.Length, outFilenameExt.Length);
        if (zone != null)
          outFilename += "_" + zone.ZoneName;


        //set edit mode for relevant lighting files
        EditorManager.EngineManager.EditOrAddRCSLightingFiles(Path.Combine(projectDir, outFilename), zone, meshLitFilename, false);

        string paramStr = "\"\""; // no v3d (first parameter just empty quotes)

        paramStr += " -project \"" + projectDir + "\"";               // the project folder
        paramStr += " -tempfolder \"" + EditorManager.Scene.LayerDirectoryName + "\"";    // the relative folder name where the temp files are located
        paramStr += " -out \"" + outFilename + "\""; // output filename
        if (zone != null)
          paramStr += " -zone \"" + zone.ZoneName + "\"";
        //paramStr += " -title \"" + EditorApp.Scene.FileName + "\""; // no window title required in this mode
        paramStr += " -quiet -minimized";

        string lightingCfgFile = VLuxLightingTool.GetLightingCfgFile((EditorManager.Scene as EditorScene).Settings);

        ManagedBase.RCS.GetProvider().EditFile(lightingCfgFile);
        paramStr += " -config \"" + lightingCfgFile + "\"";

        try
        {
          // create a batch file for stand-alone lighting
          string batchName = Path.Combine(EditorManager.Scene.LayerDirectoryName, "Relight_" + basename + ".bat");
          TextWriter txt = new StreamWriter(batchName);
          txt.WriteLine("vLux.exe " + paramStr);
          txt.Close();
        }
        catch (Exception ex)
        {
          EditorManager.DumpException(ex);
        }


        try
        {
          Process vLux = FileHelper.RunExternalTool("vLux", VLuxLightingDlg.vLuxEXEPath, paramStr, true);

          if (vLux == null)
          {
            EditorManager.MainForm.Cursor = oldCursor;
            EditorManager.Progress.HideProgressDialog();
            return false;
          }

          Console.WriteLine(vLux.StandardOutput.ReadToEnd());

          vLux.WaitForExit();
          int iExitCode = vLux.ExitCode; // can we do something with this?
        }
        catch (Exception ex)
        {
          EditorManager.ShowMessageBox("vLux did not complete lighting. Please try again.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
          EditorManager.DumpException(ex);
          bVLuxSuccess = false;
        }

        // wait until vLux is finished...

        EditorManager.Progress.Percentage += fPercentageStep;

        // reload the engine files
        if (bSuccess)
        {
          bVLuxSuccess &= EditorManager.EngineManager.ReloadLightingFiles(outFilename, zone);
          bVLuxSuccess &= EditorManager.EngineManager.ReloadStaticLightingMeshes(zone, meshLitFilename);

          EditorManager.Scene.UpdateView(true);
          EditorManager.Scene.UpdateView(true);

          //add relevant lighting files to RCS
          ManagedBase.RCS.GetProvider().AddFile(lightingCfgFile, false);
          EditorManager.EngineManager.EditOrAddRCSLightingFiles(Path.Combine(projectDir, outFilename), zone, meshLitFilename, true);
        }


        System.Diagnostics.Debug.Assert(bVLuxSuccess, "lighting was not successful");
        bSuccess &= bVLuxSuccess;

        // trigger the event (maybe another before info.Save?)
        EditorManager.OnSceneEvent(new SceneEventArgs(SceneEventArgs.Action.AfterStaticLighting, bSuccess));


      }

      // Update all shader assignments
      IScene.SendShaderResourceChangedEvent(new EventArgs());

      // repaint
      EditorManager.Scene.UpdateView(false);

      EditorManager.MainForm.Cursor = oldCursor;
      EditorManager.Progress.HideProgressDialog();
      return bSuccess;
    }

    public string Name
    {
      get { return "vLux"; }
    }

    public static string GetLightingCfgFile(EditorSceneSettings settings)
    {
      EditorScene scene = EditorManager.Scene as EditorScene;
      if (!settings.UseDefaultvLuxSettingsFile && !string.IsNullOrEmpty(settings.VLuxSettingsFile))
      {
        return Path.Combine(scene.LayerDirectoryName, settings.VLuxSettingsFile);
      }
      else
      {
        return Path.Combine(scene.LayerDirectoryName, "lighting.cfg");
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
