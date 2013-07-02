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
  #region RenderTarget shape
	/// <summary>
	/// DEPRECATED.
	/// </summary>
  [Serializable, ShapeTypeNiceName("Render Target")]
  [ShapeSearchableAttribute(false)]
  public class RenderTargetShape : Shape3D
	{
    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="name"></param>
		public RenderTargetShape(string name) : base(name)
		{
		}

    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_VIEW   = "View";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_VIEW = Shape3D.LAST_CATEGORY_ORDER_ID + 1;

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_TARGET   = "Render Target";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_TARGET = Shape3D.LAST_CATEGORY_ORDER_ID + 2;

    /// <summary>
    /// last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_TARGET;

    #endregion

    #region Engine Instance

    /// <summary>
    /// Returns the engine instance as a EngineInstanceRenderTarget
    /// </summary>
    [BrowsableAttribute(false)]
    public EngineInstanceRenderTarget EngineRenderTarget {get {return (EngineInstanceRenderTarget)_engineInstance;}}

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="bCreateChildren"></param>
    public override void CreateEngineInstance(bool bCreateChildren)
    {
      base.CreateEngineInstance(bCreateChildren);

      this._engineInstance = new EngineInstanceRenderTarget();
      SetEngineInstanceBaseProperties(); // sets the position etc.
    }


    /// <summary>
    /// Overridden function
    /// </summary>
    public override void SetEngineInstanceBaseProperties()
    {
      if (_engineInstance==null)
        return;
      EngineRenderTarget.CreateRenderTarget(_iResX,_iResY,_targetType,_targetName);
      base.SetEngineInstanceBaseProperties();
      EngineRenderTarget.SetFOV(CameraAngleX,CameraAngleY);
      EngineRenderTarget.SetClipPlanes(NearClipDistance,FarClipDistance);
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="view"></param>
    /// <param name="mode"></param>
    public override void RenderShape(VisionViewBase view, ShapeRenderMode mode)
    {
      base.RenderShape (view, mode);
      EngineRenderTarget.RenderShape(view,mode);
    }

    #endregion

    #region GUI

    /// <summary>
    /// Overridden function to get the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetIconIndex()
    {
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\Effect.bmp"), Color.Magenta);
    }

    #endregion

    #region Clone

    /// <summary>
    /// Overridden clone function
    /// </summary>
    /// <returns></returns>
    public override ShapeBase Clone()
    {
      RenderTargetShape clone =  (RenderTargetShape)base.Clone();
      clone._hotSpotConeAngleX = null;
      clone._hotSpotConeAngleY = null;
      clone._hotSpotNearClip = null;
      clone._hotSpotFarClip = null;
      return clone;
    }

    #endregion

    #region Serialization

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected RenderTargetShape(SerializationInfo info, StreamingContext context) : base(info, context)
    {
      _coneAngleX = info.GetSingle("_coneAngleX");
      _coneAngleY = info.GetSingle("_coneAngleY");
      _fNearClip = info.GetSingle("_fNearClip");
      _fFarClip = info.GetSingle("_fFarClip");
      _iResX = info.GetInt32("_iResX");
      _iResY = info.GetInt32("_iResY");
      _targetType = (RenderTargetType_e)info.GetValue("_targetType",typeof(RenderTargetType_e));
      _targetName = info.GetString("_targetName");
    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context) 
    {
      base.GetObjectData(info,context);
      info.AddValue("_coneAngleX",_coneAngleX);
      info.AddValue("_coneAngleY",_coneAngleY);
      info.AddValue("_fNearClip",_fNearClip);
      info.AddValue("_fFarClip",_fFarClip);
      info.AddValue("_iResX",_iResX);
      info.AddValue("_iResY",_iResY);
      info.AddValue("_targetType",_targetType);
      info.AddValue("_targetName",_targetName);
    }

    #endregion

    #region Members and Properties

    // view members:
    float _coneAngleX = 45.0f;
    float _coneAngleY = 45.0f;
    float _fNearClip = 5.0f;
    float _fFarClip = 10000.0f;
    // render target members
    int _iResX = 512;
    int _iResY = 512;
    RenderTargetType_e _targetType = RenderTargetType_e.ColorBuffer;
    string _targetName = "<rendertarget>";

    // properties
  

    /// <summary>
    /// The FOV in x-direction (degree)
    /// </summary>
    [SortedCategory(CAT_VIEW, CATORDER_VIEW), PropertyOrder(1),
    Description("The FOV in x-direction (degree)")]    
    public float CameraAngleX
    {
      get {return _coneAngleX;}
      set 
      {
        _coneAngleX=value;
        if (_coneAngleX<0.1f) _coneAngleX=0.1f;
          else if (_coneAngleX>179.0f) _coneAngleX=179.0f;

        if (_engineInstance!=null)
          EngineRenderTarget.SetFOV(CameraAngleX,CameraAngleY);
      }
    }

    /// <summary>
    /// The FOV in y-direction (degree)
    /// </summary>
    [SortedCategory(CAT_VIEW, CATORDER_VIEW), PropertyOrder(2),
    Description("The FOV in y-direction (degree)")]    
    public float CameraAngleY
    {
      get {return _coneAngleY;}
      set 
      {
        _coneAngleY=value;
        if (_coneAngleY<0.1f) _coneAngleY=0.1f;
          else if (_coneAngleY>179.0f) _coneAngleY=179.0f;

        if (_engineInstance!=null)
          EngineRenderTarget.SetFOV(CameraAngleX,CameraAngleY);
      }
    }

    /// <summary>
    /// The near clipping plane distance
    /// </summary>
    [SortedCategory(CAT_VIEW, CATORDER_VIEW), PropertyOrder(3),
    Description("The near clipping plane distance")]    
    public float NearClipDistance
    {
      get {return _fNearClip;}
      set 
      {
        _fNearClip=value;
        if (_fNearClip<0.001f) _fNearClip=0.001f;
          else if (_fNearClip>=_fFarClip-1.0f) _fNearClip=_fFarClip-1.0f;
        if (_engineInstance!=null)
          EngineRenderTarget.SetClipPlanes(NearClipDistance,FarClipDistance);
      }
    }

    /// <summary>
    /// The far clipping plane distance
    /// </summary>
    [SortedCategory(CAT_VIEW, CATORDER_VIEW), PropertyOrder(4),
    Description("The far clipping plane distance")]    
    public float FarClipDistance
    {
      get {return _fFarClip;}
      set 
      {
        _fFarClip=value;
        if (_fFarClip<_fNearClip+1.0f) _fFarClip=_fNearClip+1.0f;
        if (_engineInstance!=null)
          EngineRenderTarget.SetClipPlanes(NearClipDistance,FarClipDistance);
      }
    }


    /// <summary>
    /// Defines the x-resolution of the render target. Must be a power of two.
    /// </summary>
    [SortedCategory(CAT_TARGET, CATORDER_TARGET), PropertyOrder(1),
    TypeConverter(typeof(IntListTypeConverter)),AllowedIntValues(new int[] {32,64,128,256,512,1024,2048}),
    Description("Defines the x-resolution of the render target. Must be a power of two.")]    
    public int ResolutionX
    {
      get {return _iResX;}
      set 
      {
        _iResX=value;
        if (_engineInstance!=null)
          EngineRenderTarget.CreateRenderTarget(_iResX,_iResY,_targetType,_targetName);
      }
    }

    /// <summary>
    /// Defines the y-resolution of the render target. Must be a power of two.
    /// </summary>
    [SortedCategory(CAT_TARGET, CATORDER_TARGET), PropertyOrder(2),
    TypeConverter(typeof(IntListTypeConverter)),AllowedIntValues(new int[] {32,64,128,256,512,1024,2048}),
    Description("Defines the y-resolution of the render target. Must be a power of two.")]    
    public int ResolutionY
    {
      get {return _iResY;}
      set 
      {
        _iResY=value;
        if (_engineInstance!=null)
          EngineRenderTarget.CreateRenderTarget(_iResX,_iResY,_targetType,_targetName);
      }
    }

    /// <summary>
    /// Defines the render target buffer type (depends on actual usage)
    /// </summary>
    [SortedCategory(CAT_TARGET, CATORDER_TARGET), PropertyOrder(3),
    Description("Defines the render target buffer type (depends on actual usage)")]    
    public RenderTargetType_e TargetType
    {
      get {return _targetType;}
      set 
      {
        _targetType=value;
        if (_engineInstance!=null)
          EngineRenderTarget.CreateRenderTarget(_iResX,_iResY,_targetType,_targetName);
      }

    }

    /// <summary>
    /// Defines the target key to find the texture as a render target
    /// </summary>
    [SortedCategory(CAT_TARGET, CATORDER_TARGET), PropertyOrder(4),
    Description("Defines the target key to find the texture as a render target")]    
    public string TargetName
    {
      get {return _targetName;}
      set 
      {
        _targetName=value;
        if (_engineInstance!=null)
          EngineRenderTarget.CreateRenderTarget(_iResX,_iResY,_targetType,_targetName);
      }
    }

    #endregion

    #region Selection and Hotspots

    HotSpotConeAngle _hotSpotConeAngleX; // a hotspot for the cone angle in x direction
    HotSpotConeAngle _hotSpotConeAngleY; // a hotspot for the cone angle in y direction
    HotSpotDistance _hotSpotNearClip; // a hotspot for the cone fade-out-dist
    HotSpotDistance _hotSpotFarClip; // a hotspot for the cone fade-out-dist

    /// <summary>
    /// Overridden function
    /// </summary>
    public override void OnSelected()
    {
      base.OnSelected ();

      System.Diagnostics.Debug.Assert(_hotSpotConeAngleX==null);
      System.Diagnostics.Debug.Assert(_hotSpotConeAngleY==null);
      System.Diagnostics.Debug.Assert(_hotSpotNearClip==null);
      System.Diagnostics.Debug.Assert(_hotSpotFarClip==null);

      // create the hotspots:

      // hotspot for cone angle x
      _hotSpotConeAngleX = new HotSpotConeAngle(this,@"textures\Hotspot_A.tga",VisionColors.Red,HotSpotBase.PickType.Square,4.0f);
      _hotSpotConeAngleX.Distance = 50.0f * EditorManager.Settings.GlobalUnitScaling;
      _hotSpotConeAngleX.StartAngle = this.CameraAngleX;
      _hotSpotConeAngleX.ToolTipText = "FOV x";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotConeAngleX);

      // hotspot for cone angle y
      _hotSpotConeAngleY = new HotSpotConeAngle(this,@"textures\Hotspot_A.tga",VisionColors.Red,HotSpotBase.PickType.Square,4.0f);
      _hotSpotConeAngleY.Distance = 50.0f * EditorManager.Settings.GlobalUnitScaling;
      _hotSpotConeAngleY.UseOwnerUpDirection = true;
      _hotSpotConeAngleY.StartAngle = this.CameraAngleY;
      _hotSpotConeAngleY.ToolTipText = "FOV y";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotConeAngleY);

      // hotspot for fade out distance
      _hotSpotNearClip = new HotSpotDistance(this,@"textures\Hotspot_D.tga",VisionColors.Blue,HotSpotBase.PickType.Square,4.0f);
      _hotSpotNearClip.StartDistance = this.NearClipDistance;
      _hotSpotNearClip.Axis = this.XAxis;
      _hotSpotNearClip.ToolTipText = "Near clip distance";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotNearClip);

      // hotspot for fade out distance
      _hotSpotFarClip = new HotSpotDistance(this,@"textures\Hotspot_D.tga",VisionColors.Blue,HotSpotBase.PickType.Square,4.0f);
      _hotSpotFarClip.StartDistance = this.FarClipDistance;
      _hotSpotFarClip.Axis = this.XAxis;
      _hotSpotFarClip.ToolTipText = "Far clip distance";
      EditorManager.ActiveView.HotSpots.Add(_hotSpotFarClip);
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="hotSpot"></param>
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
      SafeRemoveHotSpot(_hotSpotNearClip); _hotSpotNearClip = null;
      SafeRemoveHotSpot(_hotSpotFarClip); _hotSpotFarClip = null;

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
        CameraAngleX = _hotSpotConeAngleX.CurrentAngle; // update for the view
      if (hotSpot==_hotSpotConeAngleY)
        CameraAngleY = _hotSpotConeAngleY.CurrentAngle; 
      if (hotSpot==_hotSpotNearClip)
        NearClipDistance = _hotSpotNearClip.CurrentDistance; 
      if (hotSpot==_hotSpotFarClip)
        FarClipDistance = _hotSpotFarClip.CurrentDistance; 
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragBegin(CSharpFramework.View.HotSpotBase hotSpot, VisionViewBase view)
    {
      if (hotSpot==_hotSpotConeAngleX)
        _hotSpotConeAngleX.StartAngle = this.CameraAngleX;
      if (hotSpot==_hotSpotConeAngleY)
        _hotSpotConeAngleY.StartAngle = this.CameraAngleY;
      if (hotSpot==_hotSpotNearClip)
        _hotSpotNearClip.StartDistance = this.NearClipDistance;
      if (hotSpot==_hotSpotFarClip)
        _hotSpotFarClip.StartDistance = this.FarClipDistance;
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
          CameraAngleX = _hotSpotConeAngleX.StartAngle; // set old angle for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"CameraAngleX",fNewAngle)); 
        }
      }
      if (hotSpot==_hotSpotConeAngleY)
      {
        if (_hotSpotConeAngleY.HasChanged)
        {
          float fNewAngle = _hotSpotConeAngleY.CurrentAngle;
          CameraAngleY = _hotSpotConeAngleY.StartAngle; // set old angle for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"CameraAngleY",fNewAngle));
        }
      }      
      if (hotSpot==_hotSpotNearClip)
      {
        if (_hotSpotNearClip.HasChanged)
        {
          float fNewDist = _hotSpotNearClip.CurrentDistance;
          this.NearClipDistance = _hotSpotNearClip.StartDistance; 
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"NearClipDistance",fNewDist)); 
        }
      }
      if (hotSpot==_hotSpotFarClip)
      {
        if (_hotSpotFarClip.HasChanged)
        {
          float fNewDist = _hotSpotFarClip.CurrentDistance;
          this.FarClipDistance = _hotSpotFarClip.CurrentDistance; 
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this,"FarClipDistance",fNewDist)); 
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
          _hotSpotConeAngleX.StartAngle = CameraAngleX;
      }
      if (hotSpot==_hotSpotConeAngleY)
      {
        if (!_hotSpotConeAngleY.IsDragging)
          _hotSpotConeAngleY.StartAngle = CameraAngleY;
      }
      if (hotSpot==_hotSpotNearClip)
      {
        _hotSpotNearClip.Axis = this.XAxis;
        if (!_hotSpotNearClip.IsDragging)
          _hotSpotNearClip.StartDistance = NearClipDistance;
      }
      if (hotSpot==_hotSpotFarClip)
      {
        _hotSpotFarClip.Axis = this.XAxis;
        if (!_hotSpotFarClip.IsDragging)
          _hotSpotFarClip.StartDistance = FarClipDistance;
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

  #region RenderTargetShape Creator Plugin

  class RenderTargetShapeCreator : CSharpFramework.IShapeCreatorPlugin
  {
    public RenderTargetShapeCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\Effect.bmp"), Color.Magenta);
    }

    public override string GetPluginName()
    {
      return "Render Target";
    }

    public override string GetPluginCategory()
    {
      return "Shaders";
    }

    public override ShapeBase CreateShapeInstance()
    {
      RenderTargetShape shape = new RenderTargetShape("RenderTarget");
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
