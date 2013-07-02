/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.IO;
using System.ComponentModel;
using System.Drawing;
using System.Globalization;
using System.Drawing.Design;
using System.Runtime.Serialization;
using CSharpFramework;
using ManagedFramework;
using CSharpFramework.Math;
using CSharpFramework.Shapes;
using CSharpFramework.PropertyEditors;
using CSharpFramework.DynamicProperties;
using CSharpFramework.UndoRedo;
using CSharpFramework.Actions;
using CSharpFramework.Scene;
using CSharpFramework.View;
using VisionManaged;
using CSharpFramework.Serialization;

namespace VisionEditorPlugin.Shapes
{

  #region Class : ProjectorShape

	/// <summary>
	/// Shape that defines a projector (projected wallmark class in the engine)
	/// </summary>
  [DefaultProperty("Engine Class"),
   Serializable,
   ShapeTypeNiceName("Projector")
  ]
  public class ProjectorShape : Shape3D
	{
    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_PROJECTOR = "Projector";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_PROJECTOR = Shape3D.LAST_CATEGORY_ORDER_ID + 1;

    /// <summary>
    /// last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_PROJECTOR;

    #endregion

    #region Constructor

    /// <summary>
    /// Shape constructor
    /// </summary>
    /// <param name="name"></param>
		public ProjectorShape(string name) : base(name)
		{
      this._fFadeOutDist *= EditorManager.Settings.GlobalUnitScaling;
      this._fLength *= EditorManager.Settings.GlobalUnitScaling;
		}

    ~ProjectorShape()
    {
      if (_shaderFX!=null)
        _shaderFX.OnShaderEffectChanged -= new ShaderEffectConfig.ShaderEffectChangedEventHandler(_shaderFX_OnShaderEffectChanged);
    }

    #endregion

    #region GUI

    /// <summary>
    /// Overridden function to get the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetIconIndex()
    {
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_projector.bmp"), Color.Magenta);
    }

    #endregion

    #region Engine Instance

    /// <summary>
    /// Overridden function to create the engine instance
    /// </summary>
    /// <param name="bCreateChildren"></param>
    public override void CreateEngineInstance(bool bCreateChildren)
    {
      base.CreateEngineInstance (bCreateChildren);
      this._engineInstance = new EngineInstanceProjector();
      SetEngineInstanceBaseProperties(); // sets the position etc.
    }

 
    /// <summary>
    /// helper function to access the engine instance as EngineInstanceProjector
    /// </summary>
    [BrowsableAttribute(false)]
    public EngineInstanceProjector EngineProjector
    {
      get {return (EngineInstanceProjector)_engineInstance;}
    }

    /// <summary>
    /// Overridden function to set all shape's properties (color, ...) on the engine instance
    /// </summary>
    public override void SetEngineInstanceBaseProperties()
    {
      base.SetEngineInstanceBaseProperties ();
      if (_engineInstance==null)
        return;

      EngineProjector.SetTexture(_textureFile);
      EngineProjector.SetTransparency(_transp);
      EngineProjector.SetConeAngles(_fConeAngleX,_fConeAngleY);
      EngineProjector.SetLength(_fLength * UniformScaling);
      EngineProjector.SetFadeOutRange(_fFadeOutDist * UniformScaling);
      EngineProjector.SetColor(VisionColors.RGBA(_color.R, _color.G, _color.B, _iIntensity));
      EngineProjector.SetLightmapped(_bLightmapped);
      EngineProjector.SetGeometryTypeMask(_geomTypeFilter);
      EngineProjector.SetFarClipDistance(_fFarClipDistance);
      EngineProjector.SetInfluenceBitmask((uint)_iInfluenceBitmask);
      EngineProjector.SetCustomShaderEffect(_shaderFX);
    }

    /// <summary>
    /// Overridden function to render the shape as preview
    /// </summary>
    /// <param name="view"></param>
    /// <param name="mode"></param>
    public override void RenderShape(VisionViewBase view, ShapeRenderMode mode)
    {
      base.RenderShape (view, mode);
      if (_engineInstance!=null)
        EngineProjector.RenderShape(view,mode);
    }


    public override void OnTransformationChanged()
    {
      base.OnTransformationChanged();

      // scaling might have changed
      if (HasEngineInstance())
      {
        EngineProjector.SetLength(_fLength * UniformScaling);
        EngineProjector.SetFadeOutRange(_fFadeOutDist * UniformScaling);
      }
      if (_hotSpotLength != null)
        _hotSpotLength.DisplayScaling = UniformScaling;
      if (_hotSpotFadeOutDist1 != null)
        _hotSpotFadeOutDist1.DisplayScaling = UniformScaling;
      if (_hotSpotFadeOutDist2 != null)
        _hotSpotFadeOutDist2.DisplayScaling = UniformScaling;
    }

    #endregion

    #region Clone

    /// <summary>
    /// Overridden clone function that removes the hotspots again
    /// </summary>
    /// <returns></returns>
    public override ShapeBase Clone()
    {
      ProjectorShape copy = base.Clone() as ProjectorShape;
      copy._hotSpotConeAngleX = null;
      copy._hotSpotConeAngleY = null;
      copy._hotSpotLength = null;
      copy._hotSpotFadeOutDist1 = null;
      copy._hotSpotFadeOutDist2 = null;

      if (copy._shaderFX != null)
        this._shaderFX = (ShaderEffectConfig)copy._shaderFX.Clone();

      return copy;
    }


    #endregion

    #region ISerializable

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected ProjectorShape(SerializationInfo info, StreamingContext context) : base(info, context)
    {
      _textureFile = info.GetString("_textureFile");
      _transp = (TransparencyType)info.GetValue("_transp",typeof(TransparencyType));
      _fConeAngleX = info.GetSingle("_fConeAngleX");
      _fConeAngleY = info.GetSingle("_fConeAngleY");
      _fLength = info.GetSingle("_fLength");
      _fFadeOutDist = info.GetSingle("_fFadeOutDist");
      _iIntensity = info.GetByte("_iIntensity");
      _color = (Color)info.GetValue("_color",typeof(Color));
      if (SerializationHelper.HasElement(info,"_bLightmapped"))
        _bLightmapped = info.GetBoolean("_bLightmapped");
      if (SerializationHelper.HasElement(info, "_geomTypeFilter"))
        _geomTypeFilter = (GeometryTypeMask)info.GetValue("_geomTypeFilter", typeof(GeometryTypeMask));
      else _geomTypeFilter = GeometryTypeMask.All; // backwards compatibility
      if (SerializationHelper.HasElement(info, "_fFarClipDistance"))
        _fFarClipDistance = info.GetSingle("_fFarClipDistance");
      // backwards compatibility
      if (!SerializationHelper.HasElement(info, "SupportScaling"))
        SetScaling_Internal(1.0f, 1.0f, 1.0f);
      if (SerializationHelper.HasElement(info, "_iInfluenceBitmask"))
        _iInfluenceBitmask = (FlagsInt32_e)info.GetValue("_iInfluenceBitmask", typeof(FlagsInt32_e));
      else
        _iInfluenceBitmask = FlagsInt32_e.All;
      if (SerializationHelper.HasElement(info, "_shaderFX"))
        _shaderFX = (ShaderEffectConfig)info.GetValue("_shaderFX", typeof(ShaderEffectConfig));
      
    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context)
    {
      base.GetObjectData (info, context);
      info.AddValue("_textureFile",_textureFile);
      info.AddValue("_transp",_transp);
      info.AddValue("_fConeAngleX",_fConeAngleX);
      info.AddValue("_fConeAngleY",_fConeAngleY);
      info.AddValue("_fLength",_fLength);
      info.AddValue("_fFadeOutDist",_fFadeOutDist);
      info.AddValue("_iIntensity",_iIntensity);
      info.AddValue("_color",_color);
      info.AddValue("_bLightmapped",_bLightmapped);
      info.AddValue("_geomTypeFilter", _geomTypeFilter);
      info.AddValue("_fFarClipDistance", _fFarClipDistance);
      info.AddValue("SupportScaling", true);
      info.AddValue("_iInfluenceBitmask", _iInfluenceBitmask);
      info.AddValue("_shaderFX", _shaderFX);
    }


    public override void OnDeserialization()
    {
      base.OnDeserialization();
      if (_shaderFX != null)
      {
        _shaderFX.OnShaderEffectChanged += new ShaderEffectConfig.ShaderEffectChangedEventHandler(_shaderFX_OnShaderEffectChanged);
        _shaderFX.Owner = this;
      }
    }


    #endregion

    #region Shape3D Members

    /// <summary>
    /// Configures this shape to use scene graph based parenting
    /// </summary>
    protected override Shape3D.ParentingModes ParentingMode
    {
      get { return ParentingModes.SceneGraphScaled; }
    }


    public override void OnAddedToScene()
    {
      base.OnAddedToScene();
      AddHint(HintFlags_e.OnlyUniformScale);
    }

    #endregion

    #region Selection and hot spots

    HotSpotConeAngle _hotSpotConeAngleX; // a hotspot for the cone angle in x direction
    HotSpotConeAngle _hotSpotConeAngleY; // a hotspot for the cone angle in y direction
    HotSpotDistance _hotSpotLength;      // a hotspot for the cone length
    HotSpotDistance _hotSpotFadeOutDist1; // a hotspot for the cone fade-out-dist
    HotSpotDistance _hotSpotFadeOutDist2; // a hotspot for the cone fade-out-dist

    /// <summary>
    /// Overridden function
    /// </summary>
    public override void OnSelected()
    {
      base.OnSelected ();

      System.Diagnostics.Debug.Assert(_hotSpotConeAngleX==null);
      System.Diagnostics.Debug.Assert(_hotSpotConeAngleY==null);
      System.Diagnostics.Debug.Assert(_hotSpotLength==null);
      System.Diagnostics.Debug.Assert(_hotSpotFadeOutDist1==null);
      System.Diagnostics.Debug.Assert(_hotSpotFadeOutDist2==null);

      // create the hotspots:

      // hotspot for cone angle x
      _hotSpotConeAngleX = new HotSpotConeAngle(this,@"textures\Hotspot_A.tga",VisionColors.Red,HotSpotBase.PickType.Square,4.0f);
      _hotSpotConeAngleX.Distance = 50.0f * EditorManager.Settings.GlobalUnitScaling;
      _hotSpotConeAngleX.StartAngle = this.ConeAngleX;
      _hotSpotConeAngleX.ToolTipText = "cone angle x";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotConeAngleX);

      // hotspot for cone angle y
      _hotSpotConeAngleY = new HotSpotConeAngle(this,@"textures\Hotspot_A.tga",VisionColors.Red,HotSpotBase.PickType.Square,4.0f);
      _hotSpotConeAngleY.Distance = 50.0f * EditorManager.Settings.GlobalUnitScaling;
      _hotSpotConeAngleY.UseOwnerUpDirection = true;
      _hotSpotConeAngleY.StartAngle = this.ConeAngleY;
      _hotSpotConeAngleY.ToolTipText = "cone angle y";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotConeAngleY);

      // hotspot for cone length
      _hotSpotLength = new HotSpotDistance(this,@"textures\Hotspot_L.tga",VisionColors.Blue,HotSpotBase.PickType.Square,4.0f);
      _hotSpotLength.StartDistance = this.Length;
      _hotSpotLength.DisplayScaling = UniformScaling;
      _hotSpotLength.Axis = this.XAxis;
      _hotSpotLength.ToolTipText = "length";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotLength);

      // hotspot for fade out distance
      _hotSpotFadeOutDist1 = new HotSpotDistance(this,@"textures\Hotspot_D.tga",VisionColors.Blue,HotSpotBase.PickType.Square,4.0f);
      _hotSpotFadeOutDist1.StartDistance = this.Length + this.FadeOutDistance;
      _hotSpotFadeOutDist1.Axis = this.XAxis;
      _hotSpotFadeOutDist1.DisplayScaling = UniformScaling;
      _hotSpotFadeOutDist1.ToolTipText = "fade out distance";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotFadeOutDist1);

      // hotspot for fade out distance
      _hotSpotFadeOutDist2 = new HotSpotDistance(this,@"textures\Hotspot_D.tga",VisionColors.Blue,HotSpotBase.PickType.Square,4.0f);
      _hotSpotFadeOutDist2.StartDistance = this.Length - this.FadeOutDistance;
      _hotSpotFadeOutDist2.Axis = this.XAxis;
      _hotSpotFadeOutDist2.DisplayScaling = UniformScaling;
      _hotSpotFadeOutDist2.ToolTipText = "fade out distance";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotFadeOutDist2);

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
      SafeRemoveHotSpot(_hotSpotConeAngleX);  _hotSpotConeAngleX = null;
      SafeRemoveHotSpot(_hotSpotConeAngleY);  _hotSpotConeAngleY = null;
      SafeRemoveHotSpot(_hotSpotLength);      _hotSpotLength = null;
      SafeRemoveHotSpot(_hotSpotFadeOutDist1); _hotSpotFadeOutDist1 = null;
      SafeRemoveHotSpot(_hotSpotFadeOutDist2); _hotSpotFadeOutDist2 = null;


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
      if (hotSpot==_hotSpotConeAngleX)
        ConeAngleX = _hotSpotConeAngleX.CurrentAngle; // update for the view
      if (hotSpot==_hotSpotConeAngleY)
        ConeAngleY = _hotSpotConeAngleY.CurrentAngle; 
      if (hotSpot==_hotSpotLength)
        Length = _hotSpotLength.CurrentDistance; 
      if (hotSpot==_hotSpotFadeOutDist1)
        FadeOutDistance = _hotSpotFadeOutDist1.CurrentDistance - _hotSpotLength.CurrentDistance; 
      if (hotSpot==_hotSpotFadeOutDist2)
        FadeOutDistance = _hotSpotLength.CurrentDistance - _hotSpotFadeOutDist2.CurrentDistance; 
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragBegin(CSharpFramework.View.HotSpotBase hotSpot, VisionViewBase view)
    {
      if (hotSpot==_hotSpotConeAngleX)
        _hotSpotConeAngleX.StartAngle = this.ConeAngleX;
      if (hotSpot==_hotSpotConeAngleY)
        _hotSpotConeAngleY.StartAngle = this.ConeAngleY;
      if (hotSpot==_hotSpotLength)
        _hotSpotLength.StartDistance = this.Length;
      if (hotSpot==_hotSpotFadeOutDist1)
        _hotSpotFadeOutDist1.StartDistance = this.Length + this.FadeOutDistance;
      if (hotSpot==_hotSpotFadeOutDist2)
        _hotSpotFadeOutDist2.StartDistance = this.Length - this.FadeOutDistance;
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragEnd(CSharpFramework.View.HotSpotBase hotSpot, VisionViewBase view)
    {
      if (hotSpot==_hotSpotConeAngleX)
      {
        if (_hotSpotConeAngleX.HasChanged)
        {
          float fNewAngle = _hotSpotConeAngleX.CurrentAngle;
          ConeAngleX = _hotSpotConeAngleX.StartAngle; // set old angle for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"ConeAngleX",fNewAngle)); 
        }
      }
      if (hotSpot==_hotSpotConeAngleY)
      {
        if (_hotSpotConeAngleY.HasChanged)
        {
          float fNewAngle = _hotSpotConeAngleY.CurrentAngle;
          ConeAngleY = _hotSpotConeAngleY.StartAngle; // set old angle for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"ConeAngleY",fNewAngle));
        }
      }      
      if (hotSpot==_hotSpotLength)
      {
        if (_hotSpotLength.HasChanged)
        {
          float fNewDist = _hotSpotLength.CurrentDistance;
          Length = _hotSpotLength.StartDistance; // set old angle for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"Length",fNewDist)); 
        }
      }
      if (hotSpot==_hotSpotFadeOutDist1)
      {
        if (_hotSpotFadeOutDist1.HasChanged)
        {
          float fNewDist = _hotSpotFadeOutDist1.CurrentDistance - _hotSpotLength.CurrentDistance;
          FadeOutDistance = _hotSpotFadeOutDist1.StartDistance - _hotSpotLength.CurrentDistance;; 
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"FadeOutDistance",fNewDist)); 
        }
      }
      if (hotSpot==_hotSpotFadeOutDist2)
      {
        if (_hotSpotFadeOutDist2.HasChanged)
        {
          float fNewDist = _hotSpotLength.CurrentDistance - _hotSpotFadeOutDist2.CurrentDistance;
          FadeOutDistance = _hotSpotLength.CurrentDistance - _hotSpotFadeOutDist2.StartDistance; 
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"FadeOutDistance",fNewDist)); 
        }
      }
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="hotSpot"></param>
    public override void OnHotSpotEvaluatePosition(CSharpFramework.View.HotSpotBase hotSpot)
    {
      if (hotSpot==_hotSpotConeAngleX)
      {
        if (!_hotSpotConeAngleX.IsDragging)
          _hotSpotConeAngleX.StartAngle = ConeAngleX;
      }
      if (hotSpot==_hotSpotConeAngleY)
      {
        if (!_hotSpotConeAngleY.IsDragging)
          _hotSpotConeAngleY.StartAngle = ConeAngleY;
      }
      if (hotSpot==_hotSpotLength)
      {
        _hotSpotLength.Axis = this.XAxis;
        if (!_hotSpotLength.IsDragging)
          _hotSpotLength.StartDistance = Length;
      }
      if (hotSpot==_hotSpotFadeOutDist1)
      {
        _hotSpotFadeOutDist1.Axis = this.XAxis;
        if (!_hotSpotFadeOutDist1.IsDragging)
          _hotSpotFadeOutDist1.StartDistance = FadeOutDistance + Length;
      }
      if (hotSpot==_hotSpotFadeOutDist2)
      {
        _hotSpotFadeOutDist2.Axis = this.XAxis;
        if (!_hotSpotFadeOutDist2.IsDragging)
          _hotSpotFadeOutDist2.StartDistance = Length - FadeOutDistance;
      }
    }

    /// <summary>
    /// Called every frame and per hotspot that belongs to the shape
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    /// <param name="mode"></param>
    public override void OnHotSpotRender(HotSpotBase hotSpot, VisionViewBase view, ShapeRenderMode mode)
    {
      base.OnHotSpotRender (hotSpot, view, mode);

      Vector3F vStart = _hotSpotFadeOutDist1.Position;
      Vector3F vEnd = _hotSpotFadeOutDist2.Position;
      
      view.RenderLine(vStart, vEnd, VisionColors.Yellow, 1.0f);
    }

    #endregion


    #region Projector properties

    string _textureFile = @"\perlinnoise.dds";
    TransparencyType _transp = TransparencyType.AlphaBlended;
    float _fConeAngleX = 45.0f;
    float _fConeAngleY = 45.0f;
    float _fLength = 400.0f;
    float _fFadeOutDist = 100.0f;
    byte _iIntensity = 255;
    Color _color = Color.White;
    bool _bLightmapped = false;
    GeometryTypeMask _geomTypeFilter = GeometryTypeMask.All & (~GeometryTypeMask.Terrain);
    float _fFarClipDistance = 0.0f;

    FlagsInt32_e _iInfluenceBitmask = FlagsInt32_e.All;
    ShaderEffectConfig _shaderFX;


    /// <summary>
    /// Gets or sets the texture filename
    /// </summary>
    [PrefabResolveFilename]
    [SortedCategory(CAT_PROJECTOR, CATORDER_PROJECTOR), PropertyOrder(1),
    EditorAttribute(typeof(AssetEditor), typeof(UITypeEditor)), AssetDialogFilter(new string[] { "Texture" }),
    Description("Texture file (must have 1 texel border).")]
    public string TextureFile
    {
      get {return _textureFile;}
      set 
      {
        _textureFile = value;
        if (HasEngineInstance())
          EngineProjector.SetTexture(_textureFile);
      }
    }

    /// <summary>
    /// Gets or sets the transparency type of the projected texture
    /// </summary>
    [SortedCategory(CAT_PROJECTOR, CATORDER_PROJECTOR), PropertyOrder(2),
    Description("Transparency type of the projected texture"), DefaultValue(TransparencyType.AlphaBlended)]
    public TransparencyType Transparency
    {
      get {return _transp;}
      set 
      {
        _transp=value;
        if (HasEngineInstance())
          EngineProjector.SetTransparency(_transp);
      }
    }

    /// <summary>
    /// Gets or sets the cone angle size in x direction
    /// </summary>
    [SortedCategory(CAT_PROJECTOR, CATORDER_PROJECTOR), PropertyOrder(3),
    Description("Cone opening angle in x direction"), DefaultValue(45f)]
    public float ConeAngleX
    {
      get {return _fConeAngleX;}
      set 
      {
        _fConeAngleX = value;
        if (HasEngineInstance())
          EngineProjector.SetConeAngles(_fConeAngleX, _fConeAngleY);
      }
    }

    /// <summary>
    /// Gets or sets the cone angle size in y direction
    /// </summary>
    [SortedCategory(CAT_PROJECTOR, CATORDER_PROJECTOR), PropertyOrder(4),
    Description("Cone opening angle in y direction"), DefaultValue(45f)]
    public float ConeAngleY
    {
      get {return _fConeAngleY;}
      set 
      {
        _fConeAngleY = value;
        if (HasEngineInstance())
          EngineProjector.SetConeAngles(_fConeAngleX, _fConeAngleY);
      }
    }
    /// <summary>
    /// Gets or sets the length of the projection
    /// </summary>
    [SortedCategory(CAT_PROJECTOR, CATORDER_PROJECTOR), PropertyOrder(5),
    Description("Length in world units"), DefaultValue(400f)]
    public float Length
    {
      get {return _fLength;}
      set 
      {
        _fLength = value;
        if (HasEngineInstance())
          EngineProjector.SetLength(_fLength * UniformScaling);
      }
    }
    /// <summary>
    /// Gets or sets the fade-out distance around the target position
    /// </summary>
    [SortedCategory(CAT_PROJECTOR, CATORDER_PROJECTOR), PropertyOrder(6),
    Description("Fade-out distance in world units around the target position"), DefaultValue(100f)]
    public float FadeOutDistance
    {
      get {return _fFadeOutDist;}
      set 
      {
        _fFadeOutDist = value;
        if (HasEngineInstance())
          EngineProjector.SetFadeOutRange(_fFadeOutDist * UniformScaling);
      }
    }

    /// <summary>
    /// Gets or sets the intensity of the projected map
    /// </summary>
    [SortedCategory(CAT_PROJECTOR, CATORDER_PROJECTOR), PropertyOrder(7),
    Description("Intensity of the projected map"), DefaultValue((byte)255)]
    public byte Intensity
    {
      get {return _iIntensity;}
      set 
      {
        _iIntensity = value;
        if (HasEngineInstance())
          EngineProjector.SetColor(VisionColors.RGBA(_color.R, _color.G, _color.B, _iIntensity));
      }
    }

    /// <summary>
    /// Gets or sets the color of the projected map
    /// </summary>
    [SortedCategory(CAT_PROJECTOR, CATORDER_PROJECTOR), PropertyOrder(8),
    Description("Color of the projected map")]
    [EditorAttribute(typeof(StandardColorDropDownEditor), typeof(UITypeEditor))]
    [PropertyLiveUpdate("ColorLiveUpdate"), DefaultValue(typeof(Color), "255,255,255")]
    public Color Color
    {
      get {return _color;}
      set 
      {
        if (_color==value) return;
        _color = value;
        if (_engineInstance!=null)
          EngineProjector.SetColor(VisionColors.RGBA(_color.R, _color.G, _color.B, _iIntensity));
      }
    }

    /// <summary>
    /// Proxy property used for color live update (See PropertyLiveUpdate attribute in LightColor property)
    /// </summary>
    [Browsable(false)]
    public object ColorLiveUpdate
    {
      set
      {
        Color col = _color;
        if (value is Color)
          col = (Color)value;
        if (HasEngineInstance())
          EngineProjector.SetColor(VisionColors.RGBA(col.R, col.G, col.B, _iIntensity));
        EditorManager.ActiveView.UpdateView(false);
      }
    }
    /// <summary>
    /// Gets or sets the lightmap status
    /// </summary>
    [SortedCategory(CAT_PROJECTOR, CATORDER_PROJECTOR), PropertyOrder(9),
    Description("If enabled, the projector is influenced by the geometry's static light maps"), DefaultValue(false)]
    public bool Lightmapped
    {
      get {return _bLightmapped;}
      set 
      {
        _bLightmapped = value;
        if (HasEngineInstance())
          EngineProjector.SetLightmapped(_bLightmapped);
      }
    }

    [Editor(typeof(FlagsEditor), typeof(System.Drawing.Design.UITypeEditor))]
    [SortedCategory(CAT_PROJECTOR, CATORDER_PROJECTOR), PropertyOrder(10)]
    [Description("Defines which types of geometry may potentilly be affected by this projector"), DefaultValue(GeometryTypeMask.Entities | GeometryTypeMask.StaticMeshes)]
    public GeometryTypeMask Affects
    {
      get { return _geomTypeFilter; }
      set 
      {
        _geomTypeFilter = value;
        if (HasEngineInstance())
          EngineProjector.SetGeometryTypeMask(_geomTypeFilter);
      }
    }

    /// <summary>
    ///  Affecet Meshes bitmask that determines the affect status
    /// </summary>
    [SortedCategory(CAT_PROJECTOR, CATORDER_PROJECTOR), PropertyOrder(11)]
    [Description("Influence bitmask that filters affected meshes by comparison with their VisibilityBitmask"), DefaultValue(FlagsInt32_e.All)]
    public FlagsInt32_e InfluenceBitmask
    {
      get { return _iInfluenceBitmask; }
      set
      {
        _iInfluenceBitmask = value;
        if (HasEngineInstance())
          EngineProjector.SetInfluenceBitmask((uint)_iInfluenceBitmask);
      }
    }

    /// <summary>
    /// Gets or sets the far clip distance
    /// </summary>
    [SortedCategory(CAT_PROJECTOR, CATORDER_PROJECTOR), PropertyOrder(12),
    Description("Far clip distance at which this projector is not rendered anymore. 0 means no clipping"), DefaultValue(0f)]
    public float FarClipDistance
    {
      get { return _fFarClipDistance; }
      set
      {
        _fFarClipDistance = value;
        if (_fFarClipDistance < 0.0f)
          _fFarClipDistance = 0.0f;
        if (HasEngineInstance())
          EngineProjector.SetFarClipDistance(_fFarClipDistance);
      }
    }

    [SortedCategory(CAT_PROJECTOR, CATORDER_PROJECTOR), PropertyOrder(20)]
    [Description("A custom shader effect can be specified here. All passes in the shader must be of class 'VProjectorShaderPass'")]
    public ShaderEffectConfig CustomShaderEffect
    {
      get
      {
        if (_shaderFX == null)
        {
          _shaderFX = new ShaderEffectConfig(this);
          _shaderFX.OnShaderEffectChanged += new ShaderEffectConfig.ShaderEffectChangedEventHandler(_shaderFX_OnShaderEffectChanged);
        }
        return _shaderFX;
      }
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

    void _shaderFX_OnShaderEffectChanged(object sender, ShaderEffectConfig.ShaderEffectChangedArgs e)
    {
      if (HasEngineInstance())
        EngineProjector.SetCustomShaderEffect(_shaderFX);
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

  #region ProjectorShape Creator Plugin

  class ProjectorShapeCreator : CSharpFramework.IShapeCreatorPlugin
  {
    public ProjectorShapeCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_projector.bmp"), Color.Magenta);
    }

    public override string GetPluginName()
    {
      return "Projector";
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
      return "Adds a static projector to the scene. A projector projects a texture onto world geometry.\n"+
        "Alternatively dynamic lights can be used (which can also affect models and can cast shadows) "+
        "but projectors are rendered faster by the engine.";
    }

    public override ShapeBase CreateShapeInstance()
    {
      ProjectorShape projector = new ProjectorShape("Projector");
      projector.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
      return projector;
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
