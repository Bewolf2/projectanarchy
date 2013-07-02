/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing.Design;
using TerrainEditorPlugin.Shapes;
using System.Windows.Forms;
using TerrainBase.Editing;
using System.Windows.Forms.Design;
using CSharpFramework.View;
using CSharpFramework;

namespace TerrainEditorPlugin.Editing
{
  /// <summary>
  /// List control for detail textures
  /// </summary>
  public class DetailTextureComboBoxEditor : UITypeEditor
  {

    public DetailTextureComboBoxEditor() : base()
    {
      _comboBox.SelectedIndexChanged += new EventHandler(_comboBox_SelectedIndexChanged);
      _comboBox.View = View.SmallIcon;
    }

    void _comboBox_SelectedIndexChanged(object sender, EventArgs e)
    {
      if (_comboBox.SelectedItems.Count==1)
        frmsvr.CloseDropDown();
    }

    public override object EditValue(System.ComponentModel.ITypeDescriptorContext context, IServiceProvider provider, object value)
    {
      ITerrainObject obj = context.Instance as ITerrainObject;
      if (obj == null)
        return null;
      TerrainShape terrain = obj.OwnerTerrain;
      if (terrain == null)
        return null;

      frmsvr = (IWindowsFormsEditorService)provider.GetService(typeof(IWindowsFormsEditorService));
      if (frmsvr == null)
        return null;

      GUI.DynamicImageList il = new GUI.DynamicImageList(32);
      _comboBox.BeginUpdate();
      _comboBox.Items.Clear();
      _comboBox.SmallImageList = il.ImageList;
      int iSelIndex = -1;

      if (_bSupportsNoneTexture)
      {
        ListViewItem item = new ListViewItem("None");
        item.Tag = null;
        item.ImageIndex = -1;
        _comboBox.Items.Add(item);
      }

      foreach (DetailTextureResource tex in terrain.DetailTextures)
      {
        if (tex._bIsBaseTexture && !_bSupportsBaseTexture)
          continue;

        ListViewItem item = new ListViewItem(tex.ToString());
        item.Tag = tex;
        item.ImageIndex = il.AddBitmap(EditorManager.Project.MakeAbsolute(tex.DiffuseFilename), System.Drawing.Color.Magenta);
        _comboBox.Items.Add(item);

        if (tex == value)
        {
          item.Selected = true;
          iSelIndex = item.Index;
        }
      }
      _comboBox.EndUpdate();

      if (iSelIndex>=0)
        _comboBox.EnsureVisible(iSelIndex);

      frmsvr.DropDownControl(_comboBox);
      if (_comboBox.SelectedItems.Count == 1)
        return _comboBox.SelectedItems[0].Tag as DetailTextureResource;
      return null;
    }

    IWindowsFormsEditorService frmsvr;
    ListView _comboBox = new ListView();
    protected bool _bSupportsNoneTexture = false; // Set in derived class
    protected bool _bSupportsBaseTexture = true; // Set in derived class

    public override UITypeEditorEditStyle GetEditStyle(System.ComponentModel.ITypeDescriptorContext context)
    {
      return UITypeEditorEditStyle.DropDown;
    }
  }

  /// <summary>
  /// Same as DetailTextureComboBoxEditor but additionally supports selection of a 'None' texture
  /// </summary>
  public class DetailTextureComboBoxEditorNone : DetailTextureComboBoxEditor
  {
    public DetailTextureComboBoxEditorNone()
      : base()
    {
      _bSupportsNoneTexture = true;
      _bSupportsBaseTexture = true;
    }
  }

  /// <summary>
  /// Same as DetailTextureComboBoxEditor but supports selection of 'None' instead of the 'Base' detail texture layer.
  /// </summary>
  public class DetailTextureComboBoxEditorNoneNoBase : DetailTextureComboBoxEditor
  {
    public DetailTextureComboBoxEditorNoneNoBase()
      : base()
    {
      _bSupportsNoneTexture = true;
      _bSupportsBaseTexture = false;
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
