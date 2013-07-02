/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

inline hkaiSilhouetteReferenceFrame::hkaiSilhouetteReferenceFrame()
{
	m_up.setZero();
	m_referenceAxis.setZero();
}


inline void hkaiSilhouetteReferenceFrame::project(hkVector4& vInOut) const
{
	hkVector4 v;
	hkSimdReal v0 = getFirstComponent(vInOut);
	hkSimdReal v1 = getSecondComponent(vInOut);
	v.set(v0, v1, hkSimdReal_0, hkSimdReal_0);
	vInOut = v;
}

inline void hkaiSilhouetteReferenceFrame::unproject(hkVector4& vInOut, hkSimdRealParameter missingComponent) const
{
	hkVector4 v;
	v.setAll(missingComponent);
	hkSimdReal v0 = vInOut.getComponent<0>();
	hkSimdReal v1 = vInOut.getComponent<1>();
	setUnprojected(v0, v1, v);
	v.setW(hkSimdReal_0);	// For tidyness
	vInOut = v;
}

inline void hkaiSilhouetteReferenceFrame::projectAabb(hkAabb& aabbInOut) const
{
	project(aabbInOut.m_min);
	project(aabbInOut.m_max);
	aabbInOut.m_min(2) = 0.0f;
	aabbInOut.m_max(2) = 0.0f;
	if( aabbInOut.m_min(0) > aabbInOut.m_max(0))	{ hkReal temp = aabbInOut.m_min(0); aabbInOut.m_min(0) = aabbInOut.m_max(0); aabbInOut.m_max(0) = temp;}
	if( aabbInOut.m_min(1) > aabbInOut.m_max(1))	{ hkReal temp = aabbInOut.m_min(1); aabbInOut.m_min(1) = aabbInOut.m_max(1); aabbInOut.m_max(1) = temp;}
}


const hkVector4& hkaiSilhouetteReferenceFrame::getUp() const
{
	return m_up;
}

const hkVector4& hkaiSilhouetteReferenceFrame::getReferenceAxis() const
{
	return m_referenceAxis;
}

const hkVector4& hkaiSilhouetteReferenceFrame::getOrthogonalAxis() const
{
	return m_orthogonalAxis;
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
