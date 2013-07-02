/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using TerrainBase.Config;

namespace TerrainBase.Dialogs
{
	public partial class TerrainConfigDlg : Form
	{
		public TerrainConfigDlg()
		{
			InitializeComponent();
		}

		#region Config

		TerrainConfig _config = new TerrainConfig();


		/// <summary>
		/// Gets or sets the config. The set function clones it.
		/// </summary>
		public TerrainConfig Config
		{
			get
			{
				return _config;
			}
			set
			{
				_config = (TerrainConfig)value.Clone();
				propertyGrid_Config.SelectedObject = _config;
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
