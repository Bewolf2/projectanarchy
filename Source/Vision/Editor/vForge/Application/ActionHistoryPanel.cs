/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace Editor
{
	/// <summary>
  /// Panel that displays the UNDO/REDO history (DEPRECATED?) See UndoHistoryPanel
	/// </summary>
	public class ActionHistoryPanel : System.Windows.Forms.UserControl
	{
    private CSharpFramework.ActionHistoryView actionHistoryView;
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

    /// <summary>
    /// Constructor
    /// </summary>
		public ActionHistoryPanel()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

			// TODO: Add any initialization after the InitializeComponent call

		}

		/// <summary> 
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      this.actionHistoryView = new CSharpFramework.ActionHistoryView();
      this.SuspendLayout();
      // 
      // actionHistoryView
      // 
      this.actionHistoryView.ImageIndex = -1;
      this.actionHistoryView.Location = new System.Drawing.Point(8, 16);
      this.actionHistoryView.Name = "actionHistoryView";
      this.actionHistoryView.SelectedImageIndex = -1;
      this.actionHistoryView.Size = new System.Drawing.Size(248, 256);
      this.actionHistoryView.TabIndex = 0;
      // 
      // ActionHistoryPanel
      // 
      this.Controls.Add(this.actionHistoryView);
      this.Name = "ActionHistoryPanel";
      this.Size = new System.Drawing.Size(272, 288);
      this.ResumeLayout(false);

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
