/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_NAV_MESH_EDGE_FILTER_H
#define HK_AI_NAV_MESH_EDGE_FILTER_H

#include <Ai/Pathfinding/NavMesh/hkaiNavMeshInstance.h>
#include <Ai/Pathfinding/NavMesh/hkaiNavMeshEdgePairInfo.h>
#include <Ai/Pathfinding/NavVolume/hkaiNavVolumeInstance.h>
#include <Ai/Pathfinding/NavVolume/hkaiNavVolumeCellPairInfo.h>

extern const class hkClass hkaiAstarEdgeFilterClass;

#ifdef HK_PLATFORM_SPU
#	include <Ai/Pathfinding/NavMesh/Spu/hkaiSpuNavMeshAccessor.h>
#	include <Ai/Pathfinding/NavVolume/Spu/hkaiSpuNavVolumeAccessor.h>
#endif


struct hkaiAgentTraversalInfo;

/// The hkaiAstarEdgeFilter can be used to reject edges for traversal during the A* search,
///
/// See hkaiDefaultAstarEdgeFitler for a sample implementation, and NavMeshEdgeFilterDemo
/// for examples of how to use this
class hkaiAstarEdgeFilter : public hkReferencedObject
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);
		HK_DECLARE_REFLECTION();

		enum
		{
			MAX_SIZE_FOR_SPU = 256
		};

		enum EdgeFilterType
		{
			EDGE_FILTER_DEFAULT,
			EDGE_FILTER_USER
		};

		hkEnum<EdgeFilterType, hkUint8> m_type;

		hkaiAstarEdgeFilter(hkFinishLoadedObjectFlag flag) 
			: hkReferencedObject(flag) { }
		explicit hkaiAstarEdgeFilter(EdgeFilterType type = EDGE_FILTER_USER) : m_type(type) { }
		virtual ~hkaiAstarEdgeFilter() {}


			/// Context structure for the isEnabled callback
		struct NavMeshIsEnabledCallbackContext
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI,hkaiAstarEdgeFilter::NavMeshIsEnabledCallbackContext);

			inline NavMeshIsEnabledCallbackContext( const hkaiNavMeshAccessor* meshAccessor, const hkaiAgentTraversalInfo& agentInfo, 
				const hkaiNavMeshEdgePairInfo& edgeFaceInfo, hkUint8 queryTypeFlags );

				/// Accessor for the nav mesh(es)
			const hkaiNavMeshAccessor* const m_meshAccessor;
				/// Traversal info for the A* search
			const hkaiAgentTraversalInfo& m_agentInfo;
				/// Information on the Edge->Face->Edge to check
			const hkaiNavMeshEdgePairInfo& m_edgeFaceInfo;
				/// Flags for the query (see QueryType)
			const hkFlags<hkaiNavMeshEdgePairInfo::QueryType, hkUint8> m_queryTypeFlags;
		};

			/// Returns true if traversal between the agent and the edge is enabled.
		virtual bool isEnabled( const NavMeshIsEnabledCallbackContext& context ) const = 0;

		/// Context structure for the isEnabled callback
		struct NavVolumeIsEnabledCallbackContext
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI,hkaiAstarEdgeFilter::NavVolumeIsEnabledCallbackContext);

			inline NavVolumeIsEnabledCallbackContext( const hkaiNavVolumeAccessor* volumeAccessor, const hkaiAgentTraversalInfo& agentInfo,
				const hkaiNavVolumeCellPairInfo& cellEdgeInfo, hkUint8 queryTypeFlags );

			/// Accessor for the nav volume(s)
			const hkaiNavVolumeAccessor* const m_volumeAccessor;
			/// Traversal info for the A* search
			const hkaiAgentTraversalInfo& m_agentInfo;
			/// Information on the Cell->Edge->Cell to check
			const hkaiNavVolumeCellPairInfo& m_cellEdgeInfo;
			/// Flags for the query (see QueryType)
			const hkFlags<hkaiNavVolumeCellPairInfo::QueryType, hkUint8> m_queryTypeFlags;
		};

		/// Returns true if traversal between the agent and the edge is enabled.
		virtual bool isEnabled( const NavVolumeIsEnabledCallbackContext& context ) const;

		static hkaiAstarEdgeFilter* getFromMainMemory(const hkaiAstarEdgeFilter* costModifier);
};

#include <Ai/Pathfinding/Astar/EdgeFilter/hkaiAstarEdgeFilter.inl>

#endif // HK_AI_NAV_MESH_EDGE_FILTER_H

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
