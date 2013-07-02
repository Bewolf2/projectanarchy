/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifdef HK_PLATFORM_SPU
#	include <Ai/Pathfinding/Multithreaded/Spu/hkaiSpuCommon.h>
#	include <Ai/Pathfinding/Multithreaded/Spu/hkaiSpuPathfindingConfig.h>
#	include <Common/Base/Memory/PlatformUtils/Spu/SpuDmaCache/hkSpu4WayCache.h>
#endif

//
// General Access
//
#ifndef HK_PLATFORM_SPU

inline void hkaiStreamingCollection::initNoRefCount( hkaiNavMeshInstance* meshInstance, hkaiDirectedGraphInstance* clusterGraph /*= HK_NULL*/, const hkaiNavMeshQueryMediator* mediator /*= HK_NULL*/)
{
	m_isTemporary = true;

	m_instances.setSize(1);
	m_instances[0].m_instancePtr = meshInstance;
	m_instances[0].m_clusterGraphInstance = clusterGraph;
	m_instances[0].m_mediator = mediator;
}

inline void hkaiStreamingCollection::initNoRefCount( hkaiDirectedGraphInstance* clusterGraph )
{
	m_isTemporary = true;

	m_instances.setSize(1);
	m_instances[0].m_clusterGraphInstance = clusterGraph;
}

inline void hkaiStreamingCollection::initNoRefCount( hkaiNavVolumeInstance* vol, const hkaiNavVolumeMediator* mediator )
{
	m_isTemporary = true;

	m_instances.setSize(1);
	m_instances[0].m_volumeInstancePtr = vol;
	m_instances[0].m_volumeMediator = mediator;
}


#endif //!SPU

//
// Face Access
//

inline const hkaiNavMesh::Face& hkaiStreamingCollection::getFaceFromPacked( hkaiPackedKey faceKey ) const
{
	hkaiRuntimeIndex sectionId = hkaiGetRuntimeIdFromPacked(faceKey);
	hkaiNavMesh::FaceIndex faceIndex = hkaiGetIndexFromPacked(faceKey);

	return m_instances[sectionId].m_instancePtr->getFace(faceIndex);
}

inline hkaiNavMesh::Face& hkaiStreamingCollection::getWritableFaceFromPacked( hkaiPackedKey faceKey, hkBool32 instanceIfNeeded /* = false*/)
{
	hkaiRuntimeIndex sectionId = hkaiGetRuntimeIdFromPacked(faceKey);
	hkaiNavMesh::FaceIndex faceIndex = hkaiGetIndexFromPacked(faceKey);

	return *m_instances[sectionId].m_instancePtr->getWritableFace(faceIndex, instanceIfNeeded);
}



//
// Edge Access
//

inline const hkaiNavMesh::Edge& hkaiStreamingCollection::getEdgeFromPacked( hkaiPackedKey edgeKey ) const
{
	hkaiRuntimeIndex sectionId = hkaiGetRuntimeIdFromPacked(edgeKey);
	hkaiNavMesh::EdgeIndex edgeIndex = hkaiGetIndexFromPacked(edgeKey);

	return m_instances[sectionId].m_instancePtr->getEdge(edgeIndex);
}

inline hkaiNavMesh::Edge& hkaiStreamingCollection::getWritableEdgeFromPacked( hkaiPackedKey edgeKey, hkBool32 instanceIfNeeded /* = false*/ )
{
	hkaiRuntimeIndex sectionId = hkaiGetRuntimeIdFromPacked(edgeKey);
	hkaiNavMesh::EdgeIndex edgeIndex = hkaiGetIndexFromPacked(edgeKey);

	return *m_instances[sectionId].m_instancePtr->getWritableEdge(edgeIndex, instanceIfNeeded);
}

//
// Face data access
// 

inline const hkaiNavMesh::FaceData* hkaiStreamingCollection::getFaceDataPtrFromPacked(hkaiPackedKey faceKey) const
{
	hkaiRuntimeIndex sectionId = hkaiGetRuntimeIdFromPacked(faceKey);
	hkaiNavMesh::FaceIndex faceIndex = hkaiGetIndexFromPacked(faceKey);

	return m_instances[sectionId].m_instancePtr->getFaceDataPtr(faceIndex);
}

inline hkaiNavMesh::FaceData* hkaiStreamingCollection::getWritableFaceDataPtrFromPacked(hkaiPackedKey faceKey, hkBool32 instanceIfNeeded)
{
	hkaiRuntimeIndex sectionId = hkaiGetRuntimeIdFromPacked(faceKey);
	hkaiNavMesh::FaceIndex faceIndex = hkaiGetIndexFromPacked(faceKey);

	return m_instances[sectionId].m_instancePtr->getWritableFaceDataPtr(faceIndex, instanceIfNeeded);
}

//
// Edge data access
// 

inline const hkaiNavMesh::EdgeData* hkaiStreamingCollection::getEdgeDataPtrFromPacked(hkaiPackedKey edgeKey) const
{
	hkaiRuntimeIndex sectionId = hkaiGetRuntimeIdFromPacked(edgeKey);
	hkaiNavMesh::EdgeIndex edgeIndex = hkaiGetIndexFromPacked(edgeKey);

	return m_instances[sectionId].m_instancePtr->getEdgeDataPtr(edgeIndex);
}

inline hkaiNavMesh::EdgeData* hkaiStreamingCollection::getWritableEdgeDataPtrFromPacked(hkaiPackedKey edgeKey, hkBool32 instanceIfNeeded)
{
	hkaiRuntimeIndex sectionId = hkaiGetRuntimeIdFromPacked(edgeKey);
	hkaiNavMesh::EdgeIndex edgeIndex = hkaiGetIndexFromPacked(edgeKey);

	return m_instances[sectionId].m_instancePtr->getWritableEdgeDataPtr(edgeIndex, instanceIfNeeded);
}

inline const hkaiNavMeshInstance* hkaiStreamingCollection::getNavMeshInstancePtrFromMainMem( const InstanceInfo* infoMainMem, hkaiRuntimeIndex sectionId )
{
#ifdef HK_PLATFORM_SPU
 	const InstanceInfo* cpuInstanceInfo = (infoMainMem + sectionId);
 	HKAI_SPU_LOCAL_COPY( InstanceInfo, cpuInstanceInfo, spuInstanceInfo);
	return spuInstanceInfo->m_instancePtr;
#else
	return infoMainMem[sectionId].m_instancePtr;
#endif
}

inline const hkaiDirectedGraphInstance* hkaiStreamingCollection::getGraphInstancePtrFromMainMem( const InstanceInfo* infoMainMem, hkaiRuntimeIndex sectionId )
{
#ifdef HK_PLATFORM_SPU
	const InstanceInfo* cpuInstanceInfo = (infoMainMem + sectionId);
	HKAI_SPU_LOCAL_COPY( InstanceInfo, cpuInstanceInfo, spuInstanceInfo);
	return spuInstanceInfo->m_clusterGraphInstance;
#else
	return infoMainMem[sectionId].m_clusterGraphInstance;
#endif
}

inline const hkaiNavVolumeInstance* hkaiStreamingCollection::getVolumeInstancePtrFromMainMem( const InstanceInfo* infoMainMem, hkaiRuntimeIndex sectionId )
{
#ifdef HK_PLATFORM_SPU
	const InstanceInfo* cpuInstanceInfo = (infoMainMem + sectionId);
	HKAI_SPU_LOCAL_COPY( InstanceInfo, cpuInstanceInfo, spuInstanceInfo);
	return spuInstanceInfo->m_volumeInstancePtr;
#else
	return infoMainMem[sectionId].m_volumeInstancePtr;
#endif
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
