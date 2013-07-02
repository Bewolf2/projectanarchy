/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_EVENT_PAYLOAD_H
#define HKB_EVENT_PAYLOAD_H


extern const class hkClass hkbNamedStringEventPayloadClass;

extern const class hkClass hkbNamedIntEventPayloadClass;

extern const class hkClass hkbNamedRealEventPayloadClass;

extern const class hkClass hkbNamedEventPayloadClass;

extern const class hkClass hkbStringEventPayloadClass;

extern const class hkClass hkbIntEventPayloadClass;

extern const class hkClass hkbRealEventPayloadClass;

extern const class hkClass hkbEventPayloadClass;

extern const class hkClass hkbEventPayloadListClass;

	/// All payloads of hkbEvents derive from this class.
class hkbEventPayload : public hkReferencedObject
{
	//+vtable(1)
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

	protected:

		hkbEventPayload() {}

	public:

		hkbEventPayload( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag) {}
};

	/// An event payload that contains an hkReal.
class hkbRealEventPayload : public hkbEventPayload
{
	//+vtable(1)
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbRealEventPayload() : m_data(0.0f) {}

	public:

			/// An hkReal that is the payload data.
		hkReal m_data;	//+hk.Description("The hkReal contents of the payload.")

	public:

		hkbRealEventPayload( hkFinishLoadedObjectFlag flag ) : hkbEventPayload(flag) {}
};

	/// An event payload that contains an integer.
class hkbIntEventPayload : public hkbEventPayload
{
	//+vtable(1)
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbIntEventPayload() : m_data(0) {}

	public:

			/// An hkInt32 that is the payload data.
		hkInt32 m_data;	//+hk.Description("The integer contents of the payload.")

	public:

		hkbIntEventPayload( hkFinishLoadedObjectFlag flag ) : hkbEventPayload(flag) {}
};

	/// An event payload that contains a string.
class hkbStringEventPayload : public hkbEventPayload
{
	//+vtable(1)
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbStringEventPayload() {}

	public:

			/// The string contents of the payload (unmanaged memory).
		hkStringPtr m_data;	//+hk.Description("The string contents of the payload.")

	public:

		hkbStringEventPayload( hkFinishLoadedObjectFlag flag ) : hkbEventPayload(flag), m_data(flag) {}
};

	/// An event payload that has a name.
class hkbNamedEventPayload : public hkbEventPayload
{
	//+vtable(1)
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbNamedEventPayload() {}

	public:

			/// The name of the payload (unmanaged memory).
		hkStringPtr m_name;	//+hk.Description("The name of the payload.")

	public:

		hkbNamedEventPayload( hkFinishLoadedObjectFlag flag ) : hkbEventPayload(flag), m_name(flag) {}
};

	/// An event payload that has a name and an hkReal.
class hkbNamedRealEventPayload : public hkbNamedEventPayload
{
	//+vtable(1)
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbNamedRealEventPayload() : m_data(0.0f) {}

	public:

			/// An hkReal that is the payload data.
		hkReal m_data;	//+hk.Description("The hkReal contents of the payload.")

	public:

		hkbNamedRealEventPayload( hkFinishLoadedObjectFlag flag ) : hkbNamedEventPayload(flag) {}
};

	/// An event payload that has a name and an integer.
class hkbNamedIntEventPayload : public hkbNamedEventPayload
{
	//+vtable(1)
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbNamedIntEventPayload() : m_data(0) {}

	public:

			/// An hkInt32 that is the payload data.
		hkInt32 m_data;	//+hk.Description("The integer contents of the payload.")

	public:

		hkbNamedIntEventPayload( hkFinishLoadedObjectFlag flag ) : hkbNamedEventPayload(flag) {}
};

	/// An event payload that has a name and a string.
class hkbNamedStringEventPayload : public hkbNamedEventPayload
{
	//+vtable(1)
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbNamedStringEventPayload() {}

	public:

			/// The string contents of the payload (unmanaged memory).
		hkStringPtr m_data;	//+hk.Description("The string contents of the payload.")

	public:

		hkbNamedStringEventPayload( hkFinishLoadedObjectFlag flag ) : hkbNamedEventPayload(flag), m_data(flag) {}
};

	/// An event payload that contains a list of event payloads.
class hkbEventPayloadList : public hkbEventPayload
{
	//+vtable(1)
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbEventPayloadList() {}

		~hkbEventPayloadList();

	public:

			/// The nested payloads (reference counted).
		hkArray<hkbEventPayload*> m_payloads;	//+hk.Description("The nested payloads.")

	public:

		hkbEventPayloadList( hkFinishLoadedObjectFlag flag ) : hkbEventPayload(flag), m_payloads(flag) {}
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
