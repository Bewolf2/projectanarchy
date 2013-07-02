/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKB_SCRIPT_ASSET_LOADER_H
#define HKB_SCRIPT_ASSET_LOADER_H

#include <Common/Base/Container/StringMap/hkStorageStringMap.h>

/// Load binary packfiles to cached buffers and load assets in a buffer in place.
class hkbScriptAssetLoader: public hkReferencedObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR);

		hkbScriptAssetLoader();
		virtual ~hkbScriptAssetLoader();

			/// Entry for loaded scripts
		struct ScriptEntry
		{			
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, ScriptEntry );

			ScriptEntry( const hkStringPtr& filePath, int contentBufferSize = 0 );

			bool m_precompiled;
			hkArray<char> m_content;
			hkStringPtr m_filePath;
		};

			/// Load a script and cache it.
		virtual void loadScript( const char* fullPath, bool forceLoad = false );

			/// Clears the script entries.
		virtual void clearEntries();

			/// Returns the script entries.
		hkStorageStringMap<ScriptEntry*>& accessScriptEntries();

	public:

			/// Whether to precompile the scripts.
		bool m_precompileScripts;

	private:

			/// Struct that stores information necessary to construct a character
			/// Array of scripts that have been loaded by the loader.
		hkStorageStringMap<ScriptEntry*> m_scriptEntries;
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
