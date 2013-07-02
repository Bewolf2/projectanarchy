/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE hkbVariableBindingSet::hkbVariableBindingSet()
:	m_indexOfBindingToEnable(-1),
	m_hasOutputBinding(false),
	m_initializedOffsets(false)
{
}

HK_FORCE_INLINE hkbVariableBindingSet::Binding::Binding()
:	m_memberClass(HK_NULL),
	m_offsetInObjectPlusOne(0),
	m_offsetInArrayPlusOne(0),
	m_rootVariableIndex(-1),
	m_variableIndex(0),
	m_bitIndex(-1),
	m_bindingType(BINDING_TYPE_VARIABLE),
	m_memberType(hkClassMember::TYPE_VOID),
	m_variableType(-1),
	m_flags(FLAG_NONE)
{
}

HK_FORCE_INLINE hkbVariableBindingSet::Binding::Binding( const char* memberPath, int variableIndex, BindingType bindingType, hkInt8 bitIndex )
:	m_memberPath(memberPath),
	m_memberClass(HK_NULL),
	m_offsetInObjectPlusOne(0),
	m_offsetInArrayPlusOne(0),
	m_rootVariableIndex(-1),
	m_variableIndex(variableIndex),
	m_bitIndex(bitIndex),
	m_bindingType(bindingType),
	m_memberType(hkClassMember::TYPE_VOID),
	m_variableType(-1),
	m_flags(FLAG_NONE)
{
}

HK_FORCE_INLINE void hkbVariableBindingSet::removeBinding( int index )
{
	HK_ASSERT2( 0x7681b3cb, ( m_memSizeAndFlags & hkReferencedObject::MASK_MEMSIZE ) != 0, "don't call removeBinding if I came from a packfile" );

	// the order may change
	m_bindings.removeAt( index );

	// update m_indexOfBindingToEnable
	if ( index == m_indexOfBindingToEnable )
	{
		m_indexOfBindingToEnable = -1;
	}
	else if ( m_indexOfBindingToEnable == m_bindings.getSize() )
	{
		// The element at the end is used to fill the hole when you call removeAt().
		m_indexOfBindingToEnable = index;
	}
}

HK_FORCE_INLINE int hkbVariableBindingSet::numBindings()
{
	return m_bindings.getSize();
}

HK_FORCE_INLINE const hkbVariableBindingSet::Binding& hkbVariableBindingSet::getBinding( int index )
{
	return m_bindings[index];
}

HK_FORCE_INLINE int hkbVariableBindingSet::getIndexOfBindingToEnable() 
{
	return m_indexOfBindingToEnable;
}

HK_FORCE_INLINE hkBool32 hkbVariableBindingSet::hasOutputBinding()
{
	return m_hasOutputBinding;
}

HK_FORCE_INLINE void hkbVariableBindingSet::initMemberOffsets( hkbBindable* bindable )
{
	
	
	if ( !m_initializedOffsets )
	{
		const int count = m_bindings.getSize();

		for( int i = 0; i < count; i++ )
		{
			initMemberOffsetsInternal( m_bindings[i], bindable );
		}

		m_initializedOffsets = true;
	}
}

HK_FORCE_INLINE hkbVariableBindingSet::hkbVariableBindingSet( hkFinishLoadedObjectFlag flag ) 
:	hkReferencedObject(flag),
	m_bindings(flag)
{
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
