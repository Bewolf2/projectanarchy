using CSharpFramework;
using System;
using CSharpFramework.Controls;
namespace TerrainEditorPlugin.Dialogs
{
	partial class TerrainEditorPanel
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(TerrainEditorPanel));
      this.tabControl = new System.Windows.Forms.TabControl();
      this.tabPage_Heightmap = new System.Windows.Forms.TabPage();
      this.splitter3 = new System.Windows.Forms.Splitter();
      this.groupBox_ExportDDS = new CSharpFramework.Controls.CustomGroupBox();
      this.label_ExportHeightmap = new System.Windows.Forms.Label();
      this.label_RemapToRange = new System.Windows.Forms.Label();
      this.button_ExportFilenameBrowse = new System.Windows.Forms.Button();
      this.textBox_ExportDDS = new System.Windows.Forms.TextBox();
      this.checkBox_NormalizedHMExport = new System.Windows.Forms.CheckBox();
      this.targetLabel_ExportDDS = new System.Windows.Forms.Label();
      this.button_ExportHeightmapDDS = new System.Windows.Forms.Button();
      this.splitter2 = new System.Windows.Forms.Splitter();
      this.groupBox_Import = new CSharpFramework.Controls.CustomGroupBox();
      this.comboBox_ApplyFilter = new System.Windows.Forms.ComboBox();
      this.label_ApplyFilter = new System.Windows.Forms.Label();
      this.comboBox_ImportHeightfield = new System.Windows.Forms.ComboBox();
      this.label_ImportHeightfield = new System.Windows.Forms.Label();
      this.groupBox_Terrain = new CSharpFramework.Controls.CustomGroupBox();
      this.label_TerrainSettings = new System.Windows.Forms.Label();
      this.button_ChangeConfig = new System.Windows.Forms.Button();
      this.splitter1 = new System.Windows.Forms.Splitter();
      this.tabPage_Texturing = new System.Windows.Forms.TabPage();
      this.splitContainer3 = new System.Windows.Forms.SplitContainer();
      this.listView_DetailTextures = new System.Windows.Forms.ListView();
      this.columnHeader_Name = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
      this.columnHeader_File = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
      this.contextMenuStrip_DetailTexture = new System.Windows.Forms.ContextMenuStrip(this.components);
      this.ToolStripMenuItem_ExportTextureLuminance = new System.Windows.Forms.ToolStripMenuItem();
      this.propertyGrid_DetailTexture = new CSharpFramework.Controls.PropertyGridNested();
      this.toolStrip_Texturing = new System.Windows.Forms.ToolStrip();
      this.toolStripButton_AddDetailTexture = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_DeleteDetailTexture = new System.Windows.Forms.ToolStripButton();
      this.toolStripSeparator5 = new System.Windows.Forms.ToolStripSeparator();
      this.toolStripButton_MoveUp = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_MoveDown = new System.Windows.Forms.ToolStripButton();
      this.toolStripSeparator6 = new System.Windows.Forms.ToolStripSeparator();
      this.toolStripButton_Blink = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_MaterialID = new System.Windows.Forms.ToolStripButton();
      this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
      this.toolStripButton_ImportWeightmap = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_SlopeTextures = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_SplitPerSectorTex = new System.Windows.Forms.ToolStripButton();
      this.tabPage_Decoration = new System.Windows.Forms.TabPage();
      this.splitContainer2 = new System.Windows.Forms.SplitContainer();
      this.listView_VegetationModels = new System.Windows.Forms.ListView();
      this.columnHeader1 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
      this.columnHeader2 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
      this.contextMenuStrip_Decoration = new System.Windows.Forms.ContextMenuStrip(this.components);
      this.toolStripMenuItem_DecorationFilters = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripSeparator();
      this.ToolStripMenuItem_ExportDensity = new System.Windows.Forms.ToolStripMenuItem();
      this.createLODVersionToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.propertyGrid_VegetationModel = new CSharpFramework.Controls.PropertyGridNested();
      this.toolStrip_Vegetation = new System.Windows.Forms.ToolStrip();
      this.toolStripButton_AddDecoChannel = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_DeleteDecoChannel = new System.Windows.Forms.ToolStripButton();
      this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
      this.toolStripSplitButton_Atlas = new System.Windows.Forms.ToolStripSplitButton();
      this.createNewBillboardAtlasToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.editExistingBillboardAtlasToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripSeparator7 = new System.Windows.Forms.ToolStripSeparator();
      this.toolStripButton_RebuildDirtyRange = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_RebuildVegetation = new System.Windows.Forms.ToolStripButton();
      this.toolStripSeparator8 = new System.Windows.Forms.ToolStripSeparator();
      this.toolStripButton_AddMeshComponent = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_RemoveMeshComponent = new System.Windows.Forms.ToolStripButton();
      this.tabPage_Shader = new System.Windows.Forms.TabPage();
      this.propertyGrid_Shader = new CSharpFramework.Controls.PropertyGridNested();
      this.tabPage_MeshExport = new System.Windows.Forms.TabPage();
      this.propertyGrid_MeshExport = new System.Windows.Forms.PropertyGrid();
      this.splitter5 = new System.Windows.Forms.Splitter();
      this.groupBox_MeshExport = new CSharpFramework.Controls.CustomGroupBox();
      this.label_ProcessExport = new System.Windows.Forms.Label();
      this.button_ExportPrefab = new System.Windows.Forms.Button();
      this.label_Export = new System.Windows.Forms.Label();
      this.splitter4 = new System.Windows.Forms.Splitter();
      this.toolStrip_Main = new System.Windows.Forms.ToolStrip();
      this.toolStripButton_Elevate = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_Flatten = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_Smooth = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_PickHeight = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_EditHoles = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_PaintDetailTexture = new System.Windows.Forms.ToolStripButton();
      this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
      this.toolStripButton_TextureBaked = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_DirectionalLighting = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_FullRes = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_DisplayVegetationDensity = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_DisplayMiniMap = new System.Windows.Forms.ToolStripButton();
      this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
      this.toolStripButton_Selection = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_PurgeResources = new System.Windows.Forms.ToolStripButton();
      this.splitContainer1 = new System.Windows.Forms.SplitContainer();
      this.dialogCaptionBar = new CSharpFramework.Controls.DialogCaptionBar();
      this.tabControl.SuspendLayout();
      this.tabPage_Heightmap.SuspendLayout();
      this.groupBox_ExportDDS.SuspendLayout();
      this.groupBox_Import.SuspendLayout();
      this.groupBox_Terrain.SuspendLayout();
      this.tabPage_Texturing.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.splitContainer3)).BeginInit();
      this.splitContainer3.Panel1.SuspendLayout();
      this.splitContainer3.Panel2.SuspendLayout();
      this.splitContainer3.SuspendLayout();
      this.contextMenuStrip_DetailTexture.SuspendLayout();
      this.toolStrip_Texturing.SuspendLayout();
      this.tabPage_Decoration.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).BeginInit();
      this.splitContainer2.Panel1.SuspendLayout();
      this.splitContainer2.Panel2.SuspendLayout();
      this.splitContainer2.SuspendLayout();
      this.contextMenuStrip_Decoration.SuspendLayout();
      this.toolStrip_Vegetation.SuspendLayout();
      this.tabPage_Shader.SuspendLayout();
      this.tabPage_MeshExport.SuspendLayout();
      this.groupBox_MeshExport.SuspendLayout();
      this.toolStrip_Main.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
      this.splitContainer1.Panel1.SuspendLayout();
      this.splitContainer1.SuspendLayout();
      this.SuspendLayout();
      // 
      // tabControl
      // 
      this.tabControl.Controls.Add(this.tabPage_Heightmap);
      this.tabControl.Controls.Add(this.tabPage_Texturing);
      this.tabControl.Controls.Add(this.tabPage_Decoration);
      this.tabControl.Controls.Add(this.tabPage_Shader);
      this.tabControl.Controls.Add(this.tabPage_MeshExport);
      this.tabControl.Dock = System.Windows.Forms.DockStyle.Fill;
      this.tabControl.Location = new System.Drawing.Point(0, 0);
      this.tabControl.Name = "tabControl";
      this.tabControl.SelectedIndex = 0;
      this.tabControl.Size = new System.Drawing.Size(433, 406);
      this.tabControl.TabIndex = 0;
      this.tabControl.SelectedIndexChanged += new System.EventHandler(this.tabControl_SelectedIndexChanged);
      // 
      // tabPage_Heightmap
      // 
      this.tabPage_Heightmap.BackColor = System.Drawing.Color.White;
      this.tabPage_Heightmap.Controls.Add(this.splitter3);
      this.tabPage_Heightmap.Controls.Add(this.groupBox_ExportDDS);
      this.tabPage_Heightmap.Controls.Add(this.splitter2);
      this.tabPage_Heightmap.Controls.Add(this.groupBox_Import);
      this.tabPage_Heightmap.Controls.Add(this.groupBox_Terrain);
      this.tabPage_Heightmap.Controls.Add(this.splitter1);
      this.tabPage_Heightmap.Location = new System.Drawing.Point(4, 22);
      this.tabPage_Heightmap.Name = "tabPage_Heightmap";
      this.tabPage_Heightmap.Padding = new System.Windows.Forms.Padding(6);
      this.tabPage_Heightmap.Size = new System.Drawing.Size(425, 380);
      this.tabPage_Heightmap.TabIndex = 0;
      this.tabPage_Heightmap.Text = "Heightmap";
      this.tabPage_Heightmap.Click += new System.EventHandler(this.tabPage_Heightmap_Click);
      // 
      // splitter3
      // 
      this.splitter3.Dock = System.Windows.Forms.DockStyle.Top;
      this.splitter3.Location = new System.Drawing.Point(6, 250);
      this.splitter3.Name = "splitter3";
      this.splitter3.Size = new System.Drawing.Size(413, 6);
      this.splitter3.TabIndex = 6;
      this.splitter3.TabStop = false;
      // 
      // groupBox_ExportDDS
      // 
      this.groupBox_ExportDDS.Collapsable = false;
      this.groupBox_ExportDDS.ColorHeader = System.Drawing.SystemColors.Control;
      this.groupBox_ExportDDS.ColorHover = System.Drawing.SystemColors.ActiveCaption;
      this.groupBox_ExportDDS.Controls.Add(this.label_ExportHeightmap);
      this.groupBox_ExportDDS.Controls.Add(this.label_RemapToRange);
      this.groupBox_ExportDDS.Controls.Add(this.button_ExportFilenameBrowse);
      this.groupBox_ExportDDS.Controls.Add(this.textBox_ExportDDS);
      this.groupBox_ExportDDS.Controls.Add(this.checkBox_NormalizedHMExport);
      this.groupBox_ExportDDS.Controls.Add(this.targetLabel_ExportDDS);
      this.groupBox_ExportDDS.Controls.Add(this.button_ExportHeightmapDDS);
      this.groupBox_ExportDDS.Dock = System.Windows.Forms.DockStyle.Top;
      this.groupBox_ExportDDS.HeadOutlineColor = System.Drawing.SystemColors.ControlText;
      this.groupBox_ExportDDS.Location = new System.Drawing.Point(6, 150);
      this.groupBox_ExportDDS.Name = "groupBox_ExportDDS";
      this.groupBox_ExportDDS.Padding = new System.Windows.Forms.Padding(3, 8, 3, 3);
      this.groupBox_ExportDDS.Size = new System.Drawing.Size(413, 100);
      this.groupBox_ExportDDS.SizeChangePropagationLevel = 0;
      this.groupBox_ExportDDS.TabIndex = 2;
      this.groupBox_ExportDDS.TabStop = false;
      this.groupBox_ExportDDS.Text = "Export Heightmap to DDS";
      this.groupBox_ExportDDS.Enter += new System.EventHandler(this.groupBox_ExportDDS_Enter);
      // 
      // label_ExportHeightmap
      // 
      this.label_ExportHeightmap.Location = new System.Drawing.Point(6, 72);
      this.label_ExportHeightmap.Name = "label_ExportHeightmap";
      this.label_ExportHeightmap.Size = new System.Drawing.Size(160, 20);
      this.label_ExportHeightmap.TabIndex = 5;
      this.label_ExportHeightmap.Text = "Process Export";
      this.label_ExportHeightmap.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
      // 
      // label_RemapToRange
      // 
      this.label_RemapToRange.Location = new System.Drawing.Point(6, 48);
      this.label_RemapToRange.Name = "label_RemapToRange";
      this.label_RemapToRange.Size = new System.Drawing.Size(160, 20);
      this.label_RemapToRange.TabIndex = 5;
      this.label_RemapToRange.Text = "Remap Heights to [0..1] Range";
      this.label_RemapToRange.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
      // 
      // button_ExportFilenameBrowse
      // 
      this.button_ExportFilenameBrowse.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.button_ExportFilenameBrowse.Location = new System.Drawing.Point(377, 24);
      this.button_ExportFilenameBrowse.Name = "button_ExportFilenameBrowse";
      this.button_ExportFilenameBrowse.Size = new System.Drawing.Size(30, 20);
      this.button_ExportFilenameBrowse.TabIndex = 2;
      this.button_ExportFilenameBrowse.Text = "...";
      this.button_ExportFilenameBrowse.UseVisualStyleBackColor = true;
      this.button_ExportFilenameBrowse.Click += new System.EventHandler(this.button_ExportFilenameBrowse_Click);
      // 
      // textBox_ExportDDS
      // 
      this.textBox_ExportDDS.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.textBox_ExportDDS.Location = new System.Drawing.Point(178, 24);
      this.textBox_ExportDDS.Name = "textBox_ExportDDS";
      this.textBox_ExportDDS.ReadOnly = true;
      this.textBox_ExportDDS.Size = new System.Drawing.Size(193, 20);
      this.textBox_ExportDDS.TabIndex = 1;
      this.textBox_ExportDDS.TextChanged += new System.EventHandler(this.textBox_ExportDDS_TextChanged);
      this.textBox_ExportDDS.Enter += new System.EventHandler(this.textBox_ExportDDS_Enter);
      this.textBox_ExportDDS.Leave += new System.EventHandler(this.textBox_ExportDDS_Leave);
      // 
      // checkBox_NormalizedHMExport
      // 
      this.checkBox_NormalizedHMExport.AutoSize = true;
      this.checkBox_NormalizedHMExport.Location = new System.Drawing.Point(178, 51);
      this.checkBox_NormalizedHMExport.Name = "checkBox_NormalizedHMExport";
      this.checkBox_NormalizedHMExport.Size = new System.Drawing.Size(15, 14);
      this.checkBox_NormalizedHMExport.TabIndex = 3;
      this.checkBox_NormalizedHMExport.UseVisualStyleBackColor = true;
      this.checkBox_NormalizedHMExport.CheckedChanged += new System.EventHandler(this.checkBox_NormalizedHMExport_CheckedChanged);
      // 
      // targetLabel_ExportDDS
      // 
      this.targetLabel_ExportDDS.Location = new System.Drawing.Point(6, 24);
      this.targetLabel_ExportDDS.Name = "targetLabel_ExportDDS";
      this.targetLabel_ExportDDS.Size = new System.Drawing.Size(160, 20);
      this.targetLabel_ExportDDS.TabIndex = 0;
      this.targetLabel_ExportDDS.Text = "Current Target Texture";
      this.targetLabel_ExportDDS.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
      // 
      // button_ExportHeightmapDDS
      // 
      this.button_ExportHeightmapDDS.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.button_ExportHeightmapDDS.Location = new System.Drawing.Point(178, 71);
      this.button_ExportHeightmapDDS.Name = "button_ExportHeightmapDDS";
      this.button_ExportHeightmapDDS.Size = new System.Drawing.Size(229, 22);
      this.button_ExportHeightmapDDS.TabIndex = 4;
      this.button_ExportHeightmapDDS.Text = "Export";
      this.button_ExportHeightmapDDS.UseVisualStyleBackColor = true;
      this.button_ExportHeightmapDDS.Click += new System.EventHandler(this.button_ExportHeightmapDDS_Click);
      // 
      // splitter2
      // 
      this.splitter2.Dock = System.Windows.Forms.DockStyle.Top;
      this.splitter2.Location = new System.Drawing.Point(6, 144);
      this.splitter2.Name = "splitter2";
      this.splitter2.Size = new System.Drawing.Size(413, 6);
      this.splitter2.TabIndex = 5;
      this.splitter2.TabStop = false;
      // 
      // groupBox_Import
      // 
      this.groupBox_Import.Collapsable = false;
      this.groupBox_Import.ColorHeader = System.Drawing.SystemColors.Control;
      this.groupBox_Import.ColorHover = System.Drawing.SystemColors.ActiveCaption;
      this.groupBox_Import.Controls.Add(this.comboBox_ApplyFilter);
      this.groupBox_Import.Controls.Add(this.label_ApplyFilter);
      this.groupBox_Import.Controls.Add(this.comboBox_ImportHeightfield);
      this.groupBox_Import.Controls.Add(this.label_ImportHeightfield);
      this.groupBox_Import.Dock = System.Windows.Forms.DockStyle.Top;
      this.groupBox_Import.HeadOutlineColor = System.Drawing.SystemColors.ControlText;
      this.groupBox_Import.Location = new System.Drawing.Point(6, 64);
      this.groupBox_Import.Name = "groupBox_Import";
      this.groupBox_Import.Size = new System.Drawing.Size(413, 80);
      this.groupBox_Import.SizeChangePropagationLevel = 0;
      this.groupBox_Import.TabIndex = 1;
      this.groupBox_Import.TabStop = false;
      this.groupBox_Import.Text = "Import and modify heightmap";
      // 
      // comboBox_ApplyFilter
      // 
      this.comboBox_ApplyFilter.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.comboBox_ApplyFilter.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.comboBox_ApplyFilter.FormattingEnabled = true;
      this.comboBox_ApplyFilter.Location = new System.Drawing.Point(178, 51);
      this.comboBox_ApplyFilter.Name = "comboBox_ApplyFilter";
      this.comboBox_ApplyFilter.Size = new System.Drawing.Size(229, 21);
      this.comboBox_ApplyFilter.TabIndex = 3;
      this.comboBox_ApplyFilter.SelectedIndexChanged += new System.EventHandler(this.comboBox_ApplyFilter_SelectedIndexChanged);
      // 
      // label_ApplyFilter
      // 
      this.label_ApplyFilter.Location = new System.Drawing.Point(6, 51);
      this.label_ApplyFilter.Name = "label_ApplyFilter";
      this.label_ApplyFilter.Size = new System.Drawing.Size(160, 20);
      this.label_ApplyFilter.TabIndex = 2;
      this.label_ApplyFilter.Text = "Apply Filter";
      this.label_ApplyFilter.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
      // 
      // comboBox_ImportHeightfield
      // 
      this.comboBox_ImportHeightfield.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.comboBox_ImportHeightfield.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.comboBox_ImportHeightfield.FormattingEnabled = true;
      this.comboBox_ImportHeightfield.Location = new System.Drawing.Point(178, 24);
      this.comboBox_ImportHeightfield.Name = "comboBox_ImportHeightfield";
      this.comboBox_ImportHeightfield.Size = new System.Drawing.Size(229, 21);
      this.comboBox_ImportHeightfield.TabIndex = 1;
      this.comboBox_ImportHeightfield.SelectedIndexChanged += new System.EventHandler(this.comboBox_ImportHeightfield_SelectedIndexChanged);
      // 
      // label_ImportHeightfield
      // 
      this.label_ImportHeightfield.Location = new System.Drawing.Point(6, 24);
      this.label_ImportHeightfield.Name = "label_ImportHeightfield";
      this.label_ImportHeightfield.Size = new System.Drawing.Size(160, 20);
      this.label_ImportHeightfield.TabIndex = 0;
      this.label_ImportHeightfield.Text = "Import Heightfield";
      this.label_ImportHeightfield.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
      // 
      // groupBox_Terrain
      // 
      this.groupBox_Terrain.Collapsable = false;
      this.groupBox_Terrain.ColorHeader = System.Drawing.SystemColors.Control;
      this.groupBox_Terrain.ColorHover = System.Drawing.SystemColors.ActiveCaption;
      this.groupBox_Terrain.Controls.Add(this.label_TerrainSettings);
      this.groupBox_Terrain.Controls.Add(this.button_ChangeConfig);
      this.groupBox_Terrain.Dock = System.Windows.Forms.DockStyle.Top;
      this.groupBox_Terrain.HeadOutlineColor = System.Drawing.SystemColors.ControlText;
      this.groupBox_Terrain.Location = new System.Drawing.Point(6, 12);
      this.groupBox_Terrain.Margin = new System.Windows.Forms.Padding(3, 3, 3, 9);
      this.groupBox_Terrain.Name = "groupBox_Terrain";
      this.groupBox_Terrain.Padding = new System.Windows.Forms.Padding(0);
      this.groupBox_Terrain.Size = new System.Drawing.Size(413, 52);
      this.groupBox_Terrain.SizeChangePropagationLevel = 0;
      this.groupBox_Terrain.TabIndex = 0;
      this.groupBox_Terrain.TabStop = false;
      this.groupBox_Terrain.Text = "Terrain Settings";
      this.groupBox_Terrain.Enter += new System.EventHandler(this.groupBox1_Enter);
      // 
      // label_TerrainSettings
      // 
      this.label_TerrainSettings.Location = new System.Drawing.Point(6, 24);
      this.label_TerrainSettings.Name = "label_TerrainSettings";
      this.label_TerrainSettings.Size = new System.Drawing.Size(160, 20);
      this.label_TerrainSettings.TabIndex = 4;
      this.label_TerrainSettings.Text = "Terrain Configuration";
      this.label_TerrainSettings.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
      // 
      // button_ChangeConfig
      // 
      this.button_ChangeConfig.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.button_ChangeConfig.ImageAlign = System.Drawing.ContentAlignment.MiddleRight;
      this.button_ChangeConfig.Location = new System.Drawing.Point(178, 23);
      this.button_ChangeConfig.Name = "button_ChangeConfig";
      this.button_ChangeConfig.Size = new System.Drawing.Size(229, 22);
      this.button_ChangeConfig.TabIndex = 0;
      this.button_ChangeConfig.Text = "Change Terrain Layout";
      this.button_ChangeConfig.UseVisualStyleBackColor = true;
      this.button_ChangeConfig.Click += new System.EventHandler(this.button_ChangeConfig_Click);
      // 
      // splitter1
      // 
      this.splitter1.Dock = System.Windows.Forms.DockStyle.Top;
      this.splitter1.Location = new System.Drawing.Point(6, 6);
      this.splitter1.Name = "splitter1";
      this.splitter1.Size = new System.Drawing.Size(413, 6);
      this.splitter1.TabIndex = 4;
      this.splitter1.TabStop = false;
      // 
      // tabPage_Texturing
      // 
      this.tabPage_Texturing.Controls.Add(this.splitContainer3);
      this.tabPage_Texturing.Controls.Add(this.toolStrip_Texturing);
      this.tabPage_Texturing.Location = new System.Drawing.Point(4, 22);
      this.tabPage_Texturing.Name = "tabPage_Texturing";
      this.tabPage_Texturing.Size = new System.Drawing.Size(425, 380);
      this.tabPage_Texturing.TabIndex = 1;
      this.tabPage_Texturing.Text = "Texturing";
      // 
      // splitContainer3
      // 
      this.splitContainer3.Dock = System.Windows.Forms.DockStyle.Fill;
      this.splitContainer3.Location = new System.Drawing.Point(0, 25);
      this.splitContainer3.Name = "splitContainer3";
      this.splitContainer3.Orientation = System.Windows.Forms.Orientation.Horizontal;
      // 
      // splitContainer3.Panel1
      // 
      this.splitContainer3.Panel1.Controls.Add(this.listView_DetailTextures);
      // 
      // splitContainer3.Panel2
      // 
      this.splitContainer3.Panel2.Controls.Add(this.propertyGrid_DetailTexture);
      this.splitContainer3.Size = new System.Drawing.Size(425, 355);
      this.splitContainer3.SplitterDistance = 140;
      this.splitContainer3.TabIndex = 4;
      // 
      // listView_DetailTextures
      // 
      this.listView_DetailTextures.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader_Name,
            this.columnHeader_File});
      this.listView_DetailTextures.ContextMenuStrip = this.contextMenuStrip_DetailTexture;
      this.listView_DetailTextures.Dock = System.Windows.Forms.DockStyle.Fill;
      this.listView_DetailTextures.HideSelection = false;
      this.listView_DetailTextures.Location = new System.Drawing.Point(0, 0);
      this.listView_DetailTextures.Name = "listView_DetailTextures";
      this.listView_DetailTextures.Size = new System.Drawing.Size(425, 140);
      this.listView_DetailTextures.TabIndex = 0;
      this.listView_DetailTextures.UseCompatibleStateImageBehavior = false;
      this.listView_DetailTextures.View = System.Windows.Forms.View.Details;
      this.listView_DetailTextures.SelectedIndexChanged += new System.EventHandler(this.listView_DetailTextures_SelectedIndexChanged);
      this.listView_DetailTextures.DoubleClick += new System.EventHandler(this.listView_DetailTextures_DoubleClick);
      // 
      // columnHeader_Name
      // 
      this.columnHeader_Name.Text = "Name";
      this.columnHeader_Name.Width = 106;
      // 
      // columnHeader_File
      // 
      this.columnHeader_File.Text = "Diffuse Texture";
      this.columnHeader_File.Width = 175;
      // 
      // contextMenuStrip_DetailTexture
      // 
      this.contextMenuStrip_DetailTexture.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuItem_ExportTextureLuminance});
      this.contextMenuStrip_DetailTexture.Name = "contextMenuStrip_DetailTexture";
      this.contextMenuStrip_DetailTexture.Size = new System.Drawing.Size(206, 26);
      this.contextMenuStrip_DetailTexture.Opening += new System.ComponentModel.CancelEventHandler(this.contextMenuStrip_DetailTexture_Opening);
      // 
      // ToolStripMenuItem_ExportTextureLuminance
      // 
      this.ToolStripMenuItem_ExportTextureLuminance.Name = "ToolStripMenuItem_ExportTextureLuminance";
      this.ToolStripMenuItem_ExportTextureLuminance.Size = new System.Drawing.Size(205, 22);
      this.ToolStripMenuItem_ExportTextureLuminance.Text = "Export luminance texture";
      this.ToolStripMenuItem_ExportTextureLuminance.Click += new System.EventHandler(this.ToolStripMenuItem_ExportTextureLuminance_Click);
      // 
      // propertyGrid_DetailTexture
      // 
      this.propertyGrid_DetailTexture.Dock = System.Windows.Forms.DockStyle.Fill;
      this.propertyGrid_DetailTexture.HelpBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(252)))), ((int)(((byte)(252)))), ((int)(((byte)(252)))));
      this.propertyGrid_DetailTexture.HelpBackgroundImage = ((System.Drawing.Image)(resources.GetObject("propertyGrid_DetailTexture.HelpBackgroundImage")));
      this.propertyGrid_DetailTexture.LineColor = System.Drawing.Color.FromArgb(((int)(((byte)(248)))), ((int)(((byte)(248)))), ((int)(((byte)(248)))));
      this.propertyGrid_DetailTexture.Location = new System.Drawing.Point(0, 0);
      this.propertyGrid_DetailTexture.Name = "propertyGrid_DetailTexture";
      this.propertyGrid_DetailTexture.Size = new System.Drawing.Size(425, 211);
      this.propertyGrid_DetailTexture.TabIndex = 0;
      this.propertyGrid_DetailTexture.ToolbarVisible = false;
      // 
      // 
      // 
      this.propertyGrid_DetailTexture.ToolStrip.AccessibleName = "ToolBar";
      this.propertyGrid_DetailTexture.ToolStrip.AccessibleRole = System.Windows.Forms.AccessibleRole.ToolBar;
      this.propertyGrid_DetailTexture.ToolStrip.AllowMerge = false;
      this.propertyGrid_DetailTexture.ToolStrip.AutoSize = false;
      this.propertyGrid_DetailTexture.ToolStrip.CanOverflow = false;
      this.propertyGrid_DetailTexture.ToolStrip.Dock = System.Windows.Forms.DockStyle.None;
      this.propertyGrid_DetailTexture.ToolStrip.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
      this.propertyGrid_DetailTexture.ToolStrip.Location = new System.Drawing.Point(0, 0);
      this.propertyGrid_DetailTexture.ToolStrip.Name = "";
      this.propertyGrid_DetailTexture.ToolStrip.Padding = new System.Windows.Forms.Padding(2, 0, 1, 0);
      this.propertyGrid_DetailTexture.ToolStrip.TabIndex = 1;
      this.propertyGrid_DetailTexture.ToolStrip.TabStop = true;
      this.propertyGrid_DetailTexture.ToolStrip.Text = "PropertyGridToolBar";
      this.propertyGrid_DetailTexture.ToolStrip.Visible = false;
      // 
      // toolStrip_Texturing
      // 
      this.toolStrip_Texturing.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
      this.toolStrip_Texturing.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton_AddDetailTexture,
            this.toolStripButton_DeleteDetailTexture,
            this.toolStripSeparator5,
            this.toolStripButton_MoveUp,
            this.toolStripButton_MoveDown,
            this.toolStripSeparator6,
            this.toolStripButton_Blink,
            this.toolStripButton_MaterialID,
            this.toolStripSeparator2,
            this.toolStripButton_ImportWeightmap,
            this.toolStripButton_SlopeTextures,
            this.toolStripButton_SplitPerSectorTex});
      this.toolStrip_Texturing.Location = new System.Drawing.Point(0, 0);
      this.toolStrip_Texturing.Name = "toolStrip_Texturing";
      this.toolStrip_Texturing.Size = new System.Drawing.Size(425, 25);
      this.toolStrip_Texturing.TabIndex = 0;
      // 
      // toolStripButton_AddDetailTexture
      // 
      this.toolStripButton_AddDetailTexture.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_AddDetailTexture.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_AddDetailTexture.Image")));
      this.toolStripButton_AddDetailTexture.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_AddDetailTexture.Name = "toolStripButton_AddDetailTexture";
      this.toolStripButton_AddDetailTexture.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_AddDetailTexture.Text = "Add detail texture";
      this.toolStripButton_AddDetailTexture.Click += new System.EventHandler(this.toolStripButton_AddDetailTexture_Click);
      // 
      // toolStripButton_DeleteDetailTexture
      // 
      this.toolStripButton_DeleteDetailTexture.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_DeleteDetailTexture.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_DeleteDetailTexture.Image")));
      this.toolStripButton_DeleteDetailTexture.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_DeleteDetailTexture.Name = "toolStripButton_DeleteDetailTexture";
      this.toolStripButton_DeleteDetailTexture.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_DeleteDetailTexture.Text = "Delete detail texture";
      this.toolStripButton_DeleteDetailTexture.Click += new System.EventHandler(this.toolStripButton_DeleteDetailTexture_Click);
      // 
      // toolStripSeparator5
      // 
      this.toolStripSeparator5.Name = "toolStripSeparator5";
      this.toolStripSeparator5.Size = new System.Drawing.Size(6, 25);
      // 
      // toolStripButton_MoveUp
      // 
      this.toolStripButton_MoveUp.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_MoveUp.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_MoveUp.Image")));
      this.toolStripButton_MoveUp.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_MoveUp.Name = "toolStripButton_MoveUp";
      this.toolStripButton_MoveUp.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_MoveUp.Text = "toolStripButton1";
      this.toolStripButton_MoveUp.ToolTipText = "Move this channel up in the list (affects texturing)";
      this.toolStripButton_MoveUp.Click += new System.EventHandler(this.toolStripButton_MoveUp_Click);
      // 
      // toolStripButton_MoveDown
      // 
      this.toolStripButton_MoveDown.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_MoveDown.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_MoveDown.Image")));
      this.toolStripButton_MoveDown.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_MoveDown.Name = "toolStripButton_MoveDown";
      this.toolStripButton_MoveDown.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_MoveDown.Text = "toolStripButton1";
      this.toolStripButton_MoveDown.ToolTipText = "Move this channel down in the list (affects texturing)";
      this.toolStripButton_MoveDown.Click += new System.EventHandler(this.toolStripButton_MoveDown_Click);
      // 
      // toolStripSeparator6
      // 
      this.toolStripSeparator6.Name = "toolStripSeparator6";
      this.toolStripSeparator6.Size = new System.Drawing.Size(6, 25);
      // 
      // toolStripButton_Blink
      // 
      this.toolStripButton_Blink.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_Blink.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_Blink.Image")));
      this.toolStripButton_Blink.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_Blink.Name = "toolStripButton_Blink";
      this.toolStripButton_Blink.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_Blink.Text = "Blink";
      this.toolStripButton_Blink.ToolTipText = "Highlight the coverage of this channel in the view";
      this.toolStripButton_Blink.Click += new System.EventHandler(this.toolStripButton_Blink_Click);
      // 
      // toolStripButton_MaterialID
      // 
      this.toolStripButton_MaterialID.CheckOnClick = true;
      this.toolStripButton_MaterialID.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_MaterialID.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_MaterialID.Image")));
      this.toolStripButton_MaterialID.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_MaterialID.Name = "toolStripButton_MaterialID";
      this.toolStripButton_MaterialID.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_MaterialID.Text = "MaterialID";
      this.toolStripButton_MaterialID.ToolTipText = "If enabled, the selected texture shows its coverage in the material ID map";
      this.toolStripButton_MaterialID.Click += new System.EventHandler(this.toolStripButton_MaterialID_Click);
      // 
      // toolStripSeparator2
      // 
      this.toolStripSeparator2.Name = "toolStripSeparator2";
      this.toolStripSeparator2.Size = new System.Drawing.Size(6, 25);
      // 
      // toolStripButton_ImportWeightmap
      // 
      this.toolStripButton_ImportWeightmap.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_ImportWeightmap.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_ImportWeightmap.Image")));
      this.toolStripButton_ImportWeightmap.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_ImportWeightmap.Name = "toolStripButton_ImportWeightmap";
      this.toolStripButton_ImportWeightmap.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_ImportWeightmap.Text = "Import weightmap";
      this.toolStripButton_ImportWeightmap.ToolTipText = "Import weightmap from file";
      this.toolStripButton_ImportWeightmap.Click += new System.EventHandler(this.toolStripButton_ImportWeightmap_Click);
      // 
      // toolStripButton_SlopeTextures
      // 
      this.toolStripButton_SlopeTextures.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_SlopeTextures.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_SlopeTextures.Image")));
      this.toolStripButton_SlopeTextures.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_SlopeTextures.Name = "toolStripButton_SlopeTextures";
      this.toolStripButton_SlopeTextures.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_SlopeTextures.Text = "Create detail textures from slope lookup";
      this.toolStripButton_SlopeTextures.Click += new System.EventHandler(this.toolStripButton_SlopeTextures_Click);
      // 
      // toolStripButton_SplitPerSectorTex
      // 
      this.toolStripButton_SplitPerSectorTex.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_SplitPerSectorTex.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_SplitPerSectorTex.Image")));
      this.toolStripButton_SplitPerSectorTex.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_SplitPerSectorTex.Name = "toolStripButton_SplitPerSectorTex";
      this.toolStripButton_SplitPerSectorTex.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_SplitPerSectorTex.Text = "Import a large texture and split it into per-sector textures";
      this.toolStripButton_SplitPerSectorTex.Click += new System.EventHandler(this.toolStripButton_SplitPerSectorTex_Click);
      // 
      // tabPage_Decoration
      // 
      this.tabPage_Decoration.Controls.Add(this.splitContainer2);
      this.tabPage_Decoration.Controls.Add(this.toolStrip_Vegetation);
      this.tabPage_Decoration.Location = new System.Drawing.Point(4, 22);
      this.tabPage_Decoration.Name = "tabPage_Decoration";
      this.tabPage_Decoration.Size = new System.Drawing.Size(425, 380);
      this.tabPage_Decoration.TabIndex = 2;
      this.tabPage_Decoration.Text = "Vegetation";
      this.tabPage_Decoration.UseVisualStyleBackColor = true;
      this.tabPage_Decoration.Click += new System.EventHandler(this.tabPage_Decoration_Click);
      // 
      // splitContainer2
      // 
      this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
      this.splitContainer2.Location = new System.Drawing.Point(0, 25);
      this.splitContainer2.Name = "splitContainer2";
      this.splitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal;
      // 
      // splitContainer2.Panel1
      // 
      this.splitContainer2.Panel1.Controls.Add(this.listView_VegetationModels);
      // 
      // splitContainer2.Panel2
      // 
      this.splitContainer2.Panel2.Controls.Add(this.propertyGrid_VegetationModel);
      this.splitContainer2.Size = new System.Drawing.Size(425, 355);
      this.splitContainer2.SplitterDistance = 115;
      this.splitContainer2.TabIndex = 3;
      // 
      // listView_VegetationModels
      // 
      this.listView_VegetationModels.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2});
      this.listView_VegetationModels.ContextMenuStrip = this.contextMenuStrip_Decoration;
      this.listView_VegetationModels.Dock = System.Windows.Forms.DockStyle.Fill;
      this.listView_VegetationModels.HideSelection = false;
      this.listView_VegetationModels.Location = new System.Drawing.Point(0, 0);
      this.listView_VegetationModels.Name = "listView_VegetationModels";
      this.listView_VegetationModels.Size = new System.Drawing.Size(425, 115);
      this.listView_VegetationModels.TabIndex = 0;
      this.listView_VegetationModels.UseCompatibleStateImageBehavior = false;
      this.listView_VegetationModels.View = System.Windows.Forms.View.Details;
      this.listView_VegetationModels.SelectedIndexChanged += new System.EventHandler(this.listView_VegetationModels_SelectedIndexChanged);
      this.listView_VegetationModels.DoubleClick += new System.EventHandler(this.listView_VegetationModels_DoubleClick);
      // 
      // columnHeader1
      // 
      this.columnHeader1.Text = "Name";
      this.columnHeader1.Width = 98;
      // 
      // columnHeader2
      // 
      this.columnHeader2.Text = "Model file";
      this.columnHeader2.Width = 176;
      // 
      // contextMenuStrip_Decoration
      // 
      this.contextMenuStrip_Decoration.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripMenuItem_DecorationFilters,
            this.toolStripMenuItem1,
            this.ToolStripMenuItem_ExportDensity,
            this.createLODVersionToolStripMenuItem});
      this.contextMenuStrip_Decoration.Name = "contextMenuStrip_Decoration";
      this.contextMenuStrip_Decoration.Size = new System.Drawing.Size(206, 76);
      this.contextMenuStrip_Decoration.Opening += new System.ComponentModel.CancelEventHandler(this.contextMenuStrip_Decoration_Opening);
      // 
      // toolStripMenuItem_DecorationFilters
      // 
      this.toolStripMenuItem_DecorationFilters.Name = "toolStripMenuItem_DecorationFilters";
      this.toolStripMenuItem_DecorationFilters.Size = new System.Drawing.Size(205, 22);
      this.toolStripMenuItem_DecorationFilters.Text = "Apply Filter";
      // 
      // toolStripMenuItem1
      // 
      this.toolStripMenuItem1.Name = "toolStripMenuItem1";
      this.toolStripMenuItem1.Size = new System.Drawing.Size(202, 6);
      // 
      // ToolStripMenuItem_ExportDensity
      // 
      this.ToolStripMenuItem_ExportDensity.Name = "ToolStripMenuItem_ExportDensity";
      this.ToolStripMenuItem_ExportDensity.Size = new System.Drawing.Size(205, 22);
      this.ToolStripMenuItem_ExportDensity.Text = "Export luminance texture";
      this.ToolStripMenuItem_ExportDensity.Click += new System.EventHandler(this.ToolStripMenuItem_ExportDensity_Click);
      // 
      // createLODVersionToolStripMenuItem
      // 
      this.createLODVersionToolStripMenuItem.Name = "createLODVersionToolStripMenuItem";
      this.createLODVersionToolStripMenuItem.Size = new System.Drawing.Size(205, 22);
      this.createLODVersionToolStripMenuItem.Text = "Create LOD Version";
      this.createLODVersionToolStripMenuItem.Click += new System.EventHandler(this.createLODVersionToolStripMenuItem_Click);
      // 
      // propertyGrid_VegetationModel
      // 
      this.propertyGrid_VegetationModel.Dock = System.Windows.Forms.DockStyle.Fill;
      this.propertyGrid_VegetationModel.HelpBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(252)))), ((int)(((byte)(252)))), ((int)(((byte)(252)))));
      this.propertyGrid_VegetationModel.HelpBackgroundImage = ((System.Drawing.Image)(resources.GetObject("propertyGrid_VegetationModel.HelpBackgroundImage")));
      this.propertyGrid_VegetationModel.LineColor = System.Drawing.Color.FromArgb(((int)(((byte)(248)))), ((int)(((byte)(248)))), ((int)(((byte)(248)))));
      this.propertyGrid_VegetationModel.Location = new System.Drawing.Point(0, 0);
      this.propertyGrid_VegetationModel.Name = "propertyGrid_VegetationModel";
      this.propertyGrid_VegetationModel.PropertySort = System.Windows.Forms.PropertySort.Categorized;
      this.propertyGrid_VegetationModel.Size = new System.Drawing.Size(425, 236);
      this.propertyGrid_VegetationModel.TabIndex = 0;
      this.propertyGrid_VegetationModel.ToolbarVisible = false;
      // 
      // 
      // 
      this.propertyGrid_VegetationModel.ToolStrip.AccessibleName = "ToolBar";
      this.propertyGrid_VegetationModel.ToolStrip.AccessibleRole = System.Windows.Forms.AccessibleRole.ToolBar;
      this.propertyGrid_VegetationModel.ToolStrip.AllowMerge = false;
      this.propertyGrid_VegetationModel.ToolStrip.AutoSize = false;
      this.propertyGrid_VegetationModel.ToolStrip.CanOverflow = false;
      this.propertyGrid_VegetationModel.ToolStrip.Dock = System.Windows.Forms.DockStyle.None;
      this.propertyGrid_VegetationModel.ToolStrip.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
      this.propertyGrid_VegetationModel.ToolStrip.Location = new System.Drawing.Point(0, 0);
      this.propertyGrid_VegetationModel.ToolStrip.Name = "";
      this.propertyGrid_VegetationModel.ToolStrip.Padding = new System.Windows.Forms.Padding(2, 0, 1, 0);
      this.propertyGrid_VegetationModel.ToolStrip.TabIndex = 1;
      this.propertyGrid_VegetationModel.ToolStrip.TabStop = true;
      this.propertyGrid_VegetationModel.ToolStrip.Text = "PropertyGridToolBar";
      this.propertyGrid_VegetationModel.ToolStrip.Visible = false;
      // 
      // toolStrip_Vegetation
      // 
      this.toolStrip_Vegetation.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
      this.toolStrip_Vegetation.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton_AddDecoChannel,
            this.toolStripButton_DeleteDecoChannel,
            this.toolStripSeparator3,
            this.toolStripSplitButton_Atlas,
            this.toolStripSeparator7,
            this.toolStripButton_RebuildDirtyRange,
            this.toolStripButton_RebuildVegetation,
            this.toolStripSeparator8,
            this.toolStripButton_AddMeshComponent,
            this.toolStripButton_RemoveMeshComponent});
      this.toolStrip_Vegetation.Location = new System.Drawing.Point(0, 0);
      this.toolStrip_Vegetation.Name = "toolStrip_Vegetation";
      this.toolStrip_Vegetation.Size = new System.Drawing.Size(425, 25);
      this.toolStrip_Vegetation.TabIndex = 0;
      this.toolStrip_Vegetation.Text = "Billboard Vegetation";
      this.toolStrip_Vegetation.ItemClicked += new System.Windows.Forms.ToolStripItemClickedEventHandler(this.toolStrip1_ItemClicked);
      // 
      // toolStripButton_AddDecoChannel
      // 
      this.toolStripButton_AddDecoChannel.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_AddDecoChannel.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_AddDecoChannel.Image")));
      this.toolStripButton_AddDecoChannel.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_AddDecoChannel.Name = "toolStripButton_AddDecoChannel";
      this.toolStripButton_AddDecoChannel.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_AddDecoChannel.Text = "Add";
      this.toolStripButton_AddDecoChannel.Click += new System.EventHandler(this.toolStripButton_AddDecoChannel_Click);
      // 
      // toolStripButton_DeleteDecoChannel
      // 
      this.toolStripButton_DeleteDecoChannel.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_DeleteDecoChannel.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_DeleteDecoChannel.Image")));
      this.toolStripButton_DeleteDecoChannel.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_DeleteDecoChannel.Name = "toolStripButton_DeleteDecoChannel";
      this.toolStripButton_DeleteDecoChannel.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_DeleteDecoChannel.Text = "toolStripButton1";
      this.toolStripButton_DeleteDecoChannel.ToolTipText = "Delete";
      this.toolStripButton_DeleteDecoChannel.Click += new System.EventHandler(this.toolStripButton_DeleteDecoChannel_Click);
      // 
      // toolStripSeparator3
      // 
      this.toolStripSeparator3.Name = "toolStripSeparator3";
      this.toolStripSeparator3.Size = new System.Drawing.Size(6, 25);
      // 
      // toolStripSplitButton_Atlas
      // 
      this.toolStripSplitButton_Atlas.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripSplitButton_Atlas.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.createNewBillboardAtlasToolStripMenuItem,
            this.editExistingBillboardAtlasToolStripMenuItem});
      this.toolStripSplitButton_Atlas.Image = ((System.Drawing.Image)(resources.GetObject("toolStripSplitButton_Atlas.Image")));
      this.toolStripSplitButton_Atlas.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripSplitButton_Atlas.Name = "toolStripSplitButton_Atlas";
      this.toolStripSplitButton_Atlas.Size = new System.Drawing.Size(32, 22);
      this.toolStripSplitButton_Atlas.Text = "Options for creating and editing billboard atlas files";
      // 
      // createNewBillboardAtlasToolStripMenuItem
      // 
      this.createNewBillboardAtlasToolStripMenuItem.Image = ((System.Drawing.Image)(resources.GetObject("createNewBillboardAtlasToolStripMenuItem.Image")));
      this.createNewBillboardAtlasToolStripMenuItem.Name = "createNewBillboardAtlasToolStripMenuItem";
      this.createNewBillboardAtlasToolStripMenuItem.Size = new System.Drawing.Size(216, 22);
      this.createNewBillboardAtlasToolStripMenuItem.Text = "Create new billboard Atlas";
      this.createNewBillboardAtlasToolStripMenuItem.ToolTipText = "For billboard vegetation, this option creates a new atlas file";
      this.createNewBillboardAtlasToolStripMenuItem.Click += new System.EventHandler(this.createNewBillboardAtlasToolStripMenuItem_Click);
      // 
      // editExistingBillboardAtlasToolStripMenuItem
      // 
      this.editExistingBillboardAtlasToolStripMenuItem.Image = ((System.Drawing.Image)(resources.GetObject("editExistingBillboardAtlasToolStripMenuItem.Image")));
      this.editExistingBillboardAtlasToolStripMenuItem.Name = "editExistingBillboardAtlasToolStripMenuItem";
      this.editExistingBillboardAtlasToolStripMenuItem.Size = new System.Drawing.Size(216, 22);
      this.editExistingBillboardAtlasToolStripMenuItem.Text = "Edit existing billboard Atlas";
      this.editExistingBillboardAtlasToolStripMenuItem.ToolTipText = "For billboard vegetation, this option allows for editing an existing atlas file";
      this.editExistingBillboardAtlasToolStripMenuItem.Click += new System.EventHandler(this.editExistingBillboardAtlasToolStripMenuItem_Click);
      // 
      // toolStripSeparator7
      // 
      this.toolStripSeparator7.Name = "toolStripSeparator7";
      this.toolStripSeparator7.Size = new System.Drawing.Size(6, 25);
      // 
      // toolStripButton_RebuildDirtyRange
      // 
      this.toolStripButton_RebuildDirtyRange.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_RebuildDirtyRange.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_RebuildDirtyRange.Image")));
      this.toolStripButton_RebuildDirtyRange.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_RebuildDirtyRange.Name = "toolStripButton_RebuildDirtyRange";
      this.toolStripButton_RebuildDirtyRange.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_RebuildDirtyRange.Text = "Rebuild vegetation for modified sectors";
      this.toolStripButton_RebuildDirtyRange.Click += new System.EventHandler(this.toolStripButton_RebuildDirtyRange_Click);
      // 
      // toolStripButton_RebuildVegetation
      // 
      this.toolStripButton_RebuildVegetation.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_RebuildVegetation.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_RebuildVegetation.Image")));
      this.toolStripButton_RebuildVegetation.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_RebuildVegetation.Name = "toolStripButton_RebuildVegetation";
      this.toolStripButton_RebuildVegetation.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_RebuildVegetation.Text = "Rebuild vegetation on all sectors in case the list got out of sync";
      this.toolStripButton_RebuildVegetation.Click += new System.EventHandler(this.toolStripButton_RebuildVegetation_Click);
      // 
      // toolStripSeparator8
      // 
      this.toolStripSeparator8.Name = "toolStripSeparator8";
      this.toolStripSeparator8.Size = new System.Drawing.Size(6, 25);
      // 
      // toolStripButton_AddMeshComponent
      // 
      this.toolStripButton_AddMeshComponent.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_AddMeshComponent.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_AddMeshComponent.Image")));
      this.toolStripButton_AddMeshComponent.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_AddMeshComponent.Name = "toolStripButton_AddMeshComponent";
      this.toolStripButton_AddMeshComponent.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_AddMeshComponent.Text = "Enable vegetation on mesh";
      this.toolStripButton_AddMeshComponent.ToolTipText = "Picker for static mesh shapes to enable vegetation painting on the mesh";
      this.toolStripButton_AddMeshComponent.Click += new System.EventHandler(this.toolStripButton_AddMeshComponent_Click);
      // 
      // toolStripButton_RemoveMeshComponent
      // 
      this.toolStripButton_RemoveMeshComponent.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_RemoveMeshComponent.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_RemoveMeshComponent.Image")));
      this.toolStripButton_RemoveMeshComponent.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_RemoveMeshComponent.Name = "toolStripButton_RemoveMeshComponent";
      this.toolStripButton_RemoveMeshComponent.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_RemoveMeshComponent.Text = "Disable vegetation for mesh";
      this.toolStripButton_RemoveMeshComponent.ToolTipText = "Picker for static mesh shapes to disable vegetation painting on the mesh";
      this.toolStripButton_RemoveMeshComponent.Click += new System.EventHandler(this.toolStripButton_RemoveMeshComponent_Click);
      // 
      // tabPage_Shader
      // 
      this.tabPage_Shader.Controls.Add(this.propertyGrid_Shader);
      this.tabPage_Shader.Location = new System.Drawing.Point(4, 22);
      this.tabPage_Shader.Name = "tabPage_Shader";
      this.tabPage_Shader.Size = new System.Drawing.Size(425, 380);
      this.tabPage_Shader.TabIndex = 3;
      this.tabPage_Shader.Text = "Shader";
      this.tabPage_Shader.UseVisualStyleBackColor = true;
      // 
      // propertyGrid_Shader
      // 
      this.propertyGrid_Shader.Dock = System.Windows.Forms.DockStyle.Fill;
      this.propertyGrid_Shader.HelpBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(252)))), ((int)(((byte)(252)))), ((int)(((byte)(252)))));
      this.propertyGrid_Shader.HelpBackgroundImage = ((System.Drawing.Image)(resources.GetObject("propertyGrid_Shader.HelpBackgroundImage")));
      this.propertyGrid_Shader.LineColor = System.Drawing.Color.FromArgb(((int)(((byte)(248)))), ((int)(((byte)(248)))), ((int)(((byte)(248)))));
      this.propertyGrid_Shader.Location = new System.Drawing.Point(0, 0);
      this.propertyGrid_Shader.Name = "propertyGrid_Shader";
      this.propertyGrid_Shader.Size = new System.Drawing.Size(425, 380);
      this.propertyGrid_Shader.TabIndex = 0;
      this.propertyGrid_Shader.ToolbarVisible = false;
      // 
      // 
      // 
      this.propertyGrid_Shader.ToolStrip.AccessibleName = "ToolBar";
      this.propertyGrid_Shader.ToolStrip.AccessibleRole = System.Windows.Forms.AccessibleRole.ToolBar;
      this.propertyGrid_Shader.ToolStrip.AllowMerge = false;
      this.propertyGrid_Shader.ToolStrip.AutoSize = false;
      this.propertyGrid_Shader.ToolStrip.CanOverflow = false;
      this.propertyGrid_Shader.ToolStrip.Dock = System.Windows.Forms.DockStyle.None;
      this.propertyGrid_Shader.ToolStrip.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
      this.propertyGrid_Shader.ToolStrip.Location = new System.Drawing.Point(0, 1);
      this.propertyGrid_Shader.ToolStrip.Name = "";
      this.propertyGrid_Shader.ToolStrip.Padding = new System.Windows.Forms.Padding(2, 0, 1, 0);
      this.propertyGrid_Shader.ToolStrip.Size = new System.Drawing.Size(425, 25);
      this.propertyGrid_Shader.ToolStrip.TabIndex = 1;
      this.propertyGrid_Shader.ToolStrip.TabStop = true;
      this.propertyGrid_Shader.ToolStrip.Text = "PropertyGridToolBar";
      this.propertyGrid_Shader.ToolStrip.Visible = false;
      // 
      // tabPage_MeshExport
      // 
      this.tabPage_MeshExport.Controls.Add(this.propertyGrid_MeshExport);
      this.tabPage_MeshExport.Controls.Add(this.splitter5);
      this.tabPage_MeshExport.Controls.Add(this.groupBox_MeshExport);
      this.tabPage_MeshExport.Controls.Add(this.splitter4);
      this.tabPage_MeshExport.Location = new System.Drawing.Point(4, 22);
      this.tabPage_MeshExport.Name = "tabPage_MeshExport";
      this.tabPage_MeshExport.Padding = new System.Windows.Forms.Padding(6);
      this.tabPage_MeshExport.Size = new System.Drawing.Size(425, 380);
      this.tabPage_MeshExport.TabIndex = 4;
      this.tabPage_MeshExport.Text = "Mesh Export";
      this.tabPage_MeshExport.UseVisualStyleBackColor = true;
      // 
      // propertyGrid_MeshExport
      // 
      this.propertyGrid_MeshExport.Dock = System.Windows.Forms.DockStyle.Fill;
      this.propertyGrid_MeshExport.Location = new System.Drawing.Point(6, 103);
      this.propertyGrid_MeshExport.Name = "propertyGrid_MeshExport";
      this.propertyGrid_MeshExport.Size = new System.Drawing.Size(413, 271);
      this.propertyGrid_MeshExport.TabIndex = 8;
      this.propertyGrid_MeshExport.ToolbarVisible = false;
      this.propertyGrid_MeshExport.PropertyValueChanged += new System.Windows.Forms.PropertyValueChangedEventHandler(this.propertyGrid_MeshExport_PropertyValueChanged);
      // 
      // splitter5
      // 
      this.splitter5.Dock = System.Windows.Forms.DockStyle.Top;
      this.splitter5.Location = new System.Drawing.Point(6, 97);
      this.splitter5.Name = "splitter5";
      this.splitter5.Size = new System.Drawing.Size(413, 6);
      this.splitter5.TabIndex = 7;
      this.splitter5.TabStop = false;
      // 
      // groupBox_MeshExport
      // 
      this.groupBox_MeshExport.Collapsable = false;
      this.groupBox_MeshExport.ColorHeader = System.Drawing.SystemColors.Control;
      this.groupBox_MeshExport.ColorHover = System.Drawing.SystemColors.ActiveCaption;
      this.groupBox_MeshExport.Controls.Add(this.label_ProcessExport);
      this.groupBox_MeshExport.Controls.Add(this.button_ExportPrefab);
      this.groupBox_MeshExport.Controls.Add(this.label_Export);
      this.groupBox_MeshExport.Dock = System.Windows.Forms.DockStyle.Top;
      this.groupBox_MeshExport.HeadOutlineColor = System.Drawing.SystemColors.ControlText;
      this.groupBox_MeshExport.Location = new System.Drawing.Point(6, 12);
      this.groupBox_MeshExport.Name = "groupBox_MeshExport";
      this.groupBox_MeshExport.Padding = new System.Windows.Forms.Padding(3, 8, 3, 3);
      this.groupBox_MeshExport.Size = new System.Drawing.Size(413, 85);
      this.groupBox_MeshExport.SizeChangePropagationLevel = 0;
      this.groupBox_MeshExport.TabIndex = 6;
      this.groupBox_MeshExport.TabStop = false;
      this.groupBox_MeshExport.Text = "Export Terrain Mesh";
      // 
      // label_ProcessExport
      // 
      this.label_ProcessExport.Location = new System.Drawing.Point(6, 58);
      this.label_ProcessExport.Name = "label_ProcessExport";
      this.label_ProcessExport.Size = new System.Drawing.Size(160, 20);
      this.label_ProcessExport.TabIndex = 4;
      this.label_ProcessExport.Text = "Process Export";
      this.label_ProcessExport.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
      // 
      // button_ExportPrefab
      // 
      this.button_ExportPrefab.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.button_ExportPrefab.Location = new System.Drawing.Point(178, 57);
      this.button_ExportPrefab.Name = "button_ExportPrefab";
      this.button_ExportPrefab.Size = new System.Drawing.Size(229, 22);
      this.button_ExportPrefab.TabIndex = 2;
      this.button_ExportPrefab.Text = "Export Prefab With Current Settings";
      this.button_ExportPrefab.UseVisualStyleBackColor = true;
      this.button_ExportPrefab.Click += new System.EventHandler(this.button_ExportPrefab_Click);
      // 
      // label_Export
      // 
      this.label_Export.Location = new System.Drawing.Point(6, 24);
      this.label_Export.Name = "label_Export";
      this.label_Export.Size = new System.Drawing.Size(401, 29);
      this.label_Export.TabIndex = 0;
      this.label_Export.Text = "This option allows for exporting the terrain mesh to an external file. This is op" +
    "tional and not required for displaying the terrain in the engine.";
      // 
      // splitter4
      // 
      this.splitter4.Dock = System.Windows.Forms.DockStyle.Top;
      this.splitter4.Location = new System.Drawing.Point(6, 6);
      this.splitter4.Name = "splitter4";
      this.splitter4.Size = new System.Drawing.Size(413, 6);
      this.splitter4.TabIndex = 5;
      this.splitter4.TabStop = false;
      // 
      // toolStrip_Main
      // 
      this.toolStrip_Main.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
      this.toolStrip_Main.ImageScalingSize = new System.Drawing.Size(24, 24);
      this.toolStrip_Main.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton_Elevate,
            this.toolStripButton_Flatten,
            this.toolStripButton_Smooth,
            this.toolStripButton_PickHeight,
            this.toolStripButton_EditHoles,
            this.toolStripButton_PaintDetailTexture,
            this.toolStripSeparator4,
            this.toolStripButton_TextureBaked,
            this.toolStripButton_DirectionalLighting,
            this.toolStripButton_FullRes,
            this.toolStripButton_DisplayVegetationDensity,
            this.toolStripButton_DisplayMiniMap,
            this.toolStripSeparator1,
            this.toolStripButton_Selection,
            this.toolStripButton_PurgeResources});
      this.toolStrip_Main.Location = new System.Drawing.Point(0, 50);
      this.toolStrip_Main.Name = "toolStrip_Main";
      this.toolStrip_Main.Size = new System.Drawing.Size(433, 31);
      this.toolStrip_Main.TabIndex = 0;
      this.toolStrip_Main.Text = "Main toolbar";
      // 
      // toolStripButton_Elevate
      // 
      this.toolStripButton_Elevate.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_Elevate.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_Elevate.Image")));
      this.toolStripButton_Elevate.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_Elevate.Name = "toolStripButton_Elevate";
      this.toolStripButton_Elevate.Size = new System.Drawing.Size(28, 28);
      this.toolStripButton_Elevate.Text = "Elevate Terrain";
      this.toolStripButton_Elevate.Click += new System.EventHandler(this.toolStripButton_Elevate_Click);
      // 
      // toolStripButton_Flatten
      // 
      this.toolStripButton_Flatten.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_Flatten.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_Flatten.Image")));
      this.toolStripButton_Flatten.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_Flatten.Name = "toolStripButton_Flatten";
      this.toolStripButton_Flatten.Size = new System.Drawing.Size(28, 28);
      this.toolStripButton_Flatten.Text = "Flatten Terrain";
      this.toolStripButton_Flatten.ToolTipText = "Flatten the terrain using absolute height";
      this.toolStripButton_Flatten.Click += new System.EventHandler(this.toolStripButton_Flatten_Click);
      // 
      // toolStripButton_Smooth
      // 
      this.toolStripButton_Smooth.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_Smooth.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_Smooth.Image")));
      this.toolStripButton_Smooth.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_Smooth.Name = "toolStripButton_Smooth";
      this.toolStripButton_Smooth.Size = new System.Drawing.Size(28, 28);
      this.toolStripButton_Smooth.Text = "Smooth Terrain";
      this.toolStripButton_Smooth.Click += new System.EventHandler(this.toolStripButton_Smooth_Click);
      // 
      // toolStripButton_PickHeight
      // 
      this.toolStripButton_PickHeight.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_PickHeight.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_PickHeight.Image")));
      this.toolStripButton_PickHeight.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_PickHeight.Name = "toolStripButton_PickHeight";
      this.toolStripButton_PickHeight.Size = new System.Drawing.Size(28, 28);
      this.toolStripButton_PickHeight.Text = "Pick Absolute Height";
      this.toolStripButton_PickHeight.Click += new System.EventHandler(this.toolStripButton_PickHeight_Click);
      // 
      // toolStripButton_EditHoles
      // 
      this.toolStripButton_EditHoles.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_EditHoles.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_EditHoles.Image")));
      this.toolStripButton_EditHoles.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_EditHoles.Name = "toolStripButton_EditHoles";
      this.toolStripButton_EditHoles.Size = new System.Drawing.Size(28, 28);
      this.toolStripButton_EditHoles.Text = "Edit Terrain Holes";
      this.toolStripButton_EditHoles.Click += new System.EventHandler(this.toolStripButton_EditHoles_Click);
      // 
      // toolStripButton_PaintDetailTexture
      // 
      this.toolStripButton_PaintDetailTexture.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_PaintDetailTexture.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_PaintDetailTexture.Image")));
      this.toolStripButton_PaintDetailTexture.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_PaintDetailTexture.Name = "toolStripButton_PaintDetailTexture";
      this.toolStripButton_PaintDetailTexture.Size = new System.Drawing.Size(28, 28);
      this.toolStripButton_PaintDetailTexture.Text = "Paint Terrain";
      this.toolStripButton_PaintDetailTexture.ToolTipText = "Paint on terrain with current detail texture";
      this.toolStripButton_PaintDetailTexture.Click += new System.EventHandler(this.toolStripButton_PaintDetailTexture_Click);
      // 
      // toolStripSeparator4
      // 
      this.toolStripSeparator4.Name = "toolStripSeparator4";
      this.toolStripSeparator4.Size = new System.Drawing.Size(6, 31);
      // 
      // toolStripButton_TextureBaked
      // 
      this.toolStripButton_TextureBaked.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_TextureBaked.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_TextureBaked.Image")));
      this.toolStripButton_TextureBaked.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_TextureBaked.Name = "toolStripButton_TextureBaked";
      this.toolStripButton_TextureBaked.Size = new System.Drawing.Size(28, 28);
      this.toolStripButton_TextureBaked.Text = "Toggle Baked View";
      this.toolStripButton_TextureBaked.Click += new System.EventHandler(this.toolStripButton_TextureBaked_Click);
      // 
      // toolStripButton_DirectionalLighting
      // 
      this.toolStripButton_DirectionalLighting.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_DirectionalLighting.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_DirectionalLighting.Image")));
      this.toolStripButton_DirectionalLighting.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_DirectionalLighting.Name = "toolStripButton_DirectionalLighting";
      this.toolStripButton_DirectionalLighting.Size = new System.Drawing.Size(28, 28);
      this.toolStripButton_DirectionalLighting.Text = "Toggle Directional Light";
      this.toolStripButton_DirectionalLighting.ToolTipText = "If enabled, the terrain is lit by a directional light, otherwise it uses the curr" +
    "ent static lightmap";
      this.toolStripButton_DirectionalLighting.Click += new System.EventHandler(this.toolStripButton_DirectionalLighting_Click);
      // 
      // toolStripButton_FullRes
      // 
      this.toolStripButton_FullRes.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_FullRes.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_FullRes.Image")));
      this.toolStripButton_FullRes.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_FullRes.Name = "toolStripButton_FullRes";
      this.toolStripButton_FullRes.Size = new System.Drawing.Size(28, 28);
      this.toolStripButton_FullRes.Text = "Terrain Full Resolution";
      this.toolStripButton_FullRes.ToolTipText = "Preview terrain in full resolution (No LOD)";
      this.toolStripButton_FullRes.Click += new System.EventHandler(this.toolStripButton_FullRes_Click);
      // 
      // toolStripButton_DisplayVegetationDensity
      // 
      this.toolStripButton_DisplayVegetationDensity.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_DisplayVegetationDensity.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_DisplayVegetationDensity.Image")));
      this.toolStripButton_DisplayVegetationDensity.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_DisplayVegetationDensity.Name = "toolStripButton_DisplayVegetationDensity";
      this.toolStripButton_DisplayVegetationDensity.Size = new System.Drawing.Size(28, 28);
      this.toolStripButton_DisplayVegetationDensity.Text = "Vegetation Density";
      this.toolStripButton_DisplayVegetationDensity.ToolTipText = "Display the vegetation\'s density as luminance overlay";
      this.toolStripButton_DisplayVegetationDensity.Click += new System.EventHandler(this.toolStripButton_DisplayDensityMap_Click);
      // 
      // toolStripButton_DisplayMiniMap
      // 
      this.toolStripButton_DisplayMiniMap.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_DisplayMiniMap.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_DisplayMiniMap.Image")));
      this.toolStripButton_DisplayMiniMap.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_DisplayMiniMap.Name = "toolStripButton_DisplayMiniMap";
      this.toolStripButton_DisplayMiniMap.Size = new System.Drawing.Size(28, 28);
      this.toolStripButton_DisplayMiniMap.Text = "Terrain Minimap Texture";
      this.toolStripButton_DisplayMiniMap.ToolTipText = "Overlay the terrain\'s mini map texture (see terrain properties)";
      this.toolStripButton_DisplayMiniMap.Click += new System.EventHandler(this.toolStripButton_DisplayMiniMap_Click);
      // 
      // toolStripSeparator1
      // 
      this.toolStripSeparator1.Name = "toolStripSeparator1";
      this.toolStripSeparator1.Size = new System.Drawing.Size(6, 31);
      // 
      // toolStripButton_Selection
      // 
      this.toolStripButton_Selection.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_Selection.DoubleClickEnabled = true;
      this.toolStripButton_Selection.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_Selection.Image")));
      this.toolStripButton_Selection.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_Selection.Name = "toolStripButton_Selection";
      this.toolStripButton_Selection.Size = new System.Drawing.Size(28, 28);
      this.toolStripButton_Selection.Text = "Copy and Paste";
      this.toolStripButton_Selection.ToolTipText = "Show selection of copy and paste operations. Doubleclick to move selection to cam" +
    "era";
      this.toolStripButton_Selection.Click += new System.EventHandler(this.toolStripButton_Selection_Click);
      this.toolStripButton_Selection.DoubleClick += new System.EventHandler(this.toolStripButton_Selection_DoubleClick);
      // 
      // toolStripButton_PurgeResources
      // 
      this.toolStripButton_PurgeResources.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_PurgeResources.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_PurgeResources.Image")));
      this.toolStripButton_PurgeResources.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_PurgeResources.Name = "toolStripButton_PurgeResources";
      this.toolStripButton_PurgeResources.Size = new System.Drawing.Size(28, 28);
      this.toolStripButton_PurgeResources.Text = "Cleanup";
      this.toolStripButton_PurgeResources.ToolTipText = "Clears the UNDO history and unloads unused terrain sectors";
      this.toolStripButton_PurgeResources.Click += new System.EventHandler(this.toolStripButton_PurgeResources_Click);
      // 
      // splitContainer1
      // 
      this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.splitContainer1.Location = new System.Drawing.Point(0, 81);
      this.splitContainer1.Name = "splitContainer1";
      this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
      // 
      // splitContainer1.Panel1
      // 
      this.splitContainer1.Panel1.Controls.Add(this.tabControl);
      this.splitContainer1.Panel1.Paint += new System.Windows.Forms.PaintEventHandler(this.splitContainer1_Panel1_Paint);
      this.splitContainer1.Size = new System.Drawing.Size(433, 703);
      this.splitContainer1.SplitterDistance = 406;
      this.splitContainer1.TabIndex = 2;
      // 
      // dialogCaptionBar
      // 
      this.dialogCaptionBar.BackColor = System.Drawing.SystemColors.Window;
      this.dialogCaptionBar.Caption = "Terrain Editor";
      this.dialogCaptionBar.CompactView = false;
      this.dialogCaptionBar.Description = "Editor for creating and editing the terrain heightfield, apply textures, paint ve" +
    "getation, change shaders.";
      this.dialogCaptionBar.Dock = System.Windows.Forms.DockStyle.Top;
      this.dialogCaptionBar.Image = ((System.Drawing.Image)(resources.GetObject("dialogCaptionBar.Image")));
      this.dialogCaptionBar.Location = new System.Drawing.Point(0, 0);
      this.dialogCaptionBar.Name = "dialogCaptionBar";
      this.dialogCaptionBar.SetFontColor = System.Drawing.SystemColors.ControlText;
      this.dialogCaptionBar.ShowBorder = false;
      this.dialogCaptionBar.ShowBottomLine = true;
      this.dialogCaptionBar.ShowCaptionText = true;
      this.dialogCaptionBar.ShowImage = true;
      this.dialogCaptionBar.Size = new System.Drawing.Size(433, 50);
      this.dialogCaptionBar.TabIndex = 19;
      // 
      // TerrainEditorPanel
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(433, 784);
      this.ControlBox = false;
      this.Controls.Add(this.splitContainer1);
      this.Controls.Add(this.toolStrip_Main);
      this.Controls.Add(this.dialogCaptionBar);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "TerrainEditorPanel";
      this.TabText = "Terrain Editor";
      this.Text = "Terrain Editor";
      this.Load += new System.EventHandler(this.TerrainEditorPanel_Load);
      this.tabControl.ResumeLayout(false);
      this.tabPage_Heightmap.ResumeLayout(false);
      this.groupBox_ExportDDS.ResumeLayout(false);
      this.groupBox_ExportDDS.PerformLayout();
      this.groupBox_Import.ResumeLayout(false);
      this.groupBox_Terrain.ResumeLayout(false);
      this.tabPage_Texturing.ResumeLayout(false);
      this.tabPage_Texturing.PerformLayout();
      this.splitContainer3.Panel1.ResumeLayout(false);
      this.splitContainer3.Panel2.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.splitContainer3)).EndInit();
      this.splitContainer3.ResumeLayout(false);
      this.contextMenuStrip_DetailTexture.ResumeLayout(false);
      this.toolStrip_Texturing.ResumeLayout(false);
      this.toolStrip_Texturing.PerformLayout();
      this.tabPage_Decoration.ResumeLayout(false);
      this.tabPage_Decoration.PerformLayout();
      this.splitContainer2.Panel1.ResumeLayout(false);
      this.splitContainer2.Panel2.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).EndInit();
      this.splitContainer2.ResumeLayout(false);
      this.contextMenuStrip_Decoration.ResumeLayout(false);
      this.toolStrip_Vegetation.ResumeLayout(false);
      this.toolStrip_Vegetation.PerformLayout();
      this.tabPage_Shader.ResumeLayout(false);
      this.tabPage_MeshExport.ResumeLayout(false);
      this.groupBox_MeshExport.ResumeLayout(false);
      this.toolStrip_Main.ResumeLayout(false);
      this.toolStrip_Main.PerformLayout();
      this.splitContainer1.Panel1.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
      this.splitContainer1.ResumeLayout(false);
      this.ResumeLayout(false);
      this.PerformLayout();

		}

		#endregion

    private System.Windows.Forms.TabControl tabControl;
		private System.Windows.Forms.TabPage tabPage_Heightmap;
		private System.Windows.Forms.TabPage tabPage_Texturing;
		private System.Windows.Forms.TabPage tabPage_Decoration;
    private System.Windows.Forms.ListView listView_VegetationModels;
    private System.Windows.Forms.ColumnHeader columnHeader1;
    private System.Windows.Forms.ColumnHeader columnHeader2;
    private System.Windows.Forms.ToolStrip toolStrip_Vegetation;
    private System.Windows.Forms.ToolStripButton toolStripButton_AddDecoChannel;
    private System.Windows.Forms.ToolStripButton toolStripButton_DeleteDecoChannel;
    private CSharpFramework.Controls.PropertyGridNested propertyGrid_VegetationModel;
    private System.Windows.Forms.ToolStrip toolStrip_Texturing;
    private System.Windows.Forms.ToolStripButton toolStripButton_AddDetailTexture;
    private System.Windows.Forms.ToolStripButton toolStripButton_DeleteDetailTexture;
    private System.Windows.Forms.ListView listView_DetailTextures;
    private System.Windows.Forms.ColumnHeader columnHeader_Name;
    private System.Windows.Forms.ColumnHeader columnHeader_File;
    private CSharpFramework.Controls.PropertyGridNested propertyGrid_DetailTexture;
    private System.Windows.Forms.Label label_ImportHeightfield;
    private System.Windows.Forms.ComboBox comboBox_ApplyFilter;
    private System.Windows.Forms.Label label_ApplyFilter;
    private System.Windows.Forms.ToolStrip toolStrip_Main;
    private System.Windows.Forms.ToolStripButton toolStripButton_Elevate;
    private System.Windows.Forms.ToolStripButton toolStripButton_Flatten;
    private System.Windows.Forms.ToolStripButton toolStripButton_Smooth;
    private System.Windows.Forms.ToolStripButton toolStripButton_PickHeight;
    private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
    private System.Windows.Forms.ToolStripButton toolStripButton_TextureBaked;
    private System.Windows.Forms.ToolStripButton toolStripButton_PaintDetailTexture;
    private System.Windows.Forms.ToolStripButton toolStripButton_DisplayVegetationDensity;
    private System.Windows.Forms.ToolStripButton toolStripButton_DirectionalLighting;
    private System.Windows.Forms.TabPage tabPage_Shader;
    private CSharpFramework.Controls.PropertyGridNested propertyGrid_Shader;
    private System.Windows.Forms.ToolStripButton toolStripButton_ImportWeightmap;
    private System.Windows.Forms.ToolStripButton toolStripButton_PurgeResources;
    private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
    private System.Windows.Forms.ToolStripButton toolStripButton_RebuildVegetation;
    private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
    private System.Windows.Forms.ToolStripButton toolStripButton_EditHoles;
    private System.Windows.Forms.ToolStripButton toolStripButton_SlopeTextures;
    private System.Windows.Forms.ContextMenuStrip contextMenuStrip_Decoration;
    private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem_DecorationFilters;
    private System.Windows.Forms.SplitContainer splitContainer1;
    private System.Windows.Forms.ToolStripButton toolStripButton_Blink;
    private System.Windows.Forms.ToolStripSeparator toolStripSeparator5;
    private System.Windows.Forms.Button button_ChangeConfig;
    private System.Windows.Forms.ToolStripButton toolStripButton_MoveUp;
    private System.Windows.Forms.ToolStripButton toolStripButton_MoveDown;
    private System.Windows.Forms.ToolStripSeparator toolStripSeparator6;
    private System.Windows.Forms.ToolStripButton toolStripButton_DisplayMiniMap;
    private System.Windows.Forms.Button button_ExportHeightmapDDS;
    private System.Windows.Forms.TextBox textBox_ExportDDS;
    private System.Windows.Forms.Label targetLabel_ExportDDS;
    private System.Windows.Forms.Button button_ExportFilenameBrowse;
    private System.Windows.Forms.ToolStripSeparator toolStripMenuItem1;
    private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_ExportDensity;
    private System.Windows.Forms.ContextMenuStrip contextMenuStrip_DetailTexture;
    private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_ExportTextureLuminance;
    private System.Windows.Forms.ToolStripButton toolStripButton_SplitPerSectorTex;
    private System.Windows.Forms.ToolStripButton toolStripButton_Selection;
    private System.Windows.Forms.CheckBox checkBox_NormalizedHMExport;
    private System.Windows.Forms.ToolStripSplitButton toolStripSplitButton_Atlas;
    private System.Windows.Forms.ToolStripMenuItem createNewBillboardAtlasToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem editExistingBillboardAtlasToolStripMenuItem;
    private System.Windows.Forms.ToolStripSeparator toolStripSeparator7;
    public System.Windows.Forms.ComboBox comboBox_ImportHeightfield;
    private System.Windows.Forms.ToolStripButton toolStripButton_RebuildDirtyRange;
    private System.Windows.Forms.ToolStripButton toolStripButton_AddMeshComponent;
    private System.Windows.Forms.ToolStripButton toolStripButton_RemoveMeshComponent;
    private System.Windows.Forms.ToolStripSeparator toolStripSeparator8;
    private System.Windows.Forms.ToolStripMenuItem createLODVersionToolStripMenuItem;
    private System.Windows.Forms.ToolStripButton toolStripButton_FullRes;
    private System.Windows.Forms.ToolStripButton toolStripButton_MaterialID;
    private System.Windows.Forms.SplitContainer splitContainer2;
    private System.Windows.Forms.SplitContainer splitContainer3;
    private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
    private CSharpFramework.Controls.DialogCaptionBar dialogCaptionBar;
    private CustomGroupBox groupBox_Import;
    private CustomGroupBox groupBox_Terrain;
    private CustomGroupBox groupBox_ExportDDS;
    private System.Windows.Forms.Splitter splitter3;
    private System.Windows.Forms.Splitter splitter2;
    private System.Windows.Forms.Splitter splitter1;
    private System.Windows.Forms.Label label_RemapToRange;
    private System.Windows.Forms.Label label_ExportHeightmap;
    private System.Windows.Forms.Label label_TerrainSettings;
    private System.Windows.Forms.TabPage tabPage_MeshExport;
    private System.Windows.Forms.PropertyGrid propertyGrid_MeshExport;
    private System.Windows.Forms.Splitter splitter5;
    private CustomGroupBox groupBox_MeshExport;
    private System.Windows.Forms.Label label_ProcessExport;
    private System.Windows.Forms.Button button_ExportPrefab;
    private System.Windows.Forms.Label label_Export;
    private System.Windows.Forms.Splitter splitter4;
	}
}