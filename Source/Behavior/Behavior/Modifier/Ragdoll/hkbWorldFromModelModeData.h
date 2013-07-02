/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_WORLD_FROM_MODEL_MODE_DATA_H
#define HKB_WORLD_FROM_MODEL_MODE_DATA_H


extern const class hkClass hkbWorldFromModelModeDataClass;

	/// How to treat the world-from-model when using the powered ragdoll controller.
struct hkbWorldFromModelModeData
{
	HK_DECLARE_REFLECTION();
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbWorldFromModelModeData );

		/// A bone to use for pose matching used when computing the world-from-model for the ragdoll.
	hkInt16 m_poseMatchingBone0;	//+default(-1)
									//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_RAGDOLL")
									//+hk.Description("A bone to use for pose matching when computing the world-from-model for the ragdoll.")

		/// A bone to use for pose matching used when computing the world-from-model for the ragdoll.
	hkInt16 m_poseMatchingBone1;	//+default(-1)
									//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_RAGDOLL")
									//+hk.Description("A bone to use for pose matching when computing the world-from-model for the ragdoll.")

		/// A bone to use for pose matching used when computing the world-from-model for the ragdoll.
	hkInt16 m_poseMatchingBone2;	//+default(-1)
									//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_RAGDOLL")
									//+hk.Description("A bone to use for pose matching when computing the world-from-model for the ragdoll.")

		/// How to compute the output worldFromModel when using the powered ragdoll controller.
		/// When using the rigid body ragdoll controller the input worldFromModel is always returned.
	enum WorldFromModelMode
	{
			/// Return the previous worldFromModel of the character, ignoring the incoming worldFromModel.
			/// This causes the character's position and orientation to remain fixed while in ragdoll mode.
		WORLD_FROM_MODEL_MODE_USE_OLD,

			/// Return the worldFromModel that accompanies the input pose.  This usually only makes sense
			/// if you are keyframing some of the bones, because then the ragdoll tends to follow
			/// the motion in the animation.
		WORLD_FROM_MODEL_MODE_USE_INPUT,

			/// Compute the worldFromModel by matching the animation to the ragdoll.
		WORLD_FROM_MODEL_MODE_COMPUTE,

			/// Used to indicate that the mode should not be set.
		WORLD_FROM_MODEL_MODE_NONE,
	};

		/// How to treat the world-from-model when using the powered ragdoll controller.
	hkEnum< WorldFromModelMode, hkInt8 > m_mode;	//+default(hkbWorldFromModelModeData::WORLD_FROM_MODEL_MODE_NONE)
													//+hk.Description("How to treat the world-from-model when using the powered ragdoll controller.")
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
