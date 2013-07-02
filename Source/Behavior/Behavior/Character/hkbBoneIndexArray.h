/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_BONE_INDEX_ARRAY_H
#define HKB_BONE_INDEX_ARRAY_H

#include <Behavior/Behavior/Node/hkbBindable.h>

extern const class hkClass hkbBoneIndexArrayClass;

	/// An array of bone indices that can be bound to behavior variables and character properties.
class hkbBoneIndexArray : public hkbBindable
{
	//+vtable(1)

	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbBoneIndexArray() {}

		hkbBoneIndexArray( const hkbBoneIndexArray& boneIndexArray )
		:	hkbBindable(boneIndexArray)
		{
			m_boneIndices = boneIndexArray.m_boneIndices;
		}

	public:

			/// An array of bone indices.
		hkArray<hkInt16> m_boneIndices;	//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_NONE")
										//+hk.Description("A list of bone indices.")

	public:

		hkbBoneIndexArray( hkFinishLoadedObjectFlag flag ) : hkbBindable(flag), m_boneIndices(flag) {}

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
