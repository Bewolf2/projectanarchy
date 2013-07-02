/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/Attachment.h>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Components/VSkeletalBoneProxy.hpp>


RPG_Attachment::RPG_Attachment(VisBaseEntity_cl* parent)
: m_boneName()
, m_positionOffset(0.0f, 0.0f, 0.0f)
, m_orientationOffset(0.0f, 0.0f, 0.0f)
, m_proxy(NULL)
, m_object(NULL)
, m_parent(parent)
{
}


RPG_Attachment::~RPG_Attachment()
{
}


void RPG_Attachment::Attach(VisObject3D_cl* object, const VString& boneName, const hkvVec3& positionOffset /*= hkvVec3(0.f, 0.f, 0.f)*/, const hkvVec3& orientationOffset /*= hkvVec3(0.f, 0.f, 0.f)*/)
{
  if(!m_parent ||
     !object)
  {
    return;
  }

  m_object = object;
  m_boneName = boneName;
  m_positionOffset = positionOffset;
  m_orientationOffset = orientationOffset;

  bool validBone = false;

  if(!boneName.IsEmpty())
  {
    if(m_parent->GetMesh()->GetSkeleton()->GetBoneIndexByName(m_boneName) != -1)
    {
      validBone = true;
    }
    else
    {
      Vision::Error.Warning("RPG_Attachment::Attach - Supplied bone name doesn't exist on this skeleton: %s", boneName.AsChar());
    }

  }

  if(validBone)
  {
    
    // attach the proxy to the parent bone if specified
    if (!m_proxy)
    {
      // @note: m_proxy is not deleted within this class, because the attachment parent deletes it
      m_proxy = new VSkeletalBoneProxyObject();
    }
    m_proxy->AttachToEntityBone(m_parent, m_boneName.AsChar());
    m_proxy->UpdateBoneBinding();

    // attach the new object to the proxy
    m_object->AttachToParent(m_proxy);

    VASSERT(m_proxy->GetParent() == m_parent);
    VASSERT(m_object->GetParent() == m_proxy);
  }
  else
  {
    // if no bone is specified, just attach to the parent object
    m_object->AttachToParent(m_parent);

    VASSERT(m_object->GetParent() == m_parent);
  }

  m_object->ResetLocalTransformation();
  m_object->SetLocalPosition(m_positionOffset);
  m_object->SetLocalOrientation(m_orientationOffset);
}


void RPG_Attachment::Detach(bool disposeObject)
{
  //if(m_proxy)
  //{
  //  m_proxy->DetachFromParent();
  //}

  if(m_object)
  {
    if(disposeObject)
    {
      // if disposeObject is true, take care of object destruction
      m_object->DisposeObject();
    }
    else
    {
      // if disposeObject is false, just detach the object, and let someone else take care of destroying the object
      m_object->DetachFromParent();
    }

    m_object = NULL;
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
