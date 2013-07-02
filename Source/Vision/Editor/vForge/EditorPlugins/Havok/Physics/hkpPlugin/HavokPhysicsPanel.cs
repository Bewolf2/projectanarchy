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
using System.Runtime.Serialization;
using HavokManaged;
using CSharpFramework;
using CSharpFramework.Docking;
using CSharpFramework.Serialization;
using CSharpFramework.Math;

namespace HavokEditorPlugin
{
  public partial class HavokPhysicsPanel : DockableForm
  {
    #region Class - HavokPhysicsPanelSettings

    [Serializable]
    public class HavokPhysicsPanelSettings : ISerializable
    {
      public HavokPhysicsPanelSettings() { }

      #region ISerializable Members

      protected HavokPhysicsPanelSettings(SerializationInfo info, StreamingContext context)
      {
        try
        {
          HavokManaged.WorldSetupSettings ws = new HavokManaged.WorldSetupSettings();
          HavokManaged.WorldRuntimeSettings wr = new HavokManaged.WorldRuntimeSettings();

          // Ensure, that all members of settings have proper default values
          HavokManaged.ManagedModule.ResetWorldSettings();
          HavokManaged.ManagedModule.GetWorldSetupSettings(ws);
          HavokManaged.ManagedModule.GetWorldRuntimeSettings(wr);

          ws.m_havokToVisionScale = (float)info.GetValue("HavokToVisionScale", typeof(float));
          if (SerializationHelper.HasElement(info, "HavokStaticGeomMode"))
            ws.m_staticGeomMode = (int)info.GetValue("HavokStaticGeomMode", typeof(int));
          if (SerializationHelper.HasElement(info, "HavokMergedStaticWeldingType"))
            ws.m_mergedStaticWeldingType = (int)info.GetValue("HavokMergedStaticWeldingType", typeof(int));
          
          wr.m_broadPhaseSizeAuto = (bool)info.GetValue("HavokAutoBroadPhase", typeof(bool));
          wr.m_broadPhaseSizeManual = (float)info.GetValue("HavokManualBroadPhaseSize", typeof(float));
          wr.m_gravityX = (float)info.GetValue("HavokGravityX", typeof(float));
          wr.m_gravityY = (float)info.GetValue("HavokGravityY", typeof(float));
          wr.m_gravityZ = (float)info.GetValue("HavokGravityZ", typeof(float));
          if (SerializationHelper.HasElement(info, "HavokStaticMeshCaching"))
            wr.m_shapeCaching = (bool)info.GetValue("HavokStaticMeshCaching", typeof(bool));

          if (SerializationHelper.HasElement(info, "HavokDisableConstrainedBodiesCollisions"))
          {
            wr.m_disableConstrainedBodiesCollisions = (bool)info.GetValue("HavokDisableConstrainedBodiesCollisions", typeof(bool));
          }
          if (SerializationHelper.HasElement(info, "HavokEnableLegacyCompoundShapes"))
          {
              wr.m_enableLegacyCompoundShapes = (bool)info.GetValue("HavokEnableLegacyCompoundShapes", typeof(bool));
          }

          foreach (HavokCollisionGroups_e value in Enum.GetValues(typeof(HavokCollisionGroups_e)))
          {
            if (SerializationHelper.HasElement(info, value.ToString()))
              wr.m_collisionGroupMasks[(int)value] = (UInt32)info.GetValue(value.ToString(), typeof(UInt32));
          }

          if (SerializationHelper.HasElement(info, "HavokSolverIterations"))
              wr.m_solverIterations = (int)info.GetValue("HavokSolverIterations", typeof(int));

          if (SerializationHelper.HasElement(info, "HavokSolverHardness"))
              wr.m_solverHardness = (int)info.GetValue("HavokSolverHardness", typeof(int));

          HavokManaged.ManagedModule.SetWorldSetupSettings(ws, true);
          HavokManaged.ManagedModule.SetWorldRuntimeSettings(wr);
        }
        catch
        {
        }
      }

      public void GetObjectData(SerializationInfo info, StreamingContext context)
      {
        try
        {
          HavokManaged.WorldSetupSettings ws = new HavokManaged.WorldSetupSettings();
          HavokManaged.WorldRuntimeSettings wr = new HavokManaged.WorldRuntimeSettings();

          HavokManaged.ManagedModule.GetWorldSetupSettings(ws);
          HavokManaged.ManagedModule.GetWorldRuntimeSettings(wr);

          info.AddValue("HavokToVisionScale", ws.m_havokToVisionScale);
          info.AddValue("HavokStaticGeomMode", ws.m_staticGeomMode);
          info.AddValue("HavokMergedStaticWeldingType", ws.m_mergedStaticWeldingType);
          
          info.AddValue("HavokAutoBroadPhase", wr.m_broadPhaseSizeAuto);
          info.AddValue("HavokManualBroadPhaseSize", wr.m_broadPhaseSizeManual);
          info.AddValue("HavokGravityX", wr.m_gravityX);
          info.AddValue("HavokGravityY", wr.m_gravityY);
          info.AddValue("HavokGravityZ", wr.m_gravityZ);
          info.AddValue("HavokStaticMeshCaching", wr.m_shapeCaching);
          info.AddValue("HavokDisableConstrainedBodiesCollisions", wr.m_disableConstrainedBodiesCollisions);
          info.AddValue("HavokEnableLegacyCompoundShapes", wr.m_enableLegacyCompoundShapes);
          foreach (HavokCollisionGroups_e value in Enum.GetValues(typeof(HavokCollisionGroups_e)))
          {
            info.AddValue(value.ToString(), wr.m_collisionGroupMasks[(int)value]);
          }

          info.AddValue("HavokSolverIterations", wr.m_solverIterations);
          info.AddValue("HavokSolverHardness", wr.m_solverHardness);
        }
        catch
        {
        }
      }

      #endregion
    }

    #endregion

    

    private HavokPhysicsSettingsWrapper _settings = null;

    #region Constructor

    public HavokPhysicsPanel(DockingContainer container)
      : base(container)
    {
      InitializeComponent();
      SetupCollisionDataGridView();

      GetHavokPhysicsParams();
      GetWorldRuntimeCollisionParams();
      UpdateStatus();

      EditorManager.EditorModeChanged += new EditorModeChangedEventHandler(EditorManager_EditorModeChanged);
      EditorManager.CustomSceneSerialization += new CustomSceneSerializationEventHandler(EditorManager_CustomSceneSerialization);
      EditorManager.SceneChanged += new SceneChangedEventHandler(EditorManager_SceneChanged);
      IScene.LayerChanged += new LayerChangedEventHandler(IScene_LayerChanged);
    }

    #endregion

    #region IDisposable

    protected override void Dispose(bool disposing)
    {
      EditorManager.EditorModeChanged -= new EditorModeChangedEventHandler(EditorManager_EditorModeChanged);
      EditorManager.CustomSceneSerialization -= new CustomSceneSerializationEventHandler(EditorManager_CustomSceneSerialization);
      EditorManager.SceneChanged -= new SceneChangedEventHandler(EditorManager_SceneChanged);
      IScene.LayerChanged -= new LayerChangedEventHandler(IScene_LayerChanged);

      if (disposing && (components != null))
      {
        components.Dispose();
      }
      base.Dispose(disposing);
    }

    #endregion

    #region Get/SetPhysicsParameter


    protected HavokPhysicsSettingsWrapper.SolverType_e getSolverType( int iters, int hardness )
    {
        if (iters < 4)
        {
            return (hardness < 1) ? HavokPhysicsSettingsWrapper.SolverType_e.Two_Iterations_Soft : ((hardness < 2) ? HavokPhysicsSettingsWrapper.SolverType_e.Two_Iterations_Medium : HavokPhysicsSettingsWrapper.SolverType_e.Two_Iterations_Hard);
        }
        else if (iters < 8)
        {
            return (hardness < 1) ? HavokPhysicsSettingsWrapper.SolverType_e.Four_Iterations_Soft : ((hardness < 2) ? HavokPhysicsSettingsWrapper.SolverType_e.Four_Iterations_Medium : HavokPhysicsSettingsWrapper.SolverType_e.Four_Iterations_Hard);
        } 
        // 8 or higher iters
        return (hardness < 1) ? HavokPhysicsSettingsWrapper.SolverType_e.Eight_Iterations_Soft : ((hardness < 2) ? HavokPhysicsSettingsWrapper.SolverType_e.Eight_Iterations_Medium : HavokPhysicsSettingsWrapper.SolverType_e.Eight_Iterations_Hard);
    }

    protected void getSolverIterations(HavokPhysicsSettingsWrapper.SolverType_e st, ref int iters, ref int hardness)
    {
        switch (st)
        {
            case HavokPhysicsSettingsWrapper.SolverType_e.Two_Iterations_Soft: iters = 2; hardness = 0; break;
            case HavokPhysicsSettingsWrapper.SolverType_e.Two_Iterations_Medium: iters = 2; hardness = 1; break;
            case HavokPhysicsSettingsWrapper.SolverType_e.Two_Iterations_Hard: iters = 2; hardness = 2; break;
            case HavokPhysicsSettingsWrapper.SolverType_e.Four_Iterations_Soft: iters = 4; hardness = 0; break;
            default: case HavokPhysicsSettingsWrapper.SolverType_e.Four_Iterations_Medium: iters = 4; hardness = 1; break;
            case HavokPhysicsSettingsWrapper.SolverType_e.Four_Iterations_Hard: iters = 4; hardness = 2; break;
            case HavokPhysicsSettingsWrapper.SolverType_e.Eight_Iterations_Soft: iters = 8; hardness = 0; break;
            case HavokPhysicsSettingsWrapper.SolverType_e.Eight_Iterations_Medium: iters = 8; hardness = 1; break;
            case HavokPhysicsSettingsWrapper.SolverType_e.Eight_Iterations_Hard: iters = 8; hardness = 2; break;
        }
    }

    protected void GetHavokPhysicsParams()
    {
      dialogCaptionBar.Description = "Version: " + HavokManaged.ManagedModule.GetVersionInfo();

      // Get settings
      HavokManaged.WorldSetupSettings ws = new HavokManaged.WorldSetupSettings();
      HavokManaged.ManagedModule.GetWorldSetupSettings(ws);
      HavokManaged.WorldRuntimeSettings wr = new HavokManaged.WorldRuntimeSettings();
      HavokManaged.ManagedModule.GetWorldRuntimeSettings(wr);

      _settings = new HavokPhysicsSettingsWrapper();
      _settings.VisionUnitsNoPrompt = ws.m_havokToVisionScale;
      _settings.StaticGeometryModeNoPrompt = (HavokPhysicsSettingsWrapper.StaticGeometryMode_e) ws.m_staticGeomMode;
      _settings.MergedStaticWeldingTypeNoPrompt = (HavokPhysicsSettingsWrapper.MergedStaticWeldingType_e) ws.m_mergedStaticWeldingType;
      _settings.BroadphaseSize = wr.m_broadPhaseSizeAuto ? HavokPhysicsSettingsWrapper.BroadphaseSize_e.BoundAllStaticMeshes : HavokPhysicsSettingsWrapper.BroadphaseSize_e.Manual;
      _settings.BroadphaseSizeManual = wr.m_broadPhaseSizeManual;
      _settings.Gravity = new Vector3F(wr.m_gravityX, wr.m_gravityY, wr.m_gravityZ);
      _settings.ConstrainedBodyCollision = !wr.m_disableConstrainedBodiesCollisions;
      _settings.LegacyCompoundShapes = wr.m_enableLegacyCompoundShapes;
      _settings.ShapeCaching = wr.m_shapeCaching;
      _settings.SolverType = getSolverType(wr.m_solverIterations, wr.m_solverHardness);
      
      // Update property grid
      PropertyGrid.SelectedObject = _settings;
    }

    protected void SetHavokPhysicsParams()
    {
      HavokManaged.WorldSetupSettings ws = new HavokManaged.WorldSetupSettings();
      HavokManaged.ManagedModule.GetWorldSetupSettings(ws);
      HavokManaged.WorldRuntimeSettings wr = new HavokManaged.WorldRuntimeSettings();
      HavokManaged.ManagedModule.GetWorldRuntimeSettings(wr);

      ws.m_havokToVisionScale = _settings.VisionUnitsNoPrompt;
      ws.m_staticGeomMode = (int)_settings.StaticGeometryModeNoPrompt;
      ws.m_mergedStaticWeldingType = (int)_settings.MergedStaticWeldingTypeNoPrompt;
      wr.m_broadPhaseSizeAuto = _settings.BroadphaseSize == HavokPhysicsSettingsWrapper.BroadphaseSize_e.BoundAllStaticMeshes;
      wr.m_broadPhaseSizeManual = _settings.BroadphaseSizeManual;
      wr.m_gravityX = _settings.Gravity.X;
      wr.m_gravityY = _settings.Gravity.Y;
      wr.m_gravityZ = _settings.Gravity.Z;
      wr.m_disableConstrainedBodiesCollisions = !_settings.ConstrainedBodyCollision;
      wr.m_enableLegacyCompoundShapes = _settings.LegacyCompoundShapes;
      wr.m_shapeCaching = _settings.ShapeCaching;
      getSolverIterations(_settings.SolverType, ref wr.m_solverIterations, ref wr.m_solverHardness);
     
      HavokManaged.ManagedModule.SetWorldSetupSettings(ws, false);
      HavokManaged.ManagedModule.SetWorldRuntimeSettings(wr);
      
      // Enable saving
      if (EditorManager.Scene != null && EditorManager.Scene.MainLayer != null)
        EditorManager.Scene.MainLayer.Dirty = true;

      // Physics
      HavokManaged.ManagedModule.EnableDebugRendering(_settings.VisualizeDynamicObjects, _settings.VisualizeRagdolls, _settings.VisualizeCharacterControllers, _settings.VisualizeTriggerVolumes, _settings.VisualizeStaticObjects);
      EditorManager.ActiveView.UpdateView(false);
      PropertyGrid.Refresh();
      toolStripButton_VisDynamics.Checked = _settings.VisualizeDynamicObjects;
      toolStripButton_VisController.Checked = _settings.VisualizeCharacterControllers;
      toolStripButton_VisTrigger.Checked = _settings.VisualizeTriggerVolumes;
      toolStripButton_VisStatic.Checked = _settings.VisualizeStaticObjects;
    }

    protected void GetWorldRuntimeCollisionParams()
    {
      HavokManaged.WorldRuntimeSettings wr = new HavokManaged.WorldRuntimeSettings();
      HavokManaged.ManagedModule.GetWorldRuntimeSettings(wr);

      int iRow = 0;
      foreach (HavokCollisionGroups_e rowValue in Enum.GetValues(typeof(HavokCollisionGroups_e)))
      {
        UInt32 iMask = wr.m_collisionGroupMasks[(int)rowValue];
        int iCell = 0;
        foreach (HavokCollisionGroups_e columnValue in Enum.GetValues(typeof(HavokCollisionGroups_e)))
        {
          int iBit = 1 << ((int)columnValue);
          collisionDataGridView.Rows[iRow].Cells[iCell].Value = iMask & iBit;
          iCell++;
        }
        iRow++;
      }
    }
    
    protected void SetWorldRuntimeCollisionParams()
    {
      HavokManaged.WorldRuntimeSettings wr = new HavokManaged.WorldRuntimeSettings();
      HavokManaged.ManagedModule.GetWorldRuntimeSettings(wr);

      int iRow = 0;
      foreach (HavokCollisionGroups_e rowValue in Enum.GetValues(typeof(HavokCollisionGroups_e)))
      {
        int iCell = 0;
        int iMask = (int)wr.m_collisionGroupMasks[(int)rowValue];
        foreach (HavokCollisionGroups_e columnValue in Enum.GetValues(typeof(HavokCollisionGroups_e)))
        {
          if ((Boolean)collisionDataGridView.Rows[iRow].Cells[iCell].EditedFormattedValue)
            iMask |= 1 << ((int)columnValue);
          else
            iMask &= ~(1 << ((int)columnValue));
          iCell++;
        }
        iRow++;
        wr.m_collisionGroupMasks[(int)rowValue] = (UInt32)iMask;
      }

      HavokManaged.ManagedModule.SetWorldRuntimeSettings(wr);

      // enable saving
      if (EditorManager.Scene != null && EditorManager.Scene.MainLayer != null)
        EditorManager.Scene.MainLayer.Dirty = true;
    }

    #endregion

    #region Helper

    private void UpdateStatus()
    {
      bool bIsSimulating = (EditorManager.EditorMode != EditorManager.Mode.EM_NONE);
      bool bHasScene = EditorManager.Scene != null;
      bool bHasMainLayer = bHasScene && EditorManager.Scene.MainLayer != null;
      bool bMainLayerLockedByUser = false;

      if (bHasMainLayer && (EditorManager.Scene.MainLayer.LockStatus == Layer.LayerLockStatus_e.LockedByUser))
        bMainLayerLockedByUser = true;

      PropertyGrid.Enabled = bHasScene && bMainLayerLockedByUser && !bIsSimulating;
      ToolStrip.Enabled = bHasScene;
      this.collisionDataGridView.Enabled = bHasScene && bMainLayerLockedByUser && !bIsSimulating;

      if (HavokManaged.ManagedModule.IsSimulationRunning())
      {
        toolStripButton_Play.Image = global::HavokEditorPlugin.Properties.Resources.physics_pause;
        toolStripButton_Play.ToolTipText = "Pause Simulation";
      }
      else
      {
        toolStripButton_Play.Image = global::HavokEditorPlugin.Properties.Resources.physics_play;
        toolStripButton_Play.ToolTipText = "Play Simulation";
      }

      toolStripButton_VisStatic.Checked = _settings.VisualizeStaticObjects;
      toolStripButton_VisDynamics.Checked = _settings.VisualizeDynamicObjects;
      toolStripButton_VisController.Checked = _settings.VisualizeCharacterControllers;
      toolStripButton_VisTrigger.Checked = _settings.VisualizeTriggerVolumes;
    }

    private void SetupCollisionDataGridView()
    {
      int iIndex = 0;
      foreach (HavokCollisionGroups_e value in Enum.GetValues(typeof(HavokCollisionGroups_e)))
      {
        // add columns
        DataGridViewCheckBoxColumn column = new DataGridViewCheckBoxColumn();
        column.CellTemplate = new DataGridViewCheckBoxCell();
        column.CellTemplate.Style.BackColor = ((iIndex % 2) == 0) ? Color.White : Color.LightGray;
        column.HeaderText = value.ToString();
        collisionDataGridView.Columns.Add(column);

        // add rows
        DataGridViewRow row = new DataGridViewRow();
        row.HeaderCell.Value = value.ToString();
        collisionDataGridView.Rows.Add(row);

        iIndex++;
      }
    }

    #endregion

    #region Custom Events

    void EditorManager_CustomSceneSerialization(object sender, CustomSceneSerializationArgs e)
    {
      if (e.CustomSceneObjects.IsSaving)
      {
        // get data from panel
        HavokPhysicsPanelSettings physSettings = new HavokPhysicsPanelSettings();
        e.CustomSceneObjects.AddObject("HavokEditorPlugin.EditorPlugin", physSettings);
      }
      else
      {
        HavokPhysicsPanelSettings physSettings = 
          (HavokPhysicsPanelSettings)e.CustomSceneObjects.FindObject("HavokEditorPlugin.EditorPlugin", typeof(HavokPhysicsPanelSettings));
        if (physSettings != null)
        {
          GetHavokPhysicsParams();
          GetWorldRuntimeCollisionParams();
        }
      }
    }

    void EditorManager_EditorModeChanged(object sender, EditorModeChangedArgs e)
    {
      GetWorldRuntimeCollisionParams(); // refresh grid reflecting any changes to the physics module collision filter
      UpdateStatus();
    }

    void EditorManager_SceneChanged(object sender, SceneChangedArgs e)
    {
      GetHavokPhysicsParams();
      UpdateStatus();
    }

    public void IScene_LayerChanged(object sender, LayerChangedArgs e)
    {
      if (e.action == LayerChangedArgs.Action.LockStatusChanged)
        UpdateStatus();
    }

    #endregion

    #region UI Events

    private void CollisionDataGridView_CellContentClicked(object sender, DataGridViewCellEventArgs e)
    {
      collisionDataGridView.EndEdit();
      if (e.ColumnIndex >= 0 && e.RowIndex >= 0)
      {
        // toggle corresponding counterpart, since collision has to be dis-/enabled in both collision group bitmasks
        collisionDataGridView.Rows[e.ColumnIndex].Cells[e.RowIndex].Value = collisionDataGridView.Rows[e.RowIndex].Cells[e.ColumnIndex].Value;

        SetWorldRuntimeCollisionParams();
      }
    }

    private void CollisionDataGridView_CellContentDoubleClicked(object sender, DataGridViewCellEventArgs e)
    {
      CollisionDataGridView_CellContentClicked(sender, e);
    }

    private void disableConstrainedBodiesCollisionsCheckBox_CheckedChanged(object sender, EventArgs e)
    {
      SetWorldRuntimeCollisionParams();
    }

    private void disableConstrainedBodiesCollisionsCheckBox_CheckStateChanged(object sender, EventArgs e)
    {
      SetWorldRuntimeCollisionParams();
    }

    private void PropertyGrid_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
    {
      SetHavokPhysicsParams();
    }
  
    private void toolStripButton_VisDynamics_Click(object sender, EventArgs e)
    {
      _settings.VisualizeDynamicObjects = toolStripButton_VisDynamics.Checked;
      PropertyGrid.SelectedObject = _settings;
      HavokManaged.ManagedModule.EnableDebugRendering(_settings.VisualizeDynamicObjects, _settings.VisualizeRagdolls, _settings.VisualizeCharacterControllers, _settings.VisualizeTriggerVolumes, _settings.VisualizeStaticObjects);
      EditorManager.ActiveView.UpdateView(false);
    }

    private void toolStripButton_VisStatic_Click(object sender, EventArgs e)
    {
      _settings.VisualizeStaticObjects = toolStripButton_VisStatic.Checked;
      PropertyGrid.SelectedObject = _settings;
      HavokManaged.ManagedModule.EnableDebugRendering(_settings.VisualizeDynamicObjects, _settings.VisualizeRagdolls, _settings.VisualizeCharacterControllers, _settings.VisualizeTriggerVolumes, _settings.VisualizeStaticObjects);
      EditorManager.ActiveView.UpdateView(false);
    }

    private void toolStripButton_VisController_Click(object sender, EventArgs e)
    {
      _settings.VisualizeCharacterControllers = toolStripButton_VisController.Checked;
      PropertyGrid.SelectedObject = _settings;
      HavokManaged.ManagedModule.EnableDebugRendering(_settings.VisualizeDynamicObjects, _settings.VisualizeRagdolls, _settings.VisualizeCharacterControllers, _settings.VisualizeTriggerVolumes, _settings.VisualizeStaticObjects);
      EditorManager.ActiveView.UpdateView(false);
    }

    private void toolStripButton_VisTrigger_Click(object sender, EventArgs e)
    {
      _settings.VisualizeTriggerVolumes = toolStripButton_VisTrigger.Checked;
      PropertyGrid.SelectedObject = _settings;
      HavokManaged.ManagedModule.EnableDebugRendering(_settings.VisualizeDynamicObjects, _settings.VisualizeRagdolls, _settings.VisualizeCharacterControllers, _settings.VisualizeTriggerVolumes, _settings.VisualizeStaticObjects);
      EditorManager.ActiveView.UpdateView(false);
    }

    private void toolStripButton_Play_Click(object sender, EventArgs e)
    {
      HavokManaged.ManagedModule.ToggleSimulation();
      UpdateStatus();
    }

    #endregion
   
  }

  public class HavokPhysicsSettingsWrapper : ICustomTypeDescriptor
  {
    #region Enums 

    public enum StaticGeometryMode_e
    {
      SingleInstances,
      MergedInstances
    }

    public enum MergedStaticWeldingType_e
    {
      None,
      Anticlockwise,
      Clockwise,
      Twosided
    }

    public enum BroadphaseSize_e
    {
      BoundAllStaticMeshes,
      Manual
    }

    public enum SolverType_e
    {
      Two_Iterations_Soft,
      Two_Iterations_Medium,
      Two_Iterations_Hard,
      Four_Iterations_Soft,
      Four_Iterations_Medium,
      Four_Iterations_Hard,
      Eight_Iterations_Soft,
      Eight_Iterations_Medium,
      Eight_Iterations_Hard
    }

    #endregion

    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_GENERAL = "General";
    protected const string CAT_WORLDSETUP = "World Setup (Reload scene to apply changes)";
    protected const string CAT_WORLDRUNTIME = "World Runtime (Restart simulation to apply changes)";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_GENERAL = 0;
    protected const int CATORDER_WORLDSETUP = 1;
    protected const int CATORDER_WORLDRUNTIME = 2;

    /// <summary>
    /// Last used category ID
    /// </summary>
    public const int LAST_CATEGORY_ORDER_ID = CATORDER_WORLDRUNTIME;

    #endregion

    #region Properties
    
    [SortedCategory(CAT_GENERAL, CATORDER_GENERAL), PropertyOrder(1)]
    [DisplayName("Visualize Dynamic Objects"), Description("If enabled, the collision geometry of dynamic objects will be rendered as a green wireframe.")]
    public bool VisualizeDynamicObjects
    {
      get { return _visualizeDynamicObjects; }
      set { _visualizeDynamicObjects = value; }
    }

    [SortedCategory(CAT_GENERAL, CATORDER_GENERAL), PropertyOrder(1)]
    [DisplayName("Visualize Ragdolls"), Description("If enabled, the collision geometry of ragdolls will be rendered as a green wireframe.")]
    public bool VisualizeRagdolls
    {
        get { return _visualizeRagdolls; }
        set { _visualizeRagdolls = value; }
    }

    [SortedCategory(CAT_GENERAL, CATORDER_GENERAL), PropertyOrder(2)]
    [DisplayName("Visualize Static Objects"), Description("If enabled, the collision geometry of static objects will be rendered as a purple wireframe.")]
    public bool VisualizeStaticObjects
    {
      get { return _visualizeStaticObjects; }
      set { _visualizeStaticObjects = value; }
    }

    [SortedCategory(CAT_GENERAL, CATORDER_GENERAL), PropertyOrder(3)]
    [DisplayName("Visualize Character Controllers"), Description("If enabled, the collision geometry of character controllers will be rendered as a red wireframe.")]
    public bool VisualizeCharacterControllers
    {
      get { return _visualizeCharacterControllers; }
      set { _visualizeCharacterControllers = value; }
    }

    [SortedCategory(CAT_GENERAL, CATORDER_GENERAL), PropertyOrder(4)]
    [DisplayName("Visualize Trigger Volumes"), Description("If enabled, the collision geometry of trigger volumes will be rendered as a blue wireframe.")]
    public bool VisualizeTriggerVolumes
    {
      get { return _visualizeTriggerVolumes; }
      set { _visualizeTriggerVolumes = value; }
    }

    [SortedCategory(CAT_WORLDSETUP, CATORDER_WORLDSETUP), PropertyOrder(1)]
    [DisplayName("Vision Units"), Description("Defines how many vision units define a meter in the world. Note: You must reload the scene for these changes to take effect.")]
    public float VisionUnits
    {
      get { return _visionUnits; }
      set
      {
        if (_visionUnits == value)
          return;

        if (EditorManager.Settings.PromptUserSceneReloadRequired)
        {
          EditorManager.ShowMessageBox("Changes to this property require the scene to be reloaded to take effect.\n You can disable this warning in the vForge settings.",
            "Physics", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }
        _visionUnits = value;
      }
    }

    [Browsable(false)]
    public float VisionUnitsNoPrompt
    {
      get { return _visionUnits; }
      set {_visionUnits = value; }
    }

    [SortedCategory(CAT_WORLDSETUP, CATORDER_WORLDSETUP), PropertyOrder(2)]
    [DisplayName("Static Geometry Mode"), Description("Specifies how the static geometry is represented/ optimized within Havok (SingleInstances: Each static mesh is represented as one Havok rigid body (editable, but slow), MergedInstances: All static meshes in the scene file are baked into one Havok rigid body (not editable, but fast)).")]
    public StaticGeometryMode_e StaticGeometryMode
    {
      get { return _staticGeometryMode; }
      set
      {
        if (_staticGeometryMode == value)
          return;

        if (EditorManager.Settings.PromptUserSceneReloadRequired)
        {
          EditorManager.ShowMessageBox("Changes to this property require the scene to be reloaded to take effect.\n You can disable this warning in the vForge settings.",
            "Physics", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }
        _staticGeometryMode = value;
      }
    }

    [Browsable(false)]
    public StaticGeometryMode_e StaticGeometryModeNoPrompt
    {
      get { return _staticGeometryMode; }
      set { _staticGeometryMode = value; }
    }

    [SortedCategory(CAT_WORLDSETUP, CATORDER_WORLDSETUP), PropertyOrder(3)]
    [DisplayName("Merged Static Welding Type"), Description("Specifies the welding type for merged static mesh instances. Welding is the term used to address the problem of objects bouncing as new contact points are created while transitioning from a collision with one shape to a collision with a neighboring shape.")]
    public MergedStaticWeldingType_e MergedStaticWeldingType
    {
      get { return _mergedStaticWeldingType; }
      set
      {
        if (_mergedStaticWeldingType == value)
          return;

        if (EditorManager.Settings.PromptUserSceneReloadRequired)
        {
          EditorManager.ShowMessageBox("Changes to this property require the scene to be reloaded to take effect.\n You can disable this warning in the vForge settings.",
            "Physics", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }
        _mergedStaticWeldingType = value;
      }
    }

    [Browsable(false)]
    public MergedStaticWeldingType_e MergedStaticWeldingTypeNoPrompt
    {
      get { return _mergedStaticWeldingType; }
      set { _mergedStaticWeldingType = value; }
    }

    [SortedCategory(CAT_WORLDRUNTIME, CATORDER_WORLDRUNTIME), PropertyOrder(1)]
    [DisplayName("Broadphase Size Mode"), Description("Bound all static meshes: The broadphase size will be automatically created from the bounding volumes of all static physics objects. Manual: The broadphase size will be set to value specified in Broadphase Manual Size.")]
    public BroadphaseSize_e BroadphaseSize
    {
      get { return _broadphaseSize; }
      set { _broadphaseSize = value; }
    }

    [SortedCategory(CAT_WORLDRUNTIME, CATORDER_WORLDRUNTIME), PropertyOrder(2)]
    [DisplayName("Broadphase Manual Size"), Description("Sets the manual broadphase size(a cube centered on the origin with the specified value as side length).")]
    public float BroadphaseSizeManual
    {
      get { return _broadphaseSizeManual; }
      set { _broadphaseSizeManual = Math.Min(value, 1e21f); }
    }

    [SortedCategory(CAT_WORLDRUNTIME, CATORDER_WORLDRUNTIME), PropertyOrder(3)]
    [DisplayName("Constrained Body Collision"), Description("Enable collisions between rigid bodies, which are linked by the same constraint.")]
    public bool ConstrainedBodyCollision
    {
      get { return _constrainedBodyCollision; }
      set { _constrainedBodyCollision = value; }
    }

    [SortedCategory(CAT_WORLDRUNTIME, CATORDER_WORLDRUNTIME), PropertyOrder(4)]
    [DisplayName("Legacy Compound Shapes"), Description("Enable simulation of legacy compound shapes (i.e. extended / compressed mesh shape). Note that on PLAYSTATION(R)3 the legacy shapes are mutually exclusive with the static compound shape / BV compressed mesh shape.")]
    public bool LegacyCompoundShapes
    {
        get { return _legacyCompoundShapes; }
        set { _legacyCompoundShapes = value; }
    }

    [SortedCategory(CAT_WORLDRUNTIME, CATORDER_WORLDRUNTIME), PropertyOrder(5)]
    [DisplayName("Shape Caching"), Description("Enable caching of physics shapes to file.")]
    public bool ShapeCaching
    {
      get { return _shapeCaching; }
      set { _shapeCaching = value; }
    }

    [SortedCategory(CAT_WORLDRUNTIME, CATORDER_WORLDRUNTIME), PropertyOrder(6)]
    [DisplayName("Gravity"), Description("Defines the amount of gravity in Vision units. Note: These changes will take effect at the the start of the next simulation.")]
    public Vector3F Gravity
    {
      get { return _gravity; }
      set { _gravity = value; }
    }

    [SortedCategory(CAT_WORLDRUNTIME, CATORDER_WORLDRUNTIME), PropertyOrder(7)]
    [DisplayName("Solver Type"), Description("By default the solver (so to resolve forces etc.) will be run 4 times with a medium hardness.")]
    public SolverType_e SolverType
    {
        get { return _solverType; }
        set { _solverType = value; }
    }

    private bool _visualizeDynamicObjects = false;
    private bool _visualizeRagdolls = false;
    private bool _visualizeCharacterControllers = false;
    private bool _visualizeTriggerVolumes = false;
    private bool _visualizeStaticObjects = false;
    private float _visionUnits = 100.0f;
    private StaticGeometryMode_e _staticGeometryMode = StaticGeometryMode_e.SingleInstances;
    private MergedStaticWeldingType_e _mergedStaticWeldingType = MergedStaticWeldingType_e.None;
    private BroadphaseSize_e _broadphaseSize = BroadphaseSize_e.BoundAllStaticMeshes;
    private float _broadphaseSizeManual = 0.0f;
    private bool _constrainedBodyCollision = false;
    private bool _legacyCompoundShapes = false;
    private bool _shapeCaching = true;
    private Vector3F _gravity = new Vector3F();
    private SolverType_e _solverType = SolverType_e.Four_Iterations_Medium;
   
    #endregion

    #region ICustomTypeDescriptor Members

    public AttributeCollection GetAttributes()
    {
      return TypeDescriptor.GetAttributes(this, true);
    }

    public string GetClassName()
    {
      return TypeDescriptor.GetClassName(this, true);
    }

    public string GetComponentName()
    {
      return TypeDescriptor.GetClassName(this, true);
    }

    public TypeConverter GetConverter()
    {
      return TypeDescriptor.GetConverter(this, true);
    }

    public EventDescriptor GetDefaultEvent()
    {
      return TypeDescriptor.GetDefaultEvent(this, true);
    }

    public PropertyDescriptor GetDefaultProperty()
    {
      return TypeDescriptor.GetDefaultProperty(this, true);
    }

    public object GetEditor(Type editorBaseType)
    {
      return TypeDescriptor.GetEditor(this, editorBaseType, true);
    }

    public EventDescriptorCollection GetEvents(Attribute[] attributes)
    {
      return TypeDescriptor.GetEvents(this, attributes, true);
    }

    public EventDescriptorCollection GetEvents()
    {
      return TypeDescriptor.GetEvents(this, true);
    }

    public PropertyDescriptorCollection GetProperties(Attribute[] attributes)
    {
      PropertyDescriptorCollection pdc = TypeDescriptor.GetProperties(this, attributes, true);
      PropertyDescriptorCollection filtered = new PropertyDescriptorCollection(new PropertyDescriptor[] { });
      foreach (PropertyDescriptor pd in pdc)
        if (!pd.Name.Equals("BroadphaseSizeManual") || (pd.Name.Equals("BroadphaseSizeManual") && _broadphaseSize == BroadphaseSize_e.Manual))
          filtered.Add(pd);

      return filtered;
    }

    public PropertyDescriptorCollection GetProperties()
    {
      PropertyDescriptorCollection pdc = TypeDescriptor.GetProperties(this, true);
      PropertyDescriptorCollection filtered = new PropertyDescriptorCollection(new PropertyDescriptor[] { });
      foreach (PropertyDescriptor pd in pdc)
        if (!pd.Name.Equals("BroadphaseSizeManual") || (pd.Name.Equals("BroadphaseSizeManual") && _broadphaseSize == BroadphaseSize_e.Manual))
          filtered.Add(pd);

      return filtered;
    }

    public object GetPropertyOwner(PropertyDescriptor pd)
    {
      return this;
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
