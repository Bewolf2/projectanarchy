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
using TerrainBase.Editing;

namespace TerrainBase.Terrain
{
  public enum SectorAction
  {
    Reloaded, Unloaded,
    Modified
  }

  public interface ITerrainCallback
  {

    /// <summary>
    /// Overridable callback that is called by native code when the status of a sector changed
    /// </summary>
    /// <param name="action"></param>
    /// <param name="iSectorX"></param>
    /// <param name="iSectorY"></param>
    void OnSectorChanged(SectorAction action, int iSectorX, int iSectorY);

    /// <summary>
    /// Triggered when the number of sectors changes that need decoration update
    /// </summary>
    /// <param name="iDirtySectors"></param>
    void OnDirtyDecorationRegionChanged(int iDirtySectors);

    /// <summary>
    /// Allows native code to access the model data of the terrain shape
    /// </summary>
    /// <param name="iID"></param>
    /// <returns></returns>
    DecorationModelResource GetDecorationModelByID(int iID);
    DecorationModelResource GetDecorationModelByName(string name);

    DetailTextureResource GetDetailTextureByID(int iID);
    DetailTextureResource GetDetailTextureByName(string name);

    /// <summary>
    /// Access the current selection
    /// </summary>
    TerrainSelection CurrentSelection { get;}

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
