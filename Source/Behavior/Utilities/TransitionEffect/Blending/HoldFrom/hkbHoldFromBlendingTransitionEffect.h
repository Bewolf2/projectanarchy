/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_HOLD_FROM_BLENDING_TRANSITION_H
#define HKB_HOLD_FROM_BLENDING_TRANSITION_H

#include <Behavior/Behavior/TransitionEffect/Blending/hkbBlendingTransitionEffect.h>

extern const class hkClass hkbHoldFromBlendingTransitionEffectClass;

class hkaSkeleton;

/// An example custom transition effect that holds the original pose of the from-generator
/// while blending with the to-generator.
class hkbHoldFromBlendingTransitionEffect : public hkbBlendingTransitionEffect
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

			/// Construction given the duration of the blend and optional flags.
		hkbHoldFromBlendingTransitionEffect();

	protected:

		hkbHoldFromBlendingTransitionEffect( const hkbHoldFromBlendingTransitionEffect& hfbte );

	public:

		//////////////////////
		// hkbNode interface
		//////////////////////

		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

		virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

		virtual void deactivate( const hkbContext& context ) HK_OVERRIDE;

		virtual void getChildren( GetChildrenFlags flags, ChildrenInfo& childrenInfo ) HK_OVERRIDE;

		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		///////////////////////////
		// hkbGenerator interface
		///////////////////////////

		virtual void generate(	const hkbContext& context,
								const hkbGeneratorOutput** activeChildrenOutput,
								hkbGeneratorOutput& output,
								hkReal timeOffset = 0.0f ) const HK_OVERRIDE;

		virtual void updateSync( const hkbContext& context ) HK_OVERRIDE;

		virtual void computePartitionInfo( const hkArray<hkbNodeChildInfo>& activeChildren, const hkbContext& context ) HK_OVERRIDE;

	private:

		// these are mutable because I need to set them during generate
		mutable hkQsTransform* m_heldFromPose; //+nosave+default(HK_NULL)
		mutable int m_heldFromPoseSize; //+nosave+default(0)
		mutable hkQsTransform m_heldWorldFromModel; //+nosave
		mutable const hkaSkeleton* m_heldFromSkeleton; //+nosave+default(HK_NULL)

	public:

		hkbHoldFromBlendingTransitionEffect( hkFinishLoadedObjectFlag flag ) : hkbBlendingTransitionEffect(flag) {}

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
