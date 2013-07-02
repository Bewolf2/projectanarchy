/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_WORLD_H
#define HKB_WORLD_H

#include <Behavior/Behavior/BehaviorGraph/hkbBehaviorGraph.h>
#include <Behavior/Behavior/Linker/hkbSymbolLinker.h>
#include <Behavior/Behavior/Generator/Clip/hkbUnloadedAnimationListener.h>
#include <Behavior/Utilities/Physics/hkbPhysicsBaseTypes.h>

class hkJobQueue;
class hkbAttachmentManager;
class hkbCharacter;
class hkbContext;
class hkbGeneratorOutput;
class hkbSceneModifier;
class hkbSharedEventQueue;
class hkbWorldCinfo;
class hkbWorldListener;
class hkaRaycastInterface;
class hkCriticalSection;
class hkJobThreadPool;

	/// A serialized class for serializing enums related to the hkbWorld.
struct hkbWorldEnums
{
	HK_DECLARE_REFLECTION();
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR, hkbWorldEnums);

	hkbWorldEnums() {}
	hkbWorldEnums( hkFinishLoadedObjectFlag flag ) {}

		/// Whether the simulation is playing, paused, or stepping.
	enum SimulationState
	{
			/// Currently playing.
		SIMULATION_STATE_PLAY,
			/// Started playing but has been paused.
		SIMULATION_STATE_PAUSE,
			/// Started playing but is paused and in single step mode.
		SIMULATION_STATE_STEP,
			/// Has not been played but is ready for playback.
		SIMULATION_STATE_STOP
	};

		/// Whether or not the characters are allowed to move.
	enum AccumulateMotionState
	{
		ACCUMULATE_MOTION,
		DO_NOT_ACCUMULATE_MOTION
	};
};

	/// The hkbWorld object is an optional container class for hkbCharacters.
class hkbWorld : public hkReferencedObject
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR);

			/// Initializes the world with no characters in it
		hkbWorld( const hkbWorldCinfo& cinfo );
			/// Destroys the world
		~hkbWorld();
			
			/// Gets the up axis of the world.
		const hkVector4& getUp() const;
			/// Sets the up axis of the world.
		void setUp( const hkVector4& up );
		
			/// Adds a character to the world and optionally activates its root behavior.  You cannot add the same 
			/// character to the world multiple times.  If activateCharacter is true, and the character's behavior graph is 
			/// already active it will not be activated again.
		void addCharacter( hkbCharacter* character, bool activateCharacter = true );
			/// Removes a character from the world and optionally deactivates its root behavior.  You cannot remove a 
			/// character that is not in the world.  If deactivateCharacter is true, and the character's behavior graph is
			/// already inactive it will not be deactivated again.
		void removeCharacter( hkbCharacter* character, bool deactivateCharacter = true );
			/// Gets an array of all characters in the world.
		const hkArray<hkbCharacter*>& getCharacters() const;
			/// Activate all the characters in the world.
		void activateAllCharacters() const;
			/// Deactivate all the characters in the world.
		void deactivateAllCharacters() const;
		
			/// Adds a listener to the world.
		void addListener( hkbWorldListener* listener );			
			/// Removes a listener from the world.
		void removeListener( hkbWorldListener* listener );
			/// Gets an array of all listeners in the world.			
		const hkArray<hkbWorldListener*>& getListeners() const;

			/// Updates and generates the behaviors of all characters in the world.  The output 		
			/// pose is stored in each hkbCharacter.  This will call stepSingleThreaded() if you don't pass in a jobQueue
			/// and jobThreadPool, and it will call stepMultithreaded() if you do.
		void step( hkReal deltaTime, hkJobQueue* jobQueue = HK_NULL, hkJobThreadPool* jobThreadPool = HK_NULL );
			/// Updates and generates the behaviors of all characters in the world using only the calling thread.  The output 		
			/// pose is stored in each hkbCharacter.
		void stepSingleThreaded( hkReal deltaTime );
			/// Do a full multithreaded step of all characters in the world.  For finer-grained control you
			/// can break down the multithreading step as follows:
			/// beginStepMultithreaded();
			/// while ( getMoreJobsMultithreaded( jobQueue ) )
			/// {
			///		// process all jobs
			///	}
		void stepMultithreaded( hkReal deltaTime, hkJobQueue& jobQueue, hkJobThreadPool& jobThreadPool );
			/// First updates the behaviors of all characters in the calling thread.  Then creates hkbBehaviorJobs 
			/// and adds them to jobQueue.  When you process jobQueue the hkbBehaviorJobs will generate the pose for
			/// each character.  Call hkbWorld::endStepMultiThreaded after the jobQueue is done processing.
		void beginStepMultithreaded( hkReal deltaTime );
			/// This should be called iteratively to get more jobs until it returns false, at which point the step has finished.
		bool getMoreJobsMultithreaded( hkJobQueue& jobQueue );

			/// Utility method that activates, generates and deactivates a character's behavior.  The output pose of 
			/// will be stored in the character.  If deltatime is non-zero the character's behavior will be updated
			/// by that amount.
		void singleStepCharacter( hkbCharacter* character, hkReal deltaTime = 0.0f );

			/// Gets the hkbPhysicsInterface used by the hkbWorld. May be HK_NULL if physics isn't used.
		hkbPhysicsInterface* getPhysicsInterface() const { return m_physicsInterface; }
			/// Set the hkbPhysicsInterface for this world ensuring that all the entities that are there in the old physics world
			/// are also in this world.  May be HK_NULL if physics isn't used.
		void setPhysicsInterface( hkbPhysicsInterface* physicsInterface );

		/// Gets the hkbAttachmentManager used by the hkbWorld
		hkbAttachmentManager* getAttachmentManager() const;			

			/// Links the behavior assigned to the character.
		void link( hkbCharacter* character, hkbBehaviorGraph* templateBehavior );
			/// Links the behavior assigned to the character.
		void link( hkbCharacter* character );
			/// Unlinks the behavior assigned to the character.
		void unlink( hkbCharacter* character );

			/// Looks up an event's external ID by name from the world's event symbol table.
		int getEventId( const char* name );
			/// Looks up an event name by external id from the world's event symbol table.
		const char* getEventName( int externalId );
		
			/// Looks up a variable's external ID by name from the world's variable symbol table.
		int getVariableId( const char* name );
			/// Looks up a variable name by external id from the world's variable symbol table.
		const char* getVariableName( int externalId );

			/// Returns the linker that is being used to link all of the events in all behavior graphs.
		const hkbSymbolLinker& getEventLinker();
			/// Returns the linker that is being used to link all of the events in all behavior graphs.
		hkbSymbolLinker& accessEventLinker();
			/// Returns the linker that is being used to link all of the variables in all behavior graphs.
		hkbSymbolLinker& accessVariableLinker();
			/// Returns the linker that is being used to link all of the attributes in all behavior graphs.
		hkbSymbolLinker& accessAttributeLinker();
			/// Returns the linker that is being used to link all of the character properties in all behavior graphs.
		hkbSymbolLinker& accessCharacterPropertyLinker();

			/// Whether or not multithreaded processing is going on right now.  This is used for debug checks.
		hkBool32 isMultithreading() const;

			/// Returns the shared event queue, if any.  It will only be non-null if you are multithreading update().
		hkbSharedEventQueue* getSharedEventQueue() const;
			
			/// Get the script debugger port - used by characters to connect to Havok Script Studio
		hkInt32 getScriptDebuggerPort() const;

            /// Set the clip unloaded animation listener - this is a class that determines the animation to use for hkbClipGenerators if their animation isn't loaded
		void setUnloadedAnimationListener( hkbUnloadedAnimationListener* unloadedAnimationListener );

			/// Returns the clip unloaded animation listener
		hkbUnloadedAnimationListener* getUnloadedAnimationListener() const;

		//////////////////////////////////////////////////////////////////////////
		// Deprecated Physics Implementation Specific Functions
		// Must link against hkbPhysics2012Bridge
		//////////////////////////////////////////////////////////////////////////

			/// Gets the physics world used by the hkbWorld's physics interface.
			/// This will be deprecated in the future, see getPhysicsInterface();
		hkbDeprecatedPhysicsWorldPtr getPhysicsWorld() const;
			/// Set the physics world used by the hkbWorld's physics interface. Make sure that all the entities that are there in the old physics world
			/// are also in this world. For HAT use only.
			/// This will be deprecated in the future, see setPhysicsInterface();
		void setPhysicsWorld( hkbDeprecatedPhysicsWorldPtr m_physicsWorld );

	public:

			/// The scene modifiers to be applied during generate().
		hkArray< hkRefPtr<hkbSceneModifier> > m_sceneModifiers;

	protected:			

			/// Working data needed during a step.  This is allocated and deallocated every frame when stepping.
		struct StepWorkingData
		{	
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR, StepWorkingData);
				
				/// Contexts used in hkbBehaviorJobs, one per character
			hkbContext* m_contexts;
				/// Generator output used in hkbBehaviorJobs, one per character
			hkbGeneratorOutput* m_generatorOutputs;
				/// Behavior graph working data used in hkbBehaviorJobs, one per character
			hkbBehaviorGraph::GenerateWorkingData* m_generateWorkingData;

				/// Number of characters.
			hkInt32 m_numCharacters;
				/// Delta time of the update
			hkReal m_deltaTime;
		};
	
			/// Allocates and initializes MulithreadedWorkingData for the hkbWorld step
		void allocateWorkingData( hkReal deltaTime );			
			/// Deallocates and destructs MulithreadedWorkingData for the hkbWorld step
		void deallocateWorkingData();
		
			/// Updates the behavior graphs of all characters in the world
		void updateAllCharacters( hkReal deltaTime );
			/// Updates the behavior graph of a character.
		static void updateCharacter( hkReal deltaTime, const hkbContext& context, hkbCharacter& character, hkbWorld* world );

			/// Apply the scene modifiers in m_sceneModifiers to the characters being generated.
		void applySceneModifiers( hkbContext* contexts, hkbGeneratorOutput* generatorOutputs, hkReal deltaTime, int numCharacters );

			/// Copy the pose in the generator output to the character
		void copyPoseToCharacters( hkbContext* contexts, hkbGeneratorOutput* generatorOutputs, hkReal deltaTime, int numCharacters );

			/// Calls eventRaisedCallback on all of the hkbWorldListeners.
		void notifyListenersOfEvents( hkbCharacter& character, hkbEventQueue& eventQueue, bool raisedBySdk );

			/// Transfer events from the shared event queue to the individual character queues.
		void transferSharedEvents();

	protected:

			/// hkbCharacters maintained by the world.
		hkArray<hkbCharacter*> m_characters;

			/// Listeners attached to the world.
		hkArray<hkbWorldListener*> m_listeners;

			/// Symbol linkers used to map sybmol IDs for all behaviors in the world.
		hkbSymbolLinker m_eventLinker;
		hkbSymbolLinker m_variableLinker;
		hkbSymbolLinker m_attributeLinker;
		hkbSymbolLinker m_characterPropertyLinker;
		
			/// World space up vector.
		hkVector4 m_up;	

			/// Attachment manager for behavior attachments.
		hkbAttachmentManager* m_attachmentManager;
		
			/// Optionally physics interface being used (Note: hkbPhysicsInterface inherits from hkaRaycastInterface).
		hkRefPtr<hkbPhysicsInterface> m_physicsInterface;

			/// Working data used for multithreading.
		StepWorkingData* m_workingData;

			/// A critical section that is used when notifying listeners during multithreading update().
		hkCriticalSection* m_notifyListenersCriticalSection;

			/// A queue for events passed from one character to another when multithreading.
		hkbSharedEventQueue* m_sharedEventQueue;

			/// See hkbWorldCinfo::m_enableMultithreadedUpdate.
		hkBool m_enableMultithreadedUpdate;

		enum MultithreadingStage
		{
			STAGE_NONE,
			STAGE_UPDATE,
			STAGE_GENERATE,
			STAGE_FINISH
		};

		hkEnum<MultithreadingStage, hkInt8> m_multithreadingStage;

			/// This is true whenever the world is currently doing things with multiple threads.
		hkBool m_isMultithreading;

			/// Script Debugger port used to specify which port characters in this world should connect to
		hkInt32 m_scriptDebuggerPort;

			/// A class that describes what the default reference pose looks like for hkbClipGenerator if an animation does not exist
		hkRefPtr<hkbUnloadedAnimationListener> m_unloadedAnimationListener;

	public:

			/// Whether the simulation is playing, paused, or stepping.
		hkEnum<hkbWorldEnums::SimulationState, hkUint8> m_simulationState;

			/// Whether to update each character's world-from-model
			/// based on the motion that comes from the character's behavior graph.
		hkEnum<hkbWorldEnums::AccumulateMotionState, hkUint8> m_accumulateMotionState;

		friend class hkbCpuBehaviorJob;
};

#include <Behavior/Behavior/World/hkbWorld.inl>

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
