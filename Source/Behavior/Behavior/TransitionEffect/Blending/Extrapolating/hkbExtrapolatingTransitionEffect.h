/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_EXTRAPOLATING_TRANSITION_EFFECT_H
#define HKB_EXTRAPOLATING_TRANSITION_EFFECT_H

#include <Behavior/Behavior/Generator/hkbGeneratorSyncInfo.h>
#include <Behavior/Behavior/Generator/hkbGeneratorPartitionInfo.h>
#include <Behavior/Behavior/TransitionEffect/Blending/hkbBlendingTransitionEffect.h>

extern const class hkClass hkbExtrapolatingTransitionEffectClass;

class hkaSkeleton;
class hkbGeneratorSyncInfo;
class hkbGeneratorPartitionInfo;
class hkbGeneratorOutput;

/// A transition effect that extrapolates the from-generator instead of playing it.  The from-generator is played for
/// two frames in order to measure it's gradient.  Then it is deactivated and its extrapolation is blended with the to-generator.
class hkbExtrapolatingTransitionEffect : public hkbBlendingTransitionEffect
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

			/// Construct with defaults.
		hkbExtrapolatingTransitionEffect();

			/// Construction given the duration of the blend and optional flags.
		hkbExtrapolatingTransitionEffect( hkReal duration, Flags flags = FLAG_NONE, EndMode endMode = END_MODE_NONE );

	protected:

			// copy constructor for cloning
		hkbExtrapolatingTransitionEffect( const hkbExtrapolatingTransitionEffect& ete );

	public:

		//////////////////////
		// hkbNode interface
		//////////////////////

		// hkbNode interface implementation
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

		// hkbNode interface implementation
		virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

		// hkbNode interface implementation
		virtual void deactivate( const hkbContext& context ) HK_OVERRIDE;

		// hkbNode interface implementation
		virtual void getChildren( GetChildrenFlags flags, ChildrenInfo& childrenInfo ) HK_OVERRIDE;

		// hkbNode interface implementation
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		////////////////////////////
		// hkbGenerator interface
		////////////////////////////
		
		// hkbGenerator interface implementation
		virtual void generate(	const hkbContext& context,
								const hkbGeneratorOutput** activeChildrenOutput,
								hkbGeneratorOutput& output,
								hkReal timeOffset = 0.0f ) const HK_OVERRIDE;

		// hkbGenerator interface implementation
		virtual void computePartitionInfo( const hkArray<hkbNodeChildInfo>& activeChildren, const hkbContext& context ) HK_OVERRIDE;

		// hkbGenerator interface implementation
		virtual bool canRecycleOutput() const HK_OVERRIDE;

		//////////////////////////////////
		// hkbTransitionEffect interface
		//////////////////////////////////

		// hkbTransitionEffect interface implementation
		virtual bool isDone() HK_OVERRIDE;

	protected:

			// Determine whether the node can be synced.
		virtual bool isSyncable( hkbBehaviorGraph& behaviorGraph ) HK_OVERRIDE;

			// Check whether the to-generator has been activated in this frame.
		virtual bool hasToGeneratorBeenActivatedInThisFrame() const HK_OVERRIDE;

			// Determine whether we just need the from-generator to sync
		virtual bool useFromGeneratorToSyncOnly() const HK_OVERRIDE;

			// Get the from-generator sync info.
		virtual const hkbGeneratorSyncInfo& getFromGeneratorSyncInfo( hkbBehaviorGraph& behaviorGraph ) HK_OVERRIDE;

			// Get the duration of the transition.
		virtual hkReal getTransitionDuration() const HK_OVERRIDE;

			// Is this the first frame of the transition.
		virtual bool isFirstFrame() const HK_OVERRIDE;

	private:

		HKB_BEGIN_INTERNAL_STATE(1);

		// from-generator sync info that will be used for syncing once the from-generator is deactivated
		class hkbGeneratorSyncInfo m_fromGeneratorSyncInfo; //+nosave

		// from-generator partition info that will be used for blending poses when the from-generator is deactivated. 
		mutable class hkbGeneratorPartitionInfo m_fromGeneratorPartitionInfo; //+nosave

		// the world-from-model transform that is being extrapolated
		mutable hkQsTransform m_worldFromModel; //+nosave

		// motion that is used for extrapolating m_worldFromModel
		mutable hkQsTransform m_motion; //+nosave

		// the pose as it is being extrapolated
		mutable hkArray<hkQsTransform> m_pose; //+nosave

		// additive pose that is used for extrapolating m_pose
		mutable hkArray<hkQsTransform> m_additivePose; //+nosave

		// the boneweights for the pose that is being extrapolated
		mutable hkArray<hkReal> m_boneWeights; //+nosave

		// the duration for which the to-generator runs during the blend.
		hkReal m_toGeneratorDuration; //+nosave

		// when the from generator frame samples are enough to extrapolate we deactivate the from generator
		// and set this to false
		hkBool m_isFromGeneratorActive; //+nosave

		// Whether we have stored m_pose yet.  This is needed because if there is no pose buffer at all we can't use a
		// size of 0 to distinguish whether we've got the pose yet.
		mutable hkBool m_gotPose; //+nosave

		// Whether we have stored m_additivePose yet.
		mutable hkBool m_gotAdditivePose;	//+nosave

		HKB_END_INTERNAL_STATE();

	public:

		hkbExtrapolatingTransitionEffect( hkFinishLoadedObjectFlag flag ) : hkbBlendingTransitionEffect(flag) {}

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
