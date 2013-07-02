/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_SKIN_LOADER_H
#define HKB_SKIN_LOADER_H

#include <Common/Base/Container/StringMap/hkStorageStringMap.h>
#include <Behavior/Utilities/Utils/hkbAssetLoader.h>

class hkxMesh;
class hkMeshBody;
class hkMeshShape;
class hkbCharacter;
class hkMeshMaterial;
class hkbAssetLoader;
class hkaMeshBinding;
class hkObjectResource;
class hkaBoneAttachment;
class hkMemoryMeshSystem;
class hkaAnimationContainer;

	/// Loads deformable and rigid skins from a file and converts them to a format
	/// usable by the hkDebugDisplay.
class hkbSkinLoader: public hkReferencedObject					 
{
	public:	

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR);

			/// Initializes the loader.  If using hardware skinning set maxBonesPerSkin to the maximum
			/// number of bones your vertex shader supports.  If not using hardware skinning set to -1.			
		hkbSkinLoader( hkbAssetLoader* assetLoader, hkInt32 maxBonesPerSkin, bool ignoreSkins = false, bool unloadSkins = true, bool allowMipmaps = true );

			/// Destroys the loader and unloads any loaded assets
		virtual ~hkbSkinLoader();

			/// Loads the mesh identified by skinName in the file filename and returns it as a hkMeshBody.  If the mesh
			/// has already been loaded and converted it is returned immediately.  If the file could not be loaded,
			/// the mesh couldn't be found or converted HK_NULL is returned.
		hkMeshBody* loadSkin( hkbCharacter* character, const char* filename, const char* skinName );
			/// Loads the attachment identified by boneAttachmentName in the file filename and returns it as a 
			/// hkaBoneAttachment.  If the attachment has already been loaded and converted it is returned immediately.  
			/// If the file could not be loaded, the attachment couldn't be found or converted HK_NULL is returned.
		hkaBoneAttachment* loadBoneAttachment( hkbCharacter* character, const char* filename, const char* boneAttachmentName );		

			/// Unloads all skins associated with a particular filename
		void unloadFile( const char* filename );

			/// Versions pre2011 character data to a 2011 version.  This is required because pre-2011.1 skins and attachments
			/// are specified on a per-file basis.  In 2011.1 users can specify them at a per-skin and per-attachment level.
			/// Versioning effectively adds all skins and attachments found in each file to the character's setup.  This must
			/// be done here, as opposed to patch time, because the skin / attachment file must be loaded to discover the
			/// available skins / attachments.
		void versionPre2011SkinAndBoneAttachments( hkbCharacter* character );

	protected:

			/// Finds a skin in a file, if the skin is not found HK_NULL is returned.
		hkaMeshBinding* findSkinInFile( hkbCharacter* character, const char* filename, const char* name );
			/// Finds a bone attachment in a file, if the bone attachment is not found HK_NULL is returned.
		hkaBoneAttachment* findBoneAttachmentInFile( hkbCharacter* character, const char* filename, const char* name );

			/// Loads an animation container from a file, if the file cannot be loaded or an animation container isn't
			/// found HK_NULL is returned.
		hkaAnimationContainer* loadAnimationContainer( hkbCharacter* character, const char* filename );
		
			/// The filenames of the textures used by mesh are relative to the skin they reference.  This
			/// function resolves the filenames of the textures by making them absolute relative to the
			/// base filename.
		void resolveTextureFilenames( hkxMesh* mesh, const char* filename);

			/// Multiple materials can have the same name but have different texture stages.  Since the
			/// hkMeshMaterialRegistry uses names to look up materials, and not the properties of material
			/// (e.g. texture stage names), it is possible to get material conflicts.  To avoid this
			/// problem this method is used to build a unique name for each material used by mesh.  The 
			/// name generated is "materialname_texturestage0name_texturestage1name".
		void assignUniqueNamesToMaterials( hkbCharacter* character, hkxMesh* mesh );

	protected:

			// Entry for deformable skins that have been loaded and converted.
		struct SkinEntry
		{			
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_SCENE_DATA, hkbSkinLoader::SkinEntry );

			hkRefPtr<hkMeshBody> m_skin;
			hkStringPtr m_filename;
			hkStringPtr m_skinName;			
		};
		
			// Entry for rigid skins that have been loaded and converted.
		struct BoneAttachmentEntry
		{			
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_SCENE_DATA, hkbSkinLoader::BoneAttachmentEntry );

			hkRefPtr<hkaBoneAttachment> m_boneAttachment;
			hkStringPtr m_filename;
			hkStringPtr m_boneAttachmentName;		
		};			

	protected:

			/// Array of deformable skin files that have been loaded.
		hkArray<SkinEntry> m_loadedSkins;
			/// Array of rigid skin files that have been loaded.
		hkArray<BoneAttachmentEntry> m_loadedBoneAttachments;

			/// Utility to load packfiles and tagfiles.
		hkRefPtr<hkbAssetLoader> m_assetLoader;

			/// Mesh system to convert and store meshes in a memory format.
		hkMemoryMeshSystem* m_meshSystem;
		
			/// Maximum number of bones a skin can have per section.  If -1 then
			/// sections are not split for hardware skinning.
		hkInt32 m_maxBonesPerSkin;

			/// Whether to ignore all skins.  If this is true then dummy meshes are created for each character
			/// so that its skeleton can be displayed but not the skin.
		hkBool m_ignoreSkins;

			/// If this is false a skin will never be unloaded except in the destructor.
		hkBool m_unloadSkins;

			/// If this is false we won't allow mipmaps (usually to save on texture memory).
		hkBool m_allowMipmap;
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
