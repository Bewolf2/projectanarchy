/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Common/Base/Math/Vector/hkIntVector.h>
#ifdef HK_PLATFORM_SPU
#include <Ai/Pathfinding/NavMesh/Spu/hkaiSpuNavMeshAccessor.h>
#endif


inline hkaiNavMeshTraversalUtils::ClearanceValue::ClearanceValue()
{
	m_exactValue = 0;
	m_truncatedValue = 0;
	m_valueHalf.setZero();
	m_exactValueComputed = false;
}

inline void hkaiNavMeshTraversalUtils::ClearanceValue::setFromHalf( const hkHalf& h )
{
	m_exactValue = 0;
	m_exactValueComputed = false;
	m_truncatedValue.setFromHalf( h );
	m_valueHalf = h; 
}


inline void hkaiNavMeshTraversalUtils::ClearanceValue::setFromSimdReal( hkSimdRealParameter s )
{
	m_exactValueComputed = true;
	m_exactValue = s;
	m_valueHalf.setReal<false>( s.getReal() );
	m_truncatedValue.setFromFloat( m_valueHalf.getReal() );
}

inline hkSimdReal hkaiNavMeshTraversalUtils::ClearanceValue::getTruncatedValue() const
{
	return m_truncatedValue;
}

inline hkSimdReal hkaiNavMeshTraversalUtils::ClearanceValue::getExactValue() const
{
	HK_ASSERT(0x263e9120, isExactValueComputed() );
	return m_exactValue;
}

inline hkBool32 hkaiNavMeshTraversalUtils::ClearanceValue::isExactValueComputed() const
{
	return m_exactValueComputed;
}

inline hkSimdReal hkaiNavMeshTraversalUtils::ClearanceValue::getNextTruncatedValue() const
{
#if defined(HK_HALF_IS_FLOAT)
	union
	{
		int i;
		float f;
	} IntFloat;
	IntFloat u;
	u.f = m_valueHalf.getStorage();
	u.i++;
	hkHalf nextHalf; nextHalf.setStorage( u.f );
#else
 	hkInt16 intValue = m_valueHalf.getStorage();
 	intValue++;
 	hkHalf nextHalf; nextHalf.setStorage( intValue );
#endif
	hkSimdReal floatNextValue; floatNextValue.setFromHalf(nextHalf);
 	floatNextValue.setMax(floatNextValue, m_truncatedValue); // just in case we overflowed the hkHalf.
 	return floatNextValue;
}

HK_FORCE_INLINE_ON_CPU hkSimdReal HK_CALL hkaiNavMeshTraversalUtils::getGlobalClearance(
	const hkaiGeneralAccessor& accessor,
	hkaiPackedKey startFaceKey,
	const hkaiNavMesh::Edge& startEdge,
	hkVector4Parameter up,
	hkSimdRealParameter maxGlobalClearance )
{
	accessor.setSectionOf(startFaceKey);

	HK_ASSERT2(0x19bedece, accessor->hasClearanceInfo(), "Clearance information not set up for nav mesh instance. Make sure to call hkaiNavMeshInstance::initClearanceCache and hkaiNavMeshInstance::resetVertexClearances." );

	hkHalf clearanceHalf = accessor->getGlobalClearance(startEdge.m_a);

	hkSimdReal clearanceSimd; clearanceSimd.setFromHalf(clearanceHalf);
	if (clearanceSimd > maxGlobalClearance)
	{
		hkSimdReal computedGlobalClearance = computeGlobalClearance(accessor, startFaceKey, startEdge, up, maxGlobalClearance );
		clearanceHalf.setReal<true>( computedGlobalClearance.getReal() );
		clearanceSimd.setFromHalf(clearanceHalf);
	}

	return clearanceSimd;
}

HK_FORCE_INLINE_ON_CPU hkSimdReal HK_CALL hkaiNavMeshTraversalUtils::getGlobalClearanceB(
	const hkaiGeneralAccessor& accessor,
	hkaiPackedKey faceKey,
	const hkaiNavMesh::Face& face,
	hkaiPackedKey edgeKey,
	hkVector4Parameter up,
	hkSimdRealParameter maxGlobalClearance )
{
	int index = hkaiGetIndexFromPacked(edgeKey) + 1;
	if (index >= face.m_startEdgeIndex + face.m_numEdges)
	{
		index = face.m_startEdgeIndex;
	}
	accessor.setSectionOf(edgeKey);
	hkaiNavMesh::EdgeReference edge = accessor->getEdge(index);
	return getGlobalClearance(accessor, faceKey,edge, up,maxGlobalClearance);
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
