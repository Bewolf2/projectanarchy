/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

inline hkaiAstarCostModifier::NavMeshGetModifiedCostCallbackContext::NavMeshGetModifiedCostCallbackContext( 
	const hkaiNavMeshAccessor* meshAccessor, const hkaiAgentTraversalInfo& agentInfo, const hkaiNavMeshEdgePairInfo& edgeFaceInfo, hkUint8 queryTypeFlags, hkSimdRealParameter searchPathCost, hkSimdRealParameter originalCost )
:	m_meshAccessor(meshAccessor),
	m_agentInfo(agentInfo),
	m_edgeFaceInfo(edgeFaceInfo),
	m_queryTypeFlags(queryTypeFlags),
	m_searchPathCost(searchPathCost),
	m_originalCost(originalCost)
{

}

inline hkaiAstarCostModifier::NavVolumeGetModifiedCostCallbackContext::NavVolumeGetModifiedCostCallbackContext( 
	const hkaiNavVolumeAccessor* volumeAccessor, const hkaiAgentTraversalInfo& agentInfo, const hkaiNavVolumeCellPairInfo& cellEdgeInfo, 
	hkUint8 queryTypeFlags, hkSimdRealParameter searchPathCost, hkSimdRealParameter currentCost, 
	hkSimdRealParameter adjacentCost )
:	m_volumeAccessor(volumeAccessor),
	m_agentInfo(agentInfo),
	m_cellEdgeInfo(cellEdgeInfo),
	m_queryTypeFlags(queryTypeFlags),
	m_searchPathCost(searchPathCost),
	m_currentCost(currentCost),
	m_adjacentCost(adjacentCost)
{

}

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
