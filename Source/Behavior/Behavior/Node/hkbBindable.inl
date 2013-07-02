/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE hkbBindable::hkbBindable()
:	m_areBindablesCached(false),
	m_hasEnableChanged(false)
{
}

HK_FORCE_INLINE hkbBindable::hkbBindable( const hkbBindable& bindable )
:	hkReferencedObject(bindable),
	m_variableBindingSet(bindable.m_variableBindingSet),
	m_areBindablesCached(false),
	m_hasEnableChanged(false)
{
}

HK_FORCE_INLINE hkbVariableBindingSet* hkbBindable::getVariableBindings() const
{
	return m_variableBindingSet;
}

HK_FORCE_INLINE bool hkbBindable::hasBindings() const
{
	HK_ASSERT2( 0x4c73b21b, ( m_variableBindingSet == HK_NULL ) || ( m_variableBindingSet->numBindings() > 0 ), "empty binding sets are wasteful" );

	return ( m_variableBindingSet != HK_NULL );
}

HK_FORCE_INLINE bool hkbBindable::hasEnableChanged() const
{
	return m_hasEnableChanged;
}

HK_FORCE_INLINE void hkbBindable::orHasEnableChanged( bool b )
{
	m_hasEnableChanged = (bool)m_hasEnableChanged || b;
}

HK_FORCE_INLINE void hkbBindable::clearHasEnableChanged()
{
	m_hasEnableChanged = false;
}

HK_FORCE_INLINE hkBool32 HK_CALL hkbBindable::copyVariablesToMembersInternal(
	hkbBindable* bindable,
	hkbSyncVariableSetup& syncVariableSetup,
	hkbVariableBindingSet& variableBindingSet,
	hkbBehaviorGraph& rootBehavior,
	bool reverse )
{
	hkBool32 hasOutputBindings = variableBindingSet.hasOutputBinding();

	if ( !reverse || hasOutputBindings )
	{
		variableBindingSet.initMemberOffsets( bindable );
		variableBindingSet.copyVariablesToMembers( syncVariableSetup, reverse, bindable, &rootBehavior );
	}

	return hasOutputBindings;
}

HK_FORCE_INLINE void HK_CALL hkbBindable::copyVariableToEnableInternal(
	hkbBindable* bindable,
	hkbSyncVariableSetup& syncVariableSetup,
	hkbVariableBindingSet& variableBindingSet,
	hkbBehaviorGraph& rootBehavior )
{
	if ( variableBindingSet.getIndexOfBindingToEnable() != -1 )
	{
		variableBindingSet.initMemberOffsets( bindable );
		variableBindingSet.copyVariableToEnable( syncVariableSetup, bindable, &rootBehavior );
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
