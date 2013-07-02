/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKB_ATTRIBUTE_CLASS_H
#define HKB_ATTRIBUTE_CLASS_H

#include <Common/Base/Reflection/hkClassMember.h>
#include <Common/Base/Reflection/hkClassEnum.h>

extern const hkClass hkbRoleAttributeClass;

/// An attribute for describing the role of a property of a behavior node.  You
/// can also specify additional flags that influence how HAT presents the UI for the property.
struct hkbRoleAttribute
{
	//+defineattribute(true)
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_HKCLASS, hkbRoleAttribute );
		HK_DECLARE_REFLECTION();

		enum Role
		{
				/// No particular role.
			ROLE_DEFAULT,

				/// The property should be interpreted as a filename (char*).
			ROLE_FILE_NAME,

				/// The property is a bone index (hkInt16).
			ROLE_BONE_INDEX,

				/// The property is a bone index map (hkArray<hkInt16>).  Note that it is not recommended that you use this
				/// attribute because you will not be able to bind such a node property to a character property.
			ROLE_BONE_INDEX_MAP,

				/// The property is an event ID (hkInt32)
			ROLE_EVENT_ID,

				/// The property is a behavior variable index (hkInt32).
			ROLE_VARIABLE_INDEX,

				/// The property is a behavior attribute index (hkInt32).
			ROLE_ATTRIBUTE_INDEX,

				/// The property is a time in seconds (hkReal).
			ROLE_TIME,
				
				/// The property is a script
			ROLE_SCRIPT,

				/// The property is a local frame
			ROLE_LOCAL_FRAME,

				/// The property is a bone attachment
			ROLE_BONE_ATTACHMENT,
		};

		enum RoleFlags
		{
			//+defineflags(true)

				/// No flags.
			FLAG_NONE = 0,

				/// The property refers to the ragdoll skeleton.  Use this in conjunction with
				/// ROLE_BONE_WEIGHT_ARRAY, ROLE_BONE_INDEX, or ROLE_BONE_INDEX_MAP. 
			FLAG_RAGDOLL = 1,

				/// The property should be normalized (apply this to hkVector4).
			FLAG_NORMALIZED = 1<<1,

				/// HAT will not allow the property to be bound to a variable.
			FLAG_NOT_VARIABLE = 1<<2,

				/// HAT will not show the property in the UI.
			FLAG_HIDDEN = 1<<3,

				/// The property is an output (otherwise it is an input).
			FLAG_OUTPUT = 1<<4,

				/// HAT will not allow the property to be bound to a character property.
			FLAG_NOT_CHARACTER_PROPERTY = 1<<5,
		};

			/// The role of the associated property.
		hkEnum<Role, hkInt16> m_role;

			/// The flags for the associated property.
		hkFlags<RoleFlags, hkInt16> m_flags;
};

#endif // HKB_ATTRIBUTE_CLASS_H

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
