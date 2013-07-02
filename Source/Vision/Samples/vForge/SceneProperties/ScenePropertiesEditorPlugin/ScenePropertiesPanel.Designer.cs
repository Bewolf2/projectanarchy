namespace ScenePropertiesPlugin
{
  partial class ScenePropertiesPanel
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
      this.groupBox1 = new System.Windows.Forms.GroupBox();
      this.respawnRateUpDown = new System.Windows.Forms.NumericUpDown();
      this.label5 = new System.Windows.Forms.Label();
      this.respawnTimeUpDown = new System.Windows.Forms.NumericUpDown();
      this.label4 = new System.Windows.Forms.Label();
      this.label3 = new System.Windows.Forms.Label();
      this.label2 = new System.Windows.Forms.Label();
      this.difficultyTrackbar = new System.Windows.Forms.TrackBar();
      this.groupBox2 = new System.Windows.Forms.GroupBox();
      this.descriptionBox = new System.Windows.Forms.RichTextBox();
      this.label1 = new System.Windows.Forms.Label();
      this.groupBox1.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.respawnRateUpDown)).BeginInit();
      ((System.ComponentModel.ISupportInitialize)(this.respawnTimeUpDown)).BeginInit();
      ((System.ComponentModel.ISupportInitialize)(this.difficultyTrackbar)).BeginInit();
      this.groupBox2.SuspendLayout();
      this.SuspendLayout();
      // 
      // groupBox1
      // 
      this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.groupBox1.Controls.Add(this.respawnRateUpDown);
      this.groupBox1.Controls.Add(this.label5);
      this.groupBox1.Controls.Add(this.respawnTimeUpDown);
      this.groupBox1.Controls.Add(this.label4);
      this.groupBox1.Controls.Add(this.label3);
      this.groupBox1.Controls.Add(this.label2);
      this.groupBox1.Controls.Add(this.difficultyTrackbar);
      this.groupBox1.Location = new System.Drawing.Point(12, 58);
      this.groupBox1.Name = "groupBox1";
      this.groupBox1.Size = new System.Drawing.Size(233, 145);
      this.groupBox1.TabIndex = 10;
      this.groupBox1.TabStop = false;
      this.groupBox1.Text = "Enemy Configuration";
      // 
      // respawnRateUpDown
      // 
      this.respawnRateUpDown.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.respawnRateUpDown.Location = new System.Drawing.Point(146, 110);
      this.respawnRateUpDown.Name = "respawnRateUpDown";
      this.respawnRateUpDown.Size = new System.Drawing.Size(76, 20);
      this.respawnRateUpDown.TabIndex = 15;
      this.respawnRateUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
      this.respawnRateUpDown.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
      // 
      // label5
      // 
      this.label5.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.label5.AutoSize = true;
      this.label5.Location = new System.Drawing.Point(144, 94);
      this.label5.Name = "label5";
      this.label5.Size = new System.Drawing.Size(78, 13);
      this.label5.TabIndex = 14;
      this.label5.Text = "Respawn Rate";
      // 
      // respawnTimeUpDown
      // 
      this.respawnTimeUpDown.Location = new System.Drawing.Point(9, 110);
      this.respawnTimeUpDown.Maximum = new decimal(new int[] {
            60,
            0,
            0,
            0});
      this.respawnTimeUpDown.Name = "respawnTimeUpDown";
      this.respawnTimeUpDown.Size = new System.Drawing.Size(74, 20);
      this.respawnTimeUpDown.TabIndex = 13;
      this.respawnTimeUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
      // 
      // label4
      // 
      this.label4.AutoSize = true;
      this.label4.Location = new System.Drawing.Point(6, 94);
      this.label4.Name = "label4";
      this.label4.Size = new System.Drawing.Size(78, 13);
      this.label4.TabIndex = 12;
      this.label4.Text = "Respawn Time";
      // 
      // label3
      // 
      this.label3.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.label3.AutoSize = true;
      this.label3.Location = new System.Drawing.Point(197, 30);
      this.label3.Name = "label3";
      this.label3.Size = new System.Drawing.Size(30, 13);
      this.label3.TabIndex = 11;
      this.label3.Text = "Hard";
      // 
      // label2
      // 
      this.label2.AutoSize = true;
      this.label2.Location = new System.Drawing.Point(6, 30);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(30, 13);
      this.label2.TabIndex = 10;
      this.label2.Text = "Easy";
      // 
      // difficultyTrackbar
      // 
      this.difficultyTrackbar.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.difficultyTrackbar.Location = new System.Drawing.Point(9, 46);
      this.difficultyTrackbar.Name = "difficultyTrackbar";
      this.difficultyTrackbar.Size = new System.Drawing.Size(213, 45);
      this.difficultyTrackbar.TabIndex = 9;
      // 
      // groupBox2
      // 
      this.groupBox2.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.groupBox2.Controls.Add(this.descriptionBox);
      this.groupBox2.Location = new System.Drawing.Point(12, 214);
      this.groupBox2.Name = "groupBox2";
      this.groupBox2.Size = new System.Drawing.Size(233, 215);
      this.groupBox2.TabIndex = 11;
      this.groupBox2.TabStop = false;
      this.groupBox2.Text = "Level Description";
      // 
      // descriptionBox
      // 
      this.descriptionBox.AcceptsTab = true;
      this.descriptionBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.descriptionBox.AutoWordSelection = true;
      this.descriptionBox.BulletIndent = 2;
      this.descriptionBox.Location = new System.Drawing.Point(9, 19);
      this.descriptionBox.Name = "descriptionBox";
      this.descriptionBox.Size = new System.Drawing.Size(213, 190);
      this.descriptionBox.TabIndex = 10;
      this.descriptionBox.Text = "";
      // 
      // label1
      // 
      this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.label1.BackColor = System.Drawing.Color.White;
      this.label1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.label1.Location = new System.Drawing.Point(12, 9);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(233, 42);
      this.label1.TabIndex = 12;
      this.label1.Text = "Scene Properties\r\n(sample plugin for vForge)";
      this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
      // 
      // ScenePropertiesPanel
      // 
      this.ClientSize = new System.Drawing.Size(257, 441);
      this.Controls.Add(this.label1);
      this.Controls.Add(this.groupBox2);
      this.Controls.Add(this.groupBox1);
      this.Name = "ScenePropertiesPanel";
      this.TabText = "Scene Properties Sample";
      this.Text = "Scene Properties Sample";
      this.groupBox1.ResumeLayout(false);
      this.groupBox1.PerformLayout();
      ((System.ComponentModel.ISupportInitialize)(this.respawnRateUpDown)).EndInit();
      ((System.ComponentModel.ISupportInitialize)(this.respawnTimeUpDown)).EndInit();
      ((System.ComponentModel.ISupportInitialize)(this.difficultyTrackbar)).EndInit();
      this.groupBox2.ResumeLayout(false);
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.GroupBox groupBox1;
    protected System.Windows.Forms.NumericUpDown respawnRateUpDown;
    private System.Windows.Forms.Label label5;
    protected System.Windows.Forms.NumericUpDown respawnTimeUpDown;
    private System.Windows.Forms.Label label4;
    private System.Windows.Forms.Label label3;
    private System.Windows.Forms.Label label2;
    protected System.Windows.Forms.TrackBar difficultyTrackbar;
    private System.Windows.Forms.GroupBox groupBox2;
    protected System.Windows.Forms.RichTextBox descriptionBox;
    private System.Windows.Forms.Label label1;
  }
}