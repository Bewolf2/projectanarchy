/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_TRAVERSAL_ANALYSIS_SETTINGS_H
#define HKAI_TRAVERSAL_ANALYSIS_SETTINGS_H

#include <Ai/Pathfinding/NavMesh/hkaiNavMesh.h>

struct hkaiTraversalAnalysisSettings : public hkReferencedObject
{
public:
	//+version(0)
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH);
	HK_DECLARE_REFLECTION();

	hkaiTraversalAnalysisSettings();
	hkaiTraversalAnalysisSettings(hkFinishLoadedObjectFlag f);
	~hkaiTraversalAnalysisSettings();

	/// The up-vector used for orienting trajectories.
	hkVector4 m_up; //+default(0,0,1)

	/// Matched edges must be at least this long to be considered.
	hkReal m_minEdgeLength; //+default(0.1f)

	/// Different sections will only be checked against each other if their AABBs are within this distance.
	hkReal m_maxSectionDistance; //+default(5.0f)

	hkReal m_characterHeight; //+default(2.0f);

	hkReal m_characterRadius; //+default(0.2f);

	/// The maximum height to raise a traversal's starting or ending edge to avoid intersections with geometry.
	hkReal m_raiseEdgeHeightLimit; //+default(1.0f)
};

#endif

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
