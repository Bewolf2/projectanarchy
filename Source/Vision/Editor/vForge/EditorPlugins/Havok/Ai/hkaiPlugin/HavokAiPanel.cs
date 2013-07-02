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
using CSharpFramework.Contexts;
using CSharpFramework.Docking;
using CSharpFramework.Math;
using CSharpFramework.Shapes;
using CSharpFramework.PickHandlers;
using VisionEditorPlugin.Shapes;
using TerrainEditorPlugin.Shapes;
using HavokAiEditorPlugin.Shapes;
#if USE_SPEEDTREE
using Speedtree5EditorPlugin;
using Speedtree6EditorPlugin;
#endif

using System.Collections; // for arraylist
using System.IO; // for load/save
using CSharpFramework.Serialization; // for (de)serialization
using System.Runtime.Serialization.Formatters.Binary; // for (de)serialization

using CSharpFramework.View; // for some icons
using CSharpFramework.UndoRedo; // for modification notification
using CSharpFramework.Scene; // for modification notification

namespace HavokAiPanelDialogs
{
  public partial class HavokAiPanel : DockableForm, ISceneObject
  {
    #region Constructor

    public HavokAiPanel(DockingContainer container)
      : base(container)
    {
      InitializeComponent();

      EditorManager.Actions.ActionEvent += Actions_ActionEvent;
      EditorManager.SceneEvent += EditorManager_SceneEvent;
      EditorManager.SceneChanged += EditorManager_SceneChanged;
      EditorManager.CustomSceneSerialization += EditorManager_CustomSceneSerialization;

      m_imageList.ImageList.ImageSize = new System.Drawing.Size(16, 16);
      m_imageList.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_navmeshlocalsettings.png"), Color.Magenta);  // use local settings icon for now.
      this.GlobalSettings_ListView.SmallImageList = m_imageList.ImageList;
    }

    #endregion

    #region IDisposable

    protected override void Dispose(bool disposing)
    {
      EditorManager.Actions.ActionEvent -= Actions_ActionEvent;
      EditorManager.SceneEvent -= EditorManager_SceneEvent;
      EditorManager.SceneChanged -= EditorManager_SceneChanged;
      EditorManager.CustomSceneSerialization -= EditorManager_CustomSceneSerialization;

      if (disposing && (components != null))
      {
        components.Dispose();
      }
      base.Dispose(disposing);
    }

    #endregion

    #region Scene handler events

    private void Actions_ActionEvent(object sender, ActionEventArgs e)
    {
      IAction eventAction = e.action;
      if ((eventAction is AddNavMeshGlobalSettingsAction) 
        || (eventAction is CreateNavMeshGlobalSettingsAction) 
        || (eventAction is DeleteNavMeshGlobalSettingsAction)
        || (eventAction is NavMeshGlobalSettingsGroupAction))
      {
        switch (e.state)
        {
          case ActionEventArgs.State.EndAdd:
          case ActionEventArgs.State.EndRedo:
          case ActionEventArgs.State.EndUndo:
            {
              UpdateSettingsListView();
              this.Modified = true;
            }
            break;
        }
      }
    }

    void EditorManager_SceneEvent(object sender, SceneEventArgs e)
    {
      if (e.action == SceneEventArgs.Action.AfterClosing)
      {
        // clear out the settings collection
        m_settingsDictionary = null;
        UpdateSettingsListView();
      }
      else if (e.action == SceneEventArgs.Action.BeforeExport)
      {
        ExportSceneEventArgs exportEventArgs = e as ExportSceneEventArgs;
        if (tsb_Build.ToolTipText == BUILD_DIRTY_STR)
        {
          
        }
      }
    }

    void EditorManager_SceneChanged(object sender, SceneChangedArgs e)
    {
      if (e.newscene != null)
      {
        if (m_settingsCreatedDuringSceneLoad)
        {
          // notify main layer that new settings objects were created during scene load
          Modified = true;
          m_settingsCreatedDuringSceneLoad = false;
        }
        UpdateSettingsListView();
        UpdateConnectPhysicsButton();
      }
      // Reset property grid
      GlobalSettings_PropertyGridEx1.SelectedObject = null;       
    }

    private void EditorManager_CustomSceneSerialization(object sender, CustomSceneSerializationArgs e)
    {
      if (e.CustomSceneObjects.IsSaving)
      {
        if (m_settingsDictionary != null)
        {
          e.CustomSceneObjects.AddObject("HavokAiEditorPlugin.EditorPlugin", m_settingsDictionary);
        }
      }
      else
      {
        if (m_settingsDictionary != null)
        {
          m_settingsCreatedDuringSceneLoad = true;
        }

        HavokNavMeshGlobalSettingsDictionary serializedSettingsCollection =
          (HavokNavMeshGlobalSettingsDictionary)e.CustomSceneObjects.FindObject("HavokAiEditorPlugin.EditorPlugin", typeof(HavokNavMeshGlobalSettingsDictionary));
        if (serializedSettingsCollection != null)
        {
          // replace with serialized copy.
          m_settingsDictionary = serializedSettingsCollection;
          m_settingsDictionary.Parent = this;
        }
      }
    }

    #endregion

    #region Dynamic Nav Mesh cutting checkbox

    public bool WantPhysicsConnection()
    {
      HavokNavMeshGlobalSettings settings = GetOrCreateDefaultGlobalSettings();
      return settings.ConnectToPhysics;
    }

    private void ConnectPhysicsCheckButton_Click(object sender, EventArgs e)
    {
      HavokNavMeshGlobalSettings settings = GetOrCreateDefaultGlobalSettings();
      settings.ConnectToPhysics = tsb_ConnectPhysics.Checked;
      Modified = true;
      // Also apply to engine directly if running.
      if (EditorManager.InPlayingMode)
      {
        HavokAiManaged.ManagedModule.SetConnectToPhysicsWorld(settings.ConnectToPhysics);
      }
      this.GlobalSettings_PropertyGridEx1.Refresh();
    }

    private void UpdateConnectPhysicsButton()
    {
      HavokNavMeshGlobalSettings settings = GetOrCreateDefaultGlobalSettings();
      tsb_ConnectPhysics.Checked = settings.ConnectToPhysics;
    }

    #endregion

    #region Build Nav Mesh Button

    const string BUILD_DIRTY_STR = "Build Havok AI Nav Mesh (currently out of sync with the settings)";
    const string BUILD_CLEAN_STR = "Build Havok AI Nav Mesh (currently in sync with the settings)";

    public void EnableBuildButtonAsterisk(bool enabled)
    {
      if (enabled)
      {
        tsb_Build.ToolTipText = BUILD_DIRTY_STR;
        tsb_Build.Image = global::HavokAiEditorPlugin.Properties.Resources.havok_ai_navmesh_notinsync;
      }
      else
      {
        tsb_Build.ToolTipText = BUILD_CLEAN_STR;
        tsb_Build.Image = global::HavokAiEditorPlugin.Properties.Resources.havok_ai_navmesh_insync;
      }
    }

    /// <summary>
    /// Build nav mesh button
    /// </summary>
    private void BuildNavMesh_Click(object sender, EventArgs e)
    {
      CSharpFramework.Scene.ZoneCollection zones = EditorManager.Scene.Zones.ShallowClone();
      GroupAction groupLoadAction = new GroupAction("Load all zones");

      
      foreach (CSharpFramework.Scene.Zone zone in zones)
        groupLoadAction.Add(new CSharpFramework.Actions.SetZoneLoadedStatusAction(zone, true));
      groupLoadAction.Do();

      {
        // for printing out stats
        BuildStatisticsRichTextBox.ScrollBars = RichTextBoxScrollBars.ForcedVertical;
        Font oldFont = BuildStatisticsRichTextBox.SelectionFont;
        Font newFontPlain = new Font(oldFont, oldFont.Style & ~FontStyle.Bold);
        Font newFontBold = new Font(oldFont, oldFont.Style | FontStyle.Bold);

        ShapeCollection navMeshShapes = EditorManager.Scene.AllShapesOfType(typeof(HavokNavMeshShape));

        // gather all geometry once (divide by zone later)
        ShapeCollection staticGeometries = new ShapeCollection();
        {
          int numEntities = 0, numStaticMeshes = 0, numTerrains = 0;
          int numCarvers = 0, numSeedPoints = 0, numLocalSettings = 0;
          gatherGeometricShapes(ref staticGeometries, ref numEntities, ref numStaticMeshes, ref numTerrains, ref numCarvers, ref numSeedPoints, ref numLocalSettings);

          //
          // print out some debug info
          //
          BuildStatisticsRichTextBox.SelectionStart = BuildStatisticsRichTextBox.Text.Length;
          {
            String inputGeometryLabel = "Input Geometry";
            BuildStatisticsRichTextBox.Text = inputGeometryLabel;
          }
          BuildStatisticsRichTextBox.SelectionLength = BuildStatisticsRichTextBox.Text.Length - BuildStatisticsRichTextBox.SelectionStart;
          BuildStatisticsRichTextBox.SelectionFont = newFontBold;

          {
            String inputGlobalGeometryInfo = "\n\nStatic meshes\t: " + numStaticMeshes + "\nTerrains\t\t: " + numTerrains + "\nEntities\t\t: " + numEntities +
                "\n\nCarvers\t\t: " + numCarvers + "\nSeed points\t: " + numSeedPoints + "\nLocal settings\t: " + numLocalSettings + "\n\n";
            BuildStatisticsRichTextBox.Text += inputGlobalGeometryInfo;
          }
          BuildStatisticsRichTextBox.SelectionLength = BuildStatisticsRichTextBox.Text.Length - BuildStatisticsRichTextBox.SelectionStart;
          BuildStatisticsRichTextBox.SelectionFont = newFontPlain;

          //
          // debug info end
          //
        }

        // actually build the navmeshes here
        int numBuiltNavMeshShapes = 0;
        bool allCompleted = true;
        foreach (HavokNavMeshShape shape in navMeshShapes)
        {
          int numGeometryVertices = 0, numGeometryTriangles = 0;

          // note that the build function only uses static geometries that lie in the same zone!
          bool built = shape.Build(staticGeometries, ref numGeometryVertices, ref numGeometryTriangles);

          if (built)
          {
            numBuiltNavMeshShapes++;

            //
            // print out some debug info
            //
            BuildStatisticsRichTextBox.SelectionStart = BuildStatisticsRichTextBox.Text.Length;
            {
              String navMeshLabel = "\n\nNav Mesh #" + numBuiltNavMeshShapes;
              BuildStatisticsRichTextBox.Text += navMeshLabel;
            }
            BuildStatisticsRichTextBox.SelectionLength = BuildStatisticsRichTextBox.Text.Length - BuildStatisticsRichTextBox.SelectionStart;
            BuildStatisticsRichTextBox.SelectionFont = newFontBold;

            BuildStatisticsRichTextBox.SelectionStart = BuildStatisticsRichTextBox.Text.Length;
            {
              String inputPerNavMeshGeometryInfo = "\nTotal input triangles\t: " + numGeometryTriangles + "\nTotal input vertices\t: " + numGeometryVertices + "\n\n";
              int facesSize = shape.GetNavMeshFaceSize() * shape.GetNumNavMeshFaces();
              int edgesSize = shape.GetNavMeshEdgeSize() * shape.GetNumNavMeshEdges();
              int verticesSize = shape.GetNavMeshVertexSize() * shape.GetNumNavMeshVertices();
              int totalSize = shape.GetNavMeshStructSize() + facesSize + edgesSize + verticesSize;

              String navMeshInfo = "\nTotal size\t\t: " + totalSize +
                  " bytes\nFaces ( " + shape.GetNumNavMeshFaces() + " )\t: " + facesSize +
                  "\nEdges ( " + shape.GetNumNavMeshEdges() + " )\t: " + edgesSize +
                  "\nVertices ( " + shape.GetNumNavMeshVertices() + " )\t: " + verticesSize;
              BuildStatisticsRichTextBox.Text += navMeshInfo;
            }
            BuildStatisticsRichTextBox.SelectionLength = BuildStatisticsRichTextBox.Text.Length - BuildStatisticsRichTextBox.SelectionStart;
            BuildStatisticsRichTextBox.SelectionFont = newFontPlain;

            //
            // debug info end
            //
          }
          else
          {
              allCompleted = false;
              break;
          }
        }

        if (allCompleted)
        {
            // stitch the navmeshes together
            using (HavokAiManaged.EngineInstanceHavokNavMeshLinker linker = new HavokAiManaged.EngineInstanceHavokNavMeshLinker())
            {             
                // collect all navmeshes at once
                foreach (HavokNavMeshShape shape in navMeshShapes)
                {
                    if (shape.HasEngineInstance())
                    {
                        linker.AddNavMeshShape(shape._engineInstance.GetNativeObject());

                        HavokNavMeshGlobalSettings globalSettings = GetGlobalSettings(shape.NavMeshGlobalSettingsKey);
                        if (globalSettings != null)
                        {
                          linker.m_linkEdgeMatchTolerance = globalSettings.LinkEdgeMatchTolerance;
                          linker.m_linkMaxStepHeight = globalSettings.LinkMaxStepHeight;
                          linker.m_linkMaxSeparation = globalSettings.LinkMaxSeparation;
                          linker.m_linkMaxOverhang = globalSettings.LinkMaxOverhang;

                          linker.m_linkCosPlanarAlignmentAngle = (float)Math.Cos(globalSettings.LinkPlanarAlignmentAngle / 180.0f * 3.14159f);
                          linker.m_linkCosVerticalAlignmentAngle = (float)Math.Cos(globalSettings.LinkVerticalAlignmentAngle / 180.0f * 3.14159f);
                          linker.m_linkMinEdgeOverlap = globalSettings.LinkMinEdgeOverlap;
                        }
                    }
                }

                // link them together
                linker.LinkNavMeshes();
            }

            // save it to disk (separate from next loop because we want to guarantee that we don't serialize out some runtime only data)
            foreach (HavokNavMeshShape shape in navMeshShapes)
            {
                shape.SaveNavMeshesToFile();
            }

            // finally load it into the havok ai world 
            foreach (HavokNavMeshShape shape in navMeshShapes)
            {
                shape.AddNavMeshToWorld();
            }

            if (EditorManager.InPlayingMode && WantPhysicsConnection())
            {
                HavokAiManaged.ManagedModule.SetConnectToPhysicsWorld(true);
            }
        }

        EnableBuildButtonAsterisk(false);
        //EnableStreamingDependentControls(shape.GetNumNavMeshes()>1);
      }

      groupLoadAction.Undo();

      EditorManager.ActiveView.UpdateView(true);
    }

    #endregion

    #region Enable/disable button

    public void EnableNavMeshDependentControls(bool enabled)
    {
      tsb_Build.Enabled = enabled;
      tsb_ConnectPhysics.Enabled = enabled;
      BuildStatisticsRichTextBox.Enabled = enabled;

      if (!enabled)
        BuildStatisticsRichTextBox.Text = "<statistics>";
    }

    public void EnablePathDependentControls(bool enabled, bool showPath)
    {
      tsb_TestPathPickStart.Enabled = enabled;
      tsb_TestPathPickEnd.Enabled = enabled;
    }

    #endregion

    #region Build Nav Mesh helper functions

    public static void recursivelyAddShapes(ShapeCollection siblings, ref ShapeCollection shapesOut)
    {
      foreach (ShapeBase shape in siblings)
      {
        recursivelyAddShapes(shape.ChildCollection, ref shapesOut);

        if (shape is StaticMeshShape || shape is EntityShape || shape is TerrainShape
#if USE_SPEEDTREE
            || shape is SpeedTree5GroupShape
            || shape is Speedtree6GroupShape
#endif
        )
        {
          shapesOut.Add(shape);
        }
      }
    }

    public static void gatherGeometricShapes(ref ShapeCollection shapesOut, ref int numEntitiesOut, ref int numStaticMeshesOut, ref int numTerrainsOut, ref int numCarversOut, ref int numSeedPointsOut, ref int numLocalSettingsOut)
    {
      // Grab all shapes from all layers
      foreach (Layer layer in EditorManager.Scene.Layers)
      {
        if (layer.GetIncludeInNavMesh())
        {
          recursivelyAddShapes(layer.Root.ChildCollection, ref shapesOut);
        }
      }

      // calculate some statistics
      foreach (ShapeBase shape in shapesOut)
      {
        // check if the shape to be added is in the same zone as the HavokNavMeshShape's parentZone
        // note that if HavokNavMeshShape is not in a zone, then it will only include other shapes that aren't in zones also.

        Shape3D shape3d = shape as Shape3D;
        Shape3D.eNavMeshUsage usage = shape3d.GetNavMeshUsage();
        if (usage != Shape3D.eNavMeshUsage.ExcludeFromNavMesh)
        {
          if (shape is EntityShape)
          {
            numEntitiesOut++;
          }
          else if (shape is StaticMeshShape)
          {
            numStaticMeshesOut++;
          }
          else if (shape is TerrainShape)
          {
            numTerrainsOut++;
          }
        }
      }

      ShapeCollection carvers = EditorManager.Scene.AllShapesOfType(typeof(HavokNavMeshCarverShape));
      numCarversOut = carvers.Count;

      ShapeCollection seedPoints = EditorManager.Scene.AllShapesOfType(typeof(HavokNavMeshSeedPointShape));
      numSeedPointsOut = seedPoints.Count;

      // Add local settings
      ShapeCollection localSettings = EditorManager.Scene.AllShapesOfType(typeof(HavokNavMeshLocalSettingsShape));
      numLocalSettingsOut = localSettings.Count;
    }

    #endregion

    #region Test path modification

    public class PathPointPickHandler : IPickHandler
    {
      public PathPointPickHandler(bool start) { m_start = start; }

      public override bool OnPick(MouseEventArgs e, CSharpFramework.Contexts.KeyModifier eKeyMod)
      {
        // base implementation releases the pick handler
        base.OnPick(e, eKeyMod);

        ShapeCollection selection = EditorManager.SelectedShapes;
        foreach (ShapeBase shape in selection)
        {
          if (shape is HavokAiEditorPlugin.Shapes.HavokNavMeshTestPathShape)
          {
            Vector3F vStart = new Vector3F();
            Vector3F vEnd = new Vector3F();
            EditorManager.EngineManager.GetRayAtScreenPos(out vStart, out vEnd, e.X, e.Y, EditorManager.Settings.MaxPickingDistance);

            ShapeTraceResult result = new ShapeTraceResult();
            ShapeCollection relevantShapes = EditorManager.Scene.RootShapes;
            foreach (ShapeBase relevantShape in relevantShapes)
              relevantShape.TraceShape(ShapeTraceMode_e.ShapePicking, vStart, vEnd, ref result, true);

            if (result.bHit)
            {
              Vector3F dir = vEnd - vStart;
              dir.Normalize();
              Vector3F hitPoint = vStart + dir * result.fHitDistance;

              HavokAiEditorPlugin.Shapes.HavokNavMeshTestPathShape path = shape as HavokAiEditorPlugin.Shapes.HavokNavMeshTestPathShape;
              if (m_start)
                path.StartPoint = hitPoint;
              else
                path.EndPoint = hitPoint;
            }
            break;
          }
        }

        return true;
      }

      bool m_start;
    }

    private void PickStartPoint_Click(object sender, EventArgs e)
    {
      EditorManager.ActiveView.PickHandler = new PathPointPickHandler(true);
      EditorManager.ActiveView.UpdateView(true);
    }

    private void PickEndPoint_Click(object sender, EventArgs e)
    {
      EditorManager.ActiveView.PickHandler = new PathPointPickHandler(false);
      EditorManager.ActiveView.UpdateView(true);
    }

    private void IgnoreCheckBox_CheckedChanged(object sender, EventArgs e)
    {
      if (m_ignoreCheckBoxChanged)
        return;

      ShapeCollection selection = EditorManager.SelectedShapes;
      foreach (ShapeBase shape in selection)
      {
        if (shape is HavokAiEditorPlugin.Shapes.HavokNavMeshTestPathShape)
        {
          HavokAiEditorPlugin.Shapes.HavokNavMeshTestPathShape path = shape as HavokAiEditorPlugin.Shapes.HavokNavMeshTestPathShape;
          path.Visible = !path.Visible;
          break;
        }
      }
      EditorManager.ActiveView.UpdateView(true);
    }

    #endregion

    #region Nav Mesh Generation Global Settings UI

    /// <summary>
    /// Set New Default Global Settings
    /// </summary>
    private void SetAsDefaultButton_Click(object sender, EventArgs e)
    {
      if (GlobalSettings_ListView.SelectedItems.Count == 1)
      {
        string settingsName = GlobalSettings_ListView.SelectedItems[0].Tag as string;
        System.Diagnostics.Debug.Assert(settingsName != null);
        System.Diagnostics.Debug.Assert(GetGlobalSettings(settingsName) != null);
        SetDefaultGlobalSettings(settingsName);
        UpdateSettingsListView();
      }
    }

    /// <summary>
    /// Create New Global Settings
    /// </summary>
    private void NewSettingsButton_Click(object sender, EventArgs e)
    {
      // create one
      string newSettingsName = GenerateUniqueSettingsName("GlobalSettings");
      CreateGlobalSettings(newSettingsName, true);
    }

    /// <summary>
    // Copy Global Settings
    private void CopySettingsButton_Click(object sender, EventArgs e)
    {
      if (GlobalSettings_ListView.SelectedItems.Count == 1)
      {
        string settingsName = GlobalSettings_ListView.SelectedItems[0].Tag as string;
        System.Diagnostics.Debug.Assert(settingsName != null);
        HavokNavMeshGlobalSettings settingsToBeCopied = GetGlobalSettings(settingsName);
        EditorManager.Clipboard.Data = CSharpFramework.Clipboard.EditorClipboard.CreateDataObject(settingsToBeCopied.Clone(), "Nav Mesh Global Settings");
        PasteSettingsButton.Enabled = true;
      }
    }

    /// <summary>
    /// Paste Global Settings
    /// </summary>
    private void PasteSettingsButton_Click(object sender, EventArgs e)
    {
      HavokNavMeshGlobalSettings newSettings = EditorManager.Clipboard.DataObject as HavokNavMeshGlobalSettings;
      if (newSettings == null)
      {
        return;
      }

      newSettings.Name = GenerateUniqueSettingsName(newSettings.Name + "_Copy");
      AddGlobalSettings(newSettings.Name, newSettings, true);
      PasteSettingsButton.Enabled = false;
    }

    /// <summary>
    /// Delete Global Settings
    /// </summary>
    private void DeleteSettingsButton_Click(object sender, EventArgs e)
    {
      if (GlobalSettings_ListView.SelectedItems.Count == 0)
      {
        // do nothing since no items selected
        return;
      }

      DialogResult Result = EditorManager.ShowMessageBox("Are you sure you want to delete these settings?", "Delete?", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
      if (Result != DialogResult.Yes)
      {
        // delete is canceled
        return;
      }

      string[] settingsToBeDeletedArray = new string[GlobalSettings_ListView.SelectedItems.Count];
      for (int idx = 0; idx < GlobalSettings_ListView.SelectedItems.Count; ++idx)
      {
        settingsToBeDeletedArray[idx] = GlobalSettings_ListView.SelectedItems[idx].Tag as string;
      }
      DeleteGlobalSettings(settingsToBeDeletedArray, true);
    }

    /// <summary>
    /// Import Global Settings
    /// </summary>
    private void ImportSettingsButton_Click(object sender, EventArgs e)
    {
      OpenFileDialog dlg = new OpenFileDialog();
      dlg.InitialDirectory = EditorManager.Project.ProjectDir;
      dlg.FileName = null;
      dlg.Filter = "Nav Mesh Global Settings|*.*";
      dlg.Title = "Load nav mesh global settings from file";

      if (dlg.ShowDialog(this) != DialogResult.OK)
        return;

      string filename = dlg.FileName;
      dlg.Dispose();

      HavokNavMeshGlobalSettings loadedSettings = null;
      try
      {
        BinaryFormatter fmt = SerializationHelper.BINARY_FORMATTER;
        FileStream fs = new FileStream(filename, FileMode.Open, FileAccess.Read);
        loadedSettings = (HavokNavMeshGlobalSettings)fmt.Deserialize(fs);
        fs.Close();
      }
      catch (Exception ex)
      {
        EditorManager.ShowMessageBox("Failed to load nav mesh global settings from file:\n\n" + ex.Message, "Error loading file", MessageBoxButtons.OK, MessageBoxIcon.Error);
      }

      if (loadedSettings != null)
      {
        string settingsName = Path.GetFileNameWithoutExtension(filename);
        loadedSettings.Name = GenerateUniqueSettingsName(settingsName);
        AddGlobalSettings(loadedSettings.Name, loadedSettings, true);
      }
    }

    /// <summary>
    /// Export Global Settings
    /// </summary>
    private void ExportSelectedSettingsButton_Click(object sender, EventArgs e)
    {
      if (GlobalSettings_ListView.SelectedItems.Count == 1)
      {
        string settingsName = GlobalSettings_ListView.SelectedItems[0].Tag as string;
        System.Diagnostics.Debug.Assert(settingsName != null);

        HavokNavMeshGlobalSettings settingsToExported = GetGlobalSettings(settingsName);
        System.Diagnostics.Debug.Assert(settingsToExported != null);

        SaveFileDialog dlg = new SaveFileDialog();
        dlg.InitialDirectory = EditorManager.Project.ProjectDir;
        dlg.FileName = null;
        dlg.Filter = "Nav Mesh Global Settings|*.*";
        dlg.Title = "Save nav mesh global settings to editor format";

        if (dlg.ShowDialog(this) != DialogResult.OK)
          return;

        string filename = dlg.FileName;
        dlg.Dispose();

        try
        {
          BinaryFormatter fmt = SerializationHelper.BINARY_FORMATTER;
          FileStream fs = new FileStream(filename, FileMode.Create);
          fmt.Serialize(fs, settingsToExported);
          fs.Close();
        }
        catch (Exception ex)
        {
          EditorManager.ShowMessageBox("Failed to save nav mesh global settings to file:\n\n" + ex.Message, "Error saving file", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
      }
    }

    /// <summary>
    /// refresh list view of all settings
    /// </summary>
    private void UpdateSettingsListView()
    {
      GlobalSettings_ListView.BeginUpdate();
      GlobalSettings_ListView.Items.Clear();
      if (m_settingsDictionary != null)
      {
        string[] settingsNames = m_settingsDictionary.GetAllSettingsNames();

        string defaultName = m_settingsDictionary.DefaultSettingsName;
        foreach (string name in settingsNames)
        {
          ListViewItem item = null;
          if (name == defaultName)
          {
            string itemName = name + " - Default";
            item = GlobalSettings_ListView.Items.Add(itemName, 0);
          }
          else
          {
            item = GlobalSettings_ListView.Items.Add(name, 0);
          }
          item.Tag = name;
        }
      }
      GlobalSettings_ListView.EndUpdate();
      UpdateSettingsToolStrip();
    }

    public void UpdateSettingsToolStrip()
    {
      bool bHasSelection = GlobalSettings_ListView.SelectedItems != null && GlobalSettings_ListView.SelectedItems.Count > 0;
      DeletedButton.Enabled = bHasSelection;
      CopySettingsButton.Enabled = bHasSelection;
      ExportSelectedSettingsButton.Enabled = bHasSelection;
      SetAsDefaultButton.Enabled = bHasSelection;
    }

    /// <summary>
    /// refresh property grid if different setting is selected
    /// </summary>
    private void GlobalSettingsListView_SelectedIndexChanged(object sender, EventArgs e)
    {
      if (GlobalSettings_ListView.SelectedItems.Count == 0)
      {
        GlobalSettings_PropertyGridEx1.SelectedObject = null;
        GlobalSettings_PropertyGridEx1.SelectedObjects = null;
      }
      else if (GlobalSettings_ListView.SelectedItems.Count == 1)
      {
        GlobalSettings_PropertyGridEx1.SelectedObjects = null;
        string settingsName = GlobalSettings_ListView.SelectedItems[0].Tag as string;
        HavokNavMeshGlobalSettings settings = GetGlobalSettings(settingsName);
        System.Diagnostics.Debug.Assert(settings != null);
        System.Diagnostics.Debug.Assert(settings.Name == settingsName);
        GlobalSettings_PropertyGridEx1.SelectedObject = settings;
      }
      else
      {
        ArrayList globalSettingsArray = new ArrayList(GlobalSettings_ListView.SelectedItems.Count);
        foreach (ListViewItem item in GlobalSettings_ListView.SelectedItems)
        {
          string settingsName = item.Tag as string;
          HavokNavMeshGlobalSettings settings = GetGlobalSettings(settingsName);
          System.Diagnostics.Debug.Assert(settings != null);
          System.Diagnostics.Debug.Assert(settings.Name == settingsName);
          globalSettingsArray.Add(settings);
        }
        GlobalSettings_PropertyGridEx1.SelectedObject = null;
        GlobalSettings_PropertyGridEx1.SelectedObjects = globalSettingsArray.ToArray();
      }

      if (GlobalSettings_PropertyGridEx1.SelectedObject != null || GlobalSettings_PropertyGridEx1.SelectedObjects != null)
      {
        GlobalSettings_PropertyGridEx1.ExpandAllGridItems();
      }

      UpdateSettingsToolStrip();
    }

    private void GlobalSettingsPropertyGrid_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
    {
      UpdateConnectPhysicsButton();
      Modified = true;
    }

    public bool Modifiable { get { return true; } }

    public bool Modified
    {
      set
      {
        if (value)
        {
          EnableBuildButtonAsterisk(true);
          if (EditorManager.Scene != null 
            && EditorManager.Scene.MainLayer != null
            && EditorManager.Scene.MainLayer.Modifiable)
          {
            EditorManager.Scene.MainLayer.Modified = true;
          }
        }
      }
    }

    #endregion

    #region Nav Mesh Generation Global Settings external access

    public static HavokAiPanel GetInstance()
    {
      HavokAiEditorPlugin.EditorPlugin aiPlugin = EditorManager.GetPluginByName("HavokAiEditorPlugin.EditorPlugin") as HavokAiEditorPlugin.EditorPlugin;
      if (aiPlugin != null && aiPlugin.AiPanel != null)
      {
        HavokAiPanelDialogs.HavokAiPanel aiPanel = aiPlugin.AiPanel;
        return aiPanel;
      }
      return null;
    }

    public HavokNavMeshGlobalSettings GetGlobalSettings(string settingsName)
    {
      if (m_settingsDictionary != null)
      {
        HavokNavMeshGlobalSettings settings = m_settingsDictionary.GetSettingsInstance(settingsName);
        System.Diagnostics.Debug.Assert(settings == null || settings.Name == settingsName);
        return settings;
      }
      return null;
    }

    public bool AddGlobalSettings(string settingsName, HavokNavMeshGlobalSettings settings, bool bUndoable)
    {
      if (m_settingsDictionary == null)
      {
        // create a collection if doesn't exist
        m_settingsDictionary = new HavokNavMeshGlobalSettingsDictionary(this);
      }

      if (bUndoable)
      {
        EditorManager.Actions.Add(new AddNavMeshGlobalSettingsAction(m_settingsDictionary, settingsName, settings));
        return m_settingsDictionary.GetSettingsInstance(settingsName) == settings;
      }
      else
      {
        return m_settingsDictionary.AddSettingsInstance(settingsName, settings);
      }
    }

    public HavokNavMeshGlobalSettings CreateGlobalSettings(string settingsName, bool bUndoable)
    {
      if (m_settingsDictionary == null)
      {
        // create a collection if doesn't exist
        m_settingsDictionary = new HavokNavMeshGlobalSettingsDictionary(this);
      }

      if (bUndoable)
      {
        EditorManager.Actions.Add(new CreateNavMeshGlobalSettingsAction(m_settingsDictionary, settingsName));
      }
      else
      {
        m_settingsDictionary.CreateAndAddSettingsInstance(settingsName);
      }
      HavokNavMeshGlobalSettings settings = m_settingsDictionary.GetSettingsInstance(settingsName);
      System.Diagnostics.Debug.Assert(settings == null || settings.Name == settingsName);
      return settings;
    }

    public void DeleteGlobalSettings(string[] settingsNames, bool bUndoable)
    {
      System.Diagnostics.Debug.Assert(m_settingsDictionary != null && settingsNames.Length > 0);

      if (bUndoable)
      {
        if (settingsNames.Length == 1)
        {
          string settingsName = settingsNames[0];
          EditorManager.Actions.Add(new DeleteNavMeshGlobalSettingsAction(m_settingsDictionary, settingsName));
        }
        else
        {
          NavMeshGlobalSettingsGroupAction navMeshGroupAction = new NavMeshGlobalSettingsGroupAction("Delete multiple nav mesh settings");
          foreach (string settingsName in settingsNames)
          {
            navMeshGroupAction.Add(new DeleteNavMeshGlobalSettingsAction(m_settingsDictionary, settingsName));
          }
          EditorManager.Actions.Add(navMeshGroupAction);
        }
      }
      else
      {
        foreach (string settingsName in settingsNames)
        {
          m_settingsDictionary.DeleteSettingsInstance(settingsName);
        }
      }
    }

    public HavokNavMeshGlobalSettings GetOrCreateDefaultGlobalSettings()
    {
      HavokNavMeshGlobalSettings newDefault = null;
      if (m_settingsDictionary != null)
      {
        newDefault = m_settingsDictionary.GetDefaultSettingsInstance();
      }

      if (newDefault == null)
      {
        // if null, settingsCollection is empty.
        System.Diagnostics.Debug.Assert(m_settingsDictionary == null);
        string newDefaultName = "GlobalSettings";
        newDefault = CreateGlobalSettings(newDefaultName, true);
      }
      return newDefault;
    }

    public void SetDefaultGlobalSettings(string settingsName)
    {
      if (m_settingsDictionary != null)
      {
        m_settingsDictionary.SetDefaultSettingsInstance(settingsName);
        UpdateConnectPhysicsButton();
      }
    }

    private string GenerateUniqueSettingsName(string nameBase)
    {
      if (m_settingsDictionary != null)
      {
        return m_settingsDictionary.GenerateUniqueSettingsName(nameBase);
      }
      return nameBase;
    }

    public string[] GetGlobalSettingsNameCollection()
    {
      if (m_settingsDictionary != null)
      {
        return m_settingsDictionary.GetAllSettingsNames();
      }
      return null;
    }

    #endregion

    HavokNavMeshGlobalSettingsDictionary m_settingsDictionary = null;
    private bool m_settingsCreatedDuringSceneLoad = false;
    GUI.DynamicImageList m_imageList = new GUI.DynamicImageList(2);
    private bool m_ignoreCheckBoxChanged = false;
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
