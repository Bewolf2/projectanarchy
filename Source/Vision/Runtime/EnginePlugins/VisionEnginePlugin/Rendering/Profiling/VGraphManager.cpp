/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Profiling/VGraphObject.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Profiling/VGraphManager.hpp>

VGraphManager VGraphManager::g_GlobalManager;

VGraphManager::VGraphManager()
{
  m_Instances.Reserve(4);
  m_Instances.SetGrowBy(4);
}

VGraphManager::~VGraphManager()
{
}

void VGraphManager::OneTimeInit()
{
  Vision::Callbacks.OnEngineDeInitializing += this;
}

void VGraphManager::OneTimeDeInit()
{
  ReleaseAll();
  Vision::Callbacks.OnEngineDeInitializing -= this;
}

int VGraphManager::AddInstance(VGraphObject* pInstance)
{
  for(int i=m_Instances.GetUpperBound();i>= 0;i--)
  {
    if(!m_Instances[i])
    {
      m_Instances[i] = pInstance;
      return i;
    }
  }
  m_Instances.Append(pInstance);
  return m_Instances.GetUpperBound();
}

void VGraphManager::RemoveInstance(int iIndex)
{
  VASSERT(iIndex >= 0 && iIndex < m_Instances.GetLength());
  m_Instances[iIndex] = NULL;
}

void VGraphManager::ReleaseAll()
{
  m_Instances.RemoveAll();
}

void VGraphManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if(pData->m_pSender == &Vision::Callbacks.OnEngineDeInitializing)
    ReleaseAll();
}

VGraphManager& VGraphManager::GlobalManager()
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
