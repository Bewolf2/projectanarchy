/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_BEHAVIOR_LOADING_UTILS_H
#define HKB_BEHAVIOR_LOADING_UTILS_H

#include <Common/Base/Container/StringMap/hkStringMap.h>
#include <Common/Base/Container/PointerMap/hkPointerMap.h>
#include <Behavior/Utilities/Utils/hkbAssetLoader.h>

class hkbBehaviorGraph;
class hkbCharacterStringData;
class hkbAnimationBindingSet;
class hkaAnimationBinding;
class hkaSkeleton;
class hkbCharacterSetup;

	/// Utility functions dealing with loading assets.
class hkbBehaviorLoadingUtils 
{
	public:

			/// Load a behavior (no linking).
			///
			/// If the user has already loaded some behaviors then they can populate the
			/// loadedBehaviors list.  If a referenced behavior has the same name as one
			/// that is already in the list, then the behavior in the list is used instead
			/// of loading it. Loaded behaviors are added to the loadedBehaviors list. This 
			/// results in behaviors getting loaded only once even when they are shared.
		static hkbBehaviorGraph* HK_CALL loadBehavior(  const char* behaviorPath,
														const char* behaviorFilename,
														hkbAssetLoader& assetLoader,
														hkStringMap<hkbBehaviorGraph*>& loadedBehaviors );

			/// Save a behavior.
			///
			/// If the user has already saved out some behaviors then they can populate the
			/// savedBehaviors list.  If a referenced behavior has the same name as one in the
			/// list it will not be saved, on the assumption that they are the same and it has
			/// already been saved.  Behaviors saved by this function will be
			/// added to the savedBehaviors list. This results in behaviors getting saved
			/// only once even when they are shared.
		static void HK_CALL saveBehavior(	hkbBehaviorGraph* behavior, 
											const char* behaviorPath,
											const char* behaviorFilename,
											hkbAssetSaver& assetSaver,
											hkStringMap<hkbBehaviorGraph*>& savedBehaviors );
 

			/// This method will first initialize the animation binding set (see hkbBehaviorLinkingUtils::initAnimationBindingSet),
			/// then try to load all animations specified by the stringData into the binding set (see loadAnimations),
			/// and finally, find all instances of hkbClipGenerator in rootBehavior and link the clips to their proper
			/// animation binding indices (see hkbBehaviorLinkingUtils::linkClipBindings).
			/// loadClips will also attempt to load animations directly from the clip animation name if they fail to link
			/// into the animation binding set.
		static void HK_CALL loadClips(	const hkbCharacterStringData* stringData,
										hkbAnimationBindingSet* bindingSet,
										hkbAssetLoader& assetLoader,
										hkbBehaviorGraph* rootBehavior,
										const hkaSkeleton& skeleton,
										const char* animationPath=HK_NULL,
										const hkStringMap<int>* annotationToEventIdMap=HK_NULL,
										const hkArrayBase<const char*>* animationAssetsToLoad=HK_NULL,
										hkPointerMap<void*,int>* animationAssetsToLoadCountMap=HK_NULL );

			/// Try to load animations into the animation binding set if they are not already loaded.
			/// If animationAssetsToLoad is provided, only the specified animations will attempt to be loaded.
			/// Otherwise, all the animations specified by the stringData will attempt to be loaded.
			/// animationAssetsToLoadCountMap is a map which is updated if an asset is added to the binding set, this is used to
			/// do ref-counting outside of the hkbAssetLoader so that hkbAssetLoader::unload can be called appropriately
			/// and should therefore be the same map between calls to load/unloadAnimations.
			/// The index of the item in bindingIndicesUpdated is the binding index, the bool is true if that binding has changed.
			/// Returns true if any bindings were changed.
		static hkBool HK_CALL loadAnimations(	const hkbCharacterStringData* stringData,
												hkbAnimationBindingSet* bindingSet,
												hkbAssetLoader& assetLoader,
												const hkaSkeleton& skeleton,
												const char* animationPath=HK_NULL,
												const hkStringMap<int>* annotationToEventIdMap=HK_NULL,
												const hkArrayBase<const char*>* animationAssetsToLoad=HK_NULL,
												hkPointerMap<void*,int>* animationAssetsToLoadCountMap=HK_NULL,
												hkArray<hkBool>* bindingIndicesUpdated=HK_NULL );

			/// Possibly unload animations and clear their location in the animation binding set.
			/// If animationAssetsToLoad is provided, only the specified animations will be considered for unloading.
			/// Otherwise, all the animations specified by the stringData will be considered for unloading.
			/// animationAssetsToLoadCountMap is a map which is updated if an asset is cleared from the binding set, this is used to
			/// do ref-counting outside of the hkbAssetLoader so that hkbAssetLoader::unload can be called appropriately
			/// and should therefore be the same map between calls to load/unloadAnimations.
			/// The index of the item in bindingIndicesUpdated is the binding index, the bool is true if that binding has changed.
			/// Returns true if any bindings were changed.
		static hkBool HK_CALL unloadAnimations(	const hkbCharacterStringData* stringData,
												hkbAnimationBindingSet* bindingSet,
												hkbAssetLoader& assetLoader,
												const char* animationPath=HK_NULL,
												const hkArrayBase<const char*>* animationAssetsToUnload=HK_NULL,
												hkPointerMap<void*,int>* animationAssetsToLoadCountMap=HK_NULL,
												hkArray<hkBool>* bindingIndicesUpdated=HK_NULL );

			/// Load the retargeting skeleton mappers for the character.
		static void HK_CALL loadRetargetingSkeletonMappers( hkbCharacterSetup* setup, hkbAssetLoader& assetLoader, const char* characterPath, hkArray<hkStringPtr>& loadedFiles );

	private:

			// Helper function for assigning the binding to the hkbAnimationBindingWithTriggers, possibly converting annotations to triggers and setting the skeleton if needed.  This function will not add a reference to the animationBinding when it is assigned to the bwt.
		static bool HK_CALL assignAnimationBinding( class hkbAnimationBindingWithTriggers* bwt, hkaAnimationBinding* animationBinding, const hkStringMap<int>* annotationToEventIdMap, const class hkaSkeleton& skeleton );
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
