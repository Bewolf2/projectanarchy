namespace HavokAiPanelDialogs
{
    partial class HavokAiPanel
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
          System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(HavokAiPanel));
          this.GlobalSettings_PropertyGridEx1 = new CSharpFramework.Controls.PropertyGridEx();
          this.GlobalSettings_ListView = new System.Windows.Forms.ListView();
          this.dialogCaptionBar = new CSharpFramework.Controls.DialogCaptionBar();
          this.splitContainer1 = new System.Windows.Forms.SplitContainer();
          this.splitContainer2 = new System.Windows.Forms.SplitContainer();
          this.BuildStatisticsRichTextBox = new System.Windows.Forms.RichTextBox();
          this.toolStrip_NavMeshGeneration = new System.Windows.Forms.ToolStrip();
          this.toolStripLabel1 = new System.Windows.Forms.ToolStripLabel();
          this.tsb_Build = new System.Windows.Forms.ToolStripButton();
          this.tsb_ConnectPhysics = new System.Windows.Forms.ToolStripButton();
          this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
          this.tsb_TestPathPickStart = new System.Windows.Forms.ToolStripButton();
          this.tsb_TestPathPickEnd = new System.Windows.Forms.ToolStripButton();
          this.toolStrip_NavMeshSettings = new System.Windows.Forms.ToolStrip();
          this.toolStripLabel2 = new System.Windows.Forms.ToolStripLabel();
          this.NewSettingsButton = new System.Windows.Forms.ToolStripButton();
          this.DeletedButton = new System.Windows.Forms.ToolStripButton();
          this.CopySettingsButton = new System.Windows.Forms.ToolStripButton();
          this.PasteSettingsButton = new System.Windows.Forms.ToolStripButton();
          this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
          this.ExportSelectedSettingsButton = new System.Windows.Forms.ToolStripButton();
          this.ImportSettingsButton = new System.Windows.Forms.ToolStripButton();
          this.toolStripSeparator5 = new System.Windows.Forms.ToolStripSeparator();
          this.SetAsDefaultButton = new System.Windows.Forms.ToolStripButton();
          this.splitContainer1.Panel1.SuspendLayout();
          this.splitContainer1.Panel2.SuspendLayout();
          this.splitContainer1.SuspendLayout();
          this.splitContainer2.Panel1.SuspendLayout();
          this.splitContainer2.Panel2.SuspendLayout();
          this.splitContainer2.SuspendLayout();
          this.toolStrip_NavMeshGeneration.SuspendLayout();
          this.toolStrip_NavMeshSettings.SuspendLayout();
          this.SuspendLayout();
          // 
          // GlobalSettings_PropertyGridEx1
          // 
          this.GlobalSettings_PropertyGridEx1.Dock = System.Windows.Forms.DockStyle.Fill;
          this.GlobalSettings_PropertyGridEx1.HelpBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(252)))), ((int)(((byte)(252)))), ((int)(((byte)(252)))));
          this.GlobalSettings_PropertyGridEx1.HelpBackgroundImage = ((System.Drawing.Image)(resources.GetObject("GlobalSettings_PropertyGridEx1.HelpBackgroundImage")));
          this.GlobalSettings_PropertyGridEx1.LineColor = System.Drawing.Color.FromArgb(((int)(((byte)(248)))), ((int)(((byte)(248)))), ((int)(((byte)(248)))));
          this.GlobalSettings_PropertyGridEx1.Location = new System.Drawing.Point(0, 0);
          this.GlobalSettings_PropertyGridEx1.Name = "GlobalSettings_PropertyGridEx1";
          this.GlobalSettings_PropertyGridEx1.Size = new System.Drawing.Size(389, 360);
          this.GlobalSettings_PropertyGridEx1.TabIndex = 15;
          this.GlobalSettings_PropertyGridEx1.ToolbarVisible = false;
          // 
          // 
          // 
          this.GlobalSettings_PropertyGridEx1.ToolStrip.AccessibleName = "ToolBar";
          this.GlobalSettings_PropertyGridEx1.ToolStrip.AccessibleRole = System.Windows.Forms.AccessibleRole.ToolBar;
          this.GlobalSettings_PropertyGridEx1.ToolStrip.AllowMerge = false;
          this.GlobalSettings_PropertyGridEx1.ToolStrip.AutoSize = false;
          this.GlobalSettings_PropertyGridEx1.ToolStrip.CanOverflow = false;
          this.GlobalSettings_PropertyGridEx1.ToolStrip.Dock = System.Windows.Forms.DockStyle.None;
          this.GlobalSettings_PropertyGridEx1.ToolStrip.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
          this.GlobalSettings_PropertyGridEx1.ToolStrip.Location = new System.Drawing.Point(0, 1);
          this.GlobalSettings_PropertyGridEx1.ToolStrip.Name = "";
          this.GlobalSettings_PropertyGridEx1.ToolStrip.Padding = new System.Windows.Forms.Padding(2, 0, 1, 0);
          this.GlobalSettings_PropertyGridEx1.ToolStrip.Size = new System.Drawing.Size(331, 25);
          this.GlobalSettings_PropertyGridEx1.ToolStrip.TabIndex = 1;
          this.GlobalSettings_PropertyGridEx1.ToolStrip.TabStop = true;
          this.GlobalSettings_PropertyGridEx1.ToolStrip.Text = "PropertyGridToolBar";
          this.GlobalSettings_PropertyGridEx1.ToolStrip.Visible = false;
          this.GlobalSettings_PropertyGridEx1.PropertyValueChanged += new System.Windows.Forms.PropertyValueChangedEventHandler(this.GlobalSettingsPropertyGrid_PropertyValueChanged);
          // 
          // GlobalSettings_ListView
          // 
          this.GlobalSettings_ListView.Dock = System.Windows.Forms.DockStyle.Fill;
          this.GlobalSettings_ListView.GridLines = true;
          this.GlobalSettings_ListView.Location = new System.Drawing.Point(0, 0);
          this.GlobalSettings_ListView.Name = "GlobalSettings_ListView";
          this.GlobalSettings_ListView.Size = new System.Drawing.Size(389, 72);
          this.GlobalSettings_ListView.TabIndex = 14;
          this.GlobalSettings_ListView.UseCompatibleStateImageBehavior = false;
          this.GlobalSettings_ListView.View = System.Windows.Forms.View.List;
          this.GlobalSettings_ListView.SelectedIndexChanged += new System.EventHandler(this.GlobalSettingsListView_SelectedIndexChanged);
          // 
          // dialogCaptionBar
          // 
          this.dialogCaptionBar.BackColor = System.Drawing.SystemColors.Window;
          this.dialogCaptionBar.Caption = "Havok AI";
          this.dialogCaptionBar.CompactView = false;
          this.dialogCaptionBar.Description = "(c) 2012 Havok.com Inc. All rights reserved.";
          this.dialogCaptionBar.Dock = System.Windows.Forms.DockStyle.Top;
          this.dialogCaptionBar.Image = ((System.Drawing.Image)(resources.GetObject("dialogCaptionBar.Image")));
          this.dialogCaptionBar.Location = new System.Drawing.Point(0, 0);
          this.dialogCaptionBar.Name = "dialogCaptionBar";
          this.dialogCaptionBar.SetFontColor = System.Drawing.SystemColors.ControlText;
          this.dialogCaptionBar.ShowBorder = false;
          this.dialogCaptionBar.ShowBottomLine = true;
          this.dialogCaptionBar.ShowCaptionText = true;
          this.dialogCaptionBar.ShowImage = true;
          this.dialogCaptionBar.Size = new System.Drawing.Size(389, 50);
          this.dialogCaptionBar.TabIndex = 17;
          // 
          // splitContainer1
          // 
          this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
          this.splitContainer1.Location = new System.Drawing.Point(0, 25);
          this.splitContainer1.Name = "splitContainer1";
          this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
          // 
          // splitContainer1.Panel1
          // 
          this.splitContainer1.Panel1.Controls.Add(this.GlobalSettings_ListView);
          // 
          // splitContainer1.Panel2
          // 
          this.splitContainer1.Panel2.Controls.Add(this.GlobalSettings_PropertyGridEx1);
          this.splitContainer1.Size = new System.Drawing.Size(389, 436);
          this.splitContainer1.SplitterDistance = 72;
          this.splitContainer1.TabIndex = 19;
          // 
          // splitContainer2
          // 
          this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
          this.splitContainer2.Location = new System.Drawing.Point(0, 50);
          this.splitContainer2.Name = "splitContainer2";
          this.splitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal;
          // 
          // splitContainer2.Panel1
          // 
          this.splitContainer2.Panel1.Controls.Add(this.BuildStatisticsRichTextBox);
          this.splitContainer2.Panel1.Controls.Add(this.toolStrip_NavMeshGeneration);
          // 
          // splitContainer2.Panel2
          // 
          this.splitContainer2.Panel2.Controls.Add(this.splitContainer1);
          this.splitContainer2.Panel2.Controls.Add(this.toolStrip_NavMeshSettings);
          this.splitContainer2.Size = new System.Drawing.Size(389, 573);
          this.splitContainer2.SplitterDistance = 108;
          this.splitContainer2.TabIndex = 20;
          // 
          // BuildStatisticsRichTextBox
          // 
          this.BuildStatisticsRichTextBox.Dock = System.Windows.Forms.DockStyle.Fill;
          this.BuildStatisticsRichTextBox.Location = new System.Drawing.Point(0, 25);
          this.BuildStatisticsRichTextBox.Name = "BuildStatisticsRichTextBox";
          this.BuildStatisticsRichTextBox.ReadOnly = true;
          this.BuildStatisticsRichTextBox.Size = new System.Drawing.Size(389, 83);
          this.BuildStatisticsRichTextBox.TabIndex = 4;
          this.BuildStatisticsRichTextBox.Text = "<statistics>";
          // 
          // toolStrip_NavMeshGeneration
          // 
          this.toolStrip_NavMeshGeneration.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
          this.toolStrip_NavMeshGeneration.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripLabel1,
            this.tsb_Build,
            this.tsb_ConnectPhysics,
            this.toolStripSeparator4,
            this.tsb_TestPathPickStart,
            this.tsb_TestPathPickEnd});
          this.toolStrip_NavMeshGeneration.Location = new System.Drawing.Point(0, 0);
          this.toolStrip_NavMeshGeneration.Name = "toolStrip_NavMeshGeneration";
          this.toolStrip_NavMeshGeneration.Size = new System.Drawing.Size(389, 25);
          this.toolStrip_NavMeshGeneration.TabIndex = 5;
          // 
          // toolStripLabel1
          // 
          this.toolStripLabel1.AutoSize = false;
          this.toolStripLabel1.Name = "toolStripLabel1";
          this.toolStripLabel1.Size = new System.Drawing.Size(110, 22);
          this.toolStripLabel1.Text = "Nav Mesh Generation:";
          this.toolStripLabel1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
          // 
          // tsb_Build
          // 
          this.tsb_Build.Image = global::HavokAiEditorPlugin.Properties.Resources.havok_ai_navmesh_notinsync;
          this.tsb_Build.Name = "tsb_Build";
          this.tsb_Build.Size = new System.Drawing.Size(23, 22);
          this.tsb_Build.ToolTipText = "Build Havok AI Nav Mesh (the nav mesh is currently out of sync with the settings)" +
              "";
          this.tsb_Build.Click += new System.EventHandler(this.BuildNavMesh_Click);
          // 
          // tsb_ConnectPhysics
          // 
          this.tsb_ConnectPhysics.CheckOnClick = true;
          this.tsb_ConnectPhysics.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
          this.tsb_ConnectPhysics.Image = global::HavokAiEditorPlugin.Properties.Resources.havok_ai_connectphysics;
          this.tsb_ConnectPhysics.Name = "tsb_ConnectPhysics";
          this.tsb_ConnectPhysics.Size = new System.Drawing.Size(23, 22);
          this.tsb_ConnectPhysics.ToolTipText = "Connect Nav Mesh to Havok Physics World";
          this.tsb_ConnectPhysics.Click += new System.EventHandler(this.ConnectPhysicsCheckButton_Click);
          // 
          // toolStripSeparator4
          // 
          this.toolStripSeparator4.Name = "toolStripSeparator4";
          this.toolStripSeparator4.Size = new System.Drawing.Size(6, 25);
          // 
          // tsb_TestPathPickStart
          // 
          this.tsb_TestPathPickStart.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
          this.tsb_TestPathPickStart.Image = global::HavokAiEditorPlugin.Properties.Resources.havok_ai_pickstart;
          this.tsb_TestPathPickStart.Name = "tsb_TestPathPickStart";
          this.tsb_TestPathPickStart.Size = new System.Drawing.Size(23, 22);
          this.tsb_TestPathPickStart.Text = "Pick Start Point of the Nav Mesh Test Path";
          this.tsb_TestPathPickStart.Click += new System.EventHandler(this.PickStartPoint_Click);
          // 
          // tsb_TestPathPickEnd
          // 
          this.tsb_TestPathPickEnd.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
          this.tsb_TestPathPickEnd.Image = global::HavokAiEditorPlugin.Properties.Resources.havok_ai_pickend;
          this.tsb_TestPathPickEnd.Name = "tsb_TestPathPickEnd";
          this.tsb_TestPathPickEnd.Size = new System.Drawing.Size(23, 22);
          this.tsb_TestPathPickEnd.Text = "Pick End Point of the Nav Mesh Test Path";
          this.tsb_TestPathPickEnd.Click += new System.EventHandler(this.PickEndPoint_Click);
          // 
          // toolStrip_NavMeshSettings
          // 
          this.toolStrip_NavMeshSettings.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
          this.toolStrip_NavMeshSettings.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripLabel2,
            this.NewSettingsButton,
            this.DeletedButton,
            this.CopySettingsButton,
            this.PasteSettingsButton,
            this.toolStripSeparator3,
            this.ExportSelectedSettingsButton,
            this.ImportSettingsButton,
            this.toolStripSeparator5,
            this.SetAsDefaultButton});
          this.toolStrip_NavMeshSettings.Location = new System.Drawing.Point(0, 0);
          this.toolStrip_NavMeshSettings.Name = "toolStrip_NavMeshSettings";
          this.toolStrip_NavMeshSettings.Size = new System.Drawing.Size(389, 25);
          this.toolStrip_NavMeshSettings.TabIndex = 19;
          // 
          // toolStripLabel2
          // 
          this.toolStripLabel2.AutoSize = false;
          this.toolStripLabel2.Name = "toolStripLabel2";
          this.toolStripLabel2.Size = new System.Drawing.Size(110, 22);
          this.toolStripLabel2.Text = "Nav Mesh Settings:";
          this.toolStripLabel2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
          // 
          // NewSettingsButton
          // 
          this.NewSettingsButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
          this.NewSettingsButton.Image = ((System.Drawing.Image)(resources.GetObject("NewSettingsButton.Image")));
          this.NewSettingsButton.Name = "NewSettingsButton";
          this.NewSettingsButton.Size = new System.Drawing.Size(23, 22);
          this.NewSettingsButton.ToolTipText = "Create New Settings";
          this.NewSettingsButton.Click += new System.EventHandler(this.NewSettingsButton_Click);
          // 
          // DeletedButton
          // 
          this.DeletedButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
          this.DeletedButton.Image = ((System.Drawing.Image)(resources.GetObject("DeletedButton.Image")));
          this.DeletedButton.Name = "DeletedButton";
          this.DeletedButton.Size = new System.Drawing.Size(23, 22);
          this.DeletedButton.ToolTipText = "Delete Selected Settings";
          this.DeletedButton.Click += new System.EventHandler(this.DeleteSettingsButton_Click);
          // 
          // CopySettingsButton
          // 
          this.CopySettingsButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
          this.CopySettingsButton.Image = ((System.Drawing.Image)(resources.GetObject("CopySettingsButton.Image")));
          this.CopySettingsButton.Name = "CopySettingsButton";
          this.CopySettingsButton.Size = new System.Drawing.Size(23, 22);
          this.CopySettingsButton.ToolTipText = "Copy Selected Settings";
          this.CopySettingsButton.Click += new System.EventHandler(this.CopySettingsButton_Click);
          // 
          // PasteSettingsButton
          // 
          this.PasteSettingsButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
          this.PasteSettingsButton.Image = ((System.Drawing.Image)(resources.GetObject("PasteSettingsButton.Image")));
          this.PasteSettingsButton.Name = "PasteSettingsButton";
          this.PasteSettingsButton.Size = new System.Drawing.Size(23, 22);
          this.PasteSettingsButton.ToolTipText = "Paste Settings";
          this.PasteSettingsButton.Click += new System.EventHandler(this.PasteSettingsButton_Click);
          // 
          // toolStripSeparator3
          // 
          this.toolStripSeparator3.Name = "toolStripSeparator3";
          this.toolStripSeparator3.Size = new System.Drawing.Size(6, 25);
          // 
          // ExportSelectedSettingsButton
          // 
          this.ExportSelectedSettingsButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
          this.ExportSelectedSettingsButton.Image = ((System.Drawing.Image)(resources.GetObject("ExportSelectedSettingsButton.Image")));
          this.ExportSelectedSettingsButton.Name = "ExportSelectedSettingsButton";
          this.ExportSelectedSettingsButton.Size = new System.Drawing.Size(23, 22);
          this.ExportSelectedSettingsButton.ToolTipText = "Save Selected Settings to External File";
          this.ExportSelectedSettingsButton.Click += new System.EventHandler(this.ExportSelectedSettingsButton_Click);
          // 
          // ImportSettingsButton
          // 
          this.ImportSettingsButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
          this.ImportSettingsButton.Image = ((System.Drawing.Image)(resources.GetObject("ImportSettingsButton.Image")));
          this.ImportSettingsButton.Name = "ImportSettingsButton";
          this.ImportSettingsButton.Size = new System.Drawing.Size(23, 22);
          this.ImportSettingsButton.ToolTipText = "Load Settings from External File";
          this.ImportSettingsButton.Click += new System.EventHandler(this.ImportSettingsButton_Click);
          // 
          // toolStripSeparator5
          // 
          this.toolStripSeparator5.Name = "toolStripSeparator5";
          this.toolStripSeparator5.Size = new System.Drawing.Size(6, 25);
          // 
          // SetAsDefaultButton
          // 
          this.SetAsDefaultButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
          this.SetAsDefaultButton.Image = ((System.Drawing.Image)(resources.GetObject("SetAsDefaultButton.Image")));
          this.SetAsDefaultButton.Name = "SetAsDefaultButton";
          this.SetAsDefaultButton.Size = new System.Drawing.Size(23, 22);
          this.SetAsDefaultButton.ToolTipText = "Set Selected Settings as Default Settings";
          this.SetAsDefaultButton.Click += new System.EventHandler(this.SetAsDefaultButton_Click);
          // 
          // HavokAiPanel
          // 
          this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
          this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
          this.AutoScroll = true;
          this.ClientSize = new System.Drawing.Size(389, 623);
          this.Controls.Add(this.splitContainer2);
          this.Controls.Add(this.dialogCaptionBar);
          this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
          this.Name = "HavokAiPanel";
          this.TabText = "Havok AI";
          this.Text = "Havok AI";
          this.splitContainer1.Panel1.ResumeLayout(false);
          this.splitContainer1.Panel2.ResumeLayout(false);
          this.splitContainer1.ResumeLayout(false);
          this.splitContainer2.Panel1.ResumeLayout(false);
          this.splitContainer2.Panel1.PerformLayout();
          this.splitContainer2.Panel2.ResumeLayout(false);
          this.splitContainer2.Panel2.PerformLayout();
          this.splitContainer2.ResumeLayout(false);
          this.toolStrip_NavMeshGeneration.ResumeLayout(false);
          this.toolStrip_NavMeshGeneration.PerformLayout();
          this.toolStrip_NavMeshSettings.ResumeLayout(false);
          this.toolStrip_NavMeshSettings.PerformLayout();
          this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ListView GlobalSettings_ListView;
        private CSharpFramework.Controls.PropertyGridEx GlobalSettings_PropertyGridEx1;
        private CSharpFramework.Controls.DialogCaptionBar dialogCaptionBar;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.RichTextBox BuildStatisticsRichTextBox;
        private System.Windows.Forms.ToolStrip toolStrip_NavMeshGeneration;
        private System.Windows.Forms.ToolStripButton tsb_Build;
        private System.Windows.Forms.ToolStripButton tsb_ConnectPhysics;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
        private System.Windows.Forms.ToolStripButton tsb_TestPathPickStart;
        private System.Windows.Forms.ToolStripButton tsb_TestPathPickEnd;
        private System.Windows.Forms.ToolStrip toolStrip_NavMeshSettings;
        private System.Windows.Forms.ToolStripButton NewSettingsButton;
        private System.Windows.Forms.ToolStripButton DeletedButton;
        private System.Windows.Forms.ToolStripButton ImportSettingsButton;
        private System.Windows.Forms.ToolStripButton ExportSelectedSettingsButton;
        private System.Windows.Forms.ToolStripButton SetAsDefaultButton;
        private System.Windows.Forms.ToolStripButton CopySettingsButton;
        private System.Windows.Forms.ToolStripButton PasteSettingsButton;
        private System.Windows.Forms.ToolStripLabel toolStripLabel1;
        private System.Windows.Forms.ToolStripLabel toolStripLabel2;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator5;
    }
}

