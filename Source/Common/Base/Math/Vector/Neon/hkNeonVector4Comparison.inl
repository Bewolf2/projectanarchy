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

HK_FORCE_INLINE /*static*/ const hkVector4fComparison HK_CALL hkVector4fComparison::convert(const hkVector4fMask& x)
{
	hkVector4fComparison c;
	c.m_mask = x;
	return c;
}

HK_FORCE_INLINE void hkVector4fComparison::setAnd( hkVector4fComparisonParameter a, hkVector4fComparisonParameter b )
{ 
	m_mask = vandq_u32( a.m_mask,b.m_mask ); 
}

HK_FORCE_INLINE void hkVector4fComparison::setAndNot( hkVector4fComparisonParameter a, hkVector4fComparisonParameter b )
{
	m_mask = vbicq_u32( a.m_mask, b.m_mask ); // a & !b (so bit clear and set bits in b)
}

HK_FORCE_INLINE void hkVector4fComparison::setXor( hkVector4fComparisonParameter a, hkVector4fComparisonParameter b ) 
{ 
	m_mask = veorq_u32( a.m_mask,b.m_mask );
}

HK_FORCE_INLINE void hkVector4fComparison::setNot( hkVector4fComparisonParameter a )
{ 
	m_mask = vmvnq_u32(a.m_mask);
}

HK_FORCE_INLINE void hkVector4fComparison::setOr( hkVector4fComparisonParameter a, hkVector4fComparisonParameter b ) 
{ 
	m_mask = vorrq_u32( a.m_mask,b.m_mask );
}

HK_FORCE_INLINE void hkVector4fComparison::setSelect( hkVector4fComparisonParameter comp, hkVector4fComparisonParameter trueValue, hkVector4fComparisonParameter falseValue ) 
{
	m_mask = vbslq_u32( comp.m_mask, trueValue.m_mask, falseValue.m_mask );
}

template <> HK_FORCE_INLINE void hkVector4fComparison::setSelect<hkVector4ComparisonMask::MASK_X>(hkVector4fComparisonParameter trueValue, hkVector4fComparisonParameter falseValue)
{
	m_mask = vsetq_lane_u32(vgetq_lane_u32(trueValue.m_mask, 0), falseValue.m_mask, 0);
}

template <> HK_FORCE_INLINE void hkVector4fComparison::setSelect<hkVector4ComparisonMask::MASK_XY>(hkVector4fComparisonParameter trueValue, hkVector4fComparisonParameter falseValue)
{
	m_mask = vcombine_u32(vget_low_u32(trueValue.m_mask), vget_high_u32(falseValue.m_mask));
}

template <> HK_FORCE_INLINE void hkVector4fComparison::setSelect<hkVector4ComparisonMask::MASK_XYZ>(hkVector4fComparisonParameter trueValue, hkVector4fComparisonParameter falseValue)
{
	m_mask = vsetq_lane_u32(vgetq_lane_u32(falseValue.m_mask, 3), trueValue.m_mask, 3);
}

template<hkVector4ComparisonMask::Mask M> 
HK_FORCE_INLINE void hkVector4fComparison::setSelect( hkVector4fComparisonParameter trueValue, hkVector4fComparisonParameter falseValue )
{
	HK_VECTORfCOMPARISON_MASK_CHECK;
	hkVector4fComparison cmp;	cmp.set<M>();
	setSelect(cmp, trueValue, falseValue);
}

HK_FORCE_INLINE void hkVector4fComparison::set( Mask m ) 
{ 
	HK_COMPILE_TIME_ASSERT( hkVector4ComparisonMask::INDEX_X == 0);
	HK_COMPILE_TIME_ASSERT( hkVector4ComparisonMask::MASK_X == 1);
	HK_COMPILE_TIME_ASSERT( hkVector4ComparisonMask::MASK_Y == 2);
	HK_COMPILE_TIME_ASSERT( hkVector4ComparisonMask::MASK_XYZW == 15);

	HK_MATH_ASSERT(0x557dac2a, (m&0xf)==m, "illegal mask value handed in");

	static uint32x4_t maskToComparison[16] = 
	{
		HK_NEON_CONSTANT4I(0,  0, 0, 0),
		HK_NEON_CONSTANT4I(~0,  0, 0, 0),
		HK_NEON_CONSTANT4I(0, ~0, 0, 0),
		HK_NEON_CONSTANT4I(~0, ~0, 0, 0),
		HK_NEON_CONSTANT4I(0,  0,~0, 0),
		HK_NEON_CONSTANT4I(~0,  0,~0, 0),
		HK_NEON_CONSTANT4I(0, ~0,~0, 0),
		HK_NEON_CONSTANT4I(~0, ~0,~0, 0),
		HK_NEON_CONSTANT4I(0,  0, 0,~0),
		HK_NEON_CONSTANT4I(~0,  0, 0,~0),
		HK_NEON_CONSTANT4I(0, ~0, 0,~0),
		HK_NEON_CONSTANT4I(~0, ~0, 0,~0),
		HK_NEON_CONSTANT4I(0,  0,~0,~0),
		HK_NEON_CONSTANT4I(~0,  0,~0,~0),
		HK_NEON_CONSTANT4I(0, ~0,~0,~0),
		HK_NEON_CONSTANT4I(~0, ~0,~0,~0),
	};

	m_mask = maskToComparison[m];
}

template <hkVector4ComparisonMask::Mask M>
HK_FORCE_INLINE void hkVector4fComparison::set()
{ 
	HK_VECTORfCOMPARISON_MASK_CHECK;

	static uint32x4_t maskToComparison[16] = 
	{
		HK_NEON_CONSTANT4I(0,  0, 0, 0),
		HK_NEON_CONSTANT4I(~0,  0, 0, 0),
		HK_NEON_CONSTANT4I(0, ~0, 0, 0),
		HK_NEON_CONSTANT4I(~0, ~0, 0, 0),
		HK_NEON_CONSTANT4I(0,  0,~0, 0),
		HK_NEON_CONSTANT4I(~0,  0,~0, 0),
		HK_NEON_CONSTANT4I(0, ~0,~0, 0),
		HK_NEON_CONSTANT4I(~0, ~0,~0, 0),
		HK_NEON_CONSTANT4I(0,  0, 0,~0),
		HK_NEON_CONSTANT4I(~0,  0, 0,~0),
		HK_NEON_CONSTANT4I(0, ~0, 0,~0),
		HK_NEON_CONSTANT4I(~0, ~0, 0,~0),
		HK_NEON_CONSTANT4I(0,  0,~0,~0),
		HK_NEON_CONSTANT4I(~0,  0,~0,~0),
		HK_NEON_CONSTANT4I(0, ~0,~0,~0),
		HK_NEON_CONSTANT4I(~0, ~0,~0,~0),
	};

	m_mask = maskToComparison[M];
}

template <hkVector4ComparisonMask::Mask M>
HK_FORCE_INLINE hkBool32 hkVector4fComparison::allAreSet() const 
{ 
	HK_VECTORfCOMPARISON_MASK_CHECK;

	// if we use vcgt or similar here we will need to do the reduction to a bool, so easier to just test directly 
	return  ( ((M & hkVector4ComparisonMask::MASK_X) == 0) || vgetq_lane_u32(m_mask, 0) > 0 ) &&
			( ((M & hkVector4ComparisonMask::MASK_Y) == 0) || vgetq_lane_u32(m_mask, 1) > 0 ) &&
			( ((M & hkVector4ComparisonMask::MASK_Z) == 0) || vgetq_lane_u32(m_mask, 2) > 0 ) &&
			( ((M & hkVector4ComparisonMask::MASK_W) == 0) || vgetq_lane_u32(m_mask, 3) > 0 ); 
}

template <hkVector4ComparisonMask::Mask M>
HK_FORCE_INLINE hkBool32 hkVector4fComparison::anyIsSet() const 
{ 
	HK_VECTORfCOMPARISON_MASK_CHECK;
	
	// if we use vcgt or similar here we will need to do the reduction to a bool, so easier to just test directly 
	return  ( (M & hkVector4ComparisonMask::MASK_X) && vgetq_lane_u32(m_mask, 0) > 0 ) ||
			( (M & hkVector4ComparisonMask::MASK_Y) && vgetq_lane_u32(m_mask, 1) > 0 ) ||
			( (M & hkVector4ComparisonMask::MASK_Z) && vgetq_lane_u32(m_mask, 2) > 0 ) ||
			( (M & hkVector4ComparisonMask::MASK_W) && vgetq_lane_u32(m_mask, 3) > 0 ); 
}

HK_FORCE_INLINE hkBool32 hkVector4fComparison::allAreSet( Mask m ) const 
{ 
	HK_MATH_ASSERT(0x71b4006b, (m&0xf)==m, "illegal mask value handed in");

	static uint32x4_t maskToComparison[16] = 
	{
		HK_NEON_CONSTANT4I(~0,  ~0, ~0, ~0),
		HK_NEON_CONSTANT4I(0,  ~0, ~0, ~0),
		HK_NEON_CONSTANT4I(~0, 0, ~0, ~0),
		HK_NEON_CONSTANT4I(0, 0, ~0, ~0),
		HK_NEON_CONSTANT4I(~0,  ~0,0, ~0),
		HK_NEON_CONSTANT4I(0,  ~0,0, ~0),
		HK_NEON_CONSTANT4I(~0, 0,0, ~0),
		HK_NEON_CONSTANT4I(0, 0,0, ~0),
		HK_NEON_CONSTANT4I(~0,  ~0, ~0,0),
		HK_NEON_CONSTANT4I(0,  ~0, ~0,0),
		HK_NEON_CONSTANT4I(~0, 0, ~0,0),
		HK_NEON_CONSTANT4I(0, 0, ~0,0),
		HK_NEON_CONSTANT4I(~0,  ~0,0,0),
		HK_NEON_CONSTANT4I(0,  ~0,0,0),
		HK_NEON_CONSTANT4I(~0, 0,0,0),
		HK_NEON_CONSTANT4I(0, 0,0,0),
	};

	uint32x4_t bits = vorrq_u32( maskToComparison[m], m_mask ); // should all be 1
	// if we use vcgt or similar here we will need to do the reduction to a bool, so easier to just test directly 
	return  ( vgetq_lane_u32(bits, 0) > 0 ) &&
		( vgetq_lane_u32(bits, 1) > 0 ) &&
		( vgetq_lane_u32(bits, 2) > 0 ) &&
		( vgetq_lane_u32(bits, 3) > 0 ); 
}

HK_FORCE_INLINE hkBool32 hkVector4fComparison::anyIsSet( Mask m ) const 
{ 
	HK_MATH_ASSERT(0x68b44aee, (m&0xf)==m, "illegal mask value handed in");

	static uint32x4_t maskToComparison[16] = 
	{
		HK_NEON_CONSTANT4I(0,  0, 0, 0),
		HK_NEON_CONSTANT4I(~0,  0, 0, 0),
		HK_NEON_CONSTANT4I(0, ~0, 0, 0),
		HK_NEON_CONSTANT4I(~0, ~0, 0, 0),
		HK_NEON_CONSTANT4I(0,  0,~0, 0),
		HK_NEON_CONSTANT4I(~0,  0,~0, 0),
		HK_NEON_CONSTANT4I(0, ~0,~0, 0),
		HK_NEON_CONSTANT4I(~0, ~0,~0, 0),
		HK_NEON_CONSTANT4I(0,  0, 0,~0),
		HK_NEON_CONSTANT4I(~0,  0, 0,~0),
		HK_NEON_CONSTANT4I(0, ~0, 0,~0),
		HK_NEON_CONSTANT4I(~0, ~0, 0,~0),
		HK_NEON_CONSTANT4I(0,  0,~0,~0),
		HK_NEON_CONSTANT4I(~0,  0,~0,~0),
		HK_NEON_CONSTANT4I(0, ~0,~0,~0),
		HK_NEON_CONSTANT4I(~0, ~0,~0,~0),
	};

	uint32x4_t bits = vandq_u32( maskToComparison[m], m_mask ); // should all some 1s
	// if we use vcgt or similar here we will need to do the reduction to a bool, so easier to just test directly 
	return  ( vgetq_lane_u32(bits, 0) > 0 ) ||
		( vgetq_lane_u32(bits, 1) > 0 ) ||
		( vgetq_lane_u32(bits, 2) > 0 ) ||
		( vgetq_lane_u32(bits, 3) > 0 ); 
}

HK_FORCE_INLINE hkBool32 hkVector4fComparison::allAreSet() const 
{ 
	return ( vgetq_lane_u32(m_mask, 0) > 0 ) &&
		( vgetq_lane_u32(m_mask, 1) > 0 ) &&
		( vgetq_lane_u32(m_mask, 2) > 0 ) &&
		( vgetq_lane_u32(m_mask, 3) > 0 ); 
}

HK_FORCE_INLINE hkBool32 hkVector4fComparison::anyIsSet() const 
{ 
	return ( vgetq_lane_u32(m_mask, 0) > 0 ) ||
		( vgetq_lane_u32(m_mask, 1) > 0 ) ||
		( vgetq_lane_u32(m_mask, 2) > 0 ) ||
		( vgetq_lane_u32(m_mask, 3) > 0 ); 
}

HK_FORCE_INLINE hkVector4ComparisonMask::Mask hkVector4fComparison::getMask() const 
{ 
    uint32x4_t s = HK_NEON_CONSTANT4I(1,2,4,8);
	uint32x4_t spread = vandq_u32( m_mask, s ); 
	uint32x2_t l = vget_low_u32(spread);
	uint32x2_t h = vget_high_u32(spread);
	uint32x2_t p01_23 = vpadd_u32(l, h); // 0+1,2+3
	uint32x2_t p0123 = vpadd_u32(p01_23, p01_23); // (0+1)+(2+3)

	return (hkVector4ComparisonMask::Mask)vget_lane_u32(p0123, 0);
}

HK_FORCE_INLINE hkVector4ComparisonMask::Mask hkVector4fComparison::getMask(Mask m) const 
{ 
	HK_MATH_ASSERT(0x3d2fea61, (m&0xf)==m, "illegal mask value handed in");
	return (hkVector4ComparisonMask::Mask)(getMask() & m);
}

template <hkVector4ComparisonMask::Mask M>
HK_FORCE_INLINE hkVector4ComparisonMask::Mask hkVector4fComparison::getMask() const 
{ 
	HK_VECTORfCOMPARISON_MASK_CHECK;
	return (hkVector4ComparisonMask::Mask)(getMask() & M);
}

HK_FORCE_INLINE /*static*/ hkUint32 HK_CALL hkVector4fComparison::getCombinedMask(hkVector4fComparisonParameter ca, hkVector4fComparisonParameter cb, hkVector4fComparisonParameter cc )
{
	//TODO: opt if possible
	int ma = ca.getMask();
	int mb = cb.getMask();
	int mc = cc.getMask();
	return ma | (mb << 4) | (mc << 8);
}

//
//	Horizontal or

template <>
HK_FORCE_INLINE const hkVector4fComparison hkVector4fComparison::horizontalOr<1>() const
{
	uint32x2_t xy		= vget_low_u32(m_mask);		// [x, y]

	return hkVector4fComparison::convert( vdupq_lane_u32(xy, 0) );
}

template <>
HK_FORCE_INLINE const hkVector4fComparison hkVector4fComparison::horizontalOr<2>() const
{
	uint32x2_t xy		= vget_low_u32(m_mask);		// [x, y]
	uint32x2_t yx		= vrev64_u32(xy);			// [y, x]
	uint32x2_t xy_xy	= vorr_u32(xy, yx);			// [x | y, x | y]

	return hkVector4fComparison::convert( vdupq_lane_u32(xy_xy, 0) );
}

template <>
HK_FORCE_INLINE const hkVector4fComparison hkVector4fComparison::horizontalOr<3>() const
{
	uint32x2_t xy		= vget_low_u32(m_mask);		// [x, y]
	uint32x2_t yx		= vrev64_u32(xy);			// [y, x]
	uint32x2_t xy_xy	= vorr_u32(xy, yx);			// [x | y, x | y]
	uint32x2_t zw		= vget_high_u32(m_mask);	// [z, w]
	uint32x2_t zz		= vdup_lane_u32(zw, 0);		// [z, z]
	uint32x2_t xyz_xyz	= vorr_u32(xy_xy, zz);		// [x | y | z, x | y | z]
	
	return hkVector4fComparison::convert( vdupq_lane_u32(xyz_xyz, 0) );
}

template <>
HK_FORCE_INLINE const hkVector4fComparison hkVector4fComparison::horizontalOr<4>() const
{
	uint32x2_t lo		= vget_low_u32(m_mask);		// [x, y]
	uint32x2_t hi		= vget_high_u32(m_mask);	// [z, w]
	uint32x2_t xz_yw	= vorr_u32(lo, hi);			// [x | z, y | w]
	uint32x2_t yw_xz	= vrev64_u32(xz_yw);		// [y | w, x | z]
	uint32x2_t xyzw		= vorr_u32(xz_yw, yw_xz);	// [x | y | z | w, x | y | z | w]

	return hkVector4fComparison::convert( vdupq_lane_u32(xyzw, 0) );
}

template <int N>
HK_FORCE_INLINE const hkVector4fComparison hkVector4fComparison::horizontalOr() const
{
	HK_VECTORfCOMPARISON_SUBINDEX_CHECK;
	hkVector4fComparison cmp;
	cmp.set<hkVector4ComparisonMask::MASK_NONE>();
	return cmp;
}

//
//	Horizontal and

template <> HK_FORCE_INLINE const hkVector4fComparison hkVector4fComparison::horizontalAnd<1>() const
{
	uint32x2_t xy		= vget_low_u32(m_mask);		// [x, y]
	return hkVector4fComparison::convert( vdupq_lane_u32(xy, 0) );
}

template <> HK_FORCE_INLINE const hkVector4fComparison hkVector4fComparison::horizontalAnd<2>() const
{
	uint32x2_t xy		= vget_low_u32(m_mask);		// [x, y]
	uint32x2_t yx		= vrev64_u32(xy);			// [y, x]
	uint32x2_t xy_xy	= vand_u32(xy, yx);			// [x & y, x & y]

	return hkVector4fComparison::convert( vdupq_lane_u32(xy_xy, 0) );
}

template <> HK_FORCE_INLINE const hkVector4fComparison hkVector4fComparison::horizontalAnd<3>() const
{
	uint32x2_t xy		= vget_low_u32(m_mask);		// [x, y]
	uint32x2_t yx		= vrev64_u32(xy);			// [y, x]
	uint32x2_t xy_xy	= vand_u32(xy, yx);			// [x & y, x & y]
	uint32x2_t zw		= vget_high_u32(m_mask);	// [z, w]
	uint32x2_t zz		= vdup_lane_u32(zw, 0);		// [z, z]
	uint32x2_t xyz_xyz	= vand_u32(xy_xy, zz);		// [x & y & z, x & y & z]

	return hkVector4fComparison::convert( vdupq_lane_u32(xyz_xyz, 0) );
}

template <> HK_FORCE_INLINE const hkVector4fComparison hkVector4fComparison::horizontalAnd<4>() const
{
	uint32x2_t lo		= vget_low_u32(m_mask);		// [x, y]
	uint32x2_t hi		= vget_high_u32(m_mask);	// [z, w]
	uint32x2_t xz_yw	= vand_u32(lo, hi);			// [x & z, y & w]
	uint32x2_t yw_xz	= vrev64_u32(xz_yw);		// [y & w, x & z]
	uint32x2_t xyzw		= vand_u32(xz_yw, yw_xz);	// [x & y & z & w, x & y & z & w]

	return hkVector4fComparison::convert( vdupq_lane_u32(xyzw, 0) );
}

template <int N> HK_FORCE_INLINE const hkVector4fComparison hkVector4fComparison::horizontalAnd() const
{
	HK_VECTORfCOMPARISON_SUBINDEX_CHECK;
	hkVector4fComparison cmp;
	cmp.set<hkVector4ComparisonMask::MASK_NONE>();
	return cmp;
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
