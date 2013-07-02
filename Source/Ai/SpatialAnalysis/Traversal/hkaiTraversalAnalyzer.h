/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_TRAVERSAL_ANALYZER_H
#define HKAI_TRAVERSAL_ANALYZER_H

#include <Ai/SpatialAnalysis/Traversal/hkaiTraversalType.h>

struct hkaiTraversalAnalysisContext;

class hkaiTraversalAnalyzer : public hkReferencedObject
{
public:
	//+version(0)
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL);
	HK_DECLARE_REFLECTION();

	hkaiTraversalAnalyzer() 
		: m_maxPlanarAngle(HK_REAL_PI/2 - 0.1f)
		, m_maxRelativeSlopeAngle(HK_REAL_PI/2)
		, m_userdata(0)
		, m_baseCost(0)
	{ }
	hkaiTraversalAnalyzer(hkFinishLoadedObjectFlag f)
		: hkReferencedObject(f)
	{ }
	virtual ~hkaiTraversalAnalyzer() { }

	/// The maximum angle in the horizontal plane between the two surface edges.
	/// Should be between 0 and pi/2.
	hkReal m_maxPlanarAngle; //+default(HK_REAL_PI/2 - 0.1f)

	/// The maximum angle in the vertical plane between the two surface edges.
	/// Should be between 0 and pi.
	hkReal m_maxRelativeSlopeAngle; //+default(HK_REAL_PI/2)

	/// User-supplied data to be copied to all jump descriptions built from
	/// this spec. Note that the userdata in the user edge itself will be
	/// different than this userdata.
	hkUint32 m_userdata;

	/// The base traversal cost; any user edge created by the analyzer will 
	/// have at least this cost.
	hkReal m_baseCost;

	virtual void analyze(hkaiTraversalAnalysisContext & context) const = 0;
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
