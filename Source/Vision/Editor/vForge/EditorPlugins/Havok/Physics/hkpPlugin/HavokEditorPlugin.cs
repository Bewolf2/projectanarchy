/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections;
using System.Drawing;
using System.Windows.Forms;
using NUnit.Core;
using CSharpFramework;
using ManagedFramework;
using CSharpFramework.Scene;
using CSharpFramework.Serialization;
using HavokEditorPlugin.Shapes;
using VisionEditorPlugin.Shapes;
using CSharpFramework.Shapes;
using CSharpFramework.Actions;
using CSharpFramework.DynamicProperties;
using CSharpFramework.Math;


namespace HavokEditorPlugin
{

  /// <summary>
  /// Main plugin class
  /// </summary>
  public class EditorPlugin : IEditorPluginModule
  {
    /// <summary>
    /// Constructor of EditorPlugin that sets up plugin information
    /// </summary>
    public EditorPlugin()
    {
      _version = 0;                   //version used for serialization
      _name = "HavokEditorPlugin";
    }

    /// <summary>
    /// This scope holds one global instance of the plugin info. It is returned by all shape's overridden GetPluginInformation function.
    /// This global info is filled inside InitPluginModule
    /// </summary>
    public static EditorPluginInfo EDITOR_PLUGIN_INFO = new EditorPluginInfo();

    #region InitPluginModule : register the creator plugins

    HavokPhysicsPanel _panel = null; 

    /// <summary>
    /// Overridden function that gets called when the plugin is loaded. Registers all creator plugins
    /// </summary>
    /// <returns></returns>
    public override bool InitPluginModule()
    {
      EDITOR_PLUGIN_INFO.NativePluginNames = new string[] { "Havok" };
      HavokManaged.ManagedModule.InitManagedModule();

      StaticMeshShape.UsesCollisionGroups = true; // switch to displaying collision groups
      
      // register shape classes
      creators = new IShapeCreatorPlugin[]
                 {
                   new HavokConstraintShapeCreator(),
                   new HavokConstraintChainShapeCreator()
                 };

      foreach (IShapeCreatorPlugin plugin in creators)
        EditorManager.ShapeCreatorPlugins.Add( plugin );

      // create and activate panel
      _panel = new HavokPhysicsPanel(EditorManager.ApplicationLayout.DockingArea);
      _panel.ShowDockable();

      // Register automated tests
      TestSuiteBuilder testBuilder = new TestSuiteBuilder();
      TestSuite testSuite = testBuilder.Build(typeof(EditorPlugin).Assembly.FullName);
      TestManager.AddTestSuite(testSuite);

      EditorManager.SceneChanged += new SceneChangedEventHandler(EditorManager_SceneChanged);
      IScene.EngineInstancesChanged += new EngineInstancesChangedEventHandler(IScene_EngineInstancesChanged);
      return true;
    }


    #endregion

    #region DeInitPluginModule

    /// <summary>
    /// Overridden function that gets called when the plugin is loaded. Registers all creator plugins
    /// </summary>
    /// <returns></returns>
    public override bool DeInitPluginModule()
    {
      HavokManaged.ManagedModule.DeInitManagedModule();
      foreach (IShapeCreatorPlugin plugin in creators)
        EditorManager.ShapeCreatorPlugins.Remove( plugin );

      EditorManager.SceneChanged -= new SceneChangedEventHandler(EditorManager_SceneChanged);
      IScene.EngineInstancesChanged -= new EngineInstancesChangedEventHandler(IScene_EngineInstancesChanged);

      _panel.Close();
      _panel.Dispose();
      _panel = null;

      return true;
    }

    IShapeCreatorPlugin[] creators;

    #endregion

    #region PhysX to Havok migration helpers

    void AddPhysXComponentsRecursive(ShapeComponentCollection list, ShapeCollection entityList, ShapeBase parent)
    {
      if (parent.ShapeVirtualCounter > 0 || !parent.Modifiable)
        return;
      if (parent.ComponentCount > 0)
      {
        foreach (ShapeComponent comp in parent.Components)
        {
          if (!comp.Missing)
            continue;
          if (comp.DisplayName == "vPhysXRigidBody" || comp.DisplayName == "vPhysXCharacterController")
          {
            // Catch invalid casts 
            try
            {
              UInt32 iValue = System.Convert.ToUInt32(comp.GetPropertyValue("m_iCollisionBitmask", false));
              if (iValue != 0)
                list.Add(comp);
            }
            catch(InvalidCastException)
            {}
          }
        }
      }
      EntityShape entity = parent as EntityShape;
      if (entity != null && (entity.EntityClass == "vPhysXEntity" || entity.EntityClass == "LineFollowerEntity_cl"))
      {
        entityList.Add(entity);
      }

      if (parent.HasChildren())
        foreach (ShapeBase shape in parent.ChildCollection)
          AddPhysXComponentsRecursive(list, entityList, shape);
    }


    /// <summary>
    /// Map properties from the START_VAR_TABLE(vPhysXRigidBody,... to the counterparts in START_VAR_TABLE(vHavokRigidBody...
    /// </summary>
    /// <param name="physX">Input physX rigid body property</param>
    /// <param name="havok">output havok component</param>
    void MigrateRigidBodyProperties(ShapeComponent physX, ShapeComponent havok)
    {
      {
        // Havok: "Dynamic/Keyframed/Fixed/Sphere Inertia/Box Inertia/Thin Box Inertia/Character"
        // PhysX: "Dynamic/Static/Kinematic"
        string sValue = (string)physX.GetPropertyValue("m_ePhysicsType", false); // since it is an enum, we get it as a string
        if (sValue == "Static") sValue = "Fixed";
        else if (sValue == "Kinematic") sValue = "Keyframed";
        havok.SetPropertyValue("Havok_MotionType", sValue, false);
      }

      {
        // Havok: "Box/Sphere/Convex Hull/File/Capsule/Cylinder/Mesh"
        // PhysX: "Box/Sphere/File/Hull/Mesh"
        string sValue = (string)physX.GetPropertyValue("m_ePhysicsGeom", false); // since it is an enum, we get it as a string
        if (sValue == "Hull" || sValue == "File") sValue = "Convex Hull"; // since "PhysXFile" + "Havok_FileResourceName" does not match, convert "File" to "Convex Hull"
        havok.SetPropertyValue("Shape_Type", sValue, false);
      }

      // mass of zero is not legal for dynamic rigid bodies in Havok
      float fMass = (float)physX.GetPropertyValue("m_fMass", false);
      if (fMass > 0.0f)
        havok.SetPropertyValue("Havok_Mass", fMass);

      havok.SetPropertyValue("Shape_Height", physX.GetPropertyValue("m_fHeight", false));
      havok.SetPropertyValue("Shape_Radius", physX.GetPropertyValue("m_fRadius", false));
      havok.SetPropertyValue("Shape_PivotOffset", physX.GetPropertyValue("m_vLocalOffset", false));
      havok.SetPropertyValue("Shape_BoxSize", physX.GetPropertyValue("m_bboxSize", false));
      havok.SetPropertyValue("Debug_Render", physX.GetPropertyValue("DebugRendering", false));
    }

    void MigrateCharacterControllerProperties(ShapeComponent physX, ShapeComponent havok)
    {
      havok.SetPropertyValue("Max_Slope", physX.GetPropertyValue("m_fSlopeLimit", false));
      float fRadius = (float)physX.GetPropertyValue("m_fRadius", false); 
      float fHeight = (float)physX.GetPropertyValue("m_fHeight", false);
      if (fRadius > 0.0f && fHeight > 0.0f)
      {
        havok.SetPropertyValue("Capsule_Radius", fRadius);
        Vector3F top = new Vector3F(0.0f, 0.0f, fHeight + fRadius);
        havok.SetPropertyValue("Character_Top", top);
        Vector3F bottom = new Vector3F(0.0f, 0.0f, fRadius);
        havok.SetPropertyValue("Character_Bottom", bottom);
      }
      havok.SetPropertyValue("Debug", physX.GetPropertyValue("DebugRendering", false));
    }


    void MigratePhysXEntityProperties(DynamicPropertyCollection entityProps, ShapeComponent havok)
    {
      {
        // Havok: "Dynamic/Keyframed/Fixed/Sphere Inertia/Box Inertia/Thin Box Inertia/Character"
        // PhysX: "Dynamic/Static/Kinematic"
        string sValue = (string)entityProps.GetPropertyValue("m_ePhysicsType", false); // since it is an enum, we get it as a string
        if (sValue == "Static") sValue = "Fixed";
        else if (sValue == "Kinematic") sValue = "Keyframed";
        havok.SetPropertyValue("Havok_MotionType", sValue, false);
      }

      {
        // Havok: "Box/Sphere/Convex Hull/File/Capsule/Cylinder/Mesh"
        // PhysX: "Box/Sphere/File/Hull/Mesh"
        string sValue = (string)entityProps.GetPropertyValue("m_ePhysicsGeom", false); // since it is an enum, we get it as a string
        if (sValue == "Hull" || sValue == "File") sValue = "Convex Hull"; // since PhysXFile + Havok_FileResourceName does not match, convert "File" to "Convex Hull"
        havok.SetPropertyValue("Shape_Type", sValue, false);
      }

      // mass of zero is not legal for dynamic rigid bodies in Havok. Furthermore, very old versions of the entity class
      // did not have a mass property, so additionally check whether the property is there
      object fValue = entityProps.GetPropertyValue("m_fMass", false);
      if ((fValue is float) && ((float)fValue) > 0.0f)
        havok.SetPropertyValue("Havok_Mass", fValue);

      havok.SetPropertyValue("Shape_Radius", entityProps.GetPropertyValue("m_fRadius", false));
      havok.SetPropertyValue("Shape_BoxSize", entityProps.GetPropertyValue("m_bboxSize", false));
      havok.SetPropertyValue("Shape_PivotOffset", entityProps.GetPropertyValue("m_bLocalOffset", false));
      havok.SetPropertyValue("Debug_Render", entityProps.GetPropertyValue("DebugRendering", false));
    }

    void MigrateLineFollowerEntityProperties(DynamicPropertyCollection entityProps, ShapeComponent component)
    {
      component.SetPropertyValue("Model_AnimationName", entityProps.GetPropertyValue("AnimationName", false), false);
      component.SetPropertyValue("Path_Key", entityProps.GetPropertyValue("PathKey", false), false);
      component.SetPropertyValue("Path_NumberSteps", entityProps.GetPropertyValue("PathSteps", false), false);
      component.SetPropertyValue("Path_TriggerDistance", entityProps.GetPropertyValue("TriggerDistance", false), false);
      component.SetPropertyValue("Path_InitialOffset", entityProps.GetPropertyValue("InitialOffset", false), false);
      component.SetPropertyValue("Model_DeltaRotation", entityProps.GetPropertyValue("DeltaRotation", false), false);
      component.SetPropertyValue("Model_GroundOffset", entityProps.GetPropertyValue("ModelOffset", false), false);
      component.SetPropertyValue("Model_CapsuleHeight", entityProps.GetPropertyValue("CapsuleHeight", false), false);
      component.SetPropertyValue("Model_CapsuleRadius", entityProps.GetPropertyValue("CapsuleRadius", false), false);
      component.SetPropertyValue("Debug_DisplayBoxes", entityProps.GetPropertyValue("DisplayBoxes", false), false);
      component.SetPropertyValue("Debug_RenderMesh", entityProps.GetPropertyValue("DebugRendering", false), false);
    }
    

    /// <summary>
    /// This function converts all physX related entities/components to new Havok plugin
    /// </summary>
    void MigrateToHavokComponents()
    {
      // PhysX plugin is still loaded? Then don't do anything
      IEditorPluginModule physXPlugin = EditorManager.GetPluginByName("PhysXEditorPlugin.EditorPlugin");
      if (physXPlugin != null && physXPlugin.Initialized)
        return;

      // collect all physX specific components
      ShapeComponentCollection physXComponents = new ShapeComponentCollection();
      ShapeCollection physXEntities = new ShapeCollection();
      foreach (Layer layer in EditorManager.Scene.Layers)
        if (layer.Modifiable && layer.Loaded)
          AddPhysXComponentsRecursive(physXComponents, physXEntities, layer.Root);

      if (physXComponents.Count == 0 && physXEntities.Count == 0)
        return;

      // prompt a dialog
      DialogResult res = EditorManager.ShowMessageBox("nVidia PhysX binding specific components/entities have been found in loaded layers.\n\nShould these be permanently converted to Havok components?", "nVidia PhysX plugin not loaded", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
      if (res != DialogResult.Yes)
        return;

      int iConvertedCount = 0;

      // build a list of actions for the component replacement
      if (physXComponents.Count > 0)
      {
        GroupAction actions = new GroupAction("Migrate to Havok components");
        foreach (ShapeComponent oldComp in physXComponents)
        {

          ShapeComponent newComp = null;
          if (oldComp.DisplayName == "vPhysXRigidBody")
          {
            newComp = (ShapeComponent)EditorManager.EngineManager.ComponentClassManager.CreateCollection(null, "vHavokRigidBody");
            MigrateRigidBodyProperties(oldComp, newComp);
          }
          else if (oldComp.DisplayName == "vPhysXCharacterController")
          {
            newComp = (ShapeComponent)EditorManager.EngineManager.ComponentClassManager.CreateCollection(null, "vHavokCharacterController");
            MigrateCharacterControllerProperties(oldComp, newComp);
          }
          if (newComp == null)
            continue;
          // action to remove the old component and add the new one
          actions.Add(new RemoveShapeComponentAction((ShapeBase)oldComp.Owner, oldComp));
          actions.Add(new AddShapeComponentAction((ShapeBase)oldComp.Owner, newComp));
          iConvertedCount++;
        }
        // trigger the conversion action
        EditorManager.Actions.Add(actions);
      }

      if (physXEntities.Count > 0)
      {
        GroupAction actions = new GroupAction("Migrate entities classes");
        foreach (EntityShape entity in physXEntities)
        {
          ShapeComponent newComp = null;
          if (entity.EntityClass == "vPhysXEntity")
          {
            newComp = (ShapeComponent)EditorManager.EngineManager.ComponentClassManager.CreateCollection(null, "vHavokRigidBody");
            MigratePhysXEntityProperties(entity.EntityProperties, newComp);
          }
          else if (entity.EntityClass == "LineFollowerEntity_cl")
          {
            newComp = (ShapeComponent)EditorManager.EngineManager.ComponentClassManager.CreateCollection(null, "VLineFollowerComponent");
            MigrateLineFollowerEntityProperties(entity.EntityProperties, newComp);
          }
          if (newComp == null) 
            continue;
          // we convert to base entity class and attach a component instead
          actions.Add(SetPropertyAction.CreateSetPropertyAction(entity, "EntityClass", "VisBaseEntity_cl"));
          actions.Add(new AddShapeComponentAction(entity, newComp));
          iConvertedCount++;
        }
        // trigger the conversion action
        EditorManager.Actions.Add(actions);
    }

      EditorManager.ShowMessageBox(iConvertedCount.ToString()+" Component(s) have been successfully converted.\n\nSince physics engine have quite different behavior, the parameters might have to be tweaked to match old behavior.", "PhysX to Havok component conversion", MessageBoxButtons.OK, MessageBoxIcon.Information);
    }

    bool bNewScene = true;

    void EditorManager_SceneChanged(object sender, SceneChangedArgs e)
    {
      // don't convert here as engine instances are not there yet
      bNewScene = true;
    }

    void IScene_EngineInstancesChanged(object sender, EngineInstancesChangedArgs e)
    {
      if (e.action == EngineInstancesChangedArgs.Action.PostCreateAll && bNewScene)
      {
        bNewScene = false;
        try
        {
          MigrateToHavokComponents();
        }
        catch (Exception ex)
        {
          EditorManager.DumpException(ex, true);
        }
      }
    }

    #endregion



  }


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
