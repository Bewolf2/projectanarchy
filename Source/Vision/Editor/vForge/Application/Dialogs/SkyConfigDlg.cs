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
using ManagedFramework;
using CSharpFramework;
using CSharpFramework.Scene;
using System.Drawing.Design;
using Editor.Actions;
using CSharpFramework.Clipboard;
using System.Runtime.Serialization.Formatters.Binary;
using CSharpFramework.Serialization;
using System.IO;

namespace Editor.Dialogs
{


	/// <summary>
	/// Dialog to configure the engine sky
	/// </summary>
	public class SkyConfigDlg : System.Windows.Forms.Form
	{
    private System.Windows.Forms.Button button_OK;
    private System.Windows.Forms.Button button_Cancel;
    private CSharpFramework.Controls.PropertyGridNested PropertyGrid_Layer;
    private System.Windows.Forms.ListView ListView_Layers;
    private System.Windows.Forms.ColumnHeader columnHeader_LayerName;
    private CSharpFramework.Controls.PropertyGridNested PropertyGrid_Sky;
    private System.Windows.Forms.Button button_AddLayer;
    private System.Windows.Forms.Button button_RemoveLayer;
    private Button button_LayerUp;
    private Button button_LayerDown;
    private GroupBox GroupBox_SkyProperties;
    private GroupBox GroupBox_SkyLayer;
    private GroupBox GroupBox_LayerProperties;
    private GroupBox GroupBox_OK;
    private CSharpFramework.Controls.DialogCaptionBar DialogCaptionBar;
    private GroupBox GroupBox_Options;
    private Button button_Copy;
    private Button button_Paste;
    private Button button_Save;
    private Button button_Load;
    private Button button_Export;
    private ToolTip ToolTip;
    private IContainer components;

    /// <summary>
    /// Constructor
    /// </summary>
		public SkyConfigDlg()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();
      FillLayerList(0);
      button_Paste.Enabled = EditorManager.Clipboard.DataObject is SkyConfig;
    }

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
        _skyConfig = null;
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      this.components = new System.ComponentModel.Container();
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(SkyConfigDlg));
      this.button_OK = new System.Windows.Forms.Button();
      this.button_Cancel = new System.Windows.Forms.Button();
      this.PropertyGrid_Layer = new CSharpFramework.Controls.PropertyGridNested();
      this.ListView_Layers = new System.Windows.Forms.ListView();
      this.columnHeader_LayerName = new System.Windows.Forms.ColumnHeader();
      this.PropertyGrid_Sky = new CSharpFramework.Controls.PropertyGridNested();
      this.button_AddLayer = new System.Windows.Forms.Button();
      this.button_RemoveLayer = new System.Windows.Forms.Button();
      this.button_LayerUp = new System.Windows.Forms.Button();
      this.button_LayerDown = new System.Windows.Forms.Button();
      this.GroupBox_SkyProperties = new System.Windows.Forms.GroupBox();
      this.GroupBox_SkyLayer = new System.Windows.Forms.GroupBox();
      this.GroupBox_LayerProperties = new System.Windows.Forms.GroupBox();
      this.GroupBox_OK = new System.Windows.Forms.GroupBox();
      this.DialogCaptionBar = new CSharpFramework.Controls.DialogCaptionBar();
      this.GroupBox_Options = new System.Windows.Forms.GroupBox();
      this.button_Export = new System.Windows.Forms.Button();
      this.button_Load = new System.Windows.Forms.Button();
      this.button_Save = new System.Windows.Forms.Button();
      this.button_Paste = new System.Windows.Forms.Button();
      this.button_Copy = new System.Windows.Forms.Button();
      this.ToolTip = new System.Windows.Forms.ToolTip(this.components);
      this.GroupBox_SkyProperties.SuspendLayout();
      this.GroupBox_SkyLayer.SuspendLayout();
      this.GroupBox_LayerProperties.SuspendLayout();
      this.GroupBox_OK.SuspendLayout();
      this.GroupBox_Options.SuspendLayout();
      this.SuspendLayout();
      // 
      // button_OK
      // 
      this.button_OK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.button_OK.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.button_OK.Location = new System.Drawing.Point(7, 13);
      this.button_OK.Name = "button_OK";
      this.button_OK.Size = new System.Drawing.Size(96, 37);
      this.button_OK.TabIndex = 11;
      this.button_OK.Text = "OK";
      this.button_OK.Click += new System.EventHandler(this.button_OK_Click);
      // 
      // button_Cancel
      // 
      this.button_Cancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.button_Cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.button_Cancel.Location = new System.Drawing.Point(110, 13);
      this.button_Cancel.Name = "button_Cancel";
      this.button_Cancel.Size = new System.Drawing.Size(96, 37);
      this.button_Cancel.TabIndex = 12;
      this.button_Cancel.Text = "Cancel";
      // 
      // PropertyGrid_Layer
      // 
      this.PropertyGrid_Layer.Dock = System.Windows.Forms.DockStyle.Fill;
      this.PropertyGrid_Layer.HelpBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(252)))), ((int)(((byte)(252)))), ((int)(((byte)(252)))));
      this.PropertyGrid_Layer.HelpBackgroundImage = ((System.Drawing.Image)(resources.GetObject("PropertyGrid_Layer.HelpBackgroundImage")));
      this.PropertyGrid_Layer.LineColor = System.Drawing.Color.FromArgb(((int)(((byte)(248)))), ((int)(((byte)(248)))), ((int)(((byte)(248)))));
      this.PropertyGrid_Layer.Location = new System.Drawing.Point(6, 21);
      this.PropertyGrid_Layer.Name = "PropertyGrid_Layer";
      this.PropertyGrid_Layer.Size = new System.Drawing.Size(443, 427);
      this.PropertyGrid_Layer.TabIndex = 10;
      this.PropertyGrid_Layer.ToolbarVisible = false;
      // 
      // 
      // 
      this.PropertyGrid_Layer.ToolStrip.AccessibleName = "ToolBar";
      this.PropertyGrid_Layer.ToolStrip.AccessibleRole = System.Windows.Forms.AccessibleRole.ToolBar;
      this.PropertyGrid_Layer.ToolStrip.AllowMerge = false;
      this.PropertyGrid_Layer.ToolStrip.AutoSize = false;
      this.PropertyGrid_Layer.ToolStrip.CanOverflow = false;
      this.PropertyGrid_Layer.ToolStrip.Dock = System.Windows.Forms.DockStyle.None;
      this.PropertyGrid_Layer.ToolStrip.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
      this.PropertyGrid_Layer.ToolStrip.Location = new System.Drawing.Point(0, 0);
      this.PropertyGrid_Layer.ToolStrip.Name = "";
      this.PropertyGrid_Layer.ToolStrip.Padding = new System.Windows.Forms.Padding(2, 0, 1, 0);
      this.PropertyGrid_Layer.ToolStrip.TabIndex = 1;
      this.PropertyGrid_Layer.ToolStrip.TabStop = true;
      this.PropertyGrid_Layer.ToolStrip.Text = "PropertyGridToolBar";
      this.PropertyGrid_Layer.ToolStrip.Visible = false;
      this.PropertyGrid_Layer.PropertyValueChanged += new System.Windows.Forms.PropertyValueChangedEventHandler(this.propertyGrid1_PropertyValueChanged);
      // 
      // ListView_Layers
      // 
      this.ListView_Layers.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.ListView_Layers.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader_LayerName});
      this.ListView_Layers.FullRowSelect = true;
      this.ListView_Layers.GridLines = true;
      this.ListView_Layers.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
      this.ListView_Layers.HideSelection = false;
      this.ListView_Layers.Location = new System.Drawing.Point(7, 22);
      this.ListView_Layers.MultiSelect = false;
      this.ListView_Layers.Name = "ListView_Layers";
      this.ListView_Layers.Size = new System.Drawing.Size(305, 175);
      this.ListView_Layers.TabIndex = 4;
      this.ListView_Layers.UseCompatibleStateImageBehavior = false;
      this.ListView_Layers.View = System.Windows.Forms.View.Details;
      this.ListView_Layers.SelectedIndexChanged += new System.EventHandler(this.listView_layers_SelectedIndexChanged);
      // 
      // columnHeader_LayerName
      // 
      this.columnHeader_LayerName.Text = "Sky layer";
      this.columnHeader_LayerName.Width = 249;
      // 
      // PropertyGrid_Sky
      // 
      this.PropertyGrid_Sky.Dock = System.Windows.Forms.DockStyle.Fill;
      this.PropertyGrid_Sky.HelpBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(252)))), ((int)(((byte)(252)))), ((int)(((byte)(252)))));
      this.PropertyGrid_Sky.HelpBackgroundImage = ((System.Drawing.Image)(resources.GetObject("PropertyGrid_Sky.HelpBackgroundImage")));
      this.PropertyGrid_Sky.LineColor = System.Drawing.Color.FromArgb(((int)(((byte)(248)))), ((int)(((byte)(248)))), ((int)(((byte)(248)))));
      this.PropertyGrid_Sky.Location = new System.Drawing.Point(6, 21);
      this.PropertyGrid_Sky.Name = "PropertyGrid_Sky";
      this.PropertyGrid_Sky.PropertySort = System.Windows.Forms.PropertySort.Categorized;
      this.PropertyGrid_Sky.Size = new System.Drawing.Size(307, 244);
      this.PropertyGrid_Sky.TabIndex = 2;
      this.PropertyGrid_Sky.ToolbarVisible = false;
      // 
      // 
      // 
      this.PropertyGrid_Sky.ToolStrip.AccessibleName = "ToolBar";
      this.PropertyGrid_Sky.ToolStrip.AccessibleRole = System.Windows.Forms.AccessibleRole.ToolBar;
      this.PropertyGrid_Sky.ToolStrip.AllowMerge = false;
      this.PropertyGrid_Sky.ToolStrip.AutoSize = false;
      this.PropertyGrid_Sky.ToolStrip.CanOverflow = false;
      this.PropertyGrid_Sky.ToolStrip.Dock = System.Windows.Forms.DockStyle.None;
      this.PropertyGrid_Sky.ToolStrip.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
      this.PropertyGrid_Sky.ToolStrip.Location = new System.Drawing.Point(0, 0);
      this.PropertyGrid_Sky.ToolStrip.Name = "";
      this.PropertyGrid_Sky.ToolStrip.Padding = new System.Windows.Forms.Padding(2, 0, 1, 0);
      this.PropertyGrid_Sky.ToolStrip.TabIndex = 1;
      this.PropertyGrid_Sky.ToolStrip.TabStop = true;
      this.PropertyGrid_Sky.ToolStrip.Text = "PropertyGridToolBar";
      this.PropertyGrid_Sky.ToolStrip.Visible = false;
      // 
      // button_AddLayer
      // 
      this.button_AddLayer.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.button_AddLayer.Location = new System.Drawing.Point(94, 204);
      this.button_AddLayer.Name = "button_AddLayer";
      this.button_AddLayer.Size = new System.Drawing.Size(105, 28);
      this.button_AddLayer.TabIndex = 7;
      this.button_AddLayer.Text = "Add Layer";
      this.button_AddLayer.Click += new System.EventHandler(this.button_AddLayer_Click);
      // 
      // button_RemoveLayer
      // 
      this.button_RemoveLayer.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.button_RemoveLayer.Location = new System.Drawing.Point(206, 204);
      this.button_RemoveLayer.Name = "button_RemoveLayer";
      this.button_RemoveLayer.Size = new System.Drawing.Size(106, 28);
      this.button_RemoveLayer.TabIndex = 8;
      this.button_RemoveLayer.Text = "Remove Layer";
      this.button_RemoveLayer.Click += new System.EventHandler(this.button_RemoveLayer_Click);
      // 
      // button_LayerUp
      // 
      this.button_LayerUp.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
      this.button_LayerUp.Image = global::Editor.Properties.Resources.arrow_up_blue;
      this.button_LayerUp.Location = new System.Drawing.Point(7, 204);
      this.button_LayerUp.Name = "button_LayerUp";
      this.button_LayerUp.Size = new System.Drawing.Size(33, 28);
      this.button_LayerUp.TabIndex = 5;
      this.button_LayerUp.UseVisualStyleBackColor = true;
      this.button_LayerUp.Click += new System.EventHandler(this.button_LayerUp_Click);
      // 
      // button_LayerDown
      // 
      this.button_LayerDown.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
      this.button_LayerDown.Image = global::Editor.Properties.Resources.arrow_down_blue;
      this.button_LayerDown.Location = new System.Drawing.Point(47, 204);
      this.button_LayerDown.Name = "button_LayerDown";
      this.button_LayerDown.Size = new System.Drawing.Size(32, 28);
      this.button_LayerDown.TabIndex = 6;
      this.button_LayerDown.UseVisualStyleBackColor = true;
      this.button_LayerDown.Click += new System.EventHandler(this.button_LayerDown_Click);
      // 
      // GroupBox_SkyProperties
      // 
      this.GroupBox_SkyProperties.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)));
      this.GroupBox_SkyProperties.Controls.Add(this.PropertyGrid_Sky);
      this.GroupBox_SkyProperties.Location = new System.Drawing.Point(10, 65);
      this.GroupBox_SkyProperties.Name = "GroupBox_SkyProperties";
      this.GroupBox_SkyProperties.Padding = new System.Windows.Forms.Padding(6, 6, 6, 7);
      this.GroupBox_SkyProperties.Size = new System.Drawing.Size(319, 272);
      this.GroupBox_SkyProperties.TabIndex = 13;
      this.GroupBox_SkyProperties.TabStop = false;
      this.GroupBox_SkyProperties.Text = "Sky Properties";
      // 
      // GroupBox_SkyLayer
      // 
      this.GroupBox_SkyLayer.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
      this.GroupBox_SkyLayer.Controls.Add(this.button_LayerDown);
      this.GroupBox_SkyLayer.Controls.Add(this.button_AddLayer);
      this.GroupBox_SkyLayer.Controls.Add(this.button_LayerUp);
      this.GroupBox_SkyLayer.Controls.Add(this.button_RemoveLayer);
      this.GroupBox_SkyLayer.Controls.Add(this.ListView_Layers);
      this.GroupBox_SkyLayer.Location = new System.Drawing.Point(12, 343);
      this.GroupBox_SkyLayer.Name = "GroupBox_SkyLayer";
      this.GroupBox_SkyLayer.Size = new System.Drawing.Size(319, 240);
      this.GroupBox_SkyLayer.TabIndex = 14;
      this.GroupBox_SkyLayer.TabStop = false;
      this.GroupBox_SkyLayer.Text = "Sky Layer";
      // 
      // GroupBox_LayerProperties
      // 
      this.GroupBox_LayerProperties.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.GroupBox_LayerProperties.Controls.Add(this.PropertyGrid_Layer);
      this.GroupBox_LayerProperties.Location = new System.Drawing.Point(335, 65);
      this.GroupBox_LayerProperties.Name = "GroupBox_LayerProperties";
      this.GroupBox_LayerProperties.Padding = new System.Windows.Forms.Padding(6, 6, 6, 7);
      this.GroupBox_LayerProperties.Size = new System.Drawing.Size(455, 455);
      this.GroupBox_LayerProperties.TabIndex = 15;
      this.GroupBox_LayerProperties.TabStop = false;
      this.GroupBox_LayerProperties.Text = "Layer Properties";
      // 
      // GroupBox_OK
      // 
      this.GroupBox_OK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.GroupBox_OK.Controls.Add(this.button_OK);
      this.GroupBox_OK.Controls.Add(this.button_Cancel);
      this.GroupBox_OK.Location = new System.Drawing.Point(576, 526);
      this.GroupBox_OK.Name = "GroupBox_OK";
      this.GroupBox_OK.Size = new System.Drawing.Size(214, 57);
      this.GroupBox_OK.TabIndex = 16;
      this.GroupBox_OK.TabStop = false;
      // 
      // DialogCaptionBar
      // 
      this.DialogCaptionBar.BackColor = System.Drawing.SystemColors.Window;
      this.DialogCaptionBar.Caption = "Sky Properties";
      this.DialogCaptionBar.CompactView = false;
      this.DialogCaptionBar.Description = "Here you can specify the sky shader and its properties that is used in your scene" +
          ", add sky layers and specify the respective properties for each sky layer.";
      this.DialogCaptionBar.Dock = System.Windows.Forms.DockStyle.Top;
      this.DialogCaptionBar.Image = ((System.Drawing.Image)(resources.GetObject("DialogCaptionBar.Image")));
      this.DialogCaptionBar.Location = new System.Drawing.Point(0, 0);
      this.DialogCaptionBar.Name = "DialogCaptionBar";
      this.DialogCaptionBar.SetFontColor = System.Drawing.SystemColors.ControlText;
      this.DialogCaptionBar.ShowBorder = false;
      this.DialogCaptionBar.ShowBottomLine = true;
      this.DialogCaptionBar.ShowCaptionText = true;
      this.DialogCaptionBar.ShowImage = true;
      this.DialogCaptionBar.Size = new System.Drawing.Size(802, 58);
      this.DialogCaptionBar.TabIndex = 17;
      // 
      // GroupBox_Options
      // 
      this.GroupBox_Options.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.GroupBox_Options.Controls.Add(this.button_Export);
      this.GroupBox_Options.Controls.Add(this.button_Load);
      this.GroupBox_Options.Controls.Add(this.button_Save);
      this.GroupBox_Options.Controls.Add(this.button_Paste);
      this.GroupBox_Options.Controls.Add(this.button_Copy);
      this.GroupBox_Options.Location = new System.Drawing.Point(335, 526);
      this.GroupBox_Options.Name = "GroupBox_Options";
      this.GroupBox_Options.Size = new System.Drawing.Size(235, 57);
      this.GroupBox_Options.TabIndex = 19;
      this.GroupBox_Options.TabStop = false;
      // 
      // button_Export
      // 
      this.button_Export.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
      this.button_Export.Image = global::Editor.Properties.Resources.toolbar_export;
      this.button_Export.Location = new System.Drawing.Point(190, 13);
      this.button_Export.Name = "button_Export";
      this.button_Export.Size = new System.Drawing.Size(38, 37);
      this.button_Export.TabIndex = 20;
      this.ToolTip.SetToolTip(this.button_Export, "Export the current setup to a simple engine archive format that can be read by th" +
              "e engine (VisSky_cl::ReadFromStream)");
      this.button_Export.UseVisualStyleBackColor = true;
      this.button_Export.Click += new System.EventHandler(this.toolStripButton_Export_Click);
      // 
      // button_Load
      // 
      this.button_Load.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
      this.button_Load.Image = global::Editor.Properties.Resources.toolbar_open;
      this.button_Load.Location = new System.Drawing.Point(144, 13);
      this.button_Load.Name = "button_Load";
      this.button_Load.Size = new System.Drawing.Size(38, 37);
      this.button_Load.TabIndex = 9;
      this.ToolTip.SetToolTip(this.button_Load, "Load a sky setup from a binary file");
      this.button_Load.UseVisualStyleBackColor = true;
      this.button_Load.Click += new System.EventHandler(this.toolStripButton_Load_Click);
      // 
      // button_Save
      // 
      this.button_Save.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
      this.button_Save.Image = global::Editor.Properties.Resources.toolbar_save;
      this.button_Save.Location = new System.Drawing.Point(98, 13);
      this.button_Save.Name = "button_Save";
      this.button_Save.Size = new System.Drawing.Size(39, 37);
      this.button_Save.TabIndex = 8;
      this.ToolTip.SetToolTip(this.button_Save, "Save the current setup to a binary file that can be loaded later by other scenes");
      this.button_Save.UseVisualStyleBackColor = true;
      this.button_Save.Click += new System.EventHandler(this.toolStripButton_Save_Click);
      // 
      // button_Paste
      // 
      this.button_Paste.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
      this.button_Paste.Image = global::Editor.Properties.Resources.toolbar_paste;
      this.button_Paste.Location = new System.Drawing.Point(53, 13);
      this.button_Paste.Name = "button_Paste";
      this.button_Paste.Size = new System.Drawing.Size(38, 37);
      this.button_Paste.TabIndex = 7;
      this.ToolTip.SetToolTip(this.button_Paste, "Paste this sky config from clipboard");
      this.button_Paste.UseVisualStyleBackColor = true;
      this.button_Paste.Click += new System.EventHandler(this.toolStripButton_PasteConfig_Click);
      // 
      // button_Copy
      // 
      this.button_Copy.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
      this.button_Copy.Image = global::Editor.Properties.Resources.toolbar_copy;
      this.button_Copy.Location = new System.Drawing.Point(8, 13);
      this.button_Copy.Name = "button_Copy";
      this.button_Copy.Size = new System.Drawing.Size(39, 37);
      this.button_Copy.TabIndex = 6;
      this.ToolTip.SetToolTip(this.button_Copy, "Copy this sky config to clipboard");
      this.button_Copy.UseVisualStyleBackColor = true;
      this.button_Copy.Click += new System.EventHandler(this.toolStripButton_CopyConfig_Click);
      // 
      // SkyConfigDlg
      // 
      this.AcceptButton = this.button_OK;
      this.AutoScaleBaseSize = new System.Drawing.Size(6, 15);
      this.CancelButton = this.button_Cancel;
      this.ClientSize = new System.Drawing.Size(802, 595);
      this.Controls.Add(this.GroupBox_Options);
      this.Controls.Add(this.DialogCaptionBar);
      this.Controls.Add(this.GroupBox_OK);
      this.Controls.Add(this.GroupBox_LayerProperties);
      this.Controls.Add(this.GroupBox_SkyLayer);
      this.Controls.Add(this.GroupBox_SkyProperties);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.MinimumSize = new System.Drawing.Size(820, 640);
      this.Name = "SkyConfigDlg";
      this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
      this.Text = "Edit Sky";
      this.GroupBox_SkyProperties.ResumeLayout(false);
      this.GroupBox_SkyLayer.ResumeLayout(false);
      this.GroupBox_LayerProperties.ResumeLayout(false);
      this.GroupBox_OK.ResumeLayout(false);
      this.GroupBox_Options.ResumeLayout(false);
      this.ResumeLayout(false);

    }

    #endregion


    #region Access Sky config

    SkyConfig _skyConfig = null;

    /// <summary>
    /// get or set the sky config. Set will clone the passed config
    /// </summary>
    public SkyConfig SkyConfig
    {
      get {return _skyConfig;}
      set 
      {
        if (_skyConfig != null)
          _skyConfig.Active = false;
        _skyConfig = value.Clone() as SkyConfig; // make a copy so we can still cancel the operation
        _skyConfig.Active = true;
        FillLayerList(0);
      }
    }
    #endregion

    #region Layer list

    private int SelectedLayerIndex 
    {
      get 
      {
        if (ListView_Layers.SelectedIndices.Count!=1) return -1;
        return ListView_Layers.SelectedIndices[0];
      }
    }

    private void listView_layers_SelectedIndexChanged(object sender, System.EventArgs e)
    {
      int iIndex = SelectedLayerIndex;
      SetButtonStates();
      if (iIndex<0)
      {
        this.PropertyGrid_Layer.SelectedObject = null;
        return;
      }
      this.PropertyGrid_Layer.SelectedObject = SkyConfig.Layers[iIndex].PropertyObject;
    }

    /// <summary>
    /// puts all sky layers into the list control
    /// </summary>
    void FillLayerList(int iSelectedItem)
    {
      ListView_Layers.Items.Clear();
      ListView_Layers.SelectedItems.Clear();
      if (SkyConfig == null || SkyConfig.Layers == null)
      {
        SetButtonStates();
        return;
      }
      ListView_Layers.BeginUpdate();
      foreach (SkyLayer layer in SkyConfig.Layers)
        ListView_Layers.Items.Add(layer.ToString());

      // select the first layer
      if (SkyConfig.Layers.Count > 0)
      {
        if (iSelectedItem>=ListView_Layers.Items.Count)
          iSelectedItem = ListView_Layers.Items.Count-1;

        ListView_Layers.Items[iSelectedItem].Selected = true;
      }
      ListView_Layers.EndUpdate();

      // enable/disable layer buttons
      SetButtonStates();
      this.PropertyGrid_Sky.SelectedObject = _skyConfig;
    }

    private void button_LayerUp_Click(object sender, EventArgs e)
    {
      int iIndex = SelectedLayerIndex;
      if (iIndex < 1)
        return;
      SkyLayer layer = SkyConfig.Layers[iIndex]; 
      SkyConfig.Layers.RemoveAt(iIndex);
      SkyConfig.Layers.InsertAt(iIndex - 1, layer);
      FillLayerList(iIndex - 1);
      SkyConfig.Update();
    }

    private void button_LayerDown_Click(object sender, EventArgs e)
    {
      int iIndex = SelectedLayerIndex;
      if (iIndex < 0 || iIndex >= SkyConfig.Layers.Count-1)
        return;
      SkyLayer layer = SkyConfig.Layers[iIndex];
      SkyConfig.Layers.RemoveAt(iIndex);
      SkyConfig.Layers.InsertAt(iIndex + 1, layer);
      FillLayerList(iIndex + 1);
      SkyConfig.Update();
    }

    #endregion

    #region Sky layer property grid

    private void propertyGrid1_PropertyValueChanged(object s, System.Windows.Forms.PropertyValueChangedEventArgs e)
    {
      if (SkyConfig==null) return;

      // Atmospheric scattering doesn't need to rebuild lookups on layer changes
      if (SkyConfig.SkyClass != "VAtmosphere")
      {
        SkyConfig.RecomputeLookup = true;
      }

      SkyConfig.Update();
      if (e.ChangedItem.Label=="Mapping" || e.ChangedItem.Label=="LayerName") // when mapping type changes, update the list view
      {
        FillLayerList(SelectedLayerIndex);
        //listView_layers_SelectedIndexChanged(null,null); // update the property object if layer type changed
      }
    }

    #endregion

    #region Add and remove Sky layer

    void SetButtonStates()
    {
      if (SkyConfig!=null)
      {
        int iSelIndex = SelectedLayerIndex;
        button_LayerDown.Enabled = iSelIndex >= 0 && iSelIndex < SkyConfig.LayerCount-1;
        button_LayerUp.Enabled = iSelIndex > 0;
        button_AddLayer.Enabled = SkyConfig.LayerCount < SkyConfig.MAX_LAYER_COUNT;
        button_RemoveLayer.Enabled = ListView_Layers.SelectedIndices.Count==1;
      } 
      else
      {
        button_LayerDown.Enabled = false;
        button_LayerUp.Enabled = false;
        button_AddLayer.Enabled = false;
        button_RemoveLayer.Enabled = false;
      }
    }


    static bool bLayerCountReported = false;

    private void button_AddLayer_Click(object sender, System.EventArgs e)
    {
      int iIndex = SkyConfig.Layers.Add(new SkyLayer());
      if (SkyConfig.LayerCount > 4 && !bLayerCountReported)
      {
        bLayerCountReported = true;
        EditorManager.ShowMessageBox("More than 4 layers are supported but a custom shader is needed to blend additional layers.", "More than 4 Sky Layers", MessageBoxButtons.OK, MessageBoxIcon.Information);
      }

      FillLayerList(iIndex);
      SkyConfig.Update();
    }

    private void button_RemoveLayer_Click(object sender, System.EventArgs e)
    {
      int iIndex = SelectedLayerIndex;
      System.Diagnostics.Debug.Assert(iIndex>=0);
      SkyConfig.Layers.RemoveAt(iIndex);
      FillLayerList(iIndex);
      SkyConfig.Update();
    }

    #endregion

    #region Copy & Paste config

    private void toolStripButton_CopyConfig_Click(object sender, EventArgs e)
    {
      EditorManager.Clipboard.Data = EditorClipboard.CreateDataObject(this.SkyConfig.Clone(),"Sky config");
      button_Paste.Enabled = true;
    }

    private void toolStripButton_PasteConfig_Click(object sender, EventArgs e)
    {
      SkyConfig newConf = EditorManager.Clipboard.DataObject as SkyConfig;
      if (newConf == null)
        return;
      SkyConfig = newConf; // this clones it
      SkyConfig.Update();
    }

    #endregion

    private void toolStripButton_Save_Click(object sender, EventArgs e)
    {
      if (_skyConfig == null)
        return;
      SaveFileDialog dlg = new SaveFileDialog();
      dlg.InitialDirectory = EditorManager.Project.ProjectDir;
      dlg.FileName = null;
      dlg.Filter = "Sky setup|*.Sky";
      dlg.Title = "Save this sky setup to editor format";

      if (dlg.ShowDialog(this) != DialogResult.OK)
        return;

      string filename = dlg.FileName;
      dlg.Dispose();

      try
      {
        BinaryFormatter fmt = SerializationHelper.BINARY_FORMATTER;
        FileStream fs = new FileStream(filename, FileMode.Create);
        fmt.Serialize(fs, this._skyConfig);
        fs.Close();
  }
      catch (Exception ex)
      {
        EditorManager.ShowMessageBox("Failed to save sky setup to file:\n\n" + ex.Message, "Error saving file", MessageBoxButtons.OK, MessageBoxIcon.Error);
      }
    }

    private void toolStripButton_Load_Click(object sender, EventArgs e)
    {
      if (_skyConfig == null)
        return;
      OpenFileDialog dlg = new OpenFileDialog();
      dlg.InitialDirectory = EditorManager.Project.ProjectDir;
      dlg.FileName = null;
      dlg.Filter = "Sky setup|*.Sky";
      dlg.Title = "Load Sky setup from file";

      if (dlg.ShowDialog(this) != DialogResult.OK)
        return;

      string filename = dlg.FileName;
      dlg.Dispose();

      SkyConfig _sky = null;
      try
      {
        BinaryFormatter fmt = SerializationHelper.BINARY_FORMATTER;
        FileStream fs = new FileStream(filename, FileMode.Open, FileAccess.Read);
        _sky = (SkyConfig)fmt.Deserialize(fs);
        fs.Close();
      }
      catch (Exception ex)
      {
        EditorManager.ShowMessageBox("Failed to load sky setup from file:\n\n" + ex.Message, "Error loading file", MessageBoxButtons.OK, MessageBoxIcon.Error);
      }
      if (_sky != null)
      {
        SkyConfig = _sky;
        SkyConfig.Update();
      }
      EditorManager.ActiveView.UpdateView(true);
    }

    private void toolStripButton_Export_Click(object sender, EventArgs e)
    {
      if (_skyConfig == null)
        return;
      SaveFileDialog dlg = new SaveFileDialog();
      dlg.InitialDirectory = EditorManager.Project.ProjectDir;
      dlg.FileName = null;
      dlg.Filter = "Exported Sky setup|*.VSky";
      dlg.Title = "Export Sky setup to engine-readable archive file (refer to API function VisSky_cl::ReadFromStream)";

      if (dlg.ShowDialog(this) != DialogResult.OK)
        return;

      string filename = dlg.FileName;
      dlg.Dispose();
      try
      {
        if (!_skyConfig.ExportSky(filename))
          throw new Exception("Native serialization failed");
      }
      catch (Exception ex)
      {
        EditorManager.ShowMessageBox("Failed to export sky setup to file:\n\n" + ex.Message, "Error asving file", MessageBoxButtons.OK, MessageBoxIcon.Error);
      }
    }

    private void button_OK_Click(object sender, EventArgs e)
    {
      if (_skyConfig == null)
        return;

      if (_skyConfig.VisibleBitmask == CSharpFramework.PropertyEditors.FlagsInt32_e.None)
        EditorManager.ShowMessageBox("The Visibility Bitmask of the Sky is set to 'NONE'. This means your sky will never be visible.", "Sky Configuration", MessageBoxButtons.OK, MessageBoxIcon.Information);
    }

  }

  #region Sky Type Editor

  /// <summary>
  /// Sky config editor
  /// </summary>
  public class SkyConfigEditor : UITypeEditor
  {
    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="context"></param>
    /// <returns></returns>
    [System.Security.Permissions.PermissionSet(System.Security.Permissions.SecurityAction.Demand, Name="FullTrust")]
    public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
    {
      if (context == null || context.Instance == null)
        return UITypeEditorEditStyle.None;
      if (SceneSkyConfig==null)
        return UITypeEditorEditStyle.None;
      return UITypeEditorEditStyle.Modal;
    }

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <param name="context"></param>
    /// <param name="provider"></param>
    /// <param name="value"></param>
    /// <returns></returns>
    [RefreshProperties(RefreshProperties.All)]
    [System.Security.Permissions.PermissionSet(System.Security.Permissions.SecurityAction.Demand, Name="FullTrust")]
    public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
    {
      SkyConfigDlg dlg = new SkyConfigDlg();
      SkyConfig oldsky = SceneSkyConfig;
      oldsky.Active = false; // disable this config

      dlg.SkyConfig = oldsky;
      if (dlg.ShowDialog()==DialogResult.OK)
      {
        EditorManager.Actions.Add(new SkyConfigChangedAction(SceneV3DLayer,dlg.SkyConfig));
        return dlg.SkyConfig;
      } 

       //restore old sky again
      dlg.SkyConfig.Active = false;
      oldsky.Active = true;
      oldsky.Update();

      return oldsky;
    }


    /// <summary>
    /// Helper function to access the scene's v3d layer
    /// </summary>
    public V3DLayer SceneV3DLayer
    {
      get 
      {
        if (EditorManager.Scene==null)
          return null;
        if (EditorManager.Scene.Layers.Count<1)
          return null;

        //Fix: 14864
        //Find the V3D layer
        foreach (Layer layer in EditorManager.Scene.Layers)
        {
          V3DLayer v3d = layer as V3DLayer;
          if (v3d!=null)
            return v3d;
        }
        return null;
      }
    }

    /// <summary>
    /// Helper function to access the global instance of the sky config
    /// </summary>
    public SkyConfig SceneSkyConfig
    {
      get 
      {
        V3DLayer layer = SceneV3DLayer;
        if (layer==null)
          return null;
        return layer.SkyConfig;
      }
    }




  }

  #endregion

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
