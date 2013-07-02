/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/*
using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using CSharpFramework;
using ManagedFramework;
using Editor.Shapes;

namespace Editor.Dialogs
{
	/// <summary>
	/// Summary description for DepthFogPropertiesDlg.
	/// </summary>
	public class DepthFogPropertiesDlg : System.Windows.Forms.Form
	{
    private System.Windows.Forms.Button button_OK;
    private System.Windows.Forms.Button button_Cancel;
    private System.Windows.Forms.TrackBar LinearStartDist;
    private System.Windows.Forms.TrackBar LinearDepth;
    private System.Windows.Forms.ColorDialog FogColorDialog;
    private System.Windows.Forms.RadioButton radioButton_Lin;
    private System.Windows.Forms.Label label_LinStart;
    private System.Windows.Forms.NumericUpDown numeric_LinStart;
    private System.Windows.Forms.NumericUpDown numeric_LinDepth;
    private System.Windows.Forms.Label label_LinDepth;
    private System.Windows.Forms.GroupBox groupBox_lin;
    private System.Windows.Forms.RadioButton radioButton_noFog;
    private System.Windows.Forms.RadioButton radioButton_Exp;
    private System.Windows.Forms.GroupBox groupBox_Exp;
    private System.Windows.Forms.NumericUpDown numericUpDown_Exp;
    private System.Windows.Forms.TrackBar trackBar_Exponent;
    private System.Windows.Forms.Label label_Exponent;
    private System.Windows.Forms.Button ButtonFogColor;
    private System.Windows.Forms.GroupBox groupBox_Color;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public DepthFogPropertiesDlg(DepthFogShape shape)
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

      Shape = shape;
			CopyShape = shape.Clone() as DepthFogShape;

      // set dialog data
      bAllowUpdate = false;
      this.LinearStartDist.Value = CopyShape.LinearStart;
      this.numeric_LinStart.Value = CopyShape.LinearStart;
      this.LinearDepth.Value = CopyShape.LinearDepth;
      this.numeric_LinDepth.Value = CopyShape.LinearDepth;
      this.numericUpDown_Exp.Value = (decimal)CopyShape.Exponent;
      this.ButtonFogColor.BackColor = CopyShape.FogColor;

      FogType = CopyShape.FogType;
      bAllowUpdate = true;
    }

    DepthFogShape Shape;
    DepthFogShape CopyShape;
    bool bAllowUpdate = false;

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
      this.button_OK = new System.Windows.Forms.Button();
      this.button_Cancel = new System.Windows.Forms.Button();
      this.LinearStartDist = new System.Windows.Forms.TrackBar();
      this.LinearDepth = new System.Windows.Forms.TrackBar();
      this.FogColorDialog = new System.Windows.Forms.ColorDialog();
      this.radioButton_Lin = new System.Windows.Forms.RadioButton();
      this.groupBox_lin = new System.Windows.Forms.GroupBox();
      this.label_LinDepth = new System.Windows.Forms.Label();
      this.numeric_LinDepth = new System.Windows.Forms.NumericUpDown();
      this.label_LinStart = new System.Windows.Forms.Label();
      this.numeric_LinStart = new System.Windows.Forms.NumericUpDown();
      this.radioButton_noFog = new System.Windows.Forms.RadioButton();
      this.radioButton_Exp = new System.Windows.Forms.RadioButton();
      this.groupBox_Exp = new System.Windows.Forms.GroupBox();
      this.label_Exponent = new System.Windows.Forms.Label();
      this.trackBar_Exponent = new System.Windows.Forms.TrackBar();
      this.numericUpDown_Exp = new System.Windows.Forms.NumericUpDown();
      this.ButtonFogColor = new System.Windows.Forms.Button();
      this.groupBox_Color = new System.Windows.Forms.GroupBox();
      ((System.ComponentModel.ISupportInitialize)(this.LinearStartDist)).BeginInit();
      ((System.ComponentModel.ISupportInitialize)(this.LinearDepth)).BeginInit();
      this.groupBox_lin.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.numeric_LinDepth)).BeginInit();
      ((System.ComponentModel.ISupportInitialize)(this.numeric_LinStart)).BeginInit();
      this.groupBox_Exp.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.trackBar_Exponent)).BeginInit();
      ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_Exp)).BeginInit();
      this.SuspendLayout();
      // 
      // button_OK
      // 
      this.button_OK.Location = new System.Drawing.Point(192, 376);
      this.button_OK.Name = "button_OK";
      this.button_OK.Size = new System.Drawing.Size(72, 24);
      this.button_OK.TabIndex = 1;
      this.button_OK.Text = "Apply";
      this.button_OK.Click += new System.EventHandler(this.button_OK_Click);
      // 
      // button_Cancel
      // 
      this.button_Cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.button_Cancel.Location = new System.Drawing.Point(280, 376);
      this.button_Cancel.Name = "button_Cancel";
      this.button_Cancel.Size = new System.Drawing.Size(72, 24);
      this.button_Cancel.TabIndex = 2;
      this.button_Cancel.Text = "Cancel";
      this.button_Cancel.Click += new System.EventHandler(this.button_Cancel_Click);
      // 
      // LinearStartDist
      // 
      this.LinearStartDist.LargeChange = 1000;
      this.LinearStartDist.Location = new System.Drawing.Point(32, 80);
      this.LinearStartDist.Maximum = 100000;
      this.LinearStartDist.Minimum = -100000;
      this.LinearStartDist.Name = "LinearStartDist";
      this.LinearStartDist.Size = new System.Drawing.Size(416, 42);
      this.LinearStartDist.SmallChange = 100;
      this.LinearStartDist.TabIndex = 3;
      this.LinearStartDist.TickFrequency = 1000;
      this.LinearStartDist.TickStyle = System.Windows.Forms.TickStyle.TopLeft;
      this.LinearStartDist.Scroll += new System.EventHandler(this.LinearStartDist_Scroll);
      // 
      // LinearDepth
      // 
      this.LinearDepth.LargeChange = 1000;
      this.LinearDepth.Location = new System.Drawing.Point(32, 136);
      this.LinearDepth.Maximum = 100000;
      this.LinearDepth.Minimum = 1;
      this.LinearDepth.Name = "LinearDepth";
      this.LinearDepth.Size = new System.Drawing.Size(416, 42);
      this.LinearDepth.SmallChange = 100;
      this.LinearDepth.TabIndex = 4;
      this.LinearDepth.TickFrequency = 1000;
      this.LinearDepth.TickStyle = System.Windows.Forms.TickStyle.TopLeft;
      this.LinearDepth.Value = 1000;
      this.LinearDepth.Scroll += new System.EventHandler(this.LinearDepth_Scroll);
      // 
      // radioButton_Lin
      // 
      this.radioButton_Lin.BackColor = System.Drawing.SystemColors.Control;
      this.radioButton_Lin.Checked = true;
      this.radioButton_Lin.Location = new System.Drawing.Point(16, 40);
      this.radioButton_Lin.Name = "radioButton_Lin";
      this.radioButton_Lin.Size = new System.Drawing.Size(112, 16);
      this.radioButton_Lin.TabIndex = 5;
      this.radioButton_Lin.TabStop = true;
      this.radioButton_Lin.Text = "Linear Depth Fog";
      this.radioButton_Lin.CheckedChanged += new System.EventHandler(this.radioButton_Lin_CheckedChanged);
      // 
      // groupBox_lin
      // 
      this.groupBox_lin.Controls.Add(this.label_LinDepth);
      this.groupBox_lin.Controls.Add(this.numeric_LinDepth);
      this.groupBox_lin.Controls.Add(this.label_LinStart);
      this.groupBox_lin.Location = new System.Drawing.Point(16, 40);
      this.groupBox_lin.Name = "groupBox_lin";
      this.groupBox_lin.Size = new System.Drawing.Size(520, 144);
      this.groupBox_lin.TabIndex = 6;
      this.groupBox_lin.TabStop = false;
      // 
      // label_LinDepth
      // 
      this.label_LinDepth.Location = new System.Drawing.Point(8, 80);
      this.label_LinDepth.Name = "label_LinDepth";
      this.label_LinDepth.Size = new System.Drawing.Size(64, 16);
      this.label_LinDepth.TabIndex = 9;
      this.label_LinDepth.Text = "Fog depth :";
      this.label_LinDepth.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
      // 
      // numeric_LinDepth
      // 
      this.numeric_LinDepth.Location = new System.Drawing.Point(440, 96);
      this.numeric_LinDepth.Maximum = new System.Decimal(new int[] {
                                                                     100000,
                                                                     0,
                                                                     0,
                                                                     0});
      this.numeric_LinDepth.Minimum = new System.Decimal(new int[] {
                                                                     1,
                                                                     0,
                                                                     0,
                                                                     0});
      this.numeric_LinDepth.Name = "numeric_LinDepth";
      this.numeric_LinDepth.Size = new System.Drawing.Size(72, 20);
      this.numeric_LinDepth.TabIndex = 10;
      this.numeric_LinDepth.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
      this.numeric_LinDepth.ThousandsSeparator = true;
      this.numeric_LinDepth.Value = new System.Decimal(new int[] {
                                                                   1,
                                                                   0,
                                                                   0,
                                                                   0});
      this.numeric_LinDepth.Validated += new System.EventHandler(this.numeric_LinDepth_Validated);
      this.numeric_LinDepth.ValueChanged += new System.EventHandler(this.numeric_LinDepth_ValueChanged);
      // 
      // label_LinStart
      // 
      this.label_LinStart.Location = new System.Drawing.Point(8, 24);
      this.label_LinStart.Name = "label_LinStart";
      this.label_LinStart.Size = new System.Drawing.Size(56, 16);
      this.label_LinStart.TabIndex = 0;
      this.label_LinStart.Text = "Fog start :";
      this.label_LinStart.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
      // 
      // numeric_LinStart
      // 
      this.numeric_LinStart.Location = new System.Drawing.Point(456, 80);
      this.numeric_LinStart.Maximum = new System.Decimal(new int[] {
                                                                     100000,
                                                                     0,
                                                                     0,
                                                                     0});
      this.numeric_LinStart.Minimum = new System.Decimal(new int[] {
                                                                     100000,
                                                                     0,
                                                                     0,
                                                                     -2147483648});
      this.numeric_LinStart.Name = "numeric_LinStart";
      this.numeric_LinStart.Size = new System.Drawing.Size(72, 20);
      this.numeric_LinStart.TabIndex = 8;
      this.numeric_LinStart.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
      this.numeric_LinStart.ThousandsSeparator = true;
      this.numeric_LinStart.Validated += new System.EventHandler(this.numeric_LinStart_Validated);
      this.numeric_LinStart.ValueChanged += new System.EventHandler(this.numeric_LinStart_ValueChanged);
      // 
      // radioButton_noFog
      // 
      this.radioButton_noFog.BackColor = System.Drawing.SystemColors.Control;
      this.radioButton_noFog.Location = new System.Drawing.Point(16, 16);
      this.radioButton_noFog.Name = "radioButton_noFog";
      this.radioButton_noFog.Size = new System.Drawing.Size(88, 16);
      this.radioButton_noFog.TabIndex = 9;
      this.radioButton_noFog.Text = "No Fog";
      this.radioButton_noFog.CheckedChanged += new System.EventHandler(this.radioButton_noFog_CheckedChanged);
      // 
      // radioButton_Exp
      // 
      this.radioButton_Exp.BackColor = System.Drawing.SystemColors.Control;
      this.radioButton_Exp.Location = new System.Drawing.Point(16, 192);
      this.radioButton_Exp.Name = "radioButton_Exp";
      this.radioButton_Exp.Size = new System.Drawing.Size(144, 16);
      this.radioButton_Exp.TabIndex = 10;
      this.radioButton_Exp.Text = "Exponential  Depth Fog";
      this.radioButton_Exp.CheckedChanged += new System.EventHandler(this.radioButton_Exp_CheckedChanged);
      // 
      // groupBox_Exp
      // 
      this.groupBox_Exp.Controls.Add(this.label_Exponent);
      this.groupBox_Exp.Controls.Add(this.trackBar_Exponent);
      this.groupBox_Exp.Controls.Add(this.numericUpDown_Exp);
      this.groupBox_Exp.Location = new System.Drawing.Point(16, 192);
      this.groupBox_Exp.Name = "groupBox_Exp";
      this.groupBox_Exp.Size = new System.Drawing.Size(520, 88);
      this.groupBox_Exp.TabIndex = 11;
      this.groupBox_Exp.TabStop = false;
      // 
      // label_Exponent
      // 
      this.label_Exponent.Location = new System.Drawing.Point(16, 24);
      this.label_Exponent.Name = "label_Exponent";
      this.label_Exponent.Size = new System.Drawing.Size(64, 16);
      this.label_Exponent.TabIndex = 12;
      this.label_Exponent.Text = "Exponent :";
      this.label_Exponent.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
      // 
      // trackBar_Exponent
      // 
      this.trackBar_Exponent.LargeChange = 1000;
      this.trackBar_Exponent.Location = new System.Drawing.Point(16, 40);
      this.trackBar_Exponent.Maximum = 100000;
      this.trackBar_Exponent.Name = "trackBar_Exponent";
      this.trackBar_Exponent.Size = new System.Drawing.Size(416, 42);
      this.trackBar_Exponent.SmallChange = 10;
      this.trackBar_Exponent.TabIndex = 13;
      this.trackBar_Exponent.TickFrequency = 1000;
      this.trackBar_Exponent.TickStyle = System.Windows.Forms.TickStyle.TopLeft;
      this.trackBar_Exponent.Scroll += new System.EventHandler(this.trackBar_Exponent_Scroll);
      // 
      // numericUpDown_Exp
      // 
      this.numericUpDown_Exp.DecimalPlaces = 5;
      this.numericUpDown_Exp.Increment = new System.Decimal(new int[] {
                                                                        2,
                                                                        0,
                                                                        0,
                                                                        327680});
      this.numericUpDown_Exp.Location = new System.Drawing.Point(440, 40);
      this.numericUpDown_Exp.Maximum = new System.Decimal(new int[] {
                                                                      2,
                                                                      0,
                                                                      0,
                                                                      131072});
      this.numericUpDown_Exp.Name = "numericUpDown_Exp";
      this.numericUpDown_Exp.Size = new System.Drawing.Size(72, 20);
      this.numericUpDown_Exp.TabIndex = 14;
      this.numericUpDown_Exp.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
      this.numericUpDown_Exp.ThousandsSeparator = true;
      this.numericUpDown_Exp.Value = new System.Decimal(new int[] {
                                                                    1,
                                                                    0,
                                                                    0,
                                                                    262144});
      this.numericUpDown_Exp.Validated += new System.EventHandler(this.numericUpDown_Exp_Validated);
      this.numericUpDown_Exp.ValueChanged += new System.EventHandler(this.numericUpDown_Exp_ValueChanged);
      // 
      // ButtonFogColor
      // 
      this.ButtonFogColor.BackColor = System.Drawing.Color.Red;
      this.ButtonFogColor.Location = new System.Drawing.Point(32, 312);
      this.ButtonFogColor.Name = "ButtonFogColor";
      this.ButtonFogColor.Size = new System.Drawing.Size(104, 32);
      this.ButtonFogColor.TabIndex = 12;
      this.ButtonFogColor.Click += new System.EventHandler(this.ButtonFogColor_Click);
      // 
      // groupBox_Color
      // 
      this.groupBox_Color.Location = new System.Drawing.Point(16, 288);
      this.groupBox_Color.Name = "groupBox_Color";
      this.groupBox_Color.Size = new System.Drawing.Size(520, 72);
      this.groupBox_Color.TabIndex = 13;
      this.groupBox_Color.TabStop = false;
      this.groupBox_Color.Text = "Fog color";
      // 
      // DepthFogPropertiesDlg
      // 
      this.AcceptButton = this.button_OK;
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.CancelButton = this.button_Cancel;
      this.ClientSize = new System.Drawing.Size(546, 415);
      this.Controls.Add(this.ButtonFogColor);
      this.Controls.Add(this.radioButton_Exp);
      this.Controls.Add(this.radioButton_noFog);
      this.Controls.Add(this.LinearDepth);
      this.Controls.Add(this.numeric_LinStart);
      this.Controls.Add(this.radioButton_Lin);
      this.Controls.Add(this.LinearStartDist);
      this.Controls.Add(this.button_Cancel);
      this.Controls.Add(this.button_OK);
      this.Controls.Add(this.groupBox_lin);
      this.Controls.Add(this.groupBox_Exp);
      this.Controls.Add(this.groupBox_Color);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "DepthFogPropertiesDlg";
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
      this.Text = "Depth Fog Properties";
      this.Closing += new System.ComponentModel.CancelEventHandler(this.DepthFogPropertiesDlg_Closing);
      ((System.ComponentModel.ISupportInitialize)(this.LinearStartDist)).EndInit();
      ((System.ComponentModel.ISupportInitialize)(this.LinearDepth)).EndInit();
      this.groupBox_lin.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.numeric_LinDepth)).EndInit();
      ((System.ComponentModel.ISupportInitialize)(this.numeric_LinStart)).EndInit();
      this.groupBox_Exp.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.trackBar_Exponent)).EndInit();
      ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_Exp)).EndInit();
      this.ResumeLayout(false);

    }

		#endregion

    public DepthFogShape.DepthFogType FogType
    {
      get {return fogType;}
      set 
      {
        fogType = value;
        // update UI
        bool bEnabled;
        bEnabled = fogType==DepthFogShape.DepthFogType.None;
          radioButton_noFog.Checked = bEnabled;

        bEnabled = fogType==DepthFogShape.DepthFogType.Linear;
          radioButton_Lin.Checked = bEnabled;
          LinearStartDist.Enabled = bEnabled;
          LinearDepth.Enabled = bEnabled;
          numeric_LinStart.Enabled = bEnabled;
          numeric_LinDepth.Enabled = bEnabled;
          label_LinStart.Enabled = bEnabled;
          label_LinDepth.Enabled = bEnabled;
          groupBox_lin.Enabled = bEnabled;

        bEnabled = fogType==DepthFogShape.DepthFogType.Exponential;
          groupBox_Exp.Enabled = bEnabled;
          radioButton_Exp.Checked = bEnabled;
          trackBar_Exponent.Enabled = bEnabled;
          numericUpDown_Exp.Enabled = bEnabled;

        bEnabled = fogType!=DepthFogShape.DepthFogType.None;
          groupBox_Color.Enabled = bEnabled;
          ButtonFogColor.Enabled = bEnabled;

        UpdateFogView();
      }
    }

    DepthFogShape.DepthFogType fogType = DepthFogShape.DepthFogType.None;
    
 

    private void button_OK_Click(object sender, System.EventArgs e)
    {
      // get dialog data
      DialogResult = DialogResult.OK;
    }

    private void button_Cancel_Click(object sender, System.EventArgs e)
    {
      DialogResult = DialogResult.Cancel;
    }

    private void DepthFogPropertiesDlg_Closing(object sender, System.ComponentModel.CancelEventArgs e)
    {
      if (DialogResult==DialogResult.OK)
      {
        Shape.MemberwiseCopy(CopyShape);
      }
      Shape.UpdateEngineFog();
    }

    void UpdateFogView()
    {
      if (!bAllowUpdate) return;

      // update the shape's values so its engine instance updates the view
      CopyShape.FogType = FogType;
      CopyShape.LinearStart = LinearStartDist.Value;
      CopyShape.LinearDepth = LinearDepth.Value;
      CopyShape.FogColor = ButtonFogColor.BackColor;

      CopyShape.Exponent = (float)numericUpDown_Exp.Value;
      EditorApp.ActiveView.UpdateView(true);
    }

    private void LinearStartDist_Scroll(object sender, System.EventArgs e)
    {
      numeric_LinStart.Value = LinearStartDist.Value;
      UpdateFogView();
    }

    private void LinearDepth_Scroll(object sender, System.EventArgs e)
    {
      numeric_LinDepth.Value = LinearDepth.Value;
      UpdateFogView();
    }

    private void trackBar_Exponent_Scroll(object sender, System.EventArgs e)
    {
      numericUpDown_Exp.Value = (decimal)((double)trackBar_Exponent.Value / (double)trackBar_Exponent.Maximum / 50);
      UpdateFogView();
    }

    private void numeric_LinStart_ValueChanged(object sender, System.EventArgs e)
    {
      LinearStartDist.Value = (int)numeric_LinStart.Value; 
      UpdateFogView();
    }

    private void numeric_LinDepth_ValueChanged(object sender, System.EventArgs e)
    {
      LinearDepth.Value = (int)numeric_LinDepth.Value;
      UpdateFogView();
    }

   
    private void numeric_LinStart_Validated(object sender, System.EventArgs e)
    {
      LinearStartDist.Value = (int)numeric_LinStart.Value; 
      UpdateFogView();
    }

    private void numeric_LinDepth_Validated(object sender, System.EventArgs e)
    {
      LinearDepth.Value = (int)numeric_LinDepth.Value;
      UpdateFogView();
    }

    private void numericUpDown_Exp_ValueChanged(object sender, System.EventArgs e)
    {
      trackBar_Exponent.Value = (int)(numericUpDown_Exp.Value*(decimal)trackBar_Exponent.Maximum*50);
      UpdateFogView();
    }

    private void numericUpDown_Exp_Validated(object sender, System.EventArgs e)
    {
      trackBar_Exponent.Value = (int)(numericUpDown_Exp.Value*(decimal)trackBar_Exponent.Maximum*50);
      UpdateFogView();
    }


    private void radioButton_noFog_CheckedChanged(object sender, System.EventArgs e)
    {
      if (radioButton_noFog.Checked)
        FogType = DepthFogShape.DepthFogType.None;
    }

    private void radioButton_Lin_CheckedChanged(object sender, System.EventArgs e)
    {
      if (radioButton_Lin.Checked)
        FogType = DepthFogShape.DepthFogType.Linear;
    }

    private void radioButton_Exp_CheckedChanged(object sender, System.EventArgs e)
    {
      if (radioButton_Exp.Checked)
        FogType = DepthFogShape.DepthFogType.Exponential;
    }

    private void ButtonFogColor_Click(object sender, System.EventArgs e)
    {
      FogColorDialog.AllowFullOpen = true;
      FogColorDialog.FullOpen = true;
      FogColorDialog.Color = ButtonFogColor.BackColor;
      DialogResult result = FogColorDialog.ShowDialog();
      if (result==DialogResult.OK)
      {
        ButtonFogColor.BackColor = FogColorDialog.Color;
        UpdateFogView();
      }
    }







  }
}
*/

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
