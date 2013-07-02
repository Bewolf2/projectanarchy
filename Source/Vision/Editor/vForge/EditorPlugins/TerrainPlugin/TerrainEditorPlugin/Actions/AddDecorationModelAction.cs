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
using TerrainBase.Editing;
using TerrainEditorPlugin.Shapes;
using System.Runtime.Serialization;

namespace TerrainEditorPlugin.Actions
{
  #region Class AddDecorationModelAction

  /// <summary>
  /// Adds a new decoration model to the terrain
  /// </summary>
  [Serializable]
  public class AddDecorationModelAction : IAction
  {
    public AddDecorationModelAction(TerrainShape terrain, DecorationModelResource model)
    {
      _model = model;
      _terrain = terrain;
    }

    DecorationModelResource _model;
    TerrainShape _terrain;

    public override string ShortName
    {
      get { return string.Format("Add decoration model '{0}'", _model.ToString()); }
    }

    public override void Do()
    {
      _terrain.AddDecorationModelResource(_model);
      TerrainEditor.TriggerOnDecorationModelListChanged();
    }

    public override void Undo()
    {
      _terrain.DeleteDecorationModelResource(_model);
      if (TerrainEditor.CurrentDecorationModel == _model)
        TerrainEditor.CurrentDecorationModel = null;
      TerrainEditor.TriggerOnDecorationModelListChanged();
    }

    #region ISerializable Members

    protected AddDecorationModelAction(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      _terrain = (TerrainShape)info.GetValue("_terrain", typeof(TerrainShape));
      _model = (DecorationModelResource)info.GetValue("_model", typeof(DecorationModelResource));
    }

    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);
      info.AddValue("_model", _model);
      info.AddValue("_terrain", _terrain);
    }

   #endregion

  }

  #endregion

  #region Class DeleteDecorationModelAction


  /// <summary>
  /// Removes a decoration model and all instances in the world
  /// </summary>
  [Serializable]
  public class DeleteDecorationModelAction : IAction
  {
    public DeleteDecorationModelAction(TerrainShape terrain, DecorationModelResource model)
    {
      _model = model;
      _terrain = terrain;
    }

    DecorationModelResource _model;
    TerrainShape _terrain;

    public override string ShortName
    {
      get { return string.Format("Delete decoration model '{0}'", _model.ToString()); }
    }

    public override void Do()
    {
      _terrain.DeleteDecorationModelResource(_model);
      if (TerrainEditor.CurrentDecorationModel == _model)
        TerrainEditor.CurrentDecorationModel = null;
      TerrainEditor.TriggerOnDecorationModelListChanged();
    }

    public override void Undo()
    {
      _terrain.AddDecorationModelResource(_model);
      TerrainEditor.TriggerOnDecorationModelListChanged();
    }


    #region ISerializable Members

    protected DeleteDecorationModelAction(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      _terrain = (TerrainShape)info.GetValue("_terrain", typeof(TerrainShape));
      _model = (DecorationModelResource)info.GetValue("_model", typeof(DecorationModelResource));
    }

    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);
      info.AddValue("_model", _model);
      info.AddValue("_terrain", _terrain);
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
