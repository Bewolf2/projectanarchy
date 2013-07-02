namespace Editor.Dialogs
{
  partial class MoveSceneProjectDlg
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MoveSceneProjectDlg));
      this.button_OK = new System.Windows.Forms.Button();
      this.button_Cancel = new System.Windows.Forms.Button();
      this.textBox_OldProj = new System.Windows.Forms.TextBox();
      this.textBox_newProj = new System.Windows.Forms.TextBox();
      this.button_ProjBrowse = new System.Windows.Forms.Button();
      this.label1 = new System.Windows.Forms.Label();
      this.label2 = new System.Windows.Forms.Label();
      this.captionBar = new CSharpFramework.Controls.DialogCaptionBar();
      this.SuspendLayout();
      // 
      // button_OK
      // 
      this.button_OK.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
      this.button_OK.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.button_OK.Location = new System.Drawing.Point(220, 224);
      this.button_OK.Name = "button_OK";
      this.button_OK.Size = new System.Drawing.Size(91, 31);
      this.button_OK.TabIndex = 6;
      this.button_OK.Text = "OK";
      this.button_OK.UseVisualStyleBackColor = true;
      // 
      // button_Cancel
      // 
      this.button_Cancel.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
      this.button_Cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.button_Cancel.Location = new System.Drawing.Point(325, 224);
      this.button_Cancel.Name = "button_Cancel";
      this.button_Cancel.Size = new System.Drawing.Size(91, 31);
      this.button_Cancel.TabIndex = 7;
      this.button_Cancel.Text = "Cancel";
      this.button_Cancel.UseVisualStyleBackColor = true;
      // 
      // textBox_OldProj
      // 
      this.textBox_OldProj.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.textBox_OldProj.Location = new System.Drawing.Point(12, 85);
      this.textBox_OldProj.Name = "textBox_OldProj";
      this.textBox_OldProj.ReadOnly = true;
      this.textBox_OldProj.Size = new System.Drawing.Size(585, 20);
      this.textBox_OldProj.TabIndex = 2;
      // 
      // textBox_newProj
      // 
      this.textBox_newProj.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.textBox_newProj.Location = new System.Drawing.Point(12, 157);
      this.textBox_newProj.Name = "textBox_newProj";
      this.textBox_newProj.Size = new System.Drawing.Size(585, 20);
      this.textBox_newProj.TabIndex = 4;
      // 
      // button_ProjBrowse
      // 
      this.button_ProjBrowse.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.button_ProjBrowse.Location = new System.Drawing.Point(603, 157);
      this.button_ProjBrowse.Name = "button_ProjBrowse";
      this.button_ProjBrowse.Size = new System.Drawing.Size(25, 20);
      this.button_ProjBrowse.TabIndex = 5;
      this.button_ProjBrowse.Text = "...";
      this.button_ProjBrowse.UseVisualStyleBackColor = true;
      this.button_ProjBrowse.Click += new System.EventHandler(this.button_ProjBrowse_Click);
      // 
      // label1
      // 
      this.label1.AutoSize = true;
      this.label1.Location = new System.Drawing.Point(16, 69);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(106, 13);
      this.label1.TabIndex = 1;
      this.label1.Text = "Current project path :";
      // 
      // label2
      // 
      this.label2.AutoSize = true;
      this.label2.Location = new System.Drawing.Point(16, 141);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(283, 13);
      this.label2.TabIndex = 3;
      this.label2.Text = "New project path (must be a parent folder of current path) :";
      // 
      // captionBar
      // 
      this.captionBar.BackColor = System.Drawing.SystemColors.Window;
      this.captionBar.Caption = "Move Scene Project";
      this.captionBar.CompactView = false;
      this.captionBar.Description = "Makes the scene relative to a new project file. This adjusts all project relative" +
          " file paths that are used in this scene. This operation is not undoable.";
      this.captionBar.Dock = System.Windows.Forms.DockStyle.Top;
      this.captionBar.Image = ((System.Drawing.Image)(resources.GetObject("captionBar.Image")));
      this.captionBar.Location = new System.Drawing.Point(0, 0);
      this.captionBar.Name = "captionBar";
      this.captionBar.SetFontColor = System.Drawing.SystemColors.ControlText;
      this.captionBar.ShowBorder = false;
      this.captionBar.ShowBottomLine = true;
      this.captionBar.ShowCaptionText = true;
      this.captionBar.ShowImage = true;
      this.captionBar.Size = new System.Drawing.Size(640, 50);
      this.captionBar.TabIndex = 0;
      // 
      // MoveSceneProjectDlg
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(640, 266);
      this.Controls.Add(this.label2);
      this.Controls.Add(this.label1);
      this.Controls.Add(this.button_ProjBrowse);
      this.Controls.Add(this.textBox_newProj);
      this.Controls.Add(this.textBox_OldProj);
      this.Controls.Add(this.captionBar);
      this.Controls.Add(this.button_Cancel);
      this.Controls.Add(this.button_OK);
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "MoveSceneProjectDlg";
      this.Text = "Move Scene Project";
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.Button button_OK;
    private System.Windows.Forms.Button button_Cancel;
    private CSharpFramework.Controls.DialogCaptionBar captionBar;
    private System.Windows.Forms.TextBox textBox_OldProj;
    private System.Windows.Forms.TextBox textBox_newProj;
    private System.Windows.Forms.Button button_ProjBrowse;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.Label label2;
  }
}