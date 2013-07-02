/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_CACHING_ASSET_LOADER_H
#define HKB_CACHING_ASSET_LOADER_H

#include <Common/Serialize/Resource/hkObjectResource.h>
#include <Behavior/Utilities/Utils/hkbAssetLoader.h>

class hkaAnimationContainer;
class hkaAnimationBinding;
class hkRootLevelContainer;

/// Load object on the heap as hkReferenceObjects and caches them by filename.
class hkbCachingAssetLoader: public hkbAssetLoader
{
	public:
	
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );

		//////////////////////////
		// hkbAssetLoader interface
		//////////////////////////

			/// Loads an asset on the heap and returns the top level object.  If the file could not be loaded
			/// or the top level object was not of type refClass HK_NULL is returned.
		virtual void* loadAsset( const char* rootPath, const char* filename, const hkClass& refClass ) HK_OVERRIDE;

			/// If an object with the given name is in the cache of loaded resources, remove it.
			/// Bases match on filename only, not path.
		virtual void unloadAsset( const char* rootPath, const char* filename ) HK_OVERRIDE;

			// hkbAssetLoader interface implementation.
		virtual void* loadUniqueAsset( const char* rootPath, const char* filename, const hkClass& refClass ) HK_OVERRIDE;

			// hkbAssetLoader interface implementation.
		virtual void unloadUniqueAsset( const char* rootPath, const char* filename, void* asset ) HK_OVERRIDE;

			
			
			// hkbAssetLoader interface implementation.
		virtual void* loadAssetFromStream( const char* rootPath, const char* filename, hkStreamReader* stream, const hkClass& refClass ) HK_OVERRIDE;

			/// Adds a path that will be searched if an asset is not found under the root path provided to
			/// loadAsset.  Behavior assets are exported from the Animation Tool relative to the exported project 
			/// file.  There are cases however where assets are specified relative to another source.  One example 
			/// are assets provided by the Animation Tool while remotely connected to a server.  In this case they 
			/// tend to be  relative to the client project file (.hkp) not the exported version.  To cover 
			/// this case you can specify the project path with this method.			
		void addAlternateRoot( const char* rootPath );

			/// Loads an asset on the heap and returns the top level object and the resource that contains the object.  
			/// If the file could not be loaded or the top level object was not of type refClass HK_NULL is returned.
		void* loadAsset( const char* rootPath, const char* filename, const hkClass& refClass, hkResource*& owner, bool lookInCache, hkStreamReader* stream );

		// Entry for loaded resources
		struct CachedEntry
		{			
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_SCENE_DATA, hkbCachingAssetLoader::CachedEntry );

				/// Filename for the entry being cached
			hkStringPtr m_filename;

				// Store the asset here if it was loaded using loadUniqueAsset(), otherwise HK_NULL
			void* m_assetIfUnique;
		};

	protected:		

			/// Internal version of loadAsset
		virtual void* loadAssetInternal( const char* rootPath, const char* filename, const hkClass& refClass, hkResource*& owner, bool lookInCache, hkStreamReader* stream );

			/// If the animation requires a skeleton (e.g. predictive, quantized) this function tries
			/// to set the skeleton on the animation from the given animation container.
		void setSkeletonOnAnimation( hkaAnimationContainer& animationContainer, hkaAnimationBinding& animationBinding );

			// Get the top level object from the resource.
			// This default implementation will try and use a root level container returned by getRootLevelContainer.
		virtual bool findTopLevelObject( const char* filename, void* resource, const hkClass& refClass, void*& classObject );

			/// Loads a file and returns it's contents as a hkObjectResource.  If the file could not be loaded
			/// HK_NULL is returned.  Use this method to override how hkbOnHeapAssetLoader loads a file.
		virtual void* loadFile( const hkStringBuf& fullPath, hkStreamReader* stream, void*& storingData ) = 0;

			/// Unloads a specific asset that may be unique (if asset exists).
		virtual void unloadAssetFromCache( const hkStringBuf& fullPath, void* asset ) = 0;

			/// Clears the list of loaded resources.
		virtual void unloadAllAssets() = 0;

			/// Looks for the full path in the m_loadedResources cache.
		virtual void* findFileInCache( const hkStringBuf& fullPath ) const = 0;

			/// Stores the resource in the m_loadedResources cache
		virtual bool storeResourceInCache( void* resource, const hkStringBuf& fullPath, const void* storingData ) = 0;

			/// Converts resource to an hkRootLevelContainer.
		virtual hkRootLevelContainer* getRootLevelContainer( void* resource ) = 0;

			/// Convert resource to an hkResource and set to owner, if possible.
		virtual void setResourceOwner( hkResource*& owner, void* resource ) = 0;

			/// Store specific unique asset
		virtual void storeUniqueAsset( void* uniqueClassObj, void* resource, bool lookInCache ) = 0;

			/// Array of root paths where assets might be found.
		hkArray<hkStringPtr> m_alternateRootPaths;
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
