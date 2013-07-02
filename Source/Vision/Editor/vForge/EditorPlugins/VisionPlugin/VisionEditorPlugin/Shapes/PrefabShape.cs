/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.IO;
using System.Drawing;
using System.Drawing.Design;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Runtime;
using System.Runtime.Serialization;
using System.ComponentModel;
using System.Windows;
using System.Windows.Forms;
using CSharpFramework;
using CSharpFramework.Actions;
using CSharpFramework.Shapes;
using CSharpFramework.Scene;
using CSharpFramework.PropertyEditors;
using CSharpFramework.Serialization;
using ManagedFramework;
using CSharpFramework.Math;
using CSharpFramework.View;
using CSharpFramework.DynamicProperties;
using System.Reflection;
using VisionManaged;

namespace VisionEditorPlugin.Shapes
{

  #region Class : LocalIDDict

  /// <summary>
  /// An instance of this class maps local ID (inside prefab) to scene local IDs
  /// </summary>
  [Serializable]
  public class LocalIDDict : Dictionary<uint, uint>, ISerializable, IDeserializationCallback
  {
    public LocalIDDict()
    {
    }

    public LocalIDDict(int iCapacity)
      : base(iCapacity)
    {
    }

    /// <summary>
    /// For ICustomPrefabSerialization implementations
    /// </summary>
    /// <param name="iLocalID"></param>
    /// <param name="customObj"></param>
    public void SetCustomSerializationObject(uint iLocalID, object customObj)
    {
      if (customObj == null)
        return;
      if (_customSerializationObjects == null)
        _customSerializationObjects = new Dictionary<uint, object>();

      // an old version must be removed
      if (_customSerializationObjects.ContainsKey(iLocalID))
        _customSerializationObjects.Remove(iLocalID);

      _customSerializationObjects.Add(iLocalID, customObj);
    }

    /// <summary>
    /// For ICustomPrefabSerialization implementations
    /// </summary>
    /// <param name="iLocalID"></param>
    /// <param name="customObj"></param>
    /// <returns></returns>
    public bool GetCustomSerializationObject(uint iLocalID, out object customObj)
    {
      if (_customSerializationObjects != null && _customSerializationObjects.TryGetValue(iLocalID, out customObj))
        return true;

      customObj = null;
      return false;
    }


    Dictionary<uint, object> _customSerializationObjects = null;

    #region ISerializable Members

    int iCustomCount = 0;
    uint[] iCustomKeys = null;
    object[] iCustomValues = null;

    void IDeserializationCallback.OnDeserialization(object sender)
    {
      if (_customSerializationObjects == null)
        AfterSerialization();
    }

    public void AfterSerialization()
    {
      if (iCustomCount > 0)
      {
        _customSerializationObjects = new Dictionary<uint, object>(iCustomCount);
        // objects are not yet valid in constructor
        for (int i = 0; i < iCustomCount; i++)
          _customSerializationObjects.Add(iCustomKeys[i], iCustomValues[i]);
      }
    }

    protected LocalIDDict(SerializationInfo info, StreamingContext context)
    {
      {
        int iCount = info.GetInt32("_count");

        uint[] iKeys = (uint[])info.GetValue("_keys", typeof(uint[]));
        uint[] iValues = (uint[])info.GetValue("_values", typeof(uint[]));
        for (int i = 0; i < iCount; i++)
          this.Add(iKeys[i], iValues[i]);
      }

      if (SerializationHelper.HasElement(info, "_customcount"))
      {
        iCustomCount = info.GetInt32("_customcount");
        iCustomKeys = (uint[])info.GetValue("_customkeys", typeof(uint[]));
        iCustomValues = (object[])info.GetValue("_customvalues", typeof(object[]));
      }
    }

    void ISerializable.GetObjectData(SerializationInfo info, StreamingContext context)
    {
      // local ID matching
      {
        int iCount = Count;
        info.AddValue("_count", iCount);
        uint[] iKeys = new uint[iCount];
        uint[] iValues = new uint[iCount];
        int i = 0;
        foreach (System.Collections.Generic.KeyValuePair<uint, uint> pair in this)
        {
          iKeys[i] = pair.Key;
          iValues[i] = pair.Value;
          i++;
        }
        info.AddValue("_keys", iKeys);
        info.AddValue("_values", iValues);
      }

      // custom IDs
      if (_customSerializationObjects != null)
      {
        int iCount = _customSerializationObjects.Count;
        info.AddValue("_customcount", iCount);
        uint[] iKeys = new uint[iCount];
        object[] iValues = new object[iCount];
        int i = 0;
        foreach (System.Collections.Generic.KeyValuePair<uint, object> pair in _customSerializationObjects)
        {
          iKeys[i] = pair.Key;
          iValues[i] = pair.Value;
          i++;
        }
        info.AddValue("_customkeys", iKeys);
        info.AddValue("_customvalues", iValues);
      }

    }

    #endregion

  }

  #endregion

  #region Class : LocalIDDictDict

  /// <summary>
  /// An instance of this class holds a list with prefab shape paths (string) each associated with an instance of LocalIDDict which in turn
  /// maps local IDs inside that nested prefab. An instance of this class is held by a non-nested prefab shape to be able to preserve unique IDs in
  /// nested sub-prefabs.
  /// </summary>
  [Serializable]
  public class LocalIDDictDict : Dictionary<string, LocalIDDict>, ISerializable, IDeserializationCallback
  {
    public LocalIDDictDict()
    {
    }

    public LocalIDDictDict(int iCapacity)
      : base(iCapacity)
    {
    }

    #region ISerializable Members

    int iCount = 0;
    string[] sKeys = null;
    LocalIDDict[] pValues = null;

    void IDeserializationCallback.OnDeserialization(object sender)
    {
      // pValues array is not initialized during serialization
      for (int i = 0; i < iCount; i++)
        this.Add(sKeys[i], pValues[i]);
    }

    protected LocalIDDictDict(SerializationInfo info, StreamingContext context)
    {
      iCount = info.GetInt32("_count");
      sKeys = (string[])info.GetValue("_keys", typeof(string[]));
      pValues = (LocalIDDict[])info.GetValue("_values", typeof(LocalIDDict[]));
    }



    void ISerializable.GetObjectData(SerializationInfo info, StreamingContext context)
    {
      int iCount = Count;
      info.AddValue("_count", iCount);
      string[] iKeys = new string[iCount];
      LocalIDDict[] iValues = new LocalIDDict[iCount];
      int i = 0;
      foreach (System.Collections.Generic.KeyValuePair<string, LocalIDDict> pair in this)
      {
        iKeys[i] = pair.Key;
        iValues[i] = pair.Value;
        i++;
      }
      info.AddValue("_keys", iKeys);
      info.AddValue("_values", iValues);
    }


    #endregion

  }

  #endregion

  #region Class : PrefabShape

  [Serializable, ShapeTypeNiceName("Prefab")]
  public class PrefabShape : IPrefabShape
  {


    #region Global Events

    /// <summary>
    /// This event gets fired before a prefab is collapsed in vForge. The sender is the prefab shape
    /// </summary>
    public static event EventHandler OnBeforePrefabCollapsed = null;

    /// <summary>
    /// This event gets fired after a prefab is collapsed in vForge. The sender is the prefab shape
    /// </summary>
    public static event EventHandler OnAfterPrefabCollapsed = null;

    /// <summary>
    /// Triggers OnBeforePrefabCollapsed event
    /// </summary>
    public void TriggerBeforePrefabCollapsed()
    {
      if (PrefabShape.OnBeforePrefabCollapsed != null)
        PrefabShape.OnBeforePrefabCollapsed(this, EventArgs.Empty);
    }

    /// <summary>
    /// Triggers OnAfterPrefabCollapsed event 
    /// </summary>
    public void TriggerAfterPrefabCollapsed()
    {
      if (PrefabShape.OnAfterPrefabCollapsed != null)
        PrefabShape.OnAfterPrefabCollapsed(this, EventArgs.Empty);
    }


    #endregion

    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_PREFAB = "Prefab";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_PREFAB = Shape3D.LAST_CATEGORY_ORDER_ID + 1;


    /// <summary>
    /// last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_PREFAB;

    #endregion

    #region Constructor

    public PrefabShape(string name)
      : base(name)
    {
      _ExportAsPrefabInstance = false;
      AddHint(HintFlags_e.HideChildren | HintFlags_e.NoChildren);
    }

    #endregion
  
    #region Engine Instance

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="bCreateChildren"></param>
    public override void CreateEngineInstance(bool bCreateChildren)
    {
      this._engineInstance = new EngineInstancePrefab();
      SetEngineInstanceBaseProperties(); // sets the position etc.

      base.CreateEngineInstance(bCreateChildren);
    }


    /// <summary>
    /// Overridden function
    /// </summary>
    public override void SetEngineInstanceBaseProperties()
    {
      if (_engineInstance != null)
      {
        EngineInstancePrefab instance = (EngineInstancePrefab)_engineInstance;
        instance.SetExportInstance(_ExportAsPrefabInstance);
        instance.SetFilename(_fileName);
      }
  
      base.SetEngineInstanceBaseProperties();
      BuildBoundingBox(_prefabChildrenList);
    }

    void IScene_EngineInstancesChanged(object sender, EngineInstancesChangedArgs e)
    {
      // The bounding boxes of the children are only present after this point, so we have to do this here
      if (e.action == EngineInstancesChangedArgs.Action.PostCreateAll)
      {
        BuildBoundingBox(_prefabChildrenList);
        InvalidateZoneBoundingBox();
      }
    }

    #endregion

    #region Shape overrides

    /// <summary>
    /// Hide some properties under certain circumstances
    /// </summary>
    /// <param name="pd"></param>
    /// <returns></returns>
    public override PropertyFlags_e GetPropertyFlags(PropertyFlagsProviderInfo pd)
    {

      if (pd.DynamicProperty!=null) // if this is a dynamic property then ask the matching shape
      {
        ISceneObject targetShape = null;
        object prop = PrefabDesc.FindShapeProperty(this, pd.Name, out targetShape);
        if (prop != null && targetShape is IPropertyFlagsProvider)
        {
          // but extract name-only from the property path
          int iLastDot = pd.Name.LastIndexOf('.');
          if (iLastDot >= 0)
          {
            string nameOnly = pd.Name.Substring(iLastDot + 1);
            pd = new PropertyFlagsProviderInfo { DynamicProperty = pd.DynamicProperty, PropertyDesc = pd.PropertyDesc, Name = nameOnly };
          }

          return ((IPropertyFlagsProvider)targetShape).GetPropertyFlags(pd);
        }
      }

      PropertyFlags_e flags = base.GetPropertyFlags(pd);
      if ((Editable || _perInstanceParameter==null) && pd.Name == "PerInstanceParameter")
      {
        flags |= PropertyFlags_e.Hidden;
      }
      else if (Editable && pd.Name == "Filename")
      {
        flags |= PropertyFlags_e.Readonly;
      }

      return flags;
    }

    protected override ParentingModes ParentingMode
    {
      get
      {
        return ParentingModes.SceneGraphScaled;
      }
    }

    public override void OnAddedToScene()
    {
      base.OnAddedToScene();
      if (!_ExportAsPrefabInstance) //only skip parenting if we do not use a VPrefabInstance 
      {
        this.AddHint(HintFlags_e.SkipParenting);
      }  

      PrefabDesc.OnPrefabSaved += new EventHandler(PrefabDesc_OnPrefabSaved);
      IScene.PropertyChanged += new CSharpFramework.PropertyChangedEventHandler(IScene_PropertyChanged);
      IScene.EngineInstancesChanged += new EngineInstancesChangedEventHandler(IScene_EngineInstancesChanged);

    }


    public override void PerformPostEngineInstanceCreationSetup(bool bCreateChildren)
    {
      if (_bPendingCreation)
      {
        _bPendingCreation = false;
        RecreateShapes(false, null);
        SetEngineInstanceBaseProperties();
        InvalidateZoneBoundingBox();

        PerInstanceParameterString = _pendingPerInstanceParameterString;
        _pendingPerInstanceParameterString = null;
      }

      base.PerformPostEngineInstanceCreationSetup(bCreateChildren);
    }


    public override void OnRemoveFromScene()
    {
      base.OnRemoveFromScene();
      IScene.EngineInstancesChanged -= new EngineInstancesChangedEventHandler(IScene_EngineInstancesChanged);
      PrefabDesc.OnPrefabSaved -= new EventHandler(PrefabDesc_OnPrefabSaved);
      IScene.PropertyChanged -= new CSharpFramework.PropertyChangedEventHandler(IScene_PropertyChanged);
    }


    /// <summary>
    /// Makes sure that child shapes are properly attached to the new parent
    /// </summary>
    /// <param name="newParent"></param>
    /// <param name="bValidateName"></param>
    public override void SetParent(ShapeBase newParent, bool bValidateName)
    {
      base.SetParent(newParent, bValidateName);    
  //    ShapeBase attachableParent = this.NextShape3DParent;

      // might move this code into ShapeBase if this is not a valid parent to attach (HintFlags_e.SkipParenting)
      if (_prefabChildrenList != null)// && attachableParent != null && attachableParent.HasEngineInstance())
        foreach (ShapeBase child in _prefabChildrenList)
          child.UpdateEngineInstanceAttachment();

    }

    public override void OnSceneEvent(SceneEventArgs args)
    {
      base.OnSceneEvent(args);
      if (args.action == SceneEventArgs.Action.AfterUniqueIDRepaired && !this.Editable)
      {
        _uniqueIDMatch = null;
        RecreateShapes(false, _prefabDesc);
      }
    }


    public override void OnTransformationChanged()
    {
      base.OnTransformationChanged();
      if (_bAnyPerInstanceTransformation && _perInstanceParameter != null && _perInstanceParameter.Properties != null)
      {
        foreach (DynamicProperty prop in _perInstanceParameter.Properties)
          if (prop.PropertyType.GetAttribute(typeof(PrefabTransformationUpdateAttribute), false) != null)
            ApplyPerInstanceProperty(prop);
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
      if (this._bEditable)
        return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\lock_open.png"), Color.Magenta);
      if (this.Broken)
        return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\lock_error.png"), Color.Magenta);

      return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\lock_ok.png"), Color.Magenta);
    }

    #endregion

    #region Cloning

    /// <summary>
    /// This function must be overridden, since we have to reset the hotspots
    /// </summary>
    /// <returns></returns>
    public override ShapeBase Clone()
    {
      PrefabShape copy = base.Clone() as PrefabShape;
      // the children list is null since we have the NoChildren hint flag

      // make sure we do not shallow copy these lists
      copy._flatShapes3DList = null;
      copy._prefabChildrenList = null;
      copy._mergedLocalBBox = new BoundingBox();
      copy._uniqueIDMatch = null; // force creating new IDs

      // instead, recreate the prefab using the same filename (but do not call OnAddedToScene here)
      // this also clones the per-instance properties
      if (!Editable && ParentLayer != null)
        copy.RecreateShapes(true, this.Prefab);
      return copy;
    }

    #endregion

    #region ISerializable

    /// <summary>
    /// This list holds instances of class LocalIDDict for every nested prefab. The reference to the nested prefab is done by path string, 
    /// because local IDs are not yet resolved - this would be too complicated.
    /// </summary>
    LocalIDDictDict _nestedPrefabs = null;


    void RecursiveAddCustomSerializationObjects(ShapeCollection children, LocalIDDict dict, int iVirtualCtr)
    {
      if (dict == null || children == null)
        return;
      foreach (ShapeBase child in children)
      {
        if (child.ShapeVirtualCounter != iVirtualCtr)
          break; // we can stop here

        RecursiveAddCustomSerializationObjects(child.ChildCollection, dict, iVirtualCtr); // recurse (on the same virtual counter level)

        ICustomPrefabSerialization cshape = child as ICustomPrefabSerialization;
        if (cshape == null)
          continue;
 
        object customObj = cshape.GetCustomPrefabSerializationObject();
        dict.SetCustomSerializationObject(child.LocalID, customObj);
      }
    }

    /// <summary>
    /// Before saving, a fresh list with nested prefabs is generated
    /// </summary>
    /// <param name="children"></param>
    void BuildNestedPrefabIDTables(ShapeCollection children)
    {
      if (children == null)
        return;
      foreach (ShapeBase child in children)
      {
        BuildNestedPrefabIDTables(child.ChildCollection); // recurse
        PrefabShape prefab = child as PrefabShape;
        if (prefab == null)
          continue;
        LocalIDDict ids = prefab.IDMatchTable;
        if (ids == null || ids.Count == 0)
          continue;
        if (_nestedPrefabs == null)
          _nestedPrefabs = new LocalIDDictDict();

        string path = prefab.ShapePath.ToString();
        _nestedPrefabs.Add(path, ids); // associate a nested prefab with its ID matchtable

        RecursiveAddCustomSerializationObjects(prefab._prefabChildrenList, ids, prefab.ShapeVirtualCounter+1);
      }

    }


    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected PrefabShape(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      _fileName = info.GetString("_fileName");
      _bEditable = info.GetBoolean("_bEditable");

      if (SerializationHelper.HasElement(info, "_ExportAsPrefabInstance"))
      {
        _ExportAsPrefabInstance = info.GetBoolean("_ExportAsPrefabInstance");
      }

      // old way to de-serialize
      if (SerializationHelper.HasElement(info, "_IDMatchCount"))
      {
        int iCount = info.GetInt32("_IDMatchCount");
        _uniqueIDMatch = new LocalIDDict(iCount);
        uint[] iKeys = (uint[])info.GetValue("_IDMatchKeys", typeof(uint[]));
        uint[] iValues = (uint[])info.GetValue("_IDMatchValues", typeof(uint[]));
        for (int i = 0; i < iCount; i++)
          _uniqueIDMatch.Add(iKeys[i], iValues[i]);
      }
      else if (SerializationHelper.HasElement(info, "_uniqueIDMatch"))
      {
        _uniqueIDMatch = (LocalIDDict)info.GetValue("_uniqueIDMatch", typeof(LocalIDDict));
        if (_uniqueIDMatch != null)
          _uniqueIDMatch.AfterSerialization(); // This hack is necessary so that custom objects are available here and just after whole serialization
      }

      if (SerializationHelper.HasElement(info, "_nestedPrefabs"))
        _nestedPrefabs = (LocalIDDictDict)info.GetValue("_nestedPrefabs", typeof(LocalIDDictDict));

      if (SerializationHelper.HasElement(info, "_perInstanceParameter"))
        _perInstanceParameter = (PrefabDesc.PerInstancePropertyCollection)info.GetValue("_perInstanceParameter", typeof(PrefabDesc.PerInstancePropertyCollection));
    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context) 
    {
      base.GetObjectData(info,context);

      info.AddValue("_fileName", _fileName);
      info.AddValue("_bEditable", _bEditable);
      info.AddValue("_ExportAsPrefabInstance", _ExportAsPrefabInstance);

      _nestedPrefabs = null;
      RecursiveAddCustomSerializationObjects(this._prefabChildrenList, _uniqueIDMatch, this.ShapeVirtualCounter + 1);
      BuildNestedPrefabIDTables(this._prefabChildrenList);
      if (_nestedPrefabs != null)
        info.AddValue("_nestedPrefabs", _nestedPrefabs);

      if (_uniqueIDMatch != null && _uniqueIDMatch.Count>0)
      {
        info.AddValue("_uniqueIDMatch", _uniqueIDMatch);
      }

      info.AddValue("_perInstanceParameter", this._perInstanceParameter);
    }

    public override void OnDeserialization()
    {
      base.OnDeserialization();
      if (_perInstanceParameter != null)
        _perInstanceParameter.Owner = this;

      if (!Editable) // otherwise shapes are standard children
        this.RecreateShapes(true,null);
      else //create prefabdesc anyway
        _prefabDesc = PrefabManager.CreatePrefab(_fileName);
    }


    #endregion

    #region Render and Trace


    public override void RenderShape(VisionViewBase view, ShapeRenderMode mode)
    {
      base.RenderShape(view, mode);

/*
      if (_flatShapes3DList != null && mode == ShapeRenderMode.Selected)
        foreach (Shape3D shape in _flatShapes3DList)
          shape.RenderShape(view, ShapeRenderMode.Normal);
 */
    }


    public override void OnTraceShape(ShapeTraceMode_e mode, Vector3F rayStart, Vector3F rayEnd, ref ShapeTraceResult result)
    {
      ShapeBase oldHit = result.hitShape;
      base.OnTraceShape(mode, rayStart, rayEnd, ref result);
      if (_flatShapes3DList!=null)
        foreach (Shape3D shape in _flatShapes3DList)
          shape.OnTraceShape(mode, rayStart, rayEnd, ref result);

      if (result.hitShape != null && result.hitShape != oldHit) // pretend this one has been hit
        result.hitShape = this;
    }

    #endregion

    #region Members

    string _fileName = null;
    bool _ExportAsPrefabInstance = false;
    bool _bEditable = false; // still a prefab
    bool _bBroken = false;
    PrefabDesc.PerInstancePropertyCollection _perInstanceParameter;

    #endregion

    #region Properties

    /// <summary>
    /// Indicates whether the prefab file could not be loaded
    /// </summary>
    [Browsable(false)]
    public bool Broken
    {
      get { return _bBroken; }
      set 
      {
        if (_bBroken == value)
          return;

        if (value)
        {
          EditorManager.EngineManager.LogPrintWarning("Missing or broken prefab file \"" + Filename + "\".");
        }

        _bBroken = value;
        InvalidateIcon();
      }
    }

    /// <summary>
    /// Hint flag from the header files that indicates whether this prefab can be made editable in vForge
    /// </summary>
    [Browsable(false)]
    public bool Convertible
    {
      get 
      {
        return _prefabDesc!=null && _prefabDesc.Convertible; 
      }
    }

    [Browsable(false)]
    public override object ToolTip
    {
      get
      {
        if (Editable)
          return "(editable)";

        if (!Broken)
        {
          if (_fileName == null || _fileName == "")
            return "No prefab file assigned";
          return "Filename : " + _fileName;
        }

        if (_prefabDesc != null && _prefabDesc.LastError!=null)
          return "Error : " + _prefabDesc.LastError;

        return base.ToolTip;
      }
    }


    [SortedCategory(CAT_PREFAB, CATORDER_PREFAB),PropertyOrder(2)]
    [Description("Indicates whether the shapes are currently editable or not")]
    public bool Editable
    {
      get { return _bEditable; }
    }

    /// <summary>
    /// Internal property for prefab
    /// </summary>
    [PrefabRelevant(true)]
    [Browsable(false)]
    public bool EditableInPrefab
    {
      get { return _bEditable; }
      set 
      {
        _bEditable = value;
        if (_bEditable)
        {
          RemoveHint(ShapeBase.HintFlags_e.NoChildren);
          RemoveHint(ShapeBase.HintFlags_e.HideChildren);
        }
        else
        {
          AddHint(ShapeBase.HintFlags_e.NoChildren);
          AddHint(ShapeBase.HintFlags_e.HideChildren);
        }

      }
    }

    /// <summary>
    /// Gets/sets the filename of the prefab (implements the IPrefabShape function)
    /// </summary>
    [PrefabResolveFilename]
    [DependentProperties(new string[] { "PerInstanceParameter", "AutoRenaming" })]
    [SortedCategory(CAT_PREFAB, CATORDER_PREFAB),PropertyOrder(1),
    EditorAttribute(typeof(AssetEditor), typeof(UITypeEditor)), AssetDialogFilter(new string[] { "Prefab" })]
    [ThumbnailFormat(new string[] { @"{0}.Thumbnail.bmp", @"{0}.Thumbnail.jpg" })]
    [Description("Gets/sets the filename of the prefab.")]
    override public string Filename
    {
      get
      {
        return _fileName;
      }
      set
      {
        if (_fileName == value)
        {
          return;
        }
        if (Editable)
        {
          //EditorManager.ShowMessageBox("Cannot set a new prefab filename when it is already editable.\n\nPlease create a new prefab shape", "Cannot change filename", MessageBoxButtons.OK, MessageBoxIcon.Information);
          _fileName = value;
          return;
        }
        else
        {
          if (!CheckForCyclicPrefabs(value))
            return;
        }

        _fileName = value;
        _uniqueIDMatch = null;  // uncache the IDs

        if (!IsAddedToScene) // this happens when using nested prefabs. See OnPostPrefabCreation
        {
          _bPendingCreation = true;
          return;
        }

        // Set name automatically unless the shape has been renamed manually
        if (AutoRenaming && EditorManager.Scene != null)
        {
          string name = Path.GetFileNameWithoutExtension(_fileName);
          ShapeName = String.IsNullOrEmpty(name) ? "Prefab" : name;
        }

        RecreateShapes(false,null);
        SetEngineInstanceBaseProperties();
        InvalidateZoneBoundingBox();
        
        //fire unique id changed event
        UniqueIDInPrefabChangedArgs args = new UniqueIDInPrefabChangedArgs(_prefabDesc.PrefabChildrenListWithUniqueIDFromXML);
        IScene.SendUniqueIDInPrefabChangedEvent(args);
      }
    }

    /// <summary>
    /// A prefab definition can expose per-instance variables which can be edited here
    /// </summary>
    [PrefabRelevant(false)]
    [SortedCategory(CAT_PREFAB, CATORDER_PREFAB),PropertyOrder(50)]
    [Description("A prefab definition can expose per-instance variables which can be edited here")]
    public PrefabDesc.PerInstancePropertyCollection PerInstanceParameter
    {
      get { return _perInstanceParameter; }
      set { _perInstanceParameter = value; }
    }


    /// <summary>
    /// Expose the per-instance parameters as a string for nested prefabs
    /// </summary>
    [PrefabRelevant(true)]
    [Browsable(false)]
    public string PerInstanceParameterString
    {
      get
      {
        if (_perInstanceParameter == null)
          return null;
        return _perInstanceParameter.GetPropertyParameterString(",");
      }
      set
      {
        if (string.IsNullOrEmpty(value))
          return;
        if (_perInstanceParameter != null)
          _perInstanceParameter.ParseParameterString(value, ",");
        else
          _pendingPerInstanceParameterString = value; // set later
      }
    }

    /// <summary>
    /// Overidden function
    /// </summary>
    [Browsable(false)]
    [DependentProperties(new string[] { "Filename", "PerInstanceParameter" })]
    override public PrefabDesc Prefab 
    { 
      get
      {
        return this._prefabDesc;
      }
      set
      {
        if (_prefabDesc == value)
          return;
        _uniqueIDMatch = null; // uncache the IDs
        RecreateShapes(false, value);
        SetEngineInstanceBaseProperties();
        InvalidateIcon();


      }
    }

    /// <summary>
    /// Just sets the reference; For internal use
    /// </summary>
    /// <param name="newPrefab"></param>
    public void SetPrefabInternal(PrefabDesc newPrefab)
    {
      _prefabDesc = newPrefab;
      if (newPrefab == null)
        _prefabChildrenList = null;
    }

    [SortedCategory(CAT_PREFAB, CATORDER_PREFAB), PropertyOrder(2)]
    [Description("If true the scene export preserves the prefab as an instance of entity class VPrefabInstance which loads the .vprefab file at loading time. The ObjectKey property is set for this entity. If false, the shapes are flattened out during export"),
    DefaultValue(false)]
    public bool ExportAsPrefabInstance
    {
        get { return _ExportAsPrefabInstance; }
        set 
        { 
          _ExportAsPrefabInstance = value;
          
          EngineInstancePrefab instance = (EngineInstancePrefab)_engineInstance;
          if (instance != null)
          {
            instance.SetExportInstance(value);          
          }
          if (_ExportAsPrefabInstance) //do not skip parenting if we use a VPrefabInstance 
          {
            this.RemoveHint(HintFlags_e.SkipParenting);
          }
          else
          {
            this.AddHint(HintFlags_e.SkipParenting);
          }
        }
    }

    #endregion

    #region Export

    public override bool OnExportRecursive(SceneExportInfo info)
    {
      if (_ExportAsPrefabInstance)
      {
        //When exporting as PrefabInstance is enabled
        //The VPrefabInstance should be serialized
        if (!OnExport(info))
        {
          return false;
        }
      }
      else
      {
        //otherwise skip this section and don't serialize 
        //anything for the PrefabShape itself, but go through the children
        if(!base.OnExport(info))
        {
          return false;
        }

        // stop here when inheriting the flags and this shape should not be exported
        if (ExportShape || !EditorSettingsBase.InheritShapeFlags)
        {
          ShapeCollection Collection = ChildCollection;
          foreach (ShapeBase shape in Collection)
            if (!shape.OnExportRecursive(info))
              return false;
        }
      }
      return true;
    }

    public override bool OnExport(SceneExportInfo info)
    {
      //Check existence of nested *.vprefab and show message if missing
      String path = EditorManager.Project.MakeAbsolute(_prefabDesc.BinaryFilename);
      if(!File.Exists(path))
      {

        MessageBox.Show("The nested binary prefab" + path + " does not exist yet." +
                        "Please use \"Save binary version\" from the \"Relevant operations\" context menu on the according " +
                        "PrefabInstance in the shape tree.",
                        "Binary Prefab export",
                        MessageBoxButtons.OK,
                        MessageBoxIcon.Warning);
      }
      
      //Export the reference to the nested *.vprefab using the EngineInstancePrefab
      EngineInstancePrefab instance = (EngineInstancePrefab)_engineInstance;
      if (!instance.OnExport(info))
      {
        return false;
      }
      return true;
    }

    #endregion

    #region Hierarchy handling

    static bool CheckForCyclicPrefabs(ShapeCollection children, string filename)
    {
      if (string.IsNullOrEmpty(filename))
        return true;
      foreach (ShapeBase child in children)
      {
        if (child.HasChildren() && !CheckForCyclicPrefabs(child.ChildCollection, filename))
          return false;
        PrefabShape other = child as PrefabShape;
        if (other == null || other.Editable || string.Compare(other.Filename, filename, true) != 0)
          continue;
        EditorManager.ShowMessageBox("Cannot use this as a prefab because a prefab with the same filename\nhas been detected in the hierarchy.", "Cyclic prefabs detected", MessageBoxButtons.OK, MessageBoxIcon.Error);
        return false;
      }
      return true;
    }


    public bool CheckForCyclicPrefabs(string filename)
    {
      PrefabShape parent = (PrefabShape)GetNextParentOfType(typeof(PrefabShape));
      PrefabShape last = this;
      while (parent != null && parent.ShapeVirtualCounter > 0 && parent.Editable)
      {
        last = parent;
        parent = (PrefabShape)parent.GetNextParentOfType(typeof(PrefabShape));
      }
      if (last != null)
        return CheckForCyclicPrefabs(last.ChildCollection, filename);
      return true;
    }
    
    /// <summary>
    /// Can only add children in editable mode
    /// </summary>
    /// <param name="newChild"></param>
    /// <returns></returns>
    public override bool AllowsToAddChild(ShapeBase newChild)
    {
      if (!base.AllowsToAddChild(newChild))
        return false;
      return this.Editable;
    }
    

    #endregion

    #region Prefab Handling

    PrefabDesc _prefabDesc = null;
    bool _bPendingCreation = false;
    bool _bAnyPerInstanceTransformation = false;

    string _pendingPerInstanceParameterString = null;

    CSharpFramework.Math.BoundingBox _mergedLocalBBox = new BoundingBox();
    ShapeCollection _flatShapes3DList = null;
    public ShapeCollection _prefabChildrenList = null; // only children that are relevant for prefabs

    public override bool GetLocalBoundingBox(ref CSharpFramework.Math.BoundingBox bbox)
    {
      if (Editable) // in this case there are too many possible changes to keep track of them
        BuildBoundingBox(ChildCollection);

      if (!_mergedLocalBBox.Valid)
        return base.GetLocalBoundingBox(ref bbox);

      bbox = (BoundingBox)_mergedLocalBBox.Clone();
      return true;
    }

    void BuildBoundingBoxRecursive(ShapeCollection shapes)
    {
      if (shapes == null)
        return;
      foreach (ShapeBase shape in shapes)
      {
        Shape3D shape3D = shape as Shape3D;
        if (shape3D == null)
          continue;

        BoundingBox _temp = shape3D.LocalScaledBoundingBox;
        if (_temp == null || !_temp.Valid)
          continue;

        // this is not correct as we need to multipy the matrices in the hierarchy
        Matrix3F localMat = shape3D.LocalSpaceRotation;
        _mergedLocalBBox.AddOrientedBox(_temp, shape3D.LocalSpacePosition, localMat);
      }
    }

    void BuildBoundingBox(ShapeCollection shapes)
    {
      _mergedLocalBBox.Reset();
      BuildBoundingBoxRecursive(shapes);
    }

    /// <summary>
    /// Recursively flags all children shapes
    /// </summary>
    /// <param name="shape"></param>
    /// <param name="bStatus"></param>
    public void RecursiveFlagShape(ShapeBase shape, bool bStatus)
    {
      if (shape.HasChildren())
        foreach (ShapeBase child in shape.ChildCollection)
          RecursiveFlagShape(child, bStatus);

      int iDiff = this.ShapeVirtualCounter + 1;

      if (bStatus)
      {
        shape.ShapeVirtualCounter += iDiff;
      }
      else
      {
        shape.ShapeVirtualCounter -= iDiff;
      }
      shape.ParentLayer = this.ParentLayerSafe;
      System.Diagnostics.Debug.Assert(shape.ParentLayer != null);
    }

    LocalIDDict _uniqueIDMatch = null;

    protected LocalIDDict IDMatchTable
    {
      get
      {
        if (_uniqueIDMatch==null)
          _uniqueIDMatch = new LocalIDDict();
        return _uniqueIDMatch;
      }
    }

    /// <summary>
    /// Reassign unique IDs under which the shapes were saved
    /// </summary>
    /// <param name="shape"></param>
    protected void RecursiveMatchUniqueID(ShapeBase shape)
    {
      if (shape.HasChildren())
        foreach (ShapeBase child in shape.ChildCollection)
          RecursiveMatchUniqueID(child);

      if (shape.LocalID == 0) // this shape will get a unique ID when assigned to the scene
        return;

      uint iOldID = shape.LocalID;
      if (IDMatchTable.ContainsKey(iOldID))
      {
        shape.LocalID = IDMatchTable[iOldID];
      }
      else
      {
        shape.LocalID = ParentLayer.CreateNewShapeID();
        IDMatchTable.Add(iOldID,shape.LocalID);
      }

      // custom de-serialization object stored for this shape?
      ICustomPrefabSerialization cshape = shape as ICustomPrefabSerialization;
      if (cshape != null)
      {
        object newObject;
        if (IDMatchTable.GetCustomSerializationObject(shape.LocalID, out newObject))
          cshape.SetCustomPrefabSerializationObject(newObject);
      }

    }

    protected void RefreshFromFile()
    {
      if( PrefabManager.RemovePrefab(_fileName) )
          RecreateShapes(false, null);
    }

    void RecursiveCallParentTransformationChanged(ShapeCollection shapes)
    {
      foreach (ShapeBase shape in shapes)
      {
        if (shape is Shape3D)
          ((Shape3D)shape).RecomputeWorldSpaceTransformation(); // the local space position is valid
        if (shape.HasChildren())
          RecursiveCallParentTransformationChanged(shape.ChildCollection);
      }
    }


    void ApplyPerInstanceProperty(DynamicProperty prop)
    {
      ISceneObject targetInst;
      object info = PrefabDesc.FindShapeProperty(this, prop.PropertyType.Name, out targetInst);
      if (info is PropertyInfo)
        ((PropertyInfo)info).SetValue(targetInst, prop.Value, null);
      else if (info is DynamicProperty)
      {
        DynamicProperty p = (DynamicProperty)info;
        p.Value = prop.Value;
        ((DynamicPropertyCollection)targetInst).OnValueChanged(p);
      }
    }

    void ApplyPerInstanceModifications()
    {
      if (_perInstanceParameter == null || _perInstanceParameter.Properties == null)
        return;

      foreach (DynamicProperty prop in _perInstanceParameter.Properties)
        ApplyPerInstanceProperty(prop);
    }



    void CheckForPerInstanceTransformation()
    {
      _bAnyPerInstanceTransformation = false;
      if (_perInstanceParameter == null || _perInstanceParameter.Properties == null)
        return;
      foreach (DynamicProperty prop in _perInstanceParameter.Properties)
        if (prop.PropertyType.GetAttribute(typeof(PrefabTransformationUpdateAttribute), false) != null)
        {
          _bAnyPerInstanceTransformation = true;
          break;
        }
    }

    void RemoveShapesRecursive(ShapeCollection shapes)
    {
      foreach (ShapeBase shape in shapes)
      {
        if (this.ChildCollection.Contains(shape))
          this.RemoveChild(shape);
        shape.RemoveEngineInstance(true); // this recurses
        shape.OnRemoveFromScene(); // this recurses
      }
    }

    /// <summary>
    /// Traverses the graph up until it finds the root prefab. From that it grabs the ID lookup dictionary that corresponds to this nested prefab
    /// </summary>
    /// <param name="parent"></param>
    /// <returns></returns>
    LocalIDDict GetIDMappingFromPrefabRoot(ShapeBase parent)
    {
      if (parent == null)
      {
        System.Diagnostics.Debug.Assert(false, "nested prefab does not have a prefab parent");
        return null;
      }

      if (parent.ShapeVirtualCounter == 0 && parent is PrefabShape)
      {
        LocalIDDictDict table = ((PrefabShape)parent)._nestedPrefabs;
        if (table == null)
          return null;
        string path = this.ShapePath.ToString();
        LocalIDDict dict = null;
        if (!table.TryGetValue(path, out dict))
          return null;
        return dict;
      }
      return GetIDMappingFromPrefabRoot(parent.Parent); // recurse up
    }


    /// <summary>
    /// Rebuilds the sub shapes defined by the prefab with the current filename
    /// </summary>
    /// <param name="bFromLoading">if true, this function does not call the OnAddedToScene functions</param>
    /// <returns></returns>
    bool RecreateShapes(bool bFromLoading, PrefabDesc newPrefab)
    {
      Broken = false;

      // remove the old shapes
      if (_prefabChildrenList != null)
      {
        RemoveShapesRecursive(_prefabChildrenList);
        _prefabChildrenList = null;
      }

      _bEditable = false;

      _flatShapes3DList = null;
      _mergedLocalBBox.Reset();
      _prefabChildrenList = null;

      if (newPrefab == null)
      {
        _prefabDesc = PrefabManager.CreatePrefab(_fileName);
//        _prefabDesc.Load(); // checked below
      }
      else
      {
        _prefabDesc = newPrefab;
        _fileName = newPrefab.Filename;
      }
      if (!_prefabDesc.Loaded)
      {
        Broken = true;
        return false;
      }

      // create instances
      _prefabChildrenList = _prefabDesc.CreateInstances(false,true);
      if (_prefabChildrenList == null)
      {
        Broken = true;
        return false;
      }

      if (ShapeVirtualCounter > 0)
        _uniqueIDMatch = GetIDMappingFromPrefabRoot(this.Parent);

      foreach (ShapeBase shape in _prefabChildrenList)
      {
        RecursiveFlagShape(shape, true);
        RecursiveMatchUniqueID(shape);
        if (!bFromLoading)
          shape.OnAddedToScene();

        shape.AddHint(HintFlags_e.ForceSceneGraphParenting); // the first level of children must handle it this way
        shape.SetParentInternal(this); // pretend this is the parent so AddChild does not recompute world space
        this.AddChild(shape);

        if (!bFromLoading && !shape.HasEngineInstance())
          shape.CreateEngineInstance(true); // create the engine instance
        shape.SetVisible(ParentLayer.Visible, true);
        //shape.OnParentTransformationChanged(); // confirm the current prefab shape position
      }

      // call the OnLink functions
      foreach (ShapeBase shape in _prefabChildrenList)
        shape.ReassignAllLinksRecursive();

      // confirm the current prefab shape position
      RecursiveCallParentTransformationChanged(_prefabChildrenList);

      // backup original relative positions so we can easily operate on all children (e.g. bounding box)
      _flatShapes3DList = _prefabChildrenList.FlatList3D;


      // modifiable parameter
      DynamicPropertyCollectionType type = _prefabDesc.DynamicProperties;
      if (type != null)
      {
        if (_perInstanceParameter != null)
        {
          _perInstanceParameter = (PrefabDesc.PerInstancePropertyCollection)type.CreateMigratedCollection(this._perInstanceParameter);
          ApplyPerInstanceModifications();
        }
        else
          _perInstanceParameter = (PrefabDesc.PerInstancePropertyCollection)type.CreateInstance(this);
      }
      else
        this._perInstanceParameter = null;

      CheckForPerInstanceTransformation();

      return true;
    }


    /// <summary>
    /// Respond to changes of prefabs
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void PrefabDesc_OnPrefabSaved(object sender, EventArgs e)
    {
      PrefabDesc senderPrefab = (PrefabDesc)sender;
      if (string.Compare(senderPrefab.Filename, this.Filename, true) != 0 || Editable)
        return;
      _prefabDesc = null; // enforce that it is set again in the next setter
      this.Prefab = senderPrefab;

    }


    /// <summary>
    /// Callback to propagate per-instance changes to the shapes
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void IScene_PropertyChanged(object sender, PropertyChangedArgs e)
    {
      if (!object.ReferenceEquals(e._component , this._perInstanceParameter))
        return;

      ArrayList propList = _perInstanceParameter.Properties;
      foreach (DynamicProperty dp in propList)
      {
        if (string.Compare(dp.PropertyType.DisplayName, e._propertyName) != 0)
          continue;
        ApplyPerInstanceProperty(dp);
        this.Modified = true;
      }

    }

    #endregion

    #region Relevant Operations

    const string RO_MAKEEDITABLE = "Make editable";
    const string RO_RESAVE = "Resave as prefab";
    const string RO_RESAVECOLLAPSE = "Resave as prefab and collapse";
    const string RO_RESTORECOLLAPSE = "Revert prefab to saved version and collapse";
    const string RO_SAVE_THUMBNAIL = "Save current screenshot as thumbnail";
    const string RO_SAVE_BINARY_VERSION = "Save binary version";
    const string RO_REFRESH = "Refresh shape";

    public override System.Collections.ArrayList RelevantOperations
    {
      get
      {
        ArrayList arr = base.RelevantOperations;
        if (arr == null)
          arr = new ArrayList(4);
        if (Convertible && !Editable && !Broken)
          arr.Add(RO_MAKEEDITABLE);
        if (!Editable && !Broken)
          arr.Add(RO_REFRESH);
        if (Editable && !Broken && HasChildren())
        {
          arr.Add(RO_RESAVE);
          arr.Add(RO_RESAVECOLLAPSE);
          arr.Add(RO_RESTORECOLLAPSE);
        }
        if (!string.IsNullOrEmpty(Filename))
          arr.Add(RO_SAVE_THUMBNAIL);
        if (!Broken && !Editable && PrefabManager.BINARY_SAVER != null)
          arr.Add(RO_SAVE_BINARY_VERSION);
        return arr;
      }
    }


    public override void PerformRelevantOperation(string name, int iShapeIndex, int iShapeCount)
    {
      if (name == RO_MAKEEDITABLE)
      {
        System.Diagnostics.Debug.Assert(Convertible && !Editable && !Broken);
        EditorManager.Actions.Add(new MakePrefabEditableAction(this));

        //Make sure prefab is selected again in ShapeTreeView after this operation [#5016]
        //Therefore fire the OnShapeSelectionChanged event 
        //and make sure to set the events args parameter ForceUpdateScrollPositionInShapeTree to true.
        //Otherwise it won't scroll to the right position in ShapeTreeView (the function OnBeforeSelect is responsible for that)
        ShapeCollection shapes = new ShapeCollection();
        shapes.Add(this);
        EditorManager.OnShapeSelectionChanged(new ShapeSelectionChangedArgs(null, shapes, true));
        
        return;
      }
      if (name == RO_RESAVE)
      {
        System.Diagnostics.Debug.Assert(Editable && !Broken);
        EditorManager.Actions.Add(new ResavePrefabAction(this, true, false));
        return;
      }
      if (name == RO_RESAVECOLLAPSE)
      {
        System.Diagnostics.Debug.Assert(Editable && !Broken);
        EditorManager.Actions.Add(new ResavePrefabAction(this, true, true));
        return;
      }
      if (name == RO_RESTORECOLLAPSE)
      {
        System.Diagnostics.Debug.Assert(Editable && !Broken);
        DialogResult res = EditorManager.ShowMessageBox("Do you really want to revert prefab to saved version and lose all changes?", "Confirm", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
        if (res == DialogResult.Yes)
        {
          EditorManager.Actions.Add(new ResavePrefabAction(this, false, true));
        }
        return;
      }
      if (name == RO_SAVE_THUMBNAIL)
      {
        string absFilename = EditorManager.Project.MakeAbsolute(Filename) + ".Thumbnail.jpg";
        if (!EditorManager.EngineManager.SaveViewThumbnail(absFilename, EditorSettingsBase.DefaultThumbnailResolution, EditorSettingsBase.DefaultThumbnailResolution))
          EditorManager.ShowMessageBox("Saving scene thumbnail failed.", "Error writing file", MessageBoxButtons.OK, MessageBoxIcon.Error);
        return;
      }
      if (name == RO_SAVE_BINARY_VERSION)
      {
        PrefabManager.BINARY_SAVER.SaveToBinaryFile(Prefab);
        return;
      }
      if (name == RO_REFRESH)
      {
        RefreshFromFile();
        return;
      }

      base.PerformRelevantOperation(name, iShapeIndex, iShapeCount);
    }

    #endregion

    #region Convert Editable



    public void SetEditable(bool bStatus, bool bSendMsg)
    {
      if (!Convertible)
        return;

      if (_bEditable == bStatus || _prefabChildrenList==null)
        return;

      _bEditable = bStatus;
      InvalidateIcon();

      foreach (ShapeBase shape in _prefabChildrenList)
      {
        RecursiveFlagShape(shape, !_bEditable);
        shape.SetHint(HintFlags_e.ForceSceneGraphParenting, !_bEditable); // flag the first level of children
      }

      this.EditableInPrefab = bStatus;

      // update tree view
      if (bSendMsg)
        IScene.SendShapeChangedEvent(new ShapeChangedArgs(this, ShapeChangedArgs.Action.ParentChanged));
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

  #region Class : PrefabShapeCreator

  public class PrefabShapeCreator : IShapeCreatorPlugin
  {
    public PrefabShapeCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\lock_ok.png"), System.Drawing.Color.Magenta);
    }

    public override string GetPluginName()
    {
      return "Prefab Instance";
    }

    public override string GetPluginCategory()
    {
      return "Geometry";
    }

    public override ShapeBase CreateShapeInstance()
    {
      PrefabShape shape = new PrefabShape("Prefab");
      shape.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
      return shape;
    }

    public override string GetPluginDescription()
    {
      return "A prefab shape instantiates a .prefab file that has previously been generated from shapes inside vForge.\nWhile not made editable, the content of a prefab shape changes whenever the .prefab file is modified.\nA prefab shape can be made editable to expand the shape graph underneath it.";
    }

  }

  #endregion

  #region Class : MakePrefabEditableAction

  /// <summary>
  /// Action that makes a prefab editable
  /// </summary>
  [Serializable]
  public class MakePrefabEditableAction : IAction
  {

    public MakePrefabEditableAction(PrefabShape prefab)
    {
      _prefab = prefab;
      _rootShapes = prefab._prefabChildrenList;
    }

    PrefabShape _prefab;
    ShapeCollection _rootShapes;

    #region Do / Undo

    public override bool Valid
    {
      get
      {
        if (!base.Valid)
          return false;
        if (!_prefab.Modifiable)
          return false;
        if (_rootShapes == null || _rootShapes.Count == 0)
          return false;
        return true;
      }
    }

    public override void Do()
    {
      _prefab.SetEditable(true, true);
      _prefab.Modified = true;
    }

    public override void Undo()
    {
      _prefab.SetEditable(false, true);
      _prefab.Modified = true;
    }

    #endregion

    #region ISerializable Members

    protected MakePrefabEditableAction(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      _prefab = (PrefabShape)info.GetValue("_prefab", typeof(PrefabShape));
      _rootShapes = (ShapeCollection)info.GetValue("_rootShapes", typeof(ShapeCollection));
    }

    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);
      info.AddValue("_prefab", _prefab);
      info.AddValue("_rootShapes", _rootShapes);
    }

    #endregion

    #region Name / Icon

    public override string ShortName
    {
      get { return "Make prefab '" + _prefab.ShapeName + "' editable"; }
    }

    public override string LongName
    {
      get
      {
        return ShortName;
      }
    }

    /// <summary>
    /// get the icon
    /// </summary>
    static int iImageIcon = EditorManager.GUI.ActionImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\lock_open.png"), System.Drawing.Color.Magenta);

    /// <summary>
    /// Get the layer icon that is also used in the layer list view
    /// </summary>
    public override int IconIndex { get { return iImageIcon; } }


    #endregion
  }

  #endregion

  #region Class : ResavePrefabAction

  /// <summary>
  /// Action that makes a prefab editable
  /// </summary>
  [Serializable]
  public class ResavePrefabAction : IAction
  {

    public ResavePrefabAction(PrefabShape prefabShape, bool bResave, bool bCollapse)
    {
      _prefabShape = prefabShape;
      _bResave = bResave;
      _bResaveOriginal = bResave;
      _bCollapse = bCollapse;
      _bCollapseOriginal = bCollapse;
      _prefab = PrefabManager.FindPrefab(_prefabShape.Filename); // first find alrady loaded prefab
      if (_prefab == null)
        _prefab = new PrefabDesc(_prefabShape.Filename);
    }

    PrefabShape _prefabShape;
    ShapeCollection _rootShapes;
    bool _bResave, _bCollapse;
    
    // This is required for storing the original configuration of the action as Do() will change these
    // and serialization for action recording will then always record the action in a configuration where it 
    // will not do anything when Do() is called
    bool _bResaveOriginal, _bCollapseOriginal;

    PrefabDesc _prefab;
    IAction _removeAction;

    #region Do / Undo

    public override bool Valid
    {
      get
      {
        if (!base.Valid)
          return false;
        if (!_prefabShape.HasChildren())
          return false;
        if ((_bCollapse || _bResave) && !_prefabShape.CheckForCyclicPrefabs(_prefabShape.Filename))
          return false;
        return true;
      }
    }


    public override void Do()
    {
      Vector3F oldRot = _prefabShape.Orientation;
      Vector3F oldScale = _prefabShape.Scaling;
      try
      {
        _rootShapes = _prefabShape.ChildCollection;

        Dictionary<ShapePath, ulong> prefabChildrenPathsWithOldUniqueID = new Dictionary<ShapePath, ulong>();
        

        if (_bCollapse)
        {
          //get shapepath of all children of the prefab before restore or resave the prfeab. 
          //Store it in a dictionary with key = shapepath and value = uniqueID or Shape
          foreach (ShapeBase shape in _rootShapes)
          {
            AddChildRecursiveToPrefabChildrenList(shape, prefabChildrenPathsWithOldUniqueID);
          }
          _prefabShape.TriggerBeforePrefabCollapsed();
        }


        if (_bResave)
        {
          _prefabShape.Scaling = new Vector3F(1, 1, 1);
          _prefabShape.Orientation = Vector3F.Zero; // we need alignment so update local space of children
          _prefab.CreateFromInstances(_rootShapes, _prefabShape.Position, true, true);
          _prefab.LoadHeaderInformationFromFile();
          if (_prefab.SaveToFile(null))
            _bResave = false;   // Only in the first run
          else
            _bCollapse = false; // Do not collapse prefab when saving failed
        }
        else
        {
          if (_prefab != null && !_prefab.Loaded)
            _prefab.Load();
        }

        if (_bCollapse)
        {
          // This is directly created as a remove shapes action because it's used as a part of
          // the resave and collapse prefab action when the tree-node is "collapsed"
          // so this should always have the default behavior
          _removeAction = new RemoveShapesAction(_rootShapes, "Collapse");

          _removeAction.Do();
          System.Diagnostics.Debug.Assert(_prefabShape.ChildCollection.Count == 0);
          _prefabShape.SetPrefabInternal(null); // force setting the prefab
          _prefabShape.Prefab = _prefab;

          _prefabShape.TriggerAfterPrefabCollapsed();
          DelegateUniqueIDInPrefabChange(prefabChildrenPathsWithOldUniqueID);
          
        }
      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex, false);
      }
      _prefabShape.Orientation = oldRot;
      _prefabShape.Scaling = oldScale;
    }

    /// <summary>
    /// This function fires the IScene.SendUniqueIDInPrefabChangedEvent and builds up appropriate argument list
    /// </summary>
    /// <param name="prefabChildrenPathsWithOldUniqueID">Dictionary containing the ShapePath as Key and the old UniqueID as value</param>
    private void DelegateUniqueIDInPrefabChange(Dictionary<ShapePath, ulong> prefabChildrenPathsWithOldUniqueID)
    {
      Dictionary<ShapeBase, ulong> prefabChildrenListWithOldUniqueID = new Dictionary<ShapeBase, ulong>();

      //get the shape of each shapepath after the prefab has been updated
      //also store the old unique id
      foreach (KeyValuePair<ShapePath, ulong> pair in prefabChildrenPathsWithOldUniqueID)
      {
        ShapeBase shape = pair.Key.Shape;
        if (shape != null)
        {
          prefabChildrenListWithOldUniqueID.Add(shape, pair.Value);
        }
      }

      UniqueIDInPrefabChangedArgs args = new UniqueIDInPrefabChangedArgs(prefabChildrenListWithOldUniqueID);
      IScene.SendUniqueIDInPrefabChangedEvent(args);
    }

    /// <summary>
    /// helper function to add all shapePaths of a prefab recursively to a list and store their current uniqueID
    /// </summary>
    /// <param name="shape"></param>
    /// <param name="prefabChildrenPathsWithOldUniqueID"></param>
    private void AddChildRecursiveToPrefabChildrenList(ShapeBase shape, Dictionary<ShapePath, ulong> prefabChildrenPathsWithOldUniqueID)
    {
      ShapePath path = new ShapePath(shape);
      prefabChildrenPathsWithOldUniqueID.Add(path, shape.UniqueID);
      foreach (ShapeBase child in shape.ChildCollection)
      {
        AddChildRecursiveToPrefabChildrenList(child, prefabChildrenPathsWithOldUniqueID);
      }
    }

    public override void Undo()
    {
      try
      {
        if (_bCollapse)
        {
          _prefabShape.SetEditable(true, true);
          _prefabShape.SetPrefabInternal(null);

          // in this mode, restore the old shapes and remove the old shapes
          if (!_bResave)
          {
            IAction removeOld = RemoveShapesAction.CreateRemoveShapesAction(_prefabShape.ChildCollection, "Collapse");
            removeOld.Do();
            if (_removeAction != null)
            {
              _removeAction.Undo();
            }
          }
        }
      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex, false);
      }

    }

    #endregion

    #region ISerializable Members

    protected ResavePrefabAction(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      _prefabShape = (PrefabShape)info.GetValue("_prefabShape", typeof(PrefabShape));
      _rootShapes = (ShapeCollection)info.GetValue("_rootShapes", typeof(ShapeCollection));
      _bResave = info.GetBoolean("_bResave");
      _bCollapse = info.GetBoolean("_bCollapse");
      _prefab = (PrefabDesc)info.GetValue("_prefab", typeof(PrefabDesc));
      _removeAction = (RemoveShapesAction)info.GetValue("_removeAction", typeof(RemoveShapesAction));
    }

    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);
      info.AddValue("_prefabShape", _prefabShape);
      info.AddValue("_rootShapes", _rootShapes);
      info.AddValue("_bResave", _bResaveOriginal);
      info.AddValue("_bCollapse", _bCollapseOriginal);
      info.AddValue("_prefab", _prefab);
      info.AddValue("_removeAction", (RemoveShapesAction)_removeAction);
    }

    #endregion

    #region Name / Icon

    public override string ShortName
    {
      get { return "Resave prefab '" + _prefabShape.ShapeName + "'"; }
    }

    public override string LongName
    {
      get
      {
        return ShortName;
      }
    }

    /// <summary>
    /// get the icon
    /// </summary>
    static int iImageIcon = EditorManager.GUI.ActionImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\lock_OK.png"), System.Drawing.Color.Magenta);

    /// <summary>
    /// Get the layer icon that is also used in the layer list view
    /// </summary>
    public override int IconIndex { get { return iImageIcon; } }


    #endregion
  }

  #endregion

  #region class VisionPrefabBinarySaver


  public class VisionPrefabBinarySaver : IPrefabBinarySaver
  {

    #region IPrefabBinarySaver Members

    public bool SaveToBinaryFile(PrefabDesc prefab)
    {
      if (prefab == null || !prefab.Loaded)
      {
        EditorManager.ShowMessageBox("Invalid prefab", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        return false;
      }

      // create fresh instances since they must be in the origin
      ShapeCollection shapes = prefab.CreateInstances(false, true);
      if (shapes == null)
      {
        EditorManager.ShowMessageBox("Cannot instantiate prefab:\n\n" + prefab.LastError, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
      }

      Layer layer = EditorManager.Scene.ActiveLayer;
      IAction addShapes = AddShapesAction.CreateAddShapesAction(shapes, layer.Root, layer, false, "temp add");
      addShapes.Do(); // note we do not go via undo stack

      SceneExportInfo info = new SceneExportInfo();
      info.ExportType = SceneExportInfo.ExportType_e.VPrefab;
      info.AbsoluteFilename = EditorManager.Project.MakeAbsolute(prefab.BinaryFilename);
      info.RelativeFilename = prefab.BinaryFilename;

      info.RelevantShapes = new ShapeCollection();
      foreach (ShapeBase shape in shapes)
      {
        info.RelevantShapes.AddRange(SceneExportInfo.GetRelevantExportShapes(shape));
      }

      SceneEventArgs sceneEvent = new SceneEventArgs(SceneEventArgs.Action.BeforePrefabBinaryExport, true);
      if (ExportHelper.StartPrefabExport(info))
      {
        foreach (ShapeBase shape in shapes)
          shape.ReassignAllLinksRecursive();
        foreach (ShapeBase shape in shapes)
          EditorManager.CallShapeOnSceneEventRecursive(shape, sceneEvent);
        foreach (ShapeBase shape in shapes)
          shape.OnExportRecursive(info);
        ExportHelper.EndPrefabExport(info);

        sceneEvent.action = SceneEventArgs.Action.AfterPrefabBinaryExport;
        foreach (ShapeBase shape in shapes)
          EditorManager.CallShapeOnSceneEventRecursive(shape, sceneEvent);
      }

      // also write out .vres file
      ResourceSnapshotInfo snapshot = new ResourceSnapshotInfo();
      snapshot.AbsoluteFilename = Path.ChangeExtension(info.AbsoluteFilename, "vres");
      if (ExportHelper.StartResourceSnapshot(snapshot))
      {
        foreach (ShapeBase shape in shapes)
          shape.GetDependenciesRecursive(snapshot);
        ExportHelper.EndResourceSnapshot(snapshot);
      }

      // remove instances again
      addShapes.Undo();
      foreach(ShapeBase shape in shapes)
      {
        shape.Dispose();
      }
      return true;
    }

    #endregion
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
