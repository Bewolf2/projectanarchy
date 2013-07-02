/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_EVENT_H
#define HKB_EVENT_H

#include <Behavior/Behavior/Event/hkbEventPayload.h>

class hkbNode;

extern const class hkClass hkbEventBaseClass;
extern const class hkClass hkbEventClass;
extern const class hkClass hkbEventPropertyClass;

	/// The base class for hkbEvent (with a non-reference counted payload) and hkbEventProperty (with a reference counted payload).
class hkbEventBase
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbEventBase );
		HK_DECLARE_REFLECTION();

			/// The ID of an event is a 32-bit integer.
		typedef hkInt32 Id;

			/// Event IDs allocated by the system.
		enum SystemEventIds
		{
			/// A special event ID representing an invalid event.
			EVENT_ID_NULL = -1,
		};

			/// Construct with null ID and null payload.
		hkbEventBase();

			/// Construct an event with an initial ID and zero payload.
		hkbEventBase( Id id );

			/// Construct an event with an initial ID and payload.
		hkbEventBase( Id id, hkbEventPayload* payload );

			/// Construct from an hkbEventBase.
		hkbEventBase( const hkbEventBase& e );

			/// Finish constructor.
		hkbEventBase( hkFinishLoadedObjectFlag flag );

			/// Get the ID of the event.
		Id getId() const;

			/// Set the ID of the event.
		void setId( Id id );

			/// Compare two events (by comparing their IDs).
		int operator == ( const hkbEventBase event ) const;

			/// Check to see if the event is valid (it is valid if the ID is not EVENT_ID_NULL).
		bool isValid() const;

			/// Get the event payload (user data).
		hkbEventPayload* getPayload() const;

	public:
		// These are public so that HAT can expose them in the property sheet for hkbEventProperty.

			/// the event ID
		Id m_id; //+default(hkbEventBase::EVENT_ID_NULL)+hkb.RoleAttribute("ROLE_EVENT_ID","FLAG_NONE")

			// the payload (user data)
		hkbEventPayload* m_payload;
};

/// A behavior event.
///
/// An hkbEvent contains an integer ID and a payload.  Events can be sent to behavior nodes by the user, 
/// or sent by behavior nodes to each other or to the user.  This class does not reference count the payload.
class hkbEvent : public hkbEventBase
{
	//+version(1)
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbEvent );
		HK_DECLARE_REFLECTION();

			/// Construct with null ID and null payload.
		hkbEvent();

			/// Construct an event with an initial ID and zero payload.
		hkbEvent( Id id );

			/// Construct an event with an initial ID and payload.
		hkbEvent( Id id, hkbEventPayload* payload );

			/// Construct an event with an initial ID, payload and sender.
		hkbEvent( Id id, hkbEventPayload* payload, hkbNode* sender );

			/// Construct from an hkbEvent.
		hkbEvent( const hkbEvent& ep );

			/// Finish constructor.
		hkbEvent( hkFinishLoadedObjectFlag flag );

			/// Set the event payload (user data). 
		void setPayload( hkbEventPayload* payload );

			/// Get the node that sent the event.  When using shallow cloned behaviors, the sender is a node template, not a clone.
		hkbNode* getSender() const;

	private:

			/// The node that sent the event (if any).  This is not reference counted.
		hkbNode* m_sender;	//+nosave
};

	/// An event that is used as a node property.  It differs from hkbEvent in that the payload is reference counted
	/// and it doesn't have a sender.
class hkbEventProperty : public hkbEventBase
{
	//+version(1)
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbEventProperty );
		HK_DECLARE_REFLECTION();

			/// Construct with null ID and null payload.
		hkbEventProperty();

			/// Construct with an initial ID and zero payload.
		hkbEventProperty( hkbEvent::Id id );

			/// Construct with initial ID and payload.
		hkbEventProperty( hkbEvent::Id id, hkbEventPayload* payload );

			/// Construct from an event.
		hkbEventProperty( const hkbEventBase& e );

			/// Construct from an event property.  We need this in addition to the one above because
			/// the compiler will generate a strict copy constructor if none is present, even if there
			/// is a constructor like the one above that could, in principle, be used instead.
		hkbEventProperty( const hkbEventProperty& e );

			/// Finish constructor.
		hkbEventProperty( hkFinishLoadedObjectFlag flag );

		~hkbEventProperty();

			/// Set the event payload (user data) and add a reference. 
		void setPayload( hkbEventPayload* payload );

			/// Asignment operator to reference count the payload.
		void operator = (const hkbEventProperty& e );
};

#include <Behavior/Behavior/Event/hkbEvent.inl>

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
