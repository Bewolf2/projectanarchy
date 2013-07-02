/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//
//	Sets this = i

template <>
inline void hkSimdInt<256>::setFromInt32(int i)
{
	m_lo.setFromInt32(i);
	m_hi.m_quad = vdupq_lane_s32(vget_high_s32(m_lo.m_quad), 1);
}

//
//	Sets this = a + b

template <>
inline void hkSimdInt<256>::setUnsignedAdd(HalfIntParameter a, HalfIntParameter b)
{
	// a = [a3, a2, a1, a0]
	// b = [b3, b2, b1, b0]
	const uint64x2_t a20	= vshrq_n_u64(vshlq_n_u64(vreinterpretq_u64_s32(a.m_quad), 32), 32);	// [0, a2, 0, a0]
	const uint64x2_t b20	= vshrq_n_u64(vshlq_n_u64(vreinterpretq_u64_s32(b.m_quad), 32), 32);	// [0, b2, 0, b0]
	const uint64x2_t a31	= vshrq_n_u64(vreinterpretq_u64_s32(a.m_quad), 32);						// [0, a3, 0, a1]
	const uint64x2_t b31	= vshrq_n_u64(vreinterpretq_u64_s32(b.m_quad), 32);						// [0, b3, 0, b1]
	const uint64x2_t ab20	= vaddq_u64(a20, b20);	// [a2 + b2, a0 + b0]
	const uint64x2_t ab31	= vaddq_u64(a31, b31);	// [a3 + b3, a1 + b1]

	const uint64x1_t r0 = vget_low_u64(ab20);									// [a0 + b0]
	const uint64x1_t r1 = vadd_u64(vshr_n_u64(r0, 32), vget_low_u64(ab31));		// [hi(a0 + b0) + a1 + b1]
	const uint64x1_t r2 = vadd_u64(vshr_n_u64(r1, 32), vget_high_u64(ab20));	// [hi(r1) + a2 + b2]
	const uint64x1_t r3 = vadd_u64(vshr_n_u64(r2, 32), vget_high_u64(ab31));	// [hi(r2) + a3 + b3]

	// Shuffle everything into the output vector
	// We need [r3.1, r3.0, r2.0, r1.0, r0.0]
	const int32x2x2_t r23lh	= vzip_s32(vreinterpret_s32_u64(r2), vreinterpret_s32_u64(r3));			// [r3.0, r2.0], [r3.1, r2.1]
	const int32x2_t r23		= r23lh.val[0];															// [r3.0, r2.0]
	const int32x2_t r01		= vzip_s32(vreinterpret_s32_u64(r0), vreinterpret_s32_u64(r1)).val[0];	// [r1.0, r0.0] 
	m_lo.m_quad = vcombine_s32(r01, r23);															// [r3.0, r2.0, r1.0, r0.0]
	m_hi.m_quad = vreinterpretq_s32_u64(
					vcombine_u64(	vshr_n_u64(vreinterpret_u64_s32(r23lh.val[1]), 32),
									vcreate_u64(0)));												// [0, 0, 0, r3.1]
}

//
//	Sets this = a * b while ignoring any potential overflows

template <>
inline void hkSimdInt<256>::setUnsignedMul(const hkSimdInt<256>& origA, int origB)
{
// a = [a7 a6 a5 a4][a3 a2 a1 a0], b = [*, *, *, b]
	const uint32x4_t a_7654	= vreinterpretq_u32_s32(origA.m_hi.m_quad);			// [a7, a6, a5, a4]
	const uint32x4_t a_3210	= vreinterpretq_u32_s32(origA.m_lo.m_quad);			// [a3, a2, a1, a0]
	const uint32x2_t b		= vreinterpret_u32_s32(vld1_dup_s32(&origB));		// [b, b, b, b]

	// Compute all products
	const uint64x2_t a1b_a0b = vmull_u32(vget_low_u32(a_3210), b);	// [a1 b, a0 b]
	const uint64x2_t a3b_a2b = vmull_u32(vget_high_u32(a_3210), b);	// [a3 b, a2 b]
	const uint64x2_t a5b_a4b = vmull_u32(vget_low_u32(a_7654), b);	// [a5 b, a4 b]
	const uint64x2_t a7b_a6b = vmull_u32(vget_high_u32(a_7654), b);	// [a7 b, a6 b]

	// We need to sum:
	//																   hi(a0 b) lo(a0 b)
	//														  hi(a1 b) lo(a1 b)
	//												 hi(a2 b) lo(a2 b)
	//										hi(a3 b) lo(a3 b)
	//							   hi(a4 b) lo(a4 b)
	//					  hi(a5 b) lo(a5 b)
	//			 hi(a6 b) lo(a6 b)	
	//	hi(a7 b) lo(a7 b)
	const uint64x1_t a0b = vget_low_u64(a1b_a0b),	a1b	= vget_high_u64(a1b_a0b);	// [hi(a0 b), lo(a0 b)], [hi(a1 b), lo(a1 b)]
	const uint64x1_t a2b = vget_low_u64(a3b_a2b),	a3b	= vget_high_u64(a3b_a2b);	// [hi(a2 b), lo(a2 b)], [hi(a3 b), lo(a3 b)]
	const uint64x1_t a4b = vget_low_u64(a5b_a4b),	a5b	= vget_high_u64(a5b_a4b);	// [hi(a4 b), lo(a4 b)], [hi(a5 b), lo(a5 b)]
	const uint64x1_t a6b = vget_low_u64(a7b_a6b),	a7b	= vget_high_u64(a7b_a6b);	// [hi(a6 b), lo(a6 b)], [hi(a7 b), lo(a7 b)]
	
	const uint64x2_t	a31		= vaddq_u64(vcombine_u64(a1b, a3b), vshrq_n_u64(vcombine_u64(a0b, a2b), 32));		// [(a3 b) + hi(a2 b), (a1 b) + hi(a0 b)]
	uint64x2_t			a75		= vaddq_u64(vcombine_u64(a5b, a7b), vshrq_n_u64(vcombine_u64(a4b, a6b), 32));		// [(a7 b) + hi(a6 b), (a5 b) + hi(a4 b)]
	const uint64x2_t	a42		= vcombine_u64(a2b, a4b);															// [a4 b, a2 b]
	const uint64x2_t	a20		= vaddq_u64(vshrq_n_u64(a31, 32), vshrq_n_u64(vshlq_n_u64(a42, 32), 32));			// [hi(a31[1]) + lo(a4 b), hi(a31[0]) + lo(a2 b)]
	uint64x1_t			a67l	= vadd_u64(vshr_n_u64(vget_low_u64(a75), 32), vshr_n_u64(vshl_n_u64(a6b, 32), 32));	// [hi(a75[0]) + lo(a6 b)]

	uint64x2_t	a7531	= vcombine_u64(vget_high_u64(a31), vget_low_u64(a75));								// [a75[0], a31[1]]
				a7531	= vaddq_u64(vshrq_n_u64(a20, 32), vshrq_n_u64(vshlq_n_u64(a7531, 32), 32));			// [lo(a75[0]) + hi(a20[1]), lo(a31[1]) + hi(a20[0])]
	uint64x1_t	a75h	= vadd_u64(vget_high_u64(a75), vshr_n_u64(a67l, 32));								// [a75[1] + hi(a67[0])]
	uint64x2_t	a67		= vcombine_u64(vget_high_u64(a20), a67l);											// [a67[0], a20[1]]
				a67		= vaddq_u64(vshrq_n_u64(a7531, 32), vshrq_n_u64(vshlq_n_u64(a67, 32), 32));			// [hi(a7531[1]) + lo(a67[0]), hi(a7531[0]) + lo(a20[1])]

	// Can compute the lowest 128 bits
	const uint64x2_t retLo = vcombine_u64(
		vreinterpret_u64_u32(vzip_u32(	vreinterpret_u32_u64(a0b), 
										vreinterpret_u32_u64(vget_low_u64(a31))).val[0]),	// [lo(a31[0]), lo(a0 b)]
		vreinterpret_u64_u32(vzip_u32(	vreinterpret_u32_u64(vget_low_u64(a20)),
										vreinterpret_u32_u64(vget_low_u64(a7531))).val[0])	// [lo(a7531[0]), lo(a20[0])]
		);																					// [lo(a7531[0]), lo(a20[0]), lo(a31[0]), lo(a0 b)]

						a75		= vcombine_u64(vshr_n_u64(vshl_n_u64(vget_high_u64(a7531), 32), 32), a75h);	// [a75[1], lo(a7531[1])]
						a75		= vaddq_u64(a75, vshrq_n_u64(a67, 32));										// [a75[1] + hi(a67[1]), a75[0] + hi(a67[0]]
	const uint64x1_t	a20l	= vadd_u64(	vshr_n_u64(vget_low_u64(a75), 32),
											vshr_n_u64(vshl_n_u64(vget_high_u64(a67), 32), 32));			// [hi(a75[0]) + lo(a67[1])]

	a67l = vreinterpret_u64_u32(vzip_u32(	vreinterpret_u32_u64(vget_low_u64(a67)),
											vreinterpret_u32_u64(vget_low_u64(a75))).val[0]);				// [lo(a75[0]), lo(a67[0])]
	a75h = vadd_u64(vget_high_u64(a75), vshr_n_u64(a20l, 32));												// [a75[1] + hi(a20[0])]
	a75h = vreinterpret_u64_u32(vzip_u32(vreinterpret_u32_u64(a20l), vreinterpret_u32_u64(a75h)).val[0]);	// [lo(a75[1]), lo(a20[0])]

	// Can compute the high 128 bits
	const uint64x2_t retHi = vcombine_u64(a67l, a75h);							// [lo(a75[1]), lo(a20[0]), lo(a75[0]), lo(a67[0])]
	m_hi.m_quad = vreinterpretq_s32_u64(retHi);
	m_lo.m_quad = vreinterpretq_s32_u64(retLo);
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
