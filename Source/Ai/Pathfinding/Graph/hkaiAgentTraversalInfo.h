/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_NAV_MESH_TRAVERSAL_INFO_H
#define HKAI_NAV_MESH_TRAVERSAL_INFO_H

/// Description of a character or object for use during A* searches.
/// Using different hkaiNavMeshTraversalInfo for different game entities allows you to produce different paths
/// using a single nav mesh. For example, tank units might too large to pass through thin faces, and weak soldier units
/// might not be allowed to cross certain edges.
struct hkaiAgentTraversalInfo
{
	//+version(1)
	HK_DECLARE_REFLECTION();

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH, hkaiAgentTraversalInfo);

	hkaiAgentTraversalInfo()
	{
		init();
	}

	hkaiAgentTraversalInfo(hkFinishLoadedObjectFlag f)
	{
	}

	~hkaiAgentTraversalInfo() {}

	inline void init()
	{
		m_diameter = 0.0f;
		m_filterInfo = 0;
	}

		/// Object diameter.
		/// Used to reject potential paths in the graph if the diameter is too large to pass through a face.
	hkReal m_diameter;

		/// Filter group information, passed on to an hkaiAstarCostModifier during the search.
		/// See the NavMeshEdgeFilterDemo for an example of how to allow/reject edges based on this.
	hkUint32 m_filterInfo;
};

#endif // HKAI_NAV_MESH_TRAVERSAL_INFO_H

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
