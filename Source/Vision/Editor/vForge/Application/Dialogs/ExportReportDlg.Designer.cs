namespace Editor.Dialogs
{
  partial class ExportReportDlg
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
      if (disposing && (components != null))
      {
        components.Dispose();
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ExportReportDlg));
      this.btnOK = new System.Windows.Forms.Button();
      this.reportlist = new System.Windows.Forms.ListView();
      this.Warnings = new System.Windows.Forms.ColumnHeader();
      this.imageList = new System.Windows.Forms.ImageList(this.components);
      this.SuspendLayout();
      // 
      // btnOK
      // 
      this.btnOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.btnOK.Location = new System.Drawing.Point(446, 280);
      this.btnOK.Name = "btnOK";
      this.btnOK.Size = new System.Drawing.Size(75, 23);
      this.btnOK.TabIndex = 0;
      this.btnOK.Text = "OK";
      this.btnOK.UseVisualStyleBackColor = true;
      this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
      // 
      // reportlist
      // 
      this.reportlist.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.reportlist.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.Warnings});
      this.reportlist.FullRowSelect = true;
      this.reportlist.GridLines = true;
      this.reportlist.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.None;
      this.reportlist.Location = new System.Drawing.Point(12, 12);
      this.reportlist.MultiSelect = false;
      this.reportlist.Name = "reportlist";
      this.reportlist.Size = new System.Drawing.Size(509, 262);
      this.reportlist.TabIndex = 1;
      this.reportlist.UseCompatibleStateImageBehavior = false;
      this.reportlist.View = System.Windows.Forms.View.Details;
      this.reportlist.DoubleClick += new System.EventHandler(this.reportlist_DoubleClick);
      // 
      // imageList
      // 
      this.imageList.ColorDepth = System.Windows.Forms.ColorDepth.Depth8Bit;
      this.imageList.ImageSize = new System.Drawing.Size(16, 16);
      this.imageList.TransparentColor = System.Drawing.Color.Transparent;
      // 
      // ExportReportDlg
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(533, 315);
      this.Controls.Add(this.reportlist);
      this.Controls.Add(this.btnOK);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "ExportReportDlg";
      this.Text = "Export Report";
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.Button btnOK;
    private System.Windows.Forms.ListView reportlist;
    private System.Windows.Forms.ColumnHeader Warnings;
    private System.Windows.Forms.ImageList imageList;
  }
}