/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_CHARACTER_DEBUG_DATA_H
#define HKB_CHARACTER_DEBUG_DATA_H

class hkbCharacter;
class hkbSkinLoader;
class hkaBoneAttachment;
class hkbCharacterSkinInfo;

#include <Common/GeometryUtilities/Mesh/hkMeshBody.h>

	/// Container class for temporary debug data related to a hkbCharacter.  It is primarily
	/// used by the hkbBehaviorContext (and its processes) to load and hold data needed for
	/// their debugging purposes. 	
class hkbCharacterDebugData: public hkReferencedObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR);

			/// Linkage between an ID and a deformable skin.
		struct MeshBodyIdPair
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_SCENE_DATA, hkbCharacterDebugData::MeshBodyIdPair );

			hkUlong m_id;
			hkRefPtr<hkMeshBody> m_mesh;			
			hkStringPtr m_filename;
			hkStringPtr m_meshName;
		};

			/// Linkage between an ID and an attachment
		struct AttachmentIdPair
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_SCENE_DATA, hkbCharacterDebugData::MeshBodyIdPair );

			hkUlong m_id;
			hkRefPtr<hkaBoneAttachment> m_attachment;
			hkStringPtr m_filename;
			hkStringPtr m_attachmentName;			
		};

			/// Initializes an empty debug container associated to character.
		hkbCharacterDebugData( hkbCharacter* character );
			/// Deinitializes the container.
		virtual ~hkbCharacterDebugData();		

			/// Loads all of the skins in the character's character setup using loader.
		void loadSkins( hkbSkinLoader* loader );
			/// Unloads all of the skins in the character's character setup using loader.
		void unloadSkins( hkbSkinLoader* loader );
			/// Compares the skins loaded for the character against the skins specified 
			/// in the hkbCharacterSetup.  If skins have been added to the hkbCharacterSetup
			/// they are loaded, added to the newSkins map and included debug data's maps.  
			/// If skins have been removed from the hkbCharacterSetup they are unloaded, added to
			/// removedSkins map and remove from the debug data's maps.
		void refreshSkins( hkArray<MeshBodyIdPair>& newDeformableSkins, hkArray<AttachmentIdPair>& newBoneAttachments,
						   hkArray<MeshBodyIdPair>& removedDeformableSkins, hkArray<AttachmentIdPair>& removedBoneAttachments,
						   hkbSkinLoader* loader, bool& ragdollChanged);
			/// Copy's the skin IDs in the debug data to info.
		void getSkinIds(hkbCharacterSkinInfo& info);
					
			/// Gets a array of loaded deformable skins associated with the character.  This will be 
			/// empty until load skins is called.  The hkMeshBodies in the array may be shared by other
			/// characters.  The ID's are unique to this character, use these when interacting with the
			/// hkDebugDisplay
		const hkArray<MeshBodyIdPair>& getSkins() const { return m_skins; }
			/// Gets the array of bone attachments associated with this character that have been converted
		const hkArray<AttachmentIdPair>& getBoneAttachments() const { return m_boneAttachments; }

			/// Gets a size that can be used to render debug information in the scale of the 
			/// character.  Returns -1 if it has not been computed yet (no skeleton).
		hkReal getDebugSize() const { return m_debugSize; }						
		
			/// The index of the state in the root state machine that we want to start in.
		int m_startStateIndex;

			/// How many character viewers are viewing right now.  We have to reference count them so that
			/// we know when to discard the skins.  When remote debugging there may will be multiple character viewers
			/// so we need to know when nobody needs the skin anymore.
		int m_numCharacterViewers;

	protected:
	
			/// Computes the debug size for the character.
		hkReal computeDebugSize();			

	protected:

			/// The character this container is associated with.
		hkRefPtr<hkbCharacter> m_character;

			/// The length of the diagonal of an AABB fitted to the characters reference pose.
		hkReal m_debugSize;

			/// Array of deformable skins associated with the character and a unique ID for each.
		hkArray<MeshBodyIdPair> m_skins;
			/// Array of loaded attachments that is used to check when refreshing skins
		hkArray<AttachmentIdPair> m_boneAttachments;

			/// Keeps track of ragdoll changes
		hkBool m_hasRagdoll;
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
