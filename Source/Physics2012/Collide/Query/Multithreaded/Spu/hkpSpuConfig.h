/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_COLLIDE_SPU_CONFIG_H
#define HK_COLLIDE_SPU_CONFIG_H


#include <Common/Base/Spu/Config/hkSpuConfig.h>
#include <Common/Base/hkBase.h>
#include <Physics2012/Collide/Agent/Collidable/hkpCollidable.h>
#include <Physics2012/Internal/Collide/Mopp/Code/hkpMoppCode.h>


// ===============================================================================================================================================================================================
//  GENERAL
// ===============================================================================================================================================================================================

// The total amount of SPU memory dedicated to the hkSpuStack in the collision query .elf
#define HK_SPU_TOTAL_COLLISION_QUERY_BUFFER_SIZE 62000
#define HK_SPU_TOTAL_RAYCAST_QUERY_BUFFER_SIZE 60 * 1024

// This is the size of the global UNTYPED cache. Must be a power of 2, should be reasonably sized.
#define HK_SPU_COLLIDE_UNTYPED_CACHE_SIZE 16384
#define HK_SPU_RAYCAST_UNTYPED_CACHE_SIZE 16384

// This is the size of the global MOPP cache. Must be a power of 2, should be reasonably sized.
// Note: this memory block will also be used for bringing in the broadphase for all jobs that use the broadphase on SPU, so it shouldn't be sized too small!
#define HK_SPU_COLLIDE_MOPP_CACHE_SIZE 8192
#define HK_SPU_RAYCAST_MOPP_CACHE_SIZE 16384

// automatically set/calculated values; DO NOT CHANGE
enum { HK_SPU_COLLIDE_UNTYPED_CACHE_NUM_ROWS	= HK_SPU_COLLIDE_UNTYPED_CACHE_SIZE	/ (4 * HK_SPU_UNTYPED_CACHE_LINE_SIZE )	};
enum { HK_SPU_COLLIDE_MOPP_CACHE_NUM_ROWS		= HK_SPU_COLLIDE_MOPP_CACHE_SIZE	/ (4 * HK_MOPP_CHUNK_SIZE )	};

enum { HK_SPU_RAYCAST_UNTYPED_CACHE_NUM_ROWS	= HK_SPU_RAYCAST_UNTYPED_CACHE_SIZE	/ (4 * HK_SPU_UNTYPED_CACHE_LINE_SIZE )	};
enum { HK_SPU_RAYCAST_MOPP_CACHE_NUM_ROWS		= HK_SPU_RAYCAST_MOPP_CACHE_SIZE	/ (4 * HK_MOPP_CHUNK_SIZE )	};



#if defined(HK_PLATFORM_SPU)

// ===============================================================================================================================================================================================
//  SHAPE RAYCAST
// ===============================================================================================================================================================================================

// The number of cache rows to be used when caching the collidables; allocated on the hkSpuStack
#define HK_SPU_COLLIDE_SHAPE_RAYCAST_JOB_COLLIDABLE_NUM_CACHE_ROWS		8

// This is the size of the top level shape cache. A cache miss is not really expensive, as the dma is prefetched.
// Must be power of 2, minimum 1k
#define HK_SPU_COLLIDE_SHAPE_RAYCAST_JOB_SHAPE_CACHE_SIZE				2048

// This is the size of the transform cache. A cache miss is not really expensive, as the dma is prefetched.
// Must be power of 2, minimum 256byte
#define HK_SPU_COLLIDE_SHAPE_RAYCAST_JOB_TRANSFORM_CACHE_SIZE			1024

// The maximum buffer used for bringing in the collision filter from ppu; this should be at least large enough to hold the type of filter(s) you intend to use; allocated on the hkSpuStack
#define HK_SPU_COLLIDE_SHAPE_RAYCAST_JOB_COLLISION_FILTER_BUFFER_SIZE	256

// automatically calculated values; DO NOT CHANGE
enum { HK_SPU_COLLIDE_SHAPE_RAYCAST_JOB_TRANSFORM_NUM_CACHE_ROWS		= HK_SPU_COLLIDE_SHAPE_RAYCAST_JOB_TRANSFORM_CACHE_SIZE	/ ( 4 * sizeof(hkTransform) )		};
enum { HK_SPU_COLLIDE_SHAPE_RAYCAST_JOB_SHAPE_CACHE_NUM_ROWS			= HK_SPU_COLLIDE_SHAPE_RAYCAST_JOB_SHAPE_CACHE_SIZE		/ ( 4 * HK_SPU_MAXIMUM_SHAPE_SIZE ) };


namespace hkSpuShapeRayCastDmaGroups
{
	enum
	{
		GET_COMMANDS			= 0,
		GET_COLLIDABLE_ARRAY	= 1,
		PIPELINE_TRANSFERS		= 2, // 3 pipeline stages
		GET_MOPP_CHUNK			= 5,
		WRITE_RESULTS			= 6,
		//NEXT_FREE				= 7,
	};
}


// ===============================================================================================================================================================================================
//  WORLD RAYCAST
// ===============================================================================================================================================================================================

namespace hkSpuWorldRayCastDmaGroups
{
	enum
	{
		GET_BROADPHASE											= 0,
		GET_BROADPHASE_ARRAYS_AND_COLLISION_FILTER_AND_COMMANDS	= 1,
		GET_COLLIDABLE											= 2,
		GET_SHAPE												= 3,
		GET_TRANSFORM											= 4,
		PIPELINE_TRANSFERS										= 5, // 2 pipeline stages
	};
}

// The number of cache rows to be used when caching the collidables; allocated on the hkSpuStack
#define HK_SPU_COLLIDE_WORLD_RAYCAST_JOB_COLLIDABLE_CACHE_NUM_ROWS		4

// This is the size of the shape cache. Must be power of 2, minimum 1k
#define HK_SPU_COLLIDE_WORLD_RAYCAST_JOB_SHAPE_CACHE_SIZE				2048

// This is the size of the transform cache. Must be power of 2, minimum 1k
#define HK_SPU_COLLIDE_WORLD_RAYCAST_JOB_TRANSFORM_CACHE_SIZE			1024

// The maximum buffer used for bringing in the collision filter from ppu; this should be at least large enough to hold the type of filter(s) you intend to use; allocated on the hkSpuStack
#define HK_SPU_COLLIDE_WORLD_RAYCAST_JOB_COLLISION_FILTER_BUFFER_SIZE	256

// automatically calculated values; DO NOT CHANGE
enum { HK_SPU_COLLIDE_WORLD_RAYCAST_JOB_COLLIDABLE_CACHE_SIZE			= HK_SPU_COLLIDE_WORLD_RAYCAST_JOB_COLLIDABLE_CACHE_NUM_ROWS	* ( 4 * HK_NEXT_MULTIPLE_OF(16, sizeof(hkCollidablePpu))	) };
enum { HK_SPU_COLLIDE_WORLD_RAYCAST_JOB_SHAPE_CACHE_NUM_ROWS			= HK_SPU_COLLIDE_WORLD_RAYCAST_JOB_SHAPE_CACHE_SIZE				/ ( 4 * HK_SPU_MAXIMUM_SHAPE_SIZE )							  };
enum { HK_SPU_COLLIDE_WORLD_RAYCAST_JOB_TRANSFORM_CACHE_NUM_ROWS		= HK_SPU_COLLIDE_WORLD_RAYCAST_JOB_TRANSFORM_CACHE_SIZE			/ ( 4 * sizeof(hkTransform)									) };


// ===============================================================================================================================================================================================
//  PAIR LINEAR CAST
// ===============================================================================================================================================================================================

namespace hkSpuPairLinearCastDmaGroups
{
	enum
	{
		GET_COMMANDS			= 0,
		GET_COLLISION_FILTER	= 1,
		PIPELINE_TRANSFERS		= 2, // 4 pipeline stages
		NEXT_FREE_DMA_GROUP		= 6, // we need this dummy group for a compile time assert
	};
}


// The maximum buffer used for bringing in the collision filter from ppu; this should be at least large enough to hold the type of filter(s) you intend to use; allocated on the hkSpuStack
#define HK_SPU_COLLIDE_PAIR_LINEAR_CAST_JOB_COLLISION_FILTER_BUFFER_SIZE	256

// The number of cache rows to be used when caching the collidables; allocated on the hkSpuStack
#define HK_SPU_COLLIDE_PAIR_LINEAR_CAST_JOB_COLLIDABLE_CACHE_NUM_ROWS		4

// This is the size of the top level shape cache. A cache miss is not really expensive, as the dma is prefetched.
// Must be power of 2, minimum 1k
#define HK_SPU_COLLIDE_PAIR_LINEAR_CAST_JOB_ROOT_SHAPE_CACHE_SIZE			2048

// This is the size of the transform cache. A cache miss is not really expensive, as the dma is prefetched.
// Must be power of 2, minimum 1k
#define HK_SPU_COLLIDE_PAIR_LINEAR_CAST_JOB_TRANSFORM_CACHE_SIZE			1024


// automatically calculated values; DO NOT CHANGE
enum { HK_SPU_COLLIDE_PAIR_LINEAR_CAST_JOB_ROOT_SHAPE_CACHE_NUM_ROWS		= HK_SPU_COLLIDE_PAIR_LINEAR_CAST_JOB_ROOT_SHAPE_CACHE_SIZE	/ ( 4 * HK_SPU_MAXIMUM_SHAPE_SIZE )	};
enum { HK_SPU_COLLIDE_PAIR_LINEAR_CAST_JOB_TRANSFORM_CACHE_NUM_ROWS			= HK_SPU_COLLIDE_PAIR_LINEAR_CAST_JOB_TRANSFORM_CACHE_SIZE	/ ( 4 * sizeof(hkTransform) )		};


// ===============================================================================================================================================================================================
//  WORLD LINEAR CAST
// ===============================================================================================================================================================================================

namespace hkSpuWorldLinearCastDmaGroups
{
	enum
	{
		GET_BROADPHASE																= 0,
		GET_BROADPHASE_ARRAYS_AND_COLLISION_FILTER_AND_COMMANDS_AND_INPUT_CONFIG	= 1,
		PIPELINE_TRANSFERS															= 2, // 2 pipeline stages
		GET_COLLIDABLES_IN_CALLBACK													= 4,
		GET_SHAPES_AND_TRANSFORMS_N_CALLBACKS										= 5,
	};
}

// The number of cache rows to be used when caching the collidables; allocated on the hkSpuStack
#define HK_SPU_COLLIDE_WORLD_LINEAR_CAST_JOB_COLLIDABLE_CACHE_NUM_ROWS		4

// This is the size of the shape cache. Must be power of 2, minimum 1k
#define HK_SPU_COLLIDE_WORLD_LINEAR_CAST_JOB_SHAPE_CACHE_SIZE				2048

// This is the size of the transform cache. Must be power of 2, minimum 1k
#define HK_SPU_COLLIDE_WORLD_LINEAR_CAST_JOB_TRANSFORM_CACHE_SIZE			1024

// The maximum buffer used for bringing in the collision filter from ppu; this should be at least large enough to hold the type of filter(s) you intend to use; allocated on the hkSpuStack
#define HK_SPU_COLLIDE_WORLD_LINEAR_CAST_JOB_COLLISION_FILTER_BUFFER_SIZE	256

// automatically calculated values; DO NOT CHANGE
enum { HK_SPU_COLLIDE_WORLD_LINEAR_CAST_JOB_COLLIDABLE_CACHE_SIZE			= HK_SPU_COLLIDE_WORLD_LINEAR_CAST_JOB_COLLIDABLE_CACHE_NUM_ROWS	* ( 4 * HK_NEXT_MULTIPLE_OF(16, sizeof(hkCollidablePpu))	) };
enum { HK_SPU_COLLIDE_WORLD_LINEAR_CAST_JOB_SHAPE_CACHE_NUM_ROWS			= HK_SPU_COLLIDE_WORLD_LINEAR_CAST_JOB_SHAPE_CACHE_SIZE				/ ( 4 * HK_SPU_MAXIMUM_SHAPE_SIZE							) };
enum { HK_SPU_COLLIDE_WORLD_LINEAR_CAST_JOB_TRANSFORM_CACHE_NUM_ROWS		= HK_SPU_COLLIDE_WORLD_LINEAR_CAST_JOB_TRANSFORM_CACHE_SIZE			/ ( 4 * sizeof(hkTransform)									) };


// ===============================================================================================================================================================================================
//  MOPP AABB
// ===============================================================================================================================================================================================

namespace hkSpuMoppAabbDmaGroups
{
	enum
	{
		GET_COMMANDS		= 0,
		PIPELINE_TRANSFERS	= 1, // 2 pipeline stages
		GET_MOPP_CHUNK		= 3,
		//NEXT_FREE			= 4,
	};
}


// ===============================================================================================================================================================================================
//  PAIR GET CLOSEST POINTS
// ===============================================================================================================================================================================================

// The buffer size used for bringing in the childInfos in a hkpListShape; essentially this is the batch size for processing the children, so the larger the buffer the more children can be batched; allocated on the real stack
#define HK_SPU_COLLIDE_PAIR_GET_CLOSEST_POINTS_JOB_CHILD_INFO_BUFFER_SIZE		256

// The maximum buffer used for bringing in the collision filter from ppu; this should be at least large enough to hold the type of filter(s) you intend to use; allocated on the hkSpuStack
#define HK_SPU_COLLIDE_PAIR_GET_CLOSEST_POINTS_JOB_COLLISION_FILTER_BUFFER_SIZE	256

// The number of cache rows to be used when caching the collidables; allocated on the hkSpuStack
#define HK_SPU_COLLIDE_PAIR_GET_CLOSEST_POINTS_JOB_COLLIDABLE_NUM_CACHE_ROWS	4

// This is the size of the top level shape cache. A cache miss is not really expensive, as the dma is prefetched.
// Must be power of 2, minimum 1k
#define HK_SPU_COLLIDE_PAIR_GET_CLOSEST_POINTS_JOB_ROOT_SHAPE_CACHE_SIZE		2048

// This is the size of the transform cache. A cache miss is not really expensive, as the dma is prefetched.
// Must be power of 2, minimum 1k
#define HK_SPU_COLLIDE_PAIR_GET_CLOSEST_POINTS_JOB_TRANSFORM_CACHE_SIZE			1024


// automatically calculated values; DO NOT CHANGE
enum { HK_SPU_COLLIDE_PAIR_GET_CLOSEST_POINTS_JOB_ROOT_SHAPE_NUM_CACHE_ROWS		= HK_SPU_COLLIDE_PAIR_GET_CLOSEST_POINTS_JOB_ROOT_SHAPE_CACHE_SIZE		/ ( 4 * HK_SPU_MAXIMUM_SHAPE_SIZE )							};
enum { HK_SPU_COLLIDE_PAIR_GET_CLOSEST_POINTS_JOB_TRANSFORM_NUM_CACHE_ROWS		= HK_SPU_COLLIDE_PAIR_GET_CLOSEST_POINTS_JOB_TRANSFORM_CACHE_SIZE		/ ( 4 * sizeof(hkTransform) )								};


namespace hkSpuPairGetClosestPointsDmaGroups
{
	enum
	{
		GET_COMMANDS			= 0,
		GET_COLLISION_FILTER	= 1,
		PIPELINE_TRANSFERS		= 2,	// 4 pipeline stages
		GET_CHILD_INFOS			= 15,	// This DMA group is also used from the linearCastJob. To avoid any potential DMA group collisions with hkSpuPairLinearCastDmaGroups we use a very large number here.
		//NEXT_FREE				= 16,
	};
}


// ===============================================================================================================================================================================================
//  WORLD GET CLOSEST POINTS
// ===============================================================================================================================================================================================

namespace hkSpuWorldGetClosestPointsDmaGroups
{
	enum
	{
		GET_BROADPHASE_AND_COMMANDS_AND_FILTER	= 0,
 		GET_COLLIDABLE							= 1,
 		GET_SHAPE_AND_TRANSFORM					= 2,
 		PUT_RESULTS0							= 3, // Write back to PPU
		PUT_RESULTS1							= 4, // We double-buffer the writebacks
	};
}

// This is the size of the shape cache. Must be power of 2, minimum 1k
#define HK_SPU_COLLIDE_WORLD_GET_CLOSEST_POINTS_JOB_SHAPE_CACHE_SIZE					2048

// This is the size of the transform cache. Must be power of 2, minimum 1k
#define HK_SPU_COLLIDE_WORLD_GET_CLOSEST_POINTS_JOB_TRANSFORM_CACHE_SIZE				2048

// The number of cache rows to be used when caching the collidables; allocated on the hkSpuStack
#define HK_SPU_COLLIDE_WORLD_GET_CLOSEST_POINTS_JOB_COLLIDABLE_NUM_CACHE_ROWS	4

// The maximum number of commands that will be packed into one hkpPairGetClosestPointsJob subjob. Note that a temporary buffer of this size will have to be allocated on the hkSpuStack.
#define HK_SPU_COLLIDE_WORLD_GET_CLOSEST_POINTS_MAX_NUM_COMMANDS_PER_SUB_JOB			20

// The maximum buffer used for bringing in the collision filter from ppu; this should be at least large enough to hold the type of filter(s) you intend to use; allocated on the hkSpuStack
#define HK_SPU_COLLIDE_WORLD_GET_CLOSEST_POINTS_JOB_COLLISION_FILTER_BUFFER_SIZE	256

// automatically calculated values; DO NOT CHANGE
enum { HK_SPU_COLLIDE_WORLD_GET_CLOSEST_POINTS_JOB_ROOT_SHAPE_NUM_CACHE_ROWS			= HK_SPU_COLLIDE_WORLD_GET_CLOSEST_POINTS_JOB_SHAPE_CACHE_SIZE		/ ( 4 * HK_SPU_MAXIMUM_SHAPE_SIZE )	};
enum { HK_SPU_COLLIDE_WORLD_GET_CLOSEST_POINTS_JOB_TRANSFORM_NUM_CACHE_ROWS				= HK_SPU_COLLIDE_WORLD_GET_CLOSEST_POINTS_JOB_TRANSFORM_CACHE_SIZE  / ( 4 * sizeof(hkTransform) )		};


#endif // #if defined(HK_PLATFORM_SPU)


#endif // HK_COLLIDE_SPU_CONFIG_H

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
