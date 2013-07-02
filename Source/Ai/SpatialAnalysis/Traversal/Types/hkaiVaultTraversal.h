/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_VAULT_TRAVERSAL_H
#define HKAI_VAULT_TRAVERSAL_H

#include <Ai/SpatialAnalysis/Traversal/hkaiTraversalAnalyzer.h>
#include <Ai/SpatialAnalysis/Traversal/hkaiTraversalAnnotationLibrary.h>

struct hkaiTraversalAnalysisContext;
class hkaiTraversalAnnotationLibrary;

class hkaiVaultAnalyzer : public hkaiTraversalAnalyzer
{
public:
	//+version(0)
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL);
	HK_DECLARE_REFLECTION();

	hkaiVaultAnalyzer() { }
	hkaiVaultAnalyzer(hkFinishLoadedObjectFlag f)
		: hkaiTraversalAnalyzer(f)
	{ }

	/// The minimum horizontal distance between the two sides of the vault.
	/// Must be greater than 0.
	hkReal m_minWallWidth;

	/// The maximum horizontal distance between the two sides of the vault.
	/// Must be greater than m_minWallWidth.
	hkReal m_maxWallWidth;

	/// The minimum height of the wall separating the two surfaces, measured
	/// relative to the height of the starting surface. Must be greater than 0.
	hkReal m_minWallHeight;

	/// The maximum height of the wall separating the two surfaces, measured
	/// relative to the height of the starting surface. Must be greater than
	/// m_minWallHeight.
	hkReal m_maxWallHeight;

	/// The maximum upward height difference between the starting and ending
	/// surfaces.
	hkReal m_maxUpHeight;

	/// The maximum downward height difference between the starting and ending
	/// surfaces.
	hkReal m_maxDownHeight;

	/// The vertical distance above the vault, measured relative to the top of
	/// the wall, which must be unoccluded.
	hkReal m_verticalApex;

	/// The angle to use when extracting a hand plant profile for the climb, in
	/// radians. Should be between -pi/2 and pi/2. Higher magnitude represents
	/// a more nearly vertical handplant, with 0 representing a handplant 
	/// straight downward onto the top of the wall. Positive angles represent
	/// handplants on the side of the wall closer to the beginning of the 
	/// vault.
	hkReal m_handPlantAngle;

	virtual void analyze(hkaiTraversalAnalysisContext & context) const HK_OVERRIDE;
};

class hkaiVaultUtils
{
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL, hkaiVaultUtils);

	static hkBool32 HK_CALL getHandPlantPoint(
		hkaiTraversalAnnotationLibrary const& library,
		hkaiTraversalAnnotationLibrary::Index& annotationIndex,
		hkVector4Parameter handPos,
		hkVector4 & handPlantPointOut,
		hkVector4 & handPlantFwdDirOut);

	static int HK_CALL getNumHandPlantSegments(	
		hkaiTraversalAnnotationLibrary const& library,
		hkaiTraversalAnnotationLibrary::Index& annotationIndex);

	static void HK_CALL getHandPlantSegment(
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
