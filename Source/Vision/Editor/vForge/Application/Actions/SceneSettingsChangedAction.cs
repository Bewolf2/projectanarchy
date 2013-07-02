/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using CSharpFramework;
using CSharpFramework.Actions;
using CSharpFramework.Scene;
using System.Runtime.Serialization;

namespace Editor.Actions
{
	/// <summary>
	/// Action that replaces the scene specific settings
	/// </summary>
  [Serializable]
	public class SceneSettingsChangedAction : IAction
	{
    /// <summary>
    /// Action Constructor.
    /// </summary>
    /// <param name="oldSettings">old settings in the scene</param>
    /// <param name="newSettings">new settings to be set on the scene</param>
		public SceneSettingsChangedAction(EditorSceneSettings oldSettings, EditorSceneSettings newSettings)
		{
      _oldSettings = oldSettings;
      _newSettings = newSettings;
		}
    
    #region IAction

    EditorSceneSettings _oldSettings;
    EditorSceneSettings _newSettings;

    /// <summary>
    /// Overridden function
    /// </summary>
    public override void Do()
    {
      EditorApp.Scene.Settings = _newSettings;
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    public override void Undo()
    {
      EditorApp.Scene.Settings = _oldSettings;
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    public override string ShortName
    {
      get
      {
        return "Change scene settings";
      }
    }

    #endregion

    #region ISerializable Members

    protected SceneSettingsChangedAction(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      _oldSettings = (EditorSceneSettings)info.GetValue("_oldSettings", typeof(EditorSceneSettings));
      _newSettings = (EditorSceneSettings)info.GetValue("_newSettings", typeof(EditorSceneSettings));
    }

    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);
      info.AddValue("_oldSettings", _oldSettings);
      info.AddValue("_newSettings", _newSettings);
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
