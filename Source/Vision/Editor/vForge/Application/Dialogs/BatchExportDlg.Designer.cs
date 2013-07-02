namespace Editor.Dialogs
{
  partial class BatchExportDlg
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(BatchExportDlg));
      this.buttonRun = new System.Windows.Forms.Button();
      this.buttonCancel = new System.Windows.Forms.Button();
      this.checkBoxResaveScene = new System.Windows.Forms.CheckBox();
      this.groupBoxSettings = new System.Windows.Forms.GroupBox();
      this.label2 = new System.Windows.Forms.Label();
      this.radioMismatch_Ignore = new System.Windows.Forms.RadioButton();
      this.radioMismatch_Dialog = new System.Windows.Forms.RadioButton();
      this.checkBoxMoveToCamera = new System.Windows.Forms.CheckBox();
      this.radioMismatch_Revert = new System.Windows.Forms.RadioButton();
      this.label1 = new System.Windows.Forms.Label();
      this.textBoxDirectory = new System.Windows.Forms.TextBox();
      this.buttonDirectory = new System.Windows.Forms.Button();
      this.progressBar = new System.Windows.Forms.ProgressBar();
      this.statusString = new System.Windows.Forms.Label();
      this.textBoxLog = new System.Windows.Forms.TextBox();
      this.groupBoxSettings.SuspendLayout();
      this.SuspendLayout();
      // 
      // buttonRun
      // 
      this.buttonRun.Location = new System.Drawing.Point(557, 409);
      this.buttonRun.Name = "buttonRun";
      this.buttonRun.Size = new System.Drawing.Size(75, 23);
      this.buttonRun.TabIndex = 7;
      this.buttonRun.Text = "Run";
      this.buttonRun.UseVisualStyleBackColor = true;
      this.buttonRun.Click += new System.EventHandler(this.buttonRun_Click);
      // 
      // buttonCancel
      // 
      this.buttonCancel.Location = new System.Drawing.Point(638, 409);
      this.buttonCancel.Name = "buttonCancel";
      this.buttonCancel.Size = new System.Drawing.Size(75, 23);
      this.buttonCancel.TabIndex = 8;
      this.buttonCancel.Text = "Cancel";
      this.buttonCancel.UseVisualStyleBackColor = true;
      this.buttonCancel.Click += new System.EventHandler(this.buttonCancel_Click);
      // 
      // checkBoxResaveScene
      // 
      this.checkBoxResaveScene.AutoSize = true;
      this.checkBoxResaveScene.Location = new System.Drawing.Point(6, 57);
      this.checkBoxResaveScene.Name = "checkBoxResaveScene";
      this.checkBoxResaveScene.Size = new System.Drawing.Size(116, 17);
      this.checkBoxResaveScene.TabIndex = 0;
      this.checkBoxResaveScene.Text = "Resave scene files";
      this.checkBoxResaveScene.UseVisualStyleBackColor = true;
      // 
      // groupBoxSettings
      // 
      this.groupBoxSettings.Controls.Add(this.label2);
      this.groupBoxSettings.Controls.Add(this.radioMismatch_Ignore);
      this.groupBoxSettings.Controls.Add(this.radioMismatch_Dialog);
      this.groupBoxSettings.Controls.Add(this.checkBoxMoveToCamera);
      this.groupBoxSettings.Controls.Add(this.radioMismatch_Revert);
      this.groupBoxSettings.Controls.Add(this.checkBoxResaveScene);
      this.groupBoxSettings.Location = new System.Drawing.Point(12, 41);
      this.groupBoxSettings.Name = "groupBoxSettings";
      this.groupBoxSettings.Size = new System.Drawing.Size(701, 80);
      this.groupBoxSettings.TabIndex = 4;
      this.groupBoxSettings.TabStop = false;
      this.groupBoxSettings.Text = "Export options";
      // 
      // label2
      // 
      this.label2.AutoSize = true;
      this.label2.Location = new System.Drawing.Point(6, 20);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(178, 13);
      this.label2.TabIndex = 4;
      this.label2.Text = "On .scene/.vscene name mismatch:";
      // 
      // radioMismatch_Ignore
      // 
      this.radioMismatch_Ignore.AutoSize = true;
      this.radioMismatch_Ignore.Location = new System.Drawing.Point(191, 19);
      this.radioMismatch_Ignore.Name = "radioMismatch_Ignore";
      this.radioMismatch_Ignore.Size = new System.Drawing.Size(55, 17);
      this.radioMismatch_Ignore.TabIndex = 1;
      this.radioMismatch_Ignore.Text = "Ignore";
      this.radioMismatch_Ignore.UseVisualStyleBackColor = true;
      // 
      // radioMismatch_Dialog
      // 
      this.radioMismatch_Dialog.AutoSize = true;
      this.radioMismatch_Dialog.Checked = true;
      this.radioMismatch_Dialog.Location = new System.Drawing.Point(362, 20);
      this.radioMismatch_Dialog.Name = "radioMismatch_Dialog";
      this.radioMismatch_Dialog.Size = new System.Drawing.Size(114, 17);
      this.radioMismatch_Dialog.TabIndex = 3;
      this.radioMismatch_Dialog.TabStop = true;
      this.radioMismatch_Dialog.Text = "Open export dialog";
      this.radioMismatch_Dialog.UseVisualStyleBackColor = true;
      // 
      // checkBoxMoveToCamera
      // 
      this.checkBoxMoveToCamera.AutoSize = true;
      this.checkBoxMoveToCamera.Location = new System.Drawing.Point(6, 36);
      this.checkBoxMoveToCamera.Name = "checkBoxMoveToCamera";
      this.checkBoxMoveToCamera.Size = new System.Drawing.Size(103, 17);
      this.checkBoxMoveToCamera.TabIndex = 5;
      this.checkBoxMoveToCamera.Text = "Move to camera";
      this.checkBoxMoveToCamera.UseVisualStyleBackColor = true;
      this.checkBoxMoveToCamera.Visible = false;
      // 
      // radioMismatch_Revert
      // 
      this.radioMismatch_Revert.AutoSize = true;
      this.radioMismatch_Revert.Location = new System.Drawing.Point(252, 20);
      this.radioMismatch_Revert.Name = "radioMismatch_Revert";
      this.radioMismatch_Revert.Size = new System.Drawing.Size(104, 17);
      this.radioMismatch_Revert.TabIndex = 2;
      this.radioMismatch_Revert.Text = "Revert to default";
      this.radioMismatch_Revert.UseVisualStyleBackColor = true;
      // 
      // label1
      // 
      this.label1.AutoSize = true;
      this.label1.Location = new System.Drawing.Point(13, 13);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(84, 13);
      this.label1.TabIndex = 9;
      this.label1.Text = "Search directory";
      // 
      // textBoxDirectory
      // 
      this.textBoxDirectory.Location = new System.Drawing.Point(104, 12);
      this.textBoxDirectory.Name = "textBoxDirectory";
      this.textBoxDirectory.Size = new System.Drawing.Size(557, 20);
      this.textBoxDirectory.TabIndex = 10;
      this.textBoxDirectory.TextChanged += new System.EventHandler(this.textBoxDirectory_TextChanged);
      // 
      // buttonDirectory
      // 
      this.buttonDirectory.Location = new System.Drawing.Point(667, 10);
      this.buttonDirectory.Name = "buttonDirectory";
      this.buttonDirectory.Size = new System.Drawing.Size(46, 23);
      this.buttonDirectory.TabIndex = 11;
      this.buttonDirectory.Text = "...";
      this.buttonDirectory.UseVisualStyleBackColor = true;
      this.buttonDirectory.Click += new System.EventHandler(this.buttonDirectory_Click);
      // 
      // progressBar
      // 
      this.progressBar.Location = new System.Drawing.Point(12, 438);
      this.progressBar.Name = "progressBar";
      this.progressBar.Size = new System.Drawing.Size(701, 28);
      this.progressBar.TabIndex = 12;
      // 
      // statusString
      // 
      this.statusString.AutoSize = true;
      this.statusString.Location = new System.Drawing.Point(18, 419);
      this.statusString.Name = "statusString";
      this.statusString.Size = new System.Drawing.Size(0, 13);
      this.statusString.TabIndex = 13;
      // 
      // textBoxLog
      // 
      this.textBoxLog.Location = new System.Drawing.Point(12, 135);
      this.textBoxLog.Multiline = true;
      this.textBoxLog.Name = "textBoxLog";
      this.textBoxLog.ReadOnly = true;
      this.textBoxLog.ScrollBars = System.Windows.Forms.ScrollBars.Both;
      this.textBoxLog.Size = new System.Drawing.Size(701, 261);
      this.textBoxLog.TabIndex = 14;
      this.textBoxLog.WordWrap = false;
      // 
      // BatchExportDlg
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(725, 478);
      this.Controls.Add(this.textBoxLog);
      this.Controls.Add(this.statusString);
      this.Controls.Add(this.progressBar);
      this.Controls.Add(this.buttonDirectory);
      this.Controls.Add(this.textBoxDirectory);
      this.Controls.Add(this.label1);
      this.Controls.Add(this.buttonCancel);
      this.Controls.Add(this.groupBoxSettings);
      this.Controls.Add(this.buttonRun);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "BatchExportDlg";
      this.Text = "Batch export";
      this.Load += new System.EventHandler(this.BatchExportDlg_Load);
      this.groupBoxSettings.ResumeLayout(false);
      this.groupBoxSettings.PerformLayout();
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.Button buttonRun;
    private System.Windows.Forms.Button buttonCancel;
    private System.Windows.Forms.CheckBox checkBoxResaveScene;
    private System.Windows.Forms.GroupBox groupBoxSettings;
    private System.Windows.Forms.RadioButton radioMismatch_Ignore;
    private System.Windows.Forms.RadioButton radioMismatch_Dialog;
    private System.Windows.Forms.RadioButton radioMismatch_Revert;
    private System.Windows.Forms.CheckBox checkBoxMoveToCamera;
    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.TextBox textBoxDirectory;
    private System.Windows.Forms.Button buttonDirectory;
    private System.Windows.Forms.ProgressBar progressBar;
    private System.Windows.Forms.Label statusString;
    private System.Windows.Forms.TextBox textBoxLog;
  }
}