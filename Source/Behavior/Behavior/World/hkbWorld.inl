/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE const hkbSymbolLinker& hkbWorld::getEventLinker()
{
	return m_eventLinker;
}

HK_FORCE_INLINE hkbSymbolLinker& hkbWorld::accessEventLinker()
{
	return m_eventLinker;
}

HK_FORCE_INLINE hkbSymbolLinker& hkbWorld::accessVariableLinker()
{
	return m_variableLinker;
}

HK_FORCE_INLINE hkbSymbolLinker& hkbWorld::accessAttributeLinker()
{
	return m_attributeLinker;
}

HK_FORCE_INLINE hkbSymbolLinker& hkbWorld::accessCharacterPropertyLinker()
{
	return m_characterPropertyLinker;
}

HK_FORCE_INLINE hkbAttachmentManager* hkbWorld::getAttachmentManager() const
{
	return m_attachmentManager;
}

HK_FORCE_INLINE hkBool32 hkbWorld::isMultithreading() const
{
	return m_isMultithreading;
}

HK_FORCE_INLINE hkbSharedEventQueue* hkbWorld::getSharedEventQueue() const
{
	return m_sharedEventQueue;
}

HK_FORCE_INLINE int hkbWorld::getScriptDebuggerPort() const
{
	return m_scriptDebuggerPort;
}

HK_FORCE_INLINE hkbUnloadedAnimationListener* hkbWorld::getUnloadedAnimationListener() const
{
	return m_unloadedAnimationListener;
}

HK_FORCE_INLINE void hkbWorld::setUnloadedAnimationListener( hkbUnloadedAnimationListener* unloadedAnimationListener )
{
	m_unloadedAnimationListener = unloadedAnimationListener;
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
