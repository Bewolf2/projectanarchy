/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_STATE_MACHINE_LISTENER_UTILS_H
#define HKB_STATE_MACHINE_LISTENER_UTILS_H

class hkbStateListener;
class hkbBehaviorGraph;

	/// Utility to help connect hkbStateListeners to states at load time. 
class hkbStateMachineListenerUtils
{
	public:

			/// Collects all hkbStateMachines in behavior and linearly searches for states 
			/// assigned an id of stateId.  Listener is added to each state found.  If stateId
			/// is -1 the listener is added to all state machines in behavior.
		static void HK_CALL findStateByIdAndAddListener( hkbBehaviorGraph* behavior, const char* stateMachineName, int stateId, hkbStateListener* listener );
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
