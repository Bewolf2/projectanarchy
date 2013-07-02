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
using System.ComponentModel;
using CSharpFramework.PropertyEditors;
using System.Drawing.Design;
using System.ComponentModel.Design;
using System.Runtime.Serialization;
using CSharpFramework.Serialization;
using CSharpFramework.Scene;
using CSharpFramework.Shapes;
using CSharpFramework;
using System.Collections;
using System.Reflection;
using CSharpFramework.UndoRedo;
using System.IO;
using System.Windows.Forms;

namespace TerrainBase.Editing
{
  /// <summary>
  /// Decoration model (C# representation of VTerrainDecorationModel terrain class)
  /// </summary>
  //[TypeConverter(typeof(UndoableObjectConverter))]
  [Serializable()]
  public class DecorationModelResource : ISerializable, ICloneable, ISceneObject, IDeserializationCallback
  {
    /// <summary>
    /// Must be compatible with ModelFlags_e in native class
    /// </summary>
    [Flags]
    public enum ModelFlags_e
    {
      None            = 0x00000000,
      Aligned         = 0x00000000,
      AlwaysVertical  = 0x00000001,
      ShearedAligned  = 0x00000002,

      CastLightmapShadows = 0x00000100,
      CastDynamicShadows = 0x00000200,
    }

    public enum TerrainAlignment
    {
      AlwaysVertical,
      Aligned,
      ShearedAligned
    }

    public DecorationModelResource(Shape3D terrain, int iID)
    {
      _ownerTerrain = terrain;
      ID = iID;
    }

    public bool CopyPropertiesFrom(DecorationModelResource other, bool bTriggerChangedEvent)
    {
      bool bAnyChanged = false;
      if (_Name != other._Name) { _Name = other._Name; bAnyChanged = true; }
      if (_ExportFilename != other._ExportFilename) { _ExportFilename = other._ExportFilename; bAnyChanged = true; }
      if (_Filename != other._Filename) { _Filename = other._Filename; bAnyChanged = true; }
      if (_FarClipDistance != other._FarClipDistance) { _FarClipDistance = other._FarClipDistance; bAnyChanged = true; }
      if (_NearClipDistance != other._NearClipDistance) { _NearClipDistance = other._NearClipDistance; bAnyChanged = true; }
      if (_fRelFadeStart != other._fRelFadeStart) { _fRelFadeStart = other._fRelFadeStart; bAnyChanged = true; }
      if (_visibleBitmask != other._visibleBitmask) { _visibleBitmask = other._visibleBitmask; bAnyChanged = true; }
      if (_averageScale != other._averageScale) { _averageScale = other._averageScale; bAnyChanged = true; }
      if (_sizeVariation != other._sizeVariation) { _sizeVariation = other._sizeVariation; bAnyChanged = true; }
      if (_applyWind != other._applyWind) { _applyWind = other._applyWind; bAnyChanged = true; }
      if (_applyConstraint != other._applyConstraint) { _applyConstraint = other._applyConstraint; bAnyChanged = true; }
      if (_collisionCapsuleRadius != other._collisionCapsuleRadius) { _collisionCapsuleRadius = other._collisionCapsuleRadius; bAnyChanged = true; }
      if (_castLightmapShadows != other._castLightmapShadows) { _castLightmapShadows = other._castLightmapShadows; bAnyChanged = true; }
      if (_castDynamicShadows != other._castDynamicShadows) { _castDynamicShadows = other._castDynamicShadows; bAnyChanged = true; }
      if (_alignment != other._alignment) { _alignment = other._alignment; bAnyChanged = true; }
      if (_density != other._density) { _density = other._density; bAnyChanged = true; }
      if (_randomness != other._randomness) { _randomness = other._randomness; bAnyChanged = true; }
      if (_pivotHeight != other._pivotHeight) { _pivotHeight = other._pivotHeight; bAnyChanged = true; }
      if (_customLightmapCasterMesh != other._customLightmapCasterMesh) { _customLightmapCasterMesh = other._customLightmapCasterMesh; bAnyChanged = true; }

      if (bTriggerChangedEvent && bAnyChanged)
        TriggerPropertyChangedEvent();
      return bAnyChanged;
    }

    public void AddPluginNames(ArrayList uniqueNames)
    {
      string plugin = "VisionEnginePlugin";

      if (string.Compare(System.IO.Path.GetExtension(_Filename), ".srt", true) == 0)
        plugin = "Speedtree5EnginePlugin";

      if (!uniqueNames.Contains(plugin))
        uniqueNames.Add(plugin);
    }

    #region Update Event

    public static event EventHandler OnDecorationModelResourcePropertyChanged = null;
    public static event EventHandler OnDecorationModelResourceNameChanged = null;

    void TriggerPropertyChangedEvent()
    {
      if (_ownerTerrain == null)
        return;

      Modified = true;
      if (OnDecorationModelResourcePropertyChanged != null)
        OnDecorationModelResourcePropertyChanged(this, EventArgs.Empty);

    }

    #endregion

    #region Sector range


    public void AddAffectedSector(int x, int y)
    {
      bool bChanged = false;
      if (x < _iSectorX1) { _iSectorX1 = x; bChanged = true; }
      if (y < _iSectorY1) { _iSectorY1 = y; bChanged = true; }
      if (x > _iSectorX2) { _iSectorX2 = x; bChanged = true; }
      if (y > _iSectorY2) { _iSectorY2 = y; bChanged = true; }
      if (bChanged)
        this.Modified = true;
    }

    public void ResetAffectedSectors()
    {
      _iSectorX1 = 1000000;
      _iSectorY1 = 1000000;
      _iSectorX2 = -1000000;
      _iSectorY2 = -1000000;
    }

    #endregion

    #region members and properties

    public Shape3D _ownerTerrain;

    string _Name;
    string _ExportFilename;
    string _Filename;
    public int ID = 0;
    float _FarClipDistance = 1000;
    float _NearClipDistance = 0;
    float _fRelFadeStart = 0.8f;
    float _averageScale = 1.0f;
    float _sizeVariation = 10.0f;

    float _applyWind = 50.0f;
    float _applyConstraint = 0.0f;
    float _collisionCapsuleRadius = 0.0f;
    public float SuggestedCapsuleRadius = 0.0f;

    bool _castLightmapShadows = false;
    bool _castDynamicShadows = false;
    string _customLightmapCasterMesh = null;

    TerrainAlignment _alignment = TerrainAlignment.AlwaysVertical;
    float _density = 0.8f;
    float _randomness = 1.0f;
    float _pivotHeight = 0.95f;
    FlagsInt32_e _visibleBitmask = FlagsInt32_e.All;

    public int iDensityResX = 32;
    public int iDensityResY = 32;

    // affected sector range
    public int _iSectorX1 = 1000000;
    public int _iSectorY1 = 1000000;
    public int _iSectorX2 = -1000000;
    public int _iSectorY2 = -1000000;

    [SortedCategory("Decoration model", 1), PropertyOrder(0)]
    [Description("Name of this decoration type. Ignored by the engine.")]
    public string Name
    {
      get { return _Name; }
      set
      {
        _Name = value; 
        Modified = true; // no need to trigger event
        if (OnDecorationModelResourceNameChanged != null)
          OnDecorationModelResourceNameChanged(this, EventArgs.Empty);
      }
    }

    [SortedCategory("Decoration model", 1),PropertyOrder(1)]
    [Description("Filename of the render model.")]
    [EditorAttribute(typeof(FilenameEditor), typeof(UITypeEditor)), FileDialogFilter(new string[] {".model",".srt",".atlas"})]
    [ThumbnailFormat(new string[] { @"{0}_data\Thumbnail.bmp", @"{0}_data\Thumbnail.jpg" })]
    [RefreshProperties(RefreshProperties.All)]
    public string Filename
    {
      get { return _Filename; }
      set 
      {
        // test whether we may set this value. This limitation is specifically for speedtree models, not other types
        if (_ownerTerrain != null && !string.IsNullOrEmpty(value) && string.Compare(Path.GetExtension(value),".srt")==0)
        {
          // unfortunately we have no access to terrain shape members directly, so go via reflection
          PropertyInfo desc = _ownerTerrain.GetType().GetProperty("DecorationModels");
          System.Diagnostics.Debug.Assert(desc.PropertyType==typeof(ArrayList));
          ArrayList models = (ArrayList)desc.GetValue(_ownerTerrain, null);
          if (models != null)
          {
            foreach (DecorationModelResource model in models)
              if (string.Compare(model.Filename, value, true) == 0)
              {
                EditorManager.ShowMessageBox(string.Format("The vegetation list already contains a Speedtree model of file '{0}'.\n\nSpeedtree model files must be unique in this list", value),"Cannot use this Speedtree file", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return; // don't set
              }
          }
        }

        _Filename = value; 
        ViewObject = null;  // invalidate
        TriggerPropertyChangedEvent(); 
      }
    }


    [RefreshProperties(RefreshProperties.All)]
    [SortedCategory("Decoration model", 1), PropertyOrder(2)]
    [Description("Determines how the model is aligned on the terrain")]
    public TerrainAlignment ModelAlignment
    {
      get { return _alignment; }
      set { _alignment = value; TriggerPropertyChangedEvent(); }
    }

    [SortedCategory("Decoration model", 1), PropertyOrder(3)]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.0f, 1.0f, 100)]
    [Description("Relative height of the pivot. 0.0=model pivot, 1.0=bottom of bounding box")]
    public float PivotHeight
    {
      get { return _pivotHeight; }
      set
      {
        _pivotHeight = value;
        if (_pivotHeight < 0.0f)
          _pivotHeight = 0.0f;
        if (_pivotHeight > 1.0f)
          _pivotHeight = 1.0f;
        TriggerPropertyChangedEvent();
      }
    }

    [SortedCategory("Decoration model", 1), PropertyOrder(4)]
    [Description("Distance in world units at which this model type is clipped.")]
    public float NearClipDistance
    {
      get { return _NearClipDistance; }
      set { _NearClipDistance = value; TriggerPropertyChangedEvent(); }
    }

    [SortedCategory("Decoration model", 1), PropertyOrder(5)]
    [Description("Distance in world units at which this model type is clipped.")]
    public float FarClipDistance
    {
      get { return _FarClipDistance; }
      set { _FarClipDistance = value; TriggerPropertyChangedEvent(); }
    }


    [SortedCategory("Decoration model", 1), PropertyOrder(10)]
    [Description("Render filter bitmask for filering this decoration type in render contexts (e.g. mirror)")]
    public FlagsInt32_e VisibleBitmask
    {
      get { return _visibleBitmask; }
      set { _visibleBitmask = value; TriggerPropertyChangedEvent(); }
    }

    public void SetFarClipDistanceInternal(float fValue)
    {
      _FarClipDistance = fValue;
    }

    public void SetClipDistancesInternal(float fNear, float fFar, float fFadeStart)
    {
      _fRelFadeStart = fFadeStart;
      _NearClipDistance = fNear;
      _FarClipDistance = fFar;
    }

    [SortedCategory("Decoration model", 1), PropertyOrder(11)]
    [Description("If enabled, this vegetation type is considered as shadow caster for static light computation. Should only be used for larger models. Speedtrees only cast shadows if they have a horizontal billboard.")]
    public bool CastLightmapShadows
    {
      get { return _castLightmapShadows; }
      set { _castLightmapShadows = value; TriggerPropertyChangedEvent(); }
    }

    [SortedCategory("Decoration model", 1), PropertyOrder(12)]
    [Description("If enabled, this vegetation type casts dynamic shadows (e.g. in the deferred system). Currently only supported for Speedtree files")]
    public bool CastDynamicShadows
    {
      get { return _castDynamicShadows; }
      set { _castDynamicShadows = value; TriggerPropertyChangedEvent(); }
    }

    [SortedCategory("Decoration model", 1), PropertyOrder(15)]
    [Description("The capsule radius (in units) for collisions. The capsules are sacaled with every instance, so this radius defines the radius for a scaling of 1.0. Specify 0.0 for no collision.")]
    public float CollisionCapsuleRadius
    {
      get { return _collisionCapsuleRadius; }
      set 
      { 
        _collisionCapsuleRadius = value;
        if (_collisionCapsuleRadius < 0.0f)
          _collisionCapsuleRadius = 0.0f;
        TriggerPropertyChangedEvent(); 
      }
    }

    [SortedCategory("Decoration model", 1), PropertyOrder(20)]
    public string CustomLightmapCasterMesh
    {
      get { return _customLightmapCasterMesh; }
      set 
      { 
        _customLightmapCasterMesh = value; 
        TriggerPropertyChangedEvent(); // a full vegetation update would not be necessary though
      }
    }

    [SortedCategory("Size and distribution", 2), PropertyOrder(1)]
    [Description("Average scaling factor for the model. 1.0 uses original size of the model")]
    [RefreshProperties(RefreshProperties.All)]
    public float AverageScale
    {
      get { return _averageScale; }
      set
      {
        _averageScale = value;
        if (_averageScale < 0.001f)
          _averageScale = 0.001f;
        TriggerPropertyChangedEvent();
      }
    }


    [SortedCategory("Size and distribution", 2), PropertyOrder(2)]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.0f, 100.0f, 100)]
    [Description("Variation of the average size (percentage)")]
    [RefreshProperties(RefreshProperties.All)]
    public float SizeVariation
    {
      get { return _sizeVariation; }
      set
      {
        _sizeVariation = value;
        if (_sizeVariation < 0.0f)
          _sizeVariation = 0.0f;
        if (_sizeVariation > 100.0f)
          _sizeVariation = 100.0f;
        TriggerPropertyChangedEvent();
      }
    }

    [SortedCategory("Size and distribution", 2), PropertyOrder(3)]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.0f, 2.0f, 200)]
    [Description("Relative density factor")]
    public float Density
    {
      get { return _density; }
      set 
      { 
        _density = value;
        if (_density < 0.0f)
          _density = 0.0f;
        if (_density > 2.0f)
          _density = 2.0f;
        TriggerPropertyChangedEvent();
      }
    }

    [SortedCategory("Size and distribution", 2), PropertyOrder(4)]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.0f, 1.0f, 100)]
    [Description("The distribution randomness of the instances")]
    public float DistributionRandomness
    {
      get { return _randomness; }
      set 
      { 
        _randomness = value;
        if (_randomness < 0.0f)
          _randomness = 0.0f;
        if (_randomness > 1.0f)
          _randomness = 1.0f;
        TriggerPropertyChangedEvent();
      }
    }

    
    [SortedCategory("Shader properties", 3), PropertyOrder(1)]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.0f, 200.0f, 200)]
    [Description("Percentage of wind modifier applied to this type")]
    public float ApplyWind
    {
      get { return _applyWind; }
      set 
      { 
        _applyWind = value;
        if (_applyWind < 0.0f)
          _applyWind = 0.0f;
        if (_applyWind > 200.0f)
          _applyWind = 200.0f;
        TriggerPropertyChangedEvent();
      }
    }


    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRangeAttribute(0.0f, 1.0f, 100)]
    [SortedCategory("Shader properties", 3), PropertyOrder(2)]
    [Description("Distance relative to far clip distance at which fading out starts. Default is 0.8 which means it starts fading out at 80% of the far clip distance")]
    public float RelativeFadeOutStart
    {
      get { return _fRelFadeStart; }
      set
      {
        _fRelFadeStart = value;
        if (_fRelFadeStart < 0.0f)
          _fRelFadeStart = 0.0f;
        else if (_fRelFadeStart > 1.0f)
          _fRelFadeStart = 1.0f;
        TriggerPropertyChangedEvent();
      }
    }


    [SortedCategory("Shader properties", 3), PropertyOrder(3)]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.0f, 200.0f, 200)]
    [Description("Percentage of constraint modifier applied to this type")]
    public float ApplyConstraint
    {
      get { return _applyConstraint; }
      set
      {
        _applyConstraint = value;
        if (_applyConstraint < 0.0f)
          _applyConstraint = 0.0f;
        if (_applyConstraint > 200.0f)
          _applyConstraint = 200.0f;
        TriggerPropertyChangedEvent();
      }
    }

    [SortedCategory("Export as luminance", 5), PropertyOrder(10)]
    [Description("The density map can be exported as a luminance texture. A filename can be selected here, but the export has to be triggerd via context menu")]
    [EditorAttribute(typeof(FilenameCreator), typeof(UITypeEditor)), FileDialogFilter(new string[] { ".dds" }), FileCreateDialogAllowOverwrite(true)]
    public string ExportFilename
    {
      get
      {
        return _ExportFilename;
      }
      set
      {
        if (_ExportFilename == value)
          return;

        _ExportFilename = value;
        Modified = true;
      }
    }

    [Browsable(false)]
    public bool HasExportFilename
    {
      get
      {
        return !string.IsNullOrEmpty(_ExportFilename);
      }
    }

    #endregion

    #region Internal Properties

    [Browsable(false)]
    [Description("Flags for positioning the model.")]
    [EditorAttribute(typeof(FlagsEditor), typeof(UITypeEditor))]
    public ModelFlags_e ModelFlags
    {
      get
      {
        ModelFlags_e flags = ModelFlags_e.None;
        switch (_alignment)
        {
          case TerrainAlignment.AlwaysVertical: flags |= ModelFlags_e.AlwaysVertical; break;
          case TerrainAlignment.ShearedAligned: flags |= ModelFlags_e.ShearedAligned; break;
          case TerrainAlignment.Aligned: flags |= ModelFlags_e.Aligned; break;
        }
        if (this.CastLightmapShadows)
          flags |= ModelFlags_e.CastLightmapShadows;
        if (this.CastDynamicShadows)
          flags |= ModelFlags_e.CastDynamicShadows;
        return flags;
      }
    }

    #endregion

    public override string ToString()
    {
      if (!string.IsNullOrEmpty(_Name))
        return _Name;
      if (!string.IsNullOrEmpty(_Filename))
        return _Filename;
      return "<New decoration Type>";
    }

    #region LOD

    public DecorationModelResource _LODOrigin = null;
    public List<DecorationModelResource> _LODVersions = null;

    public void AddLODVersion(DecorationModelResource res)
    {
      if (_LODVersions == null)
        _LODVersions = new List<DecorationModelResource>();
      if (!_LODVersions.Contains(res))
        _LODVersions.Add(res);
      res._LODOrigin = this;
    }

    public void SafeRemoveFromLODParent()
    {
      if (_LODOrigin!=null && _LODOrigin._LODVersions!=null && _LODOrigin._LODVersions.Contains(this))
        _LODOrigin._LODVersions.Remove(this);
    }

    #endregion

    #region ISerializable Members


    DecorationModelResource(SerializationInfo info, StreamingContext context)
    {
      _Name = info.GetString("_Name");
      _Filename = info.GetString("_Filename");
      ID = info.GetInt32("ID");
      if (SerializationHelper.HasElement(info, "_NearClipDistance"))
        _NearClipDistance = info.GetSingle("_NearClipDistance");
      if (SerializationHelper.HasElement(info, "_FarClipDistance"))
        _FarClipDistance = info.GetSingle("_FarClipDistance");
      if (SerializationHelper.HasElement(info, "_fRelFadeStart"))
        _fRelFadeStart = info.GetSingle("_fRelFadeStart");
      if (SerializationHelper.HasElement(info, "_visibleBitmask"))
        _visibleBitmask = (FlagsInt32_e)info.GetValue("_visibleBitmask", typeof(FlagsInt32_e));
      if (SerializationHelper.HasElement(info, "_alignment"))
        _alignment = (TerrainAlignment)info.GetValue("_alignment", typeof(TerrainAlignment));
      if (SerializationHelper.HasElement(info, "_sizeVariation"))
        _sizeVariation = info.GetSingle("_sizeVariation");
      if (SerializationHelper.HasElement(info, "_averageScale"))
        _averageScale = info.GetSingle("_averageScale");
      if (SerializationHelper.HasElement(info, "_density"))
        _density = info.GetSingle("_density");
      if (SerializationHelper.HasElement(info, "_randomness"))
        _randomness = info.GetSingle("_randomness");
      if (SerializationHelper.HasElement(info, "_pivotHeight"))
        _pivotHeight = info.GetSingle("_pivotHeight");
      if (SerializationHelper.HasElement(info, "_customLightmapCasterMesh"))
        _customLightmapCasterMesh = info.GetString("_customLightmapCasterMesh");
      if (SerializationHelper.HasElement(info, "_applyWind"))
        _applyWind = info.GetSingle("_applyWind");
      if (SerializationHelper.HasElement(info, "_applyConstraint"))
        _applyConstraint = info.GetSingle("_applyConstraint");
      if (SerializationHelper.HasElement(info, "_collisionCapsuleRadius"))
        _collisionCapsuleRadius = info.GetSingle("_collisionCapsuleRadius");
      if (SerializationHelper.HasElement(info, "_castLightmapShadows"))
        _castLightmapShadows = info.GetBoolean("_castLightmapShadows");
      if (SerializationHelper.HasElement(info, "_castDynamicShadows"))
        _castDynamicShadows = info.GetBoolean("_castDynamicShadows");
      if (SerializationHelper.HasElement(info, "_ExportFilename"))
        _ExportFilename = info.GetString("_ExportFilename");
      if (SerializationHelper.HasElement(info, "_iSectorX1"))
      {
        _iSectorX1 = info.GetInt32("_iSectorX1");
        _iSectorY1 = info.GetInt32("_iSectorY1");
        _iSectorX2 = info.GetInt32("_iSectorX2");
        _iSectorY2 = info.GetInt32("_iSectorY2");
      }
      if (SerializationHelper.HasElement(info, "_LODOrigin"))
        _LODOrigin = (DecorationModelResource)info.GetValue("_LODOrigin", typeof(DecorationModelResource));
    }

    public void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      info.AddValue("_Name", _Name);
      info.AddValue("_ExportFilename", _ExportFilename);
      info.AddValue("_Filename", _Filename);
      info.AddValue("ID", ID);
      info.AddValue("_NearClipDistance", _NearClipDistance);
      info.AddValue("_FarClipDistance", _FarClipDistance);
      info.AddValue("_fRelFadeStart", _fRelFadeStart);
      info.AddValue("_visibleBitmask", _visibleBitmask);
      info.AddValue("_alignment", _alignment);
      info.AddValue("_sizeVariation", _sizeVariation);
      info.AddValue("_averageScale", _averageScale);
      info.AddValue("_density", _density);
      info.AddValue("_randomness", _randomness);
      info.AddValue("_pivotHeight", _pivotHeight);
      info.AddValue("_customLightmapCasterMesh", _customLightmapCasterMesh);
      info.AddValue("_applyWind", _applyWind);
      info.AddValue("_applyConstraint", _applyConstraint);
      info.AddValue("_collisionCapsuleRadius", _collisionCapsuleRadius);
      info.AddValue("_castLightmapShadows", _castLightmapShadows);
      info.AddValue("_castDynamicShadows", _castDynamicShadows);

      info.AddValue("_iSectorX1", _iSectorX1);
      info.AddValue("_iSectorY1", _iSectorY1);
      info.AddValue("_iSectorX2", _iSectorX2);
      info.AddValue("_iSectorY2", _iSectorY2);

      info.AddValue("_LODOrigin", _LODOrigin);
    }

    #endregion

    #region IDeserializationCallback Members

    public void OnDeserialization(object sender)
    {
      if (_LODOrigin != null)
        _LODOrigin.AddLODVersion(this);
    }

    #endregion
    #region ICloneable Members

    public object Clone()
    {
      DecorationModelResource copy = (DecorationModelResource)this.MemberwiseClone();
      copy._viewObject = null;
      return copy;
    }

    #endregion

    #region ISceneObject Members

    [Browsable(false)]
    public bool Modifiable
    {
      get
      {
        if (_ownerTerrain == null)
          return true;
        return _ownerTerrain.Modifiable;
      }
    }

    [Browsable(false)]
    public bool Modified
    {
      set
      {
        if (_ownerTerrain != null)
          _ownerTerrain.Modified = value;
      }
    }

    #endregion

    #region View Object

    IDecorationResourcePGView _viewObject = null;

    [Browsable(false)]
    public IDecorationResourcePGView ViewObject
    {
      get 
      {
        if (_viewObject==null)
        {
          string fileExt = Path.GetExtension(Filename);
          if (this._LODOrigin != null)
          {
            _viewObject = new IDecorationResourcePGView_LOD(this);
          } 
          else if (string.IsNullOrEmpty(fileExt))
          {
            _viewObject = new IDecorationResourcePGView_Basic(this);
          }
          else if (string.Compare(fileExt, ".model", true) == 0 || string.Compare(fileExt, ".vmesh", true) == 0)
          {
            _viewObject = new IDecorationResourcePGView_Model(this);
          }
          else if (string.Compare(fileExt, ".srt", true) == 0)
          {
            _viewObject = new IDecorationResourcePGView_Speedtree(this);
          }
          else if (string.Compare(fileExt, ".atlas", true) == 0)
          {
            _viewObject = new IDecorationResourcePGView_Billboards(this);
          }
        }
        return _viewObject; 
      }
      set { _viewObject = value; }
    }

    #endregion

  }


  #region PropertyGrid view objects

  public class IDecorationResourcePGView : ICloneable
  {
    public IDecorationResourcePGView(DecorationModelResource owner)
    {
      _owner = owner;
    }
    protected DecorationModelResource _owner;

    #region ICloneable Members

    /// <summary>
    ///  We need the ICloneable interface only for the property grid dialog, although we are using the no-clone setter
    /// </summary>
    /// <returns></returns>
    public object Clone()
    {
      IDecorationResourcePGView copy = (IDecorationResourcePGView)this.MemberwiseClone();
      return copy;
    }

    #endregion
  }

  #region Basics

  public class IDecorationResourcePGView_Basic : IDecorationResourcePGView
  {
    public IDecorationResourcePGView_Basic(DecorationModelResource owner)
      : base(owner)
    {
    }

    [SortedCategory("Resource File", 0), PropertyOrder(0)]
    [Description("Decoration resource type (determined by file extension)")]
    public virtual string ResourceType
    {
      get { return "<none>"; }
    }

    [SortedCategory("Resource File", 0), PropertyOrder(1)]
    [Description("Name of this decoration type. Ignored by the engine.")]
    public string Name
    {
      get { return _owner.Name; }
      set { _owner.Name = value; }
    }

    [SortedCategory("Resource File", 0), PropertyOrder(2)]
    [Description("Filename of the render model.")]
    [EditorAttribute(typeof(FilenameEditor), typeof(UITypeEditor)), FileDialogFilter(new string[] { ".model", ".srt", ".atlas" })]
    [ThumbnailFormat(new string[] { @"{0}_data\Thumbnail.bmp", @"{0}_data\Thumbnail.jpg" })]
    [RefreshProperties(RefreshProperties.All)]
    public string Filename
    {
      get { return _owner.Filename; }
      set { _owner.Filename = value; }
    }




    [SortedCategory("Decoration model", 1), PropertyOrder(5)]
    [Description("Render filter bitmask for filering this decoration type in render contexts (e.g. mirror)")]
    public FlagsInt32_e VisibleBitmask
    {
      get { return _owner.VisibleBitmask; }
      set { _owner.VisibleBitmask = value; }
    }

    [SortedCategory("Decoration model", 1), PropertyOrder(6)]
    [Description("If enabled, this vegetation type is considered as shadow caster for static light computation. Should only be used for larger models.")]
    public bool CastLightmapShadows
    {
      get { return _owner.CastLightmapShadows; }
      set { _owner.CastLightmapShadows = value; }
    }


    [SortedCategory("Decoration model", 1), PropertyOrder(7)]
    [Description("If enabled, this vegetation type casts dynamic shadows (e.g. in the deferred system)")]
    public bool CastDynamicShadows
    {
      get { return _owner.CastDynamicShadows; }
      set { _owner.CastDynamicShadows = value; }
    }

    [SortedCategory("Decoration model", 1), PropertyOrder(20)]
    [Description("If lightmap shadows are enabled for this decoration type then this defines an optional low-poly mesh used for lightmap. This mesh is not used for rendering.")]
    [EditorAttribute(typeof(FilenameEditor), typeof(UITypeEditor)), FileDialogFilter(new string[] { ".model" })]
    public string CustomLightmapCasterMesh
    {
      get { return _owner.CustomLightmapCasterMesh; }
      set { _owner.CustomLightmapCasterMesh = value; }
    }


    [SortedCategory("Size and distribution", 2), PropertyOrder(1)]
    [Description("Average scaling factor for the model. 1.0 uses original size of the model")]
    [RefreshProperties(RefreshProperties.All)]
    public float AverageScale
    {
      get { return _owner.AverageScale; }
      set { _owner.AverageScale = value; }
    }


    [SortedCategory("Size and distribution", 2), PropertyOrder(2)]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.0f, 100.0f, 100)]
    [Description("Variation of the average size (percentage)")]
    [RefreshProperties(RefreshProperties.All)]
    public float SizeVariation
    {
      get { return _owner.SizeVariation; }
      set { _owner.SizeVariation = value; }
    }

    [SortedCategory("Size and distribution", 2), PropertyOrder(3)]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.0f, 2.0f, 200)]
    [Description("Relative density factor")]
    public float Density
    {
      get { return _owner.Density; }
      set { _owner.Density = value; }
    }

    [SortedCategory("Size and distribution", 2), PropertyOrder(4)]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.0f, 1.0f, 100)]
    [Description("The distribution randomness of the instances")]
    public float DistributionRandomness
    {
      get { return _owner.DistributionRandomness; }
      set { _owner.DistributionRandomness = value; }
    }

    [SortedCategory("Export as luminance", 5), PropertyOrder(10)]
    [Description("The density map can be exported as a luminance texture. A filename can be selected here, but the export has to be triggerd via context menu")]
    [EditorAttribute(typeof(FilenameCreator), typeof(UITypeEditor)), FileDialogFilter(new string[] { ".dds" }), FileCreateDialogAllowOverwrite(true)]
    public string ExportFilename
    {
      get { return _owner.ExportFilename; }
      set { _owner.ExportFilename = value; }
    }


  }

  #endregion

  #region LOD

  public class IDecorationResourcePGView_LOD : IDecorationResourcePGView
  {
    public IDecorationResourcePGView_LOD(DecorationModelResource owner)
      : base(owner)
    {
    }

    [SortedCategory("Resource File", 0), PropertyOrder(1)]
    [Description("Name of this decoration type. Ignored by the engine.")]
    public string Name
    {
      get { return _owner.Name; }
      set { _owner.Name = value; }
    }

    [SortedCategory("Resource File", 0), PropertyOrder(2)]
    [Description("Filename of the render model.")]
    [EditorAttribute(typeof(FilenameEditor), typeof(UITypeEditor)), FileDialogFilter(new string[] { ".model", ".srt", ".atlas" })]
    [ThumbnailFormat(new string[] { @"{0}_data\Thumbnail.bmp", @"{0}_data\Thumbnail.jpg" })]
    [RefreshProperties(RefreshProperties.All)]
    public string Filename
    {
      get { return _owner.Filename; }
      set { _owner.Filename = value; }
    }


    [SortedCategory("Decoration model", 1), PropertyOrder(5)]
    [Description("Render filter bitmask for filering this decoration type in render contexts (e.g. mirror)")]
    public FlagsInt32_e VisibleBitmask
    {
      get { return _owner.VisibleBitmask; }
      set { _owner.VisibleBitmask = value; }
    }

    [SortedCategory("Decoration model", 1), PropertyOrder(3)]
    [Description("Distance in world units at which this model type is clipped.")]
    public float NearClipDistance
    {
      get { return _owner.NearClipDistance; }
      set { _owner.NearClipDistance = value; }
    }

    [SortedCategory("Decoration model", 1), PropertyOrder(4)]
    [Description("Distance in world units at which this model type is clipped.")]
    public float FarClipDistance
    {
      get { return _owner.FarClipDistance; }
      set { _owner.FarClipDistance = value; }
    }

    [SortedCategory("Decoration model", 1), PropertyOrder(6)]
    [Description("If enabled, this vegetation type casts dynamic shadows")]
    public bool CastDynamicShadows
    {
      get { return _owner.CastDynamicShadows; }
      set { _owner.CastDynamicShadows = value; }
    }

    [SortedCategory("Shader properties", 3), PropertyOrder(1)]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.0f, 200.0f, 200)]
    [Description("Percentage of wind modifier applied to this type")]
    public float ApplyWind
    {
      get { return _owner.ApplyWind; }
      set { _owner.ApplyWind = value; }
    }

  }

  #endregion

  #region Model

  public class IDecorationResourcePGView_Model : IDecorationResourcePGView_Basic
  {
    public IDecorationResourcePGView_Model(DecorationModelResource owner)
      : base(owner)
    {
    }

    public override string ResourceType
    {
      get { return "Model file"; }
    }


    [RefreshProperties(RefreshProperties.All)]
    [SortedCategory("Decoration model", 1), PropertyOrder(2)]
    [Description("Determines how the model is aligned on the terrain")]
    public DecorationModelResource.TerrainAlignment ModelAlignment
    {
      get { return _owner.ModelAlignment; }
      set { _owner.ModelAlignment = value; }
    }

    [SortedCategory("Decoration model", 1), PropertyOrder(3)]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.0f, 1.0f, 100)]
    [Description("Relative height of the pivot. 0.0=model pivot, 1.0=bottom of bounding box")]
    public float PivotHeight
    {
      get { return _owner.PivotHeight; }
      set { _owner.PivotHeight = value; }
    }

    [SortedCategory("Decoration model", 1), PropertyOrder(4)]
    [Description("Distance in world units at which this model type is clipped.")]
    public float NearClipDistance
    {
      get { return _owner.NearClipDistance; }
      set { _owner.NearClipDistance = value; }
    }

    [SortedCategory("Decoration model", 1), PropertyOrder(5)]
    [Description("Distance in world units at which this model type is clipped.")]
    public float FarClipDistance
    {
      get { return _owner.FarClipDistance; }
      set { _owner.FarClipDistance = value; }
    }

    [SortedCategory("Physics", 2), PropertyOrder(1)]
    [Description("The capsule radius (in units) for collisions. The capsules are sacaled with every instance, so this radius defines the radius for a scaling of 1.0. Specify 0.0 for no collision.")]
    public float CollisionCapsuleRadius
    {
      get { return _owner.CollisionCapsuleRadius; }
      set { _owner.CollisionCapsuleRadius = value; }
    }

    [SortedCategory("Physics", 2), PropertyOrder(2)]
    [Description("Suggests a collision radius for this model so that the capsule is completely inside the bounding box. The actual radius (CollisionCapsuleRadius) can be larger or smaller.")]
    public float SuggestedCapsuleRadius
    {
      get { return _owner.SuggestedCapsuleRadius; }
    }

    [SortedCategory("Shader properties", 3), PropertyOrder(1)]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.0f, 200.0f, 200)]
    [Description("Percentage of wind modifier applied to this type")]
    public float ApplyWind
    {
      get { return _owner.ApplyWind; }
      set { _owner.ApplyWind = value; }
    }

    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRangeAttribute(0.0f, 1.0f, 100)]
    [SortedCategory("Shader properties", 3), PropertyOrder(2)]
    [Description("Distance relative to far clip distance at which fading out starts. Default is 0.8 which means it starts fading out at 80% of the far clip distance")]
    public float RelativeFadeOutStart
    {
      get { return _owner.RelativeFadeOutStart; }
      set { _owner.RelativeFadeOutStart = value; }
    }
    
    [SortedCategory("Shader properties", 3), PropertyOrder(3)]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.0f, 200.0f, 200)]
    [Description("Percentage of constraint modifier applied to this type")]
    public float ApplyConstraint
    {
      get { return _owner.ApplyConstraint; }
      set { _owner.ApplyConstraint = value; }
    }

  }

  #endregion

  #region Speedtree

  public class IDecorationResourcePGView_Speedtree : IDecorationResourcePGView_Basic
  {
    public IDecorationResourcePGView_Speedtree(DecorationModelResource owner)
      : base(owner)
    {
    }

    public override string ResourceType
    {
      get { return "Speedtree"; }
    }

    [SortedCategory("Decoration model", 1), PropertyOrder(3)]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.0f, 1.0f, 100)]
    [Description("Relative height of the pivot. 0.0=model pivot, 1.0=bottom of bounding box")]
    public float PivotHeight
    {
      get { return _owner.PivotHeight; }
      set { _owner.PivotHeight = value; }
    }

  }

  #endregion

  #region Billboards

  public class IDecorationResourcePGView_Billboards : IDecorationResourcePGView_Basic
  {
    public IDecorationResourcePGView_Billboards(DecorationModelResource owner)
      : base(owner)
    {
    }

    public override string ResourceType
    {
      get { return "Billboards"; }
    }

    [SortedCategory("Decoration model", 1), PropertyOrder(4)]
    [Description("Distance in world units at which this model type is clipped.")]
    public float NearClipDistance
    {
      get { return _owner.NearClipDistance; }
      set { _owner.NearClipDistance = value; }
    }

    [SortedCategory("Decoration model", 1), PropertyOrder(5)]
    [Description("Distance in world units at which this model type is clipped.")]
    public float FarClipDistance
    {
      get { return _owner.FarClipDistance; }
      set { _owner.FarClipDistance = value; }
    }

  }

  #endregion

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
