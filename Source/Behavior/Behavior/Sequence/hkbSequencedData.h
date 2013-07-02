/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_SEQUENCED_DATA_H
#define HKB_SEQUENCED_DATA_H

#include <Behavior/Behavior/Event/hkbEvent.h>

extern const class hkClass hkbBoolVariableSequencedDataSampleClass;
extern const class hkClass hkbBoolVariableSequencedDataClass;
extern const class hkClass hkbRealVariableSequencedDataSampleClass;
extern const class hkClass hkbRealVariableSequencedDataClass;
extern const class hkClass hkbEventSequencedDataSequencedEventClass;
extern const class hkClass hkbEventSequencedDataClass;
extern const class hkClass hkbSequencedDataClass;
extern const class hkClass hkbIntVariableSequencedDataSampleClass;
extern const class hkClass hkbIntVariableSequencedDataClass;

class hkbContext;
class hkbEventQueue;
class hkbSequence;
class hkbSymbolIdMap;

	/// A virtual base class for sequenced data.
class hkbSequencedData : public hkReferencedObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbSequencedData() {}

		hkbSequencedData( const hkbSequencedData& data ) : hkReferencedObject(data) {}

			/// Update the sequenced data to a new time, and generate events.
		virtual void update(	hkbSequence* sequence,
								const hkbContext& context, 
								hkReal time,
								hkbEventQueue& eventQueue,
								hkbSymbolIdMap* variableIdMap,
								hkbSymbolIdMap* eventIdMap,
								int& nextSampleInOut ) = 0;

	public:

		hkbSequencedData( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag) {}
};

	/// A sequence of events on a timeline.
class hkbEventSequencedData : public hkbSequencedData
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbEventSequencedData();

		hkbEventSequencedData( const hkbEventSequencedData& data );

		///////////////////////////////
		// hkbSequencedData interface 
		///////////////////////////////

			// hkbSequencedData interface implementation.
		virtual void update( hkbSequence* sequence, const hkbContext& context, hkReal time, hkbEventQueue& eventQueue, hkbSymbolIdMap* variableIdMap, hkbSymbolIdMap* eventIdMap, int& nextSampleInOut ) HK_OVERRIDE;

	public:

			/// A single sequenced event.
		struct SequencedEvent
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbEventSequencedData::SequencedEvent );
			HK_DECLARE_REFLECTION();

			SequencedEvent() {}

			SequencedEvent( hkReal time, const hkbEvent& e )
			:	m_event(e),
				m_time(time) {}

			SequencedEvent(hkFinishLoadedObjectFlag f) : m_event(f) {}

				/// The event.
			class hkbEvent m_event; //+hk.Description("The event to send.")

				/// The time at which the event should be sent.
			hkReal m_time;	//+default(0.0f)
							//+hkb.RoleAttribute("ROLE_TIME","FLAG_NONE")
							//+hk.RangeReal(absmin=0,absmax=1000)
							//+hk.Description("The time at which to send an event.")
		};

			/// The list of sequenced events.
		hkArray<struct SequencedEvent> m_events; //+hk.Description("The events and their times.")

	public:

		hkbEventSequencedData( hkFinishLoadedObjectFlag flag ) : hkbSequencedData(flag), m_events(flag) {}

};

	/// Sequenced data for an hkReal variable.
class hkbRealVariableSequencedData : public hkbSequencedData
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbRealVariableSequencedData();

		hkbRealVariableSequencedData( const hkbRealVariableSequencedData& data );

		///////////////////////////////
		// hkbSequencedData interface 
		///////////////////////////////

			// hkbSequencedData interface implementation.
		virtual void update( hkbSequence* sequence, const hkbContext& context, hkReal time, hkbEventQueue& eventQueue, hkbSymbolIdMap* variableIdMap, hkbSymbolIdMap* eventIdMap, int& nextSampleInOut ) HK_OVERRIDE;

	public:

			/// A sample of the function from time to the variable.
		struct Sample
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbRealVariableSequencedData::Sample );
			HK_DECLARE_REFLECTION();

			Sample() {}

			Sample( hkReal time, hkReal value )
			:	m_time(time),
				m_value(value) {}

				/// The time of this sample.
			hkReal m_time;	//+default(0.0f)
							//+hkb.RoleAttribute("ROLE_TIME","FLAG_NONE")
							//+hk.RangeReal(absmin=0,absmax=1000,softmax=10)
							//+hk.Description("The time of this sample.")

				/// The value of the function at this time.
			hkReal m_value; //+default(0.0f)+hk.Description("The value of the variable at this time.")

			Sample( hkFinishLoadedObjectFlag ) {}
		};

			/// The samples that make up the variable function.
		hkArray<struct Sample> m_samples; //+hk.Description("The time-value pairs that make up the sequence.")

			/// The index of the variable that is being sequenced.
		hkInt32 m_variableIndex; //+default(-1)+hkb.RoleAttribute("ROLE_VARIABLE_INDEX","FLAG_NONE")+hk.Description("The variable that is being sequenced.")

	public:

		hkbRealVariableSequencedData( hkFinishLoadedObjectFlag flag ) : hkbSequencedData(flag), m_samples(flag) {}

};

	/// Sequenced data for an hkBool variable.
class hkbBoolVariableSequencedData : public hkbSequencedData
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbBoolVariableSequencedData();

		hkbBoolVariableSequencedData( const hkbBoolVariableSequencedData& data );

		///////////////////////////////
		// hkbSequencedData interface 
		///////////////////////////////

			// hkbSequencedData interface implementation.
		virtual void update( hkbSequence* sequence, const hkbContext& context, hkReal time, hkbEventQueue& eventQueue, hkbSymbolIdMap* variableIdMap, hkbSymbolIdMap* eventIdMap, int& nextSampleInOut ) HK_OVERRIDE;

	public:

			/// A sample of the function from time to the variable.
		struct Sample
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbBoolVariableSequencedData::Sample );
			HK_DECLARE_REFLECTION();

			Sample() {}

			Sample( hkReal time, hkBool value )
			:	m_time(time),
				m_value(value) {}

				/// The time of this sample.
			hkReal m_time;	//+default(0.0f)
							//+hkb.RoleAttribute("ROLE_TIME","FLAG_NONE")
							//+hk.RangeReal(absmin=0,absmax=1000,softmax=10)
							//+hk.Description("The time of this sample.")

				/// The value of the function at this time.
			hkBool m_value; //+default(0.0f)+hk.Description("The value of the variable at this time.")
		};

			/// The samples that make up the variable function.
		hkArray<struct Sample> m_samples; //+hk.Description("The time-value pairs that make up the sequence.")

			/// The index of the variable that is being sequenced.
		hkInt32 m_variableIndex; //+default(-1)+hkb.RoleAttribute("ROLE_VARIABLE_INDEX","FLAG_NONE")+hk.Description("The variable that is being sequenced.")

	public:

		hkbBoolVariableSequencedData( hkFinishLoadedObjectFlag flag ) : hkbSequencedData(flag), m_samples(flag) {}

};

/// Sequenced data for an integer variable.
class hkbIntVariableSequencedData : public hkbSequencedData
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbIntVariableSequencedData();

		hkbIntVariableSequencedData( const hkbIntVariableSequencedData& data );

		///////////////////////////////
		// hkbSequencedData interface 
		///////////////////////////////

		// hkbSequencedData interface implementation.
		virtual void update( hkbSequence* sequence, const hkbContext& context, hkReal time, hkbEventQueue& eventQueue, hkbSymbolIdMap* variableIdMap, hkbSymbolIdMap* eventIdMap, int& nextSampleInOut ) HK_OVERRIDE;

	public:

		/// A sample of the function from time to the variable.
		struct Sample
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbIntVariableSequencedData::Sample );
			HK_DECLARE_REFLECTION();

			Sample() {}

			Sample( hkReal time, hkInt32 value )
				:	m_time(time),
					m_value(value) {}

			/// The time of this sample.
			hkReal m_time;	//+default(0.0f)
							//+hkb.RoleAttribute("ROLE_TIME","FLAG_NONE")
							//+hk.RangeReal(absmin=0,absmax=1000,softmax=10)
							//+hk.Description("The time of this sample.")

			/// The value of the function at this time.
			hkInt32 m_value; //+default(0.0f)+hk.Description("The value of the variable at this time.")
		};

		/// The samples that make up the variable function.
		hkArray<struct Sample> m_samples; //+hk.Description("The time-value pairs that make up the sequence.")

		/// The index of the variable that is being sequenced.
		hkInt32 m_variableIndex; //+default(-1)+hkb.RoleAttribute("ROLE_VARIABLE_INDEX","FLAG_NONE")+hk.Description("The variable that is being sequenced.")

	public:

		hkbIntVariableSequencedData( hkFinishLoadedObjectFlag flag ) : hkbSequencedData(flag), m_samples(flag) {}

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
