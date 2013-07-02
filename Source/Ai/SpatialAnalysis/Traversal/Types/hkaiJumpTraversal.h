/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_JUMP_TRAVERSAL_H
#define HKAI_JUMP_TRAVERSAL_H

#include <Ai/SpatialAnalysis/Traversal/hkaiTraversalAnalyzer.h>

struct hkaiTraversalAnalysisContext;
class hkaiTraversalAnnotationLibrary;

class hkaiJumpAnalyzer : public hkaiTraversalAnalyzer
{
public:
	//+version(0)
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL);
	HK_DECLARE_REFLECTION();

	hkaiJumpAnalyzer() { }
	hkaiJumpAnalyzer(hkFinishLoadedObjectFlag f)
		: hkaiTraversalAnalyzer(f)
	{ }

	/// The maximum horizontal distance between the two sides of the gap.
	/// Must be greater than 0.
	hkReal m_maxHorizontalDistance;

	/// The maximum upward height difference between the starting and ending
	/// surfaces.
	hkReal m_maxUpHeight;

	/// The maximum downward height difference between the starting and ending
	/// surfaces.
	hkReal m_maxDownHeight;

	/// The height of the jump. This is added to the character height, and the
	/// airspace extending to the resultant height must be clear of geometry in
	/// order for the jump to be successful.
	hkReal m_verticalApex;

	virtual void analyze(hkaiTraversalAnalysisContext & context) const HK_OVERRIDE;
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
