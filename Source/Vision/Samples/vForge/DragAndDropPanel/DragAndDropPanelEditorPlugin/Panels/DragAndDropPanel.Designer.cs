namespace DragAndDropPanelPlugin.Panels
{
  partial class DragAndDropPanel
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
      this.components = new System.ComponentModel.Container();
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(DragAndDropPanel));
      this.listViewEntities = new System.Windows.Forms.ListView();
      this.imageList = new System.Windows.Forms.ImageList(this.components);
      this.toolStrip = new System.Windows.Forms.ToolStrip();
      this.toolStripButtonModels = new System.Windows.Forms.ToolStripButton();
      this.toolStripButtonMeshes = new System.Windows.Forms.ToolStripButton();
      this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
      this.toolStripTextBoxFilter = new System.Windows.Forms.ToolStripTextBox();
      this.toolStripButtonClearText = new System.Windows.Forms.ToolStripButton();
      this.toolStrip.SuspendLayout();
      this.SuspendLayout();
      // 
      // listViewEntities
      // 
      this.listViewEntities.Dock = System.Windows.Forms.DockStyle.Fill;
      this.listViewEntities.LargeImageList = this.imageList;
      this.listViewEntities.Location = new System.Drawing.Point(0, 25);
      this.listViewEntities.MultiSelect = false;
      this.listViewEntities.Name = "listViewEntities";
      this.listViewEntities.ShowItemToolTips = true;
      this.listViewEntities.Size = new System.Drawing.Size(709, 241);
      this.listViewEntities.Sorting = System.Windows.Forms.SortOrder.Ascending;
      this.listViewEntities.TabIndex = 0;
      this.listViewEntities.UseCompatibleStateImageBehavior = false;
      // 
      // imageList
      // 
      this.imageList.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList.ImageStream")));
      this.imageList.TransparentColor = System.Drawing.Color.Transparent;
      this.imageList.Images.SetKeyName(0, "<NoThumbnail>");
      // 
      // toolStrip
      // 
      this.toolStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButtonModels,
            this.toolStripButtonMeshes,
            this.toolStripSeparator1,
            this.toolStripTextBoxFilter,
            this.toolStripButtonClearText});
      this.toolStrip.Location = new System.Drawing.Point(0, 0);
      this.toolStrip.Name = "toolStrip";
      this.toolStrip.Size = new System.Drawing.Size(709, 25);
      this.toolStrip.TabIndex = 1;
      // 
      // toolStripButtonModels
      // 
      this.toolStripButtonModels.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButtonModels.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonModels.Image")));
      this.toolStripButtonModels.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButtonModels.Name = "toolStripButtonModels";
      this.toolStripButtonModels.Size = new System.Drawing.Size(23, 22);
      this.toolStripButtonModels.Text = "Toggles Models in the List";
      this.toolStripButtonModels.Click += new System.EventHandler(this.toolStripButtonModels_Click);
      // 
      // toolStripButtonMeshes
      // 
      this.toolStripButtonMeshes.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButtonMeshes.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonMeshes.Image")));
      this.toolStripButtonMeshes.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButtonMeshes.Name = "toolStripButtonMeshes";
      this.toolStripButtonMeshes.Size = new System.Drawing.Size(23, 22);
      this.toolStripButtonMeshes.Text = "Toggles Meshes in the List";
      this.toolStripButtonMeshes.Click += new System.EventHandler(this.toolStripButtonMeshes_Click);
      // 
      // toolStripSeparator1
      // 
      this.toolStripSeparator1.Name = "toolStripSeparator1";
      this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
      // 
      // toolStripTextBoxFilter
      // 
      this.toolStripTextBoxFilter.Name = "toolStripTextBoxFilter";
      this.toolStripTextBoxFilter.Size = new System.Drawing.Size(150, 25);
      this.toolStripTextBoxFilter.ToolTipText = "Type To Filter The List";
      this.toolStripTextBoxFilter.TextChanged += new System.EventHandler(this.toolStripTextBoxFilter_TextChanged);
      // 
      // toolStripButtonClearText
      // 
      this.toolStripButtonClearText.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButtonClearText.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonClearText.Image")));
      this.toolStripButtonClearText.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButtonClearText.Name = "toolStripButtonClearText";
      this.toolStripButtonClearText.Size = new System.Drawing.Size(23, 22);
      this.toolStripButtonClearText.Text = "Clear Search Text";
      this.toolStripButtonClearText.Click += new System.EventHandler(this.toolStripButtonClearText_Click);
      // 
      // EntityPanel
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
      this.ClientSize = new System.Drawing.Size(709, 266);
      this.Controls.Add(this.listViewEntities);
      this.Controls.Add(this.toolStrip);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.Name = "EntityPanel";
      this.Text = "Meshes And Models";
      this.toolStrip.ResumeLayout(false);
      this.toolStrip.PerformLayout();
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.ListView listViewEntities;
    private System.Windows.Forms.ImageList imageList;
    private System.Windows.Forms.ToolStrip toolStrip;
    private System.Windows.Forms.ToolStripButton toolStripButtonModels;
    private System.Windows.Forms.ToolStripButton toolStripButtonMeshes;
    private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
    private System.Windows.Forms.ToolStripTextBox toolStripTextBoxFilter;
    private System.Windows.Forms.ToolStripButton toolStripButtonClearText;
  }
}