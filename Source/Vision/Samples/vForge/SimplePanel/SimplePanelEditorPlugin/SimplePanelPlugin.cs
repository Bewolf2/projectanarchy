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


namespace SimplePanelPlugin
{
  //class name has to be <<EditorPlugin>>
  public class EditorPlugin : IEditorPluginModule
  {
    private SimplePanel _panel = null;

    public EditorPlugin()
    {
      _version = 1;
      _name = "SimplePanelPlugin";
    }

    public override bool InitPluginModule()
    {
      _panel = new SimplePanel(EditorManager.ApplicationLayout.DockingArea);

      // Listen to scene changes, so that we can disable the panel when no scene is active
      EditorManager.SceneChanged += new SceneChangedEventHandler(EditorManager_SceneChanged);

      // Activate our panel on first load
      _panel.ShowDockable();
      _panel.Enabled = false;

      return true;
    }

    public override bool DeInitPluginModule()
    {
      EditorManager.SceneChanged -= new SceneChangedEventHandler(EditorManager_SceneChanged);    
      _panel.Close();
      _panel.Dispose();
      _panel = null;
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

    private void EditorManager_SceneChanged(object sender, SceneChangedArgs args)
    {
      // Allow editing only in case a scene is loaded
      _panel.Enabled = (EditorManager.Scene != null);
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
