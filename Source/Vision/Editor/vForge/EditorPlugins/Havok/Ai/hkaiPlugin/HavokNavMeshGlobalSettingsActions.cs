/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using CSharpFramework;

namespace HavokAiEditorPlugin.Shapes
{
  #region class NavMeshGlobalSettingsGroupAction 

  // needed just to simplify rtti checks elsewhere
  public class NavMeshGlobalSettingsGroupAction : GroupAction
  {
    public NavMeshGlobalSettingsGroupAction(string name) : base(name) { }
  }

  #endregion

  #region class AddNavMeshGlobalSettingsAction

  public class AddNavMeshGlobalSettingsAction : IAction
  {
    public AddNavMeshGlobalSettingsAction(HavokNavMeshGlobalSettingsDictionary collection, string settingsName, HavokNavMeshGlobalSettings settings)
    {
      m_collection = collection;
      m_settings = settings;
      m_settingsName = settingsName;
    }

    public override void Do()
    {
      m_collection.AddSettingsInstance(m_settingsName, m_settings);

      //ShapeCollection navMeshShapes = EditorManager.Scene.AllShapesOfType(typeof(HavokNavMeshShape));
      //foreach (HavokNavMeshShape navMeshShape in navMeshShapes)
      //{
      //  IScene.SendShapeChangedEvent(new CSharpFramework.Shapes.ShapeChangedArgs(navMeshShape, CSharpFramework.Shapes.ShapeChangedArgs.Action.RefreshProperties));
      //}
    }

    public override void Undo()
    {
      m_collection.DeleteSettingsInstance(m_settingsName);

      ShapeCollection navMeshShapes = EditorManager.Scene.AllShapesOfType(typeof(HavokNavMeshShape));
      foreach (HavokNavMeshShape navMeshShape in navMeshShapes)
      {
        if (navMeshShape.NavMeshGlobalSettingsKey == m_settingsName)
        {
          IScene.SendShapeChangedEvent(new CSharpFramework.Shapes.ShapeChangedArgs(navMeshShape, CSharpFramework.Shapes.ShapeChangedArgs.Action.RefreshProperties));
        }
      }
    }

    public override string ShortName
    {
      get { return "Add Nav Mesh Global Settings"; }
    }

    public override string LongName
    {
      get { return "Create Nav Mesh Global Settings - " + m_settingsName; }
    }

    public override bool Valid
    {
      get
      {
        if ((m_collection != null) // null check
            && (m_settingsName != null) // null check
            && (m_settingsName.Length > 1) // name sanity check
            && (m_collection.GetSettingsInstance(m_settingsName) == null)) // name already exist check
        {
          return true;
        }
        return false;
      }
    }

    HavokNavMeshGlobalSettingsDictionary m_collection;
    HavokNavMeshGlobalSettings m_settings;
    string m_settingsName;
  }

  #endregion

  #region class CreateNavMeshGlobalSettingsAction

  public class CreateNavMeshGlobalSettingsAction : IAction
  {
    public CreateNavMeshGlobalSettingsAction(HavokNavMeshGlobalSettingsDictionary collection, string settingsName)
    {
      m_collection = collection;
      m_settingsName = settingsName;
    }

    public override void Do()
    {
      System.Diagnostics.Debug.Assert(m_collection.GetSettingsInstance(m_settingsName) == null);
      m_collection.CreateAndAddSettingsInstance(m_settingsName);
    }

    public override void Undo()
    {
      System.Diagnostics.Debug.Assert(m_collection.GetSettingsInstance(m_settingsName) != null);
      m_collection.DeleteSettingsInstance(m_settingsName);

      ShapeCollection navMeshShapes = EditorManager.Scene.AllShapesOfType(typeof(HavokNavMeshShape));
      foreach (HavokNavMeshShape navMeshShape in navMeshShapes)
      {
        if (navMeshShape.NavMeshGlobalSettingsKey == m_settingsName)
        {
          IScene.SendShapeChangedEvent(new CSharpFramework.Shapes.ShapeChangedArgs(navMeshShape, CSharpFramework.Shapes.ShapeChangedArgs.Action.RefreshProperties));
        }
      }
    }

    public override string ShortName
    {
      get { return "Create New Nav Mesh Global Settings"; }
    }

    public override string LongName
    {
      get { return "Create Nav Mesh Global Settings - " + m_settingsName; }
    }

    public override bool Valid
    {
      get
      {
        if ((m_collection != null) // null check
            && (m_settingsName != null) // null check
            && (m_settingsName.Length > 1) // name sanity check
            && (m_collection.GetSettingsInstance(m_settingsName) == null)) // name already exist check
        {
          return true;
        }
        return false;
      }
    }

    HavokNavMeshGlobalSettingsDictionary m_collection;
    string m_settingsName;
  }

  #endregion

  #region class DeleteNavMeshGlobalSettingsAction

  public class DeleteNavMeshGlobalSettingsAction : IAction
  {
    public DeleteNavMeshGlobalSettingsAction(HavokNavMeshGlobalSettingsDictionary collection, string settingsName)
    {
      m_collection = collection;
      m_settings = null;
      m_settingsName = settingsName;
    }

    public override void Do()
    {
      // save old settings
      m_settings = m_collection.GetSettingsInstance(m_settingsName);
      System.Diagnostics.Debug.Assert(m_settings != null);
      m_collection.DeleteSettingsInstance(m_settingsName);

      ShapeCollection navMeshShapes = EditorManager.Scene.AllShapesOfType(typeof(HavokNavMeshShape));
      foreach (HavokNavMeshShape navMeshShape in navMeshShapes)
      {
        if (navMeshShape.NavMeshGlobalSettingsKey == m_settingsName)
        {
          IScene.SendShapeChangedEvent(new CSharpFramework.Shapes.ShapeChangedArgs(navMeshShape, CSharpFramework.Shapes.ShapeChangedArgs.Action.RefreshProperties));
        }
      }
    }

    public override void Undo()
    {
      System.Diagnostics.Debug.Assert(m_collection.GetSettingsInstance(m_settingsName) == null);
      m_collection.AddSettingsInstance(m_settingsName, m_settings);

      ShapeCollection navMeshShapes = EditorManager.Scene.AllShapesOfType(typeof(HavokNavMeshShape));
      foreach (HavokNavMeshShape navMeshShape in navMeshShapes)
      {
        if (navMeshShape.NavMeshGlobalSettingsKey == m_settingsName)
        {
          IScene.SendShapeChangedEvent(new CSharpFramework.Shapes.ShapeChangedArgs(navMeshShape, CSharpFramework.Shapes.ShapeChangedArgs.Action.RefreshProperties));
        }
      }
    }

    public override string ShortName
    {
      get { return "Delete Nav Mesh Global Settings"; }
    }

    public override string LongName 
    { 
      get { return "Delete Nav Mesh Global Settings - " + m_settingsName; } 
    }

    public override bool Valid
    {
      get
      {
        if ((m_collection != null)
          && (m_settingsName != null)
            && (m_collection.GetSettingsInstance(m_settingsName) != null))
        {
          return true;
        }
        return false;
      }
    }

    HavokNavMeshGlobalSettingsDictionary m_collection;
    HavokNavMeshGlobalSettings m_settings;
    string m_settingsName;
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
