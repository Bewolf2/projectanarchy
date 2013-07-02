/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Diagnostics;
using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using System.Windows.Forms;
using System.Xml;
using CSharpFramework;
using CSharpFramework.Actions;
using CSharpFramework.Clipboard;
using CSharpFramework.Dialogs;
using CSharpFramework.DynamicProperties;
using CSharpFramework.Helper;
using CSharpFramework.Math;
using CSharpFramework.Scene;
using CSharpFramework.Serialization;
using CSharpFramework.Shapes;
using CSharpFramework.Visitors;
using Editor.Actions;
using Editor.Dialogs;
using Editor.Vision;
using ManagedFramework;
using CSharpFramework.AssetManagement;

namespace Editor
{

  /// <summary>
  /// A game specific implementation of the IScene class
  /// </summary>
  [Serializable]
  public class EditorScene : IScene
  {

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="_Project">Owner project</param>
    /// <param name="relFilename">Project relative filename of the scene to initialize the settings.ExportPath with. Can be null.</param>
    public EditorScene(IProject _Project, string relFilename) : base(_Project)
    {
      this.FileName = relFilename;
      _sceneSettings = new EditorSceneSettings(this);
    }

    #region Description Dialog

    // Description dialog associated with this scene
    private ShowDescriptionDlg _descriptionDialog = null;

    /// <summary>
    /// Used to handle the "ShowAgain" option when the description dialog of the scene is closed
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void _OnDescriptionDialogClosed(object sender, FormClosedEventArgs e)
    {
      if (!_descriptionDialog.ShowAgain)
      {
        Settings.ShowDescriptionAtStartup = false;
        Dirty = true;
      }

      _descriptionDialog.FormClosed -= new FormClosedEventHandler(_OnDescriptionDialogClosed);

      // force making a fresh one next time
      _descriptionDialog = null;

    }

    #endregion

    #region Scene settings

    EditorSceneSettings _sceneSettings = null;
    SceneExportProfile _currentProfile = null;

    /// <summary>
    /// Gets or sets the scene relevant settings
    /// </summary>
    public EditorSceneSettings Settings
    {
      get {return _sceneSettings;}
      set 
      {
        if (_sceneSettings == value)
          return;
        _sceneSettings = value;
      }
    }


    public override SceneExportProfile CurrentExportProfile
    {
      get 
      {
        if (_currentProfile == null)
          _currentProfile = new SceneExportProfile(this, null);
        return _currentProfile; 
      }
      set
      {
        if (_currentProfile != value)
          _currentProfile = value;
      }
    }


    /// <summary>
    /// Shows the description dialog after scene loading
    /// </summary>
    public void ShowDescriptionAtStartup()
    {
      // show the description dialog
      if (Settings!=null && Settings.ShowDescriptionAtStartup)
      {
        ShowDescription();
      }
    }


    /// <summary>
    /// Shows the description dialog (e.g. after scene loading)
    /// </summary>
    public void ShowDescription()
    {
      Layer mainLayer = V3DLayer;
      // show the description dialog
      if (mainLayer!=null && mainLayer.HasDescription)
      {
        if (_descriptionDialog == null || _descriptionDialog.IsDisposed)
        {
          _descriptionDialog = new ShowDescriptionDlg();
          _descriptionDialog.FormClosed += new FormClosedEventHandler(_OnDescriptionDialogClosed);
        }
        _descriptionDialog.HeaderText = "Scene description.\nTo change the description, select the main layer and edit the description property.";
        _descriptionDialog.DescriptionText = mainLayer.Description;
        _descriptionDialog.ShowAgain = true;
        _descriptionDialog.Show(EditorManager.MainForm);
      }
    }

    #endregion

    #region IScene Members (and load)

    /// <summary>
    /// Implements the abstract IScene function to get or set the current shape spawn position
    /// </summary>
    public override Vector3F CurrentShapeSpawnPosition
    {
      get {return Settings.ShapeOrigin;}
      set {Settings.ShapeOrigin=value;}
    }

    /// <summary>
    /// Saves the .user settings file
    /// </summary>
    /// <param name="filename"></param>
    /// <returns></returns>
    protected bool SaveUserSettingsFile(string filename)
    {
      // get the latest status
      //Settings.ExportedLayersFromScene(this);  //REMOVED?

      // this file should not be but under version control. 
      // But if the user manually added the file under version control, 
      // we must make sure to disable the write protection by calling edit file
      ManagedBase.RCS.GetProvider().EditFile(filename);
      BinaryFormatter fmt = SerializationHelper.BINARY_FORMATTER;

      try
      {
        FileStream fs = File.Create(filename);
        if (fs == null) return false;
        fmt.Serialize(fs, Settings);
        fs.Close();
      }
      catch( System.UnauthorizedAccessException )
      {
        EditorManager.ShowMessageBox("Unable to write to the local user layers file: " + filename + ". Please make sure the file is writable (and not checked into source control).", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        return false;
      }
      catch( Exception ex )
      {
        EditorManager.DumpException( ex );
        EditorManager.ShowMessageBox("Error writing to local user layers file: " + filename, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        return false;
      }

      return true;
    }

    /// <summary>
    /// Tries to load the .user settings file
    /// </summary>
    /// <param name="filename"></param>
    /// <returns></returns>
    protected bool LoadUserSettingsFile(string filename)
    {
      try
      {
        if (!System.IO.File.Exists(filename))
          return false;

        BinaryFormatter fmt = SerializationHelper.BINARY_FORMATTER;
        FileStream fs = new FileStream(filename, FileMode.Open, FileAccess.Read);
        Settings = (EditorSceneSettings)fmt.Deserialize(fs);
        fs.Close();
      } 
      catch (Exception ex)
      {
        EditorManager.DumpException(ex);
        return false;
      }
      return true;
    }


    /// <summary>
    /// Overridden scene save function
    /// </summary>
    /// <returns></returns>
    public override bool Save()
    {
      return SaveAs(FileName);
    }


    bool _bSceneLoadingInProgress = false;

    /// <summary>
    /// Returns true while loading the scene
    /// </summary>
    public bool SceneLoadingInProgress
    {
      get { return _bSceneLoadingInProgress; }
    }


    /// <summary>
    /// Actually load the scene
    /// </summary>
    /// <param name="relFileName"></param>
    /// <returns></returns>
    public bool Load(string relFileName)
    {
      _bSceneLoadingInProgress = true;
      EditorManager.Progress.StatusString = "Load manifest file";
      FileName = relFileName;
      string absFileName = AbsoluteFileName;
      EditorManager.LoadManifestFile(Project,Path.Combine(LayerDirectoryName,"vForgeManifest.txt"));

      // Check for old scene file format, and refuse to load it.
      try
      {
        FileStream fs = new FileStream( absFileName , FileMode.Open, FileAccess.Read);
        long iLen = fs.Length;
        fs.Close();
        if (iLen>0) // this is an old file
        {
          String error = String.Format("Loading aborted.\nThe file format of this scene is not supported by recent versions of vForge.\nTo migrate it, please open and then re-save this file in vForge prior to version 2012.3.");
          EditorManager.ShowMessageBox(error, "Scene loading error", MessageBoxButtons.OK, MessageBoxIcon.Error);
          return false;
        }
      } 
      catch (Exception ex)
      {
        EditorManager.DumpException(ex);
        EditorManager.ShowMessageBox("An exception occurred while loading scene file '" + relFileName + "'.\nPlease check whether the file is there and it is not write protected.\n\nDetailed Message:\n" + ex.ToString(),
          "Scene loading error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        _bSceneLoadingInProgress = false;
        return false; // the file isn't there at all or write protected?
      }
      EditorManager.Progress.Percentage = 15.0f; // loaded manifest
      EditorManager.Progress.StatusString = "Load layer and zone files";

      // Load export profile. If the return value is null, CurrentExportProfile creates a new instance
      _currentProfile = SceneExportProfile.LoadProfile(this, null, false);
      if (_currentProfile == null)
        EditorSceneSettings.PATCH_PROFILE = CurrentExportProfile; // patch into profile if no dedicated file has been loaded

      // Load user specific data
      string directoryName = LayerDirectoryName;
      string userFilename = Path.Combine(directoryName, EditorManager.UserSettingsFilename);
      
      // first try user specific file:
      if (!LoadUserSettingsFile(userFilename))
      {
        // alternatively try default.user file
        userFilename = Path.Combine(directoryName, "default.user");
        LoadUserSettingsFile(userFilename);
      }
      EditorSceneSettings.PATCH_PROFILE = null;

      if (!string.IsNullOrEmpty(Settings.ExportProfileName))
      {
        SceneExportProfile profile = SceneExportProfile.LoadProfile(this, Settings.ExportProfileName, false);
        if (profile != null)
          _currentProfile = profile;
      }

      EditorManager.Progress.SetRange(15.0f, 20.0f); // remap GatherLayers to range 15..20 
      Zones.Clear();
      if (!GatherZones(Zones))
      {
        _bSceneLoadingInProgress = false;
        return false;
      }

      Layers.Clear();

      // load the layer files...
      LayerCollection newLayers = new LayerCollection();
      if (!GatherLayers(newLayers,EditorManager.Progress))
      {
        _bSceneLoadingInProgress = false;
        Layers.Clear();
        return false;
      }

      // Take the SortingOrder value
      newLayers.Sort();

      // create a dictionary for fast name lookup
      Dictionary<string, Layer> layerNameDict = new Dictionary<string, Layer>(newLayers.Count);
      foreach (Layer layer in newLayers)
        layerNameDict.Add(layer.LayerFilename, layer);

      if (!Zones.MatchupLayerNames(newLayers, layerNameDict))
      {
        _bSceneLoadingInProgress = false;
        return false;
      }

      // Add layers to the scene (do not lock them)
      foreach (Layer layer in newLayers)
        AddLayer(layer, false, false, false);

      // If project setting is to lock all layers on scene load open the layer lock dialog.
      // Otherwise all layers will be locked automatically (when not locked already by any other user).
      if (!TestManager.IsRunning && !EditorManager.SilentMode && Project.LayerLocking == EditorProject.LayerLocking_e.AskOnSceneOpen)
      {
        // Open layer lock dialog where user can select the layers to lock
        LayerLockDlg dlg = new LayerLockDlg();
        dlg.Scene = this;

        // Dialog has only an OK button as canceling the operation doesn't make much sense
        dlg.ShowDialog();
      }
      else
      {
        foreach (Layer layer in Layers)
          layer.TryLock(this, false);
      }

      if (!newLayers.CheckLayerUniqueIDs())
      {
        EditorManager.ShowMessageBox("Layer IDs in this scene are not unique. Please contact support", "Layer ID conflict", MessageBoxButtons.OK, MessageBoxIcon.Warning);
      }

      // check UID consistency:
      bool bForceDirty = false;
      /*
      RepairShapeIDVisitor repair = new RepairShapeIDVisitor();
      if (!repair.CheckScene(this))
      {
        bForceDirty = true;
        EditorManager.ShowMessageBox("The scene contained overlapping unique IDs.\nThis has been repaired.", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        repair.RepairScene(this,false);
      }
      */
      // fixup exported layer names and put the Export flag into the layers own flag
      SceneExportProfile exportprofile = CurrentExportProfile;
      exportprofile.FixupLayerNames();
      if (exportprofile.LoadedFromFile) // preserve the flags loaded from Layer files otherwise
        exportprofile.ExportedLayersToScene();

      _iSceneversion = SCENE_VERSION_CURRENT; // however, scene version does not make much sense anymore
      m_bDirty = bForceDirty;

      EditorManager.Progress.SetRange(0.0f, 100.0f); // set back sub-range
      EditorManager.Progress.Percentage = 20.0f; // loaded layer files
      _bSceneLoadingInProgress = false;

      return true;
    }

    /// <summary>
    /// Overidden function; additionally reloads the custom lighting file
    /// </summary>
    public override void OnCreateAllEngineInstances(ProgressStatus progress)
    {
      string meshAbsFilename = Path.Combine(this.LayerDirectoryName, "StaticLightingInfo.lit");
      EditorManager.EngineManager.InitStaticLightingMode(meshAbsFilename);

      // load global lightgrid first
      EditorManager.EngineManager.ReloadLightingFiles(FileName, null); // removes extension for us...

      // scale down range (20%..90%)
      if (progress != null)
        progress.SetRange(progress.Percentage, 90.0f);

      base.OnCreateAllEngineInstances(progress);

      if (progress != null)
      {
        progress.SetRange(0.0f, 100.0f); // full range again
        progress.StatusString = "Reload lighting files";
      }

      // reload lighting file
      EditorManager.EngineManager.ReloadStaticLightingMeshes(null, meshAbsFilename);
      string basename = FileNameNoExt + "_";

      // also for every zone in the scene
      foreach (Zone zone in Zones)
      {
        if (!zone.Loaded)
          continue;

        meshAbsFilename = Path.Combine(this.LayerDirectoryName, "StaticLightingInfo_"+zone.ZoneName+".lit");
        EditorManager.EngineManager.ReloadStaticLightingMeshes(zone, meshAbsFilename);
        EditorManager.EngineManager.ReloadLightingFiles(basename+zone.ZoneName, zone);
      }


      // trigger the event
      EditorManager.TriggerSceneEvent(SceneEventArgs.Action.StaticLightingLoaded, true);

      if (progress != null)
      {
        progress.StatusString = "Finalizing...";
        progress.Percentage = 98.0f;
      }

    }


    override public bool LoadLayers(LayerCollection newLayers, ProgressStatus progress, FileInfo[] files)
    {
      bool bOK = true;
      float fPercentage = 0.0f;
      string layerDir = LayerDirectoryName;
      foreach (FileInfo fileInfo in files)
      {
        fPercentage += 100.0f / (float)files.Length;
        if (fileInfo == null || (fileInfo.Attributes & FileAttributes.Directory) != 0) // file info can be null
          continue;
        if (string.Compare(fileInfo.Extension, IScene.LayerFileExtension, true) != 0)
          continue;

        string layerFile = fileInfo.Name;
        if (!fileInfo.FullName.StartsWith(LayerDirectoryName)) // assume it is a layer reference
          layerFile = this.Project.MakeRelative(fileInfo.FullName);

        Layer layer = Layers.GetLayerByFilename(layerFile);
        Layer.LayerFileStatus_e newState = Layer.LayerFileStatus_e.NewLayer;
        if (layer != null) // already there
        {
          bool bModified = layer.LastModified != fileInfo.LastWriteTime;
          System.Diagnostics.Debug.Assert(!layer.OwnsLock || !bModified);
          if (bModified && !layer.OwnsLock)
          {
            newState = Layer.LayerFileStatus_e.Modified;
          }
          else
          {
            // don't add the non-modified layer to the list
            layer.FileStatus = Layer.LayerFileStatus_e.NotModified;
            continue;
          }
        }

        BinaryFormatter fmt = SerializationHelper.BINARY_FORMATTER;
        try
        {
          // open the layer in read-only mode
          FileStream fs = new FileStream(fileInfo.FullName, FileMode.Open, FileAccess.Read);
          layer = (Layer)fmt.Deserialize(fs);
          fs.Close();

          // make sure there is only one layer of type V3DLayer [#18824]
          if (layer is V3DLayer)
          {
            foreach (Layer other in newLayers)
              if (other is V3DLayer)
                throw new Exception("The Layer directory contains more than one Layer of type 'Main Layer'. E.g. '" + layer.LayerFilename + "' and '" + other.LayerFilename + "'.\n\nIgnoring '" + layer.LayerFilename + "'");
          }
        }
        catch (Exception ex)
        {
          EditorManager.DumpException(ex);
          EditorManager.ShowMessageBox("An exception occurred while loading layer '" + fileInfo.Name + "'\n\nDetailed Message:\n" + ex.ToString(),
            "Layer loading error", MessageBoxButtons.OK, MessageBoxIcon.Error);
          continue;
        }

        if (fileInfo.FullName.StartsWith(layerDir))
          layer.SetLayerFileNameInternal(fileInfo.Name);
        else
        {
          // this layer is a reference
          string name = Project.MakeRelative(fileInfo.FullName);
          layer.SetLayerFileNameInternal(name);
          layer.IsReference = true;
        }

        layer.UpdateLastModified(fileInfo);
        layer.FileStatus = newState;
        layer.UpdateReadOnlyState(fileInfo);
        newLayers.Add(layer);
        if (progress != null)
          progress.Percentage = fPercentage;
      }

      return bOK;
    }


    public bool ImportLayerReferences(StringCollection externalLayers, StringCollection errorList)
    {
      _bSceneLoadingInProgress = true;
      bool bResult = true;
      if (errorList == null)
        errorList = new StringCollection();
      try
      {
        List<FileInfo> layerFiles = new List<FileInfo>();
        List<FileInfo> zoneFiles = new List<FileInfo>();
        LayerCollection newLayers = new LayerCollection();
        ZoneCollection newZones = new ZoneCollection();
        foreach (string name in externalLayers)
        {
          string absname = name;
          if (!FileHelper.IsAbsolute(absname))
            absname = Project.MakeAbsolute(name);
          string relname = Project.MakeRelative(absname);
          bool bIsLayer = relname.EndsWith(IScene.LayerFileExtension);
          bool bIsZone = relname.EndsWith(IScene.ZoneFileExtension);

          if (bIsLayer)
          {
            if (Layers.GetLayerByFilename(absname, null) != null)
            {
              errorList.Add(relname + " : Already exists in the scene");
              break;
            }
          }
          else if (bIsZone)
          {
            if (Zones.GetZoneByFilename(absname, null) != null)
            {
              errorList.Add(relname + " : Already exists in the scene");
              continue;
            }
          }

          if (File.Exists(absname))
          {
            if (bIsLayer)
              layerFiles.Add(new FileInfo(absname));
            if (bIsZone)
              zoneFiles.Add(new FileInfo(absname));
          }
          else
            errorList.Add(relname + " : File not found");
        }

        LoadLayers(newLayers, null, layerFiles.ToArray());
        // LoadZones(newZones, null, zoneFiles.ToArray()); // not supported yet

        // Add layers to the scene (do not lock them)
        foreach (Layer layer in newLayers)
        {
          if (layer is V3DLayer)
          {
            errorList.Add(layer.LayerFilename + " : External main layers cannot be added");
            continue;
          }

          EditorManager.Actions.Add(new AddLayerAction(layer, false));
        }

        foreach (Zone zone in newZones)
          EditorManager.Actions.Add(new AddZoneAction(zone, false));

      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex, true);
        bResult = false;
      }

      _bSceneLoadingInProgress = false;

      WriteReferenceFile();
      return bResult;
    }


    /// <summary>
    /// Write out a current text file version of the referenced layers, based on layers in the scene
    /// </summary>
    public void WriteReferenceFile()
    {
      if (SceneLoadingInProgress)
        return;
      try
      {
        string absFile = this.LayerReferenceFile;
        ManagedBase.RCS.GetProvider().EditFile(absFile);
        TextWriter writer = new StreamWriter(this.LayerReferenceFile);
        foreach (Layer layer in this.Layers)
          if (layer.IsReference)
            writer.WriteLine(layer.LayerFilename);
        writer.Close();
        ManagedBase.RCS.GetProvider().AddFile(absFile, false /* Text file */);

      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex, true);
      }
    }


    public bool ImportLayers(LayerCollection newLayers, ProgressStatus progress, FileInfo[] files)
    { 
      bool bOK = true;
      float fPercentage = 0.0f;
      foreach (FileInfo fileInfo in files)
      {
        fPercentage += 100.0f / (float)files.Length;
        if (fileInfo == null || (fileInfo.Attributes & FileAttributes.Directory) != 0) // file info can be null
          continue;
        if (string.Compare(fileInfo.Extension, IScene.LayerImportExtension, true) != 0)
          continue;

        try
        {
          string absfilename = fileInfo.FullName;
          string filename = fileInfo.Name;
          XmlTextReader xmlReader = new XmlTextReader(absfilename);
          xmlReader.WhitespaceHandling = WhitespaceHandling.None;

          XmlDocument doc = new XmlDocument();
          doc.Load(xmlReader);
          if (doc.DocumentElement == null)
            throw new Exception("XML does not contain root node");
          IEnumerator nodes = doc.DocumentElement.GetEnumerator();
          while (nodes.MoveNext())
          {
            XmlElement node = nodes.Current as XmlElement;
            if (node == null || node.Name != "layer")
              continue;

            string classname = node.GetAttribute("class");
            string name = node.GetAttribute("name");
            if (string.IsNullOrEmpty(name))
              name = Path.GetFileNameWithoutExtension(filename);

            Type t = EditorManager.ShapeFactory.GetTypeByName(classname, typeof(Layer), false);
            if (t == null)
              t = typeof(Layer);
            Layer layer = Activator.CreateInstance(t, new object[1] { name }) as Layer;
            if (layer == null)
              throw new Exception("Could not instantiate Layer");
            layer.SetLayerFileNameInternal(filename); // same filename but will replace extension

            newLayers.Add(layer);

            // apply property/value pairs to layer
            SerializationHelper.ApplyXMLProperties(node, layer, false);

            // parse for shapes
            IEnumerator propNodes = node.GetEnumerator();
            while (propNodes.MoveNext())
            {
              XmlElement propNode = propNodes.Current as XmlElement;
              if (propNode == null)
                continue;
              if (propNode.Name == "shapes")
              {
                // use prefab functionality to parse it
                PrefabDesc dummyPrefab = new PrefabDesc(null);
                ShapeCollection shapes = dummyPrefab.CreateInstances(propNode, true, true);
                if (shapes != null)
                {
                  layer.Root.SetChildCollectionInternal(shapes);
                  foreach (ShapeBase shape in shapes)
                  {
                    shape.SetParentInternal(layer.Root);
                    shape.SetParentLayerInternal(layer);
                  }
                }
                continue;
              }

            }
          }
        }
        catch (Exception ex)
        {
          EditorManager.DumpException(ex, false);
          return false;
        }

      }
      return bOK;
    }


    void FilterLayerFiles(FileInfo[] files, FileInfo[] ignoreFiles)
    {
      // sort out the layers that are bound to zones that suppor full unloading
      foreach (Zone zone in Zones)
      {
        if (zone.Loaded || zone.LayerFilenames == null || !zone.FullZoneUnload)
          continue;
        string[] ignoreNames = zone.LayerFilenames;
        foreach (string ignore in ignoreNames)
        {
          string ignoreName = Path.ChangeExtension(ignore, null);
          for (int i = 0; i < files.Length; i++)
          {
            if (files[i] == null)
              continue;
            string other = Path.ChangeExtension(files[i].Name, null); // have to compare pure names,because .Layer and .LayerXML might be mixed
            if (string.Compare(other, ignoreName, true) == 0)
            {
              files[i] = null;
              break;
            }
          }
        }
      }

      // also filter out layer files that have been loaded in another batch (e.g. .Layer is preferred over LayerXML)
      if (ignoreFiles != null)
      {
        foreach (FileInfo ignore in ignoreFiles)
        {
          if (ignore == null)
            continue;
          string ignoreName = Path.ChangeExtension(ignore.Name, null);
          for (int i = 0; i < files.Length; i++)
            if (files[i] != null && string.Compare(Path.ChangeExtension(files[i].Name, null), ignoreName, true) == 0)
            {
              files[i] = null;
              break;
            }

        }
      }
    }


    /// <summary>
    /// Returns the absolute filename of the text file that lists the external layer references
    /// </summary>
    public string LayerReferenceFile
    {
      get
      {
        return Path.Combine(LayerDirectoryName, "LayerReferences.txt");
      }
    }

    /// <summary>
    /// Update the list of layers. Ignores filenames that exist as layers
    /// </summary>
    /// <param name="newLayers"></param>
    /// <param name="progress"></param>
    /// <returns></returns>
    public bool GatherLayers(LayerCollection newLayers, ProgressStatus progress)
    {
      bool bOK = true;
      string directoryName = LayerDirectoryName;
      DirectoryInfo layerFolder = new DirectoryInfo(directoryName);

      if (!layerFolder.Exists)
        return bOK;

      FileInfo[] files = layerFolder.GetFiles("*" + IScene.LayerFileExtension); // filter out *.Layer
      FileInfo[] filesXML = layerFolder.GetFiles("*" + IScene.LayerImportExtension); // filter out *.LayerXML

      // add layer references:
      string refFile = LayerReferenceFile;
      if (File.Exists(refFile))
      {
        List<FileInfo> allFiles = new List<FileInfo>(files); ;
        TextReader txt = new StreamReader(refFile);
        while (true)
        {
          string line = txt.ReadLine();
          if (line == null)
            break;
          if (line.StartsWith("//") || !line.EndsWith(IScene.LayerFileExtension)) // commented out or not .Layer?
            continue;
          string externalname = this.Project.MakeAbsolute(line);
          if (File.Exists(externalname))
            allFiles.Add(new FileInfo(externalname));
        }
        txt.Close();
        files = allFiles.ToArray();
      }

      FilterLayerFiles(files, null);
      FilterLayerFiles(filesXML, files);

      bOK = LoadLayers(newLayers, progress, files);
      bOK &= ImportLayers(newLayers, progress, filesXML);

      return bOK;
    }

    /// <summary>
    /// Opens a dialog to confirm all the layer changes
    /// </summary>
    public void UpdateLayers()
    {
      LayerCollection modifiedLayers = new LayerCollection();
      ZoneCollection modifiedZones = new ZoneCollection();

      // gather new and modified layers
      GatherZones(modifiedZones);
      GatherLayers(modifiedLayers,null);

      // gather deleted layers
      foreach (Layer layer in Layers)
      {
        if (layer.OwnsLock) // cannot be deleted or modified
          continue;

        if (File.Exists(layer.AbsoluteLayerFilename))
          continue;
        
        layer.FileStatus = Layer.LayerFileStatus_e.Deleted;
        modifiedLayers.Add(layer);
      }

      if (modifiedLayers.Count == 0 && modifiedZones.Count == 0)
      {
        EditorManager.ShowMessageBox("No updated, new or deleted layers/zones have been detected", "Update Layers", MessageBoxButtons.OK, MessageBoxIcon.Information);
        return;
      }

      LayerUpdateDlg dlg = new LayerUpdateDlg();
      dlg.SetModifiedFiles(modifiedLayers, modifiedZones);
      if (dlg.ShowDialog()!=DialogResult.OK)
        return;

      EditorManager.Actions.StartGroup("Update Layers");

      // add new zones
      foreach (Zone zone in modifiedZones)
      {
        zone.ParentScene = this;
        Zones.Add(zone);
      }

      // now merge the layers into the scene
      foreach (Layer layer in modifiedLayers)
      {
        Layer.LayerFileStatus_e status = layer.FileStatus;

        if (status==Layer.LayerFileStatus_e.Deleted)
        {
          EditorManager.Actions.Add(new RemoveLayerAction(layer));
          continue;
        }
        if (status==Layer.LayerFileStatus_e.NewLayer)
        {
          EditorManager.Actions.Add(new AddLayerAction(layer,false)); // no unique filenames, use as-is
          continue;
        }
        if (status==Layer.LayerFileStatus_e.Modified)
        {
          Layer oldLayer = Layers.GetLayerByFilename(layer.LayerFilename);
          EditorManager.Actions.Add(new UpdateLayerAction(oldLayer,layer));
          continue;
        }
      }

      EditorManager.Actions.EndGroup();

      modifiedZones.MatchupLayerNames(Layers, null); // only call this for new zones!

      IScene.SendZoneChangedEvent(new ZoneChangedArgs(null, ZoneChangedArgs.Action.RebuildList));


    }

    Zone ImportZone(string filename)
    {
      XmlTextReader xmlReader = new XmlTextReader(filename);
      xmlReader.WhitespaceHandling = WhitespaceHandling.None;
      try
      {
        XmlDocument doc = new XmlDocument();
        doc.Load(xmlReader);
        if (doc.DocumentElement == null)
          throw new Exception("XML does not contain root node");
        IEnumerator nodes = doc.DocumentElement.GetEnumerator();
        while (nodes.MoveNext())
        {
          XmlElement node = nodes.Current as XmlElement;
          if (node == null || node.Name != "zone")
            continue;

          string classname = node.GetAttribute("class");
          string name = node.GetAttribute("name");
          bool bLoaded = true;
          if (EditorManager.Settings.ForceZonesUnloadedAtStartup || string.Compare(node.GetAttribute("loaded"), "false", true) == 0)
            bLoaded = false;

          if (string.IsNullOrEmpty(name))
            name = Path.GetFileNameWithoutExtension(filename);

          Type t = EditorManager.ShapeFactory.GetTypeByName(classname, typeof(Zone), false);
          if (t == null)
            t = typeof(Zone);
          Zone zone = Activator.CreateInstance(t, new object[1] { name }) as Zone;
          if (zone == null)
            throw new Exception("Could not instantiate zone");
          zone.SetZoneFileNameInternal(filename); // same filename but will replace extension
          zone.SetLoadedStatusInternal(bLoaded);

          // apply property/value pairs to layer
          SerializationHelper.ApplyXMLProperties(node, zone, false);

          // parse for layer-references
          IEnumerator propNodes = node.GetEnumerator();
          List<string> layerNames = new List<string>();
          while (propNodes.MoveNext())
          {
            XmlElement propNode = propNodes.Current as XmlElement;
            if (propNode == null)
              continue;
            if (propNode.Name == "layerref")
            {
              string layername = propNode.GetAttribute("filename");
              if (!string.IsNullOrEmpty(layername))
                layerNames.Add(Path.ChangeExtension(layername,IScene.LayerFileExtension)); // always use .Layer file extension here
            }
          }
          zone.LayerFilenames = layerNames.ToArray();
          return zone; // return first zone in the XML
        }
      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex, true);
        return null;
      }
      return null;
    }

    /// <summary>
    /// Loads all zone files in the layer directory into
    /// </summary>
    /// <returns></returns>
    public bool GatherZones(ZoneCollection newZones)
    {
      bool bOK = true;
      string directoryName = LayerDirectoryName;
      DirectoryInfo layerFolder = new DirectoryInfo(directoryName);

      if (!layerFolder.Exists)
        return bOK;


      List<Zone> zonesToRename = new List<Zone>();

      // binary zone files
      FileInfo[] files = layerFolder.GetFiles("*" + IScene.ZoneFileExtension); // filter out *.Zone
      foreach (FileInfo fileInfo in files)
      {
        BinaryFormatter fmt = SerializationHelper.BINARY_FORMATTER;
        Zone zone = null;
        try
        {
          // open the layer in read-only mode
          FileStream fs = new FileStream(fileInfo.FullName, FileMode.Open, FileAccess.Read);
          zone = (Zone)fmt.Deserialize(fs);
          fs.Close();
        }
        catch (Exception ex)
        {
          EditorManager.DumpException(ex);
          EditorManager.ShowMessageBox("An exception occurred while loading zone" + fileInfo.Name + "\n\nDetailed Message:\n" + ex.ToString(),
            "Zone loading error", MessageBoxButtons.OK, MessageBoxIcon.Error);
          continue;
        }
        Zone existingZoneFileName = Zones.GetZoneByFilename(zone.ZoneName, null); //check if the zone file is already in the scene
        if (existingZoneFileName != null)
        {
          continue;
        }
        Zone existingZone = Zones.GetZoneByName(zone.ZoneName); //check if the zone name already exists in the scene. This can be possible since filename and zonename don't have to match
        if (existingZone != null)
          zonesToRename.Add(zone);

        zone.SetZoneFileNameInternal(fileInfo.Name);
        zone.ParentScene = this;
        newZones.Add(zone);
      }

      // import from XML
      FileInfo[] filesXML = layerFolder.GetFiles("*" + IScene.ZoneImportExtension); // filter out *.ZoneXML
      foreach (FileInfo fileInfo in filesXML)
      {
        // already loaded as binary?
        if (newZones.GetZoneByFilename(fileInfo.Name, null) != null)
          continue;

        Zone zone = ImportZone(fileInfo.FullName);
        if (zone == null)
          continue;

        Zone existingZoneFileName = Zones.GetZoneByFilename(zone.ZoneName, null); //check if the zone file is already in the scene
        if (existingZoneFileName != null)
        {
          continue;
        }

        Zone existingZone = Zones.GetZoneByName(zone.ZoneName); //check if the zone name already exists in the scene. This can be possible since filename and zonename don't have to match
        if (existingZone != null)
          zonesToRename.Add(zone);

        zone.SetZoneFileNameInternal(fileInfo.Name);
        zone.ParentScene = this;
        newZones.Add(zone);
      }


      if (zonesToRename.Count > 0)
      {
        string warningMsg = "The following zones had to be renamed, since they are already loaded in the scene:\n";
          
        foreach (Zone zoneToRename in zonesToRename)
        {
          string uniqueZoneName = this.CreateUniqueZoneName(zoneToRename.ZoneName);
          string oldZoneName = zoneToRename.ZoneName;
          zoneToRename.SetZoneNameInternal(uniqueZoneName);
          warningMsg += oldZoneName + " -> " + uniqueZoneName + "\n";
          //make dirty so it will be saved when saving the scene
          zoneToRename.Dirty = true;
        }

        EditorManager.ShowMessageBox(warningMsg, "Renaming zone(s)", MessageBoxButtons.OK, MessageBoxIcon.Warning);
          
      }


      newZones.Sort();
      return bOK;
    }




    /// <summary>
    /// Overridden SaveAs function
    /// </summary>
    /// <param name="relFilename"></param>
    /// <returns></returns>
    public override bool SaveAs(string relFilename)
    {
      bool bOK = true;
      _lastSaveErrorMsg = ""; //reset last error message string

      bool bSaveToNewLocation = false;
      string absFileName = Path.Combine(Project.ProjectDir, relFilename);
      string oldLayerDir = this.LayerDirectoryName;
      string oldAbsFileName = AbsoluteFileName;
      if (absFileName != oldAbsFileName)
      {
        bSaveToNewLocation = true;
        FileName = relFilename;
      }
      string newLayerDir = this.LayerDirectoryName;

      // get the current camera position/orientation for the settings
      Settings.CameraPosition = EditorManager.ActiveView.CameraPosition;
      Settings.CameraAngles = EditorManager.ActiveView.CameraAngles;

      _iSceneversion = SCENE_VERSION_CURRENT;

      // STEP 0: make sure there is a folder of the scene's name
      string directoryName = absFileName + LayerDirectoryExtension;
      System.Diagnostics.Debug.Assert(directoryName.IndexOfAny(Path.GetInvalidPathChars())<0,"Layer directory path contains invalid characters");

      if (!Directory.Exists(directoryName))
        Directory.CreateDirectory(directoryName);

      // trigger event
      EditorManager.OnSceneEvent(new SceneEventArgs(SceneEventArgs.Action.BeforeSaving,true));

      // STEP 1: Create a 0 byte scene file if it doesn't exist
      if (!File.Exists(absFileName))
      {
        FileStream fs = Project.CreateDataFile(absFileName, true /* Binary file */);
        if (fs!=null) fs.Close();
      }

      // STEP 2: Save user specific data
      string userFilename = Path.Combine(directoryName, EditorManager.UserSettingsFilename);
      SaveUserSettingsFile(userFilename);
      // if there is no default.user file, create one:
      userFilename = Path.Combine(directoryName, "default.user");
      if (!File.Exists(userFilename))
        SaveUserSettingsFile(userFilename);
      
      // profiles are only saved via Export dialog
      //CurrentExportProfile.SaveToFile(); // save export profile to different file

      StringCollection additionalFilesToCopy = new StringCollection();

      // STEP 3: Save the zone files
      Zones.EnumSortingOrder();
      foreach (Zone zone in Zones)
      {
        // restructure this loop when zones support locking
        if (EditorManager.Settings.SaveModifiedLayersOnly && !zone.Dirty && !bSaveToNewLocation)
          continue;
        bOK &= zone.SaveToFile();
        if (!bOK)
        {
          _lastSaveErrorMsg = "Could not save zone " + zone.ZoneFilename;
        }
        if (bSaveToNewLocation && !zone.Loaded && zone.FullZoneUnload)
        {
          // in this case, add the layer files to a list of files that should be copied
          additionalFilesToCopy.AddRange(zone.LayerFilenames);
        }
      }


      // STEP 4: Save all .layer files. Only the dirty ones to avoid unnecessary time stamp updates for unmodified layers.
      int iSortingKey = 1;
      foreach (Layer layer in this.Layers)
      {
        layer.SortingOrder = iSortingKey++;
        if (!bSaveToNewLocation)
        {
          if (!layer.OwnsLock)
            continue;
          if (EditorManager.Settings.SaveModifiedLayersOnly && !layer.Dirty)
            continue;
          bOK &= layer.SaveToFile();
        }
        else
        {
          bOK &= layer.SaveToFile();
          if (layer.OwnsLock)
          {
            // We own the lock. Release the lock for the old location and acquire 
            // a new one at the new location
            layer.ReleaseLock(false, false);
            layer.TryLock(this, false);
            Debug.Assert(layer.OwnsLock);
          }
          else
          {
            // We do not own the lock.
            // Clear the old lock instance and acquire a new one.
            layer.ForceRemoveExternalLock();
          }
        }
        if (!bOK)
        {
          _lastSaveErrorMsg = "Could not save layer " + layer.LayerFilename;
        }
      }
      // STEP 5: Any other files to copy to new location?
      if (bSaveToNewLocation)
      {
        DirectoryInfo oldDir = new DirectoryInfo(oldLayerDir);
        DirectoryInfo newDir = new DirectoryInfo(newLayerDir);
        FileHelper.CopyFiles(oldDir, "*.lit", newDir, false, false, FileAttributes.Hidden, false);
        FileHelper.CopyFiles(oldDir, "Thumbnail.*", newDir, false, false, FileAttributes.Hidden, false);
        FileHelper.CopyFiles(oldDir, "lighting.cfg", newDir, false, false, FileAttributes.Hidden, false);
        FileHelper.CopyFiles(oldDir, "*.user", newDir, false, false, FileAttributes.Hidden, false);
        FileHelper.CopyFiles(oldDir, "*."+SceneExportProfile.FILE_EXTENSION_EXPORTPROFILE, newDir, false, false, FileAttributes.Hidden, false);
        foreach (string layerFile in additionalFilesToCopy)
          FileHelper.CopyFiles(oldDir, layerFile, newDir, false, false, FileAttributes.Hidden, false);

      }

      // STEP 6: Trigger event
      EditorManager.OnSceneEvent(new SceneEventArgs(SceneEventArgs.Action.AfterSaving,bOK));
      if (bSaveToNewLocation)
        EditorManager.OnSceneEvent(new SaveSceneAsEventArgs(oldAbsFileName, absFileName));

      if (bOK) 
        m_bDirty = false;

      // Also save the current export profile because it stores which layer should be exported:
      CurrentExportProfile.ExportedLayersFromScene();
      CurrentExportProfile.SaveToFile();

      return bOK;
    }

    /// <summary>
    /// Overridden scene close function
    /// </summary>
    /// <returns></returns> 
    public override bool Close()
    {
      // Close a possibly open description dialog when closing the scene
      if (_descriptionDialog != null)
      {
        _descriptionDialog.FormClosed -= new FormClosedEventHandler(_OnDescriptionDialogClosed);
        if (!_descriptionDialog.IsDisposed)
          _descriptionDialog.Close();
      }

      EditorManager.OnSceneEvent(new SceneEventArgs(SceneEventArgs.Action.BeforeClosing, true));

      // unlock the file (layers)
      foreach (Layer layer in Layers)
        layer.ReleaseLock(false,false);

      // Delete the scene file watcher that listens for lock/layer file changes.
      RemoveFileWatcher();

      // Stop playback
      EditorManager.EditorMode = EditorManager.Mode.EM_NONE;
      OnRemoveAllEngineInstances();
      Project.Scene = null;

      EditorManager.EngineManager.DeInitScene();

      _currentProfile = null;

      bool bResult = base.Close();

      EditorManager.OnSceneEvent(new SceneEventArgs(SceneEventArgs.Action.AfterClosing, bResult));

      return bResult;
    }

    /// <summary>
    /// Overridden dirty function
    /// </summary>
    public override bool Dirty
    {
      get  {  return (m_bDirty);    }
      set  
      {
        if (m_bDirty == value)
          return;
        m_bDirty = value;
        SendDirtyFlagChangedEvent();
      }
    }

    private bool m_bDirty;

    /// <summary>
    /// Overridden Update function
    /// </summary>
    /// <param name="bImmediate">if false, wait for the next timer</param>
    public override void UpdateView(bool bImmediate)
    {
      EditorApp.ActiveView.UpdateView(bImmediate);
    }

    /// <summary>
    /// Overrides the SceneExtents if this scene has a orthographic view box shape
    /// </summary>
    public override CSharpFramework.Math.BoundingBox SceneExtents
    {
      get
      {
        V3DLayer layer = this.V3DLayer;
        if (layer != null && layer.HasValidOrthographicViewBox)
          return layer.OrthographicViewBox.AbsBoundingBox;

        // calculate the extents
        return base.SceneExtents;
      }
    }

    public override string RendererNodeClass
    {
      get
      {
        V3DLayer layer = this.V3DLayer;
        if (layer == null) // should not happen unless the scene is broken
          return null;
        return layer.RendererNodeClass;
      }
      set
      {
        V3DLayer.RendererNodeClass = value;
      }
    }

    public override DynamicPropertyCollection RendererProperties
    {
      get
      {
        return V3DLayer.RendererProperties;
      }
      set
      {
        V3DLayer.RendererProperties = value;
      }
    }

    public override ShapeComponentCollection Postprocessors
    {
      get
      {
        return V3DLayer.Postprocessors;
      }
      set
      {
        V3DLayer.Postprocessors = value;
      }
    }

    private string _lastSaveErrorMsg = "";

    public override string LastSaveErrorMsg
    {
      get
      {
        return _lastSaveErrorMsg;
      }
    }

    public override bool AddLayer(Layer layer, bool bSetAsActive, bool bCreateEngineInstances, bool bLock)
    {
      if (!base.AddLayer(layer, bSetAsActive, bCreateEngineInstances, bLock))
        return false;

      if (layer.IsReference)
        this.WriteReferenceFile();

      return true;
    }

    public override bool RemoveLayer(Layer layer)
    {
      if (!base.RemoveLayer(layer))
        return false;

      if (layer.IsReference)
        this.WriteReferenceFile();

      return true;
    }

    #endregion
    
    #region ISerializable Members

    /// <summary>
    /// The purpose of scene serialization is only to support action recording
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected EditorScene(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      _sceneSettings = (EditorSceneSettings)info.GetValue("_sceneSettings", typeof(EditorSceneSettings));
    }

    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);
      info.AddValue("_sceneSettings", _sceneSettings);
    }

    #endregion

    #region Clipboard Helper

    /// <summary>
    /// Overridden function to paste clipboard content
    /// </summary>
    /// <param name="Clipboard"></param>
    /// <param name="opHint">optional parameter that provides additional information for pasting the data (e.g. ShapeCollection.PasteHint)</param>
    /// <returns></returns>
    public override bool PasteClipboardContent(EditorClipboard Clipboard, object opHint)
    {
      if (!Clipboard.HasData())
        return false;

      // the data object class might know how to paste? (not by default though)
      if (Clipboard.Data.TryPaste(opHint))
        return true;

      object obj = Clipboard.Data.Object;

      Layer layer;

      // single shape in clipboard?
      ShapeBase singleshape = obj as ShapeBase;
      if (singleshape!=null)
      {
       
        layer = ActiveLayer;
        ShapeBase parent = layer.ActiveShape;
        // Override the parent if a hint structure has been passed
        if (opHint != null && opHint is ShapeCollection.PasteHint)
          parent = (opHint as ShapeCollection.PasteHint).Parent;
        if (parent != null)
        {
          if (!singleshape.AllowsToSetParent(parent) || !parent.AllowsToAddChild(singleshape))
          {
            return false;
          }
        }
        EditorManager.Actions.Add(AddShapeAction.CreateAddShapeAction(singleshape.Clone(), layer.ActiveShape, layer, true));
        return true;
      }

      // shape collection in clipboard?
      ShapeCollection shapes = obj as ShapeCollection;
      if (shapes!=null)
      {
        layer = ActiveLayer;
        ShapeBase parent = layer.ActiveShape;

        // Override the parent if a hint structure has been passed
        if (opHint != null && opHint is ShapeCollection.PasteHint)
          parent = (opHint as ShapeCollection.PasteHint).Parent;

      

        shapes = (ShapeCollection)shapes.CloneForClipboard(); // clone all shapes (necessary)

        //check if it is allowed to set shape as parent
        foreach (ShapeBase shape in shapes)
        {
          if (parent != null)
          {
            if (!shape.AllowsToSetParent(parent) || !parent.AllowsToAddChild(shape))
            {
              return false;
            }
          }
         
        }


        IAction action = AddShapesAction.CreateAddShapesAction(shapes, parent, layer, true, "paste");
        EditorManager.Actions.Add(action);
     
        // set this collection as new selection in the Gizmo
        // (we can't do this in the loop above, because the ItemChange actions set a new selected item in the gizmo)
        EditorApp.ActiveView.Gizmo.Clear();
        foreach (ShapeBase shape in shapes)
          if ((shape as Shape3D)!=null)
            EditorApp.ActiveView.Gizmo.AddShape(shape,false);

        return true;
      }

      // layer in clipboard?
      layer = obj as Layer;
      if (layer!=null)
      {
        Layer newLayer = (Layer)layer.Clone();
        newLayer.SetLayerNameInternal(EditorManager.Scene.Layers.GetUniqueLayerName(newLayer.LayerName,null));
        EditorManager.Actions.Add(new AddLayerAction(newLayer));
        return true;
      }

      // more than one layer?
      LayerCollection layers = obj as LayerCollection;
      if (layers != null && layers.Count>0)
      {
        if (layers.Count > 1)
          EditorManager.Actions.StartGroup("Paste layers");
        foreach (Layer newLayer in layers)
          EditorManager.Actions.Add(new AddLayerAction((Layer)newLayer.Clone()));
        if (layers.Count > 1)
          EditorManager.Actions.EndGroup();
      }

      // a zone?
      Zone zone = obj as Zone;
      if (zone != null)
      {
        // the action takes care to generate a unique (file-)name
        EditorManager.Actions.Add(new AddZoneAction((Zone)zone.Clone(), true));
      }

      // more than one zone?
      ZoneCollection zones = obj as ZoneCollection;
      if (zones != null && zones.Count>0)
      {
        if (zones.Count > 1)
          EditorManager.Actions.StartGroup("Paste zones");
        foreach (Zone newZone in zones)
          EditorManager.Actions.Add(new AddZoneAction((Zone)newZone.Clone(), true));
        if (zones.Count > 1)
          EditorManager.Actions.EndGroup();
      }

      // sky config in clipboard?
      SkyConfig sky = obj as SkyConfig;
      if (sky != null)
      {
        if (V3DLayer!=null)
          EditorManager.Actions.Add(new SkyConfigChangedAction(V3DLayer,(SkyConfig)sky.Clone()));
        return true;
      }

      // property values in clipboard (copied via property tree)?
      PropertyDictionary propDict = EditorManager.Clipboard.DataObject as PropertyDictionary;
      if (propDict != null)
      {
        // Apply the properties to the selected shapes
        IAction propAction = propDict.CreateAction(EditorManager.SelectedShapes);
        if (propAction != null)
          EditorManager.Actions.Add(propAction);

        return true;
      }

      // single property value in clipboard (copied via property tree)?
      PropertyKeyValuePair propertyData = EditorManager.Clipboard.DataObject as PropertyKeyValuePair;
      if (propertyData != null)
      {

        IAction propAction = propertyData.CreateAction(EditorManager.SelectedShapes);
        if (propAction != null)
          EditorManager.Actions.Add(propAction);

        return true;
      }

      return false;
    }

    #endregion

    #region V3D loading

    /// <summary>
    /// deprecated, use MainLayer instead
    /// </summary>
    public V3DLayer V3DLayer
    {
      get
      {
        return (V3DLayer)MainLayer;
      }
    }

    /// <summary>
    /// Returns the main layer of the scene. The main layer contains renderer properties and is also used for custom scene element serialization.
    /// </summary>
    public override Layer MainLayer
    {
      get
      {
        foreach (Layer layer in Layers)
          if (layer is V3DLayer)
            return layer;
        return null;
      }
    }

    /// <summary>
    /// Project relative filename that is used for vLux computations: either V3D filename or the scene's filename
    /// </summary>
    public string LightingBaseFilename
    {
      get 
      {
        return FileName;
      }
    }

    #endregion

    #region Exporting


    /// <summary>
    /// Returns the absolute export path name. The returned value is always a valid absolute path
    /// </summary>
    public string AbsoluteExportPath
    {
      get 
      {
        string filename = CurrentExportProfile.ExportPath;
        if (filename != null && FileHelper.IsAbsolute(filename)) // it is already an absolute path
          return filename;

        if (string.IsNullOrEmpty(filename)) // use scene filename
          return Path.Combine(Project.ProjectDir, Path.ChangeExtension(FileName, SceneExportProfile.FILE_EXTENSION_EXPORT));

        if (Path.GetExtension(filename)==null) // add default file extension
          filename = Path.ChangeExtension(filename, SceneExportProfile.FILE_EXTENSION_EXPORT);

        return Path.Combine(Project.ProjectDir,filename);
      }
    }

    /// <summary>
    /// Returns the folder name for the zone files. This is ...\xyz.vscene_data
    /// </summary>
    public string AbsoluteZoneExportFolder
    {
      get
      {
        string filename = AbsoluteExportPath;
        return filename + "_data";
      }
    }

    private bool bAssetPreviewModeWasOn = false;

    private void PrepareAssetsForExport()
    {
      bAssetPreviewModeWasOn = EditorManager.ProfileManager.GetAssetPreviewMode();

      // make sure that the 'Asset Preview Mode' is always enabled during export
      if (!bAssetPreviewModeWasOn)
      {
        // if it is not enabled, do enable runtime mode
        EditorManager.Actions.Add(new ChangeActiveProfileAction(EditorManager.ProfileManager.GetActiveProfile()._name, true));
      }
    }

    private void ResetAssetManagementStateAfterExport()
    {
      if (!bAssetPreviewModeWasOn)
      {
        // reset the asset runtime mode back to the state before export
        EditorManager.Actions.Add(new ChangeActiveProfileAction(EditorManager.ProfileManager.GetActiveProfile()._name, false));
      }
    }

    public override bool ExportScene(string absPath, bool bShowDialog)
    {
      string assetProfileOverride = null;

      // if we do not show the dialog, we must use the active profile
      if (!bShowDialog)
      {
        assetProfileOverride = EditorManager.ProfileManager.GetActiveProfile()._name;
      }

      if (bShowDialog)
      {
        // Open export dialog
        ExportDialog dlg = new ExportDialog();
        using (dlg)
        {
          CurrentExportProfile.ExportedLayersFromScene(); // retrieve current status
          dlg.Settings = CurrentExportProfile; // clones the settings
          dlg.AutoSaveExportProfile = Settings.AutoSaveExportProfile;

          // Show dialog
          if (dlg.ShowDialog() != DialogResult.OK)
            return true;


          // Get back settings
          SceneExportProfile newProfile = dlg.Settings;
          if (!CurrentExportProfile.Equals(newProfile))
            Dirty = true;
          _currentProfile = newProfile;
          Settings.ExportProfileName = CurrentExportProfile.ProfileName;
          Settings.AutoSaveExportProfile = dlg.AutoSaveExportProfile;

          if (dlg.ExportActiveProfileOnly)
          {
            assetProfileOverride = EditorManager.ProfileManager.GetActiveProfile()._name;
          }

          EditorManager.EngineManager.CheckLightGridDataExists();
        }
      }

      // Deactivate isolate selection mode temporarily
      EditorManager.ActiveView.IsolateSelection(false, true);

      // Ensure that scene script file is set in script manager
      // This is e.g. required if the script was broken when the scene was loaded but it was corrected in the meantime
      // then the exporter scene will have the proper script
      string sceneScriptFile = ((V3DLayer)EditorManager.Scene.MainLayer).SceneScriptFile;
      if (sceneScriptFile != null && sceneScriptFile.Length > 0)
      {
        ScriptManager.SetSceneScriptFile(sceneScriptFile);
      }

      // Export
      bool bSuccess = ExportScene(absPath, assetProfileOverride);
      string absExportPath = absPath;
      if (absExportPath == null)
        absExportPath = AbsoluteExportPath;


      // Reactivate isolate selection mode
      EditorManager.ActiveView.IsolateSelection(true, true);

      // now launch the viewer
      if (bSuccess && CurrentExportProfile.RunAfterExport)
      {
        // Try to find the optimal profile to run:
        string profileToRun = null;
        if (assetProfileOverride != null)
        {
          // If exporting for one specific profile, use that one
          profileToRun = assetProfileOverride;
        }
        else if (CurrentExportProfile.SelectedAssetProfiles.IsActiveProfileSet)
        {
          // If the current profile is among the selected profiles, use that one
          profileToRun = EditorManager.ProfileManager.GetActiveProfile()._name;
        }
        else
        {
          // Otherwise, use the first profile we can find.
          foreach (IProfileManager.Profile profile in EditorManager.ProfileManager.GetProfiles())
          {
            if (CurrentExportProfile.SelectedAssetProfiles.IsProfileSet(profile._name))
            {
              profileToRun = profile._name;
              break;
            }
          }
        }

        // If there is a profile we can run, start the scene viewer.
        if (profileToRun != null)
        {
          string sceneToRun = absExportPath;
          string oldExtension = Path.GetExtension(sceneToRun);
          sceneToRun = Path.ChangeExtension(sceneToRun, profileToRun) + oldExtension;

          string path = Path.GetDirectoryName(Application.ExecutablePath);
          string absSceneViewerPath = Path.Combine(path, "vSceneViewer.exe");
          FileHelper.RunExternalTool("Scene Viewer", absSceneViewerPath, "\"" + sceneToRun + "\"", false);
        }
      }
      return bSuccess;
    }

    /// <summary>
    /// Export the scene to vscene file
    /// </summary>
    /// <param name="absPath">the absolute export path. Can be null to use the setting's path</param>
    /// <param name="profileOverride">an optional asset profile name, which overrides the asset profiles set in the current scene export profile</param>
    /// <returns></returns>
    public bool ExportScene(string absPath, string assetProfileOverride)
    {
      if (absPath != null)
        CurrentExportProfile.ExportPath = absPath;
      else
        absPath = AbsoluteExportPath;
      return ExportSceneNotSaveSettings(absPath, assetProfileOverride);
    }

    /// <summary>
    /// Export the scene to vscene file and do not save the path to the settings
    /// </summary>
    /// <param name="absPath">the absolute export path. Can be null to use the setting's path</param>
    /// <param name="profileOverride">an optional asset profile name, which overrides the asset profiles set in the current scene export profile</param>
    /// <returns></returns>
    public bool ExportSceneNotSaveSettings(string absPath, string assetProfileOverride)
    {
      EditorManager.AssetManager.UpdateAssetTransformationsOnExport = CurrentExportProfile.UpdateAssetTransformations;

      bool savedAssetPreviewMode = EditorManager.ProfileManager.GetAssetPreviewMode();
      string savedAssetProfile = EditorManager.ProfileManager.GetActiveProfile()._name;

      // Determine the asset settings (preview mode, profiles) to be used for export
      bool newAssetPreviewMode = CurrentExportProfile.UpdateAssetTransformations;
      List<string> assetProfilesToExport = new List<string>();
      if (assetProfileOverride != null)
      {
        assetProfilesToExport.Add(assetProfileOverride);
      }
      else
      {
        foreach (IProfileManager.Profile profile in EditorManager.ProfileManager.GetProfiles())
        {
          // Add all selected profiles, but not the active one
          if (profile != EditorManager.ProfileManager.GetActiveProfile() && CurrentExportProfile.SelectedAssetProfiles.IsProfileSet(profile._name))
          {
            assetProfilesToExport.Add(profile._name);
          }
        }

        // Add the active profile last, if it is selected. When exporting, we may be able to save us a
        // time-consuming profile/preview mode switch this way.
        if (CurrentExportProfile.SelectedAssetProfiles.IsActiveProfileSet)
        {
          assetProfilesToExport.Add(EditorManager.ProfileManager.GetActiveProfile()._name);
        }
      }


      bool bResult = true;
      foreach (string profileName in assetProfilesToExport)
      {
        if (!EditorManager.ProfileManager.SetActiveProfile(profileName, newAssetPreviewMode))
        {
          bResult = false;
          break;
        }

        try
        {
          bResult = _ExportSceneNotSaveSettings(absPath);
        }
        catch (Exception ex)
        {
          EditorManager.Progress.HideProgressDialog();
          EditorManager.DumpException(ex, true);
          bResult = false;
        }

        if (!bResult)
        {
          break;
        }
      }

      // Restore the saved profile state again
      EditorManager.ProfileManager.SetActiveProfile(savedAssetProfile, savedAssetPreviewMode);

      return bResult;
    }

    private bool _ExportSceneNotSaveSettings(string absPath)
    {
      if (EditorManager.Settings.SaveResourcesAtExportTime)
        EditorManager.TriggerSaveResources();

      // Incorporate the current asset profile name into the given export path...
      string profileName = EditorManager.ProfileManager.GetActiveProfile()._name;
      string oldExtension = Path.GetExtension(absPath);
      absPath = Path.ChangeExtension(absPath, profileName) + oldExtension;

      // ...and into the export profile export path (to then end up in the absolute zone export folder <sigh>).
      string oldExportPath = CurrentExportProfile.ExportPath;
      string exportProfileOwnerFileName = CurrentExportProfile.OwnerScene.FileName;
      CurrentExportProfile.ExportPath = Path.ChangeExtension(oldExportPath, profileName) + Path.GetExtension(oldExportPath);

      // That's Not Particularly Nice (tm): We try to figure out if the old export path was derived from the owner's filename
      // or whether it was set explicitly.
      oldExportPath = (oldExportPath == Path.ChangeExtension(exportProfileOwnerFileName, SceneExportProfile.FILE_EXTENSION_EXPORT))
        ? null : oldExportPath;

      bool bOK = true;
      try
      {
        bool bForceLocalPos = EditorManager.Settings.AllowZonePivots;

        /// Recreate all engine instances before export.
        /// This fixes the problem of script instances not being up to date
        /// on export (see ticket #16991).
        this.RecreateSceneEntityInstances();

        // create the directory where all the zone files (and vscene specific files) go
        string folder = this.AbsoluteZoneExportFolder;
        try
        {
          if (!Directory.Exists(folder))
            Directory.CreateDirectory(folder);
        }
        catch (Exception ex)
        {
          EditorManager.DumpException(ex, true);
          bOK = false;
        }

        // start the actual export process
        SceneExportInfo info = new SceneExportInfo();
        info.ExportType = SceneExportInfo.ExportType_e.VScene;
        info.ExportFlags = CurrentExportProfile.ExportFlags;
        info.IncludeVisibility = CurrentExportProfile.IncludeVisibilityInfo;
        info.EmbedCustomLitFile = CurrentExportProfile.EmbedCustomLitFile;
        info.AbsoluteFilename = absPath;
        info.AbsoluteExportDataFolder = folder;
        info.RelevantExportZones = GetExportRelevantZones();
        info.RelevantEmbeddedZones = GetRelevantEmbeddedZones(); // this collection might change later

        switch (EditorManager.ProfileManager.GetActiveProfile()._platform)
        {
          case TargetDevice_e.TARGETDEVICE_DX9:
            info.ShapeFilter = (int)TargetPlatformSupport.DX9;
            break;
          case TargetDevice_e.TARGETDEVICE_DX11:
            info.ShapeFilter = (int)TargetPlatformSupport.DX11;
            break;
          case TargetDevice_e.TARGETDEVICE_XBOX360:
            info.ShapeFilter = (int)TargetPlatformSupport.Xbox360;
            break;
          case TargetDevice_e.TARGETDEVICE_PS3:
            info.ShapeFilter = (int)TargetPlatformSupport.PS3;
            break;
          case TargetDevice_e.TARGETDEVICE_PSP2:
            info.ShapeFilter = (int)TargetPlatformSupport.PSP2;
            break;
          case TargetDevice_e.TARGETDEVICE_IOS:
            info.ShapeFilter = (int)TargetPlatformSupport.IOS;
            break;
          case TargetDevice_e.TARGETDEVICE_ANDROID:
            info.ShapeFilter = (int)TargetPlatformSupport.ANDROID;
            break;
          case TargetDevice_e.TARGETDEVICE_WIIU:
            info.ShapeFilter = (int)TargetPlatformSupport.WiiU;
            break;
          default:
            break;
        }

        // Trigger callbacks: ask every listener if the export actually should be performed.
        // Trigger the event, allowing plugins to cancel the export
        ExportSceneEventArgs args = new ExportSceneEventArgs(SceneEventArgs.Action.AllowExport, true, info);
        EditorManager.OnSceneEvent(args);
        if (args.CancelExport)
        {
          CurrentExportProfile.ExportPath = oldExportPath;
          return false;
        }

        // Trigger callbacks: This is only called for the vscene, not for the zones.
        // This could be changed, but listeners should handle this properly then, i.e. check the CurrentZone member
        ExportHelper.TriggerStartExportCallbacks(info);

        // Ensure that scene script file is set in script manager
        // This is e.g. required if the script was broken when the scene was loaded but it was corrected in the meantime
        // then the exporter scene will have the proper script
        // This is executed here because the callbacks also may trigger saving of scene scripts
        string sceneScriptFile = ((V3DLayer)EditorManager.Scene.MainLayer).SceneScriptFile;
        if (sceneScriptFile != null && sceneScriptFile.Length > 0)
        {
          ScriptManager.SetSceneScriptFile(sceneScriptFile);
        }

        // build plugin list (needed in StartExport)
        // We also want all plugin refs from the zones in the vscene because we cannot load engine plugins later
        IShapeVisitor collectPluginsVisitor = new SceneExportInfo.CollectPluginInfoVisitor(info);
        foreach (Layer layer in Layers)
          if (layer.ParentZone == null || info.RelevantExportZones.Contains(layer.ParentZone))
            layer.Root.RunVisitor(collectPluginsVisitor);


        EditorManager.Progress.ShowProgressDialog("Export scene");
        float fProgressStep = 100.0f / (info.RelevantExportZones.Count + 2);

        // only export layers to vscene that are not attached to a zone
        ResourceSnapshotInfo snapshot = new ResourceSnapshotInfo();

        this._bForceLocalPosition = true;

        // compute visibility in engine
        EditorManager.Progress.StatusString = "Build Visibility...";
        bool bOldUseVisibilityInEngineState = EditorManager.VisibilityBuilder.UseInEngine;
        EditorManager.Progress.SetRange(EditorManager.Progress.Percentage, EditorManager.Progress.Percentage + fProgressStep * 0.5f); // 1st half of visibility
        if (info.IncludeVisibility && EditorManager.VisibilityBuilder.Status != IVisibilityBuilder.VisibilityStatus_e.Valid)
          EditorManager.VisibilityBuilder.Build(EditorManager.Scene, EditorManager.Progress);

        EditorManager.VisibilityBuilder.UseInEngine = info.IncludeVisibility; // make sure engine instances have visibility assigned
        EditorManager.Progress.SetRange(0.0f, 100.0f);
        EditorManager.Progress.Percentage += fProgressStep * 0.5f; // 2nd half of visibilty

        // start with zone files
        if (info.RelevantExportZones.Count > 0)
        {
          SceneExportInfo zoneinfo = new SceneExportInfo();
          zoneinfo.ExportType = SceneExportInfo.ExportType_e.VZone;
          zoneinfo.ExportFlags = CurrentExportProfile.ExportFlags; // not used here though
          zoneinfo.IncludeVisibility = CurrentExportProfile.IncludeVisibilityInfo;
          zoneinfo.EmbedCustomLitFile = CurrentExportProfile.EmbedCustomLitFile;
          zoneinfo.RelevantExportZones = info.RelevantExportZones;
          zoneinfo.RelevantEmbeddedZones = info.RelevantEmbeddedZones;
          zoneinfo.ShapeFilter = info.ShapeFilter;

          foreach (Zone zone in info.RelevantExportZones)
          {
            bool bOldLoaded = zone.Loaded;
            if (!bOldLoaded)
            {
              EditorManager.Progress.StatusString = string.Format("Loading zone {0}", zone.ZoneName);
              Application.DoEvents();
              zone.ShowProgress = false;
              zone.Loaded = true;
            }
            zoneinfo.CurrentZone = zone;
            zoneinfo.AbsoluteFilename = Path.Combine(folder, zone.ExportFilename);
            snapshot.AbsoluteFilename = Path.ChangeExtension(zoneinfo.AbsoluteFilename, ".vres");
            snapshot.CurrentZone = zone;

            EditorManager.Progress.StatusString = string.Format("Export zone {0}", zone.ZoneName);
            Application.DoEvents();

            // we need the relevant shapes in this collection
            zoneinfo.RelevantShapes = new ShapeCollection();
            foreach (Layer layer in zone.Layers)
            {
              if (layer.ExportFinal)
                zoneinfo.RelevantShapes.AddRange(SceneExportInfo.GetRelevantExportShapes(layer.Root));
            }

            bOK &= ExportHelper.StartZoneExport(zoneinfo);
            bOK &= ExportHelper.StartResourceSnapshot(snapshot);

            foreach (Layer layer in zone.Layers)
            {
              if (!layer.ExportFinal)
                continue;
              if (!CurrentExportProfile.InvisibleLayersExportInvisibleObjects)
                layer.Root.SetVisible(true, true);
              if (bForceLocalPos)
                layer.Root.RunVisitor(_repositionVisitor);
              bOK &= layer.OnExport(zoneinfo);
              layer.GetDependencies(snapshot);
            }

            bOK &= ExportHelper.EndZoneExport(zoneinfo);
            bOK &= ExportHelper.EndResourceSnapshot(snapshot);

            if (!bOldLoaded)
            {
              EditorManager.Progress.StatusString = string.Format("Unloading zone {0}", zone.ZoneName);
              Application.DoEvents();
              zone.ShowProgress = false;
              zone.Loaded = false;
            }

            EditorManager.Progress.Percentage += fProgressStep;
            Application.DoEvents();
          }
        } // end of export all zones


        // vscene file
        EditorManager.Progress.StatusString = "Export layers that are not in zones to vscene";
        info.RelevantEmbeddedZones = GetRelevantEmbeddedZones(); // collection might have changed after zone export (e.g. valid bbox)
        info.RelevantShapes = new ShapeCollection();
        foreach (Layer layer in Layers)
          if (layer.ExportFinal && layer.ParentZone == null)
            info.RelevantShapes.AddRange(SceneExportInfo.GetRelevantExportShapes(layer.Root));

        // mark the plugins that should be embedded
        ExportHelper.ApplyPluginsUserSelection(CurrentExportProfile.PluginsUserSelection);

        if (!ExportHelper.StartExport(info))
        {
          if (!string.IsNullOrEmpty(info.LastErrorMessage))
            EditorManager.ShowMessageBox(info.LastErrorMessage, "Export canceled");
          else
            EditorManager.ShowMessageBox("The export was canceled for an unknown reason", "Export canceled");

          this._bForceLocalPosition = false;
          if (bForceLocalPos)
            this.OnRepositionShapes(null);
          EditorManager.VisibilityBuilder.UseInEngine = bOldUseVisibilityInEngineState; // reset the state
          EditorManager.Progress.HideProgressDialog();
          CurrentExportProfile.ExportPath = oldExportPath;
          return false;
        }

        this._bForceLocalPosition = true;

        snapshot.AbsoluteFilename = Path.Combine(folder, "resources.vres");
        snapshot.CurrentZone = null;
        bOK &= ExportHelper.StartResourceSnapshot(snapshot);
        foreach (Layer layer in Layers)
          if (layer.ExportFinal && layer.ParentZone == null)
          {
            if (!CurrentExportProfile.InvisibleLayersExportInvisibleObjects)
              layer.Root.SetVisible(true, true);

            if (bForceLocalPos)
              layer.Root.RunVisitor(_repositionVisitor);

            layer.GetDependencies(snapshot);
            bOK &= layer.OnExport(info);
          }
        bOK &= ExportHelper.EndResourceSnapshot(snapshot);
        EditorManager.Progress.Percentage += fProgressStep;


        // close everything, trigger callbacks
        bOK &= ExportHelper.EndExport(info);
        ExportHelper.TriggerEndExportCallbacks(info);
        EditorManager.VisibilityBuilder.UseInEngine = bOldUseVisibilityInEngineState; // reset the state

        EditorManager.Progress.StatusString = string.Format("Export finished - cleaning up...");

        // set back position
        this._bForceLocalPosition = false;
        if (bForceLocalPos)
          this.OnRepositionShapes(null);

        // set back other properties
        foreach (Layer layer in Layers)
        {
          if (!CurrentExportProfile.InvisibleLayersExportInvisibleObjects)
            layer.Root.SetVisible(layer.VisibleFinal, true);
        }
        EditorManager.Progress.HideProgressDialog();

        if (info.UnsupportedShapes.Count > 0)
        {
          ExportReportDlg report = new ExportReportDlg(info.UnsupportedShapes);
          report.Show();
        }
      }
      finally
      {
        CurrentExportProfile.ExportPath = oldExportPath;
      }

      return bOK;
    }

    /// <summary>
    /// Recreates all engine instances.
    /// </summary>
    private void RecreateSceneEntityInstances()
    {
      if (!EditorManager.Settings.RecreateEntitiesOnExport)
        return;
      this.OnRemoveAllEngineInstances();
      this.OnCreateAllEngineInstances(null);
    }

    private List<ShapeBase> restoreVisibility = new List<ShapeBase>();

    public override void HideUnsupportedShapes(int targetPlatform)
    {
      foreach (ShapeBase shape in restoreVisibility)
        shape.SetVisible(true, false);
      restoreVisibility.Clear();

      foreach (Layer layer in Layers)
      {
        Stack<ShapeBase> shapes = new Stack<ShapeBase>();
        shapes.Push(layer.Root);
        while (shapes.Count > 0)
        {
          ShapeBase shape = shapes.Pop();
          if (!shape.SupportsPlatform(targetPlatform))
          {
            if (shape.FinalVisibleState)
              restoreVisibility.Add(shape);

            EditorManager.EngineManager.LogPrintWarning("'" + shape.ShapeName + "' (" + shape.GetType() + ") is not supported on Mobile Platforms");
            shape.SetVisible(false, false);              
          }

          foreach (ShapeBase child in shape.ChildCollection)
            shapes.Push(child);
        }
      }     
    }

    #endregion

    #region Update static lighting


    /// <summary>
    /// Update the static lighting in the scene
    /// </summary>
    /// <param name="bShowDlg">Optionally shows the static lighting dialog</param>
    /// <param name="zones"></param>
    /// <param name="bIncludeMainZone"></param>
    /// <returns></returns>
    public override bool UpdateStaticLighting(bool bShowDlg, ZoneCollection zones, bool bIncludeMainZone)
    {
      if (zones == null)
      {
        zones = this.Zones.ShallowClone();
      }
      else
      {
        // flag default:
        foreach (Zone zone in Zones)
          zone.RunLighting = zones.Contains(zone);
      }

      return EditorManager.LightingTool.RunLighting(bShowDlg, zones, bIncludeMainZone);
    }

    private void CreateBackupFileDeleteOriginal(string sOriginal)
    {
      try
      {
        string sBackup = sOriginal + ".bak";

        // check out both files, if necessary
        ManagedBase.RCS.GetProvider().EditFile(sOriginal);
        ManagedBase.RCS.GetProvider().EditFile(sBackup);

        // reset read-only flag
        if (System.IO.File.Exists(sOriginal))
        {
          System.IO.File.SetAttributes(sOriginal, FileAttributes.Normal);

          if (System.IO.File.Exists(sBackup))
          {
            System.IO.File.SetAttributes(sBackup, FileAttributes.Normal);
          }

          // create backup file
          System.IO.File.Copy(sOriginal, sBackup, true);

          Project.RemoveDataFile(sOriginal);

          // delete original
          System.IO.File.Delete(sOriginal);
        }
      }
      catch (System.Exception ex)
      {
        EditorManager.DumpException(ex);
      }
    }


    public override bool ResetStaticLighting()
    {

      try
      {
        string litname = Path.Combine(this.LayerDirectoryName, "StaticLightingInfo.lit");

        EditorManager.EngineManager.ResetStaticLighting(null, litname); // trigger native callback
        

        string outFilename = AbsoluteFileName;
        string outFilenameExt = Path.GetExtension(outFilename);
        if (outFilenameExt != null)
          outFilename = outFilename.Remove(outFilename.Length - outFilenameExt.Length, outFilenameExt.Length);

        string lgridname = outFilename + ".vlg";
        string slgridname = outFilename + ".vslg";

        CreateBackupFileDeleteOriginal(litname);
        CreateBackupFileDeleteOriginal(lgridname);
        CreateBackupFileDeleteOriginal(slgridname);

        EditorManager.EngineManager.ReloadLightingFiles(outFilename, null);

        // TODO: zones
      }
      catch (Exception ex)
      {

        EditorManager.DumpException(ex);
        return false;
      }

      // trigger the event 
      EditorManager.OnSceneEvent(new SceneEventArgs(SceneEventArgs.Action.StaticLightingReset, true));
      this.Dirty = true;

      // repaint
      UpdateView(false);

      return true;
    }

    #endregion

    #region Update Visibility

    /// <summary>
    /// Runs the visibility builder over the scene
    /// </summary>
    /// <returns></returns>
    public bool UpdateVisibility()
    {
      IVisibilityBuilder builder = EditorManager.VisibilityBuilder;
      if (builder == null)
        return false;

      EditorManager.Progress.ShowProgressDialog("Build visibility");

      bool bResult = builder.Build(this, EditorManager.Progress);

      EditorManager.Progress.HideProgressDialog();

      UpdateView(false); // repaint
      return bResult;
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
