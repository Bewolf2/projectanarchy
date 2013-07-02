/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_ALIGN_BONE_MODIFIER_H
#define HKB_ALIGN_BONE_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>

extern const class hkClass hkbAlignBoneModifierClass;

/// An example custom modifier for aligning a bone, or the characters world-from-model, with 
/// a fixed axis.
class hkbAlignBoneModifier : public hkbModifier
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbAlignBoneModifier();

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

		//////////////////////////
		// hkbModifier interface
		//////////////////////////

			// hkbModifier interface implementation
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

		///////////////
		// properties
		///////////////

		enum AlignModeABAM
		{
			ALIGN_MODE_CHARACTER_WORLD_FROM_MODEL = 0,
			ALIGN_MODE_ANIMATION_SKELETON_BONE
		};

		enum AlignTargetMode
		{
			ALIGN_TARGET_MODE_CHARACTER_WORLD_FROM_MODEL = 0,
			ALIGN_TARGET_MODE_RAGDOLL_SKELETON_BONE,
			ALIGN_TARGET_MODE_ANIMATION_SKELETON_BONE,
			ALIGN_TARGET_MODE_USER_SPECIFIED_FRAME_OF_REFERENCE,
		};

		/// This mode specifies from where the frame of reference 
		/// that has to be modified (aligned) comes from.
		hkEnum< AlignModeABAM, hkInt8 > m_alignMode;

		/// This mode specifies from where the goal frame of reference
		/// comes from.
		hkEnum< AlignTargetMode, hkInt8 > m_alignTargetMode;

		/// If this is set to false the frames are completely aligned.
		/// Otherwise only a single axis in each frame is aligned
		hkBool m_alignSingleAxis;

		/// If the m_alignSingleAxis is set to true the axis for each frame
		/// needs to be specified.

		/// The axis of the frame of reference that has to be modified (aligned).
		hkVector4 m_alignAxis;

		/// The axis of the goal frame of reference
		hkVector4 m_alignTargetAxis;

		/// If the m_alignTargetMode is set to ALIGN_TARGET_MODE_USER_SPECIFIED_FRAME_OF_REFERENCE,
		/// a reference frame has to be specified.
		hkQuaternion m_frameOfReference;

		/// Duration for the aligning of frames.
		hkReal m_duration;	//+hkb.RoleAttribute("ROLE_TIME","FLAG_NONE")

		/// If the m_alignMode is set to ALIGN_MODE_ANIMATION_SKELETON_BONE,
		/// the index of the bone from the animation skeleton should be set
		int m_alignModeIndex;

		/// If the m_alignTargetMode is set to either ALIGN_TARGET_MODE_RAGDOLL_SKELETON_BONE or
		/// ALIGN_TARGET_MODE_ANIMATION_SKELETON_BONE the index of the bone in there respective
		/// skeletons should be set
		int m_alignTargetModeIndex;

	private:

		// time elapsed since begin was called
		hkReal m_time; //+nosave

		// time elapsed since modify was called
		hkReal m_timeSinceLastModify; //+nosave

	public:
		
		hkbAlignBoneModifier( hkFinishLoadedObjectFlag flag );
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
