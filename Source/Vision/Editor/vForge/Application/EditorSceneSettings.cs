/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Runtime.Serialization;
using System.IO;
using System.ComponentModel;
using System.Drawing.Design;
using CSharpFramework;
using CSharpFramework.Serialization;
using CSharpFramework.Dialogs;
using CSharpFramework.View;
using System.Collections;
using System.Collections.Specialized;
using CSharpFramework.Math;
using CSharpFramework.Scene;

namespace Editor
{
	/// <summary>
	/// This class provides information about scene specific properties rather than editor specific properties
	/// stored in EditorSettingsBase
	/// </summary>
  [Serializable]
  public class EditorSceneSettings : ICloneable, ISerializable, IDeserializationCallback
  {
    /// <summary>
    /// Constructor
    /// </summary>
    public EditorSceneSettings(IScene ownerScene)
    {
      _ownerScene = ownerScene;
      _cameraPosition *= EditorManager.Settings.GlobalUnitScaling;
    }

    IScene _ownerScene;
    public static SceneExportProfile PATCH_PROFILE = null; // temporary version

    #region ICloneable Members

    /// <summary>
    /// Performs a MemberwiseClone
    /// </summary>
    /// <returns></returns>
    public object Clone()
    {
      return this.MemberwiseClone();
    }

    #endregion

    #region ISerializable Members

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected EditorSceneSettings(SerializationInfo info, StreamingContext context)
    {
      _vLuxSettingsFile = info.GetString("_vLuxSettingsFile");
      _bUseDefaultvLuxSettingsFile = info.GetBoolean("_bUseDefaultvLuxSettingsFile");
      if (SerializationHelper.HasElement(info,"_shapeOrigin"))
        _shapeOrigin = (Vector3F)info.GetValue("_shapeOrigin", typeof(Vector3F));
      if (SerializationHelper.HasElement(info,"_cameraPosition"))
        _cameraPosition = (Vector3F)info.GetValue("_cameraPosition", typeof(Vector3F));
      if (SerializationHelper.HasElement(info,"_cameraAngles"))
        _cameraAngles = (Vector3F)info.GetValue("_cameraAngles", typeof(Vector3F));
      if (SerializationHelper.HasElement(info,"_description"))
        _OLDdescription = info.GetString("_description");
      if (SerializationHelper.HasElement(info,"_showDescAtStartup"))
        _showDescAtStartup = info.GetBoolean("_showDescAtStartup");
      if (SerializationHelper.HasElement(info,"_gridSettings"))
        _gridSettings = (GridSettings)info.GetValue("_gridSettings", typeof(GridSettings));
      if (SerializationHelper.HasElement(info, "_exportProfileName"))
        _exportProfileName = info.GetString("_exportProfileName");
      if (SerializationHelper.HasElement(info, "_autoSaveExportProfile"))
        _autoSaveExportProfile = info.GetBoolean("_autoSaveExportProfile");

      // the following settings are now in export profiles. Load them here for backwards compatibility
      if (PATCH_PROFILE != null) ///...but only if no dedicated profile name has been loaded 
      {
        if (SerializationHelper.HasElement(info, "_exportPath"))
          CurrentProfile.ExportPath = info.GetString("_exportPath");
        if (SerializationHelper.HasElement(info, "_bRunAfterExport"))
          CurrentProfile.RunAfterExport = info.GetBoolean("_bRunAfterExport");
        if (SerializationHelper.HasElement(info, "_exportedLayerNames"))
          CurrentProfile.ExportedLayerNames = (StringCollection)info.GetValue("_exportedLayerNames", typeof(StringCollection));
        if (SerializationHelper.HasElement(info, "_bIncludeVisibilityInfo"))
          CurrentProfile.IncludeVisibilityInfo = info.GetBoolean("_bIncludeVisibilityInfo");
        if (SerializationHelper.HasElement(info, "_bEmbedCustomLitFile"))
          CurrentProfile.EmbedCustomLitFile = info.GetBoolean("_bEmbedCustomLitFile");
      }
    }
    
    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      // thse settings are now loaded from a different file. See SceneExportProfile
      /*
      info.AddValue("_exportPath",_exportPath);
      info.AddValue("_bRunAfterExport",_bRunAfterExport);
      info.AddValue("_bIncludeVisibilityInfo", _bIncludeVisibilityInfo);
      info.AddValue("_bEmbedCustomLitFile", _bEmbedCustomLitFile);
      _exportedLayerNames = new StringCollection();
      foreach (Layer layer in _exportedLayers)
        _exportedLayerNames.Add(layer.LayerName);

      info.AddValue("_exportedLayerNames", _exportedLayerNames);
      */

      info.AddValue("_showDescAtStartup",_showDescAtStartup);
      info.AddValue("_vLuxSettingsFile",_vLuxSettingsFile);
      info.AddValue("_bUseDefaultvLuxSettingsFile",_bUseDefaultvLuxSettingsFile);
      info.AddValue("_shapeOrigin",_shapeOrigin);
      info.AddValue("_cameraPosition",_cameraPosition);
      info.AddValue("_cameraAngles",_cameraAngles);
      info.AddValue("_gridSettings", _gridSettings);
      info.AddValue("_exportProfileName", _exportProfileName);
      info.AddValue("_autoSaveExportProfile", _autoSaveExportProfile);
    }

    #endregion

    #region IDeserializationCallback Members

    /// <summary>
    /// Virtual overridable that gets called after de-serialization
    /// </summary>
    public virtual void OnDeserialization()
    {
    }

    /// <summary>
    /// Interface implementation; calls virtual OnDeserialization
    /// </summary>
    /// <param name="sender"></param>
    public void OnDeserialization(object sender)
    {
      OnDeserialization();
    }

    #endregion

    #region Description

    string _OLDdescription = null;
    bool _showDescAtStartup = true;

    /// <summary>
    /// DEPRECATED. Each layer has its own description. Still used to migrate old scene files
    /// </summary>
    [Browsable(false)]
    public string OLDDescription
    {
      get {return _OLDdescription;}
    }

    /// <summary>
    /// Determines whether the description text should be shown at startup
    /// </summary>
    [SortedCategory("Descripton", 1), PropertyOrder(1)]
    [Description("Determines whether the description text should be shown after scene loading.")]
    public bool ShowDescriptionAtStartup
    {
      get {return _showDescAtStartup;}
      set {_showDescAtStartup=value;}
    }
/*
    /// <summary>
    /// Indicates whether there is any description
    /// </summary>
    [BrowsableAttribute(false)]
    public bool HasDescription
    {
      get 
      {
        return _description!=null && _description.Length>0;
      }
    }
*/
    #endregion

    #region View (Camera,...) related settings

    Vector3F _cameraPosition = new Vector3F(-200, -200, 100);
    Vector3F _cameraAngles = new Vector3F(45, 20, 0);

    /// <summary>
    /// The stored camera position
    /// </summary>
    [Browsable(false)]
    [SortedCategory("Camera", 2), PropertyOrder(1)]
    [Description("The stored camera position")]
    public Vector3F CameraPosition
    {
      get {return _cameraPosition;}
      set {_cameraPosition=value;}
    }

    /// <summary>
    /// The stored camera direction vector
    /// </summary>
    [Browsable(false)]
    [SortedCategory("Camera", 2), PropertyOrder(2)]
    [Description("The stored camera orientation")]
    public Vector3F CameraAngles
    {
      get {return _cameraAngles;}
      set {_cameraAngles=value;}
    }

    #endregion

    #region Shape creation settings

    Vector3F _shapeOrigin = new Vector3F(0,0,0);

    /// <summary>
    /// The shape spawn position
    /// </summary>
    [Browsable(false)]
    [SortedCategory("Shape Creation", 3), PropertyOrder(1)]
    [Description("The position at which new shapes are spawned")]
    public Vector3F ShapeOrigin
    {
      get {return _shapeOrigin;}
      set {_shapeOrigin=value;}
    }

    #endregion

    #region Scene Export related settings

    string _exportProfileName = null;

    [Browsable(false)]
    public string ExportProfileName
    {
      get { return _exportProfileName; }
      set { _exportProfileName = value; }
    }

    bool _autoSaveExportProfile = false;

    [Browsable(false)]
    public bool AutoSaveExportProfile
    {
      get { return _autoSaveExportProfile; }
      set { _autoSaveExportProfile = value; }
    }

    SceneExportProfile CurrentProfile
    {
      get
      {
        if (PATCH_PROFILE != null)
          return PATCH_PROFILE;
        return _ownerScene.CurrentExportProfile;
      }
    }

    /// <summary>
    /// The export path of the scene
    /// </summary>
    [SortedCategory("Export", 4), PropertyOrder(1)]
    [Description("The export path of the scene")]
    protected string ExportPath
    {
      get { return CurrentProfile.ExportPath; }
      set { CurrentProfile.ExportPath = value; }
    }

    /// <summary>
    /// Flag that indicates whether scene should be run in viewer after export
    /// </summary>
    [SortedCategory("Export", 4), PropertyOrder(2)]
    [Description("Flag that indicates whether scene should be run in viewer after export")]
    protected bool RunAfterExport
    {
      get { return CurrentProfile.RunAfterExport; }
      set { CurrentProfile.RunAfterExport = value; }
    }

    /*
    /// <summary>
    /// Flag that indicates whether relevant engine plugins should be copied to the export path so the viewer runs properly
    /// </summary>
    [Browsable(false)]
    [SortedCategory("Export", 4), PropertyOrder(3)]
    [Description("Flag that indicates whether relevant engine plugins should be copied to the export path so the viewer runs properly")]
    public bool CopyRelevantPlugins
    {
      get {return _bCopyRelevantPlugins;}
      set {_bCopyRelevantPlugins=value;}
    }
    */

    /// <summary>
    /// Flag that indicates whether relevant engine plugins should be copied to the export path so the viewer runs properly
    /// </summary>
    [SortedCategory("Export", 4), PropertyOrder(4)]
    [Description("Flag that indicates whether visibility info should be calculated and included into the vscene file")]
    protected bool IncludeVisibilityInfo
    {
      get { return CurrentProfile.IncludeVisibilityInfo; }
      set { CurrentProfile.IncludeVisibilityInfo = value; }
    }

    /// <summary>
    /// Flag that determines whether the .lit file stored in the layer's directory should be embedded into the scene export so
    /// that the VSceneLoader triggers the Vision::Callbacks.OnLightmapFileLoaded callback at loading time.
    /// This flag is not necessary for standard scene lightmapping (e.g. of static meshes)
    /// </summary>
    [SortedCategory("Export", 4), PropertyOrder(5)]
    [Description("Flag that determines whether the .lit file stored in the layer's directory should be embedded into the scene export so"+
                 "that the VSceneLoader triggers the Vision::Callbacks.OnLightmapFileLoaded callback at loading time."+
                 "This flag is not necessary for standard scene lightmapping (e.g. of static meshes")]
    protected bool EmbedCustomLitFile
    {
      get { return CurrentProfile.EmbedCustomLitFile; }
      set { CurrentProfile.EmbedCustomLitFile = value; }
    }

    #endregion

    #region Update Static lighting related options

    bool _bUseDefaultvLuxSettingsFile = true;
    string _vLuxSettingsFile;

    /// <summary>
    /// Get the relative filename of the custom vLux settings file used for lighting from vForge
    /// </summary>
    [SortedCategory("vLux", 5), PropertyOrder(2)]
    [Description("The relative filename of the custom vLux settings file used for lighting from vForge")]
    public string VLuxSettingsFile
    {
      get {return _vLuxSettingsFile;}
      set {_vLuxSettingsFile=value;}
    }

    /// <summary>
    /// If set, vLux will be spawned using the map's default light settings file
    /// </summary>
    [SortedCategory("vLux", 5), PropertyOrder(1)]
    [Description("If set, vLux will be spawned using the map's default light settings file")]
    public bool UseDefaultvLuxSettingsFile
    {
      get {return _bUseDefaultvLuxSettingsFile;}
      set {_bUseDefaultvLuxSettingsFile=value;}
    }
    #endregion

    #region Grid settings

    GridSettings _gridSettings = new GridSettings();

    /// <summary>
    /// Gets or sets the grid settings of the scene view
    /// </summary>
    [Browsable(false)]
    public GridSettings GridSettings
    {
      get {return _gridSettings;}
      set 
      {
        _gridSettings=value;
      }
    }


    #endregion


    public override bool Equals(object obj)
    {
      EditorSceneSettings other = obj as EditorSceneSettings;
      if (other == null)
        return false;
      //if (!string.Equals(_viewerCmdLine, other._viewerCmdLine)) return false;
      if (!_showDescAtStartup.Equals(other._showDescAtStartup)) return false;
      if (!string.Equals(_vLuxSettingsFile,other._vLuxSettingsFile)) return false;
      if (!_bUseDefaultvLuxSettingsFile.Equals(other._bUseDefaultvLuxSettingsFile)) return false;
      if (!_shapeOrigin.Equals(other._shapeOrigin)) return false;
      if (!_cameraPosition.Equals(other._cameraPosition)) return false;
      if (!_cameraAngles.Equals(other._cameraAngles)) return false;
      if (!_gridSettings.Equals(other._gridSettings)) return false;
      return true;
    }

    public override int GetHashCode()
    {
      return base.GetHashCode();
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
