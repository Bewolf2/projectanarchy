/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_SILHOUETTE_GENERATION_PARAMETERS_H
#define HK_SILHOUETTE_GENERATION_PARAMETERS_H

#include <Ai/Pathfinding/Dynamic/Silhouette/SilhouetteReferenceFrame/hkaiSilhouetteReferenceFrame.h>

/// Parameters for generating a convex silhouette with optional expansion radius, corner beveling and hull simplification
struct hkaiSilhouetteGenerationParameters
{
	HK_DECLARE_REFLECTION();
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, hkaiSilhouetteGenerationParameters);

	// Constructor
	hkaiSilhouetteGenerationParameters()
	{
		m_extraExpansion = 0.0f;
		m_bevelThreshold = 0.4f;
		m_simplify2dConvexHullThreshold = 0.005f;
		m_maxSilhouetteSize = 50.0f;
	}

	hkaiSilhouetteGenerationParameters( hkFinishLoadedObjectFlag f) : m_referenceFrame(f) {}

		/// Extra expansion (for eroded nav meshes) . Must be non-negative
	hkReal m_extraExpansion;

		/// Bevel threshold (see docs) . Must be in range [0,1]
	hkReal m_bevelThreshold;

		/// Maximum silhouette extents that is guaranteed to be supported.
		/// The domain used for triangulation is the hkaiNavMesh::Face's AABB expanded by this value.
		/// Increasing this may lead to loss of accuracy for faces created by nav mesh cutting.
	hkReal m_maxSilhouetteSize;

		/// Area-based hull simplification threshold (see docs). Must be >=0
	hkReal m_simplify2dConvexHullThreshold;

		/// Reference frame
	hkaiSilhouetteReferenceFrame m_referenceFrame;
};



//
// hkaiSilhouetteMerger and related classes are no longer used,
// but they are kept here to work around a serialization bug.
//


class hkaiSilhouetteMerger : public hkReferencedObject
{
public:
	// +version(0)
	HK_DECLARE_REFLECTION();
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);

	hkaiSilhouetteMerger() {}
	~hkaiSilhouetteMerger() {}
	hkaiSilhouetteMerger( hkFinishLoadedObjectFlag f): hkReferencedObject(f), m_mergeParams(f) {}

	enum MergeType
	{
		UNUSED_MERGING_SIMPLE,
		UNUSED_MERGING_CONVEX_HULL,
	};

	hkEnum< MergeType, hkUint8 > m_mergeType;

protected:
	hkaiSilhouetteGenerationParameters m_mergeParams;
};

class hkaiSimpleSilhouetteMerger : public hkaiSilhouetteMerger 
{
	// +version(0)
public:
	HK_DECLARE_REFLECTION();
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);

	hkaiSimpleSilhouetteMerger() {}
	~hkaiSimpleSilhouetteMerger() {}
	hkaiSimpleSilhouetteMerger( hkFinishLoadedObjectFlag f): hkaiSilhouetteMerger(f) {}
};

class hkaiConvexHullSilhouetteMerger : public hkaiSilhouetteMerger 
{
	// +version(0)
public:
	HK_DECLARE_REFLECTION();
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);

	hkaiConvexHullSilhouetteMerger() {}
	~hkaiConvexHullSilhouetteMerger() {}
	hkaiConvexHullSilhouetteMerger( hkFinishLoadedObjectFlag f): hkaiSilhouetteMerger(f) {}
};


#endif // HK_SILHOUETTE_GENERATOR_H

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
