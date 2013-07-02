/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_NAVIGATION_VOLUME_UTILS_H
#define HKAI_NAVIGATION_VOLUME_UTILS_H

#include <Ai/Pathfinding/NavVolume/hkaiNavVolume.h>
#include <Ai/Pathfinding/NavVolume/hkaiNavVolumeInstance.h>
#include <Common/Base/Math/Vector/hkIntVector.h>
#ifdef HK_PLATFORM_SPU
#include <Ai/Pathfinding/NavVolume/Spu/hkaiSpuNavVolumeAccessor.h>
#endif

class hkPseudoRandomGenerator;

/// Miscellaneous utility functions for nav volumes
class hkaiNavVolumeUtils
{
	public:
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, hkaiNavVolumeUtils);

		//
		// Computation methods
		//

			/// Computes the "overlap" region between 2 AABBs, assuming that they're touching
		inline static void HK_CALL getPortalBetweenCellAabbs( const hkAabb& aabb1, const hkAabb& aabb2, hkAabb& portalOut );

			/// Get the AABB of the cell in world space
		inline static void HK_CALL getCellWorldAabb(const hkaiNavVolumeAccessor& volume, int index, hkAabb& aabbOut);
		inline static void HK_CALL getCellWorldAabb(const hkaiNavVolumeAccessor& volume, const hkaiNavVolume::Cell& cell, hkAabb& aabbOut);

		inline static void HK_CALL getCellWorldAabb(const hkaiNavVolume& volume, int index, hkAabb& aabbOut);
		inline static void HK_CALL getCellWorldAabb(const hkaiNavVolume& volume, const hkaiNavVolume::Cell& cell, hkAabb& aabbOut);

		inline static void HK_CALL convertCellToLocalAabb(const hkaiNavVolume::Cell& cell, hkAabb& aabbOut);
		inline static void HK_CALL convertCellToWorldAabb(const hkaiNavVolumeAccessor& volume, const hkaiNavVolume::Cell& cell, hkAabb& aabbOut);

			/// Get the cell center in local space
		inline static void HK_CALL getCellLocalCenter(const hkaiNavVolume& volume, hkaiNavVolume::CellIndex cellIndex, hkVector4& centerOut);
		inline static void HK_CALL getCellLocalCenter(const hkaiNavVolumeAccessor& volume, hkaiNavVolume::CellIndex cellIndex, hkVector4& centerOut);

			/// Given two points and a portal, finds the point in the portal that gives the shortest path between the points
		inline static void HK_CALL getMinimizingPortalPoint( hkVector4Parameter p1, hkVector4Parameter p2, const hkAabb& portal, hkVector4& pointOut );

			/// Given two points and a portal, finds the t-value of the intersection of the ray (p1,p2) in the plane of the portal. Returns false if numerically parallel
		inline static bool HK_CALL getPortalPlaneIntersection( hkVector4Parameter p1, hkVector4Parameter p2, const hkAabb& portal, hkSimdReal& tOut );

#ifndef HK_PLATFORM_SPU
			/// Get a uniformly distributed random point inside a given cell
		static void HK_CALL calcRandomPointInCell( const hkaiNavVolumeInstance& vol, int cellIndex, hkPseudoRandomGenerator& rand, hkVector4& pointOut);

			/// Compute the point int the specified cell that is closest to the specified position
		static void HK_CALL getClosestPointOnCell( const hkaiNavVolume& vol, hkVector4Parameter position, hkaiNavVolume::CellIndex cellIndex, hkVector4& closestPointOut );

			/// Get the volume of a given cell
		static hkSimdReal HK_CALL calcCellVolume(  const hkaiNavVolume& vol, int cellIndex );
		static hkSimdReal HK_CALL calcCellVolume(  const hkaiNavVolumeInstance& vol, int cellIndex );

			/// Removes a set of cell from a nav volume. Other cells connected to cells being removed have edges invalidated.
		static void HK_CALL removeCells( hkaiNavVolume& volume, hkArray<int>::Temp& cellsToRemove );

			/// Compute the connected regions for each cell. Returns the number of regions.
		static int HK_CALL computeRegions( const hkaiNavVolume& volume, hkArrayBase<int>& regionsOut );

			/// Creates an AABB-tree query mediator for the nav volume
		static class hkaiNavVolumeMediator* HK_CALL setupMediator( const hkaiNavVolume& vol );
#endif
			/// In Debug this validates a nav volume is correct. It checks for cell and edge degeneracy and edge consistency (ensure opposite edges are paired correctly).
			/// Call this method to validate any changes made to a nav volume
		static void HK_CALL validate( const hkaiNavVolume& vol );
		static void HK_CALL validate( const hkaiNavVolumeInstance& vol );

};

#include <Ai/Pathfinding/NavVolume/hkaiNavVolumeUtils.inl>

#endif // HKAI_NAVIGATION_VOLUME_UTILS_H

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
