/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_VISIONBEHAVIOR_COMPONENT_H
#define HK_VISIONBEHAVIOR_COMPONENT_H

#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorResource.hpp>

class hkbCharacter;
class AttachedEntity_cl;
class hkaiCharacter;
class hkaiBehavior;

class vHavokBehaviorComponent : public IVObjectComponent
{
	public:

		V_DECLARE_SERIAL_DLLEXP(vHavokBehaviorComponent, VHAVOKBEHAVIOR_IMPEXP);
		V_DECLARE_VARTABLE( vHavokBehaviorComponent, VHAVOKBEHAVIOR_IMPEXP )

			/// This ctor should only set values to null state, no actual init should go here
		VHAVOKBEHAVIOR_IMPEXP vHavokBehaviorComponent();
		VHAVOKBEHAVIOR_IMPEXP virtual ~vHavokBehaviorComponent() {}

		VOVERRIDE void SetOwner(VisTypedEngineObject_cl *pOwner);
		VOVERRIDE BOOL CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut);
		VOVERRIDE VVarChangeRes_e OnVariableValueChanging(VisVariable_cl *pVar, const char * value);
		VOVERRIDE bool SetVariable(const char * name, const char * value);
		VOVERRIDE void OnVariableValueChanged(VisVariable_cl *pVar, const char * value);

#if defined(WIN32) || defined(_VISION_DOC)
		VHAVOKBEHAVIOR_IMPEXP virtual void GetVariableAttributes(VisVariable_cl *pVariable, VVariableAttributeInfo &destInfo) HKV_OVERRIDE;
#endif

		// IVObjectComponent interface
		void MessageFunction(int id, INT_PTR paramA, INT_PTR paramB) HKV_OVERRIDE;

		/// Gets called after every Havok step.  Used to sync the Vision transforms
		VHAVOKBEHAVIOR_IMPEXP void OnAfterHavokUpdate();

		/// Update the Havok character's transform
		VHAVOKBEHAVIOR_IMPEXP void UpdateHavokTransformFromVision();

		/// Updates the collision filters on any subsystems of the character that need it
		VHAVOKBEHAVIOR_IMPEXP void UpdateCollisionFilters( hkbCharacter* character );

		/// Accessor for entity's owner
		VHAVOKBEHAVIOR_IMPEXP inline const VisBaseEntity_cl* getEntityOwner() { return m_entityOwner; }

		/// Single step the character.  Used mainly in the tool to update the pose when not simulating
		VHAVOKBEHAVIOR_IMPEXP void SingleStepCharacter();

		/// Gets a normalized project path
		void GetProjectPath( hkStringBuf& projectPath );

		/// Set the character's resource
		void SetResource( vHavokBehaviorResource* resource );

		// --- Lua API Start ---

		// Returns behavior character's world-from-model transform
		VHAVOKBEHAVIOR_IMPEXP const hkQsTransform& GetWorldFromModel() const;

		// Returns whether the specified node is active
		VHAVOKBEHAVIOR_IMPEXP bool IsNodeActive(const char* nodeName);

		// Sets the value of the selected behavior float variable
		VHAVOKBEHAVIOR_IMPEXP void SetFloatVar(const char* variableName, float value);

		// Sets the value of the selected behavior word variable
		VHAVOKBEHAVIOR_IMPEXP void SetWordVar(const char* variableName, int value);

		// Sets the value of the selected behavior float variable
		VHAVOKBEHAVIOR_IMPEXP void SetBoolVar(const char* variableName, bool value);

		// Triggers a behavior event
		VHAVOKBEHAVIOR_IMPEXP void TriggerEvent(const char* eventName);

		// Returns world space transform of the selected bone
		VHAVOKBEHAVIOR_IMPEXP void GetBoneTransform(const char* boneName, hkvVec3& outPos, hkvMat3& outRot ) const;

		/// Initialize this character
		VHAVOKBEHAVIOR_IMPEXP void InitVisionCharacter( VisBaseEntity_cl* entityOwner );

		// --- Lua API End  ---

		/// Serialization / Resource
		VHAVOKBEHAVIOR_IMPEXP VOVERRIDE void AssertValid();
		VHAVOKBEHAVIOR_IMPEXP VOVERRIDE void Serialize(VArchive &ar);
		VHAVOKBEHAVIOR_IMPEXP VOVERRIDE void OnSerialized(VArchive &ar);
#ifdef SUPPORTS_SNAPSHOT_CREATION
		VHAVOKBEHAVIOR_IMPEXP VOVERRIDE void GetDependencies(VResourceSnapshot &snapshot);
#endif

	protected:

		/// Updates the anim config and bone index list.  Usually happens after a new mesh is assigned
		void UpdateAnimationAndBoneIndexList();

		// Cleanup
		void Deinit();
		
		/// Update ragdoll driver and character controller driver based on
		/// values of m_enableRagdoll and m_useBehaviorWorldFromModel.
		void UpdateBehaviorPhysics();

	public:

		/// The Havok character
		hkbCharacter* m_character;

		/// Path to behavior project
		VString m_projectPath;

		/// Name of the behavior project
		VString m_projectName;

		/// Name of the character in the project
		VString m_characterName;

		/// Name of the behavior graph
		VString m_behaviorName;

		/// Whether to enable the ragdoll of the character, if one exists
		BOOL m_enableRagdoll;

		/// Whether to allow Behavior to modify the character's worldFromModel
		BOOL m_useBehaviorWorldFromModel;

	protected:

		/// Base entity
		VisBaseEntity_cl* m_entityOwner;

		/// Bone mapping array : index is Havok, value at index is Vision.
		// -1 means no Vision bone exists for Havok bone
		hkArray< int > m_boneIndexList;

		/// Character's resource
		VSmartPtr<vHavokBehaviorResource> m_resource;
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
