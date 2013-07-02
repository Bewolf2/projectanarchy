/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.IO;
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
	/// Summary description for InteractiveHelpPanel.
	/// </summary>
	public class InteractiveHelpPanel : DockableForm
	{
    private CSharpFramework.Controls.InteractiveHelpControl interactiveHelpControl1;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

    /// <summary>
    /// Constructor
    /// </summary>
		public InteractiveHelpPanel(DockingContainer container) : base(container)
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

      // initialize the iHelp System (but not in the design mode)
      if (!EditorManager.DesignMode)
      {
        interactiveHelpControl1.RootApplicationHelpPath = Path.Combine(EditorManager.AppDataDir, @"help\");
        interactiveHelpControl1.CurrentApplicationHelpPath = Path.Combine(EditorManager.AppDataDir, @"help\");
        interactiveHelpControl1.PageNotFoundUrl = Path.Combine(EditorManager.AppDataDir, @"help\PageNotFound.html");
        interactiveHelpControl1.DefaultErrorUrl = Path.Combine(EditorManager.AppDataDir, @"DefaultError.html");
        interactiveHelpControl1.OpenApplicationHelpPage("index");
        interactiveHelpControl1.ScriptObject = new VForgeScriptableObject();
        interactiveHelpControl1.Reload();
        EditorManager.IHelpSystem = interactiveHelpControl1;
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

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(InteractiveHelpPanel));
      this.interactiveHelpControl1 = new CSharpFramework.Controls.InteractiveHelpControl();
      this.SuspendLayout();
      // 
      // interactiveHelpControl1
      // 
      this.interactiveHelpControl1.ActivePage = null;
      this.interactiveHelpControl1.CurrentApplicationHelpPath = null;
      this.interactiveHelpControl1.CurrentProjectHelpPath = null;
      this.interactiveHelpControl1.DefaultErrorUrl = null;
      this.interactiveHelpControl1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.interactiveHelpControl1.Location = new System.Drawing.Point(0, 0);
      this.interactiveHelpControl1.Name = "interactiveHelpControl1";
      this.interactiveHelpControl1.PageNotFoundUrl = null;
      this.interactiveHelpControl1.RootApplicationHelpPath = null;
      this.interactiveHelpControl1.RootProjectHelpPath = null;
      this.interactiveHelpControl1.Size = new System.Drawing.Size(292, 413);
      this.interactiveHelpControl1.TabIndex = 0;
      // 
      // InteractiveHelpPanel
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(292, 413);
      this.Controls.Add(this.interactiveHelpControl1);
      this.DockableAreas = ((WeifenLuo.WinFormsUI.DockAreas)((((WeifenLuo.WinFormsUI.DockAreas.Float | WeifenLuo.WinFormsUI.DockAreas.DockLeft) 
        | WeifenLuo.WinFormsUI.DockAreas.DockRight) 
        | WeifenLuo.WinFormsUI.DockAreas.Document)));
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.Name = "InteractiveHelpPanel";
      this.ShowHint = WeifenLuo.WinFormsUI.DockState.DockRight;
      this.TabText = "Interactive Help";
      this.Text = "Interactive Help";
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
