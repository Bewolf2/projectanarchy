/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using CSharpFramework;
using CSharpFramework.Docking;
using WeifenLuo.WinFormsUI;

namespace Editor
{
	/// <summary>
  /// Panel that displays the UNDO/REDO history
	/// </summary>
	public class UndoHistoryPanel : DockableForm
	{
    private CSharpFramework.ActionHistoryView actionHistoryView;
		/// <summary>
		/// Required designer variable.
		/// </summary>
    private System.ComponentModel.Container components = null;

    /// <summary>
    /// Constructor
    /// </summary>
		public UndoHistoryPanel(DockingContainer container) : base(container)
		{
			//
			// Required for Windows Form Designer support
			//
      InitializeComponent();

      // Check if in design mode. visual studio 2010 can crash when you try to 
      // call any static functions or members from EditorManager when in design mode.
      if (System.ComponentModel.LicenseManager.UsageMode != System.ComponentModel.LicenseUsageMode.Designtime)
      {
        this.actionHistoryView.SetStack(EditorManager.Actions);
      }
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

    private void actionHistoryView_KeyDown(object sender, KeyEventArgs e)
    {
      EditorManager.EditorComponentKeyDown(null, e);
    }

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(UndoHistoryPanel));
      this.actionHistoryView = new CSharpFramework.ActionHistoryView();
      this.SuspendLayout();
      // 
      // actionHistoryView
      // 
      this.actionHistoryView.Dock = System.Windows.Forms.DockStyle.Fill;
      this.actionHistoryView.Location = new System.Drawing.Point(0, 0);
      this.actionHistoryView.Name = "actionHistoryView";
      this.actionHistoryView.Size = new System.Drawing.Size(292, 389);
      this.actionHistoryView.TabIndex = 0;
      this.actionHistoryView.KeyDown += new System.Windows.Forms.KeyEventHandler(this.actionHistoryView_KeyDown);
      // 
      // UndoHistoryPanel
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(292, 389);
      this.Controls.Add(this.actionHistoryView);
      this.DockableAreas = ((WeifenLuo.WinFormsUI.DockAreas)(((WeifenLuo.WinFormsUI.DockAreas.Float | WeifenLuo.WinFormsUI.DockAreas.DockLeft)
                  | WeifenLuo.WinFormsUI.DockAreas.DockRight)));
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.Name = "UndoHistoryPanel";
      this.TabText = "Undo Redo History";
      this.Text = "Undo Redo History";
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
