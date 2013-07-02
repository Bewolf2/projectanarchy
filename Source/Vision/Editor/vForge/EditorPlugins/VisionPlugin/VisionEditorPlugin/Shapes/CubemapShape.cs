/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Design;
using System.IO;
using System.Runtime.Serialization;
using CSharpFramework;
using CSharpFramework.PropertyEditors;
using CSharpFramework.Scene;
using CSharpFramework.Serialization;
using CSharpFramework.Shapes;
using CSharpFramework.View;
using VisionManaged;
using System.Collections.Generic;

namespace VisionEditorPlugin.Shapes
{

  #region Class CubemapShape
	/// <summary>
	/// Shape class that represents a cubemap that renders to texture
	/// </summary>
  [Serializable, ShapeTypeNiceName("Cubemap")]
  public class CubemapShape : Shape3D, IPropertyFlagsProvider
	{
    #region nested class : HotSpotUpdateCubemap

    class HotSpotUpdateCubemap : HotSpotPushButton
    {
      public HotSpotUpdateCubemap(CubemapShape owner) : base(owner,@"textures\Refresh.tga",VisionColors.RGB(200,200,200),VisionColors.White,8.0f)
      {
        this.Set2DOffset(30,16);
      }

      public CubemapShape Cubemap {get {return (CubemapShape)Owner;}}

      public override void EvaluatePosition()
      {
        this.Position = Owner.RenderPosition;
      }

      public override void OnClicked(VisionViewBase view)
      {
        base.OnClicked (view);
        Cubemap.UpdateCubemap();
      }


    }

    #endregion

    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_CUBEMAP = "Cubemap";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_CUBEMAP = Shape3D.LAST_CATEGORY_ORDER_ID + 1;

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_UPDATE = "Cubemap Update";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_UPDATE = Shape3D.LAST_CATEGORY_ORDER_ID + 2;

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_RENDERING = "Cubemap Rendering";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_RENDERING = Shape3D.LAST_CATEGORY_ORDER_ID + 3;

    /// <summary>
    /// last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_RENDERING;

    #endregion

    #region Constructor

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="name"></param>
		public CubemapShape(string name) : base(name)
		{
			AddHint(HintFlags_e.NoRotation);
		}

    #endregion

    #region Engine Instance

    /// <summary>
    /// Returns the engine instance as a EngineInstanceRenderTarget
    /// </summary>
    [BrowsableAttribute(false)]
    public EngineInstanceCubemapEntity CubemapEntity {get {return (EngineInstanceCubemapEntity)_engineInstance;}}

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="bCreateChildren"></param>
    public override void CreateEngineInstance(bool bCreateChildren)
    {
      this._engineInstance = new EngineInstanceCubemapEntity(this);
      SetEngineInstanceBaseProperties(); // sets the position etc.

      base.CreateEngineInstance(bCreateChildren);
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    public override void SetEngineInstanceBaseProperties()
    {
      if (_engineInstance==null)
        return;
      base.SetEngineInstanceBaseProperties();
      CubemapEntity.SetCubemapKey(_key,_iSize);
      CubemapEntity.SetUpdateParams(_bContinuousUpdate, _fUpdateInterval, _iUpdateCount, _bAlternate);
      CubemapEntity.SetClipPlanes(_fNearClip, _fFarClip);
      CubemapEntity.SetRenderFilterMask((uint)_iRenderFilterMask);
      CubemapEntity.SetBlurPasses(_iPasses);
      CubemapEntity.SetAutoGenMipMaps(_bGenMipMaps);
      CubemapEntity.SetPreviewVisible(this.Selected || _bAlwaysVisible);
      CubemapEntity.SetExportAsEntity(!IsFile);
      CubemapEntity.SetRenderingType(_eRenderingType);

      switch(_eRenderingType)
      {
        case EngineInstanceCubemapEntity.CubeMapRenderingType_e.RendererNode:
          CubemapEntity.SetRendererConfig(_rendererConfig);
          break;

        case EngineInstanceCubemapEntity.CubeMapRenderingType_e.Scene:
          break;

        case EngineInstanceCubemapEntity.CubeMapRenderingType_e.Specular:
          CubemapEntity.SetSpecularPower(_fSpecularPower);
          break;
      }
    }


    public override bool OnExport(CSharpFramework.Scene.SceneExportInfo info)
    {
      if (this.SaveOnExport)
        SaveToFile();
      return base.OnExport(info);
    }


    #endregion

    #region GUI

    /// <summary>
    /// Overridden function to get the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetIconIndex()
    {
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_cubemap.bmp"), Color.Magenta);
    }

    #endregion

    #region Cloning

    /// <summary>
    /// This function must be overridden, since we have to reset the hotspots
    /// </summary>
    /// <returns></returns>
    public override ShapeBase Clone()
    {
      CubemapShape copy = base.Clone() as CubemapShape;
      copy._hotSpotUpdate = null;
      return copy;
    }

    #endregion

    #region ISerializable

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected CubemapShape(SerializationInfo info, StreamingContext context) : base(info, context)
    {
      _key = info.GetString("_key");
      _iSize = info.GetInt32("_iSize");
      if (SerializationHelper.HasElement(info,"_bGenMipMaps"))
        _bGenMipMaps = info.GetBoolean("_bGenMipMaps");
      if (SerializationHelper.HasElement(info,"_iPasses"))
        _iPasses = info.GetInt32("_iPasses");
      if (SerializationHelper.HasElement(info,"_bContinuousUpdate"))
        _bContinuousUpdate = info.GetBoolean("_bContinuousUpdate");
      if (SerializationHelper.HasElement(info,"_iUpdateCount"))
        _iUpdateCount = info.GetInt32("_iUpdateCount");
      if (SerializationHelper.HasElement(info,"_fUpdateInterval"))
        _fUpdateInterval = info.GetSingle("_fUpdateInterval");
      if (SerializationHelper.HasElement(info, "_bAlternate"))
        _bAlternate = info.GetBoolean("_bAlternate");
      if (SerializationHelper.HasElement(info,"_fNearClip"))
        _fNearClip = info.GetSingle("_fNearClip");
      if (SerializationHelper.HasElement(info,"_fFarClip"))
        _fFarClip = info.GetSingle("_fFarClip");
      if (SerializationHelper.HasElement(info,"_iRenderFilterMask"))
        _iRenderFilterMask = (FlagsInt32_e)info.GetValue("_iRenderFilterMask", typeof(FlagsInt32_e));
    
      if (SerializationHelper.HasElement(info,"_bUpdateInEditor"))
        _bUpdateInEditor = info.GetBoolean("_bUpdateInEditor");
      if (SerializationHelper.HasElement(info, "_bSaveOnExport"))
        _bSaveOnExport = info.GetBoolean("_bSaveOnExport");

      if (SerializationHelper.HasElement(info, "_rendererConfig"))
        _rendererConfig = info.GetString("_rendererConfig");

      if (SerializationHelper.HasElement(info, "_eRenderingType"))
        _eRenderingType = (EngineInstanceCubemapEntity.CubeMapRenderingType_e) info.GetValue("_eRenderingType", typeof(EngineInstanceCubemapEntity.CubeMapRenderingType_e));

      if (SerializationHelper.HasElement(info, "_fSpecularPower"))
        _fSpecularPower = (float)info.GetDouble("_fSpecularPower");

      if (SerializationHelper.HasElement(info, "_bAlwaysVisible"))
        _bAlwaysVisible = info.GetBoolean("_bAlwaysVisible");

			AddHint(HintFlags_e.NoRotation);
      RemoveHint(HintFlags_e.NoScale);
		}

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context) 
    {
      base.GetObjectData(info,context);
      info.AddValue("_bGenMipMaps", _bGenMipMaps);
      info.AddValue("_iPasses", _iPasses);
      info.AddValue("_key", _key);
      info.AddValue("_iSize", _iSize);
      info.AddValue("_bContinuousUpdate", _bContinuousUpdate);
      info.AddValue("_iUpdateCount", _iUpdateCount);
      info.AddValue("_fUpdateInterval", _fUpdateInterval);
      info.AddValue("_bAlternate", _bAlternate);
      info.AddValue("_fNearClip", _fNearClip);
      info.AddValue("_fFarClip", _fFarClip);
      info.AddValue("_iRenderFilterMask", _iRenderFilterMask);

      info.AddValue("_bUpdateInEditor",_bUpdateInEditor);
      info.AddValue("_bSaveOnExport", _bSaveOnExport);

      info.AddValue("_rendererConfig", _rendererConfig);
      info.AddValue("_eRenderingType", _eRenderingType);
      info.AddValue("_fSpecularPower", _fSpecularPower);
      info.AddValue("_bAlwaysVisible", _bAlwaysVisible);
    }

    #endregion

    #region Relevant operations

    static string RO_SAVETOFILE = "Save to file";

    public override ArrayList RelevantOperations
    {
      get
      {
        ArrayList arr = base.RelevantOperations;
        if (IsFile)
        {
          if (arr == null)
            arr = new ArrayList(1);
          arr.Add(RO_SAVETOFILE);
        }
        return arr;
      }
    }

    private void SaveToFile()
    {
      if (CubemapEntity == null || !this.IsFile)
        return;

      string filename = this.Key; // not absolute
      CubemapEntity.SaveToFile(filename);
    }

    public override void PerformRelevantOperation(string name, int iShapeIndex, int iShapeCount)
    {
      base.PerformRelevantOperation(name, iShapeIndex, iShapeCount);
      if (name == RO_SAVETOFILE)
      {
        SaveToFile();
      }
    }

    #endregion


    #region Selection and Hotspots

    HotSpotUpdateCubemap _hotSpotUpdate = null;

    /// <summary>
    /// Overridden function
    /// </summary>
    public override void OnSelected()
    {
      base.OnSelected ();
      if (_engineInstance!=null)
        CubemapEntity.SetPreviewVisible(true);

      System.Diagnostics.Debug.Assert(_hotSpotUpdate==null);
      _hotSpotUpdate = new HotSpotUpdateCubemap(this);
      EditorManager.ActiveView.HotSpots.Add(_hotSpotUpdate);

    }

    /// <summary>
    /// Overridden function
    /// </summary>
    public override void OnUnSelected()
    {
      base.OnUnSelected ();
      if (_engineInstance!=null)
        CubemapEntity.SetPreviewVisible(_bAlwaysVisible);

      EditorManager.ActiveView.HotSpots.Remove(_hotSpotUpdate);
      _hotSpotUpdate.Remove();
      _hotSpotUpdate = null;
    }

/*
    public override void OnHotSpotEvaluatePosition(HotSpotBase hotSpot)
    {
      base.OnHotSpotEvaluatePosition (hotSpot);
      if (hotSpot==_hotSpotUpdate)
        _hotSpotUpdate.Position = this.Position;
    }
*/
    #endregion

    #region Members

    int _iPasses = 0;
    bool _bGenMipMaps=false;
    string _key = "<cubemap>";
    int _iSize = 256;
    static bool _bUpdateInEditor = true;
    bool _bContinuousUpdate = false;
    bool _bSaveOnExport = true;
    int _iUpdateCount = 0;
    float _fUpdateInterval = 0.0f;
    bool _bAlternate = false;
    float _fNearClip = -1.0f;
    float _fFarClip = -1.0f;
    string _rendererConfig = "";
    FlagsInt32_e _iRenderFilterMask = FlagsInt32_e.Bit2; // 4=engine's default
    EngineInstanceCubemapEntity.CubeMapRenderingType_e _eRenderingType = EngineInstanceCubemapEntity.CubeMapRenderingType_e.Scene;
    float _fSpecularPower = 1;
    bool _bAlwaysVisible = false;

    #endregion

    #region Properties

    /// <summary>
    /// Cubemap key that can be specified as a texture filename in shaders.
    /// </summary>
    [SortedCategory(CAT_CUBEMAP, CATORDER_CUBEMAP), PropertyOrder(1),
    Description("Cubemap key that can be specified as a texture filename in shaders. If this key specifies a .dds filename, it can be saved to file manually or on export time.")]
    [TypeConverter(typeof(SceneObjectKeyConverter)), SceneObjectKeyType(IEngineManager.ObjectKeyType_e.Cubemap)]
    public string Key
    {
      get {return _key;}
      set 
      {
        _key=value;
        if (HasEngineInstance())
        {
          CubemapEntity.SetCubemapKey(_key,_iSize);
          CubemapEntity.SetExportAsEntity(!IsFile);
        }
      }
    }

    [Description("Indicates whether the key specifies a .dds filename or not")]
    [SortedCategory(CAT_CUBEMAP, CATORDER_CUBEMAP), PropertyOrder(10)]
    public bool IsFile
    {
      get
      {
        return _key != null && _key.EndsWith(".dds", true, null);
      }
    }


    [Description("If the key specifies a target .dds file then this flag specifies whether the .dds should be saved at scene export time.")]
    [SortedCategory(CAT_CUBEMAP, CATORDER_CUBEMAP), PropertyOrder(11), DefaultValue(true)]
    public bool SaveOnExport
    {
      get
      {
        return _bSaveOnExport;
      }
      set
      {
        _bSaveOnExport = value;
      }
    }


    /// <summary>
    /// Edge size of the cubemap texture. Must be a power of 2.
    /// </summary>
    [SortedCategory(CAT_CUBEMAP, CATORDER_CUBEMAP), PropertyOrder(4),
    TypeConverter(typeof(IntListTypeConverter)),AllowedIntValues(new int[] {32,64,128,256,512,1024}),
    Description("Edge size of the cubemap texture. Must be a power of 2."), DefaultValue(256)]
    public int EdgeSize
    {
      get {return _iSize;}
      set 
      {
        _iSize=value;
        if (HasEngineInstance())
          CubemapEntity.SetCubemapKey(_key, _iSize);
        UpdateCubemap();
      }
    }

    /// <summary>
    /// If true, the cubemap gets contiuously updated in the game which costs significant performance.
    /// If false, the cubemap gets only rendered at creation time
    /// </summary>
    [SortedCategory(CAT_UPDATE, CATORDER_UPDATE), PropertyOrder(1),
    Description("If true, the cubemap gets contiuously updated in the game which costs significant performance. If false, the cubemap gets only rendered at creation time"), DefaultValue(false)]
    public bool ContinuousUpdate
    {
      get {return _bContinuousUpdate;}
      set 
      {
        _bContinuousUpdate=value;
        if (HasEngineInstance())
          CubemapEntity.SetUpdateParams(_bContinuousUpdate, _fUpdateInterval, _iUpdateCount, _bAlternate);
      }
    }

    /// <summary>
    /// If ContinuousUpdate is enabled, this defines the time interval (in seconds) between two rendering. 0 updates it every frame.
    /// </summary>
    [SortedCategory(CAT_UPDATE, CATORDER_UPDATE), PropertyOrder(2),
    Description("If ContinuousUpdate is enabled, this defines the time interval (in seconds) between two rendering. 0 updates it every frame."), DefaultValue(0f)]
    public float UpdateInterval
    {
      get {return _fUpdateInterval;}
      set 
      {
        _fUpdateInterval=value;
        if (HasEngineInstance())
          CubemapEntity.SetUpdateParams(_bContinuousUpdate, _fUpdateInterval, _iUpdateCount, _bAlternate);
      }
    }

    /// <summary>
    /// If ContinuousUpdate is enabled, this defines whether one tick only renders one cubemap face at a time, so 6 consecutive frames make up the new cubemap. With this feature the update impact is distributed over frames.
    /// </summary>
    [SortedCategory(CAT_UPDATE, CATORDER_UPDATE), PropertyOrder(2),
    Description("If ContinuousUpdate is enabled, this defines whether one tick only renders one cubemap face at a time, so 6 consecutive frames make up the new cubemap. With this feature the update impact is distributed over frames."), DefaultValue(false)]
    public bool AlternatingFaceUpdate
    {
      get { return _bAlternate; }
      set
      {
        _bAlternate = value;
        if (HasEngineInstance())
          CubemapEntity.SetUpdateParams(_bContinuousUpdate, _fUpdateInterval, _iUpdateCount, _bAlternate);
      }
    }

    /// <summary>
    /// If ContinuousUpdate is enabled, this defines the number of updates (0 for infinite count)
    /// </summary>
    [SortedCategory(CAT_UPDATE, CATORDER_UPDATE), PropertyOrder(4),
    Description("If ContinuousUpdate is enabled, this defines the number of updates (0 for infinite count)"), DefaultValue(0)]
    public int UpdateCount
    {
      get {return _iUpdateCount;}
      set 
      {
        _iUpdateCount=value;
        if (HasEngineInstance())
          CubemapEntity.SetUpdateParams(_bContinuousUpdate, _fUpdateInterval, _iUpdateCount, _bAlternate);
      }
    }

    /// <summary>
    /// Indicates whether the cubemap image will be updated every time it changes position.
    /// Only affects rendering inside the editor.
    /// </summary>
    [SortedCategory(CAT_UPDATE, CATORDER_UPDATE), PropertyOrder(4),
    Description("Indicates whether the cubemap image will be updated every time it changes position inside vForge. Only affects rendering inside the editor."), DefaultValue(true)]
    public bool AlwaysUpdateInEditor
    {
      get {return _bUpdateInEditor;}
      set 
      {
        _bUpdateInEditor=value;
        if (_bUpdateInEditor)
          UpdateCubemap();
      }
    }

      /// <summary>
      /// Indicates whether the cubemap will automatically generate mipmaps.
      /// </summary>
      [SortedCategory(CAT_UPDATE, CATORDER_UPDATE), PropertyOrder(1),
      Description("Indicates whether the cubemap will automatically generate mipmaps."), DefaultValue(false)]
      public bool AutoGenerateMipmaps
      {
          get {return _bGenMipMaps;}
          set 
          {
              _bGenMipMaps=value;
              if (HasEngineInstance())
                CubemapEntity.SetAutoGenMipMaps(_bGenMipMaps);
              UpdateCubemap();
          }
      }

    /// <summary>
    /// Number of blur passes for cubemap.
    /// </summary>
    [SortedCategory(CAT_CUBEMAP, CATORDER_CUBEMAP), PropertyOrder(5),
    TypeConverter(typeof(IntListTypeConverter)),AllowedIntValues(new int[] {0,1,2,3,4,5,6,7,8,9,10}),
    Description("Number of gaussian blur passes for cubemap (0 is fastest)."), DefaultValue(0)]
    public int NumBlurPasses
    {
      get {return _iPasses;}
      set 
      {
        _iPasses=value;
        if (HasEngineInstance())
          CubemapEntity.SetBlurPasses(_iPasses);
        UpdateCubemap();
      }
    }

    /// <summary>
    /// Always preview the cubemap in vForge.
    /// </summary>
    [SortedCategory(CAT_RENDERING, CATORDER_RENDERING), PropertyOrder(-1),
    Description("Always preview the cubemap in vForge."), DefaultValue(false)]
    public bool AlwaysVisible
    {
      get
      {
        return _bAlwaysVisible;
      }
      set
      {
        _bAlwaysVisible = value;
        if (HasEngineInstance())
        {
          CubemapEntity.SetPreviewVisible(this.Selected || _bAlwaysVisible);
        }
        UpdateCubemap();
      }
    }

    /// <summary>
    /// How to render the cubemap.
    /// </summary>
    [SortedCategory(CAT_RENDERING, CATORDER_RENDERING), PropertyOrder(0),
    Description("How to render the cubemap. Only affects vForge, exported shape will use \"Scene\"."), DefaultValue(EngineInstanceCubemapEntity.CubeMapRenderingType_e.Scene)]
    public EngineInstanceCubemapEntity.CubeMapRenderingType_e RenderingType
    {
      get
      {
        return _eRenderingType;
      }
      set
      {
        _eRenderingType = value;
        if (HasEngineInstance())
        {
          SetEngineInstanceBaseProperties();
        }
        UpdateCubemap();
      }
    }


    /// <summary>
    /// Near-clipping distance for the cubemap rendering. A value smaller than 0 uses the current main context's distance.
    /// </summary>
    [SortedCategory(CAT_RENDERING, CATORDER_RENDERING), PropertyOrder(1),
    Description("Near-clipping distance for the cubemap rendering. A value <0 uses the current main context's distance."), DefaultValue(-1f)]
    public float NearClipDistance
    {
      get {return _fNearClip;}
      set 
      {
        _fNearClip=value;
        if (HasEngineInstance())
          CubemapEntity.SetClipPlanes(_fNearClip, _fFarClip);
        UpdateCubemap();
      }
    }

    /// <summary>
    /// Far-clipping distance for the cubemap rendering. A value smaller than 0 uses the current main context's distance.
    /// </summary>
    [SortedCategory(CAT_RENDERING, CATORDER_RENDERING), PropertyOrder(2),
    Description("Far-clipping distance for the cubemap rendering. A value <0 uses the current main context's distance."), DefaultValue(-1f)]
    public float FarClipDistance
    {
      get {return _fFarClip;}
      set 
      {
        _fFarClip=value;
        if (HasEngineInstance())
          CubemapEntity.SetClipPlanes(_fNearClip, _fFarClip);
        UpdateCubemap();
      }
    }

    /// <summary>
    /// Bitmask for scene element filtering inside the cubemap render loop. Gets logically ANDed with for instance entity's VisibleBitmask value.
    /// </summary>
    [SortedCategory(CAT_RENDERING, CATORDER_RENDERING), PropertyOrder(3),
    Description("Bitmask for scene element filtering inside the cubemap render loop. Gets logically ANDed with for instance entity's VisibleBitmask value."), DefaultValue(FlagsInt32_e.Bit2)]
    public FlagsInt32_e RenderFilterMask
    {
      get {return _iRenderFilterMask;}
      set 
      {
        _iRenderFilterMask=value;
        if (HasEngineInstance())
          CubemapEntity.SetRenderFilterMask((uint)_iRenderFilterMask);
        UpdateCubemap();
      }
    }

    /// <summary>
    /// File to a renderer config that is used to render the faces.
    /// </summary>
    [SortedCategory(CAT_RENDERING, CATORDER_RENDERING), PropertyOrder(5), Description("A renderer node engine file to use for rendering the faces. vForge only, will not be exported.")]
    [EditorAttribute(typeof(FilenameEditor), typeof(UITypeEditor)), FileDialogFilter(new string[] { ".VRenderer" })]
    public string RendererConfig
    {
      get
      {
        return _rendererConfig;
      }
      set
      {
        _rendererConfig = value;
        if (HasEngineInstance())
        {
          CubemapEntity.SetRendererConfig(value);
        }
        UpdateCubemap();
      }
    }

    /// <summary>
    /// Specular power to use for rendering lights.
    /// </summary>
    [SortedCategory(CAT_RENDERING, CATORDER_RENDERING), PropertyOrder(6), Description("Specular power to use for rendering lights."), DefaultValue(1f)]
    public float SpecularPower
    {
      get
      {
        return _fSpecularPower;
      }
      set
      {
        _fSpecularPower = value;
        if (HasEngineInstance())
        {
          CubemapEntity.SetSpecularPower(value);
        }
        UpdateCubemap();
      }
    }
 

    #endregion

    #region IPropertyFlagsProvider Members

    public override PropertyFlags_e GetPropertyFlags(PropertyFlagsProviderInfo pd)
    {
      switch(_eRenderingType)
      {
        case EngineInstanceCubemapEntity.CubeMapRenderingType_e.Scene:
          if(pd.Name == "RendererConfig" || pd.Name == "SpecularPower")
            return PropertyFlags_e.Hidden;
          break;

        case EngineInstanceCubemapEntity.CubeMapRenderingType_e.RendererNode:
          if(pd.Name == "SpecularPower")
            return PropertyFlags_e.Hidden;
          break;

        case EngineInstanceCubemapEntity.CubeMapRenderingType_e.Specular:
          if (pd.Name == "NearClipDistance" || pd.Name == "FarClipDistance" || pd.Name == "RendererConfig")
            return PropertyFlags_e.Hidden;
          break;
      }

      return base.GetPropertyFlags(pd);
    }

    #endregion

    #region Cubemap functions

    /// <summary>
    /// Forces an engine update of the cubemap
    /// </summary>
    public void UpdateCubemap()
    {
      if (this._engineInstance!=null)
        CubemapEntity.UpdateCubemap();
      EditorManager.ActiveView.UpdateView(false);
    }

    /// <summary>
    /// whenever the cubemap position changes, also update the rendered image
    /// </summary>
    public override void OnTransformationChanged()
    {
      base.OnTransformationChanged ();
      if (_bUpdateInEditor)
        UpdateCubemap();
    }

    #endregion

	}

  #endregion

  #region CubemapShape Creator Plugin

  class CubemapShapeCreator : CSharpFramework.IShapeCreatorPlugin
  {
    public CubemapShapeCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_cubemap.bmp"), Color.Magenta);
    }

    public override string GetPluginName()
    {
      return "Cubemap";
    }

    public override string GetPluginCategory()
    {
      return "Effects";
    }

    /// <summary>
    /// Returns a short description text
    /// </summary>
    /// <returns></returns>
    public override string GetPluginDescription()
    {
      return "A cubemap instance renders the scene into all 6 face directions at scene startup.\n"+
        "The result cubemap texture can be referenced by shaders as a cubemap sampler.";
    }

    public override ShapeBase CreateShapeInstance()
    {
      CubemapShape shape = new CubemapShape("Cubemap");
      shape.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
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
