/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_CHARACTER_STRING_DATA_H
#define HKB_CHARACTER_STRING_DATA_H


extern const class hkClass hkbCharacterStringDataClass;

#include <Common/Base/Container/StringMap/hkStorageStringMap.h>
#include <Behavior/Utilities/Utils/hkbAssetLoader.h>

/// The string data for a character.  This is typically information that is
/// exported by HAT, and can be optionally used at runtime.  If you don't need this
/// data you should consider pruning it in your asset pipeline.
class hkbCharacterStringData : public hkReferencedObject
{
	//+vtable(1)
	//+version(9)
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbCharacterStringData() {}

			/// A filename, mesh name pair used to uniquely identify a mesh in a file.
		struct FileNameMeshNamePair
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, FileNameMeshNamePair );
			HK_DECLARE_REFLECTION();

				/// Default constructor
			FileNameMeshNamePair() {}
				/// Initializes the pair with a filename only.  The special "*" character is used as 
				/// the mesh name.  The "*" informs the system to load all meshes in the file.  This 
				/// feature is used to version pre-2011.1 assets.
			FileNameMeshNamePair(hkStringPtr filename) { m_fileName = filename; m_meshName = "*";}
				/// Serialization constructor
			FileNameMeshNamePair(hkFinishLoadedObjectFlag flag): m_fileName(flag), m_meshName(flag) {}

				/// The file which contains the mesh.
			hkStringPtr m_fileName;
				/// The name of the mesh in the file.  A "*" indicates that all meshes in the file should be loaded.
			hkStringPtr m_meshName;
		};

			/// Array of meshes that should be used as skins for the character.
		hkArray<FileNameMeshNamePair> m_skinNames;
			
			/// Array of attachments that should be used as bone attachments for the character.
		hkArray<FileNameMeshNamePair> m_boneAttachmentNames;

			/// This is an array of string name data for loading animation assets.
			/// This array is used to match animation assets with the clips that use them.
			/// If an override is not found in the FilenameData then this NameData is also used
			/// as the filename for loading.
		hkArray<hkbAssetBundleStringData> m_animationBundleNameData;

			/// This array contains file name data for loading animation assets.
			/// If there is a non-null corresponding entry in this array for an asset
			/// then it is used for loading (Note: this array may not be as long as NameData).
			/// This is how character property overrides for animations are implemented.
		hkArray<hkbAssetBundleStringData> m_animationBundleFilenameData;

			/// The names (and implicit IDs) of the character properties that this character has available.
		hkArray<hkStringPtr> m_characterPropertyNames;

			/// The filenames of the skeleton mappers for the character.
			///
			/// This allows for retargeting the animations.
		hkArray<hkStringPtr> m_retargetingSkeletonMapperFilenames;

			/// The names of the LOD for the character
		hkArray<hkStringPtr> m_lodNames;

			/// To form the mirrored pair of a sync point name, all of the substrings in m_mirroredSyncPointSubstringsA are replaced by their
			/// corresponding substrings in m_mirroredSyncPointSubstringsB.
		hkArray<hkStringPtr> m_mirroredSyncPointSubstringsA;

			/// To form the mirrored pair of a sync point name, all of the substrings in m_mirroredSyncPointSubstringsA are replaced by their
			/// corresponding substrings in m_mirroredSyncPointSubstringsB.
		hkArray<hkStringPtr> m_mirroredSyncPointSubstringsB;

			/// The name of the character.
		hkStringPtr m_name;

			/// The name of the rig.
		hkStringPtr m_rigName;

			/// The name of the ragdoll.
		hkStringPtr m_ragdollName;

			/// The name of the behavior
		hkStringPtr m_behaviorFilename;

			/// The script to run when character is activated
		hkStringPtr m_luaScriptOnCharacterActivated;

			/// The script to run when character is deactivated
		hkStringPtr m_luaScriptOnCharacterDeactivated;

			/// Lua files to register (call dofile on) when character is activated
		hkArray<hkStringPtr> m_luaFiles;

	public:

		hkbCharacterStringData( hkFinishLoadedObjectFlag flag ) 
			:	hkReferencedObject(flag), 
				m_skinNames(flag),
				m_boneAttachmentNames(flag),
				m_animationBundleNameData(flag),
				m_animationBundleFilenameData(flag),
				m_characterPropertyNames(flag),
				m_retargetingSkeletonMapperFilenames(flag),
				m_lodNames(flag),
				m_mirroredSyncPointSubstringsA(flag),
				m_mirroredSyncPointSubstringsB(flag),
				m_name(flag),
				m_rigName(flag),
				m_ragdollName(flag),
				m_behaviorFilename(flag),
				m_luaScriptOnCharacterActivated(flag),
				m_luaScriptOnCharacterDeactivated(flag),
				m_luaFiles(flag)
		{}
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
