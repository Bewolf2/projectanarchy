/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_MIRRORED_SKELETON_INFO_H
#define HKB_MIRRORED_SKELETON_INFO_H


extern const class hkClass hkbMirroredSkeletonInfoClass;

/// The setup parameters for performing mirroring on poses.  See hkaMirroredSkeleton for more details.
class hkbMirroredSkeletonInfo : public hkReferencedObject
{
	public:
		// +version(1)
		//+vtable(true)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );

		/// Construct with reasonable defaults.
		hkbMirroredSkeletonInfo();

		/// The axis of mirroring (in local bone space).
		hkVector4 m_mirrorAxis; //+default(1,0,0,0)+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NORMALIZED")

		/// The paired bones (the array index is a bone index and the value is the mapped bone index).
		hkArray<hkInt16> m_bonePairMap; //+hkb.RoleAttribute("ROLE_BONE_INDEX_MAP")

		/// The paired partitions (the array index is a partition index and the value is the mapped partition index).
		hkArray<hkInt16> m_partitionPairMap;

	public:

		hkbMirroredSkeletonInfo( hkFinishLoadedObjectFlag flag ) 
		:	hkReferencedObject(flag), 
			m_bonePairMap(flag),
			m_partitionPairMap(flag)
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
