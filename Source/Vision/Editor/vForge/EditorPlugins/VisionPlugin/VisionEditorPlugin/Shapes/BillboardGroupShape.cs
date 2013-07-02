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
using CSharpFramework.Shapes;
using System.Runtime.Serialization;
using CSharpFramework.Serialization;
using CSharpFramework;
using System.IO;
using System.Drawing;
using VisionManaged;
using System.ComponentModel;
using CSharpFramework.PropertyEditors;
using System.Drawing.Design;
using CSharpFramework.Actions;
using System.Collections;
using VisionEditorPlugin.Dialogs;
using CSharpFramework.PickHandlers;
using CSharpFramework.Math;
using CSharpFramework.Scene;
using CSharpFramework.View;
using CSharpFramework.Contexts;


namespace VisionEditorPlugin.Shapes
{

  #region Actions

  #region class BillboardDropToFloorAction

  /// <summary>
  /// Custom action that drops every single billboard to floor
  /// </summary>
  [Serializable]
  public class BillboardDropToFloorAction : IAction
  {
    public BillboardDropToFloorAction(BillboardGroupShape shape, BillboardInstance[] instances, Shape3D.DropToFloorMode mode, Vector3F axis, ShapeCollection colliderShapes)
    {
      _instances = instances;
      if (_instances == null)
        _instances = shape.Instances;

      _shape = shape;
      _mode = mode;
      _oldHeights = new float[_instances.Length];
      _newHeights = new float[_instances.Length];

      for (int i = 0; i < _instances.Length; i++)
        _oldHeights[i] = _instances[i].Z;
      _shape.EngineMesh.GetDropToFloorHeights(_shape, _instances, _mode, axis, colliderShapes);
      for (int i = 0; i < _instances.Length; i++)
        _newHeights[i] = _instances[i].Z;
    }

    public override void Do()
    {
      for (int i = 0; i < _instances.Length; i++)
        _instances[i].Z = _newHeights[i];

      _shape.UpdateBillboardMesh();
    }

    public override void Undo()
    {
      for (int i = 0; i < _instances.Length; i++)
        _instances[i].Z = _oldHeights[i];

      _shape.UpdateBillboardMesh();
    }

    public override string ShortName
    {
      get { return "Drop billboards to floor"; }
    }

    #region ISerializable Members

    protected BillboardDropToFloorAction(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      _instances = (BillboardInstance[])info.GetValue("_instances", typeof(BillboardInstance[]));
      _shape = (BillboardGroupShape)info.GetValue("_shape", typeof(BillboardGroupShape));
      _mode = (Shape3D.DropToFloorMode)info.GetValue("_mode", typeof(Shape3D.DropToFloorMode));
      _oldHeights = (float[])info.GetValue("_oldHeights", typeof(float[]));
      _newHeights = (float[])info.GetValue("_newHeights", typeof(float[]));
    }

    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);
      info.AddValue("_instances", _instances);
      info.AddValue("_shape", _shape);
      info.AddValue("_mode", _mode);
      info.AddValue("_oldHeights", _oldHeights);
      info.AddValue("_newHeights", _newHeights);
    }

    #endregion

    BillboardInstance[] _instances;
    BillboardGroupShape _shape;
    Shape3D.DropToFloorMode _mode;
    float[] _oldHeights;
    float[] _newHeights;
  }

  #endregion

  #region Class AddBillboardAction

  [Serializable]
  public class AddBillboardAction : SetPropertyAction
  {
    public AddBillboardAction(BillboardGroupShape shape, BillboardInstance newInst) : base(shape,"Instances",null)
    {
      BillboardInstance[] _newArr = new BillboardInstance[shape.Instances.Length + 1];
      for (int i = 0; i < shape.Instances.Length; i++)
        _newArr[i] = shape.Instances[i];
      _newArr[_newArr.Length - 1] = newInst;
      this._newValue = _newArr;
    }

    public override string ShortName
    {
      get
      {
        return "Add Billboard";
      }
    }

    public override string LongName
    {
      get
      {
        return ShortName;
      }
    }

    #region ISerializable Members

    protected AddBillboardAction(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      // Anything to do here?
    }

    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);
      // Anything to do here?
    }

    #endregion
  }

  #endregion

  #region Class RemoveBillboardAction

  [Serializable]
  public class RemoveBillboardAction : SetPropertyAction
  {
    public RemoveBillboardAction(BillboardGroupShape shape, BillboardInstance[] sel)
      : base(shape, "Instances", null)
    {
      _shape = shape;
      _sel = sel;
      BillboardInstance[] _newArr = new BillboardInstance[shape.Instances.Length - sel.Length];
      int j = 0;
      for (int i = 0; i < shape.Instances.Length; i++)
        if (!SelectionContains(shape.Instances[i]))
          _newArr[j++] = shape.Instances[i];

      this._newValue = _newArr;
    }

    bool SelectionContains(BillboardInstance inst)
    {
      foreach (BillboardInstance b in _sel)
        if (object.ReferenceEquals(b, inst))
          return true;
      return false;
    }

    BillboardGroupShape _shape;
    BillboardInstance[] _sel;

    public override void Do()
    {
      _shape.ClearSelection();
      base.Do();
    }


    public override string ShortName
    {
      get
      {
        if (_sel.Length==1)
          return "Delete 1 Billboard";
        return "Delete " + _sel.Length.ToString() + " Billboards";
      }
    }

    public override string LongName
    {
      get
      {
        return ShortName;
      }
    }

    #region ISerializable Members

    protected RemoveBillboardAction(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      _shape = (BillboardGroupShape)info.GetValue("_shape", typeof(BillboardGroupShape));
      _sel = (BillboardInstance[])info.GetValue("_sel", typeof(BillboardInstance[]));
    }

    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);
      info.AddValue("_shape", _shape);
      info.AddValue("_sel", _sel);
    }

    #endregion
  }

  #endregion

  #region Class MoveBillboardAction

  [Serializable]
  public class MoveBillboardAction : IAction
  {
    public MoveBillboardAction(BillboardGroupShape shape, BillboardInstance inst, Vector3F oldPos, Vector3F newPos)
    {
      _oldPos = oldPos;
      _newPos = newPos;
      _inst = inst;
      _shape = shape;
    }

    BillboardGroupShape _shape;
    BillboardInstance _inst;
    Vector3F _oldPos, _newPos;

    public override bool Valid
    {
      get
      {
        if (!_shape.Modifiable)
          return false;
        return base.Valid;
      }
    }

    public override void Do()
    {
      // CLI Conversion. 
      // Changed propertiy name. During the conversion I found that there are some C# classes that makes a direct reference to the variable that holds the value of the property
      // and sometimes they use the property name.
      /*
      _inst.X = _newPos.X;
      _inst.Y = _newPos.Y;
      _inst.Z = _newPos.Z;
      */
      _inst.X = _newPos.X;
      _inst.Y = _newPos.Y;
      _inst.Z = _newPos.Z;
      _shape.UpdateBillboardMesh();
      _shape.OnSelectionChanged(); // rebuild hotspots
    }

    public override void Undo()
    {
      // CLI Conversion. 
      // Changed propertiy name. During the conversion I found that there are some C# classes that makes a direct reference to the variable that holds the value of the property
      // and sometimes they use the property name.
      /*
      _inst.X = _oldPos.X;
      _inst.Y = _oldPos.Y;
      _inst.Z = _oldPos.Z;
      */
      _inst.X = _oldPos.X;
      _inst.Y = _oldPos.Y;
      _inst.Z = _oldPos.Z;
      _shape.UpdateBillboardMesh();
      _shape.OnSelectionChanged(); // rebuild hotspots
    }


    public override string ShortName
    {
      get
      {
        return "Move Billboard";
      }
    }

    #region ISerializable Members

    protected MoveBillboardAction(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      _shape = (BillboardGroupShape)info.GetValue("_shape", typeof(BillboardGroupShape));
      _inst = (BillboardInstance)info.GetValue("_inst", typeof(BillboardInstance));
      _oldPos = (Vector3F)info.GetValue("_oldPos", typeof(Vector3F));
      _newPos = (Vector3F)info.GetValue("_newPos", typeof(Vector3F));
    }

    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);
      info.AddValue("_shape", _shape);
      info.AddValue("_inst", _inst);
      info.AddValue("_oldPos", _oldPos);
      info.AddValue("_newPos", _newPos);
    }

    #endregion
  }

  #endregion

  #region Class ScaleBillboardAction

  [Serializable]
  public class ScaleBillboardAction : IAction
  {
    public ScaleBillboardAction(BillboardGroupShape shape, BillboardInstance inst, float oldSX, float oldSY, float newSX, float newSY)
    {
      _inst = inst;
      _shape = shape;
      _oldSX = oldSX;
      _oldSY = oldSY;
      _newSX = newSX;
      _newSY = newSY;
    }

    BillboardGroupShape _shape;
    BillboardInstance _inst;
    float _oldSX,_oldSY,_newSX,_newSY;

    public override bool Valid
    {
      get
      {
        if (!_shape.Modifiable)
          return false;
        return base.Valid;
      }
    }

    public override void Do()
    {
      _inst.ScaleX = _newSX;
      _inst.ScaleY = _newSY;
      _shape.UpdateBillboardMesh();
      _shape.OnSelectionChanged(); // rebuild hotspots
    }

    public override void Undo()
    {
      _inst.ScaleX = _oldSX;
      _inst.ScaleY = _oldSY;
      _shape.UpdateBillboardMesh();
      _shape.OnSelectionChanged(); // rebuild hotspots
    }


    public override string ShortName
    {
      get
      {
        return "Scale Billboard";
      }
    }

    #region ISerializable Members

    protected ScaleBillboardAction(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      _shape = (BillboardGroupShape)info.GetValue("_shape", typeof(BillboardGroupShape));
      _inst = (BillboardInstance)info.GetValue("_inst", typeof(BillboardInstance));
      _oldSX = info.GetSingle("_oldSX");
      _oldSY = info.GetSingle("_oldSY");
      _newSX = info.GetSingle("_newSX");
      _newSY = info.GetSingle("_newSY");
    }

    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);
      info.AddValue("_shape", _shape);
      info.AddValue("_inst", _inst);
      info.AddValue("_oldSX", _oldSX);
      info.AddValue("_oldSY", _oldSY);
      info.AddValue("_newSX", _newSX);
      info.AddValue("_newSY", _newSY);
    }

    #endregion
  }

  #endregion

  #region Class ModifyBillboardsAction

  /// <summary>
  /// Generic action for modifying a subset of instances. Create an instance of this action, then access action.NewInstances
  /// and modify the elements in this array. Then fire this action.
  /// </summary>
  [Serializable]
  public class ModifyBillboardsAction : IAction
  {
    public ModifyBillboardsAction(BillboardGroupShape shape, BillboardInstance[] sel, string name)
    {
      if (sel==null)
        sel = shape.Instances;

      _name = name;
      _shape = shape;

      oldInst = new BillboardInstance[sel.Length];
      NewInstances = new BillboardInstance[sel.Length];
      _remapping = new int[sel.Length];

      // build a mapping table of indices where to replace the list of all instances
      for (int i = 0; i < sel.Length; i++)
      {
        _remapping[i] = IndexInInstances(sel[i]);
        oldInst[i] = sel[i];
        NewInstances[i] = (BillboardInstance)oldInst[i].Clone();
      }

    }

    int IndexInInstances(BillboardInstance inst)
    {
      for (int i = 0; i < _shape.Instances.Length; i++)
        if (_shape.Instances[i] == inst)
          return i;
      return -1;
    }

    string _name;
    protected BillboardGroupShape _shape;
    protected BillboardInstance[] oldInst;
    public BillboardInstance[] NewInstances; // this array can be accessed from outside
    int[] _remapping;

    void ReplaceInstances(BillboardInstance[] list)
    {
      bool bNeedsSelUpdate = false;
      for (int i = 0; i < list.Length; i++)
        if (_remapping[i] >= 0)
        {
          // make sure it is also replaced in the list of selected instances
          BillboardInstance old = _shape.Instances[_remapping[i]];
          if (_shape._selectedBillboards != null && _shape._selectedBillboards.Contains(old))
          {
            int iPos = _shape._selectedBillboards.IndexOf(old);
            _shape._selectedBillboards[iPos] = list[i];
            bNeedsSelUpdate = true;
          }

          // replace instance
          _shape.Instances[_remapping[i]] = list[i];
        }

      if (bNeedsSelUpdate)
        _shape.OnSelectionChanged();
    }

    public override void Do()
    {
      ReplaceInstances(NewInstances);
      _shape.UpdateBillboardMesh();
    }

    public override void Undo()
    {
      ReplaceInstances(oldInst);
      _shape.UpdateBillboardMesh();
    }

    public override bool Valid
    {
      get
      {
        if (!_shape.Modifiable)
          return false;
        return base.Valid;
      }
    }

    public override string ShortName
    {
      get 
      {
        if (string.IsNullOrEmpty(_name))
          return "Modify Billboards";
        return _name;
      }
    }

    #region ISerializable Members

    protected ModifyBillboardsAction(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      _shape = (BillboardGroupShape)info.GetValue("_shape", typeof(BillboardGroupShape));
      _name = info.GetString("_name");
      NewInstances = (BillboardInstance[])info.GetValue("NewInstances", typeof(BillboardInstance[]));
      oldInst = (BillboardInstance[])info.GetValue("oldInst", typeof(BillboardInstance[]));
      _remapping = (int[])info.GetValue("_remapping", typeof(int[]));
    }

    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);
      info.AddValue("_name", _name);
      info.AddValue("_shape", _shape);
      info.AddValue("oldInst", oldInst);
      info.AddValue("NewInstances", NewInstances);
      info.AddValue("_remapping", _remapping);
    }

    #endregion

  }

  #endregion

  #region Class RecenterBillboardsAction

  [Serializable]
  public class RecenterBillboardsAction : ModifyBillboardsAction
  {
    public RecenterBillboardsAction(BillboardGroupShape shape) : base(shape,null,"Re-center billboard group")
    {
      BoundingBox bbox = shape.AbsoluteBoundingBox;
      Vector3F center = bbox.Center;
      _oldPos = shape.Position;
      _newPos = center;
      Vector3F delta = _oldPos - _newPos;
      foreach (BillboardInstance b in NewInstances)
      {
        b.X += delta.X;
        b.Y += delta.Y;
        b.Z += delta.Z;
      }
    }

    Vector3F _oldPos, _newPos;

    public override void Do()
    {
      _shape.Position = _newPos;
      base.Do();
      IScene.SendPropertyChangedEvent(new PropertyChangedArgs(_shape, "Position")); // Gizmo update
    }

    public override void Undo()
    {
      _shape.Position = _oldPos;
      base.Undo();
      IScene.SendPropertyChangedEvent(new PropertyChangedArgs(_shape, "Position")); // Gizmo update
    }

    public override string ShortName
    {
      get { return "Recenter Group"; }
    }

    #region ISerializable Members

    protected RecenterBillboardsAction(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      _newPos = (Vector3F)info.GetValue("_newPos", typeof(Vector3F));
      _oldPos = (Vector3F)info.GetValue("_oldPos", typeof(Vector3F));
    }

    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);
      info.AddValue("_newPos", _newPos);
      info.AddValue("_oldPos", _oldPos);
    }

    #endregion

  }

  #endregion

  #endregion

  #region Contexts

  public class BillboardBaseContext : CameraMoveContext
  {
    public BillboardBaseContext()
    {
      this.AllowShapeSelection = false;
      this.AllowRecenterMouse = false;
    }

    public override bool OnMouseClick()
    {
      bool bUpdate = base.OnMouseClick();

      if (_canPick && _keyMod != KeyModifier.None)
      {
        BillboardInstance inst = BillboardGroupShape.PickBillboard(MouseX, MouseY);
        if (inst != null)
        {
          BillboardGroupShape.CurrentBillboard.AddToSelection(inst, true);
          bUpdate = true;
        }
      }

      return bUpdate;
    }

    public override void OnKeyModifierChanged(KeyModifier eOldMod, KeyModifier eNewMod)
    {
      base.OnKeyModifierChanged(eOldMod, eNewMod);
      _keyMod = eNewMod;
    }

    protected KeyModifier _keyMod = KeyModifier.None;
    protected bool _canPick = false;
  }


  public class BillboardSelectContext : BillboardBaseContext
  {
    public BillboardSelectContext()
    {
      _canPick = true;
    }
  }

  public class BillboardAddContext : BillboardBaseContext
  {
    public override bool OnMouseClick()
    {
      bool bUpdate = base.OnMouseClick();

      if (_keyMod != KeyModifier.None)
      {
        BillboardGroupShape shape = BillboardGroupShape.CurrentBillboard;
        if (shape == null)
          return bUpdate;

        Vector3F startRay, endRay;
        Vector3F hitPoint = new Vector3F();
        EditorManager.EngineManager.GetRayAtScreenPos(out startRay, out endRay, MouseX, MouseY, EditorManager.Settings.MaxPickingDistance);
        if (!EditorManager.EngineManager.GetTraceHit(startRay, endRay, ref hitPoint))
          return bUpdate;

        // bring into local space:
        hitPoint -= shape.Position;
        Matrix3F rot = shape.RotationMatrix;
        rot.Transpose();
        hitPoint = rot * hitPoint;

        BillboardInstance _newInst = new BillboardInstance();
        _newInst.X = hitPoint.X;
        _newInst.Y = hitPoint.Y;
        _newInst.Z = hitPoint.Z;

        // add variation from context:
        float fScale = UniformScaling * (1.0f + (float)rnd.NextDouble()*ScaleVariation*2.0f-ScaleVariation);
        _newInst.UniformScale = fScale;

        // sub-image: Any image from the atlas
        _newInst._fAtlasValue = (float)rnd.NextDouble();

        EditorManager.Actions.Add(new AddBillboardAction(shape, _newInst));
        bUpdate = true;
      }
      return bUpdate;
    }

    Random rnd = new Random();
    public float ScaleVariation = 0.1f;
    public float UniformScaling = 1.0f;
  }


  public class BillboardRemoveContext : BillboardBaseContext
  {
    public override bool OnMouseClick()
    {
      bool bUpdate = base.OnMouseClick();

      if (_keyMod != KeyModifier.None)
      {
        BillboardInstance inst = BillboardGroupShape.PickBillboard(MouseX, MouseY);
        if (inst == null)
          return bUpdate;

        BillboardInstance[] list = new BillboardInstance[1];
        list[0] = inst;
        EditorManager.Actions.Add(new RemoveBillboardAction(BillboardGroupShape.CurrentBillboard, list));
        bUpdate = true;
      }
      return bUpdate;
    }
  }


  #endregion

  #region Hotspot classes

  public class HotSpotBillboardMove : HotSpot3D
  {
    public HotSpotBillboardMove(BillboardGroupShape shape, BillboardInstance inst)
      : base(shape, shape.BaseSizeX * 0.25f)
    {
      _inst = inst;
      this.CurrentPosition = new Vector3F(_inst.X, _inst.Y, _inst.Z);
    }

    BillboardGroupShape Billboard { get { return (BillboardGroupShape)this.Owner; } }


    public override void EvaluatePosition()
    {
      base.EvaluatePosition();
    }

    public override void OnDragEnd(VisionViewBase view)
    {
      base.OnDragEnd(view);
      Vector3F oldPos = new Vector3F(_inst.X, _inst.Y, _inst.Z);
      EditorManager.Actions.Add(new MoveBillboardAction(Billboard, _inst, oldPos, CurrentPosition));
    }

    BillboardInstance _inst;
  }

  public class HotSpotBillboardSize : HotSpotDistance
  {
    public HotSpotBillboardSize(BillboardGroupShape shape, BillboardInstance inst, bool bHoriz)
      : base(shape, @"textures\Hotspot_R.dds", VisionColors.Yellow, PickType.Square, 4.0f)
    {
      _inst = inst;
      _bHoriz = bHoriz;
      TextureAtlas.AtlasEntry atlas = shape.Atlas.GetRandomEntry(inst._fAtlasValue);

      if (_bHoriz)
      {
        StartDistance = inst.ScaleX; // so we can use it directly for scaling
        DisplayScaling = shape.BaseSizeX * 0.5f * atlas.ResultingScaleX;
      }
      else
      {
        StartDistance = inst.ScaleY; // so we can use it directly for scaling
        DisplayScaling = shape.BaseSizeY * 1.0f * atlas.ResultingScaleY;
      }
      this.CurrentPosition = new Vector3F(_inst.X, _inst.Y, _inst.Z);
    }

    BillboardGroupShape Billboard { get { return (BillboardGroupShape)this.Owner; } }

    public override void EvaluatePosition()
    {
      // match spanning in the vertex shader:
      if (_bHoriz)
      {
        Vector3F camPos = EditorManager.ActiveView.CameraPosition;
        Vector3F pivot = new Vector3F(_inst._wsX, _inst._wsY, _inst._wsZ); // assume world space is cached
        Vector3F diff = pivot - camPos;
        Vector3F camRight = Vector3F.CrossProduct(diff,Vector3F.ZAxis);
        if (camRight.GetLength() > 0.0f)
        {
          camRight.Normalize();
          this.Axis = camRight;
        }
      }
      else
        this.Axis = Vector3F.ZAxis; // always up

      base.EvaluatePosition();
    }

    public override void OnDragEnd(VisionViewBase view)
    {
      base.OnDragEnd(view);
      float fNewSX = _inst.ScaleX;
      float fNewSY = _inst.ScaleY;
      float fDist = System.Math.Max(CurrentDistance,0.01f);

      if (_bHoriz)
        fNewSX = fDist;
      else
        fNewSY = fDist;
      EditorManager.Actions.Add(new ScaleBillboardAction(Billboard, _inst, _inst.ScaleX, _inst.ScaleY, fNewSX, fNewSY));
     
    }

    bool _bHoriz;
    BillboardInstance _inst;
  }
  #endregion

  #region Class BillboardGroupShape

  [Serializable,ShapeTypeNiceName("Billboard Group")]
  public class BillboardGroupShape : Shape3D
  {
    public enum BillboardPivotCenter_e
    {
      Custom = -1, 
      UpperLeftCorner,
      UpperRightCorner,
      BottomLeftCorner,
      BottomRightCorner,
      Centered,
      TopCenter,
      BottomCenter,
      RightCenter,
      LeftCenter
    }

    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_BILLBOARDS = "Billboards";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_BILLBOARDS = Shape3D.LAST_CATEGORY_ORDER_ID + 1;

    /// <summary>
    /// last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_BILLBOARDS;

    #endregion

    #region Constructor

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="name"></param>
    public BillboardGroupShape(string name)
      : base(name)
		{
      float fScale = EditorManager.Settings.GlobalUnitScaling;
      _fBaseSizeX *= fScale;
      _fBaseSizeY *= fScale;

		}
  
    #endregion

    #region Shape3D Overrides

    public void UpdateBillboardMesh()
    {
      if (!HasEngineInstance())
        return;
      EngineMesh.SetInstances(this._instances, this.Atlas);
      EditorManager.ActiveView.UpdateView(false);
    }

    public override void CreateEngineInstance(bool bCreateChildren)
    {
      base.CreateEngineInstance(bCreateChildren);
      if (_engineInstance == null)
        _engineInstance = new EngineInstanceBillboardGroup();
      SetEngineInstanceBaseProperties(); // sets the position etc.
    }

    public override IAction GetCustomDropToFloorAction(Shape3D.DropToFloorMode mode, Vector3F axis, ShapeCollection colliderShapes)
    {
      if (!HasEngineInstance() || Instances.Length < 1)
        return base.GetCustomDropToFloorAction(mode, axis, colliderShapes);
      return new BillboardDropToFloorAction(this, null, mode, axis, colliderShapes); // all instances
    }

    /// <summary>
    /// Helper to access engine instance as EngineInstanceBillboardGroup
    /// </summary>
    [Browsable(false)]
    public EngineInstanceBillboardGroup EngineMesh { get { return (EngineInstanceBillboardGroup)_engineInstance; } }

    public override void SetEngineInstanceBaseProperties()
    {
      base.SetEngineInstanceBaseProperties();
      if (_engineInstance == null)
        return;

      EngineMesh.SetBaseSize(_fBaseSizeX, _fBaseSizeY);
      EngineMesh.SetRelCenter(_fRelCenterX, _fRelCenterY);
      EngineMesh.SetTexture(_textureFile);
      EngineMesh.SetBlendMode(_blendMode, _bUseSceneBrightness);
      EngineMesh.SetColor(VisionColors.Get(_color), _bUseSceneBrightness);
      EngineMesh.SetClipDistances(_fNearClip, _fFarClip);
      EngineMesh.SetCastStaticShadows(_bCastStaticShadows);
      EngineMesh.SetObjectKey(_objectKey);

      UpdateBillboardMesh();
    }

    public override void OnAddedToScene()
    {
      base.OnAddedToScene();
      this.AddHint(HintFlags_e.NoRotation);

      // process raw data from prefab
      if (_rawData != null)
      {
        ArrayList inst = new ArrayList(1000);
        EngineInstanceBillboardGroup.InstancesFromRawData(inst, _rawDataVersion, _rawData);
        this._instances = (BillboardInstance[])inst.ToArray(typeof(BillboardInstance));
        _rawData = null;
      }

      TextureAtlas.OnAtlasFileSaved += new EventHandler(TextureAtlas_OnAtlasFileSaved);
    }

    void TextureAtlas_OnAtlasFileSaved(object sender, EventArgs e)
    {
      TextureAtlas atlas = (TextureAtlas)sender;
      if (atlas.Filename == this.TextureAtlasFile)
        this.TextureAtlasFile = this._atlasFilename;
    }

    public override void OnSceneEvent(SceneEventArgs e)
    {
      base.OnSceneEvent(e);

      if (e.action == SceneEventArgs.Action.AfterStaticLighting || e.action == SceneEventArgs.Action.StaticLightingReset || e.action == SceneEventArgs.Action.StaticLightingLoaded)
      {
        if (this.UseSceneBrightness)
          this.UpdateBillboardMesh();
      }
      else if (e.action == SceneEventArgs.Action.AfterRendererNodeChanged)
      {
        if (HasEngineInstance())
          EngineMesh.ReassignShader();
      }
      if (e.action == SceneEventArgs.Action.BeforeClosing)
      {
        if (_contextDlg != null) // create a new dialog next scene
        {
          _contextDlg.Close();
          _contextDlg = null;
        }
      }

    }

    public override void OnRemoveFromScene()
    {
      TextureAtlas.OnAtlasFileSaved -= new EventHandler(TextureAtlas_OnAtlasFileSaved);
      base.OnRemoveFromScene();
      if (CurrentBillboard == this)
        CurrentBillboard = null;
    }

    bool _bDragging = false;

    public override void OnDragBegin(ShapeDragMode mode)
    {
      base.OnDragBegin(mode);
      _bDragging = true;
    }

    public override void OnDragEnd(ShapeDragMode mode)
    {
      _bDragging = false;
      base.OnDragEnd(mode);
    }


    public override void OnTransformationChanged()
    {
      base.OnTransformationChanged();
      if (!_bDragging && this.UseSceneBrightness)
        UpdateBillboardMesh();
    }

    #endregion

    #region Relevant Operations

    static Random _rnd = null;
    static Random Rnd
    {
      get
      {
        if (_rnd == null)
          _rnd = new Random();
        return _rnd;
      }
    }


    public void RelaxPositions(BillboardInstance[] sel)
    {
      ModifyBillboardsAction action = new ModifyBillboardsAction(this,sel,"Relax billboard positions");
      float m = Math.Max(this.PivotCenterX, 1.0f - PivotCenterX);
      m *= 2.0f; // wanted distance
      EngineMesh.RelaxPositions(action.NewInstances, m, 20);
      EditorManager.Actions.Add(action);
    }


    public void ApplyRandomScale(BillboardInstance[] sel, float fPercentage)
    {
      string name = string.Format("Apply billboard scaling +/-{0}%", fPercentage);
      ModifyBillboardsAction action = new ModifyBillboardsAction(this, sel, name);
      BillboardInstance[] scaled = action.NewInstances;
      
      foreach (BillboardInstance b in scaled)
      {
        float f = 1.0f + (float)(Rnd.NextDouble()-0.5) * 0.02f * fPercentage;
        b.ScaleX *= f;
        b.ScaleY *= f;
      }
      EditorManager.Actions.Add(action);
    }

    public void ApplyRandomAtlasFrame(BillboardInstance[] sel)
    {
      string name = string.Format("Apply random atlas frame");
      ModifyBillboardsAction action = new ModifyBillboardsAction(this, sel, name);
      BillboardInstance[] inst = action.NewInstances;

      foreach (BillboardInstance b in inst)
        b._fAtlasValue = (float)Rnd.NextDouble();
      EditorManager.Actions.Add(action);
    }

    public void RemoveOverlappingInstances(BillboardInstance[] sel)
    {
      if (sel == null)
        sel = this._instances;

      // build a list of overlapping:
      ArrayList removed = new ArrayList(100);
      float m = Math.Max(this.PivotCenterX, 1.0f - PivotCenterX);
      foreach (BillboardInstance a in sel)
        foreach (BillboardInstance b in sel)
          if (!removed.Contains(a) && !removed.Contains(b) && !object.ReferenceEquals(a, b))
          {
            Vector3F dist = new Vector3F(a.X - b.X, a.Y - b.Y, a.Z - b.Z);
            float rA = m * a._fFinalScaleX;
            float rB = m * b._fFinalScaleY;

            if (dist.GetLength() > (rA + rB))
              continue;
            removed.Add(b);
          }

      if (removed.Count < 1)
        return;

      EditorManager.Actions.Add(new RemoveBillboardAction(this, (BillboardInstance[])removed.ToArray(typeof(BillboardInstance))));
    }

    public static string RO_RANDOMFRAME = "New random atlas image";
    public static string RO_RECENTER = "Re-center group pivot";
    public static string RO_RELAX = "Relax positions";
    public static string RO_REMOVEOVERLAPPING = "Remove overlapping instances";

    public override System.Collections.ArrayList RelevantOperations
    {
      get
      {
        ArrayList arr = base.RelevantOperations;
        if (arr == null)
          arr = new ArrayList(4);
        arr.Add(RO_RECENTER);
        arr.Add(RO_RELAX);
        arr.Add(RO_REMOVEOVERLAPPING);
        if (this.Atlas.Entries.Count>1)
          arr.Add(RO_RANDOMFRAME);
        return arr;
      }
    }

    public override void PerformRelevantOperation(string name, int iShapeIndex, int iShapeCount)
    {
      if (name == RO_RECENTER)
      {
        EditorManager.Actions.Add(new RecenterBillboardsAction(this));
        return;
      }
      if (name == RO_RELAX)
      {
        RelaxPositions(null);
        return;
      }
      if (name == RO_REMOVEOVERLAPPING)
      {
        RemoveOverlappingInstances(null);
        return;
      }
      if (name == RO_RANDOMFRAME)
      {
        ApplyRandomAtlasFrame(null);
        return;
      }

      // also override some default actions from Shape3D:
      if (name == OP_RANDOM_SCALE10)
      {
        ApplyRandomScale(null,10.0f);
        return;
      }
      if (name == OP_RANDOM_SCALE30)
      {
        ApplyRandomScale(null, 30.0f);
        return;
      }

      base.PerformRelevantOperation(name, iShapeIndex, iShapeCount);
      
    }


    #endregion

    #region Tracing and Selection


    public ArrayList _selectedBillboards = null;
    public ArrayList _customHotSpots = null;

    [Browsable(false)]
    public BillboardInstance[] CurrentSelection
    {
      get
      {
        if (_selectedBillboards == null || _selectedBillboards.Count < 1)
          return null;
        return (BillboardInstance[])_selectedBillboards.ToArray(typeof(BillboardInstance));
      }
    }

    public void OnSelectionChanged()
    {
      if (_customHotSpots != null)
      {
        foreach (HotSpotBase hs in _customHotSpots)
        {
          EditorManager.ActiveView.HotSpots.Remove(hs);
          hs.Remove();
        }
        _customHotSpots.Clear();
      }

      if (_selectedBillboards == null || _selectedBillboards.Count < 1)
      {
        EditorManager.ActiveView.UpdateView(false);
        return;
      }

      if (_customHotSpots == null)
        _customHotSpots = new ArrayList(_selectedBillboards.Count * 3);

      foreach (BillboardInstance inst in _selectedBillboards)
      {
        _customHotSpots.Add(new HotSpotBillboardMove(this, inst));
        _customHotSpots.Add(new HotSpotBillboardSize(this, inst, true));
        _customHotSpots.Add(new HotSpotBillboardSize(this, inst, false));
      }
      foreach (HotSpotBase hs in _customHotSpots)
        EditorManager.ActiveView.HotSpots.Add(hs);

      EditorManager.ActiveView.UpdateView(false);
    }

    public void AddToSelection(BillboardInstance inst, bool bXOR)
    {
      if (_selectedBillboards == null)
        _selectedBillboards = new ArrayList();
      if (_selectedBillboards.Contains(inst))
      {
        if (bXOR)
          _selectedBillboards.Remove(inst);
      }
      else
        _selectedBillboards.Add(inst);

      OnSelectionChanged();
    }

    public void RemoveFromSelection(BillboardInstance inst)
    {
      if (_selectedBillboards == null || !_selectedBillboards.Contains(inst))
        return;
      _selectedBillboards.Remove(inst);
      OnSelectionChanged();
    }

    public void ClearSelection()
    {
      _selectedBillboards = null;
      OnSelectionChanged();
    }

    public BillboardInstance TraceBillboard(Vector3F startRay, Vector3F endRay)
    {
      if (!HasEngineInstance())
        return null;
      return EngineMesh.GetBestTraceHit(this,_instances,startRay,endRay);
    }

    #endregion

    #region Members and Properties

    string _textureFile = @"\plainwhite.dds";
    TransparencyType _blendMode = TransparencyType.AlphaTest;
    Color _color = Color.White;
    bool _bUseSceneBrightness = true;

    BillboardInstance[] _instances = new BillboardInstance[0];
    float _fBaseSizeX = 100.0f;
    float _fBaseSizeY = 200.0f;
    float _fRelCenterX = 0.5f;
    float _fRelCenterY = 1.0f;
    TextureAtlas _atlas = new TextureAtlas();
    string _atlasFilename;
    float _fNearClip = 0.0f;
    float _fFarClip = -1.0f;
    bool _bCastStaticShadows = false;

    [PrefabResolveFilename]
    [Description("Texture to use as a billboard.")]
    [EditorAttribute(typeof(AssetEditor), typeof(UITypeEditor)), AssetDialogFilter(new string[] { "Texture" })]
    [SortedCategory(CAT_BILLBOARDS, CATORDER_BILLBOARDS), PropertyOrder(1),
    DefaultValue(@"\plainwhite.dds")]
    public string TextureFile
    {
      get { return _textureFile; }
      set 
      { 
        _textureFile = value;
        if (HasEngineInstance())
        {
          EngineMesh.SetTexture(_textureFile);
          UpdateBillboardMesh();
        }
      }
    }

    [Browsable(false)]
    [Description("Texture atlas definition")]
    [EditorAttribute(typeof(TextureAtlasEditor), typeof(UITypeEditor))]
    [SortedCategory(CAT_BILLBOARDS, CATORDER_BILLBOARDS), PropertyOrder(2)]
    public TextureAtlas Atlas
    {
      get 
      {
        _atlas.TextureFile = this.TextureFile;
        return _atlas; 
      }
      set 
      { 
        _atlas = value; 
      }
    }

    
    [Description("Texture atlas filename (optional)")]
    [EditorAttribute(typeof(TextureAtlasFilenameEditor), typeof(UITypeEditor)), AtlasSourceTextureProperty("TextureFile")]
    [SortedCategory(CAT_BILLBOARDS, CATORDER_BILLBOARDS), PropertyOrder(3)]
    public string TextureAtlasFile
    {
      get { return _atlasFilename; }
      set 
      {
        _atlasFilename = value;
        _atlas = TextureAtlas.CreateFromFile(_atlasFilename);
        if (_atlas == null)
          _atlas = new TextureAtlas();
        _atlas.TextureFile = this.TextureFile;
        UpdateBillboardMesh();
      }
    }

    [Description("Modulation Color for all billboards")]
    [SortedCategory(CAT_BILLBOARDS, CATORDER_BILLBOARDS), PropertyOrder(4)]
    [EditorAttribute(typeof(StandardColorDropDownEditor), typeof(UITypeEditor))]
    [PropertyLiveUpdate("ModulationColorLiveUpdate"),
    DefaultValue(typeof(Color), "255,255,255")]
    public Color ModulationColor
    {
      get { return _color; }
      set
      {
        _color = value;
        if (HasEngineInstance())
        {
          EngineMesh.SetColor(VisionColors.Get(_color), _bUseSceneBrightness);
          UpdateBillboardMesh();
        }
      }
    }

    /// <summary>
    /// Proxy property used for color live update (See PropertyLiveUpdate attribute in LightColor property)
    /// </summary>
    [Browsable(false)]
    public object ModulationColorLiveUpdate
    {
      set
      {
        Color col = _color;
        if (value is Color)
          col = (Color)value;
        if (HasEngineInstance())
        {
          EngineMesh.SetColor(VisionColors.Get(col), _bUseSceneBrightness);
          UpdateBillboardMesh();
        }
        EditorManager.ActiveView.UpdateView(false);
      }
    }

    [Description("If true, instances are additionally tinted by the scene brightess at the pivot")]
    [SortedCategory(CAT_BILLBOARDS, CATORDER_BILLBOARDS), PropertyOrder(5),
    DefaultValue(true)]
    public bool UseSceneBrightness
    {
      get { return _bUseSceneBrightness; }
      set
      {
        _bUseSceneBrightness = value;
        if (HasEngineInstance())
        {
          EngineMesh.SetColor(VisionColors.Get(_color), _bUseSceneBrightness);
          EngineMesh.SetBlendMode(_blendMode, _bUseSceneBrightness);
          UpdateBillboardMesh();
        }
      }
    }
    


    [SortedCategory(CAT_BILLBOARDS, CATORDER_BILLBOARDS), PropertyOrder(10),
    DefaultValue(TransparencyType.AlphaTest)]
    public TransparencyType BlendMode
    {
      get { return _blendMode; }
      set 
      { 
        _blendMode = value;
        if (HasEngineInstance())
          EngineMesh.SetBlendMode(_blendMode, _bUseSceneBrightness);
      }
    }

    [Browsable(false)]
    [SortedCategory(CAT_BILLBOARDS, CATORDER_BILLBOARDS), PropertyOrder(50)]
    [Description("Collection of billboard instances")]
    public BillboardInstance[] Instances
    {
      get { return _instances; }
      set 
      { 
        _instances = value;
        UpdateBillboardMesh();
      }
    }

    
    [SortedCategory(CAT_BILLBOARDS, CATORDER_BILLBOARDS), PropertyOrder(20)]
    [Description("Average size of billboards in x-direction"),
    DefaultValue(100.0f)]
    public float BaseSizeX
    {
      get
      {
        return _fBaseSizeX;
      }
      set
      {
        if (_fBaseSizeX == value)
          return;
        _fBaseSizeX = value;
        if (HasEngineInstance())
        {
          EngineMesh.SetBaseSize(_fBaseSizeX, _fBaseSizeY);
          UpdateBillboardMesh();
        }
      }
    }

    [SortedCategory(CAT_BILLBOARDS, CATORDER_BILLBOARDS), PropertyOrder(22)]
    [Description("Average size of billboards in y-direction"),
    DefaultValue(200.0f)]
    public float BaseSizeY
    {
      get
      {
        return _fBaseSizeY;
      }
      set
      {
        if (_fBaseSizeY == value)
          return;
        _fBaseSizeY = value;
        if (HasEngineInstance())
        {
          EngineMesh.SetBaseSize(_fBaseSizeX, _fBaseSizeY);
          UpdateBillboardMesh();
        }
      }
    }

    [PrefabRelevant(false)]
    [DependentProperties(new string[] { "PivotCenterX", "PivotCenterY" })]
    [SortedCategory(CAT_BILLBOARDS, CATORDER_BILLBOARDS), PropertyOrder(23)]
    [Description("Pre-defined values for Pivot Center"), DefaultValue(BillboardPivotCenter_e.BottomCenter)]
    public BillboardPivotCenter_e PivotCenter
    {
      get
      {
         if (_fRelCenterX == 0.0f && _fRelCenterY == 0.0f) return BillboardPivotCenter_e.UpperLeftCorner;
         if (_fRelCenterX == 1.0f && _fRelCenterY == 0.0f) return BillboardPivotCenter_e.UpperRightCorner;
         if (_fRelCenterX == 0.0f && _fRelCenterY == 1.0f) return BillboardPivotCenter_e.BottomLeftCorner;
         if (_fRelCenterX == 1.0f && _fRelCenterY == 1.0f) return BillboardPivotCenter_e.BottomRightCorner;
         if (_fRelCenterX == 0.5f && _fRelCenterY == 0.5f) return BillboardPivotCenter_e.Centered;
         if (_fRelCenterX == 0.5f && _fRelCenterY == 0.0f) return BillboardPivotCenter_e.TopCenter;
         if (_fRelCenterX == 0.5f && _fRelCenterY == 1.0f) return BillboardPivotCenter_e.BottomCenter;
         if (_fRelCenterX == 1.0f && _fRelCenterY == 0.5f) return BillboardPivotCenter_e.RightCenter;
         if (_fRelCenterX == 0.0f && _fRelCenterY == 0.5f) return BillboardPivotCenter_e.LeftCenter;
         return BillboardPivotCenter_e.Custom;
      }
      set
      {
        switch (value)
        {
          case BillboardPivotCenter_e.UpperLeftCorner: _fRelCenterX = 0.0f; _fRelCenterY = 0.0f; break;
          case BillboardPivotCenter_e.UpperRightCorner: _fRelCenterX = 1.0f; _fRelCenterY = 0.0f; break;
          case BillboardPivotCenter_e.BottomLeftCorner: _fRelCenterX = 0.0f; _fRelCenterY = 1.0f; break;
          case BillboardPivotCenter_e.BottomRightCorner: _fRelCenterX = 1.0f; _fRelCenterY = 1.0f; break;
          case BillboardPivotCenter_e.Centered: _fRelCenterX = 0.5f; _fRelCenterY = 0.5f; break;
          case BillboardPivotCenter_e.TopCenter: _fRelCenterX = 0.5f; _fRelCenterY = 0.0f; break;
          case BillboardPivotCenter_e.BottomCenter: _fRelCenterX = 0.5f; _fRelCenterY = 1.0f; break;
          case BillboardPivotCenter_e.RightCenter: _fRelCenterX = 1.0f; _fRelCenterY = 0.5f; break;
          case BillboardPivotCenter_e.LeftCenter: _fRelCenterX = 0.0f; _fRelCenterY = 0.5f; break;
        }
        if (!HasEngineInstance())
          return;
        EngineMesh.SetRelCenter(_fRelCenterX, _fRelCenterY);
        UpdateBillboardMesh();
      }
    }


    [SortedCategory(CAT_BILLBOARDS, CATORDER_BILLBOARDS), PropertyOrder(24)]
    [Description("Relative Pivot Center"),
    DefaultValue(0.5f)]
    public float PivotCenterX
    {
      get
      {
        return _fRelCenterX;
      }
      set
      {
        if (_fRelCenterX == value)
          return;
        _fRelCenterX = value;
        if (HasEngineInstance())
        {
          EngineMesh.SetRelCenter(_fRelCenterX, _fRelCenterY);
          UpdateBillboardMesh();
        }
      }
    }

    [SortedCategory(CAT_BILLBOARDS, CATORDER_BILLBOARDS), PropertyOrder(25)]
    [Description("Relative Pivot Center"),
    DefaultValue(1.0f)]
    public float PivotCenterY
    {
      get
      {
        return _fRelCenterY;
      }
      set
      {
        if (_fRelCenterY == value)
          return;
        _fRelCenterY = value;
        if (HasEngineInstance())
        {
          EngineMesh.SetRelCenter(_fRelCenterX, _fRelCenterY);
          UpdateBillboardMesh();
        }
      }
    }

    [SortedCategory(CAT_BILLBOARDS, CATORDER_BILLBOARDS), PropertyOrder(30)]
    [Description("Near clip distance (in units)"), DefaultValue(0.0f)]
    public float NearClipDistance
    {
      get
      {
        return _fNearClip;
      }
      set
      {
        _fNearClip = value;
        if (_fNearClip < 0.0f)
          _fNearClip = 0.0f;
        if (HasEngineInstance())
          EngineMesh.SetClipDistances(_fNearClip, _fFarClip);
      }
    }

    [SortedCategory(CAT_BILLBOARDS, CATORDER_BILLBOARDS), PropertyOrder(31)]
    [Description("Far clip distance (in units). -1 means no far clipping"), DefaultValue(-1.0f)]
    public float FarClipDistance
    {
      get
      {
        return _fFarClip;
      }
      set
      {
        _fFarClip = value;
        if (_fFarClip < 0.0f)
          _fFarClip = -1.0f; // -1 means no far clipping
        if (HasEngineInstance())
          EngineMesh.SetClipDistances(_fNearClip, _fFarClip);
      }
    }

    [Description("If enabled, the billboards will cast static shadows computed by vLux. The billboards are aligned towards the most significant light source.")]
    [SortedCategory(CAT_BILLBOARDS, CATORDER_BILLBOARDS), PropertyOrder(40), DefaultValue(false)]
    public bool CastStaticShadows
    {
      get { return _bCastStaticShadows; }
      set
      {
        _bCastStaticShadows = value;
        if (HasEngineInstance())
          EngineMesh.SetCastStaticShadows(_bCastStaticShadows);
      }
    }

    #endregion

    #region Object key

    /// <summary>
    /// Object key string
    /// </summary>
    protected string _objectKey = null;

    /// <summary>
    /// Gets or sets the object key, so the object can be searched in the engine
    /// </summary>
    [Description("Arbitrary key string to identify this instance inside the game application")]
    [SortedCategory(CAT_SHAPE, CATORDER_SHAPE), PropertyOrder(100)]
    public string ObjectKey
    {
        get { return _objectKey; }
        set
        {
            _objectKey = value;
            if (HasEngineInstance())
                _engineInstance.SetObjectKey(_objectKey);
        }
    }

    #endregion

    #region Static lighting

    protected override void GetStaticLightInfo(StaticLightInfo info)
    {
      base.GetStaticLightInfo(info);

      if (CastStaticShadows && this.Instances != null && this.Instances.Length > 0)
        info.RelevantOccluder.Add(this);

    }

    #endregion

    #region Prefab

    string _rawData = null;
    int _rawDataVersion = 0;

    [Browsable(false)]
    [PrefabRelevant(true)]
    public int RawDataVersion
    {
      get 
      {
        _rawDataVersion = EngineInstanceBillboardGroup.GetCurrentRawDataVersion();
        return _rawDataVersion; 
      }
      set { _rawDataVersion = value; }
    }

    [Browsable(false)]
    [PrefabRelevant(true)]
    public string RawData
    {
      get 
      {
        _rawData = EngineInstanceBillboardGroup.RawDataFromInstances(this._instances);
        return _rawData; 
      }
      set { _rawData = value; } // processed later
    }


    #endregion

    #region ISerializable

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected BillboardGroupShape(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
//      if (SerializationHelper.HasElement(info,"_instances"))
      _instances = (BillboardInstance[])info.GetValue("_instances", typeof(BillboardInstance[]));

      _fBaseSizeX = info.GetSingle("_fBaseSizeX");
      _fBaseSizeY = info.GetSingle("_fBaseSizeY");
      _fRelCenterX = info.GetSingle("_fRelCenterX");
      _fRelCenterY = info.GetSingle("_fRelCenterY");
      _textureFile = info.GetString("_textureFile");
      _blendMode = (TransparencyType)info.GetValue("_blendMode", typeof(TransparencyType));
      _color = (Color)info.GetValue("_color", typeof(Color));
      _bUseSceneBrightness = info.GetBoolean("_bUseSceneBrightness");
      if (SerializationHelper.HasElement(info, "_atlasFilename"))
        _atlasFilename = info.GetString("_atlasFilename");
      if (SerializationHelper.HasElement(info, "_fNearClip"))
        _fNearClip = info.GetSingle("_fNearClip");
      if (SerializationHelper.HasElement(info, "_fFarClip"))
        _fFarClip = info.GetSingle("_fFarClip");
      if (SerializationHelper.HasElement(info, "_bCastStaticShadows"))
        _bCastStaticShadows = info.GetBoolean("_bCastStaticShadows");
      if (SerializationHelper.HasElement(info, "_objectKey"))
          _objectKey = info.GetString("_objectKey");
    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context) 
    {
      base.GetObjectData(info,context);
      info.AddValue("_instances", _instances);
      info.AddValue("_fBaseSizeX", _fBaseSizeX);
      info.AddValue("_fBaseSizeY", _fBaseSizeY);
      info.AddValue("_fRelCenterX", _fRelCenterX);
      info.AddValue("_fRelCenterY", _fRelCenterY);
      info.AddValue("_textureFile", _textureFile);
      info.AddValue("_blendMode", _blendMode);
      info.AddValue("_color", _color);
      info.AddValue("_bUseSceneBrightness", _bUseSceneBrightness);
      info.AddValue("_atlasFilename", _atlasFilename);
      info.AddValue("_fNearClip", _fNearClip);
      info.AddValue("_fFarClip", _fFarClip);
      info.AddValue("_bCastStaticShadows", _bCastStaticShadows);
      info.AddValue("_objectKey", _objectKey);
    }


    public override void OnDeserialization()
    {
      base.OnDeserialization();

      TextureAtlasFile = _atlasFilename; // load atlas
    }


    #endregion

    #region Clone

    /// <summary>
    /// Overridden clone function
    /// </summary>
    /// <returns></returns>
    public override ShapeBase Clone()
    {
      BillboardGroupShape copy = (BillboardGroupShape)base.Clone();

      // deep clone instance list
      copy._instances = new BillboardInstance[this._instances.Length];
      copy._selectedBillboards = null;
      copy._rawData = null;
      copy._customHotSpots = null;

      for (int i = 0; i < this._instances.Length; i++)
        copy._instances[i] = (BillboardInstance)_instances[i].Clone();

      return copy;
    }

    #endregion

    #region GUI

    /// <summary>
    /// Overridden function to get the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetIconIndex()
    {
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_billboard.bmp"), Color.Magenta);
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


    #region Context Dialog


    static BillboardGroupShape _currentBillboard = null;
 
    [Browsable(false)]
    [PrefabRelevant(false)]
    public static BillboardGroupShape CurrentBillboard
    {
      get { return BillboardGroupShape._currentBillboard; }
      set 
      {
        if (_currentBillboard == value)
          return;
        if (_currentBillboard != null)
          _currentBillboard.ClearSelection();

        _currentBillboard = value;
        if (_currentBillboard == null)
        {
          if (_contextDlg != null)
            _contextDlg.Visible = false;
        }
        else
        {
          if (_contextDlg == null)
          {
            if (TestManager.IsRunning)
              return;

            _contextDlg = new BillboardContextDlg();
            _contextDlg.Show(EditorManager.ActiveView);
          }
          _contextDlg.Visible = true;
          _currentBillboard.ClearSelection();
          // automatically switch to add mode for empty billboard groups
          if (_currentBillboard.Instances.Length==0)
            _contextDlg.SetBillboardAddContext();
        }
      }
    }

    public static BillboardInstance PickBillboard(int mx, int my)
    {
      BillboardGroupShape shape = BillboardGroupShape.CurrentBillboard;
      if (shape == null)
        return null;

      Vector3F startRay, endRay;
      EditorManager.EngineManager.GetRayAtScreenPos(out startRay, out endRay, mx, my, EditorManager.Settings.MaxPickingDistance);
      BillboardInstance inst = shape.TraceBillboard(startRay, endRay);
      return inst;
    }


    static BillboardContextDlg _contextDlg;



    public override void OnSelected()
    {
      base.OnSelected();
      if (EditorManager.SelectedShapes.Count == 1) // only this one?
        CurrentBillboard = this;
    }

    public override void OnUnSelected()
    {
      base.OnUnSelected();
      ClearSelection();
      if (CurrentBillboard == this)
        CurrentBillboard = null;
    }

    #endregion
  }

  #endregion

  #region Class : BillboardGroupShapeCreator

  class BillboardGroupShapeCreator : CSharpFramework.IShapeCreatorPlugin
  {
    public BillboardGroupShapeCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_billboard.bmp"), Color.Magenta);
    }

    /// <summary>
    /// Get the name of the creator
    /// </summary>
    /// <returns></returns>
    public override string GetPluginName()
    {
      return "Billboard Group";
    }


    /// <summary>
    /// Get the category's name
    /// </summary>
    /// <returns></returns>
    public override string GetPluginCategory()
    {
      return "Geometry";
    }

    /// <summary>
    /// Returns a short description text
    /// </summary>
    /// <returns></returns>
    public override string GetPluginDescription()
    {
      return "Adds a billboard group. A billboard group is rendered as one draw call which makes it the ideal solution to add a lot of distance billboards. If this feature is massively used across a scene, splitting into spatial billboard groups is recommended for more efficient visibility culling.";
    }


    /// <summary>
    /// Overridden function to create the shape instance
    /// </summary>
    /// <returns></returns>
    public override ShapeBase CreateShapeInstance()
    {
      BillboardGroupShape shape = new BillboardGroupShape("Billboard Group");
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
