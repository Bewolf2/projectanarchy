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
using TerrainManaged;
using System.Runtime.Serialization;
using CSharpFramework;
using TerrainEditorPlugin.Shapes;
using System.ComponentModel;
using CSharpFramework.PropertyEditors;
using CSharpFramework.Serialization;
using System.IO;
using System.Windows.Forms;
using System.Xml.Serialization;
using CSharpFramework.BaseTypes;

namespace TerrainEditorPlugin.Filter
{
  [Serializable]
  public class TerrainMeshExport : TerrainMeshExportSettings, ISerializable, IDeserializationCallback, ICloneable
  {
    public TerrainMeshExport()
    {
      Owner = null;
    }

    public TerrainMeshExport(TerrainShape owner)
    {
      Owner = owner;
    }


    #region Additional Members

    [XmlIgnore()]
    public TerrainShape Owner;
    private ProfileEnum _exportForProfile = new ProfileEnum();


    #endregion

    #region Editable Properties

    [Description("If enabled, automatic terrain mesh export is enabled when exporting the scene. The exported scene contains one static mesh per sector. These static meshes are encapsulated in zones to enable streaming functionality.")]
    [SortedCategory("Export", 1), PropertyOrder(0)]
    [XmlIgnore()]
    public bool AutomaticExport
    {
      get { return _bAutomaticExport; }
      set { _bAutomaticExport = value; }
    }

    [SortedCategory("Export", 1), PropertyOrder(1)]
    [Description("Determines for which asset profiles the export is done."), DefaultValue("All Profiles")]
    public ProfileEnum ExportForAssetProfile
    {
      get { return _exportForProfile; }
      set { _exportForProfile = value; }
    }

    [Description("If enabled, a mesh optimizer tool post-processes the mesh.")]
    [SortedCategory("Mesh generation", 1), PropertyOrder(11)]
    [XmlIgnore()]
    public bool SimplifyMesh
    {
      get { return _bSimplifyMesh; }
      set { _bSimplifyMesh = value; }
    }

    [Description("If enabled, the mesh optimizer tool uses the production simplifier which produces better simplification results. Note, that by using the production simplifier the time of processing will increase.")]
    [SortedCategory("Mesh generation", 1), PropertyOrder(12)]
    [XmlIgnore()]
    public bool ProductionSimplifier
    {
      get { return _bUseLegacySolver; }
      set { _bUseLegacySolver = value; }
    }

    [Description("Defines the texture resolution used during baking of the detailed textures. This value defines the resolution per terrain sector.")]
    [SortedCategory("Mesh generation", 1), PropertyOrder(13)]
    [TypeConverter(typeof(IntListTypeConverter)), AllowedIntValues(new int[] { 32, 64, 128, 256, 512, 1024, 2048})]
    [XmlIgnore()]
    public int TextureResolution
    {
      get { return _iTextureResolution; }
      set
      {
        _iTextureResolution = value;
        if (_iTextureResolution < 32)
          _iTextureResolution = 32;
        else if (_iTextureResolution > 2048)
          _iTextureResolution = 2048;
      }
    }

    [Description("Defines the maximum allowed distance (in world units) of a vertex in the optimized mesh to the original/unoptimized surface.")]
    [SortedCategory("Mesh generation", 1), PropertyOrder(14)]
    [XmlIgnore()]
    public float MaxErrorDistance
    {
      get { return _fMaxErrorDistance; }
      set
      {
        _fMaxErrorDistance = value;
      }
    }

    [Description("If enabled, static lighting information from lightmaps is baked into the sector's unique diffuse texture. If lightmap baking is disabled, only the diffuse terrain textures are baked into the sector's unique diffuse texture, and each exported mesh does include lightmap texture coordinates, normals, and tangents. Furthermore, when automatic export is enabled the scene's lightmaps are automatically assigned to the exported meshes.")]
    [SortedCategory("Mesh generation", 1), PropertyOrder(15)]
    [XmlIgnore()]
    public bool BakedLightmaps
    {
      get { return _bBakedLightmaps; }
      set { _bBakedLightmaps = value; }
    }

    [Description("Absolute distance (in world units) around the bounding box at which the zone is guaranteed to be loaded by the engine. Can be -1 to always use the view distance. Displayed as red border.")]
    [SortedCategory("Mesh streaming", 2), PropertyOrder(10), DefaultValue(-1f)]
    public float LoadedDistance
    {
      get { return _fLoadedDistance; }
      set
      {
        _fLoadedDistance = value;
        if (_fLoadedDistance < 0.0f)
          _fLoadedDistance = -1.0f;
      }
    }

    [Description("Margin width (measured in world units) around the loaded distance at which the engine starts streaming in the zone when the camera approaches. Displayed as yellow border.")]
    [SortedCategory("Mesh streaming", 2), PropertyOrder(11), DefaultValue(1000f)]
    public float CacheInMargin
    {
      get { return _fCacheInMargin; }
      set
      {
        if (value < 1.0f)
        {
          EditorManager.ShowMessageBox("The margin must be a positive value.", "Invalid value", MessageBoxButtons.OK, MessageBoxIcon.Error);
          return;
        }
        _fCacheInMargin = value;
      }
    }

    [Description("Margin width (measured in world units) around the cache in distance at which the zone is unloaded by the engine when the camera is further away. Displayed as green border.")]
    [SortedCategory("Mesh streaming", 2), PropertyOrder(12), DefaultValue(1000f)]
    public float CacheOutMargin
    {
      get { return _fCacheOutMargin; }
      set
      {
        if (value < 1.0f)
        {
          EditorManager.ShowMessageBox("The margin must be a positive value.", "Invalid value", MessageBoxButtons.OK, MessageBoxIcon.Error);
          return;
        }
        _fCacheOutMargin = value;
      }
    }

    #endregion

    #region Statistics Properties

    [Description("Defines the number of output height samples")]
    [SortedCategory("Statistics", 2), PropertyOrder(1)]
    public int HeightSamplesX
    {
      get 
      { 
        return Owner.Config.OverallHeightSampleCount.Width; 
      }
    }

    [Description("Defines the number of output height samples")]
    [SortedCategory("Statistics", 2), PropertyOrder(2)]
    public int HeightSamplesY
    {
      get
      {
        return Owner.Config.OverallHeightSampleCount.Height;
      }
    }

    #endregion

    #region ISerializable Members

    protected TerrainMeshExport(SerializationInfo info, StreamingContext context)
    {
      if (SerializationHelper.HasElement(info, "bAutomaticExport"))
        _bAutomaticExport = info.GetBoolean("bAutomaticExport");
      if (SerializationHelper.HasElement(info, "bBakedLightmaps"))
        _bBakedLightmaps = info.GetBoolean("bBakedLightmaps");
      if (SerializationHelper.HasElement(info, "bOptimizeMesh"))
        _bSimplifyMesh = info.GetBoolean("bOptimizeMesh");
      if (SerializationHelper.HasElement(info, "bUseLegacySolver"))
        _bUseLegacySolver = info.GetBoolean("bUseLegacySolver");
      if (SerializationHelper.HasElement(info, "iTextureResolution"))
        _iTextureResolution = info.GetInt32("iTextureResolution");
      if (SerializationHelper.HasElement(info, "fMaxErrorDistance"))
        _fMaxErrorDistance = info.GetSingle("fMaxErrorDistance");

      if (SerializationHelper.HasElement(info, "bLoadedDistance"))
        _fLoadedDistance = info.GetSingle("bLoadedDistance");
      if (SerializationHelper.HasElement(info, "fCacheInMargin"))
        _fCacheInMargin = info.GetSingle("fCacheInMargin");
      if (SerializationHelper.HasElement(info, "fCacheOutMargin"))
        _fCacheOutMargin = info.GetSingle("fCacheOutMargin");
    }

    public void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      info.AddValue("bAutomaticExport", _bAutomaticExport);
      info.AddValue("bBakedLightmaps", _bBakedLightmaps);
      info.AddValue("bOptimizeMesh", _bSimplifyMesh);
      info.AddValue("bUseLegacySolver", _bUseLegacySolver);
      info.AddValue("iTextureResolution", _iTextureResolution);
      info.AddValue("fMaxErrorDistance", _fMaxErrorDistance);

      info.AddValue("fLoadedDistance", _fLoadedDistance);
      info.AddValue("fCacheInMargin", _fCacheInMargin);
      info.AddValue("fCacheOutMargin", _fCacheOutMargin);
    }

    #endregion

    #region IDeserializationCallback Members

    public void OnDeserialization(object sender)
    {
      
    }

    #endregion

    #region ICloneable Members

    public object Clone()
    {
      TerrainMeshExport copy = (TerrainMeshExport)this.MemberwiseClone();
      return copy;
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
