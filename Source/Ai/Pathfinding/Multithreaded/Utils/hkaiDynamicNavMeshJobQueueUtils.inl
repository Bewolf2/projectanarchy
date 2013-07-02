/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_PLATFORM_SPU

#include <Ai/Pathfinding/Multithreaded/Jobs/Dynamic/hkaiNavMeshCutFaceJob.h>
#include <Ai/Pathfinding/Dynamic/Silhouette/SilhouetteGenerator/PointCloud/hkaiPointCloudSilhouetteGenerator.h>

static hkJobQueue::ProcessJobFunc s_dynamicNavMeshProcessFuncs[hkaiDynamicNavMeshJob::JOB_DYNAMIC_NAVMESH_END];

inline void HK_CALL hkaiDynamicNavMeshJobQueueUtils::registerWithJobQueue(hkJobQueue* queue)
{
#ifdef HK_PLATFORM_MULTI_THREAD
	hkJobQueue::hkJobHandlerFuncs jobHandlerFuncs;
	jobHandlerFuncs.m_popJobFunc	= hkaiDynamicNavMeshJobQueueUtils::popAiJob;
	jobHandlerFuncs.m_finishJobFunc = hkaiDynamicNavMeshJobQueueUtils::finishAiJob;

	jobHandlerFuncs.initProcessJobFuncs( s_dynamicNavMeshProcessFuncs, HK_COUNT_OF(s_dynamicNavMeshProcessFuncs) );

	jobHandlerFuncs.registerProcessJobFunc(hkaiDynamicNavMeshJob::JOB_DYNAMIC_NAVMESH_CUT_FACE, hkaiProcessNavMeshCutFaceJob);

	queue->registerJobHandler( HK_JOB_TYPE_AI_DYNAMIC, jobHandlerFuncs );

	// Register generate/willgenerate/getaabb. These will dispatch to virtual function anyway
	hkaiPointCloudSilhouetteGenerator::registerMtFunctions();

#	if defined(HK_PLATFORM_HAS_SPU)
#		ifdef HK_PLATFORM_PS3_PPU
			extern char _binary_hkaiSpursDynamicNavMesh_elf_start[];
			void* elf =	_binary_hkaiSpursDynamicNavMesh_elf_start;
#		else // Win32 SPU Simulator
			void* elf = (void*)HK_JOB_TYPE_AI_DYNAMIC;
#		endif
		queue->registerSpuElf( HK_JOB_TYPE_AI_DYNAMIC, elf );
#	endif
#endif

}
#endif // HK_PLATFORM_SPU

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
