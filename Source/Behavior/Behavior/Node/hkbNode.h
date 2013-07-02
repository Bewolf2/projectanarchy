/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_NODE_H
#define HKB_NODE_H

#include <Behavior/Behavior/Event/hkbEvent.h>
#include <Behavior/Behavior/Node/hkbBindable.h>
#include <Behavior/Behavior/Node/hkbNodeInfo.h>

extern const class hkClass hkbNodeClass;

class hkbBehaviorGraph;
class hkbContext;
class hkbEventQueue;
class hkbNodeInternalStateInfo;
class hkbVariableBindingSet;
class hkbCharacter;

/// An enumeration of some of the Behavior node types.
/// Some deprecated nodes have type HKB_NODE_TYPE_OTHER_GENERATOR or HKB_NODE_TYPE_OTHER_MODIFIER.
enum hkbNodeType
{
	HKB_NODE_TYPE_INVALID = 0,

	// Generators - must be contiguous and include Transition Effects, which must also be contiguous.
	HKB_NODE_TYPE_FIRST_GENERATOR,
	HKB_NODE_TYPE_BEHAVIOR_GRAPH = HKB_NODE_TYPE_FIRST_GENERATOR,
	HKB_NODE_TYPE_BEHAVIOR_REFERENCE_GENERATOR,
	HKB_NODE_TYPE_BLENDER_GENERATOR,
	HKB_NODE_TYPE_CLIP_GENERATOR,
	HKB_NODE_TYPE_MANUAL_SELECTOR_GENERATOR,
	HKB_NODE_TYPE_MODIFIER_GENERATOR,
	HKB_NODE_TYPE_REFERENCE_POSE_GENERATOR,
	HKB_NODE_TYPE_STATE_MACHINE,
	HKB_NODE_TYPE_SCRIPT_GENERATOR,
	HKB_NODE_TYPE_LAYER_GENERATOR,

	// You can add your own custom generator types here if you implement them on SPU.
	// You must also update hkbSpuBehaviorUtils::s_generatorSize.
	// HKB_NODE_TYPE_USER_SPU_GENERATOR_0,

	HKB_NODE_TYPE_END_OF_SPU_GENERATORS,

	// the rest of the generators don't run on SPU

	HKB_NODE_TYPE_DOCKING_GENERATOR = HKB_NODE_TYPE_END_OF_SPU_GENERATORS,
	HKB_NODE_TYPE_PARAMETRIC_MOTION_GENERATOR,
	HKB_NODE_TYPE_PIN_BONE_GENERATOR,
	HKB_NODE_TYPE_OTHER_GENERATOR,

	// This is where you can define your custom generator types if they don't run on SPU.
	// HKB_NODE_TYPE_USER_GENERATOR_0

	// Transition Effects (they are also generators) - Must be contiguous.
	HKB_NODE_TYPE_FIRST_TRANSITION_EFFECT = 48,
	HKB_NODE_TYPE_BLENDING_TRANSITION_EFFECT = HKB_NODE_TYPE_FIRST_TRANSITION_EFFECT,
	HKB_NODE_TYPE_GENERATOR_TRANSITION_EFFECT,

	// You can add your own custom transition effect types here if you implement them on SPU.
	// You must also update hkbSpuBehaviorUtils::s_transitionEffectSize.
	// HKB_NODE_TYPE_USER_SPU_TRANSITION_EFFECT_0,

	HKB_NODE_TYPE_END_OF_SPU_TRANSITION_EFFECTS,

	// The rest of the transition effects don't run on SPU.
	
	// This is where you can define your custom transition effect types if they don't run on SPU.
	// HKB_NODE_TYPE_USER_TRANSITION_EFFECT_0

	// Modifiers - Must be contiguous.
	HKB_NODE_TYPE_FIRST_MODIFIER = 64,
	HKB_NODE_TYPE_ATTACHMENT_MODIFIER = HKB_NODE_TYPE_FIRST_MODIFIER,
	HKB_NODE_TYPE_ATTRIBUTE_MODIFIER,
	HKB_NODE_TYPE_CHARACTER_CONTROLLER_MODIFIER,
	HKB_NODE_TYPE_COMBINE_TRANSFORMS_MODIFIER,
	HKB_NODE_TYPE_COMPUTE_DIRECTION_MODIFIER,
	HKB_NODE_TYPE_COMPUTE_ROTATION_FROM_AXIS_ANGLE_MODIFIER,
	HKB_NODE_TYPE_COMPUTE_ROTATION_TO_TARGET_MODIFIER,
	HKB_NODE_TYPE_DAMPING_MODIFIER,
	HKB_NODE_TYPE_DELAYED_MODIFIER,
	HKB_NODE_TYPE_EVALUATE_EXPRESSION_MODIFIER,
	HKB_NODE_TYPE_EVENTS_FROM_RANGE_MODIFIER,
	HKB_NODE_TYPE_EVENT_DRIVEN_MODIFIER,
	HKB_NODE_TYPE_FOOT_IK_CONTROLS_MODIFIER,
	HKB_NODE_TYPE_GET_WORLD_FROM_MODEL_MODIFIER,
	HKB_NODE_TYPE_HAND_IK_CONTROLS_MODIFIER,
	HKB_NODE_TYPE_KEYFRAME_BONES_MODIFIER,
	HKB_NODE_TYPE_LOOK_AT_MODIFIER,
	HKB_NODE_TYPE_MIRROR_MODIFIER,
	HKB_NODE_TYPE_MODIFIER_LIST,
	HKB_NODE_TYPE_MOVE_BONE_ATTACHMENT_MODIFIER,
	HKB_NODE_TYPE_MOVE_CHARACTER_MODIFIER,
	HKB_NODE_TYPE_POWERED_RAGDOLL_CONTROLS_MODIFIER,
	HKB_NODE_TYPE_RIGID_BODY_RAGDOLL_CONTROLS_MODIFIER,
	HKB_NODE_TYPE_ROTATE_CHARACTER_MODIFIER,
	HKB_NODE_TYPE_SET_WORLD_FROM_MODEL_MODIFIER,
	HKB_NODE_TYPE_TIMER_MODIFIER,
	HKB_NODE_TYPE_TRANSFORM_VECTOR_MODIFIER,
	HKB_NODE_TYPE_TWIST_MODIFIER,

	// You can add your own custom modifier types here if you implement them on SPU.
	// You must also update hkbSpuBehaviorUtils::s_modifierSize.
	// HKB_NODE_TYPE_USER_SPU_MODIFIER_0,

	HKB_NODE_TYPE_END_OF_SPU_MODIFIERS,
	
	// the rest of the modifiers don't run on SPU

	HKB_NODE_TYPE_DETECT_CLOSE_TO_GROUND_MODIFIER = HKB_NODE_TYPE_END_OF_SPU_MODIFIERS,
	HKB_NODE_TYPE_EVALUATE_HANDLE_MODIFIER,
	HKB_NODE_TYPE_GET_HANDLE_ON_BONE_MODIFIER,
	HKB_NODE_TYPE_GET_UP_MODIFIER,
	HKB_NODE_TYPE_JIGGLER_MODIFIER,
	HKB_NODE_TYPE_SENSE_HANDLE_MODIFIER,
	HKB_NODE_TYPE_SEQUENCE,
	HKB_NODE_TYPE_AI_STEERING_MODIFIER,
	HKB_NODE_TYPE_AI_CONTROL_MODIFIER,
	HKB_NODE_TYPE_OTHER_MODIFIER,

	// This is where you can define your custom modifier types if they don't run on SPU.
	// HKB_NODE_TYPE_USER_MODIFIER_0
};

/// A node in a behavior graph.
class hkbNode : public hkbBindable
{
	public:
		// +version(1)

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

			/// Construct a node with no name.
		hkbNode( hkbNodeType nodeType );

	protected:

		hkbNode( const hkbNode& node );

	public:

			/// Performs initialization.
			///
			/// This method must be called before any calls to update(), or handleEvent(), 
			/// hkbGenerator::generate(), or hkbModifier::modify().
			/// Typically, a call to activate() is followed by several calls to update(), handleEvent(), 
			/// hkbGenerator::generate(), or hkbModifier::modify(), and then a call to deactivate().
			/// 
			/// Do not call this method directly.  Use hkbBehaviorGraph::activate().
			/// The base class implementation does nothing.
		virtual void activate( const hkbContext& context ) {}

			/// Advance time and generate events.
			///
			/// Can only be called between calls to activate() and deactivate().
			///
			/// Do not call this method directly.  Use hkbBehaviorGraph::update().
			/// The base class implementation does nothing.
		virtual void update( const hkbContext& context, hkReal timestep ) {}

			/// Handle events that are raised.  
			/// 
			/// Can only be called between calls to activate() and deactivate().
			///
			/// Do not call this method directly.  Use hkbBehaviorGraph::handleEvent().
			/// The base class implementation does nothing.
		virtual void handleEvent( const hkbContext& context, const hkbEvent e ) {}

			/// Clean up.
			///
			/// Do not call this method directly.  Use hkbBehaviorGraph::deactivate().
		virtual void deactivate( const hkbContext& context ) {}

			/// Flags used when calling GenNumChildren() or GetChildren().
		enum GetChildrenFlagBits
		{
				/// If true, only return the active children (otherwise, all children).
			FLAG_ACTIVE_ONLY = 0x1,
				
				/// If true, only return generators (else also return modifiers).
			FLAG_GENERATORS_ONLY = 0x2,

				/// If true, the behavior graphs pointed to by hkbBehaviorReferenceGenerators
				/// will be ignored (along with all of their descendants).
			FLAG_IGNORE_REFERENCED_BEHAVIORS = 0x4,
		};

			/// An integer type for storing combinations of GetChildrenFlagsBits.
		typedef int GetChildrenFlags;

			/// Returns an approximate upper bound on the number of children of this node.  The
			/// actual number of children may be higher or lower, and the only way to find out is to
			/// call getChildren().
			///
			/// It's completely optional for a subclass to override this method, but overriding it
			/// can improve efficiency.
			///		
			/// For best efficiency this method should never return less that the 
			/// actual number of children that would be returned by getChildren() given the same
			/// internal state of the node and passed in flags.  Thus number is used to set the capacity of an
			/// hkLocalArray, so if the size is too small it will cause a heap allocation.
			///
			/// It's best if getMaxNumChildren() is sensitive to the input flags because,
			/// for example, a node's active children are sometimes far fewer than all of its 
			/// children (see hkbStateMachine::getMaxNumChildren()).
			/// If the FLAG_ACTIVE_ONLY flag is set, then only active children should be counted.
			/// For example, an hkbStateMachine has a child for each state, but only one 
			/// child is active at a time.
			/// If the FLAG_GENERATORS_ONLY flag is set, then only generators should be counted,
			/// not modifiers.
			///
			/// The base class implementation returns 2 since most nodes have only 1 or 2 children.
		virtual int getMaxNumChildren( GetChildrenFlags flags ) { return 2; }

			/// Stores returned info about the children of a node.
		struct ChildrenInfo
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbNode::ChildrenInfo );

			ChildrenInfo( hkArray<hkbNodeChildInfo>& childInfos );

				/// An array of child info (must be allocated by the caller).
			hkArray<hkbNodeChildInfo>& m_childInfos;
		};

			/// Returns the children of this node.
			///
			/// The return value of getMaxNumChildren() is used to set the initial capacity of
			/// an hkLocalArray that is passed in as childrenInfo.m_childInfos. 
			/// So to avoid heap allocations, getMaxNumChildren() should return no less
			/// than the number of children that getChildren() will provide.
			/// The base class implementation does nothing.
		virtual void getChildren( GetChildrenFlags flags, ChildrenInfo& childrenInfo ) {}

			/// Returns 1 if the node is valid, and 0 otherwise.
			/// 
			/// This function is primarily intended to be used by HAT.  It tells you
			/// whether the node is invalid due to being configured erroneously.
			/// For example, a blend node with no children cannot function, so
			/// it would be considered invalid.  If you build behaviors in code,
			/// you may want to assert that each of the nodes is valid.
			/// If the node is not valid, an error string will be returned
			/// (in debug configurations only).  Otherwise, the error
			/// string will be HK_NULL.  Note that the behavior nodes will
			/// use the HKB_ERROR_STRING macro to return a string so the
			/// environment variable HKB_ENABLE_ERROR_STRINGS must be defined.
			/// The base class implementation returns true.
			///
			/// The currently assigned character is provided if available (may be HK_NULL).
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const;

			/// If your node requires one-time precomputation, implement this method.  It is called during graph linking.
			/// Because this method will be called during construction of graphs in HAT, you could use this method
			/// to compute data that can then be serialized with the node (Note: you should then ensure that this data
			/// is not recomputed during runtime-linking).  Precompute is considered a construction step and will therefore
			/// not be guarded by isValid checks in HAT.
			/// The base class implementation does nothing.
		virtual void precompute( const hkbContext& context ) {}

			/// Clone a node returning an instance of the same type as the node.  The cloned node
			/// should be in a fresh inactive state regardless of what state this node is in.
			/// You must override this if you write a custom node.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const;

			/// Create an object appropriate for storing the internal state of the node.
			/// The base class implementation returns HK_NULL.
		virtual hkReferencedObject* createInternalState();

			/// Get the internal state of the node.  The input object should be created
			/// by calling createInternalState().  The base class implementation does nothing.
		virtual void getInternalState( hkReferencedObject& internalState ) const {}

			/// This is called right after getInternalState().  Whereas getInternalState() is usually auto-generated,
			/// getInternalStateUser() is not.
		virtual void getInternalStateUser( const hkbBehaviorGraph& rootBehavior, hkReferencedObject& internalState ) const {}

			/// Set the internal state of the node.  The input object should be created
			/// by calling createInternalState().  The base class implementation does nothing.
		virtual void setInternalState( const hkReferencedObject& internalState ) {}

			/// This is called right after setInternalState().  Whereas setInternalState() is usually auto-generated,
			/// setInternalStateUser() is not.
		virtual void setInternalStateUser(	const hkbContext& context,
											const hkReferencedObject& internalState,
											hkPointerMap<hkInt16, const hkbNodeInternalStateInfo*>& nodeIdToInternalStateMap ) {}

			/// Get the events that could cause the node to change state.  This should only be called on an active node.
			/// A node must override this if it has a property that is an event ID.
		virtual void getActiveEvents( hkPointerMap<int, bool>& activeEvents ) const {}

	protected:

			/// This is called by getActiveVariables() so that the node can report any variables that are used in a special way
			/// beyond simple bindings (e.g., hkbStateMachine::m_syncVariableIndex).  Bound variables are collected automatically 
			/// by getActiveVariables().  If a node uses variables in addition to those that are bound, it must override this method.
		virtual void getActiveVariablesSpecial( hkPointerMap<int, bool>& activeVariables ) const {}

	public:

			/// Get the variables that are being used by this node.  This should only be called on an active node.
		void getActiveVariables( hkPointerMap<int, bool>& activeVariables ) const;

			/// Whether or not this node can be used as a template (!CLONE_STATE_CLONE).
		bool canBeTemplate() const;

			/// Whether or not this node can be used as a clone (!CLONE_STATE_TEMPLATE)
		bool canBeClone() const;

			/// Returns the type of the node.
		hkbNodeType getType() const;

			/// Returns true if this is an instance of hkbGenerator.
		bool isGenerator() const;

			/// Returns true if this is an instance of hkbModifier.
		bool isModifier() const;

			/// Returns true if this is an instance of hkbTransitionEffect.
		bool isTransitionEffect() const;

			/// Returns the node's info in writable form.
		hkbNodeInfo& accessNodeInfo();

			/// Returns the node's info and creates it if necessary.
		hkbNodeInfo& createAndAccessNodeInfo();

			/// Returns the node's info in read-only form.
		const hkbNodeInfo& getNodeInfo() const;

			/// Whether m_nodeInfo has been created or not.
		bool hasNodeInfo() const;

			/// This should be called when the node is deactivated.
		void discardNodeInfo();

	protected:

			// A helper function to check for -1 before inserting a symbol ID into a map.
		static void insertSymbolId( int symbolId, hkPointerMap<int, bool>& symbolIds );

			// Used for harvesting variables from hkbBindables.
		class BoundVariablesCollector : public hkbBindableCollector
		{
			public:
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR,hkbNode::BoundVariablesCollector);
				virtual void collectBindable( hkbBindable* bindable ) HK_OVERRIDE;

					// Collected variable IDs are put into this map.
				hkPointerMap<int,bool>* m_variablesMap;
		};

	public:

			/// You can use this for whatever you want.
		hkUlong m_userData;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")

			// The name of the node.
		hkStringPtr m_name;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")

			// An ID that is unique among all nodes in a behavior graph including all
			// nested graphs.
		hkUint16 m_id;	//+nosave

		enum CloneState
		{
				/// This node is neither a clone nor a template.  This is the initial m_cloneState.  It indicates
				/// that the node has not been cloned.  If you are not using cloning the nodes will have this state.
				/// If you are using cloning, all nodes should have their state changed from this at some point as they
				/// get cloned.
			CLONE_STATE_DEFAULT,

				/// This node is a template that should not be used directly.
			CLONE_STATE_TEMPLATE,

				/// This node is a clone that can be used.
			CLONE_STATE_CLONE,
		};

			/// Indicate whether this node is a template or a clone.
		CloneState getCloneState() const;

			/// Set whether this node is a template or a clone.  For internal use only.
		void setCloneState( CloneState cloneState );

			/// This is used in some templates that work differently if the node is a template vs clone.
		enum TemplateOrClone
		{
			NODE_IS_TEMPLATE,
			NODE_IS_CLONE,
		};

	protected:

			// whether this is a clone, template, or sharable node
		hkEnum< CloneState, hkInt8 > m_cloneState; //+nosave

			// the type of this node
		hkEnum<hkbNodeType, hkUint8> m_type; //+nosave

			// this is HK_NULL iff the node is active
		hkbNodeInfo* m_nodeInfo; //+nosave

	public:

		hkbNode( hkFinishLoadedObjectFlag flag ) : hkbBindable(flag), m_name(flag) {}
};

#include <Behavior/Behavior/Node/hkbNode.inl>

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
