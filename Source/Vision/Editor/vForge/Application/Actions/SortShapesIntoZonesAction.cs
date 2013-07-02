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
using CSharpFramework;
using CSharpFramework.Shapes;
using CSharpFramework.Scene;
using CSharpFramework.Actions;
using CSharpFramework.Math;
using System.Runtime.Serialization;

namespace Editor.Actions
{
  [Serializable]
  public class SortShapesIntoZonesAction : GroupAction
  {
    public SortShapesIntoZonesAction(ShapeCollection shapes, ZoneCollection zones)
      : base("Sort shapes into zones")
    {
      _zones = zones;
      if (_zones == null)
        _zones = EditorManager.Scene.Zones;

      foreach (ShapeBase shape in shapes)
      {
        Layer layer = GetBestLayer(shape);
        bool bIsExternalLayer = !EditorManager.Scene.Layers.Contains(shape.ParentLayer);

        if (layer == shape.ParentLayer) // do not re-assign, just leave in the layer
          continue;
        if (layer == null)
        {
          layer = GetBestLayer(shape); // see again why this failed (debug)
          layer = GetUnAssignedLayer(shape);
        }

        if (bIsExternalLayer)
        {
          shape.ChildIndex = -1;
          this.Add(AddShapeAction.CreateAddShapeAction(shape, layer.Root, layer, false));
        }
        else
          this.Add(new SetShapeParentAction(shape, layer.Root));
      }
    }

    Dictionary<Layer,Layer> _UnAssignedLayers = null;

    Layer GetUnAssignedLayer(ShapeBase shape)
    {
      if (_UnAssignedLayers == null)
        _UnAssignedLayers = new Dictionary<Layer, Layer>();

      Layer layer = null;
      if (!_UnAssignedLayers.TryGetValue(shape.ParentLayer, out layer))
      {
        Object[] parameters = { "UnassignedShapes_" + shape.ParentLayer.LayerName };
        layer = EditorManager.LayerFactory.CreateInstance(parameters);
        this.Add(new AddLayerAction(layer,true));
        _UnAssignedLayers.Add(shape.ParentLayer, layer);
      }
      return layer;
    }


    ZoneCollection _zones;

    bool GetShapePosAndBox(ShapeBase shape, out Vector3F pos, out BoundingBox box)
    {
      if (shape.HasChildren())
      {
        int iValidCount = 0;
        BoundingBox baccum = new BoundingBox();
        Vector3F paccum = Vector3F.Zero;
        foreach (ShapeBase child in shape.ChildCollection)
        {
          BoundingBox b1;
          Vector3F p1;
          if (!GetShapePosAndBox(child, out p1, out b1))
            continue;
          iValidCount++;
          paccum += p1;
          if (b1 != null && b1.Valid)
            baccum.AddBox(b1);
        }
        pos = paccum;
        box = baccum;
        if (iValidCount == 0)
          return false;
        pos *= (1.0f / (float)iValidCount); // average position
        return true;
      }

      if (shape is Shape3D)
      {
        pos = ((Shape3D)shape).Position;
        box = ((Shape3D)shape).AbsoluteBoundingBox;
        if (box == null || !box.Valid)
          box = null;
        return true;
      }
      pos = Vector3F.Zero;
      box = null;
      return false;
    }

    Layer GetBestLayer(ShapeBase shape)
    {
      BoundingBox bbox;
      Vector3F pos;
      if (!GetShapePosAndBox(shape, out pos, out bbox))
        return null;
      if (bbox != null && bbox.Valid)
        pos = bbox.Center; // position from box center

      Zone bestZone = null;
      float fMinDist = 1000000000000.0f;
      foreach (Zone zone in _zones)
      {
        if (zone.Layers.Count < 1)
          continue;
        BoundingBox zonebox = zone.AssignmentBoundingBox;
        float fDist = zonebox.GetDistance(pos);
        if (fDist <= 0.0f)
        {
          if (bbox != null && bbox.Valid && zonebox.IsInside(bbox))
            fDist -= 10.0f; // extra score
        }
        if (bestZone == null || fDist < fMinDist)
        {
          bestZone = zone;
          fMinDist = fDist;
        }
      }
      if (bestZone == null)
        return null;
      return bestZone.Layers[0];
    }

    void InvalidateUI()
    {
      if (EditorManager.Scene.ActiveLayer != null)
        IScene.SendShapeChangedEvent(new ShapeChangedArgs(EditorManager.Scene.ActiveLayer.Root, ShapeChangedArgs.Action.ChildCollectionChanged));
    }

    public override void Do()
    {
      ShapeTreeView.UIUpdateLock++;
      try
      {
        base.Do();
      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex);
      }
      ShapeTreeView.UIUpdateLock--;
      InvalidateUI(); // easy method to rebuild list
    }

    public override void Undo()
    {
      ShapeTreeView.UIUpdateLock++;
      try
      {
        base.Undo();
      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex);
      }
      ShapeTreeView.UIUpdateLock--;
      InvalidateUI(); // easy method to rebuild list
    }

    #region ISerializable Members

    protected SortShapesIntoZonesAction(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
    }

    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);
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
