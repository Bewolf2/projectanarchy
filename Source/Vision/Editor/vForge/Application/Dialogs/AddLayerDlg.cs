/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//Removed. Use CSharpFramework.Dialogs.InputDlg instead
/*
using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace Editor.Dialogs
{
	/// <summary>
	/// Summary description for AddLayerDlg.
	/// </summary>
	public class AddLayerDlg : System.Windows.Forms.Form
	{
    private System.Windows.Forms.TextBox textBox_LayerName;
    private System.Windows.Forms.Button button_OK;
    private System.Windows.Forms.Button button_Cancel;
    private System.Windows.Forms.Label label1;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public AddLayerDlg()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
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
      this.textBox_LayerName = new System.Windows.Forms.TextBox();
      this.button_OK = new System.Windows.Forms.Button();
      this.button_Cancel = new System.Windows.Forms.Button();
      this.label1 = new System.Windows.Forms.Label();
      this.SuspendLayout();
      // 
      // textBox_LayerName
      // 
      this.textBox_LayerName.Location = new System.Drawing.Point(32, 32);
      this.textBox_LayerName.Name = "textBox_LayerName";
      this.textBox_LayerName.Size = new System.Drawing.Size(224, 20);
      this.textBox_LayerName.TabIndex = 0;
      this.textBox_LayerName.Text = "New Layer";
      this.textBox_LayerName.TextChanged += new System.EventHandler(this.textBox_LayerName_TextChanged);
      // 
      // button_OK
      // 
      this.button_OK.Location = new System.Drawing.Point(64, 64);
      this.button_OK.Name = "button_OK";
      this.button_OK.Size = new System.Drawing.Size(72, 24);
      this.button_OK.TabIndex = 1;
      this.button_OK.Text = "OK";
      this.button_OK.Click += new System.EventHandler(this.button_OK_Click);
      // 
      // button_Cancel
      // 
      this.button_Cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.button_Cancel.Location = new System.Drawing.Point(152, 64);
      this.button_Cancel.Name = "button_Cancel";
      this.button_Cancel.Size = new System.Drawing.Size(72, 24);
      this.button_Cancel.TabIndex = 2;
      this.button_Cancel.Text = "Cancel";
      this.button_Cancel.Click += new System.EventHandler(this.button_Cancel_Click);
      // 
      // label1
      // 
      this.label1.Location = new System.Drawing.Point(8, 8);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(144, 16);
      this.label1.TabIndex = 3;
      this.label1.Text = "Layer name:";
      // 
      // AddLayerDlg
      // 
      this.AcceptButton = this.button_OK;
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.CancelButton = this.button_Cancel;
      this.ClientSize = new System.Drawing.Size(292, 103);
      this.Controls.Add(this.label1);
      this.Controls.Add(this.button_Cancel);
      this.Controls.Add(this.button_OK);
      this.Controls.Add(this.textBox_LayerName);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "AddLayerDlg";
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
      this.Text = "Add Layer";
      this.ResumeLayout(false);

    }

		#endregion

    private void button_OK_Click(object sender, System.EventArgs e)
    {
      DialogResult = DialogResult.OK;
    }

    private void button_Cancel_Click(object sender, System.EventArgs e)
    {
      DialogResult = DialogResult.Cancel;
    }

    private void textBox_LayerName_TextChanged(object sender, System.EventArgs e)
    {
      button_OK.Enabled = LayerName.Length > 0;
    }

    public string LayerName
    {
      get {return textBox_LayerName.Text;}
      set {textBox_LayerName.Text=value;}
    }

  }
}
*/

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
