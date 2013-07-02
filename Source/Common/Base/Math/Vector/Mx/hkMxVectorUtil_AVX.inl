/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#if defined(HK_REAL_IS_DOUBLE) || (HK_SSE_VERSION < 0x50)
#error This implementation is for 32-Bit float with AVX SIMD instruction set
#endif

#include <Common/Base/Math/Vector/Mx/hkMxCommon_AVX.inl>

#if 0

namespace hkMxVectorUtil_Implementation
{
template <int I, int N> struct loadUnpack2H { HK_FORCE_INLINE static void apply(const hkHalf8* base, __m256* vOut0)
{
	hkVector4f h0,h1; base[N].unpack(h0,h1); 
	loadUnpack2H<I-2,N+1>::apply(base, vOut0);
	vOut0[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), h0.m_quad, 0x0);
	vOut0[N] = _mm256_insertf128_ps(vOut0[N], h1.m_quad, 0x1);
} };
template <int N> struct loadUnpack2H<2,N> { HK_FORCE_INLINE static void apply(const hkHalf8* base, __m256* vOut0)
{
	hkVector4f h0,h1; base[N].unpack(h0,h1); 
	vOut0[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), h0.m_quad, 0x0);
	vOut0[N] = _mm256_insertf128_ps(vOut0[N], h1.m_quad, 0x1);
} };
template <int N> struct loadUnpack2H<1,N> { HK_FORCE_INLINE static void apply(const hkHalf8* base, __m256* vOut0)
{
	hkVector4f h0; base[N].unpackFirst(h0); 
	vOut0[N] = _mm256_insertf128_ps(_mm256_setzero_ps(), h0.m_quad, 0x0);
} };
}
template <int M>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::loadUnpack(const hkHalf8* base, hkMxVectorf<M>& vOut0)
{
	hkMxVectorUtil_Implementation::loadUnpack2H<M,0>::apply(base, vOut0.m_vec.v);
}





namespace hkMxVectorUtil_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct gatherUnpackH {
HK_FORCE_INLINE static void apply(const hkHalf8* base, __m256* vOut0, __m256* vOut1)
{
	hkVector4f h0,h1,h2,h3;
	hkAddByteOffsetConst( base, (2*N) * byteAddressIncrement )->unpack(h0,h1); 
	hkAddByteOffsetConst( base, (2*N+1) * byteAddressIncrement )->unpack(h2,h3); 
	gatherUnpackH<I-2, N+1, byteAddressIncrement>::apply(base, vOut0, vOut1);
	vOut0[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), h0.m_quad, 0x0);
	vOut0[N] = _mm256_insertf128_ps(vOut0[N], h2.m_quad, 0x1);
	vOut1[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), h1.m_quad, 0x0);
	vOut1[N] = _mm256_insertf128_ps(vOut1[N], h3.m_quad, 0x1);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherUnpackH<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkHalf8* base, __m256* vOut0, __m256* vOut1)
{
	hkVector4f h0,h1,h2,h3;
	hkAddByteOffsetConst( base, (2*N) * byteAddressIncrement )->unpack(h0,h1); 
	hkAddByteOffsetConst( base, (2*N+1) * byteAddressIncrement )->unpack(h2,h3); 
	vOut0[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), h0.m_quad, 0x0);
	vOut0[N] = _mm256_insertf128_ps(vOut0[N], h2.m_quad, 0x1);
	vOut1[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), h1.m_quad, 0x0);
	vOut1[N] = _mm256_insertf128_ps(vOut1[N], h3.m_quad, 0x1);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherUnpackH<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkHalf8* base, __m256* vOut0, __m256* vOut1)
{
	hkVector4f h0,h1;
	hkAddByteOffsetConst( base, (2*N) * byteAddressIncrement )->unpack(h0,h1); 
	vOut0[N] = _mm256_insertf128_ps(_mm256_setzero_ps(), h0.m_quad, 0x0);
	vOut1[N] = _mm256_insertf128_ps(_mm256_setzero_ps(), h1.m_quad, 0x0);
} };
}
template <int M, hkUint32 byteAddressIncrement>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::gatherUnpack(const hkHalf8* base, hkMxVectorf<M>& vOut0, hkMxVectorf<M>& vOut1)
{
	hkMxVectorUtil_Implementation::gatherUnpackH<M, 0, byteAddressIncrement>::apply(base, vOut0.m_vec.v, vOut1.m_vec.v);
}


namespace hkMxVectorUtil_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct gatherUnpack2Hu {
HK_FORCE_INLINE static void apply(const hkHalf8* base, const hkUint16* indices, __m256* vOut0, __m256* vOut1)
{
	hkVector4f h0,h1,h2,h3;
	hkAddByteOffsetConst( base, indices[2*N] * byteAddressIncrement )->unpack(h0,h1); 
	hkAddByteOffsetConst( base, indices[2*N+1] * byteAddressIncrement )->unpack(h2,h3); 
	gatherUnpack2Hu<I-2,N+1, byteAddressIncrement>::apply(base, indices, vOut0, vOut1);
	vOut0[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), h0.m_quad, 0x0);
	vOut0[N] = _mm256_insertf128_ps(vOut0[N], h2.m_quad, 0x1);
	vOut1[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), h1.m_quad, 0x0);
	vOut1[N] = _mm256_insertf128_ps(vOut1[N], h3.m_quad, 0x1);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherUnpack2Hu<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkHalf8* base, const hkUint16* indices, __m256* vOut0, __m256* vOut1)
{
	hkVector4f h0,h1,h2,h3;
	hkAddByteOffsetConst( base, indices[2*N] * byteAddressIncrement )->unpack(h0,h1); 
	hkAddByteOffsetConst( base, indices[2*N+1] * byteAddressIncrement )->unpack(h2,h3); 
	vOut0[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), h0.m_quad, 0x0);
	vOut0[N] = _mm256_insertf128_ps(vOut0[N], h2.m_quad, 0x1);
	vOut1[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), h1.m_quad, 0x0);
	vOut1[N] = _mm256_insertf128_ps(vOut1[N], h3.m_quad, 0x1);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherUnpack2Hu<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkHalf8* base, const hkUint16* indices, __m256* vOut0, __m256* vOut1)
{
	hkVector4f h0,h1;
	hkAddByteOffsetConst( base, indices[2*N] * byteAddressIncrement )->unpack(h0,h1); 
	vOut0[N] = _mm256_insertf128_ps(_mm256_setzero_ps(), h0.m_quad, 0x0);
	vOut1[N] = _mm256_insertf128_ps(_mm256_setzero_ps(), h1.m_quad, 0x0);
} };
}
template <int M, hkUint32 byteAddressIncrement>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::gatherUnpack(const hkHalf8* base, const hkUint16* indices, hkMxVectorf<M>& vOut0, hkMxVectorf<M>& vOut1)
{
	hkMxVectorUtil_Implementation::gatherUnpack2Hu<M, 0, byteAddressIncrement>::apply(base, indices, vOut0.m_vec.v, vOut1.m_vec.v);
}


namespace hkMxVectorUtil_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct gatherUnpack2H {
HK_FORCE_INLINE static void apply(const hkHalf8* base, const hkInt32* indices, __m256* vOut0, __m256* vOut1)
{
	hkVector4f h0,h1,h2,h3;
	hkAddByteOffsetConst( base, indices[2*N] * byteAddressIncrement )->unpack(h0,h1); 
	hkAddByteOffsetConst( base, indices[2*N+1] * byteAddressIncrement )->unpack(h2,h3); 
	gatherUnpack2H<I-2,N+1, byteAddressIncrement>::apply(base, indices, vOut0, vOut1);
	vOut0[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), h0.m_quad, 0x0);
	vOut0[N] = _mm256_insertf128_ps(vOut0[N], h2.m_quad, 0x1);
	vOut1[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), h1.m_quad, 0x0);
	vOut1[N] = _mm256_insertf128_ps(vOut1[N], h3.m_quad, 0x1);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherUnpack2H<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkHalf8* base, const hkInt32* indices, __m256* vOut0, __m256* vOut1)
{
	hkVector4f h0,h1,h2,h3;
	hkAddByteOffsetConst( base, indices[2*N] * byteAddressIncrement )->unpack(h0,h1); 
	hkAddByteOffsetConst( base, indices[2*N+1] * byteAddressIncrement )->unpack(h2,h3); 
	vOut0[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), h0.m_quad, 0x0);
	vOut0[N] = _mm256_insertf128_ps(vOut0[N], h2.m_quad, 0x1);
	vOut1[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), h1.m_quad, 0x0);
	vOut1[N] = _mm256_insertf128_ps(vOut1[N], h3.m_quad, 0x1);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherUnpack2H<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkHalf8* base, const hkInt32* indices, __m256* vOut0, __m256* vOut1)
{
	hkVector4f h0,h1;
	hkAddByteOffsetConst( base, indices[2*N] * byteAddressIncrement )->unpack(h0,h1); 
	vOut0[N] = _mm256_insertf128_ps(_mm256_setzero_ps(), h0.m_quad, 0x0);
	vOut1[N] = _mm256_insertf128_ps(_mm256_setzero_ps(), h1.m_quad, 0x0);
} };
}
template <int M, hkUint32 byteAddressIncrement>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::gatherUnpack(const hkHalf8* base, const hkInt32* indices, hkMxVectorf<M>& vOut0, hkMxVectorf<M>& vOut1)
{
	hkMxVectorUtil_Implementation::gatherUnpack2H<M, 0, byteAddressIncrement>::apply(base, indices, vOut0.m_vec.v, vOut1.m_vec.v);
}





namespace hkMxVectorUtil_Implementation
{
template <int I, int N, hkUint32 byteOffset> struct gatherUnpackWithOffsetH {
HK_FORCE_INLINE static void apply(__m256* v0, __m256* v1, const void** base)
{
	const hkHalf8* HK_RESTRICT ptr0 = (const hkHalf8*)hkAddByteOffsetConst(base[2*N], byteOffset); 
	const hkHalf8* HK_RESTRICT ptr1 = (const hkHalf8*)hkAddByteOffsetConst(base[2*N+1], byteOffset); 
	hkVector4f hv0,hv1,hv2,hv3;
	ptr0->unpack(hv0,hv1);
	ptr1->unpack(hv2,hv3);
	gatherUnpackWithOffsetH<I-2, N+1, byteOffset>::apply(v0, v1, base);
	v0[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), hv0.m_quad, 0x0);
	v0[N] = _mm256_insertf128_ps(v0[N], hv2.m_quad, 0x1);
	v1[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), hv1.m_quad, 0x0);
	v1[N] = _mm256_insertf128_ps(v1[N], hv3.m_quad, 0x1);
} };
template <int N, hkUint32 byteOffset> struct gatherUnpackWithOffsetH<2, N, byteOffset> {
HK_FORCE_INLINE static void apply(__m256* v0, __m256* v1, const void** base)
{
	const hkHalf8* HK_RESTRICT ptr0 = (const hkHalf8*)hkAddByteOffsetConst(base[2*N], byteOffset); 
	const hkHalf8* HK_RESTRICT ptr1 = (const hkHalf8*)hkAddByteOffsetConst(base[2*N+1], byteOffset); 
	hkVector4f hv0,hv1,hv2,hv3;
	ptr0->unpack(hv0,hv1);
	ptr1->unpack(hv2,hv3);
	v0[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), hv0.m_quad, 0x0);
	v0[N] = _mm256_insertf128_ps(v0[N], hv2.m_quad, 0x1);
	v1[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), hv1.m_quad, 0x0);
	v1[N] = _mm256_insertf128_ps(v1[N], hv3.m_quad, 0x1);
} };
template <int N, hkUint32 byteOffset> struct gatherUnpackWithOffsetH<1, N, byteOffset> {
HK_FORCE_INLINE static void apply(__m256* v0, __m256* v1, const void** base)
{
	const hkHalf8* HK_RESTRICT ptr = (const hkHalf8*)hkAddByteOffsetConst(base[2*N], byteOffset);
	hkVector4f hv0,hv1;
	ptr->unpack(hv0,hv1);
	v0[N] = _mm256_insertf128_ps(_mm256_setzero_ps(), hv0.m_quad, 0x0);
	v1[N] = _mm256_insertf128_ps(_mm256_setzero_ps(), hv1.m_quad, 0x0);
} };
}
template <int M, hkUint32 byteAddressOffset>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::gatherUnpackHalf8WithOffset(const void* base[M], hkMxVectorf<M>& vOut0, hkMxVectorf<M>& vOut1)
{
	hkMxVectorUtil_Implementation::gatherUnpackWithOffsetH<M, 0, byteAddressOffset>::apply(vOut0.m_vec.v, vOut1.m_vec.v, base);
}





namespace hkMxVectorUtil_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct gatherUnpackFirstH {
HK_FORCE_INLINE static void apply(const hkHalf8* base, __m256* vOut)
{
	hkVector4f h0,h1;
	hkAddByteOffsetConst( base, (2*N) * byteAddressIncrement )->unpackFirst(h0); 
	hkAddByteOffsetConst( base, (2*N+1) * byteAddressIncrement )->unpackFirst(h1); 
	gatherUnpackFirstH<I-2,N+1, byteAddressIncrement>::apply(base, vOut);
	vOut[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), h0.m_quad, 0x0);
	vOut[N] = _mm256_insertf128_ps(vOut[N], h1.m_quad, 0x1);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherUnpackFirstH<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkHalf8* base, __m256* vOut)
{
	hkVector4f h0,h1;
	hkAddByteOffsetConst( base, (2*N) * byteAddressIncrement )->unpackFirst(h0); 
	hkAddByteOffsetConst( base, (2*N+1) * byteAddressIncrement )->unpackFirst(h1); 
	vOut[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), h0.m_quad, 0x0);
	vOut[N] = _mm256_insertf128_ps(vOut[N], h1.m_quad, 0x1);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherUnpackFirstH<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkHalf8* base, __m256* vOut)
{
	hkVector4f h;
	hkAddByteOffsetConst( base, (2*N) * byteAddressIncrement )->unpackFirst(h); 
	vOut[N] = _mm256_insertf128_ps(_mm256_setzero_ps(), h.m_quad, 0x0);
} };
}
template <int M, hkUint32 byteAddressIncrement>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::gatherUnpackFirst(const hkHalf8* base, hkMxVectorf<M>& vOut)
{
	hkMxVectorUtil_Implementation::gatherUnpackFirstH<M, 0, byteAddressIncrement>::apply(base, vOut.m_vec.v);
}


namespace hkMxVectorUtil_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct gatherUnpackFirst2Hu {
HK_FORCE_INLINE static void apply(const hkHalf8* base, const hkUint16* indices, __m256* vOut)
{
	hkVector4f h0,h1;
	hkAddByteOffsetConst( base, indices[2*N] * byteAddressIncrement )->unpackFirst(h0); 
	hkAddByteOffsetConst( base, indices[2*N+1] * byteAddressIncrement )->unpackFirst(h1); 
	gatherUnpackFirst2Hu<I-2,N+1, byteAddressIncrement>::apply(base, indices, vOut);
	vOut[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), h0.m_quad, 0x0);
	vOut[N] = _mm256_insertf128_ps(vOut[N], h1.m_quad, 0x1);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherUnpackFirst2Hu<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkHalf8* base, const hkUint16* indices, __m256* vOut)
{
	hkVector4f h0,h1;
	hkAddByteOffsetConst( base, indices[2*N] * byteAddressIncrement )->unpackFirst(h0); 
	hkAddByteOffsetConst( base, indices[2*N+1] * byteAddressIncrement )->unpackFirst(h1); 
	vOut[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), h0.m_quad, 0x0);
	vOut[N] = _mm256_insertf128_ps(vOut[N], h1.m_quad, 0x1);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherUnpackFirst2Hu<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkHalf8* base, const hkUint16* indices, __m256* vOut)
{
	hkVector4f h;
	hkAddByteOffsetConst( base, indices[2*N] * byteAddressIncrement )->unpackFirst(h); 
	vOut[N] = _mm256_insertf128_ps(_mm256_setzero_ps(), h.m_quad, 0x0);
} };
}
template <int M, hkUint32 byteAddressIncrement>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::gatherUnpackFirst(const hkHalf8* base, const hkUint16* indices, hkMxVectorf<M>& vOut)
{
	hkMxVectorUtil_Implementation::gatherUnpackFirst2Hu<M, 0, byteAddressIncrement>::apply(base, indices, vOut.m_vec.v);
}


namespace hkMxVectorUtil_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct gatherUnpackFirst2H {
HK_FORCE_INLINE static void apply(const hkHalf8* base, const hkInt32* indices, __m256* vOut)
{
	hkVector4f h0,h1;
	hkAddByteOffsetConst( base, indices[2*N] * byteAddressIncrement )->unpackFirst(h0); 
	hkAddByteOffsetConst( base, indices[2*N+1] * byteAddressIncrement )->unpackFirst(h1); 
	gatherUnpackFirst2H<I-2,N+1, byteAddressIncrement>::apply(base, indices, vOut);
	vOut[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), h0.m_quad, 0x0);
	vOut[N] = _mm256_insertf128_ps(vOut[N], h1.m_quad, 0x1);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherUnpackFirst2H<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkHalf8* base, const hkInt32* indices, __m256* vOut)
{
	hkVector4f h0,h1;
	hkAddByteOffsetConst( base, indices[2*N] * byteAddressIncrement )->unpackFirst(h0); 
	hkAddByteOffsetConst( base, indices[2*N+1] * byteAddressIncrement )->unpackFirst(h1); 
	vOut[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), h0.m_quad, 0x0);
	vOut[N] = _mm256_insertf128_ps(vOut[N], h1.m_quad, 0x1);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherUnpackFirst2H<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkHalf8* base, const hkInt32* indices, __m256* vOut)
{
	hkVector4f h;
	hkAddByteOffsetConst( base, indices[2*N] * byteAddressIncrement )->unpackFirst(h); 
	vOut[N] = _mm256_insertf128_ps(_mm256_setzero_ps(), h.m_quad, 0x0);
} };
}
template <int M, hkUint32 byteAddressIncrement>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::gatherUnpackFirst(const hkHalf8* base, const hkInt32* indices, hkMxVectorf<M>& vOut)
{
	hkMxVectorUtil_Implementation::gatherUnpackFirst2H<M, 0, byteAddressIncrement>::apply(base, indices, vOut.m_vec.v);
}


namespace hkMxVectorUtil_Implementation
{
template <int I, int N, hkUint32 byteOffset> struct gatherUnpackFirstWithOffsetH {
HK_FORCE_INLINE static void apply(__m256* v, const void** base)
{
	const hkHalf8* HK_RESTRICT ptr0 = (const hkHalf8*)hkAddByteOffsetConst(base[2*N], byteOffset); 
	const hkHalf8* HK_RESTRICT ptr1 = (const hkHalf8*)hkAddByteOffsetConst(base[2*N+1], byteOffset); 
	hkVector4f v0,v1;
	ptr0->unpackFirst(v0);
	ptr1->unpackFirst(v1);
	gatherUnpackFirstWithOffsetH<I-2, N+1, byteOffset>::apply(v, base);
	v[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), v0.m_quad, 0x0);
	v[N] = _mm256_insertf128_ps(v[N], v1.m_quad, 0x1);
} };
template <int N, hkUint32 byteOffset> struct gatherUnpackFirstWithOffsetH<2, N, byteOffset> {
HK_FORCE_INLINE static void apply(__m256* v, const void** base)
{
	const hkHalf8* HK_RESTRICT ptr0 = (const hkHalf8*)hkAddByteOffsetConst(base[2*N], byteOffset); 
	const hkHalf8* HK_RESTRICT ptr1 = (const hkHalf8*)hkAddByteOffsetConst(base[2*N+1], byteOffset); 
	hkVector4f v0,v1;
	ptr0->unpackFirst(v0);
	ptr1->unpackFirst(v1);
	v[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), v0.m_quad, 0x0);
	v[N] = _mm256_insertf128_ps(v[N], v1.m_quad, 0x1);
} };
template <int N, hkUint32 byteOffset> struct gatherUnpackFirstWithOffsetH<1, N, byteOffset> {
HK_FORCE_INLINE static void apply(__m256* v, const void** base)
{
	const hkHalf8* HK_RESTRICT ptr = (const hkHalf8*)hkAddByteOffsetConst(base[2*N], byteOffset);
	hkVector4f v0;
	ptr->unpackFirst(v0);
	v[N] = _mm256_insertf128_ps(_mm256_setzero_ps(), v0.m_quad, 0x0);
} };
}
template <int M, hkUint32 byteAddressOffset>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::gatherUnpackFirstHalf8WithOffset(const void* base[M], hkMxVectorf<M>& vOut)
{
	hkMxVectorUtil_Implementation::gatherUnpackFirstWithOffsetH<M, 0, byteAddressOffset>::apply(vOut.m_vec.v, base);
}



namespace hkMxVectorUtil_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct gatherUnpackSecondH {
HK_FORCE_INLINE static void apply(const hkHalf8* base, __m256* vOut)
{
	hkVector4f h0,h1;
	hkAddByteOffsetConst( base, (2*N) * byteAddressIncrement )->unpackSecond(h0); 
	hkAddByteOffsetConst( base, (2*N+1) * byteAddressIncrement )->unpackSecond(h1); 
	gatherUnpackSecondH<I-2,N+1, byteAddressIncrement>::apply(base, vOut);
	vOut[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), h0.m_quad, 0x0);
	vOut[N] = _mm256_insertf128_ps(vOut[N], h1.m_quad, 0x1);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherUnpackSecondH<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkHalf8* base, __m256* vOut)
{
	hkVector4f h0,h1;
	hkAddByteOffsetConst( base, (2*N) * byteAddressIncrement )->unpackSecond(h0); 
	hkAddByteOffsetConst( base, (2*N+1) * byteAddressIncrement )->unpackSecond(h1); 
	vOut[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), h0.m_quad, 0x0);
	vOut[N] = _mm256_insertf128_ps(vOut[N], h1.m_quad, 0x1);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherUnpackSecondH<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkHalf8* base, __m256* vOut)
{
	hkVector4f h;
	hkAddByteOffsetConst( base, (2*N) * byteAddressIncrement )->unpackSecond(h); 
	vOut[N] = _mm256_insertf128_ps(_mm256_setzero_ps(), h.m_quad, 0x0);
} };
}
template <int M, hkUint32 byteAddressIncrement>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::gatherUnpackSecond(const hkHalf8* base, hkMxVectorf<M>& vOut)
{
	hkMxVectorUtil_Implementation::gatherUnpackSecondH<M, 0, byteAddressIncrement>::apply(base, vOut.m_vec.v);
}


namespace hkMxVectorUtil_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct gatherUnpackSecond2H {
HK_FORCE_INLINE static void apply(const hkHalf8* base, const hkInt32* indices, __m256* vOut)
{
	hkVector4f h0,h1;
	hkAddByteOffsetConst( base, indices[2*N] * byteAddressIncrement )->unpackSecond(h0); 
	hkAddByteOffsetConst( base, indices[2*N+1] * byteAddressIncrement )->unpackSecond(h1); 
	gatherUnpackSecond2H<I-2,N+1, byteAddressIncrement>::apply(base, indices, vOut);
	vOut[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), h0.m_quad, 0x0);
	vOut[N] = _mm256_insertf128_ps(vOut[N], h1.m_quad, 0x1);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherUnpackSecond2H<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkHalf8* base, const hkInt32* indices, __m256* vOut)
{
	hkVector4f h0,h1;
	hkAddByteOffsetConst( base, indices[2*N] * byteAddressIncrement )->unpackSecond(h0); 
	hkAddByteOffsetConst( base, indices[2*N+1] * byteAddressIncrement )->unpackSecond(h1); 
	vOut[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), h0.m_quad, 0x0);
	vOut[N] = _mm256_insertf128_ps(vOut[N], h1.m_quad, 0x1);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherUnpackSecond2H<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkHalf8* base, const hkInt32* indices, __m256* vOut)
{
	hkVector4f h;
	hkAddByteOffsetConst( base, indices[2*N] * byteAddressIncrement )->unpackSecond(h); 
	vOut[N] = _mm256_insertf128_ps(_mm256_setzero_ps(), h.m_quad, 0x0);
} };
}
template <int M, hkUint32 byteAddressIncrement>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::gatherUnpackSecond(const hkHalf8* base, const hkInt32* indices, hkMxVectorf<M>& vOut)
{
	hkMxVectorUtil_Implementation::gatherUnpackSecond2H<M, 0, byteAddressIncrement>::apply(base, indices, vOut.m_vec.v);
}


namespace hkMxVectorUtil_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct gatherUnpackSecond2Hu {
HK_FORCE_INLINE static void apply(const hkHalf8* base, const hkUint16* indices, __m256* vOut)
{
	hkVector4f h0,h1;
	hkAddByteOffsetConst( base, indices[2*N] * byteAddressIncrement )->unpackSecond(h0); 
	hkAddByteOffsetConst( base, indices[2*N+1] * byteAddressIncrement )->unpackSecond(h1); 
	gatherUnpackSecond2Hu<I-2,N+1, byteAddressIncrement>::apply(base, indices, vOut);
	vOut[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), h0.m_quad, 0x0);
	vOut[N] = _mm256_insertf128_ps(vOut[N], h1.m_quad, 0x1);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherUnpackSecond2Hu<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkHalf8* base, const hkUint16* indices, __m256* vOut)
{
	hkVector4f h0,h1;
	hkAddByteOffsetConst( base, indices[2*N] * byteAddressIncrement )->unpackSecond(h0); 
	hkAddByteOffsetConst( base, indices[2*N+1] * byteAddressIncrement )->unpackSecond(h1); 
	vOut[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), h0.m_quad, 0x0);
	vOut[N] = _mm256_insertf128_ps(vOut[N], h1.m_quad, 0x1);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherUnpackSecond2Hu<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkHalf8* base, const hkUint16* indices, __m256* vOut)
{
	hkVector4f h;
	hkAddByteOffsetConst( base, indices[2*N] * byteAddressIncrement )->unpackSecond(h); 
	vOut[N] = _mm256_insertf128_ps(_mm256_setzero_ps(), h.m_quad, 0x0);
} };
}
template <int M, hkUint32 byteAddressIncrement>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::gatherUnpackSecond(const hkHalf8* base, const hkUint16* indices, hkMxVectorf<M>& vOut)
{
	hkMxVectorUtil_Implementation::gatherUnpackSecond2Hu<M, 0, byteAddressIncrement>::apply(base, indices, vOut.m_vec.v);
}


namespace hkMxVectorUtil_Implementation
{
template <int I, int N, hkUint32 byteOffset> struct gatherUnpackSecondWithOffsetH {
HK_FORCE_INLINE static void apply(__m256* v, const void** base)
{
	const hkHalf8* HK_RESTRICT ptr0 = (const hkHalf8*)hkAddByteOffsetConst(base[2*N], byteOffset); 
	const hkHalf8* HK_RESTRICT ptr1 = (const hkHalf8*)hkAddByteOffsetConst(base[2*N+1], byteOffset); 
	hkVector4f v0,v1;
	ptr0->unpackSecond(v0);
	ptr1->unpackSecond(v1);
	gatherUnpackSecondWithOffsetH<I-2,N+1, byteOffset>::apply(v, base);
	v[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), v0.m_quad, 0x0);
	v[N] = _mm256_insertf128_ps(v[N], v1.m_quad, 0x1);
} };
template <int N, hkUint32 byteOffset> struct gatherUnpackSecondWithOffsetH<2, N, byteOffset> {
HK_FORCE_INLINE static void apply(__m256* v, const void** base)
{
	const hkHalf8* HK_RESTRICT ptr0 = (const hkHalf8*)hkAddByteOffsetConst(base[2*N], byteOffset); 
	const hkHalf8* HK_RESTRICT ptr1 = (const hkHalf8*)hkAddByteOffsetConst(base[2*N+1], byteOffset); 
	hkVector4f v0,v1;
	ptr0->unpackSecond(v0);
	ptr1->unpackSecond(v1);
	v[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), v0.m_quad, 0x0);
	v[N] = _mm256_insertf128_ps(v[N], v1.m_quad, 0x1);
} };
template <int N, hkUint32 byteOffset> struct gatherUnpackSecondWithOffsetH<1, N, byteOffset> {
HK_FORCE_INLINE static void apply(__m256* v, const void** base)
{
	const hkHalf8* HK_RESTRICT ptr = (const hkHalf8*)hkAddByteOffsetConst(base[2*N], byteOffset); 
	hkVector4f v0;
	ptr->unpackSecond(v0);
	v[N] = _mm256_insertf128_ps(_mm256_setzero_ps(), v0.m_quad, 0x0);
} };
}
template <int M, hkUint32 byteAddressOffset>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::gatherUnpackSecondHalf8WithOffset(const void* base[M], hkMxVectorf<M>& vOut)
{
	hkMxVectorUtil_Implementation::gatherUnpackSecondWithOffsetH<M, 0, byteAddressOffset>::apply(vOut.m_vec.v, base);
}







namespace hkMxVectorUtil_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct scatterPackH {
HK_FORCE_INLINE static void apply(const __m256* v0, const __m256* v1, hkHalf8* HK_RESTRICT base)
{
	hkHalf8* HK_RESTRICT base0 = hkAddByteOffset( base, (2*N) * byteAddressIncrement ); 
	hkHalf8* HK_RESTRICT base1 = hkAddByteOffset( base, (2*N+1) * byteAddressIncrement ); 
	hkVector4f v2,v3,v4,v5;
	v2.m_quad = _mm256_extractf128_ps(v0[N], 0x0);
	v3.m_quad = _mm256_extractf128_ps(v1[N], 0x0);
	v4.m_quad = _mm256_extractf128_ps(v0[N], 0x1);
	v5.m_quad = _mm256_extractf128_ps(v1[N], 0x1);
	scatterPackH<I-2,N+1, byteAddressIncrement>::apply(v0, v1, base);
	base0->pack<false>( v2, v3 );
	base1->pack<false>( v4, v5 );
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterPackH<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v0, const __m256* v1, hkHalf8* HK_RESTRICT base)
{
	hkHalf8* HK_RESTRICT base0 = hkAddByteOffset( base, (2*N) * byteAddressIncrement ); 
	hkHalf8* HK_RESTRICT base1 = hkAddByteOffset( base, (2*N+1) * byteAddressIncrement ); 
	hkVector4f v2,v3,v4,v5;
	v2.m_quad = _mm256_extractf128_ps(v0[N], 0x0);
	v3.m_quad = _mm256_extractf128_ps(v1[N], 0x0);
	v4.m_quad = _mm256_extractf128_ps(v0[N], 0x1);
	v5.m_quad = _mm256_extractf128_ps(v1[N], 0x1);
	base0->pack<false>( v2, v3 );
	base1->pack<false>( v4, v5 );
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterPackH<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v0, const __m256* v1, hkHalf8* HK_RESTRICT base)
{
	hkHalf8* HK_RESTRICT base0 = hkAddByteOffset( base, (2*N) * byteAddressIncrement ); 
	hkVector4f v2,v3;
	v2.m_quad = _mm256_extractf128_ps(v0[N], 0x0);
	v3.m_quad = _mm256_extractf128_ps(v1[N], 0x0);
	base0->pack<false>( v2, v3 );
} };
}
template <int M, hkUint32 byteAddressIncrement>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::scatterPack(hkMxVectorfParameter v0, hkMxVectorfParameter v1, hkHalf8* HK_RESTRICT base)
{
	hkMxVectorUtil_Implementation::scatterPackH<M, 0, byteAddressIncrement>::apply(v0.m_vec.v, v1.m_vec.v, base);
}


namespace hkMxVectorUtil_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct scatterPackHu {
HK_FORCE_INLINE static void apply(const __m256* v0, const __m256* v1, hkHalf8* HK_RESTRICT base, const hkUint16* indices)
{
	hkHalf8* HK_RESTRICT base0 = hkAddByteOffset( base, indices[2*N] * byteAddressIncrement ); 
	hkHalf8* HK_RESTRICT base1 = hkAddByteOffset( base, indices[2*N+1] * byteAddressIncrement ); 
	hkVector4f v2,v3,v4,v5;
	v2.m_quad = _mm256_extractf128_ps(v0[N], 0x0);
	v3.m_quad = _mm256_extractf128_ps(v1[N], 0x0);
	v4.m_quad = _mm256_extractf128_ps(v0[N], 0x1);
	v5.m_quad = _mm256_extractf128_ps(v1[N], 0x1);
	scatterPackHu<I-2,N+1, byteAddressIncrement>::apply(v0, v1, base, indices);
	base0->pack<false>( v2, v3 );
	base1->pack<false>( v4, v5 );
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterPackHu<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v0, const __m256* v1, hkHalf8* HK_RESTRICT base, const hkUint16* indices)
{
	hkHalf8* HK_RESTRICT base0 = hkAddByteOffset( base, indices[2*N] * byteAddressIncrement ); 
	hkHalf8* HK_RESTRICT base1 = hkAddByteOffset( base, indices[2*N+1] * byteAddressIncrement ); 
	hkVector4f v2,v3,v4,v5;
	v2.m_quad = _mm256_extractf128_ps(v0[N], 0x0);
	v3.m_quad = _mm256_extractf128_ps(v1[N], 0x0);
	v4.m_quad = _mm256_extractf128_ps(v0[N], 0x1);
	v5.m_quad = _mm256_extractf128_ps(v1[N], 0x1);
	base0->pack<false>( v2, v3 );
	base1->pack<false>( v4, v5 );
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterPackHu<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v0, const __m256* v1, hkHalf8* HK_RESTRICT base, const hkUint16* indices)
{
	hkHalf8* HK_RESTRICT base0 = hkAddByteOffset( base, indices[2*N] * byteAddressIncrement ); 
	hkVector4f v2,v3;
	v2.m_quad = _mm256_extractf128_ps(v0[N], 0x0);
	v3.m_quad = _mm256_extractf128_ps(v1[N], 0x0);
	base0->pack<false>( v2, v3 );
} };
}
template <int M, hkUint32 byteAddressIncrement> 
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::scatterPack(hkMxVectorfParameter v0, hkMxVectorfParameter v1, hkHalf8* base, const hkUint16* indices)
{
	hkMxVectorUtil_Implementation::scatterPackHu<M, 0, byteAddressIncrement>::apply(v0.m_vec.v, v1.m_vec.v, base, indices);
}

namespace hkMxVectorUtil_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct scatterPackHui {
HK_FORCE_INLINE static void apply(const __m256* v0, const __m256* v1, hkHalf8* HK_RESTRICT base, const hkInt32* indices)
{
	hkHalf8* HK_RESTRICT base0 = hkAddByteOffset( base, indices[2*N] * byteAddressIncrement ); 
	hkHalf8* HK_RESTRICT base1 = hkAddByteOffset( base, indices[2*N+1] * byteAddressIncrement ); 
	hkVector4f v2,v3,v4,v5;
	v2.m_quad = _mm256_extractf128_ps(v0[N], 0x0);
	v3.m_quad = _mm256_extractf128_ps(v1[N], 0x0);
	v4.m_quad = _mm256_extractf128_ps(v0[N], 0x1);
	v5.m_quad = _mm256_extractf128_ps(v1[N], 0x1);
	scatterPackHui<I-2,N+1, byteAddressIncrement>::apply(v0, v1, base, indices);
	base0->pack<false>( v2, v3 );
	base1->pack<false>( v4, v5 );
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterPackHui<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v0, const __m256* v1, hkHalf8* HK_RESTRICT base, const hkInt32* indices)
{
	hkHalf8* HK_RESTRICT base0 = hkAddByteOffset( base, indices[2*N] * byteAddressIncrement ); 
	hkHalf8* HK_RESTRICT base1 = hkAddByteOffset( base, indices[2*N+1] * byteAddressIncrement ); 
	hkVector4f v2,v3,v4,v5;
	v2.m_quad = _mm256_extractf128_ps(v0[N], 0x0);
	v3.m_quad = _mm256_extractf128_ps(v1[N], 0x0);
	v4.m_quad = _mm256_extractf128_ps(v0[N], 0x1);
	v5.m_quad = _mm256_extractf128_ps(v1[N], 0x1);
	base0->pack<false>( v2, v3 );
	base1->pack<false>( v4, v5 );
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterPackHui<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v0, const __m256* v1, hkHalf8* HK_RESTRICT base, const hkInt32* indices)
{
	hkHalf8* HK_RESTRICT base0 = hkAddByteOffset( base, indices[2*N] * byteAddressIncrement ); 
	hkVector4f v2,v3;
	v2.m_quad = _mm256_extractf128_ps(v0[N], 0x0);
	v3.m_quad = _mm256_extractf128_ps(v1[N], 0x0);
	base0->pack<false>( v2, v3 );
} };
}
template <int M, hkUint32 byteAddressIncrement> 
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::scatterPack(hkMxVectorfParameter v0, hkMxVectorfParameter v1, hkHalf8* base, const hkInt32* indices)
{
	hkMxVectorUtil_Implementation::scatterPackHui<M, 0, byteAddressIncrement>::apply(v0.m_vec.v, v1.m_vec.v, base, indices);
}



namespace hkMxVectorUtil_Implementation
{
template <int I, int N, hkUint32 byteOffset> struct scatterPackWithOffsetH {
HK_FORCE_INLINE static void apply(const __m256* v0, const __m256* v1, void** base)
{
	hkHalf8* HK_RESTRICT base0 = (hkHalf8*)hkAddByteOffset( base[2*N], byteOffset ); 
	hkHalf8* HK_RESTRICT base1 = (hkHalf8*)hkAddByteOffset( base[2*N+1], byteOffset ); 
	hkVector4f v2,v3,v4,v5;
	v2.m_quad = _mm256_extractf128_ps(v0[N], 0x0);
	v3.m_quad = _mm256_extractf128_ps(v1[N], 0x0);
	v4.m_quad = _mm256_extractf128_ps(v0[N], 0x1);
	v5.m_quad = _mm256_extractf128_ps(v1[N], 0x1);
	scatterPackWithOffsetH<I-2,N+1, byteOffset>::apply(v0, v1, base);
	base0->pack<false>( v2, v3 );
	base1->pack<false>( v4, v5 );
} };
template <int N, hkUint32 byteOffset> struct scatterPackWithOffsetH<2, N, byteOffset> {
HK_FORCE_INLINE static void apply(const __m256* v0, const __m256* v1, void** base)
{
	hkHalf8* HK_RESTRICT base0 = (hkHalf8*)hkAddByteOffset( base[2*N], byteOffset ); 
	hkHalf8* HK_RESTRICT base1 = (hkHalf8*)hkAddByteOffset( base[2*N+1], byteOffset ); 
	hkVector4f v2,v3,v4,v5;
	v2.m_quad = _mm256_extractf128_ps(v0[N], 0x0);
	v3.m_quad = _mm256_extractf128_ps(v1[N], 0x0);
	v4.m_quad = _mm256_extractf128_ps(v0[N], 0x1);
	v5.m_quad = _mm256_extractf128_ps(v1[N], 0x1);
	base0->pack<false>( v2, v3 );
	base1->pack<false>( v4, v5 );
} };
template <int N, hkUint32 byteOffset> struct scatterPackWithOffsetH<1, N, byteOffset> {
HK_FORCE_INLINE static void apply(const __m256* v0, const __m256* v1, void** base)
{
	hkHalf8* HK_RESTRICT base0 = (hkHalf8*)hkAddByteOffset( base[2*N], byteOffset ); 
	hkVector4f v2,v3;
	v2.m_quad = _mm256_extractf128_ps(v0[N], 0x0);
	v3.m_quad = _mm256_extractf128_ps(v1[N], 0x0);
	base0->pack<false>( v2, v3 );
} };
}
template <int M, hkUint32 byteOffset>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::scatterPackHalf8WithOffset(hkMxVectorfParameter v0, hkMxVectorfParameter v1, void* base[M])
{
	hkMxVectorUtil_Implementation::scatterPackWithOffsetH<M, 0, byteOffset>::apply(v0.m_vec.v, v1.m_vec.v, base);
}




//
//
//
namespace hkMxVectorUtil_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct scatterPackFirstH {
HK_FORCE_INLINE static void apply(const __m256* v, hkHalf8* HK_RESTRICT base)
{
	hkHalf8* HK_RESTRICT base0 = hkAddByteOffset( base, (2*N) * byteAddressIncrement ); 
	hkHalf8* HK_RESTRICT base1 = hkAddByteOffset( base, (2*N+1) * byteAddressIncrement ); 
	hkVector4f v0,v1;
	v0.m_quad = _mm256_extractf128_ps(v[N], 0x0);
	v1.m_quad = _mm256_extractf128_ps(v[N], 0x1);
	scatterPackFirstH<I-2,N+1, byteAddressIncrement>::apply(v, base);
	base0->packFirst<false>( v0 );
	base1->packFirst<false>( v1 );
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterPackFirstH<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v, hkHalf8* HK_RESTRICT base)
{
	hkHalf8* HK_RESTRICT base0 = hkAddByteOffset( base, (2*N) * byteAddressIncrement ); 
	hkHalf8* HK_RESTRICT base1 = hkAddByteOffset( base, (2*N+1) * byteAddressIncrement ); 
	hkVector4f v0,v1;
	v0.m_quad = _mm256_extractf128_ps(v[N], 0x0);
	v1.m_quad = _mm256_extractf128_ps(v[N], 0x1);
	base0->packFirst<false>( v0 );
	base1->packFirst<false>( v1 );
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterPackFirstH<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v, hkHalf8* HK_RESTRICT base)
{
	hkHalf8* HK_RESTRICT base0 = hkAddByteOffset( base, (2*N) * byteAddressIncrement ); 
	hkVector4f v0;
	v0.m_quad = _mm256_extractf128_ps(v[N], 0x0);
	base0->packFirst<false>( v0 );
} };
}
template <int M, hkUint32 byteAddressIncrement>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::scatterPackFirst(hkMxVectorfParameter v, hkHalf8* HK_RESTRICT base)
{
	hkMxVectorUtil_Implementation::scatterPackFirstH<M, 0, byteAddressIncrement>::apply(v.m_vec.v, base);
}



//
//
//
namespace hkMxVectorUtil_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct scatterPackOneH {
HK_FORCE_INLINE static void apply(const __m256* v, hkHalf* HK_RESTRICT base)
{
	hkHalf* HK_RESTRICT base0 = hkAddByteOffset( base, (2*N) * byteAddressIncrement ); 
	hkHalf* HK_RESTRICT base1 = hkAddByteOffset( base, (2*N+1) * byteAddressIncrement ); 
	hkVector4f v0,v1;
	v0.m_quad = _mm256_extractf128_ps(v[N], 0x0);
	v1.m_quad = _mm256_extractf128_ps(v[N], 0x1);
	scatterPackOneH<I-2,N+1, byteAddressIncrement>::apply(v, base);
	hkHalf8 h0; h0.pack<false>( v0, v0 );
	hkHalf8 h1; h1.pack<false>( v1, v1 );
	h0.storePackedFirst( base0 );
	h1.storePackedFirst( base1 );
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterPackOneH<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v, hkHalf* HK_RESTRICT base)
{
	hkHalf* HK_RESTRICT base0 = hkAddByteOffset( base, (2*N) * byteAddressIncrement ); 
	hkHalf* HK_RESTRICT base1 = hkAddByteOffset( base, (2*N+1) * byteAddressIncrement ); 
	hkVector4f v0,v1;
	v0.m_quad = _mm256_extractf128_ps(v[N], 0x0);
	v1.m_quad = _mm256_extractf128_ps(v[N], 0x1);
	hkHalf8 h0; h0.pack<false>( v0, v0 );
	hkHalf8 h1; h1.pack<false>( v1, v1 );
	h0.storePackedFirst( base0 );
	h1.storePackedFirst( base1 );
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterPackOneH<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v, hkHalf* HK_RESTRICT base)
{
	hkHalf* HK_RESTRICT base0 = hkAddByteOffset( base, (2*N) * byteAddressIncrement ); 
	hkVector4f v0;
	v0.m_quad = _mm256_extractf128_ps(v[N], 0x0);
	hkHalf8 h; h.pack<false>( v0, v0 );
	h.storePackedFirst( base0 );
} };
}
template <int M, hkUint32 byteAddressIncrement>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::scatterPack(hkMxVectorfParameter v, hkHalf* HK_RESTRICT base)
{
	hkMxVectorUtil_Implementation::scatterPackOneH<M, 0, byteAddressIncrement>::apply(v.m_vec.v, base);
}




//
//
//

namespace hkMxVectorUtil_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct scatterPackFirstHu {
HK_FORCE_INLINE static void apply(const __m256* v, hkHalf8* HK_RESTRICT base, const hkUint16* indices)
{
	hkHalf8* HK_RESTRICT base0 = hkAddByteOffset( base, indices[2*N] * byteAddressIncrement ); 
	hkHalf8* HK_RESTRICT base1 = hkAddByteOffset( base, indices[2*N+1] * byteAddressIncrement ); 
	hkVector4f v0,v1;
	v0.m_quad = _mm256_extractf128_ps(v[N], 0x0);
	v1.m_quad = _mm256_extractf128_ps(v[N], 0x1);
	scatterPackFirstHu<I-2,N+1, byteAddressIncrement>::apply(v, base, indices);
	base0->packFirst<false>( v0 );
	base1->packFirst<false>( v1 );
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterPackFirstHu<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v, hkHalf8* HK_RESTRICT base, const hkUint16* indices)
{
	hkHalf8* HK_RESTRICT base0 = hkAddByteOffset( base, indices[2*N] * byteAddressIncrement ); 
	hkHalf8* HK_RESTRICT base1 = hkAddByteOffset( base, indices[2*N+1] * byteAddressIncrement ); 
	hkVector4f v0,v1;
	v0.m_quad = _mm256_extractf128_ps(v[N], 0x0);
	v1.m_quad = _mm256_extractf128_ps(v[N], 0x1);
	base0->packFirst<false>( v0 );
	base1->packFirst<false>( v1 );
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterPackFirstHu<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v, hkHalf8* HK_RESTRICT base, const hkUint16* indices)
{
	hkHalf8* HK_RESTRICT base0 = hkAddByteOffset( base, indices[2*N] * byteAddressIncrement ); 
	hkVector4f v0;
	v0.m_quad = _mm256_extractf128_ps(v[N], 0x0);
	base0->packFirst<false>( v0 );
} };
}
template <int M, hkUint32 byteAddressIncrement> 
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::scatterPackFirst(hkMxVectorfParameter v, hkHalf8* base, const hkUint16* indices)
{
	hkMxVectorUtil_Implementation::scatterPackFirstHu<M, 0, byteAddressIncrement>::apply(v.m_vec.v, base, indices);
}



namespace hkMxVectorUtil_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct scatterPackFirstHui {
HK_FORCE_INLINE static void apply(const __m256* v, hkHalf8* HK_RESTRICT base, const hkInt32* indices)
{
	hkHalf8* HK_RESTRICT base0 = hkAddByteOffset( base, indices[2*N] * byteAddressIncrement ); 
	hkHalf8* HK_RESTRICT base1 = hkAddByteOffset( base, indices[2*N+1] * byteAddressIncrement ); 
	hkVector4f v0,v1;
	v0.m_quad = _mm256_extractf128_ps(v[N], 0x0);
	v1.m_quad = _mm256_extractf128_ps(v[N], 0x1);
	scatterPackFirstHui<I-2,N+1, byteAddressIncrement>::apply(v, base, indices);
	base0->packFirst<false>( v0 );
	base1->packFirst<false>( v1 );
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterPackFirstHui<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v, hkHalf8* HK_RESTRICT base, const hkInt32* indices)
{
	hkHalf8* HK_RESTRICT base0 = hkAddByteOffset( base, indices[2*N] * byteAddressIncrement ); 
	hkHalf8* HK_RESTRICT base1 = hkAddByteOffset( base, indices[2*N+1] * byteAddressIncrement ); 
	hkVector4f v0,v1;
	v0.m_quad = _mm256_extractf128_ps(v[N], 0x0);
	v1.m_quad = _mm256_extractf128_ps(v[N], 0x1);
	base0->packFirst<false>( v0 );
	base1->packFirst<false>( v1 );
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterPackFirstHui<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v, hkHalf8* HK_RESTRICT base, const hkInt32* indices)
{
	hkHalf8* HK_RESTRICT base0 = hkAddByteOffset( base, indices[2*N] * byteAddressIncrement ); 
	hkVector4f v0;
	v0.m_quad = _mm256_extractf128_ps(v[N], 0x0);
	base0->packFirst<false>( v0 );
} };
}
template <int M, hkUint32 byteAddressIncrement> 
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::scatterPackFirst(hkMxVectorfParameter v, hkHalf8* base, const hkInt32* indices)
{
	hkMxVectorUtil_Implementation::scatterPackFirstHui<M, 0, byteAddressIncrement>::apply(v.m_vec.v, base, indices);
}



namespace hkMxVectorUtil_Implementation
{
template <int I, int N, hkUint32 byteOffset> struct scatterPackFirstWithOffsetH {
HK_FORCE_INLINE static void apply(const __m256* v, void** base)
{
	hkHalf* HK_RESTRICT base0 = (hkHalf*)hkAddByteOffset( base[2*N], byteOffset );
	hkHalf* HK_RESTRICT base1 = (hkHalf*)hkAddByteOffset( base[2*N+1], byteOffset );
	hkVector4f v0,v1;
	v0.m_quad = _mm256_extractf128_ps(v[N], 0x0);
	v1.m_quad = _mm256_extractf128_ps(v[N], 0x1);
	scatterPackFirstWithOffsetH<I-2,N+1, byteOffset>::apply(v, base);
	hkHalf8 h0; h0.pack<false>( v0, v0 );
	hkHalf8 h1; h1.pack<false>( v1, v1 );
	h0.storePackedFirst( base0 );
	h1.storePackedFirst( base1 );
} };
template <int N, hkUint32 byteOffset> struct scatterPackFirstWithOffsetH<2, N, byteOffset> {
HK_FORCE_INLINE static void apply(const __m256* v, void** base)
{
	hkHalf* HK_RESTRICT base0 = (hkHalf*)hkAddByteOffset( base[2*N], byteOffset );
	hkHalf* HK_RESTRICT base1 = (hkHalf*)hkAddByteOffset( base[2*N+1], byteOffset );
	hkVector4f v0,v1;
	v0.m_quad = _mm256_extractf128_ps(v[N], 0x0);
	v1.m_quad = _mm256_extractf128_ps(v[N], 0x1);
	hkHalf8 h0; h0.pack<false>( v0, v0 );
	hkHalf8 h1; h1.pack<false>( v1, v1 );
	h0.storePackedFirst( base0 );
	h1.storePackedFirst( base1 );
} };
template <int N, hkUint32 byteOffset> struct scatterPackFirstWithOffsetH<1, N, byteOffset> {
HK_FORCE_INLINE static void apply(const __m256* v, void** base)
{
	hkHalf* HK_RESTRICT base0 = (hkHalf*)hkAddByteOffset( base[2*N], byteOffset ); 
	hkVector4f v0;
	v0.m_quad = _mm256_extractf128_ps(v[N], 0x0);
	hkHalf8 h; h.pack<false>( v0, v0 );
	h.storePackedFirst(base0);
} };
}
template <int M, hkUint32 byteOffset>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::scatterPackFirstHalf8WithOffset(hkMxVectorfParameter v, void* base[M])
{
	hkMxVectorUtil_Implementation::scatterPackFirstWithOffsetH<M, 0, byteOffset>::apply(v.m_vec.v, base);
}


namespace hkMxVectorUtil_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct scatterPackSecondH {
HK_FORCE_INLINE static void apply(const __m256* v, hkHalf8* HK_RESTRICT base)
{
	hkHalf8* HK_RESTRICT base0 = hkAddByteOffset( base, (2*N) * byteAddressIncrement ); 
	hkHalf8* HK_RESTRICT base1 = hkAddByteOffset( base, (2*N+1) * byteAddressIncrement ); 
	hkVector4f v0,v1;
	v0.m_quad = _mm256_extractf128_ps(v[N], 0x0);
	v1.m_quad = _mm256_extractf128_ps(v[N], 0x1);
	scatterPackSecondH<I-2,N+1, byteAddressIncrement>::apply(v, base);
	base0->packSecond<false>( v0 );
	base1->packSecond<false>( v1 );
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterPackSecondH<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v, hkHalf8* HK_RESTRICT base)
{
	hkHalf8* HK_RESTRICT base0 = hkAddByteOffset( base, (2*N) * byteAddressIncrement ); 
	hkHalf8* HK_RESTRICT base1 = hkAddByteOffset( base, (2*N+1) * byteAddressIncrement ); 
	hkVector4f v0,v1;
	v0.m_quad = _mm256_extractf128_ps(v[N], 0x0);
	v1.m_quad = _mm256_extractf128_ps(v[N], 0x1);
	base0->packSecond<false>( v0 );
	base1->packSecond<false>( v1 );
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterPackSecondH<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v, hkHalf8* HK_RESTRICT base)
{
	hkHalf8* HK_RESTRICT base0 = hkAddByteOffset( base, (2*N) * byteAddressIncrement ); 
	hkVector4f v0;
	v0.m_quad = _mm256_extractf128_ps(v[N], 0x0);
	base0->packSecond<false>( v0 );
} };
}
template <int M, hkUint32 byteAddressIncrement>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::scatterPackSecond(hkMxVectorfParameter v, hkHalf8* HK_RESTRICT base)
{
	hkMxVectorUtil_Implementation::scatterPackSecondH<M, 0, byteAddressIncrement>::apply(v.m_vec.v, base);
}


namespace hkMxVectorUtil_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct scatterPackSecondHu {
HK_FORCE_INLINE static void apply(const __m256* v, hkHalf8* HK_RESTRICT base, const hkUint16* indices)
{
	hkHalf8* HK_RESTRICT base0 = hkAddByteOffset( base, indices[2*N] * byteAddressIncrement ); 
	hkHalf8* HK_RESTRICT base1 = hkAddByteOffset( base, indices[2*N+1] * byteAddressIncrement ); 
	hkVector4f v0,v1;
	v0.m_quad = _mm256_extractf128_ps(v[N], 0x0);
	v1.m_quad = _mm256_extractf128_ps(v[N], 0x1);
	scatterPackSecondHu<I-2,N+1, byteAddressIncrement>::apply(v, base, indices);
	base0->packSecond<false>( v0 );
	base1->packSecond<false>( v1 );
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterPackSecondHu<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v, hkHalf8* HK_RESTRICT base, const hkUint16* indices)
{
	hkHalf8* HK_RESTRICT base0 = hkAddByteOffset( base, indices[2*N] * byteAddressIncrement ); 
	hkHalf8* HK_RESTRICT base1 = hkAddByteOffset( base, indices[2*N+1] * byteAddressIncrement ); 
	hkVector4f v0,v1;
	v0.m_quad = _mm256_extractf128_ps(v[N], 0x0);
	v1.m_quad = _mm256_extractf128_ps(v[N], 0x1);
	base0->packSecond<false>( v0 );
	base1->packSecond<false>( v1 );
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterPackSecondHu<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v, hkHalf8* HK_RESTRICT base, const hkUint16* indices)
{
	hkHalf8* HK_RESTRICT base0 = hkAddByteOffset( base, indices[2*N] * byteAddressIncrement ); 
	hkVector4f v0;
	v0.m_quad = _mm256_extractf128_ps(v[N], 0x0);
	base0->packSecond<false>( v0 );
} };
}
template <int M, hkUint32 byteAddressIncrement> 
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::scatterPackSecond(hkMxVectorfParameter v, hkHalf8* base, const hkUint16* indices)
{
	hkMxVectorUtil_Implementation::scatterPackSecondHu<M, 0, byteAddressIncrement>::apply(v.m_vec.v, base, indices);
}

namespace hkMxVectorUtil_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct scatterPackSecondHui {
HK_FORCE_INLINE static void apply(const __m256* v, hkHalf8* HK_RESTRICT base, const hkInt32* indices)
{
	hkHalf8* HK_RESTRICT base0 = hkAddByteOffset( base, indices[2*N] * byteAddressIncrement ); 
	hkHalf8* HK_RESTRICT base1 = hkAddByteOffset( base, indices[2*N+1] * byteAddressIncrement ); 
	hkVector4f v0,v1;
	v0.m_quad = _mm256_extractf128_ps(v[N], 0x0);
	v1.m_quad = _mm256_extractf128_ps(v[N], 0x1);
	scatterPackSecondHui<I-2,N+1, byteAddressIncrement>::apply(v, base, indices);
	base0->packSecond<false>( v0 );
	base1->packSecond<false>( v1 );
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterPackSecondHui<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v, hkHalf8* HK_RESTRICT base, const hkInt32* indices)
{
	hkHalf8* HK_RESTRICT base0 = hkAddByteOffset( base, indices[2*N] * byteAddressIncrement ); 
	hkHalf8* HK_RESTRICT base1 = hkAddByteOffset( base, indices[2*N+1] * byteAddressIncrement ); 
	hkVector4f v0,v1;
	v0.m_quad = _mm256_extractf128_ps(v[N], 0x0);
	v1.m_quad = _mm256_extractf128_ps(v[N], 0x1);
	base0->packSecond<false>( v0 );
	base1->packSecond<false>( v1 );
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterPackSecondHui<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v, hkHalf8* HK_RESTRICT base, const hkInt32* indices)
{
	hkHalf8* HK_RESTRICT base0 = hkAddByteOffset( base, indices[2*N] * byteAddressIncrement ); 
	hkVector4f v0;
	v0.m_quad = _mm256_extractf128_ps(v[N], 0x0);
	base0->packSecond<false>( v0 );
} };
}
template <int M, hkUint32 byteAddressIncrement> 
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::scatterPackSecond(hkMxVectorfParameter v, hkHalf8* base, const hkInt32* indices)
{
	hkMxVectorUtil_Implementation::scatterPackSecondHui<M, 0, byteAddressIncrement>::apply(v.m_vec.v, base, indices);
}



namespace hkMxVectorUtil_Implementation
{
template <int I, int N, hkUint32 byteOffset> struct scatterPackSecondWithOffsetH {
HK_FORCE_INLINE static void apply(const __m256* v, void** base)
{
	hkHalf8* HK_RESTRICT base0 = (hkHalf8*)hkAddByteOffset( base[2*N], byteOffset ); 
	hkHalf8* HK_RESTRICT base1 = (hkHalf8*)hkAddByteOffset( base[2*N+1], byteOffset ); 
	hkVector4f v0,v1;
	v0.m_quad = _mm256_extractf128_ps(v[N], 0x0);
	v1.m_quad = _mm256_extractf128_ps(v[N], 0x1);
	scatterPackSecondWithOffsetH<I-2,N+1, byteOffset>::apply(v, base);
	base0->packSecond<false>( v0 );
	base1->packSecond<false>( v1 );
} };
template <int N, hkUint32 byteOffset> struct scatterPackSecondWithOffsetH<2, N, byteOffset> {
HK_FORCE_INLINE static void apply(const __m256* v, void** base)
{
	hkHalf8* HK_RESTRICT base0 = (hkHalf8*)hkAddByteOffset( base[2*N], byteOffset ); 
	hkHalf8* HK_RESTRICT base1 = (hkHalf8*)hkAddByteOffset( base[2*N+1], byteOffset ); 
	hkVector4f v0,v1;
	v0.m_quad = _mm256_extractf128_ps(v[N], 0x0);
	v1.m_quad = _mm256_extractf128_ps(v[N], 0x1);
	base0->packSecond<false>( v0 );
	base1->packSecond<false>( v1 );
} };
template <int N, hkUint32 byteOffset> struct scatterPackSecondWithOffsetH<1, N, byteOffset> {
HK_FORCE_INLINE static void apply(const __m256* v, void** base)
{
	hkHalf8* HK_RESTRICT base0 = (hkHalf8*)hkAddByteOffset( base[2*N], byteOffset ); 
	hkVector4f v0;
	v0.m_quad = _mm256_extractf128_ps(v[N], 0x0);
	base0->packSecond<false>( v0 );
} };
}
template <int M, hkUint32 byteOffset>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::scatterPackSecondHalf8WithOffset(hkMxVectorfParameter v, void* base[M])
{
	hkMxVectorUtil_Implementation::scatterPackSecondWithOffsetH<M, 0, byteOffset>::apply(v.m_vec.v, base);
}










namespace hkMxVectorUtil_Implementation
{
template <int I, int N, hkUint32 byteOffset> struct gatherUnpackPackedVectorWithOffsetH {
HK_FORCE_INLINE static void apply(__m256* v, const void** base)
{
	const hkPackedVector3* HK_RESTRICT ptr0 = (const hkPackedVector3*)hkAddByteOffsetConst(base[2*N], byteOffset); 
	const hkPackedVector3* HK_RESTRICT ptr1 = (const hkPackedVector3*)hkAddByteOffsetConst(base[2*N+1], byteOffset); 
	hkVector4f v0,v1;
	ptr0->unpack(v0);
	ptr1->unpack(v1);
	gatherUnpackPackedVectorWithOffsetH<I-2,N+1, byteOffset>::apply(v, base);
	v[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), v0.m_quad, 0x0);
	v[N] = _mm256_insertf128_ps(v[N], v1.m_quad, 0x1);
} };
template <int N, hkUint32 byteOffset> struct gatherUnpackPackedVectorWithOffsetH<2, N, byteOffset> {
HK_FORCE_INLINE static void apply(__m256* v, const void** base)
{
	const hkPackedVector3* HK_RESTRICT ptr0 = (const hkPackedVector3*)hkAddByteOffsetConst(base[2*N], byteOffset); 
	const hkPackedVector3* HK_RESTRICT ptr1 = (const hkPackedVector3*)hkAddByteOffsetConst(base[2*N+1], byteOffset); 
	hkVector4f v0,v1;
	ptr0->unpack(v0);
	ptr1->unpack(v1);
	v[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), v0.m_quad, 0x0);
	v[N] = _mm256_insertf128_ps(v[N], v1.m_quad, 0x1);
} };
template <int N, hkUint32 byteOffset> struct gatherUnpackPackedVectorWithOffsetH<1, N, byteOffset> {
HK_FORCE_INLINE static void apply(__m256* v, const void** base)
{
	const hkPackedVector3* HK_RESTRICT ptr = (const hkPackedVector3*)hkAddByteOffsetConst(base[2*N], byteOffset); 
	hkVector4f v0;
	ptr->unpack(v0);
	v[N] = _mm256_insertf128_ps(_mm256_setzero_ps(), v0.m_quad, 0x0);
} };
}
template <int M, hkUint32 byteAddressOffset>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::gatherUnpackPackedVectorWithOffset(const void* base[M], hkMxVectorf<M>& vOut)
{
	hkMxVectorUtil_Implementation::gatherUnpackPackedVectorWithOffsetH<M, 0, byteAddressOffset>::apply(vOut.m_vec.v, base);
}


namespace hkMxVectorUtil_Implementation
{
template <int I, int N, hkUint32 byteOffset> struct gatherUnpackPackedUnitVectorWithOffsetH {
HK_FORCE_INLINE static void apply(__m256* v, const void** base)
{
	const hkPackedUnitVector<4>* HK_RESTRICT ptr0 = (const hkPackedUnitVector<4>*)hkAddByteOffsetConst(base[2*N], byteOffset); 
	const hkPackedUnitVector<4>* HK_RESTRICT ptr1 = (const hkPackedUnitVector<4>*)hkAddByteOffsetConst(base[2*N+1], byteOffset); 
	hkVector4f v0,v1;
	ptr0->unpack(&v0);
	ptr1->unpack(&v1);
	gatherUnpackPackedUnitVectorWithOffsetH<I-2,N+1, byteOffset>::apply(v, base);
	v[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), v0.m_quad, 0x0);
	v[N] = _mm256_insertf128_ps(v[N], v1.m_quad, 0x1);
} };
template <int N, hkUint32 byteOffset> struct gatherUnpackPackedUnitVectorWithOffsetH<2, N, byteOffset> {
HK_FORCE_INLINE static void apply(__m256* v, const void** base)
{
	const hkPackedUnitVector<4>* HK_RESTRICT ptr0 = (const hkPackedUnitVector<4>*)hkAddByteOffsetConst(base[2*N], byteOffset); 
	const hkPackedUnitVector<4>* HK_RESTRICT ptr1 = (const hkPackedUnitVector<4>*)hkAddByteOffsetConst(base[2*N+1], byteOffset); 
	hkVector4f v0,v1;
	ptr0->unpack(&v0);
	ptr1->unpack(&v1);
	v[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), v0.m_quad, 0x0);
	v[N] = _mm256_insertf128_ps(v[N], v1.m_quad, 0x1);
} };
template <int N, hkUint32 byteOffset> struct gatherUnpackPackedUnitVectorWithOffsetH<1, N, byteOffset> {
HK_FORCE_INLINE static void apply(__m256* v, const void** base)
{
	const hkPackedUnitVector<4>* HK_RESTRICT ptr = (const hkPackedUnitVector<4>*)hkAddByteOffsetConst(base[2*N], byteOffset); 
	hkVector4f v0;
	ptr->unpack(&v0);
	v[N] = _mm256_insertf128_ps(_mm256_setzero_ps(), v0.m_quad, 0x0);
} };
}
template <int M, hkUint32 byteAddressOffset>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::gatherUnpackPackedUnitVectorWithOffset(const void* base[M], hkMxVectorf<M>& vOut)
{
	hkMxVectorUtil_Implementation::gatherUnpackPackedUnitVectorWithOffsetH<M, 0, byteAddressOffset>::apply(vOut.m_vec.v, base);
}























namespace hkMxVectorUtil_Implementation
{
HK_FORCE_INLINE static void transformPositionH(hkMxTransformParameter mat, const hkMxVectorf<4>& vIn, hkMxVectorf<4>& vOut)
{
	// the vectors in mat are the columns of a transform matrix
	__m256 m0,m1,m2,m3;
	{
		m0 = _mm256_permute2f128_ps(mat.m_vec.v[0],_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
		m1 = _mm256_permute2f128_ps(mat.m_vec.v[0],_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_HIGH,_MM256_A_HIGH));
		m2 = _mm256_permute2f128_ps(mat.m_vec.v[1],_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
		m3 = _mm256_permute2f128_ps(mat.m_vec.v[1],_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_HIGH,_MM256_A_HIGH));
	}

	__m256 xb,yb,zb,t;
	{
		xb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(0,0,0,0));
		yb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(1,1,1,1));
		zb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(2,2,2,2));

		t = _mm256_add_ps(m3, _mm256_mul_ps(xb, m0));
		t = _mm256_add_ps( t, _mm256_mul_ps(yb, m1));
		t = _mm256_add_ps( t, _mm256_mul_ps(zb, m2));

		vOut.m_vec.v[0] = t;
	}
	{
		xb = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(0,0,0,0));
		yb = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(1,1,1,1));
		zb = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(2,2,2,2));

		t = _mm256_add_ps(m3, _mm256_mul_ps(xb, m0));
		t = _mm256_add_ps( t, _mm256_mul_ps(yb, m1));
		t = _mm256_add_ps( t, _mm256_mul_ps(zb, m2));

		vOut.m_vec.v[1] = t;
	}
}

HK_FORCE_INLINE static void transformPositionH(hkMxTransformParameter mat, const hkMxVectorf<3>& vIn, hkMxVectorf<3>& vOut)
{
	// the vectors in mat are the columns of a transform matrix
	__m256 m0,m1,m2,m3;
	{
		m0 = _mm256_permute2f128_ps(mat.m_vec.v[0],_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
		m1 = _mm256_permute2f128_ps(mat.m_vec.v[0],_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_HIGH,_MM256_A_HIGH));
		m2 = _mm256_permute2f128_ps(mat.m_vec.v[1],_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
		m3 = _mm256_permute2f128_ps(mat.m_vec.v[1],_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_HIGH,_MM256_A_HIGH));
	}

	__m256 xb,yb,zb,t;
	{
		xb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(0,0,0,0));
		yb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(1,1,1,1));
		zb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(2,2,2,2));

		t = _mm256_add_ps(m3, _mm256_mul_ps(xb, m0));
		t = _mm256_add_ps( t, _mm256_mul_ps(yb, m1));
		t = _mm256_add_ps( t, _mm256_mul_ps(zb, m2));

		vOut.m_vec.v[0] = t;
	}
	{
		xb = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(0,0,0,0));
		yb = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(1,1,1,1));
		zb = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(2,2,2,2));

		t = _mm256_add_ps(m3, _mm256_mul_ps(xb, m0));
		t = _mm256_add_ps( t, _mm256_mul_ps(yb, m1));
		t = _mm256_add_ps( t, _mm256_mul_ps(zb, m2));

		vOut.m_vec.v[1] = t;
	}
}

HK_FORCE_INLINE static void transformPositionH(hkMxTransformParameter mat, const hkMxVectorf<2>& vIn, hkMxVectorf<2>& vOut)
{
	// the vectors in mat are the columns of a transform matrix
	__m256 m0,m1,m2,m3;
	{
		m0 = _mm256_permute2f128_ps(mat.m_vec.v[0],_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
		m1 = _mm256_permute2f128_ps(mat.m_vec.v[0],_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_HIGH,_MM256_A_HIGH));
		m2 = _mm256_permute2f128_ps(mat.m_vec.v[1],_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
		m3 = _mm256_permute2f128_ps(mat.m_vec.v[1],_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_HIGH,_MM256_A_HIGH));
	}

	__m256 xb,yb,zb,t;
	{
		xb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(0,0,0,0));
		yb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(1,1,1,1));
		zb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(2,2,2,2));

		t = _mm256_add_ps(m3, _mm256_mul_ps(xb, m0));
		t = _mm256_add_ps( t, _mm256_mul_ps(yb, m1));
		t = _mm256_add_ps( t, _mm256_mul_ps(zb, m2));

		vOut.m_vec.v[0] = t;
	}
}

HK_FORCE_INLINE static void transformPositionH(hkMxTransformParameter mat, const hkMxVectorf<1>& vIn, hkMxVectorf<1>& vOut)
{
	// the vectors in mat are the columns of a transform matrix
	__m256 m0,m1,m2,m3;
	{
		m0 = _mm256_permute2f128_ps(mat.m_vec.v[0],_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
		m1 = _mm256_permute2f128_ps(mat.m_vec.v[0],_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_HIGH,_MM256_A_HIGH));
		m2 = _mm256_permute2f128_ps(mat.m_vec.v[1],_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
		m3 = _mm256_permute2f128_ps(mat.m_vec.v[1],_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_HIGH,_MM256_A_HIGH));
	}

	__m256 xb,yb,zb,t;
	{
		xb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(0,0,0,0));
		yb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(1,1,1,1));
		zb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(2,2,2,2));

		t = _mm256_add_ps(m3, _mm256_mul_ps(xb, m0));
		t = _mm256_add_ps( t, _mm256_mul_ps(yb, m1));
		t = _mm256_add_ps( t, _mm256_mul_ps(zb, m2));

		vOut.m_vec.v[0] = t;
	}
}
}
template <int M>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::transformPosition(hkMxTransformParameter mat, hkMxVectorfParameter vIn, hkMxVectorf<M>& vOut)
{
	hkMxVectorUtil_Implementation::transformPositionH(mat, vIn, vOut);
}



namespace hkMxVectorUtil_Implementation
{

HK_FORCE_INLINE static void transformTransposePositionH(hkMxTransformParameter mat, const hkMxVectorf<4>& vIn, hkMxVectorf<4>& vOut)
{
	// the vectors in mat are the columns of a transform matrix
	__m256 m0,m1,m2,m3;
	{
		__m256 col02 = _mm256_permute2f128_ps(mat.m_vec.v[0],mat.m_vec.v[1],_MM256_PERMUTE2(_MM256_B_LOW,_MM256_A_LOW));
		__m256 col13 = _mm256_permute2f128_ps(mat.m_vec.v[0],mat.m_vec.v[1],_MM256_PERMUTE2(_MM256_B_HIGH,_MM256_A_HIGH));
		
		__m256 t01 = _mm256_shuffle_ps(col02,col13,0x44);
		__m256 t23 = _mm256_shuffle_ps(col02,col13,0xEE);

		__m256 row01 = _mm256_shuffle_ps(t01,t01,0x8D);
		__m256 row23 = _mm256_shuffle_ps(t23,t23,0x8D);

		m0 = _mm256_permute2f128_ps(row01,_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
		m1 = _mm256_permute2f128_ps(row01,_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_HIGH,_MM256_A_HIGH));
		m2 = _mm256_permute2f128_ps(row23,_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
		m3 = _mm256_permute2f128_ps(row23,_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_HIGH,_MM256_A_HIGH));
	}

	__m256 xb,yb,zb,t;
	{
		xb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(0,0,0,0));
		yb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(1,1,1,1));
		zb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(2,2,2,2));

		t = _mm256_add_ps(m3, _mm256_mul_ps(xb, m0));
		t = _mm256_add_ps( t, _mm256_mul_ps(yb, m1));
		t = _mm256_add_ps( t, _mm256_mul_ps(zb, m2));

		vOut.m_vec.v[0] = t;
	}
	{
		xb = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(0,0,0,0));
		yb = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(1,1,1,1));
		zb = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(2,2,2,2));

		t = _mm256_add_ps(m3, _mm256_mul_ps(xb, m0));
		t = _mm256_add_ps( t, _mm256_mul_ps(yb, m1));
		t = _mm256_add_ps( t, _mm256_mul_ps(zb, m2));

		vOut.m_vec.v[1] = t;
	}
}

HK_FORCE_INLINE static void transformTransposePositionH(hkMxTransformParameter mat, const hkMxVectorf<3>& vIn, hkMxVectorf<3>& vOut)
{
	// the vectors in mat are the columns of a transform matrix
	__m256 m0,m1,m2,m3;
	{
		__m256 col02 = _mm256_permute2f128_ps(mat.m_vec.v[0],mat.m_vec.v[1],_MM256_PERMUTE2(_MM256_B_LOW,_MM256_A_LOW));
		__m256 col13 = _mm256_permute2f128_ps(mat.m_vec.v[0],mat.m_vec.v[1],_MM256_PERMUTE2(_MM256_B_HIGH,_MM256_A_HIGH));

		__m256 t01 = _mm256_shuffle_ps(col02,col13,0x44);
		__m256 t23 = _mm256_shuffle_ps(col02,col13,0xEE);

		__m256 row01 = _mm256_shuffle_ps(t01,t01,0x8D);
		__m256 row23 = _mm256_shuffle_ps(t23,t23,0x8D);

		m0 = _mm256_permute2f128_ps(row01,_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
		m1 = _mm256_permute2f128_ps(row01,_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_HIGH,_MM256_A_HIGH));
		m2 = _mm256_permute2f128_ps(row23,_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
		m3 = _mm256_permute2f128_ps(row23,_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_HIGH,_MM256_A_HIGH));
	}

	__m256 xb,yb,zb,t;
	{
		xb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(0,0,0,0));
		yb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(1,1,1,1));
		zb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(2,2,2,2));

		t = _mm256_add_ps(m3, _mm256_mul_ps(xb, m0));
		t = _mm256_add_ps( t, _mm256_mul_ps(yb, m1));
		t = _mm256_add_ps( t, _mm256_mul_ps(zb, m2));

		vOut.m_vec.v[0] = t;
	}
	{
		xb = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(0,0,0,0));
		yb = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(1,1,1,1));
		zb = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(2,2,2,2));

		t = _mm256_add_ps(m3, _mm256_mul_ps(xb, m0));
		t = _mm256_add_ps( t, _mm256_mul_ps(yb, m1));
		t = _mm256_add_ps( t, _mm256_mul_ps(zb, m2));

		vOut.m_vec.v[1] = t;
	}
}

HK_FORCE_INLINE static void transformTransposePositionH(hkMxTransformParameter mat, const hkMxVectorf<2>& vIn, hkMxVectorf<2>& vOut)
{
	// the vectors in mat are the columns of a transform matrix
	__m256 m0,m1,m2,m3;
	{
		__m256 col02 = _mm256_permute2f128_ps(mat.m_vec.v[0],mat.m_vec.v[1],_MM256_PERMUTE2(_MM256_B_LOW,_MM256_A_LOW));
		__m256 col13 = _mm256_permute2f128_ps(mat.m_vec.v[0],mat.m_vec.v[1],_MM256_PERMUTE2(_MM256_B_HIGH,_MM256_A_HIGH));

		__m256 t01 = _mm256_shuffle_ps(col02,col13,0x44);
		__m256 t23 = _mm256_shuffle_ps(col02,col13,0xEE);

		__m256 row01 = _mm256_shuffle_ps(t01,t01,0x8D);
		__m256 row23 = _mm256_shuffle_ps(t23,t23,0x8D);

		m0 = _mm256_permute2f128_ps(row01,_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
		m1 = _mm256_permute2f128_ps(row01,_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_HIGH,_MM256_A_HIGH));
		m2 = _mm256_permute2f128_ps(row23,_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
		m3 = _mm256_permute2f128_ps(row23,_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_HIGH,_MM256_A_HIGH));
	}

	__m256 xb,yb,zb,t;
	{
		xb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(0,0,0,0));
		yb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(1,1,1,1));
		zb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(2,2,2,2));

		t = _mm256_add_ps(m3, _mm256_mul_ps(xb, m0));
		t = _mm256_add_ps( t, _mm256_mul_ps(yb, m1));
		t = _mm256_add_ps( t, _mm256_mul_ps(zb, m2));

		vOut.m_vec.v[0] = t;
	}
}

HK_FORCE_INLINE static void transformTransposePositionH(hkMxTransformParameter mat, const hkMxVectorf<1>& vIn, hkMxVectorf<1>& vOut)
{
	__m256 m0,m1,m2,m3;
	{
		__m256 col02 = _mm256_permute2f128_ps(mat.m_vec.v[0],mat.m_vec.v[1],_MM256_PERMUTE2(_MM256_B_LOW,_MM256_A_LOW));
		__m256 col13 = _mm256_permute2f128_ps(mat.m_vec.v[0],mat.m_vec.v[1],_MM256_PERMUTE2(_MM256_B_HIGH,_MM256_A_HIGH));

		__m256 t01 = _mm256_shuffle_ps(col02,col13,0x44);
		__m256 t23 = _mm256_shuffle_ps(col02,col13,0xEE);

		__m256 row01 = _mm256_shuffle_ps(t01,t01,0x8D);
		__m256 row23 = _mm256_shuffle_ps(t23,t23,0x8D);

		m0 = _mm256_permute2f128_ps(row01,_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
		m1 = _mm256_permute2f128_ps(row01,_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_HIGH,_MM256_A_HIGH));
		m2 = _mm256_permute2f128_ps(row23,_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
		m3 = _mm256_permute2f128_ps(row23,_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_HIGH,_MM256_A_HIGH));
	}

	__m256 xb,yb,zb,t;
	{
		xb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(0,0,0,0));
		yb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(1,1,1,1));
		zb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(2,2,2,2));

		t = _mm256_add_ps(m3, _mm256_mul_ps(xb, m0));
		t = _mm256_add_ps( t, _mm256_mul_ps(yb, m1));
		t = _mm256_add_ps( t, _mm256_mul_ps(zb, m2));

		vOut.m_vec.v[0] = t;
	}
}
}

template <int M>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::transformTransposePosition(hkMxTransformParameter mat, hkMxVectorfParameter vIn, hkMxVectorf<M>& vOut)
{
	hkMxVectorUtil_Implementation::transformTransposePositionH(mat, vIn, vOut);
}





namespace hkMxVectorUtil_Implementation
{


	HK_FORCE_INLINE static void rotateDirectionH( hkMxTransformParameter mat, const hkMxVectorf<4>& vIn, hkMxVectorf<4>& vOut)
	{
		// the vectors in mat are the columns of a transform matrix
		__m256 m0,m1,m2;
		{
			m0 = _mm256_permute2f128_ps(mat.m_vec.v[0],_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
			m1 = _mm256_permute2f128_ps(mat.m_vec.v[0],_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_HIGH,_MM256_A_HIGH));
			m2 = _mm256_permute2f128_ps(mat.m_vec.v[1],_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
		}

		__m256 xb,yb,zb,t;
		{
			xb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(0,0,0,0));
			yb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(1,1,1,1));
			zb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(2,2,2,2));

			t = _mm256_mul_ps(xb, m0);
			t = _mm256_add_ps( t, _mm256_mul_ps(yb, m1));
			t = _mm256_add_ps( t, _mm256_mul_ps(zb, m2));

			vOut.m_vec.v[0] = t;
		}
		{
			xb = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(0,0,0,0));
			yb = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(1,1,1,1));
			zb = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(2,2,2,2));

			t = _mm256_mul_ps(xb, m0);
			t = _mm256_add_ps( t, _mm256_mul_ps(yb, m1));
			t = _mm256_add_ps( t, _mm256_mul_ps(zb, m2));

			vOut.m_vec.v[1] = t;
		}
	}

	HK_FORCE_INLINE static void rotateDirectionH( hkMxTransformParameter mat, const hkMxVectorf<3>& vIn, hkMxVectorf<3>& vOut)
	{
		// the vectors in mat are the columns of a transform matrix
		__m256 m0,m1,m2;
		{
			m0 = _mm256_permute2f128_ps(mat.m_vec.v[0],_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
			m1 = _mm256_permute2f128_ps(mat.m_vec.v[0],_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_HIGH));
			m2 = _mm256_permute2f128_ps(mat.m_vec.v[1],_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
		}

		__m256 xb,yb,zb,t;
		{
			xb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(0,0,0,0));
			yb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(1,1,1,1));
			zb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(2,2,2,2));

			t = _mm256_mul_ps(xb, m0);
			t = _mm256_add_ps( t, _mm256_mul_ps(yb, m1));
			t = _mm256_add_ps( t, _mm256_mul_ps(zb, m2));

			vOut.m_vec.v[0] = t;
		}
		{
			xb = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(0,0,0,0));
			yb = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(1,1,1,1));
			zb = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(2,2,2,2));

			t = _mm256_mul_ps(xb, m0);
			t = _mm256_add_ps( t, _mm256_mul_ps(yb, m1));
			t = _mm256_add_ps( t, _mm256_mul_ps(zb, m2));

			vOut.m_vec.v[1] = t;
		}
	}

	HK_FORCE_INLINE static void rotateDirectionH( hkMxTransformParameter mat, const hkMxVectorf<2>& vIn, hkMxVectorf<2>& vOut)
	{
		// the vectors in mat are the columns of a transform matrix
		__m256 m0,m1,m2;
		{
			m0 = _mm256_permute2f128_ps(mat.m_vec.v[0],_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
			m1 = _mm256_permute2f128_ps(mat.m_vec.v[0],_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_HIGH,_MM256_A_HIGH));
			m2 = _mm256_permute2f128_ps(mat.m_vec.v[1],_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
		}

		__m256 xb,yb,zb,t;
		{
			xb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(0,0,0,0));
			yb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(1,1,1,1));
			zb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(2,2,2,2));

			t = _mm256_mul_ps(xb, m0);
			t = _mm256_add_ps( t, _mm256_mul_ps(yb, m1));
			t = _mm256_add_ps( t, _mm256_mul_ps(zb, m2));

			vOut.m_vec.v[0] = t;
		}
	}

	HK_FORCE_INLINE static void rotateDirectionH( hkMxTransformParameter mat, const hkMxVectorf<1>& vIn, hkMxVectorf<1>& vOut)
	{
		__m256 m0,m1,m2;
		{
			m0 = _mm256_permute2f128_ps(mat.m_vec.v[0],_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
			m1 = _mm256_permute2f128_ps(mat.m_vec.v[0],_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_HIGH));
			m2 = _mm256_permute2f128_ps(mat.m_vec.v[1],_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
		}

		__m256 xb,yb,zb,t;
		{
			xb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(0,0,0,0));
			yb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(1,1,1,1));
			zb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(2,2,2,2));

			t = _mm256_mul_ps(xb, m0);
			t = _mm256_add_ps( t, _mm256_mul_ps(yb, m1));
			t = _mm256_add_ps( t, _mm256_mul_ps(zb, m2));

			vOut.m_vec.v[0] = t;
		}
	}
}

template <int M>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::rotateDirection( hkMxTransformParameter mat, hkMxVectorfParameter vIn, hkMxVectorf<M>& vOut)
{
	hkMxVectorUtil_Implementation::rotateDirectionH(mat, vIn, vOut);
}


namespace hkMxVectorUtil_Implementation
{
HK_FORCE_INLINE static void rotateInverseDirectionH( hkMxTransformParameter mat, const hkMxVectorf<4>& vIn, hkMxVectorf<4>& vOut)
{
	// the vectors in mat are the columns of a transform matrix
	__m256 m0,m1,m2;
	{
		__m256 col02 = _mm256_permute2f128_ps(mat.m_vec.v[0],mat.m_vec.v[1],_MM256_PERMUTE2(_MM256_B_LOW,_MM256_A_LOW));
		__m256 col13 = _mm256_permute2f128_ps(mat.m_vec.v[0],mat.m_vec.v[1],_MM256_PERMUTE2(_MM256_B_HIGH,_MM256_A_HIGH));

		__m256 t01 = _mm256_shuffle_ps(col02,col13,0x44);
		__m256 t23 = _mm256_shuffle_ps(col02,col13,0xEE);

		__m256 row01 = _mm256_shuffle_ps(t01,t01,0x8D);
		__m256 row23 = _mm256_shuffle_ps(t23,t23,0x8D);

		m0 = _mm256_permute2f128_ps(row01,_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
		m1 = _mm256_permute2f128_ps(row01,_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_HIGH,_MM256_A_HIGH));
		m2 = _mm256_permute2f128_ps(row23,_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
	}

	__m256 xb,yb,zb,t;
	{
		xb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(0,0,0,0));
		yb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(1,1,1,1));
		zb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(2,2,2,2));

		t = _mm256_mul_ps(xb, m0);
		t = _mm256_add_ps( t, _mm256_mul_ps(yb, m1));
		t = _mm256_add_ps( t, _mm256_mul_ps(zb, m2));

		vOut.m_vec.v[0] = t;
	}
	{
		xb = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(0,0,0,0));
		yb = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(1,1,1,1));
		zb = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(2,2,2,2));

		t = _mm256_mul_ps(xb, m0);
		t = _mm256_add_ps( t, _mm256_mul_ps(yb, m1));
		t = _mm256_add_ps( t, _mm256_mul_ps(zb, m2));

		vOut.m_vec.v[1] = t;
	}
}


HK_FORCE_INLINE static void rotateInverseDirectionH( hkMxTransformParameter mat, const hkMxVectorf<3>& vIn, hkMxVectorf<3>& vOut)
{
	// the vectors in mat are the columns of a transform matrix
	__m256 m0,m1,m2;
	{
		__m256 col02 = _mm256_permute2f128_ps(mat.m_vec.v[0],mat.m_vec.v[1],_MM256_PERMUTE2(_MM256_B_LOW,_MM256_A_LOW));
		__m256 col13 = _mm256_permute2f128_ps(mat.m_vec.v[0],mat.m_vec.v[1],_MM256_PERMUTE2(_MM256_B_HIGH,_MM256_A_HIGH));

		__m256 t01 = _mm256_shuffle_ps(col02,col13,0x44);
		__m256 t23 = _mm256_shuffle_ps(col02,col13,0xEE);

		__m256 row01 = _mm256_shuffle_ps(t01,t01,0x8D);
		__m256 row23 = _mm256_shuffle_ps(t23,t23,0x8D);

		m0 = _mm256_permute2f128_ps(row01,_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
		m1 = _mm256_permute2f128_ps(row01,_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_HIGH,_MM256_A_HIGH));
		m2 = _mm256_permute2f128_ps(row23,_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
	}

	__m256 xb,yb,zb,t;
	{
		xb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(0,0,0,0));
		yb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(1,1,1,1));
		zb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(2,2,2,2));

		t = _mm256_mul_ps(xb, m0);
		t = _mm256_add_ps( t, _mm256_mul_ps(yb, m1));
		t = _mm256_add_ps( t, _mm256_mul_ps(zb, m2));

		vOut.m_vec.v[0] = t;
	}
	{
		xb = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(0,0,0,0));
		yb = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(1,1,1,1));
		zb = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(2,2,2,2));

		t = _mm256_mul_ps(xb, m0);
		t = _mm256_add_ps( t, _mm256_mul_ps(yb, m1));
		t = _mm256_add_ps( t, _mm256_mul_ps(zb, m2));

		vOut.m_vec.v[1] = t;
	}
}

HK_FORCE_INLINE static void rotateInverseDirectionH( hkMxTransformParameter mat, const hkMxVectorf<2>& vIn, hkMxVectorf<2>& vOut)
{
	// the vectors in mat are the columns of a transform matrix
	__m256 m0,m1,m2;
	{
		__m256 col02 = _mm256_permute2f128_ps(mat.m_vec.v[0],mat.m_vec.v[1],_MM256_PERMUTE2(_MM256_B_LOW,_MM256_A_LOW));
		__m256 col13 = _mm256_permute2f128_ps(mat.m_vec.v[0],mat.m_vec.v[1],_MM256_PERMUTE2(_MM256_B_HIGH,_MM256_A_HIGH));

		__m256 t01 = _mm256_shuffle_ps(col02,col13,0x44);
		__m256 t23 = _mm256_shuffle_ps(col02,col13,0xEE);

		__m256 row01 = _mm256_shuffle_ps(t01,t01,0x8D);
		__m256 row23 = _mm256_shuffle_ps(t23,t23,0x8D);

		m0 = _mm256_permute2f128_ps(row01,_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
		m1 = _mm256_permute2f128_ps(row01,_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_HIGH,_MM256_A_HIGH));
		m2 = _mm256_permute2f128_ps(row23,_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
	}

	__m256 xb,yb,zb,t;
	{
		xb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(0,0,0,0));
		yb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(1,1,1,1));
		zb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(2,2,2,2));

		t = _mm256_mul_ps(xb, m0);
		t = _mm256_add_ps( t, _mm256_mul_ps(yb, m1));
		t = _mm256_add_ps( t, _mm256_mul_ps(zb, m2));

		vOut.m_vec.v[0] = t;
	}
}
HK_FORCE_INLINE static void rotateInverseDirectionH( hkMxTransformParameter mat, const hkMxVectorf<1>& vIn, hkMxVectorf<1>& vOut)
{
	__m256 m0,m1,m2;
	{
		__m256 col02 = _mm256_permute2f128_ps(mat.m_vec.v[0],mat.m_vec.v[1],_MM256_PERMUTE2(_MM256_B_LOW,_MM256_A_LOW));
		__m256 col13 = _mm256_permute2f128_ps(mat.m_vec.v[0],mat.m_vec.v[1],_MM256_PERMUTE2(_MM256_B_HIGH,_MM256_A_HIGH));

		__m256 t01 = _mm256_shuffle_ps(col02,col13,0x44);
		__m256 t23 = _mm256_shuffle_ps(col02,col13,0xEE);

		__m256 row01 = _mm256_shuffle_ps(t01,t01,0x8D);
		__m256 row23 = _mm256_shuffle_ps(t23,t23,0x8D);

		m0 = _mm256_permute2f128_ps(row01,_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
		m1 = _mm256_permute2f128_ps(row01,_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_HIGH,_MM256_A_HIGH));
		m2 = _mm256_permute2f128_ps(row23,_mm256_undefined_ps(),_MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_LOW));
	}

	__m256 xb,yb,zb,t;
	{
		xb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(0,0,0,0));
		yb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(1,1,1,1));
		zb = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(2,2,2,2));

		t = _mm256_mul_ps(xb, m0);
		t = _mm256_add_ps( t, _mm256_mul_ps(yb, m1));
		t = _mm256_add_ps( t, _mm256_mul_ps(zb, m2));

		vOut.m_vec.v[0] = t;
	}
}
}

template <int M>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::rotateInverseDirection( hkMxTransformParameter mat, hkMxVectorfParameter vIn, hkMxVectorf<M>& vOut)
{
	hkMxVectorUtil_Implementation::rotateInverseDirectionH(mat, vIn, vOut);
}




namespace hkMxVectorUtil_Implementation
{
HK_FORCE_INLINE static __m256 vec8_setRotatedDir(const __m256& quat, const __m256& direction, const __m256& minusHalf)
{
	__m256 qreal = _mm256_permute_ps(quat, _MM256_PERMUTE(3,3,3,3));
	__m256 q2minus1 = _mm256_add_ps(minusHalf, _mm256_mul_ps(qreal, qreal) );

	__m256 ret = _mm256_mul_ps( direction, q2minus1 );

	__m256 imagDotDir = hkMxCommon_Implementation::dotProdH<3>(quat, direction);
	ret = _mm256_add_ps(ret, _mm256_mul_ps( quat, imagDotDir )); 

	__m256 imagCrossDir = hkMxCommon_Implementation::crossH( quat, direction );
	ret = _mm256_add_ps(ret, _mm256_mul_ps( imagCrossDir, qreal ));

	return _mm256_add_ps( ret, ret );
}


template <int I>
HK_FORCE_INLINE void rotateDirH(const __m256* mat, const __m256* vIn, __m256* vOut, const __m256& minusHalf)
{
	rotateDirH<I-1>(mat, vIn, vOut, minusHalf);
	vOut[I-1] = vec8_setRotatedDir( mat[I-1], vIn[I-1], minusHalf );
}
template <>
HK_FORCE_INLINE void rotateDirH<1>(const __m256* mat, const __m256* vIn, __m256* vOut, const __m256& minusHalf)
{
	vOut[0] = vec8_setRotatedDir( mat[0], vIn[0], minusHalf );
}
}
template <int M>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::rotateDirection( hkMxQuaternionfParameter mat, hkMxVectorfParameter vIn, hkMxVectorf<M>& vOut)
{
	const __m256 minusHalf = _mm256_set1_ps(-0.5f);
	hkMxVectorUtil_Implementation::rotateDirH<((M+1)>>1)>(mat.m_vec.v, vIn.m_vec.v, vOut.m_vec.v, minusHalf);
}


namespace hkMxVectorUtil_Implementation
{
HK_FORCE_INLINE static __m256 vec8_setRotatedDir(const hkTransformf* t, const __m256& b )
{
	const hkMatrix3f& m0 = t->getRotation();
	const hkMatrix3f& m1 = (t+1)->getRotation();

	__m256 c0 = _mm256_insertf128_ps(_mm256_undefined_ps(), m0.getColumn<0>().m_quad, 0x0);
	__m256 c1 = _mm256_insertf128_ps(_mm256_undefined_ps(), m0.getColumn<1>().m_quad, 0x0);
	__m256 c2 = _mm256_insertf128_ps(_mm256_undefined_ps(), m0.getColumn<2>().m_quad, 0x0);

	       c0 = _mm256_insertf128_ps(c0, m1.getColumn<0>().m_quad, 0x1);
		   c1 = _mm256_insertf128_ps(c1, m1.getColumn<1>().m_quad, 0x1);
		   c2 = _mm256_insertf128_ps(c2, m1.getColumn<2>().m_quad, 0x1);

	__m256 b0 = _mm256_permute_ps( b, _MM256_PERMUTE(0,0,0,0));
	__m256 b1 = _mm256_permute_ps( b, _MM256_PERMUTE(1,1,1,1));
	__m256 b2 = _mm256_permute_ps( b, _MM256_PERMUTE(2,2,2,2));

	__m256 r0 = _mm256_mul_ps( c0, b0 );
	__m256 r1 = _mm256_mul_ps( c1, b1 );
	__m256 r2 = _mm256_mul_ps( c2, b2 );

	return _mm256_add_ps( _mm256_add_ps(r0, r1), r2 );
}

template <int I, int N> struct rotateDirTH { HK_FORCE_INLINE static void apply(const hkTransformf* mat, const __m256* vIn, __m256* vOut)
{
	vOut[N] = vec8_setRotatedDir( mat+(2*N), vIn[N] );
	rotateDirTH<I-2,N+1>::apply(mat, vIn, vOut);
} };
template <int N> struct rotateDirTH<2,N> { HK_FORCE_INLINE static void apply(const hkTransformf* mat, const __m256* vIn, __m256* vOut)
{
	vOut[N] = vec8_setRotatedDir( mat+(2*N), vIn[N] );
} };
template <int N> struct rotateDirTH<1,N> { HK_FORCE_INLINE static void apply(const hkTransformf* mat, const __m256* vIn, __m256* vOut)
{
	const hkMatrix3f& m0 = (mat+(2*N))->getRotation();

	__m256 c0 = _mm256_insertf128_ps(_mm256_setzero_ps(), m0.getColumn<0>().m_quad, 0x0);
	__m256 c1 = _mm256_insertf128_ps(_mm256_setzero_ps(), m0.getColumn<1>().m_quad, 0x0);
	__m256 c2 = _mm256_insertf128_ps(_mm256_setzero_ps(), m0.getColumn<2>().m_quad, 0x0);

	__m256 b0 = _mm256_permute_ps( vIn[N], _MM256_PERMUTE(0,0,0,0));
	__m256 b1 = _mm256_permute_ps( vIn[N], _MM256_PERMUTE(1,1,1,1));
	__m256 b2 = _mm256_permute_ps( vIn[N], _MM256_PERMUTE(2,2,2,2));

	__m256 r0 = _mm256_mul_ps( c0, b0 );
	__m256 r1 = _mm256_mul_ps( c1, b1 );
	__m256 r2 = _mm256_mul_ps( c2, b2 );

	vOut[N] = _mm256_add_ps( _mm256_add_ps(r0, r1), r2 );
} };
}
template <int M>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::rotateDirection( const hkTransformf* mat, hkMxVectorfParameter vIn, hkMxVectorf<M>& vOut)
{
	hkMxVectorUtil_Implementation::rotateDirTH<M,0>::apply(mat, vIn.m_vec.v, vOut.m_vec.v);
}


namespace hkMxVectorUtil_Implementation
{
template <int I, int N> struct transformPosH { HK_FORCE_INLINE static void apply(const hkTransformf* mat, const hkVector4f* vIn, hkVector4f* vOut)
{
	__m256 rot = vec8_setRotatedDir( mat+(2*N), vIn[N] );

	const hkVector4f& t0 = (mat+(2*N))->getTranslation();
	const hkVector4f& t1 = (mat+(2*N)+1)->getTranslation();

	__m256 t = _mm256_insertf128_ps(_mm256_undefined_ps(), t0.m_quad, 0x0);
	       t = _mm256_insertf128_ps(t, t1.m_quad, 0x1);

	vOut[N] = _mm256_add_ps(t, rot);

	transformPosH<I-2,N+1>::apply(mat, vIn, vOut);
} };
template <int N> struct transformPosH<2,N> { HK_FORCE_INLINE static void apply(const hkTransformf* mat, const __m256* vIn, __m256* vOut)
{
	__m256 rot = vec8_setRotatedDir( mat+(2*N), vIn[N] );

	const hkVector4f& t0 = (mat+(2*N))->getTranslation();
	const hkVector4f& t1 = (mat+(2*N)+1)->getTranslation();

	__m256 t = _mm256_insertf128_ps(_mm256_undefined_ps(), t0.m_quad, 0x0);
	       t = _mm256_insertf128_ps(t, t1.m_quad, 0x1);

	vOut[N] = _mm256_add_ps(t, rot);
} };
template <int N> struct transformPosH<1,N> { HK_FORCE_INLINE static void apply(const hkTransformf* mat, const __m256* vIn, __m256* vOut)
{
	const hkMatrix3f& m0 = (mat+(2*N))->getRotation();
	const hkVector4f& t0 = (mat+(2*N))->getTranslation();

	__m256 c0 = _mm256_insertf128_ps(_mm256_setzero_ps(), m0.getColumn<0>().m_quad, 0x0);
	__m256 c1 = _mm256_insertf128_ps(_mm256_setzero_ps(), m0.getColumn<1>().m_quad, 0x0);
	__m256 c2 = _mm256_insertf128_ps(_mm256_setzero_ps(), m0.getColumn<2>().m_quad, 0x0);

	__m256 b0 = _mm256_permute_ps( vIn[N], _MM256_PERMUTE(0,0,0,0));
	__m256 b1 = _mm256_permute_ps( vIn[N], _MM256_PERMUTE(1,1,1,1));
	__m256 b2 = _mm256_permute_ps( vIn[N], _MM256_PERMUTE(2,2,2,2));

	__m256 r0 = _mm256_mul_ps( c0, b0 );
	__m256 r1 = _mm256_mul_ps( c1, b1 );
	__m256 r2 = _mm256_mul_ps( c2, b2 );

	__m256 t = _mm256_insertf128_ps(_mm256_setzero_ps(), t0.m_quad, 0x0);

	vOut[N] = _mm256_add_ps( t, _mm256_add_ps( _mm256_add_ps(r0, r1), r2 ) );
} };
}
template <int M>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::transformPosition( const hkTransformf* mat, hkMxVectorfParameter vIn, hkMxVectorf<M>& vOut)
{
	hkMxVectorUtil_Implementation::transformPosH<M,0>::apply(mat, vIn.m_vec.v, vOut.m_vec.v);
}


HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::transform4Positions(hkMxTransformParameter mat0, hkMxTransformParameter mat1, hkMxTransformParameter mat2, hkMxTransformParameter mat3, const hkMxVectorf<4>& vIn, hkMxVectorf<4>& vOut)
{
	//	creates better assembly
	//	mat0,mat1 x in0,in1
	__m256 m001,m101,m201,m301;
	__m256 xb01,yb01,zb01;
	__m256 out01;

	//	mat2,mat3 x in2,in3
	__m256 m023,m123,m223,m323;
	__m256 xb23,yb23,zb23;
	__m256 out23;
	{
		// get matrix
		m001 = _mm256_permute2f128_ps(mat0.m_vec.v[0],mat1.m_vec.v[0],_MM256_PERMUTE2(_MM256_B_LOW,_MM256_A_LOW));
		m101 = _mm256_permute2f128_ps(mat0.m_vec.v[0],mat1.m_vec.v[0],_MM256_PERMUTE2(_MM256_B_HIGH,_MM256_A_HIGH));
		m201 = _mm256_permute2f128_ps(mat0.m_vec.v[1],mat1.m_vec.v[1],_MM256_PERMUTE2(_MM256_B_LOW,_MM256_A_LOW));
		m301 = _mm256_permute2f128_ps(mat0.m_vec.v[1],mat1.m_vec.v[1],_MM256_PERMUTE2(_MM256_B_HIGH,_MM256_A_HIGH));

		m023 = _mm256_permute2f128_ps(mat2.m_vec.v[0],mat3.m_vec.v[0],_MM256_PERMUTE2(_MM256_B_LOW,_MM256_A_LOW));
		m123 = _mm256_permute2f128_ps(mat2.m_vec.v[0],mat3.m_vec.v[0],_MM256_PERMUTE2(_MM256_B_HIGH,_MM256_A_HIGH));
		m223 = _mm256_permute2f128_ps(mat2.m_vec.v[1],mat3.m_vec.v[1],_MM256_PERMUTE2(_MM256_B_LOW,_MM256_A_LOW));
		m323 = _mm256_permute2f128_ps(mat2.m_vec.v[1],mat3.m_vec.v[1],_MM256_PERMUTE2(_MM256_B_HIGH,_MM256_A_HIGH));

		//	set vector
		xb01 = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(0,0,0,0));
		yb01 = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(1,1,1,1));
		zb01 = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(2,2,2,2));

		xb23 = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(0,0,0,0));
		yb23 = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(1,1,1,1));
		zb23 = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(2,2,2,2));


		//	execute
		out01 = _mm256_add_ps( m301, _mm256_mul_ps( xb01, m001 ) );
		out01 = _mm256_add_ps(out01, _mm256_mul_ps( yb01, m101 ) );
		out01 = _mm256_add_ps(out01, _mm256_mul_ps( zb01, m201 ) );

		out23 = _mm256_add_ps( m323, _mm256_mul_ps( xb23, m023 ) );
		out23 = _mm256_add_ps(out23, _mm256_mul_ps( yb23, m123 ) );
		out23 = _mm256_add_ps(out23, _mm256_mul_ps( zb23, m223 ) );

		vOut.m_vec.v[0] = out01;
		vOut.m_vec.v[1] = out23;
	}
}

HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorfUtil::rotate4Directions(hkMxTransformParameter mat0, hkMxTransformParameter mat1, hkMxTransformParameter mat2, hkMxTransformParameter mat3, const hkMxVectorf<4>& vIn, hkMxVectorf<4>& vOut)
{
	//	creates better assembly
	//	mat0,mat1 x in0,in1
	__m256 m001,m101,m201;
	__m256 xb01,yb01,zb01;
	__m256 out01;

	//	mat2,mat3 x in2,in3
	__m256 m023,m123,m223;
	__m256 xb23,yb23,zb23;
	__m256 out23;
	{
		// get matrix
		m001 = _mm256_permute2f128_ps(mat0.m_vec.v[0],mat1.m_vec.v[0],_MM256_PERMUTE2(_MM256_B_LOW,_MM256_A_LOW));
		m101 = _mm256_permute2f128_ps(mat0.m_vec.v[0],mat1.m_vec.v[0],_MM256_PERMUTE2(_MM256_B_HIGH,_MM256_A_HIGH));
		m201 = _mm256_permute2f128_ps(mat0.m_vec.v[1],mat1.m_vec.v[1],_MM256_PERMUTE2(_MM256_B_LOW,_MM256_A_LOW));

		m023 = _mm256_permute2f128_ps(mat2.m_vec.v[0],mat3.m_vec.v[0],_MM256_PERMUTE2(_MM256_B_LOW,_MM256_A_LOW));
		m123 = _mm256_permute2f128_ps(mat2.m_vec.v[0],mat3.m_vec.v[0],_MM256_PERMUTE2(_MM256_B_HIGH,_MM256_A_HIGH));
		m223 = _mm256_permute2f128_ps(mat2.m_vec.v[1],mat3.m_vec.v[1],_MM256_PERMUTE2(_MM256_B_LOW,_MM256_A_LOW));

		//	set vector
		xb01 = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(0,0,0,0));
		yb01 = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(1,1,1,1));
		zb01 = _mm256_permute_ps(vIn.m_vec.v[0],_MM256_PERMUTE(2,2,2,2));

		xb23 = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(0,0,0,0));
		yb23 = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(1,1,1,1));
		zb23 = _mm256_permute_ps(vIn.m_vec.v[1],_MM256_PERMUTE(2,2,2,2));

		//	execute
		out01 = _mm256_mul_ps( xb01, m001 );
		out01 = _mm256_add_ps(out01, _mm256_mul_ps( yb01, m101 ));
		out01 = _mm256_add_ps(out01, _mm256_mul_ps( zb01, m201 ));

		out23 = _mm256_mul_ps( xb23, m023 );
		out23 = _mm256_add_ps(out23, _mm256_mul_ps( yb23, m123 ));
		out23 = _mm256_add_ps(out23, _mm256_mul_ps( zb23, m223 ));

		vOut.m_vec.v[0] = out01;
		vOut.m_vec.v[1] = out23;
	}
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
