/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Windows.Forms;
using CSharpFramework;

namespace AnimationLoopPlugin
{
  /// <summary>
  /// Represents a button in the toolbar to play or animate the engine view in a loop
  /// </summary>
  public class LoopButton : ToolStripSplitButton
  {
    //time interval in ms for loop
    private int _timerIntervalValue = 2000; //set default value here
    //the timer that controls the loop
    private Timer _loopTimer = new Timer();
    //image that is used for the play button
    private Bitmap _buttonIconPlay = ButtonIcons.PlayButtonImage;
    //image that is used for the stop button
    private Bitmap _buttonIconStop = ButtonIcons.StopButtonImage;
    //stores the dropdown menu of the button
    private ToolStripDropDown _play_Dropdown = null;
    //stores the last editormode that is played. Default is EM_PLAYING_IN_EDITOR
    private EditorManager.Mode _lastActiveEditorMode = EditorManager.Mode.EM_PLAYING_IN_EDITOR;
    //toolstripmenu items for the splitbutton
    private ToolStripMenuItem _toolStripMenuItem_PlayInEditor;
    private ToolStripMenuItem _toolStripMenuItem_Animate;
    private ToolStripMenuItem _toolStripMenuItem_SetInterval;
    //dialog that shows up if the interval is changed
    private IntervalChangeDialog _intervalChangeDialog;

    /// <summary>
    /// Constructor
    /// </summary>
    public LoopButton()
      : base()
    {
      //create _intervalChangeDialog but do not show it yet
      _intervalChangeDialog = new IntervalChangeDialog(this);


      //create SplitButton MenuItem for animate
      this._toolStripMenuItem_Animate = new System.Windows.Forms.ToolStripMenuItem();
      this._toolStripMenuItem_Animate.Name = "ToolStripMenuItem_Animate";
      this._toolStripMenuItem_Animate.Size = new System.Drawing.Size(150, 22);
      this._toolStripMenuItem_Animate.Text = "Loop animation";
      this._toolStripMenuItem_Animate.ToolTipText = "loops animation";
      this._toolStripMenuItem_Animate.Click += new System.EventHandler(this.ToolStripMenuItem_LoopAnimate_Click);

      //create SplitButton MenuItem for play in editor
      this._toolStripMenuItem_PlayInEditor = new System.Windows.Forms.ToolStripMenuItem();
      this._toolStripMenuItem_PlayInEditor.Name = "ToolStripMenuItem_PlayInEditor";
      this._toolStripMenuItem_PlayInEditor.Size = new System.Drawing.Size(150, 22);
      this._toolStripMenuItem_PlayInEditor.Text = "Loop play in editor";
      this._toolStripMenuItem_PlayInEditor.ToolTipText = "loops play in editor mode";
      this._toolStripMenuItem_PlayInEditor.Click += new System.EventHandler(this.ToolStripMenuItem_LoopPlayInEditor_Click);

      //create SplitButton MenuItem for set interval
      this._toolStripMenuItem_SetInterval = new System.Windows.Forms.ToolStripMenuItem();
      this._toolStripMenuItem_SetInterval.Name = "ToolStripMenuItem_SetInterval";
      this._toolStripMenuItem_SetInterval.Size = new System.Drawing.Size(150, 22);
      this._toolStripMenuItem_SetInterval.Text = "Set loop interval...";
      this._toolStripMenuItem_SetInterval.ToolTipText = "sets the loop interval";
      this._toolStripMenuItem_SetInterval.Click += new System.EventHandler(this.ToolStripMenuItem_SetInterval_Click);

      //button settings
      this.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
        this._toolStripMenuItem_Animate,
        this._toolStripMenuItem_PlayInEditor,
        this._toolStripMenuItem_SetInterval});
      this.Image = _buttonIconPlay;
      this.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolTipText = "Plays or animates loop in engine view";
      this.Visible = true;
      this.Enabled = false;
      this.ButtonClick += new System.EventHandler(this.Button_Loop_Click);

      //store dropdown value of button
      _play_Dropdown = this.DropDown;

      //register eventhandler for events that influence the loopbutton
      EditorManager.SceneChanged += new SceneChangedEventHandler(EditorManager_SceneChanged);
      EditorManager.EditorModeChanged += new EditorModeChangedEventHandler(EditorManager_EditorModeChanged);

      //add this button to the EnginePanelToolbar
      EditorManager.GUI.EnginePanelToolBar.Items.Add(this);
    }

    /// <summary>
    /// This function deregisters all eventhandlers registered from within this class
    /// </summary>
    public void DeregisterEventHandlersAndRemove()
    {
      this._toolStripMenuItem_SetInterval.Click -= new System.EventHandler(this.ToolStripMenuItem_SetInterval_Click);
      this.ButtonClick -= new System.EventHandler(this.Button_Loop_Click);
      EditorManager.SceneChanged -= new SceneChangedEventHandler(EditorManager_SceneChanged);
      EditorManager.EditorModeChanged -= new EditorModeChangedEventHandler(EditorManager_EditorModeChanged);
      //finally remove this button from the enginepaneltoolbar again
      EditorManager.GUI.EnginePanelToolBar.Items.Remove(this);
    }

    /// <summary>
    /// this function starts the EditorManager.Mode and initializes a timer to realize the loop behavior
    /// </summary>
    /// <param name="modeToPlay">editor mode that should be played</param>
    private void StartTimerAndPlay(EditorManager.Mode modeToPlay)
    {
      if (EditorManager.EditorMode == EditorManager.Mode.EM_NONE)
      {
        _loopTimer.Interval = _timerIntervalValue;
        _loopTimer.Tick += new EventHandler(TimerElapsed);
        _loopTimer.Start();

        if (_lastActiveEditorMode != modeToPlay) 
        {
          _lastActiveEditorMode = modeToPlay;
        }
        EditorManager.EditorMode = modeToPlay;
      }
      else
      {
        EditorManager.EditorMode = EditorManager.Mode.EM_NONE; //stop playing
      }
    }

    #region Properties

    /// <summary>
    /// This property sets the time interval for the loop
    /// </summary>
    public int TimerIntervalValue
    {
      get { return _timerIntervalValue; }
      set
      {
        if (value > 0) //the interval must be larger than 0
        {
          _timerIntervalValue = value;
        }
      }
    }

    #endregion

    #region Button and SplitButton Eventhandler

    private void ToolStripMenuItem_SetInterval_Click(object sender, EventArgs e)
    {
      _intervalChangeDialog.ShowDialog();
    }

    private void Button_Loop_Click(object sender, EventArgs e)
    {
      StartTimerAndPlay(_lastActiveEditorMode);
    }

    private void ToolStripMenuItem_LoopPlayInEditor_Click(object sender, EventArgs e)
    {
      StartTimerAndPlay(EditorManager.Mode.EM_PLAYING_IN_EDITOR);
    }

    private void ToolStripMenuItem_LoopAnimate_Click(object sender, EventArgs e)
    {
      StartTimerAndPlay(EditorManager.Mode.EM_ANIMATING);
    }

    #endregion

    #region Timer Eventhandler

    // this function gets called if the timer elapses
    private void TimerElapsed(object sender, EventArgs e)
    {
      EditorManager.EditorMode = EditorManager.Mode.EM_NONE; //stop playing and reset. This statement will also indirectly stop the timer in EditorManager_EditorModeChanged
      StartTimerAndPlay(_lastActiveEditorMode);
    }

    #endregion

    #region Editor Eventhandler

    //this functions gets called if the mode in the editor is changed
    private void EditorManager_EditorModeChanged(object sender, EditorModeChangedArgs e)
    {
      if (e._newMode == EditorManager.Mode.EM_NONE) 
      {
        this.Image = _buttonIconPlay;
        this.DropDown = _play_Dropdown; //reactivate dropdown menu
        if (_loopTimer.Enabled) //if timer is running, stop it
        {
          _loopTimer.Stop();
          _loopTimer.Tick -= new EventHandler(TimerElapsed);
        }
      }
      else
      {
        this.Image = _buttonIconStop;
        this.DropDown = null; //deactivate dropdown menu
      }
    }

    //this function gets called when the scene is changed
    void EditorManager_SceneChanged(object sender, SceneChangedArgs e)
    {
      if (EditorManager.Scene == null)
      {
        this.Enabled = false;
        if (_loopTimer.Enabled) //if timer is running, stop it
        {
          _loopTimer.Stop();
          _loopTimer.Tick -= new EventHandler(TimerElapsed);
        }
      }
      else
      {
        this.Enabled = true;
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
