/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_GET_HANDLE_ON_BONE_MODIFIER_H
#define HKB_GET_HANDLE_ON_BONE_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>
#include <Behavior/Behavior/Utils/hkbClosestLocalFrameCollector.h>

extern const class hkClass hkbGetHandleOnBoneModifierClass;

class hkbHandle;

/// A modifier that outputs a handle on a specified bone of the current character.  The output handle
/// can optionally refer to a local frame if you provide m_localFrameName.  The output handle can also
/// just refer to the character itself if you don't set either m_ragdollBoneIndex or m_animationBoneIndex.
class hkbGetHandleOnBoneModifier : public hkbModifier
{
	public:

		//+version(0)

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbGetHandleOnBoneModifier();

	protected:

		hkbGetHandleOnBoneModifier( const hkbGetHandleOnBoneModifier& mod );

	public:

		~hkbGetHandleOnBoneModifier();

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void deactivate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

	public:

			/// The handle that was found on the bone.
		hkRefPtr<hkbHandle> m_handleOut;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT|FLAG_NOT_CHARACTER_PROPERTY")
											//+hk.Description("The handle that was found on the bone.")

			/// The name of the local frame on the bone that we want. If this is null then the output handle
			/// will not have a local frame, and will refer directly to a bone or rigid body.
		hkStringPtr m_localFrameName;	//+hk.Description("The name of the local frame on the bone that we want. If this is null then the output handle will not have a local frame, and will refer directly to a bone or rigid body.")

			/// The bone in the ragdoll skeleton that you want a handle to.  You should set either this or m_animationBoneIndex, but not both.
		hkInt16 m_ragdollBoneIndex;	//+default(-1)
									//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_RAGDOLL")
									//+hk.Description("The bone in the ragdoll skeleton that you want a handle to. You should set either this or m_animationBoneIndex, but not both.")

			/// The bone in the animation skeleton that you want a handle to.
		hkInt16 m_animationBoneIndex;	//+default(-1)
										//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_NONE")
										//+hk.Description("The bone in the animation skeleton that you want a handle to. You should set either this or m_ragdollBoneIndex, but not both.")
	public:

		hkbGetHandleOnBoneModifier( hkFinishLoadedObjectFlag flag );

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
