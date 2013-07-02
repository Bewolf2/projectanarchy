/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_STATE_LISTENER_H
#define HKB_STATE_LISTENER_H

#include <Common/Base/Object/hkReferencedObject.h>

extern const class hkClass hkbStateListenerClass;

class hkbContext;
class hkbEventQueue;
class hkbStateMachine;

	/// Subclass this class if you want to receive callbacks regarding the state of a state machine.  Subclass instances
	/// can be attached to individual state machine states to receive callbacks when the state is entered, exited, or updated.
	/// If you have multiple characters using the same hkbStateListener instance (ie, you didn't override clone()),
	/// and you are multithreading Behavior, you will need to make sure that your class is either stateless
	/// or uses something like an hkCriticalSection to ensure exclusive access.
class hkbStateListener : public hkReferencedObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();
		
		hkbStateListener() {}

	public:

			/// This is called by onEnter() when the state is entered.  
			/// Your subclass should implement this if you want a callback when the state is entered.
			/// The state machine will have m_currentStateId set to the state that this listener is listening to.
		virtual void onEnterImplementation( const hkbContext& context, hkbStateMachine& sm ) {}

			/// This is called by onUpdate() when the state machine is updated while the state is active.
			/// Your subclass should implement this if you want a callback when the state is updated.
			/// The state machine will have m_currentStateId set to the state that this listener is listening to.
		virtual void onUpdateImplementation( const hkbContext& context, hkReal timestep, hkbStateMachine& sm ) {}

			/// This is called by onExit() when the state is exited.
			/// Your subclass should implement this if you want a callback when the state is exited.
			/// The state machine will have m_currentStateId set to the state that this listener is listening to.
		virtual void onExitImplementation( const hkbContext& context, hkbStateMachine& sm ) {}

			/// Return a clone of this listener.  This is used when cloning behavior graphs using
			/// hkbBehaviorGraph::cloneShallow().  The default implementation returns HK_NULL, which
			/// tells the hkbStateMachine not to clone the listener.
		virtual hkbStateListener* clone();

			/// This is called by the state machine when the state is entered.  
			/// This simply calls onEnterImplementation().
		void onEnter( const hkbContext& context, hkbStateMachine& sm );

			/// This is called by the state machine when the state machine is updated while the state is active.
			/// This simply calls onUpdateImplementation().
		void onUpdate( const hkbContext& context, hkReal timestep, hkbStateMachine& sm );

			/// This is called by the state machine when the state is exited.
			/// This simply calls onExitImplementation().
		void onExit( const hkbContext& context, hkbStateMachine& sm );

	public:

		hkbStateListener( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag) {}
};

#include <Behavior/Behavior/StateMachine/hkbStateListener.inl>

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
