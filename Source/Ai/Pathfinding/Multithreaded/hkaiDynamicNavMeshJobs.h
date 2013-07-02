/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_DYNAMIC_NAVMESH_JOBS_H
#define HK_AI_DYNAMIC_NAVMESH_JOBS_H

#include <Common/Base/Thread/JobQueue/hkJobQueue.h>

#include <Ai/Pathfinding/hkaiBaseTypes.h>
#include <Ai/Pathfinding/Dynamic/Partitioning/hkaiSilhouetteGeneratorSectionContext.h>

class hkaiNavMeshInstance;

/// Jobs for computing shortest paths on directed graphs and nav meshes.
struct hkaiDynamicNavMeshJob : public hkJob
{
	public:

		enum JobSubType
		{
			JOB_DYNAMIC_NAVMESH_CUT_FACE,
			JOB_DYNAMIC_NAVMESH_END
		};

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, hkaiDynamicNavMeshJob );

		HK_FORCE_INLINE hkaiDynamicNavMeshJob( JobSubType type, hkUint16 size )
			:	hkJob( HK_JOB_TYPE_AI_DYNAMIC, type, size),
				m_semaphore(HK_NULL),
				m_jobDoneFlag(HK_NULL)
		{
		}

		/// Called at the end of the job
		/// m_semaphore will be released if non-NULL, and the m_jobDoneFlag will be incremented of non-NULL
		inline void atomicIncrementAndReleaseSemaphore() const;

		/// This semaphore is released once the original job (and all its spawned children) has finished.
		hkSemaphoreBusyWait* m_semaphore;

		/// The variable at this location will be incremented (atomically) when the job is done.
		hkUint32* m_jobDoneFlag;

		
};

	/// A job to cut a single nav mesh face and create the re-triangulated results.
struct hkaiNavMeshCutFaceJob : public hkaiDynamicNavMeshJob
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, hkaiNavMeshCutFaceJob );

	HK_FORCE_INLINE hkaiNavMeshCutFaceJob( );

	enum
	{
		
		MAX_MATERIALS = 14,
	};

		/// Pointer to nav mesh instance (in main memory)
	const hkaiNavMeshInstance* m_meshInstance;
		/// Key of the face to be cut
	hkaiPackedKey m_faceKey;

	const hkaiSilhouetteGeneratorSectionContext* const* m_generatorContexts;
	int m_numGenerators;

		/// Index of the first non-cutting material.
		/// Any generator before this will be treated as cutting even if it has an non-cutting material type.
		/// This can happen in cases where we have too many distinct materials and have to force some to be cutting.
		/// In order to reduce the impact of artifacts from triangulation, we move all the cutting silhouettes to
		/// the front of the array.
	int m_firstNonCuttingGeneratorIndex;

	hkVector4 m_worldUp;
	hkReal m_extrusionAmount;
	hkReal m_radiusExpansionAmount;

	hkBool m_isWallClimbing; 

	// location of hkaiFaceCutResults
	struct hkaiFaceCutResults* m_results;
	
	const struct hkaiSilhouetteGenerationParameters* m_generationParams;

	
};

#include <Ai/Pathfinding/Multithreaded/hkaiDynamicNavMeshJobs.inl>

#endif // HK_AI_DYNAMIC_NAVMESH_JOBS_H

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
