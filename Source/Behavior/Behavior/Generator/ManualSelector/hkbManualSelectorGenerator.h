/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_MANUAL_SELECTOR_GENERATOR_H
#define HKB_MANUAL_SELECTOR_GENERATOR_H

#include <Behavior/Behavior/Generator/hkbGenerator.h>
#include <Behavior/Behavior/StateMachine/hkbStateMachine.h>

extern const class hkClass hkbManualSelectorGeneratorClass;

/// An example custom generator for selecting among a set of child generators.
class hkbManualSelectorGenerator : public hkbGenerator
{
	public:

		//+version(2)
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbManualSelectorGenerator();

	protected:

		hkbManualSelectorGenerator( const hkbManualSelectorGenerator& gen );

	public:

		~hkbManualSelectorGenerator();

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual int getMaxNumChildren( GetChildrenFlags flags ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void getChildren( GetChildrenFlags flags, ChildrenInfo& childrenInfo ) HK_OVERRIDE;
			
			// hkbNode interface implementation.
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void getInternalStateUser(	const hkbBehaviorGraph& rootBehavior, hkReferencedObject& _internalState ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void setInternalStateUser(	const hkbContext& context, const hkReferencedObject& _internalState, hkPointerMap<hkInt16,const hkbNodeInternalStateInfo*>& nodeIdToInternalState ) HK_OVERRIDE;

		///////////////////////////
		// hkbGenerator interface
		///////////////////////////

			// hkbGenerator interface implementation.
		virtual void generate(	const hkbContext& context, 
								const hkbGeneratorOutput** activeChildrenOutput,
								hkbGeneratorOutput& output,
								hkReal timeOffset = 0.0f ) const HK_OVERRIDE;

			// hkbGenerator interface implementation.
		virtual bool canRecycleOutput() const HK_OVERRIDE;

			// hkbGenerator interface implementation.
		virtual void updateSync( const hkbContext& context ) HK_OVERRIDE;

	public:

			/// set a generator to select
		void setGenerator( int index, hkbGenerator* generator );

	public:

			/// child generators to select
		hkArray<hkbGenerator*> m_generators; //+hk.Description("The child generators that can be selected.")

			/// use this to select a generator from the above list
		hkInt8 m_selectedGeneratorIndex; //+default(0)+hk.Description("The index of the selected child generator.")

			/// use to dictate if the generator can change the active subgraph after activation
		hkBool m_selectedIndexCanChangeAfterActivate; //+default(true)+hk.Description("If true the selected index can change anytime.  If false it can only change during activation.  To reset the node must be reset.")

			/// template transition effect used when changing selected generators.
		hkbTransitionEffect* m_generatorChangedTransitionEffect; //+default(HK_NULL)+hk.Description("An optional transition effect used when changing active generators.")

	private:

			// return the above member, with bounds checking
		hkInt8 getSelectedGeneratorIndex();

		HKB_BEGIN_INTERNAL_STATE(0);

			// the current generator that has been selected
		hkInt8 m_currentGeneratorIndex; 

			// the index of the generator at activation time
		hkInt8 m_generatorIndexAtActivate;

			// instances of active transitions
		hkArray< hkbStateMachine::ActiveTransitionInfo > m_activeTransitions; //+nosave

		HKB_END_INTERNAL_STATE();

	public:

		hkbManualSelectorGenerator( hkFinishLoadedObjectFlag flag );
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
