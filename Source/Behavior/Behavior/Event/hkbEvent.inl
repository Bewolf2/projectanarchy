/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/////////////////
// hkbEventBase
/////////////////

HK_FORCE_INLINE hkbEventBase::hkbEventBase()
:	m_id(EVENT_ID_NULL),
	m_payload(HK_NULL)
{
}

HK_FORCE_INLINE hkbEventBase::hkbEventBase( Id id ) 
:	m_id(id),
	m_payload(HK_NULL)
{
}

HK_FORCE_INLINE hkbEventBase::hkbEventBase( Id id, hkbEventPayload* payload )
:	m_id(id),
	m_payload(payload)
{
}

HK_FORCE_INLINE hkbEventBase::hkbEventBase( const hkbEventBase& e )
:	m_id(e.m_id),
	m_payload(e.m_payload)
{
}

HK_FORCE_INLINE hkbEventBase::hkbEventBase( hkFinishLoadedObjectFlag flag )
{
}

HK_FORCE_INLINE hkbEventBase::Id hkbEventBase::getId() const 
{ 
	return m_id; 
}

HK_FORCE_INLINE void hkbEventBase::setId( Id id ) 
{ 
	m_id = id; 
}

HK_FORCE_INLINE int hkbEventBase::operator == ( const hkbEventBase event ) const 
{ 
	return event.m_id == m_id; 
}

HK_FORCE_INLINE bool hkbEventBase::isValid() const 
{ 
	return m_id != EVENT_ID_NULL; 
}

HK_FORCE_INLINE hkbEventPayload* hkbEventBase::getPayload() const
{
	return m_payload;
}

/////////////
// hkbEvent
/////////////

HK_FORCE_INLINE hkbEvent::hkbEvent()
:	m_sender(HK_NULL)
{
}

HK_FORCE_INLINE hkbEvent::hkbEvent( Id id ) 
:	hkbEventBase(id),
	m_sender(HK_NULL)
{
}

HK_FORCE_INLINE hkbEvent::hkbEvent( Id id, hkbEventPayload* payload )
:	hkbEventBase(id, payload),
	m_sender(HK_NULL)
{
}

HK_FORCE_INLINE hkbEvent::hkbEvent( Id id, hkbEventPayload* payload, hkbNode* sender )
:	hkbEventBase(id, payload),
	m_sender(sender)
{
}

HK_FORCE_INLINE hkbEvent::hkbEvent( const hkbEvent& e )
:	hkbEventBase(e),
	m_sender(e.m_sender)
{
}

HK_FORCE_INLINE hkbEvent::hkbEvent( hkFinishLoadedObjectFlag flag )
:	hkbEventBase(flag)
{
}

HK_FORCE_INLINE void hkbEvent::setPayload( hkbEventPayload* payload )
{
	m_payload = payload;
}

HK_FORCE_INLINE hkbNode* hkbEvent::getSender() const
{
	return m_sender;
}

/////////////////////
// hkbEventProperty
/////////////////////

HK_FORCE_INLINE void hkbEventProperty::setPayload( hkbEventPayload* payload )
{
	// don't use the macro since this is in a header file
	if ( payload != HK_NULL )
	{
		payload->addReference();
	}

	if ( m_payload != HK_NULL )
	{
		m_payload->removeReference();
	}

	m_payload = payload;
}

HK_FORCE_INLINE hkbEventProperty::hkbEventProperty()
{
}

HK_FORCE_INLINE hkbEventProperty::hkbEventProperty( hkbEvent::Id id )
:	hkbEventBase( id )
{
}

HK_FORCE_INLINE hkbEventProperty::hkbEventProperty( hkbEvent::Id id, hkbEventPayload* payload )
:	hkbEventBase( id )
{
	setPayload( payload );
}

HK_FORCE_INLINE hkbEventProperty::hkbEventProperty( const hkbEventBase& e )
:	hkbEventBase( e.getId() )
{
	// this is done here instead of in the constructor for hkbEventBase because we need to reference count it
	setPayload( e.getPayload() );
}

HK_FORCE_INLINE hkbEventProperty::hkbEventProperty( const hkbEventProperty& e )
:	hkbEventBase( e.getId() )
{
	// this is done here instead of in the constructor for hkbEventBase because we need to reference count it
	setPayload( e.getPayload() );
}

HK_FORCE_INLINE hkbEventProperty::hkbEventProperty( hkFinishLoadedObjectFlag flag )
:	hkbEventBase(flag)
{
}

HK_FORCE_INLINE hkbEventProperty::~hkbEventProperty()
{
	setPayload( HK_NULL );
}

HK_FORCE_INLINE void hkbEventProperty::operator = (const hkbEventProperty& e )
{
	m_id = e.getId();
	setPayload( e.getPayload() );
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
