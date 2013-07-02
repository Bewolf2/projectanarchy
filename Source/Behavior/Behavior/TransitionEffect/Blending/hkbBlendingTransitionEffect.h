/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_BLENDING_TRANSITION_EFFECT_H
#define HKB_BLENDING_TRANSITION_EFFECT_H

#include <Behavior/Behavior/TransitionEffect/hkbTransitionEffect.h>
#include <Behavior/Behavior/Utils/hkbBlendCurveUtils.h>

class hkbGeneratorSyncInfo;

extern const hkClass hkbBlendingTransitionEffectClass;

/// A transition effect that does a linear blend between two generators.
class hkbBlendingTransitionEffect : public hkbTransitionEffect
{
	public:

		// +version(2)
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

			/// Construct with defaults.
		hkbBlendingTransitionEffect();

	protected:

		hkbBlendingTransitionEffect( const hkbBlendingTransitionEffect& bte );

	public:

			/// Flags to indicate specialized behavior.
		enum FlagBits
		{
			//+defineflags(true)

				/// No flags.
			FLAG_NONE = 0x0,

				/// Just use the worldFromModel of the generator being transitioned to. 
			FLAG_IGNORE_FROM_WORLD_FROM_MODEL = 0x1,

				/// Synchronize the cycles of the children.
			FLAG_SYNC = 0x2,

				/// Just use the worldFromModel of the generator being transitioned from. 
			FLAG_IGNORE_TO_WORLD_FROM_MODEL = 0x4,

				/// Blend the to and from world from models but ignore the to generator's rotation
			FLAG_IGNORE_TO_WORLD_FROM_MODEL_ROTATION = 0x8,
		};

			/// An integer type for storing unions of FlagBTBits.
		typedef hkUint16 Flags;

			/// Modes regarding how to deal with the end of the "from" generator.

		enum EndMode
		{
				/// Don't do anything special regarding the end of the "from" generator.
			END_MODE_NONE = 0,

				/// Ignore m_duration, and instead transition until the end of the "from" generator.
			END_MODE_TRANSITION_UNTIL_END_OF_FROM_GENERATOR = 1,

				/// If the transition begins closer than m_duration to the end of the "from" generator,
				/// shorten the transition duration to the time remaining so that the transition does
				/// not play the "from" generator beyond the end.
			END_MODE_CAP_DURATION_AT_END_OF_FROM_GENERATOR = 2,
		};

			/// Construction given the duration of the blend and optional flags.
		hkbBlendingTransitionEffect( hkReal duration, Flags flags = FLAG_NONE, EndMode endMode = END_MODE_NONE );

		//////////////////////
		// hkbNode interface 
		//////////////////////

		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

		virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

		virtual void deactivate( const hkbContext& context ) HK_OVERRIDE;

		virtual void getChildren( GetChildrenFlags flags, ChildrenInfo& childrenInfo ) HK_OVERRIDE;

		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		////////////////////////////
		// hkbGenerator interface
		////////////////////////////

		virtual void generate(	const hkbContext& context,
								const hkbGeneratorOutput** activeChildrenOutput,
								hkbGeneratorOutput& output,
								hkReal timeOffset = 0.0f ) const HK_OVERRIDE;

		virtual bool canRecycleOutput() const HK_OVERRIDE;

		virtual void updateSync( const hkbContext& context ) HK_OVERRIDE;

		//////////////////////////////////
		// hkbTransitionEffect interface
		//////////////////////////////////

		virtual bool isDone() HK_OVERRIDE;
		
		virtual void setFromGenerator( hkbGenerator* fromGen ) HK_OVERRIDE;

		virtual void setToGenerator( hkbGenerator* toGen ) HK_OVERRIDE;

		virtual hkReal getFromGeneratorBlendOutTime() HK_OVERRIDE;

		virtual hkReal getToGeneratorBlendInTime() HK_OVERRIDE;

	public:

		///////////////
		// properties
		///////////////

			/// The duration of the transition.
		hkReal m_duration;	//+default(0.0f)
							//+hkb.RoleAttribute("ROLE_TIME","FLAG_NONE")
							//+hk.RangeReal(absmin=0,absmax=1000,softmax=10)
							//+hk.Description("The duration of the transition.")

			/// The start time of the to-generator when the transition begins, expressed as a fraction of its duration.
		hkReal m_toGeneratorStartTimeFraction;	//+default(0.0)
												//+hk.RangeReal(absmin=0,absmax=1)
												//+hk.Description("The start time of the to-generator when the transition begins, expressed as a fraction of its duration.")

			/// Flags to indicate specialized behavior.
		hkFlags< FlagBits, hkUint16 > m_flags;	//+default(hkbBlendingTransitionEffect::FLAG_NONE)
												//+hk.Description("Flags for specialized behavior.")

			/// The treatment of the end of the from-generator.
		hkEnum< EndMode, hkInt8 > m_endMode;	//+default(hkbBlendingTransitionEffect::END_MODE_NONE)
												//+hk.Description("The treatment of the end of the from-generator.")

			/// Which blend curve to use.
		hkEnum< hkbBlendCurveUtils::BlendCurve, hkInt8 > m_blendCurve;	//+default(hkbBlendCurveUtils::BLEND_CURVE_SMOOTH)
																		//+hk.Description("Which blend curve to use.")

			/// The bone to align during the blend.  
		hkInt16 m_alignmentBone;	//+default(-1)
									//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_NONE")
									//+hk.Description("The bone to align during the blend.")

	protected:

			// Recompute the time remaining for the transition.
		void updateTimeRemaining( hkReal transitionDuration, const hkbGeneratorSyncInfo& syncInfo );

			// Determine whether the node can be synced.
		virtual bool isSyncable( hkbBehaviorGraph& behaviorGraph );

			// Check whether the to-generator has been activated in this frame.
		virtual bool hasToGeneratorBeenActivatedInThisFrame() const;

			// Determine whether we just need the from-generator to sync
		virtual bool useFromGeneratorToSyncOnly() const;

			// Get the from generator sync info.
		virtual const hkbGeneratorSyncInfo& getFromGeneratorSyncInfo( hkbBehaviorGraph& behaviorGraph );

			// Get the duration of the transition.
		virtual hkReal getTransitionDuration() const;

			// Is this the first frame of the transition.
		virtual bool isFirstFrame() const;

		HKB_BEGIN_INTERNAL_STATE(0);

			// The most recent world-from-model for the from-generator (used when m_alignmentBone is set).
		mutable hkVector4 m_fromPos; //+nosave
		mutable hkQuaternion m_fromRot; //+nosave
			// The most recent world-from-model for the to-generator (used when m_alignmentBone is set).
		mutable hkVector4 m_toPos; //+nosave
		mutable hkQuaternion m_toRot; //+nosave
			// The last world-from-model we output.
		mutable hkVector4 m_lastPos; //+nosave
		mutable hkQuaternion m_lastRot; //+nosave			

			// The pose of the character when the transition begins.
		hkArray<hkQsTransform> m_characterPoseAtBeginningOfTransition; //+nosave

			// the remaining time in the transition 
		hkReal m_timeRemaining; //+nosave

			// The number of seconds the transition has been active.
		hkReal m_timeInTransition; //+nosave

			// Whether to apply self-transition on the next frame.
		hkBool m_applySelfTransition; //+nosave

			// Initialize the character pose.
		mutable hkBool m_initializeCharacterPose; //+nosave

			// Do alignment at the next opportunity.
		mutable hkBool m_alignThisFrame; //+nosave

			// set to true inside the generator when the alignment is finished
		mutable hkBool m_alignmentFinished; //+nosave

		HKB_END_INTERNAL_STATE();

			// The generator that is being transitioned from (not reference counted)
		hkbGenerator* m_fromGenerator; //+nosave

			// The generator that is being transitioned to (not reference counted)
		hkbGenerator* m_toGenerator; //+nosave

	public:
		
		hkbBlendingTransitionEffect( hkFinishLoadedObjectFlag flag );

		friend class hkbBlendingTransitionEffectUtils;

#if defined (HK_PLATFORM_HAS_SPU)
		friend class hkbSpuBehaviorUtils;
#endif
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
