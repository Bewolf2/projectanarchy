/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_GET_UP_MODIFIER_H
#define HKB_GET_UP_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>

extern const class hkClass hkbGetUpModifierClass;

	/// Gradually aligns the up-vector of the character's world-from-model transform
	/// with the world up-vector.  This is useful when the character is getting
	/// up after having been a ragdoll.  When using the hkbPoweredRagdollModifier
	/// the world-from-model of the character is typically allowed to rotate
	/// freely.  But when you want the character to stand up again, the world-from-model
	/// needs to be realigned with the world so that the character stands up straight.
class hkbGetUpModifier : public hkbModifier
{
	public:

		//+version(2)

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbGetUpModifier();

	protected:

		hkbGetUpModifier( const hkbGetUpModifier& mod );

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

		///////////////////////////
		// hkbModifier interface
		///////////////////////////

			// hkbModifier interface implementation.
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

	public:

		///////////////
		// properties
		///////////////

			/// The character's up-vector is aligned with this vector until m_alignWithGroundDuration elapses, 
			/// after which it is aligned with the world up-vector.
		hkVector4 m_groundNormal;	//+default(0,0,0,0)
									//+hk.Description("The character's up-vector is aligned with this vector until m_alignWithGroundTime elapses or m_alignWithWorldEventId is received, after which it is aligned with the world up-vector.")

			/// Duration for aligning the character's up-vector.
		hkReal m_duration;	//+default(1.0)
							//+hkb.RoleAttribute("ROLE_TIME","FLAG_NONE")
							//+hk.RangeReal(absmin=0.0,absmax=100.0,softmax=10)
							//+hk.Description("The time during which to get up.")

			/// Duration for which the character's up-vector is aligned with m_groundNormal, 
			/// after which it is aligned with the world up-vector.
		hkReal m_alignWithGroundDuration;	//+default(0.0)
											//+hkb.RoleAttribute("ROLE_TIME","FLAG_NONE")
											//+hk.RangeReal(absmin=0.0,absmax=1000.0,softmax=10)
											//+hk.Description("Duration for which the character's up-vector is aligned with m_groundNormal, after which it is aligned with the world up-vector.")

			/// The root (ragdoll) bone used for pose matching.  If this is -1, the index is taken from the character's hkbBoneInfo.
		hkInt16 m_rootBoneIndex;	//+default(-1)
									//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_RAGDOLL")
									//+hk.Description("The root bone used for pose matching.  Pose matching uses three bone from the ragdoll skeleton which are assumed to be representative of the pose.  The L-shaped skeleton between these three bones is matched to determine similar poses.")

			/// A second (ragdoll) bone used for pose matching.  If this is -1, the index is taken from the character's hkbBoneInfo.
		hkInt16 m_otherBoneIndex;	//+default(-1)
									//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_RAGDOLL")
									//+hk.Description("A second bone used for pose matching.")

			/// A third (ragdoll) bone used for pose matching.  If this is -1, the index is taken from the character's hkbBoneInfo.
		hkInt16 m_anotherBoneIndex;	//+default(-1)
									//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_RAGDOLL")
									//+hk.Description("A third bone used for pose matching.")

	private:

		HKB_BEGIN_INTERNAL_STATE(0);

			// Time since the getup modifier begin method was called.
		hkReal m_timeSinceBegin; //+nosave

			// Time slice information - used by the modify() method.
		hkReal m_timeStep;	//+nosave

			// Indicates whether to compute world from model next modify.     
		hkBool m_initNextModify; //+nosave

		HKB_END_INTERNAL_STATE();

	public:

		hkbGetUpModifier( hkFinishLoadedObjectFlag flag );

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
