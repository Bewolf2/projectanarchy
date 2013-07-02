/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//
//	Returns this < 0

inline const hkVector4fComparison hkInt128Vector4::lessZero() const
{
	const int32x2_t sxy = vzip_s32(vget_high_s32(m_x.m_quad), vget_high_s32(m_y.m_quad)).val[1];	// [sy, sx]
	const int32x2_t szw = vzip_s32(vget_high_s32(m_z.m_quad), vget_high_s32(m_w.m_quad)).val[1];	// [sw, sz]
	const uint32x4_t s	= vreinterpretq_u32_s32(vshrq_n_s32(vcombine_s32(sxy, szw), 31));			// [sw, sz, sy, sx]

	hkVector4fComparison cmp;	cmp.m_mask = s;	return cmp;
}

//
//	Flips the sign of v

template <> inline void hkInt128Vector4::setFlipSign<3>(hkInt128Vector4Parameter v, hkVector4fComparisonParameter signFlip)
{
	const uint32x4_t m			= signFlip.m_mask;
	hkVector4fComparison cmpx;	cmpx.m_mask = vdupq_lane_u32(vget_low_u32(m), 0);	m_x.setFlipSign(v.m_x, cmpx);
	hkVector4fComparison cmpy;	cmpy.m_mask = vdupq_lane_u32(vget_low_u32(m), 1);	m_y.setFlipSign(v.m_y, cmpy);
	hkVector4fComparison cmpz;	cmpz.m_mask = vdupq_lane_u32(vget_high_u32(m), 0);	m_z.setFlipSign(v.m_z, cmpz);
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
