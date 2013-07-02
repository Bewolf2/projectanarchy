/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using CSharpFramework.Shapes;

using System;
using System.Runtime.Serialization;

namespace HavokEditorPlugin.Shapes
{
  #region Link Source

  /// <summary>
  /// Link source for the Havok constraint, with the following properties:
  /// - Can be connected with Havok constraint link targets only
  /// - Allows two connections at maximum (since a Havok constraint accepts two anchors only)
  /// </summary>
  [Serializable]
  public class LinkSourceHavokConstraint : LinkSource
  {
    /// <summary>
    /// Standard constructor
    /// </summary>
    /// <param name="owner"></param>
    public LinkSourceHavokConstraint(Shape3D owner)
      : base(owner, "attach to anchor", "LS_HavokConstraint")
    {
    }

    /// <summary>
    /// Default constructor used by the factory class
    /// </summary>
    /// <param name="ownerShape"></param>
    /// <param name="linkName"></param>
    /// <param name="primaryStringId"></param>
    public LinkSourceHavokConstraint(Shape3D ownerShape, string linkName, string primaryStringId)
      : base(ownerShape, linkName, primaryStringId)
    {
    }

    public override bool CanLink(ShapeLink other)
    {
      if (!(other is AnchorLinkTarget))
        return false;

      // If we are already connected, accept the link (such calls may occur)
      if (HasLinkTo(other))
        return true;

      // Do not allow more than two connections for a constraint link
      // (a constraint accepts two anchors at maximum)
      if (Links.Count >= 2)
        return false;

      return true;
    }

    #region ISerializable Members

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected LinkSourceHavokConstraint(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);
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
