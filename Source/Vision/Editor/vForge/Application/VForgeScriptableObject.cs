/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.IO;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using CSharpFramework;
using CSharpFramework.Shapes;
using CSharpFramework.Helper;
using CSharpFramework.Dialogs;
using CSharpFramework.Help;
using System.Reflection;
using System.Drawing.Design;
using System.Windows.Forms.PropertyGridInternal;

namespace Editor
{
	/// <summary>
	/// Summary description for VForgeScriptableObject.
	/// </summary>
	public class VForgeScriptableObject
	{
    /// <summary>
    /// Constructor.
    /// </summary>
    public VForgeScriptableObject()
		{
			//
			// TODO: Add constructor logic here
			//
		}

    /// <summary>
    /// Get tool plugin
    /// </summary>
    public IMenuItemPlugin GetMenuItemPlugin(string toolName)
    {
      return EditorManager.GetMenuItemPluginByName(toolName);
    }

    #region Properties
    /// <summary>
    /// The Scene of the Editor Manager
    /// </summary>
    public IScene Scene
    {
      get 
      {
        return EditorManager.Scene; 
      }
    }

    /// <summary>
    /// The IHelp System of the EditorManager
    /// </summary>
    public IiHelp IHelpSystem
    {
      get
      {
        return EditorManager.IHelpSystem;
      }
    }


    /// <summary>
    /// The play mode of the EditorManager
    /// </summary>
    public EditorManager.Mode PlayMode
    {
      get
      {
        return EditorManager.EditorMode;
      }
      set
      {
        EditorManager.EditorMode = value;
      }
    }

    /// <summary>
    /// The selected shapes of the EditorManager
    /// </summary>
    public ShapeCollection SelectedShapes
    {
      get
      {
        return EditorManager.SelectedShapes;
      }
      set
      {
        EditorManager.SelectedShapes = value;
      }
    }

    /// <summary>
    /// The first shape in the selected shapes of the EditorManager
    /// </summary>
    public ShapeBase SelectedShape
    {
      get
      {
        return EditorManager.SelectedShapes[0];
      }
      set
      {
        ShapeCollection newShapes = new ShapeCollection();
        newShapes.Add(value);
        EditorManager.SelectedShapes = newShapes;
      }
    }

    #endregion Properties
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
