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
using System.ComponentModel.Design;
using System.Reflection;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using CSharpFramework.Docking;
using CSharpFramework;
using TerrainEditorPlugin.Shapes;
using TerrainEditorPlugin.Contexts;
using TerrainBase.Editing;
using CSharpFramework.PropertyEditors;
using System.Collections;
using TerrainEditorPlugin.Controls;
using TerrainManaged;
using CSharpFramework.Dialogs;
using TerrainEditorPlugin.Editing;
using TerrainEditorPlugin.Actions;
using CSharpFramework.View;
using CSharpFramework.Scene;
using TerrainEditorPlugin.Filter;
using System.IO;
using System.Drawing.Design;
using System.Windows.Forms.Design;
using CSharpFramework.Helper;
using CSharpFramework.Clipboard;
using CSharpFramework.PickHandlers;
using CSharpFramework.Shapes;
using CSharpFramework.Math;
using CSharpFramework.Actions;
using System.Runtime.Serialization;
using CSharpFramework.Serialization;
using CSharpFramework.ShortCuts;
using CSharpFramework.AssetManagement;

namespace TerrainEditorPlugin.Dialogs
{
	public partial class TerrainEditorPanel : DockableForm
	{
		public TerrainEditorPanel(DockingContainer container)
			: base(container)
		{
      // actually this has to be done here
      this.brushListCtrl1 = new TerrainEditorPlugin.Controls.BrushListCtrl();
      this.brushListCtrl1.Dock = System.Windows.Forms.DockStyle.Top;
      this.brushListCtrl1.Location = new System.Drawing.Point(0, 254);
      this.brushListCtrl1.Name = "brushListCtrl1";
      this.brushListCtrl1.Size = new System.Drawing.Size(444, 400);
      this.brushListCtrl1.TabIndex = 1;
      //this.Controls.Add(this.brushListCtrl1);
      
      InitializeComponent();

      UpdatePanelStatus();
      UpdateHeightmapPanel();

      this.listView_DetailTextures.SmallImageList = _detailTexturesImageList.ImageList;
      _heightmapCursor = _heightmapCursor_Elevate;

      // add listeners
      EditorManager.SceneEvent += new SceneEventHandler(EditorManager_SceneEvent);
      EditorManager.SceneChanged += new SceneChangedEventHandler(EditorManager_SceneChanged);
      IScene.LayerChanged += new LayerChangedEventHandler(IScene_LayerChanged);
      TerrainEditor.OnTerrainChanged += new EventHandler(TerrainEditor_OnTerrainChanged);
      TerrainEditor.OnHeightmapEditModeChanged += new EventHandler(TerrainEditor_OnHeightmapEditModeChanged);
      TerrainEditor.OnCurrentDecorationModelChanged += new EventHandler(TerrainEditor_OnCurrentDecorationModelChanged);
      TerrainEditor.OnDecorationModelListChanged += new EventHandler(TerrainEditor_OnDecorationModelListChanged);
      TerrainEditor.OnDecorationDirtyRegionChanged += new EventHandler(TerrainEditor_OnDecorationDirtyRegionChanged);
      TerrainEditor.OnCurrentDetailTextureChanged += new EventHandler(TerrainEditor_OnCurrentDetailTextureChanged);
      TerrainEditor.OnDetailTextureListChanged += new DetailTextureListChangedEventHandler(TerrainEditor_OnDetailTextureListChanged);
      TerrainEditor.OnTextureBakedViewChanged += new EventHandler(TerrainEditor_OnTextureBakedViewChanged);
      TerrainEditor.OnDirectionalLightingChanged += new EventHandler(TerrainEditor_OnDirectionalLightingChanged);
      TerrainEditor.OnMiniMapStatusChanged += new EventHandler(TerrainEditor_OnMiniMapStatusChanged);
      TerrainEditor.OnPreviewFullResolutionStatusChanged += new EventHandler(TerrainEditor_OnPreviewFullResolutionStatusChanged);
      TerrainEditor.OnHeightmapExportFilenameChanged += new EventHandler(TerrainEditor_OnHeightmapExportFilenameChanged);
      DetailTextureResource.OnDetailTextureResourcePropertyChanged += new DetailTextureResourcePropertyChangedEventHandler(DetailTextureResource_OnDetailTextureResourcePropertyChanged);
      DecorationModelResource.OnDecorationModelResourcePropertyChanged += new EventHandler(DecorationModelResource_OnDecorationModelResourcePropertyChanged);
      VisionViewBase.MouseContextChanged += new CSharpFramework.Contexts.ContextChangedEventHandler(VisionViewBase_MouseContextChanged);
      TextureAtlas.OnAtlasFileSaved += new EventHandler(TextureAtlas_OnAtlasFileSaved);
      ShapeComponent.OnUIUpdateBegin += new EventHandler(ShapeComponent_OnUIUpdateBegin);
      ShapeComponent.OnUIUpdateFinished += new EventHandler(ShapeComponent_OnUIUpdateFinished);
      EditorManager.CustomSceneSerialization += new CustomSceneSerializationEventHandler(EditorManager_CustomSceneSerialization);
      brushListCtrl1.Dock = DockStyle.Fill;
      brushListCtrl1.AutoSize = true;

      EditorManager.GUI.RecordableControls.Add("Terrain Editor: Toggle baked mode", toolStripButton_TextureBaked);

      this.splitContainer1.Panel2.Controls.Add(brushListCtrl1);
    }

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    protected override void Dispose(bool disposing)
    {
      EditorManager.SceneEvent -= new SceneEventHandler(EditorManager_SceneEvent);
      EditorManager.SceneChanged -= new SceneChangedEventHandler(EditorManager_SceneChanged);
      IScene.LayerChanged -= new LayerChangedEventHandler(IScene_LayerChanged);
      TerrainEditor.OnTerrainChanged -= new EventHandler(TerrainEditor_OnTerrainChanged);
      TerrainEditor.OnHeightmapEditModeChanged -= new EventHandler(TerrainEditor_OnHeightmapEditModeChanged);
      TerrainEditor.OnCurrentDecorationModelChanged -= new EventHandler(TerrainEditor_OnCurrentDecorationModelChanged);
      TerrainEditor.OnDecorationModelListChanged -= new EventHandler(TerrainEditor_OnDecorationModelListChanged);
      TerrainEditor.OnDecorationDirtyRegionChanged -= new EventHandler(TerrainEditor_OnDecorationDirtyRegionChanged);
      TerrainEditor.OnCurrentDetailTextureChanged -= new EventHandler(TerrainEditor_OnCurrentDetailTextureChanged);
      TerrainEditor.OnDetailTextureListChanged -= new DetailTextureListChangedEventHandler(TerrainEditor_OnDetailTextureListChanged);
      TerrainEditor.OnTextureBakedViewChanged -= new EventHandler(TerrainEditor_OnTextureBakedViewChanged);
      TerrainEditor.OnDirectionalLightingChanged -= new EventHandler(TerrainEditor_OnDirectionalLightingChanged);
      TerrainEditor.OnMiniMapStatusChanged -= new EventHandler(TerrainEditor_OnMiniMapStatusChanged);
      TerrainEditor.OnPreviewFullResolutionStatusChanged -= new EventHandler(TerrainEditor_OnPreviewFullResolutionStatusChanged);
      TerrainEditor.OnHeightmapExportFilenameChanged -= new EventHandler(TerrainEditor_OnHeightmapExportFilenameChanged);
      DetailTextureResource.OnDetailTextureResourcePropertyChanged -= new DetailTextureResourcePropertyChangedEventHandler(DetailTextureResource_OnDetailTextureResourcePropertyChanged);
      DecorationModelResource.OnDecorationModelResourcePropertyChanged -= new EventHandler(DecorationModelResource_OnDecorationModelResourcePropertyChanged);
      VisionViewBase.MouseContextChanged -= new CSharpFramework.Contexts.ContextChangedEventHandler(VisionViewBase_MouseContextChanged);
      TextureAtlas.OnAtlasFileSaved -= new EventHandler(TextureAtlas_OnAtlasFileSaved);
      ShapeComponent.OnUIUpdateBegin -= new EventHandler(ShapeComponent_OnUIUpdateBegin);
      ShapeComponent.OnUIUpdateFinished -= new EventHandler(ShapeComponent_OnUIUpdateFinished);
      EditorManager.CustomSceneSerialization -= new CustomSceneSerializationEventHandler(EditorManager_CustomSceneSerialization);
      if (disposing)
      {
        if (components != null)
        {
          components.Dispose();
        }
      }
      base.Dispose(disposing);
    }

    GUI.DynamicImageList _detailTexturesImageList = new GUI.DynamicImageList(32);

    private TerrainEditorPlugin.Controls.BrushListCtrl brushListCtrl1;


    //Change made by Benjamin to access brush for automated test
    public TerrainEditorPlugin.Controls.BrushListCtrl BrushListCtrl1
    {
      get { return brushListCtrl1; }
    }


		private void TerrainEditorPanel_Load(object sender, EventArgs e)
		{

		}

    /// <summary>
    /// Switches back to default context in case a terrain related context is activated
    /// </summary>
    void UnsetTerrainContext()
    {
      if (EditorManager.ActiveView.CurrentContext is TerrainEditingBaseContext)
        EditorManager.ActiveView.SetDefaultContext();
    }

    protected override void OnVisibleChanged(EventArgs e)
    {
      base.OnVisibleChanged(e);

      // disable contexts if necessary:
      if (!Visible)
      {
        UnsetTerrainContext();
      }

    }


		#region Important global listeners

    /// <summary>
    /// React to scene changes
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void EditorManager_SceneEvent(object sender, SceneEventArgs e)
    {
      if ((e.action == SceneEventArgs.Action.AfterStaticLighting || e.action == SceneEventArgs.Action.StaticLightingLoaded || e.action == SceneEventArgs.Action.StaticLightingReset) && e.Success)
      {
        if (TerrainEditor.CurrentTerrain != null && TerrainEditor.CurrentTerrain.LightmapSize != TerrainShape.LightmapSize_e.NoLightmapping)
        {
          TerrainEditor.DirectionalLighting = false; // switch to lightmapping mode
        }
      }
      if (e.action == SceneEventArgs.Action.AfterRendererNodeChanged)
      {
        UpdateToolbar();
      }
    }

    /// <summary>
    /// React to scene changes
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void EditorManager_SceneChanged(object sender, SceneChangedArgs e)
    {
      UpdatePanelStatus();
      UpdateHeightmapPanel();
    }

    /// <summary>
    /// React to layer changes
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void IScene_LayerChanged(object sender, LayerChangedArgs e)
    {
      UpdatePanelStatus();
    }

    /// <summary>
    /// React to terrain changes
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void TerrainEditor_OnTerrainChanged(object sender, EventArgs e)
    {
      UpdatePanelStatus();
      UpdateHeightmapPanel();
      UpdateDecorationList();
      UpdateDetailTextureList();
      TerrainShape terrain = TerrainEditor.CurrentTerrain;
      if (terrain == null)
      {
        UnsetTerrainContext();
        this.propertyGrid_Shader.SelectedObject = null;
        this.propertyGrid_MeshExport.SelectedObject = null;
      }
      else
      {
        this.propertyGrid_Shader.SelectedObject = terrain._terrainShaderProperties;
        this.propertyGrid_MeshExport.SelectedObject = terrain.MeshExportSettings;
      }
    }

    void VisionViewBase_MouseContextChanged(object sender, CSharpFramework.Contexts.ContextChangedArgs e)
    {
      bool bIsTerrain = e.newContext is TerrainEditingBaseContext;

      if (e.newContext != g_heightmapContext)
        TerrainEditor.HeightmapEditMode = HeightmapEditMode_e.None;
    }

    [Serializable]
    public class TerrainPanelSettings : ISerializable
    {
      public TerrainPanelSettings()
      { 
      }

      public HeightmapCursorProperties HeightmapCursor_Elevate;
      public HeightmapCursorProperties HeightmapCursor_Flatten;
      public HeightmapCursorProperties HeightmapCursor_Smooth;
      public HeightmapCursorProperties HeightmapCursor_Holes;
      public Cursor3DProperties DetailTextureCursor;
      public Cursor3DProperties VegetationCursor;

      #region ISerializable Members

      protected TerrainPanelSettings(SerializationInfo info, StreamingContext context)
      {
        HeightmapCursor_Elevate = (HeightmapCursorProperties)info.GetValue("HeightmapCursor_Elevate", typeof(HeightmapCursorProperties));
        HeightmapCursor_Flatten = (HeightmapCursorProperties)info.GetValue("HeightmapCursor_Flatten", typeof(HeightmapCursorProperties));
        HeightmapCursor_Smooth = (HeightmapCursorProperties)info.GetValue("HeightmapCursor_Smooth", typeof(HeightmapCursorProperties));
        HeightmapCursor_Holes = (HeightmapCursorProperties)info.GetValue("HeightmapCursor_Holes", typeof(HeightmapCursorProperties));
        DetailTextureCursor = (Cursor3DProperties)info.GetValue("DetailTextureCursor", typeof(Cursor3DProperties));
        VegetationCursor = (Cursor3DProperties)info.GetValue("VegetationCursor", typeof(Cursor3DProperties));
        if (SerializationHelper.HasElement(info, "BrushName"))
        {
          string file = info.GetString("BrushName");
          TerrainEditor.SetBrushByName(file);
        }
      }

      public void GetObjectData(SerializationInfo info, StreamingContext context)
      {
        info.AddValue("HeightmapCursor_Elevate", HeightmapCursor_Elevate);
        info.AddValue("HeightmapCursor_Flatten", HeightmapCursor_Flatten);
        info.AddValue("HeightmapCursor_Smooth", HeightmapCursor_Smooth);
        info.AddValue("HeightmapCursor_Holes", HeightmapCursor_Holes);
        info.AddValue("DetailTextureCursor", DetailTextureCursor);
        info.AddValue("VegetationCursor", VegetationCursor);
        if (TerrainEditor.CurrentBrush != null)
          info.AddValue("BrushName", TerrainEditor.CurrentBrush.DisplayName); // use display name here becaus ewe can't use abs filenames
      }

		#endregion
    }

    void EditorManager_CustomSceneSerialization(object sender, CustomSceneSerializationArgs e)
    {
      if (e.CustomSceneObjects.IsSaving)
      {
        if (TerrainEditor.CurrentTerrain != null) // only save data when a terrain is used
        {
          // get data from panel
          TerrainPanelSettings brushes = new TerrainPanelSettings();
          brushes.HeightmapCursor_Elevate = this._heightmapCursor_Elevate;
          brushes.HeightmapCursor_Flatten = this._heightmapCursor_Flatten;
          brushes.HeightmapCursor_Smooth = this._heightmapCursor_Smooth;
          brushes.HeightmapCursor_Holes = this._heightmapCursor_Holes;
          brushes.DetailTextureCursor = this._detailTextureCursor;
          brushes.VegetationCursor = this._decorationCursor;

          e.CustomSceneObjects.AddObject("TerrainEditorPlugin.EditorPlugin", brushes);
        }
      }
      else
      {
        TerrainPanelSettings brushes = 
          (TerrainPanelSettings)e.CustomSceneObjects.FindObject("TerrainEditorPlugin.EditorPlugin", typeof(TerrainPanelSettings));
        if (brushes != null)
        {
          if (brushes.HeightmapCursor_Elevate != null) this._heightmapCursor_Elevate = brushes.HeightmapCursor_Elevate;
          if (brushes.HeightmapCursor_Flatten != null) this._heightmapCursor_Flatten = brushes.HeightmapCursor_Flatten;
          if (brushes.HeightmapCursor_Smooth != null) this._heightmapCursor_Smooth = brushes.HeightmapCursor_Smooth;
          if (brushes.HeightmapCursor_Holes != null) this._heightmapCursor_Holes = brushes.HeightmapCursor_Holes;
          if (brushes.DetailTextureCursor != null) this._detailTextureCursor = brushes.DetailTextureCursor;
          if (brushes.VegetationCursor != null) this._decorationCursor = brushes.VegetationCursor;
        }
      }
    }

		#endregion


    #region Common Editing

    public void RefreshShaderEffectPropertyGrid()
    {
      propertyGrid_Shader.Refresh();
    }

    public void UpdatePanelStatus()
    {
      bool bHasScene = EditorManager.Scene != null;
      bool bHasTerrain = TerrainEditor.CurrentTerrain != null;
      bool bSectorsEditable = bHasTerrain && !TerrainEditor.CurrentTerrain.IsReference;
      bool bTerrainEditable = bHasTerrain && !TerrainEditor.CurrentTerrain.IsReference && TerrainEditor.CurrentTerrain.Modifiable;

      this.Enabled = bHasScene && bHasTerrain;
      if (!this.Enabled)
        this.UnsetTerrainContext();
      UpdateToolbar();
      FillImportHeightfieldProviders();
      FillHightmapFilters();

      groupBox_Terrain.Enabled = bTerrainEditable;
      groupBox_Import.Enabled = bTerrainEditable;
      groupBox_ExportDDS.Enabled = bTerrainEditable;

      //this is a work around to ensure that the current detail texture properties are updated (invalidate doesn't work)
      propertyGrid_DetailTexture.SelectedObject = null;
      propertyGrid_DetailTexture.SelectedObject = TerrainEditor.CurrentDetailTexture;

      propertyGrid_DetailTexture.Enabled = bTerrainEditable;
      propertyGrid_VegetationModel.Enabled = bTerrainEditable;
      propertyGrid_Shader.Enabled = bTerrainEditable;

      propertyGrid_MeshExport.Enabled = bTerrainEditable;
      button_ExportPrefab.Enabled = bTerrainEditable;
    }

    void UpdateHeightmapPanel()
    {
      string exportFile = null;
      if (TerrainEditor.CurrentTerrain != null)
      {
        exportFile = TerrainEditor.CurrentTerrain.HeightmapExportFilename;
        checkBox_NormalizedHMExport.Checked = TerrainEditor.CurrentTerrain.NormalizeHeightmapExport;
      }
      if (string.IsNullOrEmpty(exportFile))
      {
        button_ExportHeightmapDDS.Enabled = false;
        textBox_ExportDDS.Text = "<no file specified>";
      }
      else
      {
        button_ExportHeightmapDDS.Enabled = true;
        textBox_ExportDDS.Text = exportFile;
      }
    }


    /// <summary>
    /// When the tab changes, create an editing mouse context
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
		private void tabControl_SelectedIndexChanged(object sender, EventArgs e)
		{
		}


    void UpdateToolbar()
    {
      bool bHasScene = EditorManager.Scene != null;
      bool bHasTerrain = bHasScene && TerrainEditor.CurrentTerrain != null && TerrainEditor.CurrentTerrain.EngineTerrain != null;
      bool bSectorsEditable = bHasTerrain && !TerrainEditor.CurrentTerrain.IsReference;
      bool bTerrainEditable = bHasTerrain && !TerrainEditor.CurrentTerrain.IsReference && TerrainEditor.CurrentTerrain.Modifiable;
      bool bSupportsDirectional = bHasScene && EditorManager.Scene.RendererNodeClass != IRendererNodeManager.RENDERERNODECLASS_DEFERRED; // no directional light in deferred
      if (!bSupportsDirectional)
        TerrainEditor.DirectionalLighting = false;

      // Main tool strip
      this.toolStripButton_Elevate.Enabled = bSectorsEditable;
      this.toolStripButton_Flatten.Enabled = bSectorsEditable;
      this.toolStripButton_Smooth.Enabled = bSectorsEditable;
      this.toolStripButton_EditHoles.Enabled = bSectorsEditable;
      this.toolStripButton_PaintDetailTexture.Enabled = bSectorsEditable && !TerrainEditor.TextureBakedView;

      this.toolStripButton_Elevate.Checked = TerrainEditor.HeightmapEditMode == HeightmapEditMode_e.Elevate;
      this.toolStripButton_Flatten.Checked = TerrainEditor.HeightmapEditMode == HeightmapEditMode_e.Flatten;
      this.toolStripButton_Smooth.Checked = TerrainEditor.HeightmapEditMode == HeightmapEditMode_e.Smooth;
      this.toolStripButton_EditHoles.Checked = TerrainEditor.HeightmapEditMode == HeightmapEditMode_e.AddHoles;
      this.toolStripButton_PaintDetailTexture.Checked = PaintDetailTextureMode;

      this.toolStripButton_TextureBaked.Enabled = bHasTerrain;
      this.toolStripButton_TextureBaked.Checked = TerrainEditor.TextureBakedView;
      this.toolStripButton_DirectionalLighting.Enabled = bHasTerrain && bSupportsDirectional && !TerrainEditor.TextureBakedView; // time of day is directional in the first place
      this.toolStripButton_DirectionalLighting.Checked = TerrainEditor.DirectionalLighting;
      this.toolStripButton_FullRes.Enabled = bHasTerrain;
      this.toolStripButton_FullRes.Checked = TerrainEditor.PreviewFullRes;
      this.toolStripButton_DisplayVegetationDensity.Enabled = bHasTerrain && TerrainEditor.CurrentDecorationModel != null;
      this.toolStripButton_DisplayVegetationDensity.Checked = TerrainEditor.PreviewDecorationModelWeightmap;
      this.toolStripButton_DisplayMiniMap.Enabled = bHasTerrain && TerrainEditor.CurrentTerrain.HasMinimapTexture && !TerrainEditor.TextureBakedView;
      this.toolStripButton_DisplayMiniMap.Checked = TerrainEditor.DisplayMiniMap;
      
      this.toolStripButton_Selection.Enabled = bSectorsEditable;
      this.toolStripButton_PurgeResources.Enabled = true;

      // Texture tool strip
      this.toolStripButton_AddDetailTexture.Enabled = bTerrainEditable;
      this.toolStripButton_DeleteDetailTexture.Enabled = bTerrainEditable && TerrainEditor.CurrentDetailTexture != null && !TerrainEditor.CurrentDetailTexture._bIsBaseTexture;
      this.toolStripButton_MoveUp.Enabled = bTerrainEditable && MoveDetailTextureAction.CanMove(TerrainEditor.CurrentTerrain, TerrainEditor.CurrentDetailTexture, -1);
      this.toolStripButton_MoveDown.Enabled = bTerrainEditable && MoveDetailTextureAction.CanMove(TerrainEditor.CurrentTerrain, TerrainEditor.CurrentDetailTexture, 1);
      this.toolStripButton_Blink.Enabled = bHasTerrain && TerrainEditor.CurrentDetailTexture != null && !TerrainEditor.TextureBakedView;
      this.toolStripButton_MaterialID.Enabled = bHasTerrain && TerrainEditor.CurrentTerrain.Config.HasMaterialMap;
      this.toolStripButton_ImportWeightmap.Enabled = bTerrainEditable;
      this.toolStripButton_SlopeTextures.Enabled = bTerrainEditable;
      this.toolStripButton_SplitPerSectorTex.Enabled = bTerrainEditable && TerrainEditor.CurrentDetailTexture != null;

      // Vegetation tool strip
      this.toolStripButton_AddDecoChannel.Enabled = bTerrainEditable;
      this.toolStripButton_DeleteDecoChannel.Enabled = bTerrainEditable && TerrainEditor.CurrentDecorationModel != null;
      this.toolStripSplitButton_Atlas.Enabled = bTerrainEditable;
      this.toolStripButton_RebuildDirtyRange.Enabled = bSectorsEditable && TerrainEditor.CurrentTerrain.EngineTerrain.m_iDirtyDecorationSectorCount > 0;
      this.toolStripButton_RebuildVegetation.Enabled = bTerrainEditable;
      this.toolStripButton_AddMeshComponent.Enabled = bSectorsEditable;
      this.toolStripButton_RemoveMeshComponent.Enabled = bSectorsEditable;
    }


    // different painting contexts
    static HeightmapEditingContext g_heightmapContext = new HeightmapEditingContext();
    static DetailTextureEditingContext g_detailTextureContext = new DetailTextureEditingContext();
    static DecorationEditingContext g_decorationContext = new DecorationEditingContext();

    // cursor type for every mode
    HeightmapCursorProperties _heightmapCursor = null;
    HeightmapCursorProperties _heightmapCursor_Elevate = new HeightmapCursorProperties();
    HeightmapCursorProperties _heightmapCursor_Flatten = new HeightmapCursorProperties();
    HeightmapCursorProperties _heightmapCursor_Smooth = new HeightmapCursorProperties();
    HeightmapCursorProperties _heightmapCursor_Holes = new HeightmapCursorProperties();
    Cursor3DProperties _decorationCursor = new Cursor3DProperties();
    Cursor3DProperties _detailTextureCursor = new Cursor3DProperties();

    public void SetHeightmapEditMode(HeightmapEditMode_e mode)
    {
      TerrainEditor.HeightmapEditMode = mode;
      switch (mode)
      {
        case HeightmapEditMode_e.Subtract:
        case HeightmapEditMode_e.Elevate: _heightmapCursor = _heightmapCursor_Elevate; break;
        case HeightmapEditMode_e.Flatten: _heightmapCursor = _heightmapCursor_Flatten; break;
        case HeightmapEditMode_e.Smooth: _heightmapCursor = _heightmapCursor_Smooth; break;
        case HeightmapEditMode_e.RemoveHoles:
        case HeightmapEditMode_e.AddHoles: _heightmapCursor = _heightmapCursor_Holes; break;
      }
      TerrainEditor.Cursor3DProperties = _heightmapCursor;
      EditorManager.ActiveView.CurrentContext = g_heightmapContext;
      this.tabControl.SelectedTab = tabPage_Heightmap;
      UpdateToolbar();
    }


    //made public by Benjamin for terrain test
    public bool PaintDetailTextureMode
    {
      get
      {
        return EditorManager.ActiveView.CurrentContext == g_detailTextureContext;
      }
      set
      {
        if (value == PaintDetailTextureMode)
          return;
        if (value && !TerrainEditor.TextureBakedView)
        {
          TerrainEditor.Cursor3DProperties = _detailTextureCursor;
          EditorManager.ActiveView.CurrentContext = g_detailTextureContext;
          this.tabControl.SelectedTab = tabPage_Texturing;
        }
        else
        {
          UnsetTerrainContext();
        }
        UpdateToolbar();
      }
    }

    private void toolStripButton_PurgeResources_Click(object sender, EventArgs e)
    {
      EditorManager.Actions.Reset();
      EngineInstanceTerrain.PurgeAll();
    }

    #endregion

    #region Heightmap Editing

    void TerrainEditor_OnHeightmapEditModeChanged(object sender, EventArgs e)
    {
      UpdateToolbar();
    }


    private void toolStripButton_Elevate_Click(object sender, EventArgs e)
    {
      if (TerrainEditor.HeightmapEditMode == HeightmapEditMode_e.Elevate)
        UnsetTerrainContext();
      else
        SetHeightmapEditMode(HeightmapEditMode_e.Elevate);
    }

    private void toolStripButton_Flatten_Click(object sender, EventArgs e)
    {
      if (TerrainEditor.HeightmapEditMode == HeightmapEditMode_e.Flatten)
        UnsetTerrainContext();
      else
        SetHeightmapEditMode(HeightmapEditMode_e.Flatten);
    }

    private void toolStripButton_Smooth_Click(object sender, EventArgs e)
    {
      if (TerrainEditor.HeightmapEditMode == HeightmapEditMode_e.Smooth)
        UnsetTerrainContext();
      else
        SetHeightmapEditMode(HeightmapEditMode_e.Smooth);
    }

    private void toolStripButton_EditHoles_Click(object sender, EventArgs e)
    {
      if (TerrainEditor.HeightmapEditMode == HeightmapEditMode_e.AddHoles)
        UnsetTerrainContext();
      else
        SetHeightmapEditMode(HeightmapEditMode_e.AddHoles);
    }

    private void toolStripButton_PickHeight_Click(object sender, EventArgs e)
    {
      EditorManager.ActiveView.PickHandler = new TerrainPickHandler(TerrainEditor.CurrentTerrain, _heightmapCursor_Flatten);
    }

    #endregion

    #region Decoration Editing

    void UpdateDecorationList()
    {
      listView_VegetationModels.BeginUpdate();
      listView_VegetationModels.Items.Clear();
      if (TerrainEditor.CurrentTerrain != null)
      {
        ArrayList deco = TerrainEditor.CurrentTerrain.DecorationModels;
        foreach (DecorationModelResource d in deco)
        {
          ListViewItem item = new ListViewItem(d.Name);
          if (d._LODOrigin != null)
            item.ForeColor = SystemColors.GrayText;
          item.SubItems.Add(d.Filename);
          item.Tag = d;
          listView_VegetationModels.Items.Add(item);
        }
      }
      listView_VegetationModels.EndUpdate();
    }

    private void tabPage_Decoration_Click(object sender, EventArgs e)
    {
    }

    private void listView_VegetationModels_SelectedIndexChanged(object sender, EventArgs e)
    {
      if (listView_VegetationModels.SelectedItems.Count > 0)
      {
        TerrainEditor.CurrentDecorationModel = listView_VegetationModels.SelectedItems[0].Tag as DecorationModelResource;
        PaintDecorationMode = TerrainEditor.CurrentDecorationModel != null && TerrainEditor.CurrentDecorationModel._LODOrigin == null;// can only paint with the original
      }
      else
      {
        TerrainEditor.CurrentDecorationModel = null;
        PaintDecorationMode = false;
      }
    }

    private void listView_VegetationModels_DoubleClick(object sender, EventArgs e)
    {
      if (TerrainEditor.CurrentDecorationModel == null)
        return;

      if (TerrainEditor.CurrentTerrain == null || TerrainEditor.CurrentTerrain.IsReference || !TerrainEditor.CurrentTerrain.Modifiable)
        return;

      using(PropertyGridDlg dlg = new PropertyGridDlg("Edit properties of '" + TerrainEditor.CurrentDecorationModel.Name + "'",
        "Edit all properties of this vegetation model at once so update of the terrain only happens when changes are confirmed"))
      {
        DecorationModelResource other = new DecorationModelResource(null, 0);
        other.CopyPropertiesFrom(TerrainEditor.CurrentDecorationModel, false);
        dlg.DataObjectNoClone = other.ViewObject;
        if (dlg.ShowDialog(this) != DialogResult.OK)
          return;

        TerrainEditor.CurrentDecorationModel.CopyPropertiesFrom(other, true);
      }
    }


    void TerrainEditor_OnCurrentDecorationModelChanged(object sender, EventArgs e)
    {
      if (TerrainEditor.CurrentDecorationModel==null)
        this.propertyGrid_VegetationModel.SelectedObject = null;
      else
        this.propertyGrid_VegetationModel.SelectedObject = TerrainEditor.CurrentDecorationModel.ViewObject;
      UpdateToolbar();
    }

    void TerrainEditor_OnDecorationModelListChanged(object sender, EventArgs e)
    {
      UpdateDecorationList();
    }

    void TerrainEditor_OnDecorationDirtyRegionChanged(object sender, EventArgs e)
    {
      UpdateToolbar();
    }

    void ShapeComponent_OnUIUpdateFinished(object sender, EventArgs e)
    {
    }

    void ShapeComponent_OnUIUpdateBegin(object sender, EventArgs e)
    {
    }


    void DecorationModelResource_OnDecorationModelResourcePropertyChanged(object sender, EventArgs e)
    {
      if (TerrainEditor.CurrentTerrain != null && TerrainEditor.CurrentTerrain.DecorationModels.Contains(sender))
      {
        DecorationModelResource model = (DecorationModelResource)sender;
        UpdateDecorationList();
        TerrainEditor.CurrentTerrain.UpdateDecorationModelProperties(model);
        if (model._LODOrigin != null)
          TerrainEditor.CurrentTerrain.UpdateDecorationModelProperties(model._LODOrigin);
        propertyGrid_VegetationModel.SelectedObject = model.ViewObject; // view object might be different
      }
      propertyGrid_DetailTexture.SelectedObject = TerrainEditor.CurrentDetailTexture;
    }


    //made public by Benjamin for terrain test
    public bool PaintDecorationMode
    {
      get
      {
        return EditorManager.ActiveView.CurrentContext == g_decorationContext;
      }
      set
      {
        if (value == PaintDecorationMode)
          return;
        if (value)
        {
          TerrainEditor.Cursor3DProperties = _decorationCursor;
          EditorManager.ActiveView.CurrentContext = g_decorationContext;
          this.tabControl.SelectedTab = tabPage_Decoration;
        }
        else
        {
          UnsetTerrainContext();
        }
        UpdateToolbar();
      }
    }

    DecorationModelResource CreateNewModel(DecorationModelResource lodOrigin, string name)
    {
      if (TerrainEditor.CurrentTerrain == null)
        return null;

      DecorationModelResource model = TerrainEditor.CurrentTerrain.CreateDecorationModelResource();

      // pick the model file
      OpenFileDlg fileDlg = new OpenFileDlg();
      fileDlg.InitialDirectory = EditorManager.Project.ProjectDir;
      fileDlg.Text = "Select model file";
      fileDlg.Caption = fileDlg.Text;
      fileDlg.Filter = new string[] { ".model", ".srt", ".atlas" }; // TODO: get via available plugins?
      fileDlg.FileName = model.Filename;

      if (fileDlg.ShowDialog(this) != DialogResult.OK)
        return null;

      model._LODOrigin = lodOrigin;
      model.Filename = EditorManager.Project.MakeRelative(fileDlg.FileName);
      if (string.IsNullOrEmpty(name))
        model.Name = System.IO.Path.GetFileNameWithoutExtension(model.Filename);
      else
        model.Name = name;
      EditorManager.Actions.Add(new AddDecorationModelAction(TerrainEditor.CurrentTerrain, model));
      return model;
    }


    /// <summary>
    /// Adds a decoration model via action
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void toolStripButton_AddDecoChannel_Click(object sender, EventArgs e)
    {
      CreateNewModel(null, null);
    }

    /// <summary>
    /// Delete decoration model via action
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void toolStripButton_DeleteDecoChannel_Click(object sender, EventArgs e)
    {
      if (TerrainEditor.CurrentTerrain == null || TerrainEditor.CurrentDecorationModel==null)
        return;
      EditorManager.Actions.Add(new DeleteDecorationModelAction(TerrainEditor.CurrentTerrain, TerrainEditor.CurrentDecorationModel));
    }

    private void toolStripButton_DisplayDensityMap_Click(object sender, EventArgs e)
    {
      // toggle state
      TerrainEditor.PreviewDecorationModelWeightmap = !TerrainEditor.PreviewDecorationModelWeightmap;
    }

    private void toolStripButton_DisplayMiniMap_Click(object sender, EventArgs e)
    {
      // toggle state
      TerrainEditor.DisplayMiniMap = !TerrainEditor.DisplayMiniMap;
    }

    private void toolStripButton_FullRes_Click(object sender, EventArgs e)
    {
      // toggle state
      TerrainEditor.PreviewFullRes = !TerrainEditor.PreviewFullRes;
    }

    public void UpdateDecoration(bool bFullTerrain)
    {
      if (TerrainEditor.CurrentTerrain == null)
        return;
      TerrainEditor.CurrentTerrain.EngineTerrain.SynchronizeDecorationModels(TerrainEditor.CurrentTerrain.DecorationModels);
      TerrainEditor.CurrentTerrain.EngineTerrain.UpdateDecoration(bFullTerrain);
      EditorManager.ActiveView.UpdateView(false);
      TerrainEditor.CurrentTerrain.Modified = true;
    }

    private void toolStripButton_RebuildDirtyRange_Click(object sender, EventArgs e)
    {
      UpdateDecoration(false);
    }

    private void toolStripButton_RebuildVegetation_Click(object sender, EventArgs e)
    {
      UpdateDecoration(true);
    }

    private void toolStripButton_AddMeshComponent_Click(object sender, EventArgs e)
    {
      EditorManager.ActiveView.PickHandler = new StaticMeshVegetationPickhandler(this, true);
    }

    private void toolStripButton_RemoveMeshComponent_Click(object sender, EventArgs e)
    {
      EditorManager.ActiveView.PickHandler = new StaticMeshVegetationPickhandler(this, false);
    }



    private void contextMenuStrip_Decoration_Opening(object sender, CancelEventArgs e)
    {
      toolStripMenuItem_DecorationFilters.DropDownItems.Clear();
      ToolStripMenuItem_ExportDensity.Enabled = false;
      createLODVersionToolStripMenuItem.Enabled = TerrainEditor.CurrentDecorationModel != null && TerrainEditor.CurrentDecorationModel._LODOrigin == null;

      if (TerrainEditor.CurrentTerrain == null || TerrainEditor.CurrentTerrain.EngineTerrain==null)
        return;
      if (TerrainEditor.CurrentDecorationModel == null)
        return;

      foreach (IDecorationFilter filter in TerrainEditor.DecorationFilterList)
      {
        ToolStripItem item = toolStripMenuItem_DecorationFilters.DropDownItems.Add(filter.Name);
        item.Tag = filter;
        filter.DecorationModel = TerrainEditor.CurrentDecorationModel;
        item.Click += new EventHandler(DecorationFilter_Click);
      }

      ToolStripMenuItem_ExportDensity.Enabled = TerrainEditor.CurrentDecorationModel.HasExportFilename;
    }

    void DecorationFilter_Click(object sender, EventArgs e)
    {
      IDecorationFilter filter = ((ToolStripItem)sender).Tag as IDecorationFilter;
      if (filter.HasCustomProperties)
      {
        ApplyHeightmapFilterDlg dlg = new ApplyHeightmapFilterDlg();
        dlg.Filter = filter;
        dlg.ShowDialog();
        return;
      }

      // run without dialog (see Apply button in dialog):
      EditorManager.Progress.ShowProgressDialog("Decoration filter : " + filter.Name);
      TerrainEditor.CurrentTerrain.EngineTerrain.ApplyDecorationFilter(filter, EditorManager.Progress);
      EditorManager.Actions.Add(new TerrainChangedAction(TerrainEditor.CurrentTerrain.EngineTerrain, "Decoration : " + filter.Name));
      EditorManager.Progress.HideProgressDialog();
    }

    private void ToolStripMenuItem_ExportDensity_Click(object sender, EventArgs e)
    {
      if (TerrainEditor.CurrentTerrain == null || TerrainEditor.CurrentTerrain.EngineTerrain == null)
        return;
      if (TerrainEditor.CurrentDecorationModel == null || !TerrainEditor.CurrentDecorationModel.HasExportFilename)
        return;

      bool bResult = false;
      try
      {
        string filename = EditorManager.Project.MakeAbsolute(TerrainEditor.CurrentDecorationModel.ExportFilename);

        // Ensure .dds extension
        if (Path.GetExtension(filename) != ".dds")
          filename += ".dds";

        bResult = TerrainEditor.CurrentTerrain.EngineTerrain.ExportDensityMapAsDDS(filename, TerrainEditor.CurrentDecorationModel);
      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex);
        bResult = false;
      }
      if (!bResult)
        EditorManager.ShowMessageBox(this, "Failed to export density map file", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        
    }

    private void createLODVersionToolStripMenuItem_Click(object sender, EventArgs e)
    {
      if (TerrainEditor.CurrentDecorationModel == null || TerrainEditor.CurrentDecorationModel._LODOrigin != null)
        return;
      float fFar = TerrainEditor.CurrentDecorationModel.FarClipDistance;
      float fSplit = fFar*0.5f;
      float fOldFade = TerrainEditor.CurrentDecorationModel.RelativeFadeOutStart;
      DecorationModelResource model = CreateNewModel(TerrainEditor.CurrentDecorationModel, "LOD of " + TerrainEditor.CurrentDecorationModel.Name);
      if (model == null)
        return;

      // the original model is used as closest model
      TerrainEditor.CurrentDecorationModel.SetClipDistancesInternal(0.0f, fSplit, 1.0f); // LOD version must not fade out
      model.SetClipDistancesInternal(fSplit, fFar, fOldFade);
      TerrainEditor.CurrentTerrain.UpdateDecorationModelProperties(TerrainEditor.CurrentDecorationModel);
    }

    #endregion

    #region Texture Editing

    void UpdateDetailTextureList()
    {
      listView_DetailTextures.BeginUpdate();
      listView_DetailTextures.Items.Clear();
      if (TerrainEditor.CurrentTerrain != null)
      {
        ArrayList textures = TerrainEditor.CurrentTerrain.DetailTextures;
        foreach (DetailTextureResource tex in textures)
        {
          ListViewItem item = new ListViewItem(tex.Name);
          item.SubItems.Add(tex.DiffuseFilename);
          item.Tag = tex;

          // diffuse texture as image
          int iImage = -1;
          string filename = null;
         
          if (tex.UsesPerSectorTextures)
            filename = Path.Combine(EditorManager.GetDirectory(EditorManager.DirectoryType.AppDataPath), DetailTextureResource.ICON_PERSECTORTEXTURES);
          else if (!string.IsNullOrEmpty(tex.DiffuseFilename))
            filename = EditorManager.Project.MakeAbsolute(tex.DiffuseFilename);

          if (!string.IsNullOrEmpty(filename))
            iImage = _detailTexturesImageList.AddBitmap(filename, Color.Magenta);

          item.ImageIndex = iImage; 
          listView_DetailTextures.Items.Add(item);
        }
      }
      listView_DetailTextures.EndUpdate();
    }

    void TerrainEditor_OnDetailTextureListChanged(object sender, DetailTextureListChangedArgs e)
    {
      UpdateDetailTextureList();
      UpdateToolbar();
    }

    void UpdateMaterialIDHighlighting()
    {
      if (TerrainEditor.CurrentTerrain != null && TerrainEditor.CurrentTerrain.EngineTerrain != null)
      {
        int iMatID = -1;
        if (TerrainEditor.CurrentDetailTexture != null && toolStripButton_MaterialID.Enabled && toolStripButton_MaterialID.Checked)
          iMatID = TerrainEditor.CurrentDetailTexture.MaterialID;
        TerrainEditor.CurrentTerrain.EngineTerrain.SetHighlightMaterialID(iMatID);
      }
    }

    void DetailTextureResource_OnDetailTextureResourcePropertyChanged(object sender, DetailTextureResourcePropertyChangedArgs e)
    {
      if (TerrainEditor.CurrentTerrain != null && TerrainEditor.CurrentTerrain.DetailTextures.Contains(sender))
      {
        UpdateDetailTextureList();
        TerrainEditor.CurrentTerrain.UpdateDetailTextureProperties((DetailTextureResource)sender, e._propertyName == "MaterialID");
        UpdateMaterialIDHighlighting();
      }
      propertyGrid_DetailTexture.SelectedObject = TerrainEditor.CurrentDetailTexture;
    }

    void TerrainEditor_OnCurrentDetailTextureChanged(object sender, EventArgs e)
    {
      this.propertyGrid_DetailTexture.SelectedObject = TerrainEditor.CurrentDetailTexture;
      UpdateMaterialIDHighlighting();
      UpdateToolbar();
    }

    private void listView_DetailTextures_SelectedIndexChanged(object sender, EventArgs e)
    {
      if (this.listView_DetailTextures.SelectedItems.Count > 0)
      {
        TerrainEditor.CurrentDetailTexture = listView_DetailTextures.SelectedItems[0].Tag as DetailTextureResource;
        if (TerrainEditor.CurrentDetailTexture != null && !TerrainEditor.TextureBakedView)
          PaintDetailTextureMode = true; // switch to this context
      }
      else
      {
        TerrainEditor.CurrentDetailTexture = null;
        PaintDetailTextureMode = false;
      }
    }

    private void listView_DetailTextures_DoubleClick(object sender, EventArgs e)
    {
      if (TerrainEditor.CurrentDetailTexture == null)
        return;

      if (TerrainEditor.CurrentTerrain == null || TerrainEditor.CurrentTerrain.IsReference || !TerrainEditor.CurrentTerrain.Modifiable)
        return;

      // open the UI editor from outside the property grid
      PropertyDescriptor pd = TypeDescriptor.GetProperties(TerrainEditor.CurrentTerrain)["DetailTextures"];
      TypedCollectionEditor editor = (TypedCollectionEditor)pd.GetEditor(typeof(UITypeEditor));
      DefaultServiceProvider serviceProvider = new DefaultServiceProvider(TerrainEditor.CurrentTerrain, pd);
      ArrayList oldArray = TerrainEditor.CurrentTerrain.DetailTextures;
      ArrayList newArray = ArrayListHelper.ShallowClone(oldArray);

      DetailTextureResource.AllowUpdate = false; // temporarily disable
      ArrayList newArrayRes = (ArrayList)editor.EditValue(serviceProvider, serviceProvider, newArray);
      DetailTextureResource.AllowUpdate = true;
      DialogResult res = editor.UsedCollectionForm.DialogResult;
      if (res != DialogResult.OK) // abort?
        return;
      if (newArrayRes.Count < 1 || !((DetailTextureResource)newArrayRes[0])._bIsBaseTexture)
      {
        EditorManager.ShowMessageBox(this, "Cannot remove or change order of base texture", "Changing detail textures failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
        return;
      }

      EditorManager.Actions.Add(new NewDetailTextureCollectionAction(TerrainEditor.CurrentTerrain, oldArray, newArrayRes));
    }
 

    /// <summary>
    /// Adds a detail texture via action
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void toolStripButton_AddDetailTexture_Click(object sender, EventArgs e)
    {
      if (TerrainEditor.CurrentTerrain == null)
        return;

      DetailTextureResource texture = TerrainEditor.CurrentTerrain.CreateDetailTexture();

      // pick the detail texture file
      IOpenAssetDlg assetDlg = EditorManager.AssetManager.GetOpenAssetDialog("Select Detail Texture",
          "Select which texture to use for the new detail texture layer.", "", new string[] { "Texture" }, false);

      if (assetDlg.ShowDialog(this) != DialogResult.OK)
        return;

      texture.DiffuseFilename = EditorManager.Project.MakeRelative(assetDlg.SelectedAsset);
      texture.Name = System.IO.Path.GetFileNameWithoutExtension(texture.DiffuseFilename);
      EditorManager.Actions.Add(new AddDetailTextureAction(TerrainEditor.CurrentTerrain, texture));
    }

    /// <summary>
    /// Deletes a detail texture via action
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void toolStripButton_DeleteDetailTexture_Click(object sender, EventArgs e)
    {
      if (TerrainEditor.CurrentTerrain == null || TerrainEditor.CurrentDetailTexture == null || TerrainEditor.CurrentDetailTexture._bIsBaseTexture)
        return;
      EditorManager.Actions.Add(new DeleteDetailTextureAction(TerrainEditor.CurrentTerrain, TerrainEditor.CurrentDetailTexture));
    }

    private void toolStripButton_MoveUp_Click(object sender, EventArgs e)
    {
      if (!MoveDetailTextureAction.CanMove(TerrainEditor.CurrentTerrain, TerrainEditor.CurrentDetailTexture, -1))
        return;

      MoveDetailTextureAction move = new MoveDetailTextureAction(TerrainEditor.CurrentTerrain, TerrainEditor.CurrentDetailTexture, -1);
      EditorManager.Actions.Add(move);
    }

    private void toolStripButton_MoveDown_Click(object sender, EventArgs e)
    {
      if (!MoveDetailTextureAction.CanMove(TerrainEditor.CurrentTerrain, TerrainEditor.CurrentDetailTexture, 1))
        return;

      MoveDetailTextureAction move = new MoveDetailTextureAction(TerrainEditor.CurrentTerrain, TerrainEditor.CurrentDetailTexture, 1);
      EditorManager.Actions.Add(move);
    }


    void TerrainEditor_OnTextureBakedViewChanged(object sender, EventArgs e)
    {
      toolStripButton_TextureBaked.Checked = TerrainEditor.TextureBakedView;
      PaintDetailTextureMode = false;
      UpdateToolbar();
    }

    private void toolStripButton_TextureBaked_Click(object sender, EventArgs e)
    {
      TerrainEditor.TextureBakedView = !TerrainEditor.TextureBakedView;
    }

    private void toolStripButton_PaintDetailTexture_Click(object sender, EventArgs e)
    {
      PaintDetailTextureMode = !PaintDetailTextureMode;
    }

    private void toolStripButton_DirectionalLighting_Click(object sender, EventArgs e)
    {
      TerrainEditor.DirectionalLighting = !TerrainEditor.DirectionalLighting;
    }

    void TerrainEditor_OnDirectionalLightingChanged(object sender, EventArgs e)
    {
      UpdateToolbar();
    }

    void TerrainEditor_OnMiniMapStatusChanged(object sender, EventArgs e)
    {
      UpdateToolbar();
    }

    void TerrainEditor_OnPreviewFullResolutionStatusChanged(object sender, EventArgs e)
    {
      UpdateToolbar();
    }

    private void toolStripButton_Blink_Click(object sender, EventArgs e)
    {
      if (TerrainEditor.CurrentTerrain == null || TerrainEditor.CurrentTerrain.EngineTerrain == null || TerrainEditor.CurrentDetailTexture == null)
        return;
      // blink 3 seconds
      TerrainEditor.CurrentTerrain.EngineTerrain.BlinkDetailTexture(TerrainEditor.CurrentDetailTexture, 3.0f);
      EditorManager.ActiveView.UpdateView(false); // trigger the first update
    }

    private void toolStripButton_MaterialID_Click(object sender, EventArgs e)
    {
      this.UpdateMaterialIDHighlighting();
      EditorManager.ActiveView.UpdateView(false); // trigger the first update
    }

    // make static filter objects to store data
    static ImportWeightmapFilter g_ImportWeightmap = new ImportWeightmapFilter(null);
    static SlopeDetailTexturesFilter g_SlopeTexturesFilter = new SlopeDetailTexturesFilter(null);

    /// <summary>
    /// Helper function to show a message that the undo stack will be cleared. If 'yes' is selected then this function calls EditorManager.Actions.Reset();
    /// </summary>
    /// <param name="title"></param>
    /// <returns></returns>
    public static bool PromptClearUndo(string title)
    {
      DialogResult res = EditorManager.ShowMessageBox("This operation is not undo-able and it will additionally clear the current Undo/Redo history.\n\nContinue anyway?", title, MessageBoxButtons.YesNo, MessageBoxIcon.Question);
      if (res != DialogResult.Yes)
        return false;
      EditorManager.Actions.Reset();
      return true;
    }

    private void toolStripButton_ImportWeightmap_Click(object sender, EventArgs e)
    {
      g_ImportWeightmap.OwnerTerrain = TerrainEditor.CurrentTerrain;
      using (PropertyGridDlg dlg = new PropertyGridDlg("Import Weightmap", "Choose an existing weightmap texture file and select the target channels to map"))
      {
        dlg.DataObjectNoClone = g_ImportWeightmap;
        if (dlg.ShowDialog() != DialogResult.OK)
          return;

        if (string.IsNullOrEmpty(g_ImportWeightmap.WeightmapFilename))
        {
          EditorManager.ShowMessageBox("A valid weightmap file must be specified", "Import Weightmap", MessageBoxButtons.OK, MessageBoxIcon.Information);
          return;
        }

        if (!PromptClearUndo("Import Weightmap"))
          return;

        string filename = EditorManager.Project.MakeAbsolute(g_ImportWeightmap.WeightmapFilename);
        EditorManager.Progress.ShowProgressDialog("Import weightmap");
        TerrainEditor.CurrentTerrain.EngineTerrain.ImportWeightmap(
          filename,
          g_ImportWeightmap.DestinationRed,
          g_ImportWeightmap.DestinationGreen,
          g_ImportWeightmap.DestinationBlue,
          g_ImportWeightmap.DestinationAlpha,
          EditorManager.Progress);
        TerrainEditor.CurrentTerrain.Modified = true;
        EditorManager.Progress.HideProgressDialog();
      }
    }

    private void toolStripButton_SlopeTextures_Click(object sender, EventArgs e)
    {
      g_SlopeTexturesFilter.OwnerTerrain = TerrainEditor.CurrentTerrain;
      using (PropertyGridDlg dlg = new PropertyGridDlg("Generate weightings from slope", "Choose a bitmap that is used as a gradient lookup to map onto the chosen textures"))
      {
        dlg.DataObjectNoClone = g_SlopeTexturesFilter;
        if (dlg.ShowDialog() != DialogResult.OK)
          return;
        if (string.IsNullOrEmpty(g_SlopeTexturesFilter.SlopeLookup))
        {
          EditorManager.ShowMessageBox("A valid texture must be specified for the slope lookup", "Error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
          return;
        }

        string filename = EditorManager.Project.MakeAbsolute(g_SlopeTexturesFilter.SlopeLookup);
        if (!File.Exists(filename))
        {
          EditorManager.ShowMessageBox("The specified bitmap does not exist", "Error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
          return;
        }
        if (!PromptClearUndo("Generate Detail Textures"))
          return;

        EditorManager.Progress.ShowProgressDialog("Generate weightings from slope");
        TerrainEditor.CurrentTerrain.EngineTerrain.DetailTexturesFromSlope(
          filename,
          g_SlopeTexturesFilter.DestinationRed,
          g_SlopeTexturesFilter.DestinationGreen,
          g_SlopeTexturesFilter.DestinationBlue,
          g_SlopeTexturesFilter.DestinationAlpha,
          g_SlopeTexturesFilter.NormalNoise,
          EditorManager.Progress);
        TerrainEditor.CurrentTerrain.Modified = true;
        EditorManager.Progress.HideProgressDialog();
      }
    }

    private void toolStripButton_SplitPerSectorTex_Click(object sender, EventArgs e)
    {
      if (TerrainEditor.CurrentTerrain == null || TerrainEditor.CurrentTerrain.EngineTerrain == null)
        return;
      if (TerrainEditor.CurrentDetailTexture == null)
        return;

      ImportSplitTexturesDlg dlg = new ImportSplitTexturesDlg();
      if (dlg.ShowDialog(this) != DialogResult.OK || !dlg.AnyValid)
        return;

      if (!PromptClearUndo("Split texture"))
        return;

      EditorManager.Progress.ShowProgressDialog("Split texture");
      string error = TerrainEditor.CurrentTerrain.EngineTerrain.SplitPerSectorTextures(
        dlg.DiffuseFilename, dlg.NormalmapFilename, dlg.SpecularFilename, dlg.FilteringBorder,
        TerrainEditor.CurrentDetailTexture, EditorManager.Progress);
      EditorManager.Progress.HideProgressDialog();


      if (!string.IsNullOrEmpty(error))
      {
        EditorManager.ShowMessageBox(this, "Failed to split texture(s). Detailed message:\n\n" + error, "Split texture", MessageBoxButtons.OK, MessageBoxIcon.Error);
      }
      else
      {
        EditorManager.ShowMessageBox(this,
          "Texture splitting was successful.\n\n" +
          "The texture tiles have been saved into the 'AuxiliaryTextures' folder of the terrain data folder\n" +
          "as uncompressed DDS textures.\n" +
          "Eventually these textures should be compressed using an external tool.",
          "Success", MessageBoxButtons.OK, MessageBoxIcon.Information);
      }
    }

    private void contextMenuStrip_DetailTexture_Opening(object sender, CancelEventArgs e)
    {
      bool bHasTerrain = TerrainEditor.CurrentTerrain!=null && TerrainEditor.CurrentTerrain.EngineTerrain!=null;
      DetailTextureResource tex = TerrainEditor.CurrentDetailTexture;
      ToolStripMenuItem_ExportTextureLuminance.Enabled = bHasTerrain && tex != null && tex.HasExportFilename;
    }

    private void ToolStripMenuItem_ExportTextureLuminance_Click(object sender, EventArgs e)
    {
      bool bHasTerrain = TerrainEditor.CurrentTerrain != null && TerrainEditor.CurrentTerrain.EngineTerrain != null;
      DetailTextureResource tex = TerrainEditor.CurrentDetailTexture;
      if (!bHasTerrain || tex == null || !tex.HasExportFilename)
        return;
      bool bResult = false;
      try
      {
        string filename = EditorManager.Project.MakeAbsolute(tex.ExportFilename);
        // Ensure .dds extension
        if (Path.GetExtension(filename) != ".dds" )
          filename += ".dds";

        bResult = TerrainEditor.CurrentTerrain.EngineTerrain.ExportDetailTextureAsDDS(filename, tex);
      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex);
        bResult = false;
      }
      if (!bResult)
        EditorManager.ShowMessageBox(this, "Failed to export detail texture file", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
 
    }

    #endregion

    #region Heightmap Import / Filter


    private void button_ChangeConfig_Click(object sender, EventArgs e)
    {
      if (TerrainEditor.CurrentTerrain == null)
        return;

      TerrainConfigDlg dlg = new TerrainConfigDlg();
      dlg.FullConfig = false; // allow editing a subset only
      dlg.Config = TerrainEditor.CurrentTerrain.Config;
      if (dlg.ShowDialog() != DialogResult.OK)
        return;

      // outputs additional warning
      TerrainEditor.CurrentTerrain.Config = dlg.Config;
    }

    void FillImportHeightfieldProviders()
    {
      comboBox_ImportHeightfield.BeginUpdate();
      comboBox_ImportHeightfield.Items.Clear();

      if (TerrainEditor.HeightmapImporterList.Count > 0)
      {
        comboBox_ImportHeightfield.Enabled = true;
        comboBox_ImportHeightfield.Items.Add("<select source here>");

        foreach (IHeightmapImporter importer in TerrainEditor.HeightmapImporterList)
          comboBox_ImportHeightfield.Items.Add(importer);

        comboBox_ImportHeightfield.SelectedIndex = 0;
      }
      else
      {
        comboBox_ImportHeightfield.Enabled = false;
      }

      comboBox_ImportHeightfield.EndUpdate();
    }

    void FillHightmapFilters()
    {
      comboBox_ApplyFilter.BeginUpdate();
      comboBox_ApplyFilter.Items.Clear();

      if (TerrainEditor.HeightmapFilterList.Count > 0)
      {
        comboBox_ApplyFilter.Enabled = true;
        comboBox_ApplyFilter.Items.Add("<select filter here>");

        foreach (IHeightmapFilter filter in TerrainEditor.HeightmapFilterList)
          comboBox_ApplyFilter.Items.Add(filter);
        comboBox_ApplyFilter.SelectedIndex = 0;
      }
      else
      {
        comboBox_ApplyFilter.Enabled = false;
      }

      comboBox_ApplyFilter.EndUpdate();
    }



    void RunHeightmapFilter(IHeightmapFilter filter)
    {
      // opens dialog...
      ApplyHeightmapFilterDlg.RunFilter(filter);
    }

    private void comboBox_ImportHeightfield_SelectedIndexChanged(object sender, EventArgs e)
    {
      RunHeightmapFilter(comboBox_ImportHeightfield.SelectedItem as IHeightmapFilter);
      comboBox_ImportHeightfield.SelectedIndex = 0; // switch back
    }

    private void comboBox_ApplyFilter_SelectedIndexChanged(object sender, EventArgs e)
    {
      RunHeightmapFilter(comboBox_ApplyFilter.SelectedItem as IHeightmapFilter);
      comboBox_ApplyFilter.SelectedIndex = 0; // switch back
    }

    private void button_ExportHeightmapDDS_Click(object sender, EventArgs e)
    {
      if (TerrainEditor.CurrentTerrain == null || TerrainEditor.CurrentTerrain.EngineTerrain == null)
        return;
      string filename = TerrainEditor.CurrentTerrain.HeightmapExportFilename;
      if (string.IsNullOrEmpty(filename))
        return;

      bool bResult = true;
      try
      {
        bResult = TerrainEditor.CurrentTerrain.EngineTerrain.ExportHeightmapAsDDS(EditorManager.Project.MakeAbsolute(filename), TerrainEditor.CurrentTerrain.NormalizeHeightmapExport);
      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex);
        bResult = false;
      }

      if (!bResult)
        EditorManager.ShowMessageBox(this, "Failed to export height map file", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
    }


    private void button_ExportFilenameBrowse_Click(object sender, EventArgs e)
    {
      if (TerrainEditor.CurrentTerrain == null || TerrainEditor.CurrentTerrain.EngineTerrain == null)
        return;

      CreateFileDlg fileDlg = new CreateFileDlg();
      fileDlg.Caption = "Select Texture file";
      fileDlg.Description = "Select a DDS target file to export the terrain height map data. The format of the dds texture is R32F.";
      fileDlg.InitialDirectory = EditorManager.Project.ProjectDir;
      fileDlg.Ext = ".dds";
      fileDlg.Filter = new string[] { ".dds" };
      fileDlg.AllowOverwrite = true;
      //Just use the filename and do not use any path for the fileDlg.FileName, since the CreateFileDlg always combines the path with the filename. 
      //If the FileName has path information these get also combined with the chosen path, which leads to unwanted results [#2851]
      fileDlg.FileName = Path.GetFileName(TerrainEditor.CurrentTerrain.HeightmapExportFilename);
      fileDlg.SelectedStartSubDirectory = Path.GetDirectoryName(TerrainEditor.CurrentTerrain.HeightmapExportFilename);
      if (fileDlg.ShowDialog() != DialogResult.OK)
        return;

      TerrainEditor.CurrentTerrain.HeightmapExportFilename = EditorManager.Project.MakeRelative(fileDlg.FileName);
    }

    private void checkBox_NormalizedHMExport_CheckedChanged(object sender, EventArgs e)
    {
      if (TerrainEditor.CurrentTerrain == null)
        return;
      TerrainEditor.CurrentTerrain.NormalizeHeightmapExport = checkBox_NormalizedHMExport.Checked;
    }

    void TerrainEditor_OnHeightmapExportFilenameChanged(object sender, EventArgs e)
    {
      this.UpdateHeightmapPanel();
    }

    private void textBox_ExportDDS_TextChanged(object sender, EventArgs e)
    {

    }

    private void textBox_ExportDDS_Enter(object sender, EventArgs e)
    {

    }

    private void textBox_ExportDDS_Leave(object sender, EventArgs e)
    {

    }

    #endregion

    private void splitContainer1_Panel1_Paint(object sender, PaintEventArgs e)
    {

    }

    private void tabPage_Heightmap_Click(object sender, EventArgs e)
    {

    }

    private void groupBox_ExportDDS_Enter(object sender, EventArgs e)
    {

    }

    private void groupBox1_Enter(object sender, EventArgs e)
    {

    }

    private void toolStripButton_Selection_Click(object sender, EventArgs e)
    {
      if (TerrainEditor.CurrentTerrain == null)
        return;
      TerrainEditor.CurrentTerrain.SetShowSelection(true,false);
    }

    private void toolStripButton_Selection_DoubleClick(object sender, EventArgs e)
    {
      if (TerrainEditor.CurrentTerrain == null)
        return;
      TerrainEditor.CurrentTerrain.SetShowSelection(true,true);
    }


    #region Mesh Export


    bool OpenExportSettingsDialog()
    {
      using (PropertyGridDlg dlg = new PropertyGridDlg("Mesh Export Settings", "This property grid shows properties relevant for the terrain mesh export."))
      {
        dlg.DataObject = TerrainEditor.CurrentTerrain.MeshExportSettings; // clones it
        if (dlg.ShowDialog() != DialogResult.OK)
          return false;
        TerrainEditor.CurrentTerrain.MeshExportSettings = (TerrainMeshExport)dlg.DataObject;
        return true;
      }
    }

    #endregion

    #region Billboard Atlas handling

    void TextureAtlas_OnAtlasFileSaved(object sender, EventArgs e)
    {
      if (TerrainEditor.CurrentTerrain == null)
        return;

      // refresh the billboard mesh
      TextureAtlas atlas = (TextureAtlas)sender;
      foreach (DecorationModelResource model in TerrainEditor.CurrentTerrain.DecorationModels)
      {
        if (string.Compare(model.Filename, atlas.Filename, true) != 0)
          continue;
        TerrainEditor.CurrentTerrain.EngineTerrain.ReloadDecorationModel(model);
      }
    }

    private void createNewBillboardAtlasToolStripMenuItem_Click(object sender, EventArgs e)
    {
      CreateFileDlg fileDlg = new CreateFileDlg();
      fileDlg.Caption = "Create a new Billboard Atlas file";
      fileDlg.Description = "Enter the name of the new atlas file and select the directory to create it in. Then press OK to continue.";
      fileDlg.InitialDirectory = EditorManager.Project.ProjectDir;
      fileDlg.Ext = ".atlas";
      fileDlg.AllowOverwrite = false;
      fileDlg.Filter = new string[] { ".atlas" };
      if (fileDlg.ShowDialog() != DialogResult.OK)
        return;

      TextureAtlas atlas = new TextureAtlas();
      atlas.Filename = EditorManager.Project.MakeRelative(fileDlg.FileName);
      TextureAtlasEditorDlg dlg = new TextureAtlasEditorDlg(true);
      dlg.Atlas = atlas;

      if (dlg.ShowDialog(this) != DialogResult.OK)
        return;
      dlg.Atlas.SaveToFile(null);

    }

    private void editExistingBillboardAtlasToolStripMenuItem_Click(object sender, EventArgs e)
    {

      OpenFileDlg fileDlg = new OpenFileDlg();
      fileDlg.ShowBaseData = false;
      fileDlg.Caption = "Open existing Billboard Atlas file";
      fileDlg.Description = "Browse directories to open an existing texture atlas file. Then press OK to edit the atlas.";
      fileDlg.InitialDirectory = EditorManager.Project.ProjectDir;
      fileDlg.Filter = new string[] { ".atlas" };

      // start with selected model (if it is an .atlas file)
      if (TerrainEditor.CurrentDecorationModel != null && string.Compare(Path.GetExtension(TerrainEditor.CurrentDecorationModel.Filename), ".atlas", true) == 0)
        fileDlg.FileName = EditorManager.Project.MakeAbsolute(TerrainEditor.CurrentDecorationModel.Filename);

      if (fileDlg.ShowDialog() != DialogResult.OK)
        return;

      TextureAtlas atlas = TextureAtlas.CreateFromFile(EditorManager.Project.MakeRelative(fileDlg.FileName));
      if (atlas == null)
      {
        EditorManager.ShowMessageBox("Error loading atlas file", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        return;
      }

      TextureAtlasEditorDlg dlg = new TextureAtlasEditorDlg(true);
      dlg.Atlas = atlas;

      if (dlg.ShowDialog(this) != DialogResult.OK)
        return;
      dlg.Atlas.SaveToFile(null);

    }

    #endregion

    #region Pickhandler : Decoration painting component

    public class StaticMeshVegetationPickhandler : IPickHandler
    {
      public StaticMeshVegetationPickhandler(TerrainEditorPanel panel, bool bAdd)
      {
        _terrainPanel = panel;
        _bAdd = bAdd;
      }

      bool _bAdd;
      TerrainEditorPanel _terrainPanel;

      public override bool OnPick(MouseEventArgs e, CSharpFramework.Contexts.KeyModifier eKeyMod)
      {
        base.OnPick(e, eKeyMod);

        Vector3F vStart = new Vector3F();
        Vector3F vEnd = new Vector3F();
        EditorManager.EngineManager.GetRayAtScreenPos(out vStart, out vEnd, e.X, e.Y, EditorManager.Settings.MaxPickingDistance);
        ShapeComponentType compClass = (ShapeComponentType)EditorManager.EngineManager.ComponentClassManager.GetCollectionType("VMeshPaintingComponent");
        if (compClass == null)
          return false;

        ShapeTraceResult result = new ShapeTraceResult();
        ShapeCollection relevantShapes = EditorManager.Scene.RootShapes;
        foreach (ShapeBase shape in relevantShapes)
          shape.TraceShape(ShapeTraceMode_e.ShapePicking, vStart, vEnd, ref result, true);
        if (result.bHit && result.hitShape != null)
        {
          // first find whether a componen of this type is already added:
          ShapeComponent component = null;
          if (result.hitShape.ComponentCount > 0)
            component = result.hitShape.Components.GetComponentByType(compClass);

          if (_bAdd)
          {
            if (component != null)
              return true; // already attached

            component = (ShapeComponent)compClass.CreateInstance(null);
            AddShapeComponentAction action = new AddShapeComponentAction(result.hitShape, component);
            if (action.Valid)
            {
              EditorManager.Actions.Add(action);
              _terrainPanel.UpdateDecoration(false);
            }
            else
              EditorManager.EngineManager.LogPrintWarning("Cannot attach vegetation painting component to shape " + result.hitShape.ShapeName);
          }
          else
          {
            if (component == null) // no component of this type attached
              return true;
            RemoveShapeComponentAction action = new RemoveShapeComponentAction(result.hitShape, component);
            EditorManager.Actions.Add(action);
            _terrainPanel.UpdateDecoration(false);

          }

        }
        
        return true;
      }

      public override string ScreenMessage
      {
        get
        {
          if (_bAdd)
            return "Pick a static mesh to enable terrain vegetation painting on it";
          return "Pick a static mesh to disable terrain vegetation painting on it";
        }
      }
    }

    #endregion

    private void toolStrip1_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
    {

    }

    private void button_ExportPrefab_Click(object sender, EventArgs e)
    {
      if (TerrainEditor.CurrentTerrain == null)
        return;

      // open settngs dialog the first time
      if (!TerrainEditor.CurrentTerrain.HasMeshExportSettings && !OpenExportSettingsDialog())
        return;

      if (EditorManager.ShowMessageBox("The terrain data needs to be saved before the export.\n Continue?", "Save Terrain Data", MessageBoxButtons.YesNo, MessageBoxIcon.Question) != DialogResult.Yes)
        return;

      UnsetTerrainContext();

      EditorManager.Progress.ShowProgressDialog("Exporting terrain as prefab");
      {
        // try to use/create the prefabs directory of the project
        string prefabDir = Path.Combine(EditorManager.Project.ProjectDir, EditorManager.Settings.PrefabDirectory);
        if (!Directory.Exists(prefabDir))
        {
          try
          {
            Directory.CreateDirectory(prefabDir);
          }
          catch (Exception ex)
          {
            EditorManager.DumpException(ex);
          }
        }

        // export each sector of the terrain as a .vmesh file
        TerrainEditor.CurrentTerrain.EngineTerrain.ExportTerrainMeshs(TerrainEditor.CurrentTerrain.MeshExportSettings, EditorManager.Progress, 100.0f, true);
      }
      EditorManager.Progress.HideProgressDialog();
    }

    private void propertyGrid_MeshExport_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
    {
      TerrainEditor.CurrentTerrain.Modified = true;
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
