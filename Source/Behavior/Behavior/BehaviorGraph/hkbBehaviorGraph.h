/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_BEHAVIOR_GRAPH_H
#define HKB_BEHAVIOR_GRAPH_H

#include <Behavior/Behavior/Generator/hkbGenerator.h>
#include <Behavior/Behavior/Variables/hkbVariableValueSet.h>
#include <Behavior/Behavior/Linker/hkbSymbolIdMap.h>
#include <Behavior/Behavior/Node/hkbNodeInfo.h>
#include <Behavior/Behavior/Context/hkbContext.h>
#include <Behavior/Behavior/Generator/hkbGeneratorOutput.h>
#include <Behavior/Behavior/BehaviorGraph/hkbBehaviorGraphData.h>

extern const class hkClass hkbBehaviorGraphClass;

class hkbBehaviorGraphInternalState;
class hkbGenerateNodesJob;
class hkbStateListener;
class hkbStateMachine;
class hkbTransitionEffect;
struct hkbNodePartitionInfo;
struct hkbGenerateNodesJobNodeSetup;

class hkPseudoRandomGenerator;
class hkVtableClassRegistry;
class hkTypeInfoRegistry;
template <typename T, typename A> class hkStringMap;

	/// An hkbBehaviorGraph encapsulates a behavior graph and variable set.
	///
	/// You should use the hkbBehaviorGraph class as a primary interface to the functionality of
	/// a behavior graph.  It keeps track of which nodes are active and automatically calls
	/// activate() and deactivate() on them as needed.
class hkbBehaviorGraph : public hkbGenerator
{
	//+version(1)
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		friend class hkbSequence;

			/// Construct with defaults.
		hkbBehaviorGraph();

			/// Create a behavior graph given the root generator and optional behavior data.
		hkbBehaviorGraph( hkbGenerator* rootGenerator, hkbBehaviorGraphData* data );

		hkbBehaviorGraph( const hkbBehaviorGraph& that );

		virtual ~hkbBehaviorGraph();

		//////////////////////
		// hkbNode interface
		//////////////////////

			/// Call activate() on all of the active nodes in the behavior.
			///
			/// The nodes are activated in parent-before-child order, because a node must be activated before it
			/// knows which children are active.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			/// Call update() on all of the active nodes in the behavior.
			///
			/// The nodes are updated in child-before-parent order.  The events generated during the update are
			/// returned in eventQueue.
		virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

			/// Pass an event to all of the active nodes in the behavior.
		virtual void handleEvent( const hkbContext& context, const hkbEvent e ) HK_OVERRIDE;

			/// Call deactivate() on all of the active nodes in the behavior.
		virtual void deactivate( const hkbContext& context ) HK_OVERRIDE;

			/// Return the root generator as the one child.
		virtual void getChildren( GetChildrenFlags flags, ChildrenInfo& childrenInfo ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		///////////////////////////
		// hkbGenerator interface
		///////////////////////////

			/// Generate a pose.
			///
			/// Calls generate() and modify() on the active nodes of the behavior in order to generate the root pose.
		virtual void generate(	const hkbContext& context,
								const hkbGeneratorOutput** activeChildrenOutput,
								hkbGeneratorOutput& output,
								hkReal timeOffset = 0.0f ) const HK_OVERRIDE;

			/// The hkbBehaviorGraph can recycle output.
		virtual bool canRecycleOutput() const HK_OVERRIDE;

			// hkbGenerator interface implementation.
		virtual void updateSync( const hkbContext& context ) HK_OVERRIDE;

	public:

		////////////////////////
		// access to variables
		////////////////////////

			/// Get the value of a variable that fits into a word (eg, hkReal, hkInt32, hkInt16, hkInt8, bool).
			/// You should pass in the external index of the variable that results from the linking process.
			/// See hkbSymbolLinker, hkbBehaviorLinkingUtils::linkBehavior(), and the section of the manual
			/// entitled Linking Behaviors.
		template <typename T>
		T getVariableValueWord( int externalVariableIndex );

			/// Get the value of a variable that fits into a quadword (eg, hkVector4, hkQuaternion).
			/// You should pass in the external index of the variable that results from the linking process.
			/// See hkbSymbolLinker, hkbBehaviorLinkingUtils::linkBehavior(), and the section of the manual
			/// entitled Linking Behaviors.
		template <typename T>
		const T& getVariableValueQuad( int externalVariableIndex );

			/// Get the value of a variable that is an hkReferencedObject.
			/// You should pass in the external index of the variable that results from the linking process.
			/// See hkbSymbolLinker, hkbBehaviorLinkingUtils::linkBehavior(), and the section of the manual
			/// entitled Linking Behaviors.
		hkReferencedObject* getVariableValueObject( int externalVariableIndex );

			/// Set the value of a variable that fits into a word (eg, hkReal, hkInt32, hkInt16, hkInt8, bool).
			/// You should pass in the external index of the variable that results from the linking process.
			/// See hkbSymbolLinker, hkbBehaviorLinkingUtils::linkBehavior(), and the section of the manual
			/// entitled Linking Behaviors.
		template <typename T>
		void setVariableValueWord( int externalVariableIndex, T value, bool clamp = false );

			/// Set the value of a variable that fits into a quadword (eg, hkVector4, hkQuaternion).
			/// You should pass in the external index of the variable that results from the linking process.
			/// See hkbSymbolLinker, hkbBehaviorLinkingUtils::linkBehavior(), and the section of the manual
			/// entitled Linking Behaviors.
		template <typename T>
		void setVariableValueQuad( int externalVariableIndex, const T& value );

			/// Set the value of a variable that is an hkReferencedObject.
			/// You should pass in the external index of the variable that results from the linking process.
			/// See hkbSymbolLinker, hkbBehaviorLinkingUtils::linkBehavior(), and the section of the manual
			/// entitled Linking Behaviors.
		void setVariableValueObject( int externalVariableIndex, hkReferencedObject* obj );

			/// Returns true if a variable is bound to this behavior, false otherwise.
		bool hasVariable( int externalVariableIndex ) const;

	public:

			/// This class is used internally during generate().
		class GenerateWorkingData
		{
			public:

				HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbBehaviorGraph::GenerateWorkingData );

				GenerateWorkingData(	hkbBehaviorGraph& behaviorGraph,
										const hkbContext& context,
										hkbGeneratorOutput& output,
										hkReal timeOffset );

				typedef hkPointerMap<hkbNode*,hkbGeneratorOutput*> NodeToOutputMap;

					/// The output of every node.
				HK_ALIGN16( NodeToOutputMap m_nodeToOutputMap );

					/// The behavior graph being generated.
				hkbBehaviorGraph& m_behaviorGraph;

					/// The context needed to generate().
				const hkbContext& m_context;

					/// The output buffer in which the final output will be placed.
				hkbGeneratorOutput& m_output;

					/// Memory for intermediate output tracks.
				hkVector4* m_outputTracksBuffer;

					/// The offset of each buffer from the base address.
				hkUint32* m_outputTracksBufferOffset;

					/// The time offsets for the active nodes.
				hkReal* m_timeOffsets;

					/// When initiating jobs we chain these together.
				GenerateWorkingData* m_nextData;

					/// A time offset provided by the user.  The generate may look ahead in time.
				hkReal m_timeOffset;

					/// The index of the current node being processed.
				hkInt32 m_nodeIndex;
					
					/// The index of the current partition that can run on the spu. 
				hkInt16 m_currentSpuPartitionIndex;

					/// Whether or not to set the character pose when we are done or just
					/// leave the pose in the output buffer.
				bool m_setCharacterPose;

					/// Indicates whether this is the first time we have hit a job for a behavior
				bool m_initAtStartUp;
		};

			/// Stores the relationships between state machines and which behavior graph contains each state machine.
		struct StateMachineInfo
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, StateMachineInfo );

				/// A pointer to a node which is a state machine.
			hkbStateMachine* m_stateMachineTemplate;

				/// The behavior graph to which the state machine belongs. 
				/// For the state machines in root behavior graph this would be set to null.
			hkbBehaviorGraph* m_behaviorTemplate;

				/// All the global wildcard transitions for a state machine with only conditions.
				/// If they have different priorities than the one with higher priority comes first.
				/// The index of the transition in hkbStateMachine::m_wildcardTransitions is stored.
			hkArray<hkInt16>* m_eventlessGlobalTransitions;

				/// A map from events to global transition.
				/// The index of the transition in hkbStateMachine::m_wildcardTransitions is stored.
			hkPointerMap<int, hkInt16>* m_eventToGlobalTransitionMap;

				/// The child state machine info indices
			hkArray<int>* m_childStateMachineInfoIndices;

				/// The state machine info index which contains the parent state machine of this state machine. 
			hkInt32 m_parentStateMachineInfoIndex;

				/// The index of the state in the parent state machine that contains this state machine.
			hkInt32 m_stateIndexInParent;
		};

			/// The data structures for global transitions, reference counted for shared access.
		class GlobalTransitionData : public hkReferencedObject
		{
			public:

				HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
				
				GlobalTransitionData() {}

				~GlobalTransitionData();

					/// All the state machines in the entire behavior graph.
				hkArray<struct StateMachineInfo> m_stateMachineInfos; //+nosave

					/// Indices (into the m_stateMachineInfos) of the
					/// state machine infos that have global wildcard transitions with only conditions.
				hkArray<int> m_eventlessGlobalTransitionStateMachineIndices;

					/// A map from state machines to indices in the m_stateMachineInfos
				hkPointerMap<hkbStateMachine*, int> m_stateMachineTemplateToIndexMap; //+nosave

					/// A map from event IDs to arrays of indices into the m_stateMachineInfos.
					/// This map tells us which state machines have global wildcard transitions defined
					/// for each event ID.
				hkPointerMap<int, hkArray<int>* > m_eventToStateMachineInfoIndices; //+nosave

					// A map from ids to state machine templates. This is used for looking up global transitions.
				hkPointerMap<hkUint16, hkbStateMachine*> m_idToStateMachineTemplateMap; //+nosave
		};

	public:

		///////////////
		// properties
		///////////////

			/// How do deal with variables when the behavior is inactive.
		enum VariableMode
		{
				/// Throw away the variable values and memory on deactivate().
				/// In this mode, variable memory is allocated and variable values are
				/// reset each time activate() is called.
			VARIABLE_MODE_DISCARD_WHEN_INACTIVE = 0,

				/// Don't discard the variable memory on deactivate(), and don't
				/// reset the variable values on activate() (except the first time).
			VARIABLE_MODE_MAINTAIN_VALUES_WHEN_INACTIVE,
		};

			/// How do deal with variables when the behavior is inactive.
		hkEnum<VariableMode,hkInt8> m_variableMode;

			/// Array of node pointers.
		typedef hkArray<hkbNodeInfo*> NodeList;

			/// A mapping between the active node pointers and their indices in a node list.
		typedef hkPointerMap<const hkbNode*,int> NodeToIndexMap;

	public:

			/// For internal use.  Sets all nodes to CLONE_STATE_DEFAULT.
		void setToCloneStateDefault();

			/// Clone a behavior graph by making a shallow copy.  If the original behavior graph
			/// is from a packfile, be sure not to throw out the memory.
			/// The optional root generator must be a node from this behavior graph.
			/// It cannot come from a nested behavior.
		hkbBehaviorGraph* cloneShallow( hkbGenerator* optionalRootGenerator = HK_NULL );

			/// Clone a behavior graph by making a complete copy.
			/// 
			/// A clone is represented compactly in memory similar to a packfile.
			/// If copyAnimationBindings is true, the pointer to the animation binding used
			/// by each clip generator will be copied.  In the process, a
			/// new hkbDefaultAnimationControl will be created for each hkbClipGenerator
			/// copy if the original had one.  Note that the hkaDefaultAnimationControl
			/// itself is not cloned, a new one is created with default construction.
			/// If copyMappers is true, the m_eventIdMap, m_attributeIdMap and m_variableIdMap
			/// are cloned. If this is not set to true the new behavior will lose all the
			/// mapping info. To re-create all the mapping info the user needs to re-link.
			/// If copyReferencedBehaviors is set to true, then behaviors embedded inside
			/// hkbBehaviorReferenceGenerator nodes will be cloned. Otherwise all
			/// hkbBehaviorReferenceGenerators will point to null behaviors.  If multiple
			/// hkbBehaviorReferenceGenerators reference the same behavior graph then
			/// the cloned behavior graph will do the same.  All cloned behavior graphs will be
			/// returned in behaviorToCloneMap if you provide it, including the root behavior.
			/// You can provide your own cloned behaviors by populating behaviorToCloneMap before
			/// calling this method.  If a behavior graph is encountered that is a key in behaviorToCloneMap
			/// then it will not be cloned; it will be taken from the map instead.
			/// You can override the default registries by passing in vtableClassRegistery and
			/// finishLoadedObjectRegistery.
			/// If you pass in replacementRootGenerator it should be a generator in this behavior graph.
		hkbBehaviorGraph* cloneDeep(	bool copyAnimationBindings = true,
										bool copyIdMappers = true,
										bool copyReferencedBehaviors = true,
										hkVtableClassRegistry* vtableClassRegistry = HK_NULL,
										hkTypeInfoRegistry* finishLoadedObjectRegistry = HK_NULL,
										hkbGenerator* replacementRootGenerator = HK_NULL,
										hkPointerMap<hkbBehaviorGraph*,hkbBehaviorGraph*>* behaviorToCloneMap = HK_NULL );

			/// Returns the root generator in the behavior graph.
		hkbGenerator* getRootGenerator() const;

			/// Indicates whether the behavior is active (activate() called but not yet deactivate()).
		bool isActive() const;

			/// Whether or not one of the nodes in this behavior is active.  You must pass in a template node,
			/// not a clone node.
		bool isActive( const hkbNode* nodeTemplate ) const;

			/// Set the root of the behavior graph.
			/// 
			/// Don't call this on behavior clones.
		void setRootGenerator( hkbGenerator* rootGenerator );

			/// Generate a pose.
			/// 
			/// Calls generate() and modify() on the active nodes of the behavior in order to generate the root pose.
			/// Note that this generate() method is intended to be called by the user, whereas the other one that
			/// is inherited from hkbGenerator is intended to be called internally.
		void generate(	const hkbContext& context, 
						hkbGeneratorOutput& output, 
						bool setCharacterPose = true, 
						hkReal timeOffset = 0.0f,
						bool doUpdateActiveNodesFirst = false );

			/// Pass multiple events to all of the active nodes in the behavior.
			/// 
			/// This method only updates the topology of the graph after all of the events
			/// have been processed.  This is somewhat faster than calling handleEvent()
			/// multiple times.  But if you want the topology of the graph to change
			/// after each event, so that the next event is sent to the new set of
			/// active nodes, then you must call handleEvent() instead.
		void handleEvents( const hkbContext& context, hkbEventQueue& eventQueue );

			/// Get the constant data associated with the behavior.
		hkbBehaviorGraphData* getData() const;

			/// Set the constraint data associated with the behavior.
		void setData( hkbBehaviorGraphData* data );

			/// If you want to set the values of variables before activating the behavior graph
			/// you can call this.  It will create the memory for the variables and set their
			/// initial values.  You can then set them before calling activate().
		void initVariablesBeforeActivate();

			/// Read-write access to the values of all behavior variables.
		hkbVariableValueSet* accessVariableValues();

			/// Returns the number of attributes used by the character.
		int getNumAttributes() const;

			/// Returns the number of variables used by the behavior.
		int getNumVariables() const;

			/// Returns the number of character properties.
		int getNumCharacterProperties() const;

			/// Get the map from internal event IDs to external event IDs.
		hkbSymbolIdMap* getEventIdMap() const;

			/// Set the map from internal event IDs to external event IDs.
		void setEventIdMap( hkbSymbolIdMap* eventIdMap );

			/// Get the map from internal attribute IDs to external attribute IDs.
		hkbSymbolIdMap* getAttributeIdMap() const;

			/// Set the map from internal attribute IDs to external attribute IDs.
		void setAttributeIdMap( hkbSymbolIdMap* attributeIdMap );

			/// Get the map from internal variable IDs to external variable IDs.
		hkbSymbolIdMap* getVariableIdMap() const;

			/// Set the map from internal variable IDs to external variable IDs.
		void setVariableIdMap( hkbSymbolIdMap* variableIdMap );

			/// Get the map from internal character properties to those on the character (external).
		hkbSymbolIdMap* getCharacterPropertyIdMap() const;

			/// Set the map from internal character properties to those on the character (external).
		void setCharacterPropertyIdMap( hkbSymbolIdMap* characterPropertyIdMap );

			/// Map an external event ID to an internal event ID.
		int getInternalEventId( int externalEventId ) const;

			/// Map an internal event ID to an external event ID.
		int getExternalEventId( int internalEventId ) const;

			/// Map an external event ID to its mirrored counterpart
		int getMirroredExternalEventId( int externalEventId ) const;

			/// Returns true if the behavior's attribute, event, variable or character property symbols
			/// have been linked, false otherwise.
		bool isLinked() const;

			/// Set whether the behavior has been linked (for internal use).
		void setIsLinked( bool linked );
		
			/// Get the current active nodes in the behavior
		const NodeList& getActiveNodes() const;

			/// Get the current active nodes in the behavior
		NodeList& getActiveNodes();

			/// Called internally and when multithreading.  Sets up the data for generate().
		void preGenerate(	GenerateWorkingData& data,
							bool doUpdateActiveNodes=true,
							bool initNodeToOutputMap=true );

			/// Called internally and when multithreading.  This is called after
			/// calling preGenerate and then recursively calling generateInternal.
		void postGenerate( GenerateWorkingData& data );

			/// Called internally to set the local time of an active generator.
			/// The local time is set recursively in that the generator and all 
			/// its descendants get their local time set.
		void setActiveGeneratorLocalTime( const hkbContext& context, hkbGenerator* generatorTemplate, hkReal localTime );

			/// Start an echo (see hkbClipGenerator) on a node and all of its descendants.
		void startEchoOnSubgraph( hkReal echoDuration, hkbGenerator* generatorTemplate );

			/// For internal use.  Copy the sync info of one node to another node.
		template <hkbNode::TemplateOrClone fromTemplateOrClone, hkbNode::TemplateOrClone toTemplateOrClone>
		void copySyncInfo( const hkbGenerator* from, const hkbGenerator* to );

			/// Get the writable node info for an active node.
		template <hkbNode::TemplateOrClone templateOrClone>
		hkbNodeInfo& accessNodeInfo( const hkbNode* node );

			/// Get the (const) node info for an active node.
		template <hkbNode::TemplateOrClone templateOrClone>
		const hkbNodeInfo& getNodeInfo( const hkbNode* node ) const;

			/// Returns the node info for a node that is a clone if and only if it is a transition effect.
		const hkbNodeInfo& getNodeInfoIsCloneIffIsTransitionEffect( const hkbNode* node ) const;

			/// Get the sync info for an active generator.
		template <hkbNode::TemplateOrClone templateOrClone>
		hkbGeneratorSyncInfo& accessSyncInfo( const hkbGenerator* gen );

			/// Get the (const) sync info for an active generator.
		template <hkbNode::TemplateOrClone templateOrClone>
		const hkbGeneratorSyncInfo& getSyncInfo( const hkbGenerator* gen ) const;

			/// Get the (const) sync info for an active generator.  This should only be called when the child might be a cloned TE or
			/// might be a template node that is not a TE.
		const hkbGeneratorSyncInfo& getSyncInfoIsCloneIffIsTransitionEffect( const hkbGenerator* gen ) const;

			/// Returns the clone of a node.  If m_nodeTemplateToCloneMap is HK_NULL or the node does
			/// not have a clone in the map, the input node is returned.
		hkbNode* getNodeClone( hkbNode* nodeTemplate ) const;

			/// Returns the clone of a node.  If there isn't one yet and m_nodeTemplateToCloneMap is not HK_NULL,
			/// a clone is created, stored in m_nodeTemplateToCloneMap, and returned.
		hkbNode* getOrCreateNodeClone( const hkbContext& context, hkbNode* nodeTemplate, bool mightBeOrphan );

			/// Returns the existing clone of stateListener from m_stateListenerTemplateToCloneMap if there is one.
			/// Otherwise, creates a new clone and adds it to m_stateListenerTemplateToCloneMap.
		hkbStateListener* getOrCreateStateListenerClone( hkbStateListener* stateListener );

			/// Removes the clone of the node from m_nodeTemplateToCloneMap and deletes it.
		void deleteNodeClone( hkbNode* nodeTemplate ) const;

			/// Returns the template of a node clone.
		hkbNode* getNodeTemplate( const hkbNode* nodeClone ) const;

			/// Whether or not this is a shallow clone (crated by cloneShallow()).
		bool isShallowClone() const;

			/// Set the state of a state machine that is in this graph.
		void setStateOfStateMachine(	hkbBehaviorGraph* parentBehaviorOfStateMachine,
										hkbStateMachine* stateMachineTemplate,
										int stateId,
										const hkbContext& context,
										bool ignoreTransitions=false,
										bool forceTransition=false,
										bool clearStateHistory=false );

			/// Initailize global transition data. This should be called only on the root bhavior graph.
		void initializeGlobalTransitionData();

			/// Set the global transition data. For internal use only.
		void setGlobalTransitionData( GlobalTransitionData* globalTransitionData );

			/// Get the global transition data. For internal use only.
		const GlobalTransitionData* getGlobalTransitionData() const;

			/// Get the behavior jobs.
		const hkArray<hkbGenerateNodesJob*>& getBehaviorJobs() const;

			/// Get the psuedo random generator. For intenal use only.
		hkPseudoRandomGenerator* getPseudoRandomGenerator() const;

			/// Compute a unique ID for each node.  These are stored in hkbNode::m_id.
			/// If using shallow cloning, you must call this on a template graph not
			/// on a shallow clone graph.  Returns the next available node ID in nextIdInOut.
			/// It's important to share a nextIdInOut amongst all the behaviors you are linking
			/// which share nested behaviors.
		void computeUniqueNodeIds( int& nextIdInOut );

			/// Get the state machine template given its id.
		hkbStateMachine* getStateMachineTemplateFromId( hkUint16 id ) const;

			/// Get the state machine's behavior template.
		hkbBehaviorGraph* getBehaviorTemplateFromStateMachineTemplate( hkbStateMachine* stateMachineTemplate ) const;

			/// Get a unique ID to be used for a transition effect.  TEs need dynamic unique IDs because they can 
			/// be cloned more than once for simultaneous use.
		hkUint16 getUniqueIdForTransitionEffect();

			/// Give a unique ID for a transition effect back to the pool.
		void relinquishUniqueIdForTransitionEffect( hkUint16 id );

			/// Get the internal state of the entire graph.
			/// This should only be called on the root behavior graph.
		void getInternalStateOfGraph( hkbBehaviorGraphInternalState& internalState );

			/// Activate the graph given an internal state that you would like the graph to be in.
			/// This should only be called on the root state machine.
		void activateWithInternalState( const hkbContext& context, const hkbBehaviorGraphInternalState& internalState );

			/// For internal use only.
			/// Request that the active nodes list be updated at the next appropriate time.  
			/// The value of ifThisIsTrue is OR-ed with the existing flag.
		void requestUpdateActiveNodes( const hkbContext& context, hkbNode* nodeClone, bool ifThisIsTrue = true );

			/// This is called to tell the graph that a node's m_enable has changed because
			/// of sychronizing with a behavior variable. For internal use.	
		void requestUpdateActiveNodesForEnable( bool ifThisIsTrue = true );

			/// A number that can be used for determinism checks.
		int computeCheckSumOfActiveNodeIds() const;

			/// Returns the cached map from internal variable IDs to those in the root graph.
		const hkInt32* getInternalToRootVariableIdMap() const;

			/// Returns the cached map from internal character property IDs to those in the character.
		const hkInt32* getInternalToCharacterCharacterPropertyIdMap() const;

			/// Returns the cached map from internal attribute IDs to those in the root graph.
		const hkInt32* getInternalToRootAttributeIdMap() const;

			/// Returns the template of a cloned graph.
		hkbBehaviorGraph* getTemplate();

	private:

			// A pool of unique IDs to be used for hkbTransitionEffects (all other nodes have static unique IDs).
			// If this pool is empty then m_nextUniqueId is used and incremented.  When an hkbTransitionEffect is
			// deactivated its ID is returned to the pool.
		hkArray<hkUint16> m_uniqueIdPool; //+nosave

			// A map from ids to state machine templates. This is used for looking up global transitions.
			// This is created one per template and shared by all the clones.
		hkPointerMap<hkUint16, hkbStateMachine*>* m_idToStateMachineTemplateMap; //+nosave

			// The mapper between external Id's and mirrored external Id's
		hkArray<int> m_mirroredExternalIdMap; //+nosave

			// A pseudo random generator. 
		hkPseudoRandomGenerator* m_pseudoRandomGenerator; //+nosave

			// The root node of the behavior graph.
		hkRefPtr<hkbGenerator> m_rootGenerator;

			// The constant data associated with the behavior.
		hkRefPtr<hkbBehaviorGraphData> m_data;

			// If this object is a clone, then this contains a pointer (and a reference) to the template
			// otherwise it is null.
		hkRefPtr<hkbBehaviorGraph> m_template; //+nosave

			// If this is a clone, this pointer points to the 
			// original root of the cloned behavior graph.  Usually, it
			// is the same as m_rootGenerator (if this is a clone).  
			// But if you change the root node it may be different.
		hkbGenerator* m_rootGeneratorClone; //+nosave

			// The current active nodes in the behavior.
		NodeList* m_activeNodes; //+nosave

			// A member for storing all the information required for doing global transitions.
		hkRefPtr<GlobalTransitionData> m_globalTransitionData;//+nosave

			// A mapper between internal event IDs and external event IDs.
		hkRefPtr<hkbSymbolIdMap> m_eventIdMap; //+nosave

			// A mapper between internal attribute IDs and external attribute IDs.
		hkRefPtr<hkbSymbolIdMap> m_attributeIdMap; //+nosave

			// A mapper between internal variable IDs and external variable IDs.
		hkRefPtr<hkbSymbolIdMap> m_variableIdMap; //+nosave

			// A mapper between internal character property IDs and those in the character (external).
		hkRefPtr<hkbSymbolIdMap> m_characterPropertyIdMap; //+nosave

			// The current value of the behavior variables.
		hkbVariableValueSet* m_variableValueSet; //+nosave

			// A map from template nodes to cloned nodes.
		hkPointerMap<hkbNode*,hkbNode*>* m_nodeTemplateToCloneMap; //+nosave

			// A map from template state listeners to clones.
		hkPointerMap<hkbStateListener*,hkbStateListener*>* m_stateListenerTemplateToCloneMap; //+nosave

			// Clones that have been created and added to the maps but have not been picked up yet as active nodes.
		hkArray<hkbNode*> m_recentlyCreatedClones; //+nosave

			// An element for each node describing the partitioning of the nodes
			// across processors.
		hkbNodePartitionInfo* m_nodePartitionInfo; //+nosave

			// The number of intermediate outputs needed to process the partitioned graph.
		hkInt32 m_numIntermediateOutputs; //+nosave

			/// The output size of each intermediate output
		hkArray<hkInt16> m_intermediateOutputSizes; //+nosave

			// The jobs for partition that can run on SPUs.
		hkArray<hkbGenerateNodesJob*> m_jobs; //+nosave

			// The memory required for all the partitions.
		hkArray<void*> m_allPartitionMemory; //+nosave

			// A cached map from internal variable IDs to root behavior graph variable IDs.
		hkArray<hkInt32> m_internalToRootVariableIdMap;	//+nosave

			// A cached map from internal character property IDs to the character's character property IDs.
		hkArray<hkInt32> m_internalToCharacterCharacterPropertyIdMap; //+nosave

			// A cached map from internal attribute IDs to root behavior graph attribute IDs
		hkArray<hkInt32> m_internalToRootAttributeIdMap; //+nosave

		enum
		{
			// the last 1024 IDs are for for transition effects
			FIRST_TRANSITION_EFFECT_ID = 63*1024,
		};

			// The next unique ID to use if m_uniqueIdPool is empty.
		hkUint16 m_nextUniqueId;	//+nosave

			// Whether or not the behavior has been activated.
		hkBool m_isActive; //+nosave

			// Whether or not the behavior graph has been linked (see hkbBehaviorLinkingUtils).
		hkBool m_isLinked; //+nosave

			// If this is set to true then we update the active nodes at the next opportunity.
		hkBool m_updateActiveNodes; //+nosave

			// If this is set to true then we need to go through the nodes and process any nodes whose m_enable has changed.
		hkBool m_updateActiveNodesForEnable; //+nosave

	public:

			// If this is set to true then we check the validity of the node before calling update. For HAT use only.
		hkBool m_checkNodeValidity; //+nosave

			// Whether or not any state machine has changed state or started or ended a transition this frame.
			// For internal use.
		hkBool m_stateOrTransitionChanged;	//+nosave

	private:

			// An internal constructor for creating clones.
		hkbBehaviorGraph( hkbGenerator* rootGenerator, hkbBehaviorGraphData* data, bool isClone );

	public:

			// Setup mirrored events.  mirrorStringA & mirrorStringB are equal length, null-terminated arrays of strings
			// Intended to be called on root behavior only
		void initializeMirroredEventMapping( const hkStringMap< int, hkContainerHeapAllocator>& nameToIdMap, const hkArray< hkStringPtr >& mirrorStringA, const hkArray< hkStringPtr >& mirrorStringB );

			// A recursive helper class for generate().
		void generateInternal( GenerateWorkingData& data );

			// Deallocate all job data. This must be called after any operations which may invalidate the graphs spu partition information.
		void clearAllJobs();

	private:

			// Pass an external eventId a behavior node otherwise pass an internal eventId.
		void handleEventInternal( const hkbContext& context, const hkbEvent e);

			// Recompute the set of active nodes, calling activate() and deactivate() as needed.
		void updateActiveNodes( const hkbContext& context, hkPointerMap<hkInt16,const hkbNodeInternalStateInfo*>* nodeIdToInternalState = HK_NULL );

			// Checks for any global transitions that are triggered by the given
			// event and initiates them.
		void processGlobalTransitions( const hkbContext& context, const hkbEvent e );

			// Checks for any global transitions that are triggered by conditions
			// and initiates them.
		void processEventlessGlobalTransitions( const hkbContext& context );

			// Initiate a global transition.
		void doGlobalTransition( const hkbContext& context, StateMachineInfo* info, int transitionIndex );

			// A helper function for initalizeGlobalTransitionData.  Returns the nested state machine info index.
		int collectStateMachinesRootFirstInternal(	hkbGenerator* generator,
													hkbBehaviorGraph* behavior,
													int parentStateMachineInfoIndex,
													int childIndex,
													hkPointerMap<hkbGenerator*, int>& generatorToIndexMap,
													bool& foundNestedTransitions,
													hkPointerMap<hkbGenerator*,int>& generatorToNestedStateMachineInfoIndexMap );

			// Copy variable values into nodes.
			//
			// If reverse is true, the values will be copied from the nodes into the variables.
		void copyVariablesToMembersRoot( const hkbContext& context, bool reverse, bool onlyEnable );

			// Compute the timestep for each node based on synchronization, given the root timestep.
		void computeTimestepForActiveNodes( const hkReal rootTimestep, hkReal* nodeTimesteps );

			// Throw away all state listener clones that have a reference count of 1.
		void discardUnusedStateListenerClones();

			// Cleans up currently orphaned nodes.
		void cleanupOrphanedNodes();

			// Delete some internal members.  This is called by deactivate() and by ~hkbBehaviorGraph().st
		void deleteOnDeactivate();

			// Call updateSync on all of the nodes in m_activeNodes.
		void updateSyncOnActiveNodes( const hkbContext& context );

			// The implementation for updateSyncOnActiveNodes.
		template<bool checkNodeValidity>
		void updateSyncOnActiveNodesImplementation( const hkbContext& context );

			// Copies the sync info from the provided internal state to the hkbNodeInfos.
		void setSyncOnActiveNodes( hkPointerMap<hkInt16, const hkbNodeInternalStateInfo*>& nodeIdToInternalState );

			// Set up m_internalToRootVariableIdMap etc.
		void initCachedIdMaps( hkbCharacter& character, hkbBehaviorGraph& rootBehaviorGraph );

			// helper function for setActiveGeneratorLocalTime
		void setActiveGeneratorLocalTimeInternal( const hkbContext& context, hkbNodeInfo& nodeInfo, hkReal localTime );
		
			// helper function for startEchoOnSubgraph
		void startEchoOnSubgraphInternal( hkReal echoDuration, hkbNode* nodeClone, hkArray<hkbNodeInfo*>& touched );

			// This is called on the root node to create a clone, recursively.
		void addRootNodeReference(	const hkbContext& context,
									hkbNode* nodeTemplate,
									hkPointerMap<hkInt16, const hkbNodeInternalStateInfo*>* nodeIdToInternalState );

			// This is called on the root node to remove a clone, recursively.
		void removeRootNodeReference( const hkbContext& context, hkbNode* nodeClone );

			// To properly setup global transition data for nested behaviors, we need to always ensure that any
			// rootward behavior graph's GlobalTransitionData is notified of our state machines.
		void computeUniqueNodeIdsInternal( int& nextIdInOut, hkArray<GlobalTransitionData*>& rootwardTransitionDatas );

		////////////////////////////////////////////////////
		// The following three functions are co-recursive.
		////////////////////////////////////////////////////

			// Clones a template node if there isn't a clone already, activates the node,
			// and adds a reference.  Returns the clone.
		hkbNode* addNodeReference(	const hkbContext& context,
									hkbNode* nodeTemplate,
									bool reset,
									hkPointerMap<hkInt16, const hkbNodeInternalStateInfo*>* nodeIdToInternalState );

			// Removes a reference to a clone and deactivates it if this is a last reference.
		void removeNodeReference( const hkbContext& context, hkbNode* nodeClone );

			// This is called when a node's set of active children needs to be reprocessed.
		void requestUpdateActiveNodesInternal(	const hkbContext& context,
												hkbNode* nodeClone,
												bool reset,
												hkPointerMap<hkInt16, const hkbNodeInternalStateInfo*>* nodeIdToInternalState );

	public:

		hkbBehaviorGraph( hkFinishLoadedObjectFlag flag );

		friend class hkbCpuBehaviorJob;
		friend class hkbBehaviorServer;
};

#include <Behavior/Behavior/BehaviorGraph/hkbBehaviorGraph.inl>

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
