
using CSharpFramework;
using CSharpFramework.Docking;

namespace HavokEditorPlugin
{
    partial class HavokPhysicsPanel
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
          this.components = new System.ComponentModel.Container();
          System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(HavokPhysicsPanel));
          this.collisionDataGridView = new System.Windows.Forms.DataGridView();
          this.ToolTip = new System.Windows.Forms.ToolTip(this.components);
          this.ToolStrip = new System.Windows.Forms.ToolStrip();
          this.toolStripButton_Play = new System.Windows.Forms.ToolStripButton();
          this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
          this.toolStripButton_VisDynamics = new System.Windows.Forms.ToolStripButton();
          this.toolStripButton_VisStatic = new System.Windows.Forms.ToolStripButton();
          this.toolStripButton_VisController = new System.Windows.Forms.ToolStripButton();
          this.toolStripButton_VisTrigger = new System.Windows.Forms.ToolStripButton();
          this.splitContainer1 = new System.Windows.Forms.SplitContainer();
          this.PropertyGrid = new CSharpFramework.Controls.PropertyGridNested();
      this.label_CollisionLayer = new System.Windows.Forms.Label();
          this.dialogCaptionBar = new CSharpFramework.Controls.DialogCaptionBar();
          ((System.ComponentModel.ISupportInitialize)(this.collisionDataGridView)).BeginInit();
          this.ToolStrip.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
          this.splitContainer1.Panel1.SuspendLayout();
          this.splitContainer1.Panel2.SuspendLayout();
          this.splitContainer1.SuspendLayout();
          this.SuspendLayout();
          // 
          // collisionDataGridView
          // 
          this.collisionDataGridView.AllowUserToAddRows = false;
          this.collisionDataGridView.AllowUserToDeleteRows = false;
          this.collisionDataGridView.AllowUserToOrderColumns = true;
          this.collisionDataGridView.AllowUserToResizeColumns = false;
          this.collisionDataGridView.AllowUserToResizeRows = false;
          this.collisionDataGridView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.ColumnHeader;
          this.collisionDataGridView.BorderStyle = System.Windows.Forms.BorderStyle.None;
          this.collisionDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
          this.collisionDataGridView.Dock = System.Windows.Forms.DockStyle.Fill;
          this.collisionDataGridView.Location = new System.Drawing.Point(0, 21);
          this.collisionDataGridView.Margin = new System.Windows.Forms.Padding(0);
          this.collisionDataGridView.Name = "collisionDataGridView";
          this.collisionDataGridView.RowHeadersWidthSizeMode = System.Windows.Forms.DataGridViewRowHeadersWidthSizeMode.AutoSizeToAllHeaders;
          this.collisionDataGridView.Size = new System.Drawing.Size(561, 135);
          this.collisionDataGridView.TabIndex = 9;
      this.collisionDataGridView.CellContentClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.CollisionDataGridView_CellContentClicked);
          this.collisionDataGridView.CellContentDoubleClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.CollisionDataGridView_CellContentDoubleClicked);
          // 
          // ToolStrip
          // 
          this.ToolStrip.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
          this.ToolStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton_Play,
            this.toolStripSeparator1,
            this.toolStripButton_VisDynamics,
            this.toolStripButton_VisStatic,
            this.toolStripButton_VisController,
            this.toolStripButton_VisTrigger});
          this.ToolStrip.Location = new System.Drawing.Point(0, 50);
          this.ToolStrip.Name = "ToolStrip";
          this.ToolStrip.Size = new System.Drawing.Size(561, 25);
          this.ToolStrip.TabIndex = 13;
          // 
          // toolStripButton_Play
          // 
          this.toolStripButton_Play.CheckOnClick = true;
          this.toolStripButton_Play.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
          this.toolStripButton_Play.Image = global::HavokEditorPlugin.Properties.Resources.physics_pause;
          this.toolStripButton_Play.ImageTransparentColor = System.Drawing.Color.Magenta;
          this.toolStripButton_Play.Name = "toolStripButton_Play";
          this.toolStripButton_Play.Size = new System.Drawing.Size(23, 22);
          this.toolStripButton_Play.ToolTipText = "Pause Simulation";
          this.toolStripButton_Play.Click += new System.EventHandler(this.toolStripButton_Play_Click);
          // 
          // toolStripSeparator1
          // 
          this.toolStripSeparator1.Name = "toolStripSeparator1";
          this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
          // 
          // toolStripButton_VisDynamics
          // 
          this.toolStripButton_VisDynamics.CheckOnClick = true;
          this.toolStripButton_VisDynamics.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
          this.toolStripButton_VisDynamics.Image = global::HavokEditorPlugin.Properties.Resources.physics_dynamics;
          this.toolStripButton_VisDynamics.ImageTransparentColor = System.Drawing.Color.Magenta;
          this.toolStripButton_VisDynamics.Name = "toolStripButton_VisDynamics";
          this.toolStripButton_VisDynamics.Size = new System.Drawing.Size(23, 22);
          this.toolStripButton_VisDynamics.ToolTipText = "Visualize Dynamic Objects";
          this.toolStripButton_VisDynamics.Click += new System.EventHandler(this.toolStripButton_VisDynamics_Click);
          // 
          // toolStripButton_VisStatic
          // 
          this.toolStripButton_VisStatic.CheckOnClick = true;
          this.toolStripButton_VisStatic.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
          this.toolStripButton_VisStatic.Image = global::HavokEditorPlugin.Properties.Resources.physics_static;
          this.toolStripButton_VisStatic.ImageTransparentColor = System.Drawing.Color.Magenta;
          this.toolStripButton_VisStatic.Name = "toolStripButton_VisStatic";
          this.toolStripButton_VisStatic.Size = new System.Drawing.Size(23, 22);
          this.toolStripButton_VisStatic.ToolTipText = "Visualize Static Objects";
          this.toolStripButton_VisStatic.Click += new System.EventHandler(this.toolStripButton_VisStatic_Click);
          // 
          // toolStripButton_VisController
          // 
          this.toolStripButton_VisController.CheckOnClick = true;
          this.toolStripButton_VisController.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
          this.toolStripButton_VisController.Image = global::HavokEditorPlugin.Properties.Resources.physics_character;
          this.toolStripButton_VisController.ImageTransparentColor = System.Drawing.Color.Magenta;
          this.toolStripButton_VisController.Name = "toolStripButton_VisController";
          this.toolStripButton_VisController.Size = new System.Drawing.Size(23, 22);
          this.toolStripButton_VisController.ToolTipText = "Visualize Character Controller";
          this.toolStripButton_VisController.Click += new System.EventHandler(this.toolStripButton_VisController_Click);
          // 
          // toolStripButton_VisTrigger
          // 
          this.toolStripButton_VisTrigger.CheckOnClick = true;
          this.toolStripButton_VisTrigger.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
          this.toolStripButton_VisTrigger.Image = global::HavokEditorPlugin.Properties.Resources.physics_trigger;
          this.toolStripButton_VisTrigger.ImageTransparentColor = System.Drawing.Color.Magenta;
          this.toolStripButton_VisTrigger.Name = "toolStripButton_VisTrigger";
          this.toolStripButton_VisTrigger.Size = new System.Drawing.Size(23, 22);
          this.toolStripButton_VisTrigger.ToolTipText = "Visualize Trigger Volumes";
          this.toolStripButton_VisTrigger.Click += new System.EventHandler(this.toolStripButton_VisTrigger_Click);
          // 
          // splitContainer1
          // 
          this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
          this.splitContainer1.Location = new System.Drawing.Point(0, 75);
          this.splitContainer1.Name = "splitContainer1";
          this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
          // 
          // splitContainer1.Panel1
          // 
          this.splitContainer1.Panel1.Controls.Add(this.PropertyGrid);
          // 
          // splitContainer1.Panel2
          // 
          this.splitContainer1.Panel2.Controls.Add(this.collisionDataGridView);
      this.splitContainer1.Panel2.Controls.Add(this.label_CollisionLayer);
          this.splitContainer1.Size = new System.Drawing.Size(561, 434);
          this.splitContainer1.SplitterDistance = 274;
          this.splitContainer1.TabIndex = 14;
          // 
          // PropertyGrid
          // 
          this.PropertyGrid.Dock = System.Windows.Forms.DockStyle.Fill;
          this.PropertyGrid.HelpBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(252)))), ((int)(((byte)(252)))), ((int)(((byte)(252)))));
          this.PropertyGrid.HelpBackgroundImage = ((System.Drawing.Image)(resources.GetObject("PropertyGrid.HelpBackgroundImage")));
          this.PropertyGrid.LineColor = System.Drawing.Color.FromArgb(((int)(((byte)(248)))), ((int)(((byte)(248)))), ((int)(((byte)(248)))));
          this.PropertyGrid.Location = new System.Drawing.Point(0, 0);
          this.PropertyGrid.Name = "PropertyGrid";
          this.PropertyGrid.PropertySort = System.Windows.Forms.PropertySort.Categorized;
          this.PropertyGrid.Size = new System.Drawing.Size(561, 274);
          this.PropertyGrid.TabIndex = 3;
          this.PropertyGrid.ToolbarVisible = false;
          // 
          // 
          // 
          this.PropertyGrid.ToolStrip.AccessibleName = "ToolBar";
          this.PropertyGrid.ToolStrip.AccessibleRole = System.Windows.Forms.AccessibleRole.ToolBar;
          this.PropertyGrid.ToolStrip.AllowMerge = false;
          this.PropertyGrid.ToolStrip.AutoSize = false;
          this.PropertyGrid.ToolStrip.CanOverflow = false;
          this.PropertyGrid.ToolStrip.Dock = System.Windows.Forms.DockStyle.None;
          this.PropertyGrid.ToolStrip.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
          this.PropertyGrid.ToolStrip.Location = new System.Drawing.Point(0, 0);
          this.PropertyGrid.ToolStrip.Name = "";
          this.PropertyGrid.ToolStrip.Padding = new System.Windows.Forms.Padding(2, 0, 1, 0);
          this.PropertyGrid.ToolStrip.TabIndex = 1;
          this.PropertyGrid.ToolStrip.TabStop = true;
          this.PropertyGrid.ToolStrip.Text = "PropertyGridToolBar";
          this.PropertyGrid.ToolStrip.Visible = false;
          this.PropertyGrid.PropertyValueChanged += new System.Windows.Forms.PropertyValueChangedEventHandler(this.PropertyGrid_PropertyValueChanged);
          // 
      // label_CollisionLayer
          // 
      this.label_CollisionLayer.Dock = System.Windows.Forms.DockStyle.Top;
      this.label_CollisionLayer.Location = new System.Drawing.Point(0, 0);
      this.label_CollisionLayer.Name = "label_CollisionLayer";
      this.label_CollisionLayer.Padding = new System.Windows.Forms.Padding(3);
      this.label_CollisionLayer.Size = new System.Drawing.Size(561, 21);
      this.label_CollisionLayer.TabIndex = 17;
      this.label_CollisionLayer.Text = "Collision Layer Setup:";
      this.ToolTip.SetToolTip(this.label_CollisionLayer, "Collisions between objects from different collision layers can be enabled / disab" +
        "led.");
          // 
          // dialogCaptionBar
          // 
          this.dialogCaptionBar.BackColor = System.Drawing.SystemColors.Window;
          this.dialogCaptionBar.Caption = "Havok Physics";
          this.dialogCaptionBar.CompactView = false;
          this.dialogCaptionBar.Description = "<version>";
          this.dialogCaptionBar.Dock = System.Windows.Forms.DockStyle.Top;
          this.dialogCaptionBar.Image = global::HavokEditorPlugin.Properties.Resources.physics;
          this.dialogCaptionBar.Location = new System.Drawing.Point(0, 0);
          this.dialogCaptionBar.Name = "dialogCaptionBar";
          this.dialogCaptionBar.SetFontColor = System.Drawing.SystemColors.ControlText;
          this.dialogCaptionBar.ShowBorder = false;
          this.dialogCaptionBar.ShowBottomLine = true;
          this.dialogCaptionBar.ShowCaptionText = true;
          this.dialogCaptionBar.ShowImage = true;
          this.dialogCaptionBar.Size = new System.Drawing.Size(561, 50);
          this.dialogCaptionBar.TabIndex = 11;
          // 
          // HavokPhysicsPanel
          // 
          this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
          this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
          this.ClientSize = new System.Drawing.Size(561, 509);
          this.Controls.Add(this.splitContainer1);
          this.Controls.Add(this.ToolStrip);
          this.Controls.Add(this.dialogCaptionBar);
          this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
          this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
          this.MaximizeBox = false;
          this.MinimizeBox = false;
          this.Name = "HavokPhysicsPanel";
          this.TabText = "Havok Physics";
          this.Text = "Havok Physics";
          ((System.ComponentModel.ISupportInitialize)(this.collisionDataGridView)).EndInit();
          this.ToolStrip.ResumeLayout(false);
          this.ToolStrip.PerformLayout();
          this.splitContainer1.Panel1.ResumeLayout(false);
          this.splitContainer1.Panel2.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
          this.splitContainer1.ResumeLayout(false);
          this.ResumeLayout(false);
          this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.DataGridView collisionDataGridView;
        private System.Windows.Forms.ToolTip ToolTip;
        private System.Windows.Forms.ToolStrip ToolStrip;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private CSharpFramework.Controls.PropertyGridNested PropertyGrid;
        private System.Windows.Forms.Label label_CollisionLayer;
        private System.Windows.Forms.ToolStripButton toolStripButton_Play;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton toolStripButton_VisDynamics;
        private System.Windows.Forms.ToolStripButton toolStripButton_VisController;
        private System.Windows.Forms.ToolStripButton toolStripButton_VisTrigger;
        private System.Windows.Forms.ToolStripButton toolStripButton_VisStatic;
        private CSharpFramework.Controls.DialogCaptionBar dialogCaptionBar;
    }
}