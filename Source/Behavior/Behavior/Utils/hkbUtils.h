/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_UTILS_H
#define HKB_UTILS_H

#include <Behavior/Behavior/Linker/hkbSymbolIdMap.h>
#include <Behavior/Behavior/Node/hkbNode.h>
#include <Behavior/Behavior/Node/hkbNodeInfo.h>

// forward declarations
class hkbBehaviorGraph;
class hkbContext;
class hkbGeneratorOutput;
class hkbNode;
class hkbSymbolIdMap;
class hkbCharacter;
class hkbWorld;

	/// Behavior utilities.
class hkbUtils
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbUtils );

			/// Compute the validity of the given node in the context of the character.
			///
			/// \param node The node to test.
			/// \param parentBehavior The immediate parent behavior that contains the node.
			/// \param character The character to use as context.
		static bool HK_CALL computeNodeValidity(	hkbNode* node,
													hkbBehaviorGraph* parentBehavior,
													hkbCharacter* character,
													hkStringPtr& errorString );

			/// Compute the validities of all nodes on the character.
			/// Returns the number of invalid nodes found.
			///
			/// \param character The character.
		static int HK_CALL computeAllNodeValidities(	hkbCharacter* character, 
														hkArray<hkbNode*>& validNodes, 
														hkArray<hkbNode*>& invalidNodes, 
														hkArray<hkStringPtr>& errorStrings );

			/// Information returned about a node template.
		struct NodeBehaviorPair
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbUtils::NodeBehaviorPair );

			hkbNode* m_nodeTemplate;
			hkbBehaviorGraph* m_behavior;
		};

			/// Collect all of the nodes in a behavior into a list.
			/// 
			/// The list of nodes returned is sorted so that all nodes appear in the list before their parents.
			/// \param rootNode The root of the behavior graph.
			/// \param behavior The behavior that this rootNode belongs to.  Can be HK_NULL if you don't want collect information about which behaviors the nodes belong to.
			/// \param flags Can include FLAG_GENERATORS_ONLY and/or FLAG_IGNORE_REFERENCED_BEHAVIORS.
			/// \param nodeInfo The returned list of nodes and some information about each one.
		static void HK_CALL collectNodesLeafFirst(	hkbNode* rootNode, 
													hkbBehaviorGraph* behavior,
													hkbNode::GetChildrenFlags flags, 
													hkArray<NodeBehaviorPair>::Temp& nodeInfoOut );

			/// Collect all of the active nodes in the behavior into a list.
			///
			/// For internal use only. If you want to know the active nodes in a behavior graph please call hkbBehaviorGraph::getActiveNodes().
			/// The list of nodes returned is sorted so that all nodes appear in the list before their parents.
		static void HK_CALL collectActiveNodes(	const hkbContext& context, hkArray<hkbNodeInfo*>& nodeClonesOut );


			/// Collect all of the nodes of a particular type.  Note that if you call this on
			/// a shallow clone of a behavior graph then it will return template nodes, not the cloned nodes.
		static void HK_CALL collectNodesByClass(	const hkClass& klass,
													hkbNode* rootNode, 
													hkbBehaviorGraph* rootBehavior,
													hkbNode::GetChildrenFlags flags, 
													hkArray<hkbNode*>::Temp& nodesTemplates );

			/// Compute a rotation bAligned that is as similar as possible to b, but rotated so 
			/// that the specified local axis in b is aligned with the specified local axis in a.
			/// The result is computed by rotating b in the appropriate direction to align the 
			/// axis in b with that in a.
			/// If the angle between the axis in a and that in b is below minAngle, 
			/// b is returned unchanged.  If the angle between the axis in the two 
			/// frames differs from PI (180 degrees) by more than minAngle, a is returned.
			/// It's important that minAngle be > 0.0f since there is a singularity
			/// in both boundary cases.
			/// The parameter value should be between 0 and 1 so that aligning can be
			/// done gradually
		static void HK_CALL alignLocalAxis( const hkQuaternion &a, 
											const hkQuaternion &b, 
											const hkVector4 &aAxis,
											const hkVector4 &bAxis,
											hkQuaternion &bAligned,
											hkReal fraction,
											hkReal minAngle = HK_REAL_PI / 720.0f );

			/// Find a member of an object by following a member path and return its type. 
			///
			/// The member path is of the form "member1/member2:7/member3:8".  The path 
			/// is used to navigate into child objects.  Pointers are followed automatically.
			/// Integers after a colon (eg, 7, and 8, in the above example) in the path correspond to array indices.
			/// All other elements (eg, "member1", "member2", "member3" in the above example)
			/// are member names.  Returns a pointer to the member data.  You must initialize the
			/// argument passed into arrayOut to HK_NULL.
		static void* HK_CALL findMemberByPath(	void* rootObject,
												const char* memberPath,
												hkClassMember::Type& memberTypeOut,
												const hkClass*& memberClassOut,
												const hkClassMember*& memberOut,
												hkArray<char>*& arrayOut );

			/// This computes the Id by applying the aToExternalIdMap and then inverse of bToExternalIdMap.
		static int HK_CALL convertInternalIdToInternalId(	const hkbSymbolIdMap* aToExternalIdMap, 
															const hkbSymbolIdMap* bToExternalIdMap, 
															const int aInternalIndex );

			/// Removes any events that are duplicates from the queue.  The resulting
			/// queue contains the same events in the same order, but any event whose
			/// ID and payload matches an event earlier in the queue is removed.  If two
			/// events have the same ID but different payloads (including HK_NULL) then they
			/// are not considered duplicates and both will be present in the output queue.
		static void HK_CALL removeDuplicateEvents( hkbEventQueue& queue );

			/// Gather info telling which events and variables are available in the graph right now.
		static void HK_CALL getActiveEventsAndVariables( hkbWorld* world, hkbCharacter* character, hkArray<hkInt16>& activeEvents, hkArray<hkInt16>& activeVariables );

	private:

			/// Find a member of an hkVector4 by following a member path and return its type. 
		static void* HK_CALL findMemberByPathForVector4( void* rootObject, const char* memberPath, hkClassMember::Type& memberTypeOut );

			/// Find a member of an hkQuaternion by following a member path and return its type. 
		static void* HK_CALL findMemberByPathForQuaternion( void* rootObject, const char* memberPath, hkClassMember::Type& memberTypeOut );

			/// A recursive helper class for collectNodesLeafFirst.
		static void HK_CALL collectNodesLeafFirstInternal(	hkbNode* rootNode,
															hkbBehaviorGraph* behavior,
															hkbNode::GetChildrenFlags flags,
															hkArray<NodeBehaviorPair>::Temp& nodeInfo,
															hkPointerMap<const hkbNode*,int>& nodeToIndexMap );

			/// Apply an ID map to the symbols in symbolsIn and put the mapped symbols into symbolsOut.
		static void HK_CALL applyIdMap( const class hkbSymbolIdMap* idMap, const hkPointerMap<int,bool>& symbolsIn, hkPointerMap<int,bool>& symbolsOut, bool externalToInternal );

			/// Copy symbols from a map to an array.
		static void HK_CALL copyMapToArray( const hkPointerMap<int,bool>& symbolsIn, hkArray<hkInt16>& symbolsOut );

			/// A recursive helper class for collectActiveNodes.
		static void HK_CALL collectActiveNodesInternal( const hkbContext& context, hkbNode* rootNode, hkArray<hkbNodeInfo*>& nodeClones );
};

HK_FORCE_INLINE int hkbUtils::convertInternalIdToInternalId(	const hkbSymbolIdMap* aToExternalIdMap,
																const hkbSymbolIdMap* bToExternalIdMap,
																const int aInternalIndex )
{
	// no need to do anything if the maps are the same map
	if ( aToExternalIdMap == bToExternalIdMap )
	{
		return aInternalIndex;
	}

	// Using the aToExternalIdMap convert the aInternalId to an externalIndex.
	int externalIndex = (aToExternalIdMap == HK_NULL) ? aInternalIndex : aToExternalIdMap->getExternal( aInternalIndex );
	
	HK_ASSERT2( 0xc612bcc8, externalIndex != -1, "an internal ID should always have an external ID" );

	// Using the bToExternalIdMap convert an externalId to an internalId of B.
	return (bToExternalIdMap == HK_NULL ) ? externalIndex : bToExternalIdMap->getInternal( externalIndex );
}

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
