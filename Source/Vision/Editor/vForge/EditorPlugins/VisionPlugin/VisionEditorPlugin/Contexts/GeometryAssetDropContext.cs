/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System.Diagnostics;
using CSharpFramework.Contexts;
using VisionEditorPlugin.Shapes;
using CSharpFramework;
using CSharpFramework.Math;
using CSharpFramework.Shapes;
using System.Drawing;
using CSharpFramework.Actions;
using VisionManaged;
using ManagedFramework;
using CSharpFramework.AssetManagement;

namespace VisionEditorPlugin.Contexts
{
  #region IDropContext

  /// <summary>
  /// Base class of a drag and drop context
  /// </summary>
  public class GeometryAsssetDropContext : IDropContext
  {
    #region Dragging functions

    private void createInstance(System.Windows.Forms.DragEventArgs e)
    {
      _assetPaths = CSharpFramework.Contexts.IDropContext.GetAssetPaths(e);
      _assetTypes = CSharpFramework.Contexts.IDropContext.GetAssetTypes(e);
      _assetNames = CSharpFramework.Contexts.IDropContext.GetAssetNames(e);

      Debug.Assert(_assetPaths.Length == _assetTypes.Length && _assetPaths.Length == _assetNames.Length);
      if (_assetPaths.Length == 0)
        return;

      if (_assetTypes[0] == "Model")
      {
        _instance = new EngineInstanceEntity("VisBaseEntity_cl", _assetPaths[0], null, null, true);
      }
      else if (_assetTypes[0] == "StaticMesh")
      {
        EngineInstanceStaticMesh instance = new EngineInstanceStaticMesh();
        instance.SetMeshFile(_assetPaths[0], "", 0, false);
        instance.SetCollisionBitmask(0);
        _instance = instance;
      }
      else if (_assetTypes[0] == "Prefab")
      {
        EngineInstancePrefab instance = new EngineInstancePrefab();
        instance.SetFilename(_assetPaths[0]);
        _instance = instance;
      }
    }

    private void createShape(System.Windows.Forms.DragEventArgs e)
    {
      _assetPaths = CSharpFramework.Contexts.IDropContext.GetAssetPaths(e);
      _assetTypes = CSharpFramework.Contexts.IDropContext.GetAssetTypes(e);
      _assetNames = CSharpFramework.Contexts.IDropContext.GetAssetNames(e);

      Debug.Assert(_assetPaths.Length == _assetTypes.Length && _assetPaths.Length == _assetNames.Length);
      if (_assetPaths.Length == 0)
        return;

      if (_assetTypes[0] == "Model")
      {
        EntityShape entity = new EntityShape(_assetNames[0]);
        entity.EntityClass = "VisBaseEntity_cl";
        entity.ModelFile = _assetPaths[0];
        _dummyShape = entity;
      }
      else if (_assetTypes[0] == "StaticMesh")
      {
        StaticMeshShape staticMesh = new StaticMeshShape(_assetNames[0]);
        staticMesh.MeshFileName = _assetPaths[0];
        _dummyShape = staticMesh;
      }
      else if (_assetTypes[0] == "Prefab")
      {
        PrefabShape prefabShape = new PrefabShape(_assetNames[0]);
        prefabShape.Filename = _assetPaths[0];
        _dummyShape = prefabShape;
      }
    }

    public override void DragEnter(object sender, System.Windows.Forms.DragEventArgs e)
    {
      _assetPaths = CSharpFramework.Contexts.IDropContext.GetAssetPaths(e);
      _assetTypes = CSharpFramework.Contexts.IDropContext.GetAssetTypes(e);
      _assetNames = CSharpFramework.Contexts.IDropContext.GetAssetNames(e);

      Debug.Assert(_assetPaths.Length == _assetTypes.Length && _assetPaths.Length == _assetNames.Length);
      if (_assetPaths.Length == 0)
        return;

      if (_assetTypes[0] == "Model")
      {
        createInstance(e);
      }
      else if (_assetTypes[0] == "StaticMesh")
      {
      }
      else if (_assetTypes[0] == "Prefab")
      {
      }
      else
      {
        Debug.Assert(false, "The asset type " + _assetTypes[0] + " is not supported by GeometryAsssetDropContext!");
      }
    }

    public override void DragLeave()
    {
      if (_instance != null)
      {
        _instance.Dispose();
        _instance = null;
      }

      if (_dummyShape != null)
      {
        _dummyShape.Dispose();
        _dummyShape = null;
      }
      EditorManager.ActiveView.UpdateView(false);
    }

    public override void DragOver(object sender, System.Windows.Forms.DragEventArgs e)
    {
      if (_instance == null && _dummyShape == null)
        return;

      Vector3F startRay, endRay;
      Vector3F hitPoint = new Vector3F();
      Point p = new Point(e.X, e.Y);
      Point relP = EditorManager.ActiveView.PointToClient(p);
      EditorManager.EngineManager.GetRayAtScreenPos(out startRay, out endRay, relP.X, relP.Y, EditorManager.Settings.MaxPickingDistance);
      if (EditorManager.EngineManager.GetTraceHit(startRay, endRay, ref hitPoint))
      {
        if (_instance != null)
          _instance.SetPosition(hitPoint.X, hitPoint.Y, hitPoint.Z);
        if (_dummyShape != null)
          _dummyShape.Position = hitPoint;
      }
      // No hit, so place the object near the camera on the cursor ray
      else
      {
        BoundingBox aabb = new BoundingBox();
        if (_dummyShape != null)
        {
          _dummyShape.GetLocalBoundingBox(ref aabb);
          Vector3F dir = endRay - startRay;
          dir.Normalize();
          _dummyShape.Position = startRay + dir * aabb.Radius * 3.0f;

          if (_instance != null)
            _instance.SetPosition(_dummyShape.Position.X, _dummyShape.Position.Y, _dummyShape.Position.Z);
        }
      }

      EditorManager.ActiveView.UpdateView(true);
    }

    public override void DragDrop(object sender, System.Windows.Forms.DragEventArgs e)
    {
      createShape(e);
      // Clones the dummy and places it correctly into the scene.
      EditorManager.ActiveView.DropObject(_dummyShape, e);
      EditorManager.ActiveView.Focus();

      // Cleanup
      DragLeave();
    }

    #endregion

    #region Member variables

    private string[] _assetPaths;
    private string[] _assetTypes;
    private string[] _assetNames;
    private Shape3D _dummyShape;
    private IEngineShapeInstance _instance;
    #endregion
  }

  #endregion

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
