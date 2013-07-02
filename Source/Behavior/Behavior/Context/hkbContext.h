/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_CONTEXT_H
#define HKB_CONTEXT_H

#include <Behavior/Behavior/Character/hkbCharacter.h>
#include <Behavior/Utilities/Physics/hkbPhysicsBaseTypes.h>

class hkbAttachmentManager;
class hkbBehaviorGraph;
class hkbCharacter;
class hkbGeneratorOutputListener;
class hkbNode;
class hkbProjectData;
class hkbBehaviorGraph;
class hkbSharedEventQueue;

template <typename K, typename V, typename Allocator> class hkPointerMap;

extern const class hkClass hkbContextClass;

/// The context in which a behavior is being used.
/// 
/// Provides some information about the environment in which a behavior is being used.
/// This object is meant to be temporary and lightweight, so it does not reference count
/// the objects that it is pointing to.
class hkbContext
{
	//+version(2)
	public:

		
		HK_DECLARE_REFLECTION();
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbContext );

			/// Construct with defaults.
		hkbContext();

			/// Copy constructor.
		hkbContext( const hkbContext& other );

			/// Construct from data.
		hkbContext(	hkbCharacter* character,
					hkbPhysicsInterface* physicsInterface = HK_NULL, 
					hkbAttachmentManager* attachmentManager = HK_NULL );

			/// Dtor.
		~hkbContext();

	public:

			/// Get the current character.
		hkbCharacter* getCharacter() const;

			/// Get the hkbPhysicsInterface from storage in the following order (may be HK_NULL):
			/// First the hkbContext's physics interface and then the hkbWorld's physics interface
		hkbPhysicsInterface* getPhysicsInterface() const;

			/// Get the world up vector.
		const hkVector4& getWorldUpWS() const;

			/// Get the attachment manager (might be HK_NULL).
		hkbAttachmentManager* getAttachmentManager() const;

			/// Get the project data (might be HK_NULL).
		const hkbProjectData* getProjectData() const;

			/// Get the root behavior.
		hkbBehaviorGraph* getRootBehavior() const;

			/// Get the current behavior.
		hkbBehaviorGraph* getBehavior() const;

			/// Returns the ragdoll handle from the hkbRagdollDriver.
		hkbRagdollInterface* getRagdollInterface() const;

			/// Get the event queue of the current character.
		hkbEventQueue* getEventQueue() const;

			/// Get the world-from-model transform of the current character.
		const hkQsTransform& getWorldFromModel() const;

			/// Get the read only pose (buffer) of the current character.
		const hkQsTransform* getPoseLocal() const;

			/// Get the pose (buffer) of the current character.
		hkQsTransform* accessPoseLocal() const;

			/// Get the setup for the current character.
		const hkbCharacterSetup* getCharacterSetup() const;

			/// Get the object that is listening to intermediate generator outputs.
		hkbGeneratorOutputListener* getOutputListener() const;

			/// Get the lua state
		void* getLuaState() const;

			/// Set the hkbPhysicsInterface for this context (can be HK_NULL).
		void setPhysicsInterface( hkbPhysicsInterface* physicsInterface );
		
			/// Whether or not the node is active in the root behavior.
		bool isActive( hkbNode* node ) const;

			/// Set the attachment manager (not reference counted or deleted).
		void setAttachmentManager( hkbAttachmentManager* attachmentmanager );

			/// Set the object that is listening to intermediate generator outputs.
		void setOutputListener( hkbGeneratorOutputListener* outputListener );

			/// Send an event to another character.  If m_sharedEventQueue is not null then it is used.
			/// Otherwise, the event is put directly onto the queue of the other character.
		void sendEventToCharacter( hkbEvent e, hkbCharacter* character, hkbSymbolIdMap* eventIdMap ) const;

			/// Calls hkbPhysicsInterface::markForWrite() after a debug check.
		void markPhysicsForWrite() const;

			/// Calls hkbPhysicsInterface::unmarkForWrite() after a debug check.
		void unmarkPhysicsForWrite() const;

			/// For internal use.  Asserts that the behavior world is not multithreading.
		void assertNotMultithreading() const;

		//////////////////////////////////////////////////////////////////////////
		// Deprecated Physics Implementation Specific Functions
		// Must link against hkbPhysics2012Bridge
		//////////////////////////////////////////////////////////////////////////

			/// Convenience constructor from physics world
		hkbContext(	hkbCharacter* character,
					hkbDeprecatedPhysicsWorldPtr world,
					hkbAttachmentManager* attachmentManager = HK_NULL );

			/// Get the physics world from the stored physicsInterface.
			/// This accessor will be deprecated in the future (see getPhysicsInterface).
		hkbDeprecatedPhysicsWorldPtr getWorld() const;

			/// Get the handle for the character's ragdoll pointer.
			/// This accessor will be deprecated in the future (see getRagdollInterface).
		hkbDeprecatedRagdollPtr getRagdoll() const;

			/// Set the world (can be HK_NULL).
			/// This accessor will be deprecated in the future (see setPhysicsInterface).
		void setWorld( hkbDeprecatedPhysicsWorldPtr world );


	public:

			/// The data associated with a character.
		hkbCharacter* m_character; //+nosave

			/// The root behavior.  If this behavior is set it is considered the character's root 
			/// behavior.  If not the root behavior is the one assigned to the character.
		hkbBehaviorGraph* m_rootBehavior;

			/// The active behavior.  Do not set this.  It is for internal use.
			///
			/// When the context is created we do not allow the user to set the m_behavior property and by
			/// default its value in null. The behavior node is given the responsibility to set this value.
			/// The node interface takes const reference to context, so in order to set the m_behavior property
			/// we made m_behavior mutable.			
		mutable hkbBehaviorGraph* m_behavior; //+nosave

			// In certain cases (during collectActiveNodes), the behavior will
			// not have the list of active nodes built yet.  Instead, we get them
			// from this map.
			/// For internal use.  
		mutable const hkPointerMap<const hkbNode*,int>* m_nodeToIndexMap; //+nosave
		
			/// If you set this, events will be put on this queue instead of the character's queue.
		mutable hkbEventQueue* m_eventQueue; //+nosave

			/// This queue is used to store events that are sent to other characters when multithreading update().
			/// If you are using hkbWorld then you don't have to set this yourself because the hkbWorld will do it.
			/// If it is null then the event will be placed directly onto the other character's queue.  
			/// If you are multithreading update() and using the hkbAttachmentModifier or a custom modifier that 
			/// places events onto another character's queue then this should not be null.
		hkbSharedEventQueue* m_sharedEventQueue;	//+nosave

			/// A pointer to a generator output listener
			///
			/// If you are interested in the output of a set of nodes in the
			/// behavior graph then you need to inherit from hkbGeneratorOutputListener
			/// and provide an appropriate implementation.
		hkbGeneratorOutputListener* m_generatorOutputListener;

			/// The information needed to sync variables.
		mutable hkbSyncVariableSetup* m_syncVariableSetup;	//+nosave

			/// Whether an event while being processed triggered a transition.
		mutable hkBool m_eventTriggeredTransition; //+nosave

	private:

			/// The hkbPhysicsInterface (HK_NULL if you are not using physics)
		hkRefPtr<hkbPhysicsInterface> m_physicsInterface; //+nosave

			/// The attachment manager.
		hkbAttachmentManager* m_attachmentManager; //+nosave
};

#include <Behavior/Behavior/Context/hkbContext.inl>

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
