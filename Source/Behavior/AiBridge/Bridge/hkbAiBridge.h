/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_AI_BRIDGE_H
#define HKB_AI_BRIDGE_H

#include <Behavior/Behavior/World/hkbWorldListener.h>
#include <Behavior/AiBridge/Utilities/hkbAiObjectFactory.h>

class hkbWorld;
class hkaiWorld;
class hkbCharacter;
class hkbAiControlSceneModifier;

/// The main bridge between an hkbWorld and an hkaiWorld. 
/// 
/// This should be constructed by user code after the hkbWorld and the 
/// hkaiWorld are constructed, and will manage the creation of HKAI-side 
/// representations for hkbCharacters which are subsequently added to the 
/// hkbWorld and which have AI driver control information in their setup data.
/// A reference to the bridge should be held by user code as long as
/// AI-controlled characters are in the hkbWorld.
class hkbAiBridge : public hkReferencedObject, private hkbWorldListener
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR);

	hkbAiBridge(hkbWorld* bWorld, hkaiWorld* aiWorld, hkbAiObjectFactory* aiObjectFactory = HK_NULL);
	~hkbAiBridge();

	/// If true, step() will be called from hkbWorld::step() by a scene modifier.
	/// This is true by default. You can set it to false if you want to step the
	/// AI world manually.
	hkBool m_stepAutomatically;

	/// Return the AI world used to construct the bridge.
	hkaiWorld* getAiWorld() { return m_aiWorld; }

	/// Return the object factory used to construct the bridge, or the
	/// auto-constructed hkbDefaultAiObjectFactory if no object factory was
	/// passed to the constructor.
	hkbAiObjectFactory* getAiObjectFactory() { return m_aiObjectFactory; }

private:
	/// hkbWorldListener callbacks
	virtual void characterAddedCallback(hkbCharacter* character) HK_OVERRIDE;
	virtual void characterRemovedCallback(hkbCharacter* character) HK_OVERRIDE;
	virtual void characterDeactivatedCallback(hkbCharacter* character) HK_OVERRIDE;
	virtual void characterBehaviorChangedCallback(hkbCharacter* character, hkbBehaviorGraph* oldBehavior);

private:
	hkRefPtr<hkbWorld> m_bWorld;
	hkRefPtr<hkaiWorld> m_aiWorld;
	hkRefPtr<hkbAiControlSceneModifier> m_sceneModifier;
	hkRefPtr<hkbAiObjectFactory> m_aiObjectFactory;
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
