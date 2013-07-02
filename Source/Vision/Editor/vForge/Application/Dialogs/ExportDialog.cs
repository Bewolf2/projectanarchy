/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using CSharpFramework.Dialogs;
using CSharpFramework;
using CSharpFramework.Scene;
using CSharpFramework.Controls;
using ManagedFramework;
using CSharpFramework.Helper;
using System.IO;
using System.Reflection;
using System.Collections.Generic;
using CSharpFramework.AssetManagement;
using CSharpFramework.BaseTypes;

namespace Editor.Dialogs
{
	/// <summary>
	/// Export Dialog class.
	/// </summary>
	public class ExportDialog : System.Windows.Forms.Form
	{
    private System.Windows.Forms.Button button_ExportActiveProfile;
    private System.Windows.Forms.Button button_CANCEL;
    private CSharpFramework.Controls.DialogCaptionBar captionBar;
    private System.Windows.Forms.GroupBox groupBox1;
    private System.Windows.Forms.Button button_Browse;
    private System.Windows.Forms.TextBox text_Pathname;
    private System.Windows.Forms.GroupBox settingsBox;
    private System.Windows.Forms.CheckBox checkBox_RunAfterExport;
    private System.Windows.Forms.GroupBox groupBox2;
    private CheckBox checkBox_IncludeVisibilityInfo;
    private GroupBox groupBox3;
    private Button button_CreateProfile;
    private ComboBox comboBox_Profile;
    private Button button_SaveProfile;
    private Label label1;
    private Label label2;
    private CheckBox checkBox_InvisibleLayersExportInvisibleObjects;
    private Label label3;
    private CheckBox checkBox_updateAssetTransformations;
    private Button button_ExportSelectedProfiles;
    private Label label_PathExtension;
    private CheckBox checkBox_SaveProfileOnExport;
    private ProfileDropDownControl dropDown_AssetProfiles;
    private TableLayoutPanel tableLayoutPanel1;
    private Button button_ShowAssets;
    private Button button_AllLayers;
    private Button button_InvertLayers;
    private CheckBox checkBox_LoadedZonesOnly;
    private ListView listView_Layers;
    private ColumnHeader nameHeader;
    private ColumnHeader descriptionHeader;

		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

    /// <summary>
    /// Constructor
    /// </summary>
		public ExportDialog()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

      // re-use this image list
      listView_Layers.SmallImageList = LayerTreeViewNodeBase.ImageList.ImageList;

      FillProfileCombobox();

      // Set up a ToolTip
      ToolTip toolTip = new ToolTip();
      toolTip.AutoPopDelay = 5000;
      toolTip.InitialDelay = 1000;
      toolTip.ReshowDelay = 500;
      toolTip.SetToolTip(this.checkBox_RunAfterExport, "Uses the vSceneViewer.exe in the same directory as the running vForge.exe");
		}

    private void ExportDialog_Load(object sender, EventArgs e)
    {
      //force the dialog loading the as "default" selected configuration
      comboBox_Profile_SelectionChangeCommitted(null, null);
    }

    protected override void Dispose(bool disposing)
		{
			if(disposing)
			{
				if(components != null)
				{
					components.Dispose();
				}
        comboBox_Profile.Dispose();
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ExportDialog));
      System.Windows.Forms.ListViewGroup listViewGroup1 = new System.Windows.Forms.ListViewGroup("Layers to export", System.Windows.Forms.HorizontalAlignment.Left);
      System.Windows.Forms.ListViewGroup listViewGroup2 = new System.Windows.Forms.ListViewGroup("Zones to export", System.Windows.Forms.HorizontalAlignment.Left);
      System.Windows.Forms.ListViewGroup listViewGroup3 = new System.Windows.Forms.ListViewGroup("Scene Components to export", System.Windows.Forms.HorizontalAlignment.Left);
      System.Windows.Forms.ListViewGroup listViewGroup4 = new System.Windows.Forms.ListViewGroup("Optional Plugins to embed", System.Windows.Forms.HorizontalAlignment.Left);
      System.Windows.Forms.ListViewItem listViewItem1 = new System.Windows.Forms.ListViewItem("xxx");
      System.Windows.Forms.ListViewItem listViewItem2 = new System.Windows.Forms.ListViewItem("yyy");
      System.Windows.Forms.ListViewItem listViewItem3 = new System.Windows.Forms.ListViewItem("zzz");
      System.Windows.Forms.ListViewItem listViewItem4 = new System.Windows.Forms.ListViewItem("www");
      this.button_ExportActiveProfile = new System.Windows.Forms.Button();
      this.button_CANCEL = new System.Windows.Forms.Button();
      this.groupBox1 = new System.Windows.Forms.GroupBox();
      this.dropDown_AssetProfiles = new CSharpFramework.Controls.ProfileDropDownControl();
      this.label_PathExtension = new System.Windows.Forms.Label();
      this.label3 = new System.Windows.Forms.Label();
      this.label1 = new System.Windows.Forms.Label();
      this.button_Browse = new System.Windows.Forms.Button();
      this.text_Pathname = new System.Windows.Forms.TextBox();
      this.settingsBox = new System.Windows.Forms.GroupBox();
      this.checkBox_updateAssetTransformations = new System.Windows.Forms.CheckBox();
      this.checkBox_InvisibleLayersExportInvisibleObjects = new System.Windows.Forms.CheckBox();
      this.checkBox_IncludeVisibilityInfo = new System.Windows.Forms.CheckBox();
      this.checkBox_RunAfterExport = new System.Windows.Forms.CheckBox();
      this.groupBox2 = new System.Windows.Forms.GroupBox();
      this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
      this.button_ShowAssets = new System.Windows.Forms.Button();
      this.button_AllLayers = new System.Windows.Forms.Button();
      this.button_InvertLayers = new System.Windows.Forms.Button();
      this.checkBox_LoadedZonesOnly = new System.Windows.Forms.CheckBox();
      this.listView_Layers = new System.Windows.Forms.ListView();
      this.nameHeader = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
      this.descriptionHeader = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
      this.groupBox3 = new System.Windows.Forms.GroupBox();
      this.checkBox_SaveProfileOnExport = new System.Windows.Forms.CheckBox();
      this.label2 = new System.Windows.Forms.Label();
      this.button_SaveProfile = new System.Windows.Forms.Button();
      this.button_CreateProfile = new System.Windows.Forms.Button();
      this.comboBox_Profile = new System.Windows.Forms.ComboBox();
      this.button_ExportSelectedProfiles = new System.Windows.Forms.Button();
      this.captionBar = new CSharpFramework.Controls.DialogCaptionBar();
      this.groupBox1.SuspendLayout();
      this.settingsBox.SuspendLayout();
      this.groupBox2.SuspendLayout();
      this.tableLayoutPanel1.SuspendLayout();
      this.groupBox3.SuspendLayout();
      this.SuspendLayout();
      // 
      // button_ExportActiveProfile
      // 
      this.button_ExportActiveProfile.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.button_ExportActiveProfile.Image = ((System.Drawing.Image)(resources.GetObject("button_ExportActiveProfile.Image")));
      this.button_ExportActiveProfile.ImageAlign = System.Drawing.ContentAlignment.MiddleRight;
      this.button_ExportActiveProfile.Location = new System.Drawing.Point(310, 695);
      this.button_ExportActiveProfile.Name = "button_ExportActiveProfile";
      this.button_ExportActiveProfile.Size = new System.Drawing.Size(154, 28);
      this.button_ExportActiveProfile.TabIndex = 6;
      this.button_ExportActiveProfile.Text = "Export Active Profile";
      this.button_ExportActiveProfile.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
      this.button_ExportActiveProfile.Click += new System.EventHandler(this.button_ExportActiveProfile_Click);
      // 
      // button_CANCEL
      // 
      this.button_CANCEL.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.button_CANCEL.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.button_CANCEL.Location = new System.Drawing.Point(470, 695);
      this.button_CANCEL.Name = "button_CANCEL";
      this.button_CANCEL.Size = new System.Drawing.Size(97, 28);
      this.button_CANCEL.TabIndex = 7;
      this.button_CANCEL.Text = "Cancel";
      // 
      // groupBox1
      // 
      this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.groupBox1.Controls.Add(this.dropDown_AssetProfiles);
      this.groupBox1.Controls.Add(this.label_PathExtension);
      this.groupBox1.Controls.Add(this.label3);
      this.groupBox1.Controls.Add(this.label1);
      this.groupBox1.Controls.Add(this.button_Browse);
      this.groupBox1.Controls.Add(this.text_Pathname);
      this.groupBox1.Location = new System.Drawing.Point(12, 165);
      this.groupBox1.Name = "groupBox1";
      this.groupBox1.Size = new System.Drawing.Size(555, 80);
      this.groupBox1.TabIndex = 2;
      this.groupBox1.TabStop = false;
      this.groupBox1.Text = "Target";
      // 
      // dropDown_AssetProfiles
      // 
      this.dropDown_AssetProfiles.AnchorSize = new System.Drawing.Size(290, 21);
      this.dropDown_AssetProfiles.BackColor = System.Drawing.Color.White;
      this.dropDown_AssetProfiles.DockSide = CSharpFramework.Controls.DropDownControl.eDockSide.Left;
      this.dropDown_AssetProfiles.Location = new System.Drawing.Point(104, 49);
      this.dropDown_AssetProfiles.Name = "dropDown_AssetProfiles";
      this.dropDown_AssetProfiles.Size = new System.Drawing.Size(290, 21);
      this.dropDown_AssetProfiles.TabIndex = 5;
      this.dropDown_AssetProfiles.ProfileSelectionChanged += new System.EventHandler(this.ProfileControlChanged);
      // 
      // label_PathExtension
      // 
      this.label_PathExtension.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.label_PathExtension.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
      this.label_PathExtension.Location = new System.Drawing.Point(400, 22);
      this.label_PathExtension.Name = "label_PathExtension";
      this.label_PathExtension.Size = new System.Drawing.Size(119, 20);
      this.label_PathExtension.TabIndex = 3;
      this.label_PathExtension.Text = ".[profile].vscene";
      this.label_PathExtension.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
      // 
      // label3
      // 
      this.label3.AutoSize = true;
      this.label3.Location = new System.Drawing.Point(13, 52);
      this.label3.Name = "label3";
      this.label3.Size = new System.Drawing.Size(73, 13);
      this.label3.TabIndex = 4;
      this.label3.Text = "Asset Profiles:";
      // 
      // label1
      // 
      this.label1.AutoSize = true;
      this.label1.Location = new System.Drawing.Point(13, 26);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(57, 13);
      this.label1.TabIndex = 0;
      this.label1.Text = "File Name:";
      // 
      // button_Browse
      // 
      this.button_Browse.Location = new System.Drawing.Point(525, 20);
      this.button_Browse.Name = "button_Browse";
      this.button_Browse.Size = new System.Drawing.Size(24, 24);
      this.button_Browse.TabIndex = 2;
      this.button_Browse.Text = "...";
      this.button_Browse.Click += new System.EventHandler(this.button_Browse_Click);
      // 
      // text_Pathname
      // 
      this.text_Pathname.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.text_Pathname.Location = new System.Drawing.Point(104, 23);
      this.text_Pathname.Name = "text_Pathname";
      this.text_Pathname.Size = new System.Drawing.Size(290, 20);
      this.text_Pathname.TabIndex = 1;
      this.text_Pathname.Text = "textBox1";
      this.text_Pathname.TextChanged += new System.EventHandler(this.ProfileControlChanged);
      // 
      // settingsBox
      // 
      this.settingsBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.settingsBox.Controls.Add(this.checkBox_updateAssetTransformations);
      this.settingsBox.Controls.Add(this.checkBox_InvisibleLayersExportInvisibleObjects);
      this.settingsBox.Controls.Add(this.checkBox_IncludeVisibilityInfo);
      this.settingsBox.Controls.Add(this.checkBox_RunAfterExport);
      this.settingsBox.Location = new System.Drawing.Point(12, 610);
      this.settingsBox.Name = "settingsBox";
      this.settingsBox.Size = new System.Drawing.Size(555, 75);
      this.settingsBox.TabIndex = 4;
      this.settingsBox.TabStop = false;
      this.settingsBox.Text = "Options";
      // 
      // checkBox_updateAssetTransformations
      // 
      this.checkBox_updateAssetTransformations.AutoSize = true;
      this.checkBox_updateAssetTransformations.Checked = true;
      this.checkBox_updateAssetTransformations.CheckState = System.Windows.Forms.CheckState.Checked;
      this.checkBox_updateAssetTransformations.Location = new System.Drawing.Point(247, 24);
      this.checkBox_updateAssetTransformations.Name = "checkBox_updateAssetTransformations";
      this.checkBox_updateAssetTransformations.Size = new System.Drawing.Size(207, 17);
      this.checkBox_updateAssetTransformations.TabIndex = 7;
      this.checkBox_updateAssetTransformations.Text = "Execute Asset Transformation Pipeline";
      this.checkBox_updateAssetTransformations.CheckedChanged += new System.EventHandler(this.ProfileControlChanged);
      // 
      // checkBox_InvisibleLayersExportInvisibleObjects
      // 
      this.checkBox_InvisibleLayersExportInvisibleObjects.AutoSize = true;
      this.checkBox_InvisibleLayersExportInvisibleObjects.Location = new System.Drawing.Point(16, 47);
      this.checkBox_InvisibleLayersExportInvisibleObjects.Name = "checkBox_InvisibleLayersExportInvisibleObjects";
      this.checkBox_InvisibleLayersExportInvisibleObjects.Size = new System.Drawing.Size(209, 17);
      this.checkBox_InvisibleLayersExportInvisibleObjects.TabIndex = 2;
      this.checkBox_InvisibleLayersExportInvisibleObjects.Text = "Invisible Layers export invisible Objects";
      this.checkBox_InvisibleLayersExportInvisibleObjects.CheckedChanged += new System.EventHandler(this.ProfileControlChanged);
      // 
      // checkBox_IncludeVisibilityInfo
      // 
      this.checkBox_IncludeVisibilityInfo.AutoSize = true;
      this.checkBox_IncludeVisibilityInfo.Location = new System.Drawing.Point(16, 24);
      this.checkBox_IncludeVisibilityInfo.Name = "checkBox_IncludeVisibilityInfo";
      this.checkBox_IncludeVisibilityInfo.Size = new System.Drawing.Size(167, 17);
      this.checkBox_IncludeVisibilityInfo.TabIndex = 0;
      this.checkBox_IncludeVisibilityInfo.Text = "Build and include Visibility Info";
      this.checkBox_IncludeVisibilityInfo.CheckedChanged += new System.EventHandler(this.ProfileControlChanged);
      // 
      // checkBox_RunAfterExport
      // 
      this.checkBox_RunAfterExport.AutoSize = true;
      this.checkBox_RunAfterExport.Location = new System.Drawing.Point(247, 47);
      this.checkBox_RunAfterExport.Name = "checkBox_RunAfterExport";
      this.checkBox_RunAfterExport.Size = new System.Drawing.Size(103, 17);
      this.checkBox_RunAfterExport.TabIndex = 1;
      this.checkBox_RunAfterExport.Text = "Run after Export";
      this.checkBox_RunAfterExport.CheckedChanged += new System.EventHandler(this.ProfileControlChanged);
      // 
      // groupBox2
      // 
      this.groupBox2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.groupBox2.Controls.Add(this.tableLayoutPanel1);
      this.groupBox2.Location = new System.Drawing.Point(12, 251);
      this.groupBox2.Name = "groupBox2";
      this.groupBox2.Size = new System.Drawing.Size(555, 353);
      this.groupBox2.TabIndex = 3;
      this.groupBox2.TabStop = false;
      this.groupBox2.Text = "Layers and Zones to export";
      // 
      // tableLayoutPanel1
      // 
      this.tableLayoutPanel1.ColumnCount = 4;
      this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
      this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
      this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
      this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
      this.tableLayoutPanel1.Controls.Add(this.button_ShowAssets, 3, 1);
      this.tableLayoutPanel1.Controls.Add(this.button_AllLayers, 1, 1);
      this.tableLayoutPanel1.Controls.Add(this.button_InvertLayers, 2, 1);
      this.tableLayoutPanel1.Controls.Add(this.checkBox_LoadedZonesOnly, 0, 1);
      this.tableLayoutPanel1.Controls.Add(this.listView_Layers, 0, 0);
      this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.tableLayoutPanel1.Location = new System.Drawing.Point(3, 16);
      this.tableLayoutPanel1.Name = "tableLayoutPanel1";
      this.tableLayoutPanel1.RowCount = 2;
      this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
      this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
      this.tableLayoutPanel1.Size = new System.Drawing.Size(549, 334);
      this.tableLayoutPanel1.TabIndex = 5;
      // 
      // button_ShowAssets
      // 
      this.button_ShowAssets.Location = new System.Drawing.Point(456, 308);
      this.button_ShowAssets.Name = "button_ShowAssets";
      this.button_ShowAssets.Size = new System.Drawing.Size(90, 23);
      this.button_ShowAssets.TabIndex = 7;
      this.button_ShowAssets.Text = "Show Assets";
      this.button_ShowAssets.Click += new System.EventHandler(this.button_ShowAssets_Click);
      // 
      // button_AllLayers
      // 
      this.button_AllLayers.Location = new System.Drawing.Point(264, 308);
      this.button_AllLayers.Name = "button_AllLayers";
      this.button_AllLayers.Size = new System.Drawing.Size(90, 23);
      this.button_AllLayers.TabIndex = 6;
      this.button_AllLayers.Text = "Select All";
      this.button_AllLayers.Click += new System.EventHandler(this.button_AllLayers_Click);
      // 
      // button_InvertLayers
      // 
      this.button_InvertLayers.Location = new System.Drawing.Point(360, 308);
      this.button_InvertLayers.Name = "button_InvertLayers";
      this.button_InvertLayers.Size = new System.Drawing.Size(90, 23);
      this.button_InvertLayers.TabIndex = 5;
      this.button_InvertLayers.Text = "Invert Selection";
      this.button_InvertLayers.Click += new System.EventHandler(this.button_InvertLayers_Click);
      // 
      // checkBox_LoadedZonesOnly
      // 
      this.checkBox_LoadedZonesOnly.AutoSize = true;
      this.checkBox_LoadedZonesOnly.Dock = System.Windows.Forms.DockStyle.Fill;
      this.checkBox_LoadedZonesOnly.Location = new System.Drawing.Point(3, 308);
      this.checkBox_LoadedZonesOnly.Name = "checkBox_LoadedZonesOnly";
      this.checkBox_LoadedZonesOnly.Size = new System.Drawing.Size(255, 23);
      this.checkBox_LoadedZonesOnly.TabIndex = 2;
      this.checkBox_LoadedZonesOnly.Text = "Export loaded Zones only";
      this.checkBox_LoadedZonesOnly.CheckedChanged += new System.EventHandler(this.ProfileControlChanged);
      // 
      // listView_Layers
      // 
      this.listView_Layers.AutoArrange = false;
      this.listView_Layers.CheckBoxes = true;
      this.listView_Layers.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.nameHeader,
            this.descriptionHeader});
      this.tableLayoutPanel1.SetColumnSpan(this.listView_Layers, 4);
      this.listView_Layers.Dock = System.Windows.Forms.DockStyle.Fill;
      this.listView_Layers.FullRowSelect = true;
      this.listView_Layers.GridLines = true;
      listViewGroup1.Header = "Layers to export";
      listViewGroup1.Name = "Layers";
      listViewGroup2.Header = "Zones to export";
      listViewGroup2.Name = "Zones";
      listViewGroup3.Header = "Scene Components to export";
      listViewGroup3.Name = "SceneComponents";
      listViewGroup4.Header = "Optional Plugins to embed";
      listViewGroup4.Name = "Plugins";
      this.listView_Layers.Groups.AddRange(new System.Windows.Forms.ListViewGroup[] {
            listViewGroup1,
            listViewGroup2,
            listViewGroup3,
            listViewGroup4});
      listViewItem1.Group = listViewGroup1;
      listViewItem1.StateImageIndex = 0;
      listViewItem2.Group = listViewGroup2;
      listViewItem2.StateImageIndex = 0;
      listViewItem3.Group = listViewGroup3;
      listViewItem3.StateImageIndex = 0;
      listViewItem4.Group = listViewGroup4;
      listViewItem4.StateImageIndex = 0;
      this.listView_Layers.Items.AddRange(new System.Windows.Forms.ListViewItem[] {
            listViewItem1,
            listViewItem2,
            listViewItem3,
            listViewItem4});
      this.listView_Layers.Location = new System.Drawing.Point(3, 3);
      this.listView_Layers.MultiSelect = false;
      this.listView_Layers.Name = "listView_Layers";
      this.listView_Layers.ShowItemToolTips = true;
      this.listView_Layers.Size = new System.Drawing.Size(543, 299);
      this.listView_Layers.TabIndex = 1;
      this.listView_Layers.UseCompatibleStateImageBehavior = false;
      this.listView_Layers.View = System.Windows.Forms.View.Details;
      this.listView_Layers.ItemChecked += new System.Windows.Forms.ItemCheckedEventHandler(this.listView_Layers_ItemChecked);
      // 
      // nameHeader
      // 
      this.nameHeader.Text = "Name";
      this.nameHeader.Width = 217;
      // 
      // descriptionHeader
      // 
      this.descriptionHeader.Text = "Description";
      this.descriptionHeader.Width = 357;
      // 
      // groupBox3
      // 
      this.groupBox3.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.groupBox3.Controls.Add(this.checkBox_SaveProfileOnExport);
      this.groupBox3.Controls.Add(this.label2);
      this.groupBox3.Controls.Add(this.button_SaveProfile);
      this.groupBox3.Controls.Add(this.button_CreateProfile);
      this.groupBox3.Controls.Add(this.comboBox_Profile);
      this.groupBox3.Location = new System.Drawing.Point(12, 76);
      this.groupBox3.Name = "groupBox3";
      this.groupBox3.Size = new System.Drawing.Size(555, 83);
      this.groupBox3.TabIndex = 1;
      this.groupBox3.TabStop = false;
      this.groupBox3.Text = "Export Presets";
      // 
      // checkBox_SaveProfileOnExport
      // 
      this.checkBox_SaveProfileOnExport.AutoSize = true;
      this.checkBox_SaveProfileOnExport.Location = new System.Drawing.Point(400, 53);
      this.checkBox_SaveProfileOnExport.Name = "checkBox_SaveProfileOnExport";
      this.checkBox_SaveProfileOnExport.Size = new System.Drawing.Size(132, 17);
      this.checkBox_SaveProfileOnExport.TabIndex = 4;
      this.checkBox_SaveProfileOnExport.Text = "Save Preset on Export";
      this.checkBox_SaveProfileOnExport.UseVisualStyleBackColor = true;
      // 
      // label2
      // 
      this.label2.AutoSize = true;
      this.label2.Location = new System.Drawing.Point(13, 22);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(40, 13);
      this.label2.TabIndex = 0;
      this.label2.Text = "Preset:";
      // 
      // button_SaveProfile
      // 
      this.button_SaveProfile.Image = ((System.Drawing.Image)(resources.GetObject("button_SaveProfile.Image")));
      this.button_SaveProfile.ImageAlign = System.Drawing.ContentAlignment.MiddleRight;
      this.button_SaveProfile.Location = new System.Drawing.Point(252, 46);
      this.button_SaveProfile.Name = "button_SaveProfile";
      this.button_SaveProfile.Size = new System.Drawing.Size(142, 28);
      this.button_SaveProfile.TabIndex = 2;
      this.button_SaveProfile.Text = "Save Preset";
      this.button_SaveProfile.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
      this.button_SaveProfile.UseVisualStyleBackColor = true;
      this.button_SaveProfile.Click += new System.EventHandler(this.button_SaveProfile_Click);
      // 
      // button_CreateProfile
      // 
      this.button_CreateProfile.Image = ((System.Drawing.Image)(resources.GetObject("button_CreateProfile.Image")));
      this.button_CreateProfile.ImageAlign = System.Drawing.ContentAlignment.MiddleRight;
      this.button_CreateProfile.Location = new System.Drawing.Point(104, 46);
      this.button_CreateProfile.Name = "button_CreateProfile";
      this.button_CreateProfile.Size = new System.Drawing.Size(142, 28);
      this.button_CreateProfile.TabIndex = 3;
      this.button_CreateProfile.Text = "Create Preset";
      this.button_CreateProfile.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
      this.button_CreateProfile.UseVisualStyleBackColor = true;
      this.button_CreateProfile.Click += new System.EventHandler(this.button_CreateProfile_Click);
      // 
      // comboBox_Profile
      // 
      this.comboBox_Profile.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.comboBox_Profile.FormattingEnabled = true;
      this.comboBox_Profile.Location = new System.Drawing.Point(104, 19);
      this.comboBox_Profile.Name = "comboBox_Profile";
      this.comboBox_Profile.Size = new System.Drawing.Size(290, 21);
      this.comboBox_Profile.TabIndex = 1;
      this.comboBox_Profile.SelectionChangeCommitted += new System.EventHandler(this.comboBox_Profile_SelectionChangeCommitted);
      // 
      // button_ExportSelectedProfiles
      // 
      this.button_ExportSelectedProfiles.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.button_ExportSelectedProfiles.Image = ((System.Drawing.Image)(resources.GetObject("button_ExportSelectedProfiles.Image")));
      this.button_ExportSelectedProfiles.ImageAlign = System.Drawing.ContentAlignment.MiddleRight;
      this.button_ExportSelectedProfiles.Location = new System.Drawing.Point(135, 695);
      this.button_ExportSelectedProfiles.Name = "button_ExportSelectedProfiles";
      this.button_ExportSelectedProfiles.Size = new System.Drawing.Size(169, 28);
      this.button_ExportSelectedProfiles.TabIndex = 6;
      this.button_ExportSelectedProfiles.Text = "Export Selected Profiles";
      this.button_ExportSelectedProfiles.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
      this.button_ExportSelectedProfiles.Click += new System.EventHandler(this.button_ExportSelectedProfiles_Click);
      // 
      // captionBar
      // 
      this.captionBar.BackColor = System.Drawing.SystemColors.Window;
      this.captionBar.Caption = "Exporting the Scene";
      this.captionBar.CompactView = false;
      this.captionBar.Description = resources.GetString("captionBar.Description");
      this.captionBar.Dock = System.Windows.Forms.DockStyle.Top;
      this.captionBar.Image = ((System.Drawing.Image)(resources.GetObject("captionBar.Image")));
      this.captionBar.Location = new System.Drawing.Point(0, 0);
      this.captionBar.Name = "captionBar";
      this.captionBar.SetFontColor = System.Drawing.SystemColors.ControlText;
      this.captionBar.ShowBorder = false;
      this.captionBar.ShowBottomLine = true;
      this.captionBar.ShowCaptionText = true;
      this.captionBar.ShowImage = true;
      this.captionBar.Size = new System.Drawing.Size(579, 69);
      this.captionBar.TabIndex = 0;
      // 
      // ExportDialog
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.CancelButton = this.button_CANCEL;
      this.ClientSize = new System.Drawing.Size(579, 735);
      this.Controls.Add(this.groupBox3);
      this.Controls.Add(this.groupBox2);
      this.Controls.Add(this.settingsBox);
      this.Controls.Add(this.groupBox1);
      this.Controls.Add(this.captionBar);
      this.Controls.Add(this.button_CANCEL);
      this.Controls.Add(this.button_ExportSelectedProfiles);
      this.Controls.Add(this.button_ExportActiveProfile);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "ExportDialog";
      this.ShowInTaskbar = false;
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
      this.Text = "Export Scene";
      this.Load += new System.EventHandler(this.ExportDialog_Load);
      this.groupBox1.ResumeLayout(false);
      this.groupBox1.PerformLayout();
      this.settingsBox.ResumeLayout(false);
      this.settingsBox.PerformLayout();
      this.groupBox2.ResumeLayout(false);
      this.tableLayoutPanel1.ResumeLayout(false);
      this.tableLayoutPanel1.PerformLayout();
      this.groupBox3.ResumeLayout(false);
      this.groupBox3.PerformLayout();
      this.ResumeLayout(false);

    }
		#endregion

    #region Get and set settings

    SceneExportProfile _settings = null;
    SceneExportProfile _settingsUnmodified = null;    

    /// <summary>
    /// Gets or sets the settings from which the export options are taken. The set function clones to local copy.
    /// The get function applies the export flags to each layer
    /// </summary>
    public SceneExportProfile Settings
    {
      get 
      {
        GetSettingsFromControls(); // read back from UI
        return _settings;
      }
      set 
      {
        if (value == null)
        {
          return;
        }

        _bForceReturnNotModified = true;

        // If a profile with the name of the settings to be set already exists, try to load it and use
        // it as the unmodified baseline for our settings.
        SceneExportProfile existingProfile = SceneExportProfile.LoadProfile(EditorManager.Scene, value.ProfileName, true);
        _settingsUnmodified = existingProfile != null ? existingProfile : (SceneExportProfile)value.Clone();

        // If a combo box entry for the settings doesn't yet exist, create one.
        if (FindProfileNameInComboBox(value.ProfileName) < 0)
        {
          comboBox_Profile.Items.Add(value.ProfileName);
        }

        _settings = (SceneExportProfile)value.Clone();

        // set in the controls:
        text_Pathname.Text = MakeExportBaseName(_settings.ExportPath, false);
        dropDown_AssetProfiles.Profiles = (ProfileEnum)_settings.SelectedAssetProfiles.Clone();
        checkBox_RunAfterExport.Checked = _settings.RunAfterExport;
        checkBox_IncludeVisibilityInfo.Checked = _settings.IncludeVisibilityInfo;
        checkBox_InvisibleLayersExportInvisibleObjects.Checked = _settings.InvisibleLayersExportInvisibleObjects;
        checkBox_LoadedZonesOnly.Checked = _settings.LoadedZonesOnly;
        checkBox_updateAssetTransformations.Checked = _settings.UpdateAssetTransformations;

        UIExportFlags = (SceneExportFlags_e)_settings.ExportFlags;

        UpdateListBox();
        UpdateButtonStates();

        _bForceReturnNotModified = false;
      }
    }

    uint _exportFlags = 0xffffffff;

    SceneExportFlags_e UIExportFlags
    {
      get
      {
        uint iFlags = 0xffffffff; // all other flags should be set
        foreach (ListViewItem item in listView_Layers.Items)
        {
          if (!(item.Tag is SceneExportFlags_e))
            continue;
          SceneExportFlags_e iValue = (SceneExportFlags_e)item.Tag;
          if (!item.Checked)
            iFlags &= ~((uint)iValue);
        }
        
        return (SceneExportFlags_e)iFlags;
      }
      set
      {
        _exportFlags = (uint)value;
        foreach (ListViewItem item in listView_Layers.Items)
        {
          if (!(item.Tag is SceneExportFlags_e))
            continue;
          SceneExportFlags_e iValue = (SceneExportFlags_e)item.Tag;
          item.Checked = (value & iValue) != 0;
        }
      }
    }

    bool _bForceReturnModified = false;
    bool _bForceReturnNotModified = false;

    bool CurrentSettingModified
    {
      get
      {
        if (_settingsUnmodified == null || _settings == null || !_bDlgShown)
          return false;
        if (_bForceReturnModified)
          return true;
        if (_bForceReturnNotModified)
          return false;
        SceneExportProfile current = Settings; // get from UI;
        return !current.Equals(_settingsUnmodified);
      }
    }

    void GetSettingsFromControls()
    {
      if (!_bDlgShown) // controls are not valid yet
        return;

      _settings.ExportPath = text_Pathname.Text.Trim() + FILE_EXTENSION_VSCENE;
      _settings.SelectedAssetProfiles = (ProfileEnum)dropDown_AssetProfiles.Profiles.Clone();
      _settings.RunAfterExport = checkBox_RunAfterExport.Checked;
      _settings.IncludeVisibilityInfo = checkBox_IncludeVisibilityInfo.Checked;
      _settings.InvisibleLayersExportInvisibleObjects = checkBox_InvisibleLayersExportInvisibleObjects.Checked;
      _settings.LoadedZonesOnly = checkBox_LoadedZonesOnly.Checked;
      _settings.ExportFlags = (uint)UIExportFlags;
      _settings.UpdateAssetTransformations = checkBox_updateAssetTransformations.Checked;

      _settings.ExportedLayers.Clear();
      _settings.ExportedZones.Clear();
      _settings.PluginsUserSelection.Clear();

      foreach (ListViewItem item in listView_Layers.Items)
      {
        // plugins:
        if (item.Tag is string)
          _settings.PluginsUserSelection[(string)item.Tag] = item.Checked;

        if (!item.Checked)
          continue;
        if (item.Tag is Layer)
          _settings.ExportedLayers.Add(item.Tag);
        if (item.Tag is Zone)
          _settings.ExportedZones.Add(item.Tag);
      }
      // fill up layer collection with layers that are not in the UI. This way the profiles are comparable
      foreach (Zone zone in _settings.ExportedZones)
      {
        foreach (Layer layer in zone.Layers)
          if (layer.Export && !_settings.ExportedLayers.Contains(layer))
            _settings.ExportedLayers.Add(layer);
      }
    }

    private bool ValidateSettings()
    {
      GetSettingsFromControls();
      char[] otherInvalidChars = new char[] { '*', '?' };
      if (Path.IsPathRooted(_settings.ExportPath)
        || _settings.ExportPath.Equals(FILE_EXTENSION_VSCENE, StringComparison.InvariantCultureIgnoreCase)
        || _settings.ExportPath.IndexOfAny(Path.GetInvalidPathChars()) >= 0
        || _settings.ExportPath.IndexOfAny(otherInvalidChars) >= 0)
      {
        EditorManager.ShowMessageBox(
          "The export path is not valid.\nIt must be a file name or a relative path, and it must\nonly contain characters that are allowed in file names.",
          "Export path invalid", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        text_Pathname.Focus();
        return false;
      }
      
      return true;
    }

    public bool ExportActiveProfileOnly = false;

    /// <summary>
    /// Confirm this dialog
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void button_ExportActiveProfile_Click(object sender, System.EventArgs e)
    {
      if (!ValidateSettings() || !CheckSaveExportProfile())
        return;

      Settings.ExportedLayersToScene();
      ExportActiveProfileOnly = true;

      DialogResult = DialogResult.OK;
    }

    /// <summary>
    /// Confirm this dialog
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void button_ExportSelectedProfiles_Click(object sender, System.EventArgs e)
    {
      if (!ValidateSettings() || !CheckSaveExportProfile())
        return;

      Settings.ExportedLayersToScene();
      ExportActiveProfileOnly = false;

      DialogResult = DialogResult.OK;
    }

    #endregion

    #region Export filename / path

    static readonly string FILE_EXTENSION_VSCENE = ".vscene";

    private string MakeExportBaseName(string path, bool removeProfile)
    {
      string pathOut = path;
      if (pathOut.EndsWith(FILE_EXTENSION_VSCENE, StringComparison.InvariantCultureIgnoreCase))
        pathOut = pathOut.Remove(pathOut.Length - FILE_EXTENSION_VSCENE.Length);

      if (!removeProfile)
        return pathOut;

      foreach (IProfileManager.Profile profile in EditorManager.ProfileManager.GetProfiles())
      {
        string profileExtension = "." + profile._name;
        if (pathOut.EndsWith(profileExtension, StringComparison.InvariantCultureIgnoreCase))
        {
          pathOut = pathOut.Remove(pathOut.Length - profileExtension.Length);
          break;
        }
      }

      return pathOut;
    }

    private void button_Browse_Click(object sender, System.EventArgs e)
    {
      OpenFileDlg fileDlg = new OpenFileDlg();
      fileDlg.ShowBaseData = false;
      string exportDir = "";
      if (_settings.ExportPath!=null)
        exportDir = System.IO.Path.GetDirectoryName(_settings.ExportPath);

      if (System.IO.Directory.Exists(exportDir))
        fileDlg.InitialDirectory = exportDir;
      else
        fileDlg.InitialDirectory = EditorManager.Project.ProjectDir;
      fileDlg.Filter = new string[] { "." + SceneExportProfile.FILE_EXTENSION_EXPORT };
      if (fileDlg.ShowDialog() != DialogResult.OK)
        return;

      string pathBase = MakeExportBaseName(EditorManager.Project.MakeRelative(fileDlg.FileName), true);

      text_Pathname.Text = pathBase;

      OnCheckProfileModified();
    }

    private void checkBox_LoadedZonesOnly_CheckedChanged(object sender, EventArgs e)
    {
      _settings.LoadedZonesOnly = checkBox_LoadedZonesOnly.Checked;
      UpdateButtonStates();
      UpdateListBox();
    }

    #endregion

    #region Layer listview

    private void listView_Layers_ItemChecked(object sender, ItemCheckedEventArgs e)
    {
      if (bListBuildInProgress)
        return;
      this.OnCheckProfileModified();
    }

    bool bListBuildInProgress = false;

    void UpdateListBox()
    {
      System.Diagnostics.Debug.Assert(bListBuildInProgress == false);
      bListBuildInProgress = true;
      listView_Layers.BeginUpdate();
      listView_Layers.Items.Clear();
      
      IScene scene = EditorManager.Scene;

      // layers that can be exported
      foreach (Layer layer in scene.Layers)
      {
        if (layer.ParentZone != null)
          continue;

        string itemName = layer.LayerName;
        Color textColor = Color.Black;
        if (!layer.VisibleInAssetProfile.IsActiveProfileSet)
        {
          itemName += " (disabled by asset profile)";
          textColor = Color.Gray;
        }
        ListViewItem item = listView_Layers.Items.Add(itemName);
        item.ForeColor = textColor;
        item.ImageIndex = LayerTreeViewNodeBase.GetLayerIcon(layer);
        item.Tag = layer;
        item.Checked = _settings.ExportedLayers.Contains(layer); // layer.Export;
        item.Group = listView_Layers.Groups["Layers"];

        item.SubItems.Add(layer.Description);
        item.ToolTipText = layer.AbsoluteLayerFilename;
      }

      // zones that can be exported
      foreach (Zone zone in scene.Zones)
      {
        // [#24564] Not modifiable zones can still be exported, otherwise this would not be consistent with unparented layers
        if (_settings.LoadedZonesOnly && !zone.Loaded /* || !zone.Modifiable*/)
          continue;
        ListViewItem item = listView_Layers.Items.Add(zone.ZoneName);
        item.ImageIndex = LayerTreeViewNodeBase.GetZoneIcon(zone);
        item.Tag = zone;
        item.Checked = _settings.ExportedZones.Contains(zone); // zone.Export;
        item.Group = listView_Layers.Groups["Zones"];

        item.SubItems.Add(zone.Description);
        item.ToolTipText = zone.AbsoluteZoneFilename;
      }

      // scene components
      // fill flags box
      Type enumType = typeof(SceneExportFlags_e);
      string[] flags = Enum.GetNames(enumType);
      Array values = Enum.GetValues(enumType);
      for (int i = 0; i < flags.Length; i++)
      {        
        ListViewItem item = listView_Layers.Items.Add(flags[i]);
        item.Tag = values.GetValue(i);
        item.Group = listView_Layers.Groups["SceneComponents"];

        FieldInfo fi = enumType.GetField(flags[i]);
        DescriptionAttribute[] descAttr = (DescriptionAttribute[])fi.GetCustomAttributes(typeof(DescriptionAttribute), true);
        if (descAttr.Length == 1)
          item.ToolTipText = descAttr[0].Description;
          //item.SubItems.Add(descAttr[0].Description);
      }
      UIExportFlags = (SceneExportFlags_e)_exportFlags;

      // custom plugins state
      Dictionary<string, bool> relevantPlugins = new Dictionary<string, bool>();
      ExportHelper.GetPluginsForUserSelection(relevantPlugins);
      foreach (KeyValuePair<string, bool> plugin in relevantPlugins)
      {
        string name = plugin.Key;
        ListViewItem item = listView_Layers.Items.Add(name);
        bool bState = plugin.Value;
        // sync with the settings:
        _settings.PluginsUserSelection.TryGetValue(name, out bState);

        item.Tag = name;
        item.Group = listView_Layers.Groups["Plugins"];
        item.ToolTipText = "If enabled, this plugin will be loaded by the vSceneViewer";
        item.Checked = bState;
      }

      listView_Layers.EndUpdate();
      bListBuildInProgress = false;
    }

    private void button_AllLayers_Click(object sender, System.EventArgs e)
    {
      listView_Layers.BeginUpdate();
      bListBuildInProgress = true;
      foreach (ListViewItem item in listView_Layers.Items)
        item.Checked = true;
      listView_Layers.EndUpdate();
      bListBuildInProgress = false;
    }

    private void button_InvertLayers_Click(object sender, System.EventArgs e)
    {
      listView_Layers.BeginUpdate();
      bListBuildInProgress = true;
      foreach (ListViewItem item in listView_Layers.Items)
        item.Checked = !item.Checked;
      listView_Layers.EndUpdate();
      bListBuildInProgress = false;
    }

    private void button_ShowAssets_Click(object sender, EventArgs e)
    {
      ShowAssetListDlg dlg = new ShowAssetListDlg();

      ResourceSnapshotInfo info = new ResourceSnapshotInfo();
      ExportHelper.StartResourceSnapshot(info);

      foreach (ListViewItem item in listView_Layers.Items)
      {
        if (!item.Checked)
          continue;
        if (item.Tag is Layer)
          ((Layer)item.Tag).GetDependencies(info);
        else if (item.Tag is Zone)
          ((Zone)item.Tag).GetDependencies(info);
      }
      string desc = "This list shows the native assets used by the current export profile";
      dlg.Description = desc;
      dlg.AssetList = EditorManager.AssetManager.ExtractAssetInfo(info);
      ExportHelper.EndResourceSnapshot(info); // after extracting

      dlg.ShowDialog();
    }

    #endregion

    #region Export profile

    public bool AutoSaveExportProfile
    {
      get { return checkBox_SaveProfileOnExport.Checked; }
      set { checkBox_SaveProfileOnExport.Checked = value; }
    }

    bool PromptProfileModified()
    {
      if (!CurrentSettingModified)
        return true;

      DialogResult res = EditorManager.ShowMessageBox("The current profile '" + _settings.ProfileName + "' has been modified since\nlast saving.\n\n" +
        "Select 'Yes' to save the profile\n" +
        "Select 'No' to discard the changes\n" +
        "Select 'Cancel' to abort profile switching\n"
        , "Save profile changes?", MessageBoxButtons.YesNoCancel, MessageBoxIcon.Question);

      if (res == DialogResult.Cancel)
        return false;
      if (res == DialogResult.Yes)
      {
        if (!SaveProfile(_settings))
          return false;
        Settings = _settings; // create unmodified copy
        return true;
      }

      return true;
    }


    int FindProfileNameInComboBox(string name)
    {
      for (int i = 0; i < comboBox_Profile.Items.Count; i++)
        if (string.Compare(comboBox_Profile.Items[i].ToString(), name, true) == 0)
          return i;
      return -1;
    }

    void FillProfileCombobox()
    {
      comboBox_Profile.Items.Clear();

      try
      {
        DirectoryInfo layerFolder = new DirectoryInfo(EditorManager.Scene.LayerDirectoryName);
        FileInfo[] files = layerFolder.GetFiles("*." + SceneExportProfile.FILE_EXTENSION_EXPORTPROFILE);
        foreach (FileInfo file in files)
        {
          string name = Path.GetFileNameWithoutExtension(file.Name);
          comboBox_Profile.Items.Add(name);
        }
      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex);
      }

    }

    bool CheckSaveExportProfile()
    {
      if (!checkBox_SaveProfileOnExport.Checked || !CurrentSettingModified)
        return true;

      return SaveProfile(Settings);
    }

    bool SaveProfile(SceneExportProfile settings)
    {
      if (!settings.SaveToFile())
      {
        EditorManager.ShowMessageBox("Failed to save export profile '" + settings.ProfileName + "'", "Export Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        return false;
      }
      Settings = settings; // clone and update UI
      _settingsUnmodified = (SceneExportProfile)settings.Clone();

      return true;
    }

    private void button_CreateProfile_Click(object sender, EventArgs e)
    {
      if (!PromptProfileModified())
        return;

      CreateFileDlg dlg = new CreateFileDlg();
      dlg.InitialDirectory = EditorManager.Scene.LayerDirectoryName;
      dlg.Caption = "Creating a new Export Profile";
      dlg.Description = "Enter the name of the new profile file.";
      dlg.Ext = "." + SceneExportProfile.FILE_EXTENSION_EXPORTPROFILE;
      dlg.Filter = new string[] { dlg.Ext.ToLower() };
      dlg.AllowOverwrite = false;
      dlg.SupportCustomDirectories = false;
      dlg.AllowFolderCreation = false;

      if (dlg.ShowDialog(this) != DialogResult.OK)
        return;
      SceneExportProfile settings = Settings;
      string oldName = settings.ProfileName;
      settings.ProfileName = Path.GetFileNameWithoutExtension(dlg.FileName);
      if (!SaveProfile(settings))
      {
        settings.ProfileName = oldName;
        return;
      }
      Settings = settings; // create unmodified copy
    }

    private void comboBox_Profile_SelectionChangeCommitted(object sender, EventArgs e)
    {
      if (comboBox_Profile.SelectedItem == null)
        return;
      string newName = comboBox_Profile.SelectedItem.ToString();
      if (_settings != null && string.Compare(_settings.ProfileName, newName, true) == 0)
        return;
      if (!PromptProfileModified())
      {
        // switch back to old profile
        UpdateButtonStates();
        return;
      }
      SceneExportProfile newProfile = SceneExportProfile.LoadProfile(EditorManager.Scene, newName, true);
      if (newProfile == null)
      {
        EditorManager.ShowMessageBox("Failed to load '" + newName + "'", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        UpdateButtonStates();
        return;
      }
      Settings = newProfile; // clone and apply to UI
      _settingsUnmodified = (SceneExportProfile)newProfile.Clone();
    }

    private void button_SaveProfile_Click(object sender, EventArgs e)
    {
      if (!ValidateSettings())
        return;

      SaveProfile(Settings);

      // Transfer settings to scene, so even if we cancel the export, we get the saved settings
      // when opening the export dialog again.
      if (EditorManager.Scene != null)
        EditorManager.Scene.CurrentExportProfile = Settings;
    }

    #endregion

    #region UI callbacks

    bool _bDlgShown = false;

    protected override void OnVisibleChanged(EventArgs e)
    {
      base.OnVisibleChanged(e);
      if (Visible)
      {
        _bDlgShown = true;
        UpdateButtonStates();
      }
    }

    void UpdateButtonStates()
    {
      //textBox_ReplacementPath.Enabled = checkBox_BakeReplacementResources.Checked;
      //button_BrowseReplacementPath.Enabled = checkBox_BakeReplacementResources.Checked;
      button_SaveProfile.Enabled = this.CurrentSettingModified;

      // find selected index
      if (_settings != null)
      {
        comboBox_Profile.SelectedIndex = FindProfileNameInComboBox(_settings.ProfileName);
      }
    }
    
    void OnCheckProfileModified()
    {
      button_SaveProfile.Enabled = this.CurrentSettingModified;
      UpdateListBox();
    }

    private void ProfileControlChanged(object sender, EventArgs e)
    {
      this.OnCheckProfileModified();
    }

    #endregion
  }
}

/*
 * Havok SDK - Base file, BUILD(#20130624)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available from salesteam@havok.com.
 * 
 */
