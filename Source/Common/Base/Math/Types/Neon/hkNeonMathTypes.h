/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_MATH_NEON_MATH_TYPES_H
#define HK_MATH_NEON_MATH_TYPES_H

#ifndef HK_COMPILER_HAS_INTRINSICS_NEON
#error Cant include this header on non Neon setups..
#endif

// Share double types from Fpu.
#include <Common/Base/Math/Types/Fpu/hkFpuDoubleMathTypes.h>

#ifndef HK_COMPILER_SNC
#include <arm_neon.h>
#endif

// transpose 4 hkVector4
#define HK_TRANSPOSE4f(A,B,C,D) { \
	float32x2x2_t ABxy = vzip_f32( vget_low_f32(A.m_quad), vget_low_f32(B.m_quad) );  /* (A.x, B.x) (A.y, B.y) */ \
	float32x2x2_t ABzw = vzip_f32( vget_high_f32(A.m_quad), vget_high_f32(B.m_quad) ); /* (A.z, B.z) (A.w, B.w) */ \
	float32x2x2_t CDxy = vzip_f32( vget_low_f32(C.m_quad), vget_low_f32(D.m_quad) );  /* (C.x, D.x) (C.y, D.y) */ \
	float32x2x2_t CDzw = vzip_f32( vget_high_f32(C.m_quad), vget_high_f32(D.m_quad) ); /* (A.z, B.z) (C.w, D.w) */ \
	A.m_quad = vcombine_f32(ABxy.val[0], CDxy.val[0]); \
	B.m_quad = vcombine_f32(ABxy.val[1], CDxy.val[1]); \
	C.m_quad = vcombine_f32(ABzw.val[0], CDzw.val[0]); \
	D.m_quad = vcombine_f32(ABzw.val[1], CDzw.val[1]); \
}

// transpose 3 hkVector4: w component is undefined
#define HK_TRANSPOSE3f(A,B,C) { \
	float32x2x2_t ABxy = vzip_f32( vget_low_f32( A.m_quad ), vget_low_f32( B.m_quad ) );  /* (A.x, B.x) (A.y, B.y) */ \
	float32x2x2_t ABzw = vzip_f32( vget_high_f32( A.m_quad ), vget_high_f32( B.m_quad ) ); /* (A.z, B.z) (A.w, B.w) */ \
	float32x2_t Cx = vget_low_f32( C.m_quad );\
	float32x2_t Cz = vget_high_f32( C.m_quad );\
	float32x2_t Cy = vset_lane_f32( vgetq_lane_f32( C.m_quad, 1), Cz, 0);\
	A.m_quad = vcombine_f32(ABxy.val[0], Cx ); \
	B.m_quad = vcombine_f32(ABxy.val[1], Cy ); \
	C.m_quad = vcombine_f32(ABzw.val[0], Cz ); \
}

// storage type for hkVector4 (and thus hkQuaternion)
typedef float32x4_t hkQuadFloat32;

// storage type for hkSimdReal
typedef float32x2_t hkSingleFloat32;

// storage type for hkVector4Comparison
typedef uint32x4_t hkVector4fMask; // the arm intrinsics for compare use uint32x4 types

// storage type for hkIntVector
typedef uint32x4_t hkQuadUint;

typedef int32x4_t hkSingleInt128;

struct hkQuadUlong
{
	//+hk.MemoryTracker(ignore = True)
	int64x2_t xy;
	int64x2_t zw;
};

// argument types
class hkVector4f;
typedef const hkVector4f& hkVector4fParameter;

class hkSimdFloat32;
typedef const hkSimdFloat32& hkSimdFloat32Parameter;

class hkVector4fComparison;
typedef const hkVector4fComparison& hkVector4fComparisonParameter;

class hkQuaternionf;
typedef const hkQuaternionf& hkQuaternionfParameter;

class hkIntVector;
typedef const hkIntVector& hkIntVectorParameter;

// this causes problems for the optimizer, use for debug checks only
#if defined(HK_PLATFORM_PSVITA) || defined(HK_PLATFORM_ANDROID)
#define HK_QUADFLOAT_CONSTANT(a, b, c, d)	{a, b, c, d}
#define HK_QUADINT_CONSTANT(a, b, c, d)		{a, b, c, d}

#elif defined(HK_COMPILER_MSVC)
// MSVC uses __n128 as union with varing size arrays, so compiler does not seem to like   __n128 VTEST = { 0, 1, 2, 3 }; etc
//TODO: Find compiler syntax that will work, or get it fixed etc
struct __HK_NEON_FOUR_FLOATS
{
	__HK_NEON_FOUR_FLOATS( float x, float y, float z, float w )
	{
		const float32_t temp[4] = { x, y, z, w };
		float32x2_t l = vld1_f32( temp );
		float32x2_t h = vld1_f32( temp + 2 );
		v = vcombine_f32(l,h);
	}
	union
	{
		float f[4];
		__n128 v;
	};	
};

struct __HK_NEON_FOUR_INTS
{
	__HK_NEON_FOUR_INTS( unsigned __int32 x, unsigned __int32 y, unsigned __int32 z, unsigned __int32 w )
	{
		__n64 V0 = vcreate_u32(((uint64_t)x) | ((uint64_t)y << 32));
		__n64 V1 = vcreate_u32(((uint64_t)z) | ((uint64_t)w << 32));
		v = vcombine_u32(V0, V1);
	}
	
	union 
	{
		unsigned __int32 i[4];
		__n128 v;
	};
};

struct __HK_NEON_TWO_INTS
{
	__HK_NEON_TWO_INTS( unsigned __int32 x, unsigned __int32 y)
	{
		v = vcreate_u32(((uint64_t)x) | ((uint64_t)y << 32));
	}
	
	union 
	{
		unsigned __int32 i[2];
		__n64 v;
	};
};

struct __HK_NEON_TWO_FLOATS
{
	__HK_NEON_TWO_FLOATS( float x, float y  )
	{
		const float32_t temp[2] = { x, y };
		v = vld1_f32( temp );
	}
	union
	{
		float f[2];
		__n64 v;
	};	
};

#	define HK_NEON_CONSTANT2I(a, b) __HK_NEON_TWO_INTS( (unsigned __int32)a,(unsigned __int32)b ).v
#	define HK_NEON_CONSTANT4I(a, b, c, d) __HK_NEON_FOUR_INTS( (unsigned __int32)a, (unsigned __int32)b, (unsigned __int32)c, (unsigned __int32)d ).v
#	define HK_NEON_CONSTANT2F(a, b) __HK_NEON_TWO_FLOATS(a, b).v
#	define HK_NEON_CONSTANT4F(a, b, c, d) __HK_NEON_FOUR_FLOATS( a, b, c, d ).v
#	define HK_QUADINT_CONSTANT(a, b, c, d) { (unsigned __int32)a,(unsigned __int32)b,(unsigned __int32)c,(unsigned __int32)d } // used as int[4] in code
#	define HK_QUADFLOAT_CONSTANT(a,b,c,d) __HK_NEON_FOUR_FLOATS(a,b,c,d ).v

#else
//#define HK_QUADINT_CONSTANT(a, b, c, d) *(hkQuadUint*)&( (uint32x4_t){a,b,c,d} )
#define HK_QUADINT_CONSTANT(a, b, c, d) {a,b,c,d}
#   if defined(HK_PLATFORM_IOS) && defined(__llvm__)
#       define HK_NEON_CONSTANT2(a, b) {a,b}
#       define HK_NEON_CONSTANT4(a, b, c, d) {a,b,c,d}
#       define HK_QUADFLOAT_CONSTANT(a,b,c,d) (float32x4_t){a,b,c,d}
#   else
#   define HK_QUADFLOAT_CONSTANT(a,b,c,d) *(hkQuadFloat32*)&( (float32x4_t){a,b,c,d} )
#   endif
#endif

#ifndef HK_NEON_CONSTANT4
#define HK_NEON_CONSTANT2(a, b) {a,b}
#define HK_NEON_CONSTANT4(a, b, c, d) {a,b,c,d}
#endif
#ifndef HK_NEON_CONSTANT4F
#define HK_NEON_CONSTANT2F(a, b) HK_NEON_CONSTANT2(a,b)
#define HK_NEON_CONSTANT4F(a, b, c, d) HK_NEON_CONSTANT4(a,b,c,d)
#define HK_NEON_CONSTANT2I(a, b) HK_NEON_CONSTANT2(a,b)
#define HK_NEON_CONSTANT4I(a, b, c, d) HK_NEON_CONSTANT4(a,b,c,d)
#endif



#if defined(HK_PLATFORM_IOS) && defined(__llvm__)
// LLVM, no implict uint32 -> float
#define HK_MASK_TO_VECTORf(m) (hkQuadFloat32)vreinterpretq_f32_u32(m)

#endif

#endif // HK_MATH_NEON_MATH_TYPES_H

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
