/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_SHARED_EVENT_QUEUE_H
#define HKB_SHARED_EVENT_QUEUE_H

#include <Behavior/Behavior/Event/hkbEvent.h>
#include <Common/Base/Thread/CriticalSection/hkCriticalSection.h>

class hkbCharacter;
class hkbEventQueue;

/// A sharable queue for storing pairs of hkbEvents and hkbCharacters.  When you access the queue it is locked
/// to ensure exclusive access.
class hkbSharedEventQueue
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbSharedEventQueue );

			/// Adds an event to the queue.
		void enqueue( hkbEvent event, hkbCharacter* character );

			/// Transfers the events from this queue to per-character queues.
		void transferEvents( const hkPointerMap<const hkbCharacter*, hkbEventQueue*>& characterToEventQueue );

			/// Returns the number of events currently in the queue.
		int getSize() const;

	private:

			// an entry in the queue includes an event and the character that the event is intended for
		struct QueueEntry
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbSharedEventQueue::QueueEntry );

			hkbEvent m_event;
			hkbCharacter* m_character;
		};

			// the queue itself
		hkArray<QueueEntry> m_queue;

			// this is used to ensure exclusive access
		hkCriticalSection m_criticalSection;
};

#include <Behavior/Behavior/Event/hkbSharedEventQueue.inl>

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
