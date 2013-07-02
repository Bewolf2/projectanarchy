/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_TEST_STATE_CHOOSER_H
#define HKB_TEST_STATE_CHOOSER_H

#include <Behavior/Behavior/StateMachine/hkbStateChooser.h>

	/// An hkbStateChooser for testing purposes only.
class hkbTestStateChooser : public hkbStateChooser
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbTestStateChooser() : m_int(0), m_real(0.0f) {}

			/// Returns the ID of the start state for the given state machine.
		virtual int chooseStateId( const hkbContext& context, const hkbStateMachine& stateMachine ) HK_OVERRIDE
		{
			return 0;
		}

	public:

		int m_int;
		hkReal m_real;
		hkStringPtr m_string;

	public:

		hkbTestStateChooser( hkFinishLoadedObjectFlag flag ) : hkbStateChooser(flag), m_string(flag) {}

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
