/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_VOLUME_STREAMING_UTILS_H
#define HKAI_VOLUME_STREAMING_UTILS_H

#include <Ai/Pathfinding/NavVolume/hkaiNavVolume.h>
#include <Common/Base/Algorithm/Collide/1AxisSweep/hk1AxisSweep.h>

class hkAabb;

	/// Miscellaneous utility functions for setting up streaming nav volume sections
class hkaiVolumeStreamingUtils
{
public:
	//+hk.MemoryTracker(ignore=True)

		/// Identifies a pair of adjacent cells between two known nav volumes A and B
	struct CellPair
	{
		hkaiNavVolume::CellIndex m_cellIndexA;
		hkaiNavVolume::CellIndex m_cellIndexB;
	};
	typedef hkArray< CellPair >::Temp CellPairs;

		/// Input structure for hkaiVolumeStreamingUtils::findAdjacentCellPairs
	struct FindAdjacentCellPairsInput
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI, FindAdjacentCellPairsInput);
		FindAdjacentCellPairsInput();

			/// The first volume to be connected.
		hkaiNavVolume* m_volumeA;

			/// The second volume to be connected.
		hkaiNavVolume* m_volumeB;

			/// The UID of the first volume instance.
		hkaiSectionUid m_uidA; 

			/// The UID of the second volume instance.
		hkaiSectionUid m_uidB;

			/// Tolerance value for determining if two planes of the volumes' domains overlap
		hkReal m_domainMatchTolerance;

			/// Which axis matching is allowed on. Specifying -1 (the default) allows matching on any axis
		int m_matchingAxis; //+default( -1 );

			/// Maximum allowed separating in the matching axes direction
		hkReal m_maxSeparation;

			/// Maximum allow penetration in the matching axes diretion
		hkReal m_maxPenetration;

			/// Minimum required overlap in the non-matching axes direction
		hkReal m_minOverlap;

			/// Volume-to-world translation for the first volume
		hkVector4 m_translationA;

			/// Volume-to-world translation for the second volume
		hkVector4 m_translationB;
	};

	
		/// Finds overlaps between an array of AABBs. This should be used to test for potential dependencies between nav volume sections.
		/// Once the potential dependencies are known, the individual volume pairs can be tested against each other
		/// \param[in] aabbs List of nav volume bounding boxes
		/// \param[out] pairs Pairs of indices of overlapping AABBs. These indices are indices into the aabbs array
		/// \param[in] tolerance Fudge factor for detecting adjacency between bounding volumes
	static void HK_CALL findPotentialDependencies( const hkArrayBase<hkAabb>& aabbs, hkArray<hkKeyPair>::Temp& pairs, hkReal tolerance = .05f );

		/// For a given pair of nav volumes, find all the pairs of cells that are adjacent between them
	static void HK_CALL findAdjacentCellPairs( const FindAdjacentCellPairsInput& input, CellPairs& cellPairs );

		/// Generate static 1-to-1 streaming information for a pair of hkaiNavVolume's
	static void HK_CALL generateStreamingInfo( const FindAdjacentCellPairsInput& input );

		/// Create external edge connections between two volumes for all of the given pairs of cells.
		/// Also creates streaming dependencies between the two sections
	static void HK_CALL connectCellPairs( hkaiNavVolume& sectionA, hkaiNavVolume& sectionB, hkaiSectionUid uidA, hkaiSectionUid uidB, const CellPairs& cellPairs );

private:
	enum DomainPlane
	{
		NEG_Z = -3,
		NEG_Y = -2,
		NEG_X = -1,
		POS_X = 1,
		POS_Y = 2,
		POS_Z = 3,
	};

		/// Finds all the boundary cells in the mesh that border on the given side of the domain AABB, and appends their integer AABB to the array.
		/// The AabbInt's keys are indices into the volume's cell array
	static void HK_CALL _collectBoundaryCells( const hkaiNavVolume& volume, const DomainPlane domainPlane, hkArray<hk1AxisSweep::AabbInt>::Temp& aabbsInt, hkReal tolerance );

};

#endif // HKAI_VOLUME_STREAMING_UTILS_H

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
