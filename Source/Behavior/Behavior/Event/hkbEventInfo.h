/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_EVENT_INFO_H
#define HKB_EVENT_INFO_H


extern const class hkClass hkbEventInfoClass;

	/// A description of an event that dictates whether it will be treated as a sync point
	/// and whether it will be sent by hkbClipGenerators.
class hkbEventInfo
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbEventInfo );
		HK_DECLARE_REFLECTION();

		hkbEventInfo();

			/// Whether or not this event should be treated as a sync point.
		bool isSyncPoint();

			/// Whether or not this event should be raised by clip generators.
		bool isSilent();

			/// Definitions of flags that influence the behavior of events.
		enum Flags
		{
				/// Whether or not clip generators should raise the event.
			FLAG_SILENT = 0x1,

				/// Whether or not the sync point will be 
			FLAG_SYNC_POINT = 0x2,
		};

			/// The flags that influence the behavior of events.
		hkFlags<Flags,hkUint32> m_flags;
};

#include <Behavior/Behavior/Event/hkbEventInfo.inl>

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
