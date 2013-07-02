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
using CSharpFramework.Actions;
using TerrainBase.Terrain;
using System.Runtime.Serialization;
using CSharpFramework.Serialization;

namespace TerrainBase.Actions
{
	/// <summary>
	/// Still abstract and implemented in managed code to modify native terrain
	/// </summary>
  [Serializable]
  public abstract class TerrainChangedBaseAction : IAction
	{

    public TerrainChangedBaseAction(ITerrainCallback terrain)
    {
      Terrain = terrain;
    }

    #region IAction members

    public override string ShortName {get {return _name;}}

    public override string LongName
    {
      get
      {
        return ShortName;
      }
    }

    protected string _name = "Changed Terrain";

    static int iIconIndex = EditorManager.GUI.ActionImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Actions\Brush4.bmp"),System.Drawing.Color.Magenta);

    /// <summary>
    /// Get the icon index for this action
    /// </summary>
    public override int IconIndex {get {return iIconIndex;}}


    #endregion

    #region ISerializable Members

    public byte[] CustomSerializationBlock = null;


    public void AllocateCustomSerializationBlock(int iSize)
    {
      if (iSize > 0)
        CustomSerializationBlock = new byte[iSize];
      else
        CustomSerializationBlock = null;
    }

    protected TerrainChangedBaseAction(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      _name = info.GetString("_name");
      Terrain = (ITerrainCallback)info.GetValue("Terrain", typeof(ITerrainCallback));
      if (SerializationHelper.HasElement(info, "CustomSerializationBlock"))
        CustomSerializationBlock = (byte[])info.GetValue("CustomSerializationBlock", typeof(byte[]));
    }

    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);
      info.AddValue("_name", _name);
      info.AddValue("Terrain", Terrain);
      if (CustomSerializationBlock != null)
        info.AddValue("CustomSerializationBlock", CustomSerializationBlock);
    }

   #endregion

    #region Sector Bounding Box

    public ITerrainCallback Terrain;

    public abstract int SectorX1 {get;}
    public abstract int SectorY1 {get;}
    public abstract int SectorX2 {get;}
    public abstract int SectorY2 {get;}

    #endregion

    protected void Update2DView()
    {
      int x1 = SectorX1;
      int y1 = SectorY1;
      int x2 = SectorX2;
      int y2 = SectorY2;
      for (int y = y1; y <= y2; y++)
        for (int x = x1; x <= x2; x++)
          Terrain.OnSectorChanged(SectorAction.Modified, x, y);

    }

    /// <summary>
    /// Overridable to get notifications about changed sectors. Calls Update2DView
    /// </summary>
    public virtual void UpdateSectorRange()
    {
      Update2DView();
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
