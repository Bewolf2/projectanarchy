/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.IO;
using CSharpFramework;
using VisionEditorPlugin.Tools;
using VisionEditorPlugin.Dialogs;
using System.Diagnostics;
using System.Runtime.Serialization;

namespace VisionEditorPlugin.Actions
{
	/// <summary>
	/// This action changes the color matrix (brightness, contrast, ...) for lightmap update
	/// </summary>
  [Serializable]
	public class LightmapColorMatrixChangedAction : IAction
	{
    /// <summary>
    /// Constructor
    /// </summary>
		public LightmapColorMatrixChangedAction(LightmapToolDlg dlg, string actionName, ColorMatrixParams oldBase, ColorMatrixParams newBase, ColorMatrixParams oldRadiosity, ColorMatrixParams newRadiosity)
		{
      _dlg = dlg;
      _actionName = actionName;
      _oldBase = oldBase;
      _newBase = newBase;
      _oldRadiosity = oldRadiosity;
      _newRadiosity = newRadiosity;
      Debug.Assert(_oldBase!=null);
      Debug.Assert(_newBase!=null);
      Debug.Assert(_oldRadiosity!=null);
      Debug.Assert(_newRadiosity!=null);

      _target = _dlg.Target;
		}

    #region IAction Members

    string _actionName;
    LightmapToolDlg _dlg;
    ColorMatrixParams _oldBase;
    ColorMatrixParams _newBase;
    ColorMatrixParams _oldRadiosity;
    ColorMatrixParams _newRadiosity;
    bool _bFirst = true;
    LightmapToolDlg.TweakTarget _target = LightmapToolDlg.TweakTarget.None;

    /// <summary>
    /// Sets the old color matrix
    /// </summary>
    public override void Undo()
    {
      _dlg.UpdateLockCounter++;
      _dlg.Target = _target;
      _dlg.BaseParams = _oldBase;
      _dlg.RadiosityParams = _oldRadiosity;
      _dlg.UpdateLockCounter--;
    }

    /// <summary>
    /// Sets the new color matrix
    /// </summary>
    public override void Do()
    {
      if (!_bFirst)
      {
        _dlg.UpdateLockCounter++;
        _dlg.Target = _target;
        _dlg.BaseParams = _newBase;
        _dlg.RadiosityParams = _newRadiosity;
        _dlg.UpdateLockCounter--;
      }
      _bFirst = false;
    }

    /// <summary>
    /// Short name of the action, to show in the menu
    /// </summary>
    public override string ShortName {get {return _actionName;}}

    /// <summary>
    /// Long name of the action, to show in the history
    /// </summary>
    public override string LongName 
    {
      get {return ShortName;}
    }

    #endregion

    #region ISerializable Members

    protected LightmapColorMatrixChangedAction(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      _actionName = info.GetString("_actionName");
      _dlg = (LightmapToolDlg)info.GetValue("_dlg", typeof(LightmapToolDlg));
      _oldBase = (ColorMatrixParams)info.GetValue("_oldBase", typeof(ColorMatrixParams));
      _newBase = (ColorMatrixParams)info.GetValue("_newBase", typeof(ColorMatrixParams));
      _oldRadiosity = (ColorMatrixParams)info.GetValue("_oldRadiosity", typeof(ColorMatrixParams));
      _newRadiosity = (ColorMatrixParams)info.GetValue("_newRadiosity", typeof(ColorMatrixParams));
      _bFirst = info.GetBoolean("_bFirst");
      _target = (LightmapToolDlg.TweakTarget)info.GetValue("_target", typeof(LightmapToolDlg.TweakTarget));
    }

    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);
      info.AddValue("_actionName", _actionName);
      info.AddValue("_dlg", _dlg);
      info.AddValue("_oldBase", _oldBase);
      info.AddValue("_newBase", _newBase);
      info.AddValue("_oldRadiosity", _oldRadiosity);
      info.AddValue("_newRadiosity", _newRadiosity);
      info.AddValue("_bFirst", _bFirst);
      info.AddValue("_target", _target);
    }

    #endregion

    #region Icon Index

    static int iIconIndex = EditorManager.GUI.ActionImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Actions\Palette.bmp"),System.Drawing.Color.Magenta);

    /// <summary>
    /// Return the icon index for this action
    /// </summary>
    public override int IconIndex {get {return iIconIndex;}}

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
