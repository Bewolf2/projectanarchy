/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_BONE_WEIGHT_ARRAY_H
#define HKB_BONE_WEIGHT_ARRAY_H

#include <Behavior/Behavior/Node/hkbBindable.h>

extern const class hkClass hkbBoneWeightArrayClass;

	/// An array of bone weights used when blending.
class hkbBoneWeightArray : public hkbBindable
{
	//+vtable(1)

	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbBoneWeightArray() {}

		hkbBoneWeightArray( const hkbBoneWeightArray& boneWeightArray )
		:	hkbBindable(boneWeightArray)
		{
			m_boneWeights = boneWeightArray.m_boneWeights;
		}

	public:

			/// A weight for each bone.  If the list is too short, missing bones are assumed to have weight 1.
		hkArray<hkReal> m_boneWeights;	//+hk.Description("A weight for each bone. If the list is too short, missing bones are assumed to have weight 1.")

	public:

		hkbBoneWeightArray( hkFinishLoadedObjectFlag flag ) : hkbBindable(flag), m_boneWeights(flag) {}
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
