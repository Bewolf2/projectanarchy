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

/* quad, here for inlining */

#ifndef HK_DISABLE_MATH_CONSTRUCTORS
/* construct, assign, zero */
HK_FORCE_INLINE hkVector4f::hkVector4f(hkFloat32 a, hkFloat32 b, hkFloat32 c, hkFloat32 d)
{
	float32x2_t l = HK_NEON_CONSTANT2F( a, b );
	float32x2_t h = HK_NEON_CONSTANT2F( c, d );
	m_quad = vcombine_f32(l,h);
}

HK_FORCE_INLINE hkVector4f::hkVector4f(const hkQuadFloat32& q)
{
	m_quad = q;
}

HK_FORCE_INLINE hkVector4f::hkVector4f(const hkVector4f& v)
{
	m_quad = v.m_quad;
}
#endif

HK_FORCE_INLINE void hkVector4f::set(hkFloat32 a, hkFloat32 b, hkFloat32 c, hkFloat32 d)
{
	const float32_t temp[4] = { a, b, c, d };
	float32x2_t l = vld1_f32( temp );
	float32x2_t h = vld1_f32( temp + 2 );
	m_quad = vcombine_f32(l,h);
}

HK_FORCE_INLINE void hkVector4f::set( hkSimdFloat32Parameter a, hkSimdFloat32Parameter b, hkSimdFloat32Parameter c, hkSimdFloat32Parameter d )
{
	float32x2_t l = vset_lane_f32( vget_lane_f32(b.m_real,0), a.m_real, 1);
	float32x2_t h = vset_lane_f32( vget_lane_f32(d.m_real,0), c.m_real, 1);
	m_quad = vcombine_f32(l,h);
}

HK_FORCE_INLINE void hkVector4f::setAll(const hkFloat32& a)
{
	m_quad = vdupq_n_f32( a );
}

HK_FORCE_INLINE void hkVector4f::setAll(hkSimdFloat32Parameter a)
{
	m_quad = vcombine_f32(a.m_real,a.m_real);
}

HK_FORCE_INLINE void hkVector4f::setZero()
{
	float32x2_t z = vcreate_f32(0); 
	m_quad = vcombine_f32(z,z);
}

template <int I> 
HK_FORCE_INLINE void hkVector4f::zeroComponent()
{
	HK_VECTOR4f_SUBINDEX_CHECK;
    // Add Switch to fix Clang issue were last parameter of vsetq_lane_f32 must be in constrange(0,3)
    // See RSYS-1377 for more information
    switch(I)
    {
        case 1:  m_quad = vsetq_lane_f32( 0, m_quad, 1); break;
        case 2:  m_quad = vsetq_lane_f32( 0, m_quad, 2); break;
        case 3:  m_quad = vsetq_lane_f32( 0, m_quad, 3); break;
        default: m_quad = vsetq_lane_f32( 0, m_quad, 0); break;
    }
}

HK_FORCE_INLINE void hkVector4f::zeroComponent(const int i)
{
	HK_MATH_ASSERT(0x3bc36625, (i>=0) && (i<4), "Component index out of range");
	switch (i) 
	{
		case 1:  m_quad = vsetq_lane_f32( 0, m_quad, 1); break;
		case 2:  m_quad = vsetq_lane_f32( 0, m_quad, 2); break;
		case 3:  m_quad = vsetq_lane_f32( 0, m_quad, 3); break;
		default: m_quad = vsetq_lane_f32( 0, m_quad, 0); break;
	}
}

HK_FORCE_INLINE void hkVector4f::setAdd(hkVector4fParameter v0, hkVector4fParameter v1)
{
	m_quad = vaddq_f32( v0.m_quad, v1.m_quad );
}

HK_FORCE_INLINE void hkVector4f::setSub(hkVector4fParameter v0, hkVector4fParameter v1)
{
	m_quad = vsubq_f32( v0.m_quad, v1.m_quad );
}

HK_FORCE_INLINE void hkVector4f::setMul(hkVector4fParameter v0, hkVector4fParameter v1)
{
	m_quad = vmulq_f32( v0.m_quad, v1.m_quad );
}

HK_FORCE_INLINE void hkVector4f::setMul(hkVector4fParameter v1, hkSimdFloat32Parameter r)
{
	m_quad = vmulq_f32( vcombine_f32(r.m_real,r.m_real), v1.m_quad );
}

HK_FORCE_INLINE void hkVector4f::setSubMul(hkVector4fParameter a, hkVector4fParameter b, hkSimdFloat32Parameter r)
{
	m_quad = vmlsq_f32(a.m_quad, vcombine_f32(r.m_real,r.m_real), b.m_quad ); 
}

HK_FORCE_INLINE void hkVector4f::setAddMul(hkVector4fParameter a, hkVector4fParameter b, hkSimdFloat32Parameter r)
{
	m_quad = vmlaq_f32(a.m_quad, vcombine_f32(r.m_real,r.m_real), b.m_quad ); 
}

HK_FORCE_INLINE void hkVector4f::setAddMul(hkVector4fParameter a, hkVector4fParameter x, hkVector4fParameter y)
{
	m_quad = vmlaq_f32(a.m_quad, y.m_quad, x.m_quad ); 
}

HK_FORCE_INLINE void hkVector4f::setSubMul(hkVector4fParameter a, hkVector4fParameter x, hkVector4fParameter y)
{
	m_quad = vmlsq_f32(a.m_quad, y.m_quad, x.m_quad ); 
}

HK_FORCE_INLINE void hkVector4f::setCross( hkVector4fParameter v0, hkVector4fParameter v1 )
{
#if 1 // vrev / vext version

	const float32x4_t v0_yzwx = vextq_f32( v0.m_quad, v0.m_quad, 1);
	const float32x4_t v1_yzwx = vextq_f32( v1.m_quad, v1.m_quad, 1);
	const float32x2_t v0_yz = vget_low_f32( v0_yzwx );
	const float32x2_t v1_yz = vget_low_f32( v1_yzwx );
	const float32x2_t v0_wx = vget_high_f32( v0_yzwx );
	const float32x2_t v1_wx = vget_high_f32( v1_yzwx );
	const float32x2_t v0_xw = vrev64_f32( v0_wx );
	const float32x2_t v1_xw = vrev64_f32( v1_wx );
	const float32x4_t v0_yzxw = vcombine_f32( v0_yz, v0_xw );
	const float32x4_t v1_yzxw = vcombine_f32( v1_yz, v1_xw );
	const float32x4_t cross = vmlsq_f32(vmulq_f32( v0.m_quad, v1_yzxw ), v0_yzxw, v1.m_quad );
	const float32x4_t cross_yzwx = vextq_f32( cross, cross, 1 );
	const float32x2_t c_xy = vget_low_f32( cross );
	const float32x2_t c_yz = vget_low_f32( cross_yzwx );
	m_quad = vcombine_f32( c_yz, c_xy );

#elif 0

	register float32x4_t v1 asm("q0") = v0; // not supported..
	register float32x4_t v2 asm("q2") = v1;
	register float32x4_t result asm("q8");
	asm volatile(
		"vmov    q1, q0              \n\t"
		"vmov    q3, q2              \n\t"
		"vzip.32 q0, q1              \n\t" // d0,d1,d3 = xx1, yy1, zz1
		"vzip.32 q2, q3              \n\t" // d4,d5,d6 = xx2, yy2, zz2
		"vmul.f32 d16, d1, d6        \n\t" // x' = yy1 * zz2
		"vmul.f32 d18, d3, d4        \n\t" // y' = zz1 * xx2
		"vmul.f32 d17, d0, d5        \n\t" // z' = xx1 * yy2
		"vmls.f32 d16, d3, d5        \n\t" // x' = x' - zz1 * yy2
		"vmls.f32 d18, d0, d6        \n\t" // y' = y' - xx1 * zz2
		"vmls.f32 d17, d1, d4        \n\t" // z' = z' - yy1 * xx2
		"vuzp.32 d16, d18"                 // result.xyzw = xyzz'
		: "=w" (v1), "=w" (v2), "=w" (result)
		: "0" (v1), "1" (v2)
		: "q1", "q3", "q9"
		);

	return result;

#else

	// Zip/unzipo version (see http://www.gp32x.com/board/index.php?/topic/55455-fast-neon-3-term-cross-product/)
	// Slower than vrev/vext ver in our usage anyway
	float32x4x2_t xxyyzz0 = vzipq_f32(HK_NEON_V(v0.m_quad), HK_NEON_V(v0.m_quad));
	float32x4x2_t xxyyzz1 = vzipq_f32(HK_NEON_V(v1.m_quad), HK_NEON_V(v1.m_quad));    

	float32x2_t xx0 = vget_low_f32(xxyyzz0.val[0]);
	float32x2_t yy0 = vget_high_f32(xxyyzz0.val[0]);    
	float32x2_t zz0 = vget_low_f32(xxyyzz0.val[1]);

	float32x2_t xx1 = vget_low_f32(xxyyzz1.val[0]);
	float32x2_t yy1 = vget_high_f32(xxyyzz1.val[0]);    
	float32x2_t zz1 = vget_low_f32(xxyyzz1.val[1]);

	float32x2_t x = vmul_f32(yy0, zz1); // x' = v0.y * v1.z
	float32x2_t y = vmul_f32(zz0, xx1); // x' = v0.z * v1.x  
	float32x2_t z = vmul_f32(xx0, yy1); // x' = v0.x * v1.y
	x = vmls_f32(x, zz0, yy1);          // x' = x' - v0.z * v1.y
	y = vmls_f32(y, xx0, zz1);          // y' = y' - v0.x * v1.z
	z = vmls_f32(z, yy0, xx1);          // z' = z' - v0.y * v1.x

	float32x2x2_t result = vuzp_f32(x, y);
	HK_NEON_V(m_quad) = vcombine_f32(result.val[0], z);

#endif
}

HK_FORCE_INLINE const hkVector4fComparison hkVector4f::equal(hkVector4fParameter a) const
{
	return hkVector4fComparison::convert(vceqq_f32(m_quad, a.m_quad));
}

HK_FORCE_INLINE const hkVector4fComparison hkVector4f::notEqual(hkVector4fParameter a) const
{
	uint32x4_t eq = vceqq_f32(m_quad, a.m_quad);
 	return hkVector4fComparison::convert(vmvnq_u32(eq));
}

HK_FORCE_INLINE const hkVector4fComparison hkVector4f::less(hkVector4fParameter a) const
{
	return hkVector4fComparison::convert(vcltq_f32(m_quad, a.m_quad));
}

HK_FORCE_INLINE const hkVector4fComparison hkVector4f::lessEqual(hkVector4fParameter a) const
{
	return hkVector4fComparison::convert(vcleq_f32(m_quad, a.m_quad));
}

HK_FORCE_INLINE const hkVector4fComparison hkVector4f::greater(hkVector4fParameter a) const
{
 	return hkVector4fComparison::convert(vcgtq_f32(m_quad, a.m_quad));
}

HK_FORCE_INLINE const hkVector4fComparison hkVector4f::greaterEqual(hkVector4fParameter a) const
{
 	return hkVector4fComparison::convert(vcgeq_f32(m_quad, a.m_quad));
}

HK_FORCE_INLINE const hkVector4fComparison hkVector4f::lessZero() const
{
	hkVector4f zero; zero.setZero();
	return hkVector4fComparison::convert(vcltq_f32(m_quad, zero.m_quad)); // todo how to use VCLT #0
}

HK_FORCE_INLINE const hkVector4fComparison hkVector4f::lessEqualZero() const
{
	hkVector4f zero; zero.setZero();
	return hkVector4fComparison::convert(vcleq_f32(m_quad, zero.m_quad)); // todo how to use VCLE #0
}

HK_FORCE_INLINE const hkVector4fComparison hkVector4f::greaterZero() const
{
	hkVector4f zero; zero.setZero();
	return hkVector4fComparison::convert(vcgtq_f32(m_quad, zero.m_quad)); // todo how to use VCGT #0
}

HK_FORCE_INLINE const hkVector4fComparison hkVector4f::greaterEqualZero() const
{
	hkVector4f zero; zero.setZero();
	return hkVector4fComparison::convert(vcgeq_f32(m_quad, zero.m_quad)); // todo how to use VCGE #0
}

HK_FORCE_INLINE const hkVector4fComparison hkVector4f::equalZero() const
{
	hkVector4f zero; zero.setZero();
	return hkVector4fComparison::convert(vceqq_f32(m_quad, zero.m_quad)); // todo how to use VCEQ #0
}

HK_FORCE_INLINE const hkVector4fComparison hkVector4f::notEqualZero() const
{
	hkVector4f zero; zero.setZero();
	uint32x4_t eq = vceqq_f32(m_quad, zero.m_quad); // todo how to use VCEQ #0
	return hkVector4fComparison::convert(vmvnq_u32(eq));
}

HK_FORCE_INLINE void hkVector4f::setSelect( hkVector4fComparisonParameter comp, hkVector4fParameter trueValue, hkVector4fParameter falseValue )
{
	m_quad = vbslq_f32( comp.m_mask, trueValue.m_quad, falseValue.m_quad );
}

HK_FORCE_INLINE void hkVector4f::zeroIfFalse( hkVector4fComparisonParameter comp )
{
	hkVector4f zero; zero.setZero();
	setSelect( comp, *this, zero );
}

HK_FORCE_INLINE void hkVector4f::zeroIfTrue( hkVector4fComparisonParameter comp )
{
	hkVector4f zero; zero.setZero();
	setSelect( comp, zero, *this );
}


template<hkVector4fComparison::Mask M> 
HK_FORCE_INLINE void hkVector4f::setSelect( hkVector4fParameter trueValue, hkVector4fParameter falseValue )
{
	hkVector4fComparison comp; comp.set(M);
	setSelect(comp, trueValue, falseValue);
}

template <>
HK_FORCE_INLINE void hkVector4f::setNeg<1>(hkVector4fParameter v)
{
	float32x2_t xy = vget_low_f32(v.m_quad);
	m_quad = v.m_quad;
	m_quad = vsetq_lane_f32(vget_lane_f32(vneg_f32(xy),0),m_quad,0);
}

template <>
HK_FORCE_INLINE void hkVector4f::setNeg<2>(hkVector4fParameter v)
{
	float32x2_t l = vget_low_f32(v.m_quad);
	float32x2_t h = vget_high_f32(v.m_quad);
	m_quad = vcombine_f32(vneg_f32(l),h);
}

template <>
HK_FORCE_INLINE void hkVector4f::setNeg<3>(hkVector4fParameter v)
{
	float32x2_t w = vget_high_f32(v.m_quad);
	m_quad = vnegq_f32(v.m_quad);
	m_quad = vsetq_lane_f32(vget_lane_f32(w,1),m_quad,3);
}

template <>
HK_FORCE_INLINE void hkVector4f::setNeg<4>(hkVector4fParameter v)
{
	m_quad = vnegq_f32(v.m_quad);
}

template <int N>
HK_FORCE_INLINE void hkVector4f::setNeg(hkVector4fParameter v)
{
	HK_VECTOR4f_NOT_IMPLEMENTED;
}

HK_FORCE_INLINE void hkVector4f::setAbs(hkVector4fParameter v)
{
	m_quad = vabsq_f32(v.m_quad);	
}

HK_FORCE_INLINE void hkVector4f::setMin(hkVector4fParameter a, hkVector4fParameter b)
{
	m_quad = vminq_f32(a.m_quad, b.m_quad);
}

HK_FORCE_INLINE void hkVector4f::setMax(hkVector4fParameter a, hkVector4fParameter b)
{
	m_quad = vmaxq_f32(a.m_quad, b.m_quad);
}

/* matrix3, rotation, quaternion, transform */

HK_FORCE_INLINE void hkVector4f::_setRotatedDir(const hkMatrix3f& r, hkVector4fParameter b )
{
	const hkQuadFloat32 c0 = r.getColumn<0>().m_quad;
	const hkQuadFloat32 c1 = r.getColumn<1>().m_quad;
	const hkQuadFloat32 c2 = r.getColumn<2>().m_quad;

	const hkQuadFloat32 b0 = vdupq_lane_f32( vget_low_f32(b.m_quad), 0);
	const hkQuadFloat32 b1 = vdupq_lane_f32( vget_low_f32(b.m_quad), 1);
	const hkQuadFloat32 b2 = vdupq_lane_f32( vget_high_f32(b.m_quad), 0);

	const hkQuadFloat32 r0 = vmulq_f32( c0, b0 );
	const hkQuadFloat32 r1 = vmulq_f32( c1, b1 );
	const hkQuadFloat32 r2 = vmulq_f32( c2, b2 );

	m_quad = vaddq_f32( vaddq_f32( r0, r1 ), r2 );
}

HK_FORCE_INLINE void hkVector4f::_setRotatedInverseDir(const hkMatrix3f& r, hkVector4fParameter b )
{
	hkVector4f c0 = r.getColumn<0>();
	hkVector4f c1 = r.getColumn<1>();
	hkVector4f c2 = r.getColumn<2>();

	HK_TRANSPOSE3f(c0,c1,c2);

	const hkQuadFloat32 b0 = vdupq_lane_f32( vget_low_f32(b.m_quad), 0);
	const hkQuadFloat32 b1 = vdupq_lane_f32( vget_low_f32(b.m_quad), 1);
	const hkQuadFloat32 b2 = vdupq_lane_f32( vget_high_f32(b.m_quad), 0);

	const hkQuadFloat32 r0 = vmulq_f32( c0.m_quad, b0 );
	const hkQuadFloat32 r1 = vmulq_f32( c1.m_quad, b1 );
	const hkQuadFloat32 r2 = vmulq_f32( c2.m_quad, b2 );

	m_quad = vaddq_f32( vaddq_f32( r0, r1 ), r2 );
}

template <>
HK_FORCE_INLINE const hkSimdFloat32 hkVector4f::dot<2>(hkVector4fParameter a) const
{
	float32x4_t x2 = vmulq_f32( m_quad, a.m_quad );
	float32x2_t l = vget_low_f32(x2);
	float32x2_t xy = vpadd_f32(l, l); 
	return hkSimdFloat32::convert( xy );
}

template <>
HK_FORCE_INLINE const hkSimdFloat32 hkVector4f::dot<3>(hkVector4fParameter a) const
{
	float32x4_t x2 = vmulq_f32( m_quad, a.m_quad );
	float32x2_t l = vget_low_f32(x2);
	float32x2_t h = vget_high_f32(x2); h = vset_lane_f32((hkFloat32)0, h, 1);
	float32x2_t xy_zw = vpadd_f32(l, h); 
	float32x2_t xyzw = vpadd_f32(xy_zw, xy_zw); 
	return hkSimdFloat32::convert( xyzw );
}

template <>
HK_FORCE_INLINE const hkSimdFloat32 hkVector4f::dot<4>(hkVector4fParameter a) const
{
	float32x4_t x2 = vmulq_f32( m_quad, a.m_quad );
	float32x2_t l = vget_low_f32(x2);
	float32x2_t h = vget_high_f32(x2);
	float32x2_t xy_zw = vpadd_f32(l, h); // (0+1, 2+3)
	float32x2_t xyzw = vpadd_f32(xy_zw, xy_zw); // 0+1+2+3
	return hkSimdFloat32::convert(xyzw);
}

template <int N>
HK_FORCE_INLINE const hkSimdFloat32 hkVector4f::dot(hkVector4fParameter a) const
{
	HK_VECTOR4f_NOT_IMPLEMENTED;
	return hkSimdFloat32::getConstant<HK_QUADREAL_0>();
}

HK_FORCE_INLINE const hkSimdFloat32 hkVector4f::dot4xyz1(hkVector4fParameter a) const
{
	float32x4_t x2 = vmulq_f32( m_quad, a.m_quad );
	float32x2_t l = vget_low_f32(x2);
	float32x2_t h = vget_high_f32(x2); h = vset_lane_f32(vgetq_lane_f32( m_quad,3),h,1);
	float32x2_t xy_zw = vpadd_f32(l, h); // (0+1, 2+(3))
	float32x2_t xyzw = vpadd_f32(xy_zw, xy_zw); // 0+1+2+(3)
	return hkSimdFloat32::convert(xyzw);
}

template <>
HK_FORCE_INLINE const hkSimdFloat32 hkVector4f::horizontalAdd<2>() const
{
	float32x2_t l = vget_low_f32( m_quad );
	float32x2_t xy = vpadd_f32(l, l); // (0+1, 0+1)
	return hkSimdFloat32::convert(xy);	
}

template <>
HK_FORCE_INLINE const hkSimdFloat32 hkVector4f::horizontalAdd<3>() const
{
	float32x2_t l = vget_low_f32( m_quad );
	float32x2_t h = vget_high_f32( m_quad ); h = vset_lane_f32((hkFloat32)0,h,1);
	float32x2_t xy_zw = vpadd_f32(l, h); // (0+1, 2+(3))
	float32x2_t xyzw = vpadd_f32(xy_zw, xy_zw); // 0+1+2+(3)
	return hkSimdFloat32::convert(xyzw);	
}

template <>
HK_FORCE_INLINE const hkSimdFloat32 hkVector4f::horizontalAdd<4>() const
{
	float32x2_t l = vget_low_f32( m_quad );
	float32x2_t h = vget_high_f32( m_quad );
	float32x2_t xy_zw = vpadd_f32(l, h); // (0+1, 2+3)
	float32x2_t xyzw = vpadd_f32(xy_zw, xy_zw); // 0+1+2+3
	return hkSimdFloat32::convert(xyzw);	
}

template <int N>
HK_FORCE_INLINE const hkSimdFloat32 hkVector4f::horizontalAdd() const
{
	HK_VECTOR4f_NOT_IMPLEMENTED;
	return hkSimdFloat32::getConstant<HK_QUADREAL_0>();
}

template <>
HK_FORCE_INLINE const hkSimdFloat32 hkVector4f::horizontalMul<2>() const
{
	const hkSimdFloat32 xy = getComponent<0>() * getComponent<1>();
	return xy;	
}

template <>
HK_FORCE_INLINE const hkSimdFloat32 hkVector4f::horizontalMul<3>() const
{
	const hkSimdFloat32 xyz = getComponent<0>() * getComponent<1>() * getComponent<2>();
	return xyz;
}

template <>
HK_FORCE_INLINE const hkSimdFloat32 hkVector4f::horizontalMul<4>() const
{
	// todo vpmul?
	const hkSimdFloat32 xyzw = getComponent<0>() * getComponent<1>() * getComponent<2>() * getComponent<3>();
	return xyzw;
}

template <int N>
HK_FORCE_INLINE const hkSimdFloat32 hkVector4f::horizontalMul() const
{
	HK_VECTOR4f_NOT_IMPLEMENTED;
	return hkSimdFloat32::getConstant<HK_QUADREAL_0>();
}

template <>
HK_FORCE_INLINE const hkSimdFloat32 hkVector4f::horizontalMax<1>() const
{
	return getComponent<0>();
}

template <>
HK_FORCE_INLINE const hkSimdFloat32 hkVector4f::horizontalMax<2>() const
{
	float32x2_t l = vget_low_f32(m_quad);
	return hkSimdFloat32::convert(vpmax_f32(l,l));
}

template <>
HK_FORCE_INLINE const hkSimdFloat32 hkVector4f::horizontalMax<3>() const
{
	float32x2_t l = vget_low_f32(m_quad);
	float32x2_t xy = vpmax_f32(l,l);
	float32x2_t zz = vdup_lane_f32( vget_high_f32(m_quad), 0);
	return hkSimdFloat32::convert(vmax_f32(zz, xy));
}

template <>
HK_FORCE_INLINE const hkSimdFloat32 hkVector4f::horizontalMax<4>() const
{
	float32x2_t l = vget_low_f32(m_quad);
	float32x2_t h = vget_high_f32(m_quad);
	float32x2_t xy_zw = vpmax_f32(l,h);
	return hkSimdFloat32::convert(vpmax_f32(xy_zw,xy_zw));
}

template <int N>
HK_FORCE_INLINE const hkSimdFloat32 hkVector4f::horizontalMax() const
{
	HK_VECTOR4f_NOT_IMPLEMENTED;
	return hkSimdFloat32::getConstant<HK_QUADREAL_0>();
}

template <>
HK_FORCE_INLINE const hkSimdFloat32 hkVector4f::horizontalMin<2>() const
{
	float32x2_t l = vget_low_f32(m_quad);
	return hkSimdFloat32::convert(vpmin_f32(l,l));
}

template <>
HK_FORCE_INLINE const hkSimdFloat32 hkVector4f::horizontalMin<3>() const
{
	float32x2_t l = vget_low_f32(m_quad);
	float32x2_t xy = vpmin_f32(l,l);
	float32x2_t zz = vdup_lane_f32( vget_high_f32(m_quad), 0);
	return hkSimdFloat32::convert(vmin_f32(zz, xy));
}

template <>
HK_FORCE_INLINE const hkSimdFloat32 hkVector4f::horizontalMin<4>() const
{
	float32x2_t l = vget_low_f32(m_quad);
	float32x2_t h = vget_high_f32(m_quad);
	float32x2_t xy_zw = vpmin_f32(l,h);
	return hkSimdFloat32::convert(vpmin_f32(xy_zw,xy_zw));
}

template <int N>
HK_FORCE_INLINE const hkSimdFloat32 hkVector4f::horizontalMin() const
{
	HK_VECTOR4f_NOT_IMPLEMENTED;
	return hkSimdFloat32::getConstant<HK_QUADREAL_0>();
}

/* operator () */

#if defined(HK_PLATFORM_IOS)




struct _hkFloat32Struct
{
    hkFloat32 val[4];
};

HK_FORCE_INLINE hkFloat32& hkVector4f::operator() (int a)
{
	HK_MATH_ASSERT(0x6d0c31d7, a>=0 && a<4, "index out of bounds for component access");
	return reinterpret_cast<_hkFloat32Struct&>(m_quad).val[a];
}

HK_FORCE_INLINE const hkFloat32& hkVector4f::operator() (int a) const
{
	HK_MATH_ASSERT(0x6d0c31d7, a>=0 && a<4, "index out of bounds for component access");
	return reinterpret_cast<const _hkFloat32Struct&>(m_quad).val[a];
}

#else

HK_FORCE_INLINE hkFloat32& hkVector4f::operator() (int a)
{
	HK_MATH_ASSERT(0x6d0c31d7, a>=0 && a<4, "index out of bounds for component access");
	return reinterpret_cast<hkFloat32*>(&m_quad)[a];
}

HK_FORCE_INLINE const hkFloat32& hkVector4f::operator() (int a) const
{
	HK_MATH_ASSERT(0x6d0c31d7, a>=0 && a<4, "index out of bounds for component access");
	return reinterpret_cast<const hkFloat32*>(&m_quad)[a];
}

#endif

HK_FORCE_INLINE void hkVector4f::setXYZ_W(hkVector4fParameter xyz, hkVector4fParameter w)
{
	m_quad = vsetq_lane_f32(vgetq_lane_f32(w.m_quad, 3), xyz.m_quad, 3);
}

HK_FORCE_INLINE void hkVector4f::setXYZ_W(hkVector4fParameter xyz, hkSimdFloat32Parameter w)
{
	m_quad = vsetq_lane_f32(vget_lane_f32(w.m_real, 0), xyz.m_quad, 3);
}

HK_FORCE_INLINE void hkVector4f::setW(hkVector4fParameter w)
{
	m_quad = vsetq_lane_f32(vgetq_lane_f32(w.m_quad, 3), m_quad, 3);
}

HK_FORCE_INLINE void hkVector4f::setXYZ(hkVector4fParameter xyz)
{
	m_quad = vsetq_lane_f32(vgetq_lane_f32(m_quad, 3), xyz.m_quad, 3);
}

HK_FORCE_INLINE void hkVector4f::addXYZ(hkVector4fParameter xyz)
{
	m_quad = vaddq_f32( m_quad, xyz.m_quad );
	HK_ON_DEBUG( reinterpret_cast<hkUint32*>(&m_quad)[3] = 0xffffffff; )
}

HK_FORCE_INLINE void hkVector4f::subXYZ(hkVector4fParameter xyz)
{
	m_quad = vsubq_f32( m_quad, xyz.m_quad );
	HK_ON_DEBUG( reinterpret_cast<hkUint32*>(&m_quad)[3] = 0xffffffff; )
}

HK_FORCE_INLINE void hkVector4f::setXYZ(hkFloat32 v)
{
	hkQuadFloat32 xyz = vdupq_n_f32(v);
	m_quad = vsetq_lane_f32(vgetq_lane_f32(m_quad, 3), xyz, 3);
}

HK_FORCE_INLINE void hkVector4f::setXYZ(hkSimdFloat32Parameter v)
{
	hkQuadFloat32 xyz = vcombine_f32(v.m_real, v.m_real);
	m_quad = vsetq_lane_f32(vgetq_lane_f32(m_quad, 3), xyz, 3);
}

HK_FORCE_INLINE void hkVector4f::setXYZ_0(hkVector4fParameter xyz)
{
	m_quad = vsetq_lane_f32( (hkFloat32)0, xyz.m_quad, 3);
}

HK_FORCE_INLINE void hkVector4f::setBroadcastXYZ(const int i, hkVector4fParameter v)
{
	setBroadcast(i,v);
	HK_ON_DEBUG( reinterpret_cast<hkUint32*>(&m_quad)[3] = 0xffffffff; )
}

HK_FORCE_INLINE const hkSimdFloat32 hkVector4f::getComponent(const int i) const
{
	HK_MATH_ASSERT(0x6d0c31d7, i>=0 && i<4, "index out of bounds for component access");

	hkSingleFloat32 r;
	switch(i)
	{
		case 1: r = vdup_lane_f32(vget_low_f32(m_quad), 1); break;
		case 2: r = vdup_lane_f32(vget_high_f32(m_quad), 0); break;
		case 3: r = vdup_lane_f32(vget_high_f32(m_quad), 1); break;
		default: r = vdup_lane_f32(vget_low_f32(m_quad), 0); break;
	}

	return hkSimdFloat32::convert(r);
}

template <int I>
HK_FORCE_INLINE const hkSimdFloat32 hkVector4f::getComponent() const
{
	HK_VECTOR4f_SUBINDEX_CHECK;

	hkSingleFloat32 r;
	switch(I)
	{
		case 1: r = vdup_lane_f32(vget_low_f32(m_quad), 1); break;
		case 2: r = vdup_lane_f32(vget_high_f32(m_quad), 0); break;
		case 3: r = vdup_lane_f32(vget_high_f32(m_quad), 1); break;
		default: r = vdup_lane_f32(vget_low_f32(m_quad), 0); break;
	}

	return hkSimdFloat32::convert(r);
}


HK_FORCE_INLINE void hkVector4f::setComponent(const int i, hkSimdFloat32Parameter val)
{
	HK_MATH_ASSERT(0x6d0c31d7, i>=0 && i<4, "index out of bounds for component access");
	switch(i)
	{
		case 1: m_quad = vsetq_lane_f32(vget_lane_f32(val.m_real, 0), m_quad, 1); break;
		case 2: m_quad = vsetq_lane_f32(vget_lane_f32(val.m_real, 0), m_quad, 2); break;
		case 3: m_quad = vsetq_lane_f32(vget_lane_f32(val.m_real, 0), m_quad, 3); break;
		default: m_quad = vsetq_lane_f32(vget_lane_f32(val.m_real, 0), m_quad, 0); break;
	}
}

template <int I>
HK_FORCE_INLINE void hkVector4f::setComponent(hkSimdFloat32Parameter val)
{
	HK_VECTOR4f_SUBINDEX_CHECK;
    // Add Switch to fix Clang issue were last parameter of vgetq_lane_f32 must be in constrange(0,3)
    // See RSYS-1377 for more information
	switch(I)
	{
		case 1: m_quad = vsetq_lane_f32(vget_lane_f32(val.m_real, 0), m_quad, 1); break;
		case 2: m_quad = vsetq_lane_f32(vget_lane_f32(val.m_real, 0), m_quad, 2); break;
		case 3: m_quad = vsetq_lane_f32(vget_lane_f32(val.m_real, 0), m_quad, 3); break;
		default: m_quad = vsetq_lane_f32(vget_lane_f32(val.m_real, 0), m_quad, 0); break;
	}
}

HK_FORCE_INLINE void hkVector4f::reduceToHalfPrecision()
{
	static const uint32x4_t precisionMask = HK_NEON_CONSTANT4I( 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000);
	m_quad = vreinterpretq_f32_u32( vandq_u32( vreinterpretq_u32_f32(m_quad), precisionMask ) );
}

template <> 
HK_FORCE_INLINE hkBool32 hkVector4f::isOk<1>() const
{
	static const uint32x2_t quietnan = HK_NEON_CONSTANT2I(  0x7FC00000, 0x7FC00000);
	uint32x2_t mask = vand_u32(vreinterpret_u32_f32(vget_low_f32(m_quad)), quietnan);
	uint32x2_t test = vceq_u32(mask, quietnan);
	test = vmvn_u32(test);
	return vget_lane_u32(test,0);
}

template <> 
HK_FORCE_INLINE hkBool32 hkVector4f::isOk<2>() const
{
	static const uint32x2_t quietnan = HK_NEON_CONSTANT2I( 0x7FC00000, 0x7FC00000);
	uint32x2_t mask = vand_u32(vreinterpret_u32_f32(vget_low_f32(m_quad)), quietnan);
	uint32x2_t test = vceq_u32(mask, quietnan);
	test = vmvn_u32(test);
	return vget_lane_u32(test,0) & vget_lane_u32(test,1);
}

template <> 
HK_FORCE_INLINE hkBool32 hkVector4f::isOk<3>() const
{
	static const uint32x4_t quietnan = HK_NEON_CONSTANT4I(  0x7FC00000, 0x7FC00000, 0x7FC00000, 0x7FC00000 );
	uint32x4_t mask = vandq_u32(vreinterpretq_u32_f32(m_quad), quietnan);
	uint32x4_t test = vceqq_u32(mask, quietnan);
	test = vmvnq_u32(test);
	return vgetq_lane_u32(test,0) & vgetq_lane_u32(test,1) & vgetq_lane_u32(test,2);
}

template <> 
HK_FORCE_INLINE hkBool32 hkVector4f::isOk<4>() const
{
	static const uint32x4_t quietnan = HK_NEON_CONSTANT4I( 0x7FC00000, 0x7FC00000, 0x7FC00000, 0x7FC00000 );
	uint32x4_t mask = vandq_u32(vreinterpretq_u32_f32(m_quad), quietnan);
	uint32x4_t test = vceqq_u32(mask, quietnan);
	test = vmvnq_u32(test);
	return vgetq_lane_u32(test,0) & vgetq_lane_u32(test,1) & vgetq_lane_u32(test,2) & vgetq_lane_u32(test,3);
}

template <int N> 
HK_FORCE_INLINE hkBool32 hkVector4f::isOk() const
{
	HK_VECTOR4f_NOT_IMPLEMENTED;
	return false;
}

template <> 
HK_FORCE_INLINE void hkVector4f::setPermutation<hkVectorPermutation::XYZW>(hkVector4fParameter v)
{
	m_quad = v.m_quad;
}

template <> 
HK_FORCE_INLINE void hkVector4f::setPermutation<hkVectorPermutation::YZWX>(hkVector4fParameter v)
{
	m_quad = vextq_f32( v.m_quad, v.m_quad, 1);
}

template <> 
HK_FORCE_INLINE void hkVector4f::setPermutation<hkVectorPermutation::ZWXY>(hkVector4fParameter v)
{
	m_quad = vextq_f32( v.m_quad, v.m_quad, 2);
}

template <> 
HK_FORCE_INLINE void hkVector4f::setPermutation<hkVectorPermutation::WXYZ>(hkVector4fParameter v)
{
	m_quad = vextq_f32( v.m_quad, v.m_quad, 3);
}

template <hkVectorPermutation::Permutation P> 
HK_FORCE_INLINE void hkVector4f::setPermutation(hkVector4fParameter v)
{
	const float32x4_t val = v.m_quad;

    // Add Switch to fix Clang issue were last parameter of vgetq_lane_f32 must be in constrange(0,3)
    // See RSYS-1377 for more information
    int first = (P >> 12) & 0x3;
    int second = (P >> 8) & 0x3;
    int third = (P >> 4) & 0x3;
    int fourth = (P >> 0) & 0x3;
    
    switch(first)
    {
        case 1:  m_quad = vsetq_lane_f32(vgetq_lane_f32(val, 1), m_quad, 0); break;
        case 2:  m_quad = vsetq_lane_f32(vgetq_lane_f32(val, 2), m_quad, 0); break;
        case 3:  m_quad = vsetq_lane_f32(vgetq_lane_f32(val, 3), m_quad, 0); break;
        default: m_quad = vsetq_lane_f32(vgetq_lane_f32(val, 0), m_quad, 0); break;
    }
    switch(second)
    {
        case 1:  m_quad = vsetq_lane_f32(vgetq_lane_f32(val, 1), m_quad, 1); break;
        case 2:  m_quad = vsetq_lane_f32(vgetq_lane_f32(val, 2), m_quad, 1); break;
        case 3:  m_quad = vsetq_lane_f32(vgetq_lane_f32(val, 3), m_quad, 1); break;
        default: m_quad = vsetq_lane_f32(vgetq_lane_f32(val, 0), m_quad, 1); break;
    }
    switch(third)
    {
        case 1:  m_quad = vsetq_lane_f32(vgetq_lane_f32(val, 1), m_quad, 2); break;
        case 2:  m_quad = vsetq_lane_f32(vgetq_lane_f32(val, 2), m_quad, 2); break;
        case 3:  m_quad = vsetq_lane_f32(vgetq_lane_f32(val, 3), m_quad, 2); break;
        default: m_quad = vsetq_lane_f32(vgetq_lane_f32(val, 0), m_quad, 2); break;
    }
    switch(fourth)
    {
        case 1:  m_quad = vsetq_lane_f32(vgetq_lane_f32(val, 1), m_quad, 3); break;
        case 2:  m_quad = vsetq_lane_f32(vgetq_lane_f32(val, 2), m_quad, 3); break;
        case 3:  m_quad = vsetq_lane_f32(vgetq_lane_f32(val, 3), m_quad, 3); break;
        default: m_quad = vsetq_lane_f32(vgetq_lane_f32(val, 0), m_quad, 3); break;
    }
}

HK_FORCE_INLINE const hkVector4fComparison hkVector4f::signBitSet() const
{
	static const uint32x4_t allsigns = HK_NEON_CONSTANT4I(  0x80000000, 0x80000000, 0x80000000, 0x80000000 );
	return hkVector4fComparison::convert( vtstq_u32(vreinterpretq_u32_f32(m_quad), allsigns) );
}

HK_FORCE_INLINE const hkVector4fComparison hkVector4f::signBitClear() const
{
	static const uint32x4_t allsigns = HK_NEON_CONSTANT4I(  0x80000000, 0x80000000, 0x80000000, 0x80000000 );
	return hkVector4fComparison::convert( vmvnq_u32(vtstq_u32(vreinterpretq_u32_f32(m_quad), allsigns)) );
}

HK_FORCE_INLINE void hkVector4f::setFlipSign(hkVector4fParameter v, hkVector4fComparisonParameter mask)
{
	static const uint32x4_t allsigns = HK_NEON_CONSTANT4I(  0x80000000, 0x80000000, 0x80000000, 0x80000000 );
	m_quad = vreinterpretq_f32_u32( veorq_u32(  vreinterpretq_u32_f32( v.m_quad ), vandq_u32(mask.m_mask, allsigns)));
}

HK_FORCE_INLINE void hkVector4f::setFlipSign(hkVector4fParameter v, hkVector4fParameter vSign)
{
	static const uint32x4_t allsigns = HK_NEON_CONSTANT4I( 0x80000000, 0x80000000, 0x80000000, 0x80000000 );
	m_quad = vreinterpretq_f32_u32( veorq_u32(  vreinterpretq_u32_f32( v.m_quad ), vandq_u32(vreinterpretq_u32_f32(vSign.m_quad), allsigns)));
}

HK_FORCE_INLINE void hkVector4f::setFlipSign(hkVector4fParameter v, hkSimdFloat32Parameter sSign)
{
	static const uint32x4_t allsigns = HK_NEON_CONSTANT4I( 0x80000000, 0x80000000, 0x80000000, 0x80000000 );
	float32x4_t sharedSignV = vcombine_f32(sSign.m_real, sSign.m_real);
	m_quad = vreinterpretq_f32_u32( veorq_u32(  vreinterpretq_u32_f32( v.m_quad ), vandq_u32(vreinterpretq_u32_f32(sharedSignV), allsigns)));
}


//
// advanced interface
//

namespace hkVector4f_AdvancedInterface
{

template <hkMathAccuracyMode A, hkMathDivByZeroMode D>
struct unroll_setReciprocal { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, hkVector4fParameter a)
{
	HK_VECTOR4f_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_IGNORE> { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, hkVector4fParameter a)
{
	switch (A)
	{
		case HK_ACC_23_BIT: 
			{
				self = hkMath::quadReciprocal(a.m_quad); 
			}
			break;
		case HK_ACC_12_BIT: 
			{
				hkQuadFloat32 bb = vrecpeq_f32(a.m_quad); // estimate is only 8 bit
				self = vmulq_f32( bb, vrecpsq_f32( a.m_quad, bb )); // so do a pass1
			}
			break;
		default:
			{
				self = hkMath::quadReciprocalTwoIter(a.m_quad); 
			}
			break; // HK_ACC_FULL
	}
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_ZERO> { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, hkVector4fParameter a)
{
	hkQuadFloat32 zero = vcombine_f32(vcreate_f32(0), vcreate_f32(0));
	uint32x4_t rEqualsZero = vceqq_f32( a.m_quad, zero); // todo how to use the VCEQ #0 instruction
	hkQuadFloat32 e; unroll_setReciprocal<A, HK_DIV_IGNORE>::apply(e, a);
	self = vreinterpretq_f32_u32( vbicq_u32( vreinterpretq_u32_f32(e), rEqualsZero )); 		
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_HIGH> { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, hkVector4fParameter a)
{
	hkQuadFloat32 huge = vdupq_n_f32( HK_FLOAT_HIGH );
	hkQuadFloat32 zero = vcombine_f32(vcreate_f32(0), vcreate_f32(0));
	uint32x4_t rEqualsZero = vceqq_f32( a.m_quad, zero); // todo how to use the VCEQ #0 instruction
	hkQuadFloat32 e; unroll_setReciprocal<A, HK_DIV_IGNORE>::apply(e, a);
	self = vbslq_f32( rEqualsZero, huge, e );
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_MAX> { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, hkVector4fParameter a)
{
	hkQuadFloat32 huge = vdupq_n_f32( HK_FLOAT_MAX );
	hkQuadFloat32 zero = vcombine_f32(vcreate_f32(0), vcreate_f32(0));
	uint32x4_t rEqualsZero = vceqq_f32( a.m_quad, zero); // todo how to use the VCEQ #0 instruction
	hkQuadFloat32 e; unroll_setReciprocal<A, HK_DIV_IGNORE>::apply(e, a);
	self = vbslq_f32( rEqualsZero, huge, e );
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_ZERO_AND_ONE> { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, hkVector4fParameter a)
{
	unroll_setReciprocal<A, HK_DIV_SET_ZERO>::apply(self, a);
	hkQuadFloat32 one = vdupq_n_f32( 1.0f );
	hkQuadFloat32 eps = vdupq_n_f32( HK_FLOAT_EPSILON );
	hkQuadFloat32 absVal = vabsq_f32(vsubq_f32(self, one));	
    uint32x4_t lessEqualEps = vcleq_f32(absVal, eps);
	self = vbslq_f32( lessEqualEps, one, self );
} };

} // namespace 

template <hkMathAccuracyMode A, hkMathDivByZeroMode D>
HK_FORCE_INLINE void hkVector4f::setReciprocal(hkVector4fParameter a)
{
	hkVector4f_AdvancedInterface::unroll_setReciprocal<A,D>::apply(m_quad,a);
}

HK_FORCE_INLINE void hkVector4f::setReciprocal(hkVector4fParameter a)
{
	hkVector4f_AdvancedInterface::unroll_setReciprocal<HK_ACC_23_BIT,HK_DIV_IGNORE>::apply(m_quad,a);
}



namespace hkVector4f_AdvancedInterface
{

template <hkMathAccuracyMode A, hkMathDivByZeroMode D>
struct unroll_setDiv { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, hkVector4fParameter a, hkVector4fParameter b)
{
	HK_VECTOR4f_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <hkMathAccuracyMode A>
struct unroll_setDiv<A, HK_DIV_IGNORE> { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, hkVector4fParameter a, hkVector4fParameter b)
{
	// Neon does not have a div instruction
	hkQuadFloat32 t;
	unroll_setReciprocal<A,HK_DIV_IGNORE>::apply(t,b);
	self = vmulq_f32(a.m_quad, t);
} };
template <hkMathAccuracyMode A>
struct unroll_setDiv<A, HK_DIV_SET_ZERO> { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, hkVector4fParameter a, hkVector4fParameter b)
{
	hkQuadFloat32 zero = vcombine_f32(vcreate_f32(0), vcreate_f32(0));
	uint32x4_t rEqualsZero = vceqq_f32( b.m_quad, zero); // todo how to use the VCEQ #0 instruction
	hkQuadFloat32 e; unroll_setDiv<A, HK_DIV_IGNORE>::apply(e, a, b);
	self = vreinterpretq_f32_u32( vbicq_u32( vreinterpretq_u32_f32(e), rEqualsZero ));
} };
template <hkMathAccuracyMode A>
struct unroll_setDiv<A, HK_DIV_SET_HIGH> { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, hkVector4fParameter a, hkVector4fParameter b)
{
	static const uint32x4_t allsigns = HK_NEON_CONSTANT4I( 0x80000000, 0x80000000, 0x80000000, 0x80000000 );
	hkQuadFloat32 huge = vdupq_n_f32( HK_FLOAT_HIGH );
	hkQuadFloat32 zero = vcombine_f32(vcreate_f32(0), vcreate_f32(0));
	uint32x4_t rEqualsZero = vceqq_f32( b.m_quad, zero); // todo how to use the VCEQ #0 instruction
	hkQuadFloat32 e; unroll_setDiv<A, HK_DIV_IGNORE>::apply(e, a, b);
	huge = vreinterpretq_f32_u32( veorq_u32(  vreinterpretq_u32_f32( huge ), vandq_u32(vreinterpretq_u32_f32(a.m_quad), allsigns)));
	self = vbslq_f32( rEqualsZero, huge, e );
} };
template <hkMathAccuracyMode A>
struct unroll_setDiv<A, HK_DIV_SET_MAX> { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, hkVector4fParameter a, hkVector4fParameter b)
{
	static const uint32x4_t allsigns = HK_NEON_CONSTANT4I( 0x80000000, 0x80000000, 0x80000000, 0x80000000 );
	hkQuadFloat32 huge = vdupq_n_f32( HK_FLOAT_MAX );
	hkQuadFloat32 zero = vcombine_f32(vcreate_f32(0), vcreate_f32(0));
	uint32x4_t rEqualsZero = vceqq_f32( b.m_quad, zero); // todo how to use the VCEQ #0 instruction
	hkQuadFloat32 e; unroll_setDiv<A, HK_DIV_IGNORE>::apply(e, a, b);
	huge = vreinterpretq_f32_u32( veorq_u32(  vreinterpretq_u32_f32( huge ), vandq_u32(vreinterpretq_u32_f32(a.m_quad), allsigns)));
	self = vbslq_f32( rEqualsZero, huge, e );
} };
template <hkMathAccuracyMode A>
struct unroll_setDiv<A, HK_DIV_SET_ZERO_AND_ONE> { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, hkVector4fParameter a, hkVector4fParameter b)
{
	unroll_setDiv<A, HK_DIV_SET_ZERO>::apply(self, a, b);
	hkQuadFloat32 one = vdupq_n_f32( 1.0f );
	hkQuadFloat32 eps = vdupq_n_f32( HK_FLOAT_EPSILON );
	hkQuadFloat32 absVal = vabsq_f32(vsubq_f32(self, one));	
	uint32x4_t lessEqualEps = vcleq_f32(absVal, eps);
	self = vbslq_f32( lessEqualEps, one, self );
} };

} // namespace 

template <hkMathAccuracyMode A, hkMathDivByZeroMode D>
HK_FORCE_INLINE void hkVector4f::setDiv(hkVector4fParameter v0, hkVector4fParameter v1)
{
	hkVector4f_AdvancedInterface::unroll_setDiv<A,D>::apply(m_quad,v0,v1);
}

HK_FORCE_INLINE void hkVector4f::setDiv(hkVector4fParameter v0, hkVector4fParameter v1)
{
	hkVector4f_AdvancedInterface::unroll_setDiv<HK_ACC_23_BIT,HK_DIV_IGNORE>::apply(m_quad,v0,v1);
}

template <hkMathAccuracyMode A, hkMathDivByZeroMode D>
HK_FORCE_INLINE void hkVector4f::div(hkVector4fParameter a)
{
	setDiv<A,D>( *this, a );
}

HK_FORCE_INLINE void hkVector4f::div(hkVector4fParameter a)
{
	setDiv( *this, a );
}



namespace hkVector4f_AdvancedInterface
{

template <hkMathAccuracyMode A, hkMathNegSqrtMode S>
struct unroll_setSqrtInverse { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, hkVector4fParameter a)
{
	HK_VECTOR4f_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <hkMathAccuracyMode A>
struct unroll_setSqrtInverse<A, HK_SQRT_IGNORE> { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, hkVector4fParameter a)
{
	switch (A)
	{
		case HK_ACC_23_BIT: 
			{
				self = hkMath::quadReciprocalSquareRoot(a.m_quad); 
			}
			break;
		case HK_ACC_12_BIT: 
			{
				hkQuadFloat32 r0 = vrsqrteq_f32( a.m_quad ); // estimate is only 8Bit
				hkQuadFloat32 e0 = vmulq_f32(r0, r0);
				self = vmulq_f32( vrsqrtsq_f32(  a.m_quad, e0 ), r0 ); // so do a pass1
			}
			break;
		default:
			{
				self = hkMath::quadReciprocalSquareRootTwoIter(a.m_quad); 
			}
			break; // HK_ACC_FULL
	}
} };
template <hkMathAccuracyMode A>
struct unroll_setSqrtInverse<A, HK_SQRT_SET_ZERO> { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, hkVector4fParameter a)
{
	hkQuadFloat32 zero = vcombine_f32(vcreate_f32(0), vcreate_f32(0));
	uint32x4_t rEqualsZero = vcleq_f32( a.m_quad, zero); // todo how to use the VCLE #0 instruction
	hkQuadFloat32 e; unroll_setSqrtInverse<A, HK_SQRT_IGNORE>::apply(e,a);
	self = vreinterpretq_f32_u32( vbicq_u32( vreinterpretq_u32_f32(e), rEqualsZero ));
} };
} // namespace 

template <hkMathAccuracyMode A, hkMathNegSqrtMode S>
HK_FORCE_INLINE void hkVector4f::setSqrtInverse(hkVector4fParameter a)
{
	hkVector4f_AdvancedInterface::unroll_setSqrtInverse<A,S>::apply(m_quad,a);
}

HK_FORCE_INLINE void hkVector4f::setSqrtInverse(hkVector4fParameter a)
{
	hkVector4f_AdvancedInterface::unroll_setSqrtInverse<HK_ACC_23_BIT,HK_SQRT_SET_ZERO>::apply(m_quad,a);
}



namespace hkVector4f_AdvancedInterface
{

template <hkMathAccuracyMode A, hkMathNegSqrtMode S>
struct unroll_setSqrt { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, hkVector4fParameter a)
{
	HK_VECTOR4f_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <hkMathAccuracyMode A>
struct unroll_setSqrt<A, HK_SQRT_IGNORE> { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, hkVector4fParameter a)
{
	// Neon does not have a sqrt instruction
	// need to check the 0.0 case because we use invSqrt
	hkQuadFloat32 zero = vcombine_f32(vcreate_f32(0), vcreate_f32(0));
	uint32x4_t rEqualsZero = vceqq_f32( a.m_quad, zero); // todo how to use the VCEQ #0 instruction
	hkQuadFloat32 e; unroll_setSqrtInverse<A, HK_SQRT_IGNORE>::apply(e,a);
	// Neon mul does not normalize the sign -> premul to clear sign
	e = vmulq_f32(a.m_quad, e);
	self = vreinterpretq_f32_u32( vbicq_u32( vreinterpretq_u32_f32(e), rEqualsZero ));
} };
template <hkMathAccuracyMode A>
struct unroll_setSqrt<A, HK_SQRT_SET_ZERO> { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, hkVector4fParameter a)
{
	// Neon does not have a sqrt instruction
	hkQuadFloat32 zero = vcombine_f32(vcreate_f32(0), vcreate_f32(0));
	uint32x4_t rEqualsZero = vcleq_f32( a.m_quad, zero); // todo how to use the VCLE #0 instruction
	hkQuadFloat32 e; unroll_setSqrtInverse<A, HK_SQRT_IGNORE>::apply(e,a);
	// Neon mul does not normalize the sign -> premul to clear sign
	e = vmulq_f32(a.m_quad, e);
	self = vreinterpretq_f32_u32( vbicq_u32( vreinterpretq_u32_f32(e), rEqualsZero ));
} };

} // namespace 

template <hkMathAccuracyMode A, hkMathNegSqrtMode S>
HK_FORCE_INLINE void hkVector4f::setSqrt(hkVector4fParameter a)
{
	hkVector4f_AdvancedInterface::unroll_setSqrt<A,S>::apply(m_quad,a);
}

HK_FORCE_INLINE void hkVector4f::setSqrt(hkVector4fParameter a)
{
	hkVector4f_AdvancedInterface::unroll_setSqrt<HK_ACC_23_BIT,HK_SQRT_SET_ZERO>::apply(m_quad,a);
}



namespace hkVector4f_AdvancedInterface
{
template <int N, hkMathIoMode A>
struct unroll_load { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, const hkFloat32* HK_RESTRICT p)
{
	HK_VECTOR4f_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <int N, hkMathIoMode A>
struct unroll_load_D { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, const hkDouble64* HK_RESTRICT p)
{
	HK_VECTOR4f_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <int N>
struct unroll_load<N, HK_IO_NATIVE_ALIGNED> { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, const hkFloat32* HK_RESTRICT p)
{
	// neon does not align HK_MATH_ASSERT(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkFloat32)-1) ) == 0, "pointer must be aligned to native size of hkFloat32.");
	switch (N)
	{
		case 4:
			{
				self = vld1q_f32( (float32_t const *)p );
			}
			break;
		case 3:
			{
				hkSingleFloat32 q  = vld1_f32( (float32_t const *)p );
				hkSingleFloat32 q2 = vld1_dup_f32( (float32_t const *) (p+2));
				self = vcombine_f32(q,q2);
				HK_ON_DEBUG(reinterpret_cast<hkUint32*>(&self)[3] = 0xffffffff;)
			}
			break;
		case 2:
			{
				hkSingleFloat32 q = vld1_f32( (float32_t const *)p );
				self = vcombine_f32(q,q);
				HK_ON_DEBUG(reinterpret_cast<hkUint32*>(&self)[2] = 0xffffffff; reinterpret_cast<hkUint32*>(&self)[3] = 0xffffffff;)
			}
			break;
		default:
			{
				self = vld1q_dup_f32( (float32_t const *)p );
				HK_ON_DEBUG(reinterpret_cast<hkUint32*>(&self)[1] = 0xffffffff; reinterpret_cast<hkUint32*>(&self)[2] = 0xffffffff; reinterpret_cast<hkUint32*>(&self)[3] = 0xffffffff;)
			}
			break;
	}
} };
template <int N>
struct unroll_load_D<N, HK_IO_NATIVE_ALIGNED> { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, const hkDouble64* HK_RESTRICT p)
{
	// neon does not align HK_MATH_ASSERT(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkDouble64)-1) ) == 0, "pointer must be aligned to native size of hkDouble64.");
	float a[4]; 
	switch (N)
	{
	case 4:
		{
			a[0] = float(p[0]); a[1] = float(p[1]); a[2] = float(p[2]); a[3] = float(p[3]); 
			self = vld1q_f32( (float32_t const *)a );
		}
		break;
	case 3:
		{
			a[0] = float(p[0]); a[1] = float(p[1]); a[2] = float(p[2]);
			hkSingleFloat32 q  = vld1_f32( (float32_t const *)a );
			hkSingleFloat32 q2 = vld1_dup_f32( (float32_t const *) &(a[2]));
			self = vcombine_f32(q,q2);
			HK_ON_DEBUG(reinterpret_cast<hkUint32*>(&self)[3] = 0xffffffff;)
		}
		break;
	case 2:
		{
			a[0] = float(p[0]); a[1] = float(p[1]);
			hkSingleFloat32 q = vld1_f32( (float32_t const *)a );
			self = vcombine_f32(q,q);
			HK_ON_DEBUG(reinterpret_cast<hkUint32*>(&self)[2] = 0xffffffff; reinterpret_cast<hkUint32*>(&self)[3] = 0xffffffff;)
		}
		break;
	default:
		{
			a[0] = float(p[0]);
			self = vld1q_dup_f32( (float32_t const *)a );
			HK_ON_DEBUG(reinterpret_cast<hkUint32*>(&self)[1] = 0xffffffff; reinterpret_cast<hkUint32*>(&self)[2] = 0xffffffff; reinterpret_cast<hkUint32*>(&self)[3] = 0xffffffff;)
		}
		break;
	}
} };
template <int N>
struct unroll_load<N, HK_IO_SIMD_ALIGNED> { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, const hkFloat32* HK_RESTRICT p)
{
	// neon does not align HK_MATH_ASSERT(0x64211c2f, ( ((hkUlong)p) & ((sizeof(hkFloat32)*(N!=3?N:4) )-1) ) == 0, "pointer must be aligned for SIMD.");
	unroll_load<N, HK_IO_NATIVE_ALIGNED>::apply(self,p);
} };
template <int N>
struct unroll_load_D<N, HK_IO_SIMD_ALIGNED> { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, const hkDouble64* HK_RESTRICT p)
{
	// neon does not align HK_MATH_ASSERT(0x64211c2f, ( ((hkUlong)p) & ((sizeof(hkDouble64)*(N!=3?N:4) )-1) ) == 0, "pointer must be aligned for SIMD.");
	unroll_load_D<N, HK_IO_NATIVE_ALIGNED>::apply(self,p);
} };
template <int N>
struct unroll_load<N, HK_IO_NOT_CACHED> { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, const hkFloat32* HK_RESTRICT p)
{
	unroll_load<N, HK_IO_SIMD_ALIGNED>::apply(self,p);
} };
template <int N>
struct unroll_load_D<N, HK_IO_NOT_CACHED> { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, const hkDouble64* HK_RESTRICT p)
{
	unroll_load_D<N, HK_IO_SIMD_ALIGNED>::apply(self,p);
} };
} // namespace 

template <int N, hkMathIoMode A>
HK_FORCE_INLINE void hkVector4f::load(const hkFloat32* p)
{
	HK_VECTOR4f_UNSUPPORTED_LENGTH_CHECK;
	hkVector4f_AdvancedInterface::unroll_load<N,A>::apply(m_quad, p);
}

template <int N, hkMathIoMode A>
HK_FORCE_INLINE void hkVector4f::load(const hkDouble64* p)
{
	HK_VECTOR4f_UNSUPPORTED_LENGTH_CHECK;
	hkVector4f_AdvancedInterface::unroll_load_D<N,A>::apply(m_quad, p);
}

template <int N>
HK_FORCE_INLINE void hkVector4f::load(const hkFloat32* p)
{
	HK_VECTOR4f_UNSUPPORTED_LENGTH_CHECK;
	hkVector4f_AdvancedInterface::unroll_load<N,HK_IO_SIMD_ALIGNED>::apply(m_quad, p);
}

template <int N>
HK_FORCE_INLINE void hkVector4f::load(const hkDouble64* p)
{
	HK_VECTOR4f_UNSUPPORTED_LENGTH_CHECK;
	hkVector4f_AdvancedInterface::unroll_load_D<N,HK_IO_SIMD_ALIGNED>::apply(m_quad, p);
}


namespace hkVector4f_AdvancedInterface
{
template <int N, hkMathIoMode A>
struct unroll_loadH { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, const hkHalf* HK_RESTRICT p)
{
	HK_VECTOR4f_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <int N>
struct unroll_loadH<N, HK_IO_NATIVE_ALIGNED> { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, const hkHalf* HK_RESTRICT p)
{
	// neon does not align HK_MATH_ASSERT(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkHalf)-1) ) == 0, "pointer must be aligned to native size of hkHalf.");
	switch (N)
	{
		case 4:
			{
				uint16x4_t halfs = vld1_u16( (hkUint16 const *)p );
				uint32x4_t r = vshll_n_u16(halfs, 16);
				self = vreinterpretq_f32_u32(r);
			}
			break;
		case 3:
			{
				uint16x4_t halfs;
				halfs = vld1_dup_u16( (hkUint16 const *)p );
				halfs = vld1_lane_u16( (hkUint16 const *)(p+1), halfs, 1 );
				halfs = vld1_lane_u16( (hkUint16 const *)(p+2), halfs, 2 );
				uint32x4_t r = vshll_n_u16(halfs, 16);
				self = vreinterpretq_f32_u32(r);
				HK_ON_DEBUG(reinterpret_cast<hkUint32*>(&self)[3] = 0xffffffff;)
			}
			break;
		case 2:
			{
				uint16x4_t halfs;
				halfs = vld1_dup_u16( (hkUint16 const *)p );
				halfs = vld1_lane_u16( (hkUint16 const *)(p+1), halfs, 1 );
				uint32x4_t r = vshll_n_u16(halfs, 16);
				self = vreinterpretq_f32_u32(r);
				HK_ON_DEBUG(reinterpret_cast<hkUint32*>(&self)[2] = 0xffffffff; reinterpret_cast<hkUint32*>(&self)[3] = 0xffffffff;)
			}
			break;
		default:
			{
				uint16x4_t halfs;
				halfs = vld1_dup_u16( (hkUint16 const *)p );
				uint32x4_t r = vshll_n_u16(halfs, 16);
				self = vreinterpretq_f32_u32(r);
				HK_ON_DEBUG(reinterpret_cast<hkUint32*>(&self)[1] = 0xffffffff; reinterpret_cast<hkUint32*>(&self)[2] = 0xffffffff; reinterpret_cast<hkUint32*>(&self)[3] = 0xffffffff;)
			}
			break;
	}
} };
template <int N>
struct unroll_loadH<N, HK_IO_SIMD_ALIGNED> { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, const hkHalf* HK_RESTRICT p)
{
	// neon does not align HK_MATH_ASSERT(0x64211c2f, ( ((hkUlong)p) & ((sizeof(hkHalf)*(N!=3?N:4) )-1) ) == 0, "pointer must be aligned for SIMD.");
	unroll_loadH<N, HK_IO_NATIVE_ALIGNED>::apply(self,p);
} };
template <int N>
struct unroll_loadH<N, HK_IO_NOT_CACHED> { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, const hkHalf* HK_RESTRICT p)
{
	unroll_loadH<N, HK_IO_SIMD_ALIGNED>::apply(self,p);
} };
} // namespace 

template <int N, hkMathIoMode A>
HK_FORCE_INLINE void hkVector4f::load(const hkHalf* p)
{
	HK_VECTOR4f_UNSUPPORTED_LENGTH_CHECK;
	hkVector4f_AdvancedInterface::unroll_loadH<N,A>::apply(m_quad, p);
}

template <int N>
HK_FORCE_INLINE void hkVector4f::load(const hkHalf* p)
{
	HK_VECTOR4f_UNSUPPORTED_LENGTH_CHECK;
	hkVector4f_AdvancedInterface::unroll_loadH<N,HK_IO_SIMD_ALIGNED>::apply(m_quad, p);
}


namespace hkVector4f_AdvancedInterface
{
template <int N, hkMathIoMode A>
struct unroll_loadF16 { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, const hkFloat16* HK_RESTRICT p)
{
	HK_VECTOR4f_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };

// The cortex-a8 on Iphone does not support 16-bit floats in hardware. By default, assume that other platforms do.
#if defined(HK_PLATFORM_IOS) || defined(HK_PLATFORM_ANDROID) || ( defined(HK_COMPILER_MSVC) && defined(HK_ARCH_ARM) )

template <int N>
struct unroll_loadF16<N, HK_IO_NATIVE_ALIGNED> 
{ 
	HK_FORCE_INLINE static void apply(hkQuadFloat32& self, const hkFloat16* HK_RESTRICT p)
	{
		hkQuadFloat32Union quad;

		for( int i = 0; i < N; ++i )
		{
			quad.r[i] = p[i].getReal();
		}
		self = quad.q;
// 
// 		float32_t f;
// 		switch (N)
// 		{
// 			case 4:
// 			{
// 				f = p[3].getReal(); self = vld1q_lane_f32(&f, self, 3);
// 			}			
// 			case 3:
// 			{
// 				f = p[2].getReal(); self = vld1q_lane_f32(&f, self, 2);			
// 			}			
// 			case 2:
// 			{
// 				f = p[1].getReal(); self = vld1q_lane_f32(&f, self, 1);				
// 			}		
// 			default:
// 			{
// 				f = p[0].getReal(); self = vld1q_lane_f32(&f, self, 0);				
// 				break;
// 			}
// 		}

		// Clear unused components in debug
#if defined(HK_DEBUG)	
		for (int i = N; i < 4; ++i) 
		{
			reinterpret_cast<hkUint32*>(&self)[i] = 0xffffffff;			
		}
#endif
	}
};

#else

template <int N>
struct unroll_loadF16<N, HK_IO_NATIVE_ALIGNED> { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, const hkFloat16* HK_RESTRICT p)
{
	// neon does not align HK_MATH_ASSERT(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkFloat16)-1) ) == 0, "pointer must be aligned to native size of hkFloat16.");
	switch (N)
	{
	case 4:
		{
			float16x4_t halfs = vld1_f16( (__fp16 const *)p );
			self = vcvt_f32_f16(halfs);
		}
		break;
	case 3:
		{
			float16x4_t halfs;
			halfs = vld1_dup_f16( (__fp16 const *)p );
			halfs = vld1_lane_f16( (__fp16 const *)(p+1), halfs, 1 );
			halfs = vld1_lane_f16( (__fp16 const *)(p+2), halfs, 2 );
			self = vcvt_f32_f16(halfs);
			HK_ON_DEBUG(reinterpret_cast<hkUint32*>(&self)[3] = 0xffffffff;)
		}
		break;
	case 2:
		{
			float16x4_t halfs;
			halfs = vld1_dup_f16( (__fp16 const *)p );
			halfs = vld1_lane_f16( (__fp16 const *)(p+1), halfs, 1 );
			self = vcvt_f32_f16(halfs);
			HK_ON_DEBUG(reinterpret_cast<hkUint32*>(&self)[2] = 0xffffffff; reinterpret_cast<hkUint32*>(&self)[3] = 0xffffffff;)
		}
		break;
	default:
		{
			float16x4_t halfs;
			halfs = vld1_dup_f16( (__fp16 const *)p );
			self = vcvt_f32_f16(halfs);
			HK_ON_DEBUG(reinterpret_cast<hkUint32*>(&self)[1] = 0xffffffff; reinterpret_cast<hkUint32*>(&self)[2] = 0xffffffff; reinterpret_cast<hkUint32*>(&self)[3] = 0xffffffff;)
		}
		break;
	}
} };
#endif

template <int N>
struct unroll_loadF16<N, HK_IO_SIMD_ALIGNED> { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, const hkFloat16* HK_RESTRICT p)
{
	// neon does not align HK_MATH_ASSERT(0x64211c2f, ( ((hkUlong)p) & ((sizeof(hkFloat16)*(N!=3?N:4) )-1) ) == 0, "pointer must be aligned for SIMD.");
	unroll_loadF16<N, HK_IO_NATIVE_ALIGNED>::apply(self,p);
} };
template <int N>
struct unroll_loadF16<N, HK_IO_NOT_CACHED> { HK_FORCE_INLINE static void apply(hkQuadFloat32& self, const hkFloat16* HK_RESTRICT p)
{
	unroll_loadF16<N, HK_IO_SIMD_ALIGNED>::apply(self,p);
} };
} // namespace 

template <int N, hkMathIoMode A>
HK_FORCE_INLINE void hkVector4f::load(const hkFloat16* p)
{
	HK_VECTOR4f_UNSUPPORTED_LENGTH_CHECK;
	hkVector4f_AdvancedInterface::unroll_loadF16<N,A>::apply(m_quad, p);
}

template <int N>
HK_FORCE_INLINE void hkVector4f::load(const hkFloat16* p)
{
	HK_VECTOR4f_UNSUPPORTED_LENGTH_CHECK;
	hkVector4f_AdvancedInterface::unroll_loadF16<N,HK_IO_SIMD_ALIGNED>::apply(m_quad, p);
}



namespace hkVector4f_AdvancedInterface
{
template <int N, hkMathIoMode A>
struct unroll_store { HK_FORCE_INLINE static void apply(const hkQuadFloat32& self, hkFloat32* HK_RESTRICT p)
{
	HK_VECTOR4f_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <int N>
struct unroll_store<N, HK_IO_NATIVE_ALIGNED> { HK_FORCE_INLINE static void apply(const hkQuadFloat32& self, hkFloat32* HK_RESTRICT p)
{
	// neon does not align HK_MATH_ASSERT(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkFloat32)-1) ) == 0, "pointer must be aligned to native size of hkFloat32.");
	switch (N)
	{
		case 4:
			{
				vst1q_f32( (float32_t*)p, self );
			}
			break;
		case 3:
			{
				vst1_f32( (float32_t*)p, vget_low_f32( self ) );
				vst1q_lane_f32( (float32_t*)(p+2), self, 2 );
			}
			break;
		case 2:
			{
				vst1_f32( (float32_t*)p, vget_low_f32( self ) );
			}
			break;
		default:
			{
				vst1q_lane_f32( (float32_t*)p, self, 0 );
			}
			break;
	}
} };
template <int N>
struct unroll_store<N, HK_IO_SIMD_ALIGNED> { HK_FORCE_INLINE static void apply(const hkQuadFloat32& self, hkFloat32* HK_RESTRICT p)
{
	// neon does not align HK_MATH_ASSERT(0x64211c2f, ( ((hkUlong)p) & ((sizeof(hkFloat32)*(N!=3?N:4) )-1) ) == 0, "pointer must be aligned for SIMD.");
	unroll_store<N, HK_IO_NATIVE_ALIGNED>::apply(self,p);
} };
template <int N>
struct unroll_store<N, HK_IO_NOT_CACHED> { HK_FORCE_INLINE static void apply(const hkQuadFloat32& self, hkFloat32* HK_RESTRICT p)
{
	unroll_store<N, HK_IO_SIMD_ALIGNED>::apply(self,p);
} };
template <int N, hkMathIoMode A>
struct unroll_store_D { HK_FORCE_INLINE static void apply(const hkQuadFloat32& self, hkDouble64* HK_RESTRICT p)
{
	HK_ALIGN16(hkFloat32 a[4]);
	unroll_store<N, HK_IO_SIMD_ALIGNED>::apply(self, a);
				  p[0] = hkDouble64(a[0]);
	if ( N >= 2){ p[1] = hkDouble64(a[1]); }
	if ( N >= 3){ p[2] = hkDouble64(a[2]); }
	if ( N >= 4){ p[3] = hkDouble64(a[3]); }
} };
} // namespace 

template <int N, hkMathIoMode A, hkMathRoundingMode R> 
HK_FORCE_INLINE void hkVector4f::store(hkFloat32* p) const
{
	HK_VECTOR4f_UNSUPPORTED_LENGTH_CHECK;
	hkVector4f_AdvancedInterface::unroll_store<N,A>::apply(m_quad, p);
}

template <int N, hkMathIoMode A, hkMathRoundingMode R> 
HK_FORCE_INLINE void hkVector4f::store(hkDouble64* p) const
{
	HK_VECTOR4f_UNSUPPORTED_LENGTH_CHECK;
	hkVector4f_AdvancedInterface::unroll_store_D<N,A>::apply(m_quad, p);
}

template <int N, hkMathIoMode A> 
HK_FORCE_INLINE void hkVector4f::store(hkFloat32* p) const
{
	HK_VECTOR4f_UNSUPPORTED_LENGTH_CHECK;
	hkVector4f_AdvancedInterface::unroll_store<N,A>::apply(m_quad, p);
}

template <int N, hkMathIoMode A> 
HK_FORCE_INLINE void hkVector4f::store(hkDouble64* p) const
{
	HK_VECTOR4f_UNSUPPORTED_LENGTH_CHECK;
	hkVector4f_AdvancedInterface::unroll_store_D<N,A>::apply(m_quad, p);
}

template <int N> 
HK_FORCE_INLINE void hkVector4f::store(hkFloat32* p) const
{
	HK_VECTOR4f_UNSUPPORTED_LENGTH_CHECK;
	hkVector4f_AdvancedInterface::unroll_store<N,HK_IO_SIMD_ALIGNED>::apply(m_quad, p);
}

template <int N> 
HK_FORCE_INLINE void hkVector4f::store(hkDouble64* p) const
{
	HK_VECTOR4f_UNSUPPORTED_LENGTH_CHECK;
	hkVector4f_AdvancedInterface::unroll_store_D<N,HK_IO_SIMD_ALIGNED>::apply(m_quad, p);
}



namespace hkVector4f_AdvancedInterface
{
template <int N, hkMathIoMode A, hkMathRoundingMode R>
struct unroll_storeH { HK_FORCE_INLINE static void apply(const hkQuadFloat32& self, hkHalf* HK_RESTRICT p)
{
	HK_VECTOR4f_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <int N, hkMathRoundingMode R>
struct unroll_storeH<N, HK_IO_NATIVE_ALIGNED, R> { HK_FORCE_INLINE static void apply(const hkQuadFloat32& self, hkHalf* HK_RESTRICT p)
{
	// neon does not align HK_MATH_ASSERT(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkHalf)-1) ) == 0, "pointer must be aligned to native size of hkHalf.");
	uint16x4_t r;
	if (R == HK_ROUND_NEAREST)
	{
		hkQuadFloat32 vs0 = vmulq_f32( self, g_vectorfConstants[HK_QUADREAL_PACK_HALF] );
		uint32x4_t vsi = vreinterpretq_u32_f32(vs0);
		r = vshrn_n_u32(vsi, 16);
	}
	else
	{
		uint32x4_t vsi = vreinterpretq_u32_f32(self);
		r = vshrn_n_u32(vsi, 16);
	}

	switch (N)
	{
		case 4:
			{
				vst1_u16( (hkUint16*)p, r );
			}
			break;
		case 3:
			{
				vst1_lane_u16( (hkUint16*)p, r, 0 );
				vst1_lane_u16( (hkUint16*)(p+1), r, 1 );
				vst1_lane_u16( (hkUint16*)(p+2), r, 2 );
			}
			break;
		case 2:
			{
				vst1_lane_u16( (hkUint16*)p, r, 0 );
				vst1_lane_u16( (hkUint16*)(p+1), r, 1 );
			}
			break;
		default:
			{
				vst1_lane_u16( (hkUint16*)p, r, 0 );
			}
			break;
	}
} };
template <int N, hkMathRoundingMode R>
struct unroll_storeH<N, HK_IO_SIMD_ALIGNED, R> { HK_FORCE_INLINE static void apply(const hkQuadFloat32& self, hkHalf* HK_RESTRICT p)
{
	// neon does not align HK_MATH_ASSERT(0x64211c2f, ( ((hkUlong)p) & ((sizeof(hkHalf)*(N!=3?N:4) )-1) ) == 0, "pointer must be aligned for SIMD.");
	unroll_storeH<N, HK_IO_NATIVE_ALIGNED, R>::apply(self,p);
} };
template <int N, hkMathRoundingMode R>
struct unroll_storeH<N, HK_IO_NOT_CACHED, R> { HK_FORCE_INLINE static void apply(const hkQuadFloat32& self, hkHalf* HK_RESTRICT p)
{
	unroll_storeH<N, HK_IO_SIMD_ALIGNED, R>::apply(self,p);
} };
} // namespace 

template <int N, hkMathIoMode A, hkMathRoundingMode R> 
HK_FORCE_INLINE void hkVector4f::store(hkHalf* p) const
{
	HK_VECTOR4f_UNSUPPORTED_LENGTH_CHECK;
	hkVector4f_AdvancedInterface::unroll_storeH<N,A,R>::apply(m_quad, p);
}

template <int N, hkMathIoMode A> 
HK_FORCE_INLINE void hkVector4f::store(hkHalf* p) const
{
	HK_VECTOR4f_UNSUPPORTED_LENGTH_CHECK;
	hkVector4f_AdvancedInterface::unroll_storeH<N,A,HK_ROUND_DEFAULT>::apply(m_quad, p);
}

template <int N> 
HK_FORCE_INLINE void hkVector4f::store(hkHalf* p) const
{
	HK_VECTOR4f_UNSUPPORTED_LENGTH_CHECK;
	hkVector4f_AdvancedInterface::unroll_storeH<N,HK_IO_SIMD_ALIGNED,HK_ROUND_DEFAULT>::apply(m_quad, p);
}


namespace hkVector4f_AdvancedInterface
{
template <int N, hkMathIoMode A, hkMathRoundingMode R>
struct unroll_storeF16 { HK_FORCE_INLINE static void apply(const hkQuadFloat32& self, hkFloat16* HK_RESTRICT p)
{
	HK_VECTOR4f_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };

#if defined(HK_PLATFORM_IOS) || defined(HK_PLATFORM_ANDROID) || ( defined(HK_COMPILER_MSVC) && defined(HK_ARCH_ARM) )
template <int N, hkMathRoundingMode R>
struct unroll_storeF16<N, HK_IO_NATIVE_ALIGNED, R> { HK_FORCE_INLINE static void apply(const hkQuadFloat32& self, hkFloat16* HK_RESTRICT p)
{
	float32_t f;
	switch (N)
	{
		case 4:  vst1q_lane_f32(&f, self, 3); p[3].setReal<(R == HK_ROUND_NEAREST)>(f);
		case 3:  vst1q_lane_f32(&f, self, 2); p[2].setReal<(R == HK_ROUND_NEAREST)>(f);
		case 2:  vst1q_lane_f32(&f, self, 1); p[1].setReal<(R == HK_ROUND_NEAREST)>(f);
		case 1:  vst1q_lane_f32(&f, self, 0); p[0].setReal<(R == HK_ROUND_NEAREST)>(f); break;
	}
} };

#else

template <int N, hkMathRoundingMode R>
struct unroll_storeF16<N, HK_IO_NATIVE_ALIGNED, R> { HK_FORCE_INLINE static void apply(const hkQuadFloat32& self, hkFloat16* HK_RESTRICT p)
{
	// neon does not align HK_MATH_ASSERT(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkFloat16)-1) ) == 0, "pointer must be aligned to native size of hkFloat16.");
	float16x4_t r = vcvt_f16_f32(self);

	switch (N)
	{
	case 4:
		{
			vst1_f16( (__fp16*)p, r );
		}
		break;
	case 3:
		{
			vst1_lane_f16( (__fp16*)p, r, 0 );
			vst1_lane_f16( (__fp16*)(p+1), r, 1 );
			vst1_lane_f16( (__fp16*)(p+2), r, 2 );
		}
		break;
	case 2:
		{
			vst1_lane_f16( (__fp16*)p, r, 0 );
			vst1_lane_f16( (__fp16*)(p+1), r, 1 );
		}
		break;
	default:
		{
			vst1_lane_f16( (__fp16*)p, r, 0 );
		}
		break;
	}
} };
#endif

template <int N, hkMathRoundingMode R>
struct unroll_storeF16<N, HK_IO_SIMD_ALIGNED, R> { HK_FORCE_INLINE static void apply(const hkQuadFloat32& self, hkFloat16* HK_RESTRICT p)
{
	// neon does not align HK_MATH_ASSERT(0x64211c2f, ( ((hkUlong)p) & ((sizeof(hkFloat16)*(N!=3?N:4) )-1) ) == 0, "pointer must be aligned for SIMD.");
	unroll_storeF16<N, HK_IO_NATIVE_ALIGNED, R>::apply(self,p);
} };
template <int N, hkMathRoundingMode R>
struct unroll_storeF16<N, HK_IO_NOT_CACHED, R> { HK_FORCE_INLINE static void apply(const hkQuadFloat32& self, hkFloat16* HK_RESTRICT p)
{
	unroll_storeF16<N, HK_IO_SIMD_ALIGNED, R>::apply(self,p);
} };
} // namespace 

template <int N, hkMathIoMode A, hkMathRoundingMode R> 
HK_FORCE_INLINE void hkVector4f::store(hkFloat16* p) const
{
	HK_VECTOR4f_UNSUPPORTED_LENGTH_CHECK;
	hkVector4f_AdvancedInterface::unroll_storeF16<N,A,R>::apply(m_quad, p);
}

template <int N, hkMathIoMode A> 
HK_FORCE_INLINE void hkVector4f::store(hkFloat16* p) const
{
	HK_VECTOR4f_UNSUPPORTED_LENGTH_CHECK;
	hkVector4f_AdvancedInterface::unroll_storeF16<N,A,HK_ROUND_DEFAULT>::apply(m_quad, p);
}

template <int N> 
HK_FORCE_INLINE void hkVector4f::store(hkFloat16* p) const
{
	HK_VECTOR4f_UNSUPPORTED_LENGTH_CHECK;
	hkVector4f_AdvancedInterface::unroll_storeF16<N,HK_IO_SIMD_ALIGNED,HK_ROUND_DEFAULT>::apply(m_quad, p);
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
