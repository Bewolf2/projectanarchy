/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_WORLD_LISTENER_H
#define HKB_WORLD_LISTENER_H

class hkbWorld;
class hkbEvent;
class hkbBehaviorGraph;
class hkbCharacter;
class hkMeshBody;
class hkaMeshBinding;

	/// Callback interface for hkbWorld.  Objects that want notification of hkbWorld related events should inherit
	/// from hkbWorldListener and then add themselves as a listener to hkbWorld.
class hkbWorldListener
{
	//+hk.MemoryTracker(ignore=True)
	public:				
			/// Empty destructor
		virtual ~hkbWorldListener() {}

			/// Called when a character is added to world.
		virtual void characterAddedCallback( hkbCharacter* character ) {}

			/// Called when acharacter is removed from world.
		virtual void characterRemovedCallback( hkbCharacter* character ) {}

			/// Called when the root behavior for a character is changed.
		virtual void characterBehaviorChangedCallback( hkbCharacter* character, hkbBehaviorGraph* oldBehavior ) {}

			/// Called when the root behavior for a character changes its active nodes list.
		virtual void activeNodesChangedCallback( hkbCharacter* character ) {}

			/// Called when a character is activated.
		virtual void characterActivatedCallback( hkbCharacter* character ) {}

			/// Called when a character is deactivated.
		virtual void characterDeactivatedCallback( hkbCharacter* character ) {}
		
			/// Called when an event is raised in a character's behavior.
		virtual void eventRaisedCallback( hkbCharacter* character, const hkbEvent& event, bool raisedBySdk ) {}
		
			/// Called after the world generates a character's pose.
		virtual void postGenerateCallback( hkbCharacter* character, hkReal deltaTime ) {}

			/// Called after a character has been linked.
		virtual void postLinkCallback( hkbCharacter* character, hkbBehaviorGraph* templateBehavior ) {}

			/// Called when a world is deleted.
		virtual void worldDeletedCallback( hkbWorld* world ) {}
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
