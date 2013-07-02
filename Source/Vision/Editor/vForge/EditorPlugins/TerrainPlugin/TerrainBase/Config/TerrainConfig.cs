/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using CSharpFramework;
using System.ComponentModel;
using System.Runtime.Serialization;
using System.Drawing.Design;
using CSharpFramework.Scene;
using TerrainBase.Editing;
using CSharpFramework.PropertyEditors;
using System.IO;
using CSharpFramework.UndoRedo;
using CSharpFramework.Serialization;
using ManagedBase;
using CSharpFramework.Math;
using System.Collections;
using System.Windows.Forms;

namespace TerrainBase.Config
{
  #region Class TerrainConfig

    /// <summary>
    /// This class is the C# representation of VTerrainConfig 
    /// </summary>
    [Serializable()]
    public class TerrainConfig : ICloneable, ISerializable, ISceneObject
    {
      public TerrainConfig()
      {
        _baseTexture.DiffuseFilename = @"\Textures\sand.dds";
        _baseTexture.Tiling = new Vector2F(8.0f, -8.0f);

        _sectorSize *= EditorManager.Settings.GlobalUnitScaling;
        _visibilityHeightOverTerrain *= EditorManager.Settings.GlobalUnitScaling;
      }
      /// <summary>
      /// Overridden ToString function to display the config in the property grid
      /// </summary>
      /// <returns></returns>
      public override string ToString()
      {
        return "<click here>";
      }


      #region Events

      public static event EventHandler OnMemberChanged = null;

      #endregion

      #region Validation

      public static int GetNextPowerOf2(int z, int imin, int imax)
      {
        while (imin < z && imin < imax)
          imin *= 2;
        return imin;
      }

      public void Validate()
      {
        if (_sectorCount.Width < 1) _sectorCount.Width = 1;
        if (_sectorCount.Height < 1) _sectorCount.Height = 1;
        if (_sectorCount.Width > 1024) _sectorCount.Width = 1024;
        if (_sectorCount.Height > 1024) _sectorCount.Height = 1024;

        if (_sectorSize.X < 1.0f) _sectorSize.X = 1.0f;
        if (_sectorSize.Y < 1.0f) _sectorSize.Y = 1.0f;
        if (_sectorSize.X > 100000.0f) _sectorSize.X = 100000.0f;
        if (_sectorSize.Y > 100000.0f) _sectorSize.Y = 100000.0f;

        _samplesPerSector.Width = GetNextPowerOf2(_samplesPerSector.Width, 32, 4096);
        _samplesPerSector.Height = GetNextPowerOf2(_samplesPerSector.Height, 32, 4096);
        _tilesPerSector.Width = GetNextPowerOf2(_tilesPerSector.Width, 2, _samplesPerSector.Width / 2);
        _tilesPerSector.Height = GetNextPowerOf2(_tilesPerSector.Height, 2, _samplesPerSector.Width / 2);
        if (_tilesPerSector.Width < 2) _tilesPerSector.Width = 2;
        if (_tilesPerSector.Height < 2) _tilesPerSector.Height = 2;
        if (_tilesPerSector.Width > _samplesPerSector.Width / 2) _samplesPerSector.Width = _samplesPerSector.Width / 2;
        if (_tilesPerSector.Height > _samplesPerSector.Height / 2) _samplesPerSector.Height = _samplesPerSector.Height / 2;

        _defaultWeightmapResolution.Width = GetNextPowerOf2(_defaultWeightmapResolution.Width, 16, 2048);
        _defaultWeightmapResolution.Height = GetNextPowerOf2(_defaultWeightmapResolution.Height, 16, 2048);
        _defaultDensitymapResolution.Width = GetNextPowerOf2(_defaultDensitymapResolution.Width, 16, 1024);
        _defaultDensitymapResolution.Height = GetNextPowerOf2(_defaultDensitymapResolution.Height, 16, 1024);
        if (_visibilityHeightOverTerrain < 0.0f)
          _visibilityHeightOverTerrain = 0.0f;


        if (_sectorCount.Width > _oldSectorCountX && _oldSectorCountX>0)
        {
          if (_canvasOfsX > _sectorCount.Width - _oldSectorCountX)
            _canvasOfsX = _sectorCount.Width - _oldSectorCountX;
          if (_canvasOfsX < 0)
            _canvasOfsX = 0;
        } 
        else
          _canvasOfsX = 0;

        if (_sectorCount.Height > _oldSectorCountY && _oldSectorCountY > 0)
        {
          if (_canvasOfsY > _sectorCount.Height - _oldSectorCountY)
            _canvasOfsY = _sectorCount.Height - _oldSectorCountY;
          if (_canvasOfsY < 0)
            _canvasOfsY = 0;
        }
        else
          _canvasOfsY = 0;

        if (_fSectorPurgeInterval <= 0.0f) 
          _fSectorPurgeInterval = -1.0f;

        if (_iMemoryLimit < 0)
          _iMemoryLimit = 0;
        else if (_iMemoryLimit > 2000)
          _iMemoryLimit = 2000;

        if (_materialmapResolution.Width < 0) _materialmapResolution.Width = 0;
          else if (_materialmapResolution.Width > _defaultWeightmapResolution.Width) _materialmapResolution.Width = _defaultWeightmapResolution.Width;
        if (_materialmapResolution.Height < 0) _materialmapResolution.Height = 0;
          else if (_materialmapResolution.Height > _defaultWeightmapResolution.Height) _materialmapResolution.Height = _defaultWeightmapResolution.Height;

        if (OnMemberChanged != null)
          OnMemberChanged(this, EventArgs.Empty);
      }

      void BeforeValueModified()
      {
        if (_compareModified == null && !_bIsComparisonConfig)
        {
          _compareModified = (TerrainConfig)this.Clone();
          _compareModified._bIsComparisonConfig = true;
        }
      }

      public override int GetHashCode()
      {
        return base.GetHashCode();
      }


      /// <summary>
      /// Performs a memberwise comparison
      /// </summary>
      /// <param name="obj"></param>
      /// <returns></returns>
      public override bool Equals(object obj)
      {
        TerrainConfig other = obj as TerrainConfig;
        if (other==null)
          return base.Equals(obj);

        if (_sectorCount != other._sectorCount) return false;
        if (_sectorSize != other._sectorSize) return false;
        if (_samplesPerSector != other._samplesPerSector) return false;
        if (_tilesPerSector != other._tilesPerSector) return false;
        if (_terrainPos != other._terrainPos) return false;
        if (BaseTextureFile != other.BaseTextureFile) return false;
        if (BaseTextureTiling != other.BaseTextureTiling) return false;
        if (_defaultWeightmapResolution != other._defaultWeightmapResolution) return false;
        if (_materialmapResolution != other._materialmapResolution) return false;
        if (_visibilityHeightOverTerrain != other._visibilityHeightOverTerrain) return false;
        if (_defaultDensitymapResolution.Width != other._defaultDensitymapResolution.Width) return false;
        if (_defaultDensitymapResolution.Height != other._defaultDensitymapResolution.Height) return false;
        if (_fSectorPurgeInterval != other._fSectorPurgeInterval) return false;
        if (_iMemoryLimit != other._iMemoryLimit) return false;
        if (_bNormalmapUsesBorder != other._bNormalmapUsesBorder) return false;
        if (_terrainFolder != other._terrainFolder) return false;
        return true;
      }


      #endregion

      #region From Importer

      public void SettingsFromImporter(IHeightmapImporter importer)
      {
        _sectorCount.Width = (importer.SizeX + SamplesPerSector.Width - 1) / SamplesPerSector.Width;
        _sectorCount.Height = (importer.SizeY + SamplesPerSector.Height - 1) / SamplesPerSector.Height;

        Vector2F spacing = importer.SuggestedSampleSpacing;
        if (spacing != Vector2F.Zero)
          _sectorSize = new Vector2F(spacing.X * (float)_samplesPerSector.Width, spacing.Y * (float)_samplesPerSector.Height);

        Validate();
      }

      #endregion

      #region members

      public string _terrainFolder = "";

      public Size _sectorCount = new Size(4, 4);
      public Vector2F _sectorSize = new Vector2F(10000, 10000);
      public Size _samplesPerSector = new Size(128, 128);
      public Size _tilesPerSector = new Size(16, 16);
      private Vector3F _terrainPos = Vector3F.Zero;
      public DetailTextureResource _baseTexture = new DetailTextureResource(null, 1);
      public Size _defaultWeightmapResolution = new Size(512, 512);
      public Size _defaultDensitymapResolution = new Size(32, 32);
      public Size _materialmapResolution = new Size(0, 0);

      private bool _centered = true;
      public float _visibilityHeightOverTerrain = 2000.0f;
      public bool _bModifiableOnly = false;
      public float _fSectorPurgeInterval = 120.0f;
      public Int32 _iMemoryLimit = 500;

      public int _oldSectorCountX = -1;
      public int _oldSectorCountY = -1;
      public int _canvasOfsX = 0;
      public int _canvasOfsY = 0;
      public bool _bNormalmapUsesBorder = true;

      // add sector index for sector expansion
      public int _oldSectorIndexX = -1;
      public int _oldSectorIndexY = -1;

      public int _newSectorIndexX = -1;
      public int _newSectorIndexY = -1;

      #endregion

      #region All properties

      [SortedCategory("Terrain", 1), PropertyOrder(1)]
      [Description("Determines the project-relative subfolder for all terrain related files.")]
      [EditorAttribute(typeof(FilenameCreator), typeof(UITypeEditor)), FileDialogFilter(new string[] {}), FileCreateDialogAllowOverwrite(true)]
      public string TerrainFolder
      {
        get { return _terrainFolder; }
        set 
        { 
          BeforeValueModified();
          _terrainFolder = value;
          Validate(); 
        }
      }

      #region Position

      [SortedCategory("Terrain", 1), PropertyOrder(59)]
      [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
      [Description("If enabled, the terrain gets created around the origin.")]
      public bool Centered
      {
        get { return _centered; }
        set { _centered = value; }
      }

      [SortedCategory("Terrain", 1), PropertyOrder(60)]
      [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
      [Description("Specifies the position of sector (0,0) in the world. The terrain shape can be moved later.")]
      public float TerrainPosX
      {
        get 
        {
          if (_centered)
            _terrainPos.X = OverallSizeX * -0.5f;
          return _terrainPos.X; 
        }
        set { BeforeValueModified(); _terrainPos.X = value; _centered = false; Validate(); }
      }

      [SortedCategory("Terrain", 1), PropertyOrder(61)]
      [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
      [Description("Specifies the position of sector (0,0) in the world. The terrain shape can be moved later.")]
      public float TerrainPosY
      {
        get 
        {
          if (_centered)
            _terrainPos.Y = OverallSizeY * -0.5f;
          return _terrainPos.Y; 
        }
        set { BeforeValueModified(); _terrainPos.Y = value; _centered = false; Validate(); }
      }

      [SortedCategory("Terrain", 1), PropertyOrder(62)]
      [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
      [Description("Specifies the position of sector (0,0) in the world. The terrain shape can be moved later.")]
      public float TerrainPosZ
      {
        get { return _terrainPos.Z; }
        set { BeforeValueModified(); _terrainPos.Z = value; Validate(); }
      }

      [Browsable(false)]
      public Vector3F TerrainPos
      {
        get 
        {
          return new Vector3F(TerrainPosX, TerrainPosY, TerrainPosZ); // guarantee to get back centered position 
        }
        set 
        { 
          BeforeValueModified(); 
          _terrainPos = value;
          _centered = false;
          Validate(); 
        }
      }

      [Browsable(false)]
      public Vector3F TerrainPosInternal
      {
        set
        {
          _centered = true;
          _terrainPos = value;

          // still matches the centered position?
          if (_terrainPos.X != TerrainPosX || _terrainPos.Y != TerrainPosY)
          {
            _centered = false;
            _terrainPos = value; // set again
          }
        }
      }

      #endregion

      #region Sector Count

      [SortedCategory("Sector Layout", 2), PropertyOrder(10)]
      [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
      [Description("Determines the number of sector subdivisions the terrain consists of. Each sector is streamable.")]
      public int SectorCountX
      {
        get { return _sectorCount.Width; }
        set { BeforeValueModified(); _sectorCount.Width = value; Validate(); }
      }

      [SortedCategory("Sector Layout", 2), PropertyOrder(11)]
      [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
      [Description("Determines the number of sector subdivisions the terrain consists of. Each sector is streamable.")]
      public int SectorCountY
      {
        get { return _sectorCount.Height; }
        set { BeforeValueModified(); _sectorCount.Height = value; Validate(); }
      }

      [Browsable(false)]
      public Size SectorCount
      {
        get { return _sectorCount; }
        set { BeforeValueModified(); _sectorCount = value; Validate(); }
      }

      [SortedCategory("Sector Layout", 2), PropertyOrder(12)]
      [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
      [Description("If the new terrain has more sectors then this value defines where to position the old one inside the new one")]
      public int CanvasOfsX
      {
        get { return _canvasOfsX; }
        set { BeforeValueModified(); _canvasOfsX = value; Validate(); }
      }

      [SortedCategory("Sector Layout", 2), PropertyOrder(13)]
      [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
      [Description("If the new terrain has more sectors then this value defines where to position the old one inside the new one")]
      public int CanvasOfsY
      {
        get { return _canvasOfsY; }
        set { BeforeValueModified(); _canvasOfsY = value; Validate(); }
      }
 
      #endregion

      #region Sector Size

      [SortedCategory("Sector Layout", 2), PropertyOrder(20)]
      [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
      [Description("Determines the size of one sector in world units.")]
      public float SectorSizeX
      {
        get { return _sectorSize.X; }
        set { BeforeValueModified(); _sectorSize.X = value; Validate(); }
      }

      [SortedCategory("Sector Layout", 2), PropertyOrder(21)]
      [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
      [Description("Determines the size of one sector in world units.")]
      public float SectorSizeY
      {
        get { return _sectorSize.Y; }
        set { BeforeValueModified(); _sectorSize.Y = value; Validate(); }
      }

      [Browsable(false)]
      public Vector2F SectorSize
      {
        get { return _sectorSize; }
        set { BeforeValueModified(); _sectorSize = value; Validate(); }
      }

      #endregion

      #region Samples per sector

      [SortedCategory("Sector Layout", 2), PropertyOrder(30)]
      [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
      [Description("Specifies the number of heightmap samples per sector.")]
      public int SamplesPerSectorX
      {
        get { return _samplesPerSector.Width; }
        set 
        { 
          BeforeValueModified(); _samplesPerSector.Width = value; Validate();
          if (_samplesPerSector.Width > 1024)
            EditorManager.ShowMessageBox("More than 1024 samples per sector edge is not recommended.\nYou might increase the sector count instead.", "Samples per sector", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }
      }

      [SortedCategory("Sector Layout", 2), PropertyOrder(31)]
      [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
      [Description("Specifies the number of heightmap samples per sector.")]
      public int SamplesPerSectorY
      {
        get { return _samplesPerSector.Height; }
        set 
        { 
          BeforeValueModified(); _samplesPerSector.Height = value; Validate();
          if (_samplesPerSector.Height > 1024)
            EditorManager.ShowMessageBox("More than 1024 samples per sector edge is not recommended.\nYou might increase the sector count instead.", "Samples per sector", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }
      }

      [Browsable(false)]
      public Size SamplesPerSector
      {
        get { return _samplesPerSector; }
        set { BeforeValueModified(); _samplesPerSector = value; Validate(); }
      }

      #endregion

      #region Tiles per sector

      [SortedCategory("Sector Layout", 2), PropertyOrder(40)]
      [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
      [Description("Specifies the subdivision of sectors into smaller tiles. The tiles define the granularity for possible holes.")]
      public int TilesPerSectorX
      {
        get { return _tilesPerSector.Width; }
        set { BeforeValueModified(); _tilesPerSector.Width = value; Validate(); }
      }

      [SortedCategory("Sector Layout", 2), PropertyOrder(41)]
      [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
      [Description("Specifies the subdivision of sectors into smaller tiles. The tiles define the granularity for possible holes.")]
      public int TilesPerSectorY
      {
        get { return _tilesPerSector.Height; }
        set { BeforeValueModified(); _tilesPerSector.Height = value; Validate(); }
      }

      [Browsable(false)]
      public Size TilesPerSector
      {
        get { return _tilesPerSector; }
        set { BeforeValueModified(); _tilesPerSector = value; Validate(); }
      }

      #endregion

      #region Visibility

      [SortedCategory("Sector Layout", 2), PropertyOrder(50)]
      [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
      [Description("Specifies the number of units the visibility box is higher than a terrain sector. Too large values will overestimate the sector visibility in the distance, while too small values might cause visibility problems for objects floating in the air, including the camera")]
      public float VisibilityHeightOverTerrain
      {
        get { return _visibilityHeightOverTerrain; }
        set { BeforeValueModified(); _visibilityHeightOverTerrain = value; Validate(); }
      }

      
      #endregion

      #region Streaming

      [SortedCategory("Streaming", 5), PropertyOrder(1)]
      [Description("Specifies the time (in seconds) after which terrain sectors are thrown out of memory when they are out of cached radius")]
      public float SectorPurgeInterval
      {
        get { return _fSectorPurgeInterval; }
        set { BeforeValueModified(); _fSectorPurgeInterval = value; Validate(); }
      }

      [SortedCategory("Streaming", 5), PropertyOrder(2)]
      [Description("Memory limit in megabytes for the terrain manager. 0 means no limitation. If the limit is exceeded the terrain manager is purged (not used sectors first)")]
      public int MemoryLimit
      {
        get { return _iMemoryLimit; }
        set { BeforeValueModified(); _iMemoryLimit = value; Validate(); }
      }

      #endregion


      [SortedCategory("Texturing", 3), PropertyOrder(70)]
      [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
      [EditorAttribute(typeof(AssetEditor), typeof(UITypeEditor)), AssetDialogFilter(new string[] { "Texture" })]
      [Description("Specifies the base texture. This texture can be changed later.")]
      public string BaseTextureFile
      {
        get { return _baseTexture.DiffuseFilename; }
        set { BeforeValueModified(); _baseTexture.DiffuseFilename = value; Validate(); _baseTexImage = null; }
      }

      [SortedCategory("Texturing", 3), PropertyOrder(71)]
      [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
      [Description("If enabled, filtering of the world space normalmap is simulated across sectors. This feature costs a bit more texture memory but avoids potential lighting seams at sector borders.")]
      public bool NormalmapUsesBorder
      {
        get { return _bNormalmapUsesBorder; }
        set { BeforeValueModified(); _bNormalmapUsesBorder = value; Validate(); }
      }



      int iCurrentImageSize = 0;
      Image _baseTexImage = null;

      /// <summary>
      /// Returns the image representation of current base texture file
      /// </summary>
      public Image GetBaseTextureImage(int iSize)
      {
        if (_baseTexImage != null && iCurrentImageSize==iSize)
          return _baseTexImage;

        _baseTexImage = null;
        if (!string.IsNullOrEmpty(_baseTexture.DiffuseFilename))
        {
          string infostr;
          _baseTexImage = ImageLoader.LoadImage(EditorManager.Project.MakeAbsolute(_baseTexture.DiffuseFilename), out infostr, iSize);
          iCurrentImageSize = iSize;
        }
        return _baseTexImage;
      }

      #region Basetexture tiling

      [SortedCategory("Texturing", 3), PropertyOrder(80)]
      [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
      [Description("Specifies the tiling factor of the base texture.")]
      public float BaseTextureTilingX
      {
        get { return _baseTexture.Tiling.X; }
        set { BaseTextureTiling = new Vector2F(value, _baseTexture.Tiling.Y); }
      }

      [SortedCategory("Texturing", 3), PropertyOrder(81)]
      [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
      [Description("Specifies the tiling factor of the base texture.")]
      public float BaseTextureTilingY
      {
        get { return _baseTexture.Tiling.Y; }
        set { BaseTextureTiling = new Vector2F(_baseTexture.Tiling.X, value); }
      }

      [Browsable(false)]
      public Vector2F BaseTextureTiling
      {
        get { return _baseTexture.Tiling; }
        set { BeforeValueModified(); _baseTexture.Tiling = value; Validate(); }
      }

      #endregion

      [SortedCategory("Texturing", 3), PropertyOrder(90)]
      [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
      [Description("Specifies the default resolution of a sector's weightmap texture.")]
      public int DefaultWeightmapResolutionX
      {
        get { return _defaultWeightmapResolution.Width; }
        set { BeforeValueModified(); _defaultWeightmapResolution.Width = value; Validate(); }
      }

      [SortedCategory("Texturing", 3), PropertyOrder(91)]
      [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
      [Description("Specifies the default resolution of a sector's weightmap texture.")]
      public int DefaultWeightmapResolutionY
      {
        get { return _defaultWeightmapResolution.Height; }
        set { BeforeValueModified(); _defaultWeightmapResolution.Height = value; Validate(); }
      }

      [Browsable(false)]
      public Size DefaultWeightmapResolution
      {
        get { return _defaultWeightmapResolution; }
        set { BeforeValueModified(); _defaultWeightmapResolution = value; Validate(); }
      }

      [SortedCategory("Texturing", 3), PropertyOrder(100)]
      [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
      [Description("If larger than zero, the terrain system builds a material map (per sector) that stores the most relevant texture ID so it can be queried by the runtime")]
      public int MaterialIDMapResolutionX
      {
        get { return _materialmapResolution.Width; }
        set { BeforeValueModified(); _materialmapResolution.Width = value; Validate(); }
      }

      [SortedCategory("Texturing", 3), PropertyOrder(101)]
      [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
      [Description("If larger than zero, the terrain system builds a material map (per sector) that stores the most relevant texture ID so it can be queried by the runtime")]
      public int MaterialIDMapResolutionY
      {
        get { return _materialmapResolution.Height; }
        set { BeforeValueModified(); _materialmapResolution.Height = value; Validate(); }
      }

      [Browsable(false)]
      public bool HasMaterialMap
      {
        get
        {
          return _materialmapResolution.Width>0 && _materialmapResolution.Height>0;
        }
      }

      #region Vegetation

      [SortedCategory("Vegetation", 4), PropertyOrder(10)]
      [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
      [Description("Specifies the resolution (per sector) for editing the vegetation density map (only relevant inside the editor).")]
      public int VegetationMapResolutionX
      {
        get { return _defaultDensitymapResolution.Width; }
        set { BeforeValueModified(); _defaultDensitymapResolution.Width = value; Validate(); }
      }

      [SortedCategory("Vegetation", 4), PropertyOrder(11)]
      [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
      [Description("Specifies the resolution (per sector) for editing the vegetation density map (only relevant inside the editor).")]
      public int VegetationMapResolutionY
      {
        get { return _defaultDensitymapResolution.Height; }
        set { BeforeValueModified(); _defaultDensitymapResolution.Height = value; Validate(); }
      }

      [Browsable(false)]
      public Size VegetationMapResolution
      {
        get { return _defaultDensitymapResolution; }
        set { BeforeValueModified(); _defaultDensitymapResolution = value; Validate(); }
      }

      #endregion

      #endregion

      #region Calculated members

      [SortedCategory("Statistics", 8), PropertyOrder(100)]
      [Description("Specifies the overall number of sectors.")]
      public int OverallSectorCount
      {
        get { return _sectorCount.Width * _sectorCount.Height; }
      }


      [SortedCategory("Statistics", 8), PropertyOrder(101)]
      public Size OverallHeightSampleCount
      {
        get
        {
          return new Size(_samplesPerSector.Width * _sectorCount.Width, _samplesPerSector.Height * _sectorCount.Height);
        }
      }

      [SortedCategory("Statistics", 8), PropertyOrder(102)]
      public float OverallSizeX
      {
        get { return (float)_sectorCount.Width*_sectorSize.X; }
      }

      [SortedCategory("Statistics", 8), PropertyOrder(103)]
      public float OverallSizeY
      {
        get { return (float)_sectorCount.Height * _sectorSize.Y; }
      }

      public float GetSectorOriginX(int iSectorX)
      {
        return this._sectorSize.X * (float)iSectorX + TerrainPosX;
      }

      public float GetSectorOriginY(int iSectorY)
      {
        return this._sectorSize.Y * (float)iSectorY + TerrainPosY;
      }

      public void GetSectorIndicesAtWorldPos(Vector3F pos, out int iSectorX, out int iSectorY)
      {
        iSectorX = (int)((pos.X - this.TerrainPosX) / this.SectorSizeX);
        iSectorY = (int)((pos.Y - this.TerrainPosY) / this.SectorSizeY);
      }

      public bool ClampSectorRange(ref int x1, ref int y1, ref int x2, ref int y2)
      {
        if (x1 >= this.SectorCountX || x2 < 0 || y1 >= this.SectorCountY || y2 < 0)
          return false;
        if (x1 < 0) x1 = 0;
        if (y1 < 0) y1 = 0;
        if (x2 >= SectorCountX) x2 = SectorCountX - 1;
        if (y2 >= SectorCountY) y2 = SectorCountY - 1;
        return true;
      }

      #endregion

      #region ICloneable Members

      public object Clone()
      {
        TerrainConfig copy = (TerrainConfig)this.MemberwiseClone();
        copy._baseTexture = (DetailTextureResource)this._baseTexture.Clone();
        return copy;
      }

      #endregion

      #region ISerializable Members

      public TerrainConfig(SerializationInfo info, StreamingContext context)
      {
        _terrainFolder = info.GetString("_terrainFolder");
        _sectorCount = (Size)info.GetValue("_sectorCount", typeof(Size));
        _tilesPerSector = (Size)info.GetValue("_tilesPerSector", typeof(Size));
        _samplesPerSector = (Size)info.GetValue("_samplesPerSector", typeof(Size));
        _sectorSize = (Vector2F)info.GetValue("_sectorSize", typeof(Vector2F));
        _terrainPos = (Vector3F)info.GetValue("_terrainPos", typeof(Vector3F));
        if (SerializationHelper.HasElement(info, "_VisibilityHeightOverTerrain"))
          _visibilityHeightOverTerrain = info.GetSingle("_visibilityHeightOverTerrain");
        if (SerializationHelper.HasElement(info, "_fSectorPurgeInterval"))
          _fSectorPurgeInterval = info.GetSingle("_fSectorPurgeInterval");
        if (SerializationHelper.HasElement(info, "_iMemoryLimit"))
          _iMemoryLimit = info.GetInt32("_iMemoryLimit");
        if (SerializationHelper.HasElement(info, "_bNormalmapUsesBorder"))
          _bNormalmapUsesBorder = info.GetBoolean("_bNormalmapUsesBorder");
        if (SerializationHelper.HasElement(info, "_materialmapResolution"))
          _materialmapResolution = (Size)info.GetValue("_materialmapResolution", typeof(Size));
      }

      public void GetObjectData(SerializationInfo info, StreamingContext context)
      {
        info.AddValue("_terrainFolder", _terrainFolder);
        info.AddValue("_sectorCount", _sectorCount);
        info.AddValue("_sectorSize", _sectorSize);
        info.AddValue("_samplesPerSector", _samplesPerSector);
        info.AddValue("_tilesPerSector", _tilesPerSector);
        info.AddValue("_terrainPos", _terrainPos);
        info.AddValue("_visibilityHeightOverTerrain", _visibilityHeightOverTerrain);
        info.AddValue("_fSectorPurgeInterval", _fSectorPurgeInterval);
        info.AddValue("_iMemoryLimit", _iMemoryLimit);
        info.AddValue("_bNormalmapUsesBorder", _bNormalmapUsesBorder);
        info.AddValue("_materialmapResolution", _materialmapResolution);
      }

      #endregion

      #region ISceneObject Members

      [Browsable(false)]
      public bool Modifiable
      {
        get { return true; }
      }
      
      bool _bDirty = false;
      bool _bIsComparisonConfig = false;

      TerrainConfig _compareModified = null;

      [Browsable(false)]
      public bool Modified
      {
        get 
        {
          if (_compareModified == null)
            return _bDirty;
          return !_compareModified.Equals(this); 
        }
        set 
        {
          _bDirty = value;
          if (!value && !_bIsComparisonConfig) // make a new backup
          {
            _compareModified = (TerrainConfig)this.Clone();
            _compareModified._bIsComparisonConfig = true;
          }
        }
      }

      #endregion

      #region static Helper

      /// <summary>
      /// Generates a folder name for the terrain that does not yet exist in the project
      /// </summary>
      /// <returns></returns>
      public static string CreateUniqueFolderName()
      {
        string start = Path.GetFileNameWithoutExtension(EditorManager.Scene.FileName);
        string result = start + ".TerrainData";
        int iIndex = 1;
        while (Directory.Exists(EditorManager.Project.MakeAbsolute(result)))
        {
          result = string.Format("{0}.{1}.TerrainData", start, iIndex.ToString());
          iIndex++;
        }
        return result;
      }

      #endregion
    }

  #endregion

  #region TerrainPropertySorter

  public class TerrainPropertySorter : PropertySorter
  {

    public override PropertyDescriptorCollection GetProperties(ITypeDescriptorContext context, object value, Attribute[] attributes)
    {
      PropertyDescriptorCollection originalPC = base.GetProperties(context, value, attributes);
      TerrainConfigCategoryBase catBase = value as TerrainConfigCategoryBase;
      if (catBase==null)
        return originalPC;

      ArrayList newProp = new ArrayList(originalPC.Count);
      
      foreach (PropertyDescriptor pd in originalPC)
      {
        if (pd.Name == "CanvasOfsX" || pd.Name == "CanvasOfsY")
        {
          if (catBase._config._oldSectorCountX < 0 || catBase._config._oldSectorCountY < 0)
            continue;
          // for now canvas is disabled:
          continue;
        }

        if (!catBase._config._bModifiableOnly || IsEditableProperty(pd.Name))
          newProp.Add(pd);
        else
          newProp.Add(new UndoRedoPropertyDescriptor(pd, PropertyFlags_e.Readonly, null,null)); // read-only property
      }

      return new PropertyDescriptorCollection((PropertyDescriptor[])newProp.ToArray(typeof(PropertyDescriptor)));
    }

    public static string[] EDITABLE_PROPERTIES = new string[] {
        "TerrainFolder", "Centered", "TerrainPosX", "TerrainPosY", "TerrainPosZ",
        "HeightSamplesX", "HeightSamplesY",
        "SectorCountX", "SectorCountY","CanvasOfsX","CanvasOfsY",
        "SectorSizeX", "SectorSizeY", "TerrainSizeX", "TerrainSizeY", "SampleSpacingX", "SampleSpacingY",
        "VisibilityHeightOverTerrain","SectorPurgeInterval", "MemoryLimit",
        "VegetationMapResolutionX","VegetationMapResolutionY", "NormalmapUsesBorder",
        "MaterialIDMapResolutionX", "MaterialIDMapResolutionY",
        "WeightmapResolutionX", "WeightmapResolutionY"
      };

    public static bool IsEditableProperty(string name)
    {
      foreach (string n in EDITABLE_PROPERTIES)
        if (name == n)
          return true;
      return false;
    }
  }

  public class TerrainConfigCategoryBase
  {
    public TerrainConfigCategoryBase(TerrainConfig configref)
    {
      _config = configref;
    }
    public TerrainConfig _config;
  }

  #endregion

  #region General Properties

  [TypeConverter(typeof(TerrainPropertySorter))]
  public class GeneralTerrainConfig : TerrainConfigCategoryBase
  {
    public GeneralTerrainConfig(TerrainConfig configref) : base(configref)
    {
    }

    #region File

    [SortedCategory("File", 1), PropertyOrder(1)]
    [Description("Determines the project-relative subfolder for all terrain related files.")]   
    [EditorAttribute(typeof(FilenameCreator), typeof(UITypeEditor)), FileDialogFilter(new string[] { }), FileCreateDialogAllowOverwrite(true), FileCreateDialogFolderNameReturnAttribute(true)]
    public string TerrainFolder
    {
      get { return _config.TerrainFolder; }
      set
      {
        _config.TerrainFolder = value;
      }
    }

    #endregion

    #region Position

    [SortedCategory("Positioning", 2), PropertyOrder(59)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("If enabled, the terrain gets created around the origin.")]
    public bool Centered
    {
      get { return _config.Centered; }
      set { _config.Centered = value; }
    }

    [SortedCategory("Positioning", 2), PropertyOrder(60)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("Specifies the position of sector (0,0) in the world. The terrain shape can be moved later.")]
    public float TerrainPosX
    {
      get
      {
        return _config.TerrainPosX;
      }
      set 
      { 
        _config.TerrainPosX = value; 
      }
    }

    [SortedCategory("Positioning", 2), PropertyOrder(61)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("Specifies the position of sector (0,0) in the world. The terrain shape can be moved later.")]
    public float TerrainPosY
    {
      get
      {
        return _config.TerrainPosY;
      }
      set
      {
        _config.TerrainPosY = value;
      }
    }

    [SortedCategory("Positioning", 2), PropertyOrder(62)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("Specifies the position of sector (0,0) in the world. The terrain shape can be moved later.")]
    public float TerrainPosZ
    {
      get
      {
        return _config.TerrainPosZ;
      }
      set
      {
        _config.TerrainPosZ = value;
      }
    }

    #endregion

    #region Visibility

    [SortedCategory("Visibility", 3), PropertyOrder(1)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("Specifies the number of units the visibility box is higher than a terrain sector. Too large values will overestimate the sector visibility in the distance, while too small values might cause visibility problems for objects floating in the air, including the camera")]
    public float VisibilityHeightOverTerrain
    {
      get { return _config.VisibilityHeightOverTerrain; }
      set { _config.VisibilityHeightOverTerrain = value; }
    }

    #endregion

    #region Streaming

    [SortedCategory("Streaming", 5), PropertyOrder(1)]
    [Description("Specifies the time (in seconds) after which terrain sectors are thrown out of memory when they are out of cached camera radius")]
    public float SectorPurgeInterval
    {
      get { return _config.SectorPurgeInterval; }
      set { _config.SectorPurgeInterval = value; }
    }

    [SortedCategory("Streaming", 5), PropertyOrder(2)]
    [Description("Memory limit in megabytes for the terrain manager. 0 means no limitation. If the limit is exceeded the terrain manager is purged (not used sectors first)")]
    public int MemoryLimit
    {
      get { return _config.MemoryLimit; }
      set { _config.MemoryLimit = value; }
    }

    #endregion


  }

  #endregion

  #region Heightmap Properties

  [TypeConverter(typeof(TerrainPropertySorter))]
  public class HeightmapTerrainConfig : TerrainConfigCategoryBase
  {
    public HeightmapTerrainConfig(TerrainConfig configref) : base(configref)
    {
    }

    #region Height samples

    [SortedCategory("Heightmap", 1), PropertyOrder(10)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("Determines the overall number of height samples in x direction.")]
    public int HeightSamplesX
    {
      get { return SectorCountX * SamplesPerSectorX; }
      set 
      {
        int iNewSectors = value / SamplesPerSectorX;
        if (iNewSectors < 1)
          iNewSectors = 1;
        SectorCountX = iNewSectors; 
      }
    }

    [SortedCategory("Heightmap", 1), PropertyOrder(11)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("Determines the overall number of height samples in y direction.")]
    public int HeightSamplesY
    {
      get { return SectorCountY * SamplesPerSectorY; }
      set
      {
        int iNewSectors = value / SamplesPerSectorY;
        if (iNewSectors < 1)
          iNewSectors = 1;
        SectorCountY = iNewSectors;
      }
    }

    #endregion

    #region Terrain Size

    [SortedCategory("Terrain Size", 2), PropertyOrder(10)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("Determines the overall size of the terrain in x-direction (world units).")]
    public float TerrainSizeX
    {
      get { return (float)SectorCountX * SectorSizeX; }
      set
      {
        SectorSizeX = value / SectorCountX;
      }
    }

    [SortedCategory("Terrain Size", 2), PropertyOrder(11)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("Determines the overall size of the terrain in y-direction (world units).")]
    public float TerrainSizeY
    {
      get { return (float)SectorCountY * SectorSizeY; }
      set
      {
        SectorSizeY = value / SectorCountY;
      }
    }


    [SortedCategory("Terrain Size", 2), PropertyOrder(20)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("Determines the distance (in world units) between two heightmap samples.")]
    public float SampleSpacingX
    {
      get { return SectorSizeX/(float)SamplesPerSectorX; }
      set
      {
        SectorSizeX = value * (float)SamplesPerSectorX;
      }
    }

    [SortedCategory("Terrain Size", 2), PropertyOrder(21)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("Determines the distance (in world units) between two heightmap samples.")]
    public float SampleSpacingY
    {
      get { return SectorSizeY / (float)SamplesPerSectorY; }
      set
      {
        SectorSizeY = value * (float)SamplesPerSectorY;
      }
    }

    #endregion

    #region Sector Layout

    [SortedCategory("Sector Layout", 3), PropertyOrder(10)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("Determines the number of sector subdivisions the terrain consists of. Each sector is streamable.")]
    public int SectorCountX
    {
      get { return _config.SectorCountX; }
      set { _config.SectorCountX = value; }
    }

    [SortedCategory("Sector Layout", 3), PropertyOrder(11)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("Determines the number of sector subdivisions the terrain consists of. Each sector is streamable.")]
    public int SectorCountY
    {
      get { return _config.SectorCountY; }
      set { _config.SectorCountY = value; }
    }

    [SortedCategory("Sector Layout", 3), PropertyOrder(12)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("If the new terrain has more sectors then this value defines where to position the old one inside the new one")]
    public int CanvasOfsX
    {
      get { return _config.CanvasOfsX; }
      set { _config.CanvasOfsX = value; }
    }

    [SortedCategory("Sector Layout", 3), PropertyOrder(12)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("If the new terrain has more sectors then this value defines where to position the old one inside the new one")]
    public int CanvasOfsY
    {
      get { return _config.CanvasOfsY; }
      set { _config.CanvasOfsY = value; }
    }

    [SortedCategory("Sector Layout", 3), PropertyOrder(30)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("Specifies the number of heightmap samples per sector.")]
    public int SamplesPerSectorX
    {
      get { return _config.SamplesPerSectorX; }
      set { _config.SamplesPerSectorX = value; }
    }

    [SortedCategory("Sector Layout", 3), PropertyOrder(31)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("Specifies the number of heightmap samples per sector.")]
    public int SamplesPerSectorY
    {
      get { return _config.SamplesPerSectorY; }
      set { _config.SamplesPerSectorY = value; }
    }

    [SortedCategory("Sector Layout", 3), PropertyOrder(40)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("Determines the size of one sector in world units.")]
    public float SectorSizeX
    {
      get { return _config.SectorSizeX; }
      set { _config.SectorSizeX = value; }
    }

    [SortedCategory("Sector Layout", 3), PropertyOrder(41)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("Determines the size of one sector in world units.")]
    public float SectorSizeY
    {
      get { return _config.SectorSizeY; }
      set { _config.SectorSizeY = value; }
    }


    #endregion

    #region Tiles

    [SortedCategory("Tiles", 4), PropertyOrder(40)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("Specifies the subdivision of sectors into smaller tiles. The tiles define the granularity for possible holes.")]
    public int TilesPerSectorX
    {
      get { return _config.TilesPerSectorX; }
      set { _config.TilesPerSectorX = value; }
    }

    [SortedCategory("Tiles", 4), PropertyOrder(41)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("Specifies the subdivision of sectors into smaller tiles. The tiles define the granularity for possible holes.")]
    public int TilesPerSectorY
    {
      get { return _config.TilesPerSectorY; }
      set { _config.TilesPerSectorY = value; }
    }

    #endregion

  }

  #endregion

  #region Texturing Properties

  [TypeConverter(typeof(TerrainPropertySorter))]
  public class TexturingTerrainConfig : TerrainConfigCategoryBase
  {
    public TexturingTerrainConfig(TerrainConfig configref)
      : base(configref)
    {
    }

    #region Base Texture

    [SortedCategory("Base Texture", 1), PropertyOrder(1)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [EditorAttribute(typeof(AssetEditor), typeof(UITypeEditor)), AssetDialogFilter(new string[] { "Texture" })]
    [Description("Specifies the base texture. This texture can be changed later.")]
    public string BaseTextureFile
    {
      get { return _config.BaseTextureFile; }
      set { _config.BaseTextureFile = value; }
    }

    [SortedCategory("Base Texture", 1), PropertyOrder(2)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("Specifies the tiling factor of the base texture. Can be changed later.")]
    public float BaseTextureTilingX
    {
      get { return _config.BaseTextureTilingX; }
      set { _config.BaseTextureTilingX = value; }
    }

    [SortedCategory("Base Texture", 1), PropertyOrder(3)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("Specifies the tiling factor of the base texture. Can be changed later.")]
    public float BaseTextureTilingY
    {
      get { return _config.BaseTextureTilingY; }
      set { _config.BaseTextureTilingY = value; }
    }

    #endregion

    #region Normalmap

    [SortedCategory("Normalmap", 2), PropertyOrder(1)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("If enabled, filtering of the world space normalmap is simulated across sectors. This feature costs a bit more texture memory but avoids potential lighting seams at sector borders.")]
    public bool NormalmapUsesBorder
    {
      get { return _config.NormalmapUsesBorder; }
      set { _config.NormalmapUsesBorder = value;}
    }

    #endregion

    #region Weightmap

    [SortedCategory("Weightmap (per Sector)", 3), PropertyOrder(1)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("Specifies the resolution of a sector's weightmap texture.")]
    public int WeightmapResolutionX
    {
      get { return _config.DefaultWeightmapResolutionX; }
      set { _config.DefaultWeightmapResolutionX = value; }
    }

    [SortedCategory("Weightmap (per Sector)", 3), PropertyOrder(2)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("Specifies the resolution of a sector's weightmap texture.")]
    public int WeightmapResolutionY
    {
      get { return _config.DefaultWeightmapResolutionY; }
      set { _config.DefaultWeightmapResolutionY = value; }
    }


    [SortedCategory("Weightmap (Overall)", 4), PropertyOrder(1)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("The overall resolution of the weightmap.")]
    public int OverallResolutionX
    {
      get { return _config.SectorCountX * WeightmapResolutionX; }
      set 
      {
        WeightmapResolutionX = value / _config.SectorCountX;
      }
    }

    [SortedCategory("Weightmap (Overall)", 4), PropertyOrder(3)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("The overall resolution of the weightmap.")]
    public int OverallResolutionY
    {
      get { return _config.SectorCountY * WeightmapResolutionY; }
      set
      {
        WeightmapResolutionY = value / _config.SectorCountY;
      }
    }

    [SortedCategory("Weightmap (Overall)", 4), PropertyOrder(4)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("Memory footprint statistics")]
    public string MemoryUncompressed
    {
      get 
      {
        decimal iSamplesMB = (decimal)OverallResolutionX * (decimal)OverallResolutionY / (1024 * 1024);
        return string.Format("{0:0.00} MB", iSamplesMB * 4, iSamplesMB * 4); 
      }
    }

    [SortedCategory("Weightmap (Overall)", 4), PropertyOrder(5)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("Memory footprint statistics")]
    public string MemoryDXT5
    {
      get
      {
        decimal iSamplesMB = (decimal)OverallResolutionX * (decimal)OverallResolutionY / (1024 * 1024);
        return string.Format("{0:0.00} MB", iSamplesMB);
      }
    }

    [SortedCategory("Materialmap (per Sector)", 5), PropertyOrder(1)]
    [Description("If larger than zero, the terrain system builds a material map (per sector) that stores the most relevant texture ID so it can be queried by the runtime")]
    public int MaterialIDMapResolutionX
    {
      get { return _config.MaterialIDMapResolutionX; }
      set { _config.MaterialIDMapResolutionX = value; }
    }

    [SortedCategory("Materialmap (per Sector)", 5), PropertyOrder(2)]
    [Description("If larger than zero, the terrain system builds a material map (per sector) that stores the most relevant texture ID so it can be queried by the runtime")]
    public int MaterialIDMapResolutionY
    {
      get { return _config.MaterialIDMapResolutionY; }
      set { _config.MaterialIDMapResolutionY = value; }
    }

    #endregion

  }

  #endregion

  #region Vegetation Properties

  [TypeConverter(typeof(TerrainPropertySorter))]
  public class VegetationTerrainConfig : TerrainConfigCategoryBase
  {
    public VegetationTerrainConfig(TerrainConfig configref)
      : base(configref)
    {
    }

    #region Vegetation

    [SortedCategory("Vegetation Editing", 1), PropertyOrder(1)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("Specifies the resolution (per sector) for editing the vegetation density map (only relevant inside the editor).")]
    public int VegetationMapResolutionX
    {
      get { return _config.VegetationMapResolutionX; }
      set { _config.VegetationMapResolutionX = value; }
    }

    [SortedCategory("Vegetation Editing", 1), PropertyOrder(2)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [Description("Specifies the resolution (per sector) for editing the vegetation density map (only relevant inside the editor).")]
    public int VegetationMapResolutionY
    {
      get { return _config.VegetationMapResolutionY; }
      set { _config.VegetationMapResolutionY = value; }
    }

    #endregion
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
