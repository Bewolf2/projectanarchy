/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_CALLBACK_UTILS_H
#define HKAI_CALLBACK_UTILS_H

#include <Ai/Internal/NavMesh/hkaiNavMeshGenerationProgressCallback.h>

class hkaiNavMesh;

	/// Simple utilties for generating status updates during nav mesh generation and simplification
class hkaiCallbackUtils
{
	//+hk.MemoryTracker(ignore=True)
	public:

		static hkReal HK_CALL getProgressPercentage( int step, int subStep = 0, int numSubSteps = 1);
		static bool HK_CALL shouldProcessMiddleStep( int subStep, int numSubSteps );

		static const char* HK_CALL getNavMeshGenerationStatusString(hkaiNavMeshGenerationProgressCallback::NavMeshGenerationStep step);
		static const char* HK_CALL getNavMeshGenerationReportString(hkaiNavMeshGenerationProgressCallback::NavMeshGenerationStep step);

		static const char* s_navMeshGenerationStatusStrings[];
		static const char* s_navMeshGenerationReportStrings[];

};


#endif // HKAI_CALLBACK_UTILS_H

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
