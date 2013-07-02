/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#if defined(HK_PLATFORM_IOS) || defined(HK_PLATFORM_ANDROID)
	#define HK_STD_NAMESPACE /* Nothing */
#else
	#define HK_STD_NAMESPACE std
#endif

namespace hkMath
{
	HK_FORCE_INLINE static hkFloat32 HK_CALL invSqrtF32Approx12Bit( const hkFloat32 x )
	{
		const float xhalf = 0.5f * x;
		int i = *(int*)&x;
		// All positive denormals are less than 0x800000; subtracting that causes underflow
		// into the sign bit. The extra subtracted 0x80000000 then flips that sign bit (1
		// for normal, 0 for denormal). Arithmetic bitshift copies the sign bit into all
		// bits. The resultant mask is 0 for denormals and +0, and ~0 for normals and -0.
		int mask = (i - (signed int)0x80800000) >> 31;

		i = 0x5f375a86 - (i>>1); // Lomonts initial guess
		i &= mask;
		float r = *(float*)&i;

		// Newton steps to improve
		r = (1.5f * r) - (xhalf * r) * (r * r);
		r = (1.5f * r) - (xhalf * r) * (r * r);
		return r;
	}

	HK_FORCE_INLINE static hkFloat32 HK_CALL invSqrtF32Approx23Bit( const hkFloat32 x )
	{
		const float xhalf = 0.5f * x;
		int i = *(int*)&x;
		// See comment in invSqrtF32Approx12Bit.
		int mask = (i - (signed int)0x80800000) >> 31;

		i = 0x5f375a86 - (i>>1); // Lomonts initial guess
		i &= mask;
		float r = *(float*)&i;

		// Newton steps to improve
		r = (1.5f * r) - (xhalf * r) * (r * r);
		r = (1.5f * r) - (xhalf * r) * (r * r);
		r = (1.5f * r) - (xhalf * r) * (r * r);
		return r;
	}

	HK_FORCE_INLINE static hkFloat32 HK_CALL rcpF32Approx12Bit( const hkFloat32 x )
	{
		int i = (2 * 0x3F800000) - *(int *)&x; // float 1.0 bits
		// See comment in invSqrtF32Approx12Bit. The XOR corrects for negative x.
		int mask = ((*(int *)&x - (signed int)0x80800000) ^ *(int *)&x) >> 31;
		i &= mask;
		float r = *(float *)&i;

		// Newton steps to improve
		r = r * (2.0f - x * r);
		r = r * (2.0f - x * r);
		return r;
	}

	HK_FORCE_INLINE static hkFloat32 HK_CALL rcpF32Approx23Bit( const hkFloat32 x )
	{
		int i = (2 * 0x3F800000) - *(int *)&x; // float 1.0 bits
		// See comment in invSqrtF32Approx12Bit. The XOR corrects for negative x.
		int mask = ((*(int *)&x - (signed int)0x80800000) ^ *(int *)&x) >> 31;
		i &= mask;
		float r = *(float *)&i;

		// Newton steps to improve
		r = r * (2.0f - x * r);
		r = r * (2.0f - x * r);
		r = r * (2.0f - x * r);
		return r;
	}

	//
	// need to implement
	//
#	define HK_MATH_quadReciprocal_f
	HK_FORCE_INLINE static hkQuadFloat32 HK_CALL quadReciprocal( const hkQuadFloat32& r )
	{
		hkQuadFloat32 r0 = vrecpeq_f32( r ); // estimate
		hkQuadFloat32 r1 = vmulq_f32( r0, vrecpsq_f32( r, r0 )); // pass1
		return vmulq_f32( r1, vrecpsq_f32(  r, r1 )); // pass2
	}

#	define HK_MATH_quadReciprocalSquareRoot_f
	HK_FORCE_INLINE static hkQuadFloat32 HK_CALL quadReciprocalSquareRoot( const hkQuadFloat32& r )
	{
		hkQuadFloat32 r0 = vrsqrteq_f32( r ); // estimate
		hkQuadFloat32 e0 = vmulq_f32(r0, r0);
		hkQuadFloat32 r1 = vmulq_f32( vrsqrtsq_f32(  r, e0 ), r0 ); // pass1
		hkQuadFloat32 e1 = vmulq_f32(r1, r1);		
		return vmulq_f32( vrsqrtsq_f32(  r, e1 ), r1 ); // pass2
	}

#	define HK_MATH_quadReciprocalTwoIter_f
	HK_FORCE_INLINE static hkQuadFloat32 HK_CALL quadReciprocalTwoIter( const hkQuadFloat32& r )
	{
		return quadReciprocal(r);
	}

#	define HK_MATH_quadReciprocalSquareRootTwoIter_f
	HK_FORCE_INLINE static hkQuadFloat32 HK_CALL quadReciprocalSquareRootTwoIter( const hkQuadFloat32& r )
	{
		return quadReciprocalSquareRoot(r);
	}

	// 
	// Neon optimized implementations
	//

#define HK_MATH_signBitSet
	HK_FORCE_INLINE static bool HK_CALL signBitSet(const hkFloat32& r)
	{
		return (*(reinterpret_cast<const unsigned int*>(&r)) & 0x80000000) ? true : false;
	}
	HK_FORCE_INLINE static bool HK_CALL signBitSet(const hkDouble64& r)
	{
		return (*(reinterpret_cast<const unsigned long long*>(&r)) & 0x8000000000000000ULL) ? true : false;
	}

#define HK_MATH_isFinite
	HK_FORCE_INLINE static bool HK_CALL isFinite(const hkFloat32 r)
	{
		// Check the 8 exponent bits.
		// Usually NAN == (exponent = all 1, mantissa = non-zero)
		//         INF == (exponent = all 1, mantissa = zero)
		// This simply checks the exponent
		HK_ASSERT(0x2d910c70, sizeof(hkFloat32) == sizeof(unsigned int));
		union {
			hkFloat32 f;
			unsigned int i;
		} val;

		val.f = r;
		return ((val.i & 0x7f800000) != 0x7f800000);
	}
	HK_FORCE_INLINE static bool HK_CALL isFinite(const hkDouble64 r)
	{
		// Check the 11 exponent bits.
		// Usually NAN == (exponent = all 1, mantissa = non-zero)
		//         INF == (exponent = all 1, mantissa = zero)
		// This simply checks the exponent
		HK_ASSERT(0x2d910c70, sizeof(hkDouble64) == sizeof(unsigned long long));
		union {
			hkDouble64 f;
			unsigned long long i;
		} val;
		val.f = r;
		return ((val.i & 0x7ff0000000000000ULL) != 0x7ff0000000000000ULL);
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
