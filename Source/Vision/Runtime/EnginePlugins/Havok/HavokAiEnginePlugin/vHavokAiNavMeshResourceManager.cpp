/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/HavokAiEnginePlugin.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiNavMeshResource.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiNavMeshResourceManager.hpp>

// for memory leak debugging
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

vHavokAiNavMeshResourceManager vHavokAiNavMeshResourceManager::g_GlobalManager;

vHavokAiNavMeshResourceManager::vHavokAiNavMeshResourceManager() 
	: VisResourceManager_cl("NavMeshManager",VRESOURCEMANAGERFLAG_SHOW_IN_VIEWER, 0)
{

}

vHavokAiNavMeshResourceManager::~vHavokAiNavMeshResourceManager()
{

}

/// should be called at plugin initialization time
void vHavokAiNavMeshResourceManager::OneTimeInit()
{
	Vision::ResourceSystem.RegisterResourceManager(this, VColorRef(100,40,0)); // dkw.todo: pick a color
}

/// should be called at plugin de-initialization time
void vHavokAiNavMeshResourceManager::OneTimeDeInit()
{
	Vision::ResourceSystem.UnregisterResourceManager(this);
}

VManagedResource* vHavokAiNavMeshResourceManager::CreateResource(const char *szFilename, VResourceSnapshotEntry *pExtraInfo)
{
	char szTempBuffer[FS_MAX_PATH];
	szFilename = VResourceManager::GetFilePathResolver().ResolvePath(szFilename,szTempBuffer);

	VManagedResource* pNavMeshResource = NULL;
	if (Vision::File.Exists(szFilename))
	{
		// create a new resource (unique for streaming resources)
		pNavMeshResource = new vHavokAiNavMeshResource(szFilename);
		pNavMeshResource->EnsureLoaded();
	}
	return pNavMeshResource;
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
