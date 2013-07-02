/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_BALANCE_MODIFIER_H
#define HKB_BALANCE_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>

extern const class hkClass hkbpBalanceModifierStepInfoClass;

extern const class hkClass hkbpBalanceModifierClass;

/// An example custom modifier for adjusting the pose to be more balanced.
class hkbpBalanceModifier : public hkbModifier
{
	//+version(1)

	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbpBalanceModifier();

	protected:

		hkbpBalanceModifier( const hkbpBalanceModifier& mod );

	public:

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

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

		/// The balance modifier operates by taking a set of steps, each of which performs a closed-form solution
		/// on a single joint.  Each step can either completely balance the character (if possible), or can only
		/// move a fixed fraction toward the optimal configuration.
		class StepInfo
		{
			public:
				//+version(1)

				HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbpBalanceModifier::StepInfo );
				HK_DECLARE_REFLECTION();

				StepInfo() {}

				StepInfo( hkInt16 boneIndex, hkReal fractionOfSolution )
				:	m_boneIndex(boneIndex),
					m_fractionOfSolution(fractionOfSolution)
				{
				}

				/// the bone to pivot at
				hkInt16 m_boneIndex;

				/// scale the angle required for balancing (usually by a fraction > 0 and < 1)
				hkReal m_fractionOfSolution;
		};

		/// whether or not to try to balance if it is impossible
		hkBool m_giveUp;

		/// if the error of center-of-mass is within this range then don't balance
		hkReal m_comDistThreshold;

		/// don't actually output the balanced pose (but do the computation)
		hkBool m_passThrough;

		/// the left foot bone (used to determine the ideal center-of-mass)
		hkInt16 m_ragdollLeftFootBoneIndex;

		/// the right foot bone (used to determine the ideal center-of-mass)
		hkInt16 m_ragdollRightFootBoneIndex;

		/// to balance over the ankle midpoint instead of the foot centroid midpoint, set this > 0 and <= 1
		/// the rational for this is that the ragdoll feet are usually exaggerated in size
		hkReal m_balanceOnAnklesFraction;

		/// the up axis in the world
		///<todo> this should be generalized to be any vector, and gotten from hkbContext
		int m_upAxis;

		/// the amount of time over which to fade in the modifier
		hkReal m_fadeInTime;	//+hkb.RoleAttribute("ROLE_TIME","FLAG_NONE")

		///<todo> make this more general
		/// this is added to the goal com, so you can adjust the point over which the character balances
		hkReal m_comBiasX;

		/// The steps to take during the iteration.
		hkArray< class StepInfo > m_stepInfo;

		static const hkReal DEFAULT_BALANCE_ON_ANKLES_FRACTION;

	private:

		hkReal m_timeLapsed; //+nosave

	public:

		hkbpBalanceModifier( hkFinishLoadedObjectFlag flag );

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
