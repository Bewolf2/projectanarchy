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

namespace DragAndDropPanelPlugin.Classes
{
  /// <summary>
  /// Helper class which encapsulates an entry for the panel list view
  /// </summary>
  public class DragAndDropPanelEntry : IComparable<DragAndDropPanelEntry>
  {
    public enum EntryType
    {
      Model = 1,
      StaticMesh = 2
    }

    #region Private Members

    EntryType _Type;

    string _Path;

    string _DisplayText;

    string _ThumbnailKey;

    #endregion

    public DragAndDropPanelEntry(EntryType Type, string Path, string DisplayText, string ThumbnailKey)
    {
      _Type = Type;
      _Path = Path;
      _DisplayText = DisplayText;
      _ThumbnailKey = ThumbnailKey;
    }

    #region Public Get Accessors

    public EntryType Type
    {
      get
      {
        return _Type;
      }
    }

    public string Path
    {
      get
      {
        return _Path;
      }
    }

    public string DisplayText
    {
      get
      {
        return _DisplayText;
      }
    }

    public string Thumbnailkey
    {
      get
      {
        return _ThumbnailKey;
      }
    }

    #endregion


    #region IComparable<EntityPanelEntry> Members

    public int CompareTo(DragAndDropPanelEntry other)
    {
      return _DisplayText.CompareTo(other.DisplayText);
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
