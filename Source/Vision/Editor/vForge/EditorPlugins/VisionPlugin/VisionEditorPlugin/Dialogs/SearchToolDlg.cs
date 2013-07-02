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
using CSharpFramework.Shapes;
using VisionEditorPlugin.Shapes;

namespace VisionEditorPlugin.Dialogs
{
	/// <summary>
	/// Search Panel.
	/// </summary>
	public class SearchToolDlg : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Button buttonSelect;
		private System.Windows.Forms.TextBox textEntityClass;
    private System.Windows.Forms.CheckBox checkEntityClass;
    private System.Windows.Forms.CheckBox checkModel;
    private System.Windows.Forms.TextBox textModel;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		/// <summary>
		/// Constructor
		/// </summary>
		public SearchToolDlg()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
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
      this.label1 = new System.Windows.Forms.Label();
      this.buttonSelect = new System.Windows.Forms.Button();
      this.textEntityClass = new System.Windows.Forms.TextBox();
      this.checkEntityClass = new System.Windows.Forms.CheckBox();
      this.checkModel = new System.Windows.Forms.CheckBox();
      this.textModel = new System.Windows.Forms.TextBox();
      this.SuspendLayout();
      // 
      // label1
      // 
      this.label1.Location = new System.Drawing.Point(8, 16);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(248, 16);
      this.label1.TabIndex = 0;
      this.label1.Text = "Search for all entities that match:";
      // 
      // buttonSelect
      // 
      this.buttonSelect.Location = new System.Drawing.Point(144, 120);
      this.buttonSelect.Name = "buttonSelect";
      this.buttonSelect.Size = new System.Drawing.Size(75, 23);
      this.buttonSelect.TabIndex = 5;
      this.buttonSelect.Text = "Select";
      this.buttonSelect.Click += new System.EventHandler(this.buttonSelect_Click);
      // 
      // textEntityClass
      // 
      this.textEntityClass.Location = new System.Drawing.Point(112, 48);
      this.textEntityClass.Name = "textEntityClass";
      this.textEntityClass.Size = new System.Drawing.Size(232, 20);
      this.textEntityClass.TabIndex = 2;
      this.textEntityClass.Text = "VisBaseEntity_cl";
      // 
      // checkEntityClass
      // 
      this.checkEntityClass.Checked = true;
      this.checkEntityClass.CheckState = System.Windows.Forms.CheckState.Checked;
      this.checkEntityClass.Location = new System.Drawing.Point(32, 48);
      this.checkEntityClass.Name = "checkEntityClass";
      this.checkEntityClass.Size = new System.Drawing.Size(64, 24);
      this.checkEntityClass.TabIndex = 1;
      this.checkEntityClass.Text = "Class";
      // 
      // checkModel
      // 
      this.checkModel.Location = new System.Drawing.Point(32, 80);
      this.checkModel.Name = "checkModel";
      this.checkModel.Size = new System.Drawing.Size(64, 24);
      this.checkModel.TabIndex = 3;
      this.checkModel.Text = "Model";
      // 
      // textModel
      // 
      this.textModel.Location = new System.Drawing.Point(112, 80);
      this.textModel.Name = "textModel";
      this.textModel.Size = new System.Drawing.Size(232, 20);
      this.textModel.TabIndex = 4;
      // 
      // SearchToolDlg
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(360, 149);
      this.Controls.Add(this.checkModel);
      this.Controls.Add(this.textModel);
      this.Controls.Add(this.checkEntityClass);
      this.Controls.Add(this.textEntityClass);
      this.Controls.Add(this.buttonSelect);
      this.Controls.Add(this.label1);
      this.Name = "SearchToolDlg";
      this.Text = "Search and Select";
      this.Closing += new System.ComponentModel.CancelEventHandler(this.SearchToolDlg_Closing);
      this.ResumeLayout(false);
      this.PerformLayout();

    }
		#endregion

		private void SelectEntities(ShapeBase shape, String classText, String modelText, ShapeCollection sel)
		{
			//check the entity
			EntityShape entity = shape as EntityShape;
      if (entity!=null)
      {
        bool bSelect = true;

        if (checkEntityClass.Checked)
          bSelect &= (entity.EntityClass.ToUpper().IndexOf(classText)!=-1);
        if (checkModel.Checked)
          bSelect &= (entity.ModelFile.ToUpper().IndexOf(modelText)!=-1);

        if (bSelect)
          sel.Add(shape);
      }

			//check it's children
			foreach(ShapeBase child in shape.ChildCollection)
			{
				SelectEntities(child, classText, modelText, sel);
			}

		}

		private void buttonSelect_Click(object sender, System.EventArgs e)
		{
			// gather the list of selected shapes
			ShapeCollection newSelection = new ShapeCollection();
			SelectEntities( EditorManager.Scene.ActiveLayer.Root, 
                      textEntityClass.Text.ToUpper(), 
                      textModel.Text.ToUpper(), 
                      newSelection);	
			
			//textResult.Text = "Selected " + newSelection.Count + " shapes";
			EditorManager.SelectedShapes = newSelection;
		}

    private void SearchToolDlg_Closing(object sender, System.ComponentModel.CancelEventArgs e)
    {
      e.Cancel = true; // do not close, but hide it
      Active = false;    
    }


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
				}
				else
				{
					Hide();
				}
			}
			get {return Visible;}
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
