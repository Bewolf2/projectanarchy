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
using CSharpFramework.View;
using VisionEditorPlugin.Actions;
using VisionEditorPlugin.Tools;
using CSharpFramework.ShortCuts;

namespace VisionEditorPlugin.Dialogs
{
	/// <summary>
	/// DEPRECATED.
	/// </summary>
	public class LightmapToolDlg : System.Windows.Forms.Form
	{
    private System.Timers.Timer timer_LMUpdate;
    private System.Windows.Forms.TabControl tabControl_Effects;
    private System.Windows.Forms.TabPage tabPage_BrightnessContrast;
    private System.Windows.Forms.HScrollBar hScrollBar_ContrastRadiosity;
    private System.Windows.Forms.HScrollBar hScrollBar_ContrastBase;
    private System.Windows.Forms.HScrollBar hScrollBar_BrightnessRadiosity;
    private System.Windows.Forms.HScrollBar hScrollBar_BrightnessBase;
    private System.Windows.Forms.GroupBox groupBox1;
    private System.Windows.Forms.GroupBox groupBox2;
    private System.Windows.Forms.NumericUpDown numericUpDown_BrightnessBase;
    private System.Windows.Forms.NumericUpDown numericUpDown_ContrastBase;
    private System.Windows.Forms.NumericUpDown numericUpDown_BrightnessRad;
    private System.Windows.Forms.NumericUpDown numericUpDown_ContrastRad;
    private System.Windows.Forms.TabPage tabPage_Saturation;
    private System.Windows.Forms.NumericUpDown numericUpDown_SaturationBase;
    private System.Windows.Forms.HScrollBar hScrollBar_SaturationBase;
    private System.Windows.Forms.GroupBox groupBox3;
    private System.Windows.Forms.NumericUpDown numericUpDown_SaturationRad;
    private System.Windows.Forms.HScrollBar hScrollBar_SaturationRadiosity;
    private System.Windows.Forms.GroupBox groupBox4;
    private System.Windows.Forms.Label label_contrast1;
    private System.Windows.Forms.Label label_bright1;
    private System.Windows.Forms.Label label_contrast2;
    private System.Windows.Forms.Label label_bright2;
    private System.Windows.Forms.Label label_saturation1;
    private System.Windows.Forms.ToolTip ToolTip;
    private System.Windows.Forms.StatusBar statusBar1;
    private System.Windows.Forms.ToolBar toolBar_main;
    private System.Windows.Forms.ToolBarButton toolBarButton_Save;
    private System.Windows.Forms.ToolBarButton toolBarButton_Reset;
    private System.Windows.Forms.ToolBarButton toolBarButton_Undo;
    private System.Windows.Forms.ToolBarButton toolBarButton_Redo;
    private System.Windows.Forms.ToolBarButton toolBarButton_DirectLighting;
    private System.Windows.Forms.ToolBarButton toolBarButton_Radiosity;
    private System.Windows.Forms.ToolBarButton toolBarButton1;
    private System.Windows.Forms.ToolBarButton toolBarButton2;
    private System.Windows.Forms.Label label_saturation2;
    private System.Windows.Forms.ImageList ImageList;
    private System.Windows.Forms.ComboBox comboBox_Target;
    private System.Windows.Forms.Label label_Target;
    private System.ComponentModel.IContainer components;

    /// <summary>
    /// Constructor
    /// </summary>
		public LightmapToolDlg()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			LightmapManager.OneTimeInit();
      EditorManager.SceneEvent += new SceneEventHandler(EditorManager_SceneEvent);

      _actions.ActionEvent +=new ActionEventHandler(_actions_ActionEvent);

      Target = TweakTarget.Lightmaps;
		}

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    protected override void Dispose(bool disposing)
    {
      EditorManager.SceneEvent -= new SceneEventHandler(EditorManager_SceneEvent);
      LightmapManager.OneTimeDeInit();
      if (disposing)
      {
        if (components != null)
        {
          components.Dispose();
        }
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
      this.components = new System.ComponentModel.Container();
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(LightmapToolDlg));
      this.timer_LMUpdate = new System.Timers.Timer();
      this.tabControl_Effects = new System.Windows.Forms.TabControl();
      this.tabPage_BrightnessContrast = new System.Windows.Forms.TabPage();
      this.hScrollBar_ContrastRadiosity = new System.Windows.Forms.HScrollBar();
      this.hScrollBar_ContrastBase = new System.Windows.Forms.HScrollBar();
      this.hScrollBar_BrightnessRadiosity = new System.Windows.Forms.HScrollBar();
      this.hScrollBar_BrightnessBase = new System.Windows.Forms.HScrollBar();
      this.groupBox1 = new System.Windows.Forms.GroupBox();
      this.label_bright1 = new System.Windows.Forms.Label();
      this.numericUpDown_ContrastBase = new System.Windows.Forms.NumericUpDown();
      this.numericUpDown_BrightnessBase = new System.Windows.Forms.NumericUpDown();
      this.label_contrast1 = new System.Windows.Forms.Label();
      this.groupBox2 = new System.Windows.Forms.GroupBox();
      this.numericUpDown_ContrastRad = new System.Windows.Forms.NumericUpDown();
      this.numericUpDown_BrightnessRad = new System.Windows.Forms.NumericUpDown();
      this.label_contrast2 = new System.Windows.Forms.Label();
      this.label_bright2 = new System.Windows.Forms.Label();
      this.tabPage_Saturation = new System.Windows.Forms.TabPage();
      this.numericUpDown_SaturationRad = new System.Windows.Forms.NumericUpDown();
      this.hScrollBar_SaturationRadiosity = new System.Windows.Forms.HScrollBar();
      this.groupBox4 = new System.Windows.Forms.GroupBox();
      this.label_saturation2 = new System.Windows.Forms.Label();
      this.numericUpDown_SaturationBase = new System.Windows.Forms.NumericUpDown();
      this.hScrollBar_SaturationBase = new System.Windows.Forms.HScrollBar();
      this.groupBox3 = new System.Windows.Forms.GroupBox();
      this.label_saturation1 = new System.Windows.Forms.Label();
      this.ToolTip = new System.Windows.Forms.ToolTip(this.components);
      this.statusBar1 = new System.Windows.Forms.StatusBar();
      this.toolBar_main = new System.Windows.Forms.ToolBar();
      this.toolBarButton_Save = new System.Windows.Forms.ToolBarButton();
      this.toolBarButton_Reset = new System.Windows.Forms.ToolBarButton();
      this.toolBarButton1 = new System.Windows.Forms.ToolBarButton();
      this.toolBarButton_Undo = new System.Windows.Forms.ToolBarButton();
      this.toolBarButton_Redo = new System.Windows.Forms.ToolBarButton();
      this.toolBarButton2 = new System.Windows.Forms.ToolBarButton();
      this.toolBarButton_DirectLighting = new System.Windows.Forms.ToolBarButton();
      this.toolBarButton_Radiosity = new System.Windows.Forms.ToolBarButton();
      this.ImageList = new System.Windows.Forms.ImageList(this.components);
      this.comboBox_Target = new System.Windows.Forms.ComboBox();
      this.label_Target = new System.Windows.Forms.Label();
      ((System.ComponentModel.ISupportInitialize)(this.timer_LMUpdate)).BeginInit();
      this.tabControl_Effects.SuspendLayout();
      this.tabPage_BrightnessContrast.SuspendLayout();
      this.groupBox1.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_ContrastBase)).BeginInit();
      ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_BrightnessBase)).BeginInit();
      this.groupBox2.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_ContrastRad)).BeginInit();
      ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_BrightnessRad)).BeginInit();
      this.tabPage_Saturation.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_SaturationRad)).BeginInit();
      this.groupBox4.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_SaturationBase)).BeginInit();
      this.groupBox3.SuspendLayout();
      this.SuspendLayout();
      // 
      // timer_LMUpdate
      // 
      this.timer_LMUpdate.Enabled = true;
      this.timer_LMUpdate.Interval = 50;
      this.timer_LMUpdate.SynchronizingObject = this;
      this.timer_LMUpdate.Elapsed += new System.Timers.ElapsedEventHandler(this.timer_LMUpdate_Elapsed);
      // 
      // tabControl_Effects
      // 
      this.tabControl_Effects.Controls.Add(this.tabPage_BrightnessContrast);
      this.tabControl_Effects.Controls.Add(this.tabPage_Saturation);
      this.tabControl_Effects.Location = new System.Drawing.Point(8, 72);
      this.tabControl_Effects.Name = "tabControl_Effects";
      this.tabControl_Effects.SelectedIndex = 0;
      this.tabControl_Effects.Size = new System.Drawing.Size(480, 192);
      this.tabControl_Effects.TabIndex = 3;
      // 
      // tabPage_BrightnessContrast
      // 
      this.tabPage_BrightnessContrast.Controls.Add(this.hScrollBar_ContrastRadiosity);
      this.tabPage_BrightnessContrast.Controls.Add(this.hScrollBar_ContrastBase);
      this.tabPage_BrightnessContrast.Controls.Add(this.hScrollBar_BrightnessRadiosity);
      this.tabPage_BrightnessContrast.Controls.Add(this.hScrollBar_BrightnessBase);
      this.tabPage_BrightnessContrast.Controls.Add(this.groupBox1);
      this.tabPage_BrightnessContrast.Controls.Add(this.groupBox2);
      this.tabPage_BrightnessContrast.Location = new System.Drawing.Point(4, 22);
      this.tabPage_BrightnessContrast.Name = "tabPage_BrightnessContrast";
      this.tabPage_BrightnessContrast.Size = new System.Drawing.Size(472, 166);
      this.tabPage_BrightnessContrast.TabIndex = 0;
      this.tabPage_BrightnessContrast.Text = "Brightness and Contrast";
      // 
      // hScrollBar_ContrastRadiosity
      // 
      this.hScrollBar_ContrastRadiosity.LargeChange = 20;
      this.hScrollBar_ContrastRadiosity.Location = new System.Drawing.Point(40, 136);
      this.hScrollBar_ContrastRadiosity.Maximum = 400;
      this.hScrollBar_ContrastRadiosity.Name = "hScrollBar_ContrastRadiosity";
      this.hScrollBar_ContrastRadiosity.Size = new System.Drawing.Size(347, 16);
      this.hScrollBar_ContrastRadiosity.TabIndex = 5;
      this.ToolTip.SetToolTip(this.hScrollBar_ContrastRadiosity, "Contrast of radiosity contribution");
      this.hScrollBar_ContrastRadiosity.Value = 100;
      this.hScrollBar_ContrastRadiosity.ValueChanged += new System.EventHandler(this.hScrollBar_ContrastRadiosity_ValueChanged);
      this.hScrollBar_ContrastRadiosity.Scroll += new System.Windows.Forms.ScrollEventHandler(this.hScrollBar_ContrastRadiosity_Scroll);
      // 
      // hScrollBar_ContrastBase
      // 
      this.hScrollBar_ContrastBase.LargeChange = 20;
      this.hScrollBar_ContrastBase.Location = new System.Drawing.Point(40, 56);
      this.hScrollBar_ContrastBase.Maximum = 400;
      this.hScrollBar_ContrastBase.Name = "hScrollBar_ContrastBase";
      this.hScrollBar_ContrastBase.Size = new System.Drawing.Size(347, 16);
      this.hScrollBar_ContrastBase.TabIndex = 2;
      this.ToolTip.SetToolTip(this.hScrollBar_ContrastBase, "Contrast of direct lighting");
      this.hScrollBar_ContrastBase.Value = 100;
      this.hScrollBar_ContrastBase.ValueChanged += new System.EventHandler(this.hScrollBar_ContrastBase_ValueChanged);
      this.hScrollBar_ContrastBase.Scroll += new System.Windows.Forms.ScrollEventHandler(this.hScrollBar_ContrastBase_Scroll);
      // 
      // hScrollBar_BrightnessRadiosity
      // 
      this.hScrollBar_BrightnessRadiosity.Location = new System.Drawing.Point(40, 112);
      this.hScrollBar_BrightnessRadiosity.Minimum = -100;
      this.hScrollBar_BrightnessRadiosity.Name = "hScrollBar_BrightnessRadiosity";
      this.hScrollBar_BrightnessRadiosity.Size = new System.Drawing.Size(347, 16);
      this.hScrollBar_BrightnessRadiosity.TabIndex = 4;
      this.ToolTip.SetToolTip(this.hScrollBar_BrightnessRadiosity, "Brightness of radiosity contribution");
      this.hScrollBar_BrightnessRadiosity.ValueChanged += new System.EventHandler(this.hScrollBar_BrightnessRadiosity_ValueChanged);
      this.hScrollBar_BrightnessRadiosity.Scroll += new System.Windows.Forms.ScrollEventHandler(this.hScrollBar_BrightnessRadiosity_Scroll);
      // 
      // hScrollBar_BrightnessBase
      // 
      this.hScrollBar_BrightnessBase.Location = new System.Drawing.Point(40, 32);
      this.hScrollBar_BrightnessBase.Minimum = -100;
      this.hScrollBar_BrightnessBase.Name = "hScrollBar_BrightnessBase";
      this.hScrollBar_BrightnessBase.Size = new System.Drawing.Size(347, 16);
      this.hScrollBar_BrightnessBase.TabIndex = 1;
      this.ToolTip.SetToolTip(this.hScrollBar_BrightnessBase, "Brightness of direct lighting");
      this.hScrollBar_BrightnessBase.ValueChanged += new System.EventHandler(this.hScrollBar_BrightnessBase_ValueChanged);
      this.hScrollBar_BrightnessBase.Scroll += new System.Windows.Forms.ScrollEventHandler(this.hScrollBar_BrightnessBase_Scroll);
      // 
      // groupBox1
      // 
      this.groupBox1.Controls.Add(this.label_bright1);
      this.groupBox1.Controls.Add(this.numericUpDown_ContrastBase);
      this.groupBox1.Controls.Add(this.numericUpDown_BrightnessBase);
      this.groupBox1.Controls.Add(this.label_contrast1);
      this.groupBox1.Location = new System.Drawing.Point(8, 8);
      this.groupBox1.Name = "groupBox1";
      this.groupBox1.Size = new System.Drawing.Size(456, 72);
      this.groupBox1.TabIndex = 0;
      this.groupBox1.TabStop = false;
      this.groupBox1.Text = "Direct lighting";
      // 
      // label_bright1
      // 
      this.label_bright1.Image = ((System.Drawing.Image)(resources.GetObject("label_bright1.Image")));
      this.label_bright1.Location = new System.Drawing.Point(8, 24);
      this.label_bright1.Name = "label_bright1";
      this.label_bright1.Size = new System.Drawing.Size(16, 16);
      this.label_bright1.TabIndex = 0;
      this.ToolTip.SetToolTip(this.label_bright1, "Brightness");
      // 
      // numericUpDown_ContrastBase
      // 
      this.numericUpDown_ContrastBase.DecimalPlaces = 1;
      this.numericUpDown_ContrastBase.Location = new System.Drawing.Point(392, 48);
      this.numericUpDown_ContrastBase.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
      this.numericUpDown_ContrastBase.Name = "numericUpDown_ContrastBase";
      this.numericUpDown_ContrastBase.Size = new System.Drawing.Size(56, 20);
      this.numericUpDown_ContrastBase.TabIndex = 3;
      this.numericUpDown_ContrastBase.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
      this.ToolTip.SetToolTip(this.numericUpDown_ContrastBase, "Contrast of direct lighting");
      this.numericUpDown_ContrastBase.ValueChanged += new System.EventHandler(this.numericUpDown_ContrastBase_ValueChanged);
      // 
      // numericUpDown_BrightnessBase
      // 
      this.numericUpDown_BrightnessBase.DecimalPlaces = 1;
      this.numericUpDown_BrightnessBase.Location = new System.Drawing.Point(392, 24);
      this.numericUpDown_BrightnessBase.Minimum = new decimal(new int[] {
            100,
            0,
            0,
            -2147483648});
      this.numericUpDown_BrightnessBase.Name = "numericUpDown_BrightnessBase";
      this.numericUpDown_BrightnessBase.Size = new System.Drawing.Size(56, 20);
      this.numericUpDown_BrightnessBase.TabIndex = 1;
      this.numericUpDown_BrightnessBase.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
      this.ToolTip.SetToolTip(this.numericUpDown_BrightnessBase, "Brightness of direct lighting");
      this.numericUpDown_BrightnessBase.ValueChanged += new System.EventHandler(this.numericUpDown_BrightnessBase_ValueChanged);
      // 
      // label_contrast1
      // 
      this.label_contrast1.Image = ((System.Drawing.Image)(resources.GetObject("label_contrast1.Image")));
      this.label_contrast1.Location = new System.Drawing.Point(8, 48);
      this.label_contrast1.Name = "label_contrast1";
      this.label_contrast1.Size = new System.Drawing.Size(16, 16);
      this.label_contrast1.TabIndex = 2;
      this.ToolTip.SetToolTip(this.label_contrast1, "Contrast");
      // 
      // groupBox2
      // 
      this.groupBox2.Controls.Add(this.numericUpDown_ContrastRad);
      this.groupBox2.Controls.Add(this.numericUpDown_BrightnessRad);
      this.groupBox2.Controls.Add(this.label_contrast2);
      this.groupBox2.Controls.Add(this.label_bright2);
      this.groupBox2.Location = new System.Drawing.Point(8, 88);
      this.groupBox2.Name = "groupBox2";
      this.groupBox2.Size = new System.Drawing.Size(456, 72);
      this.groupBox2.TabIndex = 3;
      this.groupBox2.TabStop = false;
      this.groupBox2.Text = "Radiosity contribution";
      // 
      // numericUpDown_ContrastRad
      // 
      this.numericUpDown_ContrastRad.DecimalPlaces = 1;
      this.numericUpDown_ContrastRad.Location = new System.Drawing.Point(392, 48);
      this.numericUpDown_ContrastRad.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
      this.numericUpDown_ContrastRad.Name = "numericUpDown_ContrastRad";
      this.numericUpDown_ContrastRad.Size = new System.Drawing.Size(56, 20);
      this.numericUpDown_ContrastRad.TabIndex = 3;
      this.numericUpDown_ContrastRad.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
      this.ToolTip.SetToolTip(this.numericUpDown_ContrastRad, "Contrast of radiosity contribution");
      this.numericUpDown_ContrastRad.ValueChanged += new System.EventHandler(this.numericUpDown_ContrastRad_ValueChanged);
      // 
      // numericUpDown_BrightnessRad
      // 
      this.numericUpDown_BrightnessRad.DecimalPlaces = 1;
      this.numericUpDown_BrightnessRad.Location = new System.Drawing.Point(392, 24);
      this.numericUpDown_BrightnessRad.Minimum = new decimal(new int[] {
            100,
            0,
            0,
            -2147483648});
      this.numericUpDown_BrightnessRad.Name = "numericUpDown_BrightnessRad";
      this.numericUpDown_BrightnessRad.Size = new System.Drawing.Size(56, 20);
      this.numericUpDown_BrightnessRad.TabIndex = 1;
      this.numericUpDown_BrightnessRad.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
      this.ToolTip.SetToolTip(this.numericUpDown_BrightnessRad, "Brightness of radiosity contribution");
      this.numericUpDown_BrightnessRad.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
      this.numericUpDown_BrightnessRad.ValueChanged += new System.EventHandler(this.numericUpDown_BrightnessRad_ValueChanged);
      // 
      // label_contrast2
      // 
      this.label_contrast2.Image = ((System.Drawing.Image)(resources.GetObject("label_contrast2.Image")));
      this.label_contrast2.Location = new System.Drawing.Point(8, 48);
      this.label_contrast2.Name = "label_contrast2";
      this.label_contrast2.Size = new System.Drawing.Size(16, 16);
      this.label_contrast2.TabIndex = 2;
      this.ToolTip.SetToolTip(this.label_contrast2, "Contrast");
      // 
      // label_bright2
      // 
      this.label_bright2.Image = ((System.Drawing.Image)(resources.GetObject("label_bright2.Image")));
      this.label_bright2.Location = new System.Drawing.Point(8, 24);
      this.label_bright2.Name = "label_bright2";
      this.label_bright2.Size = new System.Drawing.Size(16, 16);
      this.label_bright2.TabIndex = 0;
      this.ToolTip.SetToolTip(this.label_bright2, "Brightness");
      // 
      // tabPage_Saturation
      // 
      this.tabPage_Saturation.Controls.Add(this.numericUpDown_SaturationRad);
      this.tabPage_Saturation.Controls.Add(this.hScrollBar_SaturationRadiosity);
      this.tabPage_Saturation.Controls.Add(this.groupBox4);
      this.tabPage_Saturation.Controls.Add(this.numericUpDown_SaturationBase);
      this.tabPage_Saturation.Controls.Add(this.hScrollBar_SaturationBase);
      this.tabPage_Saturation.Controls.Add(this.groupBox3);
      this.tabPage_Saturation.Location = new System.Drawing.Point(4, 22);
      this.tabPage_Saturation.Name = "tabPage_Saturation";
      this.tabPage_Saturation.Size = new System.Drawing.Size(472, 166);
      this.tabPage_Saturation.TabIndex = 1;
      this.tabPage_Saturation.Text = "Saturation";
      // 
      // numericUpDown_SaturationRad
      // 
      this.numericUpDown_SaturationRad.DecimalPlaces = 1;
      this.numericUpDown_SaturationRad.Location = new System.Drawing.Point(400, 120);
      this.numericUpDown_SaturationRad.Maximum = new decimal(new int[] {
            200,
            0,
            0,
            0});
      this.numericUpDown_SaturationRad.Name = "numericUpDown_SaturationRad";
      this.numericUpDown_SaturationRad.Size = new System.Drawing.Size(56, 20);
      this.numericUpDown_SaturationRad.TabIndex = 5;
      this.numericUpDown_SaturationRad.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
      this.ToolTip.SetToolTip(this.numericUpDown_SaturationRad, "Color saturation of radiosity contribution");
      this.numericUpDown_SaturationRad.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
      this.numericUpDown_SaturationRad.ValueChanged += new System.EventHandler(this.numericUpDown_SaturationRad_ValueChanged);
      // 
      // hScrollBar_SaturationRadiosity
      // 
      this.hScrollBar_SaturationRadiosity.Location = new System.Drawing.Point(40, 120);
      this.hScrollBar_SaturationRadiosity.Maximum = 200;
      this.hScrollBar_SaturationRadiosity.Name = "hScrollBar_SaturationRadiosity";
      this.hScrollBar_SaturationRadiosity.Size = new System.Drawing.Size(347, 16);
      this.hScrollBar_SaturationRadiosity.TabIndex = 4;
      this.ToolTip.SetToolTip(this.hScrollBar_SaturationRadiosity, "Color saturation of radiosity contribution");
      this.hScrollBar_SaturationRadiosity.Value = 100;
      this.hScrollBar_SaturationRadiosity.ValueChanged += new System.EventHandler(this.hScrollBar_SaturationRadiosity_ValueChanged);
      this.hScrollBar_SaturationRadiosity.Scroll += new System.Windows.Forms.ScrollEventHandler(this.hScrollBar_SaturationRadiosity_Scroll);
      // 
      // groupBox4
      // 
      this.groupBox4.Controls.Add(this.label_saturation2);
      this.groupBox4.Location = new System.Drawing.Point(8, 88);
      this.groupBox4.Name = "groupBox4";
      this.groupBox4.Size = new System.Drawing.Size(456, 72);
      this.groupBox4.TabIndex = 3;
      this.groupBox4.TabStop = false;
      this.groupBox4.Text = "Radiosity contribution";
      // 
      // label_saturation2
      // 
      this.label_saturation2.Image = ((System.Drawing.Image)(resources.GetObject("label_saturation2.Image")));
      this.label_saturation2.Location = new System.Drawing.Point(8, 32);
      this.label_saturation2.Name = "label_saturation2";
      this.label_saturation2.Size = new System.Drawing.Size(16, 16);
      this.label_saturation2.TabIndex = 0;
      this.ToolTip.SetToolTip(this.label_saturation2, "Saturation");
      // 
      // numericUpDown_SaturationBase
      // 
      this.numericUpDown_SaturationBase.DecimalPlaces = 1;
      this.numericUpDown_SaturationBase.Location = new System.Drawing.Point(400, 40);
      this.numericUpDown_SaturationBase.Maximum = new decimal(new int[] {
            200,
            0,
            0,
            0});
      this.numericUpDown_SaturationBase.Name = "numericUpDown_SaturationBase";
      this.numericUpDown_SaturationBase.Size = new System.Drawing.Size(56, 20);
      this.numericUpDown_SaturationBase.TabIndex = 2;
      this.numericUpDown_SaturationBase.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
      this.ToolTip.SetToolTip(this.numericUpDown_SaturationBase, "Color saturation of direct lighting");
      this.numericUpDown_SaturationBase.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
      this.numericUpDown_SaturationBase.ValueChanged += new System.EventHandler(this.numericUpDown_SaturationBase_ValueChanged);
      // 
      // hScrollBar_SaturationBase
      // 
      this.hScrollBar_SaturationBase.Location = new System.Drawing.Point(40, 40);
      this.hScrollBar_SaturationBase.Maximum = 200;
      this.hScrollBar_SaturationBase.Name = "hScrollBar_SaturationBase";
      this.hScrollBar_SaturationBase.Size = new System.Drawing.Size(347, 16);
      this.hScrollBar_SaturationBase.TabIndex = 1;
      this.ToolTip.SetToolTip(this.hScrollBar_SaturationBase, "Color saturation of direct lighting");
      this.hScrollBar_SaturationBase.Value = 100;
      this.hScrollBar_SaturationBase.ValueChanged += new System.EventHandler(this.hScrollBar_SaturationBase_ValueChanged);
      this.hScrollBar_SaturationBase.Scroll += new System.Windows.Forms.ScrollEventHandler(this.hScrollBar_SaturationBase_Scroll);
      // 
      // groupBox3
      // 
      this.groupBox3.Controls.Add(this.label_saturation1);
      this.groupBox3.Location = new System.Drawing.Point(8, 8);
      this.groupBox3.Name = "groupBox3";
      this.groupBox3.Size = new System.Drawing.Size(456, 72);
      this.groupBox3.TabIndex = 0;
      this.groupBox3.TabStop = false;
      this.groupBox3.Text = "Direct lighting";
      // 
      // label_saturation1
      // 
      this.label_saturation1.Image = ((System.Drawing.Image)(resources.GetObject("label_saturation1.Image")));
      this.label_saturation1.Location = new System.Drawing.Point(8, 32);
      this.label_saturation1.Name = "label_saturation1";
      this.label_saturation1.Size = new System.Drawing.Size(16, 16);
      this.label_saturation1.TabIndex = 0;
      this.ToolTip.SetToolTip(this.label_saturation1, "Saturation");
      // 
      // statusBar1
      // 
      this.statusBar1.Location = new System.Drawing.Point(0, 269);
      this.statusBar1.Name = "statusBar1";
      this.statusBar1.Size = new System.Drawing.Size(498, 22);
      this.statusBar1.SizingGrip = false;
      this.statusBar1.TabIndex = 4;
      this.statusBar1.Text = "statusBar1";
      // 
      // toolBar_main
      // 
      this.toolBar_main.Buttons.AddRange(new System.Windows.Forms.ToolBarButton[] {
            this.toolBarButton_Save,
            this.toolBarButton_Reset,
            this.toolBarButton1,
            this.toolBarButton_Undo,
            this.toolBarButton_Redo,
            this.toolBarButton2,
            this.toolBarButton_DirectLighting,
            this.toolBarButton_Radiosity});
      this.toolBar_main.ButtonSize = new System.Drawing.Size(24, 24);
      this.toolBar_main.DropDownArrows = true;
      this.toolBar_main.ImageList = this.ImageList;
      this.toolBar_main.Location = new System.Drawing.Point(0, 0);
      this.toolBar_main.Name = "toolBar_main";
      this.toolBar_main.ShowToolTips = true;
      this.toolBar_main.Size = new System.Drawing.Size(498, 36);
      this.toolBar_main.TabIndex = 0;
      this.toolBar_main.ButtonClick += new System.Windows.Forms.ToolBarButtonClickEventHandler(this.toolBar_main_ButtonClick);
      // 
      // toolBarButton_Save
      // 
      this.toolBarButton_Save.ImageIndex = 0;
      this.toolBarButton_Save.Name = "toolBarButton_Save";
      this.toolBarButton_Save.ToolTipText = "Saves the lightmaps to uncompressed dds files";
      // 
      // toolBarButton_Reset
      // 
      this.toolBarButton_Reset.ImageIndex = 1;
      this.toolBarButton_Reset.Name = "toolBarButton_Reset";
      this.toolBarButton_Reset.ToolTipText = "Resets the current parameter";
      // 
      // toolBarButton1
      // 
      this.toolBarButton1.Name = "toolBarButton1";
      this.toolBarButton1.Style = System.Windows.Forms.ToolBarButtonStyle.Separator;
      // 
      // toolBarButton_Undo
      // 
      this.toolBarButton_Undo.ImageIndex = 2;
      this.toolBarButton_Undo.Name = "toolBarButton_Undo";
      this.toolBarButton_Undo.ToolTipText = "Undos the last operation";
      // 
      // toolBarButton_Redo
      // 
      this.toolBarButton_Redo.ImageIndex = 3;
      this.toolBarButton_Redo.Name = "toolBarButton_Redo";
      this.toolBarButton_Redo.ToolTipText = "Redos the last operation";
      // 
      // toolBarButton2
      // 
      this.toolBarButton2.Name = "toolBarButton2";
      this.toolBarButton2.Style = System.Windows.Forms.ToolBarButtonStyle.Separator;
      // 
      // toolBarButton_DirectLighting
      // 
      this.toolBarButton_DirectLighting.ImageIndex = 4;
      this.toolBarButton_DirectLighting.Name = "toolBarButton_DirectLighting";
      this.toolBarButton_DirectLighting.Style = System.Windows.Forms.ToolBarButtonStyle.ToggleButton;
      this.toolBarButton_DirectLighting.ToolTipText = "Toggles direct lighting contribution";
      // 
      // toolBarButton_Radiosity
      // 
      this.toolBarButton_Radiosity.ImageIndex = 5;
      this.toolBarButton_Radiosity.Name = "toolBarButton_Radiosity";
      this.toolBarButton_Radiosity.Style = System.Windows.Forms.ToolBarButtonStyle.ToggleButton;
      this.toolBarButton_Radiosity.ToolTipText = "Toggles radiosity contribution";
      // 
      // ImageList
      // 
      this.ImageList.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("ImageList.ImageStream")));
      this.ImageList.TransparentColor = System.Drawing.Color.Magenta;
      this.ImageList.Images.SetKeyName(0, "");
      this.ImageList.Images.SetKeyName(1, "");
      this.ImageList.Images.SetKeyName(2, "");
      this.ImageList.Images.SetKeyName(3, "");
      this.ImageList.Images.SetKeyName(4, "");
      this.ImageList.Images.SetKeyName(5, "");
      // 
      // comboBox_Target
      // 
      this.comboBox_Target.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.comboBox_Target.Items.AddRange(new object[] {
            "Lightmaps",
            "Light Grid"});
      this.comboBox_Target.Location = new System.Drawing.Point(80, 40);
      this.comboBox_Target.Name = "comboBox_Target";
      this.comboBox_Target.Size = new System.Drawing.Size(104, 21);
      this.comboBox_Target.TabIndex = 2;
      this.comboBox_Target.SelectedIndexChanged += new System.EventHandler(this.comboBox_Target_SelectedIndexChanged);
      // 
      // label_Target
      // 
      this.label_Target.Location = new System.Drawing.Point(8, 40);
      this.label_Target.Name = "label_Target";
      this.label_Target.Size = new System.Drawing.Size(64, 16);
      this.label_Target.TabIndex = 1;
      this.label_Target.Text = "Target :";
      this.label_Target.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
      // 
      // LightmapToolDlg
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(498, 291);
      this.Controls.Add(this.label_Target);
      this.Controls.Add(this.comboBox_Target);
      this.Controls.Add(this.toolBar_main);
      this.Controls.Add(this.statusBar1);
      this.Controls.Add(this.tabControl_Effects);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.Name = "LightmapToolDlg";
      this.Text = "Lightmap Tool";
      this.Closing += new System.ComponentModel.CancelEventHandler(this.LightmapToolDlg_Closing);
      ((System.ComponentModel.ISupportInitialize)(this.timer_LMUpdate)).EndInit();
      this.tabControl_Effects.ResumeLayout(false);
      this.tabPage_BrightnessContrast.ResumeLayout(false);
      this.groupBox1.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_ContrastBase)).EndInit();
      ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_BrightnessBase)).EndInit();
      this.groupBox2.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_ContrastRad)).EndInit();
      ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_BrightnessRad)).EndInit();
      this.tabPage_Saturation.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_SaturationRad)).EndInit();
      this.groupBox4.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_SaturationBase)).EndInit();
      this.groupBox3.ResumeLayout(false);
      this.ResumeLayout(false);
      this.PerformLayout();

    }
		#endregion


    #region Activate status

    /// <summary>
    /// Gets or sets the activate status of this tool
    /// </summary>
    public bool Active
    {
      set 
      {
        if (value)
        {
          Show();
          timer_LMUpdate.Enabled = true;
          LightmapsEnabled = LightmapManager.GetSceneLightmapInfo(false);
          LightGridEnabled = LightmapManager.GetSceneLightGridInfo(false);
          _lightmapsParameter.bBaseLightingAvailable = LightmapManager.bLightmapBase;
          _lightmapsParameter.bRadiosityAvailable = LightmapManager.bLightmapRadiosity;
          _lightgridParameter.bBaseLightingAvailable = LightmapManager.bLightGridBase;
          _lightgridParameter.bRadiosityAvailable = LightmapManager.bLightGridRadiosity;

          this.toolBarButton_DirectLighting.Pushed = true;
          this.toolBarButton_Radiosity.Pushed = true;

          _actions.Reset();
          _bCanSave = true;

          // force update GUI etc.
          _tweakTarget = TweakTarget.None;
          Target = TweakTarget.Lightmaps;
        }
        else
        {
          timer_LMUpdate.Enabled = false;
          Hide();
        }
      }
      get {return Visible;}
    }

    private void LightmapToolDlg_Closing(object sender, System.ComponentModel.CancelEventArgs e)
    {
      e.Cancel = true; // do not close, but hide it
      Active = false;
    }

    #endregion


    #region Status bar

    void UpdateStatusBar()
    {
      if (_currentTargetParameter.bAvailable)
      {
        string msg = "Ready for manipulation.";
        bool bBase = _currentTargetParameter.bBaseLightingAvailable;
        bool bRad = _currentTargetParameter.bRadiosityAvailable;
        if (bBase && bRad) msg += " (Base lighting and radiosity)";
          else if (bBase && !bRad) msg += " (Base lighting only)";
          else if (!bBase && bRad) msg += " (Radiosity lighting only)";
        if( bBase || bRad )
          statusBar1.Text = msg;
      } 
      else
      {
        string msg = null;
        if (EditorManager.Scene==null)
          msg = "No scene loaded.";
        else if (!LightmapManager.MapLoaded())
          msg = "No map file (.v3d) loaded.";
        else
          msg = "Current map does not have a valid light info file. Please re-light map with tweak file output.";          
        statusBar1.Text = msg;
      }
    }


    #endregion

    #region Enable status

    /// <summary>
    /// Enables or disables all lightmap relevant controls
    /// </summary>
    public bool LightmapsEnabled
    {
      get {return _lightmapsParameter.bAvailable;}
      set
      {
        _lightmapsParameter.bAvailable = value;
        UpdateStatusBar();
        UpdateToolbarStates();
      }
    }

    /// <summary>
    /// Enables or disables all lightgrid relevant controls
    /// </summary>
    public bool LightGridEnabled
    {
      get {return _lightgridParameter.bAvailable;}
      set
      {
        _lightgridParameter.bAvailable = value;
        UpdateStatusBar();
        UpdateToolbarStates();
      }
    }

    #endregion

    #region Lightmap Update

    bool _bLMDirty = false;
    int _iUpdateLockCtr = 0;

    void UpdateLightingTarget(bool bImmediate)
    {
      // while dragging, use the current ones
      ColorMatrixParams _base = (_newBaseColorParams!=null) ? _newBaseColorParams : BaseParams;
      ColorMatrixParams _radiosity = (_newRadiosityColorParams!=null) ? _newRadiosityColorParams : RadiosityParams;
      LightmapManager.ApplyColorMatrices(_base.ColorMatrix, _radiosity.ColorMatrix);

      if (bImmediate)
      {
        if (Target==TweakTarget.Lightmaps)
          LightmapManager.UpdateLightmaps(true);
        if (Target==TweakTarget.LightGrid)
          LightmapManager.UpdateLightGrid(true);
        _bLMDirty = false;
      }
      else
        _bLMDirty = true;
    }

    private void timer_LMUpdate_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
    {
      if (_bLMDirty && _iUpdateLockCtr==0)
      {
        if (Target==TweakTarget.Lightmaps)
          LightmapManager.UpdateLightmaps(true);
        if (Target==TweakTarget.LightGrid)
          LightmapManager.UpdateLightGrid(true);
        _bLMDirty = false;
      }
    }

    /// <summary>
    /// Allows to lock or unlock the update to perform multiple updates
    /// </summary>
    public int UpdateLockCounter
    {
      get {return _iUpdateLockCtr;}
      set 
      {
        _iUpdateLockCtr = value;
        if (_iUpdateLockCtr==0 && _bLMDirty)
          UpdateLightingTarget(true);
      }
    }

    #endregion
    
    #region Scene or lighting changed

    void LightingChanged(bool bForceUpdate)
    {
      if (Active)
      {
        LightmapsEnabled = LightmapManager.GetSceneLightmapInfo(bForceUpdate);
        LightGridEnabled = LightmapManager.GetSceneLightGridInfo(bForceUpdate);
      }

      //reset some params
      NewBaseParams.Reset();
      NewRadiosityParams.Reset();
      _actions.Reset();
      _bCanSave = true;

      // force update GUI etc.
      _tweakTarget = TweakTarget.None;
      Target = TweakTarget.Lightmaps;
    }

    private void EditorManager_SceneEvent(object sender, SceneEventArgs e)
    {
      if (e.action == SceneEventArgs.Action.AfterStaticLighting)
      {
        LightingChanged(true);
      }
    }

    #endregion

    #region Color matrices
/*
    ColorMatrixParams _baseColorParams = new ColorMatrixParams();
    ColorMatrixParams _radiosityColorParams = new ColorMatrixParams();
*/    
    ColorMatrixParams _newBaseColorParams = null;
    ColorMatrixParams _newRadiosityColorParams = null;

    bool _bIgnoreSlider = false;

    /// <summary>
    /// gets or sets the parameter for base lighting
    /// </summary>
    public ColorMatrixParams BaseParams
    {
      get {return _currentTargetParameter.BaseLightingParams;}
      set 
      {
        _currentTargetParameter.BaseLightingParams = value;
        _newBaseColorParams = null;

        // set the scroll bars and numeric values
        _bIgnoreSlider = true;

        hScrollBar_BrightnessBase.Value = (int)(_currentTargetParameter.BaseLightingParams.Brightness*100.0f);
        hScrollBar_ContrastBase.Value = (int)(_currentTargetParameter.BaseLightingParams.Contrast*100.0f);
        hScrollBar_SaturationBase.Value = (int)(_currentTargetParameter.BaseLightingParams.Saturation*100.0f);

        numericUpDown_BrightnessBase.Value = (decimal)(_currentTargetParameter.BaseLightingParams.Brightness*100.0f);
        numericUpDown_ContrastBase.Value = (decimal)(_currentTargetParameter.BaseLightingParams.Contrast*100.0f);
        numericUpDown_SaturationBase.Value = (decimal)(_currentTargetParameter.BaseLightingParams.Saturation*100.0f);

        _bIgnoreSlider = false;
        UpdateLightingTarget(false);
      }
    }

    /// <summary>
    /// params while modifying
    /// </summary>
    public ColorMatrixParams NewBaseParams
    {
      get 
      {
        if (_newBaseColorParams==null)
          _newBaseColorParams = (ColorMatrixParams)_currentTargetParameter.BaseLightingParams.Clone();
        return _newBaseColorParams;
      }
    }


    /// <summary>
    /// gets or sets the parameter for radiosity lighting
    /// </summary>
    public ColorMatrixParams RadiosityParams
    {
      get {return _currentTargetParameter.RadiosityParams;}
      set 
      {
        _currentTargetParameter.RadiosityParams = value;
        _newRadiosityColorParams = null;

        // set the scroll bars
        _bIgnoreSlider = true;

        hScrollBar_BrightnessRadiosity.Value = (int)(_currentTargetParameter.RadiosityParams.Brightness*100.0f);
        hScrollBar_ContrastRadiosity.Value = (int)(_currentTargetParameter.RadiosityParams.Contrast*100.0f);
        hScrollBar_SaturationRadiosity.Value = (int)(_currentTargetParameter.RadiosityParams.Saturation*100.0f);

        numericUpDown_BrightnessRad.Value = (decimal)(_currentTargetParameter.RadiosityParams.Brightness*100.0f);
        numericUpDown_ContrastRad.Value = (decimal)(_currentTargetParameter.RadiosityParams.Contrast*100.0f);
        numericUpDown_SaturationRad.Value = (decimal)(_currentTargetParameter.RadiosityParams.Saturation*100.0f);

        _bIgnoreSlider = false;
        UpdateLightingTarget(false);
      }
    }


    /// <summary>
    /// params while modifying
    /// </summary>
    public ColorMatrixParams NewRadiosityParams
    {
      get 
      {
        if (_newRadiosityColorParams==null)
          _newRadiosityColorParams = (ColorMatrixParams)_currentTargetParameter.RadiosityParams.Clone();
        return _newRadiosityColorParams;
      }
    }

    void FireChangedAction(string name)
    {
      if (!NewBaseParams.Dirty && !NewRadiosityParams.Dirty)
        return;

      _actions.Add( new LightmapColorMatrixChangedAction(
          this, "Lightmap "+name, 
          _currentTargetParameter.BaseLightingParams, NewBaseParams, 
          _currentTargetParameter.RadiosityParams, NewRadiosityParams));

      UpdateLightingTarget(false);

      // set old to new values and set new to null
      BaseParams = _newBaseColorParams;
      RadiosityParams = _newRadiosityColorParams;

//      _newBaseColorParams = null;
//      _newRadiosityColorParams = null;
    }

    #endregion

    #region On / Off lighting

    bool BaseLightingEnabled
    {
      get {return !LightmapManager.bNoBaseLight;}
      set 
      {
        LightmapManager.bNoBaseLight = !value;
        UpdateLightingTarget(false);
      }
    }

    bool RadiosityEnabled
    {
      get {return !LightmapManager.bNoRadiosity;}
      set 
      {
        LightmapManager.bNoRadiosity = !value;
        UpdateLightingTarget(false);
      }
    }


    private void ResetLightingParams()
    {
      if (toolBarButton_DirectLighting.Enabled)
      {
        toolBarButton_DirectLighting.Pushed = true;
        NewBaseParams.Reset();
      }
      if (toolBarButton_DirectLighting.Enabled)
      {
        toolBarButton_DirectLighting.Pushed = true;
        NewRadiosityParams.Reset();
      }
      UpdateLightingTarget(false);
      FireChangedAction("reset");
    }


    #endregion

    #region Brightness / Contrast

    private void hScrollBar_BrightnessBase_Scroll(object sender, System.Windows.Forms.ScrollEventArgs e)
    {
      if (_bIgnoreSlider) return;
      if (e.Type==ScrollEventType.EndScroll)
        FireChangedAction("base brightness");
    }

    private void hScrollBar_ContrastBase_Scroll(object sender, System.Windows.Forms.ScrollEventArgs e)
    {
      if (_bIgnoreSlider) return;
      if (e.Type==ScrollEventType.EndScroll)
        FireChangedAction("base contrast");
    }

    private void hScrollBar_BrightnessRadiosity_Scroll(object sender, System.Windows.Forms.ScrollEventArgs e)
    {
      if (_bIgnoreSlider) return;
      if (e.Type==ScrollEventType.EndScroll)
        FireChangedAction("radiosity brightness");
    }

    private void hScrollBar_ContrastRadiosity_Scroll(object sender, System.Windows.Forms.ScrollEventArgs e)
    {
      if (_bIgnoreSlider) return;
      if (e.Type==ScrollEventType.EndScroll)
        FireChangedAction("radiosity contrast");
    }

    private void hScrollBar_BrightnessBase_ValueChanged(object sender, System.EventArgs e)
    {
      if (_bIgnoreSlider) return;
      HScrollBar bar = (HScrollBar)sender;
      NewBaseParams.Brightness = (float)bar.Value*0.01f;
      numericUpDown_BrightnessBase.Value = bar.Value;
      UpdateLightingTarget(false);
    }

    private void hScrollBar_ContrastBase_ValueChanged(object sender, System.EventArgs e)
    {
      if (_bIgnoreSlider) return;
      HScrollBar bar = (HScrollBar)sender;
      NewBaseParams.Contrast = (float)bar.Value*0.01f;
      numericUpDown_ContrastBase.Value = bar.Value;
      UpdateLightingTarget(false);
    }

    private void hScrollBar_BrightnessRadiosity_ValueChanged(object sender, System.EventArgs e)
    {
      if (_bIgnoreSlider) return;
      HScrollBar bar = (HScrollBar)sender;
      NewRadiosityParams.Brightness = (float)bar.Value*0.01f;
      numericUpDown_BrightnessRad.Value = bar.Value;
      UpdateLightingTarget(false);
    }

    private void hScrollBar_ContrastRadiosity_ValueChanged(object sender, System.EventArgs e)
    {
      if (_bIgnoreSlider) return;
      HScrollBar bar = (HScrollBar)sender;
      NewRadiosityParams.Contrast = (float)bar.Value*0.01f;
      numericUpDown_ContrastRad.Value = bar.Value;
      UpdateLightingTarget(false);
    }


    private void numericUpDown_BrightnessBase_ValueChanged(object sender, System.EventArgs e)
    {
      NumericUpDown n = (NumericUpDown)sender;
      this.hScrollBar_BrightnessBase.Value = (int)n.Value;
    }

    private void numericUpDown_ContrastBase_ValueChanged(object sender, System.EventArgs e)
    {
      NumericUpDown n = (NumericUpDown)sender;
      this.hScrollBar_ContrastBase.Value = (int)n.Value;
    }

    private void numericUpDown_BrightnessRad_ValueChanged(object sender, System.EventArgs e)
    {
      NumericUpDown n = (NumericUpDown)sender;
      this.hScrollBar_BrightnessRadiosity.Value = (int)n.Value;
    }

    private void numericUpDown_ContrastRad_ValueChanged(object sender, System.EventArgs e)
    {
      NumericUpDown n = (NumericUpDown)sender;
      this.hScrollBar_ContrastRadiosity.Value = (int)n.Value;
    }

    #endregion

    #region Saturation

    private void hScrollBar_SaturationBase_ValueChanged(object sender, System.EventArgs e)
    {
      if (_bIgnoreSlider) return;
      HScrollBar bar = (HScrollBar)sender;
      NewBaseParams.Saturation = (float)bar.Value*0.01f;
      numericUpDown_SaturationBase.Value = bar.Value;
      UpdateLightingTarget(false);
    }

    private void hScrollBar_SaturationBase_Scroll(object sender, System.Windows.Forms.ScrollEventArgs e)
    {
      if (_bIgnoreSlider) return;
      if (e.Type==ScrollEventType.EndScroll)
        FireChangedAction("base saturation");
    }

    private void numericUpDown_SaturationBase_ValueChanged(object sender, System.EventArgs e)
    {
      NumericUpDown n = (NumericUpDown)sender;
      this.hScrollBar_SaturationBase.Value = (int)n.Value;
    }

    private void hScrollBar_SaturationRadiosity_ValueChanged(object sender, System.EventArgs e)
    {
      if (_bIgnoreSlider) return;
      HScrollBar bar = (HScrollBar)sender;
      NewRadiosityParams.Saturation = (float)bar.Value*0.01f;
      numericUpDown_SaturationRad.Value = bar.Value;
      UpdateLightingTarget(false);
    }

    private void hScrollBar_SaturationRadiosity_Scroll(object sender, System.Windows.Forms.ScrollEventArgs e)
    {
      if (_bIgnoreSlider) return;
      if (e.Type==ScrollEventType.EndScroll)
        FireChangedAction("radiosity saturation");
    }

    private void numericUpDown_SaturationRad_ValueChanged(object sender, System.EventArgs e)
    {
      NumericUpDown n = (NumericUpDown)sender;
      this.hScrollBar_SaturationRadiosity.Value = (int)n.Value;
    }

    #endregion

    #region Undo/Redo

    ActionStack _actions = new ActionStack();
    bool _bCanSave = false;

    private void _actions_ActionEvent(object sender, ActionEventArgs e)
    {
      switch (e.state)
      {
        case ActionEventArgs.State.EndAdd:
        case ActionEventArgs.State.EndRedo:
        case ActionEventArgs.State.EndUndo:
          _bCanSave = true;
          UpdateToolbarStates();
          break;
      }
    }

    private void menuItem_Undo_Click(object sender, System.EventArgs e)
    {
      _actions.Undo();
    }

    private void menuItem_Redo_Click(object sender, System.EventArgs e)
    {
      _actions.Redo();
    }

    #endregion

    #region Toolbar

    void UpdateToolbarStates()
    {
      bool bAvailable = _currentTargetParameter.bAvailable;
      toolBar_main.Enabled = bAvailable;
      toolBarButton_DirectLighting.Enabled = bAvailable && _currentTargetParameter.bBaseLightingAvailable;
      toolBarButton_Radiosity.Enabled = bAvailable && _currentTargetParameter.bRadiosityAvailable;
      if (!bAvailable)
      {
//        toolBarButton_DirectLighting.Pushed = false;
//        toolBarButton_Radiosity.Pushed = false;
        tabControl_Effects.Enabled = false;
        return;
      }

      // grey out scroll bars (base)
      bool bBaseEnabled = bAvailable && BaseLightingEnabled;
      hScrollBar_BrightnessBase.Enabled = bBaseEnabled;
      hScrollBar_ContrastBase.Enabled = bBaseEnabled;
      hScrollBar_SaturationBase.Enabled = bBaseEnabled;
      numericUpDown_BrightnessBase.Enabled = bBaseEnabled;
      numericUpDown_ContrastBase.Enabled = bBaseEnabled;
      numericUpDown_SaturationBase.Enabled = bBaseEnabled;
      label_bright1.Enabled = bBaseEnabled;
      label_contrast1.Enabled = bBaseEnabled;
      label_saturation1.Enabled = bBaseEnabled;

      // grey out scroll bars (radiosity)
      bool bRadiosityEnabled = bAvailable && RadiosityEnabled;
      hScrollBar_BrightnessRadiosity.Enabled = bRadiosityEnabled;
      hScrollBar_ContrastRadiosity.Enabled = bRadiosityEnabled;
      hScrollBar_SaturationRadiosity.Enabled = bRadiosityEnabled;
      numericUpDown_BrightnessRad.Enabled = bRadiosityEnabled;
      numericUpDown_ContrastRad.Enabled = bRadiosityEnabled;
      numericUpDown_SaturationRad.Enabled = bRadiosityEnabled;
      label_bright2.Enabled = bRadiosityEnabled;
      label_contrast2.Enabled = bRadiosityEnabled;
      label_saturation2.Enabled = bRadiosityEnabled;

      toolBarButton_Save.Enabled = _bCanSave;
      toolBarButton_Reset.Enabled = true;
      toolBarButton_Undo.Enabled = _actions.UndoCount>0;
      toolBarButton_Redo.Enabled = _actions.RedoCount>0;

      tabControl_Effects.Enabled = bAvailable;
    }

    private void toolBar_main_ButtonClick(object sender, System.Windows.Forms.ToolBarButtonClickEventArgs e)
    {
      if (e.Button==toolBarButton_Save)
      {
        // turn this temporarely off (these properties aren't target specific!)
        // enabled again when changing target
        LightmapManager.bNoBaseLight = false;
        LightmapManager.bNoRadiosity = false;
        
        // when saving, apply the respective color matrix (hack)
        LightmapManager.ApplyColorMatrices(_lightmapsParameter.BaseLightingParams.ColorMatrix, _lightmapsParameter.RadiosityParams.ColorMatrix);
        LightmapManager.SaveAllLightmaps();

        LightmapManager.ApplyColorMatrices(_lightgridParameter.BaseLightingParams.ColorMatrix, _lightgridParameter.RadiosityParams.ColorMatrix);
        LightmapManager.SaveLightGrid();

        toolBarButton_Save.Enabled = _bCanSave = false;

        // force update GUI etc.
        TweakTarget oldTarget = Target;
        _tweakTarget = TweakTarget.None;
        Target = oldTarget;
      }
      if (e.Button==toolBarButton_Reset)
        ResetLightingParams();
      if (e.Button==toolBarButton_Undo)
        _actions.Undo();
      if (e.Button==toolBarButton_Redo)
        _actions.Redo();
      if (e.Button==toolBarButton_DirectLighting)
      {
        BaseLightingEnabled = toolBarButton_DirectLighting.Pushed && _currentTargetParameter.bBaseLightingAvailable;
        UpdateToolbarStates();
      }
      if (e.Button==toolBarButton_Radiosity)
      {
        RadiosityEnabled = toolBarButton_Radiosity.Pushed && _currentTargetParameter.bRadiosityAvailable;
        UpdateToolbarStates();
      }
    }

    #endregion


    #region Target


    LightingTweakParameter _lightmapsParameter = new LightingTweakParameter();
    LightingTweakParameter _lightgridParameter = new LightingTweakParameter();
    LightingTweakParameter _currentTargetParameter = null;
    /// <summary>
    /// Defines the editing target
    /// </summary>
    public enum TweakTarget
    {
      /// <summary>
      /// Undefined
      /// </summary>
      None = -1,

      /// <summary>
      /// LIghtmaps are edited
      /// </summary>
      Lightmaps = 0,

      /// <summary>
      /// Light grid is edited
      /// </summary>
      LightGrid = 1
    }


    TweakTarget _tweakTarget = TweakTarget.None;

    /// <summary>
    /// Gets or sets the tweak target
    /// </summary>
    public TweakTarget Target
    {
      get {return _tweakTarget;}
      set
      {
        if (_tweakTarget == value)
          return;
        _tweakTarget = value;

        _currentTargetParameter = null;
        if (_tweakTarget==TweakTarget.Lightmaps)
          _currentTargetParameter = _lightmapsParameter;
        if (_tweakTarget==TweakTarget.LightGrid)
          _currentTargetParameter = _lightgridParameter;


        BaseLightingEnabled = _currentTargetParameter.bBaseLightingAvailable && toolBarButton_DirectLighting.Pushed;
        RadiosityEnabled = _currentTargetParameter.bRadiosityAvailable && toolBarButton_Radiosity.Pushed;

        // update GUI
        comboBox_Target.SelectedIndex = (int)_tweakTarget;
        BaseParams = _currentTargetParameter.BaseLightingParams;
        RadiosityParams = _currentTargetParameter.RadiosityParams;

        UpdateToolbarStates();
        UpdateStatusBar();

        UpdateLightingTarget(true);
      }

    }

    private void comboBox_Target_SelectedIndexChanged(object sender, System.EventArgs e)
    {
      Target = (TweakTarget)comboBox_Target.SelectedIndex;
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
