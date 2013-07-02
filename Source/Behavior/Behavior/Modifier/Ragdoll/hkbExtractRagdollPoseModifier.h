/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_EXTRACT_RAGDOLL_POSE_MODIFIER_H
#define HK_EXTRACT_RAGDOLL_POSE_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>

extern const class hkClass hkbExtractRagdollPoseModifierClass;

/// This modifier allows you to extract the current pose of the ragdoll 
class hkbExtractRagdollPoseModifier : public hkbModifier
{
	//+version(1)
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbExtractRagdollPoseModifier();

	protected:

		hkbExtractRagdollPoseModifier( const hkbExtractRagdollPoseModifier& mod );

	public:

		//////////////////////
		// hkbNode interface
		//////////////////////

			// hkbNode interface implementation.
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		//////////////////////////
		// hkbModifier interface
		//////////////////////////

			// hkbModifier interface implementation.
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

		///////////////
		// properties
		///////////////

			/// A bone to use for pose matching when computing the world-from-model.
		hkInt16 m_poseMatchingBone0;	//+default(-1)
										//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_RAGDOLL")
										//+hk.Description("A bone to use for pose matching when computing the world-from-model.")

			/// A bone to use for pose matching when computing the world-from-model.			
		hkInt16 m_poseMatchingBone1;	//+default(-1)
										//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_RAGDOLL")
										//+hk.Description("A bone to use for pose matching when computing the world-from-model.")

			/// A bone to use for pose matching when computing the world-from-model.			
		hkInt16 m_poseMatchingBone2;	//+default(-1)
										//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_RAGDOLL")
										//+hk.Description("A bone to use for pose matching when computing the world-from-model.")

			/// Whether to enable the computation of the world-from-model from the ragdoll pose.
		hkBool m_enableComputeWorldFromModel;	//+default(false)
												//+hk.Description("Whether to enable the computation of the world-from-model from the ragdoll pose.")

	public:

		hkbExtractRagdollPoseModifier( hkFinishLoadedObjectFlag flag );

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
