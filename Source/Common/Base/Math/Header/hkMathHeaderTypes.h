/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
//HK_REFLECTION_PARSER_EXCLUDE_FILE

#if defined(HK_PLATFORM_WIIU)
#	include <Common/Base/Math/Types/WiiU/hkWiiuMathTypes.h>
#elif HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED
#	if defined(HK_COMPILER_HAS_INTRINSICS_IA32)
#		include <Common/Base/Math/Types/Sse/hkSseMathTypes.h>
#	elif defined(HK_PLATFORM_XBOX360)
#		include <Common/Base/Math/Types/Xbox360/hkXbox360MathTypes.h>
#	elif defined(HK_ARCH_PS3) || defined(HK_ARCH_PS3SPU)
#		define HK_PLATFORM_SUPPORTS_MANY_SIMD_REGISTERS
#		include <Common/Base/Math/Types/Ps3/hkPs3MathTypes.h>
#	elif defined(HK_COMPILER_HAS_INTRINSICS_NEON)
#		define HK_PLATFORM_SUPPORTS_MANY_SIMD_REGISTERS
#		include <Common/Base/Math/Types/Neon/hkNeonMathTypes.h>
#	elif defined(HK_PLATFORM_LRB)
#		include <Common/Base/Math/Types/Lrb/hkLrbMathTypes.h>
#	else // simd
#		error Unknown platform for SIMD
#	endif
#else // not simd
#	if defined(HK_PLATFORM_RVL) && defined(HK_ENABLE_PAIRED_SINGLE_OPTS)
#		include <Common/Base/Math/Types/Gc/hkGcMathTypes.h>
#	else
#		include <Common/Base/Math/Types/Fpu/hkFpuMathTypes.h>
#	endif
#endif


#ifndef HK_MASK_TO_VECTORf
#define HK_MASK_TO_VECTORf(x) (hkQuadFloat32)x
#endif

#ifndef HK_MASK_TO_VECTORd
#define HK_MASK_TO_VECTORd(x) (hkQuadDouble64)x
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
