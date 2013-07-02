namespace TerrainEditorPlugin.Dialogs
{
  partial class TerrainCreationDlg
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(TerrainCreationDlg));
      this.button_Create = new System.Windows.Forms.Button();
      this.buttonCancel = new System.Windows.Forms.Button();
      this.captionBar = new CSharpFramework.Controls.DialogCaptionBar();
      this.radioButton_New = new System.Windows.Forms.RadioButton();
      this.radioButton_Existing = new System.Windows.Forms.RadioButton();
      this.pictureBox1 = new System.Windows.Forms.PictureBox();
      this.pictureBox2 = new System.Windows.Forms.PictureBox();
      ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
      ((System.ComponentModel.ISupportInitialize)(this.pictureBox2)).BeginInit();
      this.SuspendLayout();
      // 
      // button_Create
      // 
      this.button_Create.Location = new System.Drawing.Point(78, 407);
      this.button_Create.Name = "button_Create";
      this.button_Create.Size = new System.Drawing.Size(95, 28);
      this.button_Create.TabIndex = 3;
      this.button_Create.Text = "Create";
      this.button_Create.UseVisualStyleBackColor = true;
      this.button_Create.Click += new System.EventHandler(this.button_Create_Click);
      // 
      // buttonCancel
      // 
      this.buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.buttonCancel.Location = new System.Drawing.Point(186, 407);
      this.buttonCancel.Name = "buttonCancel";
      this.buttonCancel.Size = new System.Drawing.Size(95, 28);
      this.buttonCancel.TabIndex = 4;
      this.buttonCancel.Text = "Cancel";
      this.buttonCancel.UseVisualStyleBackColor = true;
      this.buttonCancel.Click += new System.EventHandler(this.buttonCancel_Click);
      // 
      // captionBar
      // 
      this.captionBar.BackColor = System.Drawing.SystemColors.Window;
      this.captionBar.Caption = "Create a new Terrain";
      this.captionBar.CompactView = false;
      this.captionBar.Description = "Please select the method for creating a new terrain and click on \'Create\'";
      this.captionBar.Dock = System.Windows.Forms.DockStyle.Top;
      this.captionBar.Image = ((System.Drawing.Image)(resources.GetObject("captionBar.Image")));
      this.captionBar.Location = new System.Drawing.Point(0, 0);
      this.captionBar.Name = "captionBar";
      this.captionBar.SetFontColor = System.Drawing.SystemColors.ControlText;
      this.captionBar.ShowBorder = false;
      this.captionBar.ShowBottomLine = true;
      this.captionBar.ShowCaptionText = true;
      this.captionBar.ShowImage = true;
      this.captionBar.Size = new System.Drawing.Size(378, 50);
      this.captionBar.TabIndex = 0;
      // 
      // radioButton_New
      // 
      this.radioButton_New.AutoSize = true;
      this.radioButton_New.Location = new System.Drawing.Point(24, 97);
      this.radioButton_New.Name = "radioButton_New";
      this.radioButton_New.Size = new System.Drawing.Size(120, 17);
      this.radioButton_New.TabIndex = 1;
      this.radioButton_New.TabStop = true;
      this.radioButton_New.Text = "Create a new terrain";
      this.radioButton_New.UseVisualStyleBackColor = true;
      this.radioButton_New.CheckedChanged += new System.EventHandler(this.radioButton_New_CheckedChanged);
      // 
      // radioButton_Existing
      // 
      this.radioButton_Existing.AutoSize = true;
      this.radioButton_Existing.Location = new System.Drawing.Point(24, 165);
      this.radioButton_Existing.Name = "radioButton_Existing";
      this.radioButton_Existing.Size = new System.Drawing.Size(145, 17);
      this.radioButton_Existing.TabIndex = 2;
      this.radioButton_Existing.TabStop = true;
      this.radioButton_Existing.Text = "Reference existing terrain";
      this.radioButton_Existing.UseVisualStyleBackColor = true;
      this.radioButton_Existing.CheckedChanged += new System.EventHandler(this.radioButton_Existing_CheckedChanged);
      // 
      // pictureBox1
      // 
      this.pictureBox1.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox1.Image")));
      this.pictureBox1.Location = new System.Drawing.Point(186, 72);
      this.pictureBox1.Name = "pictureBox1";
      this.pictureBox1.Size = new System.Drawing.Size(64, 64);
      this.pictureBox1.TabIndex = 12;
      this.pictureBox1.TabStop = false;
      // 
      // pictureBox2
      // 
      this.pictureBox2.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox2.Image")));
      this.pictureBox2.Location = new System.Drawing.Point(186, 139);
      this.pictureBox2.Name = "pictureBox2";
      this.pictureBox2.Size = new System.Drawing.Size(160, 64);
      this.pictureBox2.TabIndex = 13;
      this.pictureBox2.TabStop = false;
      // 
      // TerrainCreationDlg
      // 
      this.AcceptButton = this.button_Create;
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.CancelButton = this.buttonCancel;
      this.ClientSize = new System.Drawing.Size(378, 450);
      this.Controls.Add(this.pictureBox2);
      this.Controls.Add(this.pictureBox1);
      this.Controls.Add(this.radioButton_Existing);
      this.Controls.Add(this.radioButton_New);
      this.Controls.Add(this.captionBar);
      this.Controls.Add(this.buttonCancel);
      this.Controls.Add(this.button_Create);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "TerrainCreationDlg";
      this.Text = "Create a new Terrain";
      this.Load += new System.EventHandler(this.TerrainCreationDlg_Load);
      ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
      ((System.ComponentModel.ISupportInitialize)(this.pictureBox2)).EndInit();
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.Button button_Create;
    private System.Windows.Forms.Button buttonCancel;
    private CSharpFramework.Controls.DialogCaptionBar captionBar;
    private System.Windows.Forms.RadioButton radioButton_New;
    private System.Windows.Forms.RadioButton radioButton_Existing;
      private System.Windows.Forms.PictureBox pictureBox1;
    private System.Windows.Forms.PictureBox pictureBox2;
  }
}