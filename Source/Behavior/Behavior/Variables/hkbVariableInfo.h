/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_VARIABLE_INFO_H
#define HKB_VARIABLE_INFO_H

#include <Behavior/Behavior/Variables/hkbVariableValue.h>
#include <Behavior/Behavior/Attributes/hkbAttributes.h>

extern const class hkClass hkbVariableInfoClass;

	/// A description of a behavior variable..
class hkbVariableInfo
{
	public:
		// +version(1)

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbVariableInfo );
		HK_DECLARE_REFLECTION();

			/// Whether this variable fits into a machine word.
		HK_FORCE_INLINE bool isWord() const
		{
			return (	( m_type >= VARIABLE_TYPE_BOOL ) &&
						( m_type <= VARIABLE_TYPE_REAL ) );
		}

			/// Whether this variable fits into a quadword (vector or quaternion type).
		HK_FORCE_INLINE bool isQuad() const
		{
			return (	( m_type >= VARIABLE_TYPE_VECTOR3 ) &&
						( m_type <= VARIABLE_TYPE_QUATERNION ) );
		}

			/// Variable types.
		enum VariableType
		{
			VARIABLE_TYPE_INVALID = -1,

			// word-sized (at most)
			VARIABLE_TYPE_BOOL,
			VARIABLE_TYPE_INT8,
			VARIABLE_TYPE_INT16,
			VARIABLE_TYPE_INT32,
			VARIABLE_TYPE_REAL,

			VARIABLE_TYPE_POINTER, // this must be after the word-sized types and before the quad-sized types

			// quad-word sized
			VARIABLE_TYPE_VECTOR3,
			VARIABLE_TYPE_VECTOR4,
			VARIABLE_TYPE_QUATERNION,
		};

			/// The role and flags for this variable.
		struct hkbRoleAttribute m_role;

			/// The type of the variable.
		hkEnum<VariableType,hkInt8> m_type;
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
