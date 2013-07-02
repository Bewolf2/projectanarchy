/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_PROJECT_ASSET_MANAGER_H
#define HKB_PROJECT_ASSET_MANAGER_H

#include <Common/Base/Object/hkReferencedObject.h>
#include <Common/Base/Container/StringMap/hkStorageStringMap.h>
#include <Animation/Animation/Rig/hkaBoneAttachment.h>
#include <Behavior/Utilities/Physics/hkbPhysicsBaseTypes.h>

class hkStreamReader;
class hkaMeshBinding;
class hkaSkeleton;
class hkaSkeletonMapper;
class hkbAnimationBindingSet;
class hkbAssetLoader;
class hkbBehaviorGraph;
class hkbCharacter;
class hkbCharacterData;
class hkbCharacterSetup;
class hkbProjectData;
class hkbSymbolLinker;
class hkbWorld;
class hkbAssetBundle;
class hkbScriptAssetLoader;

	/// This class loads behavior projects and their subsidiary assets and helps instantiate
	/// characters while sharing assets as much as possible.  All basic assets such as skins
	/// and rigs are shared, and behavior graphs are shallow cloned.
class hkbProjectAssetManager : public hkReferencedObject
{	
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR);

			/// The management mode for assets loaded by the project asset manager.
			/// Currently this management only applies to animation data.
		enum AssetManagementMode
		{
				/// Only keep assets loaded if a character exists which references them.
			ASSET_MANAGEMENT_AUTO,

				/// Asset loading/unloading is left entirely up to the user.
				/// (see loadAnimations and unloadAnimations)
			ASSET_MANAGEMENT_MANUAL,

				/// By default, assets are automatically managed.
			ASSET_MANAGEMENT_DEFAULT = ASSET_MANAGEMENT_AUTO
		};

			/// Your must provide the constructor with an hkbAssetLoader.
			/// A physics interface must be provided if loading physics assets.
		hkbProjectAssetManager(
			hkbAssetLoader* assetLoader,
			hkbPhysicsInterface* physicsInterface = HK_NULL,
			AssetManagementMode assetManagementMode = ASSET_MANAGEMENT_DEFAULT );

			/// Clears memory allocated by this object.
			/// Loaded assets will automatically be unloaded in ASSET_MANAGEMENT_AUTO mode.
			/// Otherwise, unloading is up to the user.  Most assetLoaders will automatically
			/// cleanup when they are deleted.
		~hkbProjectAssetManager();
		
			/// Loads a project from a file.  All characters, behavior, skeletons and skins referenced in the file will
			/// be loaded.  Animations are not loaded at this point (see createCharacter and loadAnimations).
		hkBool loadProject( const char* filename );

			/// Links a specific character loaded by the project asset manager.
		void linkCharacter( const char* characterName, hkbWorld* world );

			/// Links a specific behavior loaded by the project asset manager.  If you are using behavior file references in
			/// your behaviors use this method to link them.
		void linkBehavior( const char* behaviorName, hkbWorld* world );

			/// Link all characters and behavior graphs that have been loaded using the given symbol linkers.
		void linkAll(
			hkbSymbolLinker* eventLinker,
			hkbSymbolLinker* variableLinker = HK_NULL,
			hkbSymbolLinker* attributeLinker = HK_NULL,
			hkbSymbolLinker* characterPropertyLinker = HK_NULL );

			/// Link all characters and behavior graphs that have been loaded using the symbol linkers from the given world.
		void linkAll( hkbWorld* world );

			/// Creates a character.  Use project, character, and behavior name to select the character.  All data must 
			/// be loaded, via loadProject, before a character can be created.  If a character can't be created then
			/// HK_NULL is returned.  If you want annotations in the character's animations to be converted to events
			/// then you should pass into annotationToEventIdMap the event names from your event linker. If you want 
			/// to mirror the sync points then you should pass the m_nameToIdMap of the event linker in eventNameToEventIdMap.
			/// In ASSET_MANAGEMENT_AUTO mode, any unloaded referenced animations will attempt to be loaded via the assetLoader.
		hkbCharacter* createCharacter(
			const char* projectName,
			const char* characterName,
			const char* behaviorName,
			hkbWorld* world = HK_NULL,
			const hkStringMap<int>* annotationToEventIdMap = HK_NULL,
			const hkStringMap<int>* eventNameToEventIdMap = HK_NULL,
			const hkArrayBase<const char*>* assetsToLoad = HK_NULL,
			hkbAnimationBindingSet* uniqueAnimationBindingSet = HK_NULL );

			/// Creates a character by looking for the project and behavior names in the stored data.
			/// The project is known from loadProject and the behavior chosen is the default behavior set in HAT.
		hkbCharacter* createCharacter(
			const char* characterName,
			hkbWorld* world = HK_NULL,
			const hkStringMap<int>* annotationToEventIdMap = HK_NULL,
			const hkStringMap<int>* eventNameToEventIdMap = HK_NULL,
			const hkArrayBase<const char*>* assetsToLoad = HK_NULL,
			hkbAnimationBindingSet* uniqueAnimationBindingSet = HK_NULL );
		
			/// Finds project data mapped by project name.
		hkbProjectData* findProjectData( const char *projectName ) const;
			/// Finds character data mapped by character name.
		const hkbCharacterData* findCharacterData( const char* characterName ) const;
			/// Finds character setup mapped by character name.
		const hkbCharacterSetup* findCharacterSetup( const char* characterName ) const;
			/// Finds a behavior graph given the filename.
		hkbBehaviorGraph* findBehavior( const char* behaviorName) const;
		
			/// Returns a list of file names that the project is dependent on.  The projectPath must have
			/// been successfully called through loadProject for the dependent resource list to exist.
		void getDependentResources( const char* projectPath, hkArray<hkStringPtr>& dependentResources ) const;

			/// Returns the asset loader being used by this asset manager.
		hkbAssetLoader* accessAssetLoader() const;

			/// Sets the asset loader used to load assets.
		void setAssetLoader(hkbAssetLoader* assetLoader);

			/// Returns the asset loader being used by this asset manager.
		hkbScriptAssetLoader* accessScriptAssetLoader() const;

			/// Sets the script asset loader used to load script files.
		void setScriptAssetLoader( hkbScriptAssetLoader* scriptAssetLoader );

			/// Load one or more animation assets.  Only animation assets in animationAssetsToLoad will be loaded (or all if NULL).
			/// This will affect all characters of type characterName that are sharing the character setup's animation binding set.
			/// If no characterName is provided, then any character types which use the specified assets will be affected.
			/// (loadAnimations() will load all animations the project knows about).
		void loadAnimations(
			const char* characterName=HK_NULL,
			const hkArrayBase<const char*>* animationAssetsToLoad=HK_NULL,
			const hkStringMap<int>* annotationToEventIdMap=HK_NULL,
			const hkArrayBase<hkbCharacter*>* charactersToReactivate=HK_NULL );

			/// Load one or more animation assets.  Only animation assets in animationAssetsToLoad will be loaded (or all if NULL).
			/// This will affect all characters which share an animation binding set with the the passed in character.
		void loadAnimations(
			hkbCharacter& character,
			const hkArrayBase<const char*>* animationAssetsToLoad=HK_NULL,
			const hkStringMap<int>* annotationToEventIdMap=HK_NULL,
			const hkArrayBase<hkbCharacter*>* charactersToReactivate=HK_NULL  );

			/// Unload one or more animation assets.  Only animation assets in animationAssetsToUnload will be unloaded (or all if NULL).
			/// This will affect all characters of type characterName that are sharing the character setup's animation binding set.
			/// If no characterName is provided, then any character types which use the specified assets will be affected.
			/// (unloadAnimations() will unload all animations the project knows about).
		void unloadAnimations(
			const char* characterName=HK_NULL,
			const hkArrayBase<const char*>* animationAssetsToUnload=HK_NULL,
			const hkArrayBase<hkbCharacter*>* charactersToReactivate=HK_NULL );

			/// Unload one or more animation assets.  Only animation assets in animationAssetsToUnload will be unloaded (or all if NULL).
			/// This will affect all characters which share an animation binding set with the the passed in character.
		void unloadAnimations(
			hkbCharacter& character,
			const hkArrayBase<const char*>* animationAssetsToUnload=HK_NULL,
			const hkArrayBase<hkbCharacter*>* charactersToReactivate=HK_NULL );

			/// Resets the project asset manager. Will clear out all of the loaded assets and mappings that
			/// deal with 
		void clearAssets();

			/// Returns a concatenated, normalized path.  fullPath = normalized(rootPath + fileName)
		static void getFullNormalizedPath( const char* rootPath, const char* fileName, hkStringBuf& fullPath );
			/// Returns true if the path is in either form: "C:\..." or "//network..."
		static bool isPathAbsolute( const char* filePath );

			/// Gets the hkbPhysicsInterface used by the hkbProjectAssetManager. May be HK_NULL if physics isn't used.
		hkbPhysicsInterface* getPhysicsInterface() const { return m_physicsInterface; }
			/// Set the hkbPhysicsInterface for this asset manager ensuring that all the physics objects can be loaded.
			/// May be HK_NULL if physics isn't used.
		void setPhysicsInterface( hkbPhysicsInterface* physicsInterface );

		//////////////////////////////////////////////////////////////////////////
		// Deprecated Physics Implementation Specific Functions
		// Must link against hkbPhysics2012Bridge
		//////////////////////////////////////////////////////////////////////////

			/// Your must provide the constructor with an hkbAssetLoader.
		hkbProjectAssetManager(
			hkbAssetLoader* assetLoader,
			hkbDeprecatedPhysicsWorldPtr physicsWorld,
			AssetManagementMode assetManagementMode = ASSET_MANAGEMENT_DEFAULT );

	private:

			/// Struct that stores information necessary to construct a character
		struct CharacterInfo
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbProjectAssetManager::CharacterInfo );
	
			CharacterInfo();			

				/// The character setup contains sharable character assets.
			hkRefPtr<hkbCharacterSetup> m_characterSetup;

				/// The ragdoll container a new clone must be made from this per character.
			hkRefPtr<hkbRagdollInterface> m_ragdollInterface;

				/// The name of the project
			hkStringPtr m_projectName;
		};

		struct AnimationBindingSetInfo
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbProjectAssetManager::AnimationBindingSetInfo );

				/// Instances of hkbCharacter which use the same animation binding set.
			hkArray<hkbCharacter*> m_characters;

				/// The CharacterInfo for the character this animation binding set is assigned to.
			CharacterInfo* m_characterInfo;
		};

		struct ProjectDependencies 
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbProjectAssetManager::ProjectDependencies );

				/// List of files a project is dependent on.
			hkArray<hkStringPtr> m_dependentFiles;
		};

		void init(
			hkbAssetLoader* assetLoader,
			hkbPhysicsInterface* physicsInterface,
			AssetManagementMode assetManagementMode );

		void loadOrUnloadAnimationsInternal(
			hkBool loadAssets,
			const hkbProjectData* projectData,
			const hkbCharacterSetup* characterSetup,
			hkbAnimationBindingSet* animationBindingSet,
			const hkArrayBase<const char*>* animationAssetsToLoadOrUnload=HK_NULL,
			const hkStringMap<int>* annotationToEventIdMap=HK_NULL,
			const hkArrayBase<hkbCharacter*>* charactersToReactivate=HK_NULL  );

		void unloadCharacterInstanceInternal( hkbCharacter* characterInstance );

		AnimationBindingSetInfo* getAnimationBindingSetInfo( hkbAnimationBindingSet* animationBindingSet );

		AnimationBindingSetInfo* accessAnimationBindingSetInfo( hkbAnimationBindingSet* animationBindingSet, CharacterInfo* characterInfo );

		void unlinkBehavior( hkbBehaviorGraph* behaviorTemplate );

			/// Project data mapped by project name.
		hkStorageStringMap<hkbProjectData*> m_projectDatas;
			/// Behavior data mapped by behavior name.
		hkStorageStringMap<hkbBehaviorGraph*> m_behaviorGraphs;
			/// Character info mapped by character name.
		hkStorageStringMap<CharacterInfo*> m_characterInfos;
			/// AnimationgBindingSetInfo mapped by animationBindingSet ptr.
		hkPointerMap<const hkbAnimationBindingSet*, AnimationBindingSetInfo*> m_animationBindingSetInfos;
			/// Asset loader used to load behavior related data.
		hkRefPtr<hkbAssetLoader> m_behaviorAssetLoader;
			/// Physics interface to resolve loading of physics objects.
		hkRefPtr<hkbPhysicsInterface> m_physicsInterface;
			/// Asset load counts mapped by asset ptr.  This is to ref count properly outside the hkbAssetLoader.
		hkPointerMap<void*,int> m_animationAssetsToLoadCountMap;
			/// How do we manage assets.
		hkEnum<hkbProjectAssetManager::AssetManagementMode, hkInt8> m_managementMode;
			/// Project dependencies.  Filled out by calling loadProject on a valid project.
		hkStorageStringMap<ProjectDependencies*> m_projectDependencies;
			/// Manages loading/caching script files.
		hkRefPtr<hkbScriptAssetLoader> m_scriptLoader;

	public:
			// ~hkbCharacter calls into unloadCharacterInternal
		friend class hkbCharacter;
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
