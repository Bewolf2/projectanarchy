/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/HavokPhysicsManagedPCH.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRigidBody.hpp>

namespace HavokManaged
{

  AnchorDescription::AnchorDescription(System::Int64 iAnchorIdIn, 
    VisBaseEntity_cl *pEntityIn, Vector3F ^pLocalSpaceAnchorPositionIn)
  { 
    iAnchorId = iAnchorIdIn;
    m_pEntityPtr = new VWeakPtr<VisBaseEntity_cl>(pEntityIn->GetWeakReference());
    vRelEntityPos = *pLocalSpaceAnchorPositionIn;
    m_pStaticAnchorShape = nullptr;
  }

  AnchorDescription::AnchorDescription(System::Int64 iAnchorIdIn, Shape3D ^pStaticAnchorShape)
  {
    iAnchorId = iAnchorIdIn;
    m_pStaticAnchorShape = pStaticAnchorShape;
    m_pEntityPtr = NULL;
  }

  AnchorDescription::~AnchorDescription()
  {
    // Delete the weak reference pointer (NOT the entity itself)
    V_SAFE_DELETE(m_pEntityPtr);
  }

  VisBaseEntity_cl *AnchorDescription::GetEntity()
  {
    if (m_pEntityPtr == NULL)
      return NULL;

    VASSERT(m_pEntityPtr->GetPtr()->IsOfType(V_RUNTIME_CLASS(VisBaseEntity_cl)));

    return m_pEntityPtr->GetPtr();
  }

  vHavokRigidBody *AnchorDescription::GetRigidBody()
  {
    VisBaseEntity_cl *pEntity = GetEntity();
    if (pEntity == NULL)
      return NULL;

    // Get the Havok rigid body component
    vHavokRigidBody *pRigidBody = pEntity->Components().GetComponentOfBaseType<vHavokRigidBody>();
    return pRigidBody;
  }

  hkvVec3 AnchorDescription::GetWorldSpacePosition()
  {
    if (m_pStaticAnchorShape)
    {
      Vector3F shapePosition = m_pStaticAnchorShape->Position;
      return hkvVec3(shapePosition.X, shapePosition.Y, shapePosition.Z);
    }
    else if (m_pEntityPtr)
    {
      hkvVec3 vPos = GetEntity()->GetPosition();

      hkvVec3 localPos(vRelEntityPos.X, vRelEntityPos.Y, vRelEntityPos.Z);
      hkvVec3 wsPos = vPos + GetEntity()->GetRotationMatrix() * localPos;

      return hkvVec3(wsPos.x, wsPos.y, wsPos.z);
    }

    // Should never happen
    VASSERT(false);
    return hkvVec3();
  }

  hkvVec3 AnchorDescription::GetAnchorPosition()
  {
    if (m_pStaticAnchorShape)
    {
      // For static world anchors we return the world space coordinates of the actor as anchor
      Vector3F shapePosition = m_pStaticAnchorShape->Position;
      return hkvVec3(shapePosition.X, shapePosition.Y, shapePosition.Z);
    }
    else if (m_pEntityPtr)
    {
      // For entity we return the position of the anchor relative to the actor
      return hkvVec3(vRelEntityPos.X, vRelEntityPos.Y, vRelEntityPos.Z);
    }

    // Should never happen
    VASSERT(false);
    return hkvVec3();    
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
