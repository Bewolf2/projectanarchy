/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_STATE_DEPENDENT_MODIFIER_H
#define HKB_STATE_DEPENDENT_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>

extern const class hkClass hkbStateDependentModifierClass;

class hkbStateMachine;

/// An example custom modifier that applies another modifier depending on the state of a state machine.
class hkbStateDependentModifier : public hkbModifier
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbStateDependentModifier();

	protected:

		hkbStateDependentModifier( const hkbStateDependentModifier& mod );

	public:

		//////////////////////
		// hkbNode interface 
		//////////////////////

		virtual int getMaxNumChildren( GetChildrenFlags flags ) HK_OVERRIDE;
		virtual void getChildren( GetChildrenFlags flags, ChildrenInfo& childrenInfo ) HK_OVERRIDE;
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		/////////////////////////
		// hkbModifier interface
		/////////////////////////

		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

		virtual void predictiveModify( const hkbContext& context, hkbGeneratorOutput& inOut, hkReal timeOffset ) HK_OVERRIDE;

		////////////////////////////////////////
		// hkbStateDependentModifier functions
		////////////////////////////////////////

			/// Set the modifier to be applied.
		void setModifier( hkbModifier* modifier );

			/// Set the state machine to check the state of.
		void setStateMachine( hkbStateMachine* stateMachine );

			/// Adds a state in which the modifiers will be applied.
		void addStateId( int stateId );

		////////////////
		// properties
		////////////////

	public:

			/// whether or not to apply the modifiers during state transitions
		hkBool m_applyModifierDuringTransition;

	private:

			// the states in which the modifier will be applied
		hkArray<int> m_stateIds;

			// the modifier that needs to be applied if the state machine is in any of the states specified in m_states
		hkRefPtr<hkbModifier> m_modifier;

			// whether the modifier is active or not
		hkBool m_isActive;

			// states of this state machine will be checked for applying the modifiers
		hkbStateMachine* m_stateMachine;

	public:

		hkbStateDependentModifier( hkFinishLoadedObjectFlag flag );
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
