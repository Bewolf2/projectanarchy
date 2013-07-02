/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_JOB_QUEUE_UTILS
#define HKAI_JOB_QUEUE_UTILS

#include <Common/Base/Thread/JobQueue/hkJobQueue.h>

	/// Helper class for registering with Havok's job queue system
class hkaiJobQueueUtils
{
public:
	enum RegistrationFlags
	{
		REGISTER_PATHFINDING			= 1 << HK_JOB_TYPE_AI_PATHFINDING,
		REGISTER_VOLUME_PATHFINDING		= 1 << HK_JOB_TYPE_AI_VOLUME_PATHFINDING,
		REGISTER_DYNAMIC				= 1 << HK_JOB_TYPE_AI_DYNAMIC,
		REGISTER_LOCAL_STEERING			= 1 << HK_JOB_TYPE_AI_LOCAL_STEERING,
		REGISTER_GENERATION				= 1 << HK_JOB_TYPE_AI_GENERATION,

		REGISTER_ALL = ( REGISTER_PATHFINDING	| REGISTER_VOLUME_PATHFINDING | REGISTER_DYNAMIC
												| REGISTER_LOCAL_STEERING	| REGISTER_GENERATION),
	};

	static void HK_CALL registerWithJobQueue( class hkJobQueue* jobQueue, RegistrationFlags flags = REGISTER_ALL);
};


#endif // HKAI_JOB_QUEUE_UTILS

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
