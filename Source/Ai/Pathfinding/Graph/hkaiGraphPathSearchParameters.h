/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_PATHFINDING_GRAPH_PATH_SEARCH_PARAMETERS_H
#define HK_AI_PATHFINDING_GRAPH_PATH_SEARCH_PARAMETERS_H

/// Parameters for controlling path searches on directed graphs.
/// This input data may apply to a batch of path requests.
struct hkaiGraphPathSearchParameters
{
	// +version(0)
	HK_DECLARE_REFLECTION();
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_ASTAR, hkaiGraphPathSearchParameters );

	//
	// Memory control defaults
	//	Default values for controlling search memory usage are given in two variants:
	//	* Default values for single-threaded, synchronous requests (via hkaiPathfindingUtil::findGraphPath)
	//	  These default values are generously large, and can be reduced to conserve memory.
	//	* default values for multi-threaded, asynchronous requests (via hkaiDirectedGraphAStarJob)
	//	  These default values are sized for executing searches on the SPUs, and cannot be increased
	//	  for jobs processed on the SPU.
	//

	/// Memory defaults for single-threaded, synchronous requests
	enum MemoryDefaultsSingleThreaded
	{
		OPEN_SET_SIZE_SINGLE_THREADED				= 32768,
		SEARCH_STATE_SIZE_SINGLE_THREADED			= 147968,
	};

	/// Memory defaults for multi-threaded, asynchronous requests
	enum MemoryDefaultsMultiThreaded
	{
		OPEN_SET_SIZE_MULTI_THREADED				= 2048,
		SEARCH_STATE_SIZE_MULTI_THREADED			= 9728,
	};


	hkaiGraphPathSearchParameters();

	hkaiGraphPathSearchParameters(hkFinishLoadedObjectFlag f);


	//
	// Memory control parameters
	// Reducing these numbers will reduce the size of memory allocations during the search,
	// but may cause the search to terminate early
	//

	/// Maximum memory for nodes stored the open set, in bytes. A value of 0 will use the appropriate default.
	/// Note that the open set size can be considerably lower than the search state size, since the open set can never
	/// grow larger than the search state, and nodes in the open set are half the size of search state nodes (8 vs. 16)
	int m_maxOpenSetSizeBytes; //+default(0)

	/// Maximum memory for nodes stored if the search state, in bytes. A value of 0 will use the appropriate default.
	int m_maxSearchStateSizeBytes; //+default(0)
};

#include <Ai/Pathfinding/Graph/hkaiGraphPathSearchParameters.inl>

#endif // HK_AI_PATHFINDING_GRAPH_PATH_SEARCH_PARAMETERS_H

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
