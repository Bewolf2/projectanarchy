/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using CSharpFramework;
using CSharpFramework.Contexts;
using CSharpFramework.Docking;

namespace ScenePropertiesPlugin
{
  /// <summary>
  /// Sample dialog which displays various scene specific properties in an own
  /// panel.
  /// </summary>
  public partial class ScenePropertiesPanel : DockableForm
  {
    private SceneSettingInfo sceneSettings = new SceneSettingInfo();

    public ScenePropertiesPanel(DockingContainer container)
      : base(container)
    {
      InitializeComponent();
      
      // Initialize the control fields
      InitRestrictions();
      UpdateControlFromData();

      // Listen to changes of the editing controls
      difficultyTrackbar.ValueChanged += new EventHandler(UpdateDataFromControl);
      respawnTimeUpDown.ValueChanged  += new EventHandler(UpdateDataFromControl);
      respawnRateUpDown.ValueChanged  += new EventHandler(UpdateDataFromControl);
      descriptionBox.TextChanged      += new EventHandler(UpdateDataFromControl);
    }

    /// <summary>
    /// Initializes the min/max restrictions of the various editing control
    /// </summary>
    void InitRestrictions()
    {
      difficultyTrackbar.SetRange(SceneSettingInfo.MIN_DIFFICULTY, 
                                  SceneSettingInfo.MAX_DIFFICULTY);
      
      respawnTimeUpDown.Minimum = SceneSettingInfo.MIN_RESPAWN_TIME;
      respawnTimeUpDown.Maximum = SceneSettingInfo.MAX_RESPAWN_TIME;
      respawnTimeUpDown.Value   = SceneSettingInfo.MIN_RESPAWN_TIME;

      respawnRateUpDown.Minimum = SceneSettingInfo.MIN_RESPAWN_RATE;
      respawnRateUpDown.Maximum = SceneSettingInfo.MAX_RESPAWN_RATE;
      respawnRateUpDown.Value   = SceneSettingInfo.MIN_RESPAWN_RATE;
      
      descriptionBox.Text       = SceneSettingInfo.DEFAULT_DESCRIPTION;
    }

    /// <summary>
    /// Gets or sets the scene settings displayed in this panel
    /// </summary>
    public SceneSettingInfo SceneSettings
    {
      get { return this.sceneSettings;  }
      set 
      {
        if (value != null)
          this.sceneSettings = value;
        else
          this.sceneSettings = new SceneSettingInfo();

        // Ensure that the control visualized the new data
        UpdateControlFromData();
      }
    }

    /// <summary>
    /// Helper variable that is used by this.Enabled and this.UpdateDataFromControl(...).
    /// Avoids that the scene gets dirty when enabling the panel
    /// </summary>
    private bool _doNotMakeSceneDirty = false;

    /// <summary>
    /// Enables or disabled the panel
    /// </summary>
    public new bool Enabled
    {
      get { return base.Enabled; }
      set 
      { 
        base.Enabled = value;
        _doNotMakeSceneDirty = true;
        UpdateControlFromData();
        _doNotMakeSceneDirty = false;
      }
    }

    /// <summary>
    /// Copies the current content of the editing controls into the
    /// scene settings data structure
    /// </summary>
    private void UpdateDataFromControl(object sender, EventArgs e)
    {
      // Return immediately if there is no scene settings instance
      if (this.sceneSettings == null) 
        return;

      // We do not transfer any values in case the panel is grayed out.
      // The user is not supposed to edit any data in that case.
      // (this situation may occur on loading time though, thus we take care of it)
      if (!this.Enabled) 
        return;

      // Transfer the values from the control to the scene settings instance
      this.sceneSettings.Description = descriptionBox.Text;
      this.sceneSettings.RespawnRate = Convert.ToInt32(respawnRateUpDown.Value);
      this.sceneSettings.RespawnTime = Convert.ToInt32(respawnTimeUpDown.Value);
      this.sceneSettings.Difficulty  = difficultyTrackbar.Value;

      // Notify vForge that the scene has been modified. But only when it is allowed to make the scene dirty
      if (!_doNotMakeSceneDirty)
      {
        //we must set the mainlayer to dirty, since all information will be stored in this layer. To simplify this we just set all layers to dirty
        //EditorManager.Scene.Dirty = true; //this line does not work, since it does not set the mainlayer dirty, where the informations should be saved to
        foreach (Layer layer in EditorManager.Scene.Layers)
            layer.Dirty = true;
      }
        
        
    }

    /// <summary>
    /// Copies the current content of the scene settings data structure into
    /// the editing controls.
    /// </summary>
    private void UpdateControlFromData()
    {
      // Check whether no settings are available, and thus some default values
      // have to be shown (grayed out anyway in this case)
      if (sceneSettings == null)
      {
        difficultyTrackbar.Value = SceneSettingInfo.MIN_DIFFICULTY;
        respawnTimeUpDown.Value = SceneSettingInfo.MIN_RESPAWN_TIME;
        respawnRateUpDown.Value = SceneSettingInfo.MIN_RESPAWN_RATE;
        descriptionBox.Text = SceneSettingInfo.DEFAULT_DESCRIPTION;
        return;
      }

      difficultyTrackbar.Value  = sceneSettings.Difficulty;
      respawnTimeUpDown.Value   = sceneSettings.RespawnTime;
      respawnRateUpDown.Value   = sceneSettings.RespawnRate;
      descriptionBox.Text       = sceneSettings.Description;

      this.Refresh();
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
