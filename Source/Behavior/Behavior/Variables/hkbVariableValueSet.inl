/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

template <typename T>
HK_FORCE_INLINE T hkbVariableValueSet::getWord( int variableIndex ) const
{
	HK_ASSERT( 0xc791dd74, sizeof(T) <= 4 );

	const hkbVariableValue& varValue = m_wordVariableValues[variableIndex];

	return varValue.get<T>();
}

template <typename T>
HK_FORCE_INLINE const T& hkbVariableValueSet::getQuad( int variableIndex ) const
{
	HK_ASSERT( 0x0261c539, sizeof(T) > 4 );

	const hkbVariableValue& varValue = m_wordVariableValues[variableIndex];

	int quadIndex = varValue.get<int>();

	HK_ASSERT2(	0x456125a6, quadIndex >= 0 && quadIndex < m_quadVariableValues.getSize(), 
		"The word value of a quad variable must be an index (int) into m_quadVariableValues." );

	return *reinterpret_cast<const T*>( m_quadVariableValues.begin() + quadIndex );
}

HK_FORCE_INLINE hkReferencedObject* hkbVariableValueSet::getObject( int variableIndex ) const
{
	const hkbVariableValue& varValue = m_wordVariableValues[variableIndex];

	int variantIndex = varValue.get<int>();

	HK_ASSERT2(	0x456125a6, variantIndex >= 0 && variantIndex < m_variantVariableValues.getSize(), 
		"The word value of a variant variable must be an index (int) into m_variantVariableValues." );

	return m_variantVariableValues[variantIndex].val();
}

template <typename T>
HK_FORCE_INLINE void hkbVariableValueSet::setWord( int variableIndex, T value )
{
	HK_ASSERT( 0x561298a7, sizeof(T) <= 4 );

	hkbVariableValue& varValue = m_wordVariableValues[variableIndex];

	varValue.set<T>( value );
}

template <typename T>
HK_FORCE_INLINE void hkbVariableValueSet::setQuad( int variableIndex, const T& value )
{
	HK_ASSERT( 0x9812bc65, sizeof(T) > 4 );

	const hkbVariableValue& varValue = m_wordVariableValues[variableIndex];

	int quadIndex = varValue.get<int>();

	HK_ASSERT2(	0x456125a6, quadIndex >= 0 && quadIndex < m_quadVariableValues.getSize(), 
		"The word value of a quad variable must be an index (int) into m_quadVariableValues." );

	hkVector4& valueVector4 = m_quadVariableValues[quadIndex];
	T& valueT = reinterpret_cast<T&>( valueVector4 );
	valueT = value;
}

HK_FORCE_INLINE hkbVariableValue hkbVariableValueSet::getVariableValue( int variableIndex ) const
{
	return m_wordVariableValues[variableIndex];
}


HK_FORCE_INLINE hkbVariableValue& hkbVariableValueSet::accessWord( int variableIndex )
{
	return m_wordVariableValues[variableIndex];
}

HK_FORCE_INLINE hkRefVariant& hkbVariableValueSet::accessObject( int variableIndex )
{
	const hkbVariableValue& varValue = m_wordVariableValues[variableIndex];

	int variantIndex = varValue.get<int>();

	HK_ASSERT2(	0x65612a45, variantIndex >= 0 && variantIndex < m_variantVariableValues.getSize(), 
		"The word value of a variant variable must be an index (int) into m_variantVariableValues." );

	return m_variantVariableValues[variantIndex];
}

template <typename T>
HK_FORCE_INLINE int hkbVariableValueSet::addWord( T value )
{
	hkbVariableValue v;
	v.set<T>( value );
	int index = m_wordVariableValues.getSize();
	m_wordVariableValues.pushBack( v );
	return index;
}

template <typename T>
HK_FORCE_INLINE int hkbVariableValueSet::addQuad( const T& value )
{
	// set the word initial value to contain an index into the quads
	int quadIndex = m_quadVariableValues.getSize();
	hkbVariableValue v;
	v.set<int>( quadIndex );
	int index = m_wordVariableValues.getSize();
	m_wordVariableValues.pushBack( v );

	// set the quad initial value
	m_quadVariableValues.setSize( quadIndex + 1 );
	hkVector4& valueVector4 = m_quadVariableValues[quadIndex];
	T& valueT = reinterpret_cast<T&>( valueVector4 );
	valueT = value;

	return index;
}

HK_FORCE_INLINE void hkbVariableValueSet::removeLastWord()
{
	m_wordVariableValues.removeAt( m_wordVariableValues.getSize() - 1 );
}

HK_FORCE_INLINE void hkbVariableValueSet::removeLastQuad()
{
	int variableIndex = m_wordVariableValues.getSize() - 1;
	int quadIndex = m_wordVariableValues[variableIndex].get<int>();
	m_quadVariableValues.removeAt( quadIndex );
	m_wordVariableValues.removeAt( variableIndex );
}

HK_FORCE_INLINE void hkbVariableValueSet::removeLastObject()
{
	int variableIndex = m_wordVariableValues.getSize() - 1;
	int variantIndex = m_wordVariableValues[variableIndex].get<int>();
	m_variantVariableValues.removeAt( variantIndex );
	m_wordVariableValues.removeAt( variableIndex );
}

HK_FORCE_INLINE int hkbVariableValueSet::getNumValues() const
{
	return m_wordVariableValues.getSize();
}

HK_FORCE_INLINE void hkbVariableValueSet::clear()
{
	m_wordVariableValues.setSize( 0 );
	m_quadVariableValues.setSize( 0 );
	m_variantVariableValues.setSize( 0 );
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
