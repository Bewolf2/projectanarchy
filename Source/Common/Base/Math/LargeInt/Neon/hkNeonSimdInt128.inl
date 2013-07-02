/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//
//	Sets this = i

inline void hkSimdInt<128>::setFromInt32(hkInt32 i)
{
	m_quad = vsetq_lane_s32(i, vdupq_n_s32(i >> 31), 0);
}

//
//	Sets this = 0

inline void hkSimdInt<128>::setZero()
{
	m_quad = vdupq_n_s32(0);
}

inline void hkSimdInt<128>::setFromInt64(hkInt64 i)
{
	const int64x1_t ll = vcreate_s64((hkUint64)i);
	m_quad = vreinterpretq_s32_s64(vcombine_s64(ll, vshr_n_s64(ll, 63)));
}

inline void hkSimdInt<128>::setFromUint64(hkUint64 l)
{
	const uint64x1_t ll = vcreate_u64(l);
	m_quad = vreinterpretq_s32_u64(vcombine_u64(ll, veor_u64(ll, ll)));
}

//
//	Sets this = a + b

inline void hkSimdInt<128>::setAdd(const hkSimdInt<128>& origA, const hkSimdInt<128>& origB)
{
	//																		[3, 2,			1, 0]
	const uint64x2_t a	= vreinterpretq_u64_s32(origA.m_quad);	// a	=	[a1,			a0]
	const uint64x2_t b	= vreinterpretq_u64_s32(origB.m_quad);	// b	=	[b1,			b0]
	const uint64x2_t ab	= vaddq_u64(a, b);						// ab	=	[lo(a1 + b1),	lo(a0 + b0)]

	// Compare ab < a unsigned.
	const int32x4_t m		= vdupq_n_s32(0x80000000);
	const int32x4_t mSum	= veorq_s32(vreinterpretq_s32_u64(ab), m);
	const int32x4_t mA		= veorq_s32(vreinterpretq_s32_u64(a), m);
	const uint32x4_t t		= vcltq_s32(mSum, mA);
	const uint32x4_t u		= vcgtq_s32(mSum, mA);
	uint32x4_t	z			= vorrq_u32(t, vcombine_u32(vrev64_u32(vget_low_u32(t)), vrev64_u32(vget_high_u32(t))));
				z			= vbicq_u32(z, vcombine_u32(vdup_lane_u32(vget_low_u32(u), 1), vdup_lane_u32(vget_high_u32(u), 1)));
	const uint64x2_t carry	= vcombine_u64(	vcreate_u64(0), 
											vget_low_u64(vshrq_n_u64(vreinterpretq_u64_u32(z), 63)));
	
	m_quad = vreinterpretq_s32_u64(vaddq_u64(ab, carry));
}

//
//	Increments this = this + 1 (faster than add(1)).

inline void hkSimdInt<128>::increment()
{
	// The result is (a0 == 0xFFFFFFFF) ? [a1 + 1, a0 + 1] : [a1, a0 + 1]
	const uint64x2_t one	= vdupq_n_u64(1);																	// [1, 1]
	const uint64x2_t incr11	= vaddq_u64(vreinterpretq_u64_s32(m_quad), one);									// [a1 + 1, a0 + 1]
	const uint64x2_t incr01	= vcombine_u64(vget_low_u64(incr11), vget_high_u64(vreinterpretq_u64_s32(m_quad)));	// [a1, a0 + 1]

	// Compare a0 with 0xFFFFFFFFFFFFFFFF
	uint32x2_t	cmpFF	= vceq_s32(vget_low_s32(m_quad), vdup_n_s32(-1));
	cmpFF	= vand_u32(cmpFF, vrev64_u32(cmpFF));

	// Choose one of the results
	m_quad = vreinterpretq_s32_u64(vbslq_u64(vreinterpretq_u64_u32(vdupq_lane_u32(cmpFF, 0)), incr11, incr01));
}

//
//	Returns this < 0

template <> inline const hkVector4fComparison hkSimdInt<128>::lessZero<hkVector4fComparison>() const
{
	// We need to put the sign bit everywhere
	const int64x1_t hi = vshr_n_s64(vget_high_s64(vreinterpretq_s64_s32(m_quad)), 63);
	hkVector4fComparison cmp;
	cmp.m_mask = vreinterpretq_u32_s64(vdupq_lane_s64(hi, 0));
	return cmp;
}

template <> inline const hkVector4dComparison hkSimdInt<128>::lessZero<hkVector4dComparison>() const
{
	// We need to put the sign bit everywhere
	hkVector4dComparison cmp;
	cmp.set(lessZero().getMask());
	return cmp;
}

inline const hkVector4fComparison hkSimdInt<128>::lessZero() const
{
	return lessZero<hkVector4fComparison>();
}

//
//	Returns this == 0

template <> inline const hkVector4fComparison hkSimdInt<128>::equalZero<hkVector4fComparison>() const
{
	const int32x2_t xz_yw	= vorr_s32(vget_low_s32(m_quad), vget_high_s32(m_quad));	// [x | z, y | w]
	const int32x2_t xyzw	= vorr_s32(xz_yw, vrev64_s32(xz_yw));						// [x | y | z | w, x | y | z | w]
	const uint32x2_t eq		= vceq_s32(xyzw, vdup_n_s32(0));

	hkVector4fComparison cmp;
	cmp.m_mask = vcombine_u32(eq, eq);
	return cmp;
}

template <> inline const hkVector4dComparison hkSimdInt<128>::equalZero<hkVector4dComparison>() const
{
	hkVector4dComparison cmp;
	cmp.set(equalZero().getMask());
	return cmp;
}

inline const hkVector4fComparison hkSimdInt<128>::equalZero() const
{
	return equalZero<hkVector4fComparison>();
}

//
//	Returns this == v

inline const hkVector4fComparison hkSimdInt<128>::equal(const hkSimdInt<128>& v) const
{
	const uint32x4_t xyzw	= vceqq_s32(m_quad, v.m_quad);
	const uint32x2_t xz_yw	= vand_u32(vget_low_u32(xyzw), vget_high_u32(xyzw));	// [x | z, y | w]
	const uint32x2_t eq		= vand_u32(xz_yw, vrev64_u32(xz_yw));					// [x | y | z | w, x | y | z | w]

	hkVector4fComparison cmp;
	cmp.m_mask = vcombine_u32(eq, eq);
	return cmp;
}

//
//	Sets this = cmp ? vTrue : vFalse

inline void hkSimdInt<128>::setSelect(hkVector4fComparisonParameter cmp, const hkSimdInt<128>& vTrue, const hkSimdInt<128>& vFalse)
{
	m_quad = vbslq_s32(cmp.m_mask, vTrue.m_quad, vFalse.m_quad);
}

//
//	Sets this = a & b

inline void hkSimdInt<128>::setAnd(const hkSimdInt<128>& a, const hkSimdInt<128>& b)
{
	m_quad = vandq_s32(a.m_quad, b.m_quad);
}

//
//	Sets this = a | b

inline void hkSimdInt<128>::setOr(const hkSimdInt<128>& a, const hkSimdInt<128>& b)
{
	m_quad = vorrq_s32(a.m_quad, b.m_quad);
}

//
//	Sets this = ~a

inline void hkSimdInt<128>::setNot(const hkSimdInt<128>& v)
{
	m_quad = vmvnq_s32(v.m_quad);
}

//
//	Sets this = a << I

template <> inline void hkSimdInt<128>::setShiftLeft<0>		(const hkSimdInt<128>& a)	{ m_quad = a.m_quad; }
template <> inline void hkSimdInt<128>::setShiftLeft<8>		(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vdupq_n_u8(0), vreinterpretq_u8_s32(a.m_quad), 15)); }
template <> inline void hkSimdInt<128>::setShiftLeft<16>	(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vdupq_n_u8(0), vreinterpretq_u8_s32(a.m_quad), 14)); }
template <> inline void hkSimdInt<128>::setShiftLeft<24>	(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vdupq_n_u8(0), vreinterpretq_u8_s32(a.m_quad), 13)); }
template <> inline void hkSimdInt<128>::setShiftLeft<32>	(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vdupq_n_u8(0), vreinterpretq_u8_s32(a.m_quad), 12)); }
template <> inline void hkSimdInt<128>::setShiftLeft<40>	(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vdupq_n_u8(0), vreinterpretq_u8_s32(a.m_quad), 11)); }
template <> inline void hkSimdInt<128>::setShiftLeft<48>	(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vdupq_n_u8(0), vreinterpretq_u8_s32(a.m_quad), 10)); }
template <> inline void hkSimdInt<128>::setShiftLeft<56>	(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vdupq_n_u8(0), vreinterpretq_u8_s32(a.m_quad), 9)); }
template <> inline void hkSimdInt<128>::setShiftLeft<64>	(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vdupq_n_u8(0), vreinterpretq_u8_s32(a.m_quad), 8)); }
template <> inline void hkSimdInt<128>::setShiftLeft<72>	(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vdupq_n_u8(0), vreinterpretq_u8_s32(a.m_quad), 7)); }
template <> inline void hkSimdInt<128>::setShiftLeft<80>	(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vdupq_n_u8(0), vreinterpretq_u8_s32(a.m_quad), 6)); }
template <> inline void hkSimdInt<128>::setShiftLeft<88>	(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vdupq_n_u8(0), vreinterpretq_u8_s32(a.m_quad), 5)); }
template <> inline void hkSimdInt<128>::setShiftLeft<96>	(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vdupq_n_u8(0), vreinterpretq_u8_s32(a.m_quad), 4)); }
template <> inline void hkSimdInt<128>::setShiftLeft<104>	(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vdupq_n_u8(0), vreinterpretq_u8_s32(a.m_quad), 3)); }
template <> inline void hkSimdInt<128>::setShiftLeft<112>	(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vdupq_n_u8(0), vreinterpretq_u8_s32(a.m_quad), 2)); }
template <> inline void hkSimdInt<128>::setShiftLeft<120>	(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vdupq_n_u8(0), vreinterpretq_u8_s32(a.m_quad), 1)); }
template <> inline void hkSimdInt<128>::setShiftLeft<128>	(const hkSimdInt<128>& a)	{ m_quad = vdupq_n_s32(0); }

template <int N>
inline void hkSimdInt<128>::setShiftLeft(const hkSimdInt<128>& a)
{
	// Shift first part, in number of bytes
	hkSimdInt<128> ts;			ts.setShiftLeft<(((N) >> 3) << 3)>(a);
	const uint64x2_t tmpShift	= vreinterpretq_u64_s32(ts.m_quad);

	// Shift rest, with the remaining number of bits
	// tmpShift = [hi, lo]
	// shiftR	= [hi, lo] >> (64 - (N & 7)) = most significant (N & 7) bits
	uint64x2_t shiftR = vshrq_n_u64(tmpShift, 64 - (N & 7));

	// shiftR	= [0x0...0lo, 0] 
	shiftR = vcombine_u64(vcreate_u64(0), vget_low_u64(shiftR));
	m_quad = vreinterpretq_s32_u64(vorrq_u64(vshlq_n_u64(tmpShift, N & 7), shiftR));
}

//
//	Sets this = a >> I

template <> inline void hkSimdInt<128>::setShiftRight<0, HK_VALUE_UNSIGNED>		(const hkSimdInt<128>& a)	{ m_quad = a.m_quad; }
template <> inline void hkSimdInt<128>::setShiftRight<8, HK_VALUE_UNSIGNED>		(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vreinterpretq_u8_s32(a.m_quad), vdupq_n_u8(0), 1)); }
template <> inline void hkSimdInt<128>::setShiftRight<16, HK_VALUE_UNSIGNED>	(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vreinterpretq_u8_s32(a.m_quad), vdupq_n_u8(0), 2)); }
template <> inline void hkSimdInt<128>::setShiftRight<24, HK_VALUE_UNSIGNED>	(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vreinterpretq_u8_s32(a.m_quad), vdupq_n_u8(0), 3)); }
template <> inline void hkSimdInt<128>::setShiftRight<32, HK_VALUE_UNSIGNED>	(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vreinterpretq_u8_s32(a.m_quad), vdupq_n_u8(0), 4)); }
template <> inline void hkSimdInt<128>::setShiftRight<40, HK_VALUE_UNSIGNED>	(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vreinterpretq_u8_s32(a.m_quad), vdupq_n_u8(0), 5)); }
template <> inline void hkSimdInt<128>::setShiftRight<48, HK_VALUE_UNSIGNED>	(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vreinterpretq_u8_s32(a.m_quad), vdupq_n_u8(0), 6)); }
template <> inline void hkSimdInt<128>::setShiftRight<56, HK_VALUE_UNSIGNED>	(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vreinterpretq_u8_s32(a.m_quad), vdupq_n_u8(0), 7)); }
template <> inline void hkSimdInt<128>::setShiftRight<64, HK_VALUE_UNSIGNED>	(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vreinterpretq_u8_s32(a.m_quad), vdupq_n_u8(0), 8)); }
template <> inline void hkSimdInt<128>::setShiftRight<72, HK_VALUE_UNSIGNED>	(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vreinterpretq_u8_s32(a.m_quad), vdupq_n_u8(0), 9)); }
template <> inline void hkSimdInt<128>::setShiftRight<80, HK_VALUE_UNSIGNED>	(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vreinterpretq_u8_s32(a.m_quad), vdupq_n_u8(0), 10)); }
template <> inline void hkSimdInt<128>::setShiftRight<88, HK_VALUE_UNSIGNED>	(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vreinterpretq_u8_s32(a.m_quad), vdupq_n_u8(0), 11)); }
template <> inline void hkSimdInt<128>::setShiftRight<96, HK_VALUE_UNSIGNED>	(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vreinterpretq_u8_s32(a.m_quad), vdupq_n_u8(0), 12)); }
template <> inline void hkSimdInt<128>::setShiftRight<104, HK_VALUE_UNSIGNED>	(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vreinterpretq_u8_s32(a.m_quad), vdupq_n_u8(0), 13)); }
template <> inline void hkSimdInt<128>::setShiftRight<112, HK_VALUE_UNSIGNED>	(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vreinterpretq_u8_s32(a.m_quad), vdupq_n_u8(0), 14)); }
template <> inline void hkSimdInt<128>::setShiftRight<120, HK_VALUE_UNSIGNED>	(const hkSimdInt<128>& a)	{ m_quad = vreinterpretq_s32_u8(vextq_u8(vreinterpretq_u8_s32(a.m_quad), vdupq_n_u8(0), 15)); }
template <> inline void hkSimdInt<128>::setShiftRight<128, HK_VALUE_UNSIGNED>	(const hkSimdInt<128>& a)	{ m_quad = vdupq_n_s32(0); }

namespace hkSimdInt128Impl
{
	template <int N, hkMathValueType VAL> struct Shr {};

	template <int N> struct Shr<N, HK_VALUE_UNSIGNED>
	{
		static inline void HK_CALL shr(const hkSimdInt<128>& iIn, hkSimdInt<128>& iOut)
		{
			// Shift first part, in number of bytes
#ifdef HK_PLATFORM_IOS
			uint64x2_t tmpShift;
			switch ( (N) >> 3 )
			{
			case 0: tmpShift = vreinterpretq_u64_s32(iIn.m_quad);													break;
			case 1: tmpShift = vreinterpretq_u64_u8(vextq_u8(vreinterpretq_u8_s32(iIn.m_quad), vdupq_n_u8(0), 1));	break;
			case 2: tmpShift = vreinterpretq_u64_u8(vextq_u8(vreinterpretq_u8_s32(iIn.m_quad), vdupq_n_u8(0), 2));	break;
			case 3: tmpShift = vreinterpretq_u64_u8(vextq_u8(vreinterpretq_u8_s32(iIn.m_quad), vdupq_n_u8(0), 3));	break;
			case 4: tmpShift = vreinterpretq_u64_u8(vextq_u8(vreinterpretq_u8_s32(iIn.m_quad), vdupq_n_u8(0), 4));	break;
			case 5: tmpShift = vreinterpretq_u64_u8(vextq_u8(vreinterpretq_u8_s32(iIn.m_quad), vdupq_n_u8(0), 5));	break;
			case 6: tmpShift = vreinterpretq_u64_u8(vextq_u8(vreinterpretq_u8_s32(iIn.m_quad), vdupq_n_u8(0), 6));	break;
			case 7: tmpShift = vreinterpretq_u64_u8(vextq_u8(vreinterpretq_u8_s32(iIn.m_quad), vdupq_n_u8(0), 7));	break;
			case 8: tmpShift = vreinterpretq_u64_u8(vextq_u8(vreinterpretq_u8_s32(iIn.m_quad), vdupq_n_u8(0), 8));	break;
			case 9: tmpShift = vreinterpretq_u64_u8(vextq_u8(vreinterpretq_u8_s32(iIn.m_quad), vdupq_n_u8(0), 9));	break;
			case 10: tmpShift = vreinterpretq_u64_u8(vextq_u8(vreinterpretq_u8_s32(iIn.m_quad), vdupq_n_u8(0), 10));break;
			case 11: tmpShift = vreinterpretq_u64_u8(vextq_u8(vreinterpretq_u8_s32(iIn.m_quad), vdupq_n_u8(0), 11));break;
			case 12: tmpShift = vreinterpretq_u64_u8(vextq_u8(vreinterpretq_u8_s32(iIn.m_quad), vdupq_n_u8(0), 12));break;
			case 13: tmpShift = vreinterpretq_u64_u8(vextq_u8(vreinterpretq_u8_s32(iIn.m_quad), vdupq_n_u8(0), 13));break;
			case 14: tmpShift = vreinterpretq_u64_u8(vextq_u8(vreinterpretq_u8_s32(iIn.m_quad), vdupq_n_u8(0), 14));break;
			case 15: tmpShift = vreinterpretq_u64_u8(vextq_u8(vreinterpretq_u8_s32(iIn.m_quad), vdupq_n_u8(0), 15));break;
			default: tmpShift = vdupq_n_u64(0);																		break;
			}
#else
			const uint64x2_t tmpShift = vreinterpretq_u64_u8(vextq_u8(vreinterpretq_u8_s32(iIn.m_quad), vdupq_n_u8(0), ((N) >> 3)));
#endif

			// Shift rest, with the remaining number of bits
			// tmpShift = [hi, lo]
			// shiftR	= [hi, lo] << (64 - (N & 7)) = least significant (N & 7) bits
			uint64x2_t shiftL = vshlq_n_u64(tmpShift, 64 - (N & 7));

			// shiftR	= [0, 0x0...0hi]
			shiftL = vcombine_u64(vget_high_u64(shiftL), vcreate_u64(0));
			iOut.m_quad = vreinterpretq_s32_u64(vorrq_u64(vshrq_n_u64(tmpShift, N & 7), shiftL));
		}
	};

	template <int N> struct Shr<N, HK_VALUE_SIGNED>
	{
		static inline void HK_CALL shr(const hkSimdInt<128>& iIn, hkSimdInt<128>& iOut)
		{
			hkSimdInt<128> u;	Shr<N, HK_VALUE_UNSIGNED>::shr(iIn, u);
			hkSimdInt<128> s;	s.m_quad = vshrq_n_s32(vdupq_lane_s32(vget_high_s32(iIn.m_quad), 1), 31);
			s.setShiftLeft<128 - N>(s);
			iOut.setOr(u, s);
		}
	};
}

template <int I, hkMathValueType VAL>
inline void hkSimdInt<128>::setShiftRight(const hkSimdInt<128>& a)
{
	hkSimdInt128Impl::Shr<I, VAL>::shr(a, *this);
}

template <int I>
inline void hkSimdInt<128>::setShiftRight(const hkSimdInt<128>& a)
{
	setShiftRight<I, HK_VALUE_UNSIGNED>(a);
}

//
//	Returns the I-th 32-bit word, where LSB = 0, MSB = 3

template<> inline int hkSimdInt<128>::getWord<3>() const	{	return vgetq_lane_s32(m_quad, 3);	}
template<> inline int hkSimdInt<128>::getWord<2>() const	{	return vgetq_lane_s32(m_quad, 2);	}
template<> inline int hkSimdInt<128>::getWord<1>() const	{	return vgetq_lane_s32(m_quad, 1);	}
template<> inline int hkSimdInt<128>::getWord<0>() const	{	return vgetq_lane_s32(m_quad, 0);	}

//
//	Returns the I-th 64-bit word, where LSB = 0, MSB = 3

template <> inline hkInt64 hkSimdInt<128>::getDoubleWord<1>() const	{	return vgetq_lane_s64(vreinterpretq_s64_s32(m_quad), 1);	}
template <> inline hkInt64 hkSimdInt<128>::getDoubleWord<0>() const	{	return vgetq_lane_s64(vreinterpretq_s64_s32(m_quad), 0);	}

//
//	Returns the number of leading zeros

inline int hkSimdInt<128>::countLeadingZeros() const
{
	// 32 -> 64
	const int32x4_t lz3210	= vclzq_s32(m_quad);			// [lz3, lz2, lz1, lz0]
	const int32x2_t lz32	= vget_high_s32(lz3210);		// [lz3, lz2]
	const int32x2_t lz10	= vget_low_s32(lz3210);			// [lz1, lz0]
	const int32x2_t lz31	= vzip_s32(lz10, lz32).val[1];	// [lz3, lz1]
	const int32x2_t lz32_10	= vbsl_s32(vceq_s32(lz31, vdup_n_s32(32)), vpadd_s32(lz10, lz32), lz31);	// [lz3 == 32 ? 32 + lz2 : lz3, lz1 == 32 ? 32 + lz0 : lz1]

	// 64 -> 128
	const int32x2_t hi		= vdup_lane_s32(lz32_10, 1);	// [lz32, lz32]
	return vget_lane_s32(vbsl_s32(vceq_s32(hi, vdup_n_s32(64)), vpadd_s32(lz32_10, lz32_10), hi), 0);	// [lz32 == 64 ? 64 + lz10 : lz32]
}

//
//	Returns the number of trailing zeros

inline int hkSimdInt<128>::countTrailingZeros() const
{
	const int nz32 = hkMath::countTrailingZeros<hkUint64>(vget_lane_u64(vget_high_u64(vreinterpretq_u64_s32(m_quad)), 0));
	const int nz10 = hkMath::countTrailingZeros<hkUint64>(vget_lane_u64(vget_low_u64(vreinterpretq_u64_s32(m_quad)), 0));
	return	(nz10 < 64) ? nz10 : (64 + nz32);
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
