/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE void hkbCompiledExpressionSet::clear()
{
	m_rpn.clear();
	m_expressionToRpnIndex.clear();
	m_numExpressions = 0;
}

HK_FORCE_INLINE int hkbCompiledExpressionSet::getNumExpressions() const 
{
	// m_expressionToRpnIndex has been added in 6.5.0 release version. Thus when updating this
	// object from 6.5.0 beta to 6.5.0 release m_expressionToRpnIndex would be of zero size.
	// In order to initialize it properly we make sure that the expression is evaluated at least
	// once after updating the versioning.
	return ( m_expressionToRpnIndex.getSize() > 0 ) ? m_numExpressions : 0;
}


HK_FORCE_INLINE void hkbCompiledExpressionSet::skipLeadingWhiteSpace( const hkStringBuf& expression, int& currIndex )
{
	while ( currIndex < expression.getLength() && expression[currIndex] == ' ' )
	{
		currIndex++;
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
