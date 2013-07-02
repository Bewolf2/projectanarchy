/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/HavokBehaviorEnginePlugin.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorResource.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorResourceManager.hpp>

#include <Behavior/Utilities/Utils/hkbProjectAssetManager.h>

vHavokBehaviorResourceManager vHavokBehaviorResourceManager::g_GlobalManager;

vHavokBehaviorResourceManager::vHavokBehaviorResourceManager() 
	: VisResourceManager_cl("BehaviorManager", VRESOURCEMANAGERFLAG_SHOW_IN_VIEWER, 0),
	  m_projectAssetManager(HK_NULL)
{
}

vHavokBehaviorResourceManager::~vHavokBehaviorResourceManager()
{
}

/// should be called at plugin initialization time
void vHavokBehaviorResourceManager::OneTimeInit()
{
	Vision::ResourceSystem.RegisterResourceManager(this, VColorRef(25,60,200));

	// Set project asset manager
	vHavokBehaviorModule* behaviorModule = vHavokBehaviorModule::GetInstance();
	m_projectAssetManager = behaviorModule->GetProjectAssetManager();
	m_projectAssetManager->addReference();
}

/// should be called at plugin de-initialization time
void vHavokBehaviorResourceManager::OneTimeDeInit()
{
	m_projectAssetManager->removeReference();
	m_projectAssetManager = HK_NULL;

	Vision::ResourceSystem.UnregisterResourceManager(this);
}

VManagedResource* vHavokBehaviorResourceManager::CreateResource(const char *szFilename, VResourceSnapshotEntry *pExtraInfo)
{
	char szTempBuffer[FS_MAX_PATH];
	szFilename = VResourceManager::GetFilePathResolver().ResolvePath(szFilename,szTempBuffer);

	VManagedResource* pBehaviorResource = NULL;
	if (Vision::File.Exists(szFilename))
	{
		// create a new resource (unique for streaming resources)
		pBehaviorResource = new vHavokBehaviorResource(szFilename);
		pBehaviorResource->EnsureLoaded();
	}
	return pBehaviorResource;
}

hkbProjectAssetManager* vHavokBehaviorResourceManager::GetProjectAssetManager() const
{
	return m_projectAssetManager;
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
