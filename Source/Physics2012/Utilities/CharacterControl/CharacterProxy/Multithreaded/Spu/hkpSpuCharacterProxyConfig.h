/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_CHARACTER_PROXY_SPU_CONFIG_H
#define HK_CHARACTER_PROXY_SPU_CONFIG_H


#include <Common/Base/Spu/Config/hkSpuConfig.h>
#include <Physics2012/Internal/Collide/Mopp/Code/hkpMoppCode.h>

// ===============================================================================================================================================================================================
//  GENERAL
// ===============================================================================================================================================================================================

// The total amount of SPU memory dedicated to the hkStackMemory in the character proxy .elf
#define HK_SPU_TOTAL_CHARACTER_PROXY_BUFFER_SIZE 62 * 1024


// This is the size of the global UNTYPED cache. Must be a power of 2, should be reasonably sized.
#define HK_SPU_CHARACTER_PROXY_UNTYPED_CACHE_SIZE 16384

// This is the size of the global MOPP cache. Must be a power of 2, should be reasonably sized.
// Note: this memory block will also be used for bringing in the broadphase for all jobs that use the broadphase on SPU, so it shouldn't be sized too small!
#define HK_SPU_CHARACTER_PROXY_MOPP_CACHE_SIZE 16384

// automatically set/calculated values; DO NOT CHANGE
enum { HK_SPU_CHARACTER_PROXY_UNTYPED_CACHE_NUM_ROWS	= HK_SPU_CHARACTER_PROXY_UNTYPED_CACHE_SIZE	/ (4 * HK_SPU_UNTYPED_CACHE_LINE_SIZE )	};
enum { HK_SPU_CHARACTER_PROXY_MOPP_CACHE_NUM_ROWS		= HK_SPU_CHARACTER_PROXY_MOPP_CACHE_SIZE	/ (4 * HK_MOPP_CHUNK_SIZE )	};

// This is the size of the linear cast collector buffer
#define FIXED_BUFFER_CD_POINT_COLLECTOR_CAPACTITY_ON_SPU 20

// This capacity limits the manifold, bodies and phantoms hkArraySpu arrays as we can't resize on SPU
#define HK_MANIFOLD_CAPACITY_ON_SPU 10


#if defined(HK_PLATFORM_SPU)

// ===============================================================================================================================================================================================
//  CHARACTER PROXY INTEGRATE
// ===============================================================================================================================================================================================

namespace hkSpuCharacterProxyDmaGroups
{
	enum
	{
		GET_COMMANDS							= 0,
		GET_LINEAR_CAST_DATA					= 1,
		GET_COLLIDABLE_AND_SHAPE_AND_TRANSFORM	= 2,
		WRITE_BACK_DATA							= 3,
	};
}

// The maximum buffer used for bringing in the collision filter from ppu; this should be at least large enough to hold the type of filter(s) you intend to use; allocated on the hkStackMemory
#define HK_SPU_CHARACTER_PROXY_INTEGRATE_JOB_COLLISION_FILTER_BUFFER_SIZE	256

// The number of cache rows to be used when caching the collidable; allocated on the hkStackMemory
// Must be power of 2
#define HK_SPU_CHARACTER_PROXY_INTEGRATE_JOB_COLLIDABLE_CACHE_NUM_ROWS		4

// This is the size of the top level shape cache. A cache miss is not really expensive, as the dma is prefetched.
// Must be power of 2, minimum 1k
#define HK_SPU_CHARACTER_PROXY_INTEGRATE_JOB_ROOT_SHAPE_CACHE_SIZE			2048

// This is the size of the transform cache. A cache miss is not really expensive, as the dma is prefetched.
// Must be power of 2, minimum 1k
#define HK_SPU_CHARACTER_PROXY_INTEGRATE_JOB_TRANSFORM_CACHE_SIZE			1024

// automatically calculated values; DO NOT CHANGE
enum { HK_SPU_CHARACTER_PROXY_INTEGRATE_JOB_ROOT_SHAPE_CACHE_NUM_ROWS		= HK_SPU_CHARACTER_PROXY_INTEGRATE_JOB_ROOT_SHAPE_CACHE_SIZE		/ ( 4 * HK_SPU_MAXIMUM_SHAPE_SIZE )	};
enum { HK_SPU_CHARACTER_PROXY_INTEGRATE_JOB_TRANSFORM_CACHE_NUM_ROWS		= HK_SPU_CHARACTER_PROXY_INTEGRATE_JOB_TRANSFORM_CACHE_SIZE			/ ( 4 * sizeof(hkTransform) ) };


#endif // #if defined(HK_PLATFORM_SPU)


#endif // HK_CHARACTER_PROXY_SPU_CONFIG_H

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
