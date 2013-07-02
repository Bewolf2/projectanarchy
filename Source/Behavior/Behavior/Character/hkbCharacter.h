/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_CHARACTER_H
#define HKB_CHARACTER_H

#include <Behavior/Behavior/Character/hkbCharacterSetup.h>
#include <Behavior/Behavior/Driver/CharacterController/hkbCharacterControllerDriver.h>
#include <Behavior/Behavior/Driver/FootIk/hkbFootIkDriver.h>
#include <Behavior/Behavior/Driver/HandIk/hkbHandIkDriver.h>
#include <Behavior/Behavior/Driver/Ragdoll/hkbRagdollDriver.h>
#include <Behavior/Behavior/Driver/Docking/hkbDockingDriver.h>
#include <Behavior/Behavior/Generator/hkbGeneratorOutput.h>
#include <Behavior/Behavior/Project/hkbProjectData.h>
#include <Behavior/Utilities/Utils/hkbProjectAssetManager.h>
#include <Behavior/Utilities/Physics/hkbPhysicsBaseTypes.h>

extern const class hkClass hkbCharacterClass;

class hkaBoneAttachment;
class hkbAnimationBindingSet;
class hkbBehaviorGraph;
class hkbBehaviorGraphInternalState;
class hkbCharacterSetup;
class hkbEventQueue;
class hkbWorld;
class hkbAiControlDriver;

/// A collection of information associated with an instance of a character.
class hkbCharacter : public hkReferencedObject
{
	//+version(3)
	public:

			/// An enum for describing the capabilities of the character
		enum CapabilityTypes
		{
			RAGDOLL = 1,
			CHARACTER_CONTROL = 1<<1,
			FOOT_IK = 1<<2,
			HAND_IK = 1<<3,
			DOCKING = 1<<4,
			MIRRORING = 1<<5,
		};
		typedef int Capabilities;

		
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );

			/// Construct with reasonable defaults.
		hkbCharacter();
		~hkbCharacter();

			/// Returns all of the rigid bodies in the world that are within maxDistance of sensorPosWS.  The default
			/// implementation uses the character's spatial query interface, falling back on the provided physicsInterface.
		virtual void getNearbyRigidBodies(
			const hkVector4& sensorPosWS,
			hkReal maxDistance,
			hkbPhysicsInterface* physicsInterface,
			hkArray<hkbRigidBodyHandle>& rigidBodies,
			hkUint32 collisionFilterInfo,
			bool excludeThisCharacter,
			bool inCharactersOnly );

			/// Returns all of the characters that are within maxDistance of this character.  The
			/// distance is between the world-from-model positions in the world.
		virtual void getNearbyCharacters( hkReal maxDistance, hkArray<hkbCharacter*>& characters );

			/// Activate the character and its behavior graph.
		void activate();
			/// Activate the character given an internal state that you would like the behavior graph to be in.
		void activateWithInternalState( const hkbBehaviorGraphInternalState& internalState );
			/// Deactivate the character and its behavior graph.
		void deactivate();
			/// Whether activate() has been called.
		bool isActive() const;

			/// Process an event right now.  It's better to put them into the character's event
			/// queue but if you want it done immediately call this.
		void handleEvent( const hkbEvent e );

			/// Gets the name of the character
		hkStringPtr getName() const;
			/// Sets the name of the character.
		void setName( const char* name );

			/// Set the character setup containing the shared character assets.
		void setSetup( hkbCharacterSetup* characterSetup );
			/// Get the character setup containing the shared character assets.
		const hkbCharacterSetup* getSetup() const;
			/// Get writable access to the character setup.  This should only be called at load and link
			/// time since the setup may be shared by many characters.
		hkbCharacterSetup* accessSetup() const;

			/// Get the hkbGeneratorOutput which stores the output of the character's behavior graph.  The
			/// generator output is only valid when the character is active.  If the character is not active
			/// HK_NULL is returned.
		hkbGeneratorOutput* getGeneratorOutput() const;

			/// Set the world-from-model transform.
		void setWorldFromModel( const hkQsTransform& worldFromModel );
			/// Get the world-from-model transform.
		const hkQsTransform& getWorldFromModel() const;
			/// Get the world-from-model transform.
		hkQsTransform& accessWorldFromModel() const;
			/// Get the local pose of the character as read only.
		const hkQsTransform* getPoseLocal() const;
			/// Get the local pose of the character.
		hkQsTransform* accessPoseLocal() const;
			/// Get the number of bones in the current pose.
		int getNumPoseLocal() const;

			/// Get the event queue that stores events to be processed later by the character.
		hkbEventQueue* getEventQueue() const;

			/// Get the animation bindings for this character.  This returns m_animationBindingSet if it is not null,
			/// and otherwise returns m_setup->m_animationBindingSet.
		const hkbAnimationBindingSet* getAnimationBindingSet() const;
			/// Access to the writable animation binding set.  You should only use this at load and link time
			/// since the animation binding set may be shared.
		hkbAnimationBindingSet* accessAnimationBindingSet();
			/// Sets the animation binding set for this character instance.  You should only call this
			/// if your individual characters have different bindings sets (normally each character type
			/// has its own animation binding set stored in m_setup).
		void setAnimationBindingSet( hkbAnimationBindingSet* animationBindingSet );

			/// Get the driver for the character ragdoll.
		hkbRagdollDriver* getRagdollDriver() const;
			/// Get the driver for the character controller.
		hkbCharacterControllerDriver* getCharacterControllerDriver() const;
			/// Get the driver for the footIk.
		hkbFootIkDriver* getFootIkDriver() const;
			/// Get the driver for the handIk.
		hkbHandIkDriver* getHandIkDriver() const;
			/// Get the docking driver.
		hkbDockingDriver* getDockingDriver() const;
			/// Get the AI control driver
		hkbAiControlDriver* getAiControlDriver() const;
			/// Set the AI control driver
		void setAiControlDriver(hkbAiControlDriver* aiControlDriver);

			/// Return the capabilities of the character
		Capabilities getCapabilities() const;
			/// Return whether the character is capable of the specified capabilities
		bool isCapableOf( Capabilities capabilities ) const;

			/// Get the world from model feedback from the footIk. 
		hkReal getWorldFromModelFeedback() const;

			/// Sets the project the character was loaded from
		void setProjectData(hkbProjectData* project);
			/// Gets the project associated with the character.
		const hkbProjectData* getProjectData() const;

			/// Sets the spatial query interface for this character.
			/// If this is not set, the hkbPhysicsInterface from the hkbWorld will be used.
			/// There are often more efficient methods of doing spatial queries if the source character is known.
		void setSpatialQueryInterface( hkbSpatialQueryInterface* spatialQueryInterface );
			/// Gets the spatial query interface associated with this character.
		hkbSpatialQueryInterface* getSpatialQueryInterface() const;

			/// Set the behavior graph of the character.
		void setBehavior( hkbBehaviorGraph* behavior );
			/// Gets the behavior graph associated with the character.
		hkbBehaviorGraph* getBehavior() const;

			/// Set the handle for the character's ragdoll.  This will update the hkbRagdollDriver.
		void setRagdollInterface(hkbRagdollInterface* ragdoll);
			/// Returns the ragdoll handle from the hkbRagdollDriver.
		hkbRagdollInterface* getRagdollInterface() const;
		
			/// Sets which world the character is in. Do not call, this is set by the hkbWorld.
		void setWorld( hkbWorld* world );
			/// Gets the world the character is in, HK_NULL if the character is not in a hkbWorld.
		hkbWorld* getWorld() const;
		
			/// Sets the asset loader that created this character
		void setAssetManager(hkbProjectAssetManager* assetManager);
			/// Gets the asset loader that created this character
		hkbProjectAssetManager* getAssetManager() const;
			/// Get the number of loaded animations
		int getNumLoadedAnimations() const;

			/// Returns a read only array of bone attachments on the character
		const hkArray<hkaBoneAttachment*>& getBoneAttachments() const;
		
			/// Gets the lua state used by the character
		void* getLuaState() const;
			/// Sets the lua state used by the character
		void setLuaState( void* luaState );
			/// Clears script state of the character
		void clearLuaState();
			/// Uses all scripts defined in the character's string data (m_luaFiles) and calls dofile to register them
		void initializeLua();
			/// Connect the character to the Havok Script debugger.  Port is defined by the world the character is in
		bool connectLuaDebugger();
			/// Disconnects the character from the Havok Script debugger.  Only one character can be connected at a time
		void disconnectLuaDebugger();
			/// Returns whether the lua debugger is currently connected
		bool isLuaDebuggerConnected() const;			
			/// Initializes an array of script files
		void initScriptFiles();
			/// Gets the filename from the function name
		hkStringPtr getFilenameFromFunction( const hkStringPtr& functionName ) const;
			/// Gets the line number from the function name
		int getLineNumberFromFunction( const hkStringPtr& functionName ) const;
			/// Get the number of Lod bones.
		int getNumLodBones() const;

		//////////////////////////////////////////////////////////////////////////
		// Deprecated Physics Implementation Specific Functions
		// Must link against hkbPhysics2012Bridge
		//////////////////////////////////////////////////////////////////////////

		/// Returns all of the rigid bodies in the world that are within maxDistance of sensorPosWS.  The default
		/// implementation calls hkpWorld::getClosestPoints() with a hkpSphereShape as input.  You may want to
		/// subclass hkbCharacter and implement this method in a way that is more efficient in your game.
		/// For example, you may want to use an hkpPhantom that follows the character around to collect
		/// nearby rigid bodies.  You may also want to assume that sensorPosWS is always near the character.
		/// But whether that assumption holds depends on how you configure your behavior graphs.
		void getNearbyRigidBodies(
			const hkVector4& sensorPosWS,
			hkReal maxDistance,
			hkbDeprecatedPhysicsWorldPtr world,
			hkArray<hkbRigidBodyHandle>& rigidBodies,
			hkUint32 collisionFilterInfo,
			bool excludeThisCharacter,
			bool inCharactersOnly );

			/// Set the handle for the character from a ragdoll pointer.
			/// This accessor will be deprecated in the future (see setRagdollInterface).
		void setRagdoll(hkbDeprecatedRagdollPtr ragdoll);
			/// Get the handle for the character as a ragdoll pointer.
			/// This accessor will be deprecated in the future (see getRagdollInterface).
		hkbDeprecatedRagdollPtr getRagdoll() const;

	public:

			/// The characters near this one.
		hkArray<hkbCharacter*> m_nearbyCharacters;

			/// This is an index into hkbCharacterData::m_numBonesPerLod, which is exported by HAT.  It indicates how many bones 
			/// should be used for the current level-of-detail.  If this is -1 (the default) then all bones are used.
		hkInt16 m_currentLod;

			/// This member provides a bound on the number of animation tracks that are extracted by the hkbClipGenerator.
			/// You can use this member along with m_currentLod (or independently) to gain more control over which
			/// bones are processed for the current LOD.  If this is -1 (the default) then all tracks that are required
			/// for the current hkbCharacterData::m_numBonesPerLod[m_currentLod] are extracted.
		hkInt16 m_numTracksInLod; //+nosave

			/// For any use you want. Not used by the Havok system.
		hkUlong	m_userData;

	private:

			/// Filters out rigid body results from the Physics system based on bool parameters
		void filterRigidBodyResults(
			hkbPhysicsInterface* physicsInterface,
			hkArray<hkbRigidBodyHandle>& rbsInOut,
			bool excludeThisCharacter,
			bool inCharactersOnly );

			/// Allocates an initializes the character's generator output.  If a hkbGeneratorOutput already exists
			/// it it deallocated.  However before deallocation, it's world from model and pose local tracks are
			/// stored and then copied to the new hkbGeneratorOutput.
		void allocateGeneratorOutput();

	private:

			/// Executes internal functionality when a character activates.
		void activateInternal( const hkbContext* context );

			/// Executes internal functionality when a character deactivates.
		void deactivateInternal( const hkbContext* context );

			/// The generator output used to store the output of the character's behavior graph.
		hkbGeneratorOutput* m_generatorOutput;	//+nosave+overridetype(void*)
		
			/// The name of this character instance.
		hkStringPtr m_name;

			// The ragdoll driver
		hkRefPtr<hkbRagdollDriver> m_ragdollDriver;	//+nosave
			// The character controller driver
		hkRefPtr<hkbCharacterControllerDriver> m_characterControllerDriver; //+nosave
			// The footIk driver
		hkRefPtr<hkbFootIkDriver> m_footIkDriver; //+nosave
			// The handIk driver
		hkRefPtr<hkbHandIkDriver> m_handIkDriver; //+nosave
			// The docking driver
		hkRefPtr<hkbDockingDriver> m_dockingDriver; //+nosave
			// The AI control driver. This must have dynamic type 
			// hkbAiControlDriver if it is not null; it has static type 
			// hkReferencedObject so that it does not depend on that header.
		hkRefPtr<hkReferencedObject> m_aiControlDriver; //+nosave

			// The character rig and other shared data.
		hkRefPtr<hkbCharacterSetup> m_setup;
			// Behavior graph for the character
		hkRefPtr<hkbBehaviorGraph> m_behaviorGraph;
			// Project for the character
		hkRefPtr<hkbProjectData> m_projectData;			
			// The animation binding set for this character if it is different from the one
			// in m_setup that is shared among all the characters of this type.
		hkRefPtr<hkbAnimationBindingSet> m_animationBindingSet;	//+nosave
		
			// Spatial query interface for this character. If the user wants the character to have its own
			// interface then this needs to be set, otherwise the hkbPhysicsInterface from the hkbWorld will be used.
		hkbSpatialQueryInterface* m_spatialQueryInterface; //+nosave
			// World the character is in.
		hkbWorld* m_world;	//+nosave +owned(false)

			/// Array of bone attachments on this character.  In general, these attachments are instances 
			/// of the attachments stored in the character's hkbCharacterData.
		hkArray<hkaBoneAttachment*> m_boneAttachments;	//+nosave			

			// A queue for events that need to be processed by the character.
		hkbEventQueue* m_eventQueue; //+nosave+overridetype(void*)
					
			/// The character's Lua state
		void* m_characterLuaState; //+nosave

			/// The asset manager this character was loaded with
		hkRefPtr<hkbProjectAssetManager> m_assetManager; //+nosave
					
	public:

		hkbCharacter( hkFinishLoadedObjectFlag flag );
};

#include <Behavior/Behavior/Character/hkbCharacter.inl>

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
