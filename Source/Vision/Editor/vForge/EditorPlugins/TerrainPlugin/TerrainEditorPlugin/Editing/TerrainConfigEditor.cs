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
using System.Drawing.Design;
using System.Windows.Forms;
using System.ComponentModel;
using TerrainEditorPlugin.Dialogs;
using TerrainBase.Config;
using CSharpFramework.Shapes;

namespace TerrainEditorPlugin.Editing
{

	/// <summary>
	/// UITypeEditor class that open a terrain config dialog
	/// </summary>
	public class TerrainConfigEditor : UITypeEditor
	{
		public TerrainConfigEditor()
		{
		}

		/// <summary>
		/// Overridden function
		/// </summary>
		/// <param name="context"></param>
		/// <returns></returns>
		[System.Security.Permissions.PermissionSet(System.Security.Permissions.SecurityAction.Demand, Name = "FullTrust")]
		public override UITypeEditorEditStyle GetEditStyle(System.ComponentModel.ITypeDescriptorContext context)
		{
			return UITypeEditorEditStyle.Modal;
		}

		/// <summary>
		/// Overriden Edit value function; opens the modal dialog
		/// </summary>
		/// <param name="context"></param>
		/// <param name="provider"></param>
		/// <param name="value"></param>
		/// <returns></returns>
		[RefreshProperties(RefreshProperties.All)]
		[System.Security.Permissions.PermissionSet(System.Security.Permissions.SecurityAction.Demand, Name = "FullTrust")]
		public override object EditValue(System.ComponentModel.ITypeDescriptorContext context, IServiceProvider provider, object value)
		{
			TerrainConfigDlg dlg = new TerrainConfigDlg();
      dlg.FullConfig = false; // allow editing a subset only

      TerrainConfig origConfig = (TerrainConfig)value;

      dlg.Config = origConfig;
			if (dlg.ShowDialog() != DialogResult.OK)
				return value;
			return dlg.Config;
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
