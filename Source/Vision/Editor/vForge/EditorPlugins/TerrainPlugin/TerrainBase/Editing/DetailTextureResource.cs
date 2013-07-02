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
using System.Runtime.Serialization;
using System.ComponentModel;
using CSharpFramework.PropertyEditors;
using System.Drawing.Design;
using System.Drawing;
using CSharpFramework;
using CSharpFramework.Serialization;
using CSharpFramework.Shapes;
using CSharpFramework.Scene;
using CSharpFramework.Math;

namespace TerrainBase.Editing
{
  /// <summary>
  /// Represents a detail texture resource (C# representation of VTextureWeightmapChannelResource)
  /// </summary>
  [Serializable()]
  [TypeConverter(typeof(PropertySorter))]
  public class DetailTextureResource : ISerializable, ICloneable, ISceneObject, IPropertyFlagsProvider
  {
    public DetailTextureResource(Shape3D ownerTerrain, int iID)
    {
      _ownerTerrain = ownerTerrain;
      ID = iID;
    }

    /// <summary>
    /// Empty constructor for collection editor
    /// </summary>
    public DetailTextureResource()
    {
    }


    #region members

    public Shape3D _ownerTerrain;
    public int _iListIndex = -1;
    public bool _bIsBaseTexture = false; // if true, it can't be deleted
    public bool _bUseClamping = false;
    
    public int ID = 0;
    string _Name;
    string _ExportFilename;
    string _DiffuseFilename;
    string _NormalmapFilename;
    string _SpecularmapFilename;
    public int _iMaterialID = -1;

    // affected sector range
    public int _iSectorX1 = 1000000;
    public int _iSectorY1 = 1000000;
    public int _iSectorX2 = -1000000;
    public int _iSectorY2 = -1000000;

    public float _fBakePriority = 1.0f;
    Vector2F _Tiling = new Vector2F(60, -60);
    Vector2F _Offset = new Vector2F(0, 0);
    bool _bUseThreeWayMapping = false;
    Rectangle _affectedSectorRange = new Rectangle(0, 0, 0, 0);


    #endregion

    public void UpdatePathIfRequired(String fromPath, String toPath)
    {
      if (_DiffuseFilename!=null && _DiffuseFilename.StartsWith(fromPath))
      {
        _DiffuseFilename = toPath + _DiffuseFilename.Substring(fromPath.Length, _DiffuseFilename.Length - fromPath.Length);
      }
      if (_ExportFilename!=null && _ExportFilename.StartsWith(fromPath))
      {
        _ExportFilename = toPath + _ExportFilename.Substring(fromPath.Length, _ExportFilename.Length - fromPath.Length);
      }
      if (_NormalmapFilename!=null && _NormalmapFilename.StartsWith(fromPath))
      {
        _NormalmapFilename = toPath + _NormalmapFilename.Substring(fromPath.Length, _NormalmapFilename.Length - fromPath.Length);
      }
      if (_SpecularmapFilename!=null && _SpecularmapFilename.StartsWith(fromPath))
      {
        _SpecularmapFilename = toPath + _SpecularmapFilename.Substring(fromPath.Length, _SpecularmapFilename.Length - fromPath.Length);
      }
    }

    #region Events

    public static event DetailTextureResourcePropertyChangedEventHandler OnDetailTextureResourcePropertyChanged = null;

    /// <summary>
    /// Global flag to temporarily disable changes
    /// </summary>
    public static bool AllowUpdate = true;


    void TriggerPropertyChangedEvent(DetailTextureResourcePropertyChangedArgs e)
    {
      if (!AllowUpdate || _ownerTerrain == null)
        return;

      Modified = true;
      if (OnDetailTextureResourcePropertyChanged != null)

        OnDetailTextureResourcePropertyChanged(this, e);
    }

    public bool CopyPropertiesFrom(DetailTextureResource other, bool bTriggerChangedEvent)
    {
      bool bAnyChanged = false;
      if (_Name!=other._Name) 
      {
        if (bTriggerChangedEvent)
          TriggerPropertyChangedEvent(new DetailTextureResourcePropertyChangedArgs(_Name, other._Name, "Name"));
        _Name=other._Name;
        bAnyChanged=true;
      }
      if (_ExportFilename != other._ExportFilename)
      {
        if (bTriggerChangedEvent)
          TriggerPropertyChangedEvent(new DetailTextureResourcePropertyChangedArgs(_ExportFilename, other._ExportFilename, "ExportFilename"));
        _ExportFilename = other._ExportFilename; 
        bAnyChanged = true; 
      }
      if (_DiffuseFilename!=other._DiffuseFilename) 
      {
        if (bTriggerChangedEvent)
          TriggerPropertyChangedEvent(new DetailTextureResourcePropertyChangedArgs(_DiffuseFilename, other._DiffuseFilename, "DiffuseFilename"));
        _DiffuseFilename=other._DiffuseFilename;
        bAnyChanged=true;
      }
      if (_NormalmapFilename!=other._NormalmapFilename) 
      {
        if (bTriggerChangedEvent)
          TriggerPropertyChangedEvent(new DetailTextureResourcePropertyChangedArgs(_NormalmapFilename, other._NormalmapFilename, "NormalmapFilename"));
        _NormalmapFilename=other._NormalmapFilename;
        bAnyChanged=true;
      }
      if (_SpecularmapFilename!=other._SpecularmapFilename) 
      {
        if (bTriggerChangedEvent)
          TriggerPropertyChangedEvent(new DetailTextureResourcePropertyChangedArgs(_SpecularmapFilename, other._SpecularmapFilename, "SpecularmapFilename"));
        _SpecularmapFilename=other._SpecularmapFilename;
        bAnyChanged=true;
      }
      if (_fBakePriority!=other._fBakePriority) {
        if (bTriggerChangedEvent)
          TriggerPropertyChangedEvent(new DetailTextureResourcePropertyChangedArgs(_fBakePriority, other._fBakePriority, "BakePriority"));
        _fBakePriority=other._fBakePriority;
        bAnyChanged=true;
      }
      if (_iMaterialID != other._iMaterialID) 
      {
        if (bTriggerChangedEvent)
          TriggerPropertyChangedEvent(new DetailTextureResourcePropertyChangedArgs(_iMaterialID, other._iMaterialID, "MaterialID"));
        _iMaterialID = other._iMaterialID; 
        bAnyChanged = true; 
      }
      if (_Tiling!=other._Tiling) 
      {
        if (bTriggerChangedEvent)
          TriggerPropertyChangedEvent(new DetailTextureResourcePropertyChangedArgs(_Tiling, other._Tiling, "Tiling"));
        _Tiling = other._Tiling;
        bAnyChanged=true;
      }
      if (_Offset!=other._Offset) 
      {
        if (bTriggerChangedEvent)
          TriggerPropertyChangedEvent(new DetailTextureResourcePropertyChangedArgs(_Offset, other._Offset, "Offset"));
        _Offset=other._Offset;
        bAnyChanged=true;
      }

      return bAnyChanged;
   }

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

    #region Properties

    [SortedCategory("Name", 1), PropertyOrder(1)]
    [Description("Name of this detail texture. Ignored by the engine.")]
    public string Name
    {
      get 
      {
        if (!string.IsNullOrEmpty(_Name))
          return this._Name;
        if (!string.IsNullOrEmpty(_DiffuseFilename))
          return this._DiffuseFilename;
        return "<no name>";
      }
      set 
      {
        if (_Name == value)
          return;

        string oldvalue = _Name;
        _Name = value;
        TriggerPropertyChangedEvent(new DetailTextureResourcePropertyChangedArgs(oldvalue, _Name, "Name"));
      }
    }    


    [SortedCategory("Textures", 2), PropertyOrder(20)]
    [Description("Specifies whether unique per-sector textures are used for this channel.")]
    public bool UsesPerSectorTextures
    {
      get
      {
        return _bUseClamping;
      }
    }

    public const string ICON_PERSECTORTEXTURES = @"TerrainPlugin\Icons\PerSectorTextures32.png";

    [SortedCategory("Textures", 2), PropertyOrder(1)]
    [Description("The diffuse texture.")]
    [EditorAttribute(typeof(AssetEditor), typeof(UITypeEditor)), AssetDialogFilter(new string[] { "Texture" })]
    [AlternativeTextureDisplay("%", ICON_PERSECTORTEXTURES)]
    public string DiffuseFilename
    {
      get { return _DiffuseFilename; }
      set 
      {
        if (_DiffuseFilename == value)
          return;

        _bUseClamping = false;
        if (!string.IsNullOrEmpty(value) && value.Contains("%"))
          _bUseClamping = true;

        string oldvalue = _DiffuseFilename;
        _DiffuseFilename = value;
        TriggerPropertyChangedEvent(new DetailTextureResourcePropertyChangedArgs(oldvalue, _DiffuseFilename, "DiffuseFilename"));
      }
    }

    [SortedCategory("Textures", 2), PropertyOrder(2)]
    [Description("The normalmap texture (optional).")]
    [EditorAttribute(typeof(AssetEditor), typeof(UITypeEditor)), AssetDialogFilter(new string[] { "Texture" })]
    [AlternativeTextureDisplay("%", @"TerrainPlugin\Icons\PerSectorTextures32.png")]
    public string NormalmapFilename
    {
      get { return _NormalmapFilename; }
      set
      {
        if (_NormalmapFilename == value)
          return;

        string oldvalue = _NormalmapFilename;
        _NormalmapFilename = value;
        TriggerPropertyChangedEvent(new DetailTextureResourcePropertyChangedArgs(oldvalue, _NormalmapFilename, "NormalmapFilename"));
      }
    }

    [SortedCategory("Textures", 2), PropertyOrder(3)]
    [Description("The specular map texture (optional).")]
    [EditorAttribute(typeof(AssetEditor), typeof(UITypeEditor)), AssetDialogFilter(new string[] { "Texture" })]
    [AlternativeTextureDisplay("%", @"TerrainPlugin\Icons\PerSectorTextures32.png")]
    public string SpecularmapFilename
    {
      get { return _SpecularmapFilename; }
      set
      {
        if (_SpecularmapFilename == value)
          return;

        string oldvalue = _SpecularmapFilename;
        _SpecularmapFilename = value;
        TriggerPropertyChangedEvent(new DetailTextureResourcePropertyChangedArgs(oldvalue, _SpecularmapFilename, "SpecularmapFilename"));
      }
    }

    [SortedCategory("Textures", 2), PropertyOrder(4)]
    [Description("ThreeWayMapping eliminates stretched textures in steep areas. Cannot be used on base layer.")]
    public bool UseThreeWayMapping
    {
      get { return _bUseThreeWayMapping; }
      set
      {
        if (_bIsBaseTexture) // can't use three way mapping on base layer
          return;

        if (_bUseThreeWayMapping == value)
          return;

        bool oldvalue = _bUseThreeWayMapping;
        _bUseThreeWayMapping = value;
        TriggerPropertyChangedEvent(new DetailTextureResourcePropertyChangedArgs(oldvalue, _bUseThreeWayMapping, "UseThreeWayMapping"));
      }
    }

    [SortedCategory("Textures", 2), PropertyOrder(5)]
    [Description("Tiling factor (repetitions per terrain)")]
    public float TilingX
    {
      get { return _Tiling.X; }
      set 
      {
        if (_Tiling.X == value)
          return;
        float oldvalue = _Tiling.X;
        _Tiling.X = value;
        TriggerPropertyChangedEvent(new DetailTextureResourcePropertyChangedArgs(oldvalue, _Tiling.X, "TilingX"));
      }
    }

    [SortedCategory("Textures", 2), PropertyOrder(6)]
    [Description("Tiling factor (repetitions per terrain)")]
    public float TilingY
    {
      get { return _Tiling.Y; }
      set
      {
        if (_Tiling.Y == value)
          return;
        float oldvalue = _Tiling.Y;
        _Tiling.Y = value;
        TriggerPropertyChangedEvent(new DetailTextureResourcePropertyChangedArgs(oldvalue, _Tiling.Y, "TilingY"));
      }
    }

    [Browsable(false)]
    public Vector2F Tiling
    {
      get { return _Tiling; }
      set
      {
        if (_Tiling == value)
          return;
        Vector2F oldvalue = _Tiling;
        _Tiling = value;
        TriggerPropertyChangedEvent(new DetailTextureResourcePropertyChangedArgs(oldvalue, _Tiling, "Tiling"));
      }
    }

    [SortedCategory("Textures", 2), PropertyOrder(7)]
    [Description("Texture offset measured in [0..1] range")]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.0f, 1.0f, 2048)]
    public float TilingOffsetX
    {
      get { return _Offset.X; }
      set
      {
        float fNew = value - (float)Math.Floor(value);
        if (_Offset.X == fNew)
          return;
        float oldvalue = _Offset.X;
        _Offset.X = fNew;
        TriggerPropertyChangedEvent(new DetailTextureResourcePropertyChangedArgs(oldvalue, _Offset.X, "TilingOffsetX"));
      }
    }

    [SortedCategory("Textures", 2), PropertyOrder(8)]
    [Description("Texture offset measured in [0..1] range")]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.0f, 1.0f, 2048)]
    public float TilingOffsetY
    {
      get { return _Offset.Y; }
      set
      {
        float fNew = value - (float)Math.Floor(value);
        if (_Offset.Y == fNew)
          return;
        float oldvalue = _Offset.Y;
        _Offset.Y = fNew;
        TriggerPropertyChangedEvent(new DetailTextureResourcePropertyChangedArgs(oldvalue, _Offset.Y, "TilingOffsetY"));
      }
    }


    [Browsable(false)]
    public Vector2F Offset
    {
      get { return _Offset; }
      set
      {
        if (_Offset == value)
          return;
        Vector2F oldvalue = _Offset;
        _Offset = value;
        TriggerPropertyChangedEvent(new DetailTextureResourcePropertyChangedArgs(oldvalue, _Offset, "Offset"));
      }
    }
    

    [SortedCategory("Textures", 2), PropertyOrder(9)]
    [Description("If this value is >=0 then tis value will be written into the material map for every pixel where this texture is most dominant in the weightmap. The terrain must have a material map.")]
    public int MaterialID
    {
      get { return _iMaterialID; }
      set
      {
        int iValue = value;
        if (iValue < 0)
          iValue = -1;
        if (iValue > 255)
          iValue = 255;

        if (_iMaterialID == iValue)
          return;
        int oldvalue = _iMaterialID;
        _iMaterialID = iValue;
        TriggerPropertyChangedEvent(new DetailTextureResourcePropertyChangedArgs(oldvalue, _iMaterialID, "MaterialID"));
      }
    }

    [SortedCategory("Textures", 2), PropertyOrder(10)]
    [Description("This value allows for prioritizing import texture channels over others when the limit of baked textures per sector is exceeded.")]
    public float BakePriority
    {
      get { return _fBakePriority; }
      set
      {
        if (_fBakePriority == value)
          return;
        float oldvalue = _fBakePriority;
        _fBakePriority = value;
        TriggerPropertyChangedEvent(new DetailTextureResourcePropertyChangedArgs(oldvalue, _fBakePriority, "BakePriority"));
      }
    }

    [SortedCategory("Export as luminance", 5), PropertyOrder(10)]
    [Description("The texture channel can be exported as a luminance texture. A filename can be selected here, but the export has to be triggerd via context menu")]
    [EditorAttribute(typeof(FilenameCreator), typeof(UITypeEditor)), FileDialogFilter(new string[] {".dds" }), FileCreateDialogAllowOverwrite(true)]
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

        string oldvalue = _ExportFilename;
        _ExportFilename = value;
        Modified = true;
        TriggerPropertyChangedEvent(new DetailTextureResourcePropertyChangedArgs(oldvalue, _ExportFilename, "ExportFilename"));
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

    [Browsable(false)]
    public int InternalID
    {
      get { return ID; }
    }

    #endregion

    #region ISerializable Members

    DetailTextureResource(SerializationInfo info, StreamingContext context)
    {
      ID = info.GetInt32("ID");
      _Name = info.GetString("_Name");
      _DiffuseFilename = info.GetString("_DiffuseFilename");
      _Tiling = (Vector2F)info.GetValue("_Tiling", typeof(Vector2F));
      if (SerializationHelper.HasElement(info, "_Offset"))
        _Offset = (Vector2F)info.GetValue("_Offset", typeof(Vector2F));
      if (SerializationHelper.HasElement(info, "_bUseThreeWayMapping"))
        _bUseThreeWayMapping = info.GetBoolean("_bUseThreeWayMapping");
      if (SerializationHelper.HasElement(info,"_iSectorX1"))
      {
        _iSectorX1 = info.GetInt32("_iSectorX1");
        _iSectorY1 = info.GetInt32("_iSectorY1");
        _iSectorX2 = info.GetInt32("_iSectorX2");
        _iSectorY2 = info.GetInt32("_iSectorY2");
      }
      if (SerializationHelper.HasElement(info, "_NormalmapFilename"))
        _NormalmapFilename = info.GetString("_NormalmapFilename");
      if (SerializationHelper.HasElement(info, "_SpecularmapFilename"))
        _SpecularmapFilename = info.GetString("_SpecularmapFilename");

      if (SerializationHelper.HasElement(info, "_fBakePriority"))
        _fBakePriority = info.GetSingle("_fBakePriority");
      if (SerializationHelper.HasElement(info, "_bUseClamping"))
        _bUseClamping = info.GetBoolean("_bUseClamping");
      if (SerializationHelper.HasElement(info, "_ExportFilename"))
        _ExportFilename = info.GetString("_ExportFilename");
      if (SerializationHelper.HasElement(info,"_iMaterialID"))
        _iMaterialID = info.GetInt32("_iMaterialID");
    }

    public void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      info.AddValue("ID", ID);
      info.AddValue("_Name", _Name);
      info.AddValue("_ExportFilename", _ExportFilename);
      info.AddValue("_DiffuseFilename", _DiffuseFilename);
      info.AddValue("_NormalmapFilename", _NormalmapFilename);
      info.AddValue("_SpecularmapFilename", _SpecularmapFilename);
      
      
      info.AddValue("_Tiling", _Tiling);
      info.AddValue("_Offset", _Offset);
      info.AddValue("_bUseThreeWayMapping", _bUseThreeWayMapping);
      
      info.AddValue("_iSectorX1", _iSectorX1);
      info.AddValue("_iSectorY1", _iSectorY1);
      info.AddValue("_iSectorX2", _iSectorX2);
      info.AddValue("_iSectorY2", _iSectorY2);

      info.AddValue("_fBakePriority", _fBakePriority);
      info.AddValue("_bUseClamping", _bUseClamping);
      info.AddValue("_iMaterialID", _iMaterialID);      
    }

    #endregion

    #region ICloneable Members

    public object Clone()
    {
      DetailTextureResource copy = (DetailTextureResource)MemberwiseClone();
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

    public override string ToString()
    {
      return Name;
    }


    #region IPropertyFlagsProvider Members

    public PropertyFlags_e GetPropertyFlags(PropertyFlagsProviderInfo pd)
    {
      if (_bIsBaseTexture && pd.Name == "UseThreeWayMapping")
      {
        return PropertyFlags_e.Readonly;
      }

      return PropertyFlags_e.None;
    }

    public List<PropertyDescriptor> GetAdditionalRootProperties(ITypeDescriptorContext context)
    {
      return null;
    }

    #endregion
  }


  #region Event Delegates
  /// <summary>
  /// Delegate declaration. Sends a data object of type PropertyChangedArgs which provides information about
  /// the property
  /// </summary>
  public delegate void DetailTextureResourcePropertyChangedEventHandler(object sender, DetailTextureResourcePropertyChangedArgs e);

  #endregion


  #region Class: DetailTextureResourcePropertyChangedArgs

  /// <summary>
  /// Event class for changed shape properties. This data object is used by the static DetailTextureResource.OnDetailTextureResourcePropertyChanged event.
  /// </summary>
  public class DetailTextureResourcePropertyChangedArgs : EventArgs
  {
    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="component"></param>
    /// <param name="propertyName"></param>
    public DetailTextureResourcePropertyChangedArgs(object oldValue, object newValue, string propertyName)
    {
      _oldValue = oldValue;
      _newValue = newValue;
      _propertyName = propertyName;
    }
    /// <summary>
    /// Name of the property
    /// </summary>
    public string _propertyName;

    /// <summary>
    /// Old value of the property
    /// </summary>
    public object _oldValue;

    /// <summary>
    /// New value of the property
    /// </summary>
    public object _newValue;
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
