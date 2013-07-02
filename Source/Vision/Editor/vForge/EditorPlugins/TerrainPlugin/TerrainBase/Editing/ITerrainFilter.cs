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
using System.ComponentModel;
using System.Windows.Forms;

namespace TerrainBase.Editing
{
  /// <summary>
  /// Abstract base class for the following sub-types: IHeightmapFilter, IDecorationFilter
  /// </summary>
  public abstract class ITerrainFilter
  {
    /// <summary>
    /// Get the name of this filter
    /// </summary>
    [Browsable(false)]
    public abstract string Name { get; }

    /// <summary>
    /// If OnStart fails, return a error string here
    /// </summary>
    [Browsable(false)]
    public virtual string LastError { get { return null; } }

    /// <summary>
    /// While this function returns false, the filter cannot be executed (Apply button grayed out)
    /// Re-evaluated when a property changed
    /// </summary>
    [Browsable(false)]
    public virtual bool Valid
    {
      get { return true; }
    }

    /// <summary>
    /// Return false to run the filter without the dialog, i.e. when there is nothing to setup
    /// </summary>
    [Browsable(false)]
    public virtual bool HasCustomProperties
    {
      get { return true; }
    }


    bool _bActive = false;


    /// <summary>
    /// Called when applied to a filter dialog
    /// </summary>
    [Browsable(false)]
    public virtual bool Active
    {
      get { return _bActive; }
      set
      {
        _bActive = value;
      }
    }

    /// <summary>
    /// Overridable to reset all custom parameters. Called by the constructor
    /// </summary>
    public virtual void ResetParameter()
    {
    }

    public override string ToString()
    {
      return Name;
    }

    /// <summary>
    /// Shows a message box with the latest error
    /// </summary>
    public void ShowLastError()
    {
      if (Valid)
        return;
      string lastErr = LastError;
      if (string.IsNullOrEmpty(lastErr)) // also don't show an error in this case
        return;

      MessageBox.Show(lastErr, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
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
