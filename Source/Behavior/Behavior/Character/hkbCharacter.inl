/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE void hkbCharacter::setName( const char* name )
{
	m_name = name;
}

HK_FORCE_INLINE const hkbCharacterSetup* hkbCharacter::getSetup() const
{
	return m_setup;
}

HK_FORCE_INLINE hkbCharacterSetup* hkbCharacter::accessSetup() const
{
	return m_setup;
}

HK_FORCE_INLINE hkbRagdollDriver* hkbCharacter::getRagdollDriver() const
{
	return m_ragdollDriver;
}

HK_FORCE_INLINE hkbCharacterControllerDriver* hkbCharacter::getCharacterControllerDriver() const
{
	return m_characterControllerDriver;
}

HK_FORCE_INLINE hkbFootIkDriver* hkbCharacter::getFootIkDriver() const
{
	return m_footIkDriver;
}

HK_FORCE_INLINE hkbHandIkDriver* hkbCharacter::getHandIkDriver() const
{
	return m_handIkDriver;
}

HK_FORCE_INLINE hkbDockingDriver* hkbCharacter::getDockingDriver() const
{
	return m_dockingDriver;
}

HK_FORCE_INLINE void hkbCharacter::setAiControlDriver( hkbAiControlDriver* aiControlDriver )
{
	m_aiControlDriver = reinterpret_cast<hkReferencedObject*>(aiControlDriver);
}

HK_FORCE_INLINE hkbAiControlDriver* hkbCharacter::getAiControlDriver() const
{
	return reinterpret_cast<hkbAiControlDriver*>(m_aiControlDriver.val());
}

HK_FORCE_INLINE const hkbAnimationBindingSet* hkbCharacter::getAnimationBindingSet() const
{
	if ( m_animationBindingSet != HK_NULL )
	{
		return m_animationBindingSet;
	}
	else if ( m_setup != HK_NULL )
	{
		return m_setup->m_animationBindingSet;
	}
	else
	{
		return HK_NULL;
	}
}

HK_FORCE_INLINE void hkbCharacter::setSetup( hkbCharacterSetup* setup )
{
	m_setup = setup;
	allocateGeneratorOutput();
}

HK_FORCE_INLINE void hkbCharacter::setRagdollInterface( hkbRagdollInterface* ragdoll )
{	
	m_ragdollDriver->setRagdollInterface(ragdoll);
	allocateGeneratorOutput();
}

HK_FORCE_INLINE hkbBehaviorGraph* hkbCharacter::getBehavior() const
{
	return m_behaviorGraph;
}

HK_FORCE_INLINE const hkbProjectData* hkbCharacter::getProjectData() const
{
	return m_projectData;
}

HK_FORCE_INLINE void hkbCharacter::setProjectData(hkbProjectData* projectData)
{
	m_projectData = projectData;
}

HK_FORCE_INLINE hkReal hkbCharacter::getWorldFromModelFeedback() const
{
	if ( m_footIkDriver != HK_NULL )
	{
		return m_footIkDriver->getWorldFromModelFeedback();
	}

	return 0.0f;
}

HK_FORCE_INLINE hkbEventQueue* hkbCharacter::getEventQueue() const
{
	return m_eventQueue;
}

HK_FORCE_INLINE hkbGeneratorOutput* hkbCharacter::getGeneratorOutput() const
{
	return m_generatorOutput;
}

HK_FORCE_INLINE void hkbCharacter::setWorldFromModel( const hkQsTransform& worldFromModel )
{
	m_generatorOutput->accessWorldFromModel() = worldFromModel;	
}

HK_FORCE_INLINE const hkQsTransform& hkbCharacter::getWorldFromModel() const
{
	return m_generatorOutput->getWorldFromModel();
}

HK_FORCE_INLINE hkQsTransform& hkbCharacter::accessWorldFromModel() const
{
	return m_generatorOutput->accessWorldFromModel();
}

HK_FORCE_INLINE const hkQsTransform* hkbCharacter::getPoseLocal() const
{	
	return m_generatorOutput->getPoseLocal();
}

HK_FORCE_INLINE hkQsTransform* hkbCharacter::accessPoseLocal() const
{	
	return m_generatorOutput->accessPoseLocal();
}

HK_FORCE_INLINE int hkbCharacter::getNumPoseLocal() const
{
	return m_generatorOutput->getNumPoseLocal();
}

HK_FORCE_INLINE hkbRagdollInterface* hkbCharacter::getRagdollInterface() const
{
	return getRagdollDriver()->getRagdollInterface();
}

HK_FORCE_INLINE hkbWorld* hkbCharacter::getWorld() const
{
	return m_world;
}

HK_FORCE_INLINE const hkArray<hkaBoneAttachment*>& hkbCharacter::getBoneAttachments() const
{
	return m_boneAttachments;
}

HK_FORCE_INLINE hkbProjectAssetManager* hkbCharacter::getAssetManager() const
{
	return m_assetManager;
}

HK_FORCE_INLINE void hkbCharacter::setAssetManager( hkbProjectAssetManager* assetManager )
{
	if ( m_assetManager )
	{
		m_assetManager->unloadCharacterInstanceInternal( this );
	}
	m_assetManager = assetManager;
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
