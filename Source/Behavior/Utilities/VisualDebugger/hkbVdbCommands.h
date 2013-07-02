/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_BEHAVIOR_COMMANDS_H
#define HK_BEHAVIOR_COMMANDS_H

#include <Behavior/Behavior/Character/hkbCharacter.h>
#include <Behavior/Behavior/Generator/hkbGenerator.h>
#include <Behavior/Behavior/Variables/hkbVariableInfo.h>
#include <Behavior/Behavior/Variables/hkbVariableValueSet.h>
#include <Behavior/Behavior/BehaviorGraph/hkbBehaviorGraph.h>
#include <Behavior/Behavior/BehaviorGraph/hkbBehaviorGraphData.h>
#include <Behavior/Behavior/BehaviorGraph/hkbBehaviorGraphInternalState.h>
#include <Behavior/Behavior/World/hkbWorld.h>

class hkbGenerator;

// commands
extern const class hkClass hkbCharacterControlCommandClass;
extern const class hkClass hkbRaiseEventCommandClass;
extern const class hkClass hkbSetWordVariableCommandClass;

// infos
extern const class hkClass hkbActiveNodesInfoClass;
extern const class hkClass hkbBehaviorEventsInfoClass;
extern const class hkClass hkbBehaviorGraphInternalStateInfoClass;
extern const class hkClass hkbBehaviorInfoClass;
extern const class hkClass hkbCharacterInfoClass;
extern const class hkClass hkbCharacterSimulationStateInfoClass;
extern const class hkClass hkbCharacterSkinInfoClass;
extern const class hkClass hkbEventRaisedInfoClass;
extern const class hkClass hkbLinkedSymbolInfoClass;
extern const class hkClass hkbVariableValueInfoClass;
extern const class hkClass hkbCharacterSelectedInfoClass;
extern const class hkClass hkbCharacterSteppedInfoClass;

//
// Commands originate on a client and instruct a server to do something.  For 
// example, the hkbSimulationControlCommand can pause and unpause the simulation. 
// Commands are typically lightweight.
// 

	/// Pauses, unpauses, or steps the simulation.  Turns on and off the accumulation of motion.
class hkbSimulationControlCommand : public hkReferencedObject
{
	public:
		//+vtable(true)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_VDB );

		hkbSimulationControlCommand() {}
		hkbSimulationControlCommand( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag){}

		enum SimulationControlCommand
		{
			COMMAND_PLAY,
			COMMAND_PAUSE,
			COMMAND_STEP,
			COMMAND_STOP,
			COMMAND_ACCUMULATE_MOTION,
			COMMAND_DO_NOT_ACCUMULATE_MOTION,
		};

		hkEnum<SimulationControlCommand, hkUint8> m_command;
};

	/// Changes the playback mode of a character.
class hkbCharacterControlCommand: public hkReferencedObject
{
	public:
		//+version(1)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_VDB );

		hkbCharacterControlCommand() {}
		hkbCharacterControlCommand( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag){}
		
		enum CharacterControlCommand
		{
			COMMAND_HIDE,
			COMMAND_SHOW,			
		};

		hkUint64 m_characterId;
		hkEnum<CharacterControlCommand, hkUint8> m_command;
		int m_padding;
};

	/// Raises an event for a character.
class hkbRaiseEventCommand: public hkReferencedObject
{
	public:
		//+vtable(true)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_VDB );
		
		hkbRaiseEventCommand() {}
		hkbRaiseEventCommand( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag){}
		
		hkUint64 m_characterId;
		hkBool m_global;
		hkInt32 m_externalId;
};

/// Sets which character is the activate character.
class hkbSetSelectedCharacterCommand: public hkReferencedObject
{
	public:
		//+vtable(true)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_VDB );

		hkbSetSelectedCharacterCommand() {}
		hkbSetSelectedCharacterCommand( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag){}

		hkUint64 m_characterId;
};

	/// Sets the value of a word sized variable
class hkbSetWordVariableCommand: public hkReferencedObject
{
	public:
		//+vtable(true)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_VDB );
		
		hkbSetWordVariableCommand() {}
		hkbSetWordVariableCommand( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag), m_variableName(flag) {}
		
		hkUint64 m_characterId;
		
		hkStringPtr m_variableName;
		class hkbVariableValue m_value;
		hkVector4 m_quadValue;
		hkEnum<hkbVariableInfo::VariableType, hkUint8> m_type;
		hkBool m_global;
};

	/// Command that sets the local time of a clip generator.  Used by the behavior tool to
	/// cycle through animations in different editors.
class hkbSetLocalTimeOfClipGeneratorCommand: public hkReferencedObject
{
	public:
		//+vtable(true)		
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_VDB );

		hkbSetLocalTimeOfClipGeneratorCommand() {}
		hkbSetLocalTimeOfClipGeneratorCommand( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag) {}

		hkUint64 m_characterId;		
		hkReal m_localTime;
		hkUint16 m_nodeId;
};

	/// Set the behavior graph for a character.
class hkbSetBehaviorCommand : public hkReferencedObject
{
	public:
		//+version(1)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_VDB );

		hkbSetBehaviorCommand() : m_startStateIndex(-1) {}
		hkbSetBehaviorCommand( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag), m_behavior(flag), m_rootGenerator(flag), m_referencedBehaviors(flag) {}
		
		hkUint64 m_characterId;
		hkRefPtr<hkbBehaviorGraph> m_behavior;
		hkRefPtr<hkbGenerator> m_rootGenerator;
		hkArray< hkRefPtr<hkbBehaviorGraph> > m_referencedBehaviors;
		hkInt32 m_startStateIndex;
		hkBool m_randomizeSimulation;
		int m_padding;
};

class hkbSetNodePropertyCommand : public hkReferencedObject
{
	public:
		//+version(1)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_VDB );

		hkbSetNodePropertyCommand() {}
		hkbSetNodePropertyCommand( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag), m_nodeName(flag), m_propertyName(flag) {}

		hkUint64 m_characterId;
		hkStringPtr m_nodeName;
		hkStringPtr m_propertyName;
		hkbVariableValue m_propertyValue;
		int m_padding;
};

	/// Command to let server know that the character needs their scripts rebuilt.
class hkbRebuildScriptsCommand : public hkReferencedObject
{
	public:
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_VDB );

		hkbRebuildScriptsCommand() {}
		hkbRebuildScriptsCommand( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag) {}

		hkUint64 m_characterId;
};

/// Global info - change camera variables
class hkbCameraVariablesChangedCommand : public hkReferencedObject
{
	public:
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_VDB );

		hkbCameraVariablesChangedCommand() {}
		hkbCameraVariablesChangedCommand( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag),
				m_cameraVariableFloatNames(flag), m_cameraFloatValues(flag), m_cameraVariableVectorNames(flag), m_cameraVectorValues(flag) {}

		hkArray<hkStringPtr> m_cameraVariableFloatNames;
		hkArray<hkReal> m_cameraFloatValues;

		hkArray<hkStringPtr> m_cameraVariableVectorNames;
		hkArray<hkVector4> m_cameraVectorValues;
};

class hkbAiControlPathToCommand : public hkReferencedObject
{
public:
	HK_DECLARE_REFLECTION();
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_VDB );

	hkbAiControlPathToCommand() {}
	hkbAiControlPathToCommand( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag) {}

	hkUint64 m_characterId;
	hkVector4 m_goalPoint;
};

class hkbAiControlCancelPathCommand : public hkReferencedObject
{
public:
	HK_DECLARE_REFLECTION();
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_VDB );

	hkbAiControlCancelPathCommand() {}
	hkbAiControlCancelPathCommand( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag) {}

	hkUint64 m_characterId;
};

//
// Infos originate on the server and tell clients about the server state. Clients
// use this information to invoke commands.  Infos are typically data heavy.
//

	/// Wrapper to hold enumeration.  Required to reflect the enumeration to the managed wrapper
class hkbToolNodeType
{
	public:
		HK_DECLARE_REFLECTION();
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(hkbToolNodeType, HK_MEMORY_CLASS_VDB);

		enum NodeType
		{
			NODE_TYPE_UNKNOWN = 0x0,
			NODE_TYPE_STATE_MACHINE = 0x1,
			NODE_TYPE_CLIP = 0x2,
			NODE_TYPE_BLEND = 0x4,
			NODE_TYPE_MODIFIER = 0x8,
			NODE_TYPE_GENERATOR = 0x10,
			NODE_TYPE_MODIFIER_GENERATOR = 0x20,
			NODE_TYPE_TRANSITION_EFFECT = 0x40,
			NODE_TYPE_BEHAVIOR_FILE_REFERENCE = 0x80,
		};
		
		/// Gets the type from a hkbNode
	static NodeType HK_CALL getNodeType( const hkbNode& node );
};

	/// Describes a character
class hkbCharacterAddedInfo: public hkReferencedObject
{
	public:
		//+version(1)
		//+vtable(1)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_VDB );		

		hkbCharacterAddedInfo() {}
		hkbCharacterAddedInfo( hkFinishLoadedObjectFlag flag ) :
			hkReferencedObject(flag), m_instanceName(flag), m_templateName(flag),
			m_fullPathToProject(flag), m_localScriptsPath(flag), m_remoteScriptsPath(flag),
			m_skeleton(flag), m_poseModelSpace(flag) {}
		
		hkUint64 m_characterId;

			/// The name of this character instance.
		hkStringPtr m_instanceName;
			/// The name of the character as defined in its hkbCharacterStringData::m_name.
		hkStringPtr m_templateName;

			/// The path to the HKP file from hkbProjectStringData::m_fullPathToSource.  The same project must
			/// contain the character and any behaviors that will be assigned to it if you want HAT to properly debug it.
		hkStringPtr m_fullPathToProject;

			/// The path to the script data
		hkStringPtr m_localScriptsPath;
			/// The path relative to local scripts path on the remote machine
		hkStringPtr m_remoteScriptsPath;

			/// The skeleton used by the character
		hkRefPtr<hkaSkeleton> m_skeleton;

			/// The characters initial position
		hkQsTransform m_worldFromModel;
			/// The characters initial pose
		hkArray<hkQsTransform> m_poseModelSpace;
			
};

	/// Describes a simple event related to the character.
class hkbCharacterInfo: public hkReferencedObject
{
	public:
		//+version(2)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_VDB );		

		hkbCharacterInfo() {}
		hkbCharacterInfo( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag) {}

		enum Event
		{
			REMOVED_FROM_WORLD,
			SHOWN,
			HIDDEN,
			ACTIVATED,
			DEACTIVATED,
		};

		hkUint64 m_characterId;
		hkEnum<Event, hkUint8> m_event;
		int m_padding;
};

	/// Sent when a character is stepped
class hkbCharacterSteppedInfo: public hkReferencedObject
{
	public:		
		//+version(2)
		//+vtable(1)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_VDB );

		hkbCharacterSteppedInfo() {}
		hkbCharacterSteppedInfo( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag), m_poseModelSpace(flag), m_rigidAttachmentTransforms(flag) {}

		hkUint64 m_characterId;
		hkReal m_deltaTime;

		hkQsTransform m_worldFromModel;
		hkArray<hkQsTransform> m_poseModelSpace;
		hkArray<hkQsTransform> m_rigidAttachmentTransforms;
};

	/// Lists the IDs of the display objects that represent a character's skins
class hkbCharacterSkinInfo: public hkReferencedObject
{
	public:
		//+version(2)
		//+vtable(1)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_VDB );

		hkbCharacterSkinInfo() {}
		hkbCharacterSkinInfo( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag), m_deformableSkins(flag), m_rigidSkins(flag) {}
		
		hkUint64 m_characterId;
		hkArray<hkUint64> m_deformableSkins;
		hkArray<hkUint64> m_rigidSkins;
};

	/// Describes a character's hkbBehaviorGraph.
class hkbBehaviorInfo : public hkReferencedObject
{
	public:
		//+vtable(1)
		//+version(1)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_VDB );

		hkbBehaviorInfo() {}
		hkbBehaviorInfo( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag), m_data(flag), m_idToNamePairs(flag) {}
				
			/// A structure used to link a node object in the behavior tool with a 
			/// node object in the behavior SDK.
		struct IdToNamePair
		{
			//+version(1)
			HK_DECLARE_REFLECTION();
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_VDB, hkbBehaviorInfo::IdToNamePair );

			IdToNamePair() {}
			IdToNamePair( hkFinishLoadedObjectFlag flag ) : m_behaviorName(flag), m_nodeName(flag) {}

				/// Initializes the pair from a node
			void initFromNode( const hkbBehaviorGraph& behavior, const hkbNode& node );				

				/// Name of the behavior the node belongs to
			hkStringPtr m_behaviorName;
				/// Name of the node
			hkStringPtr m_nodeName;
				/// Type of object in the Animation Tool.
			hkEnum<hkbToolNodeType::NodeType, hkUint16> m_toolType;
				/// The SDK unique id of the node
			hkInt16 m_id;

		};

		hkUint64 m_characterId;
		hkRefPtr<hkbBehaviorGraphData> m_data;	
		hkArray<IdToNamePair> m_idToNamePairs;
};

	/// After linking, this is sent to indicate which external events are used by a behavior.
class hkbBehaviorEventsInfo : public hkReferencedObject
{
	public:
		//+version(1)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_VDB );

		hkbBehaviorEventsInfo() {}
		hkbBehaviorEventsInfo( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag), m_externalEventIds(flag) {}

		hkUint64 m_characterId;

		// A list of indices into the global event table of events that are used by this behavior (including nested behaviors).
		// Note that the linking system does not create this information by default, since the root behavior does not 
		// need to know about events in nested behaviors.
		hkArray<hkInt16> m_externalEventIds;
		int m_padding;
};

	/// Describes an event raised by a character.
class hkbEventRaisedInfo: public hkReferencedObject
{
	public:
		//+version(2)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_VDB );

		hkbEventRaisedInfo() {}
		hkbEventRaisedInfo( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag), m_eventName(flag) {}
		
		hkUint64 m_characterId;
		hkStringPtr m_eventName;
		hkBool m_raisedBySdk;
		int m_senderId;
		int m_padding;
};

	/// Describes the simulation state of a character
class hkbSimulationStateInfo : public hkReferencedObject
{	
	public:
		//+vtable(true)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_VDB );

		hkbSimulationStateInfo() {}
		hkbSimulationStateInfo( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag) {}

		hkEnum<hkbWorldEnums::SimulationState, hkUint8> m_simulationState;	
};

	/// The names of all of the events in the symbol table that is used to link all behaviors.
class hkbLinkedSymbolInfo : public hkReferencedObject
{
	public:
		//+vtable(true)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_VDB );

		hkbLinkedSymbolInfo() {}
		hkbLinkedSymbolInfo( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag), m_eventNames(flag), m_variableNames(flag) {}

		hkArray<hkStringPtr> m_eventNames;
		hkArray<hkStringPtr> m_variableNames;
};

	/// Additional information about nodes which may not be contained in their internal state
class hkbAuxiliaryNodeInfo: public hkReferencedObject
{	
	public:
	    //+version(2)
		//+vtable(true)
		HK_DECLARE_REFLECTION();
	    HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VDB);
    
		hkbAuxiliaryNodeInfo(): m_type(hkbToolNodeType::NODE_TYPE_UNKNOWN), m_selfTransitionNames() {}
	    hkbAuxiliaryNodeInfo( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag), m_referenceBehaviorName(flag), m_selfTransitionNames(flag) {}
    
	    	/// Type
		hkEnum<hkbToolNodeType::NodeType, hkUint16> m_type;
    
	   		/// Depth of node in the active node list
	    hkUint8 m_depth;
    
	    	/// The name of the parent reference node
	    hkStringPtr m_referenceBehaviorName;

			/// A list of names of transition effects for active self transitions.  This should only be non-empty for state machine nodes
		hkArray<hkStringPtr> m_selfTransitionNames;
};

	/// The internal state of a behavior graph.
class hkbBehaviorGraphInternalStateInfo : public hkReferencedObject
{
	public:
		//+version(1)
		//+vtable(true)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_VDB );

		hkbBehaviorGraphInternalStateInfo() {}
		hkbBehaviorGraphInternalStateInfo( hkFinishLoadedObjectFlag flag ) 
			: hkReferencedObject(flag), 
			m_internalState(flag),
			m_auxiliaryNodeInfo(flag),
			m_activeEventIds(flag),
			m_activeVariableIds(flag){}

			/// The ID of the character who owns this behavior graph.
		hkUint64 m_characterId;

			/// The internal state or HK_NULL if the graph is inactive.
		hkRefPtr<hkbBehaviorGraphInternalState> m_internalState;

			/// Auxiliary info associated with each nodeInteranalStateInfo contained in m_internalState
		hkArray< hkRefPtr<hkbAuxiliaryNodeInfo> > m_auxiliaryNodeInfo;

			/// The external IDs (in the linker's symbol table) of the events that can cause a state change right now.
		hkArray<hkInt16> m_activeEventIds;

			/// The IDs in the root behavior of the active variables.
		hkArray<hkInt16> m_activeVariableIds;
};

/// Sets which character is the activate character.
class hkbCharacterSelectedInfo: public hkReferencedObject
{
	public:
		//+vtable(true)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_VDB );

		hkbCharacterSelectedInfo() {}
		hkbCharacterSelectedInfo( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag){}

			/// The ID of the character
		hkUint64 m_characterId;

			/// The port that Havok Script debugger is connected to
		hkInt32 m_scriptDebuggingPort;

		hkInt32 m_padding;
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
