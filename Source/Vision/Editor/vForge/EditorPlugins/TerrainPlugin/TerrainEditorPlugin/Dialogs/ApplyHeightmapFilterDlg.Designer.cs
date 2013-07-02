using CSharpFramework.Controls;
namespace TerrainEditorPlugin.Dialogs
{
  partial class ApplyHeightmapFilterDlg
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ApplyHeightmapFilterDlg));
      this.propertyGrid_Filter = new CSharpFramework.Controls.PropertyGridNested();
      this.button_Apply = new System.Windows.Forms.Button();
      this.button_Cancel = new System.Windows.Forms.Button();
      this.dialogCaptionBar1 = new CSharpFramework.Controls.DialogCaptionBar();
      this.panel1 = new System.Windows.Forms.Panel();
      this.progressBar = new System.Windows.Forms.ProgressBar();
      this.panel1.SuspendLayout();
      this.SuspendLayout();
      // 
      // propertyGrid_Filter
      // 
      this.propertyGrid_Filter.Dock = System.Windows.Forms.DockStyle.Fill;
      this.propertyGrid_Filter.Location = new System.Drawing.Point(0, 50);
      this.propertyGrid_Filter.Name = "propertyGrid_Filter";
      this.propertyGrid_Filter.Size = new System.Drawing.Size(429, 405);
      this.propertyGrid_Filter.TabIndex = 1;
      // 
      // button_Apply
      // 
      this.button_Apply.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
      this.button_Apply.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.button_Apply.Location = new System.Drawing.Point(120, 14);
      this.button_Apply.Name = "button_Apply";
      this.button_Apply.Size = new System.Drawing.Size(83, 25);
      this.button_Apply.TabIndex = 0;
      this.button_Apply.Text = "Apply";
      this.button_Apply.UseVisualStyleBackColor = true;
      this.button_Apply.Click += new System.EventHandler(this.button_Apply_Click);
      // 
      // button_Cancel
      // 
      this.button_Cancel.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
      this.button_Cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.button_Cancel.Location = new System.Drawing.Point(229, 14);
      this.button_Cancel.Name = "button_Cancel";
      this.button_Cancel.Size = new System.Drawing.Size(83, 25);
      this.button_Cancel.TabIndex = 1;
      this.button_Cancel.Text = "Cancel";
      this.button_Cancel.UseVisualStyleBackColor = true;
      // 
      // dialogCaptionBar1
      // 
      this.dialogCaptionBar1.BackColor = System.Drawing.SystemColors.Window;
      this.dialogCaptionBar1.Caption = "Heightmap Filter";
      this.dialogCaptionBar1.CompactView = false;
      this.dialogCaptionBar1.Description = "Please select filter options and click on Apply to run filter";
      this.dialogCaptionBar1.Dock = System.Windows.Forms.DockStyle.Top;
      this.dialogCaptionBar1.Image = ((System.Drawing.Image)(resources.GetObject("dialogCaptionBar1.Image")));
      this.dialogCaptionBar1.Location = new System.Drawing.Point(0, 0);
      this.dialogCaptionBar1.Name = "dialogCaptionBar1";
      this.dialogCaptionBar1.SetFontColor = System.Drawing.SystemColors.ControlText;
      this.dialogCaptionBar1.ShowBorder = false;
      this.dialogCaptionBar1.ShowBottomLine = true;
      this.dialogCaptionBar1.ShowCaptionText = true;
      this.dialogCaptionBar1.ShowImage = true;
      this.dialogCaptionBar1.Size = new System.Drawing.Size(429, 50);
      this.dialogCaptionBar1.TabIndex = 0;
      // 
      // panel1
      // 
      this.panel1.Controls.Add(this.button_Apply);
      this.panel1.Controls.Add(this.button_Cancel);
      this.panel1.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.panel1.Location = new System.Drawing.Point(0, 480);
      this.panel1.Name = "panel1";
      this.panel1.Size = new System.Drawing.Size(429, 51);
      this.panel1.TabIndex = 3;
      // 
      // progressBar
      // 
      this.progressBar.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.progressBar.Location = new System.Drawing.Point(0, 455);
      this.progressBar.Name = "progressBar";
      this.progressBar.Size = new System.Drawing.Size(429, 25);
      this.progressBar.Step = 1;
      this.progressBar.TabIndex = 2;
      // 
      // ApplyHeightmapFilterDlg
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(429, 531);
      this.Controls.Add(this.propertyGrid_Filter);
      this.Controls.Add(this.progressBar);
      this.Controls.Add(this.panel1);
      this.Controls.Add(this.dialogCaptionBar1);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow;
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "ApplyHeightmapFilterDlg";
      this.Text = "Heightmap Filter";
      this.panel1.ResumeLayout(false);
      this.ResumeLayout(false);

    }

    #endregion

    private CSharpFramework.Controls.PropertyGridNested propertyGrid_Filter;
    private System.Windows.Forms.Button button_Apply;
    private System.Windows.Forms.Button button_Cancel;
    private CSharpFramework.Controls.DialogCaptionBar dialogCaptionBar1;
    private System.Windows.Forms.Panel panel1;
    private System.Windows.Forms.ProgressBar progressBar;
  }
}