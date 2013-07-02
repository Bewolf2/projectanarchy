namespace VisionEditorPlugin.Dialogs
{
  partial class ColorGradingToolDlg
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ColorGradingToolDlg));
      this.pictureBox_Orig = new System.Windows.Forms.PictureBox();
      this.dialogCaptionBar = new CSharpFramework.Controls.DialogCaptionBar();
      this.splitContainer1 = new System.Windows.Forms.SplitContainer();
      this.label_Click1 = new System.Windows.Forms.Label();
      this.label1 = new System.Windows.Forms.Label();
      this.label_Click2 = new System.Windows.Forms.Label();
      this.pictureBox_Target = new System.Windows.Forms.PictureBox();
      this.label2 = new System.Windows.Forms.Label();
      this.panel1 = new System.Windows.Forms.Panel();
      this.label3 = new System.Windows.Forms.Label();
      this.button_Close = new System.Windows.Forms.Button();
      this.button_Save = new System.Windows.Forms.Button();
      this.comboBox_Size = new System.Windows.Forms.ComboBox();
      ((System.ComponentModel.ISupportInitialize)(this.pictureBox_Orig)).BeginInit();
      this.splitContainer1.Panel1.SuspendLayout();
      this.splitContainer1.Panel2.SuspendLayout();
      this.splitContainer1.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.pictureBox_Target)).BeginInit();
      this.panel1.SuspendLayout();
      this.SuspendLayout();
      // 
      // pictureBox_Orig
      // 
      this.pictureBox_Orig.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
      this.pictureBox_Orig.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
      this.pictureBox_Orig.Dock = System.Windows.Forms.DockStyle.Fill;
      this.pictureBox_Orig.Location = new System.Drawing.Point(0, 35);
      this.pictureBox_Orig.Name = "pictureBox_Orig";
      this.pictureBox_Orig.Size = new System.Drawing.Size(369, 400);
      this.pictureBox_Orig.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
      this.pictureBox_Orig.TabIndex = 0;
      this.pictureBox_Orig.TabStop = false;
      this.pictureBox_Orig.Click += new System.EventHandler(this.pictureBox_Orig_Click);
      // 
      // dialogCaptionBar
      // 
      this.dialogCaptionBar.BackColor = System.Drawing.SystemColors.Window;
      this.dialogCaptionBar.Caption = "Color Grading generation";
      this.dialogCaptionBar.CompactView = false;
      this.dialogCaptionBar.Description = "This tool generates a color grading table via screenshot comparison. The output t" +
          "able texture can be referenced by the tonemapper in GRADING mode. The screenshot" +
          "s are not needed during runtime";
      this.dialogCaptionBar.Dock = System.Windows.Forms.DockStyle.Top;
      this.dialogCaptionBar.Image = ((System.Drawing.Image)(resources.GetObject("dialogCaptionBar.Image")));
      this.dialogCaptionBar.Location = new System.Drawing.Point(0, 0);
      this.dialogCaptionBar.Name = "dialogCaptionBar";
      this.dialogCaptionBar.SetFontColor = System.Drawing.SystemColors.ControlText;
      this.dialogCaptionBar.ShowBorder = false;
      this.dialogCaptionBar.ShowBottomLine = true;
      this.dialogCaptionBar.ShowCaptionText = true;
      this.dialogCaptionBar.ShowImage = true;
      this.dialogCaptionBar.Size = new System.Drawing.Size(744, 50);
      this.dialogCaptionBar.TabIndex = 0;
      // 
      // splitContainer1
      // 
      this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.splitContainer1.Location = new System.Drawing.Point(0, 50);
      this.splitContainer1.Name = "splitContainer1";
      // 
      // splitContainer1.Panel1
      // 
      this.splitContainer1.Panel1.Controls.Add(this.label_Click1);
      this.splitContainer1.Panel1.Controls.Add(this.pictureBox_Orig);
      this.splitContainer1.Panel1.Controls.Add(this.label1);
      // 
      // splitContainer1.Panel2
      // 
      this.splitContainer1.Panel2.Controls.Add(this.label_Click2);
      this.splitContainer1.Panel2.Controls.Add(this.pictureBox_Target);
      this.splitContainer1.Panel2.Controls.Add(this.label2);
      this.splitContainer1.Size = new System.Drawing.Size(744, 435);
      this.splitContainer1.SplitterDistance = 369;
      this.splitContainer1.TabIndex = 94;
      // 
      // label_Click1
      // 
      this.label_Click1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.label_Click1.AutoSize = true;
      this.label_Click1.Location = new System.Drawing.Point(128, 202);
      this.label_Click1.Name = "label_Click1";
      this.label_Click1.Size = new System.Drawing.Size(115, 13);
      this.label_Click1.TabIndex = 1;
      this.label_Click1.Text = "<Click here to browse>";
      // 
      // label1
      // 
      this.label1.Dock = System.Windows.Forms.DockStyle.Top;
      this.label1.Location = new System.Drawing.Point(0, 0);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(369, 35);
      this.label1.TabIndex = 0;
      this.label1.Text = "Original image:\r\n(screenshot taken from the engine without tonemapping applied)";
      this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
      // 
      // label_Click2
      // 
      this.label_Click2.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.label_Click2.AutoSize = true;
      this.label_Click2.Location = new System.Drawing.Point(135, 202);
      this.label_Click2.Name = "label_Click2";
      this.label_Click2.Size = new System.Drawing.Size(115, 13);
      this.label_Click2.TabIndex = 1;
      this.label_Click2.Text = "<Click here to browse>";
      // 
      // pictureBox_Target
      // 
      this.pictureBox_Target.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
      this.pictureBox_Target.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
      this.pictureBox_Target.Dock = System.Windows.Forms.DockStyle.Fill;
      this.pictureBox_Target.Location = new System.Drawing.Point(0, 35);
      this.pictureBox_Target.Name = "pictureBox_Target";
      this.pictureBox_Target.Size = new System.Drawing.Size(371, 400);
      this.pictureBox_Target.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
      this.pictureBox_Target.TabIndex = 3;
      this.pictureBox_Target.TabStop = false;
      this.pictureBox_Target.Click += new System.EventHandler(this.pictureBox_Target_Click);
      // 
      // label2
      // 
      this.label2.Dock = System.Windows.Forms.DockStyle.Top;
      this.label2.Location = new System.Drawing.Point(0, 0);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(371, 35);
      this.label2.TabIndex = 0;
      this.label2.Text = "How the artist wants it:\r\n(tweaked version of the exact same screenshot)";
      this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
      // 
      // panel1
      // 
      this.panel1.Controls.Add(this.label3);
      this.panel1.Controls.Add(this.button_Close);
      this.panel1.Controls.Add(this.button_Save);
      this.panel1.Controls.Add(this.comboBox_Size);
      this.panel1.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.panel1.Location = new System.Drawing.Point(0, 485);
      this.panel1.Name = "panel1";
      this.panel1.Size = new System.Drawing.Size(744, 98);
      this.panel1.TabIndex = 1;
      // 
      // label3
      // 
      this.label3.AutoSize = true;
      this.label3.Location = new System.Drawing.Point(49, 22);
      this.label3.Name = "label3";
      this.label3.Size = new System.Drawing.Size(219, 13);
      this.label3.TabIndex = 0;
      this.label3.Text = "Dimension of the table (32 is recommended) :";
      this.label3.TextAlign = System.Drawing.ContentAlignment.TopRight;
      // 
      // button_Close
      // 
      this.button_Close.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
      this.button_Close.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.button_Close.Location = new System.Drawing.Point(324, 59);
      this.button_Close.Name = "button_Close";
      this.button_Close.Size = new System.Drawing.Size(93, 27);
      this.button_Close.TabIndex = 3;
      this.button_Close.Text = "Close";
      this.button_Close.UseVisualStyleBackColor = true;
      // 
      // button_Save
      // 
      this.button_Save.Location = new System.Drawing.Point(398, 17);
      this.button_Save.Name = "button_Save";
      this.button_Save.Size = new System.Drawing.Size(93, 23);
      this.button_Save.TabIndex = 2;
      this.button_Save.Text = "Save table!";
      this.button_Save.UseVisualStyleBackColor = true;
      this.button_Save.Click += new System.EventHandler(this.button_Save_Click);
      // 
      // comboBox_Size
      // 
      this.comboBox_Size.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.comboBox_Size.FormattingEnabled = true;
      this.comboBox_Size.Items.AddRange(new object[] {
            "16",
            "32",
            "64",
            "128",
            "256"});
      this.comboBox_Size.Location = new System.Drawing.Point(286, 19);
      this.comboBox_Size.Name = "comboBox_Size";
      this.comboBox_Size.Size = new System.Drawing.Size(92, 21);
      this.comboBox_Size.TabIndex = 1;
      // 
      // ColorGradingToolDlg
      // 
      this.AcceptButton = this.button_Close;
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.CancelButton = this.button_Close;
      this.ClientSize = new System.Drawing.Size(744, 583);
      this.Controls.Add(this.splitContainer1);
      this.Controls.Add(this.dialogCaptionBar);
      this.Controls.Add(this.panel1);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.Name = "ColorGradingToolDlg";
      this.Text = "Color Grading table generation Tool";
      ((System.ComponentModel.ISupportInitialize)(this.pictureBox_Orig)).EndInit();
      this.splitContainer1.Panel1.ResumeLayout(false);
      this.splitContainer1.Panel1.PerformLayout();
      this.splitContainer1.Panel2.ResumeLayout(false);
      this.splitContainer1.Panel2.PerformLayout();
      this.splitContainer1.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.pictureBox_Target)).EndInit();
      this.panel1.ResumeLayout(false);
      this.panel1.PerformLayout();
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.PictureBox pictureBox_Orig;
    private CSharpFramework.Controls.DialogCaptionBar dialogCaptionBar;
    private System.Windows.Forms.SplitContainer splitContainer1;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.PictureBox pictureBox_Target;
    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.Panel panel1;
    private System.Windows.Forms.Button button_Close;
    private System.Windows.Forms.Button button_Save;
    private System.Windows.Forms.ComboBox comboBox_Size;
    private System.Windows.Forms.Label label3;
    private System.Windows.Forms.Label label_Click1;
    private System.Windows.Forms.Label label_Click2;
  }
}