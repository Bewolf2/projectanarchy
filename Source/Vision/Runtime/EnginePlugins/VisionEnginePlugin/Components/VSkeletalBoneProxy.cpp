/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Components/VSkeletalBoneProxy.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


VSkeletalBoneProxyManager VSkeletalBoneProxyManager::g_BoneProxyManager;

VSkeletalBoneProxyObject::VSkeletalBoneProxyObject()
{
  SetUseEulerAngles(FALSE);
  m_iBoneIndex = -1;
  VSkeletalBoneProxyManager::g_BoneProxyManager.AddProxy(this);
}

VSkeletalBoneProxyObject::~VSkeletalBoneProxyObject()
{
//  g_BoneProxyManager.RemoveProxy(this);
}

void VSkeletalBoneProxyObject::DisposeObject()
{
  VisObject3D_cl::DisposeObject();
  VSkeletalBoneProxyManager::g_BoneProxyManager.RemoveProxy(this);
}





V_IMPLEMENT_SERIAL( VSkeletalBoneProxyObject, VisObject3D_cl, 0, &g_VisionEngineModule );
void VSkeletalBoneProxyObject::Serialize( VArchive &ar )
{
  VisObject3D_cl::Serialize(ar);
  char iLocalVersion = 0;
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion==0,"Invalid version number");
    ar >> m_iBoneIndex;
    VASSERT_MSG(m_iBoneIndex==-1 || m_iBoneIndex<((VisBaseEntity_cl *)m_pParent)->GetMesh()->GetSkeleton()->GetBoneCount(), "Invalid bone index");
  } else
  {
    ar << iLocalVersion;
    ar << m_iBoneIndex;
  }
}

/*
void VSkeletalBoneProxyObject::ModSysNotifyFunctionParentAltered( int iFlags )
{
  if (m_iBoneIndex<0)
    VisObject3D_cl::ModSysNotifyFunctionParentAltered(iFlags);
}
*/

void VSkeletalBoneProxyObject::AttachToEntityBone(VisBaseEntity_cl *pEntity, const char *szBoneName)
{
  if (pEntity==NULL || !pEntity->HasMesh() || pEntity->GetMesh()->GetSkeleton()==NULL || !szBoneName || !szBoneName[0])
  {
    AttachToEntityBone(pEntity,-1);
    return;
  }
  int iBone = pEntity->GetMesh()->GetSkeleton()->GetBoneIndexByName(szBoneName);
  AttachToEntityBone(pEntity,iBone);

}

void VSkeletalBoneProxyObject::AttachToEntityBone(VisBaseEntity_cl *pEntity, int iBoneIndex)
{
  if (pEntity==NULL)
    iBoneIndex = -1;
  VASSERT_MSG(iBoneIndex==-1 || iBoneIndex<pEntity->GetMesh()->GetSkeleton()->GetBoneCount(), "Invalid bone index");
  m_iBoneIndex = iBoneIndex;
  if (pEntity!=GetParent())
  {
    DetachFromParent();
    if (pEntity)
      AttachToParent(pEntity);
  }
}


void VSkeletalBoneProxyObject::UpdateBoneBinding()
{
  VisBaseEntity_cl *pEntity = (VisBaseEntity_cl *)m_pParent;
  if (m_iBoneIndex<0 || pEntity==NULL)
    return;

  hkvVec3 vBonePos(hkvNoInitialization);
  hkvQuat boneRot(hkvNoInitialization);
  hkvMat3 boneMat(hkvNoInitialization);
  pEntity->GetBoneCurrentWorldSpaceTransformation(m_iBoneIndex,vBonePos,boneRot);
  boneMat = boneRot.getAsMat3();

  SetPosition(vBonePos);
  SetRotationMatrix(boneMat);
  //Vision::Game.DrawCube(GetPosition(),2.f);

}




VSkeletalBoneProxyManager &VSkeletalBoneProxyManager::GlobalManager()
{
  return g_BoneProxyManager;
}


void VSkeletalBoneProxyManager::OneTimeInit()
{
}

void VSkeletalBoneProxyManager::OneTimeDeInit()
{
  VASSERT(m_AllProxies.Count()==0);
  if (m_bCallbacksRegistered)
  {
    Vision::Callbacks.OnUpdateSceneFinished -= this;
    Vision::Callbacks.OnAfterSceneUnloaded -= this;
  }
}


void VSkeletalBoneProxyManager::AddProxy(VSkeletalBoneProxyObject *pObj)
{
  if (!m_bCallbacksRegistered)
  {
    m_bCallbacksRegistered = true;
    Vision::Callbacks.OnUpdateSceneFinished += this;
    Vision::Callbacks.OnAfterSceneUnloaded += this;
  }
  m_AllProxies.Add(pObj);
}

void VSkeletalBoneProxyManager::RemoveProxy(VSkeletalBoneProxyObject *pObj)
{
  m_AllProxies.SafeRemove(pObj);
  if (m_bCallbacksRegistered && m_AllProxies.Count()==0)
  {
    m_bCallbacksRegistered = false;
    Vision::Callbacks.OnUpdateSceneFinished -= this;
    Vision::Callbacks.OnAfterSceneUnloaded -= this;
  }
}

void VSkeletalBoneProxyManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if ( pData->m_pSender == &Vision::Callbacks.OnUpdateSceneFinished )
  {
    const int iCount = m_AllProxies.Count();
    for (int i=0;i<iCount;i++)
      m_AllProxies.GetAt(i)->UpdateBoneBinding();
  }
  else if ( pData->m_pSender == &Vision::Callbacks.OnAfterSceneUnloaded )
  {
    m_AllProxies.Clear();
    Vision::Callbacks.OnUpdateSceneFinished -= this;
    Vision::Callbacks.OnAfterSceneUnloaded -= this;
    m_bCallbacksRegistered = false;
  }
}


VSkeletalBoneProxyObject* VSkeletalBoneProxyManager::FindByKey(const char *szKey, DynArray_cl<VSkeletalBoneProxyObject *> *pStoreArray) const
{
  VSkeletalBoneProxyObject *pFound = NULL;
  int iFound = 0;
  const int iCount = m_AllProxies.Count();
  for (int i=0;i<iCount;i++)
  {
    VSkeletalBoneProxyObject* pInstance = m_AllProxies.GetAt(i);
    const char *szOtherKey = pInstance->GetObjectKey();

    if (szKey==szOtherKey || ((szKey != NULL) && (szOtherKey != NULL) && !_stricmp(szKey,szOtherKey))) // szKey can be NULL to find all without a key
    {
      if (!pStoreArray)
        return pInstance;
      if (!pFound) pFound = pInstance;
      (*pStoreArray)[iFound++] = pInstance;
    }
  }
  return pFound;
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
