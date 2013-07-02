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
using System.Collections;
using System.Collections.Specialized;
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
using CSharpFramework.Serialization;
using VisionManaged;
using System.Windows.Forms;

namespace VisionEditorPlugin.Shapes
{
  #region Class : PostProcessingShape

	/// <summary>
	/// Shape for post processor.
	/// </summary>
  [Serializable, ShapeTypeNiceName("PostProcessing")]
  public class PostProcessingShape : ShapeBase
	{
    #region Nested Class : PostProcessingClassConverter
    /// <summary>
    /// Class that provides the dynamic dropdown list for the classes
    /// </summary>
    public class PostProcessingClassConverter : StringConverter
    {
      /// <summary>
      /// Overridden StringConverter function: We provide standard values
      /// </summary>
      /// <param name="context">The descriptor context</param>
      /// <returns>In our case : true</returns>
      public override bool GetStandardValuesSupported(ITypeDescriptorContext context) 
      {
        return true;
      }

      /// <summary>
      /// Overridden StringConverter function: Returns the standard values that the dropdown should have
      /// </summary>
      /// <param name="context">The descriptor context</param>
      /// <returns>Collection of standard values</returns>
      public override StandardValuesCollection GetStandardValues(ITypeDescriptorContext context) 
      {
        StringCollection classNames = EditorManager.EngineManager.TypeManager_GetClassNamesDerivedFrom("VPostProcessingTemplateBase");
        classNames.RemoveAt(0); // remove the first class (not instancable base class)
        return new StandardValuesCollection( classNames );
      }

      /// <summary>
      /// Overridden StringConverter function: 
      /// We also allow non-standard values (i.e. when an entity class is missing/unknown)
      /// </summary>
      /// <param name="context">The descriptor context</param>
      /// <returns>In our case : false</returns>
      public override bool GetStandardValuesExclusive(ITypeDescriptorContext context) 
      {
        return false;
      }
    }

    #endregion

    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_POSTPROCESSING = "PostProcessing";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_POSTPROCESSING = Shape3D.LAST_CATEGORY_ORDER_ID + 1;

    /// <summary>
    /// last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_POSTPROCESSING;

    #endregion

    #region Constructor

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="name"></param>
		public PostProcessingShape(string name) : base(name)
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
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_postprocess.bmp"), Color.Magenta);
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
      this._engineInstance = new EngineInstancePostProcessingEffect();
      IScene.PropertyChanged += new CSharpFramework.PropertyChangedEventHandler(IScene_PropertyChanged);
      SetEngineInstanceBaseProperties(); // sets the position etc.
    }

    /// <summary>
    /// Overridden function to remove the engine instance
    /// </summary>
    /// <param name="bRemoveChildren"></param>
    public override void RemoveEngineInstance(bool bRemoveChildren)
    {
      base.RemoveEngineInstance (bRemoveChildren);
      IScene.PropertyChanged -= new CSharpFramework.PropertyChangedEventHandler(IScene_PropertyChanged);
    }

    /// <summary>
    /// helper function to access the engine instance as EngineInstancePostProcessingEffect
    /// </summary>
    [BrowsableAttribute(false)]
    public EngineInstancePostProcessingEffect EnginePostProcessing
    {
      get {return (EngineInstancePostProcessingEffect)_engineInstance;}
    }

    /// <summary>
    /// Overridden function to set all shape's properties (color, ...) on the engine instance
    /// </summary>
    public override void SetEngineInstanceBaseProperties()
    {
      base.SetEngineInstanceBaseProperties ();
      if (_engineInstance==null)
        return;
      EnginePostProcessing.SetClassName(_effectClassName);
      EnginePostProcessing.SetAllVariables(EffectProperties);
      EnginePostProcessing.SetKey(_key);

      if (Active)
        EnginePostProcessing.Activate();
    }

    #endregion

    #region Clone

    /// <summary>
    /// Overridden clone function that removes the hotspots again
    /// </summary>
    /// <returns></returns>
    public override ShapeBase Clone()
    {
      PostProcessingShape copy = base.Clone() as PostProcessingShape;

      // perform deep copy of the dynamic properties
      if (_effectProperties != null)
        copy._effectProperties = (DynamicPropertyCollection)_effectProperties.Clone();
      
      return copy;
    }


    #endregion

    #region ISerializable

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected PostProcessingShape(SerializationInfo info, StreamingContext context) : base(info, context)
    {
      _effectClassName = info.GetString("_effectClassName");

      // cache the class properties
      ClassProperties = EngineInstancePostProcessingEffect.GetClassPropertyType(PropertyManager, _effectClassName);

      bool _bActive = info.GetBoolean("_bActive");
      _effectProperties = (DynamicPropertyCollection)info.GetValue( "_effectProperties", typeof(DynamicPropertyCollection) );

      if (SerializationHelper.HasElement(info,"_key"))
        _key = info.GetString("_key");

      if (_bActive)
        Activate();
    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context)
    {
      base.GetObjectData (info, context);
      info.AddValue("_effectClassName",_effectClassName);
      info.AddValue("_bActive",Active);
      info.AddValue("_effectProperties",_effectProperties);
      info.AddValue("_key",_key);
    }


    /// <summary>
    /// finalize the effect properties
    /// </summary>
    public override void OnDeserialization()
    {
      // migrate the properties to get latest attributes etc.
      if (_effectProperties!=null)  //FIX #14249: There are no properties if no EffectClass has been selected
        _effectProperties = PropertyManager.CreateMigratedCollection(_effectProperties);
      base.OnDeserialization ();
    }

    #endregion

    #region Relevant Operations

    /// <summary>
    /// Overridden function
    /// </summary>
    public override ArrayList RelevantOperations
    {
      get
      {
        ArrayList arr = base.RelevantOperations;
        if (arr==null) 
          arr = new ArrayList(4);
        arr.Add("Activate Postprocessing");
        arr.Add(ShapeObject3D.RELEVANTOP_SHAPENAME_AS_KEY);
        return arr;
      }
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    public override void PerformRelevantOperation(string name, int iShapeIndex, int iShapeCount)
    {
      if (name=="Activate Postprocessing")
      {
        Activate();
        return;
      }
      if (name == ShapeObject3D.RELEVANTOP_SHAPENAME_AS_KEY)
      {
        // start a group
        if (iShapeIndex == 0) EditorManager.Actions.StartGroup("Name as Key");

        EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this, "Key", ShapeName));

        // close group action
        if (iShapeIndex == iShapeCount - 1) EditorManager.Actions.EndGroup();
        return;
      }

      base.PerformRelevantOperation (name, iShapeIndex, iShapeCount);
    }

    #endregion

    #region Activating

    static PostProcessingShape ActivatedPostProcessingEffect = null;

    /// <summary>
    /// Activate this postprocessing effect
    /// </summary>
    public void Activate()
    {
      ActivatedPostProcessingEffect = this;
      if (EnginePostProcessing!=null)
        EnginePostProcessing.Activate();
    }

    /// <summary>
    /// Deactivate this postprocessing effect
    /// </summary>
    public void DeActivate()
    {
      if (ActivatedPostProcessingEffect==this)
        ActivatedPostProcessingEffect = null;
      if (EnginePostProcessing!=null)
        EnginePostProcessing.Deactivate();

    }


    /// <summary>
    /// overridden function
    /// </summary>
    public override void OnAddedToScene()
    {
      base.OnAddedToScene ();
      Renderer.OnRendererTypeChanging += new CancelEventHandler(Renderer_OnRendererTypeChanging); // intercept with this change
    }

    void Renderer_OnRendererTypeChanging(object sender, CancelEventArgs e)
    {
      Renderer.CancelRendererTypeChangedEventArgs arg = (Renderer.CancelRendererTypeChangedEventArgs)e;
      if (arg.wantedType == Renderer.RendererType_e.DeferredRenderer && Active)
      {
        Active = false;
        EditorManager.ShowMessageBox("Cannot use deferred shading with a postprocessing effect.\nPostprocessing effect is deactivated", "Conflicting post processing effects", MessageBoxButtons.OK, MessageBoxIcon.Information);
      }

    }

    /// <summary>
    /// overridden function
    /// </summary>
    public override void OnRemoveFromScene()
    {
      Renderer.OnRendererTypeChanging -= new CancelEventHandler(Renderer_OnRendererTypeChanging);
      DeActivate();
      base.OnRemoveFromScene ();
    }

    #endregion

    #region Shape Properties

    string _effectClassName = null;
    string _key = null;  

    /// <summary>
    /// Effect template name
    /// </summary>
    [SortedCategory(CAT_POSTPROCESSING, CATORDER_POSTPROCESSING),PropertyOrder(1),
    DependentProperties(new string[] {"EffectProperties"}),
    TypeConverter(typeof(PostProcessingClassConverter)),  
    Description("Class name of the postprocessor"),
    RefreshProperties(System.ComponentModel.RefreshProperties.All) ]  //Causes a grid refresh after the class changes because we change the entity properties
    public string EffectClass
    {
      get {return _effectClassName;}
      set
      {
        if (_effectClassName==value)
          return;
        _effectClassName = value;
        ClassProperties = EngineInstancePostProcessingEffect.GetClassPropertyType(PropertyManager, _effectClassName);
        SetEngineInstanceBaseProperties();
      }
    }

    /// <summary>
    /// Effect key
    /// </summary>
    [SortedCategory(CAT_POSTPROCESSING, CATORDER_POSTPROCESSING),PropertyOrder(2),
    Description("Arbitrary key string that can be used in the engine to find the effect")]
    public string Key
    {
      get {return _key;}
      set
      {
        if (_key==value)
          return;
        _key = value;
        EnginePostProcessing.SetKey(_key);
      }
    }


    /// <summary>
    /// Pseudo property that activates the effect
    /// </summary>
    [SortedCategory(CAT_POSTPROCESSING, CATORDER_POSTPROCESSING),PropertyOrder(3)]
    [Description("Activates or deactivates the postprocessing effect. There is at most one effect active at a time.")]
    public bool Active
    {
      get
      {
        return ActivatedPostProcessingEffect==this;
      }
      set
      {
        bool bNewVal = value;
        /*
        if (bNewVal && EditorManager.Scene.RendererType == Renderer.RendererType_e.DeferredRenderer)
        {
          EditorManager.ShowMessageBox("Cannot use deferred shading with a postprocessing effect.\nPostprocessing effect is deactivated", "Conflicting post processing effects", MessageBoxButtons.OK, MessageBoxIcon.Information);
          bNewVal = false;
        }
        */
        if (bNewVal)
          Activate();
        else
          DeActivate();
        EditorManager.ActiveView.UpdateView(false);
      }
    }


    static DynamicPropertyManager g_PropertyManager = null;
    DynamicPropertyCollectionType _classProperties = null;
    DynamicPropertyCollection _effectProperties = null;

    /// <summary>
    /// The property manager that keeps the property type classes
    /// </summary>
    public static DynamicPropertyManager PropertyManager
    {
      get 
      {
        if (g_PropertyManager==null)
          g_PropertyManager = new DynamicPropertyManager();
        return g_PropertyManager;
      }
      set 
      {
        g_PropertyManager = value;
      }
    }


    [Browsable(false)]
    DynamicPropertyCollectionType ClassProperties
    {
      get {return _classProperties;}
      set
      {
        if (_classProperties == value)
          return;
        _classProperties = value;

        if (_classProperties!=null)
          EffectProperties = _classProperties.CreateInstance(this);
        else
          EffectProperties = null;

      }
    }


    /// <summary>
    /// Effect specific property opject
    /// </summary>
    [PrefabRelevant(false)]
    [SortedCategory(CAT_POSTPROCESSING, CATORDER_POSTPROCESSING),PropertyOrder(4)]
    [TypeConverter(typeof(DynamicPropertyConverter))]
    [Description("Effect specific properties.")]
    public DynamicPropertyCollection EffectProperties
    {
      get
      {
        return _effectProperties;
      }
      set
      {
        if (_effectProperties == value)
          return;
        _effectProperties = value;
        if (_engineInstance!= null)
          EnginePostProcessing.SetAllVariables(_effectProperties);
      }

    }

    /// <summary>
    /// Returns the value of a property in the engine format (string representation)
    /// </summary>
    /// <param name="propertyName">Name of the entity property</param>
    /// <returns>String value of the property in the engine format</returns>
    protected string GetPropertyValueAsString(string propertyName)
    {
      object propertyValue = _effectProperties.GetPropertyValue(propertyName);
      return SerializationHelper.GetEngineStringFromObject(propertyValue);
    }


    private void IScene_PropertyChanged(object sender, PropertyChangedArgs e)
    {
      if ( object.ReferenceEquals(e._component,_effectProperties ))
      {
        // check whether an entity property has been changed by the user
        // and update the engine entity variable accordingly
        if ( _engineInstance != null && ParentLayer.Modifiable)
        {
          string varValue = GetPropertyValueAsString(e._propertyName);
          EnginePostProcessing.SetVariable(e._propertyName, varValue);
          ParentLayer.Modified = true;
        }
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

  #region PostProcessingShape Creator Plugin

  class PostProcessingShapeCreator : CSharpFramework.IShapeCreatorPlugin
  {
    public PostProcessingShapeCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_postprocess.bmp"), Color.Magenta);
    }

    public override string GetPluginName()
    {
      return "Postprocessing Effect";
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
      return "Adds a post processing effect to the scene.";
    }

    public override ShapeBase CreateShapeInstance()
    {
      PostProcessingShape shape = new PostProcessingShape("Postprocessing");
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
