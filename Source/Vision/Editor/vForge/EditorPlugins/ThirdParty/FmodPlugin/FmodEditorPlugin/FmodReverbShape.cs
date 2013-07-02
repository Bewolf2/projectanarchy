/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.IO;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Design;
using System.ComponentModel;
using System.Globalization;
using CSharpFramework;
using CSharpFramework.Shapes;
using CSharpFramework.Actions;
using FmodManaged;
using System.Runtime.Serialization;
using ManagedFramework;
using CSharpFramework.PropertyEditors;
using CSharpFramework.View;
using CSharpFramework.Serialization;
using CSharpFramework.Math;
using CSharpFramework.Scene;

namespace FmodEditorPlugin
{

  #region class FmodReverbShape
	/// <summary>
  /// FmodReverbShape : This is the class that represents the reverb shape in the editor. It has an engine instance that handles the
	/// native code. The engine instance code is located in the FmodManaged project (managed C++ class library)
	/// </summary>
  [Serializable]
  public class FmodReverbShape : ShapeObject3D, IPropertyValueChangingCallback
  {
    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_REVERB = "Reverb";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_REVERB = Shape3D.LAST_CATEGORY_ORDER_ID + 1;

    /// <summary>
    /// last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_REVERB;

    #endregion

    #region Constructor

    /// <summary>
    /// Constructor: Takes the name of the shape
    /// </summary>
    /// <param name="name"></param>
    /// 
    public FmodReverbShape(string name) : base(name)
    {
      this._fReverbMin *= EditorManager.Settings.GlobalUnitScaling;
      this._fReverbMax *= EditorManager.Settings.GlobalUnitScaling;
    }

    #endregion

    #region Base Overrides

    protected override Shape3D.ParentingModes ParentingMode
    {
      get
      {
        return ParentingModes.SceneGraphScaled;
      }
    }

    #endregion

    #region GUI

    /// <summary>
    /// Overridden function to get the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetIconIndex()
    {
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\reverb.bmp"), Color.Magenta);
    }

    #endregion

    #region Engine Instance

    /// <summary>
    /// Function to create the engine instance of the shape. The engine instance is of type EngineInstanceFmodReverb
    /// and located in the managed code library.
    /// </summary>
    /// <param name="bCreateChildren">relevant for the base class to create instances for children</param>
    public override void CreateEngineInstance(bool bCreateChildren)
    {
      _engineInstance = new EngineInstanceFmodReverb();
      SetEngineInstanceBaseProperties(); // sets the position etc.
      base.CreateEngineInstance(bCreateChildren);
    }

    /// <summary>
    /// Helper field to access the engine instance as casted class to perform specific operations on it
    /// </summary>
    EngineInstanceFmodReverb EngineFmodReverb {get {return (EngineInstanceFmodReverb)_engineInstance;}}

    /// <summary>
    /// Overridden render function: Let the engine instance render itself
    /// </summary>
    /// <param name="view"></param>
    /// <param name="mode"></param>
    public override void RenderShape(VisionViewBase view, ShapeRenderMode mode)
    {
      base.RenderShape(view, mode);
      if (HasEngineInstance())
        EngineFmodReverb.RenderShape(view, mode);
    }

    /// <summary>
    /// Set all properties on the engine instance. The base implementation already sets position and orientation,
    /// so there is nothing else to do here
    /// </summary>
    public override void SetEngineInstanceBaseProperties()
    {
      if (_engineInstance==null)
        return;
      base.SetEngineInstanceBaseProperties(); // now set position etc.
      EngineFmodReverb.InitReverb(_reverbProperties, _fReverbMin, _fReverbMax);
    }

    /// <summary>
    /// Pick the shape in the view. Use helper function to trace the local bounding box.
    /// </summary>
    /// <param name="rayStart">trace ray start</param>
    /// <param name="rayEnd">trace ray end</param>
    /// <param name="result">result structure to fill in</param>
    public override void OnTraceShape(ShapeTraceMode_e mode, Vector3F rayStart, Vector3F rayEnd, ref ShapeTraceResult result)
    {
      if (mode!=ShapeTraceMode_e.ShapePicking)
        return;
      if (ConversionUtils.TraceOrientedBoundingBox(LocalBoundingBox, Position, RotationMatrix, rayStart, rayEnd, ref result))
        result.hitShape = this;
    }

    #endregion

    #region Cloning

    /// <summary>
    /// This function must be overridden, since we have to reset the hotspots
    /// </summary>
    /// <returns></returns>
    public override ShapeBase Clone()
    {
      FmodReverbShape copy = (FmodReverbShape)base.Clone();
      System.Diagnostics.Debug.Assert(!copy.Selected); // turned off in ShapeBase

      copy._hotSpotReverbMin = null;
      copy._hotSpotReverbMax = null;
   
      return copy;
    }

    #endregion

    #region Reverb properties

    FmodReverbProps _reverbProperties = new FmodReverbProps();
    float _fReverbMin = 300.0f;
    float _fReverbMax = 400.0f;
    bool _bActive = true;

    void SetCustomPreset()
    {
      _reverbProperties.m_presetType = FmodReverbPresets_e.Custom;
      _reverbProperties.m_iEnvironment = (int)FmodReverbPresetEnvironment_e.Custom; 
    }

    /// <summary>
    /// Enum that defines which preset should be used
    /// </summary>
    [SortedCategory(CAT_REVERB, CATORDER_REVERB), 
    PropertyOrder(1)]
    [Description("Enum that defines which preset should be used"), DefaultValue(FmodReverbPresets_e.Generic)]
    [DependentProperties(new string[] { "Room", "RoomHF", "RoomLF", "DecayTime", "DecayHFRatio", "Reflections", "ReflectionsDelay", "Reverb", "ReverbDelay", "HFReference", "LFReference", "Diffusion", "Density" })]
    public FmodReverbPresets_e Preset 
    {
      get { return _reverbProperties.m_presetType; }
      set
      {
        if (_reverbProperties.m_presetType == value) return;
        _reverbProperties.m_presetType = value;
        _reverbProperties = FmodManaged.ManagedModule.GetReverbPreset(_reverbProperties.m_presetType);
        if (HasEngineInstance())
          EngineFmodReverb.SetReverbProperties(_reverbProperties);
      }
    }

    /// <summary>
    /// Defines the room effect level (at mid frequencies) in the range of -10000..0
    /// </summary>
    [SortedCategory(CAT_REVERB, CATORDER_REVERB),
    PropertyOrder(2),
    Description("Defines the room effect level (at mid frequencies) in the range of -10000..0")]
    [EditorAttribute(typeof(SliderEditor),typeof(UITypeEditor)), SliderRange(-10001, 0, 10001)]
    [DependentProperties(new string[] { "Preset" })]
    public int Room
    {
      get {return _reverbProperties.m_iRoom;}
      set
      {
        if (_reverbProperties.m_iRoom==value) return;
        _reverbProperties.m_iRoom = value;
        SetCustomPreset();
        if (_reverbProperties.m_iRoom<-10000) _reverbProperties.m_iRoom=-10000;
          else if (_reverbProperties.m_iRoom>0) _reverbProperties.m_iRoom=0;
        if (HasEngineInstance())
          EngineFmodReverb.SetReverbProperties(_reverbProperties);
      }
    }

    /// <summary>
    /// Defines the relative room effect level at high frequencies in the range of -10000..0
    /// </summary>
    [SortedCategory(CAT_REVERB, CATORDER_REVERB),
    PropertyOrder(3),
    Description("Defines the relative room effect level at high frequencies in the range of -10000..0")]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(-10001, 0, 10001)]
    [DependentProperties(new string[] { "Preset" })]
    public int RoomHF
    {
      get { return _reverbProperties.m_iRoomHF; }
      set
      {
        if (_reverbProperties.m_iRoomHF == value) return;
        _reverbProperties.m_iRoomHF = value;
        SetCustomPreset();
        if (_reverbProperties.m_iRoomHF < -10000) _reverbProperties.m_iRoomHF = -10000;
        else if (_reverbProperties.m_iRoomHF > 0) _reverbProperties.m_iRoomHF = 0;
        if (HasEngineInstance())
          EngineFmodReverb.SetReverbProperties(_reverbProperties);
      }
    }

    /// <summary>
    /// Defines the relative room effect level at low frequencies in the range of -10000..0
    /// </summary>
    [SortedCategory(CAT_REVERB, CATORDER_REVERB),
    PropertyOrder(4),
    Description("Defines the relative room effect level at low frequencies in the range of -10000..0")]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(-10001, 0, 10001)]
    [DependentProperties(new string[] { "Preset" })]
    public int RoomLF
    {
      get { return _reverbProperties.m_iRoomLF; }
      set
      {
        if (_reverbProperties.m_iRoomLF == value) return;
        _reverbProperties.m_iRoomLF = value;
        SetCustomPreset();
        if (_reverbProperties.m_iRoomLF < -10000) _reverbProperties.m_iRoomLF = -10000;
        else if (_reverbProperties.m_iRoomLF > 0) _reverbProperties.m_iRoomLF = 0;
        if (HasEngineInstance())
          EngineFmodReverb.SetReverbProperties(_reverbProperties);
      }
    }

    /// <summary>
    /// Defines the reverberation decay time at mid frequencies in the range of 0.1..20.0
    /// </summary>
    [SortedCategory(CAT_REVERB, CATORDER_REVERB),
    PropertyOrder(5),
    Description("Defines the reverberation decay time at mid frequencies in the range of 0.1..20.0")]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.1f, 20.0f, 1000)]
    [DependentProperties(new string[] { "Preset" })]
    public float DecayTime
    {
      get { return _reverbProperties.m_fDecayTime; }
      set
      {
        if (_reverbProperties.m_fDecayTime == value) return;
        _reverbProperties.m_fDecayTime = value;
        SetCustomPreset();
        if (_reverbProperties.m_fDecayTime < 0.1f) _reverbProperties.m_fDecayTime = 0.1f;
        else if (_reverbProperties.m_fDecayTime > 19.99f) _reverbProperties.m_fDecayTime = 20.0f;
        if (HasEngineInstance())
          EngineFmodReverb.SetReverbProperties(_reverbProperties);
      }
    } 

    /// <summary>
    /// Defines the high-frequency to mid-frequency decay time ratio in the range of 0.1..2.0
    /// </summary>
    [SortedCategory(CAT_REVERB, CATORDER_REVERB),
    PropertyOrder(6),
    Description("Defines the high-frequency to mid-frequency decay time ratio in the range of 0.1..2.0")]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.1f, 2.0f, 1000)]
    [DependentProperties(new string[] { "Preset" })]
    public float DecayHFRatio
    {
      get { return _reverbProperties.m_fDecayHFRatio; }
      set
      {
        if (_reverbProperties.m_fDecayHFRatio == value) return;
        _reverbProperties.m_fDecayHFRatio = value;
        SetCustomPreset();
        if (_reverbProperties.m_fDecayHFRatio < 0.1f) _reverbProperties.m_fDecayHFRatio = 0.1f;
        else if (_reverbProperties.m_fDecayHFRatio > 1.99f) _reverbProperties.m_fDecayHFRatio = 2.0f;
        if (HasEngineInstance())
          EngineFmodReverb.SetReverbProperties(_reverbProperties);
      }
    }

    /// <summary>
    /// Defines the early reflections level relative to room effect in the range of -10000..1000
    /// </summary>
    [SortedCategory(CAT_REVERB, CATORDER_REVERB),
    PropertyOrder(7),
    Description("Defines the early reflections level relative to room effect in the range of -10000..1000")]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(-10001, 1000, 11001)]
    [DependentProperties(new string[] { "Preset" })]
    public int Reflections
    {
      get { return _reverbProperties.m_iReflections; }
      set
      {
        if (_reverbProperties.m_iReflections == value) return;
        _reverbProperties.m_iReflections = value;
        SetCustomPreset();
        if (_reverbProperties.m_iReflections < -10000) _reverbProperties.m_iReflections = -10000;
        else if (_reverbProperties.m_iReflections > 1000) _reverbProperties.m_iReflections = 1000;
        if (HasEngineInstance())
          EngineFmodReverb.SetReverbProperties(_reverbProperties);
      }
    }

    /// <summary>
    /// Defines the initial reflection delay time in the range of 0.0..0.3
    /// </summary>
    [SortedCategory(CAT_REVERB, CATORDER_REVERB),
    PropertyOrder(8),
    Description("Defines the initial reflection delay time in the range of 0.0..0.3")]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.0f, 0.3f, 100)]
    [DependentProperties(new string[] { "Preset" })]
    public float ReflectionsDelay
    {
      get { return _reverbProperties.m_fReflectionsDelay; }
      set
      {
        if (_reverbProperties.m_fReflectionsDelay == value) return;
        _reverbProperties.m_fReflectionsDelay = value;
        SetCustomPreset();
        if (_reverbProperties.m_fReflectionsDelay < 0.0f) _reverbProperties.m_fReflectionsDelay = 0.0f;
        else if (_reverbProperties.m_fReflectionsDelay > 0.3f) _reverbProperties.m_fReflectionsDelay = 0.3f;
        if (HasEngineInstance())
          EngineFmodReverb.SetReverbProperties(_reverbProperties);
      }
    }

    /// <summary>
    /// Defines the late reverberation level relative to room effect in the range of -10000..2000
    /// </summary>
    [SortedCategory(CAT_REVERB, CATORDER_REVERB),
    PropertyOrder(9),
    Description("Defines the late reverberation level relative to room effect in the range of -10000..2000")]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(-10001, 2000, 12001)]
    [DependentProperties(new string[] { "Preset" })]
    public int Reverb
    {
      get { return _reverbProperties.m_iReverb; }
      set
      {
        if (_reverbProperties.m_iReverb == value) return;
        _reverbProperties.m_iReverb = value;
        SetCustomPreset();
        if (_reverbProperties.m_iReverb < -10000) _reverbProperties.m_iReverb = -10000;
        else if (_reverbProperties.m_iReverb > 2000) _reverbProperties.m_iReverb = 2000;
        if (HasEngineInstance())
          EngineFmodReverb.SetReverbProperties(_reverbProperties);
      }
    }

    /// <summary>
    /// Defines the late reverberation delay time relative to initial reflection in the range of 0.0..0.1
    /// </summary>
    [SortedCategory(CAT_REVERB, CATORDER_REVERB),
    PropertyOrder(10),
    Description("Defines the late reverberation delay time relative to initial reflection in the range of 0.0..0.1")]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.0f, 0.1f, 100)]
    [DependentProperties(new string[] { "Preset" })]
    public float ReverbDelay
    {
      get { return _reverbProperties.m_fReverbDelay; }
      set
      {
        if (_reverbProperties.m_fReverbDelay == value) return;
        _reverbProperties.m_fReverbDelay = value;
        SetCustomPreset();
        if (_reverbProperties.m_fReverbDelay < 0.0f) _reverbProperties.m_fReverbDelay = 0.0f;
        else if (_reverbProperties.m_fReverbDelay > 0.1f) _reverbProperties.m_fReverbDelay = 0.1f;
        if (HasEngineInstance())
          EngineFmodReverb.SetReverbProperties(_reverbProperties);
      }
    }

    /// <summary>
    /// Defines the reference high frequency (hz) in the range of 1000.0..20000.0
    /// </summary>
    [SortedCategory(CAT_REVERB, CATORDER_REVERB),
    PropertyOrder(11),
    Description("Defines the reference high frequency (hz) in the range of 1000.0..20000.0")]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(1000.0f, 20000.0f, 210000)]
    [DependentProperties(new string[] { "Preset" })]
    public float HFReference
    {
      get { return _reverbProperties.m_fHFReference; }
      set
      {
        if (_reverbProperties.m_fHFReference == value) return;
        _reverbProperties.m_fHFReference = value;
        SetCustomPreset();
        if (_reverbProperties.m_fHFReference < 1000.0f) _reverbProperties.m_fHFReference = 1000.0f;
        else if (_reverbProperties.m_fHFReference > 19999.9f) _reverbProperties.m_fHFReference = 20000.0f;
        if (HasEngineInstance())
          EngineFmodReverb.SetReverbProperties(_reverbProperties);
      }
    }

    /// <summary>
    /// Defines the reference low frequency (hz) in the range of 20.0..1000.0
    /// </summary>
    [SortedCategory(CAT_REVERB, CATORDER_REVERB),
    PropertyOrder(12),
    Description("Defines the reference low frequency (hz) in the range of 20.0..1000.0")]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(20.0f, 1000.0f, 10200)]
    [DependentProperties(new string[] { "Preset" })]
    public float LFReference
    {
      get { return _reverbProperties.m_fLFReference; }
      set
      {
        if (_reverbProperties.m_fLFReference == value) return;
        _reverbProperties.m_fLFReference = value;
        SetCustomPreset();
        if (_reverbProperties.m_fLFReference < 20.0f) _reverbProperties.m_fLFReference = 20.0f;
        else if (_reverbProperties.m_fLFReference > 999.9f) _reverbProperties.m_fLFReference = 1000.0f;
        if (HasEngineInstance())
          EngineFmodReverb.SetReverbProperties(_reverbProperties);
      }
    }

    /// <summary>
    /// Defines the value that controls the echo density in the late reverberation decay in the range of 0.0..100.0
    /// </summary>
    [SortedCategory(CAT_REVERB, CATORDER_REVERB),
    PropertyOrder(13),
    Description("Defines the value that controls the echo density in the late reverberation decay in the range of 0.0..100.0")]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.0f, 100.0f, 1000)]
    [DependentProperties(new string[] { "Preset" })]
    public float Diffusion
    {
      get { return _reverbProperties.m_fDiffusion; }
      set
      {
        if (_reverbProperties.m_fDiffusion == value) return;
        _reverbProperties.m_fDiffusion = value;
        SetCustomPreset();
        if (_reverbProperties.m_fDiffusion < 0.0f) _reverbProperties.m_fDiffusion = 0.0f;
        else if (_reverbProperties.m_fDiffusion > 100.0f) _reverbProperties.m_fDiffusion = 100.0f;
        if (HasEngineInstance())
          EngineFmodReverb.SetReverbProperties(_reverbProperties);
      }
    }

    /// <summary>
    /// Defines the value that controls the modal density in the late reverberation decay in the range of 0.0..100.0
    /// </summary>
    [SortedCategory(CAT_REVERB, CATORDER_REVERB),
    PropertyOrder(14),
    Description("Defines the value that controls the modal density in the late reverberation decay in the range of 0.0..100.0")]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.0f, 100.0f, 1000)]
    [DependentProperties(new string[] { "Preset" })]
    public float Density
    {
      get { return _reverbProperties.m_fDensity; }
      set
      {
        if (_reverbProperties.m_fDensity == value) return;
        _reverbProperties.m_fDensity = value;
        SetCustomPreset();
        if (_reverbProperties.m_fDensity < 0.0f) _reverbProperties.m_fDensity = 0.0f;
        else if (_reverbProperties.m_fDensity > 100.0f) _reverbProperties.m_fDensity = 100.0f;
        if (HasEngineInstance())
          EngineFmodReverb.SetReverbProperties(_reverbProperties);
      }
    }

    /// <summary>
    /// Defines the distance (in world units) within which the reverb has full effect
    /// </summary>
    [SortedCategory(CAT_REVERB, CATORDER_REVERB),
    PropertyOrder(15),
    Description("Defines the distance (in world units) within which the reverb has full effect")]
    public float MinDistance
    {
      get {return _fReverbMin;}
      set
      {
        if (_fReverbMin==value) return;
        _fReverbMin = value;
        if (_fReverbMin>_fReverbMax-1) _fReverbMin = _fReverbMax-1;
        if (_fReverbMin<0.0f) _fReverbMin = 0.0f;
        if (HasEngineInstance())
          EngineFmodReverb.SetReverbDistances(_fReverbMin * UniformScaling, _fReverbMax * UniformScaling);
      }
    }

    /// <summary>
    /// Defines the distance (in world units) within which the reverb has full effect
    /// </summary>
    [SortedCategory(CAT_REVERB, CATORDER_REVERB),
    PropertyOrder(16),
    Description("Defines the distance (in world units) outside of which the reverb has zero effect")]
    public float MaxDistance
    {
      get {return _fReverbMax;}
      set
      {
        if (_fReverbMax==value) return;
        _fReverbMax = value;
        if (_fReverbMax<1.0f) _fReverbMax = 1.0f;
        if (_fReverbMax<_fReverbMin+1) _fReverbMax = _fReverbMin+1;
        if (HasEngineInstance())
          EngineFmodReverb.SetReverbDistances(_fReverbMin * UniformScaling, _fReverbMax * UniformScaling);
      }
    }
    
    #endregion

    #region IPropertyValueChangingCallback Members

    public void OnPropertyValueChanging(object Instance, PropertyDescriptor propertyDesc, object newValue)
    {
      if (object.ReferenceEquals(Instance, this) && HasEngineInstance())
      {
        switch (propertyDesc.Name)
        {
          case "Room":
            Room = (int)newValue;
            break;

          case "RoomHF":
            RoomHF = (int)newValue;
            break;

          case "RoomLF":
            RoomLF = (int)newValue;
            break;

          case "DecayTime":
            DecayTime = (float)newValue;
            break;

          case "DecayHFRatio":
            DecayHFRatio = (float)newValue;
            break;

          case "Reflections":
            Reflections = (int)newValue;
            break;

          case "ReflectionsDelay":
            ReflectionsDelay = (float)newValue;
            break;

          case "Reverb":
            Reverb = (int)newValue;
            break;

          case "ReverbDelay":
            ReverbDelay = (float)newValue;
            break;

          case "HFReference":
            HFReference = (float)newValue;
            break;

          case "LFReference":
            LFReference = (float)newValue;
            break;

          case "Diffusion":
            Diffusion = (float)newValue;
            break;

          case "Density":
            Density = (float)newValue;
            break;
        }
      }
    }

    #endregion

    #region Selection and hot spots

    HotSpotDistance _hotSpotReverbMin, _hotSpotReverbMax; // hotspots for reverb distances

    public override void OnAddedToScene()
    {
      base.OnAddedToScene();
      AddHint(HintFlags_e.OnlyUniformScale);
    }

    public override void OnTransformationChanged()
    {
      base.OnTransformationChanged();

      // scaling might have changed:
      if (HasEngineInstance())
        EngineFmodReverb.SetReverbDistances(_fReverbMin * UniformScaling, _fReverbMax * UniformScaling);
      if (_hotSpotReverbMin != null)
        _hotSpotReverbMin.DisplayScaling = UniformScaling;
      if (_hotSpotReverbMax != null)
        _hotSpotReverbMax.DisplayScaling = UniformScaling;
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    public override void OnSelected()
    {
      base.OnSelected ();

      // hotspots for reverb distances
      _hotSpotReverbMin = new HotSpotDistance(this, @"textures\Hotspot_D.tga", VisionColors.White, HotSpotBase.PickType.Square, 4.0f);
      _hotSpotReverbMin.StartDistance = MinDistance;
      _hotSpotReverbMin.Axis = this.XAxis;
      _hotSpotReverbMin.DisplayScaling = UniformScaling;
      _hotSpotReverbMin.ToolTipText = "min distance";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotReverbMin);
      _hotSpotReverbMax = new HotSpotDistance(this, @"textures\Hotspot_D.tga", VisionColors.White, HotSpotBase.PickType.Square, 4.0f);
      _hotSpotReverbMax.StartDistance = MaxDistance;
      _hotSpotReverbMax.Axis = this.XAxis;
      _hotSpotReverbMax.DisplayScaling = UniformScaling;
      _hotSpotReverbMax.ToolTipText = "max distance";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotReverbMax);
    }

    void SafeRemoveHotSpot(HotSpotBase hotSpot)
    {
      if (hotSpot==null) 
        return;
      EditorManager.ActiveView.HotSpots.Remove(hotSpot);
      hotSpot.Remove();
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    public override void OnUnSelected()
    {
      // release the hotspots again:
      SafeRemoveHotSpot(_hotSpotReverbMin); _hotSpotReverbMin = null;
      SafeRemoveHotSpot(_hotSpotReverbMax); _hotSpotReverbMax = null;

      base.OnUnSelected();
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
      if (hotSpot==_hotSpotReverbMin)
        MinDistance = _hotSpotReverbMin.CurrentDistance; 
      if (hotSpot==_hotSpotReverbMax)
        MaxDistance = _hotSpotReverbMax.CurrentDistance; 
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragBegin(CSharpFramework.View.HotSpotBase hotSpot, VisionViewBase view)
    {
      if (hotSpot==_hotSpotReverbMin)
        _hotSpotReverbMin.StartDistance = this.MinDistance;
      if (hotSpot==_hotSpotReverbMax)
        _hotSpotReverbMax.StartDistance = this.MaxDistance;
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragEnd(CSharpFramework.View.HotSpotBase hotSpot, VisionViewBase view)
    {
      if (hotSpot==_hotSpotReverbMin)
      {
        if (_hotSpotReverbMin.HasChanged)
        {
          float fNewVal = _hotSpotReverbMin.CurrentDistance;
          MinDistance = _hotSpotReverbMin.StartDistance; // set old value for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this, "MinDistance", fNewVal)); 
        }
      }
      if (hotSpot==_hotSpotReverbMax)
      {
        if (_hotSpotReverbMax.HasChanged)
        {
          float fNewVal = _hotSpotReverbMax.CurrentDistance;
          MaxDistance = _hotSpotReverbMax.StartDistance; // set old value for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this, "MaxDistance", fNewVal)); 
        }
      }
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="hotSpot"></param>
    public override void OnHotSpotEvaluatePosition(CSharpFramework.View.HotSpotBase hotSpot)
    {
      if (hotSpot==_hotSpotReverbMin)
      {
        _hotSpotReverbMin.Axis = this.XAxis;
        if (!_hotSpotReverbMin.IsDragging)
          _hotSpotReverbMin.StartDistance = MinDistance;
      }
      if (hotSpot==_hotSpotReverbMax)
      {
        _hotSpotReverbMax.Axis = this.XAxis;
        if (!_hotSpotReverbMax.IsDragging)
          _hotSpotReverbMax.StartDistance = MaxDistance;
      }
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    /// <param name="mode"></param>
    public override void OnHotSpotRender(HotSpotBase hotSpot, VisionViewBase view, ShapeRenderMode mode)
    {
      base.OnHotSpotRender(hotSpot, view, mode);
    }

    #endregion

    #region Serialization and Export

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected FmodReverbShape(SerializationInfo info, StreamingContext context) : base(info, context)
    {
      _reverbProperties = (FmodReverbProps)info.GetValue("_reverbProperties", typeof(FmodReverbProps));
      _fReverbMin = info.GetSingle("_fReverbMin");
      _fReverbMax = info.GetSingle("_fReverbMax");
      _bActive = info.GetBoolean("_bActive");
    }
    
  
    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context)
    {
      base.GetObjectData (info, context);

      info.AddValue("_reverbProperties", _reverbProperties);
      info.AddValue("_fReverbMin", _fReverbMin);
      info.AddValue("_fReverbMax", _fReverbMax);
      info.AddValue("_bActive", _bActive);
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

  #region Shape Creator Plugin


  /// <summary>
  /// Creator class. An instance of the creator is registered in the plugin init function. Thus the creator shows
  /// up in the "Create" menu of the editor
  /// </summary>
  class FmodReverbShapeCreator : CSharpFramework.IShapeCreatorPlugin
  {

    /// <summary>
    /// Constructor
    /// </summary>
    public FmodReverbShapeCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\reverb.bmp"), Color.Magenta);
    }

    /// <summary>
    /// Get the name of the plugin, for instance the shape name. This name appears in the "create" menu
    /// </summary>
    /// <returns>creator name</returns>
    public override string GetPluginName()
    {
      return "Reverb";
    }

    /// <summary>
    /// Gets the plugin description
    /// </summary>
    /// <returns></returns>
    public override string GetPluginDescription()
    {
      return "This shape represents a reverb in the world.";
    }

    /// <summary>
    /// Get the plugin category name to sort the plugin name. This is useful to group creators. A null string can
    /// be returned to put the creator in the root
    /// </summary>
    /// <returns></returns>
    public override string GetPluginCategory()
    {
      return "Fmod";
    }


    /// <summary>
    /// Create a shape instance
    /// </summary>
    /// <returns></returns>
    public override ShapeBase CreateShapeInstance()
    {
      // create a reverb
      FmodReverbShape Reverb = new FmodReverbShape("Fmod - Reverb");
      Reverb.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
      return Reverb;
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
