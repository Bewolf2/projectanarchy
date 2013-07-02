namespace TerrainEditorPlugin.Dialogs
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(TerrainConfigDlg));
      this.button_OK = new System.Windows.Forms.Button();
      this.button_Cancel = new System.Windows.Forms.Button();
      this.panel1 = new System.Windows.Forms.Panel();
      this.panel2 = new System.Windows.Forms.Panel();
      this.tabControl1 = new System.Windows.Forms.TabControl();
      this.tabPage_General = new System.Windows.Forms.TabPage();
      this.propertyGrid_General = new CSharpFramework.Controls.PropertyGridBase();
      this.groupBox1 = new System.Windows.Forms.GroupBox();
      this.comboBox_Import = new System.Windows.Forms.ComboBox();
      this.label_Import = new System.Windows.Forms.Label();
      this.label_TerrainArea = new System.Windows.Forms.Label();
      this.label_TerrainSize = new System.Windows.Forms.Label();
      this.label_SampleCount = new System.Windows.Forms.Label();
      this.label3 = new System.Windows.Forms.Label();
      this.numericUpDown_Units = new System.Windows.Forms.NumericUpDown();
      this.label2 = new System.Windows.Forms.Label();
      this.label1 = new System.Windows.Forms.Label();
      this.tabPage_Heightmap = new System.Windows.Forms.TabPage();
      this.propertyGrid_Heightmap = new CSharpFramework.Controls.PropertyGridBase();
      this.tabPage_Texturing = new System.Windows.Forms.TabPage();
      this.propertyGrid_Texturing = new CSharpFramework.Controls.PropertyGridBase();
      this.tabPage_Vegetation = new System.Windows.Forms.TabPage();
      this.propertyGrid_Vegetation = new CSharpFramework.Controls.PropertyGridBase();
      this.pictureBox1 = new System.Windows.Forms.PictureBox();
      this.dialogCaptionBar1 = new CSharpFramework.Controls.DialogCaptionBar();
      this.CheckboxEnableMovingSectorPosition = new System.Windows.Forms.CheckBox();
      this.panel1.SuspendLayout();
      this.panel2.SuspendLayout();
      this.tabControl1.SuspendLayout();
      this.tabPage_General.SuspendLayout();
      this.groupBox1.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_Units)).BeginInit();
      this.tabPage_Heightmap.SuspendLayout();
      this.tabPage_Texturing.SuspendLayout();
      this.tabPage_Vegetation.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
      this.SuspendLayout();
      // 
      // button_OK
      // 
      this.button_OK.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.button_OK.Location = new System.Drawing.Point(337, 22);
      this.button_OK.Name = "button_OK";
      this.button_OK.Size = new System.Drawing.Size(76, 26);
      this.button_OK.TabIndex = 0;
      this.button_OK.Text = "OK";
      this.button_OK.UseVisualStyleBackColor = true;
      this.button_OK.Click += new System.EventHandler(this.button_OK_Click);
      // 
      // button_Cancel
      // 
      this.button_Cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.button_Cancel.Location = new System.Drawing.Point(445, 22);
      this.button_Cancel.Name = "button_Cancel";
      this.button_Cancel.Size = new System.Drawing.Size(76, 26);
      this.button_Cancel.TabIndex = 1;
      this.button_Cancel.Text = "Cancel";
      this.button_Cancel.UseVisualStyleBackColor = true;
      // 
      // panel1
      // 
      this.panel1.Controls.Add(this.button_OK);
      this.panel1.Controls.Add(this.button_Cancel);
      this.panel1.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.panel1.Location = new System.Drawing.Point(0, 558);
      this.panel1.Name = "panel1";
      this.panel1.Size = new System.Drawing.Size(864, 60);
      this.panel1.TabIndex = 1;
      // 
      // CheckboxEnableMovingSectorPosition
      // 
      this.CheckboxEnableMovingSectorPosition.Location = new System.Drawing.Point(672, 6);
      this.CheckboxEnableMovingSectorPosition.Name = "CheckboxEnableMovingSectorPosition";
      this.CheckboxEnableMovingSectorPosition.Size = new System.Drawing.Size(189, 28);
      this.CheckboxEnableMovingSectorPosition.TabIndex = 2;
      this.CheckboxEnableMovingSectorPosition.Text = " Enable moving sector position";
      // 
      // panel2
      // 
      this.panel2.Controls.Add(this.tabControl1);
      this.panel2.Dock = System.Windows.Forms.DockStyle.Left;
      this.panel2.Location = new System.Drawing.Point(0, 50);
      this.panel2.Name = "panel2";
      this.panel2.Size = new System.Drawing.Size(377, 508);
      this.panel2.TabIndex = 6;
      // 
      // tabControl1
      // 
      this.tabControl1.Controls.Add(this.tabPage_General);
      this.tabControl1.Controls.Add(this.tabPage_Heightmap);
      this.tabControl1.Controls.Add(this.tabPage_Texturing);
      this.tabControl1.Controls.Add(this.tabPage_Vegetation);
      this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.tabControl1.Location = new System.Drawing.Point(0, 0);
      this.tabControl1.Name = "tabControl1";
      this.tabControl1.SelectedIndex = 0;
      this.tabControl1.Size = new System.Drawing.Size(377, 508);
      this.tabControl1.TabIndex = 0;
      // 
      // tabPage_General
      // 
      this.tabPage_General.Controls.Add(this.propertyGrid_General);
      this.tabPage_General.Controls.Add(this.groupBox1);
      this.tabPage_General.Location = new System.Drawing.Point(4, 22);
      this.tabPage_General.Name = "tabPage_General";
      this.tabPage_General.Size = new System.Drawing.Size(369, 482);
      this.tabPage_General.TabIndex = 3;
      this.tabPage_General.Text = "General";
      this.tabPage_General.UseVisualStyleBackColor = true;
      // 
      // propertyGrid_General
      // 
      this.propertyGrid_General.Dock = System.Windows.Forms.DockStyle.Fill;
      this.propertyGrid_General.Location = new System.Drawing.Point(0, 0);
      this.propertyGrid_General.Name = "propertyGrid_General";
      this.propertyGrid_General.Size = new System.Drawing.Size(369, 320);
      this.propertyGrid_General.TabIndex = 0;
      this.propertyGrid_General.SelectedGridItemChanged += new System.Windows.Forms.SelectedGridItemChangedEventHandler(this.propertyGrid_Config_SelectedGridItemChanged);
      // 
      // groupBox1
      // 
      this.groupBox1.Controls.Add(this.comboBox_Import);
      this.groupBox1.Controls.Add(this.label_Import);
      this.groupBox1.Controls.Add(this.label_TerrainArea);
      this.groupBox1.Controls.Add(this.label_TerrainSize);
      this.groupBox1.Controls.Add(this.label_SampleCount);
      this.groupBox1.Controls.Add(this.label3);
      this.groupBox1.Controls.Add(this.numericUpDown_Units);
      this.groupBox1.Controls.Add(this.label2);
      this.groupBox1.Controls.Add(this.label1);
      this.groupBox1.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.groupBox1.Location = new System.Drawing.Point(0, 320);
      this.groupBox1.Name = "groupBox1";
      this.groupBox1.Size = new System.Drawing.Size(369, 162);
      this.groupBox1.TabIndex = 1;
      this.groupBox1.TabStop = false;
      this.groupBox1.Text = "Terrain Statistics";
      // 
      // comboBox_Import
      // 
      this.comboBox_Import.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.comboBox_Import.FormattingEnabled = true;
      this.comboBox_Import.Location = new System.Drawing.Point(134, 124);
      this.comboBox_Import.Name = "comboBox_Import";
      this.comboBox_Import.Size = new System.Drawing.Size(176, 21);
      this.comboBox_Import.TabIndex = 8;
      this.comboBox_Import.SelectedIndexChanged += new System.EventHandler(this.comboBox_Import_SelectedIndexChanged);
      // 
      // label_Import
      // 
      this.label_Import.AutoSize = true;
      this.label_Import.Location = new System.Drawing.Point(25, 127);
      this.label_Import.Name = "label_Import";
      this.label_Import.Size = new System.Drawing.Size(103, 13);
      this.label_Import.TabIndex = 7;
      this.label_Import.Text = "Import Settings from:";
      // 
      // label_TerrainArea
      // 
      this.label_TerrainArea.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
      this.label_TerrainArea.Location = new System.Drawing.Point(84, 89);
      this.label_TerrainArea.Name = "label_TerrainArea";
      this.label_TerrainArea.Size = new System.Drawing.Size(147, 16);
      this.label_TerrainArea.TabIndex = 6;
      this.label_TerrainArea.Text = "1000";
      this.label_TerrainArea.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
      // 
      // label_TerrainSize
      // 
      this.label_TerrainSize.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
      this.label_TerrainSize.Location = new System.Drawing.Point(85, 67);
      this.label_TerrainSize.Name = "label_TerrainSize";
      this.label_TerrainSize.Size = new System.Drawing.Size(146, 16);
      this.label_TerrainSize.TabIndex = 5;
      this.label_TerrainSize.Text = "1000";
      this.label_TerrainSize.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
      // 
      // label_SampleCount
      // 
      this.label_SampleCount.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
      this.label_SampleCount.Location = new System.Drawing.Point(188, 24);
      this.label_SampleCount.Name = "label_SampleCount";
      this.label_SampleCount.Size = new System.Drawing.Size(160, 16);
      this.label_SampleCount.TabIndex = 1;
      this.label_SampleCount.Text = "1000";
      this.label_SampleCount.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // label3
      // 
      this.label3.AutoSize = true;
      this.label3.Location = new System.Drawing.Point(144, 48);
      this.label3.Name = "label3";
      this.label3.Size = new System.Drawing.Size(166, 13);
      this.label3.TabIndex = 4;
      this.label3.Text = "meters, the terrain\'s overall size is:";
      // 
      // numericUpDown_Units
      // 
      this.numericUpDown_Units.DecimalPlaces = 3;
      this.numericUpDown_Units.Increment = new decimal(new int[] {
            1,
            0,
            0,
            131072});
      this.numericUpDown_Units.Location = new System.Drawing.Point(85, 44);
      this.numericUpDown_Units.Name = "numericUpDown_Units";
      this.numericUpDown_Units.Size = new System.Drawing.Size(55, 20);
      this.numericUpDown_Units.TabIndex = 3;
      this.numericUpDown_Units.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
      this.numericUpDown_Units.Value = new decimal(new int[] {
            1,
            0,
            0,
            131072});
      this.numericUpDown_Units.ValueChanged += new System.EventHandler(this.numericUpDown_Units_ValueChanged);
      // 
      // label2
      // 
      this.label2.AutoSize = true;
      this.label2.Location = new System.Drawing.Point(16, 47);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(64, 13);
      this.label2.TabIndex = 2;
      this.label2.Text = "If one unit is";
      // 
      // label1
      // 
      this.label1.AutoSize = true;
      this.label1.Location = new System.Drawing.Point(16, 24);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(166, 13);
      this.label1.TabIndex = 0;
      this.label1.Text = "Overall number of height samples:";
      // 
      // tabPage_Heightmap
      // 
      this.tabPage_Heightmap.Controls.Add(this.propertyGrid_Heightmap);
      this.tabPage_Heightmap.Location = new System.Drawing.Point(4, 22);
      this.tabPage_Heightmap.Name = "tabPage_Heightmap";
      this.tabPage_Heightmap.Padding = new System.Windows.Forms.Padding(3);
      this.tabPage_Heightmap.Size = new System.Drawing.Size(369, 482);
      this.tabPage_Heightmap.TabIndex = 0;
      this.tabPage_Heightmap.Text = "Heightmap";
      this.tabPage_Heightmap.UseVisualStyleBackColor = true;
      // 
      // propertyGrid_Heightmap
      // 
      this.propertyGrid_Heightmap.Dock = System.Windows.Forms.DockStyle.Fill;
      this.propertyGrid_Heightmap.Location = new System.Drawing.Point(3, 3);
      this.propertyGrid_Heightmap.Name = "propertyGrid_Heightmap";
      this.propertyGrid_Heightmap.Size = new System.Drawing.Size(363, 476);
      this.propertyGrid_Heightmap.TabIndex = 0;
      this.propertyGrid_Heightmap.SelectedGridItemChanged += new System.Windows.Forms.SelectedGridItemChangedEventHandler(this.propertyGrid_Config_SelectedGridItemChanged);
      // 
      // tabPage_Texturing
      // 
      this.tabPage_Texturing.Controls.Add(this.propertyGrid_Texturing);
      this.tabPage_Texturing.Location = new System.Drawing.Point(4, 22);
      this.tabPage_Texturing.Name = "tabPage_Texturing";
      this.tabPage_Texturing.Padding = new System.Windows.Forms.Padding(3);
      this.tabPage_Texturing.Size = new System.Drawing.Size(369, 482);
      this.tabPage_Texturing.TabIndex = 1;
      this.tabPage_Texturing.Text = "Texturing";
      this.tabPage_Texturing.UseVisualStyleBackColor = true;
      // 
      // CheckboxEnableMovingSectorPosition
      // 
      this.panel1.Controls.Add(this.CheckboxEnableMovingSectorPosition);
      this.panel1.Controls.Add(this.button_OK);
      this.panel1.Controls.Add(this.button_Cancel);
      this.panel1.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.panel1.Location = new System.Drawing.Point(0, 558);
      this.panel1.Name = "panel1";
      this.panel1.Size = new System.Drawing.Size(864, 60);
      this.panel1.TabIndex = 5;

      // 
      // propertyGrid_Texturing
      // 
      this.propertyGrid_Texturing.Dock = System.Windows.Forms.DockStyle.Fill;
      this.propertyGrid_Texturing.Location = new System.Drawing.Point(3, 3);
      this.propertyGrid_Texturing.Name = "propertyGrid_Texturing";
      this.propertyGrid_Texturing.Size = new System.Drawing.Size(363, 476);
      this.propertyGrid_Texturing.TabIndex = 0;
      this.propertyGrid_Texturing.SelectedGridItemChanged += new System.Windows.Forms.SelectedGridItemChangedEventHandler(this.propertyGrid_Config_SelectedGridItemChanged);
      // 
      // tabPage_Vegetation
      // 
      this.tabPage_Vegetation.Controls.Add(this.propertyGrid_Vegetation);
      this.tabPage_Vegetation.Location = new System.Drawing.Point(4, 22);
      this.tabPage_Vegetation.Name = "tabPage_Vegetation";
      this.tabPage_Vegetation.Size = new System.Drawing.Size(369, 482);
      this.tabPage_Vegetation.TabIndex = 2;
      this.tabPage_Vegetation.Text = "Vegetation";
      this.tabPage_Vegetation.UseVisualStyleBackColor = true;
      // 
      // propertyGrid_Vegetation
      // 
      this.propertyGrid_Vegetation.Dock = System.Windows.Forms.DockStyle.Fill;
      this.propertyGrid_Vegetation.Location = new System.Drawing.Point(0, 0);
      this.propertyGrid_Vegetation.Name = "propertyGrid_Vegetation";
      this.propertyGrid_Vegetation.Size = new System.Drawing.Size(369, 482);
      this.propertyGrid_Vegetation.TabIndex = 0;
      this.propertyGrid_Vegetation.SelectedGridItemChanged += new System.Windows.Forms.SelectedGridItemChangedEventHandler(this.propertyGrid_Config_SelectedGridItemChanged);
      // 
      // pictureBox1
      // 
      this.pictureBox1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
      this.pictureBox1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.pictureBox1.Location = new System.Drawing.Point(377, 50);
      this.pictureBox1.Name = "pictureBox1";
      this.pictureBox1.Size = new System.Drawing.Size(487, 508);
      this.pictureBox1.TabIndex = 7;
      this.pictureBox1.TabStop = false;
      this.pictureBox1.MouseMove += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseMove);
      this.pictureBox1.MouseDown += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseDown);
      this.pictureBox1.Paint += new System.Windows.Forms.PaintEventHandler(this.pictureBox1_Paint);
      this.pictureBox1.MouseUp += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseUp);
      // 
      // dialogCaptionBar1
      // 
      this.dialogCaptionBar1.BackColor = System.Drawing.SystemColors.Window;
      this.dialogCaptionBar1.Caption = "Setup Terrain Configuration";
      this.dialogCaptionBar1.CompactView = false;
      this.dialogCaptionBar1.Description = "Setup initial Terrain properties";
      this.dialogCaptionBar1.Dock = System.Windows.Forms.DockStyle.Top;
      this.dialogCaptionBar1.Image = ((System.Drawing.Image)(resources.GetObject("dialogCaptionBar1.Image")));
      this.dialogCaptionBar1.Location = new System.Drawing.Point(0, 0);
      this.dialogCaptionBar1.Name = "dialogCaptionBar1";
      this.dialogCaptionBar1.SetFontColor = System.Drawing.SystemColors.ControlText;
      this.dialogCaptionBar1.ShowBorder = false;
      this.dialogCaptionBar1.ShowBottomLine = true;
      this.dialogCaptionBar1.ShowCaptionText = true;
      this.dialogCaptionBar1.ShowImage = true;
      this.dialogCaptionBar1.Size = new System.Drawing.Size(864, 50);
      this.dialogCaptionBar1.TabIndex = 0;
      // 
      // TerrainConfigDlg
      // 
      this.AcceptButton = this.button_OK;
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.CancelButton = this.button_Cancel;
      this.ClientSize = new System.Drawing.Size(864, 618);
      this.Controls.Add(this.pictureBox1);
      this.Controls.Add(this.panel2);
      this.Controls.Add(this.panel1);
      this.Controls.Add(this.dialogCaptionBar1);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "TerrainConfigDlg";
      this.Text = "Terrain Config";
      this.panel1.ResumeLayout(false);
      this.panel2.ResumeLayout(false);
      this.tabControl1.ResumeLayout(false);
      this.tabPage_General.ResumeLayout(false);
      this.groupBox1.ResumeLayout(false);
      this.groupBox1.PerformLayout();
      ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_Units)).EndInit();
      this.tabPage_Heightmap.ResumeLayout(false);
      this.tabPage_Texturing.ResumeLayout(false);
      this.tabPage_Vegetation.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
      this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.Button button_OK;
		private System.Windows.Forms.Button button_Cancel;
    //private CSharpFramework.Controls.PropertyGridBase propertyGrid_Config;
    private CSharpFramework.Controls.DialogCaptionBar dialogCaptionBar1;
    private System.Windows.Forms.Panel panel1;
    private System.Windows.Forms.Panel panel2;
    private System.Windows.Forms.GroupBox groupBox1;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.Label label3;
    private System.Windows.Forms.NumericUpDown numericUpDown_Units;
    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.Label label_TerrainSize;
    private System.Windows.Forms.Label label_SampleCount;
    private System.Windows.Forms.Label label_TerrainArea;
    private System.Windows.Forms.Label label_Import;
    private System.Windows.Forms.ComboBox comboBox_Import;
    private System.Windows.Forms.PictureBox pictureBox1;
    private System.Windows.Forms.TabControl tabControl1;
    private System.Windows.Forms.TabPage tabPage_General;
    private System.Windows.Forms.TabPage tabPage_Heightmap;
    private System.Windows.Forms.TabPage tabPage_Texturing;
    private System.Windows.Forms.TabPage tabPage_Vegetation;
    private CSharpFramework.Controls.PropertyGridBase propertyGrid_General;
    private CSharpFramework.Controls.PropertyGridBase propertyGrid_Heightmap;
    private CSharpFramework.Controls.PropertyGridBase propertyGrid_Texturing;
    private CSharpFramework.Controls.PropertyGridBase propertyGrid_Vegetation;
    private System.Windows.Forms.CheckBox CheckboxEnableMovingSectorPosition;
	}
}