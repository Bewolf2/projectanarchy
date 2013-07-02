/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_GENERATE_UTILS_H
#define HKB_GENERATE_UTILS_H

class hkbCharacter;
class hkbBehaviorGraph;
class hkbGeneratorOutput;
class hkbContext;
class hkbSceneModifier;
class hkJobQueue;
class hkJobThreadPool;

	/// A scene of characters with their associated outputs and contexts.
struct hkbSceneCharacters
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR,hkbSceneCharacters);
	hkbSceneCharacters();

	struct SceneCharacterInfo
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR, SceneCharacterInfo);

		hkbCharacter* m_character;
		hkbGeneratorOutput* m_generatorOutput;
		hkbContext* m_context;
	};

		// Map from Character pointer to Scene Character Info pointer
	hkPointerMap<hkbCharacter*, SceneCharacterInfo*> m_characters;

		// Array of Scene Character Infos used to iterate through each Scene Characters deterministically
	hkArray<SceneCharacterInfo*> m_characterInfos;

	// Both the hkbRagdollSceneModifier and the hkbCharacterControllerSceneModifier may update the physics.
	// Typically the hkbCharacterControllerSceneModifier runs first and steps the physics if requested by the hkbCharacterController.
	// In that case this flag is set to true to indicate that the physics has already been stepped so that the 
	// hkbRagdollSceneModifier won't do it again.  This scheme allows us to optimize the pipeline
	// to avoid ragdoll latency if possible.
	bool m_isPhysicsUpdated;
};

	/// Utility functions for calling generate() on multiple characters.
class hkbGenerateUtils
{
	public:
			/// Call generate when multi-threading.
		static void HK_CALL generateMultiThreaded(	hkbSceneCharacters& sceneCharacters, 
													hkJobQueue& jobQueue,
													hkJobThreadPool& jobThreadPool,
													bool setCharacterPose = true );

			/// Call generate with scene modifiers.
			/// 
			/// The passed sceneModifiers list can contain physics modifier which is responsible for updating the physics.
			/// In that case if the passed in behaviors contain in its hkbSceneModifierList with an entry of physics scene 
			/// modifier then this function would update the physics simulation else it is your responsibility to update 
			/// the physics.
		static void HK_CALL generateWithSceneModifiers( hkbSceneCharacters& sceneCharacters, 
														hkbSceneModifier** sceneModifiers,
														int numSceneModifier,
														hkReal timestep );

			/// Call generate when multi-threading with scene modifiers.
			/// 
			/// The passed sceneModifiers list can contain physics modifier which is responsible for updating the physics.
			/// In that case if the passed in behaviors contain in its hkbSceneModifierList with an entry of physics scene 
			/// modifier then this function would update the physics simulation else it is your responsibility to update 
			/// the physics.
		static void HK_CALL generateMultiThreadedWithSceneModifiers(	hkbSceneCharacters& sceneCharacters, 
																		hkJobQueue& jobQueue,
																		hkJobThreadPool& jobThreadPool,
																		hkbSceneModifier** sceneModifiers,
																		int numSceneModifier, 
																		hkReal timestep );
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
