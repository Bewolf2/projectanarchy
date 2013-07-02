namespace TerrainBase.Dialogs
{
	partial class TerrainConfigDlg
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      this.button_OK = new System.Windows.Forms.Button();
      this.button_Cancel = new System.Windows.Forms.Button();
      this.propertyGrid_Config = new System.Windows.Forms.PropertyGrid();
      this.SuspendLayout();
      // 
      // button_OK
      // 
      this.button_OK.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.button_OK.Location = new System.Drawing.Point(112, 501);
      this.button_OK.Name = "button_OK";
      this.button_OK.Size = new System.Drawing.Size(76, 26);
      this.button_OK.TabIndex = 0;
      this.button_OK.Text = "OK";
      this.button_OK.UseVisualStyleBackColor = true;
      // 
      // button_Cancel
      // 
      this.button_Cancel.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.button_Cancel.Location = new System.Drawing.Point(206, 501);
      this.button_Cancel.Name = "button_Cancel";
      this.button_Cancel.Size = new System.Drawing.Size(76, 26);
      this.button_Cancel.TabIndex = 1;
      this.button_Cancel.Text = "Cancel";
      this.button_Cancel.UseVisualStyleBackColor = true;
      // 
      // propertyGrid_Config
      // 
      this.propertyGrid_Config.Dock = System.Windows.Forms.DockStyle.Top;
      this.propertyGrid_Config.Location = new System.Drawing.Point(0, 0);
      this.propertyGrid_Config.Name = "propertyGrid_Config";
      this.propertyGrid_Config.Size = new System.Drawing.Size(399, 411);
      this.propertyGrid_Config.TabIndex = 2;
      // 
      // TerrainConfigDlg
      // 
      this.AcceptButton = this.button_OK;
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.CancelButton = this.button_Cancel;
      this.ClientSize = new System.Drawing.Size(399, 538);
      this.Controls.Add(this.propertyGrid_Config);
      this.Controls.Add(this.button_Cancel);
      this.Controls.Add(this.button_OK);
      this.Name = "TerrainConfigDlg";
      this.Text = "Terrain Config";
      this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.Button button_OK;
		private System.Windows.Forms.Button button_Cancel;
		private System.Windows.Forms.PropertyGrid propertyGrid_Config;
	}
}