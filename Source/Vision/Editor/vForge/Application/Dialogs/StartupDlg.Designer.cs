namespace Editor.Dialogs
{
  partial class StartupDlg
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(StartupDlg));
      this.imageList = new System.Windows.Forms.ImageList(this.components);
      this.checkBox_ShowDialog = new System.Windows.Forms.CheckBox();
      this.button_Close = new System.Windows.Forms.Button();
      this.panel_Footer = new System.Windows.Forms.Panel();
      this.panel_Header = new System.Windows.Forms.Panel();
      this.label_Description = new System.Windows.Forms.Label();
      this.label_Title = new System.Windows.Forms.Label();
      this.panel_Start = new System.Windows.Forms.Panel();
      this.label14 = new System.Windows.Forms.Label();
      this.label13 = new System.Windows.Forms.Label();
      this.label12 = new System.Windows.Forms.Label();
      this.label11 = new System.Windows.Forms.Label();
      this.label10 = new System.Windows.Forms.Label();
      this.label9 = new System.Windows.Forms.Label();
      this.label4 = new System.Windows.Forms.Label();
      this.label5 = new System.Windows.Forms.Label();
      this.label6 = new System.Windows.Forms.Label();
      this.label3 = new System.Windows.Forms.Label();
      this.label2 = new System.Windows.Forms.Label();
      this.label1 = new System.Windows.Forms.Label();
      this.button_SampleMaps = new System.Windows.Forms.Button();
      this.button_StartupDoc = new System.Windows.Forms.Button();
      this.button_NewProject = new System.Windows.Forms.Button();
      this.button_StartupProject = new System.Windows.Forms.Button();
      this.button_OpenProject = new System.Windows.Forms.Button();
      this.button_OpenScene = new System.Windows.Forms.Button();
      this.panel_Recent = new System.Windows.Forms.Panel();
      this.label_Scenes = new System.Windows.Forms.Label();
      this.label_Projects = new System.Windows.Forms.Label();
      this.listView_Scenes = new System.Windows.Forms.ListView();
      this.listView_Projects = new System.Windows.Forms.ListView();
      this.button_Switch = new System.Windows.Forms.Button();
      this.panel_Footer.SuspendLayout();
      this.panel_Header.SuspendLayout();
      this.panel_Start.SuspendLayout();
      this.panel_Recent.SuspendLayout();
      this.SuspendLayout();
      // 
      // imageList
      // 
      this.imageList.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList.ImageStream")));
      this.imageList.TransparentColor = System.Drawing.Color.Transparent;
      this.imageList.Images.SetKeyName(0, "project");
      this.imageList.Images.SetKeyName(1, "scene");
      // 
      // checkBox_ShowDialog
      // 
      this.checkBox_ShowDialog.AutoSize = true;
      this.checkBox_ShowDialog.BackColor = System.Drawing.Color.Transparent;
      this.checkBox_ShowDialog.ForeColor = System.Drawing.Color.White;
      this.checkBox_ShowDialog.Location = new System.Drawing.Point(150, 9);
      this.checkBox_ShowDialog.Name = "checkBox_ShowDialog";
      this.checkBox_ShowDialog.Size = new System.Drawing.Size(102, 17);
      this.checkBox_ShowDialog.TabIndex = 1;
      this.checkBox_ShowDialog.Text = "Show at Startup";
      this.checkBox_ShowDialog.UseVisualStyleBackColor = false;
      this.checkBox_ShowDialog.CheckedChanged += new System.EventHandler(this.checkBox_ShowDialog_CheckedChanged);
      // 
      // button_Close
      // 
      this.button_Close.BackColor = System.Drawing.Color.Transparent;
      this.button_Close.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.button_Close.Location = new System.Drawing.Point(30, 6);
      this.button_Close.Name = "button_Close";
      this.button_Close.Size = new System.Drawing.Size(104, 22);
      this.button_Close.TabIndex = 0;
      this.button_Close.Text = "Close";
      this.button_Close.UseVisualStyleBackColor = false;
      this.button_Close.Click += new System.EventHandler(this.button_Close_Click);
      // 
      // panel_Footer
      // 
      this.panel_Footer.BackColor = System.Drawing.Color.Transparent;
      this.panel_Footer.Controls.Add(this.button_Switch);
      this.panel_Footer.Controls.Add(this.checkBox_ShowDialog);
      this.panel_Footer.Controls.Add(this.button_Close);
      this.panel_Footer.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.panel_Footer.Location = new System.Drawing.Point(0, 384);
      this.panel_Footer.Name = "panel_Footer";
      this.panel_Footer.Size = new System.Drawing.Size(600, 36);
      this.panel_Footer.TabIndex = 2;
      // 
      // panel_Header
      // 
      this.panel_Header.BackColor = System.Drawing.Color.Transparent;
      this.panel_Header.Controls.Add(this.label_Description);
      this.panel_Header.Controls.Add(this.label_Title);
      this.panel_Header.Dock = System.Windows.Forms.DockStyle.Top;
      this.panel_Header.Location = new System.Drawing.Point(0, 0);
      this.panel_Header.Name = "panel_Header";
      this.panel_Header.Size = new System.Drawing.Size(600, 60);
      this.panel_Header.TabIndex = 0;
      // 
      // label_Description
      // 
      this.label_Description.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.label_Description.BackColor = System.Drawing.Color.Transparent;
      this.label_Description.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
      this.label_Description.ForeColor = System.Drawing.Color.White;
      this.label_Description.Location = new System.Drawing.Point(27, 29);
      this.label_Description.Name = "label_Description";
      this.label_Description.Padding = new System.Windows.Forms.Padding(0, 3, 0, 0);
      this.label_Description.Size = new System.Drawing.Size(543, 27);
      this.label_Description.TabIndex = 1;
      this.label_Description.Text = "Select one of the following options to get started or open a recent project or sc" +
          "ene.";
      // 
      // label_Title
      // 
      this.label_Title.AutoSize = true;
      this.label_Title.BackColor = System.Drawing.Color.Transparent;
      this.label_Title.Font = new System.Drawing.Font("Tahoma", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.label_Title.ForeColor = System.Drawing.Color.White;
      this.label_Title.Location = new System.Drawing.Point(26, 11);
      this.label_Title.Name = "label_Title";
      this.label_Title.Size = new System.Drawing.Size(159, 19);
      this.label_Title.TabIndex = 0;
      this.label_Title.Text = "Welcome to Vision";
      // 
      // panel_Start
      // 
      this.panel_Start.BackColor = System.Drawing.Color.Transparent;
      this.panel_Start.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
      this.panel_Start.Controls.Add(this.label14);
      this.panel_Start.Controls.Add(this.label13);
      this.panel_Start.Controls.Add(this.label12);
      this.panel_Start.Controls.Add(this.label11);
      this.panel_Start.Controls.Add(this.label10);
      this.panel_Start.Controls.Add(this.label9);
      this.panel_Start.Controls.Add(this.label4);
      this.panel_Start.Controls.Add(this.label5);
      this.panel_Start.Controls.Add(this.label6);
      this.panel_Start.Controls.Add(this.label3);
      this.panel_Start.Controls.Add(this.label2);
      this.panel_Start.Controls.Add(this.label1);
      this.panel_Start.Controls.Add(this.button_SampleMaps);
      this.panel_Start.Controls.Add(this.button_StartupDoc);
      this.panel_Start.Controls.Add(this.button_NewProject);
      this.panel_Start.Controls.Add(this.button_StartupProject);
      this.panel_Start.Controls.Add(this.button_OpenProject);
      this.panel_Start.Controls.Add(this.button_OpenScene);
      this.panel_Start.Dock = System.Windows.Forms.DockStyle.Fill;
      this.panel_Start.Location = new System.Drawing.Point(0, 60);
      this.panel_Start.Name = "panel_Start";
      this.panel_Start.Padding = new System.Windows.Forms.Padding(16);
      this.panel_Start.Size = new System.Drawing.Size(600, 324);
      this.panel_Start.TabIndex = 1;
      // 
      // label14
      // 
      this.label14.BackColor = System.Drawing.Color.Transparent;
      this.label14.Location = new System.Drawing.Point(111, 252);
      this.label14.Name = "label14";
      this.label14.Padding = new System.Windows.Forms.Padding(0, 3, 0, 0);
      this.label14.Size = new System.Drawing.Size(206, 34);
      this.label14.TabIndex = 14;
      this.label14.Text = "Open a map from the Engine Samples";
      // 
      // label13
      // 
      this.label13.BackColor = System.Drawing.Color.Transparent;
      this.label13.Location = new System.Drawing.Point(111, 160);
      this.label13.Name = "label13";
      this.label13.Padding = new System.Windows.Forms.Padding(0, 3, 0, 0);
      this.label13.Size = new System.Drawing.Size(206, 34);
      this.label13.TabIndex = 8;
      this.label13.Text = "Open the Getting Started project";
      // 
      // label12
      // 
      this.label12.BackColor = System.Drawing.Color.Transparent;
      this.label12.Location = new System.Drawing.Point(111, 68);
      this.label12.Name = "label12";
      this.label12.Padding = new System.Windows.Forms.Padding(0, 3, 0, 0);
      this.label12.RightToLeft = System.Windows.Forms.RightToLeft.No;
      this.label12.Size = new System.Drawing.Size(204, 34);
      this.label12.TabIndex = 2;
      this.label12.Text = "Open the Getting Started guide";
      // 
      // label11
      // 
      this.label11.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.label11.BackColor = System.Drawing.Color.Transparent;
      this.label11.Location = new System.Drawing.Point(329, 252);
      this.label11.Name = "label11";
      this.label11.Padding = new System.Windows.Forms.Padding(0, 3, 0, 0);
      this.label11.Size = new System.Drawing.Size(160, 34);
      this.label11.TabIndex = 16;
      this.label11.Text = "Open an exisiting scene";
      this.label11.TextAlign = System.Drawing.ContentAlignment.TopRight;
      // 
      // label10
      // 
      this.label10.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.label10.BackColor = System.Drawing.Color.Transparent;
      this.label10.Location = new System.Drawing.Point(329, 160);
      this.label10.Name = "label10";
      this.label10.Padding = new System.Windows.Forms.Padding(0, 3, 0, 0);
      this.label10.Size = new System.Drawing.Size(160, 34);
      this.label10.TabIndex = 10;
      this.label10.Text = "Open an exisiting project";
      this.label10.TextAlign = System.Drawing.ContentAlignment.TopRight;
      // 
      // label9
      // 
      this.label9.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.label9.BackColor = System.Drawing.Color.Transparent;
      this.label9.Location = new System.Drawing.Point(332, 68);
      this.label9.Name = "label9";
      this.label9.Padding = new System.Windows.Forms.Padding(0, 3, 0, 0);
      this.label9.Size = new System.Drawing.Size(157, 34);
      this.label9.TabIndex = 4;
      this.label9.Text = "Create a new project";
      this.label9.TextAlign = System.Drawing.ContentAlignment.TopRight;
      // 
      // label4
      // 
      this.label4.BackColor = System.Drawing.Color.Transparent;
      this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
      this.label4.Location = new System.Drawing.Point(111, 236);
      this.label4.Name = "label4";
      this.label4.Size = new System.Drawing.Size(206, 32);
      this.label4.TabIndex = 13;
      this.label4.Text = "Engine Sample Maps";
      // 
      // label5
      // 
      this.label5.BackColor = System.Drawing.Color.Transparent;
      this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
      this.label5.Location = new System.Drawing.Point(111, 144);
      this.label5.Name = "label5";
      this.label5.Size = new System.Drawing.Size(206, 32);
      this.label5.TabIndex = 7;
      this.label5.Text = "Getting Started Project";
      // 
      // label6
      // 
      this.label6.BackColor = System.Drawing.Color.Transparent;
      this.label6.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
      this.label6.Location = new System.Drawing.Point(111, 52);
      this.label6.Name = "label6";
      this.label6.Size = new System.Drawing.Size(206, 32);
      this.label6.TabIndex = 1;
      this.label6.Text = "Getting Started Guide";
      // 
      // label3
      // 
      this.label3.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.label3.BackColor = System.Drawing.Color.Transparent;
      this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
      this.label3.Location = new System.Drawing.Point(329, 236);
      this.label3.Name = "label3";
      this.label3.Size = new System.Drawing.Size(160, 32);
      this.label3.TabIndex = 15;
      this.label3.Text = "Open Scene";
      this.label3.TextAlign = System.Drawing.ContentAlignment.TopRight;
      // 
      // label2
      // 
      this.label2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.label2.BackColor = System.Drawing.Color.Transparent;
      this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
      this.label2.Location = new System.Drawing.Point(329, 144);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(160, 32);
      this.label2.TabIndex = 9;
      this.label2.Text = "Open Project";
      this.label2.TextAlign = System.Drawing.ContentAlignment.TopRight;
      // 
      // label1
      // 
      this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.label1.BackColor = System.Drawing.Color.Transparent;
      this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
      this.label1.Location = new System.Drawing.Point(332, 52);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(157, 32);
      this.label1.TabIndex = 3;
      this.label1.Text = "New Project";
      this.label1.TextAlign = System.Drawing.ContentAlignment.TopRight;
      // 
      // button_SampleMaps
      // 
      this.button_SampleMaps.Image = ((System.Drawing.Image)(resources.GetObject("button_SampleMaps.Image")));
      this.button_SampleMaps.Location = new System.Drawing.Point(30, 222);
      this.button_SampleMaps.Margin = new System.Windows.Forms.Padding(14);
      this.button_SampleMaps.Name = "button_SampleMaps";
      this.button_SampleMaps.Size = new System.Drawing.Size(64, 64);
      this.button_SampleMaps.TabIndex = 12;
      this.button_SampleMaps.UseVisualStyleBackColor = true;
      this.button_SampleMaps.Click += new System.EventHandler(this.button_SampleMaps_Click);
      // 
      // button_StartupDoc
      // 
      this.button_StartupDoc.Image = ((System.Drawing.Image)(resources.GetObject("button_StartupDoc.Image")));
      this.button_StartupDoc.Location = new System.Drawing.Point(30, 38);
      this.button_StartupDoc.Margin = new System.Windows.Forms.Padding(14);
      this.button_StartupDoc.Name = "button_StartupDoc";
      this.button_StartupDoc.Size = new System.Drawing.Size(64, 64);
      this.button_StartupDoc.TabIndex = 0;
      this.button_StartupDoc.UseVisualStyleBackColor = true;
      this.button_StartupDoc.Click += new System.EventHandler(this.button_StartupDoc_Click);
      // 
      // button_NewProject
      // 
      this.button_NewProject.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.button_NewProject.Image = ((System.Drawing.Image)(resources.GetObject("button_NewProject.Image")));
      this.button_NewProject.Location = new System.Drawing.Point(506, 38);
      this.button_NewProject.Margin = new System.Windows.Forms.Padding(14);
      this.button_NewProject.Name = "button_NewProject";
      this.button_NewProject.Size = new System.Drawing.Size(64, 64);
      this.button_NewProject.TabIndex = 5;
      this.button_NewProject.UseVisualStyleBackColor = true;
      this.button_NewProject.Click += new System.EventHandler(this.button_NewProject_Click);
      // 
      // button_StartupProject
      // 
      this.button_StartupProject.Image = ((System.Drawing.Image)(resources.GetObject("button_StartupProject.Image")));
      this.button_StartupProject.Location = new System.Drawing.Point(30, 130);
      this.button_StartupProject.Margin = new System.Windows.Forms.Padding(14);
      this.button_StartupProject.Name = "button_StartupProject";
      this.button_StartupProject.Size = new System.Drawing.Size(64, 64);
      this.button_StartupProject.TabIndex = 6;
      this.button_StartupProject.UseVisualStyleBackColor = true;
      this.button_StartupProject.Click += new System.EventHandler(this.button_StartupProject_Click);
      // 
      // button_OpenProject
      // 
      this.button_OpenProject.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.button_OpenProject.Image = ((System.Drawing.Image)(resources.GetObject("button_OpenProject.Image")));
      this.button_OpenProject.Location = new System.Drawing.Point(506, 130);
      this.button_OpenProject.Margin = new System.Windows.Forms.Padding(14);
      this.button_OpenProject.Name = "button_OpenProject";
      this.button_OpenProject.Size = new System.Drawing.Size(64, 64);
      this.button_OpenProject.TabIndex = 11;
      this.button_OpenProject.UseVisualStyleBackColor = true;
      this.button_OpenProject.Click += new System.EventHandler(this.button_OpenProject_Click);
      // 
      // button_OpenScene
      // 
      this.button_OpenScene.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.button_OpenScene.Image = ((System.Drawing.Image)(resources.GetObject("button_OpenScene.Image")));
      this.button_OpenScene.Location = new System.Drawing.Point(506, 222);
      this.button_OpenScene.Margin = new System.Windows.Forms.Padding(14);
      this.button_OpenScene.Name = "button_OpenScene";
      this.button_OpenScene.Size = new System.Drawing.Size(64, 64);
      this.button_OpenScene.TabIndex = 17;
      this.button_OpenScene.UseVisualStyleBackColor = true;
      this.button_OpenScene.Click += new System.EventHandler(this.button_OpenScene_Click);
      // 
      // panel_Recent
      // 
      this.panel_Recent.BackColor = System.Drawing.Color.Transparent;
      this.panel_Recent.Controls.Add(this.label_Scenes);
      this.panel_Recent.Controls.Add(this.label_Projects);
      this.panel_Recent.Controls.Add(this.listView_Scenes);
      this.panel_Recent.Controls.Add(this.listView_Projects);
      this.panel_Recent.Location = new System.Drawing.Point(304, 60);
      this.panel_Recent.Name = "panel_Recent";
      this.panel_Recent.Size = new System.Drawing.Size(296, 319);
      this.panel_Recent.TabIndex = 3;
      // 
      // label_Scenes
      // 
      this.label_Scenes.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.label_Scenes.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
      this.label_Scenes.Location = new System.Drawing.Point(45, 22);
      this.label_Scenes.Name = "label_Scenes";
      this.label_Scenes.Size = new System.Drawing.Size(220, 23);
      this.label_Scenes.TabIndex = 1;
      this.label_Scenes.Text = "Recent Scenes";
      // 
      // label_Projects
      // 
      this.label_Projects.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
      this.label_Projects.Location = new System.Drawing.Point(30, 22);
      this.label_Projects.Name = "label_Projects";
      this.label_Projects.Size = new System.Drawing.Size(220, 23);
      this.label_Projects.TabIndex = 1;
      this.label_Projects.Text = "Recent Projects";
      // 
      // listView_Scenes
      // 
      this.listView_Scenes.Activation = System.Windows.Forms.ItemActivation.OneClick;
      this.listView_Scenes.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.listView_Scenes.BackColor = System.Drawing.SystemColors.Menu;
      this.listView_Scenes.Location = new System.Drawing.Point(45, 48);
      this.listView_Scenes.MultiSelect = false;
      this.listView_Scenes.Name = "listView_Scenes";
      this.listView_Scenes.ShowItemToolTips = true;
      this.listView_Scenes.Size = new System.Drawing.Size(220, 246);
      this.listView_Scenes.SmallImageList = this.imageList;
      this.listView_Scenes.TabIndex = 0;
      this.listView_Scenes.UseCompatibleStateImageBehavior = false;
      this.listView_Scenes.View = System.Windows.Forms.View.List;
      this.listView_Scenes.SelectedIndexChanged += new System.EventHandler(this.listView_Scenes_SelectedIndexChanged);
      this.listView_Scenes.DoubleClick += new System.EventHandler(this.listView_Scenes_DoubleClick);
      // 
      // listView_Projects
      // 
      this.listView_Projects.Activation = System.Windows.Forms.ItemActivation.OneClick;
      this.listView_Projects.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)));
      this.listView_Projects.BackColor = System.Drawing.SystemColors.Menu;
      this.listView_Projects.Location = new System.Drawing.Point(30, 48);
      this.listView_Projects.MultiSelect = false;
      this.listView_Projects.Name = "listView_Projects";
      this.listView_Projects.ShowItemToolTips = true;
      this.listView_Projects.Size = new System.Drawing.Size(220, 246);
      this.listView_Projects.SmallImageList = this.imageList;
      this.listView_Projects.TabIndex = 2;
      this.listView_Projects.UseCompatibleStateImageBehavior = false;
      this.listView_Projects.View = System.Windows.Forms.View.List;
      this.listView_Projects.SelectedIndexChanged += new System.EventHandler(this.listView_Projects_SelectedIndexChanged);
      this.listView_Projects.DoubleClick += new System.EventHandler(this.listView_Projects_DoubleClick);
      // 
      // button_Switch
      // 
      this.button_Switch.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.button_Switch.BackColor = System.Drawing.Color.Transparent;
      this.button_Switch.Location = new System.Drawing.Point(465, 6);
      this.button_Switch.Name = "button_Switch";
      this.button_Switch.Size = new System.Drawing.Size(104, 22);
      this.button_Switch.TabIndex = 3;
      this.button_Switch.Text = "Recent Files";
      this.button_Switch.UseVisualStyleBackColor = false;
      this.button_Switch.Click += new System.EventHandler(this.button_Switch_Click);
      // 
      // StartupDlg
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 17F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;

      //this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
      //this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
      this.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("$this.BackgroundImage")));
      this.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
      this.ClientSize = new System.Drawing.Size(600, 420);
      this.Controls.Add(this.panel_Start);
      this.Controls.Add(this.panel_Recent);
      this.Controls.Add(this.panel_Header);
      this.Controls.Add(this.panel_Footer);
      this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Sizable;
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "StartupDlg";
      this.ShowIcon = false;
      this.ShowInTaskbar = false;
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
      this.Load += new System.EventHandler(this.StartupDlg_Load);
      this.panel_Footer.ResumeLayout(false);
      this.panel_Footer.PerformLayout();
      this.panel_Header.ResumeLayout(false);
      this.panel_Header.PerformLayout();
      this.panel_Start.ResumeLayout(false);
      this.panel_Recent.ResumeLayout(false);
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.Button button_Close;
    private System.Windows.Forms.CheckBox checkBox_ShowDialog;
    private System.Windows.Forms.ImageList imageList;
    private System.Windows.Forms.Panel panel_Footer;
    private System.Windows.Forms.Panel panel_Header;
    private System.Windows.Forms.Panel panel_Start;
    private System.Windows.Forms.Label label14;
    private System.Windows.Forms.Label label13;
    private System.Windows.Forms.Label label12;
    private System.Windows.Forms.Label label11;
    private System.Windows.Forms.Label label10;
    private System.Windows.Forms.Label label9;
    private System.Windows.Forms.Label label4;
    private System.Windows.Forms.Label label5;
    private System.Windows.Forms.Label label6;
    private System.Windows.Forms.Label label3;
    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.Button button_SampleMaps;
    private System.Windows.Forms.Button button_StartupDoc;
    private System.Windows.Forms.Button button_NewProject;
    private System.Windows.Forms.Button button_StartupProject;
    private System.Windows.Forms.Button button_OpenProject;
    private System.Windows.Forms.Button button_OpenScene;
    private System.Windows.Forms.Label label_Description;
    private System.Windows.Forms.Label label_Title;
    private System.Windows.Forms.Panel panel_Recent;
    private System.Windows.Forms.Label label_Scenes;
    private System.Windows.Forms.Label label_Projects;
    private System.Windows.Forms.ListView listView_Scenes;
    private System.Windows.Forms.ListView listView_Projects;
    private System.Windows.Forms.Button button_Switch;
  }
}