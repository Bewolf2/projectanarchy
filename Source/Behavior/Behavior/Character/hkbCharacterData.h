/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_CHARACTER_DATA_H
#define HKB_CHARACTER_DATA_H

#include <Behavior/Behavior/Character/hkbCharacterStringData.h>
#include <Behavior/Behavior/Driver/FootIk/hkbFootIkDriverInfo.h>
#include <Behavior/Behavior/Driver/HandIk/hkbHandIkDriverInfo.h>
#include <Behavior/Behavior/Attributes/hkbAttributes.h>
#include <Behavior/Behavior/Variables/hkbVariableInfo.h>
#include <Behavior/Behavior/Variables/hkbVariableValueSet.h>
#include <Behavior/Behavior/Character/hkbMirroredSkeletonInfo.h>
#include <Behavior/Utilities/Physics/hkbPhysicsBaseTypes.h>

extern const class hkClass hkbCharacterDataCharacterControllerInfoClass;

extern const class hkClass hkbCharacterDataClass;

class hkaMeshBinding;
class hkaBoneAttachment;
class hkbCharacterStringData;
class hkbMirroredSkeletonInfo;
class hkbVariableValueSet;

	/// The constant serializable data associated with a character.
	/// 
	/// HAT exports an instance of hkbCharacterData for each character.
class hkbCharacterData : public hkReferencedObject
{
	//+version(10)
	//+vtable(1)

	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbCharacterData();

		~hkbCharacterData();

			/// Get the string data for the character.
		const hkbCharacterStringData* getStringData() const { return m_stringData; }

			/// Get writable access to the string data.  This should almost never be used because
			/// the string data is shared by characters.  You should only use this at load and link time.
		hkbCharacterStringData* accessStringData() { return m_stringData; }

			/// Set the string data and updates the reference counts for the old and new data.
		void setStringData( hkbCharacterStringData* stringData );

			/// Get the information on mirroring the skeleton.
		const hkbMirroredSkeletonInfo* getMirroredSkeletonInfo() const { return m_mirroredSkeletonInfo; }

			/// Set the information on mirroring the skeleton.
		void setMirroredSkeletonInfo( hkbMirroredSkeletonInfo* mirroredSkeletonInfo );

			/// Add a character property that fits into 32 bits.
		template <typename T>
		int addCharacterPropertyWord( const char* name, hkbVariableInfo::VariableType type, T value )
		{
			hkbVariableInfo info;
			info.m_type = type;
			info.m_role.m_role = hkbRoleAttribute::ROLE_DEFAULT;
			info.m_role.m_flags = hkbRoleAttribute::FLAG_NONE;
			m_stringData->m_characterPropertyNames.pushBack( name );
			m_characterPropertyInfos.pushBack( info );

			if ( m_characterPropertyValues == HK_NULL )
			{
				m_characterPropertyValues.setAndDontIncrementRefCount( new hkbVariableValueSet() );
			}

			return m_characterPropertyValues->addWord<T>( value );
		}

			/// Add a character property that stores an object.
		int addCharacterPropertyObject( const char* name, hkReferencedObject* obj );

			/// Add a character property that fits into a quadword.
		template <typename T>
		int addCharacterPropertyQuad( const char* name, hkbVariableInfo::VariableType type, const T& value )
		{
			hkbVariableInfo info;
			info.m_type = type;
			info.m_role.m_role = hkbRoleAttribute::ROLE_DEFAULT;
			info.m_role.m_flags = hkbRoleAttribute::FLAG_NONE;
			m_stringData->m_characterPropertyNames.pushBack( name );
			m_characterPropertyInfos.pushBack( info );

			if ( m_characterPropertyValues == HK_NULL )
			{
				m_characterPropertyValues.setAndDontIncrementRefCount( new hkbVariableValueSet() );
			}

			return m_characterPropertyValues->addQuad<T>( value );
		}			
						
			/// Gets the details of the bone attachment at index and stores them in attachmentOut.  The relevant
			/// details are the attachment's bone index, bone-from-attachment transform and name.  Use the 
			/// hkaBoneAttachment::m_name member to associate an object in your game to the attachment.
		void getBoneAttachment( int index, hkaBoneAttachment& attachmentOut ) const;
			/// Gets the number of a attachments associated with the character data.
		int getNumBoneAttachments() const;

	public:

			/// The properties for the character controller.
		struct hkbCharacterControllerSetup m_characterControllerSetup;

			/// The model up vector in model space (usually (0,1,0) or (0,0,1)).
		hkVector4 m_modelUpMS;

			/// The model forward vector in model space.
		hkVector4 m_modelForwardMS;

			/// The model right vector in model space.
		hkVector4 m_modelRightMS;

			/// Definitions of the character properties that this character has available.
		hkArray<class hkbVariableInfo> m_characterPropertyInfos;

			/// The number of bones per LOD for the character
		hkArray<hkInt32> m_numBonesPerLod;

			/// The values of the character properties that this character has available.
		hkRefPtr<hkbVariableValueSet> m_characterPropertyValues;

			/// The properties for the footIk driver.
		hkRefPtr<hkbFootIkDriverInfo> m_footIkDriverInfo;

			/// The properties for the handIk driver.
		hkRefPtr<hkbHandIkDriverInfo> m_handIkDriverInfo;

			/// The properties for the aiControl driver. This must have 
			/// dynamic type hkbAiControlDriverInfo if it is not HK_NULL;
			/// it has static type hkReferencedObject so that it does not 
			/// depend on that header.
		hkRefPtr<hkReferencedObject> m_aiControlDriverInfo;

			/// The string data associated with the character.
		hkRefPtr<hkbCharacterStringData> m_stringData;

			/// Information about how to mirror the skeleton.
		hkRefPtr<hkbMirroredSkeletonInfo> m_mirroredSkeletonInfo;			

			/// An array of attachment bone indices.  This array is parallel to hkbCharacterStringData::m_skinNames.
		hkArray<hkInt16> m_boneAttachmentBoneIndices;

			/// An array of attachment bone-from-attachment transforms.  This array is parallel to hkbCharacterStringData::m_boneAttachmentNames
		hkArray<hkMatrix4> m_boneAttachmentTransforms;

			/// The character is scaled by this amount.
		hkReal m_scale;	//+default(1.0f)

			/// The number of hands that the character has (for hand IK). The default is 2.
			/// This is not set by HAT but you can set it yourself so that the hkbWorld
			/// can create an hkbGeneratorOutput with the appropriate tracks.
		hkInt16 m_numHands; //+nosave

			/// The number of float slots in the character's animations.  The default is 0.
			/// This is not set by HAT but you can set it yourself so that the hkbWorld
			/// can create an hkbGeneratorOutput with the appropriate tracks.
		hkInt16 m_numFloatSlots; //+nosave

	public:

		hkbCharacterData( hkFinishLoadedObjectFlag flag );
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
