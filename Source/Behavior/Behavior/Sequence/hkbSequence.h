/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_SEQUENCE_H
#define HKB_SEQUENCE_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>
#include <Behavior/Behavior/Linker/hkbSymbolIdMap.h>
#include <Behavior/Behavior/Utils/hkbUtils.h>
#include <Behavior/Behavior/Sequence/hkbSequenceStringData.h>

extern const class hkClass hkbSequenceClass;

class hkbContext;
class hkbEventQueue;
class hkbSymbolIdMap;
class hkbSequenceStringData;
class hkbEventSequencedData;
class hkbRealVariableSequencedData;
class hkbBoolVariableSequencedData;
class hkbIntVariableSequencedData;

	/// A sequence defines variable values and events as a function of time.
	///
	/// When you update the sequence, it generates the events that it has stored on the timeline,
	/// and sets variables according to stored function curves (piecewise linear).
	/// A sequence is a modifier, so you can place it in a behavior graph if you want.
class hkbSequence : public hkbModifier
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbSequence();

	protected:

		hkbSequence( const hkbSequence& seq );

	public:

		~hkbSequence();

		//////////////////////
		// hkbNode interface
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void handleEvent( const hkbContext& context, const hkbEvent e ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

	public:

			/// Returns the string data associated with this sequence, or HK_NULL if there is none.
		hkbSequenceStringData* getStringData();

			/// Sets the string data associated with this sequence (can be HK_NULL).
		void setStringData( hkbSequenceStringData* stringData );

			/// Returns the map between the sequence's variable IDs and those of the
			/// external context.
		hkbSymbolIdMap* getVariableIdMap() const;

			/// Set the map between the sequence's variable IDs and those of the
			/// external context (usually an hkbBehaviorGraph) to which the sequence is being applied.
		void setVariableIdMap( hkbSymbolIdMap* variableIdMap );

			/// Returns the map between the sequence's event IDs and those of the
			/// external context.
		hkbSymbolIdMap* getEventIdMap() const;

			/// Set the map between the sequence's event IDs and those of the
			/// external context (usually an hkbBehaviorGraph) to which the sequence is being applied.
		void setEventIdMap( hkbSymbolIdMap* eventIdMap );

	public:

			/// The data sequences that make up this sequence.
			///
			/// This is public, so be sure to add an extra reference count.
			/// When the sequence is destroyed, it will decrement the reference
			/// count on each of these.
		hkArray<hkbEventSequencedData*> m_eventSequencedData;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_VARIABLE")
																//+hk.Description("Sequenced events.")

		hkArray<hkbRealVariableSequencedData*> m_realVariableSequencedData;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_VARIABLE")
																			//+hk.Description("Sequenced real variables.")

		hkArray<hkbBoolVariableSequencedData*> m_boolVariableSequencedData;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_VARIABLE")
																			//+hk.Description("Sequenced bool variables.")

		hkArray<hkbIntVariableSequencedData*> m_intVariableSequencedData;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_VARIABLE")
																			//+hk.Description("Sequenced int variables.")

			/// When this event is received the sequence is enabled.
		hkInt32 m_enableEventId; //+default(hkbEvent::EVENT_ID_NULL)+hkb.RoleAttribute("ROLE_EVENT_ID","FLAG_NONE")+hk.Description("When this event is received the sequence is enabled.  If no event is specified, the sequence is enabled when activated.")

			/// When this event is received the sequence is disabled.
		hkInt32 m_disableEventId; //+default(hkbEvent::EVENT_ID_NULL)+hkb.RoleAttribute("ROLE_EVENT_ID","FLAG_NONE")+hk.Description("When this event is received the sequence is disabled.")

	private:

			// String data used for linking symbols between a sequence and a behavior.
		hkRefPtr<hkbSequenceStringData> m_stringData;

			/// The map between the sequence's variable IDs and those of the
			/// external context (usually an hkbBehaviorGraph) to which the sequence is being applied.
		hkRefPtr<hkbSymbolIdMap> m_variableIdMap; //+nosave

			/// The map between the sequence's event IDs and those of the
			/// external context (usually an hkbBehaviorGraph) to which the sequence is being applied.
		hkRefPtr<hkbSymbolIdMap> m_eventIdMap; //+nosave

		HKB_BEGIN_INTERNAL_STATE(0);

		hkArray<hkInt32> m_nextSampleEvents;	//+nosave
		hkArray<hkInt32> m_nextSampleReals;		//+nosave
		hkArray<hkInt32> m_nextSampleBools;		//+nosave
		hkArray<hkInt32> m_nextSampleInts;		//+nosave

			// The current time.
		hkReal m_time; //+nosave

			// This tells whether the sequence is enabled or not
		hkBool m_isEnabled; //+nosave

		HKB_END_INTERNAL_STATE();

	public:

		hkbSequence( hkFinishLoadedObjectFlag flag );
};

#include <Behavior/Behavior/Sequence/hkbSequence.inl>

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
