/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE hkbCharacter* hkbContext::getCharacter() const 
{ 
	return m_character; 
}

HK_FORCE_INLINE const hkbProjectData* hkbContext::getProjectData() const 
{ 
	return m_character->getProjectData(); 
}

HK_FORCE_INLINE hkbBehaviorGraph* hkbContext::getRootBehavior() const
{
	return m_rootBehavior != HK_NULL ? m_rootBehavior : m_character->getBehavior();
}

HK_FORCE_INLINE hkbBehaviorGraph* hkbContext::getBehavior() const
{
	return m_behavior;
}

HK_FORCE_INLINE hkbRagdollInterface* hkbContext::getRagdollInterface() const
{
	return m_character->getRagdollInterface();
}

HK_FORCE_INLINE hkbEventQueue* hkbContext::getEventQueue() const
{
	if ( m_eventQueue != HK_NULL )
	{
		return m_eventQueue;
	}
	else
	{
		return m_character->getEventQueue();
	}
}

HK_FORCE_INLINE const hkQsTransform& hkbContext::getWorldFromModel() const
{
	return m_character->getWorldFromModel();
}

HK_FORCE_INLINE const hkQsTransform* hkbContext::getPoseLocal() const
{
	return m_character->getPoseLocal();
}

HK_FORCE_INLINE hkQsTransform* hkbContext::accessPoseLocal() const
{
	return m_character->accessPoseLocal();
}

HK_FORCE_INLINE const hkbCharacterSetup* hkbContext::getCharacterSetup() const
{
	return m_character->getSetup();
}

HK_FORCE_INLINE hkbGeneratorOutputListener* hkbContext::getOutputListener() const
{
	return m_generatorOutputListener;
}

HK_FORCE_INLINE void* hkbContext::getLuaState() const
{
	return m_character->getLuaState();
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
