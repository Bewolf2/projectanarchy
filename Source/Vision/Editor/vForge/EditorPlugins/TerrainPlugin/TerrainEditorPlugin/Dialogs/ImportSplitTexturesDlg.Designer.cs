namespace TerrainEditorPlugin.Dialogs
{
  partial class ImportSplitTexturesDlg
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ImportSplitTexturesDlg));
      this.button_OK = new System.Windows.Forms.Button();
      this.button_Cancel = new System.Windows.Forms.Button();
      this.textBox_Diffuse = new System.Windows.Forms.TextBox();
      this.textBox_Normalmap = new System.Windows.Forms.TextBox();
      this.textBox_Specular = new System.Windows.Forms.TextBox();
      this.button_ImportDiffuse = new System.Windows.Forms.Button();
      this.button_ImportNormalmap = new System.Windows.Forms.Button();
      this.button_ImportSpecular = new System.Windows.Forms.Button();
      this.label1 = new System.Windows.Forms.Label();
      this.label2 = new System.Windows.Forms.Label();
      this.label3 = new System.Windows.Forms.Label();
      this.dialogCaptionBar1 = new CSharpFramework.Controls.DialogCaptionBar();
      this.textBox1 = new System.Windows.Forms.TextBox();
      this.pictureBox1 = new System.Windows.Forms.PictureBox();
      this.checkBox_Border = new System.Windows.Forms.CheckBox();
      ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
      this.SuspendLayout();
      // 
      // button_OK
      // 
      this.button_OK.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.button_OK.Location = new System.Drawing.Point(182, 467);
      this.button_OK.Name = "button_OK";
      this.button_OK.Size = new System.Drawing.Size(82, 28);
      this.button_OK.TabIndex = 12;
      this.button_OK.Text = "Import";
      this.button_OK.UseVisualStyleBackColor = true;
      this.button_OK.Click += new System.EventHandler(this.button_OK_Click);
      // 
      // button_Cancel
      // 
      this.button_Cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.button_Cancel.Location = new System.Drawing.Point(294, 467);
      this.button_Cancel.Name = "button_Cancel";
      this.button_Cancel.Size = new System.Drawing.Size(82, 28);
      this.button_Cancel.TabIndex = 13;
      this.button_Cancel.Text = "Cancel";
      this.button_Cancel.UseVisualStyleBackColor = true;
      // 
      // textBox_Diffuse
      // 
      this.textBox_Diffuse.Location = new System.Drawing.Point(29, 262);
      this.textBox_Diffuse.Name = "textBox_Diffuse";
      this.textBox_Diffuse.Size = new System.Drawing.Size(535, 20);
      this.textBox_Diffuse.TabIndex = 3;
      this.textBox_Diffuse.TextChanged += new System.EventHandler(this.textBox_Diffuse_TextChanged);
      // 
      // textBox_Normalmap
      // 
      this.textBox_Normalmap.Location = new System.Drawing.Point(29, 314);
      this.textBox_Normalmap.Name = "textBox_Normalmap";
      this.textBox_Normalmap.Size = new System.Drawing.Size(535, 20);
      this.textBox_Normalmap.TabIndex = 6;
      this.textBox_Normalmap.TextChanged += new System.EventHandler(this.textBox_Normalmap_TextChanged);
      // 
      // textBox_Specular
      // 
      this.textBox_Specular.Location = new System.Drawing.Point(29, 367);
      this.textBox_Specular.Name = "textBox_Specular";
      this.textBox_Specular.Size = new System.Drawing.Size(535, 20);
      this.textBox_Specular.TabIndex = 9;
      this.textBox_Specular.TextChanged += new System.EventHandler(this.textBox_Specular_TextChanged);
      // 
      // button_ImportDiffuse
      // 
      this.button_ImportDiffuse.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.button_ImportDiffuse.Location = new System.Drawing.Point(570, 262);
      this.button_ImportDiffuse.Name = "button_ImportDiffuse";
      this.button_ImportDiffuse.Size = new System.Drawing.Size(25, 19);
      this.button_ImportDiffuse.TabIndex = 4;
      this.button_ImportDiffuse.Text = "...";
      this.button_ImportDiffuse.UseVisualStyleBackColor = true;
      this.button_ImportDiffuse.Click += new System.EventHandler(this.button_ImportDiffuse_Click);
      // 
      // button_ImportNormalmap
      // 
      this.button_ImportNormalmap.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.button_ImportNormalmap.Location = new System.Drawing.Point(570, 315);
      this.button_ImportNormalmap.Name = "button_ImportNormalmap";
      this.button_ImportNormalmap.Size = new System.Drawing.Size(25, 19);
      this.button_ImportNormalmap.TabIndex = 7;
      this.button_ImportNormalmap.Text = "...";
      this.button_ImportNormalmap.UseVisualStyleBackColor = true;
      this.button_ImportNormalmap.Click += new System.EventHandler(this.button_ImportNormalmap_Click);
      // 
      // button_ImportSpecular
      // 
      this.button_ImportSpecular.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.button_ImportSpecular.Location = new System.Drawing.Point(570, 368);
      this.button_ImportSpecular.Name = "button_ImportSpecular";
      this.button_ImportSpecular.Size = new System.Drawing.Size(25, 19);
      this.button_ImportSpecular.TabIndex = 10;
      this.button_ImportSpecular.Text = "...";
      this.button_ImportSpecular.UseVisualStyleBackColor = true;
      this.button_ImportSpecular.Click += new System.EventHandler(this.button_ImportSpecular_Click);
      // 
      // label1
      // 
      this.label1.AutoSize = true;
      this.label1.Location = new System.Drawing.Point(32, 243);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(106, 13);
      this.label1.TabIndex = 2;
      this.label1.Text = "Large diffuse texture:";
      // 
      // label2
      // 
      this.label2.AutoSize = true;
      this.label2.Location = new System.Drawing.Point(32, 298);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(172, 13);
      this.label2.TabIndex = 5;
      this.label2.Text = "Large normalmap texture (optional):";
      // 
      // label3
      // 
      this.label3.AutoSize = true;
      this.label3.Location = new System.Drawing.Point(32, 351);
      this.label3.Name = "label3";
      this.label3.Size = new System.Drawing.Size(161, 13);
      this.label3.TabIndex = 8;
      this.label3.Text = "Large specular texture (optional):";
      // 
      // dialogCaptionBar1
      // 
      this.dialogCaptionBar1.BackColor = System.Drawing.SystemColors.Window;
      this.dialogCaptionBar1.Caption = "Import per Sector textures";
      this.dialogCaptionBar1.CompactView = false;
      this.dialogCaptionBar1.Description = resources.GetString("dialogCaptionBar1.Description");
      this.dialogCaptionBar1.Dock = System.Windows.Forms.DockStyle.Top;
      this.dialogCaptionBar1.Image = ((System.Drawing.Image)(resources.GetObject("dialogCaptionBar1.Image")));
      this.dialogCaptionBar1.Location = new System.Drawing.Point(0, 0);
      this.dialogCaptionBar1.Name = "dialogCaptionBar1";
      this.dialogCaptionBar1.SetFontColor = System.Drawing.SystemColors.ControlText;
      this.dialogCaptionBar1.ShowBorder = false;
      this.dialogCaptionBar1.ShowBottomLine = true;
      this.dialogCaptionBar1.ShowCaptionText = true;
      this.dialogCaptionBar1.ShowImage = true;
      this.dialogCaptionBar1.Size = new System.Drawing.Size(619, 74);
      this.dialogCaptionBar1.TabIndex = 0;
      // 
      // textBox1
      // 
      this.textBox1.Location = new System.Drawing.Point(66, 80);
      this.textBox1.Multiline = true;
      this.textBox1.Name = "textBox1";
      this.textBox1.ReadOnly = true;
      this.textBox1.Size = new System.Drawing.Size(531, 147);
      this.textBox1.TabIndex = 1;
      this.textBox1.Text = resources.GetString("textBox1.Text");
      // 
      // pictureBox1
      // 
      this.pictureBox1.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox1.Image")));
      this.pictureBox1.Location = new System.Drawing.Point(10, 80);
      this.pictureBox1.Name = "pictureBox1";
      this.pictureBox1.Size = new System.Drawing.Size(50, 62);
      this.pictureBox1.TabIndex = 15;
      this.pictureBox1.TabStop = false;
      // 
      // checkBox_Border
      // 
      this.checkBox_Border.AutoSize = true;
      this.checkBox_Border.Location = new System.Drawing.Point(29, 411);
      this.checkBox_Border.Name = "checkBox_Border";
      this.checkBox_Border.Size = new System.Drawing.Size(114, 17);
      this.checkBox_Border.TabIndex = 11;
      this.checkBox_Border.Text = "Add filtering border";
      this.checkBox_Border.UseVisualStyleBackColor = true;
      // 
      // ImportSplitTexturesDlg
      // 
      this.AcceptButton = this.button_OK;
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.CancelButton = this.button_Cancel;
      this.ClientSize = new System.Drawing.Size(619, 507);
      this.Controls.Add(this.checkBox_Border);
      this.Controls.Add(this.pictureBox1);
      this.Controls.Add(this.textBox1);
      this.Controls.Add(this.label3);
      this.Controls.Add(this.label2);
      this.Controls.Add(this.label1);
      this.Controls.Add(this.button_ImportSpecular);
      this.Controls.Add(this.button_ImportNormalmap);
      this.Controls.Add(this.button_ImportDiffuse);
      this.Controls.Add(this.textBox_Specular);
      this.Controls.Add(this.textBox_Normalmap);
      this.Controls.Add(this.textBox_Diffuse);
      this.Controls.Add(this.dialogCaptionBar1);
      this.Controls.Add(this.button_Cancel);
      this.Controls.Add(this.button_OK);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "ImportSplitTexturesDlg";
      this.Text = "Import per Sector Textures";
      ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.Button button_OK;
    private System.Windows.Forms.Button button_Cancel;
    private CSharpFramework.Controls.DialogCaptionBar dialogCaptionBar1;
    private System.Windows.Forms.TextBox textBox_Diffuse;
    private System.Windows.Forms.TextBox textBox_Normalmap;
    private System.Windows.Forms.TextBox textBox_Specular;
    private System.Windows.Forms.Button button_ImportDiffuse;
    private System.Windows.Forms.Button button_ImportNormalmap;
    private System.Windows.Forms.Button button_ImportSpecular;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.Label label3;
    private System.Windows.Forms.TextBox textBox1;
    private System.Windows.Forms.PictureBox pictureBox1;
    private System.Windows.Forms.CheckBox checkBox_Border;
  }
}