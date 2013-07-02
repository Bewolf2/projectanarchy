/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.IO;
using System.Collections;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Timers;
using System.Drawing.Design;
using System.ComponentModel;
using System.Globalization;
using CSharpFramework;
using CSharpFramework.Scene;
using CSharpFramework.Shapes;
using CSharpFramework.UndoRedo;
using CSharpFramework.Controls;
using CSharpFramework.Docking;
using CSharpFramework.Clipboard;
using WeifenLuo.WinFormsUI;
using CSharpFramework.PropertyEditors;
using System.Collections.Generic;
using CSharpFramework.Actions;

namespace Editor
{
  /// <summary>
  ///Panel that contains the Property Grid.
  /// </summary>
  public class PropertyPanel : DockableForm
  {

    #region Constructor

    delegate void RefreshCallback();

    private IContainer components = null;
    private PropertyGridEx propertyGrid;
    private System.Timers.Timer _refreshTimer = new System.Timers.Timer();
    private ToolStripButton ToolStripButtonExpand;
    private ToolStripButton ToolStripButtonCollapse;
    private ToolStripSeparator ToolStripSeparator;
    private ToolStripButton ToolStripButtonCopy;
    private ToolStripButton ToolStripButtonPaste;
    private ToolStripSeparator ToolStripSeparatorHelp;
    private DialogCaptionBar dialogCaptionBar;
    private ToolStripDropDownButton ToolStripButtonSettings;
    private ToolStripMenuItem ToolStripMenuItemHelp;
    private ToolStripMenuItem ToolStripMenuItemCaption;
    private ToolStripDropDownButton ToolStripButtonRecentSelection;
    private PropertyGridCollapseState _itemCollapseState;

    /// <summary>
    /// Constructor
    /// </summary>
    public PropertyPanel(DockingContainer container) : base(container)
    {
      // This call is required by the Windows.Forms Form Designer.
      InitializeComponent();

      // TODO: Add any initialization after the InitializeComponent call

      // configure the propertygrid refresh timer
      _refreshTimer.AutoReset = false;
      _refreshTimer.Elapsed += new ElapsedEventHandler(OnRefresh);
      _refreshTimer.Interval = 200;
      _refreshTimer.Enabled=false;

      // Note that this is a global setter (it should be called once) that will work for all property grids. 
      // It is set here as we only have one property grid in the code but it could be considered to move the call to a global settings location.
      SingleTypeDescriptionProvider propertyGridSingleProvider = new SingleTypeDescriptionProvider(TypeDescriptor.GetProvider(typeof(Single)));
      TypeDescriptor.AddProvider(propertyGridSingleProvider, typeof(Single));

      // listen to the changes
      EditorScene.PropertyChanged += new CSharpFramework.PropertyChangedEventHandler(this.OnPropertyChanged);
      EditorManager.SceneChanged += new SceneChangedEventHandler(this.OnSceneChanged);
      EditorScene.ShapeChanged += new ShapeChangedEventHandler(this.OnShapeChanged);
      EditorScene.LayerChanged += new LayerChangedEventHandler(this.OnLayerChanged);
      EditorScene.ZoneChanged += new ZoneChangedEventHandler(EditorScene_ZoneChanged);
      propertyGrid.PropertyValueChanged += new PropertyValueChangedEventHandler(propertyGrid_PropertyValueChanged);

      EditorManager.ShapeSelectionChanged += new ShapeSelectionChangedEventHandler(OnSelectionChanged);

      UpdateHelpPanel();
    }

    /// <summary>
    /// Private constructor. Necessary to get this form properly shown in the designer when deriving from it.
    /// See ticket #1067 and http://stackoverflow.com/questions/1216940/
    /// </summary>
    private PropertyPanel()
      : base(null)
    {
      InitializeComponent();
    }

    /// <summary> 
    /// Clean up any resources being used.
    /// </summary>
    protected override void Dispose(bool disposing)
    {
      if (disposing)
      {
        EditorScene.PropertyChanged -= new CSharpFramework.PropertyChangedEventHandler(this.OnPropertyChanged);
        EditorManager.SceneChanged -= new SceneChangedEventHandler(this.OnSceneChanged);
        EditorScene.ShapeChanged -= new ShapeChangedEventHandler(this.OnShapeChanged);
        EditorScene.LayerChanged -= new LayerChangedEventHandler(this.OnLayerChanged);
        EditorScene.ZoneChanged -= new ZoneChangedEventHandler(EditorScene_ZoneChanged);
        propertyGrid.PropertyValueChanged -= new PropertyValueChangedEventHandler(propertyGrid_PropertyValueChanged);

        EditorManager.ShapeSelectionChanged -= new ShapeSelectionChangedEventHandler(OnSelectionChanged);

        if (components != null)
        {
          components.Dispose();
        }
      }
      base.Dispose(disposing);
    }

    #endregion

    #region Helper

    /// <summary>
    /// Get a Grid Item of the PropertyControl by its label
    /// </summary>
    /// <remarks>you will run into trouble if a property has the same name as a category, for example, Appearance</remarks>
    /// <returns>The GridItem with the given label (null if none is found)</returns>
    public GridItem getGridItem(string propertyLabel)
    {
      // get the top most Grid Item of the property tree
      GridItem rootGridItem = propertyGrid.SelectedGridItem;
      while (rootGridItem.Parent != null)
      {
        rootGridItem = rootGridItem.Parent;
      }

      // for all found Grid Items
      ArrayList allGridItems = new ArrayList();
      allGridItems.Add(rootGridItem);
      while (allGridItems.Count != 0)
      {
        GridItem testGridItem = (GridItem)allGridItems[0];
        allGridItems.RemoveAt(0);

        // check if we already found the right one
        if ( testGridItem.Label == propertyLabel)
        {
          // success
          return testGridItem;
        }

        // if we are not done yet
        foreach (GridItem newGridItem in testGridItem.GridItems)
        {
          //add all the children Grid Items to the checklist
          allGridItems.Add(newGridItem);
        }
      }

      // return failure
      return null;
    }

    /// <summary>
    /// Execute the Editor for the property with the given name
    /// </summary>
    /// <param name="propertyName">The name for the property to be edited</param>
    public void ExecuteUiEditor(string propertyName)
    {
      GridItem item = getGridItem(propertyName);
      if (item == null)
      {
        // error: Failed to find the property
        Console.WriteLine("Failed to find the property in the PropertyGrid:" + propertyName); 
        throw new Exception("Failed to find the property in the PropertyGrid:" + propertyName);
      }
      else
      {
        // set the property as the selected grid item
        propertyGrid.SelectedGridItem = item;

        // get the selected item and object of the property grid
        GridItem selectedItem = propertyGrid.SelectedGridItem;
        object selectedObject = SelectedObject;

        // get the property in the selected item
        PropertyDescriptorCollection properties = TypeDescriptor.GetProperties(SelectedObject);
        PropertyDescriptor property = properties[propertyName];

        // call the editor of the selected item
        UITypeEditor editor = (UITypeEditor)selectedItem.PropertyDescriptor.GetEditor(typeof(UITypeEditor));     
        object newValue = editor.EditValue((ITypeDescriptorContext) selectedItem, (IServiceProvider) selectedItem, selectedItem.Value);

        // set the proprty to the new value returned by the editor
        property.SetValue(selectedObject, newValue);
      }
    }

    #endregion

    #region Component Designer generated code
    /// <summary> 
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.components = new System.ComponentModel.Container();
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(PropertyPanel));
      this.propertyGrid = new CSharpFramework.Controls.PropertyGridEx();
      this.ToolStripButtonExpand = new System.Windows.Forms.ToolStripButton();
      this.ToolStripButtonCollapse = new System.Windows.Forms.ToolStripButton();
      this.ToolStripSeparator = new System.Windows.Forms.ToolStripSeparator();
      this.ToolStripButtonCopy = new System.Windows.Forms.ToolStripButton();
      this.ToolStripButtonPaste = new System.Windows.Forms.ToolStripButton();
      this.ToolStripSeparatorHelp = new System.Windows.Forms.ToolStripSeparator();
      this.ToolStripButtonRecentSelection = new System.Windows.Forms.ToolStripDropDownButton();
      this.ToolStripButtonSettings = new System.Windows.Forms.ToolStripDropDownButton();
      this.ToolStripMenuItemHelp = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripMenuItemCaption = new System.Windows.Forms.ToolStripMenuItem();
      this.dialogCaptionBar = new CSharpFramework.Controls.DialogCaptionBar();
      this.SuspendLayout();
      // 
      // propertyGrid
      // 
      this.propertyGrid.Dock = System.Windows.Forms.DockStyle.Fill;
      this.propertyGrid.HelpBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(252)))), ((int)(((byte)(252)))), ((int)(((byte)(252)))));
      this.propertyGrid.HelpBackgroundImage = ((System.Drawing.Image)(resources.GetObject("propertyGrid.HelpBackgroundImage")));
      this.propertyGrid.LineColor = System.Drawing.Color.FromArgb(((int)(((byte)(248)))), ((int)(((byte)(248)))), ((int)(((byte)(248)))));
      this.propertyGrid.Location = new System.Drawing.Point(0, 50);
      this.propertyGrid.Name = "propertyGrid";
      this.propertyGrid.Size = new System.Drawing.Size(330, 487);
      this.propertyGrid.TabIndex = 0;
      // 
      // 
      // 
      this.propertyGrid.ToolStrip.AccessibleName = "ToolBar";
      this.propertyGrid.ToolStrip.AccessibleRole = System.Windows.Forms.AccessibleRole.ToolBar;
      this.propertyGrid.ToolStrip.AllowMerge = false;
      this.propertyGrid.ToolStrip.AutoSize = false;
      this.propertyGrid.ToolStrip.CanOverflow = false;
      this.propertyGrid.ToolStrip.Dock = System.Windows.Forms.DockStyle.None;
      this.propertyGrid.ToolStrip.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
      this.propertyGrid.ToolStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripButtonExpand,
            this.ToolStripButtonCollapse,
            this.ToolStripSeparator,
            this.ToolStripButtonCopy,
            this.ToolStripButtonPaste,
            this.ToolStripSeparatorHelp,
            this.ToolStripButtonRecentSelection,
            this.ToolStripButtonSettings});
      this.propertyGrid.ToolStrip.Location = new System.Drawing.Point(0, 1);
      this.propertyGrid.ToolStrip.Name = "";
      this.propertyGrid.ToolStrip.Padding = new System.Windows.Forms.Padding(2, 0, 1, 0);
      this.propertyGrid.ToolStrip.Size = new System.Drawing.Size(330, 25);
      this.propertyGrid.ToolStrip.TabIndex = 1;
      this.propertyGrid.ToolStrip.TabStop = true;
      this.propertyGrid.ToolStrip.Text = "PropertyGridToolBar";
      this.propertyGrid.ToolStrip.ItemClicked += new ToolStripItemClickedEventHandler(ToolStrip_ItemClicked);
      this.propertyGrid.SelectedGridItemChanged += new System.Windows.Forms.SelectedGridItemChangedEventHandler(this.propertyGrid_SelectedGridItemChanged);
      // 
      // ToolStripButtonExpand
      // 
      this.ToolStripButtonExpand.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolStripButtonExpand.Image = global::Editor.Properties.Resources.expand_all;
      this.ToolStripButtonExpand.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolStripButtonExpand.Name = "ToolStripButtonExpand";
      this.ToolStripButtonExpand.Size = new System.Drawing.Size(23, 22);
      this.ToolStripButtonExpand.Text = "Expand";
      this.ToolStripButtonExpand.Click += new System.EventHandler(this.button_ExpandAll_Click);
      // 
      // ToolStripButtonCollapse
      // 
      this.ToolStripButtonCollapse.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolStripButtonCollapse.Image = global::Editor.Properties.Resources.collapse_all;
      this.ToolStripButtonCollapse.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolStripButtonCollapse.Name = "ToolStripButtonCollapse";
      this.ToolStripButtonCollapse.Size = new System.Drawing.Size(23, 22);
      this.ToolStripButtonCollapse.Text = "Collapse";
      this.ToolStripButtonCollapse.Click += new System.EventHandler(this.button_CollapseAll_Click);
      // 
      // ToolStripSeparator
      // 
      this.ToolStripSeparator.Name = "ToolStripSeparator";
      this.ToolStripSeparator.Size = new System.Drawing.Size(6, 25);
      // 
      // ToolStripButtonCopy
      // 
      this.ToolStripButtonCopy.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolStripButtonCopy.Enabled = false;
      this.ToolStripButtonCopy.Image = global::Editor.Properties.Resources.copy;
      this.ToolStripButtonCopy.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolStripButtonCopy.Name = "ToolStripButtonCopy";
      this.ToolStripButtonCopy.Size = new System.Drawing.Size(23, 22);
      this.ToolStripButtonCopy.Text = "Copy";
      this.ToolStripButtonCopy.ToolTipText = "Copy all properties of the selected category (CTRL-C)";
      this.ToolStripButtonCopy.Click += new System.EventHandler(this.Button_CopyProperties_Click);
      // 
      // ToolStripButtonPaste
      // 
      this.ToolStripButtonPaste.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolStripButtonPaste.Enabled = false;
      this.ToolStripButtonPaste.Image = global::Editor.Properties.Resources.paste;
      this.ToolStripButtonPaste.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolStripButtonPaste.Name = "ToolStripButtonPaste";
      this.ToolStripButtonPaste.Size = new System.Drawing.Size(23, 22);
      this.ToolStripButtonPaste.Text = "Paste";
      this.ToolStripButtonPaste.ToolTipText = "Paste properties from the clipboard to the selected category (CTRL-V)";
      this.ToolStripButtonPaste.Click += new System.EventHandler(this.Button_PasteProperties_Click);
      // 
      // ToolStripSeparatorHelp
      // 
      this.ToolStripSeparatorHelp.Name = "ToolStripSeparatorHelp";
      this.ToolStripSeparatorHelp.Size = new System.Drawing.Size(6, 25);
      // 
      // ToolStripButtonRecentSelection
      // 
      this.ToolStripButtonRecentSelection.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolStripButtonRecentSelection.Image = global::Editor.Properties.Resources.toolbar_recent_selection;
      this.ToolStripButtonRecentSelection.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolStripButtonRecentSelection.Name = "ToolStripButtonRecentSelection";
      this.ToolStripButtonRecentSelection.Size = new System.Drawing.Size(29, 22);
      this.ToolStripButtonRecentSelection.ToolTipText = "List of recently selected objects";
      this.ToolStripButtonRecentSelection.DropDownOpening += new EventHandler(ToolStripButtonRecentSelection_DropDownOpening);
      // 
      // ToolStripButtonSettings
      // 
      this.ToolStripButtonSettings.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolStripButtonSettings.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuItemHelp,
            this.ToolStripMenuItemCaption});
      this.ToolStripButtonSettings.Image = global::Editor.Properties.Resources.toolbar_export_settings;
      this.ToolStripButtonSettings.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolStripButtonSettings.Name = "ToolStripButtonSettings";
      this.ToolStripButtonSettings.Size = new System.Drawing.Size(29, 22);
      this.ToolStripButtonSettings.Text = "Settings";
      // 
      // ToolStripMenuItemHelp
      // 
      this.ToolStripMenuItemHelp.Checked = true;
      this.ToolStripMenuItemHelp.CheckOnClick = true;
      this.ToolStripMenuItemHelp.CheckState = System.Windows.Forms.CheckState.Checked;
      this.ToolStripMenuItemHelp.Image = global::Editor.Properties.Resources.toolbar_help;
      this.ToolStripMenuItemHelp.Name = "ToolStripMenuItemHelp";
      this.ToolStripMenuItemHelp.Size = new System.Drawing.Size(186, 22);
      this.ToolStripMenuItemHelp.Text = "Show Property Description";
      this.ToolStripMenuItemHelp.Click += new System.EventHandler(this.button_PropertyDescription_Click);
      // 
      // ToolStripMenuItemCaption
      // 
      this.ToolStripMenuItemCaption.Checked = true;
      this.ToolStripMenuItemCaption.CheckOnClick = true;
      this.ToolStripMenuItemCaption.CheckState = System.Windows.Forms.CheckState.Checked;
      this.ToolStripMenuItemCaption.Image = global::Editor.Properties.Resources.toolbar_caption;
      this.ToolStripMenuItemCaption.Name = "ToolStripMenuItemCaption";
      this.ToolStripMenuItemCaption.Size = new System.Drawing.Size(186, 22);
      this.ToolStripMenuItemCaption.Text = "Show Caption Bar";
      this.ToolStripMenuItemCaption.Click += new System.EventHandler(this.button_CaptionBar_Click);
      // 
      // dialogCaptionBar
      // 
      this.dialogCaptionBar.BackColor = System.Drawing.SystemColors.Window;
      this.dialogCaptionBar.Caption = "Property Editor";
      this.dialogCaptionBar.CompactView = false;
      this.dialogCaptionBar.Description = "Current Selection: <selection>";
      this.dialogCaptionBar.Dock = System.Windows.Forms.DockStyle.Top;
      this.dialogCaptionBar.Image = ((System.Drawing.Image)(resources.GetObject("dialogCaptionBar.Image")));
      this.dialogCaptionBar.Location = new System.Drawing.Point(0, 0);
      this.dialogCaptionBar.Name = "dialogCaptionBar";
      this.dialogCaptionBar.SetFontColor = System.Drawing.SystemColors.ControlText;
      this.dialogCaptionBar.ShowBorder = false;
      this.dialogCaptionBar.ShowBottomLine = true;
      this.dialogCaptionBar.ShowCaptionText = true;
      this.dialogCaptionBar.ShowImage = true;
      this.dialogCaptionBar.Size = new System.Drawing.Size(330, 50);
      this.dialogCaptionBar.TabIndex = 19;
      // 
      // PropertyPanel
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(330, 537);
      this.CloseButton = false;
      this.Controls.Add(this.propertyGrid);
      this.Controls.Add(this.dialogCaptionBar);
      this.DockableAreas = ((WeifenLuo.WinFormsUI.DockAreas)(((WeifenLuo.WinFormsUI.DockAreas.Float | WeifenLuo.WinFormsUI.DockAreas.DockLeft)
                  | WeifenLuo.WinFormsUI.DockAreas.DockRight)));
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.Name = "PropertyPanel";
      this.TabText = "Properties";
      this.Text = "Properties";
      this.Load += new System.EventHandler(this.PropertyPanel_Load);
      this.ResumeLayout(false);

    }



    #endregion
	
    #region Listen to Changes

    bool _bIsReadOnly = false;

    /// <summary>
    /// Sets the read-only status of this panel
    /// </summary>
    public bool ReadOnly
    {
      set
      {
        if (_bIsReadOnly==value)
          return;

        _bIsReadOnly = value;
        propertyGrid.Enabled = !value;

        // important to refresh enabled state
        InvalidatePropertyGrid();
      }
    }

    /// <summary>
    /// Event which gets triggered when the user changes property values in the property grid.
    /// </summary>
    private void propertyGrid_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
    {
      PendingActionsManager.Execute();
    }

    // event sent by the scene
    private void OnPropertyChanged(object sender, PropertyChangedArgs e)
    {
      InvalidatePropertyGrid();
    }

    private void OnShapeChanged(object sender, ShapeChangedArgs e)
    {
      switch (e.action)
      {
        case ShapeChangedArgs.Action.Removed:
          if (SelectedObject==e.shape)
            SelectedObject = null;
          break;
        case ShapeChangedArgs.Action.RefreshProperties:
        case ShapeChangedArgs.Action.NameChanged:
          if (SelectedObject == e.shape)
          {
            InvalidatePropertyGrid();
            this.ReadOnly = !e.shape.Modifiable;
          }
          break;
        case ShapeChangedArgs.Action.Clicked:
          if (!(SelectedObject is ShapeBase))
          {
            // The user selects a shape while the property grid still shows an item
            // from a different panel (e.g. layer). This typically means that the user
            // clicked in the shape tree while another panel was active. We might not
            // get a SelectionChanged event in this case (due to the focus change) and 
            // thus manually update the property grid selection now.
            SelectedObjects = EditorManager.SelectedShapes.ToArray();
          }
          break;
      }
    }

    /// <summary>
    /// Updates the property grid when the selection changed
    /// </summary>
    private void OnSelectionChanged(object sender, ShapeSelectionChangedArgs e)
    {
      if (EditorManager.GUI.UIUpdateLock > 0)
        return;

      SelectedObjects = e.NewSelection.ToArray();
      UpdateHelpPanel();
    }

    private void OnLayerChanged(object sender, LayerChangedArgs e)
    {
      if (EditorManager.GUI.UIUpdateLock > 0)
        return;

      switch (e.action)
      {
        case LayerChangedArgs.Action.Removed:
          if (SelectedObject==e.layer)
            SelectedObject = null;
          break;

        case LayerChangedArgs.Action.NameChanged:
        case LayerChangedArgs.Action.PropertyChanged:
        case LayerChangedArgs.Action.VisibleChanged:
          if (SelectedObject==e.layer)
            InvalidatePropertyGrid();
          break;

        case LayerChangedArgs.Action.Clicked:
        case LayerChangedArgs.Action.Selected:
          SelectedObject = e.layer;
          break;
        case LayerChangedArgs.Action.LockStatusChanged:
          if (SelectedObject==e.layer)
          {
            this.ReadOnly = !e.layer.Modifiable;
            InvalidatePropertyGrid();
          }
          break;
      }
    }

    void EditorScene_ZoneChanged(object sender, ZoneChangedArgs e)
    {
      if (EditorManager.GUI.UIUpdateLock > 0)
        return;

      switch (e.action)
      {
        case ZoneChangedArgs.Action.Removed:
          if (SelectedObject == e.zone)
            SelectedObject = null;
          break;

        case ZoneChangedArgs.Action.NameChanged:
        case ZoneChangedArgs.Action.PropertyChanged:
          if (SelectedObject == e.zone)
            InvalidatePropertyGrid();
          break;

        case ZoneChangedArgs.Action.Clicked:
        case ZoneChangedArgs.Action.Selected:
          SelectedObject = e.zone;
          break;
      }
    }

    private void OnSceneChanged(object sender, SceneChangedArgs e)
    {
      // make sure that we don't keep a reference to a shape of a different scene
      SelectedObject = null;

      // Clean up recent selection list
      _recentSelectionList.Clear();
    }

    private void PropertyPanel_Load(object sender, EventArgs e)
    {
      // Remove the Property Pages button
      propertyGrid.ToolStrip.Items.RemoveAt(4);
    }
        
    void ToolStrip_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
    {
      // Focus panel when toolstrip is clicked
      this.Focus();
    }

    #endregion

    #region Timer Based PropertyGrid Refresh

    private void InvalidatePropertyGrid()
    {
      // use a timer to collect subsequent invalidation calls
      _refreshTimer.Stop();
      _refreshTimer.Start();
    }
    
    private void OnRefresh(object source, ElapsedEventArgs e)
    {
      if (this.propertyGrid.InvokeRequired)
      {
        RefreshCallback d = new RefreshCallback(propertyGrid.Refresh);
        if (this.IsHandleCreated)
        {
          this.Invoke(d, new object[] { });
        }
      }
      else
        propertyGrid.Refresh();
    }

    #endregion

    #region Selected PropertyObject

    /// <summary>
    /// Gets or sets the selected object in the property grid
    /// </summary>
    public object SelectedObject
    {
      get {return this.propertyGrid.SelectedObject;}
      set 
      {
        // Remember the current item collapse state
        if (SelectedObject != null)
          _itemCollapseState = new PropertyGridCollapseState(propertyGrid);

        // Set the new selected object
        this.propertyGrid.SelectedObject=value;
        bool bModifiable = true;

        // Restore the item collapse state
        if (SelectedObject != null && _itemCollapseState != null)
          _itemCollapseState.Apply(propertyGrid);

        // Make the grid read-only the selected object is not modifiable
        if (value is ISceneObject)
          bModifiable = ((ISceneObject)value).Modifiable;
        this.ReadOnly = !bModifiable;

        // Update recent selection list
        AddToRecentSelection(value);
        UpdateCaptionBar(value);
      }
    }


    /// <summary>
    /// Gets or sets the selected objects in the property grid
    /// </summary>
    public object[] SelectedObjects
    {
      get {return this.propertyGrid.SelectedObjects;}
      set 
      {
        // Remember the current item collapse state
        if (SelectedObject != null)
          _itemCollapseState = new PropertyGridCollapseState(propertyGrid);

        // Set the new selected objects
        this.propertyGrid.SelectedObjects=value;
        bool bModifiable = true;
        
        // Make the grid read-only if at least one selected object is not modifiable
        foreach (object obj in value)
          if (obj is ISceneObject)
            if (!((ISceneObject)obj).Modifiable)
            {
              bModifiable = false;
              break;
            }
        this.ReadOnly = !bModifiable;

        // Restore the item collapse state
        if (SelectedObject != null && _itemCollapseState != null)
          _itemCollapseState.Apply(propertyGrid);  
      
        // Update recent selection list
        if (value != null && value.GetLength(0) > 0)
          AddToRecentSelection(value[0]);

        // Update caption bar
        UpdateCaptionBar(value);
      }
    }

    #endregion

    #region Category Copy Command

    void UpdateHelpPanel()
    {
      bool copyAndPasteActive = ((propertyGrid.SelectedGridItem != null) && (propertyGrid.SelectedGridItem.GridItemType == GridItemType.Category));

      ToolStripButtonCopy.Enabled = copyAndPasteActive;
      ToolStripButtonPaste.Enabled = copyAndPasteActive;
    }

    /// <summary>
    /// Event that is triggered when the selected grid item changed.
    /// We use this event to change to display a special help text for categories.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void propertyGrid_SelectedGridItemChanged(object sender, SelectedGridItemChangedEventArgs e)
    {
      UpdateHelpPanel();
    }

    private bool CopyProperties()
    {
      PropertyDictionary propDict = new PropertyDictionary();

      // Check whether the user has selected a category item
      GridItem selItem = propertyGrid.SelectedGridItem;
      if (selItem == null)
        return false;

      if (selItem.GridItemType == GridItemType.Category)
      {
        // Collect all properties of this category
        foreach (GridItem propertyItem in selItem.GridItems)
        {
          if (propertyItem.GridItemType != GridItemType.Property)
            continue;

          // Skip empty property values (which typically corresponds to a "mixed" value when multiple
          // shapes are selected)
          if (propertyItem.Value == null)
            continue;

          // Skip read-only properties
          if (propertyItem.PropertyDescriptor != null && propertyItem.PropertyDescriptor.IsReadOnly)
            continue;

          propDict.AddProperty(propertyItem.Label, propertyItem.Value);
        }
        if (!propDict.Empty)
        {
          // Copy the property values to the clipboard
          EditorManager.Clipboard.Data = EditorClipboard.CreateDataObject(propDict, "Shape Properties");
        }
        return true;
      }
      else if (selItem.GridItemType == GridItemType.Property)
      {
        // Do not copy properties for layer
        if (SelectedObject is Layer)
        {
          EditorManager.Clipboard.Data = null; //clear clipboarddata
          return true;
        }
        // Skip empty property values (which typically corresponds to a "mixed" value when multiple
        // shapes are selected)
        if (selItem.Value != null)
        {
          // Do not copy dynamicproperties
          if (selItem.Value is CSharpFramework.DynamicProperties.DynamicPropertyCollection)
          {
            EditorManager.Clipboard.Data = null; //clear clipboarddata
            return true;
          }

          EditorManager.Clipboard.Data = EditorClipboard.CreateDataObject(new PropertyKeyValuePair(selItem.Label, selItem.Value), "Shape Single Property");
        }
        return true;
      }
      else
      {
        EditorManager.Clipboard.Data = null; //clear clipboarddata
        return true;
      }
    }

    /// <summary>
    /// Processes command keys that are specific for this panel
    /// </summary>
    /// <param name="msg"></param>
    /// <param name="keyData"></param>
    /// <returns></returns>
    protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
    {
      if (keyData == (Keys.Control | Keys.C))
      {
        // Copying all properties in the selected category to the clipboard
        CopyProperties();
        return true;
      }
      else if (keyData == (Keys.Control | Keys.V))
      {
        // The active component is the view.
        EditorManager.ActiveComponent.OnPaste();
        return true;
      }
      else if (keyData == (Keys.Control | Keys.Z))
      {
        EditorManager.DoUndo();
        return true;
      }
      else if (keyData == (Keys.Control | Keys.Y))
      {
        EditorManager.DoRedo();
        return true;
      }
      return base.ProcessCmdKey(ref msg, keyData);
    }

    private void Button_CopyProperties_Click(object sender, EventArgs e)
    {
      CopyProperties();
    }

    private void Button_PasteProperties_Click(object sender, EventArgs e)
    {
      // The active component is the view.
      if (EditorManager.Clipboard.Data != null && EditorManager.Clipboard.Data.Object.GetType() == typeof(PropertyDictionary))
        EditorManager.ActiveComponent.OnPaste();
    }

    private void button_ExpandAll_Click(object sender, EventArgs e)
    {
      propertyGrid.ExpandAllGridItems();
    }

    private void button_CollapseAll_Click(object sender, EventArgs e)
    {
      propertyGrid.CollapseAllGridItems();
    }
    
    private void button_PropertyDescription_Click(object sender, EventArgs e)
    {
      propertyGrid.HelpVisible = this.ToolStripMenuItemHelp.Checked;
    }

    private void button_CaptionBar_Click(object sender, EventArgs e)
    {
      this.dialogCaptionBar.Visible = this.ToolStripMenuItemCaption.Checked;
    }

    #endregion

    #region Recent Selection

    private List<object> _recentSelectionList = new List<object>();
    private int _maxRecentSelection = 16;
 
    /// <summary>
    /// Adds an object to the recently selected object list
    /// </summary>
    /// <param name="obj">object that has been selected (shape, layer, zone)</param>
    private void AddToRecentSelection(object obj)
    {
      if (obj == null)
        return;

      // If object is already in list put it in the first place
      if (_recentSelectionList.Contains(obj))
      {
        _recentSelectionList.Remove(obj);
        _recentSelectionList.Insert(0, obj);
        return;
      }

      // If it is not in the list add it in the first place
      _recentSelectionList.Insert(0, obj);

      // Check if list exceeds maximum
      if (_recentSelectionList.Count > _maxRecentSelection)
        _recentSelectionList.RemoveAt(_maxRecentSelection);
    }
    
    /// <summary>
    /// Updates the list with the recently selected objects (shape, layer, zone).
    /// </summary>
    private void UpdateRecentSelection()
    {
      // Clean up (remove event handlers
      foreach (ToolStripMenuItem item in ToolStripButtonRecentSelection.DropDownItems)
        item.Click -= new EventHandler(recentItem_Click);
      this.ToolStripButtonRecentSelection.DropDownItems.Clear();

      foreach(object obj in _recentSelectionList)
      {
        ToolStripMenuItem item = new ToolStripMenuItem();

        // Layers
        Layer layer = obj as Layer;
        if (layer != null && EditorManager.Scene.Layers.Contains(layer))
        {
          item.Text = "Layer: " + layer.LayerName;
          item.Tag = obj;
          item.Click += new EventHandler(recentItem_Click);
          this.ToolStripButtonRecentSelection.DropDownItems.Add(item);
          int index = LayerTreeViewNodeBase.GetLayerIcon(layer);
          if (index >= 0)
            item.Image = LayerTreeViewNodeBase.ImageList.ImageList.Images[index];
          continue;
        }

        // Zones
        Zone zone = obj as Zone;
        if (zone != null && EditorManager.Scene.Zones.Contains(zone))
        {
          item.Text = "Zone: " + zone.ZoneName;
          item.Tag = obj;
          item.Click += new EventHandler(recentItem_Click);
          this.ToolStripButtonRecentSelection.DropDownItems.Add(item);
          int index = LayerTreeViewNodeBase.GetZoneIcon(zone);
          if (index >= 0)
            item.Image = LayerTreeViewNodeBase.ImageList.ImageList.Images[index];
          continue;
        }

        // Shapes
        ShapeBase shape = obj as ShapeBase;
        if (shape != null && shape.IsAddedToScene)
        {
          item.Text = "Shape: " + shape.ShapeName + " (" + shape.GetType().Name + ")";
          item.Tag = obj;
          item.Click += new EventHandler(recentItem_Click);
          this.ToolStripButtonRecentSelection.DropDownItems.Add(item);
          if (shape.IconIndex >= 0)
            item.Image = EditorManager.GUI.ShapeTreeImages.ImageList.Images[shape.IconIndex];
          continue;
        }       
      }
    }

    /// <summary>
    /// Updates the caption bar with the description of the new selection
    /// </summary>
    /// <param name="obj">object that has been selected (shape, layer, zone)</param>
    private void UpdateCaptionBar(object obj)
    {
      string selection = "Current Selection: ";
      // Layer or Zones or no selection
      if (obj == null)
        dialogCaptionBar.Description = selection + "None";
      else if (obj.GetType() == typeof(Layer) || obj.GetType() == typeof(V3DLayer))
        dialogCaptionBar.Description = selection + ((Layer)obj).LayerName+ " (Layer)";
      else if (obj.GetType() == typeof(Zone))
        dialogCaptionBar.Description = selection + ((Zone)obj).ZoneName + " (Zone)";
      
      // Shapes
      else if (obj.GetType() == typeof(ShapeBase[]))
      {
        ShapeBase[] shapes = (ShapeBase[])obj;
        if (shapes == null || shapes.GetLength(0) == 0)
          dialogCaptionBar.Description = selection + "None";               
        else if (shapes.GetLength(0) == 1)
          dialogCaptionBar.Description = selection + shapes[0].ShapeName + " (" + shapes[0].GetType().Name + ")";
        else if (shapes.GetLength(0) > 1)
          dialogCaptionBar.Description = selection + shapes.GetLength(0) + " objects";
      }
    }
    
    private void ToolStripButtonRecentSelection_DropDownOpening(object sender, EventArgs e)
    {
      UpdateRecentSelection();
    }

    private void recentItem_Click(object sender, EventArgs e)
    {
      this.Focus();
      ToolStripMenuItem recentItem = sender as ToolStripMenuItem;
      if (recentItem == null)
        return;

      object obj = recentItem.Tag;
      if (obj == null)
        return;

      if (obj.GetType() == typeof(Layer) || obj.GetType() == typeof(V3DLayer))
      {
        IScene.SendLayerChangedEvent(new LayerChangedArgs((Layer)obj, null, LayerChangedArgs.Action.Selected));
      }
      else if (obj.GetType() == typeof(Zone))
      {
        IScene.SendZoneChangedEvent(new ZoneChangedArgs((Zone)obj, ZoneChangedArgs.Action.Selected)); 
      }
      else
      {
        ShapeCollection shapes = new ShapeCollection();
        shapes.Add((ShapeBase)obj);
        EditorManager.SelectedShapes = shapes;

        // Fire event manually in case selection was the same (so event will not get fired)
        EditorManager.OnShapeSelectionChanged(new ShapeSelectionChangedArgs(null, shapes));
      }
    }

    #endregion

  }


  /// <summary>
  ///Single value description provider used with property grid items. This class redefines the behavior
  ///of a property grid when a single (float) set grid item value is wrong formatted.
  /// </summary>
  public class SingleTypeDescriptionProvider : TypeDescriptionProvider
  {
    # region Constructor

    private SingleCustomTypeDescriptor _descriptor = new SingleCustomTypeDescriptor();

    public SingleTypeDescriptionProvider(TypeDescriptionProvider parent)
      : base(parent)
    {

    }

    #endregion

    #region Override

    public override ICustomTypeDescriptor GetTypeDescriptor(Type objectType, object instance)
    {
      if (objectType == typeof(Single))
      {
        return _descriptor;
      }
      else
      {
        return base.GetTypeDescriptor(objectType, instance);
      }
    }

    #endregion
  }

  /// <summary>
  ///Single value custom type descriptor used with  provider used with SingleTypeDescriptionProvider class.
  ///of the grid item value when it is wrong formatted.
  /// </summary>
  public class SingleCustomTypeDescriptor : CustomTypeDescriptor
  {
    private SingleTypeConverter _converter = null;

    #region Override

    public override TypeConverter GetConverter()
    {
      if (_converter == null)
        _converter = new SingleTypeConverter();

      return _converter;
    }

    #endregion
  }

  /// <summary>
  ///Single value type converter used with SingleCustomTypeDescriptor class.
  ///Specifies single grid item values format check: automatically changes "," to "." and silently replaces wrong 
  ///formatted values for the last valid value.
  /// </summary>
  public class SingleTypeConverter : TypeConverter
  {
    #region Override

    public override bool CanConvertFrom(ITypeDescriptorContext context, Type sourceType)
    {
      if (sourceType == typeof(string))
      {
        return true;
      }
      else
      {
        return base.CanConvertFrom(context, sourceType);
      }
    }

    public override bool CanConvertTo(ITypeDescriptorContext context, Type destinationType)
    {
      if (destinationType == typeof(string))
      {
        return true;
      }
      else
      {
        return base.CanConvertTo(context, destinationType);
      }
    }

    public override object ConvertFrom(ITypeDescriptorContext context, CultureInfo culture, object value)
    {
      if (value.GetType() == typeof(string))
      {
        string strValue = (string)value;

        try
        {
          // Parse value replacing comma "," for a dot "."
          string parsedStrValue = strValue.Replace(',', '.');

          // Convert the value to a float
          return Single.Parse(parsedStrValue);
        }
        catch (FormatException)
        {
          // Catch any format exception and silently restore last value;
          // Note that this conversion is valid ONLY when used with PropertyGrid objects.
          GridItem item = (GridItem)context;

          return item.Value;
        }
      }
      else
      {
        return base.ConvertFrom(context, culture, value);
      }
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
