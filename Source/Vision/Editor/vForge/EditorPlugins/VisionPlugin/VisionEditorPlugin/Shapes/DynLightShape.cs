/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.IO;
using System.Collections;
using System.Drawing.Design;
using System.ComponentModel;
using System.Globalization;
using System.Runtime.Serialization;
using CSharpFramework;
using ManagedFramework;
using CSharpFramework.Math;
using CSharpFramework.Shapes;
using CSharpFramework.View;
using CSharpFramework.Scene;
using CSharpFramework.PropertyEditors;
using System.Drawing;
using CSharpFramework.UndoRedo;
using CSharpFramework.Actions;
using CSharpFramework.Serialization;
using VisionManaged;
using VisionEditorPlugin.Dialogs;
using System.Windows.Forms;


namespace VisionEditorPlugin.Shapes
{
	/// <summary>
	/// Shape that describes a dynamic light source.
	/// </summary>
  [DefaultProperty("Engine Class"),
   Serializable,
   ShapeTypeNiceName("Lightsource")
  ]
  public class DynLightShape : ShapeObject3D, ILightSource
  {
    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_STATICLIGHTING   = "Static Lighting";

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_APPEARANCE   = "Base Light Properties";

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_INFLUENCE    = "Influence";

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_ANIMATIONS   = "Animations";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_STATICLIGHTING = Shape3D.LAST_CATEGORY_ORDER_ID + 1;

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_APPEARANCE = Shape3D.LAST_CATEGORY_ORDER_ID + 2;

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_INFLUENCE  = Shape3D.LAST_CATEGORY_ORDER_ID + 3;

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_ANIMATIONS = Shape3D.LAST_CATEGORY_ORDER_ID + 4;

    /// <summary>
    /// last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_ANIMATIONS;

    #endregion

    /// <summary>
    /// Old Light type; Use LightSourceType_e instead
    /// </summary>
    private enum LightType_e
    {
      /// <summary>
      /// Light is an omni light
      /// </summary>
      Point = LightSourceType_e.Point,

      /// <summary>
      /// Light is a spotlight
      /// </summary>
      Spotlight = LightSourceType_e.Spotlight,

      /// <summary>
      /// Light is directional
      /// </summary>
      Directional = LightSourceType_e.Directional,
    };

    #region Nested Classes: Light type specific properties
    
    /// <summary>
    /// LightTypePropertiesBase : Base class of specific light type properties. Allows to set the properties of the parent,
    /// thus it needs the owner parent in the constructor
    /// </summary>
    public class LightTypePropertiesBase
    {
      /// <summary>
      /// Constructor
      /// </summary>
      /// <param name="_owner"></param>
      public LightTypePropertiesBase(DynLightShape _owner) {Owner=_owner;}

      /// <summary>
      /// Owner shape
      /// </summary>
      protected DynLightShape Owner;
    }

    /// <summary>
    /// specific omni light properties
    /// </summary>
    public class LightTypeProperties_OmniLight : LightTypePropertiesBase
    {
      /// <summary>
      /// Constructor
      /// </summary>
      /// <param name="_owner"></param>
      public LightTypeProperties_OmniLight(DynLightShape _owner) : base(_owner) {}

      /// <summary>
      /// get or set the projected texture filename
      /// </summary>
      [PropertyOrder(5), EditorAttribute(typeof(AssetEditor), typeof(UITypeEditor)), AssetDialogFilter(new string[] { "Texture" })]
      [Description("Texture used for projection by the light.")]
      public string ProjectedCubemap { get { return Owner.ProjectedTexture; } set { Owner.ProjectedTexture = value; } }
    }

    /// <summary>
    /// specific spot light properties
    /// </summary>
    public class LightTypeProperties_SpotLight : LightTypePropertiesBase
    {
      /// <summary>
      /// Constructor
      /// </summary>
      /// <param name="_owner"></param>
      public LightTypeProperties_SpotLight(DynLightShape _owner) : base(_owner) {}

      /// <summary>
      /// get or set the spot cone angle
      /// </summary>
      [PropertyOrder(4), Description("The cone angle of the spot light")]
      public float SpotAngle {get {return Owner.SpotAngle;} set {Owner.SpotAngle=value;}}

      /// <summary>
      /// get or set the projected texture filename
      /// </summary>
      [PropertyOrder(5), EditorAttribute(typeof(AssetEditor), typeof(UITypeEditor)), AssetDialogFilter(new string[] { "Texture" })]
      [Description("Texture used for projection by the light.")]
      public string ProjectedTexture { get { return Owner.ProjectedTexture; } set { Owner.ProjectedTexture = value; } }
    }

    /// <summary>
    /// specific directional light properties
    /// </summary>
    public class LightTypeProperties_Directional : LightTypePropertiesBase
    {
      /// <summary>
      /// Constructor
      /// </summary>
      /// <param name="_owner"></param>
      public LightTypeProperties_Directional(DynLightShape _owner) : base(_owner) {}
    }

    #endregion

    #region Constructor

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="nodeName">name of the shape</param>
    public DynLightShape(string nodeName) : base(nodeName)
    {
      // because it is now a radius
      this._fIntensity *= EditorManager.Settings.GlobalUnitScaling;
    }

    /// <summary>
    /// Destructor
    /// </summary>
    ~DynLightShape()
    {
    }

    #endregion

    #region GUI

    /// <summary>
    /// Overridden function to get the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetIconIndex()
    {
      switch (this.LightType)
      {
        case LightSourceType_e.Spotlight:
          return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\flashlight.bmp"), Color.Magenta);
        case LightSourceType_e.Point:
          return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\Pointlight.bmp"), Color.Magenta);
      }
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\Pointlight.bmp"), Color.Magenta);
    }

    #endregion

    #region Shape members

    /// <summary>
    /// Create the engine instance
    /// </summary>
    /// <param name="bCreateChildren"></param>
    public override void CreateEngineInstance(bool bCreateChildren)
    {
      this._engineInstance = new EngineInstanceDynLight();
      SetEngineInstanceBaseProperties(); // sets the position etc.
      SetTypeSpecificProperties(); // ...in the grid

      // Call the base function to setup attachments and process the children. The engine instance
      // has to exist at this point.
      base.CreateEngineInstance(bCreateChildren);
    }

    [Browsable(false)]
    public float FinalRadius
    {
      get
      {
        return Intensity * Math.Abs(UniformScaling); // abs is better than clamping(?)
      }
    }

    /// <summary>
    /// overridden function to set/update all specific properties on the engine instance
    /// </summary>
    public override void SetEngineInstanceBaseProperties()
    {
      if (_engineInstance==null)
        return;
      base.SetEngineInstanceBaseProperties();
      EngineLight.SetLightType(_lightType, _fSpotAngle, _sProjectedTexture);
      EngineLight.SetIntensity(FinalRadius);
      EngineLight.SetLightColor(VisionColors.Get(_lightColor));
      EngineLight.SetCastShadows(_bCastStaticShadows,_bDeprecatedModelShadows);
      EngineLight.SetInfluenceBitMasks(_iWorldInfluence,_iObjectInfluence);
      EngineLight.SetVisibleBitmask((uint)_iVisibleBitmask);
      EngineLight.SetAnimationLookup(_colorAnimTable);
      EngineLight.SetAnimationTiming(_fAnimTiming);
      EngineLight.SetAnimationPhase(_fAnimPhase);
      EngineLight.SetRemoveLightWhenFinished(_bRemoveLightAfterAnim);
      EngineLight.SetExportAsStatic(_bStatic);
      EngineLight.SetPreviewStatic(_bPreviewStatic && !_bBakedToLightmap);
      EngineLight.SetExportWhenNotRelevant(_bExportWhenNotRelevant);
      EngineLight.SetAttenuation(EngineInstanceDynLight.LightAttenuationType_e.ATTEN_CUSTOM, _sLightAttenuationTex);
      EngineLight.SetMultiplier(_lightMultiplier);
      EngineLight.SetFadeDistances(_fFadeStart, _fFadeEnd);
      EngineLight.SetTriggered(_bTriggered);
      EngineLight.SetUseSpecular(_bUseSpecular);

      UpdateLightPreviewProperties();
    }


    public override void OnTransformationChanged()
    {
 	    base.OnTransformationChanged();
      
      // scaling might have changed:
      if (HasEngineInstance())
        EngineLight.SetIntensity(FinalRadius); 
      if (_hotSpotIntensity!=null)
        _hotSpotIntensity.DisplayScaling = UniformScaling;

    }

    /// <summary>
    /// Called when this shape is added to the scene
    /// </summary>
    public override void OnAddedToScene()
    {
      base.OnAddedToScene ();
      AddHint(HintFlags_e.OnlyUniformScale);
      if (_pendingCoronaSetup != null && ((bool)_pendingCoronaSetup.GetPropertyValue("Enabled")))
      {
        this.AddComponentInternal(_pendingCoronaSetup);
        g_iCoronasConverted++;
        if (Modifiable)
          Modified = true;
      }
      _pendingCoronaSetup = null;

      if (_pendingLensFlareSetup != null && ((bool)_pendingLensFlareSetup.GetPropertyValue("Enabled")))
      {
        this.AddComponentInternal(_pendingLensFlareSetup);
        g_iCoronasConverted++;
        if (Modifiable)
          Modified = true;
      }
      _pendingLensFlareSetup = null;

      UpdateLightPreviewProperties();
    }

    /// <summary>
    /// Called when this shape is removed from the scene
    /// </summary>
    public override void OnRemoveFromScene()
    {
      // always call the base function (handles the children)
      base.OnRemoveFromScene ();
    }

    public override void OnAttachComponent(ShapeComponent component)
    {
      base.OnAttachComponent(component);
      UpdateLightPreviewProperties(); // in case the user adds a shadow component, we don't want it twice
    }

    public override void OnRemoveComponent(ShapeComponent component)
    {
      base.OnRemoveComponent(component);
      UpdateLightPreviewProperties(); // in case the user adds a shadow component, we don't want it twice
    }

    /// <summary>
    /// Access the engine instance as EngineInstanceDynLight
    /// </summary>
    [Browsable(false)]
    public EngineInstanceDynLight EngineLight {get {return _engineInstance as EngineInstanceDynLight;}}

    /// <summary>
    /// overridden function to render the shape
    /// </summary>
    /// <param name="view">destination view</param>
    /// <param name="mode">render mode</param>
    public override void RenderShape(VisionViewBase view, ShapeRenderMode mode)
    {
      if (!HasEngineInstance())
        return;

      base.RenderShape(view, mode);

      if (mode==ShapeRenderMode.Selected)
      {
        view.RenderOrientedLineBox(LocalBoundingBox, Position, RotationMatrix, VisionColors.Yellow, 1.0f);
        if (LightType == LightSourceType_e.Spotlight)
          EngineLight.HighlightShape(view,_fSpotAngle);
        else
          EngineLight.HighlightShape(view,0.0f);
      }
    }


    public override PropertyFlags_e GetPropertyFlags(PropertyFlagsProviderInfo pd)
    {
      if (LightType == LightSourceType_e.Directional)
      {
        if (pd.Name == "Radius" || pd.Name == "LightAttenuationCurve" || pd.Name == "CustomAttenuationCurveTexture")
          return PropertyFlags_e.Hidden;
      }

      if (this.LightAttenuationCurve != LightAttenuationCurve_e.Custom && pd.Name == "CustomAttenuationCurveTexture")
        return PropertyFlags_e.Hidden;

      if (!IsStatic)
      {
        if (pd.Name == "TurnOffAfterLighting" || pd.Name == "BakedToLightmap" || pd.Name == "PreviewAsDynamicLight" || pd.Name == "CastStaticShadows")
          return PropertyFlags_e.Hidden;
      }

      return base.GetPropertyFlags(pd);
    }


    #endregion

    #region Cloning

    /// <summary>
    /// This function must be overridden, since we have to reset the hotspots
    /// </summary>
    /// <returns></returns>
    public override ShapeBase Clone()
    {
      DynLightShape copy = base.Clone() as DynLightShape;

      copy._hotSpotAngle = null;
      copy._hotSpotIntensity = null;

      return copy;
    }

    #endregion

    #region Tracing

    /// <summary>
    /// Overridden trace function
    /// </summary>
    /// <param name="mode"></param>
    /// <param name="rayStart">ray start position</param>
    /// <param name="rayEnd">ray end position</param>
    /// <param name="result">trace result</param>
    public override void OnTraceShape(ShapeTraceMode_e mode, Vector3F rayStart,Vector3F rayEnd, ref ShapeTraceResult result)
    {
      if (mode!=ShapeTraceMode_e.ShapePicking || !HasEngineInstance())
        return;
      if (ConversionUtils.TraceOrientedBoundingBox(LocalBoundingBox,Position,RotationMatrix, rayStart,rayEnd, ref result))
        result.hitShape = this;
    }

    #endregion

    #region Attenuation properties

/*
    /// <summary>
    /// get or set the light type
    /// </summary>
    [Browsable(false)]
    [SortedCategory(CAT_APPEARANCE, CATORDER_APPEARANCE),
    Description("The light attenuation curve (custom or 1/x)"),
    PropertyOrder(1)]
    public EngineInstanceDynLight.LightAttenuationType_e LightAttenuation
    {
      get {return _eLightAttenuation;}
      set 
      {
        if (_eLightAttenuation==value) return;
        _eLightAttenuation = value;
        //SetTypeSpecificProperties();

        // the rest sets properties on the engine instance
        if (_engineInstance==null) return;
        EngineLight.SetAttenuation(_eLightAttenuation,_sLightAttenuationTex);
      }
    }
*/

    String _sLightAttenuationTex = "\\Atten\\smooth.dds";

    /// <summary>
    /// Light attenuation curve
    /// </summary>
    [Editor(typeof(EnumEditor), typeof(System.Drawing.Design.UITypeEditor))]
    [Description("Defines the blend operation between source and destination term")]
    public enum LightAttenuationCurve_e
    {
      /// <summary>
      /// Smooth attenuation
      /// </summary>
      [Description("Smooth curve that can be used for intense lights - Decays slow at the beginning, slow at the end ")]
      Smooth = 0,

      /// <summary>
      /// Square attenuation x^2
      /// </summary>
      [Description("x^2 - Exponential falloff - Decays fast at the beginning, slow at the end")]
      Square = 1,

      /// <summary>
      /// Quad attenuation x^4
      /// </summary>
      [Description("x^4 - Steep exponential falloff - Matches exponential falloff. Decays faster at the beginning, slow at the end")]
      Quad = 2,

      /// <summary>
      /// Linear attenuation
      /// </summary>
      [Description("Linear curve - Constant decay")]
      Linear = 3,

      /// <summary>
      /// No attenuation
      /// </summary>
      [Description("No attenuation - Produces a sharp edge")]
      None = 4,

      [Description("A custom curve texture")]
      Custom = 5

    };

    LightAttenuationCurve_e _eLightAttenuationCurve = LightAttenuationCurve_e.Smooth;

    void UpdateAttenuationProperties()
    {
      if (_engineInstance == null) 
        return;
      EngineLight.SetAttenuation(EngineInstanceDynLight.LightAttenuationType_e.ATTEN_CUSTOM, _sLightAttenuationTex);
      //The type changed, make sure the multiplier and radius are up to date
      EngineLight.SetMultiplier(_lightMultiplier);
      EngineLight.SetIntensity(FinalRadius);
    }

    /// <summary>
    /// get or set the light type
    /// </summary>
    [Browsable(true)]
    [SortedCategory(CAT_APPEARANCE, CATORDER_APPEARANCE),
    Description("The custom light attenuation curve\n"),
    PropertyOrder(3)]
    public LightAttenuationCurve_e LightAttenuationCurve
    {
      get {return _eLightAttenuationCurve;}
      set 
      {
        if (_eLightAttenuationCurve==value) return;
        _eLightAttenuationCurve = value;

        switch (_eLightAttenuationCurve)
        {
          case LightAttenuationCurve_e.Smooth:  _sLightAttenuationTex = "\\Atten\\smooth.dds"; break;
          case LightAttenuationCurve_e.Square:  _sLightAttenuationTex = "\\Atten\\square.dds"; break;
          case LightAttenuationCurve_e.Quad:    _sLightAttenuationTex = "\\Atten\\quad.dds";   break;
          case LightAttenuationCurve_e.Linear:  _sLightAttenuationTex = "\\Atten\\linear.dds"; break;
          case LightAttenuationCurve_e.None:    _sLightAttenuationTex = "\\Atten\\none.dds";   break;
          case LightAttenuationCurve_e.Custom: break; // don't modify the texture name
        }

        UpdateAttenuationProperties();
      }
    }

    [SortedCategory(CAT_APPEARANCE, CATORDER_APPEARANCE), PropertyOrder(4)]
    [Description("The custom light attenuation curve texture (only used when LightAttenuationCurve is set to Custom)\nThe first row of the texture is used to define a gradient from white to black over the radius of the light.")]
    [EditorAttribute(typeof(BitmapBrowserEditor), typeof(UITypeEditor))]
    public string CustomAttenuationCurveTexture
    {
      get { return _sLightAttenuationTex; }
      set
      {
        if (_sLightAttenuationTex == value) 
          return;
        _sLightAttenuationTex = value;
        UpdateAttenuationProperties();
      }
    }


    float _lightMultiplier = 1;

    /// <summary>
    /// get or set the light multiplier
    /// </summary>
    [SortedCategory(CAT_APPEARANCE, CATORDER_APPEARANCE),
    Description("The multiplier factor for the light intensity for overbright effects."),
    PropertyOrder(2)]
    public float Multiplier
    {
      get {return _lightMultiplier;}
      set 
      {
        if (_lightMultiplier==value) return;
        _lightMultiplier = value;

        // the rest sets properties on the engine instance
        if (_engineInstance==null) return;
        EngineLight.SetMultiplier(_lightMultiplier);
      }
    }

    float _fFadeStart = 0.0f;
    float _fFadeEnd = 0.0f;

    [Description("Defines the distance (in units) where this dynamic light is faded out (far clip distance). Do deactivate far clip distance, set a value of 0.0")]
    [SortedCategory(CAT_APPEARANCE, CATORDER_APPEARANCE), PropertyOrder(51)]
    public float DistanceFadeEnd
    {
        get { return _fFadeEnd; }
        set
        {
            _fFadeEnd = value;
            if (_fFadeStart > _fFadeEnd) _fFadeEnd = _fFadeStart;
            if (_fFadeEnd < 0.0f) _fFadeEnd = 0.0f;
            if (HasEngineInstance())
                EngineLight.SetFadeDistances(_fFadeStart, _fFadeEnd);
        }
    }

    [Description("Defines the distance (in units) to start fading out this dynamic light. Fading is only activated if DistanceFadeEnd>0")]
    [SortedCategory(CAT_APPEARANCE, CATORDER_APPEARANCE), PropertyOrder(50)]
    public float DistanceFadeStart
    {
      get { return _fFadeStart; }
      set
      {
        _fFadeStart = value;
        if (_fFadeStart > _fFadeEnd) _fFadeStart = _fFadeEnd;
        if (HasEngineInstance())
          EngineLight.SetFadeDistances(_fFadeStart, _fFadeEnd);
      }
    } 

    bool _bUseSpecular = true;

    [Description("Defines if specular highlighting is enabled for this light source (deferred only!).")]
    [SortedCategory(CAT_APPEARANCE, CATORDER_APPEARANCE), PropertyOrder(52)]
    public bool UseSpecular
    {
      get { return _bUseSpecular; }
      set
      {
        _bUseSpecular = value;
        if (HasEngineInstance())
          EngineLight.SetUseSpecular(_bUseSpecular);
      }
    }

    #endregion

    #region Light Type

    LightSourceType_e _lightType = LightSourceType_e.Point;

    /// <summary>
    /// Translate engine type constant to vForge (and vice versa). The int value can be casted to NativeLightSourceType_e
    /// </summary>
    [Browsable(false)]
    public int EngineLightType
    {
      get
      {
        switch (LightType)
        {
          case LightSourceType_e.Point: return (int)NativeLightSourceType_e.Point;
          case LightSourceType_e.Spotlight: return (int)NativeLightSourceType_e.Spotlight;
          case LightSourceType_e.Directional: return (int)NativeLightSourceType_e.Directional;
        }
        return -1;
      }
      set
      {
        switch ((NativeLightSourceType_e)value)
        {
          case NativeLightSourceType_e.Point: LightType = LightSourceType_e.Point; break;
          case NativeLightSourceType_e.Spotlight: LightType = LightSourceType_e.Spotlight; break;
          case NativeLightSourceType_e.Directional: LightType = LightSourceType_e.Directional; break;
        }
      }
    }


    /// <summary>
    /// Just internal helper function
    /// </summary>
    public void MakeTimeOfDayLight(float fIntensity, bool bCorona, string coronaFile)
    {
      LightType = LightSourceType_e.Directional;
      if (bCorona && string.IsNullOrEmpty(coronaFile))
        coronaFile = "";
      CoronaTexture = coronaFile;
      ShapeComponentType t = (ShapeComponentType)EditorManager.EngineManager.ComponentClassManager.GetCollectionType("VTimeOfDayComponent");
      System.Diagnostics.Debug.Assert(t != null, "Cannot create component of type VTimeOfDayComponent");
      if (t != null)
      {
        ShapeComponent comp = (ShapeComponent)t.CreateInstance(this);
        comp.SetPropertyValue("Intensity", fIntensity, false);
        comp.SetPropertyValue("AttachmentType", "SUNLIGHT", false);
        this.AddComponentInternal(comp);
      }
    }

    public void MakeTimeOfDayBackLight()
    {
      LightType = LightSourceType_e.Directional;
      ShapeComponentType t = (ShapeComponentType)EditorManager.EngineManager.ComponentClassManager.GetCollectionType("VTimeOfDayComponent");
      System.Diagnostics.Debug.Assert(t != null, "Cannot create component of type VTimeOfDayComponent");
      if (t != null)
      {
        ShapeComponent comp = (ShapeComponent)t.CreateInstance(this);
        comp.SetPropertyValue("AttachmentType", "SUNBACKLIGHT", false);
        this.AddComponentInternal(comp);
      }
    }

    static ShapeComponentType SpotDirComponentType
    {
      get
      {
#if HK_ANARCHY
        ShapeComponentType spotDirType = (ShapeComponentType)EditorManager.EngineManager.ComponentClassManager.GetCollectionType("VMobileShadowMapComponentSpotDirectional");
        System.Diagnostics.Debug.Assert(spotDirType != null, "VMobileShadowMapComponentSpotDirectional not found!");
#else
        ShapeComponentType spotDirType = (ShapeComponentType)EditorManager.EngineManager.ComponentClassManager.GetCollectionType("VShadowMapComponentSpotDirectional");
        System.Diagnostics.Debug.Assert(spotDirType != null, "VShadowMapComponentSpotDirectional not found!");
#endif
        return spotDirType;
      }
    }

    static ShapeComponentType PointLightComponentType
    {
      get
      {
        ShapeComponentType pointType = (ShapeComponentType)EditorManager.EngineManager.ComponentClassManager.GetCollectionType("VShadowMapComponentPoint");
        System.Diagnostics.Debug.Assert(pointType != null, "VShadowMapComponentPoint not found!");
        return pointType;
      }
    }

    static ShapeComponentType SunLightComponentType
    {
      get
      {
        ShapeComponentType sunType = (ShapeComponentType)EditorManager.EngineManager.ComponentClassManager.GetCollectionType("VShadowMapComponentSunlight");
        System.Diagnostics.Debug.Assert(sunType != null, "VShadowMapComponentSunlight not found!");
        return sunType;
      }
    }

    static ShapeComponentType MobileSpotDirComponentType
    {
      get
      {
        ShapeComponentType mobileSpotDirType = (ShapeComponentType)EditorManager.EngineManager.ComponentClassManager.GetCollectionType("VMobileShadowMapComponentSpotDirectional");
        System.Diagnostics.Debug.Assert(mobileSpotDirType != null, "VMobileShadowMapComponentSpotDirectional not found!");
        return mobileSpotDirType;
      }
    }

    /// <summary>
    /// get or set the light type
    /// </summary>
    /// 
    [DependentProperties(new string[] { "ComponentsInternal" })]
    [SortedCategory(CAT_APPEARANCE, CATORDER_APPEARANCE),
    Description("The light type (point, spotlight or directional)"),
    PropertyOrder(5)]
    public LightSourceType_e LightType
    {
      get {return _lightType;}
      set 
      {
        if (_lightType == value)
          return;
        _lightType = value;
        SetTypeSpecificProperties();
        InvalidateIcon();

        // the rest sets properties on the engine instance
        if (_engineInstance == null)
          return;

        EngineLight.SetLightType(_lightType, _fSpotAngle, _sProjectedTexture);

        // check whether components have to be removed
        if (base.ComponentCount>0)
        {

          // Remove any shadow map component since it is light type specific (e.g. we do not want to keep the SpotDirectional component when switching to a point light)
          ShapeComponentType spotDirType = SpotDirComponentType;
          ShapeComponentType pointType = PointLightComponentType;
          ShapeComponentType sunType = SunLightComponentType;
          ShapeComponentType mobileSpotDirType = MobileSpotDirComponentType;  
          ShapeComponent comp = null;

          if (spotDirType != null && pointType != null && sunType != null && mobileSpotDirType != null)
          {
            while (true)
            {
              comp = base.Components.GetComponentByType(spotDirType);
              if (comp == null)
                comp = base.Components.GetComponentByType(pointType);
              if (comp == null)
                comp = base.Components.GetComponentByType(sunType);
              if (comp == null)
                comp = base.Components.GetComponentByType(mobileSpotDirType);

              if (comp != null)
                this.OnRemoveComponent(comp);
              else
                break;
            }
          }
        }

        UpdateLightPreviewProperties();
      }
    }

    // these members are merged from all light types
    float _fSpotAngle = 45.0f;
    string _sProjectedTexture = null;

    LightTypePropertiesBase _typeProperties = null;

    /// <summary>
    /// get or set the type specific sub-properties
    /// </summary>
    [PrefabRelevant(false)]
    [SortedCategory(CAT_APPEARANCE, CATORDER_APPEARANCE),
    PropertyOrder(6),
    TypeConverter(typeof(UndoableObjectConverter))]
    public LightTypePropertiesBase TypeProperties
    {
      get {return _typeProperties;}
      set {_typeProperties = value;}
    }


    /// <summary>
    /// Update the properties
    /// </summary>
    void SetTypeSpecificProperties()
    {
      switch (_lightType)
      {
        case LightSourceType_e.Point: _typeProperties = new LightTypeProperties_OmniLight(this); break;
        case LightSourceType_e.Spotlight: _typeProperties = new LightTypeProperties_SpotLight(this); break;
        case LightSourceType_e.Directional: _typeProperties = new LightTypeProperties_Directional(this); break;
      }
    }

    /// <summary>
    /// get or set the spotlight cone angle
    /// </summary>
    [BrowsableAttribute(false)]
    [PrefabRelevant(true)]
    public float SpotAngle
    {
      get {return _fSpotAngle;}
      set 
      {
        float fClamped = value;
        if (fClamped<1.0f) fClamped = 1.0f;
        if (fClamped>179.0f) fClamped=179.0f;

        if (_fSpotAngle==fClamped) return;
        _fSpotAngle = fClamped;

        // the rest sets properties on the engine instance
        if (_engineInstance==null) return;
        EngineLight.SetLightType(_lightType, _fSpotAngle, _sProjectedTexture);
      }
    }

    /// <summary>
    /// get or set the filename for the projected texture
    /// </summary>
    [PrefabResolveFilename]
    [BrowsableAttribute(false)]
    [PrefabRelevant(true)]
    public string ProjectedTexture
    {
      get {return _sProjectedTexture;}
      set 
      {
        if (_sProjectedTexture==value) return;
        _sProjectedTexture = value;
        // the rest sets properties on the engine instance
        if (_engineInstance==null) return;
        EngineLight.SetLightType(_lightType, _fSpotAngle, _sProjectedTexture);
      }
    }

    #endregion

    #region Appearance Category

    float _fIntensity = 1000.0f;
    bool _bTriggered = true;

    /// <summary>
    /// get or set the intensity
    /// </summary>
    [Browsable(true)]
    [SortedCategory(CAT_APPEARANCE, CATORDER_APPEARANCE),
    Description("The light's range of influence. Ignored for directional lights"),
    PropertyOrder(1)]
    public float Radius
    {
      get {return _fIntensity;}
      set 
      {
        if (_fIntensity==value) return;
        _fIntensity = value;
        if (_engineInstance!=null)
          EngineLight.SetIntensity(FinalRadius);
      }
    }

    [SortedCategory(CAT_APPEARANCE, CATORDER_APPEARANCE),
    Description("If disabled, the light is turned off at startup and must be toggled on via a trigger link"),
    PropertyOrder(2)]
    public bool Triggered
    {
      get { return _bTriggered; }
      set
      {
        if (_bTriggered == value) return;
        _bTriggered = value;
        if (_engineInstance != null)
          EngineLight.SetTriggered(_bTriggered);
      }
    }

    /// <summary>
    /// Old compatibility property
    /// </summary>
    [Browsable(false)]
    public float Intensity
    {
      get { return Radius; }
      set { Radius = value; }
    }

    int _iDecayRate = 1;

    /// <summary>
    /// New decay rate compatibility
    /// </summary>
    [Browsable(false)]
    public float DecayRate
    {
        get { return _iDecayRate; }
      set
      {
        // set attenuation based on decay rate
          _iDecayRate = (int)value;
          switch (_iDecayRate)
        {
            case 0: /* not supported */ break;
            case 1: LightAttenuationCurve = LightAttenuationCurve_e.Linear; break;
            case 2: LightAttenuationCurve = LightAttenuationCurve_e.Square; break;
            case 3: /* not supported */ break;
            case 4: LightAttenuationCurve = LightAttenuationCurve_e.Quad; break;
        }
      }
    }

    Color _lightColor = System.Drawing.Color.White;

    /// <summary>
    /// get or set the light color
    /// </summary>
    [SortedCategory(CAT_APPEARANCE, CATORDER_APPEARANCE),
    Description("Light color"),
    EditorAttribute(typeof(StandardColorDropDownEditor), typeof(UITypeEditor)),
    PropertyOrder(7)]
    [PropertyLiveUpdate("LightColorLiveUpdate")]
    public Color LightColor
    {
      get {return _lightColor;}
      set 
      {
        if (_lightColor==value) return;
        _lightColor = value;
        if (_engineInstance!=null)
          EngineLight.SetLightColor(VisionColors.Get(_lightColor));
      }
    }

    /// <summary>
    /// Proxy property used for color live update (See PropertyLiveUpdate attribute in LightColor property)
    /// </summary>
    [Browsable(false)]
    public object LightColorLiveUpdate
    {
      set
      {
        Color col = _lightColor;
        if (value is Color)
          col = (Color)value;
        if (_engineInstance != null)
          EngineLight.SetLightColor(VisionColors.Get(col));
        EditorManager.ActiveView.UpdateView(false);
      }
    }

    //string _lightKey = null;

    /// <summary>
    /// Gets or sets the light key
    /// </summary>
    [Browsable(false)]
    public string LightKey
    {
      get {return ObjectKey;}
      set 
      {
        ObjectKey = value;
      }
    }


    #endregion

    #region Corona Category

    public static int g_iCoronasConverted = 0;

    ShapeComponent _pendingCoronaSetup = null;
    ShapeComponent _pendingLensFlareSetup = null;

    ShapeComponent GetCoronaComponentSafe()
    {
      if (_pendingCoronaSetup == null)
      {
        _pendingCoronaSetup = (ShapeComponent)EditorManager.EngineManager.ComponentClassManager.CreateCollection(this, "VCoronaComponent");
      }
      return _pendingCoronaSetup;
    }

    ShapeComponent GetLensFlareComponentSafe()
    {
      if (_pendingLensFlareSetup == null)
      {
        _pendingLensFlareSetup = (ShapeComponent)EditorManager.EngineManager.ComponentClassManager.CreateCollection(this, "VLensFlareComponent");
      }
      return _pendingLensFlareSetup;
    }

 
    /// <summary>
    /// get or set the corona flag
    /// </summary>
    [Browsable(false)]
    public bool EnableCorona
    {
      get {return false;}
      set 
      {
#if !HK_ANARCHY
        GetCoronaComponentSafe().SetPropertyValue("Enabled", value, false);
#endif
      }
    }

    /// <summary>
    /// get or set the lens flare flag
    /// </summary>
    [Browsable(false)]
    public bool EnableLensFlares
    {
      get { return false; }
      set
      {
#if !HK_ANARCHY
        if (value)
          GetLensFlareComponentSafe();
#endif
      }
    }

    /// <summary>
    /// Compatibility prefab function
    /// </summary>
    [Browsable(false)]
    public string CoronaTexture
    {
      get {return null;}
      set 
      {
#if !HK_ANARCHY 
        string tex = value;
        if (string.Compare(tex, "default", true) == 0)
          tex = "";
        GetCoronaComponentSafe().SetPropertyValue("CoronaTexture", tex, false);
#endif
      }
    }



    /// <summary>
    /// Compatibility prefab function
    /// </summary>
    [Browsable(false)]
    public float CoronaScaling
    {
      get { return 1.0f; }
      set 
      {
#if !HK_ANARCHY
        GetCoronaComponentSafe().SetPropertyValue("CoronaScaling", value, false);
#endif
      }
    }

    /// <summary>
    /// Compatibility prefab function
    /// </summary>
    [Browsable(false)]
    public CoronaScaleModeFlags_e CoronaFlags
    {
      get { return CoronaScaleModeFlags_e.None; }
      set 
      { 
#if !HK_ANARCHY
        ShapeComponent corona = GetCoronaComponentSafe();
        corona.SetPropertyValue("CoronaFixedSize", (value & CoronaScaleModeFlags_e.Distance) > 0 ? false : true, false);
        corona.SetPropertyValue("CoronaScaleWithIntensity", (value & CoronaScaleModeFlags_e.VisibleArea) > 0 ? true : false, false);
        corona.SetPropertyValue("CoronaGlobalFadeOut", (value & CoronaScaleModeFlags_e.GlobalFadeOut) > 0 ? true : false, false);
        corona.SetPropertyValue("CoronaRotate", (value & CoronaScaleModeFlags_e.Rotate) > 0 ? true : false, false);
#endif
      }
    }

    /// <summary>
    /// Compatibility prefab function
    /// </summary>
    [Browsable(false)]
    public short CoronaCheckBlockSize
    {
      get { return 0; }
      set 
      {
#if !HK_ANARCHY
        GetCoronaComponentSafe().SetPropertyValue("QueryRadius", (int)value, false);
#endif
      }
    }


    #endregion

      
    #region Static Shadows Category

    bool _bDeprecatedModelShadows = false;
    bool _bCastStaticShadows = true;

    /// <summary>
    /// DEPRECATED but left here for prefabs. Use CastStaticShadows now
    /// </summary>
    [Browsable(false)]
    public bool CastWorldShadows
    {
      get {return _bCastStaticShadows;}
      set 
      {
        if (_bCastStaticShadows==value) return;
        _bCastStaticShadows = value;
        if (_engineInstance!=null)
          EngineLight.SetCastShadows(_bCastStaticShadows,_bDeprecatedModelShadows);
        UpdateLightPreviewProperties();
      }
    }

    /// <summary>
    /// DEPRECATED but left here for prefabs. Use CastStaticShadows now
    /// </summary>
    [Browsable(false)]
    public bool CastModelShadows
    {
      get {return _bDeprecatedModelShadows;}
      set 
      {
        _bDeprecatedModelShadows = value;
      }
    }

    /// <summary>
    /// Flag that determines whether this static light casts static shadows that are baked into the lightmap or light grid
    /// </summary>
    [SortedCategory(CAT_STATICLIGHTING, CATORDER_STATICLIGHTING),
    Description("Flag that determines whether this static light casts static shadows that are baked into the lightmap or light grid"),
    PropertyOrder(20), DefaultValue(false)]
    public bool CastStaticShadows
    {
      get { return _bCastStaticShadows; }
      set
      {
        if (_bCastStaticShadows == value) return;
        _bCastStaticShadows = value;
        if (_engineInstance != null)
          EngineLight.SetCastShadows(_bCastStaticShadows, _bDeprecatedModelShadows);
        UpdateLightPreviewProperties();
      }
    }

    /// <summary>
    /// Pseudo property that is used as a flag to determine whether light masks in vLux should be created
    /// </summary>
    [Browsable(false), PrefabRelevant(true)]
    [SortedCategory(CAT_STATICLIGHTING, CATORDER_STATICLIGHTING), PropertyOrder(21)]
    public bool UsesLightMasks
    {
      set { } // we need the setter to be prefab compliant
      get
      {
        if (ComponentCount == 0 || !IsStatic)
          return false;
#if !HK_ANARCHY
        if (Components.GetComponentByType(PointLightComponentType) != null)
          return true;
        if (Components.GetComponentByType(SpotDirComponentType) != null)
          return true;
        if (Components.GetComponentByType(SunLightComponentType) != null)
          return true;
#endif
        if (Components.GetComponentByType(MobileSpotDirComponentType) != null)
          return true;
        return false;
      }
    }

    /// <summary>
    /// Backwards compatibility
    /// </summary>
    [Browsable(false), PrefabRelevant(false)]
    [SortedCategory(CAT_STATICLIGHTING, CATORDER_STATICLIGHTING), PropertyOrder(21)]
    public bool UsesStaticShadowmaps
    {
      set { }
      get { return false; }
    }
    

    #endregion
    

    #region Light Animation Category

    string _colorAnimTable;

    /// <summary>
    /// get or set the animation curve texture filename
    /// </summary>
    [PrefabResolveFilename]
    [SortedCategory(CAT_ANIMATIONS, CATORDER_ANIMATIONS),
    Description("Animation curve texture to get the color lookup used for animating the light."),
    EditorAttribute(typeof(AssetEditor), typeof(UITypeEditor)), AssetDialogFilter(new string[] { "Texture" }),
    PropertyOrder(1)]
    public string AnimationCurve
    {
      get {return _colorAnimTable;}
      set 
      {
        if (_colorAnimTable==value) return;
        _colorAnimTable = value;
        if (_engineInstance!=null)
          EngineLight.SetAnimationLookup(_colorAnimTable);
      }
    }

    float _fAnimTiming = 1.0f;

    /// <summary>
    /// get or set the animation timing
    /// </summary>
    [SortedCategory(CAT_ANIMATIONS, CATORDER_ANIMATIONS),
    Description("Animation cycle in seconds"),
    PropertyOrder(2)]
    public float AnimationTiming
    {
      get {return _fAnimTiming;}
      set 
      {
        if (_fAnimTiming==value) return;
        _fAnimTiming = value;
        if (_engineInstance!=null)
          EngineLight.SetAnimationTiming(_fAnimTiming);
      }
    }

    float _fAnimPhase = 0.0f;

    /// <summary>
    /// get or set the start animation phase
    /// </summary>
    [SortedCategory(CAT_ANIMATIONS, CATORDER_ANIMATIONS),
    Description("Animation phase offset percentage [0..100]"),
    PropertyOrder(3)]
    public float AnimationPhase
    {
      get {return _fAnimPhase;}
      set 
      {
        if (_fAnimPhase==value) return;
        _fAnimPhase = value;
        if (_engineInstance!=null)
          EngineLight.SetAnimationPhase(_fAnimPhase);
      }
    }


    bool _bRemoveLightAfterAnim = false;

    /// <summary>
    /// get or set the RemoveLightAfterAnimation flag
    /// </summary>
    [SortedCategory(CAT_ANIMATIONS, CATORDER_ANIMATIONS),
    Description("When this flag is enabled then the light is deleted as soon as the animation is finished"),
    PropertyOrder(4)]
    public bool RemoveLightAfterAnimation
    {
      get { return _bRemoveLightAfterAnim; }
      set
      {
        if (_bRemoveLightAfterAnim == value) return;
        _bRemoveLightAfterAnim = value;
        if (_engineInstance != null)
          EngineLight.SetRemoveLightWhenFinished(_bRemoveLightAfterAnim);
      }
    }


    #endregion

    #region Influence Category
  
    int _iWorldInfluence = 255;
    int _iObjectInfluence = 255;
    FlagsInt32_e _iVisibleBitmask = FlagsInt32_e.All;

    /// <summary>
    /// get or set the inflence bitmask for world
    /// </summary>
    [SortedCategory(CAT_INFLUENCE, CATORDER_INFLUENCE),
    Description("Bitmask for affecting static geometry. If this is a static light and this mask is set to 'none' then this light does not contribute to light maps."),
    PropertyOrder(1)]
    public FlagsInt32_e WorldInfluenceMask
    {
      get {return (FlagsInt32_e)_iWorldInfluence;}
      set 
      {
        if (_iWorldInfluence==(int)value) return;
        _iWorldInfluence = (int)value;
        if (_engineInstance!=null)
          EngineLight.SetInfluenceBitMasks(_iWorldInfluence,_iObjectInfluence);
      }
    }

    /// <summary>
    ///  get or set the inflence bitmask for entities
    /// </summary>
    [SortedCategory(CAT_INFLUENCE, CATORDER_INFLUENCE),
    Description("Bitmask for affecting entities. If this is a static light and this mask is set to 'none' then this light does not contribute to the light grid (also requires 'UseObjectInfluenceMask' set to true in the vLux lightgrid settings)"),
    PropertyOrder(2)]
    public FlagsInt32_e ObjectInfluenceMask
    {
      get {return (FlagsInt32_e)_iObjectInfluence;}
      set 
      {
        if (_iObjectInfluence==(int)value) return;
        _iObjectInfluence = (int)value;
        if (_engineInstance!=null)
          EngineLight.SetInfluenceBitMasks(_iWorldInfluence,_iObjectInfluence);
      }
    }

    /// <summary>
    /// Entity visibility bitmask that determines the visible status in the rendering contexts
    /// </summary>
    [SortedCategory(CAT_INFLUENCE, CATORDER_INFLUENCE),
    Description("Light visibility bitmask that determines the visible status in the rendering contexts."),
    PropertyOrder(3),
    DefaultValue(FlagsInt32_e.All)]
    public FlagsInt32_e VisibleBitmask
    {
      get { return _iVisibleBitmask; }
      set
      {
        _iVisibleBitmask = value;
        if (_engineInstance != null)
          EngineLight.SetVisibleBitmask((uint)_iVisibleBitmask);
      }
    }
    #endregion

    #region Static/Lightmap category

    bool _bStatic = false;
    bool _bExportWhenNotRelevant = false;
    bool _bPreviewStatic = true;
    bool _bTurnOffAfterLighting = true;
    bool _bBakedToLightmap = false;

    /// <summary>
    /// Get or set the status for exporting this light as a static light
    /// </summary>
    [SortedCategory(CAT_STATICLIGHTING, CATORDER_STATICLIGHTING),
    Description("If enabled, the light is a static light rather than a dynamic light. If static, it will be considered for lightmaps."),
    PropertyOrder(1)]
    public bool IsStatic
    {
      get {return _bStatic;} 
      set
      {
        if (_bStatic != value)
          _bBakedToLightmap = false;

        _bStatic = value;
        if (HasEngineInstance())
          EngineLight.SetExportAsStatic(_bStatic);
        ReAttachComponents();
        UpdateLightPreviewProperties();
      }
    }

    // backwards compatibility
    [Browsable(false)]
    public bool ExportAsStatic
    {
      get { return IsStatic; }
      set { IsStatic = value; }
    }


    /// <summary>
    /// Get or set the status for exporting this light as a static light
    /// </summary>
    [SortedCategory(CAT_STATICLIGHTING, CATORDER_STATICLIGHTING),
    Description("If enabled, the static light is exported even when it is not relevant (e.g. no corona, no shadowmap). This might be important to gather all lights for deferred shading. This option is ignored for dynamic lighting."),
    PropertyOrder(2)]
    public bool ExportWhenNotRelevant
    {
      get { return _bExportWhenNotRelevant; }
      set
      {
        _bExportWhenNotRelevant = value;
        if (HasEngineInstance())
          EngineLight.SetExportWhenNotRelevant(_bExportWhenNotRelevant);
      }
    }

    /// <summary>
    /// Get or set the flag for previewing a static light as a dynamic pixel light
    /// </summary>
    [SortedCategory(CAT_STATICLIGHTING, CATORDER_STATICLIGHTING),
    Description("If enabled, this static light will be displayed as a dynamic light in vForge for previewing, as long it is not baked to lightmaps (BakedToLightmap property)."),
    PropertyOrder(3)]
    public bool PreviewAsDynamicLight
    {
      get {return _bPreviewStatic;} 
      set
      {
        if (_bPreviewStatic != value)
          _bBakedToLightmap = false;
        _bPreviewStatic = value;
        if (HasEngineInstance())
          EngineLight.SetPreviewStatic(_bPreviewStatic && !_bBakedToLightmap);
        UpdateLightPreviewProperties();
      } 
    }

    /// <summary>
    /// If set, the next successful lighting update will turn off the preview a dynamic light
    /// </summary>
    [SortedCategory(CAT_STATICLIGHTING, CATORDER_STATICLIGHTING),
    Description("If enabled, the next successful static lighting update (in vForge) will turn off the previewing as a dynamic light by enabling the BakedToLightmap status."),
    PropertyOrder(4)]
    public bool TurnOffAfterLighting
    {
      get {return _bTurnOffAfterLighting;} 
      set {_bTurnOffAfterLighting = value;} 
    }

    /// <summary>
    /// Indicates whether this static light has been baked to lightmap
    /// </summary>
    [SortedCategory(CAT_STATICLIGHTING, CATORDER_STATICLIGHTING),
    Description("Indicates whether this static light has been baked to lightmap. If enabled, the light will not be previewed as a dynamic light even if PreviewAsDynamicLight is enabled"),
    PropertyOrder(10)]
    public bool BakedToLightmap
    {
      get { return _bBakedToLightmap; }
    }

    #endregion

    #region ISerializable

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected DynLightShape(SerializationInfo info, StreamingContext context) : base(info, context)
    {
      _lightColor = (Color)info.GetValue("_lightColor", typeof(Color));
      _fIntensity = info.GetSingle("_fIntensity");
      LightType_e oldType = (LightType_e)info.GetValue("_lightType", typeof(LightType_e));
      _lightType = (LightSourceType_e)oldType;

      _fSpotAngle = info.GetSingle("_fSpotAngle");
      _sProjectedTexture = info.GetString("_sProjectedTexture");
      //_bSpecularity = info.GetBoolean( "_bSpecularity");
      _bDeprecatedModelShadows = info.GetBoolean("_bModelShadows");
      _bCastStaticShadows = info.GetBoolean("_bWorldShadows");
      _colorAnimTable = info.GetString("_colorAnimTable");
      _fAnimTiming = info.GetSingle("_fAnimTiming");
      _fAnimPhase = info.GetSingle("_fAnimPhase");
      _iWorldInfluence = info.GetInt32("_iWorldInfluence");
      _iObjectInfluence = info.GetInt32("_iObjectInfluence");
      if (SerializationHelper.HasElement(info, "_iVisibleBitmask"))
        _iVisibleBitmask = (FlagsInt32_e)info.GetValue("_iVisibleBitmask", typeof(FlagsInt32_e));
      if (SerializationHelper.HasElement(info, "_bStatic"))
        _bStatic = info.GetBoolean("_bStatic");
      if (SerializationHelper.HasElement(info, "_bPreviewStatic"))
        _bPreviewStatic = info.GetBoolean("_bPreviewStatic");
      if (SerializationHelper.HasElement(info, "_bTurnOffAfterLighting"))
        _bTurnOffAfterLighting = info.GetBoolean("_bTurnOffAfterLighting");
      if (SerializationHelper.HasElement(info, "_bExportWhenNotRelevant"))
        _bExportWhenNotRelevant = info.GetBoolean("_bExportWhenNotRelevant");
      if (SerializationHelper.HasElement(info, "_bBakedToLightmap"))
        _bBakedToLightmap = info.GetBoolean("_bBakedToLightmap");
      else _bBakedToLightmap = true; // for old maps

      if (SerializationHelper.HasElement(info, "_sLightAttenuationTex"))
        _sLightAttenuationTex = info.GetString("_sLightAttenuationTex");

      if (SerializationHelper.HasElement(info, "_eLightAttenuationCurve"))
        _eLightAttenuationCurve = (LightAttenuationCurve_e)info.GetValue("_eLightAttenuationCurve", typeof(LightAttenuationCurve_e));

      if (SerializationHelper.HasElement(info, "_lightKey")) // OLD version map to base member
        _objectKey = info.GetString("_lightKey");

      if (SerializationHelper.HasElement(info, "_lightMultiplier"))
        _lightMultiplier = info.GetSingle("_lightMultiplier");
      if (SerializationHelper.HasElement(info, "_fFadeStart"))
        _fFadeStart = info.GetSingle("_fFadeStart");
      if (SerializationHelper.HasElement(info, "_fFadeEnd"))
        _fFadeEnd = info.GetSingle("_fFadeEnd");
      if (SerializationHelper.HasElement(info, "_bTriggered"))
        _bTriggered = info.GetBoolean("_bTriggered");
      if (SerializationHelper.HasElement(info, "_bUseSpecular"))
        _bUseSpecular = info.GetBoolean("_bUseSpecular");
      if (SerializationHelper.HasElement(info, "_bRemoveLightAfterAnim"))
        _bRemoveLightAfterAnim = info.GetBoolean("_bRemoveLightAfterAnim");

      // backwards compatibility
      if (!SerializationHelper.HasElement(info, "SupportScaling"))
        SetScaling_Internal(1.0f, 1.0f, 1.0f);

      // corona: convert old shape properties into component
      if (SerializationHelper.HasElement(info, "_bCorona") && info.GetBoolean("_bCorona"))
      {
#if !HK_ANARCHY
        if (SerializationHelper.HasElement(info, "_coronaTexture"))
          CoronaTexture = info.GetString("_coronaTexture");
        if (SerializationHelper.HasElement(info, "_fCoronaScaling"))
          CoronaScaling = info.GetSingle("_fCoronaScaling");
        if (SerializationHelper.HasElement(info, "_eCoronaFlags"))
          CoronaFlags = (CoronaScaleModeFlags_e)info.GetValue("_eCoronaFlags", typeof(CoronaScaleModeFlags_e));
        if (SerializationHelper.HasElement(info, "_iCheckBlockSize"))
          CoronaCheckBlockSize = info.GetInt16("_iCheckBlockSize");
#endif
      }
      else if (SerializationHelper.HasElement(info, "_bLensFlares") && info.GetBoolean("_bLensFlares"))
      {
#if !HK_ANARCHY
        // Creates default lens flare component if property is true.
        GetLensFlareComponentSafe();
#endif
      }


    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context) 
    {
      base.GetObjectData(info,context);

      info.AddValue("_lightColor", _lightColor);
      info.AddValue("_fIntensity", _fIntensity);
      info.AddValue("_lightType", (LightType_e)_lightType);
      info.AddValue("_fSpotAngle", _fSpotAngle);
      info.AddValue("_sProjectedTexture", _sProjectedTexture);
      info.AddValue("_bModelShadows", _bDeprecatedModelShadows);
      info.AddValue("_bWorldShadows", _bCastStaticShadows);
      info.AddValue("_colorAnimTable", _colorAnimTable);
      info.AddValue("_fAnimTiming", _fAnimTiming);
      info.AddValue("_fAnimPhase", _fAnimPhase);
      info.AddValue("_iWorldInfluence", _iWorldInfluence);
      info.AddValue("_iObjectInfluence", _iObjectInfluence);
      info.AddValue("_iVisibleBitmask", _iVisibleBitmask);
      info.AddValue("_bStatic",_bStatic);
      info.AddValue("_bPreviewStatic",_bPreviewStatic);
      info.AddValue("_bTurnOffAfterLighting",_bTurnOffAfterLighting);
      info.AddValue("_bExportWhenNotRelevant", _bExportWhenNotRelevant);
      info.AddValue("_bBakedToLightmap", _bBakedToLightmap);

      info.AddValue("_sLightAttenuationTex",_sLightAttenuationTex);
      info.AddValue("_eLightAttenuationCurve",_eLightAttenuationCurve);
      info.AddValue("_lightMultiplier",_lightMultiplier);
      info.AddValue("_fFadeStart", _fFadeStart);
      info.AddValue("_fFadeEnd", _fFadeEnd);
      info.AddValue("_bTriggered", _bTriggered);
      info.AddValue("_bUseSpecular", _bUseSpecular);
      info.AddValue("_bRemoveLightAfterAnim", _bRemoveLightAfterAnim);
      info.AddValue("SupportScaling", true);
    }

    #endregion

		#region Hotspot

		HotSpotConeAngle _hotSpotAngle; // a hotspot for the spot opening angle
		HotSpotDistance _hotSpotIntensity; // a hotspot for the intensity
    HotSpotColorButton _hotSpotColor; // a color button

		/// <summary>
		/// Called when the shape is selected
		/// </summary>
		public override void OnSelected()
		{
			base.OnSelected ();

			// create the hotspots:

			// hotspot for spotangle
			_hotSpotAngle = new HotSpotConeAngle(this,@"textures\Hotspot_A.tga",VisionColors.Red,HotSpotBase.PickType.Square,4.0f);
      _hotSpotAngle.Distance = 200.0f * EditorManager.Settings.GlobalUnitScaling;
			_hotSpotAngle.StartAngle = SpotAngle;
      _hotSpotAngle.ToolTipText = "cone angle";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotAngle);
			
			// hotspot for intensity
			_hotSpotIntensity = new HotSpotDistance(this,@"textures\Hotspot_R.tga",VisionColors.Yellow,HotSpotBase.PickType.Square,4.0f);
			_hotSpotIntensity.StartDistance = this.Intensity;
      _hotSpotIntensity.DisplayScaling = UniformScaling;
			_hotSpotIntensity.Axis = this.XAxis;
      _hotSpotIntensity.ToolTipText = "light influence radius";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotIntensity);

      //color
      _hotSpotColor = new HotSpotColorButton(this,"LightColor");
      _hotSpotColor.ToolTipText = "Change the light's color";
      _hotSpotColor.Set2DOffset(10.0f,10.0f);
      EditorManager.ActiveView.HotSpots.Add(_hotSpotColor);

			// add more hotspots here...
		}

		/// <summary>
		/// Called when the shape is unselected
		/// </summary>
		public override void OnUnSelected()
		{
			// release the hotspots again:
			EditorManager.ActiveView.HotSpots.Remove(_hotSpotAngle);
      EditorManager.ActiveView.HotSpots.Remove(_hotSpotIntensity);
      EditorManager.ActiveView.HotSpots.Remove(_hotSpotColor);
      _hotSpotAngle.Remove();
      _hotSpotIntensity.Remove();
      _hotSpotColor.Remove();
			_hotSpotAngle = null;
			_hotSpotIntensity = null;
      _hotSpotColor = null;
			base.OnUnSelected ();
		}


		/// <summary>
		/// Called when user starts dragging a hotspot that belongs to this shape
		/// </summary>
		/// <param name="hotSpot"></param>
		/// <param name="view"></param>
		public override void OnHotSpotDragBegin(HotSpotBase hotSpot, VisionViewBase view)
		{
			if (hotSpot==_hotSpotAngle)
				_hotSpotAngle.StartAngle = SpotAngle;
			if (hotSpot==_hotSpotIntensity)
				_hotSpotIntensity.StartDistance = Intensity;
		}

		/// <summary>
		/// Called while the user drags a hotspot that belongs to this shape
		/// </summary>
		/// <param name="hotSpot"></param>
		/// <param name="view"></param>
		/// <param name="fDeltaX"></param>
		/// <param name="fDeltaY"></param>
		public override void OnHotSpotDrag(HotSpotBase hotSpot, VisionViewBase view, float fDeltaX, float fDeltaY)
		{
			if (hotSpot==_hotSpotAngle)
				SpotAngle = _hotSpotAngle.CurrentAngle; // update for the view
			if (hotSpot==_hotSpotIntensity)
				Intensity= _hotSpotIntensity.CurrentDistance; // update for the view
		}

		/// <summary>
		/// Called when the user releases the mouse button on a hotspot
		/// </summary>
		/// <param name="hotSpot"></param>
		/// <param name="view"></param>
		public override void OnHotSpotDragEnd(HotSpotBase hotSpot, VisionViewBase view)
		{
			if (hotSpot==_hotSpotAngle)
			{
				if (_hotSpotAngle.HasChanged)
				{
					float fNewAngle = _hotSpotAngle.CurrentAngle;
					SpotAngle = _hotSpotAngle.StartAngle; // set old angle for the action
					EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"SpotAngle",fNewAngle)); // send an action which sets the property from old value to new one
				}
			}

			if (hotSpot==_hotSpotIntensity)
			{
				if (_hotSpotIntensity.HasChanged)
				{
					float fNewDist = _hotSpotIntensity.CurrentDistance;
					Intensity = _hotSpotIntensity.StartDistance; // set old value for the action
					EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"Intensity",fNewDist)); // send an action which sets the property from old value to new one
				}
			}
		}

		/// <summary>
		/// Called every frame and per hotspot that belongs to the shape
		/// </summary>
		/// <param name="hotSpot"></param>
		public override void OnHotSpotEvaluatePosition(HotSpotBase hotSpot)
		{
			if (hotSpot==_hotSpotAngle)
			{
				if (!_hotSpotAngle.IsDragging)
					_hotSpotAngle.StartAngle = _fSpotAngle;
        _hotSpotAngle.Visible = (LightType == LightSourceType_e.Spotlight); // point light doesn't have a spot angle
      }
			if (hotSpot==_hotSpotIntensity)
			{
        if (!_hotSpotIntensity.IsDragging)
          _hotSpotIntensity.StartDistance = Intensity;
				_hotSpotIntensity.Axis = this.XAxis;
        _hotSpotIntensity.Visible = (LightType == LightSourceType_e.Point || LightType == LightSourceType_e.Spotlight);
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

      if (_hotSpotIntensity.Visible)
      {
        Vector3F vStart = Position;
        Vector3F vEnd = _hotSpotIntensity.Position;
        Vector3F vAng = _hotSpotAngle.Position;
        
        view.RenderLine(vStart, vEnd, VisionColors.Yellow, 1.0f);
      }
 }

		#endregion

    #region light info update

    /// <summary>
    /// Called by the scene if the static lighting update is finished
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    public override void OnSceneEvent(SceneEventArgs e)
    {
      base.OnSceneEvent(e);

      bool bNeedsDynUpdate = false;
      if (e.action == SceneEventArgs.Action.AfterStaticLighting && e.Success)
      {
        // for this combination, turn off the dynamic light preview
        if (_bTurnOffAfterLighting && _bStatic && _bPreviewStatic && !_bBakedToLightmap)
        {
          this._bBakedToLightmap = true;
          bNeedsDynUpdate = true;
          // update the property grid
          IScene.SendPropertyChangedEvent(new PropertyChangedArgs(this, "BakedToLightmap"));
        }
      }
      if (e.action == SceneEventArgs.Action.StaticLightingReset && _bBakedToLightmap)
      {
        this._bBakedToLightmap = false;
        bNeedsDynUpdate = true;
      }

      if (bNeedsDynUpdate)
      {
        if (this.Modifiable)
          this.Modified = true; // mark layer dirty so the flag gets saved
        if (HasEngineInstance())
          EngineLight.SetPreviewStatic(_bPreviewStatic && !_bBakedToLightmap);
        UpdateLightPreviewProperties();
      }

    }

    void UpdateLightPreviewProperties()
    {
      if (!HasEngineInstance())
        return;
      bool bWantsComponent = this.IsStatic && this.PreviewAsDynamicLight && this.CastStaticShadows && !this.BakedToLightmap;
      try
      {
        EngineLight.SetUsePreviewShadowComponent(bWantsComponent);
      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex);
      }
    }


    /// <summary>
    /// virtual function to collect information for static light update
    /// </summary>
    /// <param name="info"></param>
    protected override void GetStaticLightInfo(StaticLightInfo info)
    {
      if (!ExportAsStatic)
        return;
      info.RelevantLights.Add(this);
      if (HasEngineInstance())
        EngineLight.ResetStaticGeometryInstanceList();
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

    #region ILightSource Members

    bool ILightSource.Affects(BoundingBox bbox)
    {
      if (this.LightType == LightSourceType_e.Directional) // directional lights affect everything
        return true;
      float fDist = bbox.GetDistance(this.Position); // simple range check otherwise
      return fDist <= this.Radius;
    }

    bool ILightSource.IsOccluder(BoundingBox occluderBox, BoundingBox receiverBox)
    {
      if (occluderBox.Intersects(receiverBox))
        return true;

      if (this.LightType == LightSourceType_e.Directional)
      {
        Vector3F boxDiff = receiverBox.Center - occluderBox.Center;
        Vector3F lightDir = this.XAxis;
        // pointing into same direction?
        return (lightDir * boxDiff) > 0.0f;
      }

      // no special opt for spotlights
      return EngineInstanceDynLight.IsOccluderForPointLight(Position,Radius,occluderBox,receiverBox);
    }

    static BoundingBox infiniteBox = new BoundingBox(-1000000000.0f, -1000000000.0f, -1000000000.0f, 1000000000.0f, 1000000000.0f, 1000000000.0f);


    BoundingBox ILightSource.GetRangeBox()
    {
      if (this.LightType == LightSourceType_e.Directional)
        return infiniteBox;
      Vector3F center = Position;
      float r = this.Radius;
      return new BoundingBox(center.X - r, center.Y - r, center.Z - r, center.X + r, center.Y + r, center.Z + r);
    }

    #endregion
  }

  #region OmniLight Creator Plugin

  class OmniLightCreator : CSharpFramework.IShapeCreatorPlugin
  {

    bool _bStatic = false;

    public OmniLightCreator(bool bStatic)
    {
      _bStatic = bStatic;
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\Pointlight.bmp"), Color.Magenta);
    }

    public override string GetPluginName()
    {
      if (_bStatic)
        return "Lights (Static) - Omni Light";
      return "Lights (Dynamic) - Omni Light";
    }

    public override string GetPluginCategory()
    {
      return "Lighting";
    }

    /// <summary>
    /// Returns a short description text
    /// </summary>
    /// <returns></returns>
    public override string GetPluginDescription()
    {
      if (_bStatic)
        return "Adds a static omni-light to the scene.\n"+
          "Static lights are considered when launching vLux from vForge.";
      else
        return "Adds a dynamic omni-light to the scene.\n"+
          "Dynamic lights add a rendering pass to the affected geometry and thus they are performance critical.";
    }

    public override ShapeBase CreateShapeInstance()
    {
      string name = _bStatic ? "StaticLight" : "DynamicLight";
      DynLightShape light = new DynLightShape(name);
      light.LightType = LightSourceType_e.Point;
      light.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
      if (_bStatic)
      {
        light.ExportAsStatic = true;
        light.CastStaticShadows = true;
      }
      return light;
    }
  }

  #endregion 

  #region SpotLight Creator Plugin

  class SpotLightCreator : CSharpFramework.IShapeCreatorPlugin
  {
    bool _bStatic = false;

    public SpotLightCreator(bool bStatic)
    {
      _bStatic = bStatic;
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\flashlight.bmp"), Color.Magenta);
    }

    public override string GetPluginName()
    {
      if (_bStatic)
        return "Lights (Static) - Spot Light";
      return "Lights (Dynamic) - Spot Light";
    }

    public override string GetPluginCategory()
    {
      return "Lighting";
    }

    /// <summary>
    /// Returns a short description text
    /// </summary>
    /// <returns></returns>
    public override string GetPluginDescription()
    {
      if (_bStatic)
        return "Adds a static spot light to the scene.\n"+
          "Static lights are considered when launching vLux from vForge.";
      else
        return "Adds a dynamic spot light to the scene.\n"+
          "Dynamic lights add a rendering pass to the affected geometry and thus they are performance critical.";
    }

    public override ShapeBase CreateShapeInstance()
    {
      string name = _bStatic ? "StaticSpotlight" : "DynamicSpotlight";
      DynLightShape light = new DynLightShape(name);
      light.LightType = LightSourceType_e.Spotlight;
      light.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
      if (_bStatic)
      {
        light.ExportAsStatic = true;
        light.CastStaticShadows = true;
      }
      return light;
    }
  }
  #endregion 

  #region Directional Light Creator Plugin

  class DirectionalLightCreator : CSharpFramework.IShapeCreatorPlugin
  {
    bool _bStatic = false;

    public DirectionalLightCreator(bool bStatic)
    {
      _bStatic = bStatic;
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\Pointlight.bmp"), Color.Magenta);
      CategoryIconIndex = IconIndex;
    }

    public override string GetPluginName()
    {
      if (_bStatic)
        return "Lights (Static) - Directional Light";
      return "Lights (Dynamic) - Directional Light";
    }

    public override string GetPluginCategory()
    {
      return "Lighting";
    }

    /// <summary>
    /// Returns a short description text
    /// </summary>
    /// <returns></returns>
    public override string GetPluginDescription()
    {
      if (_bStatic)
        return "Adds a static directional light to the scene.\n"+
          "Static lights are considered when launching vLux from vForge.";
      else
        return "Adds a dynamic directional light to the scene.\n"+
          "Dynamic lights add a rendering pass to the affected geometry and thus they are performance critical.";
    }

    public override ShapeBase CreateShapeInstance()
    {
      string name = _bStatic ? "StaticDirectionalLight" : "DynamicDirectionalLight";
      DynLightShape light = new DynLightShape(name);
      light.LightType = LightSourceType_e.Directional;
      light.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
      if (_bStatic)
      {
        light.ExportAsStatic = true;
        light.CastStaticShadows = true;
      }
      return light;
    }
  }
  #endregion 

  #region Time of day light Creator Plugin

  class TimeOfDaySunCreator : CSharpFramework.IShapeCreatorPlugin
  {
    bool _bShadows = true;
    bool _bBackLight = true;

    public TimeOfDaySunCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\Sunglare.bmp"), Color.Magenta);
    }

    public override string GetPluginName()
    {
      return "Time-of-day sunlight";
    }

    public override string GetPluginCategory()
    {
      return "Lighting";
    }

    /// <summary>
    /// Returns a short description text
    /// </summary>
    /// <returns></returns>
    public override string GetPluginDescription()
    {
      return "Adds a dynamic directional (sun-)light to the scene that moves with time of day setup and optionally casts shadows.";
    }

    public override ShapeBase CreateShapeInstance()
    {
      if (!TestManager.IsRunning)
      {
        CreateTimeOfDayLightDlg dlg = new CreateTimeOfDayLightDlg();
        if (dlg.ShowDialog() != DialogResult.OK)
          return null;
        _bShadows = dlg.Shadows;
        _bBackLight = dlg.BackLight;
      }

      ShapeBase returnShape = null;
      DynLightShape light = new DynLightShape("Sunlight");
      returnShape = light;
      light.LightType = LightSourceType_e.Directional;
      light.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
      light.ExportAsStatic = false;
      light.CastModelShadows = _bShadows;
      light.CastWorldShadows = _bShadows;
      light.MakeTimeOfDayLight(1.0f, true, null);

      if (_bShadows)
      {
#if HK_ANARCHY
        ShapeComponentType t = (ShapeComponentType)EditorManager.EngineManager.ComponentClassManager.GetCollectionType("VMobileShadowMapComponentSpotDirectional");
        System.Diagnostics.Debug.Assert(t != null, "Cannot create component of type VMobileShadowMapComponentSpotDirectional");
#else
        ShapeComponentType t = (ShapeComponentType)EditorManager.EngineManager.ComponentClassManager.GetCollectionType("VShadowMapComponentSpotDirectional");
        System.Diagnostics.Debug.Assert(t != null, "Cannot create component of type VShadowMapComponentSpotDirectional");
#endif
        System.Diagnostics.Debug.Assert(t != null, "Cannot create component of type VShadowMapComponentSpotDirectional");
        if (t != null)
        {
          ShapeComponent comp = (ShapeComponent)t.CreateInstance(light);
          light.AddComponentInternal(comp);
        }
      }

      if (_bBackLight)
      {
        DynLightShape backlight = new DynLightShape("Back light");
        backlight.LightType = LightSourceType_e.Directional;
        backlight.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
        backlight.ExportAsStatic = false;
        backlight.MakeTimeOfDayBackLight();

        // return a group instead
        Group3DShape group = new Group3DShape("Sunlight shapes");
        group.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
        group.AddChild(light);
        group.AddChild(backlight);
        returnShape = group;
      }

      return returnShape;
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
