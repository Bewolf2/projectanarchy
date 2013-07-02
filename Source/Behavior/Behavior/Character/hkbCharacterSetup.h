/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_CHARACTER_SETUP_H
#define HKB_CHARACTER_SETUP_H

#include <Animation/Animation/Animation/Mirrored/hkaMirroredSkeleton.h>
#include <Animation/Animation/Rig/hkaSkeleton.h>
#include <Animation/Animation/Mapper/hkaSkeletonMapper.h>
#include <Behavior/Behavior/Linker/hkbSymbolIdMap.h>
#include <Behavior/Behavior/Character/hkbAnimationBindingSet.h>

extern const class hkClass hkbCharacterSetupClass;

class hkbCharacterData;

/// A collection of constant data that describes a character setup.
/// 
/// Typically, an instance of hkbCharacterSetup is created at runtime for each character.
/// Since the data is constant, one instance of hkbCharacterSetup can be shared among
/// several hkbCharacters.
class hkbCharacterSetup : public hkReferencedObject
{
	public:

		//+version(2)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );

			/// Construction with reasonable initial values.
		hkbCharacterSetup();
		virtual ~hkbCharacterSetup();

			/// Get the character data.
		const hkbCharacterData* getData() const { return m_data; }

			/// Access the character data in writable form.  This should almost never be used because the
			/// character data may be shared by many characters.  Only use this at loading and linking time. 
		hkbCharacterData* accessData() { return m_data; }

			/// Set the character data and updates the reference counts on the old and new data.
		void setData( hkbCharacterData* data );

			/// This returns the unscaled version of the skeleton when the character is scaled.
		const hkaSkeleton* getUnscaledSkeleton() const;

			/// Set the unscaled skeleton. This needs to be called just before scaling the character.
		void setUnscaledSkeleton(const hkaSkeleton* skeleton);
			
			/// Get the mirrored skeleton used for mirroring poses.
		const hkaMirroredSkeleton* getMirroredSkeleton() const;

			/// Set the mirrored skeleton used for mirroring poses.
		void setMirroredSkeleton( hkaMirroredSkeleton* mirroredSkeleton );

			/// Get the retargeting skeleton mapper given the source skeleton name
		const hkaSkeletonMapper* getRetargetingSkeletonMapper( const char* sourceSkeletonName ) const;

			/// Get the map from internal character property IDs to external IDs.
		const hkbSymbolIdMap* getCharacterPropertyIdMap() const;

			/// Set the map from internal character property IDs to external IDs.
		void setCharacterPropertyIdMap( hkbSymbolIdMap* characterPropertyIdMap );

	public:

			/// Array of skeleton mappers for animation retargeting.
		hkArray<hkRefPtr<const hkaSkeletonMapper> > m_retargetingSkeletonMappers;

			/// The animation skeleton. For the scaled character the reference pose inside this skeleton is scaled.
		hkRefPtr<const hkaSkeleton> m_animationSkeleton;

			/// A mapper from the ragdoll skeleton to the animation skeleton.
		hkRefPtr<const hkaSkeletonMapper> m_ragdollToAnimationSkeletonMapper;

			/// A mapper from the animation skeleton to the ragdoll skeleton.
		hkRefPtr<const hkaSkeletonMapper> m_animationToRagdollSkeletonMapper;

			/// The animation bindings used for this character.  These get hooked up based on the data in
			/// hkbCharacterStringData::m_animationBundleNameData and hkbCharacterStringData::m_animationBundleFilenameData;
		hkRefPtr<hkbAnimationBindingSet> m_animationBindingSet;	//+nosave

	private:

			// Serialized data for the character.
		hkRefPtr<hkbCharacterData> m_data;

			/// The animation skeleton. If the character is scaled then this points to the unscaled version of the skeleton.
			/// If character is not scaled then this is null.
		hkRefPtr<const hkaSkeleton> m_unscaledAnimationSkeleton; //+nosave

			// A mirrored skeleton to perform animation mirroring.
			// This is mutable because it is computed on the fly during getMirroredSkeleton().
		mutable hkRefPtr<hkaMirroredSkeleton> m_mirroredSkeleton; //+nosave+overridetype(void*)

			// An ID mapper from internal character property IDs to external IDs.
		hkRefPtr<hkbSymbolIdMap> m_characterPropertyIdMap; //+nosave

			// Critical section to mediate shared thread access.
		mutable hkCriticalSection* m_criticalSection; //+nosave

	public:

		hkbCharacterSetup( hkFinishLoadedObjectFlag flag );
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
