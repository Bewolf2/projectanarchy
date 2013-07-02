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
using CSharpFramework.Math;
using CSharpFramework;
using CSharpFramework.Shapes;
using System.ComponentModel;
using CSharpFramework.PropertyEditors;

namespace TerrainBase.Editing
{
  public class TerrainSelection
  {
    [Flags, Editor(typeof(FlagsEditor), typeof(System.Drawing.Design.UITypeEditor))]
    public enum SelectionFilter_e
    {
      [Description("Height values are copied/pasted")]
      Heightmap = 0x00000001,
      [Description("All texture channels are copied/pasted")]
      Textures = 0x00000002,
      [Description("All decoration channels are copied/pasted")]
      Decoration =  0x00000004,
      [Description("All shapes that are on the terrain are copied/pasted (pivot position inside selection)")]
      Shapes = 0x00000008,
      [NonEditableFlag]
      All = Heightmap | Textures | Decoration | Shapes
    }

    public SelectionFilter_e SelectionFilter = SelectionFilter_e.Heightmap | SelectionFilter_e.Textures | SelectionFilter_e.Decoration;
    public Rectangle2D WorldSpaceExtent = new Rectangle2D();
    public float DisplayZ = 0.0f;

    public void SetExtent(float x1, float y1, float x2, float y2)
    {
      WorldSpaceExtent.X1 = x1;
      WorldSpaceExtent.Y1 = y1;
      WorldSpaceExtent.X2 = x2;
      WorldSpaceExtent.Y2 = y2;
    }


    public bool Valid
    {
      get
      {
        return WorldSpaceExtent.Valid;
      }
      set
      {
        if (!value)
        {
          WorldSpaceExtent.X1 = WorldSpaceExtent.Y1 = 1.0f;
          WorldSpaceExtent.X2 = WorldSpaceExtent.Y2 = -1.0f;
        }
      }
    }

    public void RenderSelection(VisionViewBase view, ShapeRenderMode mode)
    {
      if (!Valid)
        return;
      uint iColor = VisionColors.RGBA(50, 20, 150, 50);
      /*
      Vector3F v0 = new Vector3F(WorldSpaceExtent.X1, WorldSpaceExtent.Y1, DisplayZ);
      Vector3F v1 = new Vector3F(WorldSpaceExtent.X2, WorldSpaceExtent.Y1, DisplayZ);
      Vector3F v2 = new Vector3F(WorldSpaceExtent.X1, WorldSpaceExtent.Y2, DisplayZ);
      Vector3F v3 = new Vector3F(WorldSpaceExtent.X2, WorldSpaceExtent.Y2, DisplayZ);

      view.RenderSolidTriangle(v0, v1, v2, iColor);
      view.RenderSolidTriangle(v1, v3, v2, iColor);
      */
      float fMin = DisplayZ - EditorManager.Settings.MaxPickingDistance;
      BoundingBox renderBox = new BoundingBox(WorldSpaceExtent.X1, WorldSpaceExtent.Y1, fMin, WorldSpaceExtent.X2, WorldSpaceExtent.Y2, DisplayZ);
      view.RenderSolidBox(renderBox, iColor);

    }
  }


  [Serializable]
  public class ITerrainClipboardObject : CSharpFramework.Clipboard.ClipboardData
  {
    public ITerrainClipboardObject(object obj, string name)
      : base(obj, name)
    {
    }

    public override bool CanSerialize
    {
      get
      {
        return false; // we'd need to serialize all the content, e.g. height data
      }
    }

    /// <summary>
    ///  holds the size of the selection at copy time
    /// </summary>
    public Rectangle2D OriginalExtent = null;

    /// <summary>
    /// If not null, these shapes should be pasted along with the data
    /// </summary>
    public ShapeCollection ShapesToPaste = null;
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
