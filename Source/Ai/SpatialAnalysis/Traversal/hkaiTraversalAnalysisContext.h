/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_TRAVERSAL_ANALYSIS_CONTEXT_H
#define HKAI_TRAVERSAL_ANALYSIS_CONTEXT_H

struct hkaiTraversalAnalysisSettings;
class hkaiNavMeshInstance;
class hkaiNavMeshInstanceSegmentCaster;
class hkaiGeometrySegmentCaster;
class hkaiPairedEdgeFinder;
class hkaiTraversalAnnotationLibrary;

struct hkaiTraversalAnalysisContext
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL, hkaiTraversalAnalysisContext);

	hkaiTraversalAnalysisContext();
	~hkaiTraversalAnalysisContext();

	hkaiTraversalAnalysisSettings const* m_settings;

	hkaiNavMeshInstance const* m_fromNavMeshInstance;
	hkaiNavMeshInstance const* m_toNavMeshInstance;

	hkRefPtr<hkaiPairedEdgeFinder> m_pairedEdgeFinder;
	hkRefPtr<hkaiGeometrySegmentCaster> m_walkableGeometrySegmentCaster;
	hkRefPtr<hkaiGeometrySegmentCaster> m_cuttingGeometrySegmentCaster;

	hkRefPtr<hkaiNavMeshInstanceSegmentCaster> m_fromNavMeshInstanceSegmentCaster;
	hkRefPtr<hkaiNavMeshInstanceSegmentCaster> m_toNavMeshInstanceSegmentCaster;

	hkaiTraversalAnnotationLibrary * m_outputLibrary;
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
