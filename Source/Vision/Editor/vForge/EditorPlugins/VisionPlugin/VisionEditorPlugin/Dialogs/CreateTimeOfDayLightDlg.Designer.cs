namespace VisionEditorPlugin.Dialogs
{
  partial class CreateTimeOfDayLightDlg
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(CreateTimeOfDayLightDlg));
      this.button_OK = new System.Windows.Forms.Button();
      this.button_Cancel = new System.Windows.Forms.Button();
      this.dialogCaptionBar = new CSharpFramework.Controls.DialogCaptionBar();
      this.checkBox_Shadows = new System.Windows.Forms.CheckBox();
      this.label1 = new System.Windows.Forms.Label();
      this.checkBox_Backlight = new System.Windows.Forms.CheckBox();
      this.label2 = new System.Windows.Forms.Label();
      this.SuspendLayout();
      // 
      // button_OK
      // 
      this.button_OK.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.button_OK.Location = new System.Drawing.Point(250, 265);
      this.button_OK.Name = "button_OK";
      this.button_OK.Size = new System.Drawing.Size(89, 30);
      this.button_OK.TabIndex = 6;
      this.button_OK.Text = "OK";
      this.button_OK.UseVisualStyleBackColor = true;
      this.button_OK.Click += new System.EventHandler(this.button_OK_Click);
      // 
      // button_Cancel
      // 
      this.button_Cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.button_Cancel.Location = new System.Drawing.Point(147, 265);
      this.button_Cancel.Name = "button_Cancel";
      this.button_Cancel.Size = new System.Drawing.Size(89, 30);
      this.button_Cancel.TabIndex = 5;
      this.button_Cancel.Text = "Cancel";
      this.button_Cancel.UseVisualStyleBackColor = true;
      // 
      // dialogCaptionBar
      // 
      this.dialogCaptionBar.BackColor = System.Drawing.SystemColors.Window;
      this.dialogCaptionBar.Caption = "Create Time of Day Sunlight";
      this.dialogCaptionBar.CompactView = false;
      this.dialogCaptionBar.Description = "Select the features that the sunlight should have. All settings can be changed la" +
          "ter by adding, removing or modifying components attached to the light";
      this.dialogCaptionBar.Dock = System.Windows.Forms.DockStyle.Top;
      this.dialogCaptionBar.Image = ((System.Drawing.Image)(resources.GetObject("dialogCaptionBar.Image")));
      this.dialogCaptionBar.Location = new System.Drawing.Point(0, 0);
      this.dialogCaptionBar.Name = "dialogCaptionBar";
      this.dialogCaptionBar.SetFontColor = System.Drawing.SystemColors.ControlText;
      this.dialogCaptionBar.ShowBorder = false;
      this.dialogCaptionBar.ShowBottomLine = true;
      this.dialogCaptionBar.ShowCaptionText = true;
      this.dialogCaptionBar.ShowImage = true;
      this.dialogCaptionBar.Size = new System.Drawing.Size(492, 50);
      this.dialogCaptionBar.TabIndex = 0;
      // 
      // checkBox_Shadows
      // 
      this.checkBox_Shadows.AutoSize = true;
      this.checkBox_Shadows.Checked = true;
      this.checkBox_Shadows.CheckState = System.Windows.Forms.CheckState.Checked;
      this.checkBox_Shadows.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.checkBox_Shadows.Location = new System.Drawing.Point(43, 77);
      this.checkBox_Shadows.Name = "checkBox_Shadows";
      this.checkBox_Shadows.Size = new System.Drawing.Size(129, 17);
      this.checkBox_Shadows.TabIndex = 1;
      this.checkBox_Shadows.Text = "Dynamic Shadows\r\n";
      this.checkBox_Shadows.UseVisualStyleBackColor = true;
      // 
      // label1
      // 
      this.label1.Location = new System.Drawing.Point(73, 97);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(395, 50);
      this.label1.TabIndex = 2;
      this.label1.Text = "If enabled, a shadowmap component is added to the sunlight so that it casts dynam" +
          "ic shadows. Shadow properties (resolution etc.) can be changed on that component" +
          " later in the component layout.";
      // 
      // checkBox_Backlight
      // 
      this.checkBox_Backlight.AutoSize = true;
      this.checkBox_Backlight.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.checkBox_Backlight.Location = new System.Drawing.Point(43, 150);
      this.checkBox_Backlight.Name = "checkBox_Backlight";
      this.checkBox_Backlight.Size = new System.Drawing.Size(87, 17);
      this.checkBox_Backlight.TabIndex = 3;
      this.checkBox_Backlight.Text = "Back Light";
      this.checkBox_Backlight.UseVisualStyleBackColor = true;
      // 
      // label2
      // 
      this.label2.Location = new System.Drawing.Point(73, 170);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(395, 50);
      this.label2.TabIndex = 4;
      this.label2.Text = "If enabled, a second light is added that shines into the opposite direction of th" +
          "e sun incident angle.";
      // 
      // CreateTimeOfDayLightDlg
      // 
      this.AcceptButton = this.button_OK;
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.CancelButton = this.button_Cancel;
      this.ClientSize = new System.Drawing.Size(492, 307);
      this.Controls.Add(this.label2);
      this.Controls.Add(this.checkBox_Backlight);
      this.Controls.Add(this.label1);
      this.Controls.Add(this.checkBox_Shadows);
      this.Controls.Add(this.dialogCaptionBar);
      this.Controls.Add(this.button_Cancel);
      this.Controls.Add(this.button_OK);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "CreateTimeOfDayLightDlg";
      this.Text = "Create Time of Day Sunlight";
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.Button button_OK;
    private System.Windows.Forms.Button button_Cancel;
    private CSharpFramework.Controls.DialogCaptionBar dialogCaptionBar;
    private System.Windows.Forms.CheckBox checkBox_Shadows;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.CheckBox checkBox_Backlight;
    private System.Windows.Forms.Label label2;
  }
}