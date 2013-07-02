/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using CSharpFramework;
using CSharpFramework.Actions;
using Editor.Actions;
using CSharpFramework.Shapes;
using Editor.Shapes;
using CSharpFramework.Clipboard;
using CSharpFramework.View;
using CSharpFramework.Docking;
using CSharpFramework.Math;
using CSharpFramework.Scene;
using CSharpFramework.Dialogs;
using System.IO;
using ManagedFramework;
using System.Collections.Generic;
using System.Globalization;
using System.Collections.Specialized;

namespace Editor
{
	/// <summary>
	/// ShapeTreePanel. Contains the tree view for shapes and layers/zones.
	/// </summary>
	public class ShapeTreePanel : DockableForm
	{
    #region Members

    private CSharpFramework.ShapeTreeView shapeTreeView;
    private System.Windows.Forms.Splitter splitter1;
    private System.ComponentModel.IContainer components;
    private ContextMenuStrip contextMenuStrip_Layer;
    private ToolStripMenuItem lockLayerToolStripMenuItem;
    private ToolStripMenuItem unlockLayerToolStripMenuItem;
    private ToolStripMenuItem updateLockStatusToolStripMenuItem;
    private ToolStripSeparator toolStripMenuItem1;
    private ToolStripMenuItem deleteToolStripMenuItem;
    private ToolStripMenuItem renameToolStripMenuItem;
    private ContextMenuStrip contextMenuStrip_Shapes;
    private ToolStripMenuItem setShapeActiveToolStripMenuItem;
    private ToolStripMenuItem createToolStripMenuItem;
    private ToolStripMenuItem insertHereToolStripMenuItem;
    private ToolStripSeparator toolStripMenuItem3;
    private ToolStripMenuItem cutShapeToolStripMenuItem;
    private ToolStripMenuItem copyShapeToolStripMenuItem;
    private ToolStripMenuItem pasteShapeToolStripMenuItem;
    private ToolStripMenuItem deleteShapeToolStripMenuItem;
    private ToolStripMenuItem renameShapeToolStripMenuItem;
    private ToolStripMenuItem relevantOperationsToolStripMenuItem;
    private ToolStripMenuItem goHereToolStripMenuItem;
    private ToolStripSeparator toolStripMenuItem6;
    private ToolStripMenuItem dropToFloorToolStripMenuItem;
    private ToolStripMenuItem jumpToToolStripMenuItem;
    private ToolStripMenuItem pivotToolStripMenuItem;
    private ToolStripMenuItem boundingBoxToolStripMenuItem;
    private ToolStripMenuItem bottomCenterToolStripMenuItem;
    private ToolStripMenuItem sortShapesToolStripMenuItem;
    private ToolStripMenuItem alphabeticallyUpToolStripMenuItem;
    private ToolStripMenuItem alphabeticallyDownToolStripMenuItem;
    private ToolStripSeparator toolStripMenuItem7;
    private ToolStripMenuItem createPrefabToolStripMenuItem;
    private ToolStrip toolStrip1;
    private ToolStripButton toolStripButton_AddLayer;
    private ToolStripButton toolStripButton_DeleteLayer;
    private ToolStripButton toolStripButton_LayerMoveUp;
    private ToolStripButton toolStripButton_LayerMoveDown;
    private ToolStripButton toolStripButton_LockUpdate;
    private ToolStripButton toolStripButton_FileRefresh;
    private ToolStripSeparator toolStripSeparator1;
    private ToolStripButton toolStripButton_SortAZUp;
    private ToolStripButton toolStripButton_SortAZDown;
    private CSharpFramework.Controls.LayerTreeView treeView_Layers;
    private ContextMenuStrip contextMenuStrip_Zones;
    private ToolStripSeparator toolStripSeparator2;
    private ToolStripMenuItem toolStripMenuItem_DeleteZone;
    private ToolStripMenuItem toolStripMenuItem_RenameZone;
    private ToolStripMenuItem loadUnloadToolStripMenuItem;
    private ToolStripMenuItem relightToolStripMenuItem;
    private ToolStripMenuItem childNodesToolStripMenuItem;
    private ToolStripMenuItem selectChildNodesToolStripMenuItem;
    private ToolStripMenuItem selectRecursivelyToolStripMenuItem;
    private ToolStripMenuItem lockShapesToolStripMenuItem;
    private ToolStripMenuItem unfreezeAllShapesToolStripMenuItem;
    private ToolStripMenuItem showZoneAssetsToolStripMenuItem;
    private ToolStripMenuItem showLayerAssetsToolStripMenuItem;
    private ToolStripSeparator toolStripMenuItem_LastSeparator;
    private ToolStripMenuItem renameFileToLayerNameToolStripMenuItem;
    private ToolStripMenuItem moveToLayerToolStripMenuItem;
    private ToolStripMenuItem layersGoHereToolStripMenuItem;
    private ToolStripSeparator toolStripMenuItem8;
    private ToolStripMenuItem directionFloorToolStripMenuItem;
    private ToolStripMenuItem directionCeilingToolStripMenuItem;
    private ToolStripMenuItem directionPosXToolStripMenuItem;
    private ToolStripMenuItem directionNegXToolStripMenuItem;
    private ToolStripMenuItem directionPosYToolStripMenuItem;
    private ToolStripMenuItem directionNegYToolStripMenuItem;
    private ToolStripMenuItem sortShapesIntoZonesToolStripMenuItem;
    private ToolStripMenuItem moveShapesToolStripMenuItem;
    private ToolStripButton toolStripButton_AddZone;
    private ToolStrip toolStrip_Shapes;
    private ToolStripButton toolStripButton_AddGroup;
    private ToolStripButton toolStripButton_DeleteShape;
    private ToolStripButton toolStripButton_ShapeMoveDown;
    private ToolStripButton toolStripButton_ShapeMoveUp;
    private ToolStripButton toolStripButton_SortShapesAZUp;
    private ToolStripButton toolStripButton_SortShapesAZDown;
    private ToolStripSplitButton toolStripSplitButton_MoveToLayer;
    private ToolStripMenuItem layersGoHereToolStripMenuItem1;
    private ToolStripSeparator toolStripSeparator5;
    private ToolStripSeparator toolStripSeparator3;
    private ToolStripSeparator toolStripSeparator4;
    private ToolStripSplitButton toolStripSplitButton_ToolsOptions;
    private ToolStripMenuItem toolStripSplitButton_AutomaticFocus;
    private ToolStripMenuItem toolStripSplitButton_PropertiesOnSelection;
    private ToolStripButton toolStripButton_ImportReference;
    private TableLayoutPanel panel_UpdateLayer;
    private PictureBox pictureBox_UpdateIcon;
    private FlowLayoutPanel flowLayoutPanel1;
    private Button button_UpdateIgnore;
    private Button button_LayerUpdate;
    private Label label1;
    private ToolStripMenuItem lockStatusToolStripMenuItem;

    /// <summary>
    /// Background color of the active shape
    /// </summary>
    private static Color ACTIVESHAPE_BKCOLOR = Color.Yellow;

    #endregion Members
    
    #region Constructor/Destructor
    
    /// <summary>
    /// Constructor
    /// </summary>
    public ShapeTreePanel(DockingContainer container) : base(container)
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

      //check if in design mode. visual studio 2010 can crash when you try to call any static functions or members from EditorManager when in design mode
      if (System.ComponentModel.LicenseManager.UsageMode != System.ComponentModel.LicenseUsageMode.Designtime)
      {
        // set specific image lists
        shapeTreeView.ImageList = EditorManager.GUI.ShapeTreeImages.ImageList;

        EditorManager.SceneChanged += new SceneChangedEventHandler(shapeTreeView.OnSceneChanged);
        EditorManager.SceneChanged += new SceneChangedEventHandler(EditorManager_SceneChanged);

        EditorScene.ShapeChanged += new ShapeChangedEventHandler(shapeTreeView.OnShapeChanged);
        EditorScene.LayerChanged += new LayerChangedEventHandler(shapeTreeView.OnLayerChanged);
        EditorScene.LayerChanged += new LayerChangedEventHandler(EditorScene_LayerChanged);

        //EditorScene.LayerChanged += new LayerChangedEventHandler(layerListView.OnLayerChanged);
        //EditorManager.SceneChanged += new SceneChangedEventHandler(layerListView.OnSceneChanged);

        CSharpFramework.Layer.ActiveShapeChanged += new CSharpFramework.Layer.ActiveShapeChangedEventHandler(Layer_ActiveShapeChanged);
        shapeTreeView.NodeStateChanged += new CSharpFramework.TreeViewMS.NodeStateChangedEventHandlers(shapeTreeView_NodeStateChanged);

        // make these two context menus available in the control
        treeView_Layers.ContextMenu_Layers = this.contextMenuStrip_Layer;
        treeView_Layers.ContextMenu_Zones = this.contextMenuStrip_Zones;
        treeView_Layers.OnSelectionChanged += new EventHandler(treeView_Layers_OnSelectionChanged);
        UpdateLayerToolbar();
        UpdatedLayerInfoPanelVisible = false;

        // associate drop-to-floor directions
        directionPosXToolStripMenuItem.Tag = Vector3F.XAxis;
        directionNegXToolStripMenuItem.Tag = -Vector3F.XAxis;
        directionPosYToolStripMenuItem.Tag = Vector3F.YAxis;
        directionNegYToolStripMenuItem.Tag = -Vector3F.YAxis;
        directionCeilingToolStripMenuItem.Tag = Vector3F.ZAxis;
        directionFloorToolStripMenuItem.Tag = -Vector3F.ZAxis;

      }

    }


		/// <summary> 
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
      EditorManager.SceneChanged -= new SceneChangedEventHandler(shapeTreeView.OnSceneChanged);
      EditorManager.SceneChanged -= new SceneChangedEventHandler(EditorManager_SceneChanged);

      EditorScene.ShapeChanged -= new ShapeChangedEventHandler(shapeTreeView.OnShapeChanged);
      EditorScene.LayerChanged -= new LayerChangedEventHandler(shapeTreeView.OnLayerChanged);
      EditorScene.LayerChanged -= new LayerChangedEventHandler(EditorScene_LayerChanged);

      //EditorScene.LayerChanged -= new LayerChangedEventHandler(layerListView.OnLayerChanged);
      //EditorManager.SceneChanged -= new SceneChangedEventHandler(layerListView.OnSceneChanged);

      CSharpFramework.Layer.ActiveShapeChanged -= new CSharpFramework.Layer.ActiveShapeChangedEventHandler(Layer_ActiveShapeChanged);

			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

    /// <summary>
    /// Private constructor. Necessary to get this form properly shown in the designer when deriving from it.
    /// See ticket #1067 and http://stackoverflow.com/questions/1216940/
    /// </summary>
    private ShapeTreePanel()
      : base(null)
    {
      InitializeComponent();
    }

    #endregion Constructor/Destructor

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      this.components = new System.ComponentModel.Container();
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ShapeTreePanel));
      this.shapeTreeView = new CSharpFramework.ShapeTreeView();
      this.contextMenuStrip_Shapes = new System.Windows.Forms.ContextMenuStrip(this.components);
      this.relevantOperationsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.goHereToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem3 = new System.Windows.Forms.ToolStripSeparator();
      this.cutShapeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.copyShapeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.pasteShapeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.deleteShapeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.renameShapeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem6 = new System.Windows.Forms.ToolStripSeparator();
      this.createToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.insertHereToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.dropToFloorToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.pivotToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.boundingBoxToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.bottomCenterToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem8 = new System.Windows.Forms.ToolStripSeparator();
      this.directionFloorToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.directionCeilingToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.directionPosXToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.directionNegXToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.directionPosYToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.directionNegYToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.sortShapesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.alphabeticallyUpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.alphabeticallyDownToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.childNodesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.selectChildNodesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.selectRecursivelyToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.moveToLayerToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.layersGoHereToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem7 = new System.Windows.Forms.ToolStripSeparator();
      this.setShapeActiveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.jumpToToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.lockShapesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.createPrefabToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.splitter1 = new System.Windows.Forms.Splitter();
      this.contextMenuStrip_Layer = new System.Windows.Forms.ContextMenuStrip(this.components);
      this.lockStatusToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.lockLayerToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.unlockLayerToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.updateLockStatusToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.unfreezeAllShapesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.sortShapesIntoZonesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.showLayerAssetsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripSeparator();
      this.deleteToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.renameToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem_LastSeparator = new System.Windows.Forms.ToolStripSeparator();
      this.renameFileToLayerNameToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStrip1 = new System.Windows.Forms.ToolStrip();
      this.toolStripButton_AddLayer = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_AddZone = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_ImportReference = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_DeleteLayer = new System.Windows.Forms.ToolStripButton();
      this.toolStripSeparator5 = new System.Windows.Forms.ToolStripSeparator();
      this.toolStripButton_LayerMoveDown = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_LayerMoveUp = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_SortAZUp = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_SortAZDown = new System.Windows.Forms.ToolStripButton();
      this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
      this.toolStripButton_LockUpdate = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_FileRefresh = new System.Windows.Forms.ToolStripButton();
      this.treeView_Layers = new CSharpFramework.Controls.LayerTreeView();
      this.contextMenuStrip_Zones = new System.Windows.Forms.ContextMenuStrip(this.components);
      this.moveShapesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.loadUnloadToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.relightToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.showZoneAssetsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
      this.toolStripMenuItem_DeleteZone = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem_RenameZone = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStrip_Shapes = new System.Windows.Forms.ToolStrip();
      this.toolStripButton_AddGroup = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_DeleteShape = new System.Windows.Forms.ToolStripButton();
      this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
      this.toolStripButton_ShapeMoveDown = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_ShapeMoveUp = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_SortShapesAZUp = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_SortShapesAZDown = new System.Windows.Forms.ToolStripButton();
      this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
      this.toolStripSplitButton_MoveToLayer = new System.Windows.Forms.ToolStripSplitButton();
      this.layersGoHereToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripSplitButton_ToolsOptions = new System.Windows.Forms.ToolStripSplitButton();
      this.toolStripSplitButton_AutomaticFocus = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripSplitButton_PropertiesOnSelection = new System.Windows.Forms.ToolStripMenuItem();
      this.panel_UpdateLayer = new System.Windows.Forms.TableLayoutPanel();
      this.pictureBox_UpdateIcon = new System.Windows.Forms.PictureBox();
      this.flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
      this.button_UpdateIgnore = new System.Windows.Forms.Button();
      this.button_LayerUpdate = new System.Windows.Forms.Button();
      this.label1 = new System.Windows.Forms.Label();
      this.contextMenuStrip_Shapes.SuspendLayout();
      this.contextMenuStrip_Layer.SuspendLayout();
      this.toolStrip1.SuspendLayout();
      this.contextMenuStrip_Zones.SuspendLayout();
      this.toolStrip_Shapes.SuspendLayout();
      this.panel_UpdateLayer.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.pictureBox_UpdateIcon)).BeginInit();
      this.flowLayoutPanel1.SuspendLayout();
      this.SuspendLayout();
      // 
      // shapeTreeView
      // 
      this.shapeTreeView.AllowDrop = true;
      this.shapeTreeView.AllowMultiSelect = true;
      this.shapeTreeView.ContextMenuStrip = this.contextMenuStrip_Shapes;
      this.shapeTreeView.Dock = System.Windows.Forms.DockStyle.Top;
      this.shapeTreeView.HideSelection = false;
      this.shapeTreeView.LabelEdit = true;
      this.shapeTreeView.Location = new System.Drawing.Point(0, 25);
      this.shapeTreeView.Name = "shapeTreeView";
      this.shapeTreeView.SelectedNodes = ((System.Collections.ArrayList)(resources.GetObject("shapeTreeView.SelectedNodes")));
      this.shapeTreeView.Size = new System.Drawing.Size(340, 257);
      this.shapeTreeView.TabIndex = 1;
      // 
      // contextMenuStrip_Shapes
      // 
      this.contextMenuStrip_Shapes.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.relevantOperationsToolStripMenuItem,
            this.toolStripMenuItem3,
            this.cutShapeToolStripMenuItem,
            this.copyShapeToolStripMenuItem,
            this.pasteShapeToolStripMenuItem,
            this.deleteShapeToolStripMenuItem,
            this.renameShapeToolStripMenuItem,
            this.toolStripMenuItem6,
            this.createToolStripMenuItem,
            this.dropToFloorToolStripMenuItem,
            this.sortShapesToolStripMenuItem,
            this.childNodesToolStripMenuItem,
            this.moveToLayerToolStripMenuItem,
            this.toolStripMenuItem7,
            this.setShapeActiveToolStripMenuItem,
            this.jumpToToolStripMenuItem,
            this.lockShapesToolStripMenuItem,
            this.createPrefabToolStripMenuItem});
      this.contextMenuStrip_Shapes.Name = "contextMenuStrip_Shapes";
      this.contextMenuStrip_Shapes.Size = new System.Drawing.Size(181, 352);
      this.contextMenuStrip_Shapes.Opening += new System.ComponentModel.CancelEventHandler(this.contextMenuStrip_Shapes_Opening);
      // 
      // relevantOperationsToolStripMenuItem
      // 
      this.relevantOperationsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.goHereToolStripMenuItem});
      this.relevantOperationsToolStripMenuItem.Image = global::Editor.Properties.Resources.wrench;
      this.relevantOperationsToolStripMenuItem.Name = "relevantOperationsToolStripMenuItem";
      this.relevantOperationsToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.relevantOperationsToolStripMenuItem.Text = "Relevant Operations";
      this.relevantOperationsToolStripMenuItem.ToolTipText = "Performs relevant operations on the shapes";
      // 
      // goHereToolStripMenuItem
      // 
      this.goHereToolStripMenuItem.Name = "goHereToolStripMenuItem";
      this.goHereToolStripMenuItem.Size = new System.Drawing.Size(130, 22);
      this.goHereToolStripMenuItem.Text = "<go here>";
      // 
      // toolStripMenuItem3
      // 
      this.toolStripMenuItem3.Name = "toolStripMenuItem3";
      this.toolStripMenuItem3.Size = new System.Drawing.Size(177, 6);
      // 
      // cutShapeToolStripMenuItem
      // 
      this.cutShapeToolStripMenuItem.Image = global::Editor.Properties.Resources.cut;
      this.cutShapeToolStripMenuItem.Name = "cutShapeToolStripMenuItem";
      this.cutShapeToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.cutShapeToolStripMenuItem.Text = "Cut";
      this.cutShapeToolStripMenuItem.ToolTipText = "Cuts the selected shape(s)";
      this.cutShapeToolStripMenuItem.Click += new System.EventHandler(this.cutShapeToolStripMenuItem_Click);
      // 
      // copyShapeToolStripMenuItem
      // 
      this.copyShapeToolStripMenuItem.Image = global::Editor.Properties.Resources.copy;
      this.copyShapeToolStripMenuItem.Name = "copyShapeToolStripMenuItem";
      this.copyShapeToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.copyShapeToolStripMenuItem.Text = "Copy";
      this.copyShapeToolStripMenuItem.ToolTipText = "Copies the selected shape(s)";
      this.copyShapeToolStripMenuItem.Click += new System.EventHandler(this.copyShapeToolStripMenuItem_Click);
      // 
      // pasteShapeToolStripMenuItem
      // 
      this.pasteShapeToolStripMenuItem.Image = global::Editor.Properties.Resources.paste;
      this.pasteShapeToolStripMenuItem.Name = "pasteShapeToolStripMenuItem";
      this.pasteShapeToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.pasteShapeToolStripMenuItem.Text = "Paste";
      this.pasteShapeToolStripMenuItem.ToolTipText = "Pastes the clipboard content";
      this.pasteShapeToolStripMenuItem.Click += new System.EventHandler(this.pasteShapeToolStripMenuItem_Click);
      // 
      // deleteShapeToolStripMenuItem
      // 
      this.deleteShapeToolStripMenuItem.Image = global::Editor.Properties.Resources.delete2;
      this.deleteShapeToolStripMenuItem.Name = "deleteShapeToolStripMenuItem";
      this.deleteShapeToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.deleteShapeToolStripMenuItem.Text = "Delete";
      this.deleteShapeToolStripMenuItem.ToolTipText = "Deletes the selected shape(s)";
      this.deleteShapeToolStripMenuItem.Click += new System.EventHandler(this.deleteShapeToolStripMenuItem_Click);
      // 
      // renameShapeToolStripMenuItem
      // 
      this.renameShapeToolStripMenuItem.Image = global::Editor.Properties.Resources.rename;
      this.renameShapeToolStripMenuItem.Name = "renameShapeToolStripMenuItem";
      this.renameShapeToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.renameShapeToolStripMenuItem.Text = "Rename";
      this.renameShapeToolStripMenuItem.ToolTipText = "Renames the selected shape";
      this.renameShapeToolStripMenuItem.Click += new System.EventHandler(this.renameShapeToolStripMenuItem_Click);
      // 
      // toolStripMenuItem6
      // 
      this.toolStripMenuItem6.Name = "toolStripMenuItem6";
      this.toolStripMenuItem6.Size = new System.Drawing.Size(177, 6);
      // 
      // createToolStripMenuItem
      // 
      this.createToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.insertHereToolStripMenuItem});
      this.createToolStripMenuItem.Image = global::Editor.Properties.Resources.pawn_new;
      this.createToolStripMenuItem.Name = "createToolStripMenuItem";
      this.createToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.createToolStripMenuItem.Text = "Create";
      this.createToolStripMenuItem.ToolTipText = "All creator plugins";
      // 
      // insertHereToolStripMenuItem
      // 
      this.insertHereToolStripMenuItem.Name = "insertHereToolStripMenuItem";
      this.insertHereToolStripMenuItem.Size = new System.Drawing.Size(130, 22);
      this.insertHereToolStripMenuItem.Text = "<go here>";
      // 
      // dropToFloorToolStripMenuItem
      // 
      this.dropToFloorToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.pivotToolStripMenuItem,
            this.boundingBoxToolStripMenuItem,
            this.bottomCenterToolStripMenuItem,
            this.toolStripMenuItem8,
            this.directionFloorToolStripMenuItem,
            this.directionCeilingToolStripMenuItem,
            this.directionPosXToolStripMenuItem,
            this.directionNegXToolStripMenuItem,
            this.directionPosYToolStripMenuItem,
            this.directionNegYToolStripMenuItem});
      this.dropToFloorToolStripMenuItem.Image = global::Editor.Properties.Resources.arrow_down_blue;
      this.dropToFloorToolStripMenuItem.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.dropToFloorToolStripMenuItem.Name = "dropToFloorToolStripMenuItem";
      this.dropToFloorToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.dropToFloorToolStripMenuItem.Text = "Drop to Floor";
      this.dropToFloorToolStripMenuItem.ToolTipText = "Drops selected shapes until they collide with the floor geometry";
      this.dropToFloorToolStripMenuItem.DropDownOpening += new System.EventHandler(this.dropToFloorToolStripMenuItem_DropDownOpening);
      // 
      // pivotToolStripMenuItem
      // 
      this.pivotToolStripMenuItem.Name = "pivotToolStripMenuItem";
      this.pivotToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
      this.pivotToolStripMenuItem.Text = "Pivot";
      this.pivotToolStripMenuItem.ToolTipText = "Takes the pivot point of the shape";
      this.pivotToolStripMenuItem.Click += new System.EventHandler(this.pivotToolStripMenuItem_Click);
      // 
      // boundingBoxToolStripMenuItem
      // 
      this.boundingBoxToolStripMenuItem.Name = "boundingBoxToolStripMenuItem";
      this.boundingBoxToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
      this.boundingBoxToolStripMenuItem.Text = "Bounding Box";
      this.boundingBoxToolStripMenuItem.ToolTipText = "Collides the full bounding box with the geometry";
      this.boundingBoxToolStripMenuItem.Click += new System.EventHandler(this.boundingBoxToolStripMenuItem_Click);
      // 
      // bottomCenterToolStripMenuItem
      // 
      this.bottomCenterToolStripMenuItem.Name = "bottomCenterToolStripMenuItem";
      this.bottomCenterToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
      this.bottomCenterToolStripMenuItem.Text = "Bottom Center";
      this.bottomCenterToolStripMenuItem.ToolTipText = "Takes the bottom center of the bounding box to collide";
      this.bottomCenterToolStripMenuItem.Click += new System.EventHandler(this.bottomCenterToolStripMenuItem_Click);
      // 
      // toolStripMenuItem8
      // 
      this.toolStripMenuItem8.Name = "toolStripMenuItem8";
      this.toolStripMenuItem8.Size = new System.Drawing.Size(165, 6);
      // 
      // directionFloorToolStripMenuItem
      // 
      this.directionFloorToolStripMenuItem.Name = "directionFloorToolStripMenuItem";
      this.directionFloorToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
      this.directionFloorToolStripMenuItem.Text = "Direction : Floor";
      this.directionFloorToolStripMenuItem.Click += new System.EventHandler(this.directionFloorToolStripMenuItem_Click);
      // 
      // directionCeilingToolStripMenuItem
      // 
      this.directionCeilingToolStripMenuItem.Name = "directionCeilingToolStripMenuItem";
      this.directionCeilingToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
      this.directionCeilingToolStripMenuItem.Text = "Direction : Ceiling";
      this.directionCeilingToolStripMenuItem.Click += new System.EventHandler(this.directionFloorToolStripMenuItem_Click);
      // 
      // directionPosXToolStripMenuItem
      // 
      this.directionPosXToolStripMenuItem.Name = "directionPosXToolStripMenuItem";
      this.directionPosXToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
      this.directionPosXToolStripMenuItem.Text = "Direction : Pos X";
      this.directionPosXToolStripMenuItem.Click += new System.EventHandler(this.directionFloorToolStripMenuItem_Click);
      // 
      // directionNegXToolStripMenuItem
      // 
      this.directionNegXToolStripMenuItem.Name = "directionNegXToolStripMenuItem";
      this.directionNegXToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
      this.directionNegXToolStripMenuItem.Text = "Direction : Neg X";
      this.directionNegXToolStripMenuItem.Click += new System.EventHandler(this.directionFloorToolStripMenuItem_Click);
      // 
      // directionPosYToolStripMenuItem
      // 
      this.directionPosYToolStripMenuItem.Name = "directionPosYToolStripMenuItem";
      this.directionPosYToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
      this.directionPosYToolStripMenuItem.Text = "Direction : Pos Y";
      this.directionPosYToolStripMenuItem.Click += new System.EventHandler(this.directionFloorToolStripMenuItem_Click);
      // 
      // directionNegYToolStripMenuItem
      // 
      this.directionNegYToolStripMenuItem.Name = "directionNegYToolStripMenuItem";
      this.directionNegYToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
      this.directionNegYToolStripMenuItem.Text = "Direction : Neg Y";
      this.directionNegYToolStripMenuItem.Click += new System.EventHandler(this.directionFloorToolStripMenuItem_Click);
      // 
      // sortShapesToolStripMenuItem
      // 
      this.sortShapesToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.alphabeticallyUpToolStripMenuItem,
            this.alphabeticallyDownToolStripMenuItem});
      this.sortShapesToolStripMenuItem.Image = global::Editor.Properties.Resources.sort_descending;
      this.sortShapesToolStripMenuItem.Name = "sortShapesToolStripMenuItem";
      this.sortShapesToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.sortShapesToolStripMenuItem.Text = "Sort Shapes";
      this.sortShapesToolStripMenuItem.ToolTipText = "Sorts the children of the selected shape (or root)";
      // 
      // alphabeticallyUpToolStripMenuItem
      // 
      this.alphabeticallyUpToolStripMenuItem.Image = global::Editor.Properties.Resources.sort_az_descending;
      this.alphabeticallyUpToolStripMenuItem.Name = "alphabeticallyUpToolStripMenuItem";
      this.alphabeticallyUpToolStripMenuItem.Size = new System.Drawing.Size(183, 22);
      this.alphabeticallyUpToolStripMenuItem.Text = "Alphabetically Up";
      this.alphabeticallyUpToolStripMenuItem.ToolTipText = "Sorts the shapes by shape name (A-Z)";
      this.alphabeticallyUpToolStripMenuItem.Click += new System.EventHandler(this.alphabeticallyUpToolStripMenuItem_Click);
      // 
      // alphabeticallyDownToolStripMenuItem
      // 
      this.alphabeticallyDownToolStripMenuItem.Image = global::Editor.Properties.Resources.sort_az_ascending;
      this.alphabeticallyDownToolStripMenuItem.Name = "alphabeticallyDownToolStripMenuItem";
      this.alphabeticallyDownToolStripMenuItem.Size = new System.Drawing.Size(183, 22);
      this.alphabeticallyDownToolStripMenuItem.Text = "Alphabetically Down";
      this.alphabeticallyDownToolStripMenuItem.ToolTipText = "Sorts the shapes by shape name (Z-A)";
      this.alphabeticallyDownToolStripMenuItem.Click += new System.EventHandler(this.alphabeticallyDownToolStripMenuItem_Click);
      // 
      // childNodesToolStripMenuItem
      // 
      this.childNodesToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.selectChildNodesToolStripMenuItem,
            this.selectRecursivelyToolStripMenuItem});
      this.childNodesToolStripMenuItem.Image = global::Editor.Properties.Resources.child_nodes_icon;
      this.childNodesToolStripMenuItem.Name = "childNodesToolStripMenuItem";
      this.childNodesToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.childNodesToolStripMenuItem.Text = "Child Nodes";
      this.childNodesToolStripMenuItem.ToolTipText = "Select child nodes of this shape";
      // 
      // selectChildNodesToolStripMenuItem
      // 
      this.selectChildNodesToolStripMenuItem.Image = global::Editor.Properties.Resources.child_nodes;
      this.selectChildNodesToolStripMenuItem.Name = "selectChildNodesToolStripMenuItem";
      this.selectChildNodesToolStripMenuItem.Size = new System.Drawing.Size(173, 22);
      this.selectChildNodesToolStripMenuItem.Text = "Select Child Nodes";
      this.selectChildNodesToolStripMenuItem.ToolTipText = "Select the child nodes of the currently selected shapes";
      this.selectChildNodesToolStripMenuItem.Click += new System.EventHandler(this.selectChildNodesToolStripMenuItem_Click);
      // 
      // selectRecursivelyToolStripMenuItem
      // 
      this.selectRecursivelyToolStripMenuItem.Image = global::Editor.Properties.Resources.child_nodes_rec;
      this.selectRecursivelyToolStripMenuItem.Name = "selectRecursivelyToolStripMenuItem";
      this.selectRecursivelyToolStripMenuItem.Size = new System.Drawing.Size(173, 22);
      this.selectRecursivelyToolStripMenuItem.Text = "Select Recursively";
      this.selectRecursivelyToolStripMenuItem.ToolTipText = "Select all the child nodes of the currently selected shapes recursively";
      this.selectRecursivelyToolStripMenuItem.Click += new System.EventHandler(this.selectRecursivelyToolStripMenuItem_Click);
      // 
      // moveToLayerToolStripMenuItem
      // 
      this.moveToLayerToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.layersGoHereToolStripMenuItem});
      this.moveToLayerToolStripMenuItem.Image = global::Editor.Properties.Resources.breakpoint_into;
      this.moveToLayerToolStripMenuItem.Name = "moveToLayerToolStripMenuItem";
      this.moveToLayerToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.moveToLayerToolStripMenuItem.Text = "Move to Layer";
      // 
      // layersGoHereToolStripMenuItem
      // 
      this.layersGoHereToolStripMenuItem.Name = "layersGoHereToolStripMenuItem";
      this.layersGoHereToolStripMenuItem.Size = new System.Drawing.Size(166, 22);
      this.layersGoHereToolStripMenuItem.Text = "<Layers go here>";
      // 
      // toolStripMenuItem7
      // 
      this.toolStripMenuItem7.Name = "toolStripMenuItem7";
      this.toolStripMenuItem7.Size = new System.Drawing.Size(177, 6);
      // 
      // setShapeActiveToolStripMenuItem
      // 
      this.setShapeActiveToolStripMenuItem.Image = global::Editor.Properties.Resources.lightbulb_on;
      this.setShapeActiveToolStripMenuItem.Name = "setShapeActiveToolStripMenuItem";
      this.setShapeActiveToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.setShapeActiveToolStripMenuItem.Text = "Set Shape Active";
      this.setShapeActiveToolStripMenuItem.ToolTipText = "Marks the shape as active so added shapes will use it as parent";
      this.setShapeActiveToolStripMenuItem.Click += new System.EventHandler(this.setShapeActiveToolStripMenuItem_Click);
      // 
      // jumpToToolStripMenuItem
      // 
      this.jumpToToolStripMenuItem.Image = global::Editor.Properties.Resources.toolbar_camera_small;
      this.jumpToToolStripMenuItem.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.jumpToToolStripMenuItem.Name = "jumpToToolStripMenuItem";
      this.jumpToToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.jumpToToolStripMenuItem.Text = "Jump to Object";
      this.jumpToToolStripMenuItem.ToolTipText = "Move the camera to the selected shapes(s)";
      this.jumpToToolStripMenuItem.Click += new System.EventHandler(this.jumpToToolStripMenuItem_Click);
      // 
      // lockShapesToolStripMenuItem
      // 
      this.lockShapesToolStripMenuItem.Image = global::Editor.Properties.Resources._lock;
      this.lockShapesToolStripMenuItem.Name = "lockShapesToolStripMenuItem";
      this.lockShapesToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.lockShapesToolStripMenuItem.Text = "Freeze Shapes";
      this.lockShapesToolStripMenuItem.Click += new System.EventHandler(this.lockShapesToolStripMenuItem_Click);
      // 
      // createPrefabToolStripMenuItem
      // 
      this.createPrefabToolStripMenuItem.Image = global::Editor.Properties.Resources.folder_refresh;
      this.createPrefabToolStripMenuItem.Name = "createPrefabToolStripMenuItem";
      this.createPrefabToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.createPrefabToolStripMenuItem.Text = "Create Prefab";
      this.createPrefabToolStripMenuItem.Click += new System.EventHandler(this.createPrefabToolStripMenuItem_Click);
      // 
      // splitter1
      // 
      this.splitter1.Dock = System.Windows.Forms.DockStyle.Top;
      this.splitter1.Location = new System.Drawing.Point(0, 282);
      this.splitter1.Name = "splitter1";
      this.splitter1.Size = new System.Drawing.Size(340, 3);
      this.splitter1.TabIndex = 2;
      this.splitter1.TabStop = false;
      // 
      // contextMenuStrip_Layer
      // 
      this.contextMenuStrip_Layer.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.lockStatusToolStripMenuItem,
            this.lockLayerToolStripMenuItem,
            this.unlockLayerToolStripMenuItem,
            this.updateLockStatusToolStripMenuItem,
            this.unfreezeAllShapesToolStripMenuItem,
            this.sortShapesIntoZonesToolStripMenuItem,
            this.showLayerAssetsToolStripMenuItem,
            this.toolStripMenuItem1,
            this.deleteToolStripMenuItem,
            this.renameToolStripMenuItem,
            this.toolStripMenuItem_LastSeparator,
            this.renameFileToLayerNameToolStripMenuItem});
      this.contextMenuStrip_Layer.Name = "contextMenuStrip_Layer";
      this.contextMenuStrip_Layer.Size = new System.Drawing.Size(215, 258);
      this.contextMenuStrip_Layer.Opening += new System.ComponentModel.CancelEventHandler(this.contextMenuStrip_Layer_Opening);
      // 
      // lockStatusToolStripMenuItem
      // 
      this.lockStatusToolStripMenuItem.BackColor = System.Drawing.SystemColors.ControlLight;
      this.lockStatusToolStripMenuItem.Enabled = false;
      this.lockStatusToolStripMenuItem.Name = "lockStatusToolStripMenuItem";
      this.lockStatusToolStripMenuItem.Size = new System.Drawing.Size(214, 22);
      this.lockStatusToolStripMenuItem.Text = "LockStatus: Unknown";
      // 
      // lockLayerToolStripMenuItem
      // 
      this.lockLayerToolStripMenuItem.Image = global::Editor.Properties.Resources._lock;
      this.lockLayerToolStripMenuItem.Name = "lockLayerToolStripMenuItem";
      this.lockLayerToolStripMenuItem.Size = new System.Drawing.Size(214, 22);
      this.lockLayerToolStripMenuItem.Text = "Lock Layer";
      this.lockLayerToolStripMenuItem.ToolTipText = "Tries to lock the layer so it can be edited";
      this.lockLayerToolStripMenuItem.Click += new System.EventHandler(this.lockLayerToolStripMenuItem_Click);
      // 
      // unlockLayerToolStripMenuItem
      // 
      this.unlockLayerToolStripMenuItem.Image = global::Editor.Properties.Resources.lock_open;
      this.unlockLayerToolStripMenuItem.Name = "unlockLayerToolStripMenuItem";
      this.unlockLayerToolStripMenuItem.Size = new System.Drawing.Size(214, 22);
      this.unlockLayerToolStripMenuItem.Text = "Unlock Layer";
      this.unlockLayerToolStripMenuItem.ToolTipText = "Unlocks the layer so other users can edit it over the network";
      this.unlockLayerToolStripMenuItem.Click += new System.EventHandler(this.unlockLayerToolStripMenuItem_Click);
      // 
      // updateLockStatusToolStripMenuItem
      // 
      this.updateLockStatusToolStripMenuItem.Image = global::Editor.Properties.Resources.lock_view;
      this.updateLockStatusToolStripMenuItem.Name = "updateLockStatusToolStripMenuItem";
      this.updateLockStatusToolStripMenuItem.Size = new System.Drawing.Size(214, 22);
      this.updateLockStatusToolStripMenuItem.Text = "Update Lock Status";
      this.updateLockStatusToolStripMenuItem.ToolTipText = "Refreshes the lock status of the layer";
      this.updateLockStatusToolStripMenuItem.Click += new System.EventHandler(this.updateLockStatusToolStripMenuItem_Click);
      // 
      // unfreezeAllShapesToolStripMenuItem
      // 
      this.unfreezeAllShapesToolStripMenuItem.Image = global::Editor.Properties.Resources.lock_open;
      this.unfreezeAllShapesToolStripMenuItem.Name = "unfreezeAllShapesToolStripMenuItem";
      this.unfreezeAllShapesToolStripMenuItem.Size = new System.Drawing.Size(214, 22);
      this.unfreezeAllShapesToolStripMenuItem.Text = "Un-freeze All Shapes";
      this.unfreezeAllShapesToolStripMenuItem.ToolTipText = "If this layer contains any frozen shape, this operation releases the frozen state" +
    " on all";
      this.unfreezeAllShapesToolStripMenuItem.Click += new System.EventHandler(this.unfreezeAllShapesToolStripMenuItem_Click);
      // 
      // sortShapesIntoZonesToolStripMenuItem
      // 
      this.sortShapesIntoZonesToolStripMenuItem.Image = global::Editor.Properties.Resources.unload_zone;
      this.sortShapesIntoZonesToolStripMenuItem.Name = "sortShapesIntoZonesToolStripMenuItem";
      this.sortShapesIntoZonesToolStripMenuItem.Size = new System.Drawing.Size(214, 22);
      this.sortShapesIntoZonesToolStripMenuItem.Text = "Sort Shapes into Zones";
      this.sortShapesIntoZonesToolStripMenuItem.ToolTipText = "Sort all shapes in this layer into best matching zones";
      this.sortShapesIntoZonesToolStripMenuItem.Click += new System.EventHandler(this.sortShapesIntoZonesToolStripMenuItem_Click);
      // 
      // showLayerAssetsToolStripMenuItem
      // 
      this.showLayerAssetsToolStripMenuItem.Image = global::Editor.Properties.Resources.memory;
      this.showLayerAssetsToolStripMenuItem.Name = "showLayerAssetsToolStripMenuItem";
      this.showLayerAssetsToolStripMenuItem.Size = new System.Drawing.Size(214, 22);
      this.showLayerAssetsToolStripMenuItem.Text = "Show Layer Assets";
      this.showLayerAssetsToolStripMenuItem.Click += new System.EventHandler(this.showLayerAssetsToolStripMenuItem_Click);
      // 
      // toolStripMenuItem1
      // 
      this.toolStripMenuItem1.Name = "toolStripMenuItem1";
      this.toolStripMenuItem1.Size = new System.Drawing.Size(211, 6);
      // 
      // deleteToolStripMenuItem
      // 
      this.deleteToolStripMenuItem.Image = global::Editor.Properties.Resources.delete2;
      this.deleteToolStripMenuItem.Name = "deleteToolStripMenuItem";
      this.deleteToolStripMenuItem.Size = new System.Drawing.Size(214, 22);
      this.deleteToolStripMenuItem.Text = "Delete";
      this.deleteToolStripMenuItem.ToolTipText = "Deletes the selected layer";
      this.deleteToolStripMenuItem.Click += new System.EventHandler(this.deleteToolStripMenuItem_Click);
      // 
      // renameToolStripMenuItem
      // 
      this.renameToolStripMenuItem.Image = global::Editor.Properties.Resources.rename;
      this.renameToolStripMenuItem.Name = "renameToolStripMenuItem";
      this.renameToolStripMenuItem.Size = new System.Drawing.Size(214, 22);
      this.renameToolStripMenuItem.Text = "Rename";
      this.renameToolStripMenuItem.ToolTipText = "Renames the selected layer";
      this.renameToolStripMenuItem.Click += new System.EventHandler(this.renameToolStripMenuItem_Click);
      // 
      // toolStripMenuItem_LastSeparator
      // 
      this.toolStripMenuItem_LastSeparator.Name = "toolStripMenuItem_LastSeparator";
      this.toolStripMenuItem_LastSeparator.Size = new System.Drawing.Size(211, 6);
      // 
      // renameFileToLayerNameToolStripMenuItem
      // 
      this.renameFileToLayerNameToolStripMenuItem.Image = global::Editor.Properties.Resources.save_as;
      this.renameFileToLayerNameToolStripMenuItem.Name = "renameFileToLayerNameToolStripMenuItem";
      this.renameFileToLayerNameToolStripMenuItem.Size = new System.Drawing.Size(214, 22);
      this.renameFileToLayerNameToolStripMenuItem.Text = "Rename file to Layer name";
      this.renameFileToLayerNameToolStripMenuItem.ToolTipText = "Synchronizes the layer\'s filename with the layer\'s name. Tries to rename file(s) " +
    "on disk";
      this.renameFileToLayerNameToolStripMenuItem.Click += new System.EventHandler(this.renameFileToLayerNameToolStripMenuItem_Click);
      // 
      // toolStrip1
      // 
      this.toolStrip1.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
      this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton_AddLayer,
            this.toolStripButton_AddZone,
            this.toolStripButton_ImportReference,
            this.toolStripButton_DeleteLayer,
            this.toolStripSeparator5,
            this.toolStripButton_LayerMoveDown,
            this.toolStripButton_LayerMoveUp,
            this.toolStripButton_SortAZUp,
            this.toolStripButton_SortAZDown,
            this.toolStripSeparator1,
            this.toolStripButton_LockUpdate,
            this.toolStripButton_FileRefresh});
      this.toolStrip1.Location = new System.Drawing.Point(0, 285);
      this.toolStrip1.Name = "toolStrip1";
      this.toolStrip1.Size = new System.Drawing.Size(340, 25);
      this.toolStrip1.TabIndex = 3;
      this.toolStrip1.Text = "toolStrip_Layers";
      // 
      // toolStripButton_AddLayer
      // 
      this.toolStripButton_AddLayer.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_AddLayer.Image = global::Editor.Properties.Resources.breakpoint_new;
      this.toolStripButton_AddLayer.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_AddLayer.Name = "toolStripButton_AddLayer";
      this.toolStripButton_AddLayer.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_AddLayer.Text = "toolStripButton1";
      this.toolStripButton_AddLayer.ToolTipText = "Adds a new layer";
      this.toolStripButton_AddLayer.Click += new System.EventHandler(this.toolStripButton_AddLayer_Click);
      // 
      // toolStripButton_AddZone
      // 
      this.toolStripButton_AddZone.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_AddZone.Image = global::Editor.Properties.Resources.add_zone;
      this.toolStripButton_AddZone.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_AddZone.Name = "toolStripButton_AddZone";
      this.toolStripButton_AddZone.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_AddZone.Text = "toolStripButton1";
      this.toolStripButton_AddZone.ToolTipText = "Adds a new zone";
      this.toolStripButton_AddZone.Click += new System.EventHandler(this.toolStripButton_AddZone_Click);
      // 
      // toolStripButton_ImportReference
      // 
      this.toolStripButton_ImportReference.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_ImportReference.Image = global::Editor.Properties.Resources.toolbar_folder_in;
      this.toolStripButton_ImportReference.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_ImportReference.Name = "toolStripButton_ImportReference";
      this.toolStripButton_ImportReference.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_ImportReference.Text = "toolStripButton1";
      this.toolStripButton_ImportReference.ToolTipText = "Add a layer reference from another scene in this project. These layers cannot be " +
    "edited";
      this.toolStripButton_ImportReference.Click += new System.EventHandler(this.toolStripButton_ImportReference_Click);
      // 
      // toolStripButton_DeleteLayer
      // 
      this.toolStripButton_DeleteLayer.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_DeleteLayer.Image = global::Editor.Properties.Resources.delete2;
      this.toolStripButton_DeleteLayer.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_DeleteLayer.Name = "toolStripButton_DeleteLayer";
      this.toolStripButton_DeleteLayer.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_DeleteLayer.Text = "toolStripButton1";
      this.toolStripButton_DeleteLayer.ToolTipText = "Delete the selected layer or zone";
      this.toolStripButton_DeleteLayer.Click += new System.EventHandler(this.toolStripButton_DeleteLayer_Click);
      // 
      // toolStripSeparator5
      // 
      this.toolStripSeparator5.Name = "toolStripSeparator5";
      this.toolStripSeparator5.Size = new System.Drawing.Size(6, 25);
      // 
      // toolStripButton_LayerMoveDown
      // 
      this.toolStripButton_LayerMoveDown.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_LayerMoveDown.Image = global::Editor.Properties.Resources.arrow_down_blue;
      this.toolStripButton_LayerMoveDown.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_LayerMoveDown.Name = "toolStripButton_LayerMoveDown";
      this.toolStripButton_LayerMoveDown.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_LayerMoveDown.ToolTipText = "Move the layer one item down in the layer list";
      this.toolStripButton_LayerMoveDown.Click += new System.EventHandler(this.toolStripButton_LayerMoveDown_Click);
      // 
      // toolStripButton_LayerMoveUp
      // 
      this.toolStripButton_LayerMoveUp.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_LayerMoveUp.Image = global::Editor.Properties.Resources.arrow_up_blue;
      this.toolStripButton_LayerMoveUp.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_LayerMoveUp.Name = "toolStripButton_LayerMoveUp";
      this.toolStripButton_LayerMoveUp.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_LayerMoveUp.ToolTipText = "Move the layer up one item in the layer list";
      this.toolStripButton_LayerMoveUp.Click += new System.EventHandler(this.toolStripButton_LayerMoveUp_Click);
      // 
      // toolStripButton_SortAZUp
      // 
      this.toolStripButton_SortAZUp.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_SortAZUp.Image = global::Editor.Properties.Resources.sort_az_descending;
      this.toolStripButton_SortAZUp.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_SortAZUp.Name = "toolStripButton_SortAZUp";
      this.toolStripButton_SortAZUp.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_SortAZUp.Text = "toolStripButton1";
      this.toolStripButton_SortAZUp.ToolTipText = "Sort the layers alphabetically by name";
      this.toolStripButton_SortAZUp.Click += new System.EventHandler(this.toolStripButton_SortAZUp_Click);
      // 
      // toolStripButton_SortAZDown
      // 
      this.toolStripButton_SortAZDown.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_SortAZDown.Image = global::Editor.Properties.Resources.sort_az_ascending;
      this.toolStripButton_SortAZDown.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_SortAZDown.Name = "toolStripButton_SortAZDown";
      this.toolStripButton_SortAZDown.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_SortAZDown.Text = "toolStripButton1";
      this.toolStripButton_SortAZDown.ToolTipText = "Sort the layers alphabetically by name";
      this.toolStripButton_SortAZDown.Click += new System.EventHandler(this.toolStripButton_SortAZDown_Click);
      // 
      // toolStripSeparator1
      // 
      this.toolStripSeparator1.Name = "toolStripSeparator1";
      this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
      // 
      // toolStripButton_LockUpdate
      // 
      this.toolStripButton_LockUpdate.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_LockUpdate.Image = global::Editor.Properties.Resources.lock_view;
      this.toolStripButton_LockUpdate.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_LockUpdate.Name = "toolStripButton_LockUpdate";
      this.toolStripButton_LockUpdate.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_LockUpdate.ToolTipText = "Update the layer lock status over the network";
      this.toolStripButton_LockUpdate.Click += new System.EventHandler(this.toolStripButton_LockUpdate_Click);
      // 
      // toolStripButton_FileRefresh
      // 
      this.toolStripButton_FileRefresh.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_FileRefresh.Image = global::Editor.Properties.Resources.folder_refresh;
      this.toolStripButton_FileRefresh.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_FileRefresh.Name = "toolStripButton_FileRefresh";
      this.toolStripButton_FileRefresh.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_FileRefresh.Text = "toolStripButton1";
      this.toolStripButton_FileRefresh.ToolTipText = "Check for updated layer files that are modified in the network";
      this.toolStripButton_FileRefresh.Click += new System.EventHandler(this.toolStripButton_FileRefresh_Click);
      // 
      // treeView_Layers
      // 
      this.treeView_Layers.AllowDrop = true;
      this.treeView_Layers.AllowMultiSelect = true;
      this.treeView_Layers.CheckBoxes = true;
      this.treeView_Layers.Dock = System.Windows.Forms.DockStyle.Fill;
      this.treeView_Layers.HideSelection = false;
      this.treeView_Layers.LabelEdit = true;
      this.treeView_Layers.Location = new System.Drawing.Point(0, 310);
      this.treeView_Layers.Name = "treeView_Layers";
      this.treeView_Layers.SelectedNodes = ((System.Collections.ArrayList)(resources.GetObject("treeView_Layers.SelectedNodes")));
      this.treeView_Layers.ShowNodeToolTips = true;
      this.treeView_Layers.Size = new System.Drawing.Size(340, 342);
      this.treeView_Layers.TabIndex = 4;
      // 
      // contextMenuStrip_Zones
      // 
      this.contextMenuStrip_Zones.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.moveShapesToolStripMenuItem,
            this.loadUnloadToolStripMenuItem,
            this.relightToolStripMenuItem,
            this.showZoneAssetsToolStripMenuItem,
            this.toolStripSeparator2,
            this.toolStripMenuItem_DeleteZone,
            this.toolStripMenuItem_RenameZone});
      this.contextMenuStrip_Zones.Name = "contextMenuStrip_Layer";
      this.contextMenuStrip_Zones.Size = new System.Drawing.Size(170, 142);
      this.contextMenuStrip_Zones.Opening += new System.ComponentModel.CancelEventHandler(this.contextMenuStrip_Zones_Opening);
      // 
      // moveShapesToolStripMenuItem
      // 
      this.moveShapesToolStripMenuItem.Image = global::Editor.Properties.Resources.gizmo;
      this.moveShapesToolStripMenuItem.Name = "moveShapesToolStripMenuItem";
      this.moveShapesToolStripMenuItem.Size = new System.Drawing.Size(169, 22);
      this.moveShapesToolStripMenuItem.Text = "Move Shapes";
      this.moveShapesToolStripMenuItem.ToolTipText = "Move the content of the selected zone(s) using a move gizmo";
      this.moveShapesToolStripMenuItem.Click += new System.EventHandler(this.moveShapesToolStripMenuItem_Click);
      // 
      // loadUnloadToolStripMenuItem
      // 
      this.loadUnloadToolStripMenuItem.Image = global::Editor.Properties.Resources.unload_zone;
      this.loadUnloadToolStripMenuItem.Name = "loadUnloadToolStripMenuItem";
      this.loadUnloadToolStripMenuItem.Size = new System.Drawing.Size(169, 22);
      this.loadUnloadToolStripMenuItem.Text = "Load/Unload";
      this.loadUnloadToolStripMenuItem.Click += new System.EventHandler(this.loadUnloadToolStripMenuItem_Click);
      // 
      // relightToolStripMenuItem
      // 
      this.relightToolStripMenuItem.Image = global::Editor.Properties.Resources.lightbulb_on;
      this.relightToolStripMenuItem.Name = "relightToolStripMenuItem";
      this.relightToolStripMenuItem.Size = new System.Drawing.Size(169, 22);
      this.relightToolStripMenuItem.Text = "Relight";
      this.relightToolStripMenuItem.Click += new System.EventHandler(this.relightToolStripMenuItem_Click);
      // 
      // showZoneAssetsToolStripMenuItem
      // 
      this.showZoneAssetsToolStripMenuItem.Image = global::Editor.Properties.Resources.memory;
      this.showZoneAssetsToolStripMenuItem.Name = "showZoneAssetsToolStripMenuItem";
      this.showZoneAssetsToolStripMenuItem.Size = new System.Drawing.Size(169, 22);
      this.showZoneAssetsToolStripMenuItem.Text = "Show Zone Assets";
      this.showZoneAssetsToolStripMenuItem.Click += new System.EventHandler(this.showZoneAssetsToolStripMenuItem_Click);
      // 
      // toolStripSeparator2
      // 
      this.toolStripSeparator2.Name = "toolStripSeparator2";
      this.toolStripSeparator2.Size = new System.Drawing.Size(166, 6);
      // 
      // toolStripMenuItem_DeleteZone
      // 
      this.toolStripMenuItem_DeleteZone.Image = global::Editor.Properties.Resources.delete2;
      this.toolStripMenuItem_DeleteZone.Name = "toolStripMenuItem_DeleteZone";
      this.toolStripMenuItem_DeleteZone.Size = new System.Drawing.Size(169, 22);
      this.toolStripMenuItem_DeleteZone.Text = "Delete";
      this.toolStripMenuItem_DeleteZone.ToolTipText = "Deletes the selected zone";
      this.toolStripMenuItem_DeleteZone.Click += new System.EventHandler(this.toolStripMenuItem_DeleteZone_Click);
      // 
      // toolStripMenuItem_RenameZone
      // 
      this.toolStripMenuItem_RenameZone.Image = global::Editor.Properties.Resources.rename;
      this.toolStripMenuItem_RenameZone.Name = "toolStripMenuItem_RenameZone";
      this.toolStripMenuItem_RenameZone.Size = new System.Drawing.Size(169, 22);
      this.toolStripMenuItem_RenameZone.Text = "Rename";
      this.toolStripMenuItem_RenameZone.ToolTipText = "Renames the selected zone";
      this.toolStripMenuItem_RenameZone.Click += new System.EventHandler(this.toolStripMenuItem_RenameZone_Click);
      // 
      // toolStrip_Shapes
      // 
      this.toolStrip_Shapes.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
      this.toolStrip_Shapes.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton_AddGroup,
            this.toolStripButton_DeleteShape,
            this.toolStripSeparator3,
            this.toolStripButton_ShapeMoveDown,
            this.toolStripButton_ShapeMoveUp,
            this.toolStripButton_SortShapesAZUp,
            this.toolStripButton_SortShapesAZDown,
            this.toolStripSeparator4,
            this.toolStripSplitButton_MoveToLayer,
            this.toolStripSplitButton_ToolsOptions});
      this.toolStrip_Shapes.Location = new System.Drawing.Point(0, 0);
      this.toolStrip_Shapes.Name = "toolStrip_Shapes";
      this.toolStrip_Shapes.Size = new System.Drawing.Size(340, 25);
      this.toolStrip_Shapes.TabIndex = 0;
      this.toolStrip_Shapes.Text = "toolStrip2";
      // 
      // toolStripButton_AddGroup
      // 
      this.toolStripButton_AddGroup.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_AddGroup.Image = global::Editor.Properties.Resources.folder_new;
      this.toolStripButton_AddGroup.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_AddGroup.Name = "toolStripButton_AddGroup";
      this.toolStripButton_AddGroup.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_AddGroup.Text = "toolStripButton1";
      this.toolStripButton_AddGroup.ToolTipText = "Adds new group to layer";
      this.toolStripButton_AddGroup.Click += new System.EventHandler(this.toolStripButton_AddGroup_Click);
      // 
      // toolStripButton_DeleteShape
      // 
      this.toolStripButton_DeleteShape.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_DeleteShape.Image = global::Editor.Properties.Resources.delete2;
      this.toolStripButton_DeleteShape.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_DeleteShape.Name = "toolStripButton_DeleteShape";
      this.toolStripButton_DeleteShape.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_DeleteShape.Text = "toolStripButton2";
      this.toolStripButton_DeleteShape.ToolTipText = "Delete the selected shape";
      this.toolStripButton_DeleteShape.Click += new System.EventHandler(this.toolStripButton_DeleteShape_Click);
      // 
      // toolStripSeparator3
      // 
      this.toolStripSeparator3.Name = "toolStripSeparator3";
      this.toolStripSeparator3.Size = new System.Drawing.Size(6, 25);
      // 
      // toolStripButton_ShapeMoveDown
      // 
      this.toolStripButton_ShapeMoveDown.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_ShapeMoveDown.Image = global::Editor.Properties.Resources.arrow_down_blue;
      this.toolStripButton_ShapeMoveDown.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_ShapeMoveDown.Name = "toolStripButton_ShapeMoveDown";
      this.toolStripButton_ShapeMoveDown.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_ShapeMoveDown.Text = "toolStripButton3";
      this.toolStripButton_ShapeMoveDown.ToolTipText = "Move the shape one item down in the shape list";
      this.toolStripButton_ShapeMoveDown.Click += new System.EventHandler(this.toolStripButton_ShapeMoveDown_Click);
      // 
      // toolStripButton_ShapeMoveUp
      // 
      this.toolStripButton_ShapeMoveUp.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_ShapeMoveUp.Image = global::Editor.Properties.Resources.arrow_up_blue;
      this.toolStripButton_ShapeMoveUp.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_ShapeMoveUp.Name = "toolStripButton_ShapeMoveUp";
      this.toolStripButton_ShapeMoveUp.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_ShapeMoveUp.Text = "toolStripButton4";
      this.toolStripButton_ShapeMoveUp.ToolTipText = "Move the shape one layer up in the shape list";
      this.toolStripButton_ShapeMoveUp.Click += new System.EventHandler(this.toolStripButton_ShapeMoveUp_Click);
      // 
      // toolStripButton_SortShapesAZUp
      // 
      this.toolStripButton_SortShapesAZUp.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_SortShapesAZUp.Image = global::Editor.Properties.Resources.sort_az_descending;
      this.toolStripButton_SortShapesAZUp.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_SortShapesAZUp.Name = "toolStripButton_SortShapesAZUp";
      this.toolStripButton_SortShapesAZUp.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_SortShapesAZUp.Text = "toolStripButton5";
      this.toolStripButton_SortShapesAZUp.ToolTipText = "Sort the shapes alphabetically by name";
      this.toolStripButton_SortShapesAZUp.Click += new System.EventHandler(this.toolStripButton_SortShapesAZUp_Click);
      // 
      // toolStripButton_SortShapesAZDown
      // 
      this.toolStripButton_SortShapesAZDown.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_SortShapesAZDown.Image = global::Editor.Properties.Resources.sort_az_ascending;
      this.toolStripButton_SortShapesAZDown.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_SortShapesAZDown.Name = "toolStripButton_SortShapesAZDown";
      this.toolStripButton_SortShapesAZDown.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_SortShapesAZDown.Text = "toolStripButton6";
      this.toolStripButton_SortShapesAZDown.ToolTipText = "Sort the shapes alphabetically by name";
      this.toolStripButton_SortShapesAZDown.Click += new System.EventHandler(this.toolStripButton_SortShapesAZDown_Click);
      // 
      // toolStripSeparator4
      // 
      this.toolStripSeparator4.Name = "toolStripSeparator4";
      this.toolStripSeparator4.Size = new System.Drawing.Size(6, 25);
      // 
      // toolStripSplitButton_MoveToLayer
      // 
      this.toolStripSplitButton_MoveToLayer.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripSplitButton_MoveToLayer.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.layersGoHereToolStripMenuItem1});
      this.toolStripSplitButton_MoveToLayer.Image = global::Editor.Properties.Resources.breakpoint_into;
      this.toolStripSplitButton_MoveToLayer.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripSplitButton_MoveToLayer.Name = "toolStripSplitButton_MoveToLayer";
      this.toolStripSplitButton_MoveToLayer.Size = new System.Drawing.Size(32, 22);
      this.toolStripSplitButton_MoveToLayer.Text = "toolStripSplitButton1";
      this.toolStripSplitButton_MoveToLayer.ToolTipText = "Move selected shape to a different layer";
      this.toolStripSplitButton_MoveToLayer.ButtonClick += new System.EventHandler(this.toolStripSplitButton_MoveToLayer_ButtonClick);
      this.toolStripSplitButton_MoveToLayer.DropDownOpening += new System.EventHandler(this.toolStripSplitButton_MoveToLayer_DropDownOpening);
      // 
      // layersGoHereToolStripMenuItem1
      // 
      this.layersGoHereToolStripMenuItem1.Name = "layersGoHereToolStripMenuItem1";
      this.layersGoHereToolStripMenuItem1.Size = new System.Drawing.Size(166, 22);
      this.layersGoHereToolStripMenuItem1.Text = "<Layers go here>";
      // 
      // toolStripSplitButton_ToolsOptions
      // 
      this.toolStripSplitButton_ToolsOptions.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripSplitButton_ToolsOptions.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripSplitButton_AutomaticFocus,
            this.toolStripSplitButton_PropertiesOnSelection});
      this.toolStripSplitButton_ToolsOptions.Image = global::Editor.Properties.Resources.toolbar_selection_tools;
      this.toolStripSplitButton_ToolsOptions.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripSplitButton_ToolsOptions.Name = "toolStripSplitButton_ToolsOptions";
      this.toolStripSplitButton_ToolsOptions.Size = new System.Drawing.Size(32, 22);
      this.toolStripSplitButton_ToolsOptions.ToolTipText = "Options for the Shapes Tree View";
      this.toolStripSplitButton_ToolsOptions.DropDownOpening += new System.EventHandler(this.toolStripSplitButton_ToolsOptions_DropDownOpening);
      // 
      // toolStripSplitButton_AutomaticFocus
      // 
      this.toolStripSplitButton_AutomaticFocus.CheckOnClick = true;
      this.toolStripSplitButton_AutomaticFocus.Image = global::Editor.Properties.Resources.toolbar_focus_selection;
      this.toolStripSplitButton_AutomaticFocus.Name = "toolStripSplitButton_AutomaticFocus";
      this.toolStripSplitButton_AutomaticFocus.Size = new System.Drawing.Size(261, 22);
      this.toolStripSplitButton_AutomaticFocus.Text = "Automatic Focus On Selection";
      this.toolStripSplitButton_AutomaticFocus.Click += new System.EventHandler(this.toolStripSplitButton_AutomaticFocus_Click);
      // 
      // toolStripSplitButton_PropertiesOnSelection
      // 
      this.toolStripSplitButton_PropertiesOnSelection.CheckOnClick = true;
      this.toolStripSplitButton_PropertiesOnSelection.Image = global::Editor.Properties.Resources.toolbar_properties_selection;
      this.toolStripSplitButton_PropertiesOnSelection.Name = "toolStripSplitButton_PropertiesOnSelection";
      this.toolStripSplitButton_PropertiesOnSelection.Size = new System.Drawing.Size(261, 22);
      this.toolStripSplitButton_PropertiesOnSelection.Text = "Show Properties Panel On Selection";
      this.toolStripSplitButton_PropertiesOnSelection.Click += new System.EventHandler(this.toolStripSplitButton_PropertiesOnSelection_Click);
      // 
      // panel_UpdateLayer
      // 
      this.panel_UpdateLayer.AutoSize = true;
      this.panel_UpdateLayer.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
      this.panel_UpdateLayer.ColumnCount = 2;
      this.panel_UpdateLayer.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 40F));
      this.panel_UpdateLayer.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
      this.panel_UpdateLayer.Controls.Add(this.pictureBox_UpdateIcon, 0, 0);
      this.panel_UpdateLayer.Controls.Add(this.flowLayoutPanel1, 1, 1);
      this.panel_UpdateLayer.Controls.Add(this.label1, 1, 0);
      this.panel_UpdateLayer.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.panel_UpdateLayer.Location = new System.Drawing.Point(0, 596);
      this.panel_UpdateLayer.Name = "panel_UpdateLayer";
      this.panel_UpdateLayer.RowCount = 2;
      this.panel_UpdateLayer.RowStyles.Add(new System.Windows.Forms.RowStyle());
      this.panel_UpdateLayer.RowStyles.Add(new System.Windows.Forms.RowStyle());
      this.panel_UpdateLayer.Size = new System.Drawing.Size(340, 56);
      this.panel_UpdateLayer.TabIndex = 6;
      // 
      // pictureBox_UpdateIcon
      // 
      this.pictureBox_UpdateIcon.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.pictureBox_UpdateIcon.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox_UpdateIcon.Image")));
      this.pictureBox_UpdateIcon.Location = new System.Drawing.Point(6, 6);
      this.pictureBox_UpdateIcon.Margin = new System.Windows.Forms.Padding(6, 6, 3, 3);
      this.pictureBox_UpdateIcon.Name = "pictureBox_UpdateIcon";
      this.panel_UpdateLayer.SetRowSpan(this.pictureBox_UpdateIcon, 2);
      this.pictureBox_UpdateIcon.Size = new System.Drawing.Size(31, 47);
      this.pictureBox_UpdateIcon.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
      this.pictureBox_UpdateIcon.TabIndex = 0;
      this.pictureBox_UpdateIcon.TabStop = false;
      // 
      // flowLayoutPanel1
      // 
      this.flowLayoutPanel1.AutoSize = true;
      this.flowLayoutPanel1.Controls.Add(this.button_UpdateIgnore);
      this.flowLayoutPanel1.Controls.Add(this.button_LayerUpdate);
      this.flowLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.flowLayoutPanel1.FlowDirection = System.Windows.Forms.FlowDirection.RightToLeft;
      this.flowLayoutPanel1.Location = new System.Drawing.Point(43, 24);
      this.flowLayoutPanel1.Name = "flowLayoutPanel1";
      this.flowLayoutPanel1.Size = new System.Drawing.Size(294, 29);
      this.flowLayoutPanel1.TabIndex = 1;
      // 
      // button_UpdateIgnore
      // 
      this.button_UpdateIgnore.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.button_UpdateIgnore.Location = new System.Drawing.Point(231, 3);
      this.button_UpdateIgnore.Name = "button_UpdateIgnore";
      this.button_UpdateIgnore.Size = new System.Drawing.Size(60, 23);
      this.button_UpdateIgnore.TabIndex = 2;
      this.button_UpdateIgnore.Text = "Ignore";
      this.button_UpdateIgnore.Click += new System.EventHandler(this.button_UpdateIgnore_Click);
      // 
      // button_LayerUpdate
      // 
      this.button_LayerUpdate.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.button_LayerUpdate.Location = new System.Drawing.Point(165, 3);
      this.button_LayerUpdate.Name = "button_LayerUpdate";
      this.button_LayerUpdate.Size = new System.Drawing.Size(60, 23);
      this.button_LayerUpdate.TabIndex = 1;
      this.button_LayerUpdate.Text = "Update";
      this.button_LayerUpdate.Click += new System.EventHandler(this.button_LayerUpdate_Click);
      // 
      // label1
      // 
      this.label1.AutoSize = true;
      this.label1.Location = new System.Drawing.Point(43, 4);
      this.label1.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(191, 13);
      this.label1.TabIndex = 0;
      this.label1.Text = "One or more layers have been modified";
      // 
      // ShapeTreePanel
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(340, 652);
      this.Controls.Add(this.panel_UpdateLayer);
      this.Controls.Add(this.treeView_Layers);
      this.Controls.Add(this.toolStrip1);
      this.Controls.Add(this.splitter1);
      this.Controls.Add(this.shapeTreeView);
      this.Controls.Add(this.toolStrip_Shapes);
      this.DockableAreas = ((WeifenLuo.WinFormsUI.DockAreas)(((WeifenLuo.WinFormsUI.DockAreas.Float | WeifenLuo.WinFormsUI.DockAreas.DockLeft) 
            | WeifenLuo.WinFormsUI.DockAreas.DockRight)));
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.Name = "ShapeTreePanel";
      this.ShowHint = WeifenLuo.WinFormsUI.DockState.DockLeft;
      this.TabText = "Shapes";
      this.Text = "Shapes and Layers";
      this.contextMenuStrip_Shapes.ResumeLayout(false);
      this.contextMenuStrip_Layer.ResumeLayout(false);
      this.toolStrip1.ResumeLayout(false);
      this.toolStrip1.PerformLayout();
      this.contextMenuStrip_Zones.ResumeLayout(false);
      this.toolStrip_Shapes.ResumeLayout(false);
      this.toolStrip_Shapes.PerformLayout();
      this.panel_UpdateLayer.ResumeLayout(false);
      this.panel_UpdateLayer.PerformLayout();
      ((System.ComponentModel.ISupportInitialize)(this.pictureBox_UpdateIcon)).EndInit();
      this.flowLayoutPanel1.ResumeLayout(false);
      this.ResumeLayout(false);
      this.PerformLayout();

    }
		#endregion

    #region Event Listeners

    /// <summary>
    /// Updates the treenode background color for the active shape
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void Layer_ActiveShapeChanged(object sender, CSharpFramework.Layer.ActiveShapeChangedArgs e)
    {
      if (e.OldActiveShape != null)
      {
        // Restore the treenode background of the previously active shape
        ShapeTreeNode treeNode = shapeTreeView.FindShapeInTree(e.OldActiveShape, null);
        if (treeNode != null)
          treeNode.BackColor = Color.Empty;
      }

      if (e.NewActiveShape != null)
      {
        // Highlight the treenode for the new active shape
        ShapeTreeNode treeNode = shapeTreeView.FindShapeInTree(e.NewActiveShape, null);
        if (treeNode != null)
          treeNode.BackColor = ACTIVESHAPE_BKCOLOR;
      }
    }

    private void shapeTreeView_NodeStateChanged(object sender, CSharpFramework.TreeViewMS.NodeStateChangedArgs e)
    {
      ShapeTreeNode newNode = e.NewNode as ShapeTreeNode;
      if (e.Reason == TreeViewMS.NodeStateChangedArgs.Reasons.NodeCreated || e.Reason == TreeViewMS.NodeStateChangedArgs.Reasons.NodeUnSelected)
        if (newNode != null && EditorManager.Scene != null && newNode.shape == EditorManager.Scene.ActiveLayer.ActiveShape)
          newNode.BackColor = ACTIVESHAPE_BKCOLOR;      
    }


    void treeView_Layers_OnSelectionChanged(object sender, EventArgs e)
    {
      UpdateLayerToolbar();
    }

    #endregion

    #region Shape Context Menu


    /// <summary>
    /// Helper function
    /// </summary>
    /// <param name="order"></param>
    private void SortShapes(SortOrder order)
    {
      if (EditorManager.Scene == null || EditorManager.Scene.ActiveLayer == null)
        return;

      ShapeCollection shapes = EditorApp.ActiveView.Gizmo.Shapes;

      if (shapes != null && shapes.Count > 0)
      {
        EditorManager.Actions.StartGroup("Sort shapes");
        foreach (ShapeBase shape in shapes)
        {
          if (shape.HasChildren())
          {
            EditorManager.Actions.Add(new SortShapeChildrenAlphabeticallyAction(shape, order));
          }
          else
          {
            ShapeBase parent = shape.Parent;

            if (parent == null)
              parent = EditorManager.Scene.ActiveLayer.Root;

            if (parent != null)
              EditorManager.Actions.Add(new SortShapeChildrenAlphabeticallyAction(parent, order));
          }
        }
        EditorManager.Actions.EndGroup();
      }
      else
      {
        ShapeBase parent = EditorManager.Scene.ActiveLayer.Root;

        if (parent != null)
          EditorManager.Actions.Add(new SortShapeChildrenAlphabeticallyAction(parent, order));
      }
    }

    /// <summary>
    /// Helper function
    /// </summary>
    /// <param name="mode"></param>
    private void DoDropToFloor(Shape3D.DropToFloorMode mode, Vector3F axis)
    {
      Cursor = Cursors.WaitCursor;
      ShapeCollection shapes = EditorApp.ActiveView.Gizmo.Shapes;
      EditorManager.Actions.Add(new DropToFloorAction(shapes, mode, axis, true));
      EditorApp.ActiveView.UpdateView(false);
      Cursor = Cursors.Default;
    }

    private void contextMenuStrip_Shapes_Opening(object sender, CancelEventArgs e)
    {
      ContextMenuStrip menu = (ContextMenuStrip)sender;

      ShapeCollection shapes = shapeTreeView.SelectedShapes;

      // find the entry in the context menu and remeber its index
      ToolStripMenuItem operationItem = EditorManager.FindSubMenuItem(menu.Items, "Relevant Operations");
      int index = (operationItem != null) ? menu.Items.IndexOf(operationItem) : -1;
      int iSelCount = (shapes != null) ? shapes.Count : 0;

      // remove the old entry 
      menu.Items.Remove(operationItem);

      // and add the current one
      operationItem = shapes.GetRelevantOperationsMenuItem();
      if (index < 0)
        menu.Items.Add(operationItem);
      else
        menu.Items.Insert(index, operationItem);
      operationItem.Image = global::Editor.Properties.Resources.wrench; // removing seems to reset this

      // fill the Create items
      EditorManager.BuildCreateMenu(createToolStripMenuItem);

      // enable/disable all
      bool bHasScene = EditorManager.Scene != null;
      foreach (ToolStripItem item in menu.Items)
        item.Enabled = bHasScene;

      bool bAnyMovable = false;
      if (shapes!=null)
        foreach (ShapeBase shape in shapes)
        {
          if (!(shape is Shape3D) || !shape.Modifiable || shape.IsHintSet(ShapeBase.HintFlags_e.NoUserTransform))
            continue;
          bAnyMovable = true;
        }
      dropToFloorToolStripMenuItem.Visible = bAnyMovable;

      createPrefabToolStripMenuItem.Enabled = bHasScene && iSelCount > 0;

      this.lockShapesToolStripMenuItem.Enabled = iSelCount > 0;
      if (lockShapesToolStripMenuItem.Enabled)
      {
        string text = shapes.Count == 1 ? "shape" : "shapes";
        if (shapes.EvaluateUseShapeUnLockingPreference())
        {
          lockShapesToolStripMenuItem.Text = "Un-freeze " + text;
          this.lockShapesToolStripMenuItem.Image = global::Editor.Properties.Resources.lock_open;
        }
        else
        {
          lockShapesToolStripMenuItem.Text = "Freeze " + text;
          this.lockShapesToolStripMenuItem.Image = global::Editor.Properties.Resources._lock;
        }
      }

      // Layers shown in "Move to Layer" item
      if (bHasScene && shapes.Count>0)
      {
        moveToLayerToolStripMenuItem.DropDownItems.Clear();
        foreach (Layer layer in EditorManager.Scene.Layers)
          if (layer.Modifiable && layer.Loaded && layer != EditorManager.Scene.ActiveLayer)
          {
            if (!SetShapeParentAction.TestShapesLinkToDifferentZone(shapes, layer.Root, false))
              continue;

            ToolStripItem item = moveToLayerToolStripMenuItem.DropDownItems.Add(layer.LayerName);
            item.Tag = layer;
            item.Click += new EventHandler(ShapeMoveToLayerItem_Click);
          }
        moveToLayerToolStripMenuItem.Enabled = moveToLayerToolStripMenuItem.DropDownItems.Count > 0;
      }
      else
      {
        moveToLayerToolStripMenuItem.Enabled = false;
      }
    }

    void ShapeMoveToLayerItem_Click(object sender, EventArgs e)
    {
      ShapeCollection shapes = shapeTreeView.SelectedShapes;
      if (shapes.Count == 0)
        return;
      ShapeBase root = ((Layer)((ToolStripItem)sender).Tag).Root;
      if (!SetShapeParentAction.TestShapesLinkToDifferentZone(shapes, root, true))
        return;

      EditorManager.Actions.StartGroup("Move shapes to layer ");
      foreach (ShapeBase shape in shapes)
        EditorManager.Actions.Add(new SetShapeParentAction(shape, root));
      EditorManager.Actions.EndGroup();

      // todo: check if any shapes have actually moved...(doesn't happen if every shape you try to move is 'frozen')
      EditorManager.Scene.ActiveLayer = (Layer)(sender as ToolStripMenuItem).Tag;
    }

    private void setShapeActiveToolStripMenuItem_Click(object sender, EventArgs e)
    {
      ShapeBase shape = shapeTreeView.SelectedShape;
      EditorApp.Scene.ActiveLayer.ActiveShape = shape;
    }

    private void selectChildNodesToolStripMenuItem_Click(object sender, EventArgs e)
    {
      // set the new scene selection
      EditorManager.SelectedShapes = shapeTreeView.ChildShapes; 
    }

    private void selectRecursivelyToolStripMenuItem_Click(object sender, EventArgs e)
    {
      // set the new scene selection
      EditorManager.SelectedShapes = shapeTreeView.ChildShapesRecursively; 
    }

    private void cutShapeToolStripMenuItem_Click(object sender, EventArgs e)
    {
      shapeTreeView.OnCut();
    }

    private void copyShapeToolStripMenuItem_Click(object sender, EventArgs e)
    {
      shapeTreeView.OnCopy();
    }

    private void pasteShapeToolStripMenuItem_Click(object sender, EventArgs e)
    {
      shapeTreeView.OnPaste();
    }

    private void deleteShapeToolStripMenuItem_Click(object sender, EventArgs e)
    {
      shapeTreeView.OnDelete();
    }

    private void renameShapeToolStripMenuItem_Click(object sender, EventArgs e)
    {
      shapeTreeView.RenameShape(shapeTreeView.SelectedShape);
    }


    private void pivotToolStripMenuItem_Click(object sender, EventArgs e)
    {
      DoDropToFloor(Shape3D.DropToFloorMode.Pivot, EnginePanel.CurrentDropToFloorAxis);
    }

    private void boundingBoxToolStripMenuItem_Click(object sender, EventArgs e)
    {
      DoDropToFloor(Shape3D.DropToFloorMode.BoundingBox, EnginePanel.CurrentDropToFloorAxis);
    }

    private void bottomCenterToolStripMenuItem_Click(object sender, EventArgs e)
    {
      DoDropToFloor(Shape3D.DropToFloorMode.BottomCenter, EnginePanel.CurrentDropToFloorAxis);
    }

    private void directionFloorToolStripMenuItem_Click(object sender, EventArgs e)
    {
      ToolStripMenuItem item = sender as ToolStripMenuItem;
      if (item == null || !(item.Tag is Vector3F))
        return;
      EnginePanel.CurrentDropToFloorAxis = (Vector3F)item.Tag;
    }

    private void dropToFloorToolStripMenuItem_DropDownOpening(object sender, EventArgs e)
    {
      directionPosXToolStripMenuItem.Checked = EnginePanel.CurrentDropToFloorAxis == (Vector3F)directionPosXToolStripMenuItem.Tag;
      directionNegXToolStripMenuItem.Checked = EnginePanel.CurrentDropToFloorAxis == (Vector3F)directionNegXToolStripMenuItem.Tag;
      directionPosYToolStripMenuItem.Checked = EnginePanel.CurrentDropToFloorAxis == (Vector3F)directionPosYToolStripMenuItem.Tag;
      directionNegYToolStripMenuItem.Checked = EnginePanel.CurrentDropToFloorAxis == (Vector3F)directionNegYToolStripMenuItem.Tag;
      directionCeilingToolStripMenuItem.Checked = EnginePanel.CurrentDropToFloorAxis == (Vector3F)directionCeilingToolStripMenuItem.Tag;
      directionFloorToolStripMenuItem.Checked = EnginePanel.CurrentDropToFloorAxis == (Vector3F)directionFloorToolStripMenuItem.Tag;
    }

    private void jumpToToolStripMenuItem_Click(object sender, EventArgs e)
    {
      //Ensure that all shapes are visible:
      BoundingBox mergedBox = EditorManager.SelectedShapes.BoundingBox;
      if (!mergedBox.Valid)
        return;

      // make it local again
      Vector3F center = mergedBox.Center;
      mergedBox.Translate(-center);

      EditorManager.ActiveView.LookAt(center, mergedBox);
    }

    private void alphabeticallyUpToolStripMenuItem_Click(object sender, EventArgs e)
    {
      SortShapes(SortOrder.Ascending);
    }

    private void alphabeticallyDownToolStripMenuItem_Click(object sender, EventArgs e)
    {
      SortShapes(SortOrder.Descending);
    }

    private void createPrefabToolStripMenuItem_Click(object sender, EventArgs e)
    {
      ShapeCollection shapes = shapeTreeView.SelectedShapes;
      PrefabDesc prefab = new PrefabDesc(null);

      // try to use/create the prefabs directory of the project
      string proj = EditorManager.Project.ProjectDir;
      string prefabDir = Path.Combine(proj, EditorManager.Settings.PrefabDirectory);
      if (!Directory.Exists(prefabDir))
      {
        try
        {
          Directory.CreateDirectory(prefabDir);
        }
        catch (Exception ex)
        {
          EditorManager.DumpException(ex);
          prefabDir = proj; // OK, fallback to project directory
        }
      }

      // first select the prefab file to save
      CreateFileDlg fileDlg = new CreateFileDlg();
      fileDlg.Caption = "Save a prefab";
      fileDlg.Description = "Enter the name of the prefab file and select the directory to save it in. Then click OK to proceed.";
      fileDlg.InitialDirectory = prefabDir;
      fileDlg.Ext = ".prefab";
      fileDlg.Filter = new string[] { ".prefab" };
      fileDlg.AllowOverwrite = true;

      if (fileDlg.ShowDialog() != DialogResult.OK)
        return;

      prefab.Filename = fileDlg.FileName;

      // if the file exists, load it to get the properties
      if (File.Exists(prefab.Filename))
      {
        prefab.Load();
      }

      // set this before the dialog opens
      prefab.SourceShapesForExposedParameters = shapes;

      // edit the prefab properties
      using (PropertyGridDlg propDlg = new PropertyGridDlg("Edit prefab properties", "Edit additional prefab properties. Click OK to proceed with saving the prefab file."))
      {
        propDlg.DataObjectNoClone = prefab;
        if (propDlg.ShowDialog() != DialogResult.OK)
          return;

        // fill with the shapes
        if (!prefab.CreateFromInstances(shapes, shapes.Center, true, true))
        {
          EditorManager.ShowMessageBox("Failed to create prefab. Detailed message:\n\n" + prefab.LastError, "Creation Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
          return;
        }

        if (!prefab.SaveToFile(fileDlg.FileName))
        {
          EditorManager.ShowMessageBox("Failed to save prefab. Detailed message:\n\n" + prefab.LastError, "Saving Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
          return;
        }
      }
    }

    private void lockShapesToolStripMenuItem_Click(object sender, EventArgs e)
    {
      ShapeCollection shapes = shapeTreeView.SelectedShapes;
      if (shapes == null)
        return;
      bool bUseUnlocking = shapes.EvaluateUseShapeUnLockingPreference();
      EditorManager.Actions.Add(new FreezeShapesAction(shapes, !bUseUnlocking));
    }



    #endregion

    #region Layer Context Menu

    private void contextMenuStrip_Layer_Opening(object sender, CancelEventArgs e)
    {
      treeView_Layers.UpdateSelectionStatus();

      bool bHasScene = EditorManager.Scene != null;
      bool bEditable = treeView_Layers.Selection_AllLayersEditable;
      bool bRemovable = treeView_Layers.Selection_AllLayersRemovable;
      deleteToolStripMenuItem.Enabled = bRemovable;
      renameToolStripMenuItem.Enabled = bEditable;

      if (treeView_Layers.Selection_Layers.Count == 1)
      {
        lockStatusToolStripMenuItem.Visible = true;
        lockStatusToolStripMenuItem.Text = "LockStatus: " + treeView_Layers.Selection_Layers[0].LockStatus;
      }
      else
      {
        lockStatusToolStripMenuItem.Visible = false;
      }
      
      lockLayerToolStripMenuItem.Enabled = treeView_Layers.Selection_AnyLayerNotLocked;
      updateLockStatusToolStripMenuItem.Enabled = !bEditable && !treeView_Layers.Selection_AnyLayerReference;
      unlockLayerToolStripMenuItem.Enabled = bEditable;

      unfreezeAllShapesToolStripMenuItem.Enabled = treeView_Layers.Selection_AnyLayerHasFrozenShapes;
      showLayerAssetsToolStripMenuItem.Enabled = treeView_Layers.Selection_Layers.Count > 0;

      string errorMsg = null;
      bool bAllowRename = true;
      bool bAnyNonMatching = false;
      bool bCanSortLayersIntoZones = treeView_Layers.Selection_LayersOnly;
      foreach (Layer layer in treeView_Layers.Selection_Layers)
      {
        if (!layer.Loaded || layer.ParentZone != null || layer.IsReference)
          bCanSortLayersIntoZones = false;
        if (layer.FileNameMatchesName())
          continue;
        bAnyNonMatching = true;
        if (!layer.SetLayerNameAsFilename(true, ref errorMsg))
        {
          bAllowRename = false;
          break;
        }
      }
      bAllowRename &= bAnyNonMatching;
      toolStripMenuItem_LastSeparator.Visible = bAllowRename;
      renameFileToLayerNameToolStripMenuItem.Visible = bAllowRename;
      sortShapesIntoZonesToolStripMenuItem.Enabled = bCanSortLayersIntoZones;
    }

    private void lockLayerToolStripMenuItem_Click(object sender, EventArgs e)
    {
      foreach (Layer layer in treeView_Layers.Selection_Layers)
      {
        layer.TryLock(null, true);
      }
    }

    private void unlockLayerToolStripMenuItem_Click(object sender, EventArgs e)
    {
      foreach (Layer layer in treeView_Layers.Selection_Layers)
      {
        if (!layer.Modifiable)
          continue;
        if (!layer.Dirty || layer.SaveToFile())
        {
          if (!layer.ReleaseLock(true, true))
            EditorManager.ShowMessageBox("The layer \"" + layer.LayerName + "\" cannot be unlocked since it has been locked by the current user and has already been edited. You will need to either submit or revert the changes with your revision control system in order to unlock it again.", "Layer cannot be unlocked", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }
      }
    }

    private void updateLockStatusToolStripMenuItem_Click(object sender, EventArgs e)
    {
      foreach (Layer layer in treeView_Layers.Selection_Layers)
        layer.UpdateLockStatus(true, true);
    }

    private void cutToolStripMenuItem_Click(object sender, EventArgs e)
    {
      treeView_Layers.OnCut();
    }

    private void copyToolStripMenuItem_Click(object sender, EventArgs e)
    {
      treeView_Layers.OnCopy();
    }

    private void deleteToolStripMenuItem_Click(object sender, EventArgs e)
    {
      treeView_Layers.OnDelete();
    }

    private void renameToolStripMenuItem_Click(object sender, EventArgs e)
    {
      Layer layer = EditorApp.Scene.ActiveLayer;
      treeView_Layers.BeginRenameLayer(layer);
    }

    private void unfreezeAllShapesToolStripMenuItem_Click(object sender, EventArgs e)
    {
      foreach (Layer layer in treeView_Layers.Selection_Layers)
        EditorManager.Actions.Add(new FreezeShapesAction(layer.Root.ChildCollection, false));
    }

    private void renameFileToLayerNameToolStripMenuItem_Click(object sender, EventArgs e)
    {
      string errorMsg = null;
      foreach (Layer layer in treeView_Layers.Selection_Layers)
      {
        if (!layer.SetLayerNameAsFilename(false, ref errorMsg))
        {
          EditorManager.ShowMessageBox(string.Format("Failed to rename layer file for Layer '{0}'.\n\nDetailed message:\n{1}", layer.LayerName, errorMsg), "Failed to rename layer file", MessageBoxButtons.OK, MessageBoxIcon.Error);
          break;
        }
      }

    }

    private void sortShapesIntoZonesToolStripMenuItem_Click(object sender, EventArgs e)
    {
      ZoneCollection zones = new ZoneCollection();
      foreach (Zone zone in EditorManager.Scene.Zones)
        if (zone.ConsiderForSortIntoZones)
          zones.Add(zone);
      if (zones.Count==0)
        return;

      ShapeCollection shapes = new ShapeCollection();
      foreach (Layer layer in treeView_Layers.Selection_Layers)
      {
        if (layer.ParentZone != null || !layer.Modifiable)
          continue;
        foreach (ShapeBase shape in layer.Root.ChildCollection)
          shapes.Add(shape);

      }

      if (shapes.Count > 0)
      {
        SortShapesIntoZonesAction action = new SortShapesIntoZonesAction(shapes, zones);
        EditorManager.Actions.Add(action);
      }
    }


    #endregion

    #region Zone Context Menu

    private void contextMenuStrip_Zones_Opening(object sender, CancelEventArgs e)
    {
      bool bCanLoadUnload = treeView_Layers.Selection_ZonesOnly;
      bool bFirst = true;
      bool bLastStatus = false;

      foreach (Zone zone in treeView_Layers.Selection_Zones)
      {
        if (bFirst)
        {
          bLastStatus = zone.Loaded;
          bFirst = false;
        }
        if (bLastStatus != zone.Loaded)
        {
          bCanLoadUnload = false;
          break;
        }
        bLastStatus = zone.Loaded;
      }

      loadUnloadToolStripMenuItem.Enabled = bCanLoadUnload;
      string txt;
      if (bLastStatus)
        txt = "Unload zone";
      else
        txt = "Reload zone";
      if (treeView_Layers.Selection_Zones.Count > 1)
        txt += "s";

      loadUnloadToolStripMenuItem.Text = txt;

      relightToolStripMenuItem.Enabled = treeView_Layers.Selection_AllZonesLoaded;
      showZoneAssetsToolStripMenuItem.Enabled = treeView_Layers.Selection_Zones.Count > 0;
      moveShapesToolStripMenuItem.Enabled = treeView_Layers.Selection_Zones.Count > 0;
    }

    private void toolStripMenuItem_CutZone_Click(object sender, EventArgs e)
    {
      treeView_Layers.OnCut();
    }

    private void toolStripMenuItem_CopyZone_Click(object sender, EventArgs e)
    {
      treeView_Layers.OnCopy();

    }


    
    private void toolStripMenuItem_DeleteZone_Click(object sender, EventArgs e)
    {
      treeView_Layers.OnDelete();
    }

    private void toolStripMenuItem_RenameZone_Click(object sender, EventArgs e)
    {
      Zone zone = EditorApp.Scene.ActiveZone;
      treeView_Layers.BeginRenameZone(zone);
    }

    private void loadUnloadToolStripMenuItem_Click(object sender, EventArgs e)
    {
      if (!treeView_Layers.Selection_ZonesOnly || treeView_Layers.Selection_Zones.Count < 1)
        return;

      bool bNewState = !treeView_Layers.Selection_Zones[0].Loaded;

      ZoneCollection zones = treeView_Layers.Selection_Zones.ShallowClone();
      if (zones.Count > 1)
        EditorManager.Actions.StartGroup(bNewState ? "Reloading zones" : "Unloading zones");

      foreach (Zone zone in zones)
        EditorManager.Actions.Add(new SetZoneLoadedStatusAction(zone,bNewState));

      if (zones.Count > 1)
        EditorManager.Actions.EndGroup();
    }

    private void relightToolStripMenuItem_Click(object sender, EventArgs e)
    {
      if (treeView_Layers.Selection_Zones.Count < 1)
        return;
      EditorApp.Scene.UpdateStaticLighting(false, treeView_Layers.Selection_Zones, false);
    }


    void ShowAssetDialog(ZoneCollection zones, LayerCollection layers)
    {
      ShowAssetListDlg dlg = new ShowAssetListDlg();

      ResourceSnapshotInfo info = new ResourceSnapshotInfo();
      ExportHelper.StartResourceSnapshot(info);

      string zoneNames = null;
      string layerNames = null;

      if (zones != null)
        foreach (Zone zone in zones)
        {
          zone.GetDependencies(info);
          if (zoneNames != null)
            zoneNames += ", ";
          zoneNames += zone.ZoneName;
        }

      if (layers != null)
        foreach (Layer layer in layers)
        {
          layer.GetDependencies(info);
          if (layerNames != null)
            layerNames += ", ";
          layerNames += layer.LayerName;
        }

      string desc = "This list shows the native assets used by :";
      if (zoneNames != null)
        desc += "\n  Zones : " + zoneNames;
      if (layerNames != null)
        desc += "\n  Layers : " + layerNames;
    
      dlg.Description = desc;
      dlg.AssetList = EditorManager.AssetManager.ExtractAssetInfo(info);
      ExportHelper.EndResourceSnapshot(info); // after extracting

      dlg.ShowDialog();
    }

    private void showZoneAssetsToolStripMenuItem_Click(object sender, EventArgs e)
    {
      ZoneCollection zones = treeView_Layers.Selection_Zones;
      if (zones == null || zones.Count == 0)
        return;
      ShowAssetDialog(zones, null);
    }

    private void showLayerAssetsToolStripMenuItem_Click(object sender, EventArgs e)
    {
      LayerCollection layer = treeView_Layers.Selection_Layers;
      if (layer == null || layer.Count == 0)
        return;
      ShowAssetDialog(null, layer);

    }


    private void moveShapesToolStripMenuItem_Click(object sender, EventArgs e)
    {
      if (!treeView_Layers.Selection_ZonesOnly)
        return;
      ShapeCollection shapes = new ShapeCollection();
      foreach (Zone zone in treeView_Layers.Selection_Zones)
      {
        Shape3D shape = zone.GetZoneMoveProxyShape();
        if (shape != null)
          shapes.Add(shape);
      }
      EditorManager.SelectedShapes = shapes;
    }


    #endregion

    #region Layer Toolbar

    /// <summary>
    /// Updates the layer toolbar button states
    /// </summary>
    private void UpdateLayerToolbar()
    {
      bool bHasScene = (EditorManager.Scene != null);
      if (!bHasScene)
      {
        toolStripButton_AddLayer.Enabled = false;
        toolStripButton_AddZone.Enabled = false;
        toolStripButton_DeleteLayer.Enabled = false;
        toolStripButton_LayerMoveUp.Enabled = false;
        toolStripButton_LayerMoveDown.Enabled = false;
        toolStripButton_LockUpdate.Enabled = false;
        toolStripButton_FileRefresh.Enabled = false;
        toolStripButton_SortAZUp.Enabled = false;
        toolStripButton_SortAZDown.Enabled = false;
        toolStripButton_AddGroup.Enabled = false;
        toolStripButton_DeleteShape.Enabled = false;
        toolStripButton_SortShapesAZDown.Enabled = false;
        toolStripButton_SortShapesAZUp.Enabled = false;
        toolStripSplitButton_MoveToLayer.Enabled = false;
        toolStripButton_ShapeMoveDown.Enabled = false;
        toolStripButton_ShapeMoveUp.Enabled = false;
        toolStripSplitButton_ToolsOptions.Enabled = false;
        toolStripButton_ImportReference.Enabled = false;
        return;
      }

      Zone activeZone = EditorManager.Scene.ActiveZone;
      toolStripButton_AddLayer.Enabled = bHasScene;
      toolStripButton_AddZone.Enabled = bHasScene;
      toolStripButton_DeleteLayer.Enabled = treeView_Layers.Selection_AllLayersRemovable || treeView_Layers.Selection_AllZonesModifiable;
      toolStripButton_LayerMoveUp.Enabled = treeView_Layers.Selection_SingleLayer || treeView_Layers.Selection_SingleZone;
      toolStripButton_LayerMoveDown.Enabled = toolStripButton_LayerMoveUp.Enabled;
      toolStripButton_LockUpdate.Enabled = bHasScene;
      toolStripButton_FileRefresh.Enabled = bHasScene;
      toolStripButton_AddGroup.Enabled = bHasScene;
      toolStripButton_DeleteShape.Enabled = bHasScene;
      toolStripButton_SortShapesAZDown.Enabled = bHasScene;
      toolStripButton_SortShapesAZUp.Enabled = bHasScene;
      toolStripSplitButton_MoveToLayer.Enabled = bHasScene;
      toolStripButton_ShapeMoveDown.Enabled = bHasScene;
      toolStripButton_ShapeMoveUp.Enabled = bHasScene;
      toolStripSplitButton_ToolsOptions.Enabled = bHasScene;
      toolStripButton_ImportReference.Enabled = bHasScene;

      Zone parentZone = (treeView_Layers.Selection_Layers.Count > 0 && treeView_Layers.Selection_SingleLayer) ? treeView_Layers.Selection_Layers[0].ParentZone : null;
      int iLayerCount = (parentZone == activeZone) ? EditorManager.Scene.GetZoneLayerCount(activeZone) : EditorManager.Scene.Layers.Count;
      int iZoneCount = EditorManager.Scene.Zones.Count;
      toolStripButton_SortAZUp.Enabled = (treeView_Layers.Selection_SingleLayer && iLayerCount > 1) || (treeView_Layers.Selection_SingleZone && iZoneCount > 1);
      toolStripButton_SortAZDown.Enabled = toolStripButton_SortAZUp.Enabled;
    }

    private void toolStripButton_AddLayer_Click(object sender, EventArgs e)
    {
      Layer layer = Layer.CreateLayer(treeView_Layers.Selection_Layers);
    }

    private void toolStripButton_AddZone_Click(object sender, EventArgs e)
    {
      Zone zone = Zone.CreateZone();
      treeView_Layers.SelectedNode = null; //clear selection
    }

    private void toolStripButton_DeleteLayer_Click(object sender, EventArgs e)
    {
      treeView_Layers.OnDelete();
    }

    private void toolStripButton_LayerMoveDown_Click(object sender, EventArgs e)
    {
      IScene scene = EditorManager.Scene;
      if (scene == null)
        return;

      if (treeView_Layers.Selection_SingleLayer)
      {
        Layer layer = treeView_Layers.Selection_Layers[0];
        EditorManager.Actions.Add(new MoveLayerAction(layer, MoveLayerAction.MoveDir.Down));
      }
      if (treeView_Layers.Selection_SingleZone)
      {
        Zone zone = treeView_Layers.Selection_Zones[0];
        EditorManager.Actions.Add(new MoveZoneAction(zone, 1));
      }
    }


    private void toolStripButton_LayerMoveUp_Click(object sender, EventArgs e)
    {
      IScene scene = EditorManager.Scene;
      if (scene==null)
        return;

      if (treeView_Layers.Selection_SingleLayer)
      {
        Layer layer = treeView_Layers.Selection_Layers[0];
        EditorManager.Actions.Add(new MoveLayerAction(layer, MoveLayerAction.MoveDir.Up));
      }
      if (treeView_Layers.Selection_SingleZone)
      {
        Zone zone = treeView_Layers.Selection_Zones[0];
        EditorManager.Actions.Add(new MoveZoneAction(zone, -1));
      }
    }

    private LayerCollection GetSortingRelevantLayers()
    {
      if (EditorManager.Scene == null)
        return null;
      Zone zone = null;

      // Get layers from parent zone or from scene
      if (treeView_Layers.Selection_SingleLayer)
        zone = treeView_Layers.Selection_Layers[0].ParentZone;
      LayerCollection layers = zone != null ? zone.Layers : EditorManager.Scene.Layers;
      return layers;
    }

    private void toolStripButton_SortAZUp_Click(object sender, EventArgs e)
    {
      if (treeView_Layers.Selection_SingleLayer)
      {
        LayerCollection layers = GetSortingRelevantLayers();
        ChangeLayerOrderAction.SortAlphabetically(layers, SortOrder.Ascending);
      }
      else if (treeView_Layers.Selection_SingleZone)
      {
        ZoneCollection zones = EditorManager.Scene.Zones;
        ChangeZoneOrderAction.SortAlphabetically(zones, SortOrder.Ascending);
      }
    }

    private void toolStripButton_SortAZDown_Click(object sender, EventArgs e)
    {
      if (treeView_Layers.Selection_SingleLayer)
      {
        LayerCollection layers = GetSortingRelevantLayers();
        ChangeLayerOrderAction.SortAlphabetically(layers, SortOrder.Descending);
      }
      else if (treeView_Layers.Selection_SingleZone)
      {
        ZoneCollection zones = EditorManager.Scene.Zones;
        ChangeZoneOrderAction.SortAlphabetically(zones, SortOrder.Descending);
      }
    }

    private void toolStripButton_LockUpdate_Click(object sender, EventArgs e)
    {
      Cursor.Current = Cursors.WaitCursor;
      EditorManager.Scene.UpdateLayerLocks();
      Cursor.Current = Cursors.Default;
    }

    private void toolStripButton_FileRefresh_Click(object sender, EventArgs e)
    {
      if (EditorManager.Scene != null)
        EditorApp.Scene.UpdateLayers(); // create engine instances
    }

    private void toolStripButton_ImportReference_Click(object sender, EventArgs e)
    {
      if (EditorManager.Scene == null)
        return;

      OpenFileDlg dlg = new OpenFileDlg("Import Layer from external scene", "Import a .layer file from another scene in the same project", null, true, new string[1] { ".layer" });
      if (dlg.ShowDialog() != DialogResult.OK)
        return;
      string[] absNames = dlg.FileNames;
      StringCollection nameList = new StringCollection();
      foreach (string name in absNames)
        nameList.Add(name);
      StringCollection errorList = new StringCollection();
      EditorApp.Scene.ImportLayerReferences(nameList, errorList);

      if (errorList.Count > 0)
      {
        string error = "Error while importing the following layers as a refernces:\n\n";
        foreach (string s in errorList)
          error += "  - " + s + "\n";
        EditorManager.ShowMessageBox(error, "Error loading references", MessageBoxButtons.OK, MessageBoxIcon.Error);
      }


    }

    bool _layerInfoPanelVisible = true;

    /// <summary>
    /// gets or sets thre status of the update panel. This panel contains the the "Update" and "Ignore"
    /// button
    /// </summary>
    public bool UpdatedLayerInfoPanelVisible
    {
      get {return _layerInfoPanelVisible;}
      set
      {
        if (_layerInfoPanelVisible==value)
          return;
        _layerInfoPanelVisible = value;
        panel_UpdateLayer.Visible = value;
      }
    }

    private void button_LayerUpdate_Click(object sender, System.EventArgs e)
    {
      EditorApp.Scene.UpdateLayers();
      UpdatedLayerInfoPanelVisible = false;
    }

    private void button_UpdateIgnore_Click(object sender, System.EventArgs e)
    {
      UpdatedLayerInfoPanelVisible = false;
    }

    #endregion

    #region Event Listeners
    
    /// <summary>
    /// Listener for the OnSceneChanged event
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void EditorManager_SceneChanged(object sender, SceneChangedArgs e)
    {
      UpdateLayerToolbar();
      UpdatedLayerInfoPanelVisible = false;
    }

    /// <summary>
    /// Listens for the OnLayerChanged event
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void EditorScene_LayerChanged(object sender, LayerChangedArgs e)
    {
      if (EditorManager.GUI.UIUpdateLock > 0)
        return;

      UpdateLayerToolbar();

      bool needInvoke = this.InvokeRequired;

      if (e.action == LayerChangedArgs.Action.ContentChanged)
        UpdatedLayerInfoPanelVisible = true;
    }

    #endregion

    private void toolStripButton_DeleteShape_Click(object sender, EventArgs e)
    {
      shapeTreeView.OnDelete();
    }

    private void toolStripButton_AddGroup_Click(object sender, EventArgs e)
    {
      Layer layer = EditorManager.Scene.ActiveLayer;
      if (layer != null)
      {
        Group3DShape group = new Group3DShape("Group");
        EditorManager.Actions.Add(AddShapeAction.CreateAddShapeAction(group, layer.ActiveShape, layer, true));
      } 
    }

    private void toolStripButton_SortShapesAZUp_Click(object sender, EventArgs e)
    {
      SortShapes(SortOrder.Ascending);
    }

    private void toolStripButton_SortShapesAZDown_Click(object sender, EventArgs e)
    {
      SortShapes(SortOrder.Descending);
    }

    private void toolStripSplitButton_MoveToLayer_ButtonClick(object sender, EventArgs e)
    {
      toolStripSplitButton_MoveToLayer.ShowDropDown();
    }

    private void toolStripSplitButton_MoveToLayer_DropDownOpening(object sender, EventArgs e)
    {
      bool bHasScene = EditorManager.Scene != null;
      ShapeCollection shapes = shapeTreeView.SelectedShapes;

      toolStripSplitButton_MoveToLayer.DropDownItems.Clear();
      // add all layers to the dropdownlist
      if (bHasScene && shapes.Count > 0)
      {
        foreach (Layer layer in EditorManager.Scene.Layers)
          if (layer.Modifiable && layer.Loaded && layer != EditorManager.Scene.ActiveLayer)
          {
            ToolStripItem item = toolStripSplitButton_MoveToLayer.DropDownItems.Add(layer.LayerName);
            item.Tag = layer;
            item.Click += new EventHandler(ShapeMoveToLayerItem_Click);
          }
      }
    }

    private void toolStripButton_ShapeMoveDown_Click(object sender, EventArgs e)
    {
      SortSelectedShapes(MoveShapesDirection.Down);
    }

    private void toolStripButton_ShapeMoveUp_Click(object sender, EventArgs e)
    {
      SortSelectedShapes(MoveShapesDirection.Up);
    }

    private void toolStripSplitButton_ToolsOptions_DropDownOpening(object sender, EventArgs e)
    {
      toolStripSplitButton_AutomaticFocus.Checked = EditorManager.Settings.FocusOnSelection;
      toolStripSplitButton_PropertiesOnSelection.Checked = EditorManager.Settings.ShowPropertiesPanelOnSelection;
    }

    private void toolStripSplitButton_AutomaticFocus_Click(object sender, EventArgs e)
    {
      EditorManager.Settings.FocusOnSelection = toolStripSplitButton_AutomaticFocus.Checked;
    }

    private void toolStripSplitButton_PropertiesOnSelection_Click(object sender, EventArgs e)
    {
      EditorManager.Settings.ShowPropertiesPanelOnSelection = toolStripSplitButton_PropertiesOnSelection.Checked;
    }

    private void SortSelectedShapes(MoveShapesDirection moveDirection)
    {
      ShapeCollection selected = shapeTreeView.SelectedShapes;

      // return if any of the indices is already on the maximum.
      foreach (ShapeBase shape in selected)
      {
        if (moveDirection == MoveShapesDirection.Up)
        {
          if (shape.Parent.ChildCollection.FindIndex(i => i == shape) <= 0)
            return;
        }
        else
          if (shape.Parent.ChildCollection.FindIndex(i => i == shape) >= shape.Parent.ChildCollection.Count - 1)
            return;
      }

      // get all parents to share modified collections between their children.
      ShapeCollection parents = new ShapeCollection();
      foreach (ShapeBase shape in selected)
        if (!parents.Contains(shape.Parent))
          parents.Add(shape.Parent);

      EditorManager.Actions.StartGroup("Sort Shapes");
      foreach (ShapeBase parent in parents)
      {
        // create copy of the original collection before sorting
        ShapeCollection copyOfChildren = new ShapeCollection();
        copyOfChildren.AddRange(parent.ChildCollection);

        if (moveDirection == MoveShapesDirection.Up)
        {
          for (int i = 0; i < selected.Count; i++)
          {
            ShapeBase child = selected[i];
            if (child.Parent == parent)
            {
              int index = copyOfChildren.FindIndex(c => c == child);
              copyOfChildren.Remove(child);
              copyOfChildren.Insert(index - 1, child);
              EditorManager.Actions.Add(new SortShapeChildrenAction(parent, copyOfChildren));
            }
          }
        }
        else
          for (int i = selected.Count - 1; i > -1; i--)
          {
            ShapeBase child = selected[i];
            if (child.Parent == parent)
            {
              int index = copyOfChildren.FindIndex(c => c == child);
              copyOfChildren.Remove(child);
              copyOfChildren.Insert(index + 1, child);
              EditorManager.Actions.Add(new SortShapeChildrenAction(parent, copyOfChildren));
            }
          }
      }
      EditorManager.Actions.EndGroup();

      // recover selection
      ArrayList newSelection = new ArrayList();
      foreach (ShapeTreeNode node in shapeTreeView.Nodes)
      {
        if (selected.Contains(node.shape)) // root
          newSelection.Add(node);
        foreach (ShapeTreeNode subNode in shapeTreeView.GetChildNodes(node))
        {
          if (selected.Contains(subNode.shape)) // all children
            newSelection.Add(subNode);
        }
      }
      shapeTreeView.SelectedNodes = newSelection;
    }

    private enum MoveShapesDirection
    {
      Up,
      Down
    }
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
