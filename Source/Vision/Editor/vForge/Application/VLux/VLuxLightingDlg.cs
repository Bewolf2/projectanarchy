/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Drawing;
using System.IO;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Diagnostics;
using CSharpFramework;
using CSharpFramework.Dialogs;
using Microsoft.Win32;
using Editor.Vision;
using CSharpFramework.Controls;
using CSharpFramework.Scene;
using System.Runtime.InteropServices;
using CSharpFramework.Helper;
using System.Collections.Generic;

namespace Editor.VLux
{
	/// <summary>
	/// Dialog to edit static lighting settings
	/// </summary>
	public class VLuxLightingDlg : System.Windows.Forms.Form
	{
    private System.Windows.Forms.Button button_OK;
    private System.Windows.Forms.Button button_Cancel;
    private System.Windows.Forms.GroupBox groupBox1;
    private System.Windows.Forms.RadioButton radioButton_Standard;
    private System.Windows.Forms.RadioButton radioButton_Custom;
    private System.Windows.Forms.TextBox textBox_CustomSettingsFile;
    private System.Windows.Forms.Button button_browse;
    private System.Windows.Forms.Button button_EditvLuxSettings;
    private CSharpFramework.Controls.DialogCaptionBar dialogCaptionBar1;
    private System.Windows.Forms.Button button_Apply;
    private System.Windows.Forms.Button button_ExportOnly;
    private System.Windows.Forms.Button button_More;
    private System.Windows.Forms.Panel panel1;
    private System.Windows.Forms.Panel panel3;
    private System.Windows.Forms.Panel panel_More;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.Button button_ShowInfo;
    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.Button button_CopyParamStr;
    private System.Windows.Forms.Label label3;
    private Panel panel2;
    private ListView listView_Zones;
    private GroupBox groupBox2;
    private ColumnHeader columnHeader1;
    private ColumnHeader columnHeader2;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

    /// <summary>
    /// Constructor
    /// </summary>
		public VLuxLightingDlg()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

      button_More_Click(null,null);
      this.listView_Zones.SmallImageList = LayerTreeViewNodeBase.ImageList.ImageList;

      BuildZoneList();
		}

    /// <summary>
    /// flag that determines whether vLux should be triggered afterwards (if dialog result id OK)
    /// </summary>
    public bool DoRelighting = true;

    /// <summary>
    /// flag that determines whether only light info should be saved rather than re-lighting it
    /// </summary>
    public bool ExportInfoOnly = false;

    private bool _dialogExpanded = true;

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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(VLuxLightingDlg));
      this.button_OK = new System.Windows.Forms.Button();
      this.button_Cancel = new System.Windows.Forms.Button();
      this.groupBox1 = new System.Windows.Forms.GroupBox();
      this.button_EditvLuxSettings = new System.Windows.Forms.Button();
      this.button_browse = new System.Windows.Forms.Button();
      this.textBox_CustomSettingsFile = new System.Windows.Forms.TextBox();
      this.radioButton_Custom = new System.Windows.Forms.RadioButton();
      this.radioButton_Standard = new System.Windows.Forms.RadioButton();
      this.button_Apply = new System.Windows.Forms.Button();
      this.button_ExportOnly = new System.Windows.Forms.Button();
      this.button_More = new System.Windows.Forms.Button();
      this.panel1 = new System.Windows.Forms.Panel();
      this.panel_More = new System.Windows.Forms.Panel();
      this.label3 = new System.Windows.Forms.Label();
      this.button_CopyParamStr = new System.Windows.Forms.Button();
      this.label2 = new System.Windows.Forms.Label();
      this.button_ShowInfo = new System.Windows.Forms.Button();
      this.label1 = new System.Windows.Forms.Label();
      this.panel3 = new System.Windows.Forms.Panel();
      this.panel2 = new System.Windows.Forms.Panel();
      this.groupBox2 = new System.Windows.Forms.GroupBox();
      this.listView_Zones = new System.Windows.Forms.ListView();
      this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
      this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
      this.dialogCaptionBar1 = new CSharpFramework.Controls.DialogCaptionBar();
      this.groupBox1.SuspendLayout();
      this.panel1.SuspendLayout();
      this.panel_More.SuspendLayout();
      this.panel3.SuspendLayout();
      this.panel2.SuspendLayout();
      this.groupBox2.SuspendLayout();
      this.SuspendLayout();
      // 
      // button_OK
      // 
      this.button_OK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.button_OK.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.button_OK.Location = new System.Drawing.Point(152, 8);
      this.button_OK.Name = "button_OK";
      this.button_OK.Size = new System.Drawing.Size(80, 24);
      this.button_OK.TabIndex = 0;
      this.button_OK.Text = "Relight";
      this.button_OK.Click += new System.EventHandler(this.button_OK_Click);
      // 
      // button_Cancel
      // 
      this.button_Cancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.button_Cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.button_Cancel.Location = new System.Drawing.Point(344, 8);
      this.button_Cancel.Name = "button_Cancel";
      this.button_Cancel.Size = new System.Drawing.Size(80, 24);
      this.button_Cancel.TabIndex = 2;
      this.button_Cancel.Text = "Cancel";
      // 
      // groupBox1
      // 
      this.groupBox1.Controls.Add(this.button_EditvLuxSettings);
      this.groupBox1.Controls.Add(this.button_browse);
      this.groupBox1.Controls.Add(this.textBox_CustomSettingsFile);
      this.groupBox1.Controls.Add(this.radioButton_Custom);
      this.groupBox1.Controls.Add(this.radioButton_Standard);
      this.groupBox1.Location = new System.Drawing.Point(8, 8);
      this.groupBox1.Name = "groupBox1";
      this.groupBox1.Size = new System.Drawing.Size(416, 148);
      this.groupBox1.TabIndex = 0;
      this.groupBox1.TabStop = false;
      this.groupBox1.Text = "vLux settings";
      // 
      // button_EditvLuxSettings
      // 
      this.button_EditvLuxSettings.Location = new System.Drawing.Point(24, 109);
      this.button_EditvLuxSettings.Name = "button_EditvLuxSettings";
      this.button_EditvLuxSettings.Size = new System.Drawing.Size(120, 24);
      this.button_EditvLuxSettings.TabIndex = 4;
      this.button_EditvLuxSettings.Text = "Edit vLux settings";
      this.button_EditvLuxSettings.Click += new System.EventHandler(this.button_EditvLuxSettings_Click);
      // 
      // button_browse
      // 
      this.button_browse.Location = new System.Drawing.Point(360, 71);
      this.button_browse.Name = "button_browse";
      this.button_browse.Size = new System.Drawing.Size(20, 20);
      this.button_browse.TabIndex = 3;
      this.button_browse.Text = "...";
      this.button_browse.Click += new System.EventHandler(this.button_browse_Click);
      // 
      // textBox_CustomSettingsFile
      // 
      this.textBox_CustomSettingsFile.Location = new System.Drawing.Point(48, 71);
      this.textBox_CustomSettingsFile.Name = "textBox_CustomSettingsFile";
      this.textBox_CustomSettingsFile.Size = new System.Drawing.Size(304, 20);
      this.textBox_CustomSettingsFile.TabIndex = 2;
      this.textBox_CustomSettingsFile.TextChanged += new System.EventHandler(this.textBox_CustomSettingsFile_TextChanged);
      // 
      // radioButton_Custom
      // 
      this.radioButton_Custom.Location = new System.Drawing.Point(24, 41);
      this.radioButton_Custom.Name = "radioButton_Custom";
      this.radioButton_Custom.Size = new System.Drawing.Size(176, 27);
      this.radioButton_Custom.TabIndex = 1;
      this.radioButton_Custom.Text = "Custom settings file";
      this.radioButton_Custom.CheckedChanged += new System.EventHandler(this.radioButton_Custom_CheckedChanged);
      // 
      // radioButton_Standard
      // 
      this.radioButton_Standard.Location = new System.Drawing.Point(24, 23);
      this.radioButton_Standard.Name = "radioButton_Standard";
      this.radioButton_Standard.Size = new System.Drawing.Size(176, 22);
      this.radioButton_Standard.TabIndex = 0;
      this.radioButton_Standard.Text = "Standard settings file";
      this.radioButton_Standard.CheckedChanged += new System.EventHandler(this.radioButton_Standard_CheckedChanged);
      // 
      // button_Apply
      // 
      this.button_Apply.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.button_Apply.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.button_Apply.Location = new System.Drawing.Point(240, 8);
      this.button_Apply.Name = "button_Apply";
      this.button_Apply.Size = new System.Drawing.Size(96, 24);
      this.button_Apply.TabIndex = 1;
      this.button_Apply.Text = "Apply and exit";
      this.button_Apply.Click += new System.EventHandler(this.button_Apply_Click);
      // 
      // button_ExportOnly
      // 
      this.button_ExportOnly.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.button_ExportOnly.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.button_ExportOnly.Location = new System.Drawing.Point(16, 16);
      this.button_ExportOnly.Name = "button_ExportOnly";
      this.button_ExportOnly.Size = new System.Drawing.Size(96, 24);
      this.button_ExportOnly.TabIndex = 0;
      this.button_ExportOnly.Text = "Export info only";
      this.button_ExportOnly.Click += new System.EventHandler(this.button_ExportOnly_Click);
      // 
      // button_More
      // 
      this.button_More.Location = new System.Drawing.Point(368, 40);
      this.button_More.Name = "button_More";
      this.button_More.Size = new System.Drawing.Size(56, 24);
      this.button_More.TabIndex = 3;
      this.button_More.Text = "More >>";
      this.button_More.Click += new System.EventHandler(this.button_More_Click);
      // 
      // panel1
      // 
      this.panel1.Controls.Add(this.groupBox1);
      this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
      this.panel1.Location = new System.Drawing.Point(0, 50);
      this.panel1.Name = "panel1";
      this.panel1.Size = new System.Drawing.Size(434, 168);
      this.panel1.TabIndex = 1;
      // 
      // panel_More
      // 
      this.panel_More.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
      this.panel_More.Controls.Add(this.label3);
      this.panel_More.Controls.Add(this.button_CopyParamStr);
      this.panel_More.Controls.Add(this.label2);
      this.panel_More.Controls.Add(this.button_ShowInfo);
      this.panel_More.Controls.Add(this.label1);
      this.panel_More.Controls.Add(this.button_ExportOnly);
      this.panel_More.Dock = System.Windows.Forms.DockStyle.Top;
      this.panel_More.Location = new System.Drawing.Point(0, 519);
      this.panel_More.Name = "panel_More";
      this.panel_More.Size = new System.Drawing.Size(434, 134);
      this.panel_More.TabIndex = 4;
      // 
      // label3
      // 
      this.label3.Location = new System.Drawing.Point(120, 91);
      this.label3.Name = "label3";
      this.label3.Size = new System.Drawing.Size(288, 34);
      this.label3.TabIndex = 5;
      this.label3.Text = "Copy the argument string that is passed to vLux to the text clipboard";
      this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // button_CopyParamStr
      // 
      this.button_CopyParamStr.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.button_CopyParamStr.Location = new System.Drawing.Point(16, 96);
      this.button_CopyParamStr.Name = "button_CopyParamStr";
      this.button_CopyParamStr.Size = new System.Drawing.Size(96, 24);
      this.button_CopyParamStr.TabIndex = 4;
      this.button_CopyParamStr.Text = "Argumentstring";
      this.button_CopyParamStr.Click += new System.EventHandler(this.button_CopyParamStr_Click);
      // 
      // label2
      // 
      this.label2.Location = new System.Drawing.Point(120, 49);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(288, 40);
      this.label2.TabIndex = 3;
      this.label2.Text = "For debugging purposes display the light information for vLux";
      this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // button_ShowInfo
      // 
      this.button_ShowInfo.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.button_ShowInfo.Location = new System.Drawing.Point(16, 56);
      this.button_ShowInfo.Name = "button_ShowInfo";
      this.button_ShowInfo.Size = new System.Drawing.Size(96, 24);
      this.button_ShowInfo.TabIndex = 2;
      this.button_ShowInfo.Text = "Show info";
      this.button_ShowInfo.Click += new System.EventHandler(this.button_ShowInfo_Click);
      // 
      // label1
      // 
      this.label1.Location = new System.Drawing.Point(120, 8);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(288, 40);
      this.label1.TabIndex = 1;
      this.label1.Text = "Exports the scene relevant lighting information so vLux can be launched outside v" +
          "Forge";
      this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // panel3
      // 
      this.panel3.Controls.Add(this.button_OK);
      this.panel3.Controls.Add(this.button_Apply);
      this.panel3.Controls.Add(this.button_Cancel);
      this.panel3.Controls.Add(this.button_More);
      this.panel3.Dock = System.Windows.Forms.DockStyle.Top;
      this.panel3.Location = new System.Drawing.Point(0, 447);
      this.panel3.Name = "panel3";
      this.panel3.Size = new System.Drawing.Size(434, 72);
      this.panel3.TabIndex = 3;
      // 
      // panel2
      // 
      this.panel2.Controls.Add(this.groupBox2);
      this.panel2.Dock = System.Windows.Forms.DockStyle.Top;
      this.panel2.Location = new System.Drawing.Point(0, 218);
      this.panel2.Name = "panel2";
      this.panel2.Size = new System.Drawing.Size(434, 229);
      this.panel2.TabIndex = 2;
      // 
      // groupBox2
      // 
      this.groupBox2.Controls.Add(this.listView_Zones);
      this.groupBox2.Location = new System.Drawing.Point(8, 6);
      this.groupBox2.Name = "groupBox2";
      this.groupBox2.Size = new System.Drawing.Size(416, 220);
      this.groupBox2.TabIndex = 0;
      this.groupBox2.TabStop = false;
      this.groupBox2.Text = "Zones to light";
      // 
      // listView_Zones
      // 
      this.listView_Zones.CheckBoxes = true;
      this.listView_Zones.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2});
      this.listView_Zones.Location = new System.Drawing.Point(10, 25);
      this.listView_Zones.Name = "listView_Zones";
      this.listView_Zones.Size = new System.Drawing.Size(392, 189);
      this.listView_Zones.TabIndex = 0;
      this.listView_Zones.UseCompatibleStateImageBehavior = false;
      this.listView_Zones.View = System.Windows.Forms.View.Details;
      // 
      // columnHeader1
      // 
      this.columnHeader1.Text = "Zone Name";
      this.columnHeader1.Width = 159;
      // 
      // columnHeader2
      // 
      this.columnHeader2.Text = "Description";
      this.columnHeader2.Width = 229;
      // 
      // dialogCaptionBar1
      // 
      this.dialogCaptionBar1.BackColor = System.Drawing.SystemColors.Window;
      this.dialogCaptionBar1.Caption = "Relighting the Scene";
      this.dialogCaptionBar1.CompactView = false;
      this.dialogCaptionBar1.Description = "Please select the vLux settings file to use and press \"Relight\" to recompute the " +
          "static lighting.";
      this.dialogCaptionBar1.Dock = System.Windows.Forms.DockStyle.Top;
      this.dialogCaptionBar1.Image = ((System.Drawing.Image)(resources.GetObject("dialogCaptionBar1.Image")));
      this.dialogCaptionBar1.Location = new System.Drawing.Point(0, 0);
      this.dialogCaptionBar1.Name = "dialogCaptionBar1";
      this.dialogCaptionBar1.SetFontColor = System.Drawing.SystemColors.ControlText;
      this.dialogCaptionBar1.ShowBorder = false;
      this.dialogCaptionBar1.ShowBottomLine = true;
      this.dialogCaptionBar1.ShowCaptionText = true;
      this.dialogCaptionBar1.ShowImage = true;
      this.dialogCaptionBar1.Size = new System.Drawing.Size(434, 50);
      this.dialogCaptionBar1.TabIndex = 0;
      // 
      // StaticLightingDlg
      // 
      this.AcceptButton = this.button_OK;
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.CancelButton = this.button_Cancel;
      this.ClientSize = new System.Drawing.Size(434, 656);
      this.Controls.Add(this.panel_More);
      this.Controls.Add(this.panel3);
      this.Controls.Add(this.panel2);
      this.Controls.Add(this.panel1);
      this.Controls.Add(this.dialogCaptionBar1);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.MaximizeBox = false;
      this.MaximumSize = new System.Drawing.Size(440, 688);
      this.MinimizeBox = false;
      this.MinimumSize = new System.Drawing.Size(440, 544);
      this.Name = "StaticLightingDlg";
      this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
      this.Text = "Relight Scene";
      this.groupBox1.ResumeLayout(false);
      this.groupBox1.PerformLayout();
      this.panel1.ResumeLayout(false);
      this.panel_More.ResumeLayout(false);
      this.panel3.ResumeLayout(false);
      this.panel2.ResumeLayout(false);
      this.groupBox2.ResumeLayout(false);
      this.ResumeLayout(false);

    }
		#endregion

    #region Get and set settings

    EditorSceneSettings _settings = null;

    /// <summary>
    /// Gets or sets the settings from which the options are taken. The set function clones to local copy
    /// </summary>
    public EditorSceneSettings Settings
    {
      get {return _settings;}
      set 
      {
        _settings = (EditorSceneSettings)value.Clone();

        // set in the controls:
        textBox_CustomSettingsFile.Text = _settings.VLuxSettingsFile;
        EnableCustomSettingsFilename(!_settings.UseDefaultvLuxSettingsFile);
        radioButton_Standard.Checked = _settings.UseDefaultvLuxSettingsFile;
        radioButton_Custom.Checked = !_settings.UseDefaultvLuxSettingsFile;
      }
    }

    bool _bLightMainPart = true;

    public bool LightMainPart
    {
      get { return _bLightMainPart; }
      set { _bLightMainPart = value; }
    }



    #endregion

    #region edit vLux settings

    #region check for 64 OS and process
    //check if process is 64 bit 
    private static bool Is64BitProcess
    {
      get { return IntPtr.Size == 8; }
    }

    //check if operating system is 64 bit. Source: http://social.msdn.microsoft.com/Forums/en/csharpgeneral/thread/24792cdc-2d8e-454b-9c68-31a19892ca53
    private static bool Is64BitOperatingSystem
    {
      get
      {
        // Clearly if this is a 64-bit process we must be on a 64-bit OS.
        if (Is64BitProcess)
          return true;
        // Ok, so we are a 32-bit process, but is the OS 64-bit?
        // If we are running under Wow64 than the OS is 64-bit.
        bool isWow64;
        return ModuleContainsFunction("kernel32.dll", "IsWow64Process") && IsWow64Process(GetCurrentProcess(), out isWow64) && isWow64;
      }
    }

    private static bool ModuleContainsFunction(string moduleName, string methodName)
    {
      IntPtr hModule = GetModuleHandle(moduleName);
      if (hModule != IntPtr.Zero)
        return GetProcAddress(hModule, methodName) != IntPtr.Zero;
      return false;
    }

    [DllImport("kernel32.dll", SetLastError = true)]
    [return: MarshalAs(UnmanagedType.Bool)]
    extern static bool IsWow64Process(IntPtr hProcess, [MarshalAs(UnmanagedType.Bool)] out bool isWow64);
    [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
    extern static IntPtr GetCurrentProcess();
    [DllImport("kernel32.dll", CharSet = CharSet.Auto)]
    extern static IntPtr GetModuleHandle(string moduleName);
    [DllImport("kernel32.dll", CharSet = CharSet.Ansi, SetLastError = true)]
    extern static IntPtr GetProcAddress(IntPtr hModule, string methodName);

    #endregion

    /// <summary>
    /// Returns the relative path of vLux to the application folder
    /// </summary>
    static public string vLuxEXEPath
    {
      get
      {
        string sCurDirectory = Directory.GetCurrentDirectory();
        string[] sParts = sCurDirectory.Split('\\');

        string sConfigFolder = "";

        // find the last part of the path that starts with 'win32_' or 'x64_'
        int p = sParts.Length - 1;
        for (; p >= 0; --p)
        {
          if (sParts[p].StartsWith("win32_"))
          {
            sConfigFolder = sParts[p].Substring(6);
            break;
          }

          if (sParts[p].StartsWith("x64_"))
          {
            sConfigFolder = sParts[p].Substring(4);
            break;
          }
        }

        // sConfigFolder should now be "vs2010" or "vs2012" or something similar

        Debug.Assert(!String.IsNullOrEmpty(sConfigFolder));

        string sPathPrefix = String.Join("\\", sParts, 0, p);
        sPathPrefix += "\\";

        string[] sPaths =
        {
          sPathPrefix + "x64_"   + sConfigFolder + "\\release_dll\\DX9\\vLux.exe",
          sPathPrefix + "x64_"   + sConfigFolder + "\\dev_dll\\DX9\\vLux.exe",
          sPathPrefix + "x64_"   + sConfigFolder + "\\debug_dll\\DX9\\vLux.exe",
          sPathPrefix + "win32_" + sConfigFolder + "\\release_dll\\DX9\\vLux.exe",
          sPathPrefix + "win32_" + sConfigFolder + "\\dev_dll\\DX9\\vLux.exe",
          sPathPrefix + "win32_" + sConfigFolder + "\\debug_dll\\DX9\\vLux.exe",
        };

        // On Build machines exactly ONE version is available, so all versions must be tried, only their order may differ

        List<string> PathList = new List<string>();

        if (Is64BitOperatingSystem && (Is64BitProcess || EditorManager.Settings.AlwaysUse64BitvLux))
        {
          // 64 Bit can and should be used

          if (EditorManager.Settings.UseDebugvLux)
          {
            // prefer the more debuggable versions, only fall back to release versions, if nothing else is available

            PathList.Add(sPaths[2]); // Debug x64
            PathList.Add(sPaths[1]); // Dev x64

            PathList.Add(sPaths[5]); // Debug x86
            PathList.Add(sPaths[4]); // Dev x86

            PathList.Add(sPaths[0]); // Release x64
            PathList.Add(sPaths[3]); // Release x86
          }
          else
          {
            // prefer the faster versions, only fall back to debug if nothing else is available

            PathList.Add(sPaths[0]); // Release x64
            PathList.Add(sPaths[1]); // Dev x64
            PathList.Add(sPaths[3]); // Release x86
            PathList.Add(sPaths[4]); // Dev x86

            PathList.Add(sPaths[2]); // Debug x64
            PathList.Add(sPaths[5]); // Debug x86
          }
        }
        else
        {
          // Only 32 Bit is available

          if (EditorManager.Settings.UseDebugvLux)
          {
            // prefer the debug versions

            PathList.Add(sPaths[5]); // Debug x86
            PathList.Add(sPaths[4]); // Dev x86
            PathList.Add(sPaths[3]); // Release x86
          }
          else
          {
            // prefer the release versions

            PathList.Add(sPaths[3]); // Release x86
            PathList.Add(sPaths[4]); // Dev x86
            PathList.Add(sPaths[5]); // Debug x86            
          }
        }

        for (int i = 0; i < PathList.Count; ++i)
        {
          if (System.IO.File.Exists(PathList[i]))
            return PathList[i];
        }

        return "vLux.exe not found";
      }
    }

    private string VLuxArgumentString
    {
      get
      {
        // put together vLux parameter string
        string projectDir = EditorApp.Project.ProjectDir;
        if (projectDir.EndsWith(@"\"))
          projectDir = projectDir.Remove(projectDir.Length-1,1);

        // determine out filename (including relative path) and remove file extension
        string outFilename = EditorApp.Scene.FileName;
        string outFilenameExt = Path.GetExtension(outFilename);
        if (outFilenameExt != null)
          outFilename = outFilename.Remove(outFilename.Length - outFilenameExt.Length, outFilenameExt.Length);
 
        string paramStr = "\"\""; // no v3d !
        paramStr += " -project \"" + projectDir + "\"";               // the project folder
        paramStr += " -tempfolder \"" + EditorApp.Scene.LayerDirectoryName + "\"";    // the relative folder name where the temp files are located
        paramStr += " -out \"" + outFilename + "\""; // output filename
        paramStr += " -title \"" + EditorApp.Scene.FileName + "\""; // window title
        paramStr += " -settings";

        if (!_settings.UseDefaultvLuxSettingsFile && !string.IsNullOrEmpty(_settings.VLuxSettingsFile))
        {
          string custom = Path.Combine(EditorApp.Scene.LayerDirectoryName,_settings.VLuxSettingsFile);
          paramStr += " -config \"" + custom + "\"";
        }
        return paramStr;
      }
    }

    private void button_EditvLuxSettings_Click(object sender, System.EventArgs e)
    {
      try
      {
        //add/edit lighting cfg (either custom or default name "lighting.cfg") to RCS:
        string lightingCfgFile = VLuxLightingTool.GetLightingCfgFile(_settings);

        ManagedBase.RCS.GetProvider().EditFile(lightingCfgFile);

        Process vLux = FileHelper.RunExternalTool("vLux", VLuxLightingDlg.vLuxEXEPath, VLuxArgumentString, true);

        if (vLux == null)
          return;

        Console.WriteLine(vLux.StandardOutput.ReadToEnd());

        vLux.WaitForExit();
        int iExitCode = vLux.ExitCode;

        //add lighting cfg file to RCS
        ManagedBase.RCS.GetProvider().AddFile(lightingCfgFile, false /* Text file */);

      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex);
        EditorManager.ShowMessageBox("Could not start vLux (\"" + VLuxLightingDlg.vLuxEXEPath + "\").\n\nDetailed message:\n" + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
      }
    }
    #endregion


    #region Build Zone list

    void BuildZoneList()
    {
      this.listView_Zones.BeginUpdate();

      ListViewItem item = listView_Zones.Items.Add("Main part");
      item.Checked = LightMainPart;
      item.SubItems.Add("Geometry that is not assigned to any zone");

      foreach (Zone zone in EditorManager.Scene.Zones)
      {
        if (!zone.Loaded) // cannot light these zones
          continue;
        item = listView_Zones.Items.Add(zone.ZoneName);
        item.Tag = zone;    
        int idx = LayerTreeViewNodeBase.GetZoneIcon(zone);
        item.ImageIndex = idx;
        item.Checked = zone.RunLighting;
        item.SubItems.Add(zone.Description);
      }

      this.listView_Zones.EndUpdate();
    }

    /// <summary>
    /// Retrieves the checked status from the items
    /// </summary>
    void GetZoneFlags()
    {
      foreach (ListViewItem item in listView_Zones.Items)
      {
        Zone zone = item.Tag as Zone;
        if (zone != null)
          zone.RunLighting = item.Checked;
        else
          LightMainPart = item.Checked;
      }

    }

    #endregion


    #region Controls

    void EnableCustomSettingsFilename(bool bStatus)
    {
      button_browse.Enabled = bStatus;
      textBox_CustomSettingsFile.Enabled = bStatus;
    }

    private void radioButton_Standard_CheckedChanged(object sender, System.EventArgs e)
    {
      _settings.UseDefaultvLuxSettingsFile = true;
      EnableCustomSettingsFilename(!_settings.UseDefaultvLuxSettingsFile);
    }

    private void radioButton_Custom_CheckedChanged(object sender, System.EventArgs e)
    {
      _settings.UseDefaultvLuxSettingsFile = false;
      EnableCustomSettingsFilename(!_settings.UseDefaultvLuxSettingsFile);
    }

    private void textBox_CustomSettingsFile_TextChanged(object sender, System.EventArgs e)
    {
      _settings.VLuxSettingsFile = textBox_CustomSettingsFile.Text;
    }

    private void button_browse_Click(object sender, System.EventArgs e)
    {
      OpenFileDlg fileDlg = new OpenFileDlg();
      fileDlg.ShowBaseData = false;
      fileDlg.Caption = "Selecting the Settings File";
      fileDlg.Description = "Please select the vLux settings file you want to use and press OK to continue";
      string settingsDir = EditorApp.Scene.LayerDirectoryName;
      fileDlg.InitialDirectory = settingsDir;
      fileDlg.Filter = new string[] {".cfg"};
      if (fileDlg.ShowDialog() != DialogResult.OK)
        return;

      textBox_CustomSettingsFile.Text = fileDlg.RelativeFileName;
    }

    #endregion

    #region Buttons

    private void button_OK_Click(object sender, EventArgs e)
    {
      this.GetZoneFlags();
    }

    private void button_More_Click(object sender, System.EventArgs e)
    {
      if (_dialogExpanded)
      {
        panel_More.Visible = _dialogExpanded = false;
        this.button_More.Text = "More >>";
        this.Size = this.MinimumSize;
      }
      else
      {
        panel_More.Visible = _dialogExpanded = true;
        this.button_More.Text = "More <<";
        this.Size = this.MaximumSize;
      }
    }

    private void button_Apply_Click(object sender, System.EventArgs e)
    {
      this.GetZoneFlags();
      DoRelighting = false;
    }

    private void button_ExportOnly_Click(object sender, System.EventArgs e)
    {
      this.GetZoneFlags();
      ExportInfoOnly = true;
    }


    private void button_ShowInfo_Click(object sender, System.EventArgs e)
    {
      VisionLightInfo info = new VisionLightInfo();

      // collect all infos
      foreach (Layer layer in EditorManager.Scene.Layers)
        layer.GetStaticLightInfo(info);

      string s = info.ToText();
      
      if (s==null)
        return;

      ShowDescriptionDlg dlg = new ShowDescriptionDlg();
      dlg.Text = "Scene lighting information";
      dlg.HeaderText = "Scene elements that contribute to vLux computations";
      dlg.DescriptionText = s;
      dlg.ShowDialog(this);

    }

    private void button_CopyParamStr_Click(object sender, System.EventArgs e)
    {
      string paramStr = this.VLuxArgumentString;
      Clipboard.SetDataObject( paramStr, true );
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
