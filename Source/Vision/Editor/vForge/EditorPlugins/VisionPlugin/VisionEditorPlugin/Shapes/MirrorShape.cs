/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.IO;
using System.Drawing.Design;
using System.ComponentModel;
using System.Globalization;
using System.Runtime.Serialization;
using CSharpFramework;
using ManagedBase;
using ManagedFramework;
using CSharpFramework.Math;
using CSharpFramework.Shapes;
using CSharpFramework.PropertyEditors;
using CSharpFramework.DynamicProperties;
using CSharpFramework.UndoRedo;
using CSharpFramework.Actions;
using CSharpFramework.Scene;
using CSharpFramework.View;
using CSharpFramework.Serialization;
using VisionManaged;
using System.Drawing;

namespace VisionEditorPlugin.Shapes
{
  #region Mirror Shape
	/// <summary>
	/// Shape that represents a mirror in the engine
	/// </summary>
  [DefaultProperty("Engine Class"), Serializable, ShapeTypeNiceName("Mirror")]
  public class MirrorShape : ShapeObject3D
	{
    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_MIRRORMODEL = "Mirror Model";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_MIRRORMODEL = Shape3D.LAST_CATEGORY_ORDER_ID + 1;

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_VISIBILITY = "Mirror Visibility";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_VISIBILITY = Shape3D.LAST_CATEGORY_ORDER_ID + 2;

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_ADVANCED = "Advanced";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_ADVANCED = Shape3D.LAST_CATEGORY_ORDER_ID + 3;

    /// <summary>
    /// last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_ADVANCED;

    #endregion

    #region Constructor

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="name"></param>
		public MirrorShape(string name) : base(name)
		{
      this._fSizeX *= EditorManager.Settings.GlobalUnitScaling;
      this._fSizeY *= EditorManager.Settings.GlobalUnitScaling;

      _shaderFX = new ShaderEffectConfig(this);

      // remember to add the same stuff in the serialization constructor
      _shaderFX.OnShaderEffectChanged += new CSharpFramework.Scene.ShaderEffectConfig.ShaderEffectChangedEventHandler(_shaderFX_OnShaderEffectChanged);
    }

    /// <summary>
    /// mirror destructor
    /// </summary>
    ~MirrorShape()
    {
      if (_shaderFX!=null)
        _shaderFX.OnShaderEffectChanged -= new CSharpFramework.Scene.ShaderEffectConfig.ShaderEffectChangedEventHandler(_shaderFX_OnShaderEffectChanged);
    }

    #endregion

    #region GUI

    /// <summary>
    /// Overridden function to get the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetIconIndex()
    {
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_mirror.bmp"), Color.Magenta);
    }

    #endregion

    #region Shape3D Members

    /// <summary>
    /// Configures this shape to use parenting through VisObject3D_cl inheritance
    /// </summary>
    protected override Shape3D.ParentingModes ParentingMode
    {
      get { return ParentingModes.EngineInstance; }
    }


    public override PropertyFlags_e GetPropertyFlags(PropertyFlagsProviderInfo pd)
    {
      PropertyFlags_e flags = base.GetPropertyFlags(pd);
#if !HK_ANARCHY
      if (!(this is WaterShape))
#endif
      {
        // we can easily expose these parameters
        if (pd.Name == "ObliqueClippingPlaneOffset" || pd.Name == "FovScale")
          flags |= PropertyFlags_e.Hidden;
      }


      return flags;
    }

    public override bool GetLocalBoundingBox(ref BoundingBox bbox)
    {
      float z = EditorManager.Settings.GlobalUnitScaling;
      bbox = new BoundingBox(-_fSizeX * 0.5f, -_fSizeY * 0.5f, -z, _fSizeX * 0.5f, _fSizeY * 0.5f, z);
      return true;
    }

    void CheckRendererNodeCompatibility()
    {
      if (HasEngineInstance() && !EngineMirror.CheckRendererNodeCompatibility())
      {
        // error message
      }
    }


    public override void OnSceneEvent(SceneEventArgs args)
    {
      base.OnSceneEvent(args);
      if (args.action == SceneEventArgs.Action.AfterRendererNodeChanged)
      {
        CheckRendererNodeCompatibility();
      }
    }

    protected override void DisposeObject()
    {
      if (_shaderFX != null)
        _shaderFX.Dispose();
      _shaderFX = null;
      base.DisposeObject();
    }

    #endregion

    #region Engine instance

    [Browsable(false)]
    public virtual MirrorType NativeMirrorType
    {
      get
      {
        return MirrorType.Mirror;
      }
    }

    /// <summary>
    /// Create the engine instance
    /// </summary>
    /// <param name="bCreateChildren"></param>
    public override void CreateEngineInstance(bool bCreateChildren)
    {
      this._engineInstance = new EngineInstanceMirror(NativeMirrorType);
      SetEngineInstanceBaseProperties(); // sets the position etc.

      base.CreateEngineInstance(bCreateChildren);
    }


    /// <summary>
    /// Access the engine instance as EngineInstanceMirror
    /// </summary>
    [BrowsableAttribute(false)]
    public EngineInstanceMirror EngineMirror {get {return (EngineInstanceMirror)_engineInstance;}}

    /// <summary>
    /// Sets all properties of the engine instance
    /// </summary>
    public override void SetEngineInstanceBaseProperties()
    {
      // these functions are called before the native mirror object exists
      EngineMirror.SetResolution(_iResolution);
      EngineMirror.SetUseHDR(_bUseHDR);

      // create the actual instance
      EngineMirror.CreateMirrorInstance();

      base.SetEngineInstanceBaseProperties ();
      EngineMirror.SetSize(_fSizeX * ScaleX, _fSizeY * ScaleY);
      EngineMirror.SetModel(_modelFile);
      EngineMirror.SetShaderEffect(_shaderFX);
      EngineMirror.SetFarclipDist(_farClipDist);
      EngineMirror.SetRenderFilterMask((uint)_iContextBitmask, _bRenderExtObj);
      EngineMirror.SetRenderHook((uint)_renderHook);
      EngineMirror.SetReflectionShaderMode(_reflectionShaderMode);
      EngineMirror.SetObliqueClippingPlaneOffset(_fObliqueClippingPlaneOffset);
      EngineMirror.SetFovScale(_fFovScale);
      EngineMirror.SetUseLODFromRefContext(_bLODFromRefContext);
      CheckRendererNodeCompatibility();
    }

    /// <summary>
    /// Renders the shape in the view
    /// </summary>
    /// <param name="view"></param>
    /// <param name="mode"></param>
    public override void RenderShape(VisionViewBase view, ShapeRenderMode mode)
    {
      base.RenderShape (view, mode);
      if (_engineInstance!=null)
        EngineMirror.RenderShape(view,mode);
    }

    public override void OnTransformationChanged()
    {
      base.OnTransformationChanged();

      // scaling might have changed:
      if (HasEngineInstance())
        EngineMirror.SetSize(_fSizeX * ScaleX, _fSizeY * ScaleY);

      if (_hotSpotSizeX != null)
        _hotSpotSizeX.DisplayScaling = 0.5f * ScaleX;
      if (_hotSpotSizeY != null)
        _hotSpotSizeY.DisplayScaling = 0.5f * ScaleY;

    }

    #endregion

    #region ISerializable

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected MirrorShape(SerializationInfo info, StreamingContext context) : base(info, context)
    {
      _fSizeX = info.GetSingle("_fSizeX");
      _fSizeY = info.GetSingle("_fSizeY");
      _shaderFX = (ShaderEffectConfig)info.GetValue("_shaderFX",typeof(ShaderEffectConfig));
      _shaderFX.OnShaderEffectChanged += new CSharpFramework.Scene.ShaderEffectConfig.ShaderEffectChangedEventHandler(_shaderFX_OnShaderEffectChanged);
      _shaderFX.Owner = this;

      if (SerializationHelper.HasElement(info,"_iResolution"))
        _iResolution = info.GetInt32("_iResolution");
      if (SerializationHelper.HasElement(info,"_modelFile"))
        _modelFile = info.GetString("_modelFile");
      if (SerializationHelper.HasElement(info,"_farClipDist"))
        _farClipDist = info.GetSingle("_farClipDist");
      if (SerializationHelper.HasElement(info,"_iContextBitmask"))
        _iContextBitmask = (FlagsInt32_e)info.GetValue("_iContextBitmask", typeof(FlagsInt32_e));
      if (SerializationHelper.HasElement(info, "_reflectionShaderMode"))
        _reflectionShaderMode = (ReflectionShaderMode)info.GetValue("_reflectionShaderMode", typeof(ReflectionShaderMode));
      if (SerializationHelper.HasElement(info, "_bRenderExtObj"))
        _bRenderExtObj = info.GetBoolean("_bRenderExtObj");
      if (SerializationHelper.HasElement(info, "_fFovScale"))
        _fFovScale = info.GetSingle("_fFovScale");
      if (SerializationHelper.HasElement(info, "_fObliqueClippingPlaneOffset"))
        _fObliqueClippingPlaneOffset = info.GetSingle("_fObliqueClippingPlaneOffset");
      if (SerializationHelper.HasElement(info, "_bLODFromRefContext"))
        _bLODFromRefContext = info.GetBoolean("_bLODFromRefContext");
      if (SerializationHelper.HasElement(info, "_bUseHDR"))
        _bUseHDR = info.GetBoolean("_bUseHDR");

      if (SerializationHelper.HasElement(info, "_renderHook3"))
      {
        _renderHook = (RenderOrderBits_e)info.GetValue("_renderHook3", typeof(RenderOrderBits_e));
      }
      else if (SerializationHelper.HasElement(info, "_renderHook2"))
      {
        RenderOrderBits_e eRH;
        eRH = (RenderOrderBits_e)info.GetValue("_renderHook2", typeof(RenderOrderBits_e));

        // toggle VRH_ADDITIVE_PARTICLES with VRH_TRANSLUCENT_VOLUMES so that the VRenderHook_e enum order matches the execution order
        if (eRH == RenderOrderBits_e.AdditiveParticles)       _renderHook = RenderOrderBits_e.TranslucentVolumes;
        else if (eRH == RenderOrderBits_e.TranslucentVolumes) _renderHook = RenderOrderBits_e.AdditiveParticles;
        else                                                  _renderHook = eRH;
      }
      else if (SerializationHelper.HasElement(info, "_renderHook"))
      {
        int oldValue = info.GetInt32("_renderHook");
        _renderHook = ConversionUtils.UpdateRenderOrderBits(oldValue);
      }

      // backwards compatibility
      if (!SerializationHelper.HasElement(info, "SupportScaling"))
        SetScaling_Internal(1.0f, 1.0f, 1.0f);

      if (LinkTargets.Count==0) // old version
        this.CreateAllLinks();

    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context) 
    {
      base.GetObjectData(info,context);
      info.AddValue("_fSizeX",_fSizeX);
      info.AddValue("_fSizeY",_fSizeY);
      info.AddValue("_shaderFX",_shaderFX);
      info.AddValue("_iResolution",_iResolution);
      info.AddValue("_modelFile",_modelFile);
      info.AddValue("_farClipDist",_farClipDist);
      info.AddValue("_iContextBitmask",_iContextBitmask);
      info.AddValue("_reflectionShaderMode", _reflectionShaderMode);
      info.AddValue("_fFovScale", _fFovScale);
      info.AddValue("_fObliqueClippingPlaneOffset", _fObliqueClippingPlaneOffset);
      info.AddValue("_bLODFromRefContext", _bLODFromRefContext);
      info.AddValue("_bRenderExtObj", _bRenderExtObj);
      info.AddValue("SupportScaling", true);
      info.AddValue("_bUseHDR", _bUseHDR);
      info.AddValue("_renderHook3", _renderHook);
    }

    #endregion

    #region Cloning

    /// <summary>
    /// This function must be overridden, since we have to reset the hotspots
    /// </summary>
    /// <returns></returns>
    public override ShapeBase Clone()
    {
      MirrorShape copy = (MirrorShape)base.Clone();
      System.Diagnostics.Debug.Assert(!copy.Selected); // turned off in ShapeBase

      copy._shaderFX = (ShaderEffectConfig)_shaderFX.Clone();
      copy._shaderFX.OnShaderEffectChanged += new CSharpFramework.Scene.ShaderEffectConfig.ShaderEffectChangedEventHandler(copy._shaderFX_OnShaderEffectChanged);

      copy._hotSpotSizeX = null;
      copy._hotSpotSizeY = null;

      return copy;
    }

    #endregion

    #region Mirror properties

    float _fSizeX = 200.0f;
    float _fSizeY = 200.0f;
    int _iResolution = 512;
    bool _bUseHDR = false;
    ShaderEffectConfig _shaderFX;
    string _modelFile;
    float _farClipDist = -1.0f;
    FlagsInt32_e _iContextBitmask = FlagsInt32_e.Bit1;
    RenderOrderBits_e _renderHook = RenderOrderBits_e.Decals;
    ReflectionShaderMode _reflectionShaderMode = ReflectionShaderMode.AlwaysSimple;
    bool _bRenderExtObj = false;

    // water properties but maybe useful for mirrors
    float _fFovScale = 1.0f;
    float _fObliqueClippingPlaneOffset = 0.0f;
    bool _bLODFromRefContext = true;


    /// <summary>
    /// Size of the mirror, in world units
    /// </summary>
    [SortedCategory(CAT_MIRRORMODEL, CATORDER_MIRRORMODEL),
    PropertyOrder(1),
    Description("Size of the mirror plane in x-direction"), DefaultValue(200f)]
    public float SizeX
    {
      get {return _fSizeX;}
      set
      {
        _fSizeX = value;
        if (_fSizeX<1.0f) _fSizeX=1.0f;
        if (HasEngineInstance())
          EngineMirror.SetSize(_fSizeX * ScaleX, _fSizeY * ScaleY);
      }
    }

    /// <summary>
    /// Size of the mirror, in world units
    /// </summary>
    [SortedCategory(CAT_MIRRORMODEL, CATORDER_MIRRORMODEL),
    PropertyOrder(2),
    Description("Size of the mirror plane in y-direction"), DefaultValue(200f)]
    public float SizeY
    {
      get {return _fSizeY;}
      set
      {
        _fSizeY = value;
        if (_fSizeY<1.0f) _fSizeY=1.0f;
        if (HasEngineInstance())
          EngineMirror.SetSize(_fSizeX * ScaleX, _fSizeY * ScaleY);
      }
    }

    /// <summary>
    /// Model File Property (uses filename editor)
    /// </summary>
    [PrefabResolveFilename]
    [SortedCategory(CAT_MIRRORMODEL, CATORDER_MIRRORMODEL),
    PropertyOrder(3),
    EditorAttribute(typeof(AssetEditor), typeof(UITypeEditor)),
    AssetDialogFilter(new string[] { "Model" })]
    [Description("Model used as a mirror.")]
    [ThumbnailFormat(new string[] { @"{0}_data\Thumbnail.bmp", @"{0}_data\Thumbnail.jpg" })]
    public string ModelFile
    {
      get {return _modelFile;}
      set 
      {
        if (_modelFile==value) return;
        _modelFile = value;
        if (HasEngineInstance())
          EngineMirror.SetModel(_modelFile);
      }
    }

    /// <summary>
    /// The shader effect configuration assigned to the mirror
    /// </summary>
    [SortedCategory(CAT_MIRRORMODEL, CATORDER_MIRRORMODEL),
    PropertyOrder(4),
    TypeConverter(typeof(UndoableObjectConverter)),
    Description("The shader effect configuration assigned to the mirror")]
    public ShaderEffectConfig ShaderEffect
    {
      get {return _shaderFX;}
      set 
      {
        if (_shaderFX == value)
          return;

        if (_shaderFX != null)
        {
          _shaderFX.OnShaderEffectChanged -= new CSharpFramework.Scene.ShaderEffectConfig.ShaderEffectChangedEventHandler(_shaderFX_OnShaderEffectChanged);
          _shaderFX.Dispose();
        }
        _shaderFX = value;
        if (_shaderFX != null)
        {
          _shaderFX.OnShaderEffectChanged += new CSharpFramework.Scene.ShaderEffectConfig.ShaderEffectChangedEventHandler(_shaderFX_OnShaderEffectChanged);
          _shaderFX.Owner = this;
          _shaderFX.UpdateEffect();
        }
      }
    }

    /// <summary>
    /// The shader effect configuration assigned to the mirror
    /// </summary>
    [SortedCategory(CAT_MIRRORMODEL, CATORDER_MIRRORMODEL), PropertyOrder(5)]
    [Description("The resolution of the render target texture"), DefaultValue(512)]
    public int Resolution
    {
      get { return _iResolution; }
      set
      {
        int iNewVal = value;
        if (iNewVal < 16) iNewVal = 16;
        if (iNewVal > 2048) iNewVal = 2048;
        if (_iResolution == iNewVal)
          return;

        _iResolution = iNewVal;
        if (HasEngineInstance())
          SetEngineInstanceBaseProperties();
      }
    }

    /// <summary>
    /// HDR flag. If true, the format of the render target is set to RGBA16F. Otherwise it is set to RGBA.
    /// </summary>
    [SortedCategory(CAT_MIRRORMODEL, CATORDER_MIRRORMODEL), PropertyOrder(6)]
    [Description("If true, the format of the render target is set to RGBA16F. Otherwise it is set to RGBA. May impact performance and memory.")]
    public bool UseHDR
    {
      get { return _bUseHDR; }
      set
      {
        if (_bUseHDR == value)
          return;

        _bUseHDR = value;
        if (HasEngineInstance())
          SetEngineInstanceBaseProperties();
      }
    }

    [SortedCategory(CAT_ADVANCED, CATORDER_ADVANCED), PropertyOrder(1)]
    [Description("")]
    public RenderOrderBits_e RenderHook
    {
      get { return _renderHook; }
      set
      {
        if (_renderHook == value)
          return;

        _renderHook = value;
        if (HasEngineInstance())
          SetEngineInstanceBaseProperties();
      }
    }

    /// <summary>
    /// overridable for water shape
    /// </summary>
    public virtual void OnShaderEffectChanged()
    {
    }

    private void _shaderFX_OnShaderEffectChanged(object sender, CSharpFramework.Scene.ShaderEffectConfig.ShaderEffectChangedArgs e)
    {
      if (EngineMirror!=null)
        EngineMirror.SetShaderEffect(_shaderFX);
      OnShaderEffectChanged();
    }

    /// <summary>
    /// If >0 this value defines the far clip distance used for rendering the mirrored geometry.
    /// Otherwise the main context's far clip distance is used. 
    /// You can save a lot of performance if this distance is reduced.
    /// </summary>
    [SortedCategory(CAT_VISIBILITY, CATORDER_VISIBILITY),
    PropertyOrder(1),
    Description("If >0 this value defines the far clip distance used for rendering the mirrored geometry. Otherwise the main context's far clip distance is used. You can save a lot of performance if this distance is reduced."),
    DefaultValue(-1f)]
    public float FarClipDistance
    {
      get {return _farClipDist;}
      set 
      {
        _farClipDist=value;
        if (HasEngineInstance())
          EngineMirror.SetFarclipDist(_farClipDist);
      }
    }


    /// <summary>
    /// Entity visibility bitmask that determines the visible status in the rendering contexts
    /// </summary>
    [SortedCategory(CAT_VISIBILITY, CATORDER_VISIBILITY), PropertyOrder(2)]
    [Description("Filter bitmask for rendering the reflections. If set to 'None' reflections are not rendered at all. Choose this option in case the (water-)shader does not use reflections"), DefaultValue(FlagsInt32_e.Bit1)]
    public FlagsInt32_e ContextBitmask
    {
      get {return _iContextBitmask;}
      set
      {
        _iContextBitmask = value;
        if (HasEngineInstance())
          EngineMirror.SetRenderFilterMask((uint)_iContextBitmask, _bRenderExtObj);
      }
    }

    /// <summary>
    /// Entity visibility bitmask that determines the visible status in the rendering contexts
    /// </summary>
    [SortedCategory(CAT_VISIBILITY, CATORDER_VISIBILITY), PropertyOrder(2)]
    [Description("Render extended object types such as terrain vegetation"), DefaultValue(false)]
    public bool RenderExtendedObjects
    {
      get { return _bRenderExtObj; }
      set
      {
        _bRenderExtObj = value;
        if (HasEngineInstance())
          EngineMirror.SetRenderFilterMask((uint)_iContextBitmask, _bRenderExtObj);
      }
    }


    /// <summary>
    /// Enum that defines which shaders are used for reflected surfaces. Use AlwaysSimple for best performace
    /// </summary>
    [SortedCategory(CAT_VISIBILITY, CATORDER_VISIBILITY), PropertyOrder(20)]
    [Description("Enum that defines which shaders are used for reflected surfaces. Use AlwaysSimple for best performace"), DefaultValue(ReflectionShaderMode.AlwaysSimple)]
    public ReflectionShaderMode ReflectionShaderMode
    {
      get { return _reflectionShaderMode; }
      set
      {
        _reflectionShaderMode = value;
        if (HasEngineInstance())
          EngineMirror.SetReflectionShaderMode(_reflectionShaderMode);
      }
    }

    /// <summary>
    /// Sets the FOV scaling factor for the mirror. This value can be used to reduce texture clamping artifacts caused by perturbing the mirror normal in a shader
    /// </summary>
    [SortedCategory(CAT_VISIBILITY, CATORDER_VISIBILITY), PropertyOrder(50)]
    [Description("Sets the FOV scaling factor for the mirror. This value can be used to reduce texture clamping artifacts caused by perturbing the mirror normal in a shader."), DefaultValue(1.0f)]
    public float FovScale
    {
      get { return _fFovScale; }
      set
      {
        _fFovScale = value;
        if (_fFovScale < 0.5f)
          _fFovScale = 0.5f;
        else if (_fFovScale > 1.5f)
          _fFovScale = 1.5f;
        if (HasEngineInstance())
          EngineMirror.SetFovScale(_fFovScale);
      }
    }

    /// <summary>
    /// Can be used to specify an offset for positioning the oblique clipping plane relative to the mirror surface.
    /// Negative values will position the clipping plane below the mirror plane, thus reducing rendering artifacts
    /// e.g. for water rendering close to the shore.
    /// </summary>
    [SortedCategory(CAT_VISIBILITY, CATORDER_VISIBILITY), PropertyOrder(51)]
    [Description("Can be used to specify an offset for positioning the oblique clipping plane relative to the mirror surface. Negative values will position the clipping plane below the mirror plane, thus reducing rendering artifacts e.g. for water rendering close to the shore."),
    DefaultValue(0.0f)]
    public float ObliqueClippingPlaneOffset
    {
      get { return _fObliqueClippingPlaneOffset; }
      set
      {
        _fObliqueClippingPlaneOffset = value;
        if (HasEngineInstance())
          EngineMirror.SetObliqueClippingPlaneOffset(_fObliqueClippingPlaneOffset);
      }
    }

    /// <summary>
    /// Sets whether the LOD behavior from reference context should be used. The default value is false. When enabled,
    /// the LOD instances in the mirror switches in parallel to those in the view context.
    /// </summary>
    [SortedCategory(CAT_VISIBILITY, CATORDER_VISIBILITY), PropertyOrder(52)]
    [Description("Sets whether the LOD behavior from reference context should be used. When enabled, the LOD instances in the mirror switches in parallel to those in the view context."),
     DefaultValue(true)]
    public bool LODFromRefContext
    {
      get { return _bLODFromRefContext; }
      set
      {
        _bLODFromRefContext = value;
        if (HasEngineInstance())
          EngineMirror.SetUseLODFromRefContext(_bLODFromRefContext);
      }
    }


    #endregion

    #region Hotspot

    HotSpotDistance _hotSpotSizeX; // a hotspot for scaling mirror in x-direction
    HotSpotDistance _hotSpotSizeY; // a hotspot for scaling mirror in x-direction

    /// <summary>
    /// Overridden function
    /// </summary>
    public override void OnSelected()
    {
      base.OnSelected ();
      System.Diagnostics.Debug.Assert(_hotSpotSizeX==null);
      System.Diagnostics.Debug.Assert(_hotSpotSizeY==null);

      // hotspot for x-size
      _hotSpotSizeX = new HotSpotDistance(this,@"textures\Hotspot_Rect.dds",VisionColors.Yellow,HotSpotBase.PickType.Square,4.0f);
      _hotSpotSizeX.StartDistance = this.SizeX;
      _hotSpotSizeX.Axis = this.XAxis;
      _hotSpotSizeX.ToolTipText = "Size X";
      _hotSpotSizeX.DisplayScaling = 0.5f * ScaleX;
      EditorManager.ActiveView.HotSpots.Add(_hotSpotSizeX);
      // hotspot for y-size
      _hotSpotSizeY = new HotSpotDistance(this,@"textures\Hotspot_Rect.dds",VisionColors.Yellow,HotSpotBase.PickType.Square,4.0f);
      _hotSpotSizeY.StartDistance = this.SizeY;
      _hotSpotSizeY.Axis = this.YAxis;
      _hotSpotSizeY.ToolTipText = "Size Y";
      _hotSpotSizeY.DisplayScaling = 0.5f * ScaleY;
      EditorManager.ActiveView.HotSpots.Add(_hotSpotSizeY);
    }

    void SafeRemoveHotSpot(HotSpotBase hotSpot)
    {
      if (hotSpot==null) return;
      EditorManager.ActiveView.HotSpots.Remove(hotSpot);
      hotSpot.Remove();
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    public override void OnUnSelected()
    {
      // release the hotspots again:
      SafeRemoveHotSpot(_hotSpotSizeX);  _hotSpotSizeX = null;
      SafeRemoveHotSpot(_hotSpotSizeY);  _hotSpotSizeY = null;

      base.OnUnSelected ();
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    /// <param name="fDeltaX"></param>
    /// <param name="fDeltaY"></param>
    public override void OnHotSpotDrag(CSharpFramework.View.HotSpotBase hotSpot, VisionViewBase view, float fDeltaX, float fDeltaY)
    {
      if (hotSpot==_hotSpotSizeX)
        SizeX = _hotSpotSizeX.CurrentDistance; 
      if (hotSpot==_hotSpotSizeY)
        SizeY = _hotSpotSizeY.CurrentDistance; 
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragBegin(CSharpFramework.View.HotSpotBase hotSpot, VisionViewBase view)
    {
      if (hotSpot==_hotSpotSizeX)
        _hotSpotSizeX.StartDistance = this.SizeX;
      if (hotSpot==_hotSpotSizeY)
        _hotSpotSizeY.StartDistance = this.SizeY;
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragEnd(CSharpFramework.View.HotSpotBase hotSpot, VisionViewBase view)
    {
      if (hotSpot==_hotSpotSizeX)
      {
        if (_hotSpotSizeX.HasChanged)
        {
          float fNewVal = _hotSpotSizeX.CurrentDistance;
          SizeX = _hotSpotSizeX.StartDistance; // set old value for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"SizeX",fNewVal)); 
        }
      }
      if (hotSpot==_hotSpotSizeY)
      {
        if (_hotSpotSizeY.HasChanged)
        {
          float fNewVal = _hotSpotSizeY.CurrentDistance;
          SizeY = _hotSpotSizeY.StartDistance; // set old value for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"SizeY",fNewVal)); 
        }
      }
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="hotSpot"></param>
    public override void OnHotSpotEvaluatePosition(CSharpFramework.View.HotSpotBase hotSpot)
    {
      if (hotSpot==_hotSpotSizeX)
      {
        _hotSpotSizeX.Axis = this.XAxis;
        if (!_hotSpotSizeX.IsDragging)
          _hotSpotSizeX.StartDistance = SizeX;
      }
      if (hotSpot==_hotSpotSizeY)
      {
        _hotSpotSizeY.Axis = this.YAxis;
        if (!_hotSpotSizeY.IsDragging)
          _hotSpotSizeY.StartDistance = SizeY;
      }
    }
    #endregion

    #region Linking

    /// <summary>
    /// Create all link sources and targets provided by this shape
    /// </summary>
    public override void CreateAllLinks()
    {
      base.CreateAllLinks ();
      LinkTargets.Add(new LinkTargetVisiblity(this, "VisibilityTarget_1"));
    }

    /// <summary>
    /// Check whether src can be linked to target. Either src or target has this shape as owner
    /// </summary>
    /// <param name="src">the link source</param>
    /// <param name="target">the link target</param>
    /// <returns></returns>
    public override bool CanLink(ShapeLink src, ShapeLink target)
    {
      if (base.CanLink(src,target))
        return true;
      if (target.OwnerShape==this)
      {
        if (src is LinkSourceVisiblity) 
          return true;
      }
      return false;
    }


    /// <summary>
    /// Perform the actual linking. Either src or target has this shape as owner
    /// </summary>
    /// <param name="src">the link source</param>
    /// <param name="target">the link target</param>
    public override void OnLink(ShapeLink src, ShapeLink target)
    {
      base.OnLink (src, target);

      // perform the linking on the engine objects
      if (target.OwnerShape==this)
      {
        VisibilityObjectShape visobjShape = src.OwnerShape as VisibilityObjectShape;
        if (visobjShape!=null && visobjShape._engineInstance!=null) // at least the engine instance of the constraint can be null
          EngineMirror.AddVisibilityObject(visobjShape._engineInstance as EngineInstanceVisibilityObject);
      }
    }

    /// <summary>
    /// unlinks a target from a source. Either src or target has this shape as owner
    /// </summary>
    /// <param name="src">the link source</param>
    /// <param name="target">the link target</param>
    public override void OnUnlink(ShapeLink src, ShapeLink target)
    {
      base.OnUnlink (src, target);
      // perform the unlinking on the engine objects
      if (target.OwnerShape==this)
      {
        VisibilityObjectShape visobjShape = src.OwnerShape as VisibilityObjectShape;
        if (visobjShape!=null && visobjShape._engineInstance!=null) // at least the engine instance of the constraint can be null
          EngineMirror.RemoveVisibilityObject(visobjShape._engineInstance as EngineInstanceVisibilityObject);
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
      return EditorPlugin.EDITOR_PLUGIN_INFO;
    }

    #endregion
  }

  #endregion

  #region Mirror creator Plugin

  class MirrorCreator : CSharpFramework.IShapeCreatorPlugin
  {
    public MirrorCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_mirror.bmp"), Color.Magenta);
    }

    public override string GetPluginName()
    {
      return "Mirror";
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
      return "Adds a mirror to the scene for realtime reflections.\n"+
        "A shader has to be applied to the mirror.\n"+
        "A mirror is quite performance intensive since it renders the scene again.\n";
    }

    public override ShapeBase CreateShapeInstance()
    {
      MirrorShape shape = new MirrorShape("Mirror");
      shape.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
      //assign a default effect:
      if (EditorManager.EngineManager.IsInitialized())
      {
        shape.ShaderEffect.EffectLibrary = @"\Shaders\Mirror.Shaderlib";
        shape.ShaderEffect.EffectName = "SimpleMirror";
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
