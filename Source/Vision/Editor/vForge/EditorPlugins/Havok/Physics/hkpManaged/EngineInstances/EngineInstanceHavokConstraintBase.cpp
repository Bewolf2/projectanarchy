/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/HavokPhysicsManagedPCH.h>

using namespace System::Diagnostics;

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/EngineInstances/EngineInstanceHavokConstraintBase.hpp>

namespace HavokManaged
{

  // --------------------------------------------------------------------------
  EngineInstanceHavokConstraintBase::EngineInstanceHavokConstraintBase()
  {
    m_pFirstAnchor = nullptr;
    m_pSecondAnchor = nullptr;

    m_bConfigurationValid = true;
    m_bDisposed = false;
  }

  // --------------------------------------------------------------------------
  bool EngineInstanceHavokConstraintBase::AddEntityAnchor(
    System::Int64 iAnchorId, EngineInstanceEntity ^pEntity, Vector3F ^pLocalSpacePosition)
  {
    // Ensure that the instance has not yet been disposed
    if (m_bDisposed)
    {
      Debug::Assert(false, "Engine instance has already been disposed!");
      return false;
    }

    // Check if exactly this anchor is already assigned. If yes, that's ok - just
    // make sure not to assign it a second time.
    if (m_pFirstAnchor && m_pFirstAnchor->GetId() == iAnchorId)
      return true;
    if (m_pSecondAnchor && m_pSecondAnchor->GetId() == iAnchorId)
      return true;

    // Check whether our constraint already has two connected entities. In that case this call fails.
    if (AreBothAnchorsDefined())
      return false;

    // Get the native entity
    VisBaseEntity_cl *pNativeEntity = GetNativeEntityFromEngineInstance(pEntity);
    if (pNativeEntity == NULL)
      return false;

    // Add the entity to our "list"
    if (!m_pFirstAnchor)
      m_pFirstAnchor = gcnew AnchorDescription(iAnchorId, pNativeEntity, pLocalSpacePosition);
    else if (!m_pSecondAnchor)
      m_pSecondAnchor = gcnew AnchorDescription(iAnchorId, pNativeEntity, pLocalSpacePosition);
    else
    {
      Debug::Assert(false);
      return false;
    }

    // Finally update the native constraint instance
    RecreateNativeObject();

    return true;
  }

  // --------------------------------------------------------------------------
  bool EngineInstanceHavokConstraintBase::AddWorldAnchor(
    System::Int64 iAnchorId, Shape3D ^pStaticAnchorShape)
  {
    // Ensure that the instance has not yet been disposed
    if (m_bDisposed)
    {
      Debug::Assert(false, "Engine instance has already been disposed!");
      return false;
    }

    // Check if exactly this anchor is already assigned. If yes, that's ok - just
    // make sure not to assign it a second time.
    if (m_pFirstAnchor && m_pFirstAnchor->GetId() == iAnchorId)
      return true;
    if (m_pSecondAnchor && m_pSecondAnchor->GetId() == iAnchorId)
      return true;

    // Check whether our constraint already has two connected anchors. In that case this call fails.
    if (AreBothAnchorsDefined())
      return false;

    // Add the entity to our "list"
    if (!m_pFirstAnchor)
      m_pFirstAnchor = gcnew AnchorDescription(iAnchorId, pStaticAnchorShape);
    else if (!m_pSecondAnchor)
      m_pSecondAnchor = gcnew AnchorDescription(iAnchorId, pStaticAnchorShape);
    else
    {
      Debug::Assert(false);
      return false;
    }

    // Finally update the native constraint instance
    RecreateNativeObject();

    return true;
  }

  // --------------------------------------------------------------------------
  bool EngineInstanceHavokConstraintBase::AreBothAnchorsDefined()
  {
    if (m_pFirstAnchor && m_pSecondAnchor)
      return true;

    return false;
  }

  // --------------------------------------------------------------------------
  void EngineInstanceHavokConstraintBase::DisposeObject()
  {
    delete m_pFirstAnchor;
    m_pFirstAnchor = nullptr;
    
    delete m_pSecondAnchor;
    m_pSecondAnchor = nullptr;
    
    m_bDisposed = true;
  }

  // --------------------------------------------------------------------------
  bool EngineInstanceHavokConstraintBase::GetLocalBoundingBox(BoundingBox ^%bbox)
  {
    float r = 5.0f * EditorManager::Settings->GlobalUnitScaling;
    (*bbox).Set(-r,-r,-r,r,r,r);
    return true;
  }

  // --------------------------------------------------------------------------
  VisBaseEntity_cl *EngineInstanceHavokConstraintBase::GetNativeEntityFromEngineInstance(
    EngineInstanceEntity ^pEntity)
  {
    // Get the native entity
    VisBaseEntity_cl *pNativeEntity = (VisBaseEntity_cl *) pEntity->GetObject3D();
    if (pNativeEntity == NULL)
      return NULL;

    // Check whether the native entity indeed is a Vision entity
    if (!pNativeEntity->IsOfType(V_RUNTIME_CLASS(VisBaseEntity_cl)))
      return NULL;

    return pNativeEntity;
  }

  // --------------------------------------------------------------------------
  bool EngineInstanceHavokConstraintBase::IsConfigurationValid()
  {
    if (GetNativeObject() != IntPtr::Zero)
      return true;

    return m_bConfigurationValid;
  }

  // --------------------------------------------------------------------------
  bool EngineInstanceHavokConstraintBase::RemoveAnchor(System::Int64 iAnchorId)
  {
    // If the instance has been disposed, the anchors are already removed.
    if (m_bDisposed)
      return true;

    // Remove the anchor from our "list"
    if (m_pFirstAnchor && (m_pFirstAnchor->GetId() == iAnchorId))
    {
      delete m_pFirstAnchor;
      m_pFirstAnchor = nullptr;
    }
    else if (m_pSecondAnchor && (m_pSecondAnchor->GetId() == iAnchorId))
    {
      delete m_pSecondAnchor;
      m_pSecondAnchor = nullptr;
    }
    else
      return true;

    // Finally update the native constraint instance
    RecreateNativeObject();

    return true;
  }

  // --------------------------------------------------------------------------
  void EngineInstanceHavokConstraintBase::SetObjectKey(String ^key)
  {
    m_sCachedObjectKey = key;
    UpdateObjectKey();
  }

  // --------------------------------------------------------------------------
  void EngineInstanceHavokConstraintBase::SetOrientation(float yaw,float pitch,float roll)
  {
    m_oriY = yaw;
    m_oriP = pitch;
    m_oriR = roll;

    hkvVec3 vDir, vDummy;

    // Create a look-at matrix based on the orientation
    hkvMat3 rotMat (hkvNoInitialization);
    rotMat.setFromEulerAngles (roll, pitch, yaw);

    // Get the front vector
    rotMat.getAxisXYZ(&vDir, &vDummy, &vDummy);
    m_dirX = vDir.x;
    m_dirY = vDir.y;
    m_dirZ = vDir.z;
  }

  // --------------------------------------------------------------------------
  void EngineInstanceHavokConstraintBase::SetPosition(float x,float y,float z)
  {
    m_posX = x;
    m_posY = y;
    m_posZ = z;
  }

  // --------------------------------------------------------------------------
  void EngineInstanceHavokConstraintBase::TraceShape(
    Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result)
  {
    ShapeTraceResult ^pResult = result;
    hkvVec3 vStart(rayStart.X,rayStart.Y,rayStart.Z);
    hkvVec3 vEnd(rayEnd.X,rayEnd.Y,rayEnd.Z);
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
