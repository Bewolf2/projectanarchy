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
using TerrainManaged;
using TerrainEditorPlugin.Shapes;
using TerrainBase.Editing;
using System.Runtime.Serialization;

namespace TerrainEditorPlugin.Actions
{
  [Serializable]
  public class PaintDecorationAction : TerrainChangedAction
  {
    public PaintDecorationAction(TerrainShape terrain, DecorationModelResource model)
      : base(terrain.EngineTerrain, "Edit decoration")
    {
      _model = model;
    }

    DecorationModelResource _model;

    public override void UpdateSectorRange()
    {
      base.UpdateSectorRange();

      // extend the dirty region on the texture resource
      for (int y = SectorY1; y <= SectorY2; y++)
        for (int x = SectorX1; x <= SectorX2; x++)
          _model.AddAffectedSector(x, y);

    }

    #region ISerializable Members

    protected PaintDecorationAction(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      _model = (DecorationModelResource)info.GetValue("_model", typeof(DecorationModelResource));
    }

    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);
      info.AddValue("_model", _model);
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
