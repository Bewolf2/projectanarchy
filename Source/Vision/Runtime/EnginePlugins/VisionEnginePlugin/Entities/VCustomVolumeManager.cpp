/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/VCustomVolumeManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/VCustomVolumeObject.hpp>

VCustomVolumeManager VCustomVolumeManager::g_GlobalManager;

VCustomVolumeManager::VCustomVolumeManager() :
m_bAllowDeletion(true)
{
  m_instances.Reserve(32);
  m_instances.SetGrowBy(32);
}

VCustomVolumeManager::~VCustomVolumeManager()
{

}

void VCustomVolumeManager::OneTimeInit()
{
  Vision::Callbacks.OnWorldDeInit += this;
  Vision::Callbacks.OnEngineDeInitializing += this;
}

void VCustomVolumeManager::OneTimeDeInit()
{
  ReleaseAll();
  Vision::Callbacks.OnWorldDeInit -= this;
  Vision::Callbacks.OnEngineDeInitializing -= this;
}

int VCustomVolumeManager::AddInstance(VCustomVolumeObject* pInstance)
{
  for(int i=m_instances.GetUpperBound(); i >= 0; --i)
  {
    if(m_instances[i] == NULL)
    {
      m_instances[i] = pInstance;
      return i;
    }
  }
  m_instances.Append(pInstance);
  return m_instances.GetUpperBound();
}

void VCustomVolumeManager::RemoveInstance(int index)
{
  VASSERT(index >= 0 && index < m_instances.GetLength());
  if(m_bAllowDeletion)
    m_instances[index] = NULL;
}

VCustomVolumeObject* VCustomVolumeManager::SearchInstance(const char* szObjectKey) const
{
  int iCount = m_instances.GetLength();
  for (int i=0;i<iCount;i++)
  {
    VCustomVolumeObject* pCustomVolumeObject = m_instances.GetAt(i);
    if(pCustomVolumeObject->HasObjectKey(szObjectKey))
      return pCustomVolumeObject;
  }

  return NULL;
}

void VCustomVolumeManager::ReleaseAll()
{
  m_bAllowDeletion = false;
  for(int i=0; i<m_instances.GetLength(); ++i)
  {
    delete m_instances[i];
  }
  m_instances.SetSize(0,-1,false);
  m_bAllowDeletion = true;
}

void VCustomVolumeManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if(pData->m_pSender == &Vision::Callbacks.OnWorldDeInit ||
     pData->m_pSender == &Vision::Callbacks.OnEngineDeInitializing)
  {
    ReleaseAll();
  }
}

VCustomVolumeManager& VCustomVolumeManager::GlobalManager()
{
  return g_GlobalManager;
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
