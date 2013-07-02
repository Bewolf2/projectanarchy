/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_LOCAL_STEERING_JOBS_H
#define HK_AI_LOCAL_STEERING_JOBS_H

#include <Common/Base/Thread/JobQueue/hkJobQueue.h>

#include <Ai/Pathfinding/Character/LocalSteering/hkaiAvoidanceSolver.h>
#include <Ai/Pathfinding/NavMesh/Streaming/hkaiStreamingCollection.h>
#include <Ai/Pathfinding/Character/LocalSteering/hkaiLocalSteeringOutput.h>



/// Jobs for computing shortest paths on directed graphs and nav meshes.
struct hkaiLocalSteeringJob : public hkJob
{
	public:

		enum JobSubType
		{
			JOB_LOCAL_STEERING_AVOIDANCE_SOLVE,
			JOB_LOCAL_STEERING_CALC_VELOCITIES,
			JOB_LOCAL_STEERING_END
		};

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, hkaiLocalSteeringJob );

		HK_FORCE_INLINE hkaiLocalSteeringJob( JobSubType type, hkUint16 size )
			:	hkJob( HK_JOB_TYPE_AI_LOCAL_STEERING, type, size),
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

	/// A job to call hkaiBehavior::calcVelocities on a list of hkaiBehaviors
struct hkaiBehaviorCalcVelocitiesJob : public hkaiLocalSteeringJob
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, hkaiBehaviorCalcVelocitiesJob );

	HK_FORCE_INLINE hkaiBehaviorCalcVelocitiesJob();

		/// Pointer to an array of hkaiBehavior pointers to be processed
	class hkaiBehavior** m_behaviors;
	int m_numBehaviors;

		/// Pointer to an array of local steering input structs to hold the outputs.
		/// This array should be sized to hold one output per character managed by the processed behaviors
	hkaiLocalSteeringInput* m_outputs;
	int m_numOutputs;

	hkReal m_timestep;
};

	/// Avoidance solving information for a single character.
struct hkaiAvoidanceSolverCommand
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, hkaiAvoidanceSolverCommand );

	HK_FORCE_INLINE hkaiAvoidanceSolverCommand( ) {}

	hkaiLocalSteeringInput m_localSteeringInput;

	const class hkaiCharacter** m_nearbyCharacters;
	const class hkaiObstacleGenerator** m_obstacleGenerators;

	int m_numNearbyCharacters;
	int m_numObstacleGenerators;
	hkaiPackedKey m_currentNavMeshFaceKey;

	hkaiLocalSteeringOutput* m_output;
};

	/// Avoidance solver job consisting of several hkaiAvoidanceSolverCommands
struct hkaiAvoidanceSolverJob : public hkaiLocalSteeringJob
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, hkaiAvoidanceSolverJob );

	HK_FORCE_INLINE hkaiAvoidanceSolverJob( );

	// Inputs
	hkReal m_timestep;
	hkReal m_erosionRadius;
	const hkaiStreamingCollection::InstanceInfo* m_sectionInfo;
	
	const hkaiAvoidancePairProperties::PairData*	m_avoidancePairProps;
	int												m_numAvoidancePairProps;

	hkaiAvoidanceSolverCommand* m_commands;
	int							m_numCommands;


};

#include <Ai/Pathfinding/Multithreaded/hkaiLocalSteeringJobs.inl>

#endif // HK_AI_LOCAL_STEERING_JOBS_H

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
