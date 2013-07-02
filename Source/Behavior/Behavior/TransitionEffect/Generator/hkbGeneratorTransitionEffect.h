/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_GENERATOR_TRANSITION_EFFECT_H
#define HKB_GENERATOR_TRANSITION_EFFECT_H

#include <Behavior/Behavior/TransitionEffect/hkbTransitionEffect.h>

extern const class hkClass hkbGeneratorTransitionEffectClass;

/// A transition effect that plays a generator during a transition. This also allows to blend in
/// and blend out the to and from generators optionally.
class hkbGeneratorTransitionEffect : public hkbTransitionEffect
{
	public:

		// +version(3)

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

			/// Construct with defaults.
		hkbGeneratorTransitionEffect();

	protected:

		hkbGeneratorTransitionEffect( const hkbGeneratorTransitionEffect& gte );

	public:

		~hkbGeneratorTransitionEffect();

			/// Set the generator to play during the transition.
		void setTransitionGenerator( hkbGenerator* transitionGen );

		hkbGenerator* getTransitionGenerator() const;

		//////////////////////
		// hkbNode interface 
		//////////////////////

		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

		virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

		virtual void deactivate( const hkbContext& context ) HK_OVERRIDE;

		virtual int getMaxNumChildren( GetChildrenFlags flags ) HK_OVERRIDE;

		virtual void getChildren( GetChildrenFlags flags, ChildrenInfo& childrenInfo ) HK_OVERRIDE;

		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		////////////////////////////
		// hkbGenerator interface
		////////////////////////////

		virtual void generate(	const hkbContext& context,
								const hkbGeneratorOutput** activeChildrenOutput,
								hkbGeneratorOutput& output,
								hkReal timeOffset = 0.0f ) const HK_OVERRIDE;

		virtual void updateSync( const hkbContext& context ) HK_OVERRIDE;

		//////////////////////////////////
		// hkbTransitionEffect interface
		//////////////////////////////////

		virtual bool isDone() HK_OVERRIDE;

		virtual void setFromGenerator( hkbGenerator* fromGen ) HK_OVERRIDE;

		virtual void setToGenerator( hkbGenerator* toGen ) HK_OVERRIDE;

		virtual hkReal getFromGeneratorBlendOutTime() HK_OVERRIDE;

		virtual hkReal getToGeneratorBlendInTime() HK_OVERRIDE;

		const hkbGenerator* getFromGenerator() const;

		const hkbGenerator* getToGenerator() const;

	private:

		void updateInternal( hkbBehaviorGraph& rootBehaviorGraph );

	public:

		///////////////
		// properties
		///////////////

			// The generator that is used during the transition.
		hkRefPtr<hkbGenerator> m_transitionGenerator;	//+hk.Description("The generator to use while transitioning.")

			/// The duration for which the m_fromGenerator blends with m_transitionGenerator.
		hkReal m_blendInDuration;	//+default(0.0f)
									//+hkb.RoleAttribute("ROLE_TIME","FLAG_NONE")
									//+hk.Description("The duration for which the from-generator blends with m_transitionGenerator.")

			/// The duration for which the m_transitionGenerator blends with m_toGenerator.
		hkReal m_blendOutDuration;	//+default(0.0f)
									//+hkb.RoleAttribute("ROLE_TIME","FLAG_NONE")
									//+hk.Description("The duration for which the m_transitionGenerator blends with the to-generator.")

			/// When this is true the to-generator, if cyclic, is started m_blendOutDuration seconds before its end so that it reaches time zero when the transition ends.
		hkBool m_syncToGeneratorStartTime;	//+default(true)
											//+hk.Description("When this is true the to-generator,if cyclic, is started m_blendOutDuration seconds before its end so that it reaches time zero when the transition ends.")

	protected:

			// The to-generator goes through various states in order to get activated.
		enum ToGeneratorState
		{
				// Waiting for time to pass
			STATE_INACTIVE,

				// It is time to activate, but first we need to get it set up properly.
			STATE_READY_FOR_SET_LOCAL_TIME,

				// Setup is done, but we still need to apply the self-transition mode
			STATE_READY_FOR_APPLY_SELF_TRANSITION_MODE,

				// Fully active.
			STATE_ACTIVE
		};

		enum Stage
		{
				// We are blending from the from-generator to the transition generator.
			STAGE_BLENDING_IN,

				// We are playing the transition generator.
			STAGE_PLAYING_TRANSITION_GENERATOR,

				// We are blending from the transition generator to the to-generator.
			STAGE_BLENDING_OUT,
		};

		enum ChildState
		{
				/// The child represents the from generator
			CHILD_FROM_GENERATOR,

				/// The child represents the transition generator
			CHILD_TRANSITION_GENERATOR,

				/// The child represents the to generator
			CHILD_TO_GENERATOR,
		
				/// There is no child
			CHILD_NONE,
		};

		hkEnum< hkbGeneratorTransitionEffect::ChildState, hkInt8 >	m_childState[2];	//+nosave

			// The generator that is being transformed from (not reference counted)
		hkbGenerator* m_fromGenerator;	//+nosave

			// The generator that is being transitioned to (not reference counted)
		hkbGenerator* m_toGenerator; //+nosave

		HKB_BEGIN_INTERNAL_STATE(0);

			// The number of seconds the transition has been active.
		hkReal m_timeInTransition; //+nosave

			// The duration of the transition.
		hkReal m_duration; //+nosave

			// The current blendInDuration, as adjusted for any changes in the duration of the transition generator.
		hkReal m_effectiveBlendInDuration;	//+nosave

			// The current blendOutDuration, as adjusted for any changes in the duration of the transition generator.
		hkReal m_effectiveBlendOutDuration;	//+nosave

			// The state of the to-generator regarding being activated.
		hkEnum<hkbGeneratorTransitionEffect::ToGeneratorState,hkInt8> m_toGeneratorState; //+nosave

			// Whether to start an echo of the transition generator when updateSync() is called.
		hkBool m_echoTransitionGenerator; //+nosave

			// Whether to start an echo of the to-generator when updateSync() is called.
		hkBool m_echoToGenerator;	//+nosave

			// Set to true by activate() and false by updateSync().
		hkBool m_justActivated; //+nosave

			// This is used to set hkbContext::m_updateActiveNodes during update().
		hkBool m_updateActiveNodes;	//+nosave

			// The stage the generator transition effect is in.
		hkEnum<hkbGeneratorTransitionEffect::Stage, hkInt8> m_stage; //+nosave

		HKB_END_INTERNAL_STATE();

			// Compute the stage of the transition we are in (see above).
		void computeStage();

	public:

		hkbGeneratorTransitionEffect( hkFinishLoadedObjectFlag flag );

		friend class hkbGeneratorTransitionEffectUtils;
		friend class hkbGeneratorTransitionEffectInternalState;
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
