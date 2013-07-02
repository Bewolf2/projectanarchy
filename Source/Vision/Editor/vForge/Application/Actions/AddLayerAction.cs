/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/*
using System;
using CSharpFramework;

namespace Editor.Actions
{
	/// <summary>
	/// Action that adds a new layer
	/// </summary>
	public class AddLayerAction : IAction
	{
    public AddLayerAction(Layer layer) {addedLayer = layer;}

    #region IAction Members

    public override void Do()
    {
      EditorApp.Scene.AddLayer(addedLayer,false);
    }

    public override void Undo()
    {
      EditorApp.Scene.RemoveLayer(addedLayer);
    }

    public override string ShortName {get {return "add layer";}}

    #endregion

    Layer addedLayer;
  }


  
  /// <summary>
  /// Action that removes a layer
  /// </summary>
  public class RemoveLayerAction : IAction
  {
    public RemoveLayerAction(Layer layer) {addedLayer = layer;}

    #region IAction Members

    public override void Do()
    {
      EditorApp.Scene.RemoveLayer(addedLayer);
    }

    public override void Undo()
    {
      EditorApp.Scene.AddLayer(addedLayer,false);
    }

    public override string ShortName {get {return "remove layer";}}

    #endregion

    Layer addedLayer;
  }

}
*/

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
