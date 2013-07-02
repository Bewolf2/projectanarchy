/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_ON_HEAP_ASSET_LOADER_H
#define HKB_ON_HEAP_ASSET_LOADER_H

#include <Common/Serialize/Resource/hkObjectResource.h>
#include <Behavior/Utilities/Utils/hkbAssetLoader.h>
#include <Behavior/Utilities/Utils/hkbCachingAssetLoader.h>

class hkaAnimationContainer;
class hkaAnimationBinding;

/// Load object on the heap as hkReferencedObjects and caches them by filename.
class hkbOnHeapAssetLoader: public hkbCachingAssetLoader
{
	public:
	
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );

		// Entry for loaded resources
		struct ResourceEntry : public hkbCachingAssetLoader::CachedEntry
		{			
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_SCENE_DATA, hkbOnHeapAssetLoader::ResourceEntry );

			hkRefPtr<hkResource> m_resource;
		};

			/// Returns the list of loaded resources
		const hkArray<ResourceEntry>& getLoadedResources() const;

	public:

			/// hkbCachingAssetLoader implementation
		virtual void* loadFile( const hkStringBuf& fullPath, hkStreamReader* stream, void*& storingData ) HK_OVERRIDE;

			/// hkbCachingAssetLoader implementation
		virtual void unloadAssetFromCache( const hkStringBuf& fullPath, void* asset ) HK_OVERRIDE;

			/// hkbCachingAssetLoader implementation
		virtual void unloadAllAssets() HK_OVERRIDE;

			/// hkbCachingAssetLoader implementation
		virtual void* findFileInCache( const hkStringBuf& fullPath ) const HK_OVERRIDE;

	protected:

			/// hkbCachingAssetLoader implementation
		virtual bool storeResourceInCache( void* resource, const hkStringBuf& fullPath, const void* storingData ) HK_OVERRIDE;

			/// hkbCachingAssetLoader implementation: handle non hkRootLevelContainers
		virtual bool findTopLevelObject( const char* filename, void* resource, const hkClass& refClass, void*& classObject ) HK_OVERRIDE;

			/// hkbCachingAssetLoader implementation
		virtual hkRootLevelContainer* getRootLevelContainer( void* resource ) HK_OVERRIDE;

			/// hkbCachingAssetLoader implementation
		virtual void setResourceOwner( hkResource*& owner, void* resource ) HK_OVERRIDE;

		virtual void storeUniqueAsset( void* uniqueClassObj, void* resource, bool lookInCache ) HK_OVERRIDE;

			/// Array of resources that have been loaded by the loader.
		hkArray<ResourceEntry> m_loadedResources;
};

#endif

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
