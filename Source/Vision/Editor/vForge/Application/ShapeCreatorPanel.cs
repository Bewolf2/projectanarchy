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
using CSharpFramework;
using CSharpFramework.Scene;
using CSharpFramework.Shapes;
using CSharpFramework.View;
using CSharpFramework.Docking;
using WeifenLuo.WinFormsUI;
using Editor.Shapes;
using System.IO;
using CSharpFramework.Dialogs;
using CSharpFramework.Controls;
using CSharpFramework.Helper;

namespace Editor
{
  #region Class ShapeCreatorPanel

  /// <summary>
	/// This panel has a tree view that allows to drag creator objects onto the engine view.
	/// The engine view is responsible for creating an instance in the view. By default, all creator
	/// plugins show up in this panel, but the Vision engine view already allows to drag Shape instances
	/// into it.
	/// </summary>
	public class ShapeCreatorPanel : DockableForm
	{
    #region Nested Class ShapeCreatorTreeNode

    /// <summary>
    /// Tree node item for the creator tree
    /// </summary>
    public class ShapeCreatorTreeNode : TreeNode
    {
      /// <summary>
      /// Constructor for a category
      /// </summary>
      /// <param name="nodename"></param>
      /// <param name="iIcon"></param>
      public ShapeCreatorTreeNode(string nodename, int iIcon) : base(nodename,iIcon,iIcon)
      {
      }

      /// <summary>
      /// Constructor for creator node. The object can be of any type that is supported by the engine view as a drop target.
      /// </summary>
      /// <param name="nodename"></param>
      /// <param name="creator"></param>
      /// <param name="iIcon"></param>
      public ShapeCreatorTreeNode(string nodename, object creator, int iIcon) : base(nodename,iIcon,iIcon)
      {
        CreatorObject = creator;
      }

      /// <summary>
      /// Indicates whether this node is a category or a creator
      /// </summary>
      public bool IsCategory {get {return CreatorObject==null;}}

      /// <summary>
      /// The creator object (passed in the constructor)
      /// </summary>
      public object CreatorObject = null;
    }

    #endregion

    private System.Windows.Forms.Label label_Description;
    private System.Windows.Forms.Splitter splitter1;
    private IncrementalSearchPanel searchPanel;

    private TreeNode[] nodeState = null;
    private Panel panel_Search;
    private TreeView treeView_Creators;
    private Button button_CollapseAll;
    private Button button_ExpandAll;
    private Button recentShapesButton;
    private ContextMenuStrip recentShapesContextMenu;
    private ToolStripMenuItem recentEntriesToolStripMenuItem;
    private ToolTip toolTip1;
    private IContainer components;

    /// <summary>
    /// Constructor
    /// </summary>
		public ShapeCreatorPanel(DockingContainer container) : base(container)
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();
      label_Description.Text = null;

      IProject.NewProjectLoaded += new EventHandler(IProject_NewProjectLoaded);
      IProject.ProjectUnloaded += new EventHandler(IProject_ProjectUnloaded);
      PrefabDesc.OnPrefabSaved += new EventHandler(PrefabDesc_OnPrefabSaved);
      treeView_Creators.ImageList = TreeViewImages.ImageList;

      // Incremental search
      searchPanel.FilterChanged += new EventHandler(searchPanel_FilterChanged);

      // Used for UI updates in case the prefab directory changes (to update view of available prefabs)
      EditorManager.EditorSettingsChanged += new EditorSettingsChangedEventHandler(EditorManager_EditorSettingsChanged);

      // UI updates of ShapeCreatorPanel can be triggered from the EditorManager.GUI as desired (e.g. when ShapeCreators
      // have been added in custom plugins from e.g. some scene callback and they should be available in the list)
      EditorManager.GUI.ShapeCreatorUpdateRequest += new EventHandler(EditorManager_GUI_ShapeCreatorUpdateRequest);
		}

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    protected override void Dispose(bool disposing)
    {
      if (disposing)
      {
        IProject.NewProjectLoaded -= new EventHandler(IProject_NewProjectLoaded);
        IProject.ProjectUnloaded -= new EventHandler(IProject_ProjectUnloaded);
        PrefabDesc.OnPrefabSaved -= new EventHandler(PrefabDesc_OnPrefabSaved);

        searchPanel.FilterChanged -= new EventHandler(searchPanel_FilterChanged);
        EditorManager.EditorSettingsChanged -= new EditorSettingsChangedEventHandler(EditorManager_EditorSettingsChanged);
        EditorManager.GUI.ShapeCreatorUpdateRequest -= new EventHandler(EditorManager_GUI_ShapeCreatorUpdateRequest);
        if (components != null)
        {
          components.Dispose();
        }
      }
      base.Dispose(disposing);
    }

    /// <summary>
    /// Private constructor. Necessary to get this form properly shown in the designer when deriving from it.
    /// See ticket #1067 and http://stackoverflow.com/questions/1216940/
    /// </summary>
    private ShapeCreatorPanel()
      : base(null)
    {
      InitializeComponent();
    }

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      this.components = new System.ComponentModel.Container();
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ShapeCreatorPanel));
      this.label_Description = new System.Windows.Forms.Label();
      this.splitter1 = new System.Windows.Forms.Splitter();
      this.searchPanel = new CSharpFramework.Controls.IncrementalSearchPanel();
      this.panel_Search = new System.Windows.Forms.Panel();
      this.recentShapesButton = new System.Windows.Forms.Button();
      this.button_ExpandAll = new System.Windows.Forms.Button();
      this.button_CollapseAll = new System.Windows.Forms.Button();
      this.treeView_Creators = new System.Windows.Forms.TreeView();
      this.recentShapesContextMenu = new System.Windows.Forms.ContextMenuStrip(this.components);
      this.recentEntriesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
      this.panel_Search.SuspendLayout();
      this.recentShapesContextMenu.SuspendLayout();
      this.SuspendLayout();
      // 
      // label_Description
      // 
      this.label_Description.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.label_Description.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.label_Description.Location = new System.Drawing.Point(0, 506);
      this.label_Description.Name = "label_Description";
      this.label_Description.Size = new System.Drawing.Size(324, 44);
      this.label_Description.TabIndex = 1;
      this.label_Description.Text = "Description goes here";
      // 
      // splitter1
      // 
      this.splitter1.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.splitter1.Location = new System.Drawing.Point(0, 504);
      this.splitter1.Name = "splitter1";
      this.splitter1.Size = new System.Drawing.Size(324, 2);
      this.splitter1.TabIndex = 2;
      this.splitter1.TabStop = false;
      // 
      // searchPanel
      // 
      this.searchPanel.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.searchPanel.Enabled = false;
      this.searchPanel.Location = new System.Drawing.Point(38, 4);
      this.searchPanel.Name = "searchPanel";
      this.searchPanel.Size = new System.Drawing.Size(212, 26);
      this.searchPanel.TabIndex = 1;
      // 
      // panel_Search
      // 
      this.panel_Search.Controls.Add(this.recentShapesButton);
      this.panel_Search.Controls.Add(this.button_ExpandAll);
      this.panel_Search.Controls.Add(this.button_CollapseAll);
      this.panel_Search.Controls.Add(this.searchPanel);
      this.panel_Search.Dock = System.Windows.Forms.DockStyle.Top;
      this.panel_Search.Location = new System.Drawing.Point(0, 0);
      this.panel_Search.Name = "panel_Search";
      this.panel_Search.Padding = new System.Windows.Forms.Padding(4);
      this.panel_Search.Size = new System.Drawing.Size(324, 32);
      this.panel_Search.TabIndex = 0;
      // 
      // recentShapesButton
      // 
      this.recentShapesButton.Enabled = false;
      this.recentShapesButton.Image = global::Editor.Properties.Resources.pawn_new;
      this.recentShapesButton.Location = new System.Drawing.Point(7, 6);
      this.recentShapesButton.Name = "recentShapesButton";
      this.recentShapesButton.Size = new System.Drawing.Size(29, 21);
      this.recentShapesButton.TabIndex = 0;
      this.toolTip1.SetToolTip(this.recentShapesButton, "Recently Used Shapes");
      this.recentShapesButton.UseVisualStyleBackColor = true;
      this.recentShapesButton.Click += new System.EventHandler(this.recentShapesButton_Click);
      // 
      // button_ExpandAll
      // 
      this.button_ExpandAll.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.button_ExpandAll.Enabled = false;
      this.button_ExpandAll.Image = global::Editor.Properties.Resources.expand_all;
      this.button_ExpandAll.Location = new System.Drawing.Point(254, 5);
      this.button_ExpandAll.Name = "button_ExpandAll";
      this.button_ExpandAll.Size = new System.Drawing.Size(28, 21);
      this.button_ExpandAll.TabIndex = 2;
      this.toolTip1.SetToolTip(this.button_ExpandAll, "Expand All");
      this.button_ExpandAll.UseVisualStyleBackColor = true;
      this.button_ExpandAll.Click += new System.EventHandler(this.button_ExpandAll_Click);
      // 
      // button_CollapseAll
      // 
      this.button_CollapseAll.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.button_CollapseAll.Enabled = false;
      this.button_CollapseAll.Image = global::Editor.Properties.Resources.collapse_all;
      this.button_CollapseAll.Location = new System.Drawing.Point(288, 5);
      this.button_CollapseAll.Name = "button_CollapseAll";
      this.button_CollapseAll.Size = new System.Drawing.Size(28, 21);
      this.button_CollapseAll.TabIndex = 3;
      this.toolTip1.SetToolTip(this.button_CollapseAll, "Collapse All");
      this.button_CollapseAll.UseVisualStyleBackColor = true;
      this.button_CollapseAll.Click += new System.EventHandler(this.button_CollapseAll_Click);
      // 
      // treeView_Creators
      // 
      this.treeView_Creators.Dock = System.Windows.Forms.DockStyle.Fill;
      this.treeView_Creators.Location = new System.Drawing.Point(0, 32);
      this.treeView_Creators.Name = "treeView_Creators";
      this.treeView_Creators.Size = new System.Drawing.Size(324, 472);
      this.treeView_Creators.TabIndex = 1;
      this.treeView_Creators.NodeMouseDoubleClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.treeView_Creators_NodeMouseDoubleClick);
      this.treeView_Creators.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.treeView_Creators_AfterSelect);
      this.treeView_Creators.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.treeView_Creators_NodeMouseClick);
      this.treeView_Creators.KeyDown += new System.Windows.Forms.KeyEventHandler(this.treeView_Creators_KeyDown);
      this.treeView_Creators.ItemDrag += new System.Windows.Forms.ItemDragEventHandler(this.treeView_Creators_ItemDrag);
      // 
      // recentShapesContextMenu
      // 
      this.recentShapesContextMenu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.recentEntriesToolStripMenuItem});
      this.recentShapesContextMenu.Name = "contextMenuStrip1";
      this.recentShapesContextMenu.Size = new System.Drawing.Size(162, 26);
      // 
      // recentEntriesToolStripMenuItem
      // 
      this.recentEntriesToolStripMenuItem.Name = "recentEntriesToolStripMenuItem";
      this.recentEntriesToolStripMenuItem.Size = new System.Drawing.Size(161, 22);
      this.recentEntriesToolStripMenuItem.Text = "<recent entries>";
      // 
      // ShapeCreatorPanel
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(324, 550);
      this.Controls.Add(this.treeView_Creators);
      this.Controls.Add(this.panel_Search);
      this.Controls.Add(this.splitter1);
      this.Controls.Add(this.label_Description);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.Name = "ShapeCreatorPanel";
      this.TabText = "Shape Creators";
      this.Text = "Shape Creators";
      this.panel_Search.ResumeLayout(false);
      this.recentShapesContextMenu.ResumeLayout(false);
      this.ResumeLayout(false);

    }

		#endregion

    #region Build tree functions

    /// <summary>
    /// Clears all nodes in the tree view
    /// </summary>
    public void ClearTree()
    {
      this.treeView_Creators.Nodes.Clear();
      _selectedCreatorNode = null;
      SelectedCreatorObject = null;
    }

    /// <summary>
    /// Wraps around BeginUpdate of the tree view
    /// </summary>
    public void BeginAddCreators()
    {
      treeView_Creators.BeginUpdate();
    }

    /// <summary>
    /// Wraps around EndUpdate of the tree view
    /// </summary>
    public void EndAddCreators()
    {
      treeView_Creators.EndUpdate();
    }


    /// <summary>
    /// Adds a new creator to the tree. The creator object can be any type that is supported in the View OnDrop function
    /// </summary>
    /// <param name="parent"></param>
    /// <param name="creator"></param>
    /// <param name="nodename"></param>
    /// <param name="iIconIndex"></param>
    /// <returns></returns>
    public ShapeCreatorTreeNode AddCreator(ShapeCreatorTreeNode parent, object creator, string nodename, int iIconIndex)
    {
      TreeNodeCollection nodes = (parent==null) ? treeView_Creators.Nodes : parent.Nodes;

      ShapeCreatorTreeNode node = new ShapeCreatorTreeNode(nodename,creator,iIconIndex);
      nodes.Add(node);
      
      return node;
    }

    /// <summary>
    /// Adds a new category to the tree. Tries o find an existing category
    /// </summary>
    /// <param name="parent"></param>
    /// <param name="nodename"></param>
    /// <param name="iIconIndex"></param>
    /// <returns></returns>
    public ShapeCreatorTreeNode AddCategory(ShapeCreatorTreeNode parent, string nodename, int iIconIndex)
    {
      TreeNodeCollection nodes = (parent==null) ? treeView_Creators.Nodes : parent.Nodes;

      if (iIconIndex<0) // use the folder icon
        iIconIndex = GroupShape.GroupIconIndex;

      // find existing category
      string compare = nodename.ToLower();
      foreach (ShapeCreatorTreeNode child in nodes)
        if (child.Text.ToLower()==compare)
          return child;

      ShapeCreatorTreeNode node = new ShapeCreatorTreeNode(nodename,iIconIndex);
      nodes.Add(node);

      return node;
    }

    /// <summary>
    /// This helper function looks for a specific TreeNode with the given text
    /// in the passed TreeNode or its child nodes.
    /// </summary>
    /// <param name="parent"> Parent TreeNode </param>
    /// <param name="nodename"> Node Text to look for</param>
    /// <returns> The TreeNode if found, otherwise NULL </returns>
    private TreeNode FindCategoryTreeNode(TreeNode parent, string nodetext)
    {
      if (parent != null && parent.Text.Equals(nodetext))
        return parent;

      foreach (TreeNode node in parent.Nodes)
      {
        if (node.Nodes != null && node.Nodes.Count > 0)
        {
          if (node.Text.Equals(nodetext))
            return node;
          else
            FindCategoryTreeNode(node, nodetext);
        }
      }

      return null;
    }

    /// <summary>
    /// Stores the collapsed state of each node in the tree view. Basically a
    /// copy of the tree is kept to compare against when rebuilding the tree.
    /// </summary>
    private void StoreCollapsedState()
    {
      if (this.treeView_Creators.Nodes == null || searchPanel.IsActive)
        return;

      // Save Collapse State of TreeView
      nodeState = new TreeNode[this.treeView_Creators.Nodes.Count];
      this.treeView_Creators.Nodes.CopyTo(nodeState, 0);
    }

    /// <summary>
    /// This helper function restores the collapsed state of the given TreeNodeCollection 
    /// recursively. We must do this after the complete Tree has been build as we can not
    /// expand/collapse parent nodes when they have no children.
    /// </summary>
    /// <param name="nodes"> Collection of TreeNodes </param>
    private void RestoreCollapsedState(TreeNodeCollection nodes)
    {
      if (nodes == null || nodes.Count == 0)
        return;

      foreach (TreeNode n1 in nodes)
      {
        if (n1.Nodes != null && n1.Nodes.Count > 0)
        {
          // Restore children recursively
          RestoreCollapsedState(n1.Nodes);

          // Check this node against stored nodes
          foreach (TreeNode n2 in nodeState)
          {
            TreeNode found = FindCategoryTreeNode(n2, n1.Text);
            if (found != null)
            {
              if (found.IsExpanded)
                n1.Expand();
              else
                n1.Collapse();
              break;
            }
          }
        }
      }
    }

    /// <summary>
    /// This version adds multiple nodes into the tree id the specified path contains back slashes
    /// </summary>
    /// <param name="parent"></param>
    /// <param name="path"></param>
    /// <param name="separator"></param>
    /// <param name="iIconIndex"></param>
    /// <returns></returns>
    public ShapeCreatorTreeNode AddCategoryPath(ShapeCreatorTreeNode parent, string path, string separator, int iIconIndex)
    {
      string[] folders = path.Split(separator.ToCharArray());
      foreach (string folder in folders)
        parent = AddCategory(parent, folder, iIconIndex);
      return parent;
    }

    /// <summary>
    /// Returns the image list that is used for this tree view
    /// </summary>
    public GUI.DynamicImageList TreeViewImages
    {
      get {return EditorManager.GUI.ShapeTreeImages;}
    }

    /// <summary>
    /// Updates all project related creators, this function is called by the project update event    
    /// </summary>    
    protected void ProjectUpdate()
    {
      BeginAddCreators();
      StoreCollapsedState();
      ClearTree();
      EditorProject project = EditorApp.Project;
      if (project==null)
      {
        EndAddCreators();
        return;
      }

      int iCategoryIcon = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\folder_new.png"), Color.Magenta);

      // add all creator plugins in this project
      ShapeCreatorTreeNode catCreators = this.AddCategory(null, "Shape Creators", iCategoryIcon);
      IEditorPluginList list = EditorManager.SortedShapeCreatorPlugins;
      foreach (IShapeCreatorPlugin plugin in list)
      {
        if (!plugin.IsVisible())
          continue;

        // Apply the search filter
        if (!searchPanel.MatchesFilter(plugin.GetPluginName()))
          continue;

        ShapeCreatorTreeNode catPlugin = catCreators;
        string catName = plugin.GetPluginCategory();
        if (catName!=null && catName.Length>0)
          catPlugin = AddCategoryPath(catCreators, catName, "\\", -1);

        AddCreator(catPlugin,plugin,plugin.GetPluginName(),plugin.IconIndex);
      }

      string prefabDir = project.MakeAbsolute(EditorManager.Settings.PrefabDirectory);
      if (Directory.Exists(prefabDir))
      {

        // Add prefabs from project directory:
        try
        {
          AddPrefabCreators(prefabDir, null);
        }
        catch (Exception ex)
        {
          EditorManager.DumpException(ex);
        }
      }
      
      // Expand all if no collapsed state was restored before, otherwise restore old one.
      // We can not expand/collapse the TreeNodes at creation time as they have no children
      // assigned when they are created.
      if (nodeState == null || nodeState.Length == 0 || searchPanel.IsActive)
        treeView_Creators.ExpandAll();
      else
        RestoreCollapsedState(treeView_Creators.Nodes);
      
      EndAddCreators();
    }


    private void IProject_NewProjectLoaded(object sender, EventArgs e)
    {
      this.ProjectUpdate();
      searchPanel.Enabled = true;
      button_CollapseAll.Enabled = true;
      button_ExpandAll.Enabled = true;
      recentShapesButton.Enabled = true;
      ClearRecentShapesContextMenu();
    }

    private void IProject_ProjectUnloaded(object sender, EventArgs e)
    {
      this.ClearTree();
      searchPanel.Enabled = false;
      button_CollapseAll.Enabled = false;
      button_ExpandAll.Enabled = false;
      recentShapesButton.Enabled = false;
      ClearRecentShapesContextMenu();
    }

    void EditorManager_EditorSettingsChanged(object sender, EditorSettingsChangedArgs e)
    {
      // Update UI because prefab directory might have changed
      if (e.OldSettings.PrefabDirectory != e.NewSettings.PrefabDirectory)
        ProjectUpdate();
    }

    void EditorManager_GUI_ShapeCreatorUpdateRequest(object sender, EventArgs e)
    {
      ProjectUpdate();
    }

    void PrefabDesc_OnPrefabSaved(object sender, EventArgs e)
    {
      this.ProjectUpdate();
    }

    #endregion

    #region Drag & Drop

    private void treeView_Creators_ItemDrag(object sender, System.Windows.Forms.ItemDragEventArgs e)
    {
      ShapeCreatorTreeNode node = (ShapeCreatorTreeNode)e.Item;
      if (node.CreatorObject==null)
        return;

      // start dragging the creator object
      DoDragDrop(node.CreatorObject, DragDropEffects.Copy|DragDropEffects.Scroll);

      UpdateRecentShapesList(node.CreatorObject); // create item, even if dragdrop was cancelled.
    }

    #endregion

    #region Tree View callbacks

    ShapeCreatorTreeNode _selectedCreatorNode = null;

    private void treeView_Creators_AfterSelect(object sender, System.Windows.Forms.TreeViewEventArgs e)
    {
      _selectedCreatorNode = e.Node as ShapeCreatorTreeNode;
      if (_selectedCreatorNode == null)
        return;
      SelectedCreatorObject = _selectedCreatorNode.CreatorObject;
    }

    private void treeView_Creators_NodeMouseDoubleClick(object sender, TreeNodeMouseClickEventArgs e)
    {
      PrefabDesc prefab = SelectedCreatorObject as PrefabDesc;
      if (prefab != null)
      {
        using (PropertyGridDlg dlg = new PropertyGridDlg("Change prefab properties", "Change the properties of this prefab and click OK to apply and save the new settings"))
        {
          dlg.DataObject = prefab;
          if (dlg.ShowDialog() != DialogResult.OK)
            return;

          // get back results
          prefab = (PrefabDesc)dlg.DataObject;
          if (!prefab.PropertiesChanged) // seems untouched by property grid
            return;

          // fire the static event
          prefab.TriggerPropertiesChangedEvent();
          if (!prefab.SaveToFile(null))
          {
            EditorManager.ShowMessageBox("Changes could not be applied because the prefab shape could not be saved.\n\nDetailed message:" + prefab.LastError, "Failed to save prefab", MessageBoxButtons.OK, MessageBoxIcon.Error);
            return;
          }


          //_selectedCreatorNode.CreatorObject = prefab;
          // brute force rebuild tree:
          ProjectUpdate();
        }
      }
    }

    private void button_CollapseAll_Click(object sender, EventArgs e)
    {
      treeView_Creators.CollapseAll();
    }

    private void button_ExpandAll_Click(object sender, EventArgs e)
    {
      treeView_Creators.ExpandAll();
    }

    private void treeView_Creators_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
    {
      _selectedCreatorNode = e.Node as ShapeCreatorTreeNode;
      if (_selectedCreatorNode == null)
        return;
      SelectedCreatorObject = _selectedCreatorNode.CreatorObject;
      treeView_Creators.SelectedNode = e.Node; 
    }

    #endregion

    #region Selected Creator Object

    object _selectedCreatorObject = null;
    static string NO_DESCRIPTION = "<No description available>";

    /// <summary>
    /// get or set the creator object
    /// </summary>
    public object SelectedCreatorObject
    {
      get {return _selectedCreatorObject;}
      set
      {
        if (_selectedCreatorObject == value)
          return;
        _selectedCreatorObject = value;

        // fire event
        if (SelectedCreatorObjectChanged!=null)
          SelectedCreatorObjectChanged(this,EventArgs.Empty);

        // update description text
        string _desc = null;
        IEditorPlugin plugin = _selectedCreatorObject as IEditorPlugin;
        if (plugin!=null)
        {
          _desc = plugin.GetPluginDescription();
          if (string.IsNullOrEmpty(_desc))
            _desc = NO_DESCRIPTION;
        }

        PrefabDesc prefab = _selectedCreatorObject as PrefabDesc;
        if (prefab != null)
        {
          _desc = prefab.Description;
          if (string.IsNullOrEmpty(_desc))
            _desc = NO_DESCRIPTION;
        }

        this.label_Description.Text = _desc;
      }
    }

    /// <summary>
    /// This event gets fired when the object in SelectedCreatorObject changed
    /// </summary>
    public event EventHandler SelectedCreatorObjectChanged = null;

    #endregion

    #region Incremental Search

    void searchPanel_FilterChanged(object sender, EventArgs e)
    {
      ProjectUpdate();
      treeView_Creators.ExpandAll();
    }


    /// <summary>
    /// Implements natural sorting for filenames. This class is used in AddPrefabCreators
    /// </summary>
    private class NaturalFileNameComparer : IComparer
    {
      public int Compare(object objA, object objB)
      {
        string filenameA = Path.GetFileNameWithoutExtension(objA.ToString());
        string filenameB = Path.GetFileNameWithoutExtension(objB.ToString());
        return StringHelper.CompareStringsNaturally(filenameA, filenameB);
      }
    }

    void AddPrefabCreators(string dir, ShapeCreatorTreeNode parent)
    {
      EditorProject project = EditorApp.Project;
      int iIcon = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\lock_ok.png"), Color.Magenta);
      int iCategoryIcon = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\folder_new.png"), Color.Magenta);

      // Create the prefab category, if still missing
      ShapeCreatorTreeNode catParent = null;
      if (parent == null)
        catParent = this.AddCategory(null, "Prefabs", iCategoryIcon);
      else
        catParent = this.AddCategory(parent, dir.Substring(dir.LastIndexOf('\\')), iCategoryIcon);

      // Iterate all subdirectories
      string[] directories = Directory.GetDirectories(dir);
      foreach (string directory in directories)
        AddPrefabCreators(directory, catParent);

      // Iterate all files        
      string[] files = Directory.GetFiles(dir, "*.prefab");
      Array.Sort(files, new NaturalFileNameComparer());

      foreach (string filename in files)
      {
        string relname = project.MakeRelative(filename);
        PrefabDesc desc = PrefabManager.CreatePrefab(relname);
        if (!desc.Loaded)
          continue;

        // Get the name of the prefab
        string _name = desc.Name;
        if (_name == null || _name == "")
          _name = relname;

        // Apply the search filter
        if (!searchPanel.MatchesFilter(_name))
          continue;

        // Add the category path to the tree
        ShapeCreatorTreeNode cat = catParent;
        string catName = desc.Category;
        if (catName != null && catName != "")
          cat = AddCategoryPath(catParent, catName, "\\", -1);

        AddCreator(cat, desc, _name, iIcon);
      }

      // Check whether any prefab creators has been added
      if (catParent.Nodes.Count == 0)
        catParent.Remove();
    } 
 
    #endregion

    #region Recent Shapes Button

    private bool _startDragDrop = false;

    private void recentShapesButton_Click(object sender, EventArgs e)
    {
      recentShapesContextMenu.Show(recentShapesButton, new Point(0, recentShapesButton.Height));
    }

    private void UpdateRecentShapesList(object creatorObj)
    {
      const int maximum = 8;

      //Prefabs cannot be casted to IShapeCreatorPlugin. So we have to check first if the cast is possible to avoid an exception. [#4743]
      IShapeCreatorPlugin shapeCreatorPlugin = creatorObj as IShapeCreatorPlugin;
      if (shapeCreatorPlugin == null)
      {
        return;
      }
      CSharpFramework.EditorManager.PluginToolStripMenuItem shapeItem = new CSharpFramework.EditorManager.PluginToolStripMenuItem(shapeCreatorPlugin.Name, (IEditorPlugin)creatorObj);
      // return if item exists
      foreach (CSharpFramework.EditorManager.PluginToolStripMenuItem item in recentShapesContextMenu.Items)
        if (item.m_plugin.Name == shapeItem.m_plugin.Name)
        {
          // add to top
          recentShapesContextMenu.Items.Insert(0, item);
          return;
        }
      // remove first if maximum is reached
      if (recentShapesContextMenu.Items.Count >= maximum)
      {
        ToolStripItem toRemove = recentShapesContextMenu.Items[maximum-1];
        toRemove.MouseDown -= shapeItem_MouseDown;
        toRemove.MouseUp -= shapeItem_MouseUp;
        toRemove.MouseLeave -= shapeItem_MouseLeave;
        recentShapesContextMenu.Items.Remove(toRemove);
      }
        
      // add new
      recentShapesContextMenu.Items.Insert(0, shapeItem);
      shapeItem.MouseDown += new MouseEventHandler(shapeItem_MouseDown);
      shapeItem.MouseLeave += new EventHandler(shapeItem_MouseLeave);
      shapeItem.MouseUp += new MouseEventHandler(shapeItem_MouseUp);
    }

    private void shapeItem_MouseLeave(object sender, EventArgs e)
    {
      if (_startDragDrop)
      {
        _startDragDrop = false;
        DoDragDrop((sender as CSharpFramework.EditorManager.PluginToolStripMenuItem).m_plugin, DragDropEffects.Copy | DragDropEffects.Scroll);
        recentShapesContextMenu.Close();
      }
    }

    private void shapeItem_MouseDown(object sender, MouseEventArgs e)
    {
      _startDragDrop = true;
    }

    private void shapeItem_MouseUp(object sender, MouseEventArgs e)
    {
      _startDragDrop = false;
    }

    private void ClearRecentShapesContextMenu()
    {
      foreach (ToolStripItem item in recentShapesContextMenu.Items)
      {
        item.MouseDown -= shapeItem_MouseDown;
        item.MouseUp -= shapeItem_MouseUp;
        item.MouseLeave -= shapeItem_MouseLeave;
      }
      recentShapesContextMenu.Items.Clear();
    }

    #endregion

    #region Key Handling

    private void treeView_Creators_KeyDown(object sender, KeyEventArgs e)
    {
      EditorManager.EditorComponentKeyDown(null, e);
    }

    #endregion
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
