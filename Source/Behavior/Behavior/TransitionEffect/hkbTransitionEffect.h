/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_TRANSITION_EFFECT_H
#define HKB_TRANSITION_EFFECT_H

#include <Behavior/Behavior/Generator/hkbGenerator.h>

extern const class hkClass hkbTransitionEffectClass;

class hkbContext;
class hkbGeneratorOutput;
class hkbEventQueue;

	/// A transition effect between two generators, for example, to blend them.
	///
	/// An hkbTransitionEffect represents a transition over time between one generator and another.
	/// They are used by hkbStateMachines to generate poses when the state machine
	/// transitions between states.
class hkbTransitionEffect : public hkbGenerator
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbTransitionEffect( hkbNodeType nodeType );

	protected:

		hkbTransitionEffect( const hkbTransitionEffect& te );

	public:

		////////////////////////////////////////
		// hkbBindable virtual methods, overridden to facilitate execution in different behavior graphs
		////////////////////////////////////////

		virtual hkBool32 copyVariablesToMembers( hkbSyncVariableSetup& syncVariableSetup, hkbBehaviorGraph& rootBehavior, bool reverse ) HK_OVERRIDE;

		virtual void copyVariableToEnable( hkbSyncVariableSetup& syncVariableSetup, hkbBehaviorGraph& rootBehavior ) HK_OVERRIDE;

		////////////////////////////////////////
		// hkbTransitionEffect virtual methods
		////////////////////////////////////////

			/// Indicates whether the transition has completed or not.
		virtual bool isDone() = 0;

			/// Sets the generator that is being transitioned from.
		virtual void setFromGenerator( hkbGenerator* fromGen ) = 0;

			/// Sets the generator that is being transitioned to.
		virtual void setToGenerator( hkbGenerator* toGen ) = 0;

			/// Indicates how long the transition effect plans to blend out the from-generator.
			/// 
			/// This is used by the state machine to start transitions just enough in advance of
			/// the end of the from-generator so that it plays until the end during the blend.
		virtual hkReal getFromGeneratorBlendOutTime() = 0;

			/// Indicates how long the transition effect plans to blend in the to-generator.
			///
			/// This is used by applySelfTransitionMode() to decide how long to blend during a self-transition
			/// involving the to-generator.
		virtual hkReal getToGeneratorBlendInTime() = 0;

			/// These modes determine the behavior when the to-generator is already active when the transition begins.
		enum SelfTransitionMode
		{
			/// If the to-generator is cyclic, it will be continued without interruption.  Otherwise, it will be
			/// blended with itself using the echo feature.
			SELF_TRANSITION_MODE_CONTINUE_IF_CYCLIC_BLEND_IF_ACYCLIC = 0,

			/// Continue the to-generator without interruption.
			SELF_TRANSITION_MODE_CONTINUE,

			/// Reset the to-generator to the beginning.
			SELF_TRANSITION_MODE_RESET,

			/// Reset the to-generator to the beginning using the echo feature to blend. 
			SELF_TRANSITION_MODE_BLEND,
		};

		enum EventMode
		{
				/// Apply the event mode from m_defaultEventMode.
			EVENT_MODE_DEFAULT,

				/// Don't ignore the events of either the from-generator or the to-generator.
			EVENT_MODE_PROCESS_ALL,

				/// Ignore all events sent by or to the from-generator.
			EVENT_MODE_IGNORE_FROM_GENERATOR,

				/// Ignore all events sent by or to the to-generator.
			EVENT_MODE_IGNORE_TO_GENERATOR,
		};

			/// What to do if the to-generator is already active when the transition activates it.
		hkEnum< SelfTransitionMode, hkInt8 > m_selfTransitionMode;	//+default(hkbTransitionEffect::SELF_TRANSITION_MODE_CONTINUE_IF_CYCLIC_BLEND_IF_ACYCLIC)
																	//+hk.Description("What to do if the to-generator is already active when the transition activates it.")

			/// How to process the events of the from- and to-generators.
		hkEnum< EventMode, hkInt8 > m_eventMode;	//+default(hkbTransitionEffect::EVENT_MODE_DEFAULT)
													//+hk.Description("How to process the events of the from-generator and to-generator.")

	protected:
		
			/// For internal use only.
			/// Overriding binding information; used when this TE is executed in the context of a global transition into a different behavior graph.
		hkRefPtr<hkReferencedObject> m_patchedBindingInfo; //+nosave

			/// The default event mode to use when the event mode of a transition effect is EVENT_MODE_USE_DEFAULT.
			/// You should not set this to EVENT_MODE_USE_DEFAULT.
			/// Subclasses should set this to hkbProjectData::m_defaultEventMode inside activate().
		hkEnum< hkbTransitionEffect::EventMode, hkInt8 > m_defaultEventMode; //+nosave

			// This should be called by every subclass of hkbTransitionEffect just before activating 
			// the to-generator.  It checks whether the toGenerator is already active and
			// uses m_selfTransitionMode to make a decision about what to do about it.
		void applySelfTransitionMode( hkbBehaviorGraph& rootBehaviorGraph, hkbGenerator* toGenerator );

			// return the event mode, applying the default if appropriate
		int getEventMode();

			// Checks to see if the to-generator is active, and skips over any modifier generators.
		bool needSelfTransition( const hkbContext& context, hkbGenerator* toGenerator );

			/// Function for internal use.
			/// This function prepares m_patchedBindingInfo to redirect copyVariablesToMembers functions to the proper variables.
			/// This is necessary for effects used by global transitions since they are authored in the context of one
			/// behavior graph but are executed in another.
		void computePatchedBindings( const hkbContext& context, const hkbBehaviorGraph& referenceBehavior );

	public:

		hkbTransitionEffect( hkFinishLoadedObjectFlag flag ) : hkbGenerator(flag) {}

	friend class hkbCpuUtils;
	friend class hkbStateMachine;
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
