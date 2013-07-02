/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_DETECT_CLOSE_TO_GROUND_MODIFIER_H
#define HKB_DETECT_CLOSE_TO_GROUND_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>

extern const class hkClass hkbDetectCloseToGroundModifierClass;

	/// A modifier that sends an event when the character is close to the ground.
class hkbDetectCloseToGroundModifier : public hkbModifier
{
	public:
		// +version(2)

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbDetectCloseToGroundModifier();

	protected:

		hkbDetectCloseToGroundModifier( const hkbDetectCloseToGroundModifier& mod );

	public:

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		//////////////////////////
		// hkbModifier interface
		//////////////////////////

			// hkbModifier interface implementation
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

		///////////////
		// properties
		///////////////

			/// Event to send when the bone is below the m_closeToGroundHeight.
		class hkbEventProperty m_closeToGroundEvent;  //+hk.Description("Event to send when the bone is below the m_closeToGroundHeight.")

			/// If the specified bone is below this height it is considered to be close to the ground.
		hkReal m_closeToGroundHeight;	//+default(0.5)
										//+hk.RangeReal(absmin=0,absmax=100,softmin=0,softmax=1)
										//+hk.Description("If the specified bone is below this height it is considered to be close to the ground.")

			/// When ray casting from the bone towards the ground, the (positive) distance, 
			/// from the bone and in the down direction, where the ray ends. Default is 0.8(m), you may
			/// want to change this according to the scale of your character
		hkReal m_raycastDistanceDown;	//+default(0.8f)
										//+hk.RangeReal(absmin=0,absmax=100,softmin=0,softmax=1)
										//+hk.Description("When ray casting from the bone towards the ground, the distance below the bone where the ray ends.")

			/// The collision filter info to use when raycasting the ground.
		hkUint32 m_collisionFilterInfo;	//+default(3)
										//+hk.Description("The collision filter info to use when raycasting the ground.  Normally, you should set this to a layer that collides with your ground terrain.")

			/// The bone used to cast ray for determining the ground height.
		hkInt16 m_boneIndex;	//+default(-1)
								//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_RAGDOLL")
								//+hk.Description("The bone used to cast ray for determining the ground height.")

			/// The bone used to cast ray for determining the ground height.
		hkInt16 m_animBoneIndex;	//+default(-1)
									//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_NONE")
									//+hk.Description("The bone used to cast ray for determining the ground height.")

	private:

		HKB_BEGIN_INTERNAL_STATE(0);

			/// When the bone goes below the m_closeToGoundHeight then this flag is set to true.
		hkBool m_isCloseToGround; //+nosave

		HKB_END_INTERNAL_STATE();

	public:

		hkbDetectCloseToGroundModifier( hkFinishLoadedObjectFlag flag );

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
