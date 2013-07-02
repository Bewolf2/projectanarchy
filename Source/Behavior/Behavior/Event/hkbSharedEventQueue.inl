/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE void hkbSharedEventQueue::enqueue( hkbEvent event, hkbCharacter* character )
{
	HK_ASSERT2( 0x6152b899, event.isValid(), "null events should not go on the queue" );
	HK_ASSERT2( 0x7b81cf91, event.getSender() == HK_NULL, "an event to another character should not have a sender node" );

	QueueEntry entry;
	entry.m_character = character;
	entry.m_event = event;

	// lock the queue and enqueue the event
	m_criticalSection.enter();
	m_queue.pushBack( entry );
	m_criticalSection.leave();
}

HK_FORCE_INLINE int hkbSharedEventQueue::getSize() const
{
	return m_queue.getSize();
}

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
