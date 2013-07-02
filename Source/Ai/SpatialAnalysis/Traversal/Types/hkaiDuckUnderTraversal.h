/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_DUCK_UNDER_TRAVERSAL_H
#define HKAI_DUCK_UNDER_TRAVERSAL_H

#include <Ai/SpatialAnalysis/Traversal/hkaiTraversalAnalyzer.h>
#include <Ai/SpatialAnalysis/Traversal/hkaiTraversalAnnotationLibrary.h>

struct hkaiTraversalAnalysisContext;
class hkaiTraversalAnnotationLibrary;

class hkaiDuckUnderAnalyzer : public hkaiTraversalAnalyzer
{
public:
	//+version(0)
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL);
	HK_DECLARE_REFLECTION();

	hkaiDuckUnderAnalyzer() { }
	hkaiDuckUnderAnalyzer(hkFinishLoadedObjectFlag f)
		: hkaiTraversalAnalyzer(f)
	{ }

	/// The minimum horizontal distance between the two surfaces. Must be 
	/// greater than 0.
	hkReal m_maxHorizontalDistance;
	
	/// The minimum vertical clearance above the ground connecting the two
	/// surfaces. Must be greater than 0.
	hkReal m_minClearance;

	/// The maximum vertical clearance above the ground connecting the two
	/// surfaces. Must be greater than m_minClearance.
	hkReal m_maxClearance;

	/// The maximum vertical height difference between the two surfaces.
	hkReal m_maxHeightDifference;

	virtual void analyze(hkaiTraversalAnalysisContext & context) const HK_OVERRIDE;
};


class hkaiDuckUnderUtils
{
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL, hkaiDuckUnderUtils);

	static hkBool32 HK_CALL getClearance(
		hkaiTraversalAnnotationLibrary const& library,
		hkaiTraversalAnnotationLibrary::Index& annotationIndex,
		hkVector4Parameter startPoint,
		hkReal & clearanceOut);

	static int HK_CALL getNumClearanceIntervals(	
		hkaiTraversalAnnotationLibrary const& library,
		hkaiTraversalAnnotationLibrary::Index& annotationIndex);

	static void HK_CALL getClearanceInterval(
		hkaiTraversalAnnotationLibrary const& library,
		hkaiTraversalAnnotationLibrary::Index& annotationIndex,
		int clearanceIntervalIndex,
		hkVector4 & leftStartPointOut,
		hkVector4 & rightStartPointOut,
		hkVector4 & leftEndPointOut,
		hkVector4 & rightEndPointOut);
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
