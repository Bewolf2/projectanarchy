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

  #region class FmodSoundShape
	/// <summary>
  /// FmodSoundShape : This is the class that represents the sound shape in the editor. It has an engine instance that handles the
	/// native code. The engine instance code is located in the FmodManaged project (managed C++ class library)
	/// </summary>
  [Serializable]
  public class FmodSoundShape : ShapeObject3D, IPropertyValueChangingCallback
  {
    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_SOUNDRES = "Sound Resource";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_SOUNDRES = Shape3D.LAST_CATEGORY_ORDER_ID + 1;

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_SOUND = "Sound Instance";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_SOUND = Shape3D.LAST_CATEGORY_ORDER_ID + 2;

    /// <summary>
    /// last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_SOUND;

    #endregion

    #region Constructor

    /// <summary>
    /// The constructor of the Fmod Sound shape, just takes the Sound name
    /// </summary>
    /// <param name="name">Name of the shape in the shape tree</param>
    public FmodSoundShape(string name) : base(name)
    {
      this._fFadeMin *= EditorManager.Settings.GlobalUnitScaling;
      this._fFadeMax *= EditorManager.Settings.GlobalUnitScaling;
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

    public override PropertyFlags_e GetPropertyFlags(PropertyFlagsProviderInfo pd)
    {
      PropertyFlags_e flags = base.GetPropertyFlags(pd);

      if (!_b3D)
      {
        if (pd.Name == "MinDistance" || pd.Name == "MaxDistance" || pd.Name == "Directional" || pd.Name == "ConeInside" ||
            pd.Name == "ConeOutside" || pd.Name == "LogAttenuation")
          return PropertyFlags_e.Readonly;
      }
      else
      {
        if(!_bDirectional)
        {
          if (pd.Name == "ConeInside" || pd.Name == "ConeOutside")
            return PropertyFlags_e.Readonly;
        }
        if (pd.Name == "Pan")
          return PropertyFlags_e.Readonly;
      }
      
      return flags;
    }

    #endregion

    #region GUI

    /// <summary>
    /// Overridden function to get the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetIconIndex()
    {
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\loudspeaker.bmp"), Color.Magenta);
    }

    #endregion

    #region Engine Instance

    /// <summary>
    /// Function to create the engine instance of the shape. The engine instance is of type EngineInstanceFmodSound
    /// and located in the managed code library.
    /// </summary>
    /// <param name="bCreateChildren">relevant for the base class to create instances for children</param>
    public override void CreateEngineInstance(bool bCreateChildren)
    {
      _engineInstance = new EngineInstanceFmodSound();
      SetEngineInstanceBaseProperties(); // sets the position etc.

      // Call the base function to setup attachments and process the children. The engine instance
      // has to exist at this point.
      base.CreateEngineInstance(bCreateChildren);
    }

    /// <summary>
    /// Helper field to access the engine instance as casted class to perform specific operations on it
    /// </summary>
    EngineInstanceFmodSound EngineFmodSound {get {return (EngineInstanceFmodSound)_engineInstance;}}

    /// <summary>
    /// Overridden render function: Let the engine instance render itself
    /// </summary>
    /// <param name="view"></param>
    /// <param name="mode"></param>
    public override void RenderShape(VisionViewBase view, ShapeRenderMode mode)
    {
      base.RenderShape(view, mode);
      if (HasEngineInstance())
      {
        EngineFmodSound.RenderShape(view, mode);

        if (_hotSpotPlaySound != null)
        {
          // render again to keep the button state updated
          if (EngineFmodSound.IsPlaying())
            EditorManager.ActiveView.UpdateView(false);
          UpdatePlayIcon();
        }
      }
    }

    /// <summary>
    /// Set all properties on the engine instance. The base implementation already sets position and orientation,
    /// so there is nothing else to do here
    /// </summary>
    public override void SetEngineInstanceBaseProperties()
    {
      if (_engineInstance==null)
        return;
      EngineFmodSound.InitSound(_filename, SoundFlags | FmodCreationFlags_e.Paused, _iPriority); // first, set the file, so the native instance is created
      base.SetEngineInstanceBaseProperties (); // now set position etc.
      EngineFmodSound.SetVolume(_fVolume); 
      EngineFmodSound.SetPitch(_fPitch);
      EngineFmodSound.SetPan(_fPan);
      EngineFmodSound.Set3DFadeDistance(_fFadeMin*UniformScaling, _fFadeMax*UniformScaling);
      EngineFmodSound.SetConeAngles(_bDirectional, _fConeInside, _fConeOutside);

      // Only play the sound in case we are in play mode (not in editing mode)
      if ((SoundFlags & FmodCreationFlags_e.Paused) == 0)
      {
        EngineFmodSound.Play(false);

        // The native class will not play the sound if we are in editing mode, and thus
        // keep the sound paused. Make sure that it is marked as playing, so that the flag
        // is exported correctly.
        EngineFmodSound.RemoveFlag(FmodCreationFlags_e.Paused);
      }

      UpdateHotspotVisibility();
      RecreateCustomLinks();
    }

    void OnSoundPropertyChanged()
    {
      RemoveNativeComponents(); // since the native object will change, also re-assign new native components
      SetEngineInstanceBaseProperties();
      ReAttachComponents();
      ReassignAllLinks();
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
      FmodSoundShape copy = (FmodSoundShape)base.Clone();
      System.Diagnostics.Debug.Assert(!copy.Selected); // turned off in ShapeBase

      copy._hotSpotVolume = null;
      copy._hotSpotFadeMin = null;
      copy._hotSpotFadeMax = null;
      copy._hotSpotPlaySound = null;
      copy._hotSpotOuterCone = null;
      copy._hotSpotInnerCone = null;

      return copy;
    }

    #endregion

    #region Sound properties

    string _filename = "";
    bool _b3D = true;
    bool _bStreaming = false;
    bool _bLooped = true;
    bool _bStartPaused = false;
    bool _bDirectional = false;
    bool _bDisposeWhenFinished = true;
    bool _bMusic = false;
    bool _bCompressedData = false;
    bool _bLogAttenuation = false;
    float _fVolume = 1.0f;
    float _fPitch = 1.0f;
    float _fPan = 0.0f;
    float _fFadeMin = 100.0f;
    float _fFadeMax = 1000.0f;
    float _fTimeOfs = 0.0f; // TODO
    int _iPriority = 128;
    float _fConeOutside = 60.0f;
    float _fConeInside = 45.0f;

    [Browsable(false)]
    protected FmodCreationFlags_e SoundFlags
    {
      get
      {
        FmodCreationFlags_e flags = FmodCreationFlags_e.None;
        if (_b3D)           flags |= FmodCreationFlags_e.Is3D;
        if (_bStreaming)    flags |= FmodCreationFlags_e.Streaming;
        if (_bLooped)       flags |= FmodCreationFlags_e.Looped;
        if (_bStartPaused)  flags |= FmodCreationFlags_e.Paused;
        if (!_bDisposeWhenFinished) flags |= FmodCreationFlags_e.NoDispose;
        if (_bMusic) flags |= FmodCreationFlags_e.Music;
        if (_bCompressedData) flags |= FmodCreationFlags_e.CompressedData;
        if (_bLogAttenuation) flags |= FmodCreationFlags_e.LogAttenuation;
        return flags;
      }
    }

    /// <summary>
    /// Gets or sets the WAVE file that should be played
    /// </summary>
    [PrefabResolveFilename]
    [SortedCategory(CAT_SOUNDRES, CATORDER_SOUNDRES),
    PropertyOrder(1),
    EditorAttribute(typeof(FilenameEditor), typeof(UITypeEditor)), 
    FileDialogFilter(new string[] {".wav", ".mp3", ".ogg"})]
    [Description("The wave file to be played")]
    public string Filename
    {
      get {return _filename;}
      set 
      {
        if (_filename==value) return;
        _filename = value;
        OnSoundPropertyChanged(); // set everything because a sound instance is to be created
      }
    }

    /// <summary>
    /// If enabled, the sound is a 3D sound with attenuation, otherwise it is an ambient sound
    /// </summary>
    [SortedCategory(CAT_SOUNDRES, CATORDER_SOUNDRES),
    PropertyOrder(2),
    Description("If enabled, the sound is a 3D sound with attenuation, otherwise it is an ambient sound")]
    public bool Is3D
    {
      get {return _b3D;}
      set
      {
        if (_b3D==value) return;
        _b3D = value;
        OnSoundPropertyChanged(); // set everything because a sound instance is to be created
      }
    }

    /// <summary>
    /// If enabled, the sound is dynamically streaming from disc. Recommended for larger sound files.
    /// </summary>
    [SortedCategory(CAT_SOUNDRES, CATORDER_SOUNDRES),
    PropertyOrder(3),
    Description("If enabled, the sound is dynamically streaming from disc. Recommended for larger sound files.")]
    public bool Streaming
    {
      get {return _bStreaming;}
      set
      {
        if (_bStreaming==value) return;
        _bStreaming = value;
        OnSoundPropertyChanged(); // set everything because a sound instance is to be created
      }
    }

    /// <summary>
    /// If enabled, sound data is kept compressed in memory and decompressed at runtime during playback.
    /// </summary>
    [SortedCategory(CAT_SOUNDRES, CATORDER_SOUNDRES),
    PropertyOrder(4),
    Description("If enabled, sound data is kept compressed in memory and decompressed at runtime during playback.")]
    public bool CompressedData
    {
      get { return _bCompressedData; }
      set
      {
        if (_bCompressedData == value) return;
        _bCompressedData = value;
        OnSoundPropertyChanged(); // set everything because a sound instance is to be created
      }
    }

    /// <summary>
    /// If enabled, the 3D sound will have logarithmic attenuation, otherwise linear.
    /// </summary>
    [SortedCategory(CAT_SOUNDRES, CATORDER_SOUNDRES),
    PropertyOrder(5),
    Description("If enabled, the 3D sound will have logarithmic attenuation, otherwise linear.")]
    public bool LogAttenuation
    {
      get { return _bLogAttenuation; }
      set
      {
        if (_bLogAttenuation == value) return;
        _bLogAttenuation = value;
        OnSoundPropertyChanged(); // set everything because a sound instance is to be created
      }
    }

    /// <summary>
    /// If enabled, the sound loops, otherwise it is played once.
    /// </summary>
    [SortedCategory(CAT_SOUND, CATORDER_SOUND),
    PropertyOrder(6),
    Description("If enabled, the sound loops, otherwise it is played once.")]
    public bool Looped
    {
      get { return _bLooped; }
      set
      {
        if (_bLooped == value) return;
        _bLooped = value;
        OnSoundPropertyChanged(); // set everything because a sound instance is to be created
      }
    }
    
    /// <summary>
    /// If enabled, the sound is paused after creation and has to be activated via program code or trigger links
    /// </summary>
    [SortedCategory(CAT_SOUND, CATORDER_SOUND),
    PropertyOrder(7),
    Description("If enabled, the sound is paused after creation and has to be activated via program code or trigger links.")]
    public bool StartPaused
    {
      get { return _bStartPaused; }
      set
      {
        if (_bStartPaused == value) return;
        _bStartPaused = value;
        OnSoundPropertyChanged(); // set everything because a sound instance is to be created
      }
    }

    /// <summary>
    /// If enabled, the engine sound object is disposed automatically when finished playing. Otherwise it can be re-used via Play/ Pause within the life cycle of a scene
    /// </summary>
    [SortedCategory(CAT_SOUND, CATORDER_SOUND),
    PropertyOrder(8),
    Description("If enabled, the engine sound object is disposed automatically when finished playing. Otherwise it can be re-used via Play/ Pause within the life cycle of a scene")]
    public bool DisposeWhenFinished
    {
      get { return _bDisposeWhenFinished; }
      set
      {
        if (_bDisposeWhenFinished == value) return;
        _bDisposeWhenFinished = value;
        OnSoundPropertyChanged(); // set everything because a sound instance is to be created
      }
    }

    /// <summary>
    /// Defines the sound volume in the range of 0..1
    /// </summary>
    [SortedCategory(CAT_SOUND, CATORDER_SOUND),
    PropertyOrder(9),
    Description("Defines the sound volume in the range of 0..1")]
    [EditorAttribute(typeof(SliderEditor),typeof(UITypeEditor)), SliderRange(0.0f, 1.0f, 100)]
    public float Volume
    {
      get {return _fVolume;}
      set
      {
        if (_fVolume==value) return;
        _fVolume = value;
        if (_fVolume<0.0f) _fVolume=0.0f;
          else if (_fVolume>1.0f) _fVolume=1.0f;
        if (HasEngineInstance())
          EngineFmodSound.SetVolume(_fVolume);
      }
    }

    /// <summary>
    /// Defines the sound pitch in the range of 0..2
    /// </summary>
    [SortedCategory(CAT_SOUND, CATORDER_SOUND),
    PropertyOrder(10),
    Description("Defines the sound pitch in the range of 0..2")]
    [EditorAttribute(typeof(SliderEditor), typeof(UITypeEditor)), SliderRange(0.0f, 2.0f, 100)]
    [DisplayName("Pitch")]
    public float SoundPitch
    {
      get { return _fPitch; }
      set
      {
        if (_fPitch == value) return;
        _fPitch = value;
        if (_fPitch < 0.0f) _fPitch = 0.0f;
        else if (_fPitch > 2.0f) _fPitch = 2.0f;
        if (HasEngineInstance())
          EngineFmodSound.SetPitch(_fPitch);
      }
    }

    /// <summary>
    /// Defines the distance (in world units) at which the 3D sound has maximum volume.\nIn case of logarithmic attenuation, by default for every doubling of this MinDistance, the sound volume will halve.
    /// </summary>
    [SortedCategory(CAT_SOUND, CATORDER_SOUND),
    PropertyOrder(11),
    Description("Defines the distance (in world units) at which the 3D sound has maximum volume.\nIn case of logarithmic attenuation, by default for every doubling of this MinDistance, the sound volume will halve.")]
    public float MinDistance
    {
      get {return _fFadeMin;}
      set
      {
        if (_fFadeMin==value) return;
        _fFadeMin = value;
        if (_fFadeMin>_fFadeMax-1) _fFadeMin = _fFadeMax-1;
        if (_fFadeMin<0.0f) _fFadeMin = 0.0f;
        if (HasEngineInstance())
          EngineFmodSound.Set3DFadeDistance(_fFadeMin * UniformScaling, _fFadeMax * UniformScaling);
      }
    }

    /// <summary>
    /// Linear attenuation: Defines the distance (in world units) at which the 3D sound has completely faded out due to distance attenuation. Logarithmic attenuation: the distance (in world units) where the 3D sound stops attenuating.
    /// </summary>
    [SortedCategory(CAT_SOUND, CATORDER_SOUND),
    PropertyOrder(12),
    Description("Linear attenuation: Defines the distance (in world units) at which the 3D sound has completely faded out due to distance attenuation.\nLogarithmic attenuation: the distance (in world units) where the 3D sound stops attenuating.")]
    public float MaxDistance
    {
      get {return _fFadeMax;}
      set
      {
        if (_fFadeMax==value) return;
        _fFadeMax = value;
        if (_fFadeMax<1.0f) _fFadeMax = 1.0f;
        if (_fFadeMax<_fFadeMin+1) _fFadeMax = _fFadeMin+1;
        if (HasEngineInstance())
          EngineFmodSound.Set3DFadeDistance(_fFadeMin * UniformScaling, _fFadeMax * UniformScaling);
      }
    }

    /// <summary>
    /// Defines the sound left/ right speaker panning for 2D sounds (ignored for 3D sounds)
    /// </summary>
    [SortedCategory(CAT_SOUND, CATORDER_SOUND),
    PropertyOrder(13),
    Description("Defines the sound left/ right speaker panning for 2D sounds (ignored for 3D sounds)")]
    [EditorAttribute(typeof(SliderEditor),typeof(UITypeEditor)), SliderRange(-1.0f, 1.0f, 200)]
    public float Pan
    {
      get {return _fPan;}
      set
      {
        if (_fPan == value) return;
        _fPan = value;
        if (_fPan < -1.0f) _fPan = -1.0f;
        else if (_fPan > 1.0f) _fPan = 1.0f;
        if (HasEngineInstance())
          EngineFmodSound.SetPan(_fPan);
      }
    }

    /// <summary>
    /// Priority of the sound [0..255]. Only relevant if more channels are audible than supported by the sound engine.
    /// Important sounds should have a higher priority.
    /// </summary>
    [SortedCategory(CAT_SOUND, CATORDER_SOUND),
    PropertyOrder(14),
    Description("Priority of the sound [0..255]. Only relevant if more channels are audible than supported by the sound engine. Important sounds should have a higher priority.")]
    public int Priority
    {
      get {return _iPriority;}
      set
      {
        if (_iPriority==value) return;
        _iPriority = value;
        if (_iPriority<0) _iPriority=0;
        if (_iPriority>255) _iPriority = 255;
        OnSoundPropertyChanged(); // set everything because a sound instance is to be created
      }
    }

    /// <summary>
    /// Determines whether the sound is directional or omni sound. Directional sounds use the cone angle.
    /// </summary>
    [SortedCategory(CAT_SOUND, CATORDER_SOUND),
    PropertyOrder(15),
    Description("Determines whether the sound is directional or omni sound. Directional sounds use the cone angle.")]
    public bool Directional
    {
      get {return _bDirectional;}
      set
      {
        if (_bDirectional==value) return;
        _bDirectional = value;
        if (HasEngineInstance())
          EngineFmodSound.SetConeAngles(_bDirectional, _fConeInside, _fConeOutside);
        UpdateHotspotVisibility();

      }
    }

    /// <summary>
    /// Sets the inside cone angle in degrees for directional sounds. Ignored for omni-sounds.
    /// Inside this angle the volume is full.
    /// </summary>
    [SortedCategory(CAT_SOUND, CATORDER_SOUND),
    PropertyOrder(16),
    Description("Sets the inside cone angle in degrees for directional sounds. Ignored for omni-sounds. Inside this angle the volume is full.")]
    public float ConeInside
    {
      get {return _fConeInside;}
      set
      {
        if (_fConeInside==value) return;
        _fConeInside = value;
        if (_fConeInside<0.1f) _fConeInside = 0.1f;
        if (_fConeInside>_fConeOutside) _fConeInside = _fConeOutside;
        if (HasEngineInstance())
          EngineFmodSound.SetConeAngles(_bDirectional, _fConeInside, _fConeOutside);
      }
    }
    
    /// <summary>
    /// Sets the outside cone angle in degrees for directional sounds. Ignored for omni-sounds.
    /// Outside this angle the volume is zero.
    /// </summary>
    [SortedCategory(CAT_SOUND, CATORDER_SOUND),
    PropertyOrder(17),
    Description("Sets the outside cone angle in degrees for directional sounds. Ignored for omni-sounds. Outside this angle the volume is zero.")]
    public float ConeOutside
    {
      get {return _fConeOutside;}
      set
      {
        if (_fConeOutside==value) return;
        _fConeOutside = value;
        if (_fConeOutside<_fConeInside) _fConeOutside = _fConeInside;
        if (_fConeOutside>360.0f) _fConeOutside = 360.0f;
        if (HasEngineInstance())
          EngineFmodSound.SetConeAngles(_bDirectional, _fConeInside, _fConeOutside);
      }
    }

    /// <summary>
    /// Determines if the sound instance plays background music or not.
    /// </summary>
    [SortedCategory(CAT_SOUND, CATORDER_SOUND),
    PropertyOrder(18),
    Description("Determines if the sound instance plays background music or not.")]
    public bool BackgroundMusic
    {
      get { return _bMusic; }
      set
      {
        if (_bMusic == value) return;
        _bMusic = value;
        OnSoundPropertyChanged(); // set everything because a sound instance is to be created
      }
    }
    
    #endregion

    #region IPropertyValueChangingCallback Members

    public void OnPropertyValueChanging(object Instance, PropertyDescriptor propertyDesc, object newValue)
    {
      if (object.ReferenceEquals(Instance, this) && HasEngineInstance())
      {
        switch(propertyDesc.Name)
        {
          case "Volume":
            Volume = (float)newValue;
            break;

          case "SoundPitch":
            SoundPitch = (float)newValue;
            break;

          case "Pan":
            Pan = (float)newValue;
            break;
        }
      }
    }

    #endregion

    #region Selection and hot spots

    HotSpotDistance _hotSpotVolume; // a hotspot for sound volume
    HotSpotDistance _hotSpotFadeMin, _hotSpotFadeMax; // hotspots for attenuation
    HotSpotPlayButton _hotSpotPlaySound; // play the sound
    HotSpotConeAngle _hotSpotOuterCone, _hotSpotInnerCone; // hotspots for cone angles

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
        EngineFmodSound.Set3DFadeDistance(_fFadeMin * UniformScaling, _fFadeMax * UniformScaling);
      if (_hotSpotFadeMin != null)
        _hotSpotFadeMin.DisplayScaling = UniformScaling;
      if (_hotSpotFadeMax != null)
        _hotSpotFadeMax.DisplayScaling = UniformScaling;
    }

    void UpdatePlayIcon()
    {
      if (!HasEngineInstance() || _hotSpotPlaySound == null)
        return;
      _hotSpotPlaySound.ShowStopIcon = EngineFmodSound.IsPlaying();
    }

    /// <summary>
    ///  Play the sound in the editor (e.g. hotspot button pressed)
    /// </summary>
    public void TogglePlaySound()
    {
      if (!HasEngineInstance())
        return;
      if (EngineFmodSound.IsPlaying())
      {
        EngineFmodSound.Stop();
      }
      else
      {
        // Force the sound to play, even if we are not in play mode
        EngineFmodSound.Play(true);
      }
      UpdatePlayIcon();
    }

    void UpdateHotspotVisibility()
    {
      bool bEngineFmodSoundValid = HasEngineInstance() && EngineFmodSound.IsValid();

      if (_hotSpotVolume != null)
        _hotSpotVolume.Visible = bEngineFmodSoundValid;
      if (_hotSpotFadeMin != null)
        _hotSpotFadeMin.Visible = Is3D && bEngineFmodSoundValid;
      if (_hotSpotFadeMax != null)
        _hotSpotFadeMax.Visible = Is3D && bEngineFmodSoundValid;
      if (_hotSpotOuterCone != null)
        _hotSpotOuterCone.Visible = Is3D && Directional && bEngineFmodSoundValid;
      if (_hotSpotInnerCone != null)
        _hotSpotInnerCone.Visible = Is3D && Directional && bEngineFmodSoundValid;
      if (_hotSpotPlaySound != null)
        _hotSpotPlaySound.Visible = bEngineFmodSoundValid;
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    public override void OnSelected()
    {
      base.OnSelected ();

      System.Diagnostics.Debug.Assert(_hotSpotVolume==null);

      // hotspot for sound volume
      _hotSpotVolume = new HotSpotDistance(this, @"textures\Hotspot_V.tga", VisionColors.Cyan, HotSpotBase.PickType.Square, 10.0f);
      _hotSpotVolume.DisplayScaling = 100.0f; 
      _hotSpotVolume.StartDistance = Volume;
      _hotSpotVolume.Axis = this.ZAxis;
      _hotSpotVolume.ToolTipText = "Volume";
      _hotSpotVolume.SetRange(0.0f, 1.0f);
      EditorManager.ActiveView.HotSpots.Add(_hotSpotVolume);

      // hotspots for distance fading
      _hotSpotFadeMin = new HotSpotDistance(this, @"textures\Hotspot_D.tga", VisionColors.Blue, HotSpotBase.PickType.Square, 4.0f);
      _hotSpotFadeMin.StartDistance = MinDistance;
      _hotSpotFadeMin.Axis = this.XAxis;
      _hotSpotFadeMin.DisplayScaling = UniformScaling;
      _hotSpotFadeMin.ToolTipText = "min distance";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotFadeMin);
      _hotSpotFadeMax = new HotSpotDistance(this, @"textures\Hotspot_D.tga", VisionColors.Blue, HotSpotBase.PickType.Square, 4.0f);
      _hotSpotFadeMax.StartDistance = MaxDistance;
      _hotSpotFadeMax.Axis = this.XAxis;
      _hotSpotFadeMax.DisplayScaling = UniformScaling;
      _hotSpotFadeMax.ToolTipText = "max distance";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotFadeMax);

      // hotspots for cone angles
      _hotSpotOuterCone = new HotSpotConeAngle(this, @"textures\Hotspot_A.tga", VisionColors.Red, HotSpotBase.PickType.Square, 4.0f);
      _hotSpotOuterCone.Distance = 120.0f * EditorManager.Settings.GlobalUnitScaling;
      _hotSpotOuterCone.StartAngle = this.ConeOutside;
      _hotSpotOuterCone.ToolTipText = "Outer cone angle";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotOuterCone);
      _hotSpotInnerCone = new HotSpotConeAngle(this, @"textures\Hotspot_A.tga", VisionColors.Red, HotSpotBase.PickType.Square, 4.0f);
      _hotSpotInnerCone.Distance = 100.0f * EditorManager.Settings.GlobalUnitScaling;
      _hotSpotInnerCone.StartAngle = this.ConeInside;
      _hotSpotInnerCone.ToolTipText = "Inner cone angle";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotInnerCone);

      // hotspot for playing sound
      _hotSpotPlaySound = new HotSpotPlayButton(this, @"textures\Media_play.TGA", VisionColors.RGBA(200,200,200,255), VisionColors.White, 15.0f);
      _hotSpotPlaySound.ToolTipText = "Play sound";
      _hotSpotPlaySound.Set2DOffset(0.0f, 30.0f);
      EditorManager.ActiveView.HotSpots.Add(_hotSpotPlaySound);

      UpdateHotspotVisibility();
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
      SafeRemoveHotSpot(_hotSpotVolume); _hotSpotVolume = null;
      SafeRemoveHotSpot(_hotSpotFadeMin); _hotSpotFadeMin = null;
      SafeRemoveHotSpot(_hotSpotFadeMax); _hotSpotFadeMax = null;
      SafeRemoveHotSpot(_hotSpotPlaySound); _hotSpotPlaySound = null;
      SafeRemoveHotSpot(_hotSpotOuterCone); _hotSpotOuterCone = null;
      SafeRemoveHotSpot(_hotSpotInnerCone); _hotSpotInnerCone = null;

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
      if (hotSpot==_hotSpotVolume)
        Volume = _hotSpotVolume.CurrentDistance; 
      if (hotSpot==_hotSpotFadeMin)
        MinDistance = _hotSpotFadeMin.CurrentDistance; 
      if (hotSpot==_hotSpotFadeMax)
        MaxDistance = _hotSpotFadeMax.CurrentDistance; 
      if (hotSpot==_hotSpotOuterCone)
        ConeOutside = _hotSpotOuterCone.CurrentAngle; 
      if (hotSpot==_hotSpotInnerCone)
        ConeInside = _hotSpotInnerCone.CurrentAngle; 
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragBegin(CSharpFramework.View.HotSpotBase hotSpot, VisionViewBase view)
    {
      if (hotSpot==_hotSpotVolume)
        _hotSpotVolume.StartDistance = this.Volume;
      if (hotSpot==_hotSpotFadeMin)
        _hotSpotFadeMin.StartDistance = this.MinDistance;
      if (hotSpot==_hotSpotFadeMax)
        _hotSpotFadeMax.StartDistance = this.MaxDistance;
      if (hotSpot==_hotSpotOuterCone)
        _hotSpotOuterCone.StartAngle = ConeOutside;
      if (hotSpot==_hotSpotInnerCone)
        _hotSpotInnerCone.StartAngle = ConeInside;
      if (hotSpot==_hotSpotPlaySound) // button pressed
        TogglePlaySound();
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragEnd(CSharpFramework.View.HotSpotBase hotSpot, VisionViewBase view)
    {
      if (hotSpot==_hotSpotVolume)
      {
        if (_hotSpotVolume.HasChanged)
        {
          float fNewVal = _hotSpotVolume.CurrentDistance;
          Volume = _hotSpotVolume.StartDistance; // set old value for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this, "Volume", fNewVal)); 
        }
      }
      if (hotSpot==_hotSpotFadeMin)
      {
        if (_hotSpotFadeMin.HasChanged)
        {
          float fNewVal = _hotSpotFadeMin.CurrentDistance;
          MinDistance = _hotSpotFadeMin.StartDistance; // set old value for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this, "MinDistance", fNewVal)); 
        }
      }
      if (hotSpot==_hotSpotFadeMax)
      {
        if (_hotSpotFadeMax.HasChanged)
        {
          float fNewVal = _hotSpotFadeMax.CurrentDistance;
          MaxDistance = _hotSpotFadeMax.StartDistance; // set old value for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this, "MaxDistance", fNewVal)); 
        }
      }
      if (hotSpot==_hotSpotOuterCone)
      {
        if (_hotSpotOuterCone.HasChanged)
        {
          float fNewAngle = _hotSpotOuterCone.CurrentAngle;
          ConeOutside = _hotSpotOuterCone.StartAngle; // set old angle for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this, "ConeOutside", fNewAngle)); 
        }
      }
      if (hotSpot==_hotSpotInnerCone)
      {
        if (_hotSpotInnerCone.HasChanged)
        {
          float fNewAngle = _hotSpotInnerCone.CurrentAngle;
          ConeInside = _hotSpotInnerCone.StartAngle; // set old angle for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this, "ConeInside", fNewAngle)); 
        }
      }
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="hotSpot"></param>
    public override void OnHotSpotEvaluatePosition(CSharpFramework.View.HotSpotBase hotSpot)
    {
      if (hotSpot==_hotSpotVolume)
      {
        _hotSpotVolume.Axis = this.ZAxis;
        if (!_hotSpotVolume.IsDragging)
          _hotSpotVolume.StartDistance = Volume;
      }
      if (hotSpot==_hotSpotFadeMin)
      {
        _hotSpotFadeMin.Axis = this.XAxis;
        if (!_hotSpotFadeMin.IsDragging)
          _hotSpotFadeMin.StartDistance = MinDistance;
      }
      if (hotSpot==_hotSpotFadeMax)
      {
        _hotSpotFadeMax.Axis = this.XAxis;
        if (!_hotSpotFadeMax.IsDragging)
          _hotSpotFadeMax.StartDistance = MaxDistance;
      }
      if (hotSpot==_hotSpotOuterCone)
      {
        if (!_hotSpotOuterCone.IsDragging)
          _hotSpotOuterCone.StartAngle = ConeOutside;
      }
      if (hotSpot==_hotSpotInnerCone)
      {
        if (!_hotSpotInnerCone.IsDragging)
          _hotSpotInnerCone.StartAngle = ConeInside;
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

      if (_hotSpotVolume.Visible)
      {
        Vector3F vStart = Position;
        Vector3F vEnd = _hotSpotVolume.Position;
        view.RenderLine(vStart, vEnd, VisionColors.Cyan, 1.0f);
      }
    }

    #endregion

    #region Serialization and Export

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected FmodSoundShape(SerializationInfo info, StreamingContext context) : base(info, context)
    {
      _filename = info.GetString("_filename");
      _b3D = info.GetBoolean("_b3D");
      _bLooped = info.GetBoolean("_bLooped");
      _fVolume = info.GetSingle("_fVolume");
      _fPitch = info.GetSingle("_fPitch");
      _bStreaming = info.GetBoolean("_bStreaming");
      _fPan = info.GetSingle("_fPan");
      _fTimeOfs = info.GetSingle("_fTimeOfs");
      _fFadeMin = info.GetSingle("_fFadeMin");
      _fFadeMax = info.GetSingle("_fFadeMax");
      _iPriority = info.GetInt32("_iPriority");
      _bDirectional = info.GetBoolean("_bDirectional");
      _fConeOutside = info.GetSingle("_fConeOutside");
      _fConeInside = info.GetSingle("_fConeInside");
      _bStartPaused = info.GetBoolean("_bStartPaused");
      _bDisposeWhenFinished = info.GetBoolean("_bDisposeWhenFinished");
      _bMusic = info.GetBoolean("_bMusic");

      if (SerializationHelper.HasElement(info, "_bCompressedData"))
        _bCompressedData = info.GetBoolean("_bCompressedData");

      if (SerializationHelper.HasElement(info, "_bLogAttenuation"))
        _bLogAttenuation = info.GetBoolean("_bLogAttenuation");
    }
    
  
    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context)
    {
      base.GetObjectData (info, context); 

      info.AddValue("_filename",_filename);
      info.AddValue("_b3D",_b3D);
      info.AddValue("_bLooped",_bLooped);
      info.AddValue("_fVolume",_fVolume);
      info.AddValue("_fPitch", _fPitch);
      info.AddValue("_bStreaming",_bStreaming);
      info.AddValue("_fPan",_fPan);
      info.AddValue("_fTimeOfs",_fTimeOfs);
      info.AddValue("_fFadeMin",_fFadeMin);
      info.AddValue("_fFadeMax",_fFadeMax);
      info.AddValue("_iPriority",_iPriority);
      info.AddValue("_bDirectional",_bDirectional);
      info.AddValue("_fConeOutside",_fConeOutside);
      info.AddValue("_fConeInside",_fConeInside);
      info.AddValue("_bStartPaused", _bStartPaused);
      info.AddValue("_bCompressedData", _bCompressedData);
      info.AddValue("_bDisposeWhenFinished", _bDisposeWhenFinished);
      info.AddValue("_bMusic", _bMusic);
      info.AddValue("_bLogAttenuation", _bLogAttenuation);
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
  class FmodSoundShapeCreator : CSharpFramework.IShapeCreatorPlugin
  {

    /// <summary>
    /// Constructor
    /// </summary>
    public FmodSoundShapeCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\loudspeaker.bmp"), Color.Magenta);
      CategoryIconIndex = IconIndex;
    }

    /// <summary>
    /// Get the name of the plugin, for instance the shape name. This name appears in the "create" menu
    /// </summary>
    /// <returns>creator name</returns>
    public override string GetPluginName()
    {
      return "Sound";
    }

    /// <summary>
    /// Gets the plugin description
    /// </summary>
    /// <returns></returns>
    public override string GetPluginDescription()
    {
      return "This shape represents a sound instance in the world.";
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
      // create a shape
      FmodSoundShape Sound = new FmodSoundShape("Fmod - Sound");
      Sound.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
      return Sound;
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
