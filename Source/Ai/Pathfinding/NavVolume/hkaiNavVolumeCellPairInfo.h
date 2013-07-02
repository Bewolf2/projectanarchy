/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_NAVVOLUME_CELL_PAIR_INFO_H
#define HK_AI_NAVVOLUME_CELL_PAIR_INFO_H

#include <Ai/Pathfinding/NavVolume/hkaiNavVolumeInstance.h>

#ifdef HK_PLATFORM_SPU
#	include <Ai/Pathfinding/NavVolume/Spu/hkaiSpuNavVolumeAccessor.h>
#endif

/// Information regarding a pair of edges, and the cell between them.
/// The cell will be the cell for the outgoing edge, and the opposite cell for the incoming edge.
struct hkaiNavVolumeCellPairInfo
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI,hkaiNavVolumeCellPairInfo);

	/// Flags to indicate from where the cost modifier is being called
	enum QueryType
	{
		/// Called from A* queries, e.g., hkaiPathfindingUtil::findPath
		QUERY_TYPE_ASTAR			= 1<<0,

		/// Called during line of sight queries, e.g., hkaiLineOfSightUtil::checkLineOfSight
		QUERY_TYPE_LINE_OF_SIGHT	= 1<<1,

		/// Called by the user, i.e., non-Havok code
		QUERY_TYPE_USER				= 1<<3,

		/// Called to examine the start point of a path
		QUERY_START_POINT			= 1<<4,

		/// Called to examine an intermediate edge in the middle of a path
		QUERY_INTERMEDIATE_POINT	= 1<<5,

		/// Called to examine the goal of a path
		QUERY_END_POINT				= 1<<6
	};

	/// Information regarding an cell being examined.
	struct CellInfo
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI,hkaiNavVolumeCellPairInfo::CellInfo);

		/// The key of the cell.
		HK_PAD_ON_SPU( hkaiPackedKey ) m_cellKey;

		/// A pointer to the cell
		HK_PAD_ON_SPU( const hkaiNavVolume::Cell* ) m_cell;

		/// The position in the cell (or the start/end position of the search)
		hkVector4 m_position;

		inline CellInfo();
		inline void validate( const hkaiNavVolumeAccessor* vol = HK_NULL ) const;

		inline void setCell(hkaiPackedKey cIdx, const hkaiNavVolume::Cell* cell, hkVector4Parameter pos);
	};

	/// Edge information for the current cell.
	CellInfo m_currentCellInfo;

	/// Edge information for the adjacent cell.
	CellInfo m_adjacentCellInfo;

	/// Index of the edge
	HK_PAD_ON_SPU( hkaiPackedKey ) m_edgeKey;

	/// Pointer to the edge
	HK_PAD_ON_SPU( const hkaiNavVolume::Edge*	) m_edge;

	inline hkaiNavVolumeCellPairInfo();
	inline void validate( const hkaiNavVolumeAccessor* volume ) const;
};

#include <Ai/Pathfinding/NavVolume/hkaiNavVolumeCellPairInfo.inl>

#endif // HK_AI_NAVVOLUME_CELL_PAIR_INFO_H

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
