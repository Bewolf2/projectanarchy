/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE hkbBehaviorGraphStringData* hkbBehaviorGraphData::getStringData() 
{ 
	return m_stringData; 
}

HK_FORCE_INLINE hkbEventInfo hkbBehaviorGraphData::getEventInfo( int eventId )
{
	if ( eventId < m_eventInfos.getSize() )
	{
		return m_eventInfos[eventId];
	}
	else
	{
		HK_ASSERT2( 0x561b83c9, false, "eventId out of range of m_eventInfos" );
		return hkbEventInfo();
	}
}

// note that this should not be inlined
template <typename T>
int hkbBehaviorGraphData::addVariableWord( const char* name, hkbVariableInfo::VariableType type, T value, T minValue, T maxValue )
{
	hkbVariableInfo info;
	info.m_type = type;
	info.m_role.m_role = hkbRoleAttribute::ROLE_DEFAULT;
	info.m_role.m_flags = hkbRoleAttribute::FLAG_NONE;
	m_stringData->m_variableNames.pushBack( name );
	m_variableInfos.pushBack( info );

	hkbVariableBounds bounds;
	bounds.m_min.set<T>( minValue );
	bounds.m_max.set<T>( maxValue );
	m_variableBounds.pushBack( bounds );

	return m_variableInitialValues->addWord<T>( value );
}

HK_FORCE_INLINE void hkbBehaviorGraphData::addVariableBool( const char* name, hkbVariableInfo::VariableType type, bool value )
{
	addVariableWord<bool>( name, type, value, false, true );
}

// note that this should not be inlined
template <typename T>
int hkbBehaviorGraphData::addVariableQuad( const char* name, hkbVariableInfo::VariableType type, const T& value )
{
	hkbVariableInfo info;
	info.m_type = type;
	info.m_role.m_role = hkbRoleAttribute::ROLE_DEFAULT;
	info.m_role.m_flags = hkbRoleAttribute::FLAG_NONE;
	m_stringData->m_variableNames.pushBack( name );
	m_variableInfos.pushBack( info );

	// put in dummy min and max
	hkbVariableBounds bounds;
	bounds.m_min.set<int>( 0 );
	bounds.m_max.set<int>( 0 );
	m_variableBounds.pushBack( bounds );

	return m_variableInitialValues->addQuad<T>( value );
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
