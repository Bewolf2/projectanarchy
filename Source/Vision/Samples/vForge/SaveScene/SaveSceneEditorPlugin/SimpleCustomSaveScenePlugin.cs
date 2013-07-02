/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using CSharpFramework;

namespace SimpleCustomSaveScenePlugin
{
  //class name has to be <<EditorPlugin>>
  public class EditorPlugin : IEditorPluginModule
  {

    AdditionalGameData _additionalGameData = new AdditionalGameData();

    public EditorPlugin()
    {
      _version = 1;
      _name = "SimpleCustomSaveScenePlugin";
    }

    public override bool InitPluginModule()
    {
      // Listen to vForge's serialisation event so that we can hook into the serialisation process
      EditorManager.CustomSceneSerialization += new CustomSceneSerializationEventHandler(SerializeMe);

      return true;
    }

    public override bool DeInitPluginModule()
    {
      EditorManager.CustomSceneSerialization -= new CustomSceneSerializationEventHandler(SerializeMe);
      return true;
    }

    /// <summary>
    /// Overridable that determines whether this is a standard plugin that is always loaded and initialized
    /// </summary>
    public override bool IsStandardPlugin
    {
      get
      {
        return false;
      }
    }

    private void SerializeMe(object sender, CustomSceneSerializationArgs args)
    {
      if (args.CustomSceneObjects.IsSaving)
      {
        // Save our settings object in the vForge scene file
        args.CustomSceneObjects.AddObject("SimpleCustomSaveScenePlugin.EditorPlugin", _additionalGameData);
      }
      else
      {
        // Retrieve our settings from the scene file (if available)
        AdditionalGameData loadedData = args.CustomSceneObjects.FindObject(
          "SimpleCustomSaveScenePlugin.EditorPlugin", typeof(AdditionalGameData)) as AdditionalGameData;
        if (loadedData != null)
          _additionalGameData = loadedData;
      }
    }

  }

  /// <summary>
  /// Simple class representing some additional scene info
  /// </summary>
  ///
  [Serializable]
  public class AdditionalGameData
  {

    private int _enemy_count = 0;
    private int _enemy_respawn_time = 10;

    public AdditionalGameData()
    {
    }

    public AdditionalGameData(int defaultEnemyCount, int defaultEnemyRespawnTime)
    {
      this._enemy_count = defaultEnemyCount;
      this._enemy_respawn_time = defaultEnemyRespawnTime;
    }

    public int EnemyCount
    {
      get { return _enemy_count; }
      set { this._enemy_count = value; }
    }

    public int EnemyRespawnTime
    {
      get { return _enemy_respawn_time; }
      set { this._enemy_respawn_time = value; }
    }
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
