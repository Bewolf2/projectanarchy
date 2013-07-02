/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_NAV_MESH_COST_MODIFIER_H
#define HK_AI_NAV_MESH_COST_MODIFIER_H

#include <Ai/Pathfinding/NavMesh/hkaiNavMeshInstance.h>
#include <Ai/Pathfinding/NavMesh/hkaiNavMeshEdgePairInfo.h>
#include <Ai/Pathfinding/NavVolume/hkaiNavVolumeInstance.h>
#include <Ai/Pathfinding/NavVolume/hkaiNavVolumeCellPairInfo.h>

extern const class hkClass hkaiAstarCostModifierClass;

#ifdef HK_PLATFORM_SPU
#	include <Ai/Pathfinding/NavMesh/Spu/hkaiSpuNavMeshAccessor.h>
#endif


struct hkaiAgentTraversalInfo;

/// The hkaiAstarCostModifier can be used to reject edges for traversal during the A* search,
/// or to modify the cost of going through certain edges or faces.
///
/// See hkaiDefaultAstarCostModifier for a sample implementation, and NavMeshEdgeFilterDemo and CostModificationDemo
/// for examples of how to use this
class hkaiAstarCostModifier : public hkReferencedObject
{
	//+version(3)
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);
		HK_DECLARE_REFLECTION();

		enum
		{
			MAX_SIZE_FOR_SPU = 256
		};

		enum CostModifierType
		{
			COST_MODIFIER_DEFAULT,
			COST_MODIFIER_USER
		};

		hkEnum<CostModifierType, hkUint8> m_type;

		hkaiAstarCostModifier(hkFinishLoadedObjectFlag flag) 
			: hkReferencedObject(flag) { }
		explicit hkaiAstarCostModifier(CostModifierType type = COST_MODIFIER_USER) : m_type(type) { }
		virtual ~hkaiAstarCostModifier() {}

			/// Context structure for the getModifiedCost callback on nav meshes.
		struct NavMeshGetModifiedCostCallbackContext
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI,hkaiAstarCostModifier::NavMeshGetModifiedCostCallbackContext);

			inline NavMeshGetModifiedCostCallbackContext( const hkaiNavMeshAccessor* meshAccessor, const hkaiAgentTraversalInfo& agentInfo, 
				const hkaiNavMeshEdgePairInfo& edgeFaceInfo, hkUint8 queryTypeFlags, hkSimdRealParameter searchPathCost, hkSimdRealParameter originalCost );

				/// Accessor for the nav mesh(es)
			const hkaiNavMeshAccessor* const m_meshAccessor;
				/// Traversal info for the A* search
			const hkaiAgentTraversalInfo& m_agentInfo;
				/// Information on the Edge->Face->Edge to check
			const hkaiNavMeshEdgePairInfo& m_edgeFaceInfo;
				/// Flags for the query (see QueryType)
			const hkFlags<hkaiNavMeshEdgePairInfo::QueryType, hkUint8> m_queryTypeFlags;
				/// Cost of the whole search path up to this edge pair
			const hkSimdReal m_searchPathCost;
				/// Original, unmodified cost of the Edge->Face->Edge link
			const hkSimdReal m_originalCost;
		};

			/// Context structure for the getModifiedCost callback on nav volumes
		struct NavVolumeGetModifiedCostCallbackContext
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI,hkaiAstarCostModifier::NavVolumeGetModifiedCostCallbackContext);

			inline NavVolumeGetModifiedCostCallbackContext( const hkaiNavVolumeAccessor* volumeAccessor, const hkaiAgentTraversalInfo& agentInfo, 
				const hkaiNavVolumeCellPairInfo& cellEdgeInfo, hkUint8 queryTypeFlags, hkSimdRealParameter searchPathCost, hkSimdRealParameter currentCost, hkSimdRealParameter adjacentCost );

				/// Accessor for the nav volume(s)
			const hkaiNavVolumeAccessor* const m_volumeAccessor;
				/// Traversal info for the A* search
			const hkaiAgentTraversalInfo& m_agentInfo;
				/// Information on the Cell->Edge->Cell to check
			const hkaiNavVolumeCellPairInfo& m_cellEdgeInfo;
				/// Flags for the query (see QueryType)
			const hkFlags<hkaiNavVolumeCellPairInfo::QueryType, hkUint8> m_queryTypeFlags;
				/// Cost of the whole search path up to this cell pair
			const hkSimdReal m_searchPathCost;
				/// Euclidean cost from current cell center to the edge
			const hkSimdReal m_currentCost;
				/// Euclidean cost from edge to the adjacent cell center
			const hkSimdReal m_adjacentCost;
		};


			/// Returns the modified cost of walking through the pair of edges (and through the face).
			/// Note that if costs are being changed, line-of-sight checks during A* should be disabled. Leaving
			/// hkaiNavMeshPathSearchParameters::m_lineOfSightFlags to its default value of 
			/// hkaiNavMeshPathSearchParameters::CHECK_LINE_OF_SIGHT_IF_NO_COST_MODIFIER will handle this intelligently.
		virtual hkSimdReal getModifiedCost( const NavMeshGetModifiedCostCallbackContext& context ) const = 0;

			/// Returns the modified cost of walking through the pair of cells (and through the edge).
			/// The input costs are the euclidean distance from the current cell center to the edge, and from the edge to the adjacent cell center.
			/// Note that if costs are being changed, line-of-sight checks during A* should be disabled. Leaving
			/// hkaiNavVolumePathSearchParameters::m_lineOfSightFlags to its default value of 
			/// hkaiNavVolumePathSearchParameters::CHECK_LINE_OF_SIGHT_IF_NO_COST_MODIFIER will handle this intelligently.
		virtual hkSimdReal getModifiedCost( const NavVolumeGetModifiedCostCallbackContext& context ) const;

		static hkaiAstarCostModifier* getFromMainMemory(const hkaiAstarCostModifier* costModifier);
};

#include <Ai/Pathfinding/Astar/CostModifier/hkaiAstarCostModifier.inl>

#endif // HK_AI_NAV_MESH_COST_MODIFIER_H

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
