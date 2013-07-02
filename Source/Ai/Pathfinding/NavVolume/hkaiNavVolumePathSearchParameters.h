/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_PATHFINDING_NAVVOLUME_PATH_SEARCH_PARAMETERS_H
#define HK_AI_PATHFINDING_NAVVOLUME_PATH_SEARCH_PARAMETERS_H

#include <Common/Base/Math/Vector/hkPackedVector3.h>

class hkaiAstarCostModifier;
class hkaiAstarEdgeFilter;


/// Parameters for controlling path searches on nav volumes.
/// This input data may apply to a batch of path requests.
struct hkaiNavVolumePathSearchParameters
{
	// +version(3)
	HK_DECLARE_REFLECTION();
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_ASTAR, hkaiNavVolumePathSearchParameters );

	//
	// Memory control defaults
	//	Default values for controlling search memory usage are given in two variants:
	//	* Default values for single-threaded, synchronous requests (via hkaiVolumePathfindingUtil::findPath)
	//	  These default values are generously large, and can be reduced to conserve memory.
	//	* default values for multi-threaded, asynchronous requests (via hkaiNavVolumeAStarJob)
	//	  These default values are sized for executing searches on the SPUs, and cannot be increased
	//	  for jobs processed on the SPU.
	//

	enum ElementSizes
	{
#ifndef HK_REAL_IS_DOUBLE
		OPENSET_ELEMENT_SIZE = 8, // hkaiHeapOpenSet::IndexCostPair
		SEARCH_STATE_ELEMENT_SIZE = 16 + 2, // sizeof(hkaiSearchStateNode) + sizeof(hkInt16)
#else
		OPENSET_ELEMENT_SIZE = 16, // hkaiHeapOpenSet::IndexCostPair
		SEARCH_STATE_ELEMENT_SIZE = 32 + 2, // sizeof(hkaiSearchStateNode) + sizeof(hkInt16)
#endif
		SEARCH_STATE_OVERHEAD = 512 // sizeof(hkInt16)*256
	};

	enum
	{
		OPEN_SET_SINGLE_THREADED_NUM_ELEMENTS = 16384,
		SEARCH_STATE_SINGLE_THREADED_NUM_ELEMENTS = 2*OPEN_SET_SINGLE_THREADED_NUM_ELEMENTS,

		OPEN_SET_MULTI_THREADED_NUM_ELEMENTS = 1024,
		SEARCH_STATE_MULTI_THREADED_NUM_ELEMENTS = 2*OPEN_SET_MULTI_THREADED_NUM_ELEMENTS,
	};

	/// Memory defaults for single-threaded, synchronous requests
	enum MemoryDefaultsSingleThreaded
	{
		OPEN_SET_SIZE_SINGLE_THREADED				= OPEN_SET_SINGLE_THREADED_NUM_ELEMENTS               * OPENSET_ELEMENT_SIZE                              /* 131072 */,
		SEARCH_STATE_SIZE_SINGLE_THREADED			= SEARCH_STATE_SINGLE_THREADED_NUM_ELEMENTS           * SEARCH_STATE_ELEMENT_SIZE + SEARCH_STATE_OVERHEAD /* 590336 */,
	};

	/// Memory defaults for multi-threaded, asynchronous requests
	enum MemoryDefaultsMultiThreaded
	{
		OPEN_SET_SIZE_MULTI_THREADED				= OPEN_SET_MULTI_THREADED_NUM_ELEMENTS               * OPENSET_ELEMENT_SIZE                              /*  8192 */,
		SEARCH_STATE_SIZE_MULTI_THREADED			= SEARCH_STATE_MULTI_THREADED_NUM_ELEMENTS           * SEARCH_STATE_ELEMENT_SIZE + SEARCH_STATE_OVERHEAD /* 37376 */,
	};


	hkaiNavVolumePathSearchParameters();
	hkaiNavVolumePathSearchParameters(hkFinishLoadedObjectFlag f);

	hkBool32 operator ==( const hkaiNavVolumePathSearchParameters& o ) const;

	inline hkBool32 shouldPerformLineOfSightCheck() const;

		/// Set the up vector for the search.
	inline void setUp( hkVector4Parameter up );

		/// World up vector.
	hkPackedUnitVector<3> m_up; //+overridetype(hkInt16[3])

		/// Optional pointer to hkaiAstarCostModifier, which can be used to modify costs
		/// based on the hkaiAgentTraversalInfo.
	const hkaiAstarCostModifier* m_costModifier; //+nosave

		/// Optional pointer to hkaiAstarEdgeFilter, which can be used to reject nav mesh edges
		/// based on the hkaiAgentTraversalInfo
	const hkaiAstarEdgeFilter* m_edgeFilter; //+nosave

	enum LineOfSightFlags 
	{
		/// Don't perform any line-of-sight check before A*
		NO_LINE_OF_SIGHT_CHECK = 0,

		/// Perform a line-of-sight check in order to try to avoid a full A* search.
		/// If the line-of-sight check between the start and end points succeeds, this is returned as the shortest path.
		/// If this flag is set, a line-of-sight check will be performed if no cost modifier is specified.
		CHECK_LINE_OF_SIGHT_IF_NO_COST_MODIFIER = 1,

		

		/// Always do a line-of-sight checked, regardless of whether or not a cost modifier is specified.
		/// This check should be disabled when using a cost modifier to increase the cost of certain path edges, 
		/// as it will find any straight-line path, even if a non-LOS path is actually preferred due to modified edge costs.
		CHECK_LINE_OF_SIGHT_ALWAYS = 4
	};

	/// Controls whether a line-of-sight check is performed before A*, and how the check is configured.
	/// See LineOfSightFlags for more details.
	hkFlags<LineOfSightFlags, hkUint8> m_lineOfSightFlags; //+default(hkaiNavVolumePathSearchParameters::CHECK_LINE_OF_SIGHT_IF_NO_COST_MODIFIER)

		/// Weight to apply to the heuristic function. Values greater than 1
		/// result in faster but less accurate searches (non-optimal paths may
		/// be returned). Defaults to 1.
	hkReal m_heuristicWeight; //+default(1.0f)

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

#include <Ai/Pathfinding/NavVolume/hkaiNavVolumePathSearchParameters.inl>

#endif // HK_AI_PATHFINDING_NAVVOLUME_PATH_SEARCH_PARAMETERS_H

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
