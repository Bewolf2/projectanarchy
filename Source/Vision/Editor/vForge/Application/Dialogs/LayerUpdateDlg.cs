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

namespace Editor.Dialogs
{
	/// <summary>
	/// Dialog class for layer selection
	/// </summary>
	public class LayerUpdateDlg : System.Windows.Forms.Form
	{
    private System.Windows.Forms.Button button_OK;
    private System.Windows.Forms.Button button_Abort;
    private System.Windows.Forms.Panel panel1;
    private CSharpFramework.Controls.DialogCaptionBar captionBar;
    private System.Windows.Forms.ListView listView_Layers;
    private System.Windows.Forms.ColumnHeader columnHeader_Name;
    private System.Windows.Forms.ColumnHeader columnHeader_Status;
    private System.Windows.Forms.ImageList imageList_LayerIcons;
    private System.ComponentModel.IContainer components;

    /// <summary>
    /// Constructor
    /// </summary>
		public LayerUpdateDlg()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
		}

    static int IMAGE_NEW      = 0;
    static int IMAGE_DELETED  = 1;
    static int IMAGE_MODIFIED = 2;

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
      this.components = new System.ComponentModel.Container();
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(LayerUpdateDlg));
      this.button_OK = new System.Windows.Forms.Button();
      this.button_Abort = new System.Windows.Forms.Button();
      this.panel1 = new System.Windows.Forms.Panel();
      this.captionBar = new CSharpFramework.Controls.DialogCaptionBar();
      this.listView_Layers = new System.Windows.Forms.ListView();
      this.columnHeader_Name = new System.Windows.Forms.ColumnHeader();
      this.columnHeader_Status = new System.Windows.Forms.ColumnHeader();
      this.imageList_LayerIcons = new System.Windows.Forms.ImageList(this.components);
      this.panel1.SuspendLayout();
      this.SuspendLayout();
      // 
      // button_OK
      // 
      this.button_OK.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
      this.button_OK.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.button_OK.Location = new System.Drawing.Point(88, 24);
      this.button_OK.Name = "button_OK";
      this.button_OK.Size = new System.Drawing.Size(112, 24);
      this.button_OK.TabIndex = 0;
      this.button_OK.Text = "Accept changes";
      // 
      // button_Abort
      // 
      this.button_Abort.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
      this.button_Abort.DialogResult = System.Windows.Forms.DialogResult.Abort;
      this.button_Abort.Location = new System.Drawing.Point(216, 24);
      this.button_Abort.Name = "button_Abort";
      this.button_Abort.Size = new System.Drawing.Size(104, 24);
      this.button_Abort.TabIndex = 1;
      this.button_Abort.Text = "Abort";
      // 
      // panel1
      // 
      this.panel1.Controls.Add(this.button_OK);
      this.panel1.Controls.Add(this.button_Abort);
      this.panel1.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.panel1.Location = new System.Drawing.Point(0, 390);
      this.panel1.Name = "panel1";
      this.panel1.Size = new System.Drawing.Size(416, 56);
      this.panel1.TabIndex = 2;
      // 
      // captionBar
      // 
      this.captionBar.BackColor = System.Drawing.SystemColors.Window;
      this.captionBar.Caption = "Layer Update Result";
      this.captionBar.CompactView = false;
      this.captionBar.Description = "The following changes have been detected. Click \'Accept changes\' to accept change" +
          "s or \'Abort\' to leave the layers unmodified";
      this.captionBar.Dock = System.Windows.Forms.DockStyle.Top;
      this.captionBar.Image = ((System.Drawing.Image)(resources.GetObject("captionBar.Image")));
      this.captionBar.Location = new System.Drawing.Point(0, 0);
      this.captionBar.Name = "captionBar";
      this.captionBar.SetFontColor = System.Drawing.SystemColors.ControlText;
      this.captionBar.ShowBorder = false;
      this.captionBar.ShowBottomLine = true;
      this.captionBar.ShowCaptionText = true;
      this.captionBar.ShowImage = true;
      this.captionBar.Size = new System.Drawing.Size(416, 50);
      this.captionBar.TabIndex = 0;
      // 
      // listView_Layers
      // 
      this.listView_Layers.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader_Name,
            this.columnHeader_Status});
      this.listView_Layers.Dock = System.Windows.Forms.DockStyle.Fill;
      this.listView_Layers.Location = new System.Drawing.Point(0, 50);
      this.listView_Layers.MultiSelect = false;
      this.listView_Layers.Name = "listView_Layers";
      this.listView_Layers.Size = new System.Drawing.Size(416, 340);
      this.listView_Layers.SmallImageList = this.imageList_LayerIcons;
      this.listView_Layers.TabIndex = 1;
      this.listView_Layers.UseCompatibleStateImageBehavior = false;
      this.listView_Layers.View = System.Windows.Forms.View.Details;
      // 
      // columnHeader_Name
      // 
      this.columnHeader_Name.Text = "Layer name";
      this.columnHeader_Name.Width = 212;
      // 
      // columnHeader_Status
      // 
      this.columnHeader_Status.Text = "Status";
      this.columnHeader_Status.Width = 100;
      // 
      // imageList_LayerIcons
      // 
      this.imageList_LayerIcons.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList_LayerIcons.ImageStream")));
      this.imageList_LayerIcons.TransparentColor = System.Drawing.Color.Magenta;
      this.imageList_LayerIcons.Images.SetKeyName(0, "");
      this.imageList_LayerIcons.Images.SetKeyName(1, "");
      this.imageList_LayerIcons.Images.SetKeyName(2, "");
      // 
      // LayerUpdateDlg
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(416, 446);
      this.Controls.Add(this.listView_Layers);
      this.Controls.Add(this.captionBar);
      this.Controls.Add(this.panel1);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.MinimumSize = new System.Drawing.Size(328, 224);
      this.Name = "LayerUpdateDlg";
      this.Text = "Refresh Layer";
      this.panel1.ResumeLayout(false);
      this.ResumeLayout(false);

    }
		#endregion

    private void listView_Layers_SelectedIndexChanged(object sender, System.EventArgs e)
    {
    
    }


    /// <summary>
    /// Sets the collection of layers to be diplayed as items
    /// </summary>
    /// <param name="layers"></param>
    public void SetModifiedFiles(LayerCollection layers, ZoneCollection zones)
    {
      listView_Layers.BeginUpdate();
      listView_Layers.Items.Clear();

      foreach (Zone zone in zones)
      {
        ListViewItem item = listView_Layers.Items.Add(zone.ZoneName, IMAGE_NEW);
        item.SubItems.Add("New");
      }

      foreach (Layer layer in layers)
      {
        Layer.LayerFileStatus_e status = layer.FileStatus;
        int iImage = IMAGE_MODIFIED;
        string statusStr = "";
        switch (status)
        {
          case Layer.LayerFileStatus_e.Deleted:
            iImage = IMAGE_DELETED;
            statusStr = "Deleted";
            break;
          case Layer.LayerFileStatus_e.Modified:
            iImage = IMAGE_MODIFIED;
            statusStr = "Modified";
            break;
          case Layer.LayerFileStatus_e.NewLayer:
            iImage = IMAGE_NEW;
            statusStr = "New";
            break;
          default:
            System.Diagnostics.Debug.Assert(false,"Should never get here");
            break;
        }

        ListViewItem item = listView_Layers.Items.Add(layer.LayerName, iImage);
        item.SubItems.Add(statusStr);
      }
      listView_Layers.EndUpdate();
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
