/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/HavokBehaviorEnginePlugin.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorResource.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorResourceManager.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorComponent.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokFileStreamAccess.hpp>

#include <Behavior/Utilities/Utils/hkbProjectAssetManager.h>
#include <Behavior/Utilities/Utils/hkbAssetLoader.h>
#include <Common/Base/System/Io/Reader/Buffered/hkBufferedStreamReader.h>

extern const hkClass hkClassClass;

vHavokBehaviorAssetLoader::vHavokBehaviorAssetLoader( vHavokBehaviorResourceManager* resourceManager )
{
	m_resourceManager = resourceManager;
}

void* vHavokBehaviorAssetLoader::loadFile( const hkStringBuf& fullPath, hkStreamReader* stream, void*& storingData  )
{
	if( stream == HK_NULL )
	{
		// Load the stream
		IVFileInStream *pIn = m_resourceManager->CreateFileInStream( fullPath.cString(), HK_NULL );
		if (!pIn)
		{
			return HK_NULL;
		}

		// Wrap in a Havok stream reader
		vHavokStreamReader havokStreamReader(pIn);
		if (!havokStreamReader.isOk())
		{
			return HK_NULL;
		}

		hkBufferedStreamReader bufferedStreamReader(&havokStreamReader);
		if (!bufferedStreamReader.isOk())
		{
			return HK_NULL;
		}

		 return hkbOnHeapAssetLoader::loadFile( fullPath, &bufferedStreamReader, storingData );
	}

	// The stream reader here must be of type hkBufferedStreamReader to go through Vision's file interface
	return hkbOnHeapAssetLoader::loadFile( fullPath, stream, storingData );
}

vHavokBehaviorResource::vHavokBehaviorResource( const char* filePath )
:	VManagedResource(vHavokBehaviorResourceManager::GetInstance())
{
	Init();
	SetFilename(filePath);
}

vHavokBehaviorResource::~vHavokBehaviorResource()
{
	EnsureUnloaded();
}

void vHavokBehaviorResource::Init()
{
	FlagAsUnLoaded();
	SetResourceFlag(VRESOURCEFLAG_ALLOWUNLOAD);
	SetResourceFlag(VRESOURCEFLAG_AUTOUNLOAD);
	SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
}

BOOL vHavokBehaviorResource::Reload()
{
	// Deserialize the project asset
	vHavokBehaviorResourceManager* manager = static_cast<vHavokBehaviorResourceManager*>( GetParentManager() );
	const char* projectFilename = GetFilename();

	// Load the project
	hkbProjectAssetManager* projectAssetManager = manager->GetProjectAssetManager();
	projectAssetManager->loadProject( projectFilename );

	FlagAsLoaded();
	UpdateMemoryFootprint();
	return TRUE;
}

// unload from memory
BOOL vHavokBehaviorResource::Unload()
{
	// hkbProjectAssetManager will handle unloading

	FlagAsUnLoaded();
	UpdateMemoryFootprint();
	return true;
}

void vHavokBehaviorResource::GetDependencies(VResourceSnapshot &snapshot)
{
#ifdef SUPPORTS_SNAPSHOT_CREATION

	VManagedResource::GetDependencies(snapshot);

#endif
}

vHavokBehaviorScriptAssetLoader::vHavokBehaviorScriptAssetLoader(vHavokBehaviorResourceManager *resourceManager)
  : m_resourceManager(resourceManager)
{
}

void vHavokBehaviorScriptAssetLoader::loadScript(char const *filePath, bool forceLoad)
{
  hkStorageStringMap<ScriptEntry*>& scriptEntries = accessScriptEntries();

  // Check for cached script
  ScriptEntry* cachedEntry = HK_NULL;
  if(scriptEntries.hasKey(filePath))
  {
    if( !forceLoad )
    {
      //Already loaded.
      return;
    }
		
    // Want to force load the script asset
    scriptEntries.get(filePath, &cachedEntry);
  }

  // Load the stream
  IVFileInStream *pIn = m_resourceManager->CreateFileInStream(filePath, HK_NULL);
  if(!pIn)
  {
    return;
  }

  LONG const size = pIn->GetSize();

  if(forceLoad && cachedEntry != HK_NULL)
  {
    // Clear out current cached entry
    cachedEntry->m_content.clearAndDeallocate();

    // Set new entry
    cachedEntry->m_content.setSize(size);
    pIn->Read(cachedEntry->m_content.begin(), size);
  }
  else
  {
    // Create new script entry
    ScriptEntry* entry = new ScriptEntry(filePath, size);
    pIn->Read(entry->m_content.begin(), size);

    // Add to map
    scriptEntries.insert(filePath, entry);
  }

  pIn->Close();
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
