/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_ASSET_LOADER_H
#define HKB_ASSET_LOADER_H

class hkaSkeletonMapper;
class hkbGenerator;
class hkbBalanceRadialSelectorGenerator;
class hkbContext;
class hkaDefaultAnimationControl;
class hkStreamReader;

extern const hkClass hkbAssetBundleStringDataClass;
extern const hkClass hkbAssetBundleClass;

struct hkbAssetBundleStringData
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbAssetBundleStringData );
	HK_DECLARE_REFLECTION();

	hkbAssetBundleStringData() {}

		/// Copy Constructor explicitly defined to accommodate serialization of hkArray
	hkbAssetBundleStringData(const hkbAssetBundleStringData& src)
	{
		m_bundleName = src.m_bundleName;
		m_assetNames.setSize(src.m_assetNames.getSize());
		for ( int i = 0; i < src.m_assetNames.getSize(); i++ )
		{
			m_assetNames[i] = src.m_assetNames[i];
		}
	}

	hkbAssetBundleStringData(hkFinishLoadedObjectFlag flag): m_bundleName(flag), m_assetNames(flag) {}

		/// The filename of the animation bundle.
		/// If old-style animation loading is being used, this will be HK_NULL.
	hkStringPtr m_bundleName;

		/// The animation filenames to load.
		/// If new-style animation loading is being used, these are just for debug (the size is used though).
	hkArray<hkStringPtr> m_assetNames;
};

	/// Class which contains a bundle of assets.
	/// This is similar to the hkRootLevelContainer but is specific to Behavior and doesn't
	/// contain string data for each contained asset.
	/// It is currently only used for Animation bundles.
class hkbAssetBundle : public hkReferencedObject
{
	public:

		enum BundleType
		{
			ANIMATION_BUNDLE = 0,
		};

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR);
		HK_DECLARE_REFLECTION();

		hkbAssetBundle() {}
		hkbAssetBundle(hkFinishLoadedObjectFlag f) : hkReferencedObject(f), m_assets(f), m_name(f) {}
		~hkbAssetBundle() {}

	public:
			/// List of assets in this bundle
		hkArray<hkRefVariant> m_assets;

			/// Name of the asset bundle
		hkStringPtr m_name;

			/// Type of the asset bundle.  Currently only ANIMATION_BUNDLE is supported.
		hkEnum<hkbAssetBundle::BundleType, hkInt8> m_type;
};

	/// Abstract base class for loading an asset
class hkbAssetLoader : public hkReferencedObject
{
	public:
	
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR);

			/// Load the asset.  If you request the same asset more than once the same instance may be returned.
		virtual void* loadAsset( const char* rootPath, const char* filename, const hkClass& refClass ) = 0;
			/// Unload the given asset.
		virtual void unloadAsset( const char* rootPath, const char* filename ) = 0;
			/// Load the given asset from the given stream
		virtual void* loadAssetFromStream( const char* rootPath, const char* filename, hkStreamReader* stream, const hkClass& refClass ) = 0;
		
			/// Load an asset and return a fresh non-cached instance.  The default implementation assumes that the asset loader does not do
			/// any caching and so it just calls loadAsset().
		virtual void* loadUniqueAsset( const char* rootPath, const char* filename, const hkClass& refClass );
			/// Unload an asset returned by loadUniqueAsset.  The default implementation assumes that the asset loader does not do
			/// any caching and so it just calls unloadAsset().
		virtual void unloadUniqueAsset( const char* rootPath, const char* filename, void* asset );
};

	/// Abstract base class for saving an asset
class hkbAssetSaver : public hkReferencedObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR);

			/// Save the asset.
		virtual void saveAsset( void* object, const char* rootPath, const char* filename, const hkClass& refClass ) = 0;
};

#endif // HKB_ASSET_LOADER_H

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
