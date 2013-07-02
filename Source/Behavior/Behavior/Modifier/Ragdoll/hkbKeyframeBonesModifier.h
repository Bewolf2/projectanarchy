/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_KEYFRAME_BONES_MODIFIER_H
#define HK_KEYFRAME_BONES_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>
#include <Behavior/Behavior/Character/hkbBoneIndexArray.h>

extern const class hkClass hkbKeyframeBonesModifierKeyframeInfoClass;

extern const class hkClass hkbKeyframeBonesModifierClass;

/// This modifier allows you to specify which bones are keyframed. One float per bone is placed
/// into the track data. This data is passed through the behavior graph and can be blended by 
/// blend nodes and during transitions. The track data is used by hkbPoweredRagdollModifier
/// and hkbRigidBodyRagdollModifier to decide which bones should be keyframed.
class hkbKeyframeBonesModifier : public hkbModifier
{
	public:
		// +version(3)

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbKeyframeBonesModifier() : hkbModifier(HKB_NODE_TYPE_KEYFRAME_BONES_MODIFIER) {}

		hkbKeyframeBonesModifier( const hkbKeyframeBonesModifier& mod );

	public:

		~hkbKeyframeBonesModifier();

		//////////////////////////
		// hkbBindable interface
		//////////////////////////

			// hkbBindable interface implementation.
		virtual void collectBindables( hkbBindableCollector& collector ) HK_OVERRIDE;

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		/////////////////////////
		// hkbModifier interface
		/////////////////////////

			// hkbModifier interface implementation.
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

	public:

			/// Specify that a bone is to be keyframed.  Do not call this twice with the same boneIndex.
		void setKeyframedBone( int boneIndex );

	public:

			/// One keyframe info for each ragdoll bone that needs to be keyframed with an explicit target.
		struct KeyframeInfo
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, KeyframeInfo );
			HK_DECLARE_REFLECTION();

				/// Construct with initial values.
			KeyframeInfo()
				:	m_keyframedPosition(hkVector4::getZero()),
					m_keyframedRotation(hkQuaternion::getIdentity()),
					m_boneIndex(-1),
					m_isValid(false)
			{}

				/// The position of the keyframed bone.
			hkVector4 m_keyframedPosition;	//+default(0,0,0)
											//+hk.Description("The position of the keyframed bone.")

				/// The orientation of the keyframed bone.
			hkQuaternion m_keyframedRotation;	//+default(0,0,0,1)
												//+hk.Description("The orientation of the keyframed bone.")

				/// The ragdoll bone to be keyframed. 
			hkInt16 m_boneIndex;	//+default(-1)
									//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_RAGDOLL")
									//+hk.Description("The ragdoll bone to be keyframed.")

				/// Whether or not m_keyframedPosition and m_keyframedRotation are valid.
			hkBool m_isValid;	//+default(false)
								//+hk.Description("Whether or not m_keyframedPosition and m_keyframedRotation are valid.")

			KeyframeInfo( hkFinishLoadedObjectFlag ) {}
		};

			/// The list of bones to be keyframed with an explicit target.
		hkArray<struct KeyframeInfo> m_keyframeInfo;	//+hk.Description("The bones that will be keyframed to explicit targets, not the pose.")

			/// Keyframed bone list that HAT can set.
		hkRefPtr<hkbBoneIndexArray> m_keyframedBonesList;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_RAGDOLL|FLAG_NOT_VARIABLE")
															//+hk.Description("The bones that will be keyframed to the pose.")

	public:

		hkbKeyframeBonesModifier( hkFinishLoadedObjectFlag flag );

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
