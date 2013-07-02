/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_BEHAVIOR_CLIENT_H
#define HKB_BEHAVIOR_CLIENT_H

#include <Behavior/Behavior/Event/hkbEvent.h>
#include <Behavior/Utilities/VisualDebugger/hkbVdbCommands.h>

#include <Common/Base/Container/StringMap/hkStringMap.h>
#include <Common/Visualize/Process/hkRemoteObjectProcess.h>

extern const class hkClass hkbClientCharacterStateClass;

class hkbHistory;
class hkbGenerator;
class hkbBehaviorClient;
class hkbActiveNodeHistory;
class hkbBehaviorClientState;
class hkbBehaviorGraphInternalState;

	/// You can use this to receive interesting news from the hkbBehaviorClient.
class hkbBehaviorClientListener
{
	public:			
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR,hkbBehaviorClientListener);
		virtual ~hkbBehaviorClientListener() {}
		
			/// A character has been added.
		virtual void onCharacterAdded( hkbBehaviorClient* client, hkUint64 characterId, const char* name ) {}
			/// A character has been removed.
		virtual void onCharacterRemoved( hkbBehaviorClient* client, hkUint64 characterId, const char* name ) {}
			/// A character has been hidden.
		virtual void onCharacterHidden( hkbBehaviorClient* client, hkUint64 characterId, const char* name ) {}
			/// A character has been shown.
		virtual void onCharacterShown( hkbBehaviorClient* client, hkUint64 characterId, const char* name ) {}
			/// A character has been activated.
		virtual void onCharacterActivated( hkbBehaviorClient* client, hkUint64 characterId, const char* name ) {}
			/// A character has been deactivated.
		virtual void onCharacterDeactivated( hkbBehaviorClient* client, hkUint64 characterId, const char* name ) {}
			/// A character's skin has changed.
		virtual void onCharacterSkinChanged( hkbBehaviorClient* client, hkUint64 characterId ) {}
			/// A character's behavior graph has changed.
		virtual void onCharacterBehaviorChanged( hkbBehaviorClient* client, hkUint64 characterId, const char* name, const hkbBehaviorInfo& info ) {}
			/// The character has been stepped.
		virtual void OnCharacterStepped( hkbBehaviorClient* client, hkUint64 characterId, float deltaTime ) {}
			/// The selected character has been changed
		virtual void onCharacterSelectedChanged( hkbBehaviorClient* client, hkUint64 characterId, int port ) {}

			/// A character's playback mode changed.
		virtual void onSimulationStateChanged( hkbBehaviorClient* client, hkbWorldEnums::SimulationState state ) {}
			/// The events for the character's behavior changed.
		virtual void onBehaviorEventsChanged( hkbBehaviorClient* client, hkUint64 characterId, const char* name ) {}
		
			/// Event has been raised.
		virtual void onEventRaised( hkbBehaviorClient* client, hkUint64 characterId, const char* name, bool wasRaisedBySdk, int senderNodeId  ) {}
			/// The value of a variable has changed.
		virtual void onVariableValueChanged( hkbBehaviorClient* client, hkUint64 characterId, int variableId ) {}
			/// The active nodes in a behavior have changed.
		virtual void onActiveNodesChanged( hkbBehaviorClient* client, hkUint64 characterId ) {}
			/// The internal state of the behavior graph has changed.
		virtual void onBehaviorInternalStateChanged( hkbBehaviorClient* client, hkUint64 characterId, hkbBehaviorGraphInternalState* internalState ) {}
};

/// The state of a hkbCharacter from the hkbBehaviorClient's point of view
class hkbClientCharacterState: public hkReferencedObject
{	
	public:
		//+version(2)
		//+vtable(true)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_VDB );

			/// Initializes the character state with the unique ID of the character.
		hkbClientCharacterState( hkUint64 characterId );
		hkbClientCharacterState( hkFinishLoadedObjectFlag flag );
		~hkbClientCharacterState();

			/// Array of display IDs used for the character's deformable skins. 
		hkArray<hkUint64> m_deformableSkinIds;
			/// Array of display IDs used for the character's rigid skins. 
		hkArray<hkUint64> m_rigidSkinIds;
			/// A list of indices into the global event table of events that are used by this behavior (including nested behaviors).
		hkArray<hkInt16> m_externalEventIds;
			/// A list of auxiliary node info.
		hkArray< hkRefPtr<hkbAuxiliaryNodeInfo> > m_auxiliaryInfo;
			/// A list of indices into the global event table of events that can cause a state change right now. 
		hkArray<hkInt16> m_activeEventIds;
			/// A list of indices into the character's variables of variables that affect (or are affected by) the active nodes.
		hkArray<hkInt16> m_activeVariableIds;
			/// Unique id for the character.  This is the address of the character on the server.
		hkUint64 m_characterId;
			/// The name of the character instance.
		hkStringPtr m_instanceName;
			/// The name of the character type from its hkbCharacterStringData::m_name.
		hkStringPtr m_templateName;
			/// The full path to the HKP file.
		hkStringPtr m_fullPathToProject;
			/// Full path to script
		hkStringPtr m_localScriptsPath;
			/// Path to script relative to local scripts path
		hkStringPtr m_remoteScriptsPath;
			/// Data describing the character's behavior graph.  If HK_NULL the character does not
			/// have a behavior assigned.
		hkRefPtr<hkbBehaviorGraphData> m_behaviorData;
			/// The current values of the variables.
		hkRefPtr<hkbBehaviorGraphInternalState> m_behaviorInternalState;
			/// We need to be able to efficiently look up the internal state of each node.
		hkPointerMap<hkUint16,hkReferencedObject*>* m_nodeIdToInternalStateMap; //+nosave		
			/// Whether or not the character is currently being drawn and/or debugged.
		hkBool m_visible;
			/// Time since the character started simulation.
		hkReal m_elapsedSimulationTime;		
			/// The skeleton of the character
		hkRefPtr<hkaSkeleton> m_skeleton;
			/// The world from model transform of the character.
		hkQsTransform m_worldFromModel;	
			/// The model space pose of the character.
		hkArray<hkQsTransform> m_poseModelSpace;
			/// The model space transform of each of the character's rigid attachments
		hkArray<hkQsTransform> m_rigidAttachmentTransforms;	    
};

	/// hkbBehaviorClient maintains a scaled down representation of a remote hkbWorld.  It can also
	/// control a remote hkbWorld by sending it commands.	
class hkbBehaviorClient : public hkReferencedObject, public hkRemoteObjectClientSideListener
{	
	public:
		
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		
			/// Creates the client.  sendFunc should be set to hkVdbSendObject.
		hkbBehaviorClient(hkRemoteObjectClientSideListener::SendObject sendFunc);
			/// Destroys the client.
		virtual ~hkbBehaviorClient();					
				
			/// Start or unpause simulation of all characters.
		void playAll();
			/// Pause simulation of all characters.
		void pauseAll();
			/// Stops the simulation.  After being stopped the simulation is considered to be at the beginning.
		void stopAll();

			/// Returns whether the simulation is playing, paused, or stepping.
		hkbWorldEnums::SimulationState getSimulationState();
			/// Set if all characters should move according to the motion in their behavior graph.
		void accumulateMotionAll( bool accumulate );

			/// Returns the external ID of an event given the name.
		int getEventIdFromName( const char* eventName ) const;
			/// Enqueues an event for a specific character.
		void enqueueEvent( hkUint64 characterId, hkbEvent event );
			/// Enqueues an event for all characters.
		void enqueueEventForAll( hkbEvent event );

			/// Returns the global ID of a variable given the name.
		int getVariableGlobalIdFromName( const char* variableName ) const;
			/// Returns the external ID of a variable given the name and a character.
		int getVariableIdFromName( hkUint64 characterId, const char* variableName ) const;

			/// Sets the value of a word sized variable for a specific character.  
		void setVariableValueWord( hkUint64 characterId, const char* variableName, const hkbVariableValue& value, hkbVariableInfo::VariableType type );
			/// Sets the value of a word sized variable for all characters.
		void setVariableValueWordForAll( const char* variableName, const hkbVariableValue& value, hkbVariableInfo::VariableType type );
		/// Sets the value of a quad sized variable for a specific character.  
		void setVariableValueQuad( hkUint64 characterId, const char* variableName, const hkVector4& value, hkbVariableInfo::VariableType type );
		/// Sets the value of a quad sized variable for all characters.
		void setVariableValueQuadForAll( const char* variableName, const hkVector4& value, hkbVariableInfo::VariableType type );

			/// Sends camera variable lists of variable names and values for floats and vectors to the server
		void updateCameraVariables( const hkArray<hkStringPtr>& cameraFloatNames, const hkArray<hkStringPtr>& cameraVectorNames, const hkArray<hkReal>& cameraFloatValues, const hkArray<hkVector4>& cameraVectorValues );

			/// Replace the behavior graph for the character with the one provided.  If root generator is non NULL it
			/// will replace the behaviors default root generator.  The root generator must be child of behavior.
			/// If the root generator is a state machine then it will be set to the state at index startStateIndex.
			/// If randomizeSimulation is true, the random number generator will be re-seeded.
		void setBehavior( hkUint64 characterId, hkbBehaviorGraph* behavior, hkbGenerator* rootGenerator, int startStateIndex = -1, bool randomizeSimulation = false );
			/// Set the local time of a clip generator assigned to the character.  Assumes 
			/// the only active node is a single clip generator.  If there is not a single
			/// clip generator active nothing is done.
		void setLocalTimeOfClipGenerator( hkUint64 characterId, hkUint16 nodeId, hkReal localTime );
			/// Sets the value of a property on a node.
		void setNodeProperty( hkUint64 characterId, const char* nodeName, const char* propertyName, hkbVariableValue propertyValue );

			/// Add a listener.
		void addListener( hkbBehaviorClientListener* listener );
			/// Remove a listener.
		void removeListener( hkbBehaviorClientListener* listener );

			/// Get the number of characters for which we have state information.
		int getNumCharacters() const;
			/// Get the hkbClientCharacterState for a character by index.
		hkbClientCharacterState* getCharacterStateByIndex( int index ) const;
			/// Returns the hkbClientCharacterState with the given ID or HK_NULL if not found.
		hkbClientCharacterState* getCharacterStateById( hkUint64 id ) const;
			/// Delete all information about all characters.
		void clearCharacters();

			/// Gets the id of the active character in the client
		hkUint64 getSelectedCharacterId() const;

			/// Show a character (draw the skin and receive debug messages)
		void showCharacter( hkUint64 characterId );
			/// Hide a character (don't draw the skin or receive debug messages)
		void hideCharacter( hkUint64 characterId );		
			/// Connect to a specific character
		void selectCharacter(hkUint64 characterId);

			/// Rebuilds scripts for all characters
		void rebuildScriptsAll();
			/// Rebuilds scripts for a specific character
		void rebuildScripts(hkUint64 characterId);

			/// Computes the world space aabb of a character
		void computeAabb( hkUint64 id, hkAabb& aabb );
			/// Computes the world space transform of a bone on a character
		void computeWorldSpaceTransform( hkUint64 id, hkUint16 boneIndex, hkQsTransform& transform );

			/// Returns the symbols that resulted from linking all of the characters.
		const hkbLinkedSymbolInfo* getLinkedSymbolInfo() const;

			/// Returns the internal state of a node.
		hkReferencedObject* getNodeInternalState( hkUint64 characterId, hkUint16 nodeId );

			/// Orders an AI-controlled character to path to a given point.
		void aiControlPathTo( hkUint64 id, hkVector4Parameter goalPoint );

			/// Orders an AI-controlled character to stop pathing.
		void aiControlCancelPath( hkUint64 id );

			/// hkRemoteObjectListener interface.  Detects type and forwards to handlers below.
		virtual void receiveObjectCallback( hkReferencedObject* object, const hkClass* klass ) HK_OVERRIDE;
					
	private:	
			
			//
			// Each handler deals with different types of data coming from the
			// server. Typically the handlers cache data locally in the client 
			// and then raise listeners to notify other objects that data has 
			// changed.
			//		
					
		void handleCharacterAddedInfo( const hkbCharacterAddedInfo& info );
		void handleCharacterInfo( const hkbCharacterInfo& info );			
		void handleCharacterSteppedInfo(const hkbCharacterSteppedInfo& info);
		void handleBehaviorInfo( const hkbBehaviorInfo& info );
		void handleCharacterSkinInfo( const hkbCharacterSkinInfo& info );
		void handleEventRaisedInfo( const hkbEventRaisedInfo& info );
		void handleBehaviorGraphInternalStateInfo( const hkbBehaviorGraphInternalStateInfo& info );
		void handleSimulationStateInfo( const hkbSimulationStateInfo& info );
		void handleLinkedSymbolInfo( const hkbLinkedSymbolInfo& info );
		void handleBehaviorEventsInfo( const hkbBehaviorEventsInfo& info );
		void handleCharacterSelectedInfo( const hkbCharacterSelectedInfo& info );
					
	private:
							
			// Array of characters the client knows about
		hkArray< hkRefPtr<hkbClientCharacterState> > m_characters;
		
			/// Listeners attached to the client
		hkArray<hkbBehaviorClientListener*> m_listeners;

			/// The names of all of the events in the symbol table that is used to link all behaviors.
		hkRefPtr<const hkbLinkedSymbolInfo> m_linkedSymbolInfo;

			/// Maps the names of events to their external IDs (built from m_linkedSymbolInfo).
		hkStringMap<int> m_eventNameToIdMap;

			/// Maps the names of variables to their external IDs (built from m_linkedSymbolInfo).
		hkStringMap<int> m_variableNameToIdMap;

			/// Whether the simulation is playing, paused, or stepping.
		hkEnum<hkbWorldEnums::SimulationState, hkUint8> m_simulationState;

			/// The id of the active character in the client.  This is maintained by the client and
			/// not by messages from the server.
		hkUint64 m_selectedCharacterId;
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
