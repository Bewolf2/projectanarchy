/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_EVENT_QUEUE_H
#define HKB_EVENT_QUEUE_H

#include <Behavior/Behavior/Event/hkbEvent.h>
#include <Common/Base/Container/Queue/hkQueue.h>
#include <Behavior/Behavior/Linker/hkbSymbolIdMap.h>

/// A queue for storing hkbEvents.  
///
/// An hkbEventQueue stores events using the external IDs that result from the linking process.  
/// In order to make it easier for nodes to enqueue events during update(), the queue stores a map between
/// the local behavior's event IDs and those of the external symbol table.  When not doing update(), the map
/// is null so you have to make sure to map the event to an external before putting it on the queue.
class hkbEventQueue
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbEventQueue );

			/// Construct a queue with zero initial capacity.
		hkbEventQueue();

			/// Construct a queue with a given initial capacity.
		hkbEventQueue( int initialCapacity );

			/// Returns true if the queue is empty.
		bool isEmpty() const;

			/// Returns the number of events currently in the queue.
		int getSize() const;

			/// Adds an event to the queue.
			/// The ID of the event is assumed to be internal to the local behavior graph, so eventIdMap must be non-null.
		void enqueueWithInternalId( hkbEvent event, const hkbSymbolIdMap* eventIdMap );

			/// Adds an event coming from an hkbEventProperty with a sender (can be HK_NULL) to the queue.
			/// The ID of the event is assumed to be internal and is mapped to external by eventIdMap.
		void enqueueWithInternalId( const hkbEventProperty& eventProperty, hkbNode* sender, const hkbSymbolIdMap* eventIdMap );

			/// Adds an event to the queue.
			/// The ID of the event is assumed to be an external ID from the linker.
		void enqueueWithExternalId( hkbEvent event );

			/// Adds all of the events from the input queue to this queue.
			/// The IDs of the events are assumed to be an external ID from the linker.
		void enqueueAll( hkbEventQueue& queue );

			/// Removes an event from the queue.
		const hkbEvent dequeue();

			/// Returns true if the queue contains the given event.  
			///
		    /// The event is not removed from the queue.
			/// Requires O(n) time, where n is the size of the queue.
		bool contains( const hkbEvent event );

			/// Empties the queue.
		void clear();

	private:

			// the queue itself
		hkQueue<hkbEvent> m_queue;
};

#include <Behavior/Behavior/Event/hkbEventQueue.inl>

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
