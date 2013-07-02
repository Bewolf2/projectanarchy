/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections;
using System.IO;
using CSharpFramework;
using TerrainBase.Editing;
using TerrainEditorPlugin.Actions;
using TerrainEditorPlugin.Contexts;
using TerrainEditorPlugin.Shapes;
using TerrainManaged;

namespace TerrainEditorPlugin
{


	/// <summary>
	/// Class with static terrain editor functions. There is no instance of this editor
	/// </summary>
	public class TerrainEditor
	{
		/// <summary>
		/// Prevent creation
		/// </summary>
		private TerrainEditor()
		{
		}

		#region Init/Deinit

		public static void Init()
		{
			EditorManager.SceneChanged += new SceneChangedEventHandler(EditorManager_SceneChanged);
      EditorManager.SceneEvent += new SceneEventHandler(EditorManager_SceneEvent);
      VisionViewBase.MouseContextChanged += new CSharpFramework.Contexts.ContextChangedEventHandler(VisionViewBase_MouseContextChanged);
      Cursor3DProperties.OnCursor3DPropertiesChanged += new EventHandler(Cursor3DProperties_OnCursor3DPropertiesChanged);
      DecorationModelResource.OnDecorationModelResourceNameChanged += new EventHandler(DecorationModelResource_OnDecorationModelResourceNameChanged);
      EditorManager.EngineManager.OnAfterRenderScene += new IEngineManager.RenderSceneEventHandler(EngineManager_OnAfterRenderScene);
      EditorManager.EditorModeChanged += new EditorModeChangedEventHandler(EditorManager_EditorModeChanged);
		}

		public static void DeInit()
		{
      EditorManager.EditorModeChanged -= new EditorModeChangedEventHandler(EditorManager_EditorModeChanged);
      EditorManager.EngineManager.OnAfterRenderScene -= new IEngineManager.RenderSceneEventHandler(EngineManager_OnAfterRenderScene);
      DecorationModelResource.OnDecorationModelResourceNameChanged -= new EventHandler(DecorationModelResource_OnDecorationModelResourceNameChanged);
      Cursor3DProperties.OnCursor3DPropertiesChanged -= new EventHandler(Cursor3DProperties_OnCursor3DPropertiesChanged);
      VisionViewBase.MouseContextChanged -= new CSharpFramework.Contexts.ContextChangedEventHandler(VisionViewBase_MouseContextChanged);
      EditorManager.SceneEvent -= new SceneEventHandler(EditorManager_SceneEvent);
      EditorManager.SceneChanged -= new SceneChangedEventHandler(EditorManager_SceneChanged);
    }

		#endregion

		#region Listeners

    /// <summary>
    /// React to scene changes
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
		static void EditorManager_SceneChanged(object sender, SceneChangedArgs e)
		{
			if (e.newscene != null)
        LoadAllBrushes();
		}

    static void EditorManager_SceneEvent(object sender, SceneEventArgs e)
    {
      if (e.action==SceneEventArgs.Action.BeforeClosing)
        FreeAllBrushes();
    }

    static void EditorManager_EditorModeChanged(object sender, EditorModeChangedArgs e)
    {
      // Disable editing in Play-the-game mode
      if (EditorManager.ActiveView.CurrentContext is TerrainEditingBaseContext && e._newMode == EditorManager.Mode.EM_PLAYING_IN_GAME)
        EditorManager.ActiveView.SetDefaultContext();
    }

    static void VisionViewBase_MouseContextChanged(object sender, CSharpFramework.Contexts.ContextChangedArgs e)
    {
      if (CurrentTerrain == null || CurrentTerrain.EngineTerrain == null || Cursor3DProperties == null)
        return;
      TerrainEditor.Cursor3DProperties.Enabled = e.newContext is TerrainEditingBaseContext;
    }

    static void Cursor3DProperties_OnCursor3DPropertiesChanged(object sender, EventArgs e)
    {
      if (CurrentTerrain == null || CurrentTerrain.EngineTerrain == null || Cursor3DProperties==null)
        return;
      CurrentTerrain.EngineTerrain.Update3DCursor(Cursor3DProperties, -1.0f, -1.0f);
      EditorManager.ActiveView.UpdateView(false);
    }

    static void DecorationModelResource_OnDecorationModelResourceNameChanged(object sender, EventArgs e)
    {
      TerrainEditor.TriggerOnDecorationModelListChanged();
    }

    static bool IsTerrainContext
    {
      get
      {
        return EditorManager.ActiveView.CurrentContext is TerrainEditingBaseContext;
      }
    }

    static void EngineManager_OnAfterRenderScene(object sender, EventArgs e)
    {
      // repaint if necessary
      if (CurrentTerrain != null && CurrentTerrain.EngineTerrain != null && CurrentTerrain.EngineTerrain.WantsViewUpdate())
        EditorManager.ActiveView.UpdateView(false);
    }

		#endregion

		#region Current terrain

		static TerrainShape _currentTerrain;
    static public event EventHandler OnTerrainChanged = null;

		/// <summary>
		/// Gets or sets the current terrain for editing
		/// </summary>
		public static TerrainShape CurrentTerrain
		{
			get { return _currentTerrain; }
			set 
      {
        if (_currentTerrain == value)
          return;
        _currentTerrain = value;
        CurrentDecorationModel = null;
        CurrentDetailTexture = null;

        // set properties on new terrain
        if (_currentTerrain != null && _currentTerrain.EngineTerrain!=null)
        {
          if (_currentTerrain.IsReference)
            TextureBakedView = true; // referenced terrains use this mode
          _currentTerrain.EngineTerrain.SetPreviewDirectionalLight(_bDirectionalLighting);
          _currentTerrain.EngineTerrain.SetUseBakedTextures(_bTextureBakedView, _currentTerrain.ShaderConfig);
          _currentTerrain.EngineTerrain.SetDisplayMiniMap(_bDisplayMinimap);
        }

        if (OnTerrainChanged != null)
          OnTerrainChanged(null, EventArgs.Empty);
      }
		}


		#endregion

		#region 3D Cursor

		static Cursor3DProperties _cursor3DProperties = null;

		/// <summary>
		/// Gets or sets the cursor properties
		/// </summary>
		static public Cursor3DProperties Cursor3DProperties
		{
			get { return _cursor3DProperties; }
			set 
      {
        if (_cursor3DProperties == value)
          return;
        if (_cursor3DProperties!=null)
          _cursor3DProperties.Active = false;
        _cursor3DProperties = value;
        if (_cursor3DProperties != null)
        {
          _cursor3DProperties.Active = true;
          _cursor3DProperties.Enabled = IsTerrainContext;
        }
        Cursor3DProperties.TriggerOnCursor3DPropertiesChanged();
      }
		}

		#endregion

		#region Brushes

		static TerrainBrush _currentBrush = null;
    static ArrayList _allBrushes = new ArrayList();

    static public event EventHandler OnBrushCollectionChanged = null;
    static public event EventHandler OnCurrentBrushChanged = null;

    /// <summary>
    /// Gets or sets the current brush texture for editing
    /// </summary>
		public static TerrainBrush CurrentBrush
		{
			get { return TerrainEditor._currentBrush; }
			set 
			{
				if (_currentBrush == value)
					return;
				_currentBrush = value;
        if (OnCurrentBrushChanged!=null)
          OnCurrentBrushChanged(_currentBrush,EventArgs.Empty);
			}
		}

    /// <summary>
    /// gets or sets the collection of all brushes
    /// </summary>
    public static ArrayList AllBrushes
    {
      get { return TerrainEditor._allBrushes; }
      set 
      {
        if (TerrainEditor._allBrushes == value)
          return;
        TerrainEditor._allBrushes = value;
        if (OnBrushCollectionChanged != null)
          OnBrushCollectionChanged(null, EventArgs.Empty);
      }
    }

    /// <summary>
    /// Releases all brush resources (e.g. project close
    /// </summary>
    static void FreeAllBrushes()
    {
      // unload all brushes
      foreach (TerrainBrush b in _allBrushes)
        TerrainConversionHelpers.FreeBrush(b);
      _allBrushes.Clear();
      CurrentBrush = null;
      if (OnBrushCollectionChanged != null)
        OnBrushCollectionChanged(null, EventArgs.Empty);
    }

    /// <summary>
    /// Finds a brush by its name
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    static public TerrainBrush FindBrush(string name)
    {
      foreach (TerrainBrush b in _allBrushes)
        if (string.Compare(b.DisplayName, name, true) == 0)
          return b;
      return null;
    }

    /// <summary>
    /// Finds a brush by its filename
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    static public TerrainBrush FindBrushByFilename(string filename)
    {
      foreach (TerrainBrush b in _allBrushes)
        if (string.Compare(b.Name, filename, true) == 0)
          return b;
      return null;
    }

    public static string AbsoluteBrushFilePath
    {
      get
      {
        return Path.Combine(EditorManager.AppDataDir, @"TerrainPlugin\Brushes\WeightingMaps");
      }
    }

    /// <summary>
    /// Load all default brushes from the directory
    /// </summary>
    static void LoadAllBrushes()
    {
      if (!EditorManager.EngineManager.IsInitialized())
        return;

      try
      {
        FreeAllBrushes();
        string dir = AbsoluteBrushFilePath;
        string[] files = Directory.GetFiles(dir, "*.bmp");
        foreach (string f in files)
        {
          TerrainBrush b = TerrainConversionHelpers.CreateBrush(f);
          if (b == null || !b.Valid)
            continue;
          _allBrushes.Add(b);
        }

      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex, true);
      }

      if (OnBrushCollectionChanged != null)
        OnBrushCollectionChanged(null, EventArgs.Empty);

      if (!string.IsNullOrEmpty(_preferredBrushName))
      {
        TerrainBrush b = FindBrush(_preferredBrushName);
        if (b != null)
          CurrentBrush = b;
      }
      if (CurrentBrush == null)
        CurrentBrush = FindBrush("Smooth1"); // preferred brush to start with
      if (CurrentBrush == null && _allBrushes.Count > 0)
        CurrentBrush = (TerrainBrush)_allBrushes[0];

    }

    static string _preferredBrushName = null;

    /// <summary>
    /// Set the preferred brush name (will be set later if no brushes are yet loaded)
    /// </summary>
    /// <param name="name"></param>
    public static void SetBrushByName(string name)
    {
      _preferredBrushName = name;
      TerrainBrush brush = TerrainEditor.FindBrush(name);
      if (brush != null)
        TerrainEditor.CurrentBrush = brush;
    }

    public static void LoadNewBrushFiles(string[] absNames)
    {
      try
      {
        foreach (string f in absNames)
        {
          TerrainBrush b = FindBrushByFilename(f);
          if (b == null)
            b = TerrainConversionHelpers.CreateBrush(f);
          else
          {
            TerrainConversionHelpers.CheckBrushModified(b);
            continue;
          }

          if (b == null || !b.Valid)
            continue;
          _allBrushes.Add(b);
        }
      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex, true);
      }

      if (OnBrushCollectionChanged != null)
        OnBrushCollectionChanged(null, EventArgs.Empty);
    }

		#endregion

    #region Selectiopn

    public static TerrainSelection CurrentSelection
    {
      get
      {
        if (CurrentTerrain == null)
          return null;
        return CurrentTerrain.CurrentSelection;
      }
    }

    #endregion

    #region Filters / Heightmap Import

    static ArrayList _HeightmapImporterList = new ArrayList();
    static ArrayList _HeightmapFilterList = new ArrayList();
    static ArrayList _DecorationFilterList = new ArrayList();

    /// <summary>
    /// List of supported height map importer plugins. Register custom plugins here.
    /// Each entry must be of type IHeightmapImporter
    /// </summary>
    public static ArrayList HeightmapImporterList
    {
      get { return TerrainEditor._HeightmapImporterList; }
    }

    /// <summary>
    /// List of supported height map filter plugins. Register custom plugins here.
    /// Each entry must be of type IHeightmapFilter
    /// </summary>
    public static ArrayList HeightmapFilterList
    {
      get { return TerrainEditor._HeightmapFilterList; }
    }

    /// <summary>
    /// List of supported decoration filter plugins. Register custom plugins here.
    /// Each entry must be of type IDecorationFilter
    /// </summary>
    public static ArrayList DecorationFilterList
    {
      get { return TerrainEditor._DecorationFilterList; }
    }

    #endregion

    #region Painting (general)

    static bool _paintingDirty = false;
    static TerrainChangedAction _pendingChangedAction = null;

    /// <summary>
    /// Marks the current paint process as dirty so an update action is triggered at the end
    /// </summary>
    public static bool PaintingDirty
    {
      get { return TerrainEditor._paintingDirty; }
      set { TerrainEditor._paintingDirty = value; }
    }

    /// <summary>
    /// Starts a painting process, currently used for all painting modes. Called by the mouse context
    /// </summary>
    static public void BeginUpdatePainting()
    {
      PaintingDirty = false;
      _pendingChangedAction = null;

      if (CurrentTerrain == null)
        return;

      CurrentTerrain.EngineTerrain.SetCurrentVegetationModel(CurrentDecorationModel);
      CurrentTerrain.EngineTerrain.SetCurrentDetailTexture(CurrentDetailTexture);
    }


    /// <summary>
    /// Ends the current painting process and triggers the update action. Called by the mouse context
    /// </summary>
    static public void FinishUpdatePainting()
    {
      if (CurrentTerrain == null || !PaintingDirty)
        return;
      if (_pendingChangedAction!=null)
        EditorManager.Actions.Add(_pendingChangedAction);
      PaintingDirty = false;
    }


    #endregion

    #region Painting Heightmap

    static HeightmapEditMode_e _heightmapEditMode = HeightmapEditMode_e.None;
    static public event EventHandler OnHeightmapEditModeChanged = null;
    public static event EventHandler OnHeightmapExportFilenameChanged = null;


    /// <summary>
    /// Triggers OnHeightmapExportFilenameChanged
    /// </summary>
    public static void TriggerOnHeightmapExportFilenameChanged()
    {
      if (OnHeightmapExportFilenameChanged!=null)
        OnHeightmapExportFilenameChanged(null, EventArgs.Empty);
    }

    /// <summary>
    /// Gets or sets the current heightmap painting mode
    /// </summary>
    public static HeightmapEditMode_e HeightmapEditMode
    {
      get { return TerrainEditor._heightmapEditMode; }
      set 
      {
        if (TerrainEditor._heightmapEditMode == value)
          return;
        TerrainEditor._heightmapEditMode = value;
        if (OnHeightmapEditModeChanged != null)
          OnHeightmapEditModeChanged(null, EventArgs.Empty);

        if (CurrentTerrain != null && CurrentTerrain.EngineTerrain != null)
        {
          CurrentTerrain.EngineTerrain.SetDisplayHoleOverlay(_heightmapEditMode == HeightmapEditMode_e.AddHoles);
          CurrentTerrain.SetShowDirectionalLightShape(false);
        }

        EditorManager.ActiveView.UpdateView(false);
      }
    }

    /// <summary>
    /// Blits the brush into the heightfield using current cursor, position and brush. Called by mouse context.
    /// </summary>
    /// <param name="mode"></param>
		static public void BlitHeightmapBrush(HeightmapEditMode_e mode)
		{
			if (CurrentTerrain == null || CurrentBrush==null)
				return;

      if (_pendingChangedAction == null)
        _pendingChangedAction = new PaintHeightmapAction(CurrentTerrain);
			CurrentTerrain.EngineTerrain.BlitHeightBrush(CurrentBrush, (HeightmapCursorProperties)Cursor3DProperties, mode);
      PaintingDirty = true;
    }

		#endregion

    #region Painting Decoration

    static DecorationModelResource _currentDecorationModel = null;
    static bool _bPreviewDecorationModelWeightmap = false;
    public static event EventHandler OnCurrentDecorationModelChanged = null;
    public static event EventHandler OnDecorationModelListChanged = null;
    public static event EventHandler OnDecorationDirtyRegionChanged = null;

    /// <summary>
    /// Helper to trigger the OnCurrentDecorationModelChanged event
    /// </summary>
    public static void TriggerOnCurrentDecorationModelChanged()
    {
      if (OnCurrentDecorationModelChanged != null)
        OnCurrentDecorationModelChanged(null, EventArgs.Empty);
      UpdatePreviewDecorationModelWeightmap();
      EditorManager.ActiveView.UpdateView(false);
    }

    /// <summary>
    /// Helper to trigger the OnDecorationModelListChanged event
    /// </summary>
    public static void TriggerOnDecorationModelListChanged()
    {
      if (OnDecorationModelListChanged != null)
        OnDecorationModelListChanged(null, EventArgs.Empty);
      EditorManager.ActiveView.UpdateView(false);
    }

    /// <summary>
    /// Helper to trigger the OnDecorationDirtyRegionChanged event
    /// </summary>
    public static void TriggerOnDecorationDirtyRegionChanged()
    {
      if (OnDecorationDirtyRegionChanged != null)
        OnDecorationDirtyRegionChanged(null, EventArgs.Empty);
      EditorManager.ActiveView.UpdateView(false);
    }

    /// <summary>
    /// Gets or sets the current decoration model. Called by list view
    /// </summary>
    static public DecorationModelResource CurrentDecorationModel
    {
      get { return _currentDecorationModel; }
      set 
      {
        if (_currentDecorationModel == value)
          return;
        _currentDecorationModel = value;
        TriggerOnCurrentDecorationModelChanged();
      }
    }

    /// <summary>
    /// Internal function
    /// </summary>
    static void UpdatePreviewDecorationModelWeightmap()
    {
      if (CurrentTerrain == null || CurrentTerrain.EngineTerrain==null)
        return;
      if (_bPreviewDecorationModelWeightmap)
        CurrentTerrain.EngineTerrain.SetPreviewVegetationModelWeightmap(CurrentDecorationModel);
      else
        CurrentTerrain.EngineTerrain.SetPreviewVegetationModelWeightmap(null);
    }

    /// <summary>
    /// Gets or sets the status whether the current decoration model should display its luminance texture
    /// </summary>
    public static bool PreviewDecorationModelWeightmap
    {
      get { return TerrainEditor._bPreviewDecorationModelWeightmap; }
      set 
      { 
        TerrainEditor._bPreviewDecorationModelWeightmap = value;
        TriggerOnCurrentDecorationModelChanged();
      }
    }

    /// <summary>
    /// Blits the brush into the decoration luminance texture using current cursor, position and brush. Called by mouse context.
    /// </summary>
    /// <param name="bSubtract"></param>
    static public void BlitDecorationBrush(bool bSubtract)
    {
      if (CurrentTerrain == null || CurrentBrush == null || CurrentDecorationModel==null)
        return;

      if (_pendingChangedAction == null)
        _pendingChangedAction = new PaintDecorationAction(CurrentTerrain, CurrentDecorationModel);
      CurrentTerrain.EngineTerrain.BlitVegetationBrush(CurrentBrush, Cursor3DProperties, bSubtract);
      PaintingDirty = true;
    }

    #endregion

    #region Painting Detail Textures

    static DetailTextureResource _currentDetailTexture = null;
    static bool _bTextureBakedView = false;
    static bool _bDirectionalLighting = true;
    static bool _bDisplayMinimap = false;
    static bool _bPreviewFullRes = false;

    public static event EventHandler OnCurrentDetailTextureChanged = null;
    public static event DetailTextureListChangedEventHandler OnDetailTextureListChanged = null;
    public static event EventHandler OnTextureBakedViewChanged = null;
    public static event EventHandler OnDirectionalLightingChanged = null;
    public static event EventHandler OnMiniMapStatusChanged = null;
    public static event EventHandler OnPreviewFullResolutionStatusChanged = null;

    /// <summary>
    /// Status that determines whether directional lighting should be used as a preview mode
    /// </summary>
    public static bool DirectionalLighting
    {
      get { return TerrainEditor._bDirectionalLighting; }
      set 
      {
        if (_bDirectionalLighting == value)
          return;
        _bDirectionalLighting = value;
        if (OnDirectionalLightingChanged != null)
          OnDirectionalLightingChanged(null, EventArgs.Empty);
        if (CurrentTerrain != null && CurrentTerrain.EngineTerrain != null)
        {
          CurrentTerrain.SetShowDirectionalLightShape(_bDirectionalLighting);
          CurrentTerrain.EngineTerrain.SetPreviewDirectionalLight(_bDirectionalLighting);
        }
        EditorManager.ActiveView.UpdateView(false);
      }
    }

    /// <summary>
    /// Status that determines whether the mini map texture overlay shall be shown
    /// </summary>
    public static bool DisplayMiniMap
    {
      get { return TerrainEditor._bDisplayMinimap; }
      set
      {
        if (_bDisplayMinimap == value)
          return;
        _bDisplayMinimap = value;
        if (OnMiniMapStatusChanged != null)
          OnMiniMapStatusChanged(null, EventArgs.Empty);
        if (CurrentTerrain != null && CurrentTerrain.EngineTerrain != null)
          CurrentTerrain.EngineTerrain.SetDisplayMiniMap(_bDisplayMinimap);
        EditorManager.ActiveView.UpdateView(false);
      }
    }

    /// <summary>
    /// Determines whether terrain should be rendered with full resolution (no LOD)
    /// </summary>
    public static bool PreviewFullRes
    {
      get { return TerrainEditor._bPreviewFullRes; }
      set
      {
        if (_bPreviewFullRes == value)
          return;
        _bPreviewFullRes = value;
        if (OnPreviewFullResolutionStatusChanged != null)
          OnPreviewFullResolutionStatusChanged(null, EventArgs.Empty);
        if (CurrentTerrain != null && CurrentTerrain.EngineTerrain != null)
          CurrentTerrain.EngineTerrain.SetForceFullLOD(_bPreviewFullRes);
        EditorManager.ActiveView.UpdateView(false);
      }
    }


    /// <summary>
    /// Helper function to trigger OnCurrentDetailTextureChanged
    /// </summary>
    public static void TriggerOnCurrentDetailTextureChanged()
    {
      if (OnCurrentDetailTextureChanged != null)
        OnCurrentDetailTextureChanged(null, EventArgs.Empty);
      EditorManager.ActiveView.UpdateView(false);
    }

    /// <summary>
    /// Helper function to trigger OnDetailTextureListChanged
    /// </summary>
    public static void TriggerOnDetailTextureListChanged(DetailTextureListChangedArgs e)
    {
      if (OnDetailTextureListChanged != null)
        OnDetailTextureListChanged(null, e);
      EditorManager.ActiveView.UpdateView(false);
    }

    /// <summary>
    /// Helper function to trigger OnMiniMapStatusChanged
    /// </summary>
    public static void TriggerOnMiniMapStatusChanged()
    {
      if (OnMiniMapStatusChanged != null)
        OnMiniMapStatusChanged(null, EventArgs.Empty);
      EditorManager.ActiveView.UpdateView(false);
    }

    /// <summary>
    /// Gets or sets the current detail texture. Called by list view
    /// </summary>
    static public DetailTextureResource CurrentDetailTexture
    {
      get { return _currentDetailTexture; }
      set
      {
        if (_currentDetailTexture == value)
          return;
        _currentDetailTexture = value;
        TriggerOnCurrentDetailTextureChanged();
      }
    }

    /// <summary>
    /// If enabled, the texturing will be shown as in the engine
    /// </summary>
    static public bool TextureBakedView
    {
      get { return _bTextureBakedView; }
      set 
      {
        if (_bTextureBakedView == value)
          return;
        _bTextureBakedView = value;
        if (OnTextureBakedViewChanged != null)
          OnTextureBakedViewChanged(null, EventArgs.Empty);
        if (CurrentTerrain != null && CurrentTerrain.EngineTerrain != null)
          CurrentTerrain.EngineTerrain.SetUseBakedTextures(_bTextureBakedView, CurrentTerrain.ShaderConfig);

        EditorManager.ActiveView.UpdateView(false);
      }
    }

    /// <summary>
    /// Blits the brush into the detail texture channel using current cursor, position and brush. Called by mouse context.
    /// </summary>
    /// <param name="bSubtract"></param>
    static public void BlitDetailTextureBrush(bool bSubtract)
    {
      if (CurrentTerrain == null || CurrentBrush == null || CurrentDetailTexture == null)
        return;

      if (_pendingChangedAction == null)
        _pendingChangedAction = new PaintDetailTextureAction(CurrentTerrain, CurrentDetailTexture);
      CurrentTerrain.EngineTerrain.BlitDetailTexture(CurrentBrush, Cursor3DProperties, bSubtract);
      PaintingDirty = true;
    }

    #endregion

    #region Automated Tests

    static Dialogs.TerrainEditorPanel _editorPanel = null;

    /// <summary>
    /// Returns the terrain editor panel (UI component).
    /// Please use this method for automated tests only. Production code should not access/modify via UI components.
    /// </summary>
    static public Dialogs.TerrainEditorPanel EditorPanel
    {
      get { return _editorPanel; }
      set { _editorPanel = value; }
    }

    #endregion

  }

  #region Event Delegates
  /// <summary>
  /// Delegate declaration. Sends a data object of type PropertyChangedArgs which provides information about
  /// the property
  /// </summary>
  public delegate void DetailTextureListChangedEventHandler(object sender, DetailTextureListChangedArgs e);

  #endregion


  #region Class: DetailTextureResourcePropertyChangedArgs

  /// <summary>
  /// Event class for changed shape properties. This data object is used by the static DetailTextureResource.OnDetailTextureResourcePropertyChanged event.
  /// </summary>
  public class DetailTextureListChangedArgs : EventArgs
  {


    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="component"></param>
    /// <param name="propertyName"></param>
    public DetailTextureListChangedArgs(Action action, DetailTextureResource textureElement)
    {
      _textureElement = textureElement;
      _action = action;
    }


    /// <summary>
    /// DetailTextureListChanged Action
    /// </summary>
    public enum Action
    {
      /// <summary>
      /// Triggered when a texture is removed
      /// </summary>
      RemovedTexture,

      /// <summary>
      ///  Triggered when a texture is added
      /// </summary>
      AddedTexture,

      /// <summary>
      /// Order of the texture list has changed
      /// </summary>
      OrderChanged,

      /// <summary>
      /// A new texture list was created
      /// </summary>
      NewCreated
    };

    /// <summary>
    /// Related action (reason why event got triggered)
    /// </summary>
    public Action _action;

    /// <summary>
    /// Name of the affected texture
    /// </summary>
    public DetailTextureResource _textureElement;

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
