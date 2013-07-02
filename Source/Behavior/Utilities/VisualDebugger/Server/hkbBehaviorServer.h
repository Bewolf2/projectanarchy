/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_BEHAVIOR_SERVER_H
#define HKB_BEHAVIOR_SERVER_H


#include <Common/Visualize/Process/hkRemoteObjectProcess.h>
#include <Behavior/Behavior/Variables/hkbVariableInfo.h>
#include <Behavior/Utilities/VisualDebugger/hkbBehaviorContext.h>
#include <Behavior/Utilities/VisualDebugger/Viewer/hkbWorldViewerBase.h>
#include <Common/Base/System/Stopwatch/hkStopwatch.h>

class hkbBehaviorGraph;
class hkbCharacterControlCommand;
class hkbGenerator;
class hkbStateMachine;
class hkbRaiseEventCommand;
class hkbSetBehaviorCommand;
class hkbSetCurrentStateCommand;
class hkbSetLocalTimeOfClipGeneratorCommand;
class hkbSetNodePropertyCommand;
class hkbSetWordVariableCommand;
class hkbSimulationControlCommand;
class hkbVariableValue;
class hkbWatchNodeCommand;
class hkbAuxiliaryNodeInfo;
class hkbSetSelectedCharacterCommand;
class hkbRebuildScriptsCommand;
class hkbCameraVariablesChangedCommand;
class hkbAiControlPathToCommand;
class hkbAiControlCancelPathCommand;

	/// simple struct for temporary storage of active self transitions
class hkbActiveSelfTransitions
{
    public:
    
	    HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VDB, hkbActiveSelfTransitions);
    
	    hkStringPtr m_transitionName;
	    hkbStateMachine* m_stateMachine;
	    hkbCharacter* m_character;
	    int m_remainingFrames;
};

	/// hkbBehaviorServer is a viewer that controls characters based on commands 
	/// received from a hkbBehaviorClient.  It also sends information when hkbWorlds
	/// and hkbCharacters change.
class hkbBehaviorServer : public hkbWorldViewerBase						  
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_VDB );
		
			/// Creates the viewer.
		hkbBehaviorServer( const hkArray<hkProcessContext*>& contexts );
			/// Destroys the viewer
		~hkbBehaviorServer();

			/// hkProcess interface
		virtual int getProcessTag() HK_OVERRIDE { return m_tag; }
			
			/// hkbWorldViewerBase interface
		virtual void worldAddedCallback( hkbWorld* world ) HK_OVERRIDE;
			/// hkbWorldViewerBase interface
		virtual void showCharacter( hkbCharacter* character ) HK_OVERRIDE;
			/// hkbWorldViewerBase interface
		virtual void hideCharacter( hkbCharacter* character ) HK_OVERRIDE;

			//
			// Each method in the hkbWorldListener sends info to clients to
			// describing what happened because of the callback.  For example,
			// when characterBehaviorChanged is called a hkbBehaviorInfo object
			// is sent that describes the hkbCharacter's new behavior graph.
			//
			
			/// hkbBehaviorContextWorldListener interface.
		virtual void characterAddedToContextCallback( hkbCharacter* character ) HK_OVERRIDE;
			/// hkbBehaviorContextWorldListener interface.
		virtual void characterRemovedFromContextCallback( hkbCharacter* character ) HK_OVERRIDE;
		
			/// hkbWorldListener interface. 
		virtual void activeNodesChangedCallback( hkbCharacter* character ) HK_OVERRIDE;
			/// hkbWorldListener interface.
		virtual void characterActivatedCallback( hkbCharacter* character ) HK_OVERRIDE;
			/// hkbWorldListener interface.
		virtual void characterDeactivatedCallback( hkbCharacter* character ) HK_OVERRIDE;
			/// hkbWorldListener interface.
		virtual void eventRaisedCallback( hkbCharacter* character, const hkbEvent& event, bool raisedBySdk ) HK_OVERRIDE;		
			/// hkbWorldListener interface.
		virtual void postGenerateCallback( hkbCharacter* character, hkReal deltaTime ) HK_OVERRIDE;
			/// hkbWorldListener interface.
		virtual void postLinkCallback( hkbCharacter* character, hkbBehaviorGraph* templateBehavior ) HK_OVERRIDE;
			
			/// hkRemoteObjectServerSideListener.  This callback is raised when the server receives
			/// and object from a client.  Objects are typically commands and are dispatched to one
			/// of the "handle" methods below.
		virtual void receiveObjectCallback( hkReferencedObject* object, const hkClass* klass ) HK_OVERRIDE;
			
			/// Creates the singleton instances of the process.  Invoked by the hkProcessFactory.
		static hkProcess* HK_CALL create(const hkArray<hkProcessContext*>& contexts);
			/// Registers the process with hkProcessFactory.
		static void HK_CALL registerProcess();
			/// Gets the name of the process
		static inline const char* HK_CALL getName() { return "BehaviorServer"; }

	protected:				

			//
			// Each handler interprets and executes a command.  Typically a character,
			// or behavior is looked up and changed in some way.  Sometimes infos are
			// sent back to the client to acknowledge the command was received. 
			//
		
		void handleSimulationControlCommand( hkbSimulationControlCommand& command );		
		void handleRaiseEventCommand( hkbRaiseEventCommand& command );			
		void handleSetWordVariableCommand( hkbSetWordVariableCommand& command );
		void handleWatchNodeCommand( hkbWatchNodeCommand& command );
		void handleSetLocalTimeOfClipGeneratorCommand( hkbSetLocalTimeOfClipGeneratorCommand& command );
		void handleSetBehaviorCommand( hkbSetBehaviorCommand& command );		
		void handleSetNodePropertyCommand( hkbSetNodePropertyCommand& command );
		void handleSetSelectedCharacterCommand( hkbSetSelectedCharacterCommand& command );
		void handleRebuildScriptsCommand( hkbRebuildScriptsCommand& command );
		void handleCameraVariablesChangedCommand( hkbCameraVariablesChangedCommand& command );
		void handleAiControlPathToCommand( hkbAiControlPathToCommand& command );
		void handleAiControlCancelPathCommand( hkbAiControlCancelPathCommand& command );
		
			/// Sets the value of a variable in a character's behavior graph.
		void setWordVariableValue( hkbBehaviorGraph* behavior, const hkbVariableValue& value, hkbVariableInfo::VariableType type, hkInt32 externalId );

			/// Sets the value of a quad variable in a character's behavior graph.
		void setQuadVariableValue( hkbBehaviorGraph* behavior, const hkVector4& value, hkbVariableInfo::VariableType type, hkInt32 externalId );
		
			/// Fills an array with data that is auxiliary to the node internal state data.
		void getAuxiliaryData( hkbCharacter* character, hkArray< hkRefPtr<hkbAuxiliaryNodeInfo> >& auxiliaryInfo );

			/// Send an hkbBehaviorInfo for the character.
		void sendBehaviorInfo( hkbCharacter* character, hkbBehaviorGraph* templateBehavior );

			/// Send an hkbBehaviorEventsInfo for the character.
		void sendBehaviorEventsInfo( hkbWorld* world, hkbCharacter* character, hkbBehaviorGraph* templateBehavior );

			/// Send an hkbSimulationStateInfo indicating the simulation state of the world.
		void sendSimulationStateInfo();

			/// Send an hkbBehaviorGraphInternalStateInfo indicating the internal state of the behavior graph of the character.
		void sendBehaviorGraphInternalStateInfo( hkbCharacter* character );			

			/// Loads clips that the behavior references.  Returns true if all animation could 
			/// be loaded, false otherwise.
		bool loadClips( hkbCharacter* character, hkbBehaviorGraph* templateBehavior );					

			/// Activates a character and sets its start state to hkbCharacterDebugData::m_startStateIndex.
		void activateCharacterAndSetStartState( hkbCharacter* character );

	protected:
			/// Unique ID for the viewer
		static int m_tag;

			/// An array for storing active self transitions
		hkArray<hkbActiveSelfTransitions> m_activeSelfTransitions;

			/// Which character is selected
		hkbCharacter* m_selectedCharacter;

			/// Is selected character connected to Lua
		hkBool m_isSelectedCharacterDebuggable;
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
