/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_EVENTS_FROM_RANGE_MODIFIER_H
#define HKB_EVENTS_FROM_RANGE_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>
#include <Common/Base/Container/String/hkString.h>

extern const class hkClass hkbEventRangeDataArrayClass;

extern const class hkClass hkbEventRangeDataClass;

extern const class hkClass hkbEventsFromRangeModifierClass;

	/// A range, an event and the event mode.
struct hkbEventRangeData
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbEventRangeData );
	HK_DECLARE_REFLECTION();

	hkbEventRangeData()
	:	m_upperBound(0.0f), 
		m_event(hkbEvent::EVENT_ID_NULL),
		m_eventMode(EVENT_MODE_SEND_ON_ENTER_RANGE)
	{} 

	enum EventRangeMode
	{
			/// Send the event every frame in which the range is entered after having 
			/// been outside the range on the previous frame.
		EVENT_MODE_SEND_ON_ENTER_RANGE,

			/// Send the event every frame if the value is in the range.
		EVENT_MODE_SEND_WHEN_IN_RANGE,
	};

		/// The highest value in this range.  The lowest value of this range is the upperBound from the previous range.
	hkReal m_upperBound;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_VARIABLE")
							//+hk.Description("The highest value in this range.  The lowest value of this range is the upperBound from the previous range.")

		/// The event to send when this range is entered.
	class hkbEventProperty m_event;	//+hk.Description("The event to send when this range is entered.")
	
		/// Under what circumstances to send the event.
	hkEnum< EventRangeMode, hkInt8> m_eventMode;	//+default(hkbEventRangeData::EVENT_MODE_SEND_ON_ENTER_RANGE)
													//+hk.Description("Under what circumstances to send the event.")

		/// Finish constructor (internal).
	hkbEventRangeData( hkFinishLoadedObjectFlag flag ) : m_event(flag) {}
};

	/// An array of expressions wrapped for shared access.
class hkbEventRangeDataArray : public hkReferencedObject
{
		//+vtable(1)
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbEventRangeDataArray() {}

		hkbEventRangeDataArray( const hkbEventRangeDataArray& dataArray );

			/// A series of intervals, each of which has an event associated with it. Note that these must be in increasing order.
		hkArray<struct hkbEventRangeData> m_eventData; //+hk.Description("A series of intervals, each of which has an event associated with it. Note that these must be in increasing order.")

	public:

		hkbEventRangeDataArray( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag), m_eventData(flag) {}
};


	/// A modifier for firing events, given a variable mapped onto a set of ranges.
class hkbEventsFromRangeModifier : public hkbModifier
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbEventsFromRangeModifier();

	private:

		hkbEventsFromRangeModifier( const hkbEventsFromRangeModifier& eventsFromRangeModifier );

	public:

		~hkbEventsFromRangeModifier();

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		///////////////
		// properties
		///////////////

			/// The value that is checked against the ranges to decide which events should be sent.
		hkReal m_inputValue;	//+hk.Description("The value that is checked against the ranges to decide which events should be sent.")

			/// A lower bound of all of the range intervals.
		hkReal m_lowerBound;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_VARIABLE")
								//+hk.Description("A lower bound of all of the range intervals.")			

			/// A series of intervals, each of which has an event associated with it. Note that these must be in increasing order.
		hkRefPtr<hkbEventRangeDataArray> m_eventRanges;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_VARIABLE")
														//+hk.Description("A series of intervals, each of which has an event associated with it. Note that these must be in increasing order.")

	private:

		HKB_BEGIN_INTERNAL_STATE(0);

			// Was the range active in the previous frame. 
		hkArray<hkBool> m_wasActiveInPreviousFrame; //+nosave

		HKB_END_INTERNAL_STATE();

	public:

		hkbEventsFromRangeModifier( hkFinishLoadedObjectFlag flag );

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
