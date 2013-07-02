/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_SCENE_MODIFIER_UTILS_H
#define HKB_SCENE_MODIFIER_UTILS_H

#include <Behavior/Utilities/Physics/hkbPhysicsBaseTypes.h>

class hkbAttachmentManager;
class hkbCharacter;
class hkbSceneModifier;
struct hkbSceneCharacters;

enum hkbSceneModifierIndexEnum
{
	HKB_SCENE_MODIFIER_ATTACHMENT = 0,
	HKB_SCENE_MODIFIER_DOCKING,
	HKB_SCENE_MODIFIER_CHARACTER_CONTROLLER,
	HKB_SCENE_MODIFIER_FOOT_IK,
	HKB_SCENE_MODIFIER_HAND_IK,
	HKB_SCENE_MODIFIER_RAGDOLL,
	HKB_SCENE_MODIFIER_ATTACHMENT_FIXUP,
	HKB_SCENE_MODIFIER_HAND_IK_FIXUP,
};

	/// A scene modifier can affect all of the characters in the scene.
class hkbSceneModifier : public hkReferencedObject
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR_RUNTIME);

			/// Apply the scene modifier to all of the characters in the scene.
		virtual void modify( hkbSceneCharacters& sceneCharacters, float timestep ) = 0;
};

	/// A scene modifier that runs the attachment manager.
class hkbAttachmentSceneModifier : public hkbSceneModifier
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR_RUNTIME);

		hkbAttachmentSceneModifier();

			/// hkbSceneModifier interface implementation.
		virtual void modify( hkbSceneCharacters& sceneCharacters, float timestep ) HK_OVERRIDE;
		
			/// The attachment manager that will take care of the attachments.
		hkbAttachmentManager* m_attachmentManager;		
};

	/// A scene modifier that does attachment fixup using the attachment manager.
class hkbAttachmentFixupSceneModifier : public hkbSceneModifier
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR_RUNTIME);

		hkbAttachmentFixupSceneModifier();

			/// hkbSceneModifier interface implementation.
		virtual void modify( hkbSceneCharacters& sceneCharacters, float timestep ) HK_OVERRIDE;

			/// The attachment manager that will take care of the attachments.
		hkbAttachmentManager* m_attachmentManager;
};

	/// A scene modifier that processes the ragdoll and steps the physics world if it hasn't been stepped
	/// already (typically by the hkbCharacterControllerSceneModifier).
class hkbRagdollSceneModifier : public hkbSceneModifier
{
	public :
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR_RUNTIME);

		hkbRagdollSceneModifier();

			/// hkbSceneModifier interface implementation.
		virtual void modify( hkbSceneCharacters& sceneCharacters, float timestep ) HK_OVERRIDE;
		
			/// The hkbPhysicsInterface to step.
		hkbPhysicsInterface* m_physicsInterface;
};

	/// A scene modifier that drives the character controller and steps the physics world.
	///
	/// By default the modifier always steps the physics world but you can choose to have it
	/// do so only if necessary (it's necessary if you are using the hkbCharacterRigidBodyDriver).
class hkbCharacterControllerSceneModifier : public hkbSceneModifier
{
	public :
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR_RUNTIME);

		hkbCharacterControllerSceneModifier();

			/// hkbSceneModifier interface implementation.
		virtual void modify( hkbSceneCharacters& sceneCharacters, float timestep ) HK_OVERRIDE;
		
			/// The hkbPhysicsInterface to step.
		hkbPhysicsInterface* m_physicsInterface;

			/// When using the hkbCharacterRigidBodyDriver the physics must be stepped.  Otherwise, it's optional.
			/// If this is true physics is stepped in either case.
		bool m_stepPhysicsOnlyIfNecessary;
};

	/// A scene modifier that performs foot IK.
class hkbFootIkSceneModifier : public hkbSceneModifier
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR_RUNTIME);

			/// hkbSceneModifier interface implementation.
		virtual void modify( hkbSceneCharacters& sceneCharacters, float timestep ) HK_OVERRIDE;
};

	/// A scene modifier that performs hand IK.
class hkbHandIkSceneModifier : public hkbSceneModifier
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR_RUNTIME);

			/// hkbSceneModifier interface implementation.
		virtual void modify( hkbSceneCharacters& sceneCharacters, float timestep ) HK_OVERRIDE;
};

	/// A scene modifier that performs hand IK.
class hkbHandIkFixupSceneModifier : public hkbSceneModifier
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR_RUNTIME);

			/// hkbSceneModifier interface implementation.
		virtual void modify( hkbSceneCharacters& sceneCharacters, float timestep ) HK_OVERRIDE;
};

	/// A scene modifier that performs docking.
class hkbDockingSceneModifier : public hkbSceneModifier
{
    public:
	    HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR_RUNTIME);
    
	    /// hkbSceneModifier interface implementation.
	    virtual void modify( hkbSceneCharacters& sceneCharacters, float timestep ) HK_OVERRIDE;
};

	/// Helper functions for dealing with scene modifiers.
class hkbSceneModifierUtils
{
	public:
			/// Perform all of the scene modifiers and post-scene-modifiers on a set of characters.
		static void HK_CALL applySceneModifiers(	hkbSceneCharacters& sceneCharacters,
													hkbSceneModifier** sceneModifiers,
													int numSceneModifiers,
													float timestep );
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
