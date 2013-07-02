/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#if defined(HK_HALF_IS_FLOAT)
#error not implemented
#endif

namespace hkHalf8Util
{

template<hkMathRoundingMode R, hkMathIoMode I>
/*static*/ HK_FORCE_INLINE void pack( hkVector4fParameter v0, hkVector4fParameter v1, hkHalf* HK_RESTRICT packedHalf8 )
{
	hkVector4f vs0, vs1; 
	if ( R == HK_ROUND_NEAREST )
	{
		vs0.setMul( v0, hkVector4f::getConstant<HK_QUADREAL_PACK_HALF>());
		vs1.setMul( v1, hkVector4f::getConstant<HK_QUADREAL_PACK_HALF>());
	}
	else
	{
		vs0 = v0;
		vs1 = v1;
	}

	uint32x4_t vsi0 = vreinterpretq_u32_f32(vs0.m_quad);
	uint32x4_t vsi1 = vreinterpretq_u32_f32(vs1.m_quad);
	uint16x4_t r0 = vshrn_n_u32(vsi0, 16);
	uint16x4_t r1 = vshrn_n_u32(vsi1, 16);
	
	hkVector4f p; p.m_quad = vreinterpretq_f32_u32(vreinterpretq_u32_u16(vcombine_u16(r0, r1)) );
	p.store<4>((hkFloat32*) packedHalf8);
}


/*static*/ HK_FORCE_INLINE void unpackFirst( hkVector4fParameter packedHalf8, hkVector4f& v0 )
{
	
	uint32x4_t r0 = vshll_n_u16(vget_low_u16(vreinterpretq_u16_u32(vreinterpretq_u32_f32(packedHalf8.m_quad))), 16);
	v0.m_quad = vreinterpretq_f32_u32(r0);
}

/*static*/ HK_FORCE_INLINE void unpackSecond( hkVector4fParameter packedHalf8, hkVector4f& v1 )
{
	uint32x4_t r1 = vshll_n_u16(vget_low_u16(vreinterpretq_u16_u32(vreinterpretq_u32_f32(packedHalf8.m_quad))), 16);
	v1.m_quad = vreinterpretq_f32_u32(r1);
}


template<hkMathIoMode I>
/*static*/ HK_FORCE_INLINE void unpack( const hkHalf* HK_RESTRICT packedHalf8, hkVector4f* HK_RESTRICT v0, hkVector4f* HK_RESTRICT v1 )
{
	hkVector4f h; h.load<4>( (const hkFloat32*)packedHalf8 );
	unpackFirst( h, *v0 );
	unpackSecond( h, *v1 );
}


template<hkMathRoundingMode A>
/*static*/ HK_FORCE_INLINE void packInterleaved( hkVector4fParameter v0, hkVector4fParameter v1, hkVector4f& packedHalf8 )
{
	HK_ERROR(0x68abe9e1, "Not implemented");
}

/*static*/ HK_FORCE_INLINE void unpackFirstInterleaved( hkVector4fParameter packedHalf8, hkVector4f& v0 )
{
	HK_ERROR(0xc50f0b2, "Not implemented");
}

/*static*/ HK_FORCE_INLINE void unpackSecondInterleaved( hkVector4fParameter packedHalf8, hkVector4f& v1 )
{
	HK_ERROR(0x42f5c60c, "Not implemented");
}

/*static*/ HK_FORCE_INLINE void unpackInterleaved( hkVector4fParameter packedHalf8, hkVector4f* HK_RESTRICT v0, hkVector4f* HK_RESTRICT v1 )
{
	HK_ERROR(0x568b4066, "Not implemented");
}

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
