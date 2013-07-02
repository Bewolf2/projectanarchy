/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/HavokBehaviorEnginePlugin.hpp>

#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorComponent.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorResource.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorResourceManager.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokSync.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokVisualDebugger.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptIncludes.hpp>

#include <Behavior/Behavior/hkbBehavior.h>
#include <Behavior/Behavior/Multithreaded/hkbBehaviorJobQueueUtils.h>
#include <Behavior/Behavior/Character/hkbCharacter.h>
#include <Behavior/Behavior/Character/hkbCharacterData.h>
#include <Behavior/Behavior/World/hkbWorld.h>
#include <Behavior/Behavior/World/hkbWorldCinfo.h>
#include <Behavior/Utilities/VisualDebugger/hkbBehaviorContext.h>
#include <Behavior/Utilities/Utils/hkbBehaviorLinkingUtils.h>
#include <Behavior/Utilities/Utils/hkbOnHeapAssetLoader.h>
#include <Behavior/Utilities/Utils/hkbProjectAssetManager.h>

#include <Physics2012/Collide/Filter/Group/hkpGroupFilter.h>

#include <Common/Serialize/Util/hkVersionCheckingUtils.h>
#include <Common/Base/Reflection/Registry/hkDefaultClassNameRegistry.h>
#include <Common/Base/Reflection/Registry/hkTypeInfoRegistry.h>
#include <Common/Base/Reflection/Registry/hkVtableClassRegistry.h>

#include <Common/Serialize/Version/hkVersionPatchManager.h>
#if defined(HAVOK_SDK_VERSION_MAJOR) && (HAVOK_SDK_VERSION_MAJOR >= 2012)
#include <Common/Compat/hkHavokVersions.h>
#define HK_SERIALIZE_MIN_COMPATIBLE_VERSION_INTERNAL_VALUE HK_HAVOK_VERSION_300
void HK_CALL registerAnimationBehaviorPatches()
{
	hkVersionPatchManager& man = hkVersionPatchManager::getInstance();
	#define HK_EXCLUDE_COMMON_PATCHES
	#undef HK_FEATURE_PRODUCT_PHYSICS_2012
	#undef HK_FEATURE_PRODUCT_AI
	#undef HK_FEATURE_PRODUCT_CLOTH
	#undef HK_FEATURE_PRODUCT_DESTRUCTION_2012
	#define HK_FEATURE_PRODUCT_ANIMATION
	#define HK_FEATURE_PRODUCT_BEHAVIOR
#undef HK_FEATURE_PRODUCT_MILSIM
#undef HK_FEATURE_PRODUCT_PHYSICS
#undef HK_FEATURE_PRODUCT_DESTRUCTION

	#include <Common/Compat/Patches/hkRegisterPatches.cxx>
}
#else
	#define HK_SERIALIZE_MIN_COMPATIBLE_VERSION 650b1
	extern void HK_CALL registerAnimationPatches();
	extern void HK_CALL registerBehaviorPatches();
	void HK_CALL registerAnimationBehaviorPatches()
	{
		#define HK_EXCLUDE_COMMON_PATCHES
		#undef HK_FEATURE_PRODUCT_PHYSICS_2012
	    #undef HK_FEATURE_PRODUCT_AI
	    #undef HK_FEATURE_PRODUCT_CLOTH
	    #undef HK_FEATURE_PRODUCT_DESTRUCTION_2012
		#define HK_FEATURE_PRODUCT_ANIMATION
		#define HK_FEATURE_PRODUCT_BEHAVIOR
		registerAnimationPatches();
		registerBehaviorPatches();
	}
#endif

#ifdef VBASE_LIB
#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorClassReg.h>
#endif

// Static global manager for Behavior
vHavokBehaviorModule vHavokBehaviorModule::g_GlobalManager;


// OnUpdateSceneFinished is not handled in vHavokBehaviorModule::OnHandleCallback
// since we want to use a different sorting key for it which will cause the Entity
// LOD component's OnUpdateSceneFinished handler to be called before this one, i.e.
// the anim config should be switched before we apply the new pose to it
class vHavokBehaviorOnUpdateSceneFinishedHandler_cl : public IVisCallbackHandler_cl
{
	public:

		vHavokBehaviorOnUpdateSceneFinishedHandler_cl() {}

		VOVERRIDE void OnHandleCallback(IVisCallbackDataObject_cl *pData)
		{
			if( pData->m_pSender == &Vision::Callbacks.OnUpdateSceneFinished )
			{
				vHavokBehaviorModule::GetInstance()->UpdatePose();
			}
		}

		VOVERRIDE int GetCallbackSortingKey(VCallback *pCallback)
		{
			return 1000000;	// should be bigger than value returned by VEntityLODComponentManager::GlobalManager().GetCallbackSortingKey(pCallback)
		}
};

static vHavokBehaviorOnUpdateSceneFinishedHandler_cl g_HavokBehaviorOnUpdateSceneFinishedHandler;


vHavokBehaviorModule::vHavokBehaviorModule() :
	m_physicsModule(HK_NULL),
	m_behaviorWorld(HK_NULL),
	m_behaviorContext(HK_NULL),
	m_projectAssetManager(HK_NULL),
	m_assetLoader(HK_NULL),
	m_stepWorld(false),
	m_reinitializedPoses(false),
	m_physicsStepper(HK_NULL)
{
}

vHavokBehaviorModule::~vHavokBehaviorModule()
{
}

// should be called at plugin initialization time
void vHavokBehaviorModule::OneTimeInit()
{
	// Set up the behavior world
	if( m_behaviorWorld == HK_NULL )
	{
		// Register callbacks
		Vision::Callbacks.OnUpdateSceneBegin += this;
		Vision::Callbacks.OnUpdateSceneFinished += &g_HavokBehaviorOnUpdateSceneFinishedHandler;
		Vision::Callbacks.OnAfterSceneLoaded += this;
		vHavokPhysicsModule::OnAfterInitializePhysics += this;
		vHavokPhysicsModule::OnBeforeDeInitializePhysics += this;
	}

	// Create a VDB context
	HK_ASSERT(0x25427eb1, m_behaviorContext == HK_NULL );
	m_behaviorContext = new hkbBehaviorContext( getAssetLoader() );
}

// should be called at plugin de-initialization time
void vHavokBehaviorModule::OneTimeDeInit()
{
	vHavokBehaviorResourceManager::GetInstance()->OneTimeDeInit();

	// Unregister callbacks
	Vision::Callbacks.OnUpdateSceneBegin -= this;
	Vision::Callbacks.OnUpdateSceneFinished -= &g_HavokBehaviorOnUpdateSceneFinishedHandler;
	Vision::Callbacks.OnAfterSceneLoaded -= this;
	vHavokPhysicsModule::OnAfterInitializePhysics -= this;
	vHavokPhysicsModule::OnBeforeDeInitializePhysics -= this;

	// Context is deleted after VDB shutdown
	if (m_behaviorContext != HK_NULL)
	{
		m_behaviorContext->removeReference();
		m_behaviorContext = HK_NULL;
	}
}

void vHavokBehaviorModule::InitWorld( vHavokPhysicsModule* physicsModule )
{
	hkbWorldCinfo cinfo;
	m_behaviorWorld = new hkbWorld( cinfo );

	if(m_behaviorContext)
	{
		m_behaviorContext->addWorld( m_behaviorWorld );
	}

	vHavokBehaviorResourceManager* resourceManager = vHavokBehaviorResourceManager::GetInstance();
	m_assetLoader = new vHavokBehaviorAssetLoader( resourceManager );
	m_projectAssetManager = new hkbProjectAssetManager( m_assetLoader );

	m_scriptAssetLoader = new vHavokBehaviorScriptAssetLoader(resourceManager);
	m_projectAssetManager->setScriptAssetLoader(m_scriptAssetLoader);

	m_projectAssetManager->linkAll( m_behaviorWorld );
	resourceManager->OneTimeInit();

	// create the physics interface
	hkpWorld* physicsWorld = physicsModule->GetPhysicsWorld();
	m_physicsStepper = new vHavokPhysicsStepper( physicsWorld, physicsModule->GetJobQueue(), physicsModule->GetThreadPool() );

	// setup behavior world properties
	{
		m_behaviorWorld->setPhysicsWorld( physicsWorld );
		m_behaviorWorld->setPhysicsInterface( m_physicsStepper );
		physicsModule->SetSteppedExternally( true );
		physicsModule->SetVdbSteppedExternally(true);
		physicsModule->SetUseAsynchronousPhysics( false );

		// Set world gravity
		physicsWorld->markForWrite();
		{
			physicsModule->SetGroupsCollision( vHavokPhysicsModule::HK_LAYER_COLLIDABLE_RAGDOLL, vHavokPhysicsModule::HK_LAYER_COLLIDABLE_CONTROLLER, false );
			physicsModule->SetGroupsCollision( vHavokPhysicsModule::HK_LAYER_COLLIDABLE_RAGDOLL, vHavokPhysicsModule::HK_LAYER_COLLIDABLE_ATTACHMENTS, false  );
			physicsModule->SetGroupsCollision( vHavokPhysicsModule::HK_LAYER_COLLIDABLE_CONTROLLER, vHavokPhysicsModule::HK_LAYER_COLLIDABLE_ATTACHMENTS, false  );

			physicsModule->SetGroupsCollision( vHavokPhysicsModule::HK_LAYER_COLLIDABLE_FOOT_IK, vHavokPhysicsModule::HK_LAYER_COLLIDABLE_ATTACHMENTS, false  );
			physicsModule->SetGroupsCollision( vHavokPhysicsModule::HK_LAYER_COLLIDABLE_FOOT_IK, vHavokPhysicsModule::HK_LAYER_COLLIDABLE_RAGDOLL, false  );
			physicsModule->SetGroupsCollision( vHavokPhysicsModule::HK_LAYER_COLLIDABLE_FOOT_IK, vHavokPhysicsModule::HK_LAYER_COLLIDABLE_CONTROLLER, false  );

			physicsWorld->updateCollisionFilterOnWorld( HK_UPDATE_FILTER_ON_WORLD_FULL_CHECK, HK_UPDATE_COLLECTION_FILTER_PROCESS_SHAPE_COLLECTIONS );
		}
		physicsWorld->unmarkForWrite();
	}
}

void vHavokBehaviorModule::DeInitWorld()
{
	hkArray<vHavokBehaviorComponent*> charactersToCleanup;
	charactersToCleanup.append( m_visionCharacters );
	for( int i = 0; i < charactersToCleanup.getSize(); i++ )
	{
		charactersToCleanup[i]->SetOwner( HK_NULL );
	}

	m_visionCharacters.clearAndDeallocate();

	// Clean up
	if( m_behaviorWorld != HK_NULL )
	{
		m_behaviorWorld->removeReference();
		m_behaviorWorld = HK_NULL;
	}

	if( m_projectAssetManager != HK_NULL )
	{
		m_projectAssetManager->removeReference();
		m_projectAssetManager = HK_NULL;
	}

    if(m_scriptAssetLoader != HK_NULL)
    {
      m_scriptAssetLoader->removeReference();
      m_scriptAssetLoader = HK_NULL;
    }

	if( m_assetLoader != HK_NULL )
	{
		m_assetLoader->removeReference();
		m_assetLoader = HK_NULL;
	}

	if( m_physicsStepper != HK_NULL )
	{
		m_physicsStepper->removeReference();
		m_physicsStepper = HK_NULL;
	}
}

// implements IVisCallbackHandler_cl
void vHavokBehaviorModule::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
	// Setup
	// Register vision callbacks
	if( pData->m_pSender == &Vision::Callbacks.OnUpdateSceneBegin )
	{
		checkEditorModeChanged();
		stepModule();
	}
	else if( pData->m_pSender == &Vision::Callbacks.OnAfterSceneLoaded )
	{
		// Set physics module
		vHavokPhysicsModule* physicsModule = vHavokPhysicsModule::GetInstance();
		if( physicsModule != HK_NULL )
		{
			hkpWorld* physicsWorld = physicsModule->GetPhysicsWorld();

			// Set world gravity
			physicsWorld->markForRead();
			{
				hkVector4 up;
				up.setNeg3( physicsWorld->getGravity() );
				up.normalize3();

				m_behaviorWorld->setUp( up );
			}
			physicsWorld->unmarkForRead();
		}
		else
		{
			hkVector4 up; vHavokConversionUtils::VisVecToPhysVec_noscale( m_visionCharacters[0]->getEntityOwner()->GetObjDir_Up(), up );
			m_behaviorWorld->setUp( up );
		}

	}
	else if( pData->m_pSender == &Vision::Callbacks.OnAfterSceneUnloaded )
	{
/*#if defined(HAVOK_SDK_VERSION_MAJOR) && (HAVOK_SDK_VERSION_MAJOR >= 2012)
		// destroy all unreferenced navmesh instances
		m_projectAssetManager->clearAssets();

		hkbOnHeapAssetLoader* assetLoader = static_cast<hkbOnHeapAssetLoader*>( m_assetLoader );
		assetLoader->unloadAllAssets();

		HK_ASSERT(0x21e79cc1, m_visionCharacters.getSize() == 0 );

		// unload all unused resources
		vHavokBehaviorResourceManager::GetInstance()->PurgeUnusedResources();

		HK_ASSERT(0xc5b3c4e, vHavokBehaviorResourceManager::GetInstance()->GetResourceCount() == 0 );
#endif*/
	}
	else if( pData->m_pSender == &vHavokPhysicsModule::OnBeforeInitializePhysics )
	{
		vHavokPhysicsModuleCallbackData *pHavokData = (vHavokPhysicsModuleCallbackData*)pData;
		VISION_HAVOK_SYNC_STATICS();
		VISION_HAVOK_SYNC_PER_THREAD_STATICS( pHavokData->GetHavokModule() );

		// Register Behavior classes
		{
			hkDefaultClassNameRegistry& dcnReg	= hkDefaultClassNameRegistry::getInstance();
			hkTypeInfoRegistry&			tyReg	= hkTypeInfoRegistry::getInstance();
			hkVtableClassRegistry&		vtcReg	= hkVtableClassRegistry::getInstance();

#ifndef VBASE_LIB  // DLL, so have a full set 
			dcnReg.registerList(hkBuiltinTypeRegistry::StaticLinkedClasses);
			tyReg.registerList(hkBuiltinTypeRegistry::StaticLinkedTypeInfos);
			vtcReg.registerList(hkBuiltinTypeRegistry::StaticLinkedTypeInfos, hkBuiltinTypeRegistry::StaticLinkedClasses);
#else // Static lib, just need to add Behavior ones and reg the Behavior patches which would not have been done yet
			dcnReg.registerList(hkBuiltinBehaviorTypeRegistry::StaticLinkedClasses);
			tyReg.registerList(hkBuiltinBehaviorTypeRegistry::StaticLinkedTypeInfos);
			vtcReg.registerList(hkBuiltinBehaviorTypeRegistry::StaticLinkedTypeInfos, hkBuiltinBehaviorTypeRegistry::StaticLinkedClasses);
#endif
		}

		registerAnimationBehaviorPatches();
		hkVersionPatchManager::getInstance().recomputePatchDependencies();

		OneTimeInit();
	}
	else if( pData->m_pSender == &vHavokPhysicsModule::OnAfterInitializePhysics )
	{
		vHavokPhysicsModuleCallbackData *pHavokData = (vHavokPhysicsModuleCallbackData*)pData;
		vHavokPhysicsModule* physicsModule = pHavokData->GetHavokModule();
		InitWorld( physicsModule );
	}
	else if( pData->m_pSender == &vHavokPhysicsModule::OnBeforeDeInitializePhysics )
	{
		DeInitWorld();
	}
	else if( pData->m_pSender == &vHavokPhysicsModule::OnAfterDeInitializePhysics )
	{
		// Unload everything, the scene gets deleted!
		OneTimeDeInit();
	}
	else if( pData->m_pSender == &vHavokPhysicsModule::OnUnsyncHavokStatics )
	{
		vHavokPhysicsModuleCallbackData *pHavokData = (vHavokPhysicsModuleCallbackData*)pData;
		VISION_HAVOK_UNSYNC_STATICS();
		VISION_HAVOK_UNSYNC_PER_THREAD_STATICS( pHavokData->GetHavokModule() );
	}
	else if( pData->m_pSender == &vHavokPhysicsModule::OnBeforeWorldCreated )
	{
		vHavokBeforeWorldCreateDataObject_cl *pEventData = (vHavokBeforeWorldCreateDataObject_cl *)pData;
		hkJobQueue* jobQueue = pEventData->GetHavokModule()->GetJobQueue();
		if( jobQueue != HK_NULL )
		{
			hkbBehaviorJobQueueUtils::registerWithJobQueue( jobQueue );
		}
	}
	else if( pData->m_pSender == &vHavokVisualDebugger::OnCreatingContexts )
	{
		hkbBehaviorContext::registerAllBehaviorViewers();

		vHavokVisualDebuggerCallbackData_cl *pEventData = (vHavokVisualDebuggerCallbackData_cl *)pData;
		pEventData->m_contexts->pushBack( m_behaviorContext );
	}
	else if( pData->m_pSender == &vHavokVisualDebugger::OnAddingDefaultViewers )
	{
		vHavokVisualDebuggerCallbackData_cl *pEventData = (vHavokVisualDebuggerCallbackData_cl *)pData;
		if( pEventData->m_pVisualDebugger != HK_NULL )
		{
			hkbBehaviorContext::addDefaultViewers( pEventData->m_pVisualDebugger );
		}
	}
#ifdef VLUA_USE_SWIG_BINDING
	else if( pData->m_pSender == &IVScriptManager::OnRegisterScriptFunctions )
	{
		EnsureHavokBehaviorScriptRegistration();
	}
	else if (pData->m_pSender==&IVScriptManager::OnScriptProxyCreation)
	{
		VScriptCreateStackProxyObject * pScriptData = (VScriptCreateStackProxyObject *)pData;

		// process data only if no other callback did that
		if(!pScriptData->m_bProcessed)
		{
			int iRetParams = 0;

			// call appropriate LUA cast function
			if(pScriptData->m_pInstance->IsOfType(V_RUNTIME_CLASS(vHavokBehaviorComponent)))
			{
				iRetParams = LUA_CallStaticFunction( pScriptData->m_pLuaState, "Behavior", "vHavokBehaviorComponent", "Cast", "E>E",  pScriptData->m_pInstance );
			}

			// could we handle the object?
			if(iRetParams>0)
			{
				// the cast failed if the result is a nil value
				if(lua_isnil(pScriptData->m_pLuaState, -1))
					lua_pop(pScriptData->m_pLuaState, iRetParams); //in case of a nil value we drop the params from the lua stack
				else
					pScriptData->m_bProcessed = true; //avoid further processing
			}
		}
	}
#endif

}

vHavokBehaviorModule* vHavokBehaviorModule::GetInstance()
{
	return &g_GlobalManager;
}


void vHavokBehaviorModule::checkEditorModeChanged()
{
	// Get the editor mode
	VisEditorManager_cl::EditorMode_e editorMode = Vision::Editor.GetMode();
	

	// Step only if the editor is playing, or when not in the editor
	bool shouldStep = (editorMode == VisEditorManager_cl::EDITORMODE_PLAYING_IN_EDITOR) ||
					  (editorMode == VisEditorManager_cl::EDITORMODE_PLAYING_IN_GAME) ||
					  (!Vision::Editor.IsInEditor());

	// Mode is no longer playing and we were stepping before, make sure we reinit poses next step
	if( !shouldStep && m_stepWorld )
	{
		m_reinitializedPoses = false;
	}

	m_stepWorld = shouldStep;
}

void vHavokBehaviorModule::stepModule()
{
	if( m_stepWorld || !m_reinitializedPoses )
	{
		if( m_behaviorWorld != HK_NULL )
		{
			if( !m_reinitializedPoses )
			{
				for( int i = 0; i < m_visionCharacters.getSize(); i++ )
				{
					m_visionCharacters[i]->SingleStepCharacter();
				}

				m_reinitializedPoses = true;
			}
			else
			{
				float timestep = Vision::GetTimer()->GetTimeDifference();

				// Step Behavior
				vHavokPhysicsModule* physicsModule = vHavokPhysicsModule::GetInstance();
				if( physicsModule != HK_NULL )
				{
					physicsModule->ClearVisualDebuggerTimerData();
					if (m_visionCharacters.getSize() == 0) // skip behavior then
					{
						m_physicsStepper->step( timestep );
					}
					else
					{
						m_behaviorWorld->step( timestep, physicsModule->GetJobQueue(), physicsModule->GetThreadPool() );
					}
					physicsModule->StepVisualDebugger();
				}
				else
				{
					m_behaviorWorld->step( timestep );
				}
			}
		}
	}
    else
	{
		// If the engine isn't playing, make sure to keep the Havok transforms updated
		// in case user changes transform in vision
		for( int i = 0; i < m_visionCharacters.getSize(); i++ )
		{
			m_visionCharacters[i]->UpdateHavokTransformFromVision();
		}
	}
}

void vHavokBehaviorModule::UpdatePose()
{
	// Update the Vision properties after every step
	for( int i = 0; i < m_visionCharacters.getSize(); i++ )
	{
		m_visionCharacters[i]->OnAfterHavokUpdate();
	}
}


hkbCharacter* vHavokBehaviorModule::addCharacter( vHavokBehaviorComponent* character )
{
	hkbCharacter* behaviorCharacter = HK_NULL;

	if( m_behaviorWorld != HK_NULL && character->m_projectName.GetLen() > 0 )
	{
		hkStringBuf projectPath;
		character->GetProjectPath( projectPath );

#if defined(HAVOK_SDK_VERSION_MAJOR) && (HAVOK_SDK_VERSION_MAJOR >= 2012)
		// Load the resource (or grab from cache)
		vHavokBehaviorResource* resource = static_cast<vHavokBehaviorResource*>( vHavokBehaviorResourceManager::GetInstance()->LoadResource( projectPath.cString() ) );
		character->SetResource( resource );

		if( resource == HK_NULL )
		{
			Vision::Error.Warning( "Failed to load resource from %s", projectPath.cString() );
		}
		else
#else
		m_projectAssetManager->loadProject( projectPath.cString() );
#endif
		{
			// Create the character
			if( character->m_characterName.GetLen() > 0 )
			{
#if defined(HAVOK_SDK_VERSION_MAJOR) && (HAVOK_SDK_VERSION_MAJOR >= 2012)
				char const *projectName = VFileHelper::GetFilename(character->m_projectName);
				behaviorCharacter = m_projectAssetManager->createCharacter(projectName, character->m_characterName, character->m_behaviorName, m_behaviorWorld );
#else
				// Link the behavior.  This is done automatically in createCharacter in Havok 2012.1+
				if( behaviorCharacter != HK_NULL && m_behaviorWorld != HK_NULL )
				{
					hkbBehaviorGraph* behaviorTemplate = behaviorCharacter->getBehavior()->getTemplate();
					if( behaviorTemplate != HK_NULL )
					{
						if( behaviorTemplate->isLinked() )
						{
							hkbBehaviorLinkingUtils::unlinkBehavior( *behaviorTemplate );
						}
						
						m_behaviorWorld->link(behaviorCharacter, behaviorTemplate);
					}
				}
				behaviorCharacter = m_projectAssetManager->createCharacter( character->m_projectName, character->m_characterName, character->m_behaviorName );
#endif
					
				// Only add the character if it's valid
				if( behaviorCharacter != HK_NULL )
				{
					// Update the collision filters before adding to the world
					character->UpdateCollisionFilters( behaviorCharacter );

					// Set user data to owner component
					behaviorCharacter->m_userData = (hkUlong)character;

					// Add the character to the behavior world
					m_behaviorWorld->addCharacter( behaviorCharacter );
				}
			}
		}
	}

	if( behaviorCharacter != HK_NULL )
	{
	    // Add new character to the rest of the characters' nearby list (used for sensing)
	    for( int i = 0; i < m_visionCharacters.getSize(); i++ )
	    {
		    hkbCharacter* existingCharacter = m_visionCharacters[i]->m_character;
			if( existingCharacter != HK_NULL )
			{
		    	behaviorCharacter->m_nearbyCharacters.pushBack( existingCharacter );
			    existingCharacter->m_nearbyCharacters.pushBack( behaviorCharacter );
			}
	    }
	}

	m_visionCharacters.pushBack( character );
	return behaviorCharacter;
}


void vHavokBehaviorModule::removeCharacter( vHavokBehaviorComponent* character )
{
	// Detaching
	hkbWorld* behaviorWorld = m_behaviorWorld;
	if( behaviorWorld != HK_NULL && character->m_character != HK_NULL && character->m_character->getWorld() != HK_NULL)
	{
		m_behaviorWorld->removeCharacter( character->m_character );
	}

	int indexToRemove = -1;
	for( int i = 0; i < m_visionCharacters.getSize(); i++ )
	{
		if( m_visionCharacters[i] == character )
		{
			indexToRemove = i;
		}
		else if( character->m_character != HK_NULL )
		{
			hkbCharacter* existingCharacter = m_visionCharacters[i]->m_character;
			if( existingCharacter != HK_NULL )
			{
			    int nearbyIndex = existingCharacter->m_nearbyCharacters.indexOf( character->m_character );
			    if( nearbyIndex >= 0 )
			    {
				    existingCharacter->m_nearbyCharacters.removeAt( nearbyIndex );
			    }
			}
		}
	}

	// Remove the character from the list of characters
	if( indexToRemove >= 0 )
	{
		m_visionCharacters.removeAt( indexToRemove );
		if( character->m_character != HK_NULL ) 
		{
			character->m_character->m_nearbyCharacters.clear();
		}
	}
}

hkbProjectAssetManager* vHavokBehaviorModule::GetProjectAssetManager() const
{
	return m_projectAssetManager;
}

// ----------------------------------------------------------------------------

vHavokPhysicsStepper::vHavokPhysicsStepper( hkpWorld* world, hkJobQueue* jobQueue, hkJobThreadPool* jobThreadPool ) 
	: hkbpPhysicsInterface( world, jobQueue, jobThreadPool )
{
}

void vHavokPhysicsStepper::step( hkReal timestep )
{
	vHavokPhysicsModule* physicsModule = vHavokPhysicsModule::GetInstance();
	if( physicsModule != HK_NULL )
	{
		physicsModule->SetSteppedExternally( false );
		physicsModule->OnRunPhysics( timestep );
		physicsModule->WaitForSimulationToComplete();
		physicsModule->SetSteppedExternally( true );
	}
}

// ----------------------------------------------------------------------------

extern "C" int luaopen_Behavior(lua_State *);

//Helper function to ensure that the Havok Behavior script module has been loaded
void EnsureHavokBehaviorScriptRegistration()
{
	IVScriptManager* pSM = Vision::GetScriptManager();
	if(pSM)
	{
		lua_State* pLuaState = ((VScriptResourceManager*)pSM)->GetMasterState();
		if(pLuaState)
		{
			luaopen_Behavior(pLuaState);
		}
		else
		{
			Vision::Error.Warning("Unable to create Lua Havok Behavior Module, lua_State is NULL");
		}
	}
}

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
