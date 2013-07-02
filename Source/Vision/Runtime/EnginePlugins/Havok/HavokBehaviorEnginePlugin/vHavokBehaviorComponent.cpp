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
#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorResourceManager.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>

#include <Behavior/Behavior/Character/hkbCharacter.h>
#include <Behavior/Behavior/World/hkbWorld.h>
#include <Behavior/Behavior/Character/hkbCharacterData.h>
#include <Behavior/Behavior/Event/hkbEventQueue.h>

#include <Animation/Animation/Rig/hkaSkeletonUtils.h>
#include <Animation/Physics2012Bridge/Instance/hkaRagdollInstance.h>
#include <Animation/Physics2012Bridge/Utils/hkaRagdollUtils.h>
#include <Animation/Animation/Rig/hkaBoneAttachment.h>

#include <Physics2012/Collide/Filter/Group/hkpGroupFilter.h>
#include <Physics2012/Collide/Agent/Collidable/hkpCollidable.h>

#include <Common/Serialize/Util/hkBuiltinTypeRegistry.h>
#include <Common/Base/Reflection/Registry/hkClassNameRegistry.h>
#include <Common/Base/Container/LocalArray/hkLocalArray.h>


namespace
{
	void PopulateStrList(hkArray<hkStringPtr> const& stringsArray, VStrList *strList)
	{
		for(hkArray<hkStringPtr>::const_iterator it = stringsArray.begin(), end = stringsArray.end(); it != end; ++it)
		{
			char const *fileName = VFileHelper::GetFilename((*it).cString()); 
			strList->AddString(fileName);
		}
	}
}

// register the class in the engine module so it is available for RTTI
V_IMPLEMENT_SERIAL( vHavokBehaviorComponent, IVObjectComponent, 0, &g_vHavokBehaviorModule );

vHavokBehaviorComponent::vHavokBehaviorComponent()
{
	m_character = HK_NULL;
	m_entityOwner = HK_NULL;
	m_enableRagdoll = TRUE;
	m_useBehaviorWorldFromModel = TRUE;
}

void vHavokBehaviorComponent::SetOwner(VisTypedEngineObject_cl *pOwner)
{
	IVObjectComponent::SetOwner(pOwner);

	// Insert code here to respond to attaching this component to an object
	// This function is called with pOwner==NULL when de-attaching.
	vHavokBehaviorModule* behaviorModule = vHavokBehaviorModule::GetInstance();
	if( behaviorModule != HK_NULL )
	{
		if( pOwner != HK_NULL )
		{
			// Init
			InitVisionCharacter( (VisBaseEntity_cl*)pOwner );
		}
		else
		{
			Deinit();
		}
	}
}

BOOL vHavokBehaviorComponent::CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
	if (!IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut))
	{
		return FALSE;
	}

	// Define criteria here that allows the editor to attach this component to the passed object.
	// In our example, the object should be derived from VisObject3D_cl to be positionable.
	bool bIsValidType = pObject->IsOfType(V_RUNTIME_CLASS(VisBaseEntity_cl));

	if (!bIsValidType)
	{
		// Provide detailed information why the component can't be added.
		// This information will be displayed to the level designed in vForge.
		sErrorMsgOut = "Component can only be added to instances of VisBaseEntity_cl or derived classes.";
		return FALSE;
	}

	return TRUE;
}

VVarChangeRes_e vHavokBehaviorComponent::OnVariableValueChanging(VisVariable_cl *pVar, const char * value)
{
	// To avoid redundant and costly variable set commands when nothing is actually changing
	bool valueChanged = true;
	switch( pVar->type )
	{
		case VULPTYPE_VSTRING:
		{
			VString currentValue;
			pVar->GetValueDirect( const_cast<vHavokBehaviorComponent*>( this ), (void*)&currentValue );
			valueChanged = ( hkString::strCasecmp( currentValue, value ) != 0 );
			break;
		}
		case VULPTYPE_BOOL:
		{
			BOOL isEnabled;
			pVar->GetValueDirect( const_cast<vHavokBehaviorComponent*>( this ), (void*)&isEnabled );
			valueChanged = ( isEnabled != ( hkString::strCasecmp( value, "True" ) == 0 ) );
			break;
		}
		default:
		{
			// This function is not prepared to deal with this variable type
			VASSERT(false);
			break;
		}
	}

	return valueChanged ? VCHANGE_IS_ALLOWED : VCHANGE_IS_REDUNDANT;
}

bool vHavokBehaviorComponent::SetVariable(const char * name, const char * value)
{
	if (!strcmp(name,"m_projectName") && !m_projectPath.IsEmpty() )
	{		
		VString fullPath = m_projectPath;
		AppendRelativePath(fullPath, value);

		hkStringBuf projectPath = fullPath.GetSafeStr();
		projectPath.pathNormalize();

		m_projectName = projectPath;
		m_projectPath = "";
#if defined (WIN32)
		Vision::Editor.SetVariableInEditor( this, "m_projectPath", "", false, false );
		Vision::Editor.SetVariableInEditor( this, "Project", m_projectName, false, false );
#endif

		return true;
	}
	else
	{
		return IVObjectComponent::SetVariable(name, value);
	}
}

void vHavokBehaviorComponent::UpdateBehaviorPhysics()
{
	if( m_character != HK_NULL )
	{
		if ( m_character->getRagdollDriver() != HK_NULL )
		{
			m_character->getRagdollDriver()->setEnabled( m_useBehaviorWorldFromModel && m_enableRagdoll );
		}
		if ( m_character->getCharacterControllerDriver() != HK_NULL )
		{
			m_character->getCharacterControllerDriver()->setEnabled( m_useBehaviorWorldFromModel );
		}
	}
}

void vHavokBehaviorComponent::OnVariableValueChanged(VisVariable_cl *pVar, const char * value)
{
	// Early out if we are simulating in the Editor
	if( Vision::Editor.IsInEditor() && Vision::Editor.IsPlaying() )
		return;

	// In editor, exiting playmode, after SetOwner called with NULL, OnVariableValueChanged will be called for vartable entries
	VisBaseEntity_cl *const entityOwner = vstatic_cast<VisBaseEntity_cl*>(GetOwner());
	if(!entityOwner)
		return;

#if defined (WIN32)
	if ( hkString::strCmp( pVar->name, "m_projectName" ) == 0 )
	{
		// reset the previous setting of the Character and the Behavior properties
		m_characterName = "";
		m_behaviorName = "";
		Vision::Editor.SetVariableInEditor( this, "Character", "", false, false );
		Vision::Editor.SetVariableInEditor( this, "Behavior", "", false, false );
	}
	else if ( hkString::strCmp( pVar->name, "m_characterName" ) == 0 )
	{
		// reset the previous setting of the Behavior property
		m_behaviorName = "";
		Vision::Editor.SetVariableInEditor( this, "Behavior", "", false, false );
	}
#endif


	bool characterExists = false;
	// Make sure this character is in our module before attempting to do anything on variable change
	vHavokBehaviorModule* behaviorModule = vHavokBehaviorModule::GetInstance();
	for( int i = 0; i < behaviorModule->m_visionCharacters.getSize(); i++ )
	{
		if( behaviorModule->m_visionCharacters[i] == this )
		{
			characterExists = true;
			if ( hkString::strCmp( pVar->name, "m_enableRagdoll" ) == 0 || hkString::strCmp( pVar->name, "m_useBehaviorWorldFromModel" ) == 0 )
			{
				UpdateBehaviorPhysics();
			}
			else
			{
				// Cleanup
				Deinit();

				// Reinit
				InitVisionCharacter( entityOwner );
			}

			// Step the character once to update pose
			SingleStepCharacter();
			break;
		}
	}

	// If the character doesn't exist yet, try to create it
	if(!characterExists)
	{
		InitVisionCharacter(entityOwner);
		SingleStepCharacter();
	}
}

#if defined(WIN32) || defined(_VISION_DOC)

void vHavokBehaviorComponent::GetVariableAttributes(VisVariable_cl *pVariable, VVariableAttributeInfo &destInfo)
{
	// Hide an obsolete field
	if (!strcmp(pVariable->GetName(),"m_projectPath"))
	{
		destInfo.m_bHidden = true;
	}
}

#endif

// IVObjectComponent interface
void vHavokBehaviorComponent::MessageFunction(int id, INT_PTR paramA, INT_PTR paramB)
{
	IVObjectComponent::MessageFunction(id, paramA, paramB);

	switch(id)
	{
	case VIS_MSG_EDITOR_GETSTANDARDVALUES:
		{
			char const *key = reinterpret_cast<char const*>(paramA);
			VStrList *stringList = reinterpret_cast<VStrList*>(paramB);

			if(m_resource)
			{
				vHavokBehaviorResourceManager *resMgr = static_cast<vHavokBehaviorResourceManager*>(m_resource->GetParentManager());

				VASSERT(resMgr && resMgr->GetProjectAssetManager());
				if(resMgr && resMgr->GetProjectAssetManager())
				{
					char const *projectName = VFileHelper::GetFilename(m_projectName.AsChar());
					hkbProjectData const *const projectData = resMgr->GetProjectAssetManager()->findProjectData(projectName);

					VASSERT(projectData && projectData->getStringData());
					if(projectData && projectData->getStringData())
					{
						if(0 == hkString::strCmp(key, "Character"))
							PopulateStrList(projectData->getStringData()->m_characterFilenames, stringList);
						else if(0 == hkString::strCmp(key, "Behavior"))
							PopulateStrList(projectData->getStringData()->m_behaviorFilenames, stringList);
					}
				}
			}
		}
	}
}

void vHavokBehaviorComponent::Deinit()
{
	vHavokBehaviorModule* behaviorModule = vHavokBehaviorModule::GetInstance();
	if( behaviorModule != HK_NULL )
	{
		behaviorModule->removeCharacter( this );
		if( m_character != HK_NULL )
		{
			m_character->removeReference();
			m_character = HK_NULL;
		}

		m_entityOwner = HK_NULL;
		m_boneIndexList.clear();
	}
}

void vHavokBehaviorComponent::InitVisionCharacter( VisBaseEntity_cl* entityOwner )
{
	m_entityOwner = entityOwner;

	vHavokBehaviorModule* behaviorModule = vHavokBehaviorModule::GetInstance();
	if( behaviorModule != HK_NULL )
	{
		HK_ASSERT2(0x2f1c46e9, m_character == HK_NULL, "Character should be freed first." );

		// Try to create a character.  May fail if path/file settings are not specified
		m_character = behaviorModule->addCharacter( this );
		if( m_character != HK_NULL )
		{
			// Set up the animation config and create bone mapping
			UpdateAnimationAndBoneIndexList();

			// Set character transform
			UpdateHavokTransformFromVision();

			// Update the Physics
			UpdateBehaviorPhysics();
		}
	}
}

void vHavokBehaviorComponent::UpdateAnimationAndBoneIndexList()
{
	// Set up the animation config
	VDynamicMesh* mesh = m_entityOwner->GetMesh();
	if( mesh != HK_NULL && mesh->GetSkeleton() != HK_NULL )
	{
		// create an anim config, if one is not present
		if ( m_entityOwner->GetAnimConfig() == HK_NULL )
		{
			VisAnimFinalSkeletalResult_cl* finalSkeletalResult = HK_NULL;
			VisAnimConfig_cl* pConfig = VisAnimConfig_cl::CreateSkeletalConfig(mesh, &finalSkeletalResult);
			m_entityOwner->SetAnimConfig( pConfig );
		}

		// Create mapping
		VisSkeleton_cl* visionSkeleton = mesh->GetSkeleton();
		const hkaSkeleton* havokSkeleton = m_character->getSetup()->m_animationSkeleton;

		for( int i = 0; i < havokSkeleton->m_bones.getSize(); i++ )
		{
			const VHashString &boneName = havokSkeleton->m_bones[i].m_name.cString();
			int visionBoneIndex = visionSkeleton->GetBoneIndexByName( boneName );
			m_boneIndexList.pushBack( visionBoneIndex );
		}
	}
}

void vHavokBehaviorComponent::UpdateCollisionFilters( hkbCharacter* character )
{
	// Handle ragdoll
	hkbRagdollDriver* ragdollDriver = character->getRagdollDriver();
	vHavokPhysicsModule* physicsModule = vHavokPhysicsModule::GetInstance();
	if( ragdollDriver != HK_NULL && physicsModule != HK_NULL )
	{
		hkpWorld* physicsWorld = physicsModule->GetPhysicsWorld();
		if( physicsWorld != HK_NULL )
		{
			hkaRagdollInstance* ragdoll = ragdollDriver->getRagdoll();
			if( ragdoll != HK_NULL )
			{
				hkpGroupFilter* filter = physicsModule->GetGroupCollisionFilter();

				// Ragdoll shouldn't collide with its own constrained bodies
				hkaRagdollUtils::setCollisionLayer( ragdoll, vHavokPhysicsModule::HK_LAYER_COLLIDABLE_RAGDOLL, filter );
			}

			// Handle character controller
			hkbCharacterControllerDriver* characterControllerDriver = character->getCharacterControllerDriver();
			if( characterControllerDriver != HK_NULL )
			{
				// Change the data
				hkbCharacterControllerSetup& cinfo = character->accessSetup()->accessData()->m_characterControllerSetup;
				int newInfo = hkpGroupFilter::setLayer( cinfo.m_rigidBodySetup.m_collisionFilterInfo, vHavokPhysicsModule::HK_LAYER_COLLIDABLE_CONTROLLER );
				cinfo.m_rigidBodySetup.m_collisionFilterInfo = newInfo;
			}

			// Handle foot IK
			hkbFootIkDriverInfo* driverInfo = character->accessSetup()->accessData()->m_footIkDriverInfo;
			if( driverInfo != HK_NULL )
			{
				int newInfo = hkpGroupFilter::setLayer( driverInfo->m_collisionFilterInfo, vHavokPhysicsModule::HK_LAYER_COLLIDABLE_FOOT_IK );
				driverInfo->m_collisionFilterInfo = newInfo;
			}
		}
	}

	// Replace the shape used by the character controller with a correctly oriented capsule
	hkbCharacterControllerDriver* characterControllerDriver = character->getCharacterControllerDriver();
	if( characterControllerDriver != HK_NULL )
	{
		const hkbCharacterData* characterData = character->getSetup()->getData();			
					
		const hkReal totalHeight = characterData->m_characterControllerSetup.m_rigidBodySetup.m_shapeSetup.m_capsuleHeight;
		const hkReal radius = characterData->m_characterControllerSetup.m_rigidBodySetup.m_shapeSetup.m_capsuleRadius;

		hkVector4 upWS;
		upWS.setRotatedDir( character->getWorldFromModel().getRotation(), characterData->m_modelUpMS );

		hkVector4 vertexA = upWS;
		vertexA.mul4( totalHeight - radius );
		hkVector4 vertexB = upWS;
		vertexB.mul4( radius );
		
		/*hkpCapsuleShape* capsule = new hkpCapsuleShape(vertexA, vertexB, radius);
		characterControllerDriver->setShape( capsule );	
		capsule->removeReference();*/
	}
}


void vHavokBehaviorComponent::OnAfterHavokUpdate()
{
	if( m_character == HK_NULL || m_entityOwner == HK_NULL || m_entityOwner->GetMesh() == HK_NULL || m_entityOwner->GetMesh()->GetSkeleton() == HK_NULL )
	{
		return;
	}

	VisAnimConfig_cl* animConfig = m_entityOwner->GetAnimConfig();
	if( !animConfig )
	{
		return;
	}

	VisAnimFinalSkeletalResult_cl* skeletalResult = animConfig->GetFinalResult();
	if( !skeletalResult )
	{
		return;
	}

	// Try updating the bone index list in case a mesh was added
	if( m_boneIndexList.getSize() == 0 )
	{
		UpdateAnimationAndBoneIndexList();

		// Exit early if there's no bone index list
		if( m_boneIndexList.getSize() == 0 )
		{
			return;
		}
	}

	// Convert pose to Havok model space
	const hkQsTransform* poseLocal = m_character->getPoseLocal();
	hkArray<hkQsTransform> poseModel( m_character->getNumPoseLocal() );
	hkaSkeletonUtils::transformLocalPoseToModelPose( m_character->getNumPoseLocal(), m_character->getSetup()->m_animationSkeleton->m_parentIndices.begin(), poseLocal, poseModel.begin() );
	const hkQsTransform* pose = poseModel.begin();

	float const inverseCharacterScale = 1.0f / m_character->getSetup()->getData()->m_scale;

	// Convert pose to vision units
	VisSkeleton_cl* visionSkeleton = m_entityOwner->GetMesh()->GetSkeleton();

	
	for( int havokBoneIndex = 0; havokBoneIndex < m_character->getNumPoseLocal(); havokBoneIndex++ )
	{
		// Find the bone index
		int visionBoneIndex = m_boneIndexList[havokBoneIndex];
		if( visionBoneIndex != -1 )
		{
			HK_ON_DEBUG( VisSkeletalBone_cl* bone = visionSkeleton->GetBone(visionBoneIndex) );
			HK_ASSERT2(0x68b6649, hkString::strCmp( bone->m_sBoneName.AsChar(), m_character->getSetup()->m_animationSkeleton->m_bones[havokBoneIndex].m_name.cString() ) == 0, "" );

			const hkQsTransform& transform = pose[havokBoneIndex];

			// Convert Havok pose to Vision pose
			hkvQuat quat;
			vHavokConversionUtils::HkQuatToVisQuat( transform.getRotation(), quat );
			hkvVec3 scale; vHavokConversionUtils::PhysVecToVisVec_noscale( transform.getScale(), scale );
			hkvVec3 translation; vHavokConversionUtils::PhysVecToVisVecWorld( transform.getTranslation(), translation );

			// Behavior propagates character scale through the skeleton; need to compensate when scaling to Vision
			scale *= inverseCharacterScale;

			// Set the skeletal result
			skeletalResult->SetCustomBoneScaling( visionBoneIndex, scale, VIS_REPLACE_BONE | VIS_OBJECT_SPACE );
			skeletalResult->SetCustomBoneRotation( visionBoneIndex, quat, VIS_REPLACE_BONE | VIS_OBJECT_SPACE );
			skeletalResult->SetCustomBoneTranslation( visionBoneIndex, translation, VIS_REPLACE_BONE | VIS_OBJECT_SPACE );
		}
	}

	// Update WFM of skin or override it
	if ( m_useBehaviorWorldFromModel )
	{
		const hkQsTransform& worldFromModel = m_character->getWorldFromModel();

		// Copy the Behavior result into vision
		hkvMat3 visionRotation;
		hkvVec3 visionTranslation;
		vHavokConversionUtils::HkQuatToVisMatrix( worldFromModel.getRotation(), visionRotation );
		vHavokConversionUtils::PhysVecToVisVecWorld( worldFromModel.getTranslation(), visionTranslation );
		m_entityOwner->SetPosition( visionTranslation );
		m_entityOwner->SetRotationMatrix( visionRotation );
	}
	else
	{
		// Override Behavior results.
		// This will currently cause Behavior data being sent to HBT during remote debug to be one frame off.
		// However, the only other solution with the current APIs is to disable motion accumulation on *all*
		// Characters.  A slight delay/offset in the special case of remote debugging in HBT isn't worth that change.
		UpdateHavokTransformFromVision();
	}

#if 0

	// Draw skeleton
	for( int i = 0; i < visionSkeleton->GetBoneCount(); i++ )
	{
		VisSkeletalBone_cl* bone = visionSkeleton->GetBone(i);
		if( bone->m_iParentIndex != -1 )
		{
			hkvVec3 translation, subTranslation;
			hkvQuat rotation, subRotation;
			m_entityOwner->GetBoneCurrentWorldSpaceTransformation( i, translation, rotation );
			m_entityOwner->GetBoneCurrentWorldSpaceTransformation( bone->m_iParentIndex, subTranslation, subRotation );

			Vision::Game.DrawSingleLine( translation.x, translation.y, translation.z, subTranslation.x, subTranslation.y, subTranslation.z );
		}
	}

#endif

}


void vHavokBehaviorComponent::UpdateHavokTransformFromVision()
{
	if( m_character != HK_NULL )
	{
		/// Convert the transform from Vision to Havok
		hkQsTransform worldTransform;
		hkQuaternion quat;
		vHavokConversionUtils::VisMatrixToHkQuat( m_entityOwner->GetRotationMatrix(), quat );
		worldTransform.setRotation( quat );

		hkVector4 p; vHavokConversionUtils::VisVecToPhysVecWorld(m_entityOwner->GetPosition(),p);
		worldTransform.setTranslation( p );
		hkVector4 s; vHavokConversionUtils::VisVecToPhysVec_noscale(m_entityOwner->GetScaling(), s);
		worldTransform.setScale( s );

		// Set the Havok character's transform
		m_character->setWorldFromModel( worldTransform );
	}
}

void vHavokBehaviorComponent::SingleStepCharacter()
{
	if( m_character != HK_NULL )
	{
		hkbWorld* behaviorWorld = m_character->getWorld();
		if( behaviorWorld != HK_NULL )
		{
			behaviorWorld->singleStepCharacter( m_character );
		}
	}
}

void vHavokBehaviorComponent::SetResource( vHavokBehaviorResource* resource )
{
	m_resource = resource;
}

void vHavokBehaviorComponent::AssertValid()
{
	VASSERT(m_resource);
	VASSERT(m_resource->IsLoaded());
}

static unsigned int s_iSerialVersion = 2; // need to increment this everytime Serialize function is modified.
void vHavokBehaviorComponent::Serialize(VArchive &ar)
{
	IVObjectComponent::Serialize(ar);
	if (ar.IsLoading())
	{
		// Handle project 
		unsigned int iVersion = 0;
		ar >> iVersion;
		VASSERT((iVersion > 0) && (iVersion <= s_iSerialVersion));

		if ( iVersion <= 1 )
		{
			VString projectPath;
			ar >> projectPath;
			ar >> m_projectName;

			AppendRelativePath( projectPath, m_projectName );

			hkStringBuf fullPath = projectPath.GetSafeStr();
			fullPath.pathNormalize();

			m_projectName = fullPath;
		}
		else
		{
			ar >> m_projectName;
		}

		ar >> m_characterName;
		ar >> m_behaviorName;
		ar >> m_enableRagdoll;

		if(iVersion > 1)
		{
			ar >> m_useBehaviorWorldFromModel;
		}

		hkStringBuf fullProjectPath;
		GetProjectPath( fullProjectPath );
		m_resource = (vHavokBehaviorResource*)(vHavokBehaviorResourceManager::GetInstance()->LoadResource( fullProjectPath.cString() ));
	}
	else
	{
		ar << s_iSerialVersion;
		ar << m_projectName;
		ar << m_characterName;
		ar << m_behaviorName;
		ar << m_enableRagdoll;
		ar << m_useBehaviorWorldFromModel;			// added in version 2
	}
}

void vHavokBehaviorComponent::OnSerialized(VArchive &ar)
{
	VisTypedEngineObject_cl::OnSerialized(ar);
	AssertValid();
}

#if defined(SUPPORTS_SNAPSHOT_CREATION)
void vHavokBehaviorComponent::GetDependencies(VResourceSnapshot &snapshot)
{
	VisTypedEngineObject_cl::GetDependencies(snapshot);
	if (m_resource)
	{
		m_resource->GetDependencies(snapshot);
	}
}
#endif

void vHavokBehaviorComponent::GetProjectPath( hkStringBuf& projectPath )
{
	// Get full path
	VString fullProjectPath = m_projectName;

	// If path is not absolute, use the current scene directory as the base path
	if( !VFileHelper::IsAbsolutePath( fullProjectPath ) )
	{
		for( int i = 0; i < Vision::File.GetMaxDataDirectoryCount(); ++i )
		{
			VString dir = Vision::File.GetDataDirectory(i);
			if( !dir.IsEmpty() )
			{
				AppendRelativePath( dir, fullProjectPath );
					
				// Resolve path to clean up extraneous slashes
				char resultPath[FS_MAX_PATH];
				VFileHelper::ResolvePath( resultPath, dir );

				if( VFileHelper::Exists( resultPath ) )
				{
					fullProjectPath = resultPath;
					break;
				}
			}
		}
	}

	projectPath = fullProjectPath.GetSafeStr();
	projectPath.pathNormalize();
}

const hkQsTransform& vHavokBehaviorComponent::GetWorldFromModel() const
{
	return m_character->getWorldFromModel();
}

bool vHavokBehaviorComponent::IsNodeActive(const char* nodeName)
{
	if ( m_character != HK_NULL )
	{
		hkbBehaviorGraph* behavior = m_character->getBehavior();
		const hkbBehaviorGraph::NodeList& activeNodes = behavior->getActiveNodes();
		for( int i = 0; i < activeNodes.getSize(); ++i )
		{
			const hkbNodeInfo* nodeInfo = activeNodes[i];

			if( (nodeInfo != HK_NULL) && (nodeInfo->m_nodeClone->m_name == nodeName) )
			{
				return true;
			}
		}
	}

	return false;
}

void vHavokBehaviorComponent::SetFloatVar(const char* variableName, float value)
{
	// If there is an error we may not have a character.
	// But Script will still happily call into this function.
	if ( m_character != HK_NULL )
	{
		hkbWorld* world = m_character->getWorld();
		hkbBehaviorGraph* behavior = m_character->getBehavior();
		int idx = world->getVariableId(variableName);

		if (idx >= 0)
		{
			behavior->setVariableValueWord( idx, value, true );
		}
	}
}

void vHavokBehaviorComponent::SetWordVar(const char* variableName, int value)
{
	hkbWorld* world = m_character->getWorld();
	hkbBehaviorGraph* behavior = m_character->getBehavior();
	int idx = world->getVariableId(variableName);

	if (idx >= 0)
	{
		behavior->setVariableValueWord( idx, value, true );
	}
}

void vHavokBehaviorComponent::SetBoolVar(const char* variableName, bool value)
{
	// If there is an error we may not have a character.
	// But Script will still happily call into this function.
	if ( m_character != HK_NULL )
	{
		hkbWorld* world = m_character->getWorld();
		hkbBehaviorGraph* behavior = m_character->getBehavior();
		int idx = world->getVariableId(variableName);

		if (idx >= 0)
		{
			behavior->setVariableValueWord<hkUint8>( idx, value );
		}
	}
}

void vHavokBehaviorComponent::TriggerEvent(const char* eventName)
{
	hkbWorld* world = m_character->getWorld();
	hkbBehaviorGraph* behavior = m_character->getBehavior();

	int idx = world->getEventId(eventName);

	if ( idx >=0 )
	{
		m_character->getEventQueue()->enqueueWithExternalId(idx);
	}
}

void vHavokBehaviorComponent::GetBoneTransform( const char* boneName, hkvVec3& outPos, hkvMat3& outRot ) const
{
	VDynamicMesh* mesh = m_entityOwner->GetMesh();
	VisAnimConfig_cl* animConfig = m_entityOwner->GetAnimConfig();
	if ( !mesh || !animConfig )
	{
		return;
	}

	VisSkeleton_cl* skeleton = mesh->GetSkeleton();
	if ( !skeleton )
	{
		return;
	}

	int boneIdx = skeleton->GetBoneIndexByName( boneName );
	if ( boneIdx < 0 )
	{
		return;
	}

	VisAnimFinalSkeletalResult_cl* poseCache = animConfig->GetFinalResult();
	const VisSkeletalAnimResult_cl* pose = poseCache->GetCurrentObjectSpaceResult();
	hkvVec3 localToObjectPos = pose->GetBoneTranslation( boneIdx );
	hkvQuat localToObjectRot = pose->GetBoneRotation( boneIdx );

	// transform to world space
	hkvVec3 objectToWorldPos = m_entityOwner->GetPosition();
	hkvQuat objectToWorldRot = m_entityOwner->GetRotationMatrix().getAsQuaternion();

	hkvQuat worldRot = objectToWorldRot.multiply( localToObjectRot );
	outRot = worldRot.getAsMat3();

	outPos = objectToWorldRot.transform( localToObjectPos );
	outPos += objectToWorldPos;
}

START_VAR_TABLE(vHavokBehaviorComponent, IVObjectComponent, "Havok Behavior Character", VVARIABLELIST_FLAGS_NONE, "Havok Behavior Character" )
	// TODO remove m_projectPath once character data has caught up
	DEFINE_VAR_VSTRING      (vHavokBehaviorComponent, m_projectPath, "Defines the path of Havok Behavior project created in HBT.", "", 1024, 0, 0);
	DEFINE_VAR_VSTRING_AND_NAME(vHavokBehaviorComponent, m_projectName, "Project", "Animation project to use", "", 0, 0, "filepicker(.hkt)");
	DEFINE_VAR_VSTRING_AND_NAME(vHavokBehaviorComponent, m_characterName, "Character", "The name of the character file from HBT.", "", 0, 0, "dropdown(Character)");
	DEFINE_VAR_VSTRING_AND_NAME(vHavokBehaviorComponent, m_behaviorName, "Behavior", "The name of the behavior file from HBT.", "", 0, 0, "dropdown(Behavior)");
	DEFINE_VAR_BOOL_AND_NAME(vHavokBehaviorComponent, m_enableRagdoll, "Enable Ragdoll", "With this and m_useBehaviorWorldFromModel set to true, Ragdolls which are present in Behavior characters will be simulated.", "TRUE", 0, 0);
	DEFINE_VAR_BOOL_AND_NAME(vHavokBehaviorComponent, m_useBehaviorWorldFromModel, "Use Behavior World From Model", "With this set to true, Behaviors will affect the characters worldFromModel. If set to false, a Behavior's ragdoll, character controller, and worldFromModel will be disabled.", "TRUE", 0, 0);
END_VAR_TABLE

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
