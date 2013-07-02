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
using System.Collections;
using System.Drawing.Design;
using System.ComponentModel;
using System.Globalization;
using System.Runtime.Serialization;
using CSharpFramework;
using CSharpFramework.Math;
using CSharpFramework.Shapes;
using CSharpFramework.PropertyEditors;
using CSharpFramework.DynamicProperties;
using CSharpFramework.UndoRedo;
using CSharpFramework.Actions;
using CSharpFramework.Scene;
using CSharpFramework.View;
using CSharpFramework.Serialization;
using System.Drawing;
using TerrainManaged;
using TerrainBase;
using TerrainBase.Config;
using System.Windows.Forms;
using TerrainEditorPlugin.Dialogs;
using TerrainBase.Editing;
using TerrainEditorPlugin.Editing;
using TerrainBase.Terrain;
using System.Drawing.Imaging;
using CSharpFramework.Controls;
using ManagedBase;
using CSharpFramework.Helper;
using CSharpFramework.Dialogs;
using CSharpFramework.Visitors;
using TerrainEditorPlugin.Filter;
using CSharpFramework.Tests;
using VisionEditorPlugin.Shapes;


namespace TerrainEditorPlugin.Shapes
{

  #region Class : TerrainShaderProperties

  /// <summary>
  /// Simple wrapper class to show shader properities in the grid
  /// </summary>
  public class TerrainShaderProperties
  {
    public TerrainShaderProperties(TerrainShape owner)
    {
      _owner = owner;
    }

    TerrainShape _owner;

    /// <summary>
    /// Whether an automatic or manual shader should be used
    /// </summary>
    [SortedCategory("Terrain Shader", 1),
    RefreshProperties(System.ComponentModel.RefreshProperties.All),
    Description("AUTO: A matching shader is automatically assigned\n" +
                "MANUAL: A specific shader can be applied manually by selecting it"),
    PropertyOrder(1)]
    public ISurface.SHADERMODE_NO_TEMPLATE Shader_Mode
    {
      get { return _owner.Shader_Mode; }
      set
      {
        _owner.Shader_Mode = value;
        _owner.Modified = true;
      }
    }

    /// <summary>
    /// Shader effect config
    /// </summary>
    [SortedCategory("Terrain Shader", 1), PropertyOrder(3), TypeConverter(typeof(UndoableObjectConverter))]
    public ShaderEffectConfig Shader
    {
      get { return _owner.ShaderConfig; }
      set
      {
        _owner.ShaderConfig = value;
        _owner.Modified = true;
      }
    }
  }

  #endregion

  #region Terrain Shape

  /// <summary>
  /// shape that represents a terrain
  /// </summary>
  [Serializable]
  [RemoveShapeClearsUndoHistory]
  public class TerrainShape : Shape3D, ITerrainCallback, ITerrainObject
  {
    public enum LightmapSize_e
    {
      NoLightmapping = 0,
      SinglePage_256x256 = 1,
      SinglePage_512x512 = 2,
      SinglePage_1024x1024 = 3,
      SinglePage_2048x2048 = 4,
      PerSector_256x256 = 257,
      PerSector_512x512 = 258,
      PerSector_1024x1024 = 259,
      PerSector_2048x2048 = 260,
    }

    #region nested classes: value provider

    public class HeightmapValueProvider : IValueProvider
    {
      public HeightmapValueProvider(TerrainShape terrain)
      {
        _terrain = terrain;
        _heightSampleCount = terrain.TerrainConfig.OverallHeightSampleCount;
      }

      TerrainShape _terrain;
      Size _heightSampleCount;

      #region IValueProvider Members

      public int SizeX
      {
        get { return _heightSampleCount.Width; }
      }

      public int SizeY
      {
        get { return _heightSampleCount.Height; }
      }

      public float ValueAt(int x, int y)
      {
        return _terrain.EngineTerrain.TerrainHeightAtSamplePos(x,y);
      }

      #endregion
    }

    #endregion

    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_EDITING = "Editing";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_EDITING = Shape3D.LAST_CATEGORY_ORDER_ID + 1;

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_VISIBILITY = "Visibility";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_VISIBILITY = Shape3D.LAST_CATEGORY_ORDER_ID + 2;

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_LIGHTING = "Lighting";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_LIGHTING = Shape3D.LAST_CATEGORY_ORDER_ID + 3;

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_LOD = "Level of Detail";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_LOD = Shape3D.LAST_CATEGORY_ORDER_ID + 4;

    /// <summary>
    /// last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_LOD;

    #endregion

    #region Constructor

    public TerrainShape(string name)
      : base(name)
    {
			AddHint(HintFlags_e.NoRotation | HintFlags_e.NoScale | HintFlags_e.RetainPositionAtCreation);
			_terrainConfig.TerrainFolder = System.IO.Path.GetFileNameWithoutExtension(EditorManager.Scene.FileName);

      // start with the defaults
      ShaderConfig = new ShaderEffectConfig(this);
      _terrainShaderProperties = new TerrainShaderProperties(this);

      _shaderFX._libFilename = @"\Shaders\Terrain.ShaderLib";
      _shaderFX._effectName = "Terrain";
      _shaderFX._shadermode = ISurface.SHADERMODE.AUTO;
		}

    #endregion

    #region Engine Instance

    /// <summary>
    /// Make sure the native zone classes have unique ID
    /// </summary>
    void AssignNativeZoneIDs()
    {
      if (_visibilityZones==null)
        return;
      foreach (TerrainVisibilityZoneShape zone in _visibilityZones)
        EngineTerrain.SetZoneUniqueID(zone.SectorX, zone.SectorY, zone.UniqueID);
    }


    public override void CreateEngineInstance(bool bCreateChildren)
    {
      bool bRebuildVis = false;
      if (_engineInstance == null)
      {
        _engineInstance = new EngineInstanceTerrain(this);
        EngineTerrain.SetReferenceStatus(_bIsReference);

        _bNeedsConfigSetup = true;
        bRebuildVis = true;
      }
      EngineTerrain.UpdatePhysicsMeshes();
      SetEngineInstanceBaseProperties(); // initializes the underlying native terrain with the current config

      base.CreateEngineInstance(bCreateChildren); // call after SetEngineInstanceBaseProperties since we need a valid native terrain for component assignment

      // as this terrain affects visibility:
      if (bRebuildVis)
      {
        EditorManager.VisibilityBuilder.Build(this.ParentLayer.ParentScene, null);
        // the function call above triggers AddToSceneManager on the native side. So make sure now that zones get their unique ID, otherwise loading
        // of sectors with ID based visibility zones (billboard group) will go wrong
        AssignNativeZoneIDs();
      }

      TerrainEditor.CurrentTerrain = this;
    }

    public override void RemoveEngineInstance(bool bRemoveChildren)
    {
      // pretend that we have no instance to keep the instance
      IEngineShapeInstance _keepInst = _engineInstance;
      _engineInstance = null;

      base.RemoveEngineInstance(bRemoveChildren);

      _engineInstance = _keepInst;
    }


    public override void SetEngineInstanceBaseProperties()
    {
      if (!HasEngineInstance())
        return;

      EngineTerrain.SetReferenceStatus(_bIsReference);
      if (_bNeedsConfigSetup)
      {
        EngineTerrain.SetConfig(_terrainConfig, this.DetailTextures, this.DecorationModels);
        RecreateVisibilityZones();
        Position = _terrainConfig.TerrainPos; // get back position

        // add the first detail texture by default
        if (!string.IsNullOrEmpty(_terrainConfig.BaseTextureFile) && this.DetailTextures.Count == 0)
        {
          _terrainConfig._baseTexture.ID = GetNewDetailTextureID();
          this.AddDetailTexture(_terrainConfig._baseTexture);
        }
        if (IsReference)
        {
          _lodMetric = EngineTerrain.GetLODMetric();
        }
        _bNeedsConfigSetup = false;
      }

      base.SetEngineInstanceBaseProperties();
      EngineTerrain.SetVisibleBitmask(_iVisibleBitmask);
      EngineTerrain.SetVisibleInVisZones(_bVisibleInsideVisZones);
      UpdateLightmappingMode();
      EngineTerrain.SetLODScaling(_lodMetric, _fLODScaling, _fMirrorLODBias);
      EngineTerrain.SetCastStaticShadows(_bCastStaticShadows);
      EngineTerrain.SetLightInfluenceBitmask(_iLightInfluenceBitmask);
      EngineTerrain.SetShaderEffect(ShaderConfig);
      EngineTerrain.SetUseBakedTextures(TerrainEditor.TextureBakedView, ShaderConfig);
      EngineTerrain.SetPreviewDirectionalLight(TerrainEditor.DirectionalLighting);
      EngineTerrain.SelectTerrainBrush(TerrainEditor.CurrentBrush);
      EngineTerrain.SetExportAsEditable(_bExportAsEditable);
      EngineTerrain.SetFastUpdate(_bFastUpdate);
      EngineTerrain.SetMinimapTexture(MinimapTexture);
      EngineTerrain.SetSortDetailTextures(_bSortDetailTextures);
    }


    [Browsable(false)]
    public EngineInstanceTerrain EngineTerrain { get { return (EngineInstanceTerrain)_engineInstance; } }


    void UpdateLightmappingMode()
    {
      if (!HasEngineInstance())
        return;
      int iPageSize = 0;
      bool bPerSector = false;
      switch (this._eLightmapSize)
      {
        case LightmapSize_e.SinglePage_256x256: iPageSize = 256; break;
        case LightmapSize_e.SinglePage_512x512: iPageSize = 512; break;
        case LightmapSize_e.SinglePage_1024x1024: iPageSize = 1024; break;
        case LightmapSize_e.SinglePage_2048x2048: iPageSize = 2048; break;

        case LightmapSize_e.PerSector_256x256: iPageSize = 256; bPerSector = true; break;
        case LightmapSize_e.PerSector_512x512: iPageSize = 512; bPerSector = true; break;
        case LightmapSize_e.PerSector_1024x1024: iPageSize = 1024; bPerSector = true; break;
        case LightmapSize_e.PerSector_2048x2048: iPageSize = 2048; bPerSector = true; break;
      }

      EngineTerrain.SetLightmapPageSize(iPageSize, bPerSector);
      if (IsReference)
        _bSceneSpecificLightmaps = EngineTerrain.UsesSceneSpecificLightmaps();
      else
        EngineTerrain.SetUseSceneSpecificLightmaps(_bSceneSpecificLightmaps);
    }

    /// <summary>
    /// Updates the terrain shader if the mode is set to AUTO.
    /// </summary>
    void UpdateAutoTerrainEffect()
    {
      if (Shader_Mode != ISurface.SHADERMODE_NO_TEMPLATE.AUTO)
        return;

      // Update effect
      ShaderConfig.UpdateEffect();

      // Make sure property grid is up-to-date
      TerrainEditor.EditorPanel.RefreshShaderEffectPropertyGrid();
    }

    #endregion

    #region Shape overrides

    public override bool CanCopyPaste
    {
      get
      {
        return false;
      }
    }

    public override void OnSelected()
    {
      base.OnSelected();
      TerrainEditor.CurrentTerrain = this; // every time a terrain is selected, set it as the current one for editing
    }

		public override void OnAddedToScene()
		{
			base.OnAddedToScene();

      TerrainEditor.OnCurrentBrushChanged += new EventHandler(TerrainEditor_OnCurrentBrushChanged);

      EditorManager.SceneEvent += new SceneEventHandler(EditorManager_SceneEvent);

      this.RecreateVisibilityZones();
      Scene2DRenderingEnabled = true;
    }


    public override void OnTransformationChanged()
    {
      base.OnTransformationChanged();
      EditorManager.VisibilityBuilder.Dirty = true;
    }

    public override bool GetLocalBoundingBox(ref BoundingBox bbox)
    {
      bbox.Set(0, 0, -1000.0f, Config.OverallSizeX, Config.OverallSizeY, 1000.0f);
      return true;
    }


    public override void PerformPostEngineInstanceCreationSetup(bool bCreateChildren)
    {
      base.PerformPostEngineInstanceCreationSetup(bCreateChildren);
      SetShowDirectionalLightShape(false);

      if (PendingCreateFromV3D)
      {
        Debug.Assert(HasEngineInstance());
        EngineTerrain.CreateFromCurrentV3D(_terrainConfig);
        Position = _terrainConfig.TerrainPos; // get back position

        // import the base texture as a detail texture
        if (!string.IsNullOrEmpty(_terrainConfig.BaseTextureFile))
        {
          DetailTextureResource basetex = CreateDetailTexture();
          basetex.DiffuseFilename = _terrainConfig.BaseTextureFile;
          basetex.Tiling = new Vector2F(1, 1);
          basetex.Name = "Base Texture";
          AddDetailTexture(basetex);
        }

        this.RecreateVisibilityZones();
        EditorManager.VisibilityBuilder.Build(this.ParentLayer.ParentScene, null);
        _bNeedsConfigSetup = false;
        PendingCreateFromV3D = false;
      }
    }

    protected override void DisposeObject()
    {
      if (_shaderFX != null)
      {
        _shaderFX.Dispose();
        _shaderFX = null;
      }
      if (_pendingConfig != null)
      {
        _pendingConfig.Dispose();
        _pendingConfig = null;
      }
      base.DisposeObject();
    }


		public override void OnRemoveFromScene()
		{
			if (TerrainEditor.CurrentTerrain == this)
				TerrainEditor.CurrentTerrain = null;

      RemoveVisibilityZones();

      // finally delete our engine instance here
      if (this._engineInstance != null)
      {
        // Important: We must call the base implementation here because the overload in this class prevents the deletion
        base.RemoveEngineInstance(false);

        // rebuild visibility as we might have destroyed the last visibility zones
        EditorManager.VisibilityBuilder.Build(this.ParentLayer.ParentScene, null);
      }
      TerrainEditor.OnCurrentBrushChanged -= new EventHandler(TerrainEditor_OnCurrentBrushChanged);

      EditorManager.SceneEvent -= new SceneEventHandler(EditorManager_SceneEvent);

      SetShowDirectionalLightShape(false);

			base.OnRemoveFromScene();
		}


    protected override Shape3D.ParentingModes ParentingMode
    {
      get
      {
        return ParentingModes.SceneGraphScaled;
      }
    }

    protected override void GetStaticLightInfo(StaticLightInfo info)
    {
      if (this.CastStaticShadows)
        info.RelevantOccluder.Add(this);
      if (this.Lightmapped)
        info.RelevantReceiver.Add(this);
    }


    /// <summary>
    /// Overridden function to set camera onto terrain
    /// </summary>
    public override void OnDoubleClick()
    {
      Vector3F pos = EditorManager.ActiveView.CameraPosition;
      pos.Z = TerrainHeightAtPosition(pos) + 400.0f;

      EditorManager.ActiveView.SetCameraPosition(pos);
      EditorManager.ActiveView.UpdateView(false);
    }

    /*
    public override bool Modifiable
    {
      get
      {
        if (_bIsReference)
          return false;
        return base.Modifiable;
      }
    }
    */

    public override PropertyFlags_e GetPropertyFlags(PropertyFlagsProviderInfo pd)
    {
      if (pd.Name == "SceneSpecificLightmaps")
      {
        if (!Lightmapped)
          return PropertyFlags_e.Hidden;
        if (IsReference)
          return PropertyFlags_e.Readonly;
      }
      if (IsReference)
      {
        // these properties cannot be edited when the terrain is a reference
        if (pd.Name == "LODScheme" || pd.Name == "SortDetailTextures" || pd.Name == "x" || pd.Name == "y" || pd.Name == "z")
          return PropertyFlags_e.Readonly;
      }
      return base.GetPropertyFlags(pd);
    }

    public override TargetPlatformSupport GetSupportedPlatforms()
    {
      return TargetPlatformSupport.All & ~(TargetPlatformSupport.Mobile);
    }

    #endregion

    #region Important listeners

    void TerrainEditor_OnCurrentBrushChanged(object sender, EventArgs e)
    {
      if (HasEngineInstance())
        EngineTerrain.SelectTerrainBrush((TerrainBrush)sender);
    }

    private ZoneCollection exportedZones;

    void EditorManager_SceneEvent(object sender, SceneEventArgs e)
    {
      if (e.action == SceneEventArgs.Action.AllowExport)
      {
        if (MeshExportSettings.AutomaticExport && MeshExportSettings.ExportForAssetProfile.IsActiveProfileSet)
        {
          if (EditorManager.ShowMessageBox("The terrain data needs to be saved before the export.\n Continue?", "Save Terrain Data", MessageBoxButtons.YesNo, MessageBoxIcon.Question) != DialogResult.Yes)
          {
            if (e is ExportSceneEventArgs)
            {
              ((ExportSceneEventArgs)e).CancelExport = true;
            }
          }
        }
      }
      else if (e.action == SceneEventArgs.Action.BeforeExport && HasEngineInstance())
      {
        if (e is ExportSceneEventArgs)
        {
          if (MeshExportSettings.ExportForAssetProfile.IsActiveProfileSet)
          {
            if (!MeshExportSettings.AutomaticExport)
              return;

            EditorManager.ActiveView.SetDefaultContext();

            int iCountX = 0;
            int iCountY = 0;
            if (!EngineTerrain.GetSectorMeshCount(ref iCountX, ref iCountY))
              return;

            EditorManager.EngineManager.DisableRendering();
            EditorManager.Progress.ShowProgressDialog("Exporting terrain sectors as static meshes");
            
            try
            {
              // export each sector of the terrain as a .vmesh file
              EngineTerrain.ExportTerrainMeshs(MeshExportSettings, EditorManager.Progress, 50.0f, false);

              // collection which holds all exported zones, these zones are removed after the export (see below: SceneEventArgs.Action.AfterExport)
              exportedZones = new ZoneCollection();

              // disable visibility zone disconnecting
              EngineTerrain.SetTerrainMeshExportFlag(true);

              float fStep = 50.0f / (float)(iCountX*iCountY);
              float fPercentage = 50.0f;

              // create zone, layer, and static mesh for each sector of the terrain
              for (int sy = 0; sy < iCountY; ++sy)
              {
                for (int sx = 0; sx < iCountX; ++sx)
                {
                  string sectorFileName = EngineTerrain.GetSectorMeshFileName(sx, sy);
                  if (sectorFileName == null)
                    continue;

                  // create static mesh
                  StaticMeshShape shape = new StaticMeshShape(Path.GetFileNameWithoutExtension(sectorFileName));
                  shape.Position = (Vector3F)EngineTerrain.GetSectorOrigin(sx, sy);
                  shape.MeshFileName = sectorFileName;
                  string lightmapFileName = EngineTerrain.GetSectorLightmapName(sx, sy);
                  if (lightmapFileName != null)
                    shape.CustomLightmap = lightmapFileName;

                  // create zone
                  string uniqueFileName = FileHelper.CreateUniqueFilename(EditorManager.Scene.LayerDirectoryName, "TerrainSectorZone" + sx + "_" + sy, IScene.ZoneFileExtension);
                  object[] zoneParameters = { EditorManager.Scene.CreateUniqueZoneName("TerrainSectorZone" + sx + "_" + sy) };
                  Zone zone = EditorManager.ZoneFactory.CreateInstance(zoneParameters);
                  zone.SetZoneFileNameInternal(uniqueFileName);
                  zone.LoadedDistance = MeshExportSettings.LoadedDistance;
                  zone.CacheInMargin = MeshExportSettings.CacheInMargin;
                  zone.CacheOutMargin = MeshExportSettings.CacheOutMargin;
                  EditorManager.Scene.AddZone(zone);

                  // create layer
                  string uniqueFile = EditorManager.Scene.Layers.CreateUniqueLayerFileName(EditorManager.Scene.LayerDirectoryName, "TerrainSector" + sx + "_" + sy);
                  object[] layerParameters = { "TerrainSector" };
                  Layer layer = EditorManager.LayerFactory.CreateInstance(layerParameters);
                  layer.SetLayerFileNameInternal(uniqueFile);
                  layer.AttachToZone(zone);
                  EditorManager.Scene.AddLayer(layer, true);

                  layer.AddShape(shape, shape.Parent);

                  // add zone to export info, so that it gets exported
                  ((ExportSceneEventArgs)e).ExportInfo.RelevantExportZones.Add(zone);
                  ((ExportSceneEventArgs)e).ExportInfo.RelevantEmbeddedZones.Add(zone);

                  // keep reference to be able to remove the zone after export
                  exportedZones.Add(zone);

                  fPercentage += fStep;
                  if (EditorManager.Progress != null)
                    EditorManager.Progress.Percentage = fPercentage;

                  Application.DoEvents();
                }
              }
            }
            finally
            {
              EditorManager.Progress.HideProgressDialog();
              EditorManager.EngineManager.EnableRendering();
            }
          }
        }
      }
      else if (e.action == SceneEventArgs.Action.AfterExport && HasEngineInstance())
      {
        if (e is ExportSceneEventArgs)
        {
          if (MeshExportSettings.ExportForAssetProfile.IsActiveProfileSet)
          {
            if (!MeshExportSettings.AutomaticExport)
              return;

            // re-enable default behaviour for terrain visibility zone serialization
            EngineTerrain.SetTerrainMeshExportFlag(false);

            foreach (Zone zone in exportedZones)
            {
              // create .deleted filenames
              string absLayerFileName = zone.Layers[0].AbsoluteLayerFilename + ".deleted";
              string absZoneFileName = zone.AbsoluteZoneFilename + ".deleted";

              EditorManager.Scene.RemoveZone(zone);

              // delete .deleted files, because they are not needed in any way
              if (File.Exists(absLayerFileName))
                File.Delete(absLayerFileName);
              if (File.Exists(absZoneFileName))
                File.Delete(absZoneFileName);
            }

            exportedZones.Clear();
          }
        }
      }
    }

    public override void OnSceneEvent(SceneEventArgs e)
    {
      base.OnSceneEvent(e);

      // save the native terrain sectors also when the scene is saved
      if (e.action == SceneEventArgs.Action.AfterSaving && e.Success && HasEngineInstance())
        EngineTerrain.SaveToFile();

      // after lighting, we should mark the layer as dirty to save the terrain sector files
      //      if (e.action == SceneEventArgs.Action.AfterStaticLighting && e.Success && HasEngineInstance() && Lightmapped && Modifiable)
      //        this.Modified = true;
      if (e.action == SceneEventArgs.Action.AfterRendererNodeChanged && HasEngineInstance())
        EngineTerrain.SetShaderEffect(this.ShaderConfig);
    }


    #endregion

    #region Members

    //string _terrainFile = "";
		TerrainConfig _terrainConfig = new TerrainConfig();
    public bool _bIsReference = false; // determines whether this terrain is a reference of another terrain

    public int _iDecorationID = 1;
    public int _iDetailTextureID = 1;
    ArrayList _decorationModels = new ArrayList();
    ArrayList _detailTextures = new ArrayList();
    bool _bPendingCreateFromV3D = false;
    bool _bNeedsConfigSetup = true;
    // editable properties:
    uint _iVisibleBitmask = 0xffffffff;
    bool _bVisibleInsideVisZones = false;
    float _fLODScaling = 1.0f;
    float _fMirrorLODBias = 1.0f;
    TerrainLODMetric_e _lodMetric = TerrainLODMetric_e.DistanceOnly_NoHeight;

    bool _bExportAsEditable = false;
    bool _bFastUpdate = false;

    LightmapSize_e _eLightmapSize = LightmapSize_e.NoLightmapping;
    bool _bSceneSpecificLightmaps = false;
    bool _bCastStaticShadows = true;
    ushort _iLightInfluenceBitmask = 0xffff;
    ShaderEffectConfig _shaderFX = null;
    public TerrainShaderProperties _terrainShaderProperties;

    //TerrainMaterialProvider _materialProvider = null;
    string _minimapTexture = null;
    Image _minimapImage = null;
    string _heightmapExport = null;
    bool _bSortDetailTextures = true;
    bool _bNormalizeHeightmapExport = false;
    public bool _bBakeMode = false;

    #endregion

    #region Properties

    /// <summary>
    /// Override this property to add support for it. Note that loading/saving is handled through base class already
    /// </summary>
    eNavMeshUsage _navMeshUsage = eNavMeshUsage.IncludeInNavMesh;
    [Description("Determines how shape is used during nav mesh building")]
    public override eNavMeshUsage NavMeshUsage
    {
      get { return _navMeshUsage; }
      set { _navMeshUsage = value; }
    }

    [EditorAttribute(typeof(TerrainConfigEditor), typeof(UITypeEditor))]
    [SortedCategory(CAT_EDITING, CATORDER_EDITING), PropertyOrder(1)]
    public TerrainConfig Config
    {
      get
      {
        // ensure the terrain position is always up-to-date
        if (!_bNeedsConfigSetup) // update in progress...
          _terrainConfig.TerrainPosInternal = this.Position;

        return _terrainConfig;
      }
      set
      {
        if (_terrainConfig == value)
          return;

        int iOldSectorCountX = _terrainConfig == null ? 0 : _terrainConfig.SectorCountX;
        int iOldSectorCountY = _terrainConfig == null ? 0 : _terrainConfig.SectorCountY;
        // warning dialog:

        if (value.Modified)
        {
          DialogResult res = EditorManager.ShowMessageBox("Significant properties of the terrain have been changed.\nThis operation is not undo-able and it will additionally clear the current Undo/Redo history.\n\nContinue anyway?", "Terrain config changes", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
          if (res != DialogResult.Yes)
            return;
        }

      
        if (_terrainConfig.TerrainFolder != value.TerrainFolder)
        {
          string oldTerrainFolder = _terrainConfig.TerrainFolder;
          if (MoveTerrainFolder(value.TerrainFolder, false) == false)
          {
            value.TerrainFolder = oldTerrainFolder; // If terrain moving is failed , recover old terrain folder path.
          }
        }

        _terrainConfig = value;
        bool bSectorCountChanged = (iOldSectorCountX != _terrainConfig.SectorCountX || iOldSectorCountY != _terrainConfig.SectorCountY);

        if (_terrainConfig.Modified)
        {
          _terrainConfig.Modified = false;

          EditorManager.Actions.Reset();

          if (bSectorCountChanged)
            RemoveVisibilityZones();
          this.Position = _terrainConfig.TerrainPos;
          EngineTerrain.SetModifiableConfig(_terrainConfig);
          if (bSectorCountChanged)
            RecreateVisibilityZones();
          this.UpdateSectorBoundingBoxes();

          // Make sure detail texture resources contain valid sector index information
          this.ClampDetailTextureSectorsToValidRange();

          if (Modifiable)
            Modified = true; // mark as dirty

          EditorManager.VisibilityBuilder.Build(EditorManager.Scene, null);
          EditorManager.ActiveView.UpdateView(false);
          EditorManager.ActiveView.Gizmo.RecalcGizmoPos();
        }
      }
    }

    
    /// <summary>
    /// Project relative directory where the terrain data is stored
    /// </summary>
    [SortedCategory(CAT_EDITING, CATORDER_EDITING), PropertyOrder(2)]
    [Description("Project relative directory where the terrain data is stored")]
    public string TargetDirectory
    {
      get
      {
        return _terrainConfig._terrainFolder;
      }
    }

    /// <summary>
    /// Project relative directory where the terrain data is stored
    /// </summary>
    [SortedCategory(CAT_EDITING, CATORDER_EDITING), PropertyOrder(3)]
    [Description("Determines whether this terrain is just a reference to an existing terrain")]
    public bool IsReference
    {
      get
      {
        return _bIsReference;
      }
    }

    /// <summary>
    /// Indicates whether this is the current terrain
    /// </summary>
    [Browsable(false)]
    [SortedCategory(CAT_EDITING, CATORDER_EDITING), PropertyOrder(3)]
    [Description("Indicates whether this is the current terrain")]
    public bool IsCurrent
    {
      get
      {
        return TerrainEditor.CurrentTerrain == this;
      }
    }


    [PrefabResolveFilename]
    [Description("Optional mini map texture that is shown in the 2D overview panel.")]
    [SortedCategory(CAT_EDITING, CATORDER_EDITING), PropertyOrder(4)]
    [EditorAttribute(typeof(AssetEditor), typeof(UITypeEditor)), AssetDialogFilter(new string[] { "Texture" })]
    public string MinimapTexture
    {
      get { return _minimapTexture; }
      set
      {
        if (_minimapTexture == value)
          return;
        _minimapTexture = value;
        _minimapImage = null;
        string info;
        if (!string.IsNullOrEmpty(_minimapTexture))
          _minimapImage = ImageLoader.LoadImage(_minimapTexture, out info);
        Scene2DView.Update2DViews();
        if (HasEngineInstance())
          EngineTerrain.SetMinimapTexture(_minimapTexture);

        TerrainEditor.TriggerOnMiniMapStatusChanged();
      }
    }

    [Browsable(false)]
    public bool HasMinimapTexture
    {
      get
      {
        return !string.IsNullOrEmpty(_minimapTexture);
      }
    }


    /// <summary>
    /// Cached filename of the exported heightmap file
    /// </summary>
    [Browsable(false)]
    public string HeightmapExportFilename
    {
      get { return _heightmapExport; }
      set
      {
        if (_heightmapExport == value)
          return;
        _heightmapExport = value;
        Modified = true;
        if (TerrainEditor.CurrentTerrain == this)
          TerrainEditor.TriggerOnHeightmapExportFilenameChanged();
      }
    }

    /// <summary>
    /// If enabled, the heightmap export remaps to [0..1] range rather than using absolute float values
    /// </summary>
    [Browsable(false)]
    public bool NormalizeHeightmapExport
    {
      get
      {
        return _bNormalizeHeightmapExport;
      }
      set
      {
        if (_bNormalizeHeightmapExport == value)
          return;
        _bNormalizeHeightmapExport = value;
        Modified = true;
        if (TerrainEditor.CurrentTerrain == this)
          TerrainEditor.TriggerOnHeightmapExportFilenameChanged();
      }
    }

    

    /// <summary>
    /// Terrain visibility bitmask that determines the visible status in the rendering contexts
    /// </summary>
    [SortedCategory(CAT_VISIBILITY, CATORDER_VISIBILITY), PropertyOrder(1)]
    [Description("Terrain visibility bitmask that determines the visible status in the rendering contexts")]
    public FlagsInt32_e VisibleBitmask
    {
      get { return (FlagsInt32_e)_iVisibleBitmask; }
      set
      {
        _iVisibleBitmask = (uint)value;
        if (HasEngineInstance())
          EngineTerrain.SetVisibleBitmask(_iVisibleBitmask);
      }
    }

    /// <summary>
    /// If enabled the terrain will be visible inside inner visibility zones such as caves, otherwise it will be culled away. This flag is ignored if no portal/zone visibility setup is used.
    /// </summary>
    [SortedCategory(CAT_VISIBILITY, CATORDER_VISIBILITY), PropertyOrder(2)]
    [Description("If enabled the terrain will be visible inside inner visibility zones such as caves, otherwise it will be culled away. This flag is ignored if no portal/zone visibility setup is used.")]
    public bool VisibleInsideVisZones
    {
      get { return _bVisibleInsideVisZones; }
      set
      {
        if (_bVisibleInsideVisZones == value)
          return;
        _bVisibleInsideVisZones = value;
        if (HasEngineInstance())
          EngineTerrain.SetVisibleInVisZones(_bVisibleInsideVisZones);
        EditorManager.VisibilityBuilder.Dirty = true;
      }
    }

    
    /// <summary>
    /// Specifies the light map page size calculated for this terrain.
    /// </summary>
    [Description("Specifies the light map page size calculated for this terrain.")]
    [SortedCategory(CAT_LIGHTING, CATORDER_LIGHTING), PropertyOrder(1)]
    public LightmapSize_e LightmapSize
    {
      get { return _eLightmapSize; }
      set
      {
        _eLightmapSize = value;
        UpdateLightmappingMode();
        if (_eLightmapSize == LightmapSize_e.NoLightmapping && HasEngineInstance())
          EngineTerrain.SetFullbrightLightmaps(); // only call this when switching

        // update the terrain effect
        UpdateAutoTerrainEffect();
      }
    }

    [Browsable(false)]
    public bool Lightmapped
    {
      get { return _eLightmapSize != LightmapSize_e.NoLightmapping; }
    }


    /// <summary>
    /// Determines whether lightmaps should be scene specific when the terrain is referenced across scenes. If not, the same lightmaps are shared across all scenes that use this terrain
    /// </summary>
    [Description("Determines whether lightmaps should be scene specific when the terrain is referenced across scenes. If not, the same lightmaps are shared across all scenes that use this terrain")]
    [SortedCategory(CAT_LIGHTING, CATORDER_LIGHTING), PropertyOrder(2)]
    public bool SceneSpecificLightmaps
    {
      get { return _bSceneSpecificLightmaps; }
      set
      {
        _bSceneSpecificLightmaps = value;
        UpdateLightmappingMode();
      }
    }

    
    
    /// <summary>
    /// Gets the number of decoration objects in all currently loaded sectors
    /// </summary>
    [Browsable(false)]
    public int DecorationObjectCount
    {
      get
      {
        if (!HasEngineInstance())
          return 0;

        return EngineTerrain.GetDecorationObjectCount();
      }
    }

    

    /// <summary>
    /// Indicates whether this terrain should cast static shadows for lightmaps and light grids.
    /// </summary>
    [Description("Indicates whether this terrain should cast static shadows for lightmaps and light grids.")]
    [SortedCategory(CAT_LIGHTING, CATORDER_LIGHTING), PropertyOrder(10)]
    public bool CastStaticShadows
    {
      get { return _bCastStaticShadows; }
      set
      {
        _bCastStaticShadows = value;
        if (HasEngineInstance())
          EngineTerrain.SetCastStaticShadows(_bCastStaticShadows);
      }
    }

    /// <summary>
    /// Specifies the bitmask for static and dynamic lights that should affect this terrain. See corresponding filter mask on light source.
    /// </summary>
    [Description("Specifies the bitmask for static and dynamic lights that should affect this terrain. See corresponding filter mask on light source.")]
    [SortedCategory(CAT_LIGHTING, CATORDER_LIGHTING), PropertyOrder(20)]
    public FlagsInt16_e LightInfluenceBitmask
    {
      get { return (FlagsInt16_e)_iLightInfluenceBitmask; }
      set
      {
        _iLightInfluenceBitmask = (ushort)value;
        if (HasEngineInstance())
          EngineTerrain.SetLightInfluenceBitmask(_iLightInfluenceBitmask);
      }
    }


    /// <summary>
    /// Determines how LOD is computed for the terrain
    /// </summary>
    [SortedCategory(CAT_LOD, CATORDER_LOD), PropertyOrder(1)]
    [Description("Determines how LOD is computed for the terrain. LOD is based on distance and optionally also considers the heightmap noise in the sector. Furthermore there are variants of each mode (_NoHeight postfix) that cause the distance calculation to ignore the camera height")]
    public TerrainLODMetric_e LODScheme
    {
      get { return _lodMetric; }
      set
      {
        _lodMetric = value;
        if (HasEngineInstance())
          EngineTerrain.SetLODScaling(_lodMetric, _fLODScaling, _fMirrorLODBias);
      }
    }

    /// <summary>
    /// Relative LOD factor for the terrain. LOD switching is less aggressive for larger values.
    /// </summary>
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.0f, 4.0f, 400), PropertyLiveUpdate("LODScalingLiveUpdate")]
    [SortedCategory(CAT_LOD, CATORDER_LOD), PropertyOrder(2)]
    [Description("Relative LOD factor for the terrain. LOD switching is less aggressive for larger values.")]
    public float LODScaling
    {
      get { return _fLODScaling; }
      set
      {
        _fLODScaling = value;
        if (_fLODScaling < 0.0f)
          _fLODScaling = 0.0f;
        if (HasEngineInstance())
          EngineTerrain.SetLODScaling(_lodMetric, _fLODScaling, _fMirrorLODBias);
        if (TerrainEditor.PreviewFullRes)
          EditorManager.EngineManager.LogPrintWarning("LOD has no visual effect at the moment because it is turned off in the terrain editor toolbar");
      }
    }

    /// <summary>
    /// LOD bias value for rendering the terrain into mirrors or water
    /// </summary>
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.0f, 4.0f, 400), PropertyLiveUpdate("LODMirrorBiasLiveUpdate")]
    [SortedCategory(CAT_LOD, CATORDER_LOD), PropertyOrder(3)]
    [Description("LOD bias value for rendering the terrain into mirrors or water. This is a useful optimization as terrain does not need to be rendered with full resolution into reflections. A value of 1.0 (default) biases the LOD by one level in the mirror.")]
    public float LODMirrorBias
    {
      get { return _fMirrorLODBias; }
      set
      {
        _fMirrorLODBias = value;
        if (_fMirrorLODBias < 0.0f)
          _fMirrorLODBias = 0.0f;
        if (_fMirrorLODBias > 4.0f)
          _fMirrorLODBias = 4.0f;
        if (HasEngineInstance())
          EngineTerrain.SetLODScaling(_lodMetric, _fLODScaling, _fMirrorLODBias);
      }
    }


    [Browsable(false)]
    public object LODScalingLiveUpdate
    {
      set
      {
        float fLOD = _fLODScaling;
        if (value is float)
          fLOD = (float)value;
        if (HasEngineInstance())
        {
          EngineTerrain.SetLODScaling(_lodMetric, fLOD, _fMirrorLODBias);
          EditorManager.ActiveView.UpdateView(false);
        }
      }
    }

    [Browsable(false)]
    public object LODMirrorBiasLiveUpdate
    {
      set
      {
        float fLOD = _fMirrorLODBias;
        if (value is float)
          fLOD = (float)value;
        if (HasEngineInstance())
        {
          EngineTerrain.SetLODScaling(_lodMetric, _fLODScaling, fLOD);
          EditorManager.ActiveView.UpdateView(false);
        }
      }
    }

    [Description("Indicates whether editing functionality should be available inside the engine. Editable terrains add a bit of overhead.")]
    [SortedCategory(CAT_EDITING, CATORDER_EDITING), PropertyOrder(5)]
    public bool ExportAsEditable
    {
      get { return _bExportAsEditable; }
      set 
      { 
        _bExportAsEditable = value;
        if (HasEngineInstance())
          EngineTerrain.SetExportAsEditable(_bExportAsEditable);
      }
    }

    [Description("If enabled, dynamic terrain update is significantly faster and suitable for realtime updates. On the downside, the memory footprint is higher.")]
    [SortedCategory(CAT_EDITING, CATORDER_EDITING), PropertyOrder(6)]
    public bool FastUpdate
    {
      get { return _bFastUpdate; }
      set 
      {
        _bFastUpdate = value;
        if (HasEngineInstance())
        {
          EngineTerrain.SetFastUpdate(_bFastUpdate);
          if (Config._bNormalmapUsesBorder && _bFastUpdate)
            EditorManager.ShowMessageBox("The 'fast update' mode conflicts with the\n'NormalmapUsesBorder' flag in the terrain config.\nFor best performance, the 'NormalmapUsesBorder' flag should be turned off.", "Change to 'fast update' mode", MessageBoxButtons.OK, MessageBoxIcon.Information);
          if (this.LODScheme== TerrainLODMetric_e.DistanceAndNoise || this.LODScheme== TerrainLODMetric_e.DistanceAndNoise_NoHeight)
            EditorManager.ShowMessageBox("The 'fast update' mode conflicts with the\n'LODScheme' terrain property.\nFor best performance, the 'LODScheme' should be a 'distance only' mode.", "Change to 'fast update' mode", MessageBoxButtons.OK, MessageBoxIcon.Information);

        }
      }
    }
    

    /// <summary>
    /// Gets or sets the current terrain config
    /// </summary>
    [Browsable(false)]
    [SortedCategory(CAT_EDITING, CATORDER_EDITING), PropertyOrder(3)]
		[EditorAttribute(typeof(TerrainConfigEditor), typeof(UITypeEditor))]
		public TerrainConfig TerrainConfig
		{
			get { return _terrainConfig; }
			set 
			{
        if (_terrainConfig == value)
          return;

				_terrainConfig = value;
        OnConfigChanged();
        if (HasEngineInstance())
        {
          _bNeedsConfigSetup = false;
          EngineTerrain.SetConfig(_terrainConfig, this.DetailTextures, this.DecorationModels);
        }
			}
		}


    [Browsable(false)]
    public ShaderEffectConfig ShaderConfig
    {
      get
      {
        return this._shaderFX;
      }
      set
      {
        if (_shaderFX == value)
          return;

        if (_shaderFX != null)
        {
          _shaderFX.OnShaderEffectChanged -= new ShaderEffectConfig.ShaderEffectChangedEventHandler(_shaderFX_OnShaderEffectChanged);
          _shaderFX.Dispose();
        }

        _shaderFX = value;

        if (_shaderFX != null)
        {
          _shaderFX.OnShaderEffectChanged += new ShaderEffectConfig.ShaderEffectChangedEventHandler(_shaderFX_OnShaderEffectChanged);
          _shaderFX.Owner = this;
          _shaderFX.UpdateEffect();

          TerrainEditor.EditorPanel.RefreshShaderEffectPropertyGrid();
        }
      }
    }

    [Browsable(false)]
    public ISurface.SHADERMODE_NO_TEMPLATE Shader_Mode
    {
      get
      {
        // In case the internal mode is TEMPLATE, we return AUTO.
        // This should never happen as the terrain code does not allow you to switch to template and does not support it.
        if (ShaderConfig._shadermode == ISurface.SHADERMODE.TEMPLATE)
        {
          return ISurface.SHADERMODE_NO_TEMPLATE.AUTO;
        }
        return (ISurface.SHADERMODE_NO_TEMPLATE)ShaderConfig._shadermode;
      } 
      set
      {
        if (ShaderConfig._shadermode == (ISurface.SHADERMODE)value)
          return;
        ShaderConfig._shadermode = (ISurface.SHADERMODE)value;
        ShaderConfig.UpdateEffect();
      }
    }

    void _shaderFX_OnShaderEffectChanged(object sender, ShaderEffectConfig.ShaderEffectChangedArgs e)
    {
      if (!HasEngineInstance())
        return;
 //     Modified = true;
      EngineTerrain.SetShaderEffect(ShaderConfig);
    }

    /// <summary>
    /// Gets or sets the collection of decoration models
    /// </summary>
    [Browsable(false)]
    public ArrayList DecorationModels
    {
      get { return _decorationModels; }
      set 
      { 
        _decorationModels = value;
      }
    }

    /// <summary>
    /// Gets or sets the collection of detail textures
    /// </summary>
    [Browsable(false)]
    [EditorAttribute(typeof(TypedCollectionEditor), typeof(UITypeEditor)), EditableTypeAttribute(typeof(DetailTextureResource))]
    public ArrayList DetailTextures
    {
      get { return _detailTextures; }
      set
      {
        _detailTextures = value;
      }
    }

    /// <summary>
    /// internal flag that indicates the the terrain should be created from the current v3d
    /// </summary>
    [Browsable(false)]
    public bool PendingCreateFromV3D
    {
      get { return _bPendingCreateFromV3D; }
      set { _bPendingCreateFromV3D = value; }
    }

    [Description("Determines whether detail textures should be sorted to the same order as the detail texture definition. Should be left to true")]
    [SortedCategory("Compatibility", 10), PropertyOrder(1)]
    public bool SortDetailTextures
    {
      get
      {
        return this._bSortDetailTextures;
      }
      set
      {
        _bSortDetailTextures = value;
        if (HasEngineInstance())
          EngineTerrain.SetSortDetailTextures(_bSortDetailTextures);
      }
    }

    #endregion

    #region Decoration models

    public int GetNewDecorationID()
    {
      _iDecorationID++; // maybe synchronize with native terrain here to avoid conflicts
      return _iDecorationID - 1;
    }

    public DecorationModelResource CreateDecorationModelResource()
    {
      DecorationModelResource model = new DecorationModelResource(this, GetNewDecorationID());
      return model;
    }

    public void AddDecorationModelResource(DecorationModelResource model)
    {
      Debug.Assert(!DecorationModels.Contains(model));
      DecorationModels.Add(model);
      if (model._LODOrigin != null && DecorationModels.Contains(model._LODOrigin))
        model._LODOrigin.AddLODVersion(model);
      else
        model._LODOrigin = null;

      // also add LOD children
      if (model._LODVersions != null)
        foreach (DecorationModelResource child in model._LODVersions)
          if (!DecorationModels.Contains(child))
            DecorationModels.Add(child);

      Modified = true;
    }

    public void DeleteDecorationModelResource(DecorationModelResource model)
    {
      Debug.Assert(DecorationModels.Contains(model));
      DecorationModelResource oldparent = model._LODOrigin;
      DecorationModels.Remove(model);
      EngineTerrain.DeleteDecorationModel(model);
      model.SafeRemoveFromLODParent();
      if (oldparent != null)
        EngineTerrain.UpdateDecorationModelProperties(oldparent, false);

      // also remove LOD children
      if (model._LODVersions != null)
        foreach (DecorationModelResource child in model._LODVersions)
          if (DecorationModels.Contains(child))
          {
            EngineTerrain.DeleteDecorationModel(child);
            DecorationModels.Remove(child);
          }

      Modified = true;
    }

    public void UpdateDecorationModelProperties(DecorationModelResource model)
    {
      Debug.Assert(DecorationModels.Contains(model));
      Modified = true;
      if (HasEngineInstance())
        EngineTerrain.UpdateDecorationModelProperties(model, true);
      EditorManager.ActiveView.UpdateView(false);
    }


    #endregion

    #region Detail Textures

    public int GetNewDetailTextureID()
    {
      _iDecorationID++; // maybe synchronize with native terrain here to avoid conflicts
      return _iDecorationID - 1;
    }

    public DetailTextureResource CreateDetailTexture()
    {
      DetailTextureResource tex = new DetailTextureResource(this, GetNewDetailTextureID());
      return tex;
    }

    void EnumDetailTextures()
    {
      for (int i = 0; i < DetailTextures.Count; i++)
        ((DetailTextureResource)DetailTextures[i])._iListIndex = i;
    }

    public void AddDetailTexture(DetailTextureResource tex)
    {
      Debug.Assert(!DetailTextures.Contains(tex));
      tex._bIsBaseTexture = DetailTextures.Count == 0; // the first one is the detail texture
      if (tex._bIsBaseTexture)
      {
        tex._iSectorX1 = tex._iSectorY1 = 0;
        tex._iSectorX2 = Config._sectorCount.Width - 1;
        tex._iSectorY2 = Config._sectorCount.Height - 1;
      }

      if (tex._iListIndex < 0 || tex._iListIndex >= DetailTextures.Count)
        DetailTextures.Add(tex);
      else
        DetailTextures.Insert(tex._iListIndex, tex);
      EnumDetailTextures();
      tex._ownerTerrain = this;
      Modified = true;
      if (HasEngineInstance())
      {
        EngineTerrain.UpdateDetailTextureProperties(tex, true, null,false); // does not perform time consuming update
        EngineTerrain.UpdateDetailTextureOrder(this.DetailTextures, false, null);
      }

      // update the terrain effect
      UpdateAutoTerrainEffect();

      TerrainEditor.TriggerOnDetailTextureListChanged(new DetailTextureListChangedArgs(DetailTextureListChangedArgs.Action.AddedTexture, tex));
      EditorManager.ActiveView.UpdateView(false);
    }

    public void DeleteDetailTexture(DetailTextureResource tex)
    {
      Debug.Assert(DetailTextures.Contains(tex));
      Debug.Assert(tex._ownerTerrain==this);
      Debug.Assert(tex._bIsBaseTexture == false);
      DetailTextures.Remove(tex);
      EnumDetailTextures();
      Modified = true;
      if (!HasEngineInstance())
        return;

      bool bShowProgess = !EngineTerrain.AllSectorsLoaded(tex._iSectorX1, tex._iSectorY1,tex._iSectorX2, tex._iSectorY2);
      if (bShowProgess)
      {
        EditorManager.Progress.ShowProgressDialog("Updating detail texture properties");
        EngineTerrain.DeleteDetailTexture(tex, EditorManager.Progress);
        EditorManager.Progress.HideProgressDialog();
      }
      else
      {
        EngineTerrain.DeleteDetailTexture(tex, null); // fast enough without progress
      }

      // update the terrain effect
      UpdateAutoTerrainEffect();

      TerrainEditor.TriggerOnDetailTextureListChanged(new DetailTextureListChangedArgs(DetailTextureListChangedArgs.Action.RemovedTexture, tex));
      EditorManager.ActiveView.UpdateView(false);
      tex._ownerTerrain = null;
    }

    public void UpdateDetailTextureProperties(DetailTextureResource tex, bool bUpdateMaterialID)
    {
      Debug.Assert(DetailTextures.Contains(tex));
      Modified = true;
      if (!HasEngineInstance())
        return;

      bool bShowProgess = !EngineTerrain.AllSectorsLoaded(tex._iSectorX1, tex._iSectorY1,tex._iSectorX2, tex._iSectorY2);
      if (bShowProgess)
      {
        EditorManager.Progress.ShowProgressDialog("Updating detail texture properties");
        EngineTerrain.UpdateDetailTextureProperties(tex, true, EditorManager.Progress, bUpdateMaterialID);
        EditorManager.Progress.HideProgressDialog();
      }
      else
      {
        EngineTerrain.UpdateDetailTextureProperties(tex, true, null, bUpdateMaterialID); // fast enough without progress bar
      }

      // update the terrain effect
      UpdateAutoTerrainEffect();

      EditorManager.ActiveView.UpdateView(false);
    }

    #endregion

    #region Visibility

    TerrainVisibilityZoneShape[] _visibilityZones = null;

    public override void ResetVisibilityInfo(IVisibilityBuilder info)
    {
      base.ResetVisibilityInfo(info);

      // call it manually for the visibility zones
      if (this._visibilityZones != null)
        foreach (ShapeBase shape in _visibilityZones)
          shape.ResetVisibilityInfo(info);
    }

    public override void OnRecomputeVisibility(IVisibilityBuilder info)
    {
      base.OnRecomputeVisibility(info);

      // call it manually for the visibility zones
      if (this._visibilityZones != null)
        foreach (ShapeBase shape in _visibilityZones)
          shape.OnRecomputeVisibility(info);
    }

    public override void AssignGeometryToVisibiltyZone(VisibilityZoneShape zone)
    {
      base.AssignGeometryToVisibiltyZone(zone);
      // call it manually for the visibility zones
      if (this._visibilityZones != null)
        foreach (ShapeBase shape in _visibilityZones)
          shape.AssignGeometryToVisibiltyZone(zone);

      if (HasEngineInstance())
      {
        Debug.Assert(zone is TerrainVisibilityZoneShape);
        TerrainVisibilityZoneShape terrainZone = (TerrainVisibilityZoneShape)zone;

        this.EngineTerrain.AssignGeometryToVisibiltyZone(terrainZone.SectorX, terrainZone.SectorY, zone);
      }
    }

    public override void GetVisibilityInfo(IVisibilityBuilder info)
    {
      UpdateSectorBoundingBoxes(); // make sure box sizes are up-to-date because connecting relies on it

      base.GetVisibilityInfo(info);
      // call it manually for the visibility zones
      if (this._visibilityZones != null)
        foreach (ShapeBase shape in _visibilityZones)
          shape.GetVisibilityInfo(info);

    }



    void RemoveVisibilityZones()
    {
      if (_visibilityZones != null)
      {
        EditorManager.VisibilityBuilder.Dirty = true;
        foreach (TerrainVisibilityZoneShape zone in _visibilityZones)
        {
//          this.RemoveChild(zone);
          zone.OnRemoveFromScene();
        }
        _visibilityZones = null;
      }
      EditorManager.VisibilityBuilder.Dirty = true;
    }

    public TerrainVisibilityZoneShape GetZone(int x, int y)
    {
      if (_visibilityZones == null || x < 0 || y < 0 || x >= _terrainConfig._sectorCount.Width || y >= _terrainConfig._sectorCount.Height)
        return null;
      return _visibilityZones[y * _terrainConfig._sectorCount.Width + x];
    }

    void RecreateVisibilityZones()
    {
      RemoveVisibilityZones();

      if (EngineTerrain == null)
        return;

      int iCount = _terrainConfig.OverallSectorCount;
      int iCountX = _terrainConfig._sectorCount.Width;
      int iCountY = _terrainConfig._sectorCount.Height;
      int iZoneCount = iCountX * iCountY;

      _visibilityZones = new TerrainVisibilityZoneShape[iZoneCount];
//      bool bCallAddToScene = this.ParentLayer != null;
      int i = 0;
      int x, y;
      for (y = 0; y < iCountY; y++)
        for (x = 0; x < iCountX; x++, i++)
        {
          TerrainVisibilityZoneShape zone = new TerrainVisibilityZoneShape(this, x,y);
          _visibilityZones[i] = zone;

          // re-use ID
          if (_cachedSectorIDs != null && i < _cachedSectorIDs.Length)
            zone.LocalID = _cachedSectorIDs[i];

          zone.DebugColor = Color.DarkGreen;
          zone.ShapeVirtualCounter++;
          zone.VisibilityOwnerMeshShape = this;
          zone.ParentLayer = this.ParentLayer;
          zone.OnAddedToScene();

          if (_savedZoneBoxes!=null && _savedZoneBoxes.Length==iZoneCount)
          {
            zone.SetSectorBoundingBox(_savedZoneBoxes[i]);
          } 
          else
            zone.UpdateSectorBoundingBox();
        }

      // connect zones
      for (y = 0; y < iCountY; y++)
        for (x = 0; x < iCountX; x++, i++)
        {
          TerrainVisibilityZoneShape zone = GetZone(x, y);
          // GetZone handles zones out of range and AddNeighborZone only adds if !=null
          zone.AddNeighborZone(GetZone(x - 1, y));
          zone.AddNeighborZone(GetZone(x + 1, y));
          zone.AddNeighborZone(GetZone(x, y - 1));
          zone.AddNeighborZone(GetZone(x, y + 1));
        }
      EditorManager.VisibilityBuilder.Dirty = true;
    }

    void UpdateSectorBoundingBoxes()
    {
      if (_visibilityZones != null)
        foreach (TerrainVisibilityZoneShape zone in _visibilityZones)
          zone.UpdateSectorBoundingBox();
    }

    void ClampDetailTextureSectorsToValidRange()
    {
      if (_detailTextures == null)
        return;

      foreach (DetailTextureResource res in _detailTextures)
      {
        res._iSectorX1 = Math.Min(res._iSectorX1, Config._sectorCount.Width - 1);
        res._iSectorX2 = Math.Min(res._iSectorX2, Config._sectorCount.Width - 1);
        res._iSectorY1 = Math.Min(res._iSectorY1, Config._sectorCount.Height - 1);
        res._iSectorY2 = Math.Min(res._iSectorY2, Config._sectorCount.Height - 1);
      }
    }


    #endregion

    #region ISerializable

    ShaderEffectConfig _pendingConfig = null;
    BoundingBox[] _savedZoneBoxes = null;

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
		protected TerrainShape(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      _terrainConfig = (TerrainConfig)info.GetValue("_terrainConfig", typeof(TerrainConfig));
      _decorationModels = (ArrayList)info.GetValue("_decorationModels",typeof(ArrayList));
      _iDecorationID = info.GetInt32("_iDecorationID");
      if (SerializationHelper.HasElement(info, "_iVisibleBitmask"))
        _iVisibleBitmask = info.GetUInt32("_iVisibleBitmask");
      if (SerializationHelper.HasElement(info, "_bVisibleInsideVisZones"))
        _bVisibleInsideVisZones = info.GetBoolean("_bVisibleInsideVisZones");
      if (SerializationHelper.HasElement(info, "_fLODScaling"))
        _fLODScaling = info.GetSingle("_fLODScaling");
      if (SerializationHelper.HasElement(info, "_fMirrorLODBias"))
        _fMirrorLODBias = info.GetSingle("_fMirrorLODBias");      
      if (SerializationHelper.HasElement(info, "_lodMetric"))
        _lodMetric = (TerrainLODMetric_e)info.GetValue("_lodMetric", typeof(TerrainLODMetric_e));
      
      if (SerializationHelper.HasElement(info,"_iDetailTextureID"))
        _iDetailTextureID = info.GetInt32("_iDetailTextureID");
      if (SerializationHelper.HasElement(info, "_detailTextures"))
        _detailTextures = (ArrayList)info.GetValue("_detailTextures", typeof(ArrayList));
      if (SerializationHelper.HasElement(info, "_eLightmapSize"))
        _eLightmapSize = (LightmapSize_e)info.GetValue("_eLightmapSize", typeof(LightmapSize_e));
      if (SerializationHelper.HasElement(info, "_bCastStaticShadows"))
        _bCastStaticShadows = info.GetBoolean("_bCastStaticShadows");
      if (SerializationHelper.HasElement(info, "_iLightInfluenceBitmask"))
        _iLightInfluenceBitmask = info.GetUInt16("_iLightInfluenceBitmask");
      if (SerializationHelper.HasElement(info, "_shaderFX"))
        _pendingConfig = (ShaderEffectConfig)info.GetValue("_shaderFX", typeof(ShaderEffectConfig));
      if (SerializationHelper.HasElement(info, "_minimapTexture"))
        _minimapTexture = info.GetString("_minimapTexture");
      if (SerializationHelper.HasElement(info, "_heightmapExport"))
        _heightmapExport = info.GetString("_heightmapExport");
      if (SerializationHelper.HasElement(info, "_bNormalizeHeightmapExport"))
        _bNormalizeHeightmapExport = info.GetBoolean("_bNormalizeHeightmapExport");
      if (SerializationHelper.HasElement(info, "_bExportAsEditable"))
        _bExportAsEditable = info.GetBoolean("_bExportAsEditable");
      if (SerializationHelper.HasElement(info, "_bFastUpdate"))
        _bFastUpdate = info.GetBoolean("_bFastUpdate");
      
      if (SerializationHelper.HasElement(info, "_cachedDectorIDs"))
        _cachedSectorIDs = (uint[])info.GetValue("_cachedDectorIDs", typeof(uint[]));
      else
        _cachedSectorIDs = null;
      if (SerializationHelper.HasElement(info, "_savedZoneBoxes"))
        _savedZoneBoxes = (BoundingBox[])info.GetValue("_savedZoneBoxes", typeof(BoundingBox[]));
      else
        _savedZoneBoxes = null;

      if (SerializationHelper.HasElement(info, "_bSortDetailTextures"))
        _bSortDetailTextures = info.GetBoolean("_bSortDetailTextures");

      if (SerializationHelper.HasElement(info, "_meshExportSettings"))
        _meshExportSettings = (TerrainMeshExport)info.GetValue("_meshExportSettings", typeof(TerrainMeshExport));

      _bBakeMode = TerrainEditor.TextureBakedView; // fall back to editor setting
      if (SerializationHelper.HasElement(info, "_bBakeMode"))
        _bBakeMode = info.GetBoolean("_bBakeMode");
      if (SerializationHelper.HasElement(info, "_bIsReference"))
        _bIsReference = info.GetBoolean("_bIsReference");
      if (SerializationHelper.HasElement(info, "_bSceneSpecificLightmaps"))
        _bSceneSpecificLightmaps = info.GetBoolean("_bSceneSpecificLightmaps");

      // Make sure detail texture resources contain valid sector index information
      // (just in case an older vForge version wrote out wrong values after the terrain was resized)
      this.ClampDetailTextureSectorsToValidRange();
    }

    uint[] _cachedSectorIDs = null;


    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context) 
    {
      base.GetObjectData(info,context);
			info.AddValue("_terrainConfig", _terrainConfig);
      info.AddValue("_decorationModels", _decorationModels);
      info.AddValue("_iDecorationID", _iDecorationID);
      info.AddValue("_iVisibleBitmask", _iVisibleBitmask);
      info.AddValue("_bVisibleInsideVisZones", _bVisibleInsideVisZones);
      info.AddValue("_fLODScaling", _fLODScaling);
      info.AddValue("_fMirrorLODBias", _fMirrorLODBias);
      info.AddValue("_lodMetric", _lodMetric);
      info.AddValue("_iDetailTextureID", _iDetailTextureID);
      info.AddValue("_detailTextures", _detailTextures);
      info.AddValue("_eLightmapSize", _eLightmapSize);
      info.AddValue("_bCastStaticShadows", _bCastStaticShadows);
      info.AddValue("_iLightInfluenceBitmask", _iLightInfluenceBitmask);
      info.AddValue("_shaderFX", _shaderFX);
      info.AddValue("_minimapTexture", _minimapTexture);
      info.AddValue("_heightmapExport", _heightmapExport);
      info.AddValue("_bNormalizeHeightmapExport", _bNormalizeHeightmapExport);
      info.AddValue("_bExportAsEditable", _bExportAsEditable);
      info.AddValue("_bFastUpdate", _bFastUpdate);
      info.AddValue("_bSortDetailTextures", _bSortDetailTextures);
      
      // save the zone's IDs so we don't have to re-create when loading
      _cachedSectorIDs = null;
      _savedZoneBoxes = null;
      if (_visibilityZones!=null)
      {
        int iIndex = 0;
        _cachedSectorIDs = new uint[_visibilityZones.Length];
        _savedZoneBoxes = new BoundingBox[_visibilityZones.Length];
        foreach (ShapeBase zone in _visibilityZones)
        {
          _cachedSectorIDs[iIndex] = zone.LocalID;
          _savedZoneBoxes[iIndex] = zone.AbsoluteBoundingBox;
          iIndex++;
        }

        info.AddValue("_cachedDectorIDs", _cachedSectorIDs);
        info.AddValue("_savedZoneBoxes", _savedZoneBoxes);
      }

      if (_meshExportSettings != null)
        info.AddValue("_meshExportSettings", _meshExportSettings);

      _bBakeMode = TerrainEditor.TextureBakedView; // get latest status from editor
      info.AddValue("_bBakeMode", _bBakeMode);
      info.AddValue("_bIsReference", _bIsReference);
      info.AddValue("_bSceneSpecificLightmaps", _bSceneSpecificLightmaps);
    }

    public override void OnDeserialization()
    {
      base.OnDeserialization();
      AddHint(HintFlags_e.NoRotation | HintFlags_e.NoScale);

      if (string.IsNullOrEmpty(_terrainConfig.TerrainFolder))
        _terrainConfig.TerrainFolder = this.ShapeName;

      
      foreach (DecorationModelResource model in _decorationModels)
      {
        if (model.ID == 0)
          model.ID = GetNewDecorationID(); //OLD scene file fix
        model._ownerTerrain = this;
      }

      bool bFirst = true;
      foreach (DetailTextureResource tex in _detailTextures)
      {
        tex._ownerTerrain = this;
        tex._bIsBaseTexture = bFirst;
        bFirst = false;
      }
      EnumDetailTextures();

      if (_pendingConfig == null)
      {
        _pendingConfig = new ShaderEffectConfig(this);
        _pendingConfig._libFilename = @"\Shaders\Terrain.ShaderLib";
        _pendingConfig._effectName = "Terrain";
        _pendingConfig._shadermode = ISurface.SHADERMODE.AUTO;
      }
      ShaderConfig = _pendingConfig; // register callbacks etc.
      _terrainShaderProperties = new TerrainShaderProperties(this);

      // force update the minimap:
      string _tex = _minimapTexture;
      _minimapTexture = null;
      this.MinimapTexture = _tex;


      if (HasChildren())
      {
        //foreach (ShapeBase shape in ChildCollection)
      }

      //if (_materialProvider==null)
      //_materialProvider = new TerrainMaterialProvider(this);

      if (_meshExportSettings != null)
        _meshExportSettings.Owner = this;

      if (_bIsReference)
        _bBakeMode = true;
      TerrainEditor.TextureBakedView = _bBakeMode;

      // dir light shape saved with the file?
      foreach (ShapeBase child in ChildCollection)
        if (child is TerrainDirectionalLight)
        {
          _dirLightShape = (TerrainDirectionalLight)child;
          _dirLightShape.OwnerTerrain = this;
          break;
        }
    }


    #endregion

    #region Clone

    public override ShapeBase Clone()
    {
      TerrainShape copy = (TerrainShape) base.Clone();

      // deep copy collections
      copy.DecorationModels = new ArrayList(DecorationModels.Count);
      foreach (DecorationModelResource model in DecorationModels)
        copy.DecorationModels.Add(model.Clone());

      copy.DetailTextures = new ArrayList(DetailTextures.Count);
      foreach (DetailTextureResource tex in DetailTextures)
        copy.DetailTextures.Add(tex.Clone());

      copy._visibilityZones = null;

      return copy;
    }


    #endregion

    #region GUI

    /// <summary>
    /// Overridden function to get the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetIconIndex()
    {
      return GetTerrainIconIndex();
    }

    public static int GetTerrainIconIndex()
    {
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_terrain.bmp"), Color.Magenta);
    }

    #endregion

    #region 2D map rendering

    public override float GetScene2DRenderingSortingKey(Scene2DView view)
    {
      return 100000000.0f; // under ther sectors
    }

    ImageAttributes _2dViewAttr = new ImageAttributes();

    static float[][] matrixItems ={ 
     new float[] {1, 0, 0, 0, 0},
     new float[] {0, 1, 0, 0, 0},
     new float[] {0, 0, 1, 0, 0},
     new float[] {0, 0, 0, 0.5f, 0}, 
     new float[] {0, 0, 0, 0, 1}};

    static ColorMatrix colorMatrix = new ColorMatrix(matrixItems);

    public override void OnPaintScene2DView(Scene2DView view, PaintEventArgs e)
    {
      base.OnPaintScene2DView(view, e);

      if (_minimapImage != null)
      {
        BoundingBox abs = AbsoluteBoundingBox;
        Point p1 = view.Transform(abs.vMin);
        Point p2 = view.Transform(abs.vMax);
        Rectangle dst = new Rectangle(p1.X, p1.Y, p2.X - p1.X, p2.Y - p1.Y);

        _2dViewAttr.SetColorMatrix(colorMatrix, ColorMatrixFlag.Default, ColorAdjustType.Bitmap);
//        e.Graphics.DrawImage(_minimapImage, dst);
        e.Graphics.DrawImage(_minimapImage, dst, 0, 0, _minimapImage.Width, _minimapImage.Height, GraphicsUnit.Pixel, _2dViewAttr);
      }
    }

    static Scene2DViewFilterCategory FilterCat = null;
    public override Scene2DViewFilterCategory GetScene2DRenderingFilter(Scene2DView view)
    {
      if (FilterCat == null)
      {
        FilterCat = view.CreateFilterCategory("Terrain"); // might use "Terrain mini map"
        int iIndex = TerrainShape.GetTerrainIconIndex();
        if (iIndex >= 0)
          FilterCat.Image = EditorManager.GUI.ShapeTreeImages.ImageList.Images[iIndex];
      }
      return FilterCat;
    }

    static string VA_EDIT_SECTOR_PROPERTIES = @"Terrain\Edit sectors in property grid";
    static string VA_IMPORT_HEIGHTMAP = @"Terrain\Import heightmap";
    static string VA_REPAIR_SECTORS = @"Terrain\Repair sector(s)";
    static string VA_RELOAD_SECTORS = @"Terrain\Reload sector(s)";
   
    public override void Get2DViewActions(Scene2DView view, Scene2DActions actions)
    {
      base.Get2DViewActions(view, actions);

      if (!view.TouchesSelectionMarquee(this.AbsoluteBoundingBox))
        return;

      if (this.Modifiable)
      {
        actions.AddAction(this, VA_EDIT_SECTOR_PROPERTIES);
        actions.AddAction(this, VA_IMPORT_HEIGHTMAP);
        actions.AddAction(this, VA_REPAIR_SECTORS);
      }
      actions.AddAction(this, VA_RELOAD_SECTORS);
    }

    public override void Perform2DViewAction(Scene2DView view, GroupAction parent, string action)
    {
      base.Perform2DViewAction(view, parent, action);

      // sector range - used by all actions
      BoundingBox selBox = view.SelectionMarqueeWorldBox;
      int x1, y1, x2, y2;
      Config.GetSectorIndicesAtWorldPos(selBox.vMin, out x1, out y1);
      Config.GetSectorIndicesAtWorldPos(selBox.vMax, out x2, out y2);
      Config.ClampSectorRange(ref x1, ref y1, ref x2, ref y2);

      if (action == VA_EDIT_SECTOR_PROPERTIES)
      {
         ShapeCollection shapes = new ShapeCollection();
         for (int sy = y1; sy <= y2; sy++)
           for (int sx = x1; sx <= x2; sx++)
             shapes.Add(GetZone(sx, sy));
         EditorManager.SelectedShapes = shapes;
      }
      else if (action == VA_IMPORT_HEIGHTMAP)
      {
        ImportHeightmapDDS import = new ImportHeightmapDDS();
        x1 *= Config.SamplesPerSectorX;
        y1 *= Config.SamplesPerSectorY;
        x2 = (x2 + 1) * Config.SamplesPerSectorX;
        y2 = (y2 + 1) * Config.SamplesPerSectorY;
        ApplyHeightmapFilterDlg.RunFilter(import, x1, y1, x2, y2);
      }
      else if (action == VA_REPAIR_SECTORS)
      {
        EditorManager.Progress.ShowProgressDialog("Repair Sectors");
        EngineTerrain.EnsureSectorRangeLoaded(x1, y1, x2, y2, (int)SectorEditorFlags_e.AnythingDirty, true, true, EditorManager.Progress);
        EditorManager.Progress.HideProgressDialog();
      }
      else if (action == VA_RELOAD_SECTORS)
      {
        EditorManager.Progress.ShowProgressDialog("Reload Sectors");
        EngineTerrain.ReloadSectorRange(x1, y1, x2, y2, false, EditorManager.Progress);
        EditorManager.Progress.HideProgressDialog();
        EditorManager.ActiveView.UpdateView(false);
      }
    }

    #endregion


    #region Plugin information

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <returns></returns>
    public override EditorPluginInfo GetPluginInformation()
    {
      if (DecorationModels.Count==0)
        return EditorPlugin.EDITOR_PLUGIN_INFO;

      // need to collect plugin names from decoration types:
      EditorPluginInfo info = new EditorPluginInfo();
      ArrayList plugins = new ArrayList(2);
      plugins.AddRange(EditorPlugin.EDITOR_PLUGIN_INFO.NativePluginNames);
      foreach (DecorationModelResource model in DecorationModels)
        model.AddPluginNames(plugins);

      info.NativePluginNames = (string[])plugins.ToArray(typeof(string));
      return info;
    }

    #endregion

    #region Terrain Functions

    HeightmapValueProvider _heightmapProvider = null;

    void OnConfigChanged()
    {
      _heightmapProvider = null; // rebuild
    }

    /// <summary>
    /// Traceline on the terrain. Returns the touchpoint
    /// </summary>
    /// <param name="start"></param>
    /// <param name="end"></param>
    /// <param name="touchPoint"></param>
    /// <returns></returns>
    public bool TraceTerrain(Vector3F start, Vector3F end, ref Vector3F touchPoint)
    {
      if (!HasEngineInstance())
        return false;
      return EngineTerrain.TraceTerrain(start,end,ref touchPoint);
    }


    /// <summary>
    /// Returns the terrain height value at specified position
    /// </summary>
    /// <param name="pos"></param>
    /// <returns></returns>
    public float TerrainHeightAtPosition(Vector3F pos)
    {
      if (!HasEngineInstance())
        return 0.0f;
      return EngineTerrain.TerrainHeightAtPosition(pos);
    }

    /// <summary>
    /// Access the heightmap values
    /// </summary>
    [Browsable(false)]
    public IValueProvider Heightmap
    {
      get
      {
        if (_heightmapProvider == null)
          _heightmapProvider = new HeightmapValueProvider(this);
        return _heightmapProvider;
      }
    }
  
    bool MoveTerrainFolder(string newFolder , bool bShowMessagebox )
    {
      if (!HasEngineInstance())
        return false;

      if (bShowMessagebox == true)
      {
        DialogResult res = EditorManager.ShowMessageBox("This operation is not undo-able and it will additionally clear the current Undo/Redo history.\n\nContinue anyway?", "Move Terrain folder", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
        if (res != DialogResult.Yes)
          return false;
      }

      EditorManager.Actions.Reset();
      EditorManager.Progress.ShowProgressDialog("Moving terrain files to new folder");

      EngineTerrain.SaveAndUnload(EditorManager.Progress, 20.0f);
      DirectoryInfo srcDir = new DirectoryInfo(EditorManager.Project.MakeAbsolute(Config._terrainFolder));
      DirectoryInfo dstDir = new DirectoryInfo(EditorManager.Project.MakeAbsolute(newFolder));
      if (!FileHelper.CopyFiles(srcDir, "*.*", dstDir, true, false, FileAttributes.Hidden, false))
      {
        EditorManager.Progress.HideProgressDialog();
        EditorManager.ShowMessageBox("Copying of the terrain folders was not successful. Aborted", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        return false;
      }

      foreach (DetailTextureResource texture in DetailTextures)
      {
        texture.UpdatePathIfRequired(Config._terrainFolder, newFolder);
      }

      _bIsReference = false;
      Config._terrainFolder = newFolder;
      EngineTerrain.OnTerrainFolderChanged(newFolder, EditorManager.Progress, 80.0f);
      EditorManager.Progress.HideProgressDialog();
      TerrainEditor.EditorPanel.UpdatePanelStatus();

      Modified = true;
      return true;
    }

    void AddShapesRecursive(ShapeCollection target, ShapeBase parent, Rectangle2D selection )
    {
      if (parent.ShapeVirtualCounter==0 && parent.CanCopyPaste && (parent is Shape3D))
      {
        Shape3D shape3D = (Shape3D)parent;
        if (selection.IsInside(shape3D.x, shape3D.y))
        {
          target.Add(parent);
          //return; // iterate through children as well - the CloneForClipboard will take care of handling duplicates
        }
      }

      if (parent.HasChildren())
      {
        ShapeCollection children = parent.ChildCollection;
        foreach (ShapeBase child in children)
          AddShapesRecursive(target, child, selection);
      }
    }


    /// <summary>
    /// Creates a clipboard object that holds the clone of relevant terrain data
    /// </summary>
    /// <param name="selection">Selection to clone. Can be null to use TerrainEditor.CurrentSelection</param>
    /// <returns>The clipboard data object</returns>
    public ITerrainClipboardObject CopySelection(TerrainSelection selection)
    {
      if (!HasEngineInstance())
        return null;
      if (selection == null)
        selection = TerrainEditor.CurrentSelection;
      if (selection == null || !selection.Valid)
        return null;
      ITerrainClipboardObject data = EngineTerrain.CopySelection(selection);

      // add shapes to clone
      if ((selection.SelectionFilter & TerrainSelection.SelectionFilter_e.Shapes) != 0)
      {
        ShapeCollection shapesOnTerrain = new ShapeCollection();
        ShapeCollection roots = EditorManager.Scene.RootShapes;
        Rectangle2D rect = selection.WorldSpaceExtent;
        foreach (ShapeBase root in roots)
          AddShapesRecursive(shapesOnTerrain, root, rect);
        if (shapesOnTerrain.Count > 0)
        {
          data.ShapesToPaste = shapesOnTerrain.CloneForClipboard();

          // remap position xy to [0..1] range inside the selection
          float fInvX = 1.0f / rect.GetSizeX();
          float fInvY = 1.0f / rect.GetSizeY();
          foreach (Shape3D shape in data.ShapesToPaste)
            shape.Position = new Vector3F((shape.x - rect.X1) * fInvX, (shape.y - rect.Y1) * fInvY, shape.z);
        }
      }

      return data;
    }


    #endregion

    #region ITerrainCallback Members

    public void OnSectorChanged(SectorAction action, int iSectorX, int iSectorY)
    {
      TerrainVisibilityZoneShape zone = GetZone(iSectorX, iSectorY);
      if (zone == null)
        return;
      switch (action)
      {
        case SectorAction.Reloaded:
          zone.CurrentStatus = action;
          zone.UpdateSectorBoundingBox();
          break;
        case SectorAction.Unloaded:
          zone.CurrentStatus = action;
          break;
        case SectorAction.Modified:
          zone.ViewBitmapDirty = true;
          break;
      }
    }

    public DecorationModelResource GetDecorationModelByID(int iID)
    {
      foreach (DecorationModelResource m in _decorationModels)
        if (m.ID == iID)
          return m;
      return null;
    }

    public DecorationModelResource GetDecorationModelByName(string name)
    {
      foreach (DecorationModelResource m in _decorationModels)
        if (string.Compare(m.Name,name,true)==0)
          return m;
      return null;
    }

    public DetailTextureResource GetDetailTextureByID(int iID)
    {
      foreach (DetailTextureResource m in _detailTextures)
        if (m.ID == iID)
          return m;
      return null;
    }

    public DetailTextureResource GetDetailTextureByName(string name)
    {
      foreach (DetailTextureResource m in _detailTextures)
        if (string.Compare(m.Name, name, true) == 0)
          return m;
      return null;
    }


    public void OnDirtyDecorationRegionChanged(int iDirtySectors)
    {
      TerrainEditor.TriggerOnDecorationDirtyRegionChanged();
    }

    #endregion

    #region ITerrainObject Members

    [Browsable(false)]
    public TerrainShape OwnerTerrain
    {
      get { return this; }
    }

    #endregion

    #region Directional Light

    TerrainDirectionalLight _dirLightShape;
    bool _bDirLightUpdateInProgress = false;

    public void SetShowDirectionalLightShape(bool bShow)
    {
      if (_bDirLightUpdateInProgress)
        return;

      _bDirLightUpdateInProgress = true;
      if (_dirLightShape == null)
      {
        _dirLightShape = new TerrainDirectionalLight("Terrain Directional Light");
        _dirLightShape.OwnerTerrain = this;
        _dirLightShape.SetParent(this, false); // it is a non-serialized child shape
        _dirLightShape.OnAddedToScene();
      }

      if (_dirLightShape.Selected == bShow)
      {
        _bDirLightUpdateInProgress = false;
        return;
      }

      GizmoBase gizmo = EditorManager.ActiveView.Gizmo;
      if (!bShow)
      {
        gizmo.Clear();
        _bDirLightUpdateInProgress = false;
        return;
      }

      // set light shape in front of the camera
      _dirLightShape.Position = EditorManager.ActiveView.CameraPosition + EditorManager.ActiveView.CameraDirection * EditorManager.Settings.GlobalUnitScaling * 500.0f;
      gizmo.SetSingleShape(_dirLightShape, false);
      gizmo.DragMode = ShapeDragMode.ROTATE; // for directional light
      EditorManager.ActiveView.SetDefaultContext();
      _bDirLightUpdateInProgress = false;
    }

    #endregion

    #region Selection

    TerrainSelectionShape _selectionShape = null;

    public void SetShowSelection(bool bShow, bool bMoveToCamera)
    {
      if (_selectionShape == null)
      {
        if (!bShow)
          return;
        bMoveToCamera = true; // first time visible
        _selectionShape = new TerrainSelectionShape("Terrain Selection");
        _selectionShape.SetParent(this, false); // it is a non-serialized child shape
        _selectionShape.OnAddedToScene();
      }

      if (bMoveToCamera)
      {
        Vector3F startRay, endRay;
        Vector3F hitPoint = new Vector3F();
        int x = EditorManager.ActiveView.Size.Width / 2;
        int y = EditorManager.ActiveView.Size.Height / 2;
        EditorManager.EngineManager.GetRayAtScreenPos(out startRay, out endRay, x, y, EditorManager.Settings.MaxPickingDistance);
        if (EditorManager.EngineManager.GetTraceHit(startRay, endRay, ref hitPoint))
        {
          _selectionShape.Position = hitPoint;
        }
      }

      EditorManager.ActiveView.UpdateView(false);
      GizmoBase gizmo = EditorManager.ActiveView.Gizmo;
      if (_selectionShape.Selected == bShow)
      {
        gizmo.RecalcGizmoPos();
        return;
      }

      if (!bShow)
      {
        gizmo.Clear();
        return;
      }

      // set light shape in front of the camera
      gizmo.SetSingleShape(_selectionShape, false);
      EditorManager.ActiveView.SetDefaultContext();

    }

    [Browsable(false)]
    public TerrainSelection CurrentSelection
    {
      get
      {
        TerrainSelection sel = _selectionShape.CurrentSelection;
        if (sel != null && sel.Valid)
          return sel;
        return null;
      }
    }

    #endregion

    #region Relevant OPerations

    static string RO_MOVEFOLDER = "Move Terrain Folder";
    static string RO_MOVEFOLDER_SCENE = "xyz";

    string GetSuggestedFolderName()
    {
      return Path.GetFileNameWithoutExtension(EditorManager.Scene.FileName) + ".TerrainData";
    }

    public override ArrayList RelevantOperations
    {
      get
      {
        ArrayList op = base.RelevantOperations;
        if (op == null)
          op = new ArrayList(2);

        string sceneName = GetSuggestedFolderName();
        if (sceneName != Config.TerrainFolder)
        {
          RO_MOVEFOLDER_SCENE = string.Format("Move Terrain Folder to '{0}'", sceneName);
          op.Add(RO_MOVEFOLDER_SCENE);
        }

        op.Add(RO_MOVEFOLDER);
        return op;
      }
    }

    public override void PerformRelevantOperation(string name, int iShapeIndex, int iShapeCount)
    {
      if (name == RO_MOVEFOLDER)
      {
        CreateFileDlg FileDialog = new CreateFileDlg();
        FileDialog.Caption = "Relocate Terrain Folder";
        FileDialog.Description = "Choose a directory into which the terrain data should be relocated.";
        FileDialog.Filter = new string[] { "" };
        FileDialog.InitialDirectory = EditorManager.Project.ProjectDir;
        FileDialog.Text = "vForge";
        FileDialog.Ext = "";
        FileDialog.SelectFolder = true;

        if (FileDialog.ShowDialog() != DialogResult.OK)
          return;

        String sResult = EditorManager.Project.MakeRelative(FileDialog.FileName);

        if (string.IsNullOrEmpty(sResult) || (sResult == Config.TerrainFolder))
          return;

        MoveTerrainFolder(sResult, true);
        return;
      }
      if (name == RO_MOVEFOLDER_SCENE)
      {
        MoveTerrainFolder(GetSuggestedFolderName() , true );
      }

      base.PerformRelevantOperation(name, iShapeIndex, iShapeCount);
    }


    #endregion

    #region Export

    /// <summary>
    /// Calls export function on engine instance
    /// </summary>
    /// <returns>true if export was successful</returns>
    public override bool OnExport(SceneExportInfo info)
    {
      if (MeshExportSettings.ExportForAssetProfile.IsActiveProfileSet && MeshExportSettings.AutomaticExport)
        return true;

      return base.OnExport(info);
    }

    /// <summary>
    /// Recursively exports shape and its children
    /// </summary>
    /// <returns>true if export was successful</returns>
    public override bool OnExportRecursive(SceneExportInfo info)
    {
      if (MeshExportSettings.ExportForAssetProfile.IsActiveProfileSet && MeshExportSettings.AutomaticExport)
        return true;

      return base.OnExportRecursive(info);
    }

    #endregion

    #region Mesh Export

    TerrainMeshExport _meshExportSettings = null;

    [Browsable(false)]
    public bool HasMeshExportSettings
    {
      get
      {
        return _meshExportSettings != null;
      }
    }

    [Browsable(false)]
    public TerrainMeshExport MeshExportSettings
    {
      get 
      {
        if (_meshExportSettings == null)
        {
          _meshExportSettings = new TerrainMeshExport(this);
        }
        return _meshExportSettings; 
      }
      set 
      {
        _meshExportSettings = value;
        Modified = true;
      }
    }

    #endregion
  }

  #endregion

  #region TerrainShape Creator Plugin

  [PrefabTest.IgnoreForPrefabTests]
  class TerrainShapeCreator : CSharpFramework.IShapeCreatorPlugin
  {
      public TerrainShapeCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_terrain.bmp"), Color.Magenta);
    }

    public override string GetPluginName()
    {
      return "Terrain";
    }

    public override string GetPluginCategory()
    {
      return "Geometry";
    }

    /// <summary>
    /// Returns a short description text
    /// </summary>
    /// <returns></returns>
    public override string GetPluginDescription()
    {
      return "This shape represents a full terrain instance in the world. There can only be one terrain per scene.";
    }


    public override ShapeBase CreateShapeInstance()
    {
      if (EditorManager.Scene.FindFirstShapeOfType(typeof(TerrainShape), true) != null)
      {
        EditorManager.ShowMessageBox("There is already a terrain shape in this scene.\nThe current version of the SDK only supports one terrain per scene.", "Cannot create more than one terrain", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
        return null;
      }

			string name = EditorManager.Scene.CreateNewShapeName("Terrain");
      TerrainShape shape = new TerrainShape(name);

      if (!TestManager.IsRunning)
      {
        TerrainCreationDlg dlg = new TerrainCreationDlg();
        dlg.TerrainShape = shape;

        if (dlg.ShowDialog() != DialogResult.OK)
          return null;
        shape = dlg.TerrainShape; // should be the same though
      }
      else
      {
        shape.TerrainConfig._sectorCount.Width = 4;
        shape.TerrainConfig._sectorCount.Height = 4;
        shape.TerrainConfig.TerrainFolder = "TestTerrain";
      }
      return shape;
    }
  }

  #endregion
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
