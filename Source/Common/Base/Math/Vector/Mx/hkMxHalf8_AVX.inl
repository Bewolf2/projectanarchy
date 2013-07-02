/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#if (HK_SSE_VERSION < 0x50)
#error This implementation is for 32-Bit float with AVX SIMD instruction set
#endif

#if 0

pk.todo

namespace hkMxHalf8_Implementation
{
template <int I> HK_FORCE_INLINE void reducePrecisionH8H(__m256* v, const __m256& mask) { reducePrecisionH8H<I-1>(v,mask); v[I-1] = _mm256_and_ps(v[I-1],mask); }
template <> HK_FORCE_INLINE void reducePrecisionH8H<1>(__m256* v, const __m256& mask) { v[0] = _mm256_and_ps(v[0],mask); }
}
template <int M>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxHalf8<M>::reducePrecision(hkMxVector<M>& v )
{
	static HK_ALIGN32( const hkUint32 mask[8] ) = { 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000 };
	hkMxHalf8_Implementation::reducePrecisionH8H<((M+1)>>1)>(v.m_vec.v,*(const __m256*)&mask);
}



namespace hkMxHalf8_Implementation
{
template <int I, int N> struct packH8H { HK_FORCE_INLINE static void apply(hkHalf8* h, const __m256* vp0, const __m256* vp1) 
{ 
	hkVector4 v0,v1,v2,v3;
	v0.m_quad = _mm256_extractf128_ps(vp0[N], 0x0);
	v1.m_quad = _mm256_extractf128_ps(vp0[N], 0x1);
	v2.m_quad = _mm256_extractf128_ps(vp1[N], 0x0);
	v3.m_quad = _mm256_extractf128_ps(vp1[N], 0x1);
	packH8H<I-2,N+1>::apply(h,vp0,vp1);
	h[2*N].pack<false>(v0,v2); 
	h[2*N+1].pack<false>(v1,v3); 
} };
template <int N> struct packH8H<2,N> { HK_FORCE_INLINE static void apply(hkHalf8* h, const __m256* vp0, const __m256* vp1) 
{ 
	hkVector4 v0,v1,v2,v3;
	v0.m_quad = _mm256_extractf128_ps(vp0[N], 0x0);
	v1.m_quad = _mm256_extractf128_ps(vp0[N], 0x1);
	v2.m_quad = _mm256_extractf128_ps(vp1[N], 0x0);
	v3.m_quad = _mm256_extractf128_ps(vp1[N], 0x1);
	h[2*N].pack<false>(v0,v2); 
	h[2*N+1].pack<false>(v1,v3); 
} };
template <int N> struct packH8H<1,N> { HK_FORCE_INLINE static void apply(hkHalf8* h, const __m256* vp0, const __m256* vp1) 
{
	hkVector4 v0,v1;
	v0.m_quad = _mm256_extractf128_ps(vp0[N], 0x0);
	v1.m_quad = _mm256_extractf128_ps(vp1[N], 0x0);
	h[2*N].pack<false>(v0,v1); 
} };
}
template <int M>
HK_FORCE_INLINE void hkMxHalf8<M>::pack(hkMxVectorParameter v0, hkMxVectorParameter v1 )
{
	hkMxHalf8_Implementation::packH8H<M,0>::apply(m_half.h, v0.m_vec.v, v1.m_vec.v);
}

namespace hkMxHalf8_Implementation
{
template <int I, int N> struct packFirstH8H { HK_FORCE_INLINE static void apply(hkHalf8* h, const __m256* vp) 
{ 
	hkVector4 v0,v1;
	v0.m_quad = _mm256_extractf128_ps(vp[N], 0x0);
	v1.m_quad = _mm256_extractf128_ps(vp[N], 0x1);
	packFirstH8H<I-2,N+1>::apply(h,vp);
	h[2*N].packFirst<false>(v0); 
	h[2*N+1].packFirst<false>(v1); 
} };
template <int N> struct packFirstH8H<2,N> { HK_FORCE_INLINE static void apply(hkHalf8* h, const __m256* vp) 
{ 
	hkVector4 v0,v1;
	v0.m_quad = _mm256_extractf128_ps(vp[N], 0x0);
	v1.m_quad = _mm256_extractf128_ps(vp[N], 0x1);
	h[2*N].packFirst<false>(v0); 
	h[2*N+1].packFirst<false>(v1); 
} };
template <int N> struct packFirstH8H<1,N> { HK_FORCE_INLINE static void apply(hkHalf8* h, const __m256* vp) 
{ 
	hkVector4 v;
	v.m_quad = _mm256_extractf128_ps(vp[N], 0x0);
	h[2*N].packFirst<false>(v); 
} };
}
template <int M>
HK_FORCE_INLINE void hkMxHalf8<M>::packFirst(hkMxVectorParameter v )
{
	hkMxHalf8_Implementation::packFirstH8H<M,0>::apply(m_half.h, v.m_vec.v);
}

namespace hkMxHalf8_Implementation
{
template <int I, int N> struct packSecondH8H { HK_FORCE_INLINE static void apply(hkHalf8* h, const __m256* vp) 
{ 
	hkVector4 v0,v1;
	v0.m_quad = _mm256_extractf128_ps(vp[N], 0x0);
	v1.m_quad = _mm256_extractf128_ps(vp[N], 0x1);
	packSecondH8H<I-2,N+1>::apply(h,vp);
	h[2*N].packSecond<false>(v0); 
	h[2*N+1].packSecond<false>(v1); 
} };
template <int N> struct packSecondH8H<2,N> { HK_FORCE_INLINE static void apply(hkHalf8* h, const __m256* vp) 
{ 
	hkVector4 v0,v1;
	v0.m_quad = _mm256_extractf128_ps(vp[N], 0x0);
	v1.m_quad = _mm256_extractf128_ps(vp[N], 0x1);
	h[2*N].packSecond<false>(v0); 
	h[2*N+1].packSecond<false>(v1); 
} };
template <int N> struct packSecondH8H<1,N> { HK_FORCE_INLINE static void apply(hkHalf8* h, const __m256* vp) 
{ 
	hkVector4 v;
	v.m_quad = _mm256_extractf128_ps(vp[N], 0x0);
	h[2*N].packSecond<false>(v); 
} };
}
template <int M>
HK_FORCE_INLINE void hkMxHalf8<M>::packSecond(hkMxVectorParameter v )
{
	hkMxHalf8_Implementation::packSecondH8H<M,0>::apply(m_half.h, v.m_vec.v);
}




namespace hkMxHalf8_Implementation
{
template <int I, int N> struct unpackH8H { HK_FORCE_INLINE static void apply(const hkHalf8* h, __m256* HK_RESTRICT vp0, __m256* HK_RESTRICT vp1) 
{ 
	hkVector4 v0,v1,v2,v3;
	h[2*N].unpack(v0,v1);
	h[2*N+1].unpack(v2,v3);
	unpackH8H<I-2,N+1>::apply(h,vp0,vp1);
	vp0[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), v0.m_quad, 0x0);
	vp0[N] = _mm256_insertf128_ps(vp0[N], v2.m_quad, 0x1);
	vp1[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), v1.m_quad, 0x0);
	vp1[N] = _mm256_insertf128_ps(vp1[N], v3.m_quad, 0x1);
} };
template <int N> struct unpackH8H<2,N> { HK_FORCE_INLINE static void apply(const hkHalf8* h, __m256* HK_RESTRICT vp0, __m256* HK_RESTRICT vp1) 
{ 
	hkVector4 v0,v1,v2,v3;
	h[2*N].unpack(v0,v1);
	h[2*N+1].unpack(v2,v3);
	vp0[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), v0.m_quad, 0x0);
	vp0[N] = _mm256_insertf128_ps(vp0[N], v2.m_quad, 0x1);
	vp1[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), v1.m_quad, 0x0);
	vp1[N] = _mm256_insertf128_ps(vp1[N], v3.m_quad, 0x1);
} };
template <int N> struct unpackH8H<1,N> { HK_FORCE_INLINE static void apply(const hkHalf8* h, __m256* HK_RESTRICT vp0, __m256* HK_RESTRICT vp1) 
{ 
	hkVector4 v0,v1;
	h[2*N].unpack(v0,v1);
	vp0[N] = _mm256_insertf128_ps(_mm256_setzero_ps(), v0.m_quad, 0x0);
	vp1[N] = _mm256_insertf128_ps(_mm256_setzero_ps(), v1.m_quad, 0x0);
} };
}
template <int M>
HK_FORCE_INLINE void hkMxHalf8<M>::unpack(hkMxVector<M>& v0, hkMxVector<M>& v1) const
{
	HK_MATH_ASSERT(0xad342288, (hkUlong(&v0) + sizeof(hkMxVector<M>) <= hkUlong(this) || hkUlong(this) + sizeof(hkMxHalf8<M>) <= hkUlong(&v0))
						&& (hkUlong(&v1) + sizeof(hkMxVector<M>) <= hkUlong(this) || hkUlong(this) + sizeof(hkMxHalf8<M>) <= hkUlong(&v1)), 
						"hkMxHalf8 overlaps with the vectors being packed.");
	__m256* HK_RESTRICT v0p = v0.m_vec.v;
	__m256* HK_RESTRICT v1p = v1.m_vec.v;
	hkMxHalf8_Implementation::unpackH8H<M,0>::apply(m_half.h, v0p, v1p);
}

namespace hkMxHalf8_Implementation
{
template <int I, int N> struct unpackFirstH8H { HK_FORCE_INLINE static void apply(const hkHalf8* h, __m256* HK_RESTRICT vp) 
{ 
	hkVector4 v0,v1;
	h[2*N].unpackFirst(v0); 
	h[2*N+1].unpackFirst(v1); 
	unpackFirstH8H<I-2,N+1>(h,vp);
	vp[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), v0.m_quad, 0x0);
	vp[N] = _mm256_insertf128_ps(vp[N], v1.m_quad, 0x1);
} };
template <int N> struct unpackFirstH8H<2,N> { HK_FORCE_INLINE static void apply(const hkHalf8* h, __m256* HK_RESTRICT vp) 
{ 
	hkVector4 v0,v1;
	h[2*N].unpackFirst(v0); 
	h[2*N+1].unpackFirst(v1); 
	vp[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), v0.m_quad, 0x0);
	vp[N] = _mm256_insertf128_ps(vp[N], v1.m_quad, 0x1);
} };
template <int N> struct unpackFirstH8H<1,N> { HK_FORCE_INLINE static void apply(const hkHalf8* h, __m256* HK_RESTRICT vp) 
{ 
	hkVector4 v;
	h[2*N].unpackFirst(v); 
	vp[N] = _mm256_insertf128_ps(_mm256_setzero_ps(), v.m_quad, 0x0);
} };
}
template <int M>
HK_FORCE_INLINE void hkMxHalf8<M>::unpackFirst(hkMxVector<M>& v) const
{
	HK_MATH_ASSERT(0xad342289, hkUlong(&v) + sizeof(hkMxVector<M>) <= hkUlong(this) || hkUlong(this) + sizeof(hkMxHalf8<M>) <= hkUlong(&v), 
						"hkMxHalf8 overlaps with the vectors being packed.");
	__m256* HK_RESTRICT vp = v.m_vec.v;
	hkMxHalf8_Implementation::unpackFirstH8H<M,0>::apply(m_half.h, vp);
}

namespace hkMxHalf8_Implementation
{
template <int I, int N> struct unpackSecondH8H { HK_FORCE_INLINE static void apply(const hkHalf8* h, __m256* HK_RESTRICT vp) 
{ 
	hkVector4 v0,v1;
	h[2*N].unpackSecond(v0); 
	h[2*N+1].unpackSecond(v1); 
	unpackSecondH8H<I-2,N+1>::apply(h,vp);
	vp[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), v0.m_quad, 0x0);
	vp[N] = _mm256_insertf128_ps(vp[N], v1.m_quad, 0x1);
} };
template <int N> struct unpackSecondH8H<2,N> { HK_FORCE_INLINE static void apply(const hkHalf8* h, __m256* HK_RESTRICT vp) 
{ 
	hkVector4 v0,v1;
	h[2*N].unpackSecond(v0); 
	h[2*N+1].unpackSecond(v1); 
	vp[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), v0.m_quad, 0x0);
	vp[N] = _mm256_insertf128_ps(vp[N], v1.m_quad, 0x1);
} };
template <int N> struct unpackSecondH8H<1,N> { HK_FORCE_INLINE static void apply(const hkHalf8* h, __m256* HK_RESTRICT vp) 
{ 
	hkVector4 v;
	h[2*N].unpackSecond(v); 
	vp[N] = _mm256_insertf128_ps(_mm256_setzero_ps(), v.m_quad, 0x0);
} };
}
template <int M>
HK_FORCE_INLINE void hkMxHalf8<M>::unpackSecond(hkMxVector<M>& v) const
{
	HK_MATH_ASSERT(0xad34228a, hkUlong(&v) + sizeof(hkMxVector<M>) <= hkUlong(this) || hkUlong(this) + sizeof(hkMxHalf8<M>) <= hkUlong(&v), 
						"hkMxHalf8 overlaps with the vectors being packed.");
	__m256* HK_RESTRICT vp = v.m_vec.v;
	hkMxHalf8_Implementation::unpackSecondH8H<M,0>::apply(m_half.h, vp);
}




namespace hkMxHalf8_Implementation
{
template <int I> HK_FORCE_INLINE void setZeroH8H(hkHalf8* v) { setZeroH8H<I-1>(v); v[I-1].setZero(); }
template <> HK_FORCE_INLINE void setZeroH8H<1>(hkHalf8* v) { v[0].setZero(); }
}
template <int M>
HK_FORCE_INLINE void hkMxHalf8<M>::setZero()
{
	hkMxHalf8_Implementation::setZeroH8H<M>(m_half.h);
}


namespace hkMxHalf8_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct scatterH8H {
HK_FORCE_INLINE static void apply(const hkHalf8* v, hkHalf8* base)
{
	hkHalf8* HK_RESTRICT ptr0 = hkAddByteOffset(base, (I-1) * byteAddressIncrement); 
	scatterH8H<I-1, byteAddressIncrement>::apply(v, base);
	v[I-1].storePacked( (hkHalf*) ptr0 );
	//*ptr0 = v[I-1];
} };
template <hkUint32 byteAddressIncrement> struct scatterH8H<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkHalf8* v, hkHalf8* base)
{
	v[0].storePacked( (hkHalf*) base );
	//*base = v[0];
} };
}
template <int M>
template <hkUint32 byteAddressIncrement>
HK_FORCE_INLINE void hkMxHalf8<M>::scatter(hkHalf8* base) const
{
	hkMxHalf8_Implementation::scatterH8H<M, byteAddressIncrement>::apply(m_half.h, base);
}

#endif

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
