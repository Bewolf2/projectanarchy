/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_COMPILER_HAS_INTRINSICS_NEON
#error Cant include this header on non Neon setups..
#endif

HK_FORCE_INLINE /*static*/ const hkSimdFloat32 HK_CALL hkSimdFloat32::convert(const hkSingleFloat32& x)
{
	hkSimdFloat32 sr;
	sr.m_real = x;
	return sr;
	//HK_ASSERT(0x7c3a4d98, reinterpret_cast<hkUint32*>(&m_real)[0] == reinterpret_cast<hkUint32*>(&m_real)[1]);
}

template<int vectorConstant>
HK_FORCE_INLINE /*static*/ const hkSimdFloat32 HK_CALL hkSimdFloat32::getConstant()
{
	HK_COMPILE_TIME_ASSERT2( 
		(vectorConstant!=HK_QUADREAL_1000) && (vectorConstant!=HK_QUADREAL_0100) && (vectorConstant!=HK_QUADREAL_0010) && (vectorConstant!=HK_QUADREAL_0001) &&
		(vectorConstant!=HK_QUADREAL_m11m11) && (vectorConstant!=HK_QUADREAL_1248) && (vectorConstant!=HK_QUADREAL_8421) && (vectorConstant!=HK_QUADREAL_1010)  && (vectorConstant!=HK_QUADREAL_1100)
		, HK_SIMDFLOAT_ILLEGAL_CONSTANT_REQUEST);
	return convert(vget_low_f32(*(g_vectorfConstants + vectorConstant)));
}

HK_FORCE_INLINE /*static*/ const hkSimdFloat32 HK_CALL hkSimdFloat32::getConstant(hkVectorConstant constant)
{
	HK_MATH_ASSERT( 0x909ff234,
		(constant!=HK_QUADREAL_1000) && (constant!=HK_QUADREAL_0100) && (constant!=HK_QUADREAL_0010) && (constant!=HK_QUADREAL_0001) &&
		(constant!=HK_QUADREAL_m11m11) && (constant!=HK_QUADREAL_1248) && (constant!=HK_QUADREAL_8421) && (constant!=HK_QUADREAL_1010)  && (constant!=HK_QUADREAL_1100)
		, "not a simdreal constant");
	return convert(vget_low_f32(*(g_vectorfConstants + constant)));
}

#ifndef HK_DISABLE_IMPLICIT_SIMDREAL_FLOAT_CONVERSION
HK_FORCE_INLINE hkSimdFloat32::hkSimdFloat32(const hkFloat32& x)
{
	m_real = vdup_n_f32( x );
}

HK_FORCE_INLINE hkSimdFloat32::operator hkFloat32() const
{
	return vget_lane_f32(m_real, 0);
}
#endif

HK_FORCE_INLINE hkFloat32 hkSimdFloat32::getReal() const
{
	return vget_lane_f32(m_real, 0);
}

HK_FORCE_INLINE void hkSimdFloat32::setFromFloat(const hkFloat32& x)
{
	m_real = vld1_dup_f32( (const float32_t*)&x );
}

HK_FORCE_INLINE void hkSimdFloat32::setFromFloat(const hkDouble64& x)
{
	hkFloat32 f = hkFloat32(x);
	m_real = vld1_dup_f32( (const float32_t*)&f );
}

HK_FORCE_INLINE void hkSimdFloat32::setFromHalf(const hkHalf& h)
{
	float x = h;
	m_real = vld1_dup_f32( (const float32_t*)&x );
}

HK_FORCE_INLINE void hkSimdFloat32::setFromInt32(const hkInt32&  x)
{
	int32x2_t i = vld1_dup_s32( &x );
	m_real = vcvt_f32_s32(i); 
}

HK_FORCE_INLINE void hkSimdFloat32::setFromUint16(const hkUint16& x)
{
	int i = x;
	setFromInt32(i);
}

HK_FORCE_INLINE void hkSimdFloat32::setFromUint8(const hkUint8& x)
{
	int i = x;
	setFromInt32(i);
}

HK_FORCE_INLINE void hkSimdFloat32::setZero()
{
	m_real = vcreate_f32(0); 
}

HK_FORCE_INLINE void hkSimdFloat32::storeSaturateInt32(hkInt32* HK_RESTRICT result) const
{
	int32x2_t i = vcvt_s32_f32(m_real);
	vst1_lane_s32(result, i, 0);
}


HK_FORCE_INLINE void hkSimdFloat32::storeSaturateUint16(hkUint16* HK_RESTRICT result) const
{
	int32x2_t i = vcvt_s32_f32(m_real);
	uint16x4_t clamped = vqmovun_s32(vcombine_s32(i,i));
	vst1_lane_u16(result, clamped, 0);
}

HK_FORCE_INLINE const hkSimdFloat32 hkSimdFloat32::operator+ (hkSimdFloat32Parameter r) const
{
	return hkSimdFloat32::convert(vadd_f32(m_real,r.m_real));
}

HK_FORCE_INLINE const hkSimdFloat32 hkSimdFloat32::operator- (hkSimdFloat32Parameter r) const
{
	return hkSimdFloat32::convert(vsub_f32(m_real,r.m_real));
}

HK_FORCE_INLINE const hkSimdFloat32 hkSimdFloat32::operator* (hkSimdFloat32Parameter r) const
{
	return hkSimdFloat32::convert(vmul_f32(m_real,r.m_real));
}

#ifdef HK_DISABLE_OLD_VECTOR4_INTERFACE

HK_FORCE_INLINE bool hkSimdFloat32::operator< (hkSimdFloat32Parameter r) const
{
	uint32x2_t mask = vclt_f32(m_real, r.m_real);
	return vget_lane_u32(mask,0);
}

HK_FORCE_INLINE bool hkSimdFloat32::operator<= (hkSimdFloat32Parameter r) const
{
	uint32x2_t mask = vcle_f32(m_real, r.m_real);
	return vget_lane_u32(mask,0);
}

HK_FORCE_INLINE bool hkSimdFloat32::operator> (hkSimdFloat32Parameter r) const
{
	uint32x2_t mask = vcgt_f32(m_real, r.m_real);
	return vget_lane_u32(mask,0);
}

HK_FORCE_INLINE bool hkSimdFloat32::operator>= (hkSimdFloat32Parameter r) const
{
	uint32x2_t mask = vcge_f32(m_real, r.m_real);
	return vget_lane_u32(mask,0);
}

HK_FORCE_INLINE bool hkSimdFloat32::operator== (hkSimdFloat32Parameter r) const
{
	uint32x2_t mask = vceq_f32(m_real, r.m_real);
	return vget_lane_u32(mask,0);
}

HK_FORCE_INLINE bool hkSimdFloat32::operator!= (hkSimdFloat32Parameter r) const
{
	uint32x2_t mask = vceq_f32(m_real, r.m_real);
	mask = vmvn_u32(mask);
	return vget_lane_u32(mask,0);
}

#endif

HK_FORCE_INLINE const hkSimdFloat32 hkSimdFloat32::operator-() const
{
	return hkSimdFloat32::convert(vneg_f32(m_real));
}

HK_FORCE_INLINE const hkVector4fComparison hkSimdFloat32::less(hkSimdFloat32Parameter a) const
{
	uint32x2_t mask = vclt_f32(m_real, a.m_real);
	return hkVector4fComparison::convert(vcombine_u32(mask,mask));
}

HK_FORCE_INLINE const hkVector4fComparison hkSimdFloat32::greater(hkSimdFloat32Parameter a) const
{
	uint32x2_t mask = vcgt_f32(m_real, a.m_real);
	return hkVector4fComparison::convert(vcombine_u32(mask,mask));
}

HK_FORCE_INLINE const hkVector4fComparison hkSimdFloat32::greaterEqual(hkSimdFloat32Parameter a) const
{
	uint32x2_t mask = vcge_f32(m_real, a.m_real);
	return hkVector4fComparison::convert(vcombine_u32(mask,mask));
}

HK_FORCE_INLINE const hkVector4fComparison hkSimdFloat32::lessEqual(hkSimdFloat32Parameter a) const
{
	uint32x2_t mask = vcle_f32(m_real, a.m_real);
	return hkVector4fComparison::convert(vcombine_u32(mask,mask));
}

HK_FORCE_INLINE const hkVector4fComparison hkSimdFloat32::equal(hkSimdFloat32Parameter a) const
{
	uint32x2_t mask = vceq_f32(m_real, a.m_real);
	return hkVector4fComparison::convert(vcombine_u32(mask,mask));
}

HK_FORCE_INLINE const hkVector4fComparison hkSimdFloat32::notEqual(hkSimdFloat32Parameter a) const
{
	uint32x2_t mask = vceq_f32(m_real, a.m_real);
	mask = vmvn_u32(mask);
	return hkVector4fComparison::convert(vcombine_u32(mask,mask));
}

HK_FORCE_INLINE const hkVector4fComparison hkSimdFloat32::lessZero() const
{
	uint32x2_t mask = vclt_f32(m_real, vcreate_f32(0));  // todo how to use VCLT #0
	return hkVector4fComparison::convert(vcombine_u32(mask,mask));
}

HK_FORCE_INLINE const hkVector4fComparison hkSimdFloat32::lessEqualZero() const
{
	uint32x2_t mask = vcle_f32(m_real, vcreate_f32(0));  // todo how to use VCLE #0
	return hkVector4fComparison::convert(vcombine_u32(mask,mask));
}

HK_FORCE_INLINE const hkVector4fComparison hkSimdFloat32::greaterZero() const
{
	uint32x2_t mask = vcgt_f32(m_real, vcreate_f32(0));  // todo how to use VCGT #0
	return hkVector4fComparison::convert(vcombine_u32(mask,mask));
}

HK_FORCE_INLINE const hkVector4fComparison hkSimdFloat32::greaterEqualZero() const
{
	uint32x2_t mask = vcge_f32(m_real, vcreate_f32(0));  // todo how to use VCGE #0
	return hkVector4fComparison::convert(vcombine_u32(mask,mask));
}

HK_FORCE_INLINE const hkVector4fComparison hkSimdFloat32::equalZero() const
{
	uint32x2_t mask = vceq_f32(m_real, vcreate_f32(0)); // todo how to use VCEQ #0
	return hkVector4fComparison::convert(vcombine_u32(mask,mask));
}

HK_FORCE_INLINE const hkVector4fComparison hkSimdFloat32::notEqualZero() const
{
	uint32x2_t mask = vceq_f32(m_real, vcreate_f32(0)); // todo how to use VCEQ #0
	mask = vmvn_u32(mask);
	return hkVector4fComparison::convert(vcombine_u32(mask,mask));
}

HK_FORCE_INLINE hkBool32 hkSimdFloat32::isSignBitSet() const
{
	static const uint32x2_t allsigns = HK_NEON_CONSTANT2I( 0x80000000, 0x80000000 );
	uint32x2_t mask = vtst_u32(vreinterpret_u32_f32(m_real), allsigns);
	return vget_lane_u32(mask,0);
}

HK_FORCE_INLINE hkBool32 hkSimdFloat32::isSignBitClear() const
{
	static const uint32x2_t allsigns = HK_NEON_CONSTANT2I( 0x80000000, 0x80000000 );
	uint32x2_t mask = vtst_u32(vreinterpret_u32_f32(m_real), allsigns);
	mask = vmvn_u32(mask);
	return vget_lane_u32(mask,0);
}

HK_FORCE_INLINE const hkVector4fComparison hkSimdFloat32::signBitSet() const
{
	static const uint32x2_t allsigns = HK_NEON_CONSTANT2I( 0x80000000, 0x80000000 );
	uint32x2_t mask = vtst_u32(vreinterpret_u32_f32(m_real), allsigns);
	return hkVector4fComparison::convert(vcombine_u32(mask,mask));
}

HK_FORCE_INLINE const hkVector4fComparison hkSimdFloat32::signBitClear() const
{
	static const uint32x2_t allsigns = HK_NEON_CONSTANT2I( 0x80000000, 0x80000000 );
	uint32x2_t mask = vtst_u32(vreinterpret_u32_f32(m_real), allsigns);
	mask = vmvn_u32(mask);
	return hkVector4fComparison::convert(vcombine_u32(mask,mask));
}

HK_FORCE_INLINE hkBool32 hkSimdFloat32::isLess(hkSimdFloat32Parameter a) const
{
	uint32x2_t mask = vclt_f32(m_real, a.m_real);
	return vget_lane_u32(mask,0);
}

HK_FORCE_INLINE hkBool32 hkSimdFloat32::isLessEqual(hkSimdFloat32Parameter a) const
{
	uint32x2_t mask = vcle_f32(m_real, a.m_real);
	return vget_lane_u32(mask,0);
}

HK_FORCE_INLINE hkBool32 hkSimdFloat32::isGreater(hkSimdFloat32Parameter a) const
{
	uint32x2_t mask = vcgt_f32(m_real, a.m_real);
	return vget_lane_u32(mask,0);
}

HK_FORCE_INLINE hkBool32 hkSimdFloat32::isGreaterEqual(hkSimdFloat32Parameter a) const
{
	uint32x2_t mask = vcge_f32(m_real, a.m_real);
	return vget_lane_u32(mask,0);
}

HK_FORCE_INLINE hkBool32 hkSimdFloat32::isEqual(hkSimdFloat32Parameter a) const
{
	uint32x2_t mask = vceq_f32(m_real, a.m_real);
	return vget_lane_u32(mask,0);
}

HK_FORCE_INLINE hkBool32 hkSimdFloat32::isNotEqual(hkSimdFloat32Parameter a) const
{
	uint32x2_t mask = vceq_f32(m_real, a.m_real);
	mask = vmvn_u32(mask);
	return vget_lane_u32(mask,0);
}

HK_FORCE_INLINE hkBool32 hkSimdFloat32::isLessZero() const
{
	uint32x2_t mask = vclt_f32(m_real, vcreate_f32(0));  // todo how to use VCLT #0
	return vget_lane_u32(mask,0);
}

HK_FORCE_INLINE hkBool32 hkSimdFloat32::isLessEqualZero() const
{
	uint32x2_t mask = vcle_f32(m_real, vcreate_f32(0));  // todo how to use VCLE #0
	return vget_lane_u32(mask,0);
}

HK_FORCE_INLINE hkBool32 hkSimdFloat32::isGreaterZero() const
{
	uint32x2_t mask = vcgt_f32(m_real, vcreate_f32(0));  // todo how to use VCGT #0
	return vget_lane_u32(mask,0);
}

HK_FORCE_INLINE hkBool32 hkSimdFloat32::isGreaterEqualZero() const
{
	uint32x2_t mask = vcge_f32(m_real, vcreate_f32(0));  // todo how to use VCGE #0
	return vget_lane_u32(mask,0);
}

HK_FORCE_INLINE hkBool32 hkSimdFloat32::isEqualZero() const
{
	uint32x2_t mask = vceq_f32(m_real, vcreate_f32(0));  // todo how to use VCEQ #0
	return vget_lane_u32(mask,0);
}

HK_FORCE_INLINE hkBool32 hkSimdFloat32::isNotEqualZero() const
{
	uint32x2_t mask = vceq_f32(m_real, vcreate_f32(0));  // todo how to use VCEQ #0
	mask = vmvn_u32(mask);
	return vget_lane_u32(mask,0);
}

HK_FORCE_INLINE hkBool32 hkSimdFloat32::isOk() const
{
	static const uint32x2_t quietnan = HK_NEON_CONSTANT2I( 0x7FC00000, 0x7FC00000 );
	uint32x2_t mask = vand_u32(vreinterpret_u32_f32(m_real), quietnan);
	uint32x2_t test = vceq_u32(mask, quietnan);
	test = vmvn_u32(test);
	return vget_lane_u32(test,0);
}

HK_FORCE_INLINE void hkSimdFloat32::setSelect( hkVector4fComparisonParameter comp, hkSimdFloat32Parameter trueValue, hkSimdFloat32Parameter falseValue )
{
	HK_MATH_ASSERT(0xc78f08e, comp.allAreSet() || (comp.getMask() == hkVector4ComparisonMask::MASK_NONE), "illegal compare mask");
	m_real = vbsl_f32( vget_low_u32(comp.m_mask), trueValue.m_real, falseValue.m_real );
}

HK_FORCE_INLINE void hkSimdFloat32::zeroIfFalse( hkVector4fComparisonParameter comp )
{
	HK_MATH_ASSERT(0xc78f08e, comp.allAreSet() || (comp.getMask() == hkVector4ComparisonMask::MASK_NONE), "illegal compare mask");
	uint32x2_t mask = vand_u32(vreinterpret_u32_f32(m_real), vget_low_u32(comp.m_mask));
	m_real = vreinterpret_f32_u32(mask); 
}

HK_FORCE_INLINE void hkSimdFloat32::zeroIfTrue( hkVector4fComparisonParameter comp )
{
	HK_MATH_ASSERT(0xc78f08e, comp.allAreSet() || (comp.getMask() == hkVector4ComparisonMask::MASK_NONE), "illegal compare mask");
	uint32x2_t mask = vbic_u32( vreinterpret_u32_f32(m_real), vget_low_u32(comp.m_mask) ); // a & !b (so bit clear and set bits in b)
	m_real = vreinterpret_f32_u32(mask); 
}

HK_FORCE_INLINE void hkSimdFloat32::setMin(  hkSimdFloat32Parameter a, hkSimdFloat32Parameter b ) 
{
	m_real = vmin_f32( a.m_real, b.m_real );
}

HK_FORCE_INLINE void hkSimdFloat32::setMax(  hkSimdFloat32Parameter a, hkSimdFloat32Parameter b ) 
{
	m_real = vmax_f32( a.m_real, b.m_real );
}

HK_FORCE_INLINE void hkSimdFloat32::setAbs(  hkSimdFloat32Parameter a )
{
	m_real = vabs_f32(a.m_real);
}

HK_FORCE_INLINE void hkSimdFloat32::setFloor(  hkSimdFloat32Parameter a )
{
	setFromFloat( hkMath::floor(a.getReal()) );
}

HK_FORCE_INLINE void hkSimdFloat32::setMod(  hkSimdFloat32Parameter a, hkSimdFloat32Parameter b )
{
	setFromFloat( hkMath::fmod(a.getReal(),b.getReal()) );
}

HK_FORCE_INLINE void hkSimdFloat32::setFlipSign(hkSimdFloat32Parameter v, hkSimdFloat32Parameter sSign)
{
	static const uint32x2_t allsigns = HK_NEON_CONSTANT2I( 0x80000000, 0x80000000 );
	uint32x2_t signmask = vand_u32( vreinterpret_u32_f32(sSign.m_real), allsigns );
	m_real = vreinterpret_f32_u32( veor_u32( vreinterpret_u32_f32(v.m_real), signmask ) );
}

HK_FORCE_INLINE void hkSimdFloat32::setFlipSign(hkSimdFloat32Parameter v, hkVector4fComparisonParameter mask)
{
	HK_MATH_ASSERT(0x5831c04, mask.allAreSet() || (mask.getMask() == hkVector4ComparisonMask::MASK_NONE), "illegal compare mask");
	static const uint32x2_t allsigns = HK_NEON_CONSTANT2I( 0x80000000, 0x80000000 );
	uint32x2_t signmask = vand_u32( vget_low_u32(mask.m_mask), allsigns );
	m_real = vreinterpret_f32_u32( veor_u32( vreinterpret_u32_f32(v.m_real), signmask ) );
}

//
// advanced interface
//

namespace hkSimdFloat32_AdvancedInterface
{

template <hkMathAccuracyMode A, hkMathDivByZeroMode D>
struct unroll_setReciprocal { HK_FORCE_INLINE static void apply(hkSingleFloat32& self, hkSimdFloat32Parameter a)
{
	HK_SIMDFLOAT_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_IGNORE> { HK_FORCE_INLINE static void apply(hkSingleFloat32& self, hkSimdFloat32Parameter a)
{
	switch (A)
	{
		case HK_ACC_12_BIT: 
			{
				hkSingleFloat32 r0 = vrecpe_f32( a.m_real ); // estimate is only 8Bit
				self = vmul_f32( r0, vrecps_f32(  a.m_real, r0 )); // so do a pass1
			}
			break;
		default:
			{
				hkSingleFloat32 r0 = vrecpe_f32( a.m_real ); // estimate is only 8Bit
				hkSingleFloat32 r1 = vmul_f32( r0, vrecps_f32( a.m_real, r0 )); // pass1: 15Bit
				self = vmul_f32( r1, vrecps_f32(  a.m_real, r1 )); // pass2: 30Bit (in theory)
			}
			break; // HK_ACC_FULL and HK_ACC_23_BIT
	}
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_ZERO> { HK_FORCE_INLINE static void apply(hkSingleFloat32& self, hkSimdFloat32Parameter a)
{
	uint32x2_t equalsZero = vceq_f32(a.m_real, vcreate_f32(0));  // todo how to use VCEQ #0
	hkSingleFloat32 e; unroll_setReciprocal<A, HK_DIV_IGNORE>::apply(e, a);
	self = vreinterpret_f32_u32( vbic_u32( vreinterpret_u32_f32(e), equalsZero )); 		
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_HIGH> { HK_FORCE_INLINE static void apply(hkSingleFloat32& self, hkSimdFloat32Parameter a)
{
	hkSingleFloat32 huge = vdup_n_f32( HK_FLOAT_HIGH );
	uint32x2_t equalsZero = vceq_f32(a.m_real, vcreate_f32(0));  // todo how to use VCEQ #0
	hkSingleFloat32 e; unroll_setReciprocal<A, HK_DIV_IGNORE>::apply(e, a);
	self = vbsl_f32( equalsZero, huge, e );
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_MAX> { HK_FORCE_INLINE static void apply(hkSingleFloat32& self, hkSimdFloat32Parameter a)
{
	hkSingleFloat32 huge = vdup_n_f32( HK_FLOAT_MAX );
	uint32x2_t equalsZero = vceq_f32(a.m_real, vcreate_f32(0));  // todo how to use VCEQ #0
	hkSingleFloat32 e; unroll_setReciprocal<A, HK_DIV_IGNORE>::apply(e, a);
	self = vbsl_f32( equalsZero, huge, e );
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_ZERO_AND_ONE> { HK_FORCE_INLINE static void apply(hkSingleFloat32& self, hkSimdFloat32Parameter a)
{
	unroll_setReciprocal<A, HK_DIV_SET_ZERO>::apply(self, a);
	hkSingleFloat32 one = vdup_n_f32( 1.0f );
	hkSingleFloat32 eps = vdup_n_f32( HK_FLOAT_EPSILON );
	hkSingleFloat32 absVal = vabs_f32(vsub_f32(self, one));
	uint32x2_t lessEqualEps = vcle_f32(absVal, eps);
	self = vbsl_f32( lessEqualEps, one, self );
} };

} // namespace 

template <hkMathAccuracyMode A, hkMathDivByZeroMode D> 
HK_FORCE_INLINE void hkSimdFloat32::setReciprocal(hkSimdFloat32Parameter a)
{
	hkSimdFloat32_AdvancedInterface::unroll_setReciprocal<A,D>::apply(m_real,a);
}

HK_FORCE_INLINE void hkSimdFloat32::setReciprocal(hkSimdFloat32Parameter a)
{
	hkSimdFloat32_AdvancedInterface::unroll_setReciprocal<HK_ACC_23_BIT,HK_DIV_IGNORE>::apply(m_real,a);
}



namespace hkSimdFloat32_AdvancedInterface
{

template <hkMathAccuracyMode A, hkMathDivByZeroMode D>
struct unroll_setDiv { HK_FORCE_INLINE static void apply(hkSingleFloat32& self, hkSimdFloat32Parameter a, hkSimdFloat32Parameter b)
{
	HK_SIMDFLOAT_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <hkMathAccuracyMode A>
struct unroll_setDiv<A, HK_DIV_IGNORE> { HK_FORCE_INLINE static void apply(hkSingleFloat32& self, hkSimdFloat32Parameter a, hkSimdFloat32Parameter b)
{
	hkSingleFloat32 t;
	unroll_setReciprocal<A,HK_DIV_IGNORE>::apply(t,b);
	self = vmul_f32(a.m_real,t);
} };
template <hkMathAccuracyMode A>
struct unroll_setDiv<A, HK_DIV_SET_ZERO> { HK_FORCE_INLINE static void apply(hkSingleFloat32& self, hkSimdFloat32Parameter a, hkSimdFloat32Parameter b)
{
	hkSingleFloat32 t;
	unroll_setReciprocal<A,HK_DIV_SET_ZERO>::apply(t,b);
	self = vmul_f32(a.m_real,t);
} };
template <hkMathAccuracyMode A>
struct unroll_setDiv<A, HK_DIV_SET_HIGH> { HK_FORCE_INLINE static void apply(hkSingleFloat32& self, hkSimdFloat32Parameter a, hkSimdFloat32Parameter b)
{
	static const uint32x2_t allsigns = HK_NEON_CONSTANT2I( 0x80000000, 0x80000000 );
	hkSingleFloat32 huge = vdup_n_f32( HK_FLOAT_HIGH );
	uint32x2_t equalsZero = vceq_f32(b.m_real, vcreate_f32(0));  // todo how to use VCEQ #0
	hkSingleFloat32 e; unroll_setDiv<A, HK_DIV_IGNORE>::apply(e,a,b);
	uint32x2_t signmask = vand_u32( vreinterpret_u32_f32(a.m_real), allsigns );
	huge = vreinterpret_f32_u32( veor_u32( vreinterpret_u32_f32(huge), signmask ) );
	self = vbsl_f32( equalsZero, huge, e );
} };
template <hkMathAccuracyMode A>
struct unroll_setDiv<A, HK_DIV_SET_MAX> { HK_FORCE_INLINE static void apply(hkSingleFloat32& self, hkSimdFloat32Parameter a, hkSimdFloat32Parameter b)
{
	static const uint32x2_t allsigns = HK_NEON_CONSTANT2I( 0x80000000, 0x80000000 );
	hkSingleFloat32 huge = vdup_n_f32( HK_FLOAT_MAX );
	uint32x2_t equalsZero = vceq_f32(b.m_real, vcreate_f32(0));  // todo how to use VCEQ #0
	hkSingleFloat32 e; unroll_setDiv<A, HK_DIV_IGNORE>::apply(e,a,b);
	uint32x2_t signmask = vand_u32( vreinterpret_u32_f32(a.m_real), allsigns );
	huge = vreinterpret_f32_u32( veor_u32( vreinterpret_u32_f32(huge), signmask ) );
	self = vbsl_f32( equalsZero, huge, e );
} };
template <hkMathAccuracyMode A>
struct unroll_setDiv<A, HK_DIV_SET_ZERO_AND_ONE> { HK_FORCE_INLINE static void apply(hkSingleFloat32& self, hkSimdFloat32Parameter a, hkSimdFloat32Parameter b)
{
	unroll_setDiv<A, HK_DIV_SET_ZERO>::apply(self, a, b);
	hkSingleFloat32 one = vdup_n_f32( 1.0f );
	hkSingleFloat32 eps = vdup_n_f32( HK_FLOAT_EPSILON );
	hkSingleFloat32 absVal = vabs_f32(vsub_f32(self, one));
	uint32x2_t lessEqualEps = vcle_f32(absVal, eps);
	self = vbsl_f32( lessEqualEps, one, self );
} };

} // namespace 

template <hkMathAccuracyMode A, hkMathDivByZeroMode D> 
HK_FORCE_INLINE void hkSimdFloat32::setDiv(hkSimdFloat32Parameter a, hkSimdFloat32Parameter b)
{
	hkSimdFloat32_AdvancedInterface::unroll_setDiv<A,D>::apply(m_real,a,b);
}

HK_FORCE_INLINE void hkSimdFloat32::setDiv(hkSimdFloat32Parameter a, hkSimdFloat32Parameter b)
{
	hkSimdFloat32_AdvancedInterface::unroll_setDiv<HK_ACC_23_BIT,HK_DIV_IGNORE>::apply(m_real,a,b);
}

template <hkMathAccuracyMode A, hkMathDivByZeroMode D> 
HK_FORCE_INLINE void hkSimdFloat32::div(hkSimdFloat32Parameter a)
{
	setDiv<A,D>( *this, a );
}

HK_FORCE_INLINE void hkSimdFloat32::div(hkSimdFloat32Parameter a)
{
	setDiv( *this, a );
}

HK_FORCE_INLINE const hkSimdFloat32 hkSimdFloat32::operator/ (hkSimdFloat32Parameter r) const
{
	hkSimdFloat32 q;
	q.setDiv(*this,r);
	return q;
}



namespace hkSimdFloat32_AdvancedInterface
{

template <hkMathAccuracyMode A, hkMathNegSqrtMode S>
struct unroll_sqrtInverse { HK_FORCE_INLINE static hkSingleFloat32 apply(hkSimdFloat32Parameter self)
{
	HK_SIMDFLOAT_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
	#ifdef HK_PLATFORM_ANDROID
		return self; // silence warning
	#endif
} };
template <hkMathAccuracyMode A>
struct unroll_sqrtInverse<A, HK_SQRT_IGNORE> { HK_FORCE_INLINE static hkSingleFloat32 apply(hkSimdFloat32Parameter self)
{
	switch (A)
	{
		case HK_ACC_12_BIT: 
			{
				hkSingleFloat32 r0 = vrsqrte_f32( self.m_real ); // estimate is only 8Bit
				hkSingleFloat32 e0 = vmul_f32(r0, r0);
				return vmul_f32( vrsqrts_f32( self.m_real, e0 ), r0 ); // so do a pass1
			}
		default:
			{
				hkSingleFloat32 r0 = vrsqrte_f32( self.m_real ); // estimate is only 8Bit
				hkSingleFloat32 e0 = vmul_f32(r0, r0);
				hkSingleFloat32 r1 = vmul_f32( vrsqrts_f32( self.m_real, e0 ), r0 ); // pass1: 15bit
				hkSingleFloat32 e1 = vmul_f32(r1, r1);		
				return vmul_f32( vrsqrts_f32( self.m_real, e1 ), r1 ); // pass2: 30bit
			}
			// HK_ACC_FULL and HK_ACC_23_BIT
	}
} };
template <hkMathAccuracyMode A>
struct unroll_sqrtInverse<A, HK_SQRT_SET_ZERO> { HK_FORCE_INLINE static hkSingleFloat32 apply(hkSimdFloat32Parameter self)
{
	uint32x2_t equalsZero = vcle_f32(self.m_real, vcreate_f32(0));  // todo how to use VCLE #0
	const hkSingleFloat32 e = unroll_sqrtInverse<A, HK_SQRT_IGNORE>::apply(self);
	return vreinterpret_f32_u32( vbic_u32( vreinterpret_u32_f32(e), equalsZero )); 		
} };
} // namespace 

template <hkMathAccuracyMode A, hkMathNegSqrtMode S> 
HK_FORCE_INLINE const hkSimdFloat32 hkSimdFloat32::sqrtInverse() const
{
	return hkSimdFloat32::convert(hkSimdFloat32_AdvancedInterface::unroll_sqrtInverse<A,S>::apply(*this));
}

HK_FORCE_INLINE const hkSimdFloat32 hkSimdFloat32::sqrtInverse() const
{
	return hkSimdFloat32::convert(hkSimdFloat32_AdvancedInterface::unroll_sqrtInverse<HK_ACC_23_BIT,HK_SQRT_SET_ZERO>::apply(*this));
}


namespace hkSimdFloat32_AdvancedInterface
{

template <hkMathAccuracyMode A, hkMathNegSqrtMode S>
struct unroll_sqrt { HK_FORCE_INLINE static hkSingleFloat32 apply(hkSimdFloat32Parameter self)
{
	HK_SIMDFLOAT_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
	#ifdef HK_PLATFORM_ANDROID
		return self; // silence warning
	#endif
} };
template <hkMathAccuracyMode A>
struct unroll_sqrt<A, HK_SQRT_IGNORE> { HK_FORCE_INLINE static hkSingleFloat32 apply(hkSimdFloat32Parameter self)
{
	// Neon does not have a sqrt instruction
	hkSingleFloat32 inv = unroll_sqrtInverse<A,HK_SQRT_IGNORE>::apply(self);
	return vmul_f32(self.m_real, inv);
} };
template <hkMathAccuracyMode A>
struct unroll_sqrt<A, HK_SQRT_SET_ZERO> { HK_FORCE_INLINE static hkSingleFloat32 apply(hkSimdFloat32Parameter self)
{
	// Neon does not have a sqrt instruction
	uint32x2_t equalsZero = vcle_f32(self.m_real, vcreate_f32(0));  // todo how to use VCLE #0
	hkSingleFloat32 e = unroll_sqrtInverse<A, HK_SQRT_IGNORE>::apply(self);
	// Neon mul does not normalize the sign -> premul to clear sign
	e = vmul_f32(self.m_real, e);
	return vreinterpret_f32_u32( vbic_u32( vreinterpret_u32_f32(e), equalsZero )); 		
} };

} // namespace 

template <hkMathAccuracyMode A, hkMathNegSqrtMode S> 
HK_FORCE_INLINE const hkSimdFloat32 hkSimdFloat32::sqrt() const
{
	return hkSimdFloat32::convert(hkSimdFloat32_AdvancedInterface::unroll_sqrt<A,S>::apply(*this));
}

HK_FORCE_INLINE const hkSimdFloat32 hkSimdFloat32::sqrt() const
{
	return hkSimdFloat32::convert(hkSimdFloat32_AdvancedInterface::unroll_sqrt<HK_ACC_23_BIT,HK_SQRT_SET_ZERO>::apply(*this));
}



namespace hkSimdFloat32_AdvancedInterface
{
template <hkMathIoMode A>
struct unroll_load { HK_FORCE_INLINE static void apply(hkSingleFloat32& self, const hkFloat32* HK_RESTRICT p)
{
	HK_SIMDFLOAT_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <>
struct unroll_load<HK_IO_NATIVE_ALIGNED> { HK_FORCE_INLINE static void apply(hkSingleFloat32& self, const hkFloat32* HK_RESTRICT p)
{
	// neon does not align HK_MATH_ASSERT(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkFloat32)-1) ) == 0, "pointer must be aligned to native size of hkFloat32.");
	self = vld1_dup_f32( (float32_t const *) p);
} };
template <>
struct unroll_load<HK_IO_SIMD_ALIGNED> { HK_FORCE_INLINE static void apply(hkSingleFloat32& self, const hkFloat32* HK_RESTRICT p)
{
	unroll_load<HK_IO_NATIVE_ALIGNED>::apply(self,p);
} };
template <>
struct unroll_load<HK_IO_NOT_CACHED> { HK_FORCE_INLINE static void apply(hkSingleFloat32& self, const hkFloat32* HK_RESTRICT p)
{
	unroll_load<HK_IO_SIMD_ALIGNED>::apply(self,p);
} };
template <hkMathIoMode A>
struct unroll_load_D { HK_FORCE_INLINE static void apply(hkSingleFloat32& self, const hkDouble64* HK_RESTRICT p)
{
	// neon does not align HK_MATH_ASSERT(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkDouble64)-1) ) == 0, "pointer must be aligned to native size of hkDouble64.");
	hkFloat32 f = hkFloat32(p[0]);
	self = vld1_dup_f32( (float32_t const *) &f);
} };
} // namespace 

template <int N, hkMathIoMode A>
HK_FORCE_INLINE void hkSimdFloat32::load(const hkFloat32 *p )
{
	HK_SIMDFLOAT_DIMENSION_CHECK;
	hkSimdFloat32_AdvancedInterface::unroll_load<A>::apply(m_real, p);
}

template <int N, hkMathIoMode A>
HK_FORCE_INLINE void hkSimdFloat32::load(const hkDouble64 *p )
{
	HK_SIMDFLOAT_DIMENSION_CHECK;
	hkSimdFloat32_AdvancedInterface::unroll_load_D<A>::apply(m_real, p);
}

template <int N>
HK_FORCE_INLINE void hkSimdFloat32::load(const hkFloat32 *p )
{
	HK_SIMDFLOAT_DIMENSION_CHECK;
	hkSimdFloat32_AdvancedInterface::unroll_load<HK_IO_SIMD_ALIGNED>::apply(m_real, p);
}

template <int N>
HK_FORCE_INLINE void hkSimdFloat32::load(const hkDouble64 *p )
{
	HK_SIMDFLOAT_DIMENSION_CHECK;
	hkSimdFloat32_AdvancedInterface::unroll_load_D<HK_IO_SIMD_ALIGNED>::apply(m_real, p);
}


namespace hkSimdFloat32_AdvancedInterface
{
template <hkMathIoMode A>
struct unroll_loadH { HK_FORCE_INLINE static void apply(hkSingleFloat32& self, const hkHalf* HK_RESTRICT p)
{
	HK_SIMDFLOAT_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <>
struct unroll_loadH<HK_IO_NATIVE_ALIGNED> { HK_FORCE_INLINE static void apply(hkSingleFloat32& self, const hkHalf* HK_RESTRICT p)
{
	// neon does not align HK_MATH_ASSERT(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkHalf)-1) ) == 0, "pointer must be aligned to native size of hkHalf.");
	hkFloat32 f = hkFloat32(p[0].getReal());
	self = vld1_dup_f32( (float32_t const *) &f);
} };
template <>
struct unroll_loadH<HK_IO_SIMD_ALIGNED> { HK_FORCE_INLINE static void apply(hkSingleFloat32& self, const hkHalf* HK_RESTRICT p)
{
	unroll_loadH<HK_IO_NATIVE_ALIGNED>::apply(self,p);
} };
template <>
struct unroll_loadH<HK_IO_NOT_CACHED> { HK_FORCE_INLINE static void apply(hkSingleFloat32& self, const hkHalf* HK_RESTRICT p)
{
	unroll_loadH<HK_IO_SIMD_ALIGNED>::apply(self,p);
} };
} // namespace 

template <int N, hkMathIoMode A>
HK_FORCE_INLINE void hkSimdFloat32::load(const hkHalf *p )
{
	HK_SIMDFLOAT_DIMENSION_CHECK;
	hkSimdFloat32_AdvancedInterface::unroll_loadH<A>::apply(m_real, p);
}

template <int N>
HK_FORCE_INLINE void hkSimdFloat32::load(const hkHalf *p )
{
	HK_SIMDFLOAT_DIMENSION_CHECK;
	hkSimdFloat32_AdvancedInterface::unroll_loadH<HK_IO_SIMD_ALIGNED>::apply(m_real, p);
}


namespace hkSimdFloat32_AdvancedInterface
{
template <hkMathIoMode A>
struct unroll_loadF16 { HK_FORCE_INLINE static void apply(hkSingleFloat32& self, const hkFloat16* HK_RESTRICT p)
{
	HK_SIMDFLOAT_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <>
struct unroll_loadF16<HK_IO_NATIVE_ALIGNED> { HK_FORCE_INLINE static void apply(hkSingleFloat32& self, const hkFloat16* HK_RESTRICT p)
{
	// neon does not align HK_MATH_ASSERT(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkFloat16)-1) ) == 0, "pointer must be aligned to native size of hkFloat16.");
	hkFloat32 f = hkFloat32(p[0].getReal());
	self = vld1_dup_f32( (float32_t const *) &f);
} };
template <>
struct unroll_loadF16<HK_IO_SIMD_ALIGNED> { HK_FORCE_INLINE static void apply(hkSingleFloat32& self, const hkFloat16* HK_RESTRICT p)
{
	unroll_loadF16<HK_IO_NATIVE_ALIGNED>::apply(self,p);
} };
template <>
struct unroll_loadF16<HK_IO_NOT_CACHED> { HK_FORCE_INLINE static void apply(hkSingleFloat32& self, const hkFloat16* HK_RESTRICT p)
{
	unroll_loadF16<HK_IO_SIMD_ALIGNED>::apply(self,p);
} };
} // namespace 

template <int N, hkMathIoMode A>
HK_FORCE_INLINE void hkSimdFloat32::load(const hkFloat16 *p )
{
	HK_SIMDFLOAT_DIMENSION_CHECK;
	hkSimdFloat32_AdvancedInterface::unroll_loadF16<A>::apply(m_real, p);
}

template <int N>
HK_FORCE_INLINE void hkSimdFloat32::load(const hkFloat16 *p )
{
	HK_SIMDFLOAT_DIMENSION_CHECK;
	hkSimdFloat32_AdvancedInterface::unroll_loadF16<HK_IO_SIMD_ALIGNED>::apply(m_real, p);
}


namespace hkSimdFloat32_AdvancedInterface
{
template <hkMathIoMode A>
struct unroll_store { HK_FORCE_INLINE static void apply(const hkSingleFloat32& self, hkFloat32* HK_RESTRICT p)
{
	HK_SIMDFLOAT_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <>
struct unroll_store<HK_IO_NATIVE_ALIGNED> { HK_FORCE_INLINE static void apply(const hkSingleFloat32& self, hkFloat32* HK_RESTRICT p)
{
	// neon does not align HK_MATH_ASSERT(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkFloat32)-1) ) == 0, "pointer must be aligned to native size of hkFloat32.");
	vst1_lane_f32( (float32_t*) p, self, 0 );
} };
template <>
struct unroll_store<HK_IO_SIMD_ALIGNED> { HK_FORCE_INLINE static void apply(const hkSingleFloat32& self, hkFloat32* HK_RESTRICT p)
{
	unroll_store<HK_IO_NATIVE_ALIGNED>::apply(self,p);
} };
template <>
struct unroll_store<HK_IO_NOT_CACHED> { HK_FORCE_INLINE static void apply(const hkSingleFloat32& self, hkFloat32* HK_RESTRICT p)
{
	unroll_store<HK_IO_SIMD_ALIGNED>::apply(self,p);
} };
template <hkMathIoMode A>
struct unroll_store_D { HK_FORCE_INLINE static void apply(const hkSingleFloat32& self, hkDouble64* HK_RESTRICT p)
{
	// neon does not align HK_MATH_ASSERT(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkDouble64)-1) ) == 0, "pointer must be aligned to native size of hkDouble64.");
	hkFloat32 f;
	vst1_lane_f32( (float32_t*) &f, self, 0 );
	p[0] = hkDouble64(f);
} };
} // namespace 

template <int N, hkMathIoMode A, hkMathRoundingMode R>
HK_FORCE_INLINE void hkSimdFloat32::store(  hkFloat32 *p ) const
{
	HK_SIMDFLOAT_DIMENSION_CHECK;
	hkSimdFloat32_AdvancedInterface::unroll_store<A>::apply(m_real, p);
}

template <int N, hkMathIoMode A, hkMathRoundingMode R>
HK_FORCE_INLINE void hkSimdFloat32::store(  hkDouble64 *p ) const
{
	HK_SIMDFLOAT_DIMENSION_CHECK;
	hkSimdFloat32_AdvancedInterface::unroll_store_D<A>::apply(m_real, p);
}

template <int N, hkMathIoMode A>
HK_FORCE_INLINE void hkSimdFloat32::store(  hkFloat32 *p ) const
{
	HK_SIMDFLOAT_DIMENSION_CHECK;
	hkSimdFloat32_AdvancedInterface::unroll_store<A>::apply(m_real, p);
}

template <int N, hkMathIoMode A>
HK_FORCE_INLINE void hkSimdFloat32::store(  hkDouble64 *p ) const
{
	HK_SIMDFLOAT_DIMENSION_CHECK;
	hkSimdFloat32_AdvancedInterface::unroll_store_D<A>::apply(m_real, p);
}

template <int N>
HK_FORCE_INLINE void hkSimdFloat32::store(  hkFloat32 *p ) const
{
	HK_SIMDFLOAT_DIMENSION_CHECK;
	hkSimdFloat32_AdvancedInterface::unroll_store<HK_IO_SIMD_ALIGNED>::apply(m_real, p);
}

template <int N>
HK_FORCE_INLINE void hkSimdFloat32::store(  hkDouble64 *p ) const
{
	HK_SIMDFLOAT_DIMENSION_CHECK;
	hkSimdFloat32_AdvancedInterface::unroll_store_D<HK_IO_SIMD_ALIGNED>::apply(m_real, p);
}



namespace hkSimdFloat32_AdvancedInterface
{
template <hkMathIoMode A, hkMathRoundingMode R>
struct unroll_storeH { HK_FORCE_INLINE static void apply(const hkSingleFloat32& self, hkHalf* HK_RESTRICT p)
{
	HK_SIMDFLOAT_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <hkMathRoundingMode R>
struct unroll_storeH<HK_IO_NATIVE_ALIGNED,R> { HK_FORCE_INLINE static void apply(const hkSingleFloat32& self, hkHalf* HK_RESTRICT p)
{
	// neon does not align HK_MATH_ASSERT(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkHalf)-1) ) == 0, "pointer must be aligned to native size of hkHalf.");
	uint16x4_t r;
	if (R == HK_ROUND_NEAREST)
	{
		hkSingleFloat32 vs0 = vmul_f32( self, vget_low_f32(g_vectorfConstants[HK_QUADREAL_PACK_HALF]) );
		uint32x2_t vsi = vreinterpret_u32_f32(vs0);
		uint32x4_t vsiq = vcombine_u32(vsi,vsi);
		r = vshrn_n_u32(vsiq, 16);
	}
	else
	{
		uint32x2_t vsi = vreinterpret_u32_f32(self);
		uint32x4_t vsiq = vcombine_u32(vsi,vsi);
		r = vshrn_n_u32(vsiq, 16);
	}
	vst1_lane_u16( (hkUint16*)p, r, 0 );
} };
template <hkMathRoundingMode R>
struct unroll_storeH<HK_IO_SIMD_ALIGNED,R> { HK_FORCE_INLINE static void apply(const hkSingleFloat32& self, hkHalf* HK_RESTRICT p)
{
	unroll_storeH<HK_IO_NATIVE_ALIGNED,R>::apply(self,p);
} };
template <hkMathRoundingMode R>
struct unroll_storeH<HK_IO_NOT_CACHED,R> { HK_FORCE_INLINE static void apply(const hkSingleFloat32& self, hkHalf* HK_RESTRICT p)
{
	unroll_storeH<HK_IO_SIMD_ALIGNED,R>::apply(self,p);
} };
} // namespace 

template <int N, hkMathIoMode A, hkMathRoundingMode R>
HK_FORCE_INLINE void hkSimdFloat32::store(  hkHalf *p ) const
{
	HK_SIMDFLOAT_DIMENSION_CHECK;
	hkSimdFloat32_AdvancedInterface::unroll_storeH<A,R>::apply(m_real, p);
}

template <int N>
HK_FORCE_INLINE void hkSimdFloat32::store(  hkHalf *p ) const
{
	HK_SIMDFLOAT_DIMENSION_CHECK;
	hkSimdFloat32_AdvancedInterface::unroll_storeH<HK_IO_SIMD_ALIGNED,HK_ROUND_DEFAULT>::apply(m_real, p);
}


namespace hkSimdFloat32_AdvancedInterface
{
template <hkMathIoMode A, hkMathRoundingMode R>
struct unroll_storeF16 { HK_FORCE_INLINE static void apply(const hkSingleFloat32& self, hkFloat16* HK_RESTRICT p)
{
	HK_SIMDFLOAT_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <hkMathRoundingMode R>
struct unroll_storeF16<HK_IO_NATIVE_ALIGNED,R> { HK_FORCE_INLINE static void apply(const hkSingleFloat32& self, hkFloat16* HK_RESTRICT p)
{
	// neon does not align HK_MATH_ASSERT(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkFloat16)-1) ) == 0, "pointer must be aligned to native size of hkFloat16.");
	hkFloat32 f;
	vst1_lane_f32( (float32_t*)&f, self, 0 );
	p[0].setReal<(R == HK_ROUND_NEAREST)>(f);
} };
template <hkMathRoundingMode R>
struct unroll_storeF16<HK_IO_SIMD_ALIGNED,R> { HK_FORCE_INLINE static void apply(const hkSingleFloat32& self, hkFloat16* HK_RESTRICT p)
{
	unroll_storeF16<HK_IO_NATIVE_ALIGNED,R>::apply(self,p);
} };
template <hkMathRoundingMode R>
struct unroll_storeF16<HK_IO_NOT_CACHED,R> { HK_FORCE_INLINE static void apply(const hkSingleFloat32& self, hkFloat16* HK_RESTRICT p)
{
	unroll_storeF16<HK_IO_SIMD_ALIGNED,R>::apply(self,p);
} };
} // namespace 

template <int N, hkMathIoMode A, hkMathRoundingMode R>
HK_FORCE_INLINE void hkSimdFloat32::store(  hkFloat16 *p ) const
{
	HK_SIMDFLOAT_DIMENSION_CHECK;
	hkSimdFloat32_AdvancedInterface::unroll_storeF16<A,R>::apply(m_real, p);
}

template <int N>
HK_FORCE_INLINE void hkSimdFloat32::store(  hkFloat16 *p ) const
{
	HK_SIMDFLOAT_DIMENSION_CHECK;
	hkSimdFloat32_AdvancedInterface::unroll_storeF16<HK_IO_SIMD_ALIGNED,HK_ROUND_DEFAULT>::apply(m_real, p);
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
