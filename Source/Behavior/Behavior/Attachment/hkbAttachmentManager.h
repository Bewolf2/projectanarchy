/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_ATTACHMENT_MANAGER_H
#define HKB_ATTACHMENT_MANAGER_H

#include <Behavior/Utilities/Physics/hkbPhysicsBaseTypes.h>

class hkbCharacter;
class hkbAttachmentSetup;
class hkCriticalSection;
struct hkbAttachmentInstance;
struct hkbAttachmentTargets;
struct hkbSceneCharacters;

/// A manager for attachments.  The attachment manager implements various methods of attachment.
/// It maintains a topologically sorted list of attachments, and updates the attachments in
/// "attacher before attachee" order.
class hkbAttachmentManager
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbAttachmentManager );

			/// If a physics interface isn't provided at construction, it must be provided later before any methods are called
			/// (see setPhysicsInterface).
		hkbAttachmentManager( hkbPhysicsInterface* physicsInterface = HK_NULL );
		~hkbAttachmentManager();

			/// Form an attachment.  
		hkbAttachmentInstance* attach(hkbAttachmentSetup* setup, const hkbAttachmentTargets& targets );

			/// Release an attachment.
			/// 
			/// You should pass in an hkbAttachmentInstance that was returned by attach().
		void detach( hkbAttachmentInstance* attachment, bool deleteAttachment=true );

			/// Detach all attachments involving the given character.
		void detachCharacter( hkbCharacter* character );
			/// Update all attachments. If the attachments are being updated for doing fixup
			/// after the physics update then true should be passed for the fixup parameter.
		void update( hkReal timestep, hkbSceneCharacters& sceneCharacters, bool fixup = false );

			/// Returns the list of current attachments.
		const hkArray<hkbAttachmentInstance*>& getAttachments() const {  return m_attachments; }

			/// Set the physics interface, this must be set before any methods can be called
		void setPhysicsInterface( hkbPhysicsInterface* physicsInterface );

			/// Gets current physics interface used by this attachment manager
		const hkbPhysicsInterface* getPhysicsInterface() const;

	private:

			// insert an attachment into the list (maintaining topological sort)
		void insert( hkbAttachmentInstance* attachment );

			// update an attachment that uses rigid body keyframing
		void updateKeyframeRigidBody( hkReal timestep, hkbAttachmentInstance* attachment, hkbSceneCharacters& sceneCharacters );

			// update an attachment that sets the worldFromModel
		void updateSetWorldFromModel( hkReal timestep, hkbAttachmentInstance* attachment, hkbSceneCharacters& sceneCharacters, bool fixup );

			// add a constraint to enforce an attachment
		void addConstraint( hkbAttachmentInstance* attachment );

			// remove the constraint enforcing an attachment
		void removeConstraint( hkbAttachmentInstance* attachment );

	private:

			// a list of attachments
		hkArray<hkbAttachmentInstance*> m_attachments;

			// Physics Interface used by the attachment manager to use physics attachments
		hkRefPtr<hkbPhysicsInterface> m_physicsInterface;

			// A critical section that is used when making modifications to the manager that could happen during a multithreaded update().
		hkCriticalSection* m_criticalSection;

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
