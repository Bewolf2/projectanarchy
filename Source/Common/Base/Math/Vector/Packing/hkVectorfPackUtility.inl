/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Common/Base/Math/Vector/Packing/hkVectorPackUtilityCommon.h>

namespace hkVectorPackUtility
{

template< typename T, int NUM_BITS_X, int NUM_BITS_Y >
HK_FORCE_INLINE void decode3( const T& vertex, hkVector4f* HK_RESTRICT vectorOut )
{
	enum
	{
		TOTAL_BITS	=	sizeof(T) << 3,
		NUM_BITS_Z	=	TOTAL_BITS - (NUM_BITS_X + NUM_BITS_Y),
		MASK_X		=	(1<<NUM_BITS_X)-1,
		MASK_Y		=	(1<<NUM_BITS_Y)-1,
		MASK_Z		=	(1<<NUM_BITS_Z)-1
	};

	#if defined(HK_PLATFORM_XBOX360) | defined(HK_PLATFORM_PS3)
		if(sizeof(T) == 2)
		{
			static const HK_ALIGN16( hkUint32 mask[4] ) = { MASK_X, MASK_Y, MASK_Z, 0 };
			#if HK_ENDIAN_BIG	// !intel  
			static const HK_ALIGN16( hkUint32 shifts[4] ) = { 0, NUM_BITS_X, NUM_BITS_X + NUM_BITS_Y, 0 };
			#else	// intel
			static const HK_ALIGN16( hkUint32 shifts[4] ) = { 16, 16+NUM_BITS_X, 16 + NUM_BITS_X + NUM_BITS_Y, 0 };
			#endif
				
			hkIntVector	iv; iv.loadNotAligned<1>( (const hkUint32*)&vertex);
			iv.setBroadcast<0>(iv);
			iv.setShiftRight32( iv, (const hkIntVector&)shifts);
			iv.setAnd( iv, (const hkIntVector&)mask );
			iv.convertS32ToF32(*vectorOut);
		}
		else if(sizeof(T) == 4)
		{
			static const HK_ALIGN16( hkUint32 shifts[4] ) = { 0, NUM_BITS_X, NUM_BITS_X + NUM_BITS_Y, 0 };
			static const HK_ALIGN16( hkUint32 mask  [4] ) = { MASK_X, MASK_Y, MASK_Z, 0 };
				
			hkIntVector	iv; iv.loadNotAligned<1>( (const hkUint32*)&vertex);
			iv.setBroadcast<0>(iv);
			iv.setShiftRight32( iv, (const hkIntVector&)shifts);
			iv.setAnd( iv, (const hkIntVector&)mask );
			iv.convertS32ToF32(*vectorOut);
		}
		else if(sizeof(T) == 8)
		{
			static const HK_ALIGN16( hkUint32 shifts[4] )     = { 0,      NUM_BITS_X,             hkUint32(NUM_BITS_X+NUM_BITS_Y-32),	0 };
			static const HK_ALIGN16( hkUint32 maskXlYZ  [4] ) = { MASK_X, (1<<(32-NUM_BITS_X))-1, MASK_Z,								0 };
			static const HK_ALIGN16( hkUint32 maskuY  [4] )   = { 0,      MASK_Y,                 0,									0 };
			hkIntVector	iv; iv.loadNotAligned<2>( (const hkUint32*)&vertex);

			#if HK_ENDIAN_BIG	// !intel  
			hkIntVector ivYYXX; ivYYXX.setPermutation<hkVectorPermutation::YYXX>( iv );
			hkIntVector ivXXXX; ivXXXX.setBroadcast<0>(iv);
			#else				// intel
			hkIntVector ivYYXX; ivYYXX.setPermutation<hkVectorPermutation::XXYY>( iv );
			hkIntVector ivXXXX; ivXXXX.setBroadcast<1>(iv);
			#endif
				
			hkIntVector ivXlyZ;  ivXlyZ.setShiftRight32( ivYYXX, (const hkIntVector&)shifts);
			hkIntVector ivuY; ivuY.setShiftLeft32<32-NUM_BITS_X>(ivXXXX);
				
			ivXlyZ.setAnd( ivXlyZ,	(const hkIntVector&)maskXlYZ );	// results in   0, lowerBits of Y,  Z, 0
			ivuY.setAnd( ivuY,		(const hkIntVector&)maskuY );  	// results in   0, upper bits of Y, 0, 0 

			iv.setOr( ivXlyZ, ivuY);
				
			iv.convertS32ToF32(*vectorOut);
		}
		else
		{
			HK_ASSERT(0x7c09df24,0);
		}
	#elif defined(HK_PLATFORM_WIN32) && (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
		static const HK_ALIGN16( hkUint32 maskX[4] ) = { MASK_X, 0, 0, 0 };
		static const HK_ALIGN16( hkUint32 maskY[4] ) = { 0, MASK_Y, 0, 0 };
		static const HK_ALIGN16( hkUint32 maskZ[4] ) = { 0, 0, MASK_Z, 0 };

		if(sizeof(T) == 4)
		{
			hkIntVector a; a.load<1>( (hkUint32*)(&vertex) );

			hkIntVector z; z.setShiftRight32<31&(NUM_BITS_X + NUM_BITS_Y)>( a );z.setBroadcast<0>(z); z.setAnd( z, (const hkIntVector&)maskZ );
			hkIntVector y; y.setShiftRight32<NUM_BITS_X>( a );					y.setBroadcast<0>(y); y.setAnd( y, (const hkIntVector&)maskY );
			hkIntVector x; x.setAnd( a, (const hkIntVector&)maskX );

			hkIntVector	iv; iv.setOr( x, z ); iv.setOr( iv, y );
			iv.convertS32ToF32(*vectorOut);
		}
		else if(sizeof(T) == 8)
		{
			hkIntVector a; a.load<2>( (hkUint32*)(&vertex) );

			hkIntVector y; y.setShiftRight128<NUM_BITS_X/8>( a );
			y.setShiftRight32<NUM_BITS_X&0x7>( y );
			y.setAnd( y, (const hkIntVector&)maskY );

			hkIntVector z; z.setShiftRight32<31&(NUM_BITS_X + NUM_BITS_Y - 32)>( a );
			z.setBroadcast<1>(z);
			z.setAnd( z, (const hkIntVector&)maskZ );

			hkIntVector x; x.setAnd( a, (const hkIntVector&)maskX );

			hkIntVector	iv; iv.setOr( x, z ); iv.setOr( iv, y );
			iv.convertS32ToF32(*vectorOut);
		}
		else
		{
			hkIntVector	iv; iv.set((int) (vertex & MASK_X), (int) ((vertex >> NUM_BITS_X) & MASK_Y), (int) ((vertex >> (NUM_BITS_X + NUM_BITS_Y) ) & MASK_Z), 0);
			iv.convertS32ToF32(*vectorOut);
		}
	#else
		hkIntVector	iv; iv.set((int) (vertex & MASK_X), (int) ((vertex >> NUM_BITS_X) & MASK_Y), (int) ((vertex >> (NUM_BITS_X + NUM_BITS_Y) ) & MASK_Z), 0);
		iv.convertS32ToF32(*vectorOut);
	#endif
}



template< typename T, int NUM_BITS_X, int NUM_BITS_Y >
HK_FORCE_INLINE void decodeFractions3( const T& vertex, hkVector4f* HK_RESTRICT vectorOut )
{
	enum
	{
		TOTAL_BITS	=	sizeof(T) << 3,
		NUM_BITS_Z	=	TOTAL_BITS - (NUM_BITS_X + NUM_BITS_Y),
		MASK_X		=	(1<<NUM_BITS_X)-1,
		MASK_Y		=	(1<<NUM_BITS_Y)-1,
		MASK_Z		=	(1<<NUM_BITS_Z)-1
	};

	hkVector4f	rv;
	decode3<T,NUM_BITS_X,NUM_BITS_Y> ( vertex, &rv );

	HK_ALIGN_FLOAT( hkFloat32 ) invMask[4] = { 1.0f/MASK_X, 1.0f/hkFloat32(MASK_Y), 1.0f/hkFloat32(MASK_Z), 0.0f };

	hkVector4f	fractions; fractions.setMul( rv, (const hkVector4f&)invMask );

	vectorOut->setMul( rv, fractions );
}

#if defined(HK_REAL_IS_FLOAT)
/// Encode a vertex.
template< typename T, int NUM_BITS_X, int NUM_BITS_Y >
HK_FORCE_INLINE void	encode3(const hkAabb& aabb, hkVector4fParameter vertex, T* HK_RESTRICT output)
{
	enum
	{
		TOTAL_BITS	=	sizeof(T) << 3,
		NUM_BITS_Z	=	TOTAL_BITS - (NUM_BITS_X + NUM_BITS_Y),
		MASK_X		=	(1<<NUM_BITS_X)-1,
		MASK_Y		=	(1<<NUM_BITS_Y)-1,
		MASK_Z		=	(1<<NUM_BITS_Z)-1
	};

	hkVector4f   clampled; clampled.setClamped(vertex, aabb.m_min, aabb.m_max);
	hkVector4f	delta; delta.setSub(aabb.m_max, aabb.m_min);
	hkVector4f	invDelta; invDelta.setReciprocal<HK_ACC_23_BIT,HK_DIV_IGNORE>(delta);
	HK_ASSERT( 0xf0345456, delta.greaterZero().allAreSet<hkVector4ComparisonMask::MASK_XYZ>());	// check for valid aabb
	hkVector4f	offset; offset.setSub(vertex, aabb.m_min);
	hkVector4f	fractions; fractions.setMul(offset, invDelta);

	HK_ALIGN_FLOAT( hkFloat32 ) invMask[4] = { 1.0f/MASK_X, 1.0f/hkFloat32(MASK_Y), 1.0f/hkFloat32(MASK_Z), 0.0f };
	fractions.setAddMul( hkVector4f::getConstant<HK_QUADREAL_INV_2>(), fractions, (const hkVector4f&)invMask );

	hkIntVector i; i.setConvertF32toS32( fractions );

	const T		x = i.getU32<0>();
	const T		y = i.getU32<1>();
	const T		z = i.getU32<2>();
	const T		p = (z << (NUM_BITS_X + NUM_BITS_Y)) | (y << NUM_BITS_Y) | x;
	output[0] = p;
}
#endif // defined(HK_REAL_IS_FLOAT)

HK_FORCE_INLINE hkFloat32*	filteredStoreToStream( hkVector4fParameter vector, hkVector4ComparisonMask::Mask m, hkFloat32* dest )
{
	const unsigned int ts = sizeof(hkInt32);
	{ const hkUint32 n=hkVector4ComparisonMask::MASK_X;   vector.					store<1>(dest);	dest = hkAddByteOffset( dest, filteredStoreToStream_calcOffset( n, ts, m ) ); }
	{ const hkUint32 n=hkVector4ComparisonMask::MASK_Y;   vector.getComponent<1>().	store<1>(dest); dest = hkAddByteOffset( dest, filteredStoreToStream_calcOffset( n, ts, m ) ); }
	{ const hkUint32 n=hkVector4ComparisonMask::MASK_Z;   vector.getComponent<2>().	store<1>(dest); dest = hkAddByteOffset( dest, filteredStoreToStream_calcOffset( n, ts, m ) ); }
	{ const hkUint32 n=hkVector4ComparisonMask::MASK_W;   vector.getComponent<3>().	store<1>(dest); dest = hkAddByteOffset( dest, filteredStoreToStream_calcOffset( n, ts, m ) ); }
	return dest;
}

#if defined HK_COMPILER_MSVC
//  warning C4723: potential divide by 0
#	pragma warning(pop)
#endif

} // namespace hkVectorPackUtility

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
