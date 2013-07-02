namespace TerrainEditorPlugin.Controls
{
  partial class BrushListCtrl
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

    #region Component Designer generated code

    /// <summary> 
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.listView = new System.Windows.Forms.ListView();
      this.propertyGrid_Cursor = new CSharpFramework.Controls.PropertyGridBase();
      this.splitContainer1 = new System.Windows.Forms.SplitContainer();
      this.splitContainer1.Panel1.SuspendLayout();
      this.splitContainer1.Panel2.SuspendLayout();
      this.splitContainer1.SuspendLayout();
      this.SuspendLayout();
      // 
      // listView
      // 
      this.listView.AllowDrop = true;
      this.listView.Dock = System.Windows.Forms.DockStyle.Fill;
      this.listView.HideSelection = false;
      this.listView.Location = new System.Drawing.Point(0, 0);
      this.listView.MultiSelect = false;
      this.listView.Name = "listView";
      this.listView.Size = new System.Drawing.Size(343, 290);
      this.listView.TabIndex = 0;
      this.listView.UseCompatibleStateImageBehavior = false;
      this.listView.View = System.Windows.Forms.View.Tile;
      this.listView.SelectedIndexChanged += new System.EventHandler(this.listView_SelectedIndexChanged);
      // 
      // propertyGrid_Cursor
      // 
      this.propertyGrid_Cursor.Dock = System.Windows.Forms.DockStyle.Fill;
      this.propertyGrid_Cursor.Location = new System.Drawing.Point(0, 0);
      this.propertyGrid_Cursor.Name = "propertyGrid_Cursor";
      this.propertyGrid_Cursor.Size = new System.Drawing.Size(343, 211);
      this.propertyGrid_Cursor.TabIndex = 0;
      this.propertyGrid_Cursor.ToolbarVisible = false;
      this.propertyGrid_Cursor.Click += new System.EventHandler(this.propertyGrid_Cursor_Click);
      // 
      // splitContainer1
      // 
      this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.splitContainer1.Location = new System.Drawing.Point(0, 0);
      this.splitContainer1.Name = "splitContainer1";
      this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
      // 
      // splitContainer1.Panel1
      // 
      this.splitContainer1.Panel1.Controls.Add(this.propertyGrid_Cursor);
      // 
      // splitContainer1.Panel2
      // 
      this.splitContainer1.Panel2.Controls.Add(this.listView);
      this.splitContainer1.Size = new System.Drawing.Size(343, 505);
      this.splitContainer1.SplitterDistance = 211;
      this.splitContainer1.TabIndex = 2;
      // 
      // BrushListCtrl
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.Controls.Add(this.splitContainer1);
      this.Name = "BrushListCtrl";
      this.Size = new System.Drawing.Size(343, 505);
      this.splitContainer1.Panel1.ResumeLayout(false);
      this.splitContainer1.Panel2.ResumeLayout(false);
      this.splitContainer1.ResumeLayout(false);
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.ListView listView;
    private CSharpFramework.Controls.PropertyGridBase propertyGrid_Cursor;
    private System.Windows.Forms.SplitContainer splitContainer1;
  }
}
