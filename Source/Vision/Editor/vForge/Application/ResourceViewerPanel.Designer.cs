namespace Editor
{
  partial class ResourceViewerPanel
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ResourceViewerPanel));
      this.resourceMasterPanel = new vResourceViewerBase80.Controls.MasterPanel();
      this.SuspendLayout();
      // 
      // resourceMasterPanel
      // 
      this.resourceMasterPanel.Dock = System.Windows.Forms.DockStyle.Fill;
      this.resourceMasterPanel.Location = new System.Drawing.Point(0, 0);
      this.resourceMasterPanel.Name = "resourceMasterPanel";
      this.resourceMasterPanel.Size = new System.Drawing.Size(580, 636);
      this.resourceMasterPanel.TabIndex = 0;
      // 
      // ResourceViewerPanel
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(580, 636);
      this.Controls.Add(this.resourceMasterPanel);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "ResourceViewerPanel";
      this.TabText = "Resource Viewer";
      this.Text = "Resource Viewer";
      this.ResumeLayout(false);

    }

    #endregion

    private vResourceViewerBase80.Controls.MasterPanel resourceMasterPanel;

  }
}