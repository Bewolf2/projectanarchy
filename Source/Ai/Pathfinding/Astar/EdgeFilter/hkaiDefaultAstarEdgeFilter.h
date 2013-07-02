/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_DEFAULT_NAV_MESH_EDGE_FILTER_H
#define HK_AI_DEFAULT_NAV_MESH_EDGE_FILTER_H

#include <Ai/Pathfinding/NavMesh/hkaiNavMesh.h>
#include <Ai/Pathfinding/Astar/EdgeFilter/hkaiAstarEdgeFilter.h>

extern const class hkClass hkaiDefaultAstarEdgeFilterClass;

struct hkaiAgentTraversalInfo;

/// This edge filter uses the user data stored in the edges of a nav mesh or cell of a nav volume to decide if a path 
/// is valid or traversable. Only the bottom 5 bits of the user data is examined. 
/// For nav mesh searches, these bits for a 32 entry lookup table of masks. The masks are compared against and agent 
/// mask supplied during the query.
/// For nav volume searches, the cell is allowed if the bit indexed by the user data is set in m_cellMaskLookupTable.
///
/// See NavMeshEdgeFilterDemo for an example of how to use this.
class hkaiDefaultAstarEdgeFilter : public hkaiAstarEdgeFilter
{
	public:
		//+version(2)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_NAVMESH );
		
			/// Constructor, by default enables all collisions between all layers
		hkaiDefaultAstarEdgeFilter();

		hkaiDefaultAstarEdgeFilter(hkFinishLoadedObjectFlag flag) 
			: hkaiAstarEdgeFilter(flag) { }

		~hkaiDefaultAstarEdgeFilter() {}

		enum 
		{
			NAV_VOLUME_LOOKUP_TABLE_SIZE = 32
		};


		//
		// Nav mesh filtering setup
		//

			/// Enable between agentCategory and edgeCategory
		void navMeshEnableBetween(int agentCategory, int edgeCategory);

			/// Disables between agentCategory and edgeCategory
		void navMeshDisableBetween(int agentCategory, int edgeCategory);
		
			/// Enables between the specified categories
			/// e.g., to enable between one agent category and all edge categories,
			/// call enableUsingBitfield( 1 << myLayer, 0x1ffff)
		void navMeshEnableUsingBitfield(hkUint32 agentBitsA, hkUint32 edgeBitsB);

			/// Disables between the specified categories.
			/// See enableUsingBitfield for how to use bit fields
		void navMeshDisableUsingBitfield(hkUint32 agentBitsA, hkUint32 edgeBitsB);

		//
		// Nav volume filtering setup
		//


			/// Enable between cellCategory
		void navVolumeEnableBetween( int cellCategory );

			/// Disables between cellCategory
		void navVolumeDisableBetween( int cellCategory );

			/// Enables between the specified categories
		void navVolumeEnableUsingBitfield(hkUint32 cellBits);

			/// Disables between the specified categories.
		void navVolumeDisableUsingBitfield( hkUint32 cellBits);

		//
		// hkaiAstarEdgeFilter interface
		//

			/// The actual filter implementation between two hkUint32 values.
			/// Returns true if traversal is enabled based on their masks.
		virtual bool isEnabled( const NavMeshIsEnabledCallbackContext& context ) const HK_OVERRIDE;

			/// The actual filter implementation between two hkUint32 values.
			/// Returns true if traversal is enabled based on their masks.
		virtual bool isEnabled( const NavVolumeIsEnabledCallbackContext& context ) const HK_OVERRIDE;


	protected:
			/// The bitfield check used by isEnabled
		bool isEnabledByBitfield( const hkaiNavMeshAccessor* mesh, const hkaiAgentTraversalInfo& agentInfo, const hkaiNavMeshEdgePairInfo& edgeInfo ) const;

			/// The bitfield check used by isEnabled
		bool isEnabledByBitfield( const hkaiNavVolumeAccessor* volume, const hkaiAgentTraversalInfo& agentInfo, const hkaiNavVolumeCellPairInfo& edgeInfo ) const;

	public:

			/// Nav mesh edge filtering: 2D bitfield
		HK_ALIGN16( hkUint32 m_edgeMaskLookupTable[32] );

			/// Nav volume edge filtering: 1D bitfield
		HK_ALIGN16( hkUint32 m_cellMaskLookupTable );
};

#endif // HK_AI_DEFAULT_NAV_MESH_EDGE_FILTER_H

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
