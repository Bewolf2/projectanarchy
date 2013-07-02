/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using namespace CSharpFramework;
using namespace CSharpFramework::Math;
using namespace CSharpFramework::Shapes;

class vHavokRigidBody;

namespace HavokManaged
{
  /// The AnchorDescription structure describes a Physics anchor and its related properties.
  /// Anchors can be either in world space or, if associated with an entity, in entity space.
  public ref struct AnchorDescription
  {
    /// Constructor for an entity anchor
    AnchorDescription(System::Int64 iAnchorId, VisBaseEntity_cl *pEntityIn, Vector3F ^pLocalSpaceAnchorPositionIn);

    /// Constructor for a world anchor (static)
    AnchorDescription(System::Int64 iAnchorId, Shape3D ^pStaticAnchorShape);

    ~AnchorDescription();

    /// Gets the entity pointer, or NULL if the anchor is a static world anchor
    VisBaseEntity_cl *GetEntity();

    /// Gets the rigid body component of the entity, or NULL if the anchor is a static world anchor
    vHavokRigidBody *GetRigidBody();

    /// Gets the position of the anchor in world space coordinates
    hkvVec3 GetWorldSpacePosition();

    /// Gets the anchor position.
    /// In case of entity anchors the position is relative to the entity.
    /// In case of static world anchors the position is in world coordinates.
    hkvVec3 GetAnchorPosition();

    /// Gets the unique of the anchor
    System::Int64 GetId() { return iAnchorId; }

  protected:
    System::Int64 iAnchorId;                  ///< unique identifier for this anchor
    Shape3D ^m_pStaticAnchorShape;            ///< pointer to the static anchor shape (or NULL for entity anchors)
    VWeakPtr<VisBaseEntity_cl> *m_pEntityPtr; ///< entity to which the anchor is relative (or NULL for static world anchors)    

    Vector3F vRelEntityPos;                   ///< relative position in local space of the entity (only relevant for entity anchors)
  };

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
