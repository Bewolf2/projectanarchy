/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using CSharpFramework;
using CSharpFramework.Scene;
using System.IO;
using System.Globalization;
using System.Drawing;
using System.Windows.Forms;
using CSharpFramework.Shapes;
using CSharpFramework.Math;

namespace Editor.Vision
{
	/// <summary>
  /// Implements StaticLightInfo and overrides Save function
	/// </summary>
	public class VisionLightInfo : StaticLightInfo
	{
    /// <summary>
    /// Constructor
    /// </summary>
    public VisionLightInfo()
    {
    }

    #region static conversion helper

    /// <summary>
    /// Converts to 16bit color (V3D compatible)
    /// </summary>
    /// <param name="col"></param>
    /// <returns></returns>
    static protected ushort GetColor16(Color col)
    {
      int r = col.R*31/255;
      int g = col.G*31/255;
      int b = col.B*31/255;

      return (ushort)((r<<10)|(g<<5)|(b));
    }

    static string GetSafeStr(string str)
    {
      if (str==null)
        return "";
      return str;
    }

    #endregion

    private string ShapeNameList(ShapeCollection shapes)
    {
      string txt = "";
      foreach (ShapeBase shape in shapes)
        txt += shape.ShapePath + "\r\n";
      return txt;
    }


    /// <summary>
    /// Writes the content to the passes text writer
    /// </summary>
    /// <returns></returns>
    public string ToText()
    {
      return "Supplier shapes:\r\n" + ShapeNameList(SupplierShapes)
            + "\r\nRelevant lights:\r\n" + ShapeNameList(RelevantLights)
            + "\r\nRelevant occluder:\r\n" + ShapeNameList(RelevantOccluder)
            + "\r\nRelevant receiver:\r\n" + ShapeNameList(RelevantReceiver);


/*
      PrefabDesc prefab = new PrefabDesc(null);
      string txt = "Supplier shapes:\n";

      if (!prefab.CreateFromInstances(this.SupplierShapes, Vector3F.Zero, false, false))
        return null;
      txt += prefab.ToText();

      txt += "\n\nRelevant lights:\n";
      prefab = new PrefabDesc(null);
      if (!prefab.CreateFromInstances(this.RelevantLights, Vector3F.Zero, false, false))
        return null;
      txt += prefab.ToText();

      txt += "\n\nRelevant occluder:\n";
      prefab = new PrefabDesc(null);
      if (!prefab.CreateFromInstances(this.RelevantOccluder, Vector3F.Zero, false, false))
        return null;
      txt += prefab.ToText();

      txt += "\n\nRelevant receiver:\n";
      prefab = new PrefabDesc(null);
      if (!prefab.CreateFromInstances(this.RelevantReceiver, Vector3F.Zero, false, false))
        return null;
      txt += prefab.ToText();
      return txt;
      */
    }

    /// <summary>
    /// Overridden save function
    /// </summary>
    /// <param name="filename"></param>
    /// <returns></returns>
    public override bool Save(string filename)
    {
      PrefabDesc prefab = new PrefabDesc(filename);
      ShapeCollection all = new ShapeCollection();

      // the following shapes go into the prefab: lightgrid boxes, lights
      foreach (ShapeBase shape in this.FilteredSupplier)
        all.Add(shape);

      foreach (ShapeBase shape in this.FilteredLights)
        if (!all.Contains(shape))
          all.Add(shape);

      if (!prefab.CreateFromInstances(all, Vector3F.Zero, false, false))
        return false;
      return prefab.SaveToFile(null);
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
