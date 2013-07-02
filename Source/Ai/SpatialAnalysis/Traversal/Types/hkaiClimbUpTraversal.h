/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_CLIMB_UP_TRAVERSAL_H
#define HKAI_CLIMB_UP_TRAVERSAL_H

#include <Ai/SpatialAnalysis/Traversal/hkaiTraversalAnalyzer.h>
#include <Ai/SpatialAnalysis/Traversal/hkaiTraversalAnnotationLibrary.h>

struct hkaiTraversalAnalysisContext;
class hkaiTraversalAnnotationLibrary;

class hkaiClimbUpAnalyzer : public hkaiTraversalAnalyzer
{
public:
	//+version(0)
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL);
	HK_DECLARE_REFLECTION();

	hkaiClimbUpAnalyzer() { }
	hkaiClimbUpAnalyzer(hkFinishLoadedObjectFlag f)
		: hkaiTraversalAnalyzer(f)
	{ }

	/// The maximum horizontal gap between the edge of the lower surface and the
	/// edge of the upper surface.
	hkReal m_maxUnderhang;

	/// The minimum height difference between the lower and upper surfaces.
	/// Must be greater than zero.
	hkReal m_minUpHeight;

	/// The maximum height difference between the lower and upper surfaces.
	hkReal m_maxUpHeight;

	/// The angle to use when extracting a grab profile for the climb, in
	/// radians. Should be between 0 and pi/2, with a higher angle representing
	/// a more nearly vertical grab.
	hkReal m_grabAngle;

	/// The extents of the bounding volume to search for a grab profile. Should
	/// be greater than m_minUpHeight, and less than the character height.
	hkReal m_grabScanDepth;

	virtual void analyze(hkaiTraversalAnalysisContext & context) const HK_OVERRIDE;
};

class hkaiClimbUpUtils
{
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL, hkaiClimbUpUtils);

	static hkBool32 HK_CALL getGrabPoint(
		hkaiTraversalAnnotationLibrary const& library,
		hkaiTraversalAnnotationLibrary::Index& annotationIndex,
		hkVector4Parameter handPos,
		hkVector4 & grabPointOut,
		hkVector4 & grabFwdDirOut);

	static int HK_CALL getNumGrabSegments(	
		hkaiTraversalAnnotationLibrary const& library,
		hkaiTraversalAnnotationLibrary::Index& annotationIndex);

	static void HK_CALL getGrabSegment(
		hkaiTraversalAnnotationLibrary const& library,
		hkaiTraversalAnnotationLibrary::Index& annotationIndex,
		int handPlantSegmentIndex,
		hkVector4 & leftPointOut,
		hkVector4 & rightPointOut);
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
