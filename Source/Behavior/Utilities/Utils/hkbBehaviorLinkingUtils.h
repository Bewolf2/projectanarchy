/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_BEHAVIOR_LINKING_UTILS_H
#define HKB_BEHAVIOR_LINKING_UTILS_H

#include <Common/Base/Container/StringMap/hkStorageStringMap.h>
#include <Behavior/Behavior/StateMachine/hkbStateMachine.h>
#include <Behavior/Behavior/Character/hkbCharacterStringData.h>
#include <Behavior/Behavior/Utils/hkbUtils.h>

class hkaDefaultAnimationControl;
class hkaSkeleton;
class hkbAnimationBindingSet;
class hkbCharacter;
class hkbCharacterSetup;
class hkbCharacterData;
class hkbCharacterStringData;
class hkbBehaviorGraph;
class hkbSymbolLinker;
class hkbSequence;
class hkbStateMachine;
class hkbNode;
class hkbAssetLoader;
class hkbAssetSaver;
class hkbClipGenerator;
class hkbGenerator;
class hkbSymbolIdMap;
class hkbAnimationBindingWithTriggers;

	/// Utility functions dealing with linking the symbols in behaviors.
class hkbBehaviorLinkingUtils 
{
	public:
		
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbBehaviorLinkingUtils );

			/// Perform linking on the character and its associated behavior.
		static void HK_CALL linkCharacterAndBehaviors(	hkbCharacter& character,
														hkbBehaviorGraph* templateBehavior,
														hkbSymbolLinker* attributeLinker,
														hkbSymbolLinker* variableLinker,
														hkbSymbolLinker* eventLinker,
														hkbSymbolLinker* characterPropertyLinker );

			/// Perform linking on the character and its associated behavior.
		static void HK_CALL linkCharacterAndBehaviors(	hkbCharacter& character,
														hkbSymbolLinker* attributeLinker,
														hkbSymbolLinker* variableLinker,
														hkbSymbolLinker* eventLinker,
														hkbSymbolLinker* characterPropertyLinker );

			/// Unlink the character and its associated behavior.
		static void HK_CALL unlinkCharacterAndBehaviors( hkbCharacter& character, hkbBehaviorGraph* templateBehavior );

			/// Unlink the character and its associated behavior.
		static void HK_CALL unlinkCharacterAndBehaviors( hkbCharacter& character );

			/// Allocate the hkbBehaviorGraphData and hkbBehaviorGraphStringData for this behavior if they don't exist yet.
		static void HK_CALL allocateBehaviorDataAndStringData( hkbBehaviorGraph& behavior );

			/// Link the attributes, variables, and events of a behavior.
		static void HK_CALL linkBehavior(	hkbBehaviorGraph& behavior,
											hkbSymbolLinker* attributeLinker,
											hkbSymbolLinker* variableLinker,
											hkbSymbolLinker* eventLinker,
											hkbSymbolLinker* characterPropertyLinker );

			/// Link the attributes, variables, and events of a set of behaviors.
			/// The first behavior in the above set is assumed as the root behavior.
		static void HK_CALL linkBehaviors(	hkArray<hkbBehaviorGraph*>& behaviorsToBeLinked, 
											hkbSymbolLinker* attributeLinker,
											hkbSymbolLinker* variableLinker,
											hkbSymbolLinker* eventLinker, 
											hkbSymbolLinker* characterPropertyLinker );

			/// Link the attributes of the given behavior (but not nested behaviors).
		static void HK_CALL linkBehaviorAttributes( hkbBehaviorGraph& behavior, hkbSymbolLinker& attributeLinker );

			/// Link the variables of the given behavior (but not nested behaviors).
		static void HK_CALL linkBehaviorVariables( hkbBehaviorGraph& behavior, hkbSymbolLinker& variableLinker );

			/// Link the events of the given behavior (but not nested behaviors).
		static void HK_CALL linkBehaviorEvents( hkbBehaviorGraph& behavior, hkbSymbolLinker& eventLinker );

			/// Link the variables of the given sequence.
		static void HK_CALL linkSequenceVariables( hkbSequence& sequence, hkbSymbolLinker& variableLinker );

			/// Link the events of the given sequence.
		static void HK_CALL linkSequenceEvents( hkbSequence& sequence, hkbSymbolLinker& eventLinker );

			/// Link the character properties in the character setup.
		static void HK_CALL linkCharacterProperties( hkbCharacterSetup& characterSetup, hkbSymbolLinker& characterPropertyLinker );

			/// Link the character properties in the behavior.
		static void linkBehaviorCharacterProperties( hkbBehaviorGraph& behavior, hkbSymbolLinker& characterPropertyLinker );

			/// Clear all of the symbol mappers from a behavior and its sub-behaviors.
		static void HK_CALL unlinkBehavior( hkbBehaviorGraph& rootBehavior );

			/// Convert the annotations in the animation used by a clip generator into
			/// triggers by applying the given map.  If eventIdMap is not null, it is applied
			/// (external-to-internal) to the event ID from the map when adding the triggers.
		static void HK_CALL convertAnnotationsToTriggers(	hkbClipGenerator& clipGenerator, 
															const hkStringMap<int>& annotationToEventIdMap,
															const hkbSymbolIdMap* eventIdMap = HK_NULL );

			/// Convert the annotations in the animations used by all of the clip generators
			/// in a behavior graph into triggers by looking for a match between the annotation 
			/// name and the name of an event in a linker.
		static void HK_CALL convertAnnotationsToTriggers(	hkbBehaviorGraph& behaviorGraph, 
															const hkStringMap<int>& annotationToEventIdMap );

			/// Convert the annotations in an animation to triggers by looking for a match between the annotation 
			/// name and the name of an event in a linker.  The input animation binding is taken from the
			/// bindingWithTriggers parameter and the triggers are placed there too.
		static void HK_CALL convertAnnotationsToTriggers(	hkbAnimationBindingWithTriggers* bindingWithTriggers,
															const hkStringMap<int>& annotationToEventIdMap );

			/// Convert the annotations to triggers in all of the bindings in animationBindingSet.
			/// The bindings should already be present in animationBindingSet.  All existing triggers
			/// will be removed.  The event IDs are mapped from the annotation name using annotationToEventIdMap.
		static void HK_CALL convertAnnotationsToTriggers(	hkbAnimationBindingSet* animationBindingSet,
															const hkStringMap<int>& annotationToEventIdMap );

			/// Remove all of the triggers that have the m_isAnnotation flag set from all of the
			/// clip generators in a behavior graph.  This is useful if you are changing the animation
			/// bindings of your clip generators at runtime and want to re-convert annotations to
			/// triggers.  In such a situation, you probably want to remove the old ones.
		static void HK_CALL removeAnnotationTriggers( hkbBehaviorGraph& behaviorGraph );

			/// Sets hkbClipGenerator::m_animationBindingIndex based on stringData.
			/// This can be used if you have set up the character's hkbAnimationBindingSet and you
			/// just want to set the clips to index into it based on the clip's animation names.
			/// Clips with animation controls will not have their binding index setup.
			/// The output are clips which cannot be linked to the animation binding set *or* don't have their
			/// own animation controls.  We return the NodeBehaviorPair because the root behavior of a clip
			/// is needed for converting annotations to triggers.
		static void HK_CALL linkClipBindings( const hkbCharacterStringData* stringData, hkbBehaviorGraph* rootBehavior, hkArray<hkbUtils::NodeBehaviorPair>& clipsFailingToLink );

			/// Reactivate clips in characters which link to the specified binding indices.
			/// The index of the item in bindingIndicesUpdated is the binding index, the bool is true if that binding has changed.
		static void HK_CALL reactivateClips( const hkArrayBase<hkbCharacter*>& characters, const hkArrayBase<hkBool>& bindingIndicesUpdated );

			/// Initialize an animation binding set to be suitable for the animation binding set layout found in stringData.
		static void HK_CALL initAnimationBindingSet( const hkbCharacterStringData* stringData, hkbAnimationBindingSet* bindingSet );

			/// Finds all leaf ward hkbBehaviorReferenceGenerator in node and attempts to fix up
			/// their m_behavior members by pointing them to references in referenceBehaviors.
		static void HK_CALL connectAllReferenceBehaviors( hkbGenerator* node, const hkStorageStringMap<hkbBehaviorGraph*>& referenceBehaviors );

			/// Given two behaviors identically packed in memory copy the id mappers and animation
			/// bindings from one behavior to another.
		static void HK_CALL copyIdMappersAndAnimationBindings(	hkbBehaviorGraph* from,
																hkbBehaviorGraph* to );

			/// Compile all of the expressions in a behavior graph.  By default only the root behavior will be processed but
			/// you can pass in true for doNestedBehaviors to compile the expressions in nested behaviors too.
		static void HK_CALL compileExpressions( hkbBehaviorGraph* behavior, bool doNestedBehaviors=false );

			/// Calls precompute on all nodes in the template and all nested behaviors.
		static void HK_CALL precomputeNodeTemplates( hkbContext& context, hkbBehaviorGraph* behaviorTemplate );

	private:

			// Link the attributes, variables, and events of a sub behaviors.
		static void HK_CALL linkSubBehavior(	hkbBehaviorGraph& rootBehavior, 
												hkbBehaviorGraph& subBehavior, 
												hkbSymbolLinker* attributeLinker, 
												hkbSymbolLinker* variableLinker, 
												hkbSymbolLinker* eventLinker, 
												hkbSymbolLinker* characterPropertyLinker ); 

			// A recursive helper class for copyIdMappersAndAnimationBindings.
		static void HK_CALL copyIdMappersAndAnimationBindingsInternal(	hkbBehaviorGraph* from,
																		hkbBehaviorGraph* to,
																		hkPointerMap<hkbBehaviorGraph*, int>& processedBehaviors );

			// Compile the expressions in any hkbExpressionConditions in a list of transitions.
		static void HK_CALL compileExpressionConditions( const hkbContext& context, hkArray<hkbStateMachine::TransitionInfo>& transitions );

			// Temporary struct to find the proper binding index for a given bundleName + animationName
		struct BundleBindingInfo
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbBehaviorLinkingUtils::BundleBindingInfo );
			int m_startingIndex;
			hkStringMap<int> m_animationNameToIndex;
		};

			// Build up BundleBindingInfo for the character string data.
			// By building both BundleBindingInfo and loose animation binding indices
			// we make it possible for bundled clips to link to unbundled character string datas and visa versa.
		static void HK_CALL buildClipNameDataToBundleNameData( const hkbCharacterStringData* stringData, hkStringMap<BundleBindingInfo*>& bundleNameToInfoMap, hkArray<BundleBindingInfo>& bundleInfoMapStorage, hkStringMap<int>& looseAnimationNameToIdxMap );
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
