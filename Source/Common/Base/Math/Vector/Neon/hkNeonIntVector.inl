/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// Features available in Neon:
#define HK_INT_VECTOR_NATIVE_MISALIGNED
#define HK_INT_VECTOR_NATIVE_LOGIC
#define HK_INT_VECTOR_NATIVE_ARITH
#define HK_INT_VECTOR_NATIVE_FIXEDSHIFT
#define HK_INT_VECTOR_NATIVE_BYTESHIFT128
// #define HK_INT_VECTOR_NATIVE_BITSHIFT128 // No whole-vector bitshifting in Neon
#define HK_INT_VECTOR_NATIVE_VARIABLESHIFT
#define HK_INT_VECTOR_NATIVE_MERGEPACK
#define HK_INT_VECTOR_NATIVE_SHUFFLE
#define HK_INT_VECTOR_NATIVE_VPERM
#define HK_INT_VECTOR_NATIVE_SPLAT
#define HK_INT_VECTOR_NATIVE_FLOATCONV

#define HK_INT_VECTOR_loadAsFloat32BitRepresentation_f
HK_FORCE_INLINE void hkIntVector::loadAsFloat32BitRepresentation(const hkVector4f& x)
{
	this->m_quad = vreinterpretq_u32_f32(x.m_quad);
}

HK_FORCE_INLINE void hkIntVector::storeAsFloat32BitRepresentation( hkVector4f& out ) const
{
	out.m_quad = vreinterpretq_f32_u32(this->m_quad);
}

//
// Clearing, loading, and storing
//

HK_FORCE_INLINE void hkIntVector::setZero()
{
	m_quad = vmovq_n_u32(0);
}

HK_FORCE_INLINE void hkIntVector::setAll(const int& i)
{
	m_quad = vreinterpretq_u32_s32( vdupq_n_s32(i) );
}

HK_FORCE_INLINE void hkIntVector::setFirstComponent(int value)
{
	int32x4_t v;
	v = vmovq_n_s32(0);
	v = vsetq_lane_s32(value, v, 0);
	m_quad = vreinterpretq_u32_s32( v );
}

template <int I>
HK_FORCE_INLINE void hkIntVector::setComponent(int value)
{
	HK_INT_VECTOR_SUBINDEX_CHECK;
    // Add Switch to fix Clang issue were last parameter of vsetq_lane_32 must be in constrange(0,3)
    // See RSYS-1377 for more information
    switch(I)
    {
        case 1:  m_quad = vreinterpretq_u32_s32( vsetq_lane_s32(value, vreinterpretq_s32_u32(m_quad), 1) ); break;
        case 2:  m_quad = vreinterpretq_u32_s32( vsetq_lane_s32(value, vreinterpretq_s32_u32(m_quad), 2) ); break;
        case 3:  m_quad = vreinterpretq_u32_s32( vsetq_lane_s32(value, vreinterpretq_s32_u32(m_quad), 3) ); break;
        default:  m_quad = vreinterpretq_u32_s32( vsetq_lane_s32(value, vreinterpretq_s32_u32(m_quad), 0) ); break;
    }
}

HK_FORCE_INLINE void hkIntVector::setComponent(int I, int value)
{
	switch(I)
	{
	case 1:  m_quad = vreinterpretq_u32_s32( vsetq_lane_s32(value, vreinterpretq_s32_u32(m_quad), 1) ); break;
	case 2:  m_quad = vreinterpretq_u32_s32( vsetq_lane_s32(value, vreinterpretq_s32_u32(m_quad), 2) ); break;
	case 3:  m_quad = vreinterpretq_u32_s32( vsetq_lane_s32(value, vreinterpretq_s32_u32(m_quad), 3) ); break;
	default:  m_quad = vreinterpretq_u32_s32( vsetq_lane_s32(value, vreinterpretq_s32_u32(m_quad), 0) ); break;
	}
}


HK_FORCE_INLINE void hkIntVector::set(int x, int y, int z, int w)
{
	int32x4_t v;
	v = vmovq_n_s32(x);
	v = vsetq_lane_s32(y, v, 1);
	v = vsetq_lane_s32(z, v, 2);
	v = vsetq_lane_s32(w, v, 3);
	m_quad = vreinterpretq_u32_s32( v );
}

template <int I>
HK_FORCE_INLINE int hkIntVector::getComponent() const
{
    // Add Switch to fix Clang issue were last parameter of vgetq_lane_32 must be in constrange(0,3)
    // See RSYS-1377 for more information
    switch(I)
    {
        case 1:  return vgetq_lane_s32( vreinterpretq_s32_u32(m_quad), 1 );
        case 2:  return vgetq_lane_s32( vreinterpretq_s32_u32(m_quad), 2 );
        case 3:  return vgetq_lane_s32( vreinterpretq_s32_u32(m_quad), 3 );
        default:  return vgetq_lane_s32( vreinterpretq_s32_u32(m_quad), 0 );
    }
}

HK_FORCE_INLINE int hkIntVector::getComponent(int I) const
{
	// Add Switch to fix Clang issue were last parameter of vgetq_lane_32 must be in constrange(0,3)
	// See RSYS-1377 for more information
	switch(I)
	{
	case 1:  return vgetq_lane_s32( vreinterpretq_s32_u32(m_quad), 1 );
	case 2:  return vgetq_lane_s32( vreinterpretq_s32_u32(m_quad), 2 );
	case 3:  return vgetq_lane_s32( vreinterpretq_s32_u32(m_quad), 3 );
	default:  return vgetq_lane_s32( vreinterpretq_s32_u32(m_quad), 0 );
	}
}

template <int I>
HK_FORCE_INLINE hkUint8  hkIntVector::getU8 () const 
{
	HK_COMPILE_TIME_ASSERT2((I>=0)&&(I<16), HK_INT_VECTOR_SUBVECTOR_INDEX_OUT_OF_RANGE);
    // Add Switch to fix Clang issue were last parameter of vgetq_lane_u8 must be in constrange(0,15)
    // See RSYS-1377 for more information
    switch(I)
    {
        case 1:  return vgetq_lane_u8( vreinterpretq_u8_u32(m_quad), 1 );
        case 2:  return vgetq_lane_u8( vreinterpretq_u8_u32(m_quad), 2 );
        case 3:  return vgetq_lane_u8( vreinterpretq_u8_u32(m_quad), 3 );
        case 4:  return vgetq_lane_u8( vreinterpretq_u8_u32(m_quad), 4 );
        case 5:  return vgetq_lane_u8( vreinterpretq_u8_u32(m_quad), 5 );
        case 6:  return vgetq_lane_u8( vreinterpretq_u8_u32(m_quad), 6 );
        case 7:  return vgetq_lane_u8( vreinterpretq_u8_u32(m_quad), 7 );
        case 8:  return vgetq_lane_u8( vreinterpretq_u8_u32(m_quad), 8 );
        case 9:  return vgetq_lane_u8( vreinterpretq_u8_u32(m_quad), 9 );
        case 10: return vgetq_lane_u8( vreinterpretq_u8_u32(m_quad), 10 );
        case 11: return vgetq_lane_u8( vreinterpretq_u8_u32(m_quad), 11 );
        case 12: return vgetq_lane_u8( vreinterpretq_u8_u32(m_quad), 12 );
        case 13: return vgetq_lane_u8( vreinterpretq_u8_u32(m_quad), 13 );
        case 14: return vgetq_lane_u8( vreinterpretq_u8_u32(m_quad), 14 );
        case 15: return vgetq_lane_u8( vreinterpretq_u8_u32(m_quad), 15 );
        default: return vgetq_lane_u8( vreinterpretq_u8_u32(m_quad), 0 );
    }
}

template <int I>
HK_FORCE_INLINE hkUint16 hkIntVector::getU16() const 
{
	HK_COMPILE_TIME_ASSERT2((I>=0)&&(I<8), HK_INT_VECTOR_SUBVECTOR_INDEX_OUT_OF_RANGE);
    // Add Switch to fix Clang issue were last parameter of vgetq_lane_u16 must be in constrange(0,7)
    // See RSYS-1377 for more information
    switch(I)
    {
        case 1: return vgetq_lane_u16( vreinterpretq_u16_u32(m_quad), 1 );
        case 2: return vgetq_lane_u16( vreinterpretq_u16_u32(m_quad), 2 );
        case 3: return vgetq_lane_u16( vreinterpretq_u16_u32(m_quad), 3 );
        case 4: return vgetq_lane_u16( vreinterpretq_u16_u32(m_quad), 4 );
        case 5: return vgetq_lane_u16( vreinterpretq_u16_u32(m_quad), 5 );
        case 6: return vgetq_lane_u16( vreinterpretq_u16_u32(m_quad), 6 );
        case 7: return vgetq_lane_u16( vreinterpretq_u16_u32(m_quad), 7 );
        default: return vgetq_lane_u16( vreinterpretq_u16_u32(m_quad), 0 );
    }
}

template <int I>
HK_FORCE_INLINE hkUint32 hkIntVector::getU32() const
{
    // Add Switch to fix Clang issue were last parameter of vgetq_lane_u32 must be in constrange(0,3)
    // See RSYS-1377 for more information
    switch(I)
    {
        case 1:  return vgetq_lane_u32( m_quad, 1 );
        case 2:  return vgetq_lane_u32( m_quad, 2 );
        case 3:  return vgetq_lane_u32( m_quad, 3 );
        default: return vgetq_lane_u32( m_quad, 0 );
    }
}

HK_FORCE_INLINE hkUint32 hkIntVector::getU32(int idx) const
{
	HK_MATH_ASSERT(0xfabb2300, idx>=0 && idx<4, "int vector subindex out of range");
	switch( idx )
	{
	case 0:	return getU32<0>();
	case 1:	return getU32<1>();
	case 2:	return getU32<2>();
	case 3:	return getU32<3>();
	}
	return 0;
}

// We need to use u8 loads and stores here as u32 assumes a 4-byte aligned pointer
template <>
HK_FORCE_INLINE void hkIntVector::loadNotAligned<4>(const HK_INT_VECTOR_UNALIGNED_PTR hkUint32* p)
{
	m_quad = vreinterpretq_u32_u8(vld1q_u8( (const unsigned char*)p ));
}

template <>
HK_FORCE_INLINE void hkIntVector::loadNotAligned<3>(const HK_INT_VECTOR_UNALIGNED_PTR hkUint32* p)
{
	uint32x4_t tmp = vreinterpretq_u32_u8(vld1q_u8((const unsigned char*)p));
	m_quad = vsetq_lane_u32( vgetq_lane_u32(m_quad, 3), tmp, 3);
}

template <>
HK_FORCE_INLINE void hkIntVector::loadNotAligned<2>(const HK_INT_VECTOR_UNALIGNED_PTR hkUint32* p)
{
	uint32x4_t tmp = vreinterpretq_u32_u8(vld1q_u8((const unsigned char*)p));
	m_quad = vcombine_u32(vget_low_u32(tmp), vget_high_u32(m_quad));
}

template <>
HK_FORCE_INLINE void hkIntVector::loadNotAligned<1>(const HK_INT_VECTOR_UNALIGNED_PTR hkUint32* p)
{
	uint32x4_t tmp = vreinterpretq_u32_u8(vld1q_u8((const unsigned char*)p));
	m_quad = vsetq_lane_u32( vgetq_lane_u32(tmp, 0), m_quad, 0);
}

template <int N>
HK_FORCE_INLINE void hkIntVector::loadNotAligned(const HK_INT_VECTOR_UNALIGNED_PTR hkUint32* p)
{
	HK_INT_VECTOR_NOT_IMPLEMENTED;
}

template <>
HK_FORCE_INLINE void hkIntVector::storeNotAligned<1>(hkUint32* p) const
{
	uint32x4_t tmp = vreinterpretq_u32_u8(vld1q_u8((const unsigned char*)p));
	tmp = vsetq_lane_u32( vgetq_lane_u32(m_quad, 0), tmp, 0);
	vst1q_u8( (unsigned char*)p, vreinterpretq_u8_u32(tmp) );
}

template <>
HK_FORCE_INLINE void hkIntVector::storeNotAligned<2>(hkUint32* p) const
{
	uint32x4_t tmp = vreinterpretq_u32_u8(vld1q_u8((const unsigned char*)p));
	tmp = vcombine_u32(vget_low_u32(m_quad), vget_high_u32(tmp));
	vst1q_u8( (unsigned char*)p, vreinterpretq_u8_u32(tmp) );
}

template <>
HK_FORCE_INLINE void hkIntVector::storeNotAligned<3>(hkUint32* p) const
{
	uint32x4_t tmp = vreinterpretq_u32_u8(vld1q_u8((const unsigned char*)p));
	tmp = vsetq_lane_u32( vgetq_lane_u32(tmp, 3), m_quad, 3);
	vst1q_u8( (unsigned char*)p, vreinterpretq_u8_u32(tmp) );
}

template <>
HK_FORCE_INLINE void hkIntVector::storeNotAligned<4>(hkUint32* p) const
{
	vst1q_u8( (unsigned char*)p, vreinterpretq_u8_u32(m_quad) );
}

template <int N>
HK_FORCE_INLINE void hkIntVector::storeNotAligned(hkUint32* p) const
{
	HK_INT_VECTOR_NOT_IMPLEMENTED;
}

//
// Logical operations
//

HK_FORCE_INLINE hkBool32 hkIntVector::isNegativeAssumingAllValuesEqual(  ) const
{
	HK_ON_DEBUG( hkQuadUintUnion qu; qu.q = m_quad;)
	HK_MATH_ASSERT(0x252d00fa, qu.u[0] == qu.u[1] && qu.u[0] == qu.u[2] && qu.u[0] == qu.u[3], "assumption all values equal failed");

	uint32x4_t c = vcltq_s32( vreinterpretq_s32_u32(m_quad), vmovq_n_s32(0) );
	return vgetq_lane_u32(c, 0);
}

HK_FORCE_INLINE const hkVector4Comparison hkIntVector::compareLessThanS32( hkIntVectorParameter b ) const
{
	hkVector4Comparison c;
	c.m_mask = vcltq_s32( vreinterpretq_s32_u32(m_quad), vreinterpretq_s32_u32(b.m_quad) );
	return c;
}

HK_FORCE_INLINE const hkVector4Comparison hkIntVector::compareEqualS32(hkIntVectorParameter b ) const
{
	hkVector4Comparison c;
	c.m_mask = vceqq_s32( vreinterpretq_s32_u32(m_quad), vreinterpretq_s32_u32(b.m_quad) );
	return c;
}

HK_FORCE_INLINE const hkVector4Comparison hkIntVector::lessZeroS32() const
{
	hkVector4Comparison c;
	c.m_mask = vcltq_s32(vreinterpretq_s32_u32(m_quad), vdupq_n_s32(0));
	return c;
}

HK_FORCE_INLINE const hkVector4Comparison hkIntVector::equalZeroS32() const
{
	hkVector4Comparison c;
	c.m_mask = vceqq_u32(m_quad, vdupq_n_u32(0));
	return c;
}

HK_FORCE_INLINE const hkVector4Comparison hkIntVector::greaterZeroS32() const
{
	hkVector4Comparison c;
	c.m_mask = vcgtq_s32(vreinterpretq_s32_u32(m_quad), vdupq_n_s32(0));
	return c;
}

HK_FORCE_INLINE const hkVector4Comparison hkIntVector::greaterEqualS32(hkIntVectorParameter b) const
{
	hkVector4Comparison c;
	c.m_mask = vcgeq_s32(vreinterpretq_s32_u32(m_quad), vreinterpretq_s32_u32(b.m_quad));
	return c;
}

HK_FORCE_INLINE void hkIntVector::setNot(hkIntVectorParameter a)
{
	m_quad = vmvnq_u32(a.m_quad);
}

template <> HK_FORCE_INLINE void hkIntVector::setNegS32<4>(hkIntVectorParameter v)
{
	m_quad = vreinterpretq_u32_s32(vnegq_s32(vreinterpretq_s32_u32(v.m_quad)));
}


HK_FORCE_INLINE void hkIntVector::setOr(hkIntVectorParameter a, hkIntVectorParameter b)
{
	m_quad = vorrq_u32(a.m_quad, b.m_quad);
}

HK_FORCE_INLINE void hkIntVector::setAnd(hkIntVectorParameter a, hkIntVectorParameter b)
{
	m_quad = vandq_u32(a.m_quad, b.m_quad);
}

HK_FORCE_INLINE void hkIntVector::setXor(hkIntVectorParameter a, hkIntVectorParameter b)
{
	m_quad = veorq_u32(a.m_quad, b.m_quad);
}

HK_FORCE_INLINE void hkIntVector::setAndNot(hkIntVectorParameter a, hkIntVectorParameter b)
{
	m_quad = vbicq_u32(a.m_quad, b.m_quad);
}

//
// Arithmetic operations
//

HK_FORCE_INLINE void hkIntVector::setSubU16( hkIntVectorParameter a, hkIntVectorParameter b )
{
	m_quad = vreinterpretq_u32_u16( vsubq_u16( vreinterpretq_u16_u32(a.m_quad), vreinterpretq_u16_u32(b.m_quad) ) );
}

HK_FORCE_INLINE void hkIntVector::setAddU16( hkIntVectorParameter a, hkIntVectorParameter b )
{
	m_quad = vreinterpretq_u32_u16( vaddq_u16( vreinterpretq_u16_u32(a.m_quad), vreinterpretq_u16_u32(b.m_quad) ) );
}

HK_FORCE_INLINE void hkIntVector::setSubU32( hkIntVectorParameter a, hkIntVectorParameter b )
{
	m_quad = vsubq_u32( a.m_quad, b.m_quad );
}

HK_FORCE_INLINE void hkIntVector::setAddU32( hkIntVectorParameter a, hkIntVectorParameter b )
{
	m_quad = vaddq_u32( a.m_quad, b.m_quad );
}

HK_FORCE_INLINE void hkIntVector::setSubS32( hkIntVectorParameter a, hkIntVectorParameter b )
{
	m_quad = vreinterpretq_u32_s32(vsubq_s32(vreinterpretq_s32_u32(a.m_quad), vreinterpretq_s32_u32(b.m_quad)));
}

HK_FORCE_INLINE void hkIntVector::setAddS32( hkIntVectorParameter a, hkIntVectorParameter b )
{
	m_quad = vreinterpretq_u32_s32(vaddq_s32(vreinterpretq_s32_u32(a.m_quad), vreinterpretq_s32_u32(b.m_quad)));
}

HK_FORCE_INLINE void hkIntVector::setMul(hkIntVectorParameter vA, hkIntVectorParameter vB)
{
	m_quad = vreinterpretq_u32_s32(vmulq_s32(vreinterpretq_s32_u32(vA.m_quad), vreinterpretq_s32_u32(vB.m_quad)));
}

HK_FORCE_INLINE void hkIntVector::setAddSaturateU32( hkIntVectorParameter a, hkIntVectorParameter b )
{
	m_quad = vqaddq_u32( a.m_quad, b.m_quad );
}

HK_FORCE_INLINE void hkIntVector::setAddSaturateS16( hkIntVectorParameter a, hkIntVectorParameter b )
{
	m_quad = vreinterpretq_u32_s16( vqaddq_s16( vreinterpretq_s16_u32(a.m_quad), vreinterpretq_s16_u32(b.m_quad) ) );
}

HK_FORCE_INLINE void hkIntVector::setAddSaturateU16( hkIntVectorParameter a, hkIntVectorParameter b )
{
	m_quad = vreinterpretq_u32_u16( vqaddq_u16( vreinterpretq_u16_u32(a.m_quad), vreinterpretq_u16_u32(b.m_quad) ) );
}

HK_FORCE_INLINE void hkIntVector::setSubSaturateU32( hkIntVectorParameter a, hkIntVectorParameter b )
{
	m_quad = vqsubq_u32( a.m_quad, b.m_quad );
}

HK_FORCE_INLINE void hkIntVector::setSubSaturateS16( hkIntVectorParameter a, hkIntVectorParameter b )
{
	m_quad = vreinterpretq_u32_s16( vqsubq_s16( vreinterpretq_s16_u32(a.m_quad), vreinterpretq_s16_u32(b.m_quad) ) );
}

HK_FORCE_INLINE void hkIntVector::setSubSaturateU16( hkIntVectorParameter a, hkIntVectorParameter b )
{
	m_quad = vreinterpretq_u32_u16( vqsubq_u16( vreinterpretq_u16_u32(a.m_quad), vreinterpretq_u16_u32(b.m_quad) ) );
}

HK_FORCE_INLINE void hkIntVector::setMinU8( hkIntVectorParameter a, hkIntVectorParameter b )
{
	m_quad = vreinterpretq_u32_u8( vminq_u8( vreinterpretq_u8_u32(a.m_quad), vreinterpretq_u8_u32(b.m_quad) ) );
}

HK_FORCE_INLINE void hkIntVector::setMinS32( hkIntVectorParameter a, hkIntVectorParameter b )
{
	m_quad = vreinterpretq_u32_s32( vminq_s32( vreinterpretq_s32_u32(a.m_quad), vreinterpretq_s32_u32(b.m_quad) ) );
}

HK_FORCE_INLINE void hkIntVector::setMaxS32( hkIntVectorParameter a, hkIntVectorParameter b )
{
	m_quad = vreinterpretq_u32_s32( vmaxq_s32( vreinterpretq_s32_u32(a.m_quad), vreinterpretq_s32_u32(b.m_quad) ) );
}

HK_FORCE_INLINE void hkIntVector::setMinS16( hkIntVectorParameter a, hkIntVectorParameter b )
{
	m_quad = vreinterpretq_u32_s16( vminq_s16( vreinterpretq_s16_u32(a.m_quad), vreinterpretq_s16_u32(b.m_quad) ) );
}

HK_FORCE_INLINE void hkIntVector::setMaxS16( hkIntVectorParameter a, hkIntVectorParameter b )
{
	m_quad = vreinterpretq_u32_s16( vmaxq_s16( vreinterpretq_s16_u32(a.m_quad), vreinterpretq_s16_u32(b.m_quad) ) );
}

template <> HK_FORCE_INLINE int hkIntVector::horizontalMinS32<1>() const
{
	return getComponent<0>();
}

template <> HK_FORCE_INLINE int hkIntVector::horizontalMinS32<2>() const
{
	const int32x2_t xy = vreinterpret_s32_u32(vget_low_u32(m_quad));
	return vget_lane_s32(vpmin_s32(xy, xy), 0);
}

template <> HK_FORCE_INLINE int hkIntVector::horizontalMinS32<3>() const
{
	const int32x2_t xy = vreinterpret_s32_u32(vget_low_u32(m_quad));
	const int32x2_t zz = vdup_lane_s32(vreinterpret_s32_u32(vget_high_u32(m_quad)), 0);
	return vget_lane_s32(vmin_s32(vpmin_s32(xy, xy), zz), 0);
}

template <> HK_FORCE_INLINE int hkIntVector::horizontalMinS32<4>() const
{
	const int32x2_t xy = vreinterpret_s32_u32(vget_low_u32(m_quad));
	const int32x2_t zw = vreinterpret_s32_u32(vget_high_u32(m_quad));
	return vget_lane_s32(vmin_s32(vpmin_s32(xy, xy), vpmin_s32(zw, zw)), 0);
}

template <int N> HK_FORCE_INLINE int hkIntVector::horizontalMinS32() const
{
	HK_INT_VECTOR_NOT_IMPLEMENTED;
	return 0;
}

template <> HK_FORCE_INLINE int hkIntVector::horizontalMaxS32<1>() const
{
	return getComponent<0>();
}

template <> HK_FORCE_INLINE int hkIntVector::horizontalMaxS32<2>() const
{
	const int32x2_t xy = vreinterpret_s32_u32(vget_low_u32(m_quad));
	return vget_lane_s32(vpmax_s32(xy, xy), 0);
}

template <> HK_FORCE_INLINE int hkIntVector::horizontalMaxS32<3>() const
{
	const int32x2_t xy = vreinterpret_s32_u32(vget_low_u32(m_quad));
	const int32x2_t zz = vdup_lane_s32(vreinterpret_s32_u32(vget_high_u32(m_quad)), 0);
	return vget_lane_s32(vmax_s32(vpmax_s32(xy, xy), zz), 0);
}

template <> HK_FORCE_INLINE int hkIntVector::horizontalMaxS32<4>() const
{
	const int32x2_t xy = vreinterpret_s32_u32(vget_low_u32(m_quad));
	const int32x2_t zw = vreinterpret_s32_u32(vget_high_u32(m_quad));
	return vget_lane_s32(vmax_s32(vpmax_s32(xy, xy), vpmax_s32(zw, zw)), 0);
}

template <int N> HK_FORCE_INLINE int hkIntVector::horizontalMaxS32() const
{
	HK_INT_VECTOR_NOT_IMPLEMENTED;
	return 0;
}

//
//	Returns the sum of the first N components

template <> HK_FORCE_INLINE int hkIntVector::horizontalAddS32<1>() const
{
	return getComponent<0>();
}

template <> HK_FORCE_INLINE int hkIntVector::horizontalAddS32<2>() const
{
	const int32x2_t l = vreinterpret_s32_u32(vget_low_u32(m_quad));
	return vget_lane_s32(vpadd_s32(l, l), 0);
}

template <> HK_FORCE_INLINE int hkIntVector::horizontalAddS32<3>() const
{
	const int32x2_t xy = vreinterpret_s32_u32(vget_low_u32(m_quad));
	const int32x2_t zz = vdup_lane_s32(vreinterpret_s32_u32(vget_high_u32(m_quad)), 0);
	return vget_lane_s32(vadd_s32(vpadd_s32(xy, xy), zz), 0);
}

template <> HK_FORCE_INLINE int hkIntVector::horizontalAddS32<4>() const
{
	const int32x2_t xy = vreinterpret_s32_u32(vget_low_u32(m_quad));
	const int32x2_t zw = vreinterpret_s32_u32(vget_high_u32(m_quad));
	return vget_lane_s32(vadd_s32(vpadd_s32(xy, xy), vpadd_s32(zw, zw)), 0);
}

template <int N> HK_FORCE_INLINE int hkIntVector::horizontalAddS32() const
{
	HK_INT_VECTOR_NOT_IMPLEMENTED;
	return 0;
}

//
//	Returns the xor of the first N components

template <> HK_FORCE_INLINE int hkIntVector::horizontalXorS32<1>() const
{
	return getComponent<0>();
}

template <> HK_FORCE_INLINE int hkIntVector::horizontalXorS32<2>() const
{
	const int32x2_t xy = vreinterpret_s32_u32(vget_low_u32(m_quad));
	return vget_lane_s32(veor_s32(xy, vrev64_s32(xy)), 0);
}

template <> HK_FORCE_INLINE int hkIntVector::horizontalXorS32<3>() const
{
	const int32x2_t xy		= vreinterpret_s32_u32(vget_low_u32(m_quad));
	const int32x2_t zz		= vdup_lane_s32(vreinterpret_s32_u32(vget_high_u32(m_quad)), 0);
	const int32x2_t xy_xy	= veor_s32(xy, vrev64_s32(xy));
	return vget_lane_s32(veor_s32(xy_xy, zz), 0);
}

template <> HK_FORCE_INLINE int hkIntVector::horizontalXorS32<4>() const
{
	const int32x2_t xy		= vreinterpret_s32_u32(vget_low_u32(m_quad));
	const int32x2_t zw		= vreinterpret_s32_u32(vget_high_u32(m_quad));
	const int32x2_t xy_xy	= veor_s32(xy, vrev64_s32(xy));
	const int32x2_t zw_zw	= veor_s32(zw, vrev64_s32(zw));
	return vget_lane_s32(veor_s32(xy_xy, zw_zw), 0);
}

template <int N> HK_FORCE_INLINE int hkIntVector::horizontalXorS32() const
{
	HK_INT_VECTOR_NOT_IMPLEMENTED;
	return 0;
}

//
//	Returns the dot product of the first N components

template <> HK_FORCE_INLINE hkInt64 hkIntVector::dot<1>(hkIntVectorParameter v) const
{
	const int64x2_t xy = vmull_s32(vreinterpret_s32_u32(vget_low_u32(m_quad)), vreinterpret_s32_u32(vget_low_u32(v.m_quad)));
	return vgetq_lane_s64(xy, 0);
}

template <> HK_FORCE_INLINE hkInt64 hkIntVector::dot<2>(hkIntVectorParameter v) const
{
	const int64x2_t xy	= vmull_s32(vreinterpret_s32_u32(vget_low_u32(m_quad)), vreinterpret_s32_u32(vget_low_u32(v.m_quad)));
	return vget_lane_s64(vadd_s64(vget_low_s64(xy), vget_high_s64(xy)), 0);
}

template <> HK_FORCE_INLINE hkInt64 hkIntVector::dot<3>(hkIntVectorParameter v) const
{
	const int64x2_t xy		= vmull_s32(vreinterpret_s32_u32(vget_low_u32(m_quad)), vreinterpret_s32_u32(vget_low_u32(v.m_quad)));
	const int64x2_t zw		= vmull_s32(vreinterpret_s32_u32(vget_high_u32(m_quad)), vreinterpret_s32_u32(vget_high_u32(v.m_quad)));
	const int64x1_t xpy		= vadd_s64(vget_low_s64(xy), vget_high_s64(xy));
	return vget_lane_s64(vadd_s64(xpy, vget_low_s64(zw)), 0);
}

template <> HK_FORCE_INLINE hkInt64 hkIntVector::dot<4>(hkIntVectorParameter v) const
{
	const int64x2_t xy		= vmull_s32(vreinterpret_s32_u32(vget_low_u32(m_quad)), vreinterpret_s32_u32(vget_low_u32(v.m_quad)));
	const int64x2_t zw		= vmull_s32(vreinterpret_s32_u32(vget_high_u32(m_quad)), vreinterpret_s32_u32(vget_high_u32(v.m_quad)));
	const int64x2_t xz_yw	= vaddq_s64(xy, zw);
	return vget_lane_s64(vadd_s64(vget_low_s64(xz_yw), vget_high_s64(xz_yw)), 0);
}

template <int N> HK_FORCE_INLINE hkInt64 hkIntVector::dot(hkIntVectorParameter v) const
{
	HK_INT_VECTOR_NOT_IMPLEMENTED;
	return 0;
}

//
// Shift operations
//

template <> HK_FORCE_INLINE void hkIntVector::setShiftLeft16<0>(hkIntVectorParameter a)		{	m_quad = a.m_quad;			}
template <> HK_FORCE_INLINE void hkIntVector::setShiftLeft16<16>(hkIntVectorParameter a)	{	m_quad = vdupq_n_u32(0);	}
template <int N> HK_FORCE_INLINE void hkIntVector::setShiftLeft16(hkIntVectorParameter a)
{
#ifdef HK_PLATFORM_IOS
	// Add Switch to fix Clang issue were last parameter of vshlq_n_u16 must be in constrange(0,15)
	// See RSYS-1377 for more information
	switch(N)
	{
	case 1:  m_quad = vreinterpretq_u32_u16( vshlq_n_u16( vreinterpretq_u16_u32(a.m_quad), 1 ) ); break;
	case 2:  m_quad = vreinterpretq_u32_u16( vshlq_n_u16( vreinterpretq_u16_u32(a.m_quad), 2 ) ); break;
	case 3:  m_quad = vreinterpretq_u32_u16( vshlq_n_u16( vreinterpretq_u16_u32(a.m_quad), 3 ) ); break;
	case 4:  m_quad = vreinterpretq_u32_u16( vshlq_n_u16( vreinterpretq_u16_u32(a.m_quad), 4 ) ); break;
	case 5:  m_quad = vreinterpretq_u32_u16( vshlq_n_u16( vreinterpretq_u16_u32(a.m_quad), 5 ) ); break;
	case 6:  m_quad = vreinterpretq_u32_u16( vshlq_n_u16( vreinterpretq_u16_u32(a.m_quad), 6 ) ); break;
	case 7:  m_quad = vreinterpretq_u32_u16( vshlq_n_u16( vreinterpretq_u16_u32(a.m_quad), 7 ) ); break;
	case 8:  m_quad = vreinterpretq_u32_u16( vshlq_n_u16( vreinterpretq_u16_u32(a.m_quad), 8 ) ); break;
	case 9:  m_quad = vreinterpretq_u32_u16( vshlq_n_u16( vreinterpretq_u16_u32(a.m_quad), 9 ) ); break;
	case 10: m_quad = vreinterpretq_u32_u16( vshlq_n_u16( vreinterpretq_u16_u32(a.m_quad), 10 ) ); break;
	case 11: m_quad = vreinterpretq_u32_u16( vshlq_n_u16( vreinterpretq_u16_u32(a.m_quad), 11 ) ); break;
	case 12: m_quad = vreinterpretq_u32_u16( vshlq_n_u16( vreinterpretq_u16_u32(a.m_quad), 12 ) ); break;
	case 13: m_quad = vreinterpretq_u32_u16( vshlq_n_u16( vreinterpretq_u16_u32(a.m_quad), 13 ) ); break;
	case 14: m_quad = vreinterpretq_u32_u16( vshlq_n_u16( vreinterpretq_u16_u32(a.m_quad), 14 ) ); break;
	default: m_quad = vreinterpretq_u32_u16( vshlq_n_u16( vreinterpretq_u16_u32(a.m_quad), 15 ) ); break;
	}
#else
	m_quad = vreinterpretq_u32_u16(vshlq_n_u16(vreinterpretq_u16_u32(a.m_quad), N));
#endif
}

template <> HK_FORCE_INLINE void hkIntVector::setShiftRight16<0>(hkIntVectorParameter a)	{	m_quad = a.m_quad;			}
template <> HK_FORCE_INLINE void hkIntVector::setShiftRight16<16>(hkIntVectorParameter a)	{	m_quad = vdupq_n_u32(0);	}
template <int N> HK_FORCE_INLINE void hkIntVector::setShiftRight16( hkIntVectorParameter a)
{
#ifdef HK_PLATFORM_IOS
	// Add Switch to fix Clang issue were last parameter of vshrq_n_u16 must be in constrange(1,16)
	// See RSYS-1377 for more information
	switch(N)
	{
	case 1:  m_quad = vreinterpretq_u32_u16( vshrq_n_u16( vreinterpretq_u16_u32(a.m_quad), 1 ) ); break;
	case 2:  m_quad = vreinterpretq_u32_u16( vshrq_n_u16( vreinterpretq_u16_u32(a.m_quad), 2 ) ); break;
	case 3:  m_quad = vreinterpretq_u32_u16( vshrq_n_u16( vreinterpretq_u16_u32(a.m_quad), 3 ) ); break;
	case 4:  m_quad = vreinterpretq_u32_u16( vshrq_n_u16( vreinterpretq_u16_u32(a.m_quad), 4 ) ); break;
	case 5:  m_quad = vreinterpretq_u32_u16( vshrq_n_u16( vreinterpretq_u16_u32(a.m_quad), 5 ) ); break;
	case 6:  m_quad = vreinterpretq_u32_u16( vshrq_n_u16( vreinterpretq_u16_u32(a.m_quad), 6 ) ); break;
	case 7:  m_quad = vreinterpretq_u32_u16( vshrq_n_u16( vreinterpretq_u16_u32(a.m_quad), 7 ) ); break;
	case 8:  m_quad = vreinterpretq_u32_u16( vshrq_n_u16( vreinterpretq_u16_u32(a.m_quad), 8 ) ); break;
	case 9:  m_quad = vreinterpretq_u32_u16( vshrq_n_u16( vreinterpretq_u16_u32(a.m_quad), 9 ) ); break;
	case 10: m_quad = vreinterpretq_u32_u16( vshrq_n_u16( vreinterpretq_u16_u32(a.m_quad), 10 ) ); break;
	case 11: m_quad = vreinterpretq_u32_u16( vshrq_n_u16( vreinterpretq_u16_u32(a.m_quad), 11 ) ); break;
	case 12: m_quad = vreinterpretq_u32_u16( vshrq_n_u16( vreinterpretq_u16_u32(a.m_quad), 12 ) ); break;
	case 13: m_quad = vreinterpretq_u32_u16( vshrq_n_u16( vreinterpretq_u16_u32(a.m_quad), 13 ) ); break;
	case 14: m_quad = vreinterpretq_u32_u16( vshrq_n_u16( vreinterpretq_u16_u32(a.m_quad), 14 ) ); break;
	default: m_quad = vreinterpretq_u32_u16( vshrq_n_u16( vreinterpretq_u16_u32(a.m_quad), 15 ) ); break;	
	}
#else
	m_quad = vreinterpretq_u32_u16(vshrq_n_u16(vreinterpretq_u16_u32(a.m_quad), N));
#endif
}

template <> HK_FORCE_INLINE void hkIntVector::setShiftRightS16<0>(hkIntVectorParameter a)	{	m_quad = a.m_quad;			}
template <> HK_FORCE_INLINE void hkIntVector::setShiftRightS16<16>(hkIntVectorParameter a)	{	m_quad = vdupq_n_u32(0);	}
template<int N> HK_FORCE_INLINE void hkIntVector::setShiftRightS16( hkIntVectorParameter a )
{
#ifdef HK_PLATFORM_IOS
	// Add Switch to fix Clang issue were last parameter of vshrq_n_u16 must be in constrange(1,16)
	// See RSYS-1377 for more information
	switch(N)
	{
	case 1:  m_quad = vreinterpretq_u32_s16( vshrq_n_s16( vreinterpretq_s16_u32(a.m_quad), 1 ) ); break;
	case 2:  m_quad = vreinterpretq_u32_s16( vshrq_n_s16( vreinterpretq_s16_u32(a.m_quad), 2 ) ); break;
	case 3:  m_quad = vreinterpretq_u32_s16( vshrq_n_s16( vreinterpretq_s16_u32(a.m_quad), 3 ) ); break;
	case 4:  m_quad = vreinterpretq_u32_s16( vshrq_n_s16( vreinterpretq_s16_u32(a.m_quad), 4 ) ); break;
	case 5:  m_quad = vreinterpretq_u32_s16( vshrq_n_s16( vreinterpretq_s16_u32(a.m_quad), 5 ) ); break;
	case 6:  m_quad = vreinterpretq_u32_s16( vshrq_n_s16( vreinterpretq_s16_u32(a.m_quad), 6 ) ); break;
	case 7:  m_quad = vreinterpretq_u32_s16( vshrq_n_s16( vreinterpretq_s16_u32(a.m_quad), 7 ) ); break;
	case 8:  m_quad = vreinterpretq_u32_s16( vshrq_n_s16( vreinterpretq_s16_u32(a.m_quad), 8 ) ); break;
	case 9:  m_quad = vreinterpretq_u32_s16( vshrq_n_s16( vreinterpretq_s16_u32(a.m_quad), 9 ) ); break;
	case 10: m_quad = vreinterpretq_u32_s16( vshrq_n_s16( vreinterpretq_s16_u32(a.m_quad), 10 ) ); break;
	case 11: m_quad = vreinterpretq_u32_s16( vshrq_n_s16( vreinterpretq_s16_u32(a.m_quad), 11 ) ); break;
	case 12: m_quad = vreinterpretq_u32_s16( vshrq_n_s16( vreinterpretq_s16_u32(a.m_quad), 12 ) ); break;
	case 13: m_quad = vreinterpretq_u32_s16( vshrq_n_s16( vreinterpretq_s16_u32(a.m_quad), 13 ) ); break;
	case 14: m_quad = vreinterpretq_u32_s16( vshrq_n_s16( vreinterpretq_s16_u32(a.m_quad), 14 ) ); break;
	default: m_quad = vreinterpretq_u32_s16( vshrq_n_s16( vreinterpretq_s16_u32(a.m_quad), 15 ) ); break;
	}
#else
	m_quad = vreinterpretq_u32_s16(vshrq_n_s16(vreinterpretq_s16_u32(a.m_quad), N));
#endif
}

template <> HK_FORCE_INLINE void hkIntVector::setShiftLeft32<0>(hkIntVectorParameter a)		{	m_quad = a.m_quad;			}
template <> HK_FORCE_INLINE void hkIntVector::setShiftLeft32<32>(hkIntVectorParameter a)	{	m_quad = vdupq_n_u32(0);	}
template<int N> HK_FORCE_INLINE void hkIntVector::setShiftLeft32( hkIntVectorParameter a)
{
#ifdef HK_PLATFORM_IOS
	// Add Switch to fix Clang issue were last parameter of vshlq_n_u32 must be in constrange(0,31)
	// See RSYS-1377 for more information
	switch(N)
	{
	case 1:  m_quad = vshlq_n_u32( a.m_quad, 1 ); break;
	case 2:  m_quad = vshlq_n_u32( a.m_quad, 2 ); break;
	case 3:  m_quad = vshlq_n_u32( a.m_quad, 3 ); break;
	case 4:  m_quad = vshlq_n_u32( a.m_quad, 4 ); break;
	case 5:  m_quad = vshlq_n_u32( a.m_quad, 5 ); break;
	case 6:  m_quad = vshlq_n_u32( a.m_quad, 6 ); break;
	case 7:  m_quad = vshlq_n_u32( a.m_quad, 7 ); break;
	case 8:  m_quad = vshlq_n_u32( a.m_quad, 8 ); break;
	case 9:  m_quad = vshlq_n_u32( a.m_quad, 9 ); break;
	case 10: m_quad = vshlq_n_u32( a.m_quad, 10 ); break;
	case 11: m_quad = vshlq_n_u32( a.m_quad, 11 ); break;
	case 12: m_quad = vshlq_n_u32( a.m_quad, 12 ); break;
	case 13: m_quad = vshlq_n_u32( a.m_quad, 13 ); break;
	case 14: m_quad = vshlq_n_u32( a.m_quad, 14 ); break;
	case 15: m_quad = vshlq_n_u32( a.m_quad, 15 ); break;
	case 16: m_quad = vshlq_n_u32( a.m_quad, 16 ); break;
	case 17: m_quad = vshlq_n_u32( a.m_quad, 17 ); break;
	case 18: m_quad = vshlq_n_u32( a.m_quad, 18 ); break;
	case 19: m_quad = vshlq_n_u32( a.m_quad, 19 ); break;
	case 20: m_quad = vshlq_n_u32( a.m_quad, 20 ); break;
	case 21: m_quad = vshlq_n_u32( a.m_quad, 21 ); break;
	case 22: m_quad = vshlq_n_u32( a.m_quad, 22 ); break;
	case 23: m_quad = vshlq_n_u32( a.m_quad, 23 ); break;
	case 24: m_quad = vshlq_n_u32( a.m_quad, 24 ); break;
	case 25: m_quad = vshlq_n_u32( a.m_quad, 25 ); break;
	case 26: m_quad = vshlq_n_u32( a.m_quad, 26 ); break;
	case 27: m_quad = vshlq_n_u32( a.m_quad, 27 ); break;
	case 28: m_quad = vshlq_n_u32( a.m_quad, 28 ); break;
	case 29: m_quad = vshlq_n_u32( a.m_quad, 29 ); break;
	case 30: m_quad = vshlq_n_u32( a.m_quad, 30 ); break;
	default: m_quad = vshlq_n_u32( a.m_quad, 31 ); break;
	}
#else
	m_quad = vshlq_n_u32(a.m_quad, N);
#endif
}

template <> HK_FORCE_INLINE void hkIntVector::setShiftRight32<0>(hkIntVectorParameter a)	{	m_quad = a.m_quad;			}
template <> HK_FORCE_INLINE void hkIntVector::setShiftRight32<32>(hkIntVectorParameter a)	{	m_quad = vdupq_n_u32(0);	}
template<int N> HK_FORCE_INLINE void hkIntVector::setShiftRight32( hkIntVectorParameter a)
{
#ifdef HK_PLATFORM_IOS
	// Add Switch to fix Clang issue were last parameter of vshrq_n_u32 must be in constrange(1,32)
	// See RSYS-1377 for more information
	switch(N)
	{
	case 1:  m_quad = vshrq_n_u32( a.m_quad, 1 ); break;
	case 2:  m_quad = vshrq_n_u32( a.m_quad, 2 ); break;
	case 3:  m_quad = vshrq_n_u32( a.m_quad, 3 ); break;
	case 4:  m_quad = vshrq_n_u32( a.m_quad, 4 ); break;
	case 5:  m_quad = vshrq_n_u32( a.m_quad, 5 ); break;
	case 6:  m_quad = vshrq_n_u32( a.m_quad, 6 ); break;
	case 7:  m_quad = vshrq_n_u32( a.m_quad, 7 ); break;
	case 8:  m_quad = vshrq_n_u32( a.m_quad, 8 ); break;
	case 9:  m_quad = vshrq_n_u32( a.m_quad, 9 ); break;
	case 10: m_quad = vshrq_n_u32( a.m_quad, 10 ); break;
	case 11: m_quad = vshrq_n_u32( a.m_quad, 11 ); break;
	case 12: m_quad = vshrq_n_u32( a.m_quad, 12 ); break;
	case 13: m_quad = vshrq_n_u32( a.m_quad, 13 ); break;
	case 14: m_quad = vshrq_n_u32( a.m_quad, 14 ); break;
	case 15: m_quad = vshrq_n_u32( a.m_quad, 15 ); break;
	case 16: m_quad = vshrq_n_u32( a.m_quad, 16 ); break;
	case 17: m_quad = vshrq_n_u32( a.m_quad, 17 ); break;
	case 18: m_quad = vshrq_n_u32( a.m_quad, 18 ); break;
	case 19: m_quad = vshrq_n_u32( a.m_quad, 19 ); break;
	case 20: m_quad = vshrq_n_u32( a.m_quad, 20 ); break;
	case 21: m_quad = vshrq_n_u32( a.m_quad, 21 ); break;
	case 22: m_quad = vshrq_n_u32( a.m_quad, 22 ); break;
	case 23: m_quad = vshrq_n_u32( a.m_quad, 23 ); break;
	case 24: m_quad = vshrq_n_u32( a.m_quad, 24 ); break;
	case 25: m_quad = vshrq_n_u32( a.m_quad, 25 ); break;
	case 26: m_quad = vshrq_n_u32( a.m_quad, 26 ); break;
	case 27: m_quad = vshrq_n_u32( a.m_quad, 27 ); break;
	case 28: m_quad = vshrq_n_u32( a.m_quad, 28 ); break;
	case 29: m_quad = vshrq_n_u32( a.m_quad, 29 ); break;
	case 30: m_quad = vshrq_n_u32( a.m_quad, 30 ); break;
	default: m_quad = vshrq_n_u32( a.m_quad, 31 ); break;
	}
#else
	m_quad = vshrq_n_u32(a.m_quad, N);
#endif
}

template <> HK_FORCE_INLINE void hkIntVector::setShiftRightS32<0>(hkIntVectorParameter a)	{	m_quad = a.m_quad;			}
template <> HK_FORCE_INLINE void hkIntVector::setShiftRightS32<32>(hkIntVectorParameter a)	{	m_quad = vdupq_n_u32(0);	}
template<int N> HK_FORCE_INLINE void hkIntVector::setShiftRightS32( hkIntVectorParameter a )
{
#ifdef HK_PLATFORM_IOS
	// Add Switch to fix Clang issue were last parameter of vshrq_n_s32 must be in constrange(1,32)
	// See RSYS-1377 for more information
	switch(N)
	{
	case 1:  m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 1 ) ); break;
	case 2:  m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 2 ) ); break;
	case 3:  m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 3 ) ); break;
	case 4:  m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 4 ) ); break;
	case 5:  m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 5 ) ); break;
	case 6:  m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 6 ) ); break;
	case 7:  m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 7 ) ); break;
	case 8:  m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 8 ) ); break;
	case 9:  m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 9 ) ); break;
	case 10: m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 10 ) ); break;
	case 11: m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 11 ) ); break;
	case 12: m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 12 ) ); break;
	case 13: m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 13 ) ); break;
	case 14: m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 14 ) ); break;
	case 15: m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 15 ) ); break;
	case 16: m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 16 ) ); break;
	case 17: m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 17 ) ); break;
	case 18: m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 18 ) ); break;
	case 19: m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 19 ) ); break;
	case 20: m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 20 ) ); break;
	case 21: m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 21 ) ); break;
	case 22: m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 22 ) ); break;
	case 23: m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 23 ) ); break;
	case 24: m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 24 ) ); break;
	case 25: m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 25 ) ); break;
	case 26: m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 26 ) ); break;
	case 27: m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 27 ) ); break;
	case 28: m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 28 ) ); break;
	case 29: m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 29 ) ); break;
	case 30: m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 30 ) ); break;
	default: m_quad = vreinterpretq_u32_s32( vshrq_n_s32( vreinterpretq_s32_u32(a.m_quad), 31 ) ); break;
	}
#else
	m_quad = vreinterpretq_u32_s32(vshrq_n_s32(vreinterpretq_s32_u32(a.m_quad), N));
#endif
}

template <> HK_FORCE_INLINE void hkIntVector::setShiftLeft128<0>(hkIntVectorParameter a)	{	m_quad = a.m_quad;			}
template <> HK_FORCE_INLINE void hkIntVector::setShiftLeft128<16>(hkIntVectorParameter a)	{	m_quad = vdupq_n_u32(0);	}
template <int N> HK_FORCE_INLINE void hkIntVector::setShiftLeft128( hkIntVectorParameter a)
{
	const uint8x16_t zero = vdupq_n_u8(0);

#ifdef HK_PLATFORM_IOS
	// Add Switch to fix Clang issue were last parameter of vextq_u8 must be in constrange(0,15)
	// See RSYS-1377 for more information
	switch(N)
	{
	case 1:  m_quad = vreinterpretq_u32_u8( vextq_u8( vreinterpretq_u8_u32(a.m_quad), zero, 1) ); break;
	case 2:  m_quad = vreinterpretq_u32_u8( vextq_u8( vreinterpretq_u8_u32(a.m_quad), zero, 2) ); break;
	case 3:  m_quad = vreinterpretq_u32_u8( vextq_u8( vreinterpretq_u8_u32(a.m_quad), zero, 3) ); break;
	case 4:  m_quad = vreinterpretq_u32_u8( vextq_u8( vreinterpretq_u8_u32(a.m_quad), zero, 4) ); break;
	case 5:  m_quad = vreinterpretq_u32_u8( vextq_u8( vreinterpretq_u8_u32(a.m_quad), zero, 5) ); break;
	case 6:  m_quad = vreinterpretq_u32_u8( vextq_u8( vreinterpretq_u8_u32(a.m_quad), zero, 6) ); break;
	case 7:  m_quad = vreinterpretq_u32_u8( vextq_u8( vreinterpretq_u8_u32(a.m_quad), zero, 7) ); break;
	case 8:  m_quad = vreinterpretq_u32_u8( vextq_u8( vreinterpretq_u8_u32(a.m_quad), zero, 8) ); break;
	case 9:  m_quad = vreinterpretq_u32_u8( vextq_u8( vreinterpretq_u8_u32(a.m_quad), zero, 9) ); break;
	case 10: m_quad = vreinterpretq_u32_u8( vextq_u8( vreinterpretq_u8_u32(a.m_quad), zero, 10) ); break;
	case 11: m_quad = vreinterpretq_u32_u8( vextq_u8( vreinterpretq_u8_u32(a.m_quad), zero, 11) ); break;
	case 12: m_quad = vreinterpretq_u32_u8( vextq_u8( vreinterpretq_u8_u32(a.m_quad), zero, 12) ); break;
	case 13: m_quad = vreinterpretq_u32_u8( vextq_u8( vreinterpretq_u8_u32(a.m_quad), zero, 13) ); break;
	case 14: m_quad = vreinterpretq_u32_u8( vextq_u8( vreinterpretq_u8_u32(a.m_quad), zero, 14) ); break;
	default: m_quad = vreinterpretq_u32_u8( vextq_u8( vreinterpretq_u8_u32(a.m_quad), zero, 15) ); break;
	}
#else
    m_quad = vreinterpretq_u32_u8(vextq_u8(vreinterpretq_u8_u32(a.m_quad), zero, N));
#endif
}

template <> HK_FORCE_INLINE void hkIntVector::setShiftRight128<0>(hkIntVectorParameter a)	{	m_quad = a.m_quad;			}
template <> HK_FORCE_INLINE void hkIntVector::setShiftRight128<16>(hkIntVectorParameter a)	{	m_quad = vdupq_n_u32(0);	}
template <int N> HK_FORCE_INLINE void hkIntVector::setShiftRight128( hkIntVectorParameter a)
{
	const int invShift = ((16 - N) & 0xF);
	const uint8x16_t zero = vdupq_n_u8(0);

#ifdef HK_PLATFORM_IOS
	// Add Switch to fix Clang issue were last parameter of vextq_u8 must be in constrange(0,15)
	// See RSYS-1377 for more information
	switch(invShift)
	{
	case 1:  m_quad = vreinterpretq_u32_u8( vextq_u8( zero, vreinterpretq_u8_u32(a.m_quad), 1) ); break;
	case 2:  m_quad = vreinterpretq_u32_u8( vextq_u8( zero, vreinterpretq_u8_u32(a.m_quad), 2) ); break;
	case 3:  m_quad = vreinterpretq_u32_u8( vextq_u8( zero, vreinterpretq_u8_u32(a.m_quad), 3) ); break;
	case 4:  m_quad = vreinterpretq_u32_u8( vextq_u8( zero, vreinterpretq_u8_u32(a.m_quad), 4) ); break;
	case 5:  m_quad = vreinterpretq_u32_u8( vextq_u8( zero, vreinterpretq_u8_u32(a.m_quad), 5) ); break;
	case 6:  m_quad = vreinterpretq_u32_u8( vextq_u8( zero, vreinterpretq_u8_u32(a.m_quad), 6) ); break;
	case 7:  m_quad = vreinterpretq_u32_u8( vextq_u8( zero, vreinterpretq_u8_u32(a.m_quad), 7) ); break;
	case 8:  m_quad = vreinterpretq_u32_u8( vextq_u8( zero, vreinterpretq_u8_u32(a.m_quad), 8) ); break;
	case 9:  m_quad = vreinterpretq_u32_u8( vextq_u8( zero, vreinterpretq_u8_u32(a.m_quad), 9) ); break;
	case 10: m_quad = vreinterpretq_u32_u8( vextq_u8( zero, vreinterpretq_u8_u32(a.m_quad), 10) ); break;
	case 11: m_quad = vreinterpretq_u32_u8( vextq_u8( zero, vreinterpretq_u8_u32(a.m_quad), 11) ); break;
	case 12: m_quad = vreinterpretq_u32_u8( vextq_u8( zero, vreinterpretq_u8_u32(a.m_quad), 12) ); break;
	case 13: m_quad = vreinterpretq_u32_u8( vextq_u8( zero, vreinterpretq_u8_u32(a.m_quad), 13) ); break;
	case 14: m_quad = vreinterpretq_u32_u8( vextq_u8( zero, vreinterpretq_u8_u32(a.m_quad), 14) ); break;
	default: m_quad = vreinterpretq_u32_u8( vextq_u8( zero, vreinterpretq_u8_u32(a.m_quad), 15) ); break;
	}
#else
    m_quad = vreinterpretq_u32_u8( vextq_u8( zero, vreinterpretq_u8_u32(a.m_quad), invShift) );
#endif
}

HK_FORCE_INLINE void hkIntVector::setShiftRight16( hkIntVectorParameter a, int shift )
{
	hkInt16 shift16 = hkInt16(shift);
	const int16x8_t shift_vec = vnegq_s16( vld1q_dup_s16(&shift16 ) );
	m_quad = vreinterpretq_u32_u16( vshlq_u16( vreinterpretq_u16_u32(a.m_quad), shift_vec ) );
}

HK_FORCE_INLINE void hkIntVector::setShiftLeft16( hkIntVectorParameter a, int shift )
{
	hkInt16 shift16 = hkInt16(shift);
	const int16x8_t shift_vec = vld1q_dup_s16(&shift16 );
	m_quad = vreinterpretq_u32_u16( vshlq_u16( vreinterpretq_u16_u32(a.m_quad), shift_vec ) );
}

HK_FORCE_INLINE void hkIntVector::setShiftLeft16( hkIntVectorParameter a, hkIntVectorParameter shift )
{
	const int16x8_t shift_vec = vreinterpretq_s16_u32( shift.m_quad );
	m_quad = vreinterpretq_u32_u16( vshlq_u16( vreinterpretq_u16_u32(a.m_quad), shift_vec ) );
}

HK_FORCE_INLINE void hkIntVector::setShiftRightS16( hkIntVectorParameter a, hkIntVectorParameter shift )
{
	const int16x8_t shift_vec = vnegq_s16( vreinterpretq_s16_u32( shift.m_quad ) );
	m_quad = vreinterpretq_u32_u16( vshlq_u16( vreinterpretq_u16_u32(a.m_quad), shift_vec ) );
}

HK_FORCE_INLINE void hkIntVector::setShiftRight32( hkIntVectorParameter a, int shift )
{
	const int32x4_t shift_vec = vnegq_s32( vld1q_dup_s32(&shift) );
	m_quad = vshlq_u32( a.m_quad, shift_vec );
}

HK_FORCE_INLINE void hkIntVector::setShiftLeft32( hkIntVectorParameter a, int shift )
{
	const int32x4_t shift_vec = vld1q_dup_s32(&shift);
	m_quad = vshlq_u32( a.m_quad, shift_vec );
}

HK_FORCE_INLINE void hkIntVector::setShiftLeft32( hkIntVectorParameter a, hkIntVectorParameter shift )
{
	const int32x4_t shift_vec = vreinterpretq_s32_u32( shift.m_quad );
	m_quad = vshlq_u32( a.m_quad, shift_vec );
}

HK_FORCE_INLINE void hkIntVector::setShiftRight32( hkIntVectorParameter a, hkIntVectorParameter shift )
{
	const int32x4_t shift_vec = vnegq_s32( vreinterpretq_s32_u32( shift.m_quad ) );
	m_quad = vshlq_u32( a.m_quad, shift_vec );
}


//
// Merge operations
// Stick to the convention that "high" means x,y, "low" means z,w
//
inline void hkIntVector::setMergeHead32(hkIntVectorParameter a, hkIntVectorParameter b)
{
	m_quad = vzipq_u32( a.m_quad, b.m_quad ).val[0];
}

inline void hkIntVector::setMergeTail32(hkIntVectorParameter a, hkIntVectorParameter b)
{
	m_quad = vzipq_u32( a.m_quad, b.m_quad ).val[1];
}

inline void hkIntVector::setMergeHead16(hkIntVectorParameter a, hkIntVectorParameter b)
{
	m_quad = vreinterpretq_u32_u16( vzipq_u16( vreinterpretq_u16_u32(a.m_quad), vreinterpretq_u16_u32(b.m_quad) ).val[0] );
}

inline void hkIntVector::setMergeTail16(hkIntVectorParameter a, hkIntVectorParameter b)
{
	m_quad = vreinterpretq_u32_u16( vzipq_u16( vreinterpretq_u16_u32(a.m_quad), vreinterpretq_u16_u32(b.m_quad) ).val[1] );
}

inline void hkIntVector::setMergeHead8(hkIntVectorParameter a, hkIntVectorParameter b)
{
	m_quad = vreinterpretq_u32_u8( vzipq_u8( vreinterpretq_u8_u32(a.m_quad), vreinterpretq_u8_u32(b.m_quad) ).val[0] );
}

inline void hkIntVector::setMergeTail8(hkIntVectorParameter a, hkIntVectorParameter b)
{
	m_quad = vreinterpretq_u32_u8( vzipq_u8( vreinterpretq_u8_u32(a.m_quad), vreinterpretq_u8_u32(b.m_quad) ).val[1] );
}

//
// Pack operations
//
HK_FORCE_INLINE void hkIntVector::setConvertU32ToU16( hkIntVectorParameter a, hkIntVectorParameter b )
{
	uint16x4_t na = vmovn_u32( a.m_quad );
	uint16x4_t nb = vmovn_u32( b.m_quad );
	m_quad = vreinterpretq_u32_u16( vcombine_u16( na, nb ) );
}

HK_FORCE_INLINE void hkIntVector::setConvertSaturateS32ToU16( hkIntVectorParameter a, hkIntVectorParameter b )
{
	uint16x4_t na = vqmovun_s32( vreinterpretq_s32_u32( a.m_quad ) );
	uint16x4_t nb = vqmovun_s32( vreinterpretq_s32_u32( b.m_quad ) );
	m_quad = vreinterpretq_u32_u16( vcombine_u16( na, nb ) );
}

HK_FORCE_INLINE void hkIntVector::setConvertSaturateS32ToS16( hkIntVectorParameter a, hkIntVectorParameter b )
{
	int16x4_t na = vqmovn_s32( vreinterpretq_s32_u32( a.m_quad ) );
	int16x4_t nb = vqmovn_s32( vreinterpretq_s32_u32( b.m_quad ) );
	m_quad = vreinterpretq_u32_s16( vcombine_s16( na, nb ) );
}

HK_FORCE_INLINE void hkIntVector::setConvertSaturateS16ToU8( hkIntVectorParameter a, hkIntVectorParameter b )
{
	uint8x8_t na = vqmovun_s16( vreinterpretq_s16_u32( a.m_quad ) );
	uint8x8_t nb = vqmovun_s16( vreinterpretq_s16_u32( b.m_quad ) );
	m_quad = vreinterpretq_u32_u8( vcombine_u8( na, nb ) );
}

//
//	Sets this.u32[i] = (unsigned int)a.u16[i]

HK_FORCE_INLINE void hkIntVector::setConvertLowerU16ToU32(hkIntVectorParameter a)
{
	m_quad = vmovl_u16( vget_low_u16( vreinterpretq_u16_u32( a.m_quad ) ) );
}

//
//	Sets this.u32[i] = (unsigned int)a.u16[i + 4]

HK_FORCE_INLINE void hkIntVector::setConvertUpperU16ToU32(hkIntVectorParameter a)
{
	m_quad = vmovl_u16( vget_high_u16( vreinterpretq_u16_u32( a.m_quad ) ) );
}

//
// Shuffle
//
template <> 
HK_FORCE_INLINE void hkIntVector::setPermutation<hkVectorPermutation::XYZW>(hkIntVectorParameter v)
{
	m_quad = v.m_quad;
}

template <> 
HK_FORCE_INLINE void hkIntVector::setPermutation<hkVectorPermutation::YZWX>(hkIntVectorParameter v)
{
	m_quad = vextq_u32( v.m_quad, v.m_quad, 1);
}

template <> 
HK_FORCE_INLINE void hkIntVector::setPermutation<hkVectorPermutation::ZWXY>(hkIntVectorParameter v)
{
	m_quad = vextq_u32( v.m_quad, v.m_quad, 2);
}

template <> 
HK_FORCE_INLINE void hkIntVector::setPermutation<hkVectorPermutation::WXYZ>(hkIntVectorParameter v)
{
	m_quad = vextq_u32( v.m_quad, v.m_quad, 3);
}

template <hkVectorPermutation::Permutation P> 
HK_FORCE_INLINE void hkIntVector::setPermutation(hkIntVectorParameter v)
{
	const uint32x4_t val = v.m_quad;

    // Add Switch to fix Clang issue were last parameter of vgetq_lane_u32 must be in constrange(0,3)
    // See RSYS-1377 for more information
    const int first = (P >> 12) & 0x3;
    const int second = (P >> 8) & 0x3;
    const int third = (P >> 4) & 0x3;
    const int fourth = (P >> 0) & 0x3;
    
    switch(first)
    {
        case 1:  m_quad = vsetq_lane_u32(vgetq_lane_u32(val, 1), m_quad, 0); break;
        case 2:  m_quad = vsetq_lane_u32(vgetq_lane_u32(val, 2), m_quad, 0); break;
        case 3:  m_quad = vsetq_lane_u32(vgetq_lane_u32(val, 3), m_quad, 0); break;
        default: m_quad = vsetq_lane_u32(vgetq_lane_u32(val, 0), m_quad, 0); break;
    }
    switch(second)
    {
        case 1:  m_quad = vsetq_lane_u32(vgetq_lane_u32(val, 1), m_quad, 1); break;
        case 2:  m_quad = vsetq_lane_u32(vgetq_lane_u32(val, 2), m_quad, 1); break;
        case 3:  m_quad = vsetq_lane_u32(vgetq_lane_u32(val, 3), m_quad, 1); break;
        default: m_quad = vsetq_lane_u32(vgetq_lane_u32(val, 0), m_quad, 1); break;
    }
    switch(third)
    {
        case 1:  m_quad = vsetq_lane_u32(vgetq_lane_u32(val, 1), m_quad, 2); break;
        case 2:  m_quad = vsetq_lane_u32(vgetq_lane_u32(val, 2), m_quad, 2); break;
        case 3:  m_quad = vsetq_lane_u32(vgetq_lane_u32(val, 3), m_quad, 2); break;
        default: m_quad = vsetq_lane_u32(vgetq_lane_u32(val, 0), m_quad, 2); break;
    }
    switch(fourth)
    {
        case 1:  m_quad = vsetq_lane_u32(vgetq_lane_u32(val, 1), m_quad, 3); break;
        case 2:  m_quad = vsetq_lane_u32(vgetq_lane_u32(val, 2), m_quad, 3); break;
        case 3:  m_quad = vsetq_lane_u32(vgetq_lane_u32(val, 3), m_quad, 3); break;
        default: m_quad = vsetq_lane_u32(vgetq_lane_u32(val, 0), m_quad, 3); break;
    }
}

HK_FORCE_INLINE void hkIntVector::setPermuteU8(hkIntVectorParameter a, hkIntVectorParameter mask)
{
	HK_ON_DEBUG( const hkUint8* m = (const hkUint8*)&mask.m_quad; for (int i = 0; i < 16; i++) { HK_MATH_ASSERT(0xf820d0c2, (m[i] & 0xF0)==0, "upper 4 bits not zero"); } )

		const uint8x16_t bytes = vreinterpretq_u8_u32( a.m_quad );

	// Create a 2x table of the source values.
	const uint8x8x2_t bytesLoHi = 
	{
		{
            vget_low_u8( bytes ),
            vget_high_u8( bytes )
		}

	};

	const uint8x8_t maskLo = vget_low_u8( vreinterpretq_u8_u32(mask.m_quad) );
	const uint8x8_t maskHi = vget_high_u8( vreinterpretq_u8_u32(mask.m_quad) );

	uint8x8_t loRes = vtbl2_u8( bytesLoHi, maskLo );
	uint8x8_t hiRes = vtbl2_u8( bytesLoHi, maskHi );

	m_quad = vreinterpretq_u32_u8( vcombine_u8( loRes, hiRes ) );
}

HK_FORCE_INLINE void hkIntVector::setPermuteU8(hkIntVectorParameter a, hkIntVectorParameter b, hkIntVectorParameter mask)
{
	const uint8x16_t bytesA = vreinterpretq_u8_u32( a.m_quad );
	const uint8x16_t bytesB = vreinterpretq_u8_u32( b.m_quad );

	// Create a 4x table of the source values.
	const uint8x8x4_t table = 
	{
		{
			vget_low_u8( bytesA ),
			vget_high_u8( bytesA ),
			vget_low_u8( bytesB ),
			vget_high_u8( bytesB ),
		}
	};

	const uint8x8_t maskLo = vget_low_u8( vreinterpretq_u8_u32(mask.m_quad) );
	const uint8x8_t maskHi = vget_high_u8( vreinterpretq_u8_u32(mask.m_quad) );

	uint8x8_t loRes = vtbl4_u8( table, maskLo );
	uint8x8_t hiRes = vtbl4_u8( table, maskHi );

	m_quad = vreinterpretq_u32_u8( vcombine_u8( loRes, hiRes ) );
}

//
// Splat
//
template <int VALUE> 
HK_FORCE_INLINE void hkIntVector::splatImmediate32()
{
	HK_INT_VECTOR_IMM_SPLAT_VALUE_CHECK;
	m_quad = vreinterpretq_u32_s32( vdupq_n_s32(VALUE) );
}

template <int VALUE> 
HK_FORCE_INLINE void hkIntVector::splatImmediate16()
{
	HK_INT_VECTOR_IMM_SPLAT_VALUE_CHECK;
	m_quad = vreinterpretq_u32_s16( vdupq_n_s16(VALUE) );
}

template <int VALUE> 
HK_FORCE_INLINE void hkIntVector::splatImmediate8()
{
	HK_INT_VECTOR_IMM_SPLAT_VALUE_CHECK;
	m_quad = vreinterpretq_u32_s8( vdupq_n_s8(VALUE) );
}

template <int I> 
HK_FORCE_INLINE void hkIntVector::setBroadcast(hkIntVectorParameter a)
{
	HK_INT_VECTOR_SUBINDEX_CHECK;
    // Add Switch to fix Clang issue were last parameter of vgetq_lane_u32 must be in constrange(0,3)
    // See RSYS-1377 for more information
    switch(I)
    {
        case 1:  m_quad = vdupq_n_u32( vgetq_lane_u32( a.m_quad, 1 ) ); break;
        case 2:  m_quad = vdupq_n_u32( vgetq_lane_u32( a.m_quad, 2 ) ); break;
        case 3:  m_quad = vdupq_n_u32( vgetq_lane_u32( a.m_quad, 3 ) ); break;
        default: m_quad = vdupq_n_u32( vgetq_lane_u32( a.m_quad, 0 ) ); break;
    }
}

template<int I>
HK_FORCE_INLINE void hkIntVector::setBroadcast8(hkIntVectorParameter a)
{
    // Add Switch to fix Clang issue were last parameter of vgetq_lane_u8 must be in constrange(0,15)
    // See RSYS-1377 for more information
    switch(I)
    {
        case 1:  m_quad = vreinterpretq_u32_u8( vdupq_n_u8( vgetq_lane_u8( vreinterpretq_u8_u32(a.m_quad), 1 ) ) ); break;
        case 2:  m_quad = vreinterpretq_u32_u8( vdupq_n_u8( vgetq_lane_u8( vreinterpretq_u8_u32(a.m_quad), 2 ) ) ); break;
        case 3:  m_quad = vreinterpretq_u32_u8( vdupq_n_u8( vgetq_lane_u8( vreinterpretq_u8_u32(a.m_quad), 3 ) ) ); break;
        case 4:  m_quad = vreinterpretq_u32_u8( vdupq_n_u8( vgetq_lane_u8( vreinterpretq_u8_u32(a.m_quad), 4 ) ) ); break;
        case 5:  m_quad = vreinterpretq_u32_u8( vdupq_n_u8( vgetq_lane_u8( vreinterpretq_u8_u32(a.m_quad), 5 ) ) ); break;
        case 6:  m_quad = vreinterpretq_u32_u8( vdupq_n_u8( vgetq_lane_u8( vreinterpretq_u8_u32(a.m_quad), 6 ) ) ); break;
        case 7:  m_quad = vreinterpretq_u32_u8( vdupq_n_u8( vgetq_lane_u8( vreinterpretq_u8_u32(a.m_quad), 7 ) ) ); break;
        case 8:  m_quad = vreinterpretq_u32_u8( vdupq_n_u8( vgetq_lane_u8( vreinterpretq_u8_u32(a.m_quad), 8 ) ) ); break;
        case 9:  m_quad = vreinterpretq_u32_u8( vdupq_n_u8( vgetq_lane_u8( vreinterpretq_u8_u32(a.m_quad), 9 ) ) ); break;
        case 10: m_quad = vreinterpretq_u32_u8( vdupq_n_u8( vgetq_lane_u8( vreinterpretq_u8_u32(a.m_quad), 10 ) ) ); break;
        case 11: m_quad = vreinterpretq_u32_u8( vdupq_n_u8( vgetq_lane_u8( vreinterpretq_u8_u32(a.m_quad), 11 ) ) ); break;
        case 12: m_quad = vreinterpretq_u32_u8( vdupq_n_u8( vgetq_lane_u8( vreinterpretq_u8_u32(a.m_quad), 12 ) ) ); break;
        case 13: m_quad = vreinterpretq_u32_u8( vdupq_n_u8( vgetq_lane_u8( vreinterpretq_u8_u32(a.m_quad), 13 ) ) ); break;
        case 14: m_quad = vreinterpretq_u32_u8( vdupq_n_u8( vgetq_lane_u8( vreinterpretq_u8_u32(a.m_quad), 14 ) ) ); break;
        case 15: m_quad = vreinterpretq_u32_u8( vdupq_n_u8( vgetq_lane_u8( vreinterpretq_u8_u32(a.m_quad), 15 ) ) ); break;
        default: m_quad = vreinterpretq_u32_u8( vdupq_n_u8( vgetq_lane_u8( vreinterpretq_u8_u32(a.m_quad), 0 ) ) ); break;
    }
}

template<int I>
HK_FORCE_INLINE void hkIntVector::setBroadcast16(hkIntVectorParameter a)
{
    // Add Switch to fix Clang issue were last parameter of vgetq_lane_u16 must be in constrange(0,7)
    // See RSYS-1377 for more information
    switch(I)
    {
        case 1:  m_quad = vreinterpretq_u32_u16( vdupq_n_u16( vgetq_lane_u16( vreinterpretq_u16_u32(a.m_quad), 1 ) ) ); break;
        case 2:  m_quad = vreinterpretq_u32_u16( vdupq_n_u16( vgetq_lane_u16( vreinterpretq_u16_u32(a.m_quad), 2 ) ) ); break;
        case 3:  m_quad = vreinterpretq_u32_u16( vdupq_n_u16( vgetq_lane_u16( vreinterpretq_u16_u32(a.m_quad), 3 ) ) ); break;
        case 4:  m_quad = vreinterpretq_u32_u16( vdupq_n_u16( vgetq_lane_u16( vreinterpretq_u16_u32(a.m_quad), 4 ) ) ); break;
        case 5:  m_quad = vreinterpretq_u32_u16( vdupq_n_u16( vgetq_lane_u16( vreinterpretq_u16_u32(a.m_quad), 5 ) ) ); break;
        case 6:  m_quad = vreinterpretq_u32_u16( vdupq_n_u16( vgetq_lane_u16( vreinterpretq_u16_u32(a.m_quad), 6 ) ) ); break;
        case 7:  m_quad = vreinterpretq_u32_u16( vdupq_n_u16( vgetq_lane_u16( vreinterpretq_u16_u32(a.m_quad), 7 ) ) ); break;
        default: m_quad = vreinterpretq_u32_u16( vdupq_n_u16( vgetq_lane_u16( vreinterpretq_u16_u32(a.m_quad), 0 ) ) ); break;
    }
}

//
// int <-> float conversion
//

HK_FORCE_INLINE void hkIntVector::convertU32ToF32(hkVector4f& vOut) const
{
	vOut.m_quad = vcvtq_f32_u32(m_quad);
}

HK_FORCE_INLINE void hkIntVector::convertU32ToF32(hkVector4d& vOut) const
{
	vOut.set( hkDouble64(getU32(0)), hkDouble64(getU32(1)), hkDouble64(getU32(2)), hkDouble64(getU32(3)));
}

HK_FORCE_INLINE void hkIntVector::convertS32ToF32(hkVector4f& vOut) const
{
	vOut.m_quad = vcvtq_f32_s32( vreinterpretq_s32_u32(m_quad) );
}

HK_FORCE_INLINE void hkIntVector::convertS32ToF32(hkVector4d& vOut) const
{
	vOut.set( hkDouble64(getComponent<0>()), hkDouble64(getComponent<1>()), hkDouble64(getComponent<2>()), hkDouble64(getComponent<3>()));
}


namespace 
{
	HK_FORCE_INLINE hkUint32 __f32_u32(hkFloat32 a)
	{
		union F32U32
		{
			hkFloat32 m_asF32;
			hkUint32 m_asU32;
		};

		F32U32 value; value.m_asF32 = a;
		const hkUint32 biasedExponent = (value.m_asU32 & 0x7F800000) >> 23; 
		const int exponent = (hkInt32)biasedExponent - 127;

		// Clip negative and fractional numbers
		if (exponent < 0 || (value.m_asU32 & 0x80000000)) 
		{
			return 0;
		}

		// Obtain mantissa and shift it according to the exponent
		hkUint32 mantissa = (value.m_asU32 & 0x007FFFFF) | 0x00800000;	
		if (exponent <= 23)
		{
			mantissa >>= (23 - exponent);
		}
		else
		{		
			if (exponent < 32)
			{
				mantissa <<= (exponent - 23);
			}

			// Saturate values over 2^32 - 1
			else
			{
				mantissa = 0xFFFFFFFF;
			}
		}

		return mantissa;
	}
}

HK_FORCE_INLINE void hkIntVector::setConvertF32toU32(hkVector4dParameter vIn)
{
	hkUint32 qu[4];
	qu[0] = __f32_u32( hkFloat32(vIn(0)) );
	qu[1] = __f32_u32( hkFloat32(vIn(1)) );
	qu[2] = __f32_u32( hkFloat32(vIn(2)) );
	qu[3] = __f32_u32( hkFloat32(vIn(3)) );
	loadNotAligned<4>(qu);
}

HK_FORCE_INLINE void hkIntVector::setConvertF32toU32(hkVector4fParameter vIn)
{
	m_quad = vcvtq_u32_f32(vIn.m_quad);
}

HK_FORCE_INLINE void hkIntVector::setConvertF32toS32(hkVector4dParameter vIn)
{
	hkVector4f vfIn;
	vfIn.set(hkFloat32(vIn.getComponent<0>().getReal()),hkFloat32(vIn.getComponent<1>().getReal()),hkFloat32(vIn.getComponent<2>().getReal()), hkFloat32(vIn.getComponent<3>().getReal()));
	setConvertF32toS32(vfIn);
}

HK_FORCE_INLINE void hkIntVector::setConvertF32toS32(hkVector4fParameter vIn)
{
	m_quad = vreinterpretq_u32_s32( vcvtq_s32_f32(vIn.m_quad) );
}


template <>
HK_FORCE_INLINE void hkIntVector::load<4>(const hkUint32* p)
{
	HK_MATH_ASSERT(0x70aae483, ((hkUlong)p & 0x7) == 0, "pointer for hkIntVector::load<4> must be 8-byte aligned");
	m_quad = vld1q_u32(p);
}

template <int N>
HK_FORCE_INLINE void hkIntVector::load(const hkUint32* p)
{
	HK_MATH_ASSERT(0x70aae483, ( ((hkUlong)p) & ((sizeof(hkUint32)*(N!=3?N:4) )-1) ) == 0, "p must be aligned.");
	loadNotAligned<N>(p);
}

//
//	Store to an aligned address

template <>
HK_FORCE_INLINE void hkIntVector::store<4>(hkUint32* p) const
{
	HK_MATH_ASSERT(0x70aae483, ((hkUlong)p & 0x7) == 0, "pointer for hkIntVector::store<4> must be 8-byte aligned");
	vst1q_u32( p, m_quad );
}

template <int N>
HK_FORCE_INLINE void hkIntVector::store(hkUint32* p) const
{
	HK_MATH_ASSERT(0x70aae483, ( ((hkUlong)p) & ((sizeof(hkUint32)*(N!=3?N:4))-1) ) == 0, "p must be aligned.");
	storeNotAligned<N>(p);
}

//
//	Assuming that this = (i0, i1, i2, i3) and v = (v0, v1, v2, v3), the function will set this = (ik, ik, ik, ik)
//	where k ={0,..., 3} such that vk = max{v0, v1, v2, v3}.
HK_FORCE_INLINE void hkIntVector::broadcastComponentAtVectorMax(hkVector4Parameter v)
{
	setAll(getComponentAtVectorMax(v));
}

/// Assuming that this = (i0, i1, i2, i3) and v = (v0, v1, v2, v3), the function will return ik
/// where k in {0,..., 3} such that vk = max{v0, v1, v2, v3}.

HK_FORCE_INLINE int hkIntVector::getComponentAtVectorMax(hkVector4Parameter v) const
{
	const hkUint32* HK_RESTRICT qu = (const hkUint32*)&m_quad;

	const int vmax = v.getIndexOfMaxComponent<4>();

	return qu[vmax];
}

HK_FORCE_INLINE void hkIntVector::setSelect( hkVector4fComparisonParameter select, hkIntVectorParameter trueValue, hkIntVectorParameter falseValue )
{
	m_quad = vbslq_u32( select.m_mask, trueValue.m_quad, falseValue.m_quad );
}

HK_FORCE_INLINE void hkIntVector::setSelect(hkVector4dComparisonParameter selectMask, hkIntVectorParameter trueValue, hkIntVectorParameter falseValue)
{
	const hkUint32* HK_RESTRICT qt = reinterpret_cast<const hkUint32*>(&trueValue.m_quad);
	const hkUint32* HK_RESTRICT qf = reinterpret_cast<const hkUint32*>(&falseValue.m_quad);
	hkUint32* HK_RESTRICT qu = reinterpret_cast<hkUint32*>(&m_quad);

	qu[0] = selectMask.anyIsSet<hkVector4ComparisonMask::MASK_X>() ? qt[0] : qf[0];
	qu[1] = selectMask.anyIsSet<hkVector4ComparisonMask::MASK_Y>() ? qt[1] : qf[1];
	qu[2] = selectMask.anyIsSet<hkVector4ComparisonMask::MASK_Z>() ? qt[2] : qf[2];
	qu[3] = selectMask.anyIsSet<hkVector4ComparisonMask::MASK_W>() ? qt[3] : qf[3];
}

template<hkVector4Comparison::Mask M> 
HK_FORCE_INLINE void hkIntVector::setSelect( hkIntVectorParameter trueValue, hkIntVectorParameter falseValue )
{
	hkVector4Comparison select; select.set<M>();
	m_quad = vbslq_u32( select.m_mask, trueValue.m_quad, falseValue.m_quad );
}

template <hkIntVectorConstant vectorConstant>
HK_FORCE_INLINE /*static*/ const hkIntVector& HK_CALL hkIntVector::getConstant()
{
	return *(const hkIntVector*) (g_intVectorConstants + vectorConstant);
}

HK_FORCE_INLINE /*static*/ const hkIntVector& HK_CALL hkIntVector::getConstant(hkIntVectorConstant constant)
{
	return *(const hkIntVector*) (g_intVectorConstants + constant);
}

/// result.u32[i] = highshorts.u16[i]<<16 + lowShorts.u16[i]. highShorts.u16/lowShorts.u16[4..7] are ignored
HK_FORCE_INLINE void hkIntVector::setCombineHead16To32( hkIntVectorParameter highShorts, hkIntVectorParameter lowShorts )
{
	setMergeHead16(lowShorts, highShorts);
}

/// result.u32[i] = highshorts.u16[4+i]<<16 + lowShorts.u16[4+i]. highShorts.u16/lowShorts.u16[0..3] are ignored
HK_FORCE_INLINE void hkIntVector::setCombineTail16To32( hkIntVectorParameter highShorts, hkIntVectorParameter lowShorts )
{
	setMergeTail16(lowShorts, highShorts);
}

/// result.u16[i] = highshorts.u8[i]<<16 + lowShorts.u8[i]. highShorts.u8/lowShorts.u8[8..15] are ignored
HK_FORCE_INLINE void hkIntVector::setCombineHead8To16( hkIntVectorParameter highShorts, hkIntVectorParameter lowShorts )
{
	setMergeHead8(lowShorts, highShorts);
}

/// result.u16[i] = highshorts.u8[8+i]<<16 + lowShorts.u8[8+i]. highShorts.u8/lowShorts.u8[0..7] are ignored
HK_FORCE_INLINE void hkIntVector::setCombineTail8To16( hkIntVectorParameter highShorts, hkIntVectorParameter lowShorts )
{
	setMergeTail8(lowShorts, highShorts);
}

HK_FORCE_INLINE void hkIntVector::setSplit8To32( hkIntVectorParameter bytes )
{
	hkIntVector zero;zero.setZero();
	hkIntVector i16; i16.setCombineHead8To16( zero, bytes );
	setCombineHead16To32(zero, i16);
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
