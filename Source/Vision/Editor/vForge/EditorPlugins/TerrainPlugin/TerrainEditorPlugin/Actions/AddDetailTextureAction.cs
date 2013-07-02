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
using TerrainEditorPlugin.Shapes;
using TerrainBase.Editing;
using System.Collections;
using System.Runtime.Serialization;

namespace TerrainEditorPlugin.Actions
{
  #region Class AddDetailTextureAction

  /// <summary>
  /// Adds a detail texture to the terrain
  /// </summary>
  [Serializable]
  public class AddDetailTextureAction : IAction
  {
    public AddDetailTextureAction(TerrainShape terrain, DetailTextureResource texture)
    {
      _texture = texture;
      _terrain = terrain;
    }

    DetailTextureResource _texture;
    TerrainShape _terrain;

    public override string ShortName
    {
      get { return string.Format("Add detail texture '{0}'", _texture.ToString()); }
    }

    public override void Do()
    {
      _terrain.AddDetailTexture(_texture);
    }

    public override void Undo()
    {
      _terrain.DeleteDetailTexture(_texture);
      if (TerrainEditor.CurrentDetailTexture == _texture)
        TerrainEditor.CurrentDetailTexture = null;
    }

    #region ISerializable Members

    protected AddDetailTextureAction(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      _terrain = (TerrainShape)info.GetValue("_terrain", typeof(TerrainShape));
      _texture = (DetailTextureResource)info.GetValue("_texture", typeof(DetailTextureResource));
    }

    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);
      info.AddValue("_terrain", _terrain);
      info.AddValue("_texture", _texture);
    }

   #endregion

  }

  #endregion

  #region Class DeleteDetailTextureAction

  /// <summary>
  /// Removes a detail texture from the terrain
  /// </summary>
  [Serializable]
  public class DeleteDetailTextureAction : IAction
  {
    public DeleteDetailTextureAction(TerrainShape terrain, DetailTextureResource texture)
    {
      _texture = texture;
      _terrain = terrain;
    }

    DetailTextureResource _texture;
    TerrainShape _terrain;

    public override string ShortName
    {
      get { return string.Format("Delete detail texture '{0}'", _texture.ToString()); }
    }

    public override void Do()
    {
      _terrain.DeleteDetailTexture(_texture);
      if (TerrainEditor.CurrentDetailTexture == _texture)
        TerrainEditor.CurrentDetailTexture = null;
    }

    public override void Undo()
    {
      _terrain.AddDetailTexture(_texture);
    }

    #region ISerializable Members

    protected DeleteDetailTextureAction(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      _terrain = (TerrainShape)info.GetValue("_terrain", typeof(TerrainShape));
      _texture = (DetailTextureResource)info.GetValue("_texture", typeof(DetailTextureResource));
    }

    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);
      info.AddValue("_terrain", _terrain);
      info.AddValue("_texture", _texture);
    }

   #endregion

  }

  #endregion

  #region Class MoveDetailTextureAction

  /// <summary>
  /// Moves a detail texture within the list
  /// </summary>
  [Serializable]
  public class MoveDetailTextureAction : IAction
  {
    public MoveDetailTextureAction(TerrainShape terrain, DetailTextureResource texture, int iMoveDelta)
    {
      _texture = texture;
      _terrain = terrain;
      _iMoveDelta = iMoveDelta;
      iOldIndex = _terrain.DetailTextures.IndexOf(_texture);
      iNewIndex = iOldIndex + iMoveDelta;
    }

    DetailTextureResource _texture;
    TerrainShape _terrain;
    int _iMoveDelta;
    int iOldIndex, iNewIndex;

    public override string ShortName
    {
      get { return string.Format("Move detail texture '{0}' {1}", _texture.ToString(), _iMoveDelta > 0 ? "down" : "up"); }
    }

    public override bool Valid
    {
      get
      {
        return CanMove(_terrain, _texture, _iMoveDelta);
      }
    }

    void DoMove(int iOld, int iNew)
    {
      EditorManager.Progress.ShowProgressDialog("Resort detail textures");
      _terrain.DetailTextures.RemoveAt(iOld);
      _terrain.DetailTextures.Insert(iNew, _texture);
      _terrain.EngineTerrain.UpdateDetailTextureOrder(TerrainEditor.CurrentTerrain.DetailTextures, true, EditorManager.Progress);
      _terrain.Modified = true;
      TerrainEditor.TriggerOnDetailTextureListChanged(new DetailTextureListChangedArgs(DetailTextureListChangedArgs.Action.OrderChanged, null));
      EditorManager.Progress.HideProgressDialog();
    }

    public override void Do()
    {
      DoMove(iOldIndex, iNewIndex);
    }

    public override void Undo()
    {
      DoMove(iNewIndex,iOldIndex);
    }


    /// <summary>
    /// Helper function
    /// </summary>
    /// <param name="_terrain"></param>
    /// <param name="_texture"></param>
    /// <param name="iMoveDelta"></param>
    /// <returns></returns>
    public static bool CanMove(TerrainShape _terrain, DetailTextureResource _texture, int iMoveDelta)
    {
      if (_terrain == null || !_terrain.Modifiable || _terrain.EngineTerrain == null || _texture == null)
        return false;

      int iOldIndex = _terrain.DetailTextures.IndexOf(_texture);
      int iNewIndex = iOldIndex + iMoveDelta;

      if (_texture._bIsBaseTexture)
        return false;

      if (iOldIndex < 0 || iOldIndex >= _terrain.DetailTextures.Count)
        return false;
      if (iNewIndex < 1 || iNewIndex >= _terrain.DetailTextures.Count)
        return false;
      return true;
    }

    #region ISerializable Members

    protected MoveDetailTextureAction(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      _terrain = (TerrainShape)info.GetValue("_terrain", typeof(TerrainShape));
      _texture = (DetailTextureResource)info.GetValue("_texture", typeof(DetailTextureResource));
      iOldIndex = info.GetInt32("iOldIndex");
      iNewIndex = info.GetInt32("iNewIndex");
    }

    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);
      info.AddValue("_terrain", _terrain);
      info.AddValue("_texture", _texture);
      info.AddValue("iOldIndex", iOldIndex);
      info.AddValue("iNewIndex", iNewIndex);
    }

   #endregion

  }

  #endregion

  #region Class NewDetailTextureCollectionAction

  /// <summary>
  /// Exchanges the full collection, including support for new channels etc. 
  /// </summary>
  [Serializable]
  public class NewDetailTextureCollectionAction : IAction
  {
    public NewDetailTextureCollectionAction(TerrainShape terrain, ArrayList oldList, ArrayList newList)
    {
      _terrain = terrain;
      _oldList = oldList;
      _newList = newList;

      // re-enum new objects:
      foreach (DetailTextureResource tex in _newList)
        if (tex.ID == 0)
        {
          tex._ownerTerrain = _terrain;
          tex.ID = _terrain.GetNewDetailTextureID();
        }
    }

    TerrainShape _terrain;
    ArrayList _oldList, _newList;

    public override string ShortName
    {
      get { return "Changed detail textures"; }
    }

    void SetNewList(ArrayList oldList, ArrayList newList)
    {
      EditorManager.Progress.ShowProgressDialog("Resort detail textures");
      _terrain.DetailTextures = newList;
      _terrain.EngineTerrain.UpdateDetailTextureOrder(_terrain.DetailTextures, true, EditorManager.Progress);
      EditorManager.Progress.HideProgressDialog();
      EditorManager.ActiveView.UpdateView(false);
      _terrain.Modified = true;

      TerrainEditor.TriggerOnDetailTextureListChanged(new DetailTextureListChangedArgs(DetailTextureListChangedArgs.Action.NewCreated, null));
      if (TerrainEditor.CurrentDetailTexture != null && !newList.Contains(TerrainEditor.CurrentDetailTexture))
        TerrainEditor.CurrentDetailTexture = null;
    }


    public override void Do()
    {
      SetNewList(_oldList, _newList);
    }


    public override void Undo()
    {
      SetNewList(_newList, _oldList);
    }

    public override bool Valid
    {
      get
      {
        if (_newList.Count < 1)
          return false;
        if (!((DetailTextureResource)_newList[0])._bIsBaseTexture)
          return false;
        return base.Valid;
      }
    }

    #region ISerializable Members

    protected NewDetailTextureCollectionAction(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      _terrain = (TerrainShape)info.GetValue("_terrain", typeof(TerrainShape));
      _oldList = (ArrayList)info.GetValue("_oldList", typeof(ArrayList));
      _newList = (ArrayList)info.GetValue("_newList", typeof(ArrayList));

    }

    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);
      info.AddValue("_terrain", _terrain);
      info.AddValue("_oldList", _oldList);
      info.AddValue("_newList", _newList);
    }

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
