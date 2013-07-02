/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE hkbSymbolLinker::hkbSymbolLinker()
{
}

HK_FORCE_INLINE int hkbSymbolLinker::addSymbol( const char* name )
{
	HK_WARN_ON_DEBUG_IF( m_nameToIdMap.getWithDefault( name, -1 ) != -1, 0x2f44bea4, "Adding duplicate symbols to the map." );

	int id = m_idToNameMap.getSize();
	m_idToNameMap.pushBack( name );

	// it's important to use m_idToNameMap[id] and not name in the map because the string was copied
	m_nameToIdMap.insert( m_idToNameMap[id], id );
	return id;
}

HK_FORCE_INLINE void hkbSymbolLinker::clear()
{
	m_idToNameMap.clear();
	m_nameToIdMap.clear();
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
