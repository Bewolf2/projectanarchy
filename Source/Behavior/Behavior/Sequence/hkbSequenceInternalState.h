/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// WARNING: THIS FILE IS GENERATED. EDITS WILL BE LOST.
// Generated from 'Behavior/Behavior/Sequence/hkbSequence.h'

#ifndef HKB_SEQUENCE_INTERNAL_STATE_H
#define HKB_SEQUENCE_INTERNAL_STATE_H

#include <Behavior/Behavior/Sequence/hkbSequence.h>

class hkbSequenceInternalState : public hkReferencedObject
{
	//+vtable(1)
	//+version(0)
	public:

		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );

		hkbSequenceInternalState() {}
		hkbSequenceInternalState( hkFinishLoadedObjectFlag flag )
			: hkReferencedObject(flag)
			, m_nextSampleEvents(flag)
			, m_nextSampleReals(flag)
			, m_nextSampleBools(flag)
			, m_nextSampleInts(flag)
		{
		}

		hkArray<hkInt32> m_nextSampleEvents;
		hkArray<hkInt32> m_nextSampleReals;
		hkArray<hkInt32> m_nextSampleBools;
		hkArray<hkInt32> m_nextSampleInts;
		hkReal m_time;
		hkBool m_isEnabled;
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
