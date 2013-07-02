/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//
//	Sets all component of this to i

inline void hkInt64Vector4::setAll(hkInt64 i)
{
	m_quad.xy = m_quad.zw = vdupq_n_s64(i);
}

//
//	Sets this = 0

inline void hkInt64Vector4::setZero()
{
	const int64x1_t z = vcreate_s64(0);
	m_quad.xy = vcombine_s64(z, z);
	m_quad.zw = m_quad.xy;
}

//
//	Sets the components of the vector to the given values

inline void hkInt64Vector4::set(hkInt64 x, hkInt64 y, hkInt64 z, hkInt64 w)
{
	m_quad.xy = vcombine_s64(vcreate_s64((hkUint64)x), vcreate_s64((hkUint64)y));
	m_quad.zw = vcombine_s64(vcreate_s64((hkUint64)z), vcreate_s64((hkUint64)w));
}

//
//	Sets the I-th component to the given value

template<> inline void hkInt64Vector4::setComponent<0>(hkInt64 val)	{	m_quad.xy = vsetq_lane_s64(val, m_quad.xy, 0);	}
template<> inline void hkInt64Vector4::setComponent<1>(hkInt64 val)	{	m_quad.xy = vsetq_lane_s64(val, m_quad.xy, 1);	}
template<> inline void hkInt64Vector4::setComponent<2>(hkInt64 val)	{	m_quad.zw = vsetq_lane_s64(val, m_quad.zw, 0);	}
template<> inline void hkInt64Vector4::setComponent<3>(hkInt64 val)	{	m_quad.zw = vsetq_lane_s64(val, m_quad.zw, 1);	}

//
//	Sets this = vA - vB

inline void hkInt64Vector4::setSub(hkInt64Vector4Parameter vA, hkInt64Vector4Parameter vB)
{
	m_quad.xy = vsubq_s64(vA.m_quad.xy, vB.m_quad.xy);
	m_quad.zw = vsubq_s64(vA.m_quad.zw, vB.m_quad.zw);
}

//
//	Returns (this < 0)

template <> inline const hkVector4fComparison hkInt64Vector4::lessZero<hkVector4fComparison>() const
{
	// Shift the sign bits
	const int32x4_t xxyy	= vreinterpretq_s32_s64(vshrq_n_s64(m_quad.xy, 63));			// [x, x, y, y]
	const int32x4_t zzww	= vreinterpretq_s32_s64(vshrq_n_s64(m_quad.zw, 63));			// [z, z, w, w]
	const int32x2_t	xy		= vzip_s32(vget_low_s32(xxyy), vget_high_s32(xxyy)).val[0];		// [x, y]
	const int32x2_t zw		= vzip_s32(vget_low_s32(zzww), vget_high_s32(zzww)).val[0];		// [z, w]

	hkVector4fComparison cmp;
	cmp.m_mask = vreinterpretq_u32_s32(vcombine_s32(xy, zw));
	return cmp;
}

//
//	Returns (this < 0)

template <> inline const hkVector4dComparison hkInt64Vector4::lessZero<hkVector4dComparison>() const
{
	// Shift the sign bits
	hkVector4dComparison cmp;
	cmp.set(lessZero().getMask());
	return cmp;
}

inline const hkVector4fComparison hkInt64Vector4::lessZero() const
{
	return lessZero<hkVector4fComparison>();
}

//
//	Returns (this == 0)

inline const hkVector4fComparison hkInt64Vector4::equalZero() const
{
	const uint32x4_t xy		= vreinterpretq_u32_s64(m_quad.xy);					// [xLo, xHi, yLo, yHi]
	const uint32x4_t zw		= vreinterpretq_u32_s64(m_quad.zw);					// [zLo, zHi, wLo, wHi]
	const uint32x2_t cmpXY	= vpmax_u32(vget_low_u32(xy), vget_high_u32(xy));	// [max(xLo, xHi), max(yLo, yHi)]
	const uint32x2_t cmpZW	= vpmax_u32(vget_low_u32(zw), vget_high_u32(zw));	// [max(zLo, zHi), max(wLo, wHi)]

	hkVector4fComparison cmp;
	cmp.m_mask = vceqq_u32(vcombine_u32(cmpXY, cmpZW), vdupq_n_u32(0));
	return cmp;
}

//
//	Returns (this == v)

inline const hkVector4fComparison hkInt64Vector4::equal(hkInt64Vector4Parameter v) const
{
	const uint32x4_t cmpXXYY	= vceqq_u32(vreinterpretq_u32_s64(m_quad.xy), vreinterpretq_u32_s64(v.m_quad.xy));	// [xLo, xHi, yLo, yHi]
	const uint32x4_t cmpZZWW	= vceqq_u32(vreinterpretq_u32_s64(m_quad.zw), vreinterpretq_u32_s64(v.m_quad.zw));	// [zLo, zHi, wLo, wHi]
	const uint32x2_t cmpXY		= vpmin_u32(vget_low_u32(cmpXXYY), vget_high_u32(cmpXXYY));							// [xLo && xHi, yLo && yHi]
	const uint32x2_t cmpZW		= vpmin_u32(vget_low_u32(cmpZZWW), vget_high_u32(cmpZZWW));							// [zLo && zHi, wLo && wHi]

	hkVector4fComparison cmp;
	cmp.m_mask = vcombine_u32(cmpXY, cmpZW);
	return cmp;
}

//
//	Sets this = -v

template <> inline void hkInt64Vector4::setNeg<4>(hkInt64Vector4Parameter v)
{
	const int64x2_t zero	= vdupq_n_s64(0);
	m_quad.xy				= vsubq_s64(zero, v.m_quad.xy);
	m_quad.zw				= vsubq_s64(zero, v.m_quad.zw);
}

//
//	Sets this = vA & vB

inline void hkInt64Vector4::setAnd(hkInt64Vector4Parameter vA, hkInt64Vector4Parameter vB)
{
	m_quad.xy = vandq_s64(vA.m_quad.xy, vB.m_quad.xy);
	m_quad.zw = vandq_s64(vA.m_quad.zw, vB.m_quad.zw);
}

//
//	Sets this = cmp ? vTrue : vFalse

inline void hkInt64Vector4::setSelect(hkVector4fComparisonParameter cmp, hkInt64Vector4Parameter vTrue, hkInt64Vector4Parameter vFalse)
{
	const uint32x2_t xy		= vget_low_u32(cmp.m_mask);									// [cx, cy]
	const uint32x2_t zw		= vget_high_u32(cmp.m_mask);								// [cz, cw]
	const uint32x4_t xxyy	= vcombine_u32(vdup_lane_u32(xy, 0), vdup_lane_u32(xy, 1));	// [cx, cx, cy, cy]
	const uint32x4_t zzww	= vcombine_u32(vdup_lane_u32(zw, 0), vdup_lane_u32(zw, 1));	// [cz, cz, cw, cw]

	m_quad.xy = vbslq_s64(vreinterpretq_u64_u32(xxyy), vTrue.m_quad.xy, vFalse.m_quad.xy);
	m_quad.zw = vbslq_s64(vreinterpretq_u64_u32(zzww), vTrue.m_quad.zw, vFalse.m_quad.zw);
}

//
//	Sets this = v << shr by component

template <int I>
inline void hkInt64Vector4::setShiftLeft(hkInt64Vector4Parameter v)
{
	m_quad.xy = vshlq_n_s64(v.m_quad.xy, I);
	m_quad.zw = vshlq_n_s64(v.m_quad.zw, I);
}

inline void hkInt64Vector4::setShiftLeft(hkInt64Vector4Parameter v, int I)
{
	const int64x2_t shl	= vdupq_n_s64(I);
	m_quad.xy			= vshlq_s64(v.m_quad.xy, shl);
	m_quad.zw			= vshlq_s64(v.m_quad.zw, shl);
}

namespace hkInt64Vector4Impl
{
	template <int I, hkMathValueType VAL> struct Shr {};

	template <> struct Shr<0, HK_VALUE_UNSIGNED>	{	static inline void HK_CALL shr(hkInt64Vector4Parameter vIn, hkInt64Vector4& vOut)	{	vOut = vIn;	}	};
	template <> struct Shr<0, HK_VALUE_SIGNED>		{	static inline void HK_CALL shr(hkInt64Vector4Parameter vIn, hkInt64Vector4& vOut)	{	vOut = vIn;	}	};

	template <int I> struct Shr<I, HK_VALUE_UNSIGNED>
	{
		static inline void HK_CALL shr(hkInt64Vector4Parameter vIn, hkInt64Vector4& vOut)
		{
			vOut.m_quad.xy = vreinterpretq_s64_u64(vshrq_n_u64(vreinterpretq_u64_s64(vIn.m_quad.xy), I));
			vOut.m_quad.zw = vreinterpretq_s64_u64(vshrq_n_u64(vreinterpretq_u64_s64(vIn.m_quad.zw), I));
		}
	};

	template <int I> struct Shr<I, HK_VALUE_SIGNED>
	{
		static inline void HK_CALL shr(hkInt64Vector4Parameter vIn, hkInt64Vector4& vOut)
		{
			vOut.m_quad.xy = vshrq_n_s64(vIn.m_quad.xy, I);
			vOut.m_quad.zw = vshrq_n_s64(vIn.m_quad.zw, I);
		}
	};
}

//
//	Sets this = v >> I by component

template <int I, hkMathValueType VAL>
inline void hkInt64Vector4::setShiftRight(hkInt64Vector4Parameter v)
{
	hkInt64Vector4Impl::Shr<I, VAL>::shr(v, *this);
}

template <int I>
inline void hkInt64Vector4::setShiftRight(hkInt64Vector4Parameter v)
{
	setShiftRight<I, HK_VALUE_UNSIGNED>(v);
}

template <> inline void hkInt64Vector4::setShiftRight<HK_VALUE_UNSIGNED>(hkInt64Vector4Parameter v, int I)
{
	const int64x2_t shr	= vdupq_n_s64(-I);
	m_quad.xy			= vreinterpretq_s64_u64(vshlq_u64(vreinterpretq_u64_s64(v.m_quad.xy), shr));
	m_quad.zw			= vreinterpretq_s64_u64(vshlq_u64(vreinterpretq_u64_s64(v.m_quad.zw), shr));
}

template <> inline void hkInt64Vector4::setShiftRight<HK_VALUE_SIGNED>(hkInt64Vector4Parameter v, int I)
{
	const int64x2_t shr	= vdupq_n_s64(-I);
	m_quad.xy			= vshlq_s64(v.m_quad.xy, shr);
	m_quad.zw			= vshlq_s64(v.m_quad.zw, shr);
}

namespace hkInt64Vector4Impl
{
	// Multiples vector vA = [hiB, loB, hiA, loA] with vector vX = [*, y *, x]. The * is considered to be the lo duplicated component, i.e. [y, y, x, x]
	static inline void HK_CALL vec2x64_mul_32(const int64x2_t& x, hkIntVectorParameter y, hkSimdInt<128>& axOut, hkSimdInt<128>& byOut)
	{
		// Let x = [a1, a0, c1, c0], y = [b0, b0, d0, d0] = [Hi, Lo]
		// We must compute [(a1 * b0) << 32 + a0 * b0, ...]
		const uint32x2_t a1a0	= vreinterpret_u32_s64(vget_high_s64(x));	// [a1 a0]
		const uint32x2_t b0b0	= vget_high_u32(y.m_quad);					// [b0 b0]
		const uint32x2_t c1c0	= vreinterpret_u32_s64(vget_low_s64(x));	// [c1 c0]
		const uint32x2_t d0d0	= vget_low_u32(y.m_quad);					// [d0 d0]

		// Multiply
		const uint64x2_t a1b0_a0b0	= vmull_u32(a1a0, b0b0);				// [a1 b0, a0 b0]
		const uint64x2_t c1d0_c0d0	= vmull_u32(c1c0, d0d0);				// [c1 d0, c0 d0]

		// Shuffle into proper places
		uint64x2_t a0b0_c0d0 = vcombine_u64(vget_low_u64(c1d0_c0d0), vget_low_u64(a1b0_a0b0));		// [a0 b0, c0 d0]
		uint64x2_t a1b0_c1d0 = vcombine_u64(vget_high_u64(c1d0_c0d0), vget_high_u64(a1b0_a0b0));	// [a1 b0, c1 d0]

		// Accumulate results into 3 registers:
		//	r0 = [0..63], r1 = [32..95], r2 = [64..127]
		uint64x2_t	r2			= vshrq_n_u64(a1b0_c1d0, 32);						// r2 = (a1 * b0) >> 32
					a1b0_c1d0	= vshrq_n_u64(vshlq_n_u64(a1b0_c1d0, 32), 32);
		uint64x2_t	r1			= vaddq_u64(a1b0_c1d0, vshrq_n_u64(a0b0_c0d0, 32));	// r1 = lo(a1 * b0) + ((a0 * b0) >> 32)
					r2			= vaddq_u64(r2, vshrq_n_u64(r1, 32));				// r2 += r1 >> 32 = [hi(a b), hi(c d)]

		// Finally, gather the results into the outputs
					a0b0_c0d0	= vshrq_n_u64(vshlq_n_u64(a0b0_c0d0, 32), 32);
		uint64x2_t	r0			= vorrq_u64(vshlq_n_u64(r1, 32), a0b0_c0d0);			// [lo(a b), lo(c d)]

		byOut.m_quad = vreinterpretq_s32_u64(vcombine_u64(vget_high_u64(r0), vget_high_u64(r2)));	// a * b
		axOut.m_quad = vreinterpretq_s32_u64(vcombine_u64(vget_low_u64(r0), vget_low_u64(r2)));		// c * d
	}

	static inline int64x2_t HK_CALL vec2x64_mul_32_ret64(const int64x2_t& x, hkIntVectorParameter y)
	{
		// Let x = [a1, a0, c1, c0], y = [b0, b0, d0, d0] = [Hi, Lo]
		// We must compute [(a1 * b0) << 32 + a0 * b0, ...]
		const uint32x2_t a1a0	= vreinterpret_u32_s64(vget_high_s64(x));	// [a1 a0]
		const uint32x2_t b0b0	= vget_high_u32(y.m_quad);					// [b0 b0]
		const uint32x2_t c1c0	= vreinterpret_u32_s64(vget_low_s64(x));	// [c1 c0]
		const uint32x2_t d0d0	= vget_low_u32(y.m_quad);					// [d0 d0]

		// Multiply
		const uint64x2_t a1b0_a0b0	= vmull_u32(a1a0, b0b0);				// [a1 b0, a0 b0]
		const uint64x2_t c1d0_c0d0	= vmull_u32(c1c0, d0d0);				// [c1 d0, c0 d0]

		// Shuffle into proper places
		uint64x2_t a0b0_c0d0 = vcombine_u64(vget_low_u64(c1d0_c0d0), vget_low_u64(a1b0_a0b0));		// [a0 b0, c0 d0]
		uint64x2_t a1b0_c1d0 = vcombine_u64(vget_high_u64(c1d0_c0d0), vget_high_u64(a1b0_a0b0));	// [a1 b0, c1 d0]

		// Accumulate results into 3 registers:
		//	r0 = [0..63], r1 = [32..95], r2 = [64..127]
		uint64x2_t	r1	= vshrq_n_u64(vshlq_n_u64(a1b0_c1d0, 32), 32);
					r1	= vaddq_u64(r1, vshrq_n_u64(a0b0_c0d0, 32));	// r1 = lo(a1 * b0) + ((a0 * b0) >> 32)

		// Finally, gather the results into the outputs
		uint64x2_t	r0	= vorrq_u64(vshlq_n_u64(r1, 32), vshrq_n_u64(vshlq_n_u64(a0b0_c0d0, 32), 32));			// [lo(a b), lo(c d)]
		return vreinterpretq_s64_u64(r0);
	}

	// Returns (a > b) ? a : b
	static inline const int64x2_t HK_CALL vec2x64_max(const int64x2_t& a, const int64x2_t& b)
	{
		const int64x2_t signs_a		= vshrq_n_s64(a, 63);
		const int64x2_t signs_b		= vshrq_n_s64(b, 63);

		// Result for different signs
		const uint64x2_t signs_cmp	= vreinterpretq_u64_s64(veorq_s64(signs_a, signs_b));
		const int64x2_t res2		= vbslq_s64(vreinterpretq_u64_s64(signs_a), b, a);								// (a < 0) ? b : a;

		// Result for same signs
		const int64x2_t amb		= vsubq_s64(a, b);
		const uint64x2_t cmp	= vreinterpretq_u64_s64(vshrq_n_s64(amb, 63));
		const int64x2_t res1	= vbslq_s64(cmp, b, a);							// (a < b) ? b : a
		
		// Choose result
		return  vbslq_s64(signs_cmp, res2, res1);	// (a * b < 0) ? res2 : res1;
	}
}

//
//	Returns the dot product between this and v

template <> inline const hkSimdInt<128> hkInt64Vector4::dot<3>(hkIntVectorParameter vB) const
{
	// Compute final signs
	hkVector4fComparison cmpFlip;
	{
		const hkVector4fComparison negA	= lessZero();								// [ax < 0, ay < 0, az < 0, aw < 0]
		const hkVector4fComparison negB	= vB.lessZeroS32();							// [bx < 0, by < 0, bz < 0, bw < 0]
		hkVector4fComparison negZeroA;	negZeroA.setOr(negA, equalZero());			// [ax <= 0, ay <= 0, az <= 0, aw <= 0]
		hkVector4fComparison negZeroB;	negZeroB.setOr(negB, vB.equalZeroS32());		// [bx <= 0, by <= 0, bz <= 0, bw <= 0]

		hkVector4fComparison negA_posB;	negA_posB.setAndNot(negA, negZeroB);		// [(ax < 0) && (bx > 0), ...]
		hkVector4fComparison posA_negB;	posA_negB.setAndNot(negB, negZeroA);		// [(ax > 0) && (bx < 0), ...]
		cmpFlip.setOr(negA_posB, posA_negB);		// [sign(ax * bx), ...]
	}

	// Compute absolute values
	hkInt64Vector4 absA;	absA.setAbs(*this);
	hkIntVector absB;		absB.setAbsS32(vB);
	hkIntVector vXY;		vXY.setPermutation<hkVectorPermutation::XXYY>(absB);	// [vx, vx, vy, vy]
	hkIntVector vZW;		vZW.setPermutation<hkVectorPermutation::ZZWW>(absB);	// [vz, vz, vw, vw]

	// Compute absolute products
	hkInt128Vector4 dots;
	hkInt64Vector4Impl::vec2x64_mul_32(absA.m_quad.xy, vXY, dots.m_x, dots.m_y);
	hkInt64Vector4Impl::vec2x64_mul_32(absA.m_quad.zw, vZW, dots.m_z, dots.m_w);

	// Set signs
	dots.setFlipSign<3>(dots, cmpFlip);

	// Compute and return dot
	dots.m_x.setAdd(dots.m_x, dots.m_y);
	dots.m_x.setAdd(dots.m_x, dots.m_z);
	return dots.m_x;
}

//
//	Returns the dot product between this and v

template <> inline hkInt64 hkInt64Vector4::dot_64<3>(hkIntVectorParameter vB) const
{
	// Compute final signs
	hkVector4fComparison cmpFlip;
	{
		const hkVector4fComparison negA	= lessZero();								// [ax < 0, ay < 0, az < 0, aw < 0]
		const hkVector4fComparison negB	= vB.lessZeroS32();							// [bx < 0, by < 0, bz < 0, bw < 0]
		hkVector4fComparison negZeroA;	negZeroA.setOr(negA, equalZero());			// [ax <= 0, ay <= 0, az <= 0, aw <= 0]
		hkVector4fComparison negZeroB;	negZeroB.setOr(negB, vB.equalZeroS32());		// [bx <= 0, by <= 0, bz <= 0, bw <= 0]

		hkVector4fComparison negA_posB;	negA_posB.setAndNot(negA, negZeroB);		// [(ax < 0) && (bx > 0), ...]
		hkVector4fComparison posA_negB;	posA_negB.setAndNot(negB, negZeroA);		// [(ax > 0) && (bx < 0), ...]
		cmpFlip.setOr(negA_posB, posA_negB);		// [sign(ax * bx), ...]
	}

	// Compute absolute values
	hkInt64Vector4 absA;	absA.setAbs(*this);
	hkIntVector absB;		absB.setAbsS32(vB);
	hkIntVector vXY;		vXY.setPermutation<hkVectorPermutation::XXYY>(absB);	// [vx, vx, vy, vy]
	hkIntVector vZW;		vZW.setPermutation<hkVectorPermutation::ZZWW>(absB);	// [vz, vz, vw, vw]

	// Compute products
	hkInt64Vector4 ret;
	ret.m_quad.xy = hkInt64Vector4Impl::vec2x64_mul_32_ret64(absA.m_quad.xy, vXY);
	ret.m_quad.zw = hkInt64Vector4Impl::vec2x64_mul_32_ret64(absA.m_quad.zw, vZW);
	ret.setFlipSign(ret, cmpFlip);

	// Compute and return dot
	return vget_lane_s64(	vadd_s64(	vget_high_s64(ret.m_quad.xy),							// y + 
										vget_low_s64(vaddq_s64(ret.m_quad.xy, ret.m_quad.zw))),	// [x + z]
							0);
}

//
//	Sets this = a permutation of the components of v

template <> inline void hkInt64Vector4::setPermutation<hkVectorPermutation::YZXW>(hkInt64Vector4Parameter v)
{
	const int64x2_t yz	= vcombine_s64(vget_high_s64(v.m_quad.xy), vget_low_s64(v.m_quad.zw));	// [y, z]
	m_quad.zw			= vcombine_s64(vget_low_s64(v.m_quad.xy), vget_high_s64(v.m_quad.zw));	// [x, w]
	m_quad.xy			= yz;
}

template <> inline void hkInt64Vector4::setPermutation<hkVectorPermutation::ZXYW>(hkInt64Vector4Parameter v)
{
	const int64x2_t zx = vcombine_s64(vget_low_s64(v.m_quad.zw), vget_low_s64(v.m_quad.xy));// [z, x]
	m_quad.zw		= vcombine_s64(vget_high_s64(v.m_quad.xy), vget_high_s64(v.m_quad.zw));	// [y, w]
	m_quad.xy		= zx;
}

template <> inline void hkInt64Vector4::setPermutation<hkVectorPermutation::YXWZ>(hkInt64Vector4Parameter v)
{
	m_quad.xy = vextq_s64(v.m_quad.xy, v.m_quad.xy, 1);	// [y, x]
	m_quad.zw = vextq_s64(v.m_quad.zw, v.m_quad.zw, 1);	// [w, z]
}

template <> inline void hkInt64Vector4::setPermutation<hkVectorPermutation::XYWZ>(hkInt64Vector4Parameter v)
{
	m_quad.xy = v.m_quad.xy;							// [x, y]
	m_quad.zw = vextq_s64(v.m_quad.zw, v.m_quad.zw, 1);	// [w, z]
}

template <> inline void hkInt64Vector4::setPermutation<hkVectorPermutation::YYWW>(hkInt64Vector4Parameter v)
{
	m_quad.xy = vdupq_lane_s64(vget_high_s64(v.m_quad.xy), 0);	// [y, y]
	m_quad.zw = vdupq_lane_s64(vget_high_s64(v.m_quad.zw), 0);	// [w, w]
}

template <> inline void hkInt64Vector4::setPermutation<hkVectorPermutation::WWWW>(hkInt64Vector4Parameter v)
{
	m_quad.xy = m_quad.zw = vdupq_lane_s64(vget_high_s64(v.m_quad.zw), 0);
}

template <>
inline void hkInt64Vector4::setUnsignedMul_128<2>(hkInt64Vector4Parameter origA, hkInt64Vector4Parameter origB)
{
	// [y, x]		[HI,	LO]				[HI,	LO]
	// origA.xy =	[a1a0,	*]	origA.zw =	[c1c0,	*]
	// origB.xy =	[b1b0,	*]	origB.zw =	[d1d0,	*]

	// We must compute [ (a1 * b1) << 64 + (a1 * b0 + a0 * b1) << 32 + a0 * b0, ...]
	const uint32x2_t a1a0	= vreinterpret_u32_s64(vget_high_s64(origA.m_quad.xy));	// [a1 a0]
	const uint32x2_t b1b0	= vreinterpret_u32_s64(vget_high_s64(origB.m_quad.xy));	// [b1 b0]
	const uint32x2_t c1c0	= vreinterpret_u32_s64(vget_high_s64(origA.m_quad.zw));	// [c1 c0]
	const uint32x2_t d1d0	= vreinterpret_u32_s64(vget_high_s64(origB.m_quad.zw));	// [d1 d0]

	// Multiply
	const uint64x2_t a1b1_a0b0	= vmull_u32(a1a0, b1b0);				// [a1 b1, a0 b0]
	const uint64x2_t c1d1_c0d0	= vmull_u32(c1c0, d1d0);				// [c1 d1, c0 d0]
	const uint64x2_t a0b1_a1b0	= vmull_u32(vrev64_u32(a1a0), b1b0);	// [a0 b1, a1 b0]
	const uint64x2_t c0d1_c1d0	= vmull_u32(vrev64_u32(c1c0), d1d0);	// [c0 d1, c1 d0]

	// Shuffle into proper places
	uint64x2_t			a0b0_c0d0	= vcombine_u64(vget_low_u64(c1d1_c0d0), vget_low_u64(a1b1_a0b0));	// [a0 b0, c0 d0]
	const uint64x2_t	a1b1_c1d1	= vcombine_u64(vget_high_u64(c1d1_c0d0),vget_high_u64(a1b1_a0b0));	// [a1 b1, c1 d1]
	uint64x2_t			a1b0_c1d0	= vcombine_u64(vget_low_u64(c0d1_c1d0),	vget_low_u64(a0b1_a1b0));	// [a1 b0, c1 d0]
	uint64x2_t			a0b1_c0d1	= vcombine_u64(vget_high_u64(c0d1_c1d0),vget_high_u64(a0b1_a1b0));	// [a0 b1, c0 d1]

	// Accumulate results into 3 registers:
	//	r0 = [0..63], r1 = [32..95], r2 = [64..127]
	uint64x2_t	r2			= vaddq_u64(a1b1_c1d1, vshrq_n_u64(a1b0_c1d0, 32));		// r2 = a1 * b1 + ((a1 * b0) >> 32)
				r2			= vaddq_u64(r2, vshrq_n_u64(a0b1_c0d1, 32));			// r2 += ((a0 * b1) >> 32)
				a1b0_c1d0	= vshrq_n_u64(vshlq_n_u64(a1b0_c1d0, 32), 32);
				a0b1_c0d1	= vshrq_n_u64(vshlq_n_u64(a0b1_c0d1, 32), 32);
	uint64x2_t	r1			= vaddq_u64(a1b0_c1d0, a0b1_c0d1);						// r1 = lo(a0 * b1) + lo(a1 * b0)
				r1			= vaddq_u64(r1, vshrq_n_u64(a0b0_c0d0, 32));			// r1 += (a0 * b0) >> 32
				r2			= vaddq_u64(r2, vshrq_n_u64(r1, 32));					// r2 += r1 >> 32 = [hi(a b), hi(c d)]

	// Finally, gather the results into the outputs
				a0b0_c0d0	= vshrq_n_u64(vshlq_n_u64(a0b0_c0d0, 32), 32);
	uint64x2_t	r0			= vorrq_u64(vshlq_n_u64(r1, 32), a0b0_c0d0);			// [lo(a b), lo(c d)]
	

	// Respect convention x = Hi, y = Lo!
	m_quad.xy = vreinterpretq_s64_u64(vcombine_u64(vget_high_u64(r2), vget_high_u64(r0)));	// a * b
	m_quad.zw = vreinterpretq_s64_u64(vcombine_u64(vget_low_u64(r2), vget_low_u64(r0)));	// c * d
}

//
//	Sets this.xy = a.x * b.x this.zw = a.z * b.z, i.e. multiplies the 2 64-bit operands and returns 2 128-bit numbers

inline void hkInt64Vector4::setMul_128(hkInt64Vector4Parameter origA, hkInt64Vector4Parameter origB)
{
	// [y, x]		[HI,	LO]				[HI,	LO]
	// origA.xy =	[a1a0,	*]	origA.zw =	[c1c0,	*]
	// origB.xy =	[b1b0,	*]	origB.zw =	[d1d0,	*]
	hkInt64Vector4 absA;	absA.setAbs(origA);
	hkInt64Vector4 absB;	absB.setAbs(origB);
	hkInt64Vector4 absAB;	absAB.setUnsignedMul_128<2>(absA, absB);

	// Set proper signs
	{																				// [X, Y,		Z, W]
		const hkVector4fComparison negA	= origA.lessZero();							// [*, a < 0,	*, c < 0]
		const hkVector4fComparison negB	= origB.lessZero();							// [*, b < 0,	*, d < 0]
		hkVector4fComparison negZeroA;	negZeroA.setOr(negA, origA.equalZero());	// [*, a <= 0,	*, c <= 0]
		hkVector4fComparison negZeroB;	negZeroB.setOr(negB, origB.equalZero());	// [*, b <= 0,	*, d <= 0]
		hkVector4fComparison negA_posB;	negA_posB.setAndNot(negA, negZeroB);
		hkVector4fComparison posA_negB;	posA_negB.setAndNot(negB, negZeroA);
		hkVector4fComparison neg;		neg.setOr(negA_posB, posA_negB);			// [*, sign(ab), *, sign(cd)]

		// Set proper signs
		hkVector4fComparison signAb;	signAb.m_mask = vdupq_lane_u32(vget_low_u32(neg.m_mask), 1);
		hkVector4fComparison signCd;	signCd.m_mask = vdupq_lane_u32(vget_high_u32(neg.m_mask),1);
		hkSimdInt<128> tmpAb;			absAB.storeXy(tmpAb);
		hkSimdInt<128> tmpCd;			absAB.storeZw(tmpCd);
		tmpAb.setFlipSign(tmpAb, signAb);
		tmpCd.setFlipSign(tmpCd, signCd);
		loadXy(tmpAb);
		loadZw(tmpCd);
	}
}

//
//	Sets this = vA * vB

inline void hkInt64Vector4::setMul(hkIntVectorParameter origA, hkIntVectorParameter origB)
{
	// Remove signs
	hkIntVector vA;	vA.setAbsS32(origA);
	hkIntVector vB;	vB.setAbsS32(origB);

	// Shuffle
	const uint32x2_t a_xy	= vget_low_u32(vA.m_quad);	// [ax, ay]
	const uint32x2_t a_zw	= vget_high_u32(vA.m_quad);	// [az, aw]
	const uint32x2_t b_xy	= vget_low_u32(vB.m_quad);	// [bx, by]
	const uint32x2_t b_zw	= vget_high_u32(vB.m_quad);	// [bz, bw]

	// Multiply unsigned values
	hkInt64Vector4 absAB;
	absAB.m_quad.xy = vreinterpretq_s64_u64(vmull_u32(a_xy, b_xy));	// [ax * bx, ay * by]
	absAB.m_quad.zw = vreinterpretq_s64_u64(vmull_u32(a_zw, b_zw));	// [az * bz, aw * bw]

	// Compute signs
	hkVector4fComparison negA_posB;	negA_posB.setAnd(origA.lessZeroS32(), origB.greaterZeroS32());
	hkVector4fComparison posA_negB;	posA_negB.setAnd(origB.lessZeroS32(), origA.greaterZeroS32());
	hkVector4fComparison negAB;		negAB.setOr(negA_posB, posA_negB);

	// Flip signs if case
	setFlipSign(absAB, negAB);
}

//
//	Returns the I-th component

template<> inline hkInt64 hkInt64Vector4::getComponent<0>() const	{	return vgetq_lane_s64(m_quad.xy, 0);	}
template<> inline hkInt64 hkInt64Vector4::getComponent<1>() const	{	return vgetq_lane_s64(m_quad.xy, 1);	}
template<> inline hkInt64 hkInt64Vector4::getComponent<2>() const	{	return vgetq_lane_s64(m_quad.zw, 0);	}
template<> inline hkInt64 hkInt64Vector4::getComponent<3>() const	{	return vgetq_lane_s64(m_quad.zw, 1);	}

inline hkInt64 hkInt64Vector4::getComponent(int I) const
{
	return reinterpret_cast<const hkInt64*>(this)[I];
}

//
//	Returns the maximum component among the first N.

template <>
inline hkInt64 hkInt64Vector4::horizontalMax<3>() const
{
	int64x2_t	zz	= vdupq_lane_s64(vget_low_s64(m_quad.zw), 0);			// [z, z]
	int64x2_t	xy	= hkInt64Vector4Impl::vec2x64_max(m_quad.xy, zz);		// [max(y, z), max(x, z)]
				zz	= vcombine_s64(vget_high_s64(xy), vget_low_s64(xy));	// [max(x, z), max(y, z)]
				zz	= hkInt64Vector4Impl::vec2x64_max(xy, zz);				// max(x, y, z)

	return vgetq_lane_s64(zz, 0);
}

//
//	Sets the I-th component to zero.

template <> inline void hkInt64Vector4::zeroComponent<0>()	{	m_quad.xy = vsetq_lane_s64(0, m_quad.xy, 0);	}
template <> inline void hkInt64Vector4::zeroComponent<1>()	{	m_quad.xy = vsetq_lane_s64(0, m_quad.xy, 1);	}
template <> inline void hkInt64Vector4::zeroComponent<2>()	{	m_quad.zw = vsetq_lane_s64(0, m_quad.zw, 0);	}
template <> inline void hkInt64Vector4::zeroComponent<3>()	{	m_quad.zw = vsetq_lane_s64(0, m_quad.zw, 1);	}

//
//	Loads the .xy components of this from a 128-bit int, where x is the High part and y is the Low part.

inline void hkInt64Vector4::loadXy(const hkSimdInt<128>& xyIn)
{
	const int64x2_t vx = vreinterpretq_s64_s32(xyIn.m_quad);
	m_quad.xy = vcombine_s64(vget_high_s64(vx), vget_low_s64(vx));
}

//
//	Loads the .zw components of this from a 128-bit int, where z is the High part and w is the Low part.

inline void hkInt64Vector4::loadZw(const hkSimdInt<128>& zwIn)
{
	const int64x2_t vx = vreinterpretq_s64_s32(zwIn.m_quad);
	m_quad.zw = vcombine_s64(vget_high_s64(vx), vget_low_s64(vx));
}

//
//	Retrieves the x and y components as an 128-bit int i, where x = Hi[i] and y = Lo[i]

inline void hkInt64Vector4::storeXy(hkSimdInt<128>& x) const
{
	x.m_quad = vreinterpretq_s32_s64(vcombine_s64(vget_high_s64(m_quad.xy), vget_low_s64(m_quad.xy)));
}

//
//	Retrieves the z and w components as an 128-bit int x, where z = Hi[x] and w = Lo[x]

inline void hkInt64Vector4::storeZw(hkSimdInt<128>& x) const
{
	x.m_quad = vreinterpretq_s32_s64(vcombine_s64(vget_high_s64(m_quad.zw), vget_low_s64(m_quad.zw)));
}

//
//	Sets this = a / b. Both a and b are assumed to be unsigned!

inline void hkInt64Vector4::setUnsignedDiv(hkInt64Vector4Parameter a, hkInt64Vector4Parameter b)
{
	const uint64x2_t axy	= vreinterpretq_u64_s64(a.m_quad.xy);
	const uint64x2_t bxy	= vreinterpretq_u64_s64(b.m_quad.xy);
	m_quad.xy = vreinterpretq_s64_u64(vcombine_u64(	vcreate_u64(vgetq_lane_u64(axy, 0) / vgetq_lane_u64(bxy, 0)),
													vcreate_u64(vgetq_lane_u64(axy, 1) / vgetq_lane_u64(bxy, 1))));

	const uint64x2_t azw	= vreinterpretq_u64_s64(a.m_quad.zw);
	const uint64x2_t bzw	= vreinterpretq_u64_s64(b.m_quad.zw);
	m_quad.zw = vreinterpretq_s64_u64(vcombine_u64(	vcreate_u64(vgetq_lane_u64(azw, 0) / vgetq_lane_u64(bzw, 0)),
													vcreate_u64(vgetq_lane_u64(azw, 1) / vgetq_lane_u64(bzw, 1))));
}

//
//	Converts the 4 64-bit ints to doubles

inline void hkInt64Vector4::convertS64ToF64(hkVector4d& vOut) const
{
	vOut.set((double)vgetq_lane_s64(m_quad.xy, 0), (double)vgetq_lane_s64(m_quad.xy, 1), (double)vgetq_lane_s64(m_quad.zw, 0), (double)vgetq_lane_s64(m_quad.zw, 1));
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
