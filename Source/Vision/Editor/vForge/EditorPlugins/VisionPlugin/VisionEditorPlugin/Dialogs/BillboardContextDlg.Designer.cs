namespace VisionEditorPlugin.Dialogs
{
  partial class BillboardContextDlg
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(BillboardContextDlg));
      this.toolStrip1 = new System.Windows.Forms.ToolStrip();
      this.toolStripLabel2 = new System.Windows.Forms.ToolStripLabel();
      this.toolStripButton_ContextAdd = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_ContextRemove = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_ContextSelect = new System.Windows.Forms.ToolStripButton();
      this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
      this.toolStripLabel1 = new System.Windows.Forms.ToolStripLabel();
      this.toolStripButton_Recenter = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_DropToFloor = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_Delete = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_Relax = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_RemoveOverlapping = new System.Windows.Forms.ToolStripButton();
      this.groupBox_Create = new System.Windows.Forms.GroupBox();
      this.label2 = new System.Windows.Forms.Label();
      this.label1 = new System.Windows.Forms.Label();
      this.trackBar_ScaleVariation = new System.Windows.Forms.TrackBar();
      this.numericUpDown_Scale = new System.Windows.Forms.NumericUpDown();
      this.statusStrip1 = new System.Windows.Forms.StatusStrip();
      this.toolStripStatusLabel1 = new System.Windows.Forms.ToolStripStatusLabel();
      this.toolStrip1.SuspendLayout();
      this.groupBox_Create.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.trackBar_ScaleVariation)).BeginInit();
      ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_Scale)).BeginInit();
      this.statusStrip1.SuspendLayout();
      this.SuspendLayout();
      // 
      // toolStrip1
      // 
      this.toolStrip1.ImageScalingSize = new System.Drawing.Size(32, 32);
      this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripLabel2,
            this.toolStripButton_ContextAdd,
            this.toolStripButton_ContextRemove,
            this.toolStripButton_ContextSelect,
            this.toolStripSeparator1,
            this.toolStripLabel1,
            this.toolStripButton_Recenter,
            this.toolStripButton_DropToFloor,
            this.toolStripButton_Delete,
            this.toolStripButton_Relax,
            this.toolStripButton_RemoveOverlapping});
      this.toolStrip1.Location = new System.Drawing.Point(0, 0);
      this.toolStrip1.Name = "toolStrip1";
      this.toolStrip1.Size = new System.Drawing.Size(426, 39);
      this.toolStrip1.TabIndex = 0;
      this.toolStrip1.Text = "toolStrip1";
      // 
      // toolStripLabel2
      // 
      this.toolStripLabel2.Name = "toolStripLabel2";
      this.toolStripLabel2.Size = new System.Drawing.Size(70, 36);
      this.toolStripLabel2.Text = "Click mode:";
      // 
      // toolStripButton_ContextAdd
      // 
      this.toolStripButton_ContextAdd.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_ContextAdd.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_ContextAdd.Image")));
      this.toolStripButton_ContextAdd.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_ContextAdd.Name = "toolStripButton_ContextAdd";
      this.toolStripButton_ContextAdd.Size = new System.Drawing.Size(36, 36);
      this.toolStripButton_ContextAdd.Text = "In this mode new billboards are created per click";
      this.toolStripButton_ContextAdd.Click += new System.EventHandler(this.toolStripButton_ContextAdd_Click);
      // 
      // toolStripButton_ContextRemove
      // 
      this.toolStripButton_ContextRemove.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_ContextRemove.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_ContextRemove.Image")));
      this.toolStripButton_ContextRemove.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_ContextRemove.Name = "toolStripButton_ContextRemove";
      this.toolStripButton_ContextRemove.Size = new System.Drawing.Size(36, 36);
      this.toolStripButton_ContextRemove.Text = "In this mode existing billboards are deleted by clicking on them";
      this.toolStripButton_ContextRemove.Click += new System.EventHandler(this.toolStripButton_ContextRemove_Click);
      // 
      // toolStripButton_ContextSelect
      // 
      this.toolStripButton_ContextSelect.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_ContextSelect.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_ContextSelect.Image")));
      this.toolStripButton_ContextSelect.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_ContextSelect.Name = "toolStripButton_ContextSelect";
      this.toolStripButton_ContextSelect.Size = new System.Drawing.Size(36, 36);
      this.toolStripButton_ContextSelect.Text = "In this mode single billboards can be selected by clicking on them";
      this.toolStripButton_ContextSelect.Click += new System.EventHandler(this.toolStripButton_ContextSelect_Click);
      // 
      // toolStripSeparator1
      // 
      this.toolStripSeparator1.Name = "toolStripSeparator1";
      this.toolStripSeparator1.Size = new System.Drawing.Size(6, 39);
      // 
      // toolStripLabel1
      // 
      this.toolStripLabel1.Name = "toolStripLabel1";
      this.toolStripLabel1.Size = new System.Drawing.Size(50, 36);
      this.toolStripLabel1.Text = "Actions:";
      // 
      // toolStripButton_Recenter
      // 
      this.toolStripButton_Recenter.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_Recenter.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_Recenter.Image")));
      this.toolStripButton_Recenter.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_Recenter.Name = "toolStripButton_Recenter";
      this.toolStripButton_Recenter.Size = new System.Drawing.Size(36, 36);
      this.toolStripButton_Recenter.Text = "Recenter the group\'s pivot so it is in the center of the bounding box";
      this.toolStripButton_Recenter.Click += new System.EventHandler(this.toolStripButton_Recenter_Click);
      // 
      // toolStripButton_DropToFloor
      // 
      this.toolStripButton_DropToFloor.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_DropToFloor.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_DropToFloor.Image")));
      this.toolStripButton_DropToFloor.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_DropToFloor.Name = "toolStripButton_DropToFloor";
      this.toolStripButton_DropToFloor.Size = new System.Drawing.Size(36, 36);
      this.toolStripButton_DropToFloor.Text = "Drop selection to floor";
      this.toolStripButton_DropToFloor.Click += new System.EventHandler(this.toolStripButton_DropToFloor_Click);
      // 
      // toolStripButton_Delete
      // 
      this.toolStripButton_Delete.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_Delete.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_Delete.Image")));
      this.toolStripButton_Delete.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_Delete.Name = "toolStripButton_Delete";
      this.toolStripButton_Delete.Size = new System.Drawing.Size(36, 36);
      this.toolStripButton_Delete.Text = "Delete selection";
      this.toolStripButton_Delete.Click += new System.EventHandler(this.toolStripButton_Delete_Click);
      // 
      // toolStripButton_Relax
      // 
      this.toolStripButton_Relax.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_Relax.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_Relax.Image")));
      this.toolStripButton_Relax.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_Relax.Name = "toolStripButton_Relax";
      this.toolStripButton_Relax.Size = new System.Drawing.Size(36, 36);
      this.toolStripButton_Relax.Text = "Relax positions around billboards";
      this.toolStripButton_Relax.Click += new System.EventHandler(this.toolStripButton_Relax_Click);
      // 
      // toolStripButton_RemoveOverlapping
      // 
      this.toolStripButton_RemoveOverlapping.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_RemoveOverlapping.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_RemoveOverlapping.Image")));
      this.toolStripButton_RemoveOverlapping.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_RemoveOverlapping.Name = "toolStripButton_RemoveOverlapping";
      this.toolStripButton_RemoveOverlapping.Size = new System.Drawing.Size(36, 36);
      this.toolStripButton_RemoveOverlapping.Text = "Remove instances that overlap with others";
      this.toolStripButton_RemoveOverlapping.Click += new System.EventHandler(this.toolStripButton_RemoveOverlapping_Click);
      // 
      // groupBox_Create
      // 
      this.groupBox_Create.Controls.Add(this.label2);
      this.groupBox_Create.Controls.Add(this.label1);
      this.groupBox_Create.Controls.Add(this.trackBar_ScaleVariation);
      this.groupBox_Create.Controls.Add(this.numericUpDown_Scale);
      this.groupBox_Create.Dock = System.Windows.Forms.DockStyle.Fill;
      this.groupBox_Create.Location = new System.Drawing.Point(0, 61);
      this.groupBox_Create.Name = "groupBox_Create";
      this.groupBox_Create.Size = new System.Drawing.Size(426, 69);
      this.groupBox_Create.TabIndex = 2;
      this.groupBox_Create.TabStop = false;
      this.groupBox_Create.Text = "Create new billboards";
      // 
      // label2
      // 
      this.label2.AutoSize = true;
      this.label2.Location = new System.Drawing.Point(327, 33);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(75, 13);
      this.label2.TabIndex = 3;
      this.label2.Text = "(Randomness)";
      // 
      // label1
      // 
      this.label1.AutoSize = true;
      this.label1.Location = new System.Drawing.Point(13, 33);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(85, 13);
      this.label1.TabIndex = 0;
      this.label1.Text = "Relative scaling:";
      // 
      // trackBar_ScaleVariation
      // 
      this.trackBar_ScaleVariation.LargeChange = 10;
      this.trackBar_ScaleVariation.Location = new System.Drawing.Point(187, 31);
      this.trackBar_ScaleVariation.Maximum = 100;
      this.trackBar_ScaleVariation.Name = "trackBar_ScaleVariation";
      this.trackBar_ScaleVariation.Size = new System.Drawing.Size(134, 45);
      this.trackBar_ScaleVariation.SmallChange = 5;
      this.trackBar_ScaleVariation.TabIndex = 2;
      this.trackBar_ScaleVariation.TickFrequency = 10;
      this.trackBar_ScaleVariation.Value = 20;
      this.trackBar_ScaleVariation.Scroll += new System.EventHandler(this.trackBar_ScaleVariation_Scroll);
      // 
      // numericUpDown_Scale
      // 
      this.numericUpDown_Scale.DecimalPlaces = 2;
      this.numericUpDown_Scale.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
      this.numericUpDown_Scale.Location = new System.Drawing.Point(104, 31);
      this.numericUpDown_Scale.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            131072});
      this.numericUpDown_Scale.Name = "numericUpDown_Scale";
      this.numericUpDown_Scale.Size = new System.Drawing.Size(77, 20);
      this.numericUpDown_Scale.TabIndex = 1;
      this.numericUpDown_Scale.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
      this.numericUpDown_Scale.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
      this.numericUpDown_Scale.ValueChanged += new System.EventHandler(this.numericUpDown_Scale_ValueChanged);
      // 
      // statusStrip1
      // 
      this.statusStrip1.Dock = System.Windows.Forms.DockStyle.Top;
      this.statusStrip1.GripMargin = new System.Windows.Forms.Padding(4);
      this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripStatusLabel1});
      this.statusStrip1.Location = new System.Drawing.Point(0, 39);
      this.statusStrip1.Name = "statusStrip1";
      this.statusStrip1.Size = new System.Drawing.Size(426, 22);
      this.statusStrip1.TabIndex = 1;
      this.statusStrip1.Text = "statusStrip1";
      // 
      // toolStripStatusLabel1
      // 
      this.toolStripStatusLabel1.Name = "toolStripStatusLabel1";
      this.toolStripStatusLabel1.Size = new System.Drawing.Size(130, 17);
      this.toolStripStatusLabel1.Text = "<status text goes here>";
      // 
      // BillboardContextDlg
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(426, 130);
      this.Controls.Add(this.groupBox_Create);
      this.Controls.Add(this.statusStrip1);
      this.Controls.Add(this.toolStrip1);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.MaximizeBox = false;
      this.MaximumSize = new System.Drawing.Size(432, 154);
      this.MinimizeBox = false;
      this.MinimumSize = new System.Drawing.Size(432, 85);
      this.Name = "BillboardContextDlg";
      this.Text = "Billboards";
      this.toolStrip1.ResumeLayout(false);
      this.toolStrip1.PerformLayout();
      this.groupBox_Create.ResumeLayout(false);
      this.groupBox_Create.PerformLayout();
      ((System.ComponentModel.ISupportInitialize)(this.trackBar_ScaleVariation)).EndInit();
      ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_Scale)).EndInit();
      this.statusStrip1.ResumeLayout(false);
      this.statusStrip1.PerformLayout();
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.ToolStrip toolStrip1;
    private System.Windows.Forms.ToolStripButton toolStripButton_ContextAdd;
    private System.Windows.Forms.ToolStripButton toolStripButton_ContextRemove;
    private System.Windows.Forms.ToolStripButton toolStripButton_ContextSelect;
    private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
    private System.Windows.Forms.GroupBox groupBox_Create;
    private System.Windows.Forms.NumericUpDown numericUpDown_Scale;
    private System.Windows.Forms.TrackBar trackBar_ScaleVariation;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.StatusStrip statusStrip1;
    private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel1;
    private System.Windows.Forms.ToolStripButton toolStripButton_DropToFloor;
    private System.Windows.Forms.ToolStripButton toolStripButton_Delete;
    private System.Windows.Forms.ToolStripButton toolStripButton_Relax;
    private System.Windows.Forms.ToolStripButton toolStripButton_Recenter;
    private System.Windows.Forms.ToolStripButton toolStripButton_RemoveOverlapping;
    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.ToolStripLabel toolStripLabel2;
    private System.Windows.Forms.ToolStripLabel toolStripLabel1;
  }
}