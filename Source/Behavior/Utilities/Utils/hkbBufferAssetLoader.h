/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKB_BUFFER_ASSET_LOADER_H
#define HKB_BUFFER_ASSET_LOADER_H

#include <Behavior/Utilities/Utils/hkbAssetLoader.h>
#include <Behavior/Utilities/Utils/hkbCachingAssetLoader.h>

class hkaAnimationContainer;
class hkaAnimationBinding;

/// Load binary packfiles to cached buffers and load assets in a buffer in place.
class hkbBufferAssetLoader: public hkbCachingAssetLoader
{
	public:
	
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR);
		
			// Entry for loaded buffers
		struct BufferEntry : public hkbCachingAssetLoader::CachedEntry
		{			
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_SCENE_DATA, hkbBufferAssetLoader::BufferEntry );

			hkArray<char> m_buffer;
			void* m_contents;
		};

			/// Returns the list of loaded resources
		const hkArray<BufferEntry>& getLoadedResources() const;

	protected:

			/// Loads a given a path/stream and returns the size of the buffer and whether it was user allocated
			/// Users should override this function if a different memory buffer loading scheme is necessary.
		virtual char* loadFileInternal( const hkStringBuf& fullPath, hkStreamReader* stream, int& bufferSize, bool& isUser );	

			/// StoringData, this is info needed to store a resource loaded by loadFile
		struct BufferInfo
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_SCENE_DATA, hkbBufferAssetLoader::BufferInfo );

			int m_bufferSize;
			char* m_buffer;
			bool m_userAllocated;
		};

	protected:

			/// hkbCachingAssetLoader implementation
		virtual void* loadFile( const hkStringBuf& fullPath, hkStreamReader* stream, void*& storingData ) HK_OVERRIDE;

			/// hkbCachingAssetLoader implementation
		virtual void unloadAssetFromCache( const hkStringBuf& fullPath, void* asset ) HK_OVERRIDE;

			/// hkbCachingAssetLoader implementation
		virtual void unloadAllAssets() HK_OVERRIDE;

			/// hkbCachingAssetLoader implementation
		virtual void* findFileInCache( const hkStringBuf& fullPath ) const HK_OVERRIDE;

			/// hkbCachingAssetLoader implementation
		virtual bool storeResourceInCache( void* resource, const hkStringBuf& fullPath, const void* storingData ) HK_OVERRIDE;

			///<todo.gd hkbCachingAssetLoader implementation: handle non hkRootLevelContainers
		//virtual bool findTopLevelObject( const char* filename, void* resource, const hkClass& refClass, void*& classObject ) HK_OVERRIDE;

			/// hkbCachingAssetLoader implementation
		virtual hkRootLevelContainer* getRootLevelContainer( void* resource ) HK_OVERRIDE;

			/// hkbCachingAssetLoader implementation
		virtual void setResourceOwner( hkResource*& owner, void* resource ) HK_OVERRIDE;

			/// hkbCachingAssetLoader implementation
		virtual void storeUniqueAsset( void* uniqueClassObj, void* resource, bool lookInCache ) HK_OVERRIDE;
		
			/// Array of buffers that have been loaded by the loader.
		hkArray<BufferEntry> m_loadedBuffers;	
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
