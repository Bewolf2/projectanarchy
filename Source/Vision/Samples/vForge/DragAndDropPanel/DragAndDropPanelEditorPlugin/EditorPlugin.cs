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
using DragAndDropPanelPlugin.Panels;
using CSharpFramework;

namespace DragAndDropPanelPlugin
{
  public class EditorPlugin : CSharpFramework.IEditorPluginModule
  {
    /// <summary>
    /// Default constructor for the entity panel
    /// </summary>
    public EditorPlugin()
    {
      _name = "DragAndDropPanel";
      _version = 1;
    }

    private DragAndDropPanel _DragAndDropPanel = null;

    public override bool InitPluginModule()
    {
      _DragAndDropPanel = new DragAndDropPanel(EditorManager.ApplicationLayout.DockingArea);
      _DragAndDropPanel.ShowDockable();

      return true;
    }

    public override bool DeInitPluginModule()
    {
      //Deregister the event handlers the panel uses.
      _DragAndDropPanel.DeregisterEventHandler();
      _DragAndDropPanel.Close();
      _DragAndDropPanel.Dispose();
      _DragAndDropPanel = null;
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
